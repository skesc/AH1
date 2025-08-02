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

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>

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

void ah1(void *restrict bytes, size_t size, uint32_t hash[4])
{
  hash[0] = 0x5a44f074;
  hash[1] = 0x35e820f6;
  hash[2] = 0x674f1845;
  hash[3] = 0x7fb5de7f;

  const uint8_t *data = (const uint8_t*)bytes;
  const int nblocks = size / 4;

  const uint32_t c1 = 0x21914047;
  const uint32_t c2 = 0x1b873593;
  const uint32_t c3 = 259336153;
  const uint32_t c4 = 56011909;

  // --- Process 4-byte chunks ---
  const uint32_t *blocks = (const uint32_t *)(data);
  for (int i = 0; i < nblocks; i++) {
    uint32_t in = blocks[i];
   
    in *= c1;
    hash[i & 3] ^= c2 * RROTATE32(in, 7);
    hash[i & 3] *= LROTATE32(hash[i & 3], 19);
    hash[i & 3] += c3 * RROTATE32(hash[(i + 1) & 3], 3);
    hash[i & 3] += hash[i & 3] + c1 * hash[(i + 2) & 3];
  }

  const uint8_t *tail = (const uint8_t*)(data + nblocks * 4);
  uint32_t in = 0;

  switch(size & 3)
  {
    case 3: in ^= (uint32_t)tail[2] << 16;
    case 2: in ^= (uint32_t)tail[1] << 8;
    case 1: in ^= (uint32_t)tail[0];
            in *= c1; 
            in = LROTATE32(in, 15) + c2; 
            hash[nblocks % 4] ^= in;
  };

  hash[0] ^= size;
  hash[1] += size;
  hash[2] -= size;
  hash[3] += size;

  hash[0] += hash[1]; hash[0] -= hash[2]; hash[0] ^= hash[3];
  hash[1] -= hash[0];
  hash[2] ^= hash[0];
  hash[3] += hash[0];

  hash[0] = mix(hash[3]);
  hash[1] = mix(hash[1]);
  hash[2] = mix(hash[0]);
  hash[3] = mix(hash[2]);

#undef LROTATE32
#undef RROTATE32
}

/*
 * Prints the 128-bit hash in a hexadecimal format.
 */
void ah1_print(uint32_t hash[4])
{
  for (uint8_t i = 0; i < 4; ++i) {
    printf("%08x", hash[i]);
  }
  printf("\n");
}
