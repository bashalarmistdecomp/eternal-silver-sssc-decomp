/* PSY-Q 4.5 libetc, member vmode.o: SetVideoMode, GetVideoMode. The first
   split had it in psyq_spu_k.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s32 D_800B79B0;
/* cc97_lib: func_800954D4 */
/* return_slot_fill: func_800954D4 */

/* return_slot_fill: SetVideoMode */
/* func_80095728 */
long SetVideoMode(long mode) {
    s32 temp_v0;

    temp_v0 = D_800B79B0;
    D_800B79B0 = mode;
    return temp_v0;
}

/* far_syms: D_800B79B0 (target reaches it %hi/%lo) */

/* func_8009573C */
long GetVideoMode(void) {
    return D_800B79B0;
}

INCLUDE_PAD(12);
