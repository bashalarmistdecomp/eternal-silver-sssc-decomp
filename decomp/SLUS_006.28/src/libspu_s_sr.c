/* PSY-Q 4.5 libspu, member s_sr.o: SpuSetReverb. The first split had it in
   psyq_misc_af.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7B4C */
/* far_syms: D_800B7BB0 */
/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
s32 _SpuIsInAllocateArea_(u32);
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's with the reverb registers named. */
extern SpuUnion *D_800B7BB0;

/* far_syms: D_800B7B44 (target reaches it %hi/%lo) */
/* far_syms: D_800B7B48 (target reaches it %hi/%lo) */
/* far_syms: D_800B7B4C (target reaches it %hi/%lo) */
/* far_syms: D_800B7B44 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800B7B48 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800B7B4C (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 D_800B7B44;              /* _spu_rev_flag */
extern s32 D_800B7B48;              /* _spu_rev_reserve_wa */
extern s32 D_800B7B4C;              /* _spu_rev_offsetaddr */

/* cc97_lib: SpuSetReverb */
/* return_slot_fill: SpuSetReverb */

/* libspu SpuSetReverb(on_off) (S_SR.OBJ): switch reverb off (0) or on (1) in SPUCNT
   bit 7 and in _spu_rev_flag, which it returns. Switching on is refused (the reverb is
   switched off) when the work area is not reserved (_spu_rev_reserve_wa is not 1) and
   overlaps SpuMalloc'd memory (_SpuIsInAllocateArea_ of _spu_rev_offsetaddr). Any other
   value changes nothing. The C follows the image's instructions: the
   dispatch is a two-case switch; each arm stores the flag and changes the one bit of
   the register in place, and the compiler folds the three register writes into one.
   The reserve flag is compared with the argument, as the image compares two registers
   there, and the refusal comes first, as its instructions do. */
/* func_8009F438 */
long SpuSetReverb(long on_off)
{
    switch (on_off) {
    case 0:
        D_800B7B44 = 0;
        D_800B7BB0->rxx.spucnt &= ~0x80;
        break;
    case 1:
        if (D_800B7B48 != on_off && _SpuIsInAllocateArea_(D_800B7B4C)) {
            D_800B7B44 = 0;
            D_800B7BB0->rxx.spucnt &= ~0x80;
        } else {
            D_800B7B44 = on_off;
            D_800B7BB0->rxx.spucnt |= 0x80;
        }
        break;
    }
    return D_800B7B44;
}

INCLUDE_PAD(4);
