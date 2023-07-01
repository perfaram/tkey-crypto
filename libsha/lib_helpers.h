#ifndef SRC_LIBSHA_LIB_HELPERS_H
#define SRC_LIBSHA_LIB_HELPERS_H

#ifdef __riscv
#include <types.h>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include <errno_compat.h>

errno_t memset_s(void *, size_t, int, size_t);

#endif