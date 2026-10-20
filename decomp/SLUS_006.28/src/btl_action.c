/* Lunar SSSC — btl_action: the turns of a round and the action of each: the
   order made and run, an action run by its kind (walk, spell, item,
   series), and adjusted to the slot's state, mode and target. The first
   split had it in game_misc.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: CharStats */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlTargetSlot */
/* far_syms: CharStats g_BtlNumber g_BtlSlotAmounts g_BtlActingSlot g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber */
/* far_syms: g_BtlNumber CharStats g_BtlTargetSlot */
/* far_syms: g_BtlNumber g_BtlActingSlot CharStats g_BtlTargetSlot */
/* far_syms: g_BtlSlotAmounts (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (auto: target reaches it %hi/%lo — addressing row 3) */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
/* far_syms: g_BtlActingStatus (carried from the first split's game_misc.c) */
/* far_syms: g_BtlSlotAmounts (carried from the first split's game_misc.c) */
/* far_syms: g_BtlSlotActiveFlags (carried from the first split's game_misc.c) */

extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlNumber;
s32 BtlAnimationSet(s32, u8);
extern u8 g_BtlSlotFallMarks[];
extern u16 g_BtlSlotAmounts[];
extern u8 g_BtlSlotIds[];
extern s32 g_BtlRecords;
extern u8 g_BtlActingStatus;
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlActingSlot;
extern u8 g_BtlFallDone;
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotActiveFlags[];
s32 BtlFrameStep();                                  /* extern */
extern long HWD0, VWD0;
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */
extern BtlWalkBlock g_BtlWalkBlock;
void BtlAnimationEndWait(void *record);
s32 SoundPlay(s32, s32);
extern long HWD0;
extern long VWD0;
s32 BtlActionKindRun();
s32 BtlActionAdjust();
s32 BtlActionOutcomeClear();
s32 BtlActingRestAnimationSet();
s32 BtlPartyPanelDisp();
s32 BtlPartyTurnActionSet();
s32 BtlActionRun(void);
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern s32 CharStats;
void BtlAnimationEndWait(void *record);

/* far_syms: g_BtlPartyMode */
extern u8 g_BtlPartyMode;
s32 BtlTurnOrderRun(void);  /* TU defines this below */

/* func_8002DB8C */
u8 BtlRoundTurnsRun(void)
{
    u8 i;

    for (i = 0; i < 0xE; i++)
    {
        g_BtlSlotFallMarks[i] = 0;
        g_BtlSlotAmounts[i] = 0;
    }
    switch (g_BtlPartyMode)
    {
    case 0:
    case 1:
        return BtlTurnOrderRun();
    case 2:
        return BtlTurnOrderRun();
    }
}

/* far_syms: g_BtlSlotPlannedTargets (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */
/* far_syms: g_BtlTurnOrder (target reaches it %hi/%lo) */
extern s8 g_BtlActionShowNumber;
extern s16 g_BtlActionBitNumber;
extern s8 g_BtlActionFunctionNumber;
extern u8 g_BtlTurnOrderCursor;
extern u16 g_BtlActionPower;  /* TU declares this below */
extern s16 g_BtlAreaHalfWidth;
extern s16 g_BtlAreaHalfHeight;
extern s8 D_800B8C32;
extern s8 D_800B8C33;
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedNumbers[];
extern u8 g_BtlTurnOrder[];

/* func_8002DC30 */
s32 BtlTurnOrderRun(void)
{
    u8 end_state;

    BtlTurnOrderMake();

    for (g_BtlTurnOrderCursor = 0; g_BtlTurnOrderCursor < 0xD; g_BtlTurnOrderCursor++)
    {
        if (g_BtlTurnOrder[g_BtlTurnOrderCursor] == 0)
            return 0;

        g_BtlActionBitNumber = 0;
        g_BtlAreaHalfHeight = 0;
        g_BtlAreaHalfWidth = 0;
        g_BtlActionPower = 0;
        D_800B8C33 = 0;
        D_800B8C32 = 0;
        g_BtlActionFunctionNumber = 0;
        g_BtlActionShowNumber = 0;

        g_BtlActingSlot = g_BtlTurnOrder[g_BtlTurnOrderCursor] - 1;
        g_BtlActionKind = g_BtlSlotPlannedKinds[g_BtlActingSlot];
        g_BtlActionNumber = g_BtlSlotPlannedNumbers[g_BtlActingSlot];
        g_BtlTargetSlot = g_BtlSlotPlannedTargets[g_BtlActingSlot];
        g_BtlActingStatus = *(u8 *)(CharStats + g_BtlActingSlot * 60 + 0x15);

        if (g_BtlSlotIds[g_BtlActingSlot] == 0x7C)
            end_state = BtlActionSeriesRun() & 0xFF;
        else
            end_state = BtlActionRun() & 0xFF;

        if (end_state != 0)
            return end_state;
    }

    return 0;
}
/* far_syms: g_BtlActingSlot g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber g_BtlSlotPlannedKinds g_BtlSlotActiveFlags CharStats */
s32 BtlEndCheck();
u8 BtlPartyTargetPick(s32);
s32 rng_list_pick(s32 list, s32 count, s32 mask);

