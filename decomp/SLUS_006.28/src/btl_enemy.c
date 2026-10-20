/* Lunar SSSC — btl_enemy: the enemies' hit and special shows 0x9D to 0xBC
   (the rest of D_800B3070, which BtlActingShowRun runs), the helpers they
   share (BtlActingPlaceAnimationRun, BtlTargetActingRecordCopy,
   BtlActingPathMove) and the shot (BtlShotSet, BtlShotRun) BtlActingShowRun
   sets for each of them. The first split had it in field_engine.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "path.h"
#include "btl.h"
extern u8 g_BtlActuatorFFList[];
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern u8 g_BtlActingSlot;
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
extern BtlWalkBlock g_BtlWalkBlock;
extern u8 g_BtlSlotIds[];
extern u8 g_BtlSlotAmountKinds[];
s32 BtlAnimationSet(s32, s32);
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlFacingBothSet();
s32 BtlFrameStep();
s32 BtlTargetOutcomeShow();
s32 SoundPlay(s32, s32);  /* TU declares this below */
extern s32 CharStats;
extern s32 g_BtlRecords;
extern u8 g_BtlTargetSlot;
extern u8 g_BtlActionSound;

/* func_8003B4EC */
void BtlShow9DRun(void)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `walk_animation` only holds the constant 2; behaviour is the same with the constant written
     where it is read, but then 8 of this function's 128 instruction lines differ. */
  char walk_animation;
  s32 target_animation;
  s32 out_frames_done;
  s32 back_frames_done;
  s32 dx;
  s32 id;
  u8 cell_frames;
  void *record;
  void *end_record;
  void *out_frame_record;
  void *back_frame_record;
  cell_frames = *((s32 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x18));
  record = (g_BtlActingSlot << 6) + g_BtlRecords;
  ((BtlRecord *)record)->sprite.depth = (u16) (((BtlRecord *)record)->sprite.depth + 6);
  id = *(g_BtlActingSlot + g_BtlSlotIds);
  walk_animation = 2;
  if (id != 0x30)
  {
    if (((s32) id) < 0x31)
    {
      if (id != 7)
      {
      }
      else
      {
        goto block_5;
      }
    }
    else
      if (id == 0x5C)
    {
      block_5:
      cell_frames = 1;

    }
  }
  else
  {
    cell_frames = walk_animation;
  }
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), walk_animation);
  BtlWalkSet();
  g_BtlWalkBlock.speed = cell_frames;
  g_BtlWalkBlock.animation = walk_animation;
  BtlWalkRun();
  dx = -4;
  BtlFacingBothSet();
  if ((*((s16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) + 0x6))) < (*((s16 *) (((u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords)) + 0x6))))
  {
    dx = 4;
  }
  SoundPlay(g_BtlActionSound, 3);
  out_frames_done = 0;
  do
  {
    out_frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    out_frames_done += 1;
    ((BtlRecord *)out_frame_record)->sprite.x = (u16) (((BtlRecord *)out_frame_record)->sprite.x + dx);
    BtlFrameStep();
  }
  while (((u32) (out_frames_done & 0xFF)) < 6U);
  if (((u8) g_BtlTargetSlot) < 5U)
  {
    target_animation = 0x11;
    if ((*(g_BtlTargetSlot + g_BtlSlotAmountKinds)) == 1)
    {
      target_animation = 0x12;
    }
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), target_animation);
  }
  back_frames_done = 0;
  do
  {
    back_frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    back_frames_done += 1;
    ((BtlRecord *)back_frame_record)->sprite.x = (u16) (((BtlRecord *)back_frame_record)->sprite.x - dx);
    BtlFrameStep();
  }
  while (((u32) (back_frames_done & 0xFF)) < 6U);
  end_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  ((BtlRecord *)end_record)->sprite.depth = (u16) (((BtlRecord *)end_record)->sprite.depth - 6);
  BtlTargetOutcomeShow();
}
/* far_syms: D_800AE3C0 (target reaches it %hi/%lo) */
extern u8 D_800AE3C0[];
s32 BtlAnimationEndWait(void *);  /* TU declares this below */
s32 BtlPartyHitAnimationSet();
s32 BtlShowEnd();
s32 line_path_make(u16 *, s16, s16, s16, s32, s32);
s32 BtlAmountShowStart(s32);
extern u16 *g_LoadBuffer;

/* func_8003B730 */
void BtlShow9ERun(void)
{
  s32 unset_first_animation;
  s32 unset_out_point_count;
  s32 unset_back_point_count;
  s32 second_animation;
  u8 *depth_record;
  s32 unset_second_animation;
  s16 next_out_point;
  s16 next_back_point;
  s16 out_point;
  s16 back_point;
  s32 first_animation;
  s32 out_point_count;
  s32 back_point_count;
  u16 *out_path_y;
  u16 *back_path_y;
  u16 *out_path;
  u16 out_y;
  u16 *back_path;
  s32 out_x;
  s32 back_x;
  u16 kept_facing;
  u16 kept_x;
  u16 kept_y;
  u16 back_y;
  s32 id;
  void *acting_record;
  void *target_record;
  void *back_start_record;
  void *end_record;
  void *out_start_record;
  void *out_frame_record;
  void *back_frame_record;
  out_path = g_LoadBuffer;
  id = *(&(*(g_BtlActingSlot + g_BtlSlotIds)));
  if (id != 0x1B)
  {
    if (((s32) id) < 0x1C)
    {
      if (id != 7)
      {
      }
      else
      {
        goto block_5;
      }
    }
    else
      if (id == 0x5C)
    {
      block_5:
      first_animation = 8;

      back_point_count = unset_back_point_count;
      second_animation = 9;
      out_point_count = unset_out_point_count;
      out_point_count = 0x10;
      back_point_count = 0x40;
    }
  }
  else
  {
    first_animation = 9;
    second_animation = unset_second_animation;
    second_animation = 8;
    out_point_count = 0x13;
    back_point_count = 0x42;
  }
  acting_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  kept_x = ((BtlRecord *)acting_record)->sprite.x;
  kept_y = ((BtlRecord *)acting_record)->sprite.y;
  kept_facing = AS(s32, ((BtlRecord *)acting_record)->sprite.attr);
  out_point = 0;
  BtlFacingBothSet();
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), first_animation & 0xFFFF);
  BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
  out_start_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  target_record = (g_BtlTargetSlot << 6) + g_BtlRecords;
  line_path_make(out_path, ((BtlRecord *)out_start_record)->sprite.x, ((BtlRecord *)out_start_record)->sprite.y, ((BtlRecord *)target_record)->sprite.x, (s32) (((BtlRecord *)target_record)->sprite.y), (unsigned long) out_point_count);
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), second_animation & 0xFFFF);
  SoundPlay((u8) (g_BtlActionSound), 3);
  if (out_point_count > 0)
  {
    out_path_y = out_path + 1;
    do
    {
      out_x = *out_path;
      out_frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;
      ((BtlRecord *)out_frame_record)->sprite.x = out_x;
      out_path += 2;
      ((BtlRecord *)out_frame_record)->sprite.y = (u16) (*out_path_y);
      /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
         the block runs once, so behaviour is the same without the `do` / `while (0)`,
         but then 1 of this function's 182 instruction lines differs. */
      do
      {
        out_y = *out_path_y;
        out_path_y += 2;
        ((BtlRecord *)out_frame_record)->sprite.depth = (s16) (out_y + 1);
        BtlFrameStep(out_x);
      }
      while (0);
      next_out_point = out_point + 1;
      first_animation = unset_first_animation;
      out_point = next_out_point;
    }
    while (next_out_point < out_point_count);
  }
  g_PadActuatorList = D_800AE3C0;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  back_point = 0;
  BtlPartyHitAnimationSet();
  BtlAmountShowStart(0);
  back_path = g_LoadBuffer;
  back_start_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  line_path_make(back_path, ((BtlRecord *)back_start_record)->sprite.x, ((BtlRecord *)back_start_record)->sprite.y, (s16) kept_x, (s32) ((s16) kept_y), back_point_count);
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
  SoundPlay((u8) (g_BtlActionSound), 3);
  if (back_point_count > 0)
  {
    back_path_y = back_path + 1;
    do
    {
      back_x = *back_path;
      back_frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;
      ((BtlRecord *)back_frame_record)->sprite.x = back_x;
      back_path += 2;
      ((BtlRecord *)back_frame_record)->sprite.y = (u16) (*back_path_y);
      /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
         the block runs once, so behaviour is the same without the `do` / `while (0)`,
         but then 1 of this function's 182 instruction lines differs. */
      do
      {
        back_y = *back_path_y;
        back_path_y += 2;
        ((BtlRecord *)back_frame_record)->sprite.depth = (s16) (back_y - (-1));
        BtlFrameStep(back_x);
      }
      while (0);
      next_back_point = back_point + 1;
      back_point = next_back_point;
    }
    while (next_back_point < back_point_count);
  }
  end_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  ((BtlRecord *)end_record)->sprite.attr = kept_facing;
  depth_record = (u8 *) end_record;
  ((BtlRecord *)depth_record)->sprite.depth = (u16) (((BtlRecord *)depth_record)->sprite.y);
  BtlShowEnd();
}
/* far_syms: g_PadActuatorListWait */
/* far_syms: g_PadActuatorList */
/* far_syms: g_PadActuatorRestFlags */
/* far_syms: g_PadActuatorLevels */
/* far_syms: g_PadActuator1Level */
/* far_syms: g_BtlActuator41List */
/* far_syms: g_BtlSlotIds */
extern s32 g_BtlEffectBlock[];
extern u8 g_BtlSlotActiveFlags[];
s32 xmalloc_heap_alloc(s32);
extern u8 g_BtlActuatorFFTo45List[];
extern u8 g_BtlActuatorC8List[];
extern s32 g_BtlExtraRecords;
extern u8 g_BtlActuator41List[];
extern s8 g_PadActuatorLevels;
extern s8 g_PadActuator1Level;  /* TU declares this below */
s32 BtlExtraRecordsAlloc(s32);
s32 BtlExtraRecordsFree(s32);
s32 BtlFrameWait(s32);

/* func_8003BA6C */
void BtlShow9FRun(void)
{
  u16 lead_animation;
  u16 copy_animation;
  u16 away_animation;
  s32 dx;
  s32 dy;
  u8 n;
  u8 k;
  u8 i;
  BtlRecord *blk;
  u8 *obj;
  BtlExtraRecordsAlloc(0x15);
  lead_animation = 9;
  copy_animation = 0xA;
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
  away_animation = 0xB;
  switch (g_BtlSlotIds[g_BtlActingSlot])
  {
    case 0xE:
    case 0x6C:
      dx = 1;
      dy = -8;
      break;
    case 0x28:
      dx = 0x18;
      dy = -0x14;
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      break;
    case 0x40:
      BtlFrameWait(0x10);
      *(BtlRecord *) (g_BtlExtraRecords + 0x500) = *(BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords);
      dx = dy = 0;
      BtlAnimationSet(g_BtlExtraRecords + 0x500, 0xB);
      break;
    case 0x4F:
      dx = 0x10;
      dy = -6;
      lead_animation = 0xD;
      copy_animation = 0xE;
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 2);
      break;
  }
  g_PadActuatorList = (s32 *) g_BtlActuator41List;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  SoundPlay(g_BtlActionSound, 3);
  n = 0;
  *(BtlRecord *) g_BtlExtraRecords = *(BtlRecord *) ((g_BtlActingSlot << 6) + g_BtlRecords);
  BtlAnimationSet(g_BtlExtraRecords, lead_animation);
  blk = (BtlRecord *) g_BtlExtraRecords;
  AS(u16, blk->sprite.depth) += 2;
  AS(u16, blk->sprite.y) += dy;
  obj = (u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords);
  if (((BtlRecord *)obj)->sprite.attr & 0x2000)
  {
    blk->sprite.x = (AS(u16, ((BtlRecord *)obj)->sprite.x) - (((BtlRecord *)obj)->width >> 1)) - dx;
    while (((BtlRecord *)g_BtlExtraRecords)->sprite.x >= -0xF)
    {
      n++;
      BtlFrameStep();
      *(BtlRecord *) ((n << 6) + g_BtlExtraRecords) = *(BtlRecord *) g_BtlExtraRecords;
      BtlAnimationSet(g_BtlExtraRecords + (n << 6), copy_animation);
      ((BtlRecord *)g_BtlExtraRecords)->sprite.x -= 0x10;
    }
  }
  else
  {
    blk->sprite.x = dx + (AS(u16, ((BtlRecord *)obj)->sprite.x) + (((BtlRecord *)obj)->width >> 1));
    while (((BtlRecord *)g_BtlExtraRecords)->sprite.x < 0x150)
    {
      n++;
      BtlFrameStep();
      *(BtlRecord *) ((n << 6) + g_BtlExtraRecords) = *(BtlRecord *) g_BtlExtraRecords;
      BtlAnimationSet(g_BtlExtraRecords + (n << 6), copy_animation);
      ((BtlRecord *)g_BtlExtraRecords)->sprite.x += 0x10;
    }
  }
  *(s16 *) g_BtlExtraRecords = 0;
  BtlPartyHitAnimationSet();
  BtlAmountShowStart(1);
  for (k = 0x3C - n; k != 0; k--)
  {
    BtlFrameStep();
  }
  if (g_BtlSlotIds[g_BtlActingSlot] == 0x40)
  {
    ((BtlRecord *)g_BtlExtraRecords)[20].on = 0;
  }
  for (i = 1; i <= n; i++)
  {
    if (g_BtlSlotIds[g_BtlActingSlot] != 0x4F)
    {
      BtlAnimationSet(g_BtlExtraRecords + (i << 6), away_animation);
    }
    BtlFrameStep();
    *(s16 *) ((i << 6) + g_BtlExtraRecords) = 0;
  }
  g_PadActuatorList = 0;
  g_PadActuatorRestFlags = 0;
  g_PadActuatorListWait = 0;
  g_PadActuatorLevels = 0;
  g_PadActuator1Level = 0;
  BtlExtraRecordsFree(0x15);
  BtlShowEnd();
}

/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
s32 BtlAnimationSet(s32, s32);
s32 BtlFrameStep();
s32 SoundPlay(s32, s32);  /* TU declares this below */
extern s32 g_BtlRecords;
extern u8 g_BtlActionSound;

/* func_8003BFA0 */
void BtlShowA0Run(void)
{
  s16 off_x;
  void *record;
  void *frame_record;
  SoundPlay(g_BtlActionSound, 3);
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
  record = (g_BtlActingSlot << 6) + g_BtlRecords;
  ((BtlRecord *)record)->sprite.attr = 0x2020;
  off_x = ((s32) ((unsigned short) (AS(s32, ((BtlRecord *)record)->width)))) * (-1);
  if (off_x < ((BtlRecord *)record)->sprite.x)
  {
    do
    {
      frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;
      ((BtlRecord *)frame_record)->sprite.x = (u16) (((BtlRecord *)frame_record)->sprite.x - 8);
      BtlFrameStep();
    }
    while (off_x < (*((s16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) + 0x6))));
  }
  *((u16 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) = 0;
  SoundPlay(0U, 3);
}
/* far_syms: g_BtlActuatorC8List g_BtlActuatorToggleList g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlSlotRestAnimations g_BtlSlotFallMarks g_BtlWalkBlock */
extern u8 g_BtlActuatorRandomList[];

extern u8 g_BtlPartyHitAnimations[];
extern u8 g_BtlPartyStandAnimations[];
s32 BtlActingRestAnimationSet();                                  /* extern */
extern u8 g_BtlActuatorToggleList[];
extern u8 g_BtlSlotRestAnimations[];
extern u8 g_BtlSlotFallMarks[];
s32 BtlFallRun();
void BtlAmountShowWait(void);

/* Enemy action 0xA1 (field_state_table[3]) -- the drain. The caster
   (g_BtlActingSlot) walks up to the target (g_BtlTargetSlot), holds it for 0x50
   frames while the target flinches four times, then the knockback and the
   impact.

   1. BtlWalkSet fills the walk context g_BtlWalkBlock, which is then set:
      +0xA = 1 (shift the target column by half the attacker's width),
      +0xE = 3 (speed class), +9 = 0, +0xB = 0 (a point target), +0xF =
      0x64 (range in tiles). BtlWalkRun walks.
   2. The caster's depth (+8) is raised by one, BtlFacingBothSet(), and it
      takes anim 8.
   3. The vibration script g_BtlActuatorToggleList starts (g_PadActuatorRestFlags = 0x3F,
      g_PadActuatorListWait = 0).
   4. 0x50 frames (BtlFrameStep each), with a phase counter i beside the
      frame counter j; both start at 0 and step after the frame wait.
      BEFORE each frame's wait:
        i == 1     the action's sound g_BtlActionSound (SoundPlay(.., 3)) and
                   the target takes anim 0x12;
        i == 10    the target goes back to anim g_BtlSlotRestAnimations[target];
        i == 0x14  i = 0 (so the next frame is i == 1 again).
      That is a 20-frame cycle: the sound and anim 0x12 on frames 1, 21,
      41, 61; the anim back on frames 10, 30, 50, 70; frames 0, 20, 40, 60
      do nothing. The target's anim is set whether it is a party slot or
      not -- no `< 5` test here.
   5. The vibration script g_BtlActuatorC8List starts; BtlActingRestAnimationSet(); the caster's
      depth goes back down by one.
   6. A party target (slot < 5) takes anim 0x12; BtlTargetPushBack (the
      knockback); the impact BtlAmountShowStart(0); BtlAmountShowWait().
   7. A target with g_BtlSlotFallMarks[target] set goes through BtlFallRun;
      otherwise it takes anim g_BtlSlotRestAnimations[target].

   No RNG draw of its own (neither vibration script is g_BtlActuatorRandomList). Its own
   frames are the 0x50 of step 4; the rest are the callees'.

   Shape: i and j are u8 (masked at each test); `j = i = 0` is the chained
   assignment the target's `s1 = s0` copy comes from; BOTH increments sit
   in the for header (`j++, i++`) -- with i++ as the body's last statement
   it lands in the jal's delay slot and the back branch steals the loop
   top's andi instead (150/152). Each block's record pointer is its own
   variable. The second g_BtlTargetSlot read after the depth store is a byte
   load the store may alias; g_BtlRecords (a word) is not reloaded. */
/* func_8003C0A8 */
void BtlShowA1Run(void)
{
    u8 i;
    u8 j;
    BtlRecord *o;
    BtlRecord *o2;

    BtlWalkSet();
    g_BtlWalkBlock.walker_width_counts = 1;
    g_BtlWalkBlock.speed = 3;
    g_BtlWalkBlock.slots_block = 0;
    g_BtlWalkBlock.target_width_counts = 0;
    g_BtlWalkBlock.reach = 0x64;
    BtlWalkRun();
    o = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    o->sprite.depth++;
    BtlFacingBothSet();
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
    g_PadActuatorList = (s32 *)g_BtlActuatorToggleList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    for (j = i = 0; j < 0x50; j++, i++) {
        switch (i) {
        case 1:
            SoundPlay(g_BtlActionSound, 3);
            BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x12);
            break;
        case 10:
            BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlSlotRestAnimations[g_BtlTargetSlot]);
            break;
        case 0x14:
            i = 0;
            break;
        }
        BtlFrameStep();
    }
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlActingRestAnimationSet();
    o2 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    o2->sprite.depth--;
    if (g_BtlTargetSlot < 5) {
        BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x12);
    }
    BtlTargetPushBack();
    BtlAmountShowStart(0);
    BtlAmountShowWait();
    if (g_BtlSlotFallMarks[g_BtlTargetSlot] != 0) {
        BtlFallRun();
    } else {
        BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlSlotRestAnimations[g_BtlTargetSlot]);
    }
}
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlWalkBlock */
void BtlActingAnimationOutcomeRun(s32 animation, s32 wait_frames);

/* Enemy action 0xA2 (field_state_table[4]): the caster walks in and a
   copy of it, held in battle record 14, plays the strike.

   1. The walk context g_BtlWalkBlock is NOT refilled here (no BtlWalkSet):
      three bytes are set on whatever the last fill left -- +0xA = 0 (no
      half-width shift of the target column), +0xB = 0 (a point target),
      +0xC = 1 (must actually reach) -- and BtlWalkRun walks. The
      attacker / target, pose, speed and range are the earlier fill's.
   2. Record 14 of g_BtlRecords (the row after the eight enemy slots 6..13)
      becomes a whole copy of the caster's (g_BtlActingSlot) record; its first
      word is cleared (off), its depth (+8) raised by one, and it takes
      anim 9 (BtlAnimationSet).
   3. Record 14 is switched on (+0 = 1) and BtlActingAnimationOutcomeRun(8, 0xFF) runs:
      the caster's anim 8 with the action's sound, to the anim's last
      step, and the hit tail.
   4. Record 14 is switched off again. Nothing else is restored: the copy
      stays in the row, off.

   No frame wait and no RNG draw of its own; the frames are the walk's and
   BtlActingAnimationOutcomeRun's. No vibration script.

   Shape: nothing needed shaping. Record 14 is the array element
   ((BtlRecord *)g_BtlRecords)[14] at every use (0x380 / 0x388 off the reloaded
   base), the copy a struct assignment, and `f0 = 0; depth++;` in that
   order -- the scheduler puts the depth load first and its store in
   BtlAnimationSet's delay slot. */
/* func_8003C308 */
void BtlShowA2Run(void)
{
    g_BtlWalkBlock.walker_width_counts = 0;
    g_BtlWalkBlock.target_width_counts = 0;
    g_BtlWalkBlock.walk = 1;
    BtlWalkRun();
    ((BtlRecord *)g_BtlRecords)[14] = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s16, ((BtlRecord *)g_BtlRecords)[14].on) = 0;
    AS(u16, ((BtlRecord *)g_BtlRecords)[14].sprite.depth)++;
    BtlAnimationSet((s32)&((BtlRecord *)g_BtlRecords)[14], 9);
    AS(s16, ((BtlRecord *)g_BtlRecords)[14].on) = 1;
    BtlActingAnimationOutcomeRun(8, 0xFF);
    AS(s16, ((BtlRecord *)g_BtlRecords)[14].on) = 0;
}

/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlSlotIds g_BtlSlotAmountKinds g_BtlWalkBlock g_BtlActuatorC8List g_BtlActuatorFFTo45List */

/* Enemy action 0xA3: the actor runs over every flagged party slot. By the
   acting enemy's id (g_BtlSlotIds[g_BtlActingSlot]) it first plays anim 9 for ids
   0xA and 0x67 (and comes back through anim 0xA at the end) or anim 8 for
   id 0x22 (no closing anim, `back` 0xFF); any other caster plays nothing
   and reads `back` uninitialised. It keeps its attr word, waits for the
   anim to end and 0x3C frames more, plays the action's sound g_BtlActionSound
   and anim 8, and is lifted one px (y + 1). BtlWalkSet fills the walk
   context g_BtlWalkBlock, which is then set for speed class 1, range 0x64
   tiles, walk pose 8, no half-width shift, a point target, +9 clear and the
   action's sound as the walk's. For each party slot 0..4 flagged in
   g_BtlSlotAmountKinds the walk BtlWalkRun goes ONTO that slot's record, then
   vibration script g_BtlActuatorFFTo45List and the slot's anim 0x12. Last it walks home
   — to a stand-in record on the stack holding only the x / z it started
   from (y = z, 8 x 8), with +0xC = 4 and vibration script g_BtlActuatorC8List — the
   attr word and y are put back, the closing anim plays to its end, and the
   impact runs. No RNG draw of its own. */
/* func_8003C3D8 */
void BtlShowA3Run(void)
{
    BtlRecord a;
    u16 back;
    u16 attr;
    s16 x;
    s16 z;
    BtlRecord *p;
    BtlRecord *q;
    BtlRecord *r;

    switch (g_BtlSlotIds[g_BtlActingSlot]) {
    case 0xA:
    case 0x67:
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
        back = 0xA;
        break;
    case 0x22:
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
        back = 0xFF;
        break;
    }
    p = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    attr = p->sprite.attr;
    BtlAnimationEndWait(p);
    BtlFrameWait(0x3C);
    SoundPlay(g_BtlActionSound, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
    ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.depth += 1;
    BtlWalkSet();
    g_BtlWalkBlock.speed = 1;
    g_BtlWalkBlock.slots_block = 0;
    g_BtlWalkBlock.walker_width_counts = 0;
    g_BtlWalkBlock.target_width_counts = 0;
    g_BtlWalkBlock.reach = 0x64;
    g_BtlWalkBlock.animation = 8;
    g_BtlWalkBlock.sound = g_BtlActionSound;
    q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    x = q->sprite.x;
    z = q->sprite.y;
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++) {
        if (g_BtlSlotAmountKinds[g_BtlTargetSlot] != 0) {
            g_BtlWalkBlock.target = g_BtlRecords + (g_BtlTargetSlot << 6);
            BtlWalkRun();
            g_PadActuatorList = (s32 *)g_BtlActuatorFFTo45List;
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x12);
        }
    }
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    a.width = a.height = 8;
    a.sprite.x = x;
    a.sprite.depth = z;
    a.sprite.y = z;
    g_BtlWalkBlock.walk = 4;
    g_BtlWalkBlock.target = (s32)&a;
    BtlWalkRun();
    r = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    r->sprite.attr = attr;
    r->sprite.depth -= 1;
    if (back != 0xFF) {
        BtlAnimationSet((s32)r, back);
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    }
    BtlAmountShowStart(0);
    BtlShowEnd();
}

s32 BtlAnimationEndWait(void *);                            /* extern */
s32 BtlFrameWait(s32);                                 /* extern */

/* func_8003C6E4 */
void BtlShowA4Run(void) {
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0x14);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    ((BtlRecord *)g_BtlRecords)[7].on = 1;
    SoundPlay(0x68, 3);
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0xA);
    BtlAnimationEndWait(g_BtlRecords + 0x1C0);
    BtlFrameWait(0xA);
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0xB);
    BtlActingRestAnimationSet();
}
/* far_syms: g_BtlActuatorC8List g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlSlotIds g_BtlSlotAmountKinds g_BtlSlotRestAnimations g_BtlSlotFallMarks */
extern u8 g_BtlSlotRestAnimations[];
extern u8 g_BtlSlotFallMarks[];
s32 BtlFallRun();
void BtlAmountShowWait(void);

/* Enemy action 0xA5 (field_state_table[7]) -- the vanish. The caster
   (g_BtlActingSlot) plays anim `out` and reappears beside the target
   (g_BtlTargetSlot), strikes, and goes home through BtlActingPlaceAnimationRun.
   out / in are anims 9 / 0xA, or 0xA / 0xB for caster id 0x4A
   (g_BtlSlotIds[caster]).

   1. The caster's x and y0 are saved; it faces the target: attr (+0xC) =
      0x20 when its x is below the target's, else 0x2020.
   2. Sound 0x68 on channel 3, anim `out`, wait for the anim's end
      (BtlAnimationEndWait).
   3. It is moved: x = the target's x - 2 when attr is 0x20, + 2
      otherwise; y0 (+0xA) = the target's y0 - 0x10; depth (+8) = the
      target's depth + 1.
   4. Sound 0x68, anim `in`, wait for its end, then anim 2.
   5. By the target's result byte g_BtlSlotAmountKinds[target]:
      - 2 (a miss): 0x10 frames (BtlFrameWait); a party target (slot < 5)
        takes anim 0x11; BtlTargetBackMove (the step back); wait for the
        caster's anim; sound 0x68, anim `out`; 0x10 frames;
        BtlTargetForwardMove (the step forward again); wait; home.
      - anything else (a hit): the vibration script g_BtlActuatorC8List starts
        (g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0); wait for the caster's anim; a
        party target takes anim 0x12; BtlAmountShowStart(0); 7 frames; sound
        0x68, anim `out`; BtlTargetPushBack (the knockback); wait; home; then
        BtlAmountShowWait.
      Home is BtlActingPlaceAnimationRun(saved x, saved y0, in).
   6. A target with g_BtlSlotFallMarks[target] set goes through BtlFallRun;
      otherwise it takes anim g_BtlSlotRestAnimations[target].

   No RNG draw of its own. Its frames are the callees' (BtlAnimationEndWait,
   BtlFrameWait, BtlTargetBackMove / BtlTargetForwardMove, BtlTargetPushBack,
   BtlAmountShowWait, the sub), each through the frame wait BtlFrameStep.

   Shape: `in` is an int masked `& 0xFFFF` at each use (as BtlShow9ERun
   does); as a u16 the caster-0x4A arm loads 0xA again instead of copying
   it from `in`'s register. Each block's record pointer is its own
   variable -- one shared pointer takes one register for the whole
   function. In step 3 the caster's pointer is formed before the
   target's. */
/* func_8003C79C */
void BtlShowA5Run(void)
{
    s32 out;
    s32 in;
    u16 x;
    u16 y0;
    BtlRecord *o;
    BtlRecord *o2;
    BtlRecord *o3;
    BtlRecord *t;

    out = 9;
    in = 0xA;
    if (g_BtlSlotIds[g_BtlActingSlot] == 0x4A) {
        out = 0xA;
        in = 0xB;
    }
    o = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    x = o->sprite.x;
    y0 = o->sprite.y;
    if (o->sprite.x < ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x) {
        o->sprite.attr = 0x20;
    } else {
        o->sprite.attr = 0x2020;
    }
    SoundPlay(0x68, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), out);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    o2 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    if (o2->sprite.attr == 0x20) {
        o2->sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x - 2;
    } else {
        o2->sprite.x = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x + 2;
    }
    o3 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    o3->sprite.y = t->sprite.y - 0x10;
    o3->sprite.depth = t->sprite.depth + 1;
    SoundPlay(0x68, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), in & 0xFFFF);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 2);
    if (g_BtlSlotAmountKinds[g_BtlTargetSlot] == 2) {
        BtlFrameWait(0x10);
        if (g_BtlTargetSlot < 5) {
            BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x11);
        }
        BtlTargetBackMove();
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
        SoundPlay(0x68, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), out);
        BtlFrameWait(0x10);
        BtlTargetForwardMove();
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
        BtlActingPlaceAnimationRun((s16)x, (s16)y0, in & 0xFFFF);
    } else {
        g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
        if (g_BtlTargetSlot < 5) {
            BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x12);
        }
        BtlAmountShowStart(0);
        BtlFrameWait(7);
        SoundPlay(0x68, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), out);
        BtlTargetPushBack();
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
        BtlActingPlaceAnimationRun((s16)x, (s16)y0, in & 0xFFFF);
        BtlAmountShowWait();
    }
    if (g_BtlSlotFallMarks[g_BtlTargetSlot] != 0) {
        BtlFallRun();
    } else {
        BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlSlotRestAnimations[g_BtlTargetSlot]);
    }
}
/* The vanish's way home (BtlShowA5Run, both arms): the caster
   (g_BtlActingSlot) is put back at its saved place -- x (+6) = x, and both
   depth (+8) and y0 (+0xA) = y0 -- with sound 0x68 on channel 3, then
   plays anim `anim`, waits for its end (BtlAnimationEndWait) and goes to rest
   (BtlActingRestAnimationSet).

   An old-style definition on purpose: BtlShowA5Run above calls it with
   no prototype in scope, and its code only comes out right that way (a
   prototype whose third parameter is u16 changes 14 of its instructions),
   while this body needs `u16 anim` (an s32 masked at the call schedules
   the $s0 save two rows early).  The bytes do not decide the width of x
   and y0; s16 is what the caller sign-extends them to. */
/* func_8003CBE4 */
void BtlActingPlaceAnimationRun(x, y0, anim)
s16 x;
s16 y0;
u16 anim;
{
    u8 *actor;

    actor = (u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords);
    ((BtlRecord *)actor)->sprite.x = x;
    ((BtlRecord *)actor)->sprite.depth = y0;
    ((BtlRecord *)actor)->sprite.y = y0;
    SoundPlay(0x68, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), anim);
    BtlAnimationEndWait((void *) (g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlActingRestAnimationSet();
}

/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlSlotIds */
extern s32 g_BtlExtraRecords;

/* func_8003CC7C */
void BtlShowA6Run(void)
{
    PathPoint pts[80];
    s32 o;
    BtlRecord *q;
    PathPoint *p;
    s32 a, b, c;
    s16 dx;
    s16 dz;
    u16 anim;
    s16 n;
    s16 i;
    u8 done;
    u16 last;

    BtlExtraRecordsAlloc(1);
    a = g_BtlActingSlot;
    a <<= 6;
    b = g_BtlRecords;
    o = a + b;
    c = g_BtlTargetSlot;
    c <<= 6;
    if (((BtlRecord *)o)->sprite.x < ((BtlRecord *)(c + b))->sprite.x) {
        o += 0xC;
        *(s16 *)o = 0x20;
    } else {
        o += 0xC;
        *(s16 *)o = 0x2020;
    }
    *(BtlRecord *)g_BtlExtraRecords = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    dz = 0;
    AS(s16, ((BtlRecord *)g_BtlExtraRecords)->on) = 0;
    dx = 0;
    switch (g_BtlSlotIds[g_BtlActingSlot]) {
    case 0x3B:
        dx = 0x20;
        dz = 0x32;
        anim = 2;
        break;
    case 0x30:
        dz = 2;
        dx = 2;
        anim = 8;
        break;
    }
    BtlAnimationSet(g_BtlExtraRecords, 0xB);
    {
        BtlRecord *tt = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
        BtlRecord *qq = (BtlRecord *)g_BtlExtraRecords;
        if (qq->sprite.x < tt->sprite.x) {
            qq->sprite.x += dx;
            n = tt->sprite.x - qq->sprite.x;
        } else {
            qq->sprite.x -= dx;
            n = qq->sprite.x - tt->sprite.x;
        }
    }
    {
        BtlRecord *qq = (BtlRecord *)g_BtlExtraRecords;
        BtlRecord *tt;
        qq->sprite.y -= dz;
        tt = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
        if (qq->sprite.y < tt->sprite.y) {
            i = tt->sprite.y - qq->sprite.y;
        } else {
            i = qq->sprite.y - tt->sprite.y;
        }
    }
    if (n < i) {
        n = i;
    }
    n = n / 8;
    if (n == 0) {
        n = 1;
    }
    {
        BtlRecord *tt = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
        BtlRecord *qq = (BtlRecord *)g_BtlExtraRecords;
        line_path_make((u16 *)pts, qq->sprite.x, qq->sprite.y, tt->sprite.x, tt->sprite.y, n);
    }
    SoundPlay(0x6E, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
    if (g_BtlSlotIds[g_BtlActingSlot] == 0x3B) {
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    } else {
        BtlFrameWait(0xA);
    }
    done = 0;
    p = pts;
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    last = (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->end & 0x7FFF) - 1;
    AS(s16, ((BtlRecord *)g_BtlExtraRecords)->on) = 1;
    for (i = 0; i < n; i++, p++) {
        q = (BtlRecord *)g_BtlExtraRecords;
        q->sprite.x = p->x;
        q->sprite.y = p->y;
        q->sprite.depth = p->y + dz + 6;
        BtlFrameStep();
        if (!done) {
            b = (g_BtlActingSlot << 6) + g_BtlRecords;
            if (((BtlRecord *)b)->step == last) {
                BtlAnimationSet(b, anim);
                done = 1;
            }
        }
    }
    SoundPlay(0x23, 3);
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(0);
    if (g_BtlSlotIds[g_BtlActingSlot] == 0x3B) {
        BtlRecord *o3 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        if (o3->sprite.attr & 0x2000) {
            i = o3->sprite.x - 0x20;
        } else {
            i = o3->sprite.x + 0x20;
        }
        line_path_make((u16 *)pts, i, ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y - 0x2E, ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x, ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y, n);
    }
    p = &pts[n - 1];
    for (i = n - 1; i >= 0; i--) {
        q = (BtlRecord *)g_BtlExtraRecords;
        q->sprite.x = p->x;
        q->sprite.y = p->y;
        q->sprite.depth = p->y + 1;
        BtlFrameStep();
        p--;
    }
    BtlExtraRecordsFree(1);
    if (g_BtlSlotIds[g_BtlActingSlot] == 0x3B) {
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }
    BtlShowEnd();
}
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlSlotIds g_BtlWalkBlock */

/* Enemy action 0xA7 (field_state_table[9]): step in if the target is not
   already adjacent, then the swing.

   1. The walk anim, context byte g_BtlWalkBlock.animation: 1 when the caster's id
      (g_BtlSlotIds[g_BtlActingSlot]) is 0x12, else 0xB. The same test picks the
      sound: 0x6C for id 0x12, else 0x6D.
   2. The measure: g_BtlWalkBlock.walk = 0 (do not move) and BtlWalkRun().
      Its result 2 -- a wide target the caster already stands next to;
      BtlWalkRun has turned the caster to face it by then -- skips step
      3. Any other result (0 out of reach, 1 within reach or on the same
      cell) takes it.
   3. The sound (SoundPlay(snd, 3)), the caster takes anim 9 and plays
      it to its end (BtlAnimationEndWait); g_BtlWalkBlock.walk = 1 and BtlWalkRun()
      again, which now walks (on the anim of step 1); the sound again, anim
      0xA to its end.
   4. BtlActingAnimationOutcomeRun(2, 0xFF) -- the swing on anim 2 and the hit tail.

   BtlWalkSet is NOT called here: only context bytes +0xC and +0xD are
   written, so the attacker, target, speed, range and the other flags are
   whatever the last fill left in g_BtlWalkBlock.

   No RNG draw and no frame wait of its own; no vibration script. Frames
   are the callees' (the two anim waits, the walk, BtlActingAnimationOutcomeRun).

   Shape: nothing needed shaping. `snd` is a u16 (the one `andi 0xFFFF`,
   in the measure's branch delay slot, then two plain copies); the
   context base is a pointer local set after the if/else, so it lives in
   $s1 across the calls, while the +0xD stores before it go straight
   through the symbol (one shared `lui`). The record address is spelled
   out at each of its four uses. BtlActingAnimationOutcomeRun is called undeclared, as
   the TU has no prototype for it above this point. */
/* func_8003D228 */
void BtlShowA7Run(void)
{
    u16 snd;
    BtlWalkBlock *ctx;

    if (g_BtlSlotIds[g_BtlActingSlot] == 0x12) {
        g_BtlWalkBlock.animation = 1;
        snd = 0x6C;
    } else {
        g_BtlWalkBlock.animation = 0xB;
        snd = 0x6D;
    }
    ctx = &g_BtlWalkBlock;
    ctx->walk = 0;
    if ((u8)BtlWalkRun() != 2) {
        SoundPlay(snd, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
        ctx->walk = 1;
        BtlWalkRun();
        SoundPlay(snd, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    }
    BtlActingAnimationOutcomeRun(2, 0xFF);
}

/* far_syms: g_BtlPartyHitAnimations g_BtlPartyStandAnimations g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_BtlSlotRestAnimations */
extern u8 g_BtlExtraRecordCount;
s32 sprite_run_off(s32, s32);

/* BtlShowA8Run -- BATTLE code: enemy action 0xA8 (field_state_table[10]),
   the transformation -- the target is turned into a copy of the caster.

   1. BtlWalkEdgeRun(1), BtlFacingBothSet() (the usual opening), sound 0x5F
      (SoundPlay(0x5F, 3)); the caster (g_BtlActingSlot) takes anim 0xA and
      plays it to its end (BtlAnimationEndWait), then takes anim 0x1D;
      BtlFrameWait(0x1E) waits 0x1E frames.
   2. `obj`, a 64-byte copy of the caster's record, becomes the extra
      object (g_BtlExtraRecordCount = 1, g_BtlExtraRecords = &obj) and is placed ON the
      target (g_BtlTargetSlot): x (+6) and y0 (+0xA) the target's, depth (+8) =
      that y0 + 1, and the mirror bit 0x2000 of its +0xC word FLIPPED. It
      takes anim 0x15; 0x1E frames; sound 0x6D; 7 frames.
   3. The target's first word (+0) = 0 -- hidden -- and the copy's anim is
      waited out (BtlAnimationEndWait(&obj)); g_BtlExtraRecordCount = 0;
      sprite_run_off(0x1C, 1) frees the extra object's sprite slot.
   4. BtlTargetActingRecordCopy() (asm): the target's record becomes the caster's whole
      record, keeping its own x, y0 (written to both +8 and +0xA), +0xC
      word and +0x3C / +0x3E.
   5. The target takes anim 0x16; its hit anim g_BtlPartyHitAnimations[target] = 0x17,
      its rest anim g_BtlPartyStandAnimations[target] = 0x16, and the anim it returns to,
      g_BtlSlotRestAnimations[target], = that rest anim (read back from g_BtlPartyStandAnimations).
   6. The caster takes anim 0x1E and plays it to its end.

   Nothing here undoes it: no saved copy of the target's record outlives
   BtlTargetActingRecordCopy. No RNG draw of its own and no vibration script; every
   frame is a callee's (two anim waits, 0x1E + 0x1E + 7 frames, the copy's
   anim wait, the caster's last anim wait).

   Shape: the record copy is a struct assignment (the struct needs an s32
   member for the word loop). The placement is written x, y0, y, attr:
   the y0 load through `t` may alias the earlier stores to `obj` (both are
   in-struct), so it can only be scheduled after the x store and BEFORE
   the attr store -- with the attr update written before it, the attr
   store pins the load behind it (137/146). `obj.y = obj.y0 + 1` re-reads
   the field just stored (cse forwards it); `t->y0 + 1` loads it twice
   (one instruction more). The target's hide store is spelled out without
   a pointer variable and lands in BtlAnimationEndWait's delay slot. */
/* func_8003D354 */
void BtlShowA8Run(void)
{
    BtlRecord obj;
    BtlRecord *t;

    BtlWalkEdgeRun(1);
    BtlFacingBothSet();
    SoundPlay(0x5F, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0x1D);
    BtlFrameWait(0x1E);
    g_BtlExtraRecordCount = 1;
    g_BtlExtraRecords = (s32)&obj;
    obj = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    obj.sprite.x = t->sprite.x;
    obj.sprite.y = t->sprite.y;
    obj.sprite.depth = obj.sprite.y + 1;
    obj.sprite.attr ^= 0x2000;
    BtlAnimationSet((s32)&obj, 0x15);
    BtlFrameWait(0x1E);
    SoundPlay(0x6D, 3);
    BtlFrameWait(7);
    ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->on = 0;
    BtlAnimationEndWait(&obj);
    g_BtlExtraRecordCount = 0;
    sprite_run_off(0x1C, 1);
    BtlTargetActingRecordCopy();
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x16);
    g_BtlPartyHitAnimations[g_BtlTargetSlot] = 0x17;
    g_BtlPartyStandAnimations[g_BtlTargetSlot] = 0x16;
    g_BtlSlotRestAnimations[g_BtlTargetSlot] = g_BtlPartyStandAnimations[g_BtlTargetSlot];
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0x1E);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
}

/* far_syms: g_BtlActingSlot g_BtlRecords g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlActuatorC8List */
extern u8 g_BtlExtraRecordCount;

/* Enemy action 0xA9 (g_BtlActingShowFunctions[18]): the leap. One scratch record
   (BtlExtraRecordsAlloc(1)) takes a copy of the actor's whole record and plays
   anim 0x19. Sound 0x74, the actor's anim 0xC to its end (BtlAnimationEndWait),
   SoundPlay(0, 3), sound 0x66, anim 0xD, effect trio g_BtlActuatorC8List / 0x3F
   / 0. The actor's x and z (+0x0A) are kept aside, the scratch record is
   hidden (active = 0), the actor's y (+0x08) goes up by one, and it runs
   off the screen 8 px a frame (BtlFrameStep) the way it faces: attr &
   0x2000 -> left until x <= -(its u16 +0x3C) - 0x10, otherwise right until
   x >= (its u16 +0x3C) + 0x150 (both bounds s16). SoundPlay(0, 3) and
   0x3C frames (BtlFrameWait). It is put back at its own x with z = 0 and
   y = the kept z; the scratch record is shown (active = 1), BtlPartyHitAnimationSet,
   the impact BtlAmountShowStart(1), anim 0xE, g_BtlExtraRecordCount = 1, and z climbs 4 a
   frame until it reaches the kept value. BtlExtraRecordsFree(1), BtlShowEnd.
   No RNG draw of its own. */
/* func_8003D59C */
void BtlShowA9Run(void)
{
    BtlRecord *p;
    s16 sx;
    s16 sz;
    s16 lim;

    BtlExtraRecordsAlloc(1);
    *(BtlRecord *)g_BtlExtraRecords = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    BtlAnimationSet(g_BtlExtraRecords, 0x19);
    SoundPlay(0x74, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xC);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    SoundPlay(0, 3);
    SoundPlay(0x66, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xD);
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    p = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    sx = p->sprite.x;
    sz = p->sprite.y;
    AS(s16, ((BtlRecord *)g_BtlExtraRecords)->on) = 0;
    p->sprite.depth++;
    if (p->sprite.attr & 0x2000) {
        lim = -(u16)AS(s32, p->width) - 0x10;
        while (lim < ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x) {
            BtlRecord *q;
            BtlFrameStep();
            q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
            q->sprite.x -= 8;
        }
    } else {
        lim = (u16)AS(s32, p->width) + 0x150;
        while (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x < lim) {
            BtlRecord *q;
            BtlFrameStep();
            q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
            q->sprite.x += 8;
        }
    }
    SoundPlay(0, 3);
    BtlFrameWait(0x3C);
    {
        BtlRecord *q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        q->sprite.x = sx;
        q->sprite.y = 0;
        q->sprite.depth = sz;
    }
    AS(s16, ((BtlRecord *)g_BtlExtraRecords)->on) = 1;
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(1);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xE);
    g_BtlExtraRecordCount = 1;
    while (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y < sz) {
        BtlRecord *q;
        BtlFrameStep();
        q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        q->sprite.y += 4;
    }
    BtlExtraRecordsFree(1);
    BtlShowEnd();
}

/* far_syms: g_BtlActuatorFFList (target reaches it %hi/%lo) */

s32 BtlMarkedRestAnimationSet();                                  /* extern */
s32 BtlFallRun();                                  /* extern */
s32 BtlActingTargetLeftMove(s32);                                 /* extern */
s32 BtlAmountShowStart(s32);                                 /* extern */
s32 BtlAmountShowCheck();                                /* extern */

/* func_8003D8D8 */
void BtlShowAARun(void) {
    s32 last_step;
    s32 rest_pending;
    void *record;
    void *end_record;

    BtlActingTargetLeftMove(0x68);
    rest_pending = 1;
    BtlFacingBothSet();
    record = (g_BtlActingSlot << 6) + g_BtlRecords;
    ((BtlRecord *)record)->sprite.depth = (u16) (((BtlRecord *)record)->sprite.depth + 1);
    BtlAnimationSet(record, 9);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    SoundPlay(0x51, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
    BtlFrameWait(0xA);
    g_PadActuatorList = g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x12);
    BtlFrameWait(0x14);
    BtlAmountShowStart(1);
    BtlFrameWait(0x14);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
    last_step = (((*(s16 *)((u8 *)((g_BtlActingSlot << 6) + g_BtlRecords) + 0x3a)) & 0x7FFF) - 1) & 0xFFFF;
loop_1:
    if (BtlAmountShowCheck() != 0) {
        BtlFrameStep();
        if ((rest_pending & 0xFF) && ((*(u16 *)((u8 *)((g_BtlActingSlot << 6) + g_BtlRecords) + 0x38)) == last_step)) {
            rest_pending = 0;
            BtlActingRestAnimationSet();
        }
        goto loop_1;
    }
    BtlFallRun();
    BtlMarkedRestAnimationSet();
    if (rest_pending & 0xFF) {
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }
    end_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    ((BtlRecord *)end_record)->sprite.depth = (u16) (((BtlRecord *)end_record)->sprite.depth - 1);
}
/* far_syms: g_BtlActuatorC8List g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */

/* Enemy action 0xAB: a copy of the caster, held in battle record 14,
   strikes ON the target while the caster stays where it stands.

   1. The caster (g_BtlActingSlot) faces the target (g_BtlTargetSlot): its +0xC word
      = 0x20 when its x is below the target's, else 0x2020.
   2. Sound 0x6E (SoundPlay(0x6E, 3)); the caster takes anim 8 and
      plays it to its end (BtlAnimationEndWait).
   3. Record 14 of g_BtlRecords becomes a whole copy of the caster's record
      -- facing word and on flag included -- placed on the target: x and
      y0 (+0xA) the target's, depth (+8) = that y0 + 6. It takes anim 9,
      then its first word = 1 (on).
   4. BtlFrameWait(0x14): 20 frames. Vibration script g_BtlActuatorC8List
      (g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0), sound 0x5A, BtlFrameWait(0xA):
      10 frames, BtlPartyHitAnimationSet() (the flagged party slots flinch).
   5. Record 14's anim is waited out and it is switched off (+0 = 0); the
      impact BtlAmountShowStart(1); BtlShowEnd() (the closing calls).

   No walk: the caster never leaves its spot. No frame wait and no RNG
   draw of its own (g_BtlActuatorC8List has no random motion); the frames are the
   two anim waits and the 20 + 10 of BtlFrameWait.

   Shape: nothing needed shaping. The caster's pointer is a local for the
   facing test only; record 14 is the array element
   ((BtlRecord *)g_BtlRecords)[14] at every use, the target read through its
   own pointer after the copy, and the depth re-reads the y0 just stored
   (`[14].depth = [14].y0 + 6`). y0 and depth are u16 (lhu / plain
   addiu). */
/* func_8003DAEC */
void BtlShowABRun(void)
{
    BtlRecord *c;
    BtlRecord *t;

    c = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    if (c->sprite.x < ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x) {
        c->sprite.attr = 0x20;
    } else {
        c->sprite.attr = 0x2020;
    }
    SoundPlay(0x6E, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    ((BtlRecord *)g_BtlRecords)[14] = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    ((BtlRecord *)g_BtlRecords)[14].sprite.x = t->sprite.x;
    AS(u16, ((BtlRecord *)g_BtlRecords)[14].sprite.y) = AS(u16, t->sprite.y);
    AS(u16, ((BtlRecord *)g_BtlRecords)[14].sprite.depth) = AS(u16, ((BtlRecord *)g_BtlRecords)[14].sprite.y) + 6;
    BtlAnimationSet((s32)&((BtlRecord *)g_BtlRecords)[14], 9);
    AS(s16, ((BtlRecord *)g_BtlRecords)[14].on) = 1;
    BtlFrameWait(0x14);
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x5A, 3);
    BtlFrameWait(0xA);
    BtlPartyHitAnimationSet();
    BtlAnimationEndWait(&((BtlRecord *)g_BtlRecords)[14]);
    AS(s16, ((BtlRecord *)g_BtlRecords)[14].on) = 0;
    BtlAmountShowStart(1);
    BtlShowEnd();
}

/* far_syms: g_BtlActingSlot g_BtlRecords g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlActuatorFFList g_BtlActuatorC8List */
s32 rng_mask_below(s32, s32);

/* func_8003DCA0 */
void BtlShowACRun(void)
{
    u8 i;
    BtlRecord *p;
    BtlRecord *q;
    BtlRecord *r;
    BtlRecord *s;
    s32 v;

    /* Enemy action 0xAC (g_BtlActingShowFunctions[21]): six copies of the acting actor.
       BtlExtraRecordsAlloc(6), the copy count g_BtlExtraRecordCount = 0, then the actor's
       record is copied into the six records at g_BtlExtraRecords. Copies 0 and 1
       stand on actor records 0 and 1 (x and z theirs, y = z + 1); copies
       2..5 get a drawn spot each, x = rng_mask_below(0xFF, 0xA0) and
       z = 0x70 + rng_mask_below(0x1F, 0x20), y = z + 1 -- eight draws, x
       before z -- and are then spread one per screen quarter: copy 3
       x + 0xA0, copy 4 y and z + 0x28, copy 5 both. Sound 0x6C, the actor
       plays 9 to its end and then 8, BtlFrameWait(0x1E), sound 0x74,
       effect trio g_BtlActuatorFFList, all six copies play 0xB, g_BtlExtraRecordCount = 6,
       BtlFrameWait(0x1E), effect trio g_BtlActuatorC8List, the impact
       BtlAmountShowStart(1), copy 0's anim to its end, BtlExtraRecordsFree(6), sound
       0x6C, the actor plays 0xA to its end, BtlShowEnd. */
    BtlExtraRecordsAlloc(6);
    g_BtlExtraRecordCount = 0;
    for (i = 0; i < 2; i++) {
        *(BtlRecord *)((i << 6) + g_BtlExtraRecords) = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        p = (BtlRecord *)((i << 6) + g_BtlRecords);
        q = (BtlRecord *)((i << 6) + g_BtlExtraRecords);
        q->sprite.x = p->sprite.x;
        q->sprite.y = p->sprite.y;
        q->sprite.depth = p->sprite.y + 1;
    }
    for (i = 2; i < 6; i++) {
        *(BtlRecord *)((i << 6) + g_BtlExtraRecords) = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        ((BtlRecord *)((i << 6) + g_BtlExtraRecords))->sprite.x = rng_mask_below(0xFF, 0xA0);
        v = rng_mask_below(0x1F, 0x20);
        r = (BtlRecord *)((i << 6) + g_BtlExtraRecords);
        r->sprite.y = v + 0x70;
        r->sprite.depth = v + 0x71;
    }
    s = (BtlRecord *)g_BtlExtraRecords;
    s[3].sprite.x += 0xA0;
    s[4].sprite.y += 0x28;
    s[4].sprite.depth += 0x28;
    s[5].sprite.y += 0x28;
    s[5].sprite.depth += 0x28;
    s[5].sprite.x += 0xA0;
    SoundPlay(0x6C, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
    BtlFrameWait(0x1E);
    SoundPlay(0x74, 3);
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    for (i = 0; i < 6; i++) {
        BtlAnimationSet(g_BtlExtraRecords + (i << 6), 0xB);
    }
    g_BtlExtraRecordCount = 6;
    BtlFrameWait(0x1E);
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlAmountShowStart(1);
    BtlAnimationEndWait((void *)g_BtlExtraRecords);
    BtlExtraRecordsFree(6);
    SoundPlay(0x6C, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlShowEnd();
}
/* far_syms: g_BtlActuatorFFList g_BtlActuatorFFTwiceList g_BtlRecords g_BtlActingSlot g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlShotBlock g_BtlEffectBlock */
extern u8 g_BtlActuatorFFTwiceList[];
extern s8 g_BtlShotBlock[];
s32 xmalloc_heap_free(s32);
void BtlEffectLoad(u8 *blk, u8 id);

/* BtlShowADRun -- BATTLE code: enemy action 0xAD -- the caster is swapped
   for a stand-in block, fires one shot, and a second copy of the block
   plays on the target.

   1. BtlEffectLoad(&a, 0x5B) loads effect block 0x5B and binds the stack record `a`
      to it; BtlWalkEdgeRun(1), BtlFacingBothSet() (the usual opening).
   2. Vibration script g_BtlActuatorFFTwiceList; a.active = 1.
   3. The caster's record (g_BtlActingSlot) is saved in `b` and REPLACED by `a`,
      then given the saved x (+6), the saved z (+0xA) as both +8 and +0xA,
      and the saved attr word (+0xC); it takes anim 7.
   4. `a` -- still the block's record -- is placed on the target
      (g_BtlTargetSlot): x and z the target's, +8 = the target's z + 1.
   5. The shot block g_BtlShotBlock (BtlShotRun reads it; b3..b7 keep
      BtlShotSet's defaults): b0 = 9 (the caster's launch anim), b1 =
      0x2F (the wait after it), b2 = 0x10 (the shot's anim), muzzle offset
      h8 / hA = 0xE / -0x22. BtlShotRun() flies the shot.
   6. Vibration script g_BtlActuatorFFList; `a` takes anim 0 and becomes the extra
      object (g_BtlExtraRecords = &a, g_BtlExtraRecordCount = 1); sound 0x5B
      (SoundPlay(0x5B, 3)); 0x1E frames (BtlFrameWait); the impact
      BtlAmountShowStart(1); `a`'s anim to its end (BtlAnimationEndWait);
      g_BtlExtraRecordCount = 0.
   7. BtlFallRun(), BtlMarkedRestAnimationSet(), BtlAmountShowWait(); the caster's own
      record goes back from `b`; BtlActingRestAnimationSet(); xmalloc_heap_free frees the
      block BtlEffectLoad loaded (g_BtlEffectBlock[0]).

   No RNG draw of its own (neither vibration script uses the random
   motion); every frame is a callee's.

   Shape: the shot block's address is a pointer local set before the first
   call ($s0 across BtlEffectLoad .. BtlAnimationSet); `&a` held in a second pointer
   local from the g_BtlExtraRecords store on ($s0 again, across three calls) --
   the BtlAnimationSet(&a, 0) just before it takes the address directly.
   The two caster stores of the saved z are one chained assignment
   (+8 stored first, then +0xA, from one load). a.y re-reads the target's
   z (the a.z store may alias it). Struct assignments for the three
   64-byte copies (the struct has s32 members: the word loop). */
/* func_8003DFE4 */
void BtlShowADRun(void)
{
    BtlRecord a;
    BtlRecord b;
    BtlShotBlock *p;
    BtlRecord *q;
    BtlRecord *t;
    BtlRecord *o;

    p = (BtlShotBlock *)g_BtlShotBlock;
    BtlEffectLoad((u8 *)&a, 0x5B);
    BtlWalkEdgeRun(1);
    BtlFacingBothSet();
    g_PadActuatorList = (s32 *)g_BtlActuatorFFTwiceList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    AS(s16, a.on) = 1;
    b = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = a;
    q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    q->sprite.x = b.sprite.x;
    q->sprite.y = q->sprite.depth = b.sprite.y;
    q->sprite.attr = b.sprite.attr;
    BtlAnimationSet((s32)q, 7);
    t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    a.sprite.x = t->sprite.x;
    a.sprite.y = t->sprite.y;
    a.sprite.depth = t->sprite.y + 1;
    p->from_animation = 9;
    p->animation = 0x10;
    p->from_wait = 0x2F;
    p->dx = 0xE;
    p->dy = -0x22;
    BtlShotRun();
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlAnimationSet((s32)&a, 0);
    o = &a;
    g_BtlExtraRecords = (s32)o;
    g_BtlExtraRecordCount = 1;
    SoundPlay(0x5B, 3);
    BtlFrameWait(0x1E);
    BtlAmountShowStart(1);
    BtlAnimationEndWait(o);
    g_BtlExtraRecordCount = 0;
    BtlFallRun();
    BtlMarkedRestAnimationSet();
    BtlAmountShowWait();
    *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = b;
    BtlActingRestAnimationSet();
    xmalloc_heap_free(g_BtlEffectBlock[0]);
}

/* far_syms: g_BtlActuatorC8List g_BtlRecords g_BtlSlotIds g_BtlSlotAmountKinds g_BtlActingSlot g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
s32 BtlFacingNearestSet();

/* func_8003E274 */
void BtlShowAERun(void)
{
    u16 anim;
    s32 step;
    s16 x0;
    s16 z0;
    s16 left;
    s16 right;
    BtlRecord *p;
    BtlRecord *q;
    BtlRecord *a;
    BtlRecord *e;
    s16 *px;
    u8 dir;

    /* Enemy action 0xAE, the charge (Tank Golem, ids 0x43 / 0x75). The
       caster plays its run animation (8 / 0xC) lifted 7 px, then leaves the
       screen the way it faces at `step` px a frame: left to minus half its
       width (+0x3C), right to 0x140 plus half its width. It then makes one
       full-screen pass for every flagged party slot (g_BtlSlotAmountKinds[i]), on
       that slot's z: vibration script g_BtlActuatorC8List, the caster's sound, the
       mirror bit set for the direction, and the slot's anim 0x12 once the
       pass is over; each pass runs the opposite way to the one before.
       Last, with sound 0 it comes back to its saved x on its saved z, and
       the impact runs. `anim` and `step` are set for ids 0x43 and 0x75
       only: any other caster would read them uninitialised. */
    switch (g_BtlSlotIds[g_BtlActingSlot]) {
    case 0x43:
        anim = 8;
        step = 0x10;
        break;
    case 0x75:
        anim = 0xC;
        step = 0x10;
        break;
    }
    p = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    x0 = p->sprite.x;
    z0 = p->sprite.y;
    p->sprite.depth += 7;
    BtlAnimationSet((s32)p, anim);
    q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    right = (q->width >> 1) + 0x140;
    left = -(q->width >> 1);
    SoundPlay(g_BtlActionSound, 3);
    px = &q->sprite.x;
    if (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.attr & 0x2000) {
        while (*px > left) {
            BtlFrameStep();
            *px -= step;
        }
        dir = 1;
    } else {
        while (*px < right) {
            BtlFrameStep();
            *px += step;
        }
        dir = 0;
    }
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++) {
        if (g_BtlSlotAmountKinds[g_BtlTargetSlot] != 0) {
            g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            SoundPlay(g_BtlActionSound, 3);
            a = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
            a->sprite.y = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y;
            a->sprite.depth = a->sprite.y + 7;
            if (dir) {
                a->sprite.attr = 0x20;
                while (*px < right) {
                    BtlFrameStep();
                    *px += step;
                }
                dir = 0;
            } else {
                a->sprite.attr = 0x2020;
                while (*px > left) {
                    BtlFrameStep();
                    *px -= step;
                }
                dir = 1;
            }
            BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x12);
        }
    }
    SoundPlay(0, 3);
    e = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    e->sprite.depth = z0;
    e->sprite.y = z0;
    if (dir) {
        e->sprite.attr = 0x20;
        while (*px < x0) {
            BtlFrameStep();
            *px += step;
        }
    } else {
        e->sprite.attr = 0x2020;
        while (*px > x0) {
            BtlFrameStep();
            *px -= step;
        }
    }
    BtlAmountShowStart(0);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlFacingNearestSet();
    BtlShowEnd();
}

/* far_syms: g_BtlWalkBlock g_BtlSlotIds g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_BtlActionKind g_BtlNumber */
s32 sprite_run_off(s32, s32);
extern u8 g_BtlActionKind;
extern u8 g_BtlNumber;
s32 xmalloc_heap_free(s32);
void BtlEffectLoad(u8 *blk, u8 id);

/* func_8003E6CC */
void BtlShowAFRun(void)
{
    BtlRecord a;
    BtlRecord b;
    u16 anim;
    u16 m1;
    u16 m2;
    BtlRecord *p;
    BtlRecord *q;

    /* An enemy's strike-with-effect command, picked by the acting enemy's
       id (g_BtlSlotIds[g_BtlActingSlot]): ids 0x39/0x48/0x59, 0x79, 0x7C, 0x7D and
       0x7F choose the three animations (wind-up, strike, the effect passed
       to BtlActingAnimationOutcomeRun) and the hit kind g_BtlWalkBlock.animation; 0x7C/0x7D also
       widen the actor (+0x20 at +0x3C) and load a stand-in record with
       BtlEffectLoad. The actor turns to its target, the hit check runs, and on a
       connect the wind-up and strike play (with actor 7's 0x15/0x16/0xB
       overlay when g_BtlNumber is 0xF7). For g_BtlNumber 0xFE/0xF7 the
       stand-in takes the actor's place (same position and mirror bit) for
       the effect, then the saved record goes back, narrowed again. */
    BtlWalkSet();
    g_BtlWalkBlock.slots_block = 0;
    g_BtlWalkBlock.reach = 0x64;
    switch (g_BtlSlotIds[g_BtlActingSlot]) {
    case 0x39:
    case 0x48:
    case 0x59:
        anim = 8;
        if (g_BtlActionKind == 2) {
            anim = 2;
        }
        m1 = 9;
        m2 = 0xA;
        g_BtlWalkBlock.animation = 0xB;
        g_BtlWalkBlock.slots_block = 1;
        break;
    case 0x79:
        anim = 2;
        m1 = 1;
        g_BtlWalkBlock.animation = 0xD;
        m2 = 0xB;
        break;
    case 0x7D:
        anim = 0;
        m1 = 6;
        m2 = 7;
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->width += 0x20;
        g_BtlWalkBlock.animation = 1;
        BtlEffectLoad((u8 *)&a, 0x5E);
        break;
    case 0x7C:
        anim = 0;
        m1 = 8;
        m2 = 9;
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->width += 0x20;
        g_BtlWalkBlock.animation = 1;
        BtlEffectLoad((u8 *)&a, 0x61);
        break;
    case 0x7F:
        m1 = 0xB;
        g_BtlWalkBlock.animation = 0x10;
        m2 = 0xC;
        anim = 2;
        break;
    }
    p = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    if (p->sprite.x < ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x) {
        p->sprite.attr = 0x20;
    } else {
        p->sprite.attr = 0x2020;
    }
    g_BtlWalkBlock.walk = 0;
    if ((u8)BtlWalkRun() != 2) {
        g_BtlWalkBlock.walk = 1;
        if (g_BtlNumber == 0xF7) {
            BtlAnimationSet(g_BtlRecords + 0x1C0, 0x15);
        }
        SoundPlay(0x68, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), m1);
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
        g_BtlWalkBlock.speed = 1;
        BtlWalkRun();
        SoundPlay(0x68, 3);
        if (g_BtlNumber == 0xF7) {
            q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
            ((BtlRecord *)g_BtlRecords)[7].sprite.x = q->sprite.x;
            /* CODEGEN FORCING DEVICE (AS of the field's own type), not recovered
               source: record 7's y is an s16 and the AS reads it as one, so
               behaviour is the same as the plain member; but as the plain
               member, the inner store of the chained assignment goes through
               record 7's address formed in a register (the one
               BtlAnimationSet's argument then takes), and 1 of this
               function's 322 instruction lines differs. */
            ((BtlRecord *)g_BtlRecords)[7].sprite.depth = AS(s16, ((BtlRecord *)g_BtlRecords)[7].sprite.y) = q->sprite.y;
            BtlAnimationSet(g_BtlRecords + 0x1C0, 0x16);
            BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), m2);
            BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
            BtlAnimationSet(g_BtlRecords + 0x1C0, 0xB);
        } else {
            BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), m2);
            BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
        }
    }
    if (g_BtlNumber == 0xFE || g_BtlNumber == 0xF7) {
        b = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        if (b.sprite.attr & 0x2000) {
            a.sprite.attr |= 0x2000;
        } else {
            a.sprite.attr &= 0xDFFF;
        }
        AS(s16, a.on) = 1;
        a.sprite.x = b.sprite.x;
        a.sprite.depth = b.sprite.y;
        a.sprite.y = b.sprite.y;
        *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = a;
        BtlActingAnimationOutcomeRun(anim, 0x3C);
        *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = b;
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->width -= 0x20;
        xmalloc_heap_free(g_BtlEffectBlock[0]);
    } else {
        BtlActingAnimationOutcomeRun(anim, 0xFF);
    }
}
/* far_syms: g_BtlActuatorC8List g_BtlActuatorFFTo45List g_BtlRecords g_BtlActingSlot g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlShotBlock */
extern s8 g_BtlShotBlock[];

/* BtlShowB0Run -- BATTLE code: enemy action 0xB0, three shots with a copy
   of the caster flashed on the target after each.

   1. BtlWalkEdgeRun(1), BtlFacingBothSet() (the usual opening); the caster
      (g_BtlActingSlot) takes anim 9 and plays it to its end (BtlAnimationEndWait).
   2. The shot block g_BtlShotBlock (BtlShotRun reads it; b0 / b3 / b4 keep
      BtlShotSet's defaults): b1 = 8 (the wait after the caster's launch
      anim), b2 = 0xD (the shot's anim), muzzle offset h8 / hA = -0xC / -7,
      sounds b5 = 0x6E (before the caster's anim), b6 = 0 (none at launch),
      b7 = 0x5A (on arrival).
   3. `obj` is a 64-byte copy of the caster's record placed ON the target
      (g_BtlTargetSlot): x and y0 (+6 / +0xA) the target's, depth (+8) the
      target's + 1.
   4. Three times: vibration script g_BtlActuatorFFTo45List, the caster's +0x30 = 0,
      the shot (BtlShotRun); then the copy becomes the extra object
      (g_BtlExtraRecords = &obj, g_BtlExtraRecordCount = 1) with its +0x30 = 0 and anim 0xE,
      BtlPartyHitAnimationSet() on the FIRST pass only, vibration script g_BtlActuatorC8List,
      the copy's anim to its end (BtlAnimationEndWait), g_BtlExtraRecordCount = 0.
   5. sprite_run_off(0x1C, 1) frees the shot's sprite slot, the impact
      BtlAmountShowStart(1), and the caster takes anim 0xA. While
      BtlAmountShowCheck() is non-zero a frame passes (BtlFrameStep); on the
      frame the caster's step (+0x38) equals its last one ((+0x3A & 0x7FFF)
      - 1) BtlActingRestAnimationSet() runs, once.
   6. BtlFallRun(), BtlMarkedRestAnimationSet(); if that last step never came, the
      caster's anim is waited out (BtlAnimationEndWait).

   No RNG draw of its own (neither vibration script uses the random
   motion); every frame is a callee's.

   Shape: the block's address is a pointer local set before the first call
   (the target keeps it in $s0 across four calls); `i` and `run` are u8
   (masked at each test) and both land in $s0 after it; `last` is a u16.
   The record copy is a struct assignment (the struct needs an s32 member
   for the word loop). Struct-typed accesses throughout. */
/* func_8003EBD4 */
void BtlShowB0Run(void)
{
    BtlRecord obj;
    BtlShotBlock *p;
    BtlRecord *t;
    u8 i;
    u8 run;
    u16 last;

    p = (BtlShotBlock *)g_BtlShotBlock;
    BtlWalkEdgeRun(1);
    BtlFacingBothSet();
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    p->from_wait = 8;
    p->animation = 0xD;
    p->dx = -0xC;
    p->dy = -7;
    p->from_sound = 0x6E;
    p->launch_sound = 0;
    p->arrive_sound = 0x5A;
    obj = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    obj.sprite.x = t->sprite.x;
    obj.sprite.y = t->sprite.y;
    obj.sprite.depth = t->sprite.depth + 1;
    for (i = 0; i < 3; i++) {
        g_PadActuatorList = (s32 *)g_BtlActuatorFFTo45List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->animation = 0;
        BtlShotRun();
        g_BtlExtraRecords = (s32)&obj;
        g_BtlExtraRecordCount = 1;
        obj.animation = 0;
        BtlAnimationSet((s32)&obj, 0xE);
        if (i == 0) {
            BtlPartyHitAnimationSet();
        }
        g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        BtlAnimationEndWait(&obj);
        g_BtlExtraRecordCount = 0;
    }
    sprite_run_off(0x1C, 1);
    BtlAmountShowStart(1);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
    run = 1;
    last = (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->end & 0x7FFF) - 1;
    while (BtlAmountShowCheck() != 0) {
        BtlFrameStep();
        if (run != 0) {
            if (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->step == last) {
                BtlActingRestAnimationSet();
                run = 0;
            }
        }
    }
    BtlFallRun();
    BtlMarkedRestAnimationSet();
    if (run != 0) {
        BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    }
}

/* far_syms: g_BtlShotBlock (target reaches it %hi/%lo) */
extern s8 g_BtlShotBlock[];

/* func_8003EEC4 */
void BtlShowB1Run(void) {
    BtlWalkEdgeRun(1);
    BtlFacingBothSet();
    g_BtlShotBlock[0] = 2;
    g_BtlShotBlock[2] = 0xA;
    ((s16 *)g_BtlShotBlock)[4] = -0x1E;
    ((s16 *)g_BtlShotBlock)[5] = -0x13;
    g_BtlShotBlock[5] = 0x6A;
    g_BtlShotBlock[6] = 0;
    BtlShotRun();
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
    BtlTargetOutcomeShow();
}
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_BtlNumber g_BtlEffectBlock g_BtlActuatorC8List */

/* func_8003EF4C */
void BtlShowB2Run(void)
{
    BtlRecord a;
    BtlRecord b;
    BtlRecord *p;
    BtlRecord *q;
    BtlRecord *r;
    s16 t;

    /* An enemy command played through a stand-in record: BtlEffectLoad loads it
       (id 0x5C when g_BtlNumber is 0xFE, else 0x5F), the acting actor's own
       record is saved in b, and the stand-in takes its place with the
       actor's position and mirror bit. Actor 7 plays 0x15 and the actor 0;
       then the actor is put beside its target g_BtlTargetSlot, on the side it
       came from (x one short, z + 1, y = z + 2), plays 1, the flagged
       targets react (BtlPartyHitAnimationSet) and the impact starts
       (BtlAmountShowStart(1)). It goes back to the saved position, actor 7 plays
       0x16 and the actor 2, actor 7 goes to 0xB, and the saved record is
       restored. */
    BtlFacingBothSet();
    if (g_BtlNumber == 0xFE) {
        BtlEffectLoad((u8 *)&a, 0x5C);
    } else {
        BtlEffectLoad((u8 *)&a, 0x5F);
    }
    b = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s16, a.on) = 1;
    a.sprite.x = ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x;
    a.sprite.y = a.sprite.depth = ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y;
    if (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.attr & 0x2000) {
        a.sprite.attr |= 0x2000;
    } else {
        a.sprite.attr &= 0xDFFF;
    }
    *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = a;
    SoundPlay(0x68, 3);
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0x15);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlFrameWait(0x1E);
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x5C, 3);
    p = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    q = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    if (p->sprite.x < q->sprite.x) {
        p->sprite.x = q->sprite.x - 1;
    } else {
        p->sprite.x = q->sprite.x + 1;
    }
    r = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    t = ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y;
    r->sprite.y = t + 1;
    r->sprite.depth = t + 2;
    BtlAnimationSet((s32)r, 1);
    BtlFrameWait(0x14);
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(1);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlFrameWait(0x1E);
    ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x = b.sprite.x;
    ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.depth = ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y = b.sprite.y;
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0x16);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 2);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0xB);
    *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords) = b;
    BtlShowEnd();
    xmalloc_heap_free(g_BtlEffectBlock[0]);
}
/* far_syms: g_BtlActuatorFFTwiceList g_BtlRecords g_BtlActingSlot g_BtlNumber g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlEffectBlock */

/* BtlShowB3Run -- BATTLE code: enemy action 0xB3. Two records of an
   effect block, 0x3A px apart, play on the caster's spot while the caster
   itself is hidden; then the impact.

   1. BtlExtraRecordsAlloc(2) gets room for two 0x40 records at g_BtlExtraRecords. BtlEffectLoad
      loads effect block 0x60 -- 0x5D when the encounter g_BtlNumber is
      0xFE -- and binds record 0 to it.
   2. Record 0 takes the caster's (g_BtlActingSlot) x (+6) and z (+0xA), with
      depth (+8) = that z + 1. Record 1 is a copy of record 0.
   3. Mirror: when the caster's attr (+0xC) has 0x2000, record 0 gets the
      bit and record 1's x is 0x3A LESS; otherwise record 0 loses the bit
      and record 1's x is 0x3A MORE. Record 1 then takes record 0's attr.
   4. Record 0 is switched on (+0 = 1), the caster off (+0 = 0); record 0
      plays anim 0 to its end (BtlAnimationEndWait).
   5. Vibration script g_BtlActuatorFFTwiceList, sound 0x66 (SoundPlay(0x66, 3));
      record 1 is switched on and takes anim 3, record 0 anim 1; 0x32
      frames (BtlFrameWait); BtlPartyHitAnimationSet(); the impact
      BtlAmountShowStart(1); record 1's anim to its end.
   6. Record 0 takes anim 2, BtlAmountShowWait(), that anim to its end;
      BtlExtraRecordsFree(2) gives the two records back; the caster is switched
      on again; BtlActingRestAnimationSet(), BtlFallRun(), BtlMarkedRestAnimationSet(), and
      xmalloc_heap_free frees the block BtlEffectLoad loaded (g_BtlEffectBlock[0]).

   No RNG draw of its own (g_BtlActuatorFFTwiceList has no random motion); every frame
   is a callee's. */
/* func_8003F2EC */
void BtlShowB3Run(void)
{
    BtlExtraRecordsAlloc(2);
    if (g_BtlNumber == 0xFE) {
        BtlEffectLoad((u8 *)g_BtlExtraRecords, 0x5D);
    } else {
        BtlEffectLoad((u8 *)g_BtlExtraRecords, 0x60);
    }
    ((BtlRecord *)g_BtlExtraRecords)->sprite.x = ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x;
    ((BtlRecord *)g_BtlExtraRecords)->sprite.y = ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y;
    ((BtlRecord *)g_BtlExtraRecords)->sprite.depth = ((BtlRecord *)g_BtlExtraRecords)->sprite.y + 1;
    ((BtlRecord *)g_BtlExtraRecords)[1] = ((BtlRecord *)g_BtlExtraRecords)[0];
    if (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.attr & 0x2000) {
        ((BtlRecord *)g_BtlExtraRecords)->sprite.attr |= 0x2000;
        ((BtlRecord *)g_BtlExtraRecords)[1].sprite.x -= 0x3A;
    } else {
        ((BtlRecord *)g_BtlExtraRecords)->sprite.attr &= 0xDFFF;
        ((BtlRecord *)g_BtlExtraRecords)[1].sprite.x += 0x3A;
    }
    ((BtlRecord *)g_BtlExtraRecords)[1].sprite.attr = ((BtlRecord *)g_BtlExtraRecords)->sprite.attr;
    AS(s16, ((BtlRecord *)g_BtlExtraRecords)->on) = 1;
    AS(s16, ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->on) = 0;
    BtlAnimationSet(g_BtlExtraRecords, 0);
    BtlAnimationEndWait((void *)g_BtlExtraRecords);
    g_PadActuatorList = (s32 *)g_BtlActuatorFFTwiceList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x66, 3);
    AS(s16, ((BtlRecord *)g_BtlExtraRecords)[1].on) = 1;
    BtlAnimationSet(g_BtlExtraRecords + 0x40, 3);
    BtlAnimationSet(g_BtlExtraRecords, 1);
    BtlFrameWait(0x32);
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(1);
    BtlAnimationEndWait((void *)(g_BtlExtraRecords + 0x40));
    BtlAnimationSet(g_BtlExtraRecords, 2);
    BtlAmountShowWait();
    BtlAnimationEndWait((void *)g_BtlExtraRecords);
    BtlExtraRecordsFree(2);
    AS(s16, ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->on) = 1;
    BtlActingRestAnimationSet();
    BtlFallRun();
    BtlMarkedRestAnimationSet();
    xmalloc_heap_free(g_BtlEffectBlock[0]);
}

/* far_syms: g_BtlActuatorC8List g_BtlActuatorFFList g_BtlRecords g_BtlActingSlot g_BtlTargetSlot g_BtlActionNumber g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
extern u8 g_BtlActionNumber;

/* func_8003F530 */
void BtlShowB4Run(void)
{
    s32 step;
    s16 z;
    BtlRecord *p;
    BtlRecord *q;
    BtlRecord *r;

    /* Enemy action 0xB4, the tile strike (battle.md: Ultra Fiend). Vibration
       script g_BtlActuatorC8List and sound 7; the caster (g_BtlActingSlot) plays anim 0xB
       to its end, then rises off the top of the screen: its y (+0xA) drops
       `step` = 8 px a frame, one frame wait per step, until it is negative.
       It is switched off (first word 0) for 0x64 frames. Sound 8; it is
       switched back on with anim 8 and placed above the marked tile: x =
       g_BtlActionNumber * 8, depth (+8) = g_BtlTargetSlot * 8 + 1, y = -8 -- the tile's
       column is g_BtlActionNumber and its row is the TARGET INDEX byte g_BtlTargetSlot,
       used here as a coordinate. It then falls `step` px a frame while y is
       below depth - 1, i.e. until y == g_BtlTargetSlot * 8. Vibration script
       g_BtlActuatorFFList and sound 0x5A, anim 0xC to its end, BtlFallRun().
       Nothing puts the caster back: x, depth and y stay the tile's.

       Shape: each site forms its own record pointer (p, q, r). The fall
       loop reads y ONCE per pass into the s16 `z`, for both the test and
       the add (the target's lh + move); the rise loop tests and updates
       the field directly (lh for the test, lhu for the -=). The fall is a
       for (;;) with its exit in the middle, entered past its loads: the
       pointer q of the placement and the loop's own r are two variables --
       that is the target's `addu v0,v0,v1; move a0,v0` pair. */
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(7, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xB);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    step = 8;
    while (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y >= 0) {
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y -= step;
        BtlFrameStep();
    }
    AS(s16, ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->on) = 0;
    BtlFrameWait(0x64);
    SoundPlay(8, 3);
    p = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s16, p->on) = 1;
    BtlAnimationSet((s32)p, 8);
    q = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    q->sprite.x = g_BtlActionNumber << 3;
    q->sprite.depth = (g_BtlTargetSlot << 3) + 1;
    q->sprite.y = -8;
    for (;;) {
        r = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        z = r->sprite.y;
        if (z >= r->sprite.depth - 1) {
            break;
        }
        r->sprite.y = z + step;
        BtlFrameStep();
    }
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x5A, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xC);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlFallRun();
}

/* far_syms: g_BtlActuatorFFTo45List (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyHitAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyStandAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */
extern long HWD0, VWD0;
extern u8 g_BtlExtraRecordCount;  /* TU declares this below */

/* func_8003F784 */
void BtlShowB5Run(void)
{
  BtlRecord local;
  u16 stand_animation;
  u16 *target_y;
  u16 hit_animation;
  BtlWalkEdgeRun(1);
  BtlFacingBothSet();
  ((BtlShotBlock *)g_BtlShotBlock)->from_animation = 0xD;
  ((BtlShotBlock *)g_BtlShotBlock)->dx = -0x2D;
  ((BtlShotBlock *)g_BtlShotBlock)->dy = 0x10;
  ((BtlShotBlock *)g_BtlShotBlock)->from_wait = 0x5F;
  ((BtlShotBlock *)g_BtlShotBlock)->length = 0xF;
  ((BtlShotBlock *)g_BtlShotBlock)->from_sound = 0x50;
  ((BtlShotBlock *)g_BtlShotBlock)->arrive_sound = 0x54;
  BtlShotRun();
  g_BtlExtraRecordCount = 1;
  g_BtlExtraRecords = (s32) &local;
  memcpy(&local, (s32 *) ((g_BtlActingSlot << 6) + g_BtlRecords), 0x40);
  local.sprite.x = ((BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x;
  local.sprite.y = ((BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y;
  local.sprite.depth = (*(target_y = (u16 *) &((BtlRecord *) ((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.y)) + 1;
  BtlAnimationSet((s32) &local, 0xE);
  BtlAnimationEndWait(&local);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFTo45List;
  g_PadActuatorRestFlags = 0x3F;
  g_BtlExtraRecordCount = (g_PadActuatorListWait = 0);
  sprite_run_off(0x1C, 1);
  BtlTargetActingRecordCopy();
  switch (g_BtlSlotIds[g_BtlTargetSlot])
  {
    case 1:
      stand_animation = 0xF;
      hit_animation = 0x17;
      break;
    case 6:
      stand_animation = 0x10;
      hit_animation = 0x18;
      break;
    case 2:
      stand_animation = 0x11;
      hit_animation = 0x19;
      break;
    case 7:
      stand_animation = 0x12;
      hit_animation = 0x1A;
      break;
  }
  BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), stand_animation);
  g_BtlPartyHitAnimations[g_BtlTargetSlot] = (u8) hit_animation;
  g_BtlPartyStandAnimations[g_BtlTargetSlot] = (u8) stand_animation;
  g_BtlSlotRestAnimations[g_BtlTargetSlot] = g_BtlPartyStandAnimations[g_BtlTargetSlot];
}
/* far_syms: g_BtlRecords g_BtlActingSlot g_BtlTargetSlot */

/* The transformation's record swap, called by enemy actions 0xA8
   (BtlShowA8Run) and 0xB5 (BtlShowB5Run) once the copy on the target
   has played: the target (g_BtlTargetSlot) BECOMES the caster (g_BtlActingSlot).

   1. `saved` = the target's whole 0x40-byte battle record.
   2. The target's record = a whole copy of the caster's record (its
      on / off word, anim state and sprite included).
   3. Six halfwords of the target's own come back from `saved`: x (+6),
      y0 (+0xA), which is written to BOTH the depth (+8) and y0 (+0xA),
      so the saved depth is not kept, the +0xC word (the mirror bit with
      it), and the width / height (+0x3C / +0x3E).

   Nothing else: no call, no frame, no RNG draw, no sound. `saved` does
   not outlive the function, so nothing can undo the swap.

   Shape: three struct assignments (word loops) and one pointer local for
   the six stores. The double store is the chained assignment
   `t->y0 = t->depth = saved.y0;` -- one load, depth stored first. Two
   statements load saved.y0 twice (the store to t->depth may alias it:
   one instruction and a nop more). */
/* func_8003F9D4 */
void BtlTargetActingRecordCopy(void)
{
    BtlRecord saved;
    BtlRecord *t;

    saved = *(BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    *(BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords) = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    t->sprite.x = saved.sprite.x;
    t->sprite.y = AS(u16, t->sprite.depth) = saved.sprite.y;
    t->sprite.attr = saved.sprite.attr;
    t->width = saved.width;
    t->height = saved.height;
}

/* far_syms: g_BtlActuatorC8List g_BtlRecords g_BtlActingSlot g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlSlotActiveFlags */

/* BtlShowB6Run -- BATTLE code: enemy action 0xB6 (Slice O' Dead): the
   caster blinks out, a copy of it appears on every live party slot at
   once, the impact, and it blinks back.

   1. g_BtlExtraRecords = xmalloc_heap_alloc(0x200), cleared (mem_clear): room for
      eight 0x40 records, of which five are used.
   2. Blink out: for i = 11 down to 1, i frames (BtlFrameStep) and then the
      caster's first word (+0, the visible bit) ^= 1 -- 66 frames, eleven
      toggles, so the caster ends HIDDEN.
   3. Sound 0x62 (SoundPlay(0x62, 3)).
   4. For g_BtlTargetSlot = 0..4 (the global target index is the loop counter and
      is left at 5): copy[n] = the caster's whole 0x40 record -- for EVERY
      slot, live or not -- and, if party slot n is live (g_BtlSlotActiveFlags[n]), the
      copy's +0 ^= 1 (shown: the record was copied hidden), x and y0 (+6 /
      +0xA) the slot's, depth (+8) the slot's + 1.
   5. Vibration script g_BtlActuatorC8List (g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0);
      g_BtlExtraRecordCount = 8 (the extra-object count: eight records are drawn, the
      dead slots' and the three spare ones hidden).
   6. For each live slot n: the copy takes anim 2 (BtlAnimationSet), the
      party record's +0x34 = its +0x36, and `j` remembers n.
   7. BtlAnimationEndWait waits out the LAST live slot's copy's anim, then
      BtlPartyHitAnimationSet(), the impact BtlAmountShowStart(1), g_BtlExtraRecordCount = 0,
      sprite_run_off(0x1C, 0x14) (sprite slots 0x1C..0x2F freed).
   8. Blink back: for i = 0..10, i frames and then the caster's +0 ^= 1 --
      55 frames, eleven toggles, so the caster is VISIBLE again.
   9. BtlAmountShowWait(), xmalloc_heap_free(g_BtlExtraRecords) (the block is freed),
      BtlMarkedRestAnimationSet().

   If no party slot is live, `j` is whatever the blink-out loop left (0):
   step 7 waits on copy 0.

   No RNG draw of its own (g_BtlActuatorC8List has no random motion); its own frames
   are the 66 + 55 of the two blinks, the rest are callees'.

   Shape: `i` and `j` are u8 (masked at each test); `j` is the inner frame
   counter of both blinks AND the remembered slot ($s2 throughout). The
   record copy is a struct assignment. Each site has its own pointer
   variable: sharing one `t` between the two party loops made it a single
   pseudo and swapped $a0 / $a1 in the first loop (170/184). */
/* func_8003FAD8 */
void BtlShowB6Run(void)
{
    BtlRecord *c;
    BtlRecord *t;
    BtlRecord *u;
    u8 i;
    u8 j;

    g_BtlExtraRecords = xmalloc_heap_alloc(0x200);
    mem_clear(g_BtlExtraRecords, 0x200);
    for (i = 0xB; i != 0; i--) {
        for (j = i; j != 0; j--) {
            BtlFrameStep();
        }
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->on ^= 1;
    }
    SoundPlay(0x62, 3);
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++) {
        *(BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlExtraRecords) = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
        if (g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0) {
            c = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlExtraRecords);
            t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
            c->on ^= 1;
            c->sprite.x = t->sprite.x;
            c->sprite.y = t->sprite.y;
            c->sprite.depth = t->sprite.depth + 1;
        }
    }
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    g_BtlExtraRecordCount = 8;
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0) {
            BtlAnimationSet(g_BtlExtraRecords + (i << 6), 2);
            u = (BtlRecord *)((i << 6) + g_BtlRecords);
            u->timer = u->step_length;
            j = i;
        }
    }
    BtlAnimationEndWait((void *)(g_BtlExtraRecords + (j << 6)));
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(1);
    g_BtlExtraRecordCount = 0;
    sprite_run_off(0x1C, 0x14);
    for (i = 0; i < 0xB; i++) {
        for (j = i; j != 0; j--) {
            BtlFrameStep();
        }
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->on ^= 1;
    }
    BtlAmountShowWait();
    xmalloc_heap_free(g_BtlExtraRecords);
    BtlMarkedRestAnimationSet();
}

extern BtlWalkBlock g_BtlWalkBlock;

s32 BtlAnimationSet(s32, s32);                            /* extern */
s32 BtlWalkRun();                                  /* extern */
s32 BtlPartyHitAnimationSet();                                  /* extern */
s32 BtlFrameWait(s32);                                 /* extern */
s32 BtlShowEnd();                                  /* extern */
s32 SoundPlay(s32, s32);                              /* extern */
s32 BtlAmountShowStart(s32);                                 /* extern */
extern s32 g_BtlRecords; /* far_syms: g_BtlRecords */

/* func_8003FDB8 */
void BtlShowB7Run(void) {
    g_BtlWalkBlock.walk = 1;
    g_BtlWalkBlock.animation = 1;
    BtlWalkRun();
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x73, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 2);
    BtlFrameWait(0x1E);
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(1);
    BtlFrameWait(0x1E);
    BtlShowEnd();
}
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlShotBlock g_BtlActuatorC8List */

/* Enemy action 0xB8 (g_BtlActingShowFunctions[33]): the actor throws ITSELF. Sound 7,
   anim 8 to its end, then the actor is hidden (active = 0) and the shot
   BtlShotRun flies its anim 9 from the actor to the target in 0x2C
   frames (b0 = 0xFF: no launch anim; h8 = minus half the actor's width
   +0x3C, which cancels the shot's own half-width step; the rest of the
   block is BtlShotSet's defaults, set by the dispatcher BtlActingShowRun:
   launch sound 0x4B, +0xC / +0x14 the actor's record, +0x10 the target's).
   The actor is shown again facing by x (attr 0x20 when left of the target, else
   0x2020), its x / z kept aside and the record moved ONTO the target (x, z
   the target's, y the target's + 1); effect trio g_BtlActuatorC8List, sound 0x5A,
   anim 0xA to its end, BtlPartyHitAnimationSet, the impact BtlAmountShowStart(0). It
   turns round (attr the other way), sound 8, anim 0xB to its end, and is
   hidden again: a copy of the record goes on the stack, the record returns
   home (x, z back; y = z) and the copy is placed at the target (x; y = z =
   the target's z). The second shot flies that copy's anim 0xC in 0x28
   frames with the roles swapped (+0xC the target, +0x10 the actor, +0x14
   the copy), i.e. from the target back home. active = 1, anim 0xD to its
   end, BtlShowEnd. No RNG draw of its own. */
/* func_8003FE60 */
void BtlShowB8Run(void)
{
    BtlRecord saved;
    BtlShotBlock *fx;
    BtlRecord *p0;
    BtlRecord *p1;
    BtlRecord *q1;
    BtlRecord *p2;
    BtlRecord *q2;
    BtlRecord *p3;
    BtlRecord *q3;
    BtlRecord *p4;
    BtlRecord *p5;
    BtlRecord *q5;
    BtlRecord *p6;
    s16 sx;
    s16 sz;
    s16 z;

    SoundPlay(7, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    fx = (BtlShotBlock *)g_BtlShotBlock;
    p0 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s16, p0->on) = 0;
    fx->from_animation = 0xFF;
    fx->animation = 9;
    fx->dx = -(p0->width >> 1);
    fx->length = 0x2C;
    BtlShotRun();
    p1 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    q1 = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    AS(s16, p1->on) = 1;
    if (p1->sprite.x < q1->sprite.x) {
        p1->sprite.attr = 0x20;
    } else {
        p1->sprite.attr = 0x2020;
    }
    p2 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    q2 = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    sx = p2->sprite.x;
    sz = p2->sprite.y;
    p2->sprite.x = q2->sprite.x;
    p2->sprite.y = q2->sprite.y;
    p2->sprite.depth = q2->sprite.depth + 1;
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x5A, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(0);
    p3 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    q3 = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    if (p3->sprite.x < q3->sprite.x) {
        p3->sprite.attr = 0x2020;
    } else {
        p3->sprite.attr = 0x20;
    }
    SoundPlay(8, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xB);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    p4 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s16, p4->on) = 0;
    saved = *p4;
    p5 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    q5 = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
    p5->sprite.x = sx;
    p5->sprite.y = sz;
    p5->sprite.depth = sz;
    saved.sprite.x = q5->sprite.x;
    z = q5->sprite.y;
    fx->from = (s32)q5;
    fx->copied = (s32)&saved;
    fx->animation = 0xC;
    fx->to = (s32)p5;
    fx->length = 0x28;
    saved.sprite.y = z;
    saved.sprite.depth = z;
    BtlShotRun();
    p6 = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s16, p6->on) = 1;
    BtlAnimationSet((s32)p6, 0xD);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    BtlShowEnd();
}

/* far_syms: g_BtlActuatorRiseList (target reaches it %hi/%lo) */
/* far_syms: g_BtlActuator41List (target reaches it %hi/%lo) */
/* far_syms: g_PadActuatorLevels (target reaches it %hi/%lo) */
/* far_syms: g_PadActuator1Level (target reaches it %hi/%lo) */
/* far_syms: g_PadActuatorLevels (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadActuator1Level (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 g_BtlActuatorRiseList[];
extern u8 g_BtlActuator41List[];
extern s8 g_PadActuatorLevels;
extern s8 g_PadActuator1Level;

/* func_800401D8 */
void BtlShowB9Run(void) {
    if (g_BtlSlotIds[g_BtlActingSlot] == 0x74) {
        g_PadActuatorList = (s32 *)g_BtlActuatorRiseList;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        SoundPlay(0x65, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
        BtlFrameWait(0x20);
    } else {
        g_PadActuatorList = (s32 *)g_BtlActuator41List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
        SoundPlay(0x66, 3);
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xB);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }

    g_PadActuatorList = 0;
    g_PadActuatorRestFlags = 0;
    g_PadActuatorListWait = 0;
    g_PadActuatorLevels = 0;
    g_PadActuator1Level = 0;

    BtlAmountShowStart(1);

    if (g_BtlSlotIds[g_BtlActingSlot] == 0x74) {
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }

    BtlShowEnd();
}
/* far_syms: g_BtlActuatorRandomList (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
extern u8 g_BtlActuatorRandomList[];
extern u8 g_BtlExtraRecordCount;
s32 xmalloc_heap_free(s32);
void BtlAmountShowWait(void);
s32 BtlFacingPartySet();

/* func_80040348 */
void BtlShowBARun(void)
{
    BtlRecord start_record;
    PathPoint *path;
    PathPoint *pt;
    BtlRecord *obj;
    BtlRecord *clone;
    BtlRecord *p0;
    BtlRecord *p1;
    BtlRecord *p2;
    BtlRecord *p3;
    BtlRecord *p4;
    u8 i;
    u8 j;
    u8 max;

    g_BtlExtraRecords = xmalloc_heap_alloc(0x140);
    path = (PathPoint *)g_LoadBuffer;
    BtlFacingPartySet();
    start_record = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    start_record.sprite.y -= 0x60;
    if (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.attr & 0x2000) {
        start_record.sprite.x -= 8;
    } else {
        start_record.sprite.x += 8;
    }
    max = 0;
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0) {
            obj = (BtlRecord *)((i << 6) + g_BtlRecords);
            j = (start_record.sprite.x < obj->sprite.x ? obj->sprite.x - start_record.sprite.x : start_record.sprite.x - obj->sprite.x) / 8;
            if (max < j) {
                max = j;
            }
            obj = (BtlRecord *)((i << 6) + g_BtlRecords);
            j = (start_record.sprite.y < obj->sprite.y ? obj->sprite.y - start_record.sprite.y : start_record.sprite.y - obj->sprite.y) / 8;
            if (max < j) {
                max = j;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0) {
            line_path_make((u16 *)(path + i * 40), start_record.sprite.x, start_record.sprite.y,
                          ((BtlRecord *)((i << 6) + g_BtlRecords))->sprite.x,
                          ((BtlRecord *)((i << 6) + g_BtlRecords))->sprite.y, max);
        }
    }
    SoundPlay(0x6E, 3);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    g_PadActuatorList = (s32 *)g_BtlActuatorRandomList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    p0 = (BtlRecord *)g_BtlExtraRecords;
    p1 = p0 + 1;
    p2 = p0 + 2;
    p3 = p0 + 3;
    p4 = p0 + 4;
    *p0 = *p1 = *p2 = *p3 = *p4 = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] == 0) {
            ((BtlRecord *)((i << 6) + g_BtlExtraRecords))->on = 0;
        } else {
            BtlAnimationSet(g_BtlExtraRecords + (i << 6), 0xC);
        }
    }
    g_BtlExtraRecordCount = 5;
    for (i = 0; i < max; i++) {
        for (j = 0; j < 5; j++) {
            if (g_BtlSlotActiveFlags[j] != 0) {
                clone = (BtlRecord *)((j << 6) + g_BtlExtraRecords);
                pt = (PathPoint *)(((j * 40 + i) * 4) + (s32)path);
                clone->sprite.x = AS(s16, pt->x);
                clone->sprite.depth = (clone->sprite.y = AS(s16, pt->y)) + 1;
            }
        }
        BtlFrameStep();
    }
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0) {
            j = i;
            BtlAnimationSet(g_BtlExtraRecords + (i << 6), 0xD);
        }
    }
    SoundPlay(0x5A, 3);
    BtlFrameWait(0x10);
    BtlAmountShowStart(1);
    BtlAnimationEndWait((void *)(g_BtlExtraRecords + (j << 6)));
    g_BtlExtraRecordCount = 0;
    sprite_run_off(0x1C, 5);
    BtlAmountShowWait();
    xmalloc_heap_free(g_BtlExtraRecords);
}
/* far_syms: g_BtlActuatorC8List g_BtlPartyHitAnimations g_BtlRecords g_BtlActingSlot g_BtlTargetSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlSlotAmountKinds */
void BtlActingPathMove(s16 *path);  /* TU defines this below */

/* BtlShowBBRun -- BATTLE code: enemy action 0xBB -- the caster dashes
   onto each flagged party slot in turn, then the impact, then home.

   1. Vibration script g_BtlActuatorC8List (g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0); the
      caster's x and y0 (+6 / +0xA) are kept aside.
   2. For g_BtlTargetSlot = 0..4 (the global target index is the loop counter and
      is left at 5), each slot whose result byte g_BtlSlotAmountKinds[n] is non-zero:
        - the caster faces it: attr (+0xC) = 0x20 if its x is below the
          slot's, else 0x2020;
        - line_path_make fills the path buffer g_LoadBuffer with 0x10 points
          from the caster's (x, y0) to the slot's (x, y0);
        - BtlActingPathMove(path), the dash: caster anim 1, anim 8, the
          sound g_BtlActionSound, then one point per frame for 16 frames (x, y0,
          and y = y0 + 1);
        - the vibration script is started again;
        - BtlAnimationEndWait waits out the caster's anim;
        - the slot takes anim g_BtlPartyHitAnimations[n] (BtlAnimationSet).
      The caster is NOT sent home between targets: each dash starts where
      the last one ended.
   3. The impact BtlAmountShowStart(0).
   4. A 0x10-point path from where the caster stands back to the kept
      (x, y0), and the same dash along it.
   5. Caster anim 9, waited out (BtlAnimationEndWait); BtlActingRestAnimationSet(); the
      caster's y (+8) = its y0; BtlAmountShowWait(), BtlFallRun(),
      BtlMarkedRestAnimationSet().

   No RNG draw of its own (g_BtlActuatorC8List has no random motion). Its own code
   waits no frame: 16 per dash are the sub's, the rest BtlAnimationEndWait's.

   Shape: the kept x / y0 are u16 locals (lhu, then sll/sra at the call --
   line_path_make's parameters are s16); every site forms its own record
   pointer; the path pointer is re-read from g_LoadBuffer before each
   line_path_make. Statement order is the target's, nothing was shaped. */
