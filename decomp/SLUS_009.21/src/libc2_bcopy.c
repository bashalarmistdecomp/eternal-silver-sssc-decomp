/* PSY-Q 4.5 libc2, member bcopy.o: bcopy. The first split had it in
   main_aai.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern s32 temp_v0;
extern u32 temp_v1;
extern u32 var_a0;
extern s32 var_v0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012FD70 */
u8 *bcopy(u8 *arg0, u8 *arg1, s32 arg2) {
    s32 var_a2;
    u8 *temp_v1;
    u8 *var_a0;
    u8 *var_a1;
    u8 *var_v0;
    u8 temp_v0;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a2 = arg2;
    var_v0 = 0;
    if (var_a0 != 0) {
        temp_v1 = var_a0;
        if (var_a2 > 0) {
            do {
                temp_v0 = *var_a0;
                var_a0 += 1;
                var_a2 -= 1;
                *var_a1 = temp_v0;
                var_a1 += 1;
            } while (var_a2 > 0);
        }
        var_v0 = temp_v1;
    }
    return var_v0;
}

INCLUDE_PAD(12);
