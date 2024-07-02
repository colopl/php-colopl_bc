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

#include "ext/date/php_date.h"
#include "zend_bitset.h"

/* rand.c */

#if defined(__clang__)
__attribute__((no_sanitize("signed-integer-overflow")))
#endif
PHPAPI void php_colopl_bc_srand(zend_long seed)
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
PHPAPI zend_long php_colopl_bc_rand(void)
{
	zend_long ret;

	if (!COLOPL_BC_G(rand_is_seeded)) {
		php_colopl_bc_srand(COLOPL_BC_GENERATE_SEED());
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
		seed = COLOPL_BC_GENERATE_SEED();
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

	PHP_COLOPL_BC_RAND_RANGE(number, min, max, PHP_COLOPL_BC_RAND_MAX);
	RETURN_LONG(number);
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_getrandmax)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(PHP_COLOPL_BC_RAND_MAX);
}

/* mt_rand.c */

static inline void mt_initialize(uint32_t seed, uint32_t *state)
{
	register uint32_t *s = state;
	register uint32_t *r = state;
	register int i = 1;

	*s++ = seed & 0xffffffffU;
	for (; i < N; ++i) {
		*s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
		r++;
	}
}

static inline void mt_reload(void)
{
	register uint32_t *state = COLOPL_BC_G(mt_state);
	register uint32_t *p = state;
	register int i;

	for (i = N - M; i--; ++p) {
		*p = twist(p[M], p[0], p[1]);
	}
	for (i = M; --i; ++p) {
		*p = twist(p[M-N], p[0], p[1]);
	}
	*p = twist(p[M-N], p[0], state[0]);
	COLOPL_BC_G(mt_left) = N;
	COLOPL_BC_G(mt_next) = state;
}

PHPAPI void php_colopl_bc_mt_srand(uint32_t seed)
{
	mt_initialize(seed, COLOPL_BC_G(mt_state));
	mt_reload();

	COLOPL_BC_G(mt_rand_is_seeded) = 1;
}

PHPAPI uint32_t php_colopl_bc_mt_rand(void)
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
		seed = COLOPL_BC_GENERATE_SEED();
	}

	php_colopl_bc_mt_srand((uint32_t) seed);
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_mt_rand)
{
	zend_long min, max, number;
	int argc = ZEND_NUM_ARGS();

	if (argc == 0) {
		if (!COLOPL_BC_G(mt_rand_is_seeded)) {
			php_colopl_bc_mt_srand(COLOPL_BC_GENERATE_SEED());
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
		php_colopl_bc_mt_srand(COLOPL_BC_GENERATE_SEED());
	}

	number = (zend_long) (php_colopl_bc_mt_rand() >> 1);
	PHP_COLOPL_BC_RAND_RANGE(number, min, max, PHP_COLOPL_BC_MT_RAND_MAX);
	RETURN_LONG(number);
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_mt_getrandmax)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(PHP_COLOPL_BC_MT_RAND_MAX);
}

/* array.c */

#if PHP_VERSION_ID < 80200
PHPAPI void php_colopl_bc_array_data_shuffle(zval *array)
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
			PHP_COLOPL_BC_RAND_RANGE(rnd_idx, 0, n_left, PHP_COLOPL_BC_RAND_MAX);
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
			PHP_COLOPL_BC_RAND_RANGE(rnd_idx, 0, n_left, PHP_COLOPL_BC_RAND_MAX);
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
PHPAPI void php_colopl_bc_array_data_shuffle(zval *array)
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
			PHP_COLOPL_BC_RAND_RANGE(rnd_idx, 0, n_left, PHP_COLOPL_BC_RAND_MAX);
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
			PHP_COLOPL_BC_RAND_RANGE(rnd_idx, 0, n_left, PHP_COLOPL_BC_RAND_MAX);
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

		if ((double) (randval / (PHP_COLOPL_BC_RAND_MAX + 1.0)) < (double) num_req / (double) num_avail) {
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
PHPAPI void php_colopl_bc_string_shuffle(char *str, zend_long len)
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
		PHP_COLOPL_BC_RAND_RANGE(rnd_idx, 0, n_left, PHP_COLOPL_BC_RAND_MAX);
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
	zend_function *fn;
	php_date_obj *date;

	fn = zend_hash_str_find_ptr(CG(function_table), "date_create", strlen("date_create"));
	ZEND_ASSERT(fn);
	fn->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

	date = Z_PHPDATE_P(return_value);
	date->time->us = 0;
}

PHP_FUNCTION(Colopl_ColoplBc_Php70_date_create_immutable)
{
	zend_function *fn;
	php_date_obj *date;

	fn = zend_hash_str_find_ptr(CG(function_table), "date_create_immutable", strlen("date_create_immutable"));
	ZEND_ASSERT(fn);
	fn->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);

	date = Z_PHPDATE_P(return_value);
	date->time->us = 0;
}
