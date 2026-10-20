/* Lunar SSSC — btl_amount: the amount records, the sprites that show each
   slot's amount: their memory, start, set, off, run and step. The first
   split had it in misc.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "mem.h"
#include "btl.h"
#include "sprite.h"

s32 BtlFrameStep();                                  /* The party HUD's per-member record BtlPartyPanelDisp builds on the stack and
   BtlPartyPanelMemberDisp draws: a used flag, three status bytes (CharStats +0x1E & 4,
   +0x15, +0x14) and the hp / max hp / mp / max mp words (CharStats +0..+6). */
s32 BtlAmountRecordSet(s32);
void BtlAmountRecordOff(u32);                              /* extern */
extern s32 g_SpriteList;

s32 xmalloc_heap_free(s32);
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 g_BtlRecords;

s32 xmalloc_heap_free(s32);                               /* extern */
extern s32 g_BtlAmountRecordsRunning;
extern s32 g_BtlAmountPieces;
extern s32 g_BtlAmountTexelRecords;
extern s32 g_BtlAmountPoseRows;
extern s32 g_BtlAmountRecords;
/* One 0x24-byte sprite slot of the table at g_SpriteList (sprite_run_off
   clears a run of them by zeroing each slot's first u16). The popups own
   slots 0x72..0x7E (g_SpriteList + 0x1008), one per record, and each record
   carries the image of its slot at +0x04. */
/* A 0x40-byte damage-popup record (g_BtlAmountRecords[slot']): BtlAmountRecordsInit
   builds the 13 of them from a template, BtlAmountRecordStart fills one,
   BtlAnimationStart starts it and the tick BtlAmountRecordsStep runs it. +0x04..+0x27
   is the sprite slot image the tick copies out each frame (a struct member,
   so the copy is GCC's word-aligned movstrsi loop); +0x28 the anim table,
   +0x2C the step table ({y index, duration} u16 pairs), +0x34 the tick
   timer, +0x36 the current step's duration, +0x38 the step index, +0x3A
   the step count (bit 15 = HOLD, ignored by the tick), +0x3E a y bias.
   Struct fields, not raw offsets: GCC 2.8's alias rule lets the gp loads
   move past these stores as the target has them. One typedef serves all
   three functions: each field's width is the one every access to it uses
   (the template's two zero halfwords at +0x1C and +0x20 are spr.f18 and
   spr.f1C). */

/* func_8004A854 */
void BtlAmountRecordsFree(void) {
    g_BtlAmountRecordsRunning = 0;
    xmalloc_heap_free(g_BtlAmountPoseRows);
    xmalloc_heap_free(g_BtlAmountPieces);
    xmalloc_heap_free(g_BtlAmountTexelRecords);
    xmalloc_heap_free(g_BtlAmountRecords);
}

/* One frame of a popup's pose (0x18 bytes, all s16 - 2-aligned, so GCC
   block-copies it with lwl/lwr): +0x06 is the glyph descriptor index
   BtlAmountRecordStart writes, +0x08 the frame's x offset. */
/* A popup's digit block (g_BtlAmountPieces[slot'], 0x48 bytes): three frames. */
/* A 0xC-byte glyph descriptor (g_BtlAmountTexelRecords[21]). */
/* A popup's pose word (g_BtlAmountPoseRows[slot']). */
extern s32 g_BtlAmountStepList;
extern s32 g_BtlAmountAnimationTable;

/* Builds the damage-popup tables enemy setup (BtlEnemyLoad) has just
   allocated. A template record (off, sprite on, descriptor table
   g_BtlAmountTexelRecords, anim table g_BtlAmountAnimationTable, step table g_BtlAmountStepList) and a
   template 8x8 frame at x offset -3 are built on the stack. Each of the
   13 digit blocks gets the frame three times, at x offsets -11, -3, +5;
   each of the 13 records gets the template, its pose word {3, 0} and its
   digit block; the 21 glyph descriptors are {0, 10, 8, 8, 8 * i, 0}.
   Every table store is a struct-field store on the indexed global, which
   is what the bytes need: the pose word written through an s16 pointer
   made the build reload +0x14 and g_BtlAmountRecords, and a walking local
   pointer in the last loop grew a second induction pointer; written
   this way g_BtlAmountTexelRecords is loaded once, ahead of that loop. */
