/* -- test_mix.c
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

extern uint32_t mix(uint32_t num);

float test_hamming(uint32_t a, uint32_t b)
{
  int score = 0;
  int bits = 8 * sizeof(uint32_t);

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
  int good = 0;
  float score = 0;
  srand(time(NULL));
  
  for (int i = 0; i < RUNS; ++i) {
    int r = rand();
    float h = test_hamming(mix(r), mix(r+1));
    if (fabs(h - 0.5) < 0.1) good++;
    printf("hamming score: %f\n", h);
    score+=h;
  }

  printf("AVERAGE HAMMING SCORE: %f\n", score / RUNS);
  printf("GOOD SCORES: %d\n", good);

  return 0;
}
