/* PSY-Q 4.5 libpad, member waitrc2.o: setRC2wait, chkRC2wait. The first
   split had it in main_aai.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012FCB0 */
INCLUDE_ASM("asm/nonmatchings/main", setRC2wait);

/* func_8012FCD0 */
INCLUDE_ASM("asm/nonmatchings/main", chkRC2wait);
