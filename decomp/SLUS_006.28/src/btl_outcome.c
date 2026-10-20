/* Lunar SSSC — btl_outcome: an action's outcome shown: the acting slot's
   animation with its sound, what the action did to the target, the extra
   records' frame, the show's end, and the turn toward the party. The first
   split had it in rendering3.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlTargetSlot (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlActingSlot */
/* far_syms: g_BtlSlotActiveFlags */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
s32 SoundPlay(s32, s32);
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
void BtlAnimationStep();
extern u8 g_BtlExtraRecordCount;
extern s32 g_BtlExtraRecords;
void BtlExtraRecordsStep(void);  /* TU defines this below */
s32 BtlMarkedRestAnimationSet();
s32 BtlAmountShowWait();
s32 BtlActingRestAnimationSet();
s32 BtlFallRun();
void BtlShowEnd(void);  /* TU defines this below */
extern u8 g_BtlActingSlot;
s32 BtlActingAnimationRun(s32, s32, void *, s32);
void BtlTargetOutcomeShow();
extern s32 g_BtlRecords;
extern u8 g_BtlTargetSlot;

/* func_80041338 */
void BtlActingAnimationOutcomeRun(s32 animation, s32 wait_frames)
{
  s32 masked_animation;
  void *acting_record;
  void *end_record;
  acting_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  masked_animation = animation & 0xFFFF;
  if (((BtlRecord *)acting_record)->sprite.y <= (*((s16 *) (((u8 *) ((g_BtlTargetSlot << 6) + g_BtlRecords)) + 0xa))))
  {
    ((BtlRecord *)acting_record)->sprite.depth = (u16) (AS(s32, ((BtlRecord *)acting_record)->sprite.depth) + 1);
    BtlActingAnimationRun(animation & 0xFFFF, wait_frames & 0xFFFF, acting_record, animation);
    BtlTargetOutcomeShow();
    end_record = (g_BtlActingSlot << 6) + g_BtlRecords;
    ((BtlRecord *)end_record)->sprite.depth = (u16) (AS(s32, ((BtlRecord *)end_record)->sprite.depth) - 1);
    return;
  }
  BtlActingAnimationRun(masked_animation, wait_frames & 0xFFFF, acting_record, animation);
  BtlTargetOutcomeShow();
}
extern u8 g_BtlActionSound;
/* func_800413EC */
s32 BtlActingAnimationRun(s32 animation, s32 wait_frames, void *unused, s32 unused2) {
    s32 masked_wait_frames;

    BtlFacingBothSet();
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), animation & 0xFFFF);
    SoundPlay(g_BtlActionSound, 3);
    masked_wait_frames = wait_frames & 0xFFFF;
    if (masked_wait_frames != 0xFF) {
        BtlFrameWait(masked_wait_frames);
        return;
    }
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
}
/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotStrongMarks (target reaches it %hi/%lo) */
extern u8 g_BtlSlotAmountKinds[];
extern u8 g_BtlSlotStrongMarks[];
s32 BtlAnimationSet(s32, s32);
s32 BtlTargetBackMove();
s32 BtlAnimationEndWait(s32);
s32 BtlTargetForwardMove();
s32 BtlPartyHitAnimationSet();
void BtlTargetMissShow(void);
s32 BtlFrameStep();
s32 BtlAmountShowStart(s32);
s32 BtlAmountShowCheck();

