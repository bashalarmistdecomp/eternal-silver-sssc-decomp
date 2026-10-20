/* Lunar SSSC — btl_round: what a round's start does: the party's and the
   enemies' actions changed by their state, their action animations, and the
   statuses counted toward their end. The first split had it in
   rendering2_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
extern s32 g_BtlRecords;
extern u8 g_BtlTargetSlot;
extern u8 g_BtlActingSlot;
extern u8 g_BtlSlotActiveFlags[];
extern s32 CharStats;
extern s32 CharStats;
void BtlAnimationSet(void *record, s32 animation);
s32 BtlFrameStep();
/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
extern u8 g_BtlNumber;
extern u8 g_BtlSlotAmountKinds[];

/* la_absolute: BtlPartyActionAdjust */
/* far_syms: g_BtlSlotPlannedTargets (target reaches it %hi/%lo) */
extern u8 g_BtlPartyNotHeldFlags[5];

/* func_80029348 */
void BtlPartyActionAdjust(void)
{
    s32 result;
    u8 sw;

    g_BtlActingSlot = 0;
    while (1)
    {
        if (g_BtlSlotActiveFlags[g_BtlActingSlot] != 0)
        {
            if (*(u8 *)(CharStats + g_BtlActingSlot * 60 + 0x15) & 0x40)
            {
                result = rng_draw_copy();
                sw = result & 7;
                if (sw >= 5)
                    sw = result & 3;
                switch (sw)
                {
                    case 0:
                        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
                        break;
                    case 1:
                        if (BtlActingSpellPick() & 0xFF)
                            g_BtlSlotPlannedKinds[g_BtlActingSlot] = 3;
                        else
                            g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
                        break;
                    case 2:
                        if ((BtlActingItemPick() & 0xFF) != 0xFF)
                            g_BtlSlotPlannedKinds[g_BtlActingSlot] = 4;
                        else
                            g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
                        break;
                    case 3:
                        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 5;
                        break;
                    case 4:
                        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 6;
                        break;
                }
                g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0x64;
            }
            else
            {
                if (!g_BtlPartyNotHeldFlags[g_BtlActingSlot])
                {
                    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 0xB;
                    g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0;
                }
            }
        }
        g_BtlActingSlot++;
        if (g_BtlActingSlot >= 5)
            break;
    }
}

/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */
extern u8 g_BtlSlotPlannedNumbers[];

/* func_80029510 */
void BtlEnemyActionAdjust(void)
{
    u8 *stats;

    g_BtlActingSlot = 6;
    do
    {
        if (*(g_BtlActingSlot + g_BtlSlotActiveFlags))
        {
            stats = (u8 *)(g_BtlActingSlot * 60 + CharStats);
            if (stats[0x15] & 0x40)
            {
                switch (rng_draw() & 3)
                {
                    case 0:
                    case 1:
                        BtlEnemyActionFallbackSet();
                        break;
                    case 2:
                        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 5;
                        break;
                    case 3:
                        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 6;
                        break;
                }
            }
            else if (stats[0x15] & 0x8)
            {
                *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 0xB;
            }
            else
            {
                if (*(g_BtlActingSlot + g_BtlSlotPlannedKinds) != 5)
                {
                    if (*(g_BtlActingSlot + g_BtlSlotPlannedKinds) != 6)
                    {
                        if (*(g_BtlActingSlot + g_BtlSlotPlannedKinds) == 7)
                        {
                            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = stats[0x1D];
                        }
                    }
                }
            }
        }
    } while (++g_BtlActingSlot < 14);
}

/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
s32 rng_7f_draw();
s32 BtlStatusWearOffCheck(s32 round_count);  /* TU defines this below */

