/* PSY-Q 4.5 libapi, member pad.o, part 2 of 4: StopPAD. The first split had
   it in psyq_misc_am.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800B8440 (target reaches it %hi/%lo) */
s32 RemovePatchPad();
s32 StopPAD2();
extern s32 D_800B8440;

/* cc97_lib: StopPAD */

/* libapi StopPAD (PAD.OBJ): DisablePAD, the BIOS StopPAD2, RemovePatchPad,
   then the pad-initialised flag D_800B8440 = 0 */
/* func_800A1324 */
void StopPAD(void)
{
    DisablePAD();
    StopPAD2();
    RemovePatchPad();
    D_800B8440 = 0;
}
