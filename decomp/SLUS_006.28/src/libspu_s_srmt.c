/* SPDX-License-Identifier: MIT
   Parts of this file match code of psy-q-decomp (src/spu/s_srmt.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_srmt.o: SpuSetReverbModeType. The first split
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

/* cc97_lib: SpuSetReverbModeType */
/* return_slot_fill: SpuSetReverbModeType */
/* far_syms: D_800B7B4C */
extern s32 D_800B7B4C;              /* _spu_rev_offsetaddr */
extern SpuReverbAttr _spu_rev_attr;
extern s32 D_800B8140[];            /* _spu_rev_startaddr[10] */
extern SpuRevParam D_800B8170[];    /* _spu_rev_param[10] */
s32 _SpuIsInAllocateArea_(u32);
void _spu_setReverbAttr(SpuRevParam *entry);
void _spu_FsetRXX(s32 offset, u32 value, s32 mode);

static __inline__ void _spu_srmt_memcpy(char *dst, char *src, u32 size)
{
    while (size--) {
        *dst++ = *src++;
    }
}
/* libspu SpuSetReverbModeType (S_SRMT.OBJ) */
/* func_800A0E68 */
long SpuSetReverbModeType(long mode)
{
    SpuRevParam entry;
    s32 rev_was_on;
    u16 cnt;
    s32 clear_wa = 0;

    if (mode & 0x100) {
        mode &= ~0x100;
        clear_wa = 1;
    }
    if ((u32) mode >= 10 || _SpuIsInAllocateArea_(D_800B8140[mode])) {
        return -1;
    }
    _spu_rev_attr.mode = mode;
    D_800B7B4C = D_800B8140[mode];
    _spu_srmt_memcpy((char *)&entry, (char *)&D_800B8170[mode], sizeof(SpuRevParam));
    entry.flags = 0;
    switch (mode) {
    case 7:
        _spu_rev_attr.feedback = 0x7F;
        _spu_rev_attr.delay = 0x7F;
        break;
    case 8:
        _spu_rev_attr.feedback = 0;
        _spu_rev_attr.delay = 0x7F;
        break;
    default:
        _spu_rev_attr.feedback = 0;
        _spu_rev_attr.delay = 0;
        break;
    }
    rev_was_on = (D_800B7BB0->rxx.spucnt & 0x80) != 0;
    if (rev_was_on) {
        cnt = D_800B7BB0->rxx.spucnt;
        cnt &= ~0x80;
        D_800B7BB0->rxx.spucnt = cnt;
    }
    D_800B7BB0->rxx.rev_vol.left = 0;
    D_800B7BB0->rxx.rev_vol.right = 0;
    _spu_rev_attr.depth.left = 0;
    _spu_rev_attr.depth.right = 0;
    _spu_setReverbAttr(&entry);
    if (clear_wa) {
        SpuClearReverbWorkArea(mode);
    }
    _spu_FsetRXX(0xD1, D_800B7B4C, 0);
    if (rev_was_on) {
        cnt = D_800B7BB0->rxx.spucnt;
        cnt |= 0x80;
        D_800B7BB0->rxx.spucnt = cnt;
    }
    return 0;
}

INCLUDE_PAD(4);
