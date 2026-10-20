/* Lunar SSSC — bg_cover: the bg's cover grid (which tiles bg_first_draw
   draws a second time over sprites). The first split had it in boot_init.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern u8 g_BgCoverGrid[];     /* far array (address-loaded via lui/%lo) */

/* Fill a 15 x 21 halfword table with 0x1000. */
/* func_80013EB8 */
void bg_cover_reset(void) {
    s16 *p;
    s16 v;
    s32 i;
    s32 j;

    p = (s16 *) g_BgCoverGrid;
    v = 0x1000;
    for (i = 0xE; i >= 0; i--) {
        for (j = 0x14; j >= 0; j--) {
            *p = v;
            p++;
        }
    }
}
extern s32 g_BgScrollXLowBits;
extern s32 g_BgScrollYLowBits;

/* Lower the overlay-depth grid under a screen rectangle.

   g_BgCoverGrid holds one s16 per 16x16 screen cell, 21 to a row
   (bg_cover_reset resets all 15 rows to 0x1000 each frame); bg_first_draw
   draws a tile's overlay copy at its cell's depth.  (x, y, w, h) is a
   sprite's box in screen pixels; g_BgScrollXLowBits / g_BgScrollYLowBits are the view's
   sub-tile scroll (bg_scroll_set), which turns it into cells.  Every cell
   from (x >> 4, y >> 4) to ((x + w) >> 4, (y + h) >> 4) is lowered to
   depth - 1, with depth held to 0x200..0x300; a cell keeps a smaller value
   it already has.  Both ends are clamped to 0..21 across and 0..14 down.
   A row is 21 cells, so column 21 is the next row's first cell (and on row
   14 the halfword after the table). */
/* func_80013EF0 */
void bg_cover_box_set(s32 x, s32 y, s32 w, s32 h, s32 depth) {
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;
    s32 i;
    s32 j;
    s16 *p;

    x += g_BgScrollXLowBits;
    y += g_BgScrollYLowBits;
    x0 = x >> 4;
    y0 = y >> 4;
    x += w;
    y += h;
    x1 = x >> 4;
    y1 = y >> 4;
    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0) {
        y0 = 0;
    }
    if (x0 > 0x14) {
        x0 = 0x15;
    }
    if (y0 > 0xD) {
        y0 = 0xE;
    }
    if (x1 < 0) {
        x1 = 0;
    }
    if (y1 < 0) {
        y1 = 0;
    }
    if (x1 > 0x14) {
        x1 = 0x15;
    }
    if (y1 > 0xD) {
        y1 = 0xE;
    }
    if (depth < 0x200) {
        depth = 0x200;
    }
    if (depth > 0x300) {
        depth = 0x300;
    }
    depth--;
    for (j = y0; j <= y1; j++) {
        p = &((s16 *) g_BgCoverGrid)[j * 21 + x0];
        for (i = x0; i <= x1; i++) {
            if (depth < *p) {
                *p = depth;
            }
            p++;
        }
    }
}
