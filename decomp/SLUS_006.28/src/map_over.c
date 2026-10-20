/* Lunar SSSC — map_over: whether a box may go over the attribute map: the
   free check, OverCheck (the developers' name, and their word for it), a
   record's class, the first attribute under a box and the line between two
   spots. The first split had it in scripting_e.c. Every function is C.
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
extern u8 g_BgAttributeSlotBlocks[];
extern u8 secondary_tile_table[];

extern long HWD0;
extern long VWD0;
extern u8 D_80012CDC[];
s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground, MapBox *box, s32 layer);
void func_80069C64();
s32 debug_printf(s32 *, s32, s32, s32, s32, s32, s32, s32, s32);
void OverCheck(s32 x, s32 y, s32 arg2, void *box, s32 arg4);
s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag);
s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag);
extern s32 (*vtbl_4658_table[])();
s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground, MapBox *box, s32 layer);
void func_80069C64();
s32 debug_printf(s32 *, s32, s32, s32, s32, s32, s32, s32, s32);
void OverCheck(s32 x, s32 y, s32 arg2, void *box, s32 arg4);
extern long HWD0, VWD0;
void func_80069C64();
void MapPadDirectionsSet(s32 row);  /* extern */

s32 bg_attribute_block_load();

/* Is the pixel box (x - w .. x + w, y - h .. y + h) free on the secondary
   tile map's attribute plane (8-pixel cells, graphics.md "The attribute
   plane")? 1 = free, 0 = blocked. The field's footprint probe: every
   caller hands it a point plus an actor's box (MapSpotMoveTry,
   MapSpotSideMoveTry, MapAttributeClassGet and the map objects' walk in this TU;
   the battle segment's movers).

   flag non-zero answers 1 at once. The box is clamped to 0 .. 0x7FF on
   both axes (a negative low edge becomes 0, a high edge >= 0x800 becomes
   0x7FF) and covers the cells x0 >> 3 .. ((x1 + 7) >> 3) - 1 by
   y0 >> 3 .. ((y1 + 7) >> 3) - 1, so the high edges are exclusive. With
   no cell in it the answer is 1.

   Each cell is read as bg_attribute_block_load (bg_tile.c) leaves it:
   slot q = ((tx >> 4) & 3) | ((ty >> 2) & 0xC) of the block cache at
   scratchpad g_BgAttributeSlotBlocks must hold the cell's block id
   (tx >> 4) + (ty & 0xF0), else bg_attribute_block_load(tx, ty) is called
   to load the block; the word is then secondary_tile_table's
   [(tx & 0xF) + ((ty & 0xF) << 4) + (q << 8)]. A cell outside 0 .. 255 on
   either axis reads 0. Nothing is written here; no RNG, no frame wait.

   A cell counts when it is solid AND shaped:
     solid  bit 0x4000 -- or every cell when `layer` is 0 (the parameter
            is turned into the mask (layer == 0) << 14 on entry);
     shape  bits 0x3800, non-zero.
   Shape 0x3800 (a full block) blocks wherever it is. The other shapes
   are the four diagonals, and each is tolerated in ONE corner cell of the
   box, when the box's offset inside that cell clears the slope:
     0x0800  first row, first column   (x0 & 7) + (y0 & 7) >= 7
     0x1000  first row, last column    ((x1 - 1) & 7) < (y0 & 7)
     0x2000  last row, first column    (x0 & 7) >= ((y1 - 1) & 7)
     0x1800  last row, last column     ((x1 - 1) & 7) + ((y1 - 1) & 7) < 7
   The first row is tested before the last and the first column before
   the last, so a one-row box only ever applies the two first-row rules
   and a one-column box the two first-column ones. Anything else -- a
   shaped solid cell on an edge or inside, the wrong diagonal in a corner,
   a slope not cleared -- answers 0.

   Signature: the bytes read `layer` with lhu and `flag` with lbu from the
   caller's frame, so they are u16 and u8. The first split's battle.c and
   battle_b.c declared (s32, s32, u8, u8, s32, s32); their calls are unchanged by that.

   Shapes:
   - MARKED SPELLING: both tables are read as array MEMBERS of struct
     views cast over the TU's u8[] externs (BgAttributeSlotBlocks.key[q],
     BgAttributeSlots.cell[...]). A member's base goes through
     memory_address() before the index is evaluated, so each address is a
     compiler temporary (lui + lo_sum, two insns) that lives across the
     index computation; that life is what makes the loop pass move all
     four out of the inner loop ahead of x0 & 7 / y0 & 7, and the outer
     loop's four moves then leave no room to move those two again -- they
     stay in the inner pre-header, as the target has them -- while the
     addresses get no register and are rematerialised by reload ($t6).
     Without it: the siblings' *(T *)(sym + (i << 1)) spelling for both
     81/181, for either one alone 118 / 119, ((T *)sym)[i] 132, pointer
     locals set in the loop 137 (they keep registers), set before the
     loops 64. The objects are most likely structs in the original; the
     views stand in for declarations this TU does not have.
   - `solid` is its own u16, assigned BEFORE shape: tile then dies at
     shape's mask, which shares its register and fills the solid test's
     delay slot (shape first, the test on the expression: 174; `solid` as
     s32: 174).
   - The out-of-range arm first, and ONE `return 0` after the corner
     chain, each rule a `continue`.
   - `layer` itself is turned into the mask (lhu, andi, sltiu, sll in one
     register). */
