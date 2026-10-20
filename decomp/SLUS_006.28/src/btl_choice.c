/* Lunar SSSC — btl_choice: the choices the program makes: an enemy's action
   and target picked, and the party's own choice for each slot (one, all, by
   need, spells, items, attacks, the turn's action), with the hp, mp, status
   and spell finds and counts they rest on. The first split had it in
   misc_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
/* far_syms: g_BtlPartyNotHeldFlags (carried from the first split's misc.c) */
extern s32 CharStats;
extern u8 g_BtlSpellRowsByNumber[];
extern u8 g_BtlEnemyMostSlot;
extern u8 g_BtlEnemyMostCount;
extern u16 g_BtlPartyStat08Sum;
extern volatile char g_BtlPartyCount;
extern s16 g_BtlEnemyStat08Sum;
extern u8 g_BtlEnemyCount;
extern u8 g_BtlActingSlot;
extern u8 g_BtlPartyNotHeldFlags[];
extern s32 g_BtlRecords;
extern u8 D_800B8C65;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlNumber;
extern s8 g_BtlEnemyChoice[3];
extern s16 g_BtlPartyFoundSlot;
extern s16 g_BtlPartyFoundNeed;
extern u8 g_BtlPartyKeptTargets[8];
extern u8 g_BtlPartyKeptKinds[8];
extern u8 g_BtlPartyKeptNumbers[8];
extern u8 g_BtlPartyChoiceKind;  /* TU declares this below */
extern u8 g_BtlPartyChoiceNumber;  /* TU declares this below */
extern u8 g_BtlPartyChoiceTarget;  /* TU declares this below */
extern u8 g_BtlSlotIds[];
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlSlotPlannedNumbers[];
s32 StoryBitCheck(s32);

/* func_8004CAB8 */
void BtlEnemyActionPick(void)
{
  s32 type;
  u8 r;

  if ((u16) g_BtlEnemyStat08Sum < (u16) (g_BtlPartyStat08Sum / 20))
  {
    g_BtlEnemyChoice[0] = 6;
    g_BtlEnemyChoice[1] = 0;
    g_BtlEnemyChoice[2] = 0;
    return;
  }
  type = ((u8 *) (g_BtlActingSlot * 0x3C + CharStats))[0x1A];
  switch (type)
  {
    case 1:
      if ((u16) g_BtlEnemyStat08Sum < (u16) (g_BtlPartyStat08Sum >> 1))
      {
        g_BtlEnemyChoice[0] = 6;
        g_BtlEnemyChoice[1] = 0;
        g_BtlEnemyChoice[2] = 0;
        break;
      }
      if ((s8) rng_ff_draw() >= 0)
        BtlEnemyAttackPick(0);
      else
        BtlEnemyKind7Pick(0);
      break;
    case 2: case 3: case 4: case 5: case 6:
      BtlEnemyAttackPick((u8) (type - 2));
      break;
    case 7: case 8: case 9: case 10: case 11:
      BtlEnemyKind7Pick((u8) (type - 7));
      break;
    case 12: case 13: case 14: case 15: case 16:
      if ((u8) rng_ff_draw() < 0x55)
        BtlEnemyKind7Pick((u8) (type - 12));
      else
        BtlEnemyAttackPick((u8) (type - 12));
      break;
    case 17: case 18: case 19: case 20: case 21:
      if ((u8) rng_ff_draw() < 0x55)
        BtlEnemyAttackPick((u8) (type - 17));
      else
        BtlEnemyKind7Pick((u8) (type - 17));
      break;
    case 22:
      BtlEnemyAttackPick(5);
      break;
    case 23:
      BtlEnemyKind7Pick(5);
      break;
    case 24:
      if ((u8) rng_ff_draw() < 0x55)
        BtlEnemyKind7Pick(4);
      else
        BtlEnemyAttackPick(5);
      break;
    case 25:
      if ((u8) rng_ff_draw() < 0x55)
        BtlEnemyAttackPick(2);
      else
        BtlEnemyKind7Pick(5);
      break;
    case 26: case 27: case 28: case 29: case 30: case 31:
      BtlEnemyAttackKind7Pick((u8) (type - 26));
      break;
    case 32:
      g_BtlEnemyChoice[0] = 6;
      g_BtlEnemyChoice[1] = 0;
      g_BtlEnemyChoice[2] = 0;
      break;
    case 0:
    default:
      if ((u8) rng_ff_draw() < 0x40)
      {
        if ((s8) rng_ff_draw() >= 0)
          BtlEnemyAttackPick(0);
        else
          BtlEnemyKind7Pick(1);
      }
      else
      {
        r = rng_ff_draw();
        if (r >= 0x81)
          BtlEnemyAttackPick(0);
        else if (r < 0x40)
          BtlEnemyKind7Pick((r >= 0x20) ? 2 : 1);
        else
          BtlEnemyKind7Pick((r >= 0x62) ? 4 : 3);
      }
      break;
  }
}

/* func_8004CD08 */
void BtlEnemyAttackKind7Pick(s32 target_rule) {
    s32 ret;

    ret = rng_ff_draw();
    if (!(ret << 24 < 0)) {
        BtlEnemyAttackPick(target_rule & 0xFF);
    } else {
        BtlEnemyKind7Pick(target_rule & 0xFF);
    }
}

s32 memset();

/* func_8004CD50 */
u8 BtlPartyTargetHpPick(void)
{
    u8 buf[5];
    u8 i;
    u8 n;

    memset(buf, 0, 5);
    n = 0;
    for (i = 0; i < 5; i++) {
        if (*(u16 *)(i * 0x3C + CharStats) != 0) {
            buf[n] = i;
            n++;
        }
    }
    switch (n) {
    case 1:
        i = 0;
        break;
    case 2:
        i = (u8)rng_ff_draw() >> 7;
        break;
    case 3:
        i = (u8)rng_ff_draw() / 86;
        break;
    case 4:
        i = (u8)rng_ff_draw() >> 6;
        break;
    case 5:
        i = (u8)rng_ff_draw() / 52;
        break;
    }
    return buf[i];
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_8001256C_pad);

u16 BtlPartyTargetHpFind();
u16 BtlPartyTargetHpBelowFFFind();
ushort BtlPartyTargetStat08Find();
int BtlPartyTargetStat08BelowFFFind();

/* func_8004CE70 */
void BtlEnemyAttackPick(s32 target_rule)
{
  s8 BtlSlotNearestFind(s32);
  s8 target;
  u8 rule;
  rule = target_rule & 0xFF;
  target = 0;
  if (rng_ff_draw() & 0xFF)
  {
    rule = 5 & 0xFF;
  }
  switch (rule)
  {
    case 0:
      target = BtlSlotNearestFind(2);
      break;

    case 1:
      target = BtlPartyTargetHpFind();
      break;

    case 2:
      target = BtlPartyTargetHpBelowFFFind();
      break;

    case 3:
      target = BtlPartyTargetStat08Find();
      break;

    case 4:
      target = BtlPartyTargetStat08BelowFFFind();
      break;

    case 5:
      target = BtlPartyTargetHpPick();
      break;

  }

  *(((u8 *) (&g_BtlEnemyChoice)) + 0x2) = target;
  *(((u8 *) (&g_BtlEnemyChoice)) + 0x0) = 2;
  *((u8 *) (((u8 *) (&g_BtlEnemyChoice)) + 0x1)) = 0;
}

extern u8 g_BtlAttackRows[];

/* func_8004CF48 */
void BtlEnemyKind7Pick(s32 target_rule)
{
  s8 BtlSlotNearestFind(s32);
  s8 target;
  u8 rule;
  u8 inner;

  rule = target_rule & 0xFF;
  target = 0;
  if (rng_ff_draw() & 0xFF)
  {
    rule = 5 & 0xFF;
  }
  switch (rule)
  {
    case 0: target = BtlSlotNearestFind(2); break;
    case 1: target = BtlPartyTargetHpFind(); break;
    case 2: target = BtlPartyTargetHpBelowFFFind(); break;
    case 3: target = BtlPartyTargetStat08Find(); break;
    case 4: target = BtlPartyTargetStat08BelowFFFind(); break;
    case 5: target = BtlPartyTargetHpPick(); break;
  }

  if (*(u8 *)((u32)g_BtlAttackRows + ((*(u8 *)(CharStats + g_BtlActingSlot * 60 + 0x1D)) << 3)) == 7)
  {
    inner = *(u8 *)(CharStats + (target & 0xFF) * 60 + 0x15);
    if (inner == 4 || inner == 8 || inner == 0x30)
    {
      g_BtlEnemyChoice[0] = 2;
      g_BtlEnemyChoice[1] = 0;
      g_BtlEnemyChoice[2] = target;
    }
    else
    {
      g_BtlEnemyChoice[0] = 7;
      g_BtlEnemyChoice[1] = 0;
      g_BtlEnemyChoice[2] = target;
    }
  }
  else
  {
    g_BtlEnemyChoice[0] = 7;
    g_BtlEnemyChoice[1] = 0;
    g_BtlEnemyChoice[2] = target;
  }
}

/* no_split_addresses: BtlPartyTargetHpFind */

