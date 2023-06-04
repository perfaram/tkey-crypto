// Copyright (C) 2023 - Perceval Faramaz
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TK_HMAC_H
#define TK_HMAC_H

#include <sha1.h>
#include <types.h>

#define HMAC_SHA1_MAC_LEN SHA1_DIGEST_LEN

typedef struct {
	sha1_ctx ictx;
	sha1_ctx octx;
} hmac_sha1_ctx;

void hmac_sha1_init(hmac_sha1_ctx *ctx, const void *key, size_t keylen);

void hmac_sha1_update(hmac_sha1_ctx *ctx, const void *buf, size_t len);

void hmac_sha1_final(hmac_sha1_ctx *ctx, uint8_t *mac);

void hmac_sha1_complete(const void *key, size_t keylen, const void *buf,
			size_t len, uint8_t *mac);

#endif