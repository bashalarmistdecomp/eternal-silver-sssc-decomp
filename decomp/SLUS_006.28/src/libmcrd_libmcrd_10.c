/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 10 of 13: MemCardFormat. The
   first split had it in psyq_misc_ay.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: MemCardFormat */
/* reorder_fill: MemCardFormat */
extern McrdGlobal D_800EA960;
extern char D_80013B08[];
s32 printf();
s32 _card_format2(s32 chan);
s32 func_800A34BC(s32 ev);

/* libmcrd MemCardFormat(chan) (LIBMCRD.OBJ): format the card in port chan,
   synchronously. While a command is running (cmd, +0, not 0) it prints "Access
   Denied. : system busy" (D_80013B08; printf) and returns -1.
   Otherwise it calls _card_format2(chan) and maps the result: 0
   stays 0 (McErrNone), 4 becomes 2 (McErrCardInvalid), anything else goes through
   _rslt_to_code (func_800A34BC). */
/* func_800A3394 */
s32 MemCardFormat(s32 chan)
{
    s32 rslt;

    if (D_800EA960.cmd != 0) {
        printf(D_80013B08);
        return -1;
    }
    rslt = _card_format2(chan);
    if (rslt != 0) {
        if (rslt == 4) {
            return 2;
        }
        return func_800A34BC(rslt);
    }
    return 0;
}
