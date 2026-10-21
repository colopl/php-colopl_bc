/*
  +----------------------------------------------------------------------+
  | COLOPL PHP Backwards Compatibility Extension.                        |
  +----------------------------------------------------------------------+
  | Copyright (c) COLOPL, Inc.                                           |
  +----------------------------------------------------------------------+
  | This source file is subject to the BSD-3-Clause license that is      |
  | bundled with this package in the file LICENSE.                       |
  +----------------------------------------------------------------------+
  | Author: Go Kudo <g-kudo@colopl.co.jp>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "rfc1867.h"
#include "ext/standard/basic_functions.h"

#include "php_colopl_bc_security.h"

#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED
/* Only the types are used: the session globals are reached through the module
 * entry, so there is no link-time dependency on (a possibly shared) ext/session. */
# include "ext/session/php_session.h"
# if PHP_VERSION_ID >= 70000
#  include "zend_smart_str.h"
# else
#  include "ext/standard/php_smart_str.h"
# endif
#endif

/* TSRMLS_* are empty since PHP 7.0 and were removed in PHP 8.0. */
#ifndef TSRMLS_DC
# define TSRMLS_D	void
# define TSRMLS_DC
# define TSRMLS_C
# define TSRMLS_CC
#endif

/*
 * Security backport: session.upload_progress
 *
 * ext/session installs itself into the multipart/form-data parser through the
 * public hook `php_rfc1867_callback` (main/rfc1867.c) at MINIT time. The parser
 * only invokes the hook when it is non-NULL, so detaching it after ext/session
 * has registered disables the whole session.upload_progress code path at the
 * engine level. This cannot be re-enabled from .user.ini / .htaccess / ini_set(),
 * unlike `session.upload_progress.enabled`.
 *
 * MINIT ordering against ext/session is guaranteed by ZEND_MOD_OPTIONAL("session")
 * in the module entry (see colopl_bc.c).
 *
 * Note: the hook is shared. Any other extension that chains into
 * php_rfc1867_callback (e.g. PECL uploadprogress) is detached as well.
 */

/* PHP_INI_SYSTEM: process-wide, so plain statics are fine even under ZTS. */
static int colopl_bc_security_sup_requested = 0;	/* INI value */
static int colopl_bc_security_sup_applied = 0;		/* hook detached by us */
static int colopl_bc_security_sup_had_callback = 0;	/* a hook was registered when we detached it */

/*
 * Security backport: session.upload_progress cached pointers
 *
 * php_session_rfc1867_callback() (ext/session/session.c) keeps raw zval
 * pointers into the progress array it exports to $_SESSION:
 *
 *   post_bytes_processed         -> data["bytes_processed"]
 *   files                        -> data["files"]          (not refcounted)
 *   current_file                 -> data["files"][n]       (not refcounted)
 *   current_file_bytes_processed -> data["files"][n]["bytes_processed"]
 *
 * and writes through them on every FILE_DATA / FILE_END / END event. They are
 * only resolved once (per upload, resp. per file) while the array is shared with
 * the session and torn down / separated by every update() cycle, so they can be
 * left pointing at a copy that is no longer the one exported: on PHP 7.2+,
 * MULTIPART_EVENT_END with session.upload_progress.cleanup=0 separates `data`
 * and then stores the final byte count through the old pointer, i.e. into the
 * copy that the following update() frees.
 *
 * The hardening wraps the hook installed by ext/session and, before each event
 * that dereferences the cached pointers, rebinds all of them to the array tree
 * the progress record owns (`data`, which is always refcounted). On END with
 * cleanup=0 it separates `data` first, so ext/session's own SEPARATE_ARRAY() is
 * a no-op and the final store lands in the array that is saved. If the tree does
 * not have the layout ext/session builds, progress tracking is dropped for that
 * upload (the progress key is released) instead of following any cached pointer.
 *
 * The wrapper never changes the event, its data or the return value, and it
 * keeps calling the next hook in the chain for every event. On valid input the
 * rebound pointers are the ones ext/session already holds, so the only
 * observable difference is that, with cleanup=0, the final bytes_processed is
 * the total that was read (as on PHP 5.4 - 7.1) instead of a stale value.
 *
 * The same wrapper also closes two logic gaps of the hook that public PoC code
 * relies on, without affecting requests that work today: with
 * session.use_only_cookies=1 a session ID form field is not taken for the
 * progress record (session_start() does not take it either), and a progress
 * key that the "php" serializer cannot encode is not tracked (it would make
 * every update() save the session as an empty string).
 *
 * The record layout (php_session_rfc1867_progress, php_ps_globals) is taken from
 * the headers of the PHP release this extension is built against. A build can be
 * loaded into a later release of the same branch, so the wrapper is only
 * installed when the running PHP_VERSION_ID is affected as well and the size of
 * the session globals matches.
 */

