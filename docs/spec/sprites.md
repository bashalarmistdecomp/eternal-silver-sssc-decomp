# Sprites: pages, sprite data, the sprite list and the TIMs

This page covers how the game keeps and draws its sprites: the twelve
sprite pages in video memory and the loaders that fill them, the sprite
data a sprite is drawn from (animations, steps, poses, pieces and texel
records), the sprite list and `sprite_draw`, the draw word that picks a
sprite's palette, where the sprite palettes come from, and the full-screen
pictures the game keeps in Sony's TIM form. The families of sprites have
pages of their own: the party's field sheets, the portraits and the map
object sets are in [assets/sprites.md](assets/sprites.md), resource 2's
common sheet and the UI in [assets/battle-ui.md](assets/battle-ui.md), the
spell and item presentations in
[assets/battle-effects.md](assets/battle-effects.md), and the battle's
graphics sets and party bundle in
[code/subsystems/battle.md](code/subsystems/battle.md) §"Battle graphics —
the set bundle, the party bundle, the arena". Where each resource lies on
the disc is [file-sector-map/overview.md](file-sector-map/overview.md).

## Where the sprites are

Every sprite picture is a LUNADATA.FIL resource, read by number through
`g_ResourceSectorPairs` (`0x800AE904`), the EXE's one table of sector
pairs (601 pairs, resources 0 to `0x258`; file-sector-map/overview.md).
Every sector after the archive's header belongs to one resource, so there
is no unlisted region. The address `0x800AF0B4` is not a second table: it is the pair of
resource `0x1EC` inside the same table
([file-sector-map/lunadata-sprites.md](file-sector-map/lunadata-sprites.md)).

| resources | what | read by |
|---|---|---|
| 2 | the common sheet: one 4-bit page and three sprite data blocks | `sprite_reset` (`0x80018DBC`) |
| `0xA3`–`0xE4` | the portraits (FACE blocks), not sprite pages | `FaceDataLoad` (`0x8001B1C0`), `FaceSlot6Load` (`0x8001B50C`) |
| `0xE5`, `0xE6` | the field sprite palettes of rows 504 and 505 | `sprite_init` (`0x80018BFC`) |
| `0x167`–`0x196` | the 48 battle graphics sets | `BtlDataLoad` (`0x80018694`) |
| `0x197` | the battle's party bundle | `BtlPartyDataLoad` (`0x80018A78`), `BtlPartyLoad` (`0x8002B114`) |
| `0x198`–`0x1D7` | the spell and item effects | `BtlEffectLoad` (`0x800897DC`) |
| `0x1E8` | the party's field sheets | `MapPartySpriteDataLoad` (`0x80063D48`) |
| `0x1E9`–`0x249` | the 97 map object sets | `MapObjectsDataLoad` (`0x80065418`) |
| `0x24A`–`0x257` | the bromides and the disc-swap screen (TIMs) | `PictureShow` (`0x80024A7C`), `overlay_disc_wait` (`0x80023184`) |

A map's own texture pages (command 3 of its command list) are tile pages,
not sprite pages: [graphics.md](graphics.md). Every packed stream named
here is unpacked by `inflate_buffer` (`0x80021D6C`,
[formats/compression.md](formats/compression.md)), into the load buffer
whose address `g_LoadBuffer` (`0x800B8884`) holds, `0x80196000`, which is
also the address MENU.BIN is read to (`overlay_load` (`0x80023240`)), or
into a block of its own.

## The sprite pages

A sprite page is 0x80 VRAM words wide and 256 lines high: 256 pixels a
line at 8 bits, 512 at 4. `g_SpritePageCorners` (`0x800A7F88`, twelve
`SpritePageCorner` rows, never written) gives where each lies; the values
are the image's:

| slot | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| VRAM x | 0x140 | 0x1C0 | 0x240 | 0x2C0 | 0x340 | 0x140 | 0x1C0 | 0x240 | 0x2C0 | 0x340 | 0x3C0 | 0x3C0 |
| VRAM y | 0 | 0 | 0 | 0 | 0 | 0x100 | 0x100 | 0x100 | 0x100 | 0x100 | 0 | 0x100 |
| pixels | 8-bit | 8 | 8 | 8 | 8 | 8 | 8 | 8 | 8 | 8 | 4-bit | 4 |

