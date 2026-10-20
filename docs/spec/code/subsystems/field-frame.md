# The field's frame, and the walk

What one frame of the field does, in order, and the parts of it that decide
what a player sees and walks on: the pad and the walk, collision, the
position ring, the camera, the animation, scene changes and moves inside a
map, the bg change that opens doors, the fades, the party's shadow, the
idle status strip, the place name, and the way into a fight and back. The
code is in the `map_` files (map_frame.c, map_step.c, map_walk.c,
map_ring.c, map_view.c, map_change.c, map_fade.c ...), layer_col.c and
frame.c. The map objects and enemies are in
[map-enemies.md](map-enemies.md), the script and the object rows in
[npc-scripting.md](npc-scripting.md), the drawing of the layers in
[rendering.md](rendering.md).

Every statement here is a reading of the C. No emulator trace of the field
has been compared with it. Counts marked "in the game" are counts over the
data of the game's 97 maps.

## One field frame

The game's loop `GameRun` (`0x800559F4`, map_frame.c) calls, until
`g_GameEnded` is set, `MapFrameUpdate` (`0x80055840`), `sprite_list_draw`
(`0x8001A798`) and `MapFrameEnd` (`0x8005596C`).

| # | Call | What |
|---:|---|---|
| 1 | `MapBgChangeStep` (`0x80059A18`) | one frame of the bg change (§"The map's tile animation" below) |
| 2 | `MapPartyStep` (`0x80056248`) → `MapPadStep` (`0x80055EC4`) | the pad poll: the d-pad's four bits become an index 1..8 (a facing, clockwise from up; opposite pairs and three or four held give 0) and the answer is `g_MapPadDirections[index]`, the direction to try |
| 3 | → `MapPartyWalkStep(dir)` (`0x800664D4`), up to `g_MapPartyWalkStepCount` times | one step of the lead each; the loop stops at the first that answers 0 |
| 4 | → `MapAttributeRun` (`0x800590C0`) | the map-cell event under the lead, and its dispatch (npc-scripting.md) |
| 5 | → the buttons in `g_MapPadPressedButtons` | `0x40` talk or examine (`MapPartyTalkCheck` (`0x80067D1C`)); `0x4` or `0x8` the status strip; `0x80` or `0x100` the menu (`MenuRun(2)` (`0x8006A024`)); pad bit `0x20` released, the save page (`MenuSaveRun` (`0x80072890`)). All skipped while `g_DebugAllowed` is set and `0x100` is held; `system_init` sets that byte to 0 and nothing else writes it |
| 6 | → `MapPartyAnimationStep` (`0x80065CF8`) | the party's animation tick |
| 7 | `MapObjectsStep` (`0x800563D4`, map_step.c) | the map objects: the think pass `MapObjectsThink` (`0x8006736C`), with the touch scan at its end, then their tick and draw `MapObjectsAnimationStep` (`0x80065FD8`) |
| 8 | `MapViewStep(1, 1)` (`0x80068DB8`) | the camera snapped to its goal, then the scroll set |
| 9 | `MapObjectTouchRun` (`0x8007ACD8`) | while `g_MapObjectTouchOn` is set: the touched object's event or fight (map-enemies.md) |
| 10 | (`g_GameEnterRequest` set) | a script's request: music 0x17, the Continue page (`MenuNoWalkRun` on page 0x14), `GameEnter` with its answer |
| 11 | `palette_animation_step`, `tile_animation_step`, `MapStatusDisp` (`0x80056408`) | the palette and tile animations, the status strip |

`MapFrameEnd` then runs the tail: `layer_col_fade_step` (`0x8001E7B0`),
`bg_draw` (the two tile layers and the text grid), `palette_update`,
`tile_animation_load`, `frame_end` (`0x8001F30C`, the present),
`frame_start`, `sound_music_level_step`, the play time's frame count, and
last `MapFrameCount` (`0x800566AC`, map_step.c).

