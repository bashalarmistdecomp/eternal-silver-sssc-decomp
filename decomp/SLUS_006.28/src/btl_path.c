/* Lunar SSSC — btl_path: the walk grid's path finding: slots blocked, the
   goal and the nearest goal, the distances filled, the path made and
   followed. The first split had it in game_misc.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlWalkBlock (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds */
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotRestAnimations */
/* far_syms: g_BtlSlotRestAnimations (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
/* far_syms: g_BtlSlotRestAnimations (carried from the first split's game_misc.c) */

extern u8 g_BtlSlotRestAnimations[];
s32 BtlAnimationSet(s32, u8);
extern u8 g_BtlSlotIds[];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern s32 g_BtlRecords;
s32 BtlFrameStep();                                  /* extern */
extern long HWD0, VWD0;
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern s32 g_BtlWalkGrid;
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern s32 g_LoadBuffer;
extern u16 g_BtlWalkGoalRow;  /* TU declares this below */
extern s16 g_BtlWalkGoalColumn;

/* func_8002C190 */
void BtlWalkGridSlotBlock(s32 slot)
{
  u16 width;
  BtlRecord *p;
  u16 val3;
  s16 row;
  s16 column;
  s16 half_width;
  int prev_column;
  p = (BtlRecord *) (((slot & 0xFF) << 6) + g_BtlRecords);
  {
    g_BtlWalkGrid = g_LoadBuffer;
  }
  row = (p->sprite.y - 0x58) / 8;
  g_BtlWalkGoalRow = row;
  column = p->sprite.x / 8;
  g_BtlWalkGoalColumn = column;
  val3 = p->width >> 3;
  g_BtlWalkWidths[1] = val3;
  if (val3 & 1)
  {
    g_BtlWalkWidths[1] = val3 + 1;
  }
  width = g_BtlWalkWidths[1];
  half_width = ((s16) width) / 2;
  if (column < half_width)
  {
    g_BtlWalkCellIndexes[0] = row * 40;
    g_BtlWalkWidths[1] = width + (column - half_width);
    if (g_BtlWalkWidths[1] & 1)
    {
      g_BtlWalkWidths[1]++;
    }
  }
  else
  {
    prev_column = column - 1;
    if ((prev_column + half_width) >= 0x28)
    {
      g_BtlWalkCellIndexes[0] = (column + (row * 40)) - half_width;
      g_BtlWalkWidths[1] = (width + 0x27) - (column + ((((s16) width) / 2) - 1));
    }
    else
    {
      g_BtlWalkCellIndexes[0] = (column + (row * 40)) - (((s16) width) / 2);
    }
  }
  if (g_BtlWalkWidths[1] != 0)
  {
    do
    {
      *((u8 *) (((u32) g_BtlWalkGrid) + ((s16) g_BtlWalkCellIndexes[0]))) = 0xFE;
      g_BtlWalkCellIndexes[0]++;
      g_BtlWalkWidths[1]--;
    }
    while (g_BtlWalkWidths[1] != 0);
  }
}

/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */
extern s32 g_BtlWalkGrid;
extern u16 g_BtlWalkerWidth;
extern BtlWalkBlock g_BtlWalkBlock;
extern u8 g_BtlWalkGoalDistance;
extern u16 g_BtlWalkTargetColumn;
extern u16 g_BtlWalkTargetRow;
extern u16 g_BtlWalkGoalRow;
extern s16 g_BtlWalkGoalColumn;
extern s16 g_BtlWalkSeedColumn;
extern s16 g_BtlWalkSeedRow;

/* load_hazard_nop: BtlWalkGoalFind */
/* func_8002C304 */
void BtlWalkGoalFind(void)
{
  u8 reach;
  u16 target_row;
  s16 var;
  u8 *ptr;

  reach = g_BtlWalkBlock.reach;
  target_row = g_BtlWalkTargetRow;
  g_BtlWalkGoalDistance = reach + 1;
  g_BtlWalkSeedRow = target_row;
  if (g_BtlWalkBlock.target_width_counts != 0)
  {
    var = (s16)(g_BtlWalkTargetColumn + 1 - g_BtlWalkerWidth);
    g_BtlWalkSeedColumn = var;
    g_BtlWalkWidths[1] = g_BtlWalkWidths[0] + g_BtlWalkerWidth - 1;
    if (var < 0)
    {
      g_BtlWalkWidths[1] += var;
      g_BtlWalkSeedColumn = 0;
    }
    BtlWalkNeighbourLowestFind();
  }
  else
  {
    var = (s16)target_row;
    ptr = (u8 *)g_BtlWalkGrid + (var * 40 + (s16)g_BtlWalkTargetColumn);
    if (reach >= *ptr)
    {
      g_BtlWalkGoalColumn = g_BtlWalkTargetColumn;
      g_BtlWalkGoalRow = target_row;
      g_BtlWalkGoalDistance = *ptr;
    }
    else
    {
      g_BtlWalkSeedColumn = g_BtlWalkTargetColumn;
      g_BtlWalkWidths[1] = 1;
      BtlWalkNeighbourLowestFind();
    }
  }
}

extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkCellIndexes[2];
extern u16 g_BtlWalkRingHeight[2];
extern s16 g_BtlWalkSideCount;

