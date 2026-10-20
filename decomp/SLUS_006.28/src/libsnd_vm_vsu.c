/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vm_vsu.c) and psy-q-decomp (src/snd/vm_vsu.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_vsu.o: _SsVmVSetUp. The first split had it in
   psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern u8 D_800EA710[];   /* _svm_vab_used[16] */
extern SvmCur D_800EA6F0;
extern VabHdr *D_800EA6DC;
extern ProgAtr *D_800EA6D4;
extern VagAtr *D_800EA6E0;
s32 _SsVmVSetUp(s16 vabId, s16 prog);

/* cc97_lib: _SsVmVSetUp */
/* gnu_as_loads: _SsVmVSetUp */
extern ProgAtr *D_800EA608[];  /* _svm_vab_pg[16] */
extern VabHdr *D_800EA648[];   /* _svm_vab_vh[16] */
extern VagAtr *D_800EA690[];   /* _svm_vab_tn[16] */
extern s16 D_800EA6D2;    /* kMaxPrograms */

/* _SsVmVSetUp: make VAB vabId's tables current.  Fails with -1 when vabId
   is outside 0..15, the slot holds no opened VAB (_svm_vab_used != 1) or
   prog is not below kMaxPrograms.  Otherwise _svm_vh / _svm_pg / _svm_tn
   (D_800EA6DC / D_800EA6D4 / D_800EA6E0) take that VAB's header, program
   and tone tables, _svm_cur gets the VAB id, the program number and the
   program's index among the VAB's used programs (the low byte of
   ProgAtr.reserved1), and the result is 0. */
/* func_8009D558 */
s32 _SsVmVSetUp(s16 vabId, s16 prog)
{
    if ((u16)vabId >= 16 || D_800EA710[vabId] != 1 || prog >= D_800EA6D2) {
        return -1;
    }
    D_800EA6DC = D_800EA648[vabId];
    D_800EA6D4 = D_800EA608[vabId];
    D_800EA6E0 = D_800EA690[vabId];
    D_800EA6F0.vab_id = vabId;
    D_800EA6F0.prog = prog;
    D_800EA6F0.fake_prog = D_800EA6D4[prog].reserved1;
    return 0;
}
