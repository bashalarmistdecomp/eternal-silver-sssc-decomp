# Battle spell and item presentations: the data (SLUS_006.28)

This page covers the data of the battle's presentations (the game's word
is a show): the EXE's tables by effect number, the effect resources
(`0x198` to `0x1D7`) that `BtlEffectLoad` reads, which palette an
effect's sprites are drawn with, and the background layers (flag 4). How
the show functions play a presentation (the records, the pan, the impact
and the sound) is
[../code/subsystems/battle.md](../code/subsystems/battle.md) §"Spell
presentations"; the sprite format is [../sprites.md](../sprites.md).
Counts marked as measured were taken over the retail resources with
station-tools, a custom decomp toolkit that will be released later.

## Dispatch — `BtlShowNumberRun(n)` (`0x8007BB80`)

The show number is the acting row's: `BtlActionRowRun` (`0x80031ABC`) takes byte 6 of a
row of `g_BtlAttackRows` (`0x800AC640`, enemy specials too: row 92 has
0x50 and row 113 0x54) or of `g_BtlSpellRowsByNumber` (`0x800AA728`), or
byte 9 of an item row, into `g_BtlActionShowNumber` (`0x800B8C0F`).
`BtlAttackAnimationRun` (`0x800315A8`) runs 0x3A, 0x3B or 0x3C for some
party attacks (by the member and a worn item), and `BtlItem83Revive`
(`0x80038868`) sets 0x34.

* 0 runs nothing. 0x97 to 0xFD (`(n + 0x69) & 0xFF < 0x67`) go to
  `BtlActingShowRun` (`0x8003A158`), the shows the acting slot's own
  record plays, not covered here.
* Any other: `BtlShowOpen(n)` (`0x8007B7C8`), the function
  `g_BtlShowFunctions[n − 1]`, then `BtlShowClose()` (`0x8007BACC`). In a
  battle (`g_BtlRunning` set) `BtlShowOpen` calls
  `BtlEffectLoad(g_BtlShowRecords + 0x500, n)` (`0x800897DC`), puts 0x2000
  in that record's draw word, and when the effect's byte 2 is not 0 calls
  `VoiceSet` (`0x80055470`) with byte 3 for an acting slot whose id is
  0x0B, 0x7C or 0x7D and with byte 2 for any other.

## The EXE tables

| table | shape | per effect number |
|---|---|---|
| `g_BtlEffectRows` (`0x800AC43C`) | 4 bytes × 0x64 (numbers 0 to 0x63) | +0 flags (1 sprite data and pages, 2 a palette, 4 a background layer; 0: `BtlEffectLoad` does nothing), +1 the row of `g_BtlEffectTileRows`, +2 a voice, +3 the voice for an acting slot of id 0x0B, 0x7C or 0x7D |
| `g_BtlEffectResourceNumbers` (`0x800B5E0C`) | u16 × 0x64 | the LUNADATA resource; 0 = none, and `BtlEffectLoad` then reads whatever the buffer holds (effects 0x35 to 0x37 have flag 2 and no resource) |
| `g_BtlShowFunctions` (`0x800B5CA4`) | u32 × 0x5A, ending where `g_BtlEffectResourceNumbers` begins | the show function, by number − 1 |
| `g_BtlEffectTileRows` (`0x800AC5CC`) | 4 bytes | the background record: +0 the number of a pattern list, +1 the width, +2 the height |
| `g_TileResourceRows` (`0x800AE128`) | {u8 number, u8, u16 resource}, walked by `tile_resource_load` (`0x8001DAE4`) | the background layer's resource (`0x1D8` to `0x1E7`) |
| `g_BtlTilePatternLists` (`0x800ADF64`) | byte lists, each ended by 0xFF; `BtlTilePatternListGet(n)` (`0x8008A8F8`) returns list n (from 1; 0 = none) | the background's frame sequence |

Resources (`g_BtlEffectResourceNumbers`, read from the image): effects 1
to 0x2F → `0x198` to `0x1C6`; 0x34 → `0x1C7`; 0x3D to 0x42 → `0x1C8` to
`0x1CD`; 0x54 and 0x56 → `0x1CE` and `0x1CF`; 0x5B to 0x62 → `0x1D0` to
`0x1D7`. Effects 0x5B to 0x62 have no show function: enemy shows call
`BtlEffectLoad` for them directly (`BtlShowADRun` (`0x8003DFE4`) 0x5B,
`BtlShowB2Run` (`0x8003EF4C`) 0x5C and 0x5F, `BtlShowB3Run` (`0x8003F2EC`)
0x5D and 0x60, `BtlShowAFRun` (`0x8003E6CC`) 0x5E and 0x61, `BtlShow9ARun`
(`0x8003A80C`) 0x62).

