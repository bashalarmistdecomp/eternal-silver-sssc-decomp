# Battle data tables

The fixed tables the battle reads from the main executable's data — the
spell rows, the item rows, the attack rows, the enemies' rows and the
table of row functions — and the per-slot stats they are copied into. None
of the tables is written: each is data of the executable at a fixed
address, the same in every state of the game, so it can be read from the
image with no emulator. This page says where each table is, how a number
reaches its row and what its bytes are; battle.md has the code that reads
them, field by field.

## Where the tables are

| table | address | rows | row of number n | read through |
|---|---|---|---|---|
| spell rows, `BtlSpellRow` | `g_BtlSpellRows` (`0x800AA734`) | 0x32 of 12 bytes | row n − 1 | `g_BtlSpellRowsByNumber` (`0x800AA728`), the 12 zero bytes before the table, + n × 12 (the battle); `g_ChrSpellRows` (`0x800B8B9C`) (the menus and the Chr functions) |
| item rows, `BtlItemRow` | `g_BtlItemTable` (`0x800A9A68`) | 0xB6 (182) of 0x12 bytes | row n − 1 | `g_BtlItemRows` (`0x800B863C`), a word that holds the table's address; `g_BtlItemRowsByNumber` (`0x800A9A56`), the 0x12 bytes before the table, + n × 0x12; `g_ChrItemRows` (`0x800B8B94`) |
| attack rows | `g_BtlAttackRows` (`0x800AC640`) | 0x7F (127) of 8 bytes | row n | `BtlActionRowRun` (`0x80031ABC`), `BtlActionTargetAdjust` (`0x8002ED34`), `BtlEnemyKind7Pick` (`0x8004CF48`) |
| enemies' rows, `BtlEnemyRow` | `g_BtlEnemyRows` (`0x800A8768`) | 0x26 bytes each | row id − 1 | `BtlEnemyStatsSet` (`0x8002A638`) alone |
| row functions | `scene_state_table` (`0x800B2EF8`) | 132 function addresses | entry n − 1 | `BtlActionRowRun` |

`func_80026AB0` (font_string.c) points `g_ChrSpellRows` and `g_ChrItemRows`
at the two tables at start-up. `ChrItemUseGet` (`0x80058088`) answers −2
for an item number of 0 or above 0xB6, which gives the item count. The
attack rows end exactly where the level-up table `g_ChrLevelGainTable`
(`0x800ACA38`) begins. For the enemies' rows the register measures 0x12EE
bytes to the next label; the C bounds an enemy id only by the byte it is
read from, so the row count is not read from the code.

## The spell rows — `BtlSpellRow`

| offset | field (btl.h) | what it is |
|---|---|---|
| `+0x00` | `level` | the level the spell is learned at (`ChrLevelUp` (`0x80028554`)) |
| `+0x01` | `target_modes` | low four bits the battle's target mode, high four bits the field menu's |
| `+0x02` | `function_number` | → `g_BtlActionFunctionNumber` (`0x800B8C18`): the row function, entry number − 1 of `scene_state_table` |
| `+0x03` | `use_place` | low four bits the entry of `battle_state_table` (`0x800B5B1C`) that `MenuUseRun` (`0x80076444`) calls for a use outside battle; high four bits the place among the character's eight spells that `ScriptCommandChrSpellSet` (`0x8005EE30`) writes |
| `+0x04` | `cost` | the MP the spell costs, halved and rounded up for a caster with trait 0x1B (in battle, `BtlActionSpellRun` (`0x8002E718`)) or with worn item 0x88 in place 5 or 6 (outside one) |
| `+0x05` | `power` | → `g_BtlActionPower` (`0x800B8C2C`), a value the row's function may replace |
| `+0x06` | `show_number` | → `g_BtlActionShowNumber` (`0x800B8C0F`), the show `BtlShowNumberRun` (`0x8007BB80`) plays |
| `+0x07` | `chr_id` | the character the spell belongs to (1 for id 0xB too) |
| `+0x08` | `bit_number` | → `g_BtlActionBitNumber` (`0x800B8C10`): 1 to 7 are the element bits 1, 2, 4 .. 0x40 (`BtlHitAmountSet` (`0x80030070`)), 9 to 14 the status bits 0x100 .. 0x2000 (`BtlStatusTakeCheck` (`0x80035378`)) |
| `+0x09` | `area_half_width` | → `g_BtlAreaHalfWidth` (`0x800B8C2E`), in cells of 8 pixels |
| `+0x0A` | `area_half_height` | → `g_BtlAreaHalfHeight` (`0x800B8C30`) |
| `+0x0B` | `blocked_by_status_20` | not 0: the spell cannot be cast while the caster's status has bit 0x20 |

