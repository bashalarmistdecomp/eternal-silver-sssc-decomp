# Format: PS1 palettes (CLUTs) and the game's palette rows

This page covers the PlayStation's 16-bit colour word, the sizes of its
palettes (CLUTs), and how this game fills its palette rows in video
memory: which resource each row comes from, which function sends it, and
what the game does to the colours on the way.

## The 16-bit colour

Each colour is 2 bytes, little-endian:

```
Bit 15:     STP (semi-transparency flag)
Bits 14-10: Blue  (5 bits, multiply by 8 for 8-bit range)
Bits 9-5:   Green (5 bits, multiply by 8 for 8-bit range)
Bits 4-0:   Red   (5 bits, multiply by 8 for 8-bit range)
```

Value `0x0000` = black, and the one word the GPU does not draw (R=0, G=0, B=0, STP=0).
Value `0x7FFF` = white (R=248, G=248, B=248, STP=0).

**The low five bits are red.** A 16-bit direct-colour TIM pixel and a CLUT
entry are the same word.

## CLUT sizes

| Mode | Colors | Bytes | Usage |
|------|--------|-------|-------|
| 4-bit | 16 | 32 | one 16-colour CLUT |
| 8-bit | 256 | 512 | one 256-colour CLUT |
| a palette row | 256 | 512 | one 8-bit CLUT, or sixteen 4-bit CLUTs side by side |

