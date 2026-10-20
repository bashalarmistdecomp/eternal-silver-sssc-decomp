/* Lunar SSSC — btl_target: the functions an action's row names (the table
   at 0x800B2EF8): hp filled, added, drained and taken, statuses cleared and
   tried, revives, raises, removals and falls on the target, the marked
   slots, the area, the rows and the other side; and what they share: the
   marks (rows, front, area, party, other side), the status take check, the
   amounts and the outcome cleared. The first split had it in game_state.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlRecords */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlSpecialBytes (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlPartyFallenFlags CharStats g_BtlRecords */
/* far_syms: g_BtlPartyFallenFlags CharStats g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber g_BtlNumber */
/* far_syms: CharStats */
/* far_syms: CharStats g_BtlActingStatus */
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
extern u8 g_BtlRunning;
extern s32 g_LoadBuffer;
s32 BtlAnimationEndWait();
s32 SoundPlay(s32, s32);
s32 rng_mask_below(s32, s32); /* defined K&R below: (s32 mask, u16 n) */
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern u16 g_BtlActionBitNumber;
extern u16 g_BtlActionPower;
extern u8 g_BtlPartyNotHeldFlags[];
extern u8 g_BtlActingStatus;
extern u8 g_BtlSlotRestAnimations[];
extern u8 g_BtlSpecialBytes[];
extern long HWD0;
extern long VWD0;
void BtlTargetAddShowRun(void);
extern u16 g_BtlAreaHalfHeight;
extern s32 g_BtlRecords;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlNumber;
extern u8 g_BtlSlotFallMarks[];
extern s32 CharStats; 
void BtlTargetStat0EHitStatusRun(s32 status);
extern u8 g_BtlActionShowNumber;
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotAmountKinds[];
extern u16 g_BtlAreaHalfWidth;
extern u8 g_BtlSlotIds[];
extern BtlWalkBlock g_BtlWalkBlock;
extern u8 g_BtlActingSlot;
extern u8 g_BtlSlotActiveFlags[];
/* far_syms: g_BtlSlotAmounts (target reaches it %hi/%lo) */
extern s32 g_BtlTargetStats;
extern u8 g_BtlSlotAmounts[];

/* func_80032038 */
void BtlTargetHpFillRun(void) {
    u16 max_hp_then_amount;
    u16 old_hp;
    max_hp_then_amount = ((BtlStats *)g_BtlTargetStats)->hp_max;
    old_hp = *(u16 *)(u8 *)g_BtlTargetStats;
    *(u16 *)(u8 *)g_BtlTargetStats = max_hp_then_amount;
    max_hp_then_amount = max_hp_then_amount - old_hp;
    *(u16 *)(g_BtlSlotAmounts + g_BtlTargetSlot * 2) = max_hp_then_amount;
    BtlTargetAddShowRun();
}

void BtlTargetShowPlaceSet();                                  /* extern */
s32 BtlShowNumberRun();                                /* extern */

/* func_80032084 */
void BtlTargetAddShowRun(void) {
    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 5;
    BtlTargetShowPlaceSet();
    BtlShowNumberRun(g_BtlActionShowNumber);
}
/* far_syms: CharStats g_BtlTargetSlot g_BtlRunning */
typedef struct {
    u16 v[0xE];
} HealAmounts;
extern s32 g_ChrRecords_s;
int BtlTargetPartyMark(void);  /* TU defines this below */

/* Effect 7 (scene_state_table[6]): the heal over BtlTargetPartyMark's set.
   BtlTargetPartyMark flags the slots (g_BtlSlotAmountKinds[i] = 1): in battle the active
   ones of the caster's side, 0..4, or 6..13 when the caster's status has
   0x40; outside battle (g_BtlRunning == 0) the party positions 0..4 that hold
   a member. Then, for each flagged slot i in slot order:

     base   = max HP of the record g_BtlTargetStats points at NOW
     amount = base * 4 / 10
     roll   = rng_mask_below(0xFFFF, base / 20)   one draw, none when
                                                 base / 20 < 2
     amount = (amount < 40 ? 40 : amount) + roll (16 bits)
     g_BtlTargetStats = slot i's record: g_ChrRecords_s + (g_BtlSlotIds[i] << 6)
                  outside battle (the 0x40-byte party records); in battle
                  BtlTargetShowPlaceSet() first (the slot's position, for the number
                  shown), then CharStats + i * 0x3C (the 0x3C-byte records)
     hp += amount; when hp >= maxhp the overshoot comes off the amount and
                  hp = maxhp
     g_BtlSlotAmountKinds[i] = 5 (the heal reaction), g_BtlSlotAmounts[i] = amount landed

   and one BtlShowNumberRun(g_BtlActionShowNumber), the presentation, after the loop.

   NOTE the order: `base` is read BEFORE g_BtlTargetStats is moved to slot i. The
   dispatcher (BtlActionRowRun) leaves it on the action's target, so the first
   flagged slot is healed by 40% of the TARGET's max HP and every later one
   by 40% of the max HP of the slot healed just before it; only the clamp
   uses the slot's own. No status or KO test here: the set is the whole
   filter. g_BtlActionPower / g_BtlAreaHalfWidth are left holding the last slot's amount
   and roll.

   g_ChrRecords_s is the gp-scalar alias of the party records pointer at
   0x800B8BE4: the target reads that word %gp_rel here, while this TU's
   BtlWornItemRemove reaches it %hi/%lo under its own name and the far_syms
   marker that gives it that is TU-wide. One address, two names, one per
   addressing. BtlStats is the two halfwords a heal touches, hp at +0 and
   maxhp at +2 in both record layouts.

   Shape notes. The second read of maxhp goes through a signed local: as
   `(u16)(rec->maxhp / 20)` the front end narrows the division to an
   unsigned one (multu 0xCCCCCCCD), and the target has the signed one (mult
   0x66666667, sra 3, sign fix). `hp = maxhp` re-reads both fields: the
   target's `addu v1,a1,zero` is reload turning that second maxhp read into
   a copy. */
/* func_800320C8 */
void BtlPartyHpShareAddRun(void)
{
    s32 t;

    BtlTargetPartyMark();
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 0xE; g_BtlTargetSlot++) {
        if (g_BtlSlotAmountKinds[g_BtlTargetSlot] != 0) {
            g_BtlActionPower = (((BtlStats *)g_BtlTargetStats)->hp_max * 4) / 10;
            t = ((BtlStats *)g_BtlTargetStats)->hp_max;
            g_BtlAreaHalfWidth = rng_mask_below(0xFFFF, (u16)(t / 20));
            if (g_BtlActionPower < 0x28) {
                g_BtlActionPower = g_BtlAreaHalfWidth + 0x28;
            } else {
                g_BtlActionPower = g_BtlActionPower + g_BtlAreaHalfWidth;
            }
            if (g_BtlRunning == 0) {
                g_BtlTargetStats = g_ChrRecords_s + (g_BtlSlotIds[g_BtlTargetSlot] << 6);
            } else {
                BtlTargetShowPlaceSet();
                g_BtlTargetStats = CharStats + g_BtlTargetSlot * 0x3C;
            }
            ((BtlStats *)g_BtlTargetStats)->hp += g_BtlActionPower;
            if (((BtlStats *)g_BtlTargetStats)->hp >= ((BtlStats *)g_BtlTargetStats)->hp_max) {
                g_BtlActionPower -= ((BtlStats *)g_BtlTargetStats)->hp - ((BtlStats *)g_BtlTargetStats)->hp_max;
                ((BtlStats *)g_BtlTargetStats)->hp = ((BtlStats *)g_BtlTargetStats)->hp_max;
            }
            g_BtlSlotAmountKinds[g_BtlTargetSlot] = 5;
            /* The table as an array OBJECT, not a cast pointer: with
               `extern u16 g_BtlSlotAmounts[]` the plain `g_BtlSlotAmounts[i] = amount`
               matches as it stands (124/124 standalone), and so does this
               member spelling, which says the same under the TU's `u8[]`
               declaration. `((u16 *)g_BtlSlotAmounts)[i]` and
               `*(u16 *)(g_BtlSlotAmounts + i * 2)` do not (108/124): they load the
               index before the table's address, the address register then
               lives one insn less in the loop and loop.c leaves its `la`
               inside (14 * 2 * 3 < 93 insns; the target hoists it). */
            ((HealAmounts *)g_BtlSlotAmounts)->v[g_BtlTargetSlot] = g_BtlActionPower;
        }
    }
    BtlShowNumberRun(g_BtlActionShowNumber);
}

/* func_800322B8 */
void BtlPartyFlag1SetRun(void)
{
  u8 *active_flag;
  u8 next_target;

  g_BtlTargetSlot = 0;
  do
  {
    active_flag = g_BtlTargetSlot + g_BtlSlotActiveFlags;
    if ((*active_flag) != 0)
    {
      *(u8 *)(CharStats + g_BtlTargetSlot * 60 + 0x1E) |= 0x1;
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
    }
    next_target = g_BtlTargetSlot + 1;
    g_BtlTargetSlot = next_target;
  } while (((u32) (next_target & 0xFF)) < 5);
  BtlShowNumberRun(g_BtlActionShowNumber);
}

/* func_80032378 */
void BtlTargetStatus1ClearRun(void) {
    BtlTargetStatusClearRun(1);
}
/* func_80032398 */
void BtlTargetStatus4ClearRun(void) {
    BtlTargetStatusClearRun(4);
}
/* func_800323B8 */
void BtlTargetStatusAllClearRun(void)
{
  *(g_BtlTargetSlot + g_BtlSlotActiveFlags) = 1;
  if ((u8)g_BtlTargetSlot < 5U)
  {
    *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1E)) =
      *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1E)) & 0xFB;
  }
  BtlTargetStatusClearRun(0xFF);
}

s32 BtlHitTry();
s32 BtlHitAmountSet();
s32 BtlHitSlotsUpdate();
s32 BtlTargetReachCheck();
void BtlTargetHitRun(void);  /* TU defines this below */
void BtlAreaHitRun();
void BtlTargetAreaMark(u8);
void BtlActingAreaHitRun(void);  /* TU defines this below */
void BtlTargetAreaHitRun(void);  /* TU defines this below */
s32 BtlMarkedHitRun(void);  /* TU defines this below */
s32 BtlTargetOtherSideMark();
void BtlOtherSideHitRun(void);  /* TU defines this below */
s32 BtlTargetStatus8Clear();
s32 BtlMarkedRestAnimationSet();
s32 BtlStat0EHitAmountSet();
void BtlFallRun();
void BtlTargetStat0EHitRun(void);  /* TU defines this below */
s32 BtlAreaStat0EHitRun();
void BtlTargetAreaStat0EHitRun(void);  /* TU defines this below */
s32 BtlMarkedStat0EHitRun(void);  /* TU defines this below */
void BtlOtherSideStat0EHitRun(void);  /* TU defines this below */
s32 BtlMarkedStatus8Clear();
void BtlActingRestAnimationSet();
void BtlHitEnd(void);  /* TU defines this below */
s32 BtlTargetStatus1Set();
s32 BtlStatusTakeCheck();
void BtlSlotStatusAnimationSet(u8);
void BtlTargetStatus1TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus4Try(void);  /* TU defines this below */
void BtlTargetStatus4TryRun(void);  /* TU defines this below */
s32 BtlMarkedStatus4TryRun(void);  /* TU defines this below */
void BtlTargetAreaStatus4TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus4Set();
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlTargetStatus8Try(void);  /* TU defines this below */
void BtlTargetStatus8TryRun(void);  /* TU defines this below */
s32 BtlMarkedStatus8TryRun(void);  /* TU defines this below */
void BtlTargetAreaStatus8TryRun(void);  /* TU defines this below */
void BtlOtherSideStatus8TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus8Set();
s32 BtlTargetStatus10Set(u8);
void BtlTargetFrontMark(s32);
void BtlFrontStat0EHitStatus10Run(void);  /* TU defines this below */
s32 BtlTargetStatus30Set();
void BtlTargetStatus30TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus40Set();
void BtlTargetAreaStatus40TryRun(void);  /* TU defines this below */
void BtlTargetStatus80Try(void);  /* TU defines this below */
void BtlTargetStatus80TryRun(void);  /* TU defines this below */
void BtlTargetAreaStatus80TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus80Set();
void BtlStatRaise(u16 *, u8, s32);
s32 BtlAmountShowRun();
void BtlTargetStat08RaiseRun(void);  /* TU defines this below */
void BtlTargetStat0ARaiseRun(void);  /* TU defines this below */
s32 BtlSpecialCheck();  /* extern */
void BtlEnemyAllRemoveRun(void);  /* TU defines this below */
void BtlEnemyRemoveTryRun(u8 *unused);  /* TU defines this below */
void BtlNothingRunCopy();  /* TU defines this below */
void BtlTargetHitStatusRun(s32);
void BtlTargetHitStatus1Run(void);  /* TU defines this below */
void BtlTargetHitStatus4Run(void);  /* TU defines this below */
void BtlTargetHitStatus8Run(void);  /* TU defines this below */
void BtlTargetHitStatus10Run(void);  /* TU defines this below */
void BtlTargetStat0EHitStatus4Run(void);  /* TU defines this below */
void BtlTargetStatusSet(s32);
void BtlActingRemoveRun(void);  /* TU defines this below */
void BtlActingAreaStat0EHitStatus4Run(void);  /* TU defines this below */
void BtlActingAreaStatus4TryRun(void);  /* TU defines this below */
void BtlTargetRowsMark();
void BtlTargetRowsHpRun(void);  /* TU defines this below */
s32 rng_7f_draw(void);  /* extern */
void BtlTargetReviveRun(void);  /* TU defines this below */
void BtlSlot7HpSetRun(void);  
/* func_80032440 */
void BtlTargetStatusClearRun(u8 clear_mask)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `all_bits` only holds the constant 0xFF; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 22 instruction lines
     differs. */
  int all_bits;
  all_bits = 0xFF;
  *((u8 *) ((((void *) CharStats) + (0x3C * g_BtlTargetSlot)) + 0x15)) &= clear_mask ^ all_bits;
  BtlShowNumberRun(g_BtlActionShowNumber);
  BtlSlotStatusAnimationSet(g_BtlTargetSlot);
}

