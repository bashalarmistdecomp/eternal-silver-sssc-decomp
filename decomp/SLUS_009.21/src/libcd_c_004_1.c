/* PSY-Q 4.5 libcd, member c_004.o, part 1 of 2: data_ready_callback. The
   first split had it in main_ad.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_801283C0 */
INCLUDE_ASM("asm/nonmatchings/main", data_ready_callback);
