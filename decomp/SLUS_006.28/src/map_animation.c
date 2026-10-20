/* Lunar SSSC — map_animation: the map's animations: the party's and the
   objects' frames, a pose set, an animation set and started. The first
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

extern s32 g_MapPartyTwins;
extern s16 g_MapViewX;
extern s16 g_MapViewY;
extern s32 g_MapPartyRecords;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
void MapAnimationPoseSet(void *record, s16 pose, u16 facing);
void MapAnimationStart(void *record, s32 restart);
void MapAnimationPoseSet(void *, s16, u16);

extern s32 g_SpriteList;
extern u16 g_MapPartyBlinkCount;
void sprite_run_off(s32 first, s32 count);
void MapAnimationSet(void *record, u16 animation);  /* TU declares this below */

/* The field actors' per-frame animation tick and draw. Clears sprite slots
   0..5 and 0x46..0x4B, then for each of the six records: a live, un-held
   g_MapPartyRecords record advances its frame timer; at the frame's duration it
   steps to the next frame, and past the animation's last frame it either
   stays on that frame (animation entry bit 15) or restarts the animation
   (MapAnimationStart). The record's sprite image is copied to slot i with the
   current frame's tile. A live g_MapPartyTwins twin is put on the same
   animation (MapAnimationSet), synced to the same distance from the end and
   the same timer, and copied to slot 0x46 + i. While the blink counter
   g_MapPartyBlinkCount runs, every odd count hides both slots of each live record. */
/* func_80065CF8 */
void MapPartyAnimationStep(void)
{
    SpriteEntry *spr;
    u32 i;

    spr = (SpriteEntry *)g_SpriteList;
    sprite_run_off(0, 6);
    sprite_run_off(0x46, 6);
    for (i = 0; i < 6; i++)
    {
        if (((MapRecord *)g_MapPartyRecords)[i].on_kind.on != 0)
        {
            if (((MapRecord *)g_MapPartyRecords)[i].hold_definition.hold == 0)
            {
                MapRecord *p = &((MapRecord *)g_MapPartyRecords)[i];

                p->timer++;
                if (p->timer >= p->step_length)
                {
                    p->step++;
                    if (p->step >= p->end)
                    {
                        if (AS(SpriteAnimationRow *, p->animation_table)[p->animation].count & 0x8000)
                        {
                            p->step--;
                            p->timer = 0;
                            p->step_length = AS(SpriteStep *, p->step_list)[p->step].length;
                        }
                        else
                        {
                            MapAnimationStart(p, 1);
                        }
                    }
                    else
                    {
                        p->timer = 0;
                        p->step_length = AS(SpriteStep *, p->step_list)[p->step].length;
                    }
                }
            }
            spr[i] = ((MapRecord *)g_MapPartyRecords)[i].sprite;
            spr[i].pose = AS(SpriteStep *, ((MapRecord *)g_MapPartyRecords)[i].step_list)[((MapRecord *)g_MapPartyRecords)[i].step].pose;
        }
        if (((MapRecord *)g_MapPartyTwins)[i].on_kind.on != 0)
        {
            MapAnimationSet(&((MapRecord *)g_MapPartyTwins)[i], ((MapRecord *)g_MapPartyRecords)[i].animation);
            ((MapRecord *)g_MapPartyTwins)[i].step = ((MapRecord *)g_MapPartyTwins)[i].end - (((MapRecord *)g_MapPartyRecords)[i].end - ((MapRecord *)g_MapPartyRecords)[i].step);
            ((MapRecord *)g_MapPartyTwins)[i].timer = ((MapRecord *)g_MapPartyRecords)[i].timer;
            ((MapRecord *)g_MapPartyTwins)[i].step_length = ((MapRecord *)g_MapPartyRecords)[i].step_length;
            spr[i + 0x46] = ((MapRecord *)g_MapPartyTwins)[i].sprite;
            spr[i + 0x46].pose = AS(SpriteStep *, ((MapRecord *)g_MapPartyTwins)[i].step_list)[((MapRecord *)g_MapPartyTwins)[i].step].pose;
        }
    }
    if (g_MapPartyBlinkCount != 0)
    {
        g_MapPartyBlinkCount--;
        if (g_MapPartyBlinkCount & 1)
        {
            for (i = 0; i < 6; i++)
            {
                if (((MapRecord *)g_MapPartyRecords)[i].on_kind.on != 0)
                {
                    spr[i].on = 0;
                    spr[i + 0x46].on = 0;
                }
            }
        }
    }
}

extern u16 g_MapObjectsSpriteBits;

