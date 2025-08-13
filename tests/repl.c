/* -- repl.c
 * A REPL to compute hashes of strings.
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

#include "../pihash.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define BUFF_SIZE 2048

void ah1_print(uint32_t hash[4])
{
  printf("ah128: ");
  for (int i = 0; i < 4; ++i) {
    printf("%" PRIx32, hash[i]);
  }
  printf("\n");
}

int main(int argc, char **argv)
{
  uint32_t hash128[4];
  char buff[BUFF_SIZE] = { 0 };

  printf(">> ");
  while (fgets(buff, BUFF_SIZE, stdin)) {
    size_t buff_len = strlen(buff) - 1;
    printf("314hash32:  %" PRIx32 "\n", piHash32(buff, buff_len));
    printf("314hash64:  %" PRIx64 "\n", piHash64(buff, buff_len));
    piHash128(buff, buff_len, hash128);
    ah1_print(hash128);
    printf(">> ");
  }

  return EXIT_SUCCESS;
}

