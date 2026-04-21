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

#include "php_colopl_bc.h"

#include "ext/date/php_date.h"
#include "zend_bitset.h"

#ifndef RAND_MAX
# define RAND_MAX (1 << 15)
#endif

#if !defined(ZTS) && (defined(HAVE_LRAND48) || defined(HAVE_RANDOM))
enum {
	COLOPL_BC_RAND_MAX_VALUE = 2147483647,
	COLOPL_BC_MT_M = 397,
	COLOPL_BC_MT_RAND_MAX_VALUE = 0x7FFFFFFF
};
#else
enum {
	COLOPL_BC_RAND_MAX_VALUE = RAND_MAX,
	COLOPL_BC_MT_M = 397,
	COLOPL_BC_MT_RAND_MAX_VALUE = 0x7FFFFFFF
};
#endif

/* Emulate gcc + amd64 undefined behavior results. */
static inline zend_long php_colopl_bc_float_to_long_amd64(zend_long min, double value)
{
	if (value > ((double) ZEND_LONG_MAX) || value < ((double) ZEND_LONG_MIN)) {
		return 0;
	}

	return min + (zend_long) value;
}

static inline zend_long php_colopl_bc_rand_range(zend_long number, zend_long min, zend_long max, zend_long rand_max)
{
	return php_colopl_bc_float_to_long_amd64(
		min,
		((double) max - (double) min + 1.0) * ((double) number / ((double) rand_max + 1.0))
	);
}

static inline zend_long php_colopl_bc_generate_seed(void)
{
#ifdef PHP_WIN32
	return (((zend_long) (time(0) * GetCurrentProcessId())) ^ ((zend_long) (1000000.0 * php_combined_lcg())));
#else
	return (((zend_long) (time(0) * getpid())) ^ ((zend_long) (1000000.0 * php_combined_lcg())));
#endif
}

static inline uint32_t php_colopl_bc_mt_hi_bit(uint32_t value)
{
	return value & 0x80000000U;
}

static inline uint32_t php_colopl_bc_mt_lo_bit(uint32_t value)
{
	return value & 0x00000001U;
}

static inline uint32_t php_colopl_bc_mt_lo_bits(uint32_t value)
{
	return value & 0x7FFFFFFFU;
}

static inline uint32_t php_colopl_bc_mt_mix_bits(uint32_t left, uint32_t right)
{
	return php_colopl_bc_mt_hi_bit(left) | php_colopl_bc_mt_lo_bits(right);
}

static inline uint32_t php_colopl_bc_mt_twist(uint32_t m, uint32_t u, uint32_t v)
{
	return m ^ (php_colopl_bc_mt_mix_bits(u, v) >> 1) ^ ((uint32_t) (-(uint32_t) php_colopl_bc_mt_lo_bit(u)) & 0x9908b0dfU);
}

/* rand.c */

#if defined(__clang__)
__attribute__((no_sanitize("signed-integer-overflow")))
#endif
void php_colopl_bc_srand(zend_long seed)
{
#ifdef ZTS
	COLOPL_BC_G(rand_seed) = (uint32_t) seed;
#else
	uint32_t s = (uint32_t) seed;
	int i = 0;

	if (s == 0) {
		s = 1;
	}

	COLOPL_BC_G(gnurandom_r)[0] = s;
	for (i = 1; i < 31; i++) {
		COLOPL_BC_G(gnurandom_r)[i] = (16807LL * COLOPL_BC_G(gnurandom_r)[i - 1]) % 2147483647;
		if (COLOPL_BC_G(gnurandom_r)[i] < 0) {
			COLOPL_BC_G(gnurandom_r)[i] += 2147483647;
		}
	}
	for (i = 31; i < 34; i++) {
		COLOPL_BC_G(gnurandom_r)[i] = COLOPL_BC_G(gnurandom_r)[i - 31];
	}
	for (i = 34; i < 344; i++) {
		COLOPL_BC_G(gnurandom_r)[i] = COLOPL_BC_G(gnurandom_r)[i - 31] + COLOPL_BC_G(gnurandom_r)[i - 3];
	}

	COLOPL_BC_G(gnurandom_next) = 0;
#endif
	COLOPL_BC_G(rand_is_seeded) = 1;
}

