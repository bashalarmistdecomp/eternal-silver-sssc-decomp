/* Lunar SSSC — step_command: the map objects' step commands, the ten
   entries of the table at 0x800B4620 that StepCommandRun (map_think.c)
   calls. The first split had it in scripting_d.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "map.h"
#include "sprite.h"

void MapAnimationPoseSet(s32, s32, s32);
extern u8 g_MoveSpeedPixels[];
extern u8 g_MoveSpeedIntervals[];
s32 StepCommandSpeedSet(void *row);  /* TU defines this below */
s32 StepCommandWait(void *row);  /* TU defines this below */
s32 StepCommandScaleSkip(void *row);  /* TU defines this below */
void StepCommandHang();
s32 StepCommandMoveTo(void *row, u16 row_number);
s32 StepCommandPlace(void *row, u16 row_number);
s32 StepCommandSpeedSet(void *row);
s32 StepCommandWait(void *row);
s32 StepCommandScaleSkip(void *row);

/* func_8006370C */
void StepCommandHang() {
    for (;;) {
    }
}

/* func_80063714 */
s32 StepCommandPoseSet(void *row)
{
  unsigned short next_cursor;
  ;
  ((s32 (*)()) MapAnimationPoseSet)(row, *((u8 *) (((u8 *) (((MapObject *)row)->step_list + ((MapObject *)row)->move.step.cursor)) + 0x1)), ((MapObject *)row)->facing);
  if (1)
  {
 /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
    block runs once, so behaviour is the same without the `do` / `while (0)`, but then 4
    of this function's 20 instruction lines differ. */
 do { next_cursor = (s16) (((u16) (((MapObject *)row)->move.step.cursor)) + 2); } while (0);
  }
  ((MapObject *)row)->move.step.cursor = next_cursor;
  return 0;
}

/* func_80063764 */
s32 StepCommandFacingSet(void *row, s32 unused)
{
  u8 *facing;
  facing = ((*((u8 **) (((u8 *) row) + 0x5C))) + ((MapObject *)row)->move.step.cursor) + 1;
  ((MapObject *)row)->facing = *facing;
  MapAnimationPoseSet(row, -1, ((MapObject *)row)->facing & 0xFF);
  AS(u16, ((MapObject *)row)->move.step.cursor) += 2;
  return 0;
}

/* func_800637BC */
s32 StepCommandMoveTo(void *row, u16 row_number)
{
    MapObject *p = (MapObject *)row;
    s16 x;
    s32 y;
    u8 b1;
    u8 b2;
    u8 b3;

    x = p->move.step.busy;
    if (x == 0)
    {
        x = p->move.step.cursor;
        y = p->step_list + x;
        b1 = *(u8 *)(y + 1);
        p->move.step.goal_x = b1 << 3;
        b2 = *(u8 *)(y + 2);
        p->move.step.goal_y = b2 << 3;
        b3 = *(u8 *)(y + 4);
        p->move.step.busy = 1;
        p->facing_kept = b3;
    }
    return StepWalkStep(row, row_number);
}

/* Step handler 4 of g_StepCommandFunctions: op byte 0xF4 of a map object's STEP LIST
   (+0x5C, cursor +0x5A), WALK BY. No direct caller: the step dispatcher
   StepCommandRun calls g_StepCommandFunctions[(op + 0x10) & 0xFF](row, index) from the
   think pass. It is the map-object twin of the move script's MOVE BY
   (MoveCommandMoveBy, also op 0xF4) and the relative form of step handler
   3, StepCommandMoveTo (op 0xF3, WALK TO). The splitter's name is wrong about
   it: it updates no direction.

   The op is five bytes, `F4 dx dy -- facing`: dx and dy are SIGNED bytes
   in 8-pixel cells, byte 3 is not read (the move script has dz there),
   byte 4 is the facing to show.

   First think of the op (+0x56 == 0):
     cx = x (+6) / 8 + dx, cy = ground y (+8) / 8 + dy -- C's signed / 8,
     toward zero, on the s16 fields; both sums are kept as s16.
     If cx or cy is outside 0..0xFF: StepCommandHang(row, index). That is
     step handler 2 (op 0xF2), an empty endless loop -- the game hangs
     there. (The move script reports script error 0x11 instead.) Nothing
     has been stored at that point; were the call to return, the stores
     below would follow with the out-of-range cell.
     +0x4A = byte 4, read before the stores; goal +0x54 = cx * 8,
     +0x58 = cy * 8; +0x56 = 1.
   So the goal is NOT position + offset: it is the object's cell (its
   position cut to the 8-pixel grid, toward zero) plus the offset.
   Every think, first one included: return StepWalkStep(row, index), one
   think of the walk (it clears +0x56 and moves the cursor on by 5 on
   arrival; its answer goes back to the think pass through the
   dispatcher). The cursor +0x5A is not touched here.

   No global is read or written, and there is no frame wait and no random
   draw in this function itself.

   Shape. The op's bytes are read through a cursor that starts on byte 1
   and is stepped by each read; after the range test it is stepped over
   byte 3 and byte 4 is read into a local. The compiler folds the first
   two reads onto the list address ($a2 + 1, + 2) and keeps the cursor as
   $a2 + 3 across the call, reading 1($s0) after it. Indexing a second
   pointer (`q = p + 3; ... q[1]`) is the same instructions with the row
   and the cursor in each other's registers (51/67): the cursor's
   `p++` after the test is what ranks it above the row. The sums are
   written cell first (`x / 8 + byte`; byte first loads in the other
   order, 54/67), the goal is `cx * 8` (`cx << 3` is narrowed to the
   16-bit store and loses the sign extension, 62/67), and byte 4 goes
   through a local because its load comes before the three stores.
   The range test is one `||` of four compares: fold merges the first
   pair into one unsigned 16-bit test and leaves the second pair as two.
   index is a u16 (the `andi` at both calls); StepWalkStep is called
   with no prototype in scope, as StepCommandMoveTo above calls it. */
