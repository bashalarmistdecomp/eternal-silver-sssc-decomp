/* Lunar SSSC — btl_run: the battle from its set-up to its end message
   (BtlRun) and what its rounds and its end call: slot 6's animation, the
   0x4F place mark, slot 5's revive try, the party's stand, escape try and
   move, the stats stored back and the win. The first split had it in
   rendering2.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "tile.h"
#include "pad.h"
#include "palette.h"
#include "chr.h"
#include "btl.h"
#include "sprite.h"
extern s32 g_MusicWaitFrameKind;
extern u8 g_BtlTargetSlot;
extern u8 g_ChrLevelUpGains[];
extern s32 g_ChrRecords;
extern u8 g_BtlNumber;
extern s32 CharStats;
extern u8 g_BtlSlotIds[];
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlActingSlot;
s32 screen_disp_mask_set(s32);                                 /* extern */
extern long HWD0;
extern long VWD0;
void ChrLevelGainAdd(u16 *stat, s32 gain);
extern long HWD0, VWD0;
void sprite_run_off(s32, s32);
void mem_clear(void *destination, u32 byte_count);  /* extern */
void BtlAnimationSet(void *record, s32 animation);  /* extern */

/* far_syms: g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_PadActuatorLevels g_PadActuator1Level g_BtlRunning */
/* la_absolute: BtlRun */
void BtlFrameWait(s32);
extern s32 g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern s16 g_PadActuatorListWait;
extern u8 g_PadActuatorLevels;
extern u8 g_PadActuator1Level;
extern u8 g_BtlRunning;
extern u8 g_BtlPartyMode;
extern u8 g_BtlAutoEndAllowed;
extern u8 g_BtlRoundsLeft;
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotRestAnimations[];
extern u8 g_BtlPartyStandAnimations[5];
void BtlInit(void);
void cd_wait_btl_frame_set(s32);
void BtlSlot6AnimationAlternate(void);  /* TU defines this below */
void BtlSlot5ReviveTry(void);
void BtlRoundStatusUpdate(void);
s32 BtlEndCheck(void);
void BtlEnemyActionSet(void);
void BtlEnemyActionAdjust(void);
void BtlEnemyActionAnimationSet(void);
void BtlEnemy4FPlaceMark(void);
void BtlEnemyActionSpecialSet(void);
void BtlPartyPanelDisp(void);
void BtlMenuRun(void);
void BtlWindow1IconDisp(void);
void BtlPartyActionSet(s32);
s32 BtlPartyEscapeTry(void);  /* TU defines this below */
void BtlEscapeFailMove(s32);
void BtlPartyActionAdjust(void);
void BtlPartyActionAnimationSet(void);
s32 BtlRoundTurnsRun(void);
void BtlPartyStandAnimationSet(void);
void BtlWindow1IconClose(void);
void SoundPlay(s32, s32);
void MusicPlay(s32, s32, s32);
void BtlEndMessageDisp(s32);
void BtlPadWait(s32, s32);
void BtlWinRun(u8 *);
void BtlPartyStatsStore(u8 *);
void BtlClose(void);

