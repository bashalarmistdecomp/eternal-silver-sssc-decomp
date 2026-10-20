/* Lunar SSSC — map_think: the map objects' thinking: the pass over them,
   the wander and the turn. The first split had it in scripting_e.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"

extern s16 g_MapLimitLeft;
extern s16 g_MapLimitTop;
extern s16 g_MapLimitRight;
extern s16 g_MapLimitBottom;
extern u8 g_StepCommandFunctions[];
extern s16 g_MapViewX;
extern s16 g_MapViewY;
extern s32 g_MapObjects;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
extern s32 g_MapObjects;
s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground, MapBox *box, s32 layer);
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapAnimationPoseSet(void *, s16, u16);
s32 func_80067584(void);
int StepCommandRun(void *object_row, int row_number);
void MapAnimationSet(void *record, u16 animation);
void MapAnimationSet(void *record, u16 animation);
void MapSpotMove(int facing, int dist, u16 *spot_x, u16 *spot_y);
s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag);
s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag);
void MapAnimationPoseSet(void *, s16, u16);
void MapAnimationPoseSet(void *, s16, u16);

extern u16 g_MapObjectsThinkPhase;
extern s32 (*vtbl_4658_table[])();
s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground, MapBox *box, s32 layer);  /* extern */
void MapObjectTouchFind(void);

/* One think pass over the 0x80 map objects (the 0x60 rows of g_MapObjects),
   then MapObjectTouchFind.

   A row is skipped when it is free (+0 == 0) or +0 == 7, when its behaviour
   (+0x3D & 0x3F) is 3 or 4 -- vtbl_4658_table's two "return 0" entries --
   when it is outside the camera's neighbourhood (draw y within
   g_MapViewY - 0x64 .. + 0x144, x within g_MapViewX - 0x64 .. + 0x1A4), when
   a script slot holds it (+0 bit 0x80), or when it stands outside the world
   clip rectangle (x in g_MapLimitLeft .. g_MapLimitRight, ground y in g_MapLimitTop ..
   g_MapLimitBottom).

   The work is spread over frames by the phase counter g_MapObjectsThinkPhase, which
   this function steps 0, 1, 2, 3, 0, ...: a row thinks only on frames of its
   own parity, and a row whose +0 is below 4 only on the one frame in four
   where the counter equals (row & 3) -- so a row with +0 >= 4 thinks every
   other frame and the others every fourth.

   A row that thinks counts +0x40 down; at 0 it reloads it from +0x41 and
   calls its behaviour, vtbl_4658_table[+0x3D & 0x3F](row, index). When that
   answers non-zero the row moved: MapAttributeClassGet re-classifies its footprint
   into +0xE's bits 0x6000 and MapAnimationPoseSet(row, -1, +0x3E)
   follows.

   Matching notes: the two behaviour tests are separate statements -- joined
   by || they fold into one range test (kind - 3 < 2); the counter's wrap is
   a conditional expression (an if / else stores twice). The second pointer
   the target keeps at row + 0x3E is the loop optimiser's, not the source's. */
/* func_8006736C */
void MapObjectsThink(void)
{
    MapObject *p;
    u16 i;
    u8 live;
    s32 kind;

    p = (MapObject *)g_MapObjects;
    for (i = 0; i < 0x80; i++, p++) {
        live = p->record.on_kind.kind;
        if (live == 0 || live == 7) {
            continue;
        }
        kind = p->number & 0x3F;
        if (kind == 3) {
            continue;
        }
        if (kind == 4) {
            continue;
        }
        if (p->record.sprite.y < g_MapViewY - 0x64 || g_MapViewY + 0x144 < p->record.sprite.y) {
            continue;
        }
        if (p->record.sprite.x < g_MapViewX - 0x64 || g_MapViewX + 0x1A4 < p->record.sprite.x) {
            continue;
        }
        if (live & 0x80) {
            continue;
        }
        if ((i & 1) != (g_MapObjectsThinkPhase & 1)) {
            continue;
        }
        if (live < 4 && (i & 3) != g_MapObjectsThinkPhase) {
            continue;
        }
        if (p->record.sprite.x < g_MapLimitLeft || g_MapLimitRight < p->record.sprite.x) {
            continue;
        }
        if (p->record.sprite.depth < g_MapLimitTop || g_MapLimitBottom < p->record.sprite.depth) {
            continue;
        }
        if (--p->interval_left != 0) {
            continue;
        }
        p->interval_left = p->interval;
        if (vtbl_4658_table[kind](p, i) != 0) {
            p->record.sprite.flags = MapAttributeClassGet(1, 0x20, p->record.sprite.x, p->record.sprite.y, p->record.sprite.depth, &p->box, p->layer) | (p->record.sprite.flags & 0x9FFF);
            MapAnimationPoseSet(p, -1, p->facing);
        }
    }
    g_MapObjectsThinkPhase = (g_MapObjectsThinkPhase < 3) ? g_MapObjectsThinkPhase + 1 : 0;
    MapObjectTouchFind();
}

