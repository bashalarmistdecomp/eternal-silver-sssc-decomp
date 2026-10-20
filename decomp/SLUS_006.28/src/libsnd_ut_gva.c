/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ut_gva.c), psyz (decomp/src/libsnd/ut_gva.c) and
   psy-q-decomp (src/snd/ut_gva.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ut_gva.o: SsUtGetVagAtr. The first split had it
   in psyq_misc_m.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
extern u8 D_800EA710[];       /* _svm_vab_used: 1 = slot holds an opened VAB */
s32 _SsVmVSetUp(s16, s16);

/* cc97_lib: SsUtGetVagAtr */
/* gnu_as_loads: SsUtGetVagAtr */
/* return_slot_fill: SsUtGetVagAtr */
extern VagAtr *D_800EA6E0;    /* _svm_tn: the current VAB's tone table */
extern s8 D_800EA6F7;         /* _svm_cur + 7: the current program's index among the used programs */

/* libsnd SsUtGetVagAtr: copy tone toneNum of program progNum in VAB vabId
   out to the caller's VagAtr.  The VAB must be open (_svm_vab_used == 1),
   else -1.  _SsVmVSetUp selects the VAB's tables; the tone lives at
   _svm_tn[toneNum + 16 * the program's index among the used programs].
   Every field is copied except the reserved ones (max before min, as the
   library does); the result is 0. */
/* func_80099C28 */
s16 SsUtGetVagAtr(s16 vabId, s16 progNum, s16 toneNum, VagAtr *vagatrptr)
{
    s16 tone;

    if (D_800EA710[vabId] == 1) {
        _SsVmVSetUp(vabId, progNum);
        tone = toneNum + D_800EA6F7 * 16;
        vagatrptr->prior = D_800EA6E0[tone].prior;
        vagatrptr->mode = D_800EA6E0[tone].mode;
        vagatrptr->vol = D_800EA6E0[tone].vol;
        vagatrptr->pan = D_800EA6E0[tone].pan;
        vagatrptr->center = D_800EA6E0[tone].center;
        vagatrptr->shift = D_800EA6E0[tone].shift;
        vagatrptr->max = D_800EA6E0[tone].max;
        vagatrptr->min = D_800EA6E0[tone].min;
        vagatrptr->vibW = D_800EA6E0[tone].vibW;
        vagatrptr->vibT = D_800EA6E0[tone].vibT;
        vagatrptr->porW = D_800EA6E0[tone].porW;
        vagatrptr->porT = D_800EA6E0[tone].porT;
        vagatrptr->pbmin = D_800EA6E0[tone].pbmin;
        vagatrptr->pbmax = D_800EA6E0[tone].pbmax;
        vagatrptr->adsr1 = D_800EA6E0[tone].adsr1;
        vagatrptr->adsr2 = D_800EA6E0[tone].adsr2;
        vagatrptr->prog = D_800EA6E0[tone].prog;
        vagatrptr->vag = D_800EA6E0[tone].vag;
        return 0;
    }
    return -1;
}
INCLUDE_PAD(8);
