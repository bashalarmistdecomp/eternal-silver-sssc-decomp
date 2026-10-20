/* Lunar SSSC — map_look: the map objects' thinks 6 to 8: the looks for the
   party (kinds 0 to 0xC), what they do once they have seen it (1 to 6), the
   walks and turns those use, and the check whether an object sees the
   party. The first split had it in battle_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "menu.h"
#include "map.h"
#include "sprite.h"

extern s32 rng_draw(void); 

extern void sprite_run_off(s32 first, s32 count);
extern void mem_clear(void *destination, u32 byte_count);
/* Project (*px, *py) `dist` units along compass direction `dir` (1..8). */
extern void MapSpotMove(u16 dir, u16 dist, u16 *px, u16 *py);
/* World/camera clip rectangle: X in [g_MapLimitLeft, g_MapLimitRight),
   Y in [g_MapLimitTop, g_MapLimitBottom).  All four gp-relative halfwords. */
extern s16 g_MapLimitLeft;
extern s16 g_MapLimitTop;
extern s16 g_MapLimitRight;
extern s16 g_MapLimitBottom;
extern s32 MapFacingTowardGet(s16 x0, s16 y0, s16 x1, s16 y1, s32 facing);
extern void SoundPlay(s32 id, s32 ch);
extern void MapObjectWanderStep(s32 p, s32 row_number);

extern void MapObjectRandomWalkStart(MapObject *row, s32 draw_mode);
extern s32 MapObjectPartyLook05Step(MapObject *row, u16 row_number);
extern s32 MapObjectPartyLook06Step(MapObject *row, u16 row_number);
extern s32 MapObjectPartyLook02Step(MapObject *row, s32 unused);
extern s32 MapObjectPartyLook0BStep(MapObject *row, u16 row_number);
extern void MapObjectRandomSpotPlace(MapObject *row, s32 row_number);
extern u8 rng_range_draw(u8 fixed, u8 min, u8 max);
extern s8 MapObjectSideFacingGet(MapObject *row, s32 reverse);
extern void MapObjectMoveSet(MapObject *row, MapObject *info, s32 second_set);
extern void MapAnimationPoseSet(MapObject *record, s32 pose, s32 facing);
extern void MapObjectFacingTurnSet(MapObject *row, s32 facing);
/* Returns a status word MapObjectPartySeen02Step propagates; MapObjectPartySeen01Step
   ignores it. */
extern s32 MapObjectTowardWalk(MapObject *row, s32 row_number, s16 goal_x, s16 goal_y);
extern s32 MapAttributeFind(s32 x, s32 y, s32 w, s32 h);
extern void ScriptLabelRun(s32 label);
extern s32 MapObjectPartySeen05Step(MapObject *row, s32 unused);
extern s32 MapObjectFacingSet(MapObject *row, s32 facing);
extern s32 MapObjectAwayWalk(MapObject *row, s32 row_number, s16 away_x, s16 away_y);
extern void MapObjectNumberReset(MapObject *row, u8 *definition);

extern s32 g_MapPartyRecords;     /* base pointer (gp-relative word) */
extern s32 MapObjectFacingTurnStep(MapObject *row, s32 unused);
extern void MapObjectPartySeenCheck(u8 *, u16);
extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table 2 */
extern s32 g_MapObjectDefinitions;     /* base of a 0x26-stride record table (gp word) */
extern s32 g_MapObjects;     /* base of a 0x60-stride slot table (gp word) */
extern u16 g_MapNumber;     /* gp halfword: id-bank page for StoryBitSet cues */
s32 MapObjectOpenFacingFind(MapObject *row, u16 row_number, s32 all_facings);
extern s32 MapSpotMoveTry(u8 dir, u16 dist, u16 *px, u16 *py, u8 *probe, s32 layer, s32 flag);
void MapObjectOverlappedRemove(MapObject *row);
extern void StoryBitSet(s32 bit);
extern s32 MapObjectOverlapFind(s16 spot_x, s16 spot_y, u16 row_number);
/* Script-side movement projection: steps (*px, *py) `dist` tiles along `dir`
   using the actor's probe rect at `probe`, returns the facing actually used. */
extern s32 MapSpotMoveTry(u8 dir, u16 dist, u16 *px, u16 *py, u8 *probe,
                                 s32 layer, s32 flag);
extern u8 g_MapActuator64List[];     /* script effect table installed on first use */
extern s32 g_PadActuatorList;      /* far_syms.txt: init guard / table pointer slot */
extern u8 g_PadActuatorRestFlags;       /* far_syms.txt: lui $at + sb */
extern s16 g_PadActuatorListWait;      /* far_syms.txt: lui $at + sh */
extern s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag);

/* func_80077EE8 */
s32 value_div(s32 value, s32 divisor) {
    return value / divisor;
}
/* func_80077F1C */
s32 MapObjectPartyLookStep(MapObject *row, u16 row_number) {
    s32 result;
    if (row->turn_plan != 0) {
        result = MapObjectFacingTurnStep(row, row_number);
    } else {
        result = g_MapObjectPartyLookFunctions[row->look & 0x7F](row, row_number);
    }
    MapObjectPartySeenCheck(row, row_number);
    return result;
}
/* func_80077FAC */
s32 MapObjectPartySeenStep(MapObject *row, u16 row_number) {
    s32 result;
    s32 off;
    u8 *e;
    if (row->turn_plan != 0) {
        result = MapObjectFacingTurnStep(row, row_number);
    } else {
        off = row->record.hold_definition.definition * 0x26;
        e = (u8 *)(g_MapObjectDefinitions + off);
        result = g_MapObjectPartySeenFunctions[(e[0x1E] & 0x7F) - 1](row, row_number);
    }
    MapObjectPartySeenCheck(row, row_number);
    return result;
}
/* func_80078060 */
s32 MapObjectPartyAwayStep(MapObject *row, u16 row_number) {
    s32 result;
    if (row->turn_plan != 0) {
        result = MapObjectFacingTurnStep(row, row_number);
    } else {
        result = g_MapObjectPartySeenFunctions[5](row, row_number);
    }
    MapObjectPartySeenCheck(row, row_number);
    return result;
}
/* Roll a value in [min, max] when no fixed value is given. */
/* func_800780DC */
u8 rng_range_draw(u8 fixed, u8 min, u8 max) {
    if (fixed == 0) {
        fixed = min + rng_draw() % (max - min + 1);
    }
    return fixed;
}
/* Picks a random reachable tile inside the actor's movement bounds: reroll
   until MapAttributeFreeCheck accepts the probe rectangle, then
   again while MapObjectOverlapFind reports the tile occupied.  Kept in m2c's
   goto form on purpose — the back-edge from the tile-access test to the
   top of the reroll is what places the two loops' blocks as the target has
   them; a nested while/do-while equivalent does not match. */
/* func_8007816C */
void MapObjectRandomSpotPlace(MapObject *row, s32 row_number) {
    s16 spot_y;
    s16 spot_x;

    do {
loop_1:
        spot_x = (u16) row->move.bounds.left + (rng_draw() % (s32) ((row->move.bounds.right - row->move.bounds.left) + 1));
        spot_y = (u16) row->move.bounds.top + (rng_draw() % (s32) ((row->move.bounds.bottom - row->move.bounds.top) + 1));
        if (MapAttributeFreeCheck(spot_x + row->box.dx, spot_y + row->box.dy, row->box.half_width, row->box.half_height, (s32) row->layer, 0) == 0) {
            goto loop_1;
        }
    } while (MapObjectOverlapFind(spot_x, spot_y, row_number & 0xFFFF) & 0xFFFF);
    row->record.sprite.x = spot_x;
    row->record.sprite.y = spot_y;
    row->record.sprite.depth = spot_y;
}
/* The shared movement-step engine: walk the actor up to `unk3F` tiles toward
   (goal_x, goal_y), retrying with a shorter step on every rejection.

   Outer loop: `remaining` tiles still to cover, restarting from the actor's
   live position each pass.  Inner loop: `tries` counts a candidate step length
   down from `remaining`.  Each candidate re-derives the facing through
   MapFacingTowardGet (only when the cached `unk4A` is stale or zero), asks
   MapSpotMoveTry to project (x, y) that far along it, discards the
   result if it merely reverses the current facing (the `+3 & 7, +1` rotation
   test), and otherwise accepts it only when the destination is inside the
   unk54..unk5A bounds and MapObjectOverlapFind reports no collision.  On acceptance
   the covered distance is subtracted and the position committed; if the inner
   loop instead runs out of candidates (`tries == 0`) the cached facing is
   cleared and the walk gives up.  Returns whether any ground was covered.

   The first call also one-shot-initialises the script side (g_PadActuatorList /
   g_PadActuatorRestFlags / g_PadActuatorListWait — all three are far_syms).

   `dir` must be `u16` (each `andi …,0xFFFF` in the target is that local being
   promoted), and the `unk3E` store must come **last** in the commit block:
   written before the position stores GCC schedules the `sb` ahead of the two
   stack reloads, where the target issues both `lhu`s first. */
