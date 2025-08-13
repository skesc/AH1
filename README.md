[![collision test](https://github.com/skesc/ah1/actions/workflows/test.yaml/badge.svg)](https://github.com/skesc/ah1/actions/workflows/test.yaml)

AH1 is a family of non-cryptographic hash functions. It includes a
128-bit and a 256-bit hashing algorithm. The 256-bit variant was a later
addition. The original version only had a 128-bit verision. Have been tested against
dictionaries as large as one million words with zero detected
collisions. It has an above average Hamming score and even beats
Murmur3 hashing algorithm in some edge cases. This makes it a decent, if
not good, hashing algorithm to be used in hash tables, validate data
integrity and generate file digests.

Despite speed not being a primary motivation, the hash function has kept
up pretty good with speed too. It is still slower than many standard
hashing algorithms like CityHash, but more than usable for
general-purpose use cases and small strings.

**Installation**

```bash
sudo make
make repl   # build repl tool
make tests  # run tests
```

To use the function, include `AH1.h` in your source file. It's most
probably in the `/usr/include` directory. Otherwise, to see example
usage, see [`repl.c`](tests/repl.c) and [`digest.c`](tests/digest.c). 

**Copyright**

The MIT License (MIT)

Copyright (c) 2025 Abhigyan <nourr@duck.com>
