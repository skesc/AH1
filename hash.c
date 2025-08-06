/* -- hash.c
 * A crude implementation of AH1, a non-cryptographic hashing algorithm.
 * 
 * MIT License
 * 
 * Copyright (c) 2025 Abhigyan <nourr@duck.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "hash.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)
#include <sys/byteorder.h>
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)
#include <sys/endian.h>
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)
#include <sys/types.h>
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)
#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else
#include <byteswap.h>

#endif /* _MSC_VER */

#ifdef WORDS_BIGENDIAN
#define uint32_in_expected_order(x) (bswap_32(x))
#define uint64_in_expected_order(x) (bswap_64(x))
#else
#define uint32_in_expected_order(x) (x)
#define uint64_in_expected_order(x) (x)
#endif

#define swap(a, b)            \
{                             \
  *a ^= *b;                   \
  *b ^= *a;                   \
  *a ^= *b;                   \
}                             \

#define PERMUTE3(a, b, c) do { swap(&a, &b); swap(&a, &c); } while (0)

static inline uint32_t fetch32(const char *p)
{
  uint32_t result;
  memcpy(&result, p, sizeof(result));
  return uint32_in_expected_order(result);
}

static inline uint64_t fetch64(const char *p)
{
  uint64_t result;
  memcpy(&result, p, sizeof(result));
  return uint64_in_expected_order(result);
}

#if defined(__clang__)
#define LROTATE32(n, s) (__builtin_rotateleft32(n, s))
#define RROTATE32(n, s) (__builtin_rotateright32(n, s))

#define LROTATE64(n, s) (__builtin_rotateleft64(n, s))
#define RROTATE64(n, s) (__builtin_rotateright64(n, s))
#else
#define LROTATE32(n, s) (((n) << (s)) | ((n) >> (32 - (s))))
#define RROTATE32(n, s) (((n) >> (s)) | ((n) << (32 - (s))))

#define LROTATE64(n, s) (((n) << (s)) | ((n) >> (64 - (s))))
#define RROTATE64(n, s) (((n) >> (s)) | ((n) << (64 - (s))))
#endif

/* to debug mix function via test_mix.c */
#ifdef __AH1_DEBUG__
#define
inline uint32_t mix(uint32_t num)
#else
static inline uint32_t mix32(uint32_t num)
#endif /* __AH1_DEBUG__ */
{
#define psi     0x28330d1b
#define phi     0x483b86d5
#define L_CONST 0x3af1de9b
#define R_CONST 0x13a7ce59

  /* Inspired by Murmur3 */
  num *= phi;
  num ^= LROTATE32(num, 11);
  num ^= L_CONST * (num >> 13) + psi;
  num *= RROTATE32(num,  7);
  num ^= (R_CONST * num) << 17;

#undef psi
#undef phi
#undef L_CONST
#undef R_CONST

  return num;
}

/* to debug mix function via test_mix.c */
#ifdef __AH1_DEBUG__
#define
inline uint32_t mix(uint32_t num)
#else
static inline uint64_t mix64(uint64_t num)
#endif /* __AH1_DEBUG__ */
{

#define psi     0x28330d1b
#define phi     0x483b86d5
#define L_CONST 0x3af1de9b
#define R_CONST 0x13a7ce59

  /* Inspired by Murmur3 */
  num *= phi;
  num ^= LROTATE32(num, 31);
  num ^= L_CONST * (num >> 13) + psi;
  num *= RROTATE32(num,  11);
  num ^= (R_CONST * num) << 61;

  return num;

#undef psi
#undef phi
#undef L_CONST
#undef R_CONST

}

void AH1Hash(const char *restrict bytes, size_t size, uint32_t hash[4])
{

#define c1 0x21914047
#define c2 0x1b873593
#define c3 0x0f7527d9
#define c4 0x0356ac85

  uint32_t w = 0x5a44f074;
  uint32_t x = 0x35e820f6;
  uint32_t y = 0x674f1845;
  uint32_t z = 0x7fb5de7f;

  if (size < 16) {
    char temp[16] = { 0 };

    /* create a workaround for this redundant memcpy */
    memcpy(temp, bytes, size);

    w ^= RROTATE32(fetch32(temp), 7) * c2 +  size;
    x *= LROTATE32(fetch32(temp + 4), 19) + w;
    y += RROTATE32(fetch32(temp + 8), 3) * c3 + x * y;
    z ^= RROTATE32(fetch32(temp + 12), 11) * y + c4 * w;
    PERMUTE3(w, y, z);

  } else {

    /* hash the last 16 bytes first */
    const char *terminal = bytes + size - 16;
    w ^= RROTATE32(fetch32(terminal), 7) * c2 + size;
    x *= LROTATE32(fetch32(terminal+4), 19) + w;
    y += RROTATE32(fetch32(terminal+8), 3) * c3 + x * y;
    z ^= RROTATE32(fetch32(terminal+12), 11) * y + c4 * w;
    PERMUTE3(w, y, z);
    
    /* make size the closest minimum multiple of 16
       for instance, bring 19 to 16, 47 to 32, 16 to 0. */
    size = (size - 1) & ~(size_t) 15;
    while (size > 0) {
      w ^= RROTATE32(fetch32(bytes), 7) * c2 + size;
      x *= LROTATE32(fetch32(bytes+4), 19) + w;
      y += RROTATE32(fetch32(bytes+8), 3) * c3 + x * y;
      z ^= RROTATE32(fetch32(bytes+12), 11) * y + c4 * w;
      PERMUTE3(w, y, z);

      bytes += 16;
      size -= 16;
    }
  }
  
  w += x; w -= y; w ^= z;
  x -= w;
  y ^= w;
  z += w;
  
  hash[0] = mix32(w);
  hash[1] = mix32(z);
  hash[2] = mix32(y);
  hash[3] = mix32(x);

}

