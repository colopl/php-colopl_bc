/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2f08606e0f2e0bb35e523cdc2d3791be04d32611 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php70_srand, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seed, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php70_rand, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, seed, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php70_getrandmax, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php70_shuffle, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php70_str_shuffle, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_Colopl_ColoplBc_Php70_array_rand, 0, 1, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php70_mt_srand arginfo_Colopl_ColoplBc_Php70_srand

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php70_mt_rand, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php70_mt_getrandmax arginfo_Colopl_ColoplBc_Php70_getrandmax

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_Colopl_ColoplBc_Php70_date_create, 0, 0, DateTime, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, datetime, IS_STRING, 0, "\'now\'")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_Colopl_ColoplBc_Php70_date_create_immutable, 0, 0, DateTimeImmutable, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, datetime, IS_STRING, 0, "\'now\'")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, timezone, DateTimeZone, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php74_ksort, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SORT_REGULAR")
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php74_krsort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_asort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_arsort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_sort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_rsort arginfo_Colopl_ColoplBc_Php74_ksort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php74_usort, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(1, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php74_uasort arginfo_Colopl_ColoplBc_Php74_usort

#define arginfo_Colopl_ColoplBc_Php74_uksort arginfo_Colopl_ColoplBc_Php74_usort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php74_in_array, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, needle, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_Colopl_ColoplBc_Php74_array_search, 0, 2, MAY_BE_LONG|MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, needle, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, haystack, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php74_array_keys, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, array, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, filter_value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, strict, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php74_array_multisort, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, array)
	ZEND_ARG_VARIADIC_INFO(ZEND_SEND_PREFER_REF, rest)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php74_eq, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, op1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, op2, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php74_neq arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_lt arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_lte arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_gt arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_gte arginfo_Colopl_ColoplBc_Php74_eq

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Colopl_ColoplBc_Php74_spaceship, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, op1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, op2, IS_MIXED, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(Colopl_ColoplBc_Php70_srand);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_rand);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_getrandmax);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_shuffle);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_str_shuffle);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_array_rand);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_mt_srand);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_mt_rand);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_mt_getrandmax);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_date_create);
ZEND_FUNCTION(Colopl_ColoplBc_Php70_date_create_immutable);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_ksort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_krsort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_asort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_arsort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_sort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_rsort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_usort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_uasort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_uksort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_in_array);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_array_search);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_array_keys);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_array_multisort);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_eq);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_neq);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_lt);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_lte);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_gt);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_gte);
ZEND_FUNCTION(Colopl_ColoplBc_Php74_spaceship);


static const zend_function_entry ext_functions[] = {
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", srand, Colopl_ColoplBc_Php70_srand, arginfo_Colopl_ColoplBc_Php70_srand)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", rand, Colopl_ColoplBc_Php70_rand, arginfo_Colopl_ColoplBc_Php70_rand)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", getrandmax, Colopl_ColoplBc_Php70_getrandmax, arginfo_Colopl_ColoplBc_Php70_getrandmax)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", shuffle, Colopl_ColoplBc_Php70_shuffle, arginfo_Colopl_ColoplBc_Php70_shuffle)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", str_shuffle, Colopl_ColoplBc_Php70_str_shuffle, arginfo_Colopl_ColoplBc_Php70_str_shuffle)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", array_rand, Colopl_ColoplBc_Php70_array_rand, arginfo_Colopl_ColoplBc_Php70_array_rand)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", mt_srand, Colopl_ColoplBc_Php70_mt_srand, arginfo_Colopl_ColoplBc_Php70_mt_srand)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", mt_rand, Colopl_ColoplBc_Php70_mt_rand, arginfo_Colopl_ColoplBc_Php70_mt_rand)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", mt_getrandmax, Colopl_ColoplBc_Php70_mt_getrandmax, arginfo_Colopl_ColoplBc_Php70_mt_getrandmax)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", date_create, Colopl_ColoplBc_Php70_date_create, arginfo_Colopl_ColoplBc_Php70_date_create)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php70", date_create_immutable, Colopl_ColoplBc_Php70_date_create_immutable, arginfo_Colopl_ColoplBc_Php70_date_create_immutable)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", ksort, Colopl_ColoplBc_Php74_ksort, arginfo_Colopl_ColoplBc_Php74_ksort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", krsort, Colopl_ColoplBc_Php74_krsort, arginfo_Colopl_ColoplBc_Php74_krsort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", asort, Colopl_ColoplBc_Php74_asort, arginfo_Colopl_ColoplBc_Php74_asort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", arsort, Colopl_ColoplBc_Php74_arsort, arginfo_Colopl_ColoplBc_Php74_arsort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", sort, Colopl_ColoplBc_Php74_sort, arginfo_Colopl_ColoplBc_Php74_sort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", rsort, Colopl_ColoplBc_Php74_rsort, arginfo_Colopl_ColoplBc_Php74_rsort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", usort, Colopl_ColoplBc_Php74_usort, arginfo_Colopl_ColoplBc_Php74_usort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", uasort, Colopl_ColoplBc_Php74_uasort, arginfo_Colopl_ColoplBc_Php74_uasort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", uksort, Colopl_ColoplBc_Php74_uksort, arginfo_Colopl_ColoplBc_Php74_uksort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", in_array, Colopl_ColoplBc_Php74_in_array, arginfo_Colopl_ColoplBc_Php74_in_array)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", array_search, Colopl_ColoplBc_Php74_array_search, arginfo_Colopl_ColoplBc_Php74_array_search)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", array_keys, Colopl_ColoplBc_Php74_array_keys, arginfo_Colopl_ColoplBc_Php74_array_keys)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", array_multisort, Colopl_ColoplBc_Php74_array_multisort, arginfo_Colopl_ColoplBc_Php74_array_multisort)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", eq, Colopl_ColoplBc_Php74_eq, arginfo_Colopl_ColoplBc_Php74_eq)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", neq, Colopl_ColoplBc_Php74_neq, arginfo_Colopl_ColoplBc_Php74_neq)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", lt, Colopl_ColoplBc_Php74_lt, arginfo_Colopl_ColoplBc_Php74_lt)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", lte, Colopl_ColoplBc_Php74_lte, arginfo_Colopl_ColoplBc_Php74_lte)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", gt, Colopl_ColoplBc_Php74_gt, arginfo_Colopl_ColoplBc_Php74_gt)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", gte, Colopl_ColoplBc_Php74_gte, arginfo_Colopl_ColoplBc_Php74_gte)
	ZEND_NS_FALIAS("Colopl\\ColoplBc\\Php74", spaceship, Colopl_ColoplBc_Php74_spaceship, arginfo_Colopl_ColoplBc_Php74_spaceship)
	ZEND_FE_END
};

static void register_colopl_bc_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("COLOPL_BC_PHP74_COMPARE_MODE_SILENT", COLOPL_BC_PHP74_COMPARE_MODE_SILENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED", COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COLOPL_BC_PHP74_COMPARE_MODE_LOG", COLOPL_BC_PHP74_COMPARE_MODE_LOG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COLOPL_BC_PHP74_SORT_MODE_SILENT", COLOPL_BC_PHP74_SORT_MODE_SILENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COLOPL_BC_PHP74_SORT_MODE_DEPRECATED", COLOPL_BC_PHP74_SORT_MODE_DEPRECATED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COLOPL_BC_PHP74_SORT_MODE_LOG", COLOPL_BC_PHP74_SORT_MODE_LOG, CONST_PERSISTENT);
}
