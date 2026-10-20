/* Lunar SSSC — btl_plan: the bosses' routines (docs/spec boss-ai.md):
   BtlEnemyActionSpecialSet and the per-id BtlEnemy..ActionSet that plan,
   and re-check, an enemy's action by its id, ending in
   BtlActingAnimationSet (their rest pose) and BtlActingKindBSet. The first
   split had it in rendering3.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlActionKind */
/* far_syms: g_BtlSlotPlannedKinds */
/* far_syms: g_BtlSlotIds */
/* far_syms: g_BtlSlotActiveFlags */
/* far_syms: g_BtlActingSlot */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
extern u8 g_BtlActionKind;
extern u8 g_BtlSlotPlannedNumbers[];
u8 BtlSlotNearestFind(s32);                                /* extern */
extern u8 g_BtlSlotRestAnimations[];
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
extern u8 g_BtlExtraRecordCount;
extern s32 g_BtlExtraRecords;
s32 BtlFallRun();
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
s32 BtlActingAnimationSet(s32);
extern u8 g_BtlActingSlot;
void BtlEnemy6EEnemy70ActionSet(void);  /* TU defines this below */
extern s32 g_BtlRecords;
extern u8 g_BtlTargetSlot;/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
extern u8 g_BtlSlotAmountKinds[];
s32 BtlAnimationSet(s32, s32);
s32 BtlAnimationEndWait(s32);
extern u8 g_BtlExtraRecordCount;
extern s32 g_BtlExtraRecords;
s32 BtlFallRun();
/* far_syms: g_BtlActingSlot g_BtlRecords */
extern u8 g_BtlSlotActiveFlags[];

extern u8 g_BtlSlotIds[];
extern u8 g_BtlSlotActiveFlags[];

void BtlEnemy7FActionSet(void);  /* TU defines this below */
void BtlEnemy6FActionSet(s32);                                 /* extern */
void BtlEnemy71ActionSet(s32);                                 /* extern */
void BtlEnemy72ActionSet(void);  /* TU defines this below */
void BtlEnemy73ActionSet(s32);                                 /* extern */
void BtlEnemy74ActionSet(s32 planning);  /* TU defines this below */
s32 BtlEnemy75ActionSet(s32);                                 /* extern */
void BtlEnemy76ActionSet(void);  /* TU defines this below */
void BtlEnemy77ActionSet(s32);                                 /* extern */
s32 BtlEnemy78ActionSet(s32);                                 /* extern */
s32 BtlEnemy79ActionSet(s32);                                 /* extern */
void BtlEnemy7AActionSet(s32);                                 /* extern */
void BtlEnemy7BActionSet(s32 planning);  /* TU defines this below */
void BtlEnemy7CActionSet();                                  /* extern */
void BtlEnemy7DActionSet();                                  /* extern */
void BtlEnemy7EActionSet(void);  /* TU defines this below */

/* func_800417E4 */
void BtlEnemyActionSpecialSet(void) {
    u8 next_slot;
    u8 id;

    g_BtlActingSlot = 6;
    do {
        if (*(g_BtlActingSlot + g_BtlSlotActiveFlags) != 0) {
            id = *(g_BtlActingSlot + g_BtlSlotIds);
            switch (id) {
            case 0x6F:
                BtlEnemy6FActionSet(1);
                break;
            case 0x6E:
            case 0x70:
                BtlEnemy6EEnemy70ActionSet();
                break;
            case 0x71:
                BtlEnemy71ActionSet(1);
                break;
            case 0x72:
                BtlEnemy72ActionSet();
                break;
            case 0x73:
                BtlEnemy73ActionSet(1);
                break;
            case 0x74:
                BtlEnemy74ActionSet(1);
                break;
            case 0x75:
                BtlEnemy75ActionSet(1);
                break;
            case 0x76:
                BtlEnemy76ActionSet();
                break;
            case 0x77:
                BtlEnemy77ActionSet(1);
                break;
            case 0x78:
                BtlEnemy78ActionSet(1);
                break;
            case 0x79:
                BtlEnemy79ActionSet(1);
                break;
            case 0x7A:
                BtlEnemy7AActionSet(1);
                break;
            case 0x7B:
                BtlEnemy7BActionSet(1);
                break;
            case 0x7C:
                BtlEnemy7CActionSet();
                break;
            case 0x7D:
                BtlEnemy7DActionSet();
                break;
            case 0x7E:
                BtlEnemy7EActionSet();
                break;
            case 0x7F:
                BtlEnemy7FActionSet();
                break;
            }
        }
        next_slot = g_BtlActingSlot + 1;
        g_BtlActingSlot = next_slot;
    } while ((u32) (next_slot & 0xFF) < 0xEU);
}

