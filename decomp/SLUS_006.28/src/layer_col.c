/* Lunar SSSC — layer_col: the layers' colours and mixes: the flat draw, the
   mix modes and weights, the colour apply and its fades, the mix fades and
   sets, the checks and the order. The first split had it in ui.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
extern u8 g_LayerBgSecondRecord[];
extern u8 g_LayerBgFirstRecord[];
/* The colour/fade ramp records (0x24 bytes): 0x800AE16C (flag 4),
   0x800AE190 (8), 0x800AE1B4 (0x100), 0x800AE1D8 (0x80). A ramp runs from
   `from` toward `cur` over `dur` with `delta = cur - from`; +0x1C..+0x22
   are the window fields layer_mix_fade_start / layer_order_set use. */
extern u8 *g_PacketWritePointer;
extern u32 *g_PacketOtBase;
void layer_flat_draw(u8, u8, u8, s32);
extern u8 g_LayerSpriteRecord[];
s32 bg_col_set(s32);
s32 sprite_col_set(s32);
void layer_col_apply(s32);
extern u8 g_BtlRunning;
void tile_semi_trans_set(s8 on);
s32 BtlBgGreySet(s32);
void tile_mix_set(s32 level);
void tile_mix_fade_start(s32 arg0, s32 length, s32 level);
s32 tile_mix_fade_check(void);
s16 layer_col_fade_div(s16 value, s16 divisor);
void func_8001E048();
s16 layer_col_conv(s32 col);
void func_8001EE60();
void layer_col_fade_step();
void func_8001E048();
void func_8001EE60();
extern s16 g_MapCol;
/* far_syms: g_MapCol */
void func_8001E048();
void func_8001EE60();
void tile_semi_trans_set(s8 on);
s32 BtlBgGreySet(s32);
void tile_mix_set(s32 level);
void tile_mix_fade_start(s32 arg0, s32 length, s32 level);
s32 tile_mix_fade_check(void);
s16 layer_col_fade_div(s16 value, s16 divisor);
s16 layer_col_conv(s32 col);
void layer_col_fade_step();
s32 layer_mix_fade_check(s32 layer_mask);
void layer_col_set(s32 unused, s32 layer_mask, s32 target_col, s32 unused2);
void layer_mix_set(s32 layer_mask, u16 level);
/* far_syms: g_PacketWritePointer */
/* far_syms: g_PacketOtBase */
s32 layer_mix_fade_check(s32 layer_mask);
void layer_col_set(s32 unused, s32 layer_mask, s32 target_col, s32 unused2);
void layer_mix_set(s32 layer_mask, u16 level);
s32 BtlBgGreySet(s32);

/* func_8001DE94 */
s16 layer_col_fade_div(s16 value, s16 divisor) {
    s16 nonzero_divisor;

    nonzero_divisor = divisor;
    if ((divisor << 0x10) == 0) {
        nonzero_divisor = 1;
    }
    return (s16) (value / nonzero_divisor);
}
/* far_syms: g_PacketWritePointer (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
/* far_syms: g_PacketOtBase (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */

/* func_8001DEF4 */
void layer_flat_draw(u8 r, u8 g, u8 b, s32 otz)
{
  POLY_F4 *poly;
  DR_TPAGE *tp;
  poly = (POLY_F4 *) g_PacketWritePointer;
  SetPolyF4(poly);
  SetSemiTrans(poly, 1);
  poly->r0 = r;
  poly->g0 = g;
  poly->b0 = b;
  poly->x0 = 0;
  poly->y0 = 0;
  poly->x1 = 0x140;
  poly->y1 = 0;
  poly->x2 = 0;
  poly->y2 = 0xE0;
  poly->x3 = 0x140;
  poly->y3 = 0xE0;
  AddPrim(g_PacketOtBase + otz, poly++);
  g_PacketWritePointer = (u8 *) poly;
  tp = (DR_TPAGE *) g_PacketWritePointer;
  tp->tag = 0x1000000;
  tp->code[0] = 0xE1000220;
  AddPrim(g_PacketOtBase + otz, tp);
  g_PacketWritePointer = (u8 *) (tp + 1);
}

