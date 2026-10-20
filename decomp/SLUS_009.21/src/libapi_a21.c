/* PSY-Q 4.5 libapi, member a21.o: PAD_init2. The first split had it in
   main_m.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80120230 */
INCLUDE_ASM("asm/nonmatchings/main", PAD_init2);
