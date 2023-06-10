/*-
 * Copyright (c) 2012 The University of Oslo
 * Copyright (c) 2012-2016 Dag-Erling Smørgrav
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
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
 */

#ifdef __riscv
#include <lib.h>
#include <tklib/limits.h>
#else
#include <string.h>
#endif
#include <errno_compat.h>
#include <sha1.h>

#define CRYB_ROR(N)                                                            \
	static inline uint##N##_t ror##N(uint##N##_t i, int n)                 \
	{                                                                      \
		return (i << (-n & ((N)-1)) | i >> (n & ((N)-1)));             \
	}

#define CRYB_ROL(N)                                                            \
	static inline uint##N##_t rol##N(uint##N##_t i, int n)                 \
	{                                                                      \
		return (i << (n & ((N)-1)) | i >> (-n & ((N)-1)));             \
	}

CRYB_ROL(32);

static inline uint32_t be32dec(const void *p)
{
	return ((uint32_t)((const uint8_t *)p)[3] |
		(uint32_t)((const uint8_t *)p)[2] << 8 |
		(uint32_t)((const uint8_t *)p)[1] << 16 |
		(uint32_t)((const uint8_t *)p)[0] << 24);
}

static inline void be32decv(uint32_t *u32, const void *p, size_t n)
{
	for (const uint8_t *u8 = p; n--; u8 += sizeof *u32, u32++)
		*u32 = be32dec(u8);
}

static inline void be32enc(void *p, uint32_t u32)
{
	((uint8_t *)p)[3] = u32 & 0xff;
	((uint8_t *)p)[2] = (u32 >> 8) & 0xff;
	((uint8_t *)p)[1] = (u32 >> 16) & 0xff;
	((uint8_t *)p)[0] = (u32 >> 24) & 0xff;
}

static inline void be32encv(void *p, const uint32_t *u32, size_t n)
{
	for (uint8_t *u8 = p; n--; u8 += sizeof *u32, u32++)
		be32enc(u8, *u32);
}

static inline void be64enc(void *p, uint64_t u64)
{
	((uint8_t *)p)[7] = u64 & 0xff;
	((uint8_t *)p)[6] = (u64 >> 8) & 0xff;
	((uint8_t *)p)[5] = (u64 >> 16) & 0xff;
	((uint8_t *)p)[4] = (u64 >> 24) & 0xff;
	((uint8_t *)p)[3] = (u64 >> 32) & 0xff;
	((uint8_t *)p)[2] = (u64 >> 40) & 0xff;
	((uint8_t *)p)[1] = (u64 >> 48) & 0xff;
	((uint8_t *)p)[0] = (u64 >> 56) & 0xff;
}

/*
 * Like memset(), but checks for overflow and guarantees that the buffer
 * is overwritten even if the data will never be read.
 *
 * ISO/IEC 9899:2011 K.3.7.4.1
 */
errno_t memset_s(void *d, size_t dsz, int c, size_t n)
{
	volatile uint8_t *D;
	unsigned int i;
	uint8_t C;

	if (d == NULL)
		return (EINVAL);

	if (dsz > SIZE_MAX || n > SIZE_MAX)
		return (ERANGE);

	for (D = d, C = (uint8_t)c, i = 0; i < n && i < dsz; ++i)
		D[i] = C;
	if (n > dsz)
		return (EOVERFLOW);
	return (0);
}

static uint32_t sha1_h[5] = {
    0x67452301U, 0xefcdab89U, 0x98badcfeU, 0x10325476U, 0xc3d2e1f0U,
};

static uint32_t sha1_k[4] = {
    0x5a827999U,
    0x6ed9eba1U,
    0x8f1bbcdcU,
    0xca62c1d6U,
};

void sha1_init(sha1_ctx *ctx)
{

	memset(ctx, 0, sizeof *ctx);
	memcpy(ctx->h, sha1_h, sizeof ctx->h);
}

