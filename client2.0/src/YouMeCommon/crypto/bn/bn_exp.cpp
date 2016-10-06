/* crypto/bn/bn_exp.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
/* ====================================================================
 * Copyright (c) 1998-2005 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#include <crypto/crypto.h>
#include <memory.h>
#include "bn_lcl.h"

#include <stdlib.h>
#ifdef _WIN32
#include <malloc.h>
# ifndef alloca
#  define alloca _alloca
# endif
#elif defined(__GNUC__)
# ifndef alloca
#  define alloca(s) __builtin_alloca((s))
# endif
#elif defined(__sun)
#include <alloca.h>
#endif


#undef SPARC_T4_MONT
#if defined(OPENSSL_BN_ASM_MONT) && (defined(__sparc__) || defined(__sparc))
#include "sparc_arch.h"
namespace youmecommon {
extern unsigned int OPENSSL_sparcv9cap_P[];
}  // namespace youmecommon
# define SPARC_T4_MONT
#endif

namespace youmecommon {
/* maximum precomputation table size for *variable* sliding windows */
#define TABLE_SIZE      32
#define MONT_WORD 

	static int MOD_EXP_CTIME_COPY_TO_PREBUF(const BIGNUM *b, int top,
		unsigned char *buf, int idx,
		int width)
	{
		size_t i, j;

		if (top > b->top)
			top = b->top;           /* this works because 'buf' is explicitly
									* zeroed */
		for (i = 0, j = idx; i < top * sizeof b->d[0]; i++, j += width) {
			buf[j] = ((unsigned char *)b->d)[i];
		}

		return 1;
	}

	static int MOD_EXP_CTIME_COPY_FROM_PREBUF(BIGNUM *b, int top,
		unsigned char *buf, int idx,
		int width)
	{
		size_t i, j;

		if (bn_wexpand(b, top) == NULL)
			return 0;

		for (i = 0, j = idx; i < top * sizeof b->d[0]; i++, j += width) {
			((unsigned char *)b->d)[i] = buf[j];
		}

		b->top = top;
		bn_correct_top(b);
		return 1;
	}

	/*
	* Given a pointer value, compute the next address that is a cache line
	* multiple.
	*/