The pad word's d-pad bits: `0x1000` up, `0x2000` right, `0x4000` down,
`0x8000` left (`MapPadStep`'s switch).

## The walk's rules

Speed and the lead's box: `MapPartyBoxSet` (`0x8006497C`). Only "kind 0 or
not" is tested: `g_MapPartyKind == 0` (the world-map areas) gives one step
a frame and the box `{dx 0, dy 0, w 6, h 4}`; any other kind gives two
steps and `{0, −1, 8, 5}`. Kinds 1 and 2 walk alike. A step is one pixel
on each axis of the facing, so a diagonal step is one pixel on both.

Collision: `MapAttributeFreeCheck(x, y, w, h, layer, flag)` (`0x800692D4`).
The box is the point plus `(dx, dy)`, plus and minus `(w, h)`. The lead
probes with `layer` 0, and then every attribute with a bit of 0x3800
blocks: bit `0x4000` is not consulted for him. `0x3800` is a full block;
`0x0800`, `0x1000`, `0x2000` and `0x1800` are the four slopes, each let
through only in one corner attribute of the box and only on its open
side. Attributes outside 0..255 read 0, so the plane's edges stop nobody;
the game's maps wall their edges.

The step: `MapSpotMoveTry(dir, dist, &x, &y, box, layer, flag)`
(`0x8006661C`). It tries the facing asked, then the facing one clockwise,
then one anticlockwise, each from the same start. With all three blocked,
`MapSpotSideMoveTry` (`0x800668A8`) sends two probes along the obstacle
both ways (until their counts reach 0x41 pixels) looking for a place from
which the step is free, and the lead moves one pixel toward it: this is
what carries him through a doorway he is not lined up with.

Slides are remembered: `MapPartyWalkStep` (`0x800664D4`). A step that came
out as a neighbouring facing is kept in `g_MapPartySideStepFacing`; while
it is, a slide the opposite way is refused. A step along the facing asked
clears it. This is what stops the lead jittering in a corner.

Facing is a byte of the position ring's head entry: the direction moved;
with a fully blocked step, the direction asked; with a place a map object
refused (`MapPartyObjectsFreeCheck` (`0x800679F4`)), the direction that
step would have moved.

The d-pad map changes under the lead. `g_MapPadDirections` is nine u16
filled from one of the six rows of `g_MapPadDirectionsRows`
(`MapPadDirectionsSet(n)` (`0x80066F04`)). Row 0 answers what was pressed.
The attribute probe `MapAttributeFind` (`0x80069A20`), which
`MapAttributeRun` calls each frame, puts row 0 back first (unless the box
of script command 0x56, `ScriptCommandMapPadDirectionsChange`, is in
force: `g_MapPadDirectionsState` not 0) and lets an attribute of kind 11
turn right and left into diagonals for the next frame: this is how stairs
are walked.

The position ring: `MapPartyRingPush` (`0x80067114`), `MapPartyRingFollow`
(`0x800671C4`). 0x60 entries of `{x, ground y, height, facing, class}` at
the address in `g_MapPartyRingEntries`. Each accepted step pushes the
lead's place; party member k is put 16·k entries behind the head. The
lead's drawn y is ground y minus the entry's height.

The camera: `MapViewGoalGet` (`0x80069080`). The lead is 0xA0 from the
view's left and 0x78 from its top (so 0x68 from its bottom: the view is
0x140 × 0xE0). On each axis the low clamp is tested first: below
`g_MapLimitLeft + 0xA0` the camera is `g_MapLimitLeft`; above
`g_MapLimitRight − 0xA0` it is `g_MapLimitRight − 0x140`. The clamp is the
scene's clip rectangle, not the map's edge. The lead's x and y are read as
u16, so a lead left of or above 0 takes the high clamp.
`MapViewScrollSet` (`0x80068E98`) adds the shake's offsets and calls
`bg_scroll_set` (`0x800145BC`).

## The animation

The field record (`MapRecord`) has the battle record's layout (`+0x28`
animation table, `+0x2C` step list, `+0x30` animation number, `+0x34`
timer, `+0x36` length, `+0x38` step, `+0x3A` end) and the same tick
(`MapPartyAnimationStep` against `BtlAnimationStep` (`0x800272A8`)), but a
different setter: `MapAnimationSet` (`0x800663EC`) does not restart the
animation when the old and new numbers are both in 1..60 and in the same
group of four, that is, when only the facing changed. The step is kept and
the timer too, so a turn does not reset the walk cycle. The battle's
`BtlAnimationSet` (`0x80027208`) always restarts.

An animation number is `4 × (pose − 1) + 1 + column`
(`MapAnimationPoseSet` (`0x8006633C`)): column 0 down (facing 5), 1 up
(1, 2, 8), 2 left (6, 7), 3 right (3, 4). `MapPartyRingFollow` asks for
pose 1 whenever a step is tried, and `MapPartyAnimationStep` ticks every
frame whether the pad is held or not: nothing in the walk path stops the
cycle when the lead stands. The walk cycle of a party sheet in the game is
four steps of 10–11 field frames on poses "step, stand, step, stand"
(Alex: 7, 8, 7, 6).

## Scene set-up

`MapChange(idx, ...)` (`0x8005922C`) reads an 11-byte row of
`g_MapEnterTable` (`MapEnterRow`, include/map.h): the destination map
`+0`, the object set `+1`, the fade `+2`, a sound `+3`, the clip rectangle
`+4..+7` (`x0 = +4 << 3`, `x1 = (+5 + 1) << 3`, `y0 = +6 << 3`,
`y1 = (+7 + 1) << 3`), the arrival point `+8`, `+9` (`<< 3`) and the
facing `+10 + 1`. After the fade out it sets the map (`MapSet`
(`0x80068780`)), copies the bg changes whose story bits are set, puts
d-pad row 0 in force, makes the map's objects (`MapObjectsInit`), places
the party (`MapPartySpotSet` (`0x80064B8C`)), clears the bg change's
record, `g_MapBgChangeNumber` and the blink count, sets the clip
rectangle and snaps the camera.

`MapPartyPlace(idx, ...)` (`0x80059730`), what a map-cell event of kind 2
runs (a door or a stair), reads a 9-byte row (`MapPartyPlaceRow`) of the
table at the address in `g_MapPartyPlaceRows` (0x801E7080, 0x580 bytes
after `g_MapEnterTable`'s 0x801E6B00; `GameStart` (`0x80055B60`) stores
both): a byte `+0` that it only hands on, as the third argument of the
draw area's close and open, neither of which reads it (all 932 rows of the
97 maps hold 0 there), a sound `+1`, the clip rectangle `+2..+5`, the new
position `+6`, `+7` and the facing `+8 + 1`. The same order of work: place
the party, set the rectangle, snap the camera, with the draw area closing
before and opening after ("A door is not a fade" below).

## The map's tile animation, called the bg change: how a door opens

A map copies a rectangle of its own cells from a rectangle elsewhere in
the same map, in the tile plane and in the attribute plane at once (the
glossary's bg change; the queued tile uploads of `g_TileAnimations` are
something else, in rendering.md §"Queued VRAM cell uploads"). A map-cell
event of kind 3 starts one; so does script command 40
(`ScriptCommandMapBgChangeStart`), with a delay.

The table is at the address in `g_MapBgChangeTable` (0x801E7500,
`GameStart`), 0xA00 bytes after the scene table; its rows are
`MapBgChangeRow` (include/map.h):