A palette word (Sony's `GetClut`) is `(y << 6) | (x >> 4)`: `0x7E00` is
(0, 504), `0x7EC7` is (112, 507).

## The game's colour conventions

The game makes the word 0 its only transparent colour: index 0 of a CLUT
(or of each 16-colour group) is forced to 0, and every other colour is
kept off 0. It does so in four ways, by loader:

* `palette_bit15_set(cols, n, mask)` (`0x800207E8`): colour i becomes 0
  when `i & mask` is 0, and gets bit 15 set otherwise. With mask `0xFF`
  over 256 colours: colour 0 is 0, every other colour has STP set.
* `palette_zero_fix(cols, n, mask)` (`0x80020830`): colour i becomes 0 when
  `i & mask` is 0; any other colour stored as 0 becomes 1 (drawn black).
* `sprite_init` (`0x80018BFC`) on resources `0xE5` and `0xE6`: colour 0 is
  set to 0; colours 1 to 255 have their two bytes exchanged and bit 15 set.
  Those two resources are stored byte-swapped with STP clear.
* The battle's, the effects' and resource 2's loaders
  (`BtlPartyDataLoad`, `sprite_palette_load`, `sprite_reset`) only zero
  colour 0 (or colour 0 of each 16) and send the rest as stored.
  `BtlDataLoad` zeroes nothing: it sends its two rows as stored, colour 0
  included.

Colour 0 being the only hole is the field's transparency rule
([../code/subsystems/rendering.md](../code/subsystems/rendering.md)
§"The field's draw rule"); there is no colour key.

## The palette rows

Rows 500 to 511 of video memory, x 0, hold every palette the game draws
with. `palette_rows_load(row, h, cols)` (`0x80020588`) sends h whole rows
to (0, 500 + row); `palette_row_part_load(index, n, cols)` (`0x800205CC`)
sends n colours to (index & 0xF0, 500 + (index >> 8)).
`palette_rows_set` (`0x8002061C`) sends rows and, for rows 500 to 507, also
copies them into the palette buffer at `0x801D5B80` (eight rows of 256
colours), whose address `g_PaletteBuf` (`0x800B8ADC`) holds.

| row | what | loaded by | resource |
|---|---|---|---|
| 500 | the map's tile palette, first half | `resource_palette_load` (`0x80020A08`), type 4 sub 0: `palette_zero_fix` over 512 colours, rows 500–501 | each map |
| 500 | the battle arena's palette | `BtlDataLoad` (`0x80018694`): LoadClut of header word 8's place | the battle set, `0x167`–`0x196` |
| 501 | the map's tile palette, second half | as row 500 | each map |
| 501 | an effect's background layer | `resource_palette_load`, type 4 sub `0x80`: `palette_bit15_set` | `0x1D8`–`0x1E7` |
| 502 | the icons | `resource_palette_row_load` (`0x80020ACC`), type 12 n = 2: `palette_bit15_set` | 1 |
| 503 | the portraits | type 12 n = 3, the same | 1 |
| 504 | the field sprites (draw word `0x2000`) | `sprite_init`, byte-swapped as above | `0xE5` |
| 504 | the battle's party | `BtlPartyDataLoad` (`0x80018A78`): the place in the resource's first word; colour 0 zeroed | `0x197` |
| 505 | the field sprites (draw word `0x2100`) | `sprite_init`, byte-swapped | `0xE6` |
| 505 | the battle's enemies | `BtlDataLoad`: header word 9's place | the battle set |
| 506 | the map's object palette (draw word `0x2200`) | `resource_palette_load`, type 4 sub 1–`0x7F`: `palette_bit15_set` | each map |
| 506 | an effect's own palette | `sprite_palette_load` (`0x80018BAC`) from `BtlEffectLoad` flag 2: word 8's place, colour 0 zeroed | `0x198`–`0x1D7` |
| 507 | sixteen 4-bit CLUTs: the common sheet, the text | `sprite_reset` (`0x80018DBC`): word 0's place, colour 0 of each 16 zeroed; also copied to row 7 of `g_PaletteBuf` | 2 |
| 507 | the text (at boot) | type 12 n = 7: `palette_bit15_set`, then `palette_glyph_zero_set` (`0x8002087C`) zeroes colours `0xB0`, `0xC0`, `0xD0`, `0xE0` | 1 |
| 508 | the second tile layer's sixteen 4-bit CLUTs | `palette_bg_second_row_set` (`0x80020670`), from type 4 sub 0: the second 256 colours, bit 15 set, colour 0 of each 16 zeroed | each map |
| 510–511 | the cover's copy of the tile palette | `palette_bg_cover_rows_set` (`0x800206D4`), from type 4 sub 0: the first `g_PaletteBgCoverCount` colours (8 or 10, set by `MapLoad`) of every 16 zeroed | each map |

Where a row has two loaders, the second line is the battle's or the later
one. `main` runs `sprite_reset(0)` and then `resource_load_run(1)`, so at
boot row 507 holds resource 1's colours; `sprite_init`, which
`sprite_init_copy` (`0x80063B10`) runs from `GameStart` and from
`MapBtlRun` after every battle, ends with `sprite_reset(0)`, so from the
game's start row 507 holds resource 2's. The two rows differ in
51 colours, none in CLUTs 11 to 14 (the glyphs').

Only seven calls of Sony's `LoadClut` exist in the game's C: three in
btl_draw.c (`BtlDataLoad` twice, `BtlPartyDataLoad`) and four in
sprite_list.c (`sprite_palette_load`, `sprite_init` twice, `sprite_reset`).
Every other row is sent with `LoadImage` through `palette_rows_load`.

## The per-frame upload

`palette_update` (`0x80020134`), called once a field frame from
`MapFrameEnd`, sends every row of `g_PaletteBuf` whose mark byte
`g_PaletteRowMarks[r]` is set (by `palette_row_mark(index)`
(`0x80020118`), row = index >> 8), clearing the mark. While the byte
`g_PaletteAnimationCopyId` is not 0 it also copies `count` 16-colour CLUTs
from row 1 of the buffer, with bit 15 set and colour 0 of each zeroed, to
row 508 every frame: the palette animations of the tile palette kept in
step on the second layer's row. `palette_animation_set` (`0x800262FC`),
which `MapLoad` calls, starts a map's palette animations, and
`palette_animation_step` (`0x800263F4`) changes colours of the buffer's
row 1 once a frame and marks the row: mode 1 copies a frame of colours
from `g_PaletteAnimationCols` (`0x800B8B04`, resource 1's type-1 block +
`0x599C`), modes 2 and 3 move the colours one place up or down.

## Where each asset's palette is

* **Map tiles**: rows 500/501 (first layer), 508 (second layer), 510/511
  (the cover) — [../graphics.md](../graphics.md).
* **Field sprites**: rows 504 (party, kind 1), 505 (kind 2), 506 (the
  map's own), 507 (chests, kind 7) —
  [../assets/sprites.md](../assets/sprites.md) §"Map object sets".
* **Portraits**: row 503 — [../assets/sprites.md](../assets/sprites.md)
  §"Portraits by face number".
* **Bromides and other TIMs**: the CLUT inside each TIM, sent where the TIM
  says ([../sprites.md](../sprites.md) §"Pictures: the TIMs").
* **Battle sprites**: rows 500 (arena), 504 (party), 505 (enemies), 506
  (effects), 507 (the common sheet) —
  [../code/subsystems/battle.md](../code/subsystems/battle.md)
  §"Battle graphics — the set bundle, the party bundle, the arena".
* **The UI**: rows 502, 503, 507 — [../assets/battle-ui.md](../assets/battle-ui.md).