/* func_8006382C */
s32 StepCommandMoveBy(MapObject *row, u16 row_number)
{
    u8 *p;
    s16 cx;
    s16 cy;
    u8 dir;

    if (row->move.step.busy == 0) {
        p = AS(u8 *, row->step_list) + row->move.step.cursor + 1;
        cx = row->record.sprite.x / 8 + (s8)*p++;
        cy = row->record.sprite.depth / 8 + (s8)*p++;
        if (cx < 0 || cx >= 0x100 || cy < 0 || cy >= 0x100) {
            StepCommandHang(row, row_number);
        }
        p++;
        dir = *p;
        row->move.step.goal_x = cx * 8;
        row->move.step.goal_y = cy * 8;
        row->move.step.busy = 1;
        row->facing_kept = dir;
    }
    return StepWalkStep(row, row_number);
}

/* func_80063938 */
s32 StepCommandSpeedSet(void *row)
{
  u8 *intervals;
  s16 cursor;
  cursor = ((MapObject *)row)->move.step.cursor;
  intervals = g_MoveSpeedIntervals;
  ((MapObject *)row)->speed = (u8) (*((*((u8 *) (((u8 *) (((MapObject *)row)->step_list + cursor)) + 0x1))) + g_MoveSpeedPixels));
  ((MapObject *)row)->interval = (u8) (*((*((u8 *) (((u8 *) (((MapObject *)row)->step_list + cursor)) + 0x1))) + intervals));
  ((MapObject *)row)->move.step.cursor = (s16) (((u16) (((MapObject *)row)->move.step.cursor)) + 2);
  return 0;
}

/* func_80063990 */
s32 StepCommandWait(void *row)
{
  s16 wait_left;
  s16 *wait_then_cursor;
  u8 *cursor;
  cursor = ((u8 *) row) + 0x5a;
  if (((MapObject *)row)->move.step.busy == 0)
  {
    ((MapObject *)row)->move.wait_left = (u16) (*((u8 *) (((u8 *) (((MapObject *)row)->step_list + (*((s16 *) cursor)))) + 0x1)));
    ((MapObject *)row)->move.step.busy = 1;
  }
  wait_then_cursor = (s16 *)(s32 *) (((u8 *) row) + 0x54);
  wait_left = (*wait_then_cursor) - 1;
  *wait_then_cursor = wait_left;
  if ((wait_left << ((unsigned char) 0x10)) == 0)
  {
    ((MapObject *)row)->move.step.busy = 0;
    *(wait_then_cursor = (s16 *) (((u8 *) row) + 0x5a)) = (s16) (((u16) (*((s16 *) cursor))) + 2);
  }
  return 0;
}

/* func_800639F4 */
s32 StepCommandPlace(void *row, u16 row_number)
{
    MapObject *p = (MapObject *)row;
    s16 x;
    s32 y;
    u8 b1;
    u16 x2;
    s32 b2;

    x = p->move.step.cursor;
    y = p->step_list + x;
    b1 = *(u8 *)(y + 1);
    p->record.sprite.x = b1 << 3;
    x2 = AS(u16, p->move.step.cursor);
    b2 = *(u8 *)(y + 2);
    p->move.step.cursor = x2 + 3;
    p->record.sprite.depth = b2 << 3;
    p->record.sprite.y = b2 << 3;
    StepCommandFacingSet(row, row_number);
    return 1;
}

/* func_80063A4C */
s32 StepCommandScaleSkip(void *row) {
    ((MapObject *)row)->move.step.cursor = (u16) (((MapObject *)row)->move.step.cursor + 4);
    return 0;
}
int StepCommandRun(void *object_row, int row_number);

/* Step handler 9 of g_StepCommandFunctions (op byte 0xF9): put the step cursor (+0x5A)
   back to 2 and run the step dispatcher again on the same row. */
/* func_80063A64 */
s32 StepCommandRestart(void *row, u16 row_number) {
    ((MapObject *)row)->move.step.cursor = 2;
    return StepCommandRun(row, row_number);
}
