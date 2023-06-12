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

#ifndef PHP_COLOPL_BC_PHP70_H
#define PHP_COLOPL_BC_PHP70_H

#include "php_colopl_bc.h"

#ifndef RAND_MAX
# define RAND_MAX (1<<15)
#endif

#if !defined(ZTS) && (defined(HAVE_LRAND48) || defined(HAVE_RANDOM))
# define PHP_COLOPL_BC_RAND_MAX 2147483647
#else
# define PHP_COLOPL_BC_RAND_MAX RAND_MAX
#endif

/* Emulate gcc + amd64 undefined behavior results. */
static inline zend_long float_to_long_amd64(zend_long min, double f)
{
	if (f > ((double) ZEND_LONG_MAX) || f < ((double) ZEND_LONG_MIN)) {
		return 0;
	}

	return min + (zend_long) f;
}

# define PHP_COLOPL_BC_RAND_RANGE(__n, __min, __max, __tmax) \
	(__n) = float_to_long_amd64(__min, ((double) (__max) - (__min) + 1.0) * ((__n) / ((__tmax) + 1.0)))

#ifdef PHP_WIN32
# define COLOPL_BC_GENERATE_SEED() (((zend_long) (time(0) * GetCurrentProcessId())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
#else
# define COLOPL_BC_GENERATE_SEED() (((zend_long) (time(0) * getpid())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
#endif

/* rand.c */

PHPAPI void php_colopl_bc_srand(zend_long seed);
PHPAPI zend_long php_colopl_bc_rand(void);

/* mt_rand.c */

/*
	The following php_colopl_bc_mt_...() functions are based on a C++ class MTRand by
	Richard J. Wagner. For more information see the web page at
	http://www-personal.engin.umich.edu/~wagnerr/MersenneTwister.html

	Mersenne Twister random number generator -- a C++ class MTRand
	Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
	Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

	The Mersenne Twister is an algorithm for generating random numbers.  It
	was designed with consideration of the flaws in various other generators.
	The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
	are far greater.  The generator is also fast; it avoids multiplication and
	division, and it benefits from caches and pipelines.  For more information
	see the inventors' web page at http://www.math.keio.ac.jp/~matumoto/emt.html

	Reference
	M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
	Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
	Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

	Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
	Copyright (C) 2000 - 2003, Richard J. Wagner
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

	3. The names of its contributors may not be used to endorse or promote
	   products derived from this software without specific prior written
	   permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define PHP_COLOPL_BC_MT_RAND_MAX ((zend_long) (0x7FFFFFFF)) /* (1<<31) - 1 */

#define N				(624)					/* length of state vector */
#define M				(397)					/* a period parameter */
#define hiBit(u)		((u) & 0x80000000U)		/* mask all but highest   bit of u */
#define loBit(u)		((u) & 0x00000001U)		/* mask all but lowest    bit of u */
#define loBits(u)		((u) & 0x7FFFFFFFU)		/* mask     the highest   bit of u */
#define mixBits(u, v)	(hiBit(u)|loBits(v))	/* move hi bit of u to hi bit of v */

#define twist(m,u,v)	(m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(uint32_t)(loBit(u))) & 0x9908b0dfU))

PHPAPI void php_colopl_bc_mt_srand(uint32_t seed);
PHPAPI uint32_t php_colopl_bc_mt_rand(void);

/* array.c */

PHPAPI void php_colopl_bc_array_data_shuffle(zval *array);

/* string.c */

PHPAPI void php_colopl_bc_string_shuffle(char *str, zend_long len);

#endif	/* PHP_COLOPL_BC_PHP70_H */
