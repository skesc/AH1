[![collision test](https://github.com/skesc/ah1/actions/workflows/test.yaml/badge.svg)](https://github.com/skesc/ah1/actions/workflows/test.yaml)

AH1 is a 128-bit non-cryptographic hashing algorithm. Its mixing
function has a good Hamming distance score of 16 bits for 32-bit inputs,
and performs better than Murmur3 mix function in some cases. It has been
tested for hash collisions against wordlists containing as many as one
million words with zero collisions detected. This makes it a suitable
hash function for normal use cases (such as in calculating digests and
in hash tables).

**Installation**
```bash
sudo make
make repl   # build repl tool
make tests  # run tests
```

To use the function, include `AH1.h` in your source file. It's
installed in your user include directory. For example usage, see
`test_dict.c` and `repl.c`.

