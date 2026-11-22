# QLz Benchmarks

QLz is engineered for high decompression throughput and multi-megabyte window reach. It delivers a **7.68% compression advantage over Google Snappy** in aggregate, with up to **+86% faster decompression speeds**.

---

## 1. Primary Benchmark: Structured Data, Binaries & Modules

These files represent the core production workloads QLz is designed for: compiled application binaries, WebAssembly modules, large structured JSON documents, serialization streams, and database dumps.

*Single core, Intel Xeon 2.3 GHz, `-O3`. Compared with Google Snappy 1.2.1.*

| Benchmark File | Input Size | QLz Size | Snappy Size | Compression Advantage | Restore Speed (QLz vs Snappy) | Winner |
|---|---:|---:|---:|---:|---:|:---:|
| `catalog_module.wasm` | 95,123 B | **13,262 B** | 15,806 B | **16.10% smaller** | **7,194 MB/s** vs 4,241 MB/s (+69.6%) | **QLz** |
| `jq-macos-arm64.bin` | 807,984 B | **402,034 B** | 465,175 B | **13.57% smaller** | **1,728 MB/s** vs 1,486 MB/s (+16.3%) | **QLz** |
| `tables.json` | 973,539 B | **292,181 B** | 333,523 B | **12.40% smaller** | **2,039 MB/s** vs 1,888 MB/s (+8.0%) | **QLz** |
| `inventory.msgpack` | 56,339 B | **9,219 B** | 10,057 B | **8.33% smaller** | **7,033 MB/s** vs 4,251 MB/s (+65.4%) | **QLz** |
| `jq-linux-arm64.bin` | 1,709,616 B | **1,049,364 B** | 1,137,617 B | **7.76% smaller** | **1,486 MB/s** vs 1,180 MB/s (+25.9%) | **QLz** |
| `sample.json` | 66,467 B | **11,851 B** | 12,754 B | **7.08% smaller** | **5,427 MB/s** vs 3,567 MB/s (+52.1%) | **QLz** |
| `shopmart_seed.sql` | 68,215 B | **12,083 B** | 12,743 B | **5.18% smaller** | **6,700 MB/s** vs 3,771 MB/s (+77.7%) | **QLz** |
| `api_handler.php` | 76,539 B | **17,343 B** | 17,958 B | **3.42% smaller** | **4,765 MB/s** vs 2,558 MB/s (+86.3%) | **QLz** |
| `all.min.css` | 102,641 B | **36,587 B** | 36,598 B | **QLz wins** | **2,128 MB/s** vs 1,783 MB/s (+19.3%) | **QLz** |
| **Category Totals** | **3,956,463 B** | **1,843,924 B** | **2,042,229 B** | **9.71% compression advantage** | **~2,150 MB/s (+27% faster)** | **9 of 9 wins** |

---

## 2. Full 30-File Reference Corpus (Mixed Web, Scripts & Systems)

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

---

## For more QLz benchmarks:

Go to: https://github.com/qytqvm/QSwift/tree/main/qlz