/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
s32 BtlHitTry();
s32 BtlHitAmountSet();
s32 BtlHitSlotsUpdate();
s32 BtlTargetReachCheck();
void BtlTargetHitRun(void);  /* TU defines this below */
void BtlAreaHitRun();
void BtlTargetAreaMark(u8);
void BtlActingAreaHitRun(void);  /* TU defines this below */
void BtlTargetAreaHitRun(void);  /* TU defines this below */
s32 BtlMarkedHitRun(void);  /* TU defines this below */
s32 BtlTargetOtherSideMark();
void BtlOtherSideHitRun(void);  /* TU defines this below */
s32 BtlTargetStatus8Clear();
s32 BtlMarkedRestAnimationSet();
s32 BtlStat0EHitAmountSet();
void BtlFallRun();
void BtlTargetStat0EHitRun(void);  /* TU defines this below */
s32 BtlAreaStat0EHitRun();
void BtlTargetAreaStat0EHitRun(void);  /* TU defines this below */
s32 BtlMarkedStat0EHitRun(void);  /* TU defines this below */
void BtlOtherSideStat0EHitRun(void);  /* TU defines this below */
s32 BtlMarkedStatus8Clear();
void BtlActingRestAnimationSet();
void BtlHitEnd(void);  /* TU defines this below */
s32 BtlTargetStatus1Set();
s32 BtlStatusTakeCheck();
void BtlSlotStatusAnimationSet(u8);
void BtlTargetStatus1TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus4Try(void);  /* TU defines this below */
void BtlTargetStatus4TryRun(void);  /* TU defines this below */
s32 BtlMarkedStatus4TryRun(void);  /* TU defines this below */
void BtlTargetAreaStatus4TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus4Set();
s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlTargetStatus8Try(void);  /* TU defines this below */
void BtlTargetStatus8TryRun(void);  /* TU defines this below */
s32 BtlMarkedStatus8TryRun(void);  /* TU defines this below */
void BtlTargetAreaStatus8TryRun(void);  /* TU defines this below */
void BtlOtherSideStatus8TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus8Set();
s32 BtlTargetStatus10Set(u8);
void BtlTargetFrontMark(s32);
void BtlFrontStat0EHitStatus10Run(void);  /* TU defines this below */
s32 BtlTargetStatus30Set();
void BtlTargetStatus30TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus40Set();
void BtlTargetAreaStatus40TryRun(void);  /* TU defines this below */
void BtlTargetStatus80Try(void);  /* TU defines this below */
void BtlTargetStatus80TryRun(void);  /* TU defines this below */
void BtlTargetAreaStatus80TryRun(void);  /* TU defines this below */
s32 BtlTargetStatus80Set();
void BtlStatRaise(u16 *, u8, s32);
s32 BtlAmountShowRun();
void BtlTargetStat08RaiseRun(void);  /* TU defines this below */
void BtlTargetStat0ARaiseRun(void);  /* TU defines this below */
s32 BtlSpecialCheck();  /* extern */
void BtlEnemyAllRemoveRun(void);  /* TU defines this below */
void BtlEnemyRemoveTryRun(u8 *unused);  /* TU defines this below */
void BtlNothingRunCopy();  /* TU defines this below */
void BtlTargetHitStatusRun(s32);
void BtlTargetHitStatus1Run(void);  /* TU defines this below */
void BtlTargetHitStatus4Run(void);  /* TU defines this below */
void BtlTargetHitStatus8Run(void);  /* TU defines this below */
void BtlTargetHitStatus10Run(void);  /* TU defines this below */
void BtlTargetStat0EHitStatus4Run(void);  /* TU defines this below */
void BtlTargetStatusSet(s32);
void BtlActingRemoveRun(void);  /* TU defines this below */
void BtlActingAreaStat0EHitStatus4Run(void);  /* TU defines this below */
void BtlActingAreaStatus4TryRun(void);  /* TU defines this below */
void BtlTargetRowsMark();
void BtlTargetRowsHpRun(void);  /* TU defines this below */
s32 rng_7f_draw(void);  
/* func_800324A4 */
void BtlTargetReviveRun(void)
{
  u16 *p;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `hp_index` only holds the constant 0; behaviour is the same with the constant written
     where it is read, but then 2 of this function's 57 instruction lines differ. */
  int hp_index;
  u16 max_hp;
  if (g_BtlSlotActiveFlags[g_BtlTargetSlot] == 0)
  {
    *((u8 *) (((g_BtlTargetSlot * 60) + CharStats) + 0x15)) = 0;
    g_BtlSlotActiveFlags[g_BtlTargetSlot] = 1;
    p = (u16 *) ((g_BtlTargetSlot * 60) + CharStats);
    max_hp = p[1];
    hp_index = 0;
    p[hp_index] = max_hp;
    if ((g_BtlActionKind == 4) && (g_BtlActionNumber == 0x83))
    {
      p[2] = p[3];
    }
    *((u16 *) (((g_BtlTargetSlot * 60) + CharStats) + 8)) = *((u16 *) (((g_BtlTargetSlot * 60) + CharStats) + 0x1A));
    *((u16 *) (((g_BtlTargetSlot * 60) + CharStats) + 0xA)) = *((u16 *) (((g_BtlTargetSlot * 60) + CharStats) + 0x1C));
  }
  BtlShowNumberRun(g_BtlActionShowNumber);
}
/* func_800325AC */
void BtlPartyFlag2SetRun(void)
{
  g_BtlTargetSlot = 0;
  do
  {
    if (*(g_BtlTargetSlot + g_BtlSlotActiveFlags) != 0)
    {
      *(u8 *)(CharStats + g_BtlTargetSlot * 60 + 0x1E) |= 2;
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
    }
    g_BtlTargetSlot++;
  } while (((u32) (g_BtlTargetSlot & 0xFF)) < 5);
  BtlShowNumberRun(g_BtlActionShowNumber);
}
/* far_syms: CharStats g_BtlTargetSlot g_BtlSlotAmounts g_BtlSlotActiveFlags g_BtlSlotAmountKinds */
void BtlSlotActionAnimationSet(u8 param);  /* extern */

/* Effect 14 (scene_state_table[13]), carried by spell row 7 only (no item
   row): the party to full HP with every status lifted. BtlTargetPartyMark flags
   its set first (g_BtlSlotAmountKinds[i] = 1), but the loop does not read those
   flags: it walks the party slots 0..4 itself and takes every one that is
   active (g_BtlSlotActiveFlags[i]) or whose status byte (+0x15) is exactly 0x30, the
   stone state BtlTargetStatus30Set and the round's status tick leave a member in,
   off the field. For each, in slot order:

     g_BtlSlotAmounts[i] = maxhp - hp      the amount shown
     hp = maxhp
     +0x15 = 0                       EVERY status bit, not only 0x30
     g_BtlSlotActiveFlags[i] = 1               on the field (again, for a stone one)
     g_BtlSlotAmountKinds[i] = 5               the heal reaction
     +0x1E &= ~4

   then one BtlShowNumberRun(g_BtlActionShowNumber), the presentation, and after it
   BtlSlotActionAnimationSet(i), the standing pose, for every slot 0..4 whose
   g_BtlSlotAmountKinds is non-zero (the healed ones, and whatever BtlTargetPartyMark
   flagged there). No RNG, no MP or level term. A fallen member is NOT
   taken: BtlFallRun leaves it inactive with status 0, so neither test
   holds (the revive is effect 12, BtlTargetReviveRun). The set is fixed to
   0..4 whatever BtlTargetPartyMark chose, so a confused caster still heals the
   party.

   Shape notes. No record pointer local: the 0x3C-byte record is spelled
   from CharStats and g_BtlTargetSlot at each use, and cse joins the uses that
   the target joins. The order hp-then-status is what the bytes prove: the
   halfword stores (the amount through the array member, hp through the
   record) leave the two scalars' loads valid, so the record address
   computed for the subtraction serves the hp store and the status store;
   the status store is a byte store through a varying address, after which
   everything is reloaded, which is why g_BtlTargetSlot is read again for each
   of the three statements that follow. With the status store first (as
   m2c orders it, following the scheduled asm) the hp store has to rebuild
   the address: 101/108. The amount's destination is expanded before its
   value, which is the index * 2 the target computes ahead of the test;
   a pointer local assigned before the statement computes the record first
   (86/108). */
/* func_8003266C */
void BtlPartyRestoreRun(void)
{
    BtlTargetPartyMark();
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++) {
        if (g_BtlSlotActiveFlags[g_BtlTargetSlot] != 0
            || *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x15) == 0x30) {
            ((HealAmounts *)g_BtlSlotAmounts)->v[g_BtlTargetSlot] =
                ((BtlStats *)(CharStats + g_BtlTargetSlot * 0x3C))->hp_max
                - ((BtlStats *)(CharStats + g_BtlTargetSlot * 0x3C))->hp;
            ((BtlStats *)(CharStats + g_BtlTargetSlot * 0x3C))->hp =
                ((BtlStats *)(CharStats + g_BtlTargetSlot * 0x3C))->hp_max;
            *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x15) = 0;
            g_BtlSlotActiveFlags[g_BtlTargetSlot] = 1;
            g_BtlSlotAmountKinds[g_BtlTargetSlot] = 5;
            *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1E) &= 0xFB;
        }
    }
    BtlShowNumberRun(g_BtlActionShowNumber);
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 5; g_BtlTargetSlot++) {
        if (g_BtlSlotAmountKinds[g_BtlTargetSlot] != 0) {
            BtlSlotActionAnimationSet(g_BtlTargetSlot);
        }
    }
}

