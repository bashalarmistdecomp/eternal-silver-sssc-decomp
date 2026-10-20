# Enemies on the map — roam, see, chase, touch, and after the fight

How a map object thinks, and how the enemies among them roam, see the
party, chase it, start a fight on touch, and what the fight's answer does
to them. The C is in map_think.c (the think pass), map_look.c (the looks
and the seen steps), map_btl.c (the touch scan and the fight) and
map_touch.c (the approach, the removal). The rows themselves, how a map's
objects are made from their definitions and the script side are in
[npc-scripting.md](npc-scripting.md); the frame these run in is in
[field-frame.md](field-frame.md).

Every statement here is a reading of the C. No emulator trace of a roaming
enemy or of a touch has been compared with the running game. The counts
marked "in the game" are counts over the map object definitions of the
game's data.

## The row and its definition

A map object is a 0x60-byte row (`MapObject`, include/map.h), one of the
0x80 at the address in `g_MapObjects` (0x801D6B80, set by `GameStart`
(`0x80055B60`)). Each row has a 0x26-byte definition
(`MapObjectDefinition`), entry row +1 of the table at the address in
`g_MapObjectDefinitions`. The fields this page uses:

| Row | Field | |
|---|---|---|
| +0x3C | `layer` | which attributes stop the row (0: any bit of 0x3800; 0x80: only those with bit 0x4000 as well) |
| +0x3D | `number` | low six bits: the think; bit 0x40: it turns one facing at a time |
| +0x3E | `facing` | 1 to 8, 1 toward smaller y, then clockwise; the odd facings are the four along the axes |
| +0x3F | `speed` | the pixels a walk goes in one think |
| +0x40, +0x41 | `interval_left`, `interval` | passes of the think pass between two thinks |
| +0x43 | `look` | number 6: the low seven bits pick the look |
| +0x44, +0x45 | `sight_width`, `sight_length` | how wide and how far a number 6 row sees |
| +0x46, +0x47 | `length`, `count` | a look's two values in place of drawn ones; 0 draws |
| +0x48 | `state` | where the look or the seen step is |
| +0x49 | `thinks_left` | the thinks left of a walk, wait or turn |
| +0x4A | `facing_kept` | a facing kept aside; a walk zeroes it when nothing more could be walked |
| +0x4B | `turn_plan` | a turn in steps under way |
| +0x4C | `target` | the spot walked to or away from, or (seen 3) the row that stopped the walk |
| +0x50 | `box` | the row's box: dx, dy, half width, half height |
| +0x54 | `move.bounds` | the roam rectangle, `left <= x < right`, `top <= ground y < bottom` |

| Definition | Field | |
|---|---|---|
| +1 | `sprite_data` | the sprite data number |
| +2 | `story_bit` | 0 for none |
| +4, +5, +6 | `x`, `y`, `facing` | where the object starts (x and y in units of 8 pixels, the facing less 1) |
| +0xB | `event` | a script label (kind 4) or an encounter (kinds 5 and 6) |
| +0xC | `number` | the row's think |
| +0xD | `speed` | the speed class |
| +0x13 | `touch_reach` | how far beyond its box the object touches the party, in units of 8 pixels |
| +0x14, +0x19 | `look_set`, `second_look_set` | five bytes each, copied to +0x43 to +0x47 |
| +0x1E | `seen` | low seven bits: the seen step; bit 0x80: a square sight |
| +0x1F, +0x20 | `seen_sight_width`, `seen_sight_length` | the sight once the row has seen the party |
| +0x21 | `seen_length` | the seen steps' value in place of a drawn one |
| +0x22 | `seen_speed` | the speed class once it has seen the party |
| +0x23, +0x24 | `seen_target` | seen 2: a cell; seen 3: an action and a sprite data number |
| +0x25 | `level` | the party's least level the object stands up to |

