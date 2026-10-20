/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libspu/s_w.c),
   psyz (decomp/src/libspu/s_w.c) and psy-q-decomp (src/spu/s_r.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_r.o: SpuWrite. The first split had it in
   psyq_misc_ah.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SpuClearReverbWorkArea */
/* return_slot_fill: SpuClearReverbWorkArea */
/* far_syms: D_800B7BE8 */
extern volatile SpuTransferCallbackProc D_800B7BE8;     
/* cc97_lib: SpuWrite */
/* return_slot_fill: SpuWrite */
/* far_syms: D_800B7BE4 D_800B7BE8 */
extern s32 D_800B7BE4;                                  /* _spu_inTransfer */
u32 _spu_Fw(u8 *, u32);

/* libspu SpuWrite (S_W.OBJ): transfer size bytes at addr to SPU RAM, at the
   address SpuSetTransferStartAddr set.  The size is capped at 0x7EFF0 (SPU
   RAM less its first 0x1010 bytes) and handed to _spu_Fw, which starts a DMA
   transfer or, in I/O transfer mode, writes the bytes itself.  When no
   transfer callback is set, _spu_inTransfer is cleared (0 = a transfer is
   running; SpuIsTransferCompleted sets it back to 1).  Returns the size
   actually taken. */
/* func_800A0398 */
unsigned long SpuWrite(unsigned char *addr, unsigned long size)
{
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }
    _spu_Fw(addr, size);
    if (D_800B7BE8 == 0) {
        D_800B7BE4 = 0;
    }
    return size;
}
INCLUDE_PAD(4);
