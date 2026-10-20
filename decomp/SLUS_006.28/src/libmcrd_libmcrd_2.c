/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 2 of 13: MemCardStop,
   MemCardExist. The first split had it in psyq_misc_aq.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: MemCardStop */
extern McrdGlobal D_800EA960;
void VSyncCallbacks();
void _card_stop(void);

/* libmcrd MemCardStop (LIBMCRD.OBJ): wait until no command is running (the record's
   cmd, +0, is cleared by the VSync callback when the queued steps finish), remove
   that callback with VSyncCallbacks(7, 0) and stop the card driver
   (_card_stop). */
/* func_800A1968 */
void MemCardStop(void)
{
    while (D_800EA960.cmd != 0) {
    }
    VSyncCallbacks(7, 0);
    _card_stop();
}
/* cc97_lib: MemCardExist */
/* reorder_fill: MemCardExist */
extern u8 D_80013964[];
s32 func_800A1A10(s32 *state);
s32 printf();
void UserFuncOpen(s32 (*func)(s32 *step));

/* libmcrd MemCardExist (LIBMCRD.OBJ): start the asynchronous query of card `chan`'s
   connection. Refused with a printf ("event multiple open") and 0
   while a command is running (cmd, +0, positive); otherwise cmd = 1 (McFuncExist),
   rslt and done are cleared, chan is saved and the step function _card_exist_back
   (func_800A1A10) is queued with UserFuncOpen; returns 1. */
/* func_800A19A8 */
s32 MemCardExist(s32 chan)
{
    if (D_800EA960.cmd > 0) {
        printf(D_80013964);
        return 0;
    }
    D_800EA960.cmd = 1;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.chan = chan;
    UserFuncOpen(func_800A1A10);
    return 1;
}
