/* Lunar SSSC — btl_move: the records' moves: the walk to the screen's edge,
   the target's dodge, the two turned to face each other, and the escape's
   moves (a failed try's steps and grid, the enemies' and slot 5's way off
   the screen). The first split had it in game_state.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (target reaches it %hi/%lo) */
/* far_syms: g_BtlNumber (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: CharStats g_BtlRecords */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActionKind g_BtlActionNumber g_BtlNumber */
/* far_syms: CharStats */
/* far_syms: g_BtlRecords */
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
extern u8 g_BtlSlot5Roaming;
extern u16 g_BtlPadRecord[];
extern s32 g_LoadBuffer;
s32 BtlAnimationEndWait();
s32 SoundPlay(s32, s32);
s32 rng_mask_below(s32, s32); /* defined K&R below: (s32 mask, u16 n) */
extern u8 g_BtlPartyNotHeldFlags[];
extern u8 g_BtlSlotRestAnimations[];
extern long HWD0;
extern long VWD0;
extern s32 g_BtlRecords;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlSlotPlannedNumbers[];
extern u8 g_BtlNumber;
extern s32 CharStats; 
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotIds[];
extern BtlWalkBlock g_BtlWalkBlock;
extern u8 g_BtlActingSlot;
extern u8 g_BtlSlotActiveFlags[];
void BtlActingRestAnimationSet();
void BtlFrameWait(s32 frame_count);
s32 BtlWalkSet();
s32 BtlWalkRun();
void BtlNothingRun();
void BtlTargetMissShow(void);
void BtlEnemyActionFallbackSet(void);
s32 BtlFrameStep();
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
void BtlActingRestAnimationSet();
void BtlFrameWait(s32 frame_count);
s32 BtlWalkSet();
s32 BtlWalkRun();
void BtlNothingRun();
void BtlTargetMissShow(void);
void BtlEnemyActionFallbackSet(void);
s32 BtlFrameStep();
/* far_syms: CharStats g_BtlTargetSlot g_BtlSlotActiveFlags */
void BtlFacingBothSet(void);
/* far_syms: g_BtlTargetSlot */
void BtlActingRestAnimationSet();
void BtlActingRestAnimationSet();
s32 BtlAnimationSet();
void BtlFrameWait(s32 frame_count);
s32 BtlWalkSet();
s32 BtlWalkRun();
/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: CharStats g_BtlActingSlot g_BtlTargetSlot */
/* far_syms: CharStats g_BtlTargetSlot g_BtlActingSlot */
/* far_syms: g_BtlTargetSlot g_BtlActionNumber g_BtlActingSlot g_BtlRecords CharStats */
/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlActingSlot CharStats g_BtlTargetSlot g_BtlRecords */
extern long HWD0, VWD0;

/* far_syms: g_BtlPadRecord (target reaches it %hi/%lo) */
s32 pad_record_update(s32 *, s32);

/* func_800365DC */
s32 BtlPadWait(s32 frame_count, s32 button_mask)
{
    s32 frames_left;
    s32 kept_mask;
    s32 pad_record;

    frames_left = frame_count;
    kept_mask = button_mask;
    if (frames_left & 0xFF)
    {
        pad_record = (s32)g_BtlPadRecord;
        do
        {
            pad_record_update((s32 *)g_BtlPadRecord, 0);
            if (g_BtlPadRecord[0] & kept_mask)
                return 1;
            BtlFrameStep();
            frames_left--;
        } while (frames_left & 0xFF);
    }
    return 0;
}

/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */

/* func_80036654 */
void BtlWalkEdgeRun(s32 walk_animation)
{
    s32 base;
    s32 tgt;
    s32 cast;
    BtlWalkBlock *blk;

    base = g_BtlRecords;
    tgt = (g_BtlTargetSlot << 6) + base;
    cast = (g_BtlActingSlot << 6) + base;
    ((BtlRecord *)base)[14].sprite.y = ((BtlRecord *)tgt)->sprite.y;
    if (((BtlRecord *)cast)->sprite.x < 0xA1 && ((BtlRecord *)tgt)->sprite.x < 0xA1)
        ((BtlRecord *)base)[14].sprite.x = 0x130 - (((BtlRecord *)cast)->width >> 1);
    else if (((BtlRecord *)cast)->sprite.x >= 0xA1 && ((BtlRecord *)tgt)->sprite.x >= 0xA1)
        ((BtlRecord *)base)[14].sprite.x = (((BtlRecord *)cast)->width >> 1) + 8;
    else if (((BtlRecord *)cast)->sprite.x < 0xA1)
        ((BtlRecord *)base)[14].sprite.x = (((BtlRecord *)cast)->width >> 1) + 8;
    else
        ((BtlRecord *)base)[14].sprite.x = 0x130 - (((BtlRecord *)cast)->width >> 1);
    ((BtlRecord *)g_BtlRecords)[14].height = 8;
    ((BtlRecord *)g_BtlRecords)[14].width = 8;
    BtlWalkSet();
    if (*(g_BtlActingSlot + g_BtlSlotIds) == 0x7F) {
        walk_animation = 0x10;
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xB);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }
    if (g_BtlNumber == 0xFE || g_BtlNumber == 0xF7) {
        g_BtlWalkBlock.slots_block = 0;
    }
    blk = &g_BtlWalkBlock;
    blk->animation = walk_animation;
    blk->target_width_counts = 0;
    blk->target = g_BtlRecords + 0x380;
    BtlWalkRun();
    if (*(g_BtlActingSlot + g_BtlSlotIds) == 0x7F) {
        BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 0xC);
        BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    }
}

