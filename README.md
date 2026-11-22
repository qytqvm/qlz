# QLz

**A tiny, fast, byte-aligned LZ compressor in one C99 header.** No dependencies, no allocation, no build system needed.

- One file: `include/qlz.h` (~140 lines), MIT licensed
- 2 MB match window (LZ4 and Snappy use 64 KB), so it finds repeats far apart in large files
- Decoder never reads or writes outside the buffers you give it; corrupt input returns an error
- Deterministic output: the same input always gives the same bytes on every platform

## Quick start

```c
#define QSPL_IMPLEMENTATION      /* in exactly one .c file */
#include "qlz.h"

uint8_t *packed = malloc(qspl_bound(n));
size_t   k      = qspl_compress(input, n, packed);

long long size  = qspl_size(packed, k);            /* original size, -1 if not a QLz stream */
uint8_t *out    = malloc(size);
long long got   = qspl_decompress(packed, k, out, size);   /* < 0 on corrupt input */
```

Input and output must be separate buffers. For untrusted files, check `qspl_size` against your own limit before allocating.

## Build, test, benchmark

```sh
make test                     # round-trip + corruption tests, then the example
make qlz                      # tiny command-line tool: ./qlz -c in out  /  ./qlz -d in out
make bench && ./qlz-bench your-files/*
```

## Benchmarks

Across the entire 30-file mixed test suite (including 21 short scripts and CSS snippets under 30 KB where a 2 MB window is unused and results differ by merely ~20–50 bytes):

| Metric | QLz | Google Snappy 1.2.1 | QLz Advantage |
|---|---:|---:|---:|
| **Total compressed size** | **2,143,717 B** | 2,322,058 B | **7.68% compression advantage over Snappy** (178,341 bytes saved) |
| **Average reduction** | **56.53%** | 52.91% | **+3.62% higher data reduction** |
| **Restore speed** | **~2,070 MB/s** | ~1,690 MB/s | **+22.5% faster restore speed** |
| **Match window** | **2 MB** | 64 KB | **32× larger match reach** |

### Understanding the differences
* **Where QLz wins:** Binaries, structured JSON, database dumps, WASM, and multi-megabyte payloads where repeated patterns exist outside Snappy's 64 KB reach. On these workloads, QLz saves tens of kilobytes per file and decompresses up to 86% faster.
* **Small script snippets (<30 KB):** On tiny text snippets, Snappy's 2-byte tag can be ~20 to 50 bytes smaller, but QLz still retains higher decompression throughput.

QLz's total lead comes from large files and binaries, where its 2 MB window pays off. On small text files (CSS, HTML, short source) Snappy is usually a little smaller. See [BENCHMARKS.md](BENCHMARKS.md) for every file, and run `qlz-bench` on your own data — results vary with the data.

## Stream format

See [docs/FORMAT.md](docs/FORMAT.md). Streams start with `QSPL` 01 and the original size, so they identify themselves.

## Licence

MIT — see [LICENSE](LICENSE). QLz is developed by [Qytte](https://qytte.com).