/* func_8001DFE8 */
s32 layer_mix_mode_get(void)
{
  u8 *p;
  s32 r;
  s32 v;
  p = g_LayerBgSecondRecord;
  v = ((LayerRecord *)p)->mix;
  if ((u32)(v - 0xE) < 5U)
  {
    return 1;
  }
  if ((u32)(v - 1) < 0x1FU)
  {
    return 5;
  }
  r = 0;
  if (((LayerRecord *)g_LayerBgFirstRecord)->order < ((LayerRecord *)p)->order)
  {
    r = 3;
  }
  return r;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one call says which mix mode, not what was to be done with it. */
void func_8001E048() {

}
/* func_8001E050 */
s16 layer_col_conv(s32 col) {
    s32 level;
    u16 clamped_level;

    level = (s32) ((col + 0x100) << 0x10) >> 0x11;
    clamped_level = level;
    if (level < 0) {
        clamped_level = 0;
    }
    if ((s16) clamped_level >= 0x100) {
        clamped_level = 0xFF;
    }
    return (s16) clamped_level;
}
extern s32 g_LayerMixMode;
extern s8 g_BgMode;

/* func_8001E08C */
void layer_mix_weights_get(s16 *weights)
{
  s16 *complement_weight;
  /* CODEGEN FORCING DEVICE (address of a global in a local), not recovered source:
     `bg_second_record` only holds the address of `g_LayerBgSecondRecord`, through the type `u8
     (*)[]`; behaviour is the same with the address written where it is used, but then 6
     of this function's 32 instruction lines differ. */
  u8 (*bg_second_record)[];
  int mix_offset_then_complement;
  if ((g_LayerMixMode != 1) && (g_LayerMixMode != 5))
  {
    weights[0] = 0x10U;
    weights[1] = 0x10U;
    return;
  }
  if (g_BgMode != 1)
  {
    bg_second_record = &g_LayerBgSecondRecord;
    complement_weight = &weights[1];
    weights[0] = (u16) (AS(s32, ((LayerRecord *)bg_second_record)->mix));
    *complement_weight = (u16) (0x20 - AS(s32, ((LayerRecord *)bg_second_record)->mix));
    return;
  }
  mix_offset_then_complement = 0x1c;
  complement_weight = &weights[0];
  weights[1] = (u16) (*((s32 *) (((u8 *) (&g_LayerBgFirstRecord)) + mix_offset_then_complement)));
  mix_offset_then_complement = 0x20 - (*((u16 *) (((u8 *) (&g_LayerBgFirstRecord)) + mix_offset_then_complement)));
  *complement_weight = (u16) mix_offset_then_complement;
}
/* far_syms: g_MapColSecond */
/* far_syms: g_MapColThird */
extern s16 g_MapColSecond;
extern s16 g_MapColThird;
extern s16 g_LayerColByte2;
extern s16 g_LayerColByte1;
extern s16 g_LayerColByte0;
extern s16 g_LayerColShift;

/* func_8001E10C */
void layer_col_apply(s32 flags)
{
  s16 pair[2];
  s32 hi;
  hi = (g_BgMode == 1) << 25;
  g_LayerMixMode = layer_mix_mode_get();
  layer_mix_weights_get(pair);
  if (flags & 0xC)
  {
    g_LayerColShift = 7;
    if (g_LayerMixMode == 5)
    {
      g_LayerColShift = 8;
    }
    g_LayerColByte2 = (layer_col_conv(g_MapCol) * ((LayerRecord *) g_LayerBgFirstRecord)->value[0]) >> g_LayerColShift;
    g_LayerColByte1 = (layer_col_conv(g_MapColSecond) * ((LayerRecord *) g_LayerBgFirstRecord)->value[1]) >> g_LayerColShift;
    g_LayerColByte0 = (layer_col_conv(g_MapColThird) * ((LayerRecord *) g_LayerBgFirstRecord)->value[2]) >> g_LayerColShift;
    g_LayerColByte2 = (g_LayerColByte2 * pair[0]) >> 4;
    g_LayerColByte1 = (g_LayerColByte1 * pair[0]) >> 4;
    g_LayerColByte0 = (g_LayerColByte0 * pair[0]) >> 4;
    if (g_LayerColByte2 >= 0x100)
    {
      g_LayerColByte2 = 0xFF;
    }
    if (g_LayerColByte1 >= 0x100)
    {
      g_LayerColByte1 = 0xFF;
    }
    if (g_LayerColByte0 >= 0x100)
    {
      g_LayerColByte0 = 0xFF;
    }
    bg_col_set((g_LayerColByte2 << 16) | (g_LayerColByte1 << 8) | g_LayerColByte0 | hi);
    g_LayerColShift = 7;
    if (g_LayerMixMode == 5)
    {
      g_LayerColShift = 8;
    }
    g_LayerColByte2 = (layer_col_conv(g_MapCol) * ((LayerRecord *) g_LayerBgSecondRecord)->value[0]) >> g_LayerColShift;
    g_LayerColByte1 = (layer_col_conv(g_MapColSecond) * ((LayerRecord *) g_LayerBgSecondRecord)->value[0]) >> g_LayerColShift;
    g_LayerColByte0 = (layer_col_conv(g_MapColThird) * ((LayerRecord *) g_LayerBgSecondRecord)->value[0]) >> g_LayerColShift;
    g_LayerColByte2 = (g_LayerColByte2 * pair[1]) >> 4;
    g_LayerColByte1 = (g_LayerColByte1 * pair[1]) >> 4;
    g_LayerColByte0 = (g_LayerColByte0 * pair[1]) >> 4;
    if (g_LayerColByte2 >= 0x100)
    {
      g_LayerColByte2 = 0xFF;
    }
    if (g_LayerColByte1 >= 0x100)
    {
      g_LayerColByte1 = 0xFF;
    }
    if (g_LayerColByte0 >= 0x100)
    {
      g_LayerColByte0 = 0xFF;
    }
    bg_second_col_set((g_LayerColByte2 << 16) | (g_LayerColByte1 << 8) | g_LayerColByte0);
    func_8001E048(g_LayerMixMode);
  }
  if (flags & 0x100)
  {
    g_LayerColByte2 = (layer_col_conv(g_MapCol) * ((LayerRecord *) g_LayerSpriteRecord)->value[0]) >> 7;
    g_LayerColByte1 = (layer_col_conv(g_MapColSecond) * ((LayerRecord *) g_LayerSpriteRecord)->value[0]) >> 7;
    g_LayerColByte0 = (layer_col_conv(g_MapColThird) * ((LayerRecord *) g_LayerSpriteRecord)->value[0]) >> 7;
    if (g_LayerColByte2 >= 0x100)
    {
      g_LayerColByte2 = 0xFF;
    }
    if (g_LayerColByte1 >= 0x100)
    {
      g_LayerColByte1 = 0xFF;
    }
    if (g_LayerColByte0 >= 0x100)
    {
      g_LayerColByte0 = 0xFF;
    }
    sprite_col_set((g_LayerColByte2 << 16) | (g_LayerColByte1 << 8) | g_LayerColByte0);
  }
}

extern u8 g_LayerFlatRecord[];

/* func_8001E55C */
void layer_col_fade_record_set(s32 flags, u16 dur, u16 unused, s16 *vals)
{
  s16 unused2[3];
  LayerRecord *r0;
  LayerRecord *r3;
  LayerRecord *r3b;
  LayerRecord *r1;
  LayerRecord *r2;
  s16 i;
  if (flags & 4)
  {
    ((LayerRecord *) g_LayerBgFirstRecord)->step = 1;
    ((LayerRecord *) g_LayerBgFirstRecord)->length = dur;
    ((LayerRecord *) g_LayerBgFirstRecord)->start[0] = ((LayerRecord *) g_LayerBgFirstRecord)->target[0];
    ((LayerRecord *) g_LayerBgFirstRecord)->start[1] = ((LayerRecord *) g_LayerBgFirstRecord)->target[1];
    ((LayerRecord *) g_LayerBgFirstRecord)->start[2] = ((LayerRecord *) g_LayerBgFirstRecord)->target[2];
    ((LayerRecord *) g_LayerBgFirstRecord)->target[0] = vals[0];
    ((LayerRecord *) g_LayerBgFirstRecord)->target[1] = vals[1];
    r0 = (LayerRecord *) g_LayerBgFirstRecord;
    r0->target[2] = vals[2];
    for (i = 0; i < 3; i++)
    {
      i[r0->difference] = (i[r0->target] = layer_col_conv(i[r0->target])) - i[r0->start];
    }
  }
  if (flags & 0x80)
  {
    ((LayerRecord *) g_LayerFlatRecord)->step = 1;
    ((LayerRecord *) g_LayerFlatRecord)->length = dur;
    ((LayerRecord *) g_LayerFlatRecord)->start[0] = ((LayerRecord *) g_LayerFlatRecord)->target[0];
    ((LayerRecord *) g_LayerFlatRecord)->start[1] = ((LayerRecord *) g_LayerFlatRecord)->target[1];
    ((LayerRecord *) g_LayerFlatRecord)->start[2] = ((LayerRecord *) g_LayerFlatRecord)->target[2];
    ((LayerRecord *) g_LayerFlatRecord)->target[0] = vals[0];
    r3 = (LayerRecord *) g_LayerFlatRecord;
    r3->target[1] = vals[1];
    r3->target[2] = vals[2];
    for (i = 0; i < 3; i++)
    {
      if (i[r3->target] < 0)
      {
        i[r3->target] = 0;
      }
      if (i[r3->target] >= 0x100)
      {
        i[r3->target] = 0xFF;
      }
    }
    i = 0;
    r3b = (LayerRecord *) g_LayerFlatRecord;
    for (; i < 3; i++)
    {
      i[r3b->difference] = i[r3b->target] - i[r3b->start];
    }
  }
  if (flags & 8)
  {
    r1 = (LayerRecord *) g_LayerBgSecondRecord;
    r1->step = 1;
    r1->length = dur;
    r1->start[0] = r1->target[0];
    r1->difference[0] = (r1->target[0] = layer_col_conv(vals[0])) - r1->start[0];
  }
  if (flags & 0x100)
  {
    r2 = (LayerRecord *) g_LayerSpriteRecord;
    r2->step = 1;
    r2->length = dur;
    r2->start[0] = r2->target[0];
    r2->difference[0] = (r2->target[0] = layer_col_conv(vals[0])) - r2->start[0];
  }
}

/* func_8001E7B0 */
void layer_col_fade_step(void)
{
  LayerRecord *r0;
  LayerRecord *r3;
  LayerRecord *r1;
  LayerRecord *r2;
  LayerRecord *t;
  s16 i;
  if (((LayerRecord *) g_LayerBgFirstRecord)->step <= ((LayerRecord *) g_LayerBgFirstRecord)->length)
  {
    for (i = 0; i < 3; i++)
    {
      r0 = (LayerRecord *) g_LayerBgFirstRecord;
      i[r0->value] = i[r0->start] + layer_col_fade_div(i[r0->difference] * ((LayerRecord *) g_LayerBgFirstRecord)->step, r0->length);
    }
    if (g_BtlRunning != 0)
    {
      BtlBgGreySet(((LayerRecord *) g_LayerBgFirstRecord)->value[0]);
    }
    else
    {
      layer_col_apply(4);
    }
    ((LayerRecord *) g_LayerBgFirstRecord)->step++;
  }
  if (((LayerRecord *) g_LayerFlatRecord)->step <= ((LayerRecord *) g_LayerFlatRecord)->length)
  {
    for (i = 0; i < 3; i++)
    {
      r3 = (LayerRecord *) g_LayerFlatRecord;
      i[r3->value] = i[r3->start] + layer_col_fade_div(i[r3->difference] * ((LayerRecord *) g_LayerFlatRecord)->step, r3->length);
    }
    ((LayerRecord *) g_LayerFlatRecord)->step++;
  }
  r1 = (LayerRecord *) g_LayerBgSecondRecord;
  if (r1->step <= r1->length)
  {
    r1->value[0] = r1->start[0] + layer_col_fade_div(r1->difference[0] * r1->step, r1->length);
    if (g_BtlRunning == 0)
    {
      layer_col_apply(8);
    }
    r1->step++;
  }
  r2 = (LayerRecord *) g_LayerSpriteRecord;
  if (r2->step <= r2->length)
  {
    r2->value[0] = r2->start[0] + layer_col_fade_div(r2->difference[0] * r2->step, r2->length);
    if (g_BtlRunning == 0)
    {
      layer_col_apply(0x100);
    }
    r2->step++;
  }
  t = (LayerRecord *) g_LayerFlatRecord;
  if ((t->value[0] != 0) || (t->value[1] != 0) || (t->value[2] != 0))
  {
    layer_flat_draw(((LayerRecord *) g_LayerFlatRecord)->value[0], ((LayerRecord *) g_LayerFlatRecord)->value[1], ((LayerRecord *) g_LayerFlatRecord)->value[2], g_BtlRunning ? 0x300 : 0x40);
  }
}
extern s16 g_LayerColLowest[];
extern s16 g_LayerColHighest[];

/* Run a window / start a colour ramp: flag 0x80000 adds 0x18C, flag 4
   adds 0x80; then by the 5th argument p (a triple of s16 or a sentinel):
   0 -> the default triple &g_MapCol, -1 -> g_LayerColLowest, -2 ->
   g_LayerColHighest; any other p: outside battle with p[0] == p[1] == p[2], flag
   0x84 (all zero) or 4, otherwise flag 4 swapped for 0x80. Every path ends
   in layer_col_fade_record_set(flags, len, unused2, triple); unused is unused. The switch
   on (u32) p is the target's unsigned compare tree, and its `sltiu ..., -1`
   needs maspsx --aspsx-version 2.60. */
/* func_8001EA74 */
s32 layer_col_fade_start(s32 unused, s32 flags, s32 unused2, s32 len, s16 *p)
{
  if (flags & 0x80000)
  {
    flags |= 0x18C;
  }
  if (flags & 4)
  {
    flags |= 0x80;
  }
  switch ((u32) p)
  {
    case 0:
      layer_col_fade_record_set(flags, len, unused2, &g_MapCol);
      break;
    case 0xFFFFFFFF:
      layer_col_fade_record_set(flags, len, unused2, g_LayerColLowest);
      break;
    case 0xFFFFFFFE:
      layer_col_fade_record_set(flags, len, unused2, g_LayerColHighest);
      break;
    default:
      if ((g_BtlRunning == 0) && (p[0] == p[1]) && (p[0] == p[2]))
      {
        if (p[0] == 0)
        {
          flags |= 0x84;
        }
        else
        {
          flags |= 4;
        }
      }
      else
      {
        flags = (flags & ~4) | 0x80;
      }
      layer_col_fade_record_set(flags, len, unused2, p);
      break;
  }
}

s32 layer_mix_fade_check(s32 layer_mask);  
/* func_8001EB74 */
void layer_col_set(s32 unused, s32 layer_mask, s32 target_col, s32 unused2)
{
  layer_col_fade_start(unused, layer_mask, 1, 1, target_col);
  layer_col_fade_step();
}
/* far_syms: g_BtlRunning */
void layer_col_apply(s32);

/* func_8001EBA4 */
void layer_mix_fade_start(s32 layer_mask, s32 arg1, s32 length, s8 level)
{
  u8 *rec0;
  u8 *rec1;
  if (layer_mask & 4)
  {
    if (level < 0x20)
    {
      rec0 = g_LayerBgFirstRecord;
      ((LayerRecord *)rec0)->mix = level;
      ((LayerRecord *)rec0)->mix_on = 1;
    }
    else
    {
      ((LayerRecord *)g_LayerBgFirstRecord)->mix_on = 0;
    }
    layer_col_apply(layer_mask);
  }
  if (layer_mask & 8)
  {
    if (g_BtlRunning != 0)
    {
      tile_semi_trans_set(1);
      tile_mix_fade_start(arg1, length, level);
      return;
    }
    if (level < 0x20)
    {
      rec1 = g_LayerBgSecondRecord;
      ((LayerRecord *)rec1)->mix = level;
      ((LayerRecord *)rec1)->mix_on = 1;
    }
    else
    {
      ((LayerRecord *)g_LayerBgSecondRecord)->mix_on = 0;
    }
    layer_col_apply(layer_mask);
  }
}

/* func_8001ECA0 */
void layer_mix_set(s32 layer_mask, u16 level)
{
    if (layer_mask & 4) {
        if (g_BtlRunning == 0) {
            layer_mix_fade_start(layer_mask, 1, 1, (s8)level);
        }
    }

    if (layer_mask & 8) {
        if (g_BtlRunning != 0) {
            tile_mix_set(level & 0xFF);
        } else {
            layer_mix_fade_start(layer_mask, 1, 1, (s8)level);
        }
    }

    if (layer_mask & 0x100) {
        sprite_mix_set(0, level & 0xFF);
    }

    if (layer_mask & 0x200) {
        sprite_mix_set(1, level & 0xFF);
    }

    if (layer_mask & 0x400) {
        sprite_mix_set(2, level & 0xFF);
    }

    if (layer_mask & 0x800) {
        sprite_mix_set(3, level & 0xFF);
    }
}

/* far_syms: g_BtlRunning (target reaches it %hi/%lo) */

/* func_8001ED94 */
s32 layer_mix_fade_check(s32 layer_mask) {
    if (layer_mask & 8) {
        if (g_BtlRunning != 0) {
            return tile_mix_fade_check();
        }
        /* Duplicate return node #4. Try simplifying control flow for better match */
        return 0;
    }
    return 0;
}
/* Is any colour/fade ramp still running? 1 while a record's frame counter
   has not passed its duration, checked in the order 4, 8, 0x80, 0x100
   (0x800AE16C, 0x800AE190, 0x800AE1D8, 0x800AE1B4); `unused` is unused
   (every caller passes 0). The `while (layer_col_fade_check(0) & 0xFF)` waits. */
/* func_8001EDDC */
s32 layer_col_fade_check(s32 unused)
{
  if (((LayerRecord *) g_LayerBgFirstRecord)->step <= ((LayerRecord *) g_LayerBgFirstRecord)->length) return 1;
  if (((LayerRecord *) g_LayerBgSecondRecord)->step <= ((LayerRecord *) g_LayerBgSecondRecord)->length) return 1;
  if (((LayerRecord *) g_LayerFlatRecord)->step <= ((LayerRecord *) g_LayerFlatRecord)->length) return 1;
  if (((LayerRecord *) g_LayerSpriteRecord)->step <= ((LayerRecord *) g_LayerSpriteRecord)->length) return 1;
  return 0;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; what its caller's 1 and 0 were for is not said. */
void func_8001EE60() {

}
/* func_8001EE68 */
void layer_order_set(s32 layer_mask, s32 order)
{
  if (layer_mask & 4)
  {
    ((LayerRecord *)g_LayerBgFirstRecord)->order = order & 0xFF;
  }
  if (layer_mask & 8)
  {
    ((LayerRecord *)g_LayerBgSecondRecord)->order = order & 0xFF;
  }
}