`MapObjectMoveSet` (`0x80065A2C`) copies the five look bytes from +0x14,
or from +0x19 when its flag is not 0, and takes `speed` and `interval`
from the tables `g_MoveSpeedPixels` and `g_MoveSpeedIntervals` at the
definition's speed class + 2 for a row of kind 4 to 6.

## The thinks: the table at `0x800B4658`

`MapObjectsThink` (`0x8006736C`) calls entry `number & 0x3F` of nine
function addresses at `0x800B4658` (the label `vtbl_4658_table` in the C is
the first split's):

| Number | Function | |
|---:|---|---|
| 0 | `MapObjectWanderStep` | the plain wander inside the bounds |
| 1, 2 | `StepCommandRun` (`0x80067768`) | the row's step list (+0x5C) |
| 3, 4 | `func_80067584` | returns 0; never called, as the think pass passes over both |
| 5 | `MapObjectTurnStep` | stands and turns |
| 6 | `MapObjectPartyLookStep` (`0x80077F1C`) | an enemy that has not seen the party: its look |
| 7 | `MapObjectPartySeenStep` (`0x80077FAC`) | an enemy that has seen the party: its seen step |
| 8 | `MapObjectPartyAwayStep` (`0x80078060`) | an enemy that turned away |

One pass goes over the 0x80 rows. A row is passed over when its kind
(byte 0) is 0 or 7, its number is 3 or 4, its drawn y is outside
`g_MapViewY − 0x64` to `+ 0x144` or its x outside `g_MapViewX − 0x64` to
`+ 0x1A4`, a script row holds it (bit 0x80 of byte 0), or it stands outside
`g_MapLimitLeft` to `g_MapLimitRight`, `g_MapLimitTop` to
`g_MapLimitBottom`. The phase counter `g_MapObjectsThinkPhase` steps 0, 1,
2, 3 once a pass: a row thinks only on passes of its own parity, and a row
of kind below 4 only when the counter equals `row & 3`. So a kind 4 to 6
row is visited every other frame. A visit counts `interval_left` down; at
0 it is loaded again from `interval` and the think is called. When the
think answers not 0 the row moved: `MapAttributeClassGet` puts the class of
its footprint in bits 0x6000 of +0xE and `MapAnimationPoseSet` follows. The
pass ends with the touch scan, `MapObjectTouchFind` (`0x8007AB64`).

A wait is always `N / interval` thinks (whole division, `value_div`), so it
lasts about `2 N` frames whatever the interval; below, a bare `N` stands
for that, and "+ 1" is one think more. In each think of numbers 6 to 8 the
look or seen step runs first and the sight check after it.

## The frame order

`MapFrameUpdate` (`0x80055840`, map_frame.c): `MapBgChangeStep`
(`0x80059A18`) → `MapPartyStep` (`0x80056248`) → `MapObjectsStep`
(`0x800563D4`), which is the think pass (with the touch scan as its last
statement) and then `MapObjectsAnimationStep` → `MapViewStep(1, 1)`
(`0x80068DB8`) → `MapObjectTouchRun` (`0x8007ACD8`), only while the s8
`g_MapObjectTouchOn` is not 0. `system_init` (`0x8001EEB8`, frame.c) sets
it to 1 and nothing else in the C writes it.

## Number 6: the looks

`MapObjectPartyLookStep` calls entry `look & 0x7F` of
`g_MapObjectPartyLookFunctions` (`0x800B5B3C`), 14 function addresses,
while no turn is planned; with a turn planned (`turn_plan` not 0) it calls
`MapObjectFacingTurnStep` (`0x80078D5C`) instead. `length` and `count` are
the definition's +0x17 and +0x18 (second set: +0x1C and +0x1D); 0 means
"draw it". The counts are of the 706 definitions in the game on number 6.

| Look | Function | What it does | In the game |
|---:|---|---|---:|
| 0, 13 | `MapObjectPartyLook00Step` | the plain wander (`MapObjectWanderStep`); never looks for the party | 0 |
| 1 | `MapObjectPartyLook01Step` (`0x80078E94`) | stands | 10 |
| 2 | `MapObjectPartyLook02Step` | stands; waits (`length` or 1..3) × 0x1E, then looks along (`count` or 3..5) facings drawn among the four odd ones (`draw & 3`), 0xF each, then takes its first facing again | 1 |
| 3 | `MapObjectPartyLook03Step` | walks back to its own cell (definition +4, +5), takes the definition's facing there, and becomes look 2. With its back exactly to the cell it gives up at once | 1 |
| 4 | `MapObjectPartyLook04Step` | patrol: takes the most open of the eight facings, walks (`length` or 8..16) cells, stops and makes (`count` or 3..5) × 2 quarter turns left and right (0xF each), walks on; turns about at a wall | 105 |
| 5 | `MapObjectPartyLook05Step` | walks along an odd facing drawn at random (never the reverse of its facing) for (`length` or 8..16) cells, draws again; at a wall turns to the side that is free (about, when both are or neither is), or always about with `count` set | 67 |
| 6 | `MapObjectPartyLook06Step` | as 5 (the free side, whatever `count`), then stops (`count` or 1..3) × 0x1E | 132 |
| 7 | `MapObjectPartyLook07Step` | as 6's walk, then look 2's turns in place of the stop | 240 |
| 8 | `MapObjectPartyLook08Step` | as 5, any of the eight facings | 16 |
| 9 | `MapObjectPartyLook09Step` | as 6, any of the eight facings | 83 |
| 10 | `MapObjectPartyLook0AStep` | one draw at the start (an odd facing, or an even one when the definition's +6 is odd), then walks for ever, turning to the free side at walls | 25 |
| 11 | `MapObjectPartyLook0BStep` | waits (`length` or 1..3) × 0x1E; pose 4 for 0x10; jumps to a random free place of its roam rectangle (`MapObjectRandomSpotPlace`, two draws a try); pose 5 for 0x10; pose 1 | 25 |
| 12 | `MapObjectPartyLook0CStep` | as 9's walk, then 11's pose 4 and jump | 1 |

A walk's length is `cells × 8 / speed` thinks. No walk leaves the roam
rectangle, enters a blocked cell or steps onto another row or the party
(`MapObjectOverlapFind` (`0x80067AC4`)). A row whose number has bit 0x40
(from bit 0x40 of the definition's byte 0; 250 of the 794 kind 5 rows in
the game) turns in steps, one facing every `5 / interval` thinks, and
`MapObjectFacingTurnStep` runs in place of its look meanwhile.

## The sight check

`MapObjectPartySeenCheck` (`0x8007A58C`) runs after the look or seen step,
every think. A row on look 0, or whose definition has `seen & 0x7F` of 0,
never looks.

* The area is round the row's x and ground y and is tested against the
  party's box (`MapBoxTouchGet` (`0x80067BFC`) must answer 9, an overlap;
  boxes that only touch on an edge do not count). For an odd facing it is a
  box ahead of the row, out to `sight_length × 8` pixels and
  `sight_width × 4` pixels to each side; for an even facing a slanted strip
  (`MapBoxDiagonalTouchGet`); with bit 0x80 of the definition's `seen`
  (579 of the 706) a square of half side `sight_width × 8` whatever the
  facing. On number 7 the sizes are the definition's +0x1F and +0x20; on
  number 8 it is a square of half side 0x60.
* Then the line from the row to the party must be clear:
  `MapAttributeLineCheck` (`0x80069C6C`) with the flag 1, which stops only
  at an attribute with a bit of 0x3800 and bit 0x4000 as well.
* Seen, on number 6: when the definition's `level` is below the party's
  least level (`g_PartyLevelLeast`), or its `seen & 0x7F` is 6, the row goes
  to number 8 and turns away, without a sound. Otherwise
  `SoundPlay(g_MapObjectPartySeenSounds[sprite_data − 0x1F], 2)` and the row
  goes to number 7. Either way the actuator list `g_MapActuator64List`
  starts when none runs, `speed` and `interval` become the tables' entries
  at the definition's +0x22 + 2, and `state` and `turn_plan` are zeroed.
  (`level` is 99 for 432 of the 706: those never turn away. Equal is not
  enough: 10 < 10 fails.)
* On numbers 7 and 8 the check only moves the state: seen, 0x80 to 0xEF
  becomes 0xFF; not seen, 0xFF becomes 0x80.

## Number 7: the seen steps

`MapObjectPartySeenStep` calls entry `(seen & 0x7F) − 1` of
`g_MapObjectPartySeenFunctions` (`0x800B5B74`), six function addresses.

| `seen` | Function | What it does | In the game |
|---:|---|---|---:|
| 1 | `MapObjectPartySeen01Step` | the chase: pose 2; while the party is in sight the goal is the first party record's place, taken every think; `MapObjectTowardWalk` (`0x800782B8`) walks `speed` pixels toward it (the step slides round corners; never the exact reverse of its facing; never out of its roam rectangle). It stops when its box overlaps the party's. Sight lost: it goes on to where the party was last seen, then waits (+0x21 or 0..2) × 0x1E + 1 thinks and settles | 609 |
| 2 | `MapObjectPartySeen02Step` | walks to the cell (+0x23, +0x24); there, with +0x23 not 0, reads the attribute under it (`MapAttributeFind`) and, when `attribute >> 7` is 4, runs script label `attribute & 0x7F` (`ScriptLabelRun` (`0x80059FDC`)) to its end within the think; settles | 5 |
| 3 | `MapObjectPartySeen03Step` | the charge: turns to the party, pose 3 for 0x1E, then walks straight along that facing (not at the party) until something stops it; waits ((+0x21 or 0..2) × 0x1E + 0xF) + 1 and settles. With +0x23 not 0, a row that stopped it and whose sprite data number is +0x24 is acted on: its number becomes 4 (+0x23 = 1) or it is turned to facing 1 (+0x23 = 2), and it is taken off the map, with its story bit set (`MapObjectOverlappedRemove` (`0x8007A458`)), when the charger's wait ends | 90 (30 with +0x23) |
| 4 | `MapObjectPartySeen04Step` | walks straight on, turning one facing toward the party every (+0x21 or 8..16) cells; settles when sight is lost and the count runs out | 1 |
| 5 | `MapObjectPartySeen05Step` | stands and faces the party; sight lost: waits (+0x21 or 0..2) × 0x1E + 1, takes its first facing again and settles | 1 |
| 6 | `MapObjectPartySeen06Step` | number 8's step (below) | |

Seen 2 reads the attribute with `MapAttributeFind` (`0x80069A20`), the
probe of the party's own cell events. That function first zeroes
`g_MapPartyHpTakeOn` and, while `g_MapPadDirectionsState` is 0, puts the
pad's direction row 0 back, and it acts on an attribute of kind 10 or 11
under the object's box as it would under the party's: the HP take and the
stairs' directions. The object's think comes after the party's step in the
frame, so the next frame's `MapPadStep` sees what it left.

"Settles" is `MapObjectNumberReset` (`0x8007A408`): the number is the
definition's +0xC again (bits 0xC0 kept), `state` 0, `MapObjectMoveSet`
with the flag 1 (the second look set, +0x19 to +0x1D, the same as the
first in 702 of 706; the bounds kept; the definition's own speed), pose 1.

## Number 8: away

`MapObjectPartyAwayStep` calls entry 5 of `g_MapObjectPartySeenFunctions`,
`MapObjectPartySeen06Step`, while no turn is planned. The row turns to the
reverse of the facing toward the party, pose 2, and each think
`MapObjectAwayWalk` walks `speed` pixels directly away from the party's
place (taken again while the party is in sight). Out of sight (a square of
0x60 round it): (+0x21 or 0..2) × 0x1E + 1 thinks more of walking away,
then it settles.

## Four things the code does that a reader would not guess

* A refused try does not put the place back. Both `MapObjectTowardWalk`
  and `MapObjectAwayWalk` take the row's place once a pass and hand
  `MapSpotMoveTry` the same spot for every shorter try, which it moves in
  place, so a refused try moves the next one's start. A chaser stopped by
  the party's box stays put (every further try is deeper in the box); a
  row walking away with 2 pixels of rectangle left takes none of them; one
  that starts on the party moves 14 pixels in a think of 4.
* `MapObjectOpenFacingFind` ends a whole ring at the first facing that
  leaves the roam rectangle, so facings later in the ring lose points for
  rings they were never tried in.
* Three steps read a register they did not set: `ret` in
  `MapObjectPartySeen02Step` and `MapObjectPartySeen03Step`,
  `turned_at_once` in `MapObjectPartySeen06Step`, when `MapFacingTowardGet`
  (`0x8005FBBC`) answers 0 (the row is on its goal, or its facing is 0). It
  is `$s1`. For the first two the caller `MapObjectPartySeenStep` has put
  the row's number there, so row 0 answers 0 and starts over while any
  other row goes on. For the third, `MapObjectPartyAwayStep` keeps its
  argument in `$s0`, and `$s1` is still the think pass's row address, never
  0.
* `MapObjectPartySeen03Step` indexes the row table with `id − 2`, where
  `id` is `MapObjectOverlapFind`'s answer (1 the first party record, row
  + 2 for a row), even when the id is 0 (a wall) or 1 (the party). See
  "Open points".

## The touch

`MapObjectTouchFind` (`0x8007AB64`, map_btl.c) runs once a frame, at the
end of the think pass. It does nothing while a touch is pending
(`g_MapTouchedObject` is 0 or more) or while `g_MapPartyBlinkCount` runs. A
row counts when its kind is 4, 5 or 6 and its number is not 3. Its box,
both half sizes grown by `8 ×` the definition's `touch_reach` (1 for 740 of
the 806 such rows in the game, else 0), must touch or overlap the party's
box (any answer of `MapBoxTouchGet` but 0: a touch on an edge is enough,
where the objects' own collision needs an overlap), and the straight line
from the row to the party must be clear on the attribute plane, with the
row's `layer` as the flag. The first such row in table order is written
to `g_MapTouchedObject`.

No facing is read, neither the party's nor the row's. There is no "from
behind", and nothing like it reaches the fight: `MapObjectTouchRun`
(`0x8007ACD8`) calls `MapBtlRun(definition +0xB, 0, 0, 0, 1, 1, 1)`
(`0x8007AE58`), six constants.

`MapObjectTouchRun` then:

1. Definition +0xB of 0: the touch is dropped (66 kind 5 rows, all 3 of
   kind 6).
2. `MapObjectTouchApproach` (`0x8007B310`, map_touch.c): the row's x,
   ground y, facing and the class bits 0x6000 of +0xE are kept
   (`g_MapTouchedObjectKeptX`, `g_MapTouchedObjectKeptY`,
   `g_MapTouchedObjectKeptFacing`, `g_MapTouchedObjectKeptClass`), and for
   0x20 frames (each a `MapFrameStep` (`0x800557D4`)) it walks one pixel a
   frame toward the party in pose 2, until its box, one pixel larger each
   way, touches the party's.
3. Kind 4: `ScriptLabelRun(+0xB)`, an event script label. Kinds 5 and 6:
   the party's ids (`g_PartyIds`) below 0xC fill `g_BtlSlotIds` in order
   and id 0xC goes to its slot 5; then the fight, encounter +0xB.

## After the fight

`BtlRun` (`0x80027434`) answers 0 for a win (also when a round limit runs
out), 1 when the party is down, and 2 when the party ran
(`BtlPartyEscapeTry`); by the register's reading of `BtlRun`, a fight lost
in which one of the party's slot ids was emptied also answers 2.
`MapBtlRun` makes encounters 0xF9 and 0xFE always count as 0.

* Won: `MapObjectTouchRemove(1, 1)` (`0x8007B5CC`). The row's scaled draw
  is switched on and its scale words go 0x100, 0xF4, … 0x1C over 0x14
  frames; then the row is freed (byte 0 = 0). When its definition's
  `story_bit` is not 0 (123 of the 794 kind 5 rows) the story bit
  `definition number + (g_MapNumber << 7) − 0x2C00` is set, the bit
  `MapObjectsInit` (`0x800655C8`) tests, and it is never made again. When
  `story_bit` is 0 (671) nothing is kept: the row is back the next time
  the map's objects are made, that is, when the map is entered again. No
  timer brings a row back. A seen 3 row whose definition has an action
  (+0x23), that holds the row that stopped it and that is in its wait
  (`state` below 0x80) takes that row off too.
  When the row was the one `g_MapObjectCursor` holds,
  `MapObjectCursorAdvance` (`0x8007B724`) moves the cursor on.
* Fled: `MapBtlRun` puts the row back where it was touched (before the
  0x20 frames of closing in): x, both y words, facing, class bits, pose 2.
  Its number and state are as the fight found them. `MapObjectTouchRun`
  sets `g_MapPartyBlinkCount = 0x12C`: `MapPartyAnimationStep`
  (`0x80065CF8`) counts it down by 1 a frame, in a field frame and in each
  `MapFrameStep`; while it is not 0 the touch scan finds nothing, and on
  odd counts the party is not drawn (150 of the 300 frames). Enemies still
  see, chase and block. `MapChange`, `GameEnter` and `MapPartyPlace` zero
  the count; `ScriptLabelRun` (script_run.c) keeps it in
  `g_ScriptKeptBlinkCount`, zeroes it for the script's run and puts it back
  after; `MenuRun` (`0x8006A024`, menu_run.c) keeps it, zeroes it and puts
  it back, and zeroes it after any request.
* Lost: `MapBtlRun` zeroes the first byte of all 0x80 rows and their twins
  and of the party's six records and twins, clears the party's ids, plays
  music 0x17 and opens the Continue page (`MenuNoWalkRun` (`0x80075BD8`)
  on page 0x14); `GameEnter` (`0x80059498`) enters the field from the game
  loaded.

`g_MapBtlWinForced` (s8; `system_init` sets 0 and nothing else in the C
writes it) changes the answers. In `MapBtlRun` any answer but 2 counts as
0, so a lost fight counts as won; `MapObjectTouchRun` then makes every
answer 0, so a touched row that fled is taken off as if won.

## What else starts a fight

Only the event script's battle command `ScriptCommandMapBtlRun`
(`0x8005DFF0`, lns_intp.c), entry 43 of `g_ScriptCommandFunctions`.
`BtlRun` has one caller, `MapBtlRun`, and that has two:
`MapObjectTouchRun` and `ScriptCommandMapBtlRun`. There is no step counter
and no random encounter in the field code. A script runs from a talk, a
map cell, a kind 4 row touched, or the cell a seen 2 row reaches. When a
kind 4 row's script fights, the touch is still pending: won, the command
calls `MapObjectTouchRemove` on that row; fled, `MapBtlRun` puts the row
back and the command leaves 0x12C in `g_ScriptKeptBlinkCount`, which
becomes the blink count when the script ends.

## Tables

| Symbol | Address | Type | Count | Read by |
|---|---|---|---|---|
| `g_MapObjectPartySeenSounds` | `0x800B5B8C` | `u16` | 56 | `MapObjectPartySeenCheck`: the sound of an enemy that sees the party, by sprite data number − 0x1F |
| `g_MoveSpeedPixels`, `g_MoveSpeedIntervals` | `0x80012C0C`, `0x80012C14` | `u8` | 8 each | `MapObjectPartySeenCheck` (entry definition +0x22 + 2), `MapObjectMoveSet` and the move and step commands |
| `g_MapFacingDxs`, `g_MapFacingDys` | `0x80012C1C`, `0x80012C24` | `s8` | 8 each | `MapSpotMove` (`0x80066848`) |
| `g_MapActuator64List`, `g_MapActuatorC8List` | `0x800AE34C`, `0x800AE33C` | `u8` | command lists | the actuator lists that a sighting (`MapObjectTowardWalk`, `MapObjectPartySeenCheck`) and the fight's start (`MapBtlRun`) give `pad_update` (`0x8001F508`) |

The two tables of function addresses lie inside one run of 28 at
`0x800B5B1C` (the first split's label `battle_state_table` is on it, and
its `battle_cmd_table` at `0x800B5B44` is inside the looks): entries 0 to 7
are `MenuUseRun`'s, entries 8 to 21 are `g_MapObjectPartyLookFunctions`
(`MapObjectPartyLook00Step` to `MapObjectPartyLook0CStep`, then
`MapObjectPartyLook00Step` again for look 13), entries 22 to 27
`g_MapObjectPartySeenFunctions` (`MapObjectPartySeen01Step` to
`MapObjectPartySeen06Step`). Neither index is bounded: a look of 14 or more,
or a `seen` of 0 or more than 6, calls whatever word lies there.

Sprite list entry 0x6C: `MapObjectPartySeenCheck` switches it off
(`sprite_run_off(0x6C, 1)`) when the row it looks at has nothing to look
with and is the row `g_MapObjectCursor` holds, and `MapObjectCursorAdvance`
switches two entries from 0x6C off (`sprite_run_off(0x6C, 2)`) when the
cursor comes round to the row it was given. `sprite_run_off` (`0x80019A7C`)
only zeroes list entries; neither plays a sound. What draws entry 0x6C, and
what the cursor is kept for, is not read.

## Open points

* The step above 4 pixels. A seen row steps `speed` pixels at a time
  through `MapSpotMoveTry` (`0x8006661C`); when the way wanted and both
  neighbours are blocked, `MapSpotSideMoveTry` (`0x800668A8`) looks for a
  way round. For a distance of 4 or more the C sets `cdA = 0` in both
  branches and never sets `cdB`, which it then counts down (map_walk.c). At
  a distance of 4 that cannot change anything, as the count only ever sets
  the step to 4, which it already is; above 4 it can. The word read is the
  one at 0x3C($sp), which is the footprint probe's 0x2C($sp), its saved
  layer mask, 0x4000 or 0, when the last probe at that stack depth missed
  its cache, else whatever was there. 290 of the 706 definitions step
  exactly 4 once they have seen the party; one steps more (map 120,
  definition 61: 8 pixels).
* `MapObjectPartySeen03Step`'s read in front of the row table. With a wall
  (id 0) or the party (id 1) the C reads the definition number of row
  `id − 2`, before the table at 0x801D6B80. The 0x100 colours that
  `sprite_reset` (`0x80018DBC`, sprite_list.c) copies to 0x801D6980 lie
  there, and the byte read is the high byte of colour 0xA0 or 0xD0, which
  `sprite_reset` zeroes (it zeroes every sixteenth colour), so the
  definition number read is 0. That rests on nothing else writing that
  buffer: no other store to it by address is in the C, but a store through
  a pointer was not ruled out. In the game's object sets the comparison then
  fails either way (definition 0's sprite data number is 0 in all four sets
  that use it). Were it to pass, the number byte that +0x23 = 1 writes
  would lie in front of the table as well.
* The scale words of a shrinking row: `sprite_scale_draw` (`0x80017988`)
  takes 0x100 as 1 and multiplies the piece's offset and size by the scale
  `>> 8`, so the row goes from its full size to 0x1C / 0x100 of it; that the
  row's words reach that call through the sprite list was not traced here.
