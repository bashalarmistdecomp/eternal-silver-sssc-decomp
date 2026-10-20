# Rendering: the tile layers, the packets and the frame

How the game puts a frame on the screen: the ordering table and the order
the layers are drawn in, the field's tile layer (`bg_first_draw`) and
second layer (`bg_second_draw`), the battle's tile layer
(`tile_layer_draw`), the palette rows the tiles use, the queued uploads of
animated tile cells, the game's small packet helpers, and the screen
set-up, the present and the frame profiler. Sprites are in
[../../sprites.md](../../sprites.md), the map's data in
[../../graphics.md](../../graphics.md), and Sony's side of the GPU in
[psyq/libgpu-libgs.md](psyq/libgpu-libgs.md).

Every packet goes through two words of the CPU's scratchpad:
`g_PacketWritePointer` (`0x1F80004C`), the cursor in the frame's packet
area, and `g_PacketOtBase` (`0x1F800040`), the address of the frame's
ordering table. `frame_start` (`0x8001F294`, frame.c) sets both for the
frame buffer that is not on show.

## The field's draw rule

What one field frame draws for a map, and in what order. The frame is
`MapFrameUpdate` (`0x80055840`) (the work), `sprite_list_draw`
(`0x8001A798`) (the sprites) and `MapFrameEnd` (`0x8005596C`) (the tail),
in that order (the field loop, `GameRun` (`0x800559F4`), map_frame.c).
Everything here is read from the C; none of it was checked against the
running game.

### The ordering table is drawn from entry 0x3FF down

The table has 0x400 entries (`length` 10, set by `screen_mode_set`
(`0x8001F0C8`), frame.c), one table for each frame buffer.

| step | where |
|---|---|
| the frame's table is cleared with `GsClearOt`: `tag = org + (1 << length) - 1`, then `ClearOTagR` | `frame_start`, frame.c → `GsClearOt` (`0x8008FB98`), libgs_gs_113.c |
| `ClearOTagR` links every entry to the one below it; entry 0 gets the terminator | `ClearOTagR` (`0x8008C0F0`) → `func_8008D0BC`, libgpu_sys_1.c |
| the present draws it with `GsDrawOt`: `DrawOTag(ot->tag)`, from the last entry | `frame_end` (`0x8001F30C`), frame.c → `GsDrawOt` (`0x8008FB68`), libgs_gs_111.c |

So a higher entry is drawn earlier: it is behind. Inside one entry the
packet linked last is drawn first. Every link in the image is
`tag = *ot | len; *ot = packet`, or `AddPrim` (`0x8008E9D8`), which does
the same, so of two packets at one entry the one queued first ends up in
front.

### The layers, back to front

| entries | what | queued by |
|---|---|---|
| (before the table) | the clear: `screen_mode_set` sets the drawing environment's `isbg` to 1, so the environment clears the drawing area to its colour bytes (`0x800E6F69` to `0x800E6F6B`) when it is sent. Nothing in the image writes those bytes after `__SN_ENTRY_POINT` (`0x80013C4C`) has zeroed `0x800B8820` to `0x800EAFE8`, so the clear is black | — |
| 0x3FF | the frame's draw area, linked last, so taken first | `frame_end` → `packet_draw_area_add` (`0x80015E24`) |
| 0x3C0..0x3FF | the second layer in mode 0: `0x3C0 + ((cell & 0x1FF) >> 3)` | `bg_second_draw` (`0x800148E0`) |
| 0x300..0x3FF | the tile layer: `0x300 + ((word >> 2) & 0xFF)`, bits 2 to 9 of the tile word (part of u, all of v and the page). No retail map has a tile on page 3 (0 of 97 x 16,384 words), so in practice 0x300..0x3BF | `bg_first_draw` (`0x80014058`) |
| 0x1FF..0x2FF | sprites of class 0x2000 and 0x4000 at `0x200 + key`; the overlay copy of a bit-13 tile | `sprite_draw` (`0x8001A528`); `bg_first_draw` |
| 0x100..0x13F | the second layer in modes 3 (opaque), 1 (half and half) and 5 (added) | `bg_second_draw` |
| 0x40 | sprites of class 0; the flat layer's added quad (`layer_flat_draw` (`0x8001DEF4`), which puts it at 0x300 in a battle) | `sprite_draw`; `layer_col_fade_step` (`0x8001E7B0`) |
| 0x20..0x3F | sprites of class 0x6000: `0x20 + (0xFF - screen y) / 8` | `sprite_draw` |

What shows through a tile's hole is therefore the second layer when its
mode is 0, else the clear.

### Transparency: colour index 0, and nothing else

A texel is not drawn when its palette colour is the word 0.
`resource_palette_load` (`0x80020A08`, resource_command.c) does not load a
map's stored palette as it is. For sub-number 0, 512 colours, it first
passes it through `palette_zero_fix(buf, 0x200, 0xFF)` (`0x80020830`,
palette_row.c): colour 0 of each 256-colour row becomes 0, and every
other colour stored as 0 becomes 1, an opaque black. `0x001F` is a plain
red. From that buffer five rows of video memory are loaded (`LoadImage` at
y = row + 500, `palette_rows_load` (`0x80020588`), palette_row.c):

| VRAM row | made by | used by |
|---|---|---|
| 500, 501 | the two halves (`palette_rows_set(0, 2, buf)` (`0x8002061C`), which also keeps them in the palette buffer that `palette_update` (`0x80020134`) sends again) | the tile layer: palette word `g_BgCluts[0]` / `[2]` for tile word bit 12 clear / set |
| 508 | the second half with bit 15 set on every colour and colour 0 of every 16 made 0 (`palette_bg_second_row_set(8, 1, buf)` (`0x80020670`)) | the second layer: sixteen 16-colour palettes, palette word `0x7F00 + n` |
| 510, 511 | the two halves with the first `g_PaletteBgCoverCount` colours of every 16 made 0 (`palette_bg_cover_rows_set(0, buf)` (`0x800206D4`)), to the rectangle `g_PaletteBgCoverRectangles[0]`, (0, 510, 256, 2) | the overlay copy: `g_BgCluts[1]` / `[3]` |

A palette command with another sub-number carries 256 colours, and
`palette_bit15_set` (`0x800207E8`) makes colour 0 of them 0 and sets bit
15 on every other: below 0x80 they load row 506, at 0x80 row 501, and
above 0x80 nowhere.

`g_PaletteBgCoverCount` is 10, or 8 while the map's party kind
`g_MapPartyKind` is 0 (`MapLoad` (`0x80068A10`), map_load.c, calls
`palette_bg_cover_count_set` (`0x80020DB4`) before the map's resources are
loaded). Two of the 97 maps (9 and 43) store only 256 colours: the second
half is then what the inflate buffer `g_LoadBuffer` held before (the page
and palette commands share it), which is not read.

### Bit 13 and the overlay-depth grid: the roof a sprite walks behind

A tile whose word has bit 13 is queued a second time by `bg_first_draw`
(bg_tile.c): same texels, page and mirror, the palette pair's other entry
(rows 510 / 511), at the entry the overlay-depth grid holds for its
16 x 16 screen cell, and not at all while that is 0x1000 or more. The grid
(§"The overlay-depth grid") is reset to 0x1000 at the head of the sprite
pass and lowered, under every piece of every entry whose draw word has
`& 0x6000 == 0x2000`, to `clamp(entry, 0x200, 0x300) - 1`
(`sprite_piece_draw` (`0x80019DBC`), sprite_list.c → `bg_cover_box_set`
(`0x80013EF0`)). So the copy sits one entry in front of the frontmost
such sprite over its cell, and only there. A map with nothing standing on
it draws no overlay at all.