/* func_800692D4 */
s32 MapAttributeFreeCheck(s32 x, s32 y, s32 w, s32 h, u16 layer, u8 flag)
{
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;
    s32 tx0;
    s32 cols;
    s32 rows;
    s32 i;
    s32 j;
    u32 tx;
    u32 ty;
    s32 q;
    u32 key;
    u16 tile;
    u16 shape;
    u16 solid;

    if (flag) {
        return 1;
    }
    layer = (layer == 0) << 14;
    x0 = x - w;
    if (x0 < 0) {
        x0 = 0;
    }
    y0 = y - h;
    if (y0 < 0) {
        y0 = 0;
    }
    x1 = x + w;
    if (x1 >= 0x800) {
        x1 = 0x7FF;
    }
    y1 = y + h;
    if (y1 >= 0x800) {
        y1 = 0x7FF;
    }
    ty = y0 >> 3;
    tx0 = x0 >> 3;
    cols = ((x1 + 7) >> 3) - tx0;
    rows = ((y1 + 7) >> 3) - ty;
    for (i = 0; i < rows; i++, ty++) {
        for (j = 0; j < cols; j++) {
            tx = tx0 + j;
            if (tx >= 0x100 || ty >= 0x100) {
                tile = 0;
            } else {
                key = (tx >> 4) + (ty & 0xF0);
                q = ((tx >> 4) & 3) | ((ty >> 2) & 0xC);
                if (key != ((BgAttributeSlotBlocks *)g_BgAttributeSlotBlocks)->blocks[q]) {
                    bg_attribute_block_load(tx, ty);
                }
                tile = ((BgAttributeSlots *)secondary_tile_table)->attributes[(tx & 0xF) + ((ty & 0xF) << 4) + (q << 8)];
            }
            solid = layer | (tile & 0x4000);
            shape = tile & 0x3800;
            if (!solid) {
                continue;
            }
            if (shape == 0) {
                continue;
            }
            if (shape != 0x3800) {
                if (i == 0) {
                    if (j == 0) {
                        if (shape == 0x800 && ((x0 & 7) + (y0 & 7)) >= 7) {
                            continue;
                        }
                    } else if (j == cols - 1) {
                        if (shape == 0x1000 && ((x1 - 1) & 7) - (y0 & 7) < 0) {
                            continue;
                        }
                    }
                } else if (i == rows - 1) {
                    if (j == 0) {
                        if (shape == 0x2000 && (x0 & 7) - ((y1 - 1) & 7) >= 0) {
                            continue;
                        }
                    } else if (j == cols - 1) {
                        if (shape == 0x1800 && (((x1 - 1) & 7) + ((y1 - 1) & 7)) < 7) {
                            continue;
                        }
                    }
                }
            }
            return 0;
        }
    }
    return 1;
}

s32 debug_printf(s32 *, s32, s32, s32, s32, s32, s32, s32, s32);

/* func_800695A8 */
void OverCheck(s32 x, s32 y, s32 arg2, void *box, s32 arg4)
{
  debug_printf((s32 *)D_80012CDC, x, y, arg2, (s32) (((MapBox *)box)->dx), (s32) (((MapBox *)box)->dy), (s32) (*(((u8 *) box) + 0x2)), (s32) (((MapBox *)box)->half_height), arg4);
}

/* far_syms: g_BgAttributeSlotBlocks secondary_tile_table */