#define sha1_ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define sha1_parity(x, y, z) (((x) ^ (y) ^ (z)))
#define sha1_maj(x, y, z) ((((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z))))
#define sha1_step(t, a, f, e, w)                                               \
	do {                                                                   \
		uint32_t T =                                                   \
		    rol32(a, 5) + (f) + (e) + sha1_k[(t) / 20] + (w)[t];       \
		(e) = d;                                                       \
		d = c;                                                         \
		c = rol32(b, 30);                                              \
		b = a;                                                         \
		(a) = T;                                                       \
	} while (0)

static void sha1_compute(sha1_ctx *ctx, const uint8_t *block)
{
	uint32_t w[80], a, b, c, d, e;
	unsigned int i;

	be32decv(w, block, 16);
	for (i = 16; i < 80; ++i) {
		w[i] = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
		w[i] = rol32(w[i], 1);
	}
	a = ctx->h[0];
	b = ctx->h[1];
	c = ctx->h[2];
	d = ctx->h[3];
	e = ctx->h[4];

	sha1_step(0, a, sha1_ch(b, c, d), e, w);
	sha1_step(1, a, sha1_ch(b, c, d), e, w);
	sha1_step(2, a, sha1_ch(b, c, d), e, w);
	sha1_step(3, a, sha1_ch(b, c, d), e, w);
	sha1_step(4, a, sha1_ch(b, c, d), e, w);
	sha1_step(5, a, sha1_ch(b, c, d), e, w);
	sha1_step(6, a, sha1_ch(b, c, d), e, w);
	sha1_step(7, a, sha1_ch(b, c, d), e, w);
	sha1_step(8, a, sha1_ch(b, c, d), e, w);
	sha1_step(9, a, sha1_ch(b, c, d), e, w);
	sha1_step(10, a, sha1_ch(b, c, d), e, w);
	sha1_step(11, a, sha1_ch(b, c, d), e, w);
	sha1_step(12, a, sha1_ch(b, c, d), e, w);
	sha1_step(13, a, sha1_ch(b, c, d), e, w);
	sha1_step(14, a, sha1_ch(b, c, d), e, w);
	sha1_step(15, a, sha1_ch(b, c, d), e, w);
	sha1_step(16, a, sha1_ch(b, c, d), e, w);
	sha1_step(17, a, sha1_ch(b, c, d), e, w);
	sha1_step(18, a, sha1_ch(b, c, d), e, w);
	sha1_step(19, a, sha1_ch(b, c, d), e, w);

	sha1_step(20, a, sha1_parity(b, c, d), e, w);
	sha1_step(21, a, sha1_parity(b, c, d), e, w);
	sha1_step(22, a, sha1_parity(b, c, d), e, w);
	sha1_step(23, a, sha1_parity(b, c, d), e, w);
	sha1_step(24, a, sha1_parity(b, c, d), e, w);
	sha1_step(25, a, sha1_parity(b, c, d), e, w);
	sha1_step(26, a, sha1_parity(b, c, d), e, w);
	sha1_step(27, a, sha1_parity(b, c, d), e, w);
	sha1_step(28, a, sha1_parity(b, c, d), e, w);
	sha1_step(29, a, sha1_parity(b, c, d), e, w);
	sha1_step(30, a, sha1_parity(b, c, d), e, w);
	sha1_step(31, a, sha1_parity(b, c, d), e, w);
	sha1_step(32, a, sha1_parity(b, c, d), e, w);
	sha1_step(33, a, sha1_parity(b, c, d), e, w);
	sha1_step(34, a, sha1_parity(b, c, d), e, w);
	sha1_step(35, a, sha1_parity(b, c, d), e, w);
	sha1_step(36, a, sha1_parity(b, c, d), e, w);
	sha1_step(37, a, sha1_parity(b, c, d), e, w);
	sha1_step(38, a, sha1_parity(b, c, d), e, w);
	sha1_step(39, a, sha1_parity(b, c, d), e, w);

	sha1_step(40, a, sha1_maj(b, c, d), e, w);
	sha1_step(41, a, sha1_maj(b, c, d), e, w);
	sha1_step(42, a, sha1_maj(b, c, d), e, w);
	sha1_step(43, a, sha1_maj(b, c, d), e, w);
	sha1_step(44, a, sha1_maj(b, c, d), e, w);
	sha1_step(45, a, sha1_maj(b, c, d), e, w);
	sha1_step(46, a, sha1_maj(b, c, d), e, w);
	sha1_step(47, a, sha1_maj(b, c, d), e, w);
	sha1_step(48, a, sha1_maj(b, c, d), e, w);
	sha1_step(49, a, sha1_maj(b, c, d), e, w);
	sha1_step(50, a, sha1_maj(b, c, d), e, w);
	sha1_step(51, a, sha1_maj(b, c, d), e, w);
	sha1_step(52, a, sha1_maj(b, c, d), e, w);
	sha1_step(53, a, sha1_maj(b, c, d), e, w);
	sha1_step(54, a, sha1_maj(b, c, d), e, w);
	sha1_step(55, a, sha1_maj(b, c, d), e, w);
	sha1_step(56, a, sha1_maj(b, c, d), e, w);
	sha1_step(57, a, sha1_maj(b, c, d), e, w);
	sha1_step(58, a, sha1_maj(b, c, d), e, w);
	sha1_step(59, a, sha1_maj(b, c, d), e, w);

	sha1_step(60, a, sha1_parity(b, c, d), e, w);
	sha1_step(61, a, sha1_parity(b, c, d), e, w);
	sha1_step(62, a, sha1_parity(b, c, d), e, w);
	sha1_step(63, a, sha1_parity(b, c, d), e, w);
	sha1_step(64, a, sha1_parity(b, c, d), e, w);
	sha1_step(65, a, sha1_parity(b, c, d), e, w);
	sha1_step(66, a, sha1_parity(b, c, d), e, w);
	sha1_step(67, a, sha1_parity(b, c, d), e, w);
	sha1_step(68, a, sha1_parity(b, c, d), e, w);
	sha1_step(69, a, sha1_parity(b, c, d), e, w);
	sha1_step(70, a, sha1_parity(b, c, d), e, w);
	sha1_step(71, a, sha1_parity(b, c, d), e, w);
	sha1_step(72, a, sha1_parity(b, c, d), e, w);
	sha1_step(73, a, sha1_parity(b, c, d), e, w);
	sha1_step(74, a, sha1_parity(b, c, d), e, w);
	sha1_step(75, a, sha1_parity(b, c, d), e, w);
	sha1_step(76, a, sha1_parity(b, c, d), e, w);
	sha1_step(77, a, sha1_parity(b, c, d), e, w);
	sha1_step(78, a, sha1_parity(b, c, d), e, w);
	sha1_step(79, a, sha1_parity(b, c, d), e, w);

	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
	ctx->h[4] += e;
}

