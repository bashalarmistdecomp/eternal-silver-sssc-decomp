/* PSY-Q 4.5 libcd, member bios.o, part 3 of 3: CD_datasync, CD_getsector,
   CD_getsector2, CD_set_test_parmnum, func_80126E10. The first split had it
   in main_ac.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;

/* redistributed from the original TU (split decl repair) */
extern volatile u8 *D_80133D08;
extern volatile s32 *D_80133D4C;
s32 CD_getsector();
s32 CD_getsector2();
extern void (*D_801336C8_s)();

/* func_80126AB0 */
INCLUDE_ASM("asm/nonmatchings/main", CD_datasync);

/* func_80126C18 */
INCLUDE_ASM("asm/nonmatchings/main", CD_getsector);

/* far_syms: D_80133D08 */
/* far_syms: D_80133D14 */
/* far_syms: D_80133D18 */
/* far_syms: D_80133D3C */
/* far_syms: D_80133D40 */
/* far_syms: D_80133D44 */
/* far_syms: D_80133D48 */
/* far_syms: D_80133D4C */
extern u8 *D_80133D14;
extern s32 *D_80133D18;
extern s32 *D_80133D3C;
extern s32 *D_80133D40;
extern s32 *D_80133D44;
extern s32 *D_80133D48;

/* func_80126D18 */
s32 CD_getsector2(s32 arg0, s32 arg1) {
    volatile s32 sp0;

    *D_80133D08 = 0;
    *D_80133D14 = 0x80;
    *D_80133D3C = 0x21020843;
    *D_80133D18 = 0x1325;
    *D_80133D40 |= 0x8000;
    *D_80133D44 = arg0;
    *D_80133D48 = arg1 | 0x10000;
    if (!(*D_80133D08 & 0x40)) {
        do {

        } while (!(*D_80133D08 & 0x40));
    }
    *D_80133D4C = 0x11400100;
    sp0 = *D_80133D4C;
    return 0;
}

/* func_80126E04 */
INCLUDE_ASM("asm/nonmatchings/main", CD_set_test_parmnum);

INCLUDE_ASM("asm/nonmatchings/main", func_80126E10);
