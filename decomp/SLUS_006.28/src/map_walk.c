/* Lunar SSSC — map_walk: the party's walk: a step along a facing, a spot's
   moves tried round what stops it, and the pad's directions. The first
   split had it in scripting_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"
extern u16 g_MapPadDirections[9];
extern u8 g_MapFacingDxs[];
extern u8 g_MapFacingDys[];

extern u16 g_MapPartyRingHead[1];
extern s16 g_MapPartyAttributeCheckOff;
extern s32 g_MapPartyRecords;
extern s32 g_MapPartyRingEntries;
s32 MapPartyRingFollow();

/* far_syms: g_MapPartyBoxDx_blk */
extern s8 g_MapPartyBoxDx_blk[4];
s32 profile_mark_add(s32);
s32 MapPartyRingPush(u16, u16, s32, s32);
s32 MapPartyObjectsFreeCheck(s16, s16);
s32 MapSpotMoveTry();
extern u16 g_MapPartySideStepFacing;

/* MapSpotMoveTry on a copy of record 0's x/y (g_MapPartyRecords) in
   direction id. A result equal to id clears g_MapPartySideStepFacing; a result whose
   opposite ((d+3)&7)+1 equals the remembered g_MapPartySideStepFacing becomes 0;
   otherwise it is remembered. The x/y reads go through the struct so they
   are MEM_IN_STRUCT and may be scheduled above the sh to the stack copies. */
/* func_800664D4 */
s32 MapPartyWalkStep(s32 facing)
{
    s32 pad[2];     /* MARKED SPELLING (unused local): the target frame has 8 bytes at
                       0x20-0x27 nothing touches -- the original's local(s),
                       declaration unknown */
    u16 x;
    u16 y;
    u16 dir;
    u16 id;

    id = facing;
    x = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.x);
    y = AS(u16, ((MapRecord *)g_MapPartyRecords)->sprite.depth);
    dir = MapSpotMoveTry(id, 1, &x, &y, g_MapPartyBoxDx_blk, 0, (u8)g_MapPartyAttributeCheckOff);
    if (dir == id)
    {
        g_MapPartySideStepFacing = 0;
    }
    else if (g_MapPartySideStepFacing != 0 && g_MapPartySideStepFacing == ((dir + 3) & 7) + 1)
    {
        dir = 0;
    }
    else
    {
        g_MapPartySideStepFacing = dir;
    }
    if (dir == 0)
    {
        *((u8 *)(g_MapPartyRingHead[0] * 8 + g_MapPartyRingEntries) + 5) = facing;
    }
    else if (MapPartyObjectsFreeCheck(x, y) != 0)
    {
        MapPartyRingPush(x, y, 0, dir & 0xFF);
    }
    else
    {
        *((u8 *)(g_MapPartyRingHead[0] * 8 + g_MapPartyRingEntries) + 5) = dir;
    }
    profile_mark_add(0x21FF0000);
    MapPartyRingFollow();
    profile_mark_add(0x2100FFFF);
    return dir;
}

void MapSpotMove(int facing, int dist, u16 *spot_x, u16 *spot_y);  /* TU defines this below */
s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag);
s32 MapSpotSideMoveTry(u16 dir, u16 mode, s16 *px, s16 *py, s32 dx, s32 dy, u16 w, u16 h, u8 layer);  /* TU defines this below */

/* MapSpotMoveTry -- the name is the splitter's: this is
   the field's "step `dist` along facing `dir` if the tiles allow, else
   slide" primitive (callers: the mover tick MoveChrWalkStep, the map objects'
   MapObjectsThink / MapPartyWalkStep, and the battle segment's
   MapObjectAwayWalk, MapObjectTowardWalk, MapObjectTouchApproach).

   dir    facing 1..8, clockwise from up (MapSpotMove's tables
          g_MapFacingDxs / g_MapFacingDys: dx 0 1 1 1 0 -1 -1 -1, dy -1 -1 0 1 1 1
          0 -1 for dir - 1); 0 returns 0 at once, nothing touched.
   dist   pixels per step, handed to MapSpotMove unchanged.
   px,py  the point, read once and written back only when a step is taken.
   box    the footprint {s8 dx, dy; u8 w, h}: every probe is
          MapAttributeFreeCheck(x + dx, y + dy, w, h, layer,
          flag) -- non-zero = free.
   layer  that call's fifth argument; flag its sixth (0 from the map
          objects, g_MoveChrAttributeCheckOn == 0 from the mover tick).

   Three candidates are projected from the same start: dir, dir + 1 and
   dir - 1 (MapSpotMove masks its index, so 9 and 0 wrap). In that order,
   the first free one is taken:
     dir      -> (*px, *py) = it, returns dir;
     dir + 1  -> returns (dir & 7) + 1, i.e. that facing in 1..8;
     dir - 1  -> returns ((dir - 2) & 7) + 1.
   With all three blocked the obstacle side-step MapSpotSideMoveTry(dir,
   dist, px, py, dx, dy, w, h, layer) decides and its answer is returned
   (it moves the point itself). The result is the facing actually moved
   along, as a u16.

   Shape: an old-style definition with narrow parameters -- the entry keeps
   the raw $a0 in $s4, `dir` itself (the HImode copy, $s3) is the result
   variable and is re-assigned in each arm, and (dir + 1) / (dir & 7) are
   computed from the raw register. flag is the one value with no register
   left (nine callee-saved are taken), so its narrowed byte gets a spill
   slot (0x34) -- which is why the six probe coordinates must be SIX s16
   LOCALS, not a 3 x {x, y} array: a 12-byte BLKmode object is rounded to
   16 and pushes that slot to 0x38 (frame 0x68, 112/139). The chained
   assignments store the innermost first (x2, x1, x0). */
