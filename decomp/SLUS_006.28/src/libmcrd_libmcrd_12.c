/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 12 of 13: func_800A3510. The
   first split had it in psyq_misc_aaa.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_800A3510 */
extern volatile s32 D_800EA950[];
extern McrdGlobal D_800EA960;
u32 UserFuncComplete(void);
void UserFuncExecute(void);

/* libmcrd _callback_handler (LIBMCRD.OBJ, static): the VSync callback MemCardStart
   installs. While the queued step functions are not finished (UserFuncComplete) it runs one step (UserFuncExecute); when that step
   was the last, the command is complete: done (+8) = 1, the command code (+0) and
   its result (+4) move to the last-result pair D_800EA950 (what MemCardSync
   reports), both are cleared, and the user's MemCardCallback function (+0x44), if
   any, is called with the pair. Every call counts two frame counters (+0x50,
   +0x54). */
void func_800A3510(void)
{
    if (UserFuncComplete() == 0) {
        UserFuncExecute();
        if (UserFuncComplete() != 0) {
            D_800EA960.done = 1;
            D_800EA950[0] = D_800EA960.cmd;
            D_800EA950[1] = D_800EA960.rslt;
            D_800EA960.cmd = 0;
            D_800EA960.rslt = 0;
            if (D_800EA960.func != 0) {
                D_800EA960.func(D_800EA950[0], D_800EA950[1]);
            }
        }
    }
    D_800EA960.frames[0]++;
    D_800EA960.frames[1]++;
}