/* The turn of enemy id 0x7C (Magic Emperor, second form): BtlTurnOrderRun
   calls this instead of BtlActionRun for a slot whose g_BtlSlotIds is 0x7C.
   One planned action becomes a SCRIPT of two or three, each run by
   BtlActionRun(); a nonzero result from it ends the script. Kind 7
   (g_BtlActionKind) switches on the planned class g_BtlActionNumber (BtlEnemy7CActionSet's
   0x7F..0x83, which name no special of their own):
     0x7F  special 0x64, then 0x72 at a fresh BtlPartyTargetPick(2) target
     0x80  special 0x62, then 0x63
     0x81  special 0x68 at one of the active slots 0..3 (rng_list_pick,
           mask 3), then 0x67 while record 7's HP (CharStats + 0x1A4) is
           0, else the two plain attacks
     0x82  special 0x73 at a fresh target, then 0x65
     0x83  special 0x66, then the two plain attacks
   and nothing at all for any other class. Every other kind: two plain
   attacks (kind 2), each at a fresh BtlPartyTargetPick(2) target. Every step
   but a script's first writes its kind to g_BtlActionKind AND to the slot's
   plan g_BtlSlotPlannedKinds (BtlActionRun zeroes the plan entry when it returns
   0). Returns BtlEndCheck(), the fight's state, as a byte. */
/* func_8002DD88 */
s32 BtlActionSeriesRun(void)
{
  u8 live[4];
  u8 n;
  u8 i;

  if (g_BtlActionKind == 7)
  {
    switch (g_BtlActionNumber)
    {
      case 0x7F:
        g_BtlActionNumber = 0x64;
        if (BtlActionRun() & 0xFF)
          goto end;
        g_BtlActionKind = 7;
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
        g_BtlTargetSlot = BtlPartyTargetPick(2);
        g_BtlActionNumber = 0x72;
        BtlActionRun();
        goto end;

      case 0x80:
        g_BtlActionNumber = 0x62;
        if (BtlActionRun() & 0xFF)
          goto end;
        g_BtlActionKind = 7;
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
        g_BtlActionNumber = 0x63;
        BtlActionRun();
        goto end;

      case 0x81:
        n = 0;
        for (i = 0; i < 4; i++)
        {
          if (g_BtlSlotActiveFlags[i])
          {
            live[n] = i;
            n++;
          }
        }
        g_BtlTargetSlot = rng_list_pick((s32) live, n, 3);
        g_BtlActionNumber = 0x68;
        if (BtlActionRun() & 0xFF)
          goto end;
        if (((BtlStats *)CharStats)[7].hp)
          goto attack;
        g_BtlActionKind = 7;
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
        g_BtlActionNumber = 0x67;
        BtlActionRun();
        goto end;

      case 0x82:
        g_BtlTargetSlot = BtlPartyTargetPick(2);
        g_BtlActionNumber = 0x73;
        if (BtlActionRun() & 0xFF)
          goto end;
        g_BtlActionKind = 7;
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = 7;
        g_BtlActionNumber = 0x65;
        BtlActionRun();
        goto end;

      case 0x83:
        g_BtlActionNumber = 0x66;
        if (BtlActionRun() & 0xFF)
          goto end;
        goto attack;
    }
    goto end;
  }

attack:
  g_BtlActionKind = 2;
  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
  g_BtlTargetSlot = BtlPartyTargetPick(2);
  if (BtlActionRun() & 0xFF)
    goto end;
  g_BtlActionKind = 2;
  g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
  g_BtlTargetSlot = BtlPartyTargetPick(2);
  BtlActionRun();

end:
  return BtlEndCheck() & 0xFF;
}

