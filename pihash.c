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

#include "pihash.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

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

#define PIHASH_PERMUTE32(a, b, c) do { \
    uint32_t tmp = a; \
    a = b; \
    b = c; \
    c = tmp; \
} while (0)

#define PIHASH_PERMUTE64(a, b, c) do { \
    uint64_t tmp = a; \
    a = b; \
    b = c; \
    c = tmp; \
} while (0)

static inline uint32_t fetch8(const char *p)
{
  char result;
  memcpy(&result, p, sizeof(result));
  return (uint32_t) result;
}

static inline uint64_t fetch16(const char *p)
{
  uint16_t result;
  memcpy(&result, p, sizeof(result));
  return uint64_in_expected_order((uint64_t) result);
}

static inline uint32_t fetch32(const char *p)
{
  uint32_t result;
  memcpy(&result, p, sizeof(result));
  return uint32_in_expected_order(result);
}

// static inline uint64_t fetch64(const char *p)
// {
//   uint64_t result;
//   memcpy(&result, p, sizeof(result));
//   return uint64_in_expected_order(result);
// }

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
uint32_t mix32(uint32_t num)
#else
static inline uint32_t mix32(uint32_t num)
#endif /* __AH1_DEBUG__ */
{

  /* Inspired by Murmur3, fixed for 64-bit operations */
  num *= 0x2ca803f9UL;
  num ^= LROTATE32(num, 16);
  num ^= 0x3583c01fUL * (num >> 11) + 0x34504db3;
  num *= RROTATE32(num,  4);
  num ^= (0x243e4223 * num) << 7;

  return num;
}

/* to debug mix function via test_mix.c */
#ifdef __AH1_DEBUG__
uint64_t mix64(uint64_t num)
#else
static inline uint64_t mix64(uint64_t num)
#endif /* __AH1_DEBUG__ */
{

#define c_psi64     0x28330d1b28330d1bULL
#define c_phi64     0x483b86d5483b86d5ULL
#define c_L_CONST64 0x3af1de9b3af1de9bULL
#define c_R_CONST64 0x13a7ce5913a7ce59ULL

  /* Inspired by Murmur3, fixed for 64-bit operations */
  num *= c_phi64;
  num ^= LROTATE64(num, 31);
  num ^= c_L_CONST64 * (num >> 27) + c_psi64;
  num *= RROTATE64(num,  33);
  num ^= (c_R_CONST64 * num) << 37;

#undef c_psi64
#undef c_phi64
#undef c_L_CONST64
#undef c_R_CONST64
  return num;
}

/*
 * Returns a 32-bit hash for `bytes' of given `size'.
 *
 * @param bytes block of data to hash.
 * @param size  length of data to read to hash.
 *
 * @return a 32-bit hash value.
 */
uint32_t piHash32(const char *restrict bytes, size_t size)
{
  /* seed values */
  uint32_t w = 0x297bfef9UL;
  uint32_t x = 0x0c240623UL;
  uint32_t y = 0x39527119UL;
  uint32_t z = 0x09bc0863UL;

  /*
   * There are four registers, each of which combine to make the final
   * 32-bit (4 byte) hash. So, each register processes a 1-byte value.
   * Hence, we read in 4-byte blocks.
   */
  const size_t chunks = (size - 1) & ~(size_t) 3;
  for (size_t i = 0; i < chunks; i+=4) {
    w ^= (RROTATE32(fetch8(bytes),   11) + i * w) * 0x21914047UL;
    x += (LROTATE32(fetch8(bytes+1), 17) + w * x) * 0x0356ac85UL;
    y += (RROTATE32(fetch8(bytes+2),  3) + x * y) * 0x0f7527d9UL;
    z ^= (RROTATE32(fetch8(bytes+3), 23) + y * z) * 0x1b873593UL;
    PIHASH_PERMUTE32(x, y, z);
  }

  /* hash the terminating 4 bytes */
  char tail[4] = { 0 };

  if (size < 4)
    memcpy(tail, bytes, size);
  else
    memcpy(tail, bytes + size - (sizeof tail), (sizeof tail));

  w ^= (RROTATE32(fetch8(tail),   11) + size * w) * 0x21914047UL;
  x += (LROTATE32(fetch8(tail+1), 17) +    w * x) * 0x0356ac85UL;
  y += (RROTATE32(fetch8(tail+2),  3) +    x * y) * 0x0f7527d9UL;
  z ^= (RROTATE32(fetch8(tail+3), 23) +    y * z) * 0x1b873593UL;
  PIHASH_PERMUTE32(x, y, z);

  w += x; w -= y; w ^= z;
  x -= w;
  y ^= w;
  
  z += w;
  w += mix32(w);
  x += mix32(x) + w;
  y += mix32(y) + x;
  z += mix32(z) + y;

  return w ^ x ^ y ^ z;

}

