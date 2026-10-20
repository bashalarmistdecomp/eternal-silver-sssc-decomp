/* PSY-Q 4.5 libapi, member pad.o, part 4 of 4: _Pad1,
   _IsVSync. The first split had it in main_m.c. The tree compiles this
   run of the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80120154 */
INCLUDE_ASM("asm/nonmatchings/main", _Pad1);

/* func_801201BC */
INCLUDE_ASM("asm/nonmatchings/main", _IsVSync);
