/* Lunar SSSC — btl_slot: the slots' checks (trait, reach) and their
   records' animations (rest, action, spell, status), push back and size.
   The first split had it in game_state.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: CharStats g_BtlRecords */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActionKind */
/* far_syms: CharStats */
/* far_syms: g_BtlRecords */
/* far_syms: CharStats g_BtlActingStatus */
/* far_syms: g_BtlActingSlot */
/* far_syms: g_BtlActingSlot g_BtlRecords */
/* far_syms: CharStats g_BtlTargetSlot */
/* far_syms: g_BtlActingSlot g_BtlActionKind */
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot */
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "chr.h"
#include "btl.h"
extern u16 g_BtlPadRecord[];
s32 BtlAnimationEndWait();
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern u8 g_BtlPartyNotHeldFlags[];
extern u8 g_BtlActingStatus;
extern u8 g_BtlPartyStandAnimations[];
extern u8 g_BtlSlotRestAnimations[];
extern u8 g_BtlSpecialBytes[];
extern long HWD0;
extern long VWD0;
extern s32 g_BtlRecords;
extern u8 g_BtlActionKind;
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlSlotPlannedNumbers[];
extern s32 CharStats; 
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotIds[];
extern BtlWalkBlock g_BtlWalkBlock;
extern u8 g_BtlActingSlot;
extern u8 g_BtlSlotActiveFlags[];
s32 BtlTargetReachCheck();
void BtlActingRestAnimationSet();
void BtlSlotStatusAnimationSet(u8);
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlFrameStep();
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
s32 BtlTargetReachCheck();
void BtlActingRestAnimationSet();
void BtlSlotStatusAnimationSet(u8);
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlFrameStep();
/* far_syms: CharStats g_BtlTargetSlot g_BtlSlotActiveFlags */
void BtlSlotActionAnimationSet(u8 param);
void BtlSlotActionAnimationSet(u8 param);
s32 BtlTargetReachCheck();
/* far_syms: g_BtlTargetSlot */
s32 BtlTargetReachCheck();
void BtlActingRestAnimationSet();
void BtlActingRestAnimationSet();
void BtlSlotStatusAnimationSet(u8);
s32 BtlTargetReachCheck();
void BtlSlotStatusAnimationSet(u8);
s32 BtlAnimationSet();
s32 BtlWalkSet();
s32 BtlWalkRun();
/* far_syms: CharStats (target reaches it %hi/%lo) */
void BtlSlotStatusAnimationSet(u8);
/* far_syms: CharStats g_BtlActingSlot g_BtlTargetSlot */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot */
/* far_syms: g_BtlTargetSlot g_BtlActingSlot g_BtlRecords CharStats */
/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlActingSlot CharStats g_BtlTargetSlot g_BtlRecords */
/* far_syms: g_BtlActingStatus (target reaches it %hi/%lo) */
/* far_syms: g_BtlActingStatus (auto: target reaches it %hi/%lo — addressing row 3) */
extern long HWD0, VWD0;
/* far_syms: g_BtlPadRecord (target reaches it %hi/%lo) */
s32 pad_record_update(s32 *, s32);
/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
s32 BtlFrameStep();
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern void (*g_BtlActingShowFunctions[])(void);

s32 pad_record_update(s32 *, s32);

/* func_80037604 */
s32 BtlPadHeldCheck(s32 button_mask)
{
    pad_record_update((s32 *)g_BtlPadRecord, 0);
    return (g_BtlPadRecord[0] & button_mask) != 0;
}