/* Ruling of 2026-10-14 (docs/naming.md): Returns 0; entries 3 and 4 of the objects' think table, which are never called. */
s32 func_80067584(void) {
    return 0;
}

/* MapObjectWanderStep -- entry [0] of vtbl_4658_table: map-object
   behaviour 0, the WANDER. MapObjectsThink calls it as (row, index) each
   time the row's think timer runs out; MapObjectPartyLook00Step (map_look.c, an
   entry of two other handler tables) is a thunk to it that drops the
   result.

   Facing +0x3E == 0: the object stands; returns 1 with nothing written.
   Otherwise it tries up to +0x3F one-pixel steps along the facing
   (MapSpotMove(facing, 1, &sx, &sy) on a copy of x / draw y). A step is
   refused, and the walk ends, when
     - it leaves the roaming rectangle x0 <= x < x1, y0 <= y < y1
       (+0x54 / +0x56 / +0x58 / +0x5A)                     -> +0x42 = 0x10
     - the tiles under the footprint are not free
       (MapAttributeFreeCheck(x + box.dx, y + box.dy, box.w,
       box.h, layer +0x3C, 0) == 0)                         -> +0x42 = 0x10
     - someone stands there (MapObjectOverlapFind(x, y, index) != 0: the lead
       party actor, or another live map object)             -> +0x42 += 1
   +0x42 is the blocked count: only when the walk was cut short and it has
   reached 0x10 is it cleared and a NEW FACING drawn,
   +0x3E = rng_draw() % 8 + 1 (one draw, signed remainder) -- so a
   wall or the rectangle's edge turns the object on the spot, another
   actor only after sixteen blocked thinks. A walk that ran its full
   +0x3F steps clears +0x42.

   Returns 0 when not even the first step was taken (nothing stored, the
   caller does not re-classify or re-dispatch -- but a new facing may have
   been drawn); otherwise x (+6) and both y's (+0xA, then +8) take the
   last good position and it returns 1.

   Shape: x / y are u16 registers, the probe copies s16 stack locals (the
   lh compares, the lhu copy back); the loop is a plain `for` whose three
   refusals `break`; `p->y = p->hA = y` stores +0xA first. MapObjectOverlapFind
   and rng_draw are called undeclared. */
/* func_8006758C */
s32 MapObjectWanderStep(MapObject *p, s32 row_number)
{
    u16 x;
    u16 y;
    s16 sx;
    s16 sy;
    s32 i;

    if (p->facing == 0) {
        return 1;
    }
    x = AS(u16, p->record.sprite.x);
    y = AS(u16, p->record.sprite.y);
    for (i = 0; i < p->speed; i++) {
        sx = x;
        sy = y;
        MapSpotMove(p->facing, 1, (u16 *)&sx, (u16 *)&sy);
        if (sx < p->move.bounds.left || sx >= p->move.bounds.right || sy < p->move.bounds.top || sy >= p->move.bounds.bottom) {
            p->turn_count = 0x10;
            break;
        }
        if (MapAttributeFreeCheck(sx + p->box.dx, sy + p->box.dy, p->box.half_width, p->box.half_height, p->layer, 0) == 0) {
            p->turn_count = 0x10;
            break;
        }
        if (MapObjectOverlapFind(sx, sy, row_number & 0xFFFF) & 0xFFFF) {
            p->turn_count++;
            break;
        }
        x = sx;
        y = sy;
    }
    if (i < p->speed) {
        if (p->turn_count >= 0x10) {
            p->turn_count = 0;
            p->facing = rng_draw() % 8 + 1;
        }
        if (i == 0) {
            return 0;
        }
    } else {
        p->turn_count = 0;
    }
    AS(u16, p->record.sprite.x) = x;
    AS(u16, p->record.sprite.depth) = AS(u16, p->record.sprite.y) = y;
    return 1;
}

