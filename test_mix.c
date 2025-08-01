#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define LOG_HEX(t) printf("%.8x\n", t)

typedef unsigned int uint;

uint murmur1(uint num)
{

#define L_CONST 0x3af1de9b
#define R_CONST 0x13a7ce59
  
  /* Copied from Murmur3 */
  num ^= num >> 16;
  num *= L_CONST;
  num ^= num >> 13;
  num *= R_CONST;
  num ^= num >> 16;

#undef L_CONST
#undef R_CONST

  return num;
}

float test_hamming(uint a, uint b)
{
  int score = 0;
  int bits = 8 * sizeof(uint);

  for (int i = bits; i > 0; --i) {
    if ((a & 1) ^ (b & 1)) score+=1;
    a = a >> 1; b = b >> 1;
  }

  return (float) score / (float) bits;
}

uint mix(uint num)
{

#if defined(__clang__)
#define LROTATE32(n, s) (__builtin_rotateleft32(n, s))
#define RROTATE32(n, s) (__builtin_rotateright32(n, s))
#else
#define LROTATE32(n, s) (((n << s) | (n >> (32 - s))) & UINT32_MAX)
#define RROTATE32(n, s) (((n >> s) | (n << (32 - s))) & UINT32_MAX)
#endif

#define psi     0x28330d1b
#define phi     0x483b86d5
#define L_CONST 0x3af1de9b
#define R_CONST 0x13a7ce59
  
  num *= phi;
  num ^= LROTATE32(num, 11);
  num ^= L_CONST * (num >> 13) + psi;
  num *= RROTATE32(num,  7);
  num ^= (R_CONST * num) << 17;

#undef psi
#undef phi
#undef L_CONST
#undef R_CONST

#undef LROTATE32
#undef RROTATE32
  
  return num;
}

#define RUNS 10000

int main(void)
{
  int good = 0, m_good = 0;
  float score = 0, m_score = 0;
  srand(time(NULL));
  
  for (int i = 0; i < RUNS; ++i) {
    int r = rand();
    float h = test_hamming(mix(r), mix(r+1));
    if (fabsf(h - 0.5) < 0.1) good+=1;
    score+=h;
    printf("hamming score: %f\n", h);
    h = test_hamming(murmur1(r), murmur1(r+1));
    if (fabsf(h - 0.5) < 0.1) m_good+=1;
    m_score+=h;
  }

  printf("AVERAGE HAMMING SCORE: %f\n", score / RUNS);
  printf("GOOD SCORES: %d\n", good);
  
  printf("AVERAGE HAMMING SCORE: %f\n", m_score / RUNS);
  printf("GOOD SCORES: %d\n", m_good);

  return 0;
}
