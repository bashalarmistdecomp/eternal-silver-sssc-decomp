/* PSY-Q 4.5 libmcrd, member bios.o, part 4 of 7: _card_close. The first
   split had it in psyq_misc_aag.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: _card_close */
/* libmcrd _card_close (BIOS.OBJ): StopCARD. */
/* func_800A3DD4 */
void _card_close(void) {
    StopCARD();
}