/* func_800408F8 */
void BtlShowBBRun(void)
{
    BtlRecord *c;
    BtlRecord *o;
    BtlRecord *t;
    BtlRecord *e;
    BtlRecord *r;
    u16 *path;
    u16 x;
    u16 y0;

    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    x = ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x;
    y0 = ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y;
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++) {
        if (g_BtlSlotAmountKinds[g_BtlTargetSlot] != 0) {
            c = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
            if (c->sprite.x < ((BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords))->sprite.x) {
                c->sprite.attr = 0x20;
            } else {
                c->sprite.attr = 0x2020;
            }
            path = g_LoadBuffer;
            o = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
            t = (BtlRecord *)((g_BtlTargetSlot << 6) + g_BtlRecords);
            line_path_make(path, o->sprite.x, o->sprite.y, t->sprite.x, t->sprite.y, 0x10);
            BtlActingPathMove((s16 *)path);
            g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
            g_PadActuatorRestFlags = 0x3F;
            g_PadActuatorListWait = 0;
            BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
            BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlPartyHitAnimations[g_BtlTargetSlot]);
        }
    }
    BtlAmountShowStart(0);
    path = g_LoadBuffer;
    e = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    line_path_make(path, e->sprite.x, e->sprite.y, (s16)x, (s16)y0, 0x10);
    BtlActingPathMove((s16 *)path);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    BtlActingRestAnimationSet();
    r = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    r->sprite.depth = r->sprite.y;
    BtlAmountShowWait();
    BtlFallRun();
    BtlMarkedRestAnimationSet();
}

/* func_80040BA0 */
void BtlActingPathMove(s16 *path)
{
  s16 *path_y;
  void *frame_record;
  u8 i;

  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 1);
  i = 0x10;
  BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
  SoundPlay(g_BtlActionSound, 3);
  path_y = path + 1;

  while (i != 0)
  {
    frame_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    ((BtlRecord *)frame_record)->sprite.x = *path;
    ((BtlRecord *)frame_record)->sprite.y = *path_y;
    i--;
    ((BtlRecord *)frame_record)->sprite.depth = *path_y + 1;
    BtlFrameStep();
    path += 2;
    path_y += 2;
  }
}
/* far_syms: g_BtlActuatorC8List g_BtlActuatorFFList g_BtlRecords g_BtlActingSlot g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait g_BtlSlotAmountKinds */

/* BtlShowBCRun -- BATTLE code: enemy action 0xBC -- four copies of the
   caster, one onto each flagged party slot 0..3, and the impact.

   1. g_BtlExtraRecords = xmalloc_heap_alloc(0x100): room for FOUR 0x40 records. All
      four are filled with the caster's record as it stands at entry --
      copy 3 from the caster, 2 from 3, 1 from 2, 0 from 1.
   2. Vibration script g_BtlActuatorC8List (g_PadActuatorRestFlags = 0x3F, g_PadActuatorListWait = 0),
      sound 0x6E (SoundPlay(0x6E, 3)), BtlFacingPartySet().
   3. The caster takes anim 0xA (BtlAnimationSet) and plays it to its end
      (BtlAnimationEndWait).
   4. For slot i = 0..3 with g_BtlSlotAmountKinds[i] set (the action's flagged
      targets): copy i's first word (+0) = 1, x and y0 (+6 / +0xA) the
      slot's, depth (+8) = that y0 + 1 (NOT the slot's depth + 1, as
      BtlShowB6Run has it), the copy takes anim 0xF, and `j` remembers i.
      Slot 4 is never looked at; the copies of unflagged slots are left as
      the caster's record.
   5. Vibration script g_BtlActuatorFFList, sound 0x5A, g_BtlExtraRecordCount = 4 (the four
      records are drawn as extra objects), BtlFrameWait(0x10) (16 frames),
      the impact BtlAmountShowStart(1).
   6. BtlAnimationEndWait waits out the LAST flagged slot's copy's anim, then
      g_BtlExtraRecordCount = 0, sprite_run_off(0x1C, 4) (sprite slots 0x1C..0x1F
      freed), BtlShowEnd(), xmalloc_heap_free(g_BtlExtraRecords) (the block freed).

   `j` IS READ UNINITIALISED IN THE ORIGINAL when no slot 0..3 is flagged:
   $s2 is only written inside the `if`, so step 6 waits on record
   (caller's $s2 & 0xFF) of the block -- outside it for a value above 3.

   No RNG draw of its own (neither script has the random motion); every
   frame is a callee's.

   Shape: the four records are reached through four pointer LOCALS set up
   front (o1..o3 = o0 + 1..3): the target keeps the three sums in
   $t0 / $a3 / $a2 and each block move copies them. Indexing a local base
   (o0[3]) keeps (plus reg const) as the address and re-adds at each use
   (119/169); indexing through the global makes four copies of the base
   (150/169). The depth is the field just stored re-read (c->y0 + 1):
   t->y0 + 1 reloads the slot's field (165/169). Each site has its own
   pointer variable; `i` and `j` are u8. */
/* func_80040C70 */
void BtlShowBCRun(void)
{
    BtlRecord *o0;
    BtlRecord *o1;
    BtlRecord *o2;
    BtlRecord *o3;
    BtlRecord *c;
    BtlRecord *t;
    u8 i;
    u8 j;

    o0 = (BtlRecord *)xmalloc_heap_alloc(0x100);
    g_BtlExtraRecords = (s32)o0;
    o1 = o0 + 1;
    o2 = o0 + 2;
    o3 = o0 + 3;
    *o3 = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    *o2 = *o3;
    *o1 = *o2;
    *o0 = *o1;
    g_PadActuatorList = (s32 *)g_BtlActuatorC8List;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x6E, 3);
    BtlFacingPartySet();
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xA);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot << 6)));
    for (i = 0; i < 4; i++) {
        if (g_BtlSlotAmountKinds[i] != 0) {
            c = (BtlRecord *)((i << 6) + g_BtlExtraRecords);
            t = (BtlRecord *)((i << 6) + g_BtlRecords);
            c->on = 1;
            c->sprite.x = t->sprite.x;
            c->sprite.y = t->sprite.y;
            c->sprite.depth = c->sprite.y + 1;
            j = i;
            BtlAnimationSet((s32)c, 0xF);
        }
    }
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    SoundPlay(0x5A, 3);
    g_BtlExtraRecordCount = 4;
    BtlFrameWait(0x10);
    BtlAmountShowStart(1);
    BtlAnimationEndWait((void *)(g_BtlExtraRecords + (j << 6)));
    g_BtlExtraRecordCount = 0;
    sprite_run_off(0x1C, 4);
    BtlShowEnd();
    xmalloc_heap_free(g_BtlExtraRecords);
}

extern u8 g_BtlTargetSlot;
extern s8 g_BtlShotBlock[];  /* used as a base in pointer arithmetic (cc1) */

/* func_80040F14 */
void BtlShotSet(void) {
    g_BtlShotBlock[0] = 8;
    g_BtlShotBlock[1] = 0x63;
    g_BtlShotBlock[2] = 9;
    g_BtlShotBlock[3] = 0;
    g_BtlShotBlock[4] = 8;
    g_BtlShotBlock[5] = 0;
    g_BtlShotBlock[6] = 0x4B;
    g_BtlShotBlock[7] = 0;
    ((s16 *)g_BtlShotBlock)[4] = 0;
    ((s16 *)g_BtlShotBlock)[5] = 0;
    ((s32 *)g_BtlShotBlock)[3] = g_BtlRecords + (g_BtlActingSlot << 6);
    ((s32 *)g_BtlShotBlock)[4] = g_BtlRecords + (g_BtlTargetSlot << 6);
    ((s32 *)g_BtlShotBlock)[5] = g_BtlRecords + (g_BtlActingSlot << 6);
}
/* far_syms: g_BtlFromRecord (target reaches it %hi/%lo) */
/* far_syms: g_BtlToRecord (target reaches it %hi/%lo) */
extern BtlRecord *g_BtlFromRecord;
extern BtlRecord *g_BtlToRecord;

/* BtlShotRun — BATTLE code (the segment name is the splitter's): the
   projectile of an enemy special, played from the presentation block
   g_BtlShotBlock that BtlShotSet / the action handlers fill in
   (BtlShowADRun/04/05/09/12, BtlShow9BRun, BtlShow9CRun):
     b0  anim the ACTOR takes at launch (0xFF: none, and no b5 / b1 step)
     b1  wait after that anim: >= 0x63 until its last step
         (BtlAnimationEndWait), else b1 frames (BtlFrameWait), 0 none
     b2  the projectile's anim
     b3  flight length in frames; 0 = derive it from the distance
     b4  pixels per frame for the derived length (0 -> 8, written back)
     b5 / b6 / b7  sounds (SoundPlay(n, 3)) before the actor's anim,
         at launch, and on arrival; 0 = none
     h8 / hA  muzzle offset from the actor: x (mirrored by facing), y0
     +0xC / +0x10 / +0x14  actor, target, and the record to copy
   The projectile is a 64-byte copy of *(+0x14) on the stack, published as
   the extra object g_BtlExtraRecords with word +0 cleared, then set to 1 for the
   flight (g_BtlExtraRecordCount = 1 while it flies). Actor and target are turned to
   face each other (+0xC = 0x20 / 0x2020, the projectile takes the
   actor's) and the start is x = actor.x +- (actor's +0x3C >> 1) +- h8,
   y0 = actor.y0 + hA. The derived length is max(|dx|, |dy|) / b4, at
   least 1 -- |dy|'s SIGN test compares the actor's y0 with the target's,
   not the projectile's (the original's own asymmetry). line_path_make
   writes that many (x, y0) points from the start to the target's (x, y0)
   into the path buffer g_LoadBuffer, and the flight shows one point per
   frame (y = y0 + 1), one BtlFrameStep each. Then slot 0x1C is freed
   (sprite_run_off(0x1C, 1)). No RNG draw of its own; every frame is the
   battle frame wait (frame-rng.md), directly or under BtlAnimationEndWait /
   BtlFrameWait.

   Shape: g_BtlFromRecord / g_BtlToRecord are reached %hi/%lo HERE (btl_path.c
   reaches them %gp_rel) -- scalar far_syms, the far-array form scores
   182. `obj.attr` is stored AFTER `obj.x` in each arm: the store may
   alias the actor loads, so it holds the 0x20 / 0x2020 constant across
   them (a fourth arm-local register, which is what puts the two actor
   pointers in a2 / a3) and lets cross-jumping share the x store. The dy
   magnitude and the flight counter are ONE s16 variable (s1). The flight
   walks the buffer with `path += 2` before the frame wait: the +1
   element is a reduced giv (s0 = path + 2), the count's sign extension
   is hoisted (s3). */
/* func_80040F8C */
void BtlShotRun(void)
{
    BtlRecord obj;
    BtlShotBlock *p;
    u16 *path;
    s16 n;
    s16 i;

    p = (BtlShotBlock *)g_BtlShotBlock;
    path = g_LoadBuffer;
    g_BtlExtraRecords = (s32)&obj;
    g_BtlFromRecord = (BtlRecord *)p->from;
    g_BtlToRecord = (BtlRecord *)p->to;
    obj = *(BtlRecord *)p->copied;
    obj.on = 0;
    BtlAnimationSet((s32)&obj, p->animation);
    if (g_BtlFromRecord->sprite.x < g_BtlToRecord->sprite.x) {
        g_BtlFromRecord->sprite.attr = 0x20;
        g_BtlToRecord->sprite.attr = 0x2020;
        obj.sprite.x = p->dx + (g_BtlFromRecord->sprite.x + (g_BtlFromRecord->width >> 1));
        obj.sprite.attr = 0x20;
    } else {
        g_BtlFromRecord->sprite.attr = 0x2020;
        g_BtlToRecord->sprite.attr = 0x20;
        obj.sprite.x = g_BtlFromRecord->sprite.x - (g_BtlFromRecord->width >> 1) - p->dx;
        obj.sprite.attr = 0x2020;
    }
    obj.sprite.y = g_BtlFromRecord->sprite.y + p->dy;
    if (p->length == 0 && p->speed == 0) {
        p->speed = 8;
    }
    if (p->length != 0) {
        n = p->length;
    } else {
        if (obj.sprite.x < g_BtlToRecord->sprite.x) {
            n = g_BtlToRecord->sprite.x - obj.sprite.x;
        } else {
            n = obj.sprite.x - g_BtlToRecord->sprite.x;
        }
        if (g_BtlFromRecord->sprite.y < g_BtlToRecord->sprite.y) {
            i = g_BtlToRecord->sprite.y - obj.sprite.y;
        } else {
            i = obj.sprite.y - g_BtlToRecord->sprite.y;
        }
        if (n < i) {
            n = i;
        }
        n = n / p->speed;
        if (n == 0) {
            n = 1;
        }
    }
    line_path_make(path, obj.sprite.x, obj.sprite.y, g_BtlToRecord->sprite.x, g_BtlToRecord->sprite.y, n);
    if (p->from_animation != 0xFF) {
        if (p->from_sound != 0) {
            SoundPlay(p->from_sound, 3);
        }
        BtlAnimationSet((s32)g_BtlFromRecord, p->from_animation);
        if (p->from_wait >= 0x63) {
            BtlAnimationEndWait(g_BtlFromRecord);
        } else if (p->from_wait != 0) {
            BtlFrameWait(p->from_wait);
        }
    }
    if (p->launch_sound != 0) {
        SoundPlay(p->launch_sound, 3);
    }
    g_BtlExtraRecordCount = 1;
    obj.on = 1;
    for (i = 0; i < n; i++) {
        obj.sprite.x = path[0];
        obj.sprite.y = path[1];
        obj.sprite.depth = path[1] + 1;
        path += 2;
        BtlFrameStep();
    }
    if (p->arrive_sound != 0) {
        SoundPlay(p->arrive_sound, 3);
    }
    sprite_run_off(0x1C, 1);
    g_BtlExtraRecordCount = 0;
}