#if defined(__clang__)
__attribute__((no_sanitize("signed-integer-overflow")))
#endif
zend_long php_colopl_bc_rand(void)
{
	zend_long ret;

	if (!COLOPL_BC_G(rand_is_seeded)) {
		php_colopl_bc_srand(php_colopl_bc_generate_seed());
	}

#ifdef ZTS
	uint32_t next = COLOPL_BC_G(rand_seed);

	next = (next * 1103515245) + 12345;
	ret = (uint32_t) (next / 65536) % 2048;

	next = (next * 1103515245) + 12345;
	ret <<= 10;
	ret ^= (uint32_t) (next / 65536) % 1024;

	next = (next * 1103515245) + 12345;
	ret <<= 10;
	ret ^= (uint32_t) (next / 65536) % 1024;

	COLOPL_BC_G(rand_seed) = next;
#else
	COLOPL_BC_G(gnurandom_r)[COLOPL_BC_G(gnurandom_next) % 344] = COLOPL_BC_G(gnurandom_r)[(COLOPL_BC_G(gnurandom_next) + 313) % 344] + COLOPL_BC_G(gnurandom_r)[(COLOPL_BC_G(gnurandom_next) + 341) % 344];
	ret = ((uint32_t) COLOPL_BC_G(gnurandom_r)[COLOPL_BC_G(gnurandom_next) % 344] >> 1);
	COLOPL_BC_G(gnurandom_next) = (COLOPL_BC_G(gnurandom_next) + 1) % 344;
#endif
	return (zend_long) ret;
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_srand)
{
	zend_long seed = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(seed)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 0) {
		seed = php_colopl_bc_generate_seed();
	}

	php_colopl_bc_srand(seed);
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_rand)
{
	zend_long min, max, number;
	int argc;

	number = php_colopl_bc_rand();
	argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		RETURN_LONG(number);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	number = php_colopl_bc_rand_range(number, min, max, COLOPL_BC_RAND_MAX_VALUE);
	RETURN_LONG(number);
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_getrandmax)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(COLOPL_BC_RAND_MAX_VALUE);
}

/* mt_rand.c */

static inline void mt_initialize(uint32_t seed, uint32_t *state)
{
	register uint32_t *s = state;
	register uint32_t *r = state;
	register int i = 1;

	*s++ = seed & 0xffffffffU;
	for (; i < COLOPL_BC_MT_N; ++i) {
		*s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
		r++;
	}
}

static inline void mt_reload(void)
{
	register uint32_t *state = COLOPL_BC_G(mt_state);
	register uint32_t *p = state;
	register int i;

	for (i = COLOPL_BC_MT_N - COLOPL_BC_MT_M; i--; ++p) {
		*p = php_colopl_bc_mt_twist(p[COLOPL_BC_MT_M], p[0], p[1]);
	}
	for (i = COLOPL_BC_MT_M; --i; ++p) {
		*p = php_colopl_bc_mt_twist(p[COLOPL_BC_MT_M - COLOPL_BC_MT_N], p[0], p[1]);
	}
	*p = php_colopl_bc_mt_twist(p[COLOPL_BC_MT_M - COLOPL_BC_MT_N], p[0], state[0]);
	COLOPL_BC_G(mt_left) = COLOPL_BC_MT_N;
	COLOPL_BC_G(mt_next) = state;
}

void php_colopl_bc_mt_srand(uint32_t seed)
{
	mt_initialize(seed, COLOPL_BC_G(mt_state));
	mt_reload();

	COLOPL_BC_G(mt_rand_is_seeded) = 1;
}

uint32_t php_colopl_bc_mt_rand(void)
{
	register uint32_t s1;

	if (COLOPL_BC_G(mt_left) == 0) {
		mt_reload();
	}
	--COLOPL_BC_G(mt_left);

	s1 = *COLOPL_BC_G(mt_next)++;
	s1 ^= (s1 >> 11);
	s1 ^= (s1 <<  7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;
	return ( s1 ^ (s1 >> 18) );
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_mt_srand)
{
	zend_long seed = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(seed)
	ZEND_PARSE_PARAMETERS_END();

	if (ZEND_NUM_ARGS() == 0) {
		seed = php_colopl_bc_generate_seed();
	}

	php_colopl_bc_mt_srand((uint32_t) seed);
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_mt_rand)
{
	zend_long min, max, number;
	int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		if (!COLOPL_BC_G(mt_rand_is_seeded)) {
			php_colopl_bc_mt_srand(php_colopl_bc_generate_seed());
		}
		RETURN_LONG(php_colopl_bc_mt_rand() >> 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(max < min)) {
		zend_argument_value_error(2, "must be greater than or equal to argument #1 ($min)");
		RETURN_THROWS();
	}

	if (!COLOPL_BC_G(mt_rand_is_seeded)) {
		php_colopl_bc_mt_srand(php_colopl_bc_generate_seed());
	}

	number = (zend_long) (php_colopl_bc_mt_rand() >> 1);
	number = php_colopl_bc_rand_range(number, min, max, COLOPL_BC_MT_RAND_MAX_VALUE);
	RETURN_LONG(number);
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_mt_getrandmax)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(COLOPL_BC_MT_RAND_MAX_VALUE);
}

