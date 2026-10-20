/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 4 of 13: MemCardAccept. The
   first split had it in psyq_misc_as.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: MemCardAccept */
/* reorder_fill: MemCardAccept */
extern McrdGlobal D_800EA960;
extern u8 D_80013964[];
s32 func_800A1D94(s32 *state);
s32 printf();
void UserFuncOpen(s32 (*func)(s32 *step));

/* libmcrd MemCardAccept (LIBMCRD.OBJ): start the asynchronous check of card `chan`
   (its connection and, for a new card, the clear and load that make its files
   usable). Refused with a printf ("event multiple open") and 0
   while a command is running (cmd, +0, positive); otherwise cmd = 2
   (McFuncAccept), rslt and done are cleared, chan is saved and the step function
   _card_accept_back (func_800A1D94) is queued with UserFuncOpen;
   returns 1. MemCardOpen and MemCardDeleteFile carry this body inlined. */
/* func_800A1D2C */
s32 MemCardAccept(s32 chan)
{
    if (D_800EA960.cmd > 0) {
        printf(D_80013964);
        return 0;
    }
    D_800EA960.cmd = 2;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.chan = chan;
    UserFuncOpen(func_800A1D94);
    return 1;
}