/* func_800782B8 */
s32 MapObjectTowardWalk(MapObject *row, s32 row_number, s16 goal_x, s16 goal_y) {
    u16 spot_x;
    u16 spot_y;
    s32 remaining;
    s32 tries;
    u16 dir;

    if (g_PadActuatorList == 0) {
        g_PadActuatorList = (s32)g_MapActuator64List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
    }
    remaining = row->speed;
    while (remaining > 0) {
        spot_x = row->record.sprite.x;
        spot_y = row->record.sprite.depth;
        tries = remaining;
        while (tries > 0) {
            if (row->facing == row->facing_kept || row->facing_kept == 0) {
                row->facing_kept = MapFacingTowardGet((s16)spot_x, (s16)spot_y, goal_x, goal_y, row->facing);
            }
            dir = MapSpotMoveTry(row->facing_kept, tries, &spot_x, &spot_y,
                                        (u8 *)&row->box.dx, row->layer, 0);
            if (row->facing == ((dir + 3) & 7) + 1) {
                dir = 0;
            }
            if (dir != 0 &&
                (s16)spot_x >= row->move.bounds.left && (s16)spot_x < row->move.bounds.right &&
                (s16)spot_y >= row->move.bounds.top && (s16)spot_y < row->move.bounds.bottom &&
                (MapObjectOverlapFind((s16)spot_x, (s16)spot_y, row_number) & 0xFFFF) == 0) {
                remaining -= tries;
                row->record.sprite.x = spot_x;
                row->record.sprite.y = spot_y;
                row->record.sprite.depth = spot_y;
                row->facing = dir;
                break;
            }
            tries--;
        }
        if (tries == 0) {
            row->facing_kept = 0;
            break;
        }
    }
    return remaining != row->speed;
}

/* MapObjectTowardWalk's twin without the one-shot script init or the cached-facing
   test: every candidate re-derives the facing (unk4A) from MapFacingTowardGet, and
   MapSpotMoveTry is passed the whole `remaining` distance rather than
   `tries`.  Same commit rules as the twin (unk3E store last in the commit
   block).  `dir` is ONE s32 local reused for both calls' results: the extra
   (combined-away) definition is what gives it the allocator weight to take
   $s0 ahead of row -- as a u16 holding only MapSpotMoveTry's result
   it loses that tie and the two swap ($s0<->$s1, 88/114). */
/* func_800784C4 */
s32 MapObjectAwayWalk(MapObject *row, s32 row_number, s16 away_x, s16 away_y) {
    u16 spot_x;
    u16 spot_y;
    s32 remaining;
    s32 tries;
    s32 dir;

    remaining = row->speed;
    while (remaining > 0) {
        spot_x = row->record.sprite.x;
        spot_y = row->record.sprite.depth;
        tries = remaining;
        while (tries > 0) {
            dir = MapFacingTowardGet((s16)spot_x, (s16)spot_y, away_x, away_y, row->facing);
            row->facing_kept = ((dir + 3) & 7) + 1;
            dir = MapSpotMoveTry(row->facing_kept, remaining, &spot_x, &spot_y,
                                        (u8 *)&row->box.dx, row->layer, 0);
            if (row->facing == (((u16)dir + 3) & 7) + 1) {
                dir = 0;
            }
            if ((u16)dir != 0 &&
                (s16)spot_x >= row->move.bounds.left && (s16)spot_x < row->move.bounds.right &&
                (s16)spot_y >= row->move.bounds.top && (s16)spot_y < row->move.bounds.bottom &&
                (MapObjectOverlapFind((s16)spot_x, (s16)spot_y, row_number) & 0xFFFF) == 0) {
                remaining -= tries;
                row->record.sprite.x = spot_x;
                row->record.sprite.y = spot_y;
                row->record.sprite.depth = spot_y;
                row->facing = dir;
                break;
            }
            tries--;
        }
        if (tries == 0) {
            row->facing_kept = 0;
            break;
        }
    }
    return remaining != row->speed;
}

/* Walks the actor's queued step list (unk3F entries) backwards, asking
   MapSpotMove for each candidate square, and commits the first one that
   is inside the movement bounds, walkable, and unoccupied.  *stopped_by receives
   the blocking-entity id of the last rejected square.  Returns non-zero if
   any step remained; unk4A keeps the facing when nothing was consumed.

   DO NOT restructure the goto layout.  The physical block order
   done -> commit -> zero -> return is the target's, and it is what the
   explicit `goto zero` / `goto ret` buy: writing the tail as a plain
   if/else emits the `unk4A = 0` block where the commit block belongs
   (83/89).  arg1 is `s32` with the mask written at the call site — as a
   `u16` parameter the loop's register coloring changes (77/89). */
/* func_8007868C */
s32 MapObjectFacingWalk(MapObject *row, s32 row_number, s32 *stopped_by) {
    u16 spot_x;
    u16 spot_y;
    s32 t;
    s32 n;
    s32 distance;

    *stopped_by = 0;
    distance = row->speed;
    if (distance > 0) {
        n = distance & 0xFFFF;
    loop:
        spot_x = row->record.sprite.x;
        spot_y = row->record.sprite.depth;
        MapSpotMove(row->facing, n, &spot_x, &spot_y);
        if ((s16)spot_x >= row->move.bounds.left && (s16)spot_x < row->move.bounds.right &&
            (s16)spot_y >= row->move.bounds.top && (s16)spot_y < row->move.bounds.bottom &&
            MapAttributeFreeCheck((s16)spot_x + row->box.dx, (s16)spot_y + row->box.dy,
                                              row->box.half_width, row->box.half_height, row->layer, 0) != 0) {
            t = MapObjectOverlapFind((s16)spot_x, (s16)spot_y, row_number & 0xFFFF) & 0xFFFF;
            if (t == 0) {
                goto commit;
            }
            *stopped_by = t;
        }
        distance -= 1;
        n = distance & 0xFFFF;
        if (distance > 0) {
            goto loop;
        }
    }
done:
    if (distance != row->speed) {
        goto zero;
    }
    row->facing_kept = row->facing;
    goto ret;
commit:
    row->record.sprite.x = spot_x;
    row->record.sprite.y = spot_y;
    row->record.sprite.depth = spot_y;
    goto done;
zero:
    row->facing_kept = 0;
ret:
    return distance != 0;
}
/* Pick the facing with the most open ground for a map object's walk (its
   one caller is MapObjectPartyLook04Step).
   Expanding ring search: for ring = 1, 2, 3 … project the actor's position
   `ring * 8` units along each of the 8 compass directions (MapSpotMove).
   `blocked` is a bitmask of directions already ruled out — it starts at 0x55
   (the four odd facings 1, 3, 5, 7) unless all_facings asks for all eight — and
   the search stops once every direction is blocked (0xFF).  A direction
   that leaves the world
   bounds (g_MapLimitLeft..CE4) or fails the tile probe / collision check is
   blocked and the scan continues; one that leaves the actor's own
   unk54..unk5A bounds blocks it and abandons the whole ring.  Surviving
   directions score a point in counts[].

   The tail returns the 1-based direction with the highest count, scanning
   evens then odds (0,2,4,6 then 1,3,5,7) so ties favour the cardinals.

   `ring`, `blocked` and `i` are deliberately reused as the tail's best /
   bestDir / outer counter: the target keeps all three in the same
   callee-saved registers across both halves, and separate locals cost 14
   instructions.  The inner scan must be written as `idx = i + j * 2` so GCC
   strength-reduces it into the second walking IV instead of reversing the
   counter (worth 5).  `row_number` is u16 — it only ever feeds MapObjectOverlapFind's u16
   parameter. */
