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

#define COLOPL_BC_SECURITY_INI_DISABLE_SESSION_UPLOAD_PROGRESS "colopl_bc.security.disable_session_upload_progress"

int colopl_bc_security_minit(INIT_FUNC_ARGS);
int colopl_bc_security_mshutdown(SHUTDOWN_FUNC_ARGS);
void colopl_bc_security_minfo(ZEND_MODULE_INFO_FUNC_ARGS);

/* Whether the session.upload_progress hook is currently detached by this extension. */
int colopl_bc_security_session_upload_progress_is_disabled(void);

#endif	/* PHP_COLOPL_BC_SECURITY_H */