/* array.c */

#if PHP_VERSION_ID < 80200
void php_colopl_bc_array_data_shuffle(zval *array)
{
	uint32_t idx, j, n_elems;
	Bucket *p, temp;
	HashTable *hash;
	zend_long rnd_idx;
	uint32_t n_left;

	n_elems = zend_hash_num_elements(Z_ARRVAL_P(array));

	if (n_elems < 1) {
		return;
	}

	hash = Z_ARRVAL_P(array);
	n_left = n_elems;

	if (EXPECTED(hash->u.v.nIteratorsCount == 0)) {
		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				p = hash->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					hash->arData[j] = *p;
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_colopl_bc_rand();
			rnd_idx = php_colopl_bc_rand_range(rnd_idx, 0, n_left, COLOPL_BC_RAND_MAX_VALUE);
			if (rnd_idx != n_left) {
				temp = hash->arData[n_left];
				hash->arData[n_left] = hash->arData[rnd_idx];
				hash->arData[rnd_idx] = temp;
			}
		}
	} else {
		uint32_t iter_pos = zend_hash_iterators_lower_pos(hash, 0);

		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				p = hash->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					hash->arData[j] = *p;
					if (idx == iter_pos) {
						zend_hash_iterators_update(hash, idx, j);
						iter_pos = zend_hash_iterators_lower_pos(hash, iter_pos + 1);
					}
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_colopl_bc_rand();
			rnd_idx = php_colopl_bc_rand_range(rnd_idx, 0, n_left, COLOPL_BC_RAND_MAX_VALUE);
			if (rnd_idx != n_left) {
				temp = hash->arData[n_left];
				hash->arData[n_left] = hash->arData[rnd_idx];
				hash->arData[rnd_idx] = temp;
				zend_hash_iterators_update(hash, (uint32_t)rnd_idx, n_left);
			}
		}
	}
	HANDLE_BLOCK_INTERRUPTIONS();
	hash->nNumUsed = n_elems;
	hash->nInternalPointer = 0;

	for (j = 0; j < n_elems; j++) {
		p = hash->arData + j;
		if (p->key) {
			zend_string_release(p->key);
		}
		p->h = j;
		p->key = NULL;
	}
	hash->nNextFreeElement = n_elems;
	if (!(hash->u.flags & HASH_FLAG_PACKED)) {
		zend_hash_to_packed(hash);
	}
	HANDLE_UNBLOCK_INTERRUPTIONS();
}
#else
void php_colopl_bc_array_data_shuffle(zval *array)
{
	int64_t idx, j, n_elems, rnd_idx, n_left;
	zval *zv, temp;
	HashTable *hash;

	n_elems = zend_hash_num_elements(Z_ARRVAL_P(array));

	if (n_elems < 1) {
		return;
	}

	hash = Z_ARRVAL_P(array);
	n_left = n_elems;

	if (!HT_IS_PACKED(hash)) {
		if (!HT_HAS_STATIC_KEYS_ONLY(hash)) {
			Bucket *p = hash->arData;
			zend_long i = hash->nNumUsed;

			for (; i > 0; p++, i--) {
				if (p->key) {
					zend_string_release(p->key);
					p->key = NULL;
				}
			}
		}
		zend_hash_to_packed(hash);
	}

	if (EXPECTED(!HT_HAS_ITERATORS(hash))) {
		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				zv = hash->arPacked + idx;
				if (Z_TYPE_P(zv) == IS_UNDEF) continue;
				if (j != idx) {
					ZVAL_COPY_VALUE(&hash->arPacked[j], zv);
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_colopl_bc_rand();
			rnd_idx = php_colopl_bc_rand_range(rnd_idx, 0, n_left, COLOPL_BC_RAND_MAX_VALUE);
			if (rnd_idx != n_left) {
				ZVAL_COPY_VALUE(&temp, &hash->arPacked[n_left]);
				ZVAL_COPY_VALUE(&hash->arPacked[n_left], &hash->arPacked[rnd_idx]);
				ZVAL_COPY_VALUE(&hash->arPacked[rnd_idx], &temp);
			}
		}
	} else {
		zend_long iter_pos = zend_hash_iterators_lower_pos(hash, 0);

		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				zv = hash->arPacked + idx;
				if (Z_TYPE_P(zv) == IS_UNDEF) continue;
				if (j != idx) {
					ZVAL_COPY_VALUE(&hash->arPacked[j], zv);
					if (idx == iter_pos) {
						zend_hash_iterators_update(hash, idx, j);
						iter_pos = zend_hash_iterators_lower_pos(hash, iter_pos + 1);
					}
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_colopl_bc_rand();
			rnd_idx = php_colopl_bc_rand_range(rnd_idx, 0, n_left, COLOPL_BC_RAND_MAX_VALUE);
			if (rnd_idx != n_left) {
				ZVAL_COPY_VALUE(&temp, &hash->arPacked[n_left]);
				ZVAL_COPY_VALUE(&hash->arPacked[n_left], &hash->arPacked[rnd_idx]);
				ZVAL_COPY_VALUE(&hash->arPacked[rnd_idx], &temp);
				zend_hash_iterators_update(hash, (uint32_t)rnd_idx, n_left);
			}
		}
	}
	hash->nNumUsed = n_elems;
	hash->nInternalPointer = 0;
	hash->nNextFreeElement = n_elems;
}
#endif

