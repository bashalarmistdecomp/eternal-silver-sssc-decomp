/* SPDX-License-Identifier: MIT
   Parts of this file match code of psy-q-decomp (src/etc/intr_vb.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libgpu, member sys.o, part 6 of 6: _GPU_ResetCallback,
   func_8008E790. The first split had it in psyq_gpu_g.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _GPU_ResetCallback */
/* gnu_as_loads: _GPU_ResetCallback */
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern int func_8008DC4C(void);
extern Callback DMACallback(s32 dma, Callback f);

/* libgpu _GPU_ResetCallback (SYS.OBJ): makes the command queue's runner the DMA
   channel 2 (GPU) completion callback again, DMACallback(2, _exeque)
   (func_8008DC4C). The four unqueued transfers of the object
   (LoadImage2, StoreImage2, MoveImage2, DrawOTag2) install this function as the
   channel's callback before they start their transfer; called as that callback
   it puts _exeque back. */
/* func_8008E768 */
void _GPU_ResetCallback(void)
{
    DMACallback(2, (Callback)func_8008DC4C);
}

/* cc97_lib: func_8008E790 */
/* gnu_as_loads: func_8008E790 */

/* libgpu SYS.OBJ's own memset(ptr, value, num) (static in sys.c; the 2.60 to 4.00
   signature sets label it): stores the byte value at ptr[0 .. num - 1], counting
   num - 1 down to -1, and returns nothing. num <= 0 is not guarded: 0 stores
   nothing, a negative count runs on. Called by ResetGraph and SetDispMask on GEnv
   and its cached environments, and by _reset on ctlbuf and _que. */
void func_8008E790(u_char *ptr, int value, int num)
{
    int i;

    for (i = num - 1; i != -1; i--) {
        *ptr++ = value;
    }
}

INCLUDE_PAD(4);