/* func_80037648 */
s32 BtlTargetTraitCheck(trait_mask, first_trait, second_trait, third_trait)
s32 trait_mask;
u8 first_trait;
u8 second_trait;
u8 third_trait;
{
    u8 i;

    if (g_BtlTargetSlot < 5)
    {
        i = 0;
        do
        {
            if ((*(u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C) + i) + 0x28)) == first_trait)
            {
                return 1;
            }
            i++;
        } while (i < 6);

        if (second_trait == 0)
        {
            return 0;
        }

        i = 0;
        do
        {
            if ((*(u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C) + i) + 0x28)) == second_trait)
            {
                return 1;
            }
            i++;
        } while (i < 6);

        if (third_trait == 0)
        {
            return 0;
        }

        i = 0;
        do
        {
            if ((*(u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C) + i) + 0x28)) == third_trait)
            {
                return 1;
            }
            i++;
        } while (i < 6);

        return 0;
    }

    if (first_trait)
    {
        if ((*(u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1B)) == first_trait)
        {
            return 1;
        }
    }

    if (second_trait)
    {
        if ((*(u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1B)) == second_trait)
        {
            return 1;
        }
    }

    if (third_trait)
    {
        if ((*(u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1B)) == third_trait)
        {
            return 1;
        }
    }

    if ((trait_mask & 0xFFFF) != 0)
    {
        if (((*(u16 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x2A)) & trait_mask) != 0)
        {
            return 1;
        }

        if (((*(u16 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x28)) & trait_mask) != 0)
        {
            return 2;
        }
    }

    return 0;
}

/* func_80037890 */
void BtlActingRestAnimationSet()
{
  u8 *ptr;
  u8 status;
  u8 animation;
  ptr = (u8 *) (((g_BtlActingSlot * 0x3C) + CharStats) + 0x15);
  status = *ptr;
  g_BtlActingStatus = status;
  if (g_BtlSlotActiveFlags[g_BtlActingSlot] == 0)
  {
    goto end;
  }
  if (g_BtlActionKind == 0xB)
  {
    goto end;
  }
  if (g_BtlActingSlot < 5)
  {
    if (g_BtlPartyNotHeldFlags[g_BtlActingSlot] == 0)
    {
      goto end;
    }
  }
  if (status != 0)
  {
    BtlSlotStatusAnimationSet(g_BtlActingSlot);
    goto end;
  }
  if (g_BtlActingSlot < 5)
  {
    if (g_BtlActionKind == 5)
    {
      animation = 0x13;
    }
    else
    {
      animation = g_BtlPartyStandAnimations[g_BtlActingSlot];
    }
  }
  else
  {
    animation = 0;
  }
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), animation);
  g_BtlSlotRestAnimations[g_BtlActingSlot] = animation;
  end:
  ;

  ;
  ;
}

