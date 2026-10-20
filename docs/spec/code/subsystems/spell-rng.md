# Spell presentations that draw from the RNG

This page gives the draw sequence of each battle spell presentation that
draws random numbers, as its C in btl_show.c reads. A presentation runs
before the battle goes on, so a cast moves every later draw, and a run that
leaves a presentation out or shortens it goes another way from the game
after it. battle.md §"Spell presentations — `BtlShowNumberRun`
(`0x8007BB80`) and the effect resources" lists the shows and which of them
draw; the generator itself is menu-ui.md §"The random numbers — `rng.c`";
the draws of the battle frame are frame-rng.md.

## The RNG helpers

* **`rng_draw()` (`0x8001C770`)** — the raw draw (one step of the
  generator).
* **`rng_7f_draw()` (`0x8003A0B4`)** — one raw draw, `& 0x7F` (btl_pick.c).
* **`rng_mask_below(mask, n)` (`0x8003A0D4`)** — returns 0 **without
  drawing** when `n < 2`; otherwise exactly one raw draw, `r = raw & mask`,
  then `r -= n` while `r >= n` (so `r % n`), returned as u16. Because the
  mask is usually not a multiple of `n`, results are **not uniform**:
  `(raw & 0xF) % 6` gives 0..3 at 3/16 each and 4..5 at 2/16. The C
  (btl_pick.c) is K&R, `(s32 mask, u16 n)`, and both compares are 16-bit:
  n's upper half is ignored, so an n of 0x10001 behaves as 1 and draws
  nothing.

  Its callers, by what the draw is for:

  | use | function | call |
  |---|---|---|
  | physical damage spread | `BtlHitAmountSet` (`0x80030070`) | `(0x7F, stat_08 / 10 + 2)` |
  | stat-0E (magic) amount | `BtlStat0EHitAmountSet` (`0x80035EF8`) | `(0x3FF, stat_0e / 10)` |
  | area hit | `BtlAreaStat0EHitRun` (`0x80032C64`) | `(0xFFFF, stat_0e / 10)` |
  | turn order | `BtlTurnOrderMake` (`0x8002F4B0`) | `(0x3FF, top / 10)` |
  | heals | `BtlTargetHpShareAddRun` (`0x80031ED4`), `BtlPartyHpShareAddRun` (`0x800320C8`) | `(0xFFFF, max HP / 20)` |
  | MP and HP items | `BtlTargetMpAddRun` (`0x80031E00`), `BtlTargetHpAddRun` (`0x80031FB4`) | `(7, 8)` |
  | fixed-damage weapons | `BtlTargetHpFixedRun` (`0x80034B8C`) | `(0xF, 0xA)` |
  | the status tick | `BtlRoundStatusUpdate` (`0x800297A4`) | `(3, 4)` |
  | battle entry, when story bit 0x2D0 is set | `BtlPartyPairStatusSet` (`0x8002A010`) | `(7, 6)` |
  | pickers | `BtlEscapeFailStepPick` (`0x80036F3C`); `BtlSlot5ReviveTry` (`0x80027B6C`), `BtlEnemy6FActionSet` (`0x80041A80`), `BtlEnemy71TargetPick` (`0x80041F6C`), `BtlEnemy7AActionSet` (`0x80042958`) | `(7, n)`; `(3, n)` |
  | boss plans | `BtlEnemy73ActionSet` (`0x800420E0`) `(7, 6)`; `BtlEnemy78ActionSet` (`0x800426B0`) `(0xF, 0xA)`; `BtlEnemy7AActionSet` `(0xF, 0xA) + 0xF`, `(7, 5)`; `BtlEnemy7CActionSet` (`0x80042F4C`) `(7, 5)`, `(3, 4)` | boss-ai.md |
  | black lines over the screen's rows, in a function nothing calls | `func_80049C50` | `(0x1F, 0x1F)`, one per row (0xE0) |
  | presentations | `BtlShow06Run`, `BtlShow09Run`, `BtlShow0BRun`, `BtlShow1ARun`, `BtlShow1DRun`, `BtlShow2DRun`, `BtlShow2FRun` (below); `BtlShow54Run` (`0x800876E4`) `(0x3F, 0x30)`, `(0x1F, 0x1F)`; `BtlShowACRun` (`0x8003DCA0`) `(0xFF, 0xA0)`, `(0x1F, 0x20)` | |

## The per-frame tick also draws (all scripts)

Every script waits its frames through `BtlShowFrameStep` (`0x80089DDC`)
(btl_show_frame.c), which steps the show's sounds, pulse, wave and records
while a show is open and then calls `BtlFrameStep` (`0x80038D2C`) every
frame. Two routes from the frame draw; frame-rng.md has both in full:

