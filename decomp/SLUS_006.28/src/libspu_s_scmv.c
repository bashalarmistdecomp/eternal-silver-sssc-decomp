/* PSY-Q 4.5 libspu, member s_scmv.o: SpuSetCommonMasterVolume. The first
   split had it in psyq_misc_aj.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

/* cc97_lib: SpuSetCommonMasterVolume */
/* return_slot_fill: SpuSetCommonMasterVolume */
/* libspu SpuSetCommonMasterVolume(left, right) (S_SCMV.OBJ): the main volume registers
   (0x180 / 0x182) = each value's low 15 bits (direct mode). */
/* func_800A0DA8 */
void SpuSetCommonMasterVolume(short mvol_left, short mvol_right)
{
    D_800B7BB0->rxx.main_vol.left = mvol_left & 0x7FFF;
    D_800B7BB0->rxx.main_vol.right = mvol_right & 0x7FFF;
}

INCLUDE_PAD(4);
