// Copyright (C) 2023 - Perceval Faramaz
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TK_SHA1_H
#define TK_SHA1_H

#include <types.h>

#define SHA1_DIGEST_LEN 20
#define SHA1_BLOCK_LEN 64

// SHA1 hashing context
typedef struct {
	uint32_t count[2];
	uint32_t hash[5];
	uint32_t wbuf[16];
} sha1_ctx;

void sha1_init(sha1_ctx ctx[1]);
void sha1_update(const uint8_t data[], size_t len, sha1_ctx ctx[1]);
void sha1_final(uint8_t hval[], sha1_ctx ctx[1]);

void sha1(uint8_t hval[], const uint8_t data[], size_t len);

#endif
