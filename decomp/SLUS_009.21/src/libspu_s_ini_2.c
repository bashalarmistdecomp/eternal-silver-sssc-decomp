/* PSY-Q 4.5 libspu, member s_ini.o, part 2 of 2: SpuStart. The first split
   had it in main_al.c. The tree compiles this run of the member's functions
   with other flags than the runs beside it (config/cc_flags.txt), so the
   member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_80129FE8 */
INCLUDE_ASM("asm/nonmatchings/main", SpuStart);
