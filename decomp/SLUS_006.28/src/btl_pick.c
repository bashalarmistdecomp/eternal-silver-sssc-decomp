/* Lunar SSSC — btl_pick: the program's own picks for the acting slot: a
   spell, an item, a target by mode, the nearest slot, a party slot, a
   corner to walk to, an active slot. The first split had it in
   game_state.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlActingSlot g_BtlActionKind g_BtlActionNumber */
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot */
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "chr.h"
#include "btl.h"
s32 rng_mask_below(s32, s32); /* defined K&R below: (s32 mask, u16 n) */
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern u8 g_BtlActingStatus;
extern long HWD0;
extern long VWD0;
extern s32 g_BtlRecords;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern s32 CharStats; /* far_syms: CharStats */
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotIds[];
extern u8 g_BtlActingSlot;
extern u8 g_BtlSlotActiveFlags[];
/* far_syms: CharStats g_BtlTargetSlot */
s32 BtlQuarter0CornerGet(u8 *quarter_counts);
s32 rng_draw();
s32 rng_7f_draw(void);
s32 BtlQuarter2CornerGet(u8 *quarter_counts);
s32 BtlQuarter3CornerGet(u8 *quarter_counts);
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
s32 BtlQuarter0CornerGet(u8 *quarter_counts);
s32 rng_draw();
s32 rng_7f_draw(void);
/* far_syms: CharStats g_BtlTargetSlot g_BtlSlotActiveFlags */
u8 BtlSlotActivePick(u8 skip_acting);
/* far_syms: g_BtlTargetSlot */
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: CharStats g_BtlActingSlot g_BtlTargetSlot */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot */
/* far_syms: g_BtlTargetSlot g_BtlActionNumber g_BtlActingSlot g_BtlRecords CharStats */
u8 BtlSlotActivePick(u8 skip_acting);
/* far_syms: g_BtlActingSlot CharStats g_BtlTargetSlot g_BtlRecords */
/* far_syms: g_BtlActingStatus (target reaches it %hi/%lo) */
/* far_syms: g_BtlActingStatus (auto: target reaches it %hi/%lo — addressing row 3) */
extern long HWD0, VWD0;
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlRecords */
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern void (*g_BtlActingShowFunctions[])(void);
/* far_syms: CharStats g_BtlRecords */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber */

/* far_syms: CharStats g_BtlActingStatus */
extern BtlSpellRow g_BtlSpellRows[];
s32 rng_list_pick(s32 list, s32 count, s32 mask);  /* TU defines this below */

/* func_800393F0 */
u8 BtlActingSpellPick(void)
{
    u8 cand[8];
    u8 out[8];
    u16 mp;
    u8 level;
    u8 n;
    u8 count;
    u8 i;
    s32 id;
    BtlSpellRow *tbl;
    BtlSpellRow *row;
    BtlStats *cp;

    if (g_BtlActingStatus & 0x80) {
        return 0;
    }
    cp = (BtlStats *)(g_BtlActingSlot * 0x3C + CharStats);
    g_BtlTargetSlot = g_BtlActingSlot;
    mp = cp->mp;
    if ((u8)BtlTargetTraitCheck(0, 0x1B, 0, 0) == 1) {
        mp = mp * 2 + 1;
    }
    level = ((BtlStats *)(g_BtlActingSlot * 0x3C + CharStats))->level;
    switch (g_BtlSlotIds[g_BtlActingSlot]) {
    case 1:
    case 11:
        cand[0] = 1;
        cand[1] = 2;
        cand[2] = 3;
        cand[3] = 4;
        n = 4;
        break;
    case 2:
        n = 8;
        cand[0] = 9;
        cand[1] = 10;
        cand[2] = 11;
        cand[3] = 12;
        cand[4] = 0x2B;
        cand[5] = 0x2C;
        cand[6] = 0x2D;
        cand[7] = 0x2E;
        break;
    case 3:
        n = 4;
        cand[0] = 0xD;
        cand[1] = 0xE;
        cand[2] = 0x10;
        cand[3] = 0x12;
        break;
    case 4:
        n = 6;
        cand[0] = 0x15;
        cand[1] = 0x16;
        cand[2] = 0x17;
        cand[3] = 0x19;
        cand[4] = 0x1A;
        cand[5] = 0x1B;
        break;
    case 5:
        n = 4;
        cand[0] = 0x1D;
        cand[1] = 0x1E;
        cand[2] = 0x1F;
        cand[3] = 0x20;
        break;
    case 6:
        n = 5;
        cand[0] = 0x21;
        cand[1] = 0x22;
        cand[2] = 0x23;
        cand[3] = 0x25;
        cand[4] = 0x26;
        break;
    case 7:
        return 0;
    case 8:
        n = 4;
        cand[0] = 0x27;
        cand[1] = 0x28;
        cand[2] = 0x29;
        cand[3] = 0x2A;
        break;
    case 9:
        return 0;
    case 10:
        cand[0] = 0x2F;
        n = 1;
        break;
    }
    count = 0;
    for (i = 0; i < n; i++) {
        tbl = g_BtlSpellRows;
        id = cand[i];
        row = &tbl[id - 1];
        if (level >= row->level && mp >= row->cost) {
            out[count] = id;
            count++;
        }
    }
    if (count != 0) {
        return rng_list_pick((s32)out, count, n);
    }
    return 0;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80011E84_pad);