s32 BtlActionKindRun();                                /* extern */
s32 BtlActionAdjust();                                /* extern */
s32 BtlActionOutcomeClear();                                  /* extern */
s32 BtlActingRestAnimationSet();                                  /* extern */
s32 BtlPartyPanelDisp();                                  /* extern */
s32 BtlPartyTurnActionSet();                                  /* extern */

/* func_8002E088 */
s32 BtlActionRun(void) {
    s32 end_state;

    BtlActionOutcomeClear();
    g_BtlFallDone = 0;
    if (((u8) g_BtlActingSlot < 5U) && (g_BtlTargetSlot == 0xFE)) {
        BtlPartyTurnActionSet();
    }
    if (!(BtlActionAdjust() & 0xFF) || (end_state = BtlActionKindRun() & 0xFF, (end_state == 0))) {
        BtlPartyPanelDisp();
        *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = 0;
        BtlActingRestAnimationSet();
        end_state = 0;
    }
    return end_state;
}

s32 BtlEndCheck();

/* func_8002E12C */
s32 BtlActionKindRun()
{
  unsigned int fail_draw;
  switch (g_BtlActionKind - 2)
  {
    case 0:
      if ((g_BtlActingSlot >= 6) && (*((u8 *) (((g_BtlActingSlot * 0x3C) + CharStats) + 0x1C))))
    {
      BtlActionRowRun();
    }
    else
    {
      BtlActionAttackRun();
    }
      break;

    case 1:
      BtlActionStringDisp();
      BtlActionSpellRun();
      break;

    case 2:
      BtlActionStringDisp();
      BtlActionItemRun();
      break;

    case 3:
      BtlActionWalkRun();
      BtlFacingNearestSet();
      return 0;

    case 4:
      fail_draw = 0x60;
      if (g_BtlActingSlot < 5)
    {
      if ((g_BtlNumber >= 0xDF) || ((rng_7f_draw() & 0xFF) >= fail_draw))
      {
        BtlEscapeFailMove(g_BtlActingSlot);
        return 0;
      }
    }
      return BtlSlotLeave() & 0xFF;

    case 5:
      BtlActionRowRun();
      break;

    case 6:

    case 7:
      break;

    case 8:
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0x1D);
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      BtlActingRestAnimationSet();
      break;

  }

  BtlActionStringClose();
  BtlItem83Revive();
  return BtlEndCheck() & 0xFF;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80011A88_pad);
u8 BtlSlotNearestFind(s32);

/* func_8002E2FC */
void BtlFacingNearestSet(void)
{
  u8 who;
  u8 *self;
  u8 *them;

  /* one call per arm: the target's single jal is the three cross-jumped */
  if (g_BtlActingStatus & 0x40)
  {
    who = BtlSlotNearestFind(0);
  }
  else if (g_BtlActingSlot < 5)
  {
    who = BtlSlotNearestFind(1);
  }
  else
  {
    who = BtlSlotNearestFind(2);
  }
  self = (u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords);
  them = (u8 *) ((who << 6) + g_BtlRecords);
  if (((BtlRecord *)self)->sprite.x < ((BtlRecord *)them)->sprite.x)
  {
    ((BtlRecord *)self)->sprite.attr = 0x20;
  }
  else
  {
    ((BtlRecord *)self)->sprite.attr = 0x2020;
  }
}

/* far_syms: g_BtlPartyMoveAnimations (target reaches it %hi/%lo) */
extern u8 g_BtlPartyMoveAnimations[];
s32 BtlEnemyEscapeMove();
s32 BtlSlot5EscapeMove();
s32 BtlEndCheck();  /* TU declares this below */
s32 BtlEndMessageDisp(s32);
s32 SoundPlay(s32, s32);