`sprite_init` turns each corner into a texture page word in
`g_SpritePageTPages` (`0x800DDD30`): `GetTPage(1, 0, x, y)` for slots 0
to 9 and `GetTPage(0, 0, x, y)` for 10 and 11. It also fills `D_800DDD50`
with `GetTPage(1, 0, x + 0x40, y)` for slots 0 to 9; nothing in the C reads
that table. Slots 7, 8 and 9 lie where a map's tile pages 0 to 2 lie
(texture pages `0x19`, `0x1B`, `0x1D`); slot 11 is the page of a map's
4-bit page 3 and of the second tile layer (texture page `0x1F`).

Captures of video memory taken with station-tools (a custom decomp toolkit
that will be released later) agree with the table: a town drew its people
from texture page `0x85`, slot 0, and a battle from `0x97` (slot 6) and
`0x8D` (slot 4).

The loaders, each sending to `g_SpritePageCorners[slot]`, 0x80 words wide:

| loader | slots | what it sends |
|---|---|---|
| `sprite_page_list_load` (`0x800182B8`) | 0, 1, 2 … | the packed pages of the resource in the read buffer (`g_ResourceReadBuffer` (`0x800B8828`), which holds `0x80156000`): from header word 2 on, each word the byte offset of one page, ended by a 0 word; page n goes to slot n, 0x100 lines. Called for a map object set by `MapObjectsDataLoad`, `MapObjectsPagesLoad` (`0x8006557C`) and `MapSpriteDataReload` (`0x800644E0`). |
| `sprite_page4_image_load` (`0x80017EB0`) | 4 | one picture of a sprite data directory (resource `0x1E8`) into the first free run of 8-line units of slot 4 (a 12-byte use map, `g_SpritePage4UseMap`) |
| `sprite_page_image_add` (`0x8001808C`) | 5, 6 | the same into slot 5 or 6 (`g_SpritePageUseMap`, 0x20 units a page); when neither fits, the picture goes to slot 7's corner all the same |
| `sprite_page_effect_load` (`0x80018AE0`) | 7, 8, 9 | up to three pictures of an effect resource: entries `{u16 length, u16 place}`, both in 4-byte units, length 0 ending the list; `length >> 6` lines each (the length is a u16 read into a signed word, so it is never below 0 and the C's rounding never applies) |
| `BtlDataLoad` | 0 to 4 | a battle set's five pages, `(code >> 8) * 4` lines each |
| `BtlPartyImageLoad` (`0x800187D8`) | 4, 5, 6 | the party bundle's pictures, by a 0x60-byte use map `g_BtlSpritePageUseMap` |
| `sprite_reset` | 10 | resource 2's 4-bit sheet, to (0x3C0, y), `0x100 - y` lines from line y |

`sprite_page4_image_load`, `sprite_page_image_add` and `BtlPartyImageLoad`
then point the block's texel records at the page and the lines they chose
(`page` set, `v += unit * 8`); `sprite_reset` sets the page of every texel
record of resource 2's blocks to 10. `sprite_page_list_load` and
`sprite_page_effect_load` change nothing: those blocks already name their
slots. `sprite_page_image_send` (`0x80017D50`) is a third loader for slots
5 and 6 that reads a directory of 16-byte rows (a 32-bit place at +8 and a
32-bit size at +0xC); nothing calls it.

## Sprite data

A sprite data block (battle.md calls the same layout an actor's block)
begins with `SpriteDataHead` (sprite.h): six
u16 words, each the place of one table, in 4-byte units from the block's
start.

| word | table | row |
|---|---|---|
| 0 | the animation table | `SpriteAnimationRow` {u16 count of steps, bit 0x8000 holding the last step; u16 first step} |
| 1 | the step list | `SpriteStep` {u16 pose, u16 frames it lasts} |
| 2 | the pose table | `SpritePoseRow` {u16 count of pieces, u16 first piece} |
| 3 | the piece table | `SpritePiece`, 0x18 bytes |
| 4 | the texel records | `SpriteTexelRecord`, 12 bytes |
| 5 | the end of the texel records | the loaders count `(word 5 - word 4) / 3` records |

The loaders make a record's addresses from the head (block + word × 4):
`MapRecordSpriteDataSet` (`0x800645A8`) on the field,
`BtlRecordSpriteDataSet` (`0x80089A3C`), `BtlPartyLoad`, `BtlEnemyLoad`
(`0x8002B3AC`) and `BtlEffectLoad` in a battle. On the field the blocks
are found by number: `g_MapSpriteDataEntries` (`0x800B8C68`) holds the
address of the sprite data table, `0x801EA300` (set by `GameStart`):
0x100 entries of 0x10 bytes, one for each sprite data number, whose first
word is that number's unpacked block (0 for none, 1 while it waits to be
loaded again). `MapPartySpriteDataLoad` and `MapObjectsDataLoad` fill it;
`MapObjectsDataLoad` makes entry 0 resource 2's `g_SpriteData1`
(`0x800DDFB0`), the chest's.

A piece (`SpritePiece`):

| offset | field |
|---|---|
| +0x00 | the kind word: bits 0x7000 the kind (0 a textured rectangle; 0x1000 and 0x2000 a textured rectangle anchored by the modes in bits 0xF00, `sprite_piece_edges_get` (`0x80019BEC`); 0x3000 a textured quad on four corners; 0x4000 a box in one colour; 0x5000 an outline, in a battle one `LINE_F3`; 0x6000 nothing); bits 0x30 the piece's mirror bits |
| +0x02, +0x04 | width and height in pixels |
| +0x06 | the number of its texel record |
| +0x08, +0x0A | for kind 0 the x and y offset from the sprite's place; for the other kinds the first point |
| +0x0C … +0x16 | the other three points (kinds 0x1000 to 0x5000) |

A texel record (`SpriteTexelRecord`): +0x02 the sprite page, 0 to 11;
+0x08 and +0x0A the picture's u and v in that page. +0x00, +0x04 and +0x06
are not read by any drawer: a piece takes its size from the piece.

## The sprite list and `sprite_draw`

`g_SpriteList` (`0x800B88F4`) holds the address of the sprite list,
`0x801D3B80`, which `sprite_reset` stores and clears: 0x96 entries of
0x24 bytes, `SpriteEntry` (sprite.h). A battle record (`BtlRecord`) and a
map record carry one at +4 and copy it into the list each frame, so an
entry's +0x0A is a record's +0x0E. `sprite_list_draw` (`0x8001A798`)
draws every entry whose first word is not 0, in three runs: entries 0x46
to 0x6D, then 0 to 0x45, then 0x6E to 0x95.

| offset | field |
|---|---|
| +0x00 | on: not 0 to draw |
| +0x02 | x |
| +0x04 | depth, the sort key |
| +0x06 | y |
| +0x08 | the attribute word: 0x2000 mirrors across, 0x4000 down, 0x8000 semi-transparent with colour table entry 2 |
| +0x0A | the draw word, below |
| +0x0C | address of the texel records |
| +0x10 | address of the pose table |
| +0x14 | address of the piece table |
| +0x18 | the pose drawn |
| +0x1C | 1: scaled by +0x20 / +0x22 (0x100 = 1) |

The draw word:

| bits | read by | meaning |
|---|---|---|
| 0x00FF | `sprite_draw` | the palette's x: `GetClut(word & 0xFF, 0x1F8 \| ((word >> 8) & 3))`, so x 0 to 0xF0 in steps of 16 |
| 0x0300 | `sprite_draw` | the palette row: 504 + 0 to 3 |
| 0x0400 | `sprite_draw` | semi-transparent |
| 0x0C00 | `sprite_piece_draw` | the mix group, an index of `g_SpriteMixLevels` (`0x800B8908`) |
| 0x6000 | `sprite_draw` | the class that picks the ordering-table entry |
| 0x8000 | `sprite_draw` | colour table entry 1 |

`sprite_draw` (`0x8001A528`), from the C and the register's reading:

* Outside a battle (`g_BtlRunning` is 0) the bg's scroll is taken off x, y
  and the depth.
* The key is `0xFF - (depth + 0x20)`, held to 0..0xFF; it is 0 for entries
  from 0x6E up and 0xFF for entries 0x46 to 0x6D.
* The ordering-table entry, by class: 0 gives 0x40; 0x2000 and 0x4000 give
  0x200 + key; 0x6000 gives 0x20 + (0xFF - y) / 8. The table is drawn from
  entry 0x3FF down (rendering.md §"The field's draw rule"), so a larger
  depth stands in front, and entries 0x46 to 0x6D stand behind every
  other sprite of their class: the field's twins are there (the party's
  at 0x46 to 0x4B, `MapPartyAnimationStep` (`0x80065CF8`); the map
  objects' at 0x56 to 0x65). Entries from 0x6E up, key 0, stand in front
  of every other sprite of their class.
* Semi-transparency is `((word >> 10) | (attr >> 15)) & 1`.
* The colour is entry `(word >> 15) | ((attr >> 14) & 2)` of
  `g_SpriteCols` (`0x800A7F78`): 0x808080, 0x404040, 0, 0 in the image.
  Only entry 0 is ever written (`sprite_col_set`, and for the length of a
  call `BtlBgWaveDraw` and `sprite_grey_draw`), so bit 0x8000 of the draw
  word draws a sprite at half brightness. `g_MapObjectsSpriteBits`
  (`0x800B8C4A`), which `MapObjectsAnimationStep` ORs into every map
  object's draw word, is 0x8000 while a shop or the menu is open
  (`ShopOpen`, `MenuOpen`) and 0 otherwise.
* The pose picks a row of the pose table: count and first piece. With no
  pose table the count is 1. The pieces go to `sprite_piece_draw`
  (`0x80019DBC`) from `first + count - 1` down to `first`, all at the one
  ordering-table entry. The loop stops when its counter is 0x33 or more,
  and the counter starts at `count - 1`, so a pose of more than 51 pieces
  draws none.

Each piece's packet is linked at the head of its ordering-table entry
(`AddPrim`, in `sprite_plain_draw` (`0x80016CF0`) and `sprite_mirror_draw`
(`0x80016F44`)), and the GPU walks an entry from its head. So piece
`first`, linked last, is drawn first and lies under the others; piece
`first + count - 1` lies on top (battle.md §"The actor renderer — how a
pose is drawn").

`sprite_piece_draw` by the piece's kind:

* Kind 0: the rectangle at the piece's offset, texels from its texel
  record. With the entry's +0x1C set, through `sprite_scale_draw`
  (`0x80017988`); else, when the mirror bits (the piece's 0x30 exclusive-or
  the entry's attribute bits 0x6000 >> 9) are not 0, through
  `sprite_mirror_draw`, which takes the page number and reads the texels
  in reverse; else through `sprite_plain_draw` with the page's word from
  `g_SpritePageTPages`. An entry of class 0x6000 is drawn by those two with
  colour entry 0 put to 0x808080 for the call.
* 0x1000 and 0x2000: a quad through `sprite_quad_draw` (`0x800174F0`) from
  the edges `sprite_piece_edges_get` gives; 0x3000: a quad on the piece's
  own four corners.
* 0x4000: a box in one colour; 0x5000: an outline, or in a battle one
  `LINE_F3` in a colour of row 4 of the palette buffer; 0x6000: nothing.
* While `g_SpriteMixOn` (`0x800B8571`) is on and the group's level is not
  -1, a second packet, the mix copy, follows each piece, with the palette
  word `GetClut(0, 0x1FD)` (the register's reading of
  `sprite_plain_draw`).
* On the field (`g_BtlRunning` 0) a piece of class 0x2000 also goes to
  `bg_cover_box_set` (`0x80013EF0`).

## Who sets the draw word

* On the field, `MapRecordInit` (`0x80065940`) by the record's kind: 1 →
  `0x2000` (row 504), 2 → `0x2100` (row 505), 7 → `0x2370` (row 507, x
  0x70) with the attribute word 0, any other → `0x2200` (row 506). The
  kind comes from a definition's byte 0 (`MapObjectInit` (`0x800656CC`):
  `(byte & 0x3F) + 1`), from `MapPartyRecordInit` (`0x800649DC`: 1 for a
  party number below 0x1F, 2 for any other), or from a script command's
  byte (`ScriptCommandMoveChrRowActorSet`). Then by the definition's mode
  byte: 1 ORs 0x800, 2 ORs 0xC00, 3 makes the word 0x2000 and ORs 0x8000
  into the attribute word. `MapPartyRingFollow` (`0x800671C4`) puts the
  ring entry's class into bits 0x6000 each time it moves the party.
  assets/sprites.md §"Map object sets" has the table with what each row
  holds.
* In a battle, `BtlRecordSpriteDataSet` stores `(word & 0x3FF) | 0x2000`;
  `BtlPartyLoad` stores 0x2000 (row 504) and `BtlEnemyLoad` 0x2100 (row
  505); `BtlShowOpen` (`0x8007B7C8`) puts 0x2000 on the effect's record
  after `BtlEffectLoad` has put 0x2200 there
  ([assets/battle-effects.md](assets/battle-effects.md)).

## The sprite palette rows

`sprite_draw` reaches rows 504 to 507 only, x 0 to 0xF0. What fills them
([formats/palettes.md](formats/palettes.md) has every row):

| row | on the field | in a battle |
|---|---|---|
| 504 | resource `0xE5` (`sprite_init`) | resource `0x197` (`BtlPartyDataLoad`) |
| 505 | resource `0xE6` (`sprite_init`) | the set's header word 9 (`BtlDataLoad`) |
| 506 | the map's palette, type 4 sub 1 (`resource_palette_load` (`0x80020A08`)) | an effect's palette (`sprite_palette_load` (`0x80018BAC`), from `BtlEffectLoad`) |
| 507 | resource 2's palette, sixteen 4-bit CLUTs (`sprite_reset`) | the same |

`sprite_init` reads resource `0xE5` into the load buffer, makes colour 0
the word 0 and every other colour byte-swapped with bit 15 set, and sends
it with `LoadClut` to row 504; resource `0xE6` the same to row 505. Those
two resources are stored byte-swapped with bit 15 clear. It keeps what the
two `LoadClut` calls return in `D_800B88EA` and `D_800B88E8`, and
`GetClut(0, 0x1FA)` in `D_800B88EC`; nothing in the C reads the three. It
ends with `sprite_reset(0)`. It runs through `sprite_init_copy`
(`0x80063B10`) from `GameStart` and from `MapBtlRun` after every battle,
so rows 504 and 505 hold the field's colours again once a battle ends.

`sprite_palette_load(place, palette)` zeroes the first colour at the
place, in 4-byte units in the read buffer, and sends 256 colours with
`LoadClut` to row `0x1F4 + ((palette >> 8) & 7)`; its one call passes
0x600, row 506. On the field row 506 is sent with `LoadImage`
(`palette_rows_set` → `palette_rows_load`), not `LoadClut`.

The game's C holds seven `LoadClut` calls: `BtlDataLoad` (rows 500 and
505), `BtlPartyDataLoad` (504), `sprite_palette_load` (computed, 506),
`sprite_init` (504, 505) and `sprite_reset` (507).

Measured against video memory captured with station-tools:

* Resources `0xE5` and `0xE6` after `sprite_init`'s fix-up equal the
  captured rows 504 and 505, 512 bytes of 512, in two field scenes. In a
  battle they do not (175 and 2 bytes of 512): the battle has sent its own.
* The battle's row 504 equals resource `0x197` read as
  `BtlPartyDataLoad` reads it: the resource's first u16 is the byte place
  of its palette (`0x64` in the retail resource), and colour 0 is zeroed.
* The battle's row 505 (in that capture the Slimes' palette), less its
  colour 0, lies in five battle sets, the same 510 bytes in each:

  | resource | offset of colour 1 |
  |---|---|
  | `0x167` | `0xDD2` |
  | `0x168` | `0xD92` |
  | `0x169` | `0xD8A` |
  | `0x16B` | `0xB26` |
  | `0x16C` | `0xE06` |

  Each offset is the palette's place plus 2, since the search passed over
  colour 0; battle.md reads `0x167`'s place `0xDD0` as header word 9 × 4.
