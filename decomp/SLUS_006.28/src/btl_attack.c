/* Lunar SSSC — btl_attack: an attack: the walk to the target, the miss and
   the hit shown, the animation with its sound or show, the target found,
   the outcome cleared and the attack on all. The first split had it in
   game_misc_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot g_BtlSlotIds g_BtlSlotAmounts */
/* far_syms: g_BtlSlotAmounts (target reaches it %hi/%lo) */
/* far_syms: g_BtlTargetSlot g_BtlSlotAmounts */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
/* far_syms: CharStats (carried from the first split's game_misc.c) */
/* far_syms: g_BtlPartyHitAnimations (carried from the first split's game_misc.c) */
/* far_syms: g_BtlRecords (carried from the first split's game_misc.c) */
/* far_syms: g_BtlSlotIds (carried from the first split's game_misc.c) */
/* far_syms: g_BtlSlotAmountKinds (carried from the first split's game_misc.c) */
extern s32 CharStats;
extern u8 g_BtlPartyHitAnimations[];
extern u8 g_BtlActingStatus;
extern u8 g_BtlActingSlot;
extern s32 g_BtlRecords;
extern u8 g_BtlTargetSlot;
extern u16 g_BtlSlotAmounts[];
extern u8 g_BtlSlotIds[];
extern u8 g_BtlSlotAmountKinds[];
extern u8 g_BtlSlotRestAnimations[];
extern u8 g_BtlSlotFallMarks[];
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlSlotStrongMarks[];
s32 BtlAnimationSet(s32, u8);
void BtlAnimationEndWait(void *record);  /* extern */
void BtlTargetForwardMove(void);  /* extern */
void BtlAmountShowWait(void);  /* extern */
s32 BtlAttackWalkRun();
s32 BtlAttackAllRun();
s32 BtlActingRestAnimationSet();
s32 SoundPlay(s32, s32);
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: CharStats */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyHitAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
s32 BtlAmountShowStart(s32);
void BtlAttackShowRun(s32 show_number);
extern BtlWalkBlock g_BtlWalkBlock;

/* func_80031244 */
s32 BtlAttackWalkRun(void)
{
	u8 *ptr;
	s32 c;
	s32 ret;

	if (g_BtlActingSlot < 5)
	{
		ptr = (u8 *)((g_BtlActingSlot * 0x3C) + CharStats);
		c = ptr[0x20];
		if (c < 3)
			goto common;
		if (c < 5)
			return 1;
		if (c != 5)
			goto common;
		if (g_BtlSlotIds[g_BtlActingSlot] != 2)
			goto common;
ret1:
		return 1;
common:
		if (g_BtlActingStatus & 0x20)
		{
			g_BtlWalkBlock.walk = 0;
			ret = BtlWalkRun();
			return ((ret & 0xFF) ^ 2) < 1;
		}
		ret = BtlWalkRun();
		return ret & 0xFF;
	}

	ptr = (u8 *)((g_BtlActingSlot * 0x3C) + CharStats);
	switch (ptr[0x17] - 0x98)
	{
		case 0:
		case 2:
		case 3:
		case 12:
		case 13:
		case 20:
		case 23:
		case 25:
			goto ret1;

		case 5:
		case 15:
			ret = BtlTargetReachCheck();
			return ret & 0xFF;

		default:
			ret = BtlWalkRun();
			return ret & 0xFF;
	}
}

/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */
void BtlAttackHitShow(void);  /* TU defines this below */

/* func_80031378 */
void BtlAttackMissShow(void)
{
  int target_record;
  if (g_BtlActingSlot >= 6)
  {
    if (*((u8 *) (((g_BtlActingSlot * 0x3C) + CharStats) + 0x17)))
    {
      BtlActingShowRun(*((u8 *) ((CharStats + (g_BtlActingSlot * 0x3C)) + 0x17)));
      return;
    }
  }
  BtlAttackAnimationRun(0);
  if (g_BtlTargetSlot < 5)
  {
    BtlFrameWait(0x10);
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x11);
    BtlTargetBackMove();
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    BtlActingRestAnimationSet();
    BtlTargetForwardMove();
  }
  else
  {
    BtlTargetMissShow();
  }
  target_record = g_BtlRecords + (g_BtlTargetSlot << 6);
  *((u16 *) (((g_BtlActingSlot << 6) + g_BtlRecords) + 8)) -= 1;
  BtlAnimationSet(target_record, g_BtlSlotRestAnimations[g_BtlTargetSlot]);
}

/* far_syms: g_BtlSlotStrongMarks (target reaches it %hi/%lo) */

/* func_800314B8 */
void BtlAttackHitShow(void)
{
  if (g_BtlActingSlot >= 6)
  {
    if (*((u8 *)(g_BtlActingSlot * 0x3C + CharStats) + 0x17))
    {
      BtlActingShowRun(*((u8 *)(g_BtlActingSlot * 0x3C + CharStats) + 0x17));
      return;
    }
  }

  BtlAttackAnimationRun(1);
  SoundPlay(g_BtlSlotStrongMarks ? 0x38 : 0x23, 3);

  if (g_BtlTargetSlot < 5)
  {
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlPartyHitAnimations[g_BtlTargetSlot]);
  }

  BtlActingRestAnimationSet();
  BtlTargetPushBack();
  BtlAmountShowStart(0);
  BtlAmountShowWait();
}