void BtlFrontHitRun(void);  /* TU defines this below */
void BtlFrontStat0EHitRun(void);  /* TU defines this below */
void BtlTargetStatus10TryRun(void);  /* TU defines this below */
void BtlTargetStat08Stat0ARaiseRun(void);  /* TU defines this below */
void BtlTargetFallRun(void);  /* TU defines this below */
void BtlTargetRemoveRun(void);  /* TU defines this below */
void BtlTargetHold86Run(void);  /* TU defines this below */
void BtlTargetStatus8Run(void);  /* TU defines this below */
void BtlFacingBothSet(void);  /* extern */
void BtlSlotActionAnimationSet(u8 param);  
/* func_8003281C */
void BtlEnemyClearRun(void)
{
  u8 *stats;
  u8 next_mark_target;
  u8 next_clear_target;
  if (!(BtlSpecialCheck() & 0xFF))
  {
    g_BtlTargetSlot = 6;
    do
    {
      if ((*(g_BtlTargetSlot + g_BtlSlotActiveFlags)) != 0)
      {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
      }
      next_mark_target = g_BtlTargetSlot + 1;
      g_BtlTargetSlot = next_mark_target;
    }
    while (((u32) (next_mark_target & 0xFF)) < 0xEU);
  }
  BtlShowNumberRun(g_BtlActionShowNumber);
  g_BtlTargetSlot = 6;
  do
  {
    if ((*(g_BtlTargetSlot + g_BtlSlotAmountKinds)) == 1)
    {
      stats = (u8 *) ((g_BtlTargetSlot * 0x3C) + CharStats);
      stats[21] = (*(g_BtlTargetSlot + g_BtlSlotActiveFlags) = (*(g_BtlTargetSlot + g_BtlSlotIds) = (*((u16 *) stats) = 0)));
    }
    next_clear_target = g_BtlTargetSlot + 1;
    g_BtlTargetSlot = next_clear_target;
  }
  while (((u32) (next_clear_target & 0xFF)) < 0xEU);
}

s32 BtlHitTry();                                  /* extern */
s32 BtlHitAmountSet();                                  /* extern */
s32 BtlHitSlotsUpdate();                                  /* extern */
s32 BtlTargetReachCheck();                                /* extern */

/* func_80032958 */
void BtlTargetHitRun(void) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
        if (g_BtlAreaHalfHeight != 0) {
            BtlHitTry();
        }
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 1) {
            BtlHitAmountSet();
        }
        BtlShowNumberRun(g_BtlActionShowNumber);
        BtlHitSlotsUpdate();
    }
}

void BtlAreaHitRun();                                  /* extern */
void BtlTargetAreaMark(u8);                                 /* extern */

/* func_80032A00 */
void BtlActingAreaHitRun(void) {
    BtlTargetAreaMark(0);
    BtlAreaHitRun();
}
/* func_80032A28 */
void BtlTargetAreaHitRun(void) {
    BtlTargetAreaMark(1);
    BtlAreaHitRun();
}
/* far_syms: g_BtlTargetSlot */

/* The worker of effects 18 and 19 (scene_state_table[17] / [18]:
   BtlActingAreaHitRun = BtlTargetAreaMark(0) then this, BtlTargetAreaHitRun =
   BtlTargetAreaMark(1) then this): the physical resolver on every slot the
   area set flagged, with the attack's percent bonus picked by the flag.

   The target index g_BtlTargetSlot is saved, then walked over the fourteen
   slots 0..13. A slot whose hit flag g_BtlSlotAmountKinds[slot] is 0 is skipped.
   Otherwise the percent bonus g_BtlActionPower (BtlHitAmountSet's
   atk + atk * bonus / 100) is set by the flag:
     1 (in the area)        -> 70, the flag stays 1
     2                      -> 50, the flag becomes 1
     3 (the target itself)  -> 100, the flag becomes 1
     any other value        -> g_BtlActionPower is left as it is (the row's
                               magnitude, or the previous flagged slot's
                               bonus) and the flag is not touched
   and BtlHitAmountSet() resolves the damage on that slot. The row's own
   magnitude is therefore never used for flags 1..3.

   After the walk g_BtlTargetSlot is restored and BtlHitEnd() runs (KO
   cleanup and poses). No hit check, no evasion, no RNG draw and no frame
   wait of its own; those are BtlHitAmountSet's and BtlHitEnd's.

   Shape: nothing needed shaping. The slot's flag address is a pointer
   local (the target computes it once and stores through it), the tests
   are one switch, and the loop counter is the far global itself; the
   constant 1 and the table address in saved registers are the loop
   pass's hoists. */
/* func_80032A50 */
void BtlAreaHitRun(void)
{
    u8 saved;
    u8 *p;

    saved = g_BtlTargetSlot;
    for (g_BtlTargetSlot = 0; g_BtlTargetSlot < 14; g_BtlTargetSlot++) {
        p = &g_BtlSlotAmountKinds[g_BtlTargetSlot];
        if (*p != 0) {
            switch (*p) {
            case 1:
                g_BtlActionPower = 0x46;
                break;
            case 2:
                g_BtlActionPower = 0x32;
                *p = 1;
                break;
            case 3:
                g_BtlActionPower = 0x64;
                *p = 1;
                break;
            }
            BtlHitAmountSet();
        }
    }
    g_BtlTargetSlot = saved;
    BtlHitEnd();
}

/* func_80032B44 */
void BtlFrontHitRun(void)
{
  int stats;
  stats = g_BtlActingSlot * 0x3C;
  stats = CharStats + stats;
  ;
  if ((((BtlStats *)stats)->status & 0x40) != 0)
  {
    BtlTargetFrontMark(2);
  }
  else
    if (g_BtlActingSlot < 5)
  {
    BtlTargetFrontMark(1);
  }
  else
  {
    BtlTargetFrontMark(0);
  }
  BtlMarkedHitRun();
}

s32 BtlMarkedHitRun();                                  /* extern */
s32 BtlTargetOtherSideMark();                                  /* extern */

/* func_80032BB4 */
void BtlOtherSideHitRun(void) {
    BtlTargetOtherSideMark();
    BtlMarkedHitRun();
}

s32 BtlTargetStatus8Clear();                                  /* extern */
s32 BtlMarkedRestAnimationSet();                                  /* extern */
s32 BtlStat0EHitAmountSet();                                  /* extern */
s32 BtlTargetReachCheck();                                /* extern */
void BtlFallRun();                                  /* extern */

/* func_80032BDC */
void BtlTargetStat0EHitRun(void) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        BtlStat0EHitAmountSet();
        BtlShowNumberRun(g_BtlActionShowNumber);
        BtlTargetStatus8Clear();
        BtlFallRun();
        BtlMarkedRestAnimationSet();
    }
}

s32 BtlAreaStat0EHitRun();                                  /* extern */

/* func_80032C3C */
void BtlTargetAreaStat0EHitRun(void) {
    BtlTargetAreaMark(1);
    BtlAreaStat0EHitRun();
}
s32 ratio_13_10_mul(s32);
void BtlTargetHpAmountTake(u32 amount);  /* TU defines this below */

/* func_80032C64 */
s32 BtlAreaStat0EHitRun(void)
{
  s32 rng_mask_below(); /* this call site reads the result unmasked (int) */
  s32 mult;
  u8 saved;
  s32 r;
  u16 def;
  s32 v;

  mult = 1;
  saved = g_BtlTargetSlot;
  if ((g_BtlActionKind == 3) && (g_BtlActionNumber == 0x1A))
  {
    mult = 2;
  }
  g_BtlTargetSlot = 0;
  do
  {
    if (g_BtlSlotAmountKinds[g_BtlTargetSlot] != 0)
    switch (g_BtlSlotAmountKinds[g_BtlTargetSlot])
    {
      case 1:
        r = rng_mask_below(0xFFFF, (u16) (*((u16 *) ((CharStats + (g_BtlActingSlot * 0x3C)) + 0xE)) / 10));
        def = *((u16 *) ((CharStats + (g_BtlTargetSlot * 0x3C)) + 0x10));
        if ((g_BtlTargetSlot < 5) && (g_BtlSlotRestAnimations[g_BtlTargetSlot] == 0x13))
        {
          def = ratio_13_10_mul(def);
        }
        r = mult * (r + ((*((u8 *) ((CharStats + (g_BtlActingSlot * 0x3C)) + 0x14)) * 5) >> 1));
        v = ((*((u16 *) ((CharStats + (g_BtlActingSlot * 0x3C)) + 0xE)) + (u16) r) - def) / 2;
        if (v < 0)
        {
          v = 0;
        }
        BtlTargetHpAmountTake(v);
        break;
      case 2:
        g_BtlActionPower = (u16) mult * 2;
        BtlStat0EHitAmountSet();
        break;
      case 3:
        g_BtlActionPower = (u16) mult * 3;
        BtlStat0EHitAmountSet();
        break;
    }
    g_BtlTargetSlot++;
  }
  while (g_BtlTargetSlot < 14);
  g_BtlTargetSlot = saved;
  BtlShowNumberRun(g_BtlActionShowNumber);
  BtlFallRun();
  BtlMarkedRestAnimationSet();
  return BtlMarkedStatus8Clear();
}

s32 BtlMarkedStat0EHitRun();                                  /* extern */

/* func_80032ED8 */
void BtlOtherSideStat0EHitRun(void) {
    BtlTargetOtherSideMark();
    BtlMarkedStat0EHitRun();
}

/* func_80032F00 */
void BtlFrontStat0EHitRun(void)
{
    if (g_BtlNumber == 0xFE)
    {
        if (g_BtlActionNumber == 0x73)
        {
            g_BtlActionPower = 0x1E;
            g_BtlAreaHalfHeight = 0x8;
        }
    }
    if (((*(u8 *)(CharStats + g_BtlActingSlot * 60 + 0x15)) & 0x40) != 0)
    {
        BtlTargetFrontMark(2);
    }
    else if (g_BtlActingSlot < 5)
    {
        BtlTargetFrontMark(1);
    }
    else
    {
        BtlTargetFrontMark(0);
    }
    BtlMarkedStat0EHitRun();
}

s32 BtlMarkedStatus8Clear();                                  /* extern */
void BtlActingRestAnimationSet();                                  /* extern */

/* func_80032FA0 */
s32 BtlMarkedStat0EHitRun(void) {
    u8 saved_target;
    u8 next_target;

    saved_target = g_BtlTargetSlot;
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) != 0) {
            BtlStat0EHitAmountSet();
        }
        next_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_target;
    } while ((u32) (next_target & 0xFF) < 0xEU);
    g_BtlTargetSlot = saved_target;
    BtlShowNumberRun(g_BtlActionShowNumber);
    BtlActingRestAnimationSet();
    BtlFallRun();
    BtlMarkedRestAnimationSet();
    BtlMarkedStatus8Clear();
}
void BtlHitEnd(void);  /* TU defines this below */

/* func_80033060 */
s32 BtlMarkedHitRun(void) {
    u8 saved_target;
    u8 next_target;

    saved_target = g_BtlTargetSlot;
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) != 0) {
            BtlHitAmountSet();
        }
        next_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_target;
    } while ((u32) (next_target & 0xFF) < 0xEU);
    g_BtlTargetSlot = saved_target;
    BtlHitEnd();
}

s32 BtlMarkedStatus8Clear();                                  /* extern */
s32 BtlMarkedRestAnimationSet();                                  /* extern */
void BtlActingRestAnimationSet();                                  /* extern */
void BtlFallRun();                                  /* extern */

/* func_800330FC */
void BtlHitEnd(void) {
    BtlShowNumberRun(g_BtlActionShowNumber);
    BtlActingRestAnimationSet();
    BtlFallRun();
    BtlMarkedRestAnimationSet();
    BtlMarkedStatus8Clear();
}
s32 BtlTargetStatus1Set();                                  /* extern */
s32 BtlStatusTakeCheck();                                /* extern */
void BtlSlotStatusAnimationSet(u8);                                /* extern */

/* func_80033140 */
void BtlTargetStatus1TryRun(void) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
        if (!((*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x15)) & 0x35) && (BtlStatusTakeCheck() & 0xFF)) {
            BtlTargetStatus1Set();
        }
        BtlShowNumberRun(g_BtlActionShowNumber);
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
    }
}

s32 BtlTargetStatus4Try();                                  /* extern */
s32 BtlTargetReachCheck();                                /* extern */
void BtlSlotStatusAnimationSet(u8);                                /* extern */
s32 BtlShowNumberRun();                                /* extern */

