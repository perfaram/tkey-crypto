// Copyright (C) 2023 - Perceval Faramaz
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SHA1_H
#define SHA1_H

#ifdef __riscv
#include <types.h>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#define SHA1_DIGEST_LEN 20
#define SHA1_BLOCK_LEN 64

// SHA1 hashing context
typedef struct {
	uint8_t block[64];
	unsigned int blocklen;
	uint32_t h[5];
	uint64_t bitlen;
} sha1_ctx;

void sha1_init(sha1_ctx *ctx);
void sha1_update(sha1_ctx *ctx, const uint8_t buf[], size_t len);
void sha1_final(sha1_ctx *ctx, uint8_t digest[]);

void sha1_complete(uint8_t digest[], const uint8_t buf[], size_t len);

#endif
