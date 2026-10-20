/* Lunar SSSC — lns_move: the moving characters' rows: made live, stepped
   (walk, view, second view, scale), their goals and follows, the party's
   join and leave, the command run and MoveChrAction. The name is the
   developers' own: LNS_MOVE.C, the source file MoveChrAction's assertion
   names (its message names MoveChrAction itself); the file's other
   functions have it by being of its group. The first split had it in
   scripting_d.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_MapTwinSpriteDataNumbers */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "map.h"
#include "sprite.h"

/* at the top of the file: MoveChrSecondViewStep's prototype, well above
   its definition, names MoveChrRow */
void MoveChrScaleStep(void *);                        /* extern */
s32 MapPartyRingEntryGet(s32, s32 *);
void PartyPlacesSwap();
void PartyLevelLeastSet();
void MoveChrFollowRestart();
extern s32 g_MapObjectDefinitions;
s32 ScriptErrorReport();
extern s32 g_MoveChrLiveBits;
extern s32 g_PartyIds;
void MapAnimationPoseSet(s32, s32, s32);
extern u8 g_MoveSpeedPixels[];
extern u8 g_MoveSpeedIntervals[];
extern s16 g_MapSecondViewX;
extern s16 g_MapSecondViewY;
extern u8 g_MoveChrPartyFollow;
extern u8 g_MoveChrAttributeCheckOn;
extern u8 g_MoveChrSecondViewFollow;
/* far_syms: g_MapPartyBoxDx (target reaches it %hi/%lo) */
s32 mem_clear(s32, s32);
extern s32 g_MapSpriteDataMarks;
void MapSpriteDataMarksClear(void);  /* TU defines this below */
extern u8 g_MapTwinSpriteDataNumbers[];
void MapSpriteDataMarkSet(s32 sprite_data_number, u8 mark);  /* TU defines this below */
s16 value_goal_step(s16, s16, s32);
extern s16 g_MapSecondViewKeptX;
extern s16 g_MapSecondViewKeptY;
void MoveChrSecondViewStep(MoveChrRow *row);  /* TU defines this below */
extern s16 g_ScriptErrorNumber;
extern s8 g_MapPartyBoxDx;
extern u16 g_MapPartyKind;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern u16 g_MapNumber;
extern s16 g_MapViewKeptX;
extern s16 g_MapViewKeptY;
extern u16 g_MapEffect;
extern s32 g_MoveChrBusyBits;
extern void *g_MoveChrRows;
void MoveChrSecondViewFollowSet(void);  /* TU defines this below */
void MoveChrAction();
void MoveChrCommandRun();
void MoveChrUpdate(void);  /* TU defines this below */
void MoveChrCommandEndWait(s32);
void MoveChrFollowStart(s32);
extern u8 g_MoveChrViewFollow;
void MoveChrRowInit(s32);
void StoryBit2CAUpdate(void);
extern s32 g_MapPartyRecords;
extern s32 g_MapPartyTwins;
extern s32 g_MoveChrMixGroup1Bits;
extern s32 g_MoveChrMixGroup2Bits;
extern s32 g_MoveChrOwnActors;
extern s32 g_MoveChrOwnActorTwins;

/* func_8005F1C8 */
void MoveChrInit(void)
{
  s32 twin_record;
  u8 buf[8];
  g_MoveChrOwnActors = 0x801C9B78;
  g_MoveChrOwnActorTwins = 0x801C9DB8;
  g_MoveChrRows = 0x801C9F20;
  mem_clear(0x801C9B78, 0x708);
  g_MoveChrSecondViewFollow = 1;
  g_MoveChrAttributeCheckOn = 1;
  g_MoveChrPartyFollow = 1;
  g_MoveChrViewFollow = 0;
  g_MoveChrMixGroup2Bits = 0;
  g_MoveChrMixGroup1Bits = 0;
  g_MoveChrBusyBits = 0;
  g_MoveChrLiveBits = 0;
  layer_mix_set(0x4000, 0);
  layer_mix_set(0x8000, 0);
  MoveChrRowInit(0);
  MoveChrRowInit(1);
  MapPartyRingEntryGet(0, buf);
  *(((u8 *) g_MoveChrRows) + 0x33) = *((u8 *) g_PartyIds);
  twin_record = g_MapPartyTwins;
  AS(s32, ((MoveChrRow *)g_MoveChrRows)[1].actor) = g_MapPartyRecords;
  AS(s32, ((MoveChrRow *)g_MoveChrRows)[1].twin) = twin_record;
  AS(s32, ((MoveChrRow *)g_MoveChrRows)[1].box) = (s32) (&g_MapPartyBoxDx);
  *(((u8 *) g_MoveChrRows) + 0x51) = buf[5];
  MapSpriteDataMarksClear();
  sprite_run_off(6, 0xB);
  sprite_run_off(0x4C, 0xB);
}

s32 mem_clear(s32, s32);                            /* extern */
extern s32 g_MapSpriteDataMarks;

/* func_8005F2C4 */
void MapSpriteDataMarksClear(void) {
    mem_clear(g_MapSpriteDataMarks, 0x100);
}

extern s32 g_MapSpriteDataEntries;

/* func_8005F2E8 */
void MoveChrEnd(void) {
    u16 sprite_data_number;
    s32 entries;
    s32 marks;

    sprite_data_number = 0;
    marks = g_MapSpriteDataMarks;
    entries = g_MapSpriteDataEntries;
    do {
        if (*(u8 *)((u8 *)marks + sprite_data_number) == 1) {
            *(s32 *)((sprite_data_number << 4) + entries) = 0;
            *(s32 *)((sprite_data_number << 4) + entries + 8) = 0;
            *(s32 *)((sprite_data_number << 4) + entries + 12) = 0;
        }
        sprite_data_number += 1;
    } while (sprite_data_number < 0x100);
    g_MoveChrLiveBits = 0;
}

extern u8 g_MapTwinSpriteDataNumbers[];

/* func_8005F340 */
void MapSpriteDataMarkSet(s32 sprite_data_number, u8 mark)
{
    if (sprite_data_number < 0x1F)
    {
        s32 v = sprite_data_number / 15;
        s32 r = sprite_data_number - v * 15;
        *(u8 *)(g_MapSpriteDataMarks + r) = mark;
        *(u8 *)(r + g_MapSpriteDataMarks + 0xF) = mark;
        *(u8 *)(g_MapSpriteDataMarks + g_MapTwinSpriteDataNumbers[r - 1]) = mark;
    }
    else
    {
        *(u8 *)(g_MapSpriteDataMarks + sprite_data_number) = mark;
    }
}

/* func_8005F3C8 */
void MoveChrCommandEndWait(s32 row_mask)
{
  s32 v;
  v = row_mask;
  MapStatusClear();
  while (v & g_MoveChrBusyBits)
  {
    ScriptFrameStep(1);
  }

  return 0;
}

/* func_8005F410 */
void MoveChrRowInit(s32 row_number)
{
  s32 masked_row_number;
  s32 row_offset;
  /* CODEGEN FORCING DEVICE (store nothing reads), not recovered source:
     `row_facing` is assigned twice (the same address) and never read; behaviour is the same
     without it, but then 43 of this function's 68 instruction lines differ. */
  u8 *row_facing;
  void *row;
  void *row_again;
  masked_row_number = row_number & 0xFF;
  g_MoveChrLiveBits |= 1 << masked_row_number;
  row_offset = masked_row_number * 0x30;
  *((u8 *) (row_offset + ((s32)g_MoveChrRows))) = 1;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x1)) = 0;
  *((s8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x2)) = 0;
  row = row_offset + ((s32)g_MoveChrRows);
  ((MoveChrRow *)row)->speed = 1;
  row_facing = (u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x21);
  AS(s32, ((MoveChrRow *)row)->command) = 0;
  AS(s32, ((MoveChrRow *)row)->command_list) = 0;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x1f)) = 1;
  row_again = row_offset + ((s32)g_MoveChrRows);
  ((MoveChrRow *)row_again)->layer = 0;
  row_facing = (u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x21);
  AS(s32, ((MoveChrRow *)row_again)->actor) = 0;
  AS(s32, ((MoveChrRow *)row_again)->twin) = 0;
  AS(s32, ((MoveChrRow *)row_again)->box) = 0;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x21)) = 0;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x23)) = 0;
  *((s8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x24)) = 1;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x25)) = 1;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x26)) = 4;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x27)) = 4;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x28)) = 0;
  *((u8 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x29)) = 0;
}
void MoveChrViewStep(MoveChrRow *row);  /* TU defines this below */
s32 MapFacingTowardGet(s32 x0, s32 y0, s32 x1, s32 y1, u16 facing);
s32 MoveChrGoalCheck();
s32 MapSpotMoveTry();
s32 MapPartyRingPush();
s32 MapPartyRingFollow(void);
void MapPartyTwinSpotSet(s32 place, void *spot);
s32 MapAttributeClassGet();