/* func_800787F0 */
s32 MapObjectOpenFacingFind(MapObject *row, u16 row_number, s32 all_facings) {
    u8 counts[8];
    u16 spot_x;
    u16 spot_y;
    u16 blocked;
    s32 ring;
    s32 i;
    s32 one;
    s32 j;
    s32 idx;

    ring = 1;
    mem_clear(counts, 8);
    blocked = 0x55;
    if (all_facings != 0) {
        blocked = 0;
    }
    if (blocked != 0xFF) {
        one = 1;
        do {
            for (i = 0; i < 8; i++) {
                if (((blocked >> i) & 1) == 0) {
                    spot_x = row->record.sprite.x;
                    spot_y = row->record.sprite.depth;
                    MapSpotMove(i + 1, ring * 8, &spot_x, &spot_y);
                    if ((s16)spot_x < g_MapLimitLeft || (s16)spot_x >= g_MapLimitRight ||
                        (s16)spot_y < g_MapLimitTop || (s16)spot_y >= g_MapLimitBottom) {
                        blocked |= one << i;
                        continue;
                    }
                    if ((s16)spot_x < row->move.bounds.left || (s16)spot_x >= row->move.bounds.right ||
                        (s16)spot_y < row->move.bounds.top || (s16)spot_y >= row->move.bounds.bottom) {
                        blocked |= one << i;
                        break;
                    }
                    if (MapAttributeFreeCheck((s16)spot_x + row->box.dx,
                                                          (s16)spot_y + row->box.dy,
                                                          row->box.half_width, row->box.half_height,
                                                          row->layer, 0) == 0 ||
                        (MapObjectOverlapFind((s16)spot_x, (s16)spot_y, row_number) & 0xFFFF) != 0) {
                        blocked |= one << i;
                        continue;
                    }
                    counts[i]++;
                }
            }
            ring++;
        } while (blocked != 0xFF);
    }
    ring = 0;
    blocked = 0;
    i = 0;
    do {
        j = 0;
        do {
            idx = i + j * 2;
            if (ring < counts[idx]) {
                ring = counts[idx];
                blocked = idx + 1;
            }
            j++;
        } while (j < 4);
        i++;
    } while (i < 2);
    return blocked;
}
/* Picks the actor's next facing when it has to turn: probes the square to
   its left (facing-3) and to its right (facing+1) and prefers whichever is
   walkable, falling back to the about-face (+3).  arg1 != 0 skips the
   probing and always about-faces.

   Every branch returns `(x & 7) + 1` in full on purpose.  GCC cross-jumps
   the identical `andi`/`addiu` tail into one block placed before the
   register restores; hoisting it below the if/else as a single
   `return (v & 7) + 1;` instead sinks the `addiu` into the epilogue
   (80/81). */
/* func_80078A44 */
s8 MapObjectSideFacingGet(MapObject *row, s32 reverse) {
    u16 spot_x;
    u16 spot_y;
    s32 first_side_free;
    s32 d;

    if (reverse == 0) {
        d = ((row->facing - 3) & 7) + 1;
        spot_x = row->record.sprite.x;
        spot_y = row->record.sprite.depth;
        MapSpotMove(d, 1, &spot_x, &spot_y);
        first_side_free = MapAttributeFreeCheck((s16)spot_x + row->box.dx, (s16)spot_y + row->box.dy,
                                               row->box.half_width, row->box.half_height, row->layer, 0);
        d = ((row->facing + 1) & 7) + 1;
        spot_x = row->record.sprite.x;
        spot_y = row->record.sprite.depth;
        MapSpotMove(d, 1, &spot_x, &spot_y);
        if (MapAttributeFreeCheck((s16)spot_x + row->box.dx, (s16)spot_y + row->box.dy,
                                              row->box.half_width, row->box.half_height, row->layer, 0) != 0) {
            if (first_side_free == 0) {
                return ((row->facing + 1) & 7) + 1;
            }
            return ((row->facing + 3) & 7) + 1;
        }
        if (first_side_free != 0) {
            return ((row->facing - 3) & 7) + 1;
        }
        return ((row->facing + 3) & 7) + 1;
    }
    return ((row->facing + 3) & 7) + 1;
}
/* Pick a random facing for the map object's walk, then either turn to it
   in steps (unk3D bit6 set: MapObjectFacingTurnSet plans the turn and
   parks the actor in state 0xFE) or take it at once and start the walk
   (unk49 its length in thinks, unk48 = 0xFF).  arg1 selects the RNG
   shaping: 4 -> an odd facing 1, 3, 5, 7, 8 -> any facing 1..8, 0 ->
   re-roll an odd facing while it is the reverse of the current one.  var_a3 is
   left uninitialised for the other selectors (dead paths). */
/* func_80078B88 */
void MapObjectRandomWalkStart(MapObject *row, s32 draw_mode) {
    s32 draw;
    s32 facing;

    switch (draw_mode) {
    case 4:
        draw = rng_draw();
        facing = ((draw % 4) << 1) | 1;
        break;
    case 8:
        draw = rng_draw();
        facing = (draw % 8) + 1;
        break;
    case 0:
        do {
            draw = rng_draw();
            facing = ((draw % 4) << 1) | 1;
        } while (row->facing == ((((facing & 0xFFFF) + 3) & 7) + 1));
        break;
    }
    if (row->facing != (facing & 0xFFFF) && (row->number & 0x40)) {
        MapObjectFacingTurnSet(row, facing & 0xFFFF);
        row->thinks_left = 1;
        row->state = 0xFE;
        return;
    }
    row->facing = facing;
    row->thinks_left = value_div((rng_range_draw(row->length, 8, 0x10) & 0xFF) * 8, row->speed);
    row->state = 0xFF;
    MapAnimationPoseSet(row, -1, row->facing);
}
/* Ring search over the 8 facings, to plan a turn in steps: step one facing
   forward and one backward from row->unk3E until either lands on the target;
   unk4B encodes the step count (+1) with bit 7 set when the forward direction
   won. */
/* func_80078D00 */
void MapObjectFacingTurnSet(MapObject *row, s32 facing) {
    s32 count = 1;
    s32 fwd = row->facing;
    s32 bwd = row->facing;
    s32 target = facing & 0xFFFF;
    s32 v;
    row->turn_plan = 0;
loop:
    fwd = (fwd & 7) + 1;
    if (fwd == target) {
        goto forward;
    }
    bwd = ((bwd - 2) & 7) + 1;
    if (bwd == target) {
        goto done;
    }
    count++;
    goto loop;
forward:
    row->turn_plan = 0x80;
done:
    v = row->turn_plan + 1;
    row->turn_plan = count + v;
}
/* func_80078D5C */
s32 MapObjectFacingTurnStep(MapObject *row, s32 unused)
{
  s32 wait_passes;
  s32 facing_index;
  u8 wait_left;
  u8 turn_plan;
  s32 state;
  int a1v;
  wait_left = row->thinks_left - 1;
  row->thinks_left = wait_left;
  if (!(wait_left & 0xFF))
  {
    turn_plan = row->turn_plan - 1;
    row->turn_plan = turn_plan;
    if (turn_plan & 0x7F)
    {
      if (turn_plan & 0x80)
      {
        facing_index = row->facing & 7;
      }
      else
      {
        facing_index = (row->facing - 2) & 7;
      }
 /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
    block runs once, so behaviour is the same without the `do` / `while (0)`, but then 3
    of this function's 70 instruction lines differ. */
 do { row->facing = 1 + facing_index; } while (0);
      a1v = row->interval;
      wait_passes = 5;
      goto block_13;
    }
    state = row->state;
    row->turn_plan = 0;
    if (state == 0xFE)
    {
      row->thinks_left = value_div((rng_range_draw(row->length, 8U, 0x10U) & 0xFF) * 8, (s32) row->speed);
      goto block_9;
    }
    if (state == 0xFD)
    {
      block_9:
      row->state = 0xFF;

    }
    else
    {
      wait_passes = 0xF;
      if (state == 0xFC)
      {
        a1v = row->interval;
        wait_passes = 0x1E;
      }
      else
      {
        a1v = row->interval;
      }
      block_13:
      row->thinks_left = value_div(wait_passes, (s32) a1v);

    }
    MapAnimationPoseSet(row, -1, (s32) row->facing);
  }
  return 0;
}

/* func_80078E74 */
void MapObjectPartyLook00Step(s32 row, s32 row_number) {
    MapObjectWanderStep(row, row_number & 0xFFFF);
}
/* func_80078E94 */
s32 MapObjectPartyLook01Step(void) {
    return 0;
}
/* A map object that stands and turns about (unk43 2).  Waits (unk48 = 0xFF,
   unk49 = a rolled delay), then on each expiry latches the current facing
   into unk4A and rolls a count of turns into unk48.  While that count is
   still running it picks a random odd direction (1/3/5/7); on the last turn
   it restores the latched facing.
   If the actor must turn and unk3D bit6 allows it, MapObjectFacingTurnSet plans
   the turn in steps (unk49 = 1); otherwise it faces instantly and waits a
   fixed time.
   The unk49 store is written in BOTH branches on purpose — GCC tail-merges
   it into the join block, which is what places it before the call's $a0
   setup instead of sinking it into the delay slot. */