/* func_8006661C */
s32 MapSpotMoveTry(dir, dist, px, py, box, layer, flag)
u16 dir;
u16 dist;
s16 *px;
s16 *py;
MapBox *box;
u8 layer;
u8 flag;
{
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;

    if (dir == 0) {
        return 0;
    }
    x0 = x1 = x2 = *px;
    y0 = y1 = y2 = *py;
    MapSpotMove(dir, dist, (u16 *)&x0, (u16 *)&y0);
    MapSpotMove((u16)(dir + 1), dist, (u16 *)&x1, (u16 *)&y1);
    MapSpotMove((u16)(dir - 1), dist, (u16 *)&x2, (u16 *)&y2);
    if (MapAttributeFreeCheck(x0 + box->dx, y0 + box->dy, box->half_width, box->half_height, layer, flag)) {
        *px = x0;
        *py = y0;
    } else if (MapAttributeFreeCheck(x1 + box->dx, y1 + box->dy, box->half_width, box->half_height, layer, flag)) {
        *px = x1;
        *py = y1;
        dir = (dir & 7) + 1;
    } else if (MapAttributeFreeCheck(x2 + box->dx, y2 + box->dy, box->half_width, box->half_height, layer, flag)) {
        *px = x2;
        *py = y2;
        dir = ((dir - 2) & 7) + 1;
    } else {
        dir = MapSpotSideMoveTry(dir, dist, px, py, box->dx, box->dy, box->half_width, box->half_height, layer);
    }
    return dir;
}

/* far_syms: g_MapFacingDxs (target reaches it %hi/%lo) */
/* far_syms: g_MapFacingDys (target reaches it %hi/%lo) */

/* func_80066848 */
void MapSpotMove(int facing, int dist, u16 *spot_x, u16 *spot_y)
{
    int idx = facing - 1;
    s8 *p1 = (s8 *)g_MapFacingDxs;
    int dx = (dist & 0xFFFF) * p1[idx & 7];
    int v2 = (dist & 0xFFFF) * *(s8 *)&g_MapFacingDys[idx & 7];

    *spot_x += (u16)dx;
    *spot_y += (u16)v2;
}

s32 MapSpotSideAlignCount();
s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag);