void AH2Hash(const char *restrict bytes, size_t size, uint64_t hash[4])
{

#define d1 0x00bd8d962f0b
#define d2 0xca364cc797b1

  /* simulate two 64-bit registers with four 32-bit registers to use
   * with AH1Hash.*/
  uint32_t w1 = 0x21914047;
  uint32_t w2 = 0x21914047;
  uint32_t x1 = 0x1b873593;
  uint32_t x2 = 0x1b873593;

  uint64_t y = 0x0f7527d9;
  uint64_t z = 0x0356ac85;

  if (size < 32) {
    char temp[32] = { 0 };

    /* create a workaround for this redundant memcpy */
    memcpy(temp, bytes, size);

    /* operate on 32-bit registers, simulating AH1Hash behavior */
    w1 ^= RROTATE32(fetch32(temp), 7) * c2 +  size;
    w2 *= LROTATE32(fetch32(temp+4), 19) + w1;
    x1 += RROTATE32(fetch32(temp+8), 3) * c3 + w2 * x1;
    x2 ^= RROTATE32(fetch32(temp+12), 11) * y + c4 * w1;

    y ^= RROTATE64(fetch64(temp+16), 61) * d1 + w1 * x1;
    z *= RROTATE64(fetch64(temp+24), 13) * d2 + w2 * x2;
    swap(&y, &z);

  } else {
    /* hash the last 16 bytes first */
    const char *terminal = bytes + size - 32;
    /* operate on 32-bit registers, simulating AH1Hash behavior */
    w1 ^= RROTATE32(fetch32(terminal), 7) * c2 +  size;
    w2 *= LROTATE32(fetch32(terminal+4), 19) + w1;
    x1 += RROTATE32(fetch32(terminal+8), 3) * c3 + w2 * x1;
    x2 ^= RROTATE32(fetch32(terminal+12), 11) * y + c4 * w1;

    y ^= RROTATE64(fetch64(terminal+16), 61) * d1 + w1 * x1;
    z *= RROTATE64(fetch64(terminal+24), 13) * d2 + w2 * x2;
    swap(&y, &z);

    size = (size - 1) & ~(size_t) 31;
    while (size > 0) {
      /* operate on 32-bit registers, simulating AH1Hash behavior */
      w1 ^= RROTATE32(fetch32(bytes), 7) * c2 +  size;
      w2 *= LROTATE32(fetch32(bytes+4), 19) + w1;
      x1 += RROTATE32(fetch32(bytes+8), 3) * c3 + w2 * x1;
      x2 ^= RROTATE32(fetch32(bytes+12), 11) * y + c4 * w1;

      y ^= RROTATE64(fetch64(bytes+16), 61) * d1 + w1 * x1;
      z *= RROTATE64(fetch64(bytes+24), 13) * d2 + w2 * x2;
      swap(&y, &z);

      bytes += 32;
      size -= 32;
    }
  }

  /* obtain 64-bit registers from the 32-bit pairs */
  uint64_t w = ((uint64_t) w1 << 32) | (uint64_t ) w2;
  uint64_t x = ((uint64_t) x1 << 32) | (uint64_t ) x2;

  w += x; w -= y; w ^= z;
  x -= w;
  y ^= w;
  z += w;

  hash[0] = mix64(w);
  hash[1] = mix64(z);
  hash[2] = mix64(y);
  hash[3] = mix64(x);

}

#undef uint32_in_expected_order
#undef bswap_32
#undef swap

#undef LROTATE32
#undef RROTATE32

#undef LROTATE64
#undef RROTATE64

#undef c1
#undef c2
#undef c3
#undef c4

#undef d1
#undef d2
