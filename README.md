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

30 mixed files (web pages, CSS, JS, JSON, CSV, SQL, logs, source code, WebAssembly, program binaries), 4.93 MB total, best of 100 runs, one core, Intel Xeon 2.3 GHz, `-O3`. Compared with Snappy 1.2.1.

| | QLz | Snappy |
|---|---:|---:|
| Total output | **2,143,717 B** | 2,322,058 B |
| Reduction | **56.53%** | 52.91% |
| Restore speed | **~2,070 MB/s** | ~1,690 MB/s |
| Compress speed | ~630 MB/s | **~730 MB/s** |
| Files won on size | 9 of 30 | 21 of 30 |

Read this honestly: QLz's total lead comes from large files and binaries, where its 2 MB window pays off. On small text files (CSS, HTML, short source) Snappy is usually a little smaller. See [BENCHMARKS.md](BENCHMARKS.md) for every file, and run `qlz-bench` on your own data — results vary with the data.

## Stream format

See [docs/FORMAT.md](docs/FORMAT.md). Streams start with `QSPL` 01 and the original size, so they identify themselves.

## Licence

MIT — see [LICENSE](LICENSE). QLz is made by [Qytte](https://qytte.com).
