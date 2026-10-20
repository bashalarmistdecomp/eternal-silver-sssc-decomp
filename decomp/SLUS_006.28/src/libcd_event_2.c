/* PSY-Q 4.5 libcd, member event.o, part 2 of 2: func_80090388,
   def_cbsync, func_800903EC, func_80090414. The first split had it in
   psyq_cd_o.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
s32 CD_init(void);
s32 CD_initvol(void);
/* cc97_lib: func_80090388 */

/* libcd EVENT.OBJ, static (no SDK symbol): the reset CdInit retries. It is
   CdReset (SYS.OBJ; CdReset in libcd_sys_2.c) with the mode fixed at 1, the two tests
   written as CdReset writes them: CD_init, then CD_initvol; 0 as soon as
   either fails (returns non-zero), 1 when both succeed. */
s32 func_80090388(void)
{
    if (CD_init()) {
        return 0;
    }
    if (CD_initvol()) {
        return 0;
    }
    return 1;
}

/* cc97_lib: def_cbsync */

/* libcd EVENT.OBJ, static (no SDK symbol; def_cbsync in sotn-decomp's older event.c):
   the default sync callback CdInit installs. It delivers the BIOS event
   HwCdRom (0xF0000003) / EvSpCOMP (0x20, command completed) through DeliverEvent
   . The callback's two arguments (libcd.h CdlCB) are not used. */
/* func_800903C4 */
void def_cbsync(u_char intr, u_char *result)
{
    DeliverEvent(0xF0000003, 0x20);
}
/* cc97_lib: func_800903EC */

/* libcd EVENT.OBJ, static (no SDK symbol; def_cbready in sotn-decomp's older event.c):
   the default ready callback CdInit installs. It delivers the BIOS event
   HwCdRom (0xF0000003) / EvSpDR (0x40, data ready) through DeliverEvent
   . The callback's two arguments are not used. */
void func_800903EC(u_char intr, u_char *result)
{
    DeliverEvent(0xF0000003, 0x40);
}
/* cc97_lib: func_80090414 */

/* libcd EVENT.OBJ, static (no SDK symbol; def_cbread in sotn-decomp's older event.c):
   the default read callback CdInit installs. The same event as the ready
   callback's: HwCdRom (0xF0000003) / EvSpDR (0x40) through DeliverEvent.
   The callback's two arguments are not used. */
void func_80090414(u_char intr, u_char *result)
{
    DeliverEvent(0xF0000003, 0x40);
}

INCLUDE_PAD(12);
