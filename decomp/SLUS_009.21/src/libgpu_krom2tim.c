/* PSY-Q 4.5 libgpu, member krom2tim.o: GetTimSize, Krom2Tim. The first
   split had it in main_n.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* alias pair: the scalar store shape of D_801336C8 (the array form at
   its other declaration is load-bearing for the reader below) */
extern void (*D_801336C8_s)();

/* func_80120E00 */
INCLUDE_ASM("asm/nonmatchings/main", GetTimSize);

/* func_80120EAC */
INCLUDE_ASM("asm/nonmatchings/main", Krom2Tim);