#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED
# define COLOPL_BC_SECURITY_SUPH_INI_DEFAULT "1"
#else
# define COLOPL_BC_SECURITY_SUPH_INI_DEFAULT "0"
#endif

static int colopl_bc_security_suph_requested = 0;	/* INI value */
static int colopl_bc_security_suph_applied = 0;		/* hook wrapped by us */
static const char *colopl_bc_security_suph_state = "inactive (disabled by INI)";

#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED

# if PHP_VERSION_ID >= 80300
typedef zend_result colopl_bc_security_rfc1867_result;
# else
typedef int colopl_bc_security_rfc1867_result;
# endif

typedef colopl_bc_security_rfc1867_result (*colopl_bc_security_rfc1867_callback)(unsigned int event, void *event_data, void **extra TSRMLS_DC);

static colopl_bc_security_rfc1867_callback colopl_bc_security_suph_next = NULL;

/* Resolved at MINIT and read-only afterwards. */
# ifdef ZTS
static ts_rsrc_id colopl_bc_security_suph_ps_id = 0;
#  define COLOPL_BC_SECURITY_PS_GLOBALS() ((php_ps_globals *) ts_resource(colopl_bc_security_suph_ps_id))
# else
static php_ps_globals *colopl_bc_security_suph_ps = NULL;
#  define COLOPL_BC_SECURITY_PS_GLOBALS() (colopl_bc_security_suph_ps)
# endif

/* The running PHP still has CVE-2020-7062 (see colopl_bc_security_suph_callback()). */
static int colopl_bc_security_suph_null_data_guard = 0;

# if PHP_VERSION_ID >= 70000

/* Same conditions ext/session checks before touching the progress record. */
#  define COLOPL_BC_SECURITY_SUPH_HAS_KEY(progress) \
	(Z_TYPE((progress)->sid) && (progress)->key.s)
#  define COLOPL_BC_SECURITY_SUPH_HAS_DATA(progress) \
	(Z_TYPE((progress)->data) == IS_ARRAY)

/* Returns 0 when the tree does not have the layout ext/session builds. */
static int colopl_bc_security_suph_rebind(php_session_rfc1867_progress *progress)
{
	zval *bytes, *files, *current_file, *current_bytes;

	bytes = zend_hash_str_find(Z_ARRVAL(progress->data), "bytes_processed", sizeof("bytes_processed") - 1);
	if (!bytes || Z_TYPE_P(bytes) != IS_LONG) {
		return 0;
	}
	files = zend_hash_str_find(Z_ARRVAL(progress->data), "files", sizeof("files") - 1);
	if (!files || Z_TYPE_P(files) != IS_ARRAY || Z_ARRVAL_P(files)->nNextFreeElement < 1) {
		return 0;
	}
	/* current_file is always the element add_next_index_zval() appended last. */
	current_file = zend_hash_index_find(Z_ARRVAL_P(files), Z_ARRVAL_P(files)->nNextFreeElement - 1);
	if (!current_file || Z_TYPE_P(current_file) != IS_ARRAY) {
		return 0;
	}
	current_bytes = zend_hash_str_find(Z_ARRVAL_P(current_file), "bytes_processed", sizeof("bytes_processed") - 1);
	if (!current_bytes || Z_TYPE_P(current_bytes) != IS_LONG) {
		return 0;
	}

	progress->post_bytes_processed = bytes;
	ZVAL_COPY_VALUE(&progress->files, files);
	ZVAL_COPY_VALUE(&progress->current_file, current_file);
	progress->current_file_bytes_processed = current_bytes;

	return 1;
}

# else	/* PHP_VERSION_ID >= 70000 */

/* PHP 5: the progress record holds zval pointers (sid is inline), the tree owns them. */
#  define COLOPL_BC_SECURITY_SUPH_HAS_KEY(progress) \
	(Z_TYPE((progress)->sid) && (progress)->key.c)