/* One tick of script mover slot `row_number` (a 0x30-byte g_MoveChrRows row): the
   +0x24 countdown gates it to one step every +0x25 frames. Slot 0 is the
   camera (MoveChrViewStep) and slot 0x11 the second scroll (MoveChrSecondViewStep).
   Any other slot walks its actor (+0xC) up to +0x1E passes: each pass
   re-aims the facing +0x22 at the goal (+0x18, +0x1A) through MapFacingTowardGet
   while it still equals +0x21, steps (x, y) by 1 along it
   (MapSpotMoveTry, probe +0x14, layer +0x20, last argument
   g_MoveChrAttributeCheckOn == 0) and keeps a non-zero answer in +0x21, and asks
   MoveChrGoalCheck(slot, &x, &y, &z, answer) whether to stop; z is +2 stepped
   toward +0x1C by value_goal_step (step +0x1F) before the loop and written
   back to +2 every pass.
     Slot 1: MapPartyRingPush(x, y, z, facing -- +0x23 when that is set);
       then MapPartyRingFollow() when g_MoveChrPartyFollow is set, otherwise party spot 0
       (MapPartyRingEntryGet) becomes the actor's x, y, +0xA = y - spot byte 4 and
       +0xE's 0x6000 bits, and is stored back as spot 0 (MapPartyTwinSpotSet).
     Other slots: the actor gets x, y and +0xA = y - z, MapAttributeClassGet
       re-classifies it into +0xE's 0x6000 bits, and below slot 7 the
       result is stored as party spot row_number - 1 (x, y, z, +0x21, the bits).
   A stop sets +1 = 0, advances the script pointer +8 by 5 and copies a
   non-zero +0x23 into +0x21. The actor is then dispatched
   (MapAnimationPoseSet(actor, -1, ...)) with +0x23, or +0x21 when
   +0x23 is 0.

   Shape: MoveChrViewStep(row) emits no `move $a0` (row is still in $a0 and
   no label precedes the call); MoveChrSecondViewStep's, after a label, does.
   MapPartyRingFollow is called with NO argument: the `move $a0, $zero` in the
   branch's delay slot is MapPartyRingEntryGet's, pulled out of the other arm.
   spot.b5 is assigned before spot.h6 (the +0x21 load precedes both
   stores). The spot's fields are re-read after every actor store; x and y,
   plain u16 locals, are not. */
/* func_8005F520 */
void MoveChrWalkStep(MoveChrRow *row, u16 row_number)
{
    MapPartyRingEntry spot;
    u16 x;
    u16 y;
    s16 z;
    s32 i;
    s32 done;
    u16 dir;
    s32 flag;

    if (--row->interval_left != 0) {
        return;
    }
    row->interval_left = row->interval;
    if (row_number == 0) {
        MoveChrViewStep((MoveChrRow *)row);
        return;
    }
    if (row_number == 0x11) {
        MoveChrSecondViewStep((MoveChrRow *)row);
        return;
    }
    x = AS(u16, row->actor->sprite.x);
    y = AS(u16, row->actor->sprite.depth);
    z = row->height;
    z = value_goal_step(z, row->height_wait.goal_height, row->height_step);
    for (i = 0; i < row->speed; i++) {
        if (row->aim == row->facing) {
            row->aim = MapFacingTowardGet((s16)x, (s16)y, AS(u16, row->goal_x), AS(u16, row->goal_y), row->facing);
        }
        dir = MapSpotMoveTry(row->aim, 1, &x, &y, AS(void *, row->box), row->layer, g_MoveChrAttributeCheckOn == 0);
        done = MoveChrGoalCheck(row, &x, &y, &z, dir);
        if (dir != 0) {
            row->facing = dir;
        }
        if (row_number == 1) {
            if (row->facing_kept != 0) {
                dir = row->facing_kept;
            }
            MapPartyRingPush((s16)x, (s16)y, z, dir);
            if (g_MoveChrPartyFollow != 0) {
                MapPartyRingFollow();
            } else {
                MapPartyRingEntryGet(0, (s32 *)&spot);
                row->actor->sprite.x = spot.x;
                row->actor->sprite.y = spot.y - spot.height;
                row->actor->sprite.depth = spot.y;
                row->actor->sprite.flags = spot.class | (row->actor->sprite.flags & 0x9FFF);
                MapPartyTwinSpotSet((u16)(row_number - 1), &spot);
            }
        } else {
            row->actor->sprite.x = x;
            row->actor->sprite.y = y - z;
            row->actor->sprite.depth = y;
            flag = MapAttributeClassGet(row->speed, 0x20, (s16)x, (s16)y - z, y, AS(void *, row->box), row->layer);
            row->actor->sprite.flags = flag | (row->actor->sprite.flags & 0x9FFF);
            if (row_number < 7) {
                spot.x = x;
                spot.y = y;
                spot.height = z;
                spot.facing = row->facing;
                spot.class = flag;
                MapPartyTwinSpotSet((u16)(row_number - 1), &spot);
            }
        }
        row->height = z;
        if (done != 0) {
            row->running = 0;
            AS(s32, row->command) += 5;
            if (row->facing_kept != 0) {
                row->facing = row->facing_kept;
            }
            break;
        }
    }
    if (row->facing_kept != 0) {
        dir = row->facing_kept;
    } else {
        dir = row->facing;
    }
    MapAnimationPoseSet((s32)row->actor, -1, dir);
}

extern s16 g_MapViewX;
extern s16 g_MapViewY;

/* Step a mover toward (+0x18, +0x1A) at speed +0x1E through
   value_goal_step, keeping the position in g_MapViewX / g_MapViewY; on
   reaching g_MapViewKeptX / g_MapViewKeptY or the goal, stop it (+1 = 0) and
   advance its script pointer (+8 += 5). Sibling of MoveChrSecondViewStep. */
/* func_8005F8A4 */
void MoveChrViewStep(MoveChrRow *row)
{
    s16 view_y;

    g_MapViewX = value_goal_step(g_MapViewX, row->goal_x, row->speed);
    view_y = value_goal_step(g_MapViewY, row->goal_y, row->speed);
    g_MapViewY = view_y;
    if (((g_MapViewKeptX == g_MapViewX) && (g_MapViewKeptY == view_y)) || (((u16) row->goal_x == g_MapViewX) && ((u16) row->goal_y == view_y)))
    {
        row->running = 0;
        AS(s32, row->command) = AS(s32, row->command) + 5;
    }
}

s16 value_goal_step(s16, s16, s32);                   /* extern */
extern s16 g_MapSecondViewKeptX;
extern s16 g_MapSecondViewKeptY;

/* func_8005F948 */
void MoveChrSecondViewStep(MoveChrRow *row) {
    s16 second_view_y;

    g_MapSecondViewX = value_goal_step(g_MapSecondViewX, row->goal_x, row->speed);
    second_view_y = value_goal_step(g_MapSecondViewY, row->goal_y, row->speed);
    g_MapSecondViewY = second_view_y;
    if (((g_MapSecondViewKeptX == g_MapSecondViewX) && (g_MapSecondViewKeptY == second_view_y)) || (((u16) row->goal_x == g_MapSecondViewX) && ((u16) row->goal_y == second_view_y))) {
        row->running = 0;
        AS(s32, row->command) = AS(s32, row->command) + 5;
    }
}

void MoveChrScaleSet(u8 *row);  /* extern */

