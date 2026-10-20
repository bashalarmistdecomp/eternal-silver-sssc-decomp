/* PSY-Q 4.5 libgpu, member sys.o, part 6 of 7: _GPU_ResetCallback. The
   first split had it in main_s.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_801242A0 */
INCLUDE_ASM("asm/nonmatchings/main", _GPU_ResetCallback);
