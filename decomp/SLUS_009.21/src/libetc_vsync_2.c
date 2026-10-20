/* PSY-Q 4.5 libetc, member vsync.o, part 2 of 2: func_8011F338. The first
   split had it in main_c.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

INCLUDE_ASM("asm/nonmatchings/main", func_8011F338);