/* func_800668A8 */
s32 MapSpotSideMoveTry(u16 dir, u16 mode, s16 *px, s16 *py, s32 dx, s32 dy, u16 w, u16 h, u8 layer)
{
    s16 ax;     /* probe A position (walks along dir + 2) */
    s16 ay;
    s16 tx;     /* look-ahead scratch for either probe */
    s16 ty;
    s16 bx;     /* probe B position (walks along dir - 2) */
    s16 by;
    s32 cntA;
    s32 cntB;
    s32 stepA;
    s32 stepB;
    s32 cdA;
    s32 cdB;

    stepB = 1;
    cntA = 0;
    cntB = 0;
    ax = bx = *px + dx;
    ay = by = *py + dy;
    stepA = 1;
    if (mode < 4) {
        cdA = MapSpotSideAlignCount(dir, ax, ay, w, h, 1);
        if (cdA == 0) {
            stepA = 4;
        }
    } else {
        cdA = 0;
        stepA = mode;
    }
    if (mode < 4) {
        cdB = MapSpotSideAlignCount(dir, bx, by, w, h, 0);
        if (cdB == 0) {
            stepB = 4;
        }
    } else {
        cdA = 0;
        stepB = mode;
    }
    while (cntA < 0x41 || cntB < 0x41) {
        if (cntB >= cntA) {
            if (cntA < 0x41) {
                cntA += stepA;
                MapSpotMove((u16)(dir + 2), stepA, (u16 *)&ax, (u16 *)&ay);
                if (ax - w < 0 || ax + w >= 0x800 || ay - h < 0 || ay + h >= 0x800) {
                    cntA = 0x41;
                }
                if (cdA != 0) {
                    if (--cdA == 0) {
                        stepA = 4;
                    }
                }
            }
            if (MapAttributeFreeCheck(ax, ay, w, h, layer, 0)) {
                tx = ax;
                ty = ay;
                MapSpotMove(dir, mode, (u16 *)&tx, (u16 *)&ty);
                if (MapAttributeFreeCheck(tx, ty, w, h, layer, 0)) {
                    cntB = 0x41;
                    dir = ((dir + 1) & 7) + 1;
                    break;
                }
                tx = ax;
                ty = ay;
                MapSpotMove((u16)(dir + 1), mode, (u16 *)&tx, (u16 *)&ty);
                if (MapAttributeFreeCheck(tx, ty, w, h, layer, 0)) {
                    cntB = 0x41;
                    dir = ((dir + 1) & 7) + 1;
                    break;
                }
            } else if (stepA >= 4) {
                if (mode < 4) {
                    cntA -= stepA;
                    MapSpotMove((u16)(dir - 2), stepA, (u16 *)&ax, (u16 *)&ay);
                    stepA = 1;
                } else {
                    cntA = 0x41;
                }
            } else {
                cntA = 0x41;
            }
        }
        if (cntB < cntA) {
            if (cntB < 0x41) {
                cntB += stepB;
                MapSpotMove((u16)(dir - 2), stepB, (u16 *)&bx, (u16 *)&by);
                if (bx - w < 0 || bx + w >= 0x800 || by - h < 0 || by + h >= 0x800) {
                    cntB = 0x41;
                }
                if (cdB != 0) {
                    if (--cdB == 0) {
                        stepB = 4;
                    }
                }
            }
            if (MapAttributeFreeCheck(bx, by, w, h, layer, 0)) {
                tx = bx;
                ty = by;
                MapSpotMove(dir, mode, (u16 *)&tx, (u16 *)&ty);
                if (MapAttributeFreeCheck(tx, ty, w, h, layer, 0)) {
                    cntA = 0x41;
                    dir = ((dir - 3) & 7) + 1;
                    break;
                }
                tx = bx;
                ty = by;
                MapSpotMove((u16)(dir - 1), mode, (u16 *)&tx, (u16 *)&ty);
                if (MapAttributeFreeCheck(tx, ty, w, h, layer, 0)) {
                    cntA = 0x41;
                    dir = ((dir - 3) & 7) + 1;
                    break;
                }
            } else if (stepB >= 4) {
                if (mode < 4) {
                    cntB -= stepB;
                    MapSpotMove((u16)(dir + 2), stepB, (u16 *)&bx, (u16 *)&by);
                    stepB = 1;
                } else {
                    cntB = 0x41;
                }
            } else {
                cntB = 0x41;
            }
        }
    }
    if (cntA < 0x41 || cntB < 0x41) {
        MapSpotMove(dir, mode, (u16 *)px, (u16 *)py);
        return dir;
    }
    return 0;
}
/* MapSpotSideAlignCount(dir, x, y, w, h, flag) -- HOW MANY SINGLE PIXELS A
   SIDE-STEPPING PROBE WALKS BEFORE ITS BOX IS ON THE 4-PIXEL GRID. Pure
   arithmetic on its arguments: no global read or written, no callee, no
   RNG draw, no frame wait.

   One caller, MapSpotSideMoveTry (above), twice, before its search loop and
   only when its step `mode` is under 4:
     cdA = MapSpotSideAlignCount(dir, ax, ay, w, h, 1);   probe A, walks dir + 2
     cdB = MapSpotSideAlignCount(dir, bx, by, w, h, 0);   probe B, walks dir - 2
   dir    the blocked facing, 1..8 clockwise from up (MapSpotMove's
          numbering); only its low 16 bits are looked at.
   x, y   the probe's start: the mover's point plus the footprint's
          (dx, dy).
   w, h   the footprint's size, used as half-extents about (x, y):
          MapSpotSideMoveTry itself tests x - w, x + w, y - h and y + h
          against 0 and 0x800. h is cut to 16 bits, w is used as passed.
   flag   which probe: non-zero = A (a quarter turn clockwise of dir),
          0 = B (a quarter turn anticlockwise).

   The result is k in 0..3: the number of 1-pixel steps after which the
   box's TRAILING edge, on the axis the probe moves sideways along, is a
   multiple of 4:
     moving +x   k = (w - x) & 3   (left edge  x - w)
     moving +y   k = (h - y) & 3   (top edge   y - h)
     moving -x   k = (x + w) & 3   (right edge x + w)
     moving -y   k = (y + h) & 3   (bottom edge y + h)
   written as `4 - ((x - w) & 3)` / `x + w`, with one `& 3` at the exit.
   The axis by (flag, dir):
     flag != 0:  1, 8 -> +x    2, 3 -> +y    4, 5 -> -x    6, 7 -> -y
     flag == 0:  1, 2 -> -x    3, 4 -> -y    5, 6 -> +x    7, 8 -> +y
   For a straight facing that is the walk's own axis. For a diagonal facing
   the walk (dir +- 2) is a diagonal too and shares one component with
   dir; the axis taken is the OTHER one, the component the quarter turn
   reverses (dir 8 = up-left, probe A walks up-right: +x).

   MapSpotSideMoveTry steps the probe one pixel at a time while this count
   runs down (`if (--cdA == 0) stepA = 4;`) and four at a time after it; a
   count of 0 starts at four.

   dir outside 1..8 takes no arm of either switch and `r` is never set:
   the function then returns whatever $v1 held on entry, & 3. The only
   call path is MapSpotMoveTry -> MapSpotSideMoveTry, and the
   handler returns before it for dir 0.

   Source shapes the bytes need (scores are out of tree, where 36/40 is
   everything but the two tables' `lui` / `addiu`):
   - MARKED SPELLING: `h &= 0xFFFF;` on an int parameter, ahead of the flag
     test. The target reads the fifth argument ONCE, zero-extended, at
     entry (`lhu $t0, 0x10($sp)`) and the sum uses that register as a
     plain second operand: `addu $v1, $a2, $t0`. Declared `u16 h`
     (old-style or prototype form, the same score) the load is the same
     `lhu`, but the parameter is then a halfword register extended at each
     use, and combine folds that extension into the sum and puts it FIRST
     whatever the source order (its dump: `(plus (subreg:SI (reg:HI h))
     (reg:SI y))`): `addu $v1, $t0, $a2`, 35/40. What the bytes say is
     that h was an int-mode value extended once at the top; they do not
     say how that was spelled. `h = (u16)h;`, the same mask on a `u32 h`,
     or a `u16 h` parameter copied to an int local (`s32 hh = h;`) all
     score the same 36. A `u16` local copy is 35, the mask written at each use is 27
     (`lw`, and an `andi` in the arms), and with this spelling `h + y` is
     35: the source order is `y + h`.
   - dir is masked AT EACH SWITCH, not once. The `andi $v0, $a0, 0xFFFF` is
     in the delay slot of the flag branch, in the register the flag was
     just tested in, so it is the first instruction of both arms.
     `dir &= 0xFFFF;` at the top is 23/40. An old-style `u16 dir`
     parameter used bare in both switches scores the same 36 as the two
     masks; the int form is kept because it needs no old-style definition
     and agrees with the declaration above MapSpotSideMoveTry.
   - No `default` and no initial value for r: `r = 0;` ahead of the test
     is 30/40.
   - Each arm is written out whole. The compiler shares them itself: the
     first switch keeps only its `case 1: case 8:` arm as code of its own
     (a second copy of the second switch's `case 5: case 6:`), and its
     other three arms are the second switch's. */