/* func_80036830 */
void BtlNothingRun() {

}
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_80036838 */
void BtlTargetMissShow(void)
{
  s32 sound;
  if ((*(g_BtlTargetSlot + g_BtlSlotIds)) == 0x3E)
  {
    sound = 0x25;
    SoundPlay(sound, 3);
  }
  else
  {
    sound = 0x24;
    SoundPlay(sound, 3);
  }
  if ((*(g_BtlTargetSlot + g_BtlSlotIds)) == 0x2F)
  {
    BtlAnimationSet(g_BtlRecords + (g_BtlTargetSlot << 6), 9);
    BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
    BtlActingRestAnimationSet();
    BtlAnimationEndWait(g_BtlRecords + (g_BtlTargetSlot << 6));
    return;
  }
  BtlFrameWait(0x10);
  BtlTargetBackMove();
  BtlAnimationEndWait(g_BtlRecords + (g_BtlActingSlot << 6));
  BtlActingRestAnimationSet();
  BtlTargetForwardMove();
}
/* func_8003693C */
void BtlFacingBothSet(void)
{
  void *acting_record;
  void *target_record;
  s16 acting_x;
  s16 target_x;

  acting_record = (g_BtlActingSlot << 6) + g_BtlRecords;
  target_record = (g_BtlTargetSlot << 6) + g_BtlRecords;
  acting_x = ((BtlRecord *)acting_record)->sprite.x;
  target_x = ((BtlRecord *)target_record)->sprite.x;

  if (acting_x == target_x)
  {
    if (((BtlRecord *)acting_record)->sprite.attr & 0x2000)
    {
      ((BtlRecord *)target_record)->sprite.attr = 0x20;
    }
    else
    {
      ((BtlRecord *)target_record)->sprite.attr = 0x2020;
    }
  }
  else if (acting_x < target_x)
  {
    ((BtlRecord *)acting_record)->sprite.attr = 0x20;
    if (((*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x15)) & 0x3C) == 0)
    {
      ((BtlRecord *)target_record)->sprite.attr = 0x2020;
    }
  }
  else
  {
    ((BtlRecord *)acting_record)->sprite.attr = 0x2020;
    if (((*(u8 *)((u8 *)((g_BtlTargetSlot * 0x3C) + CharStats) + 0x15)) & 0x3C) == 0)
    {
      ((BtlRecord *)target_record)->sprite.attr = 0x20;
    }
  }
}

/* func_80036A18 */
void BtlEnemyActionFallbackSet(void)
{
    u8 id;
    id = g_BtlSlotIds[g_BtlActingSlot];
    switch (id)
    {
        case 0x10:
        case 0x18:
        case 0x1F:
        case 0x2E:
        case 0x44:
        case 0x55:
            BtlActionWalkCornerSet(1);
            *(g_BtlActingSlot + g_BtlSlotPlannedKinds) = g_BtlActionKind;
            *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = g_BtlActionNumber;
            *(g_BtlActingSlot + g_BtlSlotPlannedTargets) = g_BtlTargetSlot;
            break;
        default:
            if (g_BtlSlotPlannedKinds[g_BtlActingSlot] != 2)
            {
                g_BtlActionKind = 2;
                g_BtlSlotPlannedKinds[g_BtlActingSlot] = 2;
                if (*(u8 *)(CharStats + g_BtlActingSlot * 60 + 0x15) & 0x40)
                    g_BtlTargetSlot = BtlSlotNearestFind(0);
                else
                    g_BtlTargetSlot = BtlSlotNearestFind(2);
                g_BtlSlotPlannedTargets[g_BtlActingSlot] = g_BtlTargetSlot;
            }
    }
}

s32 BtlFrameStep();                                  /* extern */