/* func_8003321C */
void BtlTargetStatus4TryRun(void) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        BtlTargetStatus4Try();
        BtlShowNumberRun(g_BtlActionShowNumber);
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
    }
}

s32 BtlMarkedStatus4TryRun();                                  /* extern */

/* func_80033290 */
void BtlTargetAreaStatus4TryRun(void) {
    BtlTargetAreaMark(1);
    BtlMarkedStatus4TryRun();
}
/* func_800332B8 */
s32 BtlMarkedStatus4TryRun(void) {
    u8 saved_target;
    u8 next_try_target;
    u8 next_animation_target;

    saved_target = g_BtlTargetSlot;
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) != 0) {
            BtlTargetStatus4Try();
        }
        next_try_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_try_target;
    } while ((u32) (next_try_target & 0xFF) < 0xEU);
    g_BtlTargetSlot = saved_target;
    BtlShowNumberRun(g_BtlActionShowNumber);
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
        next_animation_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_animation_target;
    } while ((u32) (next_animation_target & 0xFF) < 0xEU);
}
s32 BtlAnimationSet();

s32 BtlTargetStatus4Set();                                  /* extern */

/* func_800333B8 */
s32 BtlTargetStatus4Try(void)
{
  s32 status;
  *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
  status = ((u8 *) (CharStats + (g_BtlTargetSlot * 0x3C))) + 0x15;
  if ((!((*((u8 *) status)) & 0x34)) && (BtlStatusTakeCheck() & 0xFF))
  {
    BtlTargetStatus4Set();
  }
}

s32 BtlWalkSet();
s32 BtlWalkRun();
s32 BtlTargetStatus8Try();

/* func_80033440 */
void BtlTargetStatus8TryRun(void)
{
  if (g_BtlAreaHalfWidth != 0)
  {
    if ((((u8) g_BtlActingSlot) >= 6U) && ((*(g_BtlActingSlot + g_BtlSlotIds)) == 0x10))
    {
      BtlWalkSet();
      g_BtlWalkBlock.walker_width_counts = 0;
      g_BtlWalkBlock.target_width_counts = 0;
      g_BtlWalkBlock.walk = 0;
      if (!(BtlWalkRun() & 0xFF))
      {
        g_BtlWalkBlock.walk = 1;
        BtlWalkRun();
        return;
      }
      goto block_6;
    }
    if (BtlTargetReachCheck() & 0xFF)
    {
      goto block_6;
    }
  }
  else
  {
    block_6:
    BtlTargetStatus8Try();

    BtlShowNumberRun(g_BtlActionShowNumber);
    BtlSlotStatusAnimationSet(g_BtlTargetSlot);
  }
}

s32 BtlMarkedStatus8TryRun();                                  /* extern */

/* func_80033510 */
void BtlTargetAreaStatus8TryRun(void) {
    BtlTargetAreaMark(1);
    BtlMarkedStatus8TryRun();
}
/* func_80033538 */
void BtlOtherSideStatus8TryRun(void) {
    BtlTargetOtherSideMark();
    BtlMarkedStatus8TryRun();
}
/* func_80033560 */
s32 BtlMarkedStatus8TryRun(void) {
    u8 saved_target;
    u8 next_try_target;
    u8 next_animation_target;

    saved_target = g_BtlTargetSlot;
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) != 0) {
            BtlTargetStatus8Try();
        }
        next_try_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_try_target;
    } while ((u32) (next_try_target & 0xFF) < 0xEU);
    g_BtlTargetSlot = saved_target;
    BtlShowNumberRun(g_BtlActionShowNumber);
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
        next_animation_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_animation_target;
    } while ((u32) (next_animation_target & 0xFF) < 0xEU);
}
s32 BtlTargetStatus8Set();

/* func_80033660 */
s32 BtlTargetStatus8Try(void)
{
  void *stats;
  u8 target;
  int stats_address;
  stats_address = CharStats + (g_BtlTargetSlot * 0x3C);
  target = g_BtlTargetSlot;
  *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
  stats = stats_address;
  if (((!(((BtlStats *)stats)->status & 0x35)) && (BtlStatusTakeCheck(target, stats) & 0xFF)) != 0)
  {
    BtlTargetStatus8Set();
  }
}
/* func_800336DC */
void BtlTargetStatus10TryRun(void) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        if ((g_BtlTargetSlot < 5) && (BtlStatusTakeCheck() & 0xFF)) {
            *(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x15) = 0x10;
        }
        BtlShowNumberRun(g_BtlActionShowNumber);
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
    }
}

s32 BtlTargetStatus10Set(u8);                                /* extern */
void BtlTargetFrontMark(s32);                                 /* extern */

/* func_800337A0 */
void BtlFrontStat0EHitStatus10Run(void) {
    u8 next_target;

    BtlTargetFrontMark(0);
    BtlMarkedStat0EHitRun();
    g_BtlTargetSlot = 0;
    do {
        if ((*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 1) && !((*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x15)) & 0x30) && (*(g_BtlTargetSlot + g_BtlSlotFallMarks) == 0)) {
            BtlTargetStatus10Set(g_BtlTargetSlot);
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
        next_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_target;
    } while ((u32) (next_target & 0xFF) < 5U);
}

s32 BtlTargetStatus30Set();                                  /* extern */
s32 BtlStatusTakeCheck();                                /* extern */

/* func_80033884 */
void BtlTargetStatus30TryRun(void) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        if (BtlStatusTakeCheck() & 0xFF) {
            BtlTargetStatus30Set();
        }
        BtlShowNumberRun(g_BtlActionShowNumber);
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
    }
}
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
s32 BtlTargetStatus40Set();

/* func_8003390C */
void BtlTargetAreaStatus40TryRun(void)
{
  u8 *amount_kind;
  u8 saved_target;
  u8 try_target;
  u8 next_animation_target;
  s32 status;
  BtlTargetAreaMark(1);
  saved_target = g_BtlTargetSlot;
  g_BtlTargetSlot = 0;
  do
  {
    amount_kind = g_BtlTargetSlot + g_BtlSlotAmountKinds;
    if ((*amount_kind) != 0)
    {
      status = ((u8 *) (CharStats + (g_BtlTargetSlot * 0x3C))) + 0x15;
      try_target = g_BtlTargetSlot;
      *amount_kind = 2;
      if ((!((*((u8 *) status)) & 0x3D)) && (BtlStatusTakeCheck(try_target, amount_kind) & 0xFF))
      {
        BtlTargetStatus40Set();
      }
    }
    try_target = g_BtlTargetSlot + 1;
    g_BtlTargetSlot = try_target;
  }
  while (((u32) (try_target & 0xFF)) < 0xEU);
  g_BtlTargetSlot = saved_target;
  BtlShowNumberRun(g_BtlActionShowNumber);
  g_BtlTargetSlot = 0;
  do
  {
    if ((*(g_BtlTargetSlot + g_BtlSlotAmountKinds)) == 7)
    {
      BtlSlotStatusAnimationSet(g_BtlTargetSlot);
    }
    next_animation_target = g_BtlTargetSlot + 1;
    g_BtlTargetSlot = next_animation_target;
  }
  while (((u32) (next_animation_target & 0xFF)) < 0xEU);
}

void BtlTargetStatus80Try(void);                                  /* extern */

/* func_80033A58 */
void BtlTargetStatus80TryRun(void) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        BtlTargetStatus80Try();
        BtlShowNumberRun(g_BtlActionShowNumber);
        BtlSlotStatusAnimationSet(g_BtlTargetSlot);
    }
}

void BtlTargetStatus80Try(void);                                  /* extern */
void BtlSlotStatusAnimationSet(u8);                                /* extern */
s32 BtlShowNumberRun();                                /* extern */

/* func_80033AB0 */
void BtlTargetAreaStatus80TryRun(void) {
    u8 saved_target;
    u8 next_try_target;
    u8 next_animation_target;

    BtlTargetAreaMark(1);
    saved_target = g_BtlTargetSlot;
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) != 0) {
            BtlTargetStatus80Try();
        }
        next_try_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_try_target;
    } while ((u32) (next_try_target & 0xFF) < 0xEU);
    g_BtlTargetSlot = saved_target;
    BtlShowNumberRun(g_BtlActionShowNumber);
    g_BtlTargetSlot = 0;
    do {
        if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
            BtlSlotStatusAnimationSet(g_BtlTargetSlot);
        }
        next_animation_target = g_BtlTargetSlot + 1;
        g_BtlTargetSlot = next_animation_target;
    } while ((u32) (next_animation_target & 0xFF) < 0xEU);
}
s32 BtlTargetStatus80Set();                                  /* extern */

/* func_80033BB8 */
void BtlTargetStatus80Try(void) {
    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
    if (((*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x15)) == 0) && (BtlStatusTakeCheck() & 0xFF)) {
        BtlTargetStatus80Set();
    }
}
/* far_syms: CharStats g_BtlActingSlot g_BtlTargetSlot */

/* Effect 54 (scene_init_table[30], 0x800B2FCC): attack up on the CASTER.
   The target slot g_BtlTargetSlot is first set to the caster g_BtlActingSlot; the
   rest is effect 55 (BtlTargetStat08RaiseRun, below) on that slot:
     - BtlStatRaise(&row->attack, caster, row->attack_base): the caster's
       attack (+0x08 of its 0x3C record at CharStats) goes up by
       g_BtlActionPower percent of itself, held to 170% of +0x1A; the amount
       added is left in g_BtlSlotAmounts[caster] and g_BtlSlotAmountKinds[target] = 5 on
       the way (BtlStatRaise);
     - BtlShowNumberRun(g_BtlActionShowNumber), the presentation;
     - g_BtlSlotAmountKinds[target] = 8 (the "attack up" result, menu-ui.md) and
       BtlAmountShowRun().
   No RNG draw and no frame wait of its own.
   Shape: the store to g_BtlTargetSlot is the first statement (g_BtlActingSlot is
   read again after it, and that read serves the row and the call's
   second argument; with the row taken first it is 29/32); the row is
   base + index (product first: 26/32); and +0x1A is read as a struct
   MEMBER. As `*(u16 *)((u8 *)row + 0x1A)` the load stays behind the byte
   store (29/32); the member read is scheduled ahead of it. */
/* func_80033C3C */
void BtlActingStat08RaiseRun(void)
{
    BtlStats *row;

    g_BtlTargetSlot = g_BtlActingSlot;
    row = (BtlStats *)CharStats + g_BtlActingSlot;
    BtlStatRaise(&row->stat_08, g_BtlActingSlot, row->side.party.stat_08_start);
    BtlShowNumberRun(g_BtlActionShowNumber);
    g_BtlSlotAmountKinds[g_BtlTargetSlot] = 8;
    BtlAmountShowRun();
}

/* the callee takes a full-width value (it adds it unmasked); the callers
   truncate to u16 themselves — a narrow prototype in the original
   callers' TU, spelled here as casts at the call (one-file game_state) */
void BtlStatRaise(u16 *, u8, s32);                   /* extern */
s32 BtlAmountShowRun();                                  /* extern */

