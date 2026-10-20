/* PSY-Q 4.5 libpress, member libpress.o, part 2 of 4: MDEC_rest. The
   first split had it in main_ah.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
s32 MDEC_rest(s32);

/* func_80129644 */
INCLUDE_ASM("asm/nonmatchings/main", MDEC_rest);