/* func_8005F9EC */
void MoveChrScaleStep(void *row)
{
  s8 x_not_reached;
  short y_scale_then_not_reached;
  u8 countdown;
  u8 x_direction;
  u8 y_direction;
  void *y_shrink_twin;
  void *x_grow_actor;
  void *x_grow_twin;
  void *x_shrink_actor;
  void *x_shrink_twin;
  void *y_grow_actor;
  void *y_grow_twin;
  void *y_shrink_actor;
  if ((AS(u16, ((MoveChrRow *)row)->scale_x_direction) != 0) && ((countdown = ((MoveChrRow *)row)->scale_left - 1, ((MoveChrRow *)row)->scale_left = countdown, (countdown & 0xFF) == 0)))
  {
    x_direction = (u8) (AS(s32, ((MoveChrRow *)row)->scale_x_direction));
    if (x_direction == 2)
    {
      x_grow_actor = AS(s32, ((MoveChrRow *)row)->actor);
             AS(u16, ((MapRecord *)x_grow_actor)->sprite.scale_x) += 0xC;
      x_grow_twin = AS(s32, ((MoveChrRow *)row)->twin);
             AS(u16, ((MapRecord *)x_grow_twin)->sprite.scale_x) += 0xC;
      x_not_reached = ((s16) (*((s32 *) (((u8 *) (AS(s32, ((MoveChrRow *)row)->actor))) + 0x24)))) < ((MoveChrRow *)row)->scale_x_goal;
      goto block_6;
    }
    if (x_direction == 1)
    {
      x_shrink_actor = AS(s32, ((MoveChrRow *)row)->actor);
             AS(u16, ((MapRecord *)x_shrink_actor)->sprite.scale_x) -= 0xC;
      x_shrink_twin = AS(s32, ((MoveChrRow *)row)->twin);
             AS(u16, ((MapRecord *)x_shrink_twin)->sprite.scale_x) -= 0xC;
                           x_not_reached = ((s16) (*((s32 *) (((u8 *) (AS(s32, ((MoveChrRow *)row)->actor))) + 0x24)))) > ((MoveChrRow *)row)->scale_x_goal;
      block_6:
      if (x_not_reached == 0)
      {
        ((MoveChrRow *)row)->scale_x_direction = 0;
      }

    }
    y_direction = ((MoveChrRow *)row)->scale_y_direction;
    if (y_direction == 2)
    {
      y_grow_actor = AS(s32, ((MoveChrRow *)row)->actor);
      ((MapRecord *)y_grow_actor)->sprite.scale_y = (u16) (((MapRecord *)y_grow_actor)->sprite.scale_y + 0xC);
      y_grow_twin = AS(s32, ((MoveChrRow *)row)->twin);
      ((MapRecord *)y_grow_twin)->sprite.scale_y = (u16) (((MapRecord *)y_grow_twin)->sprite.scale_y + 0xC);
      y_scale_then_not_reached = ((s16) (*((s16 *) (((u8 *) (AS(s32, ((MoveChrRow *)row)->actor))) + 0x26)))) < ((MoveChrRow *)row)->scale_y_goal;
      goto block_12;
    }
    if (y_direction == 1)
    {
      y_shrink_actor = AS(s32, ((MoveChrRow *)row)->actor);
      ((MapRecord *)y_shrink_actor)->sprite.scale_y = (u16) (((MapRecord *)y_shrink_actor)->sprite.scale_y - 0xC);
      y_shrink_twin = AS(s32, ((MoveChrRow *)row)->twin);
      ((MapRecord *)y_shrink_twin)->sprite.scale_y = (u16) (((MapRecord *)y_shrink_twin)->sprite.scale_y + (-0xC));
      y_scale_then_not_reached = (s16) (*((s16 *) (((u8 *) (AS(s32, ((MoveChrRow *)row)->actor))) + 0x26)));
      y_scale_then_not_reached = y_scale_then_not_reached > ((MoveChrRow *)row)->scale_y_goal;
      block_12:
      if (y_scale_then_not_reached == 0)
      {
        ((MoveChrRow *)row)->scale_y_direction = 0U;
      }

    }
    if (AS(u16, ((MoveChrRow *)row)->scale_x_direction) == 0)
    {
             MoveChrScaleSet(row);
      return;
    }
    ((MoveChrRow *)row)->scale_left = (u8) (AS(s16, ((MoveChrRow *)row)->scale_period));
  }
}
/* The facing to turn to. An actor at (x0, y0) that faces `facing` wants to
   head for (x1, y1): the answer is one of the eight facings, 1..8 clockwise
   from up (y grows downward: 1 up, 3 right, 5 down, 7 left), or 0 for "no
   answer" -- the two points are equal, or `facing` is 0.

     same x            5 when the goal is below (y0 < y1), else 1
     same y            3 when the goal is to the right (x0 < x1), else 7
     |dx| == |dy|      the exact diagonal: 4 / 2 right of the actor (below /
                       above), 6 / 8 left of it

   Anywhere else the bearing lies between two neighbouring facings and the
   CURRENT facing picks one of them -- the clockwise one when the current
   facing is one of the four facings clockwise from the bearing, the other
   one otherwise, i.e. the one nearer to the current facing round the circle:

     flatter than the diagonal (|dy| < |dx|)
       right, below    between 3 and 4    facing 4..7 -> 4, else 3
       right, above    between 2 and 3    facing 3..6 -> 3, else 2
       left,  below    between 6 and 7    facing 3..6 -> 6, else 7
       left,  above    between 7 and 8    facing 4..7 -> 7, else 8
     steeper (|dy| > |dx|)
       below, right    between 4 and 5    facing 1..4 -> 4, else 5
       below, left     between 5 and 6    facing 2..5 -> 5, else 6
       above, right    between 1 and 2    facing 2..5 -> 2, else 1
       above, left     between 8 and 1    facing 1..4 -> 1, else 8

   A facing above 8 takes the "else" answer everywhere. No call, no store,
   no global read. Callers: the mover tick MoveChrWalkStep (while a slot's aim
   +0x22 still equals its facing +0x21), StepWalkStep, and nine functions
   in map_look.c (eight) and map_touch.c (one) that steer an actor toward a
   point or toward the lead.

   Shape: |dx| and |dy| are `d >= 0 ? d : -d` (fold makes it ABS_EXPR, one
   abssi2 insn), each taken right after its own difference and each
   difference in a local of its own -- so the first has source and
   destination in two registers (bgez / move in the slot / negu) and the
   second in one (bgez / nop / negu). `facing` is a u16: `lhu` from the
   fifth-argument slot and an `andi` copy for the plain tests (== 0, < 5),
   while the range tests subtract from the raw register. */
/* func_8005FBBC */
s32 MapFacingTowardGet(s32 x0, s32 y0, s32 x1, s32 y1, u16 facing)
{
    s32 dx;
    s32 dy;
    s32 adx;
    s32 ady;

    if (x0 == x1 && y0 == y1) {
        return 0;
    }
    if (facing == 0) {
        return 0;
    }
    if (x0 == x1) {
        if (y0 < y1) {
            return 5;
        }
        return 1;
    }
    if (y0 == y1) {
        if (x0 < x1) {
            return 3;
        }
        return 7;
    }
    dx = x0 - x1;
    adx = dx >= 0 ? dx : -dx;
    dy = y0 - y1;
    ady = dy >= 0 ? dy : -dy;
    if (adx == ady) {
        if (x0 < x1) {
            if (y0 < y1) {
                return 4;
            }
            return 2;
        }
        if (y0 < y1) {
            return 6;
        }
        return 8;
    }
    if (ady < adx) {
        if (x0 < x1) {
            if (y0 < y1) {
                if (facing >= 4 && facing < 8) {
                    return 4;
                }
                return 3;
            }
            if (facing >= 3 && facing < 7) {
                return 3;
            }
            return 2;
        }
        if (y0 < y1) {
            if (facing >= 3 && facing < 7) {
                return 6;
            }
            return 7;
        }
        if (facing >= 4 && facing < 8) {
            return 7;
        }
        return 8;
    }
    if (y0 < y1) {
        if (x0 < x1) {
            if (facing < 5) {
                return 4;
            }
            return 5;
        }
        if (facing >= 2 && facing < 6) {
            return 5;
        }
        return 6;
    }
    if (x0 < x1) {
        if (facing >= 2 && facing < 6) {
            return 2;
        }
        return 1;
    }
    if (facing < 5) {
        return 1;
    }
    return 8;
}
/* Step `cur` toward `goal` by `step` without passing it: above the goal
   the answer is cur - step, or the goal when that has gone below it;
   below the goal cur + step, or the goal when that has gone above it;
   at the goal, cur as it is. The three values are compared as signed
   16-bit and the sum is cut to 16 bits before the overshoot test, so the
   answer is an s16 whatever `step` is.
   A pure function: no global, nothing read or written through a pointer,
   no call, so no frame wait and no random draw.
   Callers, all in this file: the mover tick MoveChrWalkStep (a slot's
   height +2 toward +0x1C by the byte +0x1F, once per tick ahead of the
   passes), the camera slot MoveChrViewStep (g_MapViewX / g_MapViewY toward
   the goal +0x18 / +0x1A by the byte +0x1E) and the second scroll
   MoveChrSecondViewStep (g_MapSecondViewX / g_MapSecondViewY, the same two fields).

   Shape. The first test has the current value on the LEFT (`cur >
   goal`): the two parameter conversions then come out in parameter order
   (cur's sll / sra into $t0, goal's into $v1) and the stepped value takes
   $a3. `goal < cur`, the same slt, converts goal first and rotates the
   three registers, 15/27; each of the other three tests turned round
   alone is the same bytes. `step` is as wide as an int: the target
   subtracts and adds the raw $a2, and a u8 parameter is masked in the
   callee (22/27; u32 is the same bytes as s32). The TU's two declarations
   say s32 too -- every caller hands it a byte it has just loaded with
   lbu, so their code is the same under u8 or s32. cur is stepped in
   place; a separate s16 local set to cur first is the same 27
   instructions. */
