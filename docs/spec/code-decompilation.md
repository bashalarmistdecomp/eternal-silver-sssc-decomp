# The main executable: packing, loading and layout

How the main executable is stored on the disc, how it gets into memory, and
how much of the disc's file the build reproduces. What runs at start-up is
in [code/subsystems/bootstrap.md](code/subsystems/bootstrap.md), the memory
map of the unpacked image in [code/engine-map.md](code/engine-map.md), and
the pages on its code are indexed in [code/README.md](code/README.md).

## The files

| File | Size | What it is |
|---|---|---|
| `SLUS_006.28` | 350,208 bytes (`0x55800`) | The boot file: a loader and the packed image of the program. Disc 2 carries the same file as `SLUS_008.99`. |
| `MENU.BIN` | 76,096 bytes (`0x12940`) | The movie-player overlay: code and data with no header, read to `0x80196000` ([code/subsystems/overlay.md](code/subsystems/overlay.md)) |

## The boot file

| File offset | Size | What |
|---|---|---|
| `0x0000` | `0x800` | The outer PS-X EXE header |
| `0x0800` | `0x800` | The loader, 940 bytes of it used, loaded at `0x80100000` |
| `0x1000` | `0x54800` | The packed stream, `0x54463` bytes of it used and the rest zero, loaded at `0x80100800` |

The outer header:

| Field | Value |
|---|---|
| Entry point (`pc0`) | `0x801001EC` |
| Load address (`t_addr`) | `0x80100000` |
| Size (`t_size`) | `0x55000` (348,160 bytes: the loader and the stream) |
| Stack (`s_addr`) | `0x801FFFF0` |

The stream unpacks to `0xA8800` (690,176) bytes, a second PS-X EXE, which
the loader writes to `0x80010800`. Its header:

| Field | Value |
|---|---|
| Entry point (`pc0`) | `0x80013C4C`, `__SN_ENTRY_POINT` |
| Load address (`t_addr`) | `0x80011000` |
| Size (`t_size`) | `0xA8000` (688,128 bytes) |
| Where it lies once unpacked | the header at `0x80010800`, the image at `0x80011000` |

The image a tree is built against, `decomp/SLUS_006.28/expected/code.bin`,
is this second executable's body ([../../BUILD.md](../../BUILD.md), step 2).
The loader does not use the inner header's other fields: it reads the entry
point from it and jumps there.

## The packing

The stream is in the format of the game's own LZ unpacker, `lz_unpack`
(`0x80022BF8`, `inflate.c`), which [formats/compression.md](formats/compression.md)
§"LZ — `lz_unpack` (`0x80022BF8`)" sets out: flag bits, literals, short
matches up to 64 bytes back and long ones up to 4096, and a long match
whose length byte is 0 as the end. The loader's unpacker is that same
function: its 88 words at `0x80100000` are `lz_unpack`'s, but for four `j`
instructions, whose targets follow where the code lies
([code/subsystems/bootstrap.md](code/subsystems/bootstrap.md)).
`recipe/extract.py` reads the boot file by the same rules (`lzss_unpack`),
and the image it gives has the sha1 the tree declares.

The executable's stream holds 226,878 codes: 127,890 literals, 25,209 short
matches, 69,986 two-byte long matches and 3,793 three-byte long matches.
It ends at `0x54463` with flag bits `0, 0`, the word `0x0FFF` and a zero
byte; the last flag byte's unused bits are zero, and the rest of the file
is zero.

### The packer's parse

How the original packer chose its codes was read off the disc's stream. It
was proven by packing the unpacked stream again to the same bytes, and the
same rules give back the Japanese release's stream (337,920 bytes,
unpacking to 681,984) unchanged, so they are the packer's, not a fit to one
file. At each position `p`:

1. **Search** the 4096 bytes before `p` (distance 1 to 4096; a match may
   run on into the bytes it is producing, as distance 1 does). The longest
   match wins, capped at **256** bytes (the length byte never exceeds 254);
   among equal lengths, the **nearest**. No reference reaches before the
   start of the output.
2. **No 2-byte match:** one literal.
3. **The longest match is 2 bytes:** at a distance up to 64, the short code;
   at a distance of 65 to 256, **both bytes go out as literals and `p + 1`
   is not searched**; at a greater distance, one literal. This is the only
   place the parse is not greedy: 6,195 literals stand where a match was on
   offer at `p + 1`.
4. **Longer:** length up to 5 and distance up to 64, the short code; else
   length up to 17, the two-byte long code; else the three-byte long code
   with length byte `length - 2`.

