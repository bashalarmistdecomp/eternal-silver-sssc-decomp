/* PSY-Q 4.5 libetc, member intr_dma.o, part 1 of 2: startIntrDMA. The first
   split had it in psyq_spu_j.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: startIntrDMA */
/* far_syms: D_800B7980 (target reaches it %hi/%lo) */
typedef void (*Callback)();     /* libetc INTR_DMA.OBJ: a DMA channel's completion callback */
extern volatile u_long *D_800B7980;     /* INTR_DMA.OBJ .data+0: -> DICR, the DMA interrupt register (0x1F8010F4) */
extern Callback D_800B7984[8];          /* INTR_DMA.OBJ .data+4: the DMA callback slots, one per channel */
void func_800954D4(void);                               /* trapIntrDMA (libetc_intr_dma_2.c) */
Callback func_80095654(int index, Callback callback);   /* setIntrDMA (libetc_intr_dma_2.c) */
void func_80095700(s32 *, s32);                         /* memclr (libetc_intr_dma_2.c) */
Callback InterruptCallback(s32 irq, Callback f);

/* libetc startIntrDMA (INTR_DMA.OBJ): start the DMA interrupt service.  Clears
   the eight callback slots, writes 0 to DICR (every channel's interrupt and the
   master enable off), installs trapIntrDMA (func_800954D4) on interrupt 3 with
   InterruptCallback and returns the address of setIntrDMA (func_80095654), the
   routine DMACallback calls through. */
/* func_80095488 */
void *startIntrDMA(void)
{
    func_80095700((s32 *)D_800B7984, 8);
    *D_800B7980 = 0;
    InterruptCallback(3, func_800954D4);
    return func_80095654;
}
