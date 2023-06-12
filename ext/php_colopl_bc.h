/*
  +----------------------------------------------------------------------+
  | COLOPL PHP Backwards Compatibility Extension.                        |
  +----------------------------------------------------------------------+
  | Copyright (c) COLOPL, Inc.                                           |
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Go Kudo <g-kudo@colopl.co.jp>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_COLOPL_BC_H
#define PHP_COLOPL_BC_H

#include "php.h"
#include "ext/standard/php_lcg.h"

#include "php_colopl_bc_php70.h"
#include "php_colopl_bc_php74.h"

extern zend_module_entry colopl_bc_module_entry;
#define phpext_colopl_bc_ptr &colopl_bc_module_entry

#define COLOPL_BC_PHP74_COMPARE_MODE_SILENT		0
#define COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED	1
#define COLOPL_BC_PHP74_COMPARE_MODE_LOG		2

#define COLOPL_BC_PHP74_SORT_MODE_SILENT		0
#define COLOPL_BC_PHP74_SORT_MODE_DEPRECATED	1
#define COLOPL_BC_PHP74_SORT_MODE_LOG			2

#define PHP_COLOPL_BC_VERSION "10.0.0"

ZEND_BEGIN_MODULE_GLOBALS(colopl_bc)
	bool rand_is_seeded;
	bool mt_rand_is_seeded;
	uint32_t rand_seed;
	uint32_t mt_state[N+1];
	uint32_t *mt_next;
	int mt_left;
	int gnurandom_r[344];
	int gnurandom_next;
	bucket_compare_func_t *multisort_func;
	zend_fcall_info user_compare_fci;
	zend_fcall_info_cache user_compare_fci_cache;
	zend_long php74_compare_mode;
	int (*php74_compare_func)(zval *op1, zval *op2);
	zend_long php74_sort_mode;
 	void (*php74_hash_sort_func)(INTERNAL_FUNCTION_PARAMETERS, zval *array, bucket_compare_func_t compare_func, bool renumber);
ZEND_END_MODULE_GLOBALS(colopl_bc)

ZEND_EXTERN_MODULE_GLOBALS(colopl_bc)

#define COLOPL_BC_G(v)	ZEND_MODULE_GLOBALS_ACCESSOR(colopl_bc, v)

PHP_MINIT_FUNCTION(colopl_bc);
PHP_MSHUTDOWN_FUNCTION(colopl_bc);
PHP_RINIT_FUNCTION(colopl_bc);
PHP_MINFO_FUNCTION(colopl_bc);
/* PHP_GINIT_FUNCTION(colopl_bc); */

#if defined(ZTS) && defined(COMPILE_DL_COLOPL_BC)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_COLOPL_BC_H */
