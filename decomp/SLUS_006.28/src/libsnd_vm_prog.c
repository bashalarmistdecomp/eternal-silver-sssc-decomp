/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_prog.o: _SsVmSetProgVol, _SsVmGetProgVol,
   _SsVmSetProgPan, _SsVmGetProgPan. The first split had it in
   psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern ProgAtr *D_800EA6D4;
s32 _SsVmVSetUp(s16 vabId, s16 prog);

/* cc97_lib: _SsVmSetProgVol */
/* return_slot_fill: _SsVmSetProgVol */

/* libsnd _SsVmSetProgVol: set the master volume of program prog in VAB
   vabId.  _SsVmVSetUp makes the VAB current; when it
   refuses, the result is -1.  Otherwise _svm_pg[prog].mvol = vol, and the
   member is read back as the result. */
/* func_8009C658 */
s32 _SsVmSetProgVol(s16 vabId, s16 prog, u8 vol)
{
    if (_SsVmVSetUp(vabId, prog) != 0) {
        return -1;
    }
    D_800EA6D4[prog].mvol = vol;
    return D_800EA6D4[prog].mvol;
}
/* cc97_lib: _SsVmGetProgVol */
/* return_slot_fill: _SsVmGetProgVol */

/* libsnd _SsVmGetProgVol: the master volume of program prog in VAB vabId
   (_svm_pg[prog].mvol), or -1 when _SsVmVSetUp refuses the pair. */
/* func_8009C6C8 */
s32 _SsVmGetProgVol(s16 vabId, s16 prog)
{
    if (_SsVmVSetUp(vabId, prog) != 0) {
        return -1;
    }
    return D_800EA6D4[prog].mvol;
}
/* cc97_lib: _SsVmSetProgPan */
/* return_slot_fill: _SsVmSetProgPan */

/* libsnd _SsVmSetProgPan: set the pan of program prog in VAB vabId:
   _svm_pg[prog].mpan = pan, read back as the result; -1 when _SsVmVSetUp
   refuses the pair. */
/* func_8009C718 */
s32 _SsVmSetProgPan(s16 vabId, s16 prog, u8 pan)
{
    if (_SsVmVSetUp(vabId, prog) != 0) {
        return -1;
    }
    D_800EA6D4[prog].mpan = pan;
    return D_800EA6D4[prog].mpan;
}
/* cc97_lib: _SsVmGetProgPan */
/* return_slot_fill: _SsVmGetProgPan */

/* libsnd _SsVmGetProgPan: the pan of program prog in VAB vabId
   (_svm_pg[prog].mpan), or -1 when _SsVmVSetUp refuses the pair. */
/* func_8009C788 */
s32 _SsVmGetProgPan(s16 vabId, s16 prog)
{
    if (_SsVmVSetUp(vabId, prog) != 0) {
        return -1;
    }
    return D_800EA6D4[prog].mpan;
}