Through rows 510 / 511 the copy shows only the texels whose colour index
has a low nibble of `g_PaletteBgCoverCount` or more. A tile's art is thus
split by colour index: in Vane's palette each group of 16 is 4 ground
colours, 6 colours, and the same 6 again; the second six are the part
that goes in front. A sprite of class 0x4000 raises no copy and is drawn
over every roof. The class is `MapAttributeClassGet` (`0x8006960C`,
map_over.c)'s answer for where the sprite stands.

### A sprite's entry

`sprite_draw` (sprite_list.c) gives the key `0xFF - (depth - scroll y +
0x20)`, the depth being the entry's ground y (+4), held to 0..0xFF; entries 0x46..0x6D (the twins, a person's shadow)
take 0xFF and entries from 0x6E up take 0, wherever they stand. The
ordering-table entry is `0x200 + key` for classes 0x2000 and 0x4000. One
pixel lower on the screen is one entry lower: in front. x, the drawn y
and the picture take no part. The pass queues entries 0x46..0x6D first,
then 0..0x45, then 0x6E..0x95 (`sprite_list_draw`), so of two sprites at
one entry the earlier group is in front. The rest is in
[../../sprites.md](../../sprites.md) §"The sprite list and `sprite_draw`".

The colour is entry `(word >> 15) | ((attr >> 14) & 2)` of `g_SpriteCols`:
0x808080, 0x404040, 0, 0 in the image. A map object whose definition's
mode byte is 3 gets the draw word 0x2000 and attribute bit 0x8000
(`MapRecordInit` (`0x80065940`)), so it is drawn black and
semi-transparent.

### The second layer

The type-7 block: 128 rows of 64 `s16`, 8 x 8 px, wrapping every
512 x 1024 px; `-1` is empty ([../../graphics.md](../../graphics.md)
§"The second tile layer (type 7)" has the cell word). `bg_second_draw`
walks a row in pairs from column `(x >> 4) * 2`: a pair whose first cell
has 0x200 is one sprite 16 px wide, or 32 when the next cell is not 0,
and the walk skips the 2 or 4 columns it covers. In the retail maps a
32-px cell is always `head, 1, head + 16 px, 0` (44,873 of them, none at
column 62): its second pair is itself the 16-px sprite of its right half,
so the picture does not depend on where the walk starts. The texels are
texture page 0x1F, where the tile layer's page 3 lies, read as 4-bit;
the palettes are row 508.

Its mode and scroll are the map record's bytes +0xB..+0xE (`MapEffectSet`
(`0x80068AC0`), map_load.c):

| | |
|---|---|
| flag 0x40 | mode 0: behind the tiles, and `g_BgMode = 1`, which puts the semi-transparency bit on the tile layer's colour word (`layer_col_apply` (`0x8001E10C`), layer_col.c): a tile texel whose colour has bit 15 is then half and half over it |
| flags 0x20 / 0x30 | a level L from +0xE's low nibble (x 2; 0xF → 0x1F): 0xE..0x12 → mode 1 (half and half), any other 1..0x1F → mode 5 (added: texture page word 0x3F); in front of everything |
| neither | mode 3: in front of everything, opaque |
| flags & 3 | the scroll: 1 = the camera, one for one; 2 = the camera / 2 with flag 0x40, x 2 without; 3 = drifting by +0xC's two nibbles through `g_MapSecondViewSpeeds` (-8 -4 -2 -1 0 1 2 4 8) and following the camera's movement; 0 = undefined once the camera moves (`MapViewScrollSet` (`0x80068E98`), map_view.c, stores a stack pair nothing set; [field-frame.md](field-frame.md) §"Two paths with no defined behaviour") |

By mode and scroll: 0 / 1: 15 maps, 0 / 2: 11, 0 / 3: 6; 1 / 1: 2,
1 / 3: 5; 3 / 0: 1 (map 151), 3 / 1: 1, 3 / 2: 1, 3 / 3: 3; 5 / 1: 13,
5 / 2: 3, 5 / 3: 12; no cell at all: 24 (Vane among them).

In modes 1 and 5 the level weighs the two layers' colour words
(`layer_mix_weights_get` (`0x8001E08C`) and `layer_col_apply`, layer_col.c):
tiles x L / 16, second layer x (0x20 - L) / 16, both halved first in mode
5 ([field-frame.md](field-frame.md) §"Screen fades and scene
transitions"). Map 24 (L = 0x16): tiles 0x58, layer 0x28. Map 23 (mode 1,
L = 0xE) is the one map whose layer word is above neutral at rest: tiles
0x70, layer 0x90.

### Not established

* Hardware: how the GPU steps between a mirrored quad's corner texels
  (they run cell + 15 back to cell - 1, held at 0 / 0xFF at a page's edge:
  2,096 of the 87,373 mirrored tiles have a plain axis held at 0xFF, where
  a straight interpolation would not show the plain run), that a sprite's
  texel column wraps in its page, dither, the 5-bit frame buffer.
* The second half of a 256-colour palette's buffer (maps 9, 43).
* What the palette and tile-cell animations change (`palette_update`,
  `palette_animation_step` (`0x800263F4`), `tile_animation_step`
  (`0x80026690`)). `palette_update` sends only rows 500 to 507 and the copy
  into row 508, so the overlay copy's rows 510 / 511 never move; the tile
  cells it shares with the base layer do.

## The tile layer: `bg_first_draw` (`0x80014058`)

`bg_draw` (`0x800145EC`, bg_tile.c), called from map_frame.c and
start_screen.c, draws three things a frame: `bg_first_draw(g_BgScrollX,
g_BgScrollY)`, `bg_second_draw(g_BgSecondScrollX, g_BgSecondScrollY)` and
the text grid (`ankprint_draw(0x10)`). `bg_scroll_set` (`0x800145BC`) sets
the first scroll and its low four bits, `g_BgScrollXLowBits` and
`g_BgScrollYLowBits`; `bg_second_scroll_set` (`0x800145DC`) sets the
second.

`bg_first_draw(x, y)` queues the tiles for the view whose top-left map
pixel is (x, y): 16 x 16 cells from (-(x & 15), -(y & 15)) while the
screen x is below 0x141 and the screen y below 0xE0, so 21 columns by 14
or 15 rows. Cells off the 128 x 128 map draw word 0. For each cell:

```
tp   = (tile & 0x1300) >> 8          # bits 0-1 = page index, bit 4 = palette pair
base layer, unless g_BgFirstHidden != 0:
    depth = ((tile >> 2) & 0xFF) + 0x300
    clut  = g_BgCluts[tp >> 3]       # entry 0 or 2
    tpage = g_BgTPages[tp & 7]
if tile & 0x2000:                    # the overlay copy
    depth = g_BgCoverGrid[cell]      # one s16 per screen cell, row-major
    if depth < 0x1000:               # 0x1000 or more hides it
        clut = g_BgCluts[(tp | 8) >> 3]   # entry 1 or 3
        the same texels, mirror and page, drawn again at that depth
```

The colour word comes from the scratchpad word `0x1F800044`. The copy's
depth is used as the entry itself (no + 0x300): the base layer keeps to
entries 0x300..0x3FF, the copy can go anywhere below 0x1000.
`g_BgFirstHidden` hides the base layer and leaves the copy; it is 0 in
the image and nothing writes it.

### 16-bit tile value format

```
Bit 15:      mirror in y   (tile & 0x8000)
Bit 14:      mirror in x   (tile & 0x4000)
Bit 13:      overlay copy  (tile & 0x2000: the tile is drawn a second time)
Bit 12:      palette pair  →  g_BgCluts[((tile >> 12) & 1) * 2], + 1 for the overlay copy
Bits 11-10:  unused
Bits 9-8:    page index    →  g_BgTPages[(tile >> 8) & 3]
Bits 7-4:    tile row      (v = tile & 0xF0)
Bits 3-0:    tile column   (u = (tile & 0xF) * 16)

Bits 9-2 also give the draw depth:
  entry = ((tile >> 2) & 0xFF) + 0x300
```

