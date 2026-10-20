/* PSY-Q 4.5 libapi, member a07.o: DeliverEvent. The first split had it in
   psyq_cd_o.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80090448 */
INCLUDE_ASM("asm/nonmatchings/psyq_cd", DeliverEvent);
