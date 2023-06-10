/*
 * Copyright (c) 2013 Corey Tabaka
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ERRNO_COMPAT_H
#define ERRNO_COMPAT_H

typedef int errno_t;

#define EPERM 1 /* Not super-user */

#define EINTR 4	  /* Interrupted system call */
#define EIO 5	  /* I/O error */
#define ENXIO 6	  /* No such device or address */
#define E2BIG 7	  /* Arg list too long */
#define ENOEXEC 8 /* Exec format error */

#define ENOMEM 12 /* Not enough core */
#define EACCES 13 /* Permission denied */
#define EFAULT 14 /* Bad address */

#define EINVAL 22 /* Invalid argument */

#define EFBIG 27  /* File too large */
#define ENOSPC 28 /* No space left on device */
#define ESPIPE 29 /* Illegal seek */

#define EDOM 33	  /* Math arg out of domain of func */
#define ERANGE 34 /* Math result not representable */
#define ENOMSG 35 /* No message of desired type */

#define ENOSTR 60  /* Device not a stream */
#define ENODATA 61 /* No data (for no delay io) */
#define ETIME 62   /* Timer expired */

#define ECOMM 70  /* Communication error on send */
#define EPROTO 71 /* Protocol error */

#define EBADMSG 77 /* Trying to read unreadable message */
#define EFTYPE 79  /* Inappropriate file type or format */

#define ENOSYS 88 /* Function not implemented */

#define ENAMETOOLONG 91 /* File or path name too long */

#define ENOBUFS 105 /* No buffer space available */

#define EMSGSIZE 122 /* Message too long */

#define ESTALE 133
#define ENOTSUP 134 /* Not supported */

#define EOVERFLOW 139 /* Value too large for defined data type */

#endif
