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

#include "php_colopl_bc.h"

#include "ext/standard/php_array.h"
#include "ext/standard/php_string.h"
#include "zend_operators.h"

/* zend_operators.c */

#define COLOPL_BC_TYPE_PAIR(t1,t2) (((t1) << 4) | (t2))

#define php_colopl_bc_convert_object_to_type(op, dst, ctype)									\
	ZVAL_UNDEF(dst);																			\
	if (Z_OBJ_HT_P(op)->cast_object(Z_OBJ_P(op), dst, ctype) == FAILURE) {						\
		zend_error(E_WARNING,																	\
			"Object of class %s could not be converted to %s", ZSTR_VAL(Z_OBJCE_P(op)->name),	\
		zend_get_type_by_const(ctype));															\
	} 																							\

static zend_never_inline zval* ZEND_FASTCALL _php_colopl_bc_zendi_convert_scalar_to_number_silent(zval *op, zval *holder)
{
	switch (Z_TYPE_P(op)) {
		case IS_NULL:
		case IS_FALSE:
			ZVAL_LONG(holder, 0);
			return holder;
		case IS_TRUE:
			ZVAL_LONG(holder, 1);
			return holder;
		case IS_STRING:
			if ((Z_TYPE_INFO_P(holder) = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &Z_LVAL_P(holder), &Z_DVAL_P(holder), 1)) == 0) {
				ZVAL_LONG(holder, 0);
			}
			return holder;
		case IS_RESOURCE:
			ZVAL_LONG(holder, Z_RES_HANDLE_P(op));
			return holder;
		case IS_OBJECT:
			php_colopl_bc_convert_object_to_type(op, holder, _IS_NUMBER);
			if (UNEXPECTED(EG(exception)) ||
				UNEXPECTED(Z_TYPE_P(holder) != IS_LONG && Z_TYPE_P(holder) != IS_DOUBLE)) {
				ZVAL_LONG(holder, 1);
			}
			return holder;
		case IS_LONG:
		case IS_DOUBLE:
		default:
			return op;
	}
}

static int legacy_compare_fast(zval *op1, zval *op2)
{
	int converted = 0;
	zval op1_copy, op2_copy;

	while (1) {
		switch (COLOPL_BC_TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case COLOPL_BC_TYPE_PAIR(IS_LONG, IS_LONG):
				return Z_LVAL_P(op1)>Z_LVAL_P(op2)?1:(Z_LVAL_P(op1)<Z_LVAL_P(op2)?-1:0);

			case COLOPL_BC_TYPE_PAIR(IS_DOUBLE, IS_LONG):
				return ZEND_NORMALIZE_BOOL(Z_DVAL_P(op1) - (double)Z_LVAL_P(op2));

			case COLOPL_BC_TYPE_PAIR(IS_LONG, IS_DOUBLE):
				return ZEND_NORMALIZE_BOOL((double)Z_LVAL_P(op1) - Z_DVAL_P(op2));

			case COLOPL_BC_TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				if (Z_DVAL_P(op1) == Z_DVAL_P(op2)) {
					return 0;
				} else {
					return ZEND_NORMALIZE_BOOL(Z_DVAL_P(op1) - Z_DVAL_P(op2));
				}

			case COLOPL_BC_TYPE_PAIR(IS_ARRAY, IS_ARRAY):
				return zend_compare_arrays(op1, op2);

			case COLOPL_BC_TYPE_PAIR(IS_NULL, IS_NULL):
			case COLOPL_BC_TYPE_PAIR(IS_NULL, IS_FALSE):
			case COLOPL_BC_TYPE_PAIR(IS_FALSE, IS_NULL):
			case COLOPL_BC_TYPE_PAIR(IS_FALSE, IS_FALSE):
			case COLOPL_BC_TYPE_PAIR(IS_TRUE, IS_TRUE):
				return 0;

			case COLOPL_BC_TYPE_PAIR(IS_NULL, IS_TRUE):
				return -1;

			case COLOPL_BC_TYPE_PAIR(IS_TRUE, IS_NULL):
				return 1;

			case COLOPL_BC_TYPE_PAIR(IS_STRING, IS_STRING):
				if (Z_STR_P(op1) == Z_STR_P(op2)) {
					return 0;
				}
				return zendi_smart_strcmp(Z_STR_P(op1), Z_STR_P(op2));

			case COLOPL_BC_TYPE_PAIR(IS_NULL, IS_STRING):
				return Z_STRLEN_P(op2) == 0 ? 0 : -1;

			case COLOPL_BC_TYPE_PAIR(IS_STRING, IS_NULL):
				return Z_STRLEN_P(op1) == 0 ? 0 : 1;

			case COLOPL_BC_TYPE_PAIR(IS_OBJECT, IS_NULL):
				return 1;

			case COLOPL_BC_TYPE_PAIR(IS_NULL, IS_OBJECT):
				return -1;

			default:
				if (Z_ISREF_P(op1)) {
					op1 = Z_REFVAL_P(op1);
					continue;
				} else if (Z_ISREF_P(op2)) {
					op2 = Z_REFVAL_P(op2);
					continue;
				}

				if (Z_TYPE_P(op1) == IS_OBJECT
				 && Z_TYPE_P(op2) == IS_OBJECT
				 && Z_OBJ_P(op1) == Z_OBJ_P(op2)) {
					return 0;
				} else if (Z_TYPE_P(op1) == IS_OBJECT) {
					return Z_OBJ_HANDLER_P(op1, compare)(op1, op2);
				} else if (Z_TYPE_P(op2) == IS_OBJECT) {
					return Z_OBJ_HANDLER_P(op2, compare)(op1, op2);
				}

				if (!converted) {
					if (Z_TYPE_P(op1) < IS_TRUE) {
						return zval_is_true(op2) ? -1 : 0;
					} else if (Z_TYPE_P(op1) == IS_TRUE) {
						return zval_is_true(op2) ? 0 : 1;
					} else if (Z_TYPE_P(op2) < IS_TRUE) {
						return zval_is_true(op1) ? 1 : 0;
					} else if (Z_TYPE_P(op2) == IS_TRUE) {
						return zval_is_true(op1) ? 0 : -1;
					} else {
						op1 = _php_colopl_bc_zendi_convert_scalar_to_number_silent(op1, &op1_copy);
						op2 = _php_colopl_bc_zendi_convert_scalar_to_number_silent(op2, &op2_copy);
						if (EG(exception)) {
							return 1; /* to stop comparison of arrays */
						}
						converted = 1;
					}
				} else if (Z_TYPE_P(op1)==IS_ARRAY) {
					return 1;
				} else if (Z_TYPE_P(op2)==IS_ARRAY) {
					return -1;
				} else {
					ZEND_UNREACHABLE();
					zend_throw_error(NULL, "Unsupported operand types");
					return 1;
				}
		}
	}
}

static int legacy_compare_slow(zval *op1, zval *op2)
{
	int bc = legacy_compare_fast(op1, op2);
	int native = zend_compare(op1, op2);
	
	if (native != bc) {
		if (COLOPL_BC_G(php74_compare_mode) & COLOPL_BC_PHP74_COMPARE_MODE_LOG) {
			php_log_err_with_severity("Incompatible compare detected", LOG_NOTICE);
		}
		if (COLOPL_BC_G(php74_compare_mode) & COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED) {
			php_error_docref(NULL, E_DEPRECATED, "Incompatible compare detected");
		}
	}

	return bc;
}

PHP_INI_MH(OnUpdateCompareMode)
{
	if (OnUpdateLong(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage) == FAILURE) {
		return FAILURE;
	}

	if (COLOPL_BC_G(php74_compare_mode) <= COLOPL_BC_PHP74_COMPARE_MODE_SILENT) {
		COLOPL_BC_G(php74_compare_func) = legacy_compare_fast;
	} else {
		COLOPL_BC_G(php74_compare_func) = legacy_compare_slow;
	}

	return SUCCESS;
}

int php_colopl_bc_compare(zval *op1, zval *op2)
{
	return COLOPL_BC_G(php74_compare_func)(op1, op2);
}

static zend_always_inline bool php_colopl_bc_fast_equal_check_long(zval *op1, zval *op2)
{
	if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
		return Z_LVAL_P(op1) == Z_LVAL_P(op2);
	}
	return php_colopl_bc_compare(op1, op2) == 0;
}

static zend_always_inline bool php_colopl_bc_fast_equal_check_string(zval *op1, zval *op2)
{
	if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		return zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
	}
	return php_colopl_bc_compare(op1, op2) == 0;
}

static zend_always_inline bool php_colopl_bc_fast_equal_check_function(zval *op1, zval *op2)
{
	if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			return Z_LVAL_P(op1) == Z_LVAL_P(op2);
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			return ((double)Z_LVAL_P(op1)) == Z_DVAL_P(op2);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			return Z_DVAL_P(op1) == Z_DVAL_P(op2);
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			return Z_DVAL_P(op1) == ((double)Z_LVAL_P(op2));
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
			return zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
		}
	}
	return php_colopl_bc_compare(op1, op2) == 0;
}

/* array.c */

static int php_colopl_bc_array_key_compare(Bucket *f, Bucket *s)
{
	zend_uchar t;
	zend_long l1, l2;
	double d;

	if (f->key == NULL) {
		if (s->key == NULL) {
			return (zend_long)f->h > (zend_long)s->h ? 1 : -1;
		} else {
			l1 = (zend_long)f->h;
			t = is_numeric_string(s->key->val, s->key->len, &l2, &d, 1);
			if (t == IS_LONG) {
				/* pass */
			} else if (t == IS_DOUBLE) {
				return ZEND_NORMALIZE_BOOL((double)l1 - d);
			} else {
				l2 = 0;
			}
		}
	} else {
		if (s->key) {
			return zendi_smart_strcmp(f->key, s->key);
		} else {
			l2 = (zend_long)s->h;
			t = is_numeric_string(f->key->val, f->key->len, &l1, &d, 1);
			if (t == IS_LONG) {
				/* pass */
			} else if (t == IS_DOUBLE) {
				return ZEND_NORMALIZE_BOOL(d - (double)l2);
			} else {
				l1 = 0;
			}
		}
	}
	return ZEND_NORMALIZE_BOOL(l1 - l2);
}

static int php_colopl_bc_array_reverse_key_compare(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare(b, a);
}

static int php_colopl_bc_array_key_compare_numeric(Bucket *f, Bucket *s)
{
	if (f->key == NULL && s->key == NULL) {
		return (zend_long)f->h > (zend_long)s->h ? 1 : -1;
	} else {
		double d1, d2;
		if (f->key) {
			d1 = zend_strtod(f->key->val, NULL);
		} else {
			d1 = (double)(zend_long)f->h;
		}
		if (s->key) {
			d2 = zend_strtod(s->key->val, NULL);
		} else {
			d2 = (double)(zend_long)s->h;
		}
		return ZEND_NORMALIZE_BOOL(d1 - d2);
	}
}

static int php_colopl_bc_array_reverse_key_compare_numeric(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_numeric(b, a);
}

static int php_colopl_bc_array_key_compare_string_case(Bucket *f, Bucket *s)
{
	const char *s1, *s2;
	size_t l1, l2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
		l1 = f->key->len;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
		l1 = buf1 + sizeof(buf1) - 1 - s1;
	}
	if (s->key) {
		s2 = s->key->val;
		l2 = s->key->len;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
		l2 = buf2 + sizeof(buf2) - 1 - s1;
	}
	return zend_binary_strcasecmp_l(s1, l1, s2, l2);
}

static int php_colopl_bc_array_reverse_key_compare_string_case(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_string_case(b, a);
}

static int php_colopl_bc_array_key_compare_string(Bucket *f, Bucket *s)
{
	const char *s1, *s2;
	size_t l1, l2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
		l1 = f->key->len;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
		l1 = buf1 + sizeof(buf1) - 1 - s1;
	}
	if (s->key) {
		s2 = s->key->val;
		l2 = s->key->len;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
		l2 = buf2 + sizeof(buf2) - 1 - s2;
	}
	return zend_binary_strcmp(s1, l1, s2, l2);
}

static int php_colopl_bc_array_reverse_key_compare_string(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_string(b, a);
}

static int php_colopl_bc_array_key_compare_string_natural_general(Bucket *f, Bucket *s, int fold_case)
{
	const char *s1, *s2;
	size_t l1, l2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
		l1 = f->key->len;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
		l1 = buf1 + sizeof(buf1) - 1 - s1;
	}
	if (s->key) {
		s2 = s->key->val;
		l2 = s->key->len;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
		l2 = buf2 + sizeof(buf2) - 1 - s1;
	}
	return strnatcmp_ex(s1, l1, s2, l2, fold_case);
}

static int php_colopl_bc_array_key_compare_string_natural_case(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_string_natural_general(a, b, 1);
}

static int php_colopl_bc_array_reverse_key_compare_string_natural_case(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_string_natural_general(b, a, 1);
}

static int php_colopl_bc_array_key_compare_string_natural(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_string_natural_general(a, b, 0);
}

static int php_colopl_bc_array_reverse_key_compare_string_natural(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_string_natural_general(b, a, 0);
}

static int php_colopl_bc_array_key_compare_string_locale(Bucket *f, Bucket *s)
{
	const char *s1, *s2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
	}
	if (s->key) {
		s2 = s->key->val;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
	}
	return strcoll(s1, s2);
}

static int php_colopl_bc_array_reverse_key_compare_string_locale(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_key_compare_string_locale(b, a);
}

static int php_colopl_bc_array_data_compare(Bucket *f, Bucket *s)
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return php_colopl_bc_compare(first, second);
}

static int php_colopl_bc_array_reverse_data_compare(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_data_compare(a, b) * -1;
}

static int php_colopl_bc_array_data_compare_numeric(Bucket *f, Bucket *s)
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return numeric_compare_function(first, second);
}

static int php_colopl_bc_array_reverse_data_compare_numeric(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_data_compare_numeric(b, a);
}

static int php_colopl_bc_array_data_compare_string_case(Bucket *f, Bucket *s)
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return string_case_compare_function(first, second);
}

static int php_colopl_bc_array_reverse_data_compare_string_case(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_data_compare_string_case(b, a);
}

static int php_colopl_bc_array_data_compare_string(Bucket *f, Bucket *s)
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return string_compare_function(first, second);
}

static int php_colopl_bc_array_reverse_data_compare_string(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_data_compare_string(b, a);
}

static int php_colopl_bc_array_natural_general_compare(Bucket *f, Bucket *s, int fold_case)
{
	zend_string *tmp_str1, *tmp_str2;
	zend_string *str1 = zval_get_tmp_string(&f->val, &tmp_str1);
	zend_string *str2 = zval_get_tmp_string(&s->val, &tmp_str2);

	int result = strnatcmp_ex(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2), fold_case);

	zend_tmp_string_release(tmp_str1);
	zend_tmp_string_release(tmp_str2);
	return result;
}

static int php_colopl_bc_array_natural_compare(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_natural_general_compare(a, b, 0);
}

static int php_colopl_bc_array_reverse_natural_compare(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_natural_general_compare(b, a, 0);
}

static int php_colopl_bc_array_natural_case_compare(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_natural_general_compare(a, b, 1);
}

static int php_colopl_bc_array_reverse_natural_case_compare(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_natural_general_compare(b, a, 1);
}

static int php_colopl_bc_array_data_compare_string_locale(Bucket *f, Bucket *s)
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return string_locale_compare_function(first, second);
}

static int php_colopl_bc_array_reverse_data_compare_string_locale(Bucket *a, Bucket *b)
{
	return php_colopl_bc_array_data_compare_string_locale(b, a);
}

static int php_colopl_bc_array_user_compare(Bucket *f, Bucket *s)
{
	zval args[2];
	zval retval;

	ZVAL_COPY(&args[0], &f->val);
	ZVAL_COPY(&args[1], &s->val);

	COLOPL_BC_G(user_compare_fci).param_count = 2;
	COLOPL_BC_G(user_compare_fci).params = args;
	COLOPL_BC_G(user_compare_fci).retval = &retval;
	if (zend_call_function(&COLOPL_BC_G(user_compare_fci), &COLOPL_BC_G(user_compare_fci_cache)) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		zend_long ret = zval_get_long(&retval);
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&args[1]);
		zval_ptr_dtor(&args[0]);
		return ZEND_NORMALIZE_BOOL(ret);
	} else {
		zval_ptr_dtor(&args[1]);
		zval_ptr_dtor(&args[0]);
		return 0;
	}
}

static int php_colopl_bc_array_user_key_compare(Bucket *f, Bucket *s)
{
	zval args[2];
	zval retval;
	zend_long result;

	if (f->key == NULL) {
		ZVAL_LONG(&args[0], f->h);
	} else {
		ZVAL_STR_COPY(&args[0], f->key);
	}
	if (s->key == NULL) {
		ZVAL_LONG(&args[1], s->h);
	} else {
		ZVAL_STR_COPY(&args[1], s->key);
	}

	COLOPL_BC_G(user_compare_fci).param_count = 2;
	COLOPL_BC_G(user_compare_fci).params = args;
	COLOPL_BC_G(user_compare_fci).retval = &retval;;
	if (zend_call_function(&COLOPL_BC_G(user_compare_fci), &COLOPL_BC_G(user_compare_fci_cache)) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		result = zval_get_long(&retval);
		zval_ptr_dtor(&retval);
	} else {
		result = 0;
	}

	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&args[1]);

	return ZEND_NORMALIZE_BOOL(result);
}