/* func_8005FD40 */
s16 value_goal_step(s16 cur, s16 goal, s32 step)
{
    if (cur > goal) {
        cur -= step;
        if (cur < goal) {
            cur = goal;
        }
    } else if (cur < goal) {
        cur += step;
        if (cur > goal) {
            cur = goal;
        }
    }
    return cur;
}

/* Has mover slot `row` arrived? The one caller is the mover tick
   MoveChrWalkStep, right after MapSpotMoveTry has stepped (*px, *py)
   and answered `dir`, the facing it stepped along (0 = it answered none);
   *pz is the height value_goal_step has already stepped toward +0x1C. The
   answer is 1 = stop the mover, 0 = keep going, and on the way the
   position is CLAMPED onto the goal (+0x18, +0x1A) one axis at a time.

     done = (*pz == +0x1C), the goal height, read unsigned.
     dir == 0             return done: with no step taken only the height
                          decides.
     +0x22 != dir         return 0: the step did not go where the slot
                          aims, so it has not arrived whatever the height.
     otherwise, by dir (1..8 clockwise from up, y growing downward):
       y axis   1, 2, 8 (upward)     *py <= goal y -> *py = goal y
                4, 5, 6 (downward)   *py >= goal y -> *py = goal y
                3, 7                 y is not looked at
       x axis   2, 3, 4 (rightward)  *px >= goal x -> *px = goal x
                6, 7, 8 (leftward)   *px <= goal x -> *px = goal x
                1, 5                 x is not looked at
     an axis still short of its goal clears done; one at or past it is
     set to the goal and leaves done alone. A dir above 8 touches neither
     axis and returns the height answer.

   So a straight facing (1, 3, 5, 7) tests ONE axis: a mover heading right
   stops at the goal's x whatever its y is. The other axis is the aim's
   business (MapFacingTowardGet re-aims every pass while +0x22 == +0x21).
   The coordinates are compared as signed 16-bit against the goal words
   read unsigned. Stores: *py and *px only -- nothing in the slot, no
   global. No call, so no frame wait and no random draw.

   Shape. The flag is cleared and then set under an `if`; written as
   `done = *pz == ...;` the compare result takes pz's register and three
   rows move. The slot's +0x1C is an s16 in MoveChrRow (the caller hands it to
   an s16 parameter) and is read here with `lhu`, hence the cast. Both
   switches are written with plain case labels on `dir`: the y switch
   compiles to the jump table (jtbl_80012BB0, eight rows for dir 1..8) and
   the x switch, whose six labels are two runs of three, to a tree of four
   compares. After the `!=` test the compiler takes the table index from
   the +0x22 byte it has just loaded and re-masks dir for the tree;
   `switch (row->b22)` for the first one gives the same bytes.
   The definition is K&R because the TU declares it `s32 MoveChrGoalCheck();`
   above its caller: a u16 promotes to int, so that declaration stays
   valid and MoveChrWalkStep's call does not change.
   Spelled with a prototype, and the declaration above made
   (MoveChrRow *, s16 *, s16 *, s16 *, u16), the whole TU's code is the same
   bytes too -- the caller's x and y are u16 locals handed to s16 *. */
/* func_8005FDAC */
s32 MoveChrGoalCheck(row, px, py, pz, dir)
    MoveChrRow *row;
    s16 *px;
    s16 *py;
    s16 *pz;
    u16 dir;
{
    s32 done;

    done = 0;
    if (*pz == (u16)row->height_wait.goal_height) {
        done = 1;
    }
    if (dir == 0) {
        return done;
    }
    if (row->aim != dir) {
        return 0;
    }
    switch (dir) {
    case 1:
    case 2:
    case 8:
        if (*py <= AS(u16, row->goal_y)) {
            *py = AS(u16, row->goal_y);
        } else {
            done = 0;
        }
        break;
    case 4:
    case 5:
    case 6:
        if (*py >= AS(u16, row->goal_y)) {
            *py = AS(u16, row->goal_y);
        } else {
            done = 0;
        }
        break;
    }
    switch (dir) {
    case 2:
    case 3:
    case 4:
        if (*px >= AS(u16, row->goal_x)) {
            *px = AS(u16, row->goal_x);
        } else {
            done = 0;
        }
        break;
    case 6:
    case 7:
    case 8:
        if (*px <= AS(u16, row->goal_x)) {
            *px = AS(u16, row->goal_x);
        } else {
            done = 0;
        }
        break;
    }
    return done;
}

extern u8 g_MoveChrMoveToList[];

/* Send the movers named by `mask` back to their rest positions, wait for
   them, then release their script slots. Called by three script commands
   with one group each: ScriptCommandMoveChrViewFollowStart (1), ScriptCommandMoveChrSecondViewFollowStart (0x20000),
   ScriptCommandMoveChrPartyFollowStart (0x7E); each first waits out MoveChrCommandEndWait itself.

   Slots are the 0x30-byte g_MoveChrRows rows, live bits in g_MoveChrLiveBits, busy
   bits in g_MoveChrBusyBits. A live slot in the mask gets a goal (+0x18 / +0x1A),
   speed g_MoveSpeedPixels[3] (+0x1E), g_MoveSpeedIntervals[3] in +0x24 / +0x25, the
   built-in script g_MoveChrMoveToList (F3 .. .. .. .. F9: one move-to, then end) as
   both its base (+4) and its pointer (+8), +1 = 1 (running), and its busy
   bit:
     bit 0     slot 0, the camera (MoveChrViewStep steps g_MapViewX /
               g_MapViewY): the goal is MapViewGoalGet's -- the lead
               actor centred, clamped to the map.
     bit 17    slot 17, the second scroll (g_MapSecondViewX / g_MapSecondViewY): the
               goal is MapSecondViewGoalGet's. When bit 0 is in the same mask
               the camera position is swapped for the camera's GOAL around
               that call, so the second scroll aims at where the camera
               will stop.
     bits 2-6  slots 2..6, the followers (bit 1 is in 0x7E but no slot 1
               is touched): the goal is party spot i - 1 from
               MapPartyRingEntryGet -- spot 0 for all of them when g_MapPartyKind is
               2 -- plus +0x1C = the spot's byte 4, +0x23 = 0, +0x22 =
               +0x21.
   MoveChrCommandEndWait(mask) then waits, one frame per pass, until none of the
   mask's busy bits is left. Release: the group flag is set (g_MoveChrViewFollow /
   g_MoveChrSecondViewFollow / g_MoveChrPartyFollow = 1), each slot's first byte and live bit are
   cleared -- for the followers whether the slot was live or not -- and
   each follower's two actor rows (g_MapPartyRecords / g_MapPartyTwins, row i - 1)
   are either switched off (first byte 0, g_MapPartyKind == 2) or handed to
   MapAnimationPoseSet(row, -1, spot byte 5).

   Shape: +1 = 1 is written AFTER the two script pointers in all three
   blocks -- a byte store through a pointer makes cse forget g_MoveChrRows,
   and the target reloads it after every other byte store but not between
   these three; the scheduler is what moves the `sb` ahead of the `sw`s.
   The follower spot index is one variable assigned twice (i - 1, then
   masked): the target builds it in $a0 itself. */
