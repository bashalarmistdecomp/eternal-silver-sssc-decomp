/* PSY-Q 4.5 libetc, member intr_vb.o, part 1 of 2: startIntrVSync. The
   first split had it in psyq_spu_h.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: startIntrVSync */
/* far_syms: D_800B7970 (target reaches it %hi/%lo) */
/* far_syms: D_800B7974 (target reaches it %hi/%lo) */
typedef void (*Callback)();     /* libetc INTR_VB.OBJ: a vertical-blank callback */
extern Callback D_800B7950[8];          /* INTR_VB.OBJ .data+0: the vertical-blank callback slots */
extern volatile s32 D_800B7970;         /* Vcount (INTR_VB.OBJ .data+0x20): the vertical-blank count */
extern volatile u_long *D_800B7974;     /* INTR_VB.OBJ .data+0x24: -> root counter 1's mode register (0x1F801114) */
void func_800953C0(void);                               /* trapIntrVSync (libetc_intr_vb_2.c) */
Callback setIntrVSync(int index, Callback callback);   /* setIntrVSync (libetc_intr_vb_2.c) */
void func_80095458(s32 *, s32);                         /* memclr (libetc_intr_vb_2.c) */
Callback InterruptCallback(s32 irq, Callback f);

/* libetc startIntrVSync (INTR_VB.OBJ): start the vertical-blank interrupt service.
   Writes 0x100 to root counter 1's mode register, zeroes Vcount (D_800B7970),
   clears the eight callback slots, installs trapIntrVSync (func_800953C0) on
   interrupt 0 with InterruptCallback and returns the address of setIntrVSync
   (setIntrVSync), the routine VSyncCallback and VSyncCallbacks (VSyncCallback,
   VSyncCallbacks) call through. */
/* func_80095368 */
void *startIntrVSync(void)
{
    *D_800B7974 = 0x100;
    D_800B7970 = 0;
    func_80095458((s32 *)D_800B7950, 8);
    InterruptCallback(0, func_800953C0);
    return setIntrVSync;
}
