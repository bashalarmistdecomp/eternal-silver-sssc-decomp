# The item, weapon and boss presentations (0x30..0x42, 0x50..0x56)

This page covers the presentations `BtlShowNumberRun(id)` (`0x8007BB80`)
runs beyond the spells' 0x01..0x2F: the item uses (0x30..0x37), the ranged
weapons' shots (0x3A..0x3C), the usable weapons' casts (0x3D..0x42, with
item effects 87 and 88), the bosses' breaths (0x50..0x53, with effect 93)
and the late boss specials (0x54..0x56, with effect 40 and encounter 0xF6's
entry). It also covers the effect handlers and entry code that run with
them. battle.md §"Spell presentations — `BtlShowNumberRun` (`0x8007BB80`)
and the effect resources" is the frame all of this hangs in. Everything
below is read from the C; where a register matters, from the program's
instructions in the image. The frame totals add the C's waits to animation
lengths taken from the game's sprite data.

Two words recur. The "row flags" of a show are byte 0 of its 4-byte row in
`g_BtlEffectRows` (`0x800AC43C`), which `BtlEffectLoad` (`0x800897DC`) acts on
(1 the effect's sprite block, 2 a palette, 4 the tile layer). The "set-up
clip" is the voice that bytes 2 and 3 of that row name, read by
`BtlShowOpen` (`0x8007B7C8`) and `BtlShowActingPlay` (`0x8008A95C`).

## Group D — the item-use presentations 0x30..0x37 and the ranged weapon shots 0x3A..0x3C

Read from btl_item.c and btl_show.c (the handlers and their workers),
btl_target.c, btl_attack.c and btl_hit.c (who runs them and what they leave
behind), and btl_effect.c, btl_show_frame.c and btl_boss.c (the display
primitives).

None of the eleven presentations draws from the RNG, starts a set-up clip
(`g_BtlEffectRows[id * 4 + 2]` is 0 for all of them) or reads a table of its
own (0x35..0x37 read `g_BtlTargetSpriteMixCols` (`0x80012EB0`) through
`BtlTargetSpriteMixRun` (`0x8007F56C`)).

### Who runs them

| id | handler (`g_BtlShowFunctions[id - 1]` (`0x800B5CA4`)) | item row (`function_number` `+0x08`, `show_number` `+0x09`) | row flags |
|---|---|---|---|
| 0x30 | `BtlShow30Run` (`0x80085818`) | 156 Herb — effect 4 (`BtlTargetHpAddRun` (`0x80031FB4`), HP + 50 + rand) | 0 |
| 0x31 | `BtlShow31Run` (`0x8008585C`) | 157 Healing Nut — effect 4 (HP + 150 + rand) | 0 |
| 0x32 | `BtlShow32Run` (`0x800858A0`) | 158 Star Light — effect 1 (`BtlTargetMpAddRun` (`0x80031E00`), MP + 30 + rand) | 0 |
| 0x33 | `BtlShow33Run` (`0x800858E4`) | 159 Silver Light — effect 2 (`BtlTargetMpFillRun` (`0x80031E84`), MP to full) | 0 |
| 0x34 | `BtlShow34Run` (`0x80085928`) | 160 Angel's Tear — effect 12 (`BtlTargetReviveRun` (`0x800324A4`)); also the Angel Ring's auto-revive `BtlItem83Revive` (`0x80038868`) | 1 (block, resource 455) |
| 0x35 | `BtlShow35Run` (`0x8008597C`) | 161 Cleansing Water — effect 10 (`BtlTargetStatus4ClearRun` (`0x80032398`) → `BtlTargetStatusClearRun(4)` (`0x80032440`)) | 2, no resource |
| 0x36 | `BtlShow35Run` | 162 Antidote — effect 9 (`BtlTargetStatus1ClearRun` (`0x80032378`) → `BtlTargetStatusClearRun(1)`) | 2, no resource |
| 0x37 | `BtlShow35Run` | 163 Holy Water — effect 11 (`BtlTargetStatusAllClearRun` (`0x800323B8`) → `BtlTargetStatusClearRun(0xFF)`) | 2, no resource |
| 0x3A | `BtlShow3ARun` (`0x80085AFC`) | the swing with `worn_item_kind` 3, or 5 on character 2 (`BtlAttackAnimationRun` (`0x800315A8`)) | 0 |
| 0x3B | `BtlShow3BRun` (`0x80085B24`) | the swing with weapon item 0x19, Sling | 0 |
| 0x3C | `BtlShow3CRun` (`0x80085B4C`) | the swing with weapon item 0x1A, Poison Darts | 0 |

Flag 2 with no resource (0x35..0x37): `BtlEffectLoad` skips `resource_load` (`0x80020230`)
(`g_BtlEffectResourceNumbers[id]` (`0x800B5E0C`) is 0) and still runs
`sprite_palette_load(data + 8, 0x600)` (`0x80018BAC`) on whatever the load buffer
`g_ResourceReadBuffer` (`0x800B8828`) held, so row 506 takes stale bytes.
`BtlShow35Run` draws nothing under row 506 (the template is never shown),
so nothing on screen shows it.

Effects 9, 10, 11 and 12 write no `g_BtlSlotAmountKinds` (`0x800E48F0`):
`BtlTargetStatusClearRun`, `BtlTargetStatusAllClearRun` and
`BtlTargetReviveRun` leave the 0 that `BtlActionOutcomeClear` (`0x800364C0`)
put there, so `BtlShowOpen`'s face-off (only for a flagged target) runs for
none of 0x34..0x37. Effects 1, 2 and 4 (and 3 and 6) leave 5 with the
amount that landed.

### 0x30..0x33 — the restoring items (`BtlShow30Run` (`0x80085818`) / `BtlShow31Run` (`0x8008585C`) / `BtlShow32Run` (`0x800858A0`) / `BtlShow33Run` (`0x800858E4`))

Each is `if (g_BtlRunning != 0) BtlActingShowPlacesRun(a, b); else
MenuUseShowPlacesPlay(a, b);` (`0x8007F518`, `0x8007EFC0`) with (a, b) =
(0x780, 6), (0x780, 7), (0x790, 6), (0x790, 7). `g_BtlRunning`
(`0x800B8E86`) is 1 for the whole battle (`BtlRun` (`0x80027434`) sets it
with the encounter number `g_BtlNumber` (`0x800B8C79`) on entry and clears it on the way
out), so in battle the first branch runs; `MenuUseShowPlacesPlay` is the
field menu's.

`BtlActingShowPlacesRun(a, b)` (`0x8007F518`):

1. `BtlShowActingPlay(0x10, 0)` (`0x8008A95C`): the caster's own record plays
   its anim 0x10 to the end (no template, no cue).
2. `BtlShowPlacesPlay(a, b)` (`0x8007F2D8`): sound 0x21 on sound slot 1
   (voice 4); for every slot `i < 0xE` flagged in `g_BtlSlotShowPlaceFlags` (`0x800E54F0`)
   an object at `g_BtlShowRecords` (`0x800B8FFC`) + i × 0x40 is bound to resource 2's bank 0
   (`g_SpriteData0` (`0x800DE088`)) under CLUT word `a`, anim `b`, `attr`
   (`+0x0C`) 0, at (`g_BtlSlotShowPlaceXs[i]` (`0x800E5548`), `g_BtlSlotShowPlaceYs[i]` (`0x800E5568`)), depth
   `g_BtlSlotShowPlaceYs[i] + 1`; an unflagged slot's object is retired
   (`BtlRecordClear` (`0x80049F18`)). Then `BtlAmountShowStart(1)`
   (`0x80088EA8`) (the impact: outcome 5 starts the green heal number, no
   spark), `BtlShowAnimationPlay(blk, b, 0)` (`0x8008A344`) on the last
   flagged object, and `while (BtlAmountShowCheck()) BtlShowFrameStep()`
   (`0x8008931C`, `0x80089DDC`).
3. `BtlShowActingEnd()` (`0x8008A9F8`) (template off, the caster shown and on
   its rest pose), `BtlShowRecordsClear()` (`0x80089BFC`) (the pool cleared).

No vibration script (the spells' `BtlEffectShowPlacesRun` (`0x8007F42C`)
starts `g_BtlActuatorRiseShortList` (`0x800AE370`); this one does not), no screen ramp.
CLUT word 0x780 is row 507's sub-CLUT 8 (Herb, Healing Nut), 0x790 sub-CLUT
9 (Star Light, Silver Light).

**Frames** = 1 (set-up) + the caster's anim 0x10 (`BtlAnimationFramesCount`
(`0x80088DF4`): 37 Alex, 41 Luna, 33 Ramus) + bank 0's anim (94 for anim 6,
110 for anim 7). The heal number lives 81 frames, shorter than either anim,
so the last wait adds nothing. 0x30 and 0x32 come to 128 / 132 / 136
frames (Ramus / Alex / Luna), 0x31 and 0x33 to 144 / 148 / 152.