/* The map objects' per-frame animation tick and draw: MapPartyAnimationStep's
   counterpart for the 0x80 rows (0x60 bytes) of g_MapObjects and their twins
   (0x3C bytes) at g_MapObjectTwins. Clears sprite slots 0x10..0x2F and 0x56..0x65,
   then for each row that is live and not held by a script slot (+0 bit
   0x80):

   - unless its behaviour (+0x3D & 0x3F) is 3, or is 6 with (+0x43 & 0x7F)
     == 1 and +0x46 == 0, the frame timer advances exactly as in
     MapPartyAnimationStep: at the frame's duration the frame index steps; past the
     animation's end it either holds the last frame (animation entry bit 15)
     or restarts (MapAnimationStart(row, 1));
   - a row outside the camera's neighbourhood (draw y within g_MapViewY -
     100 .. + 0x144, x within g_MapViewX - 100 .. + 0x1A4) draws nothing;
   - otherwise its sprite image goes to the next slot from 0x10 up, with the
     current frame's tile and the attribute bits g_MapObjectsSpriteBits (0 or 0x8000)
     OR-ed in;
   - a live twin, for the first 0x10 of them, is put on the row's animation
     (MapAnimationSet), synced to the same distance from the end, timer and
     duration, moved to the row's x / draw y / ground y, and drawn into the
     next slot from 0x56 up the same way.

   The pass ends after 0x20 drawn rows.

   Matching notes: the rows and twins are walked by pointer (the second
   pointers the target keeps at row + 8 and twin + 0x2C are the loop
   optimiser's combined address givs, based on the last field accessed), the
   slots are indexed by the two counters; tile is stored before attr (the
   scheduler cannot lift the frame-table loads over a store through another
   pointer); m is cleared before n; the twin's slot is an array member 0x46
   slots up from the same base, which is what puts 0x9D8 in the addresses
   instead of in a register. */
/* func_80065FD8 */
void MapObjectsAnimationStep(void)
{
    MapRecord *q;
    MapObjectSprites *tab;
    MapObject *p;
    u32 m;
    u32 n;
    u32 i;

    q = (MapRecord *)g_MapObjectTwins;
    tab = (MapObjectSprites *)(g_SpriteList + 0x240);
    p = (MapObject *)g_MapObjects;
    sprite_run_off(0x10, 0x20);
    sprite_run_off(0x56, 0x10);
    m = 0;
    n = 0;
    for (i = 0; i < 0x80; i++, p++, q++)
    {
        /* the next free instance, taken before the row is tested: the row's
           image, its frame's tile and the attribute bits all go through it */
        SpriteEntry *out = &tab->object[n];

        if (p->record.on_kind.kind == 0 || (p->record.on_kind.kind & 0x80))
        {
            continue;
        }
        if ((p->number & 0x3F) != 3 && ((p->number & 0x3F) != 6 || (p->look & 0x7F) != 1 || p->length != 0))
        {
            if (++p->record.timer >= p->record.step_length)
            {
                if (++p->record.step >= p->record.end)
                {
                    if (AS(SpriteAnimationRow *, p->record.animation_table)[p->record.animation].count & 0x8000)
                    {
                        p->record.step--;
                        p->record.timer = 0;
                        p->record.step_length = AS(SpriteStep *, p->record.step_list)[p->record.step].length;
                    }
                    else
                    {
                        MapAnimationStart(p, 1);
                    }
                }
                else
                {
                    p->record.timer = 0;
                    p->record.step_length = AS(SpriteStep *, p->record.step_list)[p->record.step].length;
                }
            }
        }
        if (p->record.sprite.y < g_MapViewY - 100 || p->record.sprite.y > g_MapViewY + 0x144)
        {
            continue;
        }
        if (p->record.sprite.x < g_MapViewX - 100 || p->record.sprite.x > g_MapViewX + 0x1A4)
        {
            continue;
        }
        *out = p->record.sprite;
        out->pose = AS(SpriteStep *, p->record.step_list)[p->record.step].pose;
        out->flags |= g_MapObjectsSpriteBits;
        n++;
        if (q->on_kind.on != 0 && m < 0x10)
        {
            MapAnimationSet(q, p->record.animation);
            q->step = q->end - (p->record.end - p->record.step);
            q->timer = p->record.timer;
            q->step_length = p->record.step_length;
            q->sprite.x = p->record.sprite.x;
            q->sprite.y = p->record.sprite.y;
            q->sprite.depth = p->record.sprite.depth;
            tab->twin[m] = q->sprite;
            tab->twin[m].pose = AS(SpriteStep *, q->step_list)[q->step].pose;
            tab->twin[m].flags |= g_MapObjectsSpriteBits;
            m++;
        }
        if (n >= 0x20)
        {
            break;
        }
    }
}

void MapAnimationSet(void *record, u16 animation);

/* Pick an actor's animation from a pose group and a facing.
   Animation ids are 0 (none) or 4 * group + 1 + k, k = 0..3 being the
   facing column — MapAnimationSet, which stores the id at +0x30, uses the
   same grouping to decide whether the change restarts the animation.
   pose = 0 sets id 0; pose > 0 selects group pose - 1; pose < 0 keeps
   the group of the id the actor has now (+0x30). arg2 is a facing 1..8:
   5 -> column 0; 1, 2, 8 -> column 1; 6, 7 -> column 2; 3, 4 -> column 3;
   any other facing leaves the actor alone. */