/*
 * Returns a 64-bit number as hash.
 *
 * @param bytes the bytes to read for hashing
 * @param size  number of bytes to read
 * @param hash  an array of minimum size four, set to the hash value.
 */
uint64_t piHash64(const char *restrict bytes, size_t size)
{
  /* seed values */
  uint64_t w = 0x24e76fbdbULL;
  uint64_t x = 0x251f30fb9ULL;
  uint64_t y = 0x1218121e1ULL;
  uint64_t z = 0x19403b6e1ULL;

  /*
   * There are four registers, each of which combine to make the final
   * 64-bit (8 byte) hash. So, each register processes a 2-byte value.
   * Hence, we read in 8-byte blocks.
   */
  const size_t chunks = (size - 1) & ~(size_t) 7;
  for (size_t i = 0; i < chunks; i+=8) {
    w ^= (RROTATE64(fetch16(bytes),   61) + i * w) * 0x21914047ULL;
    x += (LROTATE64(fetch16(bytes+2), 16) + w * x) * 0x0356ac85ULL;
    y += (RROTATE64(fetch16(bytes+4), 13) + x * y) * 0x0f7527d9ULL;
    z ^= (RROTATE64(fetch16(bytes+6), 19) + y * z) * 0x1b873593ULL;
    PIHASH_PERMUTE64(x, y, z);
  }

  /* hash the terminating 8 bytes */
  char tail[8] = { 0 };

  if (size < 8)
    memcpy(tail, bytes, size);
  else
    memcpy(tail, bytes + size - (sizeof tail), (sizeof tail));

  w ^= (RROTATE64(fetch16(tail),   61) + size * w) * 0x21914047ULL;
  x += (LROTATE64(fetch16(tail+2), 16) +    w * x) * 0x0356ac85ULL;
  y += (RROTATE64(fetch16(tail+4), 13) +    x * y) * 0x0f7527d9ULL;
  z ^= (RROTATE64(fetch16(tail+6), 19) +    y * z) * 0x1b873593ULL;
  PIHASH_PERMUTE64(x, y, z);

  w += x; w -= y; w ^= z;
  x -= w;
  y ^= w;
  z += w;
  
  w += mix64(w);
  x += mix64(x) + w;
  y += mix64(y) + x;
  z += mix64(z) + y;

  return w ^ x ^ y ^ z;

}

void piHash128(const char *restrict bytes, size_t size, uint32_t hash[4])
{
  /* seed values */
  uint32_t w = 0x5a44f074UL;
  uint32_t x = 0x35e820f6UL;
  uint32_t y = 0x674f1845UL;
  uint32_t z = 0x7fb5de7fUL;

  /*
   * There are four registers, each of which combine to make the final
   * 128-bit (16 byte) hash. So, each register processes a 4-byte value.
   * Hence, we read in 16-byte blocks.
   */
  const size_t chunks = (size - 1) & ~(size_t) 15;
  for (size_t i = 0; i < chunks; i+=16) {

    w ^= RROTATE32(fetch32(bytes),     7) * 0x21914047UL + (i ^ w);
    x += LROTATE32(fetch32(bytes+4),  19) * 0x1b873593UL + (w ^ x);
    y += RROTATE32(fetch32(bytes+8),  13) * 0x0f7527d9UL + (x ^ y);
    z ^= RROTATE32(fetch32(bytes+12), 11) * 0x0356ac85UL + (y ^ z);

    PIHASH_PERMUTE32(x, y, z);
  }

  /* hash the terminating 16 bytes */
  char tail[16] = { 0 };

  if (size < 16)
    memcpy(tail, bytes, size);
  else
    memcpy(tail, bytes + size - (sizeof tail), (sizeof tail));

  w ^= RROTATE32(fetch32(tail),     7) * 0x0356ac85UL;
  x += LROTATE32(fetch32(tail+4),  19) * 0x0f7527d9UL;
  y += RROTATE32(fetch32(tail+8),  13) * 0x1b873593UL;
  z ^= RROTATE32(fetch32(tail+12), 11) * 0x21914047UL;

  PIHASH_PERMUTE32(x, y, z);

  w += x; w -= y; w ^= z;
  x -= w;
  y ^= w;
  z += w;

  w = mix32(w) + size;
  x = mix32(x) +    w;
  y = mix32(y) +    x;
  z = mix32(z) +    y;

  hash[0] = w;
  hash[1] = x;
  hash[2] = y;
  hash[3] = z;
}