/* func_8005FED0 */
void MoveChrFollowStart(s32 row_mask)
{
    MapPartyRingEntry pos;
    s32 i;
    s32 idx;
    s16 sx;
    s16 sy;

    if ((row_mask & 1) && (g_MoveChrLiveBits & 1))
    {
        MapViewGoalGet(&pos);
        ((MoveChrRow *)g_MoveChrRows)[0].speed = g_MoveSpeedPixels[3];
        ((MoveChrRow *)g_MoveChrRows)[0].interval = g_MoveSpeedIntervals[3];
        ((MoveChrRow *)g_MoveChrRows)[0].interval_left = g_MoveSpeedIntervals[3];
        ((MoveChrRow *)g_MoveChrRows)[0].goal_x = pos.x;
        ((MoveChrRow *)g_MoveChrRows)[0].goal_y = pos.y;
        ((MoveChrRow *)g_MoveChrRows)[0].command_list = g_MoveChrMoveToList;
        ((MoveChrRow *)g_MoveChrRows)[0].command = g_MoveChrMoveToList;
        ((MoveChrRow *)g_MoveChrRows)[0].running = 1;
        g_MoveChrBusyBits |= 1;
    }
    if ((row_mask & 0x20000) && (((u32)g_MoveChrLiveBits >> 17) & 1))
    {
        if (row_mask & 1)
        {
            sx = g_MapViewX;
            sy = g_MapViewY;
            g_MapViewX = ((MoveChrRow *)g_MoveChrRows)[0].goal_x;
            g_MapViewY = ((MoveChrRow *)g_MoveChrRows)[0].goal_y;
        }
        MapSecondViewGoalGet(&pos);
        ((MoveChrRow *)g_MoveChrRows)[17].speed = g_MoveSpeedPixels[3];
        ((MoveChrRow *)g_MoveChrRows)[17].interval = g_MoveSpeedIntervals[3];
        ((MoveChrRow *)g_MoveChrRows)[17].interval_left = g_MoveSpeedIntervals[3];
        ((MoveChrRow *)g_MoveChrRows)[17].goal_x = pos.x;
        ((MoveChrRow *)g_MoveChrRows)[17].goal_y = pos.y;
        ((MoveChrRow *)g_MoveChrRows)[17].command_list = g_MoveChrMoveToList;
        ((MoveChrRow *)g_MoveChrRows)[17].command = g_MoveChrMoveToList;
        ((MoveChrRow *)g_MoveChrRows)[17].running = 1;
        g_MoveChrBusyBits |= 0x20000;
        if (row_mask & 1)
        {
            g_MapViewX = sx;
            g_MapViewY = sy;
        }
    }
    if (row_mask & 0x7E)
    {
        for (i = 2; i < 7; i++)
        {
            if (((u32)g_MoveChrLiveBits >> i) & 1)
            {
                idx = 0;
                if (g_MapPartyKind != 2)
                {
                    idx = i - 1;
                    idx &= 0xFFFF;
                }
                MapPartyRingEntryGet(idx, (s32 *)&pos);
                ((MoveChrRow *)g_MoveChrRows)[i].speed = g_MoveSpeedPixels[3];
                ((MoveChrRow *)g_MoveChrRows)[i].interval = g_MoveSpeedIntervals[3];
                ((MoveChrRow *)g_MoveChrRows)[i].interval_left = g_MoveSpeedIntervals[3];
                ((MoveChrRow *)g_MoveChrRows)[i].goal_x = pos.x;
                ((MoveChrRow *)g_MoveChrRows)[i].goal_y = pos.y;
                AS(u16, ((MoveChrRow *)g_MoveChrRows)[i].height_wait.goal_height) = pos.height;
                ((MoveChrRow *)g_MoveChrRows)[i].facing_kept = 0;
                ((MoveChrRow *)g_MoveChrRows)[i].aim = ((MoveChrRow *)g_MoveChrRows)[i].facing;
                ((MoveChrRow *)g_MoveChrRows)[i].command_list = g_MoveChrMoveToList;
                ((MoveChrRow *)g_MoveChrRows)[i].command = g_MoveChrMoveToList;
                ((MoveChrRow *)g_MoveChrRows)[i].running = 1;
                g_MoveChrBusyBits |= 1 << i;
            }
        }
    }
    MoveChrCommandEndWait(row_mask);
    if (row_mask & 1)
    {
        g_MoveChrViewFollow = 1;
        ((MoveChrRow *)g_MoveChrRows)[0].on = 0;
        g_MoveChrLiveBits &= ~1;
    }
    if (row_mask & 0x20000)
    {
        g_MoveChrSecondViewFollow = 1;
        ((MoveChrRow *)g_MoveChrRows)[17].on = 0;
        g_MoveChrLiveBits &= ~0x20000;
    }
    if (row_mask & 0x7E)
    {
        g_MoveChrPartyFollow = 1;
        for (i = 2; i < 7; i++)
        {
            ((MoveChrRow *)g_MoveChrRows)[i].on = 0;
            g_MoveChrLiveBits &= ~(1 << i);
            if (g_MapPartyKind == 2)
            {
                ((MapRecord *)g_MapPartyRecords)[i - 1].on_kind.on = 0;
                ((MapRecord *)g_MapPartyTwins)[i - 1].on_kind.on = 0;
            }
            else
            {
                MapPartyRingEntryGet((i - 1) & 0xFFFF, (s32 *)&pos);
                MapAnimationPoseSet(g_MapPartyRecords + (i - 1) * 0x3C, -1, pos.facing);
                MapAnimationPoseSet(g_MapPartyTwins + (i - 1) * 0x3C, -1, pos.facing);
            }
        }
    }
}
extern u8 g_MoveChrMoveToListCopy[];

/* Call the released mover groups back to their rest positions: the twin
   of MoveChrFollowStart for the groups whose flag says they are
   released -- g_MoveChrViewFollow (bit 0, the camera) and g_MoveChrPartyFollow (0x7E, the
   followers). No group released: nothing happens.

   MoveChrCommandEndWait(mask) waits out the busy bits first. Then
     camera     g_MoveChrViewFollow = 0, slot 0 is opened again (MoveChrRowInit),
                its goal (+0x18 / +0x1A) is MapViewGoalGet's, the built-in
                script g_MoveChrMoveToListCopy (F3 .. .. .. .. F9, a second copy of
                g_MoveChrMoveToList) is both base (+4) and pointer (+8), +1 = 1 and
                busy bit 0 is set.
     followers  g_MoveChrPartyFollow = 0; each slot i = 2..6 whose actor row
                (g_MapPartyRecords row i - 1) is on is opened and bound again:
                +3 = the low byte of member id i - 1 (g_PartyIds), +0xC /
                +0x10 = the two actor rows, +0x14 = &g_MapPartyBoxDx, the goal
                = party spot i - 1 from MapPartyRingEntryGet (spot 0 for all of
                them when g_MapPartyKind is 2), +0x1C = the spot's byte 4,
                +0x23 = 0, +0x21 = +0x22 = the spot's byte 5, the same
                script, +1 = 1, busy bit i.
   MoveChrCommandEndWait(mask) waits for them, then the release of
   MoveChrFollowStart is repeated: group flag back to 1, each slot's
   first byte and live bit cleared (slots 2..6 whether opened or not).
   Last, OUTSIDE the 0x7E test -- so for a camera-only mask too -- rows
   1..5 of both actor tables are switched off (g_MapPartyKind == 2) or, when
   the g_MapPartyRecords row is on, handed to MapAnimationPoseSet(row, -1,
   spot byte 5) with spot i.

   Shape: the spot index is an if/else (the target computes i - 1 in the
   branch's delay slot and jumps over the `= 0` arm); with it preset to 0
   cse skips the small block and the loop's three reduced givs come out in
   another order. +0xC is the row's typed address -- its i * 0x3C is what
   puts the 0x78 giv between the 0x3C and the 0x60 ones. +0x22 = +0x21 is
   one chained assignment: no reload of g_MoveChrRows between the two byte
   stores. */