/* func_8002C3E8 */
void BtlWalkGoalNearestFind(void)
{
  u16 size;
  u16 cnt;
  s16 top;
  s16 x;
  u16 bx;
  u16 by;
  u8 c;
  u8 d;

  size = 3;
  cnt = 0x1E0;
  top = g_BtlWalkTargetRow - 1;
  g_BtlWalkGoalDistance = g_BtlWalkBlock.reach + 1;
  do
  {
    x = g_BtlWalkTargetColumn - (size - 1) / 2;
    g_BtlWalkGoalColumn = x;
    g_BtlWalkCellIndexes[0] = x + (s16)g_BtlWalkTargetRow * 40;
    if (x >= 0)
    {
      c = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
      if (c < g_BtlWalkGoalDistance)
      {
        g_BtlWalkGoalDistance = c;
        bx = x;
        by = g_BtlWalkTargetRow;
      }
    }
    x = g_BtlWalkTargetColumn + g_BtlWalkWidths[0] + (s16)((size - 1) / 2 - 1);
    g_BtlWalkGoalColumn = x;
    g_BtlWalkCellIndexes[0] = x + (s16)g_BtlWalkTargetRow * 40;
    if (x < 40)
    {
      c = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
      if (c < g_BtlWalkGoalDistance)
      {
        g_BtlWalkGoalDistance = c;
        bx = x;
        by = g_BtlWalkTargetRow;
      }
    }
    if (top >= 0)
    {
      g_BtlWalkCellIndexes[0] = g_BtlWalkTargetColumn + top * 40;
      g_BtlWalkGoalColumn = g_BtlWalkTargetColumn;
      for (g_BtlWalkRingHeight[1] = g_BtlWalkWidths[0]; (s16)g_BtlWalkRingHeight[1] != 0; g_BtlWalkRingHeight[1]--)
      {
        d = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
        if (d < g_BtlWalkGoalDistance)
        {
          g_BtlWalkGoalDistance = d;
          bx = g_BtlWalkGoalColumn;
          by = top;
        }
        g_BtlWalkCellIndexes[0]++;
        g_BtlWalkGoalColumn++;
      }
    }
    g_BtlWalkGoalRow = top + size - 1;
    if ((s16)g_BtlWalkGoalRow < 12)
    {
      g_BtlWalkCellIndexes[0] = g_BtlWalkTargetColumn + (s16)g_BtlWalkGoalRow * 40;
      g_BtlWalkGoalColumn = g_BtlWalkTargetColumn;
      for (g_BtlWalkRingHeight[1] = g_BtlWalkWidths[0]; (s16)g_BtlWalkRingHeight[1] != 0; g_BtlWalkRingHeight[1]--)
      {
        d = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
        if (d < g_BtlWalkGoalDistance)
        {
          g_BtlWalkGoalDistance = d;
          bx = g_BtlWalkGoalColumn;
          by = g_BtlWalkGoalRow;
        }
        g_BtlWalkCellIndexes[0]++;
        g_BtlWalkGoalColumn++;
      }
    }
    g_BtlWalkGoalRow = top + 1;
    g_BtlWalkSideCount = (size - 1) / 2 - 1;
    g_BtlWalkRingHeight[0] = g_BtlWalkSideCount;
    g_BtlWalkGoalColumn = g_BtlWalkTargetColumn - 1;
    g_BtlWalkCellIndexes[0] = g_BtlWalkGoalColumn + (s16)g_BtlWalkGoalRow * 40;
    for (; (s16)g_BtlWalkRingHeight[0] != 0; g_BtlWalkRingHeight[0]--)
    {
      if (g_BtlWalkGoalColumn < 0)
        break;
      if ((s16)g_BtlWalkGoalRow >= 0)
      {
        c = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
        if (c < g_BtlWalkGoalDistance)
        {
          g_BtlWalkGoalDistance = c;
          bx = g_BtlWalkGoalColumn;
          by = g_BtlWalkGoalRow;
        }
      }
      g_BtlWalkGoalColumn--;
      g_BtlWalkGoalRow++;
      g_BtlWalkCellIndexes[0] += 39;
    }
    g_BtlWalkGoalRow = top + 1;
    g_BtlWalkGoalColumn = g_BtlWalkTargetColumn + g_BtlWalkWidths[0];
    g_BtlWalkCellIndexes[0] = g_BtlWalkGoalColumn + (s16)g_BtlWalkGoalRow * 40;
    for (g_BtlWalkRingHeight[0] = g_BtlWalkSideCount; (s16)g_BtlWalkRingHeight[0] != 0; g_BtlWalkRingHeight[0]--)
    {
      if (g_BtlWalkGoalColumn >= 40)
        break;
      if ((s16)g_BtlWalkGoalRow >= 0)
      {
        c = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
        if (c < g_BtlWalkGoalDistance)
        {
          g_BtlWalkGoalDistance = c;
          bx = g_BtlWalkGoalColumn;
          by = g_BtlWalkGoalRow;
        }
      }
      g_BtlWalkGoalColumn++;
      g_BtlWalkGoalRow++;
      g_BtlWalkCellIndexes[0] += 41;
    }
    g_BtlWalkGoalRow = top + size - 2;
    g_BtlWalkGoalColumn = g_BtlWalkTargetColumn - 1;
    g_BtlWalkCellIndexes[0] = g_BtlWalkGoalColumn + (s16)g_BtlWalkGoalRow * 40;
    for (g_BtlWalkRingHeight[0] = g_BtlWalkSideCount; (s16)g_BtlWalkRingHeight[0] != 0; g_BtlWalkRingHeight[0]--)
    {
      if (g_BtlWalkGoalColumn < 0)
        break;
      if ((s16)g_BtlWalkGoalRow < 12)
      {
        c = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
        if (c < g_BtlWalkGoalDistance)
        {
          g_BtlWalkGoalDistance = c;
          bx = g_BtlWalkGoalColumn;
          by = g_BtlWalkGoalRow;
        }
      }
      g_BtlWalkGoalColumn--;
      g_BtlWalkGoalRow--;
      g_BtlWalkCellIndexes[0] -= 41;
    }
    g_BtlWalkGoalRow = top + size - 2;
    g_BtlWalkGoalColumn = g_BtlWalkTargetColumn + g_BtlWalkWidths[0];
    g_BtlWalkCellIndexes[0] = g_BtlWalkGoalColumn + (s16)g_BtlWalkGoalRow * 40;
    for (g_BtlWalkRingHeight[0] = g_BtlWalkSideCount; (s16)g_BtlWalkRingHeight[0] != 0; g_BtlWalkRingHeight[0]--)
    {
      if (g_BtlWalkGoalColumn >= 40)
        break;
      if ((s16)g_BtlWalkGoalRow < 12)
      {
        c = *(u8 *)(g_BtlWalkGrid + (s16)g_BtlWalkCellIndexes[0]);
        if (c < g_BtlWalkGoalDistance)
        {
          g_BtlWalkGoalDistance = c;
          bx = g_BtlWalkGoalColumn;
          by = g_BtlWalkGoalRow;
        }
      }
      g_BtlWalkGoalColumn++;
      g_BtlWalkGoalRow--;
      g_BtlWalkCellIndexes[0] -= 39;
    }
    if (g_BtlWalkGoalDistance <= g_BtlWalkBlock.reach)
      break;
    top--;
    cnt--;
    size += 2;
  } while (cnt != 0);
  g_BtlWalkGoalColumn = bx;
  g_BtlWalkGoalRow = by;
}
extern u16 g_BtlWalkerColumn;
extern s16 g_BtlWalkerRow;
extern short g_BtlWalkWidths[2];

