/* Lunar SSSC — btl_walk: a battle walk set and run, and the walk grid made
   ready for it. The first split had it in rendering2_c.c. Every function is
   C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "chr.h"
#include "btl.h"
extern s32 g_BtlRecords;
extern BtlWalkBlock g_BtlWalkBlock;
extern s32 g_BtlToRecord;
extern u8 g_BtlWalkGoalDistance;
extern s32 CharStats;
extern u8 g_BtlSlotIds[];
/* far_syms: CharStats g_BtlSlotIds g_BtlSlotActiveFlags */
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlEnemySet;
extern u8 g_BtlPartyMoveAnimations[];
/* far_syms: g_BtlRecords CharStats */
/* far_syms: g_BtlSlotIds */
/* far_syms: g_BtlEnemySet (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlRecords */
void mem_clear(void *destination, u32 byte_count);

/* func_8002BA40 */
void BtlWalkCellRound(u16 *coordinate)
{
  int biased_value;
  unsigned int value_copy;
  s16 value;
  value = *coordinate;
  if (value & 7)
  {
    value_copy = value;
    biased_value = (s16) value;
    if (((s16) value) < 0)
    {
      biased_value = ((s16) value_copy) + 7;
    }
    if (((((s16) (((s16) value_copy) - ((biased_value >> 3) * 8))) + 1) - 1) < 4)
    {
      *coordinate = value & 0xFFF8;
      return;
    }
    *coordinate = (value & (~7)) + 8;
  }
}
/* far_syms: g_BtlWalkBlock */
/* far_syms: CharStats */
/* far_syms: g_BtlActingSlot */
/* far_syms: g_BtlTargetSlot */
extern u8 g_BtlActingSlot;
extern u8 g_BtlTargetSlot;

/* Fill the action record g_BtlWalkBlock for the acting slot g_BtlActingSlot:
   +0 / +4 the actor's and the target's (g_BtlTargetSlot) 0x40-byte objects,
   +8 the slot, +9..+0xC = 1, +0xE / +0xF the actor's CharStats bytes
   +0x18 / +0x13 (0x3C-byte rows). A party slot (< 5) takes +0xD from
   g_BtlPartyMoveAnimations[slot] and +0x10 = 0x2A; an enemy +0xD = 1 and +0x10 its
   CharStats +0x1E. */
/* func_8002BAA4 */
void BtlWalkSet(void)
{
  u32 idx;
  s32 idx2;
  u8 *cs;
  u8 v;
  idx = g_BtlActingSlot;
  idx2 = (s32) g_BtlTargetSlot;
  g_BtlWalkBlock.slots_block = 1;
  g_BtlWalkBlock.walker_width_counts = 1;
  g_BtlWalkBlock.target_width_counts = 1;
  g_BtlWalkBlock.walk = 1;
  g_BtlWalkBlock.walker = g_BtlRecords + (idx << 6);
  g_BtlWalkBlock.walker_slot = (u8) idx;
  g_BtlWalkBlock.target = g_BtlRecords + (idx2 << 6);
  cs = (u8 *) ((idx * 60) + CharStats);
  g_BtlWalkBlock.speed = cs[0x18];
  g_BtlWalkBlock.reach = cs[0x13];
  if (idx < 5)
  {
    v = g_BtlPartyMoveAnimations[idx];
    g_BtlWalkBlock.sound = 0x2A;
    g_BtlWalkBlock.animation = v;
  }
  else
  {
    g_BtlWalkBlock.animation = 1;
    g_BtlWalkBlock.sound = cs[0x1E];
  }
}

extern u8 g_BtlActingStatus[];
extern s32 g_BtlFromRecord;
extern u16 g_BtlWalkerColumn;
extern s16 g_BtlWalkerRow;
extern u16 g_BtlWalkerWidth;
extern u16 g_BtlWalkTargetColumn;
extern s16 g_BtlWalkTargetRow;
extern u16 g_BtlWalkWidths;
extern s16 g_BtlWalkGoalRow;
extern s16 g_BtlWalkGoalColumn;
unsigned int BtlWalkGridInit(void);  /* TU defines this below */
void BtlWalkDistanceFill(void);
void BtlWalkGoalFind(void);
void BtlWalkGoalNearestFind();
s32 BtlWalkBesideCheck();
s32 BtlWalkPathMake();
void BtlWalkFacingSet(void);
s32 BtlWalkPathFollow(void);

