/* -- mix.c
 * Utility program to test Hamming score (amount of bit flips in output
 * of mix function for change in one bit of the input) of mix function.
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

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

extern uint32_t mix32(uint32_t num);
extern uint64_t mix64(uint64_t num);

float test_hamming32(uint32_t a, uint32_t b)
{
  int score = 0;
  int bits = 8 * sizeof(uint32_t);

  for (int i = bits; i > 0; --i) {
    if ((a & 1) ^ (b & 1)) score+=1;
    a = a >> 1; b = b >> 1;
  }

  return (float) score / (float) bits;
}

float test_hamming64(uint64_t a, uint64_t b)
{
  int score = 0;
  int bits = 8 * sizeof(uint64_t);

  for (int i = bits; i > 0; --i) {
    if ((a & 1) ^ (b & 1)) score+=1;
    a = a >> 1; b = b >> 1;
  }

  return (float) score / (float) bits;
}

#ifndef RUNS
#define RUNS 10000
#endif

int main(void)
{
  float delta;
  float score32 = 0;
  float score64 = 0;
  srand(time(NULL));
  
  for (int i = 0; i < RUNS; ++i) {
    uint32_t t32 = rand();
    uint32_t t64 = ((uint64_t) rand() << 32) * 31;

    float h32 = test_hamming32(mix32(t32), mix32(t32+1));
    float h64 = test_hamming64(mix64(t64), mix64(t64+1));
    
    score32+=h32;
    score64+=h64;
  }

  score32 /= RUNS; score64 /= RUNS;

  delta = fabs(score32 - 0.5);
  printf("AVERAGE HAMMING 32 SCORE: %f (±%f)\n", score32, delta);
  assert((delta < 0.075) && "HAMMING TEST FOR 32-BIT MIX FAILED.");

  delta = fabs(score64 - 0.5);
  printf("AVERAGE HAMMING 64 SCORE: %f (±%f)\n", score64, delta);
  assert((delta < 0.075) && "HAMMING TEST FOR 64-BIT MIX FAILED.");

  return 0;
}

