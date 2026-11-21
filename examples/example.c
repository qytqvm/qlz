#define QSPL_IMPLEMENTATION
#include "../include/qlz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
  const unsigned char in[] = "QLz exact round-trip example example example example";
  size_t n = sizeof(in) - 1;
  unsigned char *pk = malloc(qspl_bound(n)), *out = malloc(n);
  if (!pk || !out) return 1;
  size_t k = qspl_compress(in, n, pk);
  int ok = qspl_size(pk, k) == (long long)n && qspl_decompress(pk, k, out, n) == (long long)n && !memcmp(in, out, n);
  printf("%zu -> %zu bytes: %s\n", n, k, ok ? "restored exactly" : "FAILED");
  free(pk); free(out); return ok ? 0 : 1;
}
