/* Lunar SSSC — btl_hit: a hit: the target moved back and forward, the try,
   the amount set and taken, the slots and statuses that follow, status 8
   cleared, the marked slots' animations and the amount's wait. The first
   split had it in game_misc.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
/* far_syms: g_BtlSlotAmounts (carried from the first split's game_misc.c) */
/* far_syms: g_BtlSlotRestAnimations (carried from the first split's game_misc.c) */
/* far_syms: g_BtlSlotActiveFlags (carried from the first split's game_misc.c) */

extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlSlotRestAnimations[];
extern u8 g_BtlNumber;
s32 BtlAnimationSet(s32, u8);
extern u8 g_BtlSlotFallMarks[];
extern u16 g_BtlSlotAmounts[];
extern u8 g_BtlSlotIds[];
extern s32 g_BtlRecords;
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlActingSlot;
extern u8 g_BtlFallDone;
extern u8 g_BtlSlotStatus8Rounds[];
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotAmountKinds[];
extern u8 g_BtlSlotActiveFlags[];
s32 BtlFrameStep();                                  /* extern */
extern long HWD0, VWD0;
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */
extern BtlWalkBlock g_BtlWalkBlock;
void BtlAnimationEndWait(void *record);
s32 SoundPlay(s32, s32);
extern long HWD0;
extern long VWD0;
s32 BtlActingRestAnimationSet();
s32 BtlPartyPanelDisp();
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern s32 CharStats;
void BtlTargetStatus8Set(void);
void BtlTargetStatus40Set(void);
void BtlTargetStatus80Set(void);
void BtlAnimationEndWait(void *record);
s32 ratio_mul(s32 value, s32 multiplier, s32 divisor);
s32 ratio_13_10_mul(s32 value);
s32 BtlSlotHitCheck(s32 slot);
void BtlTargetStatus8Clear();
void BtlMarkedStatus8Clear(void);
s32 BtlAmountShowCheck();
void BtlAmountShowWait(void);
/* far_syms: g_BtlSlotPlannedTargets (target reaches it %hi/%lo) */
extern s16 g_BtlActionBitNumber;
extern u16 g_BtlActionPower;
extern u8 g_BtlSlotPlannedTargets[];
/* far_syms: g_BtlActingSlot g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber g_BtlSlotPlannedKinds g_BtlSlotActiveFlags CharStats */
u8 BtlPartyTargetPick(s32);
s32 BtlActingRestAnimationSet();
s32 BtlPartyPanelDisp();
s32 SoundPlay(s32, s32);
/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
extern BtlWalkBlock g_BtlWalkBlock;
/* far_syms: g_BtlSlotActiveFlags */
/* far_syms: g_BtlActingSlot g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
void BtlTargetStatus4Set(void);
void BtlTargetForwardMove(void);
s32 BtlAttackTargetFind(void);
/* far_syms: g_BtlActingSlot g_BtlNumber g_BtlActionKind g_BtlActionNumber g_BtlTargetSlot CharStats */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
/* far_syms: CharStats g_BtlSlotActiveFlags g_BtlSlotIds g_BtlSlotPlannedKinds */
s32 rng_mask_below();
/* far_syms: CharStats (target reaches it %hi/%lo) */
s32 ratio_13_10_mul(s32 value);
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern s32 CharStats;
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern s32 CharStats;
/* far_syms: CharStats */
/* far_syms: g_BtlSlotStatus8Rounds (target reaches it %hi/%lo) */
void BtlTargetStatus1Set(void);  /* extern */
void BtlTargetStatus30Set(void);  /* extern */