/* func_800419B0 */
void BtlEnemy7FActionSet(void)
{
  u8 draw;

  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
  draw = rng_7f_draw() & 0xFF;

  if (draw < 0x2A)
  {
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
    g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlSlotNearestFind(2);
  }
  else if (draw < 0x54)
  {
    *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x3A;
    g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlPartyTargetPick();
  }
  else
  {
    *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x39;
  }

  BtlActingAnimationSet(0);
}
/* far_syms: g_BtlPartyHitAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyStandAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
extern u8 g_BtlActionNumber;
s32 rng_7f_draw();
extern u8 g_BtlSpecialBytes[];

extern u8 g_BtlActionNumber;
extern u8 g_BtlSpecialBytes[];
extern u8 g_BtlPartyStandAnimations[];
extern u8 g_BtlPartyHitAnimations[];
extern u8 g_BtlPartyNotHeldFlags[];
extern s32 g_BtlPartyKeptRecords;

/* func_80041A80 */
void BtlEnemy6FActionSet(s32 planning)
{
  u8 cand[4];
  u8 i;
  u8 n;
  u16 v6;
  u16 vA;
  u16 vC;
  u8 r;
  s32 next;

  planning &= 0xFF;
  if (planning == 0)
  {
    if (g_BtlActionKind == 2)
    {
      g_BtlTargetSlot = BtlSlotNearestFind(2);
    }
    else if (g_BtlActionNumber == 0x50)
    {
      g_BtlTargetSlot = BtlPartyTargetPick(2);
    }
    return;
  }
  i = 0;
  do
  {
    if (g_BtlSpecialBytes[i] != 0)
    {
      if (--g_BtlSpecialBytes[i] == 0)
      {
        switch (g_BtlSlotIds[i])
        {
        case 1:
          BtlAnimationSet(g_BtlRecords + (i << 6), 0x13);
          break;
        case 2:
          BtlAnimationSet(g_BtlRecords + (i << 6), 0x15);
          break;
        case 6:
          BtlAnimationSet(g_BtlRecords + (i << 6), 0x14);
          break;
        case 7:
          BtlAnimationSet(g_BtlRecords + (i << 6), 0x16);
          break;
        }
        v6 = *(u16 *)(((i << 6) + g_BtlRecords) + 6);
        vA = *(u16 *)(((i << 6) + g_BtlRecords) + 0xA);
        vC = *(u16 *)(((i << 6) + g_BtlRecords) + 0xC);
        BtlAnimationEndWait(((i << 6) + g_BtlRecords));
        *(BtlRecord *)(((i << 6) + g_BtlRecords)) = *(BtlRecord *)(((i << 6) + g_BtlPartyKeptRecords));
        g_BtlPartyStandAnimations[i] = g_BtlSpecialBytes[i + 5];
        g_BtlPartyHitAnimations[i] = 0x12;
        *(u16 *)(((i << 6) + g_BtlRecords) + 6) = v6;
        *(u16 *)(((i << 6) + g_BtlRecords) + 8) = vA;
        *(u16 *)(((i << 6) + g_BtlRecords) + 0xA) = vA;
        *(u16 *)(((i << 6) + g_BtlRecords) + 0xC) = vC;
        BtlAnimationSet(((i << 6) + g_BtlRecords), g_BtlPartyStandAnimations[i]);
        g_BtlPartyNotHeldFlags[i] = 1;
      }
    }
    i++;
  } while (i < 4);
  r = rng_7f_draw();
  if (r < 0x32)
  {
    next = 7;
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
  }
  else if (r < 0x64)
  {
    next = 0;
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
    g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x50;
  }
  else
  {
    next = 0;
    i = next;
    do
    {
      cand[i] = 0;
      if (g_BtlSlotActiveFlags[i] != 0 && g_BtlPartyNotHeldFlags[i] != 0)
      {
        cand[(u8)next] = i;
        next++;
      }
      i++;
    } while (i < 4);
    if ((u8)next != 0)
    {
      g_BtlSlotPlannedTargets[g_BtlActingSlot] = cand[rng_mask_below(3, (u8)next) & 0xFF];
      next = 0xC;
      g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
      g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x4F;
    }
    else
    {
      next = 7;
      g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
    }
  }
  BtlActingAnimationSet(next);
}

extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
s32 BtlActingAnimationSet(s32);                                 /* extern */
extern u8 g_BtlActingSlot;

/* func_80041E24 */
void BtlEnemy6EEnemy70ActionSet(void) {
    *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 2;
    *(g_BtlActingSlot + g_BtlSlotPlannedTargets) = 0;
    BtlActingAnimationSet(0);
}
/* far_syms: g_BtlTargetSlot (auto: target reaches it %hi/%lo — addressing row 3) */
s32 rng_draw();

/* func_80041E78 */
void BtlEnemy71ActionSet(s32 planning)
{
  volatile unsigned int pad;
  planning &= 0xFF;
  if (planning)
  {
    if (rng_draw() & 1)
    {
      if (BtlEnemy71TargetPick(0xF0) & 0xFF)
      {
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
        g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x51;
        BtlActingAnimationSet(7);
      }
      else
      {
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
        BtlActingAnimationSet(0);
      }
    }
    else
    {
      g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
      BtlActingAnimationSet(0);
    }
  }
  else
    if (g_BtlActionKind == 2)
  {
    g_BtlTargetSlot = BtlPartyTargetPick(2);
  }
  else
    if (!(BtlEnemy71TargetPick(0xF0) & 0xFF))
  {
    g_BtlActionKind = 0xB;
  }
}
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

/* func_80041F6C */
u8 BtlEnemy71TargetPick(s16 max_x)
{
  u8 cand[3];
  u8 n;
  u8 i;

  n = 0;
  i = n;
  cand[0] = cand[1] = cand[2] = 0;
  do
  {
    if (g_BtlSlotActiveFlags[i] != 0)
    {
      if (max_x >= *((s16 *) (((u8 *) ((i << 6) + g_BtlRecords)) + 0x6)))
      {
        cand[n] = i;
        n++;
      }
    }
    i++;
  } while (i < 3);
  if (n != 0)
  {
    g_BtlTargetSlot = cand[rng_mask_below(3, n) & 0xFF];
    return 1;
  }
  return 0;
}

s32 rng_draw();                                /* extern */

/* func_80042034 */
void BtlEnemy72ActionSet(void)
{
  s32 animation;
  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
  if (rng_draw() & 1)
  {
    *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x52;
    g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlSlotNearestFind(2);
    animation = 0;
  }
  else
  {
    animation = 7;
    *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x53;
  }
  BtlActingAnimationSet(animation);
}

/* func_800420E0 */
void BtlEnemy73ActionSet(s32 planning_word)
{
    u8 planning;
    u8 animation;
    s32 draw;

    planning = planning_word & 0xFF;

    if (planning == 0)
    {
        if (g_BtlActionKind == 2)
        {
            g_BtlTargetSlot = BtlSlotNearestFind(2);
        }
        else
        {
            g_BtlTargetSlot = BtlPartyTargetPick(2);
        }

        return;
    }

    *(g_BtlActingSlot + g_BtlSlotPlannedTargets) = BtlPartyTargetPick(2);
    draw = rng_mask_below(7, 6);
    *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 7;

    switch (draw & 0xFF)
    {
        case 0:
        case 1:
        case 2:
            animation = 0;
            *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 2;
            break;

        case 3:
        case 4:
            animation = 7;
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x54;
            break;

        case 5:
            animation = 0xE;
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x55;
            break;
    }

    BtlActingAnimationSet(animation);
}

/* func_80042224 */
void BtlEnemy74ActionSet(s32 planning)
{
    u8 planning_then_draw_then_animation;

    planning_then_draw_then_animation = planning & 0xFF;

    if (planning_then_draw_then_animation == 0)
    {
        g_BtlTargetSlot = BtlSlotNearestFind(2);
        return;
    }

    planning_then_draw_then_animation = rng_7f_draw() & 0xFF;

    if (planning_then_draw_then_animation < 0x2A)
    {
        planning_then_draw_then_animation = 0;
        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 2;
    }
    else
    {
        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 7;

        if (planning_then_draw_then_animation < 0x54)
        {
            planning_then_draw_then_animation = 7;
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x56;
        }
        else
        {
            planning_then_draw_then_animation = 9;
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x57;
        }
    }

    BtlActingAnimationSet(planning_then_draw_then_animation);
}
extern u8 g_BtlActionNumber;
s32 rng_7f_draw();

