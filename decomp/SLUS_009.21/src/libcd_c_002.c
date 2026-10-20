/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libcd/c_002.c), psyz (decomp/src/libcd/c_002.c) and
   psy-q-decomp (src/cd/c_002.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member c_002.o: StClearRing. The first split had it in
   main_ad.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

INCLUDE_PAD(12);
extern s32 D_801BE4C8;
extern s32 D_801BE4B4;
extern s32 D_801BE4B0;
extern s32 D_801BE4AC;
extern s32 D_801BE4D8;
extern s32 D_801BE4CC;
extern s16 D_801BE4A0;
extern s32 D_801BE4C4;
extern void init_ring_status(s32, u32);

/* func_801282E0 */
void StClearRing(void) {
    s32 t;
    t = D_801BE4C8;
    D_801BE4B4 = 0;
    D_801BE4B0 = 0;
    D_801BE4AC = 0;
    D_801BE4D8 = 0;
    init_ring_status(0, t);
    D_801BE4CC = 0;
    D_801BE4A0 = 0;
    D_801BE4C4 = 0;
}
