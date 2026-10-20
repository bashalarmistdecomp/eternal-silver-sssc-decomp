/* PSY-Q 4.5 libetc, member intr.o, part 2 of 3: GetIntrMask. The first
   split had it in main_e.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_8011F534 */
INCLUDE_ASM("asm/nonmatchings/main", GetIntrMask);
