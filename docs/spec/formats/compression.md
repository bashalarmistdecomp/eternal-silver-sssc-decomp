# Format: compression

This page covers how the game's data is packed and the routines that
unpack it. The game's own code has two unpackers, both in inflate.c:
`inflate_buffer` (`0x80021D6C`), Mark Adler's `inflate.c` as gzip 1.2.4
ships it, taking its input 16 bits at a time, and `lz_unpack`
(`0x80022BF8`), a small LZ unpacker with nothing of `inflate.c` in it.
What was compared with the published `inflate.c`, and why each routine has
the name it has, is in docs/naming.md §"A published source file's names".
The executable on the disc is packed too, in `lz_unpack`'s format: its
loader's unpacker is `lz_unpack`'s code at another address, outside the
image ([../code-decompilation.md](../code-decompilation.md) §"The packing",
[../code/subsystems/bootstrap.md](../code/subsystems/bootstrap.md)).

## Deflate — `inflate_buffer` (`0x80021D6C`)

Every deflate stream the game unpacks goes through `inflate_buffer`
(30 calls in 24 functions): resource commands 1, 2, 3, 4, 7, 12 and 16,
the FACE portrait blocks, a map's object definitions, the sprite data and
the battle data. Its five functions (inflate.c, all C):

| Function | What it is |
|---|---|
| `inflate_buffer` (`0x80021D6C`) | stands where gzip's `inflate()` stands: clears the bit buffer and the window position, sets the input and output pointers, calls `inflate_block_dynamic` until it returns the last-block bit, and returns the bytes written |
| `inflate_block_dynamic` (`0x80021DD8`) | `inflate_block`'s first statements and then `inflate_dynamic`'s body in one function |
| `huft_build` (`0x800222C4`) | gzip's `huft_build`; tables come from a pool, not `malloc` |
| `huft_pool_reset` (`0x8002282C`) | called where gzip calls `huft_free`; it only puts the pool pointer back |
| `inflate_codes` (`0x80022840`) | gzip's `inflate_codes`, a leaf; the window is the output itself |

The bit buffer `bb`/`bk` is `g_Bb`/`g_Bk`, the input pointer
`g_InflateInput` is a `u16 *`, and each refill takes one whole word
(`b |= *g_InflateInput++ << k; k += 16`). The words are little-endian, so
a word's low byte comes first and the bits arrive in the order a byte-wise
reader takes them: the bitstream is RFC 1951's. What the game's copy does
differently from gzip's is below.

- **Only dynamic-Huffman blocks exist.** The block decoder reads the
  last-block bit, then drops the two block-type bits without looking at
  them; there is no stored (type 0) or fixed (type 1) path, and no error
  path for type 3. A stream whose type bits are not 2 is still decoded as
  dynamic.
- **The code-length repeat codes are the standard ones**: 16 = repeat the
  last length 3..6 times (2 extra bits), 17 = 3..10 zeros (3 bits),
  18 = 11..138 zeros (7 bits). In the assembly the three counts appear as
  `x + 2`, `x + 2` and `x + 10` because GCC keeps `j - 1` for the
  `while (j--)` loop.
- **The tables in the image are gzip's, unchanged**: `border` =
  `g_Border` (u8[19]: 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2,
  14, 1, 15), and four `u16` arrays — `cplens` `g_Cplens` (3 … 258, 0, 0),
  `cplext` `g_Cplext` (0×8, 1×4, 2×4, 3×4, 4×4, 5×4, 0, 99, 99), `cpdist`
  `g_Cpdist` (1 … 24577), `cpdext` `g_Cpdext` (0, 0, 0, 0, 1, 1, … 13, 13)
  — plus `mask_bits` `g_MaskBits` (u16[17]) and `lbits`/`dbits` = 9/6
  (`g_Lbits`/`g_Dbits`), at `0x800AF4FC` to `0x800AF608` and `0x800B8700`.
  Read as bytes, the `u16` extra-bit tables look interleaved with zeros;
  they are not.
- What the original leaves out of gzip's version: the `nl > 286 || nd > 30`
  and `i + j > n` range checks, `huft_build`'s two `return 2` checks for a
  set of code lengths that is too large, and every test of `huft_build`'s
  and `inflate_codes`' results. Malformed input is not rejected.
- **`inflate_codes`** (`0x80022840`). Per symbol it looks up
  `b & mask_bits[bl]` in `tl`, follows sub-table links while `e > 16`, and
  then: `e == 16` a literal byte, `e == 15` end of block, anything else a
  length with `e` extra bits followed by a distance through `td`. The 15 and
  16 are `huft_build`'s own `e` values for end-of-block and literal entries,
  not symbols of the stream. gzip's `e == 99` invalid-code exits are
  absent — it always returns 0.
- **There is no sliding window.** Output goes straight into the
  destination buffer: `g_InflateWindow` is the current 32 KiB output block
  and `g_Wp` (`wp`) the write index in it. gzip's flush at `w == 0x8000`
  here only records `wp`, moves `g_InflateWindow` on by 0x8000 and resets
  `w`. A match is copied in runs of `e = min(n, 0x8000 - max(d, w))` with
  the source index `d = (w - distance) & 0x7FFF`: `d <= w` reads the
  current block, `d > w` reads the previous block at
  `g_InflateWindow + d - 0x8000` (the same byte gzip's circular window
  would hold), and an overlapping match (`w - d < e`) goes byte by byte.
  This is a plain linear-buffer inflate for every distance up to 32767.
  **One edge differs: a distance of exactly 32768** gives `d == w`, which
  takes the byte-by-byte arm and copies each destination byte onto itself
  — the output keeps whatever the buffer held, where gzip's window (and a
  linear-buffer decoder) would return the bytes 32 KiB back. gzip's deflate
  never emits a distance above 32506 (`WSIZE - MIN_LOOKAHEAD`), so no
  stream is expected to reach it; none on the disc has been checked for
  it.

