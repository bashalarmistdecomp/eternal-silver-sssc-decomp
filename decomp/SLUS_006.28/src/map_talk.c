/* Lunar SSSC — map_talk: the party's box against the map's objects (free
   check, overlap, touch) and the talk: the object the party faces found,
   and what an object of kind 7 does. The first split had it in
   scripting_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"
extern s32 g_MapProbeBoxX;
extern s32 g_MapProbeBoxY;
extern s32 g_MapProbeBoxHalfWidth;
extern s32 g_MapProbeBoxHalfHeight;
s32 StoryBitCheck(s32);

extern u16 g_MapPartyRingHead[1];
extern long HWD0;
extern long VWD0;
extern s32 g_ScriptCommandPointer;
extern u16 g_MapNumber;
extern s32 g_MapTalkObjectDefinition;
extern s32 g_MapPartyRecords;
extern s32 g_MapObjects;
extern s32 g_MapPartyRingEntries;
s32 StoryBitSet(u16);
extern s32 g_MapObjectDefinitions;
extern s8 g_MapPartyBoxDx;
extern s8 g_MapPartyBoxDy;
extern u8 g_MapPartyBoxHalfWidth;
extern u8 g_MapPartyBoxHalfHeight;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
extern s32 g_MapObjects;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapAnimationPoseSet(void *, s16, u16);
void ScriptLabelRun(s32);
s32 MapPartyRingEntryGet(s32, s32 *);
s32 MapPartyObjectsFreeCheck(s16, s16);
extern u16 g_MapPartyRingHead[1];
extern u16 g_MapPartyRingHead[1];
void MapAnimationPoseSet(void *, s16, u16);
void MapAnimationPoseSet(void *, s16, u16);
extern s32 (*vtbl_4658_table[])();
void ScriptLabelRun(s32);

/* MapPartyObjectsFreeCheck -- "IS THE LEAD'S NEXT PLACE CLEAR
   OF MAP OBJECTS": would the lead party actor, put at (spot_x, spot_y), overlap
   a live map object? One caller, the lead's walk step MapPartyWalkStep (this
   file), with the copy of party record 0's x / ground y that
   MapSpotMoveTry has just stepped past the tile test (its u16
   locals; the s16 parameters sign-extend them). A non-zero answer lets the
   step stand (MapPartyRingPush pushes the new place on the trail); on 0 the
   caller pushes nothing and stores only the direction, in the byte +5 of
   the trail's head entry.

   The probe rectangle MapBoxTouchGet tests against is published first, and
   left published: the LEAD's box (the four bytes at g_MapPartyBoxDx, set by
   MapPartyBoxSet) at the candidate place -- centre g_MapProbeBoxX = x +
   g_MapPartyBoxDx (dx), g_MapProbeBoxY = y + g_MapPartyBoxDy (dy), half sizes
   g_MapProbeBoxHalfWidth = g_MapPartyBoxHalfWidth (w), g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfHeight (h). Then the 0x80
   rows (0x60 bytes) of g_MapObjects in order:

     0   the first row that is live (+0 != 0) and whose own rectangle --
         centre x + box.dx, ground y + box.dy, the row's box at +0x50 --
         STRICTLY overlaps the probe on both axes (MapBoxTouchGet's answer 9;
         an edge or corner touch, 1..8, does not block)
     1   no such row: the place is clear

   This is MapObjectOverlapFind (the next function, a map object's own move test)
   with the roles the other way round: the probe is the lead's box, not a
   row's; each row's centre uses the row's OWN box.dx / box.dy (with the
   lead's offsets, as MapObjectOverlapFind has them: 50/52); there is no test
   against the lead and no row left out; and the answer is the opposite
   sense, 1 = clear, with no row number. No test of the row's kind: a row
   that is not thought for (7) or bound to a script slot (bit 0x80) blocks
   like any other, and a row whose box.w is 0 never blocks (MapBoxTouchGet
   answers 0 for it). Reads nothing else, writes only the four probe
   globals. No RNG draw, no frame wait (MapBoxTouchGet is a leaf).

   Source shapes the bytes need: the parameters are s16 / s16 (as s32:
   44/52) and the counter a u16 (s32: 34/52) -- the target keeps the raw
   counter in $s0 and masks it at the row scaling and at the loop test;
   the row IS a pointer local, written product-first, i * 0x60 +
   g_MapObjects (index or base-first form: 51/52); every sum is position
   first (the call's the other way 48, `g_MapPartyBoxDx + spot_x` 49); the four
   stores are written x, y, w, h -- the target's sw order x, w, y, h is the
   scheduler's (written w before y: 48). `return 0` inside a `for` and one
   `return 1` after it: the target's two `addiu $s0, 1` (the delay slot of
   the answer's `bne`, and the dead row's exit) both come from the one
   `i++`. MapBoxTouchGet is called through the implicit int declaration, as
   MapObjectOverlapFind calls it. */
