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
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
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
#define LROTATE32(n, s) (((n << s) | (n >> (32 - s))) & UINT32_MAX)
#define RROTATE32(n, s) (((n >> s) | (n << (32 - s))) & UINT32_MAX)
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

/*
 * The AH1 hash function. Intakes `size' bytes as `bytes' and a pointer
 * to the hash array. After a successful hash, it sets hash[4] to the
 * hash value of the passed data.
 */
void ah1(void *restrict bytes, size_t size, uint32_t hash[4])
{
  hash[0] = 0x5a44f074;
  hash[1] = 0x35e820f6;
  hash[2] = 0x674f1845;
  hash[3] = 0x7fb5de7f;

  uint32_t *blocks = bytes;
  size_t total_blocks = size / sizeof(uint32_t);
  size_t full_blocks  = total_blocks / 4;
  size_t last_blocks  = total_blocks % 4;
  
  uint32_t chain[4] = { 0x8bca9b, 0x2413c9, 0x49f6dc3, 0x97b1 };
  
  for (size_t iter = 0; iter < full_blocks; ++iter) {
    hash[0] &= mix(blocks[1] ^ chain[0]) + iter * blocks[3];
    hash[1] ^= mix(blocks[0] - (chain[1] ^ hash[0]));
    hash[2] += mix(blocks[3] ^ chain[2]) ^ hash[1];
    hash[3] -= mix((blocks[2] + chain[3]) * hash[2]) ^ hash[1];

    hash[0] += mix(hash[1]) * LROTATE32(hash[2], 7);
    hash[1] += mix(hash[2]) * RROTATE32(hash[3], 11);
    hash[2] ^= mix(hash[3]) * RROTATE32(hash[0], 9);
    hash[3] ^= mix(hash[0]) * LROTATE32(hash[1], 5);

    chain[0] = hash[3];
    chain[1] = hash[2];
    chain[2] = hash[0];
    chain[3] = hash[1];
    
    blocks += 4;
  }

  uint32_t tail[4] = { 0 };

  switch(size & 15) {
    case 15: tail[3] |= (uint32_t)blocks[14] << 16;
    case 14: tail[3] |= (uint32_t)blocks[13] << 8;
    case 13: tail[3] |= (uint32_t)blocks[12] << 0;
    case 12: tail[2] |= (uint32_t)blocks[11] << 24;
    case 11: tail[2] |= (uint32_t)blocks[10] << 16;
    case 10: tail[2] |= (uint32_t)blocks[9] << 8;
    case 9:  tail[2] |= (uint32_t)blocks[8] << 0;
    case 8:  tail[1] |= (uint32_t)blocks[7] << 24;
    case 7:  tail[1] |= (uint32_t)blocks[6] << 16;
    case 6:  tail[1] |= (uint32_t)blocks[5] << 8;
    case 5:  tail[1] |= (uint32_t)blocks[4] << 0;
    case 4:  tail[0] |= (uint32_t)blocks[3] << 24;
    case 3:  tail[0] |= (uint32_t)blocks[2] << 16;
    case 2:  tail[0] |= (uint32_t)blocks[1] << 8;
    case 1:  tail[0] |= (uint32_t)blocks[0] << 0;
  };

  hash[0] &= mix(tail[1] ^ chain[0]) + full_blocks * tail[3];
  hash[1] ^= mix(tail[0] - (chain[1] ^ hash[0]));
  hash[2] += mix(hash[1] * chain[2]) ^ tail[3];
  hash[3] -= mix((tail[2] + chain[3]) * hash[2]) ^ hash[1];

  hash[0] += mix(hash[1]) * LROTATE32(hash[2], 7);
  hash[1] += mix(hash[2]) * RROTATE32(hash[3], 11);
  hash[2] ^= mix(hash[3]) * RROTATE32(hash[0], 9);
  hash[3] ^= mix(hash[0]) * LROTATE32(hash[1], 5);
  
#undef LROTATE32
#undef RROTATE32

}

void ah1_print(uint32_t hash[4])
{
  for (uint8_t i = 0; i < 4; ++i) {
    printf("%08x", hash[i]);
  }
  
  printf("\n");
}
