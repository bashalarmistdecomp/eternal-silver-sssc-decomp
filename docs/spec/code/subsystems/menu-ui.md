# Menu/UI system

This page covers what the game draws over play and how the player drives it:
the battle's command menu, its windows, the party panel, the damage numbers
and the battle's messages; the field menu opened from the map (the party's
commands, magic, items, gear, status, the command set-up, the formation,
saving and loading, the options); the inventory and spell checks those
menus ask; the field's status strip; the shop; the title screen and its
menu; and the text, glyph and icon layer all of them draw with. A last part
holds code first read beside the menus that serves other parts: the random
numbers, the face slots, the layer records, the moving outlines and the
presentations' records. The battle menu's code is in `btl_menu.c`,
`btl_command.c`, `btl_window.c`, `btl_panel.c`, `btl_amount.c` and
`btl_message.c`; the field menu's in the `menu_*.c` files; the inventory's
in `chr_item.c` and `chr_worn.c`; the shop's in the `shop_*.c` files; the
title screen's in `start_screen.c`; the text and icon layer in
`ankprint.c`, `font_*.c`, `icon.c` and `cell_text.c`. Everything here is
read from the C and the register unless a statement says otherwise.

Four dispatch tables below are reached through labels the first split
named and nobody has read (`src:inherited` in the register): the field
menu's page table at `0x800B59F8` (`battle_phase_pre_table`, which runs on
into `battle_init_table` at `0x800B5A24`), the shop's step table at
`0x800B3258` (`entity_ai_table`), and the field menu's uses at
`0x800B5B1C` (`battle_state_table`). The C indexes them by those names; the
names say nothing of what the tables are. The battle menu's step table is
`g_BtlMenuStepFunctions` (`0x800B312C`); the first split's labels
`entity_type_table` (`0x800B3130`) and `entity_update_table`
(`0x800B3144`) fall inside it, at its entries 1 and 6.

## The battle menu

### The step table

`BtlMenuRun` (`0x80044118`) asks the player for the party's commands for one round. Its
one caller is `BtlRun` (`0x80027434`), once a round while `g_BtlPartyMode` is not 1 (at 1,
the auto-battle, `BtlRun` calls `BtlPartyActionSet(1)` (`0x8004D2C4`) in its place). It
switches off the two window boxes (`BtlWindowBoxesOff` (`0x80049B3C`)), puts the menu at
its start (`BtlMenuReset` (`0x80044430`): sound request, cursor, acting slot 0, step 1,
every party slot's plan reset to kind 0xB, number 0, target 0, and the
pending mask made with `BtlMenuPendingMaskGet` (`0x80049A0C`)), and, when the mask is not
0, opens the menu (`BtlMenuOpen` (`0x800445B8`)), calls entry `g_BtlMenuStep` of
`g_BtlMenuStepFunctions` and one menu frame (`BtlMenuFrameStep` (`0x80048964`)) until the
step is 0, and closes it (`BtlMenuClose` (`0x800446B4`)). A step does its work and hands
over by storing another step number; 0 ends the loop. After it `BtlRun`
calls `BtlPartyEscapeTry` (`0x80027DAC`) when `g_BtlPartyMode` is 2.

| step | handler | what |
|---|---|---|
| 0 | `BtlNothingRun` (`0x80036830`) | an empty function, never called (the loop stops at 0) |
| 1 / 2 | `BtlMenuFirstOpen` (`0x80044780`) / `BtlMenuFirstStep` (`0x80044B5C`) | the first menu: AI, Command, Tactics, Run |
| 3 / 4 | `BtlMenuPresetOpen` (`0x80044FA4`) / `BtlMenuPresetStep` (`0x8004584C`) | Tactics, the preset rows |
| 5 / 6 | `BtlMenuKindOpen` (`0x800459D4`) / `BtlMenuKindStep` (`0x80045EDC`) | one member's command grid |
| 7 / 8 | `BtlMenuSpellOpen` (`0x80046434`) / `BtlMenuSpellStep` (`0x800469AC`) | the spell list |
| 9 / 10 | `BtlMenuItemOpen` (`0x80046C98`) / `BtlMenuItemStep` (`0x800472B4`) | the item list |
| 11 / 12 | `BtlMenuTargetOpen` (`0x80047714`) / `BtlMenuTargetStep` (`0x80047D90`) | the target window |
| 13 / 14 | `BtlMenuWalkOpen` (`0x8004810C`) / `BtlMenuWalkStep` (`0x800483D4`) | the place a kind-5 action walks to |

An odd step sets a menu up and hands over to the next; an even step runs
once a frame and reads the pad. The steps test CROSS and TRIANGLE by
equality on the pressed-or-repeating word of the second pad record
(`g_MenuPadRecordRepeated[0]`, the word at +4 of `g_MenuPadRecord`):
exactly 0x40 is CROSS and exactly 0x10 TRIANGLE, so either counts only as
the one bit set. CROSS is tested first, TRIANGLE second and a direction
(`g_BtlMenuPadRecord[2]`, +4 of the first record, `& 0xF000`) last. A pad
record (`PadRecord`, pad.h) holds the buttons held now at +0, the ones
newly pressed at +2 and the ones pressed or repeating at +4.
`BtlMenuOpen` makes both records ready (delays 0x10 and 0x20) and
`BtlMenuFrameStep` brings them up to date each frame.

**The menu's frame — `BtlMenuFrameStep`.** One frame: both pad records
updated; a pending sound request played (`SoundPlay(g_BtlMenuSoundRequest,
3)` (`0x800555F0`)) and cleared; the menu's records stepped into the sprite list
(`BtlAnimationStep(g_BtlMenuCopyRecords, 0x1C, 6)` (`0x800272A8`): the five copies and
the first mark record into entries 0x1C..0x21; `BtlAnimationStep(
g_BtlMenuMarkRecords + 0x40, 0x6E, 3)`: the other three into
0x6E..0x70); the two windows moved one step (`BtlWindowsMoveStep` (`0x8004A350`)); and
the battle's frame (`BtlFrameStep` (`0x80038D2C`)). `BtlWindowsMoveWait` (`0x80048A3C`) runs it until
both windows' moving bytes (`g_BtlWindowMoves[1]` and `[29]`) are 0; it
also waits on four bytes (`D_800B8D34`, `D_800B8D35`, `D_800B8D38`,
`D_800B8D45`) that nothing writes.