| Offset | What |
|---|---|
| +0x000 | 128 u16, stored high byte first (`u16_bytes_swap` (`0x80013EA0`) swaps them): word `id` is the row's start in 16-bit units from the table; 0 means no such change |
| row +0, +1 | the top-left of the rectangle written, in attribute cells (8 pixels) |
| row +2, +3 | its width and height |
| row +4 | a story bit, 0 for none; the bit's number is +4 + 0x480 |
| row +5 | how many source pairs follow |
| row +6 | the frames each pair lasts |
| row +7 | a sound, when not 0: `SoundPlay(+7, 3)` (`0x800555F0`) |
| row +8.. | count × {u8 source x, u8 source y}, attribute cells |

In the game there are 853 rows on 66 of the 97 maps; no row has a count of
0 or an odd coordinate.

The functions (map_change.c unless said):

| Function | What |
|---|---|
| `MapAttributeRun` case 3 (map_attribute.c) | an attribute of kind 3 starts change `number & 0x7F`, only while the record is free (`g_MapBgChange.number == 0`) or the one on it has not started (`delay` not 0) |
| `MapBgChangeStart(id, delay)` (`0x80059900`) | nothing if `id == g_MapBgChangeNumber` (the change last started); a row with a story bit sets it (`StoryBitSet` (`0x800576EC`)) and does nothing if it was set already; fills the one record `g_MapBgChange` (`MapBgChangeRowSet` (`0x800598AC`)), so one change runs at a time, and with delay 0 asks for the first copy and plays the sound |
| `MapBgChangeStep` (`0x80059A18`) | the step: a start delay counts down first; else the pair's count of frames goes up and, once it reaches row +6, the next pair is taken and its copy asked for; after the last pair the record is free |
| `MapBgChangeCopy` (`0x80059B0C`) | makes the copy the record asks for: `bg_both_copy(dest, source, w, h)` (`0x800153A0`): the attribute plane cell for cell, the tile plane with all six values halved (bg_tile.c, as are `bg_copy` and `bg_attribute_copy`) |
| `MapBgChangeStoryBitsCopy` (`0x80059B6C`) | on entering a map: for every row whose story bit is set, its last pair is copied at once, so a door or wall a story bit has opened is open again on the next visit |

The frame's order: `MapBgChangeStep` is the first call of the frame; the
walk and its cell-event dispatch come next; the copy `MapBgChangeCopy` is
the first statement of `MapFrameCount`, the last call of the tail, after
`bg_draw` has read the tile plane for this frame's picture. So a copy
asked for in frame F is seen, and walked on, from frame F + 1. A change a
cell starts in frame F with n pairs of d frames each copies pair k at the
tail of frame F + k × max(d, 1), and the record is free in the step of
frame F + n × max(d, 1).

The callers clear the latch: `g_MapBgChangeNumber = 0` after a kind 2 move
(`MapPartyPlace`) and on a scene change (`MapChange`, `GameEnter`
(`0x80059498`), which also zero the record's `number` and `copy_due`).

### A town door, worked on map 31 (Vane)

The threshold is two kind 3 cells. Each door is one pair, delay 0, sound
16, no story bit: one copy of 4 × 4 attribute cells (2 × 2 tiles) from a
strip at x 1232..1263 pixels, outside the scene's clip rectangle where the
camera never goes. The copy writes 0 over the two kind 3 cells and makes
the doorway's top row kind 2 cells whose number is the move into the house:

| Door | Threshold cells | Written | From | The doorway becomes |
|---|---|---|---|---|
| 1 | (51-52, 82) | (50, 80) 4×4 | (154, 0) | kind 2, number 1 → (200, 1624) |
| 2 | (87-88, 82) | (86, 80) | (154, 4) | number 2 → (104, 1912) |
| 3 | (15-16, 56) | (14, 54) | (154, 8) | number 3 → (168, 1224) |
| 4 | (37-38, 58) | (36, 56) | (154, 12) | number 4 → (616, 1784) |
| 5 | (57-58, 54) | (56, 52) | (154, 16) | number 5 → (616, 1560) |
| 6 | (83-84, 54) | (82, 52) | (154, 20) | number 6 → (920, 1560) |
| 7 | (105-106, 56) | (104, 54) | (154, 24) | number 7 → (616, 2008) |
| 8 | (129-130, 58) | (128, 56) | (154, 28) | number 8 → (776, 1336) |
| 9 | (72-73, 30) | (72, 28) 2×4 | (154, 32) | kind 4, number 10: the map's script label 10, not a move (the map has no move 9) |
| 11 | (27-28, 71) | (26, 68) | (154, 40) | number 10 → (1000, 1784) |

Rows 10 and 12..16 of map 31 exist but no cell names them; what starts
them was not traced.

What the map's data shows about those doors:

- The doorway is walkable before the copy. Above each of Vane's
  thresholds the doorway column is 0x8000 cells (walk-behind class, no
  wall bits) up to the roof's top edge (door 1: cells (51..52, 68..81)),
  and door 11's two cells above its threshold are 0x0000. Nothing stops
  the walk there: the copy makes the doorway's top row the move before
  anyone arrives.
- Every house door is the same closed door: rows 1..8 and 11 each copy
  over tiles 0x204A 0x204B / 0x0058 0x0059 (the top two with bit 13, so
  `bg_first_draw` queues them again as walk-behind copies). Row 9's
  rectangle is 16 x 32: 0x22E6 / 0x02EC.
- Which room each door leads to: move 4 holds counter label 88 (shop 11),
  move 5 label 89 (shop 12), move 6 label 90 (shop 13), move 7 label 87
  (shop 10); move 8 is the library (labels 20..59).
- The Magic Guild's door (row 9) brings kind 4 cells of label 10, whose
  script is one command 0x26 with table index 5 (`26 00 0b 00 05 01 ff
  00`): Vane's entry 5, map 33 arriving at (504, 640).
- The Guild's way out is not its kind 1 cells: entry 4's cells (cells
  (62..63, 83), x 496..511, y 664..671) are walls (0xB884) the walk never
  enters. The two cells above them hold kind 5 label 120, whose script is
  one command 0x26 with table index 4 (`26 00 00 00 04 00 ff 00`): Vane,
  arriving at (584, 256).
- With every story bit clear, a guard stands in that doorway: map 33's
  set, definition 1 (kind 2, sprite data 151, story bit 78, number 3) at
  (504, 632), the 16-pixel passage between the hall and label 120. From
  inside the Guild nobody gets past him; arriving from Vane puts the party
  below him.
- The Guild's doors and moves out of its hall (entry 1's clip, x 0..1064,
  y 224..680): kind 3 doors 1, 4, 16 and kind 2 moves 1, 2, 3, 14. Mia's
  room (move 2's clip) leaves by label 24 (kind 4; its script only sets
  story bits: 0x27, 0x47, 0x45, END) and move 4, to (72, 568) in the hall.