#define MOD_EXP_CTIME_ALIGN(x_) \
        ((unsigned char*)(x_) + (MOD_EXP_CTIME_MIN_CACHE_LINE_WIDTH - (((size_t)(x_)) & (MOD_EXP_CTIME_MIN_CACHE_LINE_MASK))))


	int BN_mod_exp_mont_consttime(BIGNUM *rr, const BIGNUM *a, const BIGNUM *p,
		const BIGNUM *m, BN_CTX *ctx,
		BN_MONT_CTX *in_mont)
	{
		int i, bits, ret = 0, window, wvalue;
		int top;
		BN_MONT_CTX *mont = NULL;

		int numPowers;
		unsigned char *powerbufFree = NULL;
		int powerbufLen = 0;
		unsigned char *powerbuf = NULL;
		BIGNUM tmp, am;
#if defined(SPARC_T4_MONT)
		unsigned int t4 = 0;
#endif

		bn_check_top(a);
		bn_check_top(p);
		bn_check_top(m);

		top = m->top;

		if (!(m->d[0] & 1)) {
			BNerr(BN_F_BN_MOD_EXP_MONT_CONSTTIME, BN_R_CALLED_WITH_EVEN_MODULUS);
			return (0);
		}
		bits = BN_num_bits(p);
		if (bits == 0) {
			ret = BN_one(rr);
			return ret;
		}

		BN_CTX_start(ctx);

		/*
		* Allocate a montgomery context if it was not supplied by the caller. If
		* this is not done, things will break in the montgomery part.
		*/
		if (in_mont != NULL)
			mont = in_mont;
		else {
			if ((mont = BN_MONT_CTX_new()) == NULL)
				goto err;
			if (!BN_MONT_CTX_set(mont, m, ctx))
				goto err;
		}

#ifdef RSAZ_ENABLED
		/*
		* If the size of the operands allow it, perform the optimized
		* RSAZ exponentiation. For further information see
		* crypto/bn/rsaz_exp.c and accompanying assembly modules.
		*/
		if ((16 == a->top) && (16 == p->top) && (BN_num_bits(m) == 1024)
			&& rsaz_avx2_eligible()) {
			if (NULL == bn_wexpand(rr, 16))
				goto err;
			RSAZ_1024_mod_exp_avx2(rr->d, a->d, p->d, m->d, mont->RR.d,
				mont->n0[0]);
			rr->top = 16;
			rr->neg = 0;
			bn_correct_top(rr);
			ret = 1;
			goto err;
		}
		else if ((8 == a->top) && (8 == p->top) && (BN_num_bits(m) == 512)) {
			if (NULL == bn_wexpand(rr, 8))
				goto err;
			RSAZ_512_mod_exp(rr->d, a->d, p->d, m->d, mont->n0[0], mont->RR.d);
			rr->top = 8;
			rr->neg = 0;
			bn_correct_top(rr);
			ret = 1;
			goto err;
		}
#endif

		/* Get the window size to use with size of p. */
		window = BN_window_bits_for_ctime_exponent_size(bits);
#if defined(SPARC_T4_MONT)
		if (window >= 5 && (top & 15) == 0 && top <= 64 &&
			(OPENSSL_sparcv9cap_P[1] & (CFR_MONTMUL | CFR_MONTSQR)) ==
			(CFR_MONTMUL | CFR_MONTSQR) && (t4 = OPENSSL_sparcv9cap_P[0]))
			window = 5;
		else
#endif
#if defined(OPENSSL_BN_ASM_MONT5)
			if (window >= 5) {
				window = 5;             /* ~5% improvement for RSA2048 sign, and even
										* for RSA4096 */
				if ((top & 7) == 0)
					powerbufLen += 2 * top * sizeof(m->d[0]);
			}
#endif
		(void)0;

		/*
		* Allocate a buffer large enough to hold all of the pre-computed powers
		* of am, am itself and tmp.
		*/
		numPowers = 1 << window;
		powerbufLen += sizeof(m->d[0]) * (top * numPowers +
			((2 * top) >
			numPowers ? (2 * top) : numPowers));
#ifdef alloca
		if (powerbufLen < 3072)
			powerbufFree =
			(unsigned char *)alloca(powerbufLen + MOD_EXP_CTIME_MIN_CACHE_LINE_WIDTH);
		else
#endif
			if ((powerbufFree =
				(unsigned char *)OPENSSL_malloc(powerbufLen +
				MOD_EXP_CTIME_MIN_CACHE_LINE_WIDTH))
				== NULL)
				goto err;

		powerbuf = MOD_EXP_CTIME_ALIGN(powerbufFree);
		memset(powerbuf, 0, powerbufLen);

#ifdef alloca
		if (powerbufLen < 3072)
			powerbufFree = NULL;
#endif

		/* lay down tmp and am right after powers table */
		tmp.d = (BN_ULONG *)(powerbuf + sizeof(m->d[0]) * top * numPowers);
		am.d = tmp.d + top;
		tmp.top = am.top = 0;
		tmp.dmax = am.dmax = top;
		tmp.neg = am.neg = 0;
		tmp.flags = am.flags = BN_FLG_STATIC_DATA;

		/* prepare a^0 in Montgomery domain */
#if 1                           /* by Shay Gueron's suggestion */
		if (m->d[top - 1] & (((BN_ULONG)1) << (BN_BITS2 - 1))) {
			/* 2^(top*BN_BITS2) - m */
			tmp.d[0] = (0 - m->d[0]) & BN_MASK2;
			for (i = 1; i < top; i++)
				tmp.d[i] = (~m->d[i]) & BN_MASK2;
			tmp.top = top;
		}
		else
#endif
			if (!BN_to_montgomery(&tmp, BN_value_one(), mont, ctx))
				goto err;

		/* prepare a^1 in Montgomery domain */
		if (a->neg || BN_ucmp(a, m) >= 0) {
			if (!BN_mod(&am, a, m, ctx))
				goto err;
			if (!BN_to_montgomery(&am, &am, mont, ctx))
				goto err;
		}
		else if (!BN_to_montgomery(&am, a, mont, ctx))
			goto err;

#if defined(SPARC_T4_MONT)
		if (t4) {
			typedef int(*bn_pwr5_mont_f) (BN_ULONG *tp, const BN_ULONG *np,
				const BN_ULONG *n0, const void *table,
				int power, int bits);
			int bn_pwr5_mont_t4_8(BN_ULONG *tp, const BN_ULONG *np,
				const BN_ULONG *n0, const void *table,
				int power, int bits);
			int bn_pwr5_mont_t4_16(BN_ULONG *tp, const BN_ULONG *np,
				const BN_ULONG *n0, const void *table,
				int power, int bits);
			int bn_pwr5_mont_t4_24(BN_ULONG *tp, const BN_ULONG *np,
				const BN_ULONG *n0, const void *table,
				int power, int bits);
			int bn_pwr5_mont_t4_32(BN_ULONG *tp, const BN_ULONG *np,
				const BN_ULONG *n0, const void *table,
				int power, int bits);
			static const bn_pwr5_mont_f pwr5_funcs[4] = {
				bn_pwr5_mont_t4_8, bn_pwr5_mont_t4_16,
				bn_pwr5_mont_t4_24, bn_pwr5_mont_t4_32
			};
			bn_pwr5_mont_f pwr5_worker = pwr5_funcs[top / 16 - 1];

			typedef int(*bn_mul_mont_f) (BN_ULONG *rp, const BN_ULONG *ap,
				const void *bp, const BN_ULONG *np,
				const BN_ULONG *n0);
			int bn_mul_mont_t4_8(BN_ULONG *rp, const BN_ULONG *ap, const void *bp,
				const BN_ULONG *np, const BN_ULONG *n0);
			int bn_mul_mont_t4_16(BN_ULONG *rp, const BN_ULONG *ap,
				const void *bp, const BN_ULONG *np,
				const BN_ULONG *n0);
			int bn_mul_mont_t4_24(BN_ULONG *rp, const BN_ULONG *ap,
				const void *bp, const BN_ULONG *np,
				const BN_ULONG *n0);
			int bn_mul_mont_t4_32(BN_ULONG *rp, const BN_ULONG *ap,
				const void *bp, const BN_ULONG *np,
				const BN_ULONG *n0);
			static const bn_mul_mont_f mul_funcs[4] = {
				bn_mul_mont_t4_8, bn_mul_mont_t4_16,
				bn_mul_mont_t4_24, bn_mul_mont_t4_32
			};
			bn_mul_mont_f mul_worker = mul_funcs[top / 16 - 1];

			void bn_mul_mont_vis3(BN_ULONG *rp, const BN_ULONG *ap,
				const void *bp, const BN_ULONG *np,
				const BN_ULONG *n0, int num);
			void bn_mul_mont_t4(BN_ULONG *rp, const BN_ULONG *ap,
				const void *bp, const BN_ULONG *np,
				const BN_ULONG *n0, int num);
			void bn_mul_mont_gather5_t4(BN_ULONG *rp, const BN_ULONG *ap,
				const void *table, const BN_ULONG *np,
				const BN_ULONG *n0, int num, int power);
			void bn_flip_n_scatter5_t4(const BN_ULONG *inp, size_t num,
				void *table, size_t power);
			void bn_gather5_t4(BN_ULONG *out, size_t num,
				void *table, size_t power);
			void bn_flip_t4(BN_ULONG *dst, BN_ULONG *src, size_t num);

			BN_ULONG *np = mont->N.d, *n0 = mont->n0;
			int stride = 5 * (6 - (top / 16 - 1)); /* multiple of 5, but less
												   * than 32 */

			/*
			* BN_to_montgomery can contaminate words above .top [in
			* BN_DEBUG[_DEBUG] build]...
			*/
			for (i = am.top; i < top; i++)
				am.d[i] = 0;
			for (i = tmp.top; i < top; i++)
				tmp.d[i] = 0;

			bn_flip_n_scatter5_t4(tmp.d, top, powerbuf, 0);
			bn_flip_n_scatter5_t4(am.d, top, powerbuf, 1);
			if (!(*mul_worker) (tmp.d, am.d, am.d, np, n0) &&
				!(*mul_worker) (tmp.d, am.d, am.d, np, n0))
				bn_mul_mont_vis3(tmp.d, am.d, am.d, np, n0, top);
			bn_flip_n_scatter5_t4(tmp.d, top, powerbuf, 2);

			for (i = 3; i < 32; i++) {
				/* Calculate a^i = a^(i-1) * a */
				if (!(*mul_worker) (tmp.d, tmp.d, am.d, np, n0) &&
					!(*mul_worker) (tmp.d, tmp.d, am.d, np, n0))
					bn_mul_mont_vis3(tmp.d, tmp.d, am.d, np, n0, top);
				bn_flip_n_scatter5_t4(tmp.d, top, powerbuf, i);
			}

			/* switch to 64-bit domain */
			np = alloca(top * sizeof(BN_ULONG));
			top /= 2;
			bn_flip_t4(np, mont->N.d, top);

			bits--;
			for (wvalue = 0, i = bits % 5; i >= 0; i--, bits--)
				wvalue = (wvalue << 1) + BN_is_bit_set(p, bits);
			bn_gather5_t4(tmp.d, top, powerbuf, wvalue);

			/*
			* Scan the exponent one window at a time starting from the most
			* significant bits.
			*/
			while (bits >= 0) {
				if (bits < stride)
					stride = bits + 1;
				bits -= stride;
				wvalue = bn_get_bits(p, bits + 1);

				if ((*pwr5_worker) (tmp.d, np, n0, powerbuf, wvalue, stride))
					continue;
				/* retry once and fall back */
				if ((*pwr5_worker) (tmp.d, np, n0, powerbuf, wvalue, stride))
					continue;

				bits += stride - 5;
				wvalue >>= stride - 5;
				wvalue &= 31;
				bn_mul_mont_t4(tmp.d, tmp.d, tmp.d, np, n0, top);
				bn_mul_mont_t4(tmp.d, tmp.d, tmp.d, np, n0, top);
				bn_mul_mont_t4(tmp.d, tmp.d, tmp.d, np, n0, top);
				bn_mul_mont_t4(tmp.d, tmp.d, tmp.d, np, n0, top);
				bn_mul_mont_t4(tmp.d, tmp.d, tmp.d, np, n0, top);
				bn_mul_mont_gather5_t4(tmp.d, tmp.d, powerbuf, np, n0, top,
					wvalue);
			}

			bn_flip_t4(tmp.d, tmp.d, top);
			top *= 2;
			/* back to 32-bit domain */
			tmp.top = top;
			bn_correct_top(&tmp);
			OPENSSL_cleanse(np, top * sizeof(BN_ULONG));
		}
		else
#endif
#if defined(OPENSSL_BN_ASM_MONT5)
			if (window == 5 && top > 1) {
				/*
				* This optimization uses ideas from http://eprint.iacr.org/2011/239,
				* specifically optimization of cache-timing attack countermeasures
				* and pre-computation optimization.
				*/

				/*
				* Dedicated window==4 case improves 512-bit RSA sign by ~15%, but as
				* 512-bit RSA is hardly relevant, we omit it to spare size...
				*/
				void bn_mul_mont_gather5(BN_ULONG *rp, const BN_ULONG *ap,
					const void *table, const BN_ULONG *np,
					const BN_ULONG *n0, int num, int power);
				void bn_scatter5(const BN_ULONG *inp, size_t num,
					void *table, size_t power);
				void bn_gather5(BN_ULONG *out, size_t num, void *table, size_t power);
				void bn_power5(BN_ULONG *rp, const BN_ULONG *ap,
					const void *table, const BN_ULONG *np,
					const BN_ULONG *n0, int num, int power);
				int bn_get_bits5(const BN_ULONG *ap, int off);
				int bn_from_montgomery(BN_ULONG *rp, const BN_ULONG *ap,
					const BN_ULONG *not_used, const BN_ULONG *np,
					const BN_ULONG *n0, int num);

				BN_ULONG *np = mont->N.d, *n0 = mont->n0, *np2;

				/*
				* BN_to_montgomery can contaminate words above .top [in
				* BN_DEBUG[_DEBUG] build]...
				*/
				for (i = am.top; i < top; i++)
					am.d[i] = 0;
				for (i = tmp.top; i < top; i++)
					tmp.d[i] = 0;

				if (top & 7)
					np2 = np;
				else
					for (np2 = am.d + top, i = 0; i < top; i++)
						np2[2 * i] = np[i];

				bn_scatter5(tmp.d, top, powerbuf, 0);
				bn_scatter5(am.d, am.top, powerbuf, 1);
				bn_mul_mont(tmp.d, am.d, am.d, np, n0, top);
				bn_scatter5(tmp.d, top, powerbuf, 2);

# if 0
				for (i = 3; i < 32; i++) {
					/* Calculate a^i = a^(i-1) * a */
					bn_mul_mont_gather5(tmp.d, am.d, powerbuf, np2, n0, top, i - 1);
					bn_scatter5(tmp.d, top, powerbuf, i);
				}
# else
				/* same as above, but uses squaring for 1/2 of operations */
				for (i = 4; i < 32; i *= 2) {
					bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
					bn_scatter5(tmp.d, top, powerbuf, i);
				}
				for (i = 3; i < 8; i += 2) {
					int j;
					bn_mul_mont_gather5(tmp.d, am.d, powerbuf, np2, n0, top, i - 1);
					bn_scatter5(tmp.d, top, powerbuf, i);
					for (j = 2 * i; j < 32; j *= 2) {
						bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
						bn_scatter5(tmp.d, top, powerbuf, j);
					}
				}
				for (; i < 16; i += 2) {
					bn_mul_mont_gather5(tmp.d, am.d, powerbuf, np2, n0, top, i - 1);
					bn_scatter5(tmp.d, top, powerbuf, i);
					bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
					bn_scatter5(tmp.d, top, powerbuf, 2 * i);
				}
				for (; i < 32; i += 2) {
					bn_mul_mont_gather5(tmp.d, am.d, powerbuf, np2, n0, top, i - 1);
					bn_scatter5(tmp.d, top, powerbuf, i);
				}
# endif
				bits--;
				for (wvalue = 0, i = bits % 5; i >= 0; i--, bits--)
					wvalue = (wvalue << 1) + BN_is_bit_set(p, bits);
				bn_gather5(tmp.d, top, powerbuf, wvalue);

				/*
				* Scan the exponent one window at a time starting from the most
				* significant bits.
				*/
				if (top & 7)
					while (bits >= 0) {
						for (wvalue = 0, i = 0; i < 5; i++, bits--)
							wvalue = (wvalue << 1) + BN_is_bit_set(p, bits);

						bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
						bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
						bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
						bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
						bn_mul_mont(tmp.d, tmp.d, tmp.d, np, n0, top);
						bn_mul_mont_gather5(tmp.d, tmp.d, powerbuf, np, n0, top,
							wvalue);
					}
				else {
					while (bits >= 0) {
						wvalue = bn_get_bits5(p->d, bits - 4);
						bits -= 5;
						bn_power5(tmp.d, tmp.d, powerbuf, np2, n0, top, wvalue);
					}
				}

				ret = bn_from_montgomery(tmp.d, tmp.d, NULL, np2, n0, top);
				tmp.top = top;
				bn_correct_top(&tmp);
				if (ret) {
					if (!BN_copy(rr, &tmp))
						ret = 0;
					goto err;           /* non-zero ret means it's not error */
				}
			}
			else
#endif
			{
				if (!MOD_EXP_CTIME_COPY_TO_PREBUF(&tmp, top, powerbuf, 0, numPowers))
					goto err;
				if (!MOD_EXP_CTIME_COPY_TO_PREBUF(&am, top, powerbuf, 1, numPowers))
					goto err;

				/*
				* If the window size is greater than 1, then calculate
				* val[i=2..2^winsize-1]. Powers are computed as a*a^(i-1) (even
				* powers could instead be computed as (a^(i/2))^2 to use the slight
				* performance advantage of sqr over mul).
				*/
				if (window > 1) {
					if (!BN_mod_mul_montgomery(&tmp, &am, &am, mont, ctx))
						goto err;
					if (!MOD_EXP_CTIME_COPY_TO_PREBUF
						(&tmp, top, powerbuf, 2, numPowers))
						goto err;
					for (i = 3; i < numPowers; i++) {
						/* Calculate a^i = a^(i-1) * a */
						if (!BN_mod_mul_montgomery(&tmp, &am, &tmp, mont, ctx))
							goto err;
						if (!MOD_EXP_CTIME_COPY_TO_PREBUF
							(&tmp, top, powerbuf, i, numPowers))
							goto err;
					}
				}

				bits--;
				for (wvalue = 0, i = bits % window; i >= 0; i--, bits--)
					wvalue = (wvalue << 1) + BN_is_bit_set(p, bits);
				if (!MOD_EXP_CTIME_COPY_FROM_PREBUF
					(&tmp, top, powerbuf, wvalue, numPowers))
					goto err;

				/*
				* Scan the exponent one window at a time starting from the most
				* significant bits.
				*/
				while (bits >= 0) {
					wvalue = 0;         /* The 'value' of the window */

					/* Scan the window, squaring the result as we go */
					for (i = 0; i < window; i++, bits--) {
						if (!BN_mod_mul_montgomery(&tmp, &tmp, &tmp, mont, ctx))
							goto err;
						wvalue = (wvalue << 1) + BN_is_bit_set(p, bits);
					}

					/*
					* Fetch the appropriate pre-computed value from the pre-buf
					*/
					if (!MOD_EXP_CTIME_COPY_FROM_PREBUF
						(&am, top, powerbuf, wvalue, numPowers))
						goto err;

					/* Multiply the result into the intermediate result */
					if (!BN_mod_mul_montgomery(&tmp, &tmp, &am, mont, ctx))
						goto err;
				}
			}

		/* Convert the final result from montgomery to standard format */
#if defined(SPARC_T4_MONT)
		if (OPENSSL_sparcv9cap_P[0] & (SPARCV9_VIS3 | SPARCV9_PREFER_FPU)) {
			am.d[0] = 1;            /* borrow am */
			for (i = 1; i < top; i++)
				am.d[i] = 0;
			if (!BN_mod_mul_montgomery(rr, &tmp, &am, mont, ctx))
				goto err;
		}
		else
#endif
			if (!BN_from_montgomery(rr, &tmp, mont, ctx))
				goto err;
		ret = 1;
	err:
		if ((in_mont == NULL) && (mont != NULL))
			BN_MONT_CTX_free(mont);
		if (powerbuf != NULL) {
			OPENSSL_cleanse(powerbuf, powerbufLen);
			if (powerbufFree)
				OPENSSL_free(powerbufFree);
		}
		BN_CTX_end(ctx);
		return (ret);
	}