/* func_800422F4 */
s32 BtlEnemy75ActionSet(s32 planning)
{
    u8 planning_then_draw_then_animation;

    planning_then_draw_then_animation = planning & 0xFF;

    if (planning_then_draw_then_animation == 0)
    {
        if ((g_BtlActionKind == 2) || (g_BtlActionNumber == 0x58))
        {
            g_BtlTargetSlot = BtlSlotNearestFind(2);
        }
        return;
    }

    g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlPartyTargetPick(2);
    planning_then_draw_then_animation = rng_7f_draw() & 0xFF;

    if (planning_then_draw_then_animation < 0x2A)
    {
        planning_then_draw_then_animation = 0;
        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 2;
    }
    else
    {
        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 7;

        if (planning_then_draw_then_animation < 0x54)
        {
            planning_then_draw_then_animation = 7;
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x58;
        }
        else
        {
            planning_then_draw_then_animation = 0xB;
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x59;
        }
    }

    return BtlActingAnimationSet(planning_then_draw_then_animation);
}

void BtlEnemy7EActionSet(void);  /* TU defines this below */
void BtlActingKindBSet(void);  /* TU defines this below */
s32 sprite_run_off(s32, s32);

/* func_80042408 */
void BtlEnemy76ActionSet(void)
{
  s32 animation;
  if (rng_draw() & 1)
  {
    animation = 0;
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
  }
  else
  {
    animation = (g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7);
    *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x5A;
  }
  g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0;
  BtlActingAnimationSet(animation);
}
/* far_syms: g_BtlNumber */
/* far_syms: g_BtlActionNumber */
extern u8 g_BtlNumber;
extern u8 g_BtlSpecialBytes[];

/* func_800424A8 */
void BtlEnemy77ActionSet(s32 planning)
{
  s16 lo;
  s16 hi;
  s16 y;
  u16 base;
  u8 i;

  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
  if (g_BtlNumber == 0xF9)
  {
    g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x7E;
    BtlActingAnimationSet(7);
    return;
  }
  if (planning & 0xFF)
  {
    switch (g_BtlSpecialBytes[0])
    {
      case 2:
      case 0:
        g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x5C;
        g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlPartyTargetPick(2);
        i = 0;
        break;
      case 1:
        i = 0xB;
        g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x5E;
        break;
      case 3:
        i = 7;
        g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x5D;
        break;
    }
    g_BtlSpecialBytes[0] = (g_BtlSpecialBytes[0] + 1) & 3;
    BtlActingAnimationSet(i);
    return;
  }
  if (g_BtlActionNumber == 0x5E)
  {
    base = *(u16 *)((g_BtlActingSlot << 6) + g_BtlRecords + 0xA);
    lo = base - 0x18;
    hi = base + 0x10;
    for (i = 0; i < 4; i++)
    {
      if (g_BtlSlotActiveFlags[i] != 0)
      {
        y = *(s16 *)((i << 6) + g_BtlRecords + 0xA);
        if (y >= lo && y <= hi)
          return;
      }
    }
    g_BtlActionNumber = 0x5D;
  }
}

/* func_800426B0 */
s32 BtlEnemy78ActionSet(s32 planning)
{
  u8 animation;
  unsigned char draw;
  planning &= 0xFF;
  if (planning == 0)
  {
    g_BtlTargetSlot = BtlPartyTargetPick(2);
    return;
  }
  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
  animation = 7;
  draw = rng_mask_below(0xF, 0xA);
  switch (draw)
  {
    case 0:

    case 1:
      *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 2;
      animation = 0;
      break;

    case 2:
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x5F;
      break;

    case 3:

    case 4:

    case 5:

    case 6:
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x60;
      break;

    default:
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x61;
      animation = 9;
      break;

  }

  return BtlActingAnimationSet(animation);
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800123C8_pad);
/* far_syms: g_BtlSpecialBytes (target reaches it %hi/%lo) */
extern u8 g_BtlSpecialBytes[];

