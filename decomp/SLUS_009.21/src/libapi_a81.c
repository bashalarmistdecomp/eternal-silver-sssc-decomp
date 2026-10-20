/* PSY-Q 4.5 libapi, member a81.o: Krom2RawAdd. The first split had it in
   main_n.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* alias pair: the scalar store shape of D_801336C8 (the array form at
   its other declaration is load-bearing for the reader below) */
extern void (*D_801336C8_s)();

/* func_801213B0 */
INCLUDE_ASM("asm/nonmatchings/main", Krom2RawAdd);