/* func_80027434 */
s32 BtlRun(u8 btl_number)
{
    u8 party[5];
    u8 i;
    u16 result;
    u8 r;

    i = 0;
    result = 0;
    g_PadActuatorList = 0;
    g_PadActuatorRestFlags = 0;
    g_PadActuatorListWait = 0;
    g_PadActuatorLevels = 0;
    g_PadActuator1Level = 0;
    g_BtlRunning = 1;
    g_BtlPartyMode = 0;
    g_BtlNumber = btl_number;
    for (; i < 5; i++)
    {
        party[i] = g_BtlSlotIds[i];
    }
    screen_disp_mask_set(0);
    BtlInit();
    screen_disp_mask_set(1);
    cd_wait_btl_frame_set(1);
    if (g_BtlNumber == 0xF6)
    {
        BtlSlot6AnimationAlternate();
    }
    while (result == 0)
    {
        if (g_BtlNumber != 0xF9)
        {
            BtlSlot5ReviveTry();
        }
        BtlRoundStatusUpdate();
        result = BtlEndCheck() & 0xFF;
        if (result != 0)
        {
            break;
        }
        if (g_BtlNumber < 0xEB)
        {
            BtlEnemyActionSet();
            for (i = 6; i < 14; i++)
            {
                if (g_BtlSlotActiveFlags[i] != 0 && g_BtlSlotPlannedKinds[i] == 7 && g_BtlSlotPlannedTargets[i] >= 5)
                {
                    while (1)
                    {
                    }
                }
            }
            BtlEnemyActionAdjust();
            BtlEnemyActionAnimationSet();
            BtlEnemy4FPlaceMark();
        }
        else
        {
            BtlEnemyActionSpecialSet();
            result = BtlEndCheck() & 0xFF;
            if (result != 0)
            {
                break;
            }
        }
        BtlPartyPanelDisp();
        if (g_BtlPartyMode != 1)
        {
            BtlPartyPanelDisp();
            g_BtlAutoEndAllowed = 0;
            BtlMenuRun();
            if (g_BtlPartyMode == 1)
            {
                BtlWindow1IconDisp();
            }
            g_BtlAutoEndAllowed = 1;
        }
        else
        {
            BtlPartyActionSet(1);
        }
        if (g_BtlPartyMode == 2)
        {
            r = BtlPartyEscapeTry();
            if (r == 2)
            {
                result = 3;
                break;
            }
            if (r == 0)
            {
                BtlEscapeFailMove(0xFF);
            }
            for (g_BtlActingSlot = 0; g_BtlActingSlot < 5; g_BtlActingSlot++)
            {
                if (g_BtlSlotActiveFlags[g_BtlActingSlot] != 0 && ((BtlStats *) CharStats)[g_BtlActingSlot].status == 0)
                {
                    g_BtlSlotRestAnimations[g_BtlActingSlot] = g_BtlPartyStandAnimations[g_BtlActingSlot];
                }
            }
            BtlPartyActionAdjust();
        }
        else
        {
            BtlPartyActionAdjust();
            BtlPartyActionAnimationSet();
        }
        result = BtlRoundTurnsRun() & 0xFF;
        BtlPartyStandAnimationSet();
        if (g_BtlRoundsLeft != 0xFF)
        {
            if (--g_BtlRoundsLeft == 0)
            {
                result = 4;
                break;
            }
        }
    }
    if (g_BtlPartyMode == 1)
    {
        BtlWindow1IconClose();
    }
    g_MusicWaitFrameKind = 2;
    g_BtlAutoEndAllowed = 0;
    SoundPlay(0, 3);
    switch (result)
    {
    case 1:
        MusicPlay((g_BtlNumber >= 0xEB) ? 0x3E : 0x3D, 0, 0);
        BtlEndMessageDisp(1);
        BtlPadWait(0x1E, 0x40);
        BtlEndMessageDisp(0);
        BtlWinRun(party);
        break;
    case 2:
        for (g_BtlActingSlot = 0; g_BtlActingSlot < 5; g_BtlActingSlot++)
        {
            if (party[g_BtlActingSlot] != 0 && g_BtlSlotIds[g_BtlActingSlot] == 0)
            {
                result = 3;
                break;
            }
        }
        MusicPlay(0x37, 0, 0);
        BtlEndMessageDisp((result != 2) ? 2 : 4);
        BtlPadWait(0x96, 0x40);
        BtlEndMessageDisp(0);
        break;
    case 4:
        result = 1;
        break;
    }
    cd_wait_btl_frame_set(0);
    BtlPartyStatsStore(party);
    g_MusicWaitFrameKind = 0;
    result--;
    BtlClose();
    g_BtlRunning = 0;
    g_PadActuatorList = 0;
    g_PadActuatorRestFlags = 0;
    g_PadActuatorListWait = 0;
    g_PadActuatorLevels = 0;
    g_PadActuator1Level = 0;
    return (u8) result;
}

extern s32 g_BtlRecords;

/* func_80027934 */
void BtlSlot6AnimationAlternate(void) {
    s32 animation;
    s32 frames_left;

    animation = 0x11;
    BtlFrameWait(0x1E);
    frames_left = 0x5B;
    while (1) {
        if (animation & 0xFFFF)
            animation = 0;
        else
            animation = 0x11;
        frames_left--;
        BtlAnimationSet(g_BtlRecords + 0x180, animation);
        BtlFrameStep();
        if (!(frames_left & 0xFF))
            break;
    }
}
extern u8 g_BtlSlotPlannedNumbers[];
extern s32 g_BtlExtraRecords;
extern u8 g_BtlExtraRecordCount;
s32 BtlPartyTargetPick(s32);
void sprite_run_off(s32, s32);

