/* PSY-Q 4.5 libc2, member setjmp.o: setjmp, longjmp. The first split had it
   in psyq_spu_g.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

extern int setjmp(int *);

/* func_800952E8 */
INCLUDE_ASM("asm/nonmatchings/psyq_spu", setjmp);

/* func_80095324 */
INCLUDE_ASM("asm/nonmatchings/psyq_spu", longjmp);
