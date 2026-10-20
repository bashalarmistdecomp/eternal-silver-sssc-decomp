/* PSY-Q 4.5 libspu, member s_it.o: _spu_setInTransfer, _spu_getInTransfer.
   The first split had it in psyq_misc_aj.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s32 D_800B7BE4;
/* far_syms: D_800B7BE4 */

/* cc97_lib: _spu_setInTransfer */

/* libspu _spu_setInTransfer (S_IT.OBJ): _spu_inTransfer (D_800B7BE4) = 0 when a transfer
   starts (arg 1), else 1 (idle). */
/* func_800A0548 */
void _spu_setInTransfer(s32 arg0)
{
    if (arg0 == 1) {
        D_800B7BE4 = 0;
    } else {
        D_800B7BE4 = 1;
    }
}

/* far_syms: D_800B7BE4 (target reaches it %hi/%lo) */
/* far_syms: D_800B7BE4 (auto: target reaches it %hi/%lo — addressing row 3) */

/* cc97_lib: _spu_getInTransfer */
/* libspu _spu_getInTransfer (S_IT.OBJ): 1 while a transfer is running (_spu_inTransfer
   is not 1). */
/* func_800A0570 */
s32 _spu_getInTransfer(void) {
    return D_800B7BE4 != 1;
}
