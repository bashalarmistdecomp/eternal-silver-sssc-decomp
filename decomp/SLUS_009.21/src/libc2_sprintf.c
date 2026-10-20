/* PSY-Q 4.5 libc2, member sprintf.o: sprintf. The first split had it in
   main_aak.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
s32 sprintf(s32, s32, s32, s32, ...);

/* func_801309D0 */
INCLUDE_ASM("asm/nonmatchings/main", sprintf);
