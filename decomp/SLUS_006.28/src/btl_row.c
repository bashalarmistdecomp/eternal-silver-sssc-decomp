/* Lunar SSSC — btl_row: the action's row: BtlActionRowRun loads it and
   calls the function it names, and the first four of those functions (the
   target's mp and hp added). The first split had it in game_misc_b.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
/* far_syms: CharStats (carried from the first split's game_misc.c) */
/* far_syms: g_BtlSlotIds (carried from the first split's game_misc.c) */
extern s32 CharStats;
extern u8 g_BtlActingSlot;
extern u8 g_BtlTargetSlot;
extern s32 g_BtlTargetStats;
extern u16 g_BtlSlotAmounts[];
extern u8 g_BtlSlotIds[];
extern u8 val;
/* far_syms: g_BtlSpellRowsByNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: CharStats */
extern u16 g_BtlActionPower;

/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot g_BtlSlotIds g_BtlRunning g_BtlSlotAmounts g_BtlItemRows */
/* load_hazard_nop: BtlActionRowRun */
extern u8 g_BtlRunning;
extern s32 g_ChrRecords;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlActionFunctionNumber;
extern u8 g_BtlActionShowNumber;
extern s16 g_BtlAreaHalfWidth;
extern s16 g_BtlAreaHalfHeight;
extern s16 g_BtlActionBitNumber;
extern u8 g_BtlAttackRows[];
extern u8 g_BtlSpellRowsByNumber[];
extern s32 g_BtlItemRows;
extern u8 g_BtlItemRowsByNumber[];
extern void (*scene_state_table[])();
void BtlF7Slot7RecordOff();

/* func_80031ABC */
void BtlActionRowRun(void)
{
  u32 i;
  u8 *p;
  u8 cls;
  u8 *tbl;

  if (g_BtlRunning == 0)
  {
    i = 0;
    do
    {
      *(u16 *) (((i & 0xFF) * 2) + (u8 *)g_BtlSlotAmounts) = 0;
      i++;
    }
    while ((i & 0xFF) < 0xE);
    if (g_BtlTargetSlot >= 0xD)
    {
      g_BtlTargetSlot = 1;
    }
    g_BtlTargetStats = g_ChrRecords + (g_BtlTargetSlot << 6);
    g_BtlTargetSlot--;
  }
  else
  {
    g_BtlTargetStats = CharStats + (g_BtlTargetSlot * 0x3C);
  }
  switch (g_BtlActionKind)
  {
    case 2:
      g_BtlActionNumber = *(u8 *) ((g_BtlActingSlot * 0x3C) + CharStats + 0x1C);
    case 7:
      if ((g_BtlActionKind == 7) && (g_BtlSlotIds[g_BtlActingSlot] == 0x4F))
      {
        cls = *(u8 *) ((g_BtlActingSlot * 0x3C) + CharStats + 0x1D);
      }
      else
      {
        cls = g_BtlActionNumber;
      }
      tbl = g_BtlAttackRows;
      p = tbl + (cls * 8);
      g_BtlActionFunctionNumber = p[1];
      g_BtlActionPower = p[2];
      g_BtlAreaHalfWidth = p[3];
      g_BtlAreaHalfHeight = p[4];
      g_BtlActionBitNumber = p[5];
      g_BtlActionShowNumber = p[6];
      break;
    case 3:
      p = g_BtlSpellRowsByNumber + (g_BtlActionNumber * 0xC);
      g_BtlActionFunctionNumber = p[2];
      g_BtlActionPower = p[5];
      g_BtlAreaHalfWidth = p[9];
      g_BtlAreaHalfHeight = p[0xA];
      g_BtlActionShowNumber = p[6];
      g_BtlActionBitNumber = p[8];
      break;
    case 4:
      if (g_BtlActionNumber < 0x9C)
      {
        p = (u8 *) g_BtlItemRows + ((g_BtlActionNumber * 0x12) - 0x12);
        g_BtlActionFunctionNumber = p[8];
      }
      else
      {
        p = g_BtlItemRowsByNumber + (g_BtlActionNumber * 0x12);
        g_BtlActionFunctionNumber = p[8];
        g_BtlActionPower = p[0xA];
      }
      g_BtlActionShowNumber = p[9];
      break;
  }
  if (g_BtlActionFunctionNumber < 0x5F)
  {
    scene_state_table[g_BtlActionFunctionNumber - 1]();
  }
  BtlF7Slot7RecordOff();
}
/* Effect 1 (scene_state_table[0]): the fixed MP restore of the MP items
   (battle.md "Item use and the menu-time auto-battle"). The target record g_BtlTargetStats's
   +0x04 (MP) goes up by the magnitude g_BtlActionPower plus rng_mask_below(7, 8)
   (one RNG draw, 0..7) and is clamped to +0x06 (max MP). g_BtlActionPower first
   takes magnitude + roll and, when the sum reaches the maximum, the amount
   that actually landed; that amount goes to the target's pending slot
   g_BtlSlotAmounts[g_BtlTargetSlot], and BtlTargetAddShowRun sets the reaction and plays the
   presentation. No frame wait of its own.

   It is BtlTargetHpAddRun (effect 4, the HP items) on +0x04 / +0x06 instead
   of +0x00 / +0x02, and takes the same spelling: the maximum read once
   into a u16 for the test and the subtraction, and read again for the
   clamp store (storing the local instead is 22/33). */