/* The approach walk's "already beside it" test (battle.md "The approach
   walk"): BtlWalkRun calls it for a wide target, once the planner's
   tile coordinates are set, and answers 2 through BtlWalkFacingSet when this
   says so. It reads only the planner's globals and writes nothing:
     g_BtlWalkerColumn / g_BtlWalkerRow  the walker's first column / its row
     g_BtlWalkerWidth               the walker's width in tiles
     g_BtlWalkTargetColumn / g_BtlWalkTargetRow  the target's first column / its row
     g_BtlWalkWidths[0]            the target's width in tiles
   0 unless the target's row is the walker's, the one above it or the one
   below it. Then the columns: with the walker's column below the
   target's, 1 when walker column + walker width >= target column;
   otherwise 1 when target column + target width >= walker column. No
   calls, no RNG draw, no frame wait.

   All the comparisons are signed. This TU declares the columns, the
   walker's width and the target's row u16, so those reads carry (s16).

   Shape: the rows above and below are short locals set before the test.
   With `g_BtlWalkerRow + 1` written inside the third comparison the add is
   made after the second branch (31/34); `above` could be written inline.
   The refusal is the early `return 0`: with the column test inside the if
   and `return 0` last, the 0 goes to the last return's delay slot instead
   of the third branch's (32/34). The column answer is one conditional
   expression: as `if (walker < target) return ...; return ...;` the two
   arms change places (27/34). */
/* func_8002C9AC */
s32 BtlWalkBesideCheck(void)
{
  s16 above;
  s16 below;

  above = g_BtlWalkerRow - 1;
  below = g_BtlWalkerRow + 1;
  if (above != (s16) g_BtlWalkTargetRow
      && g_BtlWalkerRow != (s16) g_BtlWalkTargetRow
      && below != (s16) g_BtlWalkTargetRow)
  {
    return 0;
  }
  return (s16) g_BtlWalkerColumn < (s16) g_BtlWalkTargetColumn
      ? (s16) g_BtlWalkerColumn + (s16) g_BtlWalkerWidth >= (s16) g_BtlWalkTargetColumn
      : (s16) g_BtlWalkTargetColumn + g_BtlWalkWidths[0] >= (s16) g_BtlWalkerColumn;
}

extern s32 g_BtlFromRecord;
extern s32 g_BtlToRecord;

/* func_8002CA34 */
void BtlWalkFacingSet(void)
{
    u8 *walker_record = (u8 *)g_BtlFromRecord;
    u8 *target_record = (u8 *)g_BtlToRecord;

    if (((BtlRecord *)walker_record)->sprite.x < ((BtlRecord *)target_record)->sprite.x)
        ((BtlRecord *)walker_record)->sprite.attr = 0x20;
    else
        ((BtlRecord *)walker_record)->sprite.attr = 0x2020;

    BtlFrameStep();
}

/* far_syms: g_BtlWalkPathColumns (target reaches it %hi/%lo) */
/* far_syms: g_BtlWalkPathRows (target reaches it %hi/%lo) */
void BtlAnimationEndWait(void *record);  /* extern */
s32 SoundPlay(s32, s32);
extern u8 g_BtlWalkNodeCount;
extern u8 g_BtlWalkPathColumns[];
extern u8 g_BtlWalkPathRows[];