/* func_8002BB5C */
s32 BtlWalkRun(void)
{
  BtlRecord *pa;
  BtlRecord *pb;
  s32 ax;
  s32 ay;
  s32 bx;
  s32 by;
  s32 v;
  s32 w;
  s32 r;
  s32 clipped_width;
  s32 cost;
  if ((g_BtlWalkBlock.walker_slot < 5) && (g_BtlActingStatus[0] & 0x10))
  {
    g_BtlWalkBlock.reach = g_BtlWalkBlock.reach >> 1;
 /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
    block runs once, so behaviour is the same without the `do` / `while (0)`, but then 36
    of this function's 219 instruction lines differ. */
 do { } while (0);
  }
  if (g_BtlWalkBlock.reach >= 0x29)
  {
    g_BtlWalkBlock.reach = 0x28;
  }
  g_BtlFromRecord = g_BtlWalkBlock.walker;
  g_BtlToRecord = g_BtlWalkBlock.target;
  BtlWalkCellRound(g_BtlFromRecord + 6);
  BtlWalkCellRound(g_BtlFromRecord + 0xA);
  pa = (BtlRecord *) g_BtlFromRecord;
  AS(u16, pa->sprite.depth) = pa->sprite.y;
  ay = (pa->sprite.y - 0x58) / 8;
  g_BtlWalkerRow = ay;
  ax = pa->sprite.x / 8;
  g_BtlWalkerColumn = ax;
  pb = (BtlRecord *) g_BtlToRecord;
  by = (pb->sprite.y - 0x58) / 8;
  g_BtlWalkTargetRow = by;
  bx = pb->sprite.x / 8;
  g_BtlWalkTargetColumn = bx;
  if ((ax == bx) && (ay == by))
  {
    return 1;
  }
  if (g_BtlWalkBlock.walker_width_counts != 0)
  {
    w = ((BtlRecord *) g_BtlFromRecord)->width >> 3;
    g_BtlWalkerWidth = w;
    if (w & 1)
    {
      g_BtlWalkerWidth = w + 1;
    }
    r = g_BtlWalkerWidth;
    v = g_BtlWalkerColumn - (((s16) r) / 2);
    g_BtlWalkerColumn = v;
    if (((s16) v) < 0)
    {
      g_BtlWalkerWidth = r + v;
      g_BtlWalkerColumn = 0;
    }
  }
  else
  {
    g_BtlWalkerWidth = 1;
  }
  if (g_BtlWalkBlock.target_width_counts != 0)
  {
    w = ((BtlRecord *) g_BtlToRecord)->width >> 3;
    g_BtlWalkWidths = w;
    if (w & 1)
    {
      g_BtlWalkWidths = w + 1;
    }
    r = g_BtlWalkWidths;
    v = g_BtlWalkTargetColumn - (((s16) r) / 2);
    g_BtlWalkTargetColumn = v;
    if (((s16) v) < 0)
    {
      clipped_width = r + v;
      g_BtlWalkWidths = clipped_width;
      if (clipped_width & 1)
      {
        g_BtlWalkWidths = clipped_width + 1;
      }
      g_BtlWalkTargetColumn = 0;
    }
    if (BtlWalkBesideCheck() & 0xFF)
    {
      BtlWalkFacingSet();
      return 2;
    }
  }
  else
  {
    g_BtlWalkWidths = 1;
    if (g_BtlWalkBlock.walker_width_counts != 0)
    {
      g_BtlWalkTargetColumn = g_BtlWalkTargetColumn - (((s16) g_BtlWalkerWidth) / 2);
    }
  }
  BtlWalkGridInit();
  BtlWalkDistanceFill();
  BtlWalkGoalFind();
  cost = g_BtlWalkGoalDistance;
  if (cost >= (g_BtlWalkBlock.reach + 1))
  {
    if (g_BtlWalkBlock.walk == 0)
    {
      return 0;
    }
    BtlWalkGoalNearestFind();
    if ((g_BtlWalkGoalColumn == ((s16) g_BtlWalkerColumn)) && (g_BtlWalkGoalRow == g_BtlWalkerRow))
    {
      return 0;
    }
    if (BtlWalkPathMake() & 0xFF)
    {
      BtlWalkPathFollow();
    }
    return 0;
  }
  else
  {
    if (g_BtlWalkBlock.walk == 0)
    {
      return 1;
    }
    if (cost == 0)
    {
      return 0;
    }
    if (BtlWalkPathMake() & 0xFF)
    {
      BtlWalkPathFollow();
    }
    return 1;
  }
}

