/* PSY-Q 4.5 libapi, member c73.o: GPU_cw. The first split had it in
   psyq_gpu_g.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _GPU_ResetCallback */
/* gnu_as_loads: _GPU_ResetCallback */
     
/* func_8008E7B8 */
INCLUDE_ASM("asm/nonmatchings/psyq_gpu", GPU_cw);
