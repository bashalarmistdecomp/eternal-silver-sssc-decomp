/* SPDX-License-Identifier: MIT
   Parts of this file match code of psy-q-decomp (src/spu/s_srmd.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_srmd.o: SpuSetReverbModeDepth. The first split
   had it in psyq_misc_aj.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* far_syms: D_800B7BB0 */
/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */
extern SpuReverbAttr _spu_rev_attr;
static __inline__ void _spu_srmt_memcpy(char *dst, char *src, u32 size)
{
    while (size--) {
        *dst++ = *src++;
    }
}

/* cc97_lib: SpuSetReverbModeDepth */
/* return_slot_fill: SpuSetReverbModeDepth */

/* libspu SpuSetReverbModeDepth(left, right) (S_SRMD.OBJ): the reverb depth registers
   (0x184 / 0x186) and _spu_rev_attr.depth. */
/* func_800A1038 */
void SpuSetReverbModeDepth(short depth_left, short depth_right)
{
    D_800B7BB0->rxx.rev_vol.left = depth_left;
    D_800B7BB0->rxx.rev_vol.right = depth_right;
    _spu_rev_attr.depth.left = depth_left;
    _spu_rev_attr.depth.right = depth_right;
}
INCLUDE_PAD(8);