So the draw depth comes from the tile's own texture place; there is no
separate priority field.

### The texture-page and palette tables: `g_BgTPages`, `g_BgCluts`

Both are fixed data in the image, read as 16-bit words.

| table | index | values | meaning |
|---|---|---|---|
| `g_BgTPages` | `(tile >> 8) & 3` | `0x0099 0x009B 0x009D 0x009F` | 8-bit texels, blend mode 0, VRAM y 256, x 576 / 704 / 832 / 960 |
| `g_BgCluts`, base | `((tile >> 12) & 1) * 2` | `0x7D00`, `0x7D40` | VRAM x 0, rows 500 and 501 |
| `g_BgCluts`, overlay copy | `((tile >> 12) & 1) * 2 + 1` | `0x7F80`, `0x7FC0` | VRAM x 0, rows 510 and 511 |

The step of 128 VRAM words between pages is one 256-texel-wide 8-bit
sheet. The two base rows are the two halves of the map's 512-colour
palette: the first half for bit 12 clear, the second for bit 12 set. A
map's 256-colour palette is not used for the tiles.

### Primitive selection

```
if (tile & 0xC000) == 0   → a 16 x 16 sprite (code 0x7C) behind its own draw-mode word (tpage | 0xE1000200)
else                      → a textured quad (code 0x2C), the texel run turned per mirror bit
```

A mirrored edge runs from `u + 15` back to `u - 1`, held at 0 below and
at 0xFF from 0x100; that is why mirrored tiles are quads and not sprites.

### The page cache and its fill: `bg_block_load` (`0x80014F3C`)

The primary map is 128 x 128 u16 tile words in blocks of 8 x 8, a 16 x 16
grid of blocks (the map's tile layout, [../../graphics.md](../../graphics.md)
§"The tile layout", copied to `g_BgDataBuffer`). Sixteen blocks are kept
unpacked at once, in the sixteen 64-word slots of `tile_lookup_table`
(`0x800DB2C0`, 0x800 bytes), a 4 x 4 grid. `g_BgSlotBlocks`
(`0x1F800000`, scratchpad) holds the block number each slot holds. For
cell (tx, ty):

```
block = (tx >> 3) + ((ty >> 3) << 4)
slot  = ((tx >> 3) & 3) | ((ty >> 1) & 0xC)
if g_BgSlotBlocks[slot] != block: bg_block_load(tx, ty)
word  = tile_lookup_table[(tx & 7) + ((ty & 7) << 3) + (slot << 6)]
```

`bg_first_draw` and `bg_copy` read the map this way and are the two
callers of `bg_block_load`, which:

1. returns 0 when `tx >= 0x80 || ty >= 0x80` (unsigned), the only path
   with a value;
2. names the slot first, `g_BgSlotBlocks[slot] = block`, then fills its 64
   words by the block's offset `off`, the u16 at entry `block` of
   `g_BgBlockEntries`:

   | `off` | the slot's 64 words |
   |---|---|
   | 0 | zero |
   | below `g_BgBlockFillLimit` (signed compare) | 64 copies of the one u16 at `(off & 0xFFFE) + g_BgBlockEntries` |
   | `g_BgBlockPlainLimit` or more | 64 plain u16s copied from `g_BgBlockBase + off`; when `off` is odd, `hex_dump(g_BgBlockEntries, 0x210)` first |
   | otherwise | `lz_unpack(g_BgBlockBase + off, dst)` (`0x80022BF8`) |

`g_BgBlockFillLimit` and `g_BgBlockPlainLimit` are the layout's own words
at +0x200 and +0x202, the `fill_limit` and `plain_limit` of `BgBlockTable`
(bg.h); `bg_data_reset` (`0x80014E60`) copies them, sets the write cursor
`g_BgNextBlockEntry` to `plain_limit` rounded up to even, and sets every
slot to -1. `bg_data_init` (`0x80014EE8`) points both `g_BgBlockEntries`
(the table) and `g_BgBlockBase` (the base the offsets count from) at
`g_BgDataBuffer`, so the two are the same address.

`bg_attribute_block_load` (`0x800154F8`, bg_tile.c) is the same function
for the attribute plane: 256 x 256 cells, 16 x 16-cell blocks, 256-word
slots at `secondary_tile_table`, slot names at `g_BgAttributeSlotBlocks`,
table and base `g_BgAttributeBlockEntries` / `g_BgAttributeBlockBase`,
limits `g_BgAttributeBlockFillLimit` / `g_BgAttributeBlockPlainLimit`
(set by `bg_attribute_data_reset` (`0x8001543C`)).

### `hex_dump` (`0x80021510`): a hex dump with its print calls removed

`void hex_dump(u8 *p, s32 n)` (cdread.c) stores each of the `n` bytes,
widened to an int, in a sixteen-int row on its own stack. When the row is
full it walks the sixteen values, putting 0x2E (`.`) for each one below
0x20, and starts the row again; a last row of fewer than sixteen is never
walked. It calls nothing, touches no global and returns nothing: in this
build it has no effect. One test of the original is lost: the code forms
`.` in a register that nothing reads, which the compiler leaves only when
a second test, an upper bound, read the character after the first. The
C's bound `0x7F` is not recoverable from the bytes.

Its callers: `bg_block_load` and `bg_attribute_block_load` on an odd
plain-block offset (`g_BgBlockEntries` or `g_BgAttributeBlockEntries`,
0x210 bytes: the 256 offsets, the two limit words and twelve bytes after
them), and `MessageStringDisp` (`0x8005AB98`, message_disp.c) on entry,
with its string, 0x10 bytes. The odd offset is a "should not happen"
check that dumps to nowhere; [../../graphics.md](../../graphics.md) notes
that no map has one.

## Run-time tile writes: `bg_copy` and the two cell writes

A map changes its own tiles while it is shown by copying a rectangle of
its cells from another place in the same map, in both planes:

```
bg_both_copy(dx, dy, sx, sy, w, h):          (0x800153A0)
    bg_copy(dx >> 1, dy >> 1, sx >> 1, sy >> 1, w >> 1, h >> 1);   primary map, 16-px tiles
    bg_attribute_copy(dx, dy, sx, sy, w, h);                         attribute plane, 8-px cells
```

The arguments are in the attribute plane's 8-pixel cells, and the tile
half gets them halved. `bg_copy` (`0x80015224`) is the developers' own
name: it prints `"bg_copy(%d,%d,%d,%d,%d,%d)\n"` (`0x80011038`) with its
six arguments on entry, through `debug_printf`, which prints nothing. The
one caller of `bg_both_copy` is `MapBgChangeCopy` (`0x80059B0C`,
map_change.c), which the frame's end `MapFrameCount` (`0x800566AC`,
map_step.c) calls when the running change asks for a copy, and
`MapBgChangeStoryBitsCopy` (`0x80059B6C`) for every change whose story
bit is set; the record, its table and the frame's order are in
[field-frame.md](field-frame.md) §"The map's tile animation".

`bg_copy` goes rows top to bottom, cells left to right, one cell at a
time: it reads the source word and calls `bg_tile_set(dx + i, dy + j,
word)`. `bg_attribute_copy` (`0x800157DC`) does the same on the attribute
plane with `bg_attribute_block_load` and `bg_attribute_set`.

* A source cell outside the map (either coordinate outside 0..0x7F, by
  unsigned compares) is not skipped: it reads as word 0 and that 0 is
  written. The attribute half does the same with its bound 0x100.
* A source cell inside is read through the page cache, as `bg_first_draw`
  reads it.
