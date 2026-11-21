#define QSPL_IMPLEMENTATION
#include "../include/qlz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned rs = 12345; static unsigned rnd(void) { rs = rs * 1103515245u + 12345u; return rs >> 16; }
static int check(const unsigned char *d, size_t n, const char *name) {
  unsigned char *p = malloc(qspl_bound(n)), *o = malloc(n + 1);
  size_t k = qspl_compress(d, n, p);
  int ok = k <= qspl_bound(n) && qspl_size(p, k) == (long long)n && qspl_decompress(p, k, o, n) == (long long)n && !memcmp(d, o, n);
  /* corrupted streams must be rejected or stay in bounds, never crash */
  for (size_t i = 6; i < k; i += 1 + k / 50) { p[i] ^= 0x5a; (void)qspl_decompress(p, k, o, n); p[i] ^= 0x5a; }
  if (k > 6) ok &= qspl_decompress(p, k - 1, o, n) < 0;
  printf("%-14s %8zu -> %8zu %s\n", name, n, k, ok ? "ok" : "FAIL");
  free(p); free(o); return ok;
}
int main(void) {
  int ok = 1; size_t N = 1 << 20; unsigned char *b = malloc(N);
  ok &= check(b, 0, "empty");
  b[0] = 'x'; ok &= check(b, 1, "one byte");
  memset(b, 'a', N); ok &= check(b, N, "run");
  for (size_t i = 0; i < N; i++) b[i] = (unsigned char)rnd(); ok &= check(b, N, "random");
  for (size_t i = 0; i < N; i++) b[i] = "the quick brown fox jumps "[rnd() % 26]; ok &= check(b, N, "text-ish");
  for (size_t i = 0; i < N; i++) b[i] = (unsigned char)(i % 251 + (i / 4096)); ok &= check(b, N, "pattern");
  free(b); puts(ok ? "ALL PASSED" : "FAILURES"); return ok ? 0 : 1;
}
