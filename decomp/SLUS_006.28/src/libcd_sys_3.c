/* PSY-Q 4.5 libcd, member sys.o, part 3 of 7: CdFlush. The first split had
   it in psyq_cd_q.c. The tree compiles this run of the member's functions
   with other flags than the runs beside it (config/cc_flags.txt), so the
   member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

s32 CD_flush();                                  /* extern */

/* cc97_lib: CdFlush */
/* libcd CdFlush (SYS.OBJ): calls CD_flush (BIOS.OBJ). */
/* func_80090500 */
void CdFlush(void) {
    CD_flush();
}