/* func_80036BA8 */
void BtlFrameWait(s32 frame_count) {
    s32 frames_left;

    frames_left = frame_count;
    if (frames_left & 0xFFFF) {
        do {
            frames_left -= 1;
            BtlFrameStep();
        } while (frames_left & 0xFFFF);
    }
}
extern u8 g_BtlPartyMoveAnimations[];
void BtlEscapeFailStepPick(u8 idx, s16 *dx, s16 *dy, u8 *grid);  /* TU defines this below */
void BtlEscapeFailGridInit(u8 skip, u8 *grid);  /* TU defines this below */

/* func_80036BE4 */
void BtlEscapeFailMove(u8 who)
{
    u8 flags[5];
    s16 dx[5];
    s16 dy[5];
    u8 *grid;
    u8 i;
    u8 j;
    u8 k;
    u8 rep;

    grid = (u8 *)g_LoadBuffer;
    SoundPlay(0x27, 3);
    flags[0] = flags[1] = flags[2] = flags[3] = flags[4] = 0;
    if (who != 0xFF) {
        flags[who] = 1;
    } else {
        for (i = 0; i < 5; i++) {
            if (g_BtlSlotActiveFlags[i] != 0 && !(*(u8 *)(i * 0x3C + CharStats + 0x15) & 0x4C) && g_BtlPartyNotHeldFlags[i] != 0) {
                flags[i] = 1;
            }
        }
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 5; j++) {
            if (flags[j] != 0) {
                BtlAnimationSet(g_BtlRecords + j * 0x40, g_BtlPartyMoveAnimations[j]);
            }
        }
        for (rep = 3; rep != 0; rep--) {
            BtlEscapeFailGridInit(who, grid);
            for (j = 0; j < 5; j++) {
                if (flags[j] != 0) {
                    BtlEscapeFailStepPick(j, &dx[j], &dy[j], grid);
                }
            }
            for (j = 4; j != 0; ) {
                for (k = 0; k < 5; k++) {
                    if (flags[k] != 0) {
                        ((BtlRecord *)g_BtlRecords)[k].sprite.x += dx[k];
                        ((BtlRecord *)g_BtlRecords)[k].sprite.depth = ((BtlRecord *)g_BtlRecords)[k].sprite.y += dy[k];
                    }
                }
                j--;
                BtlFrameStep();
            }
        }
        for (j = 0; j < 5; j++) {
            if (flags[j] != 0) {
                BtlAnimationSet(g_BtlRecords + j * 0x40, g_BtlSlotRestAnimations[j]);
            }
        }
        for (j = 4, k = 4; j != 0; ) {
            if (k == 0) {
                for (k = 0; k < 5; k++) {
                    if (flags[k] != 0) {
                        ((BtlRecord *)g_BtlRecords)[k].sprite.attr ^= 0x2000;
                    }
                }
                k = 4;
                j--;
            }
            k--;
            BtlFrameStep();
        }
    }
    SoundPlay(0, 3);
}

extern long HWD0, VWD0;		 

/* func_80036F3C */
void BtlEscapeFailStepPick(u8 idx, s16 *dx, s16 *dy, u8 *grid)
{
    u8 cand[8];
    u8 *ent;
    u8 *p;
    s32 row;
    u8 i;
    u8 n;

    cand[0] = cand[1] = cand[2] = cand[3] = cand[4] = cand[5] = cand[6] = cand[7] = 0;
    ent = (u8 *)(idx * 0x40 + g_BtlRecords);
    row = (((BtlRecord *)ent)->sprite.y - 0x58) / 8;
    p = (u8 *)((s16)(((BtlRecord *)ent)->sprite.x / 8 + row * 40) + (s32)grid);
    if (p[-0x28] == 0xFF) {
        cand[0] = 1;
    }
    if (p[-0x27] == 0xFF) {
        cand[1] = 2;
    }
    if (p[1] == 0xFF) {
        cand[2] = 3;
    }
    if (p[0x29] == 0xFF) {
        cand[3] = 4;
    }
    if (p[0x28] == 0xFF) {
        cand[4] = 5;
    }
    if (p[0x27] == 0xFF) {
        cand[5] = 6;
    }
    if (p[-1] == 0xFF) {
        cand[6] = 7;
    }
    if (p[-0x29] == 0xFF) {
        cand[7] = 8;
    }
    for (i = 0, n = 0; i < 8; i++) {
        if (cand[i] != 0) {
            cand[n] = cand[i];
            n++;
        }
    }
    if (n == 0) {
        *dx = 0;
        *dy = 0;
        return;
    }
    switch (cand[(u8)rng_mask_below(7, n)]) {
    case 1:
        *dx = 0;
        *dy = -2;
        break;
    case 2:
        *dx = 2;
        *dy = -2;
        break;
    case 3:
        *dx = 2;
        *dy = 0;
        break;
    case 4:
        *dx = 2;
        *dy = 2;
        break;
    case 5:
        *dx = 0;
        *dy = 2;
        break;
    case 6:
        *dx = -2;
        *dy = 2;
        break;
    case 7:
        *dx = -2;
        *dy = 0;
        break;
    case 8:
        *dx = -2;
        *dy = -2;
        break;
    }
}
extern u8 g_BtlEnemySet; /* far_syms: g_BtlEnemySet */
s32 BtlWalkGridSlotBlock(u8);

