# Boss AI — the per-boss routines behind `BtlEnemyActionSpecialSet` (`0x800417E4`)

This page covers the boss fights: the routine each boss plans its turn
with and re-checks it by (btl_plan.c), the special encounters' set-up,
every place the battle code tests the encounter number, the two-actor
boss of encounter 0xF7, and the transformations some bosses cast on the
party. It is the companion of battle.md §"Battle entry and the enemy-AI
dispatcher" and §"The action loop — `BtlTurnOrderRun` (`0x8002DC30`),
`BtlActionAdjust` (`0x8002E914`), `BtlActionTargetAdjust` (`0x8002ED34`)".
Everything below is read from the C (btl_plan.c, btl_action.c and the
files named). Enemy and special names are the game's own, from its
string table (docs/spec/text.md §"The string table — LUNADATA resource
1"): an enemy's name is string id + 0x80, a special's string class |
0x300.

## Two entry points, one routine per boss

Every per-boss routine with an argument takes one byte:

* **1 — plan.** For a boss fight (`g_BtlNumber >= 0xEB`) the round loop
  `BtlRun` (`0x80027434`) calls `BtlEnemyActionSpecialSet` (`0x800417E4`)
  instead of the generic `BtlEnemyActionSet` (`0x8004C958`), and then only
  `BtlEndCheck` (`0x80038518`): none of the generic path's
  `BtlEnemyActionAdjust` (`0x80029510`), `BtlEnemyActionAnimationSet`
  (`0x8002965C`) or `BtlEnemy4FPlaceMark` (`0x800279A0`). A sleeping boss's
  plan is not turned into 0xB there; `BtlActionAdjust` refuses status 4 or 8
  at the turn. `BtlEnemyActionSpecialSet` walks enemy slots 6 to 13
  (`g_BtlActingSlot` (`0x800B8C26`)), plans every active one (`g_BtlSlotActiveFlags` (`0x800E49A0`)), with
  no HP or status test, and switches on its id `g_BtlSlotIds` (`0x800E48E0`); an id outside
  0x6E..0x7F plans nothing.
* **0 — re-check.** `BtlActionSpecialAdjust` (`0x8002F29C`) is called from
  `BtlActionTargetAdjust` just before the action runs, for an acting slot of
  6 or more in a fight numbered 0xEB or more, with the action loaded into
  `g_BtlActionKind` (`0x800B8C66`), `g_BtlActionNumber` (`0x800B8C67`) and `g_BtlTargetSlot` (`0x800B8C5D`). It answers 1
  for 0x6F, 0x71, 0x73, 0x74, 0x75, 0x77, 0x78, 0x79, 0x7A and 0x7B, after
  calling the routine with 0, and for 0x7C, with no call; 0 for any other
  id. When it answers 1, `BtlActionTargetAdjust` does nothing else, so for
  those bosses the kind-7 head checks (the status fallback to
  `BtlActionFallbackSet` (`0x8002F3A0`)) and the attack row's target mode
  never run; 0x6E, 0x70, 0x72, 0x76, 0x7D, 0x7E and 0x7F take the generic
  path. A kind of 0xB from a re-check reaches `BtlActionKindRun`
  (`0x8002E12C`), which has no case for it, so the turn does nothing.

A plan writes the slot's **kind** `g_BtlSlotPlannedKinds` (`0x800E4910`) (2 = plain attack,
7 = special numbered by `g_BtlSlotPlannedNumbers` (`0x800E4920`), 0xB = no action —
`BtlActionAdjust` skips the turn), often its **target**
`g_BtlSlotPlannedTargets` (`0x800E4900`), and ends with **`BtlActingAnimationSet(pose)`
(`0x80043398`)**: `BtlAnimationSet` (`0x80027208`) on the acting boss's
0x40-byte record and `g_BtlSlotRestAnimations[slot] = pose` (the rest pose
`BtlWalkPathFollow` (`0x8002CA78`) puts back after a walk). Kyle's plan
(0x7E) is the one that sets no pose. A re-check typically picks
`g_BtlTargetSlot` again, or turns the kind into 0xB (`g_BtlActionKind =
0xB`) when the special has no target left. `BtlRoundStatusUpdate`
(`0x800297A4`) clears the plan tables (kind, number, target) with the
amounts and fall marks every round, so a field a plan does not write is 0
for the round.

**Per-boss scratch: `g_BtlSpecialBytes`** (`0x800E4B50`), ten bytes that
`BtlStateInit` (`0x8002B7DC`) clears. The bosses use `[0]`, `[1]` and `[7]` as
phase counters; the transformations use `[0..3]` as per-party-slot
countdowns (written by `BtlTargetHold86Run` (`0x80034AA4`) and
`BtlTargetHold92Run` (`0x80035054`)), and `[5..8]` (`[5..9]` for 0xF5) hold
the party's saved rest poses, set by `BtlSpecialInit` (`0x8002A7FC`) for
encounters 0xEB and 0xF5 only. The table is shared, and
`BtlSlotStatusAnimationSet` (`0x8003801C`) gives a party slot whose byte is
not 0 no status pose (it keeps `g_BtlSlotRestAnimations` (`0x800E4940`)): a boss counter
in `[0]` or `[1]` changes slot 0's or 1's status pose too. That is
presentation only.