#  define COLOPL_BC_SECURITY_SUPH_HAS_DATA(progress) \
	((progress)->data && Z_TYPE_P((progress)->data) == IS_ARRAY)

static int colopl_bc_security_suph_rebind(php_session_rfc1867_progress *progress)
{
	zval **bytes, **files, **current_file, **current_bytes;

	if (zend_hash_find(Z_ARRVAL_P(progress->data), "bytes_processed", sizeof("bytes_processed"), (void **) &bytes) != SUCCESS
		|| Z_TYPE_PP(bytes) != IS_LONG
	) {
		return 0;
	}
	if (zend_hash_find(Z_ARRVAL_P(progress->data), "files", sizeof("files"), (void **) &files) != SUCCESS
		|| Z_TYPE_PP(files) != IS_ARRAY
		|| Z_ARRVAL_PP(files)->nNextFreeElement == 0
	) {
		return 0;
	}
	/* current_file is always the element add_next_index_zval() appended last. */
	if (zend_hash_index_find(Z_ARRVAL_PP(files), Z_ARRVAL_PP(files)->nNextFreeElement - 1, (void **) &current_file) != SUCCESS
		|| Z_TYPE_PP(current_file) != IS_ARRAY
	) {
		return 0;
	}
	if (zend_hash_find(Z_ARRVAL_PP(current_file), "bytes_processed", sizeof("bytes_processed"), (void **) &current_bytes) != SUCCESS
		|| Z_TYPE_PP(current_bytes) != IS_LONG
	) {
		return 0;
	}

	progress->post_bytes_processed = *bytes;
	progress->files = *files;
	progress->current_file = *current_file;
	progress->current_file_bytes_processed = *current_bytes;

	return 1;
}

# endif	/* PHP_VERSION_ID >= 70000 */

/* php_session_encode() with the "php" serializer fails for a key containing
 * PS_DELIMITER, and before PHP 7.2 also PS_UNDEF_MARKER (ext/session/session.c). */
# if PHP_VERSION_ID < 70200
#  define COLOPL_BC_SECURITY_SUPH_PHP_SERIALIZER_REJECTED "|!"
# else
#  define COLOPL_BC_SECURITY_SUPH_PHP_SERIALIZER_REJECTED "|"
# endif

static int colopl_bc_security_suph_key_encodable(const php_ps_globals *ps, const php_session_rfc1867_progress *progress)
{
	const char *key, *c;
	size_t key_len;

	if (!ps->serializer || !ps->serializer->name || strcmp(ps->serializer->name, "php") != 0) {
		return 1;
	}
# if PHP_VERSION_ID >= 70000
	key = ZSTR_VAL(progress->key.s);
	key_len = ZSTR_LEN(progress->key.s);
# else
	key = progress->key.c;
	key_len = progress->key.len;
# endif
	for (c = COLOPL_BC_SECURITY_SUPH_PHP_SERIALIZER_REJECTED; *c; c++) {
		if (memchr(key, *c, key_len)) {
			return 0;
		}
	}

	return 1;
}

/*
 * ext/session takes the session ID for the progress record from a form field
 * named after the session even with session.use_only_cookies=1, which
 * session_start() never does. With use_only_cookies=1 the record keeps the ID
 * it had before that field (none, or the cookie found when the progress key
 * arrived); the field itself still reaches $_POST.
 */
# if PHP_VERSION_ID >= 80500
/* PS(session_name) is a zend_string since PHP 8.5. */
#  define COLOPL_BC_SECURITY_SUPH_IS_SESSION_NAME(ps, name) \
	((ps)->session_name && zend_string_equals_cstr((ps)->session_name, (name), strlen(name)))
# else
#  define COLOPL_BC_SECURITY_SUPH_IS_SESSION_NAME(ps, name) \
	((ps)->session_name && strcmp((name), (ps)->session_name) == 0)
# endif

