/* PSY-Q 4.5 libspu, member s_dcb.o: _SpuDataCallback. The first split had
   it in psyq_misc_aa.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */


#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _SpuDataCallback */
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern Callback DMACallback(s32 dma, Callback f);

/* libspu _SpuDataCallback(func) (S_DCB.OBJ): make func the interrupt callback of DMA
   channel 4, the SPU's: DMACallback(4, func). Caller: SpuStart, with
   _spu_FiDMA. */
/* func_8009E9B8 */
void _SpuDataCallback(void (*func)(void)) {
    DMACallback(4, func);
}

INCLUDE_PAD(12);