/* func_800679F4 */
s32 MapPartyObjectsFreeCheck(s16 spot_x, s16 spot_y)
{
    MapObject *p;
    u16 i;

    g_MapProbeBoxX = spot_x + g_MapPartyBoxDx;
    g_MapProbeBoxY = spot_y + g_MapPartyBoxDy;
    g_MapProbeBoxHalfWidth = g_MapPartyBoxHalfWidth;
    g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfHeight;
    for (i = 0; i < 0x80; i++) {
        p = (MapObject *)(i * 0x60 + g_MapObjects);
        if (p->record.on_kind.on != 0 && (MapBoxTouchGet(p->record.sprite.x + p->box.dx, p->record.sprite.depth + p->box.dy, &p->box) & 0xFFFF) == 9) {
            return 0;
        }
    }
    return 1;
}

/* la_absolute: MapObjectOverlapFind */
/* load_hazard_nop: MapObjectOverlapFind */

/* MapObjectOverlapFind -- "IS SOMEONE STANDING THERE": would map object
   `row_number` (a row of g_MapObjects), put at (spot_x, spot_y), overlap the lead party
   actor or another live map object? Seven callers, all of them a map
   object's movement: MapObjectWanderStep (the wander) and StepWalkStep in this
   file, five more in map_look.c (MapObjectRandomSpotPlace, MapObjectTowardWalk,
   MapObjectAwayWalk, MapObjectFacingWalk, MapObjectOpenFacingFind); each
   passes a candidate x / ground y and its own row index and takes a
   non-zero answer as "occupied".

   The probe rectangle MapBoxTouchGet tests against is published first, from
   the object's own box (+0x50): centre g_MapProbeBoxX = x + box.dx,
   g_MapProbeBoxY = y + box.dy, half sizes g_MapProbeBoxHalfWidth = box.w, g_MapProbeBoxHalfHeight =
   box.h. MapBoxTouchGet(cx, cy, &box2) answers 9 when the rectangle centred
   (cx, cy) with box2's half sizes STRICTLY overlaps the probe on both axes
   (1..8 for an edge or corner touch, 0 when apart or when either width is
   0). Only 9 counts here.

     1          the lead: record 0 of the party actors (g_MapPartyRecords), centre
                x + g_MapPartyBoxDx, ground y + g_MapPartyBoxDy, the lead's box
                (the four bytes at g_MapPartyBoxDx, set by MapPartyBoxSet)
     i + 2      map object i, the first of the 0x80 rows in order that is
                live (+0 != 0), is not row `row_number`, and overlaps
     0          nobody

   For the other objects the centre is the row's x / ground y plus the
   LEAD's offsets g_MapPartyBoxDx / g_MapPartyBoxDy, not the row's own box.dx /
   box.dy -- only the box handed to MapBoxTouchGet is the row's (+0x50).
   (Written with the row's own offsets it is 76/78; MapPartyTalkCheck, the talk
   check, does use the row's own.) No test of the row's kind: 4..6 and 7 are
   obstacles like any other. No RNG draw, no frame wait.

   Source shapes the bytes need: the parameters are s16 / s16 / u16 (the
   raw $a2 is kept in $s1 and masked at both uses) and the counter a u16;
   the entry reads go through ((MapObject *)g_MapObjects)[row_number] at each of the
   four uses, with no row pointer local (a local puts the idx * 0x60 chain
   ahead of spot_x's `sll`, 77/78); the loop's row IS a pointer local, written
   product-first (index or base-first form: 77); each sum is position first
   (73 the other way) and x is stored before y (68); `row_number != i`, not
   `i != row_number` (77); the answer is narrowed, `(u16)(i + 2)`, so the add is
   done on the raw counter (`(i + 2) & 0xFFFF` keeps the counter's extension
   in a saved register, 62). The lead's row is read through MapObject as
   MapPartyTalkCheck reads it through its 0x60 type: +8 is loaded signed here.
   Two markers: la_absolute, the target's same-register `lui` / `addiu` of
   g_MapPartyBoxDx is the assembler's `la` of a small-data symbol whose bytes
   this function also loads gp-relative; load_hazard_nop, the target has
   the load-delay `nop` between `lbu $v0, 0x53($v1)` and the gp store of
   g_MapProbeBoxHalfHeight that maspsx omits for an extern (74 and 75 without).
   All seven callers zero-extend the answer, which is what a u16 return type
   gives; the definition keeps the s32 the other files declare it with
   (`u16` with `return i + 2;` is the same bytes). */
