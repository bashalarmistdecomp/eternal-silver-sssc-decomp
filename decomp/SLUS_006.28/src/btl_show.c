/* Lunar SSSC — btl_show: the effect shows: their open, voice wait, close
   and run by number (BtlShowNumberRun), the effect record's moves, copies,
   shots and places, and the spells' shows 0x01 to 0x2F (the show number is
   the spell's). The first split had it in menu.c. Every function is C.
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
/* g_BtlShowPulse: menu window state; +0 is set to 1 before a page's second
   play, +0xC is the window triple (g_BtlShowPulse.colour_1) layer_col_fade_start /
   layer_col_set take, +0x12 a second one. +4/+8 not read yet. */
extern u8 g_BtlActuatorFFTwiceList[];
extern BtlShowPulse g_BtlShowPulse;
/* pair_list_sort's sort element: a key and a tie-break sub. */
/* The 0x60-byte field entities (g_MapObjects records; g_MapPartyRecords points at
   one). Read as a struct, an entity field load is a struct access, which
   GCC 2.8's alias rule lets pass a store to a fixed-address global. */
/* The 0x3C-byte field effect records at g_MenuUseShowRecords (six slots; MenuUseShowPlacesPlay). */
/* BtlTargetSpriteMixRun's 30-entry u16 table, copied from rodata g_BtlTargetSpriteMixCols (only
   2-aligned, so the block copy checks the source alignment at run time). */
typedef struct { u16 v[30]; } Tbl30;
void BtlEffectRecordBackMove();
extern u8 g_BtlSlotShowPlaceFlags[];
extern u16 g_BtlSlotShowPlaceXs[];
extern u16 g_BtlSlotShowPlaceYs[];
extern u8 g_SpriteData0[];
void MenuUseShowPlacesPlay(u16, s32);
extern BtlEffectTileRow *g_BtlEffectTileRow;
extern u8 g_BtlActuatorToggleList[];
extern u8 g_BtlActuatorRandomList[];
extern u8 *g_BtlTilePatternList;
extern u8 *g_GameOptions;
extern u8 g_BtlActuatorRiseShortList[];
extern u8 g_BtlActuatorFFList[];
extern s32 g_BtlShowRecords;
extern s32 g_BtlShowWaveLines;
extern s32 g_BtlShowWaveBlock;
extern s32 g_MenuUseShowRecords;
extern s32 g_BtlShowOpen;
extern u8 g_BtlSlotAmountKinds[];
void BtlShowOpen(s32);
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern u8 g_BtlShowVoiceState;
extern u8 g_BtlActingSlot;
extern s32 g_BtlRecords; /* far_syms: g_BtlRecords */
extern u8 g_BtlSlotIds[];
extern u8 g_BtlTargetSlot;
s32 BtlShowRecordsClear();                                  /* extern */
s32 BtlShowActingEnd();                                  /* extern */

void *memcpy(void *, const void *, u32);  /* TU declares this below */
s32 BtlShowFrameStep();
void BtlShowVoiceWait(void);
s32 BtlEffectRecordPlay(s32);
void BtlShow04Run(void);
void BtlShowPlacesPlay(u16, u16);
s32 BtlShowActingPlay(s32, s32);
void BtlActingShowPlacesRun(s32 palette, s32 animation);
void BtlShow23Run(void);
s32 ratio_29_32_mul(s32 value);
s32 ratio_mul(s32, s32, s32);
s32 ratio_4_5_mul(s32 value);
void BtlEffectCopyTargetStandRun();
void BtlTargetSpriteMixRun(s32);

/* far_syms: CharStats */
/* far_syms: g_BtlSlotIds */
extern s32 g_BtlShowBlocks[];
extern u8 g_BtlRunning;
extern u8 g_BtlSlotActiveFlags[];
extern s32 CharStats;
extern u8 *g_BtlEffectRow;
void func_8001DC60(void);
s32 xmalloc_heap_alloc(s32);
s32 BtlShowPulseClear();
s32 BtlShowWaveSet(s32, s32, s32, s32, s32);
void BtlAmountRecordOff(u32);
s32 layer_col_set(s32, s32, s32);
void BtlFacingBothSet(void);
void BtlFacingTowardSet(u8 *, s16);
s32 BtlEffectLoad(s32, s32);
void VoiceSet();
void BtlActionStringDisp(void);

/* func_8007B7C8 */
void BtlShowOpen(s32 show)
{
  u32 i;
  u8 *map;
  u8 kind;
  u8 *sfx;
  s32 disp;
  g_BtlShowBlocks[0] = 0;
  g_BtlShowBlocks[2] = 0;
  g_BtlShowBlocks[1] = 0;
  if (g_BtlRunning != 0)
  {
    func_8001DC60();
    g_BtlShowRecords = xmalloc_heap_alloc(0x540);
    g_BtlShowWaveLines = xmalloc_heap_alloc(0x1910);
    g_BtlShowWaveBlock = xmalloc_heap_alloc(0x18);
    g_BtlShowOpen = 1;
    g_BtlShowVoiceState = 0;
    BtlShowPulseClear();
    BtlShowRecordsClear();
    BtlShowWaveSet(0, 1, 0, 1, 0);
    for (i = 0; i < 0xD; i++)
    {
      BtlAmountRecordOff(i);
    }
    layer_col_set(-1, 0x8013D, 0);
    layer_col_set(0, 0xC, 0);
    BtlShowFrameStep();
    if ((show & 0xFF) != 0x34)
    {
      if ((g_BtlActingSlot != g_BtlTargetSlot) && (g_BtlSlotAmountKinds[g_BtlTargetSlot] != 0))
      {
        if ((g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0)
            && !(*(u8 *) ((g_BtlTargetSlot * 0x3C) + CharStats + 0x15) & 0x3C))
        {
          BtlFacingBothSet();
        }
        else
        {
          BtlFacingTowardSet((u8 *) (g_BtlRecords + (g_BtlActingSlot << 6)),
                        *(s16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 6));
        }
        if ((g_BtlSlotIds[6] == 0x7C) && (((BtlStats *)CharStats)[7].hp != 0))
        {
          ((BtlRecord *)g_BtlRecords)[7].sprite.attr = ((BtlRecord *)g_BtlRecords)[6].sprite.attr;
        }
      }
    }
    map = g_BtlSlotIds;
    if (map[6] == 0x77)
    {
      ((BtlRecord *)g_BtlRecords)[6].sprite.attr &= 0x9FFF;
    }
    BtlEffectLoad(g_BtlShowRecords + 0x500, show & 0xFF);
    disp = g_BtlShowRecords;
    sfx = g_BtlEffectRow;
    ((BtlRecord *)disp)[20].sprite.flags = 0x2000;
    if (sfx[2] != 0)
    {
      kind = map[g_BtlActingSlot];
      if ((kind == 0xB) || (kind == 0x7C) || (kind == 0x7D))
      {
        VoiceSet(g_BtlEffectRow[3]);
      }
      else
      {
        VoiceSet(sfx[2]);
      }
    }
    BtlActionStringDisp();
  }
  else
  {
    g_MenuUseShowRecords = xmalloc_heap_alloc(0x168);
  }
}

s32 BtlShowFrameStep();                                  /* extern */

/* func_8007BA94 */
void BtlShowVoiceWait(void) {
    if (g_BtlShowVoiceState != 0) {
        do {
            BtlShowFrameStep();
        } while (g_BtlShowVoiceState != 0);
    }
}
s32 layer_col_set(s32, s32, s32);                           /* extern */
s32 layer_mix_set(s32, s32);                              /* extern */
s32 xmalloc_heap_free(s32);                               /* extern */
s32 sprite_mix_col_set_copy(s32);                                 /* extern */
extern u8 g_BtlRunning; /* far_syms: g_BtlRunning */

/* func_8007BACC */
void BtlShowClose(void) {
    if (g_BtlRunning != 0) {
        BtlShowActingEnd();
        BtlShowRecordsClear();
        sprite_mix_col_set_copy(0x7FFF);
        layer_col_set(-1, 0x8013D, 0);
        layer_mix_set(0x80F35, 0xFF);
        g_BtlShowOpen = 0;
        xmalloc_heap_free(g_BtlShowWaveBlock);
        xmalloc_heap_free(g_BtlShowWaveLines);
        xmalloc_heap_free(g_BtlShowRecords);
        g_BtlShowRecords = 0;
        g_BtlShowWaveLines = 0;
        g_BtlShowWaveBlock = 0;
    } else {
        xmalloc_heap_free(g_MenuUseShowRecords);
        g_MenuUseShowRecords = 0;
    }
    BtlShowVoiceWait();
}
extern u8 g_BtlShowFunctions[];
void BtlActingShowRun(s32);

/* func_8007BB80 */
void BtlShowNumberRun(s32 show)
{
  s32 show_byte;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `first_show` only holds the constant 1; behaviour is the same with the constant
     written where it is read, but then 3 of this function's 31 instruction lines differ. */
  int first_show;
  u8 *show_functions;
  show_byte = show & 0xFF;
  if (show_byte != 0)
  {
    first_show = 1;
    if (((u32) ((show + 0x69) & 0xFF)) < 0x67U)
    {
      BtlActingShowRun(show_byte);
      return;
    }
    BtlShowOpen(show_byte);
    show_functions = g_BtlShowFunctions;
    ((s32 (*)()) (*(s32 *)(((show_byte - first_show) * 4) + show_functions)))();
    BtlShowClose();
  }
}
s32 BtlShowAnimationPlay(s32, s32, s32);
s32 BtlShowSoundSet(s32, s32, s32, s32);
void BtlRecordSpotMove(s32, s32, s32, s32, s32);  /* TU declares this below */
s32 BtlFrameWait(s32);

/* func_8007BBFC */
void BtlEffectRecordMove(s32 animation, s16 frame_count, s16 spot_x, s16 spot_y)
{
  u8 id;
  s32 disp;
  id = animation;
  disp = g_BtlShowRecords;
  *((u16 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) = 0;
  ((BtlRecord *)disp)[20].on = 1;
  BtlShowAnimationPlay(disp + 0x500, id, 0);
  BtlShowSoundSet(8, 7, 0, 0);
  BtlRecordSpotMove(g_BtlShowRecords + 0x500, spot_x, spot_y, frame_count, (u8) (id + 1));
  BtlFacingTowardSet((u8 *) (g_BtlShowRecords + 0x500), 0xA0);
  BtlShowSoundSet(9, 0x37, 0, 5);
  BtlShowAnimationPlay(g_BtlShowRecords + 0x500, (short)(id + 2), 0);
  BtlFrameWait(8);
}
/* func_8007BD0C */
void BtlEffectRecordBackMove(animation, frame_count) u8 animation; s16 frame_count;
{
  void *effect_record;
  void *acting_record;
  void *goal_record;
  short n2;
  acting_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  effect_record = g_BtlShowRecords + 0x500;
  if ((((BtlRecord *)g_BtlShowRecords)[20].sprite.x != ((BtlRecord *)acting_record)->sprite.x) || (((BtlRecord *)g_BtlShowRecords)[20].sprite.y != ((BtlRecord *)acting_record)->sprite.y))
  {
    BtlShowAnimationPlay(effect_record, animation, 0);
    BtlShowSoundSet(1, 7, 0, 0);
    goal_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    BtlRecordSpotMove(g_BtlShowRecords + 0x500, (s16) (((BtlRecord *)goal_record)->sprite.x), (s16) (((BtlRecord *)goal_record)->sprite.y), (s16) (frame_count), (u8)(animation + 1));
    n2 = animation + 2;
    BtlShowAnimationPlay(g_BtlShowRecords + 0x500, n2, 0);
  }
}

extern u8 g_BtlActuatorC8List[];
extern BtlShowSound g_BtlShowSounds[];
s32 BtlShowSoundSet(s32, s32, s32, s32);
void BtlTravelRun(BtlTravelBlock *, u16);
s32 BtlAnimationSetCopy(s32, s32);
u16 BtlAnimationFramesCount(s32);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
s32 BtlAmountShowStart(s32);
s32 BtlAmountShowCheck();

/* func_8007BE00 */
void BtlShow01Run(void)
{
  s16 win[3];
  BtlTravelBlock req;
  s16 i;
  s16 n;
  s32 mode;
  u8 idx;
  u8 *rec;
  u8 *target;
  s32 base;
  s32 blk;
  u16 z;
  s32 d2;
  s32 disp;
  BtlShowSoundSet(1, 0x2A, 0, 0);
  idx = g_BtlActingSlot;
  mode = 0;
  if (g_BtlSlotIds[idx] == 0xB)
  {
    mode = 5;
  }
  rec = (u8 *) ((idx << 6) + (base = g_BtlRecords));
  disp = g_BtlShowRecords;
  blk = disp + 0x500;
  *(u16 *) rec = 0;
  *(s16 *) blk = 1;
  ((BtlRecord *)disp)[20].sprite.flags = 0x2200;
  req.mode = 1;
  req.step = 8;
  req.from = blk;
  z = ((BtlRecord *)rec)->width >> 1;
  req.from_dy = 0;
  target = (u8 *) (base + (g_BtlTargetSlot << 6));
  req.to = (s32) target;
  req.from_dx = z;
  req.to_dx = ((BtlRecord *)target)->width >> 1;
  req.to_dy = 0;
  BtlTravelRun(&req, mode);
  g_BtlShowVoiceState = 1;
  win[0] = win[1] = win[2] = 0xB4;
  d2 = g_BtlShowRecords;
  AS(u16, ((BtlRecord *)d2)[20].sprite.depth) += 1;
  BtlAnimationSetCopy(d2 + 0x500, mode + 1);
  BtlShowSoundSet(1, 0x3A, 1, 0);
  BtlShowSoundSet(2, 0x3B, 0xB, 0);
  BtlShowSoundSet(3, 0x39, 0x13, 0);
  BtlShowSoundSet(4, 0x3A, 0x1B, 0);
  BtlShowSoundSet(5, 0x39, 0x26, 0);
  BtlShowSoundSet(6, 0x3A, 0x2D, 0);
  BtlShowSoundSet(7, 0x3B, 0x38, 0);
  BtlShowSoundSet(8, 0x39, 0x41, 0);
  BtlShowSoundSet(9, 0x3A, 0x49, 0);
  BtlShowSoundSet(0xA, 0x39, 0x52, 0);
  i = 0;
  n = BtlAnimationFramesCount(g_BtlShowRecords + 0x500);
  if (n > 0)
  {
    do
    {
      if ((s16) (i % 13) == 0)
      {
        g_PadActuatorList = (s32 *) g_BtlActuatorC8List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
      }
      if (i == 0x49)
      {
        layer_col_set(0, 0xC, (s32) win);
        layer_col_fade_start(0, 0xC, 1, 0x28, 0);
        g_BtlShowSounds[2].state = 0;
        BtlAmountShowStart(1);
      }
      BtlShowFrameStep();
      i++;
    }
    while (i < n);
  }
  BtlEffectRecordBackMove((u8) (mode | 2), 0x38);
  BtlShowActingEnd();
  while (BtlAmountShowCheck() != 0)
  {
    BtlShowFrameStep();
  }
}

/* far_syms: g_BtlActuatorFFList */
s32 BtlAnimationSetCopy(s32, s32);
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlAmountShowStart(s32);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
s32 BtlShowAnimationPlay(s32, s32, s32);
s32 BtlAmountShowCheck();
void BtlEffectRecordBackMove();
void BtlRecordSpotMove(s32, s32, s32, s32, s32);  /* display-module, NOT libgpu SetDefDispEnv */

/* func_8007C110 */
void BtlShow02Run(void)
{
  s16 col[3];
  s16 i;
  unsigned int effect_record;
  s32 mode;
  u8 idx;
  s32 disp;
  idx = g_BtlActingSlot;
  disp = g_BtlShowRecords;
  effect_record = disp + 0x500;
  *((u16 *) ((idx << 6) + g_BtlRecords)) = 0;
  *((s16 *) effect_record) = 1;
  ((BtlRecord *)disp)[20].sprite.flags = 0x2200;
  mode = (g_BtlSlotIds[idx] == 0xB) * 4;
  BtlAnimationSetCopy(disp + 0x500, (u16) mode);
  BtlShowSoundSet(1, 7, 0, 0);
  i = 0;
  do
  {
    BtlShowFrameStep();
    i++;
  }
  while (i < 0x14);
  BtlRecordSpotMove(g_BtlShowRecords + 0x500, *((s16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 6)), *((s16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xA)), 0x35, 0xFF);
  g_BtlShowVoiceState = 1;
  BtlAmountShowStart(1);
  BtlShowSoundSet(1, 0x5C, 0, 0);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  col[0] = 0xB4;
  col[1] = 0x3C;
  col[2] = 0;
  layer_col_set(0, 0xC, col);
  layer_col_fade_start(0, 0xC, 1, 0x28, 0);
  BtlShowAnimationPlay(g_BtlShowRecords + 0x500, 0xFF, 0);
  while (BtlAmountShowCheck() != 0)
  {
    BtlShowFrameStep();
  }
  BtlEffectRecordBackMove((u8) (mode | 1), 0x38);
}
/* func_8007C2C4 */
void pair_list_sort(SortPair *e, u16 n, s16 dir)
{
  u16 i;
  s16 t;
  SortPair *p;
  if (dir == 1)
  {
    for (; n != 0; n--)
    {
      for (i = 0; i < n - 1; i++)
      {
        p = (SortPair *) ((i << 2) + (s32) e);
        if (p[0].key < p[1].key)
        {
          t = p[0].key;
          p[0].key = p[1].key;
          p[1].key = t;
          t = p[0].second;
          p[0].second = p[1].second;
          p[1].second = t;
        }
        else if (p[0].key == p[1].key)
        {
          if (p[0].second > p[1].second)
          {
            t = p[0].second;
            p[0].second = p[1].second;
            p[1].second = t;
          }
        }
      }
    }
  }
  else
  {
    for (; n != 0; n--)
    {
      for (i = 0; i < n - 1; i++)
      {
        p = (SortPair *) ((i << 2) + (s32) e);
        if (p[0].key > p[1].key)
        {
          t = p[0].key;
          p[0].key = p[1].key;
          p[1].key = t;
          t = p[0].second;
          p[0].second = p[1].second;
          p[1].second = t;
        }
        else if (p[0].key == p[1].key)
        {
          if (p[0].second > p[1].second)
          {
            t = p[0].second;
            p[0].second = p[1].second;
            p[1].second = t;
          }
        }
      }
    }
  }
}
s32 BtlRecordPlaceCopy(s32, s16 *);

/* BtlShow03Run: the battle's show of number 3 (entry 2 of the table
   BtlShowNumberRun calls). The template block (disp +0x500) sweeps across
   the screen 8 px a frame for 0x24 frames, from x 0x10 rightwards or 0x130
   leftwards; the targeted objects (g_BtlSlotAmountKinds) are sorted by leading edge
   (x -/+ half the +0x3C width, clamped to [0x10, 0x130], rounded to 8,
   pair_list_sort), and each frame the sweep reaches the next edge is a hit
   frame instead: window flash, sfx 0x39 on slot j, the template snaps to
   the target's y0. Then the impact, sfx 0x7B, and every target gets the
   template on page 5. BtlEffectRecordMove is called with no prototype in
   scope (x0 goes raw), and x0 is read as (s16) in the edge loop; both are
   what the bytes need. */
/* func_8007C450 */
void BtlShow03Run(void)
{
  SortPair e[14];
  s16 win[3];
  s16 i;
  s16 n;
  s16 j;
  s32 x0;
  s16 dir;
  s16 lim;
  s32 hit;
  s32 mode;
  s32 disp;
  s32 last;
  s32 idx;
  disp = g_BtlShowRecords;
  idx = g_BtlActingSlot;
  ((BtlRecord *)disp)[20].sprite.flags = 0x2200;
  mode = 0;
  if (g_BtlSlotIds[idx] == 0xB)
  {
    mode = 6;
  }
  x0 = 0x10;
  if (((BtlRecord *)disp)[20].sprite.x < 0xA0)
  {
    lim = 0x130;
    dir = -1;
  }
  else
  {
    x0 = 0x130;
    lim = 0x10;
    dir = 1;
  }
  ((void (*)()) BtlEffectRecordMove)(mode, 0x38, x0, 0x88);
  g_BtlShowVoiceState = 1;
  BtlAnimationSetCopy(g_BtlShowRecords + 0x500, mode + 3);
  win[0] = win[1] = win[2] = 0xA0;
  for (i = 0, n = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      if (dir == -1)
      {
        e[n].key = ((((BtlRecord *) g_BtlRecords)[i].sprite.x - (*(u16 *) ((i << 6) + g_BtlRecords + 0x3C) >> 1)) > (s16) x0) ? (((BtlRecord *) g_BtlRecords)[i].sprite.x - (*(u16 *) ((i << 6) + g_BtlRecords + 0x3C) >> 1)) : (s16) x0;
        e[n].key = (e[n].key < lim) ? e[n].key : lim;
      }
      else
      {
        e[n].key = ((((BtlRecord *) g_BtlRecords)[i].sprite.x + (*(u16 *) ((i << 6) + g_BtlRecords + 0x3C) >> 1) * dir) < (s16) x0) ? (((BtlRecord *) g_BtlRecords)[i].sprite.x + (*(u16 *) ((i << 6) + g_BtlRecords + 0x3C) >> 1) * dir) : (s16) x0;
        e[n].key = (e[n].key > lim) ? e[n].key : lim;
      }
      e[n].key = e[n].key / 8 * 8;
      e[n++].second = ((BtlRecord *) g_BtlRecords)[i].sprite.y;
    }
  }
  pair_list_sort(e, n, dir);
  i = 0;
  j = 0;
  hit = 0;
  do
  {
    if (j < n)
    {
      if (dir == -1)
      {
        if (((BtlRecord *)g_BtlShowRecords)[20].sprite.x >= e[j].key)
        {
          hit = 1;
        }
      }
      else
      {
        if (e[j].key >= ((BtlRecord *)g_BtlShowRecords)[20].sprite.x)
        {
          hit = 1;
        }
      }
      if (hit)
      {
        layer_col_set(0, 0xC, (s32) win);
        layer_col_fade_start(0, 0xC, 1, 2, 0);
        g_PadActuatorList = (s32 *) g_BtlActuatorC8List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        BtlShowSoundSet(j, 0x39, 0, 0);
        ((BtlRecord *)g_BtlShowRecords)[20].on = 1;
        ((BtlRecord *)g_BtlShowRecords)[20].sprite.y = e[j].second;
        hit = 0;
        ((BtlRecord *)g_BtlShowRecords)[20].sprite.depth = e[j++].second + 1;
      }
      else
      {
        goto advance;
      }
    }
    else
    {
    advance:
      disp = g_BtlShowRecords;
      ((BtlRecord *)disp)[20].on = i & 1;
      i++;
      ((BtlRecord *)disp)[20].sprite.y = 0x88;
      ((BtlRecord *)disp)[20].sprite.x -= dir * 8;
    }
    BtlShowFrameStep();
  }
  while (i < 0x24);
  BtlAmountShowStart(1);
  BtlShowSoundSet(0xA, 0x7B, 0, 0);
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
      BtlRecordPlaceCopy(g_BtlRecords + (i << 6), (s16 *) (g_BtlShowRecords + (i << 6)));
      BtlAnimationSetCopy(g_BtlShowRecords + (i << 6), 5);
      last = g_BtlShowRecords + (i << 6);
    }
  }
  BtlAnimationSetCopy(g_BtlShowRecords + 0x500, mode + 4);
  BtlShowAnimationPlay(last, mode + 5, 0);
  while (BtlAmountShowCheck() != 0)
  {
    BtlShowFrameStep();
  }
  BtlEffectRecordBackMove(mode, 0x38);
}