s32 rng_list_pick(s32 list, s32 count, s32 mask);  /* TU defines this below */

/* func_800396F4 */
s32 BtlActingItemPick(void)
{
    u8 buf[6];
    u8 n;
    u8 i;
    s32 v;

    n = 0;
    i = 0;
    do {
        v = ((BtlStats *)CharStats)[g_BtlActingSlot].items[i];
        switch (v) {
        case 0x9C ... 0x9F:
        case 0xA1 ... 0xA3:
            buf[n] = v;
            n++;
            break;
        }
        i++;
    } while (i < 6);
    if (n != 0) {
        return rng_list_pick(buf, n, 7) & 0xFF;
    }
    return 0xFF;
}
/* far_syms: g_BtlActingSlot */

/* Re-picks a target that no longer fits, by targeting mode (low nibble of
   mode): 0 keeps the acting unit itself (g_BtlActingSlot), 1 re-picks through
   BtlSlotNearestFind(1), 2..7 through BtlPartyTargetPick(mode). Modes 8..15 fall off
   the end: the original returns whatever $v0 holds, which on that path is
   the 0 left by the `slti v0, v1, 8` range test — an explicit `default:
   return 0;` compiles differently (23/33), so the fall-through is kept.
   BtlPartyTargetPick receives the masked mode in $a0 (its call sets no
   argument): that use is what keeps `mode & 0xF` in $a0 and the switch on
   a separate byte copy in $v1. */
/* func_800397B4 */
u8 BtlTargetByModeGet(u8 mode)
{
    mode &= 0xF;
    switch (mode) {
    case 0:
        return g_BtlActingSlot;
    case 1:
        return BtlSlotNearestFind(1);
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        return BtlPartyTargetPick(mode);
    }
}

/* far_syms: g_BtlActingSlot g_BtlRecords */

/* func_80039838 */
u8 BtlSlotNearestFind(s32 side)
{
    s16 best;
    u8 i;
    u8 end;
    u8 res;
    u8 *p;
    s16 cy;
    s16 cx;
    s16 dy;
    s16 dx;

    best = 0xFFF;
    i = 0;
    side &= 0xFF;
    p = (u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords);
    cy = AS(u16, ((BtlRecord *)p)->sprite.y);
    cx = AS(u16, ((BtlRecord *)p)->sprite.x);
    end = 0xE;
    switch (side)
    {
    case 1:
        i = 6;
        break;
    case 2:
        end = 5;
        break;
    }
    for (; i < end; i++)
    {
        if ((g_BtlSlotActiveFlags[i] != 0) && (i != 5) && (i != g_BtlActingSlot))
        {
            dy = AS(u16, ((BtlRecord *) g_BtlRecords)[i].sprite.y);
            dx = AS(u16, ((BtlRecord *) g_BtlRecords)[i].sprite.x);
            dy -= cy;
            if (dy < 0)
            {
                dy *= -1;
            }
            dx -= cx;
            if (dx < 0)
            {
                dx *= -1;
            }
            dy += dx;
            if (dy < best)
            {
                best = dy;
                res = i;
            }
        }
    }
    return res;
}
void BtlActionFallbackSet();