### The door's sound

On the field `g_SoundMapNumber` is the map's number (`SoundMapSet((u8)map)`
(`0x800555CC`), from `MapSet`; npc-scripting.md §"Map set-up"), which
decides what a sound 0xB..0x82 plays (`SoundPlay`, music_voice.c): with a
value other than 0 or 0x32, ids 0xB..0x18 are sequence `id − 3` of the
boot bank (resource 232, slot 0), ids 0x19..0x1E play nothing (the
sequence number passes the slot's 0x15), and ids 0x1F..0x82 are refused
and play kind 1 number 1, sequence 1, in their place. A door's sound 16
is therefore sequence 13 of resource 232. (Maps 0 and 0x32 behave as a
battle does.)

Not read: who clears story bits 0x480 and up; map 31's script label 10.

## Screen fades and scene transitions

### There is no fade layer: four layer records

Four records of 0x24 bytes (`g_LayerBgFirstRecord` (`0x800AE16C`) and the
three after it) are stepped once a frame by `layer_col_fade_step`
(layer_col.c), the first call of the frame's tail `MapFrameEnd`, and each
ends in one thing a renderer reads:

| Record | Mask bit | Ends in | Read by |
|---|---|---|---|
| `g_LayerBgFirstRecord` | 4 | the tiles' colour word, scratchpad `0x1F800044` (`bg_col_set` (`0x80014014`), bg_tile.c) | `bg_first_draw`, OR-ed into every tile packet |
| `g_LayerBgSecondRecord` | 8 | the second layer's colour word `g_BgSecondCol` (`bg_second_col_set` (`0x800148C8`), bg_tile.c) | `bg_second_draw` (`0x800148E0`) |
| `g_LayerSpriteRecord` | 0x100 | the sprites' colour word `g_SpriteCols[0]` (`sprite_col_set` (`0x80016A74`), sprite_draw.c) | the sprite packets (`sprite_plain_draw` (`0x80016CF0`), sprite_draw.c) |
| `g_LayerFlatRecord` | 0x80 | a full-screen rectangle, added | `layer_flat_draw` (`0x8001DEF4`, layer_col.c): POLY_F4, semi-transparent, draw mode `0xE1000220` (blend mode 1), ordering-table entry 0x40 |

A colour word is `0x00BBGGRR` and multiplies the texels: 0x80 a channel
leaves them as they are, 0 is black, above 0x80 brighter (the multiply and
its cut at full are the console's). A channel is
`(layer_col_conv(colour number) * record value) >> 7` (`layer_col_conv`
(`0x8001E050`), `layer_col_apply` (`0x8001E10C`)), the colour being the
map's own `g_MapCol` (0 gives 0x80), so with a colour of 0 the word is
the record's value three times. With a layer mix (rendering.md) the tiles'
and the second layer's words are further scaled by their weights / 16, and
shifted by 8 instead of 7 in mode 5.

Bit 0x80000 of a fade's mask names all four records (`layer_col_fade_start`
adds 0x18C), and bit 4 brings in the flat (0x80) as well.

The image holds the four records idle (step number 2, length 1) and at 0,
and the display switch `g_ScreenDispMask` at 0: the program starts black
with the display off, and a fade in is what first shows a picture.

### The 25-frame ramps

`layer_col_fade_start` (`0x8001EA74`, layer_col.c) starts a ramp and
`layer_col_fade_record_set` (`0x8001E55C`) seats it:
`value = from + (target − from) * step / n`, C's division, step 1..n.
`from` is the record's last target, not its value now: a ramp cut short
jumps. A target of 0 is the map's colour `g_MapCol`, −1 the three numbers
at `g_LayerColLowest` (black), −2 those at `g_LayerColHighest` (white).
Every field ramp is 0x19 = 25 frames:

```
out (0x80 -> 0):  123 118 113 108 103 98 93 88 82 77 72 67 62 57 52 47 41 36 31 26 21 16 11 6 0
in  (0 -> 0x80):    5  10  15  20  25 30 35 40 46 51 56 61 66 71 76 81 87 92 97 102 107 112 117 122 128
```

The sprites are one frame behind the tiles: their packets are built
(`sprite_list_draw`) before the tail steps the records, the tiles' after
it. That is why a fade out runs two frames past its ramp.

| | Function | Frames |
|---|---|---|
| the fade out | `MapFadeOut(mask, 0x19, target, ..)` (`0x800568D4`, map_fade.c): it waits for a running ramp, ramps all four records to the target (−1 black, −2 white), lets two more frames go by, then switches sprite list entries 0..0x95 off | 25 + 2 |
| the fade in | `MapFadeIn(mask, 0x19, ..)` (`0x800569E0`, map_fade.c): a ramp to the map's own colour; the display switch goes on after its first frame; then all four records are set to the map's colour at once | 25 |

The mask is `g_MapLayersMask | 0x130`; `g_MapLayersMask` is 4 on the field
(`MapLayersInit` (`0x80068678`), map_load.c). A fade to white runs the first
three records to 0xFF and the flat to (255, 255, 255); no scene row in the
game asks for it (below).

A fade's frame is `MapFrameStep(a, b)` (`0x800557D4`, map_frame.c): the
script's moved rows (`MoveChrUpdate`), the party's and the objects'
animation ticks, the camera `MapViewStep(a, b)`, the palette and tile
animations, the sprite pass and the tail. It is not the field loop's
frame: no pad, no walk, no think pass, no bg change step, no cell event.

### A scene change: the row's fade byte

`MapChange` (map_change.c) plays the row's sound first (`+3`, channel 3,
when not 0); then, only while `g_MapFadeState` is 0 (a script that has
faded out already skips the block):

