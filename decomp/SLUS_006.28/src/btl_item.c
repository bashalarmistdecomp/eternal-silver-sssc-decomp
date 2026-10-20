/* Lunar SSSC — btl_item: the items' shows 0x30 to 0x3D: the restoring and
   cleansing items, the ranged weapons' shots (0x3A to 0x3C, with
   BtlShowRecordTravelRun) and the first usable weapon (0x3D), with the
   effect copies they share. The first split had it in menu.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "path.h"
#include "mth.h"
#include "map.h"
#include "btl.h"
/* The request BtlTravelRun plays: pan display block `disp` from one record
   to another; +0x14/+0x18 are path tables the display module builds. */
/* The battle records (BtlRecord, btl.h) are indexed as a struct here: a
   store to one is a struct access, which GCC 2.8's alias rule keeps apart
   from fixed-address scalars like g_BtlShowRecords. */
/* The 0x3C-byte field effect records at g_MenuUseShowRecords (six slots; MenuUseShowPlacesPlay). */
/* BtlTargetSpriteMixRun's 30-entry u16 table, copied from rodata g_BtlTargetSpriteMixCols (only
   2-aligned, so the block copy checks the source alignment at run time). */
void MenuUseShowPlacesPlay(u16, s32);
extern u8 g_BtlActuatorFFList[];
extern s32 g_BtlShowRecords;
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern u8 g_BtlActingSlot;
extern s32 g_BtlRecords; /* far_syms: g_BtlRecords */
extern u8 g_BtlSlotFallMarks[];
extern u8 g_BtlTargetSlot;
s32 BtlShowActingEnd();                                  /* extern */

s32 BtlShowFrameStep();
s32 BtlShowActingPlay(s32, s32);
void BtlActingShowPlacesRun(s32 palette, s32 animation);
void BtlEffectCopyTargetStandRun();
void BtlShow34Run(void);
void BtlTargetSpriteMixRun(s32);
void BtlShow35Run(void);
void BtlShowRecordTravelRun(s32, s32, s32);
void BtlShow3ARun(void);
void BtlShow3BRun(void);
void BtlShow3CRun(void);
extern u8 g_BtlRunning;
s32 BtlShowFrameStep();
extern u8 g_BtlRunning;
/* far_syms: g_BtlRunning */
s32 BtlShowAnimationPlay(s32, s32, s32);
s32 BtlShowSoundSet(s32, s32, s32, s32);
extern BtlShowSound g_BtlShowSounds[];
s32 BtlShowSoundSet(s32, s32, s32, s32);
void BtlTravelRun(BtlTravelBlock *, u16);
/* far_syms: g_BtlActuatorFFList */
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlShowAnimationPlay(s32, s32, s32);
s32 BtlRecordPlaceCopy(s32, s16 *);
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlShowAnimationPlay(s32, s32, s32);
s32 BtlRecordPlaceCopy(s32, s16 *);
s32 BtlRecordPlaceCopy(s32, s16 *);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_mix_fade_check(s32);
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
void BtlEffectCopyTargetPlay(u16, u8, s16);
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlShowAnimationPlay(s32, s32, s32);
/* far_syms: g_BtlTargetSlot */
/* far_syms: g_BtlRecords g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags */
/* far_syms: g_PadActuatorListWait */
/* far_syms: g_PadActuatorList */
/* far_syms: g_PadActuatorRestFlags */
void BtlTravelRun(BtlTravelBlock *, u16);
s32 BtlShowActingPlay(s32, s32);
void BtlActingShowPlacesRun(s32 palette, s32 animation);
void BtlEffectCopyTargetStandRun();
void BtlShow34Run(void);
void BtlTargetSpriteMixRun(s32);
void BtlShow35Run(void);
void BtlShowRecordTravelRun(s32, s32, s32);
void BtlShow3ARun(void);
void BtlShow3BRun(void);
void BtlShow3CRun(void);
void BtlRecordSpriteDataSet(s32, u8 *, u16, u16);
void layer_mix_switch_on(s32);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_mix_fade_check(s32);
/* far_syms: g_BtlActingSlot */
s32 BtlShowAnimationPlay(s32, s32, s32);
/* far_syms: g_BtlRecords g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
void BtlEffectCopyTargetStandRun(void);  /* extern */