/* Classify a script actor's footprint against the secondary tile map
   (8-pixel tiles); the caller ORs the answer into the actor's flags
   (bits 0x6000): 0x2000 or 0x4000.

   An actor off the ground (y != ground) answers 0x4000 at once. Otherwise
   the box is (x + dx, y + dy) +- (w, h). A tile with bit 0x8000 inside the
   box decides it: 0x2000 when `layer` is 0 or the tile also has 0x4000,
   else 0x4000. With none, the tile rows from (y - top) down to the box's
   top row, `rows` of them and widened by `rows` tiles on both sides, are
   scanned for bit 0x4000: with a `layer` and no such tile the answer is
   0x4000. Last, the box is walked up from `step` to (top + dy - h) pixels,
   `step` at a time: where MapAttributeFreeCheck answers 0
   (blocked) it tries x + 1 and then x - 1, shifting x by `step` towards
   the free side, and with both blocked the answer is 0x4000. Anything
   else is 0x2000.

   Both scans inline the tile lookup of bg_attribute_get
   (the 16x16-tile block cache at scratchpad 0x1F800020 in front of
   secondary_tile_table). `acc` only ever holds bit 0x4000, so its 0x8000
   test never fires; the original keeps it.

   Matching notes: the two inlined lookups are separate objects (their own
   x/y/quadrant/tile), the first reaching the cache through a pointer set
   before its loop and the table directly, the second the other way round;
   and the second scan's width is its own variable. Each of those is what
   the register allocation and the loop hoists of the target require. */
/* func_8006960C */
s32 MapAttributeClassGet(u16 step, u16 top, s32 x, s32 y, u16 ground, MapBox *box, s32 layer)
{
    s32 v;
    s32 dy;
    s32 w;
    s32 h;
    s32 x0;
    s32 y0;
    s32 rows;
    s32 cols;
    s32 i;
    s32 j;
    u32 tx;
    u32 ty;
    u32 tx2;
    u32 ty2;
    s32 q;
    s32 q2;
    u32 tile;
    u32 tile2;
    u16 acc;
    s32 row;
    s32 cols2;
    u32 key;
    s16 *cache;
    u8 *tbl;

    if (y != ground) {
        return 0x4000;
    }
    dy = box->dy;
    x += box->dx;
    w = box->half_width;
    y += dy;
    h = box->half_height;
    x0 = x - w;
    if (x0 < 0) {
        x0 = 0;
    }
    x0 >>= 3;
    y0 = y - h;
    if (y0 < 0) {
        y0 = 0;
    }
    y0 >>= 3;
    rows = ((y + h + 7) >> 3) - y0;
    cols = ((x + w + 7) >> 3) - x0;
    cache = (s16 *)g_BgAttributeSlotBlocks;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            tx = x0 + j;
            ty = y0 + i;
            if (tx >= 0x100 || ty >= 0x100) {
                tile = 0;
            } else {
                key = (tx >> 4) + (ty & 0xF0);
                q = ((tx >> 4) & 3) | ((ty >> 2) & 0xC);
                if (key != cache[q]) {
                    bg_attribute_block_load(tx, ty);
                }
                tile = *(u16 *)(secondary_tile_table + (((tx & 0xF) + ((ty & 0xF) << 4) + (q << 8)) << 1));
            }
            if (tile & 0x8000) {
                if (layer == 0 || (tile & 0x4000)) {
                    return 0x2000;
                }
                return 0x4000;
            }
        }
    }
    acc = 0;
    i = 0;
    v = (y - dy - top) >> 3;
    rows = y0 - v;
    if (rows > 0) {
        cols2 = cols + rows * 2;
        do {
            for (j = 0; j < cols2; j++) {
                tx2 = x0 - rows + j;
                ty2 = i + v;
                if (tx2 >= 0x100 || ty2 >= 0x100) {
                    tile2 = 0;
                } else {
                    key = (tx2 >> 4) + (ty2 & 0xF0);
                    q2 = ((tx2 >> 4) & 3) | ((ty2 >> 2) & 0xC);
                    if (key != *(s16 *)(g_BgAttributeSlotBlocks + (q2 << 1))) {
                        bg_attribute_block_load(tx2, ty2);
                    }
                    tbl = secondary_tile_table;
                    tile2 = *(u16 *)(tbl + (((tx2 & 0xF) + ((ty2 & 0xF) << 4) + (q2 << 8)) << 1));
                }
                acc |= tile2 & 0x4000;
            }
            i++;
        } while (i < rows);
    }
    if (acc & 0x8000) {
        return 0x2000;
    }
    if (layer != 0 && !(acc & 0x4000)) {
        return 0x4000;
    }
    for (i = step; i <= top + dy - h; i += step) {
        row = y - i;
        if (MapAttributeFreeCheck(x, row, w, h, (u16)layer, 0) == 0) {
            if (MapAttributeFreeCheck(x + 1, row, w, h, (u16)layer, 0) != 0) {
                x += step;
            } else if (MapAttributeFreeCheck(x - 1, row, w, h, (u16)layer, 0) != 0) {
                x -= step;
            } else {
                return 0x4000;
            }
        }
    }
    return 0x2000;
}
/* far_syms: g_MapPartyHpTakeOn D_800B8E0A g_MapPartyHpTakeAmount g_MapPadDirectionsState */
extern u8 g_MapPartyHpTakeOn;
extern u8 D_800B8E0A;
extern u8 g_MapPartyHpTakeAmount;
extern u8 g_MapPadDirectionsState;