/* func_8002965C */
s32 BtlEnemyActionAnimationSet(void)
{
  u8 next_slot;
  g_BtlActingSlot = 6;
  do
  {
    if (((*(g_BtlActingSlot + g_BtlSlotActiveFlags)) != 0) && ((*((u8 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x15))) == 0))
    {
      if (g_BtlActingSlot)
      {
        BtlSlotActionAnimationSet();
      }
      else
      {
        BtlSlotActionAnimationSet();
      }
    }
    next_slot = g_BtlActingSlot + 1;
    g_BtlActingSlot = next_slot;
  }
  while (((u32) (next_slot & 0xFF)) < 0xEU);
  return BtlFrameStep();
}
/* la_absolute: BtlPartyActionAnimationSet */
/* func_800296F4 */
void BtlPartyActionAnimationSet(void)
{
  for (g_BtlActingSlot = 0; g_BtlActingSlot < 5; g_BtlActingSlot++)
  {
    if (g_BtlSlotActiveFlags[g_BtlActingSlot] != 0 && *((u8 *) (g_BtlActingSlot * 0x3C + CharStats) + 0x15) == 0 && g_BtlPartyNotHeldFlags[g_BtlActingSlot] != 0)
    {
      BtlSlotActionAnimationSet();
    }
  }
  BtlFrameStep();
}

extern u8 g_BtlFallDone;
extern u8 g_BtlPartyStatus1Shares[];
extern u8 g_BtlSlotStatus4Rounds[];
extern u16 g_BtlSlotAmounts[];
extern u8 g_BtlSlotStatus8Rounds[];
extern u8 g_BtlSlotFallMarks[];
extern u8 g_BtlSlotStatus40Rounds[];
extern u8 g_BtlSlotStatus80Rounds[];
s32 BtlActionOutcomeClear();
void BtlSlotStatusAnimationSet(u8);
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
s32 rng_mask_below(s32, s32);
void BtlAmountRecordsRun(void);
void BtlAnimationSet(void *record, s32 animation);
s32 BtlFallRun();
s32 BtlItem83Revive();

