/* PSY-Q 4.5 libgpu, member sys.o, part 7 of 7: func_801242C8. The first
   split had it in main_t.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern u32 var_a0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

void func_801242C8(s8 *arg0, s8 arg1, s32 arg2) {
    s32 var_v0;
    s8 *var_a0;

    var_a0 = arg0;
    var_v0 = arg2 - 1;
    if (arg2 != 0) {
        do {
            *var_a0 = arg1;
            var_v0 -= 1;
            var_a0 += 1;
        } while (var_v0 != -1);
    }
}

INCLUDE_PAD(4);