* A destination outside the map writes nothing (`bg_tile_set` returns 0).
* Overlap is not handled. The order is always increasing x, then
  increasing y, and every write marks its cache slot stale, so a
  destination ahead of its source (to the right on the same rows, or
  below) reads again cells this same call has already overwritten: a
  forward copy, not a memmove.
* `w <= 0` or `h <= 0` copies nothing.

The two cell writes, `bg_tile_set` (`0x8001508C`) and `bg_attribute_set`
(`0x80015644`), are twins: every difference is a constant or a symbol.
Each sets cell (x, y) to `val` with copy-on-write of the cell's block,
by the read side's block rules:

1. A cell outside the map returns 0.
2. The block's offset is read from the u16[256] table. An offset at or
   past the plain limit is already a plain block: the word is written in
   place.
3. Otherwise a new plain block is built at `base + cursor`: zeros for
   offset 0, the one word at `(off & 0xFFFE) + table` repeated for a fill
   offset (below the fill limit), `lz_unpack(base + off, block)`
   otherwise. The table entry is pointed at the cursor and the cursor
   moves on one block. Nothing bounds the cursor.
4. The word is stored and the cell's slot in the read cache is set to -1,
   so the next read fetches the block again.

| | `bg_tile_set` | `bg_attribute_set` |
|---|---|---|
| map | primary: 128 x 128 tiles, 8 x 8-tile blocks | attribute plane: 256 x 256 cells, 16 x 16-cell blocks |
| table / base | `g_BgBlockEntries` / `g_BgBlockBase` | `g_BgAttributeBlockEntries` / `g_BgAttributeBlockBase` |
| fill limit / plain limit / cursor | `g_BgBlockFillLimit` / `g_BgBlockPlainLimit` / `g_BgNextBlockEntry` | `g_BgAttributeBlockFillLimit` / `g_BgAttributeBlockPlainLimit` / `g_BgAttributeNextBlockEntry` |
| block (cursor step) | 64 words (0x80 bytes) | 256 words (0x200 bytes) |
| cache marked stale | `g_BgSlotBlocks`, slot `((x >> 3) & 3) \| ((y >> 1) & 0xC)` | `g_BgAttributeSlotBlocks`, the quadrant slot |
| only caller | `bg_copy` | `bg_attribute_copy` |

Neither touches the GPU. A cell inside the map ends with no return value;
the callers ignore it.

## The overlay-depth grid: `bg_cover_reset` (`0x80013EB8`) and `bg_cover_box_set` (`0x80013EF0`)

`g_BgCoverGrid` (`0x800DB048`) is one s16 for each 16 x 16 screen cell,
21 to a row, 15 rows, row-major: the depth `bg_first_draw` gives a bit-13
tile's overlay copy. The sprite pass builds it again each frame (both
functions in bg_cover.c):

* Reset: `bg_cover_reset()` fills all 15 x 21 cells with 0x1000, the
  value `bg_first_draw` treats as hidden. Its caller is `sprite_list_draw`,
  before it walks the sprite list.
* Lower: `bg_cover_box_set(x, y, w, h, depth)` takes a sprite piece's box
  in screen pixels. It adds the view's sub-tile scroll
  (`g_BgScrollXLowBits`, `g_BgScrollYLowBits`), turns the corners into
  cells, `(x >> 4, y >> 4)` to `((x + w) >> 4, (y + h) >> 4)`, each held to
  0..21 across and 0..14 down, holds `depth` to 0x200..0x300, and writes
  `depth - 1` into every cell of the box that holds a larger value.
* Caller: only `sprite_piece_draw`, after its draw switch, when
  `g_BtlRunning == 0` and the entry's draw word has
  `& 0x6000 == 0x2000`. It passes the piece's place and size and the
  entry the piece was drawn at.

So the overlay copy of a tile appears only in cells a flagged sprite
covers, at the entry one below the frontmost such sprite there.

The clamp admits column 21, and a row is 21 cells, so column 21 is the
next row's first cell. On row 14 that is index 315, the halfword just
past the grid's 0x276 bytes: the two bytes left before
`tile_lookup_table` (`0x800DB2C0`). `bg_first_draw` never reads index
315.

## The second tile layer: `bg_second_draw` (`0x800148E0`)

`bg_second_draw(x, y)` draws a layer of 8 x 8-pixel cells on its own
scroll, from `g_BgSecondBuffer` (the map's type-7 block, 128 rows of 64
`s16`). The cell word and the four modes are in
[../../graphics.md](../../graphics.md) §"The second tile layer (type 7)";
what the modes look like is in §"The second layer" above. The mode is
`layer_mix_mode_get()` (`0x8001DFE8`, layer_col.c):

| second record's mix | mode | entries | packet colour and page |
|---|---|---|---|
| 0xE..0x12 | 1 | 0x100 + | semi-transparent |
| any other 1..0x1F | 5 | 0x100 + | semi-transparent, texture page 0x3F (added) |
| 0, first record's order below the second's | 3 | 0x100 + | opaque |
| 0, otherwise | 0 | 0x3C0 + | opaque |

The entry adds `(cell & 0x1FF) >> 3`. The function's mode 4 returns at
once, but `layer_mix_mode_get` never answers 4. Before the cells it queues
a texture-page packet for page 0x1F at entry 0x100. A cell of -1 is
empty; a cell with 0x200 is a wide sprite, 16 or 32 px; the walk wraps at
64 columns and 128 rows; the palette word is `0x7F00 + n`, row 508. A
mirrored cell is a quad, as in §"Primitive selection".

When `g_BgMode & 0x80`, `bg_second_draw` puts the layer colour
`g_BgSecondCol` in `g_BgSecondPacketCol` (`0x1F800048`) and hands the
whole layer to `bg_second_screen_draw` instead.

### The fixed-screen path: `bg_second_screen_draw` (`0x80014D74`)

`StartResourceLoad` (`0x8004409C`, start_screen.c) sets `g_BgMode` to
-0x80 for the title screen's planets.

- Grid: 14 rows of 21 columns, a sprite at (col * 16, row * 16), no
  scroll, no layer mode.
- Cells: `g_BgSecondBuffer` read as rows of 32 `s16`, `cell = row * 32 +
  col` (the scrolled path reads the same block as 128 rows of 64).
- A cell of 0 is empty (the scrolled path uses -1). Only the low byte is
  read, as a tile number on a 16 x 16-tile page: `u = (n & 0xF) * 16`,
  `v = n & 0xF0`.
