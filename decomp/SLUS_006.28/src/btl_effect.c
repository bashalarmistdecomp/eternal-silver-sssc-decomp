/* Lunar SSSC — btl_effect: the effects the shows put on the picture and the
   records: the mix flash and colour, the show's animation play, the tile
   layer's end, scroll and patterns, the wave, the record's scale, the
   colour pulse, the acting slot's start and end, the travel paths and run,
   the spot move and the arc path. The first split had it in psyq_gpu_b.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"
/* g_BtlShowPulse: the window pulse BtlShowPulseSet arms and BtlShowPulseStep runs
   each frame (MenuWin in the first split's menu.c): +0 on, +4 phase (toggled
   per pulse), +8 window length, +0xC / +0x12 the two window triples it
   alternates. */
extern BtlShowPulse g_BtlShowPulse;
/* The pan request BtlTravelRun plays (btl.h's BtlTravelBlock, same layout):
   move display block `disp` from one record to `target` along two point
   tables BtlTravelPathsMake allocates (+0x14 x/y, +0x18 the height track). */
/* The raster wave BtlShowWaveSet arms and BtlShowWaveStep steps each frame
   (g_BtlShowWaveBlock): +0 on, +4 direction (non-zero ramps the amplitude up),
   +8 frame counter, +0xA / +0xC the two s8 arguments (+0xC = frames per
   step), +0x10 mode (4 / 8 pick the extra caller), +0x14 the 16-entry
   offset table. g_BtlShowWaveLines holds the wave's line table (BtlShowWaveLines,
   btl.h). */
extern s32 CharStats;
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlSlotIds[];
s32 BtlTravelPathsMake(BtlTravelBlock *);
void BtlShowAnimationPlay(void *blk, u16 anim, s32 blink);
s32 BtlTileScrollMove(s32, s8, s8);                         /* extern */
extern u8 g_BtlActingSlot;
extern s32 g_BtlRecords;
extern void *g_BtlShowRecords;
/* g_BtlMixFlash: the screen-shake state BtlMixFlashSet arms. */
extern BtlMixFlash g_BtlMixFlash;
s32 layer_mix_set(s32, s32);

/* func_8008A004 */
void BtlMixFlashSet(s32 win, u8 mode)
{
  if (mode == 0)
  {
    g_BtlMixFlash.mode = 0;
    return;
  }
  g_BtlMixFlash.mode = mode;
  g_BtlMixFlash.on = 1;
  g_BtlMixFlash.mask = win;
  switch (mode)
  {
    case 1:
    case 3:
      g_BtlMixFlash.count = 8;
      break;
    case 2:
      g_BtlMixFlash.count = 0xA;
      break;
    case 5:
      g_BtlMixFlash.count = 0;
      break;
  }
  g_BtlMixFlash.frame = 0;
  layer_mix_set(g_BtlMixFlash.mask, 0x14);
}
extern s16 g_BtlAmountShowFrameCount;
extern u8 g_BtlSlotAmountKinds[];
extern u16 g_BtlMixFlashXOffsets[];
extern u16 g_BtlMixFlashCols[];
void sprite_mix_col_set_copy(s32 col);  /* TU defines this below */

/* The per-frame shake/flash runner for g_BtlMixFlash (armed by
   BtlMixFlashSet; called from the battle frame BtlFrameStep). It counts
   g_BtlAmountShowFrameCount and, by mode: 5 tints with g_BtlMixFlashCols[phase % 6] and bumps
   the amplitude on step 5; 1 and 3 toggle window `win` (0x14 shown, 0xFF
   hidden) every `amp` frames, halving `amp` each time it shows, and
   stop with it shown once `amp` is 1; 2 jitters every flagged object's x
   (g_BtlSlotAmountKinds kind 1, 3 or 4) by g_BtlMixFlashXOffsets[phase] until phase reaches
   `amp`. Every mode but a finished one advances `phase`. */
/* func_8008A0C0 */
void BtlMixFlashStep(void)
{
  u16 i;
  u8 k;
  s32 m;
  if (g_BtlMixFlash.mode == 0)
  {
    return;
  }
  g_BtlAmountShowFrameCount++;
  m = g_BtlMixFlash.mode;
  switch (m)
  {
    case 5:
      sprite_mix_col_set_copy(g_BtlMixFlashCols[(s8) g_BtlMixFlash.frame % 6]);
      if ((s8) g_BtlMixFlash.frame % 6 == 5)
      {
        g_BtlMixFlash.count++;
      }
      break;

    case 1:
    case 3:
      if ((s8) g_BtlMixFlash.count == 1)
      {
        layer_mix_set(g_BtlMixFlash.mask, 0x14);
        return;
      }
      if ((s8) g_BtlMixFlash.frame < (s8) g_BtlMixFlash.count)
      {
        break;
      }
      g_BtlMixFlash.frame = 0;
      if (g_BtlMixFlash.on != 0)
      {
        g_BtlMixFlash.on = 0;
        layer_mix_set(g_BtlMixFlash.mask, 0xFF);
        return;
      }
      g_BtlMixFlash.on = 1;
      layer_mix_set(g_BtlMixFlash.mask, 0x14);
      g_BtlMixFlash.count = (s8) g_BtlMixFlash.count / 2;
      return;

    case 2:
      if ((s8) g_BtlMixFlash.frame >= (s8) g_BtlMixFlash.count)
      {
        return;
      }
      for (i = 0; i < 0xE; i++)
      {
        k = g_BtlSlotAmountKinds[i];
        if ((k == 1) || (k == 3) || (k == 4))
        {
          ((BtlRecord *) g_BtlRecords)[i].sprite.x += g_BtlMixFlashXOffsets[(s8) g_BtlMixFlash.frame];
        }
      }
      break;
  }
  g_BtlMixFlash.frame++;
}

s32 BtlAnimationFramesCount(void *);
s32 BtlShowFrameStep();
void BtlAnimationSetCopy(s32 record, s32 animation);  /* TU defines this below */

/* Play a display block's animation to its end (game code): sets anim `anim` on
   the block unless it is 0xFF (BtlAnimationSetCopy), then lets BtlAnimationFramesCount(blk)
   frames pass, one BtlShowFrameStep each. With `blink` set it waits twice as many
   frames, writing the frame counter's low bit to the block's word at +4 each
   frame, and leaves that word 1. */
/* func_8008A344 */
void BtlShowAnimationPlay(void *blk, u16 anim, s32 blink)
{
    u16 n;
    u16 i;

    if (anim != 0xFF) {
        BtlAnimationSetCopy((s32)blk, anim);
    }
    n = BtlAnimationFramesCount(blk);
    if (blink != 0) {
        n *= 2;
        for (i = 0; i < n; i++) {
            ((BtlRecord *)blk)->sprite.on = i & 1;
            BtlShowFrameStep();
        }
        ((BtlRecord *)blk)->sprite.on = 1;
    } else {
        for (i = 0; i < n; i++) {
            BtlShowFrameStep();
        }
    }
}

/* 1 while a display block's animation frame (+0x38, low 15 bits) is below its
   last step, (+0x3A & 0x7FFF) - 1 taken as a u16 (game code). */
/* func_8008A408 */
s32 BtlAnimationPlayCheck(void *blk)
{
    return (((BtlRecord *)blk)->step & 0x7FFF) < (u16)((((BtlRecord *)blk)->end & 0x7FFF) - 1);
}

s32 tile_layer_end();                                  /* extern */
s32 layer_mix_switch_on(s32);                                 /* extern */
void BtlShowWaveSet(s32, s8, s8, s32, s32 *);                   /* extern */

/* func_8008A428 */
void BtlTileEnd(void) {
    BtlShowWaveSet(0, 1, 0, 1, 0);
    layer_mix_switch_on(0x100);
    tile_layer_end();
}
s32 tile_scroll_set(s32, s32, s32);

/* tile_scroll_set(x, y, 0); the first argument is not used (game code; the
   callers pass 8). */
/* Ruling of 2026-10-14 (docs/naming.md): Not exactly tile_scroll_set: it takes x and y as second and third arguments; named for what it does. */
/* func_8008A468 */
void BtlTileScrollSet(s32 unused, s32 x, s32 y)
{
    tile_scroll_set(x, y, 0);
}

s32 tile_scroll_move(s8, s8, s32);

/* Ruling of 2026-10-14 (docs/naming.md): Not exactly tile_scroll_move: it takes the two moves as second and third arguments; named for what it does. */
/* func_8008A490 */
s32 BtlTileScrollMove(s32 unused, s8 dx, s8 dy) {
    return tile_scroll_move(dx, dy, 0);
}

s32 BtlShowFrameStep();                                  /* extern */

/* func_8008A4C0 */
void BtlTileScrollStep(s8 dx, s8 dy) {
    BtlTileScrollMove(8, dx, dy);
    BtlShowFrameStep();
}
extern BtlShowWave *g_BtlShowWaveBlock;
extern BtlShowWaveLines *g_BtlShowWaveLines;
void tile_wave_switch_set(s32);

/* func_8008A4F8 */
void BtlShowWaveSet(s32 mode, s8 a, s8 b, s32 table, s32 *tbl)
{
  BtlShowWave *w;
  if (mode == 0)
  {
    g_BtlShowWaveBlock->on = 0;
    return;
  }
  if (mode == 8)
  {
    tile_wave_switch_set(1);
  }
  w = g_BtlShowWaveBlock;
  w->grow = table;
  w->on = 1;
  w->unk_0a = a;
  w->step_frames = b;
  if (w->grow != 0)
  {
    w->mode = mode;
    g_BtlShowWaveLines->unk_002 = 1;
    g_BtlShowWaveLines->unk_004 = 0;
    g_BtlShowWaveBlock->frame_count = 0;
    g_BtlShowWaveBlock->table = tbl;
  }
  else
  {
    w->frame_count = w->frame_count % 32;
  }
}
void BtlBgWaveSet();
void tile_wave_set(s32, s32);

/* func_8008A5E4 */
void BtlShowWaveStep(void)
{
  s32 buf[16];
  s32 mode;
  s32 q;
  s32 amp;
  s32 ph;
  u32 i;
  BtlShowWave *w;
  mode = g_BtlShowWaveBlock->mode;
  if ((g_BtlShowWaveBlock == 0) || (g_BtlShowWaveBlock->on == 0))
  {
    if (mode == 4)
    {
      BtlBgWaveSet(0);
    }
    if (mode == 8)
    {
      tile_wave_switch_set(0);
    }
    return;
  }
  if (mode == 8)
  {
    tile_wave_switch_set(1);
  }
  w = g_BtlShowWaveBlock;
  q = w->frame_count / w->step_frames;
  if ((w->frame_count % w->step_frames) == 0)
  {
    if (w->frame_count < (w->step_frames * 32))
    {
      if (w->grow != 0)
      {
        amp = q;
      }
      else
      {
        amp = 0x20 - q;
      }
    }
    else
    {
      amp = (w->grow != 0) << 5;
    }
    ph = ((u16) g_BtlShowWaveBlock->frame_count >> 1) & 0xF;
    w = g_BtlShowWaveBlock;
    for (i = 0; i < 16; i++)
    {
      buf[i] = (w->table[(i + ph) & 0xF] * amp) / 32;
    }
    for (i = 0; i < 0xE0; i++)
    {
      g_BtlShowWaveLines->line[i].x = buf[i & 0xF];
    }
    if (mode == 4)
    {
      BtlBgWaveSet(amp, ph);
    }
    if (mode == 8)
    {
      tile_wave_set(amp, ph);
    }
  }
  g_BtlShowWaveBlock->frame_count++;
}

/* func_8008A7CC */
void BtlRecordScaleSet(void *record, s32 x_scale, s16 y_scale, s32 on) {
    if (on != 0) {
        ((BtlRecord *)record)->sprite.scaled = 1;
        ((BtlRecord *)record)->sprite.scale_x = x_scale;
        ((BtlRecord *)record)->sprite.scale_y = y_scale;
        return;
    }
    ((BtlRecord *)record)->sprite.scaled = 0;
    ((BtlRecord *)record)->sprite.scale_y = 0x100;
    ((BtlRecord *)record)->sprite.scale_x = 0x100;
}
/* func_8008A7F8 */
s32 BtlShowPulseSet(s32 len, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
  g_BtlShowPulse.on = 0;
  g_BtlShowPulse.next = 0;
  g_BtlShowPulse.length = len;
  g_BtlShowPulse.colour_1[0] = a;
  g_BtlShowPulse.colour_1[1] = b;
  g_BtlShowPulse.colour_1[2] = c;
  g_BtlShowPulse.colour_2[0] = d;
  g_BtlShowPulse.colour_2[1] = e;
  g_BtlShowPulse.colour_2[2] = f;
}
/* func_8008A834 */
s32 BtlShowPulseClear(void)
{
  g_BtlShowPulse.on = 0;
  g_BtlShowPulse.next = 0;
  g_BtlShowPulse.length = 0;
  g_BtlShowPulse.colour_1[0] = 0;
  g_BtlShowPulse.colour_1[1] = 0;
  g_BtlShowPulse.colour_1[2] = 0;
  g_BtlShowPulse.colour_2[0] = 0;
  g_BtlShowPulse.colour_2[1] = 0;
  g_BtlShowPulse.colour_2[2] = 0;
}
s32 layer_col_fade_check(s32);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);

/* func_8008A864 */
s32 BtlShowPulseStep(void)
{
  BtlShowPulse *w;
  w = &g_BtlShowPulse;
  if (g_BtlShowPulse.on != 0)
  {
    if ((layer_col_fade_check(0) & 0xFF) == 0)
    {
      if (w->next != 0)
      {
        layer_col_fade_start(0, 0xC, 1, w->length, (s32) w->colour_1);
      }
      else
      {
        layer_col_fade_start(0, 0xC, 1, w->length, (s32) w->colour_2);
      }
      g_BtlShowPulse.next ^= 1;
    }
  }
}
extern u8 g_BtlTilePatternLists[];

/* func_8008A8F8 */
u8 *BtlTilePatternListGet(u8 n)
{
  u32 i;
  u32 k;
  u8 m;
  if (n == 0)
  {
    return 0;
  }
  m = n - 1;
  i = 0;
  k = 0;
  while (i < m)
  {
    if (g_BtlTilePatternLists[k] == 0xFF)
    {
      i++;
    }
    k++;
  }
  return &g_BtlTilePatternLists[k];
}

extern u8 g_BtlShowVoiceState;
extern void *g_BtlEffectRow;

/* Start the acting actor's presentation animation (game code). With `cue` set
   and the presentation's set-up sound id g_BtlEffectRow[2] non-zero it raises
   g_BtlShowVoiceState (the battle frame then starts that clip). A non-zero anim (its low
   byte) is played to its end on the actor's display block, g_BtlRecords +
   g_BtlActingSlot * 0x40 (BtlShowAnimationPlay). With anim 0 the actor block's first word
   is cleared, the first word of the block at g_BtlShowRecords + 0x500 is set to 1 and
   BtlShowAnimationPlay runs on that block with anim 0. */
/* func_8008A95C */
void BtlShowActingPlay(s32 anim, s32 cue)
{
    u8 *disp;

    if (cue != 0 && *((u8 *)g_BtlEffectRow + 2) != 0) {
        g_BtlShowVoiceState = 1;
    }
    if ((anim & 0xFF) != 0) {
        BtlShowAnimationPlay((void *)(g_BtlRecords + (g_BtlActingSlot << 6)), anim & 0xFF, 0);
    } else {
        disp = g_BtlShowRecords;
        ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->on = 0;
        ((BtlRecord *)disp)[20].on = 1;
        BtlShowAnimationPlay(disp + 0x500, 0, 0);
    }
}

/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */

s32 BtlActingRestAnimationSet();                                  /* extern */

/* func_8008A9F8 */
void BtlShowActingEnd(void) {
    ((BtlRecord *)g_BtlShowRecords)[20].on = 0;
    ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->on = 1;
    BtlActingRestAnimationSet();
}
void BtlFacingTowardSet(u8 *, s16);
s32 xmalloc_heap_alloc(s32);
void line_path_make(u16 *, s16, s16, s16, s32, s32);

/* func_8008AA3C */
s32 BtlTravelPathsMake(BtlTravelBlock *req)
{
  BtlRecord *disp;
  BtlRecord *tgt;
  s16 x0;
  s16 x1;
  s16 y0;
  s16 y1;
  s16 n;
  s32 d;
  s32 t;
  disp = (BtlRecord *) req->from;
  tgt = (BtlRecord *) req->to;
  x0 = disp->sprite.x;
  x1 = tgt->sprite.x;
  if (req->mode != 0)
  {
    if (x0 < x1)
    {
      t = x1 - 8;
      if ((t - req->to_dx < x0 + req->from_dx) && (x0 + req->from_dx < x1 - req->to_dx))
      {
        x1 = x0;
      }
      else
      {
        x1 = tgt->sprite.x - (req->to_dx + req->from_dx);
      }
      if (x1 < req->from_dx + 8)
      {
        x1 = tgt->sprite.x + (req->to_dx + req->from_dx);
      }
    }
    else
    {
      if ((x1 + req->to_dx < x0 - req->from_dx) && (x0 - req->from_dx < (t = x1 + 8) + req->to_dx))
      {
        x1 = x0;
      }
      else
      {
        x1 = tgt->sprite.x + (req->to_dx + req->from_dx);
      }
      if (x1 > 0x138 - req->from_dx)
      {
        x1 = tgt->sprite.x - (req->to_dx + req->from_dx);
      }
    }
    BtlFacingTowardSet((u8 *) disp, x1);
  }
  else
  {
    if ((x0 - req->from_dx <= x1 + req->to_dx) && (x1 - req->to_dx <= x0 + req->from_dx))
    {
      x1 = x0;
    }
    else if (x0 + req->from_dx < x1 - req->to_dx)
    {
      x0 = disp->sprite.x + req->from_dx;
      x1 = tgt->sprite.x - req->to_dx;
    }
    else
    {
      x0 = disp->sprite.x - req->from_dx;
      x1 = tgt->sprite.x + req->to_dx;
    }
  }
  y0 = disp->sprite.y + req->from_dy;
  y1 = tgt->sprite.y + req->to_dy;
  if ((g_BtlSlotActiveFlags[g_BtlActingSlot] != 0) && !(((BtlStats *) (CharStats + g_BtlActingSlot * 0x3C))->status & 0x3C))
  {
    BtlFacingTowardSet((u8 *) tgt, disp->sprite.x);
  }
  if (g_BtlSlotIds[6] == 0x77)
  {
    ((BtlRecord *) g_BtlRecords)[6].sprite.attr &= 0x9FFF;
  }
  d = x1 - x0;
  if (d < 0)
  {
    d = x0 - x1;
  }
  n = (d + ((y1 - y0 >= 0) ? y1 - y0 : y0 - y1)) / req->step;
  if (n <= 0)
  {
    n = 1;
  }
  req->path = xmalloc_heap_alloc(n * 4);
  req->depth_path = xmalloc_heap_alloc(n * 4);
  line_path_make((u16 *) req->path, x0, y0, x1, y1, n);
  line_path_make((u16 *) req->depth_path, disp->sprite.x, disp->sprite.y, tgt->sprite.x, tgt->sprite.y, n);
  return n;
}

/* far_syms: g_BtlRecords */
/* far_syms: CharStats */
void BtlFacingTowardSet(u8 *, s16);
s32 xmalloc_heap_free(s32);

/* func_8008AE10 */
void BtlTravelRun(BtlTravelBlock *req, u16 page)
{
  s16 i;
  s16 n;
  BtlRecord *disp;
  BtlRecord *tgt;
  disp = (BtlRecord *) req->from;
  tgt = (BtlRecord *) req->to;
  BtlAnimationSetCopy((s32) disp, page);
  n = BtlTravelPathsMake(req);
  for (i = 0; i < n; i++)
  {
    disp->sprite.x = ((PathPoint *) req->path)[i].x;
    disp->sprite.y = ((PathPoint *) req->path)[i].y;
    disp->sprite.depth = ((PathPoint *) req->depth_path)[i].y + 1;
    BtlShowFrameStep();
  }
  if (req->mode != 0)
  {
    BtlFacingTowardSet((u8 *) disp, tgt->sprite.x);
    if ((g_BtlSlotActiveFlags[g_BtlActingSlot] != 0) && !(((BtlStats *) (CharStats + g_BtlActingSlot * 0x3C))->status & 0x3C))
    {
      BtlFacingTowardSet((u8 *) tgt, disp->sprite.x);
    }
    if (g_BtlSlotIds[6] == 0x77)
    {
      ((BtlRecord *) g_BtlRecords)[6].sprite.attr &= 0x9FFF;
    }
  }
  xmalloc_heap_free(req->path);
  xmalloc_heap_free(req->depth_path);
}
s32 xmalloc_heap_alloc(s32);
void line_path_make(u16 *, s16, s16, s16, s32, s32);

