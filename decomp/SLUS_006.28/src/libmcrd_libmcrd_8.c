/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 8 of 13: MemCardWriteData. The
   first split had it in psyq_misc_aw.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: MemCardWriteData */
/* reorder_fill: MemCardWriteData */
extern McrdGlobal D_800EA960;
extern char D_80013964[];
extern char D_80013A88[];
extern char D_80013AAC[];
extern char D_80013AD8[];
s32 printf();
s32 func_800A24B4(s32 *step);
void UserFuncOpen(s32 (*func)(s32 *step));

/* libmcrd MemCardWriteData (LIBMCRD.OBJ): start an asynchronous write of `bytes`
   bytes from `adrs` at offset `ofs` of the file MemCardOpen opened. MemCardReadData's
   twin: the same four refusals (no file open, a command running, size or offset not
   a multiple of 128; each a printf and 0), then cmd = 6 (McFuncWriteData), rslt and
   done cleared, ofs / adrs / bytes saved in the record and the step function
   _card_write_data (func_800A24B4) queued with UserFuncOpen;
   returns 1. */
/* func_800A23FC */
s32 MemCardWriteData(u32 *adrs, s32 ofs, s32 bytes)
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
    D_800EA960.cmd = 6;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.ofs = ofs;
    D_800EA960.adrs = adrs;
    D_800EA960.bytes = bytes;
    UserFuncOpen(func_800A24B4);
    return 1;
}
