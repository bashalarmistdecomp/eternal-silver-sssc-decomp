/* PSY-Q 4.5 libmcrd, member bios.o, part 5 of 7: _card_stop. The first
   split had it in psyq_misc_aah.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800EAA40 */
/* far_syms: D_800EAA44 */
/* far_syms: D_800EAA48 */
/* far_syms: D_800EAA4C */
/* far_syms: D_800EAA50 */
/* far_syms: D_800EAA54 */
/* far_syms: D_800EAA58 */
/* far_syms: D_800EAA5C */
extern s32 D_800EAA40, D_800EAA44, D_800EAA48, D_800EAA4C, D_800EAA50, D_800EAA54, D_800EAA58, D_800EAA5C;

/* cc97_lib: _card_stop */
/* return_slot_fill: _card_stop */
/* libmcrd _card_stop (BIOS.OBJ): inside a critical section, CloseEvent
   on the eight card events _card_start opened (D_800EAA40 ... D_800EAA5C). */
/* func_800A3DF4 */
void _card_stop(void)
{
    s32 crit;

    crit = EnterCriticalSection();
    CloseEvent(D_800EAA40);
    CloseEvent(D_800EAA44);
    CloseEvent(D_800EAA48);
    CloseEvent(D_800EAA4C);
    CloseEvent(D_800EAA50);
    CloseEvent(D_800EAA54);
    CloseEvent(D_800EAA58);
    CloseEvent(D_800EAA5C);
    if (crit == 1) {
        ExitCriticalSection();
    }
}
