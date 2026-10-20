/* PSY-Q 4.5 libc2, member strcmp.o: strcmp. The first split had it in
   main_aai.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012FE90 */
INCLUDE_ASM("asm/nonmatchings/main", strcmp);