/* func_80085818 */
void BtlShow30Run(void)
{
    if (g_BtlRunning != 0) {
        BtlActingShowPlacesRun(0x780, 6);
    } else {
        MenuUseShowPlacesPlay(0x780, 6);
    }
}
/* func_8008585C */
void BtlShow31Run(void)
{
    if (g_BtlRunning != 0) {
        BtlActingShowPlacesRun(0x780, 7);
    } else {
        MenuUseShowPlacesPlay(0x780, 7);
    }
}
/* func_800858A0 */
void BtlShow32Run(void)
{
    if (g_BtlRunning != 0) {
        BtlActingShowPlacesRun(0x790, 6);
    } else {
        MenuUseShowPlacesPlay(0x790, 6);
    }
}
/* func_800858E4 */
void BtlShow33Run(void)
{
    if (g_BtlRunning != 0) {
        BtlActingShowPlacesRun(0x790, 7);
    } else {
        MenuUseShowPlacesPlay(0x790, 7);
    }
}

/* func_80085928 */
void BtlShow34Run(void) {
    if (*(g_BtlTargetSlot + g_BtlSlotFallMarks) == 0) {
        BtlShowActingPlay(0x10, 0);
        BtlShowActingEnd();
    }
    BtlEffectCopyTargetStandRun();
}

/* func_8008597C */
void BtlShow35Run(void) {
    BtlShowActingPlay(0x10, 0);
    BtlTargetSpriteMixRun(0);
}
extern u8 g_SpriteData2[];
void BtlRecordSpriteDataSet(s32, u8 *, u16, u16);
void BtlTravelRun(BtlTravelBlock *, u16);

/* func_800859A8 */
void BtlShowRecordTravelRun(s32 palette, s32 animation, s32 sound)
{
  BtlTravelBlock req;
  u8 *actor;
  u8 *target;
  s32 h;
  s32 rec;
  s32 disp;
  BtlRecordSpriteDataSet(g_BtlShowRecords, g_SpriteData2, palette, animation);
  if (*(u16 *) (((g_BtlActingSlot << 6) + g_BtlRecords) + 0xC) & 0x2000)
  {
    ((BtlRecord *)g_BtlShowRecords)->sprite.attr = 0x2000;
  }
  else
  {
    ((BtlRecord *)g_BtlShowRecords)->sprite.attr = 0;
  }
  rec = g_BtlRecords + (g_BtlActingSlot << 6);
  disp = g_BtlShowRecords;
  *(s16 *) disp = 1;
  BtlRecordPlaceCopy(rec, disp);
  BtlShowSoundSet(0xB, (u8) sound, 0, 0);
  req.step = 8;
  req.mode = 0;
  req.from = g_BtlShowRecords;
  actor = (u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords);
  req.from_dx = ((BtlRecord *)actor)->width >> 1;
  req.from_dy = -0xD;
  target = (u8 *) (g_BtlRecords + (g_BtlTargetSlot << 6));
  req.to = (s32) target;
  req.to_dx = ((BtlRecord *)target)->width >> 2;
  h = -(((BtlRecord *)target)->height >> 2);
  if (h >= -0xF)
  {
    h = -0x10;
  }
  req.to_dy = h;
  BtlTravelRun(&req, animation);
}

/* func_80085AFC */
void BtlShow3ARun(void) {
    BtlShowRecordTravelRun(0x7F0, 2, 0x3C);
}
/* func_80085B24 */
void BtlShow3BRun(void) {
    BtlShowRecordTravelRun(0x790, 3, 0x4F);
}
/* func_80085B4C */
void BtlShow3CRun(void) {
    BtlShowRecordTravelRun(0x790, 4, 0x4F);
}
void BtlMixColNumberSet(u16);
void layer_mix_switch_on(s32);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_mix_fade_check(s32);
void layer_mix_switch_off(s32);