/* The map-cell EVENT probe: scans the cells of the secondary tile map's
   attribute plane (8-pixel cells, graphics.md "The attribute plane") under
   the pixel box (x - w .. x + w, y - h .. y + h) and returns the first
   event word it finds, 0 when there is none. Two callers: MapAttributeRun
   (map_attribute.c), with the lead's position and probe box g_MapPartyBoxDx..2B,
   which dispatches the returned word (kinds 1..9: scene changes, map
   scripts, the colour mix); and the battle segment's map-object walker
   (map_look.c), with a row's position and its +0x50 box, which acts on
   kind 4 only.

   The box: a negative low edge becomes 0, and there is no clamp on the high
   side (MapAttributeFreeCheck clamps both to 0..0x7FF). It
   covers the cells x0 >> 3 .. ((x + w + 7) >> 3) - 1 by y0 >> 3 ..
   ((y + h + 7) >> 3) - 1, walked row by row, left to right.

   Each cell is read as bg_attribute_block_load (bg_tile.c) leaves it, the
   same lookup the 92D4 probe makes: slot q = ((tx >> 4) & 3) |
   ((ty >> 2) & 0xC) of the block cache at scratchpad g_BgAttributeSlotBlocks must hold
   the cell's block id (tx >> 4) + (ty & 0xF0), else
   bg_attribute_block_load(tx, ty) is called to load the block; the word is
   secondary_tile_table's [(tx & 0xF) + ((ty & 0xF) << 4) + (q << 8)]. A
   cell outside 0..255 on either axis reads 0.

   The event is the word's low 11 bits (the movement probes read bits
   0x3800 / 0x4000 / 0x8000 of the same word): id = word & 0x7FF, its kind
   id >> 7 (0..15) and its argument id & 0x7F. A cell whose argument is 0
   is skipped whatever its kind. Otherwise:
     kind 0..9   the scan stops and id is returned.
     kind 10     g_MapPartyHpTakeOn = 1 and g_MapPartyHpTakeAmount = the argument; the scan goes
                 on. The pad poll MapPadStep (map_step.c) reads the latch:
                 while it is set and the counter g_MapPartyHpTakeWait is 0 it calls
                 MapPartyHpTake (map_change.c), which takes g_MapPartyHpTakeAmount off
                 the first u16 of each party member's 0x40-byte record
                 (never below 1) and restarts the counter at 0x1E.
     kind 11     D_800B8E0A = 1 (the tree has no other reference to that
                 symbol), and two entries of the d-pad map g_MapPadDirections (the
                 nine u16 direction codes MapPadDirectionsSet fills) are patched:
                 argument 3 -> [3] (right) = 2 (up-right) and [7] (left) = 6
                 (down-left); argument 7 -> [3] = 4 (down-right) and [7] = 8
                 (up-left). Right and left then walk a diagonal; any other
                 argument patches nothing. The scan goes on.
     kind 12..15 nothing.
   Before the scan both latches are cleared (g_MapPartyHpTakeOn = D_800B8E0A = 0),
   and when g_MapPadDirectionsState is 0 -- no scrambled-controls box armed by script
   command 0x56 -- MapPadDirectionsSet(0) puts the d-pad map back to "as
   pressed". So the kind-10 latch and the kind-11 diagonal last only while
   the box covers such a cell, and a kind 0..9 cell earlier in the scan
   hides the ones after it. No RNG draw, no frame wait.

   Signature: w and h are used as plain ints (no mask before the subtract:
   `u8 w, u8 h` adds two andi 0xFF and scores 142/145). map_attribute.c and
   map_look.c declare it with four s32; the first split's battle.c and
   battle_b.c declared (s32, s32, u8, u8), and their calls, which load the two
   bytes with lbu, are the same bytes either way.

   Shapes:
   - MARKED SPELLING: both tables are read as array MEMBERS of the struct
     views MapAttributeFreeCheck uses (BgAttributeSlotBlocks.key[q],
     BgAttributeSlots.cell[...]), cast over the TU's u8[] externs. A member's
     base goes through memory_address() before the index is evaluated, so
     each address is a compiler temporary that lives across the index
     computation (life 3 and 9 insns in the loop dump, against 2 for the
     siblings' *(T *)(sym + (i << 1))). That life is what makes the loop
     pass move both addresses out of the inner loop, and every move takes 3
     off the pass's threshold: nine moves precede the direction table's own
     address here, which is then "not desirable" in the inner loop and, the
     four table-address insns being moved a second time, in the outer one.
     So the direction table's lui / addiu stay inside each arm ($v0), as
     the target has them, and the two hoisted table addresses get no
     register and are rematerialised by reload ($t0). With the cast
     spelling only five moves precede it, the direction table's address is
     hoisted into a call-saved register and the frame grows to 0x48:
     78/145; the key view alone 87, the cell view alone 81,
     ((T *)sym)[i] for both 87. The objects are most likely structs in the
     original; the views stand in for declarations this TU does not have.
   - The kind-11 arms store into g_MapPadDirections itself. With the cast reads the
     same bytes need a pointer local assigned in both arms AND one value
     local carrying both constants of an arm (a pointer local alone: 132,
     with either spelling of the reads); the views need neither.
   - The out-of-range arm first (the in-range arm first: 140);
     `kind = id >> 7` ahead of the `id & 0x7F` test, which holds it in its
     delay slot (inside the test: 124). `id` and
     `kind` are u16 -- the two andi 0xFFFF -- and `tile` is not (id as
     u32: 79, kind as u32: 132, tile as u16: 139). The argument is written
     `id & 0x7F` at each of its four uses (a local of its own: 79).
   - cols and rows get no register (all nine call-saved ones are taken by
     the two loops) and live at 0x10 / 0x14($sp). */