There is no lazy matching: 15,836 references were taken although `p + 1`
had a longer match. After the end code the file is zero-padded to a
multiple of `0x800`.

## How much of the disc's file the build reproduces

- `make` gives the image, sha1 `900ce6a9dfa9563ef7e0a129602945d1303436d8`
  ([../../BUILD.md](../../BUILD.md)).
- The inner header and the built image are the whole unpacked stream:
  sha256
  `a26cf90c74c222407343966c746095727d4ac181069138440bcb3c73db63267c`
  on both sides.
- The overlay's build (`make TARGET_SERIAL=SLUS_006.28-MENU`) is the disc's
  `MENU.BIN`, whole file: sha256
  `0fc09d66f7faf118a5be5e10001f130c989ed024d6595738a62914c02753ffd5`.

The disc's file itself was rebuilt from those parts: the headers with
station-tools, a custom decomp toolkit that will be released later, and the
stream with a compressor written for the purpose.

- **The stream.** A compressor written to the rules of §"The packer's
  parse" turns the unpacked stream back into the disc's `0x54463` stream
  bytes, all 226,878 codes the same; zero-padded to `0x54800` it is the
  disc's stream.
- **The headers.** Each comes back byte for byte from three of its fields,
  the entry point, the load address and the size, with every other field
  at the value a standard PS-X EXE header carries (the stack at
  `0x801FFFF0`, the region text).
- **The whole file.** The outer header, the loader and that stream are the
  disc's `SLUS_006.28`: sha256
  `af10ecf550016e538c02aa8f2f0b937e961ac5e1d0f7e3588b837309e931fbc4`.
  The loader was copied from the disc for this. In one run, the built image
  with its inner header, packed, padded and put behind the outer header
  and the disc's loader, gives the same file.

Not in this repository:

- **The compressor.** No `make` step packs the build; the build ends with
  the image.
- **The loader**, in any decomp tree. Its unpacker is `lz_unpack`'s code,
  and its start-up, `main` and constructor loops are read in
  [code/subsystems/bootstrap.md](code/subsystems/bootstrap.md), but none of
  it is C here.
- **A disc image.** No step masters one, and there is no sector EDC/ECC or
  ISO 9660 writer. An unpacked executable (the inner header and the image)
  booted on its own would skip the loader's work, among it the zeroing of
  the block at `0x80010000` whose bytes tell the game where to read its
  files from. It has not been booted.

## What the image holds

The image runs from `0x80011000` to `0x800B9000`: read-only data first,
then Sony's start-up object at `0x80013C44`, the game's code, Sony's
libraries and the data; [code/engine-map.md](code/engine-map.md) has the
map, and the top-level [README](../../README.md) the count of functions.
The data begins with the build stamp, `$at 99-04-19 21:32:42(dosuser)$`
(`g_BuildStamp`, `0x80011000`). `MENU.BIN` begins with the same text, and
`overlay_stamp_check` (`0x800232B0`) compares the two before the overlay is
used, so the overlay on the disc is taken only if it was built with this
program.

The developers' debug text is still in the read-only data, though
`debug_printf` (`0x800216EC`), which the game's code passes it to, does
nothing with its arguments. Sony's libgpu keeps its own messages beside
it:

| Address | Text | Used by |
|---|---|---|
| `0x80011038` | `bg_copy(%d,%d,%d,%d,%d,%d)` | `bg_copy` (`0x80015224`), through `debug_printf` |
| `0x800128EC` | two stars and `mapmix=%d` | `GameEnter` (`0x80059498`), through `debug_printf` |
| `0x80012CA8` | a star and `MapEffectSet(tbl=%x,sw=%d)` | `MapEffectSet` (`0x80068AC0`), through `debug_printf` |
| `0x80012CC8` | `mapeffect=0x%x` | `MapEffectSet`, through `debug_printf` |
| `0x80013050` | `DrawSyncCallback(%08x)...` | libgpu's `sys.o` (`libgpu_sys_1.c`), through the function pointer `D_800B5F9C` |
| `0x800130D0` | `LoadImage` | libgpu's `sys.o`: the name its rectangle check `func_8008BC6C` prints |
| `0x80013124` | `DrawOTag(%08x)...` | libgpu's `sys.o` (`libgpu_sys_1.c`, `libgpu_sys_5.c`), through the function pointer `D_800B5F9C` |

The game's text, the place names among it, is in [text.md](text.md), and
the tile engine, `bg_copy` among it, in
[code/subsystems/rendering.md](code/subsystems/rendering.md).
