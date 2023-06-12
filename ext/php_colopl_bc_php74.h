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

#ifndef PHP_COLOPL_BC_PHP74_H

#include "php_colopl_bc.h"

PHPAPI PHP_INI_MH(OnUpdateCompareMode);
PHPAPI PHP_INI_MH(OnUpdateSortMode);

PHPAPI int php_colopl_bc_compare(zval *op1, zval *op2);

#define PHP_COLOPL_BC_PHP74_H
#endif	/* PHP_COLOPL_BC_PHP74_H */
