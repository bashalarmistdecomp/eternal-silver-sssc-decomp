/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_srmp.c), psyz (decomp/src/libspu/s_srmp.c) and
   psy-q-decomp (src/spu/s_srmp.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_srmp.o: SpuSetReverbModeParam. The first split
   had it in psyq_misc_af.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
s32 _SpuIsInAllocateArea_(u32);
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's with the reverb registers named. */
extern SpuUnion *D_800B7BB0;
/* far_syms: D_800B7B4C (target reaches it %hi/%lo) */
/* far_syms: D_800B7B4C (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 D_800B7B4C;

/* cc97_lib: SpuSetReverbModeParam */
/* gnu_as_loads: SpuSetReverbModeParam */
/* return_slot_fill: SpuSetReverbModeParam */
/* reorder_fill: SpuSetReverbModeParam */
/* far_syms: D_800B7B4C */
/* far_syms: D_800B7BB0 */
extern SpuReverbAttr _spu_rev_attr;
extern s32 D_800B8140[];            /* _spu_rev_startaddr[10] */
extern SpuRevParam D_800B8170[];    /* the ten presets, one per reverb mode */
void _spu_setReverbAttr(SpuRevParam *entry);  /* TU defines this below */
void _spu_FsetRXX(s32 offset, u32 value, s32 mode);

static __inline__ void _spu_srmp_memcpy(char *dst, char *src, u32 size)
{
    while (size--) {
        *dst++ = *src++;
    }
}
/* libspu SpuSetReverbModeParam(attr) (S_SRMP.OBJ). Sets the reverb mode, depth, delay time
   and feedback named by attr->mask (0 = all). A new mode (SPU_REV_MODE, optionally
   | SPU_REV_MODE_CLEAR_WA 0x100) must be 0-9 and its work area (_spu_rev_startaddr[mode])
   must not overlap SpuMalloc'd memory, else -1; it loads the mode's preset, resets
   delay/feedback to the preset's (ECHO 7: 0x7F/0x7F, DELAY 8: 0x7F/0), zeroes the reverb
   depth in the SPU and in _spu_rev_attr, and switches reverb off in SPUCNT (bit 7) around
   the register writes and the work-area start address (register 0xD1). SPU_REV_DELAYTIME
   and SPU_REV_FEEDBACK only act in the ECHO and DELAY modes: they rescale the preset's
   delay registers by delay * 0x1000 / 127 (mLSAME by twice that) and set
   vWALL = feedback * 0x8100 / 127. SPU_REV_DEPTHL/R write the depth unless the mode
   changed. Returns 0. */
/* func_8009F618 */
long SpuSetReverbModeParam(SpuReverbAttr *attr)
{
    SpuRevParam entry;
    u32 mode;
    u16 cnt;
    s32 rev_was_on = 0;
    s32 set_mode = 0;
    s32 set_delay = 0;
    s32 clear_wa = 0;
    s32 set_feedback = 0;
    u32 mask = attr->mask;
    s32 all = attr->mask == 0;

    entry.flags = 0;
    if (all || mask & 1) {
        mode = attr->mode;
        if (attr->mode & 0x100) {
            mode &= ~0x100;
            clear_wa = 1;
        }
        if (mode >= 10 || _SpuIsInAllocateArea_(D_800B8140[mode])) {
            return -1;
        }
        set_mode = 1;
        _spu_rev_attr.mode = mode;
        D_800B7B4C = D_800B8140[_spu_rev_attr.mode];
        _spu_srmp_memcpy((char *)&entry, (char *)&D_800B8170[_spu_rev_attr.mode],
                         sizeof(SpuRevParam));
        switch (_spu_rev_attr.mode) {
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
    }
    if (all || mask & 8) {
        switch (_spu_rev_attr.mode) {
        case 7:
        case 8:
            set_delay = 1;
            if (!set_mode) {
                _spu_srmp_memcpy((char *)&entry, (char *)&D_800B8170[_spu_rev_attr.mode],
                                 sizeof(SpuRevParam));
                entry.flags = 0x0C011C00;   /* mLSAME mRSAME mLCOMB1 dLSAME mLAPF1 mRAPF1 */
            }
            _spu_rev_attr.delay = attr->delay;
            entry.mLSAME = ((_spu_rev_attr.delay << 13) / 127) - entry.dAPF1;
            entry.mRSAME = ((_spu_rev_attr.delay << 12) / 127) - entry.dAPF2;
            entry.mLCOMB1 = ((_spu_rev_attr.delay << 12) / 127) + entry.mRCOMB1;
            entry.dLSAME = ((_spu_rev_attr.delay << 12) / 127) + entry.dRSAME;
            entry.mLAPF1 = ((_spu_rev_attr.delay << 12) / 127) + entry.mLAPF2;
            entry.mRAPF1 = ((_spu_rev_attr.delay << 12) / 127) + entry.mRAPF2;
            break;
        }
    }
    if (all || mask & 0x10) {
        switch (_spu_rev_attr.mode) {
        case 7:
        case 8:
            set_feedback = 1;
            if (!set_mode) {
                if (!set_delay) {
                    _spu_srmp_memcpy((char *)&entry, (char *)&D_800B8170[_spu_rev_attr.mode],
                                     sizeof(SpuRevParam));
                    entry.flags = 0x80;     /* vWALL */
                } else {
                    entry.flags |= 0x80;
                }
            }
            _spu_rev_attr.feedback = attr->feedback;
            entry.vWALL = (_spu_rev_attr.feedback * 0x8100) / 127;
            break;
        }
    }
    if (set_mode) {
        rev_was_on = (D_800B7BB0->rxx.spucnt >> 7) & 1;
        if (rev_was_on) {
            cnt = D_800B7BB0->rxx.spucnt;
            cnt &= ~0x80;
            D_800B7BB0->rxx.spucnt = cnt;
        }
    }
    if (!set_mode) {
        if (all || mask & 2) {
            D_800B7BB0->rxx.rev_vol.left = attr->depth.left;
            _spu_rev_attr.depth.left = attr->depth.left;
        }
        if (all || mask & 4) {
            D_800B7BB0->rxx.rev_vol.right = attr->depth.right;
            _spu_rev_attr.depth.right = attr->depth.right;
        }
    } else {
        D_800B7BB0->rxx.rev_vol.left = 0;
        D_800B7BB0->rxx.rev_vol.right = 0;
        _spu_rev_attr.depth.left = 0;
        _spu_rev_attr.depth.right = 0;
    }
    if (set_mode || set_delay || set_feedback) {
        _spu_setReverbAttr(&entry);
    }
    if (clear_wa) {
        SpuClearReverbWorkArea(_spu_rev_attr.mode);
    }
    if (set_mode) {
        _spu_FsetRXX(0xD1, D_800B7B4C, 0);
        if (rev_was_on) {
            cnt = D_800B7BB0->rxx.spucnt;
            cnt |= 0x80;
            D_800B7BB0->rxx.spucnt = cnt;
        }
    }
    return 0;
}

INCLUDE_PAD(12);
