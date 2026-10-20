/* Lunar SSSC — btl_fall: the falls and the end: BtlFallRun, the revive by
   item 0x83 with the worn item's removal and stats, and the end check
   (BtlEndCheck, BtlSpecialCheck). The first split had it in game_state.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlRecords */
/* far_syms: CharStats */
/* far_syms: g_BtlActingSlot */
/* far_syms: g_BtlActingSlot g_BtlRecords */
/* far_syms: CharStats g_BtlTargetSlot */
/* far_syms: g_BtlActingSlot g_BtlActionKind g_BtlActionNumber g_BtlNumber */
/* far_syms: g_BtlActingSlot g_BtlRecords g_BtlTargetSlot */
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "chr.h"
#include "btl.h"
s32 SoundPlay(s32, s32);
extern u8 g_BtlPartyNotHeldFlags[];
extern u8 g_BtlPartyStandAnimations[];
extern u8 g_BtlSpecialBytes[];
extern long HWD0;
extern long VWD0;
extern s32 g_BtlRecords;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlNumber;
extern u8 g_BtlSlotFallMarks[];
extern s32 CharStats; 
extern u8 g_BtlActionShowNumber;
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotIds[];
extern u8 g_BtlActingSlot;
extern u8 g_BtlSlotActiveFlags[];
void BtlFallRun();
s32 BtlSpecialCheck();
s32 BtlFrameStep();
s32 rng_7f_draw(void);
void BtlTargetReviveRun(void);
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
void BtlFallRun();
s32 BtlSpecialCheck();
s32 BtlFrameStep();
s32 rng_7f_draw(void);
/* far_syms: CharStats g_BtlTargetSlot g_BtlSlotActiveFlags */
int BtlEndCheck(void);
/* far_syms: g_BtlTargetSlot */
void BtlFallRun();
void BtlFallRun();
s32 BtlAnimationSet();
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: CharStats g_BtlActingSlot g_BtlTargetSlot */
s32 BtlSpecialCheck();
/* far_syms: g_BtlSlotFallMarks (target reaches it %hi/%lo) */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot */
/* far_syms: g_BtlTargetSlot g_BtlActionNumber g_BtlActingSlot g_BtlRecords CharStats */
/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */
int BtlEndCheck(void);
/* far_syms: g_BtlActingSlot CharStats g_BtlTargetSlot g_BtlRecords */
extern long HWD0, VWD0;
/* far_syms: g_BtlPartyFallenFlags (target reaches it %hi/%lo) */
extern u8 g_BtlPartyFallenFlags;
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
s32 BtlFrameStep();
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern void (*g_BtlActingShowFunctions[])(void);
/* far_syms: g_BtlPartyStandAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSpecialBytes (target reaches it %hi/%lo) */

/* far_syms: g_BtlSlotStrongMarks (target reaches it %hi/%lo) */
extern u8 g_BtlSlotStrongMarks[];

/* The critical-hit roll of a physical swing (mode g_BtlActionKind == 2),
   attacker g_BtlActingSlot on target g_BtlTargetSlot; BtlHitAmountSet calls it once per
   swing and multiplies the attack term by 1.3 (ratio_13_10_mul) when it
   returns 1. Never between two party slots (both < 5) or two enemy slots
   (both >= 6); slot 5 is eligible against either side.

   First roll, rng_7f_draw() (0..127) below a base rate: 5 for a slot >= 5,
   10 for a party slot, 50 when that slot's g_BtlSlotIds id is 2 or 3. When it
   fails, the attacker's CharStats byte +0x28 (party) or +0x1B (enemy)
   decides: 21 is always a critical, 14 and 7 draw a SECOND roll below 0x40
   and 0x20, anything else is none. So one RNG draw per eligible swing, two
   when the first fails on a 14 or a 7, none when the function returns
   before the first roll. A critical sets g_BtlSlotStrongMarks[target] = 1; nothing
   here clears it.

   n holds the base rate, then the +0x28 / +0x1B byte, then the second
   roll. The last is read off the bytes, not chosen: the target masks n
   (andi 0xFF) at the first compare, which this compiler keeps only when
   some assignment to the variable has unknown upper bits, and the only
   such value in the function is a rng_7f_draw result. A second roll
   compared without the variable drops that mask (114/124). */