/* func_8002CA78 */
s32 BtlWalkPathFollow(void)
{
  s16 start_x;
  s16 walker_y;
  s16 walker_x;
  s16 column;
  s16 node_x;
  s16 new_x;
  s32 frame_pixels;
  s32 cell_frames;
  u16 old_y;
  u16 old_x;
  BtlWalkBlock *walk_block;
  u8 node;
  u8 frames_done;
  u8 switch_val;
  if (g_BtlWalkNodeCount != 0)
  {
    walk_block = &g_BtlWalkBlock;
    if (walk_block->sound != 0xFF)
    {
      SoundPlay(walk_block->sound, 3);
    }
    switch_val = walk_block->speed;
    switch (switch_val)
    {
      case 1:
        cell_frames = 1;
        break;

      case 2:
        cell_frames = 2;
        break;

      case 3:
        cell_frames = 4;
        break;

      default:
        cell_frames = 8;
        break;

    }

    frame_pixels = value_div(8, cell_frames);
    BtlAnimationSet(g_BtlWalkBlock.walker, g_BtlWalkBlock.animation);
    g_BtlWalkGoalDistance = 0;
    do
    {
      loop_14:
      if (g_BtlWalkGoalDistance == 0)
      {
        node = g_BtlWalkNodeCount - 1;
        g_BtlWalkNodeCount = node;
        column = *((s16 *) (((u8 *) g_BtlWalkPathColumns) + ((node & 0xFF) * 2)));
        g_BtlWalkSeedColumn = (s16) (column * 8);
        start_x = ((BtlRecord *)g_BtlFromRecord)->sprite.x;
        node_x = (s16) (column * 8);
        if (start_x != node_x)
        {
          if (start_x < node_x)
            ((BtlRecord *)g_BtlFromRecord)->sprite.attr = 0x20;
          else
            ((BtlRecord *)g_BtlFromRecord)->sprite.attr = 0x2020;
        }
        g_BtlWalkSeedRow = (s16) (((*((s16 *) (((u8 *) g_BtlWalkPathRows) + (g_BtlWalkNodeCount * 2)))) * 8) + 0x58);
      }

      walker_y = ((BtlRecord *)g_BtlFromRecord)->sprite.y;
      old_y = (u16) (((BtlRecord *)g_BtlFromRecord)->sprite.y);
      if (walker_y != g_BtlWalkSeedRow)
      {
        if (walker_y < g_BtlWalkSeedRow)
        {
          ((BtlRecord *)g_BtlFromRecord)->sprite.y = (s16) (old_y + frame_pixels);
          ((BtlRecord *)g_BtlFromRecord)->sprite.depth = (s16) (AS(u16, ((BtlRecord *)g_BtlFromRecord)->sprite.depth) + frame_pixels);
        }
        else
        {
          ((BtlRecord *)g_BtlFromRecord)->sprite.y = (s16) (old_y - frame_pixels);
          ((BtlRecord *)g_BtlFromRecord)->sprite.depth = (s16) (AS(u16, ((BtlRecord *)g_BtlFromRecord)->sprite.depth) - frame_pixels);
        }
      }
      walker_x = ((BtlRecord *)g_BtlFromRecord)->sprite.x;
      old_x = (u16) (((BtlRecord *)g_BtlFromRecord)->sprite.x);
      if (walker_x != g_BtlWalkSeedColumn)
      {
        if (walker_x < g_BtlWalkSeedColumn)
        {
          new_x = (s16) (old_x + frame_pixels);
          ((BtlRecord *)g_BtlFromRecord)->sprite.x = new_x;
        }
        else
        {
          new_x = (s16) (old_x - frame_pixels);
          ((BtlRecord *)g_BtlFromRecord)->sprite.x = new_x;
        }
      }
      BtlFrameStep();
      frames_done = g_BtlWalkGoalDistance + 1;
      g_BtlWalkGoalDistance = frames_done;
      if ((frames_done & 0xFF) < cell_frames)
      {
        goto loop_14;
      }
      g_BtlWalkGoalDistance = 0;
    }
    while (g_BtlWalkNodeCount != 0);
    walk_block = &g_BtlWalkBlock;
    if (walk_block->sound != 0xFF)
    {
      SoundPlay(0, 3);
    }
    if (g_BtlSlotIds[walk_block->walker_slot] == 0x73)
    {
      BtlAnimationEndWait(g_BtlFromRecord);
    }
    if (((u8) (walk_block->walker_slot)) < 0xE)
    {
      BtlAnimationSet(g_BtlWalkBlock.walker, g_BtlSlotRestAnimations[walk_block->walker_slot]);
    }
  }
}
extern short g_BtlWalkWidths[2];

/* The approach walk's path (battle.md "The approach walk"): from the cell
   the planner chose (g_BtlWalkGoalColumn column / g_BtlWalkGoalRow row) down the cost
   grid to the walker.

   Refused (0) when the row is 13 or more, the column 41 or more (both
   compared unsigned), or the cell's cost on the 40-wide grid *g_BtlWalkGrid
   is above the reach in the walk context (g_BtlWalkBlock +0xF).

   Otherwise the scan's span count g_BtlWalkWidths[1] becomes 1, the node count
   g_BtlWalkNodeCount is cleared, and while the current cell's cost is not 0: the
   cell is recorded as node g_BtlWalkNodeCount (its column into g_BtlWalkPathColumns, its row
   into g_BtlWalkPathRows, both u16 lists; with the context's +0xA set the column
   is the wide walker's middle one, + g_BtlWalkerWidth / 2), it becomes the
   scan's seed (g_BtlWalkSeedColumn / g_BtlWalkSeedRow), BtlWalkNeighbourLowestFind moves the result
   to its lowest neighbour and the node count goes up. Answers 1.

   g_BtlWalkGoalColumn is s16 in this TU and is read unsigned here twice (the range
   check and the wide walker's column), the spelling BtlWalkNeighbourLowestFind uses.
   The cell index is column + row * 40, as in the scans above. */