#### Where the sparkle goes — `g_BtlSlotShowPlaceFlags` in battle

The three tables `g_BtlSlotShowPlaceFlags`, `g_BtlSlotShowPlaceXs` and
`g_BtlSlotShowPlaceYs` are written in battle too, not only by the field
menu's set-up:

* `BtlTargetShowPlaceSet` (`0x80036448`) (btl_target.c): when `g_BtlRunning`
  is not 0, `g_BtlSlotShowPlaceFlags[g_BtlTargetSlot] = 1` and the slot's x
  and y are the target record's `+0x06` and `+0x0A`.
* Its two callers are `BtlTargetAddShowRun` (`0x80032084`), the tail of
  effects 1, 2, 3, 4 and 6 (`g_BtlSlotAmountKinds[t] = 5;
  BtlTargetShowPlaceSet(); BtlShowNumberRun(g_BtlActionShowNumber);`), and
  `BtlPartyHpShareAddRun` (`0x800320C8`) (effect 7), once per healed slot.
* `BtlActionOutcomeClear` clears all fourteen entries (with
  `g_BtlSlotAmountKinds`, `g_BtlSlotFallMarks`, `g_BtlSlotAmounts` (`0x800E48C0`) and, for
  slots 0..4, `g_BtlPartyFallenFlags` (`0x800B8CB8`)) at the start of every
  action (`BtlActionRun` (`0x8002E088`)).

So a restoring effect's presentation always finds the healed slot or slots
flagged at their records' positions, and the sparkle plays on them. The
only readers of `g_BtlSlotShowPlaceXs` and `g_BtlSlotShowPlaceYs` are
`BtlShowPlacesPlay` and `MenuUseShowPlacesPlay` (`0x8007EFC0`): they place
the sparkle, not the number. The heal spells 0x11, 0x12, 0x14, 0x24 and
0x29 reach the same sparkle through `BtlEffectShowPlacesRun` →
`BtlShowPlacesPlay`.

Not every outcome 5 comes with a flagged slot: `g_BtlSlotAmountKinds = 5` is
also written by `BtlPartyRestoreRun` (`0x8003266C`) (effect 14),
`BtlStatRaise` (`0x80033E50`) (the ATC / DEF raises), `BtlTargetHpDrainRun`
(`0x80034770`), `BtlPartyHpDrainRun` (`0x80034108`) (effect 60) and
`BtlRoundStatusUpdate` (`0x800297A4`) (the round's regeneration), none of
which calls `BtlTargetShowPlaceSet`.

With no slot flagged, `BtlShowPlacesPlay`'s `blk` is never assigned. In the
image it is `$s0`, which `BtlActingShowPlacesRun` loaded with `a` (at
`0x8007F520`), so `BtlShowAnimationPlay` would play a "record" at address
0x780 or 0x790. No item path of the game reaches that: every effect behind
0x30..0x33 flags its target.

### 0x34 — Angel's Tear (`BtlShow34Run` (`0x80085928`))

```c
if (g_BtlSlotFallMarks[g_BtlTargetSlot] == 0) { BtlShowActingPlay(0x10, 0); BtlShowActingEnd(); }
BtlEffectCopyTargetStandRun();
```

`BtlEffectCopyTargetStandRun` (`0x8007F94C`) is Miracle Litany's body too:
vibration `g_BtlActuatorRiseShortList`; a 0x28-frame ramp of window 0xC to
(−0x32 × 3); object `g_BtlShowRecords + target * 0x40` is placed on the
target (`BtlRecordPlaceCopy` (`0x80089BD0`)) and set on anim 1 of the
template (the block of resource 455, bound by `BtlEffectLoad` on the
caster); sound 0x7E on sound slot 2 with a wait of 1 and a repeat of 0x28
(it plays on the first frame and every 0x28 after: 8 times);
`BtlAnimationFramesCount` frames of it (305), the target taking its rest
anim `g_BtlPartyStandAnimations[t]` (`0x800B8BEC`) (also stored to
`g_BtlSlotRestAnimations[t]` (`0x800E4940`)) at frame 0xF0; then slot 2's state cleared,
vibration `g_BtlActuatorC8List` (`0x800AE288`), a 0x14-frame ramp back and the wait for it.

**Frames** = 1 + the caster's anim 0x10 + 305 + 20 (359 / 363 / 367 for
Ramus / Alex / Luna). With the fall mark up: 326.

