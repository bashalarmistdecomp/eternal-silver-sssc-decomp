/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libapi/counter.c), psyz (decomp/src/libapi/counter.c) and
   psy-q-decomp (src/api/counter.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libapi, member counter.o, part 1 of 2: SetRCnt, GetRCnt. The
   first split had it in psyq_cd_e.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
/* Sony's libapi.h declares SetRCnt(unsigned long, unsigned short, long). libgs's
   GsInitVcount, below, passes -1 as the target and the image loads it as a whole word
   (`li a1,-1`); under that prototype it would be 0xFFFF. So Sony's own libgs was compiled
   with no such prototype in scope. The header's declaration is renamed out of the way for
   this file, and SetRCnt is declared and defined below with the types its caller shows. */
#define SetRCnt SetRCnt_sdk_prototype
#include "psyq_types.h"
#include "psyq_private.h"
#undef SetRCnt
s32 SetRCnt(s32, s16, s32);

/* cc97_lib: SetRCnt */
/* far_syms: D_800B6104 (target reaches it %hi/%lo) */

/* One root counter's registers, 0x10 bytes each from 0x1F801100 (the layout and the names
   are sotn-decomp's libapi/counter.c `Counter`). */
extern volatile Counter *D_800B6104;    /* COUNTER.OBJ's `_counters` pointer (0x1F801100) */

/* libapi SetRCnt (COUNTER.OBJ): program root counter `spec & 0xFFFF`. For counter 3 and
   above nothing is written and 0 is returned. Otherwise the counter's mode word is
   cleared, its target is written, and a new mode word is built from 0x48:
     counters 0 and 1   RCntMdGATE in `flags` makes it 0x49; RCntMdSC clear adds 0x100
     counter 2          RCntMdSC clear makes it 0x248
     any counter        RCntMdINTR adds 0x10
   The mode word is then written and 1 returned. The mode flags go under the SDK's
   names (kernel.h). The image tests
   the counter number signed against 3 and unsigned against 2: `n > 2` and the cast
   below give those two instructions, and so do `n >= 3` and `n <= 1u`. */
/* func_8008F268 */
s32 SetRCnt(s32 spec, s16 target, s32 flags)
{
    s32 n = spec & 0xFFFF;
    s32 mode = 0x48;

    if (n > 2) {
        return 0;
    }
    D_800B6104[n].mode = 0;
    D_800B6104[n].target = target;
    if ((u32)n < 2) {
        if (flags & RCntMdGATE) {
            mode = 0x49;
        }
        if ((flags & RCntMdSC) == 0) {
            mode |= 0x100;
        }
    } else if (n == 2) {
        if ((flags & RCntMdSC) == 0) {
            mode = 0x248;
        }
    }
    if (flags & RCntMdINTR) {
        mode |= 0x10;
    }
    D_800B6104[n].mode = mode;
    return 1;
}
/* cc97_lib: GetRCnt */

/* libapi GetRCnt (COUNTER.OBJ): the current 16-bit count of root counter spec & 0xFFFF,
   or 0 when the index is 3 or more. */
/* func_8008F304 */
long GetRCnt(unsigned long spec)
{
    s32 i = spec & 0xFFFF;

    if (i >= 3) {
        return 0;
    }
    return D_800B6104[i].rootCounter;
}
