/* PSY-Q 4.5 libspu, member s_stsa.o: SpuSetTransferStartAddr. The first
   split had it in main_ar.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */

/* func_8012B7B0 */
INCLUDE_ASM("asm/nonmatchings/main", SpuSetTransferStartAddr);