/* Enemy 0x4F's marked tile: every live enemy slot (6..13) holding enemy
   0x4F in state 7 jumps to the actor BtlPartyTargetPick(2) picks. A stack copy
   of its record takes the target's x/y (published through g_BtlExtraRecords),
   the tile column/row go to g_BtlSlotPlannedNumbers/g_BtlSlotPlannedTargets, BtlAnimationSet runs the
   copy's 0xF animation, and the real record faces the target (0x20 when
   it stands left of it, else 0x2020) before the 0x64 effect and sound
   0x1C. */
/* func_800279A0 */
void BtlEnemy4FPlaceMark(void)
{
    BtlRecord act;
    u8 who;

    g_BtlExtraRecords = (s32)&act;
    for (g_BtlActingSlot = 6; g_BtlActingSlot < 14; g_BtlActingSlot++) {
        if (g_BtlSlotActiveFlags[g_BtlActingSlot] != 0 && g_BtlSlotIds[g_BtlActingSlot] == 0x4F && g_BtlSlotPlannedKinds[g_BtlActingSlot] == 7) {
            who = BtlPartyTargetPick(2);
            act = ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot];
            act.sprite.x = ((BtlRecord *)g_BtlRecords)[who].sprite.x;
            act.sprite.y = ((BtlRecord *)g_BtlRecords)[who].sprite.y;
            g_BtlSlotPlannedNumbers[g_BtlActingSlot] = act.sprite.x / 8;
            g_BtlSlotPlannedTargets[g_BtlActingSlot] = act.sprite.y / 8;
            BtlAnimationSet(&act, 0xF);
            if (((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x < act.sprite.x) {
                ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.attr = 0x20;
            } else {
                ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.attr = 0x2020;
            }
            g_BtlExtraRecordCount = 1;
            BtlFrameWait(0x64);
            g_BtlExtraRecordCount = 0;
            sprite_run_off(0x1C, 1);
        }
    }
}
/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */
extern s8 g_BtlSlot5Roaming;
extern s8 g_BtlSlot5RoamAnimationState;
extern s8 g_BtlSlot5RoamXState;
extern s8 g_BtlSlot5RoamYState;
extern u8 g_BtlPartyNotHeldFlags[5];
extern BtlWalkBlock g_BtlWalkBlock;

/* la_absolute: BtlSlot5ReviveTry */
s32 rng_7f_draw(void);
s32 rng_mask_below(s32, s32);
void BtlSlot5Revive(s32);
s32 BtlWalkRun(void);

/* func_80027B6C */
void BtlSlot5ReviveTry(void)
{
  u8 buf[8];
  BtlRecord rec;
  u8 n;

  if (g_BtlSlotIds[5] == 0xC)
  {
    if ((u8)rng_7f_draw() < 0x1A)
    {
      n = 0;
      for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++)
      {
        buf[g_BtlTargetSlot] = 0;
        if (g_BtlPartyNotHeldFlags[g_BtlTargetSlot] != 0 && g_BtlSlotIds[g_BtlTargetSlot] != 0
            && *(u16 *)(g_BtlTargetSlot * 0x3C + CharStats) == 0)
        {
          buf[n] = g_BtlTargetSlot + 1;
          n++;
        }
      }
      if (n != 0)
      {
        g_BtlTargetSlot = rng_mask_below(3, n);
        g_BtlTargetSlot = buf[g_BtlTargetSlot] - 1;
        BtlSlot5Revive(1);
        rec.sprite.x = ((BtlRecord *)g_BtlRecords)[5].sprite.x;
        rec.sprite.y = 0x60;
        AS(s16, rec.height) = 8;
        AS(s16, rec.width) = 8;
        g_BtlWalkBlock.target = (s32)&rec;
        BtlWalkRun();
        BtlAnimationSet((void *)(g_BtlRecords + 0x140), 0);
        g_BtlSlot5RoamYState = 0;
        g_BtlSlot5RoamXState = 0;
        g_BtlSlot5RoamAnimationState = 0;
        g_BtlSlot5Roaming = 1;
      }
    }
  }
}

/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */
/* la_absolute: BtlPartyStandAnimationSet */

