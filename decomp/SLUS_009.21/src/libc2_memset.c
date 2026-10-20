/* PSY-Q 4.5 libc2, member memset.o: memset. The first split had it in
   main_aai.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
s32 memset(s32 *, s32, s32);
extern void (*D_801336C8_s)();

/* func_8012FE20 */
INCLUDE_ASM("asm/nonmatchings/main", memset);
