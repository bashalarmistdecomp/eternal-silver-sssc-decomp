/* PSY-Q 4.5 libpad, member waitrc2.o: setRC2wait, chkRC2wait. The first
   split had it in psyq_misc_aas.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* Sony libpad, WAITRC2.OBJ whole (0x800A7CD8..0x800A7D97 of the image). Every word equals the
   on-disk SDK 4.4 object's, relocation fields apart; the release linked is SDK 4.5's, which is
   the same code (the two LIBPAD.LIB differ only in member dates and in the version word of
   PADMAIN.OBJ's .data, and the image holds 4.5's, 0x00450000 at 0x800B8484). These functions
   were `.word` rows of asm/data/data_tables.data.s: the first split cut text from data at
   0x800A5000, and the image's text runs to 0x800A7D98. No config/cc_flags.txt entry: the
   object's converted functions take per-function markers (cc97_lib, and return_slot_fill by the
   epilogue rule). */

/* libpad setRC2wait (WAITRC2.OBJ +0x000), 0x20 bytes. */
/* cc97_lib: setRC2wait */
/* return_slot_fill: setRC2wait */
extern u32 D_800EAFE0;
extern u32 D_800EAFE4;

/* libpad setRC2wait (WAITRC2.OBJ, SDK 4.5): starts a wait on root counter 2. Stores the
   length in _waitTime (D_800EAFE4) and the counter's present value (the 16-bit count
   register at 0x1F801120) in _startTime (D_800EAFE0); chkRC2wait says
   when it is over. The length is in the units chkRC2wait compares in, counter ticks of
   the system clock / 8. Callers: PADMAIN.OBJ and PADIF.OBJ. */
/* func_800A7CD8 */
void setRC2wait(u32 wait)
{
    D_800EAFE4 = wait;
    D_800EAFE0 = *(volatile u16 *)0x1F801120;
}

/* libpad chkRC2wait (WAITRC2.OBJ +0x020), 0xA0 bytes. */
/* cc97_lib: chkRC2wait */
/* return_slot_fill: chkRC2wait */

/* libpad chkRC2wait (WAITRC2.OBJ, SDK 4.5): 1 when the wait setRC2wait
   started is over, 0 while it runs. Reads root counter 2's count (0x1F801120); a count
   below _startTime (D_800EAFE0) means the counter wrapped, and then the target value
   (0x1F801128) is added, or 0x10000 when the target is 0. The time gone by is the
   difference from _startTime, taken as it is when the counter runs on the system
   clock / 8 (mode register 0x1F801124, bit 9) and divided by 8 when it runs on the
   system clock; the wait is over when that reaches _waitTime (D_800EAFE4). */
/* func_800A7CF8 */
s32 chkRC2wait(void)
{
    u32 now = *(volatile u16 *)0x1F801120;

    if (now < D_800EAFE0) {
        if (*(volatile u16 *)0x1F801128 != 0) {
            now += *(volatile u16 *)0x1F801128;
        } else {
            now += 0x10000;
        }
    }
    if (*(volatile u16 *)0x1F801124 & 0x200) {
        return now - D_800EAFE0 >= D_800EAFE4;
    }
    return (now - D_800EAFE0) / 8 >= D_800EAFE4;
}
