/* Lunar SSSC — btl_weapon: the usable weapons' shows 0x3E to 0x42 (0x3D
   ends btl_item.c). The first split had it in save.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlRecords */
/* far_syms: g_BtlRecords */
/* far_syms: g_BtlActuatorFFList (target reaches it %hi/%lo) */
/* far_syms: g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlRecords g_BtlTargetSlot */

#include "include_asm.h"
#include "psyq_types.h"
#include "path.h"
#include "btl.h"
/* A display block (0x40) as BtlAnimationFramesCount reads it: +0x2C the frame table
   (u16 pairs, +2 = a frame's duration), +0x34 the tick in the frame, +0x36
   the frame's duration, +0x38 the frame index, +0x3A the frame count
   (bit 0x8000 a flag). */
extern u8 g_BtlTargetSlot;
extern s32 g_BtlRecords;
extern s32 g_BtlShowRecords;
extern u8 g_BtlActuatorFFList[];
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;

extern BtlShowSound g_BtlShowSounds[];

void BtlEffectCopyAboveShotRun(u16);                                /* extern */

/* func_80086038 */
void BtlShow3ERun(void) {
    g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    g_BtlShowSounds[0].sound = 0x6E;
    g_BtlShowSounds[1].sound = 0x51;
    g_BtlShowSounds[2].sound = 0x5A;
    g_BtlShowSounds[2].repeat = 0;
    g_BtlShowSounds[2].count = 0;
    g_BtlShowSounds[1].repeat = 0;
    g_BtlShowSounds[1].count = 0;
    g_BtlShowSounds[0].repeat = 0;
    g_BtlShowSounds[0].count = 0;
    BtlEffectCopyAboveShotRun(0xB1);
}

extern u8 g_BtlActuatorFFList[];
extern u8 D_800AE2AC[];
s32 BtlEffectCopyTargetPlay(s32, s32, s32);                           /* extern */
s32 BtlAmountShowRun();                                  /* extern */
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;

/* func_800860B4 */
void BtlShow3FRun(void) {
    g_PadActuatorList = (s32 *)D_800AE2AC;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlEffectCopyTargetPlay(0xB5, 0x6D, 0);
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlAmountShowRun();
}
/* func_8008612C */
void BtlShow40Run(void) {
    g_PadActuatorList = (s32 *)D_800AE2AC;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlEffectCopyTargetPlay(0xB1, 0x71, 0);
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlAmountShowRun();
}
/* func_800861A4 */
void BtlShow41Run(void) {
    g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    g_BtlShowSounds[0].sound = 0x7B;
    g_BtlShowSounds[1].sound = 0x79;
    g_BtlShowSounds[2].sound = 0x7A;
    g_BtlShowSounds[2].repeat = 0;
    g_BtlShowSounds[2].count = 0;
    g_BtlShowSounds[1].repeat = 0;
    g_BtlShowSounds[1].count = 0;
    g_BtlShowSounds[0].count = 0;
    g_BtlShowSounds[0].repeat = 0;
    BtlEffectCopyAboveShotRun(0xB5);
}
/* far_syms: g_PadActuatorListWait */
/* far_syms: g_PadActuatorList */
/* far_syms: g_PadActuatorRestFlags */
extern u8 g_BtlSlotIds[];
s32 BtlShowActingPlay(s32, s32);
s32 BtlShowActingEnd();
s32 BtlAnimationSetCopy(s32, s32);
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlAnimationFramesCount(s32 blk);  /* extern */
void BtlMixColNumberSet(u16);
void layer_mix_switch_on(s32);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_mix_fade_check(s32 layer_mask);  /* TU declares this below */
s32 BtlShowFrameStep();
s32 BtlShowSoundStop(s32);
void layer_mix_switch_off(s32);