/* func_80067AC4 */
s32 MapObjectOverlapFind(s16 spot_x, s16 spot_y, u16 row_number)
{
    MapObject *p;
    u16 i;

    g_MapProbeBoxX = spot_x + ((MapObject *)g_MapObjects)[row_number].box.dx;
    g_MapProbeBoxY = spot_y + ((MapObject *)g_MapObjects)[row_number].box.dy;
    g_MapProbeBoxHalfWidth = ((MapObject *)g_MapObjects)[row_number].box.half_width;
    g_MapProbeBoxHalfHeight = ((MapObject *)g_MapObjects)[row_number].box.half_height;
    if ((MapBoxTouchGet(((MapObject *)g_MapPartyRecords)->record.sprite.x + g_MapPartyBoxDx, ((MapObject *)g_MapPartyRecords)->record.sprite.depth + g_MapPartyBoxDy, (MapBox *)&g_MapPartyBoxDx) & 0xFFFF) == 9) {
        return 1;
    }
    for (i = 0; i < 0x80; i++) {
        p = (MapObject *)(i * 0x60 + g_MapObjects);
        if (p->record.on_kind.kind != 0 && row_number != i && (MapBoxTouchGet(p->record.sprite.x + g_MapPartyBoxDx, p->record.sprite.depth + g_MapPartyBoxDy, &p->box) & 0xFFFF) == 9) {
            return (u16)(i + 2);
        }
    }
    return 0;
}
/* MapBoxTouchGet -- THE BOX-AGAINST-PROBE TEST of the field: where does
   the rectangle centred (box_x, box_y) with the half sizes of `box` (bytes +2 w, +3 h; its
   dx / dy bytes are NOT read here -- every caller has already added them to
   the centre it passes) lie against the published probe rectangle, centre
   g_MapProbeBoxX / g_MapProbeBoxY, half sizes g_MapProbeBoxHalfWidth / g_MapProbeBoxHalfHeight? A leaf: it
   reads those four globals and the two bytes, writes nothing, calls
   nothing (no RNG draw, no frame wait).

     adx = |box_x - g_MapProbeBoxX|      W = box->w + g_MapProbeBoxHalfWidth
     ady = |box_y - g_MapProbeBoxY|      H = box->h + g_MapProbeBoxHalfHeight

     0   the probe's half width g_MapProbeBoxHalfWidth is 0, or box->w is 0 (only the
         two WIDTHS are tested for 0, the heights are not); or the two are
         apart on either axis: adx > W or ady > H
     9   they overlap: adx < W and ady < H, both strict
     otherwise they touch exactly, and the answer is the side of the PROBE
     the box (box_x, box_y) is on, numbered like the eight facings:
         8 1 2        1 = above   (adx < W, ady == H, box_y <= probe y)
         7 . 3        5 = below   (adx < W, ady == H, box_y >  probe y)
         6 5 4        3 = right / 7 = left   (adx == W, ady < H)
                      2, 4, 6, 8 = the corners (adx == W and ady == H)
     "right" is g_MapProbeBoxX < box_x and "below" is g_MapProbeBoxY < box_y; the
     other side of each takes the equal case.

   Eleven call sites (MapPartyObjectsFreeCheck, MapObjectOverlapFind x2, MapPartyTalkCheck in this
   file; MapObjectPartySeenCheck x5, MapObjectTouchFind in map_btl.c, MapObjectTouchApproach in
   map_touch.c) and every one masks the answer to 16 bits; MapObjectOverlapFind wants 9
   (blocked), MapPartyTalkCheck the odd sides against the lead's facing, the
   touch scan and MapObjectTouchApproach only zero / non-zero.

   Shape: each |d| is a difference in its own variable and then
   `d >= 0 ? d : -d` into a second one, which fold turns into ABS_EXPR --
   one `abssi2` insn (bgez / move / negu, difference in $v0 / $v1, result in
   $t0 / $a3), no basic-block boundary, so the scheduler is free to lift the
   g_MapProbeBoxY and box->h loads above the first abs as the target has them.
   One variable per axis (`dx = dx >= 0 ? dx : -dx`) is 62/72, `if (dx < 0)
   dx = -dx` a real branch (62), the difference written three times inside
   the ?: 60, both differences before both abs 61; W before H (70 the other
   way) and both after the abs pair (46 before). The parameters are plain
   ints: the callers' values are compared whole. The return type stays the
   int this file's earlier callers bind implicitly (map_touch.c declares it u16,
   which is what the callers' masks suggest the original had). */