/* func_80078E9C */
s32 MapObjectPartyLook02Step(MapObject *row, s32 unused) {
    u8 dec;
    u8 dec2;
    u16 dir;

    if (row->state == 0) {
        row->thinks_left = value_div((rng_range_draw(row->length, 1, 3) & 0xFF) * 0x1E, row->interval);
        row->state = 0xFF;
    }
    dec = row->thinks_left - 1;
    row->thinks_left = dec;
    if (dec == 0) {
        if (row->state == 0xFF) {
            row->facing_kept = row->facing;
            row->state = rng_range_draw(row->count, 3, 5) + 1;
        }
        dec2 = row->state - 1;
        row->state = dec2;
        if (dec2 != 0) {
            dir = ((rng_draw() & 3) * 2) | 1;
        } else {
            dir = row->facing_kept;
        }
        if (row->facing != dir && (row->number & 0x40)) {
            MapObjectFacingTurnSet(row, dir);
            row->thinks_left = 1;
        } else {
            row->facing = dir;
            row->thinks_left = value_div(0xF, row->interval);
        }
        MapAnimationPoseSet(row, -1, row->facing);
    }
    return 0;
}
/* A map object that walks back to where its definition places it (unk43 3).
   First entry latches the actor's current facing into unk4A, takes the
   0x26-record's spot (record bytes +0x04/+0x05 x8 into unk4C/unk4E) as the
   goal and asks MapFacingTowardGet for the facing toward it.  If that
   facing differs from the current one and the ring-search bit (unk3D & 0x40)
   is set, it hands off to MapObjectFacingTurnSet and parks as 0xFE for one tick;
   otherwise it adopts the facing and marks itself ready (0xFF).

   Once walking, MapObjectTowardWalk drives the step and — once it has
   cleared unk4A (no step could be taken) — the walk ends: standing on the
   goal, the facing to end on is the record's +0x06 byte + 1, otherwise the
   facing is reversed (+3 mod 8, +1) and nudged on to the next odd facing.
   That facing either goes to MapObjectFacingTurnSet or is taken with a
   value_div wait, MapAnimationPoseSet sets the pose, and unk43's low bits
   are set to 2 (the object thinks next as MapObjectPartyLook02Step).

   `nf` must be `u16`: every `andi $a1,…,0xFFFF` in the target is that
   variable being promoted at a use site, and the `sb` stores its low byte
   unmasked.  Declaring it s32 with explicit `& 0xFFFF` masks costs 17
   instructions.  The 5th argument is `row->unk3E` re-read, NOT a local copy
   — a `u8 f` temp makes GCC mask in place ($v1) instead of into a fresh
   register ($v0).  `row->unk49 = v` is duplicated into both branches so GCC
   cross-jumps it ahead of the dispatch call. */
/* func_80078FD8 */
s32 MapObjectPartyLook03Step(MapObject *row, u16 row_number) {
    u8 *rec;
    s32 ret;
    u16 nf;
    s32 t;
    s32 v;

    rec = (u8 *)(g_MapObjectDefinitions + row->record.hold_definition.definition * 0x26);
    if (row->state == 0) {
        row->target.spot.x = rec[0x4] * 8;
        row->target.spot.y = rec[0x5] * 8;
        row->facing_kept = row->facing;
        nf = MapFacingTowardGet(row->record.sprite.x, row->record.sprite.depth, row->target.spot.x, row->target.spot.y, row->facing);
        if (nf != 0) {
            if (row->facing != nf && (row->number & 0x40)) {
                MapObjectFacingTurnSet(row, nf);
                row->thinks_left = 1;
                row->state = 0xFE;
                return 0;
            }
            row->facing = nf;
        }
        row->state = 0xFF;
    }
    ret = MapObjectTowardWalk(row, row_number, row->target.spot.x, row->target.spot.y);
    if (row->facing_kept == 0) {
        if (row->record.sprite.x == row->target.spot.x && row->record.sprite.depth == row->target.spot.y) {
            nf = rec[0x6] + 1;
        } else {
            t = ((row->facing + 3) & 7) + 1;
            nf = t;
            if ((t & 1) == 0) {
                nf = (t & 7) + 1;
            }
        }
        if (row->facing != nf && (row->number & 0x40)) {
            MapObjectFacingTurnSet(row, nf);
            v = 1;
            row->thinks_left = v;
        } else {
            row->facing = nf;
            v = value_div(0xF, row->interval);
            row->thinks_left = v;
        }
        MapAnimationPoseSet(row, -1, row->facing);
        row->state = 0;
        row->look = (row->look & 0x80) + 2;
    }
    return ret;
}
/* A map object that walks along the most open facing and looks to each
   side when it stops (unk43 4).  Starts a walk (unk48 = 0xFF plus a rolled
   length; with unk4A clear the facing from MapObjectOpenFacingFind), walks
   it through MapObjectFacingWalk, then on each expiry latches the facing into
   unk4A and rolls a count of turns; while that runs it turns the facing
   two places, ((f - 3) & 7) + 1 or ((f + 1) & 7) + 1 (bit 1 of the count
   selects which), unless unk3D bit6 defers the turn to unk4B.  Restores the
   latched facing on the last step.

   Three shape requirements:
   - MapObjectOpenFacingFind takes `row` as its FIRST argument; the target
     never materializes $a0 for it because row's home register is still
     live there.  Called with only the two visible arguments the whole
     register file shifts down one ($a1->$a0, $a2->$a1).
   - MapAnimationPoseSet's third argument is re-read through
     row->unk3E rather than reusing the just-stored local, which orders the
     `sb` before the argument setup (same as MapObjectPartyLook05Step).
   - the unk49 tail must be a single store fed by an if/else whose *constant*
     side is the taken branch.  The equivalent `var = 1; if (...) var = call;`
     costs the $v0/$v1 coloring, and duplicating the store into both branches
     costs 2 instructions. */
/* func_800791C0 */
s32 MapObjectPartyLook04Step(MapObject *row, s32 row_number) {
    s32 stopped_by;
    s32 moved;
    s32 facing_index;
    s32 next_wait;
    u8 wait_left;
    u8 turns_left;

    if (row->state == 0) {
        if (row->facing_kept == 0) {
            row->facing = MapObjectOpenFacingFind(row, row_number & 0xFFFF, 1);
        }
        row->thinks_left = value_div((rng_range_draw(row->length, 8U, 0x10U) & 0xFF) * 8, (s32) row->speed);
        row->state = 0xFF;
    }
    moved = 0;
    if (row->state == 0xFF) {
        moved = MapObjectFacingWalk(row, row_number & 0xFFFF, &stopped_by);
        if ((row->facing_kept == 0) && (stopped_by != 1)) {
            row->facing = MapObjectSideFacingGet(row, 1U);
            MapAnimationPoseSet(row, -1, row->facing);
        }
    }
    wait_left = row->thinks_left - 1;
    row->thinks_left = wait_left;
    if (!(wait_left & 0xFF)) {
        if (row->state == 0xFF) {
            row->facing_kept = row->facing;
            row->state = ((rng_range_draw(row->count, 3U, 5U) & 0xFF) * 2) + 1;
        }
        turns_left = row->state - 1;
        row->state = turns_left;
        if (!(turns_left & 0xFF)) {
            row->facing = row->facing_kept;
        } else {
            if (turns_left & 2) {
                if (row->number & 0x40) {
                    row->turn_plan = 3;
                } else {
                    facing_index = row->facing - 3;
                    goto block_20;
                }
            } else if (row->number & 0x40) {
                row->turn_plan = 0x83;
            } else {
                facing_index = row->facing + 1;
block_20:
                row->facing = (facing_index & 7) + 1;
            }
            if (row->turn_plan != 0) {
                next_wait = 1;
            } else {
                next_wait = value_div(0xF, (s32) row->interval);
            }
            row->thinks_left = (u8) next_wait;
        }
        MapAnimationPoseSet(row, -1, (s32) row->facing);
    }
    return moved;
}
/* A map object that walks along straight facings drawn at random (unk43 5).
   Starts a random walk if idle (MapObjectRandomWalkStart), then while the
   actor is in the "ready" state (unk48 == 0xFF) walks via MapObjectFacingWalk,
   re-faces the actor (MapObjectSideFacingGet) when something other than the
   first party record stopped it, and counts unk49 down — reaching 0 returns
   the actor to idle.
   The third argument to MapAnimationPoseSet is read back through
   row->unk3E rather than reusing the local: GCC CSEs it to the value just
   stored, which is what orders the `sb` before the argument setup instead of
   sinking it into the call's delay slot. */
