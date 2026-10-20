/* PSY-Q 4.5 libcd, member c_003.o: StUnSetRing. The first split had it in
   main_ad.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80128340 */
INCLUDE_ASM("asm/nonmatchings/main", StUnSetRing);