/* func_800602C4 */
void MoveChrFollowRestart(void)
{
    MapPartyRingEntry pos;
    s32 mask;
    s32 i;
    s32 idx;

    mask = g_MoveChrViewFollow != 0;
    if (g_MoveChrPartyFollow != 0)
    {
        mask |= 0x7E;
    }
    if (mask == 0)
    {
        return;
    }
    MoveChrCommandEndWait(mask);
    if (mask & 1)
    {
        g_MoveChrViewFollow = 0;
        MoveChrRowInit(0);
        MapViewGoalGet(&pos);
        ((MoveChrRow *)g_MoveChrRows)[0].goal_x = pos.x;
        ((MoveChrRow *)g_MoveChrRows)[0].goal_y = pos.y;
        ((MoveChrRow *)g_MoveChrRows)[0].command_list = g_MoveChrMoveToListCopy;
        ((MoveChrRow *)g_MoveChrRows)[0].command = g_MoveChrMoveToListCopy;
        ((MoveChrRow *)g_MoveChrRows)[0].running = 1;
        g_MoveChrBusyBits |= 1;
    }
    if (mask & 0x7E)
    {
        g_MoveChrPartyFollow = 0;
        for (i = 2; i < 7; i++)
        {
            if (((MapRecord *)g_MapPartyRecords)[i - 1].on_kind.on != 0)
            {
                MoveChrRowInit(i & 0xFF);
                if (g_MapPartyKind != 2)
                {
                    idx = i - 1;
                    idx &= 0xFFFF;
                }
                else
                {
                    idx = 0;
                }
                MapPartyRingEntryGet(idx, (s32 *)&pos);
                ((MoveChrRow *)g_MoveChrRows)[i].actor_number.id = ((u16 *)g_PartyIds)[i - 1];
                AS(void *, ((MoveChrRow *)g_MoveChrRows)[i].actor) = &((MapRecord *)g_MapPartyRecords)[i - 1];
                AS(void *, ((MoveChrRow *)g_MoveChrRows)[i].twin) = &((MapRecord *)g_MapPartyTwins)[i - 1];
                AS(void *, ((MoveChrRow *)g_MoveChrRows)[i].box) = &g_MapPartyBoxDx;
                ((MoveChrRow *)g_MoveChrRows)[i].goal_x = pos.x;
                ((MoveChrRow *)g_MoveChrRows)[i].goal_y = pos.y;
                AS(u16, ((MoveChrRow *)g_MoveChrRows)[i].height_wait.goal_height) = pos.height;
                ((MoveChrRow *)g_MoveChrRows)[i].facing_kept = 0;
                ((MoveChrRow *)g_MoveChrRows)[i].aim = ((MoveChrRow *)g_MoveChrRows)[i].facing = pos.facing;
                ((MoveChrRow *)g_MoveChrRows)[i].command_list = g_MoveChrMoveToListCopy;
                ((MoveChrRow *)g_MoveChrRows)[i].command = g_MoveChrMoveToListCopy;
                ((MoveChrRow *)g_MoveChrRows)[i].running = 1;
                g_MoveChrBusyBits |= 1 << i;
            }
        }
    }
    MoveChrCommandEndWait(mask);
    if (mask & 1)
    {
        g_MoveChrViewFollow = 1;
        ((MoveChrRow *)g_MoveChrRows)[0].on = 0;
        g_MoveChrLiveBits &= ~1;
    }
    if (mask & 0x7E)
    {
        g_MoveChrPartyFollow = 1;
        for (i = 2; i < 7; i++)
        {
            ((MoveChrRow *)g_MoveChrRows)[i].on = 0;
            g_MoveChrLiveBits &= ~(1 << i);
        }
    }
    if (g_MapPartyKind == 2)
    {
        for (i = 1; i < 6; i++)
        {
            ((MapRecord *)g_MapPartyRecords)[i].on_kind.on = 0;
            ((MapRecord *)g_MapPartyTwins)[i].on_kind.on = 0;
        }
    }
    else
    {
        for (i = 1; i < 6; i++)
        {
            if (((MapRecord *)g_MapPartyRecords)[i].on_kind.on != 0)
            {
                MapPartyRingEntryGet(i & 0xFFFF, (s32 *)&pos);
                MapAnimationPoseSet(g_MapPartyRecords + i * 0x3C, -1, pos.facing);
                MapAnimationPoseSet(g_MapPartyTwins + i * 0x3C, -1, pos.facing);
            }
        }
    }
}

/* Look up script slot `id` (enabled when bit id of g_MoveChrLiveBits is set,
   else 0xFF): its 0x30-byte g_MoveChrRows record's +3 byte. Slots below 0xB
   chain further: that byte indexes the 0x60-byte g_MapObjects records,
   whose +1 indexes the 0x26-byte g_MapObjectDefinitions records' +1. */
/* func_800605E4 */
u8 MoveChrRowIdGet(u8 id)
{
  if (!(((u32) g_MoveChrLiveBits >> id) & 1))
  {
    return 0xFF;
  }
  if (id < 0xB)
  {
    return ((u8 *) g_MapObjectDefinitions)[((u8 *) g_MapObjects)[((u8 *) g_MoveChrRows)[id * 0x30 + 3] * 0x60 + 1] * 0x26 + 1];
  }
  return ((u8 *) g_MoveChrRows)[id * 0x30 + 3];
}

/* far_syms: g_MapPartyBoxDx */
extern u8 g_PartyCount;

/* func_80060684 */
void MoveChrPartyJoin(u32 chr_id, u32 row_number)
{
  MapPartyRingEntry pos;
  MoveChrRow saved_row;
  s32 idx;
  s32 i;
  u8 *e;

  idx = PartyJoinPlaceGet(chr_id & 0xFF);
  if ((idx == 0) && (((u16 *) g_PartyIds)[1] == 0xC))
  {
    PartyPlacesSwap(0, 1);
    if (g_MoveChrPartyFollow == 0)
    {
      saved_row = ((MoveChrRow *) g_MoveChrRows)[1];
      ((MoveChrRow *) g_MoveChrRows)[1] = ((MoveChrRow *) g_MoveChrRows)[2];
      ((MoveChrRow *) g_MoveChrRows)[2] = saved_row;
      AS(s32, ((MoveChrRow *) g_MoveChrRows)[1].actor) = g_MapPartyRecords;
      AS(s32, ((MoveChrRow *) g_MoveChrRows)[1].twin) = g_MapPartyTwins;
      AS(s32, ((MoveChrRow *) g_MoveChrRows)[2].actor) = (s32) (((MapRecord *) g_MapPartyRecords) + 1);
      AS(s32, ((MoveChrRow *) g_MoveChrRows)[2].twin) = (s32) (((MapRecord *) g_MapPartyTwins) + 1);
    }
  }
  for (i = g_PartyCount; idx < i; i--)
  {
    ((u16 *) g_PartyIds)[i] = ((u16 *) g_PartyIds)[i - 1];
    ((MapRecord *) g_MapPartyRecords)[i] = ((MapRecord *) g_MapPartyRecords)[i - 1];
    ((MapRecord *) g_MapPartyTwins)[i] = ((MapRecord *) g_MapPartyTwins)[i - 1];
  }
  if ((chr_id & 0xFF) < 0x1F)
  {
    FaceListIdMove(chr_id & 0xFF, idx);
  }
  if (g_MoveChrPartyFollow == 0)
  {
    i = g_PartyCount + 1;
    g_MoveChrLiveBits |= 1 << i;
    for (; (idx + 1) < i; i--)
    {
      ((MoveChrRow *) g_MoveChrRows)[i] = ((MoveChrRow *) g_MoveChrRows)[i - 1];
      AS(s32, ((MoveChrRow *) g_MoveChrRows)[i].actor) = (s32) (((MapRecord *) g_MapPartyRecords) + (i - 1));
      AS(s32, ((MoveChrRow *) g_MoveChrRows)[i].twin) = (s32) (((MapRecord *) g_MapPartyTwins) + (i - 1));
    }
  }
  g_PartyCount++;
  if ((row_number & 0xFF) == 0)
  {
    MapPartyRingEntryGet(idx & 0xFFFF, &pos);
    ((u16 *) g_PartyIds)[idx] = (u8) chr_id;
    if (MapPartySpriteDataMissingCount() != 0)
    {
      MapPartySpriteDataLoad(0);
    }
    MapPartyRecordInit(idx, chr_id & 0xFF);
    *((u16 *) ((g_MapPartyRecords + (idx * 0x3C)) + 6)) = pos.x;
    *((u16 *) ((g_MapPartyRecords + (idx * 0x3C)) + 0xA)) = pos.y - pos.height;
    *((u16 *) ((g_MapPartyRecords + (idx * 0x3C)) + 8)) = pos.y;
    if ((chr_id & 0xFF) < 0xF)
    {
      *((u16 *) ((g_MapPartyTwins + (idx * 0x3C)) + 6)) = pos.x;
      *((u16 *) ((g_MapPartyTwins + (idx * 0x3C)) + 0xA)) = pos.y;
      *((u16 *) ((g_MapPartyTwins + (idx * 0x3C)) + 8)) = pos.y;
    }
    if ((g_MoveChrPartyFollow == 0) || (idx == 0))
    {
      i = idx + 1;
      MoveChrRowInit(i & 0xFF);
      ((u8 *) g_MoveChrRows)[(i * 0x30) + 0x21] = pos.facing;
      ((u8 *) g_MoveChrRows)[(i * 0x30) + 2] = pos.height;
    }
  }
  else
  {
    if (!((((u32) g_MoveChrLiveBits) >> (row_number & 0xFF)) & 1))
    {
      g_ScriptErrorNumber = 0xF;
      ScriptErrorReport();
      return;
    }
    ((u16 *) g_PartyIds)[idx] = chr_id & 0xFF;
    if ((row_number & 0xFF) < 0xB)
    {
      i = ((u8 *) g_MoveChrRows)[((row_number & 0xFF) * 0x30) + 3];
      e = (u8 *) (g_MapObjectDefinitions + (((u8 *) g_MapObjects)[(i * 0x60) + 1] * 0x26));
      if (((e[0] & 0x3F) + 1) < 4)
      {
        StoryBitSet(e[2] + 0x580);
      }
      ((MapRecord *) g_MapPartyRecords)[idx] = *(*((MapRecord **) (((u8 *) g_MoveChrRows) + ((row_number & 0xFF) * 0x30) + 0xC)));
      ((u8 *) g_MapPartyRecords)[(idx * 0x3C) + 1] = 0;
      ((MapRecord *) g_MapPartyTwins)[idx] = *(*((MapRecord **) (((u8 *) g_MoveChrRows) + ((row_number & 0xFF) * 0x30) + 0x10)));
      ((u8 *) g_MapObjects)[i * 0x60] = 0;
      ((u8 *) g_MapObjectTwins)[i * 0x3C] = 0;
    }
    else
    {
      ((MapRecord *) g_MapPartyRecords)[idx] = *(*((MapRecord **) (((u8 *) g_MoveChrRows) + ((row_number & 0xFF) * 0x30) + 0xC)));
      ((u8 *) g_MapPartyRecords)[(idx * 0x3C) + 1] = 0;
      ((MapRecord *) g_MapPartyTwins)[idx] = *(*((MapRecord **) (((u8 *) g_MoveChrRows) + ((row_number & 0xFF) * 0x30) + 0x10)));
      ((u8 *) g_MoveChrOwnActors)[((row_number & 0xFF) * 0x60) - 0x420] = 0;
      ((u8 *) g_MoveChrOwnActorTwins)[((row_number & 0xFF) * 0x3C) - 0x294] = 0;
    }
    if ((g_MoveChrPartyFollow == 0) || (idx == 0))
    {
      i = idx + 1;
      ((MoveChrRow *) g_MoveChrRows)[i] = ((MoveChrRow *) g_MoveChrRows)[row_number & 0xFF];
    }
    ((u8 *) g_MoveChrRows)[(row_number & 0xFF) * 0x30] = 0;
    g_MoveChrLiveBits &= ~(1 << (row_number & 0xFF));
  }
  if ((g_MoveChrPartyFollow == 0) || (idx == 0))
  {
    i = idx + 1;
    ((MoveChrRow *) g_MoveChrRows)[i].actor_number.object = ((u16 *) g_PartyIds)[idx];
    AS(s32, ((MoveChrRow *) g_MoveChrRows)[i].actor) = (s32) (((MapRecord *) g_MapPartyRecords) + idx);
    AS(s32, ((MoveChrRow *) g_MoveChrRows)[i].twin) = (s32) (((MapRecord *) g_MapPartyTwins) + idx);
    AS(s32, ((MoveChrRow *) g_MoveChrRows)[i].box) = (s32) (&g_MapPartyBoxDx);
  }
  PartyLevelLeastSet();
  StoryBit2CAUpdate();
  if (g_MoveChrPartyFollow != 0)
  {
    MoveChrFollowRestart();
  }
}