/* func_8002CD04 */
s32 BtlWalkPathMake(void)
{
  if (g_BtlWalkGoalRow >= 13)
  {
    return 0;
  }
  if ((u16) g_BtlWalkGoalColumn >= 41)
  {
    return 0;
  }
  if (((u8 *) g_BtlWalkGrid)[g_BtlWalkGoalColumn + (s16) g_BtlWalkGoalRow * 40] > g_BtlWalkBlock.reach)
  {
    return 0;
  }
  g_BtlWalkWidths[1] = 1;
  g_BtlWalkNodeCount = 0;
  if (g_BtlWalkBlock.walker_width_counts != 0)
  {
    while (((u8 *) g_BtlWalkGrid)[g_BtlWalkGoalColumn + (s16) g_BtlWalkGoalRow * 40] != 0)
    {
      ((u16 *) g_BtlWalkPathColumns)[g_BtlWalkNodeCount] = (u16) g_BtlWalkGoalColumn + (s16) g_BtlWalkerWidth / 2;
      g_BtlWalkSeedColumn = g_BtlWalkGoalColumn;
      ((u16 *) g_BtlWalkPathRows)[g_BtlWalkNodeCount] = g_BtlWalkGoalRow;
      g_BtlWalkSeedRow = g_BtlWalkGoalRow;
      BtlWalkNeighbourLowestFind();
      g_BtlWalkNodeCount++;
    }
  }
  else
  {
    while (((u8 *) g_BtlWalkGrid)[g_BtlWalkGoalColumn + (s16) g_BtlWalkGoalRow * 40] != 0)
    {
      ((u16 *) g_BtlWalkPathColumns)[g_BtlWalkNodeCount] = g_BtlWalkGoalColumn;
      g_BtlWalkSeedColumn = g_BtlWalkGoalColumn;
      ((u16 *) g_BtlWalkPathRows)[g_BtlWalkNodeCount] = g_BtlWalkGoalRow;
      g_BtlWalkSeedRow = g_BtlWalkGoalRow;
      BtlWalkNeighbourLowestFind();
      g_BtlWalkNodeCount++;
    }
  }
  return 1;
}

extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkRingHeight[2];
extern u16 g_BtlWalkCellIndexes[2];

