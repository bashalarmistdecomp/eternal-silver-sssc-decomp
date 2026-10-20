/* PSY-Q 4.5 libgpu, member p17.o: SetPolyFT4. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124DB0 */
void SetPolyFT4(POLY_FT4 *arg0) {
    (*(u8 *)((u8 *)arg0 + 0x3)) = 9;
    (*(u8 *)((u8 *)arg0 + 0x7)) = 0x2C;
}

INCLUDE_PAD(12);
