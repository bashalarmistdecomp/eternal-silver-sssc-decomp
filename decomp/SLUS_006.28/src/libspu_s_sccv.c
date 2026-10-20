/* PSY-Q 4.5 libspu, member s_sccv.o: SpuSetCommonCDVolume. The first split
   had it in psyq_misc_aj.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

/* cc97_lib: SpuSetCommonCDVolume */
/* return_slot_fill: SpuSetCommonCDVolume */
/* libspu SpuSetCommonCDVolume(left, right) (S_SCCV.OBJ): the CD input volume registers
   (0x1B0 / 0x1B2). */
/* func_800A0DC8 */
void SpuSetCommonCDVolume(short cd_left, short cd_right)
{
    D_800B7BB0->rxx.cd_vol.left = cd_left;
    D_800B7BB0->rxx.cd_vol.right = cd_right;
}

INCLUDE_PAD(8);
