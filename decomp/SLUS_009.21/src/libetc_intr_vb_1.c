/* PSY-Q 4.5 libetc, member intr_vb.o, part 1 of 2: startIntrVSync. The
   first split had it in main_g.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_8011FA90 */
INCLUDE_ASM("asm/nonmatchings/main", startIntrVSync);