/* func_8002E398 */
s32 BtlSlotLeave(void)
{
  s16 off_x;
  s32 end_state;
  void *party_record;
  s32 records;
  void *other_record;
  void *party_frame_record;
  void *other_frame_record;
  SoundPlay(0x26, 3);
  if (((u8) g_BtlActingSlot) < 5U)
  {
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), *(g_BtlActingSlot + g_BtlPartyMoveAnimations));
    party_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    ((BtlRecord *)party_record)->sprite.attr = 0x20;
    if (((BtlRecord *)party_record)->sprite.x < 0x158)
    {
      loop_2:
      party_frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;

      ((BtlRecord *)party_frame_record)->sprite.x = (u16) (((BtlRecord *)party_frame_record)->sprite.x + 4);
      BtlFrameStep();
      if ((*((s16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) + 0x6))) < 0x158)
      {
        goto loop_2;
      }
    }
  }
  else
  {
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 1U);
    other_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    ((BtlRecord *)other_record)->sprite.attr = 0x2020;
    off_x = ((s32) (((BtlRecord *)other_record)->width)) * (-1);
    if (off_x < ((BtlRecord *)other_record)->sprite.x)
    {
      do
      {
        other_frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;
        ((BtlRecord *)other_frame_record)->sprite.x = (u16) (((BtlRecord *)other_frame_record)->sprite.x - 4);
        BtlFrameStep();
      }
      while (off_x < (*((s16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) + 0x6))));
    }
  }
  records = g_BtlRecords;
  *(g_BtlActingSlot + g_BtlSlotIds) = (g_BtlSlotActiveFlags[g_BtlActingSlot] = (*((u16 *) ((g_BtlActingSlot << 6) + records)) = 0));
  end_state = BtlEndCheck(records) & 0xFF;
  if (end_state != 2)
  {
    SoundPlay(0, 3);
    return end_state;
  }
  if (g_BtlActingSlot)
  {
    BtlEndMessageDisp(3);
    BtlSlot5EscapeMove();
    BtlEnemyEscapeMove();
    BtlEndMessageDisp(0);
    SoundPlay(0, 3);
    return 3;
  }
  else
  {
    BtlEndMessageDisp(3);
    BtlSlot5EscapeMove();
    BtlEnemyEscapeMove();
    BtlEndMessageDisp(0);
    SoundPlay(0, 3);
    return 3;
  }
}

/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
extern BtlWalkBlock g_BtlWalkBlock;

/* func_8002E5D4 */
void BtlActionWalkRun(void)
{
  BtlWalkBlock *bl_D_800E4B10;
  s16 place_record[0x20];
  unsigned short walk_animation;
  place_record[0x1F] = 8;
  place_record[0x1E] = 8;
  place_record[3] = (u16) (g_BtlActionNumber * 8);
  place_record[4] = (u16) (g_BtlTargetSlot * 8);
  place_record[5] = (u16) (g_BtlTargetSlot * 8);
  BtlWalkSet();
  bl_D_800E4B10 = &g_BtlWalkBlock;
  bl_D_800E4B10->target_width_counts = 0;
  bl_D_800E4B10->target = (s32) (&place_record[0]);
  if (g_BtlActingSlot < 5)
  {
    walk_animation = g_BtlPartyMoveAnimations[g_BtlActingSlot];
    goto common;
    bl_D_800E4B10->animation = walk_animation;
  }
  if (g_BtlSlotIds[g_BtlActingSlot] != 0x12)
  {
    if (g_BtlActionNumber)
    {
      walk_animation = 1;
      goto common;
    }
    else
    {
      walk_animation = 1;
      goto common;
    }
  }
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 7);
  BtlAnimationEndWait((void *) (g_BtlRecords + (g_BtlActingSlot << 6)));
  bl_D_800E4B10->animation = 1;
  BtlWalkRun();
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
  BtlAnimationEndWait((void *) (g_BtlRecords + (g_BtlActingSlot << 6)));
  return;
  common:
  bl_D_800E4B10->animation = walk_animation;
  BtlWalkRun();
}

/* far_syms: g_BtlSpellRowsByNumber (target reaches it %hi/%lo) */
extern u8 g_BtlSpellRowsByNumber[];

/* func_8002E718 */
void BtlActionSpellRun(void)
{
    u8 *ptr;
    u8 result;

    ptr = g_BtlSpellRowsByNumber + g_BtlActionNumber * 12;
    BtlActionRowRun();
    g_BtlTargetSlot = g_BtlActingSlot;
    result = BtlTargetTraitCheck(0, 0x1B, 0, 0);
    if (result == 1)
    {
        *(u16 *)(CharStats + g_BtlActingSlot * 60 + 4) = *(u16 *)(CharStats + g_BtlActingSlot * 60 + 4) - (ptr[4] + 1) / 2;
    }
    else
    {
        *(u16 *)(CharStats + g_BtlActingSlot * 60 + 4) = *(u16 *)(CharStats + g_BtlActingSlot * 60 + 4) - ptr[4];
    }
}

