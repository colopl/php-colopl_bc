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

#ifndef PHP_COLOPL_BC_PHP74_H

#include "php_colopl_bc.h"

PHPAPI PHP_INI_MH(OnUpdateCompareMode);
PHPAPI PHP_INI_MH(OnUpdateSortMode);

PHPAPI int php_colopl_bc_compare(zval *op1, zval *op2);

#define PHP_COLOPL_BC_PHP74_H
#endif	/* PHP_COLOPL_BC_PHP74_H */
