/* PSY-Q 4.5 libapi, member pad.o, part 3 of 4: func_800A135C,
   RemovePatchPad. The first split had it in psyq_misc_an.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* libapi PAD.OBJ .bss+0 (`_apiPadPatch` in SDK 4.6): the entry PAD.OBJ puts in the kernel's
   interrupt queue 1. The kernel calls func1 and, when it returns nonzero, func2. */
extern PadIntRP D_800EA8E0;

s32 EnterCriticalSection();                                  /* extern */
s32 SysDeqIntRP(s32, PadIntRP *);                   /* extern */
/* cc97_lib: func_800A135C */
/* return_slot_fill: func_800A135C */
s32 SysEnqIntRP(s32, PadIntRP *);
s32 _Pad1(void);
s32 _IsVSync(void);

static inline void pad_set_vsync(void)
{
    D_800EA8E0.func2 = _Pad1;
    D_800EA8E0.func1 = _IsVSync;
    D_800EA8E0.next = 0;
    D_800EA8E0.unk_0c = 0;
}
/* libapi SetPatchPad (PAD.OBJ, static): fill the pad entry D_800EA8E0 (func1 = _IsVSync,
   func2 = _Pad1, next and pad zero) and put it at the head of the kernel's interrupt queue 1
   (SysDeqIntRP, then SysEnqIntRP), inside a critical section; returns 1.
   The four stores are the inlined pad_set_vsync above: LIBTAP 3.3's TAP.OBJ has them out of
   line as `_SetVsync`. Inlined, the two zero stores go straight to the symbol and the two
   function addresses through a register holding &D_800EA8E0.func2, as the object has them. */
s32 func_800A135C(void)
{
    EnterCriticalSection();
    pad_set_vsync();
    SysDeqIntRP(1, &D_800EA8E0);
    SysEnqIntRP(1, &D_800EA8E0);
    ExitCriticalSection();
    return 1;
}

/* cc97_lib: RemovePatchPad */

/* libapi RemovePatchPad (PAD.OBJ, static): take the pad entry out of the kernel's interrupt
   queue 1 with SysDeqIntRP, inside a critical section; returns 1 */
/* func_800A13D4 */
s32 RemovePatchPad(void) {
    EnterCriticalSection();
    SysDeqIntRP(1, &D_800EA8E0);
    ExitCriticalSection();
    return 1;
}
