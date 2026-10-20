/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 6 of 13: MemCardReadData. The
   first split had it in psyq_misc_au.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: MemCardReadData */
/* reorder_fill: MemCardReadData */
extern McrdGlobal D_800EA960;
extern char D_80013964[];
extern char D_80013A88[];
extern char D_80013AAC[];
extern char D_80013AD8[];
s32 printf();
s32 func_800A22C0(s32 *step);
void UserFuncOpen(s32 (*func)(s32 *step));

/* libmcrd MemCardReadData (LIBMCRD.OBJ): start an asynchronous read of `bytes`
   bytes at offset `ofs` of the file MemCardOpen opened into `adrs`. Refused with a
   printf and 0 when no file is open (fd, +0x14, negative: "file not
   open"), when a command is running (cmd, +0, positive: "event multiple open"), or
   when the size or the offset is not a multiple of 128 (one card frame). Otherwise
   cmd = 5 (McFuncReadData), rslt and done are cleared, the three arguments are
   saved in the record (+0x18 ofs, +0x20 adrs, +0x1C bytes) and the step function
   _card_read_data (func_800A22C0) is queued with UserFuncOpen;
   returns 1. The VSync callback (func_800A3510) then runs the step until it finishes. */
/* func_800A2208 */
s32 MemCardReadData(u32 *adrs, s32 ofs, s32 bytes)
{
    if (D_800EA960.fd < 0) {
        printf(D_80013A88);
        return 0;
    }
    if (D_800EA960.cmd > 0) {
        printf(D_80013964);
        return 0;
    }
    if (bytes & 0x7F) {
        printf(D_80013AAC);
        return 0;
    }
    if (ofs & 0x7F) {
        printf(D_80013AD8);
        return 0;
    }
    D_800EA960.cmd = 5;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.ofs = ofs;
    D_800EA960.adrs = adrs;
    D_800EA960.bytes = bytes;
    UserFuncOpen(func_800A22C0);
    return 1;
}