s32 StoryBitClear(s32);
s16 MapObjectFind(u8 definition_number);
s16 MapObjectFreeFind(void);
void MapObjectInit(void *, void *, void *, u8);
void FaceListSwap(s32 byte_a, s32 byte_b);
void MapPartyRingFill();

/* func_80060F18 */
void MoveChrPartyLeave(s32 slot, s16 dest, s32 id)
{
    u8 *rec;
    MapObject *obj;
    MapRecord *obj2;
    s16 idx;
    s32 i;
    s32 n;
    MapPartyRingEntry pos;
    MoveChrRow saved_row;

    if ((u8)dest != 0)
    {
        if (((u32)g_MoveChrLiveBits >> (u8)dest) & 1)
        {
            g_ScriptErrorNumber = 0x10;
            ScriptErrorReport();
            return;
        }
        if ((u8)dest < 0xB)
        {
            rec = (u8 *)g_MapObjectDefinitions + id * 0x26;
            if (rec[1] != ((u16 *)g_PartyIds)[slot])
            {
                g_ScriptErrorNumber = 0xC;
                ScriptErrorReport();
                return;
            }
            if (MapObjectFind((u8)id) >= 0)
            {
                g_ScriptErrorNumber = 0xB;
                ScriptErrorReport();
                return;
            }
            idx = MapObjectFreeFind();
            if (idx < 0)
            {
                g_ScriptErrorNumber = 2;
                ScriptErrorReport();
                return;
            }
            if (((rec[0] & 0x3F) + 1) < 4)
            {
                StoryBitClear(rec[2] + 0x580);
            }
            else
            {
                StoryBitClear((u16)(id + 0x680 + ((g_MapNumber - 0x65) << 7)));
            }
            MoveChrRowInit((u8)dest);
            ((MoveChrRow *)g_MoveChrRows)[(u8)dest].actor_number.object = idx;
            MapObjectInit((MapObject *)g_MapObjects + idx, (MapRecord *)g_MapObjectTwins + idx, rec, id);
            obj = (MapObject *)g_MapObjects + idx;
            obj->record = ((MapRecord *)g_MapPartyRecords)[slot];
            obj->record.on_kind.kind = 0x81;
            obj->record.hold_definition.definition = id;
            obj2 = (MapRecord *)g_MapObjectTwins + idx;
        }
        else
        {
            idx = (u8)dest - 0xB;
            obj = (MapObject *)g_MoveChrOwnActors + idx;
            obj2 = (MapRecord *)g_MoveChrOwnActorTwins + idx;
            MoveChrRowInit((u8)dest);
            ((MoveChrRow *)g_MoveChrRows)[(u8)dest].actor_number.object = ((u16 *)g_PartyIds)[slot];
            obj->record = ((MapRecord *)g_MapPartyRecords)[slot];
            ((MapObject *)g_MoveChrOwnActors)[idx].number = 2;
        }
        *obj2 = ((MapRecord *)g_MapPartyTwins)[slot];
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[(u8)dest].box) = &obj->box;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[(u8)dest].actor) = obj;
        AS(void *, ((MoveChrRow *)g_MoveChrRows)[(u8)dest].twin) = obj2;
        ((MoveChrRow *)g_MoveChrRows)[(u8)dest].height = ((MoveChrRow *)g_MoveChrRows)[slot + 1].height;
        ((MoveChrRow *)g_MoveChrRows)[(u8)dest].facing = obj->facing = ((MoveChrRow *)g_MoveChrRows)[slot + 1].facing;
        ((MoveChrRow *)g_MoveChrRows)[(u8)dest].layer = obj->layer = ((MoveChrRow *)g_MoveChrRows)[slot + 1].layer;
        obj->turn_count = 0;
        obj->box = *(MapBox *)&g_MapPartyBoxDx;
    }
    for (i = slot; i < g_PartyCount - 1; )
    {
        ((u16 *)g_PartyIds)[i] = ((u16 *)g_PartyIds)[i + 1];
        ((MapRecord *)g_MapPartyRecords)[i] = ((MapRecord *)g_MapPartyRecords)[i + 1];
        ((MapRecord *)g_MapPartyTwins)[i] = ((MapRecord *)g_MapPartyTwins)[i + 1];
        n = i;
        i++;
        FaceListSwap(n, i);
    }
    ((u16 *)g_PartyIds)[i] = 0;
    ((MapRecord *)g_MapPartyRecords)[i].on_kind.kind = 0;
    ((MapRecord *)g_MapPartyTwins)[i].on_kind.kind = 0;
    i = slot + 1;
    if (g_MoveChrPartyFollow == 0)
    {
        for (; i < g_PartyCount; i++)
        {
            ((MoveChrRow *)g_MoveChrRows)[i] = ((MoveChrRow *)g_MoveChrRows)[i + 1];
            AS(void *, ((MoveChrRow *)g_MoveChrRows)[i].actor) = (MapRecord *)g_MapPartyRecords + (i - 1);
            AS(void *, ((MoveChrRow *)g_MoveChrRows)[i].twin) = (MapRecord *)g_MapPartyTwins + (i - 1);
        }
        g_MoveChrLiveBits &= ~(1 << i);
        ((MoveChrRow *)g_MoveChrRows)[i].on = 0;
    }
    g_PartyCount--;
    if (slot == 0)
    {
        if (g_MoveChrPartyFollow != 0)
        {
            if (((u16 *)g_PartyIds)[0] == 0xC && g_PartyCount != 1)
            {
                PartyPlacesSwap(0, 1);
                n = 2;
            }
            else
            {
                n = 1;
            }
            MapPartyRingEntryGet(n, (s32 *)&pos);
            MoveChrRowInit(1);
            AS(void *, ((MoveChrRow *)g_MoveChrRows)[1].actor) = (void *)g_MapPartyRecords;
            AS(void *, ((MoveChrRow *)g_MoveChrRows)[1].twin) = (void *)g_MapPartyTwins;
            AS(void *, ((MoveChrRow *)g_MoveChrRows)[1].box) = &g_MapPartyBoxDx;
            ((MoveChrRow *)g_MoveChrRows)[1].facing = pos.facing;
            ((MoveChrRow *)g_MoveChrRows)[1].height = pos.height;
        }
        else
        {
            if (((u16 *)g_PartyIds)[0] == 0xC && g_PartyCount != 1)
            {
                PartyPlacesSwap(0, 1);
                saved_row = ((MoveChrRow *)g_MoveChrRows)[1];
                ((MoveChrRow *)g_MoveChrRows)[1] = ((MoveChrRow *)g_MoveChrRows)[2];
                ((MoveChrRow *)g_MoveChrRows)[2] = saved_row;
                AS(void *, ((MoveChrRow *)g_MoveChrRows)[1].actor) = (void *)g_MapPartyRecords;
                AS(void *, ((MoveChrRow *)g_MoveChrRows)[1].twin) = (void *)g_MapPartyTwins;
                AS(void *, ((MoveChrRow *)g_MoveChrRows)[2].actor) = (MapRecord *)g_MapPartyRecords + 1;
                AS(void *, ((MoveChrRow *)g_MoveChrRows)[2].twin) = (MapRecord *)g_MapPartyTwins + 1;
            }
            pos.x = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.x);
            pos.y = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.depth);
            pos.height = ((MoveChrRow *)g_MoveChrRows)[1].height;
            pos.facing = ((MoveChrRow *)g_MoveChrRows)[1].facing;
        }
        MapPartyRingFill(pos.x, pos.y, pos.height, pos.facing);
    }
    PartyLevelLeastSet();
    StoryBit2CAUpdate();
    if (g_MoveChrPartyFollow != 0)
    {
        MoveChrFollowRestart();
    }
}

