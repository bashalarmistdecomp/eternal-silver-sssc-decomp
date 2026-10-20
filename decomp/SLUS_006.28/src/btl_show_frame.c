/* Lunar SSSC — btl_show_frame: the battle shows' frame and what each frame
   steps: the show sound slots and the voice, the show records and the
   effect they load, the amount show and the enemies' fall show, and the
   tile patterns a show puts in the tile layer. The first split had it in
   data_mgmt.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
extern BtlShowSound g_BtlShowSounds[];
s32 SoundPlay(s32, s32);

extern s32 tile_pattern_put(s32, s32, s32, u32, u32);
extern s32 tile_grid_clear();
extern void BtlAnimationStep(); /* defined void (AnimActor *, s32, s32) in btl_animation.c */
extern s32 BtlFrameStep();
extern void BtlWindowsMoveStep(void);
extern void VoiceStart(void);   /* defined in music_voice.c: reads no
                                      args (its $a* are set up for an
                                      outgoing call), returns nothing */
extern s32 VoiceStep(s32);
extern s32 BtlAmountShowStart(s32);
extern s32 BtlAmountShowCheck();
extern void BtlShowFrameStep(void);   /* defined below */
extern s32 BtlShowSoundsStep();
extern s32 BtlShowWaveStep();
extern s32 BtlShowPulseStep();

extern u8 g_BtlShowVoiceState;
extern BtlEffectTileRow *g_BtlEffectTileRow;
extern s32 g_BtlShowRecords;
extern s32 g_BtlShowOpen;
/* BtlEffectLoad's 13-byte name buffer: an 11-byte rodata string and two zeroed bytes. */
typedef struct { s8 s[11]; } Name11;
typedef struct { Name11 n; s8 z[2]; } Name13;
/* The 12 sound-scheduler slots at g_BtlShowSounds (BtlShowSoundsStep ticks them):
   state 0 off, 1 repeating, >1 a countdown; id plays on channel slot + 3. */
/* The battle records (BtlRecord, btl.h) are indexed as a struct here, so
   that stores to them stay apart from fixed-address scalars (GCC 2.8's
   MEM_IN_STRUCT rule). */
void BtlAnimationStart(void *, s32);
void BtlRecordSizeSet(void *obj, u16 frame);
/* far_syms: g_BtlAmountRecordsRunning */
extern s32 g_BtlRecords;
extern u16 g_BtlAmountShowLength;
extern u16 g_BtlAmountShowFrameCount;
extern u16 *g_BtlAmountShowLongestRecord;
extern s32 g_BtlAmountRecordsRunning;
extern u8 g_BtlSlotAmountKinds[];
extern u8 g_BtlSlotActiveFlags[];
extern s8 g_BtlMixFlash[];
void BtlAmountRecordOff(u32);
s32 BtlSlotActionAnimationSet(u8);
void layer_mix_switch_off(s32);
s32 sprite_mix_col_set_copy(s32);
s32 layer_mix_set(s32, s32);

/* The presentations' "effect still running?" poll: 1 while the longest
   tracked animation (g_BtlAmountShowLongestRecord, length g_BtlAmountShowLength, elapsed g_BtlAmountShowFrameCount)
   runs. Otherwise it clears the effect state: g_BtlAmountRecordsRunning, the length
   pair, effect blocks 0..12 (g_BtlRecords + 0x380 + i * 0x40: +0, +4,
   +0x14, +0x18, +0x28, +0x2C) with BtlAmountRecordOff(i) each, the 14
   objects' display bits (| 0x2000 & 0xA3FF, BtlSlotActionAnimationSet for flagged
   party slots in g_BtlSlotActiveFlags / g_BtlSlotAmountKinds), the shake (g_BtlMixFlash), the
   tint (layer_mix_switch_off, sprite_mix_col_set_copy(0x7FFF)) and window 0x400; then 0. */
/* func_8008931C */
s32 BtlAmountShowCheck(void)
{
  u16 i;
  s32 blk;
  if (g_BtlAmountShowLength != 0)
  {
    if ((*g_BtlAmountShowLongestRecord != 0) && (g_BtlAmountShowFrameCount < g_BtlAmountShowLength))
    {
      return 1;
    }
  }
  g_BtlAmountRecordsRunning = 0;
  g_BtlAmountShowLength = 0;
  g_BtlAmountShowFrameCount = 0;
  for (i = 0; i < 0xD; i++)
  {
    blk = (i << 6) + g_BtlRecords;
    ((BtlRecord *)blk)[14].sprite.on = 0;
    ((BtlRecord *)blk)[14].on = 0;
    ((BtlRecord *)blk)[14].sprite.piece_table = 0;
    ((BtlRecord *)blk)[14].sprite.pose_table = 0;
    ((BtlRecord *)blk)[14].animation_table = 0;
    ((BtlRecord *)blk)[14].step_list = 0;
    BtlAmountRecordOff(i);
  }
  for (i = 0; i < 0xE; i++)
  {
    ((BtlRecord *) g_BtlRecords)[i].sprite.flags = (((BtlRecord *) g_BtlRecords)[i].sprite.flags | 0x2000) & 0xA3FF;
    if ((g_BtlSlotActiveFlags[i] != 0) && (g_BtlSlotAmountKinds[i] != 0) && (i < 5))
    {
      BtlSlotActionAnimationSet(i);
    }
  }
  g_BtlMixFlash[0] = 0;
  layer_mix_switch_off(0x100);
  sprite_mix_col_set_copy(0x7FFF);
  layer_mix_set(0x400, 0xFF);
  return 0;
}

/* far_syms: g_BtlRecords */
/* far_syms: g_BtlNumber */
extern s32 g_BtlRecords;
extern u8 g_BtlSlotFallMarks[];
extern u8 g_BtlNumber;
extern s8 g_BtlMixFlash[];
extern u16 g_BtlEnemyFallScales[];
s32 sprite_mix_col_set_copy(s32);
void layer_mix_switch_on(s32);
void layer_mix_switch_off(s32);
s32 BtlMixFlashSet(s32, s32);
s32 BtlAnimationSetCopy(s32, s32);
s32 layer_mix_set(s32, s32);

/* func_80089488 */
void BtlEnemyFallShow(void)
{
  u16 i;
  u16 n;
  n = 0;
  for (i = 6; i < 0xE; i++)
  {
    if (g_BtlSlotFallMarks[i] != 0)
    {
      n++;
      AS(s16, ((BtlRecord *) g_BtlRecords)[i].sprite.scaled) = 1;
      AS(s32, ((BtlRecord *) g_BtlRecords)[i].sprite.scale_x) = 0x1000100;
      ((BtlRecord *) g_BtlRecords)[i].sprite.flags = (((BtlRecord *) g_BtlRecords)[i].sprite.flags & 0xCBFF) | 0x4800;
    }
  }
  if (n == 0)
  {
    return;
  }
  sprite_mix_col_set_copy(0x1F);
  layer_mix_switch_on(0x100);
  BtlMixFlashSet(0x400, 5);
  if ((g_BtlSlotFallMarks[6] != 0) && ((u8) (g_BtlNumber + 0xA) < 2))
  {
    while (g_BtlMixFlash[8] < 4)
    {
      BtlFrameStep();
    }
    BtlAnimationSetCopy(g_BtlRecords + 0x180, 5);
  }
  else
  {
    while (g_BtlMixFlash[8] < 4)
    {
      BtlFrameStep();
    }
    for (n = 0; n < 0x27; n++)
    {
      for (i = 6; i < 0xE; i++)
      {
        if (g_BtlSlotFallMarks[i] != 0)
        {
          if (n < 8)
          {
            ((BtlRecord *) g_BtlRecords)[i].sprite.scale_y = g_BtlEnemyFallScales[7 - n];
          }
          else if (n < 0x10)
          {
            ((BtlRecord *) g_BtlRecords)[i].sprite.scale_x = g_BtlEnemyFallScales[0xF - n];
          }
          else if (n < 0x1A)
          {
            ((BtlRecord *) g_BtlRecords)[i].sprite.scale_y = g_BtlEnemyFallScales[n - 0x10];
          }
          else if (n < 0x24)
          {
            AS(u16, ((BtlRecord *) g_BtlRecords)[i].sprite.y) -= 0x14;
          }
          else
          {
            ((BtlRecord *) g_BtlRecords)[i].on = 0;
            AS(u16, ((BtlRecord *) g_BtlRecords)[i].sprite.y) += 0xC8;
          }
        }
      }
      BtlFrameStep();
    }
  }
  for (i = 6; i < 0xE; i++)
  {
    if (g_BtlSlotFallMarks[i] != 0)
    {
      AS(s16, ((BtlRecord *) g_BtlRecords)[i].sprite.scaled) = 0;
      AS(s32, ((BtlRecord *) g_BtlRecords)[i].sprite.scale_x) = 0;
      ((BtlRecord *) g_BtlRecords)[i].sprite.flags = (((BtlRecord *) g_BtlRecords)[i].sprite.flags | 0x2000) & 0xA3FF;
    }
  }
  g_BtlMixFlash[0] = 0;
  layer_mix_switch_off(0x100);
  sprite_mix_col_set_copy(0x7FFF);
  layer_mix_set(0x400, 0xFF);
}

/* func_8008979C */
void BtlAmountShowRun(void) {
    BtlAmountShowStart(1);
loop_1:
    if (BtlAmountShowCheck() != 0) {
        BtlShowFrameStep();
        goto loop_1;
    }
}
/* far_syms: g_TileLayerShown */
extern Name11 D_80012F40;
extern u8 g_BtlEffectRows[];
extern BtlEffectTileRow g_BtlEffectTileRows[];
extern u16 g_BtlEffectResourceNumbers[];
extern u16 *g_ResourceReadBuffer;
extern u8 *g_BtlEffectRow;
extern s32 g_BtlTilePatternList;
extern s8 g_TileLayerShown;
extern u16 *g_BtlEffectBlock[];
extern u8 g_BtlActingSlot;
s8 *memset(s8 *, s8, s32);
s32 func_8001DC60();
s32 resource_load(u16, void *);
void *xmalloc_heap_alloc(s32);
s32 inflate_buffer(void *, void *);
void BtlRecordSpriteDataSet(void *record, void *sprite_data, u32 palette, u32 animation);  /* TU defines this below */
void BtlRecordPlaceCopy(void *source_record, void *destination_record);  /* TU defines this below */
s32 sprite_page_effect_load(void *);
s32 sprite_palette_load(void *, s32);
s32 tile_layer_reset();
s32 tile_resource_load(s32);
s32 func_8001DCDC();
s32 BtlTilePatternListGet(u8);
s32 BtlTileScrollSet(s32, s32, s32);
void tile_grid_clear_copy(s32, s32);  /* TU defines this below */

/* func_800897DC */
void BtlEffectLoad(u8 *blk, u8 id)
{
  Name13 name;
  u8 flags;
  u16 *data;
  u16 *buf;
  u16 *src;
  u8 *obj;
  name.n = D_80012F40;
  memset(name.z, 0, 2);
  g_BtlEffectRow = &g_BtlEffectRows[id * 4];
  flags = g_BtlEffectRow[0];
  if (flags == 0)
  {
    return;
  }
  func_8001DC60();
  if (g_BtlEffectResourceNumbers[id] != 0)
  {
    resource_load(g_BtlEffectResourceNumbers[id], g_ResourceReadBuffer);
  }
  data = g_ResourceReadBuffer;
  if (flags & 1)
  {
    src = (u16 *) ((u8 *) data + data[1] * 4);
    buf = xmalloc_heap_alloc(data[0] * 4);
    g_BtlEffectBlock[0] = buf;
    inflate_buffer(src, buf);
    ((BtlRecord *) blk)->sprite.texel_records = (s32) ((u8 *) buf + buf[4] * 4);
    ((BtlRecord *) blk)->sprite.pose_table = (s32) ((u8 *) buf + buf[2] * 4);
    ((BtlRecord *) blk)->sprite.piece_table = (s32) ((u8 *) buf + buf[3] * 4);
    ((BtlRecord *) blk)->animation_table = (s32) ((u8 *) buf + buf[0] * 4);
    ((BtlRecord *) blk)->step_list = (s32) ((u8 *) buf + buf[1] * 4);
    BtlRecordSpriteDataSet(blk, buf, 0x600, 0);
    obj = (u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s32, ((BtlRecord *)blk)->sprite.scale_x) = 0;
    ((BtlRecord *)blk)->sprite.scaled = 0;
    ((BtlRecord *)blk)->sprite.attr = ((BtlRecord *)obj)->sprite.attr;
    BtlRecordPlaceCopy(obj, blk);
    sprite_page_effect_load(data + 2);
  }
  if (flags & 2)
  {
    sprite_palette_load(data + 8, 0x600);
  }
  if (flags & 4)
  {
    tile_layer_reset();
    tile_resource_load(id);
    func_8001DCDC();
    g_BtlEffectTileRow = &g_BtlEffectTileRows[g_BtlEffectRow[1]];
    g_BtlTilePatternList = BtlTilePatternListGet(g_BtlEffectTileRow->pattern_list);
    BtlTileScrollSet(8, 0, 0);
    tile_grid_clear_copy(0, 1);
    g_TileLayerShown = 1;
  }
}

/* func_80089A3C */
void BtlRecordSpriteDataSet(void *record, void *sprite_data, u32 palette, u32 animation)
{
    u8 *dest = (u8 *)record;
    u8 *src = (u8 *)sprite_data;
    u32 kept_animation;
    u32 idx;

    ((BtlRecord *)dest)->on = 0;
    ((BtlRecord *)dest)->sprite.on = 1;
    ((BtlRecord *)dest)->sprite.attr = 0x20;
    ((BtlRecord *)dest)->sprite.flags = (palette & 0x3FF) | 0x2000;

    ((BtlRecord *)dest)->sprite.texel_records = (u32)(src + ((SpriteDataHead *)src)->texel_records * 4);
    ((BtlRecord *)dest)->sprite.pose_table = (u32)(src + ((SpriteDataHead *)src)->pose_table * 4);
    ((BtlRecord *)dest)->sprite.piece_table = (u32)(src + ((SpriteDataHead *)src)->piece_table * 4);
    ((BtlRecord *)dest)->animation_table = (u32)(src + ((SpriteDataHead *)src)->animation_table * 4);

    idx = ((SpriteDataHead *)src)->step_list;
    kept_animation = animation;
    ((BtlRecord *)dest)->animation = kept_animation;
    kept_animation &= 0xFFFF;

    ((BtlRecord *)dest)->sprite.pose = 0;
    ((BtlRecord *)dest)->sprite.scaled = 0;
    AS(u32, ((BtlRecord *)dest)->sprite.scale_x) = 0;
    ((BtlRecord *)dest)->animation_before = (kept_animation < 1);

    ((BtlRecord *)dest)->step_list = (u32)(src + idx * 4);
    BtlAnimationStart(dest, 1);
    BtlRecordSizeSet(dest, kept_animation);
}

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_80089B18 */
void BtlRecordSpriteBlocksSet(BtlRecord *o, s32 unused, u16 *a, u16 *b, u32 palette, u32 animation)
{
  u8 *p18;
  s32 n;
  u32 kept_animation;
  o->sprite.on = 1;
  o->on = 0;
  o->sprite.attr = 0x20;
  AS(u8 *, o->sprite.pose_table) = (u8 *) (a + 1);
  o->sprite.flags = (palette & 0x3FF) | 0x2000;
  p18 = ((u8 *) a) + ((a[0] * 4) + 2);
  AS(s32, o->sprite.scale_x) = 0;
  o->sprite.scaled = 0;
  o->sprite.pose = 0;
  AS(u8 *, o->animation_table) = (u8 *) (b + 1);
  AS(u8 *, o->sprite.piece_table) = p18;
  n = b[0] * 4;
  o->animation = animation;
  kept_animation = animation & 0xFFFF;
  o->animation_before = kept_animation < 1;
  AS(u8 *, o->step_list) = ((u8 *) b) + (n + 2);
  BtlAnimationStart(o, 1);
  BtlRecordSizeSet(o, kept_animation);
}

extern void VoiceStart(void);
void BtlTilePatternPut(s32 pattern_byte, s32 frame_count);  /* TU defines this below */
void BtlTilePatternSizePut(s32 pattern, s32 width, s32 height, s32 frame_count);  /* TU defines this below */
void tile_grid_clear_copy(s32, s32);  /* TU defines this below */
void BtlShowVoiceStep(void);  /* TU defines this below */

/* func_80089BD0 */
void BtlRecordPlaceCopy(void *source_record, void *destination_record)
{
  s16 *x_then_depth;
  if (1)
  {
    x_then_depth = (s16 *) (((u8 *) source_record) + 0x6);
    ((BtlRecord *)destination_record)->sprite.x = (u16) (*x_then_depth);
  }
  x_then_depth = (s16 *)(s32 *) (((u8 *) destination_record) + 0x8);
  ((BtlRecord *)destination_record)->sprite.y = (u16) (((BtlRecord *)source_record)->sprite.y);
  *x_then_depth = (s16) (AS(s32, ((BtlRecord *)source_record)->sprite.depth) + 1);
}
/* far_syms: g_BtlShowBlocks[0] (target reaches it %hi/%lo) */
extern s32 g_BtlShowBlocks[];

/* func_80089BFC */
void BtlShowRecordsClear(void)
{
  u32 *p;
  unsigned long i;
  p = (u32 *) g_BtlShowBlocks;
  if (p[1] != 0)
  {
    xmalloc_heap_free(p[1]);
  }
  i = 0;
  if (p[2] != 0)
  {
    xmalloc_heap_free(p[2]);
  }
  p[2] = 0;
  p[1] = 0;
  for (; i < 0x15; i++)
  {
    BtlRecordClear(g_BtlShowRecords + (i << 6));
  }

  sprite_run_off(0x1C, 0x15);
}

/* func_80089C8C */
void BtlTilePatternPut(s32 pattern_byte, s32 frame_count) {
    s32 frames_left;

    frames_left = frame_count;
    if (pattern_byte & 0xFF) {
        tile_pattern_put((pattern_byte - 1) & 0xFF, 0, 0, g_BtlEffectTileRow->width >> 1, g_BtlEffectTileRow->height >> 1);
        if (frames_left & 0xFFFF) {
            do {
                frames_left -= 1;
                BtlShowFrameStep();
            } while (frames_left & 0xFFFF);
        }
    }
}

/* func_80089D00 */
void BtlTilePatternSizePut(s32 pattern, s32 width, s32 height, s32 frame_count) {
    s32 frames_left;

    frames_left = frame_count;
    tile_pattern_put(pattern, 0, 0, width & 0xFFFF, height & 0xFFFF);
    if (frames_left & 0xFFFF) {
        do {
            frames_left -= 1;
            BtlShowFrameStep();
        } while (frames_left & 0xFFFF);
    }
}

/* A thunk: every caller passes (0, 1), which tile_grid_clear never reads. */
/* Ruling of 2026-10-14 (docs/naming.md): Does what tile_grid_clear does; nothing read says why there are two. */
/* func_80089D58 */
void tile_grid_clear_copy(s32 unused, s32 unused2) {
    tile_grid_clear();
}

/* func_80089D78 */
void BtlShowVoiceStep(void) {
    if (g_BtlShowVoiceState == 1) {
        VoiceStart();
        g_BtlShowVoiceState += 1;
    }
    if ((g_BtlShowVoiceState != 0) && !(VoiceStep(1) & 0xFF)) {
        g_BtlShowVoiceState = 0;
    }
}

/* func_80089DDC */
void BtlShowFrameStep(void) {
    if (g_BtlShowOpen != 0) {
        BtlShowSoundsStep();
        BtlShowPulseStep();
        BtlShowWaveStep();
        BtlAnimationStep(g_BtlShowRecords, 0x1C, 0x15);
    }
    BtlWindowsMoveStep();
    BtlFrameStep();
}
/* far_syms: g_BtlShowSounds (target reaches it %hi/%lo) */

/* func_80089E34 */
s32 BtlShowSoundsStep()
{
  u32 i;
  u8 *ptr;

  for (i = 0; (i & 0xFF) < 0xC; i++) {
    ptr = (u8 *)&g_BtlShowSounds[i & 0xFF];
    if (((BtlShowSound *)ptr)->state == 0) {
      continue;
    }
    if (((BtlShowSound *)ptr)->state == 1) {
      ((BtlShowSound *)ptr)->count++;
      if ((s8)((BtlShowSound *)ptr)->count < AS(s8, ((BtlShowSound *)ptr)->repeat)) {
        continue;
      }
      SoundPlay(((BtlShowSound *)ptr)->sound, (i + 3) & 0xFF);
      ((BtlShowSound *)ptr)->count = 0;
      if (AS(s8, ((BtlShowSound *)ptr)->repeat) != 0) {
        continue;
      }
      ((BtlShowSound *)ptr)->state = 0;
      ((BtlShowSound *)ptr)->sound = 0;
    } else {
      ((BtlShowSound *)ptr)->state--;
    }
  }
}
/* far_syms: g_BtlShowSounds */

/* Schedule sound `id` on scheduler slot `slot` (channel slot + 3): state 0
   plays it now, otherwise the slot repeats or counts down `period` frames
   (BtlShowSoundsStep). Slots 12+ hang. */
/* func_80089EF8 */
void BtlShowSoundSet(s32 slot, u8 id, s32 state, s32 period)
{
  g_BtlShowSounds[slot].state = state;
  if (slot >= 0xC)
  {
    for (;;)
    {
    }
  }
  if (g_BtlShowSounds[slot].state == 0)
  {
    SoundPlay(id, (u8) (slot + 3));
  }
  else
  {
    g_BtlShowSounds[slot].sound = id;
    g_BtlShowSounds[slot].count = period;
    g_BtlShowSounds[slot].repeat = period;
  }
}

/* func_80089F78 */
void BtlShowSoundStop(s32 slot)
{
  u8 *count_then_entry;
  void *slot_entry;
  g_BtlShowSounds[slot].state = 0;
  slot_entry = &g_BtlShowSounds[slot];
  count_then_entry = &((BtlShowSound *) slot_entry)->count;
  *count_then_entry = 0;
  count_then_entry = (u8 *) slot_entry;
  ((BtlShowSound *) count_then_entry)->repeat = 0;
  ((BtlShowSound *)slot_entry)->sound = 0;
  SoundPlay(0, (slot + 3) & 0xFF);
}
/* func_80089FC4 */
void BtlFacingTowardSet(u8 *record, s16 toward_x)
{
    if (((BtlRecord *)record)->sprite.x < toward_x) {
        ((BtlRecord *)record)->sprite.attr &= 0x9FFF;
    } else {
        ((BtlRecord *)record)->sprite.attr |= 0x2000;
    }
}