/* func_80067BFC */
s32 MapBoxTouchGet(s32 box_x, s32 box_y, MapBox *box)
{
    s32 dx;
    s32 dy;
    s32 adx;
    s32 ady;
    s32 w;
    s32 h;

    if (g_MapProbeBoxHalfWidth == 0 || box->half_width == 0) {
        return 0;
    }
    dx = box_x - g_MapProbeBoxX;
    adx = dx >= 0 ? dx : -dx;
    dy = box_y - g_MapProbeBoxY;
    ady = dy >= 0 ? dy : -dy;
    w = box->half_width + g_MapProbeBoxHalfWidth;
    h = box->half_height + g_MapProbeBoxHalfHeight;
    if (w < adx || h < ady) {
        return 0;
    }
    if (adx < w && ady < h) {
        return 9;
    }
    if (adx == w) {
        if (ady == h) {
            if (g_MapProbeBoxX < box_x) {
                if (g_MapProbeBoxY < box_y) {
                    return 4;
                }
                return 2;
            }
            if (g_MapProbeBoxY < box_y) {
                return 6;
            }
            return 8;
        }
        if (g_MapProbeBoxX < box_x) {
            return 3;
        }
        return 7;
    }
    if (g_MapProbeBoxY < box_y) {
        return 5;
    }
    return 1;
}

/* load_hazard_nop: MapPartyTalkCheck */
void MapObjectKind7Run(s32 idx, s32 ev);  /* TU defines this below */

