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
#  define COLOPL_BC_SECURITY_PS(v) (((php_ps_globals *) ts_resource(colopl_bc_security_suph_ps_id))->v)
# else
static php_ps_globals *colopl_bc_security_suph_ps = NULL;
#  define COLOPL_BC_SECURITY_PS(v) (colopl_bc_security_suph_ps->v)
# endif

# if PHP_VERSION_ID >= 70000

#  define COLOPL_BC_SECURITY_SUPH_TRACKING(progress) \
	(Z_TYPE((progress)->sid) && (progress)->key.s && Z_TYPE((progress)->data) == IS_ARRAY)

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

/* PHP 5: the progress record holds zval pointers, the tree owns them. */
#  define COLOPL_BC_SECURITY_SUPH_TRACKING(progress) \
	(Z_TYPE((progress)->sid) && (progress)->key.c && (progress)->data && Z_TYPE_P((progress)->data) == IS_ARRAY)

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

static colopl_bc_security_rfc1867_result colopl_bc_security_suph_callback(unsigned int event, void *event_data, void **extra TSRMLS_DC)
{
	php_session_rfc1867_progress *progress;

	switch (event) {
		case MULTIPART_EVENT_FILE_START:
		case MULTIPART_EVENT_FILE_DATA:
		case MULTIPART_EVENT_FILE_END:
		case MULTIPART_EVENT_END:
			/* Same guard as ext/session: PS(rfc1867_progress) belongs to the body
			 * being parsed only while the feature is enabled (it is not reset when
			 * a request bails out). It is allocated by MULTIPART_EVENT_START. */
			if (!COLOPL_BC_SECURITY_PS(rfc1867_enabled)) {
				break;
			}
			progress = COLOPL_BC_SECURITY_PS(rfc1867_progress);
			if (!progress || !COLOPL_BC_SECURITY_SUPH_TRACKING(progress)) {
				break;
			}
# if PHP_VERSION_ID >= 70000
			if (event == MULTIPART_EVENT_END && !COLOPL_BC_SECURITY_PS(rfc1867_cleanup)) {
				SEPARATE_ARRAY(&progress->data);
			}
# endif
			if (!colopl_bc_security_suph_rebind(progress)) {
				/* ext/session skips every event (and the END clean-up) without a key. */
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

PHP_INI_BEGIN()
	PHP_INI_ENTRY(COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS, "0", PHP_INI_SYSTEM, NULL)
	PHP_INI_ENTRY(COLOPL_BC_SECURITY_INI_HARDEN_SESSION_UPLOAD_PROGRESS, COLOPL_BC_SECURITY_SUPH_INI_DEFAULT, PHP_INI_SYSTEM, NULL)
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
	colopl_bc_security_suph_applied = 0;
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
 * ext/session before this module detaches it again.
 */

int colopl_bc_security_minit(INIT_FUNC_ARGS)
{
	REGISTER_INI_ENTRIES();

	colopl_bc_security_sup_requested = INI_BOOL(COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS) ? 1 : 0;
	colopl_bc_security_session_upload_progress_apply();

	/* After the detach: there is nothing to harden once the hook is gone. */
	colopl_bc_security_suph_requested = INI_BOOL(COLOPL_BC_SECURITY_INI_HARDEN_SESSION_UPLOAD_PROGRESS) ? 1 : 0;
	colopl_bc_security_session_upload_progress_harden(TSRMLS_C);

	return SUCCESS;
}

int colopl_bc_security_mshutdown(SHUTDOWN_FUNC_ARGS)
{
	colopl_bc_security_session_upload_progress_unharden();

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
	php_info_print_table_row(2, COLOPL_BC_SECURITY_INI_HARDEN_SESSION_UPLOAD_PROGRESS, colopl_bc_security_suph_requested ? "1" : "0");
	php_info_print_table_end();
}