/* func_800793BC */
s32 MapObjectPartyLook05Step(MapObject *row, u16 row_number) {
    s32 result;
    s32 stopped_by;
    u8 dec;
    s8 face;

    if (row->state == 0) {
        MapObjectRandomWalkStart(row, 0);
    }
    result = 0;
    if (row->state == 0xFF) {
        result = MapObjectFacingWalk(row, row_number, &stopped_by);
        if (row->facing_kept == 0 && stopped_by != 1) {
            face = MapObjectSideFacingGet(row, row->count);
            row->facing = face;
            MapAnimationPoseSet(row, -1, row->facing);
        }
        dec = row->thinks_left - 1;
        row->thinks_left = dec;
        if (dec == 0) {
            row->state = 0;
        }
    }
    return result;
}
/* A map object that walks along straight facings drawn at random and stops
   for a while after each walk (unk43 6).  Same skeleton as
   MapObjectPartyLook05Step (idle → start a random walk; ready → walk +
   re-face) but always passes 0 to MapObjectSideFacingGet, and its countdown
   tail also runs while parked (unk48 != 0xFE): on reaching 0 a ready actor
   rolls its stop's length into unk49 and parks at unk48 = 0x90, otherwise
   it returns to idle. */
/* func_80079488 */
s32 MapObjectPartyLook06Step(MapObject *row, u16 row_number) {
    s32 result;
    s32 stopped_by;
    u8 dec;
    s8 face;

    if (row->state == 0) {
        MapObjectRandomWalkStart(row, 0);
    }
    result = 0;
    if (row->state == 0xFF) {
        result = MapObjectFacingWalk(row, row_number, &stopped_by);
        if (row->facing_kept == 0 && stopped_by != 1) {
            face = MapObjectSideFacingGet(row, 0);
            row->facing = face;
            MapAnimationPoseSet(row, -1, row->facing);
        }
    }
    if (row->state != 0xFE) {
        dec = row->thinks_left - 1;
        row->thinks_left = dec;
        if (dec == 0) {
            if (row->state == 0xFF) {
                row->thinks_left = value_div((rng_range_draw(row->count, 1, 3) & 0xFF) * 0x1E, row->interval);
                row->state = 0x90;
            } else {
                row->state = 0;
            }
        }
    }
    return result;
}
/* func_800795A0 */
s32 MapObjectPartyLook07Step(MapObject *row, s32 row_number) {
    s32 result;
    if (row->state == 0) {
        MapObjectRandomWalkStart(row, 0);
    }
    result = 0;
    if (row->state == 0xFF) {
        result = MapObjectPartyLook06Step(row, row_number & 0xFFFF);
        if (row->state == 0x90) {
            row->facing_kept = row->facing;
            row->state = rng_range_draw(row->count, 3, 5) + 1;
            row->thinks_left = 1;
        }
    }
    if ((s8)row->state >= 0) {
        MapObjectPartyLook02Step(row, row_number & 0xFFFF);
    }
    return result;
}
/* func_80079658 */
void MapObjectPartyLook08Step(MapObject *row, s32 row_number) {
    if (row->state == 0) {
        MapObjectRandomWalkStart(row, 8);
    }
    MapObjectPartyLook05Step(row, row_number & 0xFFFF);
}
/* func_800796A4 */
void MapObjectPartyLook09Step(MapObject *row, s32 row_number) {
    if (row->state == 0) {
        MapObjectRandomWalkStart(row, 8);
    }
    MapObjectPartyLook06Step(row, row_number & 0xFFFF);
}
/* A map object that walks on and on, turning only where it is stopped
   (unk43 0xA).  On entry rolls a random direction — an even step
   (r%4)*2 turned into an odd facing (+1), bumped to +2 when bit0 of byte
   +0x6 of this actor's 0x26-stride record (*g_MapObjectDefinitions, indexed by unk01)
   is set.  Then shares MapObjectPartyLook05Step's tail: walk via MapObjectFacingWalk
   and re-face when something other than the first party record stopped it. */
/* func_800796F0 */
s32 MapObjectPartyLook0AStep(MapObject *row, u16 row_number) {
    s32 stopped_by;
    s32 result;
    s32 r;
    s32 dir;
    s8 face;
    u8 *e;

    if (row->state == 0) {
        r = rng_draw();
        dir = (r % 4) * 2;
        row->facing = dir + 1;
        e = (u8 *)(row->record.hold_definition.definition * 0x26 + g_MapObjectDefinitions);
        if (e[6] & 1) {
            row->facing = dir + 2;
        }
        row->state = 0xFF;
    }
    result = MapObjectFacingWalk(row, row_number, &stopped_by);
    if (row->facing_kept == 0 && stopped_by != 1) {
        face = MapObjectSideFacingGet(row, 0);
        row->facing = face;
        MapAnimationPoseSet(row, -1, row->facing);
    }
    return result;
}
/* A map object that waits and then is put at once on a random spot inside
   its bounds (unk43 0xB).  Three-phase state machine driven by the unk49
   countdown: 0xFF (waiting) → 0x90 → 0xA0 (the move itself: the end of
   0x90 fires MapObjectRandomSpotPlace and is the only step that returns 1),
   and any other state returns the actor to idle.  Each step sets its own
   pose with MapAnimationPoseSet (4 / 5 / 1).
   The three MapAnimationPoseSet calls are written out per-branch on
   purpose: GCC tail-merges them into the single shared `jal` the target has,
   and that merge is what routes the unk3E reads through $a0. Hoisting the
   call out with a1/a2 temporaries instead costs 3 instructions. */
/* func_800797E8 */
s32 MapObjectPartyLook0BStep(MapObject *row, u16 row_number) {
    s32 result;
    u8 dec;
    u8 st;

    if (row->state == 0) {
        row->thinks_left = value_div((rng_range_draw(row->length, 1, 3) & 0xFF) * 0x1E, row->interval);
        row->state = 0xFF;
    }
    dec = row->thinks_left - 1;
    row->thinks_left = dec;
    result = 0;
    if (dec == 0) {
        st = row->state;
        if (st == 0xFF) {
            row->state = 0x90;
            row->thinks_left = value_div(0x10, row->interval);
            MapAnimationPoseSet(row, 4, row->facing);
        } else if (st == 0x90) {
            row->state = 0xA0;
            row->thinks_left = value_div(0x10, row->interval);
            result = 1;
            MapObjectRandomSpotPlace(row, row_number);
            MapAnimationPoseSet(row, 5, row->facing);
        } else {
            row->state = 0;
            MapAnimationPoseSet(row, 1, row->facing);
        }
    }
    return result;
}
/* func_80079908 */
s32 MapObjectPartyLook0CStep(MapObject *row, s32 row_number) {
    s32 result;
    if (row->state == 0) {
        MapObjectRandomWalkStart(row, 8);
    }
    result = 0;
    if (row->state == 0xFF) {
        result = MapObjectPartyLook06Step(row, row_number & 0xFFFF);
        if (row->state == 0x90) {
            row->thinks_left = value_div(0x10, row->interval);
            MapAnimationPoseSet(row, 4, row->facing);
        }
    } else if (row->state != 0xFE) {
        result = MapObjectPartyLook0BStep(row, row_number & 0xFFFF);
    }
    return result;
}
/* A map object that walks after the first party record (definition +0x1E
   1).  Latches the party leader's position into unk4C/unk4E while it sees
   it (unk48 == 0xFF); once parked at 0x80 (no longer seen) it waits until
   the actor has actually reached that spot (or was stopped, unk4A clear),
   then latches the facing and rolls a wait.  While unk48 has bit7 set
   MapObjectTowardWalk walks it to the spot; otherwise control falls through
   to the ordinary MapObjectPartySeen05Step tick. */
