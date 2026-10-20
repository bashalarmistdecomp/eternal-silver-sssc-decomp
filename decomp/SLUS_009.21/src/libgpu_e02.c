/* PSY-Q 4.5 libgpu, member e02.o: LoadClut2. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124B30 */
INCLUDE_ASM("asm/nonmatchings/main", LoadClut2);
