/* bench <files...>: best-of-N compress/decompress throughput and size. */
#define QSPL_IMPLEMENTATION
#include "../include/qlz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static double now(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec + t.tv_nsec * 1e-9; }
int main(int c, char **v) {
  size_t ti = 0, to = 0;
  printf("%-28s %12s %12s %8s %10s %10s\n", "file", "input", "output", "reduct", "comp MB/s", "dec MB/s");
  for (int a = 1; a < c; a++) {
    FILE *f = fopen(v[a], "rb"); if (!f) { perror(v[a]); continue; }
    fseek(f, 0, SEEK_END); size_t n = (size_t)ftell(f); fseek(f, 0, SEEK_SET);
    unsigned char *in = malloc(n + 1), *p = malloc(qspl_bound(n)), *o = malloc(n + 1);
    if (fread(in, 1, n, f) != n) { fclose(f); continue; } fclose(f);
    int reps = n < (1 << 20) ? 200 : 10; double bc = 1e9, bd = 1e9; size_t k = 0;
    for (int r = 0; r < reps; r++) { double t = now(); k = qspl_compress(in, n, p); t = now() - t; if (t < bc) bc = t; }
    for (int r = 0; r < reps; r++) { double t = now(); qspl_decompress(p, k, o, n); t = now() - t; if (t < bd) bd = t; }
    if (memcmp(in, o, n)) { printf("%s: ROUND-TRIP FAILED\n", v[a]); return 1; }
    printf("%-28.28s %12zu %12zu %7.2f%% %10.0f %10.0f\n", v[a], n, k, 100.0 * (1 - (double)k / n), n / bc / 1e6, n / bd / 1e6);
    ti += n; to += k; free(in); free(p); free(o);
  }
  if (ti) printf("TOTAL %zu -> %zu (%.2f%% reduction)\n", ti, to, 100.0 * (1 - (double)to / ti));
  return 0;
}