/* func_800799C4 */
void MapObjectPartySeen01Step(MapObject *row, u16 row_number) {
    u8 *rec;
    s32 off;
    u8 st;

    off = row->record.hold_definition.definition * 0x26;
    rec = (u8 *)(g_MapObjectDefinitions + off);
    if (row->state == 0) {
        row->state = 0xFF;
        MapAnimationPoseSet(row, 2, row->facing);
    }
    st = row->state;
    if (st == 0xFF) {
        row->target.spot.x = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.x);
        row->target.spot.y = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.depth);
    } else if (st == 0x80 &&
               ((row->target.spot.x == row->record.sprite.x && row->target.spot.y == row->record.sprite.depth) ||
                row->facing_kept == 0)) {
        row->facing_kept = row->facing;
        row->thinks_left = value_div((rng_range_draw(rec[0x21], 0, 2) & 0xFF) * 0x1E, row->interval) + 1;
        row->state = 1;
    }
    if ((s8)row->state < 0) {
        MapObjectTowardWalk(row, row_number, row->target.spot.x, row->target.spot.y);
    } else {
        MapObjectPartySeen05Step(row, row_number);
    }
}
/* A map object that, once it has seen the first party record, walks to a
   spot its definition gives and there may run a script label (definition
   +0x1E 2).  On first entry (unk48 == 0) it targets the spot in the actor's
   own 0x26-record — record bytes +0x23/+0x24 scaled by 8 into
   unk4C/unk4E — asks MapFacingTowardGet for the facing that points there and, if
   there is one, turns to it (MapObjectFacingSet); it sets pose 2
   either way and latches busy (unk48 = 0xFF) only when the turn was taken
   at once (non-zero), otherwise the tick yields 0.

   While busy it drives the walk (MapObjectTowardWalk) and, once the actor either
   stands on its target tile or was stopped (unk4A clear), with record byte
   +0x23 not 0 looks at the attribute there: attributes whose top bits are 4
   (`(u16)t >> 7 == 4`) run label `t & 0x7F` through ScriptLabelRun.  It
   then finishes with MapObjectNumberReset.

   `ret` is deliberately read before assignment on the dir == 0 path — the
   target does the same (`$s1` is live-in to the `bnez` with no initialiser),
   which is why the first block cannot be restructured to initialise it. */
/* func_80079B18 */
s32 MapObjectPartySeen02Step(MapObject *row, u16 row_number) {
    u8 *rec;
    s32 ret;
    s32 dir;
    s32 y;
    s32 tile;

    rec = (u8 *)(g_MapObjectDefinitions + row->record.hold_definition.definition * 0x26);
    if (row->state == 0) {
        row->target.spot.x = rec[0x23] * 8;
        y = rec[0x24] * 8;
        row->target.spot.y = y;
        dir = MapFacingTowardGet(row->record.sprite.x, row->record.sprite.depth, row->target.spot.x, y, row->facing) & 0xFFFF;
        if (dir != 0) {
            ret = MapObjectFacingSet(row, dir);
        }
        MapAnimationPoseSet(row, 2, row->facing);
        if (ret == 0) {
            return 0;
        }
        row->state = 0xFF;
    }
    ret = 0;
    if ((s8)row->state < 0) {
        ret = MapObjectTowardWalk(row, row_number, row->target.spot.x, row->target.spot.y);
        if ((row->target.spot.x == row->record.sprite.x && row->target.spot.y == row->record.sprite.depth) ||
            row->facing_kept == 0) {
            if (rec[0x23] != 0) {
                tile = MapAttributeFind(row->record.sprite.x + row->box.dx,
                                                         row->record.sprite.y + row->box.dy,
                                                         row->box.half_width, row->box.half_height);
                if (((u16) tile >> 7) == 4) {
                    ScriptLabelRun(tile & 0x7F);
                }
            }
            MapObjectNumberReset(row, rec);
        }
    }
    return ret;
}
/* A map object that turns to the first party record, waits, and walks
   straight along that facing until it is stopped (definition +0x1E 3).
   First entry steers toward the party leader
   (*g_MapPartyRecords +6/+8), parks as 0xFC for a value_div(0x1E, unk41)
   countdown, and goes on as 0xFF (pose 2) on expiry.  The walk goes
   through MapObjectFacingWalk, whose out-parameter says who stopped it
   (row index + 2 into the 0x60-stride g_MapObjects table); record byte +0x23
   selects the follow-up (1 = set that row's byte +0x3D low bits to 4, 2 =
   MapAnimationPoseSet(that row, -1, 1)) but only when byte 1 of that row's
   record matches record byte +0x24.

   The slot pointer must be POINTER arithmetic on a cast base
   (`(u8 *)(off + g_MapObjects)`), not integer arithmetic — that one reshape
   is the difference between 159/160 and a match. */
/* func_80079CAC */
s32 MapObjectPartySeen03Step(MapObject *row, u16 row_number)
{
  s32 stopped_by;
  u8 *rec;
  u8 *slot;
  s32 ret;
  s32 dir;
  s32 off;
  u8 mode;
  rec = (u8 *) (g_MapObjectDefinitions + (row->record.hold_definition.definition * 0x26));
  if (row->state == 0)
  {
    dir = MapFacingTowardGet(row->record.sprite.x, row->record.sprite.depth, ((MapRecord *)g_MapPartyRecords)->sprite.x, ((MapRecord *)g_MapPartyRecords)->sprite.depth, row->facing) & 0xFFFF;
    if (dir != 0)
    {
      ret = MapObjectFacingSet(row, dir);
    }
    row->state = 0xFC;
    MapAnimationPoseSet(row, 3, row->facing);
    if (ret == 0)
    {
      return 0;
    }
    row->thinks_left = value_div(0x1E, row->interval);
  }
  if (((s8) row->state) < 0)
  {
    if (row->state == 0xFC)
    {
      if (--row->thinks_left == 0)
      {
        row->state = 0xFF;
        MapAnimationPoseSet(row, 2, row->facing);
      }
      return 0;
    }
    ret = MapObjectFacingWalk(row, row_number, &stopped_by);
    if (row->facing_kept == 0)
    {
      row->facing_kept = row->facing;
      row->thinks_left = value_div(((rng_range_draw(rec[0x21], 0, 2) & 0xFF) * 0x1E) + 0xF, row->interval) + 1;
      row->state = 1;
      MapAnimationPoseSet(row, 1, row->facing);
      row->target.overlapped = 0;
      mode = rec[0x23];
      if (mode != 0)
      {
        off = stopped_by * 0x60;
        slot = (u8 *) (off + g_MapObjects);
        if (((u8 *) (g_MapObjectDefinitions + (slot[-0xBF] * 0x26)))[1] == rec[0x24])
        {
          row->target.overlapped = stopped_by;
          if (mode == 1)
          {
            slot[-0x83] = (slot[-0x83] & 0xC0) | 4;
          }
          else
            if (mode == 2)
          {
            MapAnimationPoseSet((MapObject *) (((u8 *) g_MapObjects) + (off - 0xC0)), -1, 1);
          }
        }
      }
    }
  }
  else
  {
    ret = MapObjectPartySeen05Step(row, row_number);
    if ((row->thinks_left == 0) && (row->target.overlapped != 0))
    {
      MapObjectOverlappedRemove(row);
    }
  }
  return ret;
}
/* A map object that walks along its facing and turns one place toward the
   first party record now and then (definition +0x1E 4).  First entry latches
   busy (unk48 = 0xFF), seeds a 1-tick countdown and sets pose 2.  Each
   expiry while still in the ready
   state re-steers toward the party leader (MapFacingTowardGet over this actor's
   and the leader's coordinates): a changed direction plans the turn via
   MapObjectFacingTurnSet, of which only the first place is taken (by unk4B's
   bit 0x80) and given to MapAnimationPoseSet, then rolls the next walk's
   length from byte +0x21 of the actor's 0x26-stride record.  In any other state the record is handed to MapObjectNumberReset and the
   tick ends.  The tail resolves through MapObjectFacingWalk like MapObjectPartyLook05Step.
   Two shapes are load-bearing (do not simplify):
   - `row->unk3E = t + 1;` is written out in BOTH turn branches so GCC
     cross-jumps the shared tail itself (hoisting the store below the
     if/else loses the match);
   - the dispatch argument re-reads unk3E through a point-of-use volatile
     cast to force the target's lbu reload. */
/* func_80079F2C */
s32 MapObjectPartySeen04Step(MapObject *row, u16 row_number) {
    s32 stopped_by;
    u8 *rec;
    s32 off;
    u8 dec;
    s32 dir;
    s32 t;

    off = row->record.hold_definition.definition * 0x26;
    rec = (u8 *)(g_MapObjectDefinitions + off);
    if (row->state == 0) {
        row->state = 0xFF;
        row->thinks_left = 1;
        MapAnimationPoseSet(row, 2, row->facing);
    }
    dec = row->thinks_left - 1;
    row->thinks_left = dec;
    if (dec == 0) {
        if (row->state == 0xFF) {
            dir = MapFacingTowardGet(row->record.sprite.x, row->record.sprite.depth, ((MapRecord *)g_MapPartyRecords)->sprite.x, ((MapRecord *)g_MapPartyRecords)->sprite.depth, row->facing) & 0xFFFF;
            if (dir != 0 && row->facing != dir) {
                MapObjectFacingTurnSet(row, dir);
                if (row->turn_plan & 0x80) {
                    t = row->facing & 7;
                    row->facing = t + 1;
                } else {
                    t = (row->facing - 2) & 7;
                    row->facing = t + 1;
                }
                row->turn_plan = 0;
                MapAnimationPoseSet(row, -1, *(volatile u8 *)&row->facing);
            }
            row->thinks_left = value_div((rng_range_draw(rec[0x21], 8, 0x10) & 0xFF) * 8, row->speed);
            row->state = 0xFF;
        } else {
            MapObjectNumberReset(row, rec);
            return 0;
        }
    }
    return MapObjectFacingWalk(row, row_number, &stopped_by);
}
/* A map object that stands and faces the first party record while it sees
   it (definition +0x1E 5).  On first entry latches the current facing into
   unk4A and sets pose 2.  While unk48 == 0xFF it steers toward the party
   leader every tick (MapFacingTowardGet(self.xy, leader.xy, facing) returns the
   new facing).  unk48 == 0x80 (no longer seen) rolls a wait from byte +0x21 of this
   actor's 0x26-stride record and drops to state 1; any other state counts
   unk49 down and, at zero, restores the latched facing and hands off to
   MapObjectNumberReset. */