/* func_80037188 */
void BtlEscapeFailGridInit(u8 skip, u8 *grid)
{
    s16 i;
    s16 k;
    u8 j;

    for (i = 0; i < 0x1E0; i++) {
        grid[i] = 0xFF;
    }
    if (skip != 0xFF) {
        for (j = 0; j < 5; j++) {
            if (j != skip && g_BtlSlotIds[j] != 0) {
                BtlWalkGridSlotBlock(j);
            }
        }
    }
    for (j = 6; j < 0xE; j++) {
        if (g_BtlSlotActiveFlags[j] != 0) {
            BtlWalkGridSlotBlock(j);
        }
    }
    for (k = 0; k < 0x78; k++) {
        grid[k] = 0xFE;
    }
    for (k = 0x78; k < 0x1E0; k += 0x28) {
        grid[k] = grid[k + 1] = grid[k + 0x26] = grid[k + 0x27] = 0xFE;
    }
    j = 0;
    if (g_BtlEnemySet == 0x1C) {
        k = 0x78;
        do {
            do {
                grid[k] = 0xFE;
                k++;
                j++;
            } while (j < 0x16);
            k += 0x12;
            j = 0;
        } while (k < 0x1E0);
    }
}
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern void (*g_BtlActingShowFunctions[])(void);

/* func_80037358 */
void BtlEnemyEscapeMove(void)
{
  u8 found;
  u16 count;
  u16 val;
  s32 base;
  found = 0;
  g_BtlActingSlot = 6;
  do
  {
    if (g_BtlSlotActiveFlags[g_BtlActingSlot] && (g_BtlSlotIds[g_BtlActingSlot] == 0x12))
    {
      found = g_BtlActingSlot;
      *((s16 *) (((g_BtlActingSlot << 6) + g_BtlRecords) + 0xC)) = 0x20;
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 9);
    }
    g_BtlActingSlot++;
  }
  while (g_BtlActingSlot < 14);
  if (found & 0xFF)
  {
    BtlAnimationEndWait(g_BtlRecords + ((found & 0xFF) << 6));
  }
  g_BtlActingSlot = 6;
  do
  {
    if (g_BtlSlotActiveFlags[g_BtlActingSlot])
    {
      BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot << 6), 1);
      *((s16 *) (((g_BtlActingSlot << 6) + g_BtlRecords) + 0xC)) = 0x20;
    }
    g_BtlActingSlot++;
  }
  while (g_BtlActingSlot < 14);
  count = 0;
  found = 0;
  do
  {
    found = 0;
    base = g_BtlRecords;
    g_BtlActingSlot = 6;
    do
    {
      if (g_BtlSlotActiveFlags[g_BtlActingSlot])
      {
        val = *((u16 *) (((g_BtlActingSlot << 6) + base) + 0x6));
        val += 4;
        *((u16 *) (((g_BtlActingSlot << 6) + base) + 0x6)) = val;
        if (((s16) val) < 0x190)
        {
          found = 1;
        }
      }
      g_BtlActingSlot++;
    }
    while (g_BtlActingSlot < 14);
    if (!found)
    {
      break;
    }
    BtlFrameStep();
    count++;
  }
  while (count < 0x190);
}

/* Escape: Nall (object slot 5, id 0xC) flies off the right edge. Clears
   g_BtlSlot5Roaming, sets his attr (+0x0C) to 0x20 (the X-mirror bit 13 clear),
   then steps his x by 8 a frame
   (BtlFrameStep = the battle frame) until it reaches 0x150. */
/* func_80037578 */
void BtlSlot5EscapeMove(void)
{
    if (g_BtlSlotIds[5] == 0xC) {
        g_BtlSlot5Roaming = 0;
        ((BtlRecord *)g_BtlRecords)[5].sprite.attr = 0x20;
        while (((BtlRecord *)g_BtlRecords)[5].sprite.x < 0x150) {
            ((BtlRecord *)g_BtlRecords)[5].sprite.x += 8;
            BtlFrameStep();
        }
    }
}