static bucket_compare_func_t php_colopl_bc_get_key_compare_func(zend_long sort_type, int reverse)
{
	switch (sort_type & ~PHP_SORT_FLAG_CASE) {
		case PHP_SORT_NUMERIC:
			if (reverse) {
				return php_colopl_bc_array_reverse_key_compare_numeric;
			} else {
				return php_colopl_bc_array_key_compare_numeric;
			}
			break;

		case PHP_SORT_STRING:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_colopl_bc_array_reverse_key_compare_string_case;
				} else {
					return php_colopl_bc_array_key_compare_string_case;
				}
			} else {
				if (reverse) {
					return php_colopl_bc_array_reverse_key_compare_string;
				} else {
					return php_colopl_bc_array_key_compare_string;
				}
			}
			break;

		case PHP_SORT_NATURAL:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_colopl_bc_array_reverse_key_compare_string_natural_case;
				} else {
					return php_colopl_bc_array_key_compare_string_natural_case;
				}
			} else {
				if (reverse) {
					return php_colopl_bc_array_reverse_key_compare_string_natural;
				} else {
					return php_colopl_bc_array_key_compare_string_natural;
				}
			}
			break;

		case PHP_SORT_LOCALE_STRING:
			if (reverse) {
				return php_colopl_bc_array_reverse_key_compare_string_locale;
			} else {
				return php_colopl_bc_array_key_compare_string_locale;
			}
			break;

		case PHP_SORT_REGULAR:
		default:
			if (reverse) {
				return php_colopl_bc_array_reverse_key_compare;
			} else {
				return php_colopl_bc_array_key_compare;
			}
			break;
	}
	return NULL;
}

static bucket_compare_func_t php_colopl_bc_get_data_compare_func(zend_long sort_type, int reverse)
{
	switch (sort_type & ~PHP_SORT_FLAG_CASE) {
		case PHP_SORT_NUMERIC:
			if (reverse) {
				return php_colopl_bc_array_reverse_data_compare_numeric;
			} else {
				return php_colopl_bc_array_data_compare_numeric;
			}
			break;

		case PHP_SORT_STRING:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_colopl_bc_array_reverse_data_compare_string_case;
				} else {
					return php_colopl_bc_array_data_compare_string_case;
				}
			} else {
				if (reverse) {
					return php_colopl_bc_array_reverse_data_compare_string;
				} else {
					return php_colopl_bc_array_data_compare_string;
				}
			}
			break;

		case PHP_SORT_NATURAL:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_colopl_bc_array_reverse_natural_case_compare;
				} else {
					return php_colopl_bc_array_natural_case_compare;
				}
			} else {
				if (reverse) {
					return php_colopl_bc_array_reverse_natural_compare;
				} else {
					return php_colopl_bc_array_natural_compare;
				}
			}
			break;

		case PHP_SORT_LOCALE_STRING:
			if (reverse) {
				return php_colopl_bc_array_reverse_data_compare_string_locale;
			} else {
				return php_colopl_bc_array_data_compare_string_locale;
			}
			break;

		case PHP_SORT_REGULAR:
		default:
			if (reverse) {
				return php_colopl_bc_array_reverse_data_compare;
			} else {
				return php_colopl_bc_array_data_compare;
			}
			break;
	}
	return NULL;
}

#define COLOPL_BC_MULTISORT_ORDER	0
#define COLOPL_BC_MULTISORT_TYPE	1
#define COLOPL_BC_MULTISORT_LAST	2

int php_colopl_bc_multisort_compare(const void *a, const void *b)
{
	Bucket *ab = *(Bucket **)a;
	Bucket *bb = *(Bucket **)b;
	int r;
	zend_long result;

	r = 0;
	do {
		result = COLOPL_BC_G(multisort_func)[r](&ab[r], &bb[r]);
		if (result != 0) {
			return result > 0 ? 1 : -1;
		}
		r++;
	} while (Z_TYPE(ab[r].val) != IS_UNDEF);

	return 0;
}

#define COLOPL_BC_MULTISORT_ABORT	\
	efree(func);	\
	efree(arrays);	\
	return;

static void colopl_bc_array_bucket_p_swap(void *p, void *q)
{
	Bucket *t;
	Bucket **f = (Bucket **)p;
	Bucket **g = (Bucket **)q;

	t = *f;
	*f = *g;
	*g = t;
}

static inline void php_colopl_bc_search_array(INTERNAL_FUNCTION_PARAMETERS, int behavior)
{
	zval *value,				/* value to check for */
		 *array,				/* array to check in */
		 *entry;				/* pointer to array entry */
	zend_ulong num_idx;
	zend_string *str_idx;
	bool strict = 0;			/* strict comparison or not */

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ZVAL(value)
		Z_PARAM_ARRAY(array)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(strict)
	ZEND_PARSE_PARAMETERS_END();

	if (strict) {
		if (Z_TYPE_P(value) == IS_LONG) {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				ZVAL_DEREF(entry);
				if (Z_TYPE_P(entry) == IS_LONG && Z_LVAL_P(entry) == Z_LVAL_P(value)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETURN_STR_COPY(str_idx);
						} else {
							RETURN_LONG(num_idx);
						}
					}
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				ZVAL_DEREF(entry);
				if (fast_is_identical_function(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETURN_STR_COPY(str_idx);
						} else {
							RETURN_LONG(num_idx);
						}
					}
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		if (Z_TYPE_P(value) == IS_LONG) {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				if (php_colopl_bc_fast_equal_check_long(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETURN_STR_COPY(str_idx);
						} else {
							RETURN_LONG(num_idx);
						}
					}
				}
			} ZEND_HASH_FOREACH_END();
		} else if (Z_TYPE_P(value) == IS_STRING) {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				if (php_colopl_bc_fast_equal_check_string(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETURN_STR_COPY(str_idx);
						} else {
							RETURN_LONG(num_idx);
						}
					}
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				if (php_colopl_bc_fast_equal_check_function(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETURN_STR_COPY(str_idx);
						} else {
							RETURN_LONG(num_idx);
						}
					}
				}
			} ZEND_HASH_FOREACH_END();
		}
	}

	RETURN_FALSE;
}

#define PHP_COLOPL_BC_ARRAY_CMP_FUNC_VARS \
	zend_fcall_info old_user_compare_fci; \
	zend_fcall_info_cache old_user_compare_fci_cache \

#define PHP_COLOPL_BC_ARRAY_CMP_FUNC_BACKUP() \
	old_user_compare_fci = COLOPL_BC_G(user_compare_fci); \
	old_user_compare_fci_cache = COLOPL_BC_G(user_compare_fci_cache); \
	COLOPL_BC_G(user_compare_fci_cache) = empty_fcall_info_cache; \