`g_BtlSlotFallMarks[target]` (`0x800E4960`) is the slot's KO mark. An item use
always reads 0 (`BtlActionOutcomeClear` cleared it; `BtlTargetReviveRun` does
not set it), so the caster plays its item anim and goes back to rest first.
The other caller of 0x34 is the auto-revive pass `BtlItem83Revive` (the Angel
Ring, item 0x83, in `worn_items` `+0x26` or `+0x27`; battle.md, "The
auto-revive, beside its callers", under §"The round's pieces"). It picks the
members `BtlFallRun` (`0x800385B4`) marked in `g_BtlPartyFallenFlags`, and
`BtlFallRun` leaves their `g_BtlSlotFallMarks` set, so there the caster does
nothing and the effect starts at once.

`BtlShowOpen` skips its face-off for id 0x34.

### 0x35 / 0x36 / 0x37 — Cleansing Water, Antidote, Holy Water (`BtlShow35Run` (`0x8008597C`))

`BtlShowActingPlay(0x10, 0); BtlTargetSpriteMixRun(0);`: the caster's anim
0x10, then the cleansing glow: vibration `g_BtlActuatorRiseList` (`0x800AE358`), sound 0x33
on sound slot 1, the pulse set up (`BtlShowPulseSet(0xF, 0x28, 0x28, 0x50,
0x14, 0x14, 0x28)` (`0x8008A7F8`)), a 0x14-frame ramp to its first colour
(0x28, 0x28, 0x50), the target's draw word on mix group 2 (`& 0xE3FF |
0x800`), 0xA0 frames with the level stepping down from 0x1F every 20 frames
and back up every 5 from frame 0x8C while the glow colour walks
`g_BtlTargetSpriteMixCols`'s 30 entries; then the pulse off, the word
restored, the vibration and slot 1 stopped, `BtlShowActingEnd()`, a
0x14-frame ramp back and the wait for it.

With `who == 0` the glow is always on the target's record, also when the
caster used the item on itself (Cleanse Litany passes 1 and, on itself,
glows the template, record 20, instead). **Frames** = 1 + the caster's anim
0x10 + 0xA0 + 20 (214 / 218 / 222 for Ramus / Alex / Luna).

### 0x3A / 0x3B / 0x3C — the ranged weapons' shots (`BtlShow3ARun`, `BtlShow3BRun`, `BtlShow3CRun` → `BtlShowRecordTravelRun` (`0x800859A8`))

`BtlShowRecordTravelRun(palette, anim, sound)` with (0x7F0, 2, 0x3C),
(0x790, 3, 0x4F), (0x790, 4, 0x4F):

1. object 0 (`g_BtlShowRecords`) bound to resource 2's bank 2
   (`g_SpriteData2` (`0x800DF370`)) under CLUT word `palette`, anim `anim`
   (`BtlRecordSpriteDataSet` (`0x80089A3C`)); its `attr` (`+0x0C`) = 0x2000
   when the caster's has bit 13, else 0; placed on the caster
   (`BtlRecordPlaceCopy`: x, y, depth = the caster's `+0x08` + 1);
2. `BtlShowSoundSet(0xB, sound, 0, 0)` (`0x80089EF8`): at once, on voice 14;
3. the pan `BtlTravelRun(&req, anim)` (`0x8008AE10`): step 8 (one path point
   per 8 px), mode 0, from the caster's `width` (`+0x3C`) / 2 out and 0xD up,
   to the target's `width` / 4 short of it and its `height` (`+0x3E`) / 4 up
   (at least 0x10). Mode 0 keeps x when the two spans overlap (a vertical
   flight).

Bank 2's anims 2, 3 and 4 are single poses (16 × 8, 8 × 8, 16 × 8); sub-CLUTs
15 and 9 of row 507. No vibration, no ramp, no impact: the hit or the dodge
follows from the swing's caller.

**Frames** = 1 + n, n = max(1, (|dx| + |dy|) / 8). Tempest at (224, 128),
width 24, on a Slime at (136, 128), width 32, height 16: x 212 → 144, y 115
→ 112, n = 71 / 8 = 8, 9 frames.

`BtlTravelPathsMake` (`0x8008AA3C`) (the pan's set-up) turns the target
toward the object (`BtlFacingTowardSet(tgt, disp x)` (`0x80089FC4`)) whenever
the caster is active and free of status 0x3C; the target's own status is
not asked. That is the one thing a shot changes that the rest of the fight
can see: `BtlTargetPushBack` (`0x80037C64`)'s knockback goes the way the
attacker faces, and that enemy's later swing starts from the facing the
shot left.

#### The swing that runs them — `BtlAttackAnimationRun` (`0x800315A8`) (btl_attack.c)

```c
caster depth (+0x08) += 1;
BtlFacingBothSet();                   /* attacker and target face each other */
if (caster < 5) { BtlAnimationSet(caster, g_BtlPartyAttackAnimations[caster]); show = the shot below; }
else BtlAnimationSet(caster, 2);
if (show) BtlAttackShowRun(show);     /* BtlAnimationEndWait(caster); BtlShowNumberRun(show); */
else { SoundPlay(caster attack_sound (+0x19), 3); BtlAnimationEndWait(caster); }
caster depth (+0x08) -= 1;
```

`show` is 0x3A for `worn_item_kind` (`+0x20`) 3, or 5 with
character 2 in the caster's slot (`g_BtlSlotIds` (`0x800E48E0`)); 0x3B or 0x3C when the low byte of
`worn_items[0]` (`+0x22`) is 0x19 or 0x1A (these override 0x3A). With a
presentation the weapon's own swing sound is not played, and
`BtlAttackShowRun` (`0x8003179C`) waits the swing out before the show.

`BtlFacingBothSet` (`0x8003693C`) faces attacker and target at each other
before every plain swing, party or enemy, shot or not. It is not cosmetic:
`BtlTargetPushBack`'s knockback goes the way the attacker faces.

`BtlAttackAnimationRun` is called from `BtlAttackHitShow` (`0x800314B8`) (a
hit) and `BtlAttackMissShow` (`0x80031378`) (a miss), both after the resolver
(`BtlHitTry` (`0x8002FF0C`), and `BtlHitAmountSet` (`0x80030070`) on a hit,
in `BtlActionAttackRun` (`0x80031060`)), so `g_BtlSlotAmountKinds[target]`
already holds 1, 3 or 4, or 2, when `BtlShowOpen` tests it for its face-off.
`BtlShowOpen` tests the target's `g_BtlSlotActiveFlags` (`0x800E49A0`), which still holds
for a target this very swing felled (`BtlFallRun` clears it later).
`BtlShowClose` (`0x8007BACC`) ends every presentation with
`BtlShowActingEnd`, so the attacker is back on its rest pose
(`BtlActingRestAnimationSet` (`0x80037890`)) before the hit's sound and
impact; after a missed shot `BtlTargetMissShow` (`0x80036838`)'s
`BtlAnimationEndWait(caster)` (`0x8002FDBC`) therefore waits on the rest
anim, not on the swing's last step (31 frames against 25 for the dodge of
encounter 254's enemy).

The depth nudge does not come to nothing during a shot: the caster's
`+0x08` is one higher for the swing, so the shot's object, placed at the
caster's depth + 1, sorts at the caster's y + 2.

## Group E — the usable weapons: presentations 0x3D..0x42, item effects 87 / 88

Read from btl_weapon.c, btl_item.c, btl_target.c, btl_hit.c,
btl_show_frame.c, btl_effect.c, btl_row.c, btl_action.c and layer_col.c.

### The seven items

Names are the game's strings (`id | 0x100`, docs/spec/text.md). All seven
carry `target_modes` 0x91 (`& 0xF` = 1, an enemy). An effect number n is
entry n − 1 of `scene_state_table` (`0x800B2EF8`): entry 86 (`0x800B3050`)
is `BtlTargetHpFixedRun` (`0x80034B8C`), effect 87; entry 87 (`0x800B3054`)
is `BtlTargetStatus8ByLevelRun` (`0x80034CBC`), effect 88.

| item | name | effect | presentation | handler | `BtlTargetHpFixedRun` case | `g_BtlActionPower` | `g_BtlActionBitNumber` |
|---|---|---|---|---|---|---|---|
| 20 (0x14) | Ice Mace | 87 | 0x41 | `BtlShow41Run` (`0x800861A4`) | 0 | 0x23 | 3 |
| 21 (0x15) | Water Mace | 87 | 0x42 | `BtlShow42Run` (`0x80086220`) | 1 | 0x28 | 4 |
| 35 (0x23) | Water Cane | 87 | 0x3F | `BtlShow3FRun` (`0x800860B4`) | 15 | 0x19 | 3 |
| 36 (0x24) | Fire Cane | 87 | 0x3E | `BtlShow3ERun` (`0x80086038`) | 16 | 0x19 | 2 |
| 37 (0x25) | Dream Cane | 88 | 0x3D | `BtlShow3DRun` (`0x80085FC0`) | — | — | — |
| 38 (0x26) | Ice Cane | 87 | 0x41 | `BtlShow41Run` | 18 | 0x23 | 3 |
| 39 (0x27) | Ethereal Cane | 87 | 0x40 | `BtlShow40Run` (`0x8008612C`) | 19 | 0x23 | 2 |

No other row of the item table has effect 87 or 88. The party AI stages
six of the seven, never the Ice Cane (battle.md, `BtlPartyWornItemSet`
(`0x80050574`)).

### The action globals for an item below 0x9C

The turn loop `BtlTurnOrderRun` (`0x8002DC30`) zeroes `g_BtlActionBitNumber`
(`0x800B8C10`), `g_BtlAreaHalfHeight` (`0x800B8C30`), `g_BtlAreaHalfWidth`
(`0x800B8C2E`), `g_BtlActionPower` (`0x800B8C2C`), `D_800B8C33`, `D_800B8C32`,
`g_BtlActionFunctionNumber` (`0x800B8C18`) and `g_BtlActionShowNumber`
(`0x800B8C0F`) before every turn. `BtlActionRowRun` (`0x80031ABC`) then loads them from the action's row; for kind 4, an item, it loads only
`function_number` (`+0x08`) and `show_number` (`+0x09`) for an id below 0x9C,
and `power` (`+0x0A`) as well for 0x9C and up. So for the seven weapons the
power, the area half width and height and the bit number are 0 when the
effect starts.

* Effect 87, `BtlTargetHpFixedRun`, writes `g_BtlActionPower` and
  `g_BtlActionBitNumber` itself, by a switch on `g_BtlActionNumber` (`0x800B8C67`) − 0x14
  with six cases (the table above), then reads them. The six cases are
  exactly the six effect-87 items. `g_BtlAreaHalfWidth` and
  `g_BtlAreaHalfHeight` are read by none of `BtlTargetHpFixedRun`,
  `BtlTargetHpAmountTake` (`0x80035FD8`), `BtlTargetTraitCheck`
  (`0x80037648`), `BtlHitAmountTake` (`0x80030650`), `BtlTargetStatus8Clear`
  (`0x80030C28`) or `BtlFallRun`.
* Effect 88, `BtlTargetStatus8ByLevelRun`, reads none of the four, nor do
  `BtlTargetTraitCheck`, `BtlTargetStatus8Set` (`0x8002F958`) or
  `BtlSlotStatusAnimationSet` (`0x8003801C`).
* The presentation handlers read none of them.

The field menu's use (`MenuUseActionSet` (`0x80076590`), menu_use.c) runs
`BtlActionRowRun` with no such zeroing: there the four keep what the last
action left.

### Effect 87 — `BtlTargetHpFixedRun` (`0x80034B8C`) (btl_target.c)

1. `switch (g_BtlActionNumber - 0x14)`: the power and the bit number by item
   (table).
2. `r = rng_mask_below(0xF, 0xA)` (`0x8003A0D4`): one draw; `draw & 0xF`,
   less 10 when that is 10..15, so 0..5 come up twice as often as 6..9.
3. `BtlTargetHpAmountTake(g_BtlActionPower + r)`: the magic damage tail on
   the target with the amount as given. Nothing of the user's record enters
   (no magic stat, no level), and there is no hit check and no evasion. In
   the tail: the element switch on the bit number (2 →
   `BtlTargetTraitCheck(2, 0x2B, 0x2E, 0x30)`, 3 → `(4, 0x2C, 0x2F, 0x30)`,
   4 → `(8, 0x2D, 0x2F, 0x30)`; a resist halves and sets
   `g_BtlSlotAmountKinds` 4, a weakness doubles and sets 3, else 1); trait
   0x32 takes a quarter off; a party target on rest pose 0x13 (defending)
   halves; floor 1, clamp 999; a party target's barrier (`flags` `+0x1E` bit
   0x02) takes it all (kind 6) and is spent; trait 0x33 → 0; trait 0x34 → 0
   as well, because `g_BtlActionKind` (`0x800B8C66`) is 4 here; encounter 0xFA → 0; then the
   applier `BtlHitAmountTake`.
4. `BtlShowNumberRun(g_BtlActionShowNumber)`: the presentation.
5. `BtlTargetStatus8Clear()`: a target holding status 8 is woken (after the
   presentation, so the set-up's face-off still sees it asleep).
6. `g_BtlSlotFallMarks[target]` set → `BtlFallRun()` (the KO clean-up); else
   `BtlAnimationSet(target, g_BtlSlotRestAnimations[target])` (`0x80027208`),
   its rest pose.

Plain amounts: 25..34 (Fire and Water Cane), 35..44 (Ice Cane, Ice Mace,
Ethereal Cane), 40..49 (Water Mace); doubled against a weakness, halved
(a shift right) against a resistance.

### Effect 88 — `BtlTargetStatus8ByLevelRun` (`0x80034CBC`) (btl_target.c)

1. `g_BtlSlotAmountKinds[target] = 2`.
2. Unless the target's `status` (`+0x15`) has a bit of 0x35:
   `BtlTargetTraitCheck(0x400, 0x21, 0x26, 0)`: 1 (resist) leaves 2, 2
   (weak) sets 7, both without a draw; otherwise a chance by the target's
   `level` (`+0x14`): 0x60 below 0x11, 0x32 below 0x16, 0x18 below 0x1B,
   else 0x0C, and `rng_7f_draw()` (`0x8003A0B4`) (one draw, `& 0x7F`) below it
   sets 7.
3. `BtlShowNumberRun(g_BtlActionShowNumber)`.
4. On a 7: `BtlTargetStatus8Set()` (status 8 stored under its own 0x35 guard
   and the same resistance test, its round counter in `g_BtlSlotStatus8Rounds` (`0x800E4930`) = 0,
   its planned kind in `g_BtlSlotPlannedKinds` (`0x800E4910`) = 0xB) and `BtlSlotStatusAnimationSet(target)`
   (the pose).

Nothing of the user's record enters. There is no boss test and no barrier
test (unlike `BtlStatusTakeCheck` (`0x80035378`)). Status 0x08 is not in
0x35, so a sleeper is rolled again and its round counter restarted. Four
enemy rows are weak to 0x400 (ids 9, 13, 21, 93): the Dream Cane puts them to
sleep every time, with no draw, so a party that only uses it against them
keeps them asleep and hurts nobody.

### The presentation handlers

All six ids have row flags 3 (a block and its own CLUT), no background
layer and no set-up clip; resources 0x1C8..0x1CD. None draws from the RNG:
no call they make does, and neither vibration script they start
(`g_BtlActuatorFFList` (`0x800AE258`), `D_800AE2AC`) has a random motion op.

#### Shared pieces

* **`BtlShowActingPlay(0x20, 0)`** opens every one: the user's own record
  plays its anim 0x20 to the end (the template is not shown, no clip is
  cued). `BtlAnimationSet` has no bound on the id: the party block of
  character 11 has 32 anims, so for it the read runs past the table.
* **`BtlMixColNumberSet(id)` (`0x8008B27C`)** (btl_effect.c) is not an
  effect: it maps an id to a 15-bit colour and hands it to
  `sprite_mix_col_set` (`0x8001A904`), the glow colour. 0xB1 → 0x1F (red),
  0xB5 → 0x7C00 (blue), 0x97 → no case → 0x7FFF (white).
* **`layer_mix_fade_start(0x400, 1, 8, lvl)` (`0x8001EBA4`) does nothing**, in
  battle or out of it: the function (layer_col.c) acts on the mask bits 4
  and 8 of its first argument only. **`layer_mix_fade_check(0x400)`
  (`0x8001ED94`) is always 0**, so the two `while` waits after it never spin.
  The handlers therefore turn the glow pass on (`layer_mix_switch_on(0x100)`
  (`0x8001F4B8`)), set its colour and give the target mix group 2
  (`+0x0E & 0xE3FF | 0x800`), but window 2 keeps its level (0xFF, off) until
  the impact's shake sets it (`BtlMixFlashSet(0x400, 2)` (`0x8008A004`) →
  0x14).
* **`BtlAmountShowRun` (`0x8008979C`)** (btl_show_frame.c):
  `BtlAmountShowStart(1)`, the impact, mode 1 (red glow, the nudge, the
  number popup), then `BtlShowFrameStep` frames while `BtlAmountShowCheck`.
* **`g_BtlShowSounds` (`0x800E5588`) is the sound scheduler**: 12 slots of
  {`state`, `sound`, `count`, `repeat`}; `BtlShowSoundsStep` (`0x80089E34`)
  ticks it, and slot k plays on voice k + 3. `BtlShow3ERun` and
  `BtlShow41Run` write the `sound` of slots 0..2 and zero their `count` and
  `repeat`, leaving `state`; `BtlEffectCopyAboveShotRun` (`0x80085D2C`) then
  sets one slot's `state` to 1 at the start of each of its three stages,
  which the next frame plays once and clears. This relies on the three
  slots being idle when the handler starts.

#### `BtlEffectCopyTargetPlay(effect, elem, dy)` (`0x80085B74`) (btl_item.c)

The user's anim 0x20; object 0 = a copy of the template, on, draw word
0x2200 (the block's own CLUT), placed on the target (`BtlRecordPlaceCopy`:
x, `+0x0A`, `+0x08` = the target's + 1) with `+0x0A += dy`; the colour of
`effect`, the glow on, the (inert) window call, the target's mix group;
`BtlShowSoundSet(1, elem, 0, 0)`: sound `elem` at once on voice 4; the
object plays anim 0 to its end; the mix group and the glow off. No
`BtlShowActingEnd` (the user stays on anim 0x20's last step until the
teardown) and no impact.

#### `BtlEffectCopyAboveShotRun(effect)` (`0x80085D2C`) (btl_item.c)

The user's anim 0x20, `BtlShowActingEnd` (the user back on its rest pose);
object 0 = a copy of the template, draw word 0x2200, on, at the template's x
(the user's) with `+0x0A` = the user's − (its height + 0x10) and `+0x08` =
the user's + 1. Stage 1: slot 0's sound, anim 0 to its end. Stage 2: slot
1's sound, `+0x0A` back to the user's, `BtlTravelRun` (mode 0, one point per
16 px, from 0 across and −(height + 0x10) up to 0 across and −(target height
/ 4) up, held to −0x10) on anim 1. Stage 3: the colour, the glow, the
(inert) window call, vibration script `g_BtlActuatorFFList`, the target's
mix group, `+0x08` = the target's + 1, slot 2's sound, anim 2 to its end; the
mix group and the glow off; `BtlAmountShowRun`.

#### The six handlers

| id | handler | what it does |
|---|---|---|
| 0x3D | `BtlShow3DRun` (`0x80085FC0`) | `g_BtlActuatorFFList`; `BtlEffectCopyTargetPlay(0x97, 0x68, −(h / 4) held to −0x10)`: white, sound 0x68. Both arms of its `if (h >> 2)` make the same call. No impact (effect 88 shows no number). |
| 0x3E | `BtlShow3ERun` (`0x80086038`) | `g_BtlActuatorFFList`; slots 0..2 = sounds 0x6E, 0x51, 0x5A; `BtlEffectCopyAboveShotRun(0xB1)`: red. |
| 0x3F | `BtlShow3FRun` (`0x800860B4`) | `D_800AE2AC`; `BtlEffectCopyTargetPlay(0xB5, 0x6D, 0)`: blue, sound 0x6D; `g_BtlActuatorFFList`; `BtlAmountShowRun`. |
| 0x40 | `BtlShow40Run` (`0x8008612C`) | `D_800AE2AC`; `BtlEffectCopyTargetPlay(0xB1, 0x71, 0)`: red, sound 0x71; `g_BtlActuatorFFList`; `BtlAmountShowRun`. |
| 0x41 | `BtlShow41Run` (`0x800861A4`) | `g_BtlActuatorFFList`; slots 0..2 = sounds 0x7B, 0x79, 0x7A; `BtlEffectCopyAboveShotRun(0xB5)`: blue. |
| 0x42 | `BtlShow42Run` (`0x80086220`) | below |

**`BtlShow42Run`** (btl_weapon.c): the user's anim 0x20 and
`BtlShowActingEnd`; `dy` = max(target height / 4, 0x10); the template's draw
word = 0x2200; objects 1 and 0 = copies of it, both on: x = target x − 0x2A
− dy (object 0, `attr` 0x2020, mirrored) and target x + 0x2A + dy (object
1, `attr` 0x20), `+0x0A` = the target's `+0x0A` − dy, `+0x08` = the target's
`+0x0A` + 1. `BtlShowSoundSet(1, 0x2E, 0, 0)` (voice 4, at once); both on
anim 0; for each frame i of anim 0's length: the target's x = sx + 2 (even i)
or sx − 2 (odd i) unless `g_BtlSlotIds[target]` is 0x77, and when `i & 7` is
0 the objects close 1 px each. Then the colour of 0x97 (white), the glow,
the (inert) window call, the target's mix group, `BtlShowSoundStop(0)`
(`0x80089F78`) (it stops slot 0, voice 3, not slot 1 where 0x2E was
started), `BtlShowSoundSet(1, 0x5D, 0, 0)`, `g_BtlActuatorFFList`; both on
anim 1; for each frame i of its length: 4 px closer each while i < 8, the
same shake. The target's x back to sx; the mix group and the glow off;
`BtlAmountShowRun`.

#### Frame counts

With `U` = `BtlAnimationFramesCount` of the user's anim 0x20 (its ticks − 1),
`A_k` the same for the block's anim k, `P` the number popup's length (62 for
a hit: anim 1 of the popup table, 63 ticks), and 1 for the set-up's frame:

| id | frames | block anim ticks |
|---|---|---|
| 0x3D | 1 + U + A0 | 71 |
| 0x3E | 1 + U + A0 + n + A2 + P | 62, (16), 31 |
| 0x3F | 1 + U + A0 + P | 85 |
| 0x40 | 1 + U + A0 + P | 117 |
| 0x41 | 1 + U + A0 + n + A2 + P | 82, (21), 33 |
| 0x42 | 1 + U + A0 + A1 + P | 86, 18 |

`n` = (|dx| + |dy|) / 16 of the pan, 1 at least. For Alex (anim 0x20: 30
ticks; 24 × 23) at a Slime at (136, 128) (32 × 16): 0x3E 189 = 1 + 29 + 61 +
6 + 30 + 62 (n = (88 + 23) / 16); 0x3F 176; 0x40 208; 0x41 211; 0x42 194;
0x3D 100.

## Group F — the boss breaths 0x50..0x53 and effect 93

Read from btl_boss.c, btl_target.c, btl_slot.c and btl_plan.c, and, where a
register matters, from the program's instructions. The breaths' shared
sweep and its long form are on battle.md, §"The breath attacks' party
sweep — `BtlPartyRightMove` (`0x80086700`)" and §"The breath sweep's long
form — `BtlPartyRightMovePatternsRun` (`0x80086B64`)", with their steps, the
start of the sweep's `t` for each breath and the vibration draws (frame-rng.md
§"Draw site 2 — the vibration driver `pad_update` (`0x8001F508`)"). This group
adds what the sweep does to positions, frames and sounds, and the breaths'
own handlers.

### Effect 93 — `BtlActingRowsHpRun` (`0x80035164`) (btl_target.c)

Attack row 94, "Cold Breath", is its only user (no spell row, no item, no
other attack row has effect 93). The row: target mode 2, power 2, area half
width 0, area half height 2, bit number 3, show 0x52.

It has its own target-set loop, then `BtlMarkedStat0EHitRun()`
(`0x80032FA0`):

1. `shift = g_BtlAreaHalfHeight << 3`.
2. The centre is the caster's record (`g_BtlRecords` (`0x800B8C58`) + `g_BtlActingSlot` × 0x40), its y (`+0x0A`) read as u16.
3. `min_y = (s16)(y − (shift + 8))`, `max_y = (s16)(y + shift)`.
4. For slots 0, 1, 2, 3: if `g_BtlSlotActiveFlags[slot]` is set and the
   slot's y (s16) has `min_y <= y0 <= max_y` (both ends inclusive),
   `g_BtlSlotAmountKinds[slot] = 1`.

So: a band of rows around the caster, y − (half height · 8 + 8) .. y + half
height · 8, for Cold Breath y − 24 .. y + 16. No reach check, no x test.

It is not `BtlTargetRowsMark` (`0x80035260`) (effect 94) with another centre:

| | `BtlActingRowsHpRun` (93) | `BtlTargetRowsMark` (94) |
|---|---|---|
| centre | the caster (`g_BtlActingSlot` (`0x800B8C26`)) | the target (`g_BtlTargetSlot` (`0x800B8C5D`)) |
| slots | 0..3, always | 6..13 for a party actor, 0..4 otherwise, 0..13 under status 0x40 |
| reads the caster's side or status | no | yes |

Consequences: party slot 4 is never hit, an enemy never is, the caster's
confusion changes nothing, and the planned target (`g_BtlTargetSlot`) is
not read by the effect at all; it only reaches `BtlShowOpen`'s facing.

The Black Dragon's re-check (`BtlEnemy77ActionSet(0)` (`0x800424A8`),
btl_plan.c) tests the same band with the constants spelled out (`base −
0x18`, `base + 0x10`, slots 0..3, active): class 0x5E becomes 0x5D unless
some slot is in it (boss-ai.md). So a Cold Breath that runs always has at
least one flagged slot.

`BtlMarkedStat0EHitRun` is the worker effects 25 and 26 use too: the magic
formula `BtlStat0EHitAmountSet` (`0x80035EF8`) on each flagged slot in slot
order, then `BtlShowNumberRun(g_BtlActionShowNumber)`,
`BtlActingRestAnimationSet`, `BtlFallRun`, `BtlMarkedRestAnimationSet`
(`0x80030DA0`), `BtlMarkedStatus8Clear` (`0x80030B8C`).

### What the sweep does to positions, frames and sounds

The sweep's start value `t` decides more than the palette phase and the
first shake direction: phase 2 tests the slide thresholds (0x7E, 0x9E,
0x11E) on the x the shake has just moved, so where a record comes to rest
depends on its parity. Worked through from the C for every start x in
40..299, 194 of 260 end 1 to 3 px apart between an even and an odd start
(112 → 212 even, 210 odd; 224 → 288 even, 286 odd). All three real starts
are even, so there is one answer.

The party is not put back: `BtlSlotActionAnimationSet` (`0x80037E64`) only
sets a pose. Phases 1 and 3 run 0x20 and 0x28 passes, both even, so their ±1
shake nets 0; phase 2's 72 passes slide. With an even start: 112 → 212, 120
→ 214, 144 → 224, 168 → 240, 192 → 264, 212 → 284, 224 → 288, 284 → 288; a
record at 288 or beyond is only shaken (296 → 296). All five party records
move, fallen ones included. So after a breath the party stands up to 100 px
further right, and the next round's walks start from there.

Frames: `BtlAnimationFramesCount` of the caster's anim 0xE
(`BtlShowAnimationPlay` runs that many; 91 for the Black Dragon, whose anim
0xE is 92 ticks), 1 for the layer's frame, 0x20 + 0x48 + 0x28 = 144 passes:
236 for the Black Dragon.

Sounds, from `BtlShowSoundSet(0xA, 0x77, 0x18, 0x5C)` and `(0xB, 0x60, 0x30,
0x20)`: 0x77 on voice 13 at sweep frames 24, 116 and 208; 0x60 on voice 14
at 48, 80, 112, 144, 176 and 208. The close clears both slots'
`state` (`g_BtlShowSounds[11]`, then `[10]`) without stopping the voices.

The palette: `mem_move_16(pal, &cols[t & 0xF], 0x20)` (`0x80026178`) each
pass copies 16 colours onto `g_PaletteBuf + 0x220` (`g_PaletteBuf`
(`0x800B8ADC`)), colour-buffer entries 0x110..0x11F. Which graphics draw from
that row is not known.

### The long form, in frames

The callees the long form uses: `BtlTilePatternPut(f, n)` (`0x80089C8C`)
shows layer frame f − 1 and waits n frames (nothing, and no wait, when f is
0); `BtlTilePatternStep(i, n)` (`0x8008B21C`) is `BtlTilePatternPut` on entry
i of the list `g_BtlTilePatternList` (`0x800B8FF8`) points at, wrapping to 0
at 0xFF, returning i + 1; `BtlTileScrollSet(8, x, y)` (`0x8008A468`) sets the
layer's scroll (its first argument is unused). The pulse's first colour is
`g_BtlShowPulse.colour_1` (`g_BtlShowPulse` (`0x800E5530`), `+0x0C`) and its
switch `g_BtlShowPulse.on` (`+0x00`).

Step 7's second loop runs zero passes for both breaths: the pulse (2 frames
a ramp) has just finished a ramp when the 0x20 steps end (136 and 152 frames
after the 0x10-frame ramp started, an even count past it), and
`layer_mix_fade_check(8)` is idle (`layer_mix_set(8, 8)` (`0x8001ECA0`) sets
the level; it starts no ramp).

Step 8's loop steps 4 frames a pass until the impact's clock has passed its
popup (62 frames with a number popup: 16 passes, 64 frames; the 0x20- and
0x10-frame ramps are over by then).

What the layer shows, from the sequence (entries 1..10 for both): entry 1
during the sweep; after the list pointer steps, entry 2 for 1 + 4 frames;
then for Thunder Breath entry 3 (4 frames) and a loop over entries 4..10,
for Cold Breath entries 3..7 and a loop over 8, 9 and 10.

Sounds (`BtlShowSoundSet(slot, id, 1, repeat)`: first on the next frame,
then every `repeat`): Thunder Breath 0x73 on voice 4 every 0x1C frames (5
times), 0x74 on voice 5 once, id 0 on voices 6 (once) and 7 (every 0x24
frames); Cold Breath id 0 on voices 4 (every 0x1C) and 5 (once), 0x79 on
voice 6 once, 0x51 on voice 7 every 0x24 (5 times). A slot given id 0 is not
skipped: `BtlShowSoundsStep` hands `SoundPlay(0, voice)` (`0x800555F0`) to
the sound driver on every repeat.

### 0x50 Fire Breath — `BtlShow50Run` (`0x80086E84`)

In order: the sweep; the target's record gives `n = (|x − 0x8C| + |y0 −
0x48|) / 8` (s16, at least 1), read after the sweep moved the target, and a
line of n points from (0x8C, 0x48) to it (`line_path_make` (`0x80054C9C`));
`BtlShowPulseSet(4, 0x80, 0x60, 0, 0x40, 0x30, 0x30)`; vibration
`g_BtlActuatorFFList`; sound 0x6E on sound slot 0xB after 0x10 frames; the
caster's anim 2 to its end; the screen ramps to the pulse's first colour
over 0x10 frames and the pulse goes on. Then `n + 0x28` frames, each
visiting all 20 objects with `j = frame − 2i`:

* `j == 0`: object i becomes a copy of the caster's record, on, its depth the
  target's y + 1, anim 0x11;
* `0 <= j < n`: anim 0x10 at j == 8, 0xF at j == 16; x and y from the line;
* `j == n`: vibration `g_BtlActuatorFFList` again, sound 0x70 on slot i % 10,
  anim 0x12;
* `j == n + 0x10`: off.

With the vibration option on (`g_GameOptions[5]`), every visit also writes
`g_PadActuatorLevels` (`0x800B8698`) = `i >= 6` and `g_PadActuator1Level`
(`0x800B8699`) = `i * 0xFF / 20`, so each frame leaves (1, 242).

The loop is `n + 0x28` frames long and copy i lands at `2i + n`, so all 20
land (20 × sound 0x70, voices 3..12 twice) but only copies 0..11 are switched
off by the loop: 12..19 would be due at or after its last frame and stay on
their held anim 0x12 until `BtlShowClose` clears the pool. Then the impact
(mode 1), the pulse off, slot 0xB stopped, the line freed, the running ramp
waited out, a 0x10-frame ramp to neutral, `BtlAmountShowCheck` and the ramp
waited out, `BtlActingRestAnimationSet`, the layer blanked, `BtlTileEnd`
(`0x8008A428`).

The copies are the caster's own block: anims 0xF..0x12 of the Black
Dragon's sheet are the fireball (12, 12, 12 and 46 ticks, the last held).

Frames for the Black Dragon: 1 (set-up) + 236 + 51 (anim 2 is 52 ticks) + n
+ 0x28 + 62 (the impact's popup) = 390 + n.

### 0x51 Thunder Breath — `BtlShow51Run` (`0x80087384`), 0x52 Cold Breath — `BtlShow52Run` (`0x80087408`)

`BtlShow51Run`: `BtlShowPulseSet(2, 0x78, 0x78, 0, 0x3C, 0x3C, 0)`,
`BtlPartyRightMovePatternsRun(0, 2, 0x73, 0x74, 0, 0)`, the caster's anim
0xA to its end, `BtlActingRestAnimationSet`. Attack rows 93 and 126 (the
Black Dragon in encounter 0xF9) share it. 531 frames for the Black Dragon: 1
+ 236 + 51 + 3 (anims 8 and 9: 52 and 4 ticks) + 1 + 8 + 128 + 0 + 64 + 39
(anim 0xA: 40 ticks).

`BtlShow52Run`: `BtlShowPulseSet(2, 0, 0, 0xA0, 0, 0, 0x50)`,
`BtlPartyRightMovePatternsRun(1, 6, 0, 0, 0x79, 0x51)`,
`BtlActingRestAnimationSet`. 508 frames: 1 + 236 + 51 + 3 + 1 + 24 + 128 + 0
+ 64.

### 0x53 Arch Flare — `BtlShow53Run` (`0x80087470`)

Attack row 110, Royce's (enemy 0x7A, encounter 0xF5); set-up clip 58. No
sweep (the party is not moved) and no vibration script but
`g_BtlActuatorFFList`. In order: `g_BtlShowVoiceState` (`0x800B8FE8`) = 1
(the set-up clip starts on the next frame); the caster's anim 0x10 to its
end; `BtlActingRestAnimationSet`; 0x10 battle frames (`BtlFrameWait`
(`0x80036BA8`), no display tick); vibration `g_BtlActuatorFFList`; sound
0x5A on slot 1; `layer_mix_set(8, 8)`, `layer_col_set(0, 0xC, 0)`
(`0x8001EB74`), a 0xC-frame ramp to (0xC0, 0x48, 0); the sequence's first
six entries, 4 frames each, the list pointer stepping; sequence steps while
a ramp runs (none does); a 4-frame ramp to (0x80, 0x30, 0) and sequence
steps while it runs (one); sounds 0x5B and 0x5C on slots 1 and 2; the layer
and the screen ramped back over 0x80 frames; the impact (mode 1); sequence
steps while `BtlAmountShowCheck`, the layer ramp or the screen ramp is busy
(32 steps); the layer blanked, `BtlTileEnd`. No RNG draw.

Frames with Royce's sheet (anim 0x10 is 73 ticks): 1 + 72 + 16 + 24 + 0 + 4
+ 128 = 245. The layer loops entries 7..10 after the first six.

## Group G — presentations 0x54..0x56, effect 40, encounter 0xF6's entry

Read from btl_boss.c (`BtlShow54Run`, `BtlShow55Run`, `BtlShow56Run`),
btl_effect.c, btl_show_frame.c and layer_mix.c (their workers), btl_target.c
(`BtlFrontStat0EHitStatus10Run` (`0x800337A0`), `BtlTargetFrontMark`
(`0x8003568C`), `BtlMarkedStat0EHitRun`), btl_move.c (`BtlWalkEdgeRun`
(`0x80036654`)), btl_status.c (`BtlTargetStatus10Set` (`0x8002FA00`)),
btl_init.c (`BtlSpecialInit` (`0x8002A7FC`)) and btl_run.c (`BtlRun`,
`BtlSlot6AnimationAlternate` (`0x80027934`)).

### What the three handlers share

* The layer's sequence is stepped with `BtlTilePatternStep(i, 4)`: entry `i`
  (wrapping to 0 at the 0xFF end) shown for four frames, `i + 1` returned.
  Every loop of the three handlers is built on that call, so their counters
  count passes of 4 frames, not frames.
* 0x55 and 0x56 copy the 16 colours at `g_PaletteBuf + 0x220` (row 1 of the
  colour buffer, entries 0x10..0x1F) twice into a local `cols[32]` and, once
  a pass, write `cols[frame & 0xF .. +16]` back (`mem_move_16(pal,
  &cols[frame & 0xF], 0x20)`, `frame++`): a 16-step rotation, one step every
  4 frames. Whose CLUT those 16 entries are is not known (the layer's tiles
  are not decoded).
* 0x55 and 0x56 start the layer's raster wave `BtlShowWaveSet(8, 1, 1, 1,
  g_BtlShow55WaveFactors)` (`0x8008A4F8`): mode 8 (the background layer,
  `tile_wave_switch_set(1)` (`0x8001D304`)), every frame, ramping up over 32
  steps; under `layer_col_fade_start(0, 0xC, 1, 0x10, (−0x3C, −0x3C,
  −0x3C))` (`0x8001EA74`), the layer scrolled to (0x20, 0)
  (`BtlTileScrollSet(8, 0x20, 0)`) and brought in with
  `layer_mix_fade_start(8, 1, 0x20, 0x10)`. `g_BtlShow55WaveFactors`
  (`0x800B5C3C`) is 16 s32, the table after Inferno's
  `g_BtlShow2AWaveFactors` (`0x800B5BFC`): 0, 0x30000, 0x50000, 0x70000,
  0x80000, 0x70000, 0x50000, 0x30000, 0 and the same negated;
  `BtlShowWaveSet` indexes it as `tbl[(i + ph) & 0xF]`.
* None of the three calls `rng_draw` (`0x8001C770`) or `rng_mask_below`
  except where stated below; their vibration scripts
  (`g_BtlActuatorFFTwiceList` (`0x800AE268`), `g_BtlActuatorFFList`, `g_BtlActuatorRiseList`)
  have no random motion.

### 0x54 — `BtlShow54Run` (`0x800876E4`), Chaos Wave (attack row 113; row flags 7, set-up clip 59)

In order:

1. `g_BtlShowVoiceState = 1` (the set-up clip starts on the first frame);
   the caster plays its anim 0xD to its end (`BtlShowAnimationPlay`) and is
   put on 0xE.
2. Vibration `g_BtlActuatorFFTwiceList`; sounds `(1, 0x72, 0, 0)` and `(2,
   0x73, 0x11, 0)` (the second 0x11 frames later); window 8 straight to
   level 8 (`layer_mix_set(8, 8)`), the ramp zeroed, the pulse
   `BtlShowPulseSet(8, 0x70, 0x80, 0, 0x38, 0x40, 0)` and a 0x18-frame ramp
   to its first colour, the pulse on.
3. Six sequence entries, 4 frames each (`BtlTilePatternPut(*g_BtlTilePatternList++,
   4)`), and after each one a scatter point: `x = rng_mask_below(0x3F, 0x30)
   + 0x10`, then `y = rng_mask_below(0x1F, 0x1F) + 0x70`.
4. Passes of `BtlTilePatternStep(seed, 4)`, on the sequence from its seventh
   entry: one more point a pass until there are 20, and on while window 8
   or the ramp still runs. With the pulse's 8-frame ramps this ends exactly
   at the twentieth point: 14 passes.
5. Vibration `g_BtlActuatorFFList`. The template (`+0x500`) on with draw word
   0x200 (the block's own CLUT, no Y-sort bit); objects 0..19 are copies of
   it: object `i` with `i < 6` and `g_BtlSlotAmountKinds[i] == 1` sits on
   battle record `i` (`BtlRecordPlaceCopy`) on anim 0; every other one at
   `(pts[i].x + (i % 6) * 0x30, pts[i].y + (i % 2) * 0x20)`, depth y + 1, on
   anim 1 (and restarts `g_BtlActuatorFFList`). The template off again. The
   test is `== 1`: a party slot whose outcome is 3 (weak), 4 (resisted) or 6
   (barrier) gets a scattered block, not one on the member. Chaos Wave
   carries bit number 1: a party member holding one of the statuses
   `BtlTargetTraitCheck(1, 0x2A, 0x2E, 0x30)` matches comes out 4, and a
   barrier 6 (for a party target that function answers 0 or 1 only, so 3 is
   an enemy target's).
6. Sounds `(3, 0x74, 0, 0)` and `(4, 0x75, 0, 0x1E)` (a wait of 0: played at
   once, the repeat unused); the pulse cleared; the impact
   `BtlAmountShowStart(1)`; `n = BtlAnimationFramesCount(object 0)` passes,
   and on while window 8 or the ramp runs.
7. All 20 objects off; `layer_mix_fade_start(8, 1, 0x40, 0x1F)` and
   `layer_col_fade_start(0, 0xC, 1, 0x40, 0)`; `g_BtlActuatorFFList` again;
   passes while `BtlAmountShowCheck()` or either ramp runs;
   `g_BtlShowSounds[4].state = 0` (sound slot 4's schedule),
   `BtlActingRestAnimationSet`, the layer blanked, `BtlTileEnd`.

**RNG:** exactly 40 draws, 20 points, two `rng_mask_below` each, x before y;
12 of them during step 3, 28 during step 4.

**Length:** 1 (set-up) + (anim 0xD's `BtlAnimationFramesCount`) + 24 + 56 +
4n + 64. For Xenobia (anim 0xD: 81 ticks → 80 frames) and the block's anims
0 and 1 (both 31 ticks → n = 30): 1 + 80 + 24 + 56 + 120 + 64 = 345 frames.

### 0x55 — `BtlShow55Run` (`0x80087C4C`), Siphon Soul (attack row 102; row flags 4: no block, set-up clip 56)

Nothing here is drawn from an effect block: the "souls" are copies of the
caster's own battle record on its anim 0x17.

1. Vibration `g_BtlActuatorRiseList`; the wave and the (−0x3C) ramp (above).
   A 0x280-byte buffer: 5 × 16 points, then (`buf2 = buf + 0x50` points) 5 ×
   16 more.
2. The caster's draw word `+0x0E &= 0x9FFF`. For each party slot `i` of 0..4
   whose `g_BtlSlotAmountKinds[i]` is 1, 3 or 4: display object `i` = the
   caster's whole record (after that mask), off (`+0x00 = 0`), anim 0x17; the
   member turns to the caster's x (`BtlFacingTowardSet`); `buf[i]` =
   `arc_path_make` (`0x8008B0B4`) from (member x, member y − h/2) to (caster
   x, caster y − h/2), 16 points, height 0x48; `buf2[i]` = `line_path_make`
   from the member's (x, y) to the caster's, 16 points; the object placed at
   the member's x, y − h/2, depth = the member's `+0x08` + 1. `flag = 1`.
3. `g_BtlSlotAmountKinds[6]` and `g_BtlSlotAmounts[6]` are saved and zeroed:
   slot 6, not "the caster". The caster plays 0x10 to its end; sound `(1,
   0x55, 1, 0x24)` (every 0x24 frames); the colours copied;
   `layer_mix_fade_start(8, 1, 0x20, 0x10)`; scroll (0x20, 0);
   `g_BtlShowVoiceState = 1`; the caster plays 0x11 to its end.
4. One state a pass:
   * 1: once window 8 is still, the struck slots' objects on; sound `(2,
     0x22, 1, 0x40)` if there was any.
   * 2: 0x20 passes, then the impact `BtlAmountShowStart(1)` (the party's
     numbers; slot 6 shows none, its outcome being 0).
   * 3: 0x10 passes: every object 0..4 takes `buf[i][cnt]` as (x, y) and
     `buf2[i][cnt].y + 1` as depth (all five are written; an unstruck slot's
     points are the buffer's unwritten words, in an object that is off), and
     `BtlAmountShowCheck()` is called each pass. Then, on the first pass it
     answers 0: the buffer freed, `g_BtlSlotAmounts[0..4]` and
     `g_BtlSlotAmountKinds[0..4]` zeroed, slot 6's two values restored, the
     impact again (slot 6's own number), `g_BtlShowSounds[2].state = 0`
     (sound slot 2), sound `(4, 0x5D, 0, 0)`.
   * 4: passes while `cnt < 0x20` and the impact runs; then
     `g_BtlShowSounds[1].state = 0` (slot 1), the caster on 0x12, the five
     objects off.
   * 5: 0x10 passes, then `BtlActingRestAnimationSet`,
     `layer_mix_fade_start(8, 1, 0x30, 0x1F)`, `layer_col_fade_start(0, 0xC,
     1, 0x30, 0)`.
   * 6: until both are done.
5. The caster's draw word `(+0x0E & 0x9FFF) | 0x2000`; the layer blanked;
   `BtlTileEnd`. The vibration script is left to run out.

**The handler leaves `g_BtlSlotAmountKinds[0..4]` and `g_BtlSlotAmounts[0..4]`
at 0.** Whatever effect 60's handler does after `BtlShowNumberRun` sees no
party slot flagged by its number kind; its tail `BtlFallRun` works from
`g_BtlSlotFallMarks`, which this handler does not touch (boss-ai.md, effect
60).

`arc_path_make` makes the line first and then lifts it: the lift computed
for parameter i (1 to n − 1) is added to point i − 1, and the last point is
not moved. Its other callers are Rock'N'Roll (`BtlShow2DRun` (`0x800845C4`))
and Flash Arrow (`BtlShow2ERun` (`0x80084E78`)).

**RNG:** none. **Length** with three struck members, slot 6 on outcome 5, the
Emperor's animations and popups of 62 and 81 frames: 1 + 74 + 8 + 28 + 132 +
68 + 84 + 68 + 48 = 511 frames; states 1, 4 and 6 depend on the ramps and on
the impact's length.

### 0x56 — `BtlShow56Run` (`0x800883F8`), Fate Storm (attack row 104; row flags 7, set-up clip 56)

1. `hit[5]` zeroed; vibration `g_BtlActuatorRiseList`; sound `(1, 0x68, 0,
   0)`; the caster on anim 8 and battle record 7 (`g_BtlRecords + 0x1C0`) on
   0x15; `BtlAnimationEndWait(caster)`; **`BtlWalkEdgeRun(1)`, the lane
   stance's walk**; the caster and record 7 turn to the target's x; sound
   0x68 again; record 7 takes the caster's place (`BtlRecordPlaceCopy`); the
   caster on 9, record 7 on 0x16; `BtlAnimationEndWait(caster)`; record 7 on
   0xB. This is `BtlTargetFrontMark`'s enemy-0x7C arm, written out again
   here.
2. The wave and the (−0x3C) ramp; the template's draw word 0x200, placed on
   the caster (`BtlRecordPlaceCopy`), turned to the target; sound `(1, 0x55,
   1, 0x24)`; `BtlShowActingPlay(0, 0)`: the caster hidden, the template plays
   its anim 0 through; `g_BtlShowVoiceState = 1`; `BtlAnimationSetCopy(object
   0, 1)` (`0x8008B1FC`) (on a record nothing has bound yet; state 3
   overwrites it).
3. For party slots 0..4: an active slot (`g_BtlSlotActiveFlags`) on the side
   the caster faces (caster `+0x0C & 0x2000`: `caster.x > slot.x`; else
   `caster.x < slot.x`) is marked; a marked slot turns to the template's x and
   takes anim 0x13.
4. `cnt = (|target.x − template.x| + |target.y0 − template.y0|) / 8`; a
   `cnt`-point line from the target to the template, which step 2 put on the
   caster; its second point (`path[2]`, `path[3]`) is kept, and a 2-point line
   from the target to that point replaces it. So the target is drawn toward
   the caster, 2 of `cnt` points (16 px of the |dx| + |dy| distance). With
   `cnt < 2` the read is past the block.
5. Scroll (0x20, 0); the colours copied; `layer_mix_fade_start(8, 1, 0x20,
   0x10)`. One state a pass:
   * 1: until window 8 is still.
   * 2: 0x20 passes: with the vibration option on (`g_GameOptions[5]`
     (`0x800B8968`)) `g_PadActuator1Level += rng_draw() & 0x3F`; every marked
     slot's x −2 on an odd pass, +2 on an even one.
   * 3: 2 passes: (vibration option: `g_PadActuator1Level = 0xFF`,
     `g_PadActuatorLevels = 1`;) the target's x and y = the short path's
     point. Then both levels 0, the path freed, object 0 = a copy of the
     template (record 20), placed on the target, anim 2; sound `(2, 0x6C, 1,
     0x40)`.
   * 4: 0x20 passes (`g_PadActuator1Level += 3` with the option); then object
     0 on anim 3, `g_BtlShowSounds[2].state = 0`, sound `(3, 0x66, 0, 0)`.
   * 5: 0x18 passes, each drawing as state 2 does with the option on; then
     the template on anim 1, `BtlShowActingEnd` (template off, caster back and
     at rest), `layer_mix_fade_start(8, 1, 0x40, 0x1F)`,
     `layer_col_fade_start(0, 0xC, 1, 0x40, 0)`, `g_BtlShowSounds[1].state =
     0`.
   * 6: both levels 0 each pass; until both ramps are done.
6. The vibration stopped (pointer, flags, both levels); the layer blanked;
   `BtlTileEnd`; and when `g_BtlSlotAmountKinds[target] == 6` (the barrier
   took it) `BtlAmountShowRun()`, the impact, waited out.

The 16 colours rotate one step a pass (0x20 bytes), and the shake, flash
and flicker of states 2, 4 and 5 last 0x20, 0x20 and 0x18 passes of four
frames each.

**RNG:** none with the vibration option off; with it on, 56 draws: 0x20 in
state 2 and 0x18 in state 5, one a pass.

**Length**, not counting the lane walk's own frames: 1 + 26 + 18 + 143 + 36
+ 132 + 12 + 132 + 100 + 64 = 664 frames, + the barrier's impact (79 frames)
when the target's outcome is 6.

### Effect 40 — `BtlFrontStat0EHitStatus10Run` (`0x800337A0`) (attack row 111 "Sonic Boom", the only row with it)

Row: power 5, area half width 1, area half height 2, bit number 0, show
0xB9 (the enemy action `BtlShowB9Run` (`0x800401D8`)).

```c
BtlTargetFrontMark(0);         /* the lane */
BtlMarkedStat0EHitRun();       /* the magic formula on each flagged slot, the presentation,
                                  BtlActingRestAnimationSet, the KO tail,
                                  BtlMarkedRestAnimationSet, BtlMarkedStatus8Clear */
for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++)
    if (g_BtlSlotAmountKinds[g_BtlTargetSlot] == 1
        && !(CharStats[g_BtlTargetSlot].status & 0x30)
        && g_BtlSlotFallMarks[g_BtlTargetSlot] == 0) {
        BtlTargetStatus10Set();
        BtlSlotStatusAnimationSet(g_BtlTargetSlot);
    }
