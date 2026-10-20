/* PSY-Q 4.5 libpad, member padseqm.o, part 3 of 3: func_8012F4C4,
   _mtapFailAuto, func_8012FBF8. The first split had it in main_aai.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

INCLUDE_ASM("asm/nonmatchings/main", func_8012F4C4);

/* func_8012F8AC */
INCLUDE_ASM("asm/nonmatchings/main", _mtapFailAuto);

INCLUDE_ASM("asm/nonmatchings/main", func_8012FBF8);
