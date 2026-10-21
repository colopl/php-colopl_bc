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

PHP_INI_BEGIN()
	PHP_INI_ENTRY(COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS, "0", PHP_INI_SYSTEM, NULL)
PHP_INI_END()

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

	/* The hook type differs across PHP versions (TSRMLS_DC, int vs zend_result),
	 * so it is never stored; we only remember whether one was registered.
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

	return SUCCESS;
}

int colopl_bc_security_mshutdown(SHUTDOWN_FUNC_ARGS)
{
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
	php_info_print_table_end();
}
