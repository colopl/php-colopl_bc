/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c33b258c86782e7f6799e12286c59e9cc2e8319a */

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_srand, 0, 0, 0)
	ZEND_ARG_INFO(0, seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_rand, 0, 0, 0)
	ZEND_ARG_INFO(0, seed)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_getrandmax, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_shuffle, 0, 0, 1)
	ZEND_ARG_INFO(1, array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_str_shuffle, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_array_rand, 0, 0, 1)
	ZEND_ARG_INFO(0, array)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php70_mt_srand arginfo_Colopl_ColoplBc_Php70_srand

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_mt_rand, 0, 0, 0)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php70_mt_getrandmax arginfo_Colopl_ColoplBc_Php70_getrandmax

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php70_date_create, 0, 0, 0)
	ZEND_ARG_INFO(0, datetime)
	ZEND_ARG_INFO(0, timezone)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php70_date_create_immutable arginfo_Colopl_ColoplBc_Php70_date_create

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php74_ksort, 0, 0, 1)
	ZEND_ARG_INFO(1, array)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php74_krsort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_asort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_arsort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_sort arginfo_Colopl_ColoplBc_Php74_ksort

#define arginfo_Colopl_ColoplBc_Php74_rsort arginfo_Colopl_ColoplBc_Php74_ksort

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php74_usort, 0, 0, 2)
	ZEND_ARG_INFO(1, array)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php74_uasort arginfo_Colopl_ColoplBc_Php74_usort

#define arginfo_Colopl_ColoplBc_Php74_uksort arginfo_Colopl_ColoplBc_Php74_usort

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php74_in_array, 0, 0, 2)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php74_array_search arginfo_Colopl_ColoplBc_Php74_in_array

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php74_array_keys, 0, 0, 1)
	ZEND_ARG_INFO(0, array)
	ZEND_ARG_INFO(0, filter_value)
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php74_array_multisort, 0, 0, 1)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, array)
	ZEND_ARG_VARIADIC_INFO(ZEND_SEND_PREFER_REF, rest)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_Colopl_ColoplBc_Php74_eq, 0, 0, 2)
	ZEND_ARG_INFO(0, op1)
	ZEND_ARG_INFO(0, op2)
ZEND_END_ARG_INFO()

#define arginfo_Colopl_ColoplBc_Php74_neq arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_lt arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_lte arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_gt arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_gte arginfo_Colopl_ColoplBc_Php74_eq

