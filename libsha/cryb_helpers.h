#ifndef SRC_LIBSHA_CRYB_HELPERS_H
#define SRC_LIBSHA_CRYB_HELPERS_H

#ifdef __riscv
#include <types.h>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#define CRYB_ROR(N)                                            \
	static inline uint##N##_t ror##N(uint##N##_t i, int n)     \
	{                                                          \
		return (i << (-n & ((N)-1)) | i >> (n & ((N)-1)));     \
	}

#define CRYB_ROL(N)                                            \
	static inline uint##N##_t rol##N(uint##N##_t i, int n)     \
	{                                                          \
		return (i << (n & ((N)-1)) | i >> (-n & ((N)-1)));     \
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

#endif