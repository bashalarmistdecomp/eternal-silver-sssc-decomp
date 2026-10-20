/* PSY-Q 4.5 libgpu, member p01.o: GetClut. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124CE0 */
u_short GetClut(int arg0, int arg1) {
    return ((arg1 << 6) | ((arg0 >> 4) & 0x3F)) & 0xFFFF;
}

INCLUDE_PAD(8);