- Packet: one `PacketSprite` from `g_PacketWritePointer`, draw-mode word
  `0xE100029D` (texture page 0x9D = `g_BgTPages[2]`: VRAM (832, 256),
  8-bit texels, blend mode 0; 0x200 = dither), a sprite in the colour of
  `g_BgSecondPacketCol`, 16 x 16, palette word `0x7D40` (row 501, the
  second half of the map's palette).
- Linked by hand at entry 0x2FE of `g_PacketOtBase`; `AddPrim` is not
  called.

## The battle's tile layer: `tile_layer_draw` (`0x8001D4E0`)

`tile_layer_draw(x, y)` (tile_layer.c) draws one 16 x 16 tile-map layer at
the pixel scroll (x, y): 15 rows by columns 0..0x14, widened to -2..0x18
(after `tile_wave_strips_make`) when the wave is on (`g_TileWaveShown &&
g_TileWaveSize`). For each cell, `tile = tile_cell_get(col + (x >> 4), row
+ (y >> 4))` (`0x8001D4BC`); 0xFFFF is empty, drawn as tile 0 when
`g_TileEmptyFillOn` is set. u and v come from the low byte, the mirror
from `tile & 0xC000`, and the palette index

```
pal = (((g_TileTPageFirst - 0x19) / 2) + 4 + ((tile & 0x1FF) >> 8)) | 0x20
```

picks the palette word `g_TileCluts[pal >> 3]` and the texture page
`g_TileTPages[pal & 7]`. One of three packets goes to
`g_PacketWritePointer`, linked at entry 0x300:

* under the wave: up to three sheared quads from `g_TileWaveStrips[3]`
  (a strip with `row_count` 0 is skipped);
* a mirrored tile: one quad with the texel run turned and the far edge
  held;
* otherwise: a draw-mode word (`tpage | 0xE1000200`) and a 16 x 16 sprite.

`g_TileCol` is the colour word and `g_TileSemiTransOn` the
semi-transparency flag.

It is the battle presentations' background layer: `tile_layer_step`
(`0x8001DA78`) draws it while `g_TileLayerShown == 1` (the presentation's
flag 4 sets it), on the 32 x 32 map `tile_pattern_put` (`0x8001DB6C`)
fills from a layer resource's type-18 tile maps. There `g_TileTPageFirst`
is always 0x19, so pal = 0x24 + bit 8 of the cell: texture page
`g_TileTPages[4]` = 0x99 or `[5]` = 0x9B (8-bit, (576, 256) or (704,
256)) and palette word `g_TileCluts[4]` = 0x7D40 (row 501). Under the wave
a mirrored tile's strips are each turned in place (top v = v +
`first_row` + `row_count` - 1, bottom v = v + `first_row` - 1), not the
tile ([../../assets/battle-effects.md](../../assets/battle-effects.md)
§"Background layers (flag 4)").

### The wave's shear: `tile_wave_strips_make` (`0x8001D310`)

`tile_layer_draw` runs it once a draw while the wave is on. It builds no
packet and calls nothing; it fills the tables the drawer reads.

* The wave is the 16-entry triangle table `g_TileWaveTable` (0 1 2 3 4 3
  2 1 0 -1 -2 -3 -4 -3 -2 -1), with phase `g_TileWaveStartRow` and
  amplitude `g_TileWaveSize`, both set by `tile_wave_set(amplitude,
  phase)` (`0x8001D2F4`).
