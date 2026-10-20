/* PSY-Q 4.5 libpad, member padseqm.o, part 1 of 3: _padInitMtapSeq. The
   first split had it in main_aag.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012F220 */
INCLUDE_ASM("asm/nonmatchings/main", _padInitMtapSeq);
