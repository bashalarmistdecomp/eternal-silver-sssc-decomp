# Lunar SSSC (PS1): the map's graphics format

This page covers what a map resource holds and how the code reads it: the
command list every map is, the texture pages and palettes it sends to
video memory, the tile layout, the attribute plane and the second tile
layer. How the tiles are drawn (the 16-bit tile word, the draw order) is
[code/subsystems/rendering.md](code/subsystems/rendering.md); how a map is
loaded and entered is
[code/subsystems/map-engine.md](code/subsystems/map-engine.md); the sprites
a map shows are [sprites.md](sprites.md) and
[assets/sprites.md](assets/sprites.md) §"Map object sets"; the palette
rows are [formats/palettes.md](formats/palettes.md).

## Terminology

| Term | Meaning |
|------|---------|
| Texture page | a 256 × 256 picture in video memory (page 3: 256 × 128 at 4 bits) from which the tiles are drawn |
| Map | the playable area: the tile layout that places 16 × 16 tiles of the pages on a grid, with its attribute plane and second layer |

A page is the paint and the layout is the painting; this page never calls
one by the other's name.

## World Map Arrangement Data

There are 97 maps, LUNADATA resources 4 to 100. The archive's own table of
contents names 43 of them (MAP_xxx.BIN); the code finds every one by map
number through `g_MapResourcePairs` (`0x800AF364`) and `MapResourceFind`
(`0x80020CD0`) (text.md §"Locations"). No map is a battle background: a
battle's arena is drawn from its graphics set's pages
([code/subsystems/battle.md](code/subsystems/battle.md) §"The arena").

### A map is a command list

`resource_command_run` (`0x800202FC`, resource_load.c) walks a resource as
a list of commands. Word 0 is the command word: its low byte the type,
bits 8 to 15 a sub-number. Word 1 is the payload's byte count, and the
payload follows. The next command starts at the payload + count / 4 words;
a command word of 0 ends the list. Types 8, 10 and 11 have no handler and
are passed over. Every map holds the same commands; the sizes below were
measured over all 97 with station-tools, a custom decomp toolkit that will
be released later.

