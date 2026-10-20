/* PSY-Q 4.5 libspu, member s_snv.o: SpuSetNoiseVoice. The first split had
   it in main_ap.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */

/* func_8012B220 */
INCLUDE_ASM("asm/nonmatchings/main", SpuSetNoiseVoice);