/* func_80041484 */
void BtlTargetOutcomeShow(void)
{
  s32 sound;
  s32 last_step;
  s32 rest_pending;
  if ((*(g_BtlTargetSlot + g_BtlSlotAmountKinds)) == 2)
  {
    if (((u8) g_BtlTargetSlot) < 5U)
    {
      SoundPlay(0x24, 3);
      BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 0x11);
      BtlTargetBackMove();
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      BtlActingRestAnimationSet();
      BtlTargetForwardMove();
    }
    else
    {
      BtlTargetMissShow();
    }
  }
  else
  {
    sound = 0x23;
    if (g_BtlSlotStrongMarks != 0)
    {
      sound = 0x38;
    }
    SoundPlay(sound, 3);
    rest_pending = 1;
    BtlPartyHitAnimationSet();
    BtlAmountShowStart(0);
    last_step = (((*((s16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) + 0x3a))) & 0x7FFF) - 1) & 0xFFFF;
    loop_7:
    if (BtlAmountShowCheck() != 0)
    {
      BtlFrameStep();
      if ((rest_pending & 0xFF) && ((*((u16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlRecords)) + 0x38))) == last_step))
      {
        rest_pending = 0;
        BtlActingRestAnimationSet();
      }
      goto loop_7;
    }

    BtlFallRun();
    if (rest_pending & 0xFF)
    {
      BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
      BtlActingRestAnimationSet();
    }
  }
  BtlMarkedRestAnimationSet();
}

void BtlAnimationStep();                                   /* extern */
extern u8 g_BtlExtraRecordCount;
extern s32 g_BtlExtraRecords;

/* func_8004163C */
void BtlExtraRecordsStep(void) {
    if (g_BtlExtraRecordCount != 0) {
        BtlAnimationStep(g_BtlExtraRecords, 0x1C, g_BtlExtraRecordCount);
    }
}
s32 BtlMarkedRestAnimationSet();                                  /* extern */
s32 BtlAmountShowWait();                                  /* extern */
s32 BtlActingRestAnimationSet();                                  /* extern */
s32 BtlFallRun();                                  /* extern */

/* func_8004166C */
void BtlShowEnd(void) {
    BtlActingRestAnimationSet();
    BtlAmountShowWait();
    BtlFallRun();
    BtlMarkedRestAnimationSet();
}

/* A 0x40-byte battle record: a row of g_BtlRecords (and of the saved copy
   g_BtlPartyKeptRecords). Declared here because BtlFacingPartySet uses it. */
/* far_syms: g_BtlActingSlot g_BtlRecords */
extern u8 g_BtlSlotActiveFlags[];

/* The acting record (g_BtlActingSlot, a 0x40-byte row of g_BtlRecords) turns round
   when no active party slot stands on the side it faces. Called by two
   enemy-action handlers in btl_enemy.c before the action plays.

   With the mirror bit 0x2000 set in its attr word (+0xC): party slots
   0..4 are scanned, and an active one (g_BtlSlotActiveFlags[i] != 0) whose x (+6) is
   below the record's x ends the scan with nothing changed. When no active
   slot has a smaller x, attr = 0x20.
   With the bit clear: an active slot whose x is above the record's ends
   the scan; when there is none, attr = 0x2020.

   The store replaces the whole attr word (only 0x20 and the mirror bit
   remain) and re-reads g_BtlActingSlot and g_BtlRecords for its address. Reads
   g_BtlActingSlot, g_BtlRecords, g_BtlSlotActiveFlags[0..4] and the rows' x; writes the
   acting row's attr. No call, no frame wait, no RNG draw; it returns
   nothing.

   Shape: the acting record's x is the LEFT operand of both comparisons
   (`rec->x > row.x`, then `rec->x < row.x`), which is what puts its load
   ahead of the row's in both loops; the party rows are indexed off
   g_BtlRecords itself (the loop pass hoists that load, the record pointer's
   x read stays in the loop). */
/* func_800416A4 */
void BtlFacingPartySet(void)
{
    BtlRecord *rec;
    u8 i;

    rec = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    if (rec->sprite.attr & 0x2000) {
        for (i = 0; i < 5; i++) {
            if (g_BtlSlotActiveFlags[i] != 0 && rec->sprite.x > ((BtlRecord *)g_BtlRecords)[i].sprite.x) {
                break;
            }
        }
        if (i >= 5) {
            ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.attr = 0x20;
        }
    } else {
        for (i = 0; i < 5; i++) {
            if (g_BtlSlotActiveFlags[i] != 0 && rec->sprite.x < ((BtlRecord *)g_BtlRecords)[i].sprite.x) {
                break;
            }
        }
        if (i >= 5) {
            ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.attr = 0x2020;
        }
    }
}