What the rows hold, read from the image:

* Spells 1 to 48 have rows; 49 and 50 are all zero. The show number is not
  the spell number: spells 43 to 46 have shows 0x0D to 0x10, and spell 20
  has function 0 and show 0.
* The 48 rows name 30 different functions besides 0, the highest 94,
  under `BtlActionRowRun`'s test that the number is below 0x5F.
* The low four bits of `use_place` are not 0 for spells 13, 14, 16, 20, 33
  and 38 (1, 2, 1, 5, 1, 2): the spells with a use outside battle.
* `ChrLevelUp` learns, at each new level, the first row of the character's
  id (1 for 0xB) whose `level` is the new one, into the place its switch
  works out for ids 1 to 6 and 0xB. For every row of those ids that place is
  the high four bits of `use_place`. The rows of ids 8 and 7 (spells 39 to
  42, and 47 and 48) are all level 1, which a level-up never reaches; level-1
  rows are held from the start (resource 0xE7's records carry those), and
  id 7's two rows are id 10's from the start (battle.md §"Caster groups").

**The row does not hold the effect; it names a function.** Every action
kind (attack, spell, item, enemy special) goes through `BtlActionRowRun`,
which loads the row into one set of globals and calls the function its
number names. Healing Song is spell 33: function 3, `scene_state_table[2]`,
`BtlTargetHpShareAddRun` (`0x80031ED4`), and cost 4 (Luna's MP goes 40 → 36
when she casts it, measured in the running game). Its power, 50, is not the
amount healed: the function heals max(40% of the target's max HP, 40) +
`rng_mask_below(0xFFFF, max HP / 20)` (`0x8003A0D4`), held at max HP, and
never reads the power (battle.md §"The heal — `BtlTargetHpShareAddRun`
(`0x80031ED4`)").

## The item rows — `BtlItemRow`

| offset | field (btl.h) | what it is |
|---|---|---|
| `+0x00` | `price` | u16, what the item costs in a shop |
| `+0x02` | `sell_price` | u16, what giving it up adds to the money; 0: it cannot be given up |
| `+0x04` | `kind` | below 5 a worn item, in worn place kind + 1 (kind 4 has places 5 and 6); 5 and up a carried item (6: one that cannot be given up) |
| `+0x05` | `story_bit` | not 0: story bit + 0x2BC is set while the item is worn |
| `+0x06` | `target_modes` | low four bits the battle's target mode, high four bits the field menu's |
| `+0x07` | `use_picture` | low four bits the `battle_state_table` entry `MenuUseRun` calls; high four bits the picture `MenuUsePictureRun` (`0x8007690C`) plays |
| `+0x08` | `function_number` | the row function of the item used as an action; 0 for an item a battle cannot use, which `BtlMenuActionCheck` (`0x800493F8`) refuses |
| `+0x09` | `show_number` | → `g_BtlActionShowNumber` |
| `+0x0A` | `bonus_1_power` | a worn item's first bonus; for an item of number 0x9C or more, the power of its use (`g_BtlActionPower`) |
| `+0x0B` | `bonus_2` | a worn item's second bonus; the first worn item's is also the slot's `least_amount` |
| `+0x0C` | `bonus_stats` | the stats the two bonuses go to, selectors 1 (`stat_08`) to 8 (`walk_reach`): low four bits bonus 1's, high four bits bonus 2's |
| `+0x0D` | `trait` | copied into the party slot's `side.party.traits` |
| `+0x0E` | `attack_show` | the first worn item's goes to the slot's `attack_show`, which nothing reads for a party slot |
| `+0x0F` | `worn_item_kind` | the kind of a weapon (0 to 5; 0 in every other row) |
| `+0x10` | `wearers` | u16: bit n − 1 for each character n who may wear it |

Items 1 to 4 are worn items of kind 0 adding 6, 11, 16 and 21 to
`stat_08` (selector 1); their prices are 100, 200, 360 and 600 and their
sell prices half that. battle.md §"Worn items — what they add, and how they
come off" has what each kind holds and the worn-item code; §"Item use and
the menu-time auto-battle" has the use of an item as an action.

## The attack rows — `g_BtlAttackRows`

The rows of an enemy's attack (kind 2, by its stats' `attack_number`,
`+0x1C`, when that is not 0) and of an enemy's special (kind 7, by the
action's number; for id 0x4F by its `kind7_number`, `+0x1D`). Row k's name
is string `k | 0x300` (`BtlActionStringDisp` (`0x8004987C`)).

| byte | what it is |
|---|---|
| 0 | the target mode `BtlActionTargetAdjust` reads: 7 — when no party slot 0 to 4 is active with a status of 0, `BtlEnemyActionFallbackSet` (`0x80036A18`); otherwise the target is kept when it is below 14, active and of status 0, and becomes `BtlTargetByModeGet(7)` (`0x800397B4`) when not; 2 — the target is kept when it is below 14 and active, else `BtlTargetByModeGet(2)`; any other value, nothing. `BtlEnemyKind7Pick` plans kind 2 in place of 7 when the row of the enemy's `kind7_number` has mode 7 and the chosen target's status is exactly 4, 8 or 0x30 |
| 1 | → `g_BtlActionFunctionNumber` |
| 2 | → `g_BtlActionPower` |
| 3 | → `g_BtlAreaHalfWidth`; not 0 makes the hit functions run `BtlTargetReachCheck` (`0x800379AC`) |
| 4 | → `g_BtlAreaHalfHeight`; not 0 makes `BtlTargetHitRun` (`0x80032958`) run the evasion test |
| 5 | → `g_BtlActionBitNumber` |
| 6 | → `g_BtlActionShowNumber` |
| 7 | 0 in every row; nothing reads it |

Rows 0, 2, 23 and 105 are all zero. Row 0 is not a value the code tests
for: a kind-7 action of number 0 takes function 0, and `BtlActionRowRun`
calls the word before the function table, `0x800B2EF4`, which is 0 in the
image. battle.md §"Enemy specials — kind 7 and the numbered attack" and
§"Plain attacks and `g_BtlAttackRows`" have the rows' use.

## The enemies' rows — `BtlEnemyRow`

`BtlEnemyStatsSet` copies row id − 1 field by field into the stats of each
enemy slot 6 to 13 that has an id; the fields of `BtlEnemyRow` (btl.h) are
named by the stats' fields they go to. Row 0 is the Slime's. It holds the
action pick, the level, hp (stored as hp and hp_max), the five stats
`stat_08` to `stat_10`, `walk_reach`, `least_amount`, `attack_count`, the
trait, the attack and kind-7 numbers, the walk sound, walk speed, attack
show, attack sound and action sound, the experience and money, two items a
won battle may give with their rates, and the weak and resist masks. Byte
`+0x19` is not copied and nothing reads it. MP and the status byte are not
in the row: an enemy's stay as the battle's clear left them unless the
battle's set-up gives a status. For the special encounters (numbers 0xEB
and up) `BtlSpecialInit` (`0x8002A7FC`) then raises the enemies' level and
stats with `BtlEnemyLevelSet` (`0x8002ADF0`). battle.md §"The enemies' rows
— `g_BtlEnemyRows` (`0x800A8768`), and `BtlEnemyStatsSet`" has the byte
map.

## The stats — `BtlStats`

`CharStats` (the word at `0x800B8C20`) points at 14 records of 0x3C bytes
at `0x801D2B00`, one per slot: 0 to 4 the party, 5 the slot that stands
apart (Nall), 6 to 13 the enemies. At a battle's start `BtlPartyStatsSet`
(`0x8002A138`) fills slots 0 to 4 from the character records and
`BtlEnemyStatsSet` fills 6 to 13 from the enemies' rows; neither writes slot
5. They are battle state, not data of the executable.

| offset | field (btl.h) | what it is |
|---|---|---|
| `+0x00` / `+0x02` | `hp` / `hp_max` | u16 |
| `+0x04` / `+0x06` | `mp` / `mp_max` | u16; 0 for an enemy |
| `+0x08` | `stat_08` | u16: what a hit's amount starts from (`BtlHitAmountSet`) |
| `+0x0A` | `stat_0a` | u16: taken off a hit's amount |
| `+0x0C` | `stat_0c` | u16: the key the turn order is sorted by, plus `rng_mask_below(0x3FF, top / 10)` (`BtlTurnOrderMake` (`0x8002F4B0`)) |
| `+0x0E` | `stat_0e` | u16: what a stat-0E amount starts from (`BtlStat0EHitAmountSet` (`0x80035EF8`)), and the status chance on both sides (`BtlStatusTakeCheck`) |
| `+0x10` | `stat_10` | u16: taken off a stat-0E amount |
| `+0x12` | `attack_count` | how many times an attack hits (`BtlActionAttackRun` (`0x80031060`)); for kind 3, `BtlHitAmountSet` applies the spell's percentage that many times less one |
| `+0x13` | `walk_reach` | the most cells a walk may cover: `BtlWalkSet` (`0x8002BAA4`) copies it to `g_BtlWalkBlockReach` (`0x800E4B1F`) |
| `+0x14` | `level` | u8 |
| `+0x15` | `status` | the status bits |
| `+0x16` | `least_amount` | the least amount the slot's hit takes |
| `+0x17` | `attack_show` | an enemy's attack show (`BtlAttackHitShow` (`0x800314B8`) → `BtlActingShowRun` (`0x8003A158`)); `BtlAttackWalkRun` (`0x80031244`) decides by it whether the slot walks |
| `+0x18` | `walk_speed` | `BtlWalkPathFollow` (`0x8002CA78`): 1, 2, 3 or other → a cell every 1, 2, 4 or 8 frames |
| `+0x19` | `attack_sound` | the attack's sound |
| `+0x1A..+0x2D` | `side.party` | `stat_08_start`, `stat_0a_start`, `flags` (`+0x1E`), `stat_1f`, `worn_item_kind` (`+0x20`), `pad_21`, `worn_items[6]` (`+0x22..+0x27`), `traits[6]` (`+0x28..+0x2D`) |
| `+0x1A..+0x2D` | `side.enemy` | `action_pick` (`+0x1A`: `BtlEnemyActionPick` (`0x8004CAB8`) switches on it; `BtlActionTargetAdjust` sends 2, 7, 0xC, 0x11 and 0x1A to the nearest party slot), `trait` (`+0x1B`), `attack_number` (`+0x1C`), `kind7_number` (`+0x1D`), `walk_sound`, `action_sound`, `win_item_1`, `win_rate_1`, `win_item_2`, `win_rate_2` (`+0x20..+0x23`), `experience` (`+0x24`), `money` (`+0x26`), `weak_mask` (`+0x28`), `resist_mask` (`+0x2A`) |
| `+0x2E..+0x33` | `items[6]` | the six items the slot carries, numbers from 1 |
| `+0x34..+0x3B` | `spells[8]` | the slot's eight spells, numbers from 1 |

`BtlTargetTraitCheck` (`0x80037648`) tests the resist mask first (answer 1)
and the weak mask after (answer 2). battle.md §"The stats — `BtlStats`,
0x3C bytes at `0x801D2B00`" has each field's readers and writers and the
values measured in a battle.

## The row functions — `scene_state_table`

`scene_state_table` is the C's label, from the first split, for a run of
132 function addresses at `0x800B2EF8`. Entries 0 to 93 are the row
functions, function numbers 1 to 94: `BtlActionRowRun` calls entry
`function_number − 1` when the number is below 0x5F. Entries 94 to 131 are
the enemies' acting shows, `g_BtlActingShowFunctions` (`0x800B3070`).
battle.md §"The row runner — `BtlActionRowRun` (`0x80031ABC`)" and §"More
effect handlers: the other functions of the table" have the functions.
