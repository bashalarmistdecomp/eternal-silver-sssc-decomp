/* PSY-Q 4.5 libpad, member padmain.o, part 5 of 6: _padInitSioMode,
   func_8012D134, _padSioRW, _padSioRW2. The first split had it in
   main_ay.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_8012CE00 */
INCLUDE_ASM("asm/nonmatchings/main", _padInitSioMode);

INCLUDE_ASM("asm/nonmatchings/main", func_8012D134);

/* func_8012D224 */
INCLUDE_ASM("asm/nonmatchings/main", _padSioRW);

/* func_8012D434 */
INCLUDE_ASM("asm/nonmatchings/main", _padSioRW2);