/* func_80038310 */
s32 BtlHitStrongTry(void)
{
    u8 n;
    u8 rate;

    if (g_BtlActionKind == 2) {
        if (g_BtlActingSlot >= 5 || g_BtlTargetSlot >= 5) {
            if (g_BtlActingSlot >= 6 && g_BtlTargetSlot >= 6) {
                return 0;
            }
            if (g_BtlActingSlot < 5) {
                n = 10;
                if (g_BtlSlotIds[g_BtlActingSlot] == 2 || g_BtlSlotIds[g_BtlActingSlot] == 3) {
                    n = 50;
                }
            } else {
                n = 5;
            }
            if ((u8)rng_7f_draw() < n) {
                g_BtlSlotStrongMarks[g_BtlTargetSlot] = 1;
                return 1;
            }
            if (g_BtlActingSlot < 5) {
                n = *(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x28);
            } else {
                n = *(u8 *)(g_BtlActingSlot * 60 + CharStats + 0x1B);
            }
            switch (n) {
            case 7:
                rate = 0x20;
                break;
            case 14:
                rate = 0x40;
                break;
            case 21:
                g_BtlSlotStrongMarks[g_BtlTargetSlot] = 1;
                return 1;
            default:
                return 0;
            }
            if (rate != 0) {
                n = rng_7f_draw();
                if (n < rate) {
                    g_BtlSlotStrongMarks[g_BtlTargetSlot] = 1;
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_80038500 */
s32 BtlSpecialCheck() {
    return g_BtlNumber >= 0xEB;
}

/* func_80038518 */
int BtlEndCheck(void)
{
  u8 i;
  short party_active_count;
  u8 first_party_slot;
  u8 j;
  i = 6;
  j = 0;
  do
  {
    if (g_BtlSlotActiveFlags[i])
    {
      j++;
    }
    i++;
  }
  while (i < 0xE);
  if (j == 0)
  {
    return 1;
  }
  j = 0;
  i = (first_party_slot = j);
  do
  {
    if (g_BtlSlotActiveFlags[i])
    {
      j++;
    }
    i++;
  }
  while (i < 5);
  party_active_count = party_active_count < 1;
  party_active_count = j;
  return ((party_active_count) ? (0) : (2));
}
/* far_syms: g_BtlPartyFallenFlags CharStats g_BtlPartyKeptRecords g_BtlRecords */
extern u8 g_BtlFallDone;
extern s32 g_BtlPartyKeptRecords;
extern u8 g_BtlPartyHitAnimations[];
void BtlPartyPanelDisp();
void BtlEnemyFallShow();

/* func_800385B4 */
void BtlFallRun(void)
{
    u8 i;
    u8 *rec;
    BtlStats *cr;
    u16 x;
    u16 y;
    u16 attr;

    if (g_BtlFallDone == 0)
    {
        g_BtlFallDone++;
        for (i = 0; i < 5; i++)
        {
            if (g_BtlSlotFallMarks[i] != 0)
            {
                (&g_BtlPartyFallenFlags)[i] = 1;
                cr = (BtlStats *)(i * 0x3C + CharStats);
                cr->hp = 0;
                g_BtlSlotActiveFlags[i] = 0;
                cr->side.party.flags = 0;
                cr->status = 0;
                if (g_BtlPartyNotHeldFlags[i] == 0)
                {
                    x = *(u16 *)(((i << 6) + g_BtlRecords) + 6);
                    y = *(u16 *)(((i << 6) + g_BtlRecords) + 0xA);
                    attr = *(u16 *)(((i << 6) + g_BtlRecords) + 0xC);
                    *(BtlRecord *)((i << 6) + g_BtlRecords) = *(BtlRecord *)((i << 6) + g_BtlPartyKeptRecords);
                    *(u16 *)(((i << 6) + g_BtlRecords) + 6) = x;
                    *(u16 *)(((i << 6) + g_BtlRecords) + 0xA) = y;
                    *(u16 *)(((i << 6) + g_BtlRecords) + 8) = y;
                    *(u16 *)(((i << 6) + g_BtlRecords) + 0xC) = attr;
                    g_BtlPartyStandAnimations[i] = g_BtlSpecialBytes[i + 5];
                    g_BtlPartyHitAnimations[i] = 0x12;
                    g_BtlPartyNotHeldFlags[i] = 1;
                    g_BtlSpecialBytes[i] = 0;
                }
                BtlAnimationSet(g_BtlRecords + (i << 6), 0x1C);
            }
        }
        BtlPartyPanelDisp();
        /* the original binary keeps this counter loop over the enemy slots
           6..13 with nothing in its body (0x8003875C..0x8003876C) */
        for (i = 6; i < 0xE; i++)
        {
        }
        for (i = 6; i < 0xE; i++)
        {
            if (g_BtlSlotFallMarks[i] != 0)
            {
                break;
            }
        }
        if (i < 0xE)
        {
            if ((g_BtlSlotIds[i] != 0x70) && (g_BtlSlotIds[i] != 0x76))
            {
                SoundPlay(0x28, 3);
            }
            BtlEnemyFallShow();
            for (i = 6; i < 0xE; i++)
            {
                if (g_BtlSlotFallMarks[i] != 0)
                {
                    rec = (u8 *)(i * 0x3C + CharStats);
                    *(u16 *)rec = 0;
                    g_BtlSlotActiveFlags[i] = 0;
                    rec[0x15] = 0;
                }
            }
        }
    }
}
/* far_syms: g_BtlPartyFallenFlags CharStats g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber g_BtlNumber */
void BtlWornItemRemove(u8 who, u8 slot);  /* TU defines this below */

/* The auto-revive pass. Its three callers (BtlRoundStatusUpdate, BtlActionKindRun,
   BtlEnemy7AActionSet) run it right after the KO clean-up BtlFallRun.

   Nothing happens when the encounter id g_BtlNumber is 0xF9, or when it is
   0xFE and g_BtlSpecialBytes[0] is 5 or more.

   Otherwise g_BtlActionShowNumber (the spell id the presentation is picked by) = 0x34,
   and for every party slot 0..4 that BtlFallRun flagged as fallen
   (g_BtlPartyFallenFlags[i] != 0) and that carries item 0x83 in byte +0x26 or +0x27
   of its 0x3C stat record (CharStats):
     - one battle frame (BtlFrameStep);
     - the target g_BtlTargetSlot = the slot, the action kind g_BtlActionKind = 4
       (an item) and the action id g_BtlActionNumber = 0x83;
     - BtlTargetReviveRun(), the revive;
     - the item is taken off: BtlWornItemRemove(slot, 4) when +0x26 holds it,
       (slot, 5) otherwise. The byte is read again for this, after the
       revive.
   If at least one slot was revived, BtlActionStringClose() once at the end.
   g_BtlPartyFallenFlags is not cleared here. No RNG draw of its own; its frames are
   the one per revived slot and the callees'.

   Shape: the table's address is taken into `fell` at the top of the loop
   body. g_BtlPartyFallenFlags is small data, so its address is the assembler's
   one-instruction `la`, and loop.c moves a one-instruction constant out of
   this loop (42 insns, with calls) only when its register lives two insns.
   Taken first, the order is la, the index's zero-extension, the add, and
   the target has the la before the loop (lui $s5 / addiu $s5). Indexed
   directly, (&g_BtlPartyFallenFlags)[i], the zero-extension comes first, the la
   lives one insn and stays in the loop (77/87). */
/* func_80038868 */
void BtlItem83Revive(void)
{
    u8 i;
    u8 count;
    s32 worn_index;
    u8 *fell;

    if (g_BtlNumber == 0xF9)
    {
        return;
    }
    if (g_BtlNumber == 0xFE)
    {
        if (g_BtlSpecialBytes[0] >= 5)
        {
            return;
        }
    }
    g_BtlActionShowNumber = 0x34;
    count = 0;
    for (i = 0; i < 5; i++)
    {
        fell = &g_BtlPartyFallenFlags;
        if (fell[i] != 0)
        {
            if (*(u8 *)(CharStats + i * 60 + 0x26) == 0x83 || *(u8 *)(CharStats + i * 60 + 0x27) == 0x83)
            {
                BtlFrameStep();
                count++;
                g_BtlTargetSlot = i;
                g_BtlActionKind = 4;
                g_BtlActionNumber = 0x83;
                BtlTargetReviveRun();
                worn_index = 5;
                if (*(u8 *)(CharStats + i * 60 + 0x26) == 0x83)
                {
                    worn_index = 4;
                }
                BtlWornItemRemove(i, worn_index);
            }
        }
    }
    if (count != 0)
    {
        BtlActionStringClose();
    }
}

/* func_800389C4 */
void BtlWornItemStatLower(void *chr_record, s32 slot, s32 stat_kind, s32 bonus)
{
  s32 stat_kind_byte;
  s32 bonus_byte;
  void *stats08;
  void *stats0a;
  void *stats0c;
  void *stats0e;
  void *stats10;
  void *stats12;
  void *stats1f;
  void *stats13;
  stat_kind_byte = stat_kind & 0xFF;
  bonus_byte = bonus & 0xFF;
  switch (stat_kind_byte)
  {
    case 1:
      ((ChrRecord *)chr_record)->stat_08 = (u16) (AS(s32, ((ChrRecord *)chr_record)->stat_08) - bonus_byte);
      stats08 = ((slot & 0xFF) * 0x3C) + CharStats;
      ((BtlStats *)stats08)->stat_08 = (u16) (AS(s32, ((BtlStats *)stats08)->stat_08) - bonus_byte);
      return;
    case 2:
      ((ChrRecord *)chr_record)->stat_0a = (u16) (AS(s16, ((ChrRecord *)chr_record)->stat_0a) - bonus_byte);
      stats0a = ((slot & 0xFF) * 0x3C) + CharStats;
      ((BtlStats *)stats0a)->stat_0a = (u16) (AS(s16, ((BtlStats *)stats0a)->stat_0a) - bonus_byte);
      return;
    case 3:
      ((ChrRecord *)chr_record)->stat_0c = (u16) (AS(s16, ((ChrRecord *)chr_record)->stat_0c) - bonus_byte);
      stats0c = ((slot & 0xFF) * 0x3C) + CharStats;
      ((BtlStats *)stats0c)->stat_0c = (u16) (AS(s16, ((BtlStats *)stats0c)->stat_0c) - bonus_byte);
      return;
    case 4:
      ((ChrRecord *)chr_record)->stat_0e = (u16) (AS(s16, ((ChrRecord *)chr_record)->stat_0e) - bonus_byte);
      stats0e = ((slot & 0xFF) * 0x3C) + CharStats;
      ((BtlStats *)stats0e)->stat_0e = (u16) (AS(s16, ((BtlStats *)stats0e)->stat_0e) - bonus_byte);
      return;
    case 5:
      ((ChrRecord *)chr_record)->stat_10 = (u16) (AS(s16, ((ChrRecord *)chr_record)->stat_10) - bonus_byte);
      stats10 = ((slot & 0xFF) * 0x3C) + CharStats;
      ((BtlStats *)stats10)->stat_10 = (u16) (AS(s16, ((BtlStats *)stats10)->stat_10) - bonus_byte);
      return;
    case 6:
      ((ChrRecord *)chr_record)->attack_count = (u8) (AS(s16, ((ChrRecord *)chr_record)->attack_count) - bonus);
      stats12 = ((slot & 0xFF) * 0x3C) + CharStats;
      *(((u8 *) stats12) + 0x12) = (u8) (AS(s16, ((BtlStats *)stats12)->attack_count) - bonus);
      return;
    case 7:
      ((ChrRecord *)chr_record)->stat_1d = (u8) (((ChrRecord *)chr_record)->stat_1d - bonus);
      stats1f = ((slot & 0xFF) * 0x3C) + CharStats;
      ((BtlStats *)stats1f)->side.party.stat_1f = (u8) (((BtlStats *)stats1f)->side.party.stat_1f - bonus);
      return;
    case 8:
      ((ChrRecord *)chr_record)->walk_reach = (u8) (((ChrRecord *)chr_record)->walk_reach - bonus);
      stats13 = ((slot & 0xFF) * 0x3C) + CharStats;
      ((BtlStats *)stats13)->walk_reach = (u8) (((BtlStats *)stats13)->walk_reach - bonus);
    default:
      return;
  }
}
/* far_syms: CharStats g_ChrRecords g_BtlItemRows */
extern ChrRecord *g_ChrRecords;
extern BtlItemRow *g_BtlItemRows;

/* Unequip: take back what the item in CharStats[who].equip[slot] gave.
   The item record (18 bytes, ids 1-based) names up to two stats as the
   nibbles of +0x0C and their bonuses at +0x0A/+0x0B; BtlWornItemStatLower
   subtracts each from both the party record (g_ChrRecords, indexed by
   g_BtlSlotIds[who]) and the battle record.  Then the party record's
   equip byte (+0x22 for slot 4, else +0x23) and the slot are cleared.
   The nibbles are read as `% 16` / `/ 16`: written `& 0xF` the first
   read stays a QImode load and reorg can no longer see the second read
   as redundant on the skip path (the target branches past it). */
/* func_80038BFC */
void BtlWornItemRemove(u8 who, u8 slot)
{
    ChrRecord *party;
    BtlItemRow *item;

    party = g_ChrRecords;
    party += g_BtlSlotIds[who];
    item = &g_BtlItemRows[((BtlStats *)CharStats)[who].side.party.worn_items[slot] - 1];
    if (item->bonus_stats % 16) {
        BtlWornItemStatLower(party, who, item->bonus_stats % 16, item->bonus_1_power.bonus_1);
    }
    if (item->bonus_stats / 16) {
        BtlWornItemStatLower(party, who, item->bonus_stats / 16, item->bonus_2);
    }
    if (slot == 4) {
        party->worn_items[4] = 0;
    } else {
        party->worn_items[5] = 0;
    }
    ((BtlStats *)CharStats)[who].side.party.worn_items[slot] = 0;
}
