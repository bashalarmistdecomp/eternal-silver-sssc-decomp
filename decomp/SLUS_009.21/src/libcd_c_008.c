/* PSY-Q 4.5 libcd, member c_008.o: init_ring_status. The first split had it
   in main_ae.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 var_a2;

/* redistributed from the original TU (split decl repair) */
extern s32 D_801BE4BC;
extern void (*D_801336C8_s)();

/* func_801285F0 */
void init_ring_status(s32 arg0, u32 arg1) {
    s32 temp_v0;
    u32 var_a2;

    var_a2 = 0;
    if (arg1 != 0) {
        do {
            temp_v0 = var_a2 + arg0;
            var_a2 += 1;
            (*(s32 *)(D_801BE4BC + (temp_v0 << 5))) = 0;
        } while (var_a2 < arg1);
    }
}

INCLUDE_PAD(12);
