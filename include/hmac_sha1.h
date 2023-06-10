// Copyright (C) 2023 - Perceval Faramaz
// SPDX-License-Identifier: GPL-2.0-only

#ifndef HMAC_SHA1_H
#define HMAC_SHA1_H

#include <sha1.h>
#ifdef __riscv
#include <types.h>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#define HMAC_SHA1_MAC_LEN SHA1_DIGEST_LEN

typedef struct {
	sha1_ctx ictx;
	sha1_ctx octx;
} hmac_sha1_ctx;

void hmac_sha1_init(hmac_sha1_ctx *ctx, const void *key, size_t keylen);

void hmac_sha1_update(hmac_sha1_ctx *ctx, const void *buf, size_t len);

void hmac_sha1_final(hmac_sha1_ctx *ctx, uint8_t *mac);

void hmac_sha1_complete(uint8_t *mac, const void *key, size_t keylen,
			const void *buf, size_t len);

#endif