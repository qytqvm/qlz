# QLz stream format ("QSPL", version 1)

```
"QSPL"  0x01  varint(original size)  sequence*
```
Varints are unsigned LEB128 (7 bits per byte, high bit = more).

Each sequence:
```
token     1 byte: lit(3 bits) | rep(1 bit) | match(4 bits)
[varint]  lit - 7            when lit == 7
literals  lit bytes
-- the final sequence stops here once the original size is reached --
[varint]  offset             when rep == 0 (otherwise reuse the previous offset)
[varint]  match - 15         when match == 15
```
Match length = match + 4. Offset counts back from the current output position (1 .. 2,097,151).
The decoder rejects zero offsets, offsets before the start, overruns and trailing bytes.

Encoder: 13-bit hash of 4 bytes (32 KB table), checks the previous offset first then one hash slot,
greedy, and skips faster through data that does not match. Any encoder producing valid sequences is compatible.
