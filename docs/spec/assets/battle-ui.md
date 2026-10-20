# Battle UI graphics — format (SLUS_006.28)

This page covers the byte formats of the UI's graphics: the glyph atlas,
the UI palettes and the icons of LUNADATA resource 1, the common sprite
sheet of resource 2, and the label font in the EXE. Where each asset is
drawn (the cell grid, the HUD, the menus) is
[../code/subsystems/menu-ui.md](../code/subsystems/menu-ui.md)
§"The UI's graphics and text layer". Every rule below is read from the
code; a capture of video memory in battle, taken with station-tools (a
custom decomp toolkit that will be released later), agrees with the
placement and colours (rows 502, 503, 504 and 507, and the atlas and icon
rectangles, match it exactly).

## Resource 1 (SYSTEM.DAT)

Loaded at boot (`main` (`0x80013D2C`) → `resource_load_run(1)`
(`0x800202CC`)), walked by `resource_command_run` (`0x800202FC`). Its
commands, in order: type 1 (the string block), type 2 (the atlas TIM),
type 12 × 3 (palettes, args 2, 3, 7).

### Glyph atlas (type 2)

`resource_tim_add` (`0x800208A0`): a type-2 command with arg 0 is unpacked
and sent at once; any other arg is queued for `resource_tim_load`. The
payload is unpacked by `inflate_buffer` (`0x80021D6C`) into a TIM, which
`tim_image_load` (`0x8002073C`) sends with LoadImage.

* 4bpp, 256 × 128 texels, VRAM (960, 384). Pixel values 0..3 only.
* The TIM's CLUT flag (bit 3) is set but its CLUT block is header-only
  (length 12, 0 × 0): `tim_image_load` sends it anyway, a zero-size
  upload. The colours are row 507's.
* Drawn through tpage `0x1F` (4bpp at (960, 256)), so a draw's `v` is
  atlas row + 0x80.

Cell rules:

| cell kind | function | code `c` → texels |
|---|---|---|
| 1 | `ankprint_glyph_draw` (`0x80025B90`) (writer `ankprint_glyph_set` (`0x80025DC0`)) | `c < 0x80`: 8 high, u = (c & 0x1F)·8, v = (c >> 5)·8 + 0x80. `c ≥ 0x80`: 16 high; `c ≥ 0xA0` reads `c + 0x20`; v = ((c & 0xE0) >> 2) \| 0x80 — so 0x80-0x9F at v 0xA0, 0xA0-0xBF at 0xB0, 0xC0-0xDF at 0xB8, 0xE0.. wrap to 0x80. Width = `((cell >> 11) & 7)·8` from the CELL; CLUT = row 507 sub-CLUT `11 + (cell >> 14)` |
| 0 | `packet_sprite_add` (`0x800256C0`) via `ankprint_cell_draw` (`0x80025C20`) | 8 × 8, u = (c & 0x1F)·8, v = (c >> 5)·8 + 0xB8, CLUT 0x7ECE (sub-CLUT 14) |

At run time label strips 6-7 (`g_FontLines` (`0x800A7DC8`): (960, 478) /
(960, 495)) are sent over atlas rows 94..127.

### UI palettes (type 12)

`resource_palette_row_load` (`0x80020ACC`): n = `(word >> 8) & 0xFF`;
`inflate_buffer` → 256 entries; `palette_bit15_set(pal, 0x100, 0xFF)`
(`0x800207E8`) zeroes entry 0 and ORs `0x8000` into every other entry; for
n = 7 `palette_glyph_zero_set` (`0x8002087C`) also zeroes entries 0xB0,
0xC0, 0xD0, 0xE0 (entry 0 of sub-CLUTs 11-14); `palette_rows_set`
(`0x8002061C`) → `palette_rows_load` (`0x80020588`) LoadImage (0, 500 + n,
256, 1). Rows: 502 icons, 503 portraits, 507 text.

### Icons (type-1 block + 0x899C)

`main_buffer_init` (`0x80013E04`) → `IconInit(block + 0x899C)`
(`0x8001ABCC`) sets both the table and the stream base. Table: u16 offsets
relative to that base; 256 entries (first offset 0x200 — a layout reading,
the code bounds the icon number only by its u16). Each is an `lz_unpack`
(`0x80022BF8`) stream of 576 bytes = 24 × 24 8bpp.

