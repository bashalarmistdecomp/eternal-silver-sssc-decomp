/* PSY-Q 4.5 libpad, member padentry.o, part 3 of 3: PadInfoMode,
   PadInfoAct, PadInfoComb, PadSetActAlign, PadSetMainMode, PadSetAct. The
   first split had it in main_au.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_8012C658 */
INCLUDE_ASM("asm/nonmatchings/main", PadInfoMode);

/* func_8012C750 */
INCLUDE_ASM("asm/nonmatchings/main", PadInfoAct);

/* func_8012C824 */
INCLUDE_ASM("asm/nonmatchings/main", PadInfoComb);

/* func_8012C8CC */
INCLUDE_ASM("asm/nonmatchings/main", PadSetActAlign);

/* func_8012C904 */
INCLUDE_ASM("asm/nonmatchings/main", PadSetMainMode);

/* func_8012C94C */
INCLUDE_ASM("asm/nonmatchings/main", PadSetAct);