static colopl_bc_security_rfc1867_result colopl_bc_security_suph_formdata(void *event_data, void **extra TSRMLS_DC)
{
	php_ps_globals *ps = COLOPL_BC_SECURITY_PS_GLOBALS();
	multipart_event_formdata *data = (multipart_event_formdata *) event_data;
	php_session_rfc1867_progress *progress = ps->rfc1867_enabled ? ps->rfc1867_progress : NULL;
	colopl_bc_security_rfc1867_result result;
	zval sid;

	if (!progress || !ps->use_only_cookies || !data->name || !COLOPL_BC_SECURITY_SUPH_IS_SESSION_NAME(ps, data->name)) {
		return colopl_bc_security_suph_next(MULTIPART_EVENT_FORMDATA, event_data, extra TSRMLS_CC);
	}

# if PHP_VERSION_ID >= 70000
	ZVAL_COPY(&sid, &progress->sid);
	result = colopl_bc_security_suph_next(MULTIPART_EVENT_FORMDATA, event_data, extra TSRMLS_CC);
	zval_ptr_dtor(&progress->sid);
	ZVAL_COPY_VALUE(&progress->sid, &sid);
# else
	sid = progress->sid;
	zval_copy_ctor(&sid);
	result = colopl_bc_security_suph_next(MULTIPART_EVENT_FORMDATA, event_data, extra TSRMLS_CC);
	zval_dtor(&progress->sid);
	progress->sid = sid;
# endif

	return result;
}

static colopl_bc_security_rfc1867_result colopl_bc_security_suph_callback(unsigned int event, void *event_data, void **extra TSRMLS_DC)
{
	php_ps_globals *ps;
	php_session_rfc1867_progress *progress;

	switch (event) {
		case MULTIPART_EVENT_FORMDATA:
			return colopl_bc_security_suph_formdata(event_data, extra TSRMLS_CC);
		case MULTIPART_EVENT_FILE_START:
		case MULTIPART_EVENT_FILE_DATA:
		case MULTIPART_EVENT_FILE_END:
		case MULTIPART_EVENT_END:
			ps = COLOPL_BC_SECURITY_PS_GLOBALS();
			/* Same guard as ext/session: PS(rfc1867_progress) belongs to the body
			 * being parsed only while the feature is enabled (it is not reset when
			 * a request bails out). It is allocated by MULTIPART_EVENT_START. */
			if (!ps->rfc1867_enabled) {
				break;
			}
			progress = ps->rfc1867_progress;
			if (!progress || !COLOPL_BC_SECURITY_SUPH_HAS_KEY(progress)) {
				break;
			}
			if (!COLOPL_BC_SECURITY_SUPH_HAS_DATA(progress)) {
				/* No FILE_START has been processed yet, so php_rinit_session() has
				 * not run for this body: releasing the key here makes ext/session
				 * skip the upload without leaving anything to clean up. */
				if (event == MULTIPART_EVENT_FILE_START && !colopl_bc_security_suph_key_encodable(ps, progress)) {
					/* php_session_encode() would fail on every update(), and the
					 * session is then saved as an empty string, wiping the data
					 * stored under that ID. Such a key cannot be tracked anyway. */
					smart_str_free(&progress->key);
				} else if (event == MULTIPART_EVENT_END && !ps->rfc1867_cleanup && colopl_bc_security_suph_null_data_guard) {
					/* CVE-2020-7062 (bug #79221): with cleanup=0, END stores into the
					 * progress array without checking that it exists (NULL / IS_UNDEF
					 * dereference) before 7.2.28 / 7.3.15 / 7.4.3, which skip that store. */
					smart_str_free(&progress->key);
				}
				break;
			}
# if PHP_VERSION_ID >= 70000
			if (event == MULTIPART_EVENT_END && !ps->rfc1867_cleanup) {
				SEPARATE_ARRAY(&progress->data);
			}
# endif
			if (!colopl_bc_security_suph_rebind(progress)) {
				/* Not reachable with the tree ext/session builds. ext/session skips
				 * every event without a key, including the END clean-up, so the
				 * session state php_rinit_session() set up for this body is left to
				 * the request shutdown (debug builds report it as a leak). */
				smart_str_free(&progress->key);
			}
			break;
		default:
			break;
	}

	return colopl_bc_security_suph_next(event, event_data, extra TSRMLS_CC);
}

static long colopl_bc_security_runtime_version_id(TSRMLS_D)
{
	long version_id = 0;
# if PHP_VERSION_ID >= 70000
	zval *constant = zend_get_constant_str("PHP_VERSION_ID", sizeof("PHP_VERSION_ID") - 1);

	if (constant && Z_TYPE_P(constant) == IS_LONG) {
		version_id = (long) Z_LVAL_P(constant);
	}
# else
	zval constant;

	if (zend_get_constant("PHP_VERSION_ID", sizeof("PHP_VERSION_ID") - 1, &constant TSRMLS_CC)) {
		if (Z_TYPE(constant) == IS_LONG) {
			version_id = Z_LVAL(constant);
		}
		zval_dtor(&constant);
	}
# endif

	return version_id;
}

