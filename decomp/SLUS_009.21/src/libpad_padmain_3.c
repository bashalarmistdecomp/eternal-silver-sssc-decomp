/* PSY-Q 4.5 libpad, member padmain.o, part 3 of 6: _padIntPad,
   _padChkVsync, _padStartCom. The first split had it in main_aw.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_8012CB4C */
INCLUDE_ASM("asm/nonmatchings/main", _padIntPad);

/* func_8012CCDC */
INCLUDE_ASM("asm/nonmatchings/main", _padChkVsync);

/* func_8012CCF0 */
INCLUDE_ASM("asm/nonmatchings/main", _padStartCom);