/* func_8002FD1C */
void BtlTargetBackMove(void)
{
  s32 i;
  s32 direction;
  void *record;
  direction = -1;
  if ((*(u16 *)((u8 *)((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xC)) & 0x2000)
  {
    direction = 1;
  }
  SoundPlay(0x24, 3);
  i = 0;
  do
  {
    record = (g_BtlTargetSlot << 6) + g_BtlRecords;
    i += 1;
    ((BtlRecord *)record)->sprite.x = (u16)(AS(u16, ((BtlRecord *)record)->sprite.x) + direction);
    BtlFrameStep();
  }
  while (((u32)(i & 0xFF)) < 4U);
}

/* func_8002FDBC */
void BtlAnimationEndWait(void *record)
{
  unsigned short step;
  s32 last_step;
  last_step = ((AS(s16, ((BtlRecord *)record)->end) & 0x7FFF) - 1) & 0xFFFF;
  do
  {
    BtlFrameStep();
  }
  while ((step = AS(s32, ((BtlRecord *)record)->step)) != last_step);
}

/* func_8002FE10 */
s32 ratio_mul(s32 value, s32 multiplier, s32 divisor) {
    return (s32) (value * multiplier) / divisor;
}

/* func_8002FE54 */
s32 ratio_13_10_mul(s32 value) {
    ratio_mul(value, 0xD, 0xA);
}

/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
s32 BtlSlotHitCheck(s32 slot);  /* TU defines this below */
void BtlTargetStatus8Clear();
void BtlMarkedStatus8Clear(void);  /* TU defines this below */
s32 BtlAmountShowCheck();
void BtlAmountShowWait(void);  /* TU defines this below */

/* func_8002FE78 */
void BtlTargetForwardMove(void)
{
  s32 i;
  s32 direction;
  void *record;
  direction = 1;
  if (((short)(*((s32 *) (((u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords)) + 0xc)))) & 0x2000)
  {
    direction = -1;
  }
  i = 0;
  do
  {
    record = (g_BtlTargetSlot << 6) + g_BtlRecords;
    i += 1;
    ((BtlRecord *)record)->sprite.x = (u16) (((BtlRecord *)record)->sprite.x + direction);
    BtlFrameStep();
  }
  while (((u32) (i & 0xFF)) < 4U);
}
/* far_syms: g_BtlTargetSlot */
s32 rng_7f_draw();

/* The evasion check of an ordinary swing (battle.md "The evasion check"):
   does the swing land on the target g_BtlTargetSlot? It writes the target's
   hit-reaction code g_BtlSlotAmountKinds[target] (1 = landed, 2 = missed) and answers
   1 (landed) or 0. In order:

   1. The target's status byte (CharStats +0x15) has any of 0x3C: landed,
      no draw.
   2. BtlTargetTraitCheck(0, 0x31, 0, 0) (the target holds 0x31) and a draw
      (rng_7f_draw, the low 7 bits of the RNG) below 0x20: missed. A
      holder whose draw fails goes on.
   3. An enemy slot (6 and up) whose id g_BtlSlotIds[target] is 0x2F: missed,
      no draw.
   4. One more draw, below 0x80: landed; otherwise missed. The draw is 7
      bits, so it always lands; what matters is that it is DRAWN: every
      ordinary swing that gets here advances the RNG once (twice for a 0x31
      holder whose dodge failed).

   No frame wait. g_BtlTargetSlot is re-read at every use (it is never held
   across a call).

   Shape:
   - MARKED SPELLING: step 4's draw sits behind a three-way test of the
     target slot whose three arms are the same call. The test has no
     effect; it is written because only that shape leaves the load
     described next, and its two bounds are a choice (see the end of
     this item). The target code has a load of
     g_BtlTargetSlot into $v1 just before that call which nothing reads: it is
     what is left of the tests. The late jump pass merges the equal arms,
     the two branches then jump to the next instruction and are deleted
     with their compares, and the load they shared stays (the first
     compare is not the byte's last use, so the load is not deleted with
     it). A two-way test is removed whole, load included, and so is a
     three-way test that begins with step 3's own `>= 6`. The Japanese
     build has the same load, and under GCC 2.7.2 and the 1997 snapshot
     this source leaves it as well; a save and restore of the byte
     through a local, the other spelling that leaves the load here
     (in $v0), keeps its store under both.
     WHICH values the original tested cannot be read from the binary. The
     bounds written here are the three slot classes the battle code uses
     elsewhere (party 0..4 as in BtlTargetTraitCheck, slot 5, enemies 6 and
     up); a switch on two case values gives the same bytes.
   - The roll is held unsigned and masked, `(r & 0xFF) < 0x80`: written
     `(u8)r < 0x80` the compare is folded into a sign test (sll 24). */
/* func_8002FF0C */
s32 BtlHitTry(void)
{
    u32 r;

    if (*(u8 *)(CharStats + g_BtlTargetSlot * 60 + 0x15) & 0x3C) {
        g_BtlSlotAmountKinds[g_BtlTargetSlot] = 1;
        return 1;
    }
    if ((u8) BtlTargetTraitCheck(0, 0x31, 0, 0) != 0 && (u8) rng_7f_draw() < 0x20) {
        g_BtlSlotAmountKinds[g_BtlTargetSlot] = 2;
        return 0;
    }
    if (g_BtlTargetSlot >= 6 && g_BtlSlotIds[g_BtlTargetSlot] == 0x2F) {
        g_BtlSlotAmountKinds[g_BtlTargetSlot] = 2;
        return 0;
    }
    if (g_BtlTargetSlot < 5) {
        r = rng_7f_draw();
    } else if (g_BtlTargetSlot == 5) {
        r = rng_7f_draw();
    } else {
        r = rng_7f_draw();
    }
    if ((r & 0xFF) < 0x80) {
        g_BtlSlotAmountKinds[g_BtlTargetSlot] = 1;
        return 1;
    }
    g_BtlSlotAmountKinds[g_BtlTargetSlot] = 2;
    return 0;
}

/* far_syms: CharStats g_BtlNumber g_BtlSlotAmounts g_BtlSlotRestAnimations g_BtlActingSlot g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber */
s32 rng_mask_below();
s32 BtlHitStrongTry();
void BtlHitAmountTake();

/* The physical damage resolver: attacker g_BtlActingSlot, target g_BtlTargetSlot
   (CharStats records, 0x3C each). Attack + attack * g_BtlActionPower% plus a
   0x7F-scaled roll (rng_mask_below) and a critical hook (BtlHitStrongTry ->
   ratio_13_10_mul), minus defence, plus 5 per level of advantage, halved,
   floored at the attacker's min_damage. Mode 2 (g_BtlActionKind) applies the
   party weapon's element against the enemy's weak/resist masks; any other
   mode the command element (g_BtlActionBitNumber) through BtlTargetTraitCheck. A status
   bit 0x0C doubles it; mode 3 compounds the action class's percentage
   (rank - 1) times; a target in status 0x13 halves it; clamped to 1..999
   (above 1000); a guard check in modes 2/3/7 and the 0xFA override zero
   it. Stored to g_BtlSlotAmounts[target], then BtlHitAmountTake applies it. */
/* func_80030070 */
s32 BtlHitAmountSet(void)
{
    u16 r;
    u32 dmg;
    u8 e1;
    u8 e2;
    s32 def;
    s32 t;
    s32 pct;
    s32 i;

    r = *(u16 *)(g_BtlActingSlot * 60 + CharStats + 8) / 10 + 2;
    r = rng_mask_below(0x7F, r);
    dmg = *(u16 *)(g_BtlActingSlot * 60 + CharStats + 8);
    dmg += (s32)(dmg * g_BtlActionPower) / 100;
    def = *(u16 *)(g_BtlTargetSlot * 60 + CharStats + 0xA);
    if ((u8)BtlHitStrongTry()) {
        dmg = ratio_13_10_mul(dmg);
    }
    dmg += r;
    t = dmg - def;
    if (*(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x14) > *(u8 *)(g_BtlTargetSlot * 60 + CharStats + 0x14)) {
        t += (*(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x14) - *(u8 *)(g_BtlTargetSlot * 60 + CharStats + 0x14)) * 5;
    }
    t /= 2;
    if (t < *(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x16)) {
        t = *(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x16);
    }
    dmg = t;
    r = 0;
    if (g_BtlActionKind == 2) {
        if (g_BtlActingSlot < 6 && g_BtlTargetSlot >= 6) {
            switch (*(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x28)) {
            case 0x2A:
                r = 1;
                break;
            case 0x2B:
                r = 2;
                break;
            case 0x2C:
                r = 4;
                break;
            case 0x2D:
                r = 8;
                break;
            case 0x2E:
                r = 3;
                break;
            case 0x2F:
                r = 0xC;
                break;
            }
            if (r & *(u16 *)(g_BtlTargetSlot * 60 + CharStats + 0x28)) {
                g_BtlSlotAmountKinds[g_BtlTargetSlot] = 3;
                dmg *= 2;
            }
            if (r & *(u16 *)(g_BtlTargetSlot * 60 + CharStats + 0x2A)) {
                g_BtlSlotAmountKinds[g_BtlTargetSlot] = 4;
                dmg /= 2;
            }
        }
    } else {
        switch ((u16)g_BtlActionBitNumber) {
        case 1:
            r = 1;
            e1 = 0x2A;
            e2 = 0x2E;
            break;
        case 2:
            r = 2;
            e1 = 0x2B;
            e2 = 0x2E;
            break;
        case 3:
            r = 4;
            e1 = 0x2C;
            e2 = 0x2F;
            break;
        case 4:
            r = 8;
            e1 = 0x2D;
            e2 = 0x30;
            break;
        case 5:
            r = 0x10;
            break;
        case 6:
            r = 0x20;
            break;
        case 7:
            r = 0x40;
            break;
        }
        if (r != 0) {
            switch ((u8)BtlTargetTraitCheck(r, e1, e2, 0)) {
            case 1:
                dmg /= 2;
                g_BtlSlotAmountKinds[g_BtlTargetSlot] = 4;
                break;
            case 2:
                dmg *= 2;
                g_BtlSlotAmountKinds[g_BtlTargetSlot] = 3;
                break;
            }
        }
    }
    if (*(u8 *)(g_BtlTargetSlot * 60 + CharStats + 0x15) & 0xC) {
        dmg *= 2;
    }
    if (g_BtlActionKind == 3) {
        switch (g_BtlActionNumber) {
        case 1:
        case 29:
            pct = 0x82;
            break;
        case 30:
        case 31:
            pct = 0x78;
            break;
        case 2:
        case 3:
        case 47:
            pct = 0x6E;
            break;
        default:
            pct = 0x64;
            break;
        }
        e1 = *(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x12) - 1;
        for (i = 0; i < e1; i++) {
            dmg = dmg * pct / 100;
        }
    }
    if (g_BtlTargetSlot < 5 && g_BtlSlotRestAnimations[g_BtlTargetSlot] == 0x13) {
        dmg /= 2;
    }
    if (dmg == 0) {
        dmg = 1;
    }
    if (dmg > 1000) {
        dmg = 999;
    }
    switch (g_BtlActionKind) {
    case 2:
        if ((u8)BtlTargetTraitCheck(0, 0x33, 0x34, 0) == 1) {
            dmg = 0;
        }
        break;
    case 3:
        if ((u8)BtlTargetTraitCheck(0, 0x34, 0, 0) == 1) {
            dmg = 0;
        }
        break;
    case 7:
        if ((u8)BtlTargetTraitCheck(0, 0x33, 0, 0) == 1) {
            dmg = 0;
        }
        break;
    }
    if (g_BtlNumber == 0xFA) {
        dmg = 0;
    }
    g_BtlSlotAmounts[g_BtlTargetSlot] = dmg;
    BtlHitAmountTake();
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80011BB8_pad);
/* far_syms: g_BtlNumber CharStats g_BtlTargetSlot */

/* The damage applier (battle.md "The damage chain"): takes the amount the
   resolver left in the u16 table g_BtlSlotAmounts for the target g_BtlTargetSlot off
   that record's HP (the first halfword of its 0x3C-byte CharStats record).

   - Nothing at all when the target is an enemy slot (6 and up) and the
     encounter g_BtlNumber is 0xF9, 0xFA or 0xFE.
   - Encounter 0xF7 with target slot 6, while record 7's HP (CharStats +
     0x1A4) is not 0: the amount comes off RECORD 7's HP instead, floored
     at 0 (no KO flag and no 1 here), and the hit is handed to slot 7: its
     amount (g_BtlSlotAmounts[7] = [6], [6] = 0) and its hit-reaction code
     (g_BtlSlotAmountKinds[7] = 1, [6] = 0). Slot 6's own HP is not touched.
   - Otherwise: an amount that is not below the HP sets the KO flag
     g_BtlSlotFallMarks[target] and leaves the HP at 1, not 0 (battle.md: a later
     writer zeroes it); a smaller one is subtracted.
   No call, no RNG draw, no frame.

   Shape:
   - the three-way skip is a switch (two signed slti and a beq; as an ||
     chain it is 77/92);
   - record 7's HP is read from memory at every use, with no local. The
     target holds the raw halfword and its zero-extended copy in two
     registers (lhu / andi 0xFFFF): the subtraction reads the halfword a
     second time, so the load and its mask are not merged. Through a u16
     local the mask goes (67/92). The last block does use locals: there
     the target has one register and no mask;
   - the four moves index the u16 table directly, declared u16[] as in
     the other files that use it. Declared u8[], they need a u16 pointer
     local taken after the if / else (87/92 with a cast written at each
     use). */
/* func_80030650 */
void BtlHitAmountTake(void)
{
    u16 dmg;
    u16 hp;

    if (g_BtlTargetSlot >= 6) {
        switch (g_BtlNumber) {
        case 0xF9:
        case 0xFA:
        case 0xFE:
            return;
        }
    }
    if (g_BtlNumber == 0xF7 && g_BtlTargetSlot == 6) {
        if (((BtlStats *)CharStats)[7].hp != 0) {
            if (g_BtlSlotAmounts[6] >= ((BtlStats *)CharStats)[7].hp) {
                ((BtlStats *)CharStats)[7].hp = 0;
            } else {
                ((BtlStats *)CharStats)[7].hp -= g_BtlSlotAmounts[6];
            }
            g_BtlSlotAmounts[7] = g_BtlSlotAmounts[6];
            g_BtlSlotAmountKinds[7] = 1;
            g_BtlSlotAmounts[6] = 0;
            g_BtlSlotAmountKinds[6] = 0;
            return;
        }
    }
    dmg = g_BtlSlotAmounts[g_BtlTargetSlot];
    hp = *(u16 *)(g_BtlTargetSlot * 60 + CharStats);
    if (dmg >= hp) {
        g_BtlSlotFallMarks[g_BtlTargetSlot] = 1;
        *(u16 *)(g_BtlTargetSlot * 60 + CharStats) = 1;
        return;
    }
    *(u16 *)(g_BtlTargetSlot * 60 + CharStats) = hp - dmg;
}

/* func_800307C0 */
s32 BtlSlotHitCheck(s32 slot) {
    u8 amount_kind;

    amount_kind = g_BtlSlotAmountKinds[slot & 0xFF];
    if ((amount_kind == 1) || (amount_kind == 3) || (amount_kind == 4)) {
        return 1;
    }
    return 0;
}
/* far_syms: g_BtlNumber g_BtlActingSlot CharStats g_BtlTargetSlot */
s32 rng_7f_draw();
s32 BtlFallRun();
void BtlHitStatusUpdate(void);  /* TU defines this below */
void BtlMarkedRestAnimationSet(void);  /* TU defines this below */

/* The pass after every hit. It walks the slots the hit landed on
   (BtlSlotHitCheck: g_BtlSlotAmountKinds[slot] is 1, 3 or 4) among the first `count`:
   all 14, or only the 5 party slots when g_BtlNumber >= 0xEB.
   - A PARTY attacker (g_BtlActingSlot < 5) whose record byte +0x28 is 6, 0x0D or
     0x14 rolls an instant KO per landed slot: (u8)rng_7f_draw() below
     0x20 / 0x40 / 0x80 raises the KO flag g_BtlSlotFallMarks[slot]. Then
     BtlFallRun (KO cleanup) and nothing else.
   - Anyone else: BtlFallRun FIRST, then BtlHitStatusUpdate (the on-hit status
     roll) for each landed slot that is still active (g_BtlSlotActiveFlags), with the
     target index g_BtlTargetSlot stepped over the slots and put back afterwards,
     then BtlMarkedRestAnimationSet.
   Nothing is returned; the callers pass on whatever the last callee left.
   Shape notes: `start` is assigned once, so both entry tests fold
   to `count != 0` while the copies of it stay; `t` is one byte used as the
   KO threshold on the first path and as the saved g_BtlTargetSlot on the second;
   the second loop is a guarded do/while, not a `for` over the global. */
/* func_80030800 */
s32 BtlHitSlotsUpdate(void)
{
    u32 count;
    u8 start;
    u8 i;
    u8 t;

    count = 0xE;
    start = 0;
    if (g_BtlNumber >= 0xEB)
    {
        count = 5;
    }
    if (g_BtlActingSlot < 5)
    {
        t = 0;
        switch (*(u8 *)(CharStats + g_BtlActingSlot * 0x3C + 0x28))
        {
        case 6:
            t = 0x20;
            break;
        case 0xD:
            t = 0x40;
            break;
        case 0x14:
            t = 0x80;
            break;
        }
        if (t != 0)
        {
            for (i = start; i < count; i++)
            {
                if ((u8)BtlSlotHitCheck(i) && (u8)rng_7f_draw() < t)
                {
                    g_BtlSlotFallMarks[i] = 1;
                }
            }
            BtlFallRun();
            return;
        }
    }
    BtlFallRun();
    t = g_BtlTargetSlot;
    g_BtlTargetSlot = start;
    if (start < count)
    {
        do
        {
            if ((u8)BtlSlotHitCheck(g_BtlTargetSlot) && g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0)
            {
                BtlHitStatusUpdate();
            }
            g_BtlTargetSlot++;
        } while (g_BtlTargetSlot < count);
    }
    g_BtlTargetSlot = t;
    BtlMarkedRestAnimationSet();
}

/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotAmounts (target reaches it %hi/%lo) */

/* func_80030A04 */
void BtlHitStatusUpdate(void)
{
    unsigned int trait;
    unsigned int rate;

    rate = 0;
    if (g_BtlActingSlot < 5)
    {
        trait = *(u8 *)(CharStats + g_BtlActingSlot * 60 + 0x28);
    }
    else
    {
        trait = *(u8 *)(CharStats + g_BtlActingSlot * 60 + 0x1B);
    }

    if ((trait - 1) < 5 || trait == 0x29)
    {
        rate = 0x20;
    }
    if ((trait - 8) < 5 || trait == 0x1A)
    {
        rate = 0x40;
    }
    if ((trait - 0xF) < 5 || trait == 0x19)
    {
        rate = 0x80;
    }

    if ((rng_7f_draw() & 0xFF) < rate && (trait - 1) < 0x29)
    {
        switch (trait - 1)
        {
            case 0:
            case 7:
            case 14:
                BtlTargetStatus4Set();
                goto end;
            case 1:
            case 8:
            case 15:
                BtlTargetStatus8Set();
                goto end;
            case 2:
            case 9:
            case 16:
                BtlTargetStatus30Set();
                goto end;
            case 3:
            case 10:
            case 17:
                BtlTargetStatus40Set();
                break;
            case 4:
            case 11:
            case 18:
                BtlTargetStatus80Set();
                break;
            case 24:
            case 25:
            case 40:
                BtlTargetStatus1Set();
                break;
        }
    }
    BtlTargetStatus8Clear();
    end:
}

void BtlTargetStatus8Clear();                                  /* extern */

/* func_80030B8C */
void BtlMarkedStatus8Clear(void) {
    u8 next_target;

    g_BtlTargetSlot = 0;
    do {
        if ((*(g_BtlTargetSlot + g_BtlSlotAmountKinds) != 0) && (*(g_BtlTargetSlot + g_BtlSlotActiveFlags) != 0)) {
            BtlTargetStatus8Clear();
        }
        next_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_target;
    } while ((u32) (next_target & 0xFF) < 0xEU);
}

/* far_syms: g_BtlPartyStandAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
extern u8 g_BtlPartyStandAnimations[];

/* func_80030C28 */
void BtlTargetStatus8Clear()
{
  u8 animation;
  if ((*((u8 *) ((CharStats + (g_BtlTargetSlot * 0x3C)) + 0x15))) & 0x8)
  {
    *((u8 *) ((CharStats + (g_BtlTargetSlot * 0x3C)) + 0x15)) &= 0xF7;
    if (g_BtlTargetSlot < 0x5)
    {
      *((u8 *) ((CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1E)) &= 0xFB;
    }
    g_BtlSlotStatus8Rounds[g_BtlTargetSlot] = 0;
    if (*((u8 *) ((CharStats + (g_BtlTargetSlot * 0x3C)) + 0x15)))
    {
      BtlSlotStatusAnimationSet(g_BtlTargetSlot);
      return;
    }
    if (g_BtlSlotPlannedKinds[g_BtlTargetSlot])
    {
      BtlSlotActionAnimationSet(g_BtlTargetSlot);
      return;
    }
    if (g_BtlTargetSlot < 0x5)
    {
      animation = g_BtlPartyStandAnimations[g_BtlTargetSlot];
    }
    else
    {
      animation = 0;
    }
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), animation);
    g_BtlSlotRestAnimations[g_BtlTargetSlot] = animation;
  }
}

/* func_80030DA0 */
void BtlMarkedRestAnimationSet(void)
{
  u8 i;

  i = 0;
  do
  {
    if (*(g_BtlSlotAmountKinds + i) != 0)
    {
      if (*(g_BtlSlotActiveFlags + i) != 0)
      {
        if (*((u8 *)(CharStats + (i * 0x3C)) + 0x15) != 0)
        {
          BtlSlotStatusAnimationSet(i);
        }
        else
        {
          BtlAnimationSet(g_BtlRecords + (i << 6), g_BtlSlotRestAnimations[i]);
        }
      }
    }
    i = i + 1;
  } while (i < 0xE);
}

/* far_syms: g_BtlPartyHitAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
extern u8 g_BtlPartyHitAnimations[];

/* func_80030E78 */
void BtlPartyHitAnimationSet(void)
{
  s32 i;
  u8 val;
  void *acting_record;
  void *record;
  s16 acting_x;
  s16 record_x;
  i = 0;
  do
  {
    ;
    if (g_BtlSlotAmountKinds[i & 0xFF] != 0)
    {
      switch (g_BtlSlotAmountKinds[i & 0xFF])
      {
        case 2:
          BtlAnimationSet(g_BtlRecords + ((i & 0xFF) << 6), 0x11);
          break;

        case 1:

        case 7:
          BtlAnimationSet(g_BtlRecords + ((i & 0xFF) << 6), g_BtlPartyHitAnimations[i & 0xFF]);
          break;

        default:
          break;

      }

      acting_record = (g_BtlActingSlot << 6) + g_BtlRecords;
      record = ((i & 0xFF) << 6) + g_BtlRecords;
      acting_x = ((BtlRecord *)acting_record)->sprite.x;
      record_x = ((BtlRecord *)record)->sprite.x;
      if (((long) acting_x) != record_x)
      {
        if (acting_x < record_x)
        {
          ((BtlRecord *)record)->sprite.attr = 0x2020;
        }
        else
        {
          ((BtlRecord *)record)->sprite.attr = 0x20;
        }
      }
    }
    i++;
  }
  while (((u32) (i & 0xFF)) < 5U);
}

/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (auto: target reaches it %hi/%lo — addressing row 3) */
void BtlAttackOutcomeClear(void);  
/* func_80030F8C */
void BtlF7Slot7RecordOff(void)
{
  u16 *record;
  if (g_BtlNumber == 0xF7)
  {
    if (g_BtlTargetSlot == 6)
    {
      if ((*(record = (u16 *) (((u8 *) g_BtlRecords) + 0x1C0))) != 0)
      {
        if (((BtlStats *)CharStats)[7].hp == 0)
        {
          BtlAnimationSet(g_BtlRecords + 0x1C0, 0xC);
          BtlAnimationEndWait(g_BtlRecords + 0x1C0);
          ((BtlRecord *)g_BtlRecords)[7].on = 0;
        }
      }
    }
  }
}

s32 BtlAmountShowCheck();                                /* extern */

/* func_80031028 */
void BtlAmountShowWait(void) {
loop_1:
    if (BtlAmountShowCheck() != 0) {
        BtlFrameStep();
        goto loop_1;
    }
}

s32 BtlWalkSet(s32);
s32 BtlHitTry();
s32 BtlHitAmountSet();
s32 BtlHitSlotsUpdate();
s32 BtlAttackWalkRun();
void BtlAttackMissShow(void);  /* extern */
void BtlAttackHitShow(void);  /* extern */
s32 BtlAttackAllRun();
u8 BtlPartyTargetPick(s32);

/* func_80031060 */
void BtlActionAttackRun(void)
{
  s32 all_trait;
  BtlWalkBlock *walk_block;
  u8 target;
  u8 attacks_left;
  void *stats;
  s32 lit_0x4;
  if ((((u8) g_BtlActingSlot) < 5U) && (((stats = (g_BtlActingSlot * 0x3C) + CharStats), all_trait = 0x18, ((BtlStats *) stats)->side.party.traits[4] == all_trait) || (((BtlStats *) stats)->side.party.traits[5] == all_trait)))
  {
    BtlAttackAllRun();
    return;
  }
  BtlWalkSet(all_trait);
  attacks_left = ((BtlStats *) ((g_BtlActingSlot * 0x3C) + CharStats))->attack_count;
  if (attacks_left != 0)
  {
    lit_0x4 = 0x4;
    /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
       the block runs once, so behaviour is the same without the `do` / `while (0)`, but
       then 17 of this function's 110 instruction lines differ. */
    do
    {
      loop_6:
      if ((((u8) g_BtlNumber) >= 0xEBU) && (((u8) g_BtlActingSlot) >= 6U))
      {
        target = BtlPartyTargetPick(2);
        g_BtlTargetSlot = target;
        *(g_BtlActingSlot + g_BtlSlotPlannedTargets) = target;
      }

      if (!(BtlAttackTargetFind() & 0xFF))
      {
        g_BtlFallDone = 0;
        walk_block = &g_BtlWalkBlock;
        walk_block->target = (s32) (g_BtlRecords + (g_BtlTargetSlot << 6));
        BtlAttackOutcomeClear();
        if (BtlAttackWalkRun() & 0xFF)
        {
          if (BtlHitTry() & 0xFF)
          {
            BtlHitAmountSet();
            BtlAttackHitShow();
            BtlHitSlotsUpdate();
            BtlPartyPanelDisp();
            BtlF7Slot7RecordOff();
          }
          else
          {
            BtlAttackMissShow();
          }
        }
        else
        {
          BtlActingRestAnimationSet();
        }
        BtlFrameStep();
        attacks_left -= 1;
        if (attacks_left & 0xFF)
        {
          goto loop_6;
        }
      }
    }
    while (0);
  }
}