/* func_8002E7FC */
void BtlActionItemRun(void)
{
  u8 item;
  u8 index;
  u32 to_index;
  u8 from_index;
  BtlActionRowRun();
  if (g_BtlActionNumber < 0x9C)
  {
    return;
  }
  item = g_BtlActionNumber;
  index = 0;
  while (index < 6)
  {
    if ((*((u8 *) (((CharStats + (g_BtlActingSlot * 60)) + index) + 0x2E))) == item)
    {
      break;
    }
    index++;

  }

  to_index = index;
  if (to_index != 5)
  {
    from_index = index + 1;
    if (to_index < 5)
    {
    do
    {
      index++;
      item = from_index;
      *((u8 *) (((CharStats + (g_BtlActingSlot * 60)) + to_index) + 0x2E)) = *((u8 *) (((CharStats + (g_BtlActingSlot * 60)) + item) + 0x2E));
      to_index = index;
      from_index++;
    }
    while (to_index < 5);
    }
  }
  *((u8 *) ((CharStats + (g_BtlActingSlot * 60)) + 0x33)) = 0;
}
/* far_syms: g_BtlActingStatus g_BtlSlotActiveFlags */
extern u8 g_BtlPartyNotHeldFlags[];
s32 BtlActionStatus40Adjust(void);  /* TU defines this below */
s32 BtlActionTargetAdjust(void);  /* TU defines this below */

/* May the current actor (g_BtlActingSlot) act, and if so start its act.

   Returns 0 without doing anything when the actor's active flag
   g_BtlSlotActiveFlags[actor] is clear, when its action kind g_BtlActionKind is 0xB,
   when it is a party slot (< 5) whose g_BtlPartyNotHeldFlags flag is clear, or when
   its status byte g_BtlActingStatus has 0x04 or 0x08.  Otherwise it runs
   BtlActionStatus40Adjust for a confused actor (status 0x40) or BtlActionTargetAdjust for
   any other, and returns 1.  Its one caller, BtlActionRun, reads the
   low byte of the result. */
/* func_8002E914 */
s32 BtlActionAdjust(void)
{
  if (g_BtlSlotActiveFlags[g_BtlActingSlot] == 0)
  {
    return 0;
  }
  if (g_BtlActionKind == 0xB)
  {
    return 0;
  }
  if (g_BtlActingSlot < 5 && g_BtlPartyNotHeldFlags[g_BtlActingSlot] == 0)
  {
    return 0;
  }
  if (g_BtlActingStatus & 0xC)
  {
    return 0;
  }
  if (g_BtlActingStatus & 0x40)
  {
    BtlActionStatus40Adjust();
  }
  else
  {
    BtlActionTargetAdjust();
  }
  return 1;
}

/* far_syms: g_BtlItemTable g_BtlSpellRows g_BtlActingSlot g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber */
extern u8 g_BtlItemTable[];
extern u8 g_BtlSpellRows[];
u8 BtlSlotNearestFind(s32);