int BN_mod_exp_mont(BIGNUM *rr, const BIGNUM *a, const BIGNUM *p,
                    const BIGNUM *m, BN_CTX *ctx, BN_MONT_CTX *in_mont)
{
    int i, j, bits, ret = 0, wstart, wend, window, wvalue;
    int start = 1;
    BIGNUM *d, *r;
    const BIGNUM *aa;
    /* Table of variables obtained from 'ctx' */
    BIGNUM *val[TABLE_SIZE];
    BN_MONT_CTX *mont = NULL;

    if (BN_get_flags(p, BN_FLG_CONSTTIME) != 0) {
        return BN_mod_exp_mont_consttime(rr, a, p, m, ctx, in_mont);
    }

    bn_check_top(a);
    bn_check_top(p);
    bn_check_top(m);

    if (!BN_is_odd(m)) {
        BNerr(BN_F_BN_MOD_EXP_MONT, BN_R_CALLED_WITH_EVEN_MODULUS);
        return (0);
    }
    bits = BN_num_bits(p);
    if (bits == 0) {
        ret = BN_one(rr);
        return ret;
    }

    BN_CTX_start(ctx);
    d = BN_CTX_get(ctx);
    r = BN_CTX_get(ctx);
    val[0] = BN_CTX_get(ctx);
    if (!d || !r || !val[0])
        goto err;

    /*
     * If this is not done, things will break in the montgomery part
     */

    if (in_mont != NULL)
        mont = in_mont;
    else {
        if ((mont = BN_MONT_CTX_new()) == NULL)
            goto err;
        if (!BN_MONT_CTX_set(mont, m, ctx))
            goto err;
    }

    if (a->neg || BN_ucmp(a, m) >= 0) {
        if (!BN_nnmod(val[0], a, m, ctx))
            goto err;
        aa = val[0];
    } else
        aa = a;
    if (BN_is_zero(aa)) {
        BN_zero(rr);
        ret = 1;
        goto err;
    }
    if (!BN_to_montgomery(val[0], aa, mont, ctx))
        goto err;               /* 1 */

    window = BN_window_bits_for_exponent_size(bits);
    if (window > 1) {
        if (!BN_mod_mul_montgomery(d, val[0], val[0], mont, ctx))
            goto err;           /* 2 */
        j = 1 << (window - 1);
        for (i = 1; i < j; i++) {
            if (((val[i] = BN_CTX_get(ctx)) == NULL) ||
                !BN_mod_mul_montgomery(val[i], val[i - 1], d, mont, ctx))
                goto err;
        }
    }

    start = 1;                  /* This is used to avoid multiplication etc
                                 * when there is only the value '1' in the
                                 * buffer. */
    wvalue = 0;                 /* The 'value' of the window */
    wstart = bits - 1;          /* The top bit of the window */
    wend = 0;                   /* The bottom bit of the window */

#if 1                           /* by Shay Gueron's suggestion */
    j = m->top;                 /* borrow j */
    if (m->d[j - 1] & (((BN_ULONG)1) << (BN_BITS2 - 1))) {
        if (bn_wexpand(r, j) == NULL)
            goto err;
        /* 2^(top*BN_BITS2) - m */
        r->d[0] = (0 - m->d[0]) & BN_MASK2;
        for (i = 1; i < j; i++)
            r->d[i] = (~m->d[i]) & BN_MASK2;
        r->top = j;
        /*
         * Upper words will be zero if the corresponding words of 'm' were
         * 0xfff[...], so decrement r->top accordingly.
         */
        bn_correct_top(r);
    } else
#endif
    if (!BN_to_montgomery(r, BN_value_one(), mont, ctx))
        goto err;
    for (;;) {
        if (BN_is_bit_set(p, wstart) == 0) {
            if (!start) {
                if (!BN_mod_mul_montgomery(r, r, r, mont, ctx))
                    goto err;
            }
            if (wstart == 0)
                break;
            wstart--;
            continue;
        }
        /*
         * We now have wstart on a 'set' bit, we now need to work out how bit
         * a window to do.  To do this we need to scan forward until the last
         * set bit before the end of the window
         */
        j = wstart;
        wvalue = 1;
        wend = 0;
        for (i = 1; i < window; i++) {
            if (wstart - i < 0)
                break;
            if (BN_is_bit_set(p, wstart - i)) {
                wvalue <<= (i - wend);
                wvalue |= 1;
                wend = i;
            }
        }

        /* wend is the size of the current window */
        j = wend + 1;
        /* add the 'bytes above' */
        if (!start)
            for (i = 0; i < j; i++) {
                if (!BN_mod_mul_montgomery(r, r, r, mont, ctx))
                    goto err;
            }

        /* wvalue will be an odd number < 2^window */
        if (!BN_mod_mul_montgomery(r, r, val[wvalue >> 1], mont, ctx))
            goto err;

        /* move the 'window' down further */
        wstart -= wend + 1;
        wvalue = 0;
        start = 0;
        if (wstart < 0)
            break;
    }
#if defined(SPARC_T4_MONT)
    if (OPENSSL_sparcv9cap_P[0] & (SPARCV9_VIS3 | SPARCV9_PREFER_FPU)) {
        j = mont->N.top;        /* borrow j */
        val[0]->d[0] = 1;       /* borrow val[0] */
        for (i = 1; i < j; i++)
            val[0]->d[i] = 0;
        val[0]->top = j;
        if (!BN_mod_mul_montgomery(rr, r, val[0], mont, ctx))
            goto err;
    } else
#endif
    if (!BN_from_montgomery(rr, r, mont, ctx))
        goto err;
    ret = 1;
 err:
    if ((in_mont == NULL) && (mont != NULL))
        BN_MONT_CTX_free(mont);
    BN_CTX_end(ctx);
    bn_check_top(rr);
    return (ret);
}

}  // namespace youmecommon
