/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_crwa.c), psyz (decomp/src/libspu/s_crwa.c) and
   psy-q-decomp (src/spu/s_crwa.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_crwa.o: SpuClearReverbWorkArea. The first
   split had it in psyq_misc_ah.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BE8 */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SpuClearReverbWorkArea */
/* return_slot_fill: SpuClearReverbWorkArea */
/* far_syms: D_800B7B38 D_800B7BCC D_800B7BD8 D_800B7BE8 */
extern s32 D_800B7B38;                                  /* _spu_EVdma */
extern s32 D_800B7BCC;                                  /* _spu_transMode */
extern s32 D_800B7BD8;                                  /* _spu_mem_mode_plus */
extern volatile SpuTransferCallbackProc D_800B7BE8;     /* _spu_transferCallback */
extern s32 D_800B7C20[];                                /* _spu_zerobuf: 0x400 zero bytes */
extern s32 D_800B8140[];                                /* _spu_rev_startaddr[10] */
s32 _SpuIsInAllocateArea_(u32);
int _spu_t(int mode, ...);

/* libspu SpuClearReverbWorkArea (S_CRWA.OBJ): fill the reverb work area of
   mode rev_mode with zeroes, by DMA, and wait for it.  Refuses with -1 when
   rev_mode is not 0..9 or the mode's work area overlaps a SpuMalloc block
   (_SpuIsInAllocateArea_ of _spu_rev_startaddr[rev_mode]).  The area runs
   from the mode's start address to the end of SPU RAM (mode 0, reverb off:
   the last 0x10 address units from 0xFFF0); both are scaled to bytes by
   _spu_mem_mode_plus.  For the duration _spu_transMode is forced from 1
   (I/O) to 0 (DMA) and the user's transfer callback is taken out of
   _spu_transferCallback, so each block's end is signalled by the DMA event
   instead; both are put back at the end.  The area is written in blocks of
   at most 0x400 bytes from _spu_zerobuf: _spu_t(2, address), _spu_t(1),
   _spu_t(3, _spu_zerobuf, size), then WaitEvent(_spu_EVdma).  Returns 0. */
/* func_800A0028 */
long SpuClearReverbWorkArea(long rev_mode)
{
    volatile SpuTransferCallbackProc callback;
    s32 oldTransmode;
    s32 dstAddr;
    s32 more;
    s32 transmodeCleared;
    u32 size;
    u32 length;

    callback = 0;
    transmodeCleared = 0;
    if (rev_mode < 0 || rev_mode >= 10 || _SpuIsInAllocateArea_(D_800B8140[rev_mode])) {
        return -1;
    }
    if (rev_mode == 0) {
        length = 0x10 << D_800B7BD8;
        dstAddr = 0xFFF0 << D_800B7BD8;
    } else {
        length = (0x10000 - D_800B8140[rev_mode]) << D_800B7BD8;
        dstAddr = D_800B8140[rev_mode] << D_800B7BD8;
    }
    oldTransmode = D_800B7BCC;
    if (D_800B7BCC == 1) {
        D_800B7BCC = 0;
        transmodeCleared = 1;
    }
    more = 1;
    if (D_800B7BE8 != 0) {
        callback = D_800B7BE8;
        D_800B7BE8 = 0;
    }
    while (more != 0) {
        if (length > 0x400) {
            size = 0x400;
        } else {
            size = length;
            more = 0;
        }
        _spu_t(2, dstAddr);
        _spu_t(1);
        _spu_t(3, D_800B7C20, size);
        WaitEvent(D_800B7B38);
        length -= 0x400;
        dstAddr += 0x400;
    }
    if (transmodeCleared != 0) {
        D_800B7BCC = oldTransmode;
    }
    if (callback != 0) {
        D_800B7BE8 = callback;
    }
    return 0;
}

INCLUDE_PAD(12);