/* func_80033CBC */
void BtlTargetStat08RaiseRun(void) {
    void *stats;

    stats = (void *)CharStats + (g_BtlTargetSlot * 0x3C);
    BtlStatRaise(stats + 8, g_BtlTargetSlot, (u16)(AS(s16, ((BtlStats *)stats)->side.party.stat_08_start)));
    BtlShowNumberRun(g_BtlActionShowNumber);
    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 8;
    BtlAmountShowRun();
}
/* func_80033D2C */
void BtlTargetStat0ARaiseRun(void) {
    void *stats;

    stats = (void *)CharStats + (g_BtlTargetSlot * 0x3C);
    BtlStatRaise(stats + 0xA, g_BtlTargetSlot, (u16)AS(s32, ((BtlStats *)stats)->side.party.stat_0a_start));
    BtlShowNumberRun(g_BtlActionShowNumber);
    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 9;
    BtlAmountShowRun();
}
/* func_80033D9C */
void BtlTargetStat08Stat0ARaiseRun(void) {
    void *stats;

    BtlShowNumberRun(g_BtlActionShowNumber);
    stats = (void *)CharStats + (g_BtlTargetSlot * 0x3C);
    BtlStatRaise(stats + 8, g_BtlTargetSlot, ((BtlStats *)stats)->side.party.stat_08_start);
    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 8;
    BtlAmountShowRun();
    stats = (void *)CharStats + (g_BtlTargetSlot * 0x3C);
    BtlStatRaise(stats + 0xA, g_BtlTargetSlot, ((BtlStats *)stats)->side.party.stat_0a_start);
    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 9;
    BtlAmountShowRun();
}
/* func_80033E50 */
void BtlStatRaise(u16 *stat, unsigned char slot, s32 base)
{
  s32 amount;
  u16 *amounts;
  u32 old_value;
  int masked_base_then_limit;
  u16 limit;
  u32 new_value;
  *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 5;
  limit = *stat;
  masked_base_then_limit = 0xFFFF & base;
  old_value = limit;
  masked_base_then_limit = base + (((masked_base_then_limit * 0x46) / 100));
  new_value = masked_base_then_limit & 0xFFFF;
  limit = new_value;
  amount = g_BtlActionPower;
  amount = (old_value * amount) / 100;
  new_value = old_value + amount;
  if (limit < new_value)
  {
    amount -= new_value - limit;
  }
  amounts = (u16 *)g_BtlSlotAmounts;
  *stat = old_value + amount;
  amounts[slot] = amount;
}

s32 BtlSpecialCheck();

/* func_80033F0C */
void BtlEnemyAllRemoveRun(void)
{
  u8 *active_flag;
  u8 next_target;
  if (!(BtlSpecialCheck() & 0xFF))
  {
    g_BtlTargetSlot = 6;
    do
    {
      active_flag = g_BtlTargetSlot + g_BtlSlotActiveFlags;
      if ((*active_flag) != 0)
      {
        *(g_BtlTargetSlot + g_BtlSlotIds) = (*active_flag = 0);
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
      }
      next_target = g_BtlTargetSlot + 1;
      g_BtlTargetSlot = next_target;
    }
    while (((u32) (next_target & 0xFF)) < 0xEU);
  }
  BtlShowNumberRun(g_BtlActionShowNumber);
}

/* func_80033FCC */
void BtlEnemyRemoveTryRun(u8 *unused)
{
  u8 *amount_kind;
  u8 next_try_target;
  u8 next_remove_target;
  amount_kind = unused;
  g_BtlTargetSlot = 6;
  do
  {
    if ((*(g_BtlTargetSlot + g_BtlSlotActiveFlags)) != 0)
    {
      BtlStatusTakeCheck(amount_kind);
    }
    amount_kind = g_BtlTargetSlot + g_BtlSlotAmountKinds;
    if ((*amount_kind) == 7)
    {
      *amount_kind = 1;
    }
    next_try_target = g_BtlTargetSlot + 1;
    g_BtlTargetSlot = next_try_target;
  }
  while (((u32) (next_try_target & 0xFF)) < 0xEU);
  BtlShowNumberRun(g_BtlActionShowNumber);
  g_BtlTargetSlot = 6;
  do
  {
    if ((*(g_BtlTargetSlot + g_BtlSlotAmountKinds)) == 1)
    {
      *(g_BtlTargetSlot + g_BtlSlotIds) = (*(g_BtlTargetSlot + g_BtlSlotActiveFlags) = 0);
    }
    next_remove_target = g_BtlTargetSlot + 1;
    g_BtlTargetSlot = next_remove_target;
  }
  while (((u32) (next_remove_target & 0xFF)) < 0xEU);
}

/* func_80034108 */
void BtlPartyHpDrainRun(void)
{
  u16 *p;
  u8 *amount_kind;
  s32 sum = 0;
  g_BtlTargetSlot = 0;
  do
  {
    if ((*(g_BtlTargetSlot + g_BtlSlotActiveFlags)) != 0)
    {
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
      BtlStat0EHitAmountSet();
      sum += *((u16 *) (g_BtlSlotAmounts + (g_BtlTargetSlot * 2)));
    }
    g_BtlTargetSlot++;
  }
  while (((u32) (g_BtlTargetSlot & 0xFF)) < 5);
  p = (u16 *) ((g_BtlActingSlot * 60) + CharStats);
  p[0] += sum;
  if (p[1] < p[0])
  {
    p[0] = p[1];
  }
  amount_kind = g_BtlActingSlot + g_BtlSlotAmountKinds;
  *((u16 *) (g_BtlSlotAmounts + (g_BtlActingSlot * 2))) = sum;
  *amount_kind = 5;
  BtlShowNumberRun(g_BtlActionShowNumber);
  BtlFallRun();
}

/* func_80034260 */
void BtlSlot7HpSetRun(void) {
    u8 level;

    BtlShowNumberRun(g_BtlActionShowNumber);
    level = *(u8 *)((void *)CharStats + (g_BtlActingSlot * 0x3C) + 0x14);
    *(s16 *)((void *)CharStats + 0x1A4) = level * 0xA + 0x1F4;
}
/* far_syms: g_BtlSlotFallMarks (target reaches it %hi/%lo) */

/* func_800342C0 */
void BtlTargetFallRun(void) {
    u8 flags;
    void *stats;

    stats = (g_BtlTargetSlot * 0x3C) + CharStats;
    flags = AS(s16, ((BtlStats *)stats)->side.party.flags);
    if (flags & 2) {
        ((BtlStats *)stats)->side.party.flags = (u8) (flags & 0xFD);
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 6;
        BtlShowNumberRun(g_BtlActionShowNumber, stats);
    } else {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
        *(g_BtlTargetSlot + g_BtlSlotFallMarks) = 1;
        BtlShowNumberRun(g_BtlActionShowNumber, stats);
        BtlActingRestAnimationSet();
        BtlFallRun();
    }
    BtlMarkedRestAnimationSet();
}

/* Ruling of 2026-10-14 (docs/naming.md): Does what BtlNothingRun does; nothing read says why there are two. */
/* func_80034390 */
void BtlNothingRunCopy() {

}

void BtlTargetHitStatusRun(s32);                                 /* extern */

/* func_80034398 */
void BtlTargetHitStatus1Run(void) {
    BtlTargetHitStatusRun(1);
}
/* func_800343B8 */
void BtlTargetHitStatus4Run(void) {
    BtlTargetHitStatusRun(4);
}
/* func_800343D8 */
void BtlTargetHitStatus8Run(void) {
    BtlTargetHitStatusRun(8);
}
/* func_800343F8 */
void BtlTargetHitStatus10Run(void) {
    BtlTargetHitStatusRun(16);
}

/* func_80034418 */
void BtlTargetHitStatusRun(s32 status)
{
  s32 kept_status;
  s32 amount_kind;
  BtlTargetHitRun();
  kept_status = status;
  amount_kind = *(g_BtlTargetSlot + g_BtlSlotAmountKinds);
  if ((amount_kind == 1) || (((amount_kind != 0) && (amount_kind < 5)) && (amount_kind >= 3)))
  {
    if ((*(g_BtlTargetSlot + g_BtlSlotFallMarks)) == 0)
    {
      BtlTargetStatusSet(kept_status & 0xFF);
    }
  }
  return 0;
}

/* func_80034498 */
void BtlTargetStat0EHitStatus4Run(void) {
    BtlTargetStat0EHitStatusRun(4);
}

void BtlTargetStatusSet(s32);                               /* extern */

/* func_800344B8 */
void BtlTargetStat0EHitStatusRun(s32 status) {
    if ((g_BtlAreaHalfWidth == 0) || (BtlTargetReachCheck() & 0xFF)) {
        BtlStat0EHitAmountSet();
        BtlShowNumberRun(g_BtlActionShowNumber);
        if (*(g_BtlTargetSlot + g_BtlSlotFallMarks) != 0) {
            BtlFallRun();
            return;
        }
        BtlTargetStatusSet(status & 0xFF);
    }
}
extern u8 g_BtlSlotStatus80Rounds[];
extern u8 g_BtlSlotStatus40Rounds[];
extern u8 g_BtlSlotStatus8Rounds[];
extern u8 g_BtlSlotStatus4Rounds[];

/* BtlTargetStatusSet -- the status a special tries on the target
   g_BtlTargetSlot after its hit: BtlTargetHitStatusRun (effects 64 / 66 / 67 / 68, codes
   1 / 4 / 8 / 0x10) calls it when the hit landed (g_BtlSlotAmountKinds 1, 3 or 4) and
   g_BtlSlotFallMarks[t] is clear, BtlTargetStat0EHitStatusRun (effect 75, code 4) whenever
   g_BtlSlotFallMarks[t] is clear.

   - An ENEMY target (slot >= 6) with a boss id (g_BtlSlotIds[t] >= 0x6E):
     nothing at all, not even the pose.
   - g_BtlSlotAmountKinds[t] == 6 (the barrier took the hit): nothing at all.
   - 1 / 4 / 8 / 0x30 / 0x40 / 0x80: the applier BtlHitStatusUpdate also uses
     (BtlTargetStatus1Set / F74C / F958 / FAFC / FC08 / FC94 -- each has its own
     guard on +0x15 and its own BtlTargetTraitCheck resistance test).
   - 0x10 (stone, first stage): skipped while the target's status byte has
     bit 0x20 (so under 0x20 and under 0x30). Otherwise -- and for 0x20
     with no guard at all -- BtlTargetTraitCheck(0x800, 0x25, 0x26, 0) == 1
     resists it; else the code is STORED as the whole status byte (+0x15 of
     the 0x3C record: every other status bit is overwritten) and the four
     tick counters g_BtlSlotStatus80Rounds / 49B0 / 4930 / 48B0 [t] are cleared. Unlike
     BtlTargetStatus30Set (0x30) it neither clears the active flag nor a party
     member's +0x1E bit 4.
   - Any other code: nothing.
   Then BtlSlotStatusAnimationSet(t) re-poses the target -- on every path but the two
   early returns, whether or not a status was stored. No RNG draw and no
   frame wait here or in BtlTargetTraitCheck / BtlSlotStatusAnimationSet.

   Shape: the four clears are ONE chained assignment, innermost first
   (btl_init.c's battle reset writes the same chain): all four index
   reads are expanded before the first store, so g_BtlTargetSlot is loaded once
   after the status store; four separate statements reload it after every
   byte store (117/139). The case bodies are in the target's address order
   (1, 0x80, 4, 8, 0x10 falling into 0x20, 0x30, 0x40). The `j` past the
   reload on the first default leaf is reorg dropping a redundant load. */
/* func_80034544 */
void BtlTargetStatusSet(s32 status)
{
    if (g_BtlTargetSlot >= 6 && g_BtlSlotIds[g_BtlTargetSlot] >= 0x6E) {
        return;
    }
    if (g_BtlSlotAmountKinds[g_BtlTargetSlot] == 6) {
        return;
    }
    switch (status & 0xFF) {
    case 1:
        BtlTargetStatus1Set();
        break;
    case 0x80:
        BtlTargetStatus80Set();
        break;
    case 4:
        BtlTargetStatus4Set();
        break;
    case 8:
        BtlTargetStatus8Set();
        break;
    case 0x10:
        if (*(u8 *)(g_BtlTargetSlot * 0x3C + CharStats + 0x15) & 0x20) {
            break;
        }
    case 0x20:
        if ((BtlTargetTraitCheck(0x800, 0x25, 0x26, 0) & 0xFF) != 1) {
            *(u8 *)(g_BtlTargetSlot * 0x3C + CharStats + 0x15) = status;
            g_BtlSlotStatus4Rounds[g_BtlTargetSlot] = g_BtlSlotStatus8Rounds[g_BtlTargetSlot] = g_BtlSlotStatus40Rounds[g_BtlTargetSlot] = g_BtlSlotStatus80Rounds[g_BtlTargetSlot] = 0;
        }
        break;
    case 0x30:
        BtlTargetStatus30Set();
        break;
    case 0x40:
        BtlTargetStatus40Set();
        break;
    }
    BtlSlotStatusAnimationSet(g_BtlTargetSlot);
}
/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot g_BtlSlotAmounts g_BtlSlotAmountKinds */