/* func_8004D0A0 */
u16 BtlPartyTargetHpFind(void) {
    u16 buf[2];
    u8 i;

    memset(buf, 0, sizeof(buf));
    for (i = 0; i < 5; i++) {
        if (buf[1] < *(u16 *)((u8 *)CharStats + i * 0x3C)) {
            buf[0] = i;
        }
    }
    return buf[0];
}

/* no_split_addresses: BtlPartyTargetHpBelowFFFind */

/* func_8004D11C */
u16 BtlPartyTargetHpBelowFFFind(void) {
    u16 result[2];
    u8 i;
    u16 c;
    u16 val;

    c = 0xFF;
    result[0] = 0;
    result[1] = c;
    for (i = 0; i < 5; i++) {
        val = *(u16 *)((u8 *)CharStats + i * 0x3C);
        if (val != 0 && val < c) {
            result[0] = i;
        }
    }
    return result[0];
}

/* no_split_addresses: BtlPartyTargetStat08Find */

/* func_8004D18C */
ushort BtlPartyTargetStat08Find(void)
{
  u16 buf[2];
  u8 i;
  u8 *p;
  memset(buf, 0, 4);
  p = (u8 *) CharStats;
  for (i = 0; i < 5; i++)
  {
    if (((*((u16 *) (p + (i * 0x3C)))) != 0) && (buf[1] < (*((u16 *) ((p + (i * 0x3C)) + 8)))))
    {
      buf[0] = i;
    }
  }

  return buf[0];
}

/* func_8004D218 */
int BtlPartyTargetStat08BelowFFFind(void)
{
  u16 result[2];
  u16 c;
  u8 i;
  u16 val;
  u16 *p;
  u8 *base;
  c = 0xFF;
  result[0] = 0;
  result[1] = c;
  base = (u8 *) CharStats;
  for (i = 0; i <= (5 - 1); i++)
  {
    val = *((u16 *) (base + (i * 0x3C)));
    if (val != 0)
    {
      p = (u16 *) ((base + (i * 0x3C)) + 8);
      if ((*p) < c)
      {
        result[0] = i;
      }
    }
  }

  return result[0];
}

s32 rng_draw();                                /* extern */

/* func_8004D294 */
s32 rng_ff_draw(void) {
    return rng_draw() & 0xFF;
}

/* Ruling of 2026-10-14 (docs/naming.md): Returns 0, and nothing reaches it: nothing in the program supports a name. */
long long func_8004D2B4(void)
{
    return 0;
}

/* far_syms: g_BtlActingSlot */
/* la_absolute: BtlPartyActionSet */
extern u8 g_BtlActingSpellList;
extern u8 D_800B87A1;
extern u8 D_800B87A2;
extern u8 D_800B87A3;
extern u8 D_800B87A4;
extern u8 D_800B87A5;
extern u8 D_800B87A6;
extern u8 D_800B87A7;
u8 BtlEnemyStat08Sum(void);
u8 BtlPartyStat08Sum(void);
void BtlPartyAllChoiceSet(void);
void BtlPartyOneChoiceSet(void);

/* func_8004D2C4 */
void BtlPartyActionSet(s32 mode)
{
  u8 *rec;
  (&g_BtlEnemyCount)[1] = mode;
  D_800B87A7 = 0;
  D_800B87A6 = 0;
  D_800B87A5 = 0;
  D_800B87A4 = 0;
  D_800B87A3 = 0;
  D_800B87A2 = 0;
  D_800B87A1 = 0;
  g_BtlActingSpellList = 0;
  g_BtlEnemyMostCount = 0;
  g_BtlEnemyMostSlot = 0;
  g_BtlPartyFoundNeed = 0;
  g_BtlPartyFoundSlot = 0;
  g_BtlPartyChoiceTarget = 0;
  g_BtlPartyChoiceNumber = 0;
  g_BtlPartyChoiceKind = 0;
  D_800B8C65 = 0;
  g_BtlPartyCount = 0;
  g_BtlEnemyCount = 0;
  g_BtlPartyStat08Sum = 0;
  g_BtlEnemyStat08Sum = 0;
  D_800B8C65 = 0;
  BtlEnemyStat08Sum();
  BtlPartyStat08Sum();
  if ((mode &= 0xFF) != 0)
  {
    for (g_BtlActingSlot = 0; g_BtlActingSlot < 5; g_BtlActingSlot++)
    {
      if (g_BtlSlotIds[g_BtlActingSlot] != 0)
      {
        rec = (u8 *) (g_BtlActingSlot * 0x3C + CharStats);
        if ((*(u16 *) rec != 0) && (g_BtlPartyNotHeldFlags[g_BtlActingSlot] != 0) && (rec[0x15] != 4) && (rec[0x15] != 8) && (rec[0x15] != 0x30))
        {
          BtlPartyAllChoiceSet();
          g_BtlSlotPlannedKinds[g_BtlActingSlot] = g_BtlPartyChoiceKind;
          g_BtlSlotPlannedNumbers[g_BtlActingSlot] = g_BtlPartyChoiceNumber;
          g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0xFE;
          g_BtlPartyKeptKinds[g_BtlActingSlot] = g_BtlPartyChoiceKind;
          g_BtlPartyKeptNumbers[g_BtlActingSlot] = g_BtlPartyChoiceNumber;
          g_BtlPartyKeptTargets[g_BtlActingSlot] = 0xFE;
        }
        else
        {
          g_BtlSlotPlannedKinds[g_BtlActingSlot] = 5;
          g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0xFF;
          g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0xFE;
          g_BtlPartyKeptKinds[g_BtlActingSlot] = 5;
          g_BtlPartyKeptNumbers[g_BtlActingSlot] = 0xFF;
          g_BtlPartyKeptTargets[g_BtlActingSlot] = 0xFE;
        }
        if (g_BtlSlotPlannedKinds[g_BtlActingSlot] == 0)
        {
          while (1)
          {
          }
        }
      }
    }
  }
  else
  {
    BtlPartyOneChoiceSet();
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = g_BtlPartyChoiceKind;
    g_BtlSlotPlannedNumbers[g_BtlActingSlot] = g_BtlPartyChoiceNumber;
    g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0xFE;
    g_BtlPartyKeptKinds[g_BtlActingSlot] = g_BtlPartyChoiceKind;
    g_BtlPartyKeptNumbers[g_BtlActingSlot] = g_BtlPartyChoiceNumber;
    g_BtlPartyKeptTargets[g_BtlActingSlot] = 0xFE;
  }
}

/* far_syms: g_BtlNumber */
s32 BtlSlotHpLowCheck(s32 slot);  /* TU defines this below */
int BtlEnemyFourIdsCheck(void);  /* TU defines this below */
s32 BtlPartyStatusFind(void);  /* TU defines this below */
void BtlPartyLeaveChoiceSet(void);  /* TU defines this below */
u8 BtlSlotMpLowCheck(u8 slot, u8 half);  /* TU defines this below */
s32 BtlPartyNeedItemSet(s32 need, s32 target);  /* TU defines this below */
void BtlPartyAttackChoiceSet(void);  /* TU defines this below */

