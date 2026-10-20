/* PSY-Q 4.5 libspu, member s_m_m.o: SpuMalloc. The first split had it in
   main_ao.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();
/* func_8012AC50 */
INCLUDE_ASM("asm/nonmatchings/main", SpuMalloc);
