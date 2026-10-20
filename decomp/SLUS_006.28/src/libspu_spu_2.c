/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libspu/spu.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member spu.o, part 2 of 3: _spu_FiDMA. The first split
   had it in psyq_misc_x.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _spu_FiDMA */
/* far_syms: D_800B7BB0 D_800B7C00 D_800B7BE8 */
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* _spu_RXX -> 0x1F801C00 */
extern s32 D_800B7C00;                      /* SPU.OBJ .data+0x50, static: 1 = the DMA transfer reads */
extern volatile SpuTransferCallbackProc D_800B7BE8;    /* _spu_transferCallback */
void _spu_Fw1ts(void);

/* libspu _spu_FiDMA (SPU.OBJ): the SPU's DMA-end interrupt handler, installed
   by SpuStart.  After a write (the direction flag D_800B7C00 is 0) it first
   waits one _spu_Fw1ts.  It then takes SPUCNT's transfer mode (bits 4-5) back
   to "stop" and polls until the register reads back so, at most 0xF01 times.
   Last it reports the end of the transfer: the user's _spu_transferCallback
   (D_800B7BE8) if one is set, otherwise DeliverEvent(HwSPU 0xF0000009,
   EvSpCOMP 0x20), the event SpuIsTransferCompleted tests. */
/* func_8009E2B8 */
void _spu_FiDMA(void)
{
    u32 i;

    if (D_800B7C00 == 0) {
        _spu_Fw1ts();
    }
    D_800B7BB0->rxx.spucnt &= 0xFFCF;
    i = 0;
    while (D_800B7BB0->rxx.spucnt & 0x30) {
        if (++i > 0xF00) {
            break;
        }
    }
    if (D_800B7BE8) {
        D_800B7BE8();
        return;
    }
    DeliverEvent(0xF0000009, 0x20);
}