**The pickers.** `BtlSlotNearestFind(2)` (`0x80039838`) is the nearest
active party slot (battle.md §"The action loop — `BtlTurnOrderRun`
(`0x8002DC30`), `BtlActionAdjust` (`0x8002E914`), `BtlActionTargetAdjust`
(`0x8002ED34`)"). `BtlPartyTargetPick(2)`
(`0x8003995C`) collects party slots 0 to 4 that have an id and are active
and draws one with `rng_list_pick(list, n, 7)` (`0x8003A03C`); with none it
calls `BtlActionFallbackSet` and answers `g_BtlTargetSlot`. `rng_list_pick`
always draws; `rng_mask_below(mask, n)` (`0x8003A0D4`) does not when `n <
2`, so the Slimer's, Royce's and the Water Dragon's one-candidate picks draw
nothing. Both reduce by repeated subtraction, so a pick among n candidates
is not uniform unless n divides the mask + 1 (spell-rng.md §"The RNG
helpers"). `rng_7f_draw` (`0x8003A0B4`) is `rng_draw() & 0x7F`; `rng_draw`
(`0x8001C770`) is the raw draw.

| id | boss | plan routine | re-check |
|---|---|---|---|
| 0x6E, 0x70 | Plaster Mel, Master Mel | `BtlEnemy6EEnemy70ActionSet` (`0x80041E24`): attack at slot 0, pose 0 | generic |
| 0x6F | Saline Slimer | `BtlEnemy6FActionSet` (`0x80041A80`) | kind 2 → nearest; class 0x50 → `BtlPartyTargetPick(2)` |
| 0x71 | Water Dragon | `BtlEnemy71ActionSet` (`0x80041E78`), with `BtlEnemy71TargetPick` (`0x80041F6C`) | kind 2 → `BtlPartyTargetPick(2)`; a special with no target → 0xB |
| 0x72 | Truffle Troubler | `BtlEnemy72ActionSet` (`0x80042034`) | generic |
| 0x73 | D Master Zoc | `BtlEnemy73ActionSet` (`0x800420E0`) | kind 2 → nearest, else `BtlPartyTargetPick(2)` |
| 0x74 | Vile Crustacean | `BtlEnemy74ActionSet` (`0x80042224`) | nearest, whatever the kind |
| 0x75 | Bronze Dog | `BtlEnemy75ActionSet` (`0x800422F4`) | kind 2 or class 0x58 → nearest |
| 0x76 | Tempest | `BtlEnemy76ActionSet` (`0x80042408`) | generic |
| 0x77 | Black Dragon | `BtlEnemy77ActionSet` (`0x800424A8`) | Cold Breath → Thunder Breath when nobody is near its row |
| 0x78 | Magic Masher | `BtlEnemy78ActionSet` (`0x800426B0`) | `BtlPartyTargetPick(2)` |
| 0x79 | Magic Emperor | `BtlEnemy79ActionSet` (`0x800427C4`) | kind 2 → `BtlPartyTargetPick(2)` |
| 0x7A | Royce | `BtlEnemy7AActionSet` (`0x80042958`) | class 0x6A or 0x6D → `BtlPartyTargetPick(2)` |
| 0x7B | Xenobia | `BtlEnemy7BActionSet` (`0x80042E2C`) | `BtlPartyTargetPick(2)` |
| 0x7C | Magic Emperor (second) | `BtlEnemy7CActionSet` (`0x80042F4C`) | answers 1, no call |
| 0x7D | Ghaleon | `BtlEnemy7DActionSet` (`0x800431AC`) | generic |
| 0x7E | Kyle | `BtlEnemy7EActionSet` (`0x80043298`) | generic |
| 0x7F | Shadow Spectre | `BtlEnemy7FActionSet` (`0x800419B0`) | generic |

## The routines

**Saline Slimer (0x6F) — `BtlEnemy6FActionSet`.** Plan: for party slots 0
to 3, a countdown `g_BtlSpecialBytes[i]` that is not 0 goes down by one;
when it reaches 0 the member gets its own form back ("The restores",
below, under §"The transformations — effects 86 / 89 / 92, and the
party's `g_BtlPartyNotHeldFlags`"). Then one `rng_7f_draw` roll: below 0x32 a plain attack (pose 7);
below 0x64 the special **Gelatin Breath** (0x50, pose 0); else **Mucus**
(0x4F, pose 0xC) at a party slot 0 to 3 that is active and takes commands
(`g_BtlPartyNotHeldFlags`), drawn with `rng_mask_below(3, n)`, falling back
to a plain attack (pose 7) when there is none.

**Water Dragon (0x71) — `BtlEnemy71ActionSet`.** Plan: on a coin
(`rng_draw() & 1`) it tries `BtlEnemy71TargetPick(0xF0)`, the filter of
party slots 0 to 2 that are active with x (`+6`, s16) at most the
threshold, one drawn with `rng_mask_below(3, n)` into `g_BtlTargetSlot`;
with one it plans the special **Fire Press** (0x51, pose 7), otherwise (or
on the other side of the coin) a plain attack (pose 0). The filter writes
the turn's `g_BtlTargetSlot`, not `g_BtlSlotPlannedTargets`, so at plan time
its target is lost and only its draw stays; the re-check runs it again.

**Truffle Troubler (0x72) — `BtlEnemy72ActionSet`.** Kind 7; on a coin
(`rng_draw() & 1`) class 0x52 (Flash Bolt) at the nearest party slot, pose
0, or class 0x53 (Mad Spark), pose 7, with no target written.

**D Master Zoc (0x73) — `BtlEnemy73ActionSet`.** Target
`BtlPartyTargetPick(2)`, then `rng_mask_below(7, 6)`: 0 to 2 a plain attack
(pose 0), 3 or 4 class 0x54 (pose 7), 5 class 0x55 (pose 0xE).

**Vile Crustacean (0x74) — `BtlEnemy74ActionSet`.** An `rng_7f_draw` roll,
no target written: below 0x2A a plain attack (pose 0); below 0x54 **Mini
Doom** (0x56, pose 7); else **Crimson Ray** (0x57, pose 9).

**Bronze Dog (0x75) — `BtlEnemy75ActionSet`.** Target
`BtlPartyTargetPick(2)`, then an `rng_7f_draw` roll: below 0x2A a plain
attack (pose 0); below 0x54 **Fire Ball** (0x58, pose 7); else **Heat
Rush** (0x59, pose 0xB).

**Tempest (0x76) — `BtlEnemy76ActionSet`.** A coin (`rng_draw() & 1`): a
plain attack (pose 0) or class 0x5A (pose 7); target 0.

**Black Dragon (0x77) — `BtlEnemy77ActionSet`.** It writes kind 7 into the
plan on both entries, plan and re-check. In encounter 0xF9 it is always
**Thunder Breath** (0x7E, pose 7), on the re-check too. Otherwise a
four-step cycle on `g_BtlSpecialBytes[0]` (`(x + 1) & 3`): steps 0 and 2
**Fire Breath** (0x5C, pose 0) at `BtlPartyTargetPick(2)`; step 1 **Cold
Breath** (0x5E, pose 0xB); step 3 **Thunder Breath** (0x5D, pose 7). The
re-check turns a Cold Breath into Thunder Breath (0x5D) when no active party
member 0 to 3 stands with its y (`+0xA`) within the boss's y − 0x18 .. y +
0x10: Cold Breath needs someone near the boss's row.

**Magic Masher (0x78) — `BtlEnemy78ActionSet`.** Kind 7 and
`rng_mask_below(0xF, 0xA)`: 0 or 1 a plain attack (pose 0); 2 class 0x5F
(pose 7); 3 to 6 class 0x60 (pose 7); 7 to 9 class 0x61 (pose 9). No target
written.

**Magic Emperor (0x79) — `BtlEnemy79ActionSet`.** Plan: by bit 0 of
`g_BtlSpecialBytes[0]`, a plain attack (pose 0) or a special (pose 7) that
cycles **Inferno / Nitro Dagger / Tornado / Rock 'N Roll** (0x62 to 0x65)
on `g_BtlSpecialBytes[1]` (`(x + 1) & 3`); `[0]` goes up every plan.

**Royce (0x7A) — `BtlEnemy7AActionSet`.** Plan: `BtlActionOutcomeClear`
(`0x800364C0`), then for party slots 0 to 3 a countdown
`g_BtlSpecialBytes[i]` (the burn and the restore: "The restores", below). Then
kind 7 and `r = rng_mask_below(7, 5)`, class `0x6A + r`: **Shoot Lancer**
(pose 0), **Flare Strike** (pose 7), **Flame Bird** (pose 9), **Cross
Bridge** (pose 0xB), **Arch Flare** (pose 0xF). Flare Strike and Flame Bird
need at least four eligible party slots among 0 to 4 (active, taking
commands, status byte `+0x15` 0) and are aimed at one of them, drawn with
`rng_mask_below(3, n)`; with fewer they become Cross Bridge (0x6D, pose
0xB). With five eligible, the mask 3 never reaches the fifth candidate,
slot 4: slot 4 can be the target only when exactly four are eligible.

**Xenobia (0x7B) — `BtlEnemy7BActionSet`.** Kind 7 and `rng_draw() & 3`: 0
a plain attack (pose 0), 1 class 0x6F (pose 0xA), 2 class 0x70 (pose 7), 3
class 0x71 (pose 0xC). No target written.

**Magic Emperor, second form (0x7C) — `BtlEnemy7CActionSet`.** Kind 7 at
`BtlPartyTargetPick(2)`; one raw draw and one `rng_mask_below(7, 5)` are
spent with their results unused (every path writes the pose after), then a
five-step cycle on `g_BtlSpecialBytes[7]`: steps 0 to 2 one of the classes
0x7F, 0x80, 0x82, 0x83 by `rng_mask_below(3, 4)` (poses 4, 0, 2, 6); step 3
class 0x81 (pose 0x13) while fewer than two of party slots 0 to 4 are
inactive, otherwise a plain attack (kind 2, pose 3) at a fresh
`BtlPartyTargetPick(2)`; step 4 a coin (`rng_draw() & 1`) between that
plain attack and the step-0-to-2 pick. Classes 0x7F to 0x83 have no entry
in the special-name strings: they are scripts, below.

**Ghaleon (0x7D) — `BtlEnemy7DActionSet`.** A phase counter
`g_BtlSpecialBytes[0]`, raised every plan: below 3 no action (kind 0xB,
pose 3); at 3 **Nitro Dagger** (0x63, pose 4); after that **Hell Wave**
(0x73) at `BtlPartyTargetPick(2)`, pose 2. npc-scripting.md §"Ghaleon's own
spell, Hell Wave" has the show.

**Kyle (0x7E) — `BtlEnemy7EActionSet`.** Kind 7 and `rng_draw() & 3`: 0 a
plain attack, 1 to 3 classes 0x74 to 0x76 (Power Slash, Power Sweep, Sonic
Riser); target 0; no pose.

**Shadow Spectre (0x7F) — `BtlEnemy7FActionSet`.** Kind 7 and an
`rng_7f_draw` roll: below 0x2A a plain attack at the nearest party slot;
below 0x54 class 0x3A at `BtlPartyTargetPick()`; else class 0x39; pose 0.
The C calls `BtlPartyTargetPick` with no argument, but in the image `$a0`
is 2 at the call (set in the delay slot at `0x800419E8`), so it is the
mode-2 pick like every other.

Ids 0x7C, 0x7D and 0x74 also have arms of their own in the lane mark
`BtlTargetFrontMark` (`0x8003568C`) (below, and battle.md §"The approach
walk — `BtlWalkRun` (`0x8002BB5C`) and the arena grid"), and
`BtlTargetReachCheck` (`0x800379AC`) gives 0x79, 0x7C, 0x7D and 0x7F walk
poses of their own.

### The second Magic Emperor's turn — `BtlActionSeriesRun` (`0x8002DD88`)

The turn loop calls it instead of the generic turn `BtlActionRun`
(`0x8002E088`) when `g_BtlSlotIds[g_BtlActingSlot] == 0x7C`. Classes 0x7F to
0x83 are not specials themselves: each is a script of two or three actions,
every one run by `BtlActionRun()`; a result that is not 0 (the fight is
over) ends the script. With kind `g_BtlActionKind == 7` it switches on
`g_BtlActionNumber`:

| planned class | first action | then |
|---|---|---|
| 0x7F | 0x64 **Tornado** | 0x72 **Worm Crush** at a fresh `BtlPartyTargetPick(2)` target |
| 0x80 | 0x62 **Inferno** | 0x63 **Nitro Dagger**, target not picked again |
| 0x81 | 0x68 **Fate Storm** at `rng_list_pick(list, n, 3)`, the list being the active slots among 0 to 3 only (slot 4 is never a candidate) | 0x67 **Chaos Shield** when record 7's HP (the u16 at `CharStats + 0x1A4`) is 0; otherwise the two plain attacks |
| 0x82 | target `BtlPartyTargetPick(2)`, 0x73 **Hell Wave** | 0x65 **Rock 'N Roll** |
| 0x83 | 0x66 **Siphon Soul** | the two plain attacks |
| other | nothing | — |

"The two plain attacks" — also the whole turn for any kind other than 7 —
are twice: kind 2 into `g_BtlActionKind` and
`g_BtlSlotPlannedKinds[g_BtlActingSlot]`, target `g_BtlTargetSlot =
BtlPartyTargetPick(2)`, `BtlActionRun()`. Every action after a script's
first writes the kind (7 or 2) again into both `g_BtlActionKind` and the
plan entry, because `BtlActionRun` zeroes the actor's kind when one of its
two passes returns 0 (battle.md). The function always returns
`BtlEndCheck() & 0xFF`. The names are strings class + 0x300.

The steps it does not aim again run on `g_BtlTargetSlot` as the step before
left it: 0x80's Nitro Dagger after Inferno and 0x82's Rock 'N Roll after
Hell Wave run on 14, because both first steps end in the wake loop
`BtlMarkedStatus8Clear` (`0x80030B8C`), which steps `g_BtlTargetSlot` itself
over 0 to 13; no slot of the field is then their presentation's target, so
nobody is faced. 0x81's Chaos Shield runs on Fate Storm's target, or on the
slot an Angel Ring revived after it (`BtlItem83Revive` (`0x80038868`) sets
`g_BtlTargetSlot` per revive). `BtlActionSeriesRun` saves and uses `$s0` and
`$s1` only, so `$s2` still holds `BtlTurnOrderRun`'s `&g_BtlTurnOrder`
through its steps, the value `BtlShow50Run` (`0x80086E84`)'s sweep starts from (battle.md
§"The breath attacks' party sweep — `BtlPartyRightMove` (`0x80086700`)");
none of 0x7C's classes reaches it.

### Party counts

Two of the boss routines' party tests are counts, and a party slot nobody
was placed in (id 0, not active) counts as a member who is down:
`BtlEnemy7CActionSet`'s step 3 plans script 0x81 only while fewer than two
of slots 0 to 4 are inactive, and `BtlEnemy7AActionSet` aims Flare Strike or
Flame Bird only when four of slots 0 to 4 are eligible. With a party of
three, the second Magic Emperor never plans Fate Storm and Chaos Shield, and
Royce never casts Flare Strike or Flame Bird. Every other test builds a list
of candidates (the Slimer's Mucus 0 to 3, the Water Dragon's filter 0 to 2,
the Black Dragon's band 0 to 3, the 0x81 script's Fate Storm pick 0 to 3,
`BtlPartyTargetPick(2)`, `BtlSlotNearestFind(2)`), which an empty slot never
joins.

## Being a boss is a property of the fight — every read of the encounter number

Nothing in a battle record says "boss". Every boss rule is a test on
`g_BtlNumber` (`0x800B8C79`), the encounter number `BtlRun` stores from its argument. The
list below is every such read in the battle code, directly or through
`BtlSpecialCheck` (`0x80038500`), which answers `g_BtlNumber >= 0xEB`.

| function | test | what it decides |
|---|---|---|
| `BtlRun` (round loop) | `== 0xF6` | `BtlSlot6AnimationAlternate` (`0x80027934`), the entrance, before round 1 |
| | `!= 0xF9` | Nall's revive roll `BtlSlot5ReviveTry` (`0x80027B6C`) each round |
| | `< 0xEB` | the generic AI (`BtlEnemyActionSet`, `BtlEnemyActionAdjust`, `BtlEnemy4FPlaceMark`) — else `BtlEnemyActionSpecialSet` |
| | `>= 0xEB` | win music 0x3E, else 0x3D |
| `BtlClose` (`0x80029214`) (exit) | `>= 0xEB` | frees `g_BtlPartyKeptRecords` (`0x800B8CC8`), the saved records |
| `BtlInit` (`0x80029E68`) (entry) | `== 0xE4` | slot 5's id cleared (no Nall) |
| | `>= 0xEB` | `BtlSpecialInit` — the scaling, actor 6's footprint and pose, 0xF7's and 0xFE's record copy, 0xF9's and 0xFA's round limit |
| `BtlEnemyLoad` (`0x8002B3AC`) | row `g_BtlNumber` of `g_BtlNumberRows` (`0x800AC172`) | the enemy set and the formation |
| `BtlRoundStatusUpdate` | `!= 0xF9` | status 4's wear-off tick |
| `BtlPartyEscapeTry` (`0x80027DAC`), `BtlActionKindRun` kind 6 | `>= 0xDF` | the Run fails, with no draw |
| `BtlActionTargetAdjust` | actor `>= 6 && >= 0xEB` | `BtlActionSpecialAdjust`, the boss re-check |
| `BtlActionAttackRun` (`0x80031060`) | `>= 0xEB &&` actor `>= 6` | every swing of an enemy's plain attack picks a party member again (below) |
| `BtlHitSlotsUpdate` (`0x80030800`) | `>= 0xEB` | the post-hit pass covers slots 0 to 4 only |
| `BtlStatusTakeCheck` (`0x80035378`) | target `>= 6 && >= 0xEB` | a status spell never takes on an enemy |
| `BtlEnemyClearRun` (`0x8003281C`), `BtlEnemyAllRemoveRun` (`0x80033F0C`) (effects 15, 58) | `BtlSpecialCheck` | no enemy is taken off |
| `BtlHitAmountTake` (`0x80030650`) | target `>= 6 &&` 0xF9, 0xFA or 0xFE | nothing comes off an enemy's HP |
| | `== 0xF7 &&` target 6 | the hit comes off record 7 while it has HP (§"The second Magic Emperor, encounter 0xF7 — a two-actor boss") |
| `BtlHitAmountSet` (`0x80030070`), `BtlTargetHpAmountTake` (`0x80035FD8`) | `== 0xFA` | every amount is 0 |
| `BtlF7Slot7RecordOff` (`0x80030F8C`) | `== 0xF7` | record 7's fall |
| `BtlItem83Revive` (auto-revive, item 0x83) | `== 0xF9`; `== 0xFE &&` `g_BtlSpecialBytes[0] >= 5` | the pass does nothing |
| `BtlActingShowRun` (`0x8003A158`) | actor `== 6`, by number | the boss's voice: 0xF5 0x3A, 0xF6 0x3B, 0xF7 and 0xFE 0x38 |
| `BtlEnemyFallShow` (`0x80089488`) (enemy KO) | `g_BtlSlotFallMarks[6] &&` 0xF6 or 0xF7 | actor 6 takes animation 5 and stays drawn; nobody is scaled away |
| `BtlFrontStat0EHitRun` (`0x80032F00`) (effect 26) | `== 0xFE` (class 0x73) | power 0x1E and area half height 8 |
| `BtlWalkEdgeRun` (`0x80036654`) (lane stance) | 0xFE or 0xF7 | the walk ignores the other actors |
| `BtlShowAFRun` (`0x8003E6CC`) (action 0xAF) | 0xF7; 0xFE or 0xF7 | record 7's animations; the stand-in record in the caster's slot |
| `BtlShowB2Run` (`0x8003EF4C`), `BtlShowB3Run` (`0x8003F2EC`) (0xB2, 0xB3) | `== 0xFE` | which effect block is borrowed (0x5C / 0x5D, else 0x5F / 0x60) |
| `BtlEnemy77ActionSet` (Black Dragon) | `== 0xF9` | class 0x7E only |
| `BtlPartyAllChoiceSet` (`0x8004DB30`), `BtlPartyOneChoiceSet` (`0x8004D61C`) (party AI) | `>= 0xDF` | the tree's "go all out" arm |
| `BtlPartyHpLeastFind` (`0x8004E44C`), `BtlPartyHpNot0LeastFind` (`0x8004E6A0`) | `>= 0xDF` | both arms are the same code — no effect |

The `g_BtlSpecialBytes[0]` that `BtlItem83Revive` tests in 0xFE is
Ghaleon's turn counter, so the Angel Ring revives through his fourth turn,
not from his fifth (battle.md, "The auto-revive, beside its callers", under
§"The round's pieces").

**Every swing picks again.** In a boss fight, `BtlActionAttackRun`'s loop
opens, for an enemy attacker, with `g_BtlTargetSlot =
g_BtlSlotPlannedTargets[actor] = BtlPartyTargetPick(2)`: every swing of a
boss's plain attack, the first included (the loop's head is before
`BtlAttackTargetFind` (`0x800317E0`)), lands on a freshly drawn active party
member, one `rng_list_pick` draw each, so a boss's plain attack never lands
on the target its plan named. With no active party member,
`BtlPartyTargetPick` calls `BtlActionFallbackSet` instead (no draw), which
for an enemy sets `g_BtlActionKind = 2` and `g_BtlTargetSlot =
BtlSlotNearestFind(2)`; with no candidate that answer is a register
`BtlSlotNearestFind` never set (`$t4`), which neither `BtlPartyTargetPick`
nor `BtlActionFallbackSet` writes either. No draw is made either way, and
the fight is lost whatever slot it names.

**One boss per boss encounter.** The formations that `g_BtlNumberRows`
names field a single enemy id in actor 6 in every fight 0xEB to 0xFE —
0xEB 0x6F, 0xEC 0x71, 0xED 0x72, 0xEE 0x73, 0xEF 0x74, 0xF1 and 0xF9 0x77,
0xF2 0x7F, 0xF3 0x78, 0xF4 0x79, 0xF5 0x7A, 0xF6 0x7B, 0xF7 0x7C, 0xF8 0x6E,
0xFA 0x7E, 0xFC 0x70, 0xFD 0x76, 0xFE 0x7D — except 0xF0, two Bronze Dogs
(0x75) in actors 6 and 7. 0xFB and 0xFF name set 0, formation 0 (unused).
So "the boss is actor 6" and "every enemy of the fight is a boss" are the
same statement everywhere in the game, and the code uses both.

## Entry — `BtlSpecialInit` (`0x8002A7FC`)

`BtlInit` calls it for every fight numbered 0xEB or more. It allocates
`g_BtlPartyKeptRecords` and copies the battle records of slots 0 to 4 into
it, then by encounter: `BtlEnemyLevelSet` (`0x8002ADF0`) raises the enemies'
level (to the lead's level or the encounter's floor, capped at 50) and their
stats by per-level increments; some encounters change the first enemy's
width (actor 6 `+0x3C`: 0xEC = 0x70, 0xF1 and 0xF9 = 0x50, 0xF5 − 0x14, 0xF6
− 0x30, 0xF7 and 0xFE − 0x10); 0xF6 sets actor 6 on animation 0x11; 0xEB and
0xF5 save the party's rest poses in `g_BtlSpecialBytes[5..]`; 0xF7 and 0xFE
copy actor 6's record to actor 7 and switch actor 7 off; 0xF9 paralyses
party slots 0 to 3; 0xF9 and 0xFA set the three-round limit
`g_BtlRoundsLeft` (`0x800B8C0E`).

## The scripted fights 0xF9 / 0xFA

* **0xF9 (Black Dragon, 0x77).** After the scaling and the width:
  `CharStats[i] +0x15 = 4` for i = 0 to 3 — a store, not an OR, whoever holds
  the slot — and `BtlSlotStatusAnimationSet(i)`, the status pose. **Status 4
  is paralysis**: the panel's `BtlPartyPanelStatusDisp` (`0x8004B788`) writes
  word 0x44 "PARALYZE" for `+0x15 == 4` and 0x48 "SLEEP" for 8, and
  `BtlSlotStatusAnimationSet` gives bit 4 the pose 0x15. With
  `BtlRoundStatusUpdate`'s bit-4 wear-off tick off in this encounter the
  paralysis lasts the whole fight: no member of slots 0 to 3 acts, Nall's
  revive roll is skipped, and the Black Dragon's Thunder Breath (class 0x7E,
  the only class `BtlEnemy77ActionSet` plans here) is all that happens.
  Nothing comes off its HP (`BtlHitAmountTake`).
* **0xFA (Kyle, 0x7E).** `BtlEnemyLevelSet(0, 0, 0, 0, 0, 0, 1)`: every
  increment 0, so only the level byte moves (max(1, the lead's), capped at
  50) and Kyle keeps his row's HP, 20. With every amount 0 in this encounter
  (`BtlHitAmountSet`, `BtlTargetHpAmountTake`) and nothing off an enemy's HP
  (`BtlHitAmountTake`), neither side can fall.
* **Both: `g_BtlRoundsLeft = 3`.** `BtlRun` ends the fight after its third
  round — silently, with a win's return code, whatever the third round
  itself came to (battle.md §"The round loop — `BtlRun` (`0x80027434`)", "The
  round limit, in order").

## The Water Dragon's swing, and the grid

**The Water Dragon's swing is written for its own fight's geometry.** Its
attack presentation, enemy action 0x98 (`BtlShow98Run` (`0x8003A6D0`)),
opens with `BtlActingTargetLeftMove(0xA0)` (`0x8003A4E4`): the dragon creeps
to 0xA0 px left of its target, on the target's row, wherever the target
stands. In 0xEC the party stands at x >= 0xC0, so the dragon ends on the
field. With a target at x <= 0x58 its anchor is at −0x48 or beyond and its
whole 0x70-wide footprint (`BtlSpecialInit` sets actor 6 `+0x3C` = 0x70
there) is left of column 0. The next `BtlWalkRun` measure to it then
computes a width of 0 or less: `g_BtlWalkTargetColumn` (`0x800B8C8C`) is
the dragon's column (x / 8, rounded toward 0) − 7, clipped to 0, and
`g_BtlWalkWidths` (`0x800B8C90`) word 0 is 14 plus the negative start,
rounded up to even, as u16 — −2 for a dragon at x = −72, 0 at −64 and −56
— and the ring search `BtlWalkGoalNearestFind` (`0x8002C3E8`) indexes outside
the 40 × 12 grid. The game has no guard.

### The grid has no clip on the right

`BtlWalkRun` takes a wide actor's span as column − tiles / 2 .. + tiles / 2
− 1 (tiles = `+0x3C` / 8, rounded up to even) and clips it at column 0 only
(a negative start shortens the span and starts it at 0). A span past column
39 runs on into the next row's cells and, from the last rows, past the
480-cell grid. The game's formations never stand anyone there: of the 968
enemies they place (every encounter, each with its footprint), the furthest
reaches column 37 of 40, measured over the formation tables and the enemies'
sprite boxes. The second Magic Emperor can reach it with five members in
the field: the knockback `BtlTargetPushBack` (`0x80037C64`) can push him (56
px wide) to x = 284 (it moves a target only while x < 0x138 − width / 2),
his 0xAF strike widens him by 0x20 to 88 px (`BtlShowAFRun`), and at x = 284
that footprint passes column 39, so the next walk's measure reads past the
grid (battle.md §"The party's x shift, measured; five members").

## The second Magic Emperor, encounter 0xF7 — a two-actor boss

Read from btl_init.c (`BtlSpecialInit`, `BtlInit`), btl_hit.c
(`BtlHitAmountTake`, `BtlF7Slot7RecordOff`, `BtlActionAttackRun`),
btl_action.c (`BtlTurnOrderRun`, `BtlActionSeriesRun`, `BtlActionRun`,
`BtlActionSpellRun` (`0x8002E718`)), btl_row.c (`BtlActionRowRun`
(`0x80031ABC`)), btl_target.c (`BtlPartyHpDrainRun` (`0x80034108`),
`BtlSlot7HpSetRun` (`0x80034260`), `BtlTargetFallRun` (`0x800342C0`),
`BtlTargetFrontMark`) and btl_plan.c (`BtlEnemy7CActionSet`).

**Record 7 is the shield.** Function numbers 60, 61 and 62 of the row
functions (`scene_state_table`, entry number − 1, from `0x800B2EF8`) are
`BtlPartyHpDrainRun`, `BtlSlot7HpSetRun` and `BtlTargetFallRun`: the
attack rows 102 "Siphon Soul", 103 "Chaos Shield" and 104 "Fate Storm" are
the only rows naming them (no spell or item does, and no enemy row names
102 to 104). The pieces:

* **Entry** (`BtlSpecialInit` case 0xF7): the scaling, actor 6 `+0x3C −=
  0x10`, then record 7 = a copy of record 6 and record 7's `on` (`+0x00`) =
  0. The stats are not copied: `BtlStateInit` cleared them, so slot 7 has no
  id (`g_BtlSlotIds[7] = 0`), is never active (`BtlInit` sets the flag from
  the id), and its HP (`CharStats + 0x1A4`) is 0. No id: no arena wall (the
  walk grid skips id 0), no turn, no target, not counted by `BtlEndCheck`.
* **Effect 61** (`BtlSlot7HpSetRun`): `BtlShowNumberRun` (`0x8007BB80`)
  first (show 0xA4, `BtlShowA4Run` (`0x8003C6E4`): actor 6 on 0x14, actor 7
  shown on 0xA then 0xB), then record 7's HP = the caster's level (`+0x14`)
  × 10 + 0x1F4 (an s16 store) — 940 to 1000 for this boss (level 44 to 50).
* **The hand-off** (`BtlHitAmountTake`, battle.md §"The damage chain —
  `BtlHitAmountSet` (`0x80030070`) and `BtlHitAmountTake` (`0x80030650`)"):
  every amount for slot 6 — physical (`BtlHitAmountSet`) or stat-0E
  (`BtlTargetHpAmountTake`) — comes off record 7's HP while that is not 0,
  floored at 0; the hit moves to slot 7 (the amount `g_BtlSlotAmounts[7]`,
  the reaction `g_BtlSlotAmountKinds` (`0x800E48F0`) `[7] = 1` whatever the
  hit was, slot 6's both 0; its
  strong-hit mark `g_BtlSlotStrongMarks[6]` (`0x800E49F0`) untouched). So
  slot 6's outcome is 0 for the presentation (`BtlShowOpen` (`0x8007B7C8`)
  faces no marked target, the impact shows slot 7's number), the post-hit
  pass `BtlHitSlotsUpdate` (slots 0 to 4 in a boss fight) sees neither, and
  a drain on the boss (`BtlTargetHpDrainRun` (`0x80034770`) copies
  `g_BtlSlotAmounts[t]`) gains nothing.
* **The fall** (`BtlF7Slot7RecordOff`): called at the end of
  `BtlActionRowRun` (every spell, item and enemy row action, after its
  function) and after each landed swing of `BtlActionAttackRun` (after
  `BtlPartyPanelDisp` (`0x8004B330`)). With `g_BtlNumber == 0xF7`,
  `g_BtlTargetSlot == 6`, record 7 on and record 7's HP 0:
  `BtlAnimationSet(record 7, 0xC)`, `BtlAnimationEndWait` (`0x8002FDBC`),
  `on = 0`. The test is on the turn's `g_BtlTargetSlot`, so an emptying hit
  from a function that loops on `g_BtlTargetSlot` leaves the shield drawn at
  HP 0, taking nothing, until an action that ends on slot 6.
* **Where each function leaves `g_BtlTargetSlot`**, read for every row
  function: the target for 1-4, 6, 9-12, 17, 22, 27, 33, 36, 39, 45, 51,
  55-57, 61, 62, 64, 66-68, 75, 82-85, 87, 88 (the loop workers
  `BtlAreaHitRun` (`0x80032A50`), `BtlAreaStat0EHitRun` (`0x80032C64`),
  `BtlMarkedStat0EHitRun` (`0x80032FA0`), `BtlMarkedHitRun` (`0x80033060`)
  and `BtlHitSlotsUpdate` save and restore it); the caster for 54
  (`BtlActingStat08RaiseRun` (`0x80033C3C`)); 5 for 8, 13, 14, 40, 60 (party
  loops); 14 for 7, 15, 18-21, 24-26, 34, 37, 38, 49, 52, 59, 93, 94 — every
  tail through the wake loop `BtlMarkedStatus8Clear`, and the second loops of
  the area-status workers; 58 the target in a boss fight (its loop is
  skipped), else 14. No presentation the 0xF7 fight reaches writes it (the
  enemy actions that loop on it, such as `BtlShowA3Run` (`0x8003C3D8`) and
  `BtlShowAERun` (`0x8003E274`), are other enemies').
* **Effect 60** (`BtlPartyHpDrainRun`): for party slots 0 to 4, each active
  one is marked 1 and given `BtlStat0EHitAmountSet` (`0x80035EF8`) (its tail's
  3, 4 or 6 replaces the 1); the stored amounts are summed; the caster's HP
  += the sum (u16), held to max HP; the caster's `g_BtlSlotAmounts` (`0x800E48C0`) = the
  whole sum (not the part that fitted, unlike `BtlTargetHpDrainRun`), number
  kind 5; `BtlShowNumberRun` (show 0x55), then `BtlFallRun` (`0x800385B4`).
  No wake and no `BtlMarkedRestAnimationSet` (`0x80030DA0`): the struck stay
  as the impact left them. One draw per active member (none when the
  caster's stat_0e / 10 is below 2).
* **Effect 62** (`BtlTargetFallRun`): when the target's `+0x1E` bit 0x02
  (the barrier) is set, it is spent → number kind 6, the presentation;
  otherwise kind 1 and the fall mark `g_BtlSlotFallMarks` (`0x800E4960`) — a fall whatever
  its HP, no resistance, no evasion — then the presentation (0x56),
  `BtlActingRestAnimationSet` (`0x80037890`), `BtlFallRun`. Then
  `BtlMarkedRestAnimationSet`. No draw.
* **`BtlTargetFrontMark`'s 0x7C arm** (the lane stance of Hell Wave, class
  0x73, effect 26 with area half width 1): sound 0x68, actor 7 on 0x15, the
  caster on 8 to its last step, `BtlWalkEdgeRun(1)`, actor 7 put on the
  caster's x and y (and depth = y), sound 0x68, actor 7 on 0x16, the caster
  on 9 to its last step, actor 7 on 0xB. The 0x7D arm (Ghaleon) is
  `SoundPlay(0x68, 3)` (`0x800555F0`), animation 6 to its last step,
  `BtlWalkEdgeRun(1)`, `SoundPlay(0x68, 3)`, animation 7 to its last step;
  the 0x74 arm is `BtlWalkEdgeRun(0xD)`.

**The 0x81 arm needs four party members.** `BtlEnemy7CActionSet`'s step 3
plans 0x81 (Fate Storm, then Chaos Shield) only while fewer than two of
party slots 0 to 4 are inactive; otherwise the plain attack. So with three
members Fate Storm and Chaos Shield never come and the shield is never
shown (§"Party counts").

## The transformations — effects 86 / 89 / 92, and the party's `g_BtlPartyNotHeldFlags`

Encounters 0xEB (Saline Slimer, 0x6F), 0xED (Truffle Troubler, 0x72) and
0xF5 (Royce, 0x7A) each have a special of their own effect. Row functions:
**86 → `BtlTargetHold86Run` (`0x80034AA4`), 89 →
`BtlActingAreaStat0EHitStatus4Run` (`0x80034E54`), 92 → `BtlTargetHold92Run`
(`0x80035054`)**.

**Effect 86, attack row 79 "Mucus" (`BtlTargetHold86Run`).**
`BtlShowNumberRun(g_BtlActionShowNumber)` first: enemy action 0xB5
(`BtlShowB5Run` (`0x8003F784`)) is the transformation — the lane walk, the
shot, a copy of the caster on the target (animation 0xE, the slime landing,
the same for everyone), then `BtlTargetActingRecordCopy` (`0x8003F9D4`): the
target's battle record becomes a whole copy of the caster's, keeping its x
(`+6`), its y (`+0xA`, written to the depth `+8` too), `+0x0C` and its width
and height (`+0x3C` / `+0x3E`). From then on the target is drawn from the
slime's block. By the target's character id the show sets the form's rest
and hit animations into `g_BtlPartyStandAnimations` (`0x800B8BEC`) (and
`g_BtlSlotRestAnimations`) and `g_BtlPartyHitAnimations` (`0x800B8BB0`):

| id | who | rest | hit | restore (`BtlEnemy6FActionSet`) |
|---|---|---|---|---|
| 1 | Alex | 0x0F | 0x17 | 0x13 |
| 6 | Luna | 0x10 | 0x18 | 0x14 |
| 2 | Nash | 0x11 | 0x19 | 0x15 |
| 7 | Ramus | 0x12 | 0x1A | 0x16 |

Then `BtlTargetHold86Run` sets `g_BtlPartyNotHeldFlags[t] = 0`;
`g_BtlSpecialBytes[t]` = 4 for ids 1, 2 and 6, 5 for 7, untouched for any
other; `g_BtlSlotPlannedKinds[t] = 0xB` when it was not 0. No number kind,
no pose, no draw of its own. So Mucus lasts four of the Slimer's plans (five
for Ramus).

* `BtlShowB5Run`'s switch has no default. For any other character the C's
  two animation locals are never given a value; in the image they are `$s0`
  and `$s1`, which the function saves and sets only in the switch. `$s0` is
  `BtlActingShowRun`'s show number + 0x69 — 0x11E for action 0xB5 — so
  `BtlAnimationSet` is asked for animation 0x11E of a block that has 29;
  `$s1` is what `BtlActingShowRun`'s caller left. The game has no form for 3
  Jessica, 4 Mia, 5 Kyle, 8 Ghaleon, 9 Laike, 10 Tempest or 11 (Alex's
  second record), and its own fight never sends one of them there.
* For those ids `BtlTargetHold86Run` writes no countdown (`default:
  break`): the byte keeps the 0 the fight began with, the Slimer's plan
  never counts it down and plays no restore, and the member stays in the
  slime, taking no commands, until a fall gives its record back
  (`BtlFallRun`) or the fight ends.
* The form poses are stacks of sprite pieces: the slime's puddle, drips and
  bursting pieces are shared by all four forms, and the character pieces are
  the characters' field sprites (resource 0x1E8, docs/spec/assets/sprites.md)
  drawn mirrored. The standing body of each form's last restore pose has the
  same ink mask as that character's field-sheet pose 7, and each head over
  the puddle is that sprite's rows 0 to 12; the arms-out and arms-up bodies
  of the restore were drawn for the forms. The pieces are stored mirrored
  because a party record is drawn mirrored (`+0x0C` bit 13). This was
  measured on the decoded sprite data with station-tools, a custom decomp
  toolkit that will be released later.

**Effect 92, attack row 108 "Flame Bird" (`BtlTargetHold92Run`).** The same
shape with show 0xA8 (`BtlShowA8Run` (`0x8003D354`): no id switch; the form
rests on 0x16 and is hit on 0x17 — Royce's block), its own countdown — id
11 → 3, 5 → 2, 2 and 3 → 4, 4 → 5, none for anyone else — and
`g_BtlSlotPlannedKinds[t] = 0xB` whatever was planned.

**The restores.**
* *Saline Slimer's plan* (`BtlEnemy6FActionSet`): for party slots 0 to 3 a
  `g_BtlSpecialBytes[i]` that is not 0 goes down by one; at 0 the slime plays
  its "out" animation by the member's character id (1 → 0x13, 2 → 0x15, 6 →
  0x14, 7 → 0x16) on the form, waited out (`BtlAnimationEndWait`); the
  record is copied back from `g_BtlPartyKeptRecords` keeping `+6`, `+0xA`
  and `+0xC` (`+8` = the old `+0xA`); `g_BtlPartyStandAnimations[i] =
  g_BtlSpecialBytes[i + 5]`, `g_BtlPartyHitAnimations[i] = 0x12`, the record
  on `g_BtlPartyStandAnimations[i]`, `g_BtlPartyNotHeldFlags[i] = 1`.
* *Royce's plan* (`BtlEnemy7AActionSet`): `g_BtlExtraRecords` (`0x800B8C44`) = a record on
  the stack; for slots 0 to 3 with a byte: it goes down by one; still not 0
  → the burn (`g_BtlSlotAmountKinds[i] = 1`, `g_BtlSlotAmounts[i] =
  rng_mask_below(0xF, 0xA) + 0xF`, 15 to 24 — one draw — off the HP when
  less than it, else only the fall mark `g_BtlSlotFallMarks[i]`: the HP is
  `BtlFallRun`'s to zero); at 0 the restore —
  `g_BtlPartyStandAnimations[i] = g_BtlSpecialBytes[i + 5]`,
  `g_BtlPartyHitAnimations[i] = 0x12`, `g_BtlPartyNotHeldFlags[i] = 1`,
  the extra-record count `g_BtlExtraRecordCount` (`0x800B8BCD`) = 1, the old
  record onto the stack copy, the record
  from `g_BtlPartyKeptRecords` keeping x, y (y → depth too) and `+0x0C`, the
  copy at depth + 1 on animation 0x18 (the bird leaving) and the record on
  its rest animation, 10 battle frames (`BtlFrameWait` (`0x80036BA8`)), the
  copy off, `sprite_run_off(0x1C, 1)` (`0x80019A7C`). Slot by slot, in order.
  After the loop, with any burn: slots 5 to 13's marks cleared,
  `g_BtlFallDone` (`0x800B8C3C`) = 0, `BtlAmountRecordsRun` (`0x8004AE0C`) (the numbers,
  waited out) and, after a fall, `BtlFallRun`, `BtlItem83Revive` (the Angel
  Ring) and `BtlEndCheck` — the plan returns there, before its class draw,
  when the fight is over.
* *The fall clean-up* (`BtlFallRun`): a fall-marked party slot with
  `g_BtlPartyNotHeldFlags == 0` gets its record back the same way first
  (`g_BtlSpecialBytes[i] = 0` too), then pose 0x1C. So a fall is never shown
  in a slime's or bird's pose.
* `g_BtlPartyKeptRecords` is the copy of records 0 to 4 that `BtlSpecialInit`
  makes at the entry of every boss fight; `g_BtlSpecialBytes[5..8]` and
  `[5..9]` the rest animations it saves for 0xEB and 0xF5 only
  (`BtlStateInit` cleared `[0..9]` first, `BtlPartyStatsSet` (`0x8002A138`)
  set `g_BtlPartyStandAnimations`).

**Quirks the C has:**
* Royce's loop covers slots 0 to 3, his Flare Strike and Flame Bird
  candidates 0 to 4: a slot-4 member turned into a bird is never counted
  down — it stays one until it falls.
* Each countdown table lists that fight's own party (86: Alex 1, Nash 2,
  Luna 6, Ramus 7; 92: Alex 11, Nash 2, Jessica 3, Mia 4, Kyle 5). A member
  effect 92 has no entry for stays in the form, taking no commands, until it
  falls or the fight ends.
* Neither re-check aims these specials again (`BtlEnemy6FActionSet` with 0
  touches kind 2 and class 0x50, `BtlEnemy7AActionSet` 0x6A and 0x6D): a
  target felled between plan and turn is transformed all the same (and
  Royce's first burn then marks it fallen, which gives it its form back).
* `BtlSlotStatusAnimationSet`, for a party slot whose `g_BtlSpecialBytes`
  byte is not 0, keeps `g_BtlSlotRestAnimations` — for a transformed member
  its form's rest animation.

**Effect 89, attack row 82 "Flash Bolt"
(`BtlActingAreaStat0EHitStatus4Run`).** `BtlTargetReachCheck` — the reach
test, run whatever the area half width holds (here 10); out of reach the
caster walks closer and that is all. Within reach: the walk block
`g_BtlWalkBlock` (`0x800E4B10`) gets `walk` = 1 and `animation` = 1, then
`BtlWalkRun` (`0x8002BB5C`) (the walk to the target, walk animation 1), `BtlTargetAreaMark(0)` (`0x80035B50`) — the area around
the caster, 10 × 2 cells either side — then `BtlAreaStat0EHitRun` (effect
24's stat-0E hit on each marked slot, show 0xB7, the fall clean-up, poses,
wake), then for `g_BtlTargetSlot` = 0 and 1 only (`< 2U`): marked and active
→ `BtlTargetStatus4Try` (`0x800333B8`) (number kind 2; unless it holds a
status in 0x34, the roll `BtlStatusTakeCheck` and, when it takes, status 4,
paralysis) and `BtlSlotStatusAnimationSet`. Party slots 2 to 4 take the
damage and are never rolled. Truffle Troubler's enemy row also carries row
82 as its `attack_number` (and 83 Mad Spark as its `kind7_number`), which a
boss fight does not read: there `BtlEnemyActionSpecialSet` plans.

**`g_BtlPartyNotHeldFlags` (`0x800B8C50`) — every reader:**
`BtlMenuPendingMaskGet` (`0x80049A0C`) (the menu's party mask
`g_BtlMenuPendingMask` (`0x800B8D55`) — the command menu and the Tactics page),
`BtlPartyActionSet(1)` (`0x8004D2C4`) and `BtlPartyTurnActionSet`
(`0x8004F734`) (the party AI: a member without it plans kind 5, number
0xFF), `BtlPartyActionAdjust` (`0x80029348`) (not confused and without it:
kind 0xB, target 0), `BtlActionAdjust` (refuses its turn — tested before the
status), `BtlActingRestAnimationSet` (returns at once: no rest pose),
`BtlSlot5ReviveTry` (Nall's revive candidates), `BtlEscapeFailMove`
(`0x80036BE4`) (the failed-escape stagger, with 0xFF), `BtlWinMessageDisp`
(`0x8004BB68`) (the victory pose only when `== 1`), `BtlEnemy6FActionSet`
and `BtlEnemy7AActionSet` (the bosses' candidates), `BtlFallRun` (the
restore). Writers: `BtlStateInit` and `BtlPartyStatsSet` (1 at entry),
`BtlTargetHold86Run` and `BtlTargetHold92Run` (0), the three restores (1).
