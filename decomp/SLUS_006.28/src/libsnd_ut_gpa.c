/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ut_gpa.c), psyz (decomp/src/libsnd/ut_gpa.c) and
   psy-q-decomp (src/snd/ut_gpa.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ut_gpa.o: SsUtGetProgAtr. The first split had it
   in psyq_misc_m.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
extern u8 D_800EA710[];       /* _svm_vab_used: 1 = slot holds an opened VAB */
extern ProgAtr *D_800EA6D4;   /* _svm_pg: the current VAB's program headers */
s32 _SsVmVSetUp(s16, s16);

/* cc97_lib: SsUtGetProgAtr */
/* gnu_as_loads: SsUtGetProgAtr */
/* return_slot_fill: SsUtGetProgAtr */
/* libsnd SsUtGetProgAtr: copy program progNum's header of VAB vabId out. */
/* func_80099B18 */
s16 SsUtGetProgAtr(s16 vabId, s16 progNum, ProgAtr *progatrptr)
{
    if (D_800EA710[vabId] == 1) {
        _SsVmVSetUp(vabId, progNum);
        progatrptr->tones = D_800EA6D4[progNum].tones;
        progatrptr->mvol = D_800EA6D4[progNum].mvol;
        progatrptr->prior = D_800EA6D4[progNum].prior;
        progatrptr->mode = D_800EA6D4[progNum].mode;
        progatrptr->mpan = D_800EA6D4[progNum].mpan;
        progatrptr->attr = D_800EA6D4[progNum].attr;
        return 0;
    }
    return -1;
}

INCLUDE_PAD(12);
