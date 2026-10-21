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

#ifndef PHP_COLOPL_BC_SECURITY_H
#define PHP_COLOPL_BC_SECURITY_H

#include "php.h"

/*
 * Security backports.
 *
 * This part of the extension is intentionally written against the lowest
 * common denominator of the PHP extension API so that it can be built on
 * every PHP release since 5.4 (TSRMLS_* macros, no zend_long, no zend_string).
 *
 * Nothing here depends on the compatibility layer (php70 / php74); on PHP
 * versions outside the compatibility-layer targets the module entry contains
 * only this part ("security only" build, see php_colopl_bc.h).
 */

/* session.upload_progress hook (php_rfc1867_callback) is available since PHP 5.4.0. */
#if PHP_VERSION_ID >= 50400
# define COLOPL_BC_SECURITY_HAVE_RFC1867_CALLBACK 1
#else
# define COLOPL_BC_SECURITY_HAVE_RFC1867_CALLBACK 0
#endif

/*
 * TODO(secfix): once the upstream fix for the 2026-10-21 security release is
 * published, replace this with per-branch "first fixed" PHP_VERSION_ID checks
 * (7.4.x / 8.1.x: every release, 8.2+: below the fixed release) so that the
 * mitigation becomes a no-op on fixed builds.
 */
#define COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_AFFECTED 1

#if COLOPL_BC_SECURITY_HAVE_RFC1867_CALLBACK && COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_AFFECTED
# define COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_SUPPORTED 1
#else
# define COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_SUPPORTED 0
#endif

/*
 * PHP versions whose ext/session upload progress callback runs before the
 * 2026-10-21 security release (GHSA-w8hw-w9fx-j79w / GHSA-6wmf-4rv5-p3rp,
 * CVE-2026-109618 / CVE-2026-109619, fixed in 8.2.35 / 8.3.36 / 8.4.27 /
 * 8.5.12): it dereferences the zval pointers it caches into its progress
 * array, decodes the session before RINIT with stale globals and user code
 * enabled, and leaves $_SESSION behind (see colopl_bc_security.c). PHP <= 8.1
 * is affected on every release. Usable both in #if and at runtime.
 */
#define COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_POINTERS_AFFECTED(id) ( \
	(id) < 80235 || \
	((id) >= 80300 && (id) < 80336) || \
	((id) >= 80400 && (id) < 80427) || \
	((id) >= 80500 && (id) < 80512) \
)

/*
 * CVE-2020-7062 (bug #79221): MULTIPART_EVENT_END with session.upload_progress.cleanup=0
 * dereferences the progress array of an upload that never reached FILE_START.
 * Fixed in 7.2.28, 7.3.15 and 7.4.3; every older release is affected.
 */
#define COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_NULL_DATA_AFFECTED(id) ( \
	(id) < 70228 || \
	((id) >= 70300 && (id) < 70315) || \
	((id) >= 70400 && (id) < 70403) \
)

/* Needs the ext/session headers (HAVE_PHP_SESSION: PHP was built with session, shared or not). */
#if COLOPL_BC_SECURITY_HAVE_RFC1867_CALLBACK && defined(HAVE_PHP_SESSION) && \
	COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_POINTERS_AFFECTED(PHP_VERSION_ID)
# define COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED 1
#else
# define COLOPL_BC_SECURITY_SESSION_UPLOAD_PROGRESS_HARDENING_SUPPORTED 0
#endif

#define COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS "colopl_bc.security.disable_session_upload_progress"
#define COLOPL_BC_SECURITY_INI_HARDEN_SESSION_UPLOAD_PROGRESS "colopl_bc.security.harden_session_upload_progress"
#define COLOPL_BC_SECURITY_INI_LOCK_SESSION_SERIALIZE_HANDLER "colopl_bc.security.lock_session_serialize_handler"
#define COLOPL_BC_SECURITY_INI_ISOLATE_UNSERIALIZE "colopl_bc.security.isolate_unserialize"
#define COLOPL_BC_SECURITY_INI_ABORT_ON_MEMORY_ERROR "colopl_bc.security.abort_on_memory_error"
#define COLOPL_BC_SECURITY_INI_ABORT_ON_TIMEOUT "colopl_bc.security.abort_on_timeout"
#define COLOPL_BC_SECURITY_INI_RESET_STALE_REQUEST_STATE "colopl_bc.security.reset_stale_request_state"

/*
 * TODO(secfix): once the upstream fix for the unserialize() nested-state issue
 * is published, replace this with per-branch "first fixed" PHP_VERSION_ID checks
 * so that the wrapper is a no-op on fixed builds.
 */
#define COLOPL_BC_SECURITY_UNSERIALIZE_NESTED_STATE_AFFECTED(id) 1

int colopl_bc_security_minit(INIT_FUNC_ARGS);
int colopl_bc_security_mshutdown(SHUTDOWN_FUNC_ARGS);

/* Module post-RSHUTDOWN hook (zend_module_entry.post_deactivate_func): see reset_stale_request_state. */
int colopl_bc_security_post_deactivate(void);
void colopl_bc_security_minfo(ZEND_MODULE_INFO_FUNC_ARGS);

/* Whether the session.upload_progress hook is currently detached by this extension. */
int colopl_bc_security_session_upload_progress_is_disabled(void);

/* Whether the session.upload_progress hook is currently wrapped by the pointer hardening. */
int colopl_bc_security_session_upload_progress_is_hardened(void);

/* Whether session.serialize_handler is currently locked to PHP_INI_SYSTEM by this extension. */
int colopl_bc_security_session_serialize_handler_is_locked(void);

/* Whether unserialize() is currently wrapped so that nested calls never share its state. */
int colopl_bc_security_unserialize_is_isolated(void);

/* Whether a memory_limit / out-of-memory fatal error currently ends the process instead of bailing out. */
int colopl_bc_security_abort_on_memory_error_is_active(void);

/* Whether a max_execution_time fatal error currently ends the process instead of bailing out. */
int colopl_bc_security_abort_on_timeout_is_active(void);

/* Whether request-owned state that outlives its memory is cleared at the end of every request. */
int colopl_bc_security_stale_request_state_reset_is_active(void);

#endif	/* PHP_COLOPL_BC_SECURITY_H */
