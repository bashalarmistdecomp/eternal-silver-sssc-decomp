/* PSY-Q 4.5 libgpu, member sys.o, part 4 of 7: set_alarm,
   func_80123CA4, func_80123DE8. The first split had it in main_q.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_80123C70 */
INCLUDE_ASM("asm/nonmatchings/main", set_alarm);

INCLUDE_ASM("asm/nonmatchings/main", func_80123CA4);

INCLUDE_ASM("asm/nonmatchings/main", func_80123DE8);