/* far_syms: g_StepCommandFunctions (target reaches it %hi/%lo) */

/* func_80067768 */
int StepCommandRun(void *object_row, int row_number)
{
  int addr;
  int index;
  int result;
  addr = (*((int *) (((char *) object_row) + 0x5C))) + (*((short *) (((char *) object_row) + 0x5A)));
  index = ((*((unsigned char *) addr)) + 0x10) & 0xFF;
  if (index < ((unsigned int) 10))
  {
    result = ((int (*)(void *, int)) ((int *) g_StepCommandFunctions)[index])(object_row, row_number & 0xFFFF);
  }
  else
  {
    result = 0;
  }
  return result;
}

/* StepWalkStep -- ONE THINK OF A MAP OBJECT'S "WALK TO" STEP: the shared
   tail of step handlers 3 and 4 of g_StepCommandFunctions (StepCommandMoveTo, op 0xF3, goal = two tile
   bytes * 8; StepCommandMoveBy, op 0xF4), its only two callers. They set
   the goal +0x54 / +0x58, the facing to show +0x4A (op byte 4) and
   +0x56 = 1 on the op's first think, then call this as (row, index) every
   think and return its answer to StepCommandRun and so to the think pass
   MapObjectsThink.

   In order:
     1. +0x3E = MapFacingTowardGet(x, ground y, goal x, goal y, +0x3E): the
        facing that heads for the goal, nearest the current one. It is 0
        when the row already stands on the goal or its facing is 0, and the
        0 is stored like any other answer.
     2. A copy of x / GROUND y (+6 / +8; the wander steps the draw y +0xA)
        is moved +0x3F pixels along that facing by MapSpotMove -- which
        indexes its tables with (facing - 1) & 7, so facing 0 steps up-left
        like facing 8. The tiles are not tested here.
     3. r = MapObjectOverlapFind(copy x, copy y, index): is someone standing there
        (1 = the lead, i + 2 = map object i, 0 = nobody). When nobody is,
        the step is taken: +6 = x, +0xA and +8 = y (draw y first).
     4. Arrived -- x == goal x and ground y == goal y, both exactly, tested
        whether or not the step was taken: +0x56 = 0, the step cursor +0x5A
        += 5 (this op's length), and +0x3E = +0x4A when +0x4A is not 0.
     5. MapAnimationPoseSet(row, -1, +0x4A when it is not 0, else +0x3E):
        the animation is refreshed every think, showing the fixed facing
        when the op gave one.
     6. Returns r & 0xFFFF: MapObjectOverlapFind's answer -- NON-ZERO WHEN THE
        STEP WAS BLOCKED, 0 when it was taken.

   No RNG draw and no frame wait (MapFacingTowardGet, MapSpotMove and
   MapObjectOverlapFind / MapBoxTouchGet are plain arithmetic; MapAnimationPoseSet
   only re-seats the animation through MapAnimationSet / MapAnimationStart).

   Shape: the two copies are s16 stack locals loaded from u16 fields (lhu,
   then the sll / sra of the forwarded store for the first call, lh for the
   third, lhu for the copy back), and MapFacingTowardGet and MapObjectOverlapFind are
   called undeclared, as MapObjectWanderStep calls the latter. `r` keeps the raw
   answer and is masked at both uses. The facing shown is a u16 LOCAL set
   from a ?: (an if / else is the same bytes): the arm's byte is then loaded
   in HImode, which the post-reload CSE cannot replace with the SImode copy
   the test left in $v0, and the call is ONE call after the join. An int or
   u8 local, or the ?: written as the argument, loses the second
   `lbu 0x4A` (66/72); two whole calls in an if / else keep it but load it
   through $a0 (70/72). */