/* far_syms: g_BtlActuator96To42List (target reaches it %hi/%lo) */
extern u8 g_BtlActuator96To42List[];
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlShowAnimationPlay(s32, s32, s32);  /* TU declares this below */

/* func_8007C960 */
s32 BtlEffectRecordPlay(s32 palette)
{
  s32 id;
  s32 animation;
  g_PadActuatorList = g_BtlActuator96To42List;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  id = *(g_BtlActingSlot + g_BtlSlotIds);
  *((u16 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) = 0;
  animation = ((u32) (id ^ 0xB)) < 1U;
  ((BtlRecord *)g_BtlShowRecords)[20].on = 1;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = (s16) (palette | 0x2000);
  BtlShowSoundSet(1, 0x20, 0, 0);
  return BtlShowAnimationPlay(g_BtlShowRecords + 0x500, animation, 0);
}

s32 BtlEffectRecordPlay(s32);                                 /* extern */

/* func_8007CA10 */
void BtlShow04Run(void) {
    BtlEffectRecordPlay(0x200);
}
void BtlEffectCopyEnterRun(u16, s16 *, u8);  /* TU declares this below */
extern u8 g_BtlActuatorRiseList[];
s32 line_path_make(u16 *, s16, s16, s16, s32, s32);

/* func_8007CA30 */
void BtlEffectCopyEnterRun(u16 page, s16 *win, u8 sel)
{
  PathPoint *path;
  s16 i;
  s32 d0;
  s32 d1;
  s32 d2;
  s32 d3;
  s32 d4;
  d0 = g_BtlShowRecords;
  ((BtlRecord *) g_BtlRecords)[g_BtlActingSlot].on = 0;
  ((BtlRecord *)d0)[20].on = 1;
  ((BtlRecord *)d0)[20].sprite.flags = 0x200;
  BtlShowAnimationPlay(d0 + 0x500, sel, 0);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) win);
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  d1 = g_BtlShowRecords;
  ((BtlRecord *)d1)[5].on = 1;
  *(BtlRecord *) (d1 + 0x140) = *(BtlRecord *) (d1 + 0x500);
  d2 = g_BtlShowRecords;
  ((BtlRecord *)d2)[5].sprite.depth = 0x1000;
  ((BtlRecord *)d2)[5].sprite.attr = 0x20;
  BtlAnimationSetCopy(d2 + 0x140, (u8) page);
  path = (PathPoint *) xmalloc_heap_alloc(0x80);
  line_path_make((u16 *) path, 0x190, -0x50, 0xF0, 0x5C, 0x20);
  for (i = 0; i < 0x20; i++)
  {
    d3 = g_BtlShowRecords;
    ((BtlRecord *)d3)[5].on = i & 1;
    ((BtlRecord *)d3)[5].sprite.x = path[i].x;
    ((BtlRecord *)d3)[5].sprite.y = path[i].y;
    BtlShowFrameStep();
  }
  xmalloc_heap_free((s32) path);
  BtlShowAnimationPlay(g_BtlShowRecords + 0x140, page & 0xFF, 1);
  g_BtlShowVoiceState = 1;
  BtlShowAnimationPlay(g_BtlShowRecords + 0x140, (u8) page + 1, 1);
  for (i = 0; i < 0x20; i++)
  {
    ((BtlRecord *)g_BtlShowRecords)[5].sprite.on = i & 1;
    BtlShowFrameStep();
  }
  d4 = g_BtlShowRecords;
  ((BtlRecord *)d4)[5].sprite.on = 1;
  BtlShowAnimationPlay(d4 + 0x140, (u8) page + 2, 1);
  ((BtlRecord *)g_BtlShowRecords)[5].on = 0;
  BtlShowActingEnd();
}

s32 BtlRecordPlaceCopy(s32, s16 *);  /* TU declares this below */
s32 layer_col_fade_check(s32);

/* func_8007CC54 */
void BtlShow05Run(void)
{
  s16 win[3];
  u16 i;
  s32 blink;  /* never initialised in the original either: only its low bit is read */
  u8 *blk;
  s32 base;
  s16 on;
  win[0] = win[1] = win[2] = -0x3C;
  i = 0;
  if (g_BtlSlotIds[g_BtlActingSlot] == 0xB)
  {
    i = 5;
  }
  BtlEffectCopyEnterRun(1, win, (u8) (i));
  win[0] = win[1] = win[2] = 0x64;
  layer_col_set(0, 0xC, (s32) win);
  BtlShowSoundSet(1, 0x7D, 0x38, 0);
  BtlShowSoundSet(2, 0x7D, 0x50, 0);
  BtlShowSoundSet(3, 0x62, 0, 0);
  BtlAnimationSetCopy(g_BtlShowRecords + 0x500, 4);
  blk = 0;
  i = 0;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x200;
  do
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      blk = (u8 *) (g_BtlShowRecords + (i << 6));
      *(BtlRecord *) blk = *(BtlRecord *) (g_BtlShowRecords + 0x500);
      *(s16 *) blk = 1;
      BtlRecordPlaceCopy(g_BtlRecords + (i << 6), blk);
    }
    i++;
  }
  while (i < 0xE);
  BtlShowAnimationPlay((s32) blk, 4, 0);
  layer_col_fade_start(0, 0xC, 1, 0x60, 0);
  while (layer_col_fade_check(0) & 0xFF)
  {
    i = 0;
    base = g_BtlShowRecords;
    on = blink & 1;
    do
    {
      if (g_BtlSlotAmountKinds[i] != 0)
      {
        *(s16 *) ((i << 6) + base) = on;
      }
      i++;
    } while (i < 0xE);
    BtlShowFrameStep();
    blink++;
  }
  BtlShowRecordsClear();
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
}
extern s8 g_BtlShowShakeOffsets[];
extern s8 g_PadActuatorLevels;
extern u8 g_PadActuator1Level;
s32 rng_mask_below(s32, s32);
s32 BtlTileScrollMove(s32, s32, s8);
s32 rng_draw();