```
fade byte +2:  1 -> the fade out goes to -2 (white)
               2 -> g_MapFadeState = 2: the party-lit fade, out and in
               else the plain fade to -1 (black)
MapSeek(map, music)            the destination's music and the disc's seek
MapFadeOut                     the fade out: 27 frames
MapSet .. MapViewStep(1, 1)    the new map is loaded and set up: no frame is presented
MapFadeIn                      the fade in: 25 frames
```

`MapSeek` is `0x8006891C`. Of the 428 scene rows on the 97 maps in the game
the byte is 0 in 425 and 2 in three: map 31 entry 4 → 24 (Vane's
pentagram to the Transmission Spring), map 24 entry 9 → 107, map 107 entry
20 → 24. No row has 1.

The party-lit fade (`MapFadePartyOut` (`0x80056AEC`) out, `MapFadePartyIn`
(`0x80056CB8`) in, map_fade.c), 112 frames out and 86 in:

```
out  actuator list g_BtlActuatorToggleList
     party records and twins: +0xE |= 0x6000   drawn at 0x808080 whatever the sprites'
                                               colour word is, at ordering-table entry
                                               0x20 + (0xFF - screen y) / 8: in front of all
     25 frames   the world ramps to black (all four records: the 0x100 the fade
                 takes out of the mask comes back with 0x80000); the sprites'
                 mix switched on
     MapPartyPose4Play: pose 4, sound (0xE, 1), 60 frames
     the sprites' mix levels set (0x100: 0, 0x400: 0xC, 0x800: 0x12), the mix off
     25 + 2 frames, black                      MapFadeOut's own ramp, from 0 to 0
in   the sprites' mix on, levels 0x1F
     party records and twins: +4 = 0 (not drawn), +0xE |= 0x6000, pose 0xD
      1 frame    black, no party
     MapPartyOnPlay: +4 = 1, sound (0xE, 1), 60 frames: the party alone on black;
                 then pose 1
     25 frames: the world ramps in
     the mix off; +0xE takes the ring entry's class bits back
```

`MapPartyPose4Play` is `0x80068334` and `MapPartyOnPlay` `0x80068400`
(map_load.c). Both fades also call `layer_mix_fade_start` and
`layer_mix_fade_check` with the sprite masks 0x100, 0x400 and 0x800;
outside a battle those two act on mask bits 4 and 8 alone, so these calls
change nothing and take no frames.

### A door is not a fade: the draw area

`MapPartyPlace` (map_change.c), a map-cell event of kind 2, plays the
row's sound first (`+1`, channel 3), then closes the draw area, moves the
party, and opens it. The draw area is the rectangle at `g_ScreenDrawAreaX`
that the present makes the frame's drawing area (`screen_draw_area_set`
(`0x8001F488`), frame.c; `frame_end`, frame.c → `packet_draw_area_add`
(`0x80015E24`), packet_shape.c → `SetDrawArea` at entry 0x3FF, the first
drawn); outside it the frame buffer keeps the swap's clear, black.

```
MapDrawAreaClose (map_fade.c), frame k = 1..25, s = 26 - k:
      x = 160 - 6s, y = 112 - 4s, w = 12s, h = 8s      (10, 12, 300, 200) ... (154, 108, 12, 8)
  frame 26: the rectangle (0, 0, 319, 223); tiles and sprites black at once; display on
  frame 27: the display off (SetDispMask(0) at that frame's present)
the move (in MapPartyPlace): no frame
MapDrawAreaOpen (map_fade.c):
  frame 1: an empty rectangle (160, 112, 0, 0), the frame's tail alone
           (MapFrameEndCopy: no sprite pass), display off
  then tiles and sprites back at once, display on, and for i = 0..24:
      x = 159 - 6i, y = 111 - 4i, w = 12i + 1, h = 8i + 1    (159, 111, 1, 1) ... (15, 15, 289, 193)
  it returns with (0, 0, 319, 223) set and no frame of its own
```

`MapDrawAreaClose` is `0x80056F5C`, `MapDrawAreaOpen` `0x800570D8` and
`MapFrameEndCopy` `0x800568AC`. Both do their work only while
`g_MapFadeState` is 0. 53 frames. 6 and 4 are `0xA0 / 25` and `0x70 / 25`,
whole. The rectangle a door leaves is 319 x 223, one column and one row
short of the 320 x 224 the image starts with, and nothing else sets it
again. The second layer's word is not blacked by the closing (the mask
0x134 has no 8). Story bit 0x331 is cleared by the opening.

### The sounds

| Sound | When |
|---|---|
| (scene row `+3`, 3) | first thing of a scene change, when not 0: 24 of the 428 rows in the game (id 13: 20, id 14: 2, id 137: 2) |
| (move row `+1`, 3) | first thing of a door, when not 0: 330 of the 932 rows (id 13: 281, id 137: 46, id 14: 2, id 17: 1) |
| (0xE, 1) | the party-lit fade: with the leaving pose (out) and when the party is drawn again (in) |