/* func_8004A8A0 */
void BtlAmountRecordsInit(void)
{
  BtlRecord rec;
  SpritePiece dig;
  u32 i;

  rec.on = 0;
  rec.sprite.on = 1;
  AS(s16, rec.sprite.attr) = 0;
  rec.sprite.texel_records = g_BtlAmountTexelRecords;
  rec.sprite.pose = 0;
  rec.sprite.scaled = 0;
  AS(s32, rec.sprite.scale_x) = 0;
  rec.animation_table = g_BtlAmountAnimationTable;
  rec.step_list = g_BtlAmountStepList;
  AS(s16, dig.kind) = 0;
  dig.width = 8;
  dig.height = 8;
  AS(s16, dig.texel_col.texel) = 0;
  dig.x0 = -3;
  dig.y0 = 0;
  dig.x1 = dig.y1 = dig.x2 = dig.y2 = dig.x3 = dig.y3 = 0;
  for (i = 0; i < 13; i++)
  {
    ((BtlAmountDigits *) g_BtlAmountPieces)[i].digit[0] = ((BtlAmountDigits *) g_BtlAmountPieces)[i].digit[1] = ((BtlAmountDigits *) g_BtlAmountPieces)[i].digit[2] = dig;
    ((BtlAmountDigits *) g_BtlAmountPieces)[i].digit[0].x0 -= 8;
    ((BtlAmountDigits *) g_BtlAmountPieces)[i].digit[2].x0 += 8;
  }
  for (i = 0; i < 13; i++)
  {
    ((BtlRecord *) g_BtlAmountRecords)[i] = rec;
    ((BtlRecord *) g_BtlAmountRecords)[i].sprite.pose_table = g_BtlAmountPoseRows + i * 4;
    ((BtlRecord *) g_BtlAmountRecords)[i].sprite.piece_table = g_BtlAmountPieces + i * 0x48;
    AS(s16, ((SpritePoseRow *) g_BtlAmountPoseRows)[i].count) = 3;
    AS(s16, ((SpritePoseRow *) g_BtlAmountPoseRows)[i].first) = 0;
  }
  for (i = 0; i < 21; i++)
  {
    AS(s16, ((SpriteTexelRecord *) g_BtlAmountTexelRecords)[i].unk_00) = 0;
    AS(s16, ((SpriteTexelRecord *) g_BtlAmountTexelRecords)[i].page) = 10;
    AS(s16, ((SpriteTexelRecord *) g_BtlAmountTexelRecords)[i].width) = 8;
    AS(s16, ((SpriteTexelRecord *) g_BtlAmountTexelRecords)[i].height) = 8;
    AS(s16, ((SpriteTexelRecord *) g_BtlAmountTexelRecords)[i].u) = i * 8;
    AS(s16, ((SpriteTexelRecord *) g_BtlAmountTexelRecords)[i].v) = 0;
  }
}

/* far_syms: g_BtlRecords */
extern u16 *g_BtlAmountYOffsets;
extern u16 g_BtlSlotAmounts[];
void BtlAnimationStart(s32, s32);

/* func_8004AB40 */
void BtlAmountRecordStart(u32 slot, u8 kind, u16 val)
{
  u32 idx;
  s16 base;
  s32 shown;
  u16 d;
  BtlRecord *rec;
  s32 dig;
  s32 obj;
  s16 y;
  shown = 0;
  if (slot != 5)
  {
    idx = slot;
    if (slot >= 6)
    {
      idx = slot - 1;
    }
    rec = (BtlRecord *) (g_BtlAmountRecords + (idx << 6));
    rec->sprite.on = 1;
    rec->on = 1;
    rec->timer = 0;
    rec->step = 0;
    AS(s16, rec->sprite.flags) = (val & 0xF0) | 0x4300;
    rec->sprite.pose_table = g_BtlAmountPoseRows + (idx * 4);
    rec->sprite.piece_table = g_BtlAmountPieces + (idx * 0x48);
    rec->animation_before = 3;
    rec->animation = 3;
    rec->step_length = 3;
    rec->end = 3;
    base = 1;
    if (val & 0xF)
    {
      base = 0xB;
    }
    val = g_BtlSlotAmounts[slot];
    dig = rec->sprite.piece_table;
    d = (u16) (val / 100) % 10;
    if (d != 0)
    {
      ((SpritePiece *)dig)->texel_col.texel = base + d;
      shown = 1;
    }
    else
    {
      ((SpritePiece *)dig)->texel_col.texel = 0;
    }
    d = (u16) (val / 10) % 10;
    if (d != 0)
    {
      ((SpritePiece *)dig)[1].texel_col.texel = base + d;
      shown = 1;
    }
    else if (shown != 0)
    {
      ((SpritePiece *)dig)[1].texel_col.texel = base;
    }
    else
    {
      ((SpritePiece *)dig)[1].texel_col.texel = 0;
    }
    if (shown != 0)
    {
      ((SpritePiece *)dig)[2].texel_col.texel = base + val % 10;
    }
    else
    {
      ((SpritePiece *)dig)[1].texel_col.texel = base + val % 10;
    }
    obj = (slot << 6) + g_BtlRecords;
    rec->width = ((BtlRecord *)obj)->width;
    rec->height = 0;
    AS(u16, rec->sprite.x) = AS(u16, ((BtlRecord *)obj)->sprite.x);
    y = AS(u16, ((BtlRecord *)obj)->sprite.y) + g_BtlAmountYOffsets[*(u16 *) (rec->step_list + rec->step * 4)] - rec->height;
    rec->animation_before = rec->animation;
    rec->animation = kind;
    rec->sprite.y = rec->sprite.depth = y;
    BtlAnimationStart((s32) rec, 1);
  }
}