/* func_8002E9CC */
s32 BtlActionStatus40Adjust(void)
{
    if (g_BtlActingSlot < 5)
    {
        if ((u8)rng_7f_draw() < 8)
        {
            g_BtlActionKind = 0xA;
            return;
        }
    }
    switch (g_BtlActionKind)
    {
        case 2:
            g_BtlTargetSlot = BtlSlotNearestFind(0);
            return;
        case 3:
            g_BtlActionNumber = BtlActingSpellPick();
            if (g_BtlActionNumber != 0)
            {
                if (g_BtlSpellRows[g_BtlActionNumber * 12 - 0xB] & 0xF)
                {
                    g_BtlTargetSlot = BtlSlotActivePick(1);
                    return;
                }
                return;
            }
            g_BtlActionKind = 2;
            g_BtlTargetSlot = BtlSlotNearestFind(0);
            return;
        case 4:
            g_BtlActionNumber = BtlActingItemPick();
            if (g_BtlActionNumber != 0xFF)
            {
                u8 *p;

                /* CODEGEN FORCING DEVICE (spelling), not recovered source.
                   `p -= -(x)` is exactly `p += x`; the original
                   spelling is unknown. Revert to `p += ...` if it turns up.
                   item record (id - 1), +6: its target nibble. Spelled as
                   a subtraction of the negated offset because that is what
                   keeps the table base FIRST in the addu: cse leaves a MINUS
                   alone (not commutative) and combine folds (minus p (neg x))
                   into (plus p x) without re-canonicalizing; every PLUS
                   spelling is put offset-first by cse (the JP 2.7.2 twin has
                   the same base-first add, the spell lookup above does not). */
                p = g_BtlItemTable;
                p -= -(g_BtlActionNumber * 18);
                if (p[-0xC] & 0xF)
                {
                    g_BtlTargetSlot = BtlSlotActivePick(0);
                    return;
                }
                return;
            }
            g_BtlActionKind = 2;
            g_BtlActionNumber = 0;
            g_BtlTargetSlot = BtlSlotNearestFind(0);
            return;
        case 7:
            BtlEnemyActionFallbackSet();
            return;
        case 5:
            BtlActionWalkCornerSet(0);
            return;
        case 6:
            return;
    }
}

/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
s32 BtlEndCheck();
u8 BtlSlotNearestFind(s32);

/* func_8002EB78 */
unsigned int BtlTargetModeAdjust(u8 mode)
{
  u8 unsuited;
  u8 var;
  var = mode & 0xF;
  unsuited = 0;
  if (var == 3)
  {
    if (*(g_BtlTargetSlot + g_BtlSlotActiveFlags))
    {
      g_BtlTargetSlot = BtlTargetByModeGet(3);
    }
    return;
  }
  if (var == 4)
  {
    if (*(g_BtlTargetSlot + g_BtlSlotIds))
    {
      if (*((u8 *) ((((u8 *) CharStats) + (g_BtlTargetSlot * 60)) + 0x15)))
      {
        return;
      }
    }
    g_BtlTargetSlot = BtlTargetByModeGet(4);
    return;
  }
  if ((*(g_BtlTargetSlot + g_BtlSlotActiveFlags)) == 0)
  {
    g_BtlTargetSlot = BtlTargetByModeGet(var);
    return;
  }
  switch (var)
  {
    case 5:
      if ((*((u16 *) (((u8 *) CharStats) + (g_BtlTargetSlot * 60)))) == (*((u16 *) ((((u8 *) CharStats) + (g_BtlTargetSlot * 60)) + 2))))
    {
      unsuited = 1;
    }
      break;

    case 6:
      if ((*((u16 *) ((((u8 *) CharStats) + (g_BtlTargetSlot * 60)) + 4))) == (*((u16 *) ((((u8 *) CharStats) + (g_BtlTargetSlot * 60)) + 6))))
    {
      unsuited = 1;
    }
      break;

    case 7:
      if (*((u8 *) ((((u8 *) CharStats) + (g_BtlTargetSlot * 60)) + 0x15)))
    {
      unsuited = 1;
    }
      break;

    case 0:
      return;

  }

  if (unsuited)
  {
    g_BtlTargetSlot = BtlTargetByModeGet(var);
  }
}
/* far_syms: g_BtlActingSlot g_BtlNumber g_BtlActionKind g_BtlActionNumber g_BtlTargetSlot g_BtlActingStatus CharStats */
extern u8 g_BtlItemRowsByNumber[];
extern u8 g_BtlAttackRows[];

