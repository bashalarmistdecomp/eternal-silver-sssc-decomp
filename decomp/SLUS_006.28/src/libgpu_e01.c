/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libgpu/ext.c)
   and psyz (decomp/src/libgpu/ext.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libgpu, member e01.o: LoadClut. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */
u_short GetClut(int x, int y);  /* TU defines this below */

/* libgpu LoadClut(clut, x, y) (e01.o; EXT.OBJ before 4.4): load a 256-entry
   colour table. Builds the rectangle {x, y, 256, 1}, queues LoadImage(&rect, clut)
   and returns the CLUT id of (x, y), GetClut(x, y) ((y << 6) | ((x >> 4) & 0x3F)). */
/* func_8008E808 */
u_short LoadClut(u_long *clut, int x, int y)
{
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 256;
    rect.h = 1;
    LoadImage(&rect, clut);
    return GetClut(x, y);
}

INCLUDE_PAD(12);
