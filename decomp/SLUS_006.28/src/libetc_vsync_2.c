/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libetc/vsync.c), psyz (decomp/src/libetc/vsync.c) and
   psy-q-decomp (src/etc/vsync.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libetc, member vsync.o, part 2 of 2: func_80094B20. The first
   split had it in psyq_spu_f.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */


#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_80094B20 */
/* reorder_fill: func_80094B20 */
/* far_syms: D_800B7970 (target reaches it %hi/%lo) */
extern volatile s32 D_800B7970;     /* Vcount: the VBlank count INTR_VB.OBJ's handler advances */
extern char D_80013794[];           /* "VSync: timeout\n" */
s32 puts();
s32 ChangeClearRCnt(s32, s32);

/* libetc v_wait (VSYNC.OBJ, static; VSync's wait): spin until the VBlank count Vcount
   (D_800B7970) reaches count. The spin is bounded by max << 15 turns of the loop, counted
   down in a volatile local; when that runs out it prints "VSync: timeout" and calls
   ChangeClearPAD(0) and ChangeClearRCnt(3, 0), after which the kernel's pad driver
   passes the VBlank interrupt on without completing it (libetc's handler, which
   advances Vcount, can see it again), and returns. */
void func_80094B20(s32 count, s32 max)
{
    volatile s32 timeout = max << 15;

    while (D_800B7970 < count) {
        if (--timeout == -1) {
            puts(D_80013794);
            ChangeClearPAD(0);
            ChangeClearRCnt(3, 0);
            return;
        }
    }
}