* For pixel row i = 0..15 of a tile, `v = g_TileWaveTable[(g_TileWaveStartRow
  + i) % 16]`, and the row's x offset is `g_TileWaveRowShifts[i] = (v *
  g_TileWaveSize + 2) >> 2`, an arithmetic shift, so negative offsets
  round down.
* The rows where v is 4 and -4 (`peak[0]`, `peak[1]`) split the tile into
  strips. The three `TileWaveStrip` records (`top_shift`, `bottom_shift`,
  `first_row`, `row_count`) all get `row_count` 0 first; then rows
  0..peak0 (only when peak0 is not 0), rows peak0..peak1, and rows
  peak1..16, whose bottom edge takes row 0's offset. Each strip carries
  the x offsets of its top and bottom edge; with peak0 = 0 only two are
  filled.

## Queued VRAM cell uploads: `g_TileAnimations`

The functions are in tile_animation.c and palette_animation.c.

* `tile_image_load(cell, src)` (`0x80026754`): one `LoadImage` of an
  8 x 16 block (0x100 bytes, 16 texels a line at 8 bits) from `src` to
  x = `((cell & 0x300) >> 1) + 0x240 + ((cell & 0xF) << 3)`,
  y = `(cell & 0xF0) + 0x100`: a cell of the tile layer's pages.
* `tile_image_run_load(cell, src, count)` (`0x800267AC`): `count` of
  those in a row, `cell + 1` and `src + 0x100` a step; a count of 0 or
  below sends nothing. Its only caller is `tile_animation_load`.
* `tile_animation_load` (`0x80026814`): the flush. Four 0x18-byte
  `TileAnimation` records at `g_TileAnimations`; where `pending` (+0x0B)
  is not 0 it clears it and calls `tile_image_run_load(tile, source,
  count)` (+0x10, +0x0C, +0x14).
* `tile_animation_set(id, unused, unused2)` (`0x800265BC`) starts set
  `id`: it clears `mode` in all four records, and for an id other than 0
  copies the set's 8-byte seed rows (the pair table
  `g_TileAnimationSets` at `[(id - 1) * 2]` = {first row, count}; the rows
  at `g_TileAnimationRows`) into records 0..count - 1 with timer, frame
  and pending 0, `mode` = the seed's byte 0, `tile` (+0x10) = the seed's
  u16 at +4 and `count` (+0x14) = its byte +3. `tile_animation_step`
  steps the records. Its one caller is `MapLoad`, which passes byte +4 of
  the map record (`MapTbl.tile_animation`), a variable it never sets
  (the original hands on `$s3` as it stood at entry) and the address of
  a stack word; only the first is read.
* `palette_animation_set(id)` (`0x800262FC`) is the same start for
  palette animation. It zeroes the three bytes of the row-508 copy
  request (`g_PaletteAnimationCopyId`, `g_PaletteAnimationCopyFirst`,
  `g_PaletteAnimationCopyCount`, which `palette_update` acts on every
  frame), stops the eight slots of `g_PaletteAnimations`, calls
  `palette_animation_copy_set(id)` (`0x8002628C`), which walks the
  `{id, first, count}` rows at `g_PaletteAnimationCopyRows` through the
  cursor `g_PaletteAnimationCopyRow` and copies a hit into the three
  bytes (a miss writes nothing), then copies the set's 8-byte seed rows
  (pair table `g_PaletteAnimationSets`, seeds `g_PaletteAnimationRows`)
  into the first slots with timer and frame 0 and mode = the seed's mode
  byte. `MapLoad` passes byte +5 of the map record
  (`MapTbl.palette_animation`).

## Sprites

The sprite list, the sprite pages and the draw word are in
[../../sprites.md](../../sprites.md). `sprite_init` (`0x80018BFC`) builds
the page table `g_SpritePageTPages` once, from the fixed corners
`g_SpritePageCorners`: slots 0 to 9 8-bit, 10 and 11 4-bit. It also fills
`D_800DDD50` with the page word 64 words to the right of slots 0 to 9,
and nothing in the C reads that table. Which picture goes to which slot
is set by the loaders (sprites.md §"The sprite pages").

## Packet helpers

### A string in the 8 x 8 debug font: `packet_string_add` (`0x80016B9C`)

`void packet_string_add(s16 x, s16 y, s8 *str, u16 ot)` (sprite_draw.c)
draws up to eight characters in the 8 x 8 debug font. Its one caller is
the debug page viewer `debug_sprite_page_show` (`0x80019044`,
sprite_list.c), as `packet_string_add(0x10, 0x10, buf.c, 0x10)` after
`debug_digits_put` (`0x80018FA4`) has put the page number into
`buf.c[2]` and the row number into `buf.c[6]`. `buf` is an 8-byte copy of
the template at `D_800B857C`; it has no terminator, which is what the cap
of eight is for.

For `i` = 0..7, until `str[i] == 0`, one `PacketSprite` from
`g_PacketWritePointer`:

- tag `0x05000000`; colour and code `0x64808080`, a sprite in neutral
  colour, written as one word;
- w = 8, h = 8;
- palette word `GetClut(0xE0, 0x1FB)` (`0x8008E9B8`): the 16-colour
  palette at VRAM (0xE0, 0x1FB);
- draw-mode word `g_SpritePageTPages[0xB] | 0xE1000200`: page slot 11, a
  4-bit page;
- x = `x + i * 8`, y = `y`;
- u = `((c - 0x20) & 0x1F) * 8`, v = `0xC0 + (c - 0x20) / 31 * 8`, with
  `c = str[i]` read as a signed byte;

linked at entry `ot` with `AddPrim`, the cursor moved on 0x18 bytes.

So the font is 8 x 8 glyphs on page slot 11, 32 a row from texel row
0xC0, the first glyph character 0x20. The row is a division by 31, not
32, while the column is `& 0x1F`: characters 0x3F, 0x5E, 0x5F and
0x7D..0x7F take their glyph from one row further down than their column
implies (0x3F: column 31, row 1 instead of 0). The viewer's template and
digits use none of those characters, so it never shows. A byte of 0x80
or more gives a negative `c - 0x20`: `& 0x1F` still gives a column, and
the signed division a negative row, a v below 0xC0. The packet is the one
`sprite_plain_draw` (`0x80016CF0`) builds, with the stores in the same
order, but with no colour-table lookup, no tint and no
semi-transparency.

### One flat triangle: `packet_triangle_down_add` (`0x80015BF8`)

`void packet_triangle_down_add(s32 x, s32 y, s32 w, s32 h, s32 color, s32
ot)` (packet_shape.c) queues one flat triangle with a level top edge:
(x, y), (x + 2w, y), (x + w, y + h). Its one caller is the message
window's "more" arrow, `MessageArrowDraw` (`0x80015F0C`, message_draw.c):
w 4, h 5, colour 0x20F0F0, entry 0x10, the y bobbing with
`g_MessageArrowPhase`. The packet is a `POLY_F3` (0x14 bytes) from
`packet_triangle_take` (`0x80015A20`), linked at entry `ot` with
`AddPrim`.

| offset | field | value |
|---|---|---|
| +0x00 | tag (u32) | 0x04000000: 4 words, no link yet |
| +0x04 | r0 g0 b0 code (u32) | `color \| 0x20000000`: colour 0x00BBGGRR, code 0x20, a flat triangle |
| +0x08 / +0x0A | x0, y0 (s16) | x, y |
| +0x0C / +0x0E | x1, y1 | x + 2w, y |
| +0x10 / +0x12 | x2, y2 | x + w, y + h |

The length byte (+3 = 4) and the code byte (+7 = 0x20) are first stored
on their own, as `setPolyF3` does, then written over by the two word
stores. The colour is ORed in, so a top byte in `color` would reach the
code byte (bit 25 = semi-transparent); the one caller passes 24 bits.
`packet_triangle_right_add` (`0x80015CB8`) is the same packet turned a
quarter: (x, y), (x, y + 2h), (x + w, y + h). Nothing calls it, in the
main executable or in the MENU overlay.

### Lines, and the label-cell screen's lattice

`packet_line_add(x0, y0, x1, y1, colour0, colour1, ot)` (`0x80015D78`,
packet_shape.c) is the line packet: a 0x14-byte `LINE_G2` from
`packet_line_take` (`0x80015A40`), tag 0x04000000, `colour0 | 0x50000000`
at +4, x0 / y0 at +8 / +0xA, colour1 at +0xC, x1 / y1 at +0x10 / +0x12,
linked with `AddPrim` at `g_PacketOtBase + ot * 4`. Its callers are
font_cell.c and outline.c.

`font_lines_lattice_draw()` (`0x80016738`, font_cell.c) draws the grid
lines of `font_lines_view` (`0x800167F4`)'s screen, once a frame. That
screen keeps the 40 x 28 text grid (`ankprint_grid_get` (`0x80016668`)),
clears it and writes eight runs of sixteen label cells,
`font_cell_run_set(2, 2r + 2, 16, r * 16)` (`0x80016230`) for r = 0..7: a
16 x 8 block of 16 x 16-pixel cells whose top-left pixel is (16, 16).
Then, until `g_PadPressedButtons` is not 0, it draws each frame a
backdrop (`packet_box_col4_add(0, 0, 320, 224, 0x600000, 0x600000,
0x200000, 0x200000, 0x300)` (`0x80015B14`)), the lattice and the cells
(`ankprint_draw(0x10)` (`0x80025CEC`)), one `frame_step` (`0x8001F458`) a
frame. It then runs ten more frames and puts the kept grid back
(`ankprint_grid_set` (`0x800166D0`)).

The lattice is 26 lines: nine across, (16, y) to (272, y) for y = 16, 32
.. 144, then seventeen down, (x, 16) to (x, 144) for x = 16, 32 .. 272.
Colour 0x606060 at both ends, entry 0x2FE, so in front of the backdrop
at 0x300.

## Functions nothing calls

These are in the image with no way to reach them: no `jal` or `j`, no
data word, and no `lui` + `addiu` / `ori` pair forming the address, in the
main executable or in MENU.BIN, the only overlay file. (A raw-byte scan
of LUNADATA.FIL and the boot files of both discs finds nothing either;
that scan does not unpack packed payloads.) What each would do:

| function | would do | the live code beside it |
|---|---|---|
| `bg_tile_draw` (`0x800146B4`, bg_tile.c) | one unmirrored map tile as a sprite | `bg_first_draw`'s own sprite block |
| `bg_tile_mirror_draw(x, y, tp, u, v, z, flip)` (`0x80014770`, bg_tile.c) | one mirrored map tile as a quad over the 16 x 16 square at (x, y), at entry z: palette `g_BgCluts[tp >> 3]`, page `g_BgTPages[tp & 7]`, colour word from the scratchpad word `0x1F800044`. `flip & 0x4000` / `0x8000` mirror in x / y; a mirrored edge runs from cell + 15 back to cell - 1 (not below 0), an unmirrored one from cell to cell + 16 (0xFF at 0x100) | `bg_first_draw`'s own quad block (§"Primitive selection") |
| `packet_outline_rgb_add(x, y, w, h, r, g, b, semi, ot)` (`0x8001AA38`, sprite_list.c) | a flat-coloured rectangle outline in two `LINE_F3` packets, semi-transparent when `semi` is not 0, at entry `ot`, the colour passed as three bytes. The body proves the widths (16-bit places, 8-bit colours, u16 `ot`), not the signs | `sprite_piece_outline_draw` (`0x800197CC`), which takes the colour from the piece ([battle.md](battle.md) §"The actor renderer — how a pose is drawn") |
| `packet_box_rgb_add(x, y, w, h, r, g, b, semi, ot)` (`0x8001A910`, sprite_list.c) | a flat-coloured filled rectangle: one `TILE`, and with `semi` a `DR_TPAGE` after it at the same entry; `packet_outline_rgb_add`'s parameter list over `sprite_piece_box_draw` (`0x80019668`)'s body. Widths proven, signs not | `sprite_piece_box_draw` |
| `sprite_piece_box_opaque_draw(x, y, frame, unused, ot)` (`0x8001996C`, sprite_list.c) | a flat-coloured filled rectangle whose size comes from the piece (+0x10 / +0x12, no + 2), colour from the word at +6 or the palette buffer at `0x801D5B80`; no semi-transparency. Its fourth argument is never read | `sprite_piece_box_draw`: it reads like an earlier version of the kind-0x4000 drawer |
| `packet_triangle_add(x0, y0, x1, y1, x2, y2, r, g, b, ot)` (`0x80016AB4`, sprite_draw.c) | a flat-coloured filled triangle with three free corners: one 0x14-byte `POLY_F3` from `g_PacketWritePointer`, `SetPolyF3` (`0x8008EA48`) (length 4 at +3, code 0x20 at +7), colour bytes at +4 / +5 / +6, corners as s16 pairs at +8, +0xC, +0x10, linked at entry `ot` with `AddPrim`. No semi-transparency. Widths proven (16-bit places, u16 `ot`), signs not; the colours may be bytes or words | `packet_triangle_down_add` / `packet_triangle_right_add`: a fixed shape from (x, y, w, h), the colour as one word, the packet from `packet_triangle_take` |
| `sound_seq_rotate_play(bank, k)` (`0x80023CEC`, sound.c) | plays a sequence on the rotating slot: `sound_seq_play(bank, k, -1)` (`0x80023B24`) | `sound_music_play` (`0x80023D0C`) and `sound_effect_play` (`0x80024514`), which name a slot ([audio.md](audio.md)) |
| `packet_sprite8_add(x, y, u, v, clut, z)` (`0x800257EC`, packet_sprite.c) | one 8 x 8 textured sprite: an `SPRT_8` (0x10 bytes) from `g_PacketWritePointer` with no draw-mode word, linked at entry z with `AddPrim` ([io-hardware.md](io-hardware.md) §"An 8 x 8 sprite nothing draws") | `packet_sprite_add` (`0x800256C0`), the text grid's glyph packet: a sprite with w = h = 8 behind its own draw-mode word, linked by hand |
| `MapObjectDefinitionSpotFind(sprite_data, &x, &y, &kind)` (`0x800684FC`, map_load.c) | finds the first of the 0x80 map object definitions whose `sprite_data` is the argument: its cell x and y and its kind (`(byte & 0x3F) + 1`), or x and y -0x80 when its story bit is set (bit + 0x580 for a kind below 4, else 0x680 + its number + (map - 0x65) * 0x80). The Japanese build's twin is uncalled too | `MapObjectsInit` (`0x800655C8`)'s spawn loop, used as a query ([npc-scripting.md](npc-scripting.md)) |
| `sprite_page_image_send(id, unused)` (`0x80017D50`, sprite_draw.c), called only by `MapPartyImagesSend` (`0x80064084`, map_sprite.c), itself uncalled | puts a picture of a sprite data directory into page slot 5 or 6. It reads the directory in `g_ResourceReadBuffer` as 16-byte rows (+8 a u32 byte offset, +0xC a u32 size, 0x100 bytes a line); places it as `sprite_page_image_add` (`0x8001808C`) does (n = (size / 256 + 7) / 8 eight-line units in `g_SpritePageUseMap`, slot 5 then 6, slot 7's corner regardless when neither fits); inflates it with `inflate_buffer` (`0x80021D6C`) into `g_LoadBuffer` and sends it at once with `LoadImage` to `g_SpritePageCorners[5 + column] + (0, row * 8)`; marks the units id + 1 | `sprite_page_image_add`, `sprite_page4_image_load` (`0x80017EB0`) and `MapPartySpriteDataLoad`, which read the same directory (resource 0x1E8) as 8-byte rows with u16 fields, 64 bytes a line, and fix up the texel records and take `id == -1`, which this one does not |

Read together: the first two are the tile draw as it stood before it was
written into `bg_first_draw`; the next four are outline and fill helpers
with a direct colour or an earlier argument list; `sound_seq_rotate_play`
is a play call nothing uses and `packet_sprite8_add` a sprite packet the
text grid does not go through; the last is a picture loader for a wider
directory format than the one the disc carries. That reading is an
inference from the likenesses: nothing in the image says when they went
out of use.

Four more have no caller in the main executable (no `jal`, no `j` and no
data word for the address; MENU.BIN and the disc files were not scanned
for them):

* `IconSquareSet(x, y, cells, icon, attr)` (`0x80049EAC`, btl_window.c): a
  3 x 3 icon put into a caller's grid of 32-bit cells
  ([menu-ui.md](menu-ui.md) §"An icon put nothing calls"). For this one
  the image was also searched for an `addiu` / `ori` with the address's
  low half.
* `printmes(x, y, fmt, ...)` (`0x80025AC4`, ankprint.c): ANKPRINT.C's
  printf onto the text grid
  ([io-hardware.md](io-hardware.md) §"The printf onto the cell grid").
* `sim_console_write(s)` (`0x80021740`) and `func_800217AC(s)` (fmt.c):
  the two writes to the development host's console
  ([io-hardware.md](io-hardware.md) §"The development host's file read and console").

## The block request `g_MemRequest`: posted, never served

`func_80049AC4` (btl_window.c), called once at the top of `BtlMenuOpen`
(`0x800445B8`), copies the u16 at `g_PaletteBuf + 0xFC0` to `g_PaletteBuf
+ 0x202` (colour 0x7E0 of the palette buffer to colour 0x101), fills the
0x14-byte `MemRequest` at `g_MemRequest` (`0x800E4750`, mem.h) with
`kind` = 2, `source_fill` = 0x102C00, `destination` = 0x25E74000,
`row_words` = 0x20 and `rows` = 0x20, and runs `BtlWindowsMoveWait()`
(`0x80048A3C`).

The block's only other user is `mem_request_run` (`0x80054B30`,
mem_request.c). For `kind` 1 it copies `rows` rows of `row_words` words
from `source_fill` to `destination`, the row pitch 0x20 words for both;
for any other `kind` but 0 it fills 0x400 words at `destination` with the
word `source_fill`; then it sets `kind` to 0. Its one caller is
`mem_request_run_copy` (`0x80054B08`), which calls it and does nothing
else, and nothing in the image reaches that (no `jal`, no `j`, no address
word, no `lui` / `addiu` pair). So the request is posted and, as far as
the image shows, never served. Code loaded from outside the main
executable was not checked.

## The screen-mode set-up: `screen_mode_set` (`0x8001F0C8`)

`void screen_mode_set(s32 mode)` (frame.c) has one call, `main`
(`0x80013D2C`)'s `screen_mode_set(1)` after `system_init()`
(`0x8001EEB8`) and `main_buffer_init()` (`0x80013E04`). `mode` (0..3)
picks one entry of four word tables in the image; the entries go to four
words, and `mode` is kept as a byte:

| table | word | meaning | mode 0, 1, 2, 3 |
|---|---|---|---|
| `g_ScreenModeWidths` | `g_ScreenWidth` | display width | 256, 320, 640, 640 |
| `g_ScreenModeHeights` | `g_ScreenHeight` | display height | 240, 240, 240, 480 |
| `D_800AE230` | `D_800B8838` | nothing in the C reads it | 224, 224, 448, 448 |
| `g_ScreenModeRowsShown` | `g_ScreenRowsShown` | visible height | 224, 224, 224, 448 |
| (the mode) | `g_ScreenMode` | `frame_end` skips its `DrawSync` when it is 3 | |

Then, in order:

1. `VSync(0)` (`0x800949A8`), `SetDispMask(0)` (`0x8008BB6C`).
2. `ClearImage(&g_ScreenClearRectangle, 0, 0, 0)`, the rectangle (0, 0,
   320, 448) that covers both 224-line buffers, and `DrawSync(0)`
   (`0x8008BC04`).
3. `GsInitGraph(width, height, intmode, 1, 0)` (`0x8008EAF8`): intmode 4
   (GsOFSGPU), or 5 (with GsINTER) when the height is 480.
4. `GsDefDispBuff` (`0x8008F4D8`): (0, 0, 0, 0) at 480 lines, else
   (0, 0, 0, 0xE0), the second buffer at y = 224.
5. The two ordering tables: `g_FrameOts` is `GsOT[2]` and
   `g_FrameOtTables` their `GsOT_TAG[2][0x400]` rows (0x2000 bytes, ending
   at `g_FrameOts`). For table i: length 10, offset `i * 0xE0`, point 0,
   org = row i, tag = the row's last entry; then `GsClearOt(0, 0,
   &ot[i])` on each, which stores offset 0, point 0 and the tag again, so
   the offset stored by hand does not survive the call.
6. Sony's display environment (`D_800E6FB0`): screen.y = 8, screen.h and
   disp.h = the visible height.
7. The clip rectangle `g_ScreenClipRectangle` (w = the width, h = the
   visible height) to `GsSetClip2D` (`0x8008F1E8`).
8. The drawing environment's `isbg` (`D_800E6F50` + 0x18) = 1: the
   background clear is on.

Mode 1 is the game's: 320 x 240 with a 320 x 224 clip.

## The frame present: `frame_end` (`0x8001F30C`)

`void frame_end(void)` (frame.c) is called by `BtlFrameDraw`
(`0x80018564`), `debug_sprite_page_show` (`0x80019044`), `frame_step`
(`0x8001F458`), `StartMenuRun` (`0x80043E08`) and `MapFrameEnd`
(`0x8005596C`). `frame_step` is `layer_col_fade_step`, `frame_end`,
`frame_start`. In order:

1. `buf = GsGetActiveBuff()` (`0x8008F048`), the buffer on show.
2. Copies the 8-byte rectangle at `g_ScreenDrawAreaX` (which
   `screen_draw_area_set` (`0x8001F488`) fills as x0, y0, x1 - x0, y1 -
   y0) to the stack; when `buf == 0` it adds 0xE0 to the copy's y.
   `packet_draw_area_add(&rect)` holds its height to 0xE0 and queues it at
   entry 0x3FF.
3. `profile_mark_add(0x2100D000)`; if `g_ScreenMode != 3`, `DrawSync(0)`;
   `profile_mark_add(0x210000FF)`: two marks of the profiler, one before
   the wait for the GPU and one after.
4. `VSync(0)`, then `g_FrameVSyncTotal = VSync(-1)`. `g_FrameVSyncCount =
   g_FrameVSyncTotal - g_FramePreviousVSyncTotal`, held to 1..4, and
   `g_FramePreviousVSyncTotal = g_FrameVSyncTotal`: so
   `g_FrameVSyncCount` is the frame's length in vertical blanks.
5. `D_800B8864` goes up by 1, a frame count nothing reads.
6. `profile_start_set()` (`0x80025E8C`), `pad_frame_step()`
   (`0x8001EE98`) and `ResetGraph(1)` (`0x8008B888`), every frame; if
   `g_ProfileShown` is not 0, `profile_draw()`.
7. `GsSwapDispBuff()` (`0x8008F428`), `SetDispMask(g_ScreenDispMask)`,
   `GsDrawOt(g_FrameOt)`: the table whose address `frame_start` stored.

`frame_start` then gives the next frame the other buffer: it zeroes
`g_ProfileMarkCount`, points `g_PacketWritePointer` at that buffer's half
of `g_PacketBufs` (0x10000 bytes each) and `g_PacketOtBase` at its table,
and clears the table. What `g_ScreenMode == 3` is for is not established.

## The frame profiler: `profile_mark_add` (`0x80025DF4`) and `profile_draw` (`0x80025EAC`)

`s32 profile_mark_add(s32 word)` (profile.c) records one mark. While
`g_ProfileMarkCount` is below 0x20 it stores `GsGetVcount() -
g_ProfileStart - 10` in `g_ProfileMarkTimes[n]` and its argument in
`g_ProfileMarkCols[n]`, adds 1 to the count and returns the time; with
0x20 marks taken it does nothing. `GsGetVcount` (`0x8008F3D8`,
libgs_gs_008.c) reads root counter 1 until two reads agree, so the unit
is horizontal blanks. `profile_start_set` stamps `g_ProfileStart` with
the same call; the present runs it right after its second mark.
`profile_mark_get(i)` (`0x80025E70`) returns `g_ProfileMarkTimes[i]`.

There are 12 calls, in frame.c, map_frame.c, map_step.c and map_walk.c,
none reading the result. Each passes a word of the form `0x21BBGGRR`:
`0x210000FF`, `0x2100FF00`, `0x21FF0000`, `0x2100FFFF`, `0x21FF00FF`,
`0x21FFFF00`, `0x21008F8F` and the present's `0x2100D000`.

`profile_draw()`, called from `frame_end` while `g_ProfileShown` is not
0 (`profile_draw_flip` (`0x80075E9C`, menu_nowalk.c) toggles it), turns
the frame's marks into flat quads:

```
p   = (POLY_F4 *)g_PacketWritePointer;
x   = g_ScreenWidth - 8;
top = 0;
for (i = 0; i < g_ProfileMarkCount; i++) {
    y = g_ProfileMarkTimes[i] % 240;            a signed remainder
    colour word (p + 4) = g_ProfileMarkCols[i];
    SetPolyF4(p);                               (0x8008EA68) length 5, code 0x28
    (x0, y0) = (x, top);    (x1, y1) = (x + 8, top);
    (x2, y2) = (x, y);      (x3, y3) = (x + 8, y);
    top = y;  x -= 8;
    AddPrim(g_PacketOtBase, p);                 entry 0
    p++;                                        0x18 bytes
}
g_PacketWritePointer = p;
```

- One quad a mark, 8 pixels wide. The first is the rightmost 8 columns of
  the screen, and each next mark's is 8 further left. So the marks are a
  staircase running down and to the left: mark i's quad spans from mark
  i - 1's line down to its own.
- The y of a mark is the scan line, counted from the stamp less 10, at
  which it was taken, wrapped to 240 lines. A mark taken within 10 lines
  of the stamp gives a negative y, above the screen. A mark later than
  the one before it modulo 240 gives a quad with its top below its
  bottom; nothing orders or holds them.
- The colour word is stored whole before `SetPolyF4`, which then writes
  its top byte: every caller's `0x21BBGGRR` becomes code 0x28 (an opaque
  flat quad) with the caller's B, G, R. The callers' 0x21 never reaches
  the GPU.
- All quads go to entry 0, in front of everything.
- At most 32 quads, 256 pixels of width: at display width 256 the last
  column is x = 0..8, at 320 they end at x = 64.

## A VRAM rectangle saved and put back: `PictureVramSave` (`0x800247F4`) and `PictureVramRestore` (`0x80024820`)

`PictureVramSave()` (picture.c) is `StoreImage(&g_PictureVramRectangle,
g_PictureVramBuffer)` and `PictureVramRestore()` is `LoadImage` of the
same two. `g_PictureVramRectangle` is (320, 0, 160, 480) in the image:
VRAM x 320 to 479 down all 480 lines, which holds sprite page slots 0 and
5 and the first quarter of slots 1 and 6. The one caller, `PictureShow`
(`0x80024A7C`), sets `g_PictureVramBuffer` to `0x8016E000`, saves the
rectangle, shows bromide k (resource `0x24A` + k, its two pictures, the
voice; [../../video.md](../../video.md) §"Bromides") and puts the
rectangle back at its end.

## The upload request's clear: `func_80026280`

`void func_80026280(void) { D_800E4768[0] = 0; }` (mem_move.c).
`D_800E4768` is the request block menu_list.c, menu_run.c, shop_screen.c
and btl_menu.c fill as `[1]` = address, `[3]` = size, `[0]` = 1
([menu-ui.md](menu-ui.md) names uploads made with it), so word 0 reads as
its pending flag; nothing in the C reads word 0. Its one caller is
`func_80025FE0` (profile.c), which calls it first.

## Where the code is

| file | what |
|---|---|
| bg_tile.c | `bg_draw`, `bg_first_draw`, `bg_second_draw`, `bg_second_screen_draw`, the block caches and the cell writes |
| bg_cover.c | the overlay-depth grid |
| layer_col.c | the layer modes, colours and the flat layer |
| tile_layer.c | the battle's tile layer and the wave |
| tile_animation.c, palette_animation.c | the queued cell uploads and the palette animation start |
| palette_row.c, palette_buf.c | the palette rows and their uploads |
| packet_shape.c, packet_sprite.c, sprite_draw.c | the packet helpers |
| font_cell.c | the label-cell screen |
| frame.c | the screen set-up, the frame start and the present |
| profile.c | the profiler |
| picture.c | the VRAM save around a bromide |
| mem_request.c, mem_move.c | the two request blocks |