/* func_8008AFAC */
void BtlRecordSpotMove(s32 blk, s16 x, s16 y, s16 n, u8 page)
{
  PathPoint *path;
  s16 i;
  if (page != 0xFF)
  {
    BtlAnimationSetCopy(blk, page);
  }
  path = (PathPoint *) xmalloc_heap_alloc(n * 4);
  line_path_make((u16 *) path, ((BtlRecord *) blk)->sprite.x, ((BtlRecord *) blk)->sprite.y, x, y, n);
  for (i = 0; i < n; i++)
  {
    ((BtlRecord *) blk)->sprite.x = path[i].x;
    ((BtlRecord *) blk)->sprite.y = path[i].y;
    ((BtlRecord *) blk)->sprite.depth = path[i].y + 1;
    BtlShowFrameStep();
  }
  xmalloc_heap_free((s32) path);
}
/* func_8008B0B4 */
void arc_path_make(s16 x0, s16 y0, s16 x1, s16 y1, s16 n, s16 height, s16 *pts)
{
  s16 i;
  s16 h;
  s16 *p;

  line_path_make((u16 *) pts, x0, y0, x1, y1, n);
  i = 1;
  h = -(n / 2);
  if (i < n)
  {
    p = pts;
    do
    {
      p[1] += (height * (i + h) * (i + h)) / (h * h) - height;
      p += 2;
      i++;
    } while (i < n);
  }
}

s32 BtlAnimationSet(s32, s32);

/* Ruling of 2026-10-14 (docs/naming.md): Does what BtlAnimationSet does; nothing read says why there are two. */
/* func_8008B1FC */
void BtlAnimationSetCopy(s32 record, s32 animation) {
    BtlAnimationSet(record, animation & 0xFFFF);
}
extern u8 *g_BtlTilePatternList;
s32 BtlTilePatternPut(s32, s32);

/* func_8008B21C */
s32 BtlTilePatternStep(u16 i, s32 mode)
{
  if (g_BtlTilePatternList[i] == 0xFF)
  {
    i = 0;
  }
  BtlTilePatternPut(g_BtlTilePatternList[i++], (u16) mode);
  return i;
}
s32 sprite_mix_col_set();

/* The battle tint for an effect id (game code, not a library function): maps the
   id to a 15-bit BGR colour and hands it to sprite_mix_col_set. 0 -> 0 (black),
   0xE4 -> 0x8000, 0xE5 -> 0x4210 and 0xE6 -> 0x2108 (greys), 0xB1 -> 0x1F (red),
   0xB2 -> 0x18 (dark red), 0xC1 -> 0x21F (orange), 0xC5 -> 0x3FF (yellow),
   0xD1 -> 0x3E0 (green), 0xB5 -> 0x7C00 (blue), anything else 0x7FFF (white).
   What the bytes fix: the default is the last block (it falls into the call),
   the blocks 0, 0xE4, 0xE5, 0xE6, 0xB2, 0xC5, 0xB5 come in this order, the
   result is built in the parameter, and the switch names eleven values, one of
   them between 0xD2 and 0xE3 with the default's colour. What they do not fix:
   where the 0xB1, 0xC1 and 0xD1 blocks sit among the others (written here in
   colour order) and which id the eleventh is (0xE1 is the only id in that range
   a direct call passes, from BtlMenuOpen).
   MARKED SPELLING: `case 0xE1:` in front of `default:`. The compare tree needs
   an eleventh case value between 0xD2 and 0xE3 with the default's colour (38 of
   59 without one); which value the original named is not known. */
/* func_8008B27C */
void BtlMixColNumberSet(u16 id)
{
    switch (id) {
    case 0:
        id = 0;
        break;
    case 0xE4:
        id = 0x8000;
        break;
    case 0xE5:
        id = 0x4210;
        break;
    case 0xE6:
        id = 0x2108;
        break;
    case 0xB1:
        id = 0x1F;
        break;
    case 0xB2:
        id = 0x18;
        break;
    case 0xC1:
        id = 0x21F;
        break;
    case 0xC5:
        id = 0x3FF;
        break;
    case 0xD1:
        id = 0x3E0;
        break;
    case 0xB5:
        id = 0x7C00;
        break;
    case 0xE1:
    default:
        id = 0x7FFF;
        break;
    }
    sprite_mix_col_set(id);
}

s32 sprite_mix_col_set();                                  /* extern */

/* A thunk: callers pass the tint colour, which it hands to sprite_mix_col_set. */
/* Ruling of 2026-10-14 (docs/naming.md): Does what sprite_mix_col_set does; nothing read says why there are two. */
/* func_8008B368 */
void sprite_mix_col_set_copy(s32 col) {
    sprite_mix_col_set(col);
}
