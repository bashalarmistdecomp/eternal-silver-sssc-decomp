/* PSY-Q 4.5 libapi, member a19.o: StartPAD2. The first split had it in
   main_m.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
s32 StartPAD2();

/* func_80120210 */
INCLUDE_ASM("asm/nonmatchings/main", StartPAD2);