/* func_8002ED34 */
s32 BtlActionTargetAdjust(void)
{
    u8 i;
    u16 mp;
    u8 *spell;
    u8 *item;
    u8 *row;
    u8 *rec;
    u8 cls;

    if (g_BtlActingSlot >= 6 && g_BtlNumber >= 0xEB && (BtlActionSpecialAdjust() & 0xFF))
    {
        return;
    }
    switch (g_BtlActionKind)
    {
    case 3:
        i = g_BtlTargetSlot;
        mp = *(u16 *)(g_BtlActingSlot * 0x3C + CharStats + 4);
        g_BtlTargetSlot = g_BtlActingSlot;
        spell = g_BtlSpellRowsByNumber + g_BtlActionNumber * 12;
        if ((BtlTargetTraitCheck(0, 0x1B, 0, 0) & 0xFF) == 1)
        {
            mp = mp * 2 + 1;
        }
        g_BtlTargetSlot = i;
        if (mp < spell[4] || (g_BtlActingStatus & 0x80) || i == 0x64)
        {
            BtlActionFallbackSet();
            return;
        }
        if (i == 0xFF)
        {
            g_BtlTargetSlot = BtlTargetByModeGet(spell[1]);
            return;
        }
        if ((g_BtlActingStatus & 0x20) && spell[0xB] != 0)
        {
            BtlActionFallbackSet();
            return;
        }
        BtlTargetModeAdjust(spell[1]);
        return;
    case 4:
        if (g_BtlTargetSlot == 0x64)
        {
            BtlActionFallbackSet();
            return;
        }
        i = 0;
        if (g_BtlActionNumber >= 0x9C)
        {
            rec = (u8 *)(g_BtlActingSlot * 0x3C + CharStats);
            for (; i < 6; i++)
            {
                if (*(i + rec + 0x2E) == g_BtlActionNumber)
                {
                    break;
                }
            }
        }
        if (i >= 6)
        {
            BtlActionFallbackSet();
            return;
        }
        item = g_BtlItemRowsByNumber + g_BtlActionNumber * 18;
        if (g_BtlTargetSlot == 0xFF)
        {
            g_BtlTargetSlot = BtlTargetByModeGet(item[6]);
            return;
        }
        BtlTargetModeAdjust(item[6]);
        return;
    case 5:
        if (g_BtlActingSlot < 6)
        {
            if (g_BtlTargetSlot == 0xFF)
            {
                BtlActionWalkCornerSet(2);
            }
            return;
        }
        BtlActionWalkCornerSet(1);
        return;
    case 2:
        if (g_BtlActingSlot < 5)
        {
            if (g_BtlTargetSlot == 0xFF || g_BtlTargetSlot == 0x64)
            {
                g_BtlTargetSlot = BtlSlotNearestFind(1);
            }
            return;
        }
        rec = (u8 *)(g_BtlActingSlot * 0x3C + CharStats);
        cls = rec[0x1C];
        if (cls == 0)
        {
            if (g_BtlTargetSlot >= 6)
            {
                g_BtlTargetSlot = BtlSlotNearestFind(2);
                return;
            }
            switch (rec[0x1A])
            {
            case 2:
            case 7:
            case 0xC:
            case 0x11:
            case 0x1A:
                g_BtlTargetSlot = BtlSlotNearestFind(2);
                break;
            }
            return;
        }
        g_BtlActionNumber = cls;
    case 7:
        if (g_BtlActionKind == 7)
        {
            if (g_BtlActingStatus != 0)
            {
                BtlActionFallbackSet();
                return;
            }
            if (g_BtlSlotIds[g_BtlActingSlot] == 0x4F)
            {
                return;
            }
        }
        row = g_BtlAttackRows + g_BtlActionNumber * 8;
        switch (row[0])
        {
        case 7:
            for (i = 0; i < 5; i++)
            {
                if (g_BtlSlotActiveFlags[i] != 0 && *(u8 *)(i * 0x3C + CharStats + 0x15) == 0)
                {
                    break;
                }
            }
            if (i == 5)
            {
                BtlEnemyActionFallbackSet();
                return;
            }
            if (g_BtlTargetSlot < 0xE && g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0
                && *(u8 *)(g_BtlTargetSlot * 0x3C + CharStats + 0x15) == 0)
            {
                return;
            }
            g_BtlTargetSlot = BtlTargetByModeGet(row[0]);
            return;
        case 2:
            if (g_BtlTargetSlot < 0xE && g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0)
            {
                return;
            }
            g_BtlTargetSlot = BtlTargetByModeGet(row[0]);
            return;
        }
        return;
    }
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80011AE0_pad);
void BtlEnemy6FActionSet(s32);                                 /* extern */
s32 BtlEnemy71ActionSet(s32);                                 /* extern */
s32 BtlEnemy73ActionSet(s32);                                 /* extern */
s32 BtlEnemy74ActionSet(s32);                                 /* extern */
s32 BtlEnemy75ActionSet(s32);                                 /* extern */
s32 BtlEnemy77ActionSet(s32);                                 /* extern */
s32 BtlEnemy78ActionSet(s32);                                 /* extern */
s32 BtlEnemy79ActionSet(s32);                                 /* extern */
s32 BtlEnemy7AActionSet(s32);                                 /* extern */
s32 BtlEnemy7BActionSet(s32);                                 /* extern */