/* func_8003995C */
u8 BtlPartyTargetPick(s32 mode)
{
    u8 buf[5];
    u8 n;
    u8 i;
    u8 ok;

    n = 0;
    mode = (mode & 0xFF) - 2;
    for (i = 0; i < 5; i++)
    {
        if (g_BtlSlotIds[i] != 0)
        {
            ok = 0;
            switch (mode)
            {
            case 0:
                if (g_BtlSlotActiveFlags[i] != 0)
                    ok = 1;
                break;
            case 1:
                if (g_BtlSlotActiveFlags[i] == 0)
                    ok = 1;
                break;
            case 2:
                if (*(u8 *) (CharStats + i * 60 + 0x15) != 0)
                    ok = 1;
                break;
            case 3:
                if (g_BtlSlotActiveFlags[i] != 0)
                {
                    if (*(u16 *) (CharStats + i * 60) < *(u16 *) (CharStats + i * 60 + 2))
                        ok = 1;
                }
                break;
            case 4:
                if (g_BtlSlotActiveFlags[i] != 0)
                {
                    if (*(u16 *) (CharStats + i * 60 + 4) < *(u16 *) (CharStats + i * 60 + 6))
                        ok = 1;
                }
                break;
            case 5:
                if (g_BtlSlotActiveFlags[i] != 0)
                {
                    if (*(u8 *) (CharStats + i * 60 + 0x15) == 0)
                        ok = 1;
                }
                break;
            }
            if (ok)
            {
                buf[n] = i;
                n++;
            }
        }
    }
    if (n != 0)
    {
        return rng_list_pick(buf, n, 7);
    }
    BtlActionFallbackSet();
    return g_BtlTargetSlot;
}

s32 BtlQuarter2CornerGet(u8 *quarter_counts);
s32 BtlQuarter1CornerGet(u8 *quarter_counts);
s32 BtlQuarter3CornerGet(u8 *quarter_counts);