/* Effect 82 (the effect table's entry at 0x800B303C), a drain: the caster
   (g_BtlActingSlot) gains what the magic formula took from the target
   (g_BtlTargetSlot).

     BtlStat0EHitAmountSet()                    the magic formula on the target; its
                                        damage tail BtlTargetHpAmountTake leaves the
                                        amount in the target's entry of the
                                        u16 list g_BtlSlotAmounts
     g_BtlSlotAmounts[caster] = g_BtlSlotAmounts[target]
     caster hp (+0 of its 0x3C record in CharStats) += that amount, a u16
                                        add
     hp > maxhp (+2), strictly: the overshoot hp - maxhp comes off
                                        g_BtlSlotAmounts[caster] and hp = maxhp
     g_BtlSlotAmountKinds[caster] = 5             the heal reaction, so the number the
                                        caster shows is the gain after the
                                        clamp
     BtlShowNumberRun(g_BtlActionShowNumber)          the presentation
     BtlTargetStatus8Clear()                    wake on hit

   No hit check (the effect's param1 is not read), no KO test on either
   side and no look at the target's reaction g_BtlSlotAmountKinds[target]: the caster
   gains the stored amount whatever the formula's tail decided. No RNG draw
   and no frame wait of its own: the draws are BtlStat0EHitAmountSet's and the
   frames BtlShowNumberRun's. The target's own entry of g_BtlSlotAmounts is left as
   the tail wrote it.

   Shape: nothing but one operand order. The record address is written
   product first, g_BtlActingSlot * 0x3C + CharStats, for the target's
   `addu a0,v0,v1`; CharStats + g_BtlActingSlot * 0x3C gives the operands the
   other way round (52/53). The amount is read back from the caster's entry
   in `hp +=` (cse forwards the stored value; reading the target's entry
   again there is 27/53), and the clamp re-reads hp and maxhp through the
   record like BtlPartyHpShareAddRun's. */
/* func_80034770 */
void BtlTargetHpDrainRun(void)
{
    BtlStats *rec;

    BtlStat0EHitAmountSet();
    ((HealAmounts *)g_BtlSlotAmounts)->v[g_BtlActingSlot] = ((HealAmounts *)g_BtlSlotAmounts)->v[g_BtlTargetSlot];
    rec = (BtlStats *)(g_BtlActingSlot * 0x3C + CharStats);
    rec->hp += ((HealAmounts *)g_BtlSlotAmounts)->v[g_BtlActingSlot];
    if (rec->hp > rec->hp_max) {
        ((HealAmounts *)g_BtlSlotAmounts)->v[g_BtlActingSlot] -= rec->hp - rec->hp_max;
        rec->hp = rec->hp_max;
    }
    g_BtlSlotAmountKinds[g_BtlActingSlot] = 5;
    BtlShowNumberRun(g_BtlActionShowNumber);
    BtlTargetStatus8Clear();
}

/* func_80034844 */
void BtlActingRemoveRun(void) {
    BtlShowNumberRun(g_BtlActionShowNumber);
    *(g_BtlActingSlot + g_BtlSlotIds) = 0;
    *(g_BtlActingSlot + g_BtlSlotActiveFlags) = 0;
}
/* far_syms: g_BtlTargetSlot g_BtlActionNumber g_BtlActingSlot g_BtlRecords CharStats */

/* Effect 84 (the effect table's entry at 0x800B3044): the marked tile of
   enemy 0x4F (battle.md, BtlEnemy4FPlaceMark). g_BtlTargetSlot and g_BtlActionNumber are not
   a target slot here but the marked tile's row and column.

   Every active party slot i = 0..4 (g_BtlSlotActiveFlags[i] != 0) is tested with its
   0x40 battle object, row i of g_BtlRecords:
     - its y (+0xA, s16) must be in row * 8 - 8 .. row * 8, both ends
       included;
     - its near edge toward the caster (g_BtlActingSlot) must be in
       column * 8 - 0x20 .. column * 8 + 0x20, both ends included: x (+6)
       plus half its width (+0x3C, u16, >> 1) when it stands left of the
       caster (its x < the caster's x, strictly), x minus half its width
       otherwise. The edge is kept in 16 bits.
   A slot inside both gets the hit reaction g_BtlSlotAmountKinds[i] = 1 and the KO
   flag g_BtlSlotFallMarks[i] = 1, and its hp (+0 of its 0x3C record in CharStats)
   becomes 0. No hit check, no damage figure and no look at the slot's
   status or at the caster's slot. Then BtlShowNumberRun(g_BtlActionShowNumber), the
   presentation, whether or not anyone was caught.

   No RNG draw and no frame wait of its own; the frames are
   BtlShowNumberRun's.

   Shape:
   - The four bounds are s16 locals set before the loop. As s32 locals the
     constant 1 moves out of the loop into a saved register and the upper
     y bound becomes a register copy of the lower one's shift (61/86).
     Written inside the loop on the two tile bytes held in locals the
     bounds are hoisted, but the two y bounds share one shift (74/86).
   - The two flags are ONE chained assignment, as BtlTargetStatusSet's four
     clears are; as two statements the 1 is hoisted (63/86).
   - The edge is an s16 local; as an s32 the target's second, unsigned
     read of x goes (67/86).
   - The object's address is integer arithmetic on the s32 table base,
     index first; (u8 *)g_BtlRecords + (i << 6) swaps one addu's operands
     (85/86). */
/* func_80034898 */
void BtlPartyPlaceFallRun(void)
{
    u8 i;
    s16 ylo;
    s16 yhi;
    s16 xlo;
    s16 xhi;
    u8 *obj;
    s16 x;

    ylo = g_BtlTargetSlot * 8 - 8;
    yhi = g_BtlTargetSlot * 8;
    xlo = g_BtlActionNumber * 8 - 0x20;
    xhi = g_BtlActionNumber * 8 + 0x20;
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0) {
            obj = (u8 *)((i << 6) + g_BtlRecords);
            if (((BtlRecord *)obj)->sprite.y >= ylo && ((BtlRecord *)obj)->sprite.y <= yhi) {
                if (((BtlRecord *)obj)->sprite.x < *(s16 *)((g_BtlActingSlot << 6) + g_BtlRecords + 6)) {
                    x = ((BtlRecord *)obj)->sprite.x + (((BtlRecord *)obj)->width >> 1);
                } else {
                    x = ((BtlRecord *)obj)->sprite.x - (((BtlRecord *)obj)->width >> 1);
                }
                if (x >= xlo && x <= xhi) {
                    g_BtlSlotFallMarks[i] = g_BtlSlotAmountKinds[i] = 1;
                    ((BtlStats *)(i * 0x3C + CharStats))->hp = 0;
                }
            }
        }
    }
    BtlShowNumberRun(g_BtlActionShowNumber);
}

/* func_800349F0 */
void BtlTargetRemoveRun(void)
{
  u8 *amount_kind;

  BtlStatusTakeCheck();
  amount_kind = g_BtlTargetSlot + g_BtlSlotAmountKinds;
  if ((*amount_kind) == 7)
  {
    *amount_kind = 1;
    BtlShowNumberRun(g_BtlActionShowNumber);
    *(g_BtlTargetSlot + g_BtlSlotIds) = (*(g_BtlTargetSlot + g_BtlSlotActiveFlags) = 0);
    *(s16 *)(CharStats + g_BtlTargetSlot * 60) = 0;
  }
  else
  {
    BtlShowNumberRun(g_BtlActionShowNumber);
  }
}
/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */

/* func_80034AA4 */
void BtlTargetHold86Run(void)
{
  u8 *var;

  BtlShowNumberRun(g_BtlActionShowNumber);
  *(g_BtlTargetSlot + g_BtlPartyNotHeldFlags) = 0;
  var = g_BtlTargetSlot + g_BtlSlotIds;
  switch (*var)
  {
    case 1:
    case 2:
    case 6:
      *(g_BtlTargetSlot + g_BtlSpecialBytes) = 4;
      break;
    case 7:
      *(g_BtlTargetSlot + g_BtlSpecialBytes) = 5;
      break;
    default:
      break;
  }
  if (*(g_BtlTargetSlot + g_BtlSlotPlannedKinds) != 0)
    *(g_BtlTargetSlot + g_BtlSlotPlannedKinds) = 0xB;
}
/* func_80034B8C */
void BtlTargetHpFixedRun(void) {
    u16 r;
    switch (g_BtlActionNumber - 0x14) {
        case 16:
            g_BtlActionPower = 0x19;
            g_BtlActionBitNumber = 2;
            break;
        case 15:
            g_BtlActionPower = 0x19;
            g_BtlActionBitNumber = 3;
            break;
        case 18:
            g_BtlActionPower = 0x23;
            g_BtlActionBitNumber = 3;
            break;
        case 0:
            g_BtlActionPower = 0x23;
            g_BtlActionBitNumber = 3;
            break;
        case 19:
            g_BtlActionPower = 0x23;
            g_BtlActionBitNumber = 2;
            break;
        case 1:
            g_BtlActionPower = 0x28;
            g_BtlActionBitNumber = 4;
            break;
    }
    r = rng_mask_below(0xF, 0xA);
    BtlTargetHpAmountTake(g_BtlActionPower + r);
    BtlShowNumberRun(g_BtlActionShowNumber);
    BtlTargetStatus8Clear();
    if (*(g_BtlSlotFallMarks + g_BtlTargetSlot) != 0) {
        BtlFallRun();
        return;
    }
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), g_BtlSlotRestAnimations[g_BtlTargetSlot]);
}

/* func_80034CBC */
void BtlTargetStatus8ByLevelRun(void) {
    u8 idx;
    s32 base;
    s32 rate;

    idx = g_BtlTargetSlot;
    base = CharStats + (idx * 0x3C);
    *(idx + g_BtlSlotAmountKinds) = 2;
    if (!(((BtlStats *)base)->status & 0x35)) {

        switch (BtlTargetTraitCheck(0x400, 0x21, 0x26, 0) & 0xFF) {
            case 1:
                *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
                break;
            case 2:
                *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 7;
                break;
            default:
                if (*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x14) < 0x11) {
                    rate = 0x60;
                } else if (*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x14) < 0x16) {
                    rate = 0x32;
                } else if (*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x14) < 0x1B) {
                    rate = 0x18;
                } else {
                    rate = 0xC;
                }
                if ((rng_7f_draw() & 0xFF) < rate) {
                    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 7;
                }
                break;
        }
    }

    BtlShowNumberRun(g_BtlActionShowNumber);
    if (*(g_BtlTargetSlot + g_BtlSlotAmountKinds) == 7) {
        BtlTargetStatus8Set();
        BtlSlotStatusAnimationSet(g_BtlTargetSlot);
    }
}

/* func_80034E54 */
void BtlActingAreaStat0EHitStatus4Run(void)
{
  u8 next_slot;
  if (BtlTargetReachCheck() & 0xFF)
  {
    g_BtlWalkBlock.walk = 1;
    g_BtlWalkBlock.animation = 1;
    BtlWalkRun();
    BtlTargetAreaMark(0);
    BtlAreaStat0EHitRun();
    g_BtlTargetSlot = 0;
    do
    {
      if (((*(g_BtlTargetSlot + g_BtlSlotAmountKinds)) != 0) && ((*(g_BtlTargetSlot + g_BtlSlotActiveFlags)) != 0))
      {
        BtlTargetStatus4Try();
        BtlSlotStatusAnimationSet(g_BtlTargetSlot);
      }
      next_slot = 1 + g_BtlTargetSlot;
      g_BtlTargetSlot = next_slot;
    }
    while ((next_slot & 0xFF) < 2U);
  }
}
/* func_80034F34 */
void BtlActingAreaStatus4TryRun(void)
{
  int walk_animation;
  BtlWalkBlock *walk_block;
  if (BtlTargetReachCheck() & 0xFF)
  {
    walk_block = &g_BtlWalkBlock;
    walk_block->walk = 1;
    walk_block->animation = (walk_animation = 1);
    BtlWalkRun();
    BtlTargetAreaMark(0);
    BtlMarkedStatus4TryRun();
  }
}

