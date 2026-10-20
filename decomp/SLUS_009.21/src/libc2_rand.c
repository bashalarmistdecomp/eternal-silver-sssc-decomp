/* PSY-Q 4.5 libc2, member rand.o: rand, srand. The first split had it in
   main_aai.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern s32 temp_v0;

/* redistributed from the original TU (split decl repair) */
extern u32 D_801C1398;
s32 rand(void);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* func_8012FE50 */
s32 rand(void) {
    u32 temp_v0;

    temp_v0 = (D_801C1398 * 0x41C64E6D) + 0x3039;
    D_801C1398 = temp_v0;
    return (temp_v0 >> 0x10) & 0x7FFF;
}

/* func_8012FE80 */
INCLUDE_ASM("asm/nonmatchings/main", srand);
