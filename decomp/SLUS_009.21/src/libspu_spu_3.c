/* PSY-Q 4.5 libspu, member spu.o, part 3 of 3: _spu_Fr_, _spu_t, _spu_Fw,
   _spu_Fr, _spu_FsetRXX, _spu_FsetRXXa, _spu_FgetRXXa, _spu_FsetPCR,
   func_8012AB08, func_8012AB30, _spu_Fw1ts. The first split had it in
   main_ao.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012A57C */
INCLUDE_ASM("asm/nonmatchings/main", _spu_Fr_);

/* func_8012A624 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_t);

/* func_8012A8A4 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_Fw);

/* func_8012A928 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_Fr);

extern u16 *D_80134E8C;
extern s32 D_80134EB4;

/* func_8012A98C */
void _spu_FsetRXX(s32 idx, u32 val, s32 mode) {
    if (mode == 0) {
        D_80134E8C[idx] = (u16) val;
    } else {
        D_80134E8C[idx] = (u16) (val >> D_80134EB4);
    }
}

/* func_8012A9D0 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_FsetRXXa);

/* func_8012AA74 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_FgetRXXa);

/* func_8012AAB0 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_FsetPCR);

INCLUDE_ASM("asm/nonmatchings/main", func_8012AB08);

INCLUDE_ASM("asm/nonmatchings/main", func_8012AB30);

/* func_8012AB58 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_Fw1ts);
