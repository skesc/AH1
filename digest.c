/* -- digest.c
 * Generates AH1 digest of files.
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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("input file required.");
    return EXIT_FAILURE;
  }

  uint32_t hash[4];
  
  int fd = open(argv[1], O_RDONLY);
  if(!fd) {
    perror("file i/o: cannot open file.");
    return -1;
  }

  struct stat file_stats;
  if (fstat(fd, &file_stats) == -1) {
    perror("file i/o: cannot read file size.");
    close(fd);
    return EXIT_FAILURE;
  }

  size_t file_size = file_stats.st_size;
  if (!file_size) {
    perror("file i/o: file is empty.");
    close(fd);
    return EXIT_SUCCESS;
  }

  char *restrict map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (map == MAP_FAILED) {
    perror("file i/o: unable to map file.");
    close(fd);
    return EXIT_FAILURE;
  }
  
  ah1(map, file_size, hash);

  ah1_print(hash);

  if (munmap(map, file_size) == -1) {
    perror("file i/o: cannot unmap file from memory.");
    return EXIT_FAILURE;
  }

  close(fd);
  
  return EXIT_SUCCESS;
}

