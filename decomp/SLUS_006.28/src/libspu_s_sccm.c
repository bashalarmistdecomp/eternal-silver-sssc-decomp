/* PSY-Q 4.5 libspu, member s_sccm.o: SpuSetCommonCDMix. The first split had
   it in psyq_misc_aj.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

/* cc97_lib: SpuSetCommonCDMix */
/* return_slot_fill: SpuSetCommonCDMix */

/* libspu SpuSetCommonCDMix(cd_mix) (S_SCCM.OBJ): SPUCNT bit 0, the CD input mixed into
   the output, off (SPU_OFF, 0) or on. */
/* func_800A0E28 */
void SpuSetCommonCDMix(long cd_mix)
{
    if (cd_mix == 0) {
        D_800B7BB0->rxx.spucnt &= ~1;
    } else {
        D_800B7BB0->rxx.spucnt |= 1;
    }
}
