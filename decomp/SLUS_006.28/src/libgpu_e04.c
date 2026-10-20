/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libgpu/ext.c)
   and psyz (decomp/src/libgpu/ext.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libgpu, member e04.o: SetDefDispEnv. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetDefDispEnv */

/* libgpu SetDefDispEnv(env, x, y, w, h) (E04.OBJ; EXT.OBJ before 4.4): fills a
   display environment with the defaults for a w x h area at (x, y) and returns
   it. disp = {x, y, w, h}; screen = {0, 0, 0, 0}; isrgb24, isinter, pad1 and
   pad0 = 0, stored in that order. */
/* func_8008E938 */
DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h)
{
    env->disp.x = x;
    env->disp.y = y;
    env->disp.w = w;
    env->disp.h = h;
    env->screen.x = 0;
    env->screen.y = 0;
    env->screen.w = 0;
    env->screen.h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}

INCLUDE_PAD(4);