/* far_syms: g_BtlPartyAttackAnimations (target reaches it %hi/%lo) */
extern u8 g_BtlPartyAttackAnimations[];
s32 BtlFacingBothSet();

static inline int inline_fn(int word_offset)
{
  return word_offset * 4;
}
/* func_800315A8 */
void BtlAttackAnimationRun(s32 unused)
{
  s32 show_byte;
  s32 show_number;
  unsigned int slot_times_16;
  u8 worn_item;
  void *end_record;
  void *record;
  record = (g_BtlActingSlot << 6) + g_BtlRecords;
  ((BtlRecord *)record)->sprite.depth = (u16) (AS(s32, ((BtlRecord *)record)->sprite.depth) + 1);
  slot_times_16 = g_BtlActingSlot * 0x10;
  BtlFacingBothSet();
  show_number = 0;
  if (((u8) g_BtlActingSlot) < 5U)
  {
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), *(g_BtlActingSlot + g_BtlPartyAttackAnimations));
    ;
    if ((((*(g_BtlActingSlot + g_BtlSlotIds)) == 2) && ((slot_times_16, (*((u8 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x20))) == 5))) || ((*((u8 *) (((u8 *) (inline_fn((g_BtlActingSlot * 0x10) - g_BtlActingSlot) + CharStats)) + 0x20))) == 3))
    {
      show_number = 0x3A;
    }
    worn_item = *((s16 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x22));
    if (worn_item == 0x19)
    {
      show_number = 0x3B;
    }
    show_byte = show_number & 0xFF;
    if (worn_item == 0x1A)
    {
      show_number = 0x3C;
      goto block_10;
    }
  }
  else
  {
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 2U);
    block_10:
    show_byte = show_number & 0xFF;
  }
  if (show_byte != 0)
  {
    BtlAttackShowRun(show_byte);
  }
  else
  {
    SoundPlay((u8) (*((u8 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x19))), 3);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
  }
  end_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  ((BtlRecord *)end_record)->sprite.depth = (u16) (AS(s32, ((BtlRecord *)end_record)->sprite.depth) - 1);
}

s32 BtlShowNumberRun();                               /* extern */

/* func_8003179C */
void BtlAttackShowRun(s32 show_number) {
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    BtlShowNumberRun(show_number & 0xFF);
}

/* far_syms: g_BtlActingStatus (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

s32 BtlEndCheck();                                /* extern */
u8 BtlSlotNearestFind(s32);                                /* extern */

/* func_800317E0 */
s32 BtlAttackTargetFind(void)
{
  s32 slot_range;
  s32 end_state;
  end_state = 0;
  if (g_BtlSlotActiveFlags[g_BtlTargetSlot] == 0)
  {
    end_state = BtlEndCheck() & 0xFF;
    if (end_state == 0)
    {
      slot_range = 0;
      if (!(g_BtlActingStatus & 0x40))
      {
        slot_range = 2;
        if (((u8) g_BtlActingSlot) < 6U)
        {
          slot_range = 1;
        }
      }
      g_BtlTargetSlot = BtlSlotNearestFind(slot_range);
      end_state = 0;
    }
    return end_state;
  }
  return end_state;
}

/* func_80031878 */
void BtlAttackOutcomeClear(void)
{
  s32 slot;
  s32 i;
  i = 0;
  do
  {
    slot = i & 0xFF;
    i += 1;
    *(slot + g_BtlSlotStrongMarks) = (g_BtlSlotAmountKinds[slot] = (*(slot + g_BtlSlotFallMarks) = 0));
    *(u16 *)((slot * 2) + (u8 *)g_BtlSlotAmounts) = 0;
  }
  while (((u32) (i & 0xFF)) < 0xEU);
}
s32 BtlHitAmountSet();
s32 BtlHitSlotsUpdate();

/* func_800318DC */
s32 BtlAttackAllRun(void)
{
    u8 i;
    u8 a;

    if (g_BtlActingStatus & 0x40)
    {
        i = 0;
        a = 0;
    }
    else
    {
        i = 6;
        a = 1;
    }
    g_BtlTargetSlot = BtlSlotNearestFind((u8) (a));
    BtlAttackAnimationRun(1);
    BtlActingRestAnimationSet();
    for (g_BtlTargetSlot = i; g_BtlTargetSlot < 0xE; g_BtlTargetSlot++)
    {
        if (g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0 && g_BtlActingSlot != g_BtlTargetSlot)
        {
            g_BtlSlotAmountKinds[g_BtlTargetSlot] = 1;
            BtlHitAmountSet();
            if (g_BtlTargetSlot < 5)
            {
                BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlPartyHitAnimations[g_BtlTargetSlot]);
            }
            else if (g_BtlSlotIds[g_BtlTargetSlot] == 0x2F)
            {
                BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 7);
                g_BtlSlotAmounts[g_BtlTargetSlot] = 0;
                g_BtlSlotStrongMarks[g_BtlTargetSlot] = 0;
                g_BtlSlotAmountKinds[g_BtlTargetSlot] = 2;
            }
        }
    }
    BtlAmountShowStart(0);
    BtlAmountShowWait();
    return BtlHitSlotsUpdate();
}
