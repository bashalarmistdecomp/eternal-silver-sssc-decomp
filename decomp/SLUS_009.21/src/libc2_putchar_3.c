/* PSY-Q 4.5 libc2, member putchar.o, part 3 of 3: putchar. The first split
   had it in main_aak.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80130898 */
INCLUDE_ASM("asm/nonmatchings/main", putchar);