/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: g_BtlEnemySet (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlWalkPathColumns (target reaches it %hi/%lo) */
/* far_syms: g_BtlWalkPathRows (target reaches it %hi/%lo) */
/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
void mem_clear(void *destination, u32 byte_count);
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlWalkPathColumns[];
extern u8 g_BtlWalkPathRows[];
s32 BtlWalkGridSlotBlock(u8);
extern s32 g_LoadBuffer;
extern s32 g_BtlWalkGrid;
extern s16 g_BtlWalkCellIndexes;

/* func_8002BEC8 */
unsigned int BtlWalkGridInit(void)
{
  s16 next_top_cell;
  s16 next_left_start;
  s16 next_right_start;
  s16 next_cell;
  s16 cell;
  s32 cell_high;
  u16 left_cell;
  u16 right_cell;
  u8 next_slot;
  u8 left_cells_done;
  u8 right_cells_done;
  cell = 0;
  g_BtlWalkGrid = g_LoadBuffer;
  do
  {
    cell_high = cell << 0x10;
    next_cell = cell + 1;
    cell = next_cell;
    *((u8 *) (g_BtlWalkGrid + (cell_high >> 0x10))) = 0xFF;
  }
  while (next_cell < 0x1E0);
  mem_clear(g_BtlWalkPathColumns, 0x50U);
  mem_clear(g_BtlWalkPathRows, 0x50U);
  if (g_BtlWalkBlock.slots_block != 0)
  {
    g_BtlWalkGoalDistance = 0;
    do
    {
      if (((((*(g_BtlWalkGoalDistance + g_BtlSlotIds)) != 0) && (g_BtlWalkGoalDistance != g_BtlWalkBlock.walker_slot)) && (((((u8) g_BtlWalkGoalDistance) < 6U) || ((*(g_BtlWalkGoalDistance + g_BtlSlotActiveFlags)) != 0)) || ((*((u8 *) (((u8 *) ((g_BtlWalkGoalDistance * 0x3C) + CharStats)) + 0x15))) != 0))) && ((g_BtlWalkBlock.target_width_counts != 0) || (g_BtlToRecord != (g_BtlRecords + (g_BtlWalkGoalDistance << 6)))))
      {
        BtlWalkGridSlotBlock(g_BtlWalkGoalDistance);
      }
      next_slot = g_BtlWalkGoalDistance + 1;
      g_BtlWalkGoalDistance = next_slot;
    }
    while (((u32) (next_slot & 0xFF)) < 0xEU);
    if (g_BtlWalkBlock.walker_slot != 5)
    {
      g_BtlWalkCellIndexes = 0;
      do
      {
        *((u8 *) (g_BtlWalkGrid + g_BtlWalkCellIndexes)) = 0xFE;
        next_top_cell = ((u16) g_BtlWalkCellIndexes) + 1;
        g_BtlWalkCellIndexes = next_top_cell;
      }
      while (next_top_cell < 0x78);
      if (g_BtlEnemySet == 0x1C)
      {
        if (((u8) g_BtlWalkBlock.walker_slot) < 5U)
        {
          g_BtlWalkCellIndexes = 0x78;
          do
          {
            g_BtlWalkGoalDistance = 0;
            loop_20:
            *((u8 *) (g_BtlWalkGrid + g_BtlWalkCellIndexes)) = 0xFE;

            left_cell = (u16) g_BtlWalkCellIndexes;
            g_BtlWalkCellIndexes = left_cell + 1;
            left_cells_done = g_BtlWalkGoalDistance + 1;
            g_BtlWalkGoalDistance = left_cells_done;
            if (((u32) (left_cells_done & 0xFF)) < 0x16U)
            {
              goto loop_20;
            }
            next_left_start = left_cell + 0x13;
            g_BtlWalkCellIndexes = next_left_start;
          }
          while (next_left_start < 0x1E0);
        }
        if (g_BtlWalkBlock.walker_slot == 6)
        {
          g_BtlWalkCellIndexes = 0x8E;
          do
          {
            g_BtlWalkGoalDistance = 0;
            loop_25:
            *((u8 *) (g_BtlWalkGrid + g_BtlWalkCellIndexes)) = 0xFE;

            right_cell = (u16) g_BtlWalkCellIndexes;
            g_BtlWalkCellIndexes = right_cell + 1;
            right_cells_done = g_BtlWalkGoalDistance + 1;
            g_BtlWalkGoalDistance = right_cells_done;
            if (((u32) (right_cells_done & 0xFF)) < 0x12U)
            {
              goto loop_25;
            }
            next_right_start = right_cell + 0x17;
            g_BtlWalkCellIndexes = next_right_start;
          }
          while (next_right_start < 0x1E0);
        }
      }
    }
  }
}