/* func_800427C4 */
s32 BtlEnemy79ActionSet(s32 planning)
{
    s32 animation;

    if ((planning & 0xFF) == 0)
    {
        if (g_BtlActionKind == 2)
        {
            g_BtlTargetSlot = BtlPartyTargetPick(2);
        }
        return;
    }

    if (!(g_BtlSpecialBytes[0] & 1))
    {
        animation = 0;
        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 2;
    }
    else
    {
        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 7;
        animation = 7;

        switch (g_BtlSpecialBytes[1])
        {
        case 0:
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x62;
            break;
        case 1:
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x63;
            break;
        case 2:
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x64;
            break;
        case 3:
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x65;
            break;
        }
        g_BtlSpecialBytes[1] = (g_BtlSpecialBytes[1] + 1) & 3;
    }

    BtlActingAnimationSet(animation);
    g_BtlSpecialBytes[0]++;
}
/* far_syms: CharStats (target reaches it %hi/%lo) */
extern s8 g_BtlFallDone;
extern s32 CharStats;
extern u8 g_BtlSlotFallMarks[];
extern u16 g_BtlSlotAmounts[];
u8 BtlPartyTargetPick(s32);
void BtlActionOutcomeClear(void);
s32 rng_mask_below();
void BtlFrameWait(s32 frame_count);
void BtlAmountRecordsRun(void);
s32 BtlItem83Revive();
int BtlEndCheck(void);

/* func_80042958 */
void BtlEnemy7AActionSet(s32 planning)
{
    u8 targets[5];
    BtlRecord old;
    BtlRecord *obj;
    s32 act;
    u8 n;
    u8 i;
    s32 dmg;
    u16 *hp;
    u8 *turns;

    if ((planning & 0xFF) == 0)
    {
        if ((g_BtlActionNumber == 0x6A) || (g_BtlActionNumber == 0x6D))
        {
            g_BtlTargetSlot = BtlPartyTargetPick(2);
        }
        return;
    }
    act = 0;
    BtlActionOutcomeClear();
    g_BtlExtraRecords = (s32) &old;
    n = 0;
    for (i = 0; i < 4; i++)
    {
        g_BtlSlotAmountKinds[i] = g_BtlSlotFallMarks[i] = 0;
        turns = &g_BtlSpecialBytes[i];
        if (*turns != 0)
        {
            if (--*turns != 0)
            {
                n = 1;
                g_BtlSlotAmountKinds[i] = 1;
                g_BtlSlotAmounts[i] = dmg = rng_mask_below(0xF, 0xA) + 0xF;
                hp = (u16 *) (i * 0x3C + CharStats);
                if ((u16) dmg < *hp)
                {
                    *hp -= dmg;
                }
                else
                {
                    act = 1;
                    g_BtlSlotFallMarks[i] = 1;
                }
            }
            else
            {
                g_BtlPartyStandAnimations[i] = g_BtlSpecialBytes[i + 5];
                g_BtlPartyHitAnimations[i] = 0x12;
                g_BtlPartyNotHeldFlags[i] = 1;
                g_BtlExtraRecordCount = 1;
                old = ((BtlRecord *) g_BtlRecords)[i];
                ((BtlRecord *) g_BtlRecords)[i] = ((BtlRecord *) g_BtlPartyKeptRecords)[i];
                obj = (BtlRecord *) ((i << 6) + g_BtlRecords);
                obj->sprite.x = old.sprite.x;
                obj->sprite.y = obj->sprite.depth = old.sprite.y;
                obj->sprite.attr = old.sprite.attr;
                old.sprite.depth++;
                BtlAnimationSet((s32) &old, 0x18);
                BtlAnimationSet(g_BtlRecords + i * 0x40, g_BtlPartyStandAnimations[i]);
                BtlFrameWait(0xA);
                g_BtlExtraRecordCount = 0;
                sprite_run_off(0x1C, 1);
            }
        }
    }
    if (n)
    {
        for (i = 5; i < 14; i++)
        {
            g_BtlSlotAmountKinds[i] = g_BtlSlotFallMarks[i] = 0;
        }
        g_BtlFallDone = 0;
        BtlAmountRecordsRun();
        if ((u8) act)
        {
            BtlFallRun();
            BtlItem83Revive();
            if (BtlEndCheck() & 0xFF)
            {
                return;
            }
        }
    }
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
    act = rng_mask_below(7, 5);
    g_BtlSlotPlannedNumbers[g_BtlActingSlot] = act + 0x6A;
    switch (act & 0xFF)
    {
        case 0:
            act = 0;
            break;
        case 3:
            act = 0xB;
            break;
        case 4:
            act = 0xF;
            break;
        case 1:
        case 2:
            n = 0;
            for (i = 0; i < 5; i++)
            {
                targets[i] = 0;
                if ((g_BtlSlotActiveFlags[i] != 0) && (g_BtlPartyNotHeldFlags[i] != 0) && (*(u8 *) (CharStats + i * 0x3C + 0x15) == 0))
                {
                    targets[n] = i;
                    n++;
                }
            }
            if (n >= 4)
            {
                g_BtlSlotPlannedTargets[g_BtlActingSlot] = targets[rng_mask_below(3, n) & 0xFF];
                if ((act & 0xFF) == 1)
                {
                    act = 7;
                }
                else
                {
                    act = 9;
                }
            }
            else
            {
                act = 0xB;
                g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x6D;
            }
            break;
    }
    BtlActingAnimationSet(act & 0xFF);
}