/* func_80069A20 */
s32 MapAttributeFind(s32 x, s32 y, s32 w, s32 h)
{
    s32 x0;
    s32 y0;
    s32 cols;
    s32 rows;
    s32 i;
    s32 j;
    u32 tx;
    u32 ty;
    s32 q;
    u32 key;
    u32 tile;
    u16 id;
    u16 kind;

    x0 = x - w;
    if (x0 < 0) {
        x0 = 0;
    }
    x0 >>= 3;
    y0 = y - h;
    if (y0 < 0) {
        y0 = 0;
    }
    y0 >>= 3;
    cols = ((x + w + 7) >> 3) - x0;
    rows = ((y + h + 7) >> 3) - y0;
    g_MapPartyHpTakeOn = 0;
    D_800B8E0A = 0;
    if (g_MapPadDirectionsState == 0) {
        MapPadDirectionsSet(0);
    }
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            tx = x0 + j;
            ty = y0 + i;
            if (tx >= 0x100 || ty >= 0x100) {
                tile = 0;
            } else {
                key = (tx >> 4) + (ty & 0xF0);
                q = ((tx >> 4) & 3) | ((ty >> 2) & 0xC);
                if (key != ((BgAttributeSlotBlocks *)g_BgAttributeSlotBlocks)->blocks[q]) {
                    bg_attribute_block_load(tx, ty);
                }
                tile = ((BgAttributeSlots *)secondary_tile_table)->attributes[(tx & 0xF) + ((ty & 0xF) << 4) + (q << 8)];
            }
            id = tile & 0x7FF;
            kind = id >> 7;
            if (id & 0x7F) {
                if (kind < 10) {
                    return id;
                }
                if (kind == 10) {
                    g_MapPartyHpTakeOn = 1;
                    g_MapPartyHpTakeAmount = id & 0x7F;
                } else if (kind == 11) {
                    D_800B8E0A = 1;
                    if ((id & 0x7F) == 3) {
                        g_MapPadDirections[3] = 2;
                        g_MapPadDirections[7] = 6;
                    } else if ((id & 0x7F) == 7) {
                        g_MapPadDirections[3] = 4;
                        g_MapPadDirections[7] = 8;
                    }
                }
            }
        }
    }
    return 0;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_80069C64() {

}

