/* Copyright (C) 2002 Jean-Marc Valin */
/**
   @file misc.h
   @brief Various compatibility routines for Speex
*/
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MISC_H
#define MISC_H

#ifndef SPEEX_VERSION
#define SPEEX_MAJOR_VERSION 1         /**< Major Speex version. */
#define SPEEX_MINOR_VERSION 1         /**< Minor Speex version. */
#define SPEEX_MICRO_VERSION 12        /**< Micro Speex version. */
#define SPEEX_EXTRA_VERSION ""        /**< Extra Speex version. */
#define SPEEX_VERSION "speex-1.1.12"  /**< Speex version string. */
#endif

#include "arch.h"

#ifndef RELEASE
/** Print a named vector to stdout */
void print_vec(float *vec, int len, char *name);
#endif

/** Convert big endian */
spx_uint32_t be_int(spx_uint32_t i);
/** Convert little endian */
spx_uint32_t le_int(spx_uint32_t i);

/** Speex wrapper for calloc. To do your own dynamic allocation, all you need to do is replace this function, speex_realloc and speex_free */
void *speex_alloc (int size);

/** Same as speex_alloc, except that the area is only needed inside a Speex call (might cause problem with wideband though) */
void *speex_alloc_scratch (int size);

/** Speex wrapper for realloc. To do your own dynamic allocation, all you need to do is replace this function, speex_alloc and speex_free */
void *speex_realloc (void *ptr, int size);

/** Speex wrapper for calloc. To do your own dynamic allocation, all you need to do is replace this function, speex_realloc and speex_alloc */
void speex_free (void *ptr);

/** Same as speex_alloc, except that the area is only needed inside a Speex call (might cause problem with wideband though) */
void speex_free_scratch (void *ptr);

/** Speex wrapper for mem_move */
void *speex_move (void *dest, void *src, int n);

/** Speex wrapper for memcpy */
void speex_memcpy_bytes(char *dst, char *src, int nbytes);

/** Speex wrapper for memset */
void speex_memset_bytes(char *dst, char src, int nbytes);

/** Print error message to stderr */
void speex_error(const char *str);

/** Print warning message to stderr */
void speex_warning(const char *str);

/** Print warning message with integer argument to stderr */
void speex_warning_int(const char *str, int val);

/** Generate a vector of random numbers */
void speex_rand_vec(float std, spx_sig_t *data, int len);

/** Generate a random number */
spx_word32_t speex_rand(spx_word16_t std, spx_int32_t *seed);

/** Speex wrapper for putc */
void _speex_putc(int ch, void *file);

#endif