/* func_80042E2C */
void BtlEnemy7BActionSet(s32 planning)
{
  u8 draw_then_animation;
  ;
  if ((planning & 0xFF) == 0)
  {
    g_BtlTargetSlot = BtlPartyTargetPick(2);
    return;
  }
  draw_then_animation = rng_draw() & 3;
  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
  switch (draw_then_animation)
  {
    case 0:
      draw_then_animation = 0;
      g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
      break;

    case 1:
      draw_then_animation = 0xA;
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x6F;
      break;

    case 2:
      draw_then_animation = 0x7;
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x70;
      break;

    case 3:
      draw_then_animation = 0xC;
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x71;
      break;

  }

  BtlActingAnimationSet(draw_then_animation);
}
/* func_80042F4C */
void BtlEnemy7CActionSet(void)
{
  u8 draw_then_slot_then_animation;
  u8 count;

  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
  g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlPartyTargetPick(2);
  rng_draw();
  draw_then_slot_then_animation = rng_mask_below(7, 5);
  switch (g_BtlSpecialBytes[7])
  {
    case 4:
      draw_then_slot_then_animation = rng_draw() & 1;
      if (draw_then_slot_then_animation)
      {
block_6:
        draw_then_slot_then_animation = 3;
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
        g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlPartyTargetPick(2);
        break;
      }
    case 0:
    case 1:
    case 2:
      switch ((u8) rng_mask_below(3, 4))
      {
        case 0:
          draw_then_slot_then_animation = 4;
          g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x7F;
          break;
        case 1:
          draw_then_slot_then_animation = 0;
          g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x80;
          break;
        case 2:
          draw_then_slot_then_animation = 2;
          g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x82;
          break;
        case 3:
          draw_then_slot_then_animation = 6;
          g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x83;
          break;
      }
      break;
    case 3:
      count = 0;
      for (draw_then_slot_then_animation = 0; draw_then_slot_then_animation < 5; draw_then_slot_then_animation++)
      {
        if (g_BtlSlotActiveFlags[draw_then_slot_then_animation] == 0)
          count++;
      }
      draw_then_slot_then_animation = 0x13;
      if (count < 2)
      {
        g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0x81;
      }
      else
      {
        goto block_6;
      }
      break;
  }
  g_BtlSpecialBytes[7]++;
  if (g_BtlSpecialBytes[7] >= 5)
    g_BtlSpecialBytes[7] = 0;
  BtlActingAnimationSet(draw_then_slot_then_animation);
}

/* func_800431AC */
void BtlEnemy7DActionSet(void)
{
  s32 animation;

  if (g_BtlSpecialBytes[0] < 3)
  {
    animation = 3;
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 0xB;
  }
  else
  {
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
    if (g_BtlSpecialBytes[0] == 3)
    {
      animation = 4;
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x63;
    }
    else
    {
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x73;
      g_BtlSlotPlannedTargets[g_BtlActingSlot] = BtlPartyTargetPick(2);
      animation = 2;
    }
  }

  BtlActingAnimationSet(animation);
  g_BtlSpecialBytes[0]++;
}

/* far_syms: g_BtlSlotPlannedTargets (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */

/* func_80043298 */
void BtlEnemy7EActionSet(void)
{
  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
  switch (rng_draw() & 3)
  {
    case 0:
      g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
      break;
    case 1:
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x74;
      break;
    case 2:
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x75;
      break;
    case 3:
      *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = 0x76;
      break;
  }
  g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0;
}
/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */

/* func_80043398 */
s32 BtlActingAnimationSet(s32 animation) {
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), animation & 0xFF);
    g_BtlSlotRestAnimations[g_BtlActingSlot] = animation;
}
/* func_800433F0 */
void BtlActingKindBSet(void)
{
    g_BtlSlotPlannedKinds[g_BtlActingSlot] = 0xB;
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0);
    g_BtlSlotRestAnimations[g_BtlActingSlot] = 0;
}
