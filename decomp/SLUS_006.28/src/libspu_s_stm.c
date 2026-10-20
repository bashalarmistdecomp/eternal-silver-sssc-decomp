/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_stm.c), psyz (decomp/src/libspu/s_stm.c) and
   psy-q-decomp (src/spu/s_stm.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_stm.o: SpuSetTransferMode. The first split had
   it in psyq_misc_aj.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7B40 */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* far_syms: D_800B7B40 (target reaches it %hi/%lo) */
/* far_syms: D_800B7BCC (target reaches it %hi/%lo) */
/* far_syms: D_800B7B40 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800B7BCC (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 D_800B7B40;
extern s32 D_800B7BCC;

/* cc97_lib: SpuSetTransferMode */
/* return_slot_fill: SpuSetTransferMode */

/* libspu SpuSetTransferMode (S_STM.OBJ): remember the caller's mode in _spu_trans_mode
   (D_800B7B40) and set _spu_transMode (D_800B7BCC) to 1 for SPU_TRANSFER_BY_IO (1), else
   0 (DMA); returns the latter. */
/* func_800A0458 */
long SpuSetTransferMode(long mode)
{
    long temp;

    switch (mode) {
        case 0:
            temp = 0;
            break;
        case 1:
            temp = 1;
            break;
        default:
            temp = 0;
            break;
    }

    D_800B7B40 = mode;
    D_800B7BCC = temp;

    return temp;
}