/* func_8004ADA8 */
void BtlAmountRecordOff(u32 record_number)
{
  void *pieces;
  s16 *amount_record_then_texel_number;
  if (record_number < 0xEU)
  {
    sprite_run_off(record_number + 0x72, 1);
    amount_record_then_texel_number = (s16 *)(s32 *) ((record_number << 6) + g_BtlAmountRecords);
    *amount_record_then_texel_number = 0;
    pieces = (void *)g_BtlAmountPieces + (record_number * 0x48);
    amount_record_then_texel_number = (s16 *) (((u8 *) pieces) + 0x36);
    *amount_record_then_texel_number = 0;
    ((BtlAmountDigits *)pieces)->digit[1].texel_col.texel = 0;
    ((BtlAmountDigits *)pieces)->digit[0].texel_col.texel = 0;
  }
}

s32 BtlAmountRecordsSet();                                /* extern */
s32 BtlAnimationFramesCount(s32);                             /* extern */

/* func_8004AE0C */
void BtlAmountRecordsRun(void) {
    s32 longest_record;
    s32 frames_left;
    s32 record_number;

    longest_record = BtlAmountRecordsSet();
    if (longest_record != 0) {
        g_BtlAmountRecordsRunning = 1;
        frames_left = BtlAnimationFramesCount(longest_record);
        if (frames_left & 0xFFFF) {
            do {
                frames_left -= 1;
                BtlFrameStep();
            } while (frames_left & 0xFFFF);
        }
        g_BtlAmountRecordsRunning = 0;
    }
    record_number = 0;
    do {
        BtlAmountRecordOff(record_number & 0xFF);
        record_number += 1;
    } while ((u32) (record_number & 0xFF) < 0xDU);
}
/* far_syms: g_BtlSlotAmountKinds */
extern u8 g_BtlSlotAmountKinds[];

/* Starts, or retires, battle slot `slot`'s damage popup from the slot's
   outcome code g_BtlSlotAmountKinds[slot] (only the low 16 bits of `slot` are used).
   The number shown is BtlAmountRecordStart's business (g_BtlSlotAmounts[slot]); this
   function only picks the popup's anim and its style byte (high nibble =
   the sub-CLUT, low nibble non-zero = the second digit set):
     1 (a hit)       BtlAmountRecordStart(slot, 1, 0xE0)
     3 (weak)        BtlAmountRecordStart(slot, 1, 0xB0)
     4 (resisted)    BtlAmountRecordStart(slot, 1, 0xC4)
     5 (a heal)      BtlAmountRecordStart(slot, 0, 0xD0)
     8 (attack up)   BtlAmountRecordStart(slot, 0, 0xC0)
     9 (defence up)  BtlAmountRecordStart(slot, 0, 0xB4)
   Every other code (0, 2, 6, 7, 10 and up) retires the slot's record:
   BtlAmountRecordOff(record), record = slot below 6, slot - 1 from 6 up. Slot 5
   has no record of its own, so a slot 5 with any such code retires record
   5, which is slot 6's (BtlAmountRecordsSet passes 0..12 in order, so slot 6 is
   decided right after; BtlAmountShowStart never passes 5). With a popup code
   slot 5 does nothing (BtlAmountRecordStart returns at once for it).
   No frame wait and no RNG draw: BtlAmountRecordStart reaches only the leaf
   BtlAnimationStart, BtlAmountRecordOff only the leaf sprite_run_off.
   Returns nothing; it is declared s32 because the TU's two other
   declarations and BtlAmountShowStart's are `s32 BtlAmountRecordSet(s32)`. */
/* func_8004AE8C */
s32 BtlAmountRecordSet(s32 slot)
{
  switch (g_BtlSlotAmountKinds[slot & 0xFFFF])
  {
    case 1:
      BtlAmountRecordStart(slot & 0xFFFF, 1, 0xE0);
      break;
    case 3:
      BtlAmountRecordStart(slot & 0xFFFF, 1, 0xB0);
      break;
    case 4:
      BtlAmountRecordStart(slot & 0xFFFF, 1, 0xC4);
      break;
    case 5:
      BtlAmountRecordStart(slot & 0xFFFF, 0, 0xD0);
      break;
    case 8:
      BtlAmountRecordStart(slot & 0xFFFF, 0, 0xC0);
      break;
    case 9:
      BtlAmountRecordStart(slot & 0xFFFF, 0, 0xB4);
      break;
    default:
      if ((u32) (slot & 0xFFFF) >= 6)
      {
        slot--;
      }
      BtlAmountRecordOff(slot & 0xFFFF);
      break;
  }
}

