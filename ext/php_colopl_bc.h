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

#ifndef PHP_COLOPL_BC_H
#define PHP_COLOPL_BC_H

#include <stdarg.h>
#include <stdbool.h>

#include "php.h"

/*
 * The compatibility layer (php70 / php74 namespaces) targets PHP 7.4 and
 * PHP 8.1+ only. On every other PHP version since 5.4 the extension is built
 * in "security only" mode: just the security backports are compiled in.
 */
#if PHP_VERSION_ID < 70400 || (PHP_VERSION_ID >= 80000 && PHP_VERSION_ID < 80100)
# define COLOPL_BC_SECURITY_ONLY 1
#endif

#include "php_colopl_bc_security.h"

#define PHP_COLOPL_BC_VERSION "13.2.1"

extern zend_module_entry colopl_bc_module_entry;
#define phpext_colopl_bc_ptr &colopl_bc_module_entry

#ifndef COLOPL_BC_SECURITY_ONLY

#if PHP_VERSION_ID < 80400
#include "ext/standard/php_lcg.h"
#else
#include "ext/random/php_random.h"
#endif

#include "php_colopl_bc_php70.h"
#include "php_colopl_bc_php74.h"

#define COLOPL_BC_PHP74_COMPARE_MODE_SILENT		0
#define COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED	1
#define COLOPL_BC_PHP74_COMPARE_MODE_LOG		2

#define COLOPL_BC_PHP74_SORT_MODE_SILENT		0
#define COLOPL_BC_PHP74_SORT_MODE_DEPRECATED	1
#define COLOPL_BC_PHP74_SORT_MODE_LOG			2

#if PHP_VERSION_ID < 80000
# ifndef RETURN_THROWS
#  define RETURN_THROWS() return
# endif

# ifndef Z_PARAM_OBJECT_OF_CLASS_OR_NULL
#  define Z_PARAM_OBJECT_OF_CLASS_OR_NULL(dest, ce) Z_PARAM_OBJECT_OF_CLASS_EX(dest, ce, 1, 0)
# endif

static inline void php_colopl_bc_argument_value_error(uint32_t arg_num, const char *format, ...)
{
	char *message;
	va_list args;

	va_start(args, format);
	vspprintf(&message, 0, format, args);
	va_end(args);

	php_error_docref(NULL, E_WARNING, "Argument #%u %s", arg_num, message);
	efree(message);
}

# define zend_argument_value_error php_colopl_bc_argument_value_error
#endif

enum {
	COLOPL_BC_MT_N = 624,
	COLOPL_BC_MT_STATE_SIZE = COLOPL_BC_MT_N + 1
};

ZEND_BEGIN_MODULE_GLOBALS(colopl_bc)
	bool rand_is_seeded;
	bool mt_rand_is_seeded;
	uint32_t rand_seed;
	uint32_t mt_state[COLOPL_BC_MT_STATE_SIZE];
	uint32_t *mt_next;
	int mt_left;
	int gnurandom_r[344];
	int gnurandom_next;
#if PHP_VERSION_ID >= 80000
	bucket_compare_func_t *multisort_func;
#endif
	zend_fcall_info user_compare_fci;
	zend_fcall_info_cache user_compare_fci_cache;
	zend_long php74_compare_mode;
	int (*php74_compare_func)(zval *op1, zval *op2);
	zend_long php74_sort_mode;
#if PHP_VERSION_ID >= 80000
	void (*php74_hash_sort_func)(INTERNAL_FUNCTION_PARAMETERS, zval *array, bucket_compare_func_t compare_func, bool renumber, bool compare_func_may_call_user_code);
	void *php74_user_sort_context;
#endif
ZEND_END_MODULE_GLOBALS(colopl_bc)

ZEND_EXTERN_MODULE_GLOBALS(colopl_bc)

#define COLOPL_BC_G(v)	ZEND_MODULE_GLOBALS_ACCESSOR(colopl_bc, v)

#endif	/* !COLOPL_BC_SECURITY_ONLY */

PHP_MINIT_FUNCTION(colopl_bc);
PHP_MSHUTDOWN_FUNCTION(colopl_bc);
PHP_RINIT_FUNCTION(colopl_bc);
PHP_MINFO_FUNCTION(colopl_bc);
/* PHP_GINIT_FUNCTION(colopl_bc); */

#if defined(ZTS) && defined(COMPILE_DL_COLOPL_BC) && PHP_VERSION_ID >= 70000
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_COLOPL_BC_H */