`IconImageLoad` caches an icon in one of 20 slots: LoadImage 12 × 24
halfwords at (128 + 12·(slot % 10), 448 + 24·(slot / 10));
`IconGarbageCollect` frees slots no cell references. `IconCellDraw`
(`0x8001AD60`) draws it: SPRT 24 × 24, tpage 0x92 (8bpp at (128, 256)),
CLUT 0x7D80 = row 502, colour 0x808080 — or **0x404040 when the cell's
bit 7 is set** (dimmed: a half-brightness modulation at draw time, not a
palette).

## Resource 2 (the common FX sheet)

Resource 2 is the file the archive's table of contents calls STFIELD.DAT
(sectors 49–53). `sprite_reset(y)` (`0x80018DBC`, sprite_list.c) reads it:
`main` calls it with y = 0 at boot, `sprite_init` (`0x80018BFC`) with
y = 0 at its end (`sprite_init_copy` (`0x80063B10`) runs it from
`GameStart` and after every battle from `MapBtlRun`), and `MenuNoWalkRun`
(`0x80075BD8`) with y = 0x40. Header: eight u16, each a place in 4-byte units.

| word | meaning |
|---|---|
| 0 | the CLUT's place. Entry 0 of each of the 16 sub-CLUTs zeroed (`clut[i·16] = 0`), then Sony's `LoadClut` (256 × 1) to (0, 507), and the 256 colours copied to row 7 of the palette buffer (`0x801D6980`). No 0x8000 OR. **This is row 507 from the game's start on**: `main` loads it before resource 1, whose n = 7 row then covers it, but `sprite_init` runs `sprite_reset` again (the two rows differ in 51 entries, none in sub-CLUTs 11-14). |
| 1 / 3 / 5 | places of three packed sprite data blocks → `g_SpriteDataAddresses` = {`g_SpriteData0`, `g_SpriteData1`, `g_SpriteData2`} |
| 2 / 4 / 6 | each block's byte size (4840 / 212 / 4184) — not read by the loader |
| 7 | the sheet's place: `inflate_buffer` into `g_LoadBuffer`, LoadImage (960, y, 64, 256 − y) from line y |

**The texel-record patch.** For each block: `q = d + hdr[4]·4`, `n =
(hdr[5] − hdr[4]) / 3`, then `*(u16 *)(q + 2) = 10` for n records — every
texel record points at page slot 10 = `g_SpritePageCorners[10]` =
(960, 0).

**The sheet** inflates to 232 lines (29696 bytes) of 4bpp, 256 texels
wide. A y = 0 upload covers 256 lines; lines 232..255 are stale buffer
bytes, not resource data.

**Which sub-CLUT.** A block is bound to a display object by
`BtlRecordSpriteDataSet(obj, block, word, anim)` (`0x80089A3C`), which
stores `(word & 0x3FF) | 0x2000` in the object's +0x0E — row 507
(`(w >> 8) & 3` = 3 over base 504), sub-CLUT `(w & 0xFF) >> 4`. Every such
binding of resource 2's blocks in the C:

| bank | word | sub-CLUT | caller |
|---|---|---|---|
| 2 `g_SpriteData2` | 0x730 | 3 | `BtlMenuInit` (`0x800441B4`): battle UI record 0 (`g_BtlMenuMarkRecords`), anim 6 |
| 0 `g_SpriteData0` | 0x760 | 6 | `BtlMenuInit` (`0x800441B4`): records 1-3, anims 2 / 0xA / 0xB |
| 0 | 0x780 | 8 | `BtlShowPlacesPlay` (`0x8007F2D8`) (the heal sparkle) via `BtlEffectShowPlacesRun` (`0x8007F42C`) (`BtlShow11Run` (`0x8007FB94`), `BtlShow14Run` (`0x8007FD34`)) and `BtlActingShowPlacesRun` (`0x8007F518`) (`BtlShow30Run` (`0x80085818`), `BtlShow31Run` (`0x8008585C`)) |
| 0 | 0x790 | 9 | `BtlShowPlacesPlay` (`0x8007F2D8`) via `BtlActingShowPlacesRun` (`0x8007F518`) (`BtlShow32Run` (`0x800858A0`), `BtlShow33Run` (`0x800858E4`)) |
| 2 | 0x700 / 0x710 | 0 / 1 | `BtlAmountShowStart` (`0x80088EA8`) — the impact (0x710 for a strong hit) |
| 2 | 0x720 | 2 | `BtlAmountShowStart`, amount kind 6, anim 5 |
| 2 | 0x7C0 | 12 | `BtlAmountShowStart`, amount kind 8, anim 7 |
| 2 | 0x7B0 | 11 | `BtlAmountShowStart`, amount kind 9, anim 8 |
| 2 | 0x7F0 | 15 | `BtlShowRecordTravelRun` (`0x800859A8`) from `BtlShow3ARun` |
| 2 | 0x790 | 9 | `BtlShowRecordTravelRun` (`0x800859A8`) from `BtlShow3BRun` / `BtlShow3CRun` |

