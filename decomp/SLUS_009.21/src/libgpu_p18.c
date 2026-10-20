/* PSY-Q 4.5 libgpu, member p18.o: SetPolyG4. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124DD0 */
void SetPolyG4(POLY_G4 *arg0) {
    (*(u8 *)((u8 *)arg0 + 0x3)) = 8;
    (*(u8 *)((u8 *)arg0 + 0x7)) = 0x38;
}

INCLUDE_PAD(12);