/* func_80086220 */
void BtlShow42Run(void)
{
  BtlRecord *disp;
  BtlRecord *p1;
  s32 a;
  s32 b;
  s32 u;
  s16 dy;
  s16 w;
  s16 y;
  s16 i;
  s16 n;
  u16 sx;
  BtlShowActingPlay(0x20, 0);
  BtlShowActingEnd();
  u = ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].height >> 2;
  a = -u;
  if (a >= -0xF)
  {
    a = -0x10;
  }
  b = -u;
  dy = a;
  if (b >= -0xF)
  {
    b = -0x10;
  }
  if (b < 0)
  {
    dy = -a;
  }
  ((BtlRecord *) g_BtlShowRecords)[20].sprite.flags = 0x2200;
  disp = (BtlRecord *) g_BtlShowRecords;
  p1 = &disp[1];
  *(BtlRecord *) p1 = *(BtlRecord *) &disp[20];
  *(BtlRecord *) disp = *(BtlRecord *) p1;
  ((BtlRecord *) g_BtlShowRecords)[0].on = ((BtlRecord *) g_BtlShowRecords)[1].on = 1;
  ((BtlRecord *) g_BtlShowRecords)[0].sprite.x = (s16) (((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x - 0x2A) - dy;
  ((BtlRecord *) g_BtlShowRecords)[1].sprite.x = dy + (s16) (((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x + 0x2A);
  w = ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.y - dy;
  ((BtlRecord *) g_BtlShowRecords)[1].sprite.y = w;
  ((BtlRecord *) g_BtlShowRecords)[0].sprite.y = w;
  y = ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.y + 1;
  ((BtlRecord *) g_BtlShowRecords)[0].sprite.attr = 0x2020;
  ((BtlRecord *) g_BtlShowRecords)[1].sprite.attr = 0x20;
  ((BtlRecord *) g_BtlShowRecords)[1].sprite.depth = y;
  ((BtlRecord *) g_BtlShowRecords)[0].sprite.depth = y;
  BtlShowSoundSet(1, 0x2E, 0, 0);
  i = 0;
  BtlAnimationSetCopy(g_BtlShowRecords, 0);
  BtlAnimationSetCopy(g_BtlShowRecords + 0x40, 0);
  sx = ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x;
  n = BtlAnimationFramesCount(g_BtlShowRecords);
  while (i < n)
  {
    if (g_BtlSlotIds[g_BtlTargetSlot] != 0x77)
    {
      if (i & 1)
      {
        ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x = sx - 2;
      }
      else
      {
        ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x = sx + 2;
      }
    }
    if (!(i & 7))
    {
      ((BtlRecord *) g_BtlShowRecords)[0].sprite.x += 1;
      ((BtlRecord *) g_BtlShowRecords)[1].sprite.x -= 1;
    }
    BtlShowFrameStep();
    i++;
  }
  BtlMixColNumberSet(0x97);
  layer_mix_switch_on(0x100);
  layer_mix_fade_start(0x400, 1, 8, 0x14);
  ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags = (((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags & 0xE3FF) | 0x800;
  BtlShowSoundStop(0);
  BtlShowSoundSet(1, 0x5D, 0, 0);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlAnimationSetCopy(g_BtlShowRecords, 1);
  BtlAnimationSetCopy(g_BtlShowRecords + 0x40, 1);
  i = 0;
  n = BtlAnimationFramesCount(g_BtlShowRecords);
  while (i < n)
  {
    if (i < 8)
    {
      ((BtlRecord *) g_BtlShowRecords)[0].sprite.x += 4;
      ((BtlRecord *) g_BtlShowRecords)[1].sprite.x -= 4;
    }
    if (g_BtlSlotIds[g_BtlTargetSlot] != 0x77)
    {
      if (i & 1)
      {
        ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x = sx - 2;
      }
      else
      {
        ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x = sx + 2;
      }
    }
    BtlShowFrameStep();
    i++;
  }
  ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x = sx;
  while (layer_mix_fade_check(0x400) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_mix_fade_start(0x400, 1, 8, -1);
  while (layer_mix_fade_check(0x400) & 0xFF)
  {
    BtlShowFrameStep();
  }
  ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags &= 0xE3FF;
  layer_mix_switch_off(0x100);
  BtlAmountShowRun();
}