/* func_8002F29C */
s32 BtlActionSpecialAdjust(void) {
    u8 id;

    id = *(g_BtlActingSlot + g_BtlSlotIds);
    switch (id) {
    case 0x6F:
        BtlEnemy6FActionSet(0);
        return 1;
    case 0x71:
        BtlEnemy71ActionSet(0);
        return 1;
    case 0x73:
        BtlEnemy73ActionSet(0);
        return 1;
    case 0x74:
        BtlEnemy74ActionSet(0);
        return 1;
    case 0x75:
        BtlEnemy75ActionSet(0);
        return 1;
    case 0x77:
        BtlEnemy77ActionSet(0);
        return 1;
    case 0x78:
        BtlEnemy78ActionSet(0);
        return 1;
    case 0x79:
        BtlEnemy79ActionSet(0);
        return 1;
    case 0x7A:
        BtlEnemy7AActionSet(0);
        return 1;
    case 0x7B:
        BtlEnemy7BActionSet(0);
        return 1;
    case 0x7C:
        return 1;
    default:
        return 0;
    }
}
/* far_syms: g_BtlActingStatus (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

/* func_8002F3A0 */
void BtlActionFallbackSet(void)
{
    u8 *rec;
    u8 i;
    u8 n;

    if (g_BtlActingSlot < 5)
    {
        rec = (u8 *)(g_BtlActingSlot * 60 + CharStats);
        if (*(u16 *)rec >= (((BtlStats *)rec)->hp_max >> 3))
        {
            g_BtlActionKind = 2;
            g_BtlTargetSlot = BtlSlotNearestFind(1);
            return;
        }
        n = 0;
        for (i = 0; i < 5; i++)
        {
            if (g_BtlSlotActiveFlags[i] != 0 && (*(u8 *)(CharStats + i * 60 + 0x15) & 0x3C) == 0)
            {
                n++;
            }
        }
        if (n >= 2)
        {
            BtlActionWalkCornerSet(2);
            return;
        }
        g_BtlActionKind = 2;
        g_BtlTargetSlot = BtlSlotNearestFind(1);
        return;
    }
    g_BtlActionKind = 2;
    g_BtlTargetSlot = BtlSlotNearestFind(2);
}
/* far_syms: g_BtlPartyMode CharStats g_BtlSlotActiveFlags g_BtlTurnOrder g_BtlSlotIds g_BtlSlotPlannedKinds */
s32 rng_mask_below();
s32 memset();

/* func_8002F4B0 */
void BtlTurnOrderMake(void)
{
    u16 key[14];
    u8 i;
    u8 j;
    u16 top;
    u16 speed;
    u8 slot;

    memset(key, 0, sizeof(key));
    for (i = 0; i < 14; i++)
    {
        g_BtlTurnOrder[i] = 0;
    }

    i = 6;
    if (g_BtlPartyMode != 2)
    {
        i = 0;
    }
    top = 0;
    for (; i < 14; i++)
    {
        if (g_BtlSlotActiveFlags[i] != 0)
        {
            if ((*(u16 *)(CharStats + i * 60 + 0x14) & 0xC00) == 0)
            {
                speed = *(u16 *)(CharStats + i * 60 + 0xC);
                key[i] = speed;
                if (top < speed)
                {
                    top = speed;
                }
            }
        }
    }

    top = top / 10;
    for (i = 0; i < 14; i++)
    {
        if (g_BtlSlotActiveFlags[i] != 0)
        {
            key[i] += rng_mask_below(0x3FF, top);
            g_BtlTurnOrder[i] = i + 1;
            if (g_BtlSlotIds[i] == 0x1F)
            {
                key[i] = 0xFFFF;
                g_BtlSlotPlannedKinds[i] = 6;
            }
        }
    }

    for (i = 0; i < 13; i++)
    {
        for (j = i + 1; j < 14; j++)
        {
            if (key[j] > key[i])
            {
                slot = g_BtlTurnOrder[i];
                g_BtlTurnOrder[i] = g_BtlTurnOrder[j];
                g_BtlTurnOrder[j] = slot;
                top = key[i];
                key[i] = key[j];
                key[j] = top;
            }
        }
    }

    for (i = 0; i < 14; i++)
    {
    }
}
