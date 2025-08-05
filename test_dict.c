/* -- test_hash.c
 * Utility program to test AH1 hash function for hash collisions
 * against word lists and dictionaries to measure performance for
 * common English word inputs.
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

#include "hash.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* define max reading limits */
#define MAX_LINE_LENGTH 1024

typedef struct TestWord
{
  char *word;
  uint32_t hash[4];
} TestWord;

void ah1_print(uint32_t hash[4])
{
  for (uint8_t i = 0; i < 4; ++i) {
    printf("%08x", hash[i]);
  }
  printf("\n");
}

char *get_word(FILE *file)
{
  char buff[MAX_LINE_LENGTH];
  if (!fgets(buff, MAX_LINE_LENGTH, file)) {
    perror("Unable to read word from file.");
    exit(-1);
  }

  char *word = malloc(strlen(buff) + 1);
  if (!word) {
    perror("Unable to allocate memory to store word.");
    exit(-1);
  }

  strcpy(word, buff);
  return word;
}

unsigned int line_count(FILE *file)
{
  unsigned int lines = 0;
  char c = getc(file);
  while (c != EOF) {
    if (c == '\n') lines++;
    c = getc(file);
  }

  rewind(file);
  return lines;
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: test_dict [FILE NAME]\n");
    return -1;
  }

  FILE *wordlist = fopen(argv[1], "r");
  if (!wordlist) {
    perror("Unable to open given file.");
    return -1;
  }
  
  unsigned int lines = line_count(wordlist);
  TestWord *tests = malloc(lines * sizeof(TestWord));
  if (!tests) {
    perror("Could not allocate memory for all test cases.");
    return -1;
  }

  TestWord *test  = tests;
  unsigned int collisions = 0;
  for (unsigned int i = 0; i < lines; ++i) {
    test->word = get_word(wordlist);
    ah1(test->word, strlen(test->word), test->hash);
    uint32_t *h1 = test->hash;
    
#ifdef DEBUG
    printf("TEST CASE #%d\n", i + 1);
    printf("  SUBJECT: %s", test->word);
    printf("  ");
    ah1_print(test->hash);
#endif

    TestWord *match = tests;
    for (int j = 0; j < i; ++j) {
      ah1(match->word, strlen(match->word) - 1, match->hash);

      uint32_t *h2 = match->hash;

#ifdef DEBUG
      printf("  TARGET:  %s", match->word);
      printf("  ");
      ah1_print(match->hash);
#endif

      bool collides = ((h1[0] == h2[0]) && (h1[1] == h2[1]) && (h1[2] == h2[2]) && (h1[3] == h2[3]));
      if (collides) {
        printf("MATCH FOUND\n");
        printf("  %s", test->word);
        ah1_print(test->hash);
        printf("  %s", match->word);
        ah1_print(match->hash);
        collisions++;
      }
      match++;
    }
    
    printf("[%u/%u] Check finished for: %s", i+1, lines, test->word);
    test++;
  }
  
  fclose(wordlist);
  printf("Total collisions: %u/%u\n", collisions, lines);
  return 0;
}

