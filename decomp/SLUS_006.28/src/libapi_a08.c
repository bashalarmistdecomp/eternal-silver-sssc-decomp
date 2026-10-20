/* PSY-Q 4.5 libapi, member a08.o: OpenEvent. The first split had it in
   psyq_misc_v.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SpuStart */
/* reorder_fill: SpuStart */

/* func_8009DE58 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", OpenEvent);