/* "PartyTalkCheck1" is what the caller prints before this call: the field's
   talk / examine check. MapPartyStep calls it on the frame pad bit 0x40 is pressed
   (g_MapPadPressedButtons == 0x40). It looks for the map object the lead actor is
   touching AND facing, and runs that object's event.

   The lead's facing `dir` (1..8) is byte +5 of the current entry of the
   8-byte position ring (g_MapPartyRingEntries[g_MapPartyRingHead]). The probe rectangle for
   MapBoxTouchGet is published in four globals: centre g_MapProbeBoxX / g_MapProbeBoxY
   = record 0 of the party actors' x / ground y plus the s8 offsets
   g_MapPartyBoxDx / g_MapPartyBoxDy, half sizes g_MapProbeBoxHalfWidth / g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfWidth /
   g_MapPartyBoxHalfHeight (MapPartyBoxSet sets those four bytes).

   The 0x80 map objects (0x60 rows of g_MapObjects) are scanned in order and
   the scan stops at the first hit. A row is skipped when it is free (+0 ==
   0) or its +0 is 4, 5 or 6. MapBoxTouchGet(x + box.dx, y + box.dy, &box)
   answers on which side the row's box (+0x50) touches the probe, and the
   row is a hit only when the lead faces that side:
       3 -> dir 2, 3, 4        7 -> dir 6, 7, 8
       5 -> dir 4, 5, 6        1 -> dir 0, 1, 2 or 8
   (0, the even corner codes and 9 = overlapping never hit).

   The hit row's event is byte +0xB of its 0x26-byte definition
   (g_MapObjectDefinitions[row +1]). A row whose +0 is 7 goes to MapObjectKind7Run(index,
   event), below. Any other row with a non-zero event is turned to
   face the lead -- MapAnimationPoseSet(row, -1, ((dir + 3) & 7) + 1),
   the opposite direction -- g_MapTalkObjectDefinition = the row's +1 byte, and the event
   runs through ScriptLabelRun. If g_MapTalkObjectDefinition is not 0xFF afterwards the row
   gets its own facing back (MapAnimationPoseSet(row, -1, +0x3E)) and
   g_MapTalkObjectDefinition = 0xFF.

   Shape: `i` is reused for the event -- a variable with several sets has no
   function-wide known bits, which is what keeps the `andi 0xFFFF` at
   ScriptLabelRun's call (a separate local: the mask is dropped, or 138/154
   as a u16). `dir` is a u16: the two range tests fold to int arithmetic on
   the raw byte and the single compares use the masked copy loop.c hoists.
   The loop pointer and the post-loop pointer are separate variables (the
   target has them in $a1 and $a0). i = 0 comes before found = -1. */
/* func_80067D1C */
void MapPartyTalkCheck(void)
{
    s32 i;
    s32 found;
    u16 dir;
    MapObject *p;
    MapObject *q;

    dir = *((u8 *)(g_MapPartyRingHead[0] * 8 + g_MapPartyRingEntries) + 5);
    g_MapProbeBoxX = ((MapObject *)g_MapPartyRecords)->record.sprite.x + g_MapPartyBoxDx;
    g_MapProbeBoxY = ((MapObject *)g_MapPartyRecords)->record.sprite.depth + g_MapPartyBoxDy;
    g_MapProbeBoxHalfWidth = g_MapPartyBoxHalfWidth;
    g_MapProbeBoxHalfHeight = g_MapPartyBoxHalfHeight;
    for (i = 0, found = -1; i < 0x80; i++) {
        if (found >= 0) {
            break;
        }
        p = (MapObject *)(i * 0x60 + g_MapObjects);
        if (p->record.on_kind.kind == 0) {
            continue;
        }
        if (p->record.on_kind.kind >= 4 && p->record.on_kind.kind < 7) {
            continue;
        }
        switch (MapBoxTouchGet(p->record.sprite.x + p->box.dx, p->record.sprite.depth + p->box.dy, &p->box) & 0xFFFF) {
        case 3:
            if (dir >= 2 && dir < 5) {
                found = i;
            }
            break;
        case 7:
            if (dir >= 6) {
                found = i;
            }
            break;
        case 5:
            if (dir >= 4 && dir < 7) {
                found = i;
            }
            break;
        case 1:
            if (dir < 3 || dir == 8) {
                found = i;
            }
            break;
        }
    }
    if (found >= 0) {
        q = (MapObject *)(found * 0x60 + g_MapObjects);
        i = ((MapObjectDefinition *)g_MapObjectDefinitions)[q->record.hold_definition.definition].event.label;
        if (q->record.on_kind.kind == 7) {
            MapObjectKind7Run(found, i);
        } else if (i != 0) {
            MapAnimationPoseSet(q, -1, ((dir + 3) & 7) + 1);
            g_MapTalkObjectDefinition = ((MapObject *)(found * 0x60 + g_MapObjects))->record.hold_definition.definition;
            ScriptLabelRun((u16)i);
            if (g_MapTalkObjectDefinition != 0xFF) {
                MapAnimationPoseSet((MapObject *)(g_MapObjects + found * 0x60), -1,
                                       ((MapObject *)(g_MapObjects + found * 0x60))->facing);
                g_MapTalkObjectDefinition = 0xFF;
            }
        }
    }
}