/* func_80027CE4 */
void BtlPartyStandAnimationSet(void)
{
  for (g_BtlActingSlot = 0; g_BtlActingSlot < 5; g_BtlActingSlot++)
  {
    if (g_BtlSlotActiveFlags[g_BtlActingSlot] != 0 && ((BtlStats *) CharStats)[g_BtlActingSlot].status == 0)
    {
      g_BtlSlotRestAnimations[g_BtlActingSlot] = g_BtlPartyStandAnimations[g_BtlActingSlot];
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), g_BtlSlotRestAnimations[g_BtlActingSlot]);
    }
  }
}

/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

/* func_80027DAC */
s32 BtlPartyEscapeTry(void)
{
  s32 held_count;

  if (g_BtlNumber >= 0xDF || (u8)rng_7f_draw() < 0x40)
  {
    return 0;
  }

  SoundPlay(0x26, 3);
  BtlPartyEscapeMove();
  g_BtlActingSlot = 0;
  held_count = 0;

  do
  {
    if (g_BtlSlotActiveFlags[g_BtlActingSlot])
    {
      if ((*((u8 *)((g_BtlActingSlot * 0x3C) + CharStats) + 0x15) & 0x4C) == 0)
      {
        g_BtlSlotIds[g_BtlActingSlot] = 0;
        g_BtlSlotActiveFlags[g_BtlActingSlot] = 0;
        *(s16 *)((g_BtlActingSlot << 6) + g_BtlRecords) = 0;
      }
      else
      {
        held_count++;
      }
    }
    g_BtlActingSlot++;
  } while ((u8)g_BtlActingSlot < 5);

  if (held_count & 0xFF)
  {
    SoundPlay(0, 3);
    return 1;
  }
  BtlEndMessageDisp(3);
  BtlSlot5EscapeMove();
  BtlEnemyEscapeMove();
  BtlEndMessageDisp(0);
  SoundPlay(0, 3);
  return 2;
}
/* la_absolute: BtlPartyEscapeMove */
extern u8 g_BtlPartyMoveAnimations[5];

