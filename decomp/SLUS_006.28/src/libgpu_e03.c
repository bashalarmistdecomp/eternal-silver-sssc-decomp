/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libgpu/ext.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libgpu, member e03.o: SetDefDrawEnv. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetDefDrawEnv */
/* return_slot_fill: SetDefDrawEnv */

/* libgpu SetDefDrawEnv(env, x, y, w, h) (E03.OBJ; EXT.OBJ before 4.4): fills a
   drawing environment with the defaults for a w x h area at (x, y) and returns
   it. clip = {x, y, w, h}; tw = {0, 0, 0, 0}; the background colour 0; dtd = 1;
   dfe = 1 when the area is at most 256 lines high, or at most 288 when
   GetVideoMode() is non-zero, else 0; ofs = (x, y); tpage = 10,
   which is libgpu.h's getTPage(0, 0, 640, 0); isbg = 0. The video mode is read
   first, before anything is stored. */
/* func_8008E878 */
DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h)
{
    int video_mode = GetVideoMode();

    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->clip.h = h;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    env->dtd = 1;
    if (video_mode) {
        env->dfe = h <= 288;
    } else {
        env->dfe = h <= 256;
    }
    env->ofs[0] = x;
    env->ofs[1] = y;
    env->tpage = 10;
    env->isbg = 0;
    return env;
}

INCLUDE_PAD(12);