/* far_syms: g_MapObjectKind7Message g_MapObjectKind7Amounts g_ChrItemNoRoomState g_ChrItemNoRoomItem */
typedef struct {
    u8 b[8];
} Msg8;
extern long HWD0, VWD0;		 
extern u8 g_MapObjectKind7Message[];
extern u16 g_MapObjectKind7Amounts[];
extern s32 g_ChrItemNoRoomState;
extern s32 g_ChrItemNoRoomItem;
s32 SoundPlay(s32, s32);
s32 StoryBitSet(u16);
s32 StoryBitClear(s32);
void MessageDisp();
void ScriptCommandMoneyAdd(void);
void ScriptCommandChrItemGive(void);

/* func_80067F84 */
void MapObjectKind7Run(s32 idx, s32 ev)
{
    Msg8 msg;
    MapObjectKind7Command cmd;
    s32 info[2];
    u8 low;
    u8 state;
    s32 v;

    msg = *(Msg8 *)g_MapObjectKind7Message;
    state = *(u8 *)(g_MapObjects + idx * 0x60 + 0x3E);
    if (state < 5) {
        return;
    }
    if (state == 7 && StoryBitCheck(0x267) == 0) {
        *(u16 *)&msg = 0xF64B;
        MessageDisp(&msg, 1);
        return;
    }
    MapPartyRingEntryGet(0, info);
    MapAnimationPoseSet((void *)g_MapPartyRecords, 0xA, ((u8 *)info)[5]);
    low = ev < 0xF0;
    if (low) {
        *(u8 *)(g_MapObjects + idx * 0x60 + 0x3E) -= 4;
        SoundPlay(0xB, 2);
        MapAnimationPoseSet((void *)(g_MapObjects + idx * 0x60), -1,
                               *(u8 *)(g_MapObjects + idx * 0x60 + 0x3E));
    }
    if (ev == 0) {
        *(u16 *)&msg = 0xF64C;
        MessageDisp(&msg, 1);
    } else {
        g_ScriptCommandPointer = (s32)&cmd;
        if (!low) {
            StoryBitSet(0x2D6);
            ScriptLabelRun((u16)(ev + 0x230));
            StoryBitClear(0x2D6);
        } else {
            if (ev >= 0xB7) {
                v = g_MapObjectKind7Amounts[ev - 0xB7];
                g_ChrItemNoRoomState = 0;
                cmd.word = 0x1C;
                cmd.notice_item.notice = 0;
                cmd.amount = v;
                ScriptCommandMoneyAdd();
            } else {
                StoryBitSet(0x2D6);
                g_ChrItemNoRoomState = 0;
                cmd.word = 0x1F;
                cmd.notice_item.notice = ev;
                cmd.item_notice = 0;
                g_ChrItemNoRoomItem = ev;
                ScriptCommandChrItemGive();
                StoryBitClear(0x2D6);
            }
            if (g_ChrItemNoRoomState == 2) {
                *(u16 *)&msg = 0xF66B;
                MessageDisp(&msg, 1);
                *(u8 *)(g_MapObjects + idx * 0x60 + 0x3E) += 4;
                SoundPlay(0xB, 2);
                MapAnimationPoseSet((void *)(g_MapObjects + idx * 0x60), -1,
                                       *(u8 *)(g_MapObjects + idx * 0x60 + 0x3E));
            } else {
                StoryBitSet(*(u8 *)(g_MapObjects + idx * 0x60 + 1) + 0x2620 +
                              ((g_MapNumber > 100 ? g_MapNumber - 0x35 : g_MapNumber - 0x15) << 5));
            }
        }
    }
    MapAnimationPoseSet((void *)g_MapPartyRecords, 1, ((u8 *)info)[5]);
}