### The present, and what is not established

`frame_end` (`0x8001F30C`, frame.c), in order: the draw area's
`SetDrawArea` packet; `DrawSync(0)`, left out when `g_ScreenMode` is 3;
`VSync(0)`; the count of vertical blanks the frame took; `pad_frame_step`
(the pad is read); `ResetGraph(1)`, every frame; the profile bars when
shown; `GsSwapDispBuff`; `SetDispMask(g_ScreenDispMask)`; and `GsDrawOt`
of the table `frame_start` kept. Sony's side of those calls is in
[psyq/libgpu-libgs.md](psyq/libgpu-libgs.md).

* One frame of latency. The present swaps buffers and then queues the
  table just built, so the console shows frame k's picture from present
  k + 1 while the display switch acts at present k. A view that paints
  frame k at frame k shows the same pictures one frame early.
* The sprites' mix (`sprite_piece_draw` (`0x80019DBC`), sprite_list.c):
  with the switch `g_SpriteMixOn` on, a sprite whose group (bits 0xC00 of
  its entry's word at +0xA) has a level in `g_SpriteMixLevels` gets a
  second copy of its packets, drawn with the palette at VRAM row 509
  (`0x1FD`), whose colours are
  all `g_SpriteMixCol`. The party-lit fades turn the switch on; the colour
  is 0 from `sprite_mix_init` and only battle code sets anything else, so
  what the copy shows on the field is not established.
* What the console does with the numbers: the multiply's cut, the blend of
  an added rectangle, dither. PS1 hardware.

## The party's shadow

Each party place has two records: its member's in `g_MapPartyRecords` and a
twin in `g_MapPartyTwins` (0x3C bytes each, `MapRecord`). The twin is a
grey blob from a sprite data of its own, at the member's ground place, on
the member's animation, sorted behind every body: a ground shadow
(assets/sprites.md).

* Set-up: `MapPartyRecordInit` (`0x800649DC`, map_party.c), right after
  the member's record: the record is copied whole over the twin,
  `MapRecordInit(twin, 1, 0, m)` (`0x80065940`) (+4 = 1, +0xC = 0x20,
  +0xE = 0x2000, +0x30 = 0xFFFF, the sprite data's tables bound),
  `MapAnimationPoseSet(twin, 1, 1)`. `m` is
  `g_MapTwinSpriteDataNumbers[number − 1]` for a number below 0x1F, else
  0xD. The twin is switched off when `g_MapPartyKind == 0` (a world-map
  area) or the member's sprite data number (the number, plus 0xF in a kind
  0 area) is 0x1F or more. So: live in towns, interiors and dungeons, for
  numbers below 0x1F.
* Place: `MapPartyTwinSpotSet` (`0x800672E8`, map_ring.c), called by
  `MapPartyRingFollow` for each live place right after the member is put
  on its ring entry: x = the entry's x, both y words (+8 and +0xA) = the
  entry's ground y (the member is drawn at y − height), the class into
  +0xE's 0x6000 bits, `MapAnimationPoseSet(twin, 1, facing)`.
* Animation: `MapPartyAnimationStep` (map_animation.c), right after the
  member's tick: a live twin (its own +0 is tested) is put on the member's
  animation number with `MapAnimationSet`, on the twin's own tables, its
  step set to the same distance from the animation's end
  (`end − (member.end − member.step)`), the member's timer and length
  copied. The twin is never ticked itself. Its image goes to sprite list
  entry 0x46 + i with that step's piece.
* Draw: `sprite_draw` (`0x8001A528`) gives entries 0x46..0x6D the depth
  0xFF: entry 0x2FF of the ordering table for class 0x2000 or 0x4000,
  behind every body (a body's entry is 0x200 + 0xFF − (ground y − scroll y
  + 0x20)). `sprite_list_draw` draws those entries first. Its draw word is
  0x2000 | class and its +0xC 0x20: not semi-transparent. The blink after
  an escape hides both entries; the party-lit fades write the twin's +4 and
  +0xE as they write the member's.
* After a fight: `sprite_init_copy` (`0x80063B10`), from `MapBtlRun`
  (`0x8007AE58`, map_btl.c), runs `sprite_init`, which loads palette rows
  4 and 5 again (resources 0xE5 and 0xE6, to rows 0x1F8 and 0x1F9 of video
  memory), the sprites' field colours, which the battle had taken.

## The idle status strip