**Reading the streams outside the game.** A standard raw-deflate decoder
(zlib with `wbits = -15`) reads the same bitstream. It decodes a stream as
the game does when the stream holds only dynamic blocks, codes that zlib
accepts and no distance of 32768; it differs where the game's copy departs
from gzip's (above). The game's streams work in the game only because
every one of them uses dynamic blocks. The map data, FACE blocks and
sprite pages have been unpacked that way, and a standard raw-deflate
decoder gives the same unpacked sizes as the game's routine (measured with
station-tools, a custom decomp toolkit that will be released later).

## LZ — `lz_unpack` (`0x80022BF8`)

- **Used by**: the tile maps' packed blocks (the primary map's 8 × 8-tile
  blocks and the attribute plane's 16 × 16-cell blocks) and the icons
  `IconImageLoad` unpacks; its five callers are `bg_block_load`,
  `bg_tile_set`, `bg_attribute_block_load`, `bg_attribute_set` and
  `IconImageLoad`. Orphan region entity/animation data on the disc is
  packed the same way.
- The executable's own packed stream is in the same format, flag bits
  with two reference modes (short + long);
  [../code-decompilation.md](../code-decompilation.md) §"The packer's
  parse" has how the original packer chose its codes, measured on the
  executable's stream, which a packer written to those rules reproduces
  byte for byte. Whether the map blocks were packed by the same rules is
  not measured.

**The format as `lz_unpack(u8 *src, u8 *dst)` reads it** (inflate.c):

- Flags are a bit stream, LSB first, refilled one byte at a time from the
  same pointer the data bytes come from.
- `1` — a literal: one byte copied.
- `0 1` — a short match: one byte `b`; length `(b >> 6) + 2` (2..5),
  distance `b | 0xFFFFFFC0` (−64..−1).
- `0 0` — a long match: two bytes, big-endian, `w`; distance
  `w | 0xFFFFF000` (−4096..−1), length `(w >> 12) + 2` (3..17). A zero
  length nibble takes one more byte `n`: length `n + 2` (3..257), and
  **`n == 0` ends the stream**.
- There is no window buffer. A match copies forwards, a byte at a time,
  from `dst + written + distance`, so an overlapping source repeats.
- Nothing bounds the input or the output: the end marker is the only
  stop, and a reference before `dst` reads whatever memory is there. It
  returns the bytes written; none of its five callers reads that.

Measured by running the C's algorithm over all 10,801 distinct packed
primary-map blocks of the 97 maps: every block unpacks to exactly 128
bytes (64 tiles), and none references before its start; station-tools'
decoder, which adds two guards a file reader needs (it stops at the end of
its input, and a reference before the start yields zeros), gives identical
output on every block. "128 bytes per block" is therefore a property of
the primary map's data, not of the routine — the attribute plane's blocks
unpack to 0x200 bytes through the same code.

## Which compression for which data

A map is a LUNADATA resource, a list of commands that
`resource_command_run` (`0x800202FC`) walks
([../graphics.md](../graphics.md) §"World Map Arrangement Data"); the
handlers say how each payload is unpacked.

| Data | Where | Compression | Unpacked size |
|------|-------|-------------|---------------|
| Map texture pages (256×256 8-bit) | command 3, sub-number 0..2 or 0x80..0x82 (`resource_tpage_load`) | deflate | 65,536 B |
| Small texture pages | command 3, sub-number 3..0x7F | deflate | 16,384 B |
| Palettes | command 4 (`resource_palette_load`) | deflate | 512 B or 1,024 B |
| Palette row | command 12 (`resource_palette_row_load`) | deflate | one row of 0x100 colours |
| Pictures (TIM), the bromides' among them | command 2 (`resource_tim_add`): sub-number 0 unpacked at once, any other kept and unpacked later by `resource_tim_unpack` | deflate | 72K–82K per bromide TIM |
| Second bg layer | command 7 (`resource_bg_second_set`) | deflate | 16,384 B (0x80 rows of 64 cells) |
| Tile animation images | command 16 (`resource_tile_animation_set`) | deflate | pieces of 0x100 bytes |
| The string block (resource 1) | command 1 (`resource_command01_set`) | deflate | 0x15730 B |
| Primary map blocks | command 5 (`resource_bg_set`), the payload copied as it is | LZ, a block at a time | 128 B per block |
| Attribute-plane blocks | command 6 (`resource_bg_attribute_set`), the payload copied as it is | LZ, a block at a time | 0x200 B per block |
| Icons | inside the string block, from `+0x899C` | LZ (`IconImageLoad`) | |
| FACE portraits | FACE blocks | deflate | 20,480 B (5 × 64×64) |
| Sprite texture pages | Sprite block sub-block 2 | deflate | 65,536 B |
| EXE payload | SLUS_006.28 | LZ (the loader's copy of `lz_unpack`) | 690,176 B |
| Orphan entity data | Orphan LZ sectors | LZ | 64B–27KB |
