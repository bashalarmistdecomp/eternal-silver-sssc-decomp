# Battle System

This page is the battle of the main executable (`SLUS_006.28`): how a fight
is entered and set up, what the battle keeps for each slot, how an action's
amount is worked out, what the action handlers do, how the battle's pictures
are loaded and drawn, how a spell or an enemy action is presented, the
statuses, the round that runs it all, and the win and its level-ups. The
code is in the `btl_*.c` files of `decomp/SLUS_006.28/src`; the battle's
records and stats are typed in that folder's `include/btl.h` (`BtlRecord`,
`BtlStats`), the character records in `include/chr.h` (`ChrRecord`). The enemies' choices and the bosses are on
boss-ai.md, the map enemies that start a fight on map-enemies.md.

## Where a battle runs

* `MapBtlRun` (`0x8007AE58`) (map_btl.c) leaves the map, calls
  `BtlRun` (`0x80027434`) and puts the map back. It is called from two
  places: the touch of a map enemy (`MapObjectTouchRun` (`0x8007ACD8`),
  map_btl.c) and the script's BATTLE command (`ScriptCommandMapBtlRun`
  (`0x8005DFF0`), lns_intp.c). Both fill the party's ids into
  `g_BtlSlotIds` first.
* `BtlRun` (btl_run.c) is one battle from its set-up to its end message.
  Its argument is the battle's number, kept in `g_BtlNumber`. It sets
  `g_BtlRunning` to 1, keeps a copy of the five party ids, calls the set-up
  `BtlInit` (`0x80029E68`) (btl_init.c), and runs rounds while the result
  is 0: each round ticks the statuses (`BtlRoundStatusUpdate`
  (`0x800297A4`)), tests the end (`BtlEndCheck` (`0x80038518`)), lets the
  enemies plan, takes the party's commands (`BtlMenuRun`) or the party's
  own choices, and runs the round's turns (`BtlRoundTurnsRun`). After the
  loop it plays the end, writes the party's HP, MP and items back
  (`BtlPartyStatsStore` (`0x80028074`)), closes the battle (`BtlClose`
  (`0x80029214`)) and sets `g_BtlRunning` to 0. §"The round loop" has the
  round in full.
* The battle has 14 slots: 0 to 4 the party, 5 the companion (id 0xC, set
  apart from the sides, the target scans and the victory), 6 to 13 the
  enemies. `g_BtlSlotIds` (`0x800E48E0`) holds each slot's id: a character
  number for slots 0 to 4, 0xC for slot 5, an enemy number for 6 to 13, 0
  for an empty slot.
* Three kinds of record serve a slot:
  * the battle record, `BtlRecord`: 0x1C records of 0x40 bytes at the
    address in `g_BtlRecords` (`0x800B8C58`), which `BtlStateInit`
    (`0x8002B7DC`) sets to `0x801D2E80`. Record n is slot n's figure (its
    sprite entry, position and animation); the records past 13 serve the
    battle's other pictures;
  * the stats, `BtlStats`: 14 records of 0x3C bytes at the address in
    `CharStats` (the word at `0x800B8C20`), which `BtlStateInit` sets to
    `0x801D2B00` (§"The stats");
  * the character record, `ChrRecord`: 0x40 bytes per character id at the
    address in `g_ChrRecords` (`0x800B8BE4`), `0x801DBF80`. It lasts from
    battle to battle; a battle copies it into the stats at its start and
    writes part of it back at its end.

## Not the battle: the tables at `0x800B5B1C`

The C still calls two labels in this run `battle_state_table`
(`0x800B5B1C`) and `battle_cmd_table` (`0x800B5B44`). Both names are the
first split's and were never read (`src:inherited` in the register): they
say nothing about the tables. The 28 function addresses from `0x800B5B1C`
on are three tables, none of them the battle's:

* entries 0 to 7: the table of `MenuUseRun` (`0x80076444`) (menu_use.c),
  the use of an item or a spell from the field menu (below);
* entries 8 to 21: `g_MapObjectPartyLookFunctions` (`0x800B5B3C`), the
  fourteen walkers of a roaming map enemy; `battle_cmd_table` is entry 10,
  inside this table;
* entries 22 to 27: `g_MapObjectPartySeenFunctions` (`0x800B5B74`), the
  six walkers of an alerted map enemy.

The walkers (`MapObjectPartyLook00Step` … `MapObjectPartySeen06Step`,
map_look.c) and the map object rows they move are on map-enemies.md.

`MenuUseRun(user, number, target)` takes a number below 0xB7 as an item,
and the low four bits of byte +7 of its 0x12-byte row (`g_ChrItemRows`)
pick the entry; a number from 0xB7 is spell `number - 0xB6`, and the low
four bits of byte +3 of its 12-byte row (`g_ChrSpellRows`) pick it. The
four bits are stored in `g_MenuUseEntry` (`0x800B8FB8`) and that entry is
called. When the entry has set the used mark `g_MenuUsedMark`, the item is
taken away (`ChrItemRemove`) or the spell's cost is taken off the user's
MP: byte +4 of its row, halved and rounded up when byte +0x22 or +0x23 of
the user's character record is 0x88.

| entry | function | what it does |
|---|---|---|
| 0 | `MenuUseNumberHang` (`0x80076688`) | for a row whose four bits are 0: shows the used number and the entry in a message, then loops for ever (the function has no way out) |
| 1 | `MenuUseTargetRun` (`0x800767D8`) | the row's action on one target: `g_BtlTargetSlot` = the target, its place's position as show place 0, `BtlActionRowRun` (`0x80031ABC`), which outside a battle works on the target's character record; used mark 1 |
| 2 | `MenuUsePartyRun` (`0x80076844`) | the same on the party: every id at `g_PartyIds` up to the first 0 (six at most), passing over 0xC, goes to the next slot of `g_BtlSlotIds` with its show place; `BtlActionRowRun`; used mark 1 |
| 3 | `MenuUsePlaceNameRun` (`0x80076968`) (menu_list.c) | a three-row list of place names 0x51 to 0x5F whose story bit `name + 0x28A` is set (`MenuUsePlaceNameBitCheck` (`0x800770B0`)); the one taken becomes the menu's request, which `MenuRun` answers with `ScriptChange(0x64)` and `ScriptLabelRun(request)`. The item keeps (used mark 0) |
| 4 | `MenuUsePictureRun` (`0x8007690C`) | `MenuUseMoviePlay((byte +7 >> 4) + 0x37)`: the high four bits of the same byte pick the picture (video.md §"Which movie is which"); the item keeps |
| 5 | `MenuUseMapRow2Run` (`0x800770E4`) (menu_list.c) | ends the menu with request 5, which `MenuRun` answers with `MapRow2Change`; used mark 1 |
| 6 | `MenuUseMusicRun` (`0x80077190`) (menu_list.c) | a list of music numbers (`MenuUseMusicListSet` (`0x80077380`), story bits `0x19A + n`; sound.md §"BGM numbers"), then the music that played before |
| 7 | `MenuUseMovieRun` (`0x8007757C`) (menu_list.c) | a list of movies (`MenuUseMovieListSet` (`0x800776CC`), story bits `0x153 + n`) |

Entries 6 and 7 share the list loop `MenuUseListRun` (`0x80077A6C`)
(menu_list.c), each installing its three handlers in
`g_MenuUseListSetFunction`, `g_MenuUseListPickFunction` and
`g_MenuUseListDispFunction` first. `MenuUseRun` masks the four bits and
calls the entry with no further test, so bits of 8 to 15 would call map
walkers.

## The battle records — `BtlRecord`, 0x40 bytes at `0x801D2E80`

`g_BtlRecords` (`0x800B8C58`) points at 0x1C records of 0x40 bytes;
`BtlStateInit` (`0x8002B7DC`) sets it to `0x801D2E80` and clears 0x700
bytes. Record n is slot n's figure. The fields, as `include/btl.h` has them:

| offset | field |
|---|---|
| `+0x00` | on: 1 while the record is in use; `BtlAnimationStep` (`0x800272A8`) steps and copies only records whose word is not 0 |
| `+0x04..+0x27` | the sprite entry (`SpriteEntry`, sprite.h) copied to the sprite list every frame: `+0x04` its on word, `+0x06` x, `+0x08` depth, `+0x0A` y, `+0x0C` attribute (bit 0x2000 mirrors), `+0x0E` draw word (palette, class), `+0x10` texel records, `+0x14` pose table, `+0x18` piece table, `+0x1C` pose, `+0x20` scaled, `+0x24`/`+0x26` scale |
| `+0x28` | the animation table's address |
| `+0x2C` | the step list's address |
| `+0x30` / `+0x32` | the animation, and the one before it |
| `+0x34` / `+0x36` | frames spent on the step, and the step's length |
| `+0x38` / `+0x3A` | the step, and one past the animation's last step (bit 15: hold the last step) |
| `+0x3C` / `+0x3E` | the figure's width and height, measured by `BtlRecordSizeSet` (`0x800381D8`) |

The depth `+0x08` is kept equal to y `+0x0A` while a figure stands on the
ground; a fall, a jump, a lift or a shake moves y alone. In the `battle`
state captured from the running game (Alex, Luna and Ramus against four
Slimes) the Slimes are records 6 to 9 (`0x801D3000` to `0x801D30C0`), and
record 0's x went 271, 221, 16 as Alex walked to his target. The pair
`+0x08` / `+0x0A` is equal for every figure at rest and differs for one
lifted mid-step, which makes it look like current and maximum HP to a
search of memory; the HP are in the stats.

## The stats — `BtlStats`, 0x3C bytes at `0x801D2B00`

`CharStats` (the word at `0x800B8C20`) points at 14 records of 0x3C bytes,
one per slot. `BtlStateInit` stores `0x801D2B00` there and clears 0x348
bytes, which is 14 × 0x3C. The stats are the battle's own: at a battle's
start `BtlPartyStatsSet` (`0x8002A138`) fills slots 0 to 4 from the
character records and `BtlEnemyStatsSet` (`0x8002A638`) fills 6 to 13 from
the enemies' rows; at its end `BtlPartyStatsStore` (`0x80028074`) writes
the party's HP, MP and items back to the character records. Slot 5's
stats are written by neither.

In the `battle` state slots 0 to 2 are Alex, Luna and Ramus (ids 1, 6
and 7), the stats of slots 3 to 5 are empty (slot 5 holds Nall, id 0xC, whose
stats neither copier fills) and 6 to 9 are the four Slimes.

| offset | type | field | what the C does with it | measured in the `battle` state |
|---|---|---|---|---|
| `+0x00` | u16 | hp | taken down by hits (`BtlHitAmountTake` (`0x80030650`)), raised by heals; `BtlFallRun` (`0x800385B4`) sets 0 | 32, 24, 42 for the party, 13 for each Slime; 13 → 0 as each Slime fell |
| `+0x02` | u16 | hp_max | heals stop at it; `BtlWinRun` (`0x80028140`) adds a level's gain | rose 42 → 46 for the one who gained a level |
| `+0x04` | u16 | mp | spells' costs come off it (`BtlActionSpellRun` (`0x8002E718`)) | Luna 40 → 36 after Healing Song |
| `+0x06` | u16 | mp_max | | Ramus 0 / 0 |
| `+0x08` | u16 | stat_08 | what a hit's amount starts from (`BtlHitAmountSet` (`0x80030070`)) | |
| `+0x0A` | u16 | stat_0a | taken off a hit's amount | |
| `+0x0C` | u16 | stat_0c | the key the round's turn order is sorted by (`BtlTurnOrderMake` (`0x8002F4B0`)) | |
| `+0x0E` | u16 | stat_0e | what a stat-0E amount starts from (`BtlStat0EHitAmountSet` (`0x80035EF8`)); the status chance | |
| `+0x10` | u16 | stat_10 | taken off a stat-0E amount | |
| `+0x12` | u8 | attack_count | how many times an attack hits; kind 3's percentage is applied that many times less one | |
| `+0x13` | u8 | walk_reach | the most cells a walk may cover (`BtlWalkSet` (`0x8002BAA4`)) | 12, 10, 10 for the party, 8 for the Slimes |
| `+0x14` | u8 | level | the level gap adds to a hit's amount | 1 for the party; 1 → 2 for the one who gained a level |
| `+0x15` | u8 | status | the status bits (§"Actor animation" lists them) | |
| `+0x16` | u8 | least_amount | the least amount the slot's hit takes | |
| `+0x17` | u8 | attack_show | an enemy's attack show (`BtlActingShowRun` (`0x8003A158`)) | |
| `+0x18` | u8 | walk_speed | the walk's speed; 3 for the party | |
| `+0x19` | u8 | attack_sound | the attack's sound | |
| `+0x1A..+0x2D` | | side | party slots: `+0x1A` / `+0x1C` stat_08 and stat_0a at the start, `+0x1E` flags, `+0x1F` stat_1f, `+0x20` the first worn item's kind, `+0x22..+0x27` six worn items, `+0x28..+0x2D` six traits. Enemy slots: `+0x1A` action pick, `+0x1B` trait, `+0x1C` attack number, `+0x1D` kind-7 number, `+0x1E` walk sound, `+0x1F` action sound, `+0x20..+0x23` two items a won battle may give and their rates, `+0x24` experience, `+0x26` money, `+0x28` weak mask, `+0x2A` resist mask | |
| `+0x2E..+0x33` | u8 × 6 | items | the six items the slot carries | |
| `+0x34..+0x3B` | u8 × 8 | spells | the slot's eight spells | |

The stats hold no experience: `BtlWinRun` adds a won battle's experience
to the character records' totals (`+0x38`, held at 9,999,999) directly. After a won fight in the
`battle` state only HP and MP (and, for the one who gained a level, `+0x02`
and `+0x14`) had changed, and Luna, untouched, showed no change at all.

### The enemies' rows — `g_BtlEnemyRows` (`0x800A8768`), and `BtlEnemyStatsSet`

`g_BtlEnemyRows` is a table of 38-byte (0x26) rows in the executable's
data (`BtlEnemyRow`, btl.h), never written. `BtlEnemyStatsSet` (btl_init.c)
is the only code that reads it. For each slot 6 to 13 it takes the slot's
id from `g_BtlSlotIds` (`0x800E48E0`), stops at the first 0, and copies row
`id − 1` field by field into the slot's stats: enemy ids are 1-based, and
row 0 is the Slime's. It calls nothing.

| row | width | → stats | field |
|---|---|---|---|
| `+0x00` | u8 | `+0x1A` | action pick |
| `+0x01` | u8 | `+0x14` | level |
| `+0x02` | u16 | `+0x02` and `+0x00` | hp: stored twice, so an enemy starts full |
| `+0x04` | u16 | `+0x08` | stat_08 |
| `+0x06` | u16 | `+0x0A` | stat_0a |
| `+0x08` | u16 | `+0x0C` | stat_0c |
| `+0x0A` | u16 | `+0x0E` | stat_0e |
| `+0x0C` | u16 | `+0x10` | stat_10 |
| `+0x0E` | u8 | `+0x13` | walk_reach |
| `+0x0F` | u8 | `+0x16` | least_amount |
| `+0x10` | u8 | `+0x12` | attack_count |
| `+0x11` | u8 | `+0x1B` | trait |
| `+0x12` | u8 | `+0x1C` | attack number |
| `+0x13` | u8 | `+0x1D` | kind-7 number |
| `+0x14` | u8 | `+0x1E` | walk sound |
| `+0x15` | u8 | `+0x18` | walk_speed |
| `+0x16` | u8 | `+0x17` | attack_show |
| `+0x17` | u8 | `+0x19` | attack_sound |
| `+0x18` | u8 | `+0x1F` | action sound |
| `+0x19` | u8 | — | the one byte the copier does not read |
| `+0x1A` | u16 | `+0x24` | experience |
| `+0x1C` | u16 | `+0x26` | money |
| `+0x1E` | u8 | `+0x20` | first item a win may give |
| `+0x1F` | u8 | `+0x21` | its rate |
| `+0x20` | u8 | `+0x22` | second item |
| `+0x21` | u8 | `+0x23` | its rate |
| `+0x22` | u16 | `+0x28` | weak mask |
| `+0x24` | u16 | `+0x2A` | resist mask |

Row 0 put through this map gives the four Slimes' stats as they were
measured in the `battle` state, 26 bytes of 26 compared. The copier writes
neither MP nor the status byte `+0x15`: both stay as `BtlStateInit`'s clear
left them (0) unless the battle's set-up gives a status (`BtlInit`
(`0x80029E68`), `BtlSpecialInit` (`0x8002A7FC`)). The C bounds the id only
by the byte it is read from, so the row count is not read from the code.

### The party half — `BtlPartyStatsSet` (`0x8002A138`)

`BtlPartyStatsSet` (btl_init.c) fills the stats of slots 0 to 4 from the
same `g_BtlSlotIds`, stopping at the first 0. Its source is the slot's
character record, `*g_ChrRecords + id * 0x40` (`ChrRecord`, chr.h), with
the worn items' rows from `*g_BtlItemRows` (`0x800B863C`, which holds
`0x800A9A68`, the item table `g_BtlItemTable`). With `C` the slot's stats
and `p` its character record:

* `C+0x00..+0x11` = `p+0x00..+0x11` (hp, hp_max, mp, mp_max and the five
  stats), and stat_08 and stat_0a again at `C+0x1A` / `C+0x1C`;
  `C+0x12` / `+0x13` = `p+0x12` / `+0x13`; `C+0x14` = `p+0x1C` (level);
  `C+0x18` = 3; `C+0x1F` = `p+0x1D`; `C+0x2E..+0x33` = `p+0x24..+0x29`
  (items); `C+0x34..+0x3B` = `p+0x2A..+0x31` (spells).
* The first worn item (`p+0x1E`), when there is one: `C+0x22` = it, and
  from its row `C+0x16` = `+0x0B`, `C+0x17` = `+0x0E`, `C+0x28` = `+0x0D`
  (its trait), `C+0x20` = `+0x0F` (its kind). The kind picks the slot's
  attack, stand and move animations and its attack sound (§"Party stage
  state"). With no first worn item the animations are 10 / 5 / 0 and the
  sound 0x44.
* Worn items 2 to 6: `C+0x23..+0x27` = `p+0x1F..+0x23`, and
  `C+0x29..+0x2D` = each one's row byte `+0x0D`, with no test for an empty
  place: an empty one reads the bytes before the item table.
* `g_BtlPartyHitAnimations[slot]` = 0x12 and `g_BtlPartyNotHeldFlags[slot]`
  = 1. `g_BtlPartyNotHeldFlags` is the party's "may take a command" flag:
  effects 86 and 92 clear it while a member is in a boss's form (boss-ai.md
  §"The transformations" lists every reader and writer).

So a party member's battle stats are made from the character record when
the battle starts; they are stored nowhere. The character records are at
`0x801DBF80`, an address `GameStart` (`0x80055B60`) (game_start.c) stores
in `g_ChrRecords` among the 22 fixed addresses it sets; it is the word's
one writer.

Checked against the `battle` state after the code was read: ids 1, 6 and 7
put through this function give the state's three party records byte for
byte, except Alex's `+0x2E..+0x31` (items 1, 25, 134, 168: Dagger, Sling,
Flame Ring and Alex's Ocarina by the string table), which play had changed
since level 1.

The level-1 records are LUNADATA resource 0xE7. `ChrRecordsReset`
(`0x80063B30`) (map_sprite.c) gives the party and the character records
the values of a new game. `GameStart` calls it once at boot, and
`GameRecordsReset` (`0x80055E78`) (game_start.c), which clears the story
bits first, calls it for a new game. In order it makes the party id 1
alone (`g_PartyIds` = 1, 0, 0, 0, 0, 0), sets story bit 0x2BD, reads
resource 0xE7 (`resource_load` (`0x80020230`): resource n is the sectors
`g_ResourceSectorPairs[n]` names of LUNADATA.FIL; 0xE7 is one sector),
copies its first 0x300 bytes over the twelve character records (ids 0 to
11) and its 0x58 bytes from `+0x342` to `*g_ChrPlacePresetRecords + 8`
(the 8-byte records of ids 1 to 11), and puts each of ids 1 to 11's six
worn items on with `ChrWornItemSet(id, place, byte)` (`0x80057FFC`) from
the 66 bytes at `+0x300`. It also clears the bag's item list and the
money. The resource: `0x000-0x2FF` the twelve records, `0x300-0x341` six
worn items for each of ids 1 to 11, `0x342-0x399` eleven 8-byte place and
preset records, the rest zero.

`ChrWornItemSet` (chr_item.c) stores the item at `record[0x1D + place]`
and calls `ChrWornItemValuesAdd` (`0x800587D0`) (chr_worn.c), which adds
the item row's `+0x0B` to the stat its `+0x0C >> 4` names and `+0x0A` to
the one `+0x0C & 0xF` names, through `ChrWornItemStatAdd` (`0x80058910`):
kinds 1 to 5 the u16 words `+0x08 +0x0A +0x0C +0x0E +0x10`, 6 the byte
`+0x12`, 7 the byte `+0x1D`, 8 the byte `+0x13`, 0 nothing. They are plain
adds at the field's width, with no limit. It also sets story bit
`row[+5] + 0x2BC` when that byte is not 0. Resource 0xE7 holds the stats
without the worn items, so the record's stats include them only once
they are put on: Alex's stat_0a is 16 in the resource and 21 in the
battle (Peasant Clothes 3, Hat 2).

Script commands that change the character records (the ids are the
entries of the script's table `g_ScriptCommandFunctions`; npc-scripting.md
has the commands):

* 0x22, `ScriptCommandChrLevelItemSet` (`0x8005D2F0`), sets a character's
  level, a worn item or the items, by a kind byte. For the level: a value
  below the record's level first reloads the id's record from resource
  0xE7; then `ChrLevelAdd` (`0x8002844C`) raises the level by the
  difference through the gain table `g_ChrLevelGainTable` (`0x800ACA38`)
  (§"Level-ups"), and HP and MP are set to their maxima.
* 0x23, `ScriptCommandPartyJoin` (`0x8005D60C`), has a member join the
  party, and sets HP and MP to their maxima on every join.
* 0x5C, `ScriptCommandStoryBitsItemsReset` (`0x8005EF58`), resets the
  whole party: for ids 1 to 11 every worn item is taken off, the six items
  at `+0x24..+0x29` are copied again from 0xE7 and the level-1 worn items
  put back on; the bag's item list and all the story bits are cleared. The
  level, HP, MP and spells are not touched (npc-scripting.md §"Commands
  0x23 and 0x5C").
* 0x43, `ScriptCommandChr1ChrBChange` (`0x8005D470`), switches the lead
  between ids 1 and 0xB (both named "Alex" by the string table) and copies
  the old id's whole 0x40-byte record, and its 8-byte record at
  `g_ChrPlacePresetRecords`, over the new id's (npc-scripting.md
  §"Switch the lead's form").

The character record (`ChrRecord`, chr.h): `+0x00` / `+0x02` HP and its
maximum, `+0x04` / `+0x06` MP and its maximum, `+0x08..+0x10` the five
stats (worn items counted; kinds 1 to 5), `+0x12` attack_count (kind 6),
`+0x13` walk_reach (kind 8), `+0x14..+0x1B` the same stats without the
worn items (`ChrLevelUp` (`0x80028554`) raises both; the menu shows the
two side by side), `+0x1C` level, `+0x1D` stat_1d (kind 7), `+0x1E..+0x23`
the six worn items, places 1 to 6, `+0x24..+0x29` the six items carried,
`+0x2A..+0x31` the eight spells (`ChrSpellAnyCheck` (`0x80058654`) tests
them; a learned row goes to the place `jtbl_800118DC` gives, §"Level-ups"),
`+0x38` the total experience, `+0x3C` the experience the next level still
costs.

**The item lists' order — `ChrItemListSort` (`0x80058B3C`)** (chr_worn.c).
`who == 0xC` sorts the bag, the 0x60 item bytes at `*g_ChrItemList0C`; any
other id sorts that character's six items at `+0x24`. A list whose first
two places are not both filled is left alone. Trailing empty places are
left out; the rest is parted in two by the item row's `+0x04`, its kind:
kinds below 5 (the worn items) in front and 5 and up behind, or the other
way round when the option byte `g_GameOptions[2]` is not 0. Then
`ChrItemListPartSort(list, lo, hi)` (`0x80058E18`) sorts each part by the
kind's rank in `g_ChrItemSortRanks` and, within a rank, by item number. It
is called by `ChrItemGive` (`0x800577C4`) after it stores an item (for the
id and for the bag), by `BtlWinItemsGive` (`0x80028A54`) for each id it
gave to and for the bag, and by the option step `MenuOptionStep`
(`0x80075410`).

### The way back — `BtlPartyStatsStore` (`0x80028074`)

`void BtlPartyStatsStore(u8 *party)` (btl_run.c) undoes part of the party
half. Its one call is at the end of `BtlRun`, after the rounds whatever
their result, with the copy of the five ids `BtlRun` made before the
set-up. It calls nothing:

```c
for (i = 0; i < 5; i++) {
    if (party[i] != 0) {                 /* the character id */
        rec = &g_ChrRecords[party[i]];   /* the character record */
        rec->hp = stats[i].hp != 0 ? stats[i].hp : 1;   /* +0x00 <- +0x00 */
        rec->mp = stats[i].mp;                         /* +0x04 <- +0x04 */
        for (j = 0; j < 6; j++)
            rec->items[j] = stats[i].items[j];         /* +0x24.. <- +0x2E.. */
    }
}
```

So a member who ended the fight at 0 HP goes back with 1: nobody leaves a
battle fallen. MP and the six items go back as they stand. Nothing else is
written here: the maxima, the stats, the level, the worn items and the
spells are not copied back.

### The damage chain — `BtlHitAmountSet` (`0x80030070`) and `BtlHitAmountTake` (`0x80030650`)

Two functions of btl_hit.c take a physical hit's HP off its target:
`BtlHitAmountSet` works the amount out (§"Combat math" and §"The modifier
tail" below), stores it in `g_BtlSlotAmounts[target]` and calls
`BtlHitAmountTake`, which takes it off the target's stats. Both work on
globals: the attacker `g_BtlActingSlot`, the target `g_BtlTargetSlot`.

`BtlHitAmountTake`, in order:

1. Target slot 6 or more (an enemy) and battle number `g_BtlNumber` 0xF9,
   0xFA or 0xFE: it returns, nothing written.
2. Battle 0xF7, target slot exactly 6, and slot 7's HP (`CharStats +
   0x1A4`) not 0: slot 7's HP loses the amount `g_BtlSlotAmounts[6]` (an
   amount that is not below it leaves 0, not the 1 of the normal path, and
   sets no fall mark), and the hit is handed to slot 7
   (`g_BtlSlotAmounts[7] = g_BtlSlotAmounts[6]`, `g_BtlSlotAmountKinds[7] =
   1`, `g_BtlSlotAmounts[6] = 0`, `g_BtlSlotAmountKinds[6] = 0`); it
   returns. Slot 6's own HP is not touched. Slot 7 is a second HP pool in
   front of slot 6 in that battle: slot 7's HP comes from effect 61
   (`BtlSlot7HpSetRun` (`0x80034260`): the acting slot's level × 10 +
   500), and `BtlF7Slot7RecordOff` (`0x80030F8C`) switches slot 7's record
   off when that HP is 0. Which boss stands in slots 6 and 7 is not in the
   C; [boss-ai.md](boss-ai.md) §"The second Magic Emperor, encounter 0xF7 — a two-actor boss" reads it as his shield.
3. Otherwise:

```c
t   = g_BtlTargetSlot;
dmg = g_BtlSlotAmounts[t];
hp  = stats[t].hp;               /* CharStats + t * 0x3C */
if (dmg >= hp) {
    g_BtlSlotFallMarks[t] = 1;   /* the fall mark */
    stats[t].hp = 1;             /* 1, not 0 */
    return;
}
stats[t].hp = hp - dmg;
```

The fatal branch leaves 1 HP and the fall mark; `BtlFallRun`
(`0x800385B4`) (btl_fall.c) then sets the HP to 0, clears the slot's
active flag and status, and plays the fall. In a full fight in the
running game a write watch on a Slime's HP caught exactly these two
writers: `BtlHitAmountTake` and `BtlFallRun`.

The globals of this chain:

| global | address | what it holds |
|---|---|---|
| `CharStats` | `0x800B8C20` | the address of the 14 stats records at `0x801D2B00` |
| `g_BtlSlotAmounts` | `0x800E48C0` | 14 u16 words: the amount an action takes from or gives to each slot, shown over the slot |
| `g_BtlSlotAmountKinds` | `0x800E48F0` | 14 bytes: how each amount is shown (0 nothing, 1 HP taken, 2 a miss, 3 doubled, 4 halved, 5 something given, 6 stopped by party flag 0x02, 7 a status took, 8 and 9 a raise); before an action is worked out the same bytes mark the slots it falls on |
| `g_BtlSlotFallMarks` | `0x800E4960` | 14 bytes: 1 when the slot's HP has run out and it is to be taken out |
| `g_BtlTargetSlot` | `0x800B8C5D` | the target slot |
| `g_BtlActingSlot` | `0x800B8C26` | the acting slot, whose turn it is |
| `g_BtlActionKind` | `0x800B8C66` | the action's kind, which picks its row's table: 2 and 7 the rows of `g_BtlAttackRows`, 3 the spell rows, 4 the item rows; 5 and 0xB have no row |
| `g_BtlActionNumber` | `0x800B8C67` | the action's number within its kind |
| `g_BtlNumber` | `0x800B8C79` | the battle's number, `BtlRun`'s argument |

## Combat math — the damage formula

The head of `BtlHitAmountSet` (btl_hit.c), with `A` the attacker's stats
and `D` the target's:

```c
roll = rng_mask_below(0x7F, A->stat_08 / 10 + 2);  /* drawn first */
dmg  = A->stat_08;
dmg += dmg * g_BtlActionPower / 100;               /* the percent bonus */
if (BtlHitStrongTry())                             /* a critical hit, below */
    dmg = ratio_13_10_mul(dmg);                    /* dmg * 13 / 10 */
t = dmg + roll - D->stat_0a;
if (A->level > D->level)                           /* a level advantage only */
    t += (A->level - D->level) * 5;
t /= 2;                                            /* signed, toward zero */
if (t < A->least_amount) t = A->least_amount;      /* the attacker's floor */
/* then the modifier tail, below */
```

`rng_mask_below` (`0x8003A0D4`) is one RNG draw brought below its second
argument (no draw, and 0, when that is below 2). `BtlHitStrongTry`
(`0x80038310`) is called after the roll and before the defence: its ×1.3
applies to the attack term alone. The floor at `+0x16` is the attacker's,
not the defender's.

The formula was checked in the running game: an execution breakpoint on
the `jal` whose delay slot stores the amount (`0x80030628`) read the
amount and both slots' stats at every hit while the fight ran. In an
unchanged fight every implied roll falls inside its window:

| attacker→defender | atk | def | predicted | observed | implied roll | window |
|---|---|---|---|---|---|---|
| 2→9 | 36 | 6 | (30+r)/2 | 16 | 2-3 | 5 |
| 2→8 | 36 | 6 | (30+r)/2 | 15 | 0-1 | 5 |
| 0→7 | 20 | 6 | (14+r)/2 | 8 | 2-3 | 4 |
| 0→7 | 20 | 6 | (14+r)/2 | 7 | 0-1 | 4 |
| 7→0 | 35 | 21 | (14+r)/2 | 7 | 0-1 | 5 |
| 8→2 | 35 | 25 | (10+r)/2 | 5 | 0-1 | 5 |

Values then written into the stats in the running game, and the fight run
again:

| written | prediction | observed | what it shows |
|---|---|---|---|
| Alex `+0x16` = 50 | the attacker's floor: Alex deals 50; the defender's: a Slime hitting Alex deals 50 | Alex dealt 50, a Slime hit Alex for 8 | the floor is the attacker's |
| the same, with Alex's spell row 1 (kind 3) | 100 if a ×2 comes after the floor | 100 | a ×2 after the floor: the target's weakness (reaction 3), §"The modifier tail" |
| Alex `+0x08` = 100 | (100+r−50)/2 = 25-31 against Slime 7 | 26 | the attack term |
| Slime 7 `+0x0A` = 50 | that Slime alone resists; Slime 6 (6) does not | 26 against 198 in one fight | the subtracted stat is the defender's |
| Alex `+0x14` = 10 | (59+r)/2 = 29-32 | 31 (7-8 before) | the level term, and `+0x14` is the level |

Every slot in that battle is level 1, so the level term was seen only with
a written value. None of the six hits of the first table is a ×1.3 value
(Ramus, stat_08 36 against 6: plain 15-17, critical 20-22; observed 15 and
16), which fits six draws at 5 to 10 in 128.

## The critical hit — `BtlHitStrongTry` (`0x80038310`)

`BtlHitStrongTry` (btl_fall.c) decides whether a physical swing is a
critical hit (the C's word is "strong"): the ×1.3 of `ratio_13_10_mul`
(`0x8002FE54`) = `ratio_mul(x, 13, 10)` (`0x8002FE10`) = `(s32)(x * 13) /
10`.

```c
s32 BtlHitStrongTry(void) {
    a = g_BtlActingSlot; t = g_BtlTargetSlot;
    if (g_BtlActionKind == 2) {
        if (a >= 5 || t >= 5) {              /* not party -> party */
            if (a >= 6 && t >= 6) return 0;  /* not enemy -> enemy */
            if (a < 5) n = (g_BtlSlotIds[a] == 2 || g_BtlSlotIds[a] == 3) ? 50 : 10;
            else       n = 5;
            if ((u8)rng_7f_draw() < n) goto strong;
            n = (a < 5) ? stats[a].side.party.traits[0]   /* +0x28 */
                        : stats[a].side.enemy.trait;      /* +0x1B */
            switch (n) {
            case 7:  rate = 0x20; break;
            case 14: rate = 0x40; break;
            case 21: goto strong;
            default: return 0;
            }
            if (rate != 0) { n = rng_7f_draw(); if (n < rate) goto strong; }
        }
    }
    return 0;
strong:
    g_BtlSlotStrongMarks[t] = 1;
    return 1;
}
```

`rng_7f_draw` (`0x8003A0B4`) is the low seven bits of one RNG draw.

* The base rates, out of 128: a party slot whose id is 2 or 3 (the
  character's id, not the slot) 50; any other party slot 10; any slot of 5
  or more 5. §"Caster groups" ties id 2 to Nash; id 3 is not named in the
  C.
* The attacker's trait gives a second chance: a party slot's first trait
  `+0x28` (byte `+0x0D` of the first worn item's row, the byte
  `BtlHitAmountSet` also reads for the hit's element) or an enemy's trait
  `+0x1B`. 21 is always critical, 14 draws a second roll below 64, 7 a
  second roll below 32. The second roll is drawn only when the first
  failed. The `rate != 0` test is never false (the switch reaches it only
  with 0x20 or 0x40), but it is in the bytes.
* Draws per call: none when the kind is not 2 or both slots are on one
  side (both below 5, or both 6 or more); one on every other call; two only
  when the first failed and the trait is 7 or 14. Trait 21 is critical
  without a second draw, but the first was still taken. So every attack
  from one side on the other draws at least one number here.
* Slot 5 can strike anyone: the two guards are `< 5` and `>= 6`. Nothing
  of the target's record is read, only its slot.
* `g_BtlSlotStrongMarks` (`0x800E49F0`): set here on a critical hit and not
  cleared on a miss. `BtlAttackOutcomeClear` (`0x80031878`) clears it for
  every slot before a swing, and `BtlAttackAllRun` (`0x800318DC`) for one
  target. `BtlAmountShowStart` (`0x80088EA8`) shows a marked slot's hit
  with 0x710 and animation 1 instead of 0x700 and 0. `BtlAttackHitShow`
  (`0x800314B8`) tests the table's address, not the target's byte, in
  `SoundPlay(g_BtlSlotStrongMarks ? 0x38 : 0x23, 3)` (btl_attack.c), so it
  always plays sound 0x38.

## The modifier tail of `BtlHitAmountSet` (`0x80030070`)

Between the attacker's floor and the store to `g_BtlSlotAmounts` the
amount goes through these steps, in order (btl_hit.c):

```c
/* 1. the element. Kind 2, a party (or slot 5) attacker on an enemy only: */
if (g_BtlActionKind == 2) {
    if (a < 6 && t >= 6) {
        switch (stats[a].side.party.traits[0]) {          /* +0x28 */
        case 0x2A: r = 1;   case 0x2B: r = 2;   case 0x2C: r = 4;
        case 0x2D: r = 8;   case 0x2E: r = 3;   case 0x2F: r = 0xC;
        }   /* any other trait: r = 0 */
        if (r & stats[t].side.enemy.weak_mask)   { kinds[t] = 3; dmg *= 2; }  /* +0x28 */
        if (r & stats[t].side.enemy.resist_mask) { kinds[t] = 4; dmg /= 2; }  /* +0x2A */
    }
} else {          /* any other kind: the action's bit number */
    switch (g_BtlActionBitNumber) {
    case 1: r = 0x01; e1 = 0x2A; e2 = 0x2E;   case 2: r = 0x02; e1 = 0x2B; e2 = 0x2E;
    case 3: r = 0x04; e1 = 0x2C; e2 = 0x2F;   case 4: r = 0x08; e1 = 0x2D; e2 = 0x30;
    case 5: r = 0x10;   case 6: r = 0x20;   case 7: r = 0x40;
    }
    if (r != 0)
        switch (BtlTargetTraitCheck(r, e1, e2, 0)) {
        case 1: dmg /= 2; kinds[t] = 4; break;               /* resisted */
        case 2: dmg *= 2; kinds[t] = 3; break;               /* weak     */
        }
}
/* 2. the status: a target whose status has a bit of 0x0C takes double */
if (stats[t].status & 0xC) dmg *= 2;
/* 3. kind 3: the spell row's percentage, attack_count - 1 times */
if (g_BtlActionKind == 3) {
    pct = row 1 or 29: 130;  30 or 31: 120;  2, 3 or 47: 110;  any other: 100;
    for (i = 0; i < (u8)(stats[a].attack_count - 1); i++) dmg = dmg * pct / 100;
}
/* 4. defend: a party target resting on animation 0x13 */
if (t < 5 && g_BtlSlotRestAnimations[t] == 0x13) dmg /= 2;
/* 5. the limits */
if (dmg == 0) dmg = 1;
if (dmg > 1000) dmg = 999;
/* 6. made 0, by kind */
kind 2: if (BtlTargetTraitCheck(0, 0x33, 0x34, 0) == 1) dmg = 0;
kind 3: if (BtlTargetTraitCheck(0, 0x34, 0, 0) == 1)    dmg = 0;
kind 7: if (BtlTargetTraitCheck(0, 0x33, 0, 0) == 1)    dmg = 0;
if (g_BtlNumber == 0xFA) dmg = 0;
g_BtlSlotAmounts[t] = dmg;
BtlHitAmountTake();
```

`BtlTargetTraitCheck(mask, n1, n2, n3)` (`0x80037648`) (btl_slot.c)
answers for the target: for a party slot, 1 when one of its six traits
(`+0x28..+0x2D`) is one of the numbers, the mask unused; for a slot of 5
or more, 1 when its one trait `+0x1B` is one of the numbers, else, with a
mask, 1 when the resist mask `+0x2A` has a bit of it and 2 when the weak
mask `+0x28` has; otherwise 0 (§"The element system").

* The amount is an unsigned 32-bit word from step 1 on: the halvings are
  logical shifts, the percentage product is divided unsigned, and the 999
  clamp is an unsigned compare, so a negative amount (possible only with a
  floor of 0) becomes 999. An amount of exactly 1000 is kept.
* The loop count of step 3 is `(u8)(attack_count − 1)`: an attack_count of
  0 runs it 255 times.
* For bit numbers 5 to 7 the C assigns no trait numbers: `e1` and `e2` are
  passed as they were left. Only the mask decides those for an enemy
  target.
* The nullify test exists for kinds 2, 3 and 7 only; every other kind
  skips the call.

The ×2 seen with Alex's spell row 1 (kind 3) is step 1's weakness. Captured
in the running game for that row: amount 34, attacker 0, target 6, kind 3,
row 1, bit number 5, reaction (`g_BtlSlotAmountKinds[target]`) 3. With
stat_08 20 and a bonus of 100 % the base is 40, `t = 34 + roll`, halved to
17, doubled by the weakness to 34. The same branch, after the floor, gives
the written floor of 50 as 100. Step 3's percentage did not show there
because Alex's attack_count is 1 and the loop runs no times.

| field or global | what it is |
|---|---|
| stats `+0x28` (party) | the first trait, the hit's element for kind 2 |
| stats `+0x28` / `+0x2A` (enemy) | the weak and resist masks |
| stats `+0x15` bits 0x0C | statuses 4 and 8; a target in either takes double |
| stats `+0x12` | attack_count; for kind 3, one more than the number of times the percentage applies |
| `g_BtlActionBitNumber` (`0x800B8C10`) | the action's bit number: 1 to 7 stand for the bits 1 to 0x40 of the masks |
| `g_BtlActionNumber` | for kind 3 the spell row, which picks the percentage |
| `g_BtlSlotAmountKinds[t]` | 3 doubled (weak), 4 halved (resisted) |
| `g_BtlSlotRestAnimations[t] == 0x13` | the party slot's defend stance |

### Where actions come from

`g_BtlActionKind` and `g_BtlActionNumber` are set for each turn by
`BtlTurnOrderRun` (`0x8002DC30`) (btl_action.c) from the acting slot's
plan (§"The action loop"). Two more functions write them:
`BtlActionWalkCornerSet` (`0x80039B70`) (btl_pick.c), which makes the
acting slot's action a walk to a corner of the field, and
`BtlPartyTurnActionSet` (`0x8004F734`) (btl_choice.c). The second is the
party's own choice: at a party slot's turn whose planned target is 0xFE,
the target `BtlPartyActionSet` stores with every choice, it makes the
choice again and copies it to the turn:

```c
g_BtlActionNumber = g_BtlPartyChoiceNumber;
g_BtlActionKind   = g_BtlPartyChoiceKind;
```

`g_BtlPartyChoiceKind` (`0x800B8DAA`), `g_BtlPartyChoiceNumber`
(`0x800B8DAB`) and `g_BtlPartyChoiceTarget` are the party's choice, which
the functions of btl_choice.c work out for the acting party slot:
`BtlPartyActionSet` (`0x8004D2C4`), `BtlPartyOneChoiceSet` (`0x8004D61C`),
`BtlPartyAllChoiceSet` (`0x8004DB30`), `BtlPartyNeedChoiceSet`
(`0x8004DFA8`), `BtlPartyNeedSpellSet` (`0x8004EAE0`),
`BtlPartyOtherSideSpellSet` (`0x8004ECF4`), `BtlPartyNeedItemSet`
(`0x8004F2B4`), `BtlPartyAttackChoiceSet` (`0x8004F494`),
`BtlPartyHpShortSpellSet` (`0x8004FD70`), `BtlPartyWornItemSet`
(`0x80050574`). The kinds they store: 2 an attack, 3 a spell, 4 an item, 5
none (number and target 0xFF). §"The party's auto-battle AI" reads them.

`BtlEnemyRowsMostFind` (`0x8004FFA0`) (btl_choice.c) is one of their
aiming helpers: for each enemy slot 6 to 13 that has HP it clears the 14
bytes of `g_BtlSlotAmountKinds`, runs `BtlTargetRowsMark` (`0x80035260`)
with `g_BtlTargetSlot` = that slot, and counts the enemy slots marked. It
returns the best count and leaves the best slot in `g_BtlEnemyMostSlot`
(the count in `g_BtlEnemyMostCount`; of two equal slots the lower is
kept), then clears the bytes and `g_BtlTargetSlot`. Its caller
`BtlPartySpellOfThreeGet` (`0x8005022C`) compares it with
`BtlActingAreaEnemyCount` (`0x8004FEBC`) when the acting party member is id
5: the party's own choice of where an area spell catches most.

### The battle menu

`BtlMenuRun` (`0x80044118`) (btl_menu.c) asks the player for the party's
commands, once a round, through the steps of the table at `0x800B312C`,
called by the step number `g_BtlMenuStep`. The first menu
(`BtlMenuFirstOpen` (`0x80044780`)) has four items in two rows of two, on
the screen AI and Command over Tactics and Run, and opens on the item it
was on the last time (`g_BtlMenuKeptCursors`). `BtlMenuFirstStep`
(`0x80044B5C`) acts on the confirm button by the cursor:

* 0, AI: `g_BtlPartyMode` = 1 and `BtlPartyActionSet(1)`; from then on
  `BtlRun` calls `BtlPartyActionSet(1)` in place of the menu, so the party
  plays itself for the rest of the battle. In the `battle` state the cursor
  is on this item, and one press of the button plays the whole fight.
* 1, Command: the kind menu, one member after another.
* 2, Tactics: the presets (`BtlMenuPresetOpen`).
* 3, Run: `g_BtlPartyMode` = 2, and `BtlRun` calls `BtlPartyEscapeTry`.

The kind menu (`BtlMenuKindOpen` (`0x800459D4`), btl_command.c) has six
items in two rows of three, on the screen AI, Magic and Defend over Attack,
Item and Run; in the running game its cursor starts on AI for every member.
The small window at the top left names the item under the cursor. Attack
opens a target window (`<Enemy> x/N`); Magic the spell list and then a
target; Defend needs no target. Nothing happens until the last member's
command is given; then the round runs, and the first menu comes back.
menu-ui.md §"The battle menu" reads these steps in full.

## Worn items — what they add, and how they come off

### The item row's worn-item bytes

The item table `g_BtlItemTable` (`0x800A9A68`) has 0xB6 (182) rows of 0x12
bytes (`BtlItemRow`, btl.h), one per item number from 1. Two words point at
it, `g_BtlItemRows` (`0x800B863C`) for the battle and `g_ChrItemRows`
(`0x800B8B94`) for the menus and the Chr functions; `g_BtlItemRowsByNumber`
(`0x800A9A56`) is the address 0x12 bytes before it, from which number ×
0x12 reaches the same row. `ChrItemUseGet` (`0x80058088`) answers -2 for a
number of 0 or above 0xB6, which gives the count. The table is data in the
executable and is never written, so every row is the same in every state of
the game.

| offset | field (btl.h) | what reads it |
|---|---|---|
| `+0x00` | price | `ShopDealRun` (`0x80053194`) takes it off the money |
| `+0x02` | sell price | 0 for an item that cannot be given up (`ChrItemUseGet` question 2) |
| `+0x04` | kind | below 5 a worn item, and the place it goes: `rec[0x1E + kind]`, places 1 to 6 at `+0x1E..+0x23` of the character record (kind 4 has two places, `+0x22` and `+0x23`); 5 and up a carried item (6: one that cannot be given up). `ChrItemListSort` parts the lists by the same `< 5` |
| `+0x05` | story bit | not 0: `ChrWornItemValuesAdd` (`0x800587D0`) sets story bit `+0x05 + 0x2BC` when the item is put on, `ChrWornItemValuesRemove` (`0x8005886C`) clears it when it comes off |
| `+0x06` | target modes | low four bits the battle's, high four bits the field menu's |
| `+0x07` | use picture | low four bits the entry of `MenuUseRun`'s table, high four bits the picture of entry 4 (§"Not the battle") |
| `+0x08` | function number | `BtlActionRowRun` (§"The row runner"); 0 for an item a battle cannot use, which `BtlMenuActionCheck` refuses |
| `+0x09` | show number | `BtlActionRowRun` |
| `+0x0A` | bonus 1, or the power | the first value a worn item adds; the power of a used item from number 0x9C |
| `+0x0B` | bonus 2 | the second value; for the first worn item also the slot's `least_amount` (`BtlPartyStatsSet`) |
| `+0x0C` | bonus stats | two kinds 1 to 8: low four bits bonus 1's, high four bits bonus 2's |
| `+0x0D` | trait | copied to the slot's traits `+0x28..+0x2D` |
| `+0x0E` | attack show | the first worn item's goes to the slot's `attack_show`, which nothing reads for a party slot |
| `+0x0F` | worn item kind | the kind of a weapon (0 to 5; 0 in every other row): the first worn item's picks the slot's attack, stand and move animations and its attack sound |
| `+0x10` | wearers | u16: bit `id − 1` set for each character who may wear it |

By what each kind holds, kind 0 are the weapons (swords, maces, bows,
canes, the sling), 1 body armour, clothes and robes, 2 shields, bracelets
and armlets, 3 headgear (scarves, hats, helmets, bandannas, hairpins,
tiaras) and 4 the accessories (rings, pendants, tails, bromides, Dragon
Wings, Dragon Ring). Five items have a story bit: 17 Althena's Sword (19),
56 Dragon Armor (18), 82 Dragon Shield (16), 110 Dragon Helmet (17) and 144
Dragon Wings (15). The items whose wearer mask is 0 (the spare weapon,
armour, shield, hairpin and tools) fit nobody.

### Who may wear it, and putting it on

`ChrItemUseGet(id, number, 1)` answers yes when bit `id − 1` of `+0x10` is
set; `ShopItemDisp` (`0x80052C0C`) marks an item the member cannot wear
(flag 0x01) when its kind is below 5 and that bit is clear. All eleven
characters' level-1 worn items in resource 0xE7 pass this test at their
places.

* `ChrWornItemSet(id, place, item)` (`0x80057FFC`) (chr_item.c) stores the
  item at `record[0x1D + place]` and adds its values
  (`ChrWornItemValuesAdd`).
* `ChrWornItemRemove(id, place)` (`0x80058038`) (chr_worn.c) reads and
  clears the byte, takes the values off (`ChrWornItemValuesRemove`, the
  same adds negated) and returns the item; for an empty place it does
  nothing and returns 0.
* `ChrWornItemStatAdd` (`0x80058910`) is the add, by kind: 1 to 5 the u16
  words `+0x08 +0x0A +0x0C +0x0E +0x10`, 6 the byte `+0x12`, 7 the byte
  `+0x1D`, 8 the byte `+0x13`. It is a plain add at the field's width, so a
  value wraps; nothing holds it to a limit.
* The shop's deal (`ShopDealRun`) takes the old item off into the member's
  items (`ChrItemGive` (`0x800577C4`)) and puts the new one on; for a kind
  below 4 it makes the two calls even when the place is empty. For kind 4
  it keeps the two places in order: with both full the item at `+0x22`
  comes off, `+0x23`'s moves down to `+0x22` and the new one goes to
  `+0x23`.
* The field menu's change (`MenuWornItemChange` (`0x8006F69C`), menu_page.c)
  does the same: a place below 6 is taken off into the items; an item is
  put on at `kind + 1`, and for kind 4 the older of the two worn items
  comes off and the new one goes to the second place, as in the shop.

The character record's stats include the worn items: `ChrRecordsReset`
puts resource 0xE7's worn items on through `ChrWornItemSet`, and
`BtlPartyStatsSet` copies the stats as they stand. Alex's stat_0a of 21 is
16 from the resource plus 3 (Peasant Clothes) and 2 (Hat). `ChrLevelUp`
raises the record's stats, so a level's gain is added on top of what is
worn.

The shop and the menu show the change a worn item makes as rows of a kind
and a number (`ChrWornItemChangeMake`, `ShopItemStatsDisp`): the kind's
text is the one whose first block's code `g_ShopKindTextCodes`
(`0x800B32C8`) holds, eight codes for kinds 1 to 8. The words those blocks
show are not in the C.

### Taking a worn item off in a battle — `BtlWornItemStatLower` (`0x800389C4`) and `BtlWornItemRemove` (`0x80038BFC`)

`BtlWornItemStatLower(record, slot, kind, value)` (btl_fall.c) is the only
function that lowers the stats in a battle: by the kind (1 to 8) it takes
the value off the field of the character record and the same field of the
slot's stats.

| kind | character record | stats | kind | character record | stats |
|---|---|---|---|---|---|
| 1 | `+0x08` | `+0x08` | 5 | `+0x10` | `+0x10` |
| 2 | `+0x0A` | `+0x0A` | 6 | `+0x12` (u8) | `+0x12` |
| 3 | `+0x0C` | `+0x0C` | 7 | `+0x1D` stat_1d | `+0x1F` stat_1f |
| 4 | `+0x0E` | `+0x0E` | 8 | `+0x13` (u8) | `+0x13` |

Kind 7 is one value in two places: `BtlPartyStatsSet` copies the record's
`+0x1D` to the stats' `+0x1F`.

`BtlWornItemRemove(slot, place)` takes off worn item `place` (4 or 5) of a
party slot:

```c
party = &g_ChrRecords[g_BtlSlotIds[slot]];          /* the character record */
item  = &g_BtlItemRows[stats[slot].side.party.worn_items[place] - 1];
if (item->bonus_stats % 16) BtlWornItemStatLower(party, slot, item->bonus_stats % 16, item->bonus_1);
if (item->bonus_stats / 16) BtlWornItemStatLower(party, slot, item->bonus_stats / 16, item->bonus_2);
if (place == 4) party->worn_items[4] = 0; else party->worn_items[5] = 0;
stats[slot].side.party.worn_items[place] = 0;
```

Its one caller is `BtlItem83Revive` (`0x80038868`) (btl_fall.c), the
auto-revive: for each party slot that fell while it wore item 0x83 at
`+0x26` or `+0x27` (worn places 5 and 6) the item revives the slot
(`BtlTargetReviveRun`, with kind 4 and number 0x83, which also fills the
MP) and is taken off and used up. Nothing is done in battle 0xF9, or in
battle 0xFE once `g_BtlSpecialBytes[0]` is 5 or more. `g_BtlSlotIds` is the
link between the stats and the character records here: the slot's id is
the character record's index.

## Magic — the spell table and the heal

### The spell rows — `g_BtlSpellRows` (`0x800AA734`)

The spells are 12-byte rows (`BtlSpellRow`, btl.h) in the executable's
data, never written. Spell number n is row `n − 1`; the battle reaches it
as `g_BtlSpellRowsByNumber + n * 12` (`0x800AA728`, the address one row
before the table), the menus through `g_ChrSpellRows`.

| offset | field (btl.h) | what reads it |
|---|---|---|
| `+0x00` | level | the level the spell is learned at (`ChrLevelUp`) |
| `+0x01` | target modes | low four bits the battle's, high four bits the field menu's |
| `+0x02` | function number | → `g_BtlActionFunctionNumber` |
| `+0x03` | use place | low four bits `MenuUseRun`'s entry; high four bits the place among the character's eight spells (`ScriptCommandChrSpellSet`) |
| `+0x04` | cost | the MP the spell costs |
| `+0x05` | power | → `g_BtlActionPower` |
| `+0x06` | show number | → `g_BtlActionShowNumber`, the show that is played |
| `+0x07` | chr_id | the character the spell belongs to (§"Caster groups") |
| `+0x08` | bit number | → `g_BtlActionBitNumber` |
| `+0x09` / `+0x0A` | area half width / height | in cells of 8 pixels → `g_BtlAreaHalfWidth` / `g_BtlAreaHalfHeight` |
| `+0x0B` | blocked_by_status_20 | not 0: the spell cannot be cast while the caster's status has bit 0x20 |

`BtlActionSpellRun` (`0x8002E718`) (btl_action.c) is the cast: it runs the
row (`BtlActionRowRun`) first and then takes the cost off the caster's MP,
halved and rounded up when the caster has trait 0x1B:

```c
ptr = g_BtlSpellRowsByNumber + g_BtlActionNumber * 12;
BtlActionRowRun();
g_BtlTargetSlot = g_BtlActingSlot;
if (BtlTargetTraitCheck(0, 0x1B, 0, 0) == 1)
    stats[g_BtlActingSlot].mp -= (ptr[4] + 1) / 2;
else
    stats[g_BtlActingSlot].mp -= ptr[4];
```

Healing Song is spell 33. Its row, read in the running game when Luna cast
it, is `01 55 03 01 04 32 24 06 00 00 00 00`: level 1, target modes 0x55,
function number 3, cost 4, power 50, show 0x24, character 6 (Luna). Her MP
went 40 → 36 at her action in the round, not when the command was given.

### The heal — `BtlTargetHpShareAddRun` (`0x80031ED4`)

Function number 3 is `BtlTargetHpShareAddRun` (btl_row.c), the one-target
heal. `g_BtlTargetStats` (`0x800B8CB0`) points at the target's HP (§"The
row runner"):

```c
g_BtlActionPower   = target->hp_max * 4 / 10;            /* 40 % of the maximum */
g_BtlAreaHalfWidth = rng_mask_below(0xFFFF, target->hp_max / 20);
if (g_BtlActionPower < 40) g_BtlActionPower = g_BtlAreaHalfWidth + 40;
else                       g_BtlActionPower += g_BtlAreaHalfWidth;
target->hp += g_BtlActionPower;
if (target->hp >= target->hp_max) {
    g_BtlActionPower -= target->hp - target->hp_max;     /* what landed */
    target->hp = target->hp_max;
}
g_BtlSlotAmounts[g_BtlTargetSlot] = g_BtlActionPower;
BtlTargetAddShowRun();
```

* The amount is at least 40, so a member whose maximum is 40 or less is
  always healed to full. Below a maximum of 40 the roll's bound is under 2
  and `rng_mask_below` draws nothing.
* `g_BtlSlotAmounts` holds what landed: heals and hits use the same 14
  words, and the number shown over the slot is read from them.
* `g_BtlActionPower` is the row's power on the way in and the heal's amount
  on the way out; what the row put there is not read. The damage path reads
  it as a percent bonus (§"Combat math").
* Seen in the running game with Alex at 5 HP of 32: Healing Song wrote his
  HP first (to 32), then `BtlHitAmountTake` wrote it three times as three
  Slimes hit him, leaving 8. A pair of values before and after a round in
  which both sides act does not give a heal's amount.

### The row runner — `BtlActionRowRun` (`0x80031ABC`)

`BtlActionRowRun` (btl_row.c) is the one place every action's row goes
through. It loads the row's numbers into one set of globals, and calls the
row's function from the table of function addresses at `0x800B2EF8`
(`scene_state_table` in the C, a label that came with the first split and
was never read: the name says nothing about the table). The register's
reading is a run of 132 function addresses; entry `number − 1` is the
function of function number `number`.

```c
if (g_BtlRunning == 0) {                       /* the field menu's use */
    for (i = 0; i < 0xE; i++) g_BtlSlotAmounts[i] = 0;
    if (g_BtlTargetSlot >= 0xD) g_BtlTargetSlot = 1;
    g_BtlTargetStats = g_ChrRecords + (g_BtlTargetSlot << 6);   /* a character record */
    g_BtlTargetSlot--;
} else {
    g_BtlTargetStats = CharStats + g_BtlTargetSlot * 0x3C;     /* the slot's stats */
}
switch (g_BtlActionKind) {
case 2:                                        /* an attack */
    g_BtlActionNumber = stats[g_BtlActingSlot].attack_number;  /* byte +0x1C */
case 7:                                        /* an enemy's special */
    row = (kind == 7 && g_BtlSlotIds[acting] == 0x4F) ? stats[acting] byte +0x1D
                                                      : g_BtlActionNumber;
    p = g_BtlAttackRows + row * 8;
    function = p[1]; power = p[2]; half_width = p[3];
    half_height = p[4]; bit_number = p[5]; show = p[6];
    break;
case 3:                                        /* a spell */
    p = g_BtlSpellRowsByNumber + g_BtlActionNumber * 12;
    function = p[2]; power = p[5]; half_width = p[9];
    half_height = p[0xA]; show = p[6]; bit_number = p[8];
    break;
case 4:                                        /* an item */
    if (g_BtlActionNumber < 0x9C) { p = *g_BtlItemRows + (number - 1) * 0x12; function = p[8]; }
    else { p = g_BtlItemRowsByNumber + number * 0x12; function = p[8]; power = p[0xA]; }
    show = p[9];
    break;
}
if (g_BtlActionFunctionNumber < 0x5F)
    scene_state_table[g_BtlActionFunctionNumber - 1]();
BtlF7Slot7RecordOff();
```

* The function number is 1-based. Healing Song's row has function number
  3, and entry 2 of the table is `BtlTargetHpShareAddRun`, the function
  that wrote the target's HP in the running game. The 48 spell rows use 31
  function numbers, the highest 94; the call is made only below 0x5F.
* `g_BtlAttackRows` (`0x800AC640`, 0x7F rows of 8 bytes) are the rows of
  kinds 2 and 7. For kind 2 the row is the acting slot's attack number,
  stats byte `+0x1C`, which an enemy's row gives. `BtlActionKindRun` runs
  kind 2 through this function only for a slot of 6 or more whose `+0x1C`
  is not 0; every other attack is `BtlActionAttackRun` (`0x80031060`)
  (§"The plain physical attack", §"Enemy specials").
* The two item spellings reach the same row:
  `0x800A9A56 + n * 0x12 == 0x800A9A68 + (n − 1) * 0x12`. Only items from
  0x9C on load the power from `+0x0A`; below 0x9C that byte is a worn
  item's first value. Items 199, 204, 209 and 220 have function number 3,
  the heal.
* Outside a battle `g_BtlTargetStats` points at a character record, whose
  first four words are HP, maximum HP, MP and maximum MP as in the stats,
  so the HP and MP functions work on either.

| global | address | from a spell row | what it is |
|---|---|---|---|
| `g_BtlActionFunctionNumber` | `0x800B8C18` | `+0x02` | the function number |
| `g_BtlActionPower` | `0x800B8C2C` | `+0x05` | the power; a function may overwrite it with the amount it gave |
| `g_BtlActionShowNumber` | `0x800B8C0F` | `+0x06` | the show `BtlShowNumberRun` (`0x8007BB80`) plays |
| `g_BtlActionBitNumber` | `0x800B8C10` | `+0x08` | the bit number: 1 to 7 a bit of the weak and resist masks for an amount, 9 to 14 for a status |
| `g_BtlAreaHalfWidth` | `0x800B8C2E` | `+0x09` | the area's half width in cells; not 0, the target must be within reach |
| `g_BtlAreaHalfHeight` | `0x800B8C30` | `+0x0A` | the area's half height in cells; not 0, a hit may miss |

`BtlTurnOrderRun` zeroes all of them before each turn.

### Function 17 — `BtlTargetHitRun` (`0x80032958`), the hit by spell or attack

Spell row 1 (Alex's first spell) has function number 17,
`BtlTargetHitRun` (btl_target.c):

```c
if (g_BtlAreaHalfWidth == 0 || BtlTargetReachCheck()) {
    g_BtlSlotAmountKinds[g_BtlTargetSlot] = 1;
    if (g_BtlAreaHalfHeight != 0) BtlHitTry();          /* hit or miss */
    if (g_BtlSlotAmountKinds[g_BtlTargetSlot] == 1)
        BtlHitAmountSet();                               /* the amount, above */
    BtlShowNumberRun(g_BtlActionShowNumber);
    BtlHitSlotsUpdate();                                 /* the pass after the hit */
}
```

So such a spell is the attack's amount worked out with the spell row's
numbers, on one target. `BtlTargetReachCheck` (`0x800379AC`) (btl_slot.c)
measures the walk to the target and, when the target is out of reach,
walks the acting slot as near as it gets and answers 0. `BtlHitTry`
(`0x8002FF0C`) decides between a hit and a miss (§"The evasion
check"). `BtlHitSlotsUpdate` (`0x80030800`) (btl_hit.c) follows every hit:
for a party attacker whose first trait `+0x28` is 6, 0xD or 0x14 it draws
`rng_7f_draw` for each slot hit and marks the slot to fall on a draw below
0x20, 0x40 or 0x80, then runs `BtlFallRun` and nothing more; in every other
case it runs `BtlFallRun`, then `BtlHitStatusUpdate` (`0x80030A04`) for each
slot hit that is still active, which draws, then the slots' rest
animations. It looks at slots 0 to 13, or 0 to 4 in a battle numbered 0xEB
or more (§"The post-hit pass"). Spell rows 1 and 29 both have half
width and half height 0, so a spell of function 17 is never missed; the
attack rows of function 17 with `+0x04` = 1 (rows 1, 15, 20, 21, 24, 37)
can be.

### More effect handlers: the other functions of the table

The functions are mostly short: they run `g_BtlTargetSlot` over the
slots, act on those whose `g_BtlSlotAmountKinds` byte is set, and call
shared helpers. The rows column gives the spell rows (and items or attack
rows) that name each one.

| number | function | rows | what it does |
|---|---|---|---|
| 1 | `BtlTargetMpAddRun` (`0x80031E00`) | the MP items | `g_BtlTargetStats`'s `+0x04 += power + rng_mask_below(7, 8)` (a draw of 0 to 7), held to `+0x06`; the amount that landed into `g_BtlActionPower` and `g_BtlSlotAmounts[t]`, then `BtlTargetAddShowRun` (`0x80032084`). It is `BtlTargetHpAddRun` on `+4 / +6` in place of `+0 / +2` |
| 3 | `BtlTargetHpShareAddRun` (`0x80031ED4`) | 33 ("Healing Song"); items 199, 204, 209, 220 | the one-target heal above |
| 4 | `BtlTargetHpAddRun` (`0x80031FB4`) | items 156, 157, 239 (power 50, 150, 11) | a fixed heal: `+0x00 += power + rng_mask_below(7, 8)`, held to `+0x02`; the amount that landed to `g_BtlSlotAmounts[t]` and `g_BtlActionPower`; `BtlTargetAddShowRun` sets number kind 5 and plays the item's show. No spell row names it |
| 6 | `BtlTargetHpFillRun` (`0x80032038`) | 16 | `g_BtlTargetStats`'s `+0x00 = +0x02`, the amount to `g_BtlSlotAmounts[t]` |
| 7 | `BtlPartyHpShareAddRun` (`0x800320C8`) | | the heal of every marked party slot (§"Effect 7") |
| 8 | `BtlPartyFlag1SetRun` (`0x800322B8`) | 15 | every active party slot 0 to 4: `+0x1E |= 0x01`, number kind 1; then the show. `BtlRoundStatusUpdate` gives such a slot 5 hundredths of its maximum HP back each round |
| 11 | `BtlTargetStatusAllClearRun` (`0x800323B8`) | 18, 34 | the target made active, for a party slot `+0x1E &= ~0x04`, then `BtlTargetStatusClearRun(0xFF)` (`0x80032440`): the whole status byte cleared, the show, the record's animation again. No HP given. It is what makes a slot turned to stone (status 0x30, inactive) active again |
| 12 | `BtlTargetReviveRun` (`0x800324A4`) | 17 | only when the target's active byte is 0: status 0, active, HP to its maximum, `+0x08` / `+0x0A` from `+0x1A` / `+0x1C`; with kind 4 and number 0x83 the MP filled too. The show is played either way. It sets no number kind |
| 13 | `BtlPartyFlag2SetRun` (`0x800325AC`) | 5 | every active party slot: `+0x1E |= 0x02`, the barrier (below) |
| 14 | `BtlPartyRestoreRun` (`0x8003266C`) | 7 ("Dragon Healing") | every party slot 0 to 4 that is active or whose status byte is exactly 0x30 (stone): `maxhp − hp` to `g_BtlSlotAmounts[i]`, HP to its maximum, status 0, active, number kind 5, `+0x1E &= ~0x04`; one show `BtlShowNumberRun(g_BtlActionShowNumber)`, then `BtlSlotActionAnimationSet(i)` (`0x80037E64`) for each slot 0 to 4 whose number kind is not 0. The loop makes its own set and does not read the bytes `BtlTargetPartyMark` (`0x80036294`) has just set: a fallen member (inactive, status 0) is not revived (that is function 12), and with a confused caster, for whom `BtlTargetPartyMark` marks slots 6 to 13, it still heals 0 to 4. The power is not read and nothing is drawn. It has no `g_BtlRunning` test |
| 15 | `BtlEnemyClearRun` (`0x8003281C`) | 8 | unless `BtlSpecialCheck` (`0x80038500`) (battle 0xEB or more): number kind 1 for every active slot 6 to 13; the show; then, in every battle, each slot 6 to 13 whose number kind is 1 has its HP, id, active byte and status set to 0. The second walk also takes a slot whose number kind was left at 1 by an earlier hit (only `BtlAttackOutcomeClear` clears it, before a swing), so a slot that fell earlier can lose its id, and with it what it gives at the battle's end |
| 17 | `BtlTargetHitRun` (`0x80032958`) | 1, 29 | above |
| 18 / 19 | `BtlActingAreaHitRun` (`0x80032A00`) / `BtlTargetAreaHitRun` (`0x80032A28`) → `BtlAreaHitRun` (`0x80032A50`) | spells 30 / 2, 47; attack rows 117 / 84 | `BtlTargetAreaMark(0 / 1)` (`0x80035B50`) marks the area (1 for a slot in it, 3 for the target when the area is round the target); then for each marked slot 0 to 13 the percent bonus `g_BtlActionPower` is set by the mark (1 → 70; 2 → 50, 3 → 100, the mark then 1; any other leaves it) and `BtlHitAmountSet` runs on the slot; `g_BtlTargetSlot` is put back and `BtlHitEnd` (`0x800330FC`) runs. The row's power is not used for marks 1 to 3. No reach check, no `BtlHitTry`, no draw of its own. Nothing read sets mark 2 before this function |
| 21 | `BtlOtherSideHitRun` (`0x80032BB4`) | 3 | `BtlTargetOtherSideMark` (`0x80036394`) + `BtlMarkedHitRun` (`0x80033060`) (`BtlHitAmountSet` on each marked slot) + `BtlHitEnd`. No miss, no pass after the hit |
| 22 | `BtlTargetStat0EHitRun` (`0x80032BDC`) | 9, 21, 25 | the reach gate as in function 17, then `BtlStat0EHitAmountSet` (`0x80035EF8`), below, on one target; the fallen slots taken out, the animations |
| 25 | `BtlOtherSideStat0EHitRun` (`0x80032ED8`) | 6, 11, 23, 27, 39-42 | `BtlTargetOtherSideMark` + `BtlMarkedStat0EHitRun` (`0x80032FA0`): `BtlStat0EHitAmountSet` on each marked slot, in slot order |
| 37 | `BtlTargetAreaStatus8TryRun` (`0x80033510`) | 43 ("Sleep") | `BtlTargetAreaMark(1)` + `BtlMarkedStatus8TryRun` (`0x80033560`) → `BtlTargetStatus8Try` (`0x80033660`) for each marked slot: number kind 2; unless the slot already has a status of `0x35`, `BtlStatusTakeCheck` (`0x80035378`) decides and on a yes `BtlTargetStatus8Set` (`0x8002F958`) gives status 8. Then the show, then `BtlSlotStatusAnimationSet` (`0x8003801C`) for every slot left at 7. It is function 38 with the area in place of the whole side |
| 38 | `BtlOtherSideStatus8TryRun` (`0x80033538`) | 37 | `BtlTargetOtherSideMark` + `BtlMarkedStatus8TryRun` → `BtlTargetStatus8Try` for each slot, as function 37. A slot that already has a status is not tried |
| 45 | `BtlTargetStatus30TryRun` (`0x80033884`) | 46 ("Stone") | the reach gate as in function 17; then `BtlStatusTakeCheck` on the target with no test of its status byte and no number kind 2 first (the check writes it), and on a yes `BtlTargetStatus30Set` (`0x8002FAFC`): status 0x30, the slot inactive, its four counters cleared. The show; `BtlSlotStatusAnimationSet` after a 7 (an enemy's animation for `& 0x30` is 5) |
| 52 | `BtlTargetAreaStatus80TryRun` (`0x80033AB0`) | 45 ("Magic Box") | `BtlTargetAreaMark(1)` + `BtlTargetStatus80Try` (`0x80033BB8`) for each marked slot: number kind 2; only a slot whose status byte is exactly 0 is tried, and on a yes `BtlTargetStatus80Set` (`0x8002FC94`) gives 0x80. The show, then the animation for every 7. Function 51 is the same on one target behind the reach gate |
| 54 | `BtlActingStat08RaiseRun` (`0x80033C3C`) | 4 ("Vigor"), 32 ("Power Up") | stat_08 up on the caster: `g_BtlTargetSlot = g_BtlActingSlot`, then function 55's body |
| 55 | `BtlTargetStat08RaiseRun` (`0x80033CBC`) | 24 ("Power Drive") | stat_08 up on the target: `BtlStatRaise` (`0x80033E50`) raises `+0x08` by `g_BtlActionPower` percent of itself, held to 170 % of `+0x1A`, the amount to `g_BtlSlotAmounts`; the show; number kind 8; `BtlAmountShowRun` (`0x8008979C`) |
| 56 | `BtlTargetStat0ARaiseRun` (`0x80033D2C`) | 28 ("Ice Shell") | the same on `+0x0A` / `+0x1C`, number kind 9 |
| 57 | `BtlTargetStat08Stat0ARaiseRun` (`0x80033D9C`) | 35 ("Cascade Song") | the show first (no slot is marked yet), then both raises, each followed by its number kind (8, then 9) and its own `BtlAmountShowRun` |
| 58 | `BtlEnemyAllRemoveRun` (`0x80033F0C`) | 36 | unless `BtlSpecialCheck`: every active enemy slot's active byte and id set to 0, number kind 1; then the show. HP untouched, no fall mark |
| 59 | `BtlEnemyRemoveTryRun` (`0x80033FCC`) | 48 ("Flash Wind") | each active enemy slot 6 to 13 in order takes `BtlStatusTakeCheck` (one draw each; in a battle numbered 0xEB or more it answers no for these slots without a draw), a 7 made 1; the show; then each slot 6 to 13 whose number kind is 1 has its active byte and id set to 0. HP untouched, no fall mark: the end of function 58, slot by slot and by a draw. It has no `BtlSpecialCheck` test of its own |
| 83 | `BtlActingRemoveRun` (`0x80034844`) | | the acting slot leaves the field: its active byte cleared, without a check and without touching its HP |
| 85 | `BtlTargetRemoveRun` (`0x800349F0`) | 19 ("Fear Litany") | `BtlStatusTakeCheck` on the target (no test of its status or active byte); on a 7 the number kind becomes 1 and, after the show, which reads that 1 to walk the target off, the target's id, active byte and HP are set to 0. Anything else: the show alone. No fall mark is set, so `BtlFallRun` does not run for it, and with its id gone it gives nothing at the battle's end |
| 94 | `BtlTargetRowsHpRun` (`0x80035238`) | 31 ("Sonic Riser"); attack row 118 | `BtlTargetRowsMark` (`0x80035260`) (every active slot of the other side whose y lies from `y − (h·8 + 8)` to `y + h·8` of the target's, h the half height) + `BtlMarkedHitRun`: `BtlHitAmountSet` on each marked slot with the row's own power, then `BtlHitEnd` (the show, the fallen slots, the animations, the wake). The half width is not read |

`BtlStatRaise(stat, slot, base)` (`0x80033E50`), the raise of functions 54
to 57: number kind 5 for `g_BtlTargetSlot`; `cap = base + base * 70 / 100`
in 16 bits; `add = stat * g_BtlActionPower / 100`; when `stat + add` is
above the cap, the excess comes off `add`, so a stat already past the cap
is brought down to it; `stat += add` and `g_BtlSlotAmounts[slot] = add`.
The base is `+0x1A` / `+0x1C`, which `BtlPartyStatsSet` fills with
stat_08 and stat_0a as the member came into the battle, so a raise is
measured against those. On an enemy slot those bytes are its action pick
and trait and its attack and kind-7 numbers; a confused caster can aim a
raise there. Only function 12's revive puts the stat back: a raise lasts
the battle.

`BtlAmountShowRun` (`0x8008979C`) (btl_show_frame.c) is
`BtlAmountShowStart(1)` (`0x80088EA8`), then `BtlShowFrameStep`
(`0x80089DDC`), a frame, while `BtlAmountShowCheck` (`0x8008931C`) says
the show still runs. The raises call it after their show with number kind
8 or 9 in `g_BtlSlotAmountKinds`: the stat's word (resource 2's bank 2,
animations 7 and 8) and the amount over the slot. During the show itself
the slot's byte is the 5 `BtlStatRaise` left.

Function 12 sets no number kind, so its show sees no marked slot and the
face-off of `BtlShowOpen` (`0x8007B7C8`) does not run for it.

Function 12 and the defend stance. `BtlTargetReviveRun` does not touch
`g_BtlSlotRestAnimations` (0x13 is the defend stance the amounts are halved
by), and neither does `BtlFallRun` (it plays animation 0x1C and clears the
HP, the status and `+0x1E`), so a member who fell while defending still
has 0x13 there. The show ends it: both shows function 12 runs, Miracle
Litany's 0x15 (`BtlShow15Run` (`0x8007FD9C`)) and Angel's Tear's 0x34
(`BtlShow34Run`; the Angel Ring's revive too), are
`BtlEffectCopyTargetStandRun` (`0x8007F94C`) (btl_show.c), whose frame loop
at frame 0xF0 gives the target its stand animation
`g_BtlPartyStandAnimations[t]` and sets `g_BtlSlotRestAnimations[t]` to it.
The loop runs `BtlAnimationFramesCount` (`0x80088DF4`) frames, 305 for both
shows by their animation tables, so the write always happens. It happens
for every target, revived or not: an active member given Angel's Tear while
defending stands up and loses the stance. Nall's revive `BtlSlot5Revive`
(`0x80028E5C`) (btl_end.c) plays the stand animation but does not write
`g_BtlSlotRestAnimations`, so a member he revives keeps a stance it fell in
until the round's end.

The area status functions show their action: `BtlMarkedStatus4TryRun`
(`0x800332B8`) (34), `BtlMarkedStatus8TryRun` (37 and 38),
`BtlTargetAreaStatus40TryRun` (`0x8003390C`) (49) and
`BtlTargetAreaStatus80TryRun` (52) all call
`BtlShowNumberRun(g_BtlActionShowNumber)` between the tries and the
animations.

Spell 20, "Escape Litany", cannot be cast in a battle. Its battle target
mode (`+0x01 & 0xF`) is 9, `BtlMenuActionCheck` (`0x800493F8`)
(btl_command.c) passes a spell only when the mode is 0 or
`BtlMenuTargetMaskGet(mode)` (`0x80048AE8`) is not 0, and
`BtlMenuTargetMaskGet` answers 0 for mode 9. So the spell list greys it and
`BtlMenuSpellStep` answers the button with sound 4. Its function number is
0, which `BtlActionRowRun` would send to entry −1, the zero word at
`0x800B2EF4`; the battle never gets there.

Party `+0x1E` bit 0x02 is a barrier that stops one hit. Function 13 sets it
on every active party member; `BtlTargetHpAmountTake` (`0x80035FD8`) (the
stat-0E amount) and `BtlStatusTakeCheck` (a status) each test it for a
party target, stop the hit (number kind 6) and clear the bit.
`BtlHitAmountSet` does not read it. Bit 0x01 (function 8) is the 5 %
regeneration of `BtlRoundStatusUpdate` (`0x800297A4`) each round.
`BtlTargetStatus8Clear` (`0x80030C28`), the wake on a hit, clears bit 0x04
only. `BtlSpecialCheck` is `g_BtlNumber >= 0xEB`, the same test
`BtlHitSlotsUpdate` makes.

Helpers on this path:

* `g_BtlSlotActiveFlags[slot]` is the slot's active byte. `BtlFallRun`
  clears it on a fall, function 12 acts only where it is 0, and
  `BtlActingRemoveRun` (function 83) clears the acting slot's own.
* `BtlTargetOtherSideMark` (`0x80036394`) (btl_target.c) marks every active
  slot of the other side: 6 to 13 for a party slot, 0 to 4 for any other,
  the whole field when `g_BtlActingStatus & 0x40`; the acting slot is then
  cleared.
* `BtlEndCheck` (`0x80038518`) (btl_fall.c): 1 when no enemy slot 6 to 13
  is active, else 2 when no party slot 0 to 4 is, else 0. The enemies are
  counted first.
* `BtlTargetModeAdjust(mode)` (`0x8002EB78`) (btl_action.c) keeps the
  target when it suits the mode and picks another through
  `BtlTargetByModeGet` (`0x800397B4`) (btl_pick.c) when it does not: mode 3
  keeps only a target that is not active (a revive), 4 keeps a target with
  an id and a status, and for an active target 5 picks again when its HP is
  full, 6 when its MP is full, 7 when it has a status; every other mode
  picks again only for a target that is not active. `BtlActionTargetAdjust`
  (`0x8002ED34`) calls it twice.
* `BtlSlotActionAnimationSet` (`0x80037E64`) and `BtlSlotStatusAnimationSet`
  (`0x8003801C`) (btl_slot.c) give the record the animation of its status
  (`BtlAnimationSet` (`0x80027208`)). Party: `0x30` → 0x19, `& 0x20` →
  0x18, `& 0x10` → 0x17, `& 4` → 0x15, `& 8` → 0x16, `& 0x40` → 0x1A,
  `& 1` → 0x14, `& 0x80` → 0x1B; enemies: `& 0x30` → 5, `& 4` → 3, `& 8` →
  4, `& 0x40` → 0x11, `& 0x80` → 0x10.
* `BtlEnemyActionFallbackSet` (`0x80036A18`) (btl_move.c) gives the acting
  slot another action. `BtlEnemyActionAdjust` (`0x80029510`) (btl_round.c)
  calls it for an enemy slot with status 0x40 when `rng_draw() & 3` gives 0
  or 1; `BtlActionStatus40Adjust` (`0x8002E9CC`) and `BtlActionTargetAdjust`
  call it for kind 7, and the second also for some attacks when no party
  slot can be picked. Six ids (0x10, 0x18, 0x1F, 0x2E, 0x44, 0x55) take
  `BtlActionWalkCornerSet(1)`; any other id an attack on
  `BtlSlotNearestFind(0)` (`0x80039838`) (any slot) when its status has bit
  0x40 and `BtlSlotNearestFind(2)` (the party) when not.

### The stat-0E path — `BtlStat0EHitAmountSet` (`0x80035EF8`), `BtlTargetHpAmountTake` (`0x80035FD8`), `BtlStatusTakeCheck` (`0x80035378`)

`BtlStat0EHitAmountSet` (btl_target.c) is the amount of the spells and
specials that work from stat_0e: it hands the amount to
`BtlTargetHpAmountTake`, an end separate from `BtlHitAmountSet`'s. It works
on globals: the acting slot, the target and the power `g_BtlActionPower`.
It reads the acting slot's `+0x0E` (u16) and `+0x14` (level) and the
target's `+0x10` (u16). The roll is one draw brought below `stat_0e / 10`
(nothing is drawn when stat_0e is under 20). The halving comes before the
floor at 0, and rounds toward zero. Its six callers, `BtlTargetStat0EHitRun`
(`0x80032BDC`), `BtlAreaStat0EHitRun` (`0x80032C64`), `BtlMarkedStat0EHitRun`
(`0x80032FA0`), `BtlPartyHpDrainRun`, `BtlTargetStat0EHitStatusRun`
(`0x800344B8`) and `BtlTargetHpDrainRun` (`0x80034770`), read no result.

```c
void BtlStat0EHitAmountSet(void) {
    A = &stats[g_BtlActingSlot]; D = &stats[g_BtlTargetSlot];
    roll = rng_mask_below(0x3FF, (u16)(A->stat_0e / 10));
    def  = D->stat_10;
    v = (A->stat_0e + A->level * g_BtlActionPower + (u16)roll - def) / 2;
    if (v < 0) v = 0;
    BtlTargetHpAmountTake(v);
}

void BtlTargetHpAmountTake(u32 s0) {
    switch (g_BtlActionBitNumber) {        /* bit numbers 1 to 4 only */
    case 1: r = BtlTargetTraitCheck(1, 0x2A, 0x2E, 0x30); break;
    case 2: r = BtlTargetTraitCheck(2, 0x2B, 0x2E, 0x30); break;
    case 3: r = BtlTargetTraitCheck(4, 0x2C, 0x2F, 0x30); break;
    case 4: r = BtlTargetTraitCheck(8, 0x2D, 0x2F, 0x30); break;
    default: r = 0;
    }
    if (r == 1) { s0 >>= 1; kind = 4; } else if (r == 2) { s0 <<= 1; kind = 3; } else kind = 1;
    if (BtlTargetTraitCheck(0, 0x32, 0, 0)) s0 -= s0 >> 2;     /* trait 0x32: a quarter off */
    if (t < 5 && g_BtlSlotRestAnimations[t] == 0x13) s0 >>= 1; /* defending */
    if (s0 == 0) s0 = 1;  if (s0 >= 1000) s0 = 999;            /* unsigned */
    if (t < 5 && (stats[t] +0x1E & 2)) { kind = 6; s0 = 0; clear the bit; }   /* the barrier */
    if (BtlTargetTraitCheck(0, 0x33, 0, 0) == 1) s0 = 0;
    if (g_BtlActionKind == 4 && BtlTargetTraitCheck(0, 0x34, 0, 0) == 1) s0 = 0;
    if (g_BtlNumber == 0xFA) s0 = 0;
    g_BtlSlotAmounts[t] = s0; BtlHitAmountTake();
}
```

* stat_0e and stat_10 are this amount's attack and defence words, as
  stat_08 and stat_0a are `BtlHitAmountSet`'s; stat_0e is also the status
  chance below. docs/spec/battle-data.md calls them magic and magic
  defence; the C has no word for them. The Slime's are 6 and 4.
* Bit numbers 5 to 7 are not tested in this end; `BtlHitAmountSet`'s path
  for kinds other than 2 tests them.

`BtlStatusTakeCheck` (btl_target.c) decides whether a status takes on the
target. It returns 1 and sets the target's number kind to 7 for a yes, and
0 with 2 (or 6) for a no:

```c
if (t >= 6 && g_BtlNumber >= 0xEB)  -> no (2), no draw
if (t < 5 && (+0x1E & 2))           -> no (6), the barrier spent
switch (g_BtlActionBitNumber) {     /* bit numbers 9 to 14 */
    9: (0x100, 0x1E, 0x21)   10: (0x200, 0x1F, 0x21)   11: (0x400, 0x20, 0x21)
   12: (0x800, 0x22, 0x25)   13: (0x1000, 0x23, 0x25)  14: (0x2000, 0x24, 0x25)
}   r = BtlTargetTraitCheck(mask, a, b, 0x26): 1 -> no (2), 2 -> yes (7), no draw
chance = A->stat_0e + g_BtlActionPower - D->stat_0e - (t < 5 ? D->stat_1f : 0);
chance held to 5..0x80;  yes when rng_7f_draw() < chance
```

So the target's stat_0e is its resistance to a status, and a party target
adds `+0x1F` (kind 7 of the worn items' values). A chance of 0x80 is
always a yes, and the draw is still made.

### Caster groups

The spell row's `+0x07` is the id of the character the spell belongs to,
and it parts the 48 rows into eight lists. Row 1, the spell Alex was seen
casting, is in list 1 (rows 1 to 8); row 33, Luna's Healing Song, in list 6
(rows 33 to 38). `ChrLevelUp` (`0x80028554`) teaches a row to the id equal
to its `+0x07` (id 0xB takes id 1's rows) at the level in its `+0x00`,
into the character record's eight places `+0x2A..+0x31` (§"Level-ups").
Which spells a character has at level 1 is resource 0xE7's (§"The party
half"); list 7's two rows are id 10's from the start. The string table
agrees (docs/spec/text.md): spell row N is named by string `N + 0x1B6`, and
list 2's rows 9 to 12 are Thunderbolt, Thunder Bomb, Thunder Thrust and
Spark Ball, Nash's; character 2 is Nash.

## Battle entry graphics — `BtlInit` (`0x80029E68`) and the encounter tables

`BtlInit` (btl_init.c) sets a battle up. In order: the lists and the pad
are cleared, `BtlStateInit` (`0x8002B7DC`) places and clears the records
and the stats, `BtlEnemyLoad` (`0x8002B3AC`) loads the enemies' pictures
and seats them, `BtlEnemyStatsSet` and `BtlPartyStatsSet` fill the stats
(§"The stats"), battle 0xE4 empties slot 5, and `BtlPartyLoad`
(`0x8002B114`) loads and seats the party. Then a battle numbered 0xEB or
more runs `BtlSpecialInit` (`0x8002A7FC`); in any other, story bit 0x2D5
gives every party slot status 0x40 and is cleared. Story bit 0x2D0 runs
`BtlPartyPairStatusSet` (`0x8002A010`). Last come the menu's set-up, the
arena's grey (0x80), and the active bytes: 1 for each slot that has an id,
0 for the others and always 0 for slot 5.

The encounter's pictures and enemies come from tables in the executable's
data, chosen by `g_BtlNumber`, the battle's number:

```c
/* BtlEnemyLoad (btl_init.c), the enemy side */
p   = &g_BtlNumberRows[g_BtlNumber * 2];      /* two bytes a battle: { set, group } */
g_BtlEnemySet = p[0];
ids = g_BtlEnemySetIds + p[0] * 4;            /* the set's enemy kinds, 4 bytes */
grp = g_BtlEnemyGroups + p[1] * 24;           /* the group, 0x18 bytes */
BtlDataLoad(g_BtlEnemySet);                   /* the set's pictures, below */
g_BtlAmountAnimationTable = &g_BtlAmountAnimations[1];
g_BtlAmountYOffsets       = g_BtlAmountYOffsetTable;
g_BtlAmountStepList       = &g_BtlAmountAnimations[g_BtlAmountAnimations[0] * 2 + 1];
/* four blocks from the heap for the amount's pictures; BtlAmountRecordsInit */
/* one sprite data block for each enemy kind; then each enemy of the group is seated */

/* BtlDataLoad (btl_draw.c), one set of pictures */
if (g_BtlDataResources[set] == 0) return;     /* no pictures: g_BtlPage4ImageRows kept */
resource_load(g_BtlDataResources[set], hdr);  /* hdr: u16 words */
LoadClut(buf + hdr[8] * 4, 0, 500);
LoadClut(buf + hdr[9] * 4, 0, 505);
for (i = 0; i < 5 && hdr[10 + 2i] != 0; i++) {
    inflate_buffer(buf + hdr[11 + 2i] * 4, g_LoadBuffer);
    LoadImage({ g_SpritePageCorners[i].x, g_SpritePageCorners[i].y, 0x80,
                (hdr[10 + 2i] >> 8) * 4 }, g_LoadBuffer);
}
g_BtlPage4ImageRows = (hdr[18] >> 8) * 4;
```

* `g_BtlDataResources` (`0x800A7E18`) holds one resource number (u16) per
  set: `0x167 + (set − 1)` for the first sets, with 0 for a set that has
  none. Sets 30, 42 and 44 are 0; entries from 52 on are not resource
  numbers. Palette row 505 is the enemies' (the Slime's, sprites.md), and
  resources 0x167 to 0x16C carry it unchanged.
* `g_SpritePageCorners` (sprites.md) gives where each page goes in video
  memory: (320,0), (448,0), (576,0), (704,0), (832,0) for pages 0 to 4.
* `g_BtlNumberRows` (`0x800AC172`) holds 255 battles over 49 sets (set
  numbers 0 to 51). Battles 1 to 8 use set 1 (resource 0x167).
* Two of the tables are counted from 1, and the symbol the code names sits
  one entry before the data: the compiler writes `&tbl[(n − 1) * 2]` as
  `(tbl − 2) + n * 2`. The group table (`g_BtlEnemyGroups` (`0x800AA974`),
  256 records 0 to 255, directly after the spell table at `0x800AA728 + 49
  × 12`) ends at `0x800AC174` = `g_BtlNumberRows + 2`, so battles 1 to 255
  are at `0x800AC174..0x800AC372`, and a battle 0 would read the last two
  bytes of group 255. The kind lists (sets 1 to 51) begin at `0x800AC374`
  = `g_BtlEnemySetIds` (`0x800AC370`) + 4, just after the battle table and
  its alignment. 51 is the highest set any battle names. The code bounds
  none of these tables.
* Battles 87 and 88 name set 12 with group 0, an empty record: pictures
  with no enemies in the group. What fills them is not read.
* The group record (0x18 bytes), as `BtlEnemyLoad` seats it: `+0..+7` the
  enemy ids for slots 6 to 13, a 0 ending the list; `+8..+15` the x and
  `+16..+23` the y, in units of 8 pixels. For each enemy: record `+0x06` =
  x × 8, `+0x0A` = y × 8 held to `0x70..0xB0`, `+0x08` = `+0x0A`. Group 1 is
  four Slimes (id 1) at x 80, 136, 48, 104 and y 128, 128, 160, 160; in the
  `battle` state each Slime picture's top left is (−12, −14) from those, so
  a record's x and y are an anchor inside the picture, not its corner.
* `g_BtlEnemySetIds[set]` (4 bytes) lists the set's enemy kinds: up to four
  ids, ended by 0. The set's resource begins with one u16 pair per kind,
  `{length in words, place in words}`: each kind's block is inflated
  (`inflate_buffer` (`0x80021D6C`)) into a block of `length × 4` bytes from
  `xmalloc_heap_alloc` (`0x80022F4C`), kept in
  `g_BtlEnemySpriteDataBlocks[kind]`. The kind count is not in the
  resource: `BtlEnemyLoad` inflates as many blocks as the kind list has ids,
  and the unused pairs need not be 0. Each enemy finds its id among the
  first three ids of the list and takes that block; an id that matches none
  of them, the fourth kind's included, takes block 3. The block begins with
  a `SpriteDataHead` (sprite.h) of word places: the record's animation
  table (`+0x28`), step list (`+0x2C`), pose table (`+0x14`), piece table
  (`+0x18`) and texel records (`+0x10`) are the block + word × 4
  (§"The actor renderer", §"Actor animation"). `BtlRecordInit`
  (`0x8002B770`) follows. Also for each enemy: `g_BtlSlotIds[slot] = id`,
  `+0x0C` = 0x20 (0x2020 when x ≥ 0xC0: bit 0x2000 mirrors the figure),
  `+0x0E` = 0x2100, and `BtlAnimationSet(record, 0)` (`0x80027208`),
  animation 0.
* The four heap blocks have fixed sizes: 0x38 for `g_BtlAmountPoseRows`,
  0x3F0 for `g_BtlAmountPieces`, 0xFC for `g_BtlAmountTexelRecords`, 0x340
  for `g_BtlAmountRecords`.
* `BtlEnemyLoad` opens by copying the 13-byte string `"B_DATA00.CMP"`
  (`D_800119C4`) into a 32-byte buffer on its stack, which nothing reads: a
  file name from development. No such file is on the disc. The C's `char
  name[32]` is the size that gives the function its stack frame.
* `BtlAmountRecordsInit` (`0x8004A8A0`) (btl_amount.c) reads
  `g_BtlAmountStepList` (`0x800B8D68`) and `g_BtlAmountAnimationTable`
  (`0x800B8D6C`), which point into `g_BtlAmountAnimations` (`0x800B3168`);
  `BtlAmountRecordStart` (`0x8004AB40`) and `BtlAmountRecordsStep`
  (`0x8004AF94`) read `g_BtlAmountYOffsets`, which points at
  `g_BtlAmountYOffsetTable` (`0x800B3218`). These are the numbers shown over
  a slot, not a figure's animation (that is `BtlAnimationStep`, §"Actor
  animation"). `BtlAmountRecordsStep` steps the 13 records at
  `g_BtlAmountRecords` (0x40 bytes each, with the battle record's timer and
  step fields `+0x34..+0x3A`, but steps of {y place, length}) and frees one
  through `BtlAmountRecordOff` (`0x8004ADA8`) when its run ends (menu-ui.md
  §"The damage numbers"). `BtlAmountRecordStart`
  reads the slot's amount (`g_BtlSlotAmounts`) and splits it into hundreds,
  tens and units, one record a digit.

## Battle graphics — the set bundle, the party bundle, the arena

Every one of the 48 sets and the 12 party characters decodes by the
layouts below. Set 1's four pages match video memory in the running
`battle` state at the `g_SpritePageCorners` places row for row (256, 256,
248 and 48 rows), and palette rows 500, 504 and 505 match in all 512
entries.

### The set bundle (`BtlDataLoad` (`0x80018694`))

```
+0x00  4 x {u16 length_words, u16 place_words}    the kinds' blocks (inflated to length * 4)
+0x10  u16 palette A place  -> LoadClut(buf + place*4, 0, 500)   the arena's palette
+0x12  u16 palette B place  -> LoadClut(..., 0, 505)             the enemies' palette
+0x14  5 x {u16 code, u16 place_words}: pages, ended by a code of 0
       page i -> g_SpritePageCorners[i], 0x80 halfwords x (code >> 8) * 4 lines
g_BtlPage4ImageRows = (page 4's code >> 8) * 4    the lines of page 4 the set uses
```

`BtlDataLoad(set)` (btl_draw.c) has one caller, `BtlEnemyLoad`, with
`g_BtlEnemySet`.

* `g_BtlDataResources[set]` is the resource number; 0 means the set has
  none, and the function returns at once with `g_BtlPage4ImageRows` as it
  was.
* `resource_load(resource, g_ResourceReadBuffer)` reads the bundle. Every
  address handed on is `g_ResourceReadBuffer` + word place × 4.
* `hdr[8]`, `hdr[9]` (`+0x10`, `+0x12`): the two palettes, to `(0, 500)`
  and `(0, 505)`.
* `hdr[10 + 2n]`, `hdr[11 + 2n]` for n = 0 to 4: page n's code and place.
  A code of 0 ends the list. The page is inflated into `g_LoadBuffer` and
  sent with `LoadImage` to `g_SpritePageCorners[n]`, 0x80 wide and `(code
  >> 8) * 4` lines high.
* `g_BtlPage4ImageRows` (`0x800B8900`) = `(hdr[18] >> 8) * 4` (`+0x24`,
  page 4's code), read from the header whether or not the loop reached
  page 4.
* It draws no random number and waits no frame of its own (`resource_load`
  reads the disc).

sprites.md gives palette B's place as `0xDD2`; it is this `0xDD0` + 2,
because that search passed over palette entry 0.

### The party bundle — resource `0x197` (`BtlPartyDataLoad` (`0x80018A78`), `BtlPartyLoad` (`0x8002B114`), `BtlPartyImageLoad` (`0x800187D8`))

```c
/* BtlPartyDataLoad (btl_draw.c) */
resource_load(0x197, g_ResourceReadBuffer);  hdr = *(u16 *)g_ResourceReadBuffer;
*(u16 *)(buf + (hdr & 0xFFFE)) = 0;              /* palette entry 0: see-through */
LoadClut(buf + (hdr & 0xFFFC), 0, 504);          /* the party's palette */
BtlPartyImageAdd(0, 0);                          /* empties the page map */

/* BtlPartyLoad (btl_init.c): each slot 0 to 5 whose id is not 0 */
e = buf + 2 + (id - 1) * 8;                      /* a BtlPartyDataRow */
blk = xmalloc_heap_alloc(e->data_length);  inflate_buffer(buf + e->data_place * 4, blk);
g_BtlPartySpriteDataBlocks[slot] = blk;          /* the record's tables from blk's head */
if (slot != BtlPartyTallestFind()) BtlPartyImageAdd(e, blk);
else                               BtlPartyImagePage4Add(e, blk);
BtlRecordInit(slot);  record +0x0C = 0x2020;  +0x0E = 0x2000;      /* palette row 504 */
if (slot != 5) {
    record +0x06 = g_BtlPartyPlaceXCells[slot] * 8;
    record +0x0A = g_BtlPartyPlaceYCells[slot] * 8;
    BtlAnimationSet(record, g_BtlPartyStandAnimations[slot]);
    if (g_BtlPartyPlaceXCells[slot] < 0x10) record +0x0C = 0x20;   /* the left half: not mirrored */
} else {                                          /* slot 5 */
    record +0x06 = 0xA0;  +0x08 = +0x0A = 0x68;  BtlAnimationSet(record, 5);
}
record +0x08 = record +0x0A;
/* after the loop: g_BtlSlot5Roaming = (g_BtlSlotIds[5] == 0xC), and its six counters cleared */
```

`BtlPartyImageLoad(row, data, page4_first)` (btl_draw.c) places a party
picture in video memory. `g_BtlSpritePageUseMap` holds one byte per unit of
8 lines of pages 4, 5 and 6 (`g_SpritePageCorners[4..6]` = (832,0),
(320,256), (448,256)), 0x20 units a page. A row of 0 empties the map and
marks the first `g_BtlPage4ImageRows / 8` units of page 4 taken by the
set's page 4. A picture needs `(height + 7) / 8` units, the height being
the high byte of the row's word at `+6`. A page is taken when its first
free unit plus the units needed is at most 0x20; nothing after that unit
is checked, pictures being only ever added. The order is page 4 first
when `page4_first` is set, then pages 6, 5 and 4; when none fits the
picture goes to page 5 from line 0, over what is there. The picture is
inflated into `g_LoadBuffer` and sent 0x80 wide and `height` lines high
(not × 4, as the set's pages are). Then every texel record of the block is
patched: `+0x02` gets the page's number and `+0x0A` (v) gets the start
line × 8 added. In the resource the records are relative to their own
picture (page 0, v from its top). `BtlPartyImageAdd` (`0x80018A38`) and
`BtlPartyImagePage4Add` (`0x80018A58`) call it with the flag 0 and 1;
`BtlPartyTallestFind` (`0x8002B064`) picks the slot whose picture is the
tallest that still fits in what page 4 has left, and that slot's picture
alone tries page 4 first.

The 12 rows: every block inflates to exactly its stated length, every
picture to exactly `256 × (code >> 8)` bytes; each has 33 animations,
except id 11 (32) and id 12 (8, Nall). The `battle` state holds ids 1, 6
and 7 in slots 0 to 2 (Alex, Luna and Ramus on the panel) and 12 in slot 5;
that ties each row to its character. Nothing in the code names one.

### Party stage state (`BtlPartyStatsSet` (`0x8002A138`))

`BtlPartyStatsSet` builds each member's stats from the character record
(§"The party half") and, by a switch on the first worn item's `+0x0F` (the
weapon's kind), picks for the slot `g_BtlPartyAttackAnimations`,
`g_BtlPartyStandAnimations` and `g_BtlPartyMoveAnimations` (attack, stand
and move) and the attack sound; `g_BtlPartyHitAnimations` is always 0x12.

| first worn item | attack / stand / move | attack sound |
|---|---|---|
| none | 10 / 5 / 0 | 0x44 |
| kind 0 | 11 / 6 / 1 | 0x3B |
| kind 1 | 13 / 8 / 3 | 0x39 |
| kind 2 | 13 / 8 / 3 | 0x43 |
| kind 3 | 13 / 8 / 3 for id 10, else 14 / 9 / 4 | 0x3C |
| kind 4 | 12 / 7 / 2 | 0x4B |
| kind 5 | 14 / 9 / 4 | by id: 0x39 for ids 1, 5 and 11, 0x3C for 2, 0x43 for 3 and 4; any other id keeps 0 |

The four tables are 0 in the image (they are filled here and cleared by
`BtlStateInit`). Ids 1 and 6 wear no weapon at level 1 (10 / 5 / 0) and id
7's weapon (item 1) is kind 0 (11 / 6 / 1): the two sets the running
`battle` state shows.

The party's places come from the characters' 8-byte records at
`g_ChrPlacePresetRecords` (`0x800B8C74`), one per character id: bytes
`+0` and `+1` are the character's battle place (x, y) in units of 8
pixels, bytes `+2..+7` its three presets, (kind, number) pairs. Resource
0xE7's 0x58 bytes from `+0x342` fill ids 1 to 11 (`ChrRecordsReset` copies
them to the address + 8), and begin with (28,16), (33,18), (28,20) for ids
1, 6 and 7, the places the `battle` state shows.

* `MenuBtlPlacesStep` (`0x80071D50`) (menu_page.c) is the field menu's
  places page: the pad steps a member's place (x down while it is 0x1B or
  more and up while below 0x22, so 0x1A to 0x22; y down while 0xF or more
  and up while below 0x16, so 0xE to 0x16), and the confirm writes
  `g_MenuBtlPlacesXCells[i]` and `g_MenuBtlPlacesYCells[i]` to `id*8 + 0`
  and `+ 1` for every party place whose id is neither 0 nor 0xC. So
  (28,16), (33,18), (28,20) are the new game's places, and the player can
  change them. `MenuBtlPlacesOpen` (`0x80072464`) (menu_places.c) is the
  page's set-up: it reads each member's place into the two menu tables and
  writes none (menu-ui.md, "The formation editor and the member list
  page").
* `MapBtlRun` (`0x8007AE58`) (map_btl.c) moves them into the battle. For
  each party slot i of 0 to 4 with `id = g_BtlSlotIds[i]` (the list ends at
  the first 0; ids of 0x1F or more are passed over) it copies the id's
  eight bytes to eight byte tables: `g_BtlPartyPlaceXCells` (`0x800B8E88`)
  = place x less the argument `place_x_offset`, `g_BtlPartyPlaceYCells`
  (`0x800B8E90`) = place y, and `+2..+7` to `g_BtlPartyPreset0Kinds` /
  `g_BtlPartyPreset0Numbers`, `g_BtlPartyPreset1Kinds` /
  `g_BtlPartyPreset1Numbers`, `g_BtlPartyPreset2Kinds` /
  `g_BtlPartyPreset2Numbers`. `place_x_offset` is 0 for a touched map enemy
  (`MapObjectTouchRun`: `MapBtlRun(rec[0xB], 0, 0, 0, 1, 1, 1)`) and byte
  `[1]` of the battle row for a scripted battle (§"The script's BATTLE
  command"). `BtlPartyLoad` then places slot i at x = place x × 8, y =
  place y × 8, facing 0x20 when place x is below 0x10 (else 0x2020).
* The other readers of the records do not touch the places:
  `MenuPresetStep` (`0x8007070C`) and `MenuPresetMemberDisp` (`0x80070F1C`)
  (the command set-up page) read only the (kind, number) pairs from `+2`.

### The party's x shift, measured; five members

`MapBtlRun`'s `place_x_offset` comes from one place:
`ScriptCommandMapBtlRun` (`0x8005DFF0`) passes byte `[1]` of the BATTLE
row; the map enemy's entry passes 0. The rows, `g_ScriptMapBtlRows`
(`0x800B366C`), are 64 rows of 8 bytes, 37 of them used. Byte 1 is 10 for
battles 226 (rows 1 and 2), 228, 229, 232 and 233, and 20 for 227, 230 and
231; every other row holds 0. Over every map object set and every event
script, battles 226 to 233 are started only by BATTLE commands, each by
rows that agree, and the five battles that both a map enemy and a script
start (41, 86, 180, 181, 182) have 0 in their rows: the shift depends on
the battle's number alone. The groups show what it does: 226, 232 and 233
hold enemies at x 5 to 10 and 28 to 33, and the shifted party stands
between them at x 18 to 23 (an ambush); 227, 230 and 231 hold them at x 24
to 35, mirrored, and the party stands at x 8 to 13, below 0x10, so
`BtlPartyLoad` leaves `+0x0C` at 0x20 and the party faces right.
`g_BtlPartyPlaceXCells` is a byte table, so the subtraction wraps, though
no place the menu offers (x of 0x1A or more) less 20 comes near.

Five members: `MapBtlRun` seats `g_BtlSlotIds[0..4]`,
`ScriptCommandMapBtlRun` fills it with up to five ids below 0xC (0xC, Nall,
goes to slot 5), and the panel draws five columns (`BtlPartyPanelDisp`
(`0x8004B330`)). The rows' own lists name five for 0xF0 (1, 2, 4, 5, 3)
and for 0xF4, 0xF5, 0xF6, 0xF7 and 0xFE (11, 2, 4, 5, 3, Nall); 0xF1 and
0xF9 name four (11 or 1, 2, 5, 10), 0xF2 and 0xF3 four (11, 4, 5, 3), 0xEB,
0xEE and 0xEF four, 0xEC three, 0xED two. The places page has no test for
two members on one place: a member can be stepped onto another's place,
and the confirm writes both. Two story members whose new-game places
coincide (Kyle and Ramus at (28,20); Ghaleon, Laike and Tempest at (30,18))
are never in the party together. Two paths are reached only by a party of
five:

* The show of enemy action 0xBC (`BtlShowBCRun` (`0x80040C70`), attack row
  57 "Piercing Eyes", battle 0xF2's Shadow Spectre; function 25 marks
  `BtlTargetOtherSideMark`'s slots) stands a copy of the caster on each
  marked party slot 0 to 3 (slot 4 is not looked at) and waits on the
  last. With slots 0 to 3 all fallen and slot 4 standing, the record it
  waits on is picked by a register the code never set. 0xF2's own row
  fields four.
* The grid's missing clip on the right ([boss-ai.md](boss-ai.md) §"The grid
  has no clip on the right"): in 0xF7 with its row's five, the party's swings can push
  the Emperor from x 272 to 284 (`BtlTargetPushBack` (`0x80037C64`): 4
  pixels a step, held inside `w / 2 + 8 .. 0x138 − w / 2` for his width of
  56, the sheet's 72 less 0xF7's 0x10, so 284 is allowed); his 0xAF strike
  then widens him by 0x20 to 88, the footprint passes column 39, and the
  walk's measure reads past the 480-cell grid.

### The arena (`BtlFrameDraw` (`0x80018564`))

`BtlFrameDraw` (btl_draw.c) draws the battle's frame: the sprite list, the
layers' fade step, the arena, the text grid, and the frame's end and
start. The arena is two fixed sprites drawn through `sprite_grey_draw`
(`0x80016E90`) → `sprite_plain_draw` (`0x80016CF0`) at depth 0x300 in the
ordering table, behind every figure (figures use 0x20 to 0x2FF):

| | screen x,y | w x h | u,v | tpage | palette |
|---|---|---|---|---|---|
| A | 0,0 | 256x240 | 0,0 | `g_SpritePageTPages[0]` = 0x85 (page (320,0)) | GetClut(0,500) |
| B | 256,0 | 80x240 | 0,0 | `g_SpritePageTPages[1]` = 0x87 (page (448,0)) | GetClut(0,500) |

`sprite_grey_draw(x, y, u, v, w, h, tp, clut, semi, grey, ot)` (sprite_draw.c)
is `sprite_plain_draw`'s plain sprite with colour-table entry 0 replaced
for the length of the call:

1. `old = g_SpriteCols[0]`
2. `g_SpriteCols[0] = grey | grey << 8 | grey << 16`, the low byte of the
   tenth argument in all three channels
3. `sprite_plain_draw(x, y, u, v, w, h, tp, clut, semi, 0, ot)`: colour 0,
   so the sprite's colour word is `g_SpriteCols[0] | 0x64000000`
4. `g_SpriteCols[0] = old`

Everything but `grey` is passed on unchanged. `BtlFrameDraw` is its only
caller (two calls), with the two rows of the table, `semi` 0, `ot` 0x300
and `grey` the word `g_BtlBgGrey` passed whole; the function keeps only its
low byte. Because the entry is put back, the arena never leaves its grey in
the colour table: the figures drawn after it in the same frame read their
own `g_SpriteCols[col]`.

The colour is (b,b,b) with b the low byte of `g_BtlBgGrey` (0x80 at the
battle's start; the fades `tile_mix_set` (`0x8001DD38`) and
`tile_mix_fade_step` (`0x8001DDFC`) dim it). The display is 320x240 with a
320x224 clip (`screen_mode_set` (`0x8001F0C8`), `packet_draw_area_add`
(`0x80015E24`)), so what shows is page 0's lines 0 to 223 and page 1's
columns 0 to 63; no set changes the shape, only the pages and palette A.
There is no fill behind it: the arena is the background, and palette A has
no 0x0000 entry. When `g_BtlBgWave` is not 0 a wave is drawn in its place
(`BtlBgWaveDraw` (`0x800183A0`), 14 strips of 16 lines shifted by the
triangle table `g_BtlBgWaveTable`), which the effect player sets
(`BtlShowWaveStep` (`0x8008A5E4`), effect record `+0x10 == 4`). That draw
stores the whole `g_BtlBgGrey` word in `g_SpriteCols[0]`, where the flat
draw stores the byte three times; the two give the same colour only when
the word is itself a grey (with 0x80 the wave's colour has red alone).
Whether that shows on the screen has not been looked at.
`g_SpritePageTPages` is built once by `sprite_init` (`0x80018BFC`) as
`GetTPage(1, 0, page)` for pages 0 to 9.

## The actor renderer — how a pose is drawn

`sprite_list_draw` (`0x8001A798`) (sprite_list.c) draws the sprite list
`g_SpriteList` (entries of 0x24 bytes, `SpriteEntry`, sprite.h) in three
ranges, 0x46 to 0x6D, 0 to 0x45 and 0x6E to 0x95, through `sprite_draw`
(`0x8001A528`). The battle's `BtlAnimationStep` (`0x800272A8`) and the
field's `MenuRecordsAnimationStep` (`0x8006BE10`) both fill it, so the
field's figures and the battle's share one renderer. An entry is the
record's `+0x04..+0x27` copied whole, with `+0x18` the current pose. An
entry is drawn only when its `+0x00` word (the record's `+0x04`) is not 0.

Where (`sprite_draw`, whose only caller is `sprite_list_draw`): the entry's
`+0x02` x and `+0x06` y (the record's `+0x06`, `+0x0A`), each less the
camera from `bg_scroll_get` (`0x80014030`) unless `g_BtlRunning` is set.
The entry's `+0x04` (the record's `+0x08`) gives the depth: d = `+0x04` −
camera y + 0x20, depth = 0xFF − d for 0 ≤ d < 0x100, 0 above, 0xFF below.
Entries 0x46 to 0x6D take 0xFF and entries from 0x6E take 0. The pose
table is the entry's `+0x10` (the record's `+0x14`), the piece table the
entry's `+0x14` (the record's `+0x18`).

Per entry (`sprite_draw`):

* The record's `+0x0E` is the draw word. The palette is `GetClut(word &
  0xFF, 0x1F8 | ((word >> 8) & 3))`: the party's 0x2000 gives row 504, the
  enemies' 0x2100 row 505. `word & 0x6000` picks the ordering-table cell:
  0 → 0x40; 0x2000 → 0x200 + depth (the depth of the paragraph above, from
  the entry's `+0x04`, not from the drawn y `+0x06`); 0x4000 → 0x200 +
  depth as well (two `case`s with the same body; a plain 0x200 is only the
  switch's default, which no value reaches); 0x6000 → 0x20 + (0xFF − y) /
  8, a signed division on the y less the camera. `(+0x0E bit 10) | (+0x0C
  bit 15)` is passed down as the primitive's semi-transparency switch (the
  drawers OR 2 into the code byte), and `(+0x0E bit 15) | (+0x0C bit 15)
  << 1` picks the colour word `g_SpriteCols[]` (0x6000 makes it 0x808080
  for that draw).
* `+0x0C` bits 13 and 14 mirror the whole figure in x and y
  (`sprite_piece_draw` (`0x80019DBC`): `(+0x0C >> 9) & 0x30`, exclusive-or
  with each piece's own bits).
* The pose `{u16 count, u16 first}` (`SpritePoseRow`) at
  `pose_table[pose]`: with no pose table the count is 1, but the first
  piece is still read as `poses[pose * 2 + 1]` through the null address;
  the code has no second test. The pieces are drawn from `first + count −
  1` down to `first`: `for (i = count − 1; i >= 0; i--) { if (i >= 0x33)
  break; sprite_piece_draw(...); }`. The 0x33 test is on the counting-down
  `i`: a pose of up to 51 pieces draws all of them, and a pose of more than
  51 draws none (the loop breaks on its first pass).
* Which piece ends on top. The loop is the call order, every piece of the
  entry is given the same ordering-table cell, and both emitters link with
  `AddPrim` (`0x8008E9D8`) on `g_PacketOtBase + ot`, which makes the new
  packet the head of the cell's chain (`p->next = cell->next; cell->next =
  p`) and does no depth arithmetic of its own. So the piece added last
  (`first`) is drawn first and lies at the bottom, and `first + count − 1`
  ends on top. Within one call the tint copy (below) is linked before its
  piece, so it is drawn right after the piece, over it. The multi-part
  enemies (the Ultra Fiend) are stacked by this rule.
* The same cells order the figures: a record's depth is 0x200 + (0xFF − (y
  + 0x20)) from `+0x08`, and at equal depth the lower slot (added first by
  `BtlAnimationStep`) is on top. Draw word 0 puts a figure in cell 0x40,
  0x2000 and 0x4000 sort it by y, and 0x6000 puts it in front of mode 0.
* Slot 5 and the windows. `BtlPartyLoad` gives every record it sets up,
  Nall's (slot 5) included, `+0x0C` = 0x2020 and `+0x0E` = 0x2000: he is
  sorted by y like the party (`+0x08` is his depth y, and his bob moves
  it). The battle windows (`BtlWindow0Set` (`0x8004A13C`)'s two display
  places) are in cell 0x40, the labels and faces lower still (0x38, 0x10),
  so every figure of draw word 0x2000 or 0x4000 (0x200 + key) is behind
  them; only draw word 0 (cell 0x40, by the order of adding) and 0x6000
  (0x20 to 0x3F) could be in front.

Per piece (`sprite_piece_draw`), the 0x18-byte piece (`SpritePiece`,
sprite.h):

```
+0x00 u16 kind   & 0x7000 the kind, & 0x30 the piece's mirror (x, y), & 0x0F00 the anchor modes
+0x02 s16 width   +0x04 s16 height   +0x06 u16 texel record number (or a colour)
+0x08..+0x16  8 x s16: (x0, y0) the offset for kind 0, the points of a rectangle or quad otherwise
texel record (0x0C, SpriteTexelRecord): +0x02 u16 page (-> g_SpritePageTPages), +0x08 u, +0x0A v
```

* Kind 0, a sprite: `mirror = figure bits ^ piece bits`; not mirrored →
  `sprite_plain_draw` (`0x80016CF0`) (a SPRT) at (x + x0, y + y0); mirrored
  → `sprite_mirror_draw` (`0x80016F44`), a POLY_FT4 whose left edge samples
  u + w − 1 and right edge u − 1, the texels u to u + w − 1 reversed. The
  place mirrors only with the figure's bit: x − x0 − w. The piece's `+0x00`
  is not a u offset: its values 0 and 16 are 0 and the x-mirror bit 0x10,
  and a 0x10 piece at x0 = 0 beside a plain one at x0 = −16 is a symmetric
  sprite's mirrored right half. Read as a u offset it pulls stray texels
  from the next picture on the page; read as the mirror bit it draws Alex's
  shadow symmetric.
* Kind 0, the two unscaled emitters (sprite_draw.c):
  * `sprite_plain_draw(x, y, u, v, w, h, tp, clut, semi, col, ot)` (also
    called by `sprite_grey_draw` (`0x80016E90`) for the arena's two halves
    and twice by the debug page viewer `debug_sprite_page_show`
    (`0x80019044`)) takes one 0x18-byte packet at the packet cursor
    `g_PacketWritePointer`: a draw-mode word and a SPRT. Tag 0x05000000,
    mode `0xE1000200 | tp` (`tp` is the texture page word; the callers have
    already looked it up in `g_SpritePageTPages`), colour `g_SpriteCols[col]
    | 0x64000000` (`| 2` in the code byte when `semi`), and x, y, u, v (u8),
    w, h, clut as passed. Nothing is clamped, mirrored or scaled.
  * `sprite_mirror_draw(x, y, tu, tv, tw, th, tp, clut, semi, col, ot, mir)`
    (one caller, `sprite_piece_draw`) builds one POLY_FT4 (0x28 bytes, tag
    0x09000000, colour `g_SpriteCols[col] | 0x2C000000`, `| 2` when
    `semi`). Here `tp` is the page number and the function reads
    `g_SpritePageTPages[tp]` itself, and tu and tv come as the texel
    record's full u16 `+0x08` and `+0x0A` (the plain path passes only the
    low bytes). The mirror is in the geometry, the vertex order reversed
    while the texel edges stay ascending, shifted down by one, on both axes
    (`mir` bit 0 x, bit 1 y):

    ```
    X plain     x0 = x        x1 = x + tw    u0 = tu        u1 = tu + tw
    X mirrored  x0 = x + tw   x1 = x         u0 = tu - 1    u1 = tu + tw - 1
    Y plain     y0 = y        y1 = y + th    v0 = tv        v1 = tv + th
    Y mirrored  y0 = y + th   y1 = y         v0 = tv - 1    v1 = tv + th - 1
    ```

    u0 and v0 are held at 0 (when `tu − 1` would be negative: `tu = 0;
    tw--`, after x0 was taken from the full width, so the quad keeps its
    width on the screen and the texel span is one shorter); u1 and v1 are
    held to 0xFF. On an axis that is not mirrored the far texel edge is the
    exclusive `v + h`, not `v + h − 1`. (x, y) is the rectangle's top left
    either way; the place's mirror stays in `sprite_piece_draw`.
  * The tint copy, on both. When `g_SpriteMixLevelGrey` is not 0x808080 a
    second packet follows in the buffer, a copy of the first with colour
    `g_SpriteMixCopyCol`, palette `GetClut(0, 0x1FD)` (`0x8008E9B8`), blend
    mode 1 (`mode |= 0x20` / `tpage |= 0x20`) and the semi bit, and it is
    linked first; the piece itself is linked last and always, into the same
    cell. `sprite_quad_draw` (`0x800174F0`) and `sprite_scale_draw`
    (`0x80017988`) end the same way.
* Kind 0, scaled: `sprite_scale_draw`. When the entry's `+0x1C` is 1,
  `sprite_piece_draw` sends a kind-0 piece here with the 8.8 scale `hx` /
  `hy` = the entry's `+0x20` / `+0x22`. It is always one POLY_FT4. The
  offset (`+8` / `+0xA`) and the size (`+2` / `+4`) are each scaled `(v *
  h) >> 8`, the size held to at least 2. The texels are never swapped: UV
  is always (u, v) to (u + w − 1, v + h − 1), the far edge held to 0xFF;
  the mirror is geometry only, on the combined `mir` = figure bits ^ piece
  bits, so here a piece's own mirror bit moves the piece too, unlike the
  unscaled path, where the place mirrors only with the figure's bit.
  * `g_BtlRunning == 1` (a battle): x0 = x + dx, x1 = x0 + w; mirrored x0
    = x − dx, x1 = x0 − w (the scaled offset is negated, so the piece flips
    about the figure's origin); y the same with dy and h. Nothing is
    widened.
  * otherwise: the size is widened by one: x1 = x0 + w + 1; mirrored x1 =
    x + dx − 1, x0 = x1 + w − 1 (flipped in place, one pixel left and one
    narrower); y1 = y0 + h + 1; mirrored y0 = y + dy + h + 2, y1 = y + dy.

  The colour is `g_SpriteCols[col]`, with the semi bit, and
  `sprite_quad_draw`'s tint tail (a second, adding copy on palette row 509
  when `g_SpriteMixLevelGrey` is not 0x808080). For draw word 0x6000
  (`entry +0xA & 0x6000 == 0x6000`) the caller makes `g_SpriteCols[col]`
  0x808080 around the unscaled draw only (kept and put back); the scaled
  call uses `g_SpriteCols[col]` as it is.
* A fifth emitter that nothing calls: `sprite_mirror_scale_draw`
  (`0x800171C4`). The image holds no `jal` to it and no word with its
  address, so no piece kind reaches it. `(x, y, ox, oy, tu, tv, tw, th, hx,
  hy, tp, clut, semi, col, ot, mir)` builds one POLY_FT4 with top left `(x
  + ((ox * hx) >> 8), y + ((oy * hy) >> 8))` and size `((tw * hx) >> 8)` ×
  `((th * hy) >> 8)`, `hx` / `hy` 8.8 fixed, the shifts logical. Nothing is
  clamped or widened (`sprite_scale_draw` holds the size to at least 2).
  Unlike `sprite_scale_draw` it mirrors by the texel edges,
  `sprite_quad_draw`'s rule, on `mir` bit 0 (x) and bit 1 (y): plain u0 =
  tu, u1 = tu + tw − 1 held to 0xFF; mirrored u0 = tu + tw − 1 held to
  0xFF, u1 = tu − 1 held at 0; v the same with tv and th. The texel span is
  the unscaled tw and th (taken before the size is scaled). `tp` is the
  page number (as in `sprite_mirror_draw`), the colour `g_SpriteCols[col] |
  0x2C000000`, `semi` the semi bit, then the same tint tail, linked at
  `g_PacketOtBase + ot`.
* Kinds 0x1000 and 0x2000: an anchored rectangle, corners from
  `sprite_piece_edges_get` (`0x80019BEC`): mode = kind & 0xF00; 0 →
  (+08,+0A) to (+10,+12); otherwise the x mode (bits 8 and 9) 1 = from +08
  rightward by +0C, 2 = centred ((+0C + 1) / 2 each side), 3 = leftward;
  the y mode (bits 10 and 11) the same with +0A and +0E.
* Kind 0x3000: a free quad, points (+08,+0A) top left, (+0C,+0E) top
  right, (+10,+12) bottom right, (+14,+16) bottom left. Both rectangle
  kinds and quads go to `sprite_quad_draw`: a POLY_FT4 with UV top left
  (u,v), top right (u+w−1,v), bottom left (u,v+h−1), bottom right
  (u+w−1,v+h−1) (swapped by the piece's mirror bits), +1 on the larger
  coordinate of each vertex pair, an optional 8.8 scale (`entry +0x1C ==
  1` → `+0x20/+0x22`), and the figure's mirror negating x.
* Kinds 0x4000 and 0x5000: untextured, a coloured primitive from a 15-bit
  colour. No battle block has them, but the command windows are drawn with
  them (menu-ui.md §"The battle windows"). The piece's `+6` with
  bit 15 set is the colour; with it clear it indexes a colour table in
  memory. The semi-transparency switch for these is the entry's `+0x08`
  bit 15.
  * 0x4000: `sprite_piece_box_draw` (`0x80019668`) (sprite_list.c): one
    `TILE`, its colour the word at `+6` or that entry of the u16 table at
    `0x801D5B80` (the colour buffer `g_PaletteBuf` points at). `semi` not 0
    does two things: the tile is marked semi-transparent, and a `DR_TPAGE`
    packet (code 0xE1000200: dither on, blend mode 0) is linked at the same
    ordering-table entry after it; `AddPrim` puts a packet at the head of
    its entry, so the mode word, added second, is sent first. The packet
    pointer ends 0x10 on, 0x18 with `semi`. In a battle (`g_BtlRunning ==
    1`) at (`+08`, `+0A`) sized (`+10` − `+08`, `+12` − `+0A`); otherwise
    at the piece's (x, y) sized (`+10` + 2, `+12` + 2).
  * 0x5000 in a battle: one `LINE_F3` built in `sprite_piece_draw` through
    the three points (`+0C`, `+0E`) → (`+08`, `+0A`) → (`+14`, `+16`), with
    the colour table at `0x801D6380`.
  * 0x5000 outside a battle: `sprite_piece_outline_draw(x − 3, y − 3, +10
    + 3, +12 + 3, piece, semi, ot)` (`0x800197CC`), never
    `sprite_piece_box_draw`: a rectangle's outline in two `LINE_F3` packets,
    (x, y) → (x + w, y) → (x + w, y + h) and (x + w, y + h) → (x, y + h) →
    (x, y). Its colour table is `0x801D5B80`, the tile's, not the battle
    line's. r = (low byte & 0x1F) << 3, g = (word & 0x3E0) >> 2, b = (word &
    0x7C00) >> 7, worked out once for both packets; `SetSemiTrans(p, 1)` on
    each when `semi` is not 0; both linked at ordering-table entry `ot`, the
    packet pointer `g_PacketWritePointer` ending 0x30 on.
* `g_SpriteMixOn` and `g_SpriteMixLevels` pick a colour change; anything
  but 0x808080 adds an adding copy against the flat palette row 509.

In the battle blocks kind 0 is about 98 % of the pieces; the party's
blocks carry 3 to 10 rectangle or quad pieces each (the weapons' arcs), the
enemy blocks of set 1 none.

## Actor animation — tables, tick, ids

```c
void BtlAnimationSet(record, n) {                  /* btl_animation.c */
    if (record->animation != n) {
        record->animation_before = record->animation;
        record->animation = n;
        BtlAnimationStart(record, 1);
    }
}
void BtlAnimationStart(record, restart) {          /* 0x80027240 */
    row = animation_table[record->animation];      /* {u16 count | 0x8000 hold, u16 first} */
    record->step = row.first;  record->end = row.first + row.count;   /* keeps bit 15 */
    if (restart) { record->timer = 0; record->step_length = step_list[record->step].length; }
}
void BtlAnimationStep(BtlRecord *record, s32 first, s32 n) {
    entry = g_SpriteList + first;
    sprite_run_off(first, n);                      /* entries first..first+n-1: +0 = 0 */
    for (i = 0; i < n; i++, record++, entry++) {   /* i a u8 */
        if (record->on == 0) continue;             /* the entry still moves on */
        if (++record->timer >= record->step_length) {
            old = record->step++;
            if (!(record->end & 0x8000)) { if (record->step >= record->end) BtlAnimationStart(record, 1); }  /* loop */
            else if (record->step >= (record->end & 0x7FFF)) record->step = old;                           /* hold */
            record->timer = 0;
            record->step_length = step_list[record->step].length;
        }
        *entry = record->sprite;                   /* +0x04..+0x27 */
        entry->pose = step_list[record->step].pose;
    }
}
```

The step on is always stored, and a held animation writes `old` back. The
sprite entry moves on with every record, on or not, and `sprite_run_off`
(`0x80019A7C`) first clears the `+0` word of all n entries, so a record
that is off leaves a cleared entry, not an old one. The steps are
`SpriteStep {u16 pose; u16 length}`, the animation rows `SpriteAnimationRow
{u16 count; u16 first}` (sprite.h). Its callers: `BtlFrameStep`
(`0x80038D2C`) (`g_BtlRecords`, 0, 0x1C), btl_window.c (`g_BtlRecords`, 0,
0x14), btl_outcome.c (`g_BtlExtraRecords`, 0x1C, `g_BtlExtraRecordCount`),
btl_show_frame.c (`g_BtlShowRecords`, 0x1C, 0x15) and btl_command.c
(`g_BtlMenuCopyRecords`, 0x1C, 6, and `g_BtlMenuMarkRecords + 0x40`, 0x6E,
3).

Record set-up. Both battle set-ups first point the record's `+0x10`,
`+0x14`, `+0x18`, `+0x28` and `+0x2C` at the tables of the block, then
call `BtlRecordInit(slot)` (`0x8002B770`) (btl_init.c), then set the words
`+0x0C`, `+0x0E` and the place `+0x06` / `+0x0A`. `BtlRecordInit` sets
`+0x00` = 1 (on) and calls `BtlRecordSizeSet(record, 0)` (`0x800381D8`)
(btl_slot.c), then sets `+0x30` = 0xFFFF (no animation, so the set-up's
`BtlAnimationSet` (`0x80027208`) always starts one), `+0x04` = 1 (the
sprite entry's on word), and clears `+0x1C` and `+0x20`.
`BtlRecordSizeSet(record, n)` measures the first pose of animation n: over
that pose's pieces it keeps the lowest x and y (neither above 0) and the
highest x + width and y + height, and stores the spans at `+0x3C` (width)
and `+0x3E` (height).

The tick is one per battle frame: `BtlFrameStep` calls
`BtlAnimationStep(g_BtlRecords, 0, 0x1C)` once, and the display waits
`VSync(0)` with no make-up for a missed frame, so a step of length 1 is
1/59.94 s (NTSC). `BtlAnimationEndWait` (`0x8002FDBC`) (the wait after a
swing) returns when the last step begins, not when it ends.
(`ShopAnimationSet` (`0x80053648`) and `MapAnimationStart` (`0x80066468`)
start animations of the same layout on the field; the second masks the
hold bit.)

### Battle animation ids

The ids are per block, but the code picks them, so these hold for every
enemy and party block. They come from the 92 places in the code that set
an animation.

| figure | id | meaning | where |
|---|---|---|---|
| enemy | 0 | rest | `BtlEnemyLoad` (`0x8002B3AC`) set-up; `BtlActingRestAnimationSet` (`0x80037890`) after acting; `BtlSlotStatusAnimationSet` (`0x8003801C`) with status 0 |
| enemy | 1 | move | `BtlWalkSet` (`0x8002BAA4`) puts 1 in the walk block's `+0x0D` for a slot of 5 or more, played by `BtlWalkPathFollow` (`0x8002CA78`) during the walk; also the walks off the field |
| enemy | 2 | the swing (once) | `BtlAttackAnimationRun` (`0x800315A8`): `slot < 5 ? g_BtlPartyAttackAnimations[slot] : 2` |
| enemy | 3, 4, 5, 0x11, 0x10 | the statuses' rests | `BtlSlotStatusAnimationSet`: `& 0x30 → 5, & 4 → 3, & 8 → 4, & 0x40 → 0x11, & 0x80 → 0x10`, the first that matches |
| enemy | 7, 8 | 7 while a special (kind 7) is planned; 8 the animation show 0x97 plays at its target (2 for id 0x72) | `BtlSlotActionAnimationSet` (`0x80037E64`), `BtlShow97Run` (`0x8003A3BC`) |
| party | `g_BtlPartyStandAnimations` / `g_BtlPartyMoveAnimations` / `g_BtlPartyAttackAnimations` | stand / move / attack, by the weapon's kind | `BtlPartyLoad` (`0x8002B114`), `BtlPartyStandAnimationSet` (`0x80027CE4`) (the round's end), `BtlAttackAnimationRun` |
| party | 0x11 | missed | `BtlAttackMissShow` (`0x80031378`), `BtlPartyHitAnimationSet` (`0x80030E78`) |
| party | 0x12 | hit | `BtlAttackHitShow` (`0x800314B8`), `BtlPartyHitAnimationSet`, `BtlAttackAllRun` (`0x800318DC`) |
| party | 0x13 | defend (an amount is halved while the slot rests on it) | `BtlActingRestAnimationSet`, `BtlSlotActionAnimationSet` |
| party | 0x0F | the win | `BtlWinMessageDisp` (`0x8004BB68`): each active member with HP, not in status 0x30, id not 0xC |
| party | 0x14..0x1B | the statuses | `BtlSlotStatusAnimationSet`: `== 0x30 → 0x19, & 0x20 → 0x18, & 0x10 → 0x17, & 4 → 0x15, & 8 → 0x16, & 0x40 → 0x1A, & 1 → 0x14, & 0x80 → 0x1B` |
| party | 0x1C | fallen | `BtlFallRun` (`0x800385B4`) |
| party | 0x1D; 0x1E, 0x1F | 0x1D the animation of kind 10; 0x1E or 0x1F while a spell is planned, by id and spell | `BtlActionKindRun` (`0x8002E12C`), `BtlSlotSpellAnimationGet` (`0x80037F84`) |
| slot 5 | 5; 0-3; 6; 7 | the set-up; the roaming machine (`BtlSlot5RoamStep` (`0x80038EE8`), while `g_BtlSlot5Roaming` is set); the walk to a fallen member; the revive (26 in 128 each round, `BtlSlot5ReviveTry` (`0x80027B6C`) → `BtlSlot5Revive` (`0x80028E5C`)) | with the revive the voice 0x3D is played from the XA file (sound.md §"The XA voice clips"); the member comes back with 10 HP when story bit 0x2AB is set and 1 HP when not |

Enemies have no hit, dodge or fall animation. A hit is a push back only
(`BtlTargetPushBack` (`0x80037C64`): kind 2, the target's HP at or below a
third of its maximum after the hit, 8 steps of 4 pixels; 2 steps for ids
of 0x6E or more); a dodge is a step aside (`BtlTargetMissShow`
(`0x80036838`)); a fall is an effect and sound 0x28 (`BtlFallRun`). The
plain hit runs in `BtlAttackHitShow`'s order: the attacker rests
(`BtlActingRestAnimationSet`), `BtlTargetPushBack`, the impact. A push
step that does not move waits no frame, and the push draws no number of its
own. The dodge is `BtlAttackMissShow` → `BtlTargetBackMove` (`0x8002FD1C`)
/ `BtlTargetForwardMove` (`0x8002FE78`) / `BtlTargetMissShow`.
`BtlTargetReachCheck` (`0x800379AC`) gives eight enemy ids animations of
their own round the walk (the animation before the walk, the walk block's
`+0x0D` walking animation, the animation after): 0x59, 0x2A, 0x39 and 0x48
9 / 0xB / 0xA; 0x79 1 / 0xD / 0xB; 0x7D 6 / 1 / 7; 0x12 9 / 1 / 0xA; 0x7C
8 / 1 / 9, with record 7 on 0x15 before the walk and on 0x16 and then 0xB
after it; 0x7F 0xB / 0x10 / 0xC; any other id walks alone. The `+8` depth
nudge (`BtlAttackAnimationRun` +1, `BtlAttackMissShow` and
`BtlActionAttackRun` (`0x80031060`) −1) comes to nothing over a swing,
except during a ranged weapon's shot (0x3A to 0x3C), whose object then
sorts one line deeper (battle-presentations-2.md, "Group D"). Animations 6 and
9 to 15 are played only by shows keyed to an id or a battle.

The status bits, as these readers use them (the names are by what the code
does with them): 0x08 sleep (no action, ended by a physical hit, counter
`g_BtlSlotStatus8Rounds`), 0x04 paralysis (no action and no dodge, counter
`g_BtlSlotStatus4Rounds`), 0x30 stone (0x10 → 0x20 → 0x30 over the
rounds), 0x40 confusion (a command drawn at random, targets over the whole
field), 0x80 silence (the magic item greyed), 0x01 poison (the maximum HP
× `g_BtlPartyStatus1Shares` / 100 each round, 10 % growing by 2 a round to
20 %). Party `+0x1E` bit 0x01 gives 5 % of the maximum HP back each round
(`BtlRoundStatusUpdate` (`0x800297A4`)).

### Slots and the frame

The record and stats slots: 0 to 4 the party, 5 the companion (left out of
the sides, the target scans and the win), 6 to 13 the enemies, 14 a
scratch marker for a move's goal. The battle's main loop is `BtlRun`
(`0x80027434`) (btl_run.c); `BtlFrameStep` (`0x80038D2C`) (btl_frame.c) is
its tick each frame (the records, the extra copies through
`BtlExtraRecordsStep` (`0x8004163C`), the arena, the flush);
`BtlSlot5RoamStep` (`0x80038EE8`) is the companion's roaming tick, not the
main loop.

## Spell presentations — `BtlShowNumberRun` (`0x8007BB80`) and the effect resources

What a cast looks like. Every function of an action's row calls
`BtlShowNumberRun(g_BtlActionShowNumber)` (btl_show.c) with the show
number its row gave (a spell row's `+0x06`). The party's spells have shows
0x01 to 0x2F. `BtlShowNumberRun` sends 0x97 to 0xFD to `BtlActingShowRun`
(`0x8003A158`) (the enemies' shows, below) and every other number but 0
through `BtlShowOpen(n)` (`0x8007B7C8`) (the set-up, which also loads
effect n), the function `g_BtlShowFunctions[n − 1]` and `BtlShowClose`
(`0x8007BACC`) (which frees what was taken and waits for the show's
voice). menu-ui.md §"The presentations' records" reads some of the same
functions' records.

Three tables in the executable, one entry per number:

| table | entries | per number |
|---|---|---|
| `g_BtlShowFunctions` (`0x800B5CA4`) | 90 function addresses, numbers 1 to 0x5A (the next word is `g_BtlEffectResourceNumbers`) | the show; nothing keeps a number from 0x5B to 0x96, 0xFE or 0xFF from reading past the run |
| `g_BtlEffectRows` (`0x800AC43C`) | 0x64 rows of 4 bytes (0x190 bytes to `g_BtlEffectTileRows`) | flags, tile row, two sounds (kinds 0xB, 0x7C and 0x7D use the second) |
| `g_BtlEffectResourceNumbers` (`0x800B5E0C`) | u16, 0xC8 bytes | the LUNADATA resource `BtlEffectLoad` reads |

The party's shows load resources `0x198 + n − 1` (0x198 to 0x1C6); the
item shows 0x3D to 0x42 load 0x1C8 to 0x1CD; effects 0x5B to 0x62 have
resources (0x1D0 to 0x1D7) but no show function: `BtlShowADRun`,
`BtlShowAFRun`, `BtlShowB2Run`, `BtlShowB3Run` and `BtlShow9ARun`
(`0x8003A80C`) call `BtlEffectLoad` for them themselves.

The loader, `BtlEffectLoad(record, n)` (`0x800897DC`) (btl_show_frame.c),
by the flags of the effect's row:

* 1, a sprite block and its pages. The resource's word 0 is the block's
  length in words, word 1 its place in words: inflated, it is a sprite data
  block (`SpriteDataHead`, the same layout as the figures', §"The actor
  renderer"), and the record's `+0x28`, `+0x2C`, `+0x14`, `+0x18` and
  `+0x10` are pointed at its tables; `BtlRecordSpriteDataSet`
  (`0x80089A3C`) makes it ready on animation 0. The record is
  `g_BtlShowRecords + 0x500`, the effect's record. From word 2 on, up to
  three {length, place} pairs: each inflates to a 256-pixel-wide 8-bit page
  of `length >> 6` lines, sent to `g_SpritePageCorners[7..9]` = (576 / 704 /
  832, 256) (`sprite_page_effect_load`); a length of 0 ends the list. The
  texel records in the resource already name pages 7 to 9, so they are not
  patched as the party's are. The block holds the caster's frames as well
  as the effect's: effect 1 is Alex's whole sword technique.
* 2, its own palette. Word 8 is its place: `sprite_palette_load` clears
  entry 0 and sends it to row 506. Which palette a pose uses is the code's
  choice: `BtlRecordSpriteDataSet` sets the effect record's `+0x0E` to
  0x2200 (row 506), `BtlShowOpen` then sets 0x2000 (row 504, the party's),
  and a show that wants the effect's palette sets 0x2200 itself
  (`BtlShow01Run` (`0x8007BE00`) does). An effect with flag 1 alone has no
  palette of its own.
* 4, a background layer. `tile_layer_reset` and `tile_resource_load(n)`
  (tile_layer.c) load the command-list resource `g_TileResourceRows[...]`
  (0x1D8 to 0x1E7) and run it through `resource_command_run`
  (`0x800202FC`): a type-4 palette, type-3 pages and type-18 tile pattern
  frames (`resource_tile_pattern_set` (`0x80020C24`) → `g_TilePatterns[k]`).
  The tile row `g_BtlEffectTileRows[row]` = {pattern list, width, height}
  picks a pattern list, which `BtlTilePatternStep` (`0x8008B21C`) and
  `BtlTilePatternPut` (`0x80089C8C`) step and `tile_pattern_put`
  (`0x8001DB6C`) copies (width/2 × height/2 u16 cells) into the layer's
  map. Nine party shows have one (0x08, 0x0B, 0x1D, 0x1E, 0x27, 0x2A to
  0x2C, 0x2F), and seven enemy effects (0x50 to 0x56). The layer is drawn
  by `tile_layer_draw` (`0x8001D4E0`) (through `tile_layer_step`
  (`0x8001DA78`)): a cell's bit 8 picks the page (`g_TileTPages[4 + bit]`:
  (576 / 704, 256), the effect's pages 7 and 8 for the eleven effects that
  have flag 1 too), the palette is always row 501 (the layer resource's
  type-4 palette), and 0x4000 / 0x8000 mirror and flip (battle-effects.md
  §"Background layers (flag 4)").

The common effects, resource 2 (`sprite_reset` (`0x80018DBC`),
sprite_list.c): a palette for row 507 (16 palettes of sixteen colours,
entry 0 of each cleared), three sprite blocks, `g_SpriteData0` (the heal's
sparkle), `g_SpriteData1` and `g_SpriteData2` (the hit sparks), whose texel
records are patched to page 10 = (960, 0), 4-bit, and their picture at
(960, y). The heals use them (`BtlShowPlacesPlay` (`0x8007F2D8`):
`g_SpriteData0` with palette 0x780), and so does every impact
(`BtlAmountShowStart` (`0x80088EA8`): `g_SpriteData2`, 0x700 / 0x710).

All 64 flag-1 resources inflate to exactly their stated lengths and parse
as sprite data blocks, with their pictures on pages 7, 8 and 9; effect 1
under row 506 shows Alex's technique cleanly. Resource 2's three blocks
parse the same way (133, 4 and 124 poses).

The shows are scripts stepped frame by frame over a pool of 21 show records
of 0x40 bytes at `g_BtlShowRecords` (0 to 13 copy the battle records,
`+0x140` an extra one, `+0x500` the effect's), drawn through the figures'
animation and renderer (`BtlAnimationStep`). `BtlShow01Run` (show 1) is
the pattern: hide the caster and let the effect's record play it; travel
caster → target (`BtlTravelRun` (`0x8008AE10`)); set the animation
(`BtlAnimationSetCopy` (`0x8008B1FC`)); give eleven sounds their frames
(`BtlShowSoundSet(slot, sound, delay, repeat)` (`0x80089EF8`)); step
`BtlAnimationFramesCount` (`0x80088DF4`) frames, one `BtlShowFrameStep`
(`0x80089DDC`) each (the whole battle frame runs inside it); at frame 0x49
a fade (`layer_col_fade_start` (`0x8001EA74`)) and the impact
(`BtlAmountShowStart`); walk back (`BtlEffectRecordBackMove`
(`0x8007BD0C`)), put back, wait on `BtlAmountShowCheck` (`0x8008931C`).

`BtlAmountShowStart` (btl_boss.c) is one function of 0x470 bytes, from
`0x80088EA8` to `0x80089318`; the file btl_show_frame.c begins after it, at
`BtlAmountShowCheck`.

Seven shows draw random numbers, so a cast of one moves every later draw,
and a run that leaves the show out goes another way from the game after it.
A sweep of the call graph four calls deep (station-tools, a custom decomp
toolkit that will be released later) finds no other spell, item or shared
show function (the impact, the travels, the fades, the sound slots, the
show's frame) that reaches `rng_draw` (`0x8001C770`), apart from the battle
frame's roaming of slot 5. Every show runs after its action's amount:
`BtlTargetStat0EHitRun` (`0x80032BDC`) (22), `BtlMarkedStat0EHitRun`
(`0x80032FA0`) (25) and `BtlAreaStat0EHitRun` (`0x80032C64`) (24) call
`BtlShowNumberRun` after `BtlStat0EHitAmountSet` and before the wake and
the falls, which draw nothing. "Marked" below is `g_BtlSlotAmountKinds` as
the action leaves it: `BtlTargetOtherSideMark` (`0x80036394`) sets its slots
to 1 (without clearing first), `BtlTargetHpAmountTake` sets a hit target's
byte to 1, 3 or 4 but never 0, and bytes 14 and 15 of the 16-byte array are
never written. `rand(m, n)` is `rng_mask_below` (no draw when n < 2),
`r7f` is `rng_7f_draw` = `next() & 0x7F`. The shake's draws are made only
when `g_GameOptions[5]` is set, the option byte a new game sets to 1 and
pad bit 2 flips in a battle (`BtlFrameStep`): the vibration option. The
draws are the DualShock's actuator levels, `g_PadActuator1Level += next() &
0x3F` and `g_PadActuatorLevels = next() != 0`, which `pad_update`
(`0x8001F508`) hands the pad library each frame. spell-rng.md has these
shows' draws in full.

| show | spell | script | draws |
|---|---|---|---|
| 0x06 | Dragon Anger | `BtlShow06Run` | 16 meteors, k starting at frame 8k: an even k whose record k/2 + 6 is marked hits it, any other draws rand(0xFF, 0xFF), rand(0x3F, 0x3F). Each landing (frame 8k + 8) starts a 3-frame burst; with the shake each burst frame draws `next()` twice. The order over 0xB0 frames: spawn(0), then for m = 1 to 15 spawn(m) and three bursts, then the last three bursts |
| 0x09 | Thunderbolt | `BtlShow09Run` | five bolts 13 frames apart, each rand(0xF, 6) (the loop's animation length + 0x41 frames always passes the fifth) |
| 0x0B | Thunder Thrust | `BtlShow0BRun` (`0x8007E444`) | records 0 to 13: each one not marked r7f, rand(0x3F, 0x3F) |
| 0x1A | Flameria | `BtlShow1ARun` | records 0 to 15: each one not marked (14 and 15 always) r7f, rand(0x3F, 0x3F) |
| 0x1D | Ice Wall | `BtlShow1DRun` | 48 rounds of four frames (`BtlTilePatternStep(i, 4)`, the background's patterns, `BtlTilePatternPut`); block k starts at round k with rand(0x3F, 0x30), rand(0x1F, 0x1C) and animation rand(3, 3) + 1, and draws all three again whenever its animation reaches its last step (`BtlAnimationPlayCheck` (`0x8008A408`)) before round 0x21, so the count depends on the lengths of the effect block's animations |
| 0x2D | Rock'N'Roll | `BtlShow2DRun` | records 0 to 19: each one that is not a marked 0 to 13 r7f, rand(0x3F, 0x3F); then `total + 0x138` frames (total = 0x40 + six widths each × 4/5 = 232) of one shake `next()` each |
| 0x2F | Flash Wind | `BtlShow2FRun` | ten streaks drawn again with rand(0xFF, 0xD0) at their start and whenever one leaves the screen, while the fade of mask 8 runs and then for a fixed tail: the count depends on the fade's timing |

Enemy attack rows reach three of them through the same functions: row 95
(Thunderbolt, function 22), 97 (Thunder Thrust, 25) and 101 (Rock'N'Roll,
25). Of the bosses' shows in the same table, 0x54 (`BtlShow54Run`
(`0x800876E4`)) draws a number too (§"Shows 0x9F, 0x54 and 0x9C in
detail").

The party's shows. The spell names are those of the spell rows, string
`row + 0x1B6`, not of the show numbers: rows 43 to 46 have shows 0x0D to
0x10. "Pages" are the lines of each page the effect's resource sends.

| show | spell (row) | flags | resource | function | pages | draws |
|---|---|---|---|---|---|---|
| 0x01 | Sword Dance (1) | 03 | 0x198 | `BtlShow01Run` (`0x8007BE00`) | [136] |  |
| 0x02 | Explosion Staff (2) | 03 | 0x199 | `BtlShow02Run` (`0x8007C110`) | [176] |  |
| 0x03 | Flash Cut (3) | 03 | 0x19a | `BtlShow03Run` | [108] |  |
| 0x04 | Vigor (4) | 03 | 0x19b | `BtlShow04Run` | [176] |  |
| 0x05 | Dragon Protect (5) | 03 | 0x19c | `BtlShow05Run` | [256,96] |  |
| 0x06 | Dragon Anger (6) | 03 | 0x19d | `BtlShow06Run` | [256,164] | yes |
| 0x07 | Dragon Healing (7) | 03 | 0x19e | `BtlShow07Run` | [256,100] |  |
| 0x08 | Dragon Grief (8) | 07 | 0x19f | `BtlShow08Run` | [256,252,156] |  |
| 0x09 | Thunderbolt (9) | 03 | 0x1a0 | `BtlShow09Run` | [204] | yes |
| 0x0A | Thunder Bomb (10) | 03 | 0x1a1 | `BtlShow0ARun` | [208] |  |
| 0x0B | Thunder Thrust (11) | 07 | 0x1a2 | `BtlShow0BRun` (`0x8007E444`) | [256,240] | yes |
| 0x0C | Spark Ball (12) | 03 | 0x1a3 | `BtlShow0CRun` | [108] |  |
| 0x0D | Sleep (43) | 03 | 0x1a4 | `BtlShow0DRun` | [108] |  |
| 0x0E | Confusion (44) | 03 | 0x1a5 | `BtlShow0ERun` | [108] |  |
| 0x0F | Magic Box (45) | 03 | 0x1a6 | `BtlShow0FRun` | [108] |  |
| 0x10 | Stone (46) | 03 | 0x1a7 | `BtlShow10Run` | [112] |  |
| 0x11 | Heal Litany (13) | 01 | 0x1a8 | `BtlShow11Run` (`0x8007FB94`) | [32] |  |
| 0x12 | Calm Litany (14) | 01 | 0x1a9 | `BtlShow11Run` (`0x8007FB94`) | [32] |  |
| 0x13 | Saint Litany (15) | 01 | 0x1aa | `BtlShow13Run` (`0x8007FBD8`) | [60] |  |
| 0x14 | Althena Litany (16) | 01 | 0x1ab | `BtlShow14Run` (`0x8007FD34`) | [32] |  |
| 0x15 | Miracle Litany (17) | 01 | 0x1ac | `BtlShow15Run` (`0x8007FD9C`) | [48] |  |
| 0x16 | Cleanse Litany (18) | 03 | 0x1ad | `BtlShow16Run` (`0x8007FDD0`) | [32] |  |
| 0x17 | Fear Litany (19) | 01 | 0x1ae | `BtlShow17Run` | [40] |  |
| 0x18 | Flame Bomb (21) | 03 | 0x1af | `BtlShow18Run` | [148] |  |
| 0x19 | Flame Circle (22) | 03 | 0x1b0 | `BtlShow19Run` | [244,248,156] |  |
| 0x1A | Flameria (23) | 03 | 0x1b1 | `BtlShow1ARun` | [180] | yes |
| 0x1B | Power Drive (24) | 03 | 0x1b2 | `BtlShow1BRun` | [204] |  |
| 0x1C | Ice Lance (25) | 03 | 0x1b3 | `BtlShow1CRun` | [256,32] |  |
| 0x1D | Ice Wall (26) | 07 | 0x1b4 | `BtlShow1DRun` | [256,24] | yes |
| 0x1E | Blizzard (27) | 05 | 0x1b5 | `BtlShow1ERun` | [256,224] |  |
| 0x1F | Ice Shell (28) | 03 | 0x1b6 | `BtlShow1BRun` | [204] |  |
| 0x20 | Power Slash (29) | 01 | 0x1b7 | `BtlShow20Run` | [92] |  |
| 0x21 | Power Sweep (30) | 03 | 0x1b8 | `BtlShow21Run` | [144] |  |
| 0x22 | Sonic Riser (31) | 03 | 0x1b9 | `BtlShow22Run` | [108] |  |
| 0x23 | Power Up (32) | 01 | 0x1ba | `BtlShow23Run` | [88] |  |
| 0x24 | Healing Song (33) | 01 | 0x1bb | `BtlShow11Run` (`0x8007FB94`) | [32] |  |
| 0x25 | Purity Song (34) | 03 | 0x1bc | `BtlShow16Run` (`0x8007FDD0`) | [32] |  |
| 0x26 | Cascade Song (35) | 03 | 0x1bd | `BtlShow1BRun` | [196] |  |
| 0x27 | Escape Song (36) | 05 | 0x1be | `BtlShow27Run` | [256,64] |  |
| 0x28 | Temptation Song (37) | 03 | 0x1bf | `BtlShow28Run` | [96] |  |
| 0x29 | Tranquil Song (38) | 01 | 0x1c0 | `BtlShow11Run` (`0x8007FB94`) | [32] |  |
| 0x2A | Inferno (39) | 07 | 0x1c1 | `BtlShow2ARun` | [256,256,136] |  |
| 0x2B | Nitro Dagger (40) | 05 | 0x1c2 | `BtlShow2BRun` | [256,164] |  |
| 0x2C | Tornado (41) | 07 | 0x1c3 | `BtlShow2CRun` | [256,172] |  |
| 0x2D | Rock'N'Roll (42) | 03 | 0x1c4 | `BtlShow2DRun` | [96] | yes |
| 0x2E | Flash Arrow (47) | 03 | 0x1c5 | `BtlShow2ERun` | [192,244] |  |
| 0x2F | Flash Wind (48) | 07 | 0x1c6 | `BtlShow2FRun` | [256,224] | yes |

### Eight of the party's shows

None of these draws a random number: their only draws are those of the
frame they wait in, `BtlShowFrameStep`. `BtlTilePatternStep(seed, n)` here
steps the background's patterns with a carried counter; it draws nothing.
The words they share: the vibration list is the actuators' command list
`g_PadActuatorList` (`g_BtlActuatorFFList` is the usual one), set with
`g_PadActuatorRestFlags = 0x3F` and `g_PadActuatorListWait = 0` (§"The vibration scripts"); the colour pulse `g_BtlShowPulse` fades the
layers of mask 0xC between its two colours (`colour_1` at `+0x0C`,
`colour_2` at `+0x12`) while its `on` word is set; the fade of mask 8 is
`layer_mix_fade_start(8, 1, speed, level)` (`0x8001EBA4`) with
`layer_mix_fade_check(8)` (`0x8001ED94`) for busy; a travel is a
`BtlTravelBlock` on the stack (`mode`, `step` the pixels a point); the
records are the 0x40-byte show records (`+6` x, `+8` depth, `+0xA` y, `+0xC`
facing, bit 0x2000 mirrors, `+0xE` the draw word, `+0x20` scaled, `+0x24` /
`+0x26` the scale, `+0x3C` width, `+0x3E` height).

* 0x03 Flash Cut (`BtlShow03Run`). The effect's record is a cursor that
  sweeps the screen and slashes every target. `mode` = 6 when the caster's
  id is 0xB, else 0. It sweeps right from x 0x10 (up to 0x130) when the
  effect record's x is below 0xA0, else left from 0x130.
  `BtlEffectRecordMove(mode, 0x38, x0, 0x88)` (`0x8007BBFC`) opens the
  effect. Each marked record becomes a {key, y} pair: key = its leading
  edge (x ∓ width/2), held to 0x10 to 0x130 and rounded toward zero to a
  multiple of 8; `pair_list_sort` (`0x8007C2C4`) sorts the pairs along the
  sweep. Over 0x24 counted frames the cursor blinks and moves 8 pixels a
  frame; a frame on which it reaches the next key is a hit and is not
  counted: the layers of mask 0xC are set to (0xA0, 0xA0, 0xA0) and fade
  back over 2 frames, the vibration
  list becomes `g_BtlActuatorC8List`, sound 0x39 plays on slot j, and the
  cursor jumps to the target's y. Then the impact `BtlAmountShowStart(1)`
  and sound 0x7B; every target gets a copy of the effect's record aimed at
  it (`BtlRecordPlaceCopy` (`0x80089BD0`)) playing animation 5, and the last
  target's copy plays `mode + 5`. The original leaves `last` unset when
  nothing is targeted, and the C keeps that.
* 0x17 Fear Litany (`BtlShow17Run`). A travel (mode 0, step 2) from the
  caster (0, −height) to the target (0, −height/2) makes two paths
  (`BtlTravelPathsMake` (`0x8008AA3C`), which returns n). For n + 0x48
  frames, nine copies of the effect's record start 8 frames apart
  (animation k % 3 + 1) and ride the same path, a trail of nine. When the
  target's `g_BtlSlotAmountKinds` is 1 (it was taken): the vibration list
  `g_BtlActuatorToggleList`, the target gets facing 0x2000 and it and the
  trail slide left 8 pixels a frame for (x + width)/8 + 1 frames, off the
  screen, and the target is hidden. Otherwise the sprites sink 2 pixels a
  frame and vanish 8 frames apart.
* 0x19 Flame Circle (`BtlShow19Run`). The colour pulse is given the
  colours (0xA0, 0x3C, −0x28) and (0x50, 3, −0x14), the layers fade to the
  first over 0x32 frames, and the pulse is switched on; the effect's record becomes the target's picture, placed
  above its head (y − max(height/4, 0x10)), and plays animation 1 for its
  length × 3/2 frames, blinking (`+4 = i & 1`); at frame 0x40 the pulse
  is switched off and the layers are set at once to its second colour. Animation 2 then plays
  at the target's y for its length × 2 frames, with the impact at frame
  0x10.
* 0x20 Power Slash (`BtlShow20Run`). The caster is hidden and the effect's
  record travels to the target (mode 1, step 8, width/2 beside both ends;
  the vibration list `g_BtlActuatorC8LateList`); the layers are set to
  (0x78, 0x78, 0x78) and fade back over 0x10 frames, the impact plays, and when the record did not end on the caster's x and
  y it is moved back (`BtlRecordSpotMove` (`0x8008AFAC`), sound 7 on slot
  6).
* 0x22 Sonic Riser (`BtlShow22Run`). `BtlEffectRecordMove(5, 0x3F, x,
  target y)`, x = 0x10 or 0x130 by the effect record's side; the record is
  copied into a trail of 20 stepping 0x10 pixels against its facing. Over
  0x2B frames copy k turns on at frame k (animation 2), then animations 3
  and 4 (at d = 1 and 0x15) and clears at d = 0x17; frame 0x15 is the
  impact. The vibration lists `g_BtlActuatorFFTwiceList` and then
  `g_BtlActuatorC8List`.
* 0x27 Escape Song (`BtlShow27Run`). The pattern table
  `g_BtlShow27PatternList` = 01 02 03 04 05 04 03 02 FF. The fade of mask 8
  is aimed at the caster and pulsed (levels 8 / 0xF) for nine steps; then
  three loops step the background's patterns through the table
  (`BtlTilePatternSizePut`, `tile_pattern_put` and one frame) while the
  fade flashes six times and settles. Every enemy (slots 6 to 13) whose
  `g_BtlSlotAmountKinds` is 1 gets facing 0x2000 and walks off the left
  edge 8 pixels a frame for max((x + width)/8, 6) frames, then is hidden:
  the escape.
* 0x2B Nitro Dagger (`BtlShow2BRun`). Two tables: animations {2, 3, 2, 1}
  by direction (`g_BtlShow2BAnimations`) and the eight edge points of a
  320×224 screen (`g_BtlShow2BPoints`). Eight records get a 16-point path
  from the centre (160, 112) to edge point i (`line_path_make`
  (`0x80054C9C`)) and are doubled (+8); records 0, 3 and 4 are mirrored.
  Sound 0x7B on slots 1 to 4 at delays 1, 5, 9 and 13 (repeat 0x10), 0x79 on
  slot 5. For 168 frames, 16 daggers in two waves (d = f − 4j, the second
  wave 8 frames later) fly their direction's path six times (every 16
  frames), scaled by `g_BtlShowScales[7 − (d%16)/2]` × 3/2, while the fade
  of mask 8 pulses every 8 frames. Then the impact and the end
  `BtlShowActingThreeIdsEnd` (`0x80082D1C`).
* 0x2C Tornado (`BtlShow2CRun`). For each target: a 32-point spiral from
  its place round (0xA0, 0x90), each point a quarter turn on (up right →
  up left → down left → down right) at 29/32 of the last radius
  (`ratio_29_32_mul` (`0x800839C0`) = (v·29 + 16) >> 5), made into a smooth
  curve of 64 points (below). The targets walk a 32-step line onto the
  spiral, ride the curve for 63 frames, rise 992 pixels off the top over 32
  frames, then fall back onto the landing places `g_BtlShow2CPoints[k]`
  (200 pixels in 5 steps of 2 × 20) and bounce 48, 24, 12, 6 and 3 pixels. A
  target wider than 0x40 has its copy scaled by 0x4000 / width.

Tornado's path is a natural cubic spline in 16.16 fixed point
(mth_curv.c; the developers' file name, MTH_CURV.C, is in `MTH_Div`'s
assertion). `MTH_CurvePointsMake(work, pts, n, count, out)` (`0x8001CFB0`)
refuses n < 3 or count < 2 (`MTH_CurveCountsCheck` (`0x8001CF1C`)), puts the
work block's nine scratch arrays in `g_MthH` to `D_800B89A0`
(`MTH_CurveWorkSet` (`0x8001CF40`), which checks `work[0] >= n`; Tornado's
work block is `g_BtlShow2CCurveWorkBlock`), splits the points into x and y
arrays, builds the spline (`MTH_maketable2` (`0x8001CD50`): the knots are
the running sum of the chord lengths from the fixed-point hypotenuse
`MTH_Hypot` (`0x8001C838`), scaled to 0..1; the moments from
`MTH_maketable` (`0x8001C9BC`)) and takes `count` points at t = i/(count −
1) through `MTH_spline2` (`0x8001CE7C`) (x and y each through `MTH_spline`
(`0x8001CC10`): a binary search for the interval, then the cubic).
`MTH_maketable` is the tridiagonal solve: h[i] = t[i+1] − t[i], the
slopes, a forward pass r = h[i]/diag[i], m[i+1] = (s[i+2] − s[i+1]) −
m[i]·r, then substitution back, with m[0] = m[n−1] = 0. `MTH_Mul`
(`0x8001D110`) is the 16.16 multiply (a·b) >> 16 through a 64-bit product;
the divide is `MTH_Div` (§"Tornado's divide — `MTH_Div`").
`MTH_maketable`, `MTH_spline`, `MTH_maketable2`, `MTH_spline2` and the
square root `MTH_mysqrt` (`0x8001C7B0`) are published routines from a book
of algorithms in C, written over in 16.16 fixed point; the register's rows
name the source (`src:public-source`).

### Item and enemy-action presentations

* `BtlEffectCopyAboveShotRun(n)` (`0x80085D2C`) (btl_item.c) is the item
  show behind `BtlShow3ERun` and `BtlShow41Run`. The acting slot plays
  animation 0x20 to its end (`BtlShowActingPlay`) and goes to rest. Show
  record 0, a copy of the effect's record with draw word 0x2200, appears
  above the user (y − (height + 0x10)). The callers put three sounds in
  the show sound slots `g_BtlShowSounds`; the animations are fixed: record
  0 plays animation 0 with slot 0's sound, then slot 1's, travels (mode 0,
  step 0x10) down to the target's head (a quarter of its height up, at
  least 0x10) on animation 1, and plays animation 2 at the target with slot
  2's sound. Before animation 2, `BtlMixColNumberSet(n)` (`0x8008B27C`)
  sets the mix colour, the target is put in mix group 2 (0x800 in bits
  0x1C00 of `+0xE`), and `layer_mix_fade_start(0x400, 1, 8, 0x14)` is
  called, which does nothing for that mask. `BtlAmountShowRun` ends it.
  `BtlShowAnimationPlay(record, animation, x)` (`0x8008A344`) sets the
  animation (unless 0xFF), then waits its length
  (`BtlAnimationFramesCount`) in frames. battle-presentations-2.md, "Group E", has the six item shows 0x3D to 0x42.
* `BtlShow42Run` (`0x80086220`) (btl_weapon.c): two show records, a
  mirrored pair either side of the target (x ∓ (0x2A + d), d = max(height/4,
  16)), d above it at depth `+0x0A` + 1, close in 1 pixel every 8 frames
  while the target shakes ±2 pixels (not when its id is 0x77); then the mix
  colour of 0x97 and the same mix group, `BtlShowSoundStop(0)`
  (`0x80089F78`) (which stops slot 0, not the slot the show's sound is on),
  animation 1 and a faster close (4 pixels a frame for 8 frames).
* `BtlActingShowRun(n)` (`0x8003A158`) (btl_acting.c) runs the enemies'
  shows, 0x97 and up. Its callers are `BtlAttackMissShow` (`0x80031378`)
  and `BtlAttackHitShow` (`0x800314B8`), which pass the acting slot's
  `attack_show` (stats `+0x17`), and `BtlShowNumberRun`. It fills the shot
  block `g_BtlShotBlock` (`BtlShotSet` (`0x80040F14`)), sets the action's
  sound `g_BtlActionSound` from stats `+0x19` (kind 2) or `+0x1F` (any
  other kind), shows the action's name (`BtlActionStringDisp`
  (`0x8004987C`); for kind 7 of id 0x4F the name comes from `+0x1D`, and
  `g_BtlActionNumber` is put back from `g_BtlSlotPlannedNumbers` after),
  plays a voice for slot 6 in battles 0xF5, 0xF6, 0xF7 and 0xFE (0x3A, 0x3B,
  0x38 and 0x38; `VoiceSet` (`0x80055470`), `VoiceStart` (`0x800554C4`),
  `VoiceStep` (`0x80055524`)), and calls
  `g_BtlActingShowFunctions[(n + 0x69) & 0xFF]`, entry `n − 0x97`, with no
  test of the range: the 38 functions for shows 0x97 to 0xBC (entry 0
  `BtlShow97Run`, entry 3 `BtlShow9ARun`). §"The enemy actions"
  has all 38.
* `BtlShow9ARun` (`0x8003A80C`) is enemy action 0x9A's special attack. The
  acting slot faces the target and winds up by its id (0x1E; 0x3A, 0x65,
  0x66; 0x3D; id 0x7A changes form by loading effect 0x62 through
  `BtlEffectLoad` over its record, plays sound 0x4F and makes the action's
  sound 0x5A), a shot (`BtlExtraRecordsAlloc(1)` (`0x8003A340`) into
  `g_BtlExtraRecords`) is placed on the target, the vibration runs for 20
  frames, the impact plays (not for 0x3D), and the shot is freed
  (`BtlExtraRecordsFree(1)` (`0x8003A388`)); 0x7A changes back at the end.
* `g_BtlActingShowFunctions` and the run at `0x800B2EF8`. The 38
  addresses are entries 94 to 131 of the run of 132 function addresses
  that begins at `0x800B2EF8` (entries 0 to 93 are the functions of the
  action rows, §"The row runner"). The C still names entry 101 of the run
  `field_state_table` (`0x800B308C`), a label that came with the first
  split and was never read; it is entry 7 of `g_BtlActingShowFunctions`,
  the function of show 0x9E. `BtlShowA6Run` is entry 15 (0xA6) and
  `BtlShowBARun` entry 35 (0xBA). The shows share a way of working: the
  acting slot faces its target (`+0x0C` = 0x20 / 0x2020 by x, as in
  `BtlShow9ARun`), a copy of the acting record goes into the extra record
  `g_BtlExtraRecords`, `line_path_make(buf, x0, y0, x1, y1, n)` fills an
  n-point path, `BtlFrameStep` runs one frame, the vibration list is set,
  and `BtlAmountShowStart(mode)` is the impact.
* `BtlShowA6Run` is action 0xA6: the enemy throws a copy of itself.
  `BtlExtraRecordsAlloc(1)`, the acting record copied into
  `g_BtlExtraRecords`, hidden. By the acting slot's id: 0x3B dx 0x20, dz
  0x32, animation 2; 0x30 dx 2, dz 2, animation 8. Any other id leaves the
  animation unset (the original reads a register it never set). The copy
  plays animation 0xB, is moved dx toward the target and up dz, and a path
  of max(|dx|, |dz|) / 8 points (at least 1) is made in an 80-entry buffer
  on the stack. Sound 0x6E, the acting record on animation 9; id 0x3B waits
  out the animation, other ids 10 frames. The vibration list
  `g_BtlActuatorC8List`, the copy shown, then it flies one point a frame at
  y = z + dz + 6. When the acting record's step `+0x38` reaches `(+0x3A &
  0x7FFF) − 1` it switches to the animation of its id, once. Sound 0x23,
  `BtlPartyHitAnimationSet` (`0x80030E78`), the impact
  `BtlAmountShowStart(0)`. Id 0x3B makes the path again from the acting
  record's hand (x ∓ 0x20 by the mirror bit 0x2000, z − 0x2E). The copy
  flies back the other way at y = z + 1, is freed
  (`BtlExtraRecordsFree(1)`), and id 0x3B plays animation 0xA.
* `BtlShowBARun` is action 0xBA: the enemy splits into five copies, one
  per party slot. `g_BtlExtraRecords = xmalloc_heap_alloc(0x140)` (five
  records of 0x40 bytes), the path buffer `g_LoadBuffer` (40 points a
  slot), and `BtlFacingPartySet` (`0x800416A4`) turns the acting record
  toward the party. The start is the acting record moved x ∓ 8 by the
  mirror bit and z − 0x60. The path's length is the largest |dx|/8, |dz|/8
  over the active slots 0 to 4. Sound 0x6E, the acting record on animation
  8 (waited out), the vibration list `g_BtlActuatorRandomList`. All five
  copies are set from the acting record and `g_BtlExtraRecordCount = 5`
  (the count `BtlExtraRecordsStep` (`0x8004163C`) steps from
  `g_BtlExtraRecords` into sprite entries 0x1C on); copies for empty slots
  are hidden, the rest play animation 0xC and fly one point a frame at y = z
  + 1. On arrival: the vibration list `g_BtlActuatorFFList`, animation 0xD,
  sound 0x5A, 16 frames, the impact `BtlAmountShowStart(1)`, and a wait for
  the last copy's animation. Then `g_BtlExtraRecordCount = 0`,
  `sprite_run_off(0x1C, 5)` clears the five sprite entries (the 0x24-byte
  entries of `g_SpriteList`), `BtlAmountShowWait` (`0x80031028`) runs frames
  until `BtlAmountShowCheck` answers 0 (the effects are done), and
  `xmalloc_heap_free` (`0x80022F88`) frees the copies.
## Confusion — status 0x40

Status 0x40 makes a slot's plan and its turn's target random.

* **Inflicting it.** Effect 49 (`BtlTargetAreaStatus40TryRun` (`0x8003390C`)): `BtlTargetAreaMark(1)` (`0x80035B50`) marks the area round
  the target; every marked slot's number kind is set to 2, and unless its
  status holds a bit of 0x3D the status roll `BtlStatusTakeCheck` (`0x80035378`) (it draws) decides `BtlTargetStatus40Set` (`0x8002FC08`).
  That stores status 0x40 and zeroes the slot's `g_BtlSlotStatus40Rounds`,
  unless `BtlTargetTraitCheck(0x1000, 0x25, 0x26, 0)` (`0x80037648`) answers 1 (the target holds 0x25
  or 0x26 among its traits, or, past slot 4, bit 0x1000 in its `+0x2A`
  word). Then the action's show
  and the status animations of the slots left at number kind 7. On a hit:
  `BtlHitStatusUpdate` (`0x80030A04`) calls `BtlTargetStatus40Set` (`0x8002FC08`) when the attacker's trait byte
  (`+0x28` of a party member's stats, `+0x1B` of another's) is 4, 11 or
  18, on a draw below 0x20, 0x40 or 0x80 out of 0x80 by which of the three it is.
* **The party's plan — `BtlPartyActionAdjust` (`0x80029348`)** (called by `BtlRun` (`0x80027434`) after
  the commands, and after an escape try and its failed-escape move, before
  the round's turns): each active party slot holding 0x40 draws r = `rng_draw_copy()` (`0x8001C790`); r & 7, or
  r & 3 when that is 5 or more: 0 attack (kind 2); 1 a spell (kind 3) when `BtlActingSpellPick` (`0x800393F0`)
  finds a row, else attack; 2 an item (kind 4) when `BtlActingItemPick` (`0x800396F4`) finds one of the
  items 0x9C to 0x9F and 0xA1 to 0xA3 among the six the member carries,
  else attack; 3 kind 5, a walk (`BtlActionWalkRun` (`0x8002E5D4`)); 4 kind 6, run. Its planned
  target becomes 0x64. An active slot without status 0x40 whose
  `g_BtlPartyNotHeldFlags` byte is 0 gets kind 0xB and target 0.
* **The enemies' plan — `BtlEnemyActionAdjust` (`0x80029510`)**: an active slot 6..13 holding 0x40 draws
  `rng_draw() & 3` (`0x8001C770`): 0 / 1 `BtlEnemyActionFallbackSet` (`0x80036A18`), 2 kind 5 (a walk), 3 kind 6
  (run: `BtlActionKindRun` (`0x8002E12C`) lets a slot of 5 or more leave without a draw, so a
  confused enemy may leave the field). `BtlEnemyActionFallbackSet` (`0x80036A18`) sends the
  six ids 0x10 0x18 0x1F 0x2E 0x44 0x55 to `BtlActionWalkCornerSet(1)` (`0x80039B70`) (kind 5, a walk to a
  corner of the field chosen by where the party's slots stand); any other
  enemy whose plan is not already kind 2 attacks, when confused, the
  nearest active slot of either side, slot 5 and itself excepted
  (`BtlSlotNearestFind(0)` (`0x80039838`); unconfused, the nearest party member,
  `BtlSlotNearestFind(2)`).
* **The turn — `BtlActionAdjust` (`0x8002E914`) → `BtlActionStatus40Adjust` (`0x8002E9CC`)**, which replaces
  `BtlActionTargetAdjust` (`0x8002ED34`)'s re-validation when the acting slot's status has 0x40:
  a party member first loses the turn on `rng_7f_draw() < 8` (`0x8003A0B4`) (kind
  0xA: animation 0x1D, `BtlAnimationEndWait` (`0x8002FDBC`), the rest pose — `BtlActionKindRun` (`0x8002E12C`)
  case 8). Then by kind: 2 the nearest slot of either side; 3 a fresh
  `BtlActingSpellPick` (`0x800393F0`) row, targeting `BtlSlotActivePick(1)` (`0x80039F9C`) (any active slot but 5
  and the caster, `rng_list_pick(list, n, 15)` (`0x8003A03C`)) when the row's
  `+0x01 & 0xF` (its battle target mode) is set, else the caster, whom
  `BtlActingSpellPick` (`0x800393F0`) made the target; no row: attack the nearest; 4 a fresh
  `BtlActingItemPick` (`0x800396F4`) item, targeting `BtlSlotActivePick(0)` (`0x80039F9C`) (the user may be
  picked too) when the low four bits of the item row's `+6` are set; none
  carried: attack the nearest; 7 `BtlEnemyActionFallbackSet` (`0x80036A18`); 5
  `BtlActionWalkCornerSet(0)` (`0x80039B70`) (a corner chosen by where all the other slots
  stand); 6 runs.
* **`BtlActingSpellPick` (`0x800393F0`)** — none under status 0x80; else a fixed candidate list
  per character id (1/11: rows 1-4; 2: 9-12, 0x2B-0x2E; 3: 0x0D 0x0E 0x10
  0x12; 4: 0x15-0x17 0x19-0x1B; 5: 0x1D-0x20; 6: 0x21-0x23 0x25 0x26; 8:
  0x27-0x2A; 10: 0x2F; 7 and 9 none), filtered by `+0x00` (learn level) ≤
  the caster's level and `+0x04` (cost) ≤ its MP (MP doubled + 1 when
  `BtlTargetTraitCheck(0, 0x1B, 0, 0)` (`0x80037648`) matches, the trait that halves the cost),
  one drawn by `rng_list_pick` (`0x8003A03C`) with the fixed list's length as its mask
  (`rng_draw_copy() & n`, less the kept count until below it). It makes the
  caster the target. The list is the game's, not the learned one: a confused
  member can cast rows it has not learned yet, if its level allows.
* **Targeting while confused.** The acting slot's status bit 0x40
  (`g_BtlActingStatus`, or `+0x15` of its stats) widens `BtlTargetOtherSideMark` (`0x80036394`),
  `BtlAttackTargetFind` (`0x800317E0`)'s re-pick of a target that is no longer active and `BtlAttackAllRun` (`0x800318DC`) to
  the whole field (minus the actor), turns `BtlTargetAreaMark` (`0x80035B50`)'s area to
  the actor's own side, widens the row ahead that `BtlTargetFrontMark` (`0x8003568C`) marks
  (effect 20, `BtlFrontHitRun` (`0x80032B44`)) to both sides, and moves `BtlTargetPartyMark` (`0x80036294`)
  (effect 7's set, slots 0 to 4) to slots 6 to 13: a confused party healer
  heals the enemies.
* **Effects 7 and 11** (reached by confused casts of rows 14/38 and 18/34):
  7 (`BtlPartyHpShareAddRun` (`0x800320C8`)) is the heal formula over `BtlTargetPartyMark` (`0x80036294`)'s
  set (§"Effect 7, the heal-all" has the exact formula, whose maximum is not
  each target's own); 11 (`BtlTargetStatusAllClearRun` (`0x800323B8`)) flags the target
  active, clears a party target's `+0x1E` bit 0x04 and every status bit
  (`BtlTargetStatusClearRun(0xFF)` (`0x80032440`)), restoring no HP.

A confused cast of an element 5-7 row at a party target (Sword Dance at an
ally) reaches `BtlTargetTraitCheck` (`0x80037648`) with two ids that `BtlHitAmountSet` (`0x80030070`) never
sets on those elements: the C gives `e1` and `e2` a value only for
elements 1 to 4, so they hold what the callers left. §"The magic element
path" traces what they hold and when a party target reacts to them.

## The battle frame's per-frame systems

* **`BtlFrameStep` (`0x80038D2C`), the frame, in order:**
  - Input toggles: `g_PadPressedButtons` equal to 2 flips `g_GameOptions[5]` and switches
    `g_PadActuatorList` to the EXE table `g_BtlActuatorFFList` (with `g_PadActuatorRestFlags` =
    0x3F) or to 0; equal to 1 flips `[6]`. Each calls `SoundPlay(3 − flag, 1)` (`0x800555F0`).
  - The auto-battle cancel: when `g_BtlAutoEndAllowed` is set and
    `g_BtlPartyMode` is 1, `BtlPadHeldCheck(0x10)` (`0x80037604`) answering 1 sets
    `g_BtlPartyMode` to 0 and closes the window-1 icon (`BtlWindow1IconClose` (`0x800497D4`)).
  - Slot 5's roaming, `BtlSlot5RoamStep` (`0x80038EE8`), when `g_BtlSlotIds[5]` is 12.
  - Then the mix flash `BtlMixFlashStep` (`0x8008A0C0`), the animation tick
    `BtlAnimationStep(g_BtlRecords, 0, 0x1C)` (`0x800272A8`), the extra records
    `BtlExtraRecordsStep` (`0x8004163C`), the amount records `BtlAmountRecordsStep` (`0x8004AF94`), the menu
    outlines and the play-time clock `BtlMenuOutlineStep` (`0x800489F0`), the show's voice
    `BtlShowVoiceStep` (`0x80089D78`), the arena draw `BtlFrameDraw` (`0x80018564`), the tile layer and its fade
    `tile_layer_step` (`0x8001DA78`), and the music-volume fade `sound_music_level_step` (`0x800245F0`).
* **The mix flash, `BtlMixFlashSet` (`0x8008A004`) and `BtlMixFlashStep` (`0x8008A0C0`).** Its state is
  `g_BtlMixFlash` (`BtlMixFlash` in `btl.h`): +0 mode, +1 on, +4 the layer
  mask, +8 count, +9 frame. `BtlMixFlashSet(mask, mode)` (`0x8008A004`) with mode 0 only
  clears the mode; otherwise it stores the mode, on = 1 and the mask, sets
  the count to 8 for modes 1 and 3, 0xA for 2 and 0 for 5, zeroes the
  frame and calls `layer_mix_set(mask, 0x14)` (`0x8001ECA0`). Each frame `BtlMixFlashStep` (`0x8008A0C0`)
  does nothing while the mode is 0; otherwise it adds 1 to
  `g_BtlAmountShowFrameCount` and by mode:
  - **Mode 2, the hit shake:** while the frame is below the count (10
    frames), every slot whose number kind `g_BtlSlotAmountKinds[k]` is 1, 3
    or 4 (hp taken, doubled, halved) moves its x (+6) by
    `g_BtlMixFlashXOffsets[frame]`. That EXE table holds 12, −24, 23, −22,
    21, −20, 19, −18, 17, −8, which add up to 0.
  - **Modes 1 and 3, the blink:** every count frames the mask's layers
    switch between `layer_mix_set` (`0x8001ECA0`) 0x14 (shown) and 0xFF (none), the count
    halving at each show; at a count of 1 they stay shown.
  - **Mode 5, the glow cycle:** `sprite_mix_col_set_copy(g_BtlMixFlashCols[frame % 6])` (`0x8008B368`), six BGR555
    colours, the count going up by 1 on every sixth frame.
    `sprite_mix_col_set_copy` (`0x8008B368`) only hands its argument to `sprite_mix_col_set` (`0x8001A904`),
    which stores it in `g_SpriteMixCol`: the colour of the sprites' mix,
    the glow colour of the layer-2 actors (§"The screen effects": CLUT row
    509), not a screen tint.
* **Amount records, `BtlAmountRecordsStep` (`0x8004AF94`)** (while `g_BtlAmountRecordsRunning` is
  set: `BtlAmountShowStart` (`0x80088EA8`) and `BtlAmountRecordsRun` (`0x8004AE0C`) set it, `BtlAmountShowCheck` (`0x8008931C`)
  clears it). The 13 records at `*g_BtlAmountRecords`, 0x40 bytes each, show
  each slot's amount as digits and use the actor record's animation fields.
  - Record k belongs to slot k for k below 5 and to slot k + 1 from
    record 5 on. Slot 5, the companion, has none.
  - They draw as sprite-list entries 0x72 to 0x7E, with pose 0.
  - A new step sets the record's +0x08/+0x0A to the slot's record's y +
    `g_BtlAmountYOffsets[n]` − record +0x3E, n being the first word of the
    step's row.
  - At the animation's end, `BtlAmountRecordOff(k)` (`0x8004ADA8`) switches the record off.
* **Menu outlines, `BtlMenuOutlineStep` (`0x800489F0`) → `OutlineScreenStep` (`0x8001BE30`).** Outlines,
  26 by 26, round records 2 and 3 of the battle menu's records
  `*g_BtlMenuMarkRecords` (record 2 is put round the icon under the menu's
  cursor). A live record whose `+0x30` bit 0 is clear steps its frame
  counter `g_OutlineScreenFrameCounts[n]` (0..0x29), and every live record
  draws kind `g_OutlineScreenKinds[n]` at frame counter / 7. The kinds are
  1 in the EXE. The pass ends with the play-time clock `PlayTimeStep` (`0x8001C064`):
  `g_PlayTimeMinutes` minutes, one per 3,600 frames, going round to 0
  after 59,999.
* **Fades.** `tile_mix_fade_step` (`0x8001DDFC`) steps a timed fade over `g_TileMixFadeLength`
  frames (the level is linear in time) into `tile_mix_set(level)` (`0x8001DD38`).
  That sets `g_TileSemiTransOn` (on below 0x1F), the packed colour `g_TileCol`
  (level × 4 per channel), and the arena brightness `g_BtlBgGrey` =
  0x80 − level × 4, which is 0x80 in the EXE; from level 0x1F on the grey
  is 0x80 (the arena's 0) and semi-transparency is off.
  `sound_music_level_step` (`0x800245F0`) does the same for the music volume through
  `sound_music_volume_set` (`0x80023C4C`): SsSeqSetVol at (v × 0x50 / 0xFF, clamped to 0x50).

## The spell-effect machinery

The helpers almost every spell presentation calls. The records they move
are the battle's 0x40-byte records: the slots' (`g_BtlRecords`) and the
show's (`g_BtlShowRecords`, 0x15 records; record 20, at `+0x500`, is the
effect's record).

* **Animation length.** `BtlAnimationFramesCount(record)` (`0x80088DF4`) copies the record to the
  stack and runs its animation to the end: tick `+0x34` against
  duration `+0x36`, step `+0x38` up to `+0x3A & 0x7FFF`, per-step
  durations from the table at `+0x2C` (entry `+2`). It returns the frame
  count, which is how many `BtlShowFrameStep` (`0x80089DDC`) frames a presentation waits.
* **The impact.** `BtlAmountShowStart(mode)` (`0x80088EA8`) sets the glow colour and starts
  the mix flash by its `mode` argument: mode != 0 `sprite_mix_col_set_copy(0x1F)` (`0x8008B368`) (red) and
  `BtlMixFlashSet(0x400, 3)` (`0x8008A004`) (the blink) when slot 6's id `g_BtlSlotIds[6]` is 0x77,
  or 0x7C with slot 7's hp (`CharStats + 0x1A4`) not 0, else `(0x400, 2)`
  (the shake); mode 0 `sprite_mix_col_set_copy(0x7FFF)` (`0x8008B368`) (white) and mode 1 (the
  blink). Then `layer_mix_switch_on(0x100)` (`0x8001F4B8`), clearing `g_BtlAmountShowLongestRecord`,
  `g_BtlAmountShowLength`, `g_BtlAmountShowFrameCount`. For each slot i in
  0..13 except 5 (k = its record number, i, or i − 1 from slot 6 on) it
  calls `BtlAmountRecordSet(i)` (`0x8004AE8C`), which starts or switches off the slot's
  amount record by its number kind. An amount record that is on
  (`g_BtlAmountRecords + k * 0x40`) sets `g_BtlAmountRecordsRunning = 1`,
  and the one with the longest `BtlAnimationFramesCount` (`0x80088DF4`) length becomes the
  pair `g_BtlAmountShowLongestRecord`/`g_BtlAmountShowLength`. Then it
  switches on `g_BtlSlotAmountKinds[i]`, with the slot's own show record at
  `g_BtlRecords + 0x3C0 + k * 0x40` (records 15 to 27) made from the
  sprite bank `g_SpriteData2` by `BtlRecordSpriteDataSet(record, bank, palette word, animation)` (`0x80089A3C`):
  - 6: sound 0x62 at once on show sound slot 0xB; palette word 0x720,
    animation 5;
  - 8 / 9: palette word 0x7C0 / 0x7B0, animation kind − 1, and the
    record's +8 (its depth) = 0x1000;
  - 1 / 3 / 4: a slot below 5 first puts its own record on animation
    `g_BtlPartyHitAnimations[i]`. When slot 6's id is 0x7C, slot 7 instead
    stops the mix flash (`BtlMixFlashSet(0x400, 0)` (`0x8008A004`)), plays sound 0x62 on
    show sound slot 0xA and copies record 6 into its show record, on
    animation 0xD and placed at record 6. With mode != 0 the slot's record
    only takes display bits 0x4800 (mask 0xCBFF) and no show record starts.
    Otherwise palette word 0x710 on animation 1 (with those bits on the
    slot's record) when `g_BtlSlotStrongMarks[i]` is set, else 0x700 on
    animation 0;
  - any other kind: no show record.

  Each started show record is set +0 = 1, +0xC &= 0xFF9F and put at its
  slot's record with `BtlRecordPlaceCopy` (`0x80089BD0`); the longest animation is kept.
* **Caster → target pan.** `BtlTravelPathsMake(req)` (`0x8008AA3C`) builds the two paths of the
  travel block (`BtlTravelBlock` in `btl.h`) with `line_path_make` (`0x80054C9C`):
  `+0x14` the path the traveller takes, `+0x18` a straight path from the
  traveller's own place to the target's, whose y + 1 becomes the
  traveller's depth. n = (|dx| + |dy|) / `req->step` (`+0x01`), at least 1.
  With `req->mode` (`+0x00`) set, the end x stops the traveller beside the
  target on its own side (`from_dx` + `to_dx` apart, with 8 of slack; when
  that would put it below `from_dx + 8` or above 0x138 − `from_dx`, beside
  the target's other side), and the traveller is turned toward that x;
  otherwise the two ends are drawn apart by `from_dx` and `to_dx` unless
  they overlap. y = each record's `+0xA` plus `from_dy`/`to_dy`. The target
  is turned toward the traveller when the acting slot is active and its
  status has no bit of 0x3C, and kind 0x77 (`g_BtlSlotIds[6]`) clears bits
  13–14 of `g_BtlRecords + 0x18C`. It returns n.
  `BtlTravelRun(req, animation)` (`0x8008AE10`) puts the traveller on `animation`, then each
  show frame copies `path[i]` to its `+6`/`+0xA` and `depth_path[i].y + 1`
  to `+8`. With `mode` set it turns traveller and target toward each other
  again (the target under the same test) and repeats the 0x77 clear. Both
  paths are freed at the end.
* **Move-to and arc.** `BtlRecordSpotMove(record, x, y, n, animation)` (`0x8008AFAC`) puts the record on
  `animation` unless it is 0xFF, fills an n-point path from its `+6`/`+0xA`
  to (x, y), and walks it one point per show frame (`+8` = y + 1).
  `arc_path_make(x0, y0, x1, y1, n, h, path)` (`0x8008B0B4`) lays out the
  straight path, then bends it into a parabola: point i − 1 gets
  `y += h * t * t / (half * half) − h`, t = i + half, half = −(n / 2).
* **Shake, blink and glow cycle.** `BtlMixFlashSet` (`0x8008A004`) and `BtlMixFlashStep` (`0x8008A0C0`):
  §"The battle frame's per-frame systems".
* **Colour pulse.** `g_BtlShowPulse` (`BtlShowPulse` in `btl.h`: +0 on,
  +4 which colour comes next, +8 length, +0xC and +0x12 two colours of
  three channels). `BtlShowPulseSet(len, a, b, c, d, e, f)` (`0x8008A7F8`) stores the length
  and the two colours and leaves the pulse off (the shows store 1 in +0 to
  start it); `BtlShowPulseClear` (`0x8008A834`) clears it all; `BtlShowPulseStep` (`0x8008A864`), while on
  and `layer_col_fade_check(0)` (`0x8001EDDC`) is 0, starts `layer_col_fade_start(0, 0xC, 1, len, …)` (`0x8001EA74`)
  toward the +0x12 colour and the +0xC colour by turns.
* **Background sequence.** `BtlTilePatternListGet(n)` (`0x8008A8F8`) returns the n-th (1-based)
  0xFF-terminated byte run in `g_BtlTilePatternLists` (NULL for 0).
  `BtlTilePatternStep(i, frames)` (`0x8008B21C`) plays byte `i` of `g_BtlTilePatternList` through
  `BtlTilePatternPut(byte, frames)` (`0x80089C8C`), restarting at 0 on 0xFF, and returns
  i + 1 (the u16 place callers keep). `BtlTilePatternPut` (`0x80089C8C`) puts pattern
  byte − 1 into the tile layer's grid, sized by bytes 1 and 2 of the
  effect's tile row (each halved), and runs that many show frames; a byte
  of 0 does nothing.
* **Raster wave.** `BtlShowWaveSet(mode, a, step, grow, table)` (`0x8008A4F8`) arms the
  wave block `*g_BtlShowWaveBlock` (`BtlShowWave` in `btl.h`: +0 on, +4
  grow, +8 frame counter, +0xA the second argument, which nothing reads,
  +0xC frames per step, +0x10 mode, +0x14 a 16-entry offset table); mode 0
  switches it off, and mode 8 first calls `tile_wave_switch_set(1)` (`0x8001D304`). A
  growing wave stores the mode and the table and zeroes the counter; a
  falling one keeps them and takes the counter modulo 32.
  `BtlShowWaveStep` (`0x8008A5E4`) steps it each show frame. On every step-th frame the
  amplitude is counter / step growing (0x20 less that falling), holding at
  0x20 or 0 past step × 32 frames. The table is rotated by
  (counter >> 1) & 0xF and scaled by amp / 32 into the x of all 0xE0 line
  records (`g_BtlShowWaveLines + 0xC`, 12 bytes each), which nothing in
  the program reads: the wave reaches the screen as (amp, phase) handed to
  `BtlBgWaveSet` (`0x80018368`) for mode 4 or `tile_wave_set` (`0x8001D2F4`) for mode 8. With the wave
  off, mode 4 calls `BtlBgWaveSet(0)` (`0x80018368`) and mode 8 `tile_wave_switch_set(0)` (`0x8001D304`).
* **Effect done?** `BtlAmountShowCheck()` (`0x8008931C`) returns 1 while the longest
  animation the impact recorded is running (`g_BtlAmountShowLongestRecord`
  still on, `g_BtlAmountShowFrameCount` below `g_BtlAmountShowLength`).
  Otherwise it resets the effect state and returns 0: `g_BtlAmountRecordsRunning`
  and the length pair cleared; records 14 to 26 (`g_BtlRecords + 0x380 + i * 0x40`,
  i 0..12) get `+0/+4/+0x14/+0x18/+0x28/+0x2C` zeroed and
  `BtlAmountRecordOff(i)` (`0x8004ADA8`) switches amount record i off; the 14 slots'
  records get their display bits restored (`| 0x2000 & 0xA3FF`), with
  `BtlSlotActionAnimationSet` (`0x80037E64`) for active party slots that have a number kind;
  then the mix flash's mode byte, the glow (`layer_mix_switch_off` (`0x8001F4E0`),
  `sprite_mix_col_set_copy(0x7FFF)` (`0x8008B368`)) and `layer_mix_set(0x400, 0xFF)` (`0x8001ECA0`). The records
  it clears are one before the ones `BtlAmountShowStart` (`0x80088EA8`) fills (15 to 27):
  the C clears record 14 and leaves record 27 as it is.
* **Sound scheduling.** `g_BtlShowSounds` is 12 four-byte slots
  (`BtlShowSound` in `btl.h`: `state` 0 off / 1 playing / above 1 a wait
  counted down one show frame at a time, `sound`, `count`, `repeat`),
  stepped by `BtlShowSoundsStep` (`0x80089E34`): at state 1 the count goes up, and once it
  is no longer below `repeat` the sound plays on channel slot + 3 and the
  count goes back to 0; a `repeat` of 0 then turns the slot off.
  `BtlShowSoundSet(slot, id, state, period)` (`0x80089EF8`) writes `state` (hangs for a
  slot ≥ 12), then plays `id` at once on channel slot + 3 when `state`
  is 0, else stores `id` and sets both `count` and `repeat` to `period`.
  `SoundPlay(id, ch)` (`0x800555F0`) is the sound request: 0 stops the channel
  (`sound_seq_stop_copy` (`0x80024590`)); 1..0xA are bank-1 sounds id − 1; 0xB..0x82 depend
  on `g_SoundMapNumber` (0x32 or 0: 0x82 → bank 2 #1, 0x18 → bank 1 #0x17,
  ≥ 0x1F → bank 2 id − 0x1F; otherwise only ids below 0x1F, bank 1);
  0x83..0x12A look up the 16-entry table `g_SoundIds` (bank 3) while
  `g_SoundMapNumber` is set. An id of 0x12B or more, and an id from 0x83 on
  that the table does not hold, return 2 with no sound; any other refused
  id plays bank-1 #1 and returns 2. Bank-1 sounds 9..0x19 shift down 2;
  it ends `sound_effect_play(ch, bank, snd)` (`0x80024514`) and returns 1.
* **Window ramps (ui).** `layer_col_fade_check` (`0x8001EDDC`) returns 1 while any of the four
  layer records (next item) has a step not above its length (checked 4, 8,
  0x80, 0x100), the poll behind every `while (layer_col_fade_check(0) & 0xFF)` (`0x8001EDDC`);
  it does not read its argument.
  `layer_col_fade_start(-, flags, a2, len, triple)` (`0x8001EA74`) starts a fade, its first
  argument unread: flag 0x80000 adds 0x18C, flag 4 adds 0x80; `triple` 0
  is the default `&g_MapCol`, −1 `g_LayerColLowest`, −2 `g_LayerColHighest`;
  a real triple outside battle with three equal values adds 0x84 when they
  are 0, else 4, otherwise flag 4 is swapped for 0x80; every path calls
  `layer_col_fade_record_set(flags, len, a2, triple)` (`0x8001E55C`).
* **Fade and tint ramps (ui).** Four 0x24-byte layer records (`LayerRecord`
  in `layer.h`): `g_LayerBgFirstRecord` 0x800AE16C (flag 4),
  `g_LayerBgSecondRecord` 0x800AE190 (8), `g_LayerSpriteRecord` 0x800AE1B4
  (0x100), `g_LayerFlatRecord` 0x800AE1D8 (0x80). Fields: +0 step, +2
  length, +4 start[3], +0xA value[3], +0x10 target[3], +0x16 difference[3];
  +0x1C the mix level, +0x1E the order number and +0x20 a mix flag, used
  by the first two records. `layer_col_fade_record_set(flags, len, -, vals)` (`0x8001E55C`)
  starts the selected fades at step 1 (start = target, target = vals,
  difference = target − start; record 0x80 clamps to 0..0xFF, the others
  go through `layer_col_conv` (`0x8001E050`); records 8 and 0x100 take one channel).
  `layer_col_fade_step` (`0x8001E7B0`) steps each while its step is not above its length:
  `value[i] = start[i] + layer_col_fade_div(difference[i] * step, length)` (`0x8001DE94`).
  Record 4 then goes to `BtlBgGreySet` (`0x8001835C`) in battle (`g_BtlRunning`) or
  `layer_col_apply(4)` (`0x8001E10C`); 8 and 0x100 to `layer_col_apply` (`0x8001E10C`) outside battle; a
  non-zero 0x80 triple to `layer_flat_draw(r, g, b, battle ? 0x300 :
  0x40)` (`0x8001DEF4`). `layer_col_apply(flags)` (`0x8001E10C`): for flag 4 or 8 it scales
  `layer_col_conv` (`0x8001E050`) of the three map colours (`g_MapCol`, `g_MapColSecond`,
  `g_MapColThird`) by the first record's three values and, apart, by the
  second record's one value, shifted down by `g_LayerColShift` (7, or 8
  when `layer_mix_mode_get()` (`0x8001DFE8`) is 5) and weighted by the two mix weights of
  `layer_mix_weights_get` (`0x8001E08C`) (× weight / 16); it clamps each channel to 0xFF,
  hands the first packed RGB to `bg_col_set` (`0x80014014`) (| (`g_BgMode` == 1) << 25)
  and the second to `bg_second_col_set` (`0x800148C8`), then passes
  `g_LayerMixMode` to `func_8001E048` (`0x8001E048`), an empty function. For flag 0x100 it scales the map
  colours by the sprite record's value (shifted down 7) into
  `sprite_col_set` (`0x80016A74`).
* **Tint by effect.** `BtlMixColNumberSet(effect)` (`0x8008B27C`) maps an effect id
  to a 15-bit colour for `sprite_mix_col_set` (`0x8001A904`): 0 → 0, 0xB1 → 0x1F,
  0xB2 → 0x18, 0xB5 → 0x7C00, 0xC1 → 0x21F, 0xC5 → 0x3FF, 0xD1 → 0x3E0,
  0xE4 → 0x8000, 0xE5 → 0x4210, 0xE6 → 0x2108, else 0x7FFF.
* **KO effect (enemy death), `BtlFallRun` (`0x800385B4`) -> `BtlEnemyFallShow` (`0x80089488`).**
  `BtlFallRun` (`0x800385B4`) (btl_fall.c; it does nothing while `g_BtlFallDone` is not 0,
  and sets it) first takes every fall-marked (`g_BtlSlotFallMarks`) party
  slot 0..4 out: its `g_BtlPartyFallenFlags` byte becomes 1 and its hp,
  active byte, flags and status 0 (a member whose `g_BtlPartyNotHeldFlags`
  byte is 0 first gets its record back from `g_BtlPartyKeptRecords`, in
  its own place), and it plays animation 0x1C (`BtlAnimationSet` (`0x80027208`)). It
  runs the HUD refill `BtlPartyPanelDisp` (`0x8004B330`), and for the first fall-marked
  enemy slot 6..13 calls `SoundPlay(0x28, 3)` (`0x800555F0`) unless its id
  `g_BtlSlotIds` is 0x70 or 0x76, then `BtlEnemyFallShow` (`0x80089488`), then zeroes the
  marked enemies' hp, `g_BtlSlotActiveFlags` and status. The effect
  (btl_show_frame.c) is pure code: no effect record, no resource, one EXE
  table. With no enemy marked it does nothing.
  - Set-up: each marked enemy's record gets `+0x20` = 1, scale `+0x24/+0x26`
    = 0x100 / 0x100 and `+0x0E` = `& 0xCBFF | 0x4800` (layer 2, the glow
    layer); `sprite_mix_col_set_copy(0x1F)` (`0x8008B368`) (glow red), `layer_mix_switch_on(0x100)` (`0x8001F4B8`) (glow
    on), `BtlMixFlashSet(0x400, 5)` (`0x8008A004`) (the mix flash in mode 5 on mask 0x400,
    level 0x14: the glow colour steps `g_BtlMixFlashCols[frame % 6]` each
    frame, the count `g_BtlMixFlash[8]` +1 every 6th).
  - Phase 1: battle frames (`BtlFrameStep` (`0x80038D2C`)) until `g_BtlMixFlash[8]` >= 4 --
    24 frames of colour cycling.
  - Phase 2, encounter `g_BtlNumber` 0xF6 / 0xF7 with slot 6 marked: animation 5
    on slot 6 (`BtlAnimationSetCopy` (`0x8008B1FC`)), no wait; its `+0x00` is never cleared, so
    the boss stays drawn. Otherwise 0x27 frames n = 0..38, one battle frame
    after each pass over the marked records: n < 8 y scale =
    `g_BtlEnemyFallScales[7 - n]`; n < 16 x scale = `[15 - n]`; n < 26 y scale = `[n - 16]`; n < 36
    `+0x0A` (drawn y) -= 0x14; else `+0x00` = 0 (not drawn) and `+0x0A` +=
    0xC8. `g_BtlEnemyFallScales` (EXE, 10 x u16): 11, 17, 27, 42, 67, 104, 163, 256,
    399, 623 -- so the sprite squashes to a dot in y, then in x, is pulled
    out as a tall thin line (x 11/256, y up to 623/256), rises 200 px in 10
    frames and is gone: 24 + 39 = 63 battle frames.
  - Tail: `+0x20` = 0, scale 0, `+0x0E` `| 0x2000 & 0xA3FF`, `g_BtlMixFlash[0]`
    = 0, `layer_mix_switch_off(0x100)` (`0x8001F4E0`) (glow off), `sprite_mix_col_set_copy(0x7FFF)` (`0x8008B368`),
    `layer_mix_set(0x400, 0xFF)` (`0x8001ECA0`).
* **The effect loader, `BtlEffectLoad(record, id)` (`0x800897DC`).** Its 4-byte row
  `g_BtlEffectRows[id * 4]` (kept in `g_BtlEffectRow`) gives the flags in
  byte 0 (0 = no effect). The LUNADATA resource `g_BtlEffectResourceNumbers[id]`
  goes to `g_ResourceReadBuffer` when that number is not 0 (word 0 = the
  unpacked size / 4, word 1 = the offset / 4 of a packed stream that
  `inflate_buffer` (`0x80021D6C`) unpacks). Flag 1: unpack into a heap block kept in
  `g_BtlEffectBlock[0]`, whose first five u16 words give the record's
  `+0x28/+0x2C/+0x14/+0x18/+0x10` tables, then
  `BtlRecordSpriteDataSet(record, block, 0x600, 0)` (`0x80089A3C`); the record's scales are
  cleared, its facing copied from the acting slot's record and the record
  put at that record's place (`BtlRecordPlaceCopy` (`0x80089BD0`)); `sprite_page_effect_load` (`0x80018AE0`)
  sends the resource's pictures to sprite pages 7 to 9. Flag 2:
  `sprite_palette_load` (`0x80018BAC`) of the resource's bytes from 0x10, with 0x600. Flag 4:
  the tile layer
  (`tile_layer_reset` (`0x8001D284`), `tile_resource_load(id)` (`0x8001DAE4`), `g_BtlEffectTileRow =
  &g_BtlEffectTileRows[byte1 * 4]`, `g_BtlTilePatternList` from
  `BtlTilePatternListGet` (`0x8008A8F8`) of the tile row's byte 0, the scroll set to (0, 0)
  by `BtlTileScrollSet(8, 0, 0)` (`0x8008A468`), the grid cleared, `g_TileLayerShown` = 1; battle-effects.md §"Background layers (flag 4)" has the layer's format).
  A 13-byte "EFF_00.DAT" name buffer is built and never read.
* **The restoring shows' places.** Outside a battle, shows 0x11, 0x14 and
  0x30 to 0x33 call `MenuUseShowPlacesPlay(palette word, animation)` (`0x8007EFC0`). It
  starts the vibration script `g_BtlActuatorRiseShortList` and sound 0x18
  on show sound slot 1, builds a 0x3C-byte record at `g_MenuUseShowRecords`
  for the first of the six places flagged in `g_BtlSlotShowPlaceFlags`,
  from the sprite bank `g_SpriteData0`, starts it
  (`ShopAnimationSet(rec, animation, 1)` (`0x80053648`)) and copies it to each later flagged
  place at `g_BtlSlotShowPlaceXs[i]` / `g_BtlSlotShowPlaceYs[i]`, then
  steps them (`ShopAnimationStep` (`0x8005381C`), `MenuFrameStep` (`0x8006ACB0`)) until the last record's
  +0x38 reaches `(+0x3A & 0x7FFF) − 1`. In a battle, shows 0x11 and 0x14
  call `BtlEffectShowPlacesRun(palette word, animation)` (`0x8007F42C`): it starts the same
  vibration script (`g_PadActuatorList`, §"The vibration scripts"), plays
  the effect's animation 0 in the acting slot's place (`BtlShowActingPlay(0, 1)` (`0x8008A95C`),
  which also starts the effect's voice), waits 0x30 show frames if
  `g_BtlShowVoiceState` is set, and places a show record on each flagged
  slot through `BtlShowPlacesPlay(palette word, animation)` (`0x8007F2D8`): sound 0x21 on show
  sound slot 1, each record made from `g_SpriteData0` at the slot's place,
  then `BtlAmountShowStart(1)` (`0x80088EA8`) and show frames until `BtlAmountShowCheck` (`0x8008931C`)
  answers 0.
* **The target's glow.** `BtlTargetSpriteMixRun(who)` (`0x8007F56C`) (shows 0x16 and 0x25
  with 1, 0x35 to 0x37 with 0) starts the vibration script
  `g_BtlActuatorRiseList`, sound 0x33 on show sound slot 1 and a colour
  pulse (`BtlShowPulseSet(0xF, 0x28, 0x28, 0x50, 0x14, 0x14, 0x28)` (`0x8008A7F8`),
  after a 0x14-frame fade to its first colour), and sets display bit 0x800
  (mask 0xE3FF) on the effect's record (`g_BtlShowRecords + 0x50E`) when
  `who` is set and the actor is its own target, or else on the target's
  record's +0xE. Then for 0xA0 show frames a mix level from 0x1F
  (`sprite_mix_set(2, lvl)` (`0x8001A8D0`)) steps down every 20 frames and back up every 5
  from frame 0x8C, with `sprite_mix_col_set` (`0x8001A904`) fed the 30-entry table
  `g_BtlTargetSpriteMixCols`; then everything is undone and the layers fade
  back over 0x14 frames.
* **The walk back.** `BtlEffectRecordBackMove(a, n)` (`0x8007BD0C`) does nothing when the
  effect's record (`g_BtlShowRecords + 0x500`) stands at the acting slot's
  record's place. Otherwise it plays animation `a` to its end, sound 7 on
  show sound slot 1, moves the record back to the acting record with
  `BtlRecordSpotMove(..., n, a + 1)` (`0x8008AFAC`) and plays `a + 2`.

### The enemy actions — `g_BtlActingShowFunctions`, all 38 handlers

`BtlActingShowRun(id)` (`0x8003A158`) (§"Item and enemy-action presentations")
runs `g_BtlActingShowFunctions[id − 0x97]`. The table at `0x800B3070` is the
last 38 words of a run of 132 function addresses from `0x800B2EF8`; the
first 94 are the effect functions `BtlActionRowRun` (`0x80031ABC`) calls
through (effect n is word n − 1). Its 38 words are the handlers for actions
0x97..0xBC. The C's inherited labels `field_state_table` (`0x800B308C`) and
`field_cmd_table` (`0x800B30C4`) fall inside it, at entries 7 and 21; their
names say nothing about it. After the table come the battle menu's three
12-byte kept-cursor tables (`g_BtlMenuKindKeptCursors`,
`g_BtlMenuSpellKeptCursors`, `g_BtlMenuItemKeptCursors`, zero in the image)
and then the menu's step table `g_BtlMenuStepFunctions`. `BtlShowNumberRun`
(`0x8007BB80`) hands `BtlActingShowRun` every number from 0x97 to 0xFD, and
`BtlActingShowRun` tests no range: a number from 0xBD on would call through
those bytes. Nothing dispatches 0xBD..0xFD. The C does not tie the table to
enemies: `BtlShow9BRun` walks on `g_BtlPartyMoveAnimations` for an acting
slot below 5. Two roads lead in:

* **the physical swing of an enemy whose CharStats `+0x17` is set** —
  `BtlAttackHitShow` (`0x800314B8`) (it landed) and `BtlAttackMissShow` (`0x80031378`) (it missed) both hand
  `+0x17` over and return, so the handler plays hit AND miss (it tests
  `g_BtlSlotAmountKinds[target] == 2`), and `BtlAttackAnimationRun` (`0x800315A8`)'s swing never runs;
* **every effect function that shows its action**, through
  `BtlShowNumberRun(g_BtlActionShowNumber)` (`0x8007BB80`) when the show
  number is in 0x97..0xFD. `BtlActionRowRun` (`0x80031ABC`) takes that number
  from byte 6 of an attack or spell row (byte 9 of an item row). Among the
  effects that call it after resolving: 17 `BtlTargetHitRun` (`0x80032958`),
  20 / 21 `BtlMarkedHitRun` (`0x80033060`), 22 `BtlTargetStat0EHitRun` (`0x80032BDC`), 24
  `BtlAreaStat0EHitRun` (`0x80032C64`), 25 / 26 `BtlMarkedStat0EHitRun` (`0x80032FA0`), the status rolls (27, 33, 36,
  39, 51: the number kind is 2, a miss, unless the roll takes, 7), 64, 66,
  67 and 68 through `BtlTargetHitStatusRun` (`0x80034418`) and so
  `BtlTargetHitRun` (`0x80032958`) (65 is `BtlNothingRun` (`0x80036830`)), 75
  `BtlTargetStat0EHitStatusRun` (`0x800344B8`) (BEFORE its status roll), 82
  `BtlTargetHpDrainRun` (`0x80034770`), 83 `BtlActingRemoveRun` (`0x80034844`) (before the actor's id is
  cleared), 84 `BtlPartyPlaceFallRun` (`0x80034898`). Show number 0 is nothing.

**The handler does its own walking**, which is why `BtlAttackWalkRun` (`0x80031244`)'s reach
switch on `+0x17` lets some actions through without a walk (0x98, 0x9A,
0x9B, 0xA4, 0xA5, 0xAC, 0xAF, 0xB1), and for 0x9D and 0xA7 asks only
`BtlTargetReachCheck` (`0x800379AC`), which walks only when the target is out
of reach. The approach happens inside the action, at the action's speed and
on its anim, so where the enemy stands afterwards, and every later reach
test, depends on the handler.

**The shared pieces** (all battle-record code: no display set-up, no
template, waits on `BtlFrameStep` (`0x80038D2C`) only):

| function | what |
|---|---|
| `BtlActingAnimationRun(anim, n)` (`0x800413EC`) | face off (`BtlFacingBothSet` (`0x8003693C`)), the caster's anim, the action's sound `g_BtlActionSound` on channel 3; `n` frames, or its anim's last step when `n` = 0xFF |
| `BtlActingAnimationOutcomeRun(anim, n)` (`0x80041338`) | that and the hit tail, the caster one depth step (`+0x08`) forward meanwhile when its `+0x0A` ≤ the target's |
| `BtlTargetOutcomeShow` (`0x80041484`) | **the hit tail.** Number kind 2 (a miss): a target below 5 — sound 0x24, anim 0x11, `BtlTargetBackMove` (`0x8002FD1C`) (sound 0x24 again, then a pixel a frame back for four), the caster's anim out and `BtlActingRestAnimationSet` (`0x80037890`), `BtlTargetForwardMove` (`0x8002FE78`) (forward again); any other target `BtlTargetMissShow` (`0x80036838`) (0x25 for id 0x3E, else 0x24; id 0x2F plays its anim 9). Otherwise: the hit sound, `BtlPartyHitAnimationSet` (`0x80030E78`), `BtlAmountShowStart(0)` (`0x80088EA8`), frames until `BtlAmountShowCheck` (`0x8008931C`) is done — the caster rested once its anim reaches the last step — `BtlFallRun` (`0x800385B4`), and a caster still mid-anim waited out. Then `BtlMarkedRestAnimationSet` (`0x80030DA0`) |
| `BtlPartyHitAnimationSet` (`0x80030E78`) | marked party slots 0..4: number kind 2 → anim 0x11, 1 / 7 → `g_BtlPartyHitAnimations[i]`; each marked slot turns to the caster unless its x equals the caster's |
| `BtlShowEnd` (`0x8004166C`) | `BtlActingRestAnimationSet` (`0x80037890`), `BtlAmountShowWait` (`0x80031028`), `BtlFallRun` (`0x800385B4`), `BtlMarkedRestAnimationSet` (`0x80030DA0`) |
| `BtlActingTargetLeftMove(gap)` (`0x8003A4E4`) | the creep: to `gap` px left of the target on its `+0x0A`, max(\|dx\|, \|dy\|) / 2 points, anim 1, five frames a point |
| `BtlFacingPartySet` (`0x800416A4`) | the acting record turns round when no active party slot stands on the side it faces: with the mirror bit 0x2000 set and no active slot 0..4 at a strictly smaller x, `attr = 0x20`; with the bit clear and none at a strictly larger x, `attr = 0x2020` (the whole word is replaced) |
| `BtlExtraRecordsAlloc(n)` (`0x8003A340`) / `BtlExtraRecordsFree(n)` (`0x8003A388`) / `BtlExtraRecordsStep` (`0x8004163C`) | n zeroed records from the heap at `g_BtlExtraRecords`, `g_BtlExtraRecordCount = n`; the battle frame ticks and draws the first `g_BtlExtraRecordCount` of them (`BtlAnimationStep(g_BtlExtraRecords, 0x1C, g_BtlExtraRecordCount)` (`0x800272A8`)). A handler copies the caster's whole record in (`*(ActObj *)g_BtlExtraRecords = …`) and plays an anim of the caster's block on it — the bite at the target, the thrown weapon, the beam's segments |
| `BtlShotSet` (`0x80040F14`) / `BtlShotRun` (`0x80040F8C`) | **the shot.** `g_BtlShotBlock`: [0] the caster's anim (0xFF: none, and no [5] sound and no [1] wait), [1] its wait (below 0x63 that many frames, 0 none, else its last step), [2] the shot's anim, [3] points (0: distance / pace), [4] pace (0: 8), [5] / [6] / [7] sounds before / at launch / on arrival (0 none), s16 [4] / [5] the offset, s32 [3] / [4] / [5] from / to / the record copied. Defaults 8, 0x63, 9, 0, 8, 0, 0x4B, 0, (0, 0), caster → target. From and to are turned to each other; the copy starts at from's facing edge + dx, `+0x0A` + dy, and flies one point a frame. For the distance, the y difference's sign is taken from from's `+0x0A` against to's, not the copy's |

**The hit sound never varies.** `BtlTargetOutcomeShow` (`0x80041484`) and `BtlAttackHitShow` (`0x800314B8`) both
test the ADDRESS of the per-slot critical array, not an entry of it
(`g_BtlSlotStrongMarks ? 0x38 : 0x23` in `btl_attack.c`, `g_BtlSlotStrongMarks != 0`
in `btl_outcome.c`), which is never zero: every physical hit plays 0x38 and 0x23 only where a handler asks for
it by name (`BtlShowA6Run`). The critical flag itself is per target
(`BtlHitStrongTry` (`0x80038310`) writes `g_BtlSlotStrongMarks[g_BtlTargetSlot]`) and the impact reads it
properly for the spark.

**`g_BtlSlotActiveFlags` still holds a slot the action just felled.** The resolver
raises `g_BtlSlotFallMarks` (KO pending); `BtlFallRun` (`0x800385B4`) — called from the
handler's tail — is what clears the active flag. So `BtlShowB6Run`'s
"each active party slot" includes a member whose HP the action took to 0.

**Uninitialised paths.** `BtlShow9ERun` (ids other than 0x1B / 7 /
0x5C), `BtlShow9FRun` (other than 0xE / 0x6C / 0x28 / 0x40 / 0x4F),
`BtlShowA3Run`, `BtlShowA6Run` (other than 0x3B / 0x30: the later
animation), `BtlShowAERun`, `BtlShowAFRun`, `BtlShowB5Run` and
`BtlShowBCRun` (no slot 0..3 marked) read locals their switch never sets
for any other caster. Every template that uses them is one of the handled
ids.

| id | handler | what |
|---|---|---|
| 0x97 | `BtlShow97Run` (`0x8003A3BC`) | the walk (anim 1), vibration `g_BtlActuatorC8LateList` (id 0x62 `D_800AE2FC`, 0x3C `D_800AE2C0`, 0x6B `D_800AE3DC`), `BtlActingAnimationOutcomeRun(8, 0xFF)` (`0x80041338`) (anim 2 for 0x72). Its speed store precedes `BtlWalkSet` (`0x8002BAA4`), which rewrites it: dead |
| 0x98 | `BtlShow98Run` (`0x8003A6D0`) | Water Dragon's swing: creep to 0xA0 px, `BtlActingAnimationOutcomeRun(2, 0x3C)` (`0x80041338`) |
| 0x99 | `BtlShow99Run` (`0x8003A6FC`) | the walk (its anim / speed edits are dead the same way), `D_800AE2D4` (id 6 `D_800AE2E8`), `BtlActingAnimationOutcomeRun(8, 0xFF)` (`0x80041338`), 0x32 frames |
| 0x9A | `BtlShow9ARun` (`0x8003A80C`) | a copy of the caster on the target (§"Item and enemy-action presentations") |
| 0x9B | `BtlShow9BRun` (`0x8003AD08`) | the shot by caster id (a switch: 2 / 0x33, 0x11 / 0x6D, 0x16 / 0x60 / 0x64, 0x18 / 0x4A, 0x2D, 0x2F, 0x42, 0x46, 0x49, 0x4D, 0x76; others the defaults) after the lane walk (`BtlWalkEdgeRun` (`0x80036654`); `g_BtlPartyMoveAnimations` for a caster below 5, else anim 1); vibration before (the 0x16 group `g_BtlActuator41List`, 0x2D `D_800AE3CC`, others none) and after (`g_BtlActuatorFFTo45List`, or `g_BtlActuatorC8List` for the 0x16 group); the hit tail |
| 0x9C | `BtlShow9CRun` (`0x8003AFA0`) | the shot, then a copy of the caster on the target with the impact (1) — not for 0x49, 0x7F, or 0x50 outside kind 2; the lane walk on anim 1 (0xE for 0x74). Detail in §"Shows 0x9F, 0x54 and 0x9C in detail" |
| 0x9D | `BtlShow9DRun` (`0x8003B4EC`) | the flies: a depth bump of 6, the walk on anim 2 at speed `+0x18` (2 for 0x30, 1 for 7 / 0x5C), six frames of 4 px in and six out, the hit tail |
| 0x9E | `BtlShow9ERun` (`0x8003B730`) | the dive: a line of 0x13 points (0x10 for ids 7 and 0x5C) onto the target, `D_800AE3C0`, the impact (0), 0x42 (0x40) points home |
| 0x9F | `BtlShow9FRun` (`0x8003BA6C`) | the beam: a train of copies every 0x10 px from the caster's facing edge to the screen's, `g_BtlActuator41List`, the impact (1). Detail in §"Shows 0x9F, 0x54 and 0x9C in detail" |
| 0xA0 | `BtlShowA0Run` (`0x8003BFA0`) | the exit (effect 83): anim 8, 8 px a frame off the left edge, the record off; the slot's id and active byte are not touched here (`BtlActingRemoveRun` (`0x80034844`), effect 83, clears them after its show) |
| 0xA1 | `BtlShowA1Run` (`0x8003C0A8`) | The Deep's drain: the walk (speed 3, reach 0x64, which `BtlWalkRun` holds at 0x28, the other slots not blocking), `g_BtlActuatorToggleList`, 0x50 frames in a 20-frame cycle (the sound, the target on 0x12 and back), the knockback `BtlTargetPushBack` (`0x80037C64`), the impact (0) |
| 0xA2 | `BtlShowA2Run` (`0x8003C308`) | a walk on the walk block as it was last filled (`BtlWalkSet` is not called; both widths off); record 14 becomes a copy of the caster on anim 9; `BtlActingAnimationOutcomeRun(8, 0xFF)` (`0x80041338`) |
| 0xA3 | `BtlShowA3Run` (`0x8003C3D8`) | by caster id: 0xA / 0x67 anim 9 and a closing anim 0xA, 0x22 anim 8 and no closing anim; the `attr` word (`+0xC`) kept, the anim to its end, 0x3C frames, the action's sound `g_BtlActionSound` on channel 3, anim 8, `+8` up by one. The walk block `g_BtlWalkBlock` (`BtlWalkSet` (`0x8002BAA4`), then speed 1, reach 0x64, walk anim 8, its own `+9` / `+0xA` / `+0xB` = 0, the action's sound as the walk's) and a walk `BtlWalkRun` (`0x8002BB5C`) ONTO each party slot 0..4 marked in `g_BtlSlotAmountKinds`, with `g_BtlActuatorFFTo45List` and the slot's anim 0x12 after each. Home: `g_BtlActuatorC8List`, then a walk with the block's `+0xC` = 4 to a 0x40-byte stand-in on the stack that holds only the start x / `+0xA` (`+8` = `+0xA`, `+0x3C` = `+0x3E` = 8). `attr` restored, `+8` down by one, the closing anim to its end, the impact (0) |
| 0xA4 | `BtlShowA4Run` (`0x8003C6E4`) | anim 0x14; record 7 switched on and shown on anims 0xA then 0xB (sound 0x68) |
| 0xA5 | `BtlShowA5Run` (`0x8003C79C`) | the vanish: anim 9 out, reappear 2 px beside the target and 0x10 above, anim 0xA in; the dodge or the impact; home through `BtlActingPlaceAnimationRun` (`0x8003CBE4`) (sound 0x68 each way). Id 0x4A (Death Warrior) uses 0xA / 0xB |
| 0xA6 | `BtlShowA6Run` (`0x8003CC7C`) | the thrown self (§"Item and enemy-action presentations") |
| 0xA7 | `BtlShowA7Run` (`0x8003D228`) | measure first; out of reach — sound (0x6C for 0x12, else 0x6D), anim 9, the walk on anim 1 (0x12) / 0xB (others), anim 0xA; `BtlActingAnimationOutcomeRun(2, 0xFF)` (`0x80041338`) |
| 0xA8 | `BtlShowA8Run` (`0x8003D354`) | a transformation: the lane walk, a flipped copy on the target (anim 0x15), the target hidden, `BtlTargetActingRecordCopy` (`0x8003F9D4`) — the target's record becomes a copy of the caster's, keeping its own place, facing and size; rest and stand 0x16, hit 0x17, with no test that the target is a party slot |
| 0xA9 | `BtlShowA9Run` (`0x8003D59C`) | the leap. `BtlExtraRecordsAlloc(1)` (`0x8003A340`); the scratch record `g_BtlExtraRecords` takes a copy of the caster's whole record and plays anim 0x19. Sound 0x74, the caster's anim 0xC to its end, `SoundPlay(0, 3)` (`0x800555F0`), sound 0x66, anim 0xD, `g_BtlActuatorC8List`. Its x and `+0xA` are kept, the scratch record hidden, `+8` up by one, and it runs off the screen 8 px a frame the way it faces: `attr & 0x2000` left until x <= −(its u16 `+0x3C`) − 0x10, otherwise right until x >= (its u16 `+0x3C`) + 0x150. `SoundPlay(0, 3)` (`0x800555F0`), 0x3C frames. Back at its own x with `+0xA` = 0 and `+8` = the kept `+0xA`; the scratch record shown, `BtlPartyHitAnimationSet` (`0x80030E78`), the impact (1), anim 0xE, `g_BtlExtraRecordCount = 1`, and `+0xA` goes up 4 a frame to the kept value (the record comes down from the top of the screen). `BtlExtraRecordsFree(1)` (`0x8003A388`), `BtlShowEnd` (`0x8004166C`) |
| 0xAA | `BtlShowAARun` (`0x8003D8D8`) | Water Dragon's special: creep to 0x68 px, anims 9 / 8 / 0xA, `g_BtlActuatorFFList`, the impact (1); nothing moves it back |
| 0xAB | `BtlShowABRun` (`0x8003DAEC`) | record 14 a copy of the caster ON the target (anim 9), `g_BtlActuatorC8List`, sound 0x5A, the impact (1) |
| 0xAC | `BtlShowACRun` (`0x8003DCA0`) | **draws**: six copies. `BtlExtraRecordsAlloc(6)` (`0x8003A340`), `g_BtlExtraRecordCount = 0`, the caster's record copied into the six records at `g_BtlExtraRecords`. Copies 0 and 1 stand on battle records 0 and 1 (x and `+0xA` theirs, `+8` = `+0xA` + 1); copies 2..5 each get x = `rng_mask_below(0xFF, 0xA0)` (`0x8003A0D4`) and `+0xA` = `rng_mask_below(0x1F, 0x20) + 0x70` (`0x8003A0D4`) (`+8` one more) — eight draws before its first frame, x before `+0xA` — and are then spread one per screen quarter: copy 3 x + 0xA0, copy 4 `+8` and `+0xA` + 0x28, copy 5 both. Sound 0x6C, the caster's anim 9 to its end then anim 8, 0x1E frames, sound 0x74, `g_BtlActuatorFFList`; all six copies on anim 0xB, `g_BtlExtraRecordCount = 6`, 0x1E frames, `g_BtlActuatorC8List`, the impact (1); copy 0's anim to its end, `BtlExtraRecordsFree(6)` (`0x8003A388`), sound 0x6C, the caster's anim 0xA to its end, `BtlShowEnd` (`0x8004166C`) |
| 0xAD | `BtlShowADRun` (`0x8003DFE4`) | the caster drawn from effect 0x5B's data (`BtlEffectLoad` (`0x800897DC`)); a shot; a second record of the effect on the target; the impact (1) |
| 0xAE | `BtlShowAERun` (`0x8003E274`) | Tank Golem's charge: off the screen at 0x10 px a frame, one full-screen pass per marked party slot on its `+0x0A`, home, the impact (0) (0x43 anim 8, 0x75 anim 0xC) |
| 0xAF | `BtlShowAFRun` (`0x8003E6CC`) | measure, then wind-up / walk at speed 1 / landing by caster id (0x39 / 0x48 / 0x59, 0x79, 0x7C, 0x7D, 0x7F), sound 0x68; `BtlActingAnimationOutcomeRun` (`0x80041338`). 0x7C / 0x7D widen by 0x20 and load effect 0x61 / 0x5E. In battles 0xFE and 0xF7 the strike is played from that effect's record for 0x3C frames, and then the width goes back and the effect's block is freed — for any caster id, though only 0x7C and 0x7D load the record; in other battles 0x7C and 0x7D keep the wider width and the block. In battle 0xF7 record 7 plays 0x15 / 0x16 / 0xB beside the walk |
| 0xB0 | `BtlShowB0Run` (`0x8003EBD4`) | three shots, a copy on the target after each, the impact (1) |
| 0xB1 | `BtlShowB1Run` (`0x8003EEC4`) | Stone Creep: the lane walk, a shot (anim 2, shot anim 0xA, sound 0x6A), anim 9, the hit tail |
| 0xB2 | `BtlShowB2Run` (`0x8003EF4C`) | face off; the caster drawn from effect 0x5F's data (0x5C in battle 0xFE) with its own x / `+0xA` and mirror bit, its record kept aside; sound 0x68, record 7 on anim 0x15, the stand-in's anim 0 to its end, 0x1E frames; `g_BtlActuatorC8List`, sound 0x5C; a jump BESIDE the target, on the side it came from (x the target's − 1 when it was left of it, else + 1; `+0xA` the target's `+0xA` + 1; `+8` the target's `+0xA` + 2) on anim 1, 0x14 frames, `BtlPartyHitAnimationSet` (`0x80030E78`), the impact (1); to its end, 0x1E frames, back to the kept x / `+0xA`; record 7 on 0x16, anim 2 to its end, record 7 on 0xB; the kept record restored, `BtlShowEnd` (`0x8004166C`), `xmalloc_heap_free(g_BtlEffectBlock[0])` (`0x80022F88`) |
| 0xB3 | `BtlShowB3Run` (`0x8003F2EC`) | two records of effect 0x60 (0x5D in 0xFE) 0x3A px apart, the caster's own record off, the impact (1) |
| 0xB4 | `BtlShowB4Run` (`0x8003F530`) | Ultra Fiend's tile strike: up off the screen at 8 px a frame, 0x64 frames gone, down onto the marked tile (`g_BtlActionNumber` the column, `g_BtlTargetSlot` the row, 8 px each), `BtlFallRun` (`0x800385B4`) — and it stays there; no amount show |
| 0xB5 | `BtlShowB5Run` (`0x8003F784`) | a transformation by shot: `BtlTargetActingRecordCopy` (`0x8003F9D4`), the target's stand / rest and hit anims by ITS character id (1 → 0xF / 0x17, 6 → 0x10 / 0x18, 2 → 0x11 / 0x19, 7 → 0x12 / 0x1A); no amount show |
| 0xB6 | `BtlShowB6Run` (`0x8003FAD8`) | Slice O' Dead: blinks out (66 frames), a copy of it on every active party slot (anim 2), the impact (1), blinks back (55) |
| 0xB7 | `BtlShowB7Run` (`0x8003FDB8`) | a walk on the walk block as it was last filled (`BtlWalkSet` is not called: on the swing road that is `BtlActionAttackRun`'s), `g_BtlActuatorFFList`, sound 0x73, anim 2, the impact (1) |
| 0xB8 | `BtlShowB8Run` (`0x8003FE60`) | the self-throw, both ways as shots. Sound 7, anim 8 to its end, the record hidden; shot anim 9, 0x2C points, no launch anim (`[0]` = 0xFF), offset word `+8` = −(the record's `+0x3C` >> 1). Shown again facing by x (attr 0x20 left of the target, else 0x2020) ON the target (x and `+0xA` the target's, `+8` the target's + 1): `g_BtlActuatorC8List`, sound 0x5A, anim 0xA to its end, `BtlPartyHitAnimationSet` (`0x80030E78`), the impact (0). Turned round, sound 8, anim 0xB to its end, hidden; the record goes home (`+8` and `+0xA` both the kept `+0xA`) and a stack copy of it, placed at the target, flies back as shot anim 0xC, 0x28 points, with the block's roles swapped (`+0xC` the target, `+0x10` the caster, `+0x14` the copy). Shown, anim 0xD to its end, `BtlShowEnd` (`0x8004166C`) |
| 0xB9 | `BtlShowB9Run` (`0x800401D8`) | Vile Crustacean (id 0x74) `g_BtlActuatorRiseList` / sound 0x65 / anim 0xA; others `g_BtlActuator41List` / 0x66 / 0xB; the impact (1) |
| 0xBA | `BtlShowBARun` (`0x80040348`) | Chaos Fiend's five (§"Item and enemy-action presentations") |
| 0xBB | `BtlShowBBRun` (`0x800408F8`) | the dash onto each marked party slot (`BtlActingPathMove` (`0x80040BA0`), 16 points), the impact (0), home |
| 0xBC | `BtlShowBCRun` (`0x80040C70`) | four copies, one onto each marked slot 0..3 (anim 0xF), the impact (1) |

The enemy names in the table, and which enemy uses which handler, come from
the game's data, not from the C, which tests only ids. Only 0xAC draws from
the RNG; none of the vibration scripts any handler starts uses the random
motion except `BtlShowBARun`'s `g_BtlActuatorRandomList`, which it replaces
before that script's draws begin.

### Shows 0x9F, 0x54 and 0x9C in detail

None of them draws from the RNG except where stated.

* **`BtlShow9FRun` (`0x8003BA6C`)** (`btl_enemy.c`) is enemy action 0x9F, a
  line of copies of the acting record. `BtlExtraRecordsAlloc(0x15)`
  (`0x8003A340`) takes 0x15 extra records, the acting record goes on anim 8,
  then a switch on the acting slot's id `g_BtlSlotIds[g_BtlActingSlot]` picks
  the (dx, dy) offset and the copies' anims (9 for the first, 0xA for the
  rest, 0xB to take them away): ids 0xE and 0x6C (1, -8); 0x28 (24, -20)
  after `BtlAnimationEndWait` (`0x8002FDBC`); 0x40 (0, 0) after 0x10 frames,
  with extra record 0x14 (`g_BtlExtraRecords + 0x500`) a copy of the acting
  record on anim 0xB; 0x4F (16, -6) with anims 0xD / 0xE and no take-away
  anim, the acting record first playing anim 9 to its end and then anim 2.
  The acting record is copied into extra record 0, offset, and a new 64-byte
  copy is stamped every frame 16 px apart until it leaves the screen (facing
  flag 0x2000 at +0xC: leftward to x < -15, otherwise rightward to
  x >= 0x150). Then record 0 goes off, `BtlPartyHitAnimationSet` and the
  impact (1), 60 - n idle frames, and one copy taken away per frame.
* **`BtlShow54Run` (`0x800876E4`)** (`btl_boss.c`) is show 0x54 of
  `BtlShowNumberRun`'s own table, not an enemy action: the 20-record scatter
  effect. The caster's record plays anim 0xD to its end and then 0xE, with
  sounds 0x72 at once and 0x73 at the 0x11th show frame, and the window pulse
  arms (`BtlShowPulseSet` (`0x8008A7F8`), `layer_col_fade_start` (`0x8001EA74`)
  on its +0xC triple, `g_BtlShowPulse.on = 1`). One random point is drawn with
  each of the first six tile patterns, x = 0x10 +
  `rng_mask_below(0x3F, 0x30)` (`0x8003A0D4`) and y = 0x70 +
  `rng_mask_below(0x1F, 0x1F)` (`0x8003A0D4`), then one more with each pattern
  step until there are 20, the steps going on while the fades run: these ARE
  RNG draws, 40 of them. Each of the 20 records at `g_BtlShowRecords` is a
  copy of record 20, the +0x500 template (+0 = 1, +0xE = 0x200): a record
  below 6 whose slot's number kind in `g_BtlSlotAmountKinds` is 1 takes the
  slot's own position (`BtlRecordPlaceCopy` (`0x80089BD0`), anim 0), the rest
  their point + (i % 6) * 0x30 / (i % 2) * 0x20 (anim 1). Sounds 0x74/0x75,
  the pulse off, the impact (1), `BtlAnimationFramesCount` (`0x80088DF4`)'s
  frame count of pattern steps, the records off, the windows faded out over
  0x40 frames, pattern steps while `BtlAmountShowCheck` (`0x8008931C`) holds
  or a fade runs.
* **`BtlShow9CRun` (`0x8003AFA0`)** (`btl_acting.c`) is enemy action 0x9C,
  entry 5 of `g_BtlActingShowFunctions`: the strike from on top of the
  target. A switch on the enemy id `g_BtlSlotIds[g_BtlActingSlot]` picks the copy's anim (default 0xB) and fills the shot
  block `g_BtlShotBlock`. Ids 0x49 and 0x7F, and 0x50 unless
  `g_BtlActionKind == 2`, skip the impact; id 0x74 passes 0xE to
  `BtlWalkEdgeRun` (`0x80036654`). Then `BtlFacingBothSet`, the shot
  (`BtlShotRun` (`0x80040F8C`)) and `g_BtlActuatorC8List`. A 64-byte stack
  copy of the actor record becomes the extra object `g_BtlExtraRecords` at
  the target's x / `+0xA` (`+8` = `+0xA` + 1) on the picked anim; 0x10
  frames, `BtlPartyHitAnimationSet`. With the impact, `BtlAmountShowStart(1)`
  (`0x80088EA8`) starts and frames run while `BtlAmountShowCheck()`
  (`0x8008931C`) holds; when the copy reaches its last step the actor goes
  idle (`BtlActingRestAnimationSet` (`0x80037890`)). For id 0x7A (Royce) the
  actor is instead put on anim 0x11 at that point, and the loop then compares
  the COPY's step `+0x38` with `(actor +0x3A & 0x7FFF) - 1`, the last step of
  the actor's anim 0x11. Both are absolute step numbers of the same step list
  (`+0x38` and `+0x3A` hold first + step and first + count), so the actor is
  rested in the loop only if the copy's step reaches that number; otherwise,
  after the loop, the C waits out the actor's anim and does not call
  `BtlActingRestAnimationSet`. After the loop, for every id, the copy's anim
  is waited out and the copy released; with the impact `BtlFallRun` follows,
  without it 30 more frames.

## The screen effects, the pad and the sound of a presentation

What a presentation does to the screen, as the C does it: the ramps in
`layer_col.c`, the shake runner `BtlMixFlashStep` (`0x8008A0C0`) in
`btl_effect.c`, and the glow in the sprite drawers of `sprite_list.c` and
`sprite_draw.c`.

**The order one frame is built in.** `BtlShowFrameStep` (`0x80089DDC`), while
a presentation is open (`g_BtlShowOpen`), runs the sound slots
`BtlShowSoundsStep` (`0x80089E34`), the pulse `BtlShowPulseStep` (`0x8008A864`),
the wave `BtlShowWaveStep` (`0x8008A5E4`) and the display objects
(`BtlAnimationStep(g_BtlShowRecords, 0x1C, 0x15)` (`0x800272A8`)); then, open
or not, `BtlWindowsMoveStep` (`0x8004A350`) and the battle frame `BtlFrameStep`
(`0x80038D2C`). That runs the shake runner `BtlMixFlashStep` (`0x8008A0C0`)
(which also counts the impact clock `g_BtlAmountShowFrameCount`), the actors'
records and the rest of the per-frame systems, then `BtlFrameDraw`
(`0x80018564`): the actors (`sprite_list_draw` (`0x8001A798`); `sprite_draw`
(`0x8001A528`) puts them at OT 0x200 + depth, or 0x20 + (0xFF − y) / 8 for
class 0x6000), the ramps `layer_col_fade_step` (`0x8001E7B0`), the arena
sprites, the frame's end and the next frame's start. The background layer
(`tile_layer_step` (`0x8001DA78`)) comes after that, so it goes into the next
frame's OT first. The OT draws the highest index first (`frame_start` (`0x8001F294`) clears it
with `GsClearOt` (`0x8008FB98`), which links it with `ClearOTagR` (`0x8008C0F0`)) and, inside a slot, the last prim added first, so slot 0x300
draws the arena, then the `E1 0x220` draw mode (abr 1, additive), the overlay,
then the background layer's tiles — and every actor after. **No actor is ever
under the overlay.**

**The ramps.** `layer_col_fade_record_set` (`0x8001E55C`) starts a record from
its previous target (`start = target`, not the value reached);
`layer_col_fade_step` (`0x8001E7B0`) steps
`value = start + layer_col_fade_div(difference · t, length)` (`0x8001DE94`)
(C division, truncating; a length of 0 is read as 1) for t = 1..length, the
first step on the frame it is armed. In battle:

| record | drawn as |
|---|---|
| 4 (`g_LayerBgFirstRecord`) | the arena's brightness `g_BtlBgGrey` = value[0] (`BtlBgGreySet` (`0x8001835C`); colour (b, b, b); 0x80 = as is) |
| 0x80 (`g_LayerFlatRecord`) | while any channel is nonzero, `layer_flat_draw` (`0x8001DEF4`): an ADDITIVE POLY_F4 (0,0)-(320,224) in `value`, OT 0x300 — over the arena, under the actors |
| 8, 0x100 | nothing in battle (`layer_col_apply` (`0x8001E10C`) runs only outside it) — but they still hold up `layer_col_fade_check` (`0x8001EDDC`)'s waits |

`layer_col_fade_start` (`0x8001EA74`) in battle gives a real triple to record
0x80 and not record 4 (`(flags & ~4) | 0x80`), and record 0x80 clamps it to
0..0xFF — so **a negative triple draws nothing in battle**: Thunderbolt's
(`BtlShow09Run` (`0x8007DD10`)) `(-0x3C, -0x3C, -0x3C)` is a 16-frame wait. The three codes
skip that rewrite, and record 4 takes them through `layer_col_conv`
(`0x8001E050`): 0 (`g_MapCol`, zeros) returns the arena to 0x80 and the
overlay to 0; −1 (`g_LayerColLowest`, −0x100 each) takes the arena to black;
−2 (`g_LayerColHighest`, 0x100 each) to 0xFF plus a 255 overlay — a white-out.
Thunderbolt's visible part is the pulse's first triple, (0x3C, 0x3C, 0) over
20 frames, then the pulse (it starts on the second triple): 40, 20, 0, 20, 40,
60, period 6.

**The hit reaction** (`g_BtlMixFlash`, `BtlMixFlashStep` (`0x8008A0C0`)): mode
2 nudges every slot whose outcome is 1/3/4 by `g_BtlMixFlashXOffsets[step]` —
cumulative +12, −12, +11, −11, +10, −10, +9, −9, +8, 0, ten frames — only the
hit slots, never the screen; modes 1 / 3 blink layer 2's glow (on 8, off 9, on
5, off 5, on 3, off 3, then on); mode 5 (the KO fade) cycles the glow colour
through `g_BtlMixFlashCols` one frame each.

**The glow.** An actor's mix group (the page's "layer") is
`(+0x0E >> 10) & 3`. The impact's `& 0xCBFF | 0x4800` makes it 2 and clears
the semi-transparency bit 0x400; it also moves the ordering class from 0x2000
to 0x4000, which `sprite_draw` (`0x8001A528`) orders the same way (OT 0x200 +
depth). With `g_SpriteMixOn` on (`layer_mix_switch_on(0x100)` (`0x8001F4B8`))
and the group's level `c = g_SpriteMixLevels[group]` set
(`layer_mix_set(0x100 << group, c)` (`0x8001ECA0`); 0xFF off), every drawer
adds a second copy of the sprite: CLUT row 509, which `sprite_mix_palette_load`
(`0x80019B90`) fills — the 256-entry u16 buffer `g_SpriteMixPalette`, 0x0000
(transparent) then 255 × (`g_SpriteMixCol` | 0x8000) — and uploads with
`palette_rows_load_start(9, 1, g_SpriteMixPalette)` (`0x80019B54`), its only
caller being `sprite_piece_draw` (`0x80019DBC`). The copy's colour is
`g_SpriteMixCopyCol` = 4c grey, ADDITIVE, in the same OT slot, drawn over the
sprite. The sprite itself is not darkened (it keeps its `g_SpriteCols` colour):
each opaque texel becomes `pixel + glow · c / 32` (c = 0x14 → 62.5 %). The
impact's glow is red (0x1F) for a spell, white (0x7FFF) for mode 0.
`sprite_piece_draw` sets `g_SpriteMixLevelGrey` = 4(32 − c) for a piece with a
mix and 0x808080 for one without, and nothing resets it after the actor pass;
the arena goes through the same drawer (`sprite_plain_draw` (`0x80016CF0`), by way of
`sprite_grey_draw` (`0x80016E90`)), so it gets a copy too whenever the last piece drawn before
it had a mix.

**Window 8** (the background layer): level L sets the arena to 0x80 − 4L and
the layer to grey 4L, additive, while L < 0x1F; at L ≥ 0x1F the arena is black
and the layer opaque (`tile_mix_set` (`0x8001DD38`)).
`layer_mix_fade_start(8, _, dur, lvl)` (`0x8001EBA4`) in battle hands over to
`tile_mix_fade_start` (`0x8001DDA4`), which ramps L linearly to 0x1F − lvl,
starting from `g_TileCol >> 2` capped at 0x20 — the low byte, read as
`(char) g_TileCol`, is **unsigned** in this build, so a ramp after an opaque layer starts at
0x20.

**The raster wave** (`BtlShowWaveStep` (`0x8008A5E4`)): mode 4 (Inferno,
`BtlShow2ARun` (`0x80082D9C`)) shears the ARENA: `BtlBgWaveSet` (`0x80018368`) sets
`g_BtlBgWave = amp · g_BtlBgWaveTable[ph] / 8` (the table is a 0..±4 triangle,
so up to 16 px); mode 8 (`BtlShow55Run` (`0x80087C4C`) and `BtlShow56Run` (`0x800883F8`)) hands (amp, ph) to the background layer's own wave
(`tile_wave_set` (`0x8001D2F4`)). The 0xE0 scanline x values `BtlShowWaveStep`
writes into `g_BtlShowWaveLines` are never read anywhere in the C.

While `g_BtlBgWave` is not 0, `BtlFrameDraw` (`0x80018564`) draws the arena
with `BtlBgWaveDraw(a, x, w, page)` (`0x800183A0`) in place of its two flat
sprites. It draws seven passes of two free quads (`sprite_quad_draw`
(`0x800174F0`), OT 0x300, 1:1), 14 strips of 16 lines: the first strip of a
pass has its top edge moved right by `a` and its bottom edge left by `a`, the
second is the mirror, so each strip's bottom edge is the next one's top and
the picture zigzags. Colour-table entry 0 (`g_SpriteCols[0]`) is replaced by
`g_BtlBgGrey` for the draw and restored after it. `BtlFrameDraw` calls it per
page as `(g_BtlBgWave, 0, 0x100, 0)` and `(g_BtlBgWave, 0x100, 0x40, 1)`: the
sheared right page is 0x40 wide where the flat one is 0x50.

**`BtlRecordSpriteDataSet` (`0x80089A3C`)** (the bind) points a record at its
sprite data's tables, sets its draw word to `(palette & 0x3FF) | 0x2000`,
starts its animation, zeroes the scale words (+0x20, and +0x24 as a u32) and
measures the footprint `BtlRecordSizeSet(obj, anim)` (`0x800381D8`) — what
Tornado's (`BtlShow2CRun` (`0x800839E8`)) `0x4000 / +0x3C` scale divides by.
`BtlRecordScaleSet(obj, x, y, on)` (`0x8008A7CC`) switches the scale on, or
off to (0x100, 0x100).

### The vibration scripts — `g_PadActuatorList`

The three stores every script start makes (`g_PadActuatorList = &sym;
g_PadActuatorRestFlags = 0x3F; g_PadActuatorListWait = 0`) drive the
controller's two actuators (the DualShock's motors). `pad_update`
(`0x8001F508`) (`pad_update.c`, once a frame) steps the byte script at
`g_PadActuatorList`, then reads both ports with `PadGetState` (`0x800A4DA4`).
Ops 0x10/0x20/0x30/0x50 set a level / max / min / period per actuator
(`cmd & 1`), 0x40 picks the motion (`cmd & 0x4E`: 0x40 off, 0x44 ramp up,
0x46 down, 0x4A / 0x4C toggle, **0x48 random — the one op that draws from the
battle RNG**, `rng_draw() % (2w) − w` (`0x8001C770`), w the step), 0x60 waits,
0 ends — into the two actuator bytes `g_PadActuatorLevels` /
`g_PadActuator1Level`. The rest of the op set: 0x80 the step, 0x70 / 0x90
reset a motor, 0xC0 / 0xB0 a counted loop, 0xA0 a wait on the clock
`g_MovieFrameNumber`.

**One script uses the random motion — `g_BtlActuatorRandomList`.** Of the 25
scripts the handlers start (`g_MapActuatorOneFrameList` ..
`g_BtlActuatorQuickenList`), only `g_BtlActuatorRandomList` carries op 0x49 —
motor 1, motion 0x48: `90 4A 11:FF 45 81:02 60:50 | 21:FF 31:AF 53:03 4A 49
60:28 | 53:01 42 12:00 31:00 47 60:50 00`. After its first 0x50-frame wait it
puts motor 1 on the random motion with period 3 for 0x28 frames: a draw every
fourth frame, **ten draws on script frames 85, 89 .. 121** (counting the frame
after the handler's assignment as 1) — made with vibration on or off, since
`pad_update` (`0x8001F508`) zeroes the levels only afterwards. Three party
presentations run it that long: **Ice Wall** (`BtlShow1DRun` (`0x800813C0`), among its own
block rolls), **Inferno** (`BtlShow2ARun`, which draws nothing else) and
**Flash Wind** (`BtlShow2FRun` (`0x80085194`)). `BtlShowBARun` (`0x80040348`, enemy action
0xBA) starts it too, but replaces it with `g_BtlActuatorFFList` once its
clones' walk is done — one frame per 8 pixels of the longest distance, well
before frame 85. The breath attacks' two routines in `btl_boss.c`,
`BtlPartyRightMove` (`0x80086700`) and `BtlPartyRightMovePatternsRun`
(`0x80086B64`) (§"The breath attacks' party sweep" and §"The breath sweep's
long form"), start it as well. **`g_GameOptions[5]` is the VIBRATION
option**: with it on, the shows that shake the pad themselves write the
actuators directly from the RNG (Dragon Anger 2 draws per burst frame;
Rock'N'Roll 240 + 544, the 240 being `BtlShakesRun` (`0x800843E0`)'s two draws
a frame, 144 before the main loop and 96 after the impact) — the draws
spell-rng.md counts.

### The set-up clip — `g_BtlShowVoiceState`

`BtlShowOpen` (`0x8007B7C8`) loads the presentation's set-up sound when the
effect row's byte 2 is nonzero (`VoiceSet(g_BtlEffectRow[2])` (`0x80055470`),
or `[3]` for caster kinds 0xB / 0x7C / 0x7D); `BtlShowActingPlay(…, cue)`
(`0x8008A95C`) raises `g_BtlShowVoiceState` only when that byte is nonzero;
`BtlShowVoiceStep` (`0x80089D78`) (in the battle frame) starts it
(`VoiceStart` (`0x800554C4`)) and clears the flag once `VoiceStep(1)`
(`0x80055524`) reports it done; the teardown `BtlShowClose` (`0x8007BACC`) ends
with `BtlShowVoiceWait` (`0x8007BA94`), which runs presentation frames until
then. The clip is XA, from `_AUDIO.XA` (sound.md §"The XA voice clips").

### The sound in battle

`BtlShowSoundSet(slot, id, state, period)` (`0x80089EF8`) schedules
`SoundPlay(id, slot + 3)` (`0x800555F0`) (above). In battle bank slot 0 holds
resource 232 (`sound_common_load` (`0x80023E00`), which `main` calls) and slot
1 resource **358** (0x166). The battle entry `MapBtlRun` (`0x8007AE58`) first
sends the drive there with `sound_btl_seek` (`0x80024044`) =
`resource_seek(0x166)` (`0x80020290`), then calls `SoundMapSet(0)` (`0x800555CC`), which sets
`g_SoundMapNumber` to 0 and loads the bank for area 0 — resource 0x166 — as
bank 1 (`sound_area_load` (`0x80023F78`)). `sound_effect_play`
(`0x80024514`)'s kind-2 bound `< 0x60` is that bank's 96 sequences. Every
effect sound id is in 0x1F..0x82, which reaches slot 1 only while
`g_SoundMapNumber` is 0x32 or 0 — and in battle it is 0: id `n` plays bank 358
sequence `n − 0x1F` on SEQ player `slot + 3`. The music: `MapBtlRun` starts
`MusicPlay(arg)` (`0x80054F2C`) with its seventh argument (1, "Fighting
Spirits", for a random battle); `BtlRun` (`0x80027434`)'s tail stops channel 3
(`SoundPlay(0, 3)`) and plays 0x3E "Victory" (`g_BtlNumber >= 0xEB`) or 0x3D
"Winners" for a win, and 0x37 "Broken Blade" (bank 283) for a wipe — also
when the wipe becomes result 3 because a party slot that started the battle
holds id 0.

### Tornado's divide — `MTH_Div` (`0x8001D168`)

`MTH_Div(a, b)` (Tornado's spline; `mth_curv.c` — the original file was
MTH_CURV.C, by its own assert string) is a 16.16 fixed-point divide, not an
integer one: (a << 16) / b done in 32 bits. The dividend is shifted up while
it still fits (below 0x40000000), the DIVISOR shifted down for whatever is
left of the sixteen bits — its low bits are lost, so the result is exact for
|a| < 0x8000 and otherwise can only come out too large in magnitude — then one
signed `div`; when the divisor reached 1 before the sixteen shifts were used,
the quotient is shifted up by the rest. The sign is applied last (truncation
toward zero). A zero divisor calls the assert hook ("MTH_Div() by zero", line
565), an empty function in this build, and falls into the `div`'s own trap.

### The physical hit's impact — `BtlAttackHitShow` (`0x800314B8`)

**The physical hit runs the same impact.** After the resolver, a landed swing:
an enemy whose CharStats `+0x17` is set takes `BtlActingShowRun(+0x17)`
(`0x8003A158`), its own action presentation, and nothing below; otherwise
`BtlAttackAnimationRun(1)` (`0x800315A8`) — the swing anim, and its sound
`SoundPlay(+0x19, 3)` (`0x800555F0`) (the weapon's: party `BtlPartyStatsSet`
(`0x8002A138`) sets 0x44 unarmed; an enemy's is the template byte the copier
writes to +0x19) unless the party weapon has a swing presentation of its own
(`BtlAttackShowRun` (`0x8003179C`) 0x3A for weapon kind 3, or 5 on character
2; 0x3B / 0x3C for weapon items 0x19 / 0x1A) — then the hit's sound, **always
0x38** (the source reads `g_BtlSlotStrongMarks ? 0x38 : 0x23`, but the test is
of the array's address — §"The enemy actions"), a party target's hit anim, the
attacker's rest pose (`BtlActingRestAnimationSet` (`0x80037890`)), the
knockback `BtlTargetPushBack` (`0x80037C64`), `BtlAmountShowStart(0)`
(`0x80088EA8`) (white glow, the blink, the hit / critical / barrier spark, the
number popup) and `BtlAmountShowWait` (`0x80031028`) —
`while (BtlAmountShowCheck()) BtlFrameStep()` (`0x8008931C`, `0x80038D2C`),
battle frames with no display tick. `BtlAttackAnimationRun` faces attacker and
target at each other (`BtlFacingBothSet`) before EVERY swing. The evade path
is `BtlAttackMissShow` (`0x80031378`); it and the swing presentations
0x3A..0x3C are in battle-presentations-2.md §"Group D — the item-use
presentations 0x30..0x37 and the ranged weapon shots 0x3A..0x3C", which also
has `BtlAttackAnimationRun` in full.

## Battle entry and the enemy-AI dispatcher

* **`MapBtlRun` (`0x8007AE58`)** (`map_btl.c`) is the battle run from the
  field: it leaves the map, calls the battle main loop `BtlRun` (`0x80027434`)
  and puts the map back. It is the only writer of the party's home tiles
  `g_BtlPartyPlaceXCells` / `g_BtlPartyPlaceYCells`, which `BtlPartyLoad`
  (`0x8002B114`) reads: for each party slot it copies bytes 0 and 1 of the
  member's 8-byte record at `g_ChrPlacePresetRecords + id * 8`, byte 0 less
  its own second argument, and bytes 2 to 7 into the six preset tables
  `g_BtlPartyPreset0Kinds` … `g_BtlPartyPreset2Numbers`. Where the party
  stands: §"The party's x shift, measured; five members".
* **`BtlEnemyActionSpecialSet` (`0x800417E4`)** (`btl_plan.c`) is not UI: it
  walks the active enemy slots 6..13 and switches on the enemy id, `0x6E..0x7F`,
  calling one of 17 per-boss routines (`BtlEnemy6EEnemy70ActionSet`
  (`0x80041E24`) for both 0x6E and 0x70, `BtlEnemy6FActionSet` (`0x80041A80`), …
  `BtlEnemy7FActionSet` (`0x800419B0`)). Each sets the slot's planned kind
  `g_BtlSlotPlannedKinds`, number `g_BtlSlotPlannedNumbers` and target
  `g_BtlSlotPlannedTargets`, most of them from `rng_7f_draw` (`0x8003A0B4`)
  rolls (`rng_draw() & 0x7F`, `btl_pick.c`). For example id 0x7F: a draw below
  42 attacks the nearest party member (`BtlSlotNearestFind(2)`), 42 to 83 is
  special 0x3A at a `BtlPartyTargetPick` (`0x8003995C`) pick, and the other
  44/128 is special 0x39. A slot of any other id gets no plan from it.
  `BtlRun` (`0x80027434`) calls it INSTEAD of the generic `BtlEnemyActionSet`
  (`0x8004C958`) when `g_BtlNumber >= 0xEB`, so a boss-class fight never runs
  the generic AI types at all; its only call is there. At the slot's turn
  `BtlActionSpecialAdjust` (`0x8002F29C`) calls ten of the same routines again
  with 0 (§"The action loop"). `BtlSpecialInit` (`0x8002A7FC`) scales the
  enemies' levels for these fights, and `BtlActionSeriesRun` (`0x8002DD88`)
  runs the turn of id 0x7C. boss-ai.md has each routine. In a battle below
  0xEB every enemy's choice is `BtlEnemyActionSet` (`0x8004C958`) (§"The generic
  enemy AI"). `BtlPartyActionSet` (`0x8004D2C4`) is not enemy AI: it is the
  PARTY's auto-battle (main-menu AI, `g_BtlPartyMode == 1`).
* The battle's menus are read on `menu-ui.md` §"The battle menu".

## The script's BATTLE command — `ScriptCommandMapBtlRun` (`0x8005DFF0`)

How a scripted fight starts, and where the battle's party list comes
from. An 8-byte script command, reached only through the script tables
(entry 43 of the run of 97 function addresses at `0x800B333C`). It first
waits for the music (`ScriptMusicWait` (`0x8005A364`)).

| Operand | Meaning |
|---|---|
| `+2` u16 | a script label (0 = none) |
| `+4` | row + 1 of the **encounter rows** at `g_ScriptMapBtlRows`, 8 bytes each: `[0]` the encounter id, `[1]` `MapBtlRun` (`0x8007AE58`)'s second argument (taken off each member's home x cell), `[2..7]` a 0-terminated party list |
| `+5` | `MapBtlRun`'s fourth argument, the way back to the map (passed on) |
| `+6` | `MapBtlRun`'s seventh argument, the music it requests with `MusicPlay` (passed on) |
| `+7` | BGM id; 0xFF = keep, otherwise `g_MusicNumber` = id and `g_MusicKind = MusicKindGet(id) & 0xFF` (`0x80055448`) |

Which encounters have a nonzero `[1]` — the party's x shift — and which
parties the rows name: §"The party's x shift, measured; five members",
under "Party stage state".

**The party list `g_BtlSlotIds`** (six bytes) is cleared, then filled:

* the row's own list is EMPTY (its first byte 0) -> from the field party
  `g_PartyIds`, up to its first 0: **ids below 0xC go in, in order; id 0xC
  (Nall) goes to slot 5; any other id is not copied**;
* otherwise from the row's bytes: 0xC to slot 5 and every other id in order,
  with no test of its range — a scripted fight can fix its own party.

Then `MapBtlRun(row[0], row[1], label, +5, g_MoveChrViewFollow, g_MoveChrSecondViewFollow,
+6)` (`0x8007AE58`) runs the battle. Its result is `BtlRun`'s: 0 when the
enemies are gone (and after a round limit), 1 when the party is gone, 2 for an
escape (§"The round loop"); `MapBtlRun` turns any result of encounters 0xF9 and
0xFE into 0, and any result but 2 while `g_MapBtlWinForced` is set. For a 1
with no label `MapBtlRun` has already cleared the party and the map objects and
run the menu from its page 0x14 into `GameEnter` (`0x80059498`). By the result:

* 1 with no label: the 0x100 bytes at `*g_MapSpriteDataMarks` are cleared,
  `g_ScriptKeptBlinkCount = 0`, `g_ScriptEnded = 1`;
* 1 with a label: the script pointer goes back to this command and
  `ScriptCommandLabelJump` (`0x8005BF5C`) (the label jump) takes `+2`;
* 2: `g_ScriptKeptBlinkCount = 0x12C`, `MapFadeIn(g_MapLayersMask | 0x130, 0x19, …)` (`0x800569E0`),
  `g_ScriptEnded = 1`;
* anything else (a win): `MapObjectTouchRemove(…)` (`0x8007B5CC`) when `g_MapTouchedObject >= 0`.

The command draws nothing from the RNG itself; its call tree contains the
whole battle.

## The round loop — `BtlRun` (`0x80027434`)

`BtlRun(btl_number)` (`0x80027434`) (`btl_run.c`) is the battle main loop — the
call `MapBtlRun` (`0x8007AE58`) makes from the field. Its argument is stored as
`g_BtlNumber` — the **encounter id** (`BtlEnemyLoad` (`0x8002B3AC`) indexes `g_BtlNumberRows`
with it, §"Battle entry graphics"), so every "encounter class" test is a
test on the id: `>= 0xEB` boss-class (encounters 235-255), `>= 0xDF` no
running (223+), `== 0xF9` / `0xFA` / `0xF6` single encounters.

On entry it zeroes the five pad-actuator words (`g_PadActuatorList`,
`g_PadActuatorRestFlags`, `g_PadActuatorListWait`, `g_PadActuatorLevels`,
`g_PadActuator1Level`), sets `g_BtlRunning = 1` and `g_BtlPartyMode = 0`, and
keeps **`party[5]`**, a copy of `g_BtlSlotIds[0..4]`. Then `BtlInit()` (`0x80029E68`)
between `screen_disp_mask_set(0)` (`0x8001EFEC`) and `(1)`, `cd_wait_btl_frame_set(1)` (`0x80020E7C`),
and `BtlSlot6AnimationAlternate()` (`0x80027934`) for encounter 0xF6. One round:

```c
while (result == 0) {
    if (g_BtlNumber != 0xF9) BtlSlot5ReviveTry();  /* Nall's revive roll       */
    BtlRoundStatusUpdate();                        /* the status tick          */
    if ((result = BtlEndCheck()) != 0) break;      /* poison can end the fight */
    if (g_BtlNumber < 0xEB) {
        BtlEnemyActionSet();                       /* the generic enemy AI     */
        /* the hang, below */
        BtlEnemyActionAdjust();                    /* confusion, status 8      */
        BtlEnemyActionAnimationSet();              /* poses, one battle frame  */
        BtlEnemy4FPlaceMark();                     /* enemy id 0x4F            */
    } else {
        BtlEnemyActionSpecialSet();                /* the per-boss AI          */
        if ((result = BtlEndCheck()) != 0) break;
    }
    BtlPartyPanelDisp();
    if (g_BtlPartyMode != 1) {
        BtlPartyPanelDisp();
        g_BtlAutoEndAllowed = 0;
        BtlMenuRun();                              /* the party's command menu */
        if (g_BtlPartyMode == 1) BtlWindow1IconDisp();
        g_BtlAutoEndAllowed = 1;
    } else BtlPartyActionSet(1);                   /* the party's auto-battle  */
    if (g_BtlPartyMode == 2) {                     /* main-menu Run            */
        r = BtlPartyEscapeTry();
        if (r == 2) { result = 3; break; }
        if (r == 0) BtlEscapeFailMove(0xFF);       /* the failed-escape move   */
        /* active, status-free members: g_BtlSlotRestAnimations = stand animation */
        BtlPartyActionAdjust();
    } else { BtlPartyActionAdjust(); BtlPartyActionAnimationSet(); }
    result = BtlRoundTurnsRun();                   /* -> BtlTurnOrderRun, the actions */
    BtlPartyStandAnimationSet();                   /* party back to stand poses */
    if (g_BtlRoundsLeft != 0xFF && --g_BtlRoundsLeft == 0) { result = 4; break; }
}
```

The callees, by address: `BtlSlot5ReviveTry` (`0x80027B6C`), `BtlRoundStatusUpdate`
(`0x800297A4`), `BtlEndCheck` (`0x80038518`), `BtlEnemyActionSet` (`0x8004C958`),
`BtlEnemyActionAdjust` (`0x80029510`), `BtlEnemyActionAnimationSet` (`0x8002965C`),
`BtlEnemy4FPlaceMark` (`0x800279A0`), `BtlEnemyActionSpecialSet` (`0x800417E4`),
`BtlPartyPanelDisp` (`0x8004B330`), `BtlMenuRun` (`0x80044118`), `BtlWindow1IconDisp`
(`0x80049740`), `BtlPartyActionSet` (`0x8004D2C4`), `BtlPartyEscapeTry` (`0x80027DAC`),
`BtlEscapeFailMove` (`0x80036BE4`), `BtlPartyActionAdjust` (`0x80029348`),
`BtlPartyActionAnimationSet` (`0x800296F4`), `BtlRoundTurnsRun` (`0x8002DB8C`),
`BtlTurnOrderRun` (`0x8002DC30`), `BtlPartyStandAnimationSet` (`0x80027CE4`).
`BtlEndCheck` answers 1 when no enemy slot 6..13 is active, 2 when no party
slot 0..4 is, and 0 otherwise.

Main-menu cursor (`BtlMenuFirstStep` (`0x80044B5C`), `btl_menu.c`): 0 = AI → `g_BtlPartyMode = 1` +
`BtlPartyActionSet(1)` (`0x8004D2C4`) (the PARTY's auto-battle — §"Where actions
come from" describes its staging functions), 1 = Command, 2 = Tactics,
3 = Run → `g_BtlPartyMode = 2`.

* **A round limit.** After `BtlPartyStandAnimationSet` (`0x80027CE4`), every round: if `g_BtlRoundsLeft !=
  0xFF` it is decremented, and at 0 the loop ends with **result 4**. 0xFF =
  no limit. Result 4 maps to 1 below — the function returns 0, the same as
  a win, but WITHOUT the victory sequence (case 4 runs none of case 1).
* **A debug trap left in the release build.** On the generic-AI path,
  right after `BtlEnemyActionSet` (`0x8004C958`): any active enemy slot 6..13 (`g_BtlSlotActiveFlags`)
  with kind `g_BtlSlotPlannedKinds == 7` and target `g_BtlSlotPlannedTargets >= 5` (Nall's
  slot or an enemy slot) sends the game into an empty `while (1)` — a hang.
* **`party[5]`**, the copy taken on entry, is what the result-2 test compares
  against (`party[i] != 0 && g_BtlSlotIds[i] == 0` → result 3) and what
  `BtlWinRun` and `BtlPartyStatsStore` are handed.
* **The exit**, after the loop: `BtlWindow1IconClose()` (`0x800497D4`) if the party's AI was on
  (`g_BtlPartyMode == 1`); `g_MusicWaitFrameKind = 2`, `g_BtlAutoEndAllowed = 0`,
  `SoundPlay(0, 3)` (`0x800555F0`); then by result —
  1 (win): fanfare `MusicPlay(0x3D)` (`0x80054F2C`) (0x3E when `g_BtlNumber >= 0xEB`),
  `BtlEndMessageDisp(1)` (`0x8004BA18`), `BtlPadWait(0x1E, 0x40)` (`0x800365DC`), `BtlEndMessageDisp(0)` (`0x8004BA18`),
  `BtlWinRun(party)` (`0x80028140`) (§"Victory");
  2 (no party): becomes 3 if an original member's id is now 0 (someone ran), then
  `MusicPlay(0x37)` (`0x80054F2C`), `BtlEndMessageDisp(result != 2 ? 2 : 4)` (`0x8004BA18`),
  `BtlPadWait(0x96, 0x40)` (`0x800365DC`), `BtlEndMessageDisp(0)` (`0x8004BA18`);
  3 (the main-menu Run's escape): nothing;
  4 (round limit): becomes 1.
  Then `cd_wait_btl_frame_set(0)` (`0x80020E7C`), `BtlPartyStatsStore(party)` (`0x80028074`) (HP, MP and carried
  items back to the persistent table — "The way back"), `g_MusicWaitFrameKind = 0`,
  `BtlClose()` (`0x80029214`), `g_BtlRunning = 0` and the five pad-actuator words zeroed again,
  and it returns `(u8)(result - 1)`: 0 won (or round limit), 1 lost, 2 escaped.
* **The round limit, in order.** Only `BtlSpecialInit` (`0x8002A7FC`) sets `g_BtlRoundsLeft` — to 3, for encounters
  0xF9 (Black Dragon) and 0xFA (Kyle); `BtlStateInit` (`0x8002B7DC`)'s 0xFF stands for
  every other fight. The count is taken AFTER `BtlRoundTurnsRun` (`0x8002DB8C`) has returned
  and `BtlPartyStandAnimationSet` (`0x80027CE4`) has run, and BEFORE `while (result == 0)` reads the
  result — so in the third round an ending found INSIDE the actions
  (`BtlTurnOrderRun` (`0x8002DC30`) hands back the first nonzero turn result: no enemy left,
  no party, the last runner's escape from `BtlSlotLeave` (`0x8002E398`)) is overwritten
  by 4. An ending found at the top of a round (`BtlEndCheck` (`0x80038518`) after the
  status tick, or after `BtlEnemyActionSpecialSet` (`0x800417E4`) in a boss-class fight) and the
  main-menu Run's escape (`BtlPartyEscapeTry` (`0x80027DAC`) → 3) `break` before the count and
  stand. Result 4 runs nothing of a win's or a loss's tail — no music, no
  `BtlEndMessageDisp` (`0x8004BA18`) message, no `BtlPadWait` (`0x800365DC`) wait, no `BtlWinRun` (`0x80028140`) (no
  revive, no victory poses, no EXP, silver or drops); `BtlPartyStatsStore` (`0x80028074`) still
  writes HP (1 for a fallen member), MP and the carried items back, and the
  battle returns 0, a win's code. Its caller `MapBtlRun` (`0x8007AE58`) (`map_btl.c`)
  also turns ANY result of 0xF9 (and of 0xFE) into 0; 0xFA keeps a party
  wipe in rounds 1–2 as a loss (1). Which encounters set it, and what they
  do at entry: boss-ai.md §"The scripted fights 0xF9 / 0xFA".

### The round's pieces

What the round loop calls, each as its C has it.

* **Action re-validation — `BtlActionTargetAdjust` (`0x8002ED34`)**, at each
  actor's turn, and **the generic enemy AI — `BtlEnemyActionSet` (`0x8004C958`)
  / `BtlEnemyActionPick` (`0x8004CAB8`)**, before the menu: §"The action loop"
  and §"The generic enemy AI" have them in full.
* **The dispatcher — `BtlActionRowRun` (`0x80031ABC`).** Kind 2 first takes `g_BtlActionNumber`
  from the actor's CharStats `+0x1C`. Kind 7 on enemy id 0x4F
  takes its row number from the actor's CharStats `+0x1D`, not `g_BtlActionNumber`
  (kind 7 re-tests `g_BtlActionKind == 7`, so a kind-2 fall-through skips it).
  The handler is called with NO argument: entry `g_BtlActionFunctionNumber - 1` of the
  table of function addresses at `0x800B2EF8` (the C's `scene_state_table`), only for
  a function number below 0x5F; `BtlF7Slot7RecordOff()` (`0x80030F8C`) follows.
  `g_BtlSlotAmounts` is 14 u16 halfwords; `g_BtlActionFunctionNumber` is unsigned.
* **Target pickers.** `BtlSlotNearestFind` (`0x80039838`) (nearest): mode 1 slots
  6..13, mode 2 0..4, else 0..13; skips inactive slots, slot 5 and the actor;
  distance |dy| + |dx| (unsigned fields, s16 differences) against 0xFFF;
  strict `<`, so the first minimum in slot order wins; with no candidate the
  return register is never set; no RNG. `BtlPartyTargetPick` (`0x8003995C`): party
  slots 0..4 holding a character that pass the mode (2 active, 3 inactive, 4
  status != 0, 5 active and HP < max, 6 active and MP < max, 7 active
  and status 0; other modes none), then ONE draw via `rng_list_pick(buf, n,
  7)` (`0x8003A03C`); none -> `BtlActionFallbackSet()` (`0x8002F3A0`) and return `g_BtlTargetSlot`.
* **Defeated-slot cleanup — `BtlFallRun` (`0x800385B4`)** (`btl_fall.c`). Runs once
  (`g_BtlFallDone` 0 -> 1; its callers reset it). Party slots 0..4 flagged
  in `g_BtlSlotFallMarks`: `g_BtlPartyFallenFlags[i] = 1`, HP / `+0x1E` / status / active
  zeroed; a held one (`g_BtlPartyNotHeldFlags` 0) then gets its record back from
  `*g_BtlPartyKeptRecords`, keeping its place and facing, its stand animation back from
  `g_BtlSpecialBytes[i + 5]`, hit animation 0x12, its hold ended (not-held 1, hold count
  `g_BtlSpecialBytes[i]` 0), as at a hold's end (the holds of Saline Slimer, 0x6F, and
  Royce, 0x7A: boss-ai.md); pose 0x1C. Then `BtlPartyPanelDisp()` (`0x8004B330`) and, if
  an enemy 6..13 is flagged, sound `SoundPlay(0x28, 3)` (`0x800555F0`) (not when the
  first flagged one's id is 0x70 / 0x76), **the enemies' fall `BtlEnemyFallShow` (`0x80089488`)**, and every flagged enemy's
  HP / active / status zeroed. No direct RNG; its frames are the fall show's.
* **The auto-revive pass — `BtlItem83Revive` (`0x80038868`).** Run
  by its three callers right after the cleanup above (`BtlRoundStatusUpdate` (`0x800297A4`),
  `BtlActionKindRun` (`0x8002E12C`) after `BtlActionStringClose()` (`0x8004999C`), `BtlEnemy7AActionSet` (`0x80042958`)). It returns at
  once in encounter 0xF9, and in 0xFE when `g_BtlSpecialBytes[0]` is 5 or more. Otherwise `g_BtlActionShowNumber = 0x34`,
  and for each party slot 0..4 the cleanup has just flagged as fallen
  (`g_BtlPartyFallenFlags[i] != 0`) that wears item 0x83 in stat byte `+0x26` or
  `+0x27`: one battle frame (`BtlFrameStep` (`0x80038D2C`)), `g_BtlTargetSlot = i`,
  `g_BtlActionKind = 4`, `g_BtlActionNumber = 0x83`, the revive `BtlTargetReviveRun` (`0x800324A4`), and
  the item taken off (`BtlWornItemRemove(i, 4)` (`0x80038BFC`) when `+0x26` holds it,
  `(i, 5)` otherwise). If any slot was revived, `BtlActionStringClose()` (`0x8004999C`) once.
  `g_BtlPartyFallenFlags` is not cleared here. No RNG draw of its own.
* **The auto-revive, beside its callers** — the Angel Ring: item 0x83, string 0x183,
  kind 4 — the two accessory slots, CharStats `+0x26` / `+0x27`; modifier
  defence +1, selector 2; status byte 0x1C. What reading it beside its
  callers settles:
  - **`g_BtlSpecialBytes[0]` in 0xFE is Ghaleon's turn counter**: `BtlEnemy7DActionSet` (`0x800431AC`)
    plans kind 0xB while it is below 3, special 0x63 at 3 and special 0x73 after,
    and raises it after each plan, once a round. So the ring works through the actions of
    his fourth round — the round of special 0x63 — and not in the actions of his fifth
    round or later.
  - `g_BtlPartyFallenFlags` means "fell in this action": `BtlActionOutcomeClear` (`0x800364C0`) clears it at
    every `BtlActionRun` (`0x8002E088`), and `BtlActionKindRun` (`0x8002E12C`) runs the pass after every
    action it dispatches (kind 5 and a run, kind 6, return before it, and fell
    nobody) — so the pass revives a member that was active as the action began
    and has fallen now. Because the pass runs BEFORE `BtlEndCheck` (`0x80038518`), a party that fell to its
    last member fights on when a ring brings one back.
  - After the status tick (`BtlRoundStatusUpdate` (`0x800297A4`)) `g_BtlActingSlot` is 5 —
    `BtlPartyStandAnimationSet` (`0x80027CE4`)'s loop at the end of the previous round leaves it
    there — so presentation 0x34's template is aimed at slot 5 (Nall); after
    an action it is the actor.
  - `BtlTargetReviveRun` (`0x800324A4`) refills MP only for `g_BtlActionKind == 4 && g_BtlActionNumber ==
    0x83` — this pass; a revive through the same function by any other row
    (function number 12) restores HP but not MP. It resets `+0x08` / `+0x0A` from
    `+0x1A` / `+0x1C`, and `BtlWornItemRemove` (`0x80038BFC`) then takes the ring's modifier
    off the CURRENT `+0x0A` (`BtlWornItemStatLower` (`0x800389C4`), kind 2): after a revive the member's defence is its
    battle-entry value less one. `BtlWornItemRemove` does not touch the six bytes at
    `+0x28`, so the status byte the ring gave
    stays. The persistent record loses the ring too (its `+0x22` /
    `+0x23` and the modifier, in `g_ChrRecords`).
  - 0x34 runs with the slot's `g_BtlSlotFallMarks` still up, so `BtlShow34Run` (`0x80085928`)
    skips the caster's anim 0x10; `BtlEffectCopyTargetStandRun` (`0x8007F94C`) stands the member on
    `g_BtlPartyStandAnimations` at frame 0xF0 and stores that in `g_BtlSlotRestAnimations`: a defend stance it fell in
    is gone.
* **The all-targets swing — `BtlAttackAllRun` (`0x800318DC`)**: `g_BtlTargetSlot =
  BtlSlotNearestFind(confused ? 0 : 1)` (`0x80039838`), `BtlAttackAnimationRun(1)` (`0x800315A8`), `BtlActingRestAnimationSet()` (`0x80037890`),
  then every active slot except the actor (6..13, or 0..13 when confused):
  number kind `g_BtlSlotAmountKinds` 1 and `BtlHitAmountSet()` (`0x80030070`); party slots re-pose from `g_BtlPartyHitAnimations`;
  enemy id 0x2F gets pose 7, its pending damage and strong mark cleared, number kind 2. Then
  `BtlAmountShowStart(0)` (`0x80088EA8`), `BtlAmountShowWait()` (`0x80031028`), `return BtlHitSlotsUpdate()` (`0x80030800`).
* **Nall's revive — `BtlSlot5Revive` (`0x80028E5C`)**. Sets `g_BtlSlot5Roaming = 0` —
  the slot-5 idle machine is OFF from here (frame-rng.md §"Draw site 1" stops
  drawing) and nothing here turns it back on. Walks Nall beside the member
  (approach walk), 60 frames (+ voice waits when arg0), 33 jitter frames,
  then HP = 10 if `StoryBitCheck(0x2AB)` (`0x80057778`) else 1, `+0x08` / `+0x0A` from
  their battle-entry copies, active. No direct draws —
  the 26/128 revive roll is in the caller `BtlSlot5ReviveTry` (`0x80027B6C`).
* **Victory — `BtlWinRun` (`0x80028140`)** (in full in §"Victory"), in order: drop list
  `g_BtlWinItems` and the seven bytes after it cleared; party slots with a status get `+0x1E &= 0xFB`
  (the 0x04 bit), status 0, re-activated, BEFORE any revive;
  inactive status-free ones get `BtlSlot5Revive(0)` (`0x80028E5C`); any revive ->
  `BtlFrameWait(0x14)` (`0x80036BA8`) (20 battle frames);
  then `BtlPartyPanelDisp` (`0x8004B330`), `BtlWinRewardSum` (`0x80028D78`) (EXP / silver), `BtlWinItemsGive` (`0x80028A54`)
  (item drops, whose rate checks are the victory's only RNG draws),
  `BtlWinMessageDisp` (`0x8004BB68`); then level-ups per slot (each: `bzero(g_ChrLevelUpGains, 12)`,
  `g_ChrLevelUpGains[0]` = current id, `ChrLevelUp(ORIGINAL id from party[], rec)` (`0x80028554`), HP/MP
  gains added to current and max, each capped 999, level +1, sound 0x1F, `BtlLevelUpMessageDisp` (`0x8004BC7C`) — frames); EXP
  capped 9999999. Only HP, MP and the level byte of the battle's CharStats change;
  the other stats change in the persistent record (§"Level-ups").

### Battle entry, the round's start and the revive roll

* **The revive roll — `BtlSlot5ReviveTry` (`0x80027B6C`)**. Runs every round
  (the round loop skips it on encounter 0xF9) only when slot 5 is Nall
  (`g_BtlSlotIds[5] == 0xC`). Draw 1: `rng_7f_draw()` (`0x8003A0B4`) (raw `& 0x7F`) `<
  0x1A` — **26/128**. Candidates: party slots 0..4 commandable
  (`g_BtlPartyNotHeldFlags`), with an id and at 0 HP, in slot order; draw 2
  `rng_mask_below(3, n)` (`0x8003A0D4`) only when there are 2+ (none for one). Both draws
  precede any frame. Then `BtlSlot5Revive(1)` (`0x80028E5C`) (Nall's revive), Nall walks
  back (target at her own x, y 0x60, 8x8, `BtlWalkRun` (`0x8002BB5C`)), pose 0, idle
  states reset and `g_BtlSlot5Roaming = 1` (frame-rng.md).
* **The party's plan — `BtlPartyActionAdjust` (`0x80029348`)**: in full in
  §"Confusion — status 0x40". A confused member's plan takes ONE raw draw,
  `r = rng_draw_copy()` (`0x8001C790`): index `r & 7`, or `r & 3` of the same
  draw when `r & 7` is 5 or more, so cases 1-3 are twice as likely as 0 and
  4. Only the KIND is stored, with target 100: the confusion handler
  `BtlActionStatus40Adjust` (`0x8002E9CC`) picks the spell or item again at
  action time, so a confused caster or item user pays those pick draws
  twice. `rng_list_pick` (`0x8003A03C`) (the pick) is one draw,
  `(raw & mask)` brought below n by subtraction, not rejection-sampled.
* **Stand poses / commandable tick** — `BtlPartyStandAnimationSet` (`0x80027CE4`): each
  active, status-free party slot gets `g_BtlSlotRestAnimations[i] = g_BtlPartyStandAnimations[i]` and
  that pose; `BtlPartyActionAnimationSet` (`0x800296F4`): each active, status-free,
  commandable one gets `BtlSlotActionAnimationSet()` (`0x80037E64`), then ONE battle frame
  (`BtlFrameStep`).
* **The escape run — `BtlPartyEscapeMove` (`0x80027EF0`)**: every active
  status-free member (`+0x15 & 0x4C == 0`) gets its walk pose
  `g_BtlPartyMoveAnimations[i]`, faces right, and moves +4 px a frame until all reach
  x >= 0x158 (at most 0x158 frames), one battle frame per frame; no draws.
* **Battle entry — `BtlInit` (`0x80029E68`)**: `func_8002A130` (empty), `BtlListClear` (`0x8002A10C`),
  `BtlPadClear` (`0x8002A0F4`), `pad_record_init(g_BtlPadRecord, 2, 0, 0)` (`0x8002687C`), then `BtlStateInit` (`0x8002B7DC`) (state
  init), `BtlEnemyLoad` (`0x8002B3AC`) (enemy setup), `BtlEnemyStatsSet` (`0x8002A638`), `BtlPartyStatsSet` (`0x8002A138`);
  **encounter 0xE4 clears slot 5's id** (no Nall) before `BtlPartyLoad` (`0x8002B114`)
  (the party bundle). Boss-class (>= 0xEB) -> `BtlSpecialInit` (`0x8002A7FC`) (enemy level
  scaling). Otherwise **story flag 0x2D5** (`StoryBitCheck` (`0x80057778`)) confuses every
  party slot (status 0x40, `BtlSlotStatusAnimationSet(i)` (`0x8003801C`)) and clears the flag. In
  every fight, **flag 0x2D0** -> `BtlPartyPairStatusSet` (`0x8002A010`): ONE draw at entry, before any
  frame, `rng_mask_below(7, 6)` (`0x8003A0D4`) = `(raw & 7)` brought below 6, picks a pair from
  `g_BtlPartyPairs` = (0,1) (0,2) (3,1) (3,2) (4,1) (4,2) (pairs 0-1 at 2/8, the
  rest 1/8); both slots get status 8 (sleep), `+0x1E = 4`,
  `BtlSlotStatusAnimationSet` (`0x8003801C`). Then display setup (`BtlMenuInit`, `tile_mix_fade_stop`,
  `BtlBgGreySet(0x80)`, `sprite_col_set(0x808080)`, `BtlBlackLinesRun`) and
  `g_BtlSlotActiveFlags[i] = (id != 0)` for 0..13, slot 5 forced inactive,
  `BtlPartyChoiceClear()` (`0x8004F6E4`).
* **State init — `BtlStateInit` (`0x8002B7DC`)**: actor table at 0x801D2E80,
  CharStats at 0x801D2B00, both cleared; ids and number kinds cleared for ENEMY slots
  6..13 only; per-slot tables cleared; party slots commandable; party AI
  off; `g_BtlAutoEndAllowed = 1`; `g_BtlRoundsLeft = 0xFF` (no round limit).
* **Enemy setup — `BtlEnemyLoad` (`0x8002B3AC`)**: `g_BtlNumberRows[id*2]` -> set
  `x` (`g_BtlEnemySet`, `BtlDataLoad(x)` (`0x80018694`), graphics ids at `g_BtlEnemySetIds +
  x*4`) and formation `y` (`g_BtlEnemyGroups + y*24`); per formation entry (up to
  8) into slots 6+: id, the graphics block's pointers,
  `BtlRecordInit(slot)` (`0x8002B770`), x = `row[i+8]*8`, y = `row[i+16]*8` clamped
  0x70..0xB0 (and copied to `+8`), facing 0x2020 when x >= 0xC0, pose 0.
* **Level scaling — `BtlEnemyLevelSet` (`0x8002ADF0`)**, from `BtlSpecialInit` (`0x8002A7FC`) per
  encounter with the formation's increments: level = `max(lvl, slot 0's
  +0x14)` capped at 50; per enemy slot 6..13 until the first
  empty: level byte; HP and max HP both set to HP + level x hp (both pinned at 0xFFFF if
  max + level x hp would pass it); `+0x08..+0x10` += level x increment (each pinned at
  0xFFFF); no draws. A boss template's
  own HP is 1 or a few hundred — this scaling is what makes the Water
  Dragon 911 HP at level 13.
* **The party bundle — `BtlPartyLoad` (`0x8002B114`)**: after `BtlPartyDataLoad`, the slot whose
  image goes to page 4 (`BtlPartyTallestFind()` (`0x8002B064`)) is chosen ONCE, before any page is allocated; party
  slots placed at the home tiles `g_BtlPartyPlaceXCells[i] * 8` / `g_BtlPartyPlaceYCells[i] * 8`
  (only read here), on their stand pose,
  unmirrored when the column < 0x10; slot 5 at x 0xA0, y 0x68, pose 5; then
  the idle machine's start, `g_BtlSlot5Roaming = 1` when slot 5 is Nall (frame-rng.md).

### The party's auto-battle AI

The party AI is the party half of `btl_choice.c` (`0x8004D294` to
`0x8005066C`). Its functions stage an action in three bytes,
`g_BtlPartyChoiceKind` (kind), `g_BtlPartyChoiceNumber` (number) and
`g_BtlPartyChoiceTarget` (target), which two entry points copy into the
slot's plan:

* `BtlPartyActionSet(mode)` (`0x8004D2C4`) plans at menu time, and
* `BtlPartyTurnActionSet` (`0x8004F734`) plans again at the member's own turn.

**The RNG.** The party AI has one draw site:
`BtlPartyOtherSideSpellSet` (`0x8004ECF4`), the spell pick, calls
`rng_ff_draw()` (`0x8004D294`) (`rng_draw() & 0xFF`) only for character id 8
(Ghaleon as a party member), and only when `BtlActingSpellListMake()` (`0x8004F054`) found a
castable spell. `r / 63` picks the single spell level: 0 -> 1 (and 4,
i.e. r >= 252: 67/256 in all), 1 -> 2, 2 -> 3, 3 -> 4 (63/256 each). That is
one draw per `BtlPartyOtherSideSpellSet` (`0x8004ECF4`) call. No other party-AI function, and
none of the functions they call (`BtlSlotNearestFind`, `BtlTargetTraitCheck`,
`BtlTargetRowsMark`, `StoryBitCheck`), draws or waits a frame, and none calls
through a pointer. (Ids 1/2/5/11 use levels 3/2/1, id 4 uses 7/6/5 unless
the enemy with the highest `+0x08` has `+0x2A & 4`, id 10 uses 1: no draw.)

**The sums.** `party` below is `g_BtlPartyStat08Sum`, the sum of the `+0x08` words of
the stats of the party slots that have an id and HP (the stat a hit's
amount starts from; `include/btl.h`), and `enemy` is `g_BtlEnemyStat08Sum`, the same over
slots 6..13. The live counts are `g_BtlPartyCount` (party) and `g_BtlEnemyCount` (enemy).
`BtlPartyStat08Sum` (`0x8004F5E4`) and `BtlEnemyStat08Sum` (`0x8004F664`) make them.

* **The menu-time pass — `BtlPartyActionSet(mode)` (`0x8004D2C4`).** It stores
  `mode` in the byte after `g_BtlEnemyCount`, clears the spell list, the found
  slot and need, the staged choice and the sums, and makes the sums again.
  Its callers:
  - `BtlPartyActionSet(1)`: the main menu's AI command (`BtlMenuFirstStep` (`0x80044B5C`),
    which also sets `g_BtlPartyMode` = 1), and `BtlRun` (`0x80027434`) every round
    while `g_BtlPartyMode` is 1, in place of the menu.
  - `BtlPartyActionSet(0)`: one member's AI command in its own command grid
    (`BtlMenuKindStep` (`0x80045EDC`), command 1), and a Tactics row's cell of kind 1
    (`BtlMenuPresetRowActionSet` (`0x80044E24`)).

  With `mode` set, it walks `g_BtlActingSlot` over slots 0..4. A slot with an
  id, HP, a non-zero `g_BtlPartyNotHeldFlags` byte and a status byte other than
  4, 8 or 0x30 runs `BtlPartyAllChoiceSet` (`0x8004DB30`) and takes the staged kind
  and number. Any other slot with an id gets 5/0xFF. A planned kind of 0
  then hangs (an endless loop). The staged choice is not cleared between
  members, so a member whose handler stages nothing inherits the previous
  member's choice. With `mode` 0 it runs `BtlPartyOneChoiceSet` (`0x8004D61C`) for
  `g_BtlActingSlot` alone, with no test of the slot and no hang. In both modes
  the planned target `g_BtlSlotPlannedTargets` is 0xFE, not the staged target. The kept
  copies `g_BtlPartyKeptKinds`, `g_BtlPartyKeptNumbers` and `g_BtlPartyKeptTargets` get the same three values.
* **Per-slot handler — `BtlPartyOneChoiceSet` (`0x8004D61C`)** (mode 0). It plans for slot
  `g_BtlActingSlot`, with character id `g_BtlSlotIds[slot]`. Two shorthands
  below: "the spell path" is `if (BtlPartyOtherSideSpellSet()) BtlPartyAttackChoiceSet()`
  (`0x8004ECF4`, `0x8004F494`): attack when no spell was staged. "The heal path" is
  `if (!BtlPartyHpLeastFind() || BtlPartyNeedItemSet(9, g_BtlPartyFoundSlot)) BtlPartyAttackChoiceSet()`
  (`0x8004E44C`, `0x8004F2B4`): attack when no one is hurt or no healing item was staged.
  1. When `BtlSlotHpLowCheck(slot)` (`0x8004E318`) answers (HP below a quarter of
     its maximum), `BtlPartyNeedItemSet(9, slot)` (`0x8004F2B4`) tries a healing item.
     When that stages nothing:
     - ids 3 and 6 stage DEFEND (5/0xFF/0xFF: the walk to a corner and the
       0x13 stance, §"Retreat, the magic arm and the fallbacks") when
       `BtlPartyNeedSpellSet(9, slot)` (`0x8004EAE0`) stages nothing either;
     - other ids, when `BtlPartyOtherSideSpellSet()` (`0x8004ECF4`) stages nothing,
       stage defend if 2+ members live, else `BtlPartyLeaveChoiceSet()` (`0x8004E2B8`).
       That stages RUN (6/0xFF/0xFF), except that with one enemy left and
       `g_BtlPartyStat08Sum` below `g_BtlPartyCount` it attacks instead.

     The handler returns here either way.
  2. Else, for ids other than 7 and 8, when `BtlSlotMpLowCheck(slot, 0)` (`0x8004E348`)
     answers (MP below an eighth of its maximum):
     `BtlPartyAttackChoiceSet()` (`0x8004F494`) when `BtlPartyNeedItemSet(8, slot)` (`0x8004F2B4`)
     answers non-zero, which it always does (below). Then it returns.
  3. Else, by id:
     - ids 1 and 11: the spell path when `party/3*2 < enemy`, or
       `BtlEnemyFourIdsCheck()` (`0x8004E194`) is 1, or the battle number `g_BtlNumber` is
       0xDF or more; else `BtlPartyAttackChoiceSet` (`0x8004F494`).
     - ids 2 and 10: the spell path when `party/3*2 < enemy` or the
       battle number is 0xDF or more; else the heal path.
     - id 4: the spell path when `party/3*2 < enemy` or `BtlEnemyFourIdsCheck() == 1` (`0x8004E194`); else the heal path.
     - id 5: the spell path when `party/2 < enemy`, `BtlEnemyFourIdsCheck() == 1` (`0x8004E194`) or the battle
       number is 0xDF or more; else the heal path.
     - id 8: the spell path when `party/2 < enemy` or the battle number is
       0xDF or more; else `BtlPartyAttackChoiceSet` (`0x8004F494`).
     - ids 3 and 6, the healers, in this order:
       a. When `BtlPartyHpLeastFind()` (`0x8004E44C`) finds someone, attack when
          `BtlPartyNeedSpellSet(9, g_BtlPartyFoundSlot)` (`0x8004EAE0`) and
          `BtlPartyNeedItemSet(9, g_BtlPartyFoundSlot)` (`0x8004F2B4`) both stage nothing.
       b. Else, when `BtlPartyStatusFind()` (`0x8004E91C`) finds someone, attack when
          `BtlPartyNeedSpellSet(g_BtlPartyFoundNeed, g_BtlPartyFoundSlot)` (`0x8004EAE0`) and
          `BtlPartyNeedChoiceSet()` (`0x8004DFA8`) both stage nothing.
       c. Else id 3 attacks. Id 6 attacks too, unless exactly one member lives. Then
          she stages MAGIC kind 3, number 0x24, target 0xFF when `enemy > party`,
          `BtlActingSpellListMake()` (`0x8004F054`) finds a castable spell and
          `BtlActingSpellUsableGet(4)` (`0x8004EF4C`) answers; otherwise nothing is staged.
     - ids 7 and 9: the heal path.
     - 0 and above 11: nothing.
* **The all-members handler — `BtlPartyAllChoiceSet` (`0x8004DB30`)** (mode 1). A
  different tree, with these thresholds:
  - ids 1 and 11: `party/3*2 < enemy`, `BtlEnemyFourIdsCheck() == 1` (`0x8004E194`) or the battle number 0xDF or more;
  - id 2: `party/3 < enemy` or 0xDF;
  - id 4: `party/3 < enemy`, `BtlEnemyFourIdsCheck() == 1` (`0x8004E194`) or 0xDF;
  - id 5: `party/2 < enemy`, `BtlEnemyFourIdsCheck() == 1` (`0x8004E194`) or 0xDF;
  - id 8: `party/3 < enemy` or 0xDF;
  - id 10: `party/3*2 < enemy` or 0xDF.

  When the test holds they take the spell path. Otherwise
  `BtlPartyNeedChoiceSet()` (`0x8004DFA8`) runs, and they attack when it answers non-zero.

  Ids 7 and 9, when `BtlPartyHpLeastFind()` (`0x8004E44C`) finds someone, attack when
  `BtlPartyNeedItemSet(9, g_BtlPartyFoundSlot)` (`0x8004F2B4`) stages nothing; otherwise they take
  the `BtlPartyNeedChoiceSet` (`0x8004DFA8`) arm.

  Ids 3 and 6 go in this order:
  1. When someone is hurt (`BtlPartyHpLeastFind` (`0x8004E44C`)) or holds a status
     (`BtlPartyStatusFind` (`0x8004E91C`)), they stop when `BtlPartyNeedSpellSet` (`0x8004EAE0`) stages a spell.
     Otherwise they attack when `BtlPartyNeedChoiceSet()` (`0x8004DFA8`) answers non-zero.
  2. With neither, id 6 with exactly one member alive stages kind 3, row 0x24
     when `enemy > party`, a spell is castable and `BtlActingSpellUsableGet(4)` (`0x8004EF4C`)
     answers. When the row is not castable she stages nothing. When either of the
     first two tests fails she attacks.
  3. Otherwise they attack unless `BtlSlotMpLowCheck(BtlPartyMpSlotGet(), 1)` (`0x8004E348`)
     answers. `BtlPartyMpSlotGet` (`0x8004E2B0`) returns 0, so the test is slot 0's MP
     below half. When it answers, `BtlPartyNeedItemSet(8, 0)` (`0x8004F2B4`) answers 1 and they attack anyway.

  Any other id: nothing.
* **The turn pass — `BtlPartyTurnActionSet` (`0x8004F734`).** `BtlActionRun` (`0x8002E088`), the
  action loop's per-actor step, calls it before `BtlActionAdjust` (`0x8002E914`) when
  `g_BtlActingSlot < 5 && g_BtlTargetSlot == 0xFE`. So every plan that
  `BtlPartyActionSet` (`0x8004D2C4`) made, in either mode, is made again at the member's own
  turn, from the board as it stands.
  - It stores 0 in the byte after `g_BtlEnemyCount`, clears the staged choice and
    makes the sums again.
  - A member that is dead, on hold (`g_BtlPartyNotHeldFlags` 0) or has status 4 /
    8 / 0x30 gets 5/0xFF/0xFF.
  - Otherwise it works by the planned kind. "The fallback" is
    `BtlPartyWornItemSet()` (`0x80050574`), then `BtlPartyAttackChoiceSet()` (`0x8004F494`) when that staged
    nothing.
    - **2**: when `party/3*2 < enemy` and live enemies outnumber live
      members, `BtlPartyNeedItemChoiceSet()` (`0x8004FABC`). Its staged item stands; when it
      stages nothing, or the test fails, the fallback runs.
    - **4**: `BtlPartyNeedItemChoiceSet()` (`0x8004FABC`); its item stands, else the fallback.
    - **3**: `BtlPartyPlannedSpellChoiceSet()` (`0x8004FC24`).
    - **5**: when `enemy < party/3*2` and `live members −
      BtlPartyPlannedCount() < live enemies` (`0x8004FE5C`):
      `BtlPartyNeedItemChoiceSet()` (`0x8004FABC`) staging nothing gives 5/0xFF/0xFF, else its
      item stands. When the test fails, the fallback runs.
    - **6, 10, 11**: nothing is staged, so the plan becomes kind 0.
    - **0, 1, 7, 8, 9 and anything above 11**: an endless loop.
  - The staged choice goes to `g_BtlSlotPlannedKinds`, `g_BtlSlotPlannedNumbers` and `g_BtlSlotPlannedTargets`;
    to `g_BtlActionKind`, `g_BtlActionNumber` and `g_BtlTargetSlot`; and to the kept copies.
* **`BtlPartyNeedItemChoiceSet` (`0x8004FABC`)** answers 0 when it stages an item, else 1.
  Each step runs only while nothing is staged:
  1. When `BtlSlotHpLowCheck(actor)` (`0x8004E318`) answers, it tries `BtlPartyNeedItemSet(9, actor)` (`0x8004F2B4`).
  2. When `BtlPartyHpLeastFind()` (`0x8004E44C`) finds someone, it tries
     `BtlPartyNeedItemSet(9, g_BtlPartyFoundSlot)` (`0x8004F2B4`).
  3. When `BtlPartyStatusFind()` (`0x8004E91C`) finds someone, it tries
     `BtlPartyNeedItemSet(g_BtlPartyFoundNeed, g_BtlPartyFoundSlot)` (`0x8004F2B4`).
  4. It takes the first party slot n that has low MP
     (`BtlSlotMpLowCheck(n, 0)` (`0x8004E348`)) and is not already the target of an earlier
     slot's spell or item (`BtlSlotPlannedTargetCheck(n)` (`0x8004E3C8`) answers 0), for an actor
     whose id is not 7 or 8. It answers `BtlPartyNeedItemSet(8, n) != 0` (`0x8004F2B4`), which is
     always 1.

  `BtlPartyNeedChoiceSet` (`0x8004DFA8`) runs steps 2 to 4 (not step 1), after a step of
  its own. For ids 3 and 6, when `BtlPartyHpShortTwoCheck()` (`0x8004E204`) answers and a
  spell is castable, it stages kind 3 with `BtlActingSpellUsableGet(2)` (`0x8004EF4C`) (id 3)
  or `(6)` (id 6) at target 0xFF.
* **The stagers.**
  - **`BtlPartyAttackChoiceSet` (`0x8004F494`)** works in this order:
    1. It finds `t = BtlSlotNearestFind(1)` (`0x80039838`), the nearest enemy, first, always.
    2. A status of exactly 0x20 stages DEFEND.
    3. Ids 2, 4 and 6 first try `BtlPartyWornItemSet()` (`0x80050574`). When that stages the
       worn weapon it stands and the function returns. Otherwise
       `BtlActingStat20Not3Or4Check()` (`0x80050630`) answering 1 stages defend: it answers 1 when the
       first worn item's kind (`+0x20`) is neither 3 nor 4.
    4. Ids 1, 0xB, 4, 5 and 3 defend when the target's id is 0x2F, 0x38, 0x3E or 0x3F.
    5. Otherwise it stages ATTACK: kind 2, number 0xFF, target t.
  - **`BtlPartyNeedItemSet(need, target)` (`0x8004F2B4`)** stages an ITEM (kind 4) at `target`.
    The candidates by `need`:

    | `need` | Candidates |
    |---|---|
    | 0 | 0xA2, then 0xA3 |
    | 2 | 0xA1, then 0xA3 |
    | 3..7 | 0xA3 |
    | 9 | 0x9C, then 0x9D; 9 becomes 10 when the `g_BtlPartyFoundSlot` member (not `target`) is at 0 HP |
    | 10 | 0xA0 |
    | 8 | none: it answers 1 and stages nothing, so no AI step that asks for need 8 ever stages an item |
    | 1 and above 10 | none tried: it stages kind 4 with item 0 and answers 0 |

    `BtlActingItemMissingCheck(item)` (`0x8004F3F8`) answers 0 when the item is among the
    acting slot's six carried bytes (`+0x2E`). It answers 1 for every item while story
    flag 0x3D9 is set (`StoryBitCheck` (`0x80057778`)). The first held candidate is staged.
    The answer is the last check's (non-zero = nothing to use).
  - **`BtlPartyWornItemSet` (`0x80050574`)** stages the member's first worn item
    as an item, at the enemy `BtlEnemyStat08HighestFind` (`0x8004F218`) returns. It does so only
    when story flag 0x3D9 is clear, the level is below 26 and the item is
    0x23, 0x24, 0x25, 0x27, 0x14 or 0x15.
* **The tests.**
  - **`BtlPartyHpShortTwoCheck` (`0x8004E204`)** answers 1 when two or more slots
    have HP that is not 0 and is below `maxHP / 3 * 2`. It looks at slots 0 up to
    `g_BtlPartyCount` − 1: the live count bounds the slot number, so with an
    empty or fallen slot, that many of the last slots are not looked at.
  - **`BtlPartyHpLeastFind` (`0x8004E44C`)** finds the party slot with the least HP below 2/3
    of its maximum, dead members included. **`BtlPartyHpNot0LeastFind` (`0x8004E6A0`)** does
    the same, leaving out slots at 0 HP. Both:
    - skip a slot that `BtlSlotPlannedTargetCheck(i)` (`0x8004E3C8`) answers 1 for, while the byte after
      `g_BtlEnemyCount` is set;
    - keep the running least in `g_BtlPartyFoundNeed`, from 0xFF, so only a member
      with HP below 0xFF can be found;
    - store the slot in `g_BtlPartyFoundSlot`, then set `g_BtlPartyFoundNeed = 9`, and
      answer found;
    - write the loop twice, under `g_BtlNumber >= 0xDF` and not, statement for
      statement the same.
  - **`BtlPartyStatusFind` (`0x8004E91C`)** finds the first slot with a status, with the
    same skip. Its need is `BtlSlotStatusNeedGet` (`0x8004E9F4`).
  - **`BtlSlotPlannedTargetCheck(t)` (`0x8004E3C8`)** answers 1 when a slot numbered below the
    acting slot has `t` as its planned target with a planned kind of 3 or 4.
  - **`BtlActingSpellListMake` (`0x8004F054`)** builds the actor's castable-spell list at
    `g_BtlActingSpellList[0..7]`, in the order of the stats' eight spell bytes (`+0x34`). A spell
    is castable when MP > cost (strict), or MP >= `(cost + 1) >> 1` when
    `BtlTargetTraitCheck(0, 0x1B, 0, 0) == 1` (`0x80037648`). It answers 1 when none is castable.
  - **`BtlActingSpellUsableGet(arg)` (`0x8004EF4C`)** takes `r = BtlActingSpellNumberGet(arg)` (`0x800504E4`). It
    answers r when an entry of that list equals it, else 0. Under a status of exactly
    0x20, r counts only when `BtlSpellByte0BGet(r) == 0` (`0x8005066C`).
  - **`BtlActingAreaEnemyCount(spell)` (`0x8004FEBC`)** counts the live enemies 6..13 inside
    the spell's area rectangle around the caster (`|dx| <= row[9]*8`, `|dy| <=
    row[10]*8`, inclusive).
  - **`BtlEnemyAreaMostFind(spell)` (`0x800500F0`)** does the same count around each live
    enemy, itself included. It keeps the first enemy with the most in
    `g_BtlEnemyMostSlot` / `g_BtlEnemyMostCount`.
  - **`BtlEnemyRowsMostFind` (`0x8004FFA0`)** does the same with the lane band of
    `BtlTargetRowsMark` (`0x80035260`) (below). For each live enemy as `g_BtlTargetSlot` it clears
    `g_BtlSlotAmountKinds`, marks the band and counts the marked enemies, keeping the first
    best in `g_BtlEnemyMostSlot` / `g_BtlEnemyMostCount`. Then it clears the marks, sets
    `g_BtlTargetSlot` to 0 and returns the count.
  - **`BtlEnemyStat08HighestFind` (`0x8004F218`)** returns the live enemy slot with the highest
    `+0x08`, or 0 when there is none. Its argument is unused. The running best is kept as a byte, so
    after a value of 256 or more, later slots are compared with its low
    byte. The compare is a strict `<`, so the first maximum wins.
* **The spell pick — `BtlPartyOtherSideSpellSet` (`0x8004ECF4`)** answers 1 when it stages
  nothing.
  - It needs a castable spell (`BtlActingSpellListMake` (`0x8004F054`)).
  - It picks three levels by id (above). For any id but 1, 2, 4, 5, 8, 10 and 11
    they are left unset. Ids 3 and 6 never call it; `BtlPartyOneChoiceSet` (`0x8004D61C`)'s
    low-HP step calls it for ids 7 and 9.
  - When the actor's own planned target is 0xFE, it stages kind 3 with
    `BtlPartySpellOfThreeGet(a, b, c)` (`0x8005022C`). The target is `g_BtlEnemyMostSlot` when that
    is set, else `BtlEnemyStat08HighestFind()` (`0x8004F218`).
  - Otherwise it stages the last castable of the three (c over b over a),
    at target 0xFE.
* **`BtlPartySpellOfThreeGet(u8, u8, u8)` (`0x8005022C`).** Its candidates a, b and d are
  `BtlActingSpellUsableGet` (`0x8004EF4C`) of each argument (0 = not castable). The id is the
  actor's character id. Choosing d always clears `g_BtlEnemyMostSlot`.
  - **a set, id 5, b set**: a when `BtlActingAreaEnemyCount(b) < BtlEnemyRowsMostFind()`
    (`0x8004FEBC`, `0x8004FFA0`) and a second `BtlEnemyRowsMostFind()` (`0x8004FFA0`) is >= 2. Else b when d is 0
    or a second `BtlActingAreaEnemyCount(b)` (`0x8004FEBC`) is >= 2. Else d.
  - **a set, id 5, no b**: d when d is set and `BtlEnemyRowsMostFind() < 2` (`0x8004FFA0`), else a.
  - **a set, other ids**: a when `BtlEnemyCountBelow4Check()` (`0x8004F048`) returns 0. Otherwise:
    - ids 1/2/4/10/11 with b call `BtlEnemyAreaMostFind(b)` (`0x800500F0`). Then: b when
      `g_BtlEnemyMostCount >= 2`; else d when set; else b when `g_BtlEnemyMostCount >= g_BtlEnemyCount` (live
      enemies); else a.
    - The same ids without b: d when set, else a.
    - Id 8: a. Any other id: 0.
  - **a unset, b set**:
    - id 5: d when `BtlActingAreaEnemyCount(b) < 2` (`0x8004FEBC`) and d is set, else b;
    - ids 1/2/4/10/11: call `BtlEnemyAreaMostFind(b)` (`0x800500F0`), then d when
      `g_BtlEnemyMostCount < 2` and d is set, else b;
    - other ids: 0.
  - **a and b unset**: d when set, otherwise an endless loop.
* **Lane marking — `BtlTargetRowsMark` (`0x80035260`)** (`btl_target.c`). It takes no argument;
  `btl_choice.c`'s caller passes one, which is ignored. The side scanned is set by the actor:
  confused -> 0..13; party actor -> 6..13; enemy -> 0..4. Every active slot
  whose y (`+0xA`) lies in `[y - (span + 8), y + span]` gets `g_BtlSlotAmountKinds = 1`.
  Here y is the target `g_BtlTargetSlot`'s y, and `span = g_BtlAreaHalfHeight * 8`. It is a pure
  vertical band: there is no x, width or facing test, and `g_BtlAreaHalfWidth` is not read. The
  actor's own slot is not skipped, so a confused actor can mark itself.

### Item use and the menu-time auto-battle

* **Item use is kind 4; the item row.** `BtlActionRowRun` (`0x80031ABC`) case 4 reads the item's 0x12-byte row
  (`g_BtlItemRowsByNumber + id * 0x12`, the same row as `g_BtlItemRows`' row id − 1).
  - `+0x08` goes to `g_BtlActionFunctionNumber` (the effect id).
  - `+0x09` goes to `g_BtlActionShowNumber` (the id `BtlShowNumberRun` (`0x8007BB80`) presents).
  - For ids `>= 0x9C` only, `+0x0A` goes to `g_BtlActionPower` (the magnitude).
  - `+0x06` is the target mode (`BtlActionTargetAdjust` (`0x8002ED34`), `BtlMenuActionCheck` (`0x800493F8`)).

  Case 4 loads nothing else. In battle, `BtlTurnOrderRun` (`0x8002DC30`) zeroes `g_BtlActionPower`,
  `g_BtlAreaHalfWidth`, `g_BtlAreaHalfHeight`, `g_BtlActionBitNumber`, `g_BtlActionFunctionNumber` and `g_BtlActionShowNumber`
  before every action, so an item below 0x9C acts with a power of 0.

  The AI's consumables are ids `0x9C..0xA3`. Their effects:

  | Effect | What it does | Function |
  |---|---|---|
  | 4 | HP + 50 / + 150 + `rng_mask_below(7, 8)` (`0x8003A0D4`, a draw of 0..7), held to the maximum | `BtlTargetHpAddRun` (`0x80031FB4`) |
  | 1 | MP + 30 + the same draw, the same arithmetic on `+0x04` / `+0x06` | `BtlTargetMpAddRun` (`0x80031E00`) |
  | 2 | MP to full | `BtlTargetMpFillRun` (`0x80031E84`) |
  | 12 | revive | `BtlTargetReviveRun` (`0x800324A4`) |
  | 10 | clear status bit `0x04` (`BtlTargetStatusClearRun(4)` (`0x80032440`)) | `BtlTargetStatus4ClearRun` (`0x80032398`) |
  | 9 | clear status bit `0x01` (`BtlTargetStatusClearRun(1)` (`0x80032440`)) | `BtlTargetStatus1ClearRun` (`0x80032378`) |
  | 11 | clear every status | `BtlTargetStatusAllClearRun` (`0x800323B8`) |

  The weapons `0x14 0x15 0x23..0x27` carry effects 87 / 88 and target rule
  `0x91` (enemies); battle-presentations-2.md §"Group E — the usable weapons"
  has them. `BtlPartyTurnActionSet` (`0x8004F734`) and `BtlPartyAttackChoiceSet` (`0x8004F494`) stage them
  through `BtlPartyWornItemSet` (`0x80050574`), for a character under level 26 whose weapon is
  one. Not all seven are staged: the test is `weapon − 0x23 < 3` or 0x27, 0x14,
  0x15, so the Ice Cane (0x26) is never staged.
* **`BtlActionItemRun` (`0x8002E7FC`)** is the item arm of `BtlActionKindRun` (`0x8002E12C`). It runs after
  `BtlActionStringDisp` (`0x8004987C`) has shown string `id | 0x100`.
  1. It runs `BtlActionRowRun` (`0x80031ABC`) first.
  2. Only for `id >= 0x9C`, the first of the six carried bytes (`+0x2E`) equal
     to the id is closed up (later ones move down one).
  3. `+0x33` is zeroed, also when nothing matched. For a party item
     `BtlActionTargetAdjust` (`0x8002ED34`) has already made sure of a match.
* **`BtlActionTargetAdjust` (`0x8002ED34`) kind 4**:
  1. Target 100 -> `BtlActionFallbackSet` (`0x8002F3A0`).
  2. An id `>= 0x9C` must be among the six carried bytes, else
     `BtlActionFallbackSet` (`0x8002F3A0`). An id below 0x9C is not looked up.
  3. Target 0xFF -> `BtlTargetByModeGet(item[+6])` (`0x800397B4`); any other target ->
     `BtlTargetModeAdjust(item[+6])` (`0x8002EB78`).

  `BtlTargetModeAdjust` (`0x8002EB78`) reads the modes as a spell row's `+0x01 & 0xF`:
  - 3 re-picks an active target, keeping an inactive one;
  - 4 keeps a target that has an id and holds a status, and re-picks any other;
  - every other mode first re-picks an inactive target; then 5 re-picks a
    member at max HP, 6 one at max MP, 7 one holding a status, and 0 keeps it.
* **`BtlMenuActionCheck` (`0x800493F8`), command 4.** It answers 0 for a status `+0x15 & 0x4C`, or
  `& 0x30 == 0x30`. It answers 0 while `StoryBitCheck(0x3D9)` (`0x80057778`) is set: a story flag that bans
  items (clear on a new game). The same flag disables the item cell of the
  battle command grid (`BtlMenuKindOpen` (`0x800459D4`)), the AI's item stagers and the first
  of the field menu's three item commands (`MenuItemCommandStep` (`0x8006DD94`)). Its fourth argument
  `flag`, when not 0, then answers 1 at once. Otherwise:
  - the item must be among the six equipped bytes (`+0x22`) or the six carried (`+0x2E`);
  - its row's `+0x08` must be non-zero;
  - then `g_BtlMenuTargetMode = +0x06 & 0xF` and `g_BtlMenuTargetMask =
    BtlMenuTargetMaskGet(mode)` (`0x80048AE8`); the item is usable when the mode is 0 or the candidate mask is
    non-zero.
* **The item list** (`BtlMenuItemOpen` (`0x80046C98`), `BtlMenuItemStep` (`0x800472B4`), `BtlMenuItemDisp` (`0x80046BB4`)).
  - **Cells.** Cells 0..5 are the equipped items, at (i * 4 + 0xD, 7). Cells 6..11
    are the carried ones, at y 0xB. The icon is the item id, and a cell
    `BtlMenuActionCheck` (`0x800493F8`) refuses is marked disabled. Cell 12 is the Item icon (0xEC), at (3, 2).
  - **Labels.** Label slot 0, at (5, 3), is never written by `BtlMenuItemOpen` (`0x80046C98`), so it
    still holds the command grid's "Command". Slots 5 and 0x11 carry the cursor
    cell's name (`id | 0x100`) and description (`id | 0x200`), in style 4 when it is
    unusable. An empty cell or list shows string `0x60C` in slot 5, 0x18 cells wide.
    With no item at all, the cursor record is placed at (0x68, 0x38).
  - **The cursor.** It steps by `BtlMenuCursorMove(pad, cells, 6, 2, cur)` (`0x80048D2C`). LEFT and RIGHT wrap inside the row
    and walk past absent cells. UP and DOWN change row and walk left from where they
    land to the first item. From column 0 they walk once more, from the row's end.
    When that ends on column 0 too, the cursor stays at `cur`, so a row with no
    item is not entered. `g_BtlMenuItemKeptCursors[character]` remembers the cursor.
  - **CROSS.** On an absent or disabled cell, CROSS only asks for sound 4. Otherwise it sets
    `g_BtlSlotPlannedNumbers[actor] = icon` and runs `BtlMenuActionCheck` (`0x800493F8`) again. When
    `g_BtlMenuTargetMask != 0` it goes to the target window (state 0xB). Otherwise
    it goes on to the next character, writing no target: the slot keeps the 0 that
    `BtlMenuKindOpen` (`0x800459D4`) stored when its command grid opened.
* **The plan is made twice.** Both modes of `BtlPartyActionSet` (`0x8004D2C4`) store the
  placeholder target `0xFE` in `g_BtlSlotPlannedTargets`, dropping the staged target
  (`g_BtlPartyChoiceTarget`). `BtlActionRun` (`0x8002E088`) then calls `BtlPartyTurnActionSet` (`0x8004F734`) for a
  member whose target is 0xFE, so each AI plan is made again at the member's
  own turn (§"The party's auto-battle AI").
  - **The byte after `g_BtlEnemyCount`** is set in mode 1 and 0 in the turn pass.
    `BtlSlotPlannedTargetCheck` (`0x8004E3C8`)'s "an earlier slot already targets it" test compares
    against the earlier slots' `g_BtlSlotPlannedTargets`. The finds ask it only in the mode-1 pass,
    where those targets hold 0xFE, so there it never fires. Only
    `BtlPartyNeedChoiceSet` (`0x8004DFA8`) and `BtlPartyNeedItemChoiceSet` (`0x8004FABC`) ask it in the turn pass,
    where an earlier slot that has acted holds a real target.
  - **The spell-of-three branch.** The tick (`BtlRoundStatusUpdate` (`0x800297A4`)) clears
    `g_BtlSlotPlannedTargets` every round, and `BtlPartyActionSet` (`0x8004D2C4`) writes 0xFE only after the
    handler returns. So the branch of `BtlPartyOtherSideSpellSet` (`0x8004ECF4`) for
    `g_BtlSlotPlannedTargets[actor] == 0xFE` is reached in the turn pass, or at menu time only for a slot
    already planned this round. That branch is `BtlPartySpellOfThreeGet` (`0x8005022C`), which chooses
    a target through `g_BtlEnemyMostSlot`. In the turn pass, `BtlTurnOrderRun` (`0x8002DC30`) has just
    zeroed `g_BtlAreaHalfHeight`, the span that `BtlTargetRowsMark` (`0x80035260`), and so
    `BtlEnemyRowsMostFind` (`0x8004FFA0`), uses: the band is `[y − 8, y]`.
  - **Kind 0 in the turn pass.** Planned kind 6 (`BtlPartyLeaveChoiceSet` (`0x8004E2B8`) is
    mode 0 only) and kinds 10 / 11 leave the turn pass with kind 0.
* **Hangs and unset reads.**
  - `BtlActingSpellNumberGet` (`0x800504E4`) loops forever for a character id with no arm (0, 7,
    9, 12 and up). `BtlPartyPlannedSpellChoiceSet` (`0x8004FC24`) loops forever for ids 7 and 9;
    any other id without an arm returns.
  - `BtlPartySpellOfThreeGet` (`0x8005022C`) loops forever when nothing is castable, and
    `BtlPartyActionSet` (`0x8004D2C4`) does on a mode-1 plan of kind 0.
  - A mode-0 plan of kind 0 reaches the endless loop of `BtlPartyTurnActionSet` (`0x8004F734`) at the
    member's turn. `BtlPartyOneChoiceSet` (`0x8004D61C`) leaves kind 0 for a lone id 6 whom
    nothing else moves, when `enemy > party` does not hold or row 0x24 is not
    castable. Nothing between the menu and the turn changes that plan:
    `BtlPartyActionAdjust` (`0x80029348`) rewrites only confused and held members.
  - `BtlSlotStatusNeedGet` (`0x8004E9F4`) returns its second argument, which its caller
    `BtlPartyStatusFind` (`0x8004E91C`) never passes, for a status byte that is not one of 0 / 1 /
    4 / 8 / 0x10 / 0x20 / 0x30 / 0x40 / 0x80 (poison + sleep, say).
  - `BtlPartyOtherSideSpellSet` (`0x8004ECF4`) leaves its spell levels unset for the ids it has
    no arm for (above).
* **What follows from the tree's shape.**
  * The tree names spell rows by character (`BtlActingSpellNumberGet` (`0x800504E4`): a level index
    plus the character's base), never from the learned list. The learned list only
    says whether the row is castable. 28 rows in all: 1-3 (ids 1, 11),
    9-0xB (2), 0xD 0xE 0x11 0x12 (3), 0x15-0x17 and 0x19-0x1B (4),
    0x1D-0x1F (5), 0x21 0x22 0x24 0x26 (6), 0x27-0x2A (8), 0x2F (10). The
    AI plans no other row: Vigor, the dragon spells, Sleep, Stone, the raises, Saint /
    Althena / Fear Litany, Cascade / Temptation Song, Flash Wind and the rest.
  * `BtlPartySpellOfThreeGet` (`0x8005022C`) (ids 1, 2, 4, 10, 11) takes its first row, the
    all-enemies row and the highest of the three, when four or more enemies
    stand (`BtlEnemyCountBelow4Check` (`0x8004F048`) answers 0). Against fewer:
    - it takes the area row when `BtlEnemyAreaMostFind` (`0x800500F0`) finds two or more
      enemies inside it;
    - else the single-target row when that is castable;
    - else the area row when its count reaches the live enemies;
    - else the first row.

    With the area row not castable, it takes the single-target row, else
    the first. Id 5 weighs its three by the `BtlActingAreaEnemyCount` (`0x8004FEBC`) /
    `BtlEnemyRowsMostFind` (`0x8004FFA0`) counts. Character 4 takes levels 3/2/1 (rows 0x15-0x17, the fire set)
    when the enemy with the highest `+0x08` has bit 4 of its resist mask (`+0x2A & 4`), else
    levels 7/6/5 (rows 0x19-0x1B, the ice set).
  * **The AI never sings Escape Song.** Character 6's lone-and-out-hit arm
    in `BtlPartyAllChoiceSet` (`0x8004DB30`) plans row 0x24 at menu time. But her turn makes the
    plan again, and the arm of `BtlPartyPlannedSpellChoiceSet` (`0x8004FC24`) for id 6 stages only
    row 0x26 (`BtlPartyHpShortSpellSet` (`0x8004FD70`)), or 0x21 or 0x22 (`BtlPartyNeedSpellSet` (`0x8004EAE0`)), or
    nothing. The menu-time arm matters all the same. When the enemies out-hit her and
    row 0x24 is not castable there (below level 12, or 10 MP or less) while
    another row is, it stages nothing at all, and `BtlPartyActionSet` (`0x8004D2C4`) hangs on
    the kind-0 plan.
  * A member put to sleep after the menu-time pass (`BtlTargetStatus8Set` (`0x8002F958`) writes
    kind 0xB into its plan) and woken or cured before its turn does nothing that
    round: the turn pass's arm for 11 stages nothing, leaving kind 0.
  * `BtlPartyHpLeastFind` (`0x8004E44C`) ranks the dead among the hurt (least HP first). So a
    fallen member makes every non-healer reach for item 0xA0 instead of using a
    Herb on someone living. Not carrying one, `BtlPartyOneChoiceSet` (`0x8004D61C`)'s heal path
    attacks, and `BtlPartyNeedChoiceSet` (`0x8004DFA8`) and `BtlPartyNeedItemChoiceSet` (`0x8004FABC`) pass on to the
    status cures.
* **Stop.** `BtlFrameStep` (`0x80038D2C`) clears `g_BtlPartyMode` and calls `BtlWindow1IconClose` (`0x800497D4`)
  while TRIANGLE is held (`BtlPadHeldCheck(0x10)` (`0x80037604`)), `g_BtlAutoEndAllowed` is set and the AI is on
  (`g_BtlPartyMode` 1). The round in progress still runs: members whose target
  is 0xFE plan again.

### Thirteen enemy actions in detail, and the shot

The detail behind thirteen rows of the handler table in §"The enemy
actions", with the shot several of them fly. The handlers 0x97 and 0x9B
are in `btl_acting.c`; the others, the shot and the helpers below are in
`btl_enemy.c`. None of them draws from the RNG itself, and none starts
`g_BtlActuatorRandomList`, the one vibration script that draws (the
register's reading of it: the one list the battle starts with the random
way 0x48); their frames are the frame waits of their callees. Below, a
record's x is its `+6`, y0 its `+0xA` (the screen y it is drawn at), depth
its `+8` (the sort key: a larger depth is drawn in front) and the facing
word its `+0xC` (0x20 faces right, 0x2020 left; bit 0x2000 is the mirror).

* **The shot — `BtlShotSet` (`0x80040F14`) and `BtlShotRun` (`0x80040F8C`).**
  `BtlActingShowRun` (`0x8003A158`) calls `BtlShotSet` before every
  handler, so the shot block `g_BtlShotBlock` holds its defaults when a
  handler starts: `b0`..`b7` = 8, 0x63, 9, 0, 8, 0, 0x4B, 0; `h8` = `hA` =
  0; `+0xC` and `+0x14` the acting slot's record, `+0x10` the target's.
  Seven handlers fly it with `BtlShotRun`: `BtlShow9BRun` (`0x8003AD08`),
  `BtlShow9CRun` (`0x8003AFA0`), `BtlShowADRun` (`0x8003DFE4`),
  `BtlShowB0Run` (`0x8003EBD4`), `BtlShowB1Run` (`0x8003EEC4`),
  `BtlShowB5Run` (`0x8003F784`) and `BtlShowB8Run` (`0x8003FE60`, twice).
  The fields: `b0` the actor's launch anim (0xFF = none, which ALSO skips
  sound `b5` and the wait `b1`); `b1` the wait after it (`>= 0x63`: to the
  anim's last step, `BtlAnimationEndWait` (`0x8002FDBC`); else `b1` frames,
  `BtlFrameWait` (`0x80036BA8`); 0 none); `b2` the projectile's anim; `b3`
  the flight length in frames (0 = derive); `b4` pixels per frame for the
  derived length — **when `b3` and `b4` are both 0, `b4` becomes 8 and is
  written back into the block**; `b5` / `b6` / `b7` sounds before the anim,
  at launch, on arrival (0 none); `h8` / `hA` the muzzle offset (x mirrored
  by facing); `+0xC` / `+0x10` / `+0x14` actor, target, the record to copy.
  Set-up: a 64-byte copy of the `+0x14` record, its first word cleared,
  becomes the extra object `g_BtlExtraRecords` and takes anim `b2`; actor
  and target are turned to face each other (`+0xC` = 0x20 / 0x2020, the
  copy takes the actor's); start x = actor x ± (actor `+0x3C` >> 1) ±
  `h8`, y0 = actor y0 + `hA`. Derived length = max(|dx|, |dy|) / `b4`, at
  least 1 — **the sign test for |dy| compares the ACTOR's y0 with the
  target's, not the projectile's start** (the original's asymmetry: with
  a muzzle offset `hA` that crosses the target's y0 the "absolute" dy
  comes out negative). `line_path_make` (`0x80054C9C`) writes n points into
  the path buffer `g_LoadBuffer` (point i = start + delta·i/n for i =
  1..n, the last forced to the target). Then sound `b5`, anim `b0` and the
  wait `b1`; sound `b6`; `g_BtlExtraRecordCount = 1` and the copy shown; one
  point per frame (`BtlFrameStep` (`0x80038D2C`)) with depth = y0 + 1; sound
  `b7`; `sprite_run_off(0x1C, 1)` (`0x80019A7C`) frees sprite slot 0x1C and
  `g_BtlExtraRecordCount = 0`. Frames: n for the flight plus those of the
  wait `b1`.
* **The vanish's way home — `BtlActingPlaceAnimationRun(x, y0, anim)`
  (`0x8003CBE4`)**, called by both arms of `BtlShowA5Run`. It stores `x` at
  the acting record's x and `y0` at both its depth and its y0, then
  `SoundPlay(0x68, 3)` (`0x800555F0`) (sound 0x68),
  `BtlAnimationSet(actor, anim)` (`0x80027208`), `BtlAnimationEndWait(actor)`
  (wait for the anim's end) and `BtlActingRestAnimationSet()` (`0x80037890`).
* **0x97 — `BtlShow97Run` (`0x8003A3BC`)**, entry 0 of
  `g_BtlActingShowFunctions`. `script = g_BtlActuatorC8LateList`, `anim = 8`;
  by the caster's id `g_BtlSlotIds[caster]`: 0x62 -> `D_800AE2FC`, 0x3C ->
  `D_800AE2C0`, 0x6B -> `D_800AE3DC`, 0x72 -> anim 2. Then `BtlWalkSet()`
  (`0x8002BAA4`) fills the walk block, **`g_BtlWalkBlock` `+0xD` (the walk
  anim) = 1 between the two calls** (for a party-slot caster this
  overrides the `g_BtlPartyMoveAnimations[slot]` that `BtlWalkSet` put
  there), `BtlWalkRun()` (`0x8002BB5C`) walks with its result discarded,
  the vibration trio comes AFTER the walk (`g_PadActuatorList = script`,
  `g_PadActuatorRestFlags = 0x3F`, `g_PadActuatorListWait = 0`) and
  `BtlActingAnimationOutcomeRun(anim, 0xFF)` (`0x80041338`) strikes. The two
  stores to the walk speed `+0xE` (CharStats `+0x18`, and 2 for id 0x42)
  come before `BtlWalkSet`, which writes that byte itself from CharStats
  `+0x18`: both are dead, so **enemy 0x42 does not walk at speed 2 here**.
  Neither walk function takes an argument. No loop and no wait of its
  own.
* **0x9B, the plain shot — `BtlShow9BRun` (`0x8003AD08`)**
  (`g_BtlActingShowFunctions[4]`). `BtlShotSet` has already run, so the shot
  block holds its defaults. Locals `before = 0`, `after =
  g_BtlActuatorFFTo45List`; a switch on the acting slot's id
  `g_BtlSlotIds[g_BtlActingSlot]` (ids 2..0x76; an id not listed changes
  nothing) writes, in the C's order:

  | id | writes |
  |---|---|
  | 2, 0x33 | b5 = 0x5E |
  | 0x11, 0x6D | b7 = 0x7C, h8 = -0xA, b5 = 0, hA = -4, b1 = 0 |
  | 0x16, 0x60, 0x64 | before = `g_BtlActuator41List`, after = `g_BtlActuatorC8List`, b5 = 0x64, b1 = 0x1B, b6 = 0, hA = -8 |
  | 0x18, 0x4A | h8 = 0xA, hA = -0x1B, b1 = 0x38 |
  | 0x2D | before = `D_800AE3CC`, b5 = 0x52, b6 = 0x36, h8 = 4, hA = -1, b1 = 0x3A, b4 = 2 |
  | 0x2F | b0 = 2, b2 = 0xA, b1 = 0x35, h8 = 4, hA = -0xC |
  | 0x42 | b5 = 0x4E, b0 = 2, b1 = 0x27 |
  | 0x46 | b5 = 0x3C, b0 = 2, b1 = 0x20, h8 = -8, hA = -0x14 |
  | 0x49 | b5 = 0x4E, b7 = 0x5A, b0 = 2, b1 = 0x10, h8 = -7, b6 = 0, hA = -2 |
  | 0x4D | b0 = 2, h8 = -0x25, hA = -0x2D |
  | 0x76 | b0 = 2, b2 = 0xC, b5 = 0x3C |

  b3 and the three words at `+0xC` / `+0x10` / `+0x14` are never written
  here. Then: the lane walk `BtlWalkEdgeRun(walk)` (`0x80036654`) (`walk =
  g_BtlPartyMoveAnimations[g_BtlActingSlot]` for a party slot, else 1),
  `BtlFacingBothSet()` (`0x8003693C`), the effect trio with `before` (**0
  clears the pointer** for every id but the 0x16 group and 0x2D), the
  shot `BtlShotRun()`, the trio again with `after`, the hit tail
  `BtlTargetOutcomeShow()` (`0x80041484`). No targeting, damage or status
  logic of its own.
* **0xA1, the drain — `BtlShowA1Run` (`0x8003C0A8`).** `BtlWalkSet()` fills
  the walk block `g_BtlWalkBlock`, then the handler sets `+0xA = 1` (the
  walker's width counts, as `BtlWalkSet` left it), `+0xE = 3` (the speed:
  4 frames a cell), `+9 = 0` (the other slots' records block no cell of
  the walk grid), `+0xB = 0` (the target counts as one cell wide) and
  `+0xF = 0x64` (the reach, which `BtlWalkRun` holds at 0x28 as it holds
  any reach above 0x28), and `BtlWalkRun()` walks. **The caster's depth
  goes up by one for the hold**; `BtlFacingBothSet()`; caster anim 8;
  vibration `g_BtlActuatorToggleList`. Then 0x50 frames (`BtlFrameStep`
  each) with a phase counter beside the frame counter, both from 0 and
  both stepped after the wait: phase 1 — the action's sound
  `SoundPlay(g_BtlActionSound, 3)` and the target takes anim 0x12; phase
  10 — the target goes back to anim `g_BtlSlotRestAnimations[target]`;
  phase 0x14 — the phase is reset to 0. **So the sound and anim 0x12 fall
  on frames 1, 21, 41 and 61, the anim back on frames 10, 30, 50 and 70,
  and frames 0, 20, 40 and 60 do nothing.** In this loop the target's
  anim is set **with no `< 5` test — an enemy target is animated too**.
  After it: vibration `g_BtlActuatorC8List`, `BtlActingRestAnimationSet()`,
  the caster's depth back down; a PARTY target takes anim 0x12; the
  knockback `BtlTargetPushBack()` (`0x80037C64`); the impact
  `BtlAmountShowStart(0)` (`0x80088EA8`); `BtlAmountShowWait()`
  (`0x80031028`); then `BtlFallRun()` (`0x800385B4`) if
  `g_BtlSlotFallMarks[target]`, else the target takes anim
  `g_BtlSlotRestAnimations[target]`.
* **0xA5, the vanish — `BtlShowA5Run` (`0x8003C79C`).** Anims out / in are
  9 / 0xA, or 0xA / 0xB for caster id 0x4A. The caster's x and y0 are
  saved; it faces the target (`+0xC` = 0x20 if its x is below the
  target's, else 0x2020), sound 0x68, anim out, wait. It reappears at x =
  target x − 2 when `+0xC` is 0x20, else + 2 — **the side follows the
  facing word** — y0 = target y0 − 0x10 and **depth = the target's + 1**.
  Sound 0x68, anim in, wait, anim 2. Then by
  `g_BtlSlotAmountKinds[target]`: 2 (miss) -> `BtlFrameWait(0x10)`, a party
  target takes anim 0x11, `BtlTargetBackMove` (`0x8002FD1C`), wait, sound
  0x68, anim out, `BtlFrameWait(0x10)`, `BtlTargetForwardMove`
  (`0x8002FE78`), wait, home; otherwise (hit) -> vibration
  `g_BtlActuatorC8List`, wait, a party target takes anim 0x12,
  `BtlAmountShowStart(0)`, `BtlFrameWait(7)`, sound 0x68, anim out, the
  knockback `BtlTargetPushBack`, wait, home, `BtlAmountShowWait`. Home is
  `BtlActingPlaceAnimationRun(saved x, saved y0, in)`. Last: `BtlFallRun()`
  if `g_BtlSlotFallMarks[target]`, else the target takes anim
  `g_BtlSlotRestAnimations[target]`.
* **0xA8, the transformation — `BtlShowA8Run` (`0x8003D354`).**
  `BtlWalkEdgeRun(1)`, `BtlFacingBothSet()`, sound 0x5F; the caster takes
  anim 0xA to its end, then anim 0x1D; 0x1E frames. A 64-byte copy of the
  caster's record becomes the extra object (`g_BtlExtraRecordCount = 1`) on
  the target: x and y0 the target's, depth = that y0 + 1, **bit 0x2000 of
  its `+0xC` word flipped** (the mirror); anim 0x15; 0x1E frames; sound
  0x6D; 7 frames. Then the target's first word = 0 — **it is hidden 7
  frames after sound 0x6D, before the copy's anim ends**; the copy's anim
  is waited out; `g_BtlExtraRecordCount = 0`; `sprite_run_off(0x1C, 1)`.
  `BtlTargetActingRecordCopy()` (`0x8003F9D4`, which `BtlShowB5Run` calls
  too) overwrites the target's whole 0x40 record with the caster's,
  **keeping the target's own x, its y0 (written to both depth and y0, so
  its own depth is not kept), its `+0xC` word and `+0x3C` / `+0x3E`**. The
  target takes anim 0x16, and the row's "rest 0x16, hit 0x17" are TABLE
  WRITES: `g_BtlPartyHitAnimations[target] = 0x17` (the hit anim other
  actions play on it), `g_BtlPartyStandAnimations[target] = 0x16` (its rest
  anim) and `g_BtlSlotRestAnimations[target]` = that rest anim. The caster
  takes anim 0x1E to its end. **Nothing here can undo it**: the saved copy
  of the target's record is a stack local of `BtlTargetActingRecordCopy`
  and the three table bytes are not restored. No vibration script at all.
* **0xAD — `BtlShowADRun` (`0x8003DFE4`)**: the caster is swapped for a
  stand-in block, fires one shot, and the block's record plays on the
  target. `BtlEffectLoad(&a, 0x5B)` (`0x800897DC`) loads effect block 0x5B
  and binds the stack record `a`; `BtlWalkEdgeRun(1)`,
  `BtlFacingBothSet()`; vibration `g_BtlActuatorFFTwiceList`; `a`'s first
  word = 1. The caster's record is saved and replaced by `a` — **it keeps
  its x and its `+0xC` word, and takes the saved y0 as both depth and
  y0** — then anim 7. `a` is then placed on the target (`g_BtlTargetSlot`):
  x and y0 the target's, depth = the target's y0 + 1 (**the "second
  record" is the same stack record, moved after its contents were copied
  into the caster's slot**). Shot block: `b0 = 9`, `b1 = 0x2F`, `b2 =
  0x10`, muzzle 0xE / −0x22 (`b3..b7` not written here); `BtlShotRun()`.
  Vibration `g_BtlActuatorFFList`; `a` takes anim 0 and becomes the extra
  object (`g_BtlExtraRecordCount = 1`); sound 0x5B; `BtlFrameWait(0x1E)`; the
  impact `BtlAmountShowStart(1)`; `a`'s anim waited out;
  `g_BtlExtraRecordCount = 0`. Then `BtlFallRun()`,
  `BtlMarkedRestAnimationSet()` (`0x80030DA0`), `BtlAmountShowWait()`, the
  caster's record restored, `BtlActingRestAnimationSet()`,
  `xmalloc_heap_free(g_BtlEffectBlock[0])` (`0x80022F88`).
* **0xAE, the Tank Golem's charge — `BtlShowAERun` (`0x8003E274`).** Ids
  0x43 / 0x75 take anim 8 / 0xC at a step of 0x10 px a frame; any other
  caster reads both uninitialised. The caster's depth goes up by 7 (it is
  drawn in front; its y0 does not move), it takes its anim with the
  action's sound `g_BtlActionSound`, and it leaves the screen the way it
  faces — to `−(+0x3C >> 1)` or `0x140 + (+0x3C >> 1)`, one frame per step.
  Then, with the global target index `g_BtlTargetSlot` as the loop counter
  over 0..4 (left at 5), it makes one full-screen pass per flagged party
  slot (`g_BtlSlotAmountKinds`): vibration `g_BtlActuatorC8List`, the sound
  again, the caster on that slot's y0 with depth = that y0 + 7, **each
  pass running opposite to the one before**, and the slot taking anim
  0x12 when the pass is over. Then `SoundPlay(0, 3)`, the caster comes
  back to its saved x with depth and y0 both its saved y0, and
  `BtlAmountShowStart(0)`, `BtlAnimationEndWait`, `BtlFacingNearestSet`
  (`0x8002E2FC`), `BtlShowEnd` (`0x8004166C`).
* **0xB0 — `BtlShowB0Run` (`0x8003EBD4`)**: three shots, a copy of the
  caster flashed on the target after each. `BtlWalkEdgeRun(1)`,
  `BtlFacingBothSet()`; caster anim 9 to its end. Shot block: `b1 = 8`,
  `b2 = 0xD`, muzzle −0xC / −7, sounds `b5 = 0x6E`, `b6 = 0`, `b7 = 0x5A`
  (`b0`, `b3`, `b4` not written here). A 64-byte copy of the caster's
  record is placed on the target: x and y0 the target's, **depth = the
  target's + 1**. Three passes: vibration `g_BtlActuatorFFTo45List`, the
  caster's `+0x30` (its anim number) = 0, the shot; the copy becomes the
  extra object (`g_BtlExtraRecordCount = 1`), its `+0x30 = 0`, anim 0xE;
  **`BtlPartyHitAnimationSet()` (`0x80030E78`) on the first pass only**;
  vibration `g_BtlActuatorC8List`, the copy's anim to its end,
  `g_BtlExtraRecordCount = 0`. Then `sprite_run_off(0x1C, 1)`, the impact
  `BtlAmountShowStart(1)`, caster anim 0xA; one frame per pass while
  `BtlAmountShowCheck()` (`0x8008931C`) is non-zero, and **on the frame the
  caster's step `+0x38` equals `(+0x3A & 0x7FFF) − 1`,
  `BtlActingRestAnimationSet()` runs, once**; `BtlFallRun()`,
  `BtlMarkedRestAnimationSet()`; if that step never came the caster's anim
  is waited out.
* **0xB3 — `BtlShowB3Run` (`0x8003F2EC`).** `BtlExtraRecordsAlloc(2)`
  (`0x8003A340`) gets room for two 0x40 records at `g_BtlExtraRecords`,
  cleared, and sets `g_BtlExtraRecordCount = 2`, so both records are drawn
  from then on; `BtlEffectLoad(g_BtlExtraRecords, 0x60)` loads the effect
  block and binds record 0 — block 0x5D when the encounter `g_BtlNumber` is
  0xFE. **The records stand on the CASTER's spot**, not the target's:
  record 0 takes the caster's x and y0, depth = that y0 + 1; record 1 is a
  copy of it, **0x3A px to the side the caster's mirror bit picks** (bit
  0x2000 set: record 0 gets the bit and record 1's x is 0x3A less; clear:
  record 0 loses it and record 1's x is 0x3A more), both ending with the
  same `+0xC` word. Record 0 on, **the caster off**; record 0 plays anim 0
  to its end. Vibration `g_BtlActuatorFFTwiceList`, sound 0x66; record 1 on
  with anim 3, record 0 anim 1; 50 frames (`BtlFrameWait(0x32)`);
  `BtlPartyHitAnimationSet()`; the impact `BtlAmountShowStart(1)`; record
  1's anim to its end. Record 0 anim 2, `BtlAmountShowWait()`, that anim to
  its end; `BtlExtraRecordsFree(2)` (`0x8003A388`: the count back to 0,
  `sprite_run_off(0x1C, 2)`, the room freed); the caster back on;
  `BtlActingRestAnimationSet()`, `BtlFallRun()`,
  `BtlMarkedRestAnimationSet()`; the block is freed.
* **0xB4, the tile strike — `BtlShowB4Run` (`0x8003F530`).** Vibration
  `g_BtlActuatorC8List`, sound 7, caster anim 0xB to its end. **Rise**:
  while the caster's y0 is >= 0, y0 −= 8 and one frame passes — it stops
  on the first negative y0, `y0 / 8 + 1` frames. Its first word = 0
  (hidden) and `BtlFrameWait(0x64)`: 100 frames gone. Sound 8; first word
  = 1; anim 8. **Placement**: x = `g_BtlActionNumber * 8`, depth =
  `g_BtlTargetSlot * 8 + 1`, y0 = −8 — the tile's ROW is the target-index
  byte `g_BtlTargetSlot`, used here as a coordinate. **Fall**: while y0 <
  depth − 1, y0 += 8 and one frame passes — it lands on y0 =
  `g_BtlTargetSlot * 8` exactly, after `g_BtlTargetSlot + 1` frames.
  Vibration `g_BtlActuatorFFList`, sound 0x5A, anim 0xC to its end,
  `BtlFallRun()`. **"It stays there" is literal**: nothing saves or
  restores the caster's x, depth or y0 and no walk home is called, so its
  record keeps the tile's position when the handler returns.
* **0xB6, Slice O' Dead — `BtlShowB6Run` (`0x8003FAD8`).** `g_BtlExtraRecords
  = xmalloc_heap_alloc(0x200)` (`0x80022F4C`), cleared. Blink out: for i =
  11 down to 1, i frames then the caster's first word `^= 1` — 66 frames,
  eleven toggles, hidden. Sound 0x62. Copies: **the global target index
  `g_BtlTargetSlot` is the loop counter (left at 5)**; copy n = the
  caster's whole record **for every slot, live or not**; for a live slot
  (`g_BtlSlotActiveFlags[n]`) the copy's first word `^= 1`, x and y0 are the
  slot's and depth = the slot's + 1. Vibration `g_BtlActuatorC8List`;
  **`g_BtlExtraRecordCount = 8`** (eight extra objects, not five). Each live
  slot's copy takes anim 2 and its party record gets `+0x34 = +0x36`.
  `BtlAnimationEndWait` waits out the LAST live slot's copy (**with no live
  slot: copy 0**), then `BtlPartyHitAnimationSet()`, the impact
  `BtlAmountShowStart(1)`, `g_BtlExtraRecordCount = 0`,
  `sprite_run_off(0x1C, 0x14)`. Blink back: i = 0..10, i frames then `^= 1`
  — 55 frames. `BtlAmountShowWait()`, `xmalloc_heap_free(g_BtlExtraRecords)`,
  `BtlMarkedRestAnimationSet()`.
* **0xBB — `BtlShowBBRun` (`0x800408F8`)**: the caster dashes onto each
  flagged party slot in turn. Vibration `g_BtlActuatorC8List`; the caster's
  x / y0 are kept aside. For `g_BtlTargetSlot` = 0..4 (**again the global,
  left at 5**) with `g_BtlSlotAmountKinds[n]` set: the caster faces the slot
  (`+0xC` = 0x20 if its x is below the slot's, else 0x2020);
  `line_path_make` fills the path buffer `g_LoadBuffer` with 0x10 points;
  `BtlActingPathMove(path)` (`0x80040BA0`) — caster anim 1, anim 8, sound
  `g_BtlActionSound`, one point per frame for 16 frames (x, y0, and depth =
  y0 + 1); **the vibration script is started again**; the caster's anim is
  waited out; the slot takes anim `g_BtlPartyHitAnimations[n]`. **The
  caster is not sent home between targets** — each dash starts where the
  last ended. Then the impact `BtlAmountShowStart(0)`, a 0x10-point path
  back to the kept position through the same sub (so anim 1, anim 8 and
  the sound replay), caster anim 9 waited out,
  `BtlActingRestAnimationSet()`, the caster's depth = its y0,
  `BtlAmountShowWait()`, `BtlFallRun()`, `BtlMarkedRestAnimationSet()`.
* **0xBC — `BtlShowBCRun` (`0x80040C70`)**: four copies of the caster, one
  onto each flagged party slot 0..3. `g_BtlExtraRecords =
  xmalloc_heap_alloc(0x100)`; all four records are the caster's as it
  stands at entry. Vibration `g_BtlActuatorC8List`, sound 0x6E,
  `BtlFacingPartySet()` (`0x800416A4`) (the turn-round in the table of
  shared pieces, §"The enemy actions"); caster anim 0xA to its end. For
  slot i = 0..3 with `g_BtlSlotAmountKinds[i]` set: copy i's first word = 1,
  x and y0 the slot's, **depth = that y0 + 1** (not the slot's depth + 1
  as in 0xB6), anim 0xF. **Slot 4 is never looked at.** Vibration
  `g_BtlActuatorFFList`, sound 0x5A, `g_BtlExtraRecordCount = 4` (all four are
  drawn; an unflagged slot's copy is still the caster's record),
  `BtlFrameWait(0x10)`, the impact `BtlAmountShowStart(1)`;
  `BtlAnimationEndWait` waits out the last flagged slot's copy;
  `g_BtlExtraRecordCount = 0`, `sprite_run_off(0x1C, 4)`, `BtlShowEnd()`,
  `xmalloc_heap_free(g_BtlExtraRecords)`. **Uninitialised path
  (original):** the remembered slot is written only for a flagged slot;
  with none of 0..3 flagged the final wait is on record `(caller's $s2 &
  0xFF)` of the block, outside it for any value above 3.

### Effect 7, the heal-all — `BtlPartyHpShareAddRun` (`0x800320C8`)

`scene_state_table[6]`: entry 6 of the run of 132 function addresses at
`0x800B2EF8` that `BtlActionRowRun` (`0x80031ABC`) calls through (the C
uses the first split's label for it; the name was never read). Carried by
spell rows 14 and 38 and by item 244.
`BtlTargetPartyMark` (`0x80036294`) flags the set; then, per slot i = 0..13
in slot order whose `g_BtlSlotAmountKinds[i]` is non-zero when the loop
reaches it:

1. `base` = the max HP (`+2`) of the record `g_BtlTargetStats` points at **on
   entry to the iteration**;
2. `amount = base * 4 / 10`;
3. `roll = rng_mask_below(0xFFFF, (u16)(base / 20))` (`0x8003A0D4`) — the only RNG use:
   **one draw per flagged slot, and none when `base / 20 < 2`** (base <
   40);
4. `amount = (amount < 40 ? 40 : amount) + roll`, in 16 bits (the C keeps
   the amount in `g_BtlActionPower` and the roll in `g_BtlAreaHalfWidth`,
   which are left holding the last slot's);
5. **only now** is `g_BtlTargetStats` moved to slot i — in battle
   `BtlTargetShowPlaceSet()` (`0x80036448`) (the slot's x / y, which only
   the heal SPARKLE reads: `BtlShowPlacesPlay` (`0x8007F2D8`) places it
   there, not the popup; battle-presentations-2.md, part D) and
   `CharStats + i * 0x3C`; outside battle (`g_BtlRunning == 0`, the menu's
   use of the same handler) `g_ChrRecords + (g_BtlSlotIds[i] << 6)`;
6. `hp += amount`; if `hp >= maxhp` the overshoot comes off the amount and
   `hp = maxhp`;
7. `g_BtlSlotAmountKinds[i] = 5`, `g_BtlSlotAmounts[i] = amount`.

After the loop, one `BtlShowNumberRun(g_BtlActionShowNumber)` (`0x8007BB80`).
There is no KO or status test here.

**So the amount is not computed from each target's own max HP.** Steps 1
to 4 read the record `g_BtlTargetStats` still points at: the action's
target (set by `BtlActionRowRun` before the call) for the FIRST flagged
slot, then the PREVIOUSLY HEALED slot for each later one. Only the clamp
in step 6 uses the slot's own record. Both the amount and the draw (its
modulus, and whether it happens at all) follow from that.
("`g_BtlTargetStats` points at the heal target's record" holds for effect 3,
the single heal, only.)

The C reads `g_ChrRecords` here through the alias `g_ChrRecords_s`: this
function reaches the word `%gp_rel`, while `BtlWornItemRemove`
(`0x80038BFC`) in the same file reaches it `%hi/%lo` under its own name.
One address, two names, one per addressing.

### The creep — `BtlActingTargetLeftMove(gap)` (`0x8003A4E4`)

The slow approach of enemy actions 0x98 (`BtlShow98Run`, gap 0xA0) and
0xAA (`BtlShowAARun`, gap 0x68). Goal: x = target.x − `gap`, y = target.y
(`+0xA`) — **the gap is always subtracted, whichever side the caster is
on, and neither its width nor its facing enters.** Points: count =
max(|goal.x − caster.x|, |target.y − caster.y|) / 2; **a count of 0 does
nothing at all** (no anim, no frame). `line_path_make` (`0x80054C9C`) builds the path (point
k = start + delta · k / count, the last forced to the goal) into an
80-point buffer ON THE STACK — nothing bounds the count, so a distance of
162 px or more would write past it. The caster takes anim 1; then one
`BtlFrameStep` (`0x80038D2C`) per pass. The first five frames move nothing; from then on the
caster takes the next point after every five frames (x, y, depth `+8` =
y + 1), point k after frame 5 · k. The pass that takes the last point
still runs its frame, so the creep lasts 5 × count + 1 frames. The caster
is left at the goal in anim 1, its facing word untouched. No RNG draw of
its own.

### Enemy actions 0xA2, 0xA7 and 0xAB

Three small handlers in `btl_enemy.c`. None draws from the RNG itself.

* **0xA2 — `BtlShowA2Run`.** It does NOT refill the walk block
  `g_BtlWalkBlock`: it sets three bytes on it as the last fill left it —
  `+0xA = 0` (the walker's width does not count), `+0xB = 0` (nor the
  target's), `+0xC = 1` (walk, not only measure) — and `BtlWalkRun()` (`0x8002BB5C`)
  walks. Range, speed and walk animation are whatever the block already
  held. Then record 14 of `g_BtlRecords` (base + 0x380) becomes a whole
  copy of the caster's record — first word 0, depth (`+8`) + 1, anim 9 —
  is switched on, `BtlActingAnimationOutcomeRun(8, 0xFF)` (`0x80041338`) runs (the caster on anim 8
  with the action's sound `g_BtlActionSound` until its last step, then the
  target's outcome), and record 14 is switched off again. The handler
  starts no sound of its own and no vibration script, and
  `g_BtlExtraRecordCount` / `g_BtlExtraRecords` are not touched.
* **0xA7 — `BtlShowA7Run`**: step in if the target is not already
  adjacent, then the swing. By the caster's id: 0x12 sets the walk
  animation (block `+0xD`) to 1 and the sound to 0x6C, any other id 0xB
  and 0x6D. **The measure**: `+0xC = 0` (do not move) and `BtlWalkRun()` (`0x8002BB5C`)
  — it returns 2 only when the block's `+0xB` (the target's width counts)
  is set and the two already stand side by side, and only then does it
  turn the caster toward the target. Any other result — **0 out of
  reach, or 1 within reach but not adjacent** — takes the walk arm: the
  sound, caster anim 9 to its end, `+0xC = 1` and `BtlWalkRun()` (`0x8002BB5C`) again
  (the walk, on the animation chosen above), **the sound again**, anim
  0xA to its end. Then `BtlActingAnimationOutcomeRun(2, 0xFF)` (`0x80041338`). The block is
  not refilled here either (only `+0xC` and `+0xD` are written); which
  function fills it before this handler runs is not read. No vibration
  script.
* **0xAB — `BtlShowABRun` (`0x8003DAEC`).** The caster faces the target (`+0xC` =
  0x20 if its x is below the target's, else 0x2020); sound 0x6E; caster
  anim 8 to its end. Record 14 = a whole copy of the caster's record
  placed ON the target: x and y (`+0xA`) the target's, **depth = that
  y + 6** (not + 1 as in the other copy actions); anim 9; on. Then 20
  frames (`BtlFrameWait(0x14)` (`0x80036BA8`)), vibration `g_BtlActuatorC8List`, sound
  0x5A, 10 frames, `BtlPartyHitAnimationSet()` (`0x80030E78`); record 14's anim is waited
  out and it is switched off; the impact `BtlAmountShowStart(1)` (`0x80088EA8`);
  `BtlShowEnd()` (`0x8004166C`). **No walk — the caster stays put.** `g_BtlExtraRecordCount` /
  `g_BtlExtraRecords` are not touched.

### The breath attacks' party sweep — `BtlPartyRightMove` (`0x80086700`)

The opening shared by the three breath presentations: 0x50 Fire Breath
(attack row 92, handler `BtlShow50Run`, which calls it
directly), 0x51 Thunder Breath (attack rows 93 and 126, `BtlShow51Run`) and
0x52 Cold Breath (attack row 94, `BtlShow52Run`) — the last two through
`BtlPartyRightMovePatternsRun` (`0x80086B64`) (next section). In order:

1. Every live party slot (`g_BtlSlotActiveFlags[i]`) whose status byte
   (`CharStats` record `+0x15`) has none of 0x3C set turns toward x = 0,
   the left edge the breath comes from (`BtlFacingTowardSet(rec, 0)` (`0x80089FC4`) sets
   bit 0x2000 of `+0xC` for any x of 0 or more), and takes anim 0x13.
2. The vibration script `g_BtlActuatorRandomList` starts (`g_PadActuatorRestFlags = 0x3F`,
   `g_PadActuatorListWait = 0`); sounds 0x77 and 0x60 go on scheduler slots 10 and 11
   (`BtlShowSoundSet(0xA, 0x77, 0x18, 0x5C)` (`0x80089EF8`), `(0xB, 0x60, 0x30, 0x20)`).
3. The actor `g_BtlActingSlot` plays anim 0xE to its end (`BtlShowAnimationPlay` (`0x8008A344`)),
   `BtlTileScrollSet(8, 0, 0)` (`0x8008A468`) zeroes the scroll, backdrop `*g_BtlTilePatternList` is
   shown for one frame (`BtlTilePatternPut(b, 1)` (`0x80089C8C`)), and the tile layer's
   fade over 0x20 frames starts (`layer_mix_fade_start(8, 1, 0x20, 8)` (`0x8001EBA4`)).
4. Three phases, ONE frame per pass (`BtlTileScrollStep(dx, 0)` (`0x8008A4C0`) scrolls and
   runs the frame wait), each pass rotating the 16 colours at
   `g_PaletteBuf + 0x220` to start at `t & 0xF`:
   * while the fade runs (`layer_mix_fade_check(8)` (`0x8001ED94`)): every slot's x ±1 by `t`'s
     parity, `t -= 0x11`, scroll −0x10;
   * 72 passes: each slot's x moves right (+4 below 0x7E, +2 below 0x9E,
     +1 below 0x11E) then ±2 by parity, `t -= 0x15`, scroll −0x14;
   * `layer_mix_fade_start(8, 1, 0x28, 0x1F)` (`0x8001EBA4`), the fade back over 0x28 frames,
     then as the first phase while it runs.
5. Teardown: `g_PadActuatorList = 0`, `g_PadActuatorRestFlags = 0`, `g_PadActuatorListWait = 0`, both
   motor levels 0, sound slots 10 and 11 cleared (`g_BtlShowSounds[11].state`,
   `[10].state`), and `BtlSlotActionAnimationSet(i)` (`0x80037E64`) (the animation of its
   planned action) for the slots of step 1.

The shake and the slide apply to **all five slots**, live or not, and
nothing puts their x back.

**`t` is never given a value.** The C declares it and reads it first in
`t & 0xF`; the compiled function keeps it in `$s2` and never writes that
register before the first read, so `t` starts at whatever the caller left
in `$s2`. Both steps are odd, so the parity flips every frame whatever
the start. The start decides the palette phase, the first shake
direction and where the party rests after the sweep: phase 2 tests its
thresholds on the x the shake has just moved, so an even and an odd start
can leave a record 1 to 3 px apart. The start is known for all three
breaths, from the program's instructions:

* through `BtlPartyRightMovePatternsRun` (`0x80086B64`), the long form copies its fourth argument
  into `$s2` in the delay slot of its call to the sweep, so `t` starts at
  that argument — **0x74 for Thunder Breath** (`BtlShow51Run` passes `(0,
  2, 0x73, 0x74, 0, 0)`) and **0 for Cold Breath** (`BtlShow52Run`, `(1,
  6, 0, 0, 0x79, 0x51)`);
* for Fire Breath, `BtlShow50Run` calls the sweep before writing `$s2`.
  The turn loop `BtlTurnOrderRun` (`0x8002DC30`) holds the address of `g_BtlTurnOrder`
  (0x800E49C0) in `$s2` across its call of `BtlActionRun` (`0x8002E088`), and none of
  `BtlActionRun` (`0x8002E088`), `BtlActionKindRun` (`0x8002E12C`), `BtlActionRowRun` (`0x80031ABC`), `BtlTargetStat0EHitRun` (`0x80032BDC`),
  `BtlShowNumberRun` (`0x8007BB80`) or `BtlShow50Run` writes the register, so `t` starts at
  0x800E49C0.

All three starts are even; the palette phase starts at 0, 4 and 0.
battle-presentations-2.md §"Group F — the boss breaths 0x50..0x53 and
effect 93" works out the resting places.

**RNG.** The function draws nothing itself, but it starts `g_BtlActuatorRandomList`,
the one vibration script with the random motion: the driver
`pad_update` (`0x8001F508`) draws on driver calls 84, 88 .. 120 after the assignment (ten
draws, as `pad_update` (`0x8001F508`)'s C steps the list). The sweep outlasts that by its
own length — the caster's anim, then 0x20 + 72 + 0x28 passes by the
fades' lengths — so all ten are made here, vibration on or off.

### The breath sweep's long form — `BtlPartyRightMovePatternsRun` (`0x80086B64`)

`void BtlPartyRightMovePatternsRun(u8 scroll_switch, u16 pattern_count,
u8 first_sound, u8 second_sound, u8 third_sound, u8 fourth_sound)` (`0x80086B64`), the
rest of Thunder Breath and Cold Breath: `BtlShow51Run` calls it as `(0,
2, 0x73, 0x74, 0, 0)` and `BtlShow52Run` as `(1, 6, 0, 0, 0x79, 0x51)`,
each after setting the pulse's colours with `BtlShowPulseSet` (`0x8008A7F8`). In order:

1. The shared sweep `BtlPartyRightMove()` (`0x80086700`) (above), with its `t` starting at
   `second_sound`.
2. The backdrop cursor `g_BtlTilePatternList` steps one entry; the caster
   (`g_BtlActingSlot`) plays anims 8 and 9, each to its end
   (`BtlShowAnimationPlay(rec, anim, 0)` (`0x8008A344`)).
3. The vibration script `g_BtlActuatorRandomList` is started AGAIN (`g_PadActuatorRestFlags =
   0x3F`, `g_PadActuatorListWait = 0` — the sweep's own run was torn down in its
   step 5), and scheduler slots 1..4 take the four sounds:
   `BtlShowSoundSet(slot, sound, 1, repeat)` (`0x80089EF8`) with repeats 0x1C, 0, 0, 0x24.
4. `BtlTileScrollSet(8, 0x7C, 0x9C)` (`0x8008A468`) when `scroll_switch` is set, else `(8,
   0x70, 0x90)` — two calls in the source, one per arm.
5. Backdrop `*g_BtlTilePatternList` for one frame (`BtlTilePatternPut(b, 1)` (`0x80089C8C`)), then
   `layer_mix_set(8, 8)` (`0x8001ECA0`), `layer_col_set(0, 0xC, 0)` (`0x8001EB74`), `layer_col_fade_start(0,
   0xC, 1, 0x10, g_BtlShowPulse.colour_1)` (`0x8001EA74`) and `g_BtlShowPulse.on` = 1
   (`BtlShow50Run` makes the same pair of calls).
6. `pattern_count` more backdrops, 4 frames each, the cursor stepping past
   every one (`BtlTilePatternPut(*g_BtlTilePatternList++, 4)` (`0x80089C8C`)): 2 for Thunder
   Breath, 6 for Cold Breath.
7. 0x20 passes of `t = BtlTilePatternStep(t, 4)` (`0x8008B21C`), 4 frames each, `t` starting
   at 0 here (a local of this function, not the sweep's); then
   `BtlShowPulseClear()` (`0x8008A834`) and the impact `BtlAmountShowStart(1)` (`0x80088EA8`); then more
   passes while `layer_mix_fade_check(8)` (`0x8001ED94`) or `layer_col_fade_check(0)` (`0x8001EDDC`) returns a
   non-zero byte.
8. The close: `g_BtlShowSounds[4].state`, `[3]`, `[2]`, `[1]` cleared (sound
   slots 4..1), `layer_mix_fade_start(8, 1, 0x20, 0x1F)` (`0x8001EBA4`), `layer_col_fade_start(0, 0xC,
   1, 0x10, 0)` (`0x8001EA74`), the vibration script stopped (`g_PadActuatorList = 0`,
   `g_PadActuatorRestFlags = 0`, `g_PadActuatorListWait = 0`) and `g_PadActuatorLevels` / `g_PadActuator1Level`
   zeroed; more passes while `BtlAmountShowCheck()` (`0x8008931C`), `layer_mix_fade_check(8)` (`0x8001ED94`) or
   `layer_col_fade_check(0)` (`0x8001EDDC`) is non-zero; `tile_grid_clear_copy(0, 1)` (`0x80089D58`),
   `BtlTileEnd()` (`0x8008A428`).

No RNG draw of its own. The restart in step 3 makes the script's ten
draws a second time: from the restart to the stop in step 8 there are at
least 1 + 4 × `pattern_count` + 0x20 × 4 frames (137 for Thunder Breath,
153 for Cold Breath), more than the 120 driver calls the script's last
draw falls on. So Thunder Breath and Cold Breath make 20 draws each and
Fire Breath, which does not come through this function, 10 — counted from
the C's waits and measured.

### The status tick — `BtlRoundStatusUpdate` (`0x800297A4`)

`BtlRun` (`0x80027434`) calls it at the start of every round, after
`BtlSlot5ReviveTry` (`0x80027B6C`) and before `BtlEndCheck`. In order
(`btl_round.c`):

1. `BtlActionOutcomeClear` zeroes the round's per-slot bytes and words.
2. Every active slot 0..13 holding a status: for each of bits `0x04`
   (counter `g_BtlSlotStatus4Rounds`; skipped when `g_BtlNumber == 0xF9`), `0x08`
   (`g_BtlSlotStatus8Rounds`), `0x40` (`g_BtlSlotStatus40Rounds`), `0x80` (`g_BtlSlotStatus80Rounds`): bump the
   u8 counter and clear the bit when **`BtlStatusWearOffCheck(counter)` (`0x80029E30`)** says so
   (bit 8 also clears a party slot's `+0x1E` bit `0x04`); the counter restarts.
   Then `BtlSlotStatusAnimationSet(slot)` poses the slot by its status.
3. With the global `g_BtlTargetSlot` as the loop index, every slot's
   `g_BtlSlotAmountKinds`, `g_BtlSlotPlannedKinds`, `g_BtlSlotPlannedNumbers`,
   `g_BtlSlotPlannedTargets`, `g_BtlSlotFallMarks` and its u16 `g_BtlSlotAmounts`
   are cleared, and **regeneration**: an active slot gains `max HP * pct / 100 +
   rng_mask_below(3, 4)` (`0x8003A0D4`, a draw of 0..3), where pct is 10 when
   `BtlTargetTraitCheck(0, 0x1D, 0, 0)` (`0x80037648`) answers 1 (trait `0x1D`:
   one of a party slot's six trait bytes at `+0x28`, or an enemy's trait byte
   `+0x1B`), plus 5 for a party member with `+0x1E` bit `0x01`. HP stops at
   the maximum, and `g_BtlSlotAmounts[s]` gets the amount actually gained
   (less any overflow past max HP), with popup kind `g_BtlSlotAmountKinds[s] = 5`.
   **Effect 8, `BtlPartyFlag1SetRun` (`0x800322B8`, spell row 15), sets that
   bit on every active party slot: a 5 % regeneration** (§"More effect
   handlers"). If any slot gained, `BtlAmountRecordsRun` (`0x8004AE0C`) shows
   the popups.
4. Active party slots: status `0x10` becomes `0x20` (pose 0x18); `0x20`
   becomes `0x30`, pose 0x19, and the slot's active byte is set to 0 (off
   the field). The C stores the whole byte. Stone sets in over two rounds.
5. **Poison**, party slots 0..4 (no active check): each slot's popup kind and
   amount are first cleared; bit `0x01` costs
   `max HP * g_BtlPartyStatus1Shares[s] / 100` with popup kind 1. The
   percentage is 10 when `BtlTargetStatus1Set` (`0x8002F848`) inflicts it,
   then grows by 2 a round, held at 20 (0x14). A cost `>=` current HP sets
   `g_BtlSlotFallMarks[s] = 1` and leaves the HP alone; a smaller cost is
   taken off. `BtlAmountRecordsRun` shows the popups if anyone was poisoned;
   if any slot was marked, `g_BtlFallDone = 0` and `BtlFallRun` (`0x800385B4`)
   and `BtlItem83Revive` (`0x80038868`) run (the first sets a marked party
   slot's HP, status and active byte to 0).
6. The six tables of step 3 are cleared again for all 14 slots, and
   `g_BtlTargetSlot = 0`.

**`BtlStatusWearOffCheck(n)` (`0x80029E30`)**: `rng_7f_draw() < ((n - 1) << 5) & 0xFF`
(`rng_7f_draw` (`0x8003A0B4`) is `rng_draw() & 0x7F`) — 0, 32, 64, 96 out of
128 on ticks 1-4, certain on 5. The draw is made either way. So a status
is never cleared on its first tick and always by its fifth. The appliers
reset the matching counter when they inflict (`BtlTargetStatus4Set`
(`0x8002F74C`) → `g_BtlSlotStatus4Rounds`, `BtlTargetStatus8Set` (`0x8002F958`)
→ `g_BtlSlotStatus8Rounds`, and so on; `BtlTargetStatus10Set` (`0x8002FA00`)
resets all four). `BtlTargetStatus8Set` (`0x8002F958`) also sets
`g_BtlSlotPlannedKinds[t] = 0xB`, so a target put to sleep before its turn
loses this round's action even if it is woken again.

### Turn order — `BtlTurnOrderMake` (`0x8002F4B0`)

`btl_action.c`; its one caller is `BtlTurnOrderRun` (`0x8002DC30`), once per
round. In outline:

```c
for (s = (g_BtlPartyMode == 2 ? 6 : 0); s < 14; s++)        /* top stat_0c   */
    if (g_BtlSlotActiveFlags[s] && !(CharStats[s].u16[+0x14] & 0xC00))   /* +0x15 & 0x0C */
        { key[s] = CharStats[s].u16[+0x0C]; top = max(top, key[s]); }
for (s = 0; s < 14; s++) if (g_BtlSlotActiveFlags[s]) {            /* EVERY active  */
    key[s] += rng_mask_below(0x3FF, top / 10);          /* u16 wrap      */
    order[s] = s + 1;
    if (g_BtlSlotIds[s] == 0x1F) { key[s] = 0xFFFF; g_BtlSlotPlannedKinds[s] = 6; }
}
for (i = 0; i < 13; i++) for (j = i + 1; j < 14; j++)   /* exchange sort */
    if (key[j] > key[i]) { swap(order[i], order[j]); swap(key[i], key[j]); }
```

* The key is **`stat_0c` (`+0x0C`)**, the stat named by its place
  (`include/btl.h`); `BtlEnemyStatsSet` takes an enemy's from its enemy
  row's word `+8` (the Slime's is 16).
* A slot holding status 4 or 8 contributes no `stat_0c` and sorts on its roll
  alone; on a main-menu-Run round (`g_BtlPartyMode` 2) the party does too —
  but **every active slot still draws its roll**.
* The 14 u16 keys are a stack array cleared with `memset` (`0x8008B758`), and
  the 14 order bytes `g_BtlTurnOrder` are zeroed first. The sort runs over all
  14 entries, inactive ones included (key 0, order 0). It exchanges only on a
  strictly greater key (unsigned 16-bit), but it is not stable: an exchange
  with a third entry can put two equal keys out of slot order.
* `BtlTurnOrderRun` (`0x8002DC30`) walks `g_BtlTurnOrder` until its first 0 — so a slot whose
  key ends at 0 can sort behind an inactive slot's 0 and never be reached.
* Slot 5 never appears: battle entry (`BtlInit` (`0x80029E68`)) sets `g_BtlSlotActiveFlags`
  from the ids and then sets `g_BtlSlotActiveFlags[5]` to 0.
* Enemy id `0x1F` always goes first, and its action is Run.
* The C ends with a loop of 14 passes whose body is empty.
* **RNG draws, precisely:** one `rng_mask_below(0x3FF, top / 10)` (`0x8003A0D4`) per
  ACTIVE slot, in ascending slot order, all before the sort — each at
  most one draw of `rng_draw` (`0x8001C770`). **When `top / 10 < 2` (top `stat_0c`
  under 20, or no slot qualified for the top) the helper returns 0 and
  draws NOTHING**, so such a round consumes no RNG values here. Under
  main-menu Run the top is taken over slots 6..13 only, but the party
  slots still roll.

### The action loop — `BtlTurnOrderRun` (`0x8002DC30`), `BtlActionAdjust` (`0x8002E914`), `BtlActionTargetAdjust` (`0x8002ED34`)

`BtlRoundTurnsRun` (`0x8002DB8C`) clears `g_BtlSlotFallMarks` and
`g_BtlSlotAmounts` for all 14 slots and calls `BtlTurnOrderRun` (`0x8002DC30`)
in every party mode. `BtlTurnOrderRun` makes the order (`BtlTurnOrderMake`)
and walks at most 13 entries of `g_BtlTurnOrder`, stopping at the first 0.
For each actor it first clears the effect-parameter globals
`g_BtlActionBitNumber`, `g_BtlAreaHalfHeight`, `g_BtlAreaHalfWidth`,
`g_BtlActionPower`, `D_800B8C33`, `D_800B8C32`, `g_BtlActionFunctionNumber` and
`g_BtlActionShowNumber`, so every action starts with them at 0. Then it loads
`g_BtlActionKind`, `g_BtlActionNumber` and `g_BtlTargetSlot` (kind, number,
target) from `g_BtlSlotPlannedKinds`, `g_BtlSlotPlannedNumbers` and
`g_BtlSlotPlannedTargets`, sets `g_BtlActingStatus` to the actor's status byte
`+0x15`, and calls `BtlActionRun` (`0x8002E088`)
(or `BtlActionSeriesRun` (`0x8002DD88`) for enemy id `0x7C` — a two- or three-action script
per planned number, boss-ai.md); a nonzero result (the fight is
over) ends the loop.

`BtlActionRun` (`0x8002E088`) first runs `BtlActionOutcomeClear` and sets
`g_BtlFallDone = 0`; a party actor whose target is `0xFE` makes its choice
again (`BtlPartyTurnActionSet` (`0x8004F734`)). Then it runs `BtlActionAdjust`
(`0x8002E914`), then `BtlActionKindRun` (`0x8002E12C`); if either returns 0 it
redraws the party panel (`BtlPartyPanelDisp`), zeroes the actor's planned kind
and `BtlActingRestAnimationSet` (`0x80037890`) re-poses it.

**`BtlActionAdjust` (`0x8002E914`)**, its tests in order — may the actor act?
No if inactive, if its kind is
`0xB`, if a party slot's `g_BtlPartyNotHeldFlags` is 0, or if its status has `0x0C`
(4 or 8: **the turn is skipped**). Status `0x40` goes to
`BtlActionStatus40Adjust` (`0x8002E9CC`) (confusion: a party member does nothing on 8/128 — kind
10, pose 0x1D — and an attack targets the nearest slot on EITHER side);
otherwise `BtlActionTargetAdjust` (`0x8002ED34`).

**`BtlActionTargetAdjust` (`0x8002ED34`)** — re-validate the plan. An enemy
slot in a battle numbered `0xEB` or more first goes to
`BtlActionSpecialAdjust` (`0x8002F29C`): ids 0x6F, 0x71, 0x73-0x75 and
0x77-0x7B are handed to their own `BtlEnemy..ActionSet(0)` routines, id 0x7C's
plan is kept as it is, and for all of these the adjust ends there. Then, by
kind:

* kind 2, party: a target of `0xFF` / 100 becomes the nearest enemy
  (`BtlSlotNearestFind(1)` (`0x80039838`)); any other is kept, even a fallen one —
  `BtlAttackTargetFind` (`0x800317E0`) re-picks before each hit.
* kind 2, enemy whose `attack_number` (`+0x1C`) is 0: a target `>= 6` is
  always replaced by the nearest party member (`BtlSlotNearestFind(2)`
  (`0x80039838`)); a party-side target is replaced the same way when the
  enemy's `action_pick` (`+0x1A`) is 2, 7, 12, 17 or 26, and kept otherwise.
  A nonzero `+0x1C` becomes `g_BtlActionNumber` and the plan goes on through the
  kind-7 row check below (the kind stays 2; `BtlActionKindRun` sends such an
  attack to `BtlActionRowRun`). Slot 5 takes this enemy arm for kind 2
  (the party arm is slots below 5), but the party arm for kind 5 (below 6).
* kind 7: an actor holding any status falls back to `BtlActionFallbackSet`
  (`0x8002F3A0`); enemy id 0x4F keeps its plan. Otherwise the row's first byte
  `g_BtlAttackRows[number * 8]` is the target mode: 7 wants a status-free
  target (when no active party slot 0..4 is status-free,
  `BtlEnemyActionFallbackSet` (`0x80036A18`); otherwise any active,
  status-free target below 14 is kept, enemies included, and any other is
  re-picked by `BtlTargetByModeGet(7)` (`0x800397B4`)); 2 wants an active
  target below 14 (else `BtlTargetByModeGet(2)`); any other mode keeps the
  target. The tests make no RNG draw of their own.
* `BtlActionFallbackSet` (`0x8002F3A0`): an enemy attacks the nearest party
  member; a party member attacks the nearest enemy — unless it is under 1/8
  of its max HP and at least two active party members (itself counted) are
  free of the statuses in `0x3C`, when it walks to a corner,
  `BtlActionWalkCornerSet(2)` (`0x80039B70`).
* kind 3: not enough MP (doubled + 1 under trait `0x1B`, asked of the
  caster), status `0x80`, or target 100 → `BtlActionFallbackSet` (`0x8002F3A0`);
  a target of `0xFF` is chosen by `BtlTargetByModeGet(spell +0x01)`; status
  `0x20` with spell `+0x0B` set → the fallback; else
  `BtlTargetModeAdjust(spell +0x01)` (`0x8002EB78`) — so **`+0x01` of a spell row
  is its target mode**, the same vocabulary as item `+0x06`.
* kind 4: target 100 → the fallback; an item id `>= 0x9C` must be among the
  six bytes at `+0x2E` of the actor's stats, else the fallback; then a target
  of `0xFF` is chosen by `BtlTargetByModeGet(item +0x06)`, any other checked by
  `BtlTargetModeAdjust(item +0x06)`.
* kind 5: an enemy → `BtlActionWalkCornerSet(1)` (`0x80039B70`); a party member
  (slots below 6) with target `0xFF` → `BtlActionWalkCornerSet(2)` (`0x80039B70`).

`BtlTargetModeAdjust(mode)` (`0x8002EB78`) re-picks with `BtlTargetByModeGet`
by the mode's low four bits: mode 3 when the target is active; mode 4 unless
the target has an id and a status; any other mode when the target is
inactive (mode 0 then picks the actor itself), or else for mode 5 when its
HP is full, mode 6 when its MP is full, mode 7 when it holds a status. An
active target under any other mode is kept.

**`BtlActionKindRun` (`0x8002E12C`)** dispatches kinds 2..10: 2 attack
(`BtlActionAttackRun` (`0x80031060`), or `BtlActionRowRun` (`0x80031ABC`) for an
enemy slot whose `+0x1C` is set), 3 magic (`BtlActionStringDisp`
(`0x8004987C`), the action's name, then `BtlActionSpellRun` (`0x8002E718`)),
4 item (the name, then `BtlActionItemRun` (`0x8002E7FC`)), **5 = walk, then the 0x13 stance**
(`BtlActionWalkRun` (`0x8002E5D4`) walks to the tile (number, target), in
units of 8, on the walker's walk anim — a party slot's
`g_BtlPartyMoveAnimations` pick, anim 1 for an enemy, except enemy id `0x12`,
which plays anim 7 before the walk and anim 8 after, each waited out by
`BtlAnimationEndWait` (`0x8002FDBC`); `BtlFacingNearestSet` (`0x8002E2FC`) faces
the nearest foe; kind 5 returns 0, so `BtlActionRun`'s
`BtlActingRestAnimationSet` (`0x80037890`) sees kind 5 and rests a
status-free party member on pose **0x13**, which it stores in
`g_BtlSlotRestAnimations`), **6 = RUN**, 7 enemy special (`BtlActionRowRun`
(`0x80031ABC`)), 8-9 nothing, 10 pose 0x1D. Every kind that does not return
early ends with `BtlActionStringClose`, `BtlItem83Revive` and the answer of
`BtlEndCheck`.

A party target resting on pose 0x13 takes less: `BtlHitAmountSet`
(`0x80030070`) and `BtlTargetHpAmountTake` (`0x80035FD8`) halve the amount, and
`BtlAreaStat0EHitRun` (`0x80032C64`) raises the target's defence word by 13/10
(`ratio_13_10_mul` (`0x8002FE54`)). `BtlPartyStandAnimationSet` (`0x80027CE4`)
returns every active, status-free party member to its stand pose after
the loop — **the defend stance lasts from the defender's own turn to the
end of that round**, so it only covers enemies that act after it.

### The generic enemy AI — `BtlEnemyActionSet` (`0x8004C958`)

Runs before the party's menu when `g_BtlNumber < 0xEB` (`btl_message.c`):

```c
enemy_total = sum of +0x08 over slots 6..13 with an id and HP   /* u16 */
party_total = the same over slots 0..4                           /* u16 */
for (s = 6; s < 14; s++)
    if (id && hp && status != 4 && status != 8 && status != 0x30) {
        BtlEnemyActionPick();           /* -> g_BtlEnemyChoice {kind, number, target} */
        g_BtlSlotPlannedKinds[s] = kind; g_BtlSlotPlannedNumbers[s] = number; g_BtlSlotPlannedTargets[s] = target;
    }
```

The sums are `BtlEnemyStat08Sum` (`0x8004F664`) and `BtlPartyStat08Sum`
(`0x8004F5E4`), over `stat_08` (`+0x08`), the stat a hit's amount starts from;
they also count the slots into `g_BtlEnemyCount` and `g_BtlPartyCount`. Right
after, `BtlRun` (`0x80027434`) stops in an endless loop if any active enemy
plans kind 7 at a target of 5 or more, then runs `BtlEnemyActionAdjust`
(`0x80029510`).

**`BtlEnemyActionPick` (`0x8004CAB8`)**: kind 6 (FLEE) when `enemy_total < party_total / 20`;
otherwise a 33-arm switch on **`action_pick` (`+0x1A`), the AI type**
(`BtlEnemyStatsSet` takes it from the enemy row's byte `+0`; the Slime's is 2).
`r` is a fresh `rng_ff_draw()` (`0x8004D294`, `rng_draw() & 0xFF`) at each
test; "bit 7" means the draw's bit 7:

| type | plan |
|------|------|
| 0, > 0x20 | `r < 0x40` → bit 7 of a second draw: clear `BtlEnemyAttackPick(0)` (`0x8004CE70`), set `BtlEnemyKind7Pick(1)` (`0x8004CF48`); otherwise a second draw `r2`: `>= 0x81` → `BtlEnemyAttackPick(0)` (`0x8004CE70`); below 0x20, 0x40, 0x62 or 0x81 → `BtlEnemyKind7Pick(1)`, `(2)`, `(3)` or `(4)` (`0x8004CF48`) |
| 1 | flee when `enemy_total < party_total / 2`, else bit 7: clear `BtlEnemyAttackPick(0)` (`0x8004CE70`), set `BtlEnemyKind7Pick(0)` (`0x8004CF48`) |
| 2-6 | `BtlEnemyAttackPick(type - 2)` (`0x8004CE70`) |
| 7-11 | `BtlEnemyKind7Pick(type - 7)` (`0x8004CF48`) |
| 12-16 | `r < 0x55` → `BtlEnemyKind7Pick(type - 12)` (`0x8004CF48`), else `BtlEnemyAttackPick(type - 12)` (`0x8004CE70`) |
| 17-21 | `r < 0x55` → `BtlEnemyAttackPick(type - 17)` (`0x8004CE70`), else `BtlEnemyKind7Pick(type - 17)` (`0x8004CF48`) |
| 22 | `BtlEnemyAttackPick(5)` (`0x8004CE70`) |
| 23 | `BtlEnemyKind7Pick(5)` (`0x8004CF48`) |
| 24 | `r < 0x55` → `BtlEnemyKind7Pick(4)` (`0x8004CF48`), else `BtlEnemyAttackPick(5)` (`0x8004CE70`) |
| 25 | `r < 0x55` → `BtlEnemyAttackPick(2)` (`0x8004CE70`), else `BtlEnemyKind7Pick(5)` (`0x8004CF48`) |
| 26-31 | `BtlEnemyAttackKind7Pick(type - 26)` (`0x8004CD08`) |
| 32 | flee |

**`BtlEnemyAttackPick(mode)` (`0x8004CE70`)** is the attack plan
`{2, 0, target}` — and it replaces the mode with 5 unless `rng_ff_draw()`
(`0x8004D294`) returns 0, so the mode it is handed applies 1 time in 256.
Modes: 0 nearest party member (`BtlSlotNearestFind(2)` (`0x80039838`)), 1
`BtlPartyTargetHpFind` (`0x8004D0A0`), 2 `BtlPartyTargetHpBelowFFFind` (`0x8004D11C`), 3 `BtlPartyTargetStat08Find` (`0x8004D18C`),
4 `BtlPartyTargetStat08BelowFFFind` (`0x8004D218`). All four load their running best
ONCE and never update it, so each returns the LAST party slot 0..4 passing
a fixed test (0 if none): HP > 0; 0 < HP < 0xFF; HP != 0 and `stat_08`
(`+0x08`) > 0; HP != 0 and `stat_08` < 0xFF. Mode 5 is
**`BtlPartyTargetHpPick` (`0x8004CD50`)**: a random party slot among 0..4 with nonzero HP (HP,
not the active flag): with n of them, the index is 0 (n = 1, no draw),
`r >> 7`, `r / 86`, `r >> 6` or `r / 52` for n = 2..5.

**`BtlEnemyKind7Pick(mode)` (`0x8004CF48`)** is the **special-attack
plan**: the same target choice as `BtlEnemyAttackPick` (`0x8004CE70`) (mode → 5 unless
`rng_ff_draw() == 0` (`0x8004D294`), then the same six pickers), but the action is
**kind 7** — unless the enemy's special row `g_BtlAttackRows[+0x1D * 8]`
has target mode 7 and the chosen target holds status exactly 4, 8 or
0x30, which falls back to kind 2. `BtlEnemyActionAdjust` (`0x80029510`) then
copies `kind7_number` (`+0x1D`, the enemy row's byte `+0x13`) into
`g_BtlSlotPlannedNumbers` for every kind-7 enemy not confused or asleep, and
`BtlActionKindRun` (`0x8002E12C`) sends kind 7 to `BtlActionRowRun` (`0x80031ABC`) — the row's effect
handler. **`BtlEnemyAttackKind7Pick(k)` (`0x8004CD08`)** flips a coin on bit 7 of
`rng_ff_draw()` (`0x8004D294`): clear → `BtlEnemyAttackPick(k)` (`0x8004CE70`), set → `BtlEnemyKind7Pick(k)` (`0x8004CF48`).

### Enemy specials — kind 7 and the numbered attack

Every AI type except 2-6, 0x16 and 0x20 can reach `BtlEnemyKind7Pick` (`0x8004CF48`), and an
enemy whose `attack_number` (`+0x1C`) is nonzero attacks with row `attack_number` instead of
`BtlActionAttackRun` (`0x80031060`)'s swing. Both run a row of **`g_BtlAttackRows`**
(`0x800AC640`; battle-data.md): 127 8-byte rows, never written, ending exactly
where the level-up table `g_ChrLevelGainTable` (`0x800ACA38`) begins. A row is
`{target mode, function number, power, area half width, area half height,
bit number, show number, 0}`; the string of row k's name is `k | 0x300`
(`BtlActionStringDisp` (`0x8004987C`), for kind 7 of an enemy slot). The
table has three readers: `BtlActionRowRun` (`0x80031ABC`) (+1..+6),
`BtlActionTargetAdjust` (`0x8002ED34`) and `BtlEnemyKind7Pick` (`0x8004CF48`)
(+0).

The plan is made by `BtlEnemyActionPick` (`0x8004CAB8`) and its pickers
(§"The generic enemy AI"); `BtlEnemyActionAdjust` (`0x80029510`) gives a
kind-7 plan its row number from `kind7_number` (`+0x1D`); `BtlActionTargetAdjust`
(`0x8002ED34`) checks the target against the row's mode, a nonzero
`attack_number` joining that check with the kind left at 2, and
`BtlActionFallbackSet` (`0x8002F3A0`) is the fallback attack (§"The action
loop"). The rest of the chain:

* **`BtlTargetByModeGet(m)` (`0x800397B4`)**: `m & 0xF` = 0 the actor itself, 1
  `BtlSlotNearestFind(1)` (`0x80039838`), 2..7 **`BtlPartyTargetPick(m)` (`0x8003995C`)**: the party slots
  0..4 with an id passing the mode's test (2 active, 3 inactive, 4 any
  status, active or not, 5 active with HP < max, 6 active with MP < max,
  7 active with `+0x15 == 0`) are listed, and
  **`rng_list_pick(list, n, 7)` (`0x8003A03C`)** draws one —
  `rng_draw_copy() & 7`, less n until below n; with none,
  `BtlActionFallbackSet` (`0x8002F3A0`) gives the actor another action. For
  modes 8 to 15 the C has no return statement.
* **`BtlEnemyActionFallbackSet` (`0x80036A18`)**: ids `0x10 0x18 0x1F
  0x2E 0x44 0x55` take `BtlActionWalkCornerSet(1)` (`0x80039B70`), a walk to a corner
  chosen by where the party's slots stand, and the result becomes their plan; any other enemy
  whose `g_BtlSlotPlannedKinds` is not already 2 becomes kind 2 at the
  nearest slot — of either side (`BtlSlotNearestFind(0)`) when it is
  confused, else the nearest party member (`BtlSlotNearestFind(2)`
  (`0x80039838`)).
* **`BtlActionRowRun` (`0x80031ABC`) cases 2 / 7** load the row: kind 2 first
  takes `+0x1C` as `g_BtlActionNumber`, and kind 7 for enemy id 0x4F uses
  `+0x1D` in place of the number. Bytes 1-6 go to
  `g_BtlActionFunctionNumber`, `g_BtlActionPower`, `g_BtlAreaHalfWidth`,
  `g_BtlAreaHalfHeight`, `g_BtlActionBitNumber` (the element) and
  `g_BtlActionShowNumber`, and it calls entry `function number − 1` of the
  run of 132 function addresses at `0x800B2EF8` (the C's `scene_state_table`)
  when the number is below `0x5F`, so 94 is the highest number it runs.
  **Function number 0 is not guarded**: it passes the `< 0x5F` test and
  calls the word at `0x800B2EF4`, which is 0. Only the boss ids (`>= 0x6E`)
  pair a zero `+0x1D` with an AI type that plans kind 7, and they fight only
  in boss-class encounters, where `BtlEnemyActionSpecialSet` (`0x800417E4`)
  plans instead.

The handlers the attack rows name, by what they need. "The reach check" is
`BtlTargetReachCheck` (`0x800379AC`), made only when `g_BtlAreaHalfWidth`
(row byte 3) is not 0 (§"The approach walk"); in `BtlTargetHitRun` a nonzero
`g_BtlAreaHalfHeight` (byte 4) also lets `BtlHitTry` (`0x8002FF0C`) decide a miss.

| effect | handler | needs |
|---|---|---|
| 17 | `BtlTargetHitRun` (`0x80032958`) | the reach check |
| 21 / 25 | `BtlOtherSideHitRun` (`0x80032BB4`) / `BtlOtherSideStat0EHitRun` (`0x80032ED8`) | every active slot of the other side (`BtlTargetOtherSideMark` (`0x80036394`)) |
| 22 | `BtlTargetStat0EHitRun` (`0x80032BDC`) | the reach check |
| 64 / 66 / 67 / 68 | `BtlTargetHitStatus1Run` (`0x80034398`) / `BtlTargetHitStatus4Run` (`0x800343B8`) / `BtlTargetHitStatus8Run` (`0x800343D8`) / `BtlTargetHitStatus10Run` (`0x800343F8`) → `BtlTargetHitStatusRun(1 / 4 / 8 / 0x10)` (`0x80034418`) | effect 17's handler, then **`BtlTargetStatusSet(code)` (`0x80034544`)** |
| 82 | `BtlTargetHpDrainRun` (`0x80034770`) | a drain, with no reach check whatever byte 3 holds |
| 83 | `BtlActingRemoveRun` (`0x80034844`) | the actor leaves: its id and active byte become 0, with no fall mark and no `BtlFallRun` |
| 27 / 33 / 36 | `BtlTargetStatus1TryRun` (`0x80033140`) / `BtlTargetStatus4TryRun` (`0x8003321C`) → `BtlTargetStatus4Try` (`0x800333B8`) / `BtlTargetStatus8TryRun` (`0x80033440`) → `BtlTargetStatus8Try` (`0x80033660`) | the reach check, then a status roll (`BtlStatusTakeCheck` (`0x80035378`)) and status 0x01 / 4 / 8; 36 measures enemy id 0x10 with neither width counting (a one-tile walker to a one-tile target) |
| 39 / 51 | `BtlTargetStatus10TryRun` (`0x800336DC`) / `BtlTargetStatus80TryRun` (`0x80033A58`) → `BtlTargetStatus80Try` (`0x80033BB8`) | the reach check, then a status roll and stone 0x10 (party targets only, stored by the handler itself with no guard or resist test) / 0x80 |
| 24 / 34 | `BtlTargetAreaStat0EHitRun` (`0x80032C3C`) → `BtlAreaStat0EHitRun` (`0x80032C64`) / `BtlTargetAreaStatus4TryRun` (`0x80033290`) → `BtlMarkedStatus4TryRun` (`0x800332B8`), both after `BtlTargetAreaMark(1)` (`0x80035B50`) | the AREA set (below) |
| 49 | `BtlTargetAreaStatus40TryRun` (`0x8003390C`), after `BtlTargetAreaMark(1)` (`0x80035B50`) | the area, then status 0x40 on each (§"Confusion — status 0x40") |
| 19 | `BtlTargetAreaHitRun` (`0x80032A28`) → `BtlAreaHitRun` (`0x80032A50`), after `BtlTargetAreaMark(1)` (`0x80035B50`) | the area, then the resolver on each flagged slot, the bonus by its flag (§"More effect handlers"); attack row 84 "Press" names it |
| 18 / 94 | `BtlActingAreaHitRun` (`0x80032A00`) → `BtlAreaHitRun` (`0x80032A50`) / `BtlTargetRowsHpRun` (`0x80035238`) → `BtlTargetRowsMark` (`0x80035260`), `BtlMarkedHitRun` (`0x80033060`) | the area around the ACTOR / the band of the target's rows, then the resolver on each. Attack rows 117 / 118 (Kyle's) carry bit number 6: under kind 7 at a party slot `BtlHitAmountSet` hands `BtlTargetTraitCheck` (`0x80037648`) two trait numbers that path never sets (§"The magic element path"; row 116's effect 17 is the same case) |
| 75 | `BtlTargetStat0EHitStatus4Run` (`0x80034498`) → `BtlTargetStat0EHitStatusRun(4)` (`0x800344B8`) | the reach check, `BtlStat0EHitAmountSet` (`0x80035EF8`), then `BtlTargetStatusSet(4)` (`0x80034544`) unless KO'd |
| 20 / 26 | `BtlFrontHitRun` (`0x80032B44`) / `BtlFrontStat0EHitRun` (`0x80032F00`) → `BtlTargetFrontMark` (`0x8003568C`) → `BtlMarkedHitRun` (`0x80033060`) / `BtlMarkedStat0EHitRun` (`0x80032FA0`) | the LANE (below), then the resolver / `BtlStat0EHitAmountSet` on each |
| 84 | `BtlPartyPlaceFallRun` (`0x80034898`) | enemy 0x4F's marked tile (below) |
| 60 | `BtlPartyHpDrainRun` (`0x80034108`) | `BtlStat0EHitAmountSet` on every active party slot 0..4; the caster regains the whole sum (number kind 5; the number shown is the sum, before HP stops at the maximum); then `BtlFallRun` ([boss-ai.md](boss-ai.md) §"The second Magic Emperor, encounter 0xF7 — a two-actor boss") |
| 61 | `BtlSlot7HpSetRun` (`0x80034260`) | the presentation (0xA4 shows actor 7), then record 7's HP = caster level x 10 + 500 — encounter 0xF7's shield |
| 62 | `BtlTargetFallRun` (`0x800342C0`) | the target falls (number kind 1, fall mark, `BtlFallRun`) unless bit 0x02 of its `+0x1E`, the barrier bit, takes it: the bit is cleared and the number kind is 6 |

* **`BtlTargetStatusSet(code)` (`0x80034544`)** — an enemy target with a boss id
  (`>= 0x6E`): nothing, not even the pose; `g_BtlSlotAmountKinds[t] == 6` (the
  barrier took the hit): nothing. Otherwise codes 1 / 4 / 8 / 0x30 / 0x40 /
  0x80 go to the appliers `BtlHitStatusUpdate` (`0x80030A04`) uses
  (`BtlTargetStatus1Set` (`0x8002F848`), `BtlTargetStatus4Set` (`0x8002F74C`),
  `BtlTargetStatus8Set` (`0x8002F958`), `BtlTargetStatus30Set` (`0x8002FAFC`),
  `BtlTargetStatus40Set` (`0x8002FC08`), `BtlTargetStatus80Set` (`0x8002FC94`)),
  each with its own guard and resist test. **0x10** (stone) is skipped while
  the target's status has the bit 0x20 (so under 0x20 and under 0x30); then
  — like 0x20, which has no guard — it is resisted when
  `(BtlTargetTraitCheck(0x800, 0x25, 0x26, 0) & 0xFF) == 1` (`0x80037648`), or
  else the code's low byte is STORED as the WHOLE status byte and all four
  round counters are cleared (unlike `BtlTargetStatus30Set`, it clears
  neither the active flag nor a party member's `+0x1E` bit 0x04). Any other
  code does nothing. `BtlSlotStatusAnimationSet` (`0x8003801C`) re-poses on
  every path but the two early returns — on a resist, a guard refusal and an
  unknown code too. No draw. Only codes 1 / 4 / 8 / 0x10
  (`BtlTargetHitStatusRun` (`0x80034418`)) and 4 (`BtlTargetStat0EHitStatusRun`
  (`0x800344B8`)) are ever passed, so the 0x20 / 0x30 / 0x40 / 0x80 arms are
  unreached by the callers. `BtlTargetHitStatusRun` (`0x80034418`) calls it
  only when the hit landed (`g_BtlSlotAmountKinds` 1/3/4) and
  `g_BtlSlotFallMarks[t]` is clear — and the KO cleanup `BtlFallRun`
  (`0x800385B4`) does not clear that flag, so **a target the hit KO'd never
  takes the status**.
* **`BtlTargetHpDrainRun` (`0x80034770`)** (effect 82) — `BtlStat0EHitAmountSet` (`0x80035EF8`) on the
  target; the target's stored amount is COPIED into the caster's entry
  (`g_BtlSlotAmounts[caster] = g_BtlSlotAmounts[t]`) and added to the caster's HP (u16
  add); when HP is strictly above max HP the overshoot comes off the
  caster's entry and HP = max HP; `g_BtlSlotAmountKinds[caster] = 5`, the
  presentation `BtlShowNumberRun(g_BtlActionShowNumber)` (`0x8007BB80`), then `BtlTargetStatus8Clear` (`0x80030C28`). The
  target's reaction byte is never read: the caster gains the stored
  amount whatever the damage tail decided.
* **`BtlTargetAreaMark(side)` (`0x80035B50`)** — the AREA set, on a
  grid of its own: 40 × **28** cells of 8 px from (0, 0) (`0x460` bytes at
  `*g_LoadBuffer`, cleared). Each active slot of the other side (its own
  side for a confused actor, `g_BtlActingStatus & 0x40`) other than the actor
  writes slot + 1 into cells `x/8 ± j`, j < `+0x3C >> 4`, of its row
  `y/8` — flat-indexed, so the left edge spills into the row above, and a
  record narrower than 16 px writes nothing. Then
  **`BtlTargetAreaCellsMark(w, h, x, y)` (`0x80035DE0`)** flags (1) every slot found in the
  `(2·g_BtlAreaHalfWidth + 1) × (2·g_BtlAreaHalfHeight + 1)` rectangle
  centred on the target (side ≠ 0) or the caster, clipped to the grid;
  with side ≠ 0 the target itself is flagged 3; the caster never is.
  `g_BtlSlotAmountKinds` starts each action clear (`BtlActionOutcomeClear`
  (`0x800364C0`)).
* **`BtlAreaStat0EHitRun` (`0x80032C64`)** — effect 24's worker, each
  slot 0..13 in order, with `stat_0e` (`+0x0E`) of the caster and `stat_10`
  (`+0x10`) of the target: flag 1 → `roll = rng_mask_below(0xFFFF, stat_0e / 10)` (`0x8003A0D4`),
  `(stat_0e + (m · (roll + level·5/2) & 0xFFFF) − stat_10) / 2`
  (floor 0; the target's `stat_10` ×13/10 — `ratio_13_10_mul` (`0x8002FE54`) — when it
  is a party member resting on the defend pose 0x13) through `BtlTargetHpAmountTake` (`0x80035FD8`); flag 2 / 3 →
  `BtlStat0EHitAmountSet` (`0x80035EF8`) with power 2m / 3m; m = 2 for spell row `0x1A`
  cast as magic (kind 3), else 1. Then the presentation, KO cleanup, poses
  (`BtlMarkedRestAnimationSet` (`0x80030DA0`)) and
  `BtlMarkedStatus8Clear` (`0x80030B8C`). Effect 34 (`BtlMarkedStatus4TryRun` (`0x800332B8`)) instead rolls status 4
  (`BtlTargetStatus4Try` (`0x800333B8`)) on each flagged slot. Party spell
  rows 10, 22, 26 (24) and 12 (34) use them too.
* **`BtlEnemy4FPlaceMark` (`0x800279A0`)** — the round's step after
  `BtlEnemyActionAnimationSet` (`0x8002965C`): every active enemy with id **0x4F** (Ultra Fiend)
  planning kind 7 draws `BtlPartyTargetPick(2)` (`0x8003995C`) and MARKS that party member's
  tile (`g_BtlSlotPlannedNumbers = x/8`, `g_BtlSlotPlannedTargets = y/8`), then a
  copy of its own record is drawn on the tile with animation 0xF, the enemy
  faces it, and 100 frames pass. `BtlActionTargetAdjust` (`0x8002ED34`) keeps
  0x4F's plan, and `BtlActionRowRun` (`0x80031ABC`) loads its row from `+0x1D`
  for kind 7, so `g_BtlActionNumber` / `g_BtlTargetSlot` stay the tile: attack row 76 is
  effect 84, **`BtlPartyPlaceFallRun` (`0x80034898`)**: every active party slot whose Y is
  in tileY·8 − 8 .. tileY·8 and whose near edge (x ± `+0x3C`/2, toward the
  caster; "left of the caster" is a strict test) is within tileX·8 ± 0x20,
  both ranges inclusive, is KO'd outright: `g_BtlSlotAmountKinds = 1`,
  `g_BtlSlotFallMarks = 1` and HP 0. `BtlShowNumberRun` (`0x8007BB80`) runs
  whether or not anyone was caught. Whoever moved off the mark escapes;
  whoever stepped onto it does not.
* **The LANE — `BtlTargetFrontMark(side)` (`0x8003568C`)**, effects 20
  and 26: with `g_BtlAreaHalfWidth` set, **`BtlWalkEdgeRun(anim)` (`0x80036654`)** first
  puts actor RECORD 14 on the target's row at the far screen edge from the
  target (x = `0x130 − +0x3C/2` of the caster when the target stands left of `0xA1`, else
  `+0x3C/2 + 8`) and walks the caster toward it (walk block `+0x0B` = 0;
  encounters 0xFE / 0xF7 also clear `+0x09`); ids 0x7D, 0x7C, 0x74 and 0x7F
  add poses of their own around the walk. Record 14 is also the record whose
  y `BtlActionWalkCornerSet` (`0x80039B70`) reads by its loop-counter quirk.
  Then (`BtlFacingBothSet` (`0x8003693C`) turning the two to face each other)
  the caster faces the target, and every active slot of the range other
  than the caster (side 0: party, 1: enemies, 2: all — `BtlFrontHitRun`
  (`0x80032B44`) / `BtlFrontStat0EHitRun` (`0x80032F00`) pass 2 for a confused
  caster, 1 for a party one, else 0) whose edge on the caster's facing side
  (x ∓ its `+0x3C`/2) lies beyond the caster's edge on that side, within
  the caster's y − (`g_BtlAreaHalfHeight`·8 + 8) .. y + `g_BtlAreaHalfHeight`·8,
  is flagged 1. Effect 20 runs the resolver on each (`BtlMarkedHitRun`
  (`0x80033060`)), 26 `BtlStat0EHitAmountSet` (`BtlMarkedStat0EHitRun`
  (`0x80032FA0`); in encounter 0xFE, action number 0x73 runs at power 30, band 8).
* **`BtlPartyActionAdjust` (`0x80029348`)** — the party's twin of `BtlEnemyActionAdjust` (`0x80029510`): an
  active member whose `g_BtlPartyNotHeldFlags` byte is 0 gets kind 0xB; a
  confused one re-plans from `rng_draw_copy() & 7` (`0x8001C790`) (or `& 3`)
  (§"Confusion — status 0x40"). It draws only for status `0x40`.

**For an enemy of `action_pick` 2 (the Slime's) this is: attack, a random
target — and then `BtlActionTargetAdjust` (`0x8002ED34`) replaces any party-side
target with the nearest party member.** The random pick only costs its draws.

### The walk planner's scratch — what each function does

The planner is `btl_walk.c` and `btl_path.c`. The reach check
`BtlTargetReachCheck` (`0x800379AC`) asks it whether the target is within
reach; the flood, the out-of-reach search, the path and the decision flow
are in §"The approach walk".

The arena is a **40 x 12 tile grid** (`0x28` columns, `0xC` rows, 8 px
tiles; row = `(y - 0x58) / 8`, column = `x / 8`). `g_BtlWalkGrid` points at a
40-wide `u8` cost grid at `*g_LoadBuffer`; `0xFE` marks a cell an actor
occupies (and rows 0-2, unless the walker is slot 5), `0xFF` is
unreached. The planner's small globals:

| global | meaning |
|---|---|
| `g_BtlFromRecord` / `g_BtlToRecord` | walker / target record pointers (copied from `g_BtlWalkBlock +0 / +4`) |
| `g_BtlWalkerColumn` / `g_BtlWalkerRow` | walker column / row (tiles) |
| `g_BtlWalkerWidth` | walker width in tiles (`+0x3C >> 3`, rounded up to even), or 1 |
| `g_BtlWalkTargetColumn` / `g_BtlWalkTargetRow` | target column / row |
| `g_BtlWalkWidths[0]` / `[1]` | target width in tiles / the span count the scans use |
| `g_BtlWalkGoalDistance` | best cost found so far (seeded with reach + 1 by `BtlWalkGoalFind` (`0x8002C304`)) |
| `g_BtlWalkSeedColumn` / `g_BtlWalkSeedRow` | seed column / row for the neighbour scan |
| `g_BtlWalkGoalColumn` / `g_BtlWalkGoalRow` / `g_BtlWalkCellIndexes` | result column / row / flat index (`row * 40 + col`) |
| `g_BtlWalkRingHeight[1]` | the scan's running count |

The walk block `g_BtlWalkBlock` (`0x800E4B10`, `BtlWalkBlock` in `include/btl.h`;
filled by `BtlWalkSet` (`0x8002BAA4`)): `+0` walker record, `+4` target
record, `+8` walker slot (`< 5` = party), `+9` the other slots' records
block cells, `+0xA` the walker's width counts, `+0xB` the target's width
counts, `+0xC` walk (0: only measure whether the target is within reach),
`+0xD` walk pose, `+0xE` speed (1/2/3/other -> a tile every 1/2/4/8 frames,
i.e. 8/4/2/1 px a frame — `BtlWalkPathFollow` (`0x8002CA78`) steps
`value_div(8, n)` (`0x80077EE8`) = 8 / n px for n frames), `+0xF` reach in
tiles, `+0x10` the walk's sound (`0xFF` = none). `BtlWalkSet` sets `+9` to
`+0xC` to 1 and copies speed and reach from the walker's stats (`+0x18`,
`+0x13`); a party walker gets its `g_BtlPartyMoveAnimations` pose and sound
0x2A, an enemy pose 1 and its stats' `+0x1E`.

- **`BtlWalkRun` (`0x8002BB5C`)** — the driver. Halves the reach for a party walker
  when `g_BtlActingStatus & 0x10`, caps it at 40, snaps the walker's x/y to
  the nearest multiple of 8 through `BtlWalkCellRound` (`0x8002BA40`) (the
  target's are not snapped), derives the tile coordinates above, and returns **1** at
  once when both share a tile. Otherwise it widens the walker (`+0xA`: its
  column moves left by half its width) and the target (`+0xB`) by their sprite widths — the
  target path calls `BtlWalkBesideCheck` (`0x8002C9AC`) and returns **2** through
  `BtlWalkFacingSet` (`0x8002CA34`) when the two stand side by side; with the
  target's width not counting, the target column moves left by half the
  walker's width instead — then `BtlWalkGridInit` (`0x8002BEC8`) (blocks the
  occupied cells), `BtlWalkDistanceFill` (`0x8002D254`) (the cost flood), `BtlWalkGoalFind` (`0x8002C304`) (the
  best-cell search). Verdict: best cost `<= reach` returns **1** (reachable;
  with `+0xC` set it walks there: `BtlWalkPathMake` (`0x8002CD04`) then the walk
  `BtlWalkPathFollow` (`0x8002CA78`); a best cost of 0 with `+0xC` set
  returns 0 with no walk), else **0** (with `+0xC`: `BtlWalkGoalNearestFind` (`0x8002C3E8`) picks a
  fallback cell and walks unless it is the walker's own tile).
- **`BtlWalkGoalFind` (`0x8002C304`)** — seeds the search: best = reach + 1, seed row =
  target row. A wide target (`+0xB`) seeds the column at its left column + 1
  − the walker's width, with a span of the target's width + the walker's
  width − 1 (clipped at column 0), then `BtlWalkNeighbourLowestFind`
  (`0x8002CEE0`); a point target takes its own cell when its cost is within
  reach, else seeds there with a span of 1 and runs the same search.
- **`BtlWalkNeighbourLowestFind` (`0x8002CEE0`)** — the neighbour search around the seed run:
  the cell right of the span, the cell left of the seed, the cells above and
  below the seed, then the row above and the row below, each from one cell
  left of the seed for span + 2 cells; the lowest cost that beats the
  running best wins and lands in `g_BtlWalkGoalColumn` / `g_BtlWalkGoalRow`.
  Its two result variables are a third and a fourth parameter that no
  caller passes, so when no cell is lower the two words get whatever those
  registers (`$a2/$a3`) held — the same kind of leftover as the
  uninitialised `$a3` of `MapObjectRandomWalkStart` (`0x80078B88`).
- **`BtlWalkCellDistanceGet(x, y)` (`0x8002D89C`)** — the flood's relaxation, on the 40x12
  byte grid `*g_BtlWalkGrid` (`cell(x, y) = base[y * 40 + x]`, the pointer
  reloaded for every cell). `best = 0xFF`, lowered by each on-grid neighbour
  with an unsigned byte `<`, in the order row y−1 (left, middle, right; only
  when `y != 0`), right (`x < 39`), row y+1 (only when `y < 11`), left
  (`x != 0`); then `c = cell(x, y)`: `c < best` returns `c`, else
  `best < 0xFE` returns `best + 1`, else `0xFF`. So a cell keeps its own cost
  only when it is STRICTLY lower than its lowest neighbour; a cell EQUAL to
  its lowest neighbour is raised to that + 1, and a lowest neighbour of
  `0xFE` (blocked) or `0xFF` gives `0xFF`. It writes nothing, calls nothing
  and draws nothing — `BtlWalkDistanceFill` (`0x8002D254`) (four call sites,
  one per ring side) stores the result.
- **`BtlWalkGridSlotBlock(slot)` (`0x8002C190`)** — writes `0xFE` across the cells an actor's
  footprint covers on its row (width tiles rounded up to even, centred,
  clamped to the row). Called by `BtlWalkGridInit` (`0x8002BEC8`) and `BtlEscapeFailGridInit` (`0x80037188`).
- **`BtlWalkPathFollow` (`0x8002CA78`)** — the walk itself, when the path has
  nodes: step size from `value_div(8, frames a cell)` (`0x80077EE8`), pose
  `+0xD`, then per path node (`g_BtlWalkPathColumns` columns,
  `g_BtlWalkPathRows` rows, `g_BtlWalkNodeCount` nodes left, `g_BtlWalkGoalDistance` the frame count) it moves
  the walker's x/y by the step toward the node's pixel position
  (`col * 8`, `row * 8 + 0x58`), setting the facing word `+0xC` (`0x20` /
  `0x2020`) when the x differs and calling `BtlFrameStep` (`0x80038D2C`) each frame; at the end it restores
  the rest pose from `g_BtlSlotRestAnimations[slot]` (slot `< 14`) and, for id
  `0x73`, first waits out the animation (`BtlAnimationEndWait` (`0x8002FDBC`)). `SoundPlay(+0x10, 3)` (`0x800555F0`) at
  the start and `(0, 3)` at the end bracket the walk's sound.
- **`BtlTargetReachCheck` (`0x800379AC`)** — the per-id wrapper: after
  `BtlWalkSet` (`0x8002BAA4`) a first `BtlWalkRun` (`0x8002BB5C`) only measures
  (`+0xC` = 0) and answers **1** at once when the target is within reach.
  Otherwise `+0xC` = 1: a party walker walks twice running, the second
  walk starting where the first ended (§"The approach walk"), and the
  answer is 0. Enemy ids `0x59/0x2A/0x39/0x48` set the walk pose `+0xD = 0xB`,
  `0x12/0x7D/0x7C` `+0xD = 1`, `0x79` `+0xD = 0xD`, `0x7F` `0x10`, each with a
  pose before and after the walk (9 and 0xA unless the id has its own); it
  plays the first pose, walks with a second `BtlWalkRun` (`0x8002BB5C`),
  plays the second pose, and for `0x7C` also moves an extra actor at
  `g_BtlRecords + 0x1C0` (record 7) to the walker's position. Any other id:
  `BtlWalkRun(); return 0` (`0x8002BB5C`). Every walk ends with the answer 0,
  so the action does not go on that turn.

### Wake on hit — `BtlTargetStatus8Clear` (`0x80030C28`)

A target holding status 8 loses it when hit: `+0x15 &= ~8`, party
`+0x1E &= ~4`, `g_BtlSlotStatus8Rounds[t] = 0`, re-pose (by its remaining
status, else by its planned action, else a party member's stand pose or an
enemy's pose 0). Callers: the tail of
`BtlHitStatusUpdate` (`0x80030A04`) — every on-hit status pass, **except** the paths out of
the 4 / 8 / 0x30 appliers, which jump past the call —
effect 22 (`BtlTargetStat0EHitRun` (`0x80032BDC`)), effect 82
(`BtlTargetHpDrainRun` (`0x80034770`)) and `BtlTargetHpFixedRun`
(`0x80034B8C`) directly, and `BtlMarkedStatus8Clear` (`0x80030B8C`) (every
marked, still-active slot), the last call of `BtlHitEnd`,
`BtlMarkedStat0EHitRun` and `BtlAreaStat0EHitRun`, so at the end of every
effect that hits a marked set (18-21, 24-26, 94 and others). Besides this,
status 8 skips the turn (`BtlActionAdjust`), doubles the hits taken
(`BtlHitAmountSet` (`0x80030070`)'s `0xC` test) and removes evasion
(`BtlHitTry` (`0x8002FF0C`)'s `0x3C` test).

### Run

* **Per character, kind 6** (`BtlActionKindRun` (`0x8002E12C`)): a party actor fails when
  `g_BtlNumber >= 0xDF` (no draw) or `rng_7f_draw() >= 0x60` — gets away on
  96/128 — and a failure is `BtlEscapeFailMove(actor)` (`0x80036BE4`), the shake. An enemy
  always goes. Success is **`BtlSlotLeave` (`0x8002E398`)**: sound 0x26, and the
  actor walks off its side (party right to x = 0x158 on its walk pose with
  `+0x0C = 0x20`, enemies on pose 1 left past `-(+0x3C)` with `0x2020`, 4 px
  a frame); its id, active flag and its record's `+0x00` word are zeroed —
  its HP is not. If that leaves no party slot active, the end message runs,
  the companion and the enemies move off (`BtlSlot5EscapeMove`,
  `BtlEnemyEscapeMove`) and the battle ends as an escape (3); otherwise
  `BtlEndCheck`'s answer is returned (1 when the last enemy has left).
* **Main menu** (`BtlPartyEscapeTry` (`0x80027DAC`)): unless `g_BtlNumber >= 0xDF`,
  `rng_7f_draw() >= 0x40` (64/128) succeeds: sound `0x26` and
  `BtlPartyEscapeMove` (`0x80027EF0`), then every active member whose status
  has none of `0x4C` leaves the field (id, active flag and record `+0x00`
  zeroed), the rest stay; all gone → the end message and the escape (`BtlRun`
  (`0x80027434`) ends with 3). A failure shakes every free member
  (`BtlEscapeFailMove(0xFF)` (`0x80036BE4`)). The round then runs in
  `g_BtlPartyMode` 2, with the party's `stat_0c` out of the order key
  (§"Turn order").
* **The shake MOVES them** (`BtlEscapeFailMove` (`0x80036BE4`), `BtlEscapeFailStepPick` (`0x80036F3C`),
  `BtlEscapeFailGridInit` (`0x80037188`); sound 0x27). The
  runners — the one that failed, or with 0xFF every active party member
  free of status 0x4C (and commandable, `g_BtlPartyNotHeldFlags`) — make three passes:
  the walk anim; three steps, each rebuilding the PLACEMENT grid
  (`BtlEscapeFailGridInit` (`0x80037188`): all open; with one runner the other party slots with an
  id marked, with 0xFF none; every active enemy marked; rows 0-2 and
  columns 0-1 / 38-39 walls; in enemy set 0x1C also columns 0-21 of every
  lower row) and giving each runner, in
  slot order, one of the OPEN cells among its eight neighbours (N, NE, E,
  SE, S, SW, W, NW) drawn by `rng_mask_below(7, n)` (`0x8003A0D4`) — then 4 frames of 2 px
  toward it; the rest pose; then the facing bit 0x2000 of `+0x0C` toggled
  four times, one every four frames (the runners turn about twice). So a
  failed escape scatters the party by up to nine tiles
  and spends up to nine draws per runner (none for a step with fewer than
  two open cells). A runner on row 11 tests row 12
  — past the grid, into the rest of the `g_LoadBuffer` buffer (leftover
  data).

## Victory — `BtlWinRun` (`0x80028140`)

`BtlRun` (`0x80027434`) calls `BtlWinRun(party)` (`0x80028140`) only for a win
(result 1), with `party` its copy of the party ids taken at the battle's
start. Before it come the fanfare `MusicPlay(0x3D)` (`0x80054F2C`) (`0x3E` in a
battle numbered `0xEB` or more), the end message and a pad wait. In order
(`btl_run.c`):

1. Party slots 0..4 with an id: one holding a status has its status and its
   `+0x1E` bit `0x04` cleared and is made active again; one with no status
   that is inactive (fallen) is revived by `BtlSlot5Revive(0)` (`0x80028E5C`),
   slot 5's revive with no voice: HP 1 (10 once story bit `0x2AB` is set),
   `stat_08` and `stat_0a` put back from their start copies, active again.
   Any revival → `BtlFrameWait(0x14)` (`0x80036BA8`) — a 20-frame wait (`n` ×
   `BtlFrameStep` (`0x80038D2C`), the frame wait; no draw of its own). Then
   `BtlPartyPanelDisp` (`0x8004B330`) redraws the panel.
2. **`BtlWinRewardSum` (`0x80028D78`)**: over enemy slots 6..13 that still hold
   an id — a killed enemy keeps it; one that ran (`BtlSlotLeave`
   (`0x8002E398`)), or that effect 15 (`BtlEnemyClearRun` (`0x8003281C`)) or 58
   (`BtlEnemyAllRemoveRun` (`0x80033F0C`)) took off, does not — **EXP** = Σ
   `experience` (`+0x24`, capped at 99999, into `g_BtlWinExperience`) and
   **money** = Σ `money` (`+0x26`; `g_BtlWinMoney` holds the sum capped at
   0xFFFF; the uncapped sum is added to the party's money `*g_GameMoney`,
   capped at 9999999). `BtlEnemyStatsSet` takes the two from the enemy row's
   words `+0x1A` / `+0x1C`. The Slime is worth 1 EXP and 6 money.
3. **`BtlWinItemsGive` (`0x80028A54`)**: per such enemy in slot order — if
   `win_item_1` (`+0x20`, the enemy row's byte `+0x1E`) is an item 1..0xA4,
   it drops when **`BtlWinItemRateCheck` (`0x80028A0C`)** passes: a nonzero
   item and `rng_7f_draw() <= win_rate_1` (`+0x21`); if that fails,
   `win_item_2` (`+0x22`) with `win_rate_2` (`+0x23`). An out-of-range first
   item means NO drop and no second roll; only a nonzero item spends a draw.
   The drops (at most 8, one per enemy slot) go to `g_BtlWinItems`, then onto
   id `0xC`'s 0x60-byte item list `*g_ChrItemList0C`; what does not fit goes
   to party members 0..4 (ids 8..10 excepted), at most one each, into the
   first empty one of the six item bytes at `+0x2E`. `ChrItemListSort` then
   runs for each party id and once for `0xC`. The Slime drops item 156 on
   `<= 1` (2/128).
4. `BtlWinMessageDisp` (`0x8004BB68`) — the victory pose (animation 0xF) for
   each standing member, then the messages of the reward and of each item
   won.
5. **Level-ups**, per party slot with an id, against the persistent 0x40
   record `g_ChrRecords + id * 0x40`, spending the award `g_BtlWinExperience`: while
   `experience_next` (`+0x3C`, EXP to the next level) `<=` the EXP left and the
   level `+0x1C` is below 99, the EXP left drops by `+0x3C`, `g_ChrLevelUpGains`
   is cleared with the id in its first byte, `ChrLevelUp(id, record)`
   (`0x80028554`) raises the record one level (§"Level-ups" — it increments
   `+0x1C` itself), and the BATTLE record `CharStats[slot]` gets the same HP
   and MP gains on both current and max (`ChrLevelGainAdd` (`0x800289E0`) ×4,
   cap 999) and its level `+0x14` +1; sound `0x1F`, the level-up window
   (`BtlPartyPanelDisp` (`0x8004B330`), `BtlLevelUpMessageDisp` (`0x8004BC7C`)).
   Then `+0x3C` -= what is left (0 at level 99), and `experience` (`+0x38`,
   total EXP) += the whole award, capped at 9999999. `stat_08` and the other
   stats are NOT refreshed in `CharStats` — they change in the persistent
   record and reach battle at the next `BtlPartyStatsSet` (`0x8002A138`).

## Level-ups — `ChrLevelUp` (`0x80028554`), `ChrLevelAdd` (`0x8002844C`)

**`ChrLevelUp(id, rec)` (`0x80028554`)** (`chr_levelup.c`) raises a persistent
record one level. `L` = the OLD level (`rec+0x1C`, latched in
`g_BtlTargetSlot`); `hdr = g_ChrLevelGainTable + (id - 1) * 0x14`, ten big-endian u16
offsets from `g_ChrLevelGainTable`; `gain(w) = g_ChrLevelGainTable[BE16(hdr[w]) + L]`
(`ChrLevelGainGet` (`0x80028520`), below). In order, each gain also saved
in `g_ChrLevelUpGains[1..9]` for the window:

| word | into | rule |
|---|---|---|
| 0 | `+0x02` max HP | `ChrLevelGainAdd` (`0x800289E0`): add, 999 cap |
| 1 | `+0x06` max MP | the same — **skipped for ids 7 and 9** |
| 2-6 | `stat_08`..`stat_10` (`+0x08`..`+0x10`) | add, 999 cap; and the u8 copies `stat_08_bare`..`stat_10_bare` at `+0x14`..`+0x18`, saturating at 255 |
| 7 | `attack_count` (`+0x12`) and `attack_count_bare` (`+0x19`) | u8 add, wraps |
| 8 | `walk_reach` (`+0x13`) and `walk_reach_bare` (`+0x1A`) | u8 add, wraps |
| — | `+0x1C` | `L + 1` |
| 9 | `experience_next` (`+0x3C`) | `BE32(g_ChrLevelGainTable + BE16(hdr[9]) + L * 4)` — EXP to the next level |

Every id's word 9 is `0xDC`: one shared EXP table, entry `k` = the EXP from
level `k + 1` to `k + 2` (20, 26, 34, 44, 57 … 232996 at 98→99; entries 98
and 99 are 999999 and never reached). Each level-1 record's `+0x38 + +0x3C`
is exactly entry 0 (Alex 10 + 10, id 7 18 + 2, the rest 0 + 20).

Then **the spell**: over spell-table rows 1..50 (`g_BtlSpellRows`, stride
0x0C), the FIRST row whose `+0x07` equals the id (id `0xB` matches id 1's
rows) and whose `+0x00` equals the NEW level is learned — row number into
`g_ChrLevelUpGains[0xA]`, and into `spells` (`rec+0x2A + slot`), where a
switch on the id gives the slot:

| id | slot |
|---|---|
| 1, 11 | row − 1 |
| 2 | row − 9 below row 13, else row − 39 |
| 3 / 4 / 5 / 6 | row − 13 / − 21 / − 29 / − 33 |
| 7-10 | no case: the C leaves the slot unset — unreachable with this table, because rows of groups 7 and 8 are all learned at level 1 |

So **spell row `+0x00` is the learn level** and **`+0x07` is the learner's
character id** (§"Caster groups"). Checked against data the code never read:
resource 0xE7's level-1 lists sit in exactly these slots — id 1 `[1]`, 2
`[_, 10, …, 46]`, 3 `[13, …, 20]`, 4 `[_, 22, _, _, 25, _, _, 28]`, 5
`[29..32]`, 6 `[33]`, 11 `[1, _, _, _, 5, 6, 7, 8]` — and raising ids 2, 3
and 4 to 99 fills all eight slots without a collision, and id 6 fills six —
one per row of group 6. Id 10 holds group 7's two rows from the start;
groups 7/8 are never learned by levelling.

**`ChrLevelGainGet(hdr)` (`0x80028520`)** is one gain:
`u8 ChrLevelGainGet(u8 *hdr)` returns `g_ChrLevelGainTable[BE16(*(u16 *)hdr) +
g_BtlTargetSlot]`. `hdr` points at one big-endian u16 inside the member's
header; that word is the offset from the table's base to the gain's byte
row, and the row is indexed by the OLD level, which `ChrLevelUp` (`0x80028554`)
stores in the u8 `g_BtlTargetSlot` before its first call. It reads
only: no store, no callee, no RNG draw, no frame wait. Its one caller is
`ChrLevelUp` (`0x80028554`), at seven sites: `hdr + 0, 2, 4, 6, 8, 0xA, 0xC`, words 0
to 6 of the table above. Words 7 and 8 are read by the same expression
written inline in `ChrLevelUp` (`0x80028554`), and word 9, the EXP row of 4 bytes a
level, is inline there too.

**`ChrLevelAdd(id, n)` (`0x8002844C`)** — n levels:
unless the record is already at 99, `g_ChrLevelUpGains` is cleared and up to `n &
0xFF` times `+0x38 += +0x3C` (capped 9999999) and `ChrLevelUp` (`0x80028554`);
reaching 99 zeroes `+0x3C` and stops. Each level is paid for with exactly
its own EXP, so `+0x38` stays the level's threshold. Callers:

* **`ScriptCommandChrLevelItemSet` (`0x8005D2F0`), kind 0** (`lns_intp.c`) —
  "set level N", what a joining character runs: if N is below the record's
  level it first reloads that id's raw record from resource 0xE7
  (`resource_load(0xE7, 0x801CFB00)`, 0x40 bytes); if the level then differs,
  `ChrLevelAdd(id, N - level)` (`0x8002844C`), and HP/MP are set to their
  maxima. (Kind 7 gives the item `value` with `ChrItemGive`, or for a value
  of 0 clears the six item bytes at `+0x24` — or, for id `0xC`, the 0x60-byte
  list; any other kind is a worn-item place: `ChrItemUseGet` (`0x80058088`),
  whose answer is not used, `ChrWornItemRemove` (`0x80058038`),
  `ChrWornItemSet(id, place, item)` (`0x80057FFC`).)
* **`ScriptCommandChrLevelGapAdd` (`0x8005EC30`)** (`lns_intp.c`; script command
  0x57) — levels member `who` up to the level of the member in the
  party's FIRST slot (not an operand), by the whole gap or half of it when
  the command's fourth byte is 1; nothing happens when `who` is ahead, and
  HP / MP are refilled even for a gap of 0 (npc-scripting.md §"Smaller
  commands and helpers").
* `ChrLevelSet` (`chr_level.c`), which nothing reaches, does what kind 0 does
  and takes the six worn items off around the reload.

**What a party member at level N is**, then: resource 0xE7's equipped
level-1 record (§"The party half") through `ChrLevelAdd(id, N - 1)`
(`0x8002844C`) and the refill — exactly `ScriptCommandChrLevelItemSet`'s
result on a new-game record; `BtlPartyStatsSet` (`0x8002A138`) turns it into
the battle record. Facts the tables give: id 7 gains only HP (42 → 85 by
level 99) and `walk_reach` (`+0x13`, 10 → 18); ids 8 and 9 start at 99; Alex
at 99 has 525 HP / 340 MP.

## The plain physical attack — `BtlActionAttackRun` (`0x80031060`)

`BtlActionKindRun` (`0x8002E12C`) switches on the action kind (kinds
2..10). Kind 2 sends **every party attacker, and every enemy whose
`attack_number` (`+0x1C`) is 0**, to `BtlActionAttackRun` (`0x80031060`); an
enemy with a nonzero `+0x1C` goes through `BtlActionRowRun` (`0x80031ABC`)
instead, i.e. the row table `g_BtlAttackRows` and its effect handler. (Row
0 is all zero; a function number of 0 would make `BtlActionRowRun` call the
word before its run of 132 function addresses at `0x800B2EF8`, which is 0.
The dispatch is why that never happens.)

```c
void BtlActionAttackRun(void) {                       /* btl_hit.c */
    a = g_BtlActingSlot;
    if (a < 5 && (traits[4] == 0x18 || traits[5] == 0x18)) {   /* +0x2C / +0x2D */
        BtlAttackAllRun(); return;   /* one hit on every active slot of the other side */
    }
    BtlWalkSet();
    for (n = CharStats[a].attack_count; n != 0; n--) {  /* u8: +0x12 SWINGS */
        if (g_BtlNumber >= 0xEB && a >= 6)            /* boss-class: re-pick */
            g_BtlSlotPlannedTargets[a] = g_BtlTargetSlot = BtlPartyTargetPick(2);   /* every swing */
        if (BtlAttackTargetFind()) return;           /* target gone & fight over */
        g_BtlFallDone = 0; g_BtlWalkBlock.target = &records[g_BtlTargetSlot];
        BtlAttackOutcomeClear();                     /* clear per-swing flags */
        if (BtlAttackWalkRun()) {                    /* reach (geometry) */
            if (BtlHitTry()) {                       /* evasion */
                BtlHitAmountSet();                   /* resolve + apply */
                BtlAttackHitShow();                  /* hit presentation */
                BtlHitSlotsUpdate();                 /* post-hit pass */
                BtlPartyPanelDisp(); BtlF7Slot7RecordOff();
            } else BtlAttackMissShow();              /* miss presentation */
        } else BtlActingRestAnimationSet();
        BtlFrameStep();                              /* frame wait */
    }
}
```

`BtlAttackAllRun` (`0x800318DC`) is the attack of a party slot whose fifth or
sixth trait is `0x18`; `BtlPartyTargetPick` is at `0x8003995C`, `BtlHitTry`
§"The evasion check", `BtlHitSlotsUpdate` §"The post-hit pass".

* **`attack_count` (`+0x12`) is the swing count.** It is the field
  `BtlHitAmountSet`'s compounding loops on (`(u8)(count-1)` times for kind 3);
  here the plain attack swings that many times. A count of 0 = no swing at
  all.
* **`BtlAttackTargetFind` (`0x800317E0`)** — if the current target's active flag
  is clear: return `BtlEndCheck()` (`0x80038518`) when nonzero (stop), else re-pick with
  `BtlSlotNearestFind(mode)` (`0x80039838`), mode 0 when `g_BtlActingStatus & 0x40`, else 1 for a
  party attacker (slot < 6), 2 for an enemy.
* **`BtlSlotNearestFind(mode)` (`0x80039838`)** — the NEAREST active slot by
  Manhattan distance between records' `+0x06` and `+0x0A` (x and y): mode 1
  scans 6..13, 2 scans 0..4, else 0..13; skips slot 5 and the attacker; best
  starts at `0xFFF` and is replaced only on a strict signed-16 `<`, so ties
  keep the lowest slot. No RNG. The C sets no result when nothing qualifies
  (callers check `BtlEndCheck` (`0x80038518`) first).
* **`BtlAttackWalkRun` (`0x80031244`)** — the reach check, every swing.
  Party: `worn_item_kind` (`+0x20`, the kind of the first worn item) 3 or 4
  always reaches, 5 does for character id 2; otherwise a holder of status
  `0x20` only MEASURES (walk block `+0x0C = 0`) and reaches only an adjacent
  target (`BtlWalkRun() == 2` (`0x8002BB5C`)), anyone else `BtlWalkRun() != 0`
  (`0x8002BB5C`). Enemy: a switch on `attack_show` (`+0x17`) − 0x98 — 0x98, 0x9A,
  0x9B, 0xA4, 0xA5, 0xAC, 0xAF, 0xB1 always reach, 0x9D and 0xA7 take
  `BtlTargetReachCheck` (`0x800379AC`), everything else (and any `+0x17`
  outside 0x98..0xB1) `BtlWalkRun` (`0x8002BB5C`). Enemy `+0x17` is also what
  `BtlAttackHitShow` (`0x800314B8`) and `BtlAttackMissShow` (`0x80031378`) hand
  `BtlActingShowRun` (`0x8003A158`) (the attack's presentation) when it is not 0.
  No RNG but the frame waits (`BtlWalkPathFollow` (`0x8002CA78`) / `BtlWalkDistanceFill` (`0x8002D254`) →
  `BtlFrameStep` (`0x80038D2C`)). See §"The approach walk".
* **`BtlAttackOutcomeClear` (`0x80031878`)** clears `g_BtlSlotStrongMarks` (crit),
  `g_BtlSlotAmountKinds` (reaction), `g_BtlSlotFallMarks` (KO flag) and `g_BtlSlotAmounts` for all
  14 slots, every swing.

### The approach walk — `BtlWalkRun` (`0x8002BB5C`) and the arena grid

Every walk, reach check and hit check in battle runs over one grid,
rebuilt for each measurement (`btl_walk.c`, `btl_path.c`). The scratch
globals are tabled in §"The walk planner's scratch"; this section is the
whole flow.

**The grid** (`g_BtlWalkGrid`, which holds the address `g_LoadBuffer`): 40
columns × 12 rows of 8 px, row 0 at y = `0x58`; a cell holds `0xFF` (not
reached), `0xFE` (blocked) or a distance in tiles. Coordinates come from the
record's anchor: column `x / 8`, row `(y − 0x58) / 8` (C division, toward
zero), y being `+0x0A`.

**`BtlWalkSet` (`0x8002BAA4`)** — the walk block `g_BtlWalkBlock`: `+0x00` / `+0x04`
the attacker's / target's records, `+0x08` the attacker's slot,
`+0x09..+0x0C` = 1 (`slots_block`: the other slots block cells and the fill
makes `reach` passes; `walker_width_counts`; `target_width_counts`; `walk`),
`+0x0D` the walk anim (party `g_BtlPartyMoveAnimations[slot]`, enemy 1),
`+0x0E` = `walk_speed` (`CharStats +0x18`), `+0x0F` = `walk_reach` (`CharStats
+0x13`: **the most cells a walk may cover**), `+0x10` a footstep sound
(party `0x2A`, enemy `+0x1E`).

**The footprint is the record's `+0x3C`**, which `BtlRecordInit` (`0x8002B770`)
sets at battle entry through **`BtlRecordSizeSet(record, 0)` (`0x800381D8`)**:
over the pieces of the pose of animation 0's first step, width =
|min(0, xo)| + max(0, xo + w) and height likewise from yo / h (piece `+0x08`
/ `+0x02` / `+0x0A` / `+0x04`, every piece whatever its kind) → `+0x3C` /
`+0x3E`. Measured in a running battle: Alex 24×23, Luna 24×24, Ramus 25×23,
companion 16×32, Slime 32×16. In tiles: `+0x3C >> 3`, rounded up to even,
centred on the anchor column — and ONE row tall.

**`BtlWalkRun` (`0x8002BB5C`)** — the decision:

```c
if (walker_slot < 5 && g_BtlActingStatus & 0x10) reach >>= 1;   /* in place: a   */
if (reach > 40) reach = 40;                              /* 2nd call halves again */
BtlWalkCellRound(&a->x); BtlWalkCellRound(&a->y);   /* snap to 8 px, rem 4 rounds up; depth = y */
g_BtlWalkerColumn = col(a), g_BtlWalkerRow = row(a);
g_BtlWalkTargetColumn = col(t), g_BtlWalkTargetRow = row(t);
if (same cell) return 1;
g_BtlWalkerWidth = walker_width_counts ? tiles(a) : 1;
    g_BtlWalkerColumn -= g_BtlWalkerWidth / 2 (clamped at 0, the width shrinking);
if (target_width_counts) {
    g_BtlWalkWidths[0] = tiles(t); g_BtlWalkTargetColumn -= g_BtlWalkWidths[0] / 2 (clamped, kept even);
    if (BtlWalkBesideCheck()) { BtlWalkFacingSet(); return 2; }   /* adjacent */
} else { g_BtlWalkWidths[0] = 1; if (walker_width_counts) g_BtlWalkTargetColumn -= g_BtlWalkerWidth / 2; }
BtlWalkGridInit(); BtlWalkDistanceFill(); BtlWalkGoalFind();
if (g_BtlWalkGoalDistance <= reach) {      /* within reach */
    if (!walk) return 1;  if (g_BtlWalkGoalDistance == 0) return 0;
    if (BtlWalkPathMake()) BtlWalkPathFollow(); return 1;
}
if (!walk) return 0;                        /* out of reach: walk as close as it gets */
BtlWalkGoalNearestFind(); if (it is the walker's own cell) return 0;
if (BtlWalkPathMake()) BtlWalkPathFollow(); return 0;
```

* **`BtlWalkBesideCheck` (`0x8002C9AC`)** — adjacent: target row within ±1
  of the walker's, and the column spans meeting (target column `<=` walker
  column + walker width when the target is to the right, walker column `<=`
  target column + target width otherwise), all signed 16-bit.
  **`BtlWalkFacingSet` (`0x8002CA34`)** faces it: record `+0x0C` = `0x20` when the walker's
  x is less than the target's, else `0x2020` (bit `0x2000` = turned toward
  smaller x, facing left; a battle's party records were measured holding
  `0x2020`), then one battle frame.
* **`BtlWalkGridInit` (`0x8002BEC8`)** — the grid: all `0xFF`, the path lists
  cleared; with `slots_block`, every slot 0..13 with an id except the walker,
  and except a DEAD ENEMY (a slot ≥ 6 neither active nor holding a status),
  and except the target unless its width counts, is marked by
  **`BtlWalkGridSlotBlock` (`0x8002C190`)**: `+0x3C / 8` tiles (even), centred,
  of its one row, clipped at the edges — so KO'd party members stay obstacles
  and fallen enemies do not. Then, unless the walker is slot 5, rows 0-2 are
  blocked, and enemy set `0x1C` (`g_BtlEnemySet`) also blocks the party off
  columns 0-21 and slot 6 off columns 22-39, rows 3-11.
* **`BtlWalkDistanceFill` (`0x8002D254`)** — the distance field: the walker's
  cell (its LEFT column when wide) = 0, then `reach` passes (1 without
  `slots_block`, never more than 40), each sweeping `reach` square rings
  outward — top row left→right, right column down, bottom row right→left,
  left column up — every cell that **`BtlWalkCellBlockedCheck` (`0x8002DAFC`)**
  passes (the cell, or for a wide walker any of the `g_BtlWalkerWidth` cells
  from it, is not `0xFE`) set by **`BtlWalkCellDistanceGet` (`0x8002D89C`)**:
  m = the lowest of its 8 neighbours; the cell keeps its own value if that is
  below m, else becomes m + 1 (`0xFF` when m is `0xFE` or `0xFF`). A wide
  walker's columns run 1 .. 39 − width. A battle frame after every 65 rings.
* **`BtlWalkGoalFind` (`0x8002C304`)** — the goal: `g_BtlWalkGoalDistance` = reach + 1;
  a wide target is approached from any cell where the walker would touch it —
  **`BtlWalkNeighbourLowestFind` (`0x8002CEE0`)** around the span from target
  column + 1 − walker width to target column + target width − 1 on the
  target's row (a start left of column 0 is moved to 0 and the span
  shortened): the cell right of the span, the cell left of it, above and
  below its first cell, then the rows above and below one cell wider each
  side, strict `<` (the first found wins). A one-tile target: its own cell if
  within reach, else the same scan around it. When no cell is lower, the C
  leaves the goal column and row holding whatever its two unpassed
  parameters held — harmless, since the distance stays above the reach.
* **`BtlWalkGoalNearestFind` (`0x8002C3E8`)** — out of reach: Manhattan rings around the
  target's span, growing by one: the left and right points on its row,
  the rows above and below over the span, then four diagonals; the lowest
  value (strict `<`, from reach + 1) — the first ring holding a cell
  within reach ends the search (at most 480 rings).
* **`BtlWalkPathMake` (`0x8002CD04`)** — the path: refused when the goal's
  row is 13 or more, its column 41 or more, or its cell beyond the reach;
  from it, `BtlWalkNeighbourLowestFind` (`0x8002CEE0`) with a 1-cell span
  repeatedly takes the lowest neighbour down to 0, recording each cell (a
  wide walker's by its middle column, goal column + `g_BtlWalkerWidth / 2`)
  into `g_BtlWalkPathColumns` / `g_BtlWalkPathRows`.
* **`BtlWalkPathFollow` (`0x8002CA78`)** — the walk, from the cell next to the
  walker to the goal: the footstep sound (unless `0xFF`) and the walk anim,
  then a tile every 1 / 2 / 4 / 8 frames for walk speed 1 / 2 / 3 / other
  (8 / frames px a frame on each axis, diagonals both at once), facing
  (`+0x0C`) by the x direction of each tile; then sound 0, and the walker's
  `g_BtlSlotRestAnimations` pose (enemy id 0x73 first runs its animation to
  the end). The anchor becomes `(cell x · 8, cell y · 8 + 0x58)`.
  **Nobody walks back**: battle actors stay where their last walk left
  them, and `BtlSlotNearestFind` (`0x80039838`)'s nearest-target picks measure from there.

**The hit check `BtlTargetReachCheck` (`0x800379AC`)**: `BtlWalkSet` (`0x8002BAA4`), then `BtlWalkRun` (`0x8002BB5C`)
with `+0x0C = 0` — a target within the caster's walk reach (or adjacent)
is hit from where the caster stands; otherwise `+0x0C = 1` and the
caster walks — a PARTY caster TWICE (the C calls `BtlWalkRun` twice running:
the second walk starts where the first ended, on a reach halved again under
status 0x10), an enemy once (some ids play animations of their own before
and after it) — and the effect does nothing. So a special's or spell's
RANGE is its caster's `walk_reach`. `BtlTargetStatus8TryRun` (`0x80033440`)
(effect 36) measures enemy id 0x10 with neither width counting (`+0x0A..+0x0C`
cleared) instead.

### Retreat, the magic arm and the fallbacks

* **`BtlActionWalkCornerSet(mode)` (`0x80039B70`)** — a corner to
  retreat to, kind 5's destination TILE: `g_BtlActionNumber` = column,
  `g_BtlTargetSlot` = row, `g_BtlActionKind` = 5. It counts the active slots other
  than the walker — 0..4 for mode 1 (an enemy fleeing the party), 6..13
  for mode 2, all 14 otherwise — per quadrant (x < `0xA0`, y < `0x88`).
  With exactly one, the corner across from it; with more, by the walker's
  quadrant through **`BtlQuarter0CornerGet` (`0x80039E84`) / `BtlQuarter2CornerGet` (`0x80039EC8`) / `BtlQuarter1CornerGet` (`0x80039F10`)
  / `BtlQuarter3CornerGet` (`0x80039F54`)**, each picking the side, then the
  quarter, with fewer of the counted slots. **Quirk**: the walker's row half
  is read from record `i` — the loop counter as the loop left it, i.e.
  record 5 for mode 1 and record **14** otherwise — not from the walker.
  Record 14 (`g_BtlRecords + 0x380`) is not a slot's record:
  `BtlWalkEdgeRun` (`0x80036654`), the walk to the screen's edge that
  `BtlTargetFrontMark` and nine show handlers use, makes it the place to walk
  to (`+0x386` / `+0x38A`), and `BtlShowA2Run` (`0x8003C308`) and
  `BtlShowABRun` (`0x8003DAEC`) copy an actor's record into it. In a battle
  measured before any of these had run its y was 0 ("top"). The column is
  half the walker's width in tiles (low byte of `+0x3C` / 8, rounded up,
  then even) from the chosen edge; the row `0x0E` (y `0x70`) or `0x16` (y `0xB0`).
* **`BtlActionWalkRun` (`0x8002E5D4`)** walks there: `BtlWalkSet` (`0x8002BAA4`), the target
  replaced by a stack record at (column · 8, row · 8), 8 wide and 8 high,
  `+0x0B` cleared; `BtlWalkRun` (`0x8002BB5C`). (A wide walker's columns are
  filled only from 1 to 39 − width, so it ends a column or two short of the
  edge.)
  **`BtlFacingNearestSet` (`0x8002E2FC`)** then faces the nearest foe:
  `BtlSlotNearestFind(0)` (`0x80039838`) when
  `g_BtlActingStatus & 0x40`, `(1)` when the acting slot `g_BtlActingSlot` is below
  5, `(2)` otherwise; the acting record's `+0x0C` = `0x20` when its x
  (`+0x06`, s16) is below the picked record's, else `0x2020`. It writes
  nothing else. A party member then takes pose 0x13 —
  **`BtlActingRestAnimationSet` (`0x80037890`)** tests the CURRENT kind `g_BtlActionKind` == 5, which
  survives `BtlActionRun` (`0x8002E088`) zeroing `g_BtlSlotPlannedKinds` — the defend stance.
* **`BtlActionTargetAdjust` (`0x8002ED34`), kind 5**: an enemy →
  `BtlActionWalkCornerSet(1)` (`0x80039B70`); a party member with target
  `0xFF` → `BtlActionWalkCornerSet(2)` (`0x80039B70`), else the tile already
  planned. The menu's kind 5 (`BtlMenuKindStep` (`0x80045EDC`), its third
  item) leads to `BtlMenuWalkStep` (`0x800483D4`), where the pad moves a copy
  of the member and the tile it is left on becomes the planned number and
  target. The party's own choice (`BtlPartyTurnActionSet` (`0x8004F734`))
  plans kind 5 at number and target `0xFF`: **retreat to the corner away
  from the enemies, face them, brace**.
* **`BtlActionTargetAdjust` (`0x8002ED34`), kind 3**: its MP, status and
  target tests and `BtlTargetModeAdjust` (`0x8002EB78`) are in §"The action
  loop". A re-pick by modes 2..7 is `BtlPartyTargetPick` (`0x8003995C`), over
  the PARTY. Spell rows: `+0x01` = `0x91` (Alex's damage rows: re-pick = the
  nearest enemy), `0x90` (whole-side rows: the caster), `0x55` (Healing Song:
  a member below max HP, else the fallback).
* **`BtlActionFallbackSet` (`0x8002F3A0`)**, party branch: under 1/8 of max HP while
  at least two active party members (itself counted) are free of the
  statuses in `0x3C` → `BtlActionWalkCornerSet(2)` (`0x80039B70`), a retreat;
  otherwise kind 2 at `BtlSlotNearestFind(1)` (`0x80039838`).
* **`BtlEnemyActionFallbackSet` (`0x80036A18`)**'s six ids (`0x10 0x18 0x1F 0x2E 0x44 0x55`) →
  `BtlActionWalkCornerSet(1)` (`0x80039B70`): they retreat from the party.

Effect 39 (`BtlTargetStatus10TryRun` (`0x800336DC`)) rolls (`BtlStatusTakeCheck`)
only against a party target and stores `0x10` outright (no guard, no
resistance, no counter). Effect 68 (`BtlTargetHitStatus10Run` (`0x800343F8`))
gives the same status after a hit that lands and does not fell, through
`BtlTargetStatusSet(0x10)` (`0x80034544`), which does test resistance
(`BtlTargetTraitCheck(0x800, 0x25, 0x26, 0)`) and zeroes the four counters.
Effect 51 (`BtlTargetStatus80TryRun` (`0x80033A58`)) and the `0x80` on-hit
codes leave status `0x80`.

### The evasion check — `BtlHitTry` (`0x8002FF0C`)

In outline (`btl_hit.c`):

```c
int BtlHitTry(void) {
    t = g_BtlTargetSlot;
    if (CharStats[t].status & 0x3C) { g_BtlSlotAmountKinds[t] = 1; return 1; }   /* no draw */
    if (BtlTargetTraitCheck(0, 0x31, 0, 0) && rng_7f_draw() < 0x20)
        { g_BtlSlotAmountKinds[t] = 2; return 0; }             /* dodge; a failed dodge falls through */
    if (t >= 6 && g_BtlSlotIds[t] == 0x2F)
        { g_BtlSlotAmountKinds[t] = 2; return 0; }             /* no draw */
    if (rng_7f_draw() < 0x80) { g_BtlSlotAmountKinds[t] = 1; return 1; }   /* always: the draw is & 0x7F */
    g_BtlSlotAmountKinds[t] = 2; return 0;                     /* unreachable */
}
```

`rng_7f_draw` (`0x8003A0B4`) is `rng_draw() & 0x7F` (`0x8001C770`). The last roll
cannot fail, but **it is drawn**: every ordinary swing advances the RNG
once here, before the resolver's own draws. (The C writes that last draw
three times, for a party target, for slot 5 and for an enemy, all alike.)
A target with trait `0x31` that fails to dodge draws twice.

### The post-hit pass — `BtlHitSlotsUpdate` (`0x80030800`) + `BtlHitStatusUpdate` (`0x80030A04`)

`BtlHitSlotsUpdate` (`0x80030800`, `btl_hit.c`; called by `BtlActionAttackRun`
(`0x80031060`), `BtlAttackAllRun` (`0x800318DC`) and `BtlTargetHitRun`
(`0x80032958`)) walks the slot indices 0..count−1 — 14 slots, or 5 when
`g_BtlNumber >= 0xEB` — and acts on those `BtlSlotHitCheck` (`0x800307C0`)
calls landed (`g_BtlSlotAmountKinds` is 1, 3 or 4):

* **Instant KO.** A **party** attacker whose first trait (`+0x28`) is 6 /
  `0x0D` / `0x14`: per landed slot, `(u8)rng_7f_draw() < 0x20 / 0x40 / 0x80`
  sets the KO flag `g_BtlSlotFallMarks`. The draw is made only for landed
  slots (`&&` short-circuit); since the helper is `& 0x7F`, **threshold 0x80
  (trait `0x14`) always succeeds**. The active flag `g_BtlSlotActiveFlags` is
  NOT tested. All the flags are raised first and `BtlFallRun` (`0x800385B4`)
  runs ONCE, after the loop; the function returns there —
  **`BtlMarkedRestAnimationSet` (`0x80030DA0`) (poses) is not called on this
  path**, and no status roll is made.
* **Anyone else:** `BtlFallRun` (`0x800385B4`) FIRST — so a slot the hit just
  killed is no longer active — then, per landed slot still active,
  `BtlHitStatusUpdate` (`0x80030A04`). The loop variable IS the global target
  index `g_BtlTargetSlot` (saved before, restored after) — that is how
  `BtlHitStatusUpdate` and its appliers know the slot. Then
  `BtlMarkedRestAnimationSet()` (`0x80030DA0`) puts every active slot with a
  number kind on the pose of its status, or on its rest pose
  `g_BtlSlotRestAnimations`.
* It sets no return value (declared `s32`; `BtlAttackAllRun` (`0x800318DC`)
  ends `return BtlHitSlotsUpdate();`, passing on whatever the last callee
  left).

`BtlHitStatusUpdate` (`0x80030A04`), in outline:

```c
void BtlHitStatusUpdate(void) {
    c = (g_BtlActingSlot < 5) ? CharStats[a].traits[0] : CharStats[a].trait;   /* +0x28 / +0x1B */
    chance = 0;
    if ((u32)(c - 1)  < 5 || c == 0x29) chance = 0x20;
    if ((u32)(c - 8)  < 5 || c == 0x1A) chance = 0x40;
    if ((u32)(c - 15) < 5 || c == 0x19) chance = 0x80;
    if (rng_7f_draw() < chance && (u32)(c - 1) < 0x29)   /* the draw is UNCONDITIONAL */
        switch (c - 1) { ... }                           /* a status applier */
    BtlTargetStatus8Clear();                             /* skipped after the 4, 8 and 0x30 appliers */
}
```

The C's switch over `c − 1` (41 cases): codes 1-5, 8-12 and 15-19 each map,
in order, to `BtlTargetStatus4Set` (`0x8002F74C`), `BtlTargetStatus8Set`
(`0x8002F958`), `BtlTargetStatus30Set` (`0x8002FAFC`), `BtlTargetStatus40Set`
(`0x8002FC08`), `BtlTargetStatus80Set` (`0x8002FC94`); `0x19`, `0x1A` and `0x29`
map to `BtlTargetStatus1Set` (`0x8002F848`); the rest do nothing. After the
4, 8 and 0x30 appliers the function returns at once; every other path ends
in `BtlTargetStatus8Clear` (`0x80030C28`), the wake on hit (§"Wake on hit").
The appliers (`btl_status.c`) guard on the target's `+0x15`, then ask
`BtlTargetTraitCheck` (`0x80037648`) as a resistance test, then STORE the whole
status byte:

| applier | guard (`+0x15`) | `BtlTargetTraitCheck` (`0x80037648`) args | stores | also |
|---------|-----------------|----------------------|--------|------|
| `BtlTargetStatus4Set` (`0x8002F74C`) | `& 0x34 == 0` | `0x200, 0x21, 0x26, 0` | 4 | `g_BtlSlotStatus4Rounds[t] = 0`; party: `+0x1E &= ~4` |
| `BtlTargetStatus1Set` (`0x8002F848`) | party only, `& 0x35 == 0` | `0x100, 0x21, 0x26, 0` | 1 | `g_BtlPartyStatus1Shares[t] = 10`; `+0x1E &= ~4` |
| `BtlTargetStatus8Set` (`0x8002F958`) | `& 0x35 == 0` | `0x400, 0x21, 0x26, 0` | 8 | `g_BtlSlotStatus8Rounds[t] = 0`, `g_BtlSlotPlannedKinds[t] = 0xB` |
| `BtlTargetStatus30Set` (`0x8002FAFC`) | none | `0x800, 0x25, 0x26, 0` | `0x30` | clears four counters AND the active flag; party: `+0x1E &= ~4` |
| `BtlTargetStatus40Set` (`0x8002FC08`) | `& 0x3D == 0` | `0x1000, 0x25, 0x26, 0` | `0x40` | `g_BtlSlotStatus40Rounds[t] = 0` |
| `BtlTargetStatus80Set` (`0x8002FC94`) | `== 0` | `0x2000, 0x25, 0x26, 0` | `0x80` | `g_BtlSlotStatus80Rounds[t] = 0` |

(`BtlTargetStatus10Set` (`0x8002FA00`) — `0x10`, same shape as `BtlTargetStatus30Set` (`0x8002FAFC`) without the
active-flag clear — is not in this switch.) Status `0x40` is the one
`BtlEnemyActionAdjust` (`0x80029510`) and `BtlPartyActionAdjust` (`0x80029348`)
turn into random behaviour (§"Confusion — status 0x40").

**So an ordinary hit on a surviving target draws once more, after the
resolver, whatever the attacker's code** — code 0 has chance 0 but the
roll is still made. A killing hit does not (`BtlFallRun` ran first and
the slot is no longer active).

### What this does to the RNG stream

Per plain swing at an ordinary target: evasion 1, the resolver's variance
roll 1, the strong-hit roll 1-2 (`BtlHitStrongTry` (`0x80038310`): kind 2
only, and only between the two sides), and the post-hit roll 1 if the
target survived. That much is the combat code's own. It is **not** the whole
stream of a fight: `BtlSlot5RoamStep` (`0x80038EE8`), which the battle frame
`BtlFrameStep` (`0x80038D2C`) calls when slot 5 holds id 0xC — every frame of
every swing and of every presentation wait — draws `rng_draw()` (`0x8001C770`)
for slot 5's idle animation, its walk across and its walk up and down
(twelve call sites in the C, all behind `g_BtlSlot5Roaming`), so the real
stream interleaves with frame timing.

## The element system — `BtlTargetTraitCheck` (`0x80037648`)

`BtlTargetTraitCheck` (`0x80037648`, `btl_slot.c`) is the trait and element
test every damage path consults. It writes nothing and calls nothing.

```c
/* mask = element or status bits (u16); first/second/third = up to three traits (u8) */
int BtlTargetTraitCheck(u16 mask, u8 first, u8 second, u8 third)
{
    int t = g_BtlTargetSlot;                       /* the TARGET index */
    BtlStats *r = &CharStats[t];

    if (t < 5) {                              /* --- PARTY target --- */
        /* +0x28..+0x2D is the six-byte traits[] array. first is scanned with
           NO zero test; second and third end the search when zero. */
        for (i = 0; i < 6; i++) if (r->traits[i] == first) return 1;
        if (second == 0) return 0;
        for (i = 0; i < 6; i++) if (r->traits[i] == second) return 1;
        if (third == 0) return 0;
        for (i = 0; i < 6; i++) if (r->traits[i] == third) return 1;
        return 0;                             /* mask is IGNORED */
    }

    /* --- ENEMY target (index >= 5) --- */
    for (each of first, second, third that is nonzero)
        if (r->trait == arg) return 1;        /* +0x1B, one trait byte */
    if (mask == 0) return 0;
    if (r->resist_mask & mask) return 1;      /* +0x2A, RESIST */
    if (r->weak_mask & mask) return 2;        /* +0x28, WEAK   */
    return 0;
}
```

**The party side's unguarded first trait** means a call with a first trait
of 0 against a party member who has ANY empty trait byte would return 1
(resist). No call passes one: every caller's first trait is a nonzero
constant, and the one call whose first trait is not a constant, the magic
element path's, passes `0xC0` or `0xF0` (§"The magic element path").

### The record fields it reads

The stats record (`BtlStats`, `include/btl.h`) is a **union by role**
(`side.party` / `side.enemy`): the same offsets are a trait array for party
members and element masks for enemies. Nothing here is a single layout for
all 14 slots.

| field | party slot (index < 5) | enemy slot (index >= 5) |
|-------|--------------------------|---------------------------|
| `+0x1B` | (not read by this function) | `trait`, a **single trait byte** |
| `+0x28` | `traits[0]` of the six bytes `+0x28..+0x2D` (byte `+0x0D` of each worn item's row) | `weak_mask`, a **u16 element-WEAKNESS mask** |
| `+0x2A` | `traits[2]`, `traits[3]` | `resist_mask`, a **u16 RESIST mask** (bits 1 to 0x40 elements, 0x100 to 0x2000 statuses) |

Both masks are read as 16-bit words, by this function and by
`BtlHitAmountSet` (`0x80030070`). The Slimes' `weak_mask` `0x00F0` is weak to
elements 5, 6 and 7; their `resist_mask` is `0x2100`, whose low byte is
`0x00`.

### The physical element path in `BtlHitAmountSet` (`0x80030070`)

For `g_BtlActionKind == 2`, in outline:

```c
if (g_BtlActingSlot < 6 && g_BtlTargetSlot >= 6) {   /* PARTY attacker vs ENEMY target only */
    switch (attacker->traits[0]) {         /* the ATTACKER's first trait */
        case 0x2A: m = 0x01; case 0x2B: m = 0x02; case 0x2C: m = 0x04;
        case 0x2D: m = 0x08; case 0x2E: m = 0x03; case 0x2F: m = 0x0C;
        default:   m = 0;
    }
    if (m & target->weak_mask)   { g_BtlSlotAmountKinds[t] = 3; dmg <<= 1; }  /* WEAK   */
    if (m & target->resist_mask) { g_BtlSlotAmountKinds[t] = 4; dmg >>= 1; }  /* RESIST */
}
```

Three things follow. **(1)** Physical element is gated to party-attacks-enemy
— enemy physicals never carry an element. **(2)** The mask comes from the
*attacker's* first trait (`0x2A..0x2F`, from its first worn item); traits
`0x2E` and `0x2F` carry two elements each. **(3)** Weakness and resist are
**separate `if`s**, not a switch: a mask matching both doubles and then
halves.

### The magic element path (`g_BtlActionKind != 2`)

`g_BtlActionBitNumber` (element id, 1-7) selects the mask **and** up to two
traits, then calls the test:

| element id | mask | trait `second` | trait `third` |
|---|---|---|---|
| 1 | `0x01` | `0x2A` | `0x2E` |
| 2 | `0x02` | `0x2B` | `0x2E` |
| 3 | `0x04` | `0x2C` | `0x2F` |
| 4 | `0x08` | `0x2D` | `0x30` |
| 5 | `0x10` | *residue* | *residue* |
| 6 | `0x20` | *residue* | *residue* |
| 7 | `0x40` | *residue* | *residue* |

```c
if (mask) switch (BtlTargetTraitCheck(mask, e1, e2, 0)) {
    case 1: dmg >>= 1; g_BtlSlotAmountKinds[t] = 4; break;   /* RESIST */
    case 2: dmg <<= 1; g_BtlSlotAmountKinds[t] = 3; break;   /* WEAK   */
}
```

The two traits are passed as the test's first and second (`e1`, `e2`; the
third is 0). The halving or doubling comes before kind 3's percentage
pass, so a halved spell is not always exactly half.

**Elements 5-7 pass register residue, not "no trait".** Their arms of the
switch set only the mask: the C leaves `e1` and `e2` unset there, and the
compiled function keeps them in the callee-saved `$s2` and `$s3`, which it
writes only on the element 1..4 arms. So the test receives the low bytes of
whatever the nearest frame above that uses those registers left in them.
For a kind-3 cast the chain is
`MapBtlRun → BtlRun → BtlRoundTurnsRun → BtlTurnOrderRun →
BtlActionRun → BtlActionKindRun → BtlActionSpellRun → BtlActionRowRun → handler →
BtlHitAmountSet` (`0x8007AE58`, `0x80027434`, `0x8002DB8C`, `0x8002DC30`, `0x8002E088`, `0x8002E12C`, `0x8002E718`, `0x80031ABC`, `0x80030070`), and the saved-register use of each frame (its prologue in the compiled code) is:

| frame | saves | what it keeps there |
|---|---|---|
| `MapBtlRun` (`0x8007AE58`) (battle wrapper) | s0-s6 | **`$s3` = its 5th argument** (`view_follow`) |
| `BtlRun` (`0x80027434`) (battle loop) | s0-s2 | — |
| `BtlRoundTurnsRun` (`0x8002DB8C`) | none | — |
| `BtlTurnOrderRun` (`0x8002DC30`) (action loop) | s0-s2 | **`$s2` = `&g_BtlTurnOrder`** (`0x800E49C0`), constant over its loop |
| `BtlActionRun` (`0x8002E088`), `BtlActionKindRun` (`0x8002E12C`), `BtlActionRowRun` (`0x80031ABC`) | none | — |
| `BtlActionSpellRun` (`0x8002E718`) (the cast) | s0 | — |
| `BtlTargetHitRun` (`0x80032958`) (effect 17) | s0 | — |
| `BtlTargetHitStatusRun` (`0x80034418`) (effects 64, 66, 67, 68, over effect 17) | s0 | — |
| `BtlTargetRowsHpRun` (`0x80035238`) (effect 94) | none | — |
| `BtlMarkedHitRun` (`0x80033060`) (effects 20, 21, 94's worker) | s0, s1 | — |
| `BtlAreaHitRun` (`0x80032A50`) (effects 18, 19's worker) | s0-s2 | **`$s2` = `&g_BtlSlotAmountKinds`** (`0x800E48F0`) |

`BtlActionSeriesRun` (`0x8002DD88`), enemy 0x7C's script and a second way
into `BtlActionRun`, saves s0 and s1 only. An enemy special (kind 7), and an
enemy's kind-2 attack with a row, reach their handler through
`BtlActionKindRun` → `BtlActionRowRun`, which save nothing: the same pairs as
a cast. So under elements 5-7 the test is called as:

| reached through | first trait (from `$s2`) | second trait (from `$s3`) |
|---|---|---|
| effect 17 (and 64 / 66 / 67 / 68 over it), and 20 / 21 / 94 through `BtlMarkedHitRun` (`0x80033060`) | `0xC0` | arg 5 of `MapBtlRun` (`0x8007AE58`) |
| effects 18 / 19 through `BtlAreaHitRun` (`0x80032A50`) | `0xF0` | arg 5 of `MapBtlRun` (`0x8007AE58`) |

`MapBtlRun` (`0x8007AE58`)'s fifth argument is **1** for a touched map enemy
(`MapObjectTouchRun` (`0x8007ACD8`): `MapBtlRun(rec[0xB], 0, 0, 0, 1, 1, 1)`)
and `g_MoveChrViewFollow` (0 or 1) for a script battle
(`ScriptCommandMapBtlRun` (`0x8005DFF0`)). The magic tail
`BtlTargetHpAmountTake` (`0x80035FD8`) consults no trait under elements 5-7.

What the pairs do, by the game's data:

* No item (`+0x0D`) and no enemy template (`+0x1B`) holds `0xC0` or `0xF0`,
  so the first trait never matches; in particular it is not 0, so the
  unguarded party scan does not fire on an empty trait byte.
* **Trait 1** is carried by items 184, 186, 217 and 225 and by enemy
  template 35. The items lie past the item table (`ChrItemUseGet`
  (`0x80058088`) answers only for numbers 1..0xB6), so no party member wears
  one. The only holder in a battle is enemy template 35, the Fire Elemental,
  fielded by encounters 95 and 96: in a map-enemy battle it takes an
  element-5..7 hit (Alex's and Kyle's rows) as RESIST (halved, number kind 4).
* In a script battle the second trait is 0 or 1, and the two differ only on
  a holder of trait 1; no BATTLE command starts encounter 95 or 96, so the
  difference never shows.

**A measured case: the Slime battle's opening spell.** The fight recorded
amount 34, element 5, number kind 3 for spell number 1. Spell row 1 has
power 100, element 5 and MP cost 6; element 5's mask is `0x10`; the
Slime's `weak_mask` is `0x00F0`; `0x10 & 0x00F0 != 0` → the test answers 2 →
WEAK, number kind 3, the amount doubled — 17 × 2 = 34. Alex's MP went
10 → 4 in that fight, the row's cost of 6. Spell number 1 is Alex's first
spell.

### Plain attacks and `g_BtlAttackRows`

`BtlActionRowRun` (`0x80031ABC`)'s kind-2 arm sets `g_BtlActionNumber` from
the attacker's `+0x1C` and loads `g_BtlActionPower` (the percent bonus) from
byte `+0x02` of that row of `g_BtlAttackRows`. But a plain Attack does not
go there: `BtlActionKindRun` (`0x8002E12C`) sends kind 2 to `BtlActionRowRun`
only for an enemy slot whose `attack_number` (`+0x1C`) is not 0; a party
member's attack, and an enemy's with `attack_number` 0, go to
`BtlActionAttackRun` (`0x80031060`), which loads no row. `g_BtlActionPower`
then stays at the 0 `BtlTurnOrderRun` (`0x8002DC30`) set for the action. For
a party slot `+0x1C` is another field, `stat_0a_start` (`stat_0a` as
`BtlPartyStatsSet` set it). The measured values there (Alex 21, Luna 18,
Ramus 25) are those, not row numbers, and every measured ordinary attack
shows a bonus of 0 — Ramus at `stat_08` 36 against `stat_0a` 6 dealt 15-16,
which is `(30 + roll) / 2` (§"Combat math — the damage formula"). The rows of
`g_BtlAttackRows` are the enemies' kind-2 attack rows and kind-7 special
rows.