/* func_80039B70 */
void BtlActionWalkCornerSet(u8 mode)
{
  u8 quad[4];
  s32 end;
  s32 i;
  s32 count;
  u8 t;
  u8 half;

  i = 0;
  end = 0xE;
  quad[0] = quad[1] = quad[2] = quad[3] = 0;
  switch (mode)
  {
    case 1:
      end = 5;
      break;
    case 2:
      i = 6;
      break;
  }
  count = 0;
  for (; (u8)i < (u8)end; i++)
  {
    if ((u8)i != g_BtlActingSlot && g_BtlSlotActiveFlags[(u8)i] != 0)
    {
      count++;
      if (((BtlRecord *)g_BtlRecords)[(u8)i].sprite.x < 0xA0)
      {
        if (((BtlRecord *)g_BtlRecords)[(u8)i].sprite.y < 0x88)
          quad[0]++;
        else
          quad[2]++;
      }
      else
      {
        if (((BtlRecord *)g_BtlRecords)[(u8)i].sprite.y < 0x88)
          quad[1]++;
        else
          quad[3]++;
      }
    }
  }
  if ((u8)count == 1)
  {
    if (quad[0] + quad[2] != 0)
    {
      i = 1;
      if (quad[0] != 0)
        i = 3;
    }
    else
    {
      i = (quad[1] != 0) << 1;
    }
  }
  else
  {
    if (((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].sprite.x < 0xA0)
    {
      if (((BtlRecord *)g_BtlRecords)[(u8)i].sprite.y < 0x88)
        i = BtlQuarter0CornerGet(quad);
      else
        i = BtlQuarter2CornerGet(quad);
    }
    else
    {
      if (((BtlRecord *)g_BtlRecords)[(u8)i].sprite.y < 0x88)
        i = BtlQuarter1CornerGet(quad);
      else
        i = BtlQuarter3CornerGet(quad);
    }
  }
  t = (u8)((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].width >> 3;
  end = t;
  if (((BtlRecord *)g_BtlRecords)[g_BtlActingSlot].width & 7)
    end = (u8)(t + 1);
  if (end & 1)
    end++;
  half = (u8)end >> 1;
  switch ((u8)i)
  {
    case 0:
      g_BtlActionNumber = half;
      g_BtlTargetSlot = 0xE;
      break;
    case 1:
      g_BtlActionNumber = 0x28 - half;
      g_BtlTargetSlot = 0xE;
      break;
    case 2:
      g_BtlActionNumber = half;
      g_BtlTargetSlot = 0x16;
      break;
    case 3:
      g_BtlActionNumber = 0x28 - half;
      g_BtlTargetSlot = 0x16;
      break;
  }
  g_BtlActionKind = 5;
}

/* func_80039E84 */
s32 BtlQuarter0CornerGet(u8 *quarter_counts)
{
  u8 corner;
  s32 unused;
  u8 quarter2_count;
  u8 quarter1_count;
  u8 quarter0_count;
  quarter0_count = quarter_counts[0];
  quarter2_count = quarter_counts[2];
  quarter1_count = quarter_counts[1];
  ;
  if ((quarter0_count + quarter2_count) > (quarter1_count + quarter_counts[3]))
  {
    corner = 1;
    if (quarter1_count > quarter_counts[3])
    {
      return 3;
    }
    return corner;
  }
  corner = (quarter2_count < quarter0_count) * 2;
  return corner;
}
/* func_80039EC8 */
s32 BtlQuarter2CornerGet(u8 *quarter_counts)
{
  u8 corner;
  u8 quarter3_count;
  u8 quarter2_count;
  u8 quarter1_count;
  u8 quarter0_count;
  quarter0_count = quarter_counts[0];
  quarter2_count = quarter_counts[2];
  quarter1_count = quarter_counts[1];
  quarter3_count = quarter_counts[3];
  if ((quarter0_count + quarter2_count) > (quarter1_count + quarter3_count))
  {
    if (!(quarter1_count < quarter3_count))
    {
      return 3;
    }
    return 1;
  }
  corner = ((quarter0_count < quarter2_count) ^ 1) << 1;
  return corner;
}
/* func_80039F10 */
s32 BtlQuarter1CornerGet(u8 *quarter_counts)
{
  u8 quarter2_count;
  u8 quarter0_count;
  u8 quarter3_count;
  u8 quarter1_count;
  quarter1_count = quarter_counts[1];
  quarter3_count = quarter_counts[3];
  quarter0_count = quarter_counts[0];
  quarter2_count = quarter_counts[2];
  if ((quarter1_count + quarter3_count) > (quarter0_count + quarter2_count))
  {
    return (quarter2_count < quarter0_count) << 1;
  }
  if (!(quarter3_count < quarter1_count))
  {
    return 1;
  }
  return 3;
}

/* func_80039F54 */
s32 BtlQuarter3CornerGet(u8 *quarter_counts)
{
  u8 quarter2_count;
  u8 quarter0_count;
  u8 quarter3_count;
  u8 quarter1_count;
  quarter1_count = quarter_counts[1];
  quarter3_count = quarter_counts[3];
  quarter0_count = quarter_counts[0];
  quarter2_count = quarter_counts[2];
  if ((quarter1_count + quarter3_count) > (quarter0_count + quarter2_count))
  {
    return ((quarter0_count < quarter2_count) ^ 1) << 1;
  }
  if (quarter1_count < quarter3_count)
  {
    if (quarter_counts)
    {
      return 1;
    }
    else
    {
      return 1;
    }
  }
  return 3;
}
/* func_80039F9C */
u8 BtlSlotActivePick(u8 skip_acting)
{
    u8 candidates[14];
    u8 candidate_count;
    u8 slot;

    candidate_count = 0;
    slot = 0;
    while (slot < 14)
    {
        if (g_BtlSlotActiveFlags[slot] != 0)
        {
            if (slot != 5)
            {
                if (slot != (u8)g_BtlActingSlot || skip_acting == 0)
                {
                    candidates[candidate_count] = slot;
                    candidate_count++;
                }
            }
        }
        slot++;
    }
    return rng_list_pick(candidates, candidate_count, 15);
}

/* func_8003A03C */
s32 rng_list_pick(s32 list, s32 count, s32 mask)
{
  s32 entry;
  unsigned long mask_then_pick;
  s32 kept_count;
  unsigned draw;
  entry = list;
  mask_then_pick = mask;
  kept_count = count;
  draw = rng_draw_copy() & mask_then_pick;
  while ((draw & 0xFF) >= (kept_count & 0xFF))
  {
    draw -= kept_count;
  }
  entry = entry + (draw & 0xFF);
  mask_then_pick = *((u8 *) entry);
  draw = mask_then_pick;
  return draw;
}

s32 rng_draw();                                /* extern */

/* func_8003A0B4 */
s32 rng_7f_draw(void) {
    return rng_draw() & 0x7F;
}
/* The RNG roll: one draw from rng_draw, masked by `mask`, reduced
   into 0..n-1 by repeated subtraction (16-bit compares; the target's
   `subu` in the loop branch's delay slot plus the `addu` after it is
   reorg stealing the loop's first insn, not source). n < 2 returns 0
   without drawing. Spelled K&R: the bytes want the u16 `n` (the
   callee's own `andi 0xFFFF`), while every caller's prototype says
   (s32, s32) — after default promotion the two agree, so no caller's
   argument passing changes. */
/* func_8003A0D4 */
s32 rng_mask_below(mask, n)
    s32 mask;
    u16 n;
{
    u16 r;

    if (n < 2) {
        return 0;
    }
    r = rng_draw() & mask;
    while (r >= n) {
        r -= n;
    }
    return r;
}