PHP_FUNCTION(Colopl_ColoplBc_Php70_shuffle)
{
	zval *array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_EX(array, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	php_colopl_bc_array_data_shuffle(array);

	RETURN_TRUE;
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_array_rand)
{
	zval *input;
	zend_long randval, num_req = 1;
	int num_avail;
	zend_string *string_key;
	zend_ulong num_key;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(num_req)
	ZEND_PARSE_PARAMETERS_END();

	num_avail = zend_hash_num_elements(Z_ARRVAL_P(input));

	if (ZEND_NUM_ARGS() > 1) {
		if (num_req <= 0 || num_req > num_avail) {
			zend_argument_value_error(2, "must be between 1 and the number of elements in argument #1 ($array)");
			RETURN_THROWS();
		}
	}

	/* Make the return value an array only if we need to pass back more than one result. */
	if (num_req > 1) {
		array_init_size(return_value, (uint32_t)num_req);
	}

	/* We can't use zend_hash_index_find() because the array may have string keys or gaps. */
	ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(input), num_key, string_key) {
		if (!num_req) {
			break;
		}

		randval = php_colopl_bc_rand();

		if ((double) randval / ((double) COLOPL_BC_RAND_MAX_VALUE + 1.0) < (double) num_req / (double) num_avail) {
			/* If we are returning a single result, just do it. */
			if (Z_TYPE_P(return_value) != IS_ARRAY) {
				if (string_key) {
					RETURN_STR_COPY(string_key);
				} else {
					RETURN_LONG(num_key);
				}
			} else {
				/* Append the result to the return value. */
				if (string_key) {
					add_next_index_str(return_value, zend_string_copy(string_key));
				} else {
					add_next_index_long(return_value, num_key);
				}
			}
			num_req--;
		}
		num_avail--;
	} ZEND_HASH_FOREACH_END();

}

/* string.c */
void php_colopl_bc_string_shuffle(char *str, zend_long len)
{
	zend_long n_elems, rnd_idx, n_left;
	char temp;

	n_elems = len;

	if (n_elems <= 1) {
		return;
	}

	n_left = n_elems;

	while (--n_left) {
		rnd_idx = php_colopl_bc_rand();
		rnd_idx = php_colopl_bc_rand_range(rnd_idx, 0, n_left, COLOPL_BC_RAND_MAX_VALUE);
		if (rnd_idx != n_left) {
			temp = str[n_left];
			str[n_left] = str[rnd_idx];
			str[rnd_idx] = temp;
		}
	}
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_str_shuffle)
{
	zend_string *arg;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_STRINGL(ZSTR_VAL(arg), ZSTR_LEN(arg));
	if (Z_STRLEN_P(return_value) > 1) {
		php_colopl_bc_string_shuffle(Z_STRVAL_P(return_value), (zend_long) Z_STRLEN_P(return_value));
	}
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_date_create)
{
	zval *timezone_object = NULL;
	char *time_str = NULL;
	size_t time_str_len = 0;
	php_date_obj *date;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(timezone_object, php_date_get_timezone_ce())
	ZEND_PARSE_PARAMETERS_END();

	php_date_instantiate(php_date_get_date_ce(), return_value);
	if (!php_date_initialize(Z_PHPDATE_P(return_value), time_str, time_str_len, NULL, timezone_object, 0)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	date = Z_PHPDATE_P(return_value);
	date->time->us = 0;
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_date_create_immutable)
{
	zval *timezone_object = NULL;
	char *time_str = NULL;
	size_t time_str_len = 0;
	php_date_obj *date;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(time_str, time_str_len)
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(timezone_object, php_date_get_timezone_ce())
	ZEND_PARSE_PARAMETERS_END();

	php_date_instantiate(php_date_get_immutable_ce(), return_value);
	if (!php_date_initialize(Z_PHPDATE_P(return_value), time_str, time_str_len, NULL, timezone_object, 0)) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	date = Z_PHPDATE_P(return_value);
	date->time->us = 0;
}
