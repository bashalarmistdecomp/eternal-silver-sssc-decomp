/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libspu/s_snc.c) and
   psy-q-decomp (src/spu/s_snc.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_snc.o: SpuSetNoiseClock. The first split had
   it in psyq_misc_af.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 */
/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's with the reverb registers named. */
extern SpuUnion *D_800B7BB0;
/* cc97_lib: SpuSetNoiseClock */
/* return_slot_fill: SpuSetNoiseClock */

/* libspu SpuSetNoiseClock (S_SNC.OBJ) */
/* func_8009F3E8 */
long SpuSetNoiseClock(long n_clock) {
    long n;

    if (n_clock < 0) {
        n = 0;
    } else if (n_clock > 0x3F) {
        n = 0x3F;
    } else {
        n = n_clock;
    }
    /* SPUCNT bits 8-13: the noise clock */
    D_800B7BB0->rxx.spucnt = (D_800B7BB0->rxx.spucnt & ~0x3F00) | ((n & 0x3F) << 8);
    return n;
}

INCLUDE_PAD(8);
