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

## Benchmarks & Performance

QLz combines a **2 MB sliding match window** with a high-throughput streaming decoder designed for instant data restore.

### Headline Results vs Google Snappy 1.2.1

*Single core, Intel Xeon 2.3 GHz, `-O3`.*

| Metric | QLz | Google Snappy 1.2.1 | Performance Advantage |
|---|---:|---:|---:|
| **Decompression Speed** | **~2,070 MB/s** | ~1,690 MB/s | **+22.5% faster restore** |
| **Output Size (30-file suite)** | **2,143,717 B** | 2,322,058 B | **7.68% compression advantage over Snappy** |
| **Overall Reduction** | **56.53%** | 52.91% | **178,341 bytes smaller total footprint** |
| **Match Window** | **2,097,151 bytes (2 MB)** | 65,536 bytes (64 KB) | **32× larger cross-file pattern reach** |


### Speed & Density Highlights on Production Payloads
* **WebAssembly (`catalog_module.wasm`):** QLz is **16.1% smaller** and decompresses **+69.6% faster** (7,194 MB/s vs 4,241 MB/s).
* **ARM64 Binary (`jq-linux-arm64.bin`):** QLz saves **88,253 bytes** (**7.8% advantage**) with **+25.9% faster restore**.
* **Structured JSON (`tables.json`):** QLz saves **41,342 bytes** (**12.4% advantage**) with **+8.0% faster restore**.
* **Database Seeds (`shopmart_seed.sql`):** QLz is **5.2% smaller** and restores **+77.7% faster** (6,700 MB/s vs 3,771 MB/s).

Full file-by-file metrics and instructions to run your own benchmarks are in [BENCHMARKS.md](BENCHMARKS.md).


## Stream format

See [docs/FORMAT.md](docs/FORMAT.md). Streams start with `QSPL` 01 and the original size, so they identify themselves.

## Licence

MIT — see [LICENSE](LICENSE). QLz is developed by [Qytte](https://qytte.com).


### How to Run & Benchmark

Clone the repository and compile the standalone C harness:

```bash
# Build the benchmark harness
gcc -O3 -o qlz-bench example.c

# Run benchmark across target files
./qlz-bench path/to/files/*

