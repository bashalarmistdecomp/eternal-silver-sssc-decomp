/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_ini.c), psyz (decomp/src/libspu/s_ini.c) and
   psy-q-decomp (src/spu/s_ini.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_ini.o, part 2 of 2: SpuStart. The first split
   had it in psyq_misc_v.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SpuStart */
/* reorder_fill: SpuStart */
/* far_syms: D_800B7BA0 D_800B7B38 */
extern s32 D_800B7BA0;              /* _spu_isCalled */
extern s32 D_800B7B38;              /* _spu_EVdma */
s32 EnterCriticalSection(void);
void _SpuDataCallback(void (*)(void));
void _spu_FiDMA(void);

/* libspu SpuStart (S_INI.OBJ): hook the SPU's DMA interrupt, once.  If
   _spu_isCalled (D_800B7BA0) is still 0 it is set to 1 and, inside a critical
   section, _spu_FiDMA is installed with _SpuDataCallback
    and the SPU transfer-complete event is opened and enabled:
   OpenEvent(HwSPU 0xF0000009, EvSpCOMP 0x20, EvMdNOINTR 0x2000, no handler),
   its descriptor kept in _spu_EVdma (D_800B7B38) for SpuIsTransferCompleted.
   Caller: _SpuInit. */
/* func_8009DDE0 */
void SpuStart(void)
{
    if (D_800B7BA0 == 0) {
        D_800B7BA0 = 1;
        EnterCriticalSection();
        _SpuDataCallback(_spu_FiDMA);
        D_800B7B38 = OpenEvent(0xF0000009, 0x20, 0x2000, 0);
        EnableEvent(D_800B7B38);
        ExitCriticalSection();
    }
}

INCLUDE_PAD(8);