/* func_8006633C */
void MapAnimationPoseSet(void *record, s16 pose, u16 facing) {
    u16 anim;

    anim = pose;
    if (pose == 0) {
        MapAnimationSet(record, anim);
        return;
    }
    if (pose < 0) {
        anim = ((((MapRecord *)record)->animation - 1) & 0xFFFD) | 1;
    } else {
        anim = ((pose - 1) * 4) | 1;
    }
    switch (facing) {
    case 1:
    case 2:
    case 8:
        MapAnimationSet(record, anim + 1);
        break;
    case 3:
    case 4:
        MapAnimationSet(record, anim + 3);
        break;
    case 5:
        MapAnimationSet(record, anim);
        break;
    case 6:
    case 7:
        MapAnimationSet(record, anim + 2);
        break;
    }
}
/* MapAnimationSet -- PUT A RECORD ON AN ANIMATION: the setter every animated record
   goes through -- the field actors and their twins (MapPartyAnimationStep), the map
   objects' twins (MapObjectsAnimationStep), MapAnimationPoseSet's pose-and-facing
   pick, StepWalkStep's kind 3, and the g_MenuRecords actors of menu_page.c,
   menu_run.c, menu_card.c and menu_list.c (38 call sites). arg0 is any record with the
   animation state at +0x28..+0x3B (MapRecord here; MapObject, MapRecord and the
   other TUs' actors share those offsets), animation the animation id.

   - animation equal to the id the record is on (+0x30): nothing happens.
   - Otherwise +0x32 = the old id, +0x30 = animation, and
     MapAnimationStart(rec, restart) re-seats the frame range.
   - restart is 0 when the old and the new id are both poses 1..60 of the
     same group of four -- ((id - 1) & 0xFFFC) + 1, the group's first id,
     is equal -- i.e. only the facing column changed (ids are 4 * group +
     1 + column, see MapAnimationPoseSet): MapAnimationStart then leaves the
     frame timer +0x34 and duration +0x36 alone. Any other change (id 0, an
     id above 60, another group) gives restart 1: timer 0 and the first
     frame's duration.

   Reads +0x30, writes +0x30 and +0x32; MapAnimationStart writes +0x38 / +0x3A
   (and +0x34 / +0x36 on a restart). No return value. No RNG draw and no
   frame wait (the one callee is a leaf).

   Source shapes the bytes need:
   - In the last test BOTH ids are read back from the record
     (rec->animation_before and rec->anim), not taken from the parameter. The target's
     `lhu $v1, 0x32($a0)` is the first read; its `andi $v0, $a1, 0xFFFF`
     is the second: a load of +0x30 in every RTL pass up to register
     allocation, which the post-reload pass (reload_cse_regs) turns into a
     zero-extension of $a1 because $a1 still holds the halfword just stored
     there. Written with animation the first pass of cse reuses the entry
     block's zero-extension instead (one register for all four uses, one
     instruction short): 22/31.
   - The group test is old == new with the `+ 1` on both sides (new == old
     26/31; without the `+ 1` 25/31).
   - The two calls are written out; cross-jumping merges them (a flag local
     set in an if / else and one call is the same bytes only when the chain
     is written as its || negation, 18/31 as &&).
   The two range tests of the new id match with rec->anim or with animation;
   the record is used for them as in the last test. The pointer local is
   for reading only (casts at each access are the same bytes). */
/* func_800663EC */
void MapAnimationSet(void *record, u16 animation)
{
    MapRecord *p = record;

    if (p->animation != animation) {
        p->animation_before = p->animation;
        p->animation = animation;
        if (p->animation_before >= 1 && p->animation_before <= 60 && p->animation >= 1 && p->animation <= 60
            && ((p->animation_before - 1) & 0xFFFC) + 1 == ((p->animation - 1) & 0xFFFC) + 1) {
            MapAnimationStart(p, 0);
        } else {
            MapAnimationStart(p, 1);
        }
    }
}

/* func_80066468 */
void MapAnimationStart(void *record, s32 restart) {
    u16 first_step;

    first_step = (*(s16 *)((u8 *)((((MapRecord *)record)->animation * 4) + ((MapRecord *)record)->animation_table) + 0x2));
    ((MapRecord *)record)->step = first_step;
    ((MapRecord *)record)->end = (s16) (first_step + ((*(s32 *)((((MapRecord *)record)->animation * 4) + ((MapRecord *)record)->animation_table)) & 0x7FFF));
    if (restart != 0) {
        ((MapRecord *)record)->timer = 0;
        ((MapRecord *)record)->step_length = (u16) (*(s16 *)((u8 *)((((MapRecord *)record)->step * 4) + ((MapRecord *)record)->step_list) + 0x2));
    }
}
