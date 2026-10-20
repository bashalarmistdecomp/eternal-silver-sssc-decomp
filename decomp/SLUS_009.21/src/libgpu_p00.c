/* PSY-Q 4.5 libgpu, member p00.o: GetTPage. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124CA0 */
u_short GetTPage(int arg0, int arg1, int arg2, int arg3) {
    return ((arg0 & 3) << 7) | ((arg1 & 3) << 5) | ((s32) (arg3 & 0x100) >> 4) | ((s32) (arg2 & 0x3FF) >> 6) | ((arg3 & 0x200) * 4);
}

INCLUDE_PAD(4);