## The resource — `BtlEffectLoad`, `sprite_page_effect_load`, `sprite_palette_load`

`BtlEffectLoad(record, n)` (btl_show_frame.c) reads the resource to the
read buffer when its number is not 0, then acts on the flags:

| offset | field | read |
|---|---|---|
| +0x00 | u16 the sprite data's length, 4-byte units | flag 1: `xmalloc_heap_alloc(length × 4)` (`0x80022F4C`), the stream unpacked into it with `inflate_buffer` |
| +0x02 | u16 the packed sprite data's place, 4-byte units (5 in every resource, measured) | flag 1: the record pointed at the sprite data's tables, then `BtlRecordSpriteDataSet(record, data, 0x600, 0)` (`0x80089A3C`), which makes its draw word 0x2200; the record takes the acting slot's attribute word and place |
| +0x04 | 3 × {u16 length, u16 place}, 4-byte units, length 0 ending them | flag 1, `sprite_page_effect_load` (`0x80018AE0`): each unpacked and sent with `LoadImage`, 0x80 words (256 texels at 8 bits) by `length >> 6` lines, to `g_SpritePageCorners[7 + i]` = (576, 704, 832; 256) |
| +0x10 | u16 the palette's place, 4-byte units | flag 2, `sprite_palette_load(+0x10, 0x600)` (`0x80018BAC`): colour 0 zeroed, `LoadClut` to row 0x1F4 + ((0x600 >> 8) & 7) = 506 |

Flag 4 is the background layer (below). The texel records already name
page slots 7 to 9: nothing changes them, unlike resource 2's and the party
bundle's. Measured: all 64 flag-1 resources unpack to exactly their
declared length and read as sprite data. A block holds the caster's frames
as well as the effect's (effect 1 is Alex's whole technique).

## Which palette — the code's choice

`BtlRecordSpriteDataSet` gives the effect's record the draw word 0x2200
(row 506); `BtlShowOpen` then puts **0x2000 (row 504)** there, which in a
battle holds the party bundle's palette (resource `0x197`,
`BtlPartyDataLoad` (`0x80018A78`)). A show function that wants the
resource's own colours writes 0x2200 back. In station-tools' renders
effect 1's frames are right only under their own palette, and the flag-1
effects with no palette of their own (0x24, Healing Song, for one) only
under the party's.

The show functions that write 0x2200 into that record (`+0x50E` of
`g_BtlShowRecords` (`0x800B8FFC`)) themselves: `BtlShow01Run`
(`0x8007BE00`), `BtlShow02Run` (`0x8007C110`), `BtlShow03Run`
(`0x8007C450`), `BtlShow18Run` (`0x80080344`), `BtlShow1ARun`
(`0x80080BF8`), `BtlShow1CRun` (`0x80081218`), `BtlShow2ARun`
(`0x80082D9C`), `BtlShow2CRun` (`0x800839E8`), `BtlShow2DRun`
(`0x800845C4`), `BtlShow2ERun` (`0x80084E78`), `BtlShow2FRun`
(`0x80085194`), `BtlShow42Run` (`0x80086220`). `BtlShow1BRun`
(`0x80081068`, the show of 0x1B, 0x1F and 0x26), `BtlEffectCopyTargetPlay`
(`0x80085B74`) and `BtlEffectCopyAboveShotRun` (`0x80085D2C`) write it to
another record's +0x0E. Which word the other flag-2 effects end with goes
through callees and was not read. Effects 0x5B to 0x62 are not run through
`BtlShowNumberRun`, so `BtlShowOpen` never sets theirs: `BtlEffectLoad`
leaves it 0x2200, and whether their callers change it was not read.

## Background layers (flag 4)

Sixteen effects have one: 0x08, 0x0B, 0x1D, 0x1E, 0x27, 0x2A to 0x2C,
0x2F and 0x50 to 0x56 (battle.md's list of nine is the party's).

Loading, in `BtlEffectLoad`: `tile_layer_reset` (`0x8001D284`: the map
blanked by `tile_grid_clear` (`0x8001DC14`); `g_TileLayerShown`,
`g_TileEmptyFillOn`, `g_TileSemiTransOn`, the scroll and the layer wave's
amplitude and phase 0; `tile_col_set(0x808080)` (`0x8001D278`)), then
`tile_resource_load(n)`: the `g_TileResourceRows` row's resource read to
`0x80156000` and run by `resource_command_run` (`0x800202FC`), then
`g_TileTPageFirst` = 0x19. Then `func_8001DCDC`, `g_BtlEffectTileRow` =
`&g_BtlEffectTileRows[byte 1]`, `g_BtlTilePatternList` =
`BtlTilePatternListGet(its byte 0)`, `BtlTileScrollSet(8, 0, 0)`
(`0x8008A468`), `tile_grid_clear_copy(0, 1)` (`0x80089D58`) and
`g_TileLayerShown` = 1. A layer resource holds three command types and
nothing else:

| type, sub | handler | what it does |
|---|---|---|
| 4, 0x80 | `resource_palette_load` (`0x80020A08`) | unpacked; `palette_bit15_set(buf, 0x100, 0xFF)` (`0x800207E8`): colour 0 becomes the word 0 (not drawn) and every other colour gets bit 15; `palette_rows_set(1, 1, buf)` (`0x8002061C`): one row, row 0x1F4 + 1 = **501** |
| 3, 0x80 + n | `resource_tpage_load` (`0x8002097C`) | unpacked; `tpage_image_load(tile_tpage_get(n))` (`0x800204F0`, `0x8001DC94`): 0x80 × 0x100 words at texture page 0x19 + 2n (`tpage_position_set` (`0x800204D0`): (576 or 704, 256)); only 0x50 to 0x53 and 0x55 carry pages |
| 18, k | `resource_tile_pattern_set` (`0x80020C24`) → `tile_pattern_set` (`0x8001DB38`) | the payload's address into `g_TilePatterns[k]` (`0x800E0A00`): 16 slots, `g_TileWaveStrips` follows; 15 is the most a resource uses (measured) |

A frame: `BtlTilePatternPut(f, n)` (`0x80089C8C`) calls
`tile_pattern_put(f − 1, 0, 0, w >> 1, h >> 1)` (`0x8001DB6C`): `rows` runs
of `cols` u16 from `g_TilePatterns[f − 1]`, run i to map row i, into the
32 × 32 cell map (`tile_cell_address` (`0x8001D498`): `((y & 0x1F) << 5) +
(x & 0x1F)`). The record's width and height count 8-pixel units, so effect
8's 40 × 28 is 20 × 14 cells of 16 pixels, the whole 320 × 224. Escape
Song's `BtlTilePatternSizePut(k, 7, 7, n)` (`0x80089D00`) copies slot k
itself (no − 1), 7 × 7, its record's size. A payload rounded up to 4 bytes
carries a padding word nothing reads (0x1D, 0x27). Every sequence
(`g_BtlTilePatternLists`, and Escape Song's `g_BtlShow27PatternList`
(`0x80012EEC`): 1 2 3 4 5 4 3 2) names only slots its resource sets
(measured).

The drawer, `tile_layer_draw` (`0x8001D4E0`, tile_layer.c; rendering.md),
run by `tile_layer_step` (`0x8001DA78`) while `g_TileLayerShown` is 1 at
`(g_TileScrollX, g_TileScrollY)`: 15 rows × columns 0 to 0x14 from the
scroll's cell, the map wrapping at 32. A cell word:

* 0xFFFF: empty, not drawn; drawn as cell 0 while `g_TileEmptyFillOn` is
  set (only Dragon Grief sets it: `BtlShow08Run`'s `tile_empty_fill_set(1)`
  (`0x8001DD98`));
* u = (c & 0xF) << 4, v = c & 0xF0: 16 × 16 texels;
* pal = (((`g_TileTPageFirst` − 0x19) / 2) + 4 + ((c & 0x1FF) >> 8)) | 0x20
  = 0x24 + bit 8, since `g_TileTPageFirst` is 0x19 (`tile_resource_load`;
  `tile_tpage_get` and `tile_tpage_first_get` (`0x8001DC74`) only ever
  store 0x19);
* the texture page `g_TileTPages[pal & 7]` (`0x800AE0F8`): [4] = 0x99
  (576, 256), [5] = 0x9B (704, 256), 8-bit, blend rate 0 (`| 0x20`: rate 1
  when semi-transparent);
* the palette `g_TileCluts[pal >> 3]` (`0x800AE114`): [4] = 0x7D40, (0,
  **501**);
* 0x4000 and 0x8000: mirror across and down, a POLY_FT4 with the u or v
  run reversed (u + 15 to u − 1, the far edge held to 0 or 0xFF);
* bits 9 to 13 are not read; no retail cell sets one (measured).

So the page is the cell's bit 8 and the palette is always the layer
resource's row 501. Eleven of the sixteen effects carry no page: their
texels are the effect resource's pages, which flag 1
(`sprite_page_effect_load`) has just sent to slots 7 and 8, the same
(576 / 704, 256). The five flag-4-only effects send their own. Measured:
every cell of every frame, and cell 0 for `g_TileEmptyFillOn`, lands on
texels a loader filled. Rendered they are whole pictures: Dragon Grief's
dark ring, Ice Wall's rising pillar, the Black Dragon's purple sky and
lightning, Nitro Dagger's full-screen rings (mirrored quarters: the
mirror bits).

