/*******************************************************************************
 * pk_rsa.c
 *
 * History:
 *  2015/06/25 - [Zhi He] create file
 *
 * Copyright (C) 2015 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include "cryptography_if.h"

#ifndef DNOT_INCLUDE_C_HEADER
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include <bldfunc.h>
#endif

#include "big_number.h"

//ASN1
#define ASN1_BOOLEAN                 0x01
#define ASN1_INTEGER                 0x02
#define ASN1_BIT_STRING              0x03
#define ASN1_OCTET_STRING            0x04
#define ASN1_NULL                    0x05
#define ASN1_OID                     0x06
#define ASN1_UTF8_STRING             0x0C
#define ASN1_SEQUENCE                0x10
#define ASN1_SET                     0x11
#define ASN1_PRINTABLE_STRING        0x13
#define ASN1_T61_STRING              0x14
#define ASN1_IA5_STRING              0x16
#define ASN1_UTC_TIME                0x17
#define ASN1_GENERALIZED_TIME        0x18
#define ASN1_UNIVERSAL_STRING        0x1C
#define ASN1_BMP_STRING              0x1E
#define ASN1_PRIMITIVE               0x00
#define ASN1_CONSTRUCTED             0x20
#define ASN1_CONTEXT_SPECIFIC        0x80

//OIDs
static unsigned char gsOIDDigestSHA256[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01};
//{ joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistalgorithm(4) hashalgs(2) 1 }

//static unsigned char gsOIDDigestSHA384[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02};
//{ joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistalgorithm(4) hashalgs(2) 2 }

//static unsigned char gsOIDDigestSHA512[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03};
//{ joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistalgorithm(4) hashalgs(2) 3 }

#define DRSA_SIGN   1
#define DRSA_CRYPT  2

void rsa_init(rsa_context_t* ctx, int padding, int hash_id)
{
    memset(ctx, 0, sizeof(rsa_context_t));
    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

void rsa_free(rsa_context_t *ctx)
{
    big_number_free(&ctx->Vi);
    big_number_free(&ctx->Vf);
    big_number_free(&ctx->RQ);
    big_number_free(&ctx->RP);
    big_number_free(&ctx->RN);
    big_number_free(&ctx->QP);
    big_number_free(&ctx->DQ);
    big_number_free(&ctx->DP);
    big_number_free(&ctx->Q);
    big_number_free(&ctx->P);
    big_number_free(&ctx->D);
    big_number_free(&ctx->E);
    big_number_free(&ctx->N);
}

int rsa_gen_key(rsa_context_t *ctx, int (*f_rng)(void *, unsigned char *, unsigned int), void *p_rng, unsigned int nbits, int exponent)
{
    int ret;
    big_number_t P1, Q1, H, G;

    if (f_rng == NULL || nbits < 128 || exponent < 3) {
        return CRYPTO_ECODE_BAD_INPUT_DATA;
    }

    big_number_init(&P1);
    big_number_init(&Q1);
    big_number_init(&H);
    big_number_init(&G);

    D_CLEAN_IF_FAILED(big_number_lset(&ctx->E, exponent));

    do {
        D_CLEAN_IF_FAILED(big_number_gen_prime(&ctx->P, (nbits + 1) >> 1, 0, f_rng, p_rng));
        D_CLEAN_IF_FAILED(big_number_gen_prime(&ctx->Q, (nbits + 1) >> 1, 0, f_rng, p_rng));

        if (big_number_cmp_big_number(&ctx->P, &ctx->Q) < 0) {
            big_number_swap(&ctx->P, &ctx->Q);
        }

        if (big_number_cmp_big_number(&ctx->P, &ctx->Q) == 0 ) {
            continue;
        }

        D_CLEAN_IF_FAILED(big_number_mul_big_number(&ctx->N, &ctx->P, &ctx->Q));

        if (big_number_msb(&ctx->N) != nbits) {
            continue;
        }

        D_CLEAN_IF_FAILED(big_number_sub_int(&P1, &ctx->P, 1));
        D_CLEAN_IF_FAILED(big_number_sub_int(&Q1, &ctx->Q, 1));
        D_CLEAN_IF_FAILED(big_number_mul_big_number(&H, &P1, &Q1));
        D_CLEAN_IF_FAILED(big_number_gcd(&G, &ctx->E, &H));
    } while (big_number_cmp_int(&G, 1) != 0);

    D_CLEAN_IF_FAILED(big_number_inv_mod(&ctx->D , &ctx->E, &H));
    D_CLEAN_IF_FAILED(big_number_mod_big_number(&ctx->DP, &ctx->D, &P1));
    D_CLEAN_IF_FAILED(big_number_mod_big_number(&ctx->DQ, &ctx->D, &Q1));
    D_CLEAN_IF_FAILED(big_number_inv_mod(&ctx->QP, &ctx->Q, &ctx->P));

    ctx->len = (big_number_msb(&ctx->N) + 7) >> 3;

cleanup:

    big_number_free(&P1);
    big_number_free(&Q1);
    big_number_free(&H);
    big_number_free(&G);

    if (ret != 0) {
        rsa_free(ctx);
        return CRYPTO_ECODE_GENERATE_KEY_FAIL;
    }

    return CRYPTO_ECODE_OK;
}

int rsa_check_pubkey(const rsa_context_t *ctx)
{
    if (!ctx->N.p || !ctx->E.p) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    if ((ctx->N.p[0] & 1) == 0 || (ctx->E.p[0] & 1) == 0) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    if (big_number_msb(&ctx->N) < 128 || big_number_msb(&ctx->N) > D_BIG_NUMBER_MAX_BITS ) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    if (big_number_msb(&ctx->E) < 2 || big_number_cmp_big_number(&ctx->E, &ctx->N) >= 0) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    return CRYPTO_ECODE_OK;
}

int rsa_check_privkey(const rsa_context_t *ctx)
{
    int ret = CRYPTO_ECODE_OK;
    big_number_t PQ, DE, P1, Q1, H, I, G, G2, L1, L2, DP, DQ, QP;

    if ((ret = rsa_check_pubkey(ctx)) != 0) {
        return ret;
    }

    if (!ctx->P.p || !ctx->Q.p || !ctx->D.p) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    big_number_init(&PQ);
    big_number_init(&DE);
    big_number_init(&P1);
    big_number_init(&Q1);
    big_number_init(&H);
    big_number_init(&I);
    big_number_init(&G);
    big_number_init(&G2);
    big_number_init(&L1);
    big_number_init(&L2);
    big_number_init(&DP);
    big_number_init(&DQ);
    big_number_init(&QP);

    D_CLEAN_IF_FAILED(big_number_mul_big_number(&PQ, &ctx->P, &ctx->Q));
    D_CLEAN_IF_FAILED(big_number_mul_big_number(&DE, &ctx->D, &ctx->E));
    D_CLEAN_IF_FAILED(big_number_sub_int(&P1, &ctx->P, 1));
    D_CLEAN_IF_FAILED(big_number_sub_int(&Q1, &ctx->Q, 1));
    D_CLEAN_IF_FAILED(big_number_mul_big_number(&H, &P1, &Q1));
    D_CLEAN_IF_FAILED(big_number_gcd(&G, &ctx->E, &H));

    D_CLEAN_IF_FAILED(big_number_gcd(&G2, &P1, &Q1));
    D_CLEAN_IF_FAILED(big_number_div_big_number(&L1, &L2, &H, &G2));
    D_CLEAN_IF_FAILED(big_number_mod_big_number(&I, &DE, &L1));

    D_CLEAN_IF_FAILED(big_number_mod_big_number(&DP, &ctx->D, &P1));
    D_CLEAN_IF_FAILED(big_number_mod_big_number(&DQ, &ctx->D, &Q1));
    D_CLEAN_IF_FAILED(big_number_inv_mod(&QP, &ctx->Q, &ctx->P));

    if (big_number_cmp_big_number(&PQ, &ctx->N) != 0 ||
         big_number_cmp_big_number(&DP, &ctx->DP) != 0 ||
         big_number_cmp_big_number(&DQ, &ctx->DQ) != 0 ||
         big_number_cmp_big_number(&QP, &ctx->QP) != 0 ||
         big_number_cmp_int(&L2, 0) != 0 ||
         big_number_cmp_int(&I, 1) != 0 ||
         big_number_cmp_int(&G, 1) != 0) {
        ret = CRYPTO_ECODE_INVALID_KEY;
    }

cleanup:
    big_number_free(&PQ);
    big_number_free(&DE);
    big_number_free(&P1);
    big_number_free(&Q1);
    big_number_free(&H);
    big_number_free(&I);
    big_number_free(&G);
    big_number_free(&G2);
    big_number_free(&L1);
    big_number_free(&L2);
    big_number_free(&DP);
    big_number_free(&DQ);
    big_number_free(&QP);

    if (ret != 0) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    return CRYPTO_ECODE_OK;
}

int rsa_check_key_pair(const rsa_context_t* pub, const rsa_context_t* prv)
{
    if (rsa_check_pubkey(pub) != 0 || rsa_check_privkey(prv) != 0) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    if (big_number_cmp_big_number(&pub->N, &prv->N) != 0 || big_number_cmp_big_number(&pub->E, &prv->E) != 0) {
        return CRYPTO_ECODE_INVALID_KEY;
    }

    return CRYPTO_ECODE_OK;
}

static int __rsa_public(rsa_context_t* ctx, const unsigned char* input, unsigned char* output)
{
    int ret = CRYPTO_ECODE_OK;
    unsigned int olen;
    big_number_t T;

    big_number_init(&T);

    D_CLEAN_IF_FAILED(big_number_read_binary(&T, input, ctx->len));

    if (big_number_cmp_big_number(&T, &ctx->N) >= 0) {
        big_number_free(&T);
        return CRYPTO_ECODE_BAD_INPUT_DATA;
    }

    olen = ctx->len;
    D_CLEAN_IF_FAILED(big_number_exp_mod(&T, &T, &ctx->E, &ctx->N, &ctx->RN));
    D_CLEAN_IF_FAILED(big_number_write_binary(&T, output, olen));

cleanup:

    big_number_free(&T);

    if (ret != 0) {
        return CRYPTO_ECODE_PK_PUBLIC_FAILED;
    }

    return CRYPTO_ECODE_OK;
}

static int __rsa_private(rsa_context_t* ctx, const unsigned char* input, unsigned char* output)
{
    int ret;
    unsigned int olen;
    big_number_t T, T1, T2;

    big_number_init(&T);
    big_number_init(&T1);
    big_number_init(&T2);

    D_CLEAN_IF_FAILED(big_number_read_binary(&T, input, ctx->len));

    if (big_number_cmp_big_number(&T, &ctx->N) >= 0) {
        big_number_free(&T);
        return CRYPTO_ECODE_BAD_INPUT_DATA;
    }

    D_CLEAN_IF_FAILED(big_number_exp_mod(&T1, &T, &ctx->DP, &ctx->P, &ctx->RP));
    D_CLEAN_IF_FAILED(big_number_exp_mod(&T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ));

    D_CLEAN_IF_FAILED(big_number_sub_big_number(&T, &T1, &T2));
    D_CLEAN_IF_FAILED(big_number_mul_big_number(&T1, &T, &ctx->QP));
    D_CLEAN_IF_FAILED(big_number_mod_big_number(&T, &T1, &ctx->P));

    D_CLEAN_IF_FAILED(big_number_mul_big_number(&T1, &T, &ctx->Q));
    D_CLEAN_IF_FAILED(big_number_add_big_number(&T, &T2, &T1));

    olen = ctx->len;
    D_CLEAN_IF_FAILED(big_number_write_binary(&T, output, olen));

cleanup:

    big_number_free(&T);
    big_number_free(&T1);
    big_number_free(&T2);

    if (ret != 0) {
        return CRYPTO_ECODE_PK_PRIVATE_FAILED;
    }

    return CRYPTO_ECODE_OK;
}

int rsa_sha256_sign(rsa_context_t* ctx, const unsigned char* hash, unsigned char* sig)
{
    unsigned int nb_pad, olen, oid_size = 0;
    unsigned char *p = sig;

    olen = ctx->len;
    nb_pad = olen - 3;

    oid_size = sizeof(gsOIDDigestSHA256);

    nb_pad -= 10 + oid_size;
    nb_pad -= DCRYPTO_SHA256_DIGEST_LENGTH;

    if (( nb_pad < 8 ) || ( nb_pad > olen )) {
        return CRYPTO_ECODE_BAD_INPUT_DATA;
    }

    *p++ = 0;
    *p++ = DRSA_SIGN;
    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;

    //ASN1 1
    *p++ = ASN1_SEQUENCE | ASN1_CONSTRUCTED;
    *p++ = (unsigned char) (0x08 + oid_size + DCRYPTO_SHA256_DIGEST_LENGTH);

    //ASN1 2
    *p++ = ASN1_SEQUENCE | ASN1_CONSTRUCTED;
    *p++ = (unsigned char) (0x04 + oid_size);

    //OID
    *p++ = ASN1_OID;
    *p++ = oid_size & 0xFF;
    memcpy(p, gsOIDDigestSHA256, oid_size);
    p += oid_size;

    *p++ = ASN1_NULL;
    *p++ = 0x00;
    *p++ = ASN1_OCTET_STRING;
    *p++ = DCRYPTO_SHA256_DIGEST_LENGTH;

    //digest
    memcpy(p, hash, DCRYPTO_SHA256_DIGEST_LENGTH);

    return __rsa_private(ctx, sig, sig);
}

int rsa_sha256_verify(rsa_context_t* ctx, const unsigned char* hash, const unsigned char* sig)
{
    int ret;
    unsigned int len, siglen;
    unsigned char *p;
    unsigned char buf[D_BIG_NUMBER_MAX_SIZE];

    siglen = ctx->len;

    if (siglen < 16 || siglen > sizeof(buf)) {
        return CRYPTO_ECODE_BAD_INPUT_DATA;
    }

    ret = __rsa_public(ctx, sig, buf);

    if (CRYPTO_ECODE_OK != ret) {
        return ret;
    }

    p = buf;

    if (*p++ != 0 || *p++ != DRSA_SIGN) {
        return CRYPTO_ECODE_SIGNATURE_INVALID_PADDING;
    }

    while (*p != 0) {
        if (p >= buf + siglen - 1 || *p != 0xFF) {
            return CRYPTO_ECODE_SIGNATURE_INVALID_PADDING;
        }
        p++;
    }

    p++;
    len = siglen - ( p - buf );

    //check ASN1 header 1
    if ((ASN1_CONSTRUCTED | ASN1_SEQUENCE) != p[0]) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p ++;

    //check ASN1 size 1
    if (p[0] & 0x80) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    if (p[0] + 2 != len) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p ++;

    //check ASN1 header 2
    if ((ASN1_CONSTRUCTED | ASN1_SEQUENCE) != p[0]) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p ++;

    //check ASN1 size 2
    if (p[0] + 6 + DCRYPTO_SHA256_DIGEST_LENGTH != len) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p ++;

    //check ASN1 for OID
    if ((ASN1_OID != p[0]) || (sizeof(gsOIDDigestSHA256) != p[1])) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p += 2;

    //check OID
    if (memcmp(gsOIDDigestSHA256, p, sizeof(gsOIDDigestSHA256))) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p += sizeof(gsOIDDigestSHA256);

    if ((p[0] != ASN1_NULL) || (0 != p[1])) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p += 2;

    if ((p[0] != ASN1_OCTET_STRING) || (DCRYPTO_SHA256_DIGEST_LENGTH != p[1])) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }
    p += 2;

    if (memcmp( p, hash, DCRYPTO_SHA256_DIGEST_LENGTH) != 0) {
        return CRYPTO_ECODE_SIGNATURE_VERIFY_FAIL;
    }

    return CRYPTO_ECODE_OK;
}

