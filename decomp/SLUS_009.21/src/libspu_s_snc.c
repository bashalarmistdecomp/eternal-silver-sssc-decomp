/* PSY-Q 4.5 libspu, member s_snc.o: SpuSetNoiseClock. The first split had
   it in main_aq.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012B510 */
INCLUDE_ASM("asm/nonmatchings/main", SpuSetNoiseClock);
