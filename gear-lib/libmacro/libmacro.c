/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "libmacro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined (__linux__) || defined (__CYGWIN__)
#include <unistd.h>
#endif

#include <stdint.h>
#include <signal.h>
#include <errno.h>

void *memdup(const void *src, size_t len)
{
    void *dst = calloc(1, len);
    if (LIKELY(dst != NULL)) {
        memcpy(dst, src, len);
    }
    return dst;
}

struct iovec *iovec_create(size_t len)
{
    struct iovec *vec = CALLOC(1, struct iovec);
    if (LIKELY(vec != NULL)) {
        vec->iov_len = len;
        vec->iov_base = calloc(1, len);
        if (UNLIKELY(vec->iov_base == NULL)) {
            free(vec);
            vec = NULL;
        }
    }
    return vec;
}

void iovec_destroy(struct iovec *vec)
{
    if (LIKELY(vec != NULL)) {
        /* free(NULL) do nop */
        free(vec->iov_base);
        free(vec);
    }
}

/**
 * Fast little endian check
 * NOTE: not applicable for PDP endian
 */
bool is_little_endian(void)
{
    static uint16_t x = 0x01;
    return *((uint8_t *) &x);
}

