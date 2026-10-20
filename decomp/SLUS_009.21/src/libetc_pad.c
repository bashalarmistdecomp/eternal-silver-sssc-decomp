/* PSY-Q 4.5 libetc, member pad.o: PadRead, PadStop. The first split had it
   in main_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern s32 D_801B86F0;
extern void (*D_801336C8_s)();
s32 PAD_dr();

s32 PAD_dr();                                  /* extern */

/* func_8011F16C */
u_long PadRead(int unused0) {
    PAD_dr();
    return ~D_801B86F0;
}

/* func_8011F194 */
void PadStop(void) {
    StopPAD();
}

INCLUDE_PAD(12);