/* func_8004D61C */
void BtlPartyOneChoiceSet(void)
{
    u8 id;

    if ((u8)BtlSlotHpLowCheck(g_BtlActingSlot)) {
        if ((u8)BtlPartyNeedItemSet(9, g_BtlActingSlot)) {
            id = g_BtlSlotIds[g_BtlActingSlot];
            if (id == 3 || id == 6) {
                if ((u8)BtlPartyNeedSpellSet(9, g_BtlActingSlot)) {
                    g_BtlPartyChoiceKind = 5;
                    g_BtlPartyChoiceNumber = 0xFF;
                    g_BtlPartyChoiceTarget = 0xFF;
                }
            } else if ((u8)BtlPartyOtherSideSpellSet()) {
                if (g_BtlPartyCount >= 2) {
                    g_BtlPartyChoiceKind = 5;
                    g_BtlPartyChoiceNumber = 0xFF;
                    g_BtlPartyChoiceTarget = 0xFF;
                } else {
                    BtlPartyLeaveChoiceSet();
                }
            }
        }
        return;
    }
    id = g_BtlSlotIds[g_BtlActingSlot];
    if (id != 7 && id != 8 && BtlSlotMpLowCheck(g_BtlActingSlot, 0)) {
        if ((u8)BtlPartyNeedItemSet(8, g_BtlActingSlot)) {
            BtlPartyAttackChoiceSet();
        }
        return;
    }
    switch (g_BtlSlotIds[g_BtlActingSlot]) {
    case 1:
    case 11:
        if ((g_BtlPartyStat08Sum / 3) * 2 < (u16)g_BtlEnemyStat08Sum
            || (BtlEnemyFourIdsCheck() & 0xFF) == 1 || g_BtlNumber >= 0xDF) {
            if ((u8)BtlPartyOtherSideSpellSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 2:
        if ((g_BtlPartyStat08Sum / 3) * 2 < (u16)g_BtlEnemyStat08Sum || g_BtlNumber >= 0xDF) {
            if ((u8)BtlPartyOtherSideSpellSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else if (!(u8)BtlPartyHpLeastFind() || (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot)) {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 3:
        if ((u8)BtlPartyHpLeastFind()) {
            if ((u8)BtlPartyNeedSpellSet(9, (u8)g_BtlPartyFoundSlot) && (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot)) {
                BtlPartyAttackChoiceSet();
            }
        } else if ((u8)BtlPartyStatusFind()) {
            if ((u8)BtlPartyNeedSpellSet((u8)g_BtlPartyFoundNeed, (u8)g_BtlPartyFoundSlot) && (u8)BtlPartyNeedChoiceSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 4:
        if ((g_BtlPartyStat08Sum / 3) * 2 < (u16)g_BtlEnemyStat08Sum
            || (BtlEnemyFourIdsCheck() & 0xFF) == 1) {
            if ((u8)BtlPartyOtherSideSpellSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else if (!(u8)BtlPartyHpLeastFind() || (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot)) {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 5:
        if ((g_BtlPartyStat08Sum >> 1) < (u16)g_BtlEnemyStat08Sum
            || (BtlEnemyFourIdsCheck() & 0xFF) == 1 || g_BtlNumber >= 0xDF) {
            if ((u8)BtlPartyOtherSideSpellSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else if (!(u8)BtlPartyHpLeastFind() || (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot)) {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 6:
        if ((u8)BtlPartyHpLeastFind()) {
            if ((u8)BtlPartyNeedSpellSet(9, (u8)g_BtlPartyFoundSlot) && (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot)) {
                BtlPartyAttackChoiceSet();
            }
        } else if ((u8)BtlPartyStatusFind()) {
            if ((u8)BtlPartyNeedSpellSet((u8)g_BtlPartyFoundNeed, (u8)g_BtlPartyFoundSlot) && (u8)BtlPartyNeedChoiceSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else if ((*(u8 *)&g_BtlPartyCount) == 1) {
            /* the TU declares g_BtlPartyCount volatile (for other users); this read
               is a plain one in the target — reorg steals the `li 1` into the
               preceding branch's delay slot, which a volatile load blocks.
               Same idiom as BtlPartyLeaveChoiceSet's loop bound above. */
            if ((u16)g_BtlEnemyStat08Sum > g_BtlPartyStat08Sum && !(u8)BtlActingSpellListMake()
                && (u8)BtlActingSpellUsableGet(4)) {
                g_BtlPartyChoiceKind = 3;
                g_BtlPartyChoiceNumber = 0x24;
                g_BtlPartyChoiceTarget = 0xFF;
            }
        } else {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 8:
        if ((g_BtlPartyStat08Sum >> 1) < (u16)g_BtlEnemyStat08Sum || g_BtlNumber >= 0xDF) {
            if ((u8)BtlPartyOtherSideSpellSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 10:
        if ((g_BtlPartyStat08Sum / 3) * 2 < (u16)g_BtlEnemyStat08Sum || g_BtlNumber >= 0xDF) {
            if ((u8)BtlPartyOtherSideSpellSet()) {
                BtlPartyAttackChoiceSet();
            }
        } else if (!(u8)BtlPartyHpLeastFind() || (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot)) {
            BtlPartyAttackChoiceSet();
        }
        break;
    case 7:
    case 9:
        if (!(u8)BtlPartyHpLeastFind() || (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot)) {
            BtlPartyAttackChoiceSet();
        }
        break;
    }
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800125B4_pad);
s32 BtlPartyMpSlotGet(void);  /* TU defines this below */

/* func_8004DB30 */
void BtlPartyAllChoiceSet(void)
{
    s32 ok;

    switch (g_BtlSlotIds[g_BtlActingSlot])
    {
    case 1:
    case 11:
        if (g_BtlPartyStat08Sum / 3 * 2 < (u16)g_BtlEnemyStat08Sum || (u8)BtlEnemyFourIdsCheck() == 1 || g_BtlNumber >= 0xDF)
        {
            ok = (u8)BtlPartyOtherSideSpellSet();
            goto check;
        }
        break;
    case 2:
        if (g_BtlPartyStat08Sum / 3 < (u16)g_BtlEnemyStat08Sum || g_BtlNumber >= 0xDF)
        {
            ok = (u8)BtlPartyOtherSideSpellSet();
            goto check;
        }
        break;
    case 3:
        if ((u8)BtlPartyHpLeastFind())
        {
            if (!(u8)BtlPartyNeedSpellSet(9, (u8)g_BtlPartyFoundSlot))
                return;
dfa:
            ok = (u8)BtlPartyNeedChoiceSet();
            goto check;
        }
        if ((u8)BtlPartyStatusFind())
        {
            if (!(u8)BtlPartyNeedSpellSet((u8)g_BtlPartyFoundNeed, (u8)g_BtlPartyFoundSlot))
                return;
            goto dfa;
        }
        if (!BtlSlotMpLowCheck(BtlPartyMpSlotGet(), 1))
            goto call;
        ok = (u8)BtlPartyNeedItemSet(8, (u8)BtlPartyMpSlotGet());
        goto check;
    case 4:
        if (g_BtlPartyStat08Sum / 3 < (u16)g_BtlEnemyStat08Sum || (u8)BtlEnemyFourIdsCheck() == 1 || g_BtlNumber >= 0xDF)
        {
            ok = (u8)BtlPartyOtherSideSpellSet();
            goto check;
        }
        break;
    case 5:
        if (g_BtlPartyStat08Sum / 2 < (u16)g_BtlEnemyStat08Sum || (u8)BtlEnemyFourIdsCheck() == 1 || g_BtlNumber >= 0xDF)
        {
            ok = (u8)BtlPartyOtherSideSpellSet();
            goto check;
        }
        break;
    case 6:
        if ((u8)BtlPartyHpLeastFind())
        {
            if (!(u8)BtlPartyNeedSpellSet(9, (u8)g_BtlPartyFoundSlot))
                return;
            goto dfa;
        }
        if ((u8)BtlPartyStatusFind())
        {
            if (!(u8)BtlPartyNeedSpellSet((u8)g_BtlPartyFoundNeed, (u8)g_BtlPartyFoundSlot))
                return;
            goto dfa;
        }
        if (*(u8 *)&g_BtlPartyCount == 1)
        {
            if ((u16)g_BtlEnemyStat08Sum > (u16)g_BtlPartyStat08Sum && !(u8)BtlActingSpellListMake())
            {
                if ((u8)BtlActingSpellUsableGet(4))
                {
                    g_BtlPartyChoiceKind = 3;
                    g_BtlPartyChoiceNumber = 0x24;
                    g_BtlPartyChoiceTarget = 0xFF;
                }
                return;
            }
            goto call;
        }
        if (!BtlSlotMpLowCheck(BtlPartyMpSlotGet(), 1))
            goto call;
        ok = (u8)BtlPartyNeedItemSet(8, (u8)BtlPartyMpSlotGet());
        goto check;
    case 7:
        if ((u8)BtlPartyHpLeastFind())
        {
            ok = (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot);
            goto check;
        }
        break;
    case 8:
        if (g_BtlPartyStat08Sum / 3 < (u16)g_BtlEnemyStat08Sum || g_BtlNumber >= 0xDF)
        {
            ok = (u8)BtlPartyOtherSideSpellSet();
            goto check;
        }
        break;
    case 9:
        if ((u8)BtlPartyHpLeastFind())
        {
            ok = (u8)BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot);
            goto check;
        }
        break;
    case 10:
        if (g_BtlPartyStat08Sum / 3 * 2 < (u16)g_BtlEnemyStat08Sum || g_BtlNumber >= 0xDF)
        {
            ok = (u8)BtlPartyOtherSideSpellSet();
            goto check;
        }
        break;
    default:
        return;
    }
    ok = (u8)BtlPartyNeedChoiceSet();
check:
    if (ok)
    {
call:
        BtlPartyAttackChoiceSet();
    }
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800125E4_pad);
/* far_syms: g_BtlActingSlot (target reaches it %hi/%lo) */
s32 BtlPartyHpShortTwoCheck(void);  /* TU defines this below */
s32 BtlSlotPlannedTargetCheck(u16 target);  /* TU defines this below */

/* func_8004DFA8 */
s32 BtlPartyNeedChoiceSet(void)
{
  s32 kind_then_slot;
  u8 result;
  s32 answer;
  u8 id;
  u8 r;

  result = 1;
  id = g_BtlSlotIds[g_BtlActingSlot];
  kind_then_slot = 3;
  switch (id)
  {
    case 3:
    case 6:
      answer = BtlPartyHpShortTwoCheck();
      if ((u8)answer == 0)
        goto LABEL_1;
      answer = BtlActingSpellListMake();
      if ((u8)answer != 0)
        goto LABEL_1;
      id = g_BtlSlotIds[g_BtlActingSlot];
      switch (id)
      {
        case 3:
          r = BtlActingSpellUsableGet(2);
          break;
        case 6:
          r = BtlActingSpellUsableGet(6);
          break;
        default:
          goto LABEL_1;
      }
      if (r != 0)
      {
        result = 0;
        g_BtlPartyChoiceKind = kind_then_slot;
        g_BtlPartyChoiceNumber = r;
        g_BtlPartyChoiceTarget = 0xFF;
      }
      goto LABEL_1;
    default:
      goto LABEL_1;
  }

LABEL_1:
  if ((u8)result == 0)
    goto LABEL_4;
  answer = BtlPartyHpLeastFind();
  if ((u8)answer != 0)
    result = BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot);
  if ((u8)result == 0)
    goto LABEL_4;
  answer = BtlPartyStatusFind();
  if ((u8)answer != 0)
    result = BtlPartyNeedItemSet((u8)g_BtlPartyFoundNeed, (u8)g_BtlPartyFoundSlot);
  if ((u8)result == 0)
    goto LABEL_4;
  kind_then_slot = 0;
  do
  {
    answer = BtlSlotMpLowCheck((u8)kind_then_slot, 0);
    if ((u8)answer != 0)
    {
      id = g_BtlSlotIds[g_BtlActingSlot];
      if (id == 7)
        goto LABEL_inc;
      if (id == 8)
        goto LABEL_inc;
      answer = BtlSlotPlannedTargetCheck((u8)kind_then_slot);
      if ((u8)answer == 0)
      {
        result = ((u8)BtlPartyNeedItemSet(8, (u8)kind_then_slot) > 0);
        goto LABEL_4;
      }
    }
  LABEL_inc:
    kind_then_slot++;
    result = 1;
  } while ((u8)kind_then_slot < 5);

LABEL_4:
  return (u8)result;
}

/* func_8004E194 */
int BtlEnemyFourIdsCheck(void)
{
    unsigned char i;
    int result;
    u8 val;

    result = 0;
    for (i = 6; i < 0xE; i++) {
        val = g_BtlSlotIds[i];
        if (val == 0x2F || val == 0x38 || val == 0x3E) {
            result = 1;
            break;
        }
        if (val == 0x3F) {
            result = 1;
            break;
        }
    }
    return result;
}

/* func_8004E204 */
s32 BtlPartyHpShortTwoCheck(void)
{
    u8 result;
    u8 count;
    u8 i;
    u16 *p;

    result = 0;
    count = 0;
    for (i = 0; i < (*(u8 *)&g_BtlPartyCount); i++)
    {
        p = (u16 *)((i * 60) + CharStats);
        if (p[0] != 0 && p[0] < p[1] / 3 * 2)
        {
            count++;
        }
    }
    if (count >= 2)
    {
        result = 1;
    }
    return result;
}

/* Ruling of 2026-10-14 (docs/naming.md): Returns 0 in this build; named from what its one caller does with the answer. */
/* func_8004E2B0 */
s32 BtlPartyMpSlotGet(void) {
    return 0;
}

extern u8 g_BtlPartyChoiceKind;
extern u8 g_BtlPartyChoiceNumber;
extern u8 g_BtlPartyChoiceTarget;

/* func_8004E2B8 */
void BtlPartyLeaveChoiceSet(void)
{
  if (g_BtlEnemyCount == 1)
  {
    if (((u16) g_BtlPartyStat08Sum) < ((u8) g_BtlPartyCount))
    {
      BtlPartyAttackChoiceSet();
      return;
    }
    goto block_4;
  }
  block_4:
  g_BtlPartyChoiceKind = 6;

  g_BtlPartyChoiceNumber = 0xFF;
  g_BtlPartyChoiceTarget = 0xFF;
}

/* far_syms: CharStats (target reaches it %hi/%lo) */
s32 BtlEnemyCountBelow4Check(void);  /* TU defines this below */
s32 BtlPartyPlannedCount(void);  /* TU defines this below */
u8 BtlSpellByte0BGet(s32 spell);  
/* func_8004E318 */
s32 BtlSlotHpLowCheck(s32 slot)
{
  u16 *p;
  slot &= 0xFF;
  return ((u16 *) CharStats)[(slot * 30)] < (((u16 *) CharStats)[(slot * 30) + 1] >> 2);
}

/* func_8004E348 */
u8 BtlSlotMpLowCheck(u8 slot, u8 half) {
    u8 result;

    result = 0;
    if (half == 0) {
        u16 f4 = *(u16 *)((u8 *)CharStats + slot * 0x3C + 4);
        u16 f6 = *(u16 *)((u8 *)CharStats + slot * 0x3C + 6);
        result = (f4 < f6 / 8);
    } else {
        u16 f4 = *(u16 *)((u8 *)CharStats + slot * 0x3C + 4);
        u16 f6 = *(u16 *)((u8 *)CharStats + slot * 0x3C + 6);
        if (f4 < f6 / 2)
            result = 1;
    }
    return result;
}

/* far_syms: g_BtlSlotPlannedTargets (target reaches it %hi/%lo) */

/* func_8004E3C8 */
s32 BtlSlotPlannedTargetCheck(u16 target)
{
    u8 i;
    u8 result;
    u8 count;

    result = 0;
    count = g_BtlActingSlot;
    i = 0;
    while ((i & 0xFF) < count)
    {
        if ((target & 0xFF) != g_BtlSlotPlannedTargets[i & 0xFF])
        {
            i++;
        }
        else if ((u32)g_BtlSlotPlannedKinds[i & 0xFF] - 3 < 2)
        {
            i++;
            result = 1;
            break;
        }
        else
        {
            i++;
        }
    }

    return result;
}

/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */

/* func_8004E44C */
s32 BtlPartyHpLeastFind(void)
{
    s32 found;
    u8 i;
    u16 *p;
    u16 *q;

    g_BtlPartyFoundNeed = 0xFF;
    found = 0;
    if (g_BtlNumber >= 0xDF)
    {
        for (i = 0; i < 5; i++)
        {
            if (g_BtlSlotIds[i] != 0)
            {
                p = (u16 *)((i * 60) + CharStats);
                if (p[0] < p[1] / 3 * 2)
                {
                    if ((&g_BtlEnemyCount)[1] != 0)
                    {
                        if (!(u8)BtlSlotPlannedTargetCheck(i))
                        {
                            q = (u16 *)((i * 60) + CharStats);
                            if (q[0] < (u16)g_BtlPartyFoundNeed)
                            {
                                g_BtlPartyFoundSlot = i;
                                g_BtlPartyFoundNeed = q[0];
                                found = 1;
                            }
                        }
                    }
                    else if (p[0] < (u16)g_BtlPartyFoundNeed)
                    {
                        g_BtlPartyFoundSlot = i;
                        g_BtlPartyFoundNeed = p[0];
                        found = 1;
                    }
                }
            }
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            if (g_BtlSlotIds[i] != 0)
            {
                p = (u16 *)((i * 60) + CharStats);
                if (p[0] < p[1] / 3 * 2)
                {
                    if ((&g_BtlEnemyCount)[1] != 0)
                    {
                        if (!(u8)BtlSlotPlannedTargetCheck(i))
                        {
                            q = (u16 *)((i * 60) + CharStats);
                            if (q[0] < (u16)g_BtlPartyFoundNeed)
                            {
                                g_BtlPartyFoundSlot = i;
                                g_BtlPartyFoundNeed = q[0];
                                found = 1;
                            }
                        }
                    }
                    else if (p[0] < (u16)g_BtlPartyFoundNeed)
                    {
                        g_BtlPartyFoundSlot = i;
                        g_BtlPartyFoundNeed = p[0];
                        found = 1;
                    }
                }
            }
        }
    }
    g_BtlPartyFoundNeed = 9;
    return found;
}

/* far_syms: CharStats g_BtlNumber */

/* func_8004E6A0 */
s32 BtlPartyHpNot0LeastFind(void)
{
    u8 i;
    s32 found;
    u16 *rec;
    u16 *p;

    found = 0;
    g_BtlPartyFoundNeed = 0xFF;
    if (g_BtlNumber >= 0xDF)
    {
        for (i = 0; i < 5; i++)
        {
            if (g_BtlSlotIds[i] != 0)
            {
                rec = (u16 *)((i * 60) + CharStats);
                if (rec[0] < rec[1] / 3 * 2)
                {
                    if ((&g_BtlEnemyCount)[1])
                    {
                        if ((u8) BtlSlotPlannedTargetCheck(i))
                        {
                            continue;
                        }
                        p = (u16 *)((i * 60) + CharStats);
                        if ((u16) g_BtlPartyFoundNeed > p[0] && p[0] != 0)
                        {
                            g_BtlPartyFoundSlot = i;
                            found = 1;
                            g_BtlPartyFoundNeed = p[0];
                        }
                    }
                    else if (rec[0] < (u16) g_BtlPartyFoundNeed && rec[0] != 0)
                    {
                        g_BtlPartyFoundSlot = i;
                        found = 1;
                        g_BtlPartyFoundNeed = rec[0];
                    }
                }
            }
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            if (g_BtlSlotIds[i] != 0)
            {
                rec = (u16 *)((i * 60) + CharStats);
                if (rec[0] < rec[1] / 3 * 2)
                {
                    if ((&g_BtlEnemyCount)[1])
                    {
                        if ((u8) BtlSlotPlannedTargetCheck(i))
                        {
                            continue;
                        }
                        p = (u16 *)((i * 60) + CharStats);
                        if ((u16) g_BtlPartyFoundNeed > p[0] && p[0] != 0)
                        {
                            g_BtlPartyFoundSlot = i;
                            found = 1;
                            g_BtlPartyFoundNeed = p[0];
                        }
                    }
                    else if (rec[0] < (u16) g_BtlPartyFoundNeed && rec[0] != 0)
                    {
                        g_BtlPartyFoundSlot = i;
                        found = 1;
                        g_BtlPartyFoundNeed = rec[0];
                    }
                }
            }
        }
    }
    g_BtlPartyFoundNeed = 9;
    return found;
}

/* func_8004E91C */
s32 BtlPartyStatusFind(void)
{
  u8 i;
  int stats;
  s32 result;
  result = 0;
  for (i = 0; i < 5; i++)
  {
    if (g_BtlSlotIds[i] == 0)
    {
      continue;
    }
    if ((*((u8 *) ((stats = (i * 60) + CharStats) + 0x15))) == 0)
    {
      continue;
    }
    if ((&g_BtlEnemyCount)[1])
    {
      if ((u8) BtlSlotPlannedTargetCheck(i))
      {
        continue;
      }
    }
    g_BtlPartyFoundSlot = i;
    g_BtlPartyFoundNeed = (u8) BtlSlotStatusNeedGet(i);
    result = 1;
    break;
  }

  return result;
}

/* func_8004E9F4 */
s32 BtlSlotStatusNeedGet(s32 slot, s32 need)
{
  u8 *p;
  slot &= 0xFF;
  p = ((u8 *) ((4 * (slot * 15)) + CharStats)) + 0x15;
  switch (*p)
  {
    case 0:
      need = 0xFF;
      break;

    case 1:
      need = 0;
      break;

    case 4:
      need = 2;
      break;

    case 8:
      need = 3;
      break;

    case 0x10:

    case 0x20:

    case 0x30:
      need = 4;
      break;

    case 0x40:
      need = 6;
      break;

    case 0x80:
      need = 7;
      break;

  }

  return need & 0xFF;
}

/* func_8004EAE0 */
s32 BtlPartyNeedSpellSet(u8 kind, u8 slot)
{
  u8 r;
  u8 c;
  s32 ret;

  r = 0;
  if (kind == 9 && *(u16 *) (slot * 0x3C + CharStats) == 0)
    kind = 10;
  if ((u8) BtlActingSpellListMake())
  {
    ret = 1;
    goto end;
  }
  switch (kind)
  {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      switch (g_BtlSlotIds[g_BtlActingSlot])
      {
        case 3:
          r = BtlActingSpellUsableGet(6);
          break;
        case 6:
          r = BtlActingSpellUsableGet(2);
          break;
      }
      break;
    case 1:
      break;
    case 8:
      r = 0;
      break;
    case 9:
      c = g_BtlSlotIds[g_BtlActingSlot];
      if (c == 3 || c == 6)
        r = BtlActingSpellUsableGet(1);
      break;
    case 10:
      switch (g_BtlSlotIds[g_BtlActingSlot])
      {
        case 3:
          r = BtlActingSpellUsableGet(5);
          break;
        case 6:
          r = 0;
          break;
      }
      if (r == 0)
      {
        if ((u8) BtlPartyHpNot0LeastFind())
        {
          c = g_BtlSlotIds[g_BtlActingSlot];
          if (c == 3 || c == 6)
            r = BtlActingSpellUsableGet(1);
          if (r == 0)
          {
            ret = 1;
            goto end;
          }
          slot = g_BtlPartyFoundSlot;
        }
      }
      break;
  }
  if (r != 0)
  {
    g_BtlPartyChoiceKind = 3;
    g_BtlPartyChoiceNumber = r;
    g_BtlPartyChoiceTarget = slot;
    ret = 0;
  }
  else
  {
    ret = 1;
  }
end:
  return ret;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80012614_pad);
u8 BtlActingSpellUsableGet(u8 spell_index);  /* TU defines this below */

/* Party AI, magic stager (staged kind 3). Picks up to three candidate spell
   levels a/b/c for the acting member's AI mode g_BtlSlotIds[actor]: mode 4
   asks for 7/6/5 unless the target's CharStats +0x2A mask has bit 2, modes
   1/2/5/11 (and that case) 3/2/1, mode 8 one level from rng_ff_draw()/63,
   mode 10 level 1. Mode 0xFE in g_BtlSlotPlannedTargets[actor] hands the choice to
   BtlPartySpellOfThreeGet; otherwise each candidate goes through BtlActingSpellUsableGet (0 =
   not castable) and the LAST castable one is staged. Returns 1 when nothing
   was staged. */
/* func_8004ECF4 */
s32 BtlPartyOtherSideSpellSet(void)
{
    u8 a, b, c, d;
    s32 ret;

    if (!(BtlActingSpellListMake() & 0xFF)) {
        switch (g_BtlSlotIds[g_BtlActingSlot]) {
        case 4:
            if (!(*(u16 *)(CharStats + (u8)BtlEnemyStat08HighestFind() * 0x3C + 0x2A) & 4)) {
                a = 7;
                b = 6;
                c = 5;
                break;
            }
        case 1:
        case 2:
        case 5:
        case 11:
            a = 3;
            b = 2;
            c = 1;
            break;
        case 8:
            switch (rng_ff_draw() / 63) {
            case 1:
                a = b = c = 2;
                break;
            case 2:
                a = b = c = 3;
                break;
            case 3:
                a = b = c = 4;
                break;
            case 0:
            default:
                a = b = c = 1;
                break;
            }
            break;
        case 10:
            a = b = c = 1;
            break;
        }
        if (g_BtlSlotPlannedTargets[g_BtlActingSlot] == 0xFE) {
            g_BtlEnemyMostCount = 0;
            g_BtlEnemyMostSlot = 0;
            d = BtlPartySpellOfThreeGet(a, b, c);
            g_BtlPartyChoiceKind = 3;
            g_BtlPartyChoiceNumber = d;
            if (g_BtlEnemyMostSlot != 0) {
                g_BtlPartyChoiceTarget = g_BtlEnemyMostSlot;
            } else {
                g_BtlPartyChoiceTarget = BtlEnemyStat08HighestFind();
            }
            ret = 0;
        } else {
            a = BtlActingSpellUsableGet(a);
            b = BtlActingSpellUsableGet(b);
            c = BtlActingSpellUsableGet(c);
            if (a != 0 || b != 0 || c != 0) {
                if (a != 0) {
                    d = a;
                }
                if (b != 0) {
                    d = b;
                }
                if (c != 0) {
                    d = c;
                }
                g_BtlPartyChoiceKind = 3;
                g_BtlPartyChoiceNumber = d;
                g_BtlPartyChoiceTarget = 0xFE;
                ret = 0;
            } else {
                ret = 1;
            }
        }
    } else {
        ret = 1;
    }
    return ret;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80012644_pad);
/* la_absolute: BtlActingSpellUsableGet */
s32 BtlActingSpellNumberGet(s32 spell_index_then_spell);  /* TU defines this below */

/* func_8004EF4C */
u8 BtlActingSpellUsableGet(u8 spell_index)
{
    s32 r;
    u8 ok;
    u8 i;
    u8 c;

    ok = 1;
    r = BtlActingSpellNumberGet(spell_index);
    i = 0;
    c = r;
    do
    {
        if (*(u8 *) (g_BtlActingSlot * 0x3C + CharStats + 0x15) == 0x20)
        {
            if (c == (&g_BtlActingSpellList)[i])
            {
                if (!(BtlSpellByte0BGet(c) & 0xFF))
                {
found:
                    ok = 0;
                    break;
                }
            }
        }
        else if (c == (&g_BtlActingSpellList)[i])
        {
            goto found;
        }
        i++;
    } while (i < 8);
    if (ok)
    {
        r = 0;
    }
    return (u8) r;
}

/* func_8004F048 */
s32 BtlEnemyCountBelow4Check(void) {
    return (u8) g_BtlEnemyCount < 4U;
}

/* far_syms: g_BtlTargetSlot */
/* la_absolute: BtlActingSpellListMake */
extern u8 g_BtlTargetSlot;
s32 BtlTargetTraitCheck(s32, s32, s32, s32);

/* func_8004F054 */
u8 BtlActingSpellListMake(void)
{
    u8 ret;
    u8 i;
    u8 n;
    u8 j;
    u8 spell;
    u8 *row;

    /* the chained `i = ret = 0;` ahead of the loop (its store to i is dead)
       is load-bearing: it is ret's extra reference, ranking ret ahead of the
       hoisted g_BtlActingSpellList base in global allocation ($s4 vs $s5; -10 rows
       without it) */
    i = ret = 0;
    for (i = 0; i < 8; i++)
    {
        (&g_BtlActingSpellList)[i] = 0;
    }
    n = 0;
    for (j = 0; j < 8; j++)
    {
        spell = ((u8 *) (g_BtlActingSlot * 0x3C + CharStats + j))[0x34];
        if (spell != 0)
        {
            g_BtlTargetSlot = g_BtlActingSlot;
            row = g_BtlSpellRowsByNumber + spell * 12;
            if ((BtlTargetTraitCheck(0, 0x1B, 0, 0) & 0xFF) == 1)
            {
                if ((row[4] + 1) >> 1 <= *(u16 *) (g_BtlActingSlot * 0x3C + CharStats + 4))
                {
                    (&g_BtlActingSpellList)[n] = ((u8 *) (g_BtlActingSlot * 0x3C + CharStats + j))[0x34];
                    n++;
                }
            }
            else if (row[4] < *(u16 *) (g_BtlActingSlot * 0x3C + CharStats + 4))
            {
                (&g_BtlActingSpellList)[n] = ((u8 *) (g_BtlActingSlot * 0x3C + CharStats + j))[0x34];
                n++;
            }
        }
    }
    if (n == 0)
    {
        ret = 1;
    }
    return ret;
}

/* func_8004F218 */
u8 BtlEnemyStat08HighestFind(u8 unused)
{
    u8 r[2];
    u8 i;
    u8 *p;

    r[0] = r[1] = 0;
    for (i = 6; i < 14; i++)
    {
        if (g_BtlSlotIds[i] != 0)
        {
            p = (u8 *)(i * 0x3C + CharStats);
            if (*(u16 *)p != 0 && r[1] < ((BtlStats *)p)->stat_08)
            {
                r[0] = i;
                r[1] = ((BtlStats *)p)->stat_08;
            }
        }
    }
    return r[0];
}

s32 BtlActingItemMissingCheck(s32 item);  /* TU defines this below */

/* func_8004F2B4 */
s32 BtlPartyNeedItemSet(s32 need, s32 target)
{
    s32 result;
    u8 item;
    u8 m;

    m = g_BtlPartyFoundSlot;
    result = 0;
    item = 0;
    if ((need & 0xFF) == 9)
    {
        if (*(u16 *)(m * 0x3C + CharStats) == 0)
        {
            need = 0xA;
        }
    }
    switch (need & 0xFF)
    {
    case 0:
        item = 0xA2;
        if (BtlActingItemMissingCheck(0xA2) & 0xFF)
        {
            item = 0xA3;
            result = BtlActingItemMissingCheck(0xA3);
        }
        break;
    case 1:
        break;
    case 2:
        item = 0xA1;
        if (BtlActingItemMissingCheck(0xA1) & 0xFF)
        {
            item = 0xA3;
            result = BtlActingItemMissingCheck(0xA3);
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        item = 0xA3;
        result = BtlActingItemMissingCheck(0xA3);
        break;
    case 8:
        result = 1;
        break;
    case 9:
        item = 0x9C;
        if (BtlActingItemMissingCheck(0x9C) & 0xFF)
        {
            item = 0x9D;
            result = BtlActingItemMissingCheck(0x9D);
        }
        break;
    case 10:
        item = 0xA0;
        result = BtlActingItemMissingCheck(0xA0);
        break;
    }
    if ((result & 0xFF) == 0)
    {
        g_BtlPartyChoiceKind = 4;
        g_BtlPartyChoiceNumber = item;
        g_BtlPartyChoiceTarget = target;
    }
    return result & 0xFF;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80012674_pad);
/* func_8004F3F8 */
s32 BtlActingItemMissingCheck(s32 item)
{
  s32 result;
  unsigned long base;
  u8 target;
  u8 i;
  result = 1;
  if (StoryBitCheck(0x3D9) == 0)
  {
    base = (g_BtlActingSlot * 60) + CharStats;
    target = (u8) item;
    for (i = 0; i < 6; i++)
    {
      if (target == (*((((u8 *) base) + i) + 0x2E)))
      {
        result = 0;
        break;
      }
    }

  }
  return result;
}

s32 BtlActingStat20Not3Or4Check(void);  /* TU defines this below */
s32 BtlPartyWornItemSet(void);  /* TU defines this below */

/* func_8004F494 */
void BtlPartyAttackChoiceSet(void)
{
    s32 t;
    u8 id;

    t = BtlSlotNearestFind(1);
    if (*(u8 *)(g_BtlActingSlot * 0x3C + CharStats + 0x15) == 0x20)
    {
        goto defend;
    }
    id = g_BtlSlotIds[g_BtlActingSlot];
    if ((id == 2) || (id == 4) || (id == 6))
    {
        if ((BtlPartyWornItemSet() & 0xFF) == 0)
        {
            return;
        }
        if (BtlActingStat20Not3Or4Check() & 0xFF)
        {
            goto defend;
        }
    }
    id = g_BtlSlotIds[g_BtlActingSlot];
    if ((id == 1) || (id == 0xB) || (id == 4) || (id == 5) || (id == 3))
    {
        id = g_BtlSlotIds[t & 0xFF];
        if ((id == 0x2F) || (id == 0x38) || (id == 0x3E) || (id == 0x3F))
        {
        defend:
            g_BtlPartyChoiceKind = 5;
            g_BtlPartyChoiceNumber = 0xFF;
            g_BtlPartyChoiceTarget = 0xFF;
            return;
        }
    }
    g_BtlPartyChoiceKind = 2;
    g_BtlPartyChoiceNumber = 0xFF;
    g_BtlPartyChoiceTarget = t;
}

/* func_8004F5E4 */
u8 BtlPartyStat08Sum(void)
{
    u8 i;
    u8 n;

    n = 0;
    for (i = 0; i < 5; i++)
    {
        if ((g_BtlSlotIds[i] != 0) && (*(u16 *) (i * 0x3C + CharStats) != 0))
        {
            g_BtlPartyStat08Sum += *(u16 *) (i * 0x3C + CharStats + 8);
            n++;
        }
    }
    g_BtlPartyCount = n;
    return n;
}

/* func_8004F664 */
u8 BtlEnemyStat08Sum(void)
{
    u8 i;
    u8 n;

    n = 0;
    for (i = 6; i < 0xE; i++)
    {
        if ((g_BtlSlotIds[i] != 0) && (*(u16 *) (i * 0x3C + CharStats) != 0))
        {
            g_BtlEnemyStat08Sum += *(u16 *) (i * 0x3C + CharStats + 8);
            n++;
        }
    }
    g_BtlEnemyCount = n;
    return n;
}

/* far_syms: g_BtlPartyKeptTargets (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyKeptKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyKeptNumbers (target reaches it %hi/%lo) */

/* func_8004F6E4 */
void BtlPartyChoiceClear(void) {
    s32 slot;
    s32 i;
    u8 *p9C;
    u8 *pA4;
    u8 *p94;

    i = 0;
    p9C = g_BtlPartyKeptKinds;
    pA4 = g_BtlPartyKeptNumbers;
    p94 = g_BtlPartyKeptTargets;
    do {
        slot = i & 0xFF;
        i += 1;
        *(u8 *)(slot + (s32) p9C) = 0;
        *(u8 *)(slot + (s32) pA4) = 0;
        *(u8 *)(slot + (s32) p94) = 0;
    } while ((u32) (i & 0xFF) < 5U);
}

/* far_syms: CharStats g_BtlSlotIds g_BtlActingSlot g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber */
/* la_absolute: BtlPartyTurnActionSet */
extern u8 g_BtlTargetSlot;
u8 BtlPartyNeedItemChoiceSet(void);  /* TU defines this below */
void BtlPartyPlannedSpellChoiceSet(void);  /* TU defines this below */

/* func_8004F734 */
void BtlPartyTurnActionSet(void)
{
  u8 *rec;
  s32 n;

  (&g_BtlEnemyCount)[1] = 0;
  g_BtlPartyStat08Sum = 0;
  g_BtlEnemyStat08Sum = 0;
  D_800B8C65 = 0;
  g_BtlPartyChoiceTarget = 0;
  g_BtlPartyChoiceNumber = 0;
  g_BtlPartyChoiceKind = 0;
  BtlEnemyStat08Sum();
  BtlPartyStat08Sum();
  if (g_BtlSlotIds[g_BtlActingSlot] != 0)
  {
    rec = (u8 *) (g_BtlActingSlot * 0x3C + CharStats);
    if ((*(u16 *) rec != 0) && (g_BtlPartyNotHeldFlags[g_BtlActingSlot] != 0) && (rec[0x15] != 4) && (rec[0x15] != 8) && (rec[0x15] != 0x30))
    {
      switch (g_BtlSlotPlannedKinds[g_BtlActingSlot])
      {
      case 2:
        /* a plain (non-volatile) read of g_BtlPartyCount, as BtlPartyHpShortTwoCheck's
           loop: the TU declares it volatile for another function, and the
           volatile load would be scheduled ahead of g_BtlEnemyCount's */
        if ((g_BtlPartyStat08Sum / 3 * 2 < (u16) g_BtlEnemyStat08Sum) && (g_BtlEnemyCount > *(u8 *) &g_BtlPartyCount))
        {
      case 4:
          if (!BtlPartyNeedItemChoiceSet())
            break;
        }
        goto other;
      case 3:
        BtlPartyPlannedSpellChoiceSet();
        break;
      case 5:
        if (((u16) g_BtlEnemyStat08Sum < g_BtlPartyStat08Sum / 3 * 2) && ((n = g_BtlEnemyCount, (u8) g_BtlPartyCount - (u8) BtlPartyPlannedCount()) < n))
        {
          if (BtlPartyNeedItemChoiceSet())
          {
            g_BtlPartyChoiceKind = 5;
            g_BtlPartyChoiceNumber = 0xFF;
            g_BtlPartyChoiceTarget = 0xFF;
          }
          break;
        }
      other:
        if ((u8) BtlPartyWornItemSet())
          BtlPartyAttackChoiceSet();
        break;
      case 6:
      case 10:
      case 11:
        break;
      default:
        while (1)
        {
        }
      }
      g_BtlSlotPlannedKinds[g_BtlActingSlot] = g_BtlPartyChoiceKind;
      g_BtlSlotPlannedNumbers[g_BtlActingSlot] = g_BtlPartyChoiceNumber;
      g_BtlSlotPlannedTargets[g_BtlActingSlot] = g_BtlPartyChoiceTarget;
      g_BtlActionNumber = g_BtlPartyChoiceNumber;
      g_BtlActionKind = g_BtlPartyChoiceKind;
      g_BtlTargetSlot = g_BtlPartyChoiceTarget;
      g_BtlPartyKeptKinds[g_BtlActingSlot] = g_BtlPartyChoiceKind;
      g_BtlPartyKeptNumbers[g_BtlActingSlot] = g_BtlPartyChoiceNumber;
      g_BtlPartyKeptTargets[g_BtlActingSlot] = g_BtlPartyChoiceTarget;
    }
    else
    {
      g_BtlSlotPlannedKinds[g_BtlActingSlot] = 5;
      g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0xFF;
      g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0xFF;
      g_BtlPartyKeptKinds[g_BtlActingSlot] = 5;
      g_BtlPartyKeptNumbers[g_BtlActingSlot] = 0xFF;
      g_BtlPartyKeptTargets[g_BtlActingSlot] = 0xFF;
    }
  }
}

/* func_8004FABC */
u8 BtlPartyNeedItemChoiceSet(void)
{
  u8 ok;
  u8 n;

  ok = 1;
  n = 0;
  if ((u8)BtlSlotHpLowCheck(g_BtlActingSlot))
    ok = BtlPartyNeedItemSet(9, g_BtlActingSlot);
  if (ok)
  {
    if ((u8)BtlPartyHpLeastFind())
      ok = BtlPartyNeedItemSet(9, (u8)g_BtlPartyFoundSlot);
    if (ok)
    {
      if ((u8)BtlPartyStatusFind())
        ok = BtlPartyNeedItemSet((u8)g_BtlPartyFoundNeed, (u8)g_BtlPartyFoundSlot);
      if (ok)
      {
        for (; n < 5; n++)
        {
          if (BtlSlotMpLowCheck(n, 0) && (g_BtlSlotIds[g_BtlActingSlot] != 7) && (g_BtlSlotIds[g_BtlActingSlot] != 8) && !(u8)BtlSlotPlannedTargetCheck(n))
          {
            ok = (u8)BtlPartyNeedItemSet(8, n) != 0;
            break;
          }
        }
      }
    }
  }
  return ok;
}

/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */

/* func_8004FC24 */
void BtlPartyPlannedSpellChoiceSet(void)
{
    u8 id;
    u8 answer;

    id = g_BtlSlotIds[g_BtlActingSlot];
    answer = 1;
    switch (id)
    {
        case 3:
            if ((BtlPartyHpShortSpellSet() & 0xFF) || g_BtlSlotPlannedNumbers[g_BtlActingSlot] == 0x11)
                goto LABEL_13;
            return;

        case 6:
            if ((BtlPartyHpShortSpellSet() & 0xFF) == 0)
                return;

        LABEL_13:
            if (BtlPartyHpLeastFind() & 0xFF)
                answer = BtlPartyNeedSpellSet(9, (u8)g_BtlPartyFoundSlot) & 0xFF;

            if (answer & 0xFF)
            {
                if (BtlPartyStatusFind() & 0xFF)
                    answer = BtlPartyNeedSpellSet((u8)g_BtlPartyFoundNeed, (u8)g_BtlPartyFoundSlot) & 0xFF;
            }

            if (answer & 0xFF)
                BtlPartyNeedSpellSet(9, g_BtlActingSlot);
            return;

        case 7:
            while (1)
                ;

        case 9:
            while (1)
                ;

        case 1:
        case 2:
        case 4:
        case 5:
        case 8:
        case 10:
        case 11:
            BtlPartyOtherSideSpellSet();
            return;
    }
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800126CC_pad);
/* func_8004FD70 */
s32 BtlPartyHpShortSpellSet(void)
{
  s32 kind;
  s32 result;
  u8 answer_then_spell;
  u8 id;
  result = 1;
  BtlActingSpellListMake();
  answer_then_spell = BtlPartyHpShortTwoCheck();
  if (((u8) answer_then_spell) == 0)
    goto block_1;
  id = g_BtlSlotIds[g_BtlActingSlot];
  kind = 3;
  switch (id)
  {
    case 3:
      answer_then_spell = BtlActingSpellUsableGet(2);
      break;

    case 6:
      answer_then_spell = BtlActingSpellUsableGet(6);
      break;

    default:
      return result;

  }

  if (((u8) answer_then_spell) == 0)
    goto block_1;
  result = 0;
  /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
     block runs once, so behaviour is the same without the `do` / `while (0)`, but then 9
     of this function's 41 instruction lines differ. */
  do
  {
    g_BtlPartyChoiceKind = kind;
  }
  while (0);
  g_BtlPartyChoiceNumber = answer_then_spell;
  g_BtlPartyChoiceTarget = 0xFF;
  block_1:
  return result;

}

/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
s32 BtlActingStat20Not3Or4Check(void);  /* TU defines this below */

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_8004FE18 */
s32 BtlSlotPlannedCheck(s32 wanted_slot)
{
  u8 *slot_ids;
  u8 id;
  u8 result;
  s32 slot;
  slot_ids = g_BtlSlotIds;
  slot = wanted_slot & 0xFF;
  result = 1;
  id = *(slot + slot_ids);
  if (id != 0)
  {
    result = (*(slot + g_BtlSlotPlannedKinds)) > 0;
  }
  return result;
}

/* func_8004FE5C */
s32 BtlPartyPlannedCount(void)
{
  s32 i;
  s32 planned_count;
  u8 slot;
  planned_count = 0;
  i = 0;
  slot = 0 & 0xFF;
  do
  {
    slot = i & 0xFF;
    if (((*(slot + g_BtlSlotIds)) != 0) && ((*(slot + g_BtlSlotPlannedKinds)) != 0))
    {
      planned_count += 1;
    }
    i += 1;
  }
  while (((u32) (i & 0xFF)) < 5U);
  return planned_count & 0xFF;
}

/* far_syms: CharStats g_BtlActingSlot g_BtlRecords */

/* func_8004FEBC */
u8 BtlActingAreaEnemyCount(u8 spell)
{
    u8 n;
    u8 i;
    s32 base;
    u8 *c;
    u8 *e;
    u8 *row;
    s32 rx;
    s32 ry;

    n = 0;
    i = 6;
    base = g_BtlRecords;
    c = (u8 *)((g_BtlActingSlot << 6) + base);
    row = &g_BtlSpellRowsByNumber[spell * 12];
    rx = row[9] * 8;
    ry = row[10] * 8;
    for (; i < 14; i++)
    {
        if (*(u16 *)(i * 0x3C + CharStats) != 0)
        {
            e = (u8 *)((i << 6) + base);
            if (((BtlRecord *)e)->sprite.x <= ((BtlRecord *)c)->sprite.x + rx && ((BtlRecord *)e)->sprite.x >= ((BtlRecord *)c)->sprite.x - rx
                && ((BtlRecord *)e)->sprite.y <= ((BtlRecord *)c)->sprite.y + ry && ((BtlRecord *)e)->sprite.y >= ((BtlRecord *)c)->sprite.y - ry)
            {
                n++;
            }
        }
    }
    return n;
}

/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
extern u8 g_BtlSlotAmountKinds[];
s32 BtlTargetRowsMark(s32);
extern u8 g_BtlTargetSlot;

static inline int inline_fn(u8 slot)
{
  return slot & 0xFF;
}
/* func_8004FFA0 */
u8 BtlEnemyRowsMostFind(void)
{
  u8 slot;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `unused_argument` only holds the constant 0; behaviour is the same with the constant written
     where it is read, but then 1 of this function's 77 instruction lines differs. */
  int unused_argument;
  u8 enemy_slot;
  u16 zero;
  u16 i;
  s32 j;
  s32 k;
  u8 next_target;
  s32 marked_count;
  unsigned short lit_6;
  g_BtlEnemyMostCount = 0;
  g_BtlEnemyMostSlot = 0;
  g_BtlTargetSlot = 6;
  do
  {
    marked_count = 0;
    if ((*((u16 *) ((g_BtlTargetSlot * 0x3C) + CharStats))) != 0)
    {
      unused_argument = 0;
      i = 0;
      slot = inline_fn(0);
      do
      {
        slot = i & 0xFF;
        *(slot + g_BtlSlotAmountKinds) = 0;
        i += 1;
      }
      while (((u32) inline_fn(i)) < 0xEU);
      BtlTargetRowsMark(unused_argument);
      marked_count = 0;
      j = 6;
      lit_6 = 6;
      do
      {
        enemy_slot = inline_fn(lit_6);
        enemy_slot = j & 0xFF;
        if ((*(enemy_slot + g_BtlSlotAmountKinds)) != 0)
        {
          marked_count += 1;
        }
        j += 1;
      }
      while (((u32) (j & 0xFF)) < 0xEU);

    }
    if (((u8) g_BtlEnemyMostCount) < ((u32) (marked_count & 0xFF)))
    {
      g_BtlEnemyMostCount = marked_count;
      g_BtlEnemyMostSlot = g_BtlTargetSlot;
    }
    next_target = g_BtlTargetSlot + 1;
    g_BtlTargetSlot = next_target;
    k = 0;
  }
  while (((u32) inline_fn(next_target)) < 0xEU);
  if (marked_count)
  {
    zero = inline_fn(0);
  }
  else
  {
    zero = inline_fn(0);
  }
  do
  {
    g_BtlSlotAmountKinds[k & 0xFF] = 0;
    k += 1;
  }
  while (((u32) (k & 0xFF)) < 0xEU);
  g_BtlTargetSlot = 0;
  return g_BtlEnemyMostCount;
}

/* far_syms: g_BtlSpellRowsByNumber (target reaches it %hi/%lo) */

/* func_800500F0 */
u8 BtlEnemyAreaMostFind(s32 spell)
{
  s16 record_x;
  s16 record_y;
  s32 other_x;
  s32 other_y;
  s32 half_height;
  s32 half_width;
  u8 other_slot;
  u16 slot_byte;
  s16 other_slot_byte;
  u8 inside_count;
  u8 slot;
  void *other_record;
  void *record;
  void *spell_row;
  slot = 6;
  spell_row = ((spell & 0xFF) * 0xC) + g_BtlSpellRowsByNumber;
  g_BtlEnemyMostSlot = (g_BtlEnemyMostCount = 0);
  half_width = ((BtlSpellRow *)spell_row)->area_half_width * 8;
  half_height = ((BtlSpellRow *)spell_row)->area_half_height * 8;
  slot_byte = 6 & 0xFF;
  do
  {
    inside_count = 0;
    slot_byte = slot & 0xFF;
    if ((*((u16 *) ((slot_byte * 0x3C) + CharStats))) != 0)
    {
      other_slot = 6;
      record = (slot_byte << 6) + g_BtlRecords;
      other_slot_byte = 6 & 0xFF;
      do
      {
        other_slot_byte = other_slot & 0xFF;
        if ((*((u16 *) ((other_slot_byte * 0x3C) + CharStats))) != 0)
        {
          other_record = (other_slot_byte << 6) + g_BtlRecords;
          record_x = ((BtlRecord *)record)->sprite.x;
          other_x = ((BtlRecord *)other_record)->sprite.x;
          if (((record_x + half_width) >= other_x) && (other_x >= (record_x - half_width)))
          {
            if (1)
            {
              record_y = ((BtlRecord *)record)->sprite.y;
            }
            other_y = ((BtlRecord *)other_record)->sprite.y;
            if (((record_y + half_height) >= other_y) && (other_y >= (record_y - half_height)))
            {
              inside_count += 1;
            }
          }
        }
        other_slot += 1;
      }
      while (((u32) (other_slot & 0xFF)) < 0xEU);
      if (((u8) g_BtlEnemyMostCount) < ((u32) (inside_count & 0xFF)))
      {
        g_BtlEnemyMostSlot = slot;
        g_BtlEnemyMostCount = inside_count;
      }
    }
    slot += 1;
    slot_byte = slot & 0xFF;
  }
  while (((u32) (slot & 0xFF)) < 0xEU);
  return g_BtlEnemyMostSlot;
}

/* Party-AI staging pick: of three candidates (each through the
 * BtlActingSpellUsableGet conflict check), choose by the acting character's id
 * (g_BtlSlotIds[g_BtlActingSlot]); taking the third clears g_BtlEnemyMostSlot. */
/* func_8005022C */
u8 BtlPartySpellOfThreeGet(u8 first_spell, u8 second_spell, u8 third_spell)
{
    u8 r;
    u8 c;

    r = 0;
    first_spell = BtlActingSpellUsableGet(first_spell);
    second_spell = BtlActingSpellUsableGet(second_spell);
    /* the target holds the third result twice ($s4, copied to $s3):
     * c is read only in the id-5 / no-second-candidate arm */
    c = BtlActingSpellUsableGet(third_spell);
    third_spell = c;
    if (first_spell)
    {
        if (g_BtlSlotIds[g_BtlActingSlot] == 5)
        {
            if (second_spell)
            {
                if ((BtlActingAreaEnemyCount(second_spell) < BtlEnemyRowsMostFind()) && (BtlEnemyRowsMostFind() >= 2))
                {
                    r = first_spell;
                }
                else if (!third_spell)
                {
                    r = second_spell;
                }
                else if (BtlActingAreaEnemyCount(second_spell) >= 2)
                {
                    r = second_spell;
                }
                else
                {
                    r = third_spell;
                    g_BtlEnemyMostSlot = 0;
                }
            }
            else
            {
                r = first_spell;
                if (c && (BtlEnemyRowsMostFind() < 2))
                {
                    g_BtlEnemyMostSlot = 0;
                    r = c;
                }
            }
        }
        else if (!(u8)BtlEnemyCountBelow4Check())
        {
            r = first_spell;
        }
        else
        {
            switch (g_BtlSlotIds[g_BtlActingSlot])
            {
            case 1:
            case 2:
            case 4:
            case 10:
            case 11:
                if (second_spell)
                {
                    BtlEnemyAreaMostFind(second_spell);
                    if (g_BtlEnemyMostCount < 2)
                    {
                        if (third_spell)
                        {
                            r = third_spell;
                            g_BtlEnemyMostSlot = 0;
                        }
                        else if (g_BtlEnemyMostCount >= g_BtlEnemyCount)
                        {
                            r = second_spell;
                        }
                        else
                        {
                            r = first_spell;
                        }
                    }
                    else
                    {
                        r = second_spell;
                    }
                }
                else if (third_spell)
                {
                    r = third_spell;
                    g_BtlEnemyMostSlot = 0;
                }
                else
                {
                    r = first_spell;
                }
                break;
            case 8:
                r = first_spell;
                break;
            }
        }
    }
    else if (second_spell)
    {
        switch (g_BtlSlotIds[g_BtlActingSlot])
        {
        case 5:
            if (BtlActingAreaEnemyCount(second_spell) < 2)
            {
                if (third_spell)
                {
                    r = third_spell;
                    g_BtlEnemyMostSlot = 0;
                }
                else
                {
                    r = second_spell;
                }
            }
            else
            {
                r = second_spell;
            }
            break;
        case 1:
        case 2:
        case 4:
        case 10:
        case 11:
            BtlEnemyAreaMostFind(second_spell);
            if (g_BtlEnemyMostCount >= 2)
            {
                r = second_spell;
            }
            else if (third_spell)
            {
                r = third_spell;
                g_BtlEnemyMostSlot = 0;
            }
            else
            {
                r = second_spell;
            }
            break;
        }
    }
    else if (third_spell)
    {
        r = third_spell;
        g_BtlEnemyMostSlot = 0;
    }
    else
    {
        for (;;)
        {
        }
    }
    return r;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_8001272C_pad);
/* func_800504E4 */
s32 BtlActingSpellNumberGet(s32 spell_index_then_spell)
{
	switch (g_BtlSlotIds[g_BtlActingSlot]) {
		case 1:
		case 11:
			break;
		case 2:
			spell_index_then_spell += 8;
			break;
		case 3:
			spell_index_then_spell += 0xC;
			break;
		case 4:
			spell_index_then_spell += 0x14;
			break;
		case 5:
			spell_index_then_spell += 0x1C;
			break;
		case 6:
			spell_index_then_spell += 0x20;
			break;
		case 8:
			spell_index_then_spell += 0x26;
			break;
		case 10:
			spell_index_then_spell += 0x2E;
			break;
		default:
			for (;;) ;
	}

	return spell_index_then_spell & 0xFF;
}

u8 BtlEnemyStat08HighestFind(u8);

/* func_80050574 */
s32 BtlPartyWornItemSet(void)
{
  unsigned int choice_item;
  unsigned long stats_then_worn_item;
  s32 result;
  u8 worn_item;
  void *stats;
  result = 1;
  if (StoryBitCheck(0x3D9) == 0)
  {
    stats_then_worn_item = (12 * (g_BtlActingSlot * 5)) + CharStats;
    stats = stats_then_worn_item;
    result = 1;
    if (((BtlStats *) stats)->level < 0x1AU)
    {
      worn_item = ((BtlStats *) stats)->side.party.worn_items[0];
      choice_item = worn_item;
      if (((u32) (choice_item + (-0x23))) >= 3U)
      {
        choice_item = worn_item;
        stats_then_worn_item = choice_item & 0xFF;
        if (((stats_then_worn_item == 0x27) || (stats_then_worn_item == 0x14)) || ((result = 1, stats_then_worn_item == 0x15)))
        {
          goto block_7;
        }
      }
      else
      {
        block_7:
        g_BtlPartyChoiceKind = 4;

        g_BtlPartyChoiceNumber = choice_item;
        g_BtlPartyChoiceTarget = BtlEnemyStat08HighestFind(choice_item);
        result = 0;
      }
    }
  }
  return result;
}

/* func_80050630 */
s32 BtlActingStat20Not3Or4Check(void)
{
  return !(((unsigned) ((*((u8 *) ((CharStats + (4 * (15 * g_BtlActingSlot))) + 0x20))) - 3)) < 2);
}

/* func_8005066C */
u8 BtlSpellByte0BGet(s32 spell)
{
  u8 *spell_row;
  spell_row = (u8 *) (((spell & 0xFF) * 0xC) + g_BtlSpellRowsByNumber);
  return ((BtlSpellRow *)spell_row)->blocked_by_status_20;
}