/* func_8007A0B4 */
s32 MapObjectPartySeen05Step(MapObject *row, s32 unused) {
    u8 *rec;
    s32 off;
    u8 st;
    u8 dec;
    s32 v;

    off = row->record.hold_definition.definition * 0x26;
    rec = (u8 *)(g_MapObjectDefinitions + off);
    if (row->state == 0) {
        row->state = 0xFF;
        row->facing_kept = row->facing;
        MapAnimationPoseSet(row, 2, row->facing);
    }
    st = row->state;
    if (st == 0xFF) {
        v = MapFacingTowardGet(row->record.sprite.x, row->record.sprite.depth, ((MapRecord *)g_MapPartyRecords)->sprite.x,
                          ((MapRecord *)g_MapPartyRecords)->sprite.depth, row->facing);
        row->facing = v;
        MapAnimationPoseSet(row, -1, row->facing);
    } else if (st == 0x80) {
        row->thinks_left = value_div((rng_range_draw(rec[0x21], 0, 2) & 0xFF) * 0x1E, row->interval) + 1;
        row->state = 1;
    } else {
        dec = row->thinks_left - 1;
        row->thinks_left = dec;
        if (dec == 0) {
            row->facing = row->facing_kept;
            MapObjectNumberReset(row, rec);
        }
    }
    return 0;
}
/* A map object that turns its back on the first party record and walks
   away from it (definition +0x1E 6, and MapObjectPartyAwayStep's every
   think).  On first entry latches the leader's position into
   unk4C/unk4E, steers away from it (the reverse of MapFacingTowardGet's
   facing), and only commits (unk48 = 0xFF) if
   MapObjectFacingSet took the turn at once — otherwise it bails for this tick.
   Thereafter it re-latches the leader position while it sees it, rolls a
   wait at 0x80, counts down whenever bit7 of unk48 is clear
   (MapObjectNumberReset at zero), and otherwise walks away through
   MapObjectAwayWalk.
   `turned_at_once` is deliberately uninitialised: on the path where MapFacingTowardGet
   returns 0 the target reads $s1 without ever writing it. */
/* func_8007A1E0 */
s32 MapObjectPartySeen06Step(MapObject *row, u16 row_number) {
    s32 turned_at_once;
    u8 *rec;
    s32 off;
    s32 v;
    u16 ty;
    u8 st;
    u8 dec;

    off = row->record.hold_definition.definition * 0x26;
    rec = (u8 *)(g_MapObjectDefinitions + off);
    if (row->state == 0) {
        row->target.spot.x = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.x);
        ty = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.depth);
        row->target.spot.y = ty;
        v = MapFacingTowardGet(row->record.sprite.x, row->record.sprite.depth, row->target.spot.x, (s16)ty, row->facing);
        if (v & 0xFFFF) {
            turned_at_once = MapObjectFacingSet(row, ((v + 3) & 7) + 1);
        }
        MapAnimationPoseSet(row, 2, row->facing);
        if (turned_at_once == 0) {
            return 0;
        }
        row->state = 0xFF;
    }
    st = row->state;
    if (st == 0xFF) {
        row->target.spot.x = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.x);
        row->target.spot.y = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.depth);
    } else if (st == 0x80) {
        row->thinks_left = value_div((rng_range_draw(rec[0x21], 0, 2) & 0xFF) * 0x1E, row->interval) + 1;
        row->state = 1;
    }
    if ((s8)row->state >= 0) {
        dec = row->thinks_left - 1;
        row->thinks_left = dec;
        if (dec == 0) {
            MapObjectNumberReset(row, rec);
            return 0;
        }
    }
    return MapObjectAwayWalk(row, row_number, row->target.spot.x, row->target.spot.y);
}
/* Box against a strip along a diagonal.  arg6[2]/box[3] are the box's
   half width and half height, box_x/box_y its centre, strip_x0..arg3 the strip's
   two corners as (x0,y0,x1,y1); returns 9 on overlap else 0 (0 also for a
   half width of 0).  Four tests in the diagonal measures y - x and x + y,
   each an early return.

   The `if (strip_x1) {...} else {...}` with IDENTICAL arms is a CODEGEN FORCING
   DEVICE, not logic.  It splits the five extent
   computations into their own basic block, which is what puts the five input
   scalars in the target's registers; GCC does not fold the duplicate arms
   away.  Removing it costs the match outright (35/35 -> 23/35), and so does
   replacing it with a `do {} while (0)` wrapper (23/35), so the branch
   itself — not merely the extra block — is what matters.  Same category as
   the dead `other` reference in FaceListByteMove. */
/* func_8007A37C */
s32 MapBoxDiagonalTouchGet(s32 strip_x0, s32 strip_y0, s32 strip_x1, s32 strip_y1, s32 box_x, s32 box_y,
                  u8 *box) {
    s32 r;
    s32 s;
    s32 lo1;
    int second_diagonal;
    s32 lo2;
    s32 hi1;
    s32 hi2;

    r = box[2];
    if (r == 0) {
        return 0;
    }
    if (strip_x1) {
        s = box[3];
        lo1 = box_x - r;
        hi1 = box_x + r;
        lo2 = box_y - s;
        hi2 = box_y + s;
    } else {
        s = box[3];
        lo1 = box_x - r;
        hi1 = box_x + r;
        lo2 = box_y - s;
        hi2 = box_y + s;
    }
    if ((strip_y0 - strip_x0) < (hi2 - hi1)) {
        return 0;
    }
    second_diagonal = strip_y1 - strip_x1;
    if ((lo2 - lo1) < second_diagonal) {
        return 0;
    }
    if ((strip_y0 + strip_x0) > (lo2 + hi1)) {
        return 0;
    }
    if ((strip_y1 + strip_x1) < (hi2 + lo1)) {
        return 0;
    }
    return 9;
}
/* func_8007A408 */
void MapObjectNumberReset(MapObject *row, u8 *definition) {
    row->number = definition[0xC] | (row->number & 0xC0);
    row->state = 0;
    MapObjectMoveSet(row, definition, 1);
    MapAnimationPoseSet(row, 1, row->facing);
}
/* Takes off the map the object that stopped this one's walk, and sets that
   object's story bit; it plays nothing.  unk4C (slot index + 2, kept by
   MapObjectPartySeen03Step) selects a 0x60-stride entry of *g_MapObjects;
   that slot's byte 1 is an actor id whose 0x26-stride record holds byte 2.
   When that byte is not 0: slots of kind below 4 (slot[0] < 4) set story
   bit byte 2 + 0x580; others set the bit of slot[1] plus the g_MapNumber
   bank page (* 0x80, - 0x2C00).  Clears unk4C and the slot's byte 0 (out
   of use) either way.
   Do not simplify: `off` is deliberately ONE variable reused for both stride
   computations — slot[1]'s first load must die so the else branch reloads it
   (the target has two lbu's; a fresh local keeps it live and loses the
   match). */