/* func_80066E64 */
s32 MapSpotSideAlignCount(s32 dir, s32 x, s32 y, s32 w, s32 h, s32 flag)
{
    s32 r;

    h &= 0xFFFF;
    if (flag != 0) {
        switch (dir & 0xFFFF) {
        case 1:
        case 8:
            r = 4 - ((x - w) & 3);
            break;
        case 2:
        case 3:
            r = 4 - ((y - h) & 3);
            break;
        case 4:
        case 5:
            r = x + w;
            break;
        case 6:
        case 7:
            r = y + h;
            break;
        }
    } else {
        switch (dir & 0xFFFF) {
        case 1:
        case 2:
            r = x + w;
            break;
        case 3:
        case 4:
            r = y + h;
            break;
        case 5:
        case 6:
            r = 4 - ((x - w) & 3);
            break;
        case 7:
        case 8:
            r = 4 - ((y - h) & 3);
            break;
        }
    }
    return r & 3;
}

extern u8 g_MapPadDirectionsRows[6][9];

/* MapPadDirectionsSet(n) -- SETS THE FIELD'S D-PAD MAP. Row n of the six 9-byte
   rows at g_MapPadDirectionsRows (n >= 5 is taken as 5) is copied, each byte widened
   to a halfword, into the nine u16 direction codes at g_MapPadDirections.

   g_MapPadDirections[k] is what the field's pad poll MapPadStep (map_step.c)
   returns for d-pad index k -- 0 nothing held, then clockwise from up:
   1 up, 2 up-right, 3 right, 4 down-right, 5 down, 6 down-left, 7 left,
   8 up-left (pad bits 0x1000 / 0x2000 / 0x4000 / 0x8000) -- and the
   returned code is the direction the lead is asked to walk
   (MapPartyWalkStep), in the same numbering. The table is .bss, so every
   direction reads 0 until the first call. Three places in the image form
   its address: the pad poll (a read), this function (the whole table) and
   MapAttributeFind (matching C, below), which patches
   two entries after this function has run: where a scanned map cell's
   event has kind 0xB (bits 7 up) it sets D_800B8E0A = 1 and stores [3]
   and [7] (right, left) = 2 and 6 when the event's low 7 bits are 3,
   = 4 and 8 when they are 7 -- right and left then walk a diagonal.

   The rows (image bytes at 0x800B5160; 54 bytes, then two of padding
   before g_MapTblNumbers):
     0:  0 1 2 3 4 5 6 7 8   as pressed
     1:  0 7 8 1 2 3 4 5 6   a quarter turn anticlockwise (up walks left)
     2:  0 7 6 5 4 3 2 1 8   mirrored, up <-> left and right <-> down
     3:  0 3 2 1 8 7 6 5 4   mirrored, up <-> right and down <-> left
     4:  0 3 4 5 6 7 8 1 2   a quarter turn clockwise (up walks right)
     5:  0 0 0 0 0 0 0 0 0   the pad moves nobody
   Callers, from the image: with 0 -- MapChange and GameEnter
   (map_change.c, entering the field), MapPadStep (map_step.c, when the
   lead is outside the box g_MapPadDirectionsState guards) and
   MapAttributeFind (on entry, when g_MapPadDirectionsState is 0,
   before its cell scan); with (rng_draw() & 3) + 1, one
   RNG draw picking a row 1..4 -- ScriptCommandMapPadDirectionsChange (script command 0x56,
   lns_intp.c), which then sets g_MapPadDirectionsState = 1 and the box
   g_MapPadDirectionsBoxLeft..06 around the lead. No caller passes 5 or more, so nothing in the
   image selects row 5 through this function. No callee, no RNG draw and
   no frame wait of its own; nothing returned.

   Source shapes the bytes need:
   - MARKED SPELLING: `rows`, a pointer local that holds the table's
     address before the row is formed. The target loads the address first
     (`lui` / `addiu $v0`), then forms n * 9 in `$v1` and adds the address
     to it. With the table named in the row expression itself
     (`src = g_MapPadDirectionsRows[row];`) the product comes first and the address
     after it: 7/20, the walking pointer in `$a1`, the counter in `$a0`.
     With one pointer for both (`src = g_MapPadDirectionsRows[0]; src += row * 9;`)
     the address goes to `$v1` and the product to `$v0`: 16/20. A local
     set where it is declared, ahead of the clamp, is 7/20. The bytes say
     the address was in a variable of its own ahead of the product; they
     do not say what that variable was called or typed (`u8 *tbl` with
     `src = tbl + row * 9` over a flat `u8 []` is the same 20 words).
   - The source is walked through a pointer and the destination indexed
     by the counter. An indexed source is not strength-reduced (an `addu`
     and the `lbu` every pass, 7-12/20). The destination as a second
     pointer set in the `for`'s first clause is the same bytes; set as a
     statement before the `for` it is 14/20 (the 0 of the counter then
     comes after its `lui` / `addiu`).
   - `row >= 5` and `row > 4` are the same bytes; a `do { } while
     (i < 9)` is the same bytes as the `for`.
   g_MapPadDirections is `u16 [9]` here (nine halfword stores; the next symbol is
   0x18 on); map_step.c still declares it `u8 []` and reads it through a
   cast. */
/* func_80066F04 */
void MapPadDirectionsSet(s32 row)
{
    u8 (*rows)[9];
    u8 *src;
    s32 i;

    if (row >= 5) {
        row = 5;
    }
    rows = g_MapPadDirectionsRows;
    src = rows[row];
    for (i = 0; i < 9; i++) {
        g_MapPadDirections[i] = *src++;
    }
}