extern s16 g_ScriptErrorNumber;
extern s8 g_MapPartyBoxDx;
extern u16 g_MapPartyKind;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern u16 g_MapNumber;
extern s16 g_MapViewKeptX;
extern s16 g_MapViewKeptY;
extern u16 g_MapEffect;
extern s32 g_MoveChrBusyBits;
extern void *g_MoveChrRows;

/* func_80061788 */
void MoveChrSecondViewFollowSet(void)
{
  int mode;
  if (((u32) ((mode = g_MapEffect & 3) - 1)) >= 2U)
  {
    g_MoveChrSecondViewFollow = 1;
    *(((u8 *) g_MoveChrRows) + 0x330) = 0;
    g_MoveChrBusyBits &= 0xFFFDFFFF;
    g_MoveChrLiveBits &= 0xFFFDFFFF;
  }
}

void MoveChrAction();                                  /* extern */
void MoveChrCommandRun();                                  /* extern */

/* func_800617D8 */
void MoveChrUpdate(void) {
    MoveChrCommandRun();
    MoveChrAction();
}

extern u8 g_MoveChrCommandFunctions[];

/* func_80061800 */
void MoveChrCommandRun(void) {
    s32 row_number;
    s32 row_offset;
    s32 i;
    void *row;

    i = 0;
    do {
        row_number = i & 0xFFFF;
        if (((u32) g_MoveChrBusyBits >> row_number) & 1) {
            row_offset = row_number * 0x30;
            row = row_offset + ((s32)g_MoveChrRows);
            ((s32 (*)()) *(s32 *)(((((*(u8 *)(AS(s32, ((MoveChrRow *)row)->command))) + 0x10) & 0xFF) * 4) + g_MoveChrCommandFunctions))(row, row_number);
            MoveChrScaleStep(((s32)g_MoveChrRows) + row_offset);
        }
        i += 1;
    } while ((u32) (i & 0xFFFF) < 0x12U);
}
extern s32 g_SpriteList;
extern u8 D_80012B84[];
extern u8 D_80012BA0[];

/* MoveChrAction (the name is the EXE's: its assert prints
   "MoveChrAction:<invalid pattern>" / "LNS_MOVE.C", line 0x3FF) -- the
   script actors' per-frame animation tick and draw: MapPartyAnimationStep's and
   MapObjectsAnimationStep's counterpart for script slots 7..0x10 (the 0x30-byte
   g_MoveChrRows rows, live bits g_MoveChrLiveBits). MoveChrUpdate calls it right
   after MoveChrCommandRun has stepped the movers, once per field frame
   (ScriptFrameStep, MapFrameStep).

   Sprite slots 6..0x10 and 0x4C..0x56 are cleared first. Then for each
   live slot i, on its actor row p (slot +0xC) and twin q (slot +0x10):
   - the frame timer advances; at the frame's duration the frame index
     steps. Past the animation's end it either holds the last frame
     (animation entry bit 15: frame--, timer 0, the frame's duration) or
     restarts (MapAnimationStart(p, 1)); otherwise timer 0 and the next frame's
     duration -- and ONLY on that path the frame table pointer is asserted
     to lie in RAM (0x80000000..0x801FFFFF), else assert_fail;
   - the row's sprite image goes to sprite slot i - 1 with the current
     frame's tile (unlike the map objects' pass there is no camera cull and
     no held/behaviour test: every live slot is ticked and drawn);
   - a live twin (q's first byte) is put on the row's animation
     (MapAnimationSet), synced to the same distance from the end, timer and
     duration, takes the row's x, and BOTH its ground y and its draw y
     from the row's GROUND y (the row's draw y is ground - height, so the
     twin sits on the ground under a raised actor), and goes to sprite
     slot i + 0x45 with its own frame's tile.
   No frame wait and no RNG draw (its callees reach neither).

   Shape: as MapPartyAnimationStep (map_animation.c): u16 counter, the two
   row pointers read up front,
   struct-typed sprite slots indexed i - 1 and i + 0x45 (the 0x9B4 /
   0x9CC offsets off i * 0x24 + base are cse's), and the twin's two y
   stores as one chained assignment (two statements would reload the
   row's ground y after the first store). Callees are called undeclared. */
/* func_800618B4 */
void MoveChrAction(void)
{
    SpriteEntry *spr;
    MapRecord *p;
    MapRecord *q;
    u16 i;

    spr = (SpriteEntry *)g_SpriteList;
    sprite_run_off(6, 0xB);
    sprite_run_off(0x4C, 0xB);
    for (i = 7; i < 0x11; i++) {
        if (((u32)g_MoveChrLiveBits >> i) & 1) {
            p = ((MoveChrRow *)g_MoveChrRows)[i].actor;
            q = ((MoveChrRow *)g_MoveChrRows)[i].twin;
            if (++p->timer >= p->step_length) {
                if (++p->step >= p->end) {
                    if (AS(SpriteAnimationRow *, p->animation_table)[p->animation].count & 0x8000) {
                        p->step--;
                        p->timer = 0;
                        p->step_length = AS(SpriteStep *, p->step_list)[p->step].length;
                    } else {
                        MapAnimationStart(p, 1);
                    }
                } else {
                    p->timer = 0;
                    if ((u32)AS(SpriteStep *, p->step_list) < 0x80000000 || (u32)AS(SpriteStep *, p->step_list) > 0x801FFFFF) {
                        assert_fail(D_80012B84, D_80012BA0, 0x3FF);
                    }
                    p->step_length = AS(SpriteStep *, p->step_list)[p->step].length;
                }
            }
            spr[i - 1] = p->sprite;
            spr[i - 1].pose = AS(SpriteStep *, p->step_list)[p->step].pose;
            if (q->on_kind.on != 0) {
                MapAnimationSet(q, p->animation);
                q->step = q->end - (p->end - p->step);
                q->timer = p->timer;
                q->step_length = p->step_length;
                q->sprite.x = p->sprite.x;
                q->sprite.y = AS(u16, q->sprite.depth) = AS(u16, p->sprite.depth);
                spr[i + 0x45] = q->sprite;
                spr[i + 0x45].pose = AS(SpriteStep *, q->step_list)[q->step].pose;
            }
        }
    }
}

s32 MapRecordSpriteDataSet(u8 *, u8);

/* func_80061B8C */
void MoveChrSpriteDataSet(void)
{
  unsigned int sprite_data_number;
  s32 row_offset;
  u8 *twin_numbers;
  s32 row_number;
  u8 *twin_record;
  void *row;
  row_number = 0xB;
  twin_numbers = g_MapTwinSpriteDataNumbers;
  row_offset = 0x210;
  do
  {
    if ((((u32) g_MoveChrLiveBits) >> row_number) & 1)
    {
      row = row_offset + ((s32)g_MoveChrRows);
      if (((MoveChrRow *)row)->on != 0)
      {
        sprite_data_number = ((MoveChrRow *)row)->actor_number.id;
        MapRecordSpriteDataSet(AS(s32, ((MoveChrRow *)row)->actor), sprite_data_number);
      }
      twin_record = *((s32 *) (((u8 *) (row_offset + ((s32)g_MoveChrRows))) + 0x10));
      if ((*twin_record) != 0)
      {
        sprite_data_number = *(u8 *)((s32)sprite_data_number + (s32)twin_numbers - 1);
        MapRecordSpriteDataSet(twin_record, sprite_data_number);
      }
    }
    row_number += 1;
    row_offset += 0x30;
  }
  while (row_number < 0x11);
}
