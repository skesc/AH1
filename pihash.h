/* -- hash.h
 * Header file for functions related AH1 hashing algorithm.
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

#ifndef __AH1_H__
#define __AH1_H__

#include <stdint.h>
#include <stdlib.h>

/*
 * Returns a 32-bit hash for `bytes' of given `size'.
 *
 * @param bytes block of data to hash.
 * @param size  length of data to read to hash.
 *
 * @return a 32-bit hash value.
 */
uint32_t piHash32(const char *bytes, size_t size);

/*
 * Returns a 64-bit hash for `bytes' of given `size'.
 *
 * @param bytes block of data to hash.
 * @param size  length of data to read to hash.
 *
 * @return a 64-bit hash value.
 */
uint64_t piHash64(const char *bytes, size_t size);

/*
 * A 128-bit non-cryptographic hash function for use in hash tables
 * and calculating message digests.
 *
 * @param bytes the bytes to read for hashing
 * @param size  number of bytes to read
 * @param hash  an array of minimum size four, set to the hash value.
 */
void piHash128(const char *bytes, size_t size, uint32_t hash[4]);

/*
 * A 256-bit variant of piHash128.
 *
 * @param bytes the block of data to hash.
 * @param size  length of data to read for hashing.
 * @param hash  an array of minimum size four, set to the hash value.
 */
void piHash256(const char *bytes, size_t size, uint64_t hash[4]);

#endif /* __AH1_H__ */

