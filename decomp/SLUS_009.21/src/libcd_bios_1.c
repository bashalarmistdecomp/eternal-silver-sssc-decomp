/* PSY-Q 4.5 libcd, member bios.o, part 1 of 3: func_80125780, CD_sync,
   CD_ready, CD_cw, CD_vol, CD_flush, CD_initvol. The first split had it in
   main_aa.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* the .s slot is 0x30; the code is 0x2C (the rest is trailing padding) */

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();
s32 CD_initvol();
s32 CD_vol();

INCLUDE_ASM("asm/nonmatchings/main", func_80125780);

/* func_80125CE4 */
INCLUDE_ASM("asm/nonmatchings/main", CD_sync);

/* func_80125F64 */
INCLUDE_ASM("asm/nonmatchings/main", CD_ready);

/* func_8012622C */
INCLUDE_ASM("asm/nonmatchings/main", CD_cw);

/* func_80126638 */
INCLUDE_ASM("asm/nonmatchings/main", CD_vol);

/* func_801266C0 */
INCLUDE_ASM("asm/nonmatchings/main", CD_flush);

/* func_80126794 */
INCLUDE_ASM("asm/nonmatchings/main", CD_initvol);
