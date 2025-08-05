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

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_32(x) BSWAP_32(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_32(x) bswap32(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_32(x) swap32(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_32(x) bswap32(x)
#endif

#else

#include <byteswap.h>

#endif

#ifdef WORDS_BIGENDIAN
#define uint32_in_expected_order(x) (bswap_32(x))
#else
#define uint32_in_expected_order(x) (x)
#define uint64_in_expected_order(x) (x)
#endif

#define PERMUTE3(a, b, c) do { swap(&a, &b); swap(&a, &c); } while (0)

static uint32_t UNALIGNED_LOAD32(const char *p) {
  uint32_t result;
  memcpy(&result, p, sizeof(result));
  return result;
}

static inline void swap(uint32_t *a, uint32_t *b)
{
  *a ^= *b;
  *b ^= *a;
  *a ^= *b;
}

static inline uint32_t Fetch32(const char *p)
{
  return uint32_in_expected_order(UNALIGNED_LOAD32(p));
}

#if defined(__clang__)
#define LROTATE32(n, s) (__builtin_rotateleft32(n, s))
#define RROTATE32(n, s) (__builtin_rotateright32(n, s))
#else
#define LROTATE32(n, s) (((n) << (s)) | ((n) >> (32 - (s))))
#define RROTATE32(n, s) (((n) >> (s)) | ((n) << (32 - (s))))
#endif

static inline uint32_t mix(uint32_t num)
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

void AH1Hash(const char *restrict bytes, size_t size, uint32_t hash[4])
{
  uint32_t w = 0x5a44f074;
  uint32_t x = 0x35e820f6;
  uint32_t y = 0x674f1845;
  uint32_t z = 0x7fb5de7f;

  const uint32_t c1 = 0x21914047;
  const uint32_t c2 = 0x1b873593;
  const uint32_t c3 = 259336153;
  const uint32_t c4 = 56011909;

  if (size < 16) {
    uint8_t temp[16] = {0};
    memcpy(temp, bytes, size);

    w ^= RROTATE32(Fetch32(temp), 7) * c2 +  size;
    x *= LROTATE32(Fetch32(temp + 4), 19) + (w);
    y += RROTATE32(Fetch32(temp + 8), 3) * c3 + x * y;
    z ^= RROTATE32(Fetch32(temp + 12), 11) * y + c4 * w;
    PERMUTE3(w, y, z);

  } else {
    const char *terminal = bytes + size - 16;
    w ^= RROTATE32(Fetch32(terminal), 7) * c2 + size;
    x *= LROTATE32(Fetch32(terminal+4), 19) + (w);
    y += RROTATE32(Fetch32(terminal+8), 3) * c3 + x * y;
    z ^= RROTATE32(Fetch32(terminal+12), 11) * y + c4 * w;
    PERMUTE3(w, y, z);
    
    size = (size - 1) & ~(size_t) 15;
    while (size > 0) {
      w ^= RROTATE32(Fetch32(bytes), 7) * c2 + size;
      x *= LROTATE32(Fetch32(bytes+4), 19) + w;
      y += RROTATE32(Fetch32(bytes+8), 3) * c3 + x * y;
      z ^= RROTATE32(Fetch32(bytes+12), 11) * y + c4 * w;
      PERMUTE3(w, y, z);

      bytes += 16;
      size -= 16;
    }
  }
  
  w += x; w -= y; w ^= z;
  x -= w;
  y ^= w;
  z += w;
  
  hash[0] = mix(w);
  hash[1] = mix(z);
  hash[2] = mix(y);
  hash[3] = mix(x);
}

#undef uint32_in_expected_order
#undef bswap_32
#undef LROTATE32
#undef RROTATE32

