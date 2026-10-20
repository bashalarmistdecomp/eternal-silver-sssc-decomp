/* PSY-Q 4.5 libgpu, member font.o: SetDumpFnt, FntLoad, FntOpen, FntFlush,
   FntPrint. The first split had it in main_n.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
void SetDumpFnt(s32 arg0);  /* TU defines this below */
s32 FntOpen(s32, s32, s32, s32, s32, s32);

extern s32 D_80132C6C[];
/* far_syms: D_80132C70 */
extern s32 D_80132C70;
/* alias pair: the scalar store shape of D_801336C8 (the array form at
   its other declaration is load-bearing for the reader below) */
/* far_syms: D_801336C8_s */
extern void (*D_801336C8_s)();
/* no_delayed_branch: SetDumpFnt */

/* func_80120380 */
void SetDumpFnt(s32 arg0) {
    if (arg0 < 0) {
        return;
    }
    if (D_80132C6C[0] < arg0) {
        return;
    }
    D_80132C70 = arg0;
    D_801336C8_s = (void (*)()) FntPrint;
}

/* func_801203C0 */
INCLUDE_ASM("asm/nonmatchings/main", FntLoad);

/* func_80120460 */
INCLUDE_ASM("asm/nonmatchings/main", FntOpen);

/* func_80120718 */
INCLUDE_ASM("asm/nonmatchings/main", FntFlush);

/* func_80120A34 */
INCLUDE_ASM("asm/nonmatchings/main", FntPrint);