| effect | resource | flags | record w × h → cells | slots | sequence | page 0 / page 1 (source, lines) |
|---|---|---|---|---|---|---|
| 0x08 | 0x1D8 | 7 | 40 × 28 → 20 × 14 | 6 | 1 1 2 2 3 3 2 2 4 5 6 | effect 0x19F 256 / 252 |
| 0x0B | 0x1D9 | 7 | 12 × 22 → 6 × 11 | 12 | 12 11 … 1 | effect 0x1A2 256 / 240 |
| 0x1D | 0x1DA | 7 | 14 × 26 → 7 × 13 | 15 | 1-7, 9-15 | effect 0x1B4 256 / 24 |
| 0x1E | 0x1DB | 5 | 64 × 28 → 32 × 14 | 1 | 1 | effect 0x1B5 256 / 224 |
| 0x27 | 0x1DC | 5 | 14 × 14 → 7 × 7 | 9 | 1-9 | effect 0x1BE 256 / 64 |
| 0x2A | 0x1DD | 7 | 40 × 28 → 20 × 14 | 4 | 1 1 2 2 3 3 4 4 | effect 0x1C1 256 / 256 |
| 0x2B | 0x1DE | 5 | 40 × 28 → 20 × 14 | 6 | 1 1 … 6 6 | effect 0x1C2 256 / 164 |
| 0x2C | 0x1DF | 7 | 16 × 22 → 8 × 11 | 4 | 1-4 | effect 0x1C3 256 / 172 |
| 0x2F | 0x1E0 | 7 | 64 × 28 → 32 × 14 | 1 | 1 | effect 0x1C6 256 / 224 |
| 0x50 | 0x1E1 | 4 | 64 × 64 → 32 × 32 | 1 | 1 | layer 80 / — |
| 0x51 | 0x1E2 | 4 | 64 × 64 → 32 × 32 | 10 | 1-10 | layer 256 / 48 |
| 0x52 | 0x1E3 | 4 | 64 × 64 → 32 × 32 | 10 | 1-10 | layer 256 / 48 |
| 0x53 | 0x1E4 | 4 | 40 × 28 → 20 × 14 | 10 | 1-10 | layer 256 / 96 |
| 0x54 | 0x1E5 | 7 | 40 × 28 → 20 × 14 | 10 | 1-10 9 8 | effect 0x1CE 240 / — |
| 0x55 | 0x1E6 | 4 | 48 × 28 → 24 × 14 | 5 | 1-5 4 3 2 | layer 256 / 96 |
| 0x56 | 0x1E7 | 7 | 48 × 28 → 24 × 14 | 5 | 1-5 4 3 2 | effect 0x1CF 256 / 256 |

("—": no loader fills that page and no cell uses it; video memory there
holds an earlier scene's pixels.)

How it is drawn. At ordering-table entry 0x300, linked after the arena,
the `E1 0x220` mode and the overlay: over them and under every actor
(battle.md §"The screen effects, the pad and the sound of a presentation";
the Black Dragon stands in front of its own breath's sky). Window 8 blends
it: `g_TileSemiTransOn` = 1 makes it semi-transparent at rate 1 (B + F),
and every texel blends, since `palette_bit15_set` set bit 15 on all but
colour 0; `g_TileCol` modulates every texel (0x80 = as stored). Because
`tile_layer_reset` sets `g_TileCol` to 0x808080, a window-8 ramp started
after `BtlEffectLoad` begins at level 0x20 (`tile_mix_fade_start`
(`0x8001DDA4`): 0x80 >> 2, capped), so its first frames show the layer
opaque over a black arena. The layer's own wave (`g_TileWaveShown` on,
`g_TileWaveSize` the amplitude, `g_TileWaveStartRow` the phase:
`BtlShowWaveSet` (`0x8008A4F8`) and `BtlShowWaveStep` (`0x8008A5E4`) mode
8 → `tile_wave_switch_set` (`0x8001D304`) and `tile_wave_set`
(`0x8001D2F4`); Siphon Soul and Fate Storm) widens the columns to −2 to
0x18 and draws each tile as up to three sheared strips
(`tile_wave_strips_make` (`0x8001D310`), rendering.md); with amplitude 32
a row moves up to ±32 pixels. Under the wave a mirrored-down tile's strips
are each mirrored in place (vT = v + yo + h − 1, vB = v + yo − 1), not the
tile: only a strip that spans the whole tile shows it upside down. The
layer goes into the next frame's ordering table (`tile_layer_step` runs
after the flip).
