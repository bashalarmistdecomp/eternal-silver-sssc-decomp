/* PSY-Q 4.5 libmcrd, member bios.o, part 2 of 7: _card_open. The first
   split had it in psyq_misc_aae.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: _card_open */
/* libmcrd _card_open (BIOS.OBJ): InitCARD (its argument is the caller's,
   passed through), StartCARD, _bu_init. */
/* func_800A3BC8 */
void _card_open(long val) {
    InitCARD(val);
    StartCARD();
    _bu_init();
}