/* func_8002CEE0 */
void BtlWalkNeighbourLowestFind(unused, unused2, lowest_row, lowest_column)
s32 unused;
s32 unused2;
u16 lowest_row;
s16 lowest_column;
{
  s16 above_row;
  s16 below_row;
  s32 upper_sweep_row;
  s16 right_column;
  s16 left_column;
  s16 right_cell;
  s16 left_cell;
  s16 above_cell;
  s16 upper_sweep_cell;
  s16 below_cell;
  s16 lower_sweep_row;
  s16 upper_cells_left;
  s16 lower_cells_left;
  u8 upper_sweep_distance;
  u8 *sweep_cells_left;
  u8 lower_sweep_distance;
  u8 right_distance;
  u8 left_distance;
  u8 above_distance;
  u8 below_distance;
  right_column = g_BtlWalkSeedColumn + (*((s16 *) (((u8 *) (&g_BtlWalkWidths)) + 0x2)));
  g_BtlWalkGoalColumn = right_column;
  if (right_column < 0x28)
  {
    right_cell = right_column + (g_BtlWalkSeedRow * 0x28);
    g_BtlWalkCellIndexes[0] = right_cell;
    right_distance = *((s32 *) (g_BtlWalkGrid + right_cell));
    if (right_distance < ((u8) g_BtlWalkGoalDistance))
    {
      g_BtlWalkGoalDistance = right_distance;
      lowest_column = right_column;
      lowest_row = (u16) g_BtlWalkSeedRow;
    }
  }
  if (g_BtlWalkSeedColumn != 0)
  {
    left_column = g_BtlWalkSeedColumn - 1;
    g_BtlWalkGoalColumn = left_column;
    left_cell = left_column + (g_BtlWalkSeedRow * 0x28);
    g_BtlWalkCellIndexes[0] = left_cell;
    left_distance = *((s32 *) (g_BtlWalkGrid + left_cell));
    if (left_distance < ((u8) g_BtlWalkGoalDistance))
    {
      g_BtlWalkGoalDistance = left_distance;
      lowest_column = left_column;
      lowest_row = (u16) g_BtlWalkSeedRow;
    }
  }
  if (g_BtlWalkSeedRow != 0)
  {
    above_row = ((u16) g_BtlWalkSeedRow) - 1;
    g_BtlWalkGoalRow = above_row;
    above_cell = g_BtlWalkSeedColumn + (above_row * 0x28);
    g_BtlWalkCellIndexes[0] = above_cell;
    above_distance = *((s32 *) (g_BtlWalkGrid + above_cell));
    if (above_distance < ((u8) g_BtlWalkGoalDistance))
    {
      g_BtlWalkGoalDistance = above_distance;
      lowest_column = (s16) g_BtlWalkSeedColumn;
      lowest_row = (u16) above_row;
    }
  }
  below_row = ((u16) g_BtlWalkSeedRow) + 1;
  g_BtlWalkGoalRow = below_row;
  if (below_row < 0xC)
  {
    below_cell = g_BtlWalkSeedColumn + (below_row * 0x28);
    g_BtlWalkCellIndexes[0] = below_cell;
    below_distance = *((s32 *) (g_BtlWalkGrid + below_cell));
    if (below_distance < ((u8) g_BtlWalkGoalDistance))
    {
      g_BtlWalkGoalDistance = below_distance;
      lowest_column = (s16) g_BtlWalkSeedColumn;
      lowest_row = (u16) below_row;
    }
  }
  upper_sweep_row = (u16) g_BtlWalkSeedRow;
  if (g_BtlWalkSeedRow != 0)
  {
    upper_sweep_row = upper_sweep_row - 1;
    {
      g_BtlWalkGoalRow = upper_sweep_row;
    }
    g_BtlWalkGoalColumn = (s16) g_BtlWalkSeedColumn;
    *((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2)) = (s16) ((*((s16 *) (((u8 *) (&g_BtlWalkWidths)) + 0x2))) + 1);
    if (g_BtlWalkSeedColumn != 0)
    {
      g_BtlWalkGoalColumn = g_BtlWalkSeedColumn - 1;
      *((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2)) = (s16) ((*((s16 *) (((u8 *) (&g_BtlWalkWidths)) + 0x2))) + 2);
    }
    g_BtlWalkCellIndexes[0] = ((u16) g_BtlWalkGoalColumn) + (((s16) upper_sweep_row) * 0x28);
    if ((*((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2))) != 0)
    {
      /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
         the block runs once, so behaviour is the same without the `do` / `while (0)`,
         but then 15 of this function's 221 instruction lines differ. */
      do
      {
        loop_17:
        if (g_BtlWalkGoalColumn < 0x28)
        {
          upper_sweep_distance = *((s32 *) (g_BtlWalkGrid + (upper_sweep_cell = (s16) g_BtlWalkCellIndexes[0])));
          if (upper_sweep_distance < ((u8) g_BtlWalkGoalDistance))
          {
            lowest_row = (u16) g_BtlWalkGoalRow;
            lowest_column = (s16) ((u16) g_BtlWalkGoalColumn);
            g_BtlWalkGoalDistance = upper_sweep_distance;
          }
          g_BtlWalkGoalColumn = ((u16) g_BtlWalkGoalColumn) + 1;
          g_BtlWalkCellIndexes[0] = ((u16) g_BtlWalkCellIndexes[0]) + 1;
          upper_cells_left = ((u16) (*((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2)))) - 1;
          *((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2)) = upper_cells_left;
          if ((upper_cells_left << 0x10) != 0)
          {
            goto loop_17;
          }
        }
      }
      while (0);
    }
  }
  lower_sweep_row = ((u16) g_BtlWalkSeedRow) + 1;
  g_BtlWalkGoalRow = lower_sweep_row;
  if (lower_sweep_row < 0xC)
  {
    *((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2)) = (s16) ((*((s16 *) (((u8 *) (&g_BtlWalkWidths)) + 0x2))) + 1);
    g_BtlWalkGoalColumn = (s16) g_BtlWalkSeedColumn;
    if (g_BtlWalkSeedColumn != 0)
    {
      g_BtlWalkGoalColumn = g_BtlWalkSeedColumn - 1;
      *((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2)) = (s16) ((*((s16 *) (((u8 *) (&g_BtlWalkWidths)) + 0x2))) + 2);
    }
    g_BtlWalkCellIndexes[0] = ((u16) g_BtlWalkGoalColumn) + (lower_sweep_row * 0x28);
    if ((*((s16 *) (((u8 *) (&g_BtlWalkRingHeight)) + 0x2))) != 0)
    {
      /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
         the block runs once, so behaviour is the same without the `do` / `while (0)`,
         but then 15 of this function's 221 instruction lines differ. */
      do
      {
        loop_26:
        if (g_BtlWalkGoalColumn < 0x28)
        {
          lower_sweep_distance = *((s32 *) (g_BtlWalkGrid + (left_column = (s16) g_BtlWalkCellIndexes[0])));
          if (lower_sweep_distance < ((u8) g_BtlWalkGoalDistance))
          {
            lowest_row = (u16) g_BtlWalkGoalRow;
            lowest_column = (s16) ((u16) g_BtlWalkGoalColumn);
            g_BtlWalkGoalDistance = lower_sweep_distance;
          }
          sweep_cells_left = ((u8 *) (&g_BtlWalkRingHeight)) + 0x2;
          g_BtlWalkGoalColumn = ((u16) g_BtlWalkGoalColumn) + 1;
          g_BtlWalkCellIndexes[0] = ((u16) g_BtlWalkCellIndexes[0]) + 1;
          lower_cells_left = ((u16) (below_row = *((s16 *) sweep_cells_left))) - 1;
          *((s16 *) sweep_cells_left) = lower_cells_left;
          if ((lower_cells_left << 0x10) != 0)
          {
            goto loop_26;
          }
        }
      }
      while (0);
    }
  }
  g_BtlWalkGoalColumn = lowest_column;
  g_BtlWalkGoalRow = (s16) lowest_row;
}
/* far_syms: g_BtlWalkBlockReach */
/* load_hazard_nop: BtlWalkDistanceFill */
extern short g_BtlWalkWidths[2];
extern u16 g_BtlWalkRingHeight[2];
extern u16 g_BtlWalkRingWidth;
extern u16 g_BtlWalkCellIndexes[2];
extern u16 g_BtlWalkerColumn;
extern s16 g_BtlWalkerRow;
extern u8 g_BtlWalkBlockReach;
int BtlWalkCellBlockedCheck(void);  /* TU defines this below */
u8 BtlWalkCellDistanceGet(s16, s16);