The party's names, levels, HP and MP on the bottom rows of the 40 × 28
cell grid, the same grid the message window draws (menu-ui.md §"The
field's status strip — `MapStatusDisp`").

Three globals. `g_MapIdleCount` (u16) the idle count, `g_MapStatusRequest`
(u16) the request, `g_MapPartyHpTakeWait` (u8) the kind 10 countdown. The
request lives one frame: raised by the pad poll or a button, acted on by
`MapStatusDisp` at the end of `MapFrameUpdate`, cleared by `MapFrameCount`
(map_step.c), the last call of the tail. What it draws stays on the grid
(nothing redraws the strip every frame) until a clear.

When it goes up:

| Where | File | What |
|---|---|---|
| `MapPadStep` | map_step.c | the d-pad map's answer is 0: the count goes up while below 0x12C; the call that reaches 0x12C = 300 raises request 1. It then stays at 300 (no second request) |
| `MapPartyStep` | map_step.c | the buttons that went down this poll are exactly 0x4 or exactly 0x8: count = 0x12C, request 1, at once |
| `MapPadStep` | map_step.c | while `g_MapPartyHpTakeOn` is set (an attribute of kind 10 under the lead) and `g_MapPartyHpTakeWait` is 0: `MapPartyHpTake` (`0x80059E84`) (each member's HP less the attribute's number, at least 1 left; a red colour at once; `g_MapPartyHpTakeWait = 0x1E`), count = 0x12C, request 1; the wait is then taken down by 1 each call (0x1D after the take) and the map's colour put back below 0x1D |
| script command 0x58 | lns_intp.c | `ScriptCommandPartyMpHalve` (`0x8005ECC8`) sets the count to 0x12C (no request) |

When it comes down: `MapPadStep`: the d-pad answer is not 0 and the count
is not 0 → count 0, request 2, unless `g_MapPartyHpTakeOn` is set and
`g_MapPartyHpTakeWait` is 0xF or more. `MapStatusClear` (`0x8005A7C8`,
message_disp.c): when the count is 0x12C or more or a request is pending,
count 0, request 2, `MapStatusDisp` at once and one frame. Its callers: the
message command (`ScriptCommandMessageDisp` (`0x8005AB1C`), and through
`ScriptMoveEndWait` (`0x8005A508`)), both item notices
(`MessageNoticeNumberDisp` (`0x8005BA70`), `MessageNoticeItemDisp`
(`0x8005BB6C`)), `ScriptCommandShopRun` (`0x8005E384`),
`ScriptCommandPartyHpMpFill` (`0x8005E3D0`), `ScriptCommandPartyMpHalve`
and others, so a window never opens over the strip. `MapObjectTouchRun`
zeroes the count after a touch (map_btl.c).

What it draws: `MapStatusDisp` (map_step.c). Any request but 0:
`CellRowsClear(0x16, 4, 0x20)` (`0x800573E0`) takes the cells of kind below
3 off rows 0x16..0x19. Request 1 then, for party place 5 down to 0 with a
number 1..0xB, at column x = 0x20 for the first found and 6 less for each
further one (the strip fills from the right):

| Row | Cells | What (atlas glyph codes, after `CellGlyphSet` (`0x80057500`)'s + 1) |
|---|---|---|
| 0x16 | x .. x+3 | the name: `CellChrGlyphSet(number, ..)` (`0x8005754C`), codes (number − 1) × 4 + 0x10 .. + 0x13 (number 0xB is drawn as number 1) |
| 0x17 | x, x+1 | 0xB, 0xC: "LV" |
| 0x17 | x+2, x+3 | +0x1C, the level, 2 digits |
| 0x18 | x+1 .. x+3 | +0, HP, 3 digits, colour `MapStatusColGet(+0, +2)` (`0x80056674`) |
| 0x19 | x+1 .. x+3 | +4, MP, 3 digits, colour `MapStatusColGet(+4, +6)` |

then, left of the last member (x already 6 less), 0x5D 0x5F on row 0x18
("HP") and 0x5E 0x5F on row 0x19 ("MP") at x+2, x+3. A number is
`CellNumberSet` (`0x80057400`): `fmt_sprintf("%10d")`, the last n
characters, a blank left as the cell was; a number of more than 3 digits
loses its high ones. Every cell is kind 1 (`ankprint_glyph_set`
(`0x80025DC0`)), 8 × 8 from the UI atlas (`ankprint_glyph_draw`
(`0x80025B90`)), at ordering-table entry 0x30. Colours: names, labels and
the level 0x3000 → sub-palette 14; `MapStatusColGet(cur, max)` gives
0x2000 when equal → 13, 0x3000 when cur > max >> 1 → 14, 0x1000 when
cur > max >> 2 → 12, else 0 → 11. No band: the strip is cells on the map,
no rectangle behind them. Request 2: `func_800572D8`, an empty function;
the clear is all.

For one member the strip is columns 0x1C..0x23 of rows 0x16..0x19: pixels
(224..287, 176..207), the bottom right.

## The place name

The field shows no place name on arrival. `MapSet` (`0x80068780`,
map_load.c, every map entry) stores the map record's byte 6 in
`g_MapPlaceName` and, for 0x51..0x64 (the towns), sets story bit
number + 0x28A, and draws nothing. The readers of `g_MapPlaceName`:
`MenuCardListDisp` (`0x800734F0`)'s save and load page header
(menu_card.c, `font_string_proportional_put(g_MapPlaceName + 0x400)`
(`0x80027054`)); the save block (`MenuSaveWrite` (`0x80074024`),
menu_card.c) and the memory card file's title (`card_save_header_set`
(`0x800253C8`) → `card_save_title_set` (`0x8001BF50`), outline.c, from the
short names `g_CardSavePlaceNames[number − 0x51]`); the load (`GameEnter`,
map_change.c) and script command `ScriptCommandPlaceNameSet`
(`0x8005E9D4`, lns_intp.c), which set it. String `0x400 + number` is also
printed by `MenuUsePlaceNamePut` (`0x80076E14`, menu_list.c) for a list of
places and by the save list (menu_card.c). The scene change `MapChange`,
the field entry `GameEnter` and the field frame print no string. A map's
event script can still print a name with its message command; that is the
script's data, not the engine.

## Into a fight and back

`MapBtlRun(btl_number, place_x_offset, label, return_mode, view_follow,
second_view_follow, music_number)` (map_btl.c), the battle's wrapper. A
touched enemy calls it as `(definition +0xB, 0, 0, 0, 1, 1, 1)`
(`MapObjectTouchRun`).

In, frame by frame from the touch (after the row's 0x20 frames of closing
in, `MapObjectTouchApproach` (`0x8007B310`)):

1. The actuator list: `g_PadActuatorList = g_MapActuatorC8List`,
   `g_PadActuatorRestFlags = 0x3F`, `g_PadActuatorListWait = 0`. The pad
   read `pad_update` (`0x8001F508`, pad_update.c) plays it every poll;
   `BtlRun` (`0x80027434`, btl_run.c) zeroes those words as it starts.
2. The party's eight place bytes copied for the battle;
   `sound_btl_seek` (`0x80024044`) = `resource_seek(0x166)`, a seek of the
   drive with nothing read. No picture.
3. `MapFadeOut(g_MapLayersMask | 0x130, 0x19, -1, 1, 1)`: frames 1..25 the
   four records go to black, frames 26 and 27 black; each a
   `MapFrameStep(1, 1)` frame, the map, the party and the rows still
   animating, no pad, no walk. Then sprite list entries 0..0x95 off. 27
   frames. No flash, no swirl: nothing else draws.
4. `g_MoveChrLiveBits` kept and cleared, the heap (`xmalloc_heap_reset`),
   the tile data's slots (`bg_slot_reset`), `SoundMapSet(0)` (sound map 0).
5. The music: `MusicPlay(music_number, 0, 0)` (`0x80054F2C`), 1 from a
   touch, after the picture is black (`MusicPlay` itself waits on the old
   track's fade).
6. `BtlRun(btl_number)`, the battle: it zeroes the actuator words,
   switches the display off, sets the battle up, switches it on.

Back:

1. `bg_slot_reset`, `PartyLevelLeastSet` (`0x80065028`); encounters 0xF9
   and 0xFE, and any answer but 2 while `g_MapBtlWinForced` is set, count
   as 0 (won).
2. `layer_col_set(0, g_MapLayersMask | 0x80130, -1)` (`0x8001EB74`): every
   record black at once; `sprite_init_copy` (palette rows 4 and 5);
   `MapLayerOrderSet` (`0x8006869C`).
3. Unless `return_mode` is 2 or the party is down with no label:
   `MapReload` (`0x800686EC`), the area's sound map, its music
   (`MapMusicSet(g_MusicNumber, ..)` (`0x80058EE4`)) and what the map's
   record sets.
4. `MessageReset` (`0x80059F7C`, the message window's reset); sprite list
   entries 0..0x95 off.
5. Party down (answer 1, `label` 0): `MapDataReload` (`0x80068748`); music
   0x17; `FaceListIdMove(0xC, 0)` (`0x80076328`); the six party records,
   their twins and the id list zeroed; the 0x80 object rows freed; the
   map's colours `g_MapCol`, `g_MapColSecond`, `g_MapColThird` = −0x38;
   `MapFadeIn(0x30, 5, ..)`: 5 frames in to a darkened, empty map; then the
   Continue page (`MenuNoWalkRun` (`0x80075BD8`) on page 0x14) and
   `GameEnter` enters the field again.
6. Otherwise: the heap, `g_MoveChrLiveBits` back; unless `return_mode` is
   2: `MapSpriteDataReload(0)` (`0x800644E0`, the party's sprite data
   loaded again), `MapBgChangeStoryBitsCopy` (the bg changes of set story
   bits copied), `MapViewStep(1, 1)` (the camera), and after a flee the
   touched row back where it was touched; `MoveChrSpriteDataSet`; with
   `return_mode` 0, `MapFadeIn(g_MapLayersMask | 0x130, 0x19, 1, 1)`: 25
   frames from black to the map's own colour.
7. Back in `MapObjectTouchRun`: a won fight's row shrinks for 0x14 frames
   (`MapObjectTouchRemove` (`0x8007B5CC`)), after the fade in; a fled one
   starts the 300 frames of blinking.

Not read: what the battle draws in its own first and last frames; how many
frames `MusicPlay` waits for the old track; whether the battle clears the
cell grid (and with it a status strip that was up).

## Two paths with no defined behaviour

Both are in the original.

- `MapSpotSideMoveTry` with a distance of 4 or more: probe B's countdown is
  never set (the C writes probe A's a second time; B's stack slot keeps
  what it had, read at `0x800669E8`) and is then counted down. The lead
  always passes 1; the map objects' walks (map_look.c) pass their speed
  (map-enemies.md, "Open points").
- `MapSpotSideAlignCount` (`0x80066E64`) with a facing outside 1..8 leaves
  its result unset (its switches have no default). Not reachable while the
  d-pad rows hold 0..8.

## Where the code is

| Function | Address | File |
|---|---|---|
| `MapPartyStep` | `0x80056248` | map_step.c |
| `MapPadStep` | `0x80055EC4` | map_step.c |
| `MapPadDirectionsSet` | `0x80066F04` | map_walk.c |
| `MapPartyBoxSet` | `0x8006497C` | map_party.c |
| `MapPartyWalkStep` | `0x800664D4` | map_walk.c |
| `MapSpotMoveTry` | `0x8006661C` | map_walk.c |
| `MapSpotSideMoveTry` | `0x800668A8` | map_walk.c |
| `MapAttributeFreeCheck` | `0x800692D4` | map_over.c |
| `MapAttributeFind` | `0x80069A20` | map_over.c |
| `MapAttributeClassGet` | `0x8006960C` | map_over.c |
| `MapPartyRingPush`, `MapPartyRingFill`, `MapPartyRingFollow` | `0x80067114`, `0x80067064`, `0x800671C4` | map_ring.c |
| `MapAnimationPoseSet`, `MapAnimationSet`, `MapAnimationStart`, `MapPartyAnimationStep` | `0x8006633C`, `0x800663EC`, `0x80066468`, `0x80065CF8` | map_animation.c |
| `MapViewGoalGet`, `MapViewStep`, `MapViewScrollSet` | `0x80069080`, `0x80068DB8`, `0x80068E98` | map_view.c |
| `bg_scroll_set` | `0x800145BC` | bg_tile.c |
| `MapChange`, `MapPartyPlace` | `0x8005922C`, `0x80059730` | map_change.c |

Related pages: npc-scripting.md (the object rows, the script interpreter,
the moved rows), rendering.md (the tile layers and the sprite renderer),
graphics.md (the map resource and the attribute plane's bits),
assets/sprites.md (the party's field sheets).
