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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

#include "ext/standard/info.h"

#include "php_colopl_bc.h"
#include "colopl_bc_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(colopl_bc);

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("colopl_bc.php74.compare_mode", "0", PHP_INI_ALL, OnUpdateCompareMode, php74_compare_mode, zend_colopl_bc_globals, colopl_bc_globals)
	STD_PHP_INI_ENTRY("colopl_bc.php74.sort_mode", "0", PHP_INI_ALL, OnUpdateSortMode, php74_sort_mode, zend_colopl_bc_globals, colopl_bc_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(colopl_bc)
{
	zend_function *fentry;

	REGISTER_INI_ENTRIES();

	register_colopl_bc_symbols(module_number);

	/* Get original func ptr's */
	/* date_create */
	fentry = zend_hash_str_find_ptr(CG(function_table), "date_create", strlen("date_create"));
	ZEND_ASSERT(fentry);
	COLOPL_BC_G(date_create_fptr) = fentry->internal_function.handler;
	/* date_create_immutable */
	fentry = zend_hash_str_find_ptr(CG(function_table), "date_create_immutable", strlen("date_create_immutable"));
	ZEND_ASSERT(fentry);
	COLOPL_BC_G(date_create_immutable_fptr) = fentry->internal_function.handler;

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(colopl_bc)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

PHP_RINIT_FUNCTION(colopl_bc)
{
#if defined(ZTS) && defined(COMPILE_DL_COLOPL_BC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(colopl_bc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "colopl_bc support", "enabled");
	php_info_print_table_end();
}

PHP_GINIT_FUNCTION(colopl_bc)
{
#if defined(ZTS) && defined(COMPILE_DL_COLOPL_BC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
}

zend_module_entry colopl_bc_module_entry = {
	STANDARD_MODULE_HEADER,
	"colopl_bc",
	ext_functions,
	PHP_MINIT(colopl_bc),
	PHP_MSHUTDOWN(colopl_bc),
	PHP_RINIT(colopl_bc),
	NULL,
	PHP_MINFO(colopl_bc),
	PHP_COLOPL_BC_VERSION,
	PHP_MODULE_GLOBALS(colopl_bc),
	PHP_GINIT(colopl_bc),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_COLOPL_BC
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(colopl_bc)
#endif