/* func_80085B74 */
void BtlEffectCopyTargetPlay(u16 effect, u8 elem, s16 dy)
{
  u8 *target;
  s32 disp;
  BtlShowActingPlay(0x20, 0);
  *(BtlRecord *) g_BtlShowRecords = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  target = (u8 *) (g_BtlRecords + (g_BtlTargetSlot << 6));
  disp = g_BtlShowRecords;
  *(s16 *) disp = 1;
  ((BtlRecord *)disp)->sprite.flags = 0x2200;
  BtlRecordPlaceCopy((s32) target, (s16 *) disp);
  AS(u16, ((BtlRecord *)g_BtlShowRecords)->sprite.y) += dy;
  BtlMixColNumberSet(effect);
  layer_mix_switch_on(0x100);
  layer_mix_fade_start(0x400, 1, 8, 0x14);
  *(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xE) = (*(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xE) & 0xE3FF) | 0x800;
  BtlShowSoundSet(1, elem, 0, 0);
  BtlShowAnimationPlay(g_BtlShowRecords, 0, 0);
  while (layer_mix_fade_check(0x400) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_mix_fade_start(0x400, 1, 8, -1);
  while (layer_mix_fade_check(0x400) & 0xFF)
  {
    BtlShowFrameStep();
  }
  *(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xE) &= 0xE3FF;
  layer_mix_switch_off(0x100);
}
void BtlAmountShowRun(void);

/* func_80085D2C */
void BtlEffectCopyAboveShotRun(u16 effect)
{
  BtlTravelBlock req;
  s32 dy;
  s32 lift;
  BtlShowActingPlay(0x20, 0);
  BtlShowActingEnd();
  *(BtlRecord *) g_BtlShowRecords = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  ((BtlRecord *) g_BtlShowRecords)->sprite.flags = 0x2200;
  ((BtlRecord *) g_BtlShowRecords)->on = 1;
  lift = *(u16 *) ((u8 *) &((BtlRecord *) g_BtlRecords)[g_BtlActingSlot] + 0x3E) + 0x10;
  ((BtlRecord *) g_BtlShowRecords)->sprite.y = ((BtlRecord *) g_BtlRecords)[g_BtlActingSlot].sprite.y - lift;
  ((BtlRecord *) g_BtlShowRecords)->sprite.depth = ((BtlRecord *) g_BtlRecords)[g_BtlActingSlot].sprite.depth + 1;
  g_BtlShowSounds[0].state = 1;
  BtlShowAnimationPlay(g_BtlShowRecords, 0, 0);
  g_BtlShowSounds[1].state = 1;
  ((BtlRecord *) g_BtlShowRecords)->sprite.y = ((BtlRecord *) g_BtlRecords)[g_BtlActingSlot].sprite.y;
  req.mode = 0;
  req.step = 0x10;
  req.from = g_BtlShowRecords;
  req.from_dx = 0;
  req.from_dy = -(*(u16 *) ((u8 *) &((BtlRecord *) g_BtlRecords)[g_BtlActingSlot] + 0x3E) + 0x10);
  req.to = (s32) &((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot];
  req.to_dx = 0;
  dy = -(*(u16 *) ((u8 *) &((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot] + 0x3E) >> 2);
  if (dy >= -0xF)
  {
    dy = -0x10;
  }
  req.to_dy = dy;
  BtlTravelRun(&req, 1);
  BtlMixColNumberSet(effect);
  layer_mix_switch_on(0x100);
  layer_mix_fade_start(0x400, 1, 8, 0x14);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags = (((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags & 0xE3FF) | 0x800;
  ((BtlRecord *) g_BtlShowRecords)->sprite.depth = ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.depth + 1;
  g_BtlShowSounds[2].state = 1;
  BtlShowAnimationPlay(g_BtlShowRecords, 2, 0);
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

/* func_80085FC0 */
void BtlShow3DRun(void)
{
  s32 dy;
  s32 quarter_height_then_dy;
  g_PadActuatorListWait = 0;
  g_PadActuatorList = g_BtlActuatorFFList;
  quarter_height_then_dy = (s32) (((u16) (*((s16 *) (((u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords)) + 0x3e)))) >> 2);
  g_PadActuatorRestFlags = 0x3F;
  dy = -quarter_height_then_dy;
  if (dy >= (-0xF))
  {
    dy = -0x10;
  }
  if (quarter_height_then_dy)
  {
    quarter_height_then_dy = dy;
    BtlEffectCopyTargetPlay(0x97, 0x68, quarter_height_then_dy);
  }
  else
  {
    quarter_height_then_dy = dy;
    BtlEffectCopyTargetPlay(0x97, 0x68, quarter_height_then_dy);
  }
}