/* func_8007CEA4 */
void BtlShow06Run(void)
{
  s16 win[3];
  s16 frame;
  s16 k;
  s16 j;
  s16 idx;
  s16 x;
  s16 y;
  s16 bcnt;
  u8 burst;
  PathPoint *paths;
  u8 *obj;
  s32 blk;
  frame = 0;
  if (g_BtlSlotIds[g_BtlActingSlot] == 0xB)
  {
    frame = 6;
  }
  win[0] = -0x14;
  win[1] = -0x28;
  win[2] = -0x3C;
  BtlEffectCopyEnterRun(3, win, frame);
  win[0] = 0x50;
  win[1] = -0x14;
  win[2] = -0x3C;
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) win);
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.attr = 0x20;
  BtlAnimationSetCopy(g_BtlShowRecords + 0x500, 1);
  frame = 0;
  burst = 0;
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x5A, 1, 0x20);
  BtlShowSoundSet(2, 0x5A, 0x11, 0x20);
  BtlShowSoundSet(3, 0x55, 1, 0x28);
  paths = (PathPoint *) xmalloc_heap_alloc(0x200);
  do
  {
    for (k = 0; k < 0x10; k++)
    {
      j = frame - k * 8;
      if ((u16) j < 8)
      {
        if (j == 0)
        {
          idx = k / 2 + 6;
          if (((k & 1) == 0) && (g_BtlSlotAmountKinds[idx] != 0))
          {
            obj = (u8 *) ((idx << 6) + g_BtlRecords);
            x = AS(u16, ((BtlRecord *)obj)->sprite.x);
            y = AS(u16, ((BtlRecord *)obj)->sprite.y);
          }
          else
          {
            x = rng_mask_below(0xFF, 0xFF) + 0x20;
            y = rng_mask_below(0x3F, 0x3F) + 0x70;
          }
          line_path_make((u16 *) &paths[k * 8], x + 0x80, 0, x, y, 8);
          *(BtlRecord *) ((k << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
          ((BtlRecord *) g_BtlShowRecords)[k].on = 1;
          ((BtlRecord *) g_BtlShowRecords)[k].sprite.depth = y | 0x1000;
        }
        blk = (k << 6) + g_BtlShowRecords;
        ((BtlRecord *)blk)->sprite.x = paths[k * 8 + j].x;
        ((BtlRecord *)blk)->sprite.y = paths[k * 8 + j].y;
      }
      else if (j == 8)
      {
        BtlAnimationSetCopy(g_BtlShowRecords + (k << 6), 2);
        if (burst == 0)
        {
          burst = 1;
          bcnt = 0;
        }
      }
      else if (j == 0x38)
      {
        *(s16 *) ((k << 6) + g_BtlShowRecords) = 0;
      }
    }
    if (burst != 0)
    {
      BtlTileScrollMove(4, 0, g_BtlShowShakeOffsets[bcnt]);
      for (k = 0; k < 0xE; k++)
      {
        ((BtlRecord *) g_BtlRecords)[k].sprite.y -= g_BtlShowShakeOffsets[bcnt];
      }
      bcnt++;
      if (g_GameOptions[5] != 0)
      {
        g_PadActuator1Level += rng_draw() & 0x3F;
        g_PadActuatorLevels = rng_draw() != 0;
      }
      if (bcnt >= 3)
      {
        burst = 0;
      }
    }
    BtlShowFrameStep();
    frame++;
  }
  while (frame < 0xB0);
  xmalloc_heap_free((s32) paths);
  BtlAmountShowStart(1);
  g_BtlShowSounds[2].state = 0;
  g_BtlShowSounds[1].state = 0;
  BtlShowSoundSet(3, 0x55, 0, 0);
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  BtlShowRecordsClear();
  g_PadActuator1Level = 0;
  g_PadActuatorLevels = 0;
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
}

s32 layer_col_fade_start(s32, s32, s32, s32, s32);
s32 layer_col_fade_check(s32);
void BtlEffectCopyEnterRun(u16, s16 *, u8);
s32 BtlAmountShowStart(s32);
s32 BtlAmountShowCheck();
s32 BtlRecordPlaceCopy(s32, s16 *);
s32 BtlAnimationSetCopy(s32, s32);

/* func_8007D39C */
void BtlShow07Run(void)
{
  s16 sp[3];
  s16 *unused;
  s16 *show_record;
  s32 slot;
  s32 mode_then_slot;
  s32 length;
  u8 *unused2;
  mode_then_slot = 0;
  length = 0x14;
  if (g_BtlSlotIds[g_BtlActingSlot] == 0xB)
  {
    mode_then_slot = 5;
  }
  sp[1] = -0x3C;
  sp[0] = -0x3C;
  sp[2] = -length;
  BtlEffectCopyEnterRun(2, sp, mode_then_slot & 0xFF);
  BtlShowSoundSet(1, 0x22, 0, 0);
  sp[1] = 0x64;
  sp[0] = 0x64;
  sp[2] = 0x82;
  layer_col_set(0, 0xC, (s32) sp);
  layer_col_fade_start(0, 0xC, 1, 0xA0, 0);
  BtlAnimationSetCopy(g_BtlShowRecords + 0x500, 1);
  show_record = 0;
  mode_then_slot = 0;
  BtlAmountShowStart(1);
  do
  {
    slot = mode_then_slot & 0xFFFF;
    if ((*(slot + g_BtlSlotAmountKinds)) != 0)
    {
      show_record = g_BtlShowRecords + (slot << 6);
      *((BtlRecord *) show_record) = *((BtlRecord *) (g_BtlShowRecords + 0x500));
      *show_record = 1;
      BtlRecordPlaceCopy(g_BtlRecords + ((mode_then_slot & 0xFFFF) << 6), show_record);
    }
    mode_then_slot += 1;
  }
  while (((u32) (mode_then_slot & 0xFFFF)) < 0xEU);
  BtlShowAnimationPlay((s32) show_record, 1, 0);
  loop_9:
  if (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
    goto loop_9;
  }

  layer_col_fade_start(0, 0xC, 1, length, 0);
  BtlShowRecordsClear();
  loop_12:
  if ((mode_then_slot = BtlAmountShowCheck()) != 0)
  {
    block_14:
    BtlShowFrameStep();

    goto loop_12;
  }

  if (layer_col_fade_check(0) & 0xFF)
  {
    {
      BtlShowFrameStep();
      goto loop_12;
    }
  }
}
extern u16 g_BtlShowScales[];
s32 BtlTileScrollSet(s32, s32, s32);  /* TU declares this below */
s32 tile_semi_trans_set(s32);
s32 tile_empty_fill_set(s32);
s32 tile_col_grey_set(s32);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 tile_mix_fade_start(s32, s32, s32);
s32 layer_mix_fade_check(s32);
s32 tile_mix_fade_check();
s32 BtlTilePatternPut(s32, s32);
void tile_grid_clear_copy(s32, s32);
s32 BtlTileEnd();

/* func_8007D5A0 */
void BtlShow08Run(void)
{
  s16 win[3];
  s16 k;
  s16 t;
  s16 j;
  s16 f;
  s16 u;
  PathPoint *path;
  u8 *r1;
  u8 *r2;
  u8 *r3;
  u8 *r4;
  u16 v;
  s32 sel;
  s32 base;
  sel = g_BtlSlotIds[g_BtlActingSlot] == 0xB;
  win[0] = win[1] = win[2] = -0x32;
  BtlEffectCopyEnterRun(1, win, sel * 4);
  BtlTileScrollSet(8, 0, 0x28);
  tile_semi_trans_set(1);
  tile_empty_fill_set(1);
  tile_col_grey_set(0);
  layer_mix_fade_start(8, 1, 0x96, 0xF);
  tile_mix_fade_start(1, 0x96, 0xF);
  BtlShowSoundSet(1, 0x68, 1, 0x10);
  BtlShowSoundSet(2, 0x55, 1, 0x20);
  BtlShowSoundSet(3, 0x61, 1, 0);
  k = 0;
  t = 0;
  while ((layer_mix_fade_check(8) & 0xFF) || (tile_mix_fade_check() != 0))
  {
    if (t++ & 1)
    {
      j = 0;
      base = g_BtlShowRecords;
      for (; j < 8; j++)
      {
        *(u16 *) ((j << 6) + base + 0x18A) += 1;
      }
    }
    else
    {
      j = 0;
      base = g_BtlShowRecords;
      for (; j < 8; j++)
      {
        *(u16 *) ((j << 6) + base + 0x18A) -= 1;
      }
    }
    if (k >= 8)
    {
      k = 0;
    }
    BtlTilePatternPut(g_BtlTilePatternList[k++], 1);
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
  path = (PathPoint *) xmalloc_heap_alloc(0x280);
  t = 0;
  u = 0;
  for (; t < 0x64; t++)
  {
    for (j = 0; j < 8; j++)
    {
      if (g_BtlSlotAmountKinds[j + 6] == 1)
      {
        f = t - j * 10;
        if ((u16) f < 0x14)
        {
          if (f == 0)
          {
            if (g_GameOptions[5] != 0)
            {
              g_PadActuator1Level = ((8 - j) * 0xFF) / 8;
              g_PadActuatorLevels = j < 2;
            }
            r1 = (u8 *) ((j << 6) + g_BtlRecords);
            ((BtlRecord *)r1)[6].sprite.scaled = 1;
            ((BtlRecord *)r1)[6].sprite.scale_x = AS(u16, ((BtlRecord *)r1)[6].sprite.scale_y) = g_BtlShowScales[0];
            line_path_make((u16 *) (path + j * 20), ((BtlRecord *)r1)[6].sprite.x, ((BtlRecord *)r1)[6].sprite.y, 0xA0, 0x48, 0x14);
          }
          r2 = (u8 *) ((j << 6) + g_BtlRecords);
          ((BtlRecord *)r2)[6].sprite.x = path[j * 20 + f].x;
          ((BtlRecord *)r2)[6].sprite.y = path[j * 20 + f].y;
          v = g_BtlShowScales[(s16) (f * 2 / 5)];
          ((BtlRecord *)r2)[6].sprite.scale_x = AS(u16, ((BtlRecord *)r2)[6].sprite.scale_y) = v;
          continue;
        }
        else if (f == 0x14)
        {
          g_PadActuator1Level = 0;
          g_PadActuatorLevels = 0;
          *(s16 *) ((j << 6) + g_BtlRecords + 0x180) = 0;
          continue;
        }
      }
      if (t & 1)
      {
        r3 = (u8 *) ((j << 6) + g_BtlRecords);
        ((BtlRecord *)r3)[6].sprite.x = AS(u16, ((BtlRecord *)r3)[6].sprite.x) + 1;
        ((BtlRecord *)r3)[6].sprite.y = AS(u16, ((BtlRecord *)r3)[6].sprite.y) + 3;
      }
      else
      {
        r4 = (u8 *) ((j << 6) + g_BtlRecords);
        ((BtlRecord *)r4)[6].sprite.x = AS(u16, ((BtlRecord *)r4)[6].sprite.x) - 1;
        ((BtlRecord *)r4)[6].sprite.y = AS(u16, ((BtlRecord *)r4)[6].sprite.y) - 3;
      }
    }
    if (!(layer_mix_fade_check(8) & 0xFF) && (tile_mix_fade_check() == 0))
    {
      if (u & 1)
      {
        layer_mix_fade_start(8, 1, 0xA, 4);
        tile_mix_fade_start(1, 0xA, 4);
      }
      else
      {
        layer_mix_fade_start(8, 1, 0xA, 0x10);
        tile_mix_fade_start(1, 0xA, 0x10);
      }
      u++;
    }
    if (k >= 8)
    {
      k = 0;
    }
    BtlTilePatternPut(g_BtlTilePatternList[k++], 1);
  }
  xmalloc_heap_free((s32) path);
  g_BtlShowSounds[2].state = 0;
  g_BtlShowSounds[1].state = 0;
  BtlShowSoundSet(4, 0x55, 0, 0);
  BtlShowSoundSet(5, 0x66, 0, 0);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  layer_mix_set(8, 0xFF);
  layer_mix_fade_start(8, 1, 0x40, 0x1F);
  tile_mix_fade_start(1, 0x40, 0x1F);
  g_BtlTilePatternList += 8;
  if (*g_BtlTilePatternList != 0xFF)
  {
    do
    {
      BtlTilePatternPut(*g_BtlTilePatternList++, 3);
    }
    while (*g_BtlTilePatternList != 0xFF);
  }
  while ((layer_mix_fade_check(8) & 0xFF) || (tile_mix_fade_check() != 0))
  {
    BtlShowFrameStep();
  }
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}

/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
void BtlEffectShowPlacesRun(u16, u16);
void BtlEffectShotRun(s32);

s32 BtlShowSoundSet(s32, s32, s32, s32);                        /* extern */
s32 BtlShowSoundStop(s32);                                 /* extern */
s32 BtlShowAnimationPlay(s32, s32, s32);                         /* extern */

/* func_8007DC40 */
void BtlShowActingStart(s32 stop_sound) {
    if (*(g_BtlActingSlot + g_BtlSlotIds) == 0x78) {
        if (stop_sound != 0) {
            BtlShowSoundSet(1, 0x5E, 0, 0);
            BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 8, 0);
            BtlShowSoundStop(1);
            return;
        }
        BtlShowSoundSet(1, 0x5D, 0, 0);
        BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 0xA, 0);
        return;
    }
    BtlShowActingPlay(0, 1);
}
/* far_syms: g_BtlTargetSlot */
s32 BtlShowPulseSet(s32, s32, s32, s32, s32, s32, s32);