/* func_80034F84 */
void BtlTargetStatus8Run(void)
{
  u8 *ptr;
  ptr = (u8 *) (CharStats + (g_BtlTargetSlot * 0x3C));
  *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
  if (!(ptr[0x15] & 0x4))
  {
    if ((BtlTargetTraitCheck(0x400, 0x21, 0x26, 0) & 0xFF) != 1)
    {
      ptr[0x15] = (ptr[0x15] & 0xBE) | 0x8;
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 7;
    }
  }
  BtlShowNumberRun(g_BtlActionShowNumber);
  BtlSlotStatusAnimationSet(g_BtlTargetSlot);
}
/* func_80035054 */
void BtlTargetHold92Run(void)
{
  u8 id;
  BtlShowNumberRun(g_BtlActionShowNumber);
  *(g_BtlTargetSlot + g_BtlPartyNotHeldFlags) = 0;
  id = *(g_BtlTargetSlot + g_BtlSlotIds);
  switch (id)
  {
    case 11:
      *(g_BtlTargetSlot + g_BtlSpecialBytes) = 3;
      break;
    case 5:
      *(g_BtlTargetSlot + g_BtlSpecialBytes) = 2;
      break;
    case 2:
    case 3:
      *(g_BtlTargetSlot + g_BtlSpecialBytes) = 4;
      break;
    case 4:
      *(g_BtlTargetSlot + g_BtlSpecialBytes) = 5;
      break;
  }
  *(g_BtlTargetSlot + g_BtlSlotPlannedKinds) = 0xB;
}

/* func_80035164 */
s32 BtlActingRowsHpRun(void)
{
  u16 acting_y;
  s16 min_y;
  s16 max_y;
  s16 val;
  s32 i;
  s32 shift;
  s32 shift8;
  void *ptr;
  i = 0;
  ptr = (g_BtlActingSlot << 6) + g_BtlRecords;
  shift = g_BtlAreaHalfHeight << 3;
  acting_y = AS(u16, ((BtlRecord *)ptr)->sprite.y);
  shift8 = shift + 8;
  min_y = (s16) (acting_y - shift8);
  max_y = (s16) (acting_y + shift);
  do
  {
    if (g_BtlSlotActiveFlags[i & 0xFF])
    {
      val = *((s16 *) (((u8 *) (((i & 0xFF) << 6) + g_BtlRecords)) + 0xA));
      if ((val >= min_y) && (val <= max_y))
      {
        g_BtlSlotAmountKinds[i & 0xFF] = 1;
      }
    }
    i++;
  }
  while (((u32) (((char) i) & 0xFF)) < 4U);
  return BtlMarkedStat0EHitRun();
}

void BtlTargetRowsMark();                                  /* extern */

/* func_80035238 */
void BtlTargetRowsHpRun(void) {
    BtlTargetRowsMark();
    BtlMarkedHitRun();
}
/* far_syms: g_BtlActingSlot CharStats g_BtlTargetSlot g_BtlRecords */

/* func_80035260 */
void BtlTargetRowsMark(void)
{
  u8 who;
  s32 i;
  u8 end;
  u8 n;
  s32 rec;
  u16 span;
  u16 y;
  s16 lo;
  s16 hi;

  who = g_BtlActingSlot;
  if (*((u8 *) ((who * 0x3C) + CharStats + 0x15)) & 0x40)
  {
    i = 0;
    end = 0xE;
  }
  else if (who < 5)
  {
    i = 6;
    end = 0xE;
  }
  else
  {
    i = 0;
    end = 5;
  }
  rec = (g_BtlTargetSlot << 6) + g_BtlRecords;
  span = g_BtlAreaHalfHeight * 8;
  y = AS(u16, ((BtlRecord *)rec)->sprite.y);
  lo = y - (span + 8);
  hi = y + span;
  for (n = (u8) i; n < end; n = (u8) ++i)
  {
    if (g_BtlSlotActiveFlags[n] != 0)
    {
      if ((*((s16 *) (((n << 6) + g_BtlRecords) + 0xA)) >= lo) && (hi >= *((s16 *) (((n << 6) + g_BtlRecords) + 0xA))))
      {
        g_BtlSlotAmountKinds[n] = 1;
      }
    }
  }
}

/* func_80035378 */
s32 BtlStatusTakeCheck()
{
    u16 trait_mask;
    u8 first_trait;
    u8 second_trait;
    u8 answer;
    s16 rate;
    u16 target_stat0e;

    *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 0;

    if (g_BtlTargetSlot >= 6 && g_BtlNumber >= 0xEB)
    {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
        return 0;
    }

    if (g_BtlTargetSlot < 5 && (*(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1E) & 0x2) != 0)
    {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 6;
        *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1E) &= 0xFD;
        return 0;
    }

    trait_mask = 0;
    switch (g_BtlActionBitNumber - 9)
    {
        case 0:
            trait_mask = 0x100;
            first_trait = 0x1E;
            second_trait = 0x21;
            break;
        case 1:
            trait_mask = 0x200;
            first_trait = 0x1F;
            second_trait = 0x21;
            break;
        case 2:
            trait_mask = 0x400;
            first_trait = 0x20;
            second_trait = 0x21;
            break;
        case 3:
            trait_mask = 0x800;
            first_trait = 0x22;
            second_trait = 0x25;
            break;
        case 4:
            trait_mask = 0x1000;
            first_trait = 0x23;
            second_trait = 0x25;
            break;
        case 5:
            trait_mask = 0x2000;
            first_trait = 0x24;
            second_trait = 0x25;
            break;
    }

    if (trait_mask != 0)
    {
        answer = BtlTargetTraitCheck(trait_mask, first_trait, second_trait, 0x26) & 0xFF;
        switch (answer)
        {
            case 1:
                *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
                return 0;
            case 2:
                *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 7;
                return 1;
        }
    }

    rate = *(u16 *)(CharStats + g_BtlActingSlot * 0x3C + 0xE) + g_BtlActionPower;

    if (g_BtlTargetSlot < 5)
    {
        target_stat0e = *(u16 *)(CharStats + g_BtlTargetSlot * 0x3C + 0xE) +
             *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1F);
    }
    else
    {
        target_stat0e = *(u16 *)(CharStats + g_BtlTargetSlot * 0x3C + 0xE);
    }

    rate = rate - target_stat0e;

    if (rate >= 0x80)
    {
        rate = 0x80;
    }
    else if (rate < 5)
    {
        rate = 5;
    }

    if ((rng_7f_draw() & 0xFF) < rate)
    {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 7;
        return 1;
    }
    else
    {
        *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 2;
        return 0;
    }
}
/* func_8003568C */
void BtlTargetFrontMark(s32 side)
{
  s32 rec;
  s32 p;
  s32 cur;
  s32 record7_y;
  s32 i;
  s32 end;
  u8 id;
  s16 edge;
  s16 lo;
  s16 hi;
  u16 y;
  u16 span;
  u32 n;
  s32 other;

  if (g_BtlAreaHalfWidth != 0)
  {
    id = g_BtlSlotIds[g_BtlActingSlot];
    if (id == 0x7D)
    {
      SoundPlay(0x68, 3);
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 6);
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      BtlWalkEdgeRun(1);
      SoundPlay(0x68, 3);
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 7);
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }
    else if (id == 0x7C)
    {
      SoundPlay(0x68, 3);
      BtlAnimationSet(g_BtlRecords + 0x1C0, 0x15);
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 8);
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      BtlWalkEdgeRun(1);
      p = (g_BtlActingSlot << 6) + g_BtlRecords;
      ((BtlRecord *)g_BtlRecords)[7].sprite.x = AS(u16, ((BtlRecord *)p)->sprite.x);
      record7_y = AS(u16, ((BtlRecord *)p)->sprite.y);
      ((BtlRecord *)g_BtlRecords)[7].sprite.y = record7_y;
      ((BtlRecord *)g_BtlRecords)[7].sprite.depth = record7_y;
      SoundPlay(0x68, 3);
      BtlAnimationSet(g_BtlRecords + 0x1C0, 0x16);
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      BtlAnimationSet(g_BtlRecords + 0x1C0, 0xB);
    }
    else if (id == 0x74)
    {
      BtlWalkEdgeRun(0xD);
    }
    else
    {
      BtlWalkEdgeRun(1);
    }
  }
  BtlFacingBothSet();
  rec = (g_BtlActingSlot << 6) + g_BtlRecords;
  if (((BtlRecord *)rec)->sprite.x < *((s16 *) (((g_BtlTargetSlot << 6) + g_BtlRecords) + 6)))
  {
    ((BtlRecord *)rec)->sprite.attr = 0x20;
  }
  else
  {
    ((BtlRecord *)rec)->sprite.attr = 0x2020;
  }
  switch ((u8) side)
  {
    case 0:
      i = 0;
      end = 5;
      break;
    case 1:
      i = 6;
      end = 0xE;
      break;
    case 2:
      i = 0;
      end = 0xE;
      break;
  }
  cur = (g_BtlActingSlot << 6) + g_BtlRecords;
  span = g_BtlAreaHalfHeight * 8;
  y = AS(u16, ((BtlRecord *)cur)->sprite.y);
  lo = y - (span + 8);
  hi = y + span;
  if (((BtlRecord *)cur)->sprite.attr & 0x2000)
  {
    edge = AS(u16, ((BtlRecord *)cur)->sprite.x) - (((BtlRecord *)cur)->width >> 1);
    for (n = (u8) i; n < (u8) end; n = (u8) ++i)
    {
      if ((n != g_BtlActingSlot) && (g_BtlSlotActiveFlags[n] != 0))
      {
        other = (n << 6) + g_BtlRecords;
        if ((s16) (AS(u16, ((BtlRecord *)other)->sprite.x) - (((BtlRecord *)other)->width >> 1)) < edge)
        {
          if ((((BtlRecord *)other)->sprite.y >= lo) && (hi >= ((BtlRecord *)other)->sprite.y))
          {
            g_BtlSlotAmountKinds[n] = 1;
          }
        }
      }
    }
  }
  else
  {
    edge = AS(u16, ((BtlRecord *)cur)->sprite.x) + (((BtlRecord *)cur)->width >> 1);
    for (n = (u8) i; n < (u8) end; n = (u8) ++i)
    {
      if ((n != g_BtlActingSlot) && (g_BtlSlotActiveFlags[n] != 0))
      {
        other = (n << 6) + g_BtlRecords;
        if (edge < (s16) (AS(u16, ((BtlRecord *)other)->sprite.x) + (((BtlRecord *)other)->width >> 1)))
        {
          if ((((BtlRecord *)other)->sprite.y >= lo) && (hi >= ((BtlRecord *)other)->sprite.y))
          {
            g_BtlSlotAmountKinds[n] = 1;
          }
        }
      }
    }
  }
}
void BtlTargetAreaCellsMark(s16 count, s16 rows, s16 col, s16 row);  /* TU defines this below */

