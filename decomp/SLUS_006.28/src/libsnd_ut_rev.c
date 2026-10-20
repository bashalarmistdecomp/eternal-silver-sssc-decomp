/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ut_rev.c) and psyz (decomp/src/libsnd/ut_rev.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ut_rev.o: SsUtSetReverbType, SsUtGetReverbType.
   The first split had it in psyq_misc_o.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SsUtSetReverbType */
/* return_slot_fill: SsUtSetReverbType */
/* reorder_fill: SsUtSetReverbType */
extern SpuReverbAttr D_800EA5F0;     /* _svm_rattr: the reverb attributes libsnd hands to SpuSetReverbModeParam */
long SpuSetReverbModeParam(SpuReverbAttr *);

/* libsnd SsUtSetReverbType (UT_REV.OBJ): select the SPU reverb type.
   type is 0..9 (SS_REV_TYPE_OFF .. SS_REV_TYPE_PIPE); a negative type
   means "the same type, and clear the reverb work area" (the SPU's
   SPU_REV_MODE_CLEAR_WA bit, 0x100, is set in the mode).  Out of range
   returns -1 with nothing written.  Otherwise _svm_rattr.mask =
   SPU_REV_MODE and .mode = the type; type 0 also switches reverb off
   (SpuSetReverb(0)); SpuSetReverbModeParam(&_svm_rattr) applies it and the
   (positive) type is returned. */
/* func_80099F38 */
short SsUtSetReverbType(short type)
{
    short _type;
    int is_neg;

    is_neg = 0;
    _type = type;
    if (type < 0) {
        is_neg = 1;
        _type = -type;
    }
    if (_type >= 0 && _type < 10) {
        D_800EA5F0.mask = 1;
        if (is_neg) {
            D_800EA5F0.mode = _type | 0x100;
        } else {
            D_800EA5F0.mode = _type;
        }
        if (_type == 0) {
            SpuSetReverb(0);
        }
        SpuSetReverbModeParam(&D_800EA5F0);
        return _type;
    }
    return -1;
}

/* cc97_lib: SsUtGetReverbType */

/* libsnd SsUtGetReverbType (UT_REV.OBJ): the reverb type last set, _svm_rattr.mode. */
/* func_80099FD4 */
s16 SsUtGetReverbType(void) {
    return D_800EA5F0.mode;
}
INCLUDE_PAD(4);
