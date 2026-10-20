/* PSY-Q 4.5 libgpu, member p10.o: SetSemiTrans. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 var_v0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124D60 */
void SetSemiTrans(void *arg0, s32 arg1) {
    u8 var_v0;

    if (arg1 != 0) {
        var_v0 = (*(u8 *)((u8 *)arg0 + 0x7)) | 2;
    } else {
        var_v0 = (*(u8 *)((u8 *)arg0 + 0x7)) & 0xFD;
    }
    (*(u8 *)((u8 *)arg0 + 0x7)) = var_v0;
}

INCLUDE_PAD(8);