/* func_800297A4 */
void BtlRoundStatusUpdate(void)
{
    u8 i;
    u8 *st;
    u8 *cnt;
    BtlStats *p;
    u8 flag;
    u16 regen;
    u8 s;
    BtlStats *q;
    BtlStats *r;
    u8 party_status;
    u8 *t;
    s32 dmg;
    u8 poisoned;

    BtlActionOutcomeClear();
    i = 0;
    do {
        if (g_BtlSlotActiveFlags[i] != 0) {
            p = (BtlStats *)(i * 0x3C + CharStats);
            t = &p->status;
            s = *t;
            if (s != 0) {
                st = t;
                if ((s & 4) && g_BtlNumber != 0xF9) {
                    cnt = &g_BtlSlotStatus4Rounds[i];
                    *cnt += 1;
                    if ((u8)BtlStatusWearOffCheck(*cnt)) {
                        p->status ^= 4;
                        *cnt = 0;
                    }
                }
                if (*st & 8) {
                    g_BtlSlotStatus8Rounds[i] += 1;
                    if ((u8)BtlStatusWearOffCheck(g_BtlSlotStatus8Rounds[i])) {
                        *st ^= 8;
                        if (i < 5) {
                            ((BtlStats *)(i * 0x3C + CharStats))->side.party.flags &= 0xFB;
                        }
                        g_BtlSlotStatus8Rounds[i] = 0;
                    }
                }
                if (*st & 0x40) {
                    g_BtlSlotStatus40Rounds[i] += 1;
                    if ((u8)BtlStatusWearOffCheck(g_BtlSlotStatus40Rounds[i])) {
                        *st ^= 0x40;
                        g_BtlSlotStatus40Rounds[i] = 0;
                    }
                }
                if (*st & 0x80) {
                    g_BtlSlotStatus80Rounds[i] += 1;
                    if ((u8)BtlStatusWearOffCheck(g_BtlSlotStatus80Rounds[i])) {
                        *st ^= 0x80;
                        g_BtlSlotStatus80Rounds[i] = 0;
                    }
                }
                BtlSlotStatusAnimationSet(i);
            }
        }
        i++;
    } while (i < 14);
    flag = 0;
    g_BtlTargetSlot = 0;
    do {
        g_BtlSlotAmountKinds[g_BtlTargetSlot] = g_BtlSlotPlannedKinds[g_BtlTargetSlot] = g_BtlSlotPlannedNumbers[g_BtlTargetSlot] =
            g_BtlSlotPlannedTargets[g_BtlTargetSlot] = g_BtlSlotFallMarks[g_BtlTargetSlot] = 0;
        g_BtlSlotAmounts[g_BtlTargetSlot] = 0;
        if (g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0) {
            regen = 0;
            if ((u8)BtlTargetTraitCheck(0, 0x1D, 0, 0) == 1) {
                regen = 10;
            }
            if (g_BtlTargetSlot < 5 && (((BtlStats *)(g_BtlTargetSlot * 0x3C + CharStats))->side.party.flags & 1)) {
                regen += 5;
            }
            if (regen != 0) {
                flag = 1;
                regen = ((BtlStats *)CharStats)[g_BtlTargetSlot].hp_max * regen / 100;
                regen += rng_mask_below(3, 4);
                ((BtlStats *)CharStats)[g_BtlTargetSlot].hp += regen;
                if (((BtlStats *)CharStats)[g_BtlTargetSlot].hp >= ((BtlStats *)CharStats)[g_BtlTargetSlot].hp_max) {
                    regen -= ((BtlStats *)CharStats)[g_BtlTargetSlot].hp - ((BtlStats *)CharStats)[g_BtlTargetSlot].hp_max;
                    ((BtlStats *)CharStats)[g_BtlTargetSlot].hp = ((BtlStats *)CharStats)[g_BtlTargetSlot].hp_max;
                }
                g_BtlSlotAmounts[g_BtlTargetSlot] = regen;
                g_BtlSlotAmountKinds[g_BtlTargetSlot] = 5;
            }
        }
    } while (++g_BtlTargetSlot < 14);
    if (flag) {
        BtlAmountRecordsRun();
    }
    for (i = 0; i < 5; i++) {
        cnt = &g_BtlSlotActiveFlags[i];
        if (*cnt != 0) {
            q = (BtlStats *)(CharStats + i * 0x3C);
            party_status = q->status;
            if (party_status != 0) {
                if (party_status & 0x10) {
                    q->status = 0x20;
                    BtlAnimationSet((void *)(g_BtlRecords + (i << 6)), 0x18);
                } else if (party_status & 0x20) {
                    q->status = 0x30;
                    BtlAnimationSet((void *)(g_BtlRecords + (i << 6)), 0x19);
                    *cnt = 0;
                }
            }
        }
    }
    flag = 0;
    poisoned = 0;
    for (i = 0; i < 5; i++) {
        g_BtlSlotAmountKinds[i] = 0;
        g_BtlSlotAmounts[i] = 0;
        q = (BtlStats *)(CharStats + i * 0x3C);
        if (q->status & 1) {
            g_BtlSlotAmountKinds[i] = 1;
            t = g_BtlPartyStatus1Shares;
            r = (BtlStats *)(i * 0x3C + CharStats);
            dmg = r->hp_max * t[i] / 100;
            g_BtlSlotAmounts[i] = dmg;
            poisoned = 1;
            if ((u16)dmg >= r->hp) {
                flag = 1;
                g_BtlSlotFallMarks[i] = 1;
            } else {
                r->hp -= dmg;
                if (t[i] != 0x14) {
                    t[i] += 2;
                }
            }
        }
    }
    if (poisoned) {
        BtlAmountRecordsRun();
    }
    if (flag) {
        g_BtlFallDone = 0;
        BtlFallRun();
        BtlItem83Revive();
    }
    i = poisoned = 0;
    g_BtlTargetSlot = 0;
    for (; i < 14; i++) {
        g_BtlSlotAmountKinds[i] = g_BtlSlotPlannedKinds[i] = g_BtlSlotPlannedNumbers[i] = g_BtlSlotPlannedTargets[i] = g_BtlSlotFallMarks[i] = 0;
        g_BtlSlotAmounts[i] = 0;
    }
}

s32 rng_7f_draw();

/* func_80029E30 */
s32 BtlStatusWearOffCheck(s32 round_count)
{
  u32 masked_limit;
  int prior_rounds;
  unsigned long limit;
  char draw;
  prior_rounds = (limit = round_count - 1) & 0xFF;
  limit = 5;
  limit = prior_rounds << limit;
  draw = rng_7f_draw() & 0xFF;
  limit = limit & 0xFF;
  ;
  masked_limit = (unsigned char) ((u32) limit);
  ;
  return ((u32) draw) < masked_limit;
}