/* func_80035B50 */
void BtlTargetAreaMark(u8 round_target)
{
  u8 who;
  u8 i;
  u8 end;
  s16 hi;
  s16 lo;
  s16 n;
  u8 *p;
  u8 *q;
  s32 grid;
  u8 id;
  s16 w;
  s16 h;
  s16 col;
  s16 row;

  if (round_target)
  {
    who = g_BtlTargetSlot;
  }
  else
  {
    who = g_BtlActingSlot;
  }
  i = 0;
  end = 5;
  if (g_BtlActingStatus & 0x40)
  {
    if (g_BtlActingSlot >= 6)
    {
      i = 6;
      end = 14;
    }
  }
  else if (g_BtlActingSlot < 5)
  {
    i = 6;
    end = 14;
  }
  grid = g_LoadBuffer;
  for (lo = 0, hi = 0x230; lo < 0x230; lo++, hi++)
  {
    *((u8 *) (grid + hi)) = 0;
    *((u8 *) (grid + lo)) = 0;
  }
  for (; i < end; i++)
  {
    if ((g_BtlSlotActiveFlags[i] != 0) && (i != g_BtlActingSlot))
    {
      p = (u8 *) ((i << 6) + g_BtlRecords);
      lo = ((BtlRecord *)p)->sprite.y / 8;
      hi = lo = (((BtlRecord *)p)->sprite.x / 8) + (lo * 40);
      id = i + 1;
      for (n = ((BtlRecord *)p)->width >> 4; n != 0; n--)
      {
        *((u8 *) (grid + lo)) = id;
        *((u8 *) (grid + hi)) = id;
        hi++;
        lo--;
      }
    }
  }
  q = (u8 *) ((who << 6) + g_BtlRecords);
  w = (g_BtlAreaHalfWidth * 2) + 1;
  h = (g_BtlAreaHalfHeight * 2) + 1;
  col = (((BtlRecord *)q)->sprite.x / 8) - g_BtlAreaHalfWidth;
  row = (((BtlRecord *)q)->sprite.y / 8) - g_BtlAreaHalfHeight;
  BtlTargetAreaCellsMark(w, h, col, row);
  /* CODEGEN FORCING DEVICE, not recovered logic.
     `(who << 6) & 0x3F` is ALWAYS 0 (a shifted-by-6 value has its low six
     bits clear), so this `if` still depends on round_target alone — behaviour is
     unchanged.  It is here because the target keeps the record offset
     who << 6 in $s0 (and round_target in $s1, frame 0x20) ACROSS the
     BtlTargetAreaCellsMark call with no visible later use: the original read the
     offset in some post-call test that combine proved constant and deleted,
     after flow had already counted it as live across the call (the JP
     2.7.2 twin keeps the same $s0, so it is source, not compiler).  The
     original spelling is unknown — likely a debug/range check on the
     offset.  Replace this disjunct when that shape is found; removing it
     drops the match to 148/164. */
  if (round_target || ((who << 6) & 0x3F))
  {
    g_BtlSlotAmountKinds[g_BtlTargetSlot] = 3;
  }
  g_BtlSlotAmountKinds[g_BtlActingSlot] = 0;
}

/* func_80035DE0 */
void BtlTargetAreaCellsMark(s16 count, s16 rows, s16 col, s16 row)
{
  s16 n;
  s16 idx;
  u8 v;
  s32 grid;
  grid = g_LoadBuffer;
  if (col < 0)
  {
    /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
       the block runs once, so behaviour is the same without the `do` / `while (0)`, but
       then 9 of this function's 70 instruction lines differ. */
    do
    {
      count = count + col;
    }
    while (0);
    col = 0;
  }
  if ((col + count) >= 0x28)
  {
    count = 0x28 - col;
  }
  if (row < 0)
  {
    rows = rows + row;
    row = 0;
  }
  if ((row + rows) >= 0x1C)
  {
    rows = 0x1C - row;
  }
  if (rows != 0)
  {
    do
    {
      idx = col + (row * 40);
      n = count;
      if (n != 0)
      {
        do
        {
          v = *((u8 *) (grid + idx));
          if (v != 0)
          {
            g_BtlSlotAmountKinds[(u8) (v - 1)] = 1;
          }
          n--;
          idx++;
        }
        while (n != 0);
      }
      /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
         the block runs once, so behaviour is the same without the `do` / `while (0)`,
         but then 14 of this function's 70 instruction lines differ. */
      do
      {
        rows--;
      }
      while (0);
      row++;
    }
    while (rows != 0);
  }
}
/* The magic damage formula. Attacker = CharStats[g_BtlActingSlot], target =
   CharStats[g_BtlTargetSlot] (0x3C-byte battle records: +0x0E u16 magic,
   +0x10 u16 magic defence, +0x14 u8 level); g_BtlActionPower is the spell
   row's magnitude.
     roll = rng_mask_below(0x3FF, magic / 10)   one RNG draw & 0x3FF,
                                                reduced below magic / 10
                                                (no draw when magic < 20)
     v    = (magic + level * magnitude + roll - target's magic defence) / 2
   signed, halved toward zero, floored at 0, then handed to the magic
   damage tail BtlTargetHpAmountTake (element / status modifiers, 1..999 clamp,
   the store). The attacker's record is re-read after the roll; the
   target's defence is loaded before the sum is formed, as in the inline
   copy of this formula in BtlAreaStat0EHitRun. Callers ignore the result (the
   TU's forward declarations say s32; nothing is returned). */
/* func_80035EF8 */
s32 BtlStat0EHitAmountSet(void)
{
    s32 r;
    u16 def;
    s32 v;

    r = rng_mask_below(0x3FF, (u16)(*(u16 *)(CharStats + g_BtlActingSlot * 0x3C + 0xE) / 10));
    def = *(u16 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x10);
    v = (*(u16 *)(CharStats + g_BtlActingSlot * 0x3C + 0xE)
         + *(u8 *)(CharStats + g_BtlActingSlot * 0x3C + 0x14) * g_BtlActionPower
         + (u16)r
         - def) / 2;
    if (v < 0) {
        v = 0;
    }
    BtlTargetHpAmountTake(v);
}

/* func_80035FD8 */
void BtlTargetHpAmountTake(u32 amount)
{
  u32 taken_amount;
  s32 result;

  taken_amount = amount;
  switch (g_BtlActionBitNumber)
  {
    case 1:
      result = BtlTargetTraitCheck(1, 0x2A, 0x2E, 0x30);
      break;
    case 2:
      result = BtlTargetTraitCheck(2, 0x2B, 0x2E, 0x30);
      break;
    case 3:
      result = BtlTargetTraitCheck(4, 0x2C, 0x2F, 0x30);
      break;
    case 4:
      result = BtlTargetTraitCheck(8, 0x2D, 0x2F, 0x30);
      break;
    default:
      result = 0;
      break;
  }
  switch (result & 0xFF)
  {
    case 1:
      taken_amount >>= 1;
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 4;
      break;
    case 2:
      taken_amount <<= 1;
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 3;
      break;
    default:
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 1;
      break;
  }
  if (BtlTargetTraitCheck(0, 0x32, 0, 0) & 0xFF)
    taken_amount -= taken_amount >> 2;
  if (g_BtlTargetSlot < 5 && g_BtlSlotRestAnimations[g_BtlTargetSlot] == 0x13)
    taken_amount >>= 1;
  if (taken_amount == 0)
    taken_amount = 1;
  if (taken_amount >= 0x3E8)
    taken_amount = 0x3E7;
  if (g_BtlTargetSlot < 5)
  {
    if (*(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1E) & 0x2)
    {
      *(g_BtlTargetSlot + g_BtlSlotAmountKinds) = 6;
      taken_amount = 0;
      *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1E) &= 0xFD;
    }
  }
  if ((BtlTargetTraitCheck(0, 0x33, 0, 0) & 0xFF) == 1)
    taken_amount = 0;
  if (g_BtlActionKind == 4 && (BtlTargetTraitCheck(0, 0x34, 0, 0) & 0xFF) == 1)
    taken_amount = 0;
  if (g_BtlNumber == 0xFA)
    taken_amount = 0;
  *(u16 *)(g_BtlSlotAmounts + g_BtlTargetSlot * 2) = taken_amount;
  BtlHitAmountTake();
}
/* far_syms: g_BtlRunning (target reaches it %hi/%lo) */

static inline int inline_fn(u8 end)
{
  return end & 0xFF;
}
/* func_80036294 */
int BtlTargetPartyMark(void)
{
  short party_end;
  s32 i;
  u8 end;
  u8 marked_slot;
  u32 loop_end;
  u16 slot;
  i = 0;
  end = 5;
  if (g_BtlRunning != 0)
  {
    slot = 0 & 0xFF;
    if ((*((u8 *) (((u8 *) ((g_BtlActingSlot * 0x3C) + CharStats)) + 0x15))) & 0x40)
    {
      i = 6;
      end = 0xE;
      slot = 6 & 0xFF;
    }
    slot = i & 0xFF;
    loop_end = inline_fn(end);
    if (slot < loop_end)
    {
      do
      {
        if ((*(slot + g_BtlSlotActiveFlags)) != 0)
        {
          marked_slot = slot;
          *(marked_slot + g_BtlSlotAmountKinds) = 1;
        }
        i += 1;
        slot = i & 0xFF;
      }
      while (slot < loop_end);
    }
    return;
  }
  marked_slot = 0 & 0xFF;
  party_end = 5U;
  do
  {
    marked_slot = i & 0xFF;
    if ((*(marked_slot + g_BtlSlotIds)) != 0)
    {
      *(marked_slot + g_BtlSlotAmountKinds) = 1;
    }
    i += 1;
  }
  while (((u32) (i & 0xFF)) < party_end);
}

/* far_syms: g_BtlActingStatus (target reaches it %hi/%lo) */
/* far_syms: g_BtlActingStatus (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_80036394 */
s32 BtlTargetOtherSideMark()
{
  u8 slot;
  u8 end;

  if (g_BtlActingStatus & 0x40)
  {
    slot = 0;
    end = 0xE;
  }
  else if (((u8) g_BtlActingSlot) < 5U)
  {
    slot = 6;
    end = 0xE;
  }
  else
  {
    slot = 0;
    end = 5;
  }

  while (slot < end)
  {
    if (g_BtlSlotActiveFlags[slot])
    {
      g_BtlSlotAmountKinds[slot] = 1;
    }
    slot++;
  }

  g_BtlSlotAmountKinds[g_BtlActingSlot] = 0;
}
/* far_syms: g_BtlSlotShowPlaceFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotShowPlaceXs (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotShowPlaceYs (target reaches it %hi/%lo) */
extern long HWD0, VWD0;
extern u8 g_BtlSlotShowPlaceFlags[];
extern u8 g_BtlSlotShowPlaceXs[];
extern u8 g_BtlSlotShowPlaceYs[];

/* func_80036448 */
void BtlTargetShowPlaceSet(void)
{
  u8 *show_place_x;
  s32 word_offset;
  void *record;
  if (g_BtlRunning != 0)
  {
    *(g_BtlTargetSlot + g_BtlSlotShowPlaceFlags) = 1;
    word_offset = g_BtlTargetSlot * 2;
    show_place_x = word_offset + g_BtlSlotShowPlaceXs;
    record = (g_BtlTargetSlot << 6) + g_BtlRecords;
    *((u16 *) show_place_x) = ((BtlRecord *)record)->sprite.x;
    *((u16 *) (word_offset + g_BtlSlotShowPlaceYs)) = ((BtlRecord *)record)->sprite.y;
  }
}
/* far_syms: g_BtlPartyFallenFlags (target reaches it %hi/%lo) */
extern u8 g_BtlPartyFallenFlags;

/* func_800364C0 */
void BtlActionOutcomeClear(void)
{
	s32 i;
	s32 party_slot;
	s32 other_slot;

	i = 0;
	do
	{
		party_slot = i & 0xFF;
		*(party_slot + g_BtlSlotShowPlaceFlags) = (*(party_slot + g_BtlSlotAmountKinds) = (*(party_slot + g_BtlSlotFallMarks) = (*(party_slot + (&g_BtlPartyFallenFlags)) = 0)));
		*(u16 *)((party_slot * 2) + g_BtlSlotShowPlaceXs) = (*(u16 *)((party_slot * 2) + g_BtlSlotShowPlaceYs) = (*(u16 *)((party_slot * 2) + g_BtlSlotAmounts) = 0));
		i += 1;
	} while (((u32)(i & 0xFF)) < 5U);

	other_slot = i & 0xFF;
	if (((u32)other_slot) < 0xEU)
	{
		do
		{
			i += 1;
			*(other_slot + g_BtlSlotShowPlaceFlags) = (*(other_slot + g_BtlSlotAmountKinds) = (*(other_slot + g_BtlSlotFallMarks) = 0));
			*(u16 *)((other_slot * 2) + g_BtlSlotShowPlaceXs) = (*(u16 *)((other_slot * 2) + g_BtlSlotShowPlaceYs) = (*(u16 *)((other_slot * 2) + g_BtlSlotAmounts) = 0));
			other_slot = i & 0xFF;
		} while (((u32)other_slot) < 0xEU);
	}
}
