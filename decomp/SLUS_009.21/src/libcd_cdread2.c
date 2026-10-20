/* PSY-Q 4.5 libcd, member cdread2.o: CdRead2, func_801282B4. The first
   split had it in main_ac.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80128230 */
INCLUDE_ASM("asm/nonmatchings/main", CdRead2);

void func_801282B4(void) {
    StCdInterrupt();
}
