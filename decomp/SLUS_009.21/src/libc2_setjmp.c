/* PSY-Q 4.5 libc2, member setjmp.o: setjmp, longjmp. The first split had it
   in main_aak.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80131320 */
INCLUDE_ASM("asm/nonmatchings/main", setjmp);

/* func_8013135C */
INCLUDE_ASM("asm/nonmatchings/main", longjmp);