/* func_800379AC */
s32 BtlTargetReachCheck(void)
{
  BtlWalkBlock *blk;
  s32 first_animation;
  s32 second_animation;
  s32 rec;
  u16 record7_y;
  BtlWalkSet();
  blk = &g_BtlWalkBlock;
  blk->walk = 0;
  if (BtlWalkRun() & 0xFF)
  {
    return 1;
  }
  blk->walk = 1;
  if (g_BtlActingSlot < 5)
  {
    BtlWalkRun();
    goto fail;
  }
  first_animation = 9;
  second_animation = 0xA;
  switch (g_BtlSlotIds[g_BtlActingSlot])
  {
    case 0x59:

    case 0x2A:

    case 0x39:

    case 0x48:
      g_BtlWalkBlock.animation = 0xB;
      break;

    case 0x79:
      first_animation = 1;
      second_animation = 0xB;
      blk->animation = 0xD;
      break;

    case 0x7D:
      first_animation = 6;
      second_animation = 7;
      blk->animation = 1;
      break;

    case 0x12:
      blk->animation = 1;
      break;

    case 0x7C:
      first_animation = 8;
      second_animation = 9;
      blk->animation = 1;
      BtlAnimationSet(g_BtlRecords + 0x1C0, 0x15);
      break;

    case 0x7F:
      first_animation = 0xB;
      second_animation = 0xC;
      blk->animation = 0x10;
      break;

    default:
      goto fail;

  }

  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), first_animation);
  BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
  BtlWalkRun();
  if (g_BtlSlotIds[g_BtlActingSlot] == 0x7C)
  {
    rec = (g_BtlActingSlot << 6) + g_BtlRecords;
    ((BtlRecord *)g_BtlRecords)[7].sprite.x = AS(u16, ((BtlRecord *)rec)->sprite.x);
    record7_y = AS(u16, ((BtlRecord *)rec)->sprite.y);
    ((BtlRecord *)g_BtlRecords)[7].sprite.y = record7_y;
    ((BtlRecord *)g_BtlRecords)[7].sprite.depth = record7_y;
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0x16);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), second_animation);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0xB);
    return 0;
  }
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), second_animation);
  BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
  return 0;
  fail:
  BtlWalkRun();

  return 0;
}
/* func_80037C64 */
void BtlTargetPushBack(void)
{
    s16 target_x;
    u8 i;
    s32 step;
    u32 step_count;
    s16 max_x;
    s16 min_x;
    u16 width;
    u16 new_x;

    if (g_BtlActionKind != 2)
        return;

    if (g_BtlSlotIds[g_BtlTargetSlot] == 0x77)
        return;

    if (g_BtlSlotIds[g_BtlTargetSlot] == 0x71)
        return;

    step_count = 8;
    if (g_BtlSlotIds[g_BtlTargetSlot] >= 0x6E)
        step_count = 2;

    if ((*((u16 *)(CharStats + g_BtlTargetSlot * 60 + 2)) / 3) < *((u16 *)(CharStats + g_BtlTargetSlot * 60)))
        return;

    if ((*((u16 *)(g_BtlRecords + g_BtlActingSlot * 0x40 + 0xC))) & 0x2000)
    {
        *((u16 *)(g_BtlRecords + g_BtlTargetSlot * 0x40 + 0xC)) = 0x20;
        step = -4;
    }
    else
    {
        *((u16 *)(g_BtlRecords + g_BtlTargetSlot * 0x40 + 0xC)) = 0x2020;
        step = 4;
    }

    i = 0;
    width = *((u16 *)(g_BtlRecords + g_BtlTargetSlot * 0x40 + 0x3C));
    target_x = *((u16 *)(g_BtlRecords + g_BtlTargetSlot * 0x40 + 0x6));
    new_x = (width >> 1) + 8;
    max_x = 0x138 - (width >> 1);

    min_x = new_x;

    for (; i < step_count; i++)
    {
        if (min_x < target_x && target_x < max_x)
        {
            new_x = target_x + step;
            *((u16 *)(g_BtlRecords + g_BtlTargetSlot * 0x40 + 6)) = new_x;
            target_x = new_x;
            if (g_BtlSlotIds[g_BtlTargetSlot] == 0x7C)
            {
                if (((BtlStats *)CharStats)[7].hp != 0)
                    ((BtlRecord *)g_BtlRecords)[7].sprite.x = new_x;
            }
            BtlFrameStep();
        }
    }
}

/* func_80037E64 */
void BtlSlotActionAnimationSet(u8 param)
{
  u16 animation;

  if (*(u8 *)((CharStats + (param * 0x3C)) + 0x15))
  {
    BtlSlotStatusAnimationSet(param);
    return;
  }

  if (param < 0x5)
  {
    switch (g_BtlSlotPlannedKinds[param])
    {
      case 3:
        animation = BtlSlotSpellAnimationGet();
        break;
      case 5:
        animation = 0x13;
        break;
      default:
        if (g_BtlSlotRestAnimations[param] == 0x13)
        {
          animation = 0x13;
        }
        else
        {
          animation = g_BtlPartyStandAnimations[param];
        }
        break;
    }
  }
  else
  {
    if (g_BtlSlotPlannedKinds[param] == 0x7)
    {
      animation = 0x7;
    }
    else
    {
      animation = 0;
    }
  }

  BtlAnimationSet(g_BtlRecords + (param << 6), animation);
  g_BtlSlotRestAnimations[param] = animation;
}
/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */

/* func_80037F84 */
u8 BtlSlotSpellAnimationGet(u8 arg)
{
  int spell;
  u8 planned_number;
  u8 id;
  planned_number = g_BtlSlotPlannedNumbers[arg];
  id = g_BtlSlotIds[arg];
  switch (id - 1)
  {
    case 0:

    case 10:
      if (planned_number != 0)
    {
      spell = (int) planned_number;
      if (spell < 5)
      {
        return 0x1F;
      }
      if (spell < 9)
      {
        return 0x1E;
      }
    }
      break;

    case 1:

    case 2:

    case 3:

    case 5:

    case 7:
      return 0x1E;

    case 4:
      return 0x1F;

    case 9:
      if (planned_number == 0x2F)
    {
      return 0x1F;
    }
      return 0x1E;

  }

}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80011E34_pad);
/* far_syms: g_BtlPartyStandAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSpecialBytes (target reaches it %hi/%lo) */