/* func_8002D254 */
void BtlWalkDistanceFill(void)
{
  u8 n;
  u16 w;
  u16 h;
  u16 y;
  s16 x;
  s16 xmin;
  s16 xmax;
  s16 t;

  n = 0;
  w = 3;
  h = 3;
  g_BtlWalkCellIndexes[0] = g_BtlWalkerColumn + g_BtlWalkerRow * 40;
  ((u8 *)g_BtlWalkGrid)[(s16)g_BtlWalkCellIndexes[0]] = 0;
  y = g_BtlWalkerRow;
  x = g_BtlWalkerColumn;
  if (x != 0)
    x--;
  else
    w--;
  if (g_BtlWalkerRow != 0)
    y--;
  else
    h--;
  xmin = 1;
  if (g_BtlWalkBlock.walker_width_counts != 0)
  {
    xmax = 40 - g_BtlWalkerWidth;
  }
  else
  {
    xmin = 0;
    xmax = 40;
  }
  if (g_BtlWalkBlock.slots_block != 0)
    g_BtlWalkGoalDistance = g_BtlWalkBlock.reach;
  else
    g_BtlWalkGoalDistance = 1;
  if (g_BtlWalkGoalDistance > 40)
    g_BtlWalkGoalDistance = 40;
  if (g_BtlWalkGoalDistance != 0)
  {
    do
    {
      g_BtlWalkRingHeight[0] = h;
      g_BtlWalkGoalRow = y;
      g_BtlWalkRingWidth = w;
      g_BtlWalkGoalColumn = x;
      g_BtlWalkNodeCount = g_BtlWalkBlockReach;
      if (g_BtlWalkNodeCount != 0)
      {
        do
        {
          if ((s16)g_BtlWalkGoalRow >= 0)
          {
            if (g_BtlWalkGoalColumn < xmin)
            {
              g_BtlWalkSeedColumn = xmin;
              g_BtlWalkSideCount = g_BtlWalkGoalColumn + g_BtlWalkRingWidth;
            }
            else
            {
              g_BtlWalkSeedColumn = g_BtlWalkGoalColumn;
              g_BtlWalkSideCount = g_BtlWalkRingWidth;
            }
            g_BtlWalkCellIndexes[0] = g_BtlWalkSeedColumn + (s16)g_BtlWalkGoalRow * 40;
            while (g_BtlWalkSideCount != 0 && g_BtlWalkSeedColumn < xmax)
            {
              if (!(u8)BtlWalkCellBlockedCheck())
                ((u8 *)g_BtlWalkGrid)[(s16)g_BtlWalkCellIndexes[0]] = BtlWalkCellDistanceGet(g_BtlWalkSeedColumn, g_BtlWalkGoalRow);
              g_BtlWalkSideCount--;
              g_BtlWalkSeedColumn++;
              g_BtlWalkCellIndexes[0]++;
            }
          }
          g_BtlWalkSeedColumn = g_BtlWalkGoalColumn + g_BtlWalkRingWidth - 1;
          if (g_BtlWalkSeedColumn < xmax)
          {
            if ((s16)g_BtlWalkGoalRow < 0)
            {
              g_BtlWalkSeedRow = 0;
              g_BtlWalkSideCount = g_BtlWalkGoalRow + g_BtlWalkRingHeight[0];
            }
            else
            {
              g_BtlWalkSeedRow = g_BtlWalkGoalRow;
              g_BtlWalkSideCount = g_BtlWalkRingHeight[0];
            }
            g_BtlWalkCellIndexes[0] = g_BtlWalkSeedColumn + g_BtlWalkSeedRow * 40;
            while (g_BtlWalkSideCount != 0 && g_BtlWalkSeedRow < 12)
            {
              if (!(u8)BtlWalkCellBlockedCheck())
                ((u8 *)g_BtlWalkGrid)[(s16)g_BtlWalkCellIndexes[0]] = BtlWalkCellDistanceGet(g_BtlWalkSeedColumn, g_BtlWalkSeedRow);
              g_BtlWalkSideCount--;
              g_BtlWalkSeedRow++;
              g_BtlWalkCellIndexes[0] += 40;
            }
          }
          g_BtlWalkSeedRow = g_BtlWalkGoalRow + g_BtlWalkRingHeight[0] - 1;
          if (g_BtlWalkSeedRow < 12)
          {
            if (g_BtlWalkGoalColumn + (s16)g_BtlWalkRingWidth - 1 >= xmax)
            {
              g_BtlWalkSeedColumn = xmax - 1;
              if (g_BtlWalkGoalColumn < xmin)
                g_BtlWalkSideCount = xmax;
              else
                g_BtlWalkSideCount = xmax - g_BtlWalkGoalColumn;
            }
            else
            {
              t = g_BtlWalkGoalColumn + g_BtlWalkRingWidth;
              g_BtlWalkSeedColumn = t - 1;
              if (g_BtlWalkGoalColumn < xmin)
                g_BtlWalkSideCount = t;
              else
                g_BtlWalkSideCount = g_BtlWalkRingWidth;
            }
            g_BtlWalkCellIndexes[0] = g_BtlWalkSeedColumn + g_BtlWalkSeedRow * 40;
            while (g_BtlWalkSideCount != 0 && g_BtlWalkSeedColumn >= xmin)
            {
              if (!(u8)BtlWalkCellBlockedCheck())
                ((u8 *)g_BtlWalkGrid)[(s16)g_BtlWalkCellIndexes[0]] = BtlWalkCellDistanceGet(g_BtlWalkSeedColumn, g_BtlWalkSeedRow);
              g_BtlWalkSideCount--;
              g_BtlWalkSeedColumn--;
              g_BtlWalkCellIndexes[0]--;
            }
          }
          if (g_BtlWalkGoalColumn >= xmin)
          {
            g_BtlWalkSeedColumn = g_BtlWalkGoalColumn;
            if ((s16)g_BtlWalkGoalRow + (s16)g_BtlWalkRingHeight[0] - 1 >= 12)
            {
              g_BtlWalkSeedRow = 11;
              if ((s16)g_BtlWalkGoalRow < 0)
                g_BtlWalkSideCount = 12;
              else
                g_BtlWalkSideCount = 12 - g_BtlWalkGoalRow;
            }
            else
            {
              t = g_BtlWalkGoalRow + g_BtlWalkRingHeight[0];
              g_BtlWalkSeedRow = t - 1;
              if ((s16)g_BtlWalkGoalRow < 0)
                g_BtlWalkSideCount = t;
              else
                g_BtlWalkSideCount = g_BtlWalkRingHeight[0];
            }
            g_BtlWalkCellIndexes[0] = g_BtlWalkSeedColumn + g_BtlWalkSeedRow * 40;
            while (g_BtlWalkSideCount != 0 && g_BtlWalkSeedRow >= 0)
            {
              if (!(u8)BtlWalkCellBlockedCheck())
                ((u8 *)g_BtlWalkGrid)[(s16)g_BtlWalkCellIndexes[0]] = BtlWalkCellDistanceGet(g_BtlWalkSeedColumn, g_BtlWalkSeedRow);
              g_BtlWalkSideCount--;
              g_BtlWalkSeedRow--;
              g_BtlWalkCellIndexes[0] -= 40;
            }
          }
          n++;
          if (n > 64)
          {
            BtlFrameStep();
            n = 0;
          }
          g_BtlWalkNodeCount--;
          g_BtlWalkGoalRow--;
          g_BtlWalkGoalColumn--;
          g_BtlWalkRingHeight[0] += 2;
          g_BtlWalkRingWidth += 2;
        } while (g_BtlWalkNodeCount != 0);
      }
    } while (--g_BtlWalkGoalDistance != 0);
  }
}
/* The cost flood's relaxation step for one cell of the 40x12 arena grid
   (*g_BtlWalkGrid, one byte per cell, row stride 40; 0xFE is a blocked cell,
   0xFF one the flood has not reached).  Returns the cost cell (x, y)
   should hold: the lowest of its up-to-eight neighbours plus one, unless
   the cell's own value is already strictly lower, which is then returned
   unchanged.  Neighbours off the grid are skipped (x == 0, x >= 39,
   y == 0, y >= 11).  A lowest neighbour of 0xFE or 0xFF gives 0xFF, so
   walls and unreached cells never seed a cost.  BtlWalkDistanceFill (the
   flood, above) stores the result back into the cell.  Read order: row
   y-1 (left, middle, right), right, row y+1 (left, middle, right), left,
   centre.

   Every cell is read into `c` except the middle one of row y+1, which
   is compared straight from memory.  That exception is source shape:
   it is what puts the row y-1 middle cell's value in $v1 and its address
   in $a1 (the reverse of row y+1), and the Japanese build (SLPS_013.97,
   GCC 2.7.2) is reproduced register for register by the same shape.

   CODEGEN FORCING DEVICE (spelling), not recovered source:
   the row y+1 middle cell runs its minimum through `t` (t = best; if
   (cell < t) t = cell; best = t) where the original most likely had the
   plain `if (cell < best) best = cell;`.  Behaviour-identical.  The copy
   back from `t` -- a variable assigned on both sides of a label -- is the
   one assignment to `best` whose bits combine cannot bound, and that
   keeps the target's `andi 0xFF` on `best` at all eight later compares
   (with only 0xFF and byte loads assigned, this GCC 2.8.1 drops every
   one: 126/152).  `t` costs no instruction: it shares $a2 with `best`.
   The plain form is a full match under a GCC 2.8.1 built with
   SHORT_IMMEDIATES_SIGN_EXTEND defined. */