```

* `BtlTargetFrontMark` is called with 0 whatever the caster is (party slots
  0..4), where effects 20 and 26 pass 2 for a confused caster and 1 for a
  party one. Its stance (area half width not 0 → `BtlWalkEdgeRun`, walk anim
  1 for Xenobia), face-off and band (`y − (half height · 8 + 8) .. y + half
  height · 8` around the caster, beyond its leading edge) are effect 26's.
* The three guards, in this order: the outcome is exactly 1 (not 3, 4 or the
  barrier's 6); the status byte holds neither 0x10 nor 0x20 (read after
  `BtlMarkedStatus8Clear`'s wakes); the hit did not KO the slot.
* `BtlTargetStatus10Set` (btl_status.c): `if (BtlTargetTraitCheck(0x800, 0x25,
  0x26, 0) != 1)`, `status` (`+0x15`) = 0x10 stored outright (no guard of its
  own: 0x01, 0x04, 0x40 and 0x80 are overwritten), the slot's
  `g_BtlSlotStatus80Rounds` (`0x800E49E0`), `g_BtlSlotStatus40Rounds` (`0x800E49B0`),
  `g_BtlSlotStatus8Rounds` and `g_BtlSlotStatus4Rounds` (`0x800E48B0`) = 0, and for a party
  slot `flags` (`+0x1E`) `&= 0xFB`. No draw.
* `BtlSlotStatusAnimationSet` re-poses the slot whether or not the status was
  stored.
* The loop runs on `g_BtlTargetSlot` itself and leaves it at 5.

### Encounter 0xF6's entry

The entry of encounter 0xF6 (Xenobia) has two pieces:

1. `BtlSpecialInit` case 0xF6: `BtlAnimationSet(actor 6, 0x11)`, then
   `BtlEnemyLevelSet(0xD2, 4, 3, 1, 4, 2, 0x2B)` (`0x8002ADF0`) and actor 6's
   `width` (`+0x3C`) `-= 0x30`.
2. `BtlSlot6AnimationAlternate`, which `BtlRun` calls for 0xF6 only, after
   `BtlInit()` (`0x80029E68`), `screen_disp_mask_set(1)` (`0x8001EFEC`) and
   `cd_wait_btl_frame_set(1)` (`0x80020E7C`), and before the round loop:

   ```c
   BtlFrameWait(0x1E);                     /* 0x1E battle frames */
   animation = 0x11;
   for (n = 0x5B; n != 0; n--) {
       animation = animation ? 0 : 0x11;   /* 0 first */
       BtlAnimationSet(actor 6, animation); /* a change restarts the animation */
       BtlFrameStep();                     /* one battle frame */
   }
   ```

   That is 121 battle frames before round 1: 30 of Xenobia's anim 0x11 (a
   hold anim of 24 steps, 112 ticks: pose 66 for 60 ticks, then a flicker of
   its own), then 91 frames switching between the first steps of anims 0 and
   0x11, ending on 0. No draw of its own; with Nall's idle machine on, these
   are 121 more frames of its draws (`BtlFrameStep` (`0x80038D2C`),
   frame-rng.md).