/* func_80031E00 */
void BtlTargetMpAddRun(void)
{
  u16 *rec;
  u16 max;
  s32 gain;
  unsigned mp;

  gain = g_BtlActionPower + rng_mask_below(7, 8);
  rec = (u16 *) g_BtlTargetStats;
  mp = rec[2] + gain;
  g_BtlActionPower = gain;
  rec[2] = mp;
  max = rec[3];
  if ((mp & 0xFFFF) >= max)
  {
    g_BtlActionPower = gain - (mp - max);
    rec[2] = rec[3];
  }
  g_BtlSlotAmounts[g_BtlTargetSlot] = g_BtlActionPower;
  BtlTargetAddShowRun();
}

/* far_syms: g_BtlSlotAmounts (target reaches it %hi/%lo) */

/* func_80031E84 */
void BtlTargetMpFillRun(void) {
    u8 *ptr;
    u16 a;
    u16 b;

    ptr = (u8 *)g_BtlTargetStats;
    a = ((BtlStats *)ptr)->mp_max;
    b = ((BtlStats *)ptr)->mp;
    *(u16 *)((u8 *)g_BtlSlotAmounts + g_BtlTargetSlot * 2) = a - b;
    ((BtlStats *)ptr)->mp = ((BtlStats *)ptr)->mp_max;
    BtlTargetAddShowRun();
}
/* far_syms: g_BtlTargetSlot g_BtlSlotAmounts */

/* Effect 3 (scene_state_table[2]): the heal of one target by a share of
   its max HP (battle.md "The heal formula"). On the record g_BtlTargetStats
   points at (BtlStats: hp at +0x00, max HP at +0x02, in both record
   layouts):

     amount = max HP * 4 / 10
     roll   = rng_mask_below(0xFFFF, max HP / 20)   0 .. max HP / 20 - 1, one
                                                   RNG draw; none (and 0)
                                                   when max HP is under 40
     amount = (amount < 40 ? 40 : amount) + roll   (16 bits)
     hp += amount; when hp >= max HP the overshoot comes off the amount
                   and hp = max HP

   g_BtlActionPower holds the amount throughout (the 40%, then with the floor
   and the roll, then what actually landed) and g_BtlAreaHalfWidth the roll. The
   amount that landed goes to the target's pending slot
   g_BtlSlotAmounts[g_BtlTargetSlot], and BtlTargetAddShowRun sets the reaction and plays
   the presentation. No frame wait of its own.

   It is the body of BtlPartyHpShareAddRun's loop (effect 7, btl_target.c) for
   the one target, and takes that function's spelling:
   - BtlStats members, not offset casts. The store to hp must be a struct
     member's: after a store through a plain `u16 *` the amount in
     g_BtlActionPower is read from memory again, and the target keeps it in $a2.
   - The second max HP read goes through a signed local: as
     `(u16)(rec->maxhp / 20)` the front end narrows the division to an
     unsigned one (multu 0xCCCCCCCD), and the target has the signed one
     (mult 0x66666667, sra 3, sign fix).
   - `hp = maxhp` re-reads max HP after the store to g_BtlActionPower: the
     target's `addu v1,a1,zero` is that read, turned into a copy of the
     register that still holds the value after reload. */
/* func_80031ED4 */
void BtlTargetHpShareAddRun(void)
{
  s32 t;

  g_BtlActionPower = ((BtlStats *) g_BtlTargetStats)->hp_max * 4 / 10;
  t = ((BtlStats *) g_BtlTargetStats)->hp_max;
  g_BtlAreaHalfWidth = rng_mask_below(0xFFFF, (u16) (t / 20));
  if (g_BtlActionPower < 40)
  {
    g_BtlActionPower = g_BtlAreaHalfWidth + 40;
  }
  else
  {
    g_BtlActionPower += g_BtlAreaHalfWidth;
  }
  ((BtlStats *) g_BtlTargetStats)->hp += g_BtlActionPower;
  if (((BtlStats *) g_BtlTargetStats)->hp >= ((BtlStats *) g_BtlTargetStats)->hp_max)
  {
    g_BtlActionPower -= ((BtlStats *) g_BtlTargetStats)->hp - ((BtlStats *) g_BtlTargetStats)->hp_max;
    ((BtlStats *) g_BtlTargetStats)->hp = ((BtlStats *) g_BtlTargetStats)->hp_max;
  }
  g_BtlSlotAmounts[g_BtlTargetSlot] = g_BtlActionPower;
  BtlTargetAddShowRun();
}

/* func_80031FB4 */
void BtlTargetHpAddRun(void)
{
  u16 *ptr;
  u16 max_hp;
  s32 val;
  unsigned sum;
  val = g_BtlActionPower + rng_mask_below(7, 8);
  ptr = (u16 *) g_BtlTargetStats;
  sum = ptr[0] + val;
  g_BtlActionPower = val;
  ptr[0] = sum;
  max_hp = ptr[1];
  if ((sum & 0xFFFF) >= max_hp)
  {
    g_BtlActionPower = val - (sum - max_hp);
    ptr[0] = ptr[1];
  }
  *((u16 *) ((u8 *)g_BtlSlotAmounts + (g_BtlTargetSlot * 2))) = g_BtlActionPower;
  BtlTargetAddShowRun();
}
