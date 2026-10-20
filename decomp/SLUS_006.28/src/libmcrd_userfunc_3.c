/* PSY-Q 4.5 libmcrd, member userfunc.o, part 3 of 3: UserFuncComplete. The
   first split had it in psyq_misc_aad.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800B8470 (target reaches it %hi/%lo) */

/* cc97_lib: UserFuncComplete */
extern s32 D_800B8470;

/* libmcrd UserFuncComplete (USERFUNC.OBJ): 1 when the stack of step functions is empty
   (its top index D_800B8470 is negative), 0 while a step is left. libmcrd's VSync
   callback (_callback_handler, func_800A3510) runs UserFuncExecute only while
   this is 0, and reports the command complete when it turns 1. */
/* func_800A3B10 */
u32 UserFuncComplete(void)
{
    return D_800B8470 < 0;
}

INCLUDE_PAD(8);