**Open and close — `BtlMenuOpen` / `BtlMenuClose`.** `BtlMenuOpen`
(called by `BtlMenuRun` and around the battle's messages, below) runs
`func_80049AC4` (it copies one palette colour, fills a request block
nothing serves, and waits for the windows), sets the sprites' mix colour
with `BtlMixColNumberSet(0xE1)` (`0x8008B27C`) (0x7FFF, white), zeroes the held buttons,
makes the two pad records ready, empties the 0x40 blocks of the menu's
text and uploads them, and takes 0x380 bytes for `g_LinePaths`, the
paths the Tactics copies glide along. `BtlMenuClose` switches off the mark
records, clears the text grid's rows (`BtlMenuCellsClear(0x30)` (`0x80048C44`)), closes
window 0 (`BtlWindow0Set(2, 1, 0, 0)` (`0x8004A13C`)) and waits for it, switches off the
copies, lets one more frame pass and frees `g_LinePaths`. Its loop over
the four mark records never moves its pointer: it zeroes record 0's first
word four times, as the image does.

**The items — `g_BtlMenuItems`.** 0x20 items of 12 bytes (`BtlMenuItem`,
btl.h): `+0` on, `+1` not choosable, `+2` / `+4` x / y in cells, `+6` the
icon number, `+8` a word. An item's label is string `icon + 0x100`.
`BtlMenuIconCellsSet(cursor_flag, n)` (`0x80048698`) puts each item that is on on the
grid with `IconCellSet(x, y, icon, word ^ 0x20200000)` (`0x8001AF04`): `IconCellSet`
tests its last argument only for non-zero (non-zero = dimmed), so only an
item whose word is exactly 0x20200000 is bright and every other value,
0x200000 included, is dimmed. With `cursor_flag` set the item under the
cursor is bright whatever its word, and mark record 2 is put on its cell
(x and y times 8). Its second argument goes only to the empty
`BtlCellRowsUpdate` (`0x80048C9C`).

**The cursor — `BtlMenuCursorMove(pad, items, columns, rows, cursor)` (`0x80048D2C`).**
It answers the cell the pad moves the cursor to and stores nothing. Item n
stands in column `n % columns` of row `n / columns`; an item whose `+0` is
0 is passed over. The bits of the pad's +4 word are tried in the order
0x8000 (left: one column back, wrapping to the row's last column; past
items that are off back to the first that is on, or column 0, and when
that is off too round the row once more), 0x2000 (right, the same
forwards), 0x1000 (up: one row up, wrapping; then along that row to the
first item that is on), and any other bit of 0xF000 as down. A move that
finds nothing leaves the cursor where it was. Callers: the first menu and
Tactics with 2 × 2, the command grid 3 × 2, the spell list 4 × 2, the item
list 6 × 2.

**The menus' sounds — `g_BtlMenuSoundRequest`.** A step does not play a
sound: it leaves a number in `g_BtlMenuSoundRequest` and the menu's frame
plays it (`SoundPlay(request, 3)`) and clears it. 1 is a direction, 2 an
accepted CROSS, 3 TRIANGLE, 4 a refused CROSS (a command that is off, an
empty or uncastable spell, an unusable item). Ids 1 to 4 play resource
232's sequences 0 to 3 (`SoundPlay` gives ids below 0xB sequence id − 1, then
lowers sequences 9 and up by 2: [sound.md](../../sound.md) §"The battle's
sound requests"). Who
sets what: `BtlMenuFirstStep` 2 and 1; `BtlMenuPresetStep` 2, 3, 1;
`BtlMenuKindStep` 4, 2, 3, 1; `BtlMenuSpellStep` and `BtlMenuItemStep` 4,
2, 3, 1; `BtlMenuTargetStep` 2, 3, 1; `BtlMenuWalkStep` 2, 3. The shop
uses the same byte and its own loop plays it (§"The shop").

**The cursors kept between uses.** `g_BtlMenuKeptCursors` (u8 [2]) holds
the first menu's cursor in byte 0 and the Tactics page's in byte 1; the
set-up steps start the cursor from them and the input steps store a moved
cursor back. Nothing else reads or writes them, so they carry over from
round to round and from fight to fight (both 0 in the image). The command
grid, the spell list and the item list keep one byte per character id:
`g_BtlMenuKindKeptCursors`, `g_BtlMenuSpellKeptCursors` and
`g_BtlMenuItemKeptCursors`, 12 bytes each, all 0 in the image.

**Who the menu asks — `g_BtlMenuPendingMask`.** `BtlMenuPendingMaskGet`
makes the mask: bit i for party slot i (0..4) unless the slot is not active
(`g_BtlSlotActiveFlags`), its status (`+0x15` of its `BtlStats` record,
btl.h, at `CharStats + slot * 0x3C`, `CharStats` being the word at
`0x800B8C20`; the C reads it as the word at `+0x14` & 0xC00) has 0x04 or
0x08, both 0x10 and 0x20, or 0x40, or its byte of `g_BtlPartyNotHeldFlags`
is 0. `BtlMenuReset` and `BtlMenuFirstOpen` store it. So a confused member
(0x40) is never asked (its command is `BtlPartyActionAdjust` (`0x80029348`)'s random
plan), and neither is a stone one (0x30).

### The first menu — steps 1 and 2

`BtlMenuFirstOpen` makes the pending mask again, gives each masked slot the
plan kind 0xB (`g_BtlSlotPlannedKinds` and `g_BtlActionKind`) and its pose
(`BtlSlotActionAnimationSet` (`0x80037E64`)), and leaves the plan of an unmasked slot as
it was. Then `g_BtlActingSlot = 0`, `g_BtlPartyMode = 0`, the cursor from
`g_BtlMenuKeptCursors[0]`, and window 0 opens at cell (2, 1), 0x24 × 0xA
(`BtlWindow0Set`). Four items, all on and all at 0x200000 (dim but for the
cursor's): AI 0xED, Command 0xF0, Tactics 0xEF, Run 0xEE at ((i % 2)·5 + 3,
(i / 2)·5 + 2). The cursor item's label goes into text block 0, five
blocks, shown at (3, 5). The enemy groups: each active enemy slot 6..13
joins the first of four groups that holds its id, or the first empty one;
an enemy whose id fits none once all four are taken is not counted. Each
group is a line at (0xD, 2 + 2j): its name (string id + 0x80, eight blocks
from block 5 + 8j), a big "/" (`BtlCellGlyphSet(0x9C, 0x20, row, 1,
0x3000, 1)` (`0x8004B290`), code 0x8E) at column 0x20 and the count as one digit at
column 0x22 (`CellNumberSet` (`0x80057400`)). Mark record 2 (the cursor) is switched on
and the step waits for the window.

`BtlMenuFirstStep`, on CROSS: sound 2, `BtlMenuClear` (`0x80048794`), and by the cursor:

| cursor | item | does |
|---|---|---|
| 0 | AI | `g_BtlPartyMode = 1`, step 0, `BtlPartyActionSet(1)`: each party slot with an id whose HP is not 0, that is not on hold and whose status byte is not 4, 8 or 0x30 gets the program's own choice (`BtlPartyAllChoiceSet` (`0x8004DB30`)); any other slot with an id gets kind 5, number 0xFF; every planned target is 0xFE, which has `BtlActionRun` (`0x8002E088`) call `BtlPartyTurnActionSet` (`0x8004F734`) at the slot's turn to store its action again. `BtlRun` calls `BtlPartyActionSet(1)` in the menu's place in the rounds that follow |
| 1 | Command | `g_BtlPartyMode = 0`, step 5 |
| 2 | Tactics | `g_BtlPartyMode = 0`, step 3 |
| 3 | Run | `g_BtlPartyMode = 2`, step 0; `BtlRun` then calls `BtlPartyEscapeTry` |

A direction: sound 1, the cursor through `BtlMenuCursorMove(…, 2, 2, …)`;
when it moved it is kept in `g_BtlMenuKeptCursors[0]` and the label is
redrawn (`BtlMenuTextStringPut` (`0x80048CA4`), `BtlMenuTextImageLoad` (`0x80048D00`),
`BtlMenuIconCellsSet(1, 8)`). After AI, `BtlRun` puts up a small window
with the AI icon (`BtlWindow1IconDisp` (`0x80049740`): window 1 at (0x21, 1), 5 × 5, icon
0xED at (0x22, 2)) while the rounds go on without the menu.

### Tactics — steps 3 and 4

The first menu's third item opens the preset rows: each listed member's
three stored commands, one row each, plus a row of "Command".

**The rows are each character's data.** The 8-byte record
`*g_ChrPlacePresetRecords + id·8` (`ChrPlacePresetRecord`, chr.h) holds
the formation cell (+0, +1) and three {kind, byte} pairs (+2..+7), the
three Tactics. A new game's come from LUNADATA resource 0xE7 at 0x342
(`ChrRecordsReset` (`0x80063B30`): `memmove(*g_ChrPlacePresetRecords + 8, buf + 0x342,
0x58)`, ids 1..11): for all eleven characters `[x, y, 1, 0, 2, 0, 5, 0]`,
Tactics 1 AI, Tactics 2 Attack, Tactics 3 kind 5. The field menu's
command set-up page edits them (§"Pages 9, 0xE, 0xF and 0x10: the command
set-up"). Before every fight `MapBtlRun` (`0x8007AE58`) copies each party member's pairs
(ids < 0x1F of `g_BtlSlotIds`) into six slot tables, all zero in the
image: +2 / +3 to `g_BtlPartyPreset0Kinds` / `g_BtlPartyPreset0Numbers`,
+4 / +5 to `g_BtlPartyPreset1Kinds` / `g_BtlPartyPreset1Numbers`, +6 / +7
to `g_BtlPartyPreset2Kinds` / `g_BtlPartyPreset2Numbers`.

**The set-up — `BtlMenuPresetOpen`.** It takes two blocks with
`xmalloc_heap_alloc` (`0x80022F4C`): `g_BtlMenuPresetCells`, 0x78 bytes, four rows of
five 6-byte cells (`BtlMenuPresetCell`: +0 the string, +2 the colour, +4
the kind, +5 the number), and `g_BtlMenuPresetSlots`, the five members'
slots. Each slot of the pending mask goes on the list and gets a cell in
each row. Rows 0..2 come from the three preset tables, by kind: 0 →
{0xF0 "Command", 0, kind 0}; 1 → {0xED "AI", kind 1}; 2 → {0xE9, kind 2,
0xFF}; 5 → {0xEB, kind 5, 0xFF}; 6 → {0xEE, kind 6, 0xFF}; 3 and 4 → the
string is the table's number (a spell's icon, row + 0xB6, or an item id),
and when `BtlMenuActionCheck(stats, kind, number, 0)` (`0x800493F8`) says the member can
do it now, kind 3 with number + 0x4A (the spell's row) or kind 4 with the
number; otherwise colour 4 (dim) with kind 5 and 0xFF, a dim kind-5 cell
under the spell's or item's icon. Kinds 7 and up have no case and the cell
keeps the allocator's bytes. Row 3 is {0xF0, 0, kind 0} for everyone. The
MP-halving test inside `BtlMenuActionCheck` (trait 0x1B) is asked of the
acting slot `g_BtlActingSlot`, which `BtlMenuFirstOpen` leaves at 0, so
party slot 0's trait decides it for every member.

Items 0..3 are 0xF3 "Tactics 1", 0xF4, 0xF5 and 0xF0 "Command" at ((i %
2)·5 + 3, (i / 2)·5 + 2), at 0x200000; `g_BtlMenuItemCount` is the
members + 4; the cursor is `g_BtlMenuKeptCursors[1]`. Per member j: font
cells for eight blocks at (0x14, j·3 + 3), item 4 + j at (0x10, j·3 + 2),
lit (0x20200000), and the member is shown by its battle record: the record
is copied into `g_BtlMenuCopyRecords[j]` with bits 0x6000 of `+0x0E`
cleared and facing bit 0x2000 set, its own record is switched off (`+0 =
0`), and `line_path_make` (`0x80054C9C`) writes 16 points from where it stands to (0x68,
(j·3 + 5)·8) into `g_LinePaths + j * 0x40` (point k = from + d·k / 16, C
division toward zero, the last on the target). Window 0 opens at (2, 1),
0x24 × 0x11, and over 16 menu frames the copies glide along their points;
only then do the labels go in (`BtlMenuPresetRowDisp` (`0x80044CCC`)) and the step waits
for the window.

**The redraw — `BtlMenuPresetRowDisp`.** The cursor item's name into text
block 0 (five blocks); for each listed member i the cell
`g_BtlMenuPresetCells[cursor * 5 + i]`: its string + 0x100 in the cell's
colour into blocks 5 + i·8 (eight blocks), item 4 + i's icon = the cell's
string; last `BtlMenuIconCellsSet(1, 0xF)`.

**The input — `BtlMenuPresetStep`.** CROSS: sound 2,
`BtlMenuPresetRowActionSet(&cells[cursor * 5])` (`0x80044E24`), `BtlMenuPresetClose` (`0x8004571C`),
then step 5 with `g_BtlActingSlot = 0` when the pending mask still has a
bit of 0x1F, else step 0 (the round). TRIANGLE: sound 3, step 1,
`BtlMenuPresetClose` (`BtlMenuFirstOpen` makes the mask and the 0xB plans
afresh). A direction: sound 1, the cursor through `BtlMenuCursorMove(…,
2, 2, …)`, and only a cursor that changed is kept in
`g_BtlMenuKeptCursors[1]` and redrawn. It does not set `g_BtlPartyMode`.

**What a row does — `BtlMenuPresetRowActionSet(row)`.** For each listed
member: `g_BtlActingSlot` = its slot and its target
`g_BtlSlotPlannedTargets = 0xFF`; then by the cell's kind: 0 leaves the
member pending (its mask bit stays, so the command grid asks for it); 1
runs `BtlPartyActionSet(0)`, the one-member choice `BtlPartyOneChoiceSet` (`0x8004D61C`)
(`battle.md`), which plans target 0xFE so that the member's own turn plans
again through `BtlPartyTurnActionSet`; 2..6 store the kind in
`g_BtlSlotPlannedKinds` and the cell's +5 in `g_BtlSlotPlannedNumbers`, at
target 0xFF, which `BtlActionTargetAdjust` (`0x8002ED34`) picks at the turn (Attack: the
nearest enemy, `BtlSlotNearestFind(1)` (`0x80039838`); magic and items: the row's own
rule; kind 5: `BtlActionWalkCornerSet(2)` (`0x80039B70`)); 7 and up store nothing. Every
kind but 0 clears the slot's mask bit. It returns the mask; the caller does
not read it.

**The take-down — `BtlMenuPresetClose`.** It frees the two blocks, empties
the menu (`BtlMenuClear`), and over 16 frames moves each copy back along
its points, 15 down to 0; then every copy is switched off and every party
slot's battle record (0..4, by number, not through the list) switched on.

### The command grid — steps 5 and 6

**The set-up — `BtlMenuKindOpen`.** At slot 5 or more it sets step 0 (the
round) and returns. A slot whose pending-mask bit is clear is stepped past
(`g_BtlActingSlot += 1`) and the step returns, so it runs again on the next
frame for the next slot. Otherwise: step 6; the slot's plan reset (kind
`g_BtlSlotPlannedKinds` = `g_BtlActionKind` = 0xB, target and number 0);
its pose (`BtlSlotActionAnimationSet`); the pulse set going
(`BtlMenuPulseSet(g_BtlMenuPulse, 2, 0x400, 6)` (`0x8004888C`)); font cells for five
blocks at (3, 5) and five at (0x1B, 5); window 0 at (2, 1), 0x24 × 0xA;
the member's portrait at (0xD, 2) (`BtlMenuActingFaceSet` (`0x80049D60`), face (id − 1)·5
+ 1). Items 0..5 are the grid, AI 0xED, Magic 0xEA, kind 5 0xEB / Attack
0xE9, Items 0xEC, Run 0xEE at ((i % 3)·5 + 0x17, (i / 3)·5 + 2), at
0x200000; items 6..9 are the first menu's four at the left, not
choosable, with Command (item 7) lit (0x20200000, choosable byte 0), and
mark record 3 is put on item 7's cell at (0x40, 0x10). `g_BtlMenuItemCount
= 0xA`. Magic (item 1) is off when the slot's id is 0 or 0xC, when all
eight spell bytes at `+0x34` of its stats are 0, or when its status has
0x80; Items (item 4) is off while story bit 0x3D9 is set
(`StoryBitCheck` (`0x80057778`)). The cursor starts at `g_BtlMenuKindKeptCursors[id]`.
"Command" goes into text block 0; `BtlMenuKindDisp` (`0x80045960`) writes the cursor
item's name into blocks 5..9, in colour 4 (dim) when that item is off,
and sets the icons; mark records 3 and 2 are switched on and the step waits
for the window. `BtlCellRowsUpdate` and `func_80048C94`, called here with
arguments, are empty functions.

**The input — `BtlMenuKindStep`.** CROSS: the cursor 0..5 picks the kind
`g_BtlActionKind` = 1, 3, 5, 2, 4, 6. An item that is off (`+1 == 1`), or
a kind `BtlMenuActionCheck(stats, kind, 0, 1)` refuses, asks for sound 4
and only runs the tail. Otherwise the kind goes to `g_BtlSlotPlannedKinds`,
sound 2, `BtlMenuKindClose` (`0x80045E98`), and by kind: 1 → `BtlPartyActionSet(0)` and,
like 6, step 5 with the next slot; 3 → step 7; 5 → step 0xD; 4 → step 9;
2 → step 0xB with `g_BtlMenuTargetMode = 1` and `g_BtlMenuTargetMask =
BtlMenuTargetMaskGet(1)`. TRIANGLE: sound 3, step 5, `BtlMenuKindClose`,
and the slot steps back to the nearest earlier one whose mask bit is set;
with none (the slot was 0, or slot 0 is reached with its bit clear), step
1. One test in that arm is a marked spelling in the C: a conditional on a
variable holding 3 whose two outcomes are the same instructions; the
bytes need it, but its wording is the repository's, not the original's.
A direction: sound 1, the cursor through `BtlMenuCursorMove(…, 3, 2, …)`,
kept in `g_BtlMenuKindKeptCursors[id]` and redrawn (`BtlMenuKindDisp`)
when it changed. The tail, after a direction or a refused CROSS:
`BtlMenuPulseStep(g_BtlMenuPulse, 0)` (`0x800488A8`), and the slot's battle record `+4`
takes the pulse block's `+4` word, so the member's sprite blinks: drawn
while the level falls, not while it climbs.

`BtlMenuKindClose` switches off mark record 3, empties the menu
(`BtlMenuClear`) and sets the slot's battle record `+4` to 1, so the
sprite is left on.

**Who can do what — `BtlMenuActionCheck(stats, kind, number, flag)`.** It
answers 1 or 0. The status is the stats' `+0x15`.

| kind | answer |
|---|---|
| 0, 1 | 1 |
| 2, 5, 6 | 0 when the status has a bit of 0x4C or both 0x10 and 0x20 |
| 3 (magic) | 0 when the status has a bit of 0xCC or both 0x10 and 0x20; with `flag` set, 1 at once. Otherwise the spell is `(u8)(number + 0x4A)` with its 12-byte row `g_BtlSpellRows[spell − 1]`: 0 when the status has 0x20 and the row's `+0xB` is not 0 (sealed); the spell must be among the eight bytes at `+0x34` of the stats, and the MP `+4` must cover the row's cost `+4`, halved rounding up when `BtlTargetTraitCheck(0, 0x1B, 0, 0)` (`0x80037648`) answers for the acting slot (`g_BtlTargetSlot` is set to it first) |
| 4 (item) | 0 for the same status bits as kind 2, or while story bit 0x3D9 is set; with `flag` set, 1; otherwise the item must be one of the six bytes at `+0x22` or the six at `+0x2E` of the stats, and byte `+8` of its 0x12-byte row of `g_BtlItemTable` must not be 0 |
| 7 and up | the C runs into a loop with no end |

For kinds 3 and 4 past the `flag` test it then stores the target mode in
`g_BtlMenuTargetMode` (the low nibble of the spell row's `+1` or the item
row's `+6`) and `g_BtlMenuTargetMask = BtlMenuTargetMaskGet(mode)`, and
answers 1 when the mode is 0 or the mask is not 0. The trait is asked of
the acting slot whichever stats were passed.

### The spell list — steps 7 and 8

**The set-up — `BtlMenuSpellOpen`.** Step 8; `g_BtlActionNumber` and the
slot's planned number cleared; font cells for five blocks at (6, 3), four
at (0x1C, 0xA) and two rows of twelve at column 0xC, rows 2 and 4; window
0 at (2, 1), 0x24 × 0xE; the portrait at (3, 6). Items 0..7 are the eight
spell bytes at `+0x34` of the slot's stats at ((i % 4)·4 + 0xC, (i / 4)·4 +
7): a byte that is not 0 gives an item that is on, icon byte + 0xB6, lit,
and not choosable when `BtlMenuActionCheck(stats, kind, icon, 0)` says it
cannot be cast; an empty byte gives an item that is off. The cursor starts
at `g_BtlMenuSpellKeptCursors[id]`, walks down to the first item that is
on, failing that up, else 0; unlike the item list it does not write the
remembered byte back. Item 8 is the Magic icon 0xEA at (3, 2);
`g_BtlMenuItemCount = 9`. Text: item 8's label in block 0 and string 0x611
(two `0xF9nn` advances of 16 pixels, then the "MP" glyphs 0x9938 / 0x993B)
in blocks 5..8; a big "/" at (0x20, 0xC); the caster's MP (`+4` of the
stats) as three digits at (0x21, 0xC), colour 0x2000 when it equals the
maximum (`+6`), else 0x3000. Then `BtlMenuSpellDisp` (`0x80046258`), mark record 2 on, and
the window wait.

**The cursor's spell — `BtlMenuSpellDisp`.** With n the cursor item's icon:
`g_BtlActionNumber` and the slot's planned number = `(u8)(n + 0x4A)` (the
spell number) and the slot's pose; the strings n + 0x100 and n + 0x200 (the
spell's name and description) into the twelve blocks from 9 and from 0x15,
shown at (0xC, 2) and (0xC, 4), in colour 4 when the item is not
choosable; the cost (0 for n = 0, else byte `+4` of the row
`g_BtlSpellRows + (n − 0xB7) * 0xC`, halved rounding up under trait 0x1B)
as up to three digits at (0x1D, 0xC). It reads no argument;
`BtlMenuSpellStep` hands it `g_BtlMenuSpellKeptCursors`, which it ignores.

**The input — `BtlMenuSpellStep`.** CROSS on an item that is off or not
choosable: sound 4. Otherwise sound 2; `BtlMenuActionCheck` is called for
what it stores (the target mode and mask) and its answer is not read; the
planned number = icon + 0x4A. A mask of 0 with a spell other than 0x1E
(Power Sweep) goes to step 5 with the next slot, so a whole-side spell
skips the target window; anything else to step 0xB. Then `BtlMenuClear`.
TRIANGLE: sound 3, step 5 with the same slot, `BtlMenuClear` and
`BtlActingRestAnimationSet` (`0x80037890`). A direction: sound 1, `BtlMenuCursorMove(…, 4,
2, …)`, a moved cursor kept and redrawn.

### The item list — steps 9 and 10

**The set-up — `BtlMenuItemOpen`.** Step 0xA; action number and planned
number cleared; font cells for five blocks at (5, 3) and two rows of twelve
at column 0xC, rows 2 and 4; window 0 at (2, 1), 0x24 × 0xE; the portrait
at (3, 6). Items 0..5 are the six bytes at `+0x22` of the slot's stats at
(i·4 + 0xD, 7), items 6..11 the six at `+0x2E` at the same x and y 0xB,
each on, lit and choosable as `BtlMenuActionCheck` says, or off for an
empty byte. With all twelve off, string 0x60C goes into blocks 5..0x1C and
mark record 2 to pixel (0x68, 0x38). The cursor starts at
`g_BtlMenuItemKeptCursors[id]` and walks as the spell list's does; with no
item on it is 0 and the kept byte is set to 0 too. Item 12 is the Items icon
0xEC at (3, 2); `g_BtlMenuItemCount = 0xD`. Text block 0 is uploaded, but
nothing writes item 12's label into it. Then `BtlMenuItemDisp` (`0x80046BB4`), mark record
2 on, and the window wait.

`BtlMenuItemDisp`: for an item that is on, strings n + 0x100 and n + 0x200
(name and description) into the twelve blocks from 5 and from 0x11, colour
4 when not choosable; for an item that is off, string 0x60C, mode 9 and
mask 0.

`BtlMenuItemStep`: as the spell list's, with `BtlMenuCursorMove(…, 6, 2,
…)`, the planned number = the item's id, and step 5 (next slot) for a mask
of 0, else step 0xB.

### The target window — steps 11 and 12

The window shows the ACTION, not the target. **`BtlMenuTargetOpen`** opens
window 0 at (2, 1), 0x22 × 5, switches on layer 0x100's mix, sets the pulse
(`BtlMenuPulseSet(g_BtlMenuPulse, 1, 0x400, 0xC)`), and makes one item at
(3, 2), lit: 0xE9 for Attack (kind 2), the spell's icon (planned number +
0xB6) for kind 3, the item for kind 4; its label (string icon + 0x100) goes
into eight blocks shown at (7, 3). The mark byte `g_BtlMenuMarkKind` is 1
for the area spells (`g_BtlActionNumber` 2, 0xA, 0xC, 0x16, 0x1A, 0x1E,
0x2B, 0x2C, 0x2D, 0x2F), 2 for 0x1F (the lane), else 0. Mark record 0 (the
arrow) is switched on; for any mode but 1 mark record 1 (the hand) too.

* **Candidates — `BtlMenuTargetMaskGet(mode)` (`0x80048AE8`)** (a byte in, 16 bits out,
  bit n for slot n; it writes nothing and calls nothing): mode 0 and 9
  none; a mode whose low nibble is 1, the enemies 6..13 that have an id,
  HP that is not 0 and are active; any other, the party slots 0..4 whose
  id is neither 0 nor 0xC and that are active, stoned (both 0x10 and 0x20)
  or at 0 HP. The mode is 1 for Attack (`BtlMenuKindStep`) and the low
  nibble of the spell or item row for magic and items
  (`BtlMenuActionCheck`). Both callers store the answer in
  `g_BtlMenuTargetMask`.
* **The first target** (by `g_BtlMenuTargetMode & 0xF`): mode 1, the
  candidate nearest the caster's (x + 7, y + 7) by |dx| + |dy| (strictly
  less, so the lowest slot wins a tie); 5 and 6, the active party slot
  missing the most HP or MP (slot 0 when none is short); every other mode
  the caster, 3 and 4 after a search whose result is not used.
* **The input — `BtlMenuTargetStep`.** The cursor is a slot here. CROSS:
  sound 2, step 5, `g_BtlSlotPlannedTargets[actor] = cursor`,
  `BtlMenuTargetClose` (`0x80047CEC`), and the next slot. TRIANGLE: sound 3; the step
  becomes 5, 7 or 9 for a planned kind of 2, 3 or 4 (back to the grid, the
  spell list or the item list) and stays where it is for any other kind;
  `BtlMenuTargetClose`. A direction is any word with `g_BtlMenuPadRecord[2]
  & 0xA000` (sound 1), but the walks want exactly 0x8000 or 0x2000: both
  at once, or one with another bit, move nothing. LEFT walks down from
  cursor − 1 while the slot is above 0 and not a candidate, and takes the
  slot it stops on if that is a candidate (so slot 0 is tried last);
  otherwise it wraps, from slot 13 down to the first candidate above the
  cursor. RIGHT is the mirror (slot 13 tried last, then from slot 0 up). A
  wrap that finds nothing ends on the cursor. A cursor that moved re-places
  the marks (`BtlMenuTargetMark` (`0x8004746C`)). Every call ends with the pulse step.
* **The marks — `BtlMenuTargetMark`.** Mark record 0 (resource 2's bank
  `g_SpriteData2`, anim 6, under 0x730: the arrow) at the target's x and
  head, its `+0xA` minus its height `+0x3E`, no higher than 0x28, and its
  `+8` = (`+0xA` + 1) | 0x1000; mark record 1 (the hand) at ((0x1F − 6n)·8,
  0xB8), n the party slots from 4 down to the one above the cursor that
  have an id and are active, stoned or at 0 HP. The fourteen number kinds
  `g_BtlSlotAmountKinds` are cleared; with a mark byte that is not 0 the
  target is the cursor and the area's half-width and half-height are the
  spell row's `+9` and `+0xA`, and `BtlTargetAreaMark(mode & 0xF)` (`0x80035B50`) (mark
  byte 1) or `BtlTargetRowsMark()` (`0x80035260`) (2) sets the number kinds of the slots
  it reaches. Last every slot's draw word `+0xE` has bits 0x1C00 cleared,
  and 0x800 (mix group 2) is set on the cursor's slot alone, or with a mark
  byte on every slot whose number kind was set.
* **The pulse — `BtlMenuPulseSet` / `BtlMenuPulseStep`.** The 0x10-byte
  block `g_BtlMenuPulse` (`BtlMenuPulse`, btl.h): `+0` the mode, `+4` the
  direction (1 falling), `+8` a mask, `+0xC` the level and `+0xE` its top.
  Each step moves the level one toward 0 or the top and turns at either
  end. For mode 1 it then calls `layer_mix_set(mask, level * 2 & 0xFE)` (`0x8001ECA0`):
  with mask 0x400 that is the level of mix group 2, the glow of the marked
  slots (`battle.md`, "The screen effects"). Mode 2 (the command grid)
  calls nothing; its caller copies the direction word into the member's
  sprite switch. The glow's colour is the sprites' mix colour
  `g_SpriteMixCol`, which `BtlMenuOpen` sets to 0x7FFF each time the menu
  opens.
* **`BtlMenuTargetClose`** switches off mark records 0 and 1, resets the
  cursor slot's `+8` from its `+0xA`, clears every slot's number kind and
  draw bits 0x1C00, ends the glow (`layer_mix_set(0x400, 0xFF)`,
  `layer_mix_switch_off(0x100)` (`0x8001F4E0`)) and empties the menu (`BtlMenuClear`).

### The place to walk to — steps 13 and 14

The command grid's item 2 (icon 0xEB, string 0x1EB) gives kind 5, the walk
(`BtlActionWalkRun` (`0x8002E5D4`)). **`BtlMenuWalkOpen`** sets the pulse (`(1, 0x400,
0xC)`), makes both pad records ready again (`(0, 0x40)` and `(0x20,
0x40)`), writes string 0x1EB into four blocks shown at (7, 3), copies the
acting slot's battle record into its copy record (on, draw bits 0x1C00 set
to 0x800, `+8` = `+0xA` + 1), opens window 0 at (2, 1), 0x22 × 5, switches
on layer 0x100's mix, and makes one item, icon 0xEB at (3, 2), lit. After the
window wait the copy takes the stand animation
`g_BtlPartyStandAnimations[c] − 5` when a direction is held, else 0x13.

**`BtlMenuWalkStep`** moves the copy with the held directions, 2 pixels a
frame: left (facing 0x2020) while x ≥ 0x1A, right (facing 0x20) while x <
0x127, up while its `+0xA` ≥ 0x72, down while it is < 0xAF, with `+8` kept
at `+0xA` + 1; a new press of a direction restarts the walking animation,
and with no direction held it goes back to 0x13. CROSS: sound 2, step 5,
the planned number = the copy's x / 8 and the planned target = its `+0xA`
/ 8, `BtlMenuWalkClose` (`0x8004834C`), the next slot. TRIANGLE: sound 3, step 5,
`BtlMenuWalkClose`, nothing stored. Every call that stays ends with the
pulse step. `BtlMenuWalkClose` makes the first pad record ready again,
ends the glow, switches the copy off and clears the draw bits on the copy
and on the battle record, and empties the menu.

### The menu's records — `BtlMenuInit`

`BtlMenuInit` (`0x800441B4`), once a battle (from `BtlInit` (`0x80029E68`)): `layer_order_set(0x204,
1)` (`0x8001EE68`), `(0x508, 2)` and `(0x30, 3)`; `BtlWindowsInit` (`0x80049F40`); three heap blocks —
0x240 bytes, nine 0x40-byte records (`BtlRecord`, btl.h), the first five
`g_BtlMenuCopyRecords` (one per party slot, the copies the menu moves about)
and the last four `g_BtlMenuMarkRecords`; 0x10 bytes `g_BtlMenuPulse`; 0x180
bytes `g_BtlMenuItems`. The five copies' first words are zeroed. The four
mark records are bound to resource 2's banks (`BtlRecordSpriteDataSet` (`0x80089A3C`)):
record 0 `g_SpriteData2` anim 6 under 0x730 (the target arrow), records 1..3
`g_SpriteData0` anims 2, 0xA and 0xB under 0x760 (the target hand, the
item cursor, the mark on the menu a sub-menu came from); records 1..3 get
`+8 = 0xFFF`, all four `+0xC = 0`, and records 1..3 `+0xE &= 0x9FFF`. The
item count is 0 and only items 5..0x1F are zeroed (the loop carries on from
the 5 the records' loop left its counter at; items 0..4 are not). Then
`g_BtlMixFlash[8] = [9] = 8`, `[0] = [1] = 0`; bit 1 of `D_800B8EEC` set
and bits 0xC of `MenuScreenBits.unk_0e` (at `D_800E4E70`) cleared;
`layer_mix_switch_off(8)`, which does nothing (§"The two layer records");
the text grid cleared (`BtlCellRowsClear(0,
0x20, 0x30)` (`0x80049DD0`)); the four words at `D_800E4768` left as 1, 0x80300000, 0 and
0x1000; and `g_BtlMenuTextBlocks = 0x801CBB00` with
`font_target_set(0x801CBB00)` (`0x800270AC`), which takes one argument. `BtlMenuFree` (`0x800444D0`)
(from `BtlClose` (`0x80029214`)) undoes it and switches the whole sprite list off.
`BtlMenuOutlineStep` (`0x800489F0`), called by the battle frame `BtlFrameStep` every
frame, draws the animated outline round mark records 2 and 3
(`OutlineScreenStep` (`0x8001BE30`), below) and counts the play time (`PlayTimeStep` (`0x8001C064`)).

**The menu's reset — `BtlMenuClear`.** Eight call sites, no arguments.
It empties the 0x40 blocks of the menu's text and uploads them; empties the
`g_BtlMenuItemCount` items in use (`+8`, `+6`, `+4`, `+2` and `+0`; the
`+1` byte is left as it is, and the count is not changed); switches off
mark records 3 and 2; and clears rows 2..0x10 of the text grid, first the
cells of kind 3 or more and then those below 3 (`BtlCellRowsClear(2, 0xF,
0x10)` and `(2, 0xF, 0x20)`, each followed by the empty
`BtlCellRowsUpdate`). No window is touched.

### The menu's text

The menu's text is 0x80-byte blocks from `0x801CBB00`
(`g_BtlMenuTextBlocks`), each a 16 × 16 cell of the label font.
`BtlMenuTextStringPut(block, id, colour, n)` empties n blocks from `block`
and makes them the font's target (`font_target_clear` (`0x8002707C`)), sets the colour
bits (`font_col_set` (`0x800270BC`)) and writes string `id` in mode 1
(`font_string_put(id, 0, 1)` (`0x80026F78`)): proportional and left-aligned from pixel 0
of the first block, each glyph's own blank left columns kept, each
advancing by its width (rightmost inked or outline column + 1; a blank 5).
`0xF9nn` moves the pen nn pixels (`font_pen_move` (`0x800270CC`), carrying whole blocks).
`BtlMenuTextImageLoad(block, place, n)` sends n blocks to video memory
(`font_image_load` (`0x80016438`)); every caller passes the same number for the block and
the place. `BtlMenuTextCellsSet(x, y, n, rows, -, place)` (`0x80049DF0`) sets the text
grid's cells that show them: row r at (x, y + 2r), n blocks from place + r·n
(`font_cell_run_set` (`0x80016230`)); its fifth argument is not read. A kind-2 cell
draws one 16-pixel-high sprite n·16 wide from strip `(place & 0x7F) >> 4`,
row `place & 15` of `g_FontLines` (strip 0 at VRAM (0, 448), tpage 0x10),
CLUT 0x7ECE, opaque, OT 0x38. `BtlCellRowsUpdate(row, row * 0x40, rows,
mask)`, called after many of these, is an empty function: the grid is drawn
from memory every frame. `BtlCellRowsClear(row, rows, mask)` is
`ankprint_rows_clear` (`0x80025A68`) with the same arguments, and `BtlMenuCellsClear(bits)`
clears all rows of the icon and face cells (0x10) and rows 0..0x13 of the
others (0x20).

### The battle windows

There are two windows. Window 0 holds the menus and the messages; window 1
the AI icon and the end message's second part. Each is two sprite-list
entries at `g_SpriteList + 0x900 + window * 0x48` (entries 0x40 + 2·window
and the next): an outline and a box.

**Opening and closing — `BtlWindow0Set(x, y, w, h)` /
`BtlWindowSet(window, x, y, w, h)` (`0x8004A1C0`).** In cells of 8 pixels. The rectangle
record `g_BtlWindowRectangles[window]` (`BtlWindowRectangle`, btl.h, 14
bytes) gets byte 0 = 2, byte 1 = 1, x·8 and y·8 at +2 / +4, the old width
and height kept at +6 / +8 and the new w·8, h·8 at +0xA / +0xC; the first
words of the window's box and outline blocks are set to 1;
`BtlWindowMoveStart(window)` (`0x8004A26C`) starts the move and one menu frame passes.
Closing is the same call with w = h = 0. The menus use (2, 1, 0x24, 0xA),
the spell and item lists (2, 1, 0x24, 0xE), Tactics (2, 1, 0x24, 0x11),
the target and walk windows (2, 1, 0x22, 5).

**The move — `BtlWindowMoveStart` / `BtlWindowsMoveStep`.** The move
`g_BtlWindowMoves[window]` (`BtlWindowMove`, 0x1C bytes): `+0` the mode
byte, `+1` moving, `+2` the step, `+4/+6/+8/+0xA` the edges now (left,
top, right, bottom), `+0xC..+0x12` the edges to reach, `+0x14` / `+0x18`
the right and bottom edges' speeds (s32, 1/256 pixel a step). The start
does nothing while a move runs; otherwise it copies the mode, sets step 0,
the edges now to x, y, x + old width, y + old height, the edges to reach to
x, y, x + w, y + h, each speed to `(s16)(new − old) << 5`, and the moving
byte last. Each frame, for a window that is on and moving, the right edge
is `+8 + (vx·step >> 8)` and the bottom `+0xA + (vy·step >> 8)`, each
clamped at its target on the side its speed's sign says (an axis that
arrives also puts its left or top edge on the target and stops); at step 8
or more, or with both speeds 0, the targets become the edges, step and
moving are cleared, and a window left with no width and no height has its
outline block's first word cleared (closed); otherwise the step goes up by
one. So the corner stays put and the size goes from old to new in eight
steps, nine frames counting the first. Then `BtlWindowSpritesSet(window,
left, top, right, bottom)` (`0x8004A5EC`) writes the sprites. `BtlWindowsMoveStep` runs
from the menu's frame and from the presentations' frame
`BtlShowFrameStep` (`0x80089DDC`).

**The tables — `BtlWindowsInit`** (once, from `BtlMenuInit`). It switches
off sprite entries 0x40 and 0x41 and, for each window i:

| table | stride | written |
|---|---|---|
| `g_BtlWindowMoves[i]` | 0x1C | bytes `+0` and `+1` = 0 |
| `g_BtlWindowRectangles[i]` | 14 | all zero |
| `g_BtlWindowBoxes[i]` (the box, a `SpriteEntry`) | 0x24 | `+4` = 0x500, `+8` = 0x8000, `+0x14` = `&g_BtlWindowBoxPieces[i]`, the rest 0 |
| `g_BtlWindowBoxPieces[i]` (a `SpritePiece`) | 0x18 | `+0` = 0x4000, `+6` = 0x3E4, the eight corner words 0 |
| `g_BtlWindowOutlines[i]` (the outline, a `SpriteEntry`) | 0x24 | `+4` = 0x1000, `+0x10` = `&g_BtlWindowOutlinePoseRows[i]`, `+0x14` = `&g_BtlWindowOutlinePieces[2·i]`, the rest 0 |
| `g_BtlWindowOutlinePoseRows[i]` (a `SpritePoseRow`) | 4 | 2, 0: two pieces |

and the four outline pieces `g_BtlWindowOutlinePieces[0..3]` get `+0` =
0x5000, zero points, and `+6` = 0x3E6 for pieces 0 and 2, 0x3E4 for 1 and
3. These are sprite-list entries and pieces as `sprite_draw` (`0x8001A528`) and
`sprite_piece_draw` (`0x80019DBC`) read them: `+4` a depth, `+0x10` a pose row (a piece
count and the first piece), `+0x14` the pieces; in battle a piece of kind
0x4000 is one rectangle in one colour between its corners at +8/+0xA and
+0x10/+0x12, and kind 0x5000 one LINE_F3 through its points at +0xC, +8
and +0x14, the word at +6 picking the colour.

**The sprites — `BtlWindowSpritesSet`.** The first entry becomes a copy of
the outline block; when it is on, piece 2·window gets the points (left,
top), (right − 1, top), (left, top), (left, bottom) and the next piece
(right, bottom), (right, top), (right, bottom), (left + 1, bottom). When the
move's mode byte is 2, the second entry becomes a copy of the box block and
its piece gets the four corners. (A mode byte of 1 for window 0 would copy
window 1's box into the first entry instead; both setters store 2, and no
function stores 1.) The renderer draws window 0 from those two entries, OT
bucket 0x40, absolute coordinates in battle (`g_BtlRunning`), in this
order:

1. the draw-mode word 0xE1000200 (abr 0: background / 2 + colour / 2;
   `sprite_piece_box_draw` (`0x80019668`) adds it after the TILE in the same bucket, which
   runs last-in first-out);
2. a semi-transparent TILE (x0, y0) w × h, colour index 0x3E4 read through
   RAM `0x801D5B80` (the copy of CLUT rows 500..507) = row 503 entry 0xE4,
   (48, 48, 40); semi by the entry's `+0x08` bit 15 (`g_BtlWindowBoxes + 8`
   = 0x8000), not `+0x0E` bit 10;
3. an opaque LINE_F3 (x1 − 1, y0) → (x0, y0) → (x0, y1), colour 0x3E6 read
   through `0x801D6380` (the kind-0x5000 base) = row 507 entry 0xE6, (64,
   64, 96);
4. an opaque LINE_F3 (x1, y0) → (x1, y1) → (x0 + 1, y1), 0x3E4 through the
   same base = row 507 entry 0xE4 = 0x8000, black.

x1 = x0 + w, y1 = y0 + h. The window's placement was checked byte for byte
against an emulator capture of the battle menu.

### The party panel

The HUD has no window: it is glyph cells on rows 0x17..0x1A of the text
grid. **`BtlPartyPanelDisp` (`0x8004B330`)** (called by `BtlRun`, `BtlWinRun` (`0x80028140`),
`BtlSlot5Revive` (`0x80028E5C`), `BtlActionRun`, `BtlActionAttackRun` (`0x80031060`) and `BtlFallRun` (`0x800385B4`))
clears the cells below kind 3 of rows 0x17..0x1A, then lists each party
slot 0..4 that has an id and is active, stoned (both 0x30 bits) or at 0 HP:
its id and a record (`BtlPartyPanelMember`, btl.h) of bit 4 of the stats'
`+0x1E`, the status `+0x15`, the byte `+0x14` and the first four words (HP,
its maximum, MP, its maximum). The members are written last to first into
columns 4, 3, … by `BtlPartyPanelMemberDisp(column, id, record)` (`0x8004B5B8`), so the
panel ends at the right. Left of the leftmost column go three glyphs at x =
column·6 + 4, colour 0x3000: 0x5D and 0x5F on row 0x19 and 0x5E, two cells
wide, on row 0x1A (the "H P" and "MP" labels).

**One column — `BtlPartyPanelMemberDisp`.** Nothing for ids 0 and 0xC; id
0xB is written as 1. With x = column·6: the name tag on row 0x17, one glyph
four cells wide of code id·4 + 0xC at x + 8; the status on row 0x18
(`BtlPartyPanelStatusDisp` (`0x8004B788`)); HP and MP as three digits at x + 9 on rows
0x19 and 0x1A (`BtlPartyPanelNumberDisp` (`0x8004B944`), which writes three glyphs 0x5C
instead when its first argument, the stats' `+0x14` for MP, is 0xFF).

The status glyph (four cells wide), by the first that holds: 0 HP 0x58;
the `+0x1E` bit 0x3C; status 1, 2 or 3 0x40; 4 0x44; 8 0x48; 0x10, 0x20 or
0x30 0x4C; 0x40 0x50; 0x80 0x54. With status 0 it writes glyph 0xB ("LV"),
two cells, and the level (the stats' `+0x14`) as two digits at x + 2, or
two glyphs 0x5C when that byte is 0xFF.

The colours are palette words of the glyph cell: 0, 0x1000, 0x2000 and
0x3000 pick sub-CLUTs 11, 12, 13 and 14 of row 507 (`ankprint_glyph_set` (`0x80025DC0`)
shifts the colour into the cell's bits 14..15, and `ankprint_glyph_draw` (`0x80025B90`)
draws with palette `0x7EC0 + (((cell >> 14) + 0xB) & 0xF)`). The name tag
and the status: 0 at 0 HP, otherwise 0x1000, and 0x3000 when the `+0x1E`
bit and the status byte are both 0. HP: 0 at a quarter of its maximum or
less, 0x1000 at half or less, 0x3000 below the maximum, 0x2000 at it. MP:
0x2000 when it equals its maximum, then the same three steps; above its
maximum no colour is set. No band is drawn behind the panel:
`MessageShadeDraw` (`0x80015E88`)'s band height `g_MessageShadeHeight` is set only by
field scripting (`MessageShadeSet` (`0x80015EE8`), message_disp.c).

### The damage numbers

The numbers over a hit or healed actor are amount records drawn by the
ordinary actor renderer: the tick copies them into the shared sprite list
each frame, as the actor tick (`BtlAnimationStep`) copies actors.

**Starting one — `BtlAmountRecordSet(slot)` (`0x8004AE8C`) / `BtlAmountRecordStart(slot,
anim, style)` (`0x8004AB40`).** `BtlAmountRecordSet` switches on the slot's outcome
`g_BtlSlotAmountKinds[slot]` and starts a record with an animation and a
style byte; the value is `g_BtlSlotAmounts[slot]`. Slots 0..4 use records
0..4 and slots 6..13 records 5..12 (slot − 1); slot 5 (the companion) has
none, and a new number on a slot replaces its record.

| outcome | meaning | anim | style | sub-CLUT | digits |
|---|---|---|---|---|---|
| 1 | a hit (a critical too) | 1 | 0xE0 | 14 white | A |
| 3 | WEAK | 1 | 0xB0 | 11 red | A |
| 4 | RESIST | 1 | 0xC4 | 12 yellow | B |
| 5 | a heal | 0 | 0xD0 | 13 green | A |
| 8 | attack up (+0x08, `BtlTargetStat08RaiseRun` (`0x80033CBC`)) | 0 | 0xC0 | 12 orange | A |
| 9 | defence up (+0x0A, `BtlTargetStat0ARaiseRun` (`0x80033D2C`)) | 0 | 0xB4 | 11 blue | B |
| 2, 6, 7, other | evaded, the barrier, a status took | — | — | — | none: the record is retired (`BtlAmountRecordOff` (`0x8004ADA8`)) |

The retire indexes the records as the start does (`slot` below 6, `slot −
1` from 6 up), so for slot 5 an outcome with no number retires record 5,
which is slot 6's; an outcome with a number does nothing for slot 5. There
is no "MISS" and no word for a critical (it only swaps the impact's
animation); the words "ATC" / "DEF" for outcomes 8 / 9 are drawn by
`BtlAmountShowStart` (`0x80088EA8`), which binds `g_SpriteData2` with draw words 0x7C0 /
0x7B0 and anims 7 / 8. The round's status tick `BtlRoundStatusUpdate` (`0x800297A4`) sets
outcome 5 for regeneration and 1 for poison before its own pass
(`BtlAmountRecordsRun` (`0x8004AE0C`)).

`BtlAmountRecordStart` fills the record at `g_BtlAmountRecords + record *
0x40`: the first word and the entry's first word 1; timer `+0x34` and step
`+0x38` 0; the entry's draw word (`+0xE` of the record) = `0x4300 | (style
& 0xF0)`, row 507 with the style's high nibble as the sub-CLUT; the pose
row and the digit pieces pointed at the record's own rows of
`g_BtlAmountPoseRows` and `g_BtlAmountPieces`. The value's hundreds, tens
and ones go to the texel numbers of the three pieces (`+6`, `+0x1E`,
`+0x36` of the digit block) as base + digit, base 1, or 0xB when the
style's low nibble is not 0. Zeros before the first digit are left empty
(picture 0); a two-digit value puts its tens in the middle piece and its
ones in the right, and a one-digit value sits in the middle piece; the
thousands are dropped (`(v / 100) % 10`). The record's `+0x3C` is copied
from the slot's battle record, x from its `+6`, and y (to the entry's `+4`
and `+6`) = its `+0xA` plus the y offset step 0 picks; `+0x3E` = 0. The
animation goes to `+0x30` and `BtlAnimationStart(record, 1)` (`0x80027240`) starts it.

**The glyphs and the records — `BtlAmountRecordsInit` (`0x8004A8A0`).** A leaf, called
by `BtlEnemyLoad` (`0x8002B3AC`) after it has allocated the four tables. 21 texel records
(`g_BtlAmountTexelRecords`, 0xC bytes each) `{0, page slot 10, 8, 8, u =
8·i, v = 0}`: resource 2's sheet (VRAM (960, 0)), line 0, entry 0 blank,
1..10 the digits 0..9 in style A (CLUT indices 1..3), 11..20 in style B
(indices 5..7). 13 digit blocks (`g_BtlAmountPieces`, `BtlAmountDigits`,
0x48 bytes = three 0x18-byte pieces `{0, 8, 8, 0, −3, 0, 0…}`, the first
and third shifted −8 / +8, so 8 × 8 frames at x offsets −11, −3 and +5).
13 records (`g_BtlAmountRecords`, 0x40 bytes) from a stack template —
`+0x04 = 1`, `+0x10 = g_BtlAmountTexelRecords`, `+0x28 =
g_BtlAmountAnimationTable`, `+0x2C = g_BtlAmountStepList`, `+0x00`,
`+0x0C`, `+0x24` = 0, s16 `+0x1C`, `+0x20` = 0, every other byte copied
out uninitialised — each bound to its pose word (`+0x14 =
g_BtlAmountPoseRows + 4·i`, set to {3, 0}) and its digit block (`+0x18`).
The caller allocates 0x38 and 0x3F0 bytes, room for 14 pose words and 14
digit blocks, but only 13 of each are filled (the 0x340 bytes of records
and 0xFC of texel records are filled exactly). Draw flags `0x4300 | (style
& 0xF0)`: depth mode 0x4000 (a fixed 0x200), opaque, 0x808080.

**The motion — `BtlAmountRecordsStep` (`0x8004AF94`).** Called from the battle's frame
`BtlFrameStep` every frame, it does nothing unless
`g_BtlAmountRecordsRunning` is set. It frees the 13 sprite slots
0x72..0x7E (`g_SpriteList + 0x1008`; `sprite_run_off(first, n)` (`0x80019A7C`) clears n
slots' first words), then walks the 13 records with two counters, the
slot i (5 has no record) and the record j. A record whose `+0` is set
counts its timer `+0x34` up to the step's duration `+0x36`, then steps
on; past the last step (`+0x3A & 0x7FFF`: the hold bit is ignored)
`BtlAmountRecordOff(j)` retires it. Otherwise the timer restarts, the
duration comes from the step's {y index, duration} pair, and y = the
slot's `+0x0A` (`g_BtlRecords + i·0x40`) + `g_BtlAmountYOffsets[y index]`
− `+0x3E` goes to the entry's `+0x08` and `+0x0A`. Every live record's
0x24-byte sprite image (`+0x04..+0x27`) is copied into slot 0x72 + j with
that slot's `+0x18` cleared. The tables are `g_BtlAmountAnimations`
(`0x800B3168`), whose word 0 is a count (2) followed by the animation
table and the step list, and `g_BtlAmountYOffsetTable` (`0x800B3218`);
`BtlEnemyLoad` stores their addresses in the words
`g_BtlAmountAnimationTable`, `g_BtlAmountStepList` and
`g_BtlAmountYOffsets`. Anim 0 = 20 steps from 0, anim 1 = 21 from 20,
both with the hold bit, which this tick ignores; the steps are {y index,
duration}; the y table is 31 s16 offsets. Anim 0
(heal, stat up) drifts from −46 to −8 by 2 pixels every 3 ticks and holds
25 ticks, 81 in all; anim 1 (damage) bounces (peaks −28, −17, −11 over a
resting −8) and holds 25, 62 in all. `BtlAmountRecordStart` takes the
first y from step 0's offset, so a damage number shows at −46 for one frame,
then −12.

**Who waits.** The impact `BtlAmountShowStart` starts one per flagged slot
(0..13 except 5); `BtlAmountShowCheck` (`0x8008931C`) holds the presentation until the
longest has finished. The standalone pass `BtlAmountRecordsRun` →
`BtlAmountRecordsSet` (`0x8004B1D0`) starts records for slots 0..12 (5 included, 13
never) and runs `BtlFrameStep` for the longest animation's length,
setting and clearing `g_BtlAmountRecordsRunning` round its pass.

### The battle's messages

All of them use window 0, the menus' window (`BtlWindow0Set` and
`BtlWindowSet(0, …)` write the same rectangle record). Each opens, waits
out its move (`BtlWindowsMoveWait`), and only then prints; closing, the
text goes first. Each of the end, win and level-up messages is framed by
`BtlMessageMenuReset` (`0x8004BCB4`), `BtlMenuOpen` and `BtlMenuClose`.

* **The action's name — `BtlActionStringDisp` (`0x8004987C`)**, from the action
  dispatcher `BtlActionKindRun` (`0x8002E12C`) (before the spell and item arms),
  `BtlShowOpen` (`0x8007B7C8`) and `BtlActingShowRun` (`0x8003A158`). Nothing while the options byte
  `action_strings` (`g_GameOptions` + 6, `GameOptions`, game.h) is 0;
  `GameStart` (`0x80055B60`) sets it to 1, and `BtlFrameStep` and the Options page flip
  it. Kind 3: the spell (string number + 0x1B6); kind 4 for a party slot:
  the item (`number | 0x100`); kind 7 for an enemy slot: its special
  (`number | 0x300`); anything else, nothing. The string goes into eight
  blocks shown at (3, 2) in window 0 at (2, 1), 0x12 × 4.
  `BtlActionStringClose` (`0x8004999C`), after every action of `BtlActionKindRun` that has
  not returned and in `BtlItem83Revive` (`0x80038868`), clears those cells and closes the
  window whether or not a string went up.
* **The battle's end — `BtlEndMessageDisp(kind)` (`0x8004BA18`).** The developers' own
  name: its debug print begins `"BtlEndMessageDisp("`. With kind 0 it waits
  one frame and empties the text (`BtlMessagePadWait(1, 0)` (`0x8004C8B0`)) and closes the
  menu. Otherwise: window 0 at (2, 1), 0x24 × 4, text cells for 0x11
  blocks at (3, 2), and the message `g_BtlEndMessage` = {0xFF01, 0xF648,
  0xFFFF} with its second code + kind − 1, or 0xF669 for kind 4: 1 "Alex's
  party won the battle!", 2 "…ran away!", 3 "…fled!", 4 "It's a dead
  man's party!". Last it sets window 1 to size 0. The round loop's tail: a
  win holds kind 1 for 0x1E frames or until CROSS is held (`BtlPadWait(0x1E,
  0x40)` (`0x800365DC`)); the fallen party kind 4 — or 2 when a member got away earlier —
  for 0x96 frames or CROSS; an escape (from `BtlSlotLeave` (`0x8002E398`) /
  `BtlPartyEscapeTry`) shows kind 3 while slot 5 (id 0xC, Nall) flies off
  (`BtlSlot5EscapeMove` (`0x80037578`): only when `g_BtlSlotIds[5] == 0xC`; it clears
  `g_BtlSlot5Roaming`, sets the record's facing `+0xC` to 0x20, then +8 a
  frame, one `BtlFrameStep` each, until x reaches 0x150) and every enemy
  left walks off right (`BtlEnemyEscapeMove` (`0x80037358`): id 0x12 plays anim 9 first,
  then all anim 1, +4 a frame until none is below x 400 or 400 frames have
  passed); then kind 0 closes it.
* **The rewards — `BtlWinRun` → `BtlWinMessageDisp` (`0x8004BB68`).** It puts each party
  member that stands on animation 0xF, then shows the two sums and the
  items. `BtlWinRewardMessageDisp` (`0x8004BD1C`): nothing when both sums are 0; window 0
  at (2, 1, 0x24, 6 with both lines, else 4); the EXP `g_BtlWinExperience`
  (s32) as five digits at (4, 2), colour 0x3000, and " Exp Points gained!"
  in text cells at (9, 2); the silver `g_BtlWinMoney` (u16) on row 4 (row 2
  without EXP), its five digits at (4, row), the coin glyph (`BtlCellGlyphSet(
  0x9E, 9, row, 1, 0x2000, 1)`, code 0x8F) at (9, row) and " obtained." at
  (0xA, row). The message is a copy of `g_BtlWinRewardMessage` {0xFF01,
  0xF623, 0xFF02, 0xF624, 0xFFFF} written again from its second code
  (0xF623 and 0xFF02 only with EXP, 0xF624 only with silver, then 0xFFFF).
  `BtlWinItemsMessageDisp` (`0x8004C068`) then shows each non-zero byte of
  `g_BtlWinItems` with `BtlWinItemMessageDisp` (`0x8004BF8C`): window 0 (2, 1, 0x24, 5),
  item 0 of the menu takes the item's icon, and `g_BtlWinItemMessage`
  {0xFF01, item | 0xF100, 0xF624, 0xFFFF} ("Herb obtained.") at (9, 3).
  Each waits for CROSS or up to 0x81 frames and empties the text
  (`BtlMessagePadWait(1, 1)`).
* **A level — `BtlLevelUpMessageDisp` (`0x8004BC7C`) → `BtlLevelUpGainsDisp` (`0x8004C324`).** Nothing
  when `g_ChrLevelUpGains[0]` (the member's id) is 0. Window 0 (2, 1, 0x24,
  6), two lines of 0x11 blocks at column 4, rows 2 and 4, and
  `g_BtlLevelUpGainsMessage` {0xFF01, 0xF000 | id (0xF001 for id 0xB),
  0xF613, 0xF625, 0xFFFF}; then `BtlLevelUpGainDisp(i)` (`0x8004C0D8`) for i = 0..9, each
  followed by `BtlMessagePadWait(0, 1)`. A gain line (i = 0..8, skipped
  when the gain byte is 0) empties the second line (0x14 blocks at
  `g_BtlMenuTextBlocks + 0x880`), writes string 0x628 + i, a space and
  string 0x626, and puts the gain as two digits at column `div + 5` (`div
  + 6` when `g_FontPen` is not 0), `div = (g_FontTarget − 0x880 −
  g_BtlMenuTextBlocks) / 64` (signed), on row 4, colour 0x2000. Line 9 is the
  spell learned: window 0 (2, 1, 0x24, 5), item 0's icon = the byte + 0xB6,
  and the message `g_BtlLevelUpGainMessage` with its second code the byte +
  0xF1B6 (string 0x1B6 + the spell).
* **The codes — `BtlMessageDisp` (`0x8004C498`) → `BtlMessageControlRun` (`0x8004C5E4`).** A message is
  a list of 16-bit codes ending at 0xFFFF. `BtlMessageDisp` saves the line
  number `g_MessageLine`, sets it to 0 and empties the three lines of 0x11
  blocks (0x33 blocks at `0x801CBB00`); a code up to 0xEFFF is a glyph
  (`font_code_put(code, 1)` (`0x80026D64`)), after which the line's 0x11 blocks are
  uploaded; a higher code goes to `BtlMessageControlRun`, which switches on
  the high byte:

  | code | does |
  |---|---|
  | 0xFF01 | starts the three lines again: zeroes 0x33 bytes at `0x801CBB00`, uploads lines 0..2 (slots 0, 0x11, 0x22), line 0 |
  | 0xFF02 | the next line (`font_target_set(0x801CBB00 + line * 0x880)`); the third advance falls through into 0xFF01's reset |
  | 0xFF00 | waits for CROSS: makes the first pad record ready, then runs menu frames while its `+4` lacks 0x40 (no frame when it already has it) |
  | 0xFF03 | `BtlWindow0Set(2, 1, 0, 0)`, closes the window |
  | other 0xFFnn | nothing |
  | 0xF8nn | nn menu frames (nn read again every pass) |
  | 0xF9nn | `font_pen_move(nn)`, advances nn pixels |
  | 0xFAnn, 0xFBnn | nothing |
  | 0xFCnn | `font_col_set((nn << 2) & 0xFC)`: the colour bits OR'ed into every pixel `font_code_put` writes, which pick the palette entries the glyph's two values become |
  | 0xF0nn, 0xF1nn, 0xF4nn, 0xF5nn, 0xF6nn | string `code & 0xFFF` of the string table, walked by `font_string_walk` (`0x8005A884`) with `BtlMessageStringCodePut` (`0x8004C56C`) and `BtlMessageStringControlRun` (`0x8004C5A8`) |

  A case 0xF080 is written beside the string codes and can never be met:
  the value switched on is cut to its high byte.

## The field menu

### Opening and running it

**`MenuRun(page)` (`0x8006A024`)** runs the menu from a page until it closes. Its
callers: the field's button step `MapPartyStep` (`0x80056248`) with 2, when the buttons
just pressed are exactly 0x80 or exactly 0x100 (right after it prints a
debug line of the developers'); the inventory add `ChrItemGive` (`0x800577C4`),
when nobody had room for an item, and the script runner `ScriptLabelRun` (`0x80059FDC`),
when `ChrItemWaitingCheck` (`0x80057DA4`) answers that items wait, both with 0x16 and
both after `MessageStringDisp(g_ChrItemNoRoomString, 1)` (`0x8005AB98`).

* Before: the idle count `g_MapIdleCount = 0`; the six 16-bit party ids at
  `g_PartyIds` are kept in the 12 bytes at `g_MenuKeptIds`; an id of 0x1F
  or more is set to 0 for the menu's time, and its party record
  (`g_MapPartyRecords`, 0x3C bytes) gets 1 in byte `+1` (its animation
  timer stops) and bit 0x8000 in the word at `+0xE`; the counter
  `g_MapPartyBlinkCount` is kept and set to 0; `g_MenuPage = page`.
* Then `MenuOpen` (`0x8006A328`), `MenuPagesRun` (`0x8006D900`) and `MenuClose` (`0x8006A81C`).
* After: the six ids come back, byte `+1` and bit 0x8000 are cleared on all
  six records (not only the ones set), and `g_MapPartyBlinkCount` is put
  back. Then the request the menu left in `g_MenuRequest`: 1 to 4,
  `GameEnter(request)` (`0x80059498`) (a save was loaded); 5, `MapRow2Change` (`0x8006824C`); 6 to 0x50,
  nothing; 0x51 and up, first `ScriptLabelRun(0x46)` when story bit 0x29E
  is set, story bit 0x397 is clear and the script number `g_ScriptNumber`
  is 0x2C, then `g_MapFadeState = 2`, `MapFadeOut(g_MapLayersMask | 0x130,
  0x19, -1, 1, 1)` (`0x800568D4`), `ScriptChange(0x64)` (`0x80068D7C`) and `ScriptLabelRun(request)`.
  After any request it is cleared with `g_MapPartyBlinkCount`. Last, always,
  `StoryBit2CAUpdate` (`0x800585D0`).

**`MenuSaveRun` (`0x80072890`)** is the same run from the save page 0x11. Its one caller
is `MapPartyStep`, on the frame pad button 0x20 is let go (held at the call
before, not now). It sets page 0x11, `font_target_clear(0x801CBB00, 0x7F)`,
stage `g_MenuPageStage = 0`, sound request 2, `g_MapIdleCount = 0` and a
redraw request; keeps and blanks the ids and `g_MapPartyBlinkCount` as
`MenuRun` does; sets `g_MenuOnePageRunning = 1` and the member cursor 0;
runs `MenuOpen`, `MenuPagesRun` and `MenuClose`; then
`g_MenuOnePageRunning = 0`, puts everything back and calls
`MenuIconBytesDisp(0xFFFF, 1, 2, 0)` (`0x8006C21C`). It acts on no request.
`g_MenuOnePageRunning` at 1 is what `MenuSaveStep` (`0x80073054`) tests on a cancel, to
leave the menu instead of going back to page 7.

**`MenuInit` (`0x8006A228`)**, once a game (the first call of `GameStart`), zeroes the
menu's words (page, request, sound and redraw requests, the cursors), sets
the member cursors `g_MenuMemberCursor` and `g_MenuPresetMemberCursor` to
1, points `g_MenuCardBuffer` at `0x801CFB00`, copies the 12-byte text
`D_80012D34` to `D_800E4F80` and the 11-byte text `D_80012D40` after it,
to `g_GameSaveHeadText` (`D_800E4F80 + 0xC`; `GameStart` copies its first
ten bytes to the start of the saved records `g_GameSaveBlock`, so every
save's records begin with it). The save writer `MenuSaveWrite` (`0x80074024`) writes two
decimal digits of the save's number over bytes 9 and 10 of the first text,
and nothing reads those 12 bytes. Then `MenuInit` gives the menu's blocks
their fixed addresses:
`g_MenuShapes = 0x801CAB00`, `g_MenuRenderEntries = 0x801CAB78`,
`g_MenuRecords = 0x801CACE0`, `g_MenuKeptIds = 0x801CAE84`,
`g_MenuFacingWords = 0x801CAE90` and `g_MenuPoseWords = 0x801CAEA8`.

**`MenuOpen`** opens the screen: `SoundPlay(5, 1)`; sprite entries
0x87..0x95 off; `MenuRecordsInit` (`0x8006AD38`); the face words cleared and the view's
follow byte `g_MoveChrViewFollow = 0`; `CellAllClear(0x30)` (`0x80057288`);
`MenuTextStringsPut` (`0x8006A7B0`). Each party member's record gets a start and a step a
sixteenth of the way to its spot in the line-up, x =
`g_MenuLineUpSpotXs[place]` + `g_MapViewX` (0x70, 0x50, 0x98, 0xC0, 0xE8,
0x110 for places 0..5) and y = `g_MapViewY` + 0x26, and bits 0x6000 in its
`+0xE`; with the party kind `g_MapPartyKind` at 2 every record starts from
the first one's place. The layers fade (`layer_col_fade_start` (`0x8001EA74`)), the map's
objects are ticked once with `g_MapObjectsSpriteBits = 0x8000`, and 16 menu
frames walk the records there (at frame 8, with the party kind 0, each gets
its menu sprite data). Then each stands on its spot with
`MapAnimationPoseSet(record, 2, 5)` (`0x8006633C`), and the page is written.

**`MenuClose`** is its reverse: entries 0x87..0x95 off, the menu's records
4..0 off, `D_800B8EEC |= 1`, bits 0x30 of `MenuScreenBits.unk_0e` cleared,
the layers' mix and order put back, `CellAllClear(0x10)`; each record walks
over 16 frames back to its place in the ring (`MapPartyRingEntryGet` (`0x80066FB4`)), the
map's colour fades back, and the records get their field sprite data again
at frame 8. Last `MenuPageDisp(g_MenuPage)` (`0x8006B710`) with the page at 0, one frame,
and `g_MoveChrViewFollow = 1`.

**The page loop — `MenuPagesRun`.** `g_MenuBackOutLatch = 0`; then, while
`g_MenuPage` is not 0: `pad_record_update(g_MenuPadRecord, 0x14)` (`0x80026930`); the
function at entry `g_MenuPage` of the page table at `0x800B59F8`; one menu
frame (`MenuFrameStep` (`0x8006ACB0`)); a sound request `g_MenuSoundRequest` that is not 0
played (`SoundPlay(request, 1)`) and cleared; and `g_MenuBackOutPattern`
shifted right by 2. Callers: `MenuRun`, `MenuSaveRun` and `MenuNoWalkRun` (`0x80075BD8`).

**The back-out.** A page leaves on a new press of 0x130 (0x100 SELECT,
0x20 CIRCLE, 0x10 TRIANGLE) or when `g_MenuBackOutLatch &
g_MenuBackOutPattern` is not 0. Leaving, it sets `g_MenuBackOutLatch` (1
for 0x100, 2 for 0x20) and `g_MenuBackOutPattern = 0x30B30B30`. After two
shifts of the loop that word has its low bits set again, so the page the
menu returns to is left too, two passes on, and so on down to page 0: a
SELECT or CIRCLE press closes the whole menu a page at a time.
`MenuCommandStep` (`0x8006DA44`) tests only bit 0 of the pattern and clears a latch of 2,
so a run begun with CIRCLE stops at page 2.

**One frame — `MenuFrameStep`.** The party's records stepped into the
sprite list (`MapPartyAnimationStep` (`0x80065CF8`)), the view (`MapViewStep(0, 1)` (`0x80068DB8`)), the
palette and tile animations, the menu's seven records
(`MenuRecordsAnimationStep` (`0x8006BE10`)), `sprite_list_draw` (`0x8001A798`); on pages 0x11, 0x12 and
0x14 `g_MenuCardAcceptResult = card_accept_poll()`; the end of the frame
(`MapFrameEndCopy` (`0x800568AC`)); and `MapBgChangeStep` (`0x80059A18`).

**The pad record.** `g_MenuPadRecord` is a `PadRecord`: the steps read the
pressed-or-repeating word (`g_MenuPadRecord[2]`, +4) for directions, L1
(0x4) and R1 (0x8), and the newly pressed word (`g_MenuPadRecord[1]`, +2)
for CROSS (0x40) and the back-out bits. The button names are the standard
pad bits.

### The page table

`MenuPagesRun` calls through the 23 words at `0x800B59F8`. The C reaches
them as `battle_phase_pre_table`, and entries 0xB on as `battle_init_table`
(`0x800B5A24`): two labels the first split named and nobody read; the
words are one table, the menu's pages.

| page | handler | what |
|---|---|---|
| 0, 1, 8 | `MenuPageNumberHang` (`0x8006D9A8`) | no page: one menu frame, then string 0x612 ("MenuError") with the page number in two digits through `MessageDisp` (`0x8005AD50`), and a loop with no end; 0 is never called (the loop stops on it), and no page sets 1 or 8 |
| 2 | `MenuCommandStep` | a member's command list (the menu's first page) |
| 3 | `MenuSpellStep` (`0x8006EE68`) | Magic: a member's spells |
| 4 | `MenuItemCommandStep` (`0x8006DD94`) | Items: Use, Give, Drop |
| 5 | `MenuWornItemStep` (`0x8006F408`) | Equipment: worn items and items |
| 6 | `MenuChrRecordStep` (`0x8006FC74`) | Status: the character record |
| 7 | `MenuPage7Step` (`0x80072A08`) | System: Save, Load, Setup |
| 9 | `MenuPresetStep` (`0x8007070C`) | Tactics: the command set-up, the slot step |
| 0xA | `MenuBtlPlacesStep` (`0x80071D50`) | Formation |
| 0xB | `MenuItemUseStep` (`0x8006E05C`) | Use Item |
| 0xC | `MenuItemGiveStep` (`0x8006E2A0`) | Give Item |
| 0xD, 0x16 | `MenuItemDropStep` (`0x8006E4A0`) | Drop Item; 0x16 is the same page when nobody had room for an item |
| 0xE | `MenuPresetKindStep` (`0x800713B8`) | Tactics: the kind row |
| 0xF | `MenuPresetSpellStep` (`0x800715A0`) | Tactics: the spell picker |
| 0x10 | `MenuPresetItemStep` (`0x80071984`) | Tactics: the item picker |
| 0x11 | `MenuSaveStep` | Save |
| 0x12, 0x14 | `MenuLoadStep` (`0x80074ADC`) | Load (0x14 from the start screen and after a lost battle) |
| 0x13, 0x15 | `MenuOptionStep` (`0x80075410`) | Setup, the options (0x15 from the start screen) |

The words in the third column are the string table's for the pages'
command bytes (`MenuCommandsSet` (`0x8006C5A4`), below). `MenuPageNumberHang` is a stop
for a page with no handler, not game flow; `MenuUseNumberHang` (`0x80076688`) does the
same for a use (§"The use list").

**How the pages are reached.** Page 2's command bytes are
`g_MenuMemberCommands` (EA EC FB FA: Magic, Items, Equipment, Status) for
a character and `g_MenuCommands0C` (FC EC E7 F1: System, Items, Tactics,
Formation) for id 0xC, the party's own entry (`MenuNoWalkOpen` (`0x80075CE0`) puts 0xC
first in the party list). CROSS sends a character to page cursor + 3
(3, 4, 5, 6) and id 0xC to cursor + 7 (7, 9, 0xA) except for its Items
(cursor 1), which goes to page 4 as well. Page 4 sends its cursor + 0xB
(0xB, 0xC, 0xD) and page 7 its cursor + 0x11 (0x11, 0x12, 0x13).

### The menu's display layer

`MenuRecordsInit` builds it, from `MenuOpen`, from `MenuNoWalkOpen` and
from `MenuUsePlaceNameRun` (`0x80076968`). It empties three blocks and fills them, placed
from the view (`g_MapViewX`, `g_MapViewY`):

* **`g_MenuShapes`**: five 0x18-byte shapes of the kind `ShapeDraw` (`0x8001B754`) and
  the sprite pass draw (+0 kind 0x4000 a rectangle or 0x5000 an outline of
  four points, +6 a colour index, the points from +8). Shape 0: kind 0x4000,
  colour 0x761, width and height words 0x1F and 0xF at `+0x10` / `+0x12`;
  it is copied to 1..4. Shape 3 becomes an outline, colour 0x7E5, through
  (−3, −3), (0xE2, −3), (0xE2, 0x1A), (−3, 0x1A); shape 4 gets colour 0x76A
  and 0x3F at `+0xC` and `+0x10`. Shape 0 is the bar over the cursor's row;
  the pages resize it (`MenuPresetOpen` (`0x800711A8`) to 0x80 × 0x10, `MenuBtlPlacesOpen` (`0x80072464`)
  to 0x40 × 0x10, the save list to 0x90 × 0x20 or 0x50 × 0x10).
* **`g_MenuRecords`**: seven 0x3C-byte records of the party records' form,
  bound to resource 2's bank `g_SpriteData0` (as `ShopObjectInit` (`0x80054948`) binds the
  shop's), flags 0x6360 at `+0xE`, `+0x32 = 0xFFFF`, at the view +
  (0x20, 0x10) and, for records 1..4, (0x68, 0x13), (0x60, 0x98), (0x68,
  0x13) and (0x60, 0x10); animations 0xA, 8, 0xA, 9, 0xA, 4 and 5 (by
  `MapAnimationSet` (`0x800663EC`)). Record 0 marks the command column's cursor, 1 the
  member pick's marker, 2 the item cursor, 3 the member cursor, 4 the
  presets' cursor, and 5 and 6 the marks above and below a list (5 is on
  while the list is scrolled, 6 while more rows follow).
* **`g_MenuRenderEntries`**: ten 0x24-byte sprite-list entries. Entries 0
  and 1 are record 0's image as a still sprite (the first step of
  animation 0xE) at x 0x3A and 0xAA; entries 2..6 draw shapes 0..4 (entry 2
  the bar, entry 3 the answer prompt's cursor); 7 and 8 copy entry 6, and 9
  copies entry 5 at y + 0x60.

`MapAnimationSet(record, animation)` is the field's "give a record an
animation": nothing when it already runs it; otherwise the old number goes
to `+0x32`, the new to `+0x30`, and `MapAnimationStart(record, restart)` (`0x80066468`)
puts it at the first step, with `restart` 0 only when both numbers are in
1..60 and in the same group of four (a change of facing alone keeps the
timer).

**`MenuRecordsAnimationStep`**, from `MenuFrameStep`: entries 0x8F..0x95
off; for each of the seven records the timer `+0x34` goes up, at the
step's length `+0x36` the step `+0x38` goes on, and a step at the end
`+0x3A` holds the last step when the animation's count has its top bit, or
restarts (`MapAnimationStart(record, 1)`). The record's 0x24-byte image is
copied to sprite entry 0x95 − i with the step's picture at its `+0x18`, and
`OutlineMapStep(g_MenuRecords, i, 0)` (`0x8001BCA0`) draws the animated outline round
records 0..4 that are on. It steps every record, on or off.

**The line-up — `MenuLineUpDisp(slot, mark, own, item)` (`0x8006BFC0`).** The end of
every page's frame. With `item` 0 or more it first calls
`MenuPoseWordsSet(item)` (`0x8006B48C`). Then for each place with an id: the party record
gets byte `+1 = 1` (held); on the member cursor's place, record 3 goes
over the member (x − 8, y − 0x15) and with `own` set the pose becomes 1; on
the place of `slot` the pose becomes 1 unless the page is 0xC, and with
`mark` set the record is let go (it alone animates) and record 1 goes over
it the same way. Last `MapAnimationPoseSet(record, pose, facing)` from the
place's two words. `g_MenuMemberPlaces` maps a cursor to a place: 1, 0, 2,
3, 4, 5, so that with the spots' x the cursors run left to right.

**The poses by page — `MenuPoseWordsSet(item)`** sets the pose word
(`g_MenuPoseWords`) and the facing word (`g_MenuFacingWords`) of each
place:

| page | effect |
|---|---|
| 0xC (Give) | per member, below |
| 9, 0xE, 0xF, 0x10 | every place: facing 5, pose 1 |
| 0xA | every place: facing 7, pose 2 |
| any other | every place: facing 5, pose 2 |

On page 0xC, for each place up to the first empty one (the places after it
keep their words): the facing is 1 (cannot take the item) when the id is
8, 9 or 10, when `ChrItemRoomCheck(id)` (`0x80057BE8`) answers 0, or when the item is
0xA8; otherwise 5 when `ChrItemUseGet(0x10, item, 1)` (`0x80058088`) is not 0 (bit 15 of
the item's mask is clear) or the id is below 2 or above 10; otherwise 1.
Those are the tests `ChrItemGive` makes, so the line-up shows in advance
who would take the item; 0xA8 is the one rule of its own, refused here and
not in `ChrItemGive`. The pose is 1 when the item's kind (`+4` of its 0x12-
byte row) is 5 or more or `ChrItemUseGet(id, item, 1)` answers 0 (bit id −
1 of the mask is set: the member may wear it), otherwise 2. The kind byte
is read for any item, so item 0 reads the 0x12 bytes before the table.
Callers: `MenuOpen` with the page, and `MenuLineUpDisp` with its fourth
argument.

**The walk to a line-up and back — `MenuLineUpMove(xs, ys)` (`0x8006D2F4`) /
`MenuLineUpSpotsMove` (`0x8006D5CC`).** `MenuLineUpMove` empties the page
(`MenuCommandsSet(0, 0)`, `MenuCommandsDisp(4)` (`0x8006C178`), `MenuIconBytesSet(0, 0)` (`0x8006C6F8`),
`MenuIconBytesDisp(0xFFFF, 1, 2, 0)`, `MenuPageDisp(0xFFFF)`,
`font_target_clear(0x801CD500, 0x4C)`), then walks every member's record an
eighth of the way to `(xs[place], ys[place])` each frame for eight menu
frames (on frames 0, 2, 4 and 6 the text upload block `D_800E4768` sends
0x801CDB00 + frame × 0x400, on frame 7 0x801CD500), puts each on its point
and gives it `MapAnimationPoseSet(record, -1, 5)`. `MenuLineUpSpotsMove`
walks them back to the line-up's spots the same way (uploads on frames 2
and 4) and sets `+8` to the view's y + 0x106. The first is called by
`MenuPresetOpen` and `MenuBtlPlacesOpen`, the second by `MenuPresetClose` (`0x800712E4`)
and `MenuBtlPlacesClose` (`0x800727C0`).

### The machinery the pages share

**The cursor in a row of six — `MenuCursorMove(cell, cells, bits)` (`0x8006B33C`).** It
answers the cell a cursor moves to and stores nothing. `cell = row * 6 +
column`; only the column changes. `bits & 0x6000` (right, down) steps +1,
else `bits & 0x9000` (up, left) −1, else the cell comes back as it was. The
column wraps inside 0..5. With no list (`cells == 0`) the first step is the
answer. With a list it steps on over zero bytes; a whole lap back to its
own column ends on that column when `cells[cell]` (indexed by the whole
cell number) is not 0 and on column 0 when it is. So an empty row parks the
cursor on its first cell, and the answer never leaves the row. The same
routine serves the six-column grids (left and right) and the six-entry
lists (up and down, `cell < 6`). Callers pass `g_MenuCommands`
(`MenuCommandStep`, `MenuItemCommandStep`, `MenuPage7Step`),
`g_MenuIconBytes` or `g_MenuIconBytes + 6` (the item pages, the spell page,
the pickers, `MenuPresetStep`), `g_MenuPresetKindIcons`
(`MenuPresetKindStep`), or no list (`MenuWornItemStep`).

**The member cursor — `MenuMemberCursorMove(&cursor, bits)` (`0x8006B2B4`).** `bits &
0x6008` (right, down, R1) +1, `& 0x9004` (left, up, L1) −1, wrapping in
0..5, and on until the place `g_MenuMemberPlaces[cursor]` holds an id: an
empty place is never stopped on (with no id at all it would not end). The
pages loop round it to step over ids they refuse (0xC; or 8, 9 and 10).

**The command column — `MenuCommandsSet(who, page)` /
`MenuCommandsDisp(cursor)`.** `MenuCommandsSet` zeroes the six bytes
`g_MenuCommands` and writes the page's own. A command byte is an icon
number; its strings are the byte + 0x100 and + 0x200, and the names below
are the string table's.

| page | written |
|---|---|
| 1, 2 | four: `g_MenuMemberCommands` (Magic, Items, Equipment, Status) for `who != 0xC`, `g_MenuCommands0C` (System, Items, Tactics, Formation) for 0xC |
| 4 | three: `g_MenuItemCommands` (Use Item, Give Item, Drop Item: F7 F8 F9) |
| 5, 6 | one: `g_MenuMemberCommands[g_MenuCommandCursor]` |
| 7 | three: `g_MenuPage7Commands` (Save, Load, Setup: FE FF FD) |
| 9, 0xE, 0xF, 0x10 | one: `g_MenuCommands0C[g_MenuCommandCursor]` |
| 0xA | one: 0xF1 (Formation) |
| 0x11..0x13 | one: `g_MenuPage7Commands[page − 0x11]` |
| 0x14 | one: 0xFF (Load) |
| 0x15 | one: 0xFD (Setup) |
| 0x16 | one: `g_MenuItemCommands[2]`, 0xF9 (Drop Item) |
| any other | nothing: all six stay 0 |

`MenuCommandsDisp(cursor)` puts the first four bytes down the left as
icons, `IconCellSet(4, i * 3 + 2, byte, (cursor != i) << 28)` (dimmed but
on the cursor's row; a byte of 0 clears the cell), calls
`CellAllUpdate(0x10)` (`0x8005AD8C`), and puts record 0 at y = cursor × 0x18 +
`g_MapViewY` + 0x10. With 4 no row is lit.

**The icon grid — `MenuIconBytesSet(who, page)` / `MenuIconBytesDisp(slot,
mode, rows, cursor)`.** `g_MenuIconBytes` is twelve bytes, two rows of six.
`MenuIconBytesSet` zeroes them and, by the page: page 2 for id 0xC, twelve
bytes of the 0x60-byte list at `g_ChrItemList0C` from the scroll
`g_MenuItemListScroll` (pulled back while the row after it is empty, the
list cursor `g_MenuItemListCursor` moving down a row with it); page 2 for a
character with page 2's cursor on 0, and page 3, the spells
(`MenuIconBytesSpellsSet` (`0x8006C84C`): the eight spell bytes at `+0x2A` of the
character record, each + 0xB6); page 2 with the cursor elsewhere, and page
5, the twelve bytes from `+0x1E` (six worn items, six items); pages 9,
0xE, 0xF and 0x10, bytes 6..8 = 0xE8, 0xF2, 0xF6. `MenuIconBytesDisp`
shows them: mode bit 0 from grid row 0x13 with the face word
`g_MessageFaceFirst`, side 0; else bit 2 from row 2, the same face word
(with `g_MessageFacePlaces & 8` for a member); else row 0xB,
`g_MessageFaceSecond`, side 0x8000 (with `g_MessageFacePlaces & 4`). Byte
`i * 6 + j` is `IconCellSet(j * 4 + 0xC, row + i * 4, byte, 0)`; on page 5
that cell is cleared instead and the icon goes to `(j * 5 + 7, i * 4 +
8)`. Record 2 (the item cursor) goes to x = (cursor % 6) × 0x20 +
`g_MapViewX` + 0x60, y = (cursor / 6) × 0x20 + `g_MapViewY` + 0x98 (page
5: × 0x28 + 0x38 and + 0x40). Last the face: `MessageFaceEntrySet(
g_MenuMemberPlaces[slot] * 5 + 1, …)` (`0x8005B448`), 0 for none. A face that has to come
off the disc runs frames inside this call.

**The text under the cursor — `MenuCursorTextDisp(number, colour)` (`0x8006C8F0`).**
Strings number + 0x200 and number + 0x100 written into blocks 0x34 and
0x40 of the menu's text (`0x801CD500` and `0x801CDB00`), in the colour
given (4 dims), and sent to video memory (`MenuTextPartsLoad(3)` (`0x8006C964`)); a page
shows them with `MenuTextPairDisp` (`0x8006B9A4`), the + 0x100 string over the + 0x200
one. By the string table these are an item's or a command's name and its
description.

**The page's furniture — `MenuPageDisp(page)`.** `CellAllClear(0x20)`
first. Pages 1..4, 7, 0xB, 0xC, 0xD and 0x16: `MenuPartyStatusDisp` (`0x8006BA34`) (the
members' boxes and the silver) and `MenuTextPairDisp(0xC, 0xE)`. Page 5:
`MenuTextPairDisp(0xC, 0x10)`, the member's glyphs
(`CellChrGlyphSet(id, 3, 0x10, 0xDEAD2001, 0x3000, 1)` (`0x8005754C`)), the tall glyphs
0xA and 0xB at (7, 0x10) and (8, 0x10), the level (`+0x1C`) in two tall
digits at (9, 0x10), and the text blocks 0x78 (code 0xEEE0, three blocks) at
(2, 8) and 0x7B (0xEEEC, two blocks) at (2, 0xC). Page 6:
`MenuChrRecordTextDisp` (`0x8006BCB8`). Pages 9 and 0xE..0x10: for the cursors 2, 3, 4,
5, 0 and 1 whose id is neither 0 nor 0xC, the n-th found gets eight blocks
from block 0x48 + n × 8 at column 0x10, row 3 + n × 3; then
`MenuTextPairDisp(0xC, 0x12)`. Pages 0x11, 0x12 and 0x14: `MenuCardListDisp` (`0x800734F0`)
and `MenuCardResultDisp` (`0x8006B65C`). Pages 0x13 and 0x15: `MenuOptionTextDisp` (`0x80075900`). Pages
0, 8, 0xA and 0x17 up (0xFFFF among them): nothing more.
`CellTextDisp` (`0x800572E0`)'s code argument is `0xED00 + 4 * block`: block b of the text
at `0x801CBB00` (blocks of 0x80 bytes).

**The two-choice prompt — `MenuAnswerRun(string, -, column, row,
lines)` (`0x8006CC84`).** It writes a string with two answers under it and runs a cursor
between them. It plays and clears the sound request, clears the rows
(`CellRowsClear(row, lines * 2, 0x20)` (`0x800573E0`)), shows the silver
(`MenuMoneyDisp(3, 0xF)` (`0x8006BC4C`)) when the row is below 0x12, lets one frame go,
writes the string and string 0x637 (the two answers, side by side) into the
text and shows them, and puts render entry 3 (the cursor) at y =
`g_MapViewY` + row × 8, two rows lower when `lines` is not 1. Its second
argument is not read. The answer starts on the right (1). Each pass reads
the pad, then:

* `+4 & 0xA000` (left / right): sound request 1, the answer turns over.
* else a press of 0x130 or the back-out latch: the latch set as the pages
  do, sound request 3, **returns 1**, with no frame passed.
* else a press of 0x40: sound request 2 for 0 and 3 for 1; **returns the
  answer** (0 left, 1 right), with no frame passed.
* otherwise shape 1 is sized and the cursor placed (x = column × 8 +
  `g_MapViewX` − 1 on the left answer, + 0x20 on the right), the entry is
  copied to sprite entry 0x88, a pending sound is played, and one frame
  passes. After that frame, on page 0x11 a non-zero `card_change_check()` (`0x80025638`)
  returns 2 for string 0x670 and otherwise sets stage 0 and a redraw, calls
  `MenuSaveOpen` (`0x80072F58`) and returns 1; on pages 0x12 and 0x14 it sets stage 0 and
  returns 1.

So 1 is "no" as well as "left", and 2 is page 0x11's exit for string
0x670 when the card changes. On the way out sprite entries 0x87..0x8E go
off and the text is emptied. Callers: `MenuItemDropStep`
`(0x65F, 0x801CC380, 0xC, 0xE, 2)`, and `MenuCardRead` (`0x80072C1C`), `MenuSaveWrite`
`(0x638, 0, 0x19, 0x14, 1)` and `MenuLoadStep` on the save pages.

**The member pick — `MenuMemberPickRun(exclude)` (`0x8006CA40`).** It runs the pick of a
member on the line-up and returns the id picked, or 0. It plays and clears
the sound request, moves its cursor `g_MenuMemberPickCursor` forward off
the excluded id, and switches record 1 (the marker) on. Each pass:
`MenuLineUpDisp(cursor, 1, 1, -1)`, one menu frame, the pad: `+4 & 0x8004`
steps back and `& 0x2008` forward, past the excluded id (sound request
1); a press of 0x130 or the latch sets the latch, leaves sound request 3
and returns 0; a press of 0x40 takes the cursor's place only when its
facing word (`g_MenuFacingWords`) is 5, leaving sound request 2 and
returning the id, and otherwise sets sound request 4. Record 1 goes off on
the way out. Callers: `MenuSpellStep` and `MenuItemUseStep` with 0xC, and
`MenuItemGiveStep` with the giver; on page 0xC the facing words come from
`MenuPoseWordsSet`, so only a member who can take the item is accepted.

**The notice — `MenuNoticeDisp(string, column, row, lines, wait)` (`0x8006D06C`).** With
a string: the sound request played and cleared; the rows cleared; blocks
from block 0x50 (`0x801CE300`) emptied, `lines × 0xC` of them; the lines
written in colour 8 when `g_MenuNoticeCol8` is set (`MenuCardResultDisp`
sets it; it is cleared after), line i being string + i at block 0x50 +
i × 0xC, shown by `CellTextDisp(0xC, row + i * 2, 0xC, 1, 0xC, 0xDEAD2001,
0xEE40 + i * 0x30)`. With `wait`: menu frames until a press of 0x50, then
sound 2. With `wait`, or with no string: the text emptied and one strip
shown at (column, row) with the code `0xEE40 + i * 0x30`; with no string
the line loop has not run and `i` is whatever the C leaves in it. Every
caller passes `wait` 0 (`MenuCardResultDisp`, `MenuCardRead`,
`MenuSaveWrite`, `MenuLoadStep`), so a notice stays up and no frame
passes.

### Page 2: a member's commands — `MenuCommandStep`

`who` is the id under the member cursor; the cursor `g_MenuCommandCursor`
runs over `g_MenuCommands`. A latch of 2 is cleared on entry. First match:

* `+4 & 0x5000` (up / down): `MenuCursorMove(cursor, g_MenuCommands,
  bits)`; sound request 1, redraw.
* `+4 & 0xA00C` (left, right, L1, R1): `MenuMemberCursorMove`; sound
  request 1, redraw.
* the back-out (with `g_MenuBackOutPattern & 1` only): the latch set, page
  0, records 1 and 0 off, the page emptied (`MenuCommandsSet(0, 0)`,
  `MenuCommandsDisp(4)`, `MenuIconBytesSet(0, 0)`, `MenuIconBytesDisp(
  0xFFFF, 1, 2, 0)`, `MenuPageDisp(0xFFFF)`); sound request 6.
* a press of 0x40: ids 8, 9 and 10 are refused commands 1 and 2 (Items,
  Equipment) with sound request 4. Otherwise sound request 2 and the page is
  cursor + 3, or cursor + 7 for id 0xC with the cursor not on 1. A character
  on command 0 first gives record 0 animation 0xB. On acceptance records 1
  and 0 go off, `font_target_clear(0x801CD500, 0xC)`, `MenuTextPartsLoad(1)`,
  `MenuPageDisp(1)` and `MenuCommandsSet(who, page)` run before the new
  page's redraw is asked for.
* the redraw (`g_MenuRedrawRequest == 2`): `MenuCommandsSet`,
  `MenuCommandsDisp(cursor)`, `MenuIconBytesSet`, `MenuIconBytesDisp(
  g_MenuMemberCursor, 1, 2, 0xC)`, `MenuCursorTextDisp(command, 4 for a
  refused pair, else 0)`, record 0 to animation 0xA, records 1 and 0 on.

Every call ends with `MenuLineUpDisp(g_MenuMemberCursor, 1, 0, 0)`.

### Page 3: Magic — `MenuSpellStep`

The icon bytes are the member's spells (`MenuIconBytesSet(id, 3)`: the
eight spell bytes at `+0x2A`, each + 0xB6), in a grid of two rows of six;
the cursor is `g_MenuSpellCursor`.

* `+4 & 0xA000`: `MenuCursorMove` along the row; sound request 1.
* `+4 & 0x1000` / `0x4000`: up a row (cursor − 6) when the cursor is 6 or
  more, down (+ 6) when it is below 6; sound request 1.
* `+4 & 0xC` (L1 / R1): the member cursor moves, again while the id is
  0xC; sound request 2.
* the back-out: records 3, 2 and 0 off, record 0 to animation 0xA, page 2,
  `MenuPageDisp(2)`, sound request 3.
* a press of 0x40: sound request 2; the spell is the icon byte + 0x4A as a
  byte (0 for an empty cell); `ChrSpellUseGet(id, spell, 2)` (`0x80058698`), the cost and
  use question; for icon byte 0xCA (spell 0x14, whose name is string
  0x1CA, "Escape Litany") with story bit 0x2D2 clear the answer is −1.
  Below 0: sound request 4. Otherwise record 2 to animation 0xB; the
  target is 0xC for an answer below 5 and `MenuMemberPickRun(0xC)` for 5 and
  up; with a target the sound request is played and cleared and
  `MenuUseRun(id, icon byte, target)` (`0x80076444`) uses the spell (it is handed the icon
  byte, not the spell number); record 2 back to 0xA.
* the redraw (`g_MenuRedrawRequest == 3`): the icon bytes again, a cursor
  on an empty cell moved (along the row, then the other row, then 0), the
  page, and for a spell its cost, byte `+4` of its 12-byte row
  (`g_ChrSpellRows`), halved rounding up when worn item 5 or 6 (`+0x22` or
  `+0x23` of the character record) is item 0x88, as three tall digits at
  (0x1E, 0x17), a "/" and the member's MP (`+4`) at (0x22, 0x17) in colour
  0x2000 at its maximum (`+6`) and 0x3000 below; then one frame, the grid
  (`MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, cursor)`), and the strings
  in colour 4 when `ChrSpellUseGet` answers −1 or story bit 0x2D2 keeps
  0xCA from being used.

Every call ends with `MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0)`.

### Page 4: Items — `MenuItemCommandStep`

The cursor `g_MenuItemCommandCursor` runs over the three command bytes.
`+4 & 0x5000` moves it; L1 / R1 (`0xC`) move the member, again while the
id is 8, 9 or 10; left and right do nothing. The back-out: page 2, records
6, 5, 3 and 0 off, `MenuPageDisp(2)`, sound request 3. A press of 0x40 on
Use Item (entry 0) while story bit 0x3D9 is set is refused with sound
request 4 (and the redraw dims that entry); otherwise records 3 and 0 off,
record 0 to animation 0xB, the page becomes cursor + 0xB,
`font_target_clear(0x801CD500, 0x14)`, `MenuTextPartsLoad(3)`,
`MenuPageDisp(page)`, and one frame passes. The redraw (request 4) shows
the commands and the member's items (`MenuItemIconBytesDisp(who, which,
0x2000)` (`0x8006EBEC`)). Story bit 0x3D9 also turns Items off in the battle's command
grid.

### Pages 0xB, 0xC, 0xD and 0x16: the item grid

**The pad — `MenuItemPadStep(&who, &which)` (`0x8006E80C`).** Called first by the three
item pages; it returns 1 when it did something with the pad. `who` is the
id under the member cursor and `which` is `who == 0xC`: id 0xC's list of
0x60 bytes scrolls (scroll `g_MenuItemListScroll`, cursor
`g_MenuItemListCursor`), a character's twelve bytes (six worn items, six
items) do not (cursor `g_MenuItemCursor`).

* `+4 & 0xA000`: unless the cursor is 0 on an empty cell, `MenuCursorMove`
  steps until it lands on a byte that is not 0; sound request 1, redraw.
* `+4 & 0x1000`: cursor − 6 when it is 6 or more; otherwise, on the list,
  the scroll goes back a row while it is not 0. Sound request 1.
* `+4 & 0x4000`: cursor + 6 when it is below 6 and icon byte 6 is not 0;
  otherwise, on the list, the scroll goes on a row while it is below 0xF and
  the row two below is not empty. Sound request 1.
* `+4 & 0xC`: the member cursor moves, again while the id is 8, 9 or 10;
  both outputs written again; sound request 2, redraw.
* the back-out: the latch set; records 3, 2 and 0 off. On page 0x16 with
  `g_ChrItemNoRoomState == 3`: sound request 4, redraw, return 1. On page
  0x16 otherwise: `g_ChrItemNoRoomState -= 2`, the page cleared and drawn
  again (`MenuCommandsSet`, `MenuCommandsDisp`, `MenuIconBytesSet`,
  `MenuIconBytesDisp`, `MenuPageDisp`), and the page becomes 0. Any other
  page: page 4. Sound request 3.

**The grid's redraw — `MenuItemIconBytesDisp(who, which, dir)`.** Records
5 and 6 (the scroll marks) go to x = `g_MapViewX` + 0x120, y = `g_MapViewY`
+ 0x98 / + 0xC8; `MenuPageDisp`. The page: for a character the twelve
bytes at `+0x1E` of its record; for the list `g_ChrItemList0C` + scroll ×
6, and while the page's second row is empty and the scroll is not 0 the
list scrolls back a row (a list cursor in the top row moving down with its
item). Both marks off; for the list, mark 5 on when the scroll is not 0 and
mark 6 on when the scroll is below 0xF and the row under the page is not
empty. `MenuIconBytesSet(who, page)`, then the twelve bytes over
`g_MenuIconBytes`. A cursor on an empty cell is repaired:
`MenuCursorMove(cursor, cells, dir)`; still empty, the list goes back a row
when in its second row, a character's top row tries the cell below and then
0, and its second row takes the cell above unchecked. Last
`MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, cursor)`, with 0xC (off the
grid) as the cursor on page 4. Callers: page 4 with `dir` 0x2000, the three
item pages, and `MenuUseMoviePlay` (`0x800778C8`) with 0.

**Page 0xB, Use Item — `MenuItemUseStep`.** After the pad step, on a press
of 0x40: sound request 2; `r = ChrItemUseGet(who, item, 3)`, the high
nibble of the item's byte `+6`. Item 0xA4 with story bit 0x2D2 clear and
item 0x90 with story bit 0x2D3 clear give −1 (by the string table, 0xA4 is
the Dragonfly Wing and 0x90 the Dragon Wings; bit 0x2D2 also keeps spell
0xCA back on page 3). Below 0: sound request 4. Otherwise record 2 to
animation 0xB; the target is 0xC for `r` below 5 and
`MenuMemberPickRun(0xC)` for 5 and up; with a target the sound request is
played and cleared, `MenuUseRun(who, item, target)` uses the item (and
`ChrItemRemove` (`0x80057A54`) takes it), and the redraw's direction becomes 0x8000. A
pick that is left uses nothing. The redraw dims the strings when the check
answers −1 or the story bit is clear.

**Page 0xC, Give Item — `MenuItemGiveStep`.** After the pad step, on a
press of 0x40: sound request 2; refused (sound request 4) with fewer than
two in the party (`g_PartyCount < 2`) or on an empty cell. Otherwise record
2 to animation 0xB and `MenuMemberPickRun(who)`; for a member picked the
item comes off the giver, through `ChrWornItemRemove(who, cursor + 1)` (`0x80058038`) for a
character's first six bytes (its worn items) and `ChrItemRemove(who, item)`
for any other, and `ChrItemGive(picked, item)` gives it, its answer not
looked at; the direction becomes 0x8000. The strings are never dimmed. It
ends with `MenuLineUpDisp(g_MenuMemberCursor, 0, 0, item)`, so the line-up
shows who can take the item and the pick takes only those.

**Pages 0xD and 0x16, Drop Item — `MenuItemDropStep`.** Page 0x16 is the
same page, opened by `MenuRun(0x16)` when nobody had room for an item. On
page 0x16, while `g_ChrItemNoRoomState` is below 2: `MenuCommandsSet(0,
0x16)`, `MenuCommandsDisp(0)`, and the state goes up by 2. After the pad
step, on a press of 0x40: sound request 2; an item for which
`ChrItemUseGet(who, item, 2)` is below 0 (no sale price) is refused with
sound request 4, and so are a character's first six bytes on page 0x16.
Otherwise record 2 to animation 0xB and `MenuAnswerRun(0x65F, 0x801CC380,
0xC, 0xE, 2)`; on answer 0 the item goes: `ChrWornItemRemove(who, cursor +
1)` for a worn item, and the cursor moved with `MenuCursorMove(cursor,
g_MenuIconBytes, 0x2000)`; `ChrItemRemove(who, item)` for any other, and
then on page 0x16 either, with the state at 2, `ChrItemGive(0,
g_ChrItemNoRoomItem)`, `g_ChrItemNoRoomItem = 0`, `StoryBitClear(0x2D6)` (`0x80057730`) and
sound request 0xC, or the state set to 2 when `ChrItemWaitingGive` (`0x80057DE0`) answers
not 0; the direction becomes 0x8000. Whatever the answer, record 2 goes
back to 0xA and a redraw is asked for. The redraw (0xD or 0x16):
`MenuItemIconBytesDisp(who, which, dir)`, then `MenuCursorTextDisp(item,
colour)`, the colour 4 (dimmed) when `ChrItemUseGet(who, item, 2)` answers
−1 and, on page 0x16, also for a character's first six bytes; otherwise 0. Every call ends with
`MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0)`.

### Page 5: Equipment — `MenuWornItemStep`

On its first call (`g_MenuWornItemOpen == 0`) it writes strings 0x687
("Gear") into five blocks at `0x801CF700` and 0x1EC ("Items") at
`0x801CF880`, and sets `g_MenuWornItemOpen`. The cursor
`g_MenuWornItemCursor` runs over the icon bytes: 0..5 the worn items, 6..11
the items.

* `+4 & 0xA000`: `MenuCursorMove(cursor, 0, bits)`, with no list, so empty
  cells are not stepped over; `& 0x1000` / `0x4000` a row up or down,
  redrawn only when there is a row to move to; sound request 1.
* `+4 & 0xC`: the member, again while the id is 8, 9, 10 or 0xC; sound
  request 2.
* the back-out: records 3 and 2 off, `MenuIconBytesSet(who, 0)`,
  `MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, 0xC)`, sprite entries
  0x87.. off, page 2, `MenuPageDisp(2)`, `g_MenuWornItemOpen = 0` (the
  labels are written again next time), sound request 3.
* a press of 0x40: `MenuWornItemChange` (`0x8006F69C`).
* the redraw (request 5): `MenuWornItemDisp` (`0x8006F850`), records 3 and 2 on.

**What can be taken off or put on — `MenuWornItemColGet(who)` (`0x8006FA50`).** 4 when
the cell is empty; for a worn item, 4 when none of the six item cells is
empty (no room for it), else 0; for an item, 4 when its kind (`+4` of its
row) is 0 and story bit 0x2D4 is set, otherwise 0 when `ChrItemUseGet(who,
item, 1)` answers 0 (the member may wear it) and 4 when not. 4 is the
colour the strings are dimmed with.

**The pick — `MenuWornItemChange`.** A 4 from `MenuWornItemColGet` is
refused with sound request 4 and nothing else. Otherwise one of three arms,
then sound request 2 and a redraw. A worn cell: `old =
ChrWornItemRemove(who, cursor + 1)` takes it off and `ChrItemGive(who,
old)` puts it with the items. An item of a kind other than 4: the place is
the kind + 1, and whatever is worn there comes off. An item of kind 4, the
kind with two places (5 and 6, `+0x22` / `+0x23`): when `+0x23` holds
something, place 5 is taken off, `+0x23`'s item is copied down to `+0x22`
and `+0x23` cleared; the new item goes to place 6 when `+0x22` holds
something, else to 5. So the two places are a queue: the new item goes
behind what is worn, and with both full the one in place 5 comes off. The
moved item stays worn through plain byte stores, so the bonuses are added
and taken back (`ChrWornItemValuesAdd` (`0x800587D0`) / `ChrWornItemValuesRemove` (`0x8005886C`)) only
for the item that comes off and the one that goes on. Both put-on arms end
`ChrWornItemSet(who, place, item)` (`0x80057FFC`), `ChrItemRemove(who, item)`,
`ChrItemGive(who, old)`.

**The draw — `MenuWornItemDisp`.** Render entries 5 and 9 go to sprite
entries 0x8A and 0x8E; then the page (`MenuIconBytesSet`,
`MenuIconBytesDisp(slot, 1, 2, cursor)`, `MenuCommandsSet`,
`MenuCommandsDisp(0)`, `MenuPageDisp`). With `bad =
MenuWornItemColGet(who)`, the pair compared is, for a worn cell, nothing
new against that item (what taking it off changes) and, for an item, that
item against the one worn at its kind's place (kind 4: place 6 when `+0x23`
is empty, else place 5, the shop's rule). The lookup runs for an empty cell
too, reading the 0x12 bytes before the item table. Only when `bad` is 0:
`ChrWornItemChangeMake(who, buf, new, old)` (`0x800581B8`) and `MenuChangeRowsDisp(who,
buf)` (`0x8006FB3C`) draw up to three rows (label, now, after) from text row 0x14, two
apart. Then `MenuCursorTextDisp(item, bad)`.

### Page 6: Status — `MenuChrRecordStep`

`D_800B8EF4[0]` is the tab: 1 the numbers, 2 the six worn items, 4 the
eight spells. On the first call (tab 0) `MenuChrRecordOpen` (`0x800704F4`) runs and the tab
becomes 1. Left: 1 to 4, 2 to 1, 4 to 2; right: 1 to 2, 2 to 4, 4 to 1; so
tab 3 (the six items, which `MenuChrRecordDisp` (`0x8006FEBC`) can draw) is never shown.
L1 / R1 move the member, again while the id is 0xC: ids 8, 9 and 10 are
not stepped over here, unlike pages 4 and 5. The back-out:
`MenuChrRecordClose` (`0x80070630`), page 2, tab 0, `MenuPageDisp(2)`, sound request 3.
Up, down and CROSS do nothing. The redraw (request 6):
`MenuChrRecordDisp(tab, id)`, and record 1 on unless the tab is 4. Every
call ends with `MenuLineUpDisp(g_MenuMemberCursor, 1, 0, 0)`.

**The set-up — `MenuChrRecordOpen`.** The page emptied
(`MenuPageDisp(0xFFFF)`, `MenuCommandsSet(0, page)`, `MenuCommandsDisp(0)`,
`MenuIconBytesSet(0, 0)`, `MenuIconBytesDisp(0xFFFF, 1, 2, 0)`), then
`MenuIconBytesDisp(g_MenuMemberCursor, 4, 2, 0)` for the member's face.
Records 5 and 6 go to (`g_MapViewX` + 0x10, `g_MapViewY` + 0x68) and
(`g_MapViewX` + 0x128, the same y) with animations 3 and 2, and are switched
on: the tab row's left and right marks, by their places. The labels are
written into the text at `0x801CBB00`: strings 0x616, 0x617 and 0x618 from
block 0 (`font_string_fixed_put` (`0x8002702C`)); strings 0x619..0x621 ("Attack",
"Defense", "Agility", "Wisdom", "Magic Endr", "#Attack", "Luck", "Range",
"Exp"), four blocks each from block 0x18; and string 0x622 ("Next Level")
from block 0x3B, the fourth block of "Exp"'s, which is not cleared.
`MenuChrRecordClose` switches records 5, 6 and 2 off, puts 5 and 6 back at
the item list's places with animations 4 and 5, clears the icon column
(`MenuIconsClear(0x12, 2, 1, 8)` (`0x8006C4E0`)), redraws the upper grid with no face, draws
the empty page and gives each present member's id back to byte 0 of its
party record.

**The page — `MenuChrRecordDisp(tab, who)`.** It clears record 1's `+4`,
redraws the frame (`MenuIconsClear(0x12, 2, 1, 8)`, `MenuIconBytesSet(0,
0)`, `MenuIconBytesDisp(g_MenuMemberCursor, 4, 1, 0xC)`, `MenuPageDisp`) and
draws the header from the character record: `CellChrGlyphSet(who, 6, 0xE,
0xDEAD2001, 0x3000, 1)`, then three `MenuTallNumberSet` (`0x80069F04`) numbers, the level
(`+0x1C`, two digits), the experience (`+0x38`, seven) and the experience
to the next level (`+0x3C`, six). Each of the six party records whose place
has an id gets byte 0 = the id's low byte, or 0 on tab 4. Then by the tab:

| tab | body |
|---|---|
| 1 | `MenuChrNumbersDisp(who)` (`0x8007020C`) |
| 2 | the six worn items (`+0x1E`), column 5 |
| 3 | the six items (`+0x24`), column 5 |
| 4 | the eight spells (`+0x2A`), column 2; a spell byte that is not 0 is drawn as byte − 0x4A |

Entry i of a list is written into its own block of the text at
`0x801CDB00` (string `id | 0x100`) and its icon put with `IconCellSet(0x12,
col + 3 * i, id, 0)`; after `MenuTextPartsLoad(0x1FE)` each gets
`CellTextDisp(0x15, col + 3 * i + 1, 8, 1, 8, 0xDEAD2001, 0xEE00 + 0x20 *
i)`. The switch has no default, so any other tab reads the count, the
column and the list unset.

**The numbers — `MenuChrNumbersDisp(who)`.** Label strips 0xED40 (row 5),
0xED60 (row 0xA), 0xEDD0 (row 0x16) and 0xEDC0 (row 0x18) at column 0x12;
on each value row glyph 0xC at column 0x1A and a one-block strip at 0x1E
(0xED18 on rows 5 and 7, 0xED1C on the eight below); HP and its maximum
(`+0`, `+2`) on row 5 and MP and its maximum (`+4`, `+6`) on row 7 as three
digits at columns 0x1B and 0x20; then the eight stat rows 0xA..0x18, two
apart, each `MenuNumberCompareDisp(0x1B, row, bare, total)` (`0x8006BD74`) in the order of
the stat table below.

**The stats, kinds 1 to 8.** The kinds a worn item's row names
(`ChrWornItemStatAdd` (`0x80058910`)), the order of the shop's labels (`g_ShopKindTextCodes`
EE40 EE50 EE60 EE70 EE80 EE98 EEA8 EEB4, widths 4 4 4 4 6 4 3 3, strings
0x619..0x620) and the status page's rows agree. The character record's
fields are `ChrRecord`'s (chr.h): each number with what worn items add, and
beside it the number without them.

| kind | stat (string) | with worn items | without |
|---|---|---|---|
| 1 | Attack | `stat_08` (`+0x08`, u16) | `stat_08_bare` (`+0x14`) |
| 2 | Defense | `stat_0a` (`+0x0A`, u16) | `stat_0a_bare` (`+0x15`) |
| 3 | Agility | `stat_0c` (`+0x0C`, u16) | `stat_0c_bare` (`+0x16`) |
| 4 | Wisdom | `stat_0e` (`+0x0E`, u16) | `stat_0e_bare` (`+0x17`) |
| 5 | Magic Endr | `stat_10` (`+0x10`, u16) | `stat_10_bare` (`+0x18`) |
| 6 | #Attack | `attack_count` (`+0x12`) | `attack_count_bare` (`+0x19`) |
| 8 | Range | `walk_reach` (`+0x13`) | `walk_reach_bare` (`+0x1A`) |
| 7 | Luck | `stat_1d` (`+0x1D`) | `stat_1d_bare` (`+0x1B`) |

`MenuNumberCompareDisp` shows the number without worn items beside the one
with them, coloured by which is greater.

### Page 7: System — `MenuPage7Step`

The cursor `g_MenuPage7Cursor` runs over the three command bytes; a cursor
of 3 or more is first moved with `MenuCursorMove(cursor, g_MenuCommands,
0x4000)`. `+4 & 0x5000` moves it; L1, R1, left and right do nothing. The
back-out: page 2, sound request 3, records 3 and 0 off (no `MenuPageDisp(2)`
here, unlike page 4). A press of 0x40: record 0 to animation 0xB, records 3
and 0 off, the page becomes cursor + 0x11 (Save 0x11, Load 0x12, Setup
0x13), `font_target_clear(0x801CBB00, 0x7F)`, stage `g_MenuPageStage = 0`,
sound request 2.

### Pages 9, 0xE, 0xF and 0x10: the command set-up

The page edits each character's three stored commands, the 8-byte record
`g_ChrPlacePresetRecords + id * 8` (`ChrPlacePresetRecord`, chr.h): the
formation cell at `+0` / `+1` and three {kind, number} pairs from `+2`,
which the battle's Tactics rows use (§"Tactics — steps 3 and 4"). The
writer is `MenuPresetSet(who, kind, number)` (`0x80071170`), called by `MenuPresetKindStep`
and the two pickers with unsigned bytes.

**Page 9, the slot step — `MenuPresetStep`.** On its first frame
(`D_800B8EF4[1] == 0`) it calls `MenuPresetOpen`. `+4 & 0xA000` moves the
entry cursor `g_MenuPresetCursor` over `g_MenuIconBytes + 6`; `+4 & 0x5000`
moves the member cursor `g_MenuPresetMemberCursor`, again while the id is
0xC (`MenuMemberCursorMove` never stops on an empty place). The back-out:
`D_800B8EF4[1] = 0`, `MenuPresetClose`, page 2, `MenuPageDisp(2)`, sound
request 3. A press of 0x40: the kind cursor `g_MenuPresetChoiceCursor`
takes the entry's stored kind, record 4 off, record 2 to animation 0xB,
page 0xE, sound request 2, and marks 5 and 6 placed above and below the
member's row. The confirm arm first loops while the id is 0 or 0xC without
reading the id again, so it would never end on such a place; it is never
met, because every move asks for a redraw and the redraw's own loop (which
does read the id again) moves the cursor off such a place first. The
redraw (request 9) calls `MenuPresetDisp(who)` (`0x800709D4`). Every call ends with
`MenuLineUpDisp(g_MenuPresetMemberCursor, 1, 0, 0)`.

**The set-up — `MenuPresetOpen`.** Sprite entries 0x87..0x8E off; records
5 and 6 at x = `g_MapViewX` + 0x120; render entry 2 at x = `g_MapViewX` +
0x80; shape 0 made a 0x80 × 0x10 bar (right x 0x7F, lower y 0xF). Then the
party is put in a column: for display order i = 0..5 the place is
`g_MenuMemberPlaces[(i + 1) % 6]` (places 0, 2, 3, 4, 5, 1); a character
(id neither 0 nor 0xC) goes to x = `g_MapViewX` + 0x48, y = `g_MapViewY` +
0x26 + n × 0x18 (n counting the characters); id 0xC keeps where its record
stands and has the record's `+4` cleared, so it is not drawn. Last
`MenuLineUpMove(xs, ys)`; the arrays' entries for empty places are not
written and not read.

**The page — `MenuPresetDisp(who)`**, in the mode `g_MenuPage`: 9 the
command kinds; 0xE the six kinds of `g_MenuPresetKindIcons` (F0 ED E9 EA EC
EB); 0xF four of the eight spell bytes at `+0x2A` + `g_MenuPresetHalf` ×
4 of the member's record, each that is not 0 + 0xB6; 0x10 a row of six
copied into `g_MenuIconBytes`. It draws a row for each place
`g_MenuMemberPlaces[(i + 1) % 6]` (ids 0 and 0xC skipped) through
`MenuPresetMemberDisp` (`0x80070F1C`), the member cursor giving the row that is lit.
The lit entry is dimmed (colour 4) when it cannot be used: in mode 9 for
kind 3, and in mode 0xF, by the cost against the member's MP as on page 3;
the other kinds and modes ask `ChrItemHeldCheck` (`0x80057E48`), `ChrSpellAnyCheck` (`0x80058654`),
`ChrItemAnyCheck` (`0x80057CB8`), `ChrSpellUseGet` or `ChrItemUseGet`. It then places the
cursor records, and in mode 9 copies render entry 2 to sprite entry
`g_SpriteList + 0x12FC`. One frame passes inside it.

**A member's row — `MenuPresetMemberDisp(n, who, sel)`.** It reads the
member's `g_ChrPlacePresetRecords` bytes `+2 + entry * 2` (kind) and `+3 +
entry * 2` (number), with entry = `g_MenuPresetCursor`, and returns the
row's icon. Kind 3: the icon is the number, dimmed when the member's MP is
below the spell's cost (byte `+4` of row icon − 0xB7 of `g_ChrSpellRows`,
halved rounding up for item 0x88 in worn place 5 or 6). Kind 4: the icon is
the number, dimmed when `ChrItemHeldCheck(who, id, 0)` answers 0. Any other
kind: `g_MenuPresetKindIcons[kind]`, never dimmed. On page 9 the name
(`id | 0x100`) is written, dimmed or not, and the icon put at column 0xC,
row n × 3 + 2; for the open member on the other pages six icons go across
the row instead (from `g_MenuPresetKindIcons` on page 0xE, from
`g_MenuIconBytes` otherwise).

**Page 0xE, the kind row — `MenuPresetKindStep`.** The cursor
`g_MenuPresetChoiceCursor` is the kind, an index into the six icons.
`+4 & 0xA000` moves it (`MenuCursorMove` over `g_MenuPresetKindIcons`). The
back-out: page 9, record 4 off, record 2 to animation 0xA, sound request 3.
A press of 0x40: sound request 2, then by the kind: 3 goes to page 0xF only
when `ChrSpellAnyCheck(who)` answers not 0 (any of the eight spell bytes is
not 0), 4 to page 0x10 only when `ChrItemAnyCheck(who)` answers not 0;
otherwise sound request 4. Any other kind (0, 1, 2, 5) is written at once,
`MenuPresetSet(who, kind, 0)`, and the page goes back to 9. The redraw
(request 0xE) calls `MenuPresetDisp(who)`.

**Page 0xF, the spell picker — `MenuPresetSpellStep`.** The list is two
halves of four spell bytes at `+0x2A`; the half is `g_MenuPresetHalf`, the
cursor `g_MenuPresetChoiceCursor` over the half's copy in `g_MenuIconBytes`.
`+4 & 0xA000` moves the cursor; `0x1000` / `0x4000` pick half 0 / 1, redrawn
only on a change. The back-out: page 0xE with the cursor on 3, sound
request 3. A press of 0x40: `ChrSpellUseGet(who, (u8)(byte + 0x4A), 4)`; at
0 or more, `MenuPresetSet(who, 3, byte)` and back to page 9 with the cursor
3, sound request 2; otherwise sound request 4. The redraw (request 0xF): an
empty half 0 turns to half 1 only when half 1 has a spell; an empty half 1
always turns back. Record 6 is on, on half 0 when half 1 has a spell, and
record 5 on half 1 when half 0 has one (the more-below and more-above
marks). The four bytes are copied to `g_MenuIconBytes`, a cursor on an empty
entry is moved, and `MenuPresetDisp(who)` draws the page.

**Page 0x10, the item picker — `MenuPresetItemStep`**, the same handler
over other bytes: half 0 is the six worn items at `+0x1E`, half 1 the six
items at `+0x24`; the cursor goes back to 4; the check is
`ChrItemUseGet(who, byte, 4)` with no bias, and it writes kind 4; and its
turn to half 1 looks only at half 1's first byte (`+0x24`), where the spell
picker looks at all four.

Every call of the four ends with `MenuLineUpDisp(g_MenuPresetMemberCursor,
1, 0, 0)`. None waits a frame itself; `MenuPresetDisp` runs one.

### Page 0xA: Formation — `MenuBtlPlacesStep`

`g_MenuBtlPlacesState[0]` is the mode (0 opening, `MenuBtlPlacesOpen`; 1
just opened, made 2 by a direction or CROSS; 2 moving a member; 3 the
confirm prompt) and `g_MenuBtlPlacesState[1]` the cursor (the member slot
in mode 2, the prompt row 0..2 in mode 3). In mode 2 left and right step
member `k = g_MenuMemberPlaces[slot]`'s x cell `g_MenuBtlPlacesXCells[k]`
(0x1A..0x22) and up and down its y cell `g_MenuBtlPlacesYCells[k]`
(0xE..0x16), and its party record is placed again (`+6` = `g_MapViewX` +
(x − 3) × 8, `+0xA` = `g_MapViewY` + (y − 6) × 8, `+8` = that + 0xE0).
CROSS walks the slots, then the prompt: row 0 writes every member's cells
into `+0` / `+1` of its `g_ChrPlacePresetRecords` record (the formation
cells), row 1 goes back to moving, row 2 closes (`MenuBtlPlacesClose`). The
back-out sets `g_MenuBackOutPattern = 0x30B30B30`. Sound requests: 1 a
move, 2 a confirm, 3 a cancel.

What it allows: every one of the 81 cells x 0x1A..0x22 by y 0xE..0x16, one
press a cell, and no test of a clash: nothing compares one member's cell
with another's, so two members can stand on one cell and row 0 writes both.
The cursor walks `g_MenuMemberPlaces` from entry 1 (place 0) up, so place
1 is never the one moved; the write skips ids 0 and 0xC.

**The set-up — `MenuBtlPlacesOpen`.** Sprite entries 0x87..0x8E off;
render entry 2 at x = `g_MapViewX` + 0xA8 and shape 0 made a 0x40 × 0x10
bar; `MenuPageDisp(g_MenuPage)` and one frame. Then the members: display
order i takes place `k = g_MenuMemberPlaces[(i + 1) % 6]`; a character
(neither 0 nor 0xC) gets its formation cell copied from its
`g_ChrPlacePresetRecords` record into `g_MenuBtlPlacesXCells[k]` /
`g_MenuBtlPlacesYCells[k]` and a screen place from it (x = `g_MapViewX` +
(x − 3) × 8, y = `g_MapViewY` + (y − 6) × 8); id 0xC keeps where its record
stands and has `+4` cleared. Both lists go to `MenuLineUpMove`. Each
character's record then gets `MapAnimationPoseSet(record, 2, 7)`, and its
0x24-byte sprite image is copied to entry k of the list at `g_SpriteList +
0xD8`, 0x70 to the left, with `+0x18` from its animation's step. Entries 6
and 7 are copies of render entries 1 and 0. It ends with the page's text:
`font_target_clear(0x801CD500, 0x14)`, string 0x660,
`font_target_set(0x801CDB00)`, `font_target_clear(0x801CDB00, 0xC)`,
strings 0x647 and 0x1F1, and `MenuTextPartsLoad(3)`. It reads the cells and
writes none.

### Pages 0x11, 0x12 and 0x14: Save and Load

`MenuSaveStep` (page 0x11) and `MenuLoadStep` (0x12, 0x14) run in stages
(`g_MenuPageStage`); `MenuSaveOpen` and `MenuLoadOpen` (`0x800749F4`) open them, and both
read the card with `MenuCardRead`.

**The card read — `MenuCardRead`.** From its start, to which it comes back:
the 0x20-byte table `g_MenuCardSlotFlags` (`MenuCardSlotFlags`, menu.h)
cleared; `MenuCardListOpen` (`0x80073D58`); `ankprint_rows_clear(0x14, 6, 0x20)` and
`MenuNoticeDisp(0x675, 0xC, 0x16, 1, 0)`; two frames; the result
`g_MenuCardResult` and `g_MenuCardReadFailed` zeroed; and
`g_MenuCardFreeBlockCount = card_dir_read()`, the card's free blocks in port
`g_CardPort`, or −1.

* 0 or more: `MenuCardListRead(g_MenuCardBuffer)` (`0x80075198`); when it found no save
  and no block is free the result is 1; `g_MenuCardReadFailed == 1` starts
  over. Then the cursor (row `g_MenuCardListRow`, scroll
  `g_MenuCardListScroll`): on another port than `g_MenuCardKeptPort` both
  are zeroed; on the kept port the cursor stays if
  `g_MenuCardListSlots[row + scroll]` is still `g_MenuCardKeptSlot`, and
  otherwise both are zeroed and walked forward (the row up to 2, then the
  scroll) to the entry that holds it, giving up past `g_MenuCardListLast`,
  or at it on a page other than 0x11. `MenuSaveWrite` keeps the port and
  slot of the save it writes.
* −1: on the kept port `g_MenuCardKeptSlot = 0`; then `card_result_get()` (`0x80024F38`)
  (the last card call's answer, `g_CardResult`) decides:

  | answer | what |
  |---|---|
  | 1 | result 2, the port flipped (`g_CardPort ^= 0x10`) |
  | 2 | result 3, the port flipped |
  | 4 | `g_MenuNoticeCol8 = 1` and notice 0x67E at row 0x16 (0x14 on page 0x11). Off page 0x11, result 4. On it, `MenuAnswerRun(0x670, 0, 0x19, 0x18, 1)`: 0 puts up notice 0x679, lets two frames go, calls `card_format()` (`0x80024F44`) (`MemCardFormat` of the port), brings the pad record up to date and starts over; 2 starts over; 1 is result 4; any other returns with result 0 |
  | other | result 0 |

`MenuCardResultDisp` puts a notice up for results 1 to 4 on the save pages
(strings 0x673, 0x66D, 0x671 and 0x67E).

**The list's screen — `MenuCardListOpen`.** Records 5 and 6 (the scroll
marks) off at x = `g_MapViewX` + 0x108, y = `g_MapViewY` + 0x28 and + 0x90;
`MenuPageDisp(g_MenuPage)`; sprite entries 0x87..0x8E off; render entry 2
off at x = `g_MapViewX` + 0x80, shape 2's lower y values 0xF, entry 4 at y
= `g_MapViewY` + 0x18; `MenuCommandsSet(0, page)` and `MenuCommandsDisp(0)`;
byte 0 of each party record with an id cleared; the page's command string
(`g_MenuCommands[0] | 0x100`) written at `0x801CCC00` and shown as a strip
of three blocks at (4, 5) (code 0xED88).

**The list — `MenuCardListRead(buffer)`.** It returns the number of saves
found.

1. `g_MenuCardSlotHeads` (sixteen 16-byte heads, `GameSaveHead`) and
   `g_MenuCardSlotFlags` are cleared.
2. For each of the 15 slots, `card_save_records_head_read(i, buffer)` (`0x8007473C`).
   Answer 0: the buffer's first 16 bytes become the slot's head, its flag
   `flags[i]` = 1, and the slot is added to `g_MenuCardListSlots`. Answer 5:
   the slot is passed over (no file). Any other: `g_MenuCardReadFailed = 1`
   and the scan ends.
3. `g_MenuCardListLast` = the count − 1 (−1 on a card with none); a cursor
   past it is zeroed.
4. A second pass makes the half that can be picked, `flags[15 + i]`: 1 for
   a slot holding a save, and 1 for an empty slot while fewer than
   `g_MenuCardFreeBlockCount` empty ones have been granted.
   `MenuCardSlotFlags.pickable_count` (byte `+0x1F`) counts the 1s. List
   entries past the last are set to 0xFF.

So the free block count bounds how many empty slots are offered: 15 less
the card's 8 KB blocks in use. `MenuCardListDisp` reads a head's `+0xA`,
`+0xE` and `+0xF`.

**A save file's head — `card_save_records_head_read(i, buffer)`.** The
template `g_CardSaveFileName` is the string `"BASLUS-00628LUNAR000"`; it
writes the three decimal digits of `i` over the last three characters, so
the saves are the files `BASLUS-00628LUNAR000` to `…014`, and returns
`card_file_read(name, buffer, 0x200, 0x80)` (`0x800250B8`): `MemCardReadFile(g_CardPort,
name, buffer, 0x200, 0x80)` then `card_command_wait(3)` (`0x80024E78`), 0x80 bytes from
offset 0x200 of the file; 0 is read.

**The scroll clamp and marks — `MenuCardListScrollAdjust` (`0x80073BDC`).** It runs when
the list opens (`MenuSaveOpen`, `MenuLoadOpen`) and after every cursor
step and pick. Page 0x11 (three rows): a row of 3 or more becomes row 2 and
the scroll goes up one; the scroll is pulled back one at a time, never
below 0, while `scroll + 2 > last + 1`; the down mark is off when `last <
scroll + 2` or `scroll + 2 == 0xE` (the third row showing the fifteenth
entry), else on. Other pages with no list (free block count −1): row 3,
scroll 0, both marks off. Other pages otherwise: a row of 3 or more becomes
3, the scroll is pulled back the same way, and the down mark is off on row
3, else on; row 3 on these pages is an entry below the three list rows,
which `MenuLoadStep` steps to past the list and `MenuCardListDisp` draws
with a smaller bar. Both list arms end with the up mark on while the scroll
is not 0.

### Pages 0x13 and 0x15: Setup — `MenuOptionStep`

On stage 0 it calls `MenuOptionOpen` (`0x80075988`) (between `font_cell_switch_set(0)` (`0x80016128`)
and `(1)`), which empties the page, copies the seven bytes at
`g_GameOptions` into the working copy `g_MenuOptions`, writes the rows'
names and values and lets a frame go. The cursor `g_MenuOptionRow` is row
0..5 and edits working byte row + 1:

| row | byte (`GameOptions`, game.h) | values |
|---|---|---|
| 0 | `+1` `mono`: `SoundMonoSet` (`0x800550D4`) picks mono or stereo by it | 0, 1 |
| 1 | `+2` `sort_order`: `ChrItemListSort` (`0x80058B3C`) puts the items of kind below 5 in front when it is 0, behind when not | 0, 1 |
| 2 | `+3` `press_wait`: `ScriptLabelRun` copies it to `g_MessagePressWait` | 0, 1 |
| 3 | `+4` `message_speed`: `ScriptLabelRun` copies it to `g_MessageSpeed` | 0, 1, 2 |
| 4 | `+5` `actuators`: the controller's motors may run | 0, 1 |
| 5 | `+6` `action_strings`: the battle shows each action's name | 0, 1 |

Left and right flip the byte, or step row 3's round 0..2; up and down move
the row, wrapping; sound request 1 each. A press of 0x170 leaves and
applies the copy: a changed byte 1 calls `MusicPlay(0, 2, 0)` (`0x80054F2C`), lets a frame
go, stores it, calls `SoundMonoSet` and starts the music `g_MusicNumber`
again; a changed byte 2 is stored and every list sorted again
(`ChrItemListSort(1..12)`); all seven bytes go back to `g_GameOptions`;
`MenuOptionClose` (`0x80075B28`), stage 0, sound request 2. Every frame
`MenuOptionValueBoxesDraw(row)` (`0x800756E4`) runs, and the redraw (0x13 or 0x15) writes
the cursor row's values again (`MenuOptionValuesPut` (`0x800757AC`)).

**The layout — `g_MenuOptionEntries`.** 0x14 entries of ten bytes
(`MenuOptionEntry`, menu.h): +0 column, +1 row, +2 blocks, +3 a width in
pixels, +4 the block in the text from `0x801CD500`, +6 a string id; in
entries 1 to 6, the six rows, +8 the first value entry and +9 the count of
values. `MenuOptionValuesPut(row)` writes a row's values, the chosen one in
colour 0 and the others in colour 4, and returns 0x1F0.

**The boxes — `MenuOptionValueBoxesDraw(row)`.** For each row i, the entry
of its chosen value, `g_MenuOptionEntries[first + value]`, gets a flat
rectangle at (column × 8, row × 8), as many pixels wide as its `+3` and 0x12
high, at ordering-table entry 0x40, in 0x404080 on the cursor's row and
0x408040 on the others (`packet_box_add` (`0x80015A60`), packet_shape.c: an opaque
POLY_F4). It writes nothing else.

### The menu without the walk — `MenuNoWalkRun`

`MenuNoWalkRun` runs `MenuNoWalkOpen`, the page loop `MenuPagesRun` and
`MenuNoWalkClose` (`0x80075E34`). It is reached from `StartMenuPageRun` (`0x80075C28`) (the start
screen's Load and Setup, pages 0x14 and 0x15), from the battle wrapper
`MapBtlRun` and from the field frame `MapFrameUpdate` (`0x80055840`) when
`g_GameEnterRequest` is set; the last two set page 0x14 and music 0x17
first.

**`MenuNoWalkOpen`** is `MenuOpen` without the party's walk; two frames
pass in it. The message's face words and `g_MoveChrViewFollow` cleared;
records 5 and 6 off at x = `g_MapViewX` + 0x108, y = `g_MapViewY` + 0x28 and
+ 0x90; sprite entries 0x87..0x95 off; `MenuRecordsInit`;
`layer_order_set(0x800, 5)` and `(0x100, 4)`; `CellAllClear(0x30)` and the
empty `CellAllUpdate(0x20)`; `font_target_clear(0x801CBB00, 0xC0)`, a frame,
the target set again and the thirteen strings 0x616..0x622 written;
`*(u16 *)g_PartyIds = 0xC`, `g_MenuMemberCursor = 1` and a redraw; a second
frame; `pad_record_init(g_MenuPadRecord, 0x14, 0xF0D0, 0)` (`0x8002687C`); `D_800B8EEC |=
1`, bits 0x30 of `MenuScreenBits.unk_0e` set to 0x10, `layer_mix_set(0x10,
0x10)`, and the sound request 0.

**`StartMenuPageRun(page)`** clears the back-out latch, calls the empty
`func_80056AE4(0x30, 1)`, sets the layers' order, stores the page, zeroes
the seven view words `StartScreenInit` (`0x800436F0`) zeroes, sets
`g_MenuOnePageRunning`, calls `FaceListEntriesSet` (`0x800760D0`) and `MenuNoWalkRun`,
puts it all back, and returns the request `MenuNoWalkRun` returns.

### The use list — `menu_list.c`

**The pick list's layout — `MenuUseListOpen(list, xoff, width)` (`0x80077D98`).** Two
callers, each with its own 12-byte list: `MenuUsePlaceNameRun` (0x108,
0x9F) and `MenuUseListRun` (`0x80077A6C`) (0x130, 0xAF). It clears `g_MenuUsedMark`;
switches the two scroll marks off at x = `g_MapViewX` + `xoff`, y =
`g_MapViewY` + 0xA0 / + 0xC8; makes shape 0's right edge `width` and its
bottom 0xF; puts render entry 2 at x = `g_MapViewX` + 0x80; stores the low
byte of `g_MenuUseNumber` in `g_MenuIconBytes[0]`; shows the grid with
`MenuIconBytesDisp(MenuMemberCursorFind(g_MenuUseUserId), 1, 2, 0)`
(`MenuMemberCursorFind` (`0x8006D298`) turns an id into its member cursor); and zeroes the
caller's list.

**Showing the picked entry — `MenuUseDrawAreaOpen` (`0x800779A0`).** Callers:
`MenuUsePictureRun` (`0x8007690C`) and `MenuUseMoviePick` (`0x800775D4`), each right after
`MenuUseMoviePlay`. `MenuCursorTextDisp(g_MenuUseNumber, 0)`; records 0, 2
and 3 on; `MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0)`; the map's colour
triple `g_MapCol` kept in a local and all three set to −100;
`MapDrawAreaOpen(g_MapLayersMask | 0x130, 0x19, 0, 0, 0)` (`0x800570D8`) (five arguments),
which applies the triple itself through `layer_col_set` (`0x8001EB74`);
`layer_col_set(-1, 0x100, 0)`; the triple put back.

**The use — `MenuUseRun(user, number, target)`.** The Magic and Use pages
call it with the icon byte: below 0xB7 an item, 0xB7 and up a spell
(number − 0xB6). It keeps the three in `g_MenuUseUserId`, `g_MenuUseNumber`
and `g_MenuUseTargetId`, takes the low four bits of the item row's `+7`
(or of the spell row's `+3`) as an entry of the table of function
addresses at `0x800B5B1C`, and calls that entry. The C reaches the table as
`battle_state_table`, a label of the first split that nobody read; its
first eight words are the menu's uses: 0 `MenuUseNumberHang` (it shows
the number in a message and stops, as `MenuPageNumberHang` does), 1
`MenuUseTargetRun` (`0x800767D8`), 2 `MenuUsePartyRun` (`0x80076844`), 3 `MenuUsePlaceNameRun`, 4
`MenuUsePictureRun`, 5 `MenuUseMapRow2Run` (`0x800770E4`), 6 `MenuUseMusicRun` (`0x80077190`), 7
`MenuUseMovieRun` (`0x8007757C`). The words after them belong to other code; nothing in
`MenuUseRun` keeps a value of 8 to 15 from calling one. When the use set
`g_MenuUsedMark`, the price is paid: an item is taken away
(`ChrItemRemove(user, item)`), or the spell's cost comes off the user's
MP, halved rounding up when worn place 5 or 6 holds item 0x88. A use that
was left (the list pickers clear the mark) costs nothing.

## The inventory

The menus and the shop change a character's items through the functions of
`chr_item.c` and `chr_worn.c`. A character's record is the 0x40-byte
`ChrRecord` (chr.h) at `g_ChrRecords + id * 0x40`: six worn items at
`+0x1E..+0x23` (places 1 to 6) and six items at `+0x24..+0x29`. Id 0xC, the
party's own entry, has no items of its own record: its list is the 0x60
bytes at `g_ChrItemList0C`. Eight more bytes at `g_ChrItemWaitingList` hold
items waiting for room. An item is a row of the item table, `BtlItemRow`
(btl.h): row number − 1 of the 0xB6 rows of 0x12 bytes that `g_ChrItemRows`
points at (`+0` the price, `+2` the sale price, `+4` the kind, `+5` a story
bit, `+6` two target modes, `+0xA` / `+0xB` two bonuses, `+0xC` the two
stats they go to, `+0x10` who may wear it). A spell is a 12-byte
`BtlSpellRow` (`+1` two target modes, `+4` the cost), row number − 1 of
`g_ChrSpellRows`.

Ids 8, 9 and 10 have no items: `ChrItemGive`, `ChrItemRemove`,
`ChrItemRoomCheck` and `ChrItemAnyCheck` pass over them. Each of these
walks the party (`g_PartyIds`, `g_PartyCount` entries) in order; with `who`
0 any member will do, with any other `who` only that one.

**Adding an item — `ChrItemGive(who, item)`.** For the first member with
room (`ChrItemRoomCheck`): id 0xC takes the item in the first empty byte of
its list; anyone else in the first empty one of its six items. One item a
byte, no stacking. An item whose `+0x10` word has bit 15 set
(`ChrItemUseGet(0x10, item, 1)` answers 0), given to a member other than
ids 1 and 0xB, goes into the shared list instead: from byte 0x5F down, the
first byte that is empty or holds an item without bit 15 takes it, and the
member gets what that byte held (nothing, when it was empty); with no such
byte the member keeps the item. After an add: `ChrItemListSort` for the
list (and for 0xC after an exchange), `StoryBitClear(0x2D6)`, answer 1.
When nobody has room: with story bit 0x2D6 set the item is kept in
`g_ChrItemNoRoomItem`; then, with `g_ChrItemNoRoomState` 0, the field
message `MessageStringDisp(g_ChrItemNoRoomString, 1)` (string 0x66A) and the
menu's page 0x16 (`MenuRun(0x16)`), and the answer is 1 when story bit
0x2D6 is clear afterwards, 0 when not; with the state not 0,
`ChrItemWaitingAdd(item)` (`0x80057A18`) puts the item in the first empty one of the eight
waiting bytes (nothing when all eight are full) and answers 1.

**Taking one away — `ChrItemRemove(who, item)`.** It takes one copy away
and answers 1, or 0 when nobody holds it. With `who` 0 the waiting bytes
come first, byte 7 down to 0. Then each member in order: id 0xC's list from
byte 0x5F down; anyone else's six items from the sixth down, and then its
six worn items from place 6 down, where a find is handed to
`ChrWornItemRemove(id, place)`. Every list is walked from its last byte, so
the copy taken is the last one of the first list that holds any. An item
byte that is emptied is closed up by `ChrItemListGapClose(list, place)` (`0x800589E0`)
(0xD the waiting bytes, 0xC the shared list, any other id its items): each
byte after it moves down one place, the walk ending after it has moved an
empty one, and the last byte moved is cleared. The order of what is left
is kept; nothing is sorted. Worn places are not closed up. Its callers:
the script command `ScriptCommandChrItemRemove` (`0x8005D164`) (`who` 0), the shop's sell
(`ShopDealRun` (`0x80053194`)), the Give and Drop pages, `MenuWornItemChange`, and the
menu's use `MenuUseRun`, so an item used from the field menu leaves through
here.

**The checks.**

* `ChrItemRoomCheck(who)` answers 1 when a member it walks has an empty
  item byte (for 0xC, an empty byte of the list), else 0.
* `ChrItemAnyCheck(who)` answers 1 when a member it walks has any of its
  twelve bytes `+0x1E..+0x29` taken (for 0xC, any byte of the list);
  otherwise it answers `ChrItemWaitingCheck()`: 1 when any of the eight
  waiting bytes is taken. So a member with nothing at all still answers 1
  while an item waits.
* `ChrItemHeldCheck(who, item, worn_only)` answers 1 when the item is held.
  With `worn_only` 0 and `who` 0 or 0xC, the shared list first; with `who`
  0xC the answer is then 0. Otherwise, for each member but 0xC (all of them
  for `who` 0), its six items when `worn_only` is 0, and then its six worn
  items, place 6 down to 1. Ids 8, 9 and 10 are not passed over here, and
  item 0 is not refused (it asks whether a byte is empty). Callers: the
  script command `ScriptCommandChrItemHeldJump` (`0x8005D1CC`) (`who` 0, the item and
  `worn_only` from its operands), and `MenuPresetDisp` /
  `MenuPresetMemberDisp`, which dim a preset item when `(who, item, 0)`
  answers 0.
* `ChrSpellAnyCheck(who)` answers 1 when any of the eight spell bytes at
  `+0x2A` of the record is not 0.
* `ChrItemWaitingGive()` gives the last waiting item (byte 7 down) with
  `ChrItemGive(0, item)` and empties its byte; it answers 1 when that was
  byte 0, the last one waiting, and 0 otherwise or when none waited.

**What an item may do — `ChrItemUseGet(who, item, mode)`.** It reads the
item's row and nothing else. An item of 0 or above 0xB6 answers −2,
whatever the mode. Otherwise by the mode:

| mode | answer |
|---|---|
| 1 | 0 when bit `who` − 1 of `+0x10` is set (the member may wear it), else −1 |
| 2 | 0 when the sale price `+2` is not 0, else −1 |
| 3 | the high four bits of `+6` (the field menu's target mode), −1 when they are 9 |
| 4 | the low four bits of `+6` (the battle's target mode), −1 when they are 9 |
| any other | −1 |

The callers that pass `who` 0x10 read bit 15 of the mask (`ChrItemGive`,
`MenuPoseWordsSet`). With `who` 0 in mode 1 the shift is by −1; no caller
passes that. Mode 3 is asked by the Use page (`MenuItemUseStep`: below 0
refused, below 5 no target, 5 and up a member to pick), mode 2 by the Drop
pages, mode 4 by the command set-up's item picker and `MenuPresetDisp`,
mode 1 by `ChrItemGive`, `MenuPoseWordsSet`, `MenuWornItemColGet` and the
script command `ScriptCommandChrLevelItemSet` (`0x8005D2F0`), which does not look at the
answer.

**What a spell may do — `ChrSpellUseGet(who, spell, mode)`.** A spell of 0
or above 0x32 answers −2. Mode 4: the low four bits of the row's `+1`. Mode
2: the MP test first, the cost `+4` halved rounding up when worn place 5 or
6 (`+0x22` / `+0x23`) holds item 0x88, and −1 when the member's MP (`+4` of
its record) is below it; then as mode 3. Mode 3: the high four bits of
`+1`. Any other mode −1. A value of 9 answers −1. So the high four bits are
the field menu's target mode and the low four the battle's, as for an item,
and 9 means the spell cannot be used there. The Magic page asks mode 2;
the command set-up page and its spell picker ask mode 4, with no MP test;
nothing asks mode 3 on its own. The battle's own cost test asks
`BtlTargetTraitCheck(0, 0x1B, 0, 0)` instead of looking for item 0x88
(§"Who can do what"). The callers pass the icon byte + 0x4A as a byte,
which turns icons 0xB7..0xE8 into spells 1..0x32, and 0 for an empty cell:
the Magic page's use tests the answer `>= 0`, so −2 is refused, and its
redraw tests `!= -1`, so −2 is not dimmed.

**Worn items.**

* `ChrWornItemSet(id, place, item)` stores the item at `+0x1D + place` and
  calls `ChrWornItemValuesAdd(id, item)`; it does not look at what the
  place held. `ChrWornItemRemove(id, place)` reads the place, zeroes it,
  calls `ChrWornItemValuesRemove(id, item)` and returns the item (0 for an
  empty place).
* `ChrWornItemValuesAdd` adds the row's `+0xB` to the stat the high four
  bits of `+0xC` name and `+0xA` to the stat the low four bits name
  (`ChrWornItemStatAdd`), and sets story bit `+5` + 0x2BC when `+5` is not
  0. `ChrWornItemValuesRemove` takes both off and clears the bit. Neither
  does anything for item 0.
* `ChrWornItemStatAdd(record, kind, value)` adds a signed value, at the
  number's own width and with no limit, to the number the kind picks:
  1..5 the words at `+8`, `+0xA`, `+0xC`, `+0xE`, `+0x10`; 6 the byte
  `+0x12`; 7 the byte `+0x1D`; 8 the byte `+0x13`; 0 and above 8 nothing.
  These are the stat kinds of §"Page 6: Status".

**The change preview — `ChrWornItemChangeMake(who, out, new, old)`.** Up to
four rows of three 16-bit words {kind, now, after}, in a 0x18-byte buffer
it clears first; nothing for two empty items. Called only by the shop's
`ShopItemStatsDisp` (`0x80053E0C`) and the menu's `MenuWornItemDisp`. A row is filled by
`ChrWornItemChangeRowSet(who, row, add, sub)` (`0x800584C0`): now = the member's number
for the kind, after = now − sub + add. The low four bits of `+0xC` come
first, with the `+0xA` bonuses: the same kind on both items gives one row;
two kinds two rows, the new item's first; a kind on one item only, one row.
When the new item's low kind is 0, row 0 takes the old item's low kind with
−its bonus, and is written even when that kind is 0 too: {0, 0, −bonus}.
The high four bits, with the `+0xB` bonuses, follow the same rules from the
next row: row 2 after two low rows, row 1 otherwise. The two readers differ
on a row of kind 0: the shop's panel passes over it and draws the rows
after it, while the menu's `MenuChangeRowsDisp` stops at it, so on page 5
two items with no low kind show no rows at all. The shop's caller declares
its buffer as nine 16-bit words (three rows), where the preview clears
0x18 bytes.

**The list's order — `ChrItemListSort(id)`.** It sorts the six items of a
character, or id 0xC's list, unless the first two bytes are not both
taken. The part up to the last taken byte is first parted by the kind
(`+4`): kinds below 5 in front and 5 and up behind, or the other way round
when the option byte `sort_order` (`GameOptions +2`) is not 0. Then
`ChrItemListPartSort` (`0x80058E18`) sorts each part of two bytes or more. Callers:
`ChrItemGive`, the battle's `BtlWinItemsGive` (`0x80028A54`), and `MenuOptionStep` for ids
1 to 12 when the option changes.

## The field's status strip — `MapStatusDisp`

The strip of the party's levels, HP and MP at the bottom of the field
screen. `MapFrameUpdate`, the field frame, calls `MapStatusDisp` (`0x80056408`) every
frame, and `MapStatusClear` (`0x8005A7C8`) calls it to take the strip down. It acts on the
request `g_MapStatusRequest`, which the end of every field frame
(`MapFrameCount` (`0x800566AC`)) sets back to 0. Who asks:

* `MapPadStep` (`0x80055EC4`), the field's pad step, asks 1 when no direction has come for
  0x12C calls (`g_MapIdleCount` reaches 0x12C), and 2 when a direction comes
  with the count not 0, unless an HP loss is pending with its wait at 0xF
  or more; after `MapPartyHpTake` (`0x80059E84`) it sets the count to 0x12C and asks 1.
* `MapPartyStep` asks 1 on a new press of exactly L1 or R1 (0x4 or 0x8),
  after printing a debug line of the developers', and sets the count to
  0x12C.
* `MapStatusClear`, when the count is 0x12C or more or a request is
  pending, sets the count to 0, asks 2, calls `MapStatusDisp` and lets a
  frame go (`ScriptFrameStep(1)` (`0x8005A1F0`)). Its callers include the shop's script
  command `ScriptCommandShopRun` (`0x8005E384`) and the notice `MessageNoticeNumberDisp` (`0x8005BA70`).

Any request clears rows 0x16..0x19 (`CellRowsClear(0x16, 4, 0x20)`).
Request 1 then draws, for the party's places 5 down to 0 whose id is 1 to
0xB, at column x (0x20 for the first found, 6 less for each after it): the
name on row 0x16 (`CellChrGlyphSet`); glyph numbers 0xA and 0xB and the
level (`+0x1C`) in two digits on row 0x17; HP (`+0`) in three digits on row
0x18 and MP (`+4`) on row 0x19, each in the colour `MapStatusColGet(now,
maximum)` (`0x80056674`) gives against `+2` / `+6`: 0x2000 at the maximum, 0x3000 above
half, 0x1000 above a quarter, else 0. Left of the last member go the labels,
glyph numbers 0x5C and 0x5E at columns x + 2 and x + 3 of row 0x18, and 0x5D
and 0x5E on row 0x19. `CellGlyphSet` (`0x80057500`) writes number + 1 (number | 0x80 for a
tall glyph) as the atlas code, so these are codes 0xB, 0xC, 0x5D, 0x5F and
0x5E (§"The atlas"). Any other request calls the empty `func_800572D8` and
draws nothing.

## The shop

The whole shop, from the script command that opens it to the frame it
closes in. Its code is `shop_run.c`, `shop_screen.c`, `shop_strings.c` and
`shop_face.c`; its records are in shop.h.

### How it opens, and the stock

Script command 0x2E (`ScriptCommandShopRun`, entry 46 of the script's
command table at `0x800B333C`) calls `MapStatusClear`, moves the command
pointer on by 4, calls `ShopRun` (`0x80050694`) with the byte after the command word, and
then `MessageRecordsInit` (`0x8005A580`) (the message's cursor and arrow records made
again). `ShopRun(shop)` does nothing for a low byte of 0 or 8; otherwise it
sets the byte `g_ShopStockRow` to shop − 1 and calls `ShopOpen` (`0x800506EC`), the step
loop `ShopStepsRun` (`0x800516CC`) and `ShopClose` (`0x800512C4`).

**The stock** is `g_ShopStockRows` (`0x800A8468`, never written): 0x28 rows
of six item numbers (`ShopStockRow`, shop.h; 0 an empty place).
`ShopStockGet(row)` (`0x8001BF1C`) (outline.c) answers row n for a low byte n below 0x28
and the last row, 0x27, for 0x28 and up. Row 7, shop 8's, is six zeros in
the image, so the rule that shop 8 opens nothing loses nothing.
`ShopListSet(0)` (`0x80051768`) copies the six into the list; then, for each of the party's
numbers, when the six item bytes at `+0x24` of that number's record hold
item 0xB5 (the Rememberizer), every 0xB5 of the six is cleared. For id 0xC
that test reads the bytes at `g_ChrRecords + 0x324`, not id 0xC's list.
`ShopDealStep` (`0x80053534`) fills the list again as soon as a deal leaves 0xB5 under the
cursor.

**Shop 0x22** (`g_ShopStockRow + 1 == 0x22`, row 0x21) takes no money:
`ShopItemDisp` (`0x80052C0C`) never sets the "more than the money" flag, `ShopDealRun`
takes nothing off, and `ShopItemTextDisp` (`0x80053B18`) shows the price as 0.

**Prices** are the item table's (`BtlItemRow`): `+0` the price, `+2` the
sale price. A sale adds the sale price and stops at 0x98967F (9,999,999).

### The screen

Everything is placed from `g_ShopOrigin`, the view (`g_MapViewX`,
`g_MapViewY`) when the shop opens; the cell grid is fixed on the screen.
After its walk `ShopOpen` writes the shop's labels into the text: the 0x16
strings `0x600 + g_ShopStringIds[j]` into blocks `0x14 +
g_ShopStringBlocks[j]`, so label j is shown by cell code `0xED00 + 4 *
(0x14 + g_ShopStringBlocks[j])`. Before that it calls the empty
`ShopTextStringReport` (`0x800539B0`) with (code, string, 0, count) for most of the same
labels: five before the walk, one in each of frames 0 to 7 and 9 to 14.
The calls write nothing.

| code | string | text | where |
|---|---|---|---|
| 0xED50 / 0xED5C | 0x603 / 0x604 | Sell? / Buy? | (0xC, 0x14), the prompt |
| 0xED68 / 0xEE38 / 0xEDEC / 0xED74 | 0x605 / 0x614 / 0x60B / 0x606 | Equip / Buy / Sell / Cancel | row 0x16, columns 0xC, 0x13, 0x1A |
| 0xED80 | 0x607 | Money | (3, 0xD); the money in tall digits at (3..9, 0xF), glyph 0x8F at (10, 0xF) |
| 0xEE2C | 0x60E | Price | (3, 9); the price at (3..9, 0xB) |
| 0xED8C / 0xEDAC / 0xEDC8 / 0xEDF4 / 0xEE10 | 0x608..0x60D | Can't equip this! / Can't sell this! / Not enough money! / Don't have any! / Inventory full! | (0xC, 0x14), a second message at (0xC, 0x16) |
| 0xEE40..0xEEB4 | 0x619..0x620 | Attack Defense Agility Wisdom Magic Endr #Attack Luck Range | the preview's rows 0x14 / 0x16 / 0x18 (`g_ShopKindTextCodes` / `g_ShopKindTextWidths`) |
| 0xEEC0 | 0x661 | glyph 0x99F8 (an arrow) | column 0x1E of a preview row |

The item's two strings (number + 0x100 and number + 0x200; by the string
table its name and its description) are written into blocks 0..7 and
8..0x13 and shown at (0xC, 6) and (0xC, 8) by `ShopItemStringsDisp` (`0x80054A08`), in
colour 4 (dimmed) when the item is more than the money or cannot be sold.
The list is twelve icon cells at ((i % 6) * 4 + 0xC, (i / 6) * 4 + 0xB)
(`ShopListEntry`, shop.h). The member's face, (id − 1) * 5 + 1, is set by
`ShopFaceSet` (`0x80054C0C`) in face place 1, cell (3, 0x12).

**Seven objects**, `g_ShopObjects`: 0x3C-byte records drawn from resource
2's bank `g_SpriteData0`, made by `ShopObjectInit` and stepped by
`ShopAnimationStep` (`0x8005381C`) into sprite entries 0x72..0x78.

| object | animation | what | where |
|---|---|---|---|
| 0, 1 | 0, 1 (word 0x7F0) | the Buy and Sell plates | (+0x20, +0x10), (+0x20, +0x28) |
| 2 | 12 (0xD held) | the pointer | |
| 3 | 8 (9 held) | the member cursor | |
| 4 | 10 (11 held) | the list cursor | |
| 5, 6 | 4, 5 | the up and down marks of id 0xC's list | (+0x120, +0x58), (+0x120, +0x88) |

Objects 2..6 have the word 0x760. `ShopObjectInit` stores `(word & 0x3FF)
| 0x6000` at `+0xE`. An object it sets up never holds its last step (its
end word lacks bit 15); `ShopAnimationSet` (`0x80053648`) keeps the animation's raw count
with its bit 15, so an animation whose count has that bit holds its last
step. When both marks are on, `ShopListSet` gives each the other's timer,
step length and place in the animation, so the two run in step.

**The outlines.** `OutlineMapStep(g_ShopObjects, i, 1)` draws the moving
outline of `OutlineDraw` (`0x8001B954`) round objects 2..4 when they are on (outline
slots 9..11, kinds 0, 2 and 1 of `g_OutlineMapKinds`: 50 × 26, 18 × 26 and
26 × 26). Its counter does not step while the object's animation number is
odd: the held animations.

**The shapes**, `g_ShopShapeEntries` and `g_ShopShapes`, drawn by
`ShapeDraw`: 0 the prompt's bar, a flat rectangle 0x30 × 0x10 (the width
from `ShopDealDisp` (`0x800530E8`)) in the colour word `g_ShapeBoxCol` (0x3050E0), at x
(choice × 7 + 0xC) × 8, y +0xB0; 1 and 2 outlines of four lines in
`g_ShapeOutlineCol` (0xF0F0F0) round list row 0 and row 1; 3 one outline
round both, for id 0xC's list. Each shape entry is also copied whole into
a sprite entry (0x6E + n) whose pose table (`+0x10`) is 0.

**The line-up.** The party's field records walk into a row: id 12 to place
0, ids 1..7 and 11 to places 1 and on in party order, at (+0x70 + 0x20 ×
place, +0x28), with 0x1000 at `+8`, bits 0x7000 at `+0xE`,
`MapAnimationPoseSet(record, 1, 5)`, over 16 frames. `ShopLineUpMove` (`0x800541AC`) then
moves each to row `row` of its wanted entry (y = +(row + 4) × 8) in four
frames and poses it by the entry's pose and facing (`ShopLineUpEntry`,
shop.h): the picked member one row lower; on the buy side each member's
pose 1 when it may wear the item (2 when not) and facing 5 when it has a
free item byte (1 when not). Ids 8, 9, 10 and 0x1F and up go to the last
place, held (`+1 = 1`) and with bit 0x8000 at `+0xE`. The party's ids at
`g_PartyIds` are put in line-up order for the whole shop and put back at the
end, so every inventory function the shop calls sees the line-up's order.
With no id 12 in the party, place 0 keeps the id it had, and its record
walks a path the opening did not make (whatever its heap block holds). The
first byte of each record of `g_MapPartyTwins` is zeroed, and the map's
objects are stepped once with `g_MapObjectsSpriteBits = 0x8000` in the
opening's fourth frame and once with 0 in the closing's eleventh.

### The steps and keys

`ShopStepsRun` sets `g_ShopStep = 1`; each pass updates the shop's pad
record `g_BtlMenuPadRecord` (`pad_record_update`, delay 0x10; the record is
not reset when the shop opens), calls entry `g_ShopStep − 1` of the four
words at `0x800B3258`, plays a sound request `g_BtlMenuSoundRequest` with
`SoundPlay(request, 3)` (1 a move, 2 a confirm, 3 a cancel, 4 a refusal),
and runs one frame (`ShopFrameStep` (`0x800536AC`)), until the step is 0. The C reaches
the table as `entity_ai_table`, a label of the first split that nobody
read; its words are the shop's four steps.

| step | function | pressed or repeating | newly pressed |
|---|---|---|---|
| 1 Buy / Sell | `ShopBuySellStep` (`0x80051D98`) | up / down (0x5000) turn the choice over | exactly 0x10 (TRIANGLE) leaves; exactly 0x40 (CROSS) → 2 |
| 2 member | `ShopMemberStep` (`0x80052014`) | right / R1 next, left / L1 back (0xA00C), wrapping | 0x10 → 1; 0x40 → 3 |
| 3 list | `ShopItemStep` (`0x8005268C`) | L1 / R1 (0xC) the member; left / right wrap in the row; up / down change row (id 0xC's list scrolls; on the buy side up from row 0 lowers the byte at `0x800B87AF`, three after `g_ShopMemberCursor`, which no other C names) | 0x10 → 2; 0x40 asks, or refuses |
| 4 Buy? / Sell? | `ShopDealStep` | right / left over the two choices, or three with Equip, wrapping | 0x10 → 3; 0x40 does it (`ShopDealRun`) |

The cursors are bytes: `g_ShopMemberCursor`, `g_ShopBuySell` (0 buy, 1
sell) and `g_ShopCursors` (the scroll row, the list entry, the deal's
choice). Opening a shop does not reset them.

**The member pick — `ShopMemberDisp` (`0x80051E60`).** Called by `ShopBuySellStep` on a
confirm, by `ShopMemberStep` after every move and by `ShopItemStep` on a
cancel. A member cursor at or past `g_ShopPlaceCount` becomes 1. The wanted
entries: the member under the cursor row 1, pose 1; every other row 0, pose
2; facing 5 for all. Then `ShopLineUpMove`, the member cursor (object 3)
at x = origin + (cursor × 4 + 0xD) × 8, y = origin + 0x14, and the face. On
the sell side the 0x18 × 6 cells at (0xC, 0x14) are cleared, `ShopListSet`
fills the list with what the member has, and with a count of 0 the label
0xEDF4 ("Don't have any!") is shown there. `ShopItemLineUpSet` (`0x800521F8`) (the buy
side, from `ShopItemDisp`, `ShopItemStep` after a refusal and `ShopDealRun`
when a deal fails) sets the wanted entries by the item as above;
`ShopMemberLineUpSet` (`0x800523BC`) (the sell side) is `ShopMemberDisp`'s entries with no
clamp and no list.

**The list's cursor snap — `ShopListCursorAdjust(stepped_right)` (`0x80054410`).** It
calls nothing: it puts the list cursor `g_ShopCursors[1]` (row × 6 +
column) on an entry that is on (its first word not 0). `ShopItemStep`
calls it after every move (1 only after a step right), `ShopItemDisp` and
`ShopMemberStep` with 0. The first found wins: the cursor's own entry;
along its row in the step's way (right to column 5, else left to column
0); the same row from the far end; the other row, from the cursor's column
left; the other row, from its column right; nothing on, entry 0. The C of
the wrap from column 0 is written `while ((lim = 5, col < lim) && cell ==
0) col++;` and is marked in the C as a device that makes the compiler give
the original's bytes, not as recovered source: `lim` is always 5, so the
test is `col < 5`. How the original spelled it is not known.

**The flags of the item under the cursor**, `g_ShopItemFlags`, set by
`ShopItemDisp` on the buy side for the member under the cursor:

| flag | set when |
|---|---|
| 0x20 | the price is more than the money; never in shop 0x22 |
| 0x10 | the item is worn (kind below 5) |
| 0x01 | the member may not wear it (its bit of `+0x10` is clear) |
| 0x02 | the member may and the place is taken (kind 4: both `+0x22` and `+0x23`; other kinds `+0x1E + kind`) |
| 0x40 | no free item byte (`ChrItemRoomCheck(id) == 0`) |

0x01 and 0x02 are written only for a worn item: for any other they keep the
last worn item's values. `ShopItemStep` refuses with "Not enough money!"
on 0x20, and with "Inventory full!" on 0x40, except for an item the member
may wear, which it refuses for room only when the place is taken too
(0x42): an item that would go on an empty place is not refused for want of
room. On the sell side it refuses an empty entry (no message) and an item
of kind 6 or with no sale price ("Can't sell this!").

**The deal — `ShopDealOpen` (`0x80052EAC`) / `ShopDealRun`.** `ShopDealOpen` sets the
deal's cursor to 0 and `g_ShopDealWearable` to 1 on the buy side when the
item is worn and the member may wear it; it writes the choices on row 0x16,
seven columns apart: Equip (with `g_ShopDealWearable`), Buy or Sell, and
Cancel. `ShopDealRun`, on CROSS, takes the choice (plus 1 when there is no
Equip) for the member `id` under the cursor:

* **0, Equip (buy and wear).** Kind 4 has two places, 5 and 6 (`+0x22`,
  `+0x23`). Both full: room is needed (`ChrItemRoomCheck`, else the deal
  fails), place 5's item goes to the items (`ChrItemGive(id,
  ChrWornItemRemove(id, 5))`), `+0x23`'s byte is copied to `+0x22`, and
  the new item goes to place 6. Only `+0x22` full: place 6. Only `+0x23`
  full: its byte is copied to `+0x22` and the new item goes to place 6.
  Neither: place 5. Other kinds: a full place needs room (else the deal
  fails); then `ChrItemGive(id, ChrWornItemRemove(id, kind + 1))` and
  `ChrWornItemSet(id, kind + 1, item)`. Then the price is paid.
* **1 on the sell side.** For a member other than 0xC, entries 0..5 are worn
  items and are taken off (`ChrWornItemRemove(id, entry + 1)`); otherwise
  `ChrItemRemove(id, item)`. The sale price is added, up to 0x98967F, and
  `ShopListSet` fills the list again.
* **1 on the buy side.** Room is needed (else the deal fails);
  `ChrItemGive(id, item)`, then the price is paid.
* **2, Cancel.** Sound request 3, which the end overwrites with 2.
* **The price** comes off `*g_GameMoney` unless the shop is 0x22. Nothing
  here tests that the money suffices: `ShopItemDisp` has set flag 0x20 and
  `ShopItemStep` has refused.
* **The end.** A failed deal shows "Inventory full!" (`ShopRefusalDisp(0xEE10,
  0)` (`0x800524C4`)), runs `ShopItemLineUpSet` and opens the prompt again. Otherwise sound
  request 2, the money shown again (`ShopMoneyDisp` (`0x80053A44`)), step 3, and
  `ShopDealClose` (`0x80053120`) goes back to the list.

The Equip arm for another kind calls `ChrItemGive(id,
ChrWornItemRemove(id, kind + 1))` even when the place is empty, with item 0.
When the member's six item bytes are full and the place is empty, the room
test is passed over (it is made only for a full place) and `ChrItemGive(id,
0)` finds no room: it takes its nobody-has-room path, which, with
`g_ChrItemNoRoomState` 0, shows `g_ChrItemNoRoomString` and runs the field
menu's page 0x16 in the middle of the shop.

**A refusal — `ShopRefusalDisp(first, second)`.** It shows one or two
messages at (0xC, 0x14) and (0xC, 0x16) (the first seven or nine blocks
wide), plays sound 4 (`SoundPlay(4, 3)`), clears the newly pressed word,
and runs up to 0x80 frames, each updating the pad first, until a button is
newly pressed; then `SoundPlay(3, 3)`. An empty entry passes no message
and waits the same way.

**The frame — `ShopFrameStep`.** In order: `ShopAnimationStep(
g_MapPartyRecords, 0, g_ShopPlaceCount)`, the line-up into sprite entries
0 on; `OutlineMapStep(g_ShopObjects, i, 1)` for i 0..6; `ShopAnimationStep(
g_ShopObjects, 0x72, 7)`; sprite entries 0x6E..0x71 off, and each shape
entry that is on copied to entry 0x6E + n and its shape drawn with
`ShapeDraw`; `palette_animation_step` (`0x800263F4`), `tile_animation_step` (`0x80026690`),
`sprite_list_draw`; the text upload `D_800E4768` asked for when
`g_ShopTextBlockCount` is not 0; `MapFrameEndCopy`, which calls the field's
frame end `MapFrameEnd` (`0x8005596C`) (`layer_col_fade_step` (`0x8001E7B0`), `bg_draw` (`0x800145EC`), `palette_update` (`0x80020134`),
`tile_animation_load` (`0x80026814`), `frame_end` (`0x8001F30C`), `frame_start` (`0x8001F294`), `sound_music_level_step` (`0x800245F0`),
`g_PlayTimeFrames` up by one, `MapFrameCount`); and `g_ShopTextBlockCount =
0`. One frame passes, in `frame_end`. Its only route to `rng_draw` (`0x8001C770`) is the
pad's motor driver in that frame (frame-rng.md), and the shop starts no
motor pattern.

**The tick — `ShopAnimationStep(records, entry, n)`**, for n records of
0x3C bytes: entries `entry`.. off (`sprite_run_off`), then for each record
whose first byte is not 0: the timer `+0x34` goes up; at the step's length
`+0x36` the step `+0x38` goes on; with bit 15 of the end word `+0x3A` set, a
step that has reached the end's low 15 bits goes back one (the last step
holds); with it clear, a step at the end starts the animation again
(`MapAnimationStart(record, 1)`); in each case the timer is set to 0 and the
length read again. The record's 0x24-byte image (`+4`) is copied to its
entry with the step's pose at `+0x18`. Its hold test is bit 15 of the end
word, where the other ticks test bit 15 of the animation's count; the end
word gets that bit only from `ShopAnimationSet`. Callers: `ShopFrameStep`
(twice), `ShopOpen` (the last place's held record alone) and
`MenuUseShowPlacesPlay` (`0x8007EFC0`) (`btl_show.c`, six records of `g_MenuUseShowRecords`
into entries 0x6E..0x73).

**Leaving — `ShopClose`.** In order: `CellAllClear(0x30)` and the empty
`func_800572D8`; the message's face words cleared and `g_MessageRow =
0x14`; the text at `0x801CBB00` emptied and sent (0x2A80 bytes through the
upload block `D_800E4768`); `D_800B8966 = 0x10` and `layer_mix_set(0x10,
0x20)`; the first byte of objects 0..5 cleared (object 6, the down mark, is
not), the four shapes off, sprite entries 0x6E..0x78 off; for each place, a
16-point path back to where the opening found that id's record; sound 6
(`SoundPlay(6, 4)`); 16 frames of the walk (frame 10 also steps the map's
objects with `g_MapObjectsSpriteBits = 0`); more frames while
`layer_col_fade_check(0)` (`0x8001EDDC`) runs; `layer_mix_switch_off(0x10)`,
`layer_order_set(0x800, 1)`, `D_800B8EEC |= 1`, the held-button words
`g_PadHeldButtons` and `g_PadSecondHeldButtons` cleared, bits 0x30 of
`MenuScreenBits.unk_0e` cleared; the ids, the 0x3C-byte records and the
twins' first bytes put back for `g_PartyCount` places, and places up to 6
after them cleared; `ShopFree` (`0x800548F4`); sprite entries 0x46..0x4B and 0..5 off;
`MessageDisp({0xFF01, 0xFFFF}, 0)`, a new message page (its rows cleared,
`MessageLineDisp(1)` (`0x8005AD94`) for each of three rows); and `layer_col_set(-1,
g_MapLayersMask & 0x30, 0)`.

**The plate not chosen.** `ShopBuySellDisp` (`0x80051C5C`) puts the pointer (object 2) at
x = origin + 0x20, y = origin + (choice × 3 + 2) × 8, and gives the plate
not chosen byte `+1 = 1` (its animation stops) and bit 15 of `+0xE`, the
sprite's colour bit; the chosen plate has both cleared. The plates' `+0xE`
also has both bits of 0x6000 (from `ShopObjectInit`), and the sprite pass
draws such an entry in 0x808080 whatever the colour bit says
(`sprite_piece_draw`, sprite_list.c), so the bit changes nothing on screen.

**The sell side's count.** `ShopListSet` counts what a character has to
show by testing the six worn bytes twice (its second loop tests the first
six again, not the item bytes): a member who wears nothing counts as having
nothing, and "Don't have any!" is shown, though the list still holds its
items.

**The arcs of a deal, in frames** (from the pass that reads the button to
the next pass, counted from the C):

* the opening: 16 walking frames and `ShopBuySellOpen` (`0x80051CFC`)'s four; the first
  pass is in the 21st frame. Sound 5 (`SoundPlay(5, 4)`).
* Buy / Sell to the member: 4 (`ShopMemberDisp`'s line-up) + 1 = 5. Member
  to list: 4 + 1 on either side (`ShopItemLineUpSet` from `ShopItemDisp` on
  the buy side, `ShopMemberLineUpSet` on the sell side). List to prompt: 1.
* a purchase, with or without Equip: back to the list in 4 + 1.
* a sale: 1 (the sell side has no line-up there).
* a refusal: up to 0x80 frames of waiting, then the buy side's line-up (4)
  and 1: 128 + 4 + 1 with no button pressed.
* leaving: 16 walking frames, the new message page, and any fade still
  running.

**What the shop writes**: the money `*g_GameMoney`; the characters' worn
items, items and stats (`ChrWornItemStatAdd` by the row's `+0xC`, `+0xA`
and `+0xB`); id 0xC's list; story bits (`+5` + 0x2BC set or cleared by
`ChrWornItemValuesAdd` / `ChrWornItemValuesRemove`; 0x2D6 by `ChrItemGive`);
the party's ids and records (in line-up order, then put back); the cell
grid, the text and its blocks, the icon slots, the face places, and its
cursor bytes.

### Where each shop is

Every command 0x2E of the 62 event scripts: 72 commands in 19 scripts, all
scripts walked whole. "script:label" is the label whose code holds the
command (`+` several at that offset); "←" the labels that reach it by plain
jumps (command 0x03); maps those whose area record's byte 8 names the
script, the location that record's byte 6 (string 0x400 + id). Which
counter cell or person reaches a label is not in this table.

| shop | row | script:label | maps | location |
|---|---|---|---|---|
| 1 | 0 | 0:444; 2:344+ ← 341, 342; 49:344+ | 9, 21 | 81 Burg; 158 Monster Test |
| 2 | 1 | 2:356+; 49:356+ | 21 | 81 Burg |
| 3 | 2 | 3:571, 3:572; 54:567+ ← 73 | 22 | 82 Saith |
| 4 | 3 | 5:612+ ← 10, 102; 5:621; 52:622, 52:623 | 25, 151 | 83 Meribia |
| 5 | 4 | 5:630 ← 11, 103; 5:631; 52:632 | 25, 151 | 83 Meribia |
| 6 | 5 | 5:632+ ← 12, 104; 5:641; 52:642 | 25, 151 | 83 Meribia |
| 7 | 6 | 5:642+ ← 13, 105; 5:651; 46:561; 53:553+ ← 30 | 25, 27, 151, 152 | 83 Meribia; 114 Black Rose Street |
| 10..13 | 9..12 | 9:392 / 402 / 412 / 422 ← 87 / 88 / 89 / 90 (and 9:395 / 405 / 415 / 425) | 31 | 86 Vane |
| 14 | 13 | 12:386+ ← 88; 12:391 | 29, 105 | 88 Lann; 124 Lann Island |
| 15 | 14 | 46:551; 53:544+ ← 25 | 27, 152 | 114 Black Rose Street |
| 16 | 15 | 46:541, 46:542 | 27, 152 | 114 Black Rose Street |
| 17..22 | 16..21 | 14:352, 360, 342, 494, 549, 539 | 39, 42 | 89 Reza; 126 Thieves' Guild |
| 23 | 22 | 17:230 ← 50, 106; 17:231..238 | 35, 113 | 90 Iluk; 129 Iluk Field |
| 24..26 | 23..25 | 15:400, 380, 320 | 45 | 91 Meryod |
| 27 | 26 | 20:250 ← 30, 108; 20:251 | 41, 54 | 92 Lyton; 134 Lyton Lake |
| 28..30 | 27..29 | 24:331..333, 361..363, 290 / 291 | 46 | 93 Tamur |
| 31 | 30 | 43:231 | 26, 62 | 113 Hispaniola |
| 32, 34, 38, 39 | 31, 33, 37, 38 | 45:474, 477, 838, 479 | 60, 70 | 116 Dross' Shop |
| 36 | 35 | 27:220 ← 105; 27:221 | 38 | 94 Pao |
| 37 | 36 | 53:536+ ← 20 | — | no area record names script 53 |

Rows no command opens: 7 (shop 8), 8, 32, 34 and 39 (the row the clamp
gives). Scripts 49, 52, 53 and 54 are named by no area record (a script
jump, command 0x42, is not followed). Shop 0x22 (34) is Dross' Shop's.

### Ramus' store

"Dross' Shop" (maps 60 and 70, script 45) becomes Ramus' store in the
story: script 45 sets the location to 0xA4 (command 0x49), whose name is
string 1188 "Ramus' Shop", and its four shops are his:

| shop | who, by the line before it | stock (the game's prices) |
|---:|---|---|
| 38 | Ramus himself: "Hey, Alex! I have great stuff for sale ... but it doesn't come cheap. I'm sorry, but I'm selling it barely above cost..." | Mia's Bromide 3, Jess' Bromide 3 (5,000 each) |
| 32 | his clerk: "Welcome to Ramus' store, green-eyes." | Silver Sword 1,800, Chain Mail 1,200, Silver Shield 900, Healing Nut 200, Star Light 1,000, Holy Water 100 |
| 34 | the clerk: "My manager said to take anything you want" — the free shop (0x22) | Healing Nut, Star Light, Angel's Tear, Holy Water |
| 39 | the clerk: "I guarantee that you want this item." | Rememberizer (65,000) |

These rest on the scripts' TEXT commands and the stock rows, read from the
disc's data.

**The front door, and who stands inside.** Meribia (maps 25 and 151, the
same planes, pages and CLUT rows byte for byte; script 5) has no kind-1
cell into the shop. Its door is a tile animation: the doorstep cells
(29..30, 44) (x 232..247, y 352..359) are kind 3, argument 4; Meribia's
row 4 copies 32 × 32 from (96, 848), the stash row along y 848..879, onto
(224, 336), the door drawn open, in both planes. The stash's cells (13..14,
107) hold kind 4, label 2, so after the copy the doorway (232..247,
344..351) runs label 2, whose first command is 0x26 with operand c[4] = 7:
scene entry 7 of the current map (151: map 70, set 70, clip x 48..480 y
0..448, start (264, 432) facing up; 25: the same into map 60). The shop's
second door (x 304..319, tile animation 5) copies in a kind-1 cell with
argument 8: entry 8, the back room (clip x 496..832, y 0..224). The other
harbour doors of the stash row (labels 4, 26..50) work the same way. (The
0x26 at 0x8A8 is label 6's: entry 6, map 27 / 152.) Leaving: the shop
room's one way out is its front door, kind 5 label 18 (cells (32..33,
55)), whose 0x26 takes map 70's entry 6, Meribia 151 at (240, 368) facing
down, in front of the door; the back room's door is kind 5 label 19 (entry
7, (312, 352)); kind 4 label 8 and kind-1 cells 5 and 8 lead to the sewers
(118) and Meribia from map 70's other rooms.

Set 70 (map 70's own objects) has eight people, none with a story bit, so
all of them stand there whatever the story's state. Ramus himself is
definition 63: kind 1, sheet 7 (his own field sheet), behaviour 3, cell
(24, 19) = (192, 152) facing down, beside the desk left of the long
counter; his talk byte 122 → label 410 → (a command 0x0B test) 443 → 838,
the TEXT before command 0x2E 38 above. The clerk is definition 55 (sheet
190, behaviour 3) behind the counter's right end at (336, 136); the kind-6
cell in front of it, label 127 at (336..351, 160..167), and its own talk
byte 124 both reach label 470, whose 0x0B tests lead to shops 34 and 32
(which one, by the story's state, was not followed further). The counter's
left half, label 23 at (272..287, 160..167), reaches label 380, lines
only. The eighth person (definition 2, sheet 153) is in another room of the
map, at (1752, 80). Set 60 holds the same room earlier in the story: Ramus
there twice (definitions 53 and 63, story bits 11 and 35), and a person of
sheet 216 behind the counter's left half (definition 52, bit 12). This
rests on the scripts' labels and scene changes, the maps' attribute planes
and the object sets, read from the disc's data.

## The title screen's data and draw

The C calls it the start screen (`start_screen.c`). `main` calls
`GameStart`, which calls `StartRun` (`0x8004345C`) and hands its answer to `GameEnter`,
which enters the field.

**The run — `StartRun(card_mask)`.** It seeds `g_StartMenuCardMask` with
its argument, sets the layers' order and mix, calls `StartInit` (`0x80044064`), and runs
steps from `g_StartFirstStep − 1` (no step at all for 0) until step 0xFF:

| step | function | next |
|---|---|---|
| 0 | `StartPictureLoadShow` (`0x800435A4`): `StartResourceLoad` (`0x8004409C`), then picture 0 of the resource shown to its end (`StartPictureShow(0)` (`0x8001607C`)) | 2 when the pad ended the picture, else 1 |
| 1 | `StartMoviePlay` (`0x800435FC`): movie 0, the opening, with skipping allowed on START (`g_MovieSkipAllowed = 0x800`) | 2 |
| 2 | `StartScreenShow` (`0x80043C64`), the logo and the prompt | 0xFF on START, 3 when its time runs out |
| 3 | `StartResourceLoad` | 1 |

So when the prompt has waited its time out, the start's resource is read
again and the opening movie plays again. Then `StartMenuRun` (`0x80043E08`); then
`MusicPlay(0, 2, 0)` and a fade out of the layers, of −2 for New Game and
−1 otherwise; the menu's answer is returned.

**The resource — `StartResourceLoad`.** `resource_load_run(3)` (`0x800202CC`) reads
resource 3 of LUNADATA.FIL and runs its list of commands; `g_BgMode =
-0x80`; `bg_data_reset` (`0x80014E60`) and `bg_attribute_data_reset` (`0x8001543C`). By the disc's data,
resource 3 holds five 320 × 224 8-bit TIMs at video memory (512, 0), CLUT
row 480 (the opening's publisher cards, shown before the title), and three
256 × 256 8-bit TIMs, each packed with inflate16 (command type 2), that are
the title's pages:

| page | VRAM x | CLUT row | holds |
|---|---|---|---|
| logo | 384 | 504 | the LUNAR SILVER STAR STORY COMPLETE logo (0,0x30 .. 0xF0 x 0x58), the copyright lines (0,0 0x100 x 0x30; 0,0x90 0x20 x 0x30) |
| press | 768 | 504 | "PRESS START BUTTON", 0xF8 x 0x10 cells, 15 of them (v = 0, 0x10 .. 0xE0) |
| menu | 896 | 505 | NEW GAME (0,0x10 0x68x0x10), CONTINUE (0x80,0x10 0x78x0x10; the form with no card 0x80,0x20 0x70x0x10), OPTION (0,0x30 0x50x0x10), the cursor's ten 16 x 16 cells along v 0 |

The logo and the press page both load their CLUT to video memory row 504
(x 0); the later one, the press TIM's, is what both draw with. Its entry
255 is 0x8000 (black with the STP bit: opaque), where the logo TIM's own
copy has 0 (see-through), so the logo drawn with its own CLUT would have a
speckled outline.

**The backdrop is a map.** `StartScreenDraw` (`0x80043848`) first calls `bg_draw`, which
draws the first tile layer at the scroll (`g_BgScrollX`, `g_BgScrollY`);
nothing on the start's path calls `bg_scroll_set` (`0x800145BC`), so the scroll is the
boot value 0, 0. The resource's type 3, 4 and 5 entries are its three
texture pages, the tile CLUT and the layout: by the data, a night sky of
one fill tile and two patches of star cloud. The planets, Earth and the
moon, are the second layer, drawn over the sky: with `g_BgMode = -0x80`,
`bg_second_draw` (`0x800148E0`) hands the layer to `bg_second_screen_draw` (`0x80014D74`) (rendering.md
§"The second tile layer"), a fixed grid of 21 × 14 sprites of 16 × 16 at
(column · 16, row · 16) from the type-7 block (`g_BgSecondBuffer`, 32
16-bit cells a row; 0 empty; no scroll), cell n being texel ((n & 0xF) ·
16, n & 0xF0) of type-3 page 2 (texture page 0x1D, mode word 0xE100029D)
with CLUT 0x7D40 (row 501). The layer's placement was compared by position
with the game running in an emulator.

**The flash palettes — `StartScreenInit`.** The first statement of
`StartScreenShow`. `StoreImage` reads the 256-colour CLUT row at video
memory (0, 0x1F8), then `DrawSync(0)`. For n from 1 to 32, every colour but
the first is moved n/32 of the way to white, each 5-bit channel c becoming
`(c * (32 - n) + 31 * n) >> 5`, and gets bit 15; the first is copied as
read. Each row goes with `LoadImage` to (0x200, 0xDF + n), then
`DrawSync(0)`. So rows 0xE0..0xFF at x 0x200 hold the palette from 1/32
white to all white. Then the seven view words `g_MapEffect`,
`g_MapSecondViewY`, `g_MapSecondViewX`, `g_MapViewY`, `g_MapViewX`,
`g_MapViewKeptY` and `g_MapViewKeptX` are set to 0, `MapViewStep(0, 1)`,
and one frame end (`MapFrameEnd`).

**The logo and the prompt — `StartScreenShow`.** It sets the mix of layers
4 and 8 to 0x20, the colour of 0xC to 0, starts a fade of the screen
(`screen_fade_start(0x20, 2)` (`0x8001C0CC`)), and then calls `StartScreenDraw(phase,
frame, 4)` once a frame:

| phase | lasts | what |
|---|---|---|
| 0 | until `layer_col_fade_check(0)` answers 0 | the screen fade started before the loop runs (`screen_fade_start(0x20, 2)`: 0x20 frames; bit 1 of the mode picks black or white, bit 0 whether the fade grows or dies away) |
| 1 | 0x20 frames | the logo goes to white (its CLUT row 0xE0 + frame) |
| 2 | 0x20 frames | the logo comes back from white (row 0xFF − frame) |
| 3 | up to 0x1388 frames | the prompt; at frame 0 `SoundMapSet(1)` (`0x800555CC`) and music 0x17; a new press of START (0x800) from frame 1 returns 1 after `SoundPlay(2, 3)` |
| 4 | until `layer_col_fade_check(0)` answers 0 | after 0x1388 frames of phase 3: `MusicPlay(0, 2, 0)` and `screen_fade_start(0x20, 3)`; then it returns 0 |

**The draw — `StartScreenDraw(phase, frame, row)`.** Called by
`StartScreenShow` with row 4 and by `StartMenuRun` with phase 0x10. It
steps the fades (`layer_col_fade_step`), calls `bg_draw` and
`MessageShadeDraw`, then adds sprites with `packet_sprite_add(x, y, u, v,
w, h, tpage, clut, 0x10)` (`0x800256C0`), the texture page made by `tpage_word_make` (`0x80043624`):

* always: the copyright lines at (0xF, 0xBC) and (0x10F, 0xBC), 0x100 and
  0x20 wide, 0x30 high, CLUT 0x7E00;
* phase 0x10 and up (the menu): the three rows at (0x69, 0x90 / 0x9D /
  0xAA), CLUT 0x7E40, the middle row in one of two forms by
  `g_StartMenuCardMask`, and the cursor at (0x5D, 0x90 + (row − 4) * 13),
  cell (frame % 40, folded at 20) / 2 of ten;
* phase 3: the prompt, 0xF8 wide at (0x2D, 0x99), v = (frame % 90 + 1) *
  0x10 while frame % 90 is below 14, else 0;
* last, the logo, 0xF0 × 0x58 at (0x2D, 0x34): CLUT `((frame + 0xE0) << 6)
  + 0x20` in phase 1, `((0xFF − frame) << 6) + 0x20` in phase 2 (rows 0xE0
  + frame and 0xFF − frame at x 0x200, `StartScreenInit`'s palettes), and
  0x7E00 otherwise.

It waits no frame and draws no random number; its callers end the frame.

**The menu — `StartMenuRun`.** First ten times `VSync(0)` and
`pad_record_update(g_BtlPadRecord, 0)`, then `screen_fade_start(0, 0)`. The
cursor `g_StartMenuCursor` starts on row 5 when `g_StartMenuCardMask` is not
0, else on row 4. Each frame while the cursor is 4, 5 or 6:
`StartScreenDraw(0x10, frame, cursor)`, `StartMenuCardUpdate` (`0x80043654`), `frame_end`
and `frame_start`, `pad_record_update`. A new press of 0x40 or 0x800 (CROSS
or START): `SoundPlay(2, 3)`, then by the row:

| row | what it does | its picture |
|---|---|---|
| 4 | returns 4, `GameEnter`'s way in with no save | NEW GAME |
| 5 | `StartMenuPageRun(0x14)`, the load page; a result that is not 0 is returned, 0 stays in the menu | CONTINUE |
| 6 | `StartMenuPageRun(0x15)`, the option page; then back to the menu | OPTION |

UP (0x1000): one row up, 4 round to 6; DOWN (0x4000): one row down, 6
round to 4; `SoundPlay(1, 3)`. Row 5 is passed over while the card mask is
0.

**The card mask — `StartMenuCardUpdate`**, once a frame of the menu.
`card_accept_poll()` (`0x800255A0`) takes one step of an accept on the port `g_CardPort`
(`MemCardAccept` and `MemCardSync`) and returns the last answer. The
port's bit is bit 0 for port 0 and bit 1 for port 0x10. On an answer of 1
the C writes `g_StartMenuCardMask &= 1 << (g_CardPort >> 4)`, which keeps
that port's bit and clears the other port's, and turns the port over; on
any other answer the port's bit is set. So two answers of 1 in turn, one
from each port, leave the mask 0, and any other answer lets row 5 be
chosen. With the mask 0 and the cursor on row 5, the cursor goes to row 4.

## The UI's graphics and text layer

Every battle UI graphic comes from LUNADATA resource 1 (SYSTEM.DAT),
resource 2, or the executable: found from the code that uploads and draws
them, and their placement compared with a capture of video memory taken in
an emulator during a battle. The party panel has no window, and the
command windows are text cells over one see-through rectangle with two edge
lines (§"The battle windows").

| asset | bytes | code | VRAM | drawn with |
|---|---|---|---|---|
| glyph atlas, 256×128 4bpp (values 0-3) | res 1, command type 2: an inflate16-packed TIM | `resource_tim_add` (`0x800208A0`) → `tim_image_load` (`0x8002073C`) (palette_row.c) | (960, 384) | tpage 0x1F, CLUT row 507 entries 11-14 |
| UI palettes | res 1, three type-12 commands (`word >> 8` = 2, 3, 7) | `resource_palette_row_load` (`0x80020ACC`): inflate16, colour 0 made 0 and bit 15 set on the others (`palette_bit15_set` (`0x800207E8`)), row 7 also clears 0xB0/C0/D0/E0 (`palette_glyph_zero_set` (`0x8002087C`)) | rows 502 (icons), 503 (faces), 507 (text colours) | — |
| battle row 507 | res 2's palette (replaces res 1's) | `sprite_reset` (`0x80018DBC`) | row 507 | sub-CLUTs 11-14 = the text colours |
| 256 icons, 24×24 8bpp | res 1's type-1 block at `+0x899C`: a 0x200-byte table of 16-bit offsets, then one LZSS stream (576 bytes) an icon | `IconCellSet(x, y, icon, dim)` → `IconImageLoad` (`0x8001ADEC`): a cache of 20 slots | (128 + 12·(slot % 10), 448 + 24·(slot / 10)) | `IconCellDraw` (`0x8001AD60`): tpage 0x92, palette 0x7D80 (row 502); colour 0x404040 when dimmed |
| label font, 16 px, 1bpp | the executable: `g_FontPatterns20` (ASCII 0x20-0x7A) and `g_FontPatterns9908` (codes 0x9908-0x99F8), 32 bytes a glyph (15 big-endian 16-bit rows) | `font_pattern_get` (`0x80026B1C`) (the pick), `font_pattern_outline` (`0x80026B98`) (ink 1, an outline 3 round it), `font_code_put` (width = the last inked column + 1, 5 for a blank; value \| colour 0/4/8/12) | drawn into 16×16 4bpp cells, sent by `font_line_image_load` (`0x80016350`) to the eight strips of `g_FontLines` | tpage from the strip, palette 0x7ECE |
| UI and effects sheet, 256×232 4bpp | res 2: a header of 16-bit offsets (palette `+0x14`, three animation banks `+0x214` / `+0x72C` / `+0x78C`, page `+0xBC0`), each inflate16-packed | `sprite_reset` | (960, 0) | the records' animation path (battle.md §"Spell presentations") |

Every glyph code in the string table falls in the label font's two ranges,
so its fallback to the BIOS's Kanji ROM (`Krom2RawAdd`) never runs on this
disc. Resource 2's three banks are `g_SpriteData0`, `g_SpriteData1` and
`g_SpriteData2`. The battle menu's marks are records bound to banks 2 and 0
(§"The menu's records"), the field menu's, the message's and the shop's
records to bank 0, entry 0 of the map's sprite data to bank 1
(`MapObjectsDataLoad` (`0x80065418`)), and the presentations' records to banks 0 and 2.

### The cell grid

`g_AnkprintGrid` is 40 × 28 16-bit cells of 8 pixels (320 × 224), and
`g_AnkprintRowMarks` marks the rows written to; `ankprint_cell_set` (`0x80025864`) writes a
cell and `ankprint_cell_get` (`0x800258A0`) reads one. `ankprint_grid_set(src)` (`0x800166D0`) writes a
whole screen of them, `ankprint_cell_set(x, y, *src++)` for 28 rows of 40;
its one caller is `font_lines_view` (`0x800167F4`), which puts back the grid
`ankprint_grid_get` (`0x80016668`) saved. `ankprint_draw` (`0x80025CEC`) (each frame) calls
`ankprint_cell_draw` (`0x80025C20`) for each cell that is not 0, by its kind, bits 8..10:

| kind | drawer | cell |
|---|---|---|
| 0 | `packet_sprite_add` | an 8×8 ASCII glyph `c = cell & 0xFF` (u = (c & 0x1F)·8, v = (c >> 5)·8 + 0xB8), palette 0x7ECE |
| 1 | `ankprint_glyph_draw` | atlas glyph `c`: width `((cell >> 11) & 7)·8`, CLUT 11 + `(cell >> 14)`; c below 0x80 is 8 high (v = 0x80 + (c >> 5)·8), c of 0x80 and up 16 high, and from 0xA0 read as c + 0x20. Writer `ankprint_glyph_set` |
| 2 | `font_cell_draw` (`0x80016134`) | label strip `(cell >> 11) & 7`, u = `cell & 0xF0`, width `((cell & 0xF) + 1)·16` |
| 3 | `IconCellDraw` | icon slot `cell & 0x7F`, bit 7 dimmed |
| 4 | `FaceCellDraw` (`0x8001B4B0`) | a 64×64 face, from texel ((cell & 1) · 64, 0xC0) of texture page 0x94, palette 0x7DC0 |

Another writer of the grid is the debug print `printmes(x, y, fmt, ...)` (`0x80025AC4`)
(ankprint.c; the name is the developers' own, from its assertion's text,
source file ANKPRINT.C; io-hardware.md §"The printf onto the cell grid").
It formats into the 0x40-byte buffer `g_PrintmesBuffer`, which ends where
`g_AnkprintGrid` starts, and writes one kind-0 cell a character with
`ankprint_cell_set`. Nothing in the image calls it. Its assertion lets
through `y < 0x1D`, so row 28, one past the grid's 28 rows, is accepted:
`g_AnkprintGrid + 28 * 80` is `g_AnkprintRowMarks`, the row marks (read off
the addresses).

### The atlas

Small digits are code d + 1 and tall digits d | 0x80 (`CellGlyphSet`); by
the atlas's pictures, `0x0B` is "LV"; a name tag is `id·4 + 0xC`, four
cells; the status words, four cells each, are `0x40` POISON, `0x44`
PARALYZE, `0x48` SLEEP, `0x4C` STONED, `0x50` CONFUSE, `0x54` MUTE, `0x58`
FAINT; `0x5D` H, `0x5E` MP, `0x5F` P. The party panel's colours are in
§"The party panel". No band is drawn behind the panel.

### The icon cache — `IconCellSet(x, y, icon, attr)`

It puts icon `icon` on cell (x, y) of the grid.

* `icon == 0`: `ankprint_cell_set(x, y, 0)` clears the cell; it returns 0.
* Otherwise `IconSlotFind(icon)` (`0x8001AC30`), the slot already holding it, or −1. On a
  miss: `IconSlotFindFree()` (`0x8001ABF4`) (the first slot whose icon number `+4` is 0);
  none, then `IconGarbageCollect()` (`0x8001AC6C`) (it marks the slots that cells of kind
  3 still name and frees the rest) and `IconSlotFindFree()` again; still
  none, `assert_fail("Garbage Error!", "ICON.C", 0x132)` (`0x8001B050`). `assert_fail` is
  an empty function in this build, so the code goes on with slot −1: the
  record is written 8 bytes before `g_IconSlots`, the picture goes to x
  0x74, and the cell stored is 0xFFFF (read from the code). Then
  `IconImageLoad(slot, icon)` unpacks the icon (`lz_unpack` (`0x80022BF8`) from
  `g_IconBase` + its offset) into `g_IconImageBuffer`, sends it with
  `LoadImage` to its slot's place, and waits with `DrawSync(0)`.
* Last, the cell: `ankprint_cell_set(x, y, (attr ? 0x80 : 0) | 0x300 |
  slot)`: kind 3 in bits 8..10, the slot in bits 0..6, and bit 7 dimmed
  (`IconCellDraw` draws it in 0x404040 instead of 0x808080).

The cache is the 20 slots of 8 bytes at `g_IconSlots` (`+0` / `+2` the
texel place, `+4` the icon number, `+6` the collector's mark), looked up by
icon number: a hit sends nothing and only writes the cell, so an icon shown
twice is unpacked once. No frame passes and no random number is drawn. A
RECT copied from `g_IconImageRectangle` to the stack is never read. The
word garbage and the file name are the developers' own, from the
assertion: the icon cache was a source file of its own, ICON.C.

### An icon put nothing calls — `IconSquareSet(x, y, cells, icon, attr)`

It writes a 3 × 3 square into a grid of 32-bit words, 64 words a row,
that the caller hands in: from word `y * 64 + x`, the first word `(icon *
18 + 0x200900) | attr` and, for an icon that is not 0, each of the nine 2
more than the one before, across the three rows (icon n takes the nine
values from n · 18 on); for icon 0 all nine are `0x200900 | attr`. It reads
nothing but its arguments and calls nothing. Nothing in the image calls
it: no jump reaches it and no data word holds its address (the executable
image was searched; the disc's other files were not). The grid it writes
is not the 40 × 28 grid of 16-bit cells `IconCellSet` writes, and no grid
of 64 words a row is met in the C. Two facts tie it to the icon code: its
arguments are those of `CellIconSet` (`0x800573B8`) (cell_text.c), `(x, y, <dropped>,
icon, attr)`, which hands them on as `IconCellSet(x, y, icon, attr)` with
the third dropped where this function takes the grid; and `0x200900`, the
word for icon 0 here, is the third argument, not read, of the program's two
3 × 3 clears of icon cells: `BtlWindow1IconClose` (`0x800497D4`)'s `BtlCellAreaClear(x, y,
0x200900, 3, 3, 0x10)` (`0x80049E7C`) (btl_window.c, just above it) and `ShopListDisp` (`0x800540B0`)'s
`BtlCellAreaClearCopy(x, y, 0x200900, 3, 3, 0x10)` (`0x8005417C`) (shop_screen.c). That
reads like the body of an older icon draw left behind after its callers
moved to `IconCellSet`; the code does not prove that history.

### Layout is code, not data

No table of the executable places a window: the handlers hold the cell
places. The battle menu's items are 12-byte records at `g_BtlMenuItems`
(§"The items"); `BtlMenuKindOpen`, for one, puts six icons at x = (i % 3)·5
+ 0x17, y = (i / 3)·5 + 2. `BtlMenuCursorMove` is the grid's cursor and
draws nothing. The target mark is 36 lines of colour going round a
rectangle (`OutlineScreenStep` / `OutlineDraw`, below; sizes
`g_OutlineKindSizes`, starting places `g_OutlineStartPlaces`, piece lengths
`g_OutlinePieceLengths`, colours `g_OutlinePieceCols`).

## Code first read with the menus

These functions were first read beside the menu code. They serve other
parts of the program; their own pages are named with each.

### The random numbers — `rng.c`

The generator is the subtractive one of Knuth, in the form of Numerical
Recipes' `ran3`: `g_RngNumbers` is its table `ia[56]` (32-bit words, word 0
never touched) and `g_RngCursor` its cursor. `rng_seed(seed)` (`0x8001C5F8`) stores the
seed in word 55 and fills the others at index `21 * i % 55` for i = 1..54
(`k = seed - k`, plus 10^9 when below 0), then refills the table three
times and sets the cursor to 55. `rng_refill` (`0x8001C548`) is `ran3`'s refill:
`ia[i] -= ia[i + 31]` for i = 1..24 and `ia[i] -= ia[i - 24]` for 25..55,
each plus 10^9 when below 0. `rng_next` (`0x8001C6B0`) moves the cursor on, refills and
sets it to 1 when it passes 55, and returns the word under it; `rng_below(n)` (`0x8001C704`)
does the same and returns `(n * (u16)word) >> 16`, a number below n.
`rng_draw` returns `rng_next()`; it is the draw the rest of the program
calls (68 calls in 12 files), and `rng_draw_copy` (`0x8001C790`) does the same. Three
things differ from the book's `ran3`: the seed is used as it is (the book
uses 161803398 − seed), the table is refilled three times where the book
does it four, and the draw takes the 55 words of each refill in order
where the book steps two cursors 31 apart, one word at a time. The program
seeds it once, with 0x3039 (12345), in its start-up `system_init` (`0x8001EEB8`)
(frame.c). frame-rng.md has who draws.

### The face slots — `face.c`

Face data is read from the disc into six areas of 0x3000 bytes,
`FaceSlotAddress(slot)` (`0x8001B15C`) = `0x801A9B00 + slot * 0x3000`. `FaceSlotGet(n)` (`0x8001B200`)
returns the slot that holds face data n: it looks through the six bytes of
`g_FaceSlotNumbers` for n + 1 and returns that slot when `FaceDataCheck` (`0x8001B54C`)
accepts what lies there (a four-byte tag, a length from 1 to 0x4E1F, and a
matching sum); otherwise it steps `g_FaceSlotLast` (0 after 5), reads the
data into that slot (`FaceDataLoad` (`0x8001B1C0`)) and stores n + 1. When the check
rejects what was read it first calls `assert_fail` with a message, a file
name and line 0x9E: the developers' file name for this code is the one
face.c takes. `FaceSlotReset` (`0x8001B178`) clears the six
bytes and `g_FaceSlotLast`; its callers are `FaceSlotResetCopy` (`0x8001B1A0`) and
`MoviePlay` (`0x800232D4`), after the movie.

**A face's picture — `FaceImageLoad(slot, picture, page)` (`0x8001B310`).** It unpacks the
slot's face data, from its byte 0x20 on, with `inflate_buffer` (`0x80021D6C`) into the
buffer `g_LoadBuffer` points to, and sends the 0x1000 bytes of the picture
(64 × 64 pixels at 8 bits) with `LoadImage` to the rectangle of 32 × 64
words at (0x100 + 32 * page, 0x1C0) of video memory, then `DrawSync(0)`.
Its one caller is `FaceCellSet` (`0x8001B3DC`). `FaceCellDraw` draws a face cell from
texel ((cell & 1) · 64, 0xC0) of texture page 0x94, the same place, so two
pages are drawn.

### The two layer records — `layer_col.c`

`g_LayerBgFirstRecord` (`0x800AE16C`, mask bit 4) and
`g_LayerBgSecondRecord` (`0x800AE190`, bit 8) are the first two of four
0x24-byte layer records: the colour fades of the map's two tile layers
(rendering.md). `+0x1C` is the mix, `+0x1E` the order number.

* `layer_mix_fade_start(mask, -, frames, mix)` (`0x8001EBA4`): bit 4 stores a mix below
  0x20 in the first record with 1 at `+0x20` (a mix of 0x20 or more stores
  only 0 at `+0x20`), then `layer_col_apply(mask)` (`0x8001E10C`). Bit 8 in a battle fades
  the tile layer's level (`tile_semi_trans_set(1)` (`0x8001DD00`), `tile_mix_fade_start` (`0x8001DDA4`));
  outside one, the same stores in the second record. Nothing in the C reads
  `+0x20`.
* `layer_mix_set(mask, level)`: bit 4 (outside a battle) and bit 8 as above
  at once; bits 0x100, 0x200, 0x400 and 0x800 set the four sprite mix
  levels (`sprite_mix_set(0..3, level)` (`0x8001A8D0`)).
* `layer_order_set(mask, n)` stores the low byte of n at `+0x1E` of the
  first record for bit 4 and of the second for bit 8; no other bit is
  tested, so most of its calls store nothing.
* `layer_mix_mode_get()` (`0x8001DFE8`): 1 when the second record's mix is 14..18; 5 for
  any other mix of 1..31; otherwise 3 when the first record's order number
  is below the second's, else 0. `bg_second_draw` draws the second layer by
  it.
* `layer_mix_weights_get(out)` (`0x8001E08C`): for modes 1 and 5, the second record's mix
  and 0x20 less it (the first record's when `g_BgMode` is 1, the other way
  round); for the other modes both 0x10.
* `layer_mix_switch_off(bits)` acts only for 0x100 (`sprite_mix_switch_off` (`0x8001A8F8`));
  any other argument does nothing, so `BtlMenuInit`'s
  `layer_mix_switch_off(8)` does nothing.

### The moving outlines — `outline.c`

`OutlineDraw(x, y, kind, frame)` draws a lap of 36 line pieces round a
rectangle (`g_OutlineKindSizes`: kind 0 50 × 26, 1 26 × 26, 2 18 × 26),
nine piece lengths (`g_OutlinePieceLengths`) in nine colours
(`g_OutlinePieceCols`, from 0x1F1F1F down to 0x00071F) four times round,
starting at `g_OutlineStartPlaces[kind][frame]` along the top edge and
cutting a piece at each corner; the frame moves the start, so the colours
run round. Two callers pass a counter / 7 as the frame:

* `OutlineMapStep(records, n, mode)` for 0x3C-byte records. Mode 0:
  records 0..4 whose `+4` word is not 0, slot n (the field menu,
  `MenuRecordsAnimationStep`). Mode 1: records 2..4 whose first byte is not
  0, slot n + 7 (the shop, `ShopFrameStep`). Unless bit 0 of the record's
  `+0x30` is set, the slot's counter in `g_OutlineMapFrameCounts` goes up,
  back to 0 after 0x29. The outline goes at the record's x and y less the
  scroll (`bg_scroll_get` (`0x80014030`)) less 1, kind `g_OutlineMapKinds[slot]` (1, 2, 1,
  2, 1 for slots 0..4; 0, 2, 1 for 9..11).
* `OutlineScreenStep(records, n, -)` for 0x40-byte records 2 and 3 only,
  with its own counters (`g_OutlineScreenFrameCounts`) and kinds
  (`g_OutlineScreenKinds`, 1 for all), no scroll taken off, and a third
  argument it does not read. Its one caller, `BtlMenuOutlineStep`, passes
  the battle menu's mark records 1, 2 and 3; record 1 is turned away.

### Map objects — `map_touch.c`

* `MapObjectCursorAdvance(stop)` (`0x8007B724`) moves `g_MapObjectCursor` on (0x7F round
  to 0) to the next row of `g_MapObjects` whose first byte is 4..6 and whose
  `+0x3D & 0x3F` is not 3; when it comes round to `stop` it calls
  `sprite_run_off(0x6C, 2)`, switching sprite entries 0x6C and 0x6D off,
  and stops there. Its one caller is `MapObjectTouchRemove` (`0x8007B5CC`). It is map
  object code, not a menu cursor.
* `MapObjectTouchApproach` (`0x8007B310`) keeps the touched object's x, ground y, facing
  and bits 0x6000 of `+0xE` (`g_MapTouchedObjectKeptX`, `…KeptY`,
  `…KeptFacing`, `…KeptClass`, which `MapBtlRun` puts back after a battle
  that answered 2), copies its box `+0x50` grown by one, poses it
  (`MapAnimationPoseSet(row, 2, facing)`), sets the probe box
  (`g_MapProbeBoxX` on) to the party's box at the first party record's
  place, and runs 0x20 frames (`MapFrameStep(1, 1)` (`0x800557D4`) and `MapBgChangeStep`):
  until `MapBoxTouchGet` (`0x80067BFC`) finds the boxes touching or a move fails, it aims
  at the first party record (`MapFacingTowardGet` (`0x8005FBBC`)) while its last move was
  along its aim, and `MapSpotMoveTry` (`0x8006661C`) moves it one pixel. It is
  npc-scripting.md's touch.
* `MapObjectTouchWait` (`0x8007B4D4`) is the same without the walk: 0x20 frames go by.
  Nothing reaches it: no jump, and no data word holds its address.

### The disc's track table — `cd_toc_read`

`cd_toc_read` (`0x8001B63C`) (cd_track.c) keeps `CdGetToc(g_CdTocEntries)`, the number of
tracks, in `g_CdTrackCount`, then runs a loop from 0 to that number with
nothing in its body. `cd_track_play` (`0x8001B684`) takes the buffer as 4-byte entries
and plays nothing for a track above `g_CdTrackCount`. Its one call is in
`cd_dir_init` (`0x80020F98`) (cdread.c). io-hardware.md has the disc.

### The presentations' records

`BtlShowNumberRun` (`0x8007BB80`), the `BtlShow…Run` functions of `g_BtlShowFunctions`
and the helpers below are the battle's presentations, what an action looks
like on the screen; battle.md §"Spell presentations" and
battle-presentations-2.md have them by number. Their records:

* **Running one — `BtlShowNumberRun(n)`.** 0: nothing. 0x97..0xFD:
  `BtlActingShowRun(n)`. Any other n: `BtlShowOpen(n)`, the function of word
  n − 1 of `g_BtlShowFunctions`, `BtlShowClose` (`0x8007BACC`). The table at `0x800B5CA4`
  has 90 words, for 1..0x5A; nothing keeps a number from 0x5B to 0x96, 0xFE
  or 0xFF from reading a word past it (word 90 is the table of effect
  resource numbers). The first split's label `menu_page_table`
  (`0x800B5CFC`), which nothing in the C names, falls inside this table at
  word 22: it is no menu's page table.
* **`BtlShowOpen(n)`** zeroes `g_BtlShowBlocks[0..2]`. In a battle it takes
  0x540 bytes for `g_BtlShowRecords`, 0x15 show records of 0x40 bytes
  (record 20, at `+0x500`, is the effect's record), 0x1910 for
  `g_BtlShowWaveLines` and 0x18 for `g_BtlShowWaveBlock`; sets
  `g_BtlShowOpen`, so the show frame `BtlShowFrameStep` steps the records
  into sprite entries 0x1C on; clears the voice byte, the colour pulse
  `g_BtlShowPulse` (0x18 bytes: on, which colour is next, a length, two
  colours) and the records; switches amount records 0..12 off; and sets
  the layers' colours (`layer_col_set(-1, 0x8013D, 0)`, `layer_col_set(0,
  0xC, 0)`). Outside a battle it takes only `g_MenuUseShowRecords`.
  `BtlShowClose` frees them again.
* **The travel — `BtlTravelRun(block, animation)` (`0x8008AE10`).** The 0x20-byte
  `BtlTravelBlock` (btl.h): `+0` the mode, `+1` the step (pixels a point:
  8, 0x10 or 2), `+4` the record that travels, `+8` / `+0xA` its offsets,
  `+0xC` the record it travels to, `+0x10` / `+0x12` those offsets, `+0x14`
  and `+0x18` the two paths. It puts the record on the animation, makes the
  two paths (`BtlTravelPathsMake` (`0x8008AA3C`)), and for each point sets the record's x
  and y from the first and its `+8` from the second, one show frame each;
  with a mode not 0 it turns the two records toward each other. Its callers
  are `BtlShow01Run` (`0x8007BE00`), `BtlEffectShotRun` (`0x8007ECA4`), `BtlShowRecordTravelRun` (`0x800859A8`),
  `BtlEffectCopyAboveShotRun` (`0x80085D2C`) and `BtlShow20Run` (`0x80081AD8`).
* **`BtlEffectCopyEnterRun(a, colour, b)` (`0x8007CA30`)**, the opening shows 5..8 share:
  the effect's record plays animation b; the layers of mask 0xC fade to the
  colour over 0x14 frames; show record 5 (`+0x140`) takes a copy of the
  effect's record and walks a path of 0x20 points from `line_path_make`
  (an 0x80-byte block from `xmalloc_heap_alloc`, freed after), one point a
  show frame.
* **`BtlShowPlacesPlay(a, b)` (`0x8007F2D8`)**: sound 0x21, then for each of the 14 slots
  whose show place is on (`g_BtlSlotShowPlaceFlags`), show record i bound
  to `g_SpriteData0` with word a and animation b at
  (`g_BtlSlotShowPlaceXs[i]`, `g_BtlSlotShowPlaceYs[i]`); the others are
  cleared; then the amount show and animation b of the last record made,
  played to its end.
* **`pair_list_sort(pairs, n, direction)` (`0x8007C2C4`)** sorts pairs of signed 16-bit
  numbers {key, second} in place: by key from the largest for direction 1,
  from the smallest otherwise, equal keys by their second number from the
  smallest. It is the textbook bubble sort: n passes comparing neighbours.
  Its one caller is `BtlShow03Run` (`0x8007C450`).