#define PHP_COLOPL_BC_ARRAY_CMP_FUNC_RESTORE() \
	zend_release_fcall_info_cache(&COLOPL_BC_G(user_compare_fci_cache)); \
	COLOPL_BC_G(user_compare_fci) = old_user_compare_fci; \
	COLOPL_BC_G(user_compare_fci_cache) = old_user_compare_fci_cache; \

static void legacy_hash_sort_fast(INTERNAL_FUNCTION_PARAMETERS, zval *array, bucket_compare_func_t comapre_func, bool renumber)
{
	zend_hash_sort(Z_ARRVAL_P(array), comapre_func, renumber);
}

static void legacy_hash_sort_slow(INTERNAL_FUNCTION_PARAMETERS, zval *array, bucket_compare_func_t compare_func, bool renumber)
{
	zval legacy;
	zend_internal_function *fn;
	char *fnname_ptr;

	ZVAL_DUP(&legacy, array);

	legacy_hash_sort_fast(INTERNAL_FUNCTION_PARAM_PASSTHRU, &legacy, compare_func, renumber);

	/* Get native PHP bundled (internal) function ptr */
	fnname_ptr = execute_data->func->internal_function.function_name->val;
	/* 
	 * Extracting last piece of namespaces, they're always identical a native function name
	 * 
	 * e.g.)
	 * Colopl\ColoplBc\Php74\*****
	 *                                     ^^^^^
	 */
	while (strchr(fnname_ptr, '\\')) {
		++fnname_ptr;
	}
	fn = zend_hash_str_find_ptr(EG(function_table), fnname_ptr, strlen(fnname_ptr));
	if (EXPECTED(fn != NULL)) {
		/* Passthrough PHP bundled (internal) function */
		fn->handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

		if (!zend_is_identical(&legacy, array)) {
			if (COLOPL_BC_G(php74_sort_mode) & COLOPL_BC_PHP74_SORT_MODE_LOG) {
				php_log_err_with_severity("Incompatible sort detected", LOG_NOTICE);
			}
			if (COLOPL_BC_G(php74_sort_mode) & COLOPL_BC_PHP74_SORT_MODE_DEPRECATED) {
				php_error_docref(NULL, E_DEPRECATED, "Incompatible sort detected");
			}
		}
	}

	zval_ptr_dtor(array);
	ZVAL_ARR(array, Z_ARRVAL(legacy));
}

PHP_INI_MH(OnUpdateSortMode)
{
	if (OnUpdateLong(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage) == FAILURE) {
		return FAILURE;
	}

	if (COLOPL_BC_G(php74_sort_mode) <= 0) {
		COLOPL_BC_G(php74_hash_sort_func) = legacy_hash_sort_fast;
	} else {
		COLOPL_BC_G(php74_hash_sort_func) = legacy_hash_sort_slow;
	}

	return SUCCESS;
}

