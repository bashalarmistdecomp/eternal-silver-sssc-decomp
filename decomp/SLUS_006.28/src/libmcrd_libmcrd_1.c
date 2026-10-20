/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 1 of 13: MemCardStart. The
   first split had it in psyq_misc_ap.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: MemCardStart */
/* return_slot_fill: MemCardStart */
extern McrdGlobal D_800EA960;
void func_800A3510(void);
void UserFuncInit(void);
void _card_start(void);
void VSyncCallbacks();

/* libmcrd MemCardStart() (LIBMCRD.OBJ): start the memory-card system. Clears the
   record's port mask (+0xC) and the user's MemCardCallback function (+0x44), empties
   the step-function queue (UserFuncInit), clears the running command,
   its result and the completion flag (+0, +4, +8), zeroes the two per-port counters
   (+0x54, then +0x50 from it), marks no file open (fd = -1) and sets the two per-port
   flags (+0x4C, +0x48) to 1; then _card_start() (opens and enables
   the card events and starts the BIOS card driver) and installs the queue's VSync
   callback _callback_handler (func_800A3510) with VSyncCallbacks(7, ...). */
/* func_800A18F8 */
void MemCardStart(void)
{
    D_800EA960.mask = 0;
    D_800EA960.func = 0;
    UserFuncInit();
    D_800EA960.cmd = 0;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.frames[0] = D_800EA960.frames[1] = 0;
    D_800EA960.fd = -1;
    D_800EA960.new_card[0] = D_800EA960.new_card[1] = 1;
    _card_start();
    VSyncCallbacks(7, func_800A3510);
}
