/* PSY-Q 4.5 libetc, member vsync.o, part 1 of 2: VSync. The first split had
   it in main_b.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();
s32 VSync(s32);

/* func_8011F1C0 */
INCLUDE_ASM("asm/nonmatchings/main", VSync);
