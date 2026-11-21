/* qlz -c in out  |  qlz -d in out */
#define QSPL_IMPLEMENTATION
#include "../include/qlz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned char *slurp(const char *p, size_t *n) {
  FILE *f = fopen(p, "rb"); if (!f) return NULL; fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
  unsigned char *b = malloc(s ? (size_t)s : 1); if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
  fclose(f); *n = (size_t)s; return b;
}
int main(int c, char **v) {
  if (c != 4 || (strcmp(v[1], "-c") && strcmp(v[1], "-d"))) { fprintf(stderr, "usage: qlz -c|-d in out\n"); return 2; }
  size_t n; unsigned char *in = slurp(v[2], &n); if (!in) { perror(v[2]); return 1; }
  unsigned char *out; long long m;
  if (v[1][1] == 'c') { out = malloc(qspl_bound(n)); m = (long long)qspl_compress(in, n, out); }
  else { long long s = qspl_size(in, n); if (s < 0) { fprintf(stderr, "not a QLz stream\n"); return 1; }
    out = malloc(s ? (size_t)s : 1); m = qspl_decompress(in, n, out, (size_t)s); }
  if (m < 0) { fprintf(stderr, "corrupt stream\n"); return 1; }
  FILE *f = fopen(v[3], "wb"); if (!f || fwrite(out, 1, (size_t)m, f) != (size_t)m) { perror(v[3]); return 1; }
  fclose(f); return 0;
}
