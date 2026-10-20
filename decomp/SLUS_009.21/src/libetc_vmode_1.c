/* PSY-Q 4.5 libetc, member vmode.o, part 1 of 2: SetVideoMode. The first
   split had it in main_t.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

extern s32 D_8013381C;

/* func_801242F0 */
long SetVideoMode(long arg0) {
    s32 old = D_8013381C;
    D_8013381C = arg0;
    return old;
}