| type | handler | what it does |
|---|---|---|
| 9 | `resource_map_enter_table_set` (`0x80020B8C`) | copies the payload to the map's enter table, at `MapEnterTableAddress()` (`0x80055E6C`) = `0x801E6B00`, only while `g_MapResourceSw` (`0x800B86D8`) is set (a map entered, not one loaded again). 11-byte rows that `MapChange` (`0x8005922C`) reads: byte 0 the map, 1 the object set, 2 the fade, 3 a sound for `SoundPlay` when not 0, 4 to 7 the view's limits in 8-pixel units, 8 to 10 the arrival spot. It is the first command of every map, so a map's first word is 9 and its second this command's byte count. |
| 3, sub 0 to 2 | `resource_tpage_load` (`0x8002097C`) | unpacks the payload and sends it with `tpage_image_load` (`0x800204F0`): 0x80 words by 0x100 lines at texture page `0x19 + 2k`, (576, 704 or 832, 256). Read at 8 bits: 256 × 256. Always 65,536 bytes. |
| 3, sub 3 to 0x7F | `resource_tpage_load` | `tpage_small_image_load` (`0x8002053C`): 0x40 words by 0x80 lines at texture page `0x1F`, (960, 256). The drawers read it at 4 bits, so it is a 256 × 128-texel, 16-colour sheet, the upper half of that page. Always 16,384 bytes; maps use sub 3. |
| 3, sub 0x80 to 0x82 | `resource_tpage_load` | the same pages as 0 to 2, through `tile_tpage_get` (`0x8001DC94`), which notes them for the tile layer (the effects' layers use this form) |
| 4, sub 0 | `resource_palette_load` (`0x80020A08`) | two rows of 256 colours: `palette_zero_fix` (`0x80020830`), rows 500 and 501 (`palette_rows_set` (`0x8002061C`)), the second again as row 508 (`palette_bg_second_row_set` (`0x80020670`)) and both again as rows 510 and 511 (`palette_bg_cover_rows_set` (`0x800206D4`)). 1,024 bytes on 95 maps, 512 (one row) on 2. |
| 4, sub 1 to 0x7F | `resource_palette_load` | one row: `palette_bit15_set` (`0x800207E8`), row 506, the map objects' palette. 512 bytes on every map, all sub 1. |
| 4, sub 0x80 | `resource_palette_load` | one row, row 501 (the effects' layers) |
| 5 | `resource_bg_set` (`0x80020B34`) | copies the payload into the buffer whose address `g_BgDataBuffer` (`0x800B882C`) holds: the tile layout |
| 6 | `resource_bg_attribute_set` (`0x80020B60`) | copies it into `g_BgAttributeDataBuffer`'s (`0x800B8834`): the attribute plane |
| 7 | `resource_bg_second_set` (`0x80020BD8`) | unpacks it into `g_BgSecondBuffer`'s (`0x800B887C`), 0x4000 bytes: the second tile layer |
| 16 | `resource_tile_animation_set` (`0x80020BFC`) | unpacks it to `0x801C1B00`, `g_TileAnimationTiles`: the tile images the tile animations copy, 0x100 bytes each (rendering.md §"Queued VRAM cell uploads") |

The other types: 1 (`resource_command01_set` (`0x80020480`), resource 1's
block), 2 (`resource_tim_add` (`0x800208A0`), a TIM: sprites.md §"Pictures:
the TIMs"), 12 (`resource_palette_row_load` (`0x80020ACC`), one palette
row), 13, 14, 15 and 17 (the sound banks: file-sector-map/overview.md) and
18 (`resource_tile_pattern_set` (`0x80020C24`), the effects' tile layers:
assets/battle-effects.md).

Every packed payload is unpacked by `inflate_buffer` (`0x80021D6C`,
inflate.c): gzip's inflate, reading its input 16 bits at a time and taking
every block as a dynamic-Huffman one
([formats/compression.md](formats/compression.md)). A standard raw-deflate
decoder (zlib with wbits −15) also unpacks the maps' pages and palettes to
exactly the sizes above (measured with station-tools).

### Pixels and transparency

Pages 0 to 2 are 8 bits a pixel and page 3 is 4; the colours are the
PlayStation's 16-bit word ([formats/palettes.md](formats/palettes.md)).
There is no colour key. The hole is colour index 0 of each 256-colour row:
`palette_zero_fix` makes it the word 0, which the GPU does not draw, and
makes a colour stored as 0 at any other index 1, drawn black
(code/subsystems/rendering.md §"The field's draw rule"). A map's pages
hold its tiles only; its people and objects are drawn from the sprite
pages of its object set ([sprites.md](sprites.md) §"The sprite pages").

### The tile layout — `bg_block_load` (`0x80014F3C`, bg_tile.c)

The layout (command 5) begins with `BgBlockTable` (bg.h):

```
u16 entry[256]      block (bx, by) at bx + by * 16: a 16 x 16 grid of
                    8 x 8-tile blocks, 128 x 128 tiles, 2048 px square
u16 fill_limit      +0x200, copied to g_BgBlockFillLimit (0x800B88C0)
u16 plain_limit     +0x202, copied to g_BgBlockPlainLimit (0x800B88C4)
the blocks' data

entry == 0          64 x tile word 0
entry <  fill_limit 64 x the u16 at table + (entry & ~1)
entry >= plain_limit 64 plain u16s at table + entry; an odd entry first
                    calls hex_dump(table, 0x210) (0x80021510); no map has one
otherwise           lz_unpack(table + entry) (0x80022BF8): 64 tiles,
                    ending at the stream's own end mark
```

`bg_data_reset` (`0x80014E60`) copies the two limits. A block is unpacked
into one of 16 slots of the block cache: for tile (tx, ty), slot
`((tx >> 3) & 3) | ((ty >> 1) & 0xC)`, a 4 × 4 grid of blocks.
`hex_dump` walks the bytes as a dump of memory would and prints nothing in
this build. In Caldor Isle's layout the fill words at +0x208 and +0x20A
are 0x10C1 (the sea) and 0x1000. A tile (x, y) is word `(x & 7) + (y & 7) * 8`
of its block, drawn by `bg_first_draw` (`0x80014058`); the tile word's
bits (u, v, page, palette, mirror) are in rendering.md §"16-bit Tile Value
Format", which also has the writes a map makes to its own layout while it
runs (§"Run-time tile writes"). There is no skip of tile 0.

### The attribute plane (type 6)

The same block rules on a finer grid, `bg_attribute_block_load`
(`0x800154F8`): 16 × 16 blocks of 16 × 16 cells of 8 pixels, a 256 × 256
plane. Only the field's movement and event code reads it, so it is
walkability and trigger data, not a picture. Its readers are
`MapAttributeFreeCheck` (`0x800692D4`), `MapAttributeClassGet`
(`0x8006960C`), `MapAttributeFind` (`0x80069A20`) and
`MapAttributeLineCheck` (`0x80069C6C`) (npc-scripting.md §"The footprint
probe" and §"The map-cell event probe"); `bg_attribute_get` (`0x80015928`)
reads it too and nothing calls it. The cell word:

| bits | read by | meaning |
|---|---|---|
| 0x8000 | `MapAttributeClassGet` | the class's cover (npc-scripting.md) |
| 0x4000 | `MapAttributeFreeCheck` | solid; with layer 0 every shaped cell counts as solid |
| 0x3800 | `MapAttributeFreeCheck` | the shape: 0 never blocks, 0x3800 a full block, 0x0800 / 0x1000 / 0x2000 / 0x1800 the four diagonals, each passed over in one corner cell of the box |
| 0x07FF | `MapAttributeFind` | an attribute: kind in bits 7 to 10, number in bits 0 to 6; number 0 = none |

Measured: every map has a type-6 plane. Vane's (map 31) has 10,788
full-block cells, 255 diagonal ones, no cell with 0x4000, and 0x8000 on
44,116 of its 65,536 cells.

### The second tile layer (type 7) — `bg_second_draw` (`0x800148E0`)

Every map carries a type-7 block and it always unpacks to exactly 0x4000
bytes (measured): 128 rows of 64 `s16` cells of 8 × 8 pixels, a
512 × 1024-pixel plane that wraps both ways (`row & 0x7F`, `col & 0x3F`).
`g_BgSecondBuffer` has two readers, `bg_second_draw` and
`bg_second_screen_draw` (`0x80014D74`), so the block is a picture, not
collision. Measured: 24 maps leave it empty (every cell −1), 71 use wide
cells and 47 mirrored ones.

```
cell word (-1 = empty)
0x001F          texel u / 8
0x01E0          texel v / 8
0x01F8 (>> 3)   ordering-table offset 0..63, added to the mode's base: the
                same bits as v and the top two of u
0x0200          head of a wide tile: one sprite 8 high and 16 wide, or 32
                wide when the next cell is not 0; it takes 2 or 4 cells,
                and its mirror bits are not read
0x0400 / 0x0800 mirror in x / y (8 x 8 cells): a POLY_FT4 whose texel edges
                run cell + 7 down to cell - 1 (held at 0)
0xF000          palette n: palette word 0x7F00 + n, row 508, x = 16 n
```

* Texels: page 3, at 4 bits (the draw-mode word `0xE1000200 | 0x1F`), so
  a 256 × 128-texel sheet, exactly the u and v range of a cell word.
* Palettes: row 508, which `resource_palette_load` makes from the second
  half of the type-4 sub-0 palette with bit 15 set on every colour and
  colour 0 of each 16 zeroed: the sixteen palettes `0x7F00 + n`.
* Scroll: its own. `bg_draw` (`0x800145EC`) draws it after
  `bg_first_draw` at `g_BgSecondScrollX` and `g_BgSecondScrollY`, which
  scripts set through `bg_second_scroll_set` (`0x800145DC`); x >> 4 picks
  the first cell pair, so it steps in 16s with a 0 to 15 pixel offset.
* Mode: `layer_mix_mode_get()` (`0x8001DFE8`;
  code/subsystems/menu-ui.md §"The two layer records — `layer_col.c`"):

  | mode | ordering-table entries | blending |
  |---|---|---|
  | 0 | 0x3C0 + offset | opaque |
  | 3 | 0x100 + offset | opaque |
  | 1 | 0x100 + offset | semi-transparent (colour word bit 25) |
  | 5 | 0x100 + offset | semi-transparent, page word 0x3F: blend rate 1 (B + F) |

  The switch also has a case 4 that draws nothing and falls through on 2,
  but `layer_mix_mode_get` returns only 0, 1, 3 and 5. The colour word is
  `g_BgSecondCol` (`0x800B8554`, `bg_second_col_set` (`0x800148C8`)). A
  texture page packet for page 0x1F is linked at entry 0x100 first
  (`packet_tpage_add` (`0x8002566C`)).
* Packets: a plain cell is a draw-mode word and a 0x64 sprite 8 × 8, a
  mirrored one a POLY_FT4 through `AddPrim`, a wide one a single 16- or
  32-pixel sprite.

The ordering table is drawn from entry 0x3FF down (`frame_start`
(`0x8001F294`) and `frame_end` (`0x8001F30C`), frame.c), so mode 0's
entries from 0x3C0 lie behind the tile layer's 0x300 range, and the other
modes' entries from 0x100 in front of the tiles and the actors
(rendering.md §"The field's draw rule"). So what shows through a tile's
transparent pixels is the second layer when its mode is 0, else the
frame's clear colour, black. `bg_both_copy` (`0x800153A0`) and `bg_copy`
(`0x80015224`) draw nothing: they copy a rectangle of cells inside the
layout when a map changes its own tiles (rendering.md §"Run-time tile
writes").

`bg_second_screen_draw` is the form taken while bit 0x80 of `g_BgMode`
(`0x800B8774`) is set, which `StartResourceLoad` (`0x8004409C`) sets for
the title: the block read as one fixed screen of 14 rows of 21 tiles of
16 × 16, word `row * 32 + column`, 0 drawing nothing. Its tiles come from
the 8-bit texture page `0x1D` (draw-mode word `0xE100029D`) with palette
word `0x7D40`, row 501, at ordering-table entry 0x2FE, not from page 3.

## The other resources

* Resource 1 (SYSTEM.DAT): type 1 (87,856 bytes unpacked: the string
  table at `+0x0000` to `0x5999`, text.md §"The string table"; the palette
  animations' colour frames at `+0x599C`, `g_PaletteAnimationCols`; the
  icons at `+0x899C`, `IconInit` (`0x8001ABCC`)), type 2 (the glyph atlas
  TIM, 16,416 bytes, 256 × 128 at 4 bits, video memory (960, 384)) and
  three type 12 (palette rows 502, 503 and 507, 512 bytes each).
  [assets/battle-ui.md](assets/battle-ui.md) §"Resource 1 (SYSTEM.DAT)".
* Resource 2 (STFIELD.DAT) is not a command list: `sprite_reset`
  (`0x80018DBC`) reads it as eight u16 places of a palette, three packed
  sprite data and a packed 4-bit page. assets/battle-ui.md §"Resource 2
  (the common FX sheet)".
* Resource 3 (OPEN1.BIN), the title screen: a command list of pages,
  palettes, TIMs and the fixed screen above.
  [file-sector-map/overview.md](file-sector-map/overview.md) §"OPEN1.BIN —
  resource 3" and code/subsystems/menu-ui.md §"The title screen's data and
  draw".
* MENU.BIN, on the disc beside LUNADATA.FIL, is not a picture file: it is
  the movie player's overlay ([video.md](video.md)).