INCLUDE_RODATA("asm/data/jtbl", D_800124AC);
INCLUDE_RODATA("asm/data/jtbl", g_BtlWinRewardMessage);
INCLUDE_RODATA("asm/data/jtbl", g_BtlLevelUpGainsMessage);

/* Ticks the 13 damage-popup records every battle frame while
   g_BtlAmountRecordsRunning is set: clears their sprite slots (0x72, 13), then for each
   live record (party/enemy slot i, slot 5 has no record) advances the
   timer; at the step's duration it moves to the next step and re-places
   y = the actor's +0x0A + g_BtlAmountYOffsets[step's y index] - bias, or retires
   the record (BtlAmountRecordOff) past the last step. A live record's sprite
   image is copied into its slot with +0x18 cleared. */
/* func_8004AF94 */
void BtlAmountRecordsStep(void)
{
  SpriteEntry *spr;
  BtlRecord *rec;
  u8 i;
  u8 j;
  s16 y;

  spr = (SpriteEntry *) (g_SpriteList + 0x1008);
  if (g_BtlAmountRecordsRunning != 0)
  {
    sprite_run_off(0x72, 0xD);
    rec = (BtlRecord *) g_BtlAmountRecords;
    i = 0;
    j = 0;
    do
    {
      if (i != 5)
      {
        if (rec->on != 0)
        {
          if (++rec->timer >= rec->step_length)
          {
            if (++rec->step >= (rec->end & 0x7FFF))
            {
              BtlAmountRecordOff(j);
              j++;
              goto next;
            }
            y = *(u16 *) ((i << 6) + g_BtlRecords + 0xA) + g_BtlAmountYOffsets[*(u16 *) (rec->step_list + rec->step * 4)] - rec->height;
            rec->timer = 0;
            rec->sprite.y = rec->sprite.depth = y;
            rec->step_length = *(u16 *) (rec->step_list + rec->step * 4 + 2);
          }
          spr[j] = rec->sprite;
          spr[j].pose = 0;
        }
        j++;
      next:
        rec++;
      }
      i++;
    } while (j < 13);
  }
}

/* Ruling of 2026-10-14 (docs/naming.md): BtlAnimationStart with the first step's sign changed; nothing reaches it, and nothing read says what that is for. */
void func_8004B164(void *record, s32 restart) {
    u16 first_step;

    first_step = -(*(s16 *)((u8 *)((((BtlRecord *)record)->animation * 4) + ((BtlRecord *)record)->animation_table) + 0x2));
    ((BtlRecord *)record)->step = first_step;
    ((BtlRecord *)record)->end = (s16) (first_step + (*(s16 *)((((BtlRecord *)record)->animation * 4) + ((BtlRecord *)record)->animation_table)));
    if (restart != 0) {
        ((BtlRecord *)record)->timer = 0;
        ((BtlRecord *)record)->step_length = (u16) (*(s16 *)((u8 *)((((BtlRecord *)record)->step * 4) + ((BtlRecord *)record)->step_list) + 0x2));
    }
}

/* func_8004B1D0 */
s32 BtlAmountRecordsSet(void)
{
  s32 record_offset;
  s32 frame_count;
  s32 slot;
  s32 record_number;
  s32 longest_frame_count;
  s32 longest_record;
  s32 masked_record_number;
  u16 *amount_record;
  slot = 0;
  do
  {
    BtlAmountRecordSet(slot & 0xFFFF);
    slot += 1;
  }
  while (((u32) (slot & 0xFFFF)) < 0xDU);
  longest_record = 0;
  record_number = 0;
  longest_frame_count = 0;
  masked_record_number = 0 & 0xFFFF;
  do
  {
    masked_record_number = record_number & 0xFFFF;
    record_offset = masked_record_number << 6;
    amount_record = record_offset + g_BtlAmountRecords;
    if ((*amount_record) != 0)
    {
      frame_count = BtlAnimationFramesCount(amount_record);
      if (((u32) (longest_frame_count & 0xFFFF)) < ((u32) (frame_count & 0xFFFF)))
      {
        longest_frame_count = frame_count;
        longest_record = g_BtlAmountRecords + record_offset;
      }
    }
    record_number += 1;
  }
  while (((u32) (record_number & 0xFFFF)) < 0xDU);
  return longest_record;
}