/* func_80027EF0 */
void BtlPartyEscapeMove(void)
{
  u16 frame;
  u8 moving;

  for (g_BtlActingSlot = 0; g_BtlActingSlot < 5; g_BtlActingSlot++)
  {
    if (g_BtlSlotActiveFlags[g_BtlActingSlot] != 0
        && (*(u8 *)(g_BtlActingSlot * 0x3C + CharStats + 0x15) & 0x4C) == 0)
    {
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), g_BtlPartyMoveAnimations[g_BtlActingSlot]);
      *(s16 *)((g_BtlActingSlot << 6) + g_BtlRecords + 0xC) = 0x20;
    }
  }
  for (frame = 0; frame < 0x158; frame++)
  {
    moving = 0;
    for (g_BtlActingSlot = 0; g_BtlActingSlot < 5; g_BtlActingSlot++)
    {
      if (g_BtlSlotActiveFlags[g_BtlActingSlot] != 0
          && (*(u8 *)(g_BtlActingSlot * 0x3C + CharStats + 0x15) & 0x4C) == 0)
      {
        ((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x += 4;
        if (((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x < 0x158)
          moving = 1;
      }
    }
    if (!moving)
      break;
    BtlFrameStep();
  }
}
/* The persistent party record (0x40 bytes, indexed by character id, based at
   *g_ChrRecords) and the battle record (0x3C bytes, CharStats[slot]) — the
   layouts BtlPartyStatsSet (btl_init.c) fills one from the other with.
   BtlStats is that file's BtlStats; this TU's BtlStats (above
   BtlRun) names only `status`, the same byte as unk15 here. */

/* The battle's write-back, called once by the round loop's exit
   (BtlRun) with its party id list: for each of the five party slots
   whose id is non-zero, the persistent record of that id takes the slot's
   battle record's HP — 1 when the member ended the fight at 0, so nobody
   leaves a battle dead — its MP, and its six carried items. Nothing else
   goes back: max HP / MP, the stats, level, equipment and spells are the
   persistent record's own (BtlPartyStatsSet derives the battle copy from them,
   level-ups write them directly). */
/* func_80028074 */
void BtlPartyStatsStore(u8 *party)
{
    ChrRecord *rec;
    u8 i;
    u8 j;

    for (i = 0; i < 5; i++) {
        if (party[i] != 0) {
            rec = (ChrRecord *) g_ChrRecords;
            rec += party[i];
            if (((BtlStats *) CharStats)[i].hp != 0) {
                rec->hp = ((BtlStats *) CharStats)[i].hp;
            } else {
                rec->hp = 1;
            }
            rec->mp = ((BtlStats *) CharStats)[i].mp;
            for (j = 0; j < 6; j++) {
                rec->items[j] = ((BtlStats *) CharStats)[i].items[j];
            }
        }
    }
}

extern u8 g_BtlWinItems;
extern u8 D_800B8C01;
extern u8 D_800B8C02;
extern u8 D_800B8C03;
extern u8 D_800B8C04;
extern u8 D_800B8C05;
extern u8 D_800B8C06;
extern u8 D_800B8C07;
extern u32 g_BtlWinExperience;
void BtlSlot5Revive(s32);
void BtlWinRewardSum(void);
void BtlWinItemsGive(u8 *);
void BtlWinMessageDisp(void);
void ChrLevelUp(u8, u8 *);
void BtlLevelUpMessageDisp(void);

/* func_80028140 */
void BtlWinRun(u8 *party)
{
    u8 n;
    u8 i;
    u8 *rec;
    u32 left;
    u32 total;
    s32 off;

    n = 0;
    g_BtlWinItems = D_800B8C01 = D_800B8C02 = D_800B8C03 =
        D_800B8C04 = D_800B8C05 = D_800B8C06 = D_800B8C07 = 0;
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++)
    {
        if (g_BtlSlotIds[g_BtlTargetSlot] != 0)
        {
            i = *((u8 *) (g_BtlTargetSlot * 0x3C + CharStats) + 0x15);
            if (i != 0)
            {
                *((u8 *) (g_BtlTargetSlot * 0x3C + CharStats) + 0x1E) &= 0xFB;
                *((u8 *) (g_BtlTargetSlot * 0x3C + CharStats) + 0x15) = 0;
                g_BtlSlotActiveFlags[g_BtlTargetSlot] = 1;
            }
            else if (g_BtlSlotActiveFlags[g_BtlTargetSlot] == 0)
            {
                BtlSlot5Revive(0);
                n++;
            }
        }
    }
    if (n)
    {
        BtlFrameWait(0x14);
    }
    i = 0;
    BtlPartyPanelDisp();
    BtlWinRewardSum();
    BtlWinItemsGive(party);
    BtlWinMessageDisp();
    for (; i < 5; i++)
    {
        if (g_BtlSlotIds[i] != 0)
        {
            rec = (u8 *) g_ChrRecords;
            rec += g_BtlSlotIds[i] << 6;
            left = g_BtlWinExperience;
            while (left >= ((ChrRecord *)rec)->experience_next && ((ChrRecord *)rec)->level < 99)
            {
                left -= ((ChrRecord *)rec)->experience_next;
                mem_clear(g_ChrLevelUpGains, 0xC);
                g_ChrLevelUpGains[0] = g_BtlSlotIds[i];
                ChrLevelUp(party[i], rec);
                off = i * 0x3C;
                ChrLevelGainAdd((u16 *) (CharStats + off), g_ChrLevelUpGains[1]);
                ChrLevelGainAdd((u16 *) (CharStats + off) + 1, g_ChrLevelUpGains[1]);
                ChrLevelGainAdd((u16 *) (CharStats + off) + 2, g_ChrLevelUpGains[2]);
                ChrLevelGainAdd((u16 *) (CharStats + off) + 3, g_ChrLevelUpGains[2]);
                *((u8 *) (off + CharStats) + 0x14) += 1;
                SoundPlay(0x1F, 3);
                BtlPartyPanelDisp();
                BtlLevelUpMessageDisp();
            }
            if (((ChrRecord *)rec)->level >= 99)
            {
                ((ChrRecord *)rec)->experience_next = 0;
            }
            else
            {
                ((ChrRecord *)rec)->experience_next -= left;
            }
            total = ((ChrRecord *)rec)->experience + g_BtlWinExperience;
            ((ChrRecord *)rec)->experience = total;
            if (total > 0x98967F)
            {
                ((ChrRecord *)rec)->experience = 0x98967F;
            }
        }
    }
}