/* func_8007DD10 */
void BtlShow09Run(void)
{
  s16 xs[5];
  s16 k;
  s16 t;
  s16 i;
  s16 n;
  s16 end;
  u16 x;
  u16 y0;
  u16 w;
  s32 disp;
  BtlRecord *obj;

  BtlShowActingStart(1);
  g_BtlShowPulse.colour_1[0] = g_BtlShowPulse.colour_1[1] = g_BtlShowPulse.colour_1[2] = -0x3C;
  layer_col_fade_start(0, 0xC, 1, 0x10, (s32) g_BtlShowPulse.colour_1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowPulseSet(3, 0x3C, 0x3C, -0x50, 0, 0, -0x14);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  *(BtlRecord *) (g_BtlShowRecords + 0x140) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  disp = g_BtlShowRecords;
  ((BtlRecord *) (disp + 0x140))->sprite.flags = 0x200;
  ((BtlRecord *) (disp + 0x140))->on = 0;
  ((BtlRecord *) (disp + 0x140))->sprite.depth = ((BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.depth + 1;
  BtlAnimationSetCopy(disp + 0x140, 1);
  n = BtlAnimationFramesCount(g_BtlShowRecords + 0x140);
  obj = (BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
  x = obj->sprite.x;
  y0 = obj->sprite.y;
  w = obj->width / 2;
  g_BtlShowPulse.on = 1;
  xs[0] = x - w;
  xs[1] = x + w;
  w >>= 1;
  xs[2] = x - w;
  xs[3] = x + w;
  xs[4] = x;
  end = n + 0x41;
  for (k = 0; k < end; k++)
  {
    t = k;
    for (i = 0; i < 5; i++, t -= 0xD)
    {
      if (t == 0)
      {
        *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x140);
        ((BtlRecord *) ((i << 6) + g_BtlShowRecords))->on = 1;
        ((BtlRecord *) ((i << 6) + g_BtlShowRecords))->sprite.x = xs[i];
        ((BtlRecord *) ((i << 6) + g_BtlShowRecords))->sprite.y = y0 + rng_mask_below(0xF, 6);
        g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        BtlShowSoundSet(i, 0x75, 0, 0);
        if (i == 4)
        {
          BtlAmountShowStart(1);
        }
      }
      else if (t == n)
      {
        ((BtlRecord *) ((i << 6) + g_BtlShowRecords))->on = 0;
      }
    }
    BtlShowFrameStep();
  }
  BtlShowActingEnd();
  BtlShowRecordsClear();
  BtlShowPulseClear();
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
  layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_1);
  layer_col_fade_start(0, 0xC, 1, 0x50, 0);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
}

s32 BtlShowPulseSet(s32, s32, s32, s32, s32, s32, s32);

/* func_8007E124 */
void BtlShow0ARun(void)
{
  s16 i;
  s32 disp;
  u8 *obj;
  BtlShowActingStart(1);
  g_BtlShowPulse.colour_1[0] = g_BtlShowPulse.colour_1[1] = g_BtlShowPulse.colour_1[2] = -0x3C;
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  BtlShowPulseSet(3, 0x3C, 0x3C, -0x50, 0, 0, -0x14);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  BtlShowSoundSet(1, 0x72, 1, 0);
  BtlShowSoundSet(2, 0x72, 1, 0);
  *(BtlRecord *) g_BtlShowRecords = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  disp = g_BtlShowRecords;
  g_PadActuatorList = (s32 *) g_BtlActuatorFFTwiceList;
  ((BtlRecord *) disp)->on = 1;
  obj = (u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
  g_PadActuatorRestFlags = 0x3F;
  ((BtlRecord *)disp)->sprite.x = ((BtlRecord *) obj)->sprite.x;
  g_PadActuatorListWait = 0;
  ((BtlRecord *)disp)->sprite.y = ((BtlRecord *) obj)->sprite.y - 0x10;
  ((BtlRecord *)disp)->sprite.depth = ((BtlRecord *) obj)->sprite.y + 0x10;
  ((BtlRecord *)disp)->sprite.flags = 0x200;
  BtlShowAnimationPlay(disp, 1, 0);
  BtlShowSoundSet(1, 0x72, 1, 0x1C);
  BtlShowSoundSet(2, 0x72, 1, 0x1C);
  BtlShowSoundSet(3, 0x73, 1, 0x10);
  BtlShowSoundSet(4, 0x73, 1, 0x10);
  BtlShowSoundSet(5, 0x74, 1, 0);
  g_BtlShowPulse.on = 1;
  BtlShowAnimationPlay(g_BtlShowRecords, 2, 0);
  BtlShowPulseClear();
  for (i = 1; i < 6; i++)
  {
    BtlShowSoundStop(i);
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlAmountShowStart(1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_1);
  layer_col_fade_start(0, 0xC, 1, 0x50, 0);
  BtlShowAnimationPlay(g_BtlShowRecords, 3, 0);
  *(s16 *) g_BtlShowRecords = 0;
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
  BtlShowActingEnd();
}
/* far_syms: g_BtlRecords g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorLevels g_PadActuator1Level */
s32 rng_7f_draw(void);  /* TU declares this below */
s32 BtlRecordScaleSet();

/* func_8007E444 */
void BtlShow0BRun(void)
{
  s16 i;
  s16 j;
  s16 n;
  s16 x;
  s16 v;
  s16 total;
  s16 d;

  *(BtlRecord *) (g_BtlShowRecords + 0x140) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  ((BtlRecord *) g_BtlShowRecords)[5].sprite.x = 0xA0;
  ((BtlRecord *) g_BtlShowRecords)[5].sprite.y = 0x78;
  ((BtlRecord *) g_BtlShowRecords)[5].sprite.depth = 0x1078;
  ((BtlRecord *) g_BtlShowRecords)[5].sprite.flags = 0x200;
  BtlShowActingStart(0);
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseList;
  g_PadActuatorRestFlags = 0x3F;
  g_BtlShowPulse.colour_1[0] = g_BtlShowPulse.colour_1[1] = g_BtlShowPulse.colour_1[2] = -0x3C;
  g_PadActuatorListWait = 0;
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  BtlShowPulseSet(3, 0x3C, 0x3C, -0x50, 0, 0, -0x14);
  BtlShowSoundSet(1, 0x73, 1, 0x20);
  ((BtlRecord *) g_BtlShowRecords)[5].on = 1;
  *(BtlRecord *) (g_BtlShowRecords + 0x180) = *(BtlRecord *) (g_BtlShowRecords + 0x140);
  BtlAnimationSetCopy(g_BtlShowRecords + 0x180, 1);
  g_BtlShowPulse.on = 1;
  for (i = 2; i < 4; i++)
  {
    BtlAnimationSetCopy(g_BtlShowRecords + 0x140, (u16) i);
    j = 0;
    n = BtlAnimationFramesCount(g_BtlShowRecords + 0x140);
    for (; j < n; j++)
    {
      ((BtlRecord *) g_BtlShowRecords)[5].on = j & 1;
      BtlShowFrameStep();
    }
  }
  ((BtlRecord *) g_BtlShowRecords)[5].on = 1;
  g_BtlShowPulse.on = 0;
  g_BtlShowSounds[1].state = 0;
  while ((layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
  layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_1);
  layer_col_fade_start(0, 0xC, 1, 0x50, (s32) g_BtlShowPulse.colour_2);
  BtlShowAnimationPlay(g_BtlShowRecords + 0x140, 4, 0);
  for (j = 0; j < 0x10; j++)
  {
    ((BtlRecord *) g_BtlShowRecords)[5].sprite.y++;
    v = g_BtlShowScales[j / 2];
    BtlRecordScaleSet(g_BtlShowRecords + 0x140, v, v, 1);
    BtlShowFrameStep();
  }
  ((BtlRecord *) g_BtlShowRecords)[6].on = 0;
  ((BtlRecord *) g_BtlShowRecords)[5].on = 0;
  layer_mix_fade_start(8, 1, 0x14, 0x1F);
  BtlTileScrollSet(8, ((g_BtlEffectTileRow->width >> 1) - 0x14) * 8, 0x20);
  layer_mix_fade_start(8, 1, 0x14, 8);
  BtlShowSoundSet(2, 0x75, 0, 0);
  layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_1);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_2);
  for (j = 0; j < 0xC; j++)
  {
    BtlTilePatternPut(*g_BtlTilePatternList++, 3);
    if (j == 0xA)
    {
      layer_mix_fade_start(8, 1, 0x14, 0x1F);
    }
  }
  while ((layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFTwiceList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  tile_grid_clear_copy(0, 1);
  x = 0xA0;
  BtlTileEnd();
  BtlShowActingEnd();
  g_PadActuatorList = (s32 *) g_BtlActuatorToggleList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(3, 0x5B, 1, 0x18);
  BtlShowSoundSet(4, 0x5B, 7, 0x18);
  BtlShowSoundSet(5, 0x5B, 0xD, 0x18);
  BtlShowSoundSet(6, 0x5B, 0x13, 0x18);
  for (j = 0; j < 0xE; j++)
  {
    *(BtlRecord *) ((j << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
    ((BtlRecord *) g_BtlShowRecords)[j].sprite.flags = 0x200;
    BtlAnimationSetCopy(g_BtlShowRecords + (j << 6), 5);
    if (g_BtlSlotAmountKinds[j] != 0)
    {
      x = ((BtlRecord *) g_BtlRecords)[j].sprite.x;
      ((BtlRecord *) g_BtlShowRecords)[j].sprite.x = x;
      ((BtlRecord *) g_BtlShowRecords)[j].sprite.y = ((BtlRecord *) g_BtlRecords)[j].sprite.y;
    }
    else
    {
      ((BtlRecord *) g_BtlShowRecords)[j].sprite.x = rng_7f_draw() & 0xFF;
      if (x < 0xA0)
      {
        x = ((BtlRecord *) g_BtlShowRecords)[j].sprite.x += 0xA0;
      }
      else
      {
        x = ((BtlRecord *) g_BtlShowRecords)[j].sprite.x += 0x20;
      }
      ((BtlRecord *) g_BtlShowRecords)[j].sprite.y = rng_mask_below(0x3F, 0x3F) + 0x70;
    }
    ((BtlRecord *) g_BtlShowRecords)[j].sprite.depth = ((BtlRecord *) g_BtlShowRecords)[j].sprite.y + 1;
  }
  g_BtlShowPulse.colour_1[0] = g_BtlShowPulse.colour_1[1] = 0x78;
  g_BtlShowPulse.colour_1[2] = -0x78;
  x = BtlAnimationFramesCount(g_BtlShowRecords);
  total = x + 0x54;
  for (i = 0; i < total; i++)
  {
    for (j = 0; j < 0xE; j++)
    {
      d = i - j * 6;
      if (d == 0)
      {
        layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_1);
        layer_col_fade_start(0, 0xC, 1, 5, 0);
        ((BtlRecord *) g_BtlShowRecords)[j].on = 1;
      }
      else if (d == x)
      {
        ((BtlRecord *) g_BtlShowRecords)[j].on = 0;
      }
    }
    if (i == total / 2)
    {
      BtlAmountShowStart(1);
    }
    BtlShowFrameStep();
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
  for (j = 3; j < 7; j++)
  {
    BtlShowSoundStop(j);
  }
  BtlShowPulseClear();
  layer_mix_fade_start(8, 1, 0x14, 0x1F);
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  while ((BtlAmountShowCheck() != 0) || (layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
}

/* far_syms: g_PadActuatorListWait */
/* far_syms: g_PadActuatorList */
/* far_syms: g_PadActuatorRestFlags */
void BtlTravelRun(BtlTravelBlock *, u16);

/* func_8007ECA4 */
void BtlEffectShotRun(s32 col)
{
  s16 win[3];
  BtlTravelBlock req;
  u8 *actor;
  u8 *target;
  s32 disp;
  s32 h;
  s32 base;
  s32 d2;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x200;
  win[0] = win[1] = win[2] = -0x1E;
  layer_col_fade_start(0, 0xC, 1, 4, (s32) win);
  BtlShowActingPlay(0, 1);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(2, 0x4F, 0, 0);
  *(BtlRecord *) g_BtlShowRecords = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  disp = g_BtlShowRecords;
  actor = (u8 *) ((g_BtlActingSlot << 6) + (base = g_BtlRecords));
  *(s16 *) disp = 1;
  req.mode = 0;
  req.step = 0x10;
  req.from = disp;
  req.from_dx = ((BtlRecord *)actor)->width >> 1;
  target = (u8 *) (base + (g_BtlTargetSlot << 6));
  req.from_dy = -(((BtlRecord *)actor)->height >> 1);
  req.to = (s32) target;
  req.to_dx = 0;
  h = -(((BtlRecord *)target)->height >> 2);
  if (h >= -0xF)
  {
    h = -0x10;
  }
  req.to_dy = h;
  BtlTravelRun(&req, 1);
  BtlShowActingEnd();
  g_BtlShowSounds[1].state = 0;
  layer_col_set(0, 0xC, col);
  layer_col_fade_start(0, 0xC, 1, 0x20, 0);
  BtlShowSoundSet(3, 0x73, 0, 0);
  BtlShowSoundSet(4, 0x73, 0, 0);
  d2 = g_BtlShowRecords;
  ((BtlRecord *)d2)->sprite.flags = 0x200;
  BtlShowAnimationPlay(d2, 2, 0);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  BtlShowSoundStop(3);
  BtlShowSoundStop(4);
}

void BtlShowPlacesPlay(u16, u16);
s32 BtlShowActingPlay(s32, s32);
void BtlActingShowPlacesRun(s32 palette, s32 animation);  /* TU defines this below */
void BtlShow23Run(void);  /* TU defines this below */
s32 ratio_29_32_mul(s32 value);  /* TU defines this below */
void BtlEffectCopyTargetStandRun();
void BtlTargetSpriteMixRun(s32);

/* func_8007EEC0 */
void BtlShow0CRun(void)
{
    s16 sp[3];

    sp[1] = 0xA0;
    sp[0] = 0xA0;
    sp[2] = 0;
    BtlEffectShotRun(sp);
}

/* func_8007EEF0 */
void BtlShow0DRun(void)
{
    s16 sp[3];

    sp[1] = 0x50;
    sp[0] = 0x50;
    sp[2] = 0xA0;
    BtlEffectShotRun(sp);
}

/* func_8007EF24 */
void BtlShow0ERun(void)
{
    s16 sp[3];

    sp[0] = 0xA0;
    sp[2] = 0x50;
    sp[1] = 0x50;
    BtlEffectShotRun(sp);
}
/* func_8007EF58 */
void BtlShow0FRun(void)
{
    s16 sp[3];

    sp[0] = 0xA0;
    sp[1] = 0x50;
    sp[2] = 0xA0;
    BtlEffectShotRun(sp);
}
/* func_8007EF8C */
void BtlShow10Run(void)
{
    s16 sp[3];

    sp[0] = 0x50;
    sp[2] = 0xA0;
    sp[1] = 0xA0;
    BtlEffectShotRun(sp);
}
s32 ShopAnimationSet(MapRecord *, s32, s32);
void ShopAnimationStep(s32, s32, s32);
s32 MenuFrameStep();

/* The sprite data head's word 0 is read from the global through its
   member's address (*(u16 *)&((SpriteDataHead *)g_SpriteData0)->animation_table):
   as a member load (of the global or of bank) or AS(u16, ...) the
   function grows by 0x10 bytes. */
/* func_8007EFC0 */
void MenuUseShowPlacesPlay(u16 a, s32 b)
{
  u16 i;
  u16 n;
  SpriteDataHead *bank;
  MapRecord *rec;
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseShortList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x18, 0, 0);
  i = 0;
  while ((g_BtlSlotShowPlaceFlags[i] == 0) && (i < 6))
  {
    ((MapRecord *) g_MenuUseShowRecords)[i].on_kind.on = 0;
    i++;
  }
  rec = &((MapRecord *) g_MenuUseShowRecords)[i];
  bank = (SpriteDataHead *) g_SpriteData0;
  rec->on_kind.on = 1;
  rec->sprite.on = 1;
  rec->sprite.attr = 0;
  rec->sprite.flags = (a & 0x3FF) | 0x6000;
  rec->sprite.texel_records = bank->texel_records * 4 + (s32) bank;
  rec->sprite.pose_table = bank->pose_table * 4 + (s32) bank;
  rec->sprite.piece_table = bank->piece_table * 4 + (s32) bank;
  rec->animation_table = *(u16 *)&((SpriteDataHead *) g_SpriteData0)->animation_table * 4 + (s32) bank;
  rec->step_list = bank->step_list * 4 + (s32) bank;
  AS(s32, rec->sprite.scale_x) = 0;
  rec->sprite.scaled = 0;
  rec->sprite.pose = 0;
  rec->animation_before = 0xFF;
  rec->animation = 0xFF;
  ShopAnimationSet(rec, b, 1);
  n = (rec->end & 0x7FFF) - 1;
  if (i < 6)
  {
    for (; i < 6; i++)
    {
      if (g_BtlSlotShowPlaceFlags[i] != 0)
      {
        ((MapRecord *) g_MenuUseShowRecords)[i] = *rec;
        rec = &((MapRecord *) g_MenuUseShowRecords)[i];
        AS(u16, rec->sprite.x) = g_BtlSlotShowPlaceXs[i];
        AS(u16, rec->sprite.y) = g_BtlSlotShowPlaceYs[i];
        AS(u16, rec->sprite.depth) = g_BtlSlotShowPlaceYs[i] + 1;
      }
      else
      {
        ((MapRecord *) g_MenuUseShowRecords)[i].on_kind.on = 0;
      }
    }
  }
  if (n != 0)
  {
    while (rec->step < n)
    {
      ShopAnimationStep(g_MenuUseShowRecords, 0x6E, 6);
      MenuFrameStep();
    }
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
}

void BtlRecordSpriteDataSet(s32, u8 *, u16, u16);
s32 BtlRecordClear(s32);

/* func_8007F2D8 */
void BtlShowPlacesPlay(u16 a, u16 b)
{
  BtlRecord unused;  /* never used; the original's frame holds 0x40 bytes for it */
  u32 i;
  s32 blk;
  BtlShowSoundSet(1, 0x21, 0, 0);
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotShowPlaceFlags[i] != 0)
    {
      blk = g_BtlShowRecords + (i << 6);
      BtlRecordSpriteDataSet(blk, g_SpriteData0, a, b);
      *(s16 *) blk = 1;
      ((BtlRecord *)blk)->sprite.attr = 0;
      ((BtlRecord *)blk)->sprite.x = g_BtlSlotShowPlaceXs[i];
      ((BtlRecord *)blk)->sprite.y = g_BtlSlotShowPlaceYs[i];
      ((BtlRecord *)blk)->sprite.depth = g_BtlSlotShowPlaceYs[i] + 1;
    }
    else
    {
      BtlRecordClear(g_BtlShowRecords + (i << 6));
    }
  }
  BtlAmountShowStart(1);
  BtlShowAnimationPlay(blk, b, 0);
  while (BtlAmountShowCheck() != 0)
  {
    BtlShowFrameStep();
  }
}
/* Battle-mode counterpart of MenuUseShowPlacesPlay (BtlShow11Run / BtlShow14Run
   pick by g_BtlRunning): point the text cursor at g_BtlActuatorRiseShortList, count the
   objects flagged in g_BtlSlotShowPlaceFlags (a count nothing reads), wait 0x30 frames
   if g_BtlShowVoiceState, place the per-object blocks (BtlShowPlacesPlay(a, b)), then
   clear the cursor state.
   The dead count is what the target's lone `lui/addiu $v1, g_BtlSlotShowPlaceFlags`
   ahead of an empty loop records: its two-variable cycle is not trivially
   dead at cse, so the flag test's branch survives flow (which deletes the
   cycle but keeps the load the branch uses and the loop-hoisted address);
   jump2 then deletes the empty branch and its computation by REG_DEAD
   notes, and the hoisted pointer, live across iterations, carries none. */
/* func_8007F42C */
void BtlEffectShowPlacesRun(u16 a, u16 b)
{
  u16 i;
  s32 count;
  s32 found;
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseShortList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  count = 0;
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotShowPlaceFlags[i] != 0)
    {
      found = count + 1;
      count = found;
    }
  }
  BtlShowActingPlay(0, 1);
  if (g_BtlShowVoiceState != 0)
  {
    i = 0x30;
    do
    {
      BtlShowFrameStep();
      i--;
    }
    while (i != 0);
  }
  BtlShowPlacesPlay(a, b);
  BtlShowActingEnd();
  BtlShowRecordsClear();
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
}

/* func_8007F518 */
void BtlActingShowPlacesRun(s32 palette, s32 animation) {
    BtlShowActingPlay(0x10, 0);
    BtlShowPlacesPlay(palette & 0xFFFF, animation & 0xFFFF);
    BtlShowActingEnd();
    BtlShowRecordsClear();
}
extern Tbl30 g_BtlTargetSpriteMixCols;
void sprite_mix_switch_on();
void sprite_mix_set();
void sprite_mix_col_set();
void sprite_mix_switch_off();

/* func_8007F56C */
void BtlTargetSpriteMixRun(s32 who)
{
  Tbl30 tbl;
  u16 i;
  u16 lvl;
  tbl = g_BtlTargetSpriteMixCols;
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x33, 0, 0);
  BtlShowPulseSet(0xF, 0x28, 0x28, 0x50, 0x14, 0x14, 0x28);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  sprite_mix_switch_on();
  if ((who != 0) && (g_BtlActingSlot == g_BtlTargetSlot))
  {
    ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = (((BtlRecord *)g_BtlShowRecords)[20].sprite.flags & 0xE3FF) | 0x800;
  }
  else
  {
    ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags = (((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags & 0xE3FF) | 0x800;
  }
  lvl = 0x1F;
  sprite_mix_set(2, 0x1F);
  g_BtlShowPulse.on = 1;
  for (i = 0; i < 0xA0; i++)
  {
    if (i < 0x8C)
    {
      if ((i % 20) == 0)
      {
        lvl--;
        sprite_mix_set(2, lvl);
      }
    }
    else if ((i % 5) == 0)
    {
      lvl++;
      sprite_mix_set(2, lvl);
    }
    sprite_mix_col_set(tbl.v[(u16) (i % 30)]);
    BtlShowFrameStep();
  }
  BtlShowPulseClear();
  sprite_mix_set(2, -1);
  sprite_mix_switch_off();
  if ((who != 0) && (g_BtlActingSlot == g_BtlTargetSlot))
  {
    ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags &= 0xE3FF;
  }
  else
  {
    ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.flags &= 0xE3FF;
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
  BtlShowSoundStop(1);
  BtlShowActingEnd();
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
}

extern u8 g_BtlPartyStandAnimations[];
extern u8 g_BtlSlotRestAnimations[];

/* func_8007F94C */
void BtlEffectCopyTargetStandRun(void)
{
  s16 win[3];
  s16 i;
  s16 n;
  s32 off;
  s32 blk;
  s32 rec;
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseShortList;
  g_PadActuatorRestFlags = 0x3F;
  win[0] = win[1] = win[2] = -0x32;
  g_PadActuatorListWait = 0;
  layer_col_fade_start(0, 0xC, 1, 0x28, (s32) win);
  *(BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  off = g_BtlTargetSlot << 6;
  rec = g_BtlRecords + off;
  blk = off + g_BtlShowRecords;
  *(s16 *) blk = 1;
  BtlRecordPlaceCopy(rec, (s16 *) blk);
  BtlAnimationSetCopy(g_BtlShowRecords + (g_BtlTargetSlot << 6), 1);
  BtlShowSoundSet(2, 0x7E, 1, 0x28);
  n = BtlAnimationFramesCount(g_BtlShowRecords + (g_BtlTargetSlot << 6));
  for (i = 0; i < n; i++)
  {
    if (i == 0xF0)
    {
      BtlAnimationSetCopy(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlPartyStandAnimations[g_BtlTargetSlot]);
      g_BtlSlotRestAnimations[g_BtlTargetSlot] = g_BtlPartyStandAnimations[g_BtlTargetSlot];
    }
    BtlShowFrameStep();
  }
  g_BtlShowSounds[2].state = 0;
  g_PadActuatorList = (s32 *) g_BtlActuatorC8List;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
}

/* func_8007FB94 */
void BtlShow11Run(void)
{
    if (g_BtlRunning != 0) {
        BtlEffectShowPlacesRun(0x780, 6);
    } else {
        MenuUseShowPlacesPlay(0x780, 6);
    }
}
extern s8 g_PadActuatorLevels;
extern u8 g_PadActuator1Level;

/* func_8007FBD8 */
void BtlShow13Run(void)
{
  s16 i;
  s32 blk;
  BtlShowActingPlay(0, 1);
  blk = 0;
  g_PadActuatorList = (s32 *) g_BtlActuatorToggleList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      blk = g_BtlShowRecords + (i << 6);
      *(BtlRecord *) blk = *(BtlRecord *) (g_BtlShowRecords + 0x500);
      BtlRecordPlaceCopy(g_BtlRecords + (i << 6), (s16 *) blk);
      BtlAnimationSetCopy(blk, 1);
    }
  }
  BtlShowSoundSet(2, 0x29, 0, 0);
  BtlShowAnimationPlay(blk, 1, 0);
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
}

extern s8 g_PadActuatorLevels; /* far_syms: g_PadActuatorLevels */
extern u8 g_PadActuator1Level; /* far_syms: g_PadActuator1Level */

/* func_8007FD34 */
void BtlShow14Run(void)
{
    if (g_GameOptions[5] != 0) {
        g_PadActuator1Level = 0;
        g_PadActuatorLevels = 1;
    }
    if (g_BtlRunning != 0) {
        BtlEffectShowPlacesRun(0x780, 7);
    } else {
        MenuUseShowPlacesPlay(0x780, 7);
    }
}

/* func_8007FD9C */
void BtlShow15Run(void)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `cue` only holds the constant 1; behaviour is the same with the constant
     written where it is read, but then 4 of this function's 13 instruction lines differ. */
  int cue;
  /* CODEGEN FORCING DEVICE (copy of a copy), not recovered source:
     `passed_cue` only copies `cue`, which is not assigned again afterwards; behaviour
     is the same with `cue` used directly, but then 4 of this function's 13
     instruction lines differ. */
  unsigned long long passed_cue;
  cue = 1;
  passed_cue = cue;
  BtlShowActingPlay(0, passed_cue);
  BtlShowActingEnd();
  BtlEffectCopyTargetStandRun();
}
/* func_8007FDD0 */
void BtlShow16Run(void)
{
    if (g_GameOptions[5] != 0) {
        g_PadActuator1Level = 0x46;
        g_PadActuatorLevels = 0;
    }
    BtlShowActingPlay(0, 1);
    BtlTargetSpriteMixRun(1);
}
s32 BtlTravelPathsMake(BtlTravelBlock *);

/* func_8007FE20 */
void BtlShow17Run(void)
{
  BtlTravelBlock req;
  s16 frame;
  s16 k;
  s16 d;
  s16 n;
  s16 total;
  s16 m;
  s32 t;
  u8 *rec;
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  layer_mix_fade_start(8, 1, 0x14, 0x1F);
  BtlShowActingPlay(0, 1);
  BtlShowActingEnd();
  req.step = 2;
  req.mode = 0;
  req.from_dx = 0;
  req.from = g_BtlRecords + (g_BtlActingSlot << 6);
  req.from_dy = -*(u16 *) (req.from + 0x3E);
  req.to = g_BtlRecords + (g_BtlTargetSlot << 6);
  req.to_dx = 0;
  req.to_dy = -*(u16 *) (req.to + 0x3E) / 2;
  n = BtlTravelPathsMake(&req);
  BtlShowSoundSet(1, 0x7D, 1, 0x40);
  BtlShowSoundSet(2, 0x7D, 0x20, 0x40);
  total = n + 0x48;
  for (frame = 0; frame < total; frame++)
  {
    for (k = 0; k < 9; k++)
    {
      d = frame - k * 8;
      if (d < 0)
      {
        continue;
      }
      if (d == 0)
      {
        *(BtlRecord *) ((k << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
        ((BtlRecord *) g_BtlShowRecords)[k].on = 1;
        BtlAnimationSetCopy((s32) &((BtlRecord *) g_BtlShowRecords)[k], (u16) (k % 3 + 1));
      }
      if (d < n)
      {
        ((BtlRecord *) g_BtlShowRecords)[k].sprite.x = ((PathPoint *) req.path)[d].x;
        ((BtlRecord *) g_BtlShowRecords)[k].sprite.y = ((PathPoint *) req.path)[d].y;
        ((BtlRecord *) g_BtlShowRecords)[k].sprite.depth = ((PathPoint *) req.depth_path)[d].y + 1;
      }
    }
    BtlShowFrameStep();
  }
  xmalloc_heap_free(req.path);
  xmalloc_heap_free(req.depth_path);
  for (k = 0; k < 9; k++)
  {
    ((BtlRecord *) g_BtlShowRecords)[k].sprite.depth = ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.depth + 1;
  }
  if (g_BtlSlotAmountKinds[g_BtlTargetSlot] == 1)
  {
    g_PadActuatorList = (s32 *) g_BtlActuatorToggleList;
    g_PadActuatorListWait = 0;
    rec = (u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
    t = ((BtlRecord *) rec)->sprite.x + ((BtlRecord *)rec)->width;
    g_PadActuatorRestFlags = 0x3F;
    m = t / 8 + 1;
    ((BtlRecord *) rec)->sprite.attr |= 0x2000;
    for (frame = 0; frame < m; frame++)
    {
      for (k = 0; k < 9; k++)
      {
        ((BtlRecord *) g_BtlShowRecords)[k].sprite.x -= 8;
      }
      ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].sprite.x -= 8;
      BtlShowFrameStep();
    }
    ((BtlRecord *) g_BtlRecords)[g_BtlTargetSlot].on = 0;
  }
  else
  {
    n = 0x50;
    for (frame = 0; frame < n; frame++)
    {
      for (k = 0; k < 9; k++)
      {
        d = frame - k * 8;
        if (d >= 0)
        {
          if (d == 8)
          {
            ((BtlRecord *) g_BtlShowRecords)[k].on = 0;
          }
          else
          {
            ((BtlRecord *) g_BtlShowRecords)[k].sprite.y += 2;
          }
        }
      }
      BtlShowFrameStep();
    }
    BtlShowFrameStep();
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
  g_BtlShowSounds[2].state = 0;
  g_BtlShowSounds[1].state = 0;
}

/* func_80080344 */
void BtlShow18Run(void)
{
  s16 win[3];
  PathPoint *path;
  s16 frame;
  s16 k;
  s16 d;
  s32 disp;
  u8 *obj;
  u8 *o2;
  s32 blk;
  s32 b0;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  win[0] = win[1] = win[2] = -0x3C;
  layer_col_fade_start(0, 0xC, 1, 0x10, (s32) win);
  BtlShowActingPlay(0, 1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  *(BtlRecord *) (g_BtlShowRecords + 0x4C0) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  disp = g_BtlShowRecords;
  ((BtlRecord *)disp)[19].on = 1;
  ((BtlRecord *)disp)[19].sprite.depth = *(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xA) + 1;
  ((BtlRecord *)disp)[19].sprite.y = 0x18;
  path = (PathPoint *) xmalloc_heap_alloc(0x40);
  obj = (u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
  if (((BtlRecord *)obj)->sprite.x < 0xA1)
  {
    ((BtlRecord *)g_BtlShowRecords)[19].sprite.attr |= 0x2000;
    ((BtlRecord *)g_BtlShowRecords)[19].sprite.x = AS(u16, ((BtlRecord *)obj)->sprite.x) + 0x80;
  }
  else
  {
    ((BtlRecord *)g_BtlShowRecords)[19].sprite.attr &= 0x9FFF;
    ((BtlRecord *)g_BtlShowRecords)[19].sprite.x = AS(u16, ((BtlRecord *)obj)->sprite.x) - 0x80;
  }
  o2 = (u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
  line_path_make((u16 *) path, ((BtlRecord *)g_BtlShowRecords)[19].sprite.x, ((BtlRecord *)g_BtlShowRecords)[19].sprite.y, ((BtlRecord *)o2)->sprite.x, ((BtlRecord *)o2)->sprite.y, 0x10);
  BtlShowSoundSet(1, 0x6E, 0, 0);
  win[0] = 0x78;
  win[1] = 0x58;
  win[2] = 0;
  layer_col_fade_start(0, 0xC, 1, 0x10, (s32) win);
  BtlShowAnimationPlay(g_BtlShowRecords + 0x4C0, 1, 0);
  BtlShowSoundSet(4, 0x70, 0, 0);
  g_PadActuatorList = (s32 *) g_BtlActuatorToggleList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  for (frame = 0; frame < 0x70; frame++)
  {
    for (k = 0; k < 8; k++)
    {
      d = frame - k * 8;
      if ((u16) d < 0x10)
      {
        if (d == 0)
        {
          *(BtlRecord *) ((k << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x4C0);
          b0 = (k << 6) + g_BtlShowRecords;
          ((BtlRecord *)b0)->sprite.depth = *(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 8) + 1;
          BtlAnimationSetCopy(b0, 3);
          if (k + 1 == 8)
          {
            g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            BtlShowSoundSet(1, 0x6E, 0, 0);
            BtlAnimationSetCopy(g_BtlShowRecords + 0x4C0, 2);
            BtlAmountShowStart(1);
            layer_col_fade_start(0, 0xC, 1, 0x30, 0);
          }
        }
        blk = (k << 6) + g_BtlShowRecords;
        ((BtlRecord *)blk)->sprite.x = path[d].x;
        ((BtlRecord *)blk)->sprite.y = path[d].y;
      }
      else if (d == 0x10)
      {
        BtlAnimationSetCopy(g_BtlShowRecords + (k << 6), 4);
        BtlShowSoundSet(5, 0x5A, 0, 0);
      }
      else if (d == 0x40)
      {
        *(s16 *) ((k << 6) + g_BtlShowRecords) = 0;
      }
    }
    BtlShowFrameStep();
  }
  xmalloc_heap_free((s32) path);
  BtlShowActingEnd();
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
}
/* func_800807F0 */
void BtlShow19Run(void)
{
  u16 i;
  u16 n;
  u8 *obj;
  s32 blk;
  s32 h;
  s32 b2;
  BtlShowActingPlay(0, 1);
  BtlShowPulseSet(3, 0xA0, 0x3C, -0x28, 0x50, 3, -0x14);
  layer_col_fade_start(0, 0xC, 1, 0x32, (s32) g_BtlShowPulse.colour_1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x6E, 0, 0);
  BtlShowSoundSet(2, 0x6F, 1, 0);
  *(BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  obj = (u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
  blk = (g_BtlTargetSlot << 6) + g_BtlShowRecords;
  ((BtlRecord *)blk)->sprite.x = ((BtlRecord *) obj)->sprite.x;
  h = -(((BtlRecord *)obj)->height >> 2);
  if (h >= -0xF)
  {
    h = -0x10;
  }
  ((BtlRecord *)blk)->sprite.y = ((BtlRecord *) obj)->sprite.y + h;
  ((BtlRecord *)blk)->sprite.depth = 0x1000;
  ((BtlRecord *)blk)->sprite.flags = 0x200;
  g_BtlShowPulse.on = 1;
  BtlAnimationSetCopy(blk, 1);
  n = BtlAnimationFramesCount(g_BtlShowRecords + (g_BtlTargetSlot << 6)) * 3 / 2;
  for (i = 0; i < n; i++)
  {
    if (i == 0x40)
    {
      BtlShowSoundSet(2, 0x6E, 0, 0);
      g_BtlShowPulse.on = 0;
      layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_2);
    }
    *(s16 *) ((g_BtlTargetSlot << 6) + g_BtlShowRecords + 4) = i & 1;
    BtlShowFrameStep();
  }
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  b2 = (g_BtlTargetSlot << 6) + g_BtlShowRecords;
  ((BtlRecord *)b2)->sprite.y = ((BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y;
  BtlAnimationSetCopy(b2, 2);
  n = BtlAnimationFramesCount(g_BtlShowRecords + (g_BtlTargetSlot << 6)) * 2;
  for (i = 0; i < n; i++)
  {
    if (i == 0x10)
    {
      BtlAmountShowStart(1);
      BtlShowSoundSet(1, 0x6E, 0, 0);
      BtlShowSoundSet(2, 0x70, 0, 0);
      BtlShowSoundSet(3, 0x70, 0, 0);
      layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_1);
      layer_col_fade_start(0, 0xC, 1, 0x20, 0);
    }
    *(s16 *) ((g_BtlTargetSlot << 6) + g_BtlShowRecords + 4) = i & 1;
    BtlShowFrameStep();
  }
  BtlShowPulseClear();
  BtlShowActingEnd();
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
}

s32 rng_7f_draw(void);

/* func_80080BF8 */
void BtlShow1ARun(void)
{
  s16 i;
  s16 k;
  s16 x;
  s16 d;
  s32 off;
  s32 b0;
  s32 b1;
  s32 b2;
  u16 r;
  u16 v;
  u8 *obj;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  BtlShowActingPlay(0, 1);
  BtlShowPulseSet(3, 0x78, 0, -0x28, 0x3C, 0, -0x14);
  layer_col_fade_start(0, 0xC, 1, 0x32, (s32) g_BtlShowPulse.colour_1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x70, 1, 0x10);
  BtlShowSoundSet(2, 0x70, 1, 0x10);
  BtlShowSoundSet(3, 0x5A, 8, 0x10);
  x = 0;
  for (i = 0; i < 0x10; i++)
  {
    *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
    off = i << 6;
    b0 = off + g_BtlShowRecords;
    *(s16 *) b0 = 0;
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      obj = (u8 *) (off + g_BtlRecords);
      x = ((BtlRecord *) obj)->sprite.x;
      g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
      g_PadActuatorListWait = 0;
      ((BtlRecord *)b0)->sprite.x = x;
      g_PadActuatorRestFlags = 0x3F;
      ((BtlRecord *)b0)->sprite.y = ((BtlRecord *) obj)->sprite.y - 0xB0;
    }
    else
    {
      ((BtlRecord *) (b1 = off + g_BtlShowRecords))->sprite.x = r = (u8) rng_7f_draw();
      if (x < 0xA0)
      {
        v = r + 0xA0;
        ((BtlRecord *) b1)->sprite.x = v;
        x = v;
      }
      else
      {
        v = r + 0x20;
        ((BtlRecord *) b1)->sprite.x = v;
        x = v;
      }
      *(u16 *) ((i << 6) + g_BtlShowRecords + 0xA) = rng_mask_below(0x3F, 0x3F) - 0x40;
    }
    b2 = (i << 6) + g_BtlShowRecords;
    ((BtlRecord *)b2)->sprite.depth = AS(u16, ((BtlRecord *)b2)->sprite.y) + 0xB4;
    BtlAnimationSetCopy(b2, 1);
  }
  *(s16 *) g_BtlShowRecords = 1;
  for (k = 0; k < 0xB0; k++)
  {
    for (i = 0; i < 0x10; i++)
    {
      d = k - i * 8;
      if (d < 0)
      {
        continue;
      }
      if (d == 0)
      {
        *(s16 *) ((i << 6) + g_BtlShowRecords) = 1;
      }
      else if (d < 0x10)
      {
        *(u16 *) ((i << 6) + g_BtlShowRecords + 0xA) += 0xB;
      }
      else if (d == 0x10)
      {
        g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        layer_col_set(0, 0xC, (s32) g_BtlShowPulse.colour_1);
        layer_col_fade_start(0, 0xC, 1, 3, (s32) g_BtlShowPulse.colour_2);
        BtlAnimationSetCopy(g_BtlShowRecords + (i << 6), 2);
      }
      else if (d == 0x30)
      {
        *(s16 *) ((i << 6) + g_BtlShowRecords) = 0;
      }
    }
    BtlShowFrameStep();
  }
  BtlAmountShowStart(1);
  g_BtlShowSounds[3].state = 0;
  g_BtlShowSounds[2].state = 0;
  g_BtlShowSounds[1].state = 0;
  BtlShowActingEnd();
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
}

/* func_80081068 */
void BtlShow1BRun(void)
{
  u16 i;
  s32 disp;
  BtlShowActingPlay(0, 1);
  if (g_BtlShowVoiceState != 0)
  {
    i = 0x38;
    do
    {
      BtlShowFrameStep();
      i--;
    }
    while (i != 0);
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseShortList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  *(BtlRecord *) g_BtlShowRecords = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  BtlRecordPlaceCopy(g_BtlRecords + (g_BtlTargetSlot << 6), (s16 *) g_BtlShowRecords);
  disp = g_BtlShowRecords;
  *(s16 *) disp = 1;
  ((BtlRecord *)disp)->sprite.flags = 0x2200;
  *(BtlRecord *) (disp + 0x40) = *(BtlRecord *) disp;
  ((BtlRecord *)g_BtlShowRecords)[1].sprite.depth = *(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 8) - 1;
  BtlShowSoundSet(1, 0x20, 0, 0);
  BtlAnimationSetCopy(g_BtlShowRecords + 0x40, 2);
  BtlShowAnimationPlay(g_BtlShowRecords, 1, 0);
  disp = g_BtlShowRecords;
  ((BtlRecord *)disp)[1].on = 0;
  *(s16 *) disp = 0;
  BtlShowActingEnd();
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
}

/* func_80081218 */
void BtlShow1CRun(void)
{
  s16 win[3];
  s32 off;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  BtlShowActingPlay(0, 1);
  win[0] = win[1] = -0x28;
  win[2] = 0x28;
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) win);
  BtlShowSoundSet(1, 0x68, 1, 0);
  BtlShowSoundSet(2, 0x7A, 0, 0);
  BtlShowSoundSet(3, 0x7B, 1, 0);
  g_PadActuatorList = (s32 *) g_BtlActuatorC8List;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  *(BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  off = g_BtlTargetSlot << 6;
  BtlRecordPlaceCopy(g_BtlRecords + off, (s16 *) (g_BtlShowRecords + off));
  BtlAmountShowStart(1);
  BtlShowAnimationPlay(g_BtlShowRecords + (g_BtlTargetSlot << 6), 1, 0);
  BtlShowActingEnd();
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
}
s32 BtlAnimationPlayCheck(s32);
s32 BtlTilePatternStep(u16, s32);

/* func_800813C0 */
void BtlShow1DRun(void)
{
  s16 win[3];
  u16 base[2];
  s16 i;
  s16 k;
  s16 d;
  u16 seed;
  s32 off;
  s32 blk;
  s32 b0;
  u8 *obj;
  u16 y;
  s16 n;
  s32 r;
  BtlShowActingPlay(0, 1);
  g_PadActuatorList = (s32 *) g_BtlActuatorRandomList;
  g_PadActuatorRestFlags = 0x3F;
  win[0] = win[1] = -0x1E;
  win[2] = 0x3C;
  g_PadActuatorListWait = 0;
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) win);
  BtlShowSoundSet(1, 0x5F, 0, 0);
  BtlTileScrollSet(8, g_BtlEffectTileRow->width * 4 - ((BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x, 0xB0 - ((BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y);
  tile_grid_clear_copy(0, 1);
  layer_mix_set(8, 0xC);
  for (i = 0; i < 7; i++)
  {
    BtlTilePatternPut(*g_BtlTilePatternList++, 4);
  }
  layer_mix_fade_start(8, 1, 0x1E, 0x1F);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    BtlShowFrameStep();
  }
  BtlShowActingEnd();
  BtlShowSoundSet(2, 0x60, 1, 0x20);
  tile_grid_clear_copy(0, 1);
  layer_mix_set(8, 0xF);
  for (i = 0; i < 4; i++)
  {
    if (i >= 2)
    {
      BtlTileScrollMove(8, 0, 6);
    }
    BtlTilePatternPut(*g_BtlTilePatternList++, 4);
  }
  BtlShowSoundSet(3, 0x68, 1, 0x10);
  n = 0x30;
  base[0] = *(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 6);
  base[1] = *(u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xA) - 0x10;
  i = 0;
  seed = 0;
  for (; i < n; i++)
  {
    for (k = 0; k < 0x10; k++)
    {
      d = i - k;
      if (d == 0)
      {
        s32 o;
        *(BtlRecord *) ((k << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
        o = k << 6;
        b0 = o + g_BtlShowRecords;
        *(s16 *) b0 = 1;
        ((BtlRecord *)b0)->sprite.flags = 0x200;
        r = rng_mask_below(0x3F, 0x30);
        *(u16 *) (o + g_BtlShowRecords + 6) = base[0] + r - 0x18;
        r = rng_mask_below(0x1F, 0x1C);
        y = base[1] + r;
        blk = o + g_BtlShowRecords;
        ((BtlRecord *)blk)->sprite.y = y - 0xE;
        ((BtlRecord *)blk)->sprite.depth = y + 3;
        BtlAnimationSetCopy(g_BtlShowRecords + o, (u16) (rng_mask_below(3, 3) + 1));
      }
      else if (d > 0)
      {
        off = k << 6;
        if (*(u16 *) (off + g_BtlShowRecords) != 0)
        {
          if (BtlAnimationPlayCheck(off + g_BtlShowRecords) == 0)
          {
            if (i >= 0x21)
            {
              *(s16 *) (off + g_BtlShowRecords) = 0;
            }
            else
            {
              r = rng_mask_below(0x3F, 0x30);
              *(u16 *) (off + g_BtlShowRecords + 6) = base[0] + r - 0x18;
              r = rng_mask_below(0x1F, 0x1C);
              y = base[1] + r;
              blk = off + g_BtlShowRecords;
              ((BtlRecord *)blk)->sprite.y = y - 0xE;
              ((BtlRecord *)blk)->sprite.depth = y + 3;
              BtlAnimationSetCopy(g_BtlShowRecords + off, (u16) (rng_mask_below(3, 3) + 1));
            }
          }
        }
      }
    }
    seed = BtlTilePatternStep(seed, 4);
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlAmountShowStart(1);
  layer_mix_fade_start(8, 1, 0x10, 0x1F);
  layer_col_fade_start(0, 0xC, 1, 0x10, 0);
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF) || (layer_mix_fade_check(8) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 4);
  }
  g_BtlShowSounds[3].state = 0;
  g_BtlShowSounds[2].state = 0;
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}

extern u8 g_BtlActuatorQuickenList[];
void layer_mix_switch_on(s32);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 BtlTilePatternPut(s32, s32);
s32 layer_mix_fade_check(s32);
s32 BtlTileScrollStep(s32, s32);
void tile_grid_clear_copy(s32, s32);
s32 BtlTileEnd();

/* func_80081870 */
void BtlShow1ERun(void)
{
  u8 i;
  BtlShowActingPlay(0, 1);
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      ((BtlRecord *) g_BtlRecords)[i].sprite.flags = (((BtlRecord *) g_BtlRecords)[i].sprite.flags & 0x83FF) | 0x4800;
    }
  }
  sprite_mix_col_set_copy(0x7FFF);
  layer_mix_switch_on(0x100);
  layer_mix_fade_start(0x400, 1, 0x14, 0x10);
  g_PadActuatorList = (s32 *) g_BtlActuatorQuickenList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x79, 1, 0);
  BtlShowSoundSet(2, 0x55, 1, 0);
  BtlTilePatternPut(*g_BtlTilePatternList, 1);
  for (i = 0; i < 6; i++)
  {
    if (i == 5)
    {
      BtlAmountShowStart(1);
    }
    if (i & 1)
    {
      layer_mix_fade_start(8, 1, 0x14, 0x1F);
    }
    else
    {
      layer_mix_fade_start(8, 1, 0x14, 8);
    }
    while (layer_mix_fade_check(8) & 0xFF)
    {
      BtlTileScrollStep(0x10, 0);
    }
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
  BtlShowActingEnd();
  layer_mix_fade_start(8, 1, 0x14, 0x1F);
  while ((BtlAmountShowCheck() != 0) || (layer_mix_fade_check(8) & 0xFF))
  {
    BtlTileScrollStep(0x10, 0);
  }
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      ((BtlRecord *) g_BtlRecords)[i].sprite.flags = (((BtlRecord *) g_BtlRecords)[i].sprite.flags & 0x83FF) | 0x2000;
    }
  }
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}
extern u8 g_BtlActuatorC8LateList[];

/* func_80081AD8 */
void BtlShow20Run(void)
{
  BtlTravelBlock req;
  s16 win[3];
  u8 *rec;
  u8 *target;
  s32 base;
  s32 disp;
  s32 d2;
  u16 z;
  BtlShowSoundSet(1, 0x37, 0, 0);
  BtlShowActingPlay(0, 0);
  BtlFrameWait(8);
  BtlShowSoundSet(2, 0x2A, 0, 0);
  g_PadActuatorListWait = 0;
  rec = (u8 *) ((g_BtlActingSlot << 6) + (base = g_BtlRecords));
  disp = g_BtlShowRecords;
  *(u16 *) rec = 0;
  ((BtlRecord *)disp)[20].on = 1;
  req.mode = 1;
  req.step = 8;
  req.from = disp + 0x500;
  z = ((BtlRecord *)rec)->width >> 1;
  g_PadActuatorList = (s32 *) g_BtlActuatorC8LateList;
  req.from_dy = 0;
  target = (u8 *) (base + (g_BtlTargetSlot << 6));
  req.to = (s32) target;
  req.from_dx = z;
  req.to_dx = ((BtlRecord *)target)->width >> 1;
  g_PadActuatorRestFlags = 0x3F;
  req.to_dy = 0;
  BtlTravelRun(&req, 1);
  BtlShowSoundStop(2);
  g_BtlShowVoiceState = 1;
  BtlShowSoundSet(3, 0x39, 0x32, 0);
  d2 = g_BtlShowRecords;
  ((BtlRecord *)d2)[20].sprite.depth = AS(u16, ((BtlRecord *)d2)[20].sprite.y) + 1;
  BtlShowAnimationPlay(d2 + 0x500, 2, 0);
  win[0] = win[1] = win[2] = 0x78;
  layer_col_set(0, 0xC, (s32) win);
  layer_col_fade_start(0, 0xC, 1, 0x10, 0);
  BtlAmountShowStart(1);
  BtlFrameWait(8);
  if ((((BtlRecord *)g_BtlShowRecords)[20].sprite.x != *(s16 *) (((g_BtlActingSlot << 6) + g_BtlRecords) + 6)) || (((BtlRecord *)g_BtlShowRecords)[20].sprite.y != *(s16 *) (((g_BtlActingSlot << 6) + g_BtlRecords) + 0xA)))
  {
    BtlShowSoundSet(6, 7, 0, 0);
    BtlRecordSpotMove(g_BtlShowRecords + 0x500, ((BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x, ((BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y, 0x3F, 3);
  }
  BtlShowActingEnd();
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
}

/* func_80081D24 */
void BtlShow21Run(void)
{
  s16 win[3];
  s16 i;
  s16 n;
  s32 disp;
  g_PadActuatorList = (s32 *) g_BtlActuatorFFTwiceList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  ((BtlRecord *) g_BtlRecords)[g_BtlActingSlot].on = 0;
  disp = g_BtlShowRecords;
  ((BtlRecord *)disp)[20].sprite.flags = 0x200;
  ((BtlRecord *)disp)[20].on = 1;
  win[0] = win[1] = win[2] = 0xA0;
  BtlShowSoundSet(1, 0x37, 0, 0);
  BtlShowSoundSet(2, 0x3A, 0x24, 0);
  g_BtlShowVoiceState = 1;
  BtlAnimationSetCopy(g_BtlShowRecords + 0x500, 0);
  n = BtlAnimationFramesCount(g_BtlShowRecords + 0x500);
  for (i = 0; (i < n) || (BtlAmountShowCheck() != 0); i++)
  {
    if (i == 0x44)
    {
      layer_col_set(0, 0xC, (s32) win);
      layer_col_fade_start(0, 0xC, 1, 0x10, 0);
      BtlAmountShowStart(1);
    }
    BtlShowFrameStep();
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
}
/* func_80081E98 */
void BtlShow22Run(void)
{
  s16 i;
  s16 k;
  s16 d;
  s16 x;
  s16 step;
  s32 n;
  s32 m;
  s32 base;
  s32 disp;
  s32 d0;
  s32 b2;
  s32 one;
  ((BtlRecord *) (base = g_BtlRecords))[g_BtlActingSlot].on = 0;
  disp = g_BtlShowRecords;
  x = 0x130;
  ((BtlRecord *)disp)[20].on = 1;
  if (((BtlRecord *)disp)[20].sprite.x < 0xA0)
  {
    x = 0x10;
  }
  ((void (*)()) BtlEffectRecordMove)(5, 0x3F, x, *(u16 *) (((g_BtlTargetSlot << 6) + base) + 0xA));
  BtlShowSoundSet(1, 0x37, 0, 0);
  BtlShowActingPlay(0, 0);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFTwiceList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  g_BtlShowVoiceState = 1;
  BtlShowSoundSet(2, 0x39, 0x3B, 0);
  BtlShowSoundSet(3, 0x36, 0x40, 0);
  BtlShowAnimationPlay(g_BtlShowRecords + 0x500, 1, 0);
  BtlShowPulseSet(2, 0x50, 0x50, 0x50, 0x14, 0x14, 0x14);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  *(BtlRecord *) g_BtlShowRecords = *(BtlRecord *) (g_BtlShowRecords + 0x500);
  d0 = g_BtlShowRecords;
  ((BtlRecord *)d0)->sprite.flags = 0x200;
  ((BtlRecord *)d0)->sprite.depth = AS(u16, ((BtlRecord *)d0)->sprite.y) + 1;
  *(s16 *) d0 = 0;
  if (((BtlRecord *)d0)[20].sprite.attr & 0x2000)
  {
    step = -0x10;
    ((BtlRecord *)d0)->sprite.x = AS(u16, ((BtlRecord *)d0)->sprite.x) - 0x20;
  }
  else
  {
    step = 0x10;
    ((BtlRecord *)d0)->sprite.x = AS(u16, ((BtlRecord *)d0)->sprite.x) + 0x20;
  }
  for (i = 1; i < 0x14; i++)
  {
    *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) ((i << 6) + g_BtlShowRecords - 0x40);
    *(u16 *) ((i << 6) + g_BtlShowRecords + 6) += step;
  }
  g_BtlShowPulse.on = 1;
  i = 0;
  one = 1;
  n = 0x2B;
  m = 0x15;
  for (; i < n; i++)
  {
    for (k = 0; k < 0x14; k++)
    {
      d = i - k;
      if (d == 0)
      {
        b2 = (k << 6) + g_BtlShowRecords;
        *(s16 *) b2 = one;
        BtlAnimationSetCopy(b2, 2);
        if (k == one)
        {
          BtlAnimationSetCopy(g_BtlShowRecords, 4);
        }
      }
      else if (d == one)
      {
        BtlAnimationSetCopy(g_BtlShowRecords + (k << 6), 3);
      }
      else if (d == 0x15)
      {
        BtlAnimationSetCopy(g_BtlShowRecords + (k << 6), 4);
      }
      else if (d == 0x17)
      {
        *(s16 *) ((k << 6) + g_BtlShowRecords) = 0;
      }
    }
    if (i == m)
    {
      BtlAmountShowStart(1);
      BtlShowPulseClear();
    }
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorC8List;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundStop(3);
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  BtlEffectRecordBackMove(5, 0x3F);
  BtlShowActingEnd();
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
}

/* func_800822C8 */
void BtlShow23Run(void) {
    BtlEffectRecordPlay(0);
}
/* far_syms: g_BtlActingSlot */
typedef struct { u8 b[9]; } Seq9;
extern u8 g_BtlShow27PatternList[];
void BtlTilePatternSizePut(s32, s32, s32, s32);

/* func_800822E8 */
void BtlShow27Run(void)
{
  Seq9 seq;
  s16 win[3];
  u16 i;
  u16 j;
  u16 k;
  u16 seed;
  u16 n;
  s32 t;
  BtlRecord *obj;
  seq = *(Seq9 *) g_BtlShow27PatternList;
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowActingPlay(0, 1);
  win[0] = win[1] = 0x1E;
  win[2] = 0x3C;
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) win);
  BtlTileScrollSet(8, g_BtlEffectTileRow->width * 4 - ((BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x, g_BtlEffectTileRow->height * 4 + (((BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords))->height >> 1) - ((BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y);
  i = 0;
  seed = 0;
  layer_mix_fade_start(8, 1, 0x28, 0xF);
  do
  {
    seed = BtlTilePatternStep(seed, 1);
    j = 0;
    do
    {
      if (!(layer_mix_fade_check(8) & 0xFF))
      {
        if (i & 1)
        {
          layer_mix_fade_start(8, 1, 0x14, 8);
        }
        else
        {
          layer_mix_fade_start(8, 1, 0x14, 0xF);
        }
        i++;
      }
      BtlShowFrameStep();
      j++;
    }
    while (j < 3);
    i++;
  }
  while (i < 9);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_mix_fade_start(8, 1, 0x28, 0x1F);
  layer_col_fade_start(0, 0xC, 1, 0x50, 0);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_mix_fade_start(8, 1, 0x28, 8);
  BtlTilePatternSizePut(0, 7, 7, 1);
  BtlShowSoundSet(1, 0x47, 0, 0);
  BtlShowSoundSet(2, 0x47, 0x80, 0);
  i = 0;
  k = 0;
  do
  {
    if (!(layer_mix_fade_check(8) & 0xFF))
    {
      if (i & 1)
      {
        layer_mix_fade_start(8, 1, 0xA, 8);
      }
      else
      {
        layer_mix_fade_start(8, 1, 0x1E, 0xF);
      }
      i++;
    }
    if (seq.b[k] == 0xFF)
    {
      k = 0;
    }
    BtlTilePatternSizePut(seq.b[k++], 7, 7, 0);
    for (j = 0; j < 0x10; j++)
    {
      BtlTileScrollStep(1, -1);
    }
  }
  while (i < 6);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    if (seq.b[k] == 0xFF)
    {
      k = 0;
    }
    BtlTilePatternSizePut(seq.b[k++], 7, 7, 0);
    for (j = 0; j < 0x10; j++)
    {
      BtlTileScrollStep(1, -1);
    }
  }
  layer_mix_fade_start(8, 1, 0x28, 0x1F);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    if (seq.b[k] == 0xFF)
    {
      k = 0;
    }
    BtlTilePatternSizePut(seq.b[k++], 7, 7, 0);
    for (j = 0; j < 0x10; j++)
    {
      BtlTileScrollStep(1, -1);
    }
  }
  BtlShowActingEnd();
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
  g_PadActuatorList = (s32 *) g_BtlActuatorToggleList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  i = 6;
  n = 6;
  for (; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] == 1)
    {
      obj = (BtlRecord *) ((i << 6) + g_BtlRecords);
      n = (n > (obj->sprite.x + obj->width) / 8) ? n : (obj->sprite.x + obj->width) / 8;
      obj->sprite.attr |= 0x2000;
    }
  }
  for (j = 0; j < n; j++)
  {
    for (i = 6; i < 0xE; i++)
    {
      if (g_BtlSlotAmountKinds[i] == 1)
      {
        ((BtlRecord *) g_BtlRecords)[i].sprite.x -= 8;
      }
    }
    BtlShowFrameStep();
  }
  for (i = 6; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] == 1)
    {
      ((BtlRecord *) g_BtlRecords)[i].on = 0;
    }
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
}

/* func_800828EC */
void BtlShow28Run(void)
{
  u16 i;
  s32 off;
  s32 blk;
  u8 *obj;
  s32 d;
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowActingPlay(0, 1);
  BtlShowPulseSet(0x1E, 0x50, 0x32, 0x32, 0x1E, 0x14, 0x14);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorC8List;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
      off = i << 6;
      d = off + g_BtlShowRecords;
      ((BtlRecord *) d)->sprite.flags = 0x200;
      BtlAnimationSetCopy(d, 1);
      obj = (u8 *) (off + g_BtlRecords);
      blk = off + g_BtlShowRecords;
      ((BtlRecord *)blk)->sprite.x = AS(u16, ((BtlRecord *)obj)->sprite.x);
      if (((BtlRecord *)obj)->height < 0x18)
      {
        ((BtlRecord *)blk)->sprite.y = AS(u16, ((BtlRecord *)obj)->sprite.y) - 0x18;
      }
      else
      {
        ((BtlRecord *)blk)->sprite.y = AS(u16, ((BtlRecord *)obj)->sprite.y) - ((BtlRecord *)obj)->height;
      }
      if (*(s16 *) ((i << 6) + g_BtlShowRecords + 0xA) < 0x28)
      {
        *(s16 *) ((i << 6) + g_BtlShowRecords + 0xA) = 0x28;
      }
      *(u16 *) ((i << 6) + g_BtlShowRecords + 8) = *(u16 *) ((i << 6) + g_BtlRecords + 8) + 1;
      blk = (i << 6) + g_BtlShowRecords;
    }
  }
  g_BtlShowPulse.on = 1;
  BtlShowSoundSet(1, 0x48, 0, 0);
  BtlShowSoundSet(2, 0x48, 0x10, 0);
  BtlShowAnimationPlay(blk, 1, 0);
  BtlShowActingEnd();
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseShortList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      BtlAnimationSetCopy(g_BtlShowRecords + (i << 6), 2);
    }
  }
  BtlShowAnimationPlay(blk, 2, 0);
  BtlShowPulseClear();
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_col_fade_start(0, 0xC, 1, 0x14, 0);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
}

s32 BtlShowAnimationPlay(s32, s32, s32);

/* func_80082C54 */
void BtlShowActingThreeIdsStart(void)
{
    s32 animation;
    u8 idx;

    idx = g_BtlActingSlot;
    switch (g_BtlSlotIds[idx]) {
    case 0x79:
        BtlShowAnimationPlay(g_BtlRecords + (idx << 6), 8, 0);
        BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 9, 0);
        BtlShowActingEnd();
        return;
    case 0x7D:
        animation = 8;
        break;
    case 0x7C:
        animation = 0xE;
        break;
    default:
        animation = 0;
        break;
    }
    BtlShowActingPlay(animation, 1);
}

/* func_80082D1C */
void BtlShowActingThreeIdsEnd(void)
{
    s32 animation;
    s32 blink;
    u8 idx;

    idx = g_BtlActingSlot;
    switch (g_BtlSlotIds[idx]) {
    case 0x79:
        return;
    case 0x7D:
        animation = 9;
        blink = 0;
        break;
    case 0x7C:
        animation = 0xF;
        blink = 0;
        break;
    default:
        goto tail;
    }
    BtlShowAnimationPlay(g_BtlRecords + (idx << 6), animation, blink);
tail:
    BtlShowActingEnd();
}
extern u8 g_BtlActuatorFFToAFList[];
extern u8 g_BtlShow2AWaveFactors[];
s32 BtlTileScrollSet(s32, s32, s32);
s32 BtlTileScrollMove(s32, s32, s8);
s32 BtlTilePatternStep(u16, s32);

/* func_80082D9C */
void BtlShow2ARun(void)
{
  u16 i;
  u16 j;
  u16 n;
  u16 seed;
  s32 k;
  s32 blk;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  BtlTileScrollSet(8, 0, 0xE0);
  BtlShowActingThreeIdsStart();
  BtlShowPulseSet(0x78, 0x78, 0x28, -0xA, 0x3C, 0x14, 0);
  layer_col_fade_start(0, 0xC, 1, 0x14, (s32) g_BtlShowPulse.colour_1);
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_mix_fade_start(8, 1, 0x14, 0xC);
  BtlShowWaveSet(4, 1, 5, 1, g_BtlShow2AWaveFactors);
  BtlShowSoundSet(1, 0x70, 1, 0x40);
  BtlShowSoundSet(2, 0x70, 0x21, 0x40);
  i = 0;
  seed = 0;
  g_PadActuatorList = (s32 *) g_BtlActuatorRandomList;
  g_PadActuatorRestFlags = 0x13;
  g_PadActuatorListWait = 0;
  g_BtlShowPulse.on = 1;
  for (; i < 0x140; i++)
  {
    if (i & 1)
    {
      BtlTileScrollMove(8, 0, -1);
    }
    seed = BtlTilePatternStep(seed, 1);
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(3, 0x51, 0, 0);
  BtlShowSoundSet(4, 0x6F, 0x28, 0);
  for (j = 0; j < 2; j++)
  {
    for (i = 0; i < 0xE; i++)
    {
      if (g_BtlSlotAmountKinds[i] != 0)
      {
        blk = g_BtlShowRecords + (i << 6);
        *(BtlRecord *) blk = *(BtlRecord *) (g_BtlShowRecords + 0x500);
        *(s16 *) blk = 1;
        BtlRecordPlaceCopy(g_BtlRecords + (i << 6), (s16 *) blk);
        BtlAnimationSetCopy(blk, (u16) (j + 1));
      }
    }
    n = BtlAnimationFramesCount(blk);
    for (i = 0; i < n; i++)
    {
      seed = BtlTilePatternStep(seed, 1);
    }
  }
  g_PadActuatorList = (s32 *) g_BtlActuatorFFToAFList;
  g_PadActuatorRestFlags = 0x2C;
  g_PadActuatorListWait = 0;
  for (i = 0; i < 0x140; i++)
  {
    k = i;  /* int-wide: the zero-extended index stays in one register */
    if (k & 1)
    {
      BtlTileScrollMove(8, 0, 1);
    }
    if (k == 0x64)
    {
      BtlShowWaveSet(4, 1, 5, 0, g_BtlShow2AWaveFactors);
    }
    else if (k == 0xF0)
    {
      layer_mix_fade_start(8, 1, 0x50, 0x1F);
      g_BtlShowPulse.on = 0;
    }
    seed = BtlTilePatternStep(seed, 1);
  }
  g_BtlShowSounds[2].state = 0;
  g_BtlShowSounds[1].state = 0;
  while ((layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 1);
  }
  layer_col_fade_start(0, 0xC, 1, 0x28, 0);
  BtlShowSoundSet(3, 0x6E, 0, 0);
  BtlShowSoundSet(4, 0x6E, 0, 0);
  BtlAmountShowStart(1);
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
  for (i = 0; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      blk = g_BtlShowRecords + (i << 6);
      BtlAnimationSetCopy(blk, 3);
    }
  }
  g_BtlShowPulse.on = 1;
  n = BtlAnimationFramesCount(blk);
  for (i = 0; i < n; i++)
  {
    seed = BtlTilePatternStep(seed, 1);
  }
  BtlShowPulseClear();
  for (i = 0; i < 0xE; i++)
  {
    ((BtlRecord *) g_BtlShowRecords)[i].on = 0;
  }
  BtlShowActingThreeIdsEnd();
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 1);
  }
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}
extern u8 g_BtlShow2BPoints[];
extern u8 g_BtlShow2BAnimations[];
void *memcpy(void *, const void *, u32);

/* func_80083310 */
void BtlShow2BRun(void)
{
  u16 anim[4];
  PathPoint corner[8];
  s16 i;
  s16 j;
  s16 d;
  s16 k;
  u16 seed;
  s32 n;
  PathPoint *path;
  s32 blk;
  s32 blk2;
  BtlRecord *o;

  memcpy(anim, g_BtlShow2BAnimations, sizeof(anim));
  memcpy(corner, g_BtlShow2BPoints, sizeof(corner));
  path = (PathPoint *) xmalloc_heap_alloc(0x200);
  i = 0;
  do
  {
    *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
    blk = (i << 6) + g_BtlShowRecords;
    *(s16 *) blk = 0;
    BtlAnimationSetCopy(blk, anim[i % 4]);
    blk2 = (i << 6) + g_BtlShowRecords;
    ((BtlRecord *)blk2)->sprite.scaled = 1;
    ((BtlRecord *)blk2)->sprite.depth = 0x1000;
    if ((i == 0) || (i == 3) || (i == 4))
    {
      *(u16 *) (((i << 6) + g_BtlShowRecords) + 0xC) |= 0x2000;
    }
    else
    {
      ((BtlRecord *)blk2)->sprite.attr &= 0x9FFF;
    }
    line_path_make((u16 *) &path[i * 16], 0xA0, 0x70, corner[i].x, (s16) corner[i].y, 0x10);
    *(BtlRecord *) (((i << 6) + g_BtlShowRecords) + 0x200) = *(BtlRecord *) ((i << 6) + g_BtlShowRecords);
    i++;
  }
  while (i < 8);
  seed = 0;
  BtlShowActingThreeIdsStart();
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseShortList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowPulseSet(0xA, 0, 0, 0x78, 0, 0, 0x3C);
  layer_col_fade_start(0, 0xC, 1, 0x28, (s32) g_BtlShowPulse.colour_1);
  layer_mix_fade_start(8, 1, 0x28, 0xF);
  while ((layer_col_fade_check(0) & 0xFF) || (layer_mix_fade_check(8) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 1);
  }
  BtlShowSoundSet(1, 0x7B, 1, 0x10);
  BtlShowSoundSet(2, 0x7B, 5, 0x10);
  BtlShowSoundSet(3, 0x7B, 9, 0x10);
  BtlShowSoundSet(4, 0x7B, 0xD, 0x10);
  BtlShowSoundSet(5, 0x79, 1, 0);
  g_BtlShowPulse.on = 1;
  n = 1;
  i = 0;
  do
  {
    for (j = 0; j < 0x10; j++)
    {
      if (j < 8)
      {
        d = i - j * 4;
      }
      else
      {
        d = (s16) (i - 8) - j * 4;  /* the (s16) keeps fold from reassociating
                                       to i - (j*4 + 8): $t1 = i - 8 is hoisted */
      }
      if ((d >= 0) && (d < 0x60))
      {
        if (d == 0)
        {
          ((BtlRecord *) g_BtlShowRecords)[j].on = 1;
        }
        k = d % 16 + (j % 8) * 16;
        o = (BtlRecord *) ((j << 6) + g_BtlShowRecords);
        o->sprite.x = path[k].x;
        o->sprite.y = path[k].y;
        o->sprite.scale_x = o->sprite.scale_y = (s16) g_BtlShowScales[7 - (k & 0xF) / 2] * 3 / 2;
      }
      else if (d == 0x60)
      {
        ((BtlRecord *) g_BtlShowRecords)[j].on = 0;
        if (j == 0xF)
        {
          g_BtlShowSounds[4].state = 0;
          g_BtlShowSounds[3].state = 0;
          g_BtlShowSounds[2].state = 0;
          g_BtlShowSounds[1].state = 0;
        }
      }
    }
    if (!(layer_mix_fade_check(8) & 0xFF))
    {
      if (n & 1)
      {
        layer_mix_fade_start(8, 1, 8, 0x1F);
      }
      else
      {
        layer_mix_fade_start(8, 1, 8, 0xF);
      }
      n++;
    }
    seed = BtlTilePatternStep(seed, 1);
    i++;
  }
  while (i < 0xA8);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowActingThreeIdsEnd();
  BtlAmountShowStart(1);
  xmalloc_heap_free((s32) path);
  BtlShowPulseClear();
  while ((layer_col_fade_check(0) & 0xFF) || (layer_mix_fade_check(8) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 1);
  }
  layer_mix_fade_start(8, 1, 0x28, 0x1F);
  layer_col_fade_start(0, 0xC, 1, 0x28, 0);
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF) || (layer_mix_fade_check(8) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 1);
  }
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}

/* func_800839C0 */
s32 ratio_29_32_mul(s32 value) {
    s32 product;
    s32 rounded_product;

    product = value * 0x1D;
    rounded_product = product + 0x10;
    if (rounded_product < 0) {
        rounded_product = product + 0x2F;
    }
    return rounded_product >> 5;
}
/* A 16.16 fixed-point point: show 0x2C's spiral and its spline. */
/* g_BtlShow2CPoints: show 0x2C's landing spots, one PathPoint per target (8 entries,
   up to g_BtlShow2CCurveWorkBlock); g_BtlShow2CCurveWorkBlock: MTH_CurvePointsMake's first argument. */
extern PathPoint g_BtlShow2CPoints[];
extern u8 g_BtlShow2CCurveWorkBlock[];
s32 MTH_CurvePointsMake();  /* mth_curv.c; no prototype here */

/* BtlShow2CRun: the battle's show of number 0x2C (entry 0x2B of the table
   BtlShowNumberRun calls). Each flagged target gets a 32-point spiral
   around (0xA0, 0x90) from its own position (next quadrant per step,
   distance scaled 29/32 by ratio_29_32_mul), splined to 64 points; it walks
   a 32-step line onto the spiral, rides the spline for 63 frames, rises off
   the top of the screen for 32, then lands at g_BtlShow2CPoints[k]: a 200 px drop
   over 5 frames and a 48/24/12/6/3 px bounce. The quadrant test is a
   four-clause else-if chain (UR, UL, LL, LR); that, the struct-indexed
   actor records, the p = spl + 1 walk and x reused as the bounce height are
   what place the bytes. */

/* func_800839E8 */
void BtlShow2CRun(void)
{
  s16 i;
  s16 n;
  s16 k;
  s16 j;
  s16 x;
  s16 y;
  u16 bg;
  MthPoint *spl;
  MthPoint *pts;
  MthPoint *p;
  PathPoint *path;
  s32 blk;

  i = 0;
  n = 0;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  BtlShowActingThreeIdsStart();
  g_PadActuatorList = (s32 *) g_BtlActuatorRiseList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  for (; i < 0xE; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      n++;
    }
  }
  spl = (MthPoint *) xmalloc_heap_alloc(n << 9);
  pts = (MthPoint *) xmalloc_heap_alloc(n << 8);
  for (i = 0, k = 0; k < n; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      x = ((BtlRecord *) g_BtlRecords)[i].sprite.x;
      y = ((BtlRecord *) g_BtlRecords)[i].sprite.y;
      pts[0].x = x << 16;
      pts[0].y = y << 16;
      for (j = 0; j < 0x1F; j++)
      {
        if ((x >= 0xA0) && (y < 0x90))
        {
          x = 0xA0 - ratio_29_32_mul(x - 0x9F);
          y = 0x90 - ratio_29_32_mul(0x90 - y);
        }
        else if ((x < 0xA0) && (y < 0x90))
        {
          x = 0xA0 - ratio_29_32_mul(0xA0 - x);
          y = ratio_29_32_mul(0x90 - y) + 0x90;
        }
        else if ((x < 0xA0) && (y >= 0x90))
        {
          x = ratio_29_32_mul(0xA0 - x) + 0xA0;
          y = ratio_29_32_mul(y - 0x8F) + 0x90;
        }
        else if ((x >= 0xA0) && (y >= 0x90))
        {
          x = ratio_29_32_mul(x - 0x9F) + 0xA0;
          y = 0x90 - ratio_29_32_mul(y - 0x8F);
        }
        pts[j + 1].x = x << 16;
        pts[j + 1].y = y << 16;
      }
      p = &spl[k << 6];
      MTH_CurvePointsMake(g_BtlShow2CCurveWorkBlock, pts, 0x20, 0x40, p);
      k++;
    }
  }
  xmalloc_heap_free((s32) pts);
  layer_mix_fade_start(8, 1, 0xA0, 6);
  BtlTileScrollSet(8, g_BtlEffectTileRow->width * 4 - 0xA0, 0);
  bg = 0;
  BtlShowPulseSet(0x28, 0x50, 0x8C, 0x78, 0x3C, 0x46, 0x3C);
  layer_col_fade_start(0, 0xC, 1, 0x50, (s32) g_BtlShowPulse.colour_1);
  BtlShowSoundSet(1, 0x61, 1, 0);
  while (layer_col_fade_check(0) & 0xFF)
  {
    bg = BtlTilePatternStep(bg, 2);
  }
  path = (PathPoint *) xmalloc_heap_alloc(n << 7);
  for (i = 0, k = 0; k < n; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      line_path_make((u16 *) &path[k << 5], ((BtlRecord *) g_BtlRecords)[i].sprite.x, ((BtlRecord *) g_BtlRecords)[i].sprite.y, spl[(k << 6) + 1].x >> 16, spl[(k << 6) + 1].y >> 16, 0x20);
      k++;
    }
  }
  g_BtlShowPulse.on = 1;
  for (j = 0; j < 0x20; j++)
  {
    for (i = 0, k = 0; k < n; i++)
    {
      if (g_BtlSlotAmountKinds[i] != 0)
      {
        ((BtlRecord *) g_BtlRecords)[i].sprite.x = path[(k << 5) + j].x;
        ((BtlRecord *) g_BtlRecords)[i].sprite.y = ((BtlRecord *) g_BtlRecords)[i].sprite.depth = path[(k << 5) + j].y;
        k++;
      }
    }
    bg = BtlTilePatternStep(bg, 2);
  }
  xmalloc_heap_free((s32) path);
  BtlShowSoundSet(2, 0x60, 1, 0x20);
  BtlShowSoundSet(3, 0x60, 0x51, 0x20);
  p = spl + 1;
  for (j = 1; j < 0x40; j++)
  {
    for (i = 0, k = 0; k < n; i++)
    {
      if (g_BtlSlotAmountKinds[i] != 0)
      {
        ((BtlRecord *) g_BtlRecords)[i].sprite.x = p[k << 6].x >> 16;
        ((BtlRecord *) g_BtlRecords)[i].sprite.y = ((BtlRecord *) g_BtlRecords)[i].sprite.depth = p[k << 6].y >> 16;
        k++;
      }
    }
    bg = BtlTilePatternStep(bg, 2);
    p++;
  }
  xmalloc_heap_free((s32) spl);
  for (j = 0; j < 0x20; j++)
  {
    for (i = 0, k = 0; k < n; i++)
    {
      if (g_BtlSlotAmountKinds[i] != 0)
      {
        ((BtlRecord *) g_BtlRecords)[i].sprite.y -= j * 2;
        k++;
      }
    }
    bg = BtlTilePatternStep(bg, 2);
  }
  BtlShowPulseClear();
  layer_mix_fade_start(8, 1, 0x50, 0x1F);
  while (layer_col_fade_check(0) & 0xFF)
  {
    bg = BtlTilePatternStep(bg, 2);
  }
  g_BtlShowSounds[3].state = 0;
  g_BtlShowSounds[2].state = 0;
  g_BtlShowSounds[1].state = 0;
  layer_col_fade_start(0, 0xC, 1, 0xA0, 0);
  for (i = 0, k = 0; k < n; i++)
  {
    if (g_BtlSlotAmountKinds[i] != 0)
    {
      *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
      g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
      g_PadActuatorRestFlags = 0x3F;
      g_PadActuatorListWait = 0;
      ((BtlRecord *) g_BtlShowRecords)[i].sprite.x = ((BtlRecord *) g_BtlRecords)[i].sprite.x = g_BtlShow2CPoints[k].x;
      ((BtlRecord *) g_BtlRecords)[i].sprite.y = g_BtlShow2CPoints[k].y - 0xC8;
      ((BtlRecord *) g_BtlShowRecords)[i].sprite.y = ((BtlRecord *) g_BtlShowRecords)[i].sprite.depth = ((BtlRecord *) g_BtlRecords)[i].sprite.depth = g_BtlShow2CPoints[k].y;
      ((BtlRecord *) g_BtlShowRecords)[i].on = 0;
      BtlAnimationSetCopy((i << 6) + g_BtlShowRecords, 1);
      if (*(u16 *) ((i << 6) + g_BtlRecords + 0x3C) > 0x40)
      {
        blk = (i << 6) + g_BtlShowRecords;
        ((BtlRecord *)blk)->sprite.scale_x = ((BtlRecord *)blk)->sprite.scale_y = 0x4000 / ((BtlRecord *)blk)->width;
        ((BtlRecord *)blk)->sprite.scaled = 1;
      }
      BtlShowSoundSet((s16) (i % 4) + 4, 0x3F, 0, 0);
      for (j = 0; j < 5; j++)
      {
        ((BtlRecord *) g_BtlRecords)[i].sprite.y += 0x14;
        bg = BtlTilePatternStep(bg, 1);
        ((BtlRecord *) g_BtlRecords)[i].sprite.y += 0x14;
        BtlShowFrameStep();
      }
      x = 0x30;
      ((BtlRecord *) g_BtlShowRecords)[i].on = 1;
      for (j = 0; j < 5; j++, x /= 2)
      {
        ((BtlRecord *) g_BtlRecords)[i].sprite.y -= x;
        bg = BtlTilePatternStep(bg, 1);
        ((BtlRecord *) g_BtlRecords)[i].sprite.y += x;
        BtlShowFrameStep();
      }
      k++;
    }
  }
  BtlAmountShowStart(1);
  BtlShowActingThreeIdsEnd();
  while ((BtlAmountShowCheck() != 0) || (layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    bg = BtlTilePatternStep(bg, 2);
  }
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}

s32 ratio_mul(s32, s32, s32);                           /* extern */

/* func_800843BC */
s32 ratio_4_5_mul(s32 value) {
    ratio_mul(value, 4, 5);
}
s32 BtlTileScrollMove(s32, s32, s8);
s32 rng_draw();

/* func_800843E0 */
void BtlShakesRun(s16 period, s8 *offsets)
{
  s16 t;
  s16 k;
  s16 j;
  s32 active;
  s8 *p;
  s32 base;
  k = (t = 0);
  active = 0;
  for (; t < (period * 6); t++)
  {
    if (active)
    {
      p = &offsets[k];
      BtlTileScrollMove(4, 0, *p);
      base = g_BtlRecords;
      for (j = 0; j < 0xE; j++)
      {
        *((u16 *) (((j << 6) + base) + 0xA)) -= *(&offsets[k]);
      }
      AS(u16, ((BtlRecord *)g_BtlShowRecords)[20].sprite.y) -= offsets[k];
      k++;
      if (k >= 6)
      {
        active = 0;
      }
    }
    else
      if ((t % period) == 0)
    {
      active = 1;
      k = 0;
    }
    if (g_GameOptions[5] != 0)
    {
      g_PadActuator1Level += rng_draw() & 0x3F;
      g_PadActuatorLevels = rng_draw() & 1;
    }
    BtlShowFrameStep();
  }
}
extern u8 g_BtlActuator0ToggleList[];
extern s8 g_BtlShow2DSmallShakeOffsets[];
extern s8 g_BtlShow2DLargeShakeOffsets[];
s32 arc_path_make(s32, s32, s32, s32, s32, s32, PathPoint *);

/* func_800845C4 */
void BtlShow2DRun(void)
{
  s16 dx[20];
  s16 tx[20];
  s16 ty[20];
  s16 i;
  s16 j;
  s16 w;
  s16 h;
  s16 total;
  s16 d;
  s8 k;
  s32 side;
  PathPoint *path;
  s32 base;
  s32 blk;
  s32 b0;
  u8 *obj;

  w = 0x40;
  i = 0;
  total = 0;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  BtlShowActingThreeIdsStart();
  do
  {
    total += w;
    w = ratio_4_5_mul(w);
    i++;
  }
  while (i < 6);
  path = (PathPoint *) xmalloc_heap_alloc(total * 4);
  w = 0x40;
  h = 0x90;
  i = 0;
  j = 0;
  do
  {
    arc_path_make((s16) (j * 2), 0, (s16) ((j + w) * 2), 0, w, h, &path[j]);
    j += w;
    w = ratio_4_5_mul(w);
    h = ratio_4_5_mul(h);
    i++;
  }
  while (i < 6);
  i = 0;
  side = 0;
  do
  {
    *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
    *(s16 *) ((i << 6) + g_BtlShowRecords) = 0;
    if ((i < 0xE) && (g_BtlSlotAmountKinds[i] != 0))
    {
      obj = (u8 *) ((i << 6) + g_BtlRecords);
      side = 0;
      if (((BtlRecord *) obj)->sprite.x < 0xA0)
      {
        dx[i] = ((BtlRecord *) obj)->sprite.x + (total * 2);
      }
      else
      {
        side = 1;
        dx[i] = ((BtlRecord *) obj)->sprite.x - (total * 2);
      }
      tx[i] = ((BtlRecord *) ((i << 6) + g_BtlRecords))->sprite.x;
      ty[i] = ((BtlRecord *) ((i << 6) + g_BtlRecords))->sprite.y;
    }
    else
    {
      if (side != 0)
      {
        side = 0;
        tx[i] = (u8) rng_7f_draw() + 0xA0;
        dx[i] = tx[i] - (total * 2);
      }
      else
      {
        side = 1;
        tx[i] = (u8) rng_7f_draw() + 0x20;
        dx[i] = tx[i] + (total * 2);
      }
      ty[i] = rng_mask_below(0x3F, 0x3F) + 0x70;
    }
    i++;
  }
  while (i < 0x14);
  BtlShowPulseSet(0x20, -0x3C, -0x3C, -0x3C, -0x1E, -0x1E, -0x1E);
  layer_col_fade_start(0, 0xC, 1, 0x28, (s32) g_BtlShowPulse.colour_1);
  g_BtlShowPulse.on = 1;
  BtlShowSoundSet(1, 0x55, 1, 0x20);
  BtlShakesRun(6, g_BtlShow2DSmallShakeOffsets);
  BtlShakesRun(6, g_BtlShow2DLargeShakeOffsets);
  g_BtlShowSounds[1].repeat = 0x10;
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3B;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(2, 0x5A, 1, 0x10);
  i = 0;
  k = 0;
  side = 0;
  for (; i < (total + 0x138); i++)
  {
    if (g_GameOptions[5] != 0)
    {
      g_PadActuator1Level += rng_draw() & 0x3F;
    }
    for (j = 0; j < 0x14; j++)
    {
      d = i - (j * 14);
      if ((d >= 0) && (d < total))
      {
        if (d == 0)
        {
          g_PadActuatorRestFlags = 0x3A;
          g_PadActuatorList = (s32 *) g_BtlActuator0ToggleList;
          g_PadActuatorListWait = 0;
          *(BtlRecord *) ((j << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
          b0 = (j << 6) + g_BtlShowRecords;
          ((BtlRecord *)b0)->sprite.flags = 0x200;
          *(s16 *) b0 = 1;
          BtlAnimationSetCopy(b0, 1);
        }
        if (dx[j] < tx[j])
        {
          *(u16 *) (((j << 6) + g_BtlShowRecords) + 6) = dx[j] + path[d].x;
        }
        else
        {
          *(u16 *) (((j << 6) + g_BtlShowRecords) + 6) = dx[j] - path[d].x;
        }
        blk = (j << 6) + g_BtlShowRecords;
        ((BtlRecord *)blk)->sprite.y = ty[j] + path[d].y;
        ((BtlRecord *)blk)->sprite.depth = (ty[j] + path[d].y) | 0x1000;
      }
      else if (d == total)
      {
        g_PadActuatorList = (s32 *) g_BtlActuator0ToggleList;
        g_PadActuatorRestFlags = 0x3A;
        g_PadActuatorListWait = 0;
        BtlShowSoundSet(((s16) (j % 2)) + 3, 0x4E, 0, 0);
        BtlAnimationSetCopy(g_BtlShowRecords + (j << 6), 2);
      }
      else if (d == (total + 0x20))
      {
        *(s16 *) ((j << 6) + g_BtlShowRecords) = 0;
      }
    }
    if (side)
    {
      BtlTileScrollMove(4, 0, g_BtlShowShakeOffsets[k]);
      base = g_BtlRecords;
      for (j = 0; j < 0xE; j++)
      {
        *((u16 *) (((j << 6) + base) + 0xA)) -= g_BtlShowShakeOffsets[k];
      }
      AS(u16, ((BtlRecord *)g_BtlShowRecords)[20].sprite.y) -= g_BtlShowShakeOffsets[k];
      k++;
      if (k >= 6)
      {
        side = 0;
      }
    }
    else if (((s16) (i % 10)) == 0)
    {
      side = 1;
      k = 0;
    }
    if (g_GameOptions[5] != 0)
    {
      g_PadActuator1Level &= 0x7F;
    }
    BtlShowFrameStep();
  }
  xmalloc_heap_free((s32) path);
  g_BtlShowSounds[3].state = 0;
  g_BtlShowSounds[2].state = 0;
  g_BtlShowSounds[1].state = 0;
  BtlAmountShowStart(1);
  BtlShakesRun(4, g_BtlShow2DLargeShakeOffsets);
  BtlShowPulseClear();
  while (layer_col_fade_check(0) & 0xFF)
  {
    BtlShowFrameStep();
  }
  layer_col_fade_start(0, 0xC, 1, 0xA0, 0);
  BtlShowSoundSet(1, 0x55, 0, 0);
  BtlShakesRun(4, g_BtlShow2DSmallShakeOffsets);
  BtlTileScrollSet(4, 8, 0);
  g_PadActuator1Level = 0;
  g_PadActuatorLevels = 0;
  while ((BtlAmountShowCheck() != 0) || (layer_col_fade_check(0) & 0xFF))
  {
    BtlShowFrameStep();
  }
  BtlShowActingThreeIdsEnd();
}

s32 arc_path_make(s32, s32, s32, s32, s32, s32, PathPoint *);

/* func_80084E78 */
void BtlShow2ERun(void)
{
  s16 win[3];
  s16 i;
  PathPoint *path;
  s32 d0;
  int effect_record;
  s32 d1;
  s32 d2;
  u8 *obj;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  BtlShowSoundSet(1, 0x3C, 0x1F, 0);
  BtlShowActingPlay(0, 1);
  BtlShowActingEnd();
  effect_record = g_BtlShowRecords + 0x500;
  *((BtlRecord *) g_BtlShowRecords) = *((BtlRecord *) effect_record);
  d0 = g_BtlShowRecords;
  *((s16 *) d0) = 1;
  if (((BtlRecord *)d0)->sprite.attr & 0x2000)
  {
    ((BtlRecord *)d0)->sprite.x = AS(u16, ((BtlRecord *)d0)->sprite.x) - 0x10;
  }
  else
  {
    ((BtlRecord *)d0)->sprite.x = AS(u16, ((BtlRecord *)d0)->sprite.x) + 0x10;
  }
  ((BtlRecord *)g_BtlShowRecords)->sprite.depth = 0x1000;
  path = (PathPoint *) xmalloc_heap_alloc(0xC0);
  obj = (u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords);
  arc_path_make(((BtlRecord *)g_BtlShowRecords)->sprite.x, ((BtlRecord *)g_BtlShowRecords)->sprite.y, ((BtlRecord *)obj)->sprite.x, ((BtlRecord *)obj)->sprite.y, 0x30, 0x40, path);
  BtlAnimationSetCopy(g_BtlShowRecords, 2);
  for (i = 0; i < 0x30; i++)
  {
    if (i == 6)
    {
      BtlAnimationSetCopy(g_BtlShowRecords, 2);
    }
    else
      if (i == 0x12)
    {
      BtlAnimationSetCopy(g_BtlShowRecords, 1);
    }
    else
      if (i == 0x1E)
    {
      ((BtlRecord *)g_BtlShowRecords)->sprite.attr |= 0x4000;
      BtlAnimationSetCopy(g_BtlShowRecords, 2);
    }
    else
      if (i == 0x2A)
    {
      BtlAnimationSetCopy(g_BtlShowRecords, 3);
    }
    d1 = g_BtlShowRecords;
    ((BtlRecord *)d1)->sprite.x = path[i].x;
    if (i < 0x1E)
    {
      ((BtlRecord *)d1)->sprite.y = path[i].y;
    }
    else
    {
      ((BtlRecord *)d1)->sprite.y = path[i].y - 0x10;
    }
    BtlShowFrameStep();
  }
  xmalloc_heap_free((s32) path);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  d2 = g_BtlShowRecords;
  ((BtlRecord *)d2)->sprite.depth = (*((u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xA))) + 1;
  ((BtlRecord *)d2)->sprite.attr &= 0xBFFF;
  win[0] = 0xA0;
  win[1] = 0x28;
  win[2] = 0;
  layer_col_set(0, 0xC, (s32) win);
  layer_col_fade_start(0, 0xC, 1, 0x20, 0);
  BtlAmountShowStart(1);
  BtlShowSoundSet(2, 0x5B, 0, 0);
  ((BtlRecord *)g_BtlShowRecords)->sprite.y = *((u16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 0xA));
  BtlShowAnimationPlay(g_BtlShowRecords, 4, 0);
  tile_grid_clear_copy(0, 1);
  while (BtlAmountShowCheck() != 0)
  {
    BtlShowFrameStep();
  }
}
/* far_syms: g_BtlRecords g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
extern u8 g_BtlShow2FFirstSpeeds[];
extern u8 g_BtlShow2FSecondSpeeds[];
void *memcpy(void *, const void *, u32);

/* func_80085194 */
void BtlShow2FRun(void)
{
  s16 spd1[10];
  s16 spd2[10];
  u16 i;
  u16 j;
  BtlRecord *o;

  memcpy(spd1, g_BtlShow2FFirstSpeeds, sizeof(spd1));
  memcpy(spd2, g_BtlShow2FSecondSpeeds, sizeof(spd2));
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x2200;
  BtlShowActingPlay(0, 0);
  g_BtlShowVoiceState = 1;
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  i = 0;
  do
  {
    *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
    o = (BtlRecord *) ((i << 6) + g_BtlShowRecords);
    o->sprite.x = 0x140;
    o->sprite.depth = 0x1000;
    o->on = 0;
    o->sprite.attr &= 0xDFFF;
    o->sprite.flags &= 0x9FFF;
    BtlAnimationSetCopy((s32) o, (u16) (i % 5 + 1));
    i++;
  } while (i < 10);
  g_PadActuatorList = (s32 *) g_BtlActuatorRandomList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlTilePatternPut(*g_BtlTilePatternList, 1);
  i = 0;
  BtlShowSoundSet(1, 0x61, 0, 0);
  layer_mix_fade_start(8, 1, 0x3C, 0xF);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    for (j = 0; j < 10; j++)
    {
      if ((u16) (i + j * -5) == 0)
      {
        ((BtlRecord *) g_BtlShowRecords)[j].on = 1;
        ((BtlRecord *) g_BtlShowRecords)[j].sprite.y = rng_mask_below(0xFF, 0xD0);
      }
      else if (((BtlRecord *) g_BtlShowRecords)[j].sprite.x < 0)
      {
        ((BtlRecord *) g_BtlShowRecords)[j].sprite.x = 0x140;
        ((BtlRecord *) g_BtlShowRecords)[j].sprite.y = rng_mask_below(0xFF, 0xD0);
      }
      else
      {
        ((BtlRecord *) g_BtlShowRecords)[j].sprite.x = (((BtlRecord *) g_BtlShowRecords)[j].sprite.x - spd1[j]) % 320;
      }
    }
    i++;
    BtlTileScrollStep(0xC, 0);
  }
  i = 0;
  do
  {
    for (j = 0; j < 10; j++)
    {
      if (((BtlRecord *) g_BtlShowRecords)[j].sprite.x < 0)
      {
        ((BtlRecord *) g_BtlShowRecords)[j].sprite.x = 0x140;
        ((BtlRecord *) g_BtlShowRecords)[j].sprite.y = rng_mask_below(0xFF, 0xD0);
      }
      else
      {
        ((BtlRecord *) g_BtlShowRecords)[j].sprite.x = (((BtlRecord *) g_BtlShowRecords)[j].sprite.x - spd2[j]) % 320;
      }
    }
    for (j = 6; j < 14; j++)
    {
      if (g_BtlSlotAmountKinds[j] == 1)
      {
        if ((u16) (i - 0x50) < 0x1E)
        {
          ((BtlRecord *) g_BtlRecords)[j].sprite.y -= 3;
          ((BtlRecord *) g_BtlRecords)[j].sprite.x -= 12;
          continue;
        }
        if (i & 1)
        {
          ((BtlRecord *) g_BtlRecords)[j].sprite.x -= 1;
        }
      }
      if (i & 1)
      {
        ((BtlRecord *) g_BtlRecords)[j].sprite.y += 1;
        ((BtlRecord *) g_BtlRecords)[j].sprite.x += 2;
      }
      else
      {
        ((BtlRecord *) g_BtlRecords)[j].sprite.y -= 1;
        ((BtlRecord *) g_BtlRecords)[j].sprite.x -= 2;
      }
    }
    BtlTileScrollStep(0x10, 0);
    i++;
  } while (i < 0xA0);
  for (j = 0; j < 14; j++)
  {
    if (g_BtlSlotAmountKinds[j] == 1)
    {
      ((BtlRecord *) g_BtlRecords)[j].on = 0;
    }
  }
  layer_mix_fade_start(8, 1, 0x78, 0x1F);
  while (layer_mix_fade_check(8) & 0xFF)
  {
    for (j = 0; j < 10; j++)
    {
      if (((BtlRecord *) g_BtlShowRecords)[j].on != 0)
      {
        if (((BtlRecord *) g_BtlShowRecords)[j].sprite.x < 0)
        {
          ((BtlRecord *) g_BtlShowRecords)[j].on = 0;
        }
        else
        {
          ((BtlRecord *) g_BtlShowRecords)[j].sprite.x = (((BtlRecord *) g_BtlShowRecords)[j].sprite.x - spd2[j]) % 320;
        }
      }
    }
    for (j = 6; j < 14; j++)
    {
      if (i & 1)
      {
        ((BtlRecord *) g_BtlRecords)[j].sprite.y += 1;
        ((BtlRecord *) g_BtlRecords)[j].sprite.x += 1;
      }
      else
      {
        ((BtlRecord *) g_BtlRecords)[j].sprite.y -= 1;
        ((BtlRecord *) g_BtlRecords)[j].sprite.x -= 1;
      }
    }
    i++;
    BtlTileScrollStep(0xD, 0);
  }
  BtlShowActingEnd();
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}