/* func_800677D4 */
s32 StepWalkStep(MapObject *p, s32 row_number)
{
    s16 sx;
    s16 sy;
    s32 r;
    u16 dir;

    sx = AS(u16, p->record.sprite.x);
    sy = AS(u16, p->record.sprite.depth);
    p->facing = MapFacingTowardGet(sx, sy, p->move.step.goal_x, p->move.step.goal_y, p->facing);
    MapSpotMove(p->facing, p->speed, (u16 *)&sx, (u16 *)&sy);
    r = MapObjectOverlapFind(sx, sy, row_number & 0xFFFF);
    if ((r & 0xFFFF) == 0) {
        AS(u16, p->record.sprite.x) = sx;
        AS(u16, p->record.sprite.depth) = AS(u16, p->record.sprite.y) = sy;
    }
    if ((s16)AS(u16, p->record.sprite.x) == p->move.step.goal_x && (s16)AS(u16, p->record.sprite.depth) == p->move.step.goal_y) {
        p->move.step.busy = 0;
        AS(u16, p->move.step.cursor) += 5;
        if (p->facing_kept != 0) {
            p->facing = p->facing_kept;
        }
    }
    dir = p->facing_kept != 0 ? p->facing_kept : p->facing;
    MapAnimationPoseSet(p, -1, dir);
    return r & 0xFFFF;
}

/* MapObjectTurnStep(row, index) -- entry [5] of vtbl_4658_table (the word
   at 0x800B466C; `overworld_state_table` is the splitter's label for
   entries 5..8 of that one table): map-object behaviour 5, STAND AND TURN
   AT RANDOM. Nothing calls it by name; the think pass MapObjectsThink calls
   vtbl_4658_table[row +0x3D & 0x3F](row, index) each time the row's think
   timer (+0x40) runs out. The index is not read. The name is the
   splitter's, and it shares nothing with MapObjectPartyLookStep..03
   (map_look.c), which are entries 6..8.

   Each call counts the byte +0x42 up (the wander MapObjectWanderStep's blocked
   count; here a plain think count) and returns 0 while it is below 0x10.
   From 0x10 on:
     - a row whose kind byte +0 gives 3 waits for its animation: while
       frame + 1 < end (+0x38 / +0x3A as u16, i.e. the animation is not on
       its last frame) it returns 0 with the count left as it is, so the
       test is made again at every think until the frame comes round (and
       if the byte wraps past 0xFF, sixteen thinks later);
     - then ONE RNG draw: dir = rng_draw() % 8 + 1 (a signed
       remainder: 1..8 for a non-negative draw), and +0x42 = 0;
     - kind (+0 & 0x7F) != 3: the facing +0x3E = dir and
       MapAnimationPoseSet(row, 1, +0x3E) -- pose group 0 turned to
       that facing (animation id 1..4);
     - kind 3: MapAnimationSet(row, dir) -- animation id `dir` itself, 1..8
       (group 0 or group 1, any column); the facing byte is NOT written.
   Always returns 0, so the think pass never re-classifies the footprint
   or re-dispatches the animation after it. The object does not move. No
   frame wait: the callees are rng_draw, MapAnimationPoseSet ->
   MapAnimationSet -> MapAnimationStart, none of which waits.

   The first kind test is written with a REMAINDER, `+0 % 0x7F == 3` (an
   unsigned divide by 127: multu by 0x02040811), where the second masks,
   `(+0 & 0x7F) != 3` -- the original's own slip, kept. The two differ
   only with bit 0x80 set (0x82 % 0x7F is 3, 0x83 % 0x7F is 4), and the
   think pass never calls a behaviour for a row with that bit (bound to a
   script slot), so by this path they agree.

   Source shapes the bytes need: the remainder on the u8 member itself (C
   does it in unsigned char: the `multu` and the `andi 0xFF` after it); the
   dispatch's third argument RE-READ from the row after the store, which
   is where the target's `sb $a1, 0x3E($a0)` through the argument register
   and its `andi $a2, $a1, 0xFF` come from (`dir & 0xFF`, `(u8)dir` or
   plain `dir` there: 59/64); `dir` an int (a u8 local: 53/64; u16 gives
   the same bytes). One `return 0` at each early exit and one at the end:
   reorg copies the `move $v0, $zero` into every branch's delay slot. The
   second parameter is the caller's (row, index) convention and compiles
   to nothing. MapObject is this function's own typedef name. */
/* func_800678F4 */
s32 MapObjectTurnStep(MapObject *p, s32 unused)
{
    s32 dir;

    if (++p->turn_count < 0x10) {
        return 0;
    }
    if (p->record.on_kind.kind % 0x7F == 3 && p->record.step + 1 < p->record.end) {
        return 0;
    }
    dir = rng_draw() % 8 + 1;
    p->turn_count = 0;
    if ((p->record.on_kind.kind & 0x7F) != 3) {
        p->facing = dir;
        MapAnimationPoseSet(p, 1, p->facing);
    } else {
        MapAnimationSet(p, dir);
    }
    return 0;
}
