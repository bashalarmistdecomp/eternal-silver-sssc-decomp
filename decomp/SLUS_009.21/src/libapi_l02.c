/* PSY-Q 4.5 libapi, member l02.o: SysEnqIntRP. The first split had it in
   main_m.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80120240 */
INCLUDE_ASM("asm/nonmatchings/main", SysEnqIntRP);