Outside a battle the same sparkles of bank 0 are played by
`MenuUseShowPlacesPlay` (`0x8007EFC0`), which its callers hand the same
words 0x780 and 0x790.

Bank 1 (`g_SpriteData1`, 4 poses) is never bound that way:
`MapObjectsDataLoad` (`0x80065418`) installs it as entry 0 of the field's
sprite data table `g_MapSpriteDataEntries`, and `MapObjectInit`
(`0x800656CC`) binds every kind-7 map object to entry 0. Kind 7 gets the
draw word 0x2370 (`MapRecordInit` (`0x80065940`)): `GetClut(0x70, 0x1F8 |
3)` = row 507, entries 0x70..0x7F, sub-CLUT 7, which resource 2 sends. Its
four poses are a treasure chest — closed (16 x 16) and open (16 x 24),
twice over in two colourings (poses 0 / 1 and 2 / 3).

The sheet holds the hit sparks, the ATC/DEF tags, the Buy/Sell plates,
the menu frame corners and arrows (bank 0) — what each anim is for is its
caller's.

## The label font (EXE)

Glyphs: `g_FontPatterns20` (`0x800B217C`, codes 0x20..0x7A) and
`g_FontPatterns9908` (`0x800B035C`, 0x9908..0x99F8), 32 bytes each, of
which 30 are read: fifteen big-endian u16 rows, bit 15 leftmost.

* **Select** — `font_pattern_get` (`0x80026B1C`): the two ranges, else the
  BIOS Kanji ROM (`Krom2RawAdd` (`0x8008B428`)), `g_FontBlankPattern` on
  failure. Every code in the string table is in range, so the ROM path
  never runs on this disc.
* **Outline** — `font_pattern_outline` (`0x80026B98`): rows a[1..15] = the
  glyph, a[0] = a[16] = 0; outline b[i] = a[i−1] \| a[i+1] \| a[i] \|
  a[i] << 1 \| a[i] >> 1 — a plus-shaped dilation, no diagonals. 17 rows of
  2-bit pixels: 1 = ink, else 3 = outline, else 0. Stored big-endian (the
  2bpp row as a BE u32).
* **Draw** — `font_code_put` (`0x80026D64`) (`void (u16 glyph, s32
  mode)`): rows 0..15 only (row 16, the outline under the last glyph row,
  is dropped). Width = rightmost non-zero column (ink or outline) + 1;
  blank = 5. Columns are found through `g_FontColumnShifts` (`0x800B2EE4`)
  = {6,4,2,0,14,12,10,8,22,…} — the column's shift once the BE row is read
  back as a little-endian word. Mode 0 (centred): align to the next 16-px
  cell, advance (16 − width)/2 (8 for a blank), draw; any other mode:
  draw at the pen.
  Each column advances 1 px (`font_pen_move` (`0x800270CC`)). The nibble
  written into the 16 × 16 4bpp cell is `style | value` (style
  `g_FontColBits`, `font_col_set` (`0x800270BC`); `BtlMenuTextStringPut`
  (`0x80048CA4`)'s callers pass 0, `(item disabled) · 4`, or the colour
  of a row of `g_BtlMenuPresetCells`).
* **Colour** — the strips draw with CLUT 0x7ECE (`font_cell_draw`
  (`0x80016134`)): row 507 sub-CLUT 14, so value 1/3 at style 0/4/8/12
  show entries 1/3, 5/7, 9/11, 13/15 of resource 2's sub-CLUT 14.