/* func_8002D89C */
u8 BtlWalkCellDistanceGet(s16 x, s16 y)
{
  u8 best;
  s16 yy;
  u8 c;
  u8 t;

  best = 0xFF;
  if (y != 0)
  {
    yy = y - 1;
    if (x != 0)
    {
      c = *((u8 *) ((((yy * 40) + x) + g_BtlWalkGrid) - 1));
      if (c < best)
        best = c;
    }
    c = *((u8 *) (g_BtlWalkGrid + (x + (yy * 40))));
    if (c < best)
      best = c;
    if (x < 0x27)
    {
      c = *((u8 *) ((g_BtlWalkGrid + (x + (yy * 40))) + 1));
      if (c < best)
        best = c;
    }
  }
  if (x < 0x27)
  {
    c = *((u8 *) ((((y * 40) + x) + g_BtlWalkGrid) + 1));
    if (c < best)
      best = c;
  }
  if (y < 0xB)
  {
    yy = y + 1;
    if (x != 0)
    {
      c = *((u8 *) ((((yy * 40) + x) + g_BtlWalkGrid) - 1));
      if (c < best)
        best = c;
    }
    t = best;
    if (*((u8 *) (g_BtlWalkGrid + (x + (yy * 40)))) < t)
      t = *((u8 *) (g_BtlWalkGrid + (x + (yy * 40))));
    best = t;
    if (x < 0x27)
    {
      c = *((u8 *) ((g_BtlWalkGrid + (x + (yy * 40))) + 1));
      if (c < best)
        best = c;
    }
  }
  if (x != 0)
  {
    c = *((u8 *) ((((y * 40) + x) + g_BtlWalkGrid) - 1));
    if (c < best)
      best = c;
  }
  c = *((u8 *) (g_BtlWalkGrid + (x + (y * 40))));
  if (c < best)
    return c;
  if (best < 0xFE)
    return best + 1;
  return 0xFF;
}

extern long HWD0;
extern long VWD0;
void BtlAnimationEndWait(void *record);  /* extern */
extern s32 g_BtlWalkGrid;
extern u16 g_BtlWalkerWidth;

/* func_8002DAFC */
int BtlWalkCellBlockedCheck(void)
{
  if ((*((u8 *) (((u32) g_BtlWalkGrid) + ((s16) g_BtlWalkCellIndexes[0])))) == 0xFE)
  {
    return 1;
  }
  if (g_BtlWalkBlock.walker_width_counts != 0)
  {
    g_BtlWalkCellIndexes[1] = g_BtlWalkCellIndexes[0];
    g_BtlWalkWidths[1] = g_BtlWalkerWidth;
    if (((short) g_BtlWalkWidths[1]) != 0)
    {
      do
      {
        if ((*((u8 *) (((u32) g_BtlWalkGrid) + ((s16) g_BtlWalkCellIndexes[1])))) == 0xFE)
        {
          return 1;
        }
        g_BtlWalkCellIndexes[1]++;
        g_BtlWalkWidths[1]--;
      }
      while (g_BtlWalkWidths[1] != 0);
    }
  }
  return 0;
}