* Only x 0 of rows 504 to 507 held a sprite palette in the captures; the
  second layer's draws in a town used x 32, 48 and 64 of row 508
  (`0x7F02` to `0x7F04`).

## Pictures: the TIMs

The full-screen pictures, the title's pictures and the glyph atlas are
packed TIMs in type-2 commands of a resource's command list
(graphics.md §"A map is a command list"). `resource_tim_add`
(`0x800208A0`) takes the command: with sub-number 0 it unpacks the payload
with `inflate_buffer` into the load buffer and sends it at once with
`tim_image_load` (`0x8002073C`); with any other it keeps the payload's
address in `g_ResourceTims` (`0x800E0C10`) as the next kept picture.
`resource_tim_unpack` (`0x80020918`) unpacks kept picture n into the load
buffer; `resource_tim_load` (`0x80020950`) unpacks it and sends it.
`resource_command_run` resets the count of kept pictures each time it
starts a list.

`tim_image_load` hands the TIM's address + 4 to Sony's `GsGetTimInfo`
(`0x8008FC48`), sends the pixels with `LoadImage` to the rectangle the
header gives when its width and height are not 0, sends the palette to its
rectangle when bit 3 of the mode word is set, and waits with `DrawSync(0)`.
The layout it reads is the PlayStation's TIM:

```
+0   u32 id, 0x10
+4   u32 mode: bits 0-2 the pixel size (0 4-bit, 1 8-bit, 2 16-bit),
         bit 3 a palette follows
if a palette:
     u32 length of this block, header included
     u16 x, y in video memory
     u16 w, h (w in 16-bit words)
     the colours
then the pixels:
     u32 length of this block, header included
     u16 x, y in video memory
     u16 w (in 16-bit words), h
     the pixels: w × 4 of them a line at 4 bits, w × 2 at 8, w at 16
```

Who reads them:

| resource | pictures | how |
|---|---|---|
| 1 | the glyph atlas | sub 0, sent at once at boot ([assets/battle-ui.md](assets/battle-ui.md)) |
| 3 | the title's pictures and pages | `StartPictureShow` (`0x8001607C`) shows kept picture 0 with `resource_tim_load`; `StartScreenDraw` (`0x80043848`) draws its sprites, the logo among them, from 8-bit pages at (384, 0), (768, 0) and (896, 0) ([assets/title-logo.md](assets/title-logo.md)) |
| `0x24A` + k | bromide k, k = 0 to 12: two pictures | `PictureShow(k)` runs the resource, then `resource_tim_load(0)` and `resource_tim_load(1)`; `MoviePlay` (`0x800232D4`) calls it for play-event numbers `0x37` to `0x43` (video.md §"Which movie is which") |
| `0x257` | the disc-swap screen | `overlay_disc_wait` runs the resource; `cd_change_wait` (`0x80024D2C`) unpacks kept picture 0 or 1, 2 more after a refused disc, and `screen_picture_put` puts it on the screen from the buffer |

The station-tools exporters measured the bromides' pictures at 320 × 256,
320 × 240 and 320 × 224, 8 bits a pixel. Two of the title's sizes are
those of 8-bit TIMs with a 256-colour palette: 72,224 bytes is 320 × 224
pixels and 66,080 is 256 × 256, each with 32 bytes of headers and 512 of
palette (file-sector-map/overview.md §"OPEN1.BIN — resource 3").