/* Line of sight on the secondary tile map's attribute plane: walks the
   straight line from `from` to `to` (pixel positions, u16 x / y) and
   answers 1 when nothing on it blocks, 0 at the first blocking tile.

   The line is cut into n = (max(|dx|, |dy|) + 3) >> 2 steps -- one sample
   every 4 pixels of the longer axis, rounded up -- in 16.16 fixed point:
   x and y start at from << 16 and each step adds (dx << 16) / n,
   (dy << 16) / n. The samples are the START of each step: sample 0 is
   `from` itself and `to` is never sampled. n == 0 -- the two points equal
   -- answers 1 at once.

   A sample's tile is (x >> 19, y >> 19) -- 8-pixel tiles -- read through
   the 16x16-tile block cache at scratchpad 0x1F800020 in front of
   secondary_tile_table (the lookup bg_attribute_get
   makes; MapAttributeClassGet inlines the same one); a tile outside 0..255 on
   either axis reads as 0.

   A tile BLOCKS when any of its bits 0x3800 is set -- but with `flag`
   non-zero only a tile that also has bit 0x4000 counts; with `flag` 0
   every tile with 0x3800 bits blocks.

   Callers (map_look.c, map_btl.c): MapObjectPartySeenCheck confirms an
   actor's contact with the lead's probe box (flag 1), MapObjectTouchFind a
   map object's touch (flag = the object row's +0x3C byte).

   Shape: |dx| and |dy| are written `d >= 0 ? d : -d`, which fold turns
   into ABS_EXPR -- one abssi2 insn (bgez / move in the slot / negu), so
   the whole entry is ONE basic block and the scheduler orders it
   (`d < 0 ? -d : d` is not folded and stays a real branch: 97/125). The
   larger one is picked with an if / else that adds the 3 in each arm into
   a temporary of its own (`adx >= ady ? adx : ady` becomes MAX_EXPR, a
   conditional move with one shared add: 103; `n` itself as the temporary:
   122; the arms the other way round: 123). The 16.16 start values are
   taken FIRST, before the deltas (assigned just before the loop: 105).
   `flag` is a u8 parameter (s32: 85): with all nine call-saved registers
   taken by the loop it lives in the frame (sb / lbu at 0x10($sp)). The two
   table addresses are hoisted by loop.c, get no register and are
   rematerialised by reload in $t0. */
/* func_80069C6C */
s32 MapAttributeLineCheck(MapSpot *from, MapSpot *to, u8 flag)
{
    s32 dx;
    s32 dy;
    s32 adx;
    s32 ady;
    s32 n;
    s32 v;
    s32 stepx;
    s32 stepy;
    s32 fx;
    s32 fy;
    s32 i;
    u32 tx;
    u32 ty;
    s32 q;
    u32 key;
    u32 tile;

    fx = AS(u16, from->x) << 16;
    fy = AS(u16, from->y) << 16;
    dx = AS(u16, to->x) - AS(u16, from->x);
    dy = AS(u16, to->y) - AS(u16, from->y);
    adx = dx >= 0 ? dx : -dx;
    ady = dy >= 0 ? dy : -dy;
    if (adx >= ady) {
        v = adx + 3;
    } else {
        v = ady + 3;
    }
    n = v >> 2;
    if (n == 0) {
        return 1;
    }
    stepx = (dx << 16) / n;
    stepy = (dy << 16) / n;
    for (i = 0; i < n; i++) {
        tx = fx >> 19;
        ty = fy >> 19;
        if (tx >= 0x100 || ty >= 0x100) {
            tile = 0;
        } else {
            key = (tx >> 4) + (ty & 0xF0);
            q = ((tx >> 4) & 3) | ((ty >> 2) & 0xC);
            if (key != *(s16 *)(g_BgAttributeSlotBlocks + (q << 1))) {
                bg_attribute_block_load(tx, ty);
            }
            tile = *(u16 *)(secondary_tile_table + (((tx & 0xF) + ((ty & 0xF) << 4) + (q << 8)) << 1));
        }
        if (((tile & 0x4000) || flag == 0) && (tile & 0x3800)) {
            return 0;
        }
        fx += stepx;
        fy += stepy;
    }
    return 1;
}