static void php_colopl_bc_usort(INTERNAL_FUNCTION_PARAMETERS, bucket_compare_func_t compare_func, zend_bool renumber)
{
	zval *array, arr;
	PHP_COLOPL_BC_ARRAY_CMP_FUNC_VARS;

	PHP_COLOPL_BC_ARRAY_CMP_FUNC_BACKUP();

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_EX2(array, 0, 1, 0)
		Z_PARAM_FUNC(COLOPL_BC_G(user_compare_fci), COLOPL_BC_G(user_compare_fci_cache))
	ZEND_PARSE_PARAMETERS_END_EX( PHP_COLOPL_BC_ARRAY_CMP_FUNC_RESTORE(); return );

	/* Copy array, so the in-place modifications will not be visible to the callback function */
	ZVAL_DUP(&arr, array);
	if (zend_hash_num_elements(Z_ARRVAL(arr)) == 0) {
		PHP_COLOPL_BC_ARRAY_CMP_FUNC_RESTORE();
		RETURN_TRUE;
	}
	
	COLOPL_BC_G(php74_hash_sort_func)(INTERNAL_FUNCTION_PARAM_PASSTHRU, &arr, compare_func, renumber);

	zval_ptr_dtor(array);
	ZVAL_ARR(array, Z_ARRVAL(arr));

	PHP_COLOPL_BC_ARRAY_CMP_FUNC_RESTORE();
	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_ksort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_colopl_bc_get_key_compare_func(sort_type, 0);

	COLOPL_BC_G(php74_hash_sort_func)(INTERNAL_FUNCTION_PARAM_PASSTHRU, array, cmp, 0);

	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_krsort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_colopl_bc_get_key_compare_func(sort_type, 1);

	COLOPL_BC_G(php74_hash_sort_func)(INTERNAL_FUNCTION_PARAM_PASSTHRU, array, cmp, 0);

	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_asort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_colopl_bc_get_data_compare_func(sort_type, 0);

	COLOPL_BC_G(php74_hash_sort_func)(INTERNAL_FUNCTION_PARAM_PASSTHRU, array, cmp, 0);

	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_arsort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_colopl_bc_get_data_compare_func(sort_type, 1);

	COLOPL_BC_G(php74_hash_sort_func)(INTERNAL_FUNCTION_PARAM_PASSTHRU, array, cmp, 0);

	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_sort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_colopl_bc_get_data_compare_func(sort_type, 0);

	COLOPL_BC_G(php74_hash_sort_func)(INTERNAL_FUNCTION_PARAM_PASSTHRU, array, cmp, 1);

	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_rsort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_colopl_bc_get_data_compare_func(sort_type, 1);

	COLOPL_BC_G(php74_hash_sort_func)(INTERNAL_FUNCTION_PARAM_PASSTHRU, array, cmp, 1);

	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_usort)
{
	php_colopl_bc_usort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_colopl_bc_array_user_compare, 1);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_uasort)
{
	php_colopl_bc_usort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_colopl_bc_array_user_compare, 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_uksort)
{
	php_colopl_bc_usort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_colopl_bc_array_user_key_compare, 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_in_array)
{
	php_colopl_bc_search_array(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_array_search)
{
	php_colopl_bc_search_array(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_array_keys)
{
	zval *input,				/* Input array */
		 *search_value = NULL,	/* Value to search for */
		 *entry,				/* An entry in the input array */
		 new_val;				/* New value */
	bool strict = 0;			/* do strict comparison */
	zend_ulong num_idx;
	zend_string *str_idx;
	zend_array *arrval;
	zend_ulong elem_count;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ARRAY(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(search_value)
		Z_PARAM_BOOL(strict)
	ZEND_PARSE_PARAMETERS_END();
	arrval = Z_ARRVAL_P(input);
	elem_count = zend_hash_num_elements(arrval);

	/* Base case: empty input */
	if (!elem_count) {
		RETURN_COPY(input);
	}

	/* Initialize return array */
	if (search_value != NULL) {
		array_init(return_value);

		if (strict) {
			ZEND_HASH_FOREACH_KEY_VAL(arrval, num_idx, str_idx, entry) {
				ZVAL_DEREF(entry);
				if (fast_is_identical_function(search_value, entry)) {
					if (str_idx) {
						ZVAL_STR_COPY(&new_val, str_idx);
					} else {
						ZVAL_LONG(&new_val, num_idx);
					}
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &new_val);
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_KEY_VAL(arrval, num_idx, str_idx, entry) {
				if (php_colopl_bc_fast_equal_check_function(search_value, entry)) {
					if (str_idx) {
						ZVAL_STR_COPY(&new_val, str_idx);
					} else {
						ZVAL_LONG(&new_val, num_idx);
					}
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &new_val);
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		array_init_size(return_value, elem_count);
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
			if (HT_IS_PACKED(arrval) && HT_IS_WITHOUT_HOLES(arrval)) {
				/* Optimistic case: range(0..n-1) for vector-like packed array */
				zend_ulong lval = 0;

				for (; lval < elem_count; ++lval) {
					ZEND_HASH_FILL_SET_LONG(lval);
					ZEND_HASH_FILL_NEXT();
				}
			} else {
				/* Go through input array and add keys to the return array */
				ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(input), num_idx, str_idx, entry) {
					if (str_idx) {
						ZEND_HASH_FILL_SET_STR_COPY(str_idx);
					} else {
						ZEND_HASH_FILL_SET_LONG(num_idx);
					}
					ZEND_HASH_FILL_NEXT();
				} ZEND_HASH_FOREACH_END();
			}
		} ZEND_HASH_FILL_END();
	}
}

#if PHP_VERSION_ID < 80200
PHP_FUNCTION(Colopl_ColoplBc_Php74_array_multisort)
{
	zval*			args;
	zval**			arrays;
	Bucket**		indirect;
	uint32_t		idx;
	Bucket*			p;
	HashTable*		hash;
	int				argc;
	int				array_size;
	int				num_arrays = 0;
	int				parse_state[COLOPL_BC_MULTISORT_LAST]; /* 0 - flag not allowed 1 - flag allowed */
	int				sort_order = PHP_SORT_ASC;
	int				sort_type = PHP_SORT_REGULAR;
	int				i, k, n;
	bucket_compare_func_t *func;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* Allocate space for storing pointers to input arrays and sort flags. */
	arrays = (zval **)ecalloc(argc, sizeof(zval *));
	for (i = 0; i < COLOPL_BC_MULTISORT_LAST; i++) {
		parse_state[i] = 0;
	}
	func = COLOPL_BC_G(multisort_func) = ecalloc(argc, sizeof(bucket_compare_func_t));

	/* Here we go through the input arguments and parse them. Each one can
	 * be either an array or a sort flag which follows an array. If not
	 * specified, the sort flags defaults to PHP_SORT_ASC and PHP_SORT_REGULAR
	 * accordingly. There can't be two sort flags of the same type after an
	 * array, and the very first argument has to be an array. */
	for (i = 0; i < argc; i++) {
		zval *arg = &args[i];

		ZVAL_DEREF(arg);
		if (Z_TYPE_P(arg) == IS_ARRAY) {
			SEPARATE_ARRAY(arg);
			/* We see the next array, so we update the sort flags of
			 * the previous array and reset the sort flags. */
			if (i > 0) {
				COLOPL_BC_G(multisort_func)[num_arrays - 1] = php_colopl_bc_get_data_compare_func(sort_type, sort_order != PHP_SORT_ASC);
				sort_order = PHP_SORT_ASC;
				sort_type = PHP_SORT_REGULAR;
			}
			arrays[num_arrays++] = arg;

			/* Next one may be an array or a list of sort flags. */
			for (k = 0; k < COLOPL_BC_MULTISORT_LAST; k++) {
				parse_state[k] = 1;
			}
		} else if (Z_TYPE_P(arg) == IS_LONG) {
			switch (Z_LVAL_P(arg) & ~PHP_SORT_FLAG_CASE) {
				case PHP_SORT_ASC:
				case PHP_SORT_DESC:
					/* flag allowed here */
					if (parse_state[COLOPL_BC_MULTISORT_ORDER] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_order = Z_LVAL_P(arg) == PHP_SORT_DESC ? PHP_SORT_DESC : PHP_SORT_ASC;
						parse_state[COLOPL_BC_MULTISORT_ORDER] = 0;
					} else {
						zend_argument_type_error(i + 1, "must be an array or a sort flag that has not already been specified");
						COLOPL_BC_MULTISORT_ABORT;
					}
					break;

				case PHP_SORT_REGULAR:
				case PHP_SORT_NUMERIC:
				case PHP_SORT_STRING:
				case PHP_SORT_NATURAL:
				case PHP_SORT_LOCALE_STRING:
					/* flag allowed here */
					if (parse_state[COLOPL_BC_MULTISORT_TYPE] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_type = (int)Z_LVAL_P(arg);
						parse_state[COLOPL_BC_MULTISORT_TYPE] = 0;
					} else {
						zend_argument_type_error(i + 1, "must be an array or a sort flag that has not already been specified");
						COLOPL_BC_MULTISORT_ABORT;
					}
					break;

				default:
					zend_argument_value_error(i + 1, "must be a valid sort flag");
					COLOPL_BC_MULTISORT_ABORT;
					break;

			}
		} else {
			zend_argument_type_error(i + 1, "must be an array or a sort flag");
			COLOPL_BC_MULTISORT_ABORT;
		}
	}
	/* Take care of the last array sort flags. */
	COLOPL_BC_G(multisort_func)[num_arrays - 1] = php_colopl_bc_get_data_compare_func(sort_type, sort_order != PHP_SORT_ASC);

	/* Make sure the arrays are of the same size. */
	array_size = zend_hash_num_elements(Z_ARRVAL_P(arrays[0]));
	for (i = 0; i < num_arrays; i++) {
		if (zend_hash_num_elements(Z_ARRVAL_P(arrays[i])) != (uint32_t)array_size) {
			zend_value_error("Array sizes are inconsistent");
			COLOPL_BC_MULTISORT_ABORT;
		}
	}

	/* If all arrays are empty we don't need to do anything. */
	if (array_size < 1) {
		efree(func);
		efree(arrays);
		RETURN_TRUE;
	}

	/* Create the indirection array. This array is of size MxN, where
	 * M is the number of entries in each input array and N is the number
	 * of the input arrays + 1. The last column is NULL to indicate the end
	 * of the row. */
	indirect = (Bucket **)safe_emalloc(array_size, sizeof(Bucket *), 0);
	for (i = 0; i < array_size; i++) {
		indirect[i] = (Bucket *)safe_emalloc((num_arrays + 1), sizeof(Bucket), 0);
	}
	for (i = 0; i < num_arrays; i++) {
		k = 0;
		for (idx = 0; idx < Z_ARRVAL_P(arrays[i])->nNumUsed; idx++) {
			p = Z_ARRVAL_P(arrays[i])->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			indirect[k][i] = *p;
			k++;
		}
	}
	for (k = 0; k < array_size; k++) {
		ZVAL_UNDEF(&indirect[k][num_arrays].val);
	}

	/* Do the actual sort magic - bada-bim, bada-boom. */
	zend_sort(indirect, array_size, sizeof(Bucket *), php_colopl_bc_multisort_compare, (swap_func_t)colopl_bc_array_bucket_p_swap);

	/* Restructure the arrays based on sorted indirect - this is mostly taken from zend_hash_sort() function. */
	for (i = 0; i < num_arrays; i++) {
		int repack;

		hash = Z_ARRVAL_P(arrays[i]);
		hash->nNumUsed = array_size;
		hash->nInternalPointer = 0;
		repack = !(HT_FLAGS(hash) & HASH_FLAG_PACKED);

		for (n = 0, k = 0; k < array_size; k++) {
			hash->arData[k] = indirect[k][i];
			if (hash->arData[k].key == NULL) {
				hash->arData[k].h = n++;
			} else {
				repack = 0;
			}
		}
		hash->nNextFreeElement = array_size;
		if (repack) {
			zend_hash_to_packed(hash);
		} else if (!(HT_FLAGS(hash) & HASH_FLAG_PACKED)) {
			zend_hash_rehash(hash);
		}
	}

	/* Clean up. */
	for (i = 0; i < array_size; i++) {
		efree(indirect[i]);
	}
	efree(indirect);
	efree(func);
	efree(arrays);
	RETURN_TRUE;
}
#else
PHP_FUNCTION(Colopl_ColoplBc_Php74_array_multisort)
{
	zval*			args;
	zval**			arrays;
	Bucket**		indirect;
	uint32_t		idx;
	HashTable*		hash;
	int				argc;
	int				array_size;
	int				num_arrays = 0;
	int				parse_state[COLOPL_BC_MULTISORT_LAST];   /* 0 - flag not allowed 1 - flag allowed */
	int				sort_order = PHP_SORT_ASC;
	int				sort_type  = PHP_SORT_REGULAR;
	int				i, k, n;
	bucket_compare_func_t *func;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* Allocate space for storing pointers to input arrays and sort flags. */
	arrays = (zval **)ecalloc(argc, sizeof(zval *));
	for (i = 0; i < COLOPL_BC_MULTISORT_LAST; i++) {
		parse_state[i] = 0;
	}
	func = COLOPL_BC_G(multisort_func) = ecalloc(argc, sizeof(bucket_compare_func_t));

	/* Here we go through the input arguments and parse them. Each one can
	 * be either an array or a sort flag which follows an array. If not
	 * specified, the sort flags defaults to PHP_SORT_ASC and PHP_SORT_REGULAR
	 * accordingly. There can't be two sort flags of the same type after an
	 * array, and the very first argument has to be an array. */
	for (i = 0; i < argc; i++) {
		zval *arg = &args[i];

		ZVAL_DEREF(arg);
		if (Z_TYPE_P(arg) == IS_ARRAY) {
			SEPARATE_ARRAY(arg);
			/* We see the next array, so we update the sort flags of
			 * the previous array and reset the sort flags. */
			if (i > 0) {
				COLOPL_BC_G(multisort_func)[num_arrays - 1] = php_colopl_bc_get_data_compare_func(sort_type, sort_order != PHP_SORT_ASC);
				sort_order = PHP_SORT_ASC;
				sort_type = PHP_SORT_REGULAR;
			}
			arrays[num_arrays++] = arg;

			/* Next one may be an array or a list of sort flags. */
			for (k = 0; k < COLOPL_BC_MULTISORT_LAST; k++) {
				parse_state[k] = 1;
			}
		} else if (Z_TYPE_P(arg) == IS_LONG) {
			switch (Z_LVAL_P(arg) & ~PHP_SORT_FLAG_CASE) {
				case PHP_SORT_ASC:
				case PHP_SORT_DESC:
					/* flag allowed here */
					if (parse_state[COLOPL_BC_MULTISORT_ORDER] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_order = Z_LVAL_P(arg) == PHP_SORT_DESC ? PHP_SORT_DESC : PHP_SORT_ASC;
						parse_state[COLOPL_BC_MULTISORT_ORDER] = 0;
					} else {
						zend_argument_type_error(i + 1, "must be an array or a sort flag that has not already been specified");
						COLOPL_BC_MULTISORT_ABORT;
					}
					break;

				case PHP_SORT_REGULAR:
				case PHP_SORT_NUMERIC:
				case PHP_SORT_STRING:
				case PHP_SORT_NATURAL:
				case PHP_SORT_LOCALE_STRING:
					/* flag allowed here */
					if (parse_state[COLOPL_BC_MULTISORT_TYPE] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_type = (int)Z_LVAL_P(arg);
						parse_state[COLOPL_BC_MULTISORT_TYPE] = 0;
					} else {
						zend_argument_type_error(i + 1, "must be an array or a sort flag that has not already been specified");
						COLOPL_BC_MULTISORT_ABORT;
					}
					break;

				default:
					zend_argument_value_error(i + 1, "must be a valid sort flag");
					COLOPL_BC_MULTISORT_ABORT;
					break;

			}
		} else {
			zend_argument_type_error(i + 1, "must be an array or a sort flag");
			COLOPL_BC_MULTISORT_ABORT;
		}
	}
	/* Take care of the last array sort flags. */
	COLOPL_BC_G(multisort_func)[num_arrays - 1] = php_colopl_bc_get_data_compare_func(sort_type, sort_order != PHP_SORT_ASC);

	/* Make sure the arrays are of the same size. */
	array_size = zend_hash_num_elements(Z_ARRVAL_P(arrays[0]));
	for (i = 0; i < num_arrays; i++) {
		if (zend_hash_num_elements(Z_ARRVAL_P(arrays[i])) != (uint32_t)array_size) {
			zend_value_error("Array sizes are inconsistent");
			COLOPL_BC_MULTISORT_ABORT;
		}
	}

	/* If all arrays are empty we don't need to do anything. */
	if (array_size < 1) {
		efree(func);
		efree(arrays);
		RETURN_TRUE;
	}

	/* Create the indirection array. This array is of size MxN, where
	 * M is the number of entries in each input array and N is the number
	 * of the input arrays + 1. The last column is UNDEF to indicate the end
	 * of the row. It also stores the original position for stable sorting. */
	indirect = (Bucket **)safe_emalloc(array_size, sizeof(Bucket *), 0);
	for (i = 0; i < array_size; i++) {
		indirect[i] = (Bucket *)safe_emalloc((num_arrays + 1), sizeof(Bucket), 0);
	}
	for (i = 0; i < num_arrays; i++) {
		k = 0;
		if (HT_IS_PACKED(Z_ARRVAL_P(arrays[i]))) {
			zval *zv = Z_ARRVAL_P(arrays[i])->arPacked;
			for (idx = 0; idx < Z_ARRVAL_P(arrays[i])->nNumUsed; idx++, zv++) {
				if (Z_TYPE_P(zv) == IS_UNDEF) continue;
				ZVAL_COPY_VALUE(&indirect[k][i].val, zv);
				indirect[k][i].h = idx;
				indirect[k][i].key = NULL;
				k++;
			}
		} else {
			Bucket *p = Z_ARRVAL_P(arrays[i])->arData;
			for (idx = 0; idx < Z_ARRVAL_P(arrays[i])->nNumUsed; idx++, p++) {
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				indirect[k][i] = *p;
				k++;
			}
		}
	}
	for (k = 0; k < array_size; k++) {
		ZVAL_UNDEF(&indirect[k][num_arrays].val);
		Z_EXTRA_P(&indirect[k][num_arrays].val) = k;
	}

	/* Do the actual sort magic - bada-bim, bada-boom. */
	zend_sort(indirect, array_size, sizeof(Bucket *), php_colopl_bc_multisort_compare, (swap_func_t)colopl_bc_array_bucket_p_swap);

	/* Restructure the arrays based on sorted indirect - this is mostly taken from zend_hash_sort() function. */
	for (i = 0; i < num_arrays; i++) {
		hash = Z_ARRVAL_P(arrays[i]);
		hash->nNumUsed = array_size;
		hash->nNextFreeElement = array_size;
		hash->nInternalPointer = 0;
		if (HT_IS_PACKED(hash)) {
			for (k = 0; k < array_size; k++) {
				ZVAL_COPY_VALUE(&hash->arPacked[k], &indirect[k][i].val);
			}
		} else {
			int repack = 1;

			for (n = 0, k = 0; k < array_size; k++) {
				hash->arData[k] = indirect[k][i];
				if (hash->arData[k].key == NULL) {
					hash->arData[k].h = n++;
				} else {
					repack = 0;
				}
			}
			if (repack) {
				zend_hash_to_packed(hash);
			}
		}
	}

	/* Clean up. */
	for (i = 0; i < array_size; i++) {
		efree(indirect[i]);
	}
	efree(indirect);
	efree(func);
	efree(arrays);
	RETURN_TRUE;
}
#endif

/* BinaryOps */

PHP_FUNCTION(Colopl_ColoplBc_Php74_eq)
{
	zval *op1, *op2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(op1)
		Z_PARAM_ZVAL(op2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(php_colopl_bc_compare(op1, op2) == 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_neq)
{
	zval *op1, *op2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(op1)
		Z_PARAM_ZVAL(op2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(php_colopl_bc_compare(op1, op2) != 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_lt)
{
	zval *op1, *op2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(op1)
		Z_PARAM_ZVAL(op2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(php_colopl_bc_compare(op1, op2) < 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_lte)
{
	zval *op1, *op2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(op1)
		Z_PARAM_ZVAL(op2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(php_colopl_bc_compare(op1, op2) <= 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_gt)
{
	zval *op1, *op2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(op1)
		Z_PARAM_ZVAL(op2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(php_colopl_bc_compare(op2, op1) < 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_gte)
{
	zval *op1, *op2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(op1)
		Z_PARAM_ZVAL(op2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(php_colopl_bc_compare(op2, op1) <= 0);
}

PHP_FUNCTION(Colopl_ColoplBc_Php74_spaceship)
{
	zval *op1, *op2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(op1)
		Z_PARAM_ZVAL(op2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(php_colopl_bc_compare(op1, op2));
}
