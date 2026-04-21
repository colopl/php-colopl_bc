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
	php_info_print_table_row(2, "bc version", PHP_COLOPL_BC_VERSION);
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