static zend_module_entry *colopl_bc_security_find_session_module(TSRMLS_D)
{
# if PHP_VERSION_ID >= 70000
	return zend_hash_str_find_ptr(&module_registry, "session", sizeof("session") - 1);
# else
	zend_module_entry *module;

	if (zend_hash_find(&module_registry, "session", sizeof("session"), (void **) &module) != SUCCESS) {
		return NULL;
	}
	return module;
# endif
}

#endif	/* COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED */

/*
 * Security backport: session.serialize_handler lock
 *
 * session.serialize_handler is PHP_INI_ALL, so the format a session file is
 * decoded with can be switched per directory (.user.ini, .htaccess) or at run
 * time (ini_set(), session_start() options). Decoding data that was written by
 * one handler with another is an object-injection primitive ("handler
 * mismatch"). This mitigation narrows the directive to PHP_INI_SYSTEM in the
 * engine's INI registry after ext/session has registered it, so only php.ini
 * and the SAPI configuration (php_admin_value, -d) decide the handler; every
 * later change is refused by the engine, exactly as for any other
 * PHP_INI_SYSTEM directive.
 *
 * Feature-disabling for deployments that rely on per-directory handlers,
 * therefore opt-in (default 0). The value itself is never changed.
 */

static int colopl_bc_security_ssh_requested = 0;	/* INI value */
static int colopl_bc_security_ssh_applied = 0;		/* modifiable narrowed by us */
static int colopl_bc_security_ssh_orig_modifiable = 0;

#define COLOPL_BC_SECURITY_SSH_DIRECTIVE "session.serialize_handler"

static zend_ini_entry *colopl_bc_security_ssh_find_entry(TSRMLS_D)
{
#if PHP_VERSION_ID >= 70000
	return zend_hash_str_find_ptr(EG(ini_directives), COLOPL_BC_SECURITY_SSH_DIRECTIVE, sizeof(COLOPL_BC_SECURITY_SSH_DIRECTIVE) - 1);
#else
	zend_ini_entry *entry;

	if (zend_hash_find(EG(ini_directives), COLOPL_BC_SECURITY_SSH_DIRECTIVE, sizeof(COLOPL_BC_SECURITY_SSH_DIRECTIVE), (void **) &entry) == SUCCESS) {
		return entry;
	}
	return NULL;
#endif
}

int colopl_bc_security_session_serialize_handler_is_locked(void)
{
	return colopl_bc_security_ssh_applied;
}

/* MINIT: runs after ext/session's MINIT (module dependency), i.e. after the directive exists. */
static void colopl_bc_security_session_serialize_handler_lock(TSRMLS_D)
{
	zend_ini_entry *entry;

	if (!colopl_bc_security_ssh_requested || colopl_bc_security_ssh_applied) {
		return;
	}
	entry = colopl_bc_security_ssh_find_entry(TSRMLS_C);
	if (!entry) {
		return;	/* ext/session is not loaded: nothing to lock */
	}
	colopl_bc_security_ssh_orig_modifiable = entry->modifiable;
	entry->modifiable = PHP_INI_SYSTEM;
	colopl_bc_security_ssh_applied = 1;
}

/* MSHUTDOWN: runs before ext/session's MSHUTDOWN, so the directive still exists. */
static void colopl_bc_security_session_serialize_handler_unlock(TSRMLS_D)
{
	zend_ini_entry *entry;

	if (!colopl_bc_security_ssh_applied) {
		return;
	}
	entry = colopl_bc_security_ssh_find_entry(TSRMLS_C);
	if (entry) {
		entry->modifiable = colopl_bc_security_ssh_orig_modifiable;
	}
	colopl_bc_security_ssh_applied = 0;
}

static const char *colopl_bc_security_session_serialize_handler_state(void)
{
	if (colopl_bc_security_ssh_applied) {
		return "active (session.serialize_handler is PHP_INI_SYSTEM)";
	}
	if (colopl_bc_security_ssh_requested) {
		return "inactive (session.serialize_handler is not registered)";
	}
	return "inactive (disabled by INI)";
}