* `BtlFrameStep` → `BtlSlot5RoamStep` (`0x80038EE8`) → `rng_draw`: the
  slot-5 idle machine, run only when slot 5 is Nall (`g_BtlSlotIds[5] ==
  0xC`) and only while `g_BtlSlot5Roaming != 0`; 0 to 6 draws a frame from
  its three state machines (frame-rng.md §"Draw site 1").
* **`pad_update` (`0x8001F508`), the pad and vibration driver**, reached
  every frame through `frame_end` (`0x8001F30C`) → `pad_frame_step` (`0x8001F30C`,
  `0x8001EE98`): one `rng_draw` per motor each time that motor's period
  timer runs out in vibration way 0x48. It runs whenever a vibration script
  is going — **even with vibration switched off**, because the draws come
  before the off test — and spell scripts start those scripts
  (`g_PadActuatorList = g_BtlActuatorFFList` and others). Of the 25
  vibration scripts the battle starts, only `g_BtlActuatorRandomList`
  (`0x800AE310`) has the random way (motor 1). It draws **ten times, on
  script frames 85, 89 .. 121** (frame-rng.md §"Draw site 2"). Of the
  spells, Ice Wall, Inferno and Flash Wind start it and keep it running
  that long, so those three casts carry ten draws more than their own,
  between them by frame. The breath presentations start it too (battle.md
  §"The breath attacks' party sweep — `BtlPartyRightMove`
  (`0x80086700`)").

**The counts below are the scripts' own draws; the per-frame draws above
come on top.**

## Per script

**Dragon Anger — `BtlShow06Run` (`0x8007CEA4`) (0x06).** Sixteen meteors,
meteor k starting at frame 8k of a loop of 0xB0 frames. At its start, an
even k whose enemy slot `k / 2 + 6` is marked (`g_BtlSlotAmountKinds` (`0x800E48F0`)) is
aimed at that slot with no draw; every other meteor draws twice, in order —
x = `rng_mask_below(0xFF, 0xFF)` + 0x20, then y = `rng_mask_below(0x3F,
0x3F)` + 0x70. **2 × (16 − marked enemy slots among 6..13)** draws. Each
landing (frame 8k + 8) starts a 3-frame burst of the screen shake; when the
vibration option `g_GameOptions[5]` (`0x800B8968`) is set, every burst frame draws twice
more, raw — `g_PadActuator1Level += rng_draw() & 0x3F`, then
`g_PadActuatorLevels = rng_draw() != 0` — which is **96** draws for the 16
bursts. Within frame 8m the meteor's draws come before the burst's. The
vibration script it starts, `g_BtlActuatorFFList` (`0x800AE258`), has no random way.

**Thunderbolt — `BtlShow09Run` (`0x8007DD10`) (0x09).** One
`rng_mask_below(0xF, 6)` per bolt, five bolts per cast: **5 draws**, at
frames k = 0, 13, 26, 39, 52 of the frame loop. The loop runs n + 0x41
frames, n = `BtlAnimationFramesCount(template)` (`0x80088DF4`), so all five
always spawn. Each draw is bolt i's y offset: y = target y (`+0xA`) + r, r
0..5 (not uniform, above). The bolts' x places are fixed (target x − w/2,
+ w/2, − w/4, + w/4, x; w = target `+0x3C`).

**Thunder Thrust — `BtlShow0BRun` (`0x8007E444`) (0x0B).** All draws come
in one set-up loop over records k = 0..13, after the opening and before the
bolts' loop: a record whose slot is marked (`g_BtlSlotAmountKinds[k] != 0`,
a target) draws nothing; every other record draws twice, in order —
`rng_7f_draw()` (x = r + 0x20 if the previous x >= 0xA0, else r + 0xA0;
"previous x" starts at 0xA0 and target records set it too, so the other
bolts alternate sides), then `rng_mask_below(0x3F, 0x3F)` (y = r + 0x70, r
0..62). **2 × (unmarked slots among 0..13)** draws.

**Flameria — `BtlShow1ARun` (`0x80080BF8`) (0x1A).** All draws come in one
set-up loop over records i = 0..15, before the 0xB0-frame loop, which draws
nothing. "Previous x" starts at 0. A record whose `g_BtlSlotAmountKinds[i]`
is set takes the target's x and its y − 0xB0, restarts
`g_BtlActuatorFFList`, and draws nothing; every other record draws twice,
in order — `rng_7f_draw()` (x = r + 0xA0 when the previous x is below 0xA0,
else r + 0x20), then `rng_mask_below(0x3F, 0x3F)` − 0x40 for its y
(`+0xA`). Records 14 and 15 read the two bytes after the fourteen slots'
(`0x800E48FE`, `0x800E48FF`, just before `g_BtlSlotPlannedTargets`
(`0x800E4900`)), which battle.md finds never written, so they always draw.
**2 × (16 − marked slots among 0..13)** draws.

