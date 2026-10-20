/* PSY-Q 4.5 libgpu, member p09.o: TermPrim. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124D40 */
void TermPrim(void *arg0) {
    *(s32 *) arg0 |= 0xFFFFFF;
}

INCLUDE_PAD(8);