/* func_8007A458 */
void MapObjectOverlappedRemove(MapObject *row) {
    u8 *slot;
    u16 id;
    u8 *rec;
    s32 off;

    off = (row->target.overlapped * 0x60) - 0xC0;
    slot = (u8 *)(g_MapObjects + off);
    off = slot[1] * 0x26;
    rec = (u8 *)(g_MapObjectDefinitions + off);
    if (rec[2] != 0) {
        if (slot[0] < 4) {
            StoryBitSet(rec[2] + 0x580);
        } else {
            id = slot[1] + ((g_MapNumber << 7) - 0x2C00);
            StoryBitSet(id & 0xFFFF);
        }
    }
    row->target.overlapped = 0;
    slot[0] = 0;
}
/* func_8007A51C */
s32 MapObjectFacingSet(MapObject *row, s32 facing) {
    if (row->facing != (facing & 0xFFFF) && (row->number & 0x40)) {
        MapObjectFacingTurnSet(row, facing & 0xFFFF);
        row->thinks_left = 1;
        row->state = 0xFD;
        return 0;
    }
    row->facing = facing;
    return 1;
}
/* far_syms: g_MapPartyBoxDx_blk g_MapObjectPartySeenSounds g_MoveSpeedPixels g_MoveSpeedIntervals g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
extern s32 g_MapObjectCursor;
extern s8 g_MapPartyBoxDx;
extern s8 g_MapPartyBoxDy;
extern s8 g_MapPartyBoxDx_blk[4];
extern s32 g_MapProbeBoxX;
extern s32 g_MapProbeBoxY;
extern s32 g_MapProbeBoxHalfWidth;
extern s32 g_MapProbeBoxHalfHeight;
extern u8 g_PartyLevelLeast;
extern u16 g_MapObjectPartySeenSounds[];
extern u8 g_MoveSpeedPixels[];
extern u8 g_MoveSpeedIntervals[];
s32 MapBoxTouchGet(s32, s32, s8 *);
s32 MapAttributeLineCheck();

/* Contact check for actor `a` against the probe box at g_MapPartyRecords
   (+ the g_MapPartyBoxDx/29 offsets). Its 0x26-byte record is g_MapObjectDefinitions[a+1].
   Mode 8 (or record flag 0x80) tests one square of radius 0x60 / record
   +0x1F*8 / actor +0x44*8 via MapBoxTouchGet; otherwise the facing (+0x3E,
   1-8 clockwise) picks an axis box (odd, through g_MapProbeBoxX..BE0) or a
   diagonal strip (even, MapBoxDiagonalTouchGet) from the w/h at actor +0x44/45 or
   record +0x1F/20 (mode 7). Result 9 confirms on the attribute plane
   (MapAttributeLineCheck); a hit switches the mode to 8 (or
   raises the record's event and mode 7) and loads the reaction timers.

   Source shape the match needs: the probe box's
   address is passed as g_MapPartyBoxDx_blk declared SIZED ([4]) plus the
   far_syms marker — cc1 then emits one `la` (never split, never put in a
   delay slot) and GAS expands it %hi/%lo; an unsized [] makes cc1 split it
   into HIGH/LO_SUM, which the schedulers move apart. The radius reuses `h`
   (one pseudo with the height: the target keeps both in $t1); the mode
   tests repeat `a[0x3D] & 0x3F` instead of a variable; the 0xFF guard is a
   nested if (an && lets fold_range_test drop it). `box` is the 24 bytes of
   frame no access reaches. */
/* func_8007A58C */
void MapObjectPartySeenCheck(u8 *a, u16 id)
{
  u8 *rec;
  u16 x;
  u16 y;
  s16 w;
  s16 h;
  s32 hit;
  u16 res;
  u8 m;
  s32 box[6];
  rec = (u8 *) (g_MapObjectDefinitions + (a[1] * 0x26));
  if (((((MapObject *)a)->look & 0x7F) == 0) || ((rec[0x1E] & 0x7F) == 0))
  {
    if (g_MapObjectCursor == id)
    {
      sprite_run_off(0x6C, 1);
    }
    return;
  }
  x = AS(u16, ((MapObject *)a)->record.sprite.x);
  y = AS(u16, ((MapObject *)a)->record.sprite.depth);
  hit = 0;
  if ((rec[0x1E] & 0x80) || ((((MapObject *)a)->number & 0x3F) == 8))
  {
    if ((((MapObject *)a)->number & 0x3F) == 8)
    {
      h = 0x60;
    }
    else
      if ((((MapObject *)a)->number & 0x3F) == 7)
    {
      h = rec[0x1F] * 8;
    }
    else
    {
      h = ((MapObject *)a)->sight_width * 8;
    }
    g_MapProbeBoxX = (s16) x;
    g_MapProbeBoxY = (s16) y;
    g_MapProbeBoxHalfWidth = h;
    g_MapProbeBoxHalfHeight = h;
    res = MapBoxTouchGet(((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, g_MapPartyBoxDx_blk);
  }
  else
  {
    if ((((MapObject *)a)->number & 0x3F) == 7)
    {
      w = rec[0x1F] * 4;
      h = rec[0x20] * 4;
    }
    else
    {
      w = ((MapObject *)a)->sight_width * 4;
      h = ((MapObject *)a)->sight_length * 4;
    }
    res = 0;
    switch (((MapObject *)a)->facing)
    {
      case 1:
        g_MapProbeBoxX = (s16) x;
        g_MapProbeBoxY = ((s16) y) - h;
        g_MapProbeBoxHalfWidth = w;
        g_MapProbeBoxHalfHeight = h;
        res = MapBoxTouchGet(((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, g_MapPartyBoxDx_blk);
        break;

      case 2:
        res = MapBoxDiagonalTouchGet(((s16) x) - w, ((s16) y) - w, (((s16) x) + (h * 2)) + w, (((s16) y) - (h * 2)) + w, ((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, (u8 *) g_MapPartyBoxDx_blk);
        break;

      case 3:
        g_MapProbeBoxX = ((s16) x) + h;
        g_MapProbeBoxY = (s16) y;
        g_MapProbeBoxHalfWidth = h;
        g_MapProbeBoxHalfHeight = w;
        res = MapBoxTouchGet(((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, g_MapPartyBoxDx_blk);
        break;

      case 4:
        res = MapBoxDiagonalTouchGet(((s16) x) - w, ((s16) y) + w, (((s16) x) + (h * 2)) + w, (((s16) y) + (h * 2)) - w, ((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, (u8 *) g_MapPartyBoxDx_blk);
        break;

      case 5:
        g_MapProbeBoxX = (s16) x;
        g_MapProbeBoxY = ((s16) y) + h;
        g_MapProbeBoxHalfWidth = w;
        g_MapProbeBoxHalfHeight = h;
        res = MapBoxTouchGet(((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, g_MapPartyBoxDx_blk);
        break;

      case 6:
        res = MapBoxDiagonalTouchGet((((s16) x) - (h * 2)) - w, (((s16) y) + (h * 2)) - w, ((s16) x) + w, ((s16) y) + w, ((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, (u8 *) g_MapPartyBoxDx_blk);
        break;

      case 7:
        g_MapProbeBoxX = ((s16) x) - h;
        g_MapProbeBoxY = (s16) y;
        g_MapProbeBoxHalfWidth = h;
        g_MapProbeBoxHalfHeight = w;
        res = MapBoxTouchGet(((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, g_MapPartyBoxDx_blk);
        break;

      case 8:
        res = MapBoxDiagonalTouchGet((((s16) x) - (h * 2)) - w, (((s16) y) - (h * 2)) + w, ((s16) x) + w, ((s16) y) - w, ((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.depth + g_MapPartyBoxDy, (u8 *) g_MapPartyBoxDx_blk);
        break;

    }

  }
  if (res == 9)
  {
    hit = MapAttributeLineCheck(a + 6, g_MapPartyRecords + 6, 1);
  }
  if (hit)
  {
    m = ((MapObject *)a)->number;
    if ((m & 0x3F) < 7)
    {
      if ((rec[0x25] < g_PartyLevelLeast) || ((rec[0x1E] & 0x7F) == 6))
      {
        ((MapObject *)a)->number = (m & 0xC0) | 8;
      }
      else
      {
        SoundPlay(g_MapObjectPartySeenSounds[rec[1] - 0x1F], 2);
        ((MapObject *)a)->number = (((MapObject *)a)->number & 0xC0) | 7;
      }
      if (g_PadActuatorList == 0)
      {
        g_PadActuatorList = (s32) g_MapActuator64List;
        g_PadActuatorRestFlags = 0x3F;
        g_PadActuatorListWait = 0;
      }
      ((MapObject *)a)->speed = g_MoveSpeedPixels[rec[0x22] + 2];
      ((MapObject *)a)->interval_left = (((MapObject *)a)->interval = g_MoveSpeedIntervals[rec[0x22] + 2]);
      ((MapObject *)a)->state = 0;
      ((MapObject *)a)->turn_plan = 0;
    }
    else if (((MapObject *)a)->state != 0xFF)
    {
      if (((u8) (((MapObject *)a)->state + 0x80)) < 0x70)
      {
        ((MapObject *)a)->state = 0xFF;
      }
    }
  }
  else
    if (((((MapObject *)a)->number & 0x3F) >= 7) && (((MapObject *)a)->state == 0xFF))
  {
    ((MapObject *)a)->state = 0x80;
  }
}
