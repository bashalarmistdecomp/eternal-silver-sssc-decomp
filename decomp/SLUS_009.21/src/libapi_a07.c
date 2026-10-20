/* PSY-Q 4.5 libapi, member a07.o: DeliverEvent. The first split had it in
   main_aa.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* the .s slot is 0x30; the code is 0x2C (the rest is trailing padding) */

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();
/* func_80125160 */
INCLUDE_ASM("asm/nonmatchings/main", DeliverEvent);