**Ice Wall — `BtlShow1DRun` (`0x800813C0`) (0x1D).** Three draws per block
spawn or respawn, in order: `rng_mask_below(0x3F, 0x30)` the x (target x +
r − 0x18), `rng_mask_below(0x1F, 0x1C)` the y (y = target y − 0x10 + r;
`+0xA` = y − 0xE, `+8` = y + 3), `rng_mask_below(3, 3) + 1` the animation
page (page 1 at 1/2, pages 2 and 3 at 1/4). The loop runs 48 passes, each
`BtlTilePatternStep(seed, 4)` (`0x8008B21C`), four frames when the
backdrop's pattern byte is not 0; within a pass it looks at blocks k =
0..15 in order. Block k spawns at pass k (16 spawns = 48 draws). At passes
1..0x20 every block already spawned (k < pass) that is shown and whose
animation has ended (`BtlAnimationPlayCheck(block) == 0` (`0x8008A408`))
respawns (3 draws); from pass 0x21 a finished block is cleared without a
draw. **48 + 3 × respawns** — and the respawn count depends on the lengths
of the animation pages drawn. **Plus the vibration script's ten:**
`g_BtlActuatorRandomList` starts right after the cast animation, 74 frames
before the first block (seven backdrops of four frames, the layer's
30-frame fade, four more backdrops of four), so its draws land between the
blocks' from the third pass on, and move which pages the later blocks get.
At the end `g_BtlActuatorFFList` replaces it.

**Inferno — `BtlShow2ARun` (`0x80082D9C`) (0x2A).** No draw of its own.
`g_BtlActuatorRandomList` runs under its first 0x140 backdrop steps, one
frame each: **10 draws**.

**Rock'N'Roll — `BtlShow2DRun` (`0x800845C4`) (0x2D).** (1) Placement, 20
rock records in order 0..19: a record below 14 whose slot is marked aims at
that target (x, y) with no draw; every other record draws x then y —
`(u8)rng_7f_draw()` + 0x20 or + 0xA0 (alternating halves, starting on the
side of the most recent marked target, else the left), then
`rng_mask_below(0x3F, 0x3F)` + 0x70: **2 × (20 − marked slots among
0..13)**, every cast. (2) When the vibration option `g_GameOptions[5]` is
set (pad bit 2 flips it in a battle, `BtlFrameStep`), the frames draw for
the actuator levels `g_PadActuator1Level` (`0x800B8699`) / `g_PadActuatorLevels` (`0x800B8698`): the
shakes of `BtlShakesRun` (`0x800843E0`) (period × 6 frames at 2 draws:
periods 6, 6, 4, 4 → **240**, 144 before the main loop and 96 after the
impact) and the main loop's 544 frames (`total + 0x138`, total = 232) at 1
draw → **544**. With the option off, none of these. The script decides no
damage.

**Flash Wind — `BtlShow2FRun` (`0x80085194`) (0x2F).** The only draw is
`rng_mask_below(0xFF, 0xD0)`, a streak's new row (`+0xA`); the x places
never depend on a draw, so the count per cast is fixed. Phase A runs while
the fade of mask 8 started by `layer_mix_fade_start(8, 1, 0x3C, 0xF)`
(`0x8001EBA4`) runs (60 ticks when `g_BtlRunning != 0`, none when 0): on
frame i, for streak j = 0..9, `(u16)(i − 5j) == 0` spawns it (1 draw), else
x < 0 puts it back at 0x140 (1 draw), else x = (x − spd1[j]) % 320 (C
truncation). Phase B is 160 frames: x < 0 puts it back (1 draw), else x =
(x − spd2[j]) % 320. Phase C (a fade of 0x78 ticks) draws nothing.
`g_BtlActuatorRandomList` starts one frame before phase A: its ten draws
fall in phase B, among the streaks'. The speed tables: spd1
`g_BtlShow2FFirstSpeeds` (`0x80012F18`) = 12, 10, 4, 8, 12, 6, 4, 8, 6, 10; spd2
`g_BtlShow2FSecondSpeeds` (`0x80012F2C`) = 16, 14, 8, 12, 16, 10, 8, 12, 10, 14; every
streak starts at x = 0x140, hidden. Stepping the C's loops gives **76
draws** when `g_BtlRunning != 0` (20 in A + 56 in B) and **50** when 0;
the order is by frame, j = 0..9 within a frame.

The bosses' shows in the same table draw too: 0x54 (`BtlShow54Run`) in
battle.md §"Shows 0x9F, 0x54 and 0x9C in detail", and the breaths (0x50 to
0x52) through the vibration script, battle-presentations-2.md §"Group F —
the boss breaths 0x50..0x53 and effect 93".
