/* PSY-Q 4.5 libetc, member intr.o, part 1 of 2: ResetCallback,
   InterruptCallback, DMACallback, VSyncCallback, VSyncCallbacks,
   StopCallback, RestartCallback, CheckCallback, GetIntrMask. The first
   split had it in psyq_spu_f.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_80094B20 */
/* reorder_fill: func_80094B20 */
/* far_syms: D_800B7930 (target reaches it %hi/%lo) */

typedef void (*Callback)();     
/* intrEnv (0x1068 bytes at D_800B68A8): libetc_intr_2.c's, where INTR.OBJ's own functions are */

/* the callback table (0x20 bytes at 0x800B7910) that D_800B7930 (pCallbacks) points to:
   libetc_intr_2.c's */
extern IntrCallbacks *D_800B7930;   /* pCallbacks */
extern IntrEnv D_800B68A8;          /* intrEnv */
extern volatile u16 *D_800B7938;    
/* The nine API functions that open libetc INTR.OBJ. Each of the first seven forwards
   its arguments to an entry of the callback table and returns what the entry returns. */

/* cc97_lib: ResetCallback */
/* libetc ResetCallback (INTR.OBJ): the table's ResetCallback entry, startIntr. */
/* func_80094BD8 */
int ResetCallback(void) {
    return D_800B7930->ResetCallback();
}

/* cc97_lib: InterruptCallback */
/* libetc InterruptCallback(irq, f) (INTR.OBJ): the table's InterruptCallback entry,
   setIntr: install f as the handler of interrupt irq and return the handler it replaces. */
/* func_80094C08 */
Callback InterruptCallback(s32 irq, Callback f) {
    return D_800B7930->InterruptCallback(irq, f);
}

/* cc97_lib: DMACallback */
/* libetc DMACallback(dma, f) (INTR.OBJ): the table's DMACallback entry (INTR_DMA.OBJ's
   setIntrDMA once startIntr has run): install f as DMA channel dma's completion callback. */
/* func_80094C38 */
Callback DMACallback(s32 dma, Callback f) {
    return D_800B7930->DMACallback(dma, f);
}

/* cc97_lib: VSyncCallback */
/* libetc VSyncCallback(f) (INTR.OBJ): VSyncCallbacks(4, f), slot 4 of the VBlank
   handler's callback list, through the table's VSyncCallbacks entry. */
/* func_80094C68 */
int VSyncCallback(void (*f)(void)) {
    return D_800B7930->VSyncCallbacks(4, f);
}

/* cc97_lib: VSyncCallbacks */
/* libetc VSyncCallbacks(ch, f) (INTR.OBJ): the table's VSyncCallbacks entry (INTR_VB.OBJ's
   setter once startIntr has run): install f in slot ch of the VBlank callback list. */
/* func_80094C9C */
void *VSyncCallbacks(s32 ch, Callback f) {
    return D_800B7930->VSyncCallbacks(ch, f);
}

/* cc97_lib: StopCallback */
/* libetc StopCallback (INTR.OBJ): the table's StopCallback entry, stopIntr. */
/* func_80094CCC */
int StopCallback(void) {
    return D_800B7930->StopCallback();
}

/* cc97_lib: RestartCallback */
/* libetc RestartCallback (INTR.OBJ): the table's RestartCallback entry, restartIntr. */
/* func_80094CFC */
int RestartCallback(void) {
    return D_800B7930->RestartCallback();
}
/* cc97_lib: CheckCallback */

/* libetc CheckCallback (INTR.OBJ): intrEnv.inInterrupt, non-zero while trapIntr is running
   the handlers (the caller is itself inside a callback). */
/* func_80094D2C */
int CheckCallback(void) {
    return D_800B68A8.inInterrupt;
}
/* cc97_lib: GetIntrMask */
/* far_syms: D_800B7938 (target reaches it %hi/%lo) */

/* libetc GetIntrMask (INTR.OBJ): the interrupt mask register I_MASK. */
/* func_80094D3C */
u16 GetIntrMask(void) {
    return *D_800B7938;
}
