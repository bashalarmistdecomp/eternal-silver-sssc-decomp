/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_itc.c), psyz (decomp/src/libspu/s_itc.c) and
   psy-q-decomp (src/spu/s_itc.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_itc.o: SpuIsTransferCompleted. The first split
   had it in psyq_misc_aj.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BE4 (target reaches it %hi/%lo) */
/* far_syms: D_800B7BE4 (auto: target reaches it %hi/%lo — addressing row 3) */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s32 D_800B7BE4;
/* far_syms: D_800B7B40 (target reaches it %hi/%lo) */
/* far_syms: D_800B7B40 (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 D_800B7B40;
/* cc97_lib: SpuIsTransferCompleted */
/* return_slot_fill: SpuIsTransferCompleted */
/* far_syms: D_800B7B40 */
/* far_syms: D_800B7BE4 */
/* far_syms: D_800B7B38 */
extern s32 D_800B7B38;   /* _spu_EVdma: the SPU DMA-end event descriptor */

/* libspu SpuIsTransferCompleted (S_ITC.OBJ) */
/* func_800A0488 */
long SpuIsTransferCompleted(long flag) {
    long ev;

    if (D_800B7B40 == 1 || D_800B7BE4 == 1) {
        return 1;
    }
    ev = TestEvent(D_800B7B38);
    if (flag == 1) {
        if (ev == 0) {
            do {
            } while (TestEvent(D_800B7B38) == 0);
        }
        ev = 1;
        D_800B7BE4 = ev;
        return ev;
    }
    if (ev == 1) {
        D_800B7BE4 = ev;
    }
    return ev;
}

INCLUDE_PAD(12);
