/* crypto/crypto.h */
/* ====================================================================
 * Copyright (c) 1998-2006 The OpenSSL Project.  All rights reserved.
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
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 * ECDH support in OpenSSL originally developed by
 * SUN MICROSYSTEMS, INC., and contributed to the OpenSSL project.
 */

#ifndef HEADER_CRYPTO_H
# define HEADER_CRYPTO_H

#include <stdlib.h>

#include <crypto/e_os2.h>

# ifndef OPENSSL_NO_FP_API
#include <stdio.h>
# endif


# ifdef CHARSET_EBCDIC
#include <openssl/ebcdic.h>
# endif

/*
 * Resolve problems on some operating systems with symbol names that clash
 * one way or another
 */

//#ifdef  __cplusplus
//extern "C" {
//#endif
namespace youmecommon {

# define CRYPTO_LOCK_ERR                 1
# define CRYPTO_LOCK_EX_DATA             2
# define CRYPTO_LOCK_X509                3
# define CRYPTO_LOCK_X509_INFO           4
# define CRYPTO_LOCK_X509_PKEY           5
# define CRYPTO_LOCK_X509_CRL            6
# define CRYPTO_LOCK_X509_REQ            7
# define CRYPTO_LOCK_DSA                 8
# define CRYPTO_LOCK_RSA                 9
# define CRYPTO_LOCK_EVP_PKEY            10
# define CRYPTO_LOCK_X509_STORE          11
# define CRYPTO_LOCK_SSL_CTX             12
# define CRYPTO_LOCK_SSL_CERT            13
# define CRYPTO_LOCK_SSL_SESSION         14
# define CRYPTO_LOCK_SSL_SESS_CERT       15
# define CRYPTO_LOCK_SSL                 16
# define CRYPTO_LOCK_SSL_METHOD          17
# define CRYPTO_LOCK_RAND                18
# define CRYPTO_LOCK_RAND2               19
# define CRYPTO_LOCK_MALLOC              20
# define CRYPTO_LOCK_BIO                 21
# define CRYPTO_LOCK_GETHOSTBYNAME       22
# define CRYPTO_LOCK_GETSERVBYNAME       23
# define CRYPTO_LOCK_READDIR             24
# define CRYPTO_LOCK_RSA_BLINDING        25
# define CRYPTO_LOCK_DH                  26
# define CRYPTO_LOCK_MALLOC2             27
# define CRYPTO_LOCK_DSO                 28
# define CRYPTO_LOCK_DYNLOCK             29
# define CRYPTO_LOCK_ENGINE              30
# define CRYPTO_LOCK_UI                  31
# define CRYPTO_LOCK_ECDSA               32
# define CRYPTO_LOCK_EC                  33
# define CRYPTO_LOCK_ECDH                34
# define CRYPTO_LOCK_BN                  35
# define CRYPTO_LOCK_EC_PRE_COMP         36
# define CRYPTO_LOCK_STORE               37
# define CRYPTO_LOCK_COMP                38
# define CRYPTO_LOCK_FIPS                39
# define CRYPTO_LOCK_FIPS2               40
# define CRYPTO_NUM_LOCKS                41

# define CRYPTO_LOCK             1
# define CRYPTO_UNLOCK           2
# define CRYPTO_READ             4
# define CRYPTO_WRITE            8
# define ENTROPY_NEEDED 32      /* require 256 bits = 32 bytes of randomness */
#define  OPENSSL_malloc malloc
#define OPENSSL_malloc_locked malloc
#define  OPENSSL_free free
#define  OPENSSL_free_locked free
# define RSAerr(f,r)
# define BNerr(f,r)   
#  define CRYPTO_w_lock(a)
#  define CRYPTO_w_unlock(a)
#  define CRYPTO_r_lock(a)
#  define CRYPTO_r_unlock(a)
#  define CRYPTO_add(a,b,c)       ((*(a))+=(b))
# define CTXDBG_ENTRY(str, ctx)
# define CTXDBG_EXIT(ctx)
# define CTXDBG_RET(ctx,ret)
# define EVPerr(f,r)
#  define MD_DIGEST_LENGTH        MD5_DIGEST_LENGTH
#  define MD_Init(a)              EVP_DigestInit_ex(a,EVP_md5(), NULL)
#  define MD(a,b,c)               EVP_Digest(a,b,c,NULL,EVP_md5(), NULL)
# define MD_Update(a,b,c)        EVP_DigestUpdate(a,b,c)
# define MD_Final(a,b)           EVP_DigestFinal_ex(a,b,NULL)
#define NID_rsaEncryption               6
#define NID_rsa         19
#define NID_md5         4
#define NID_md5WithRSA          104
#define NID_md5WithRSAEncryption                8
# define ENTROPY_NEEDED 32      /* require 256 bits = 32 bytes of randomness */
void OPENSSL_cleanse(void *ptr, size_t len);
int OPENSSL_isservice(void);
}  // namespace youmecommon
//#ifdef  __cplusplus
//}
//#endif
#endif
