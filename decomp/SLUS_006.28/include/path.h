/* path.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef PATH_H
#define PATH_H

#include "layout.h"

/* A point of a path: x then y, signed 16-bit pixels, as line_path_make and
   arc_path_make write them and the callers put a record's sprite entry on one
   point a frame (g_LinePaths' rows of 16, the shows' path buffers, a travel
   block's two paths); also the shows' fixed lists of such places (D_800B5ED4,
   BtlShow54Run's scatter). */
typedef struct {
    /* +0x00 x in pixels: line_path_make writes start + (end - start) * i / n;
       the callers copy it to sprite.x. */
    u16 x;
    /* +0x02 y in pixels; the callers copy it to sprite.y and make the depth
       from it (y + 1, y | 0x1000, the second path's y + 1). */
    u16 y;
} PathPoint;

LAYOUT_SIZE(PathPoint, 0x04);
LAYOUT_AT(PathPoint, x, x, 0x00);
LAYOUT_AT(PathPoint, y, y, 0x02);

#endif