void sha1_update(sha1_ctx *ctx, const uint8_t buf[], size_t len)
{
	size_t copylen;

	while (len) {
		if (ctx->blocklen > 0 || len < sizeof ctx->block) {
			copylen = sizeof ctx->block - ctx->blocklen;
			if (copylen > len)
				copylen = len;
			memcpy(ctx->block + ctx->blocklen, buf, copylen);
			ctx->blocklen += copylen;
			if (ctx->blocklen == sizeof ctx->block) {
				sha1_compute(ctx, ctx->block);
				ctx->blocklen = 0;
			}
		} else {
			copylen = sizeof ctx->block;
			sha1_compute(ctx, buf);
		}
		ctx->bitlen += copylen * 8;
		buf += copylen;
		len -= copylen;
	}
}

void sha1_final(sha1_ctx *ctx, uint8_t *digest)
{

	ctx->block[ctx->blocklen++] = 0x80;
	memset(ctx->block + ctx->blocklen, 0,
	       sizeof ctx->block - ctx->blocklen);
	if (ctx->blocklen > 56) {
		sha1_compute(ctx, ctx->block);
		ctx->blocklen = 0;
		memset(ctx->block, 0, sizeof ctx->block);
	}
	be64enc(ctx->block + 56, ctx->bitlen);
	sha1_compute(ctx, ctx->block);
	be32encv(digest, ctx->h, 5);
	memset_s(ctx, 0, sizeof *ctx, sizeof *ctx);
}

void sha1_complete(uint8_t digest[], const uint8_t buf[], size_t len)
{
	sha1_ctx ctx;

	sha1_init(&ctx);
	sha1_update(&ctx, buf, len);
	sha1_final(&ctx, digest);
}