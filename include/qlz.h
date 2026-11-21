/* QLz (stream format "QSPL") - standalone, zero-dependency C99 header. Copyright (c) 2026 Qytte. MIT License.
 * Ultra-fast byte-aligned LZ for low-latency storage, queues and RPC; positioned against LZ4 / Snappy.
 * Same LZ token stream as QSwift Turbo method 1, tuned for speed: 13-bit hash table (32 KB, stays in L1),
 * single probe (repeat offset, then one slot), greedy, LZ4-style skip acceleration on misses, 2 MB window.
 *
 *   #define QSPL_IMPLEMENTATION   (in one .c file)
 *   size_t    qspl_bound(size_t n);                                   // worst-case output size
 *   size_t    qspl_compress(const uint8_t *in, size_t n, uint8_t *out); // returns bytes written
 *   long long qspl_size(const uint8_t *in, size_t n);                 // original size, -1 if not QSPL
 *   long long qspl_decompress(const uint8_t *in, size_t n, uint8_t *out, size_t cap); // bytes, <0 on error
 *
 * Stream: "QSPL" 01 | varint original size | LZ tokens. Token = lit(3) | rep(1) | match-4(4); literals;
 * [offset varint]; [match ext varint]. Decoding never reads or writes outside the given buffers. */
#ifndef QSWIFT_QLZ_H
#define QSWIFT_QLZ_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
size_t qspl_bound(size_t n);
size_t qspl_compress(const uint8_t *in, size_t n, uint8_t *out);
long long qspl_size(const uint8_t *in, size_t n);
long long qspl_decompress(const uint8_t *in, size_t n, uint8_t *out, size_t cap);
#ifdef __cplusplus
}
#endif
#endif

#ifdef QSPL_IMPLEMENTATION
#ifndef QSPL_NO_LIBC
#include <string.h>
#endif
#define QSPL_HB 13
#define QSPL_WINDOW ((1u << 21) - 1)
#define QSPL_SKIP 2
static inline uint32_t qspl_r32(const uint8_t *d) { uint32_t v; memcpy(&v, d, 4); return v; }
static inline size_t qspl_put(uint8_t *b, size_t w, uint64_t v) { while (v >= 128) { b[w++] = (uint8_t)((v & 127) | 128); v >>= 7; } b[w++] = (uint8_t)v; return w; }
static inline long long qspl_mlen(const uint8_t *a, const uint8_t *b, long long k, long long lim) {
  while (k + 8 <= lim) { uint64_t x, y; memcpy(&x, a + k, 8); memcpy(&y, b + k, 8);
#if defined(__GNUC__) && (!defined(__BYTE_ORDER__) || __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    if (x != y) return k + (__builtin_ctzll(x ^ y) >> 3);
#else
    if (x != y) break;
#endif
    k += 8; }
  while (k < lim && a[k] == b[k]) k++;
  return k;
}
size_t qspl_bound(size_t n) { return n + (n >> 3) + 96; }
static size_t qspl_lits(uint8_t *b, size_t w, const uint8_t *d, size_t lit, int fast) {
  if (fast && lit <= 16) memcpy(b + w, d, 16); else memcpy(b + w, d, lit);
  return w + lit;
}
size_t qspl_compress(const uint8_t *d, size_t n, uint8_t *b) {
  int32_t head[1 << QSPL_HB];
  size_t w = 0; b[w++] = 'Q'; b[w++] = 'S'; b[w++] = 'P'; b[w++] = 'L'; b[w++] = 1; w = qspl_put(b, w, n);
  long long last = (long long)n - 4, ip = 0, anchor = 0; uint32_t rep = 0, miss = 0;
  for (int i = 0; i < (1 << QSPL_HB); i++) head[i] = -1;
  while (ip <= last) {
    uint32_t q = qspl_r32(d + ip), h = (q * 0x9e3779b1u) >> (32 - QSPL_HB);
    long long L = 0, O = 0, lim = (long long)n - ip;
    if (rep && (long long)rep <= ip && qspl_r32(d + ip - rep) == q) { L = qspl_mlen(d + ip - rep, d + ip, 4, lim); O = rep; }
    else { int32_t c = head[h];
      if (c >= 0 && ip - c <= (long long)QSPL_WINDOW && qspl_r32(d + c) == q) { L = qspl_mlen(d + c, d + ip, 4, lim); O = ip - c; } }
    head[h] = (int32_t)ip;
    if (L < 4) { ip += 1 + (miss++ >> QSPL_SKIP); continue; }
    miss = 0;
    size_t lit = (size_t)(ip - anchor); uint32_t isRep = (uint32_t)O == rep; long long ml = L - 4;
    b[w++] = (uint8_t)(((lit < 7 ? lit : 7) << 5) | (isRep << 4) | (ml < 15 ? ml : 15));
    if (lit >= 7) w = qspl_put(b, w, lit - 7);
    w = qspl_lits(b, w, d + anchor, lit, anchor + 16 <= (long long)n);
    if (!isRep) { w = qspl_put(b, w, (uint64_t)O); rep = (uint32_t)O; }
    if (ml >= 15) w = qspl_put(b, w, (uint64_t)(ml - 15));
    ip += L; anchor = ip;
    if (ip - 2 <= last) head[(qspl_r32(d + ip - 2) * 0x9e3779b1u) >> (32 - QSPL_HB)] = (int32_t)(ip - 2);
  }
  if (anchor < (long long)n) { size_t lit = n - (size_t)anchor; b[w++] = (uint8_t)((lit < 7 ? lit : 7) << 5);
    if (lit >= 7) w = qspl_put(b, w, lit - 7); memcpy(b + w, d + anchor, lit); w += lit; }
  return w;
}
static int qspl_get(const uint8_t *s, size_t end, size_t *p, uint64_t *v) {
  uint64_t r = 0; int sh = 0; uint8_t c;
  do { if (*p >= end || sh > 56) return -1; c = s[(*p)++]; r |= (uint64_t)(c & 127) << sh; sh += 7; } while (c & 128);
  *v = r; return 0;
}
long long qspl_size(const uint8_t *b, size_t n) {
  if (n < 6 || b[0] != 'Q' || b[1] != 'S' || b[2] != 'P' || b[3] != 'L' || b[4] != 1) return -1;
  size_t p = 5; uint64_t v; if (qspl_get(b, n, &p, &v)) return -1; return (long long)v;
}
#define QSPL_GETV(dst) do { if (qspl_get(s, end, &p, &v_)) return -1; dst = v_; } while (0)
long long qspl_decompress(const uint8_t *s, size_t end, uint8_t *out, size_t cap) {
  long long N = qspl_size(s, end); if (N < 0 || (unsigned long long)N > cap) return -1;
  size_t n = (size_t)N, p = 5, op = 0; uint64_t rep = 0, v_; QSPL_GETV(v_);
  /* Fast loop: while 64 bytes of slack remain on both sides, copies use fixed 8/16-byte steps. */
  while (op + 64 <= n && p + 64 <= end) {
    unsigned t = s[p++]; size_t lit = t >> 5;
    if (lit < 7) memcpy(out + op, s + p, 8);
    else { uint64_t x; QSPL_GETV(x); lit = 7 + (size_t)x;
      if (lit > end - p || lit > n - op) return -1;
      if (lit + 32 <= end - p && lit + 32 <= n - op) { for (size_t k = 0; k < lit; k += 16) memcpy(out + op + k, s + p + k, 16); }
      else memcpy(out + op, s + p, lit); }
    op += lit; p += lit;
    if (op >= n) { if (op > n) return -1; break; }
    uint64_t off;
    if (t & 16) off = rep;
    else { unsigned b0 = s[p];
      if (b0 < 128) { off = b0; p += 1; }
      else { unsigned b1 = s[p + 1]; if (b1 < 128) { off = (b0 & 127) | (b1 << 7); p += 2; } else { uint64_t x; p += 2; QSPL_GETV(x); off = (b0 & 127) | ((b1 & 127) << 7) | (x << 14); } }
      rep = off; }
    size_t ml = (size_t)(t & 15) + 4;
    if ((t & 15) == 15) { uint64_t x; QSPL_GETV(x); ml += (size_t)x; }
    if (off == 0 || off > op || ml > n - op) return -1;
    uint8_t *dst = out + op; const uint8_t *src = dst - off;
    if (ml + 16 <= n - op) {
      if (off >= 16) { for (size_t k = 0; k < ml; k += 16) memcpy(dst + k, src + k, 16); }
      else if (off >= 8) { for (size_t k = 0; k < ml; k += 8) memcpy(dst + k, src + k, 8); }
      else if (off == 1) memset(dst, src[0], ml);
      else { /* short repeat: copy one period, then double the copied span each step */
        for (size_t k = 0; k < off; k++) dst[k] = src[k];
        for (size_t k = off; k < ml; ) { size_t c = ml - k < k ? ml - k : k; memcpy(dst + k, dst, c); k += c; } }
    } else for (size_t k = 0; k < ml; k++) dst[k] = src[k];
    op += ml;
  }
  while (op < n) {
    if (p >= end) return -1;
    unsigned t = s[p++]; size_t lit = t >> 5;
    if (lit == 7) { uint64_t x; QSPL_GETV(x); lit = 7 + (size_t)x; }
    if (lit > end - p || lit > n - op) return -1;
    if (lit <= 16 && end - p >= 16 && n - op >= 16) memcpy(out + op, s + p, 16); else memcpy(out + op, s + p, lit);
    op += lit; p += lit;
    if (op == n) break;
    uint64_t off; if (t & 16) off = rep; else { QSPL_GETV(off); rep = off; }
    size_t ml = (size_t)(t & 15) + 4;
    if ((t & 15) == 15) { uint64_t x; QSPL_GETV(x); ml += (size_t)x; }
    if (off == 0 || off > op || ml > n - op) return -1;
    uint8_t *dst = out + op; const uint8_t *src = dst - off;
    if (off >= 16 && ml + 16 <= n - op) { for (size_t k = 0; k < ml; k += 16) memcpy(dst + k, src + k, 16); }
    else if (off >= 8 && ml + 8 <= n - op) { for (size_t k = 0; k < ml; k += 8) memcpy(dst + k, src + k, 8); }
    else for (size_t k = 0; k < ml; k++) dst[k] = src[k];
    op += ml;
  }
  return p == end ? N : -1;
}
#endif