#define arginfo_Colopl_ColoplBc_Php74_spaceship arginfo_Colopl_ColoplBc_Php74_eq

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
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "srand"), zif_Colopl_ColoplBc_Php70_srand, arginfo_Colopl_ColoplBc_Php70_srand, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "srand"), zif_Colopl_ColoplBc_Php70_srand, arginfo_Colopl_ColoplBc_Php70_srand, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "rand"), zif_Colopl_ColoplBc_Php70_rand, arginfo_Colopl_ColoplBc_Php70_rand, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "rand"), zif_Colopl_ColoplBc_Php70_rand, arginfo_Colopl_ColoplBc_Php70_rand, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "getrandmax"), zif_Colopl_ColoplBc_Php70_getrandmax, arginfo_Colopl_ColoplBc_Php70_getrandmax, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "getrandmax"), zif_Colopl_ColoplBc_Php70_getrandmax, arginfo_Colopl_ColoplBc_Php70_getrandmax, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "shuffle"), zif_Colopl_ColoplBc_Php70_shuffle, arginfo_Colopl_ColoplBc_Php70_shuffle, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "shuffle"), zif_Colopl_ColoplBc_Php70_shuffle, arginfo_Colopl_ColoplBc_Php70_shuffle, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "str_shuffle"), zif_Colopl_ColoplBc_Php70_str_shuffle, arginfo_Colopl_ColoplBc_Php70_str_shuffle, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "str_shuffle"), zif_Colopl_ColoplBc_Php70_str_shuffle, arginfo_Colopl_ColoplBc_Php70_str_shuffle, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "array_rand"), zif_Colopl_ColoplBc_Php70_array_rand, arginfo_Colopl_ColoplBc_Php70_array_rand, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "array_rand"), zif_Colopl_ColoplBc_Php70_array_rand, arginfo_Colopl_ColoplBc_Php70_array_rand, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "mt_srand"), zif_Colopl_ColoplBc_Php70_mt_srand, arginfo_Colopl_ColoplBc_Php70_mt_srand, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "mt_srand"), zif_Colopl_ColoplBc_Php70_mt_srand, arginfo_Colopl_ColoplBc_Php70_mt_srand, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "mt_rand"), zif_Colopl_ColoplBc_Php70_mt_rand, arginfo_Colopl_ColoplBc_Php70_mt_rand, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "mt_rand"), zif_Colopl_ColoplBc_Php70_mt_rand, arginfo_Colopl_ColoplBc_Php70_mt_rand, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "mt_getrandmax"), zif_Colopl_ColoplBc_Php70_mt_getrandmax, arginfo_Colopl_ColoplBc_Php70_mt_getrandmax, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "mt_getrandmax"), zif_Colopl_ColoplBc_Php70_mt_getrandmax, arginfo_Colopl_ColoplBc_Php70_mt_getrandmax, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "date_create"), zif_Colopl_ColoplBc_Php70_date_create, arginfo_Colopl_ColoplBc_Php70_date_create, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "date_create"), zif_Colopl_ColoplBc_Php70_date_create, arginfo_Colopl_ColoplBc_Php70_date_create, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "date_create_immutable"), zif_Colopl_ColoplBc_Php70_date_create_immutable, arginfo_Colopl_ColoplBc_Php70_date_create_immutable, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php70", "date_create_immutable"), zif_Colopl_ColoplBc_Php70_date_create_immutable, arginfo_Colopl_ColoplBc_Php70_date_create_immutable, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "ksort"), zif_Colopl_ColoplBc_Php74_ksort, arginfo_Colopl_ColoplBc_Php74_ksort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "ksort"), zif_Colopl_ColoplBc_Php74_ksort, arginfo_Colopl_ColoplBc_Php74_ksort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "krsort"), zif_Colopl_ColoplBc_Php74_krsort, arginfo_Colopl_ColoplBc_Php74_krsort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "krsort"), zif_Colopl_ColoplBc_Php74_krsort, arginfo_Colopl_ColoplBc_Php74_krsort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "asort"), zif_Colopl_ColoplBc_Php74_asort, arginfo_Colopl_ColoplBc_Php74_asort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "asort"), zif_Colopl_ColoplBc_Php74_asort, arginfo_Colopl_ColoplBc_Php74_asort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "arsort"), zif_Colopl_ColoplBc_Php74_arsort, arginfo_Colopl_ColoplBc_Php74_arsort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "arsort"), zif_Colopl_ColoplBc_Php74_arsort, arginfo_Colopl_ColoplBc_Php74_arsort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "sort"), zif_Colopl_ColoplBc_Php74_sort, arginfo_Colopl_ColoplBc_Php74_sort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "sort"), zif_Colopl_ColoplBc_Php74_sort, arginfo_Colopl_ColoplBc_Php74_sort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "rsort"), zif_Colopl_ColoplBc_Php74_rsort, arginfo_Colopl_ColoplBc_Php74_rsort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "rsort"), zif_Colopl_ColoplBc_Php74_rsort, arginfo_Colopl_ColoplBc_Php74_rsort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "usort"), zif_Colopl_ColoplBc_Php74_usort, arginfo_Colopl_ColoplBc_Php74_usort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "usort"), zif_Colopl_ColoplBc_Php74_usort, arginfo_Colopl_ColoplBc_Php74_usort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "uasort"), zif_Colopl_ColoplBc_Php74_uasort, arginfo_Colopl_ColoplBc_Php74_uasort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "uasort"), zif_Colopl_ColoplBc_Php74_uasort, arginfo_Colopl_ColoplBc_Php74_uasort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "uksort"), zif_Colopl_ColoplBc_Php74_uksort, arginfo_Colopl_ColoplBc_Php74_uksort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "uksort"), zif_Colopl_ColoplBc_Php74_uksort, arginfo_Colopl_ColoplBc_Php74_uksort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "in_array"), zif_Colopl_ColoplBc_Php74_in_array, arginfo_Colopl_ColoplBc_Php74_in_array, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "in_array"), zif_Colopl_ColoplBc_Php74_in_array, arginfo_Colopl_ColoplBc_Php74_in_array, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "array_search"), zif_Colopl_ColoplBc_Php74_array_search, arginfo_Colopl_ColoplBc_Php74_array_search, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "array_search"), zif_Colopl_ColoplBc_Php74_array_search, arginfo_Colopl_ColoplBc_Php74_array_search, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "array_keys"), zif_Colopl_ColoplBc_Php74_array_keys, arginfo_Colopl_ColoplBc_Php74_array_keys, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "array_keys"), zif_Colopl_ColoplBc_Php74_array_keys, arginfo_Colopl_ColoplBc_Php74_array_keys, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "array_multisort"), zif_Colopl_ColoplBc_Php74_array_multisort, arginfo_Colopl_ColoplBc_Php74_array_multisort, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "array_multisort"), zif_Colopl_ColoplBc_Php74_array_multisort, arginfo_Colopl_ColoplBc_Php74_array_multisort, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "eq"), zif_Colopl_ColoplBc_Php74_eq, arginfo_Colopl_ColoplBc_Php74_eq, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "eq"), zif_Colopl_ColoplBc_Php74_eq, arginfo_Colopl_ColoplBc_Php74_eq, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "neq"), zif_Colopl_ColoplBc_Php74_neq, arginfo_Colopl_ColoplBc_Php74_neq, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "neq"), zif_Colopl_ColoplBc_Php74_neq, arginfo_Colopl_ColoplBc_Php74_neq, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "lt"), zif_Colopl_ColoplBc_Php74_lt, arginfo_Colopl_ColoplBc_Php74_lt, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "lt"), zif_Colopl_ColoplBc_Php74_lt, arginfo_Colopl_ColoplBc_Php74_lt, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "lte"), zif_Colopl_ColoplBc_Php74_lte, arginfo_Colopl_ColoplBc_Php74_lte, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "lte"), zif_Colopl_ColoplBc_Php74_lte, arginfo_Colopl_ColoplBc_Php74_lte, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "gt"), zif_Colopl_ColoplBc_Php74_gt, arginfo_Colopl_ColoplBc_Php74_gt, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "gt"), zif_Colopl_ColoplBc_Php74_gt, arginfo_Colopl_ColoplBc_Php74_gt, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "gte"), zif_Colopl_ColoplBc_Php74_gte, arginfo_Colopl_ColoplBc_Php74_gte, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "gte"), zif_Colopl_ColoplBc_Php74_gte, arginfo_Colopl_ColoplBc_Php74_gte, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "spaceship"), zif_Colopl_ColoplBc_Php74_spaceship, arginfo_Colopl_ColoplBc_Php74_spaceship, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY(ZEND_NS_NAME("Colopl\\ColoplBc\\Php74", "spaceship"), zif_Colopl_ColoplBc_Php74_spaceship, arginfo_Colopl_ColoplBc_Php74_spaceship, 0)
#endif
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
