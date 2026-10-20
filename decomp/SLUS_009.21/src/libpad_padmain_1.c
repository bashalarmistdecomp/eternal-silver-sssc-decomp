/* PSY-Q 4.5 libpad, member padmain.o, part 1 of 6: PadEnableCom,
   _padSetVsyncParam. The first split had it in main_au.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_8012C9A0 */
INCLUDE_ASM("asm/nonmatchings/main", PadEnableCom);

/* func_8012CAB8 */
INCLUDE_ASM("asm/nonmatchings/main", _padSetVsyncParam);