/*
 * Security backport: unserialize() nested-state isolation
 *
 * unserialize() keeps per-call state (the var_hash used to resolve r: / R:
 * back-references and to queue delayed __wakeup / __unserialize / destructor
 * calls). A nested unserialize() call shares the outermost call's state unless
 * BG(serialize_lock) is held. ext/standard holds that lock itself around every
 * nested entry point it intends to support (__wakeup, __unserialize,
 * Serializable::unserialize(), unserialize_callback_func), so sharing can only
 * be reached from code that runs during unserialize() by accident: destructors
 * of values that are replaced or released while the call is still parsing or
 * tearing down its state. Such a nested call can then resolve a back-reference
 * into state that the outer call is in the middle of destroying.
 *
 * This mitigation wraps the unserialize() handler and holds BG(serialize_lock)
 * for the whole duration of the call, so that every nested (un)serialize()
 * call, whatever started it, runs on its own state. The intended nested entry
 * points already run under the lock, so their behaviour does not change; the
 * only observable difference is that a destructor-started nested unserialize()
 * can no longer resolve back-references into the outer call, which was never a
 * supported use.
 */

typedef void (*colopl_bc_security_zif_handler)(INTERNAL_FUNCTION_PARAMETERS);

static int colopl_bc_security_unser_requested = 0;	/* INI value */
static int colopl_bc_security_unser_applied = 0;	/* handler replaced by us */
static colopl_bc_security_zif_handler colopl_bc_security_unser_orig = NULL;

int colopl_bc_security_unserialize_is_isolated(void)
{
	return colopl_bc_security_unser_applied;
}