/* func_8003801C */
void BtlSlotStatusAnimationSet(u8 param)
{
  u8 *ptr;
  u8 animation;
  s32 result;
  ptr = (u8 *) ((CharStats + (param * 0x3C)) + 0x15);
  if ((*ptr) == 0)
  {
    if (g_BtlSlotPlannedKinds[param] != 0)
    {
      return BtlSlotActionAnimationSet(param);
    }
  }
  if (param < 0x5)
  {
    animation = g_BtlSlotRestAnimations[param];
    if (g_BtlSpecialBytes[param] == 0)
    {
      if ((*ptr) == 0x30)
      {
        animation = 0x19;
      }
      else
        if ((*ptr) & 0x20)
      {
        animation = 0x18;
      }
      else
        if ((*ptr) & 0x10)
      {
        animation = 0x17;
      }
      else
        if ((*ptr) & 0x4)
      {
        animation = 0x15;
      }
      else
        if ((*ptr) & 0x8)
      {
        animation = 0x16;
      }
      else
        if ((*ptr) & 0x40)
      {
        animation = 0x1A;
      }
      else
        if ((*ptr) & 0x1)
      {
        animation = 0x14;
      }
      else
        if ((*ptr) & 0x80)
      {
        animation = 0x1B;
      }
      else
      {
        animation = g_BtlPartyStandAnimations[param];
      }
    }
  }
  else
  {
    animation = 0;
    if ((*ptr) & 0x30)
    {
      animation = 5;
    }
    else
      if ((*ptr) & 0x4)
    {
      animation = 3;
    }
    else
      if ((*ptr) & 0x8)
    {
      animation = 4;
    }
    else
      if ((*ptr) & 0x40)
    {
      animation = 0x11;
    }
    else
      if ((*ptr) & 0x80)
    {
      animation = 0x10;
    }
  }
  result = BtlAnimationSet(g_BtlRecords + (param << 6), animation);
  g_BtlSlotRestAnimations[param] = animation;
  return result;
}

/* func_800381D8 */
void BtlRecordSizeSet(u8 *obj, u16 frame)
{
    u16 maxx;
    u16 maxy;
    s16 minx;
    s16 miny;
    u16 *groups;
    SpritePiece *parts;
    u16 *anim;
    u16 *seq;
    u16 n;

    maxx = 0;
    maxy = 0;
    minx = 0;
    miny = 0;
    groups = *(u16 **)(obj + 0x14);
    parts = *(SpritePiece **)(obj + 0x18);
    anim = *(u16 **)(obj + 0x28);
    seq = *(u16 **)(obj + 0x2C);
    while (frame != 0)
    {
        frame--;
        anim += 2;
    }
    seq += anim[1] * 2;
    groups += seq[0] * 2;
    n = groups[0];
    parts += groups[1];
    while (n != 0)
    {
        if ((s16)AS(u16, parts->x0) < minx)
            minx = AS(u16, parts->x0);
        if (maxx < (s16)AS(u16, parts->x0) + AS(u16, parts->width))
            maxx = AS(u16, parts->x0) + AS(u16, parts->width);
        if ((s16)AS(u16, parts->y0) < miny)
            miny = AS(u16, parts->y0);
        if (maxy < (s16)AS(u16, parts->y0) + AS(u16, parts->height))
            maxy = AS(u16, parts->y0) + AS(u16, parts->height);
        n--;
        parts++;
    }
    if (minx < 0)
        minx *= -1;
    ((BtlRecord *)obj)->width = minx + maxx;
    if (miny < 0)
        miny *= -1;
    ((BtlRecord *)obj)->height = miny + maxy;
}
