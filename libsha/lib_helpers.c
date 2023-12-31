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

#include "lib_helpers.h"
#include <tklib/limits.h>

// LCOV_EXCL_START
// NOLINTBEGIN(bugprone-easily-swappable-parameters)

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

// NOLINTEND(bugprone-easily-swappable-parameters)
// LCOV_EXCL_STOP