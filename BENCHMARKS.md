# QLz benchmark — every file

Sizes in bytes. Same machine and settings as the README. Lower is better.

| File | Input | QLz | Snappy 1.2.1 | Smaller |
|---|---:|---:|---:|---|
| all.min.css | 102,641 | 36,587 | 36,598 | QLz |
| analytics_module.wasm | 95,141 | 29,716 | 28,936 | Snappy |
| api_handler.php | 76,539 | 17,343 | 17,958 | QLz |
| async-calls-sync.wast | 11,330 | 3,820 | 3,590 | Snappy |
| cancel-delivery.wast | 26,507 | 7,645 | 7,170 | Snappy |
| catalog_module.wasm | 95,123 | 13,262 | 15,806 | QLz |
| data_processor.py | 66,930 | 18,286 | 16,524 | Snappy |
| design.css | 20,260 | 5,842 | 5,182 | Snappy |
| employee_directory.html | 104,960 | 23,689 | 21,078 | Snappy |
| inventory_audit_report.html | 100,391 | 19,688 | 18,828 | Snappy |
| inventory.msgpack | 56,339 | 9,219 | 10,057 | QLz |
| inventory_processor.cpp | 56,340 | 8,409 | 8,027 | Snappy |
| jq-LICENSE.txt | 6,026 | 3,684 | 3,138 | Snappy |
| jq-linux-arm64.bin | 1,709,616 | 1,049,364 | 1,137,617 | QLz |
| jq-macos-arm64.bin | 807,984 | 402,034 | 465,175 | QLz |
| pixel-art-editor.html | 12,747 | 6,023 | 5,565 | Snappy |
| pixel-art-worker.js | 14,115 | 6,032 | 5,580 | Snappy |
| sample.csv | 106,912 | 57,032 | 52,987 | Snappy |
| sample.json | 66,467 | 11,851 | 12,754 | QLz |
| shopmart_seed.sql | 68,215 | 12,083 | 12,743 | QLz |
| style (2).css | 15,111 | 4,752 | 4,179 | Snappy |
| style (3).css | 10,202 | 4,338 | 3,906 | Snappy |
| style (5).css | 29,263 | 10,360 | 9,351 | Snappy |
| system_event_log.txt | 106,508 | 27,865 | 25,941 | Snappy |
| tables.json | 973,539 | 292,181 | 333,523 | QLz |
| tasks.py | 18,671 | 8,022 | 7,433 | Snappy |
| test_api.py | 29,682 | 7,220 | 6,724 | Snappy |
| test_forms.py | 15,910 | 5,598 | 5,221 | Snappy |
| transaction_ledger.txt | 100,362 | 35,611 | 34,615 | Snappy |
| user_agents.py | 27,509 | 6,161 | 5,852 | Snappy |
| **Total** | **4,931,340** | **2,143,717** | **2,322,058** | **QLz (−7.7%)** |

A raw run of `qlz-bench` on these files is in [bench/results/sample-run.txt](bench/results/sample-run.txt).
The sample files themselves are not included (some belong to third parties).

## Add your own results

```sh
make bench
./qlz-bench path/to/files/* > bench/results/$(date +%F)-my-data.txt
```
Commit the text file with a line saying the CPU and compiler.