static void colopl_bc_security_unserialize_handler(INTERNAL_FUNCTION_PARAMETERS)
{
	BG(serialize_lock)++;
	colopl_bc_security_unser_orig(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	BG(serialize_lock)--;
}

static zend_function *colopl_bc_security_find_internal_function(const char *name, size_t len TSRMLS_DC)
{
#if PHP_VERSION_ID >= 70000
	return zend_hash_str_find_ptr(CG(function_table), name, len);
#else
	zend_function *fn;

	if (zend_hash_find(CG(function_table), name, len + 1, (void **) &fn) == SUCCESS) {
		return fn;
	}
	return NULL;
#endif
}

static void colopl_bc_security_unserialize_isolate(TSRMLS_D)
{
	zend_function *fn;

	if (!colopl_bc_security_unser_requested || colopl_bc_security_unser_applied) {
		return;
	}
	if (!COLOPL_BC_SECURITY_UNSERIALIZE_NESTED_STATE_AFFECTED(PHP_VERSION_ID)) {
		return;
	}
	fn = colopl_bc_security_find_internal_function("unserialize", sizeof("unserialize") - 1 TSRMLS_CC);
	if (!fn || fn->type != ZEND_INTERNAL_FUNCTION || !fn->internal_function.handler) {
		return;
	}
	colopl_bc_security_unser_orig = fn->internal_function.handler;
	fn->internal_function.handler = colopl_bc_security_unserialize_handler;
	colopl_bc_security_unser_applied = 1;
}

static void colopl_bc_security_unserialize_restore(TSRMLS_D)
{
	zend_function *fn;

	if (!colopl_bc_security_unser_applied) {
		return;
	}
	fn = colopl_bc_security_find_internal_function("unserialize", sizeof("unserialize") - 1 TSRMLS_CC);
	if (fn && fn->type == ZEND_INTERNAL_FUNCTION && fn->internal_function.handler == colopl_bc_security_unserialize_handler) {
		fn->internal_function.handler = colopl_bc_security_unser_orig;
	}
	colopl_bc_security_unser_orig = NULL;
	colopl_bc_security_unser_applied = 0;
}

static const char *colopl_bc_security_unserialize_state(void)
{
	if (colopl_bc_security_unser_applied) {
		return "active (unserialize() runs with serialize_lock held)";
	}
	if (!colopl_bc_security_unser_requested) {
		return "inactive (disabled by INI)";
	}
	if (!COLOPL_BC_SECURITY_UNSERIALIZE_NESTED_STATE_AFFECTED(PHP_VERSION_ID)) {
		return "inactive (not needed on this PHP version)";
	}
	return "inactive (unserialize() is not registered)";
}

PHP_INI_BEGIN()
	PHP_INI_ENTRY(COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS, "0", PHP_INI_SYSTEM, NULL)
	PHP_INI_ENTRY(COLOPL_BC_SECURITY_INI_HARDEN_SESSION_UPLOAD_PROGRESS, COLOPL_BC_SECURITY_SUPH_INI_DEFAULT, PHP_INI_SYSTEM, NULL)
	PHP_INI_ENTRY(COLOPL_BC_SECURITY_INI_LOCK_SESSION_SERIALIZE_HANDLER, "0", PHP_INI_SYSTEM, NULL)
	PHP_INI_ENTRY(COLOPL_BC_SECURITY_INI_ISOLATE_UNSERIALIZE, "1", PHP_INI_SYSTEM, NULL)
PHP_INI_END()

int colopl_bc_security_session_upload_progress_is_hardened(void)
{
	return colopl_bc_security_suph_applied;
}

static void colopl_bc_security_session_upload_progress_harden(TSRMLS_D)
{
#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED
	zend_module_entry *session;
	long version_id;

	if (!colopl_bc_security_suph_requested) {
		colopl_bc_security_suph_state = "inactive (disabled by INI)";
		return;
	}
	if (colopl_bc_security_sup_applied) {
		colopl_bc_security_suph_state = "inactive (session.upload_progress hook detached)";
		return;
	}
	session = colopl_bc_security_find_session_module(TSRMLS_C);
	if (!session) {
		colopl_bc_security_suph_state = "inactive (session extension not loaded)";
		return;
	}
	if (php_rfc1867_callback == NULL) {
		colopl_bc_security_suph_state = "inactive (no php_rfc1867_callback was registered)";
		return;
	}
	if (session->globals_size != sizeof(php_ps_globals)
# ifdef ZTS
		|| !session->globals_id_ptr
# else
		|| !session->globals_ptr
# endif
	) {
		colopl_bc_security_suph_state = "inactive (session globals layout mismatch)";
		return;
	}
	version_id = colopl_bc_security_runtime_version_id(TSRMLS_C);
	if (version_id <= 0 || !COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_POINTERS_AFFECTED(version_id)) {
		colopl_bc_security_suph_state = "inactive (not applicable to the running PHP version)";
		return;
	}

# ifdef ZTS
	colopl_bc_security_suph_ps_id = *session->globals_id_ptr;
# else
	colopl_bc_security_suph_ps = (php_ps_globals *) session->globals_ptr;
# endif
	colopl_bc_security_suph_null_data_guard = COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_NULL_DATA_AFFECTED(version_id) ? 1 : 0;
	colopl_bc_security_suph_next = php_rfc1867_callback;
	php_rfc1867_callback = colopl_bc_security_suph_callback;
	colopl_bc_security_suph_applied = 1;
	colopl_bc_security_suph_state = "active (php_rfc1867_callback wrapped)";
#else
	colopl_bc_security_suph_state = "inactive (not applicable to this PHP version)";
#endif
}

static void colopl_bc_security_session_upload_progress_unharden(void)
{
#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED
	if (!colopl_bc_security_suph_applied) {
		return;
	}

	/* Hand the chain back before this module is unloaded; ext/session's MSHUTDOWN
	 * (which runs after ours) only clears the hook when it is its own callback.
	 * A hook installed after ours is expected to restore ours on its MSHUTDOWN. */
	if (php_rfc1867_callback == colopl_bc_security_suph_callback) {
		php_rfc1867_callback = colopl_bc_security_suph_next;
	}
	colopl_bc_security_suph_next = NULL;
	colopl_bc_security_suph_null_data_guard = 0;
	colopl_bc_security_suph_applied = 0;
#endif
}

static const char *colopl_bc_security_session_upload_progress_null_data_state(void)
{
#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED
	if (!colopl_bc_security_suph_applied) {
		return "inactive (pointer hardening is inactive)";
	}
	return colopl_bc_security_suph_null_data_guard
		? "active"
		: "inactive (fixed in the running PHP version)";
#else
	return "inactive (not applicable to this PHP version)";
#endif
}

int colopl_bc_security_session_upload_progress_is_disabled(void)
{
	return colopl_bc_security_sup_applied;
}

static void colopl_bc_security_session_upload_progress_apply(void)
{
#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_SUPPORTED
	if (!colopl_bc_security_sup_requested || colopl_bc_security_sup_applied) {
		return;
	}

	/* The detached hook is never called again, so it is not stored; we only
	 * remember whether one was registered.
	 * NULL when ext/session is not loaded: nothing to detach, but stay "applied"
	 * so that a hook registered later cannot be reached either. */
	colopl_bc_security_sup_had_callback = php_rfc1867_callback != NULL;
	php_rfc1867_callback = NULL;
	colopl_bc_security_sup_applied = 1;
#endif
}

/*
 * No restore on MSHUTDOWN: ext/session's MSHUTDOWN only clears the hook when it
 * still points at its own callback, so the end state is NULL either way, and a
 * subsequent MINIT cycle (e.g. a graceful restart of a threaded SAPI) re-registers
 * ext/session before this module detaches it again. The bookkeeping is reset so
 * that this also holds when the module is not unloaded in between (static builds,
 * ZEND_DONT_UNLOAD_MODULES).
 */
static void colopl_bc_security_session_upload_progress_reset(void)
{
	colopl_bc_security_sup_applied = 0;
	colopl_bc_security_sup_had_callback = 0;
}

int colopl_bc_security_minit(INIT_FUNC_ARGS)
{
	REGISTER_INI_ENTRIES();

	colopl_bc_security_sup_requested = INI_BOOL(COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS) ? 1 : 0;
	colopl_bc_security_session_upload_progress_apply();

	/* After the detach: there is nothing to harden once the hook is gone. */
	colopl_bc_security_suph_requested = INI_BOOL(COLOPL_BC_SECURITY_INI_HARDEN_SESSION_UPLOAD_PROGRESS) ? 1 : 0;
	colopl_bc_security_session_upload_progress_harden(TSRMLS_C);

	colopl_bc_security_ssh_requested = INI_BOOL(COLOPL_BC_SECURITY_INI_LOCK_SESSION_SERIALIZE_HANDLER) ? 1 : 0;
	colopl_bc_security_session_serialize_handler_lock(TSRMLS_C);

	colopl_bc_security_unser_requested = INI_BOOL(COLOPL_BC_SECURITY_INI_ISOLATE_UNSERIALIZE) ? 1 : 0;
	colopl_bc_security_unserialize_isolate(TSRMLS_C);

	return SUCCESS;
}

int colopl_bc_security_mshutdown(SHUTDOWN_FUNC_ARGS)
{
	colopl_bc_security_unserialize_restore(TSRMLS_C);
	colopl_bc_security_session_serialize_handler_unlock(TSRMLS_C);
	colopl_bc_security_session_upload_progress_unharden();
	colopl_bc_security_session_upload_progress_reset();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

void colopl_bc_security_minfo(ZEND_MODULE_INFO_FUNC_ARGS)
{
	const char *state;

#if COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_SUPPORTED
	if (colopl_bc_security_sup_applied) {
		state = colopl_bc_security_sup_had_callback
			? "active (php_rfc1867_callback detached)"
			: "active (no php_rfc1867_callback was registered)";
	} else {
		state = "inactive (disabled by INI)";
	}
#else
	state = "inactive (not applicable to this PHP version)";
#endif

	php_info_print_table_start();
	php_info_print_table_header(2, "colopl_bc security backports", "enabled");
	php_info_print_table_row(2, "session.upload_progress hook mitigation", state);
	php_info_print_table_row(2, COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS, colopl_bc_security_sup_requested ? "1" : "0");
	php_info_print_table_row(2, "session.upload_progress pointer hardening", colopl_bc_security_suph_state);
	php_info_print_table_row(2, "session.upload_progress CVE-2020-7062 guard", colopl_bc_security_session_upload_progress_null_data_state());
	php_info_print_table_row(2, COLOPL_BC_SECURITY_INI_HARDEN_SESSION_UPLOAD_PROGRESS, colopl_bc_security_suph_requested ? "1" : "0");
	php_info_print_table_row(2, "session.serialize_handler lock", colopl_bc_security_session_serialize_handler_state());
	php_info_print_table_row(2, COLOPL_BC_SECURITY_INI_LOCK_SESSION_SERIALIZE_HANDLER, colopl_bc_security_ssh_requested ? "1" : "0");
	php_info_print_table_row(2, "unserialize() nested-state isolation", colopl_bc_security_unserialize_state());
	php_info_print_table_row(2, COLOPL_BC_SECURITY_INI_ISOLATE_UNSERIALIZE, colopl_bc_security_unser_requested ? "1" : "0");
	php_info_print_table_end();
}
