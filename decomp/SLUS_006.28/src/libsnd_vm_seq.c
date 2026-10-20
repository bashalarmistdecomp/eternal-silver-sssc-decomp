/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c) and psyz (decomp/src/libsnd/vm_seq.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_seq.o: _SsVmSetSeqVol, _SsVmGetSeqVol,
   _SsVmGetSeqLVol, _SsVmGetSeqRVol, _SsVmSeqKeyOff. The first split had it
   in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s16 D_800EA6D0;       /* _svm_stereo_mono */
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SsScore *D_800E9628[];  /* _ss_score */
extern u32 D_800B79C0;      /* _snd_vmask */
extern SvmVoice D_800E9EC0[];  /* _svm_voice[24] */
extern s8 D_800EA6E4;       /* _SsVmMaxVoice */
extern SvmCur D_800EA6F0;
extern VabHdr *D_800EA6DC;
extern u8 D_800EA400[];
extern u16 D_800EA420[];
extern ProgAtr *D_800EA6D4;
extern VagAtr *D_800EA6E0;
s32 _SsVmVSetUp(s16 vabId, s16 prog);

/* cc97_lib: _SsVmSetSeqVol */
/* gnu_as_loads: _SsVmSetSeqVol */
/* return_slot_fill: _SsVmSetSeqVol */
/* far_syms: D_800B79C0 (target reaches it %hi/%lo) */

/* libsnd _SsVmSetSeqVol (VM_SEQ.OBJ): set a score's volume (score =
   _ss_score[n & 0xFF] + (n >> 8)), clamp both channels to 127, and
   recompute the SPU volume of every voice that score owns: unmasked, same
   seq/sep, same VAB.  For each such voice: _SsVmVSetUp(vab, prog) selects
   the VAB's prog/tone tables, then vol = voice vol * channel vol / 127 *
   0x3FFF * VAB master / 16129 * prog master * tone vol / 16129, split by
   the score's L/R volumes, panned by the tone, prog and voice pans (< 64
   scales R, else L by (127 - pan) / 63), forced equal (the louder) in
   mono, squared / 16383 into _svm_sreg_buf[voice] L/R, and the voice
   marked dirty (bits 0, 1). */
/* func_8009C7D8 */
short _SsVmSetSeqVol(short seq_sep_num, short voll, short volr)
{
    SsScore *score;
    short voice;
    int vol;
    int vm;
    unsigned int vl;
    unsigned int vr;
    u16 l;
    u16 r;
    u8 pv;

    score = &D_800E9628[seq_sep_num & 0xFF][(seq_sep_num & 0xFF00) >> 8];
    score->voll = voll;
    score->volr = volr;
    if (score->voll >= 127) {
        score->voll = 127;
    }
    if (score->volr >= 127) {
        score->volr = 127;
    }
    for (voice = 0; voice < D_800EA6E4; voice++) {
        if ((D_800B79C0 & (1 << voice)) == 0
            && D_800E9EC0[voice].seq_sep == seq_sep_num
            && D_800E9EC0[voice].vab_id == score->vab_id) {
            _SsVmVSetUp(D_800E9EC0[voice].vab_id, D_800E9EC0[voice].prog);
            vol = (D_800E9EC0[voice].vol * score->vol[D_800E9EC0[voice].channel]) / 127;
            vm = vol * 0x3FFF;
            vl = (D_800EA6DC->mvol * vm) / 16129;
            vl = (vl * D_800EA6D4[D_800E9EC0[voice].pg].mvol
                  * D_800EA6E0[D_800E9EC0[voice].prog * 16 + D_800E9EC0[voice].tone].vol) / 16129;
            vr = vl;
            vl = (vl * score->voll) / 127;
            vr = (vr * score->volr) / 127;
            pv = D_800EA6E0[D_800E9EC0[voice].prog * 16 + D_800E9EC0[voice].tone].pan;
            if (pv < 64) {
                l = vl;
                r = (vr * pv) / 63;
            } else {
                l = (vl * (127 - pv)) / 63;
                r = vr;
            }
            pv = D_800EA6D4[D_800E9EC0[voice].pg].mpan;
            if (pv < 64) {
                r = (r * pv) / 63;
            } else {
                l = (l * (127 - pv)) / 63;
            }
            pv = D_800E9EC0[voice].pan;
            if (pv < 64) {
                r = (r * pv) / 63;
            } else {
                l = (l * (127 - pv)) / 63;
            }
            if (D_800EA6D0 == 1) {
                if (l < r) {
                    l = r;
                } else {
                    r = l;
                }
            }
            l = (l * l) / 16383;
            r = (r * r) / 16383;
            D_800EA420[voice * 8 + 0] = l;
            D_800EA420[voice * 8 + 1] = r;
            D_800EA400[voice] |= 3;
        }
    }
    return seq_sep_num;
}

/* cc97_lib: _SsVmGetSeqVol */
/* gnu_as_loads: _SsVmGetSeqVol */

/* libsnd _SsVmGetSeqVol (VM_SEQ.OBJ): record seq_sep_num as the current
   access number (_svm_cur.seq_sep) and write the left and right volume of
   the score it names (_ss_score[n & 0xFF] + (n >> 8)) to *voll and *volr.
   Returns the access number, read back from _svm_cur. */
/* func_8009CD44 */
s16 _SsVmGetSeqVol(s16 seq_sep_num, s16 *voll, s16 *volr)
{
    SsScore *score;

    score = &D_800E9628[seq_sep_num & 0xFF][(seq_sep_num & 0xFF00) >> 8];
    D_800EA6F0.seq_sep = seq_sep_num;
    *voll = score->voll;
    *volr = score->volr;
    return D_800EA6F0.seq_sep;
}
/* cc97_lib: _SsVmGetSeqLVol */
/* gnu_as_loads: _SsVmGetSeqLVol */

/* libsnd _SsVmGetSeqLVol (VM_SEQ.OBJ): record seq_sep_num as the current
   access number (_svm_cur.seq_sep) and return the left volume of the
   score it names (_ss_score[n & 0xFF] + (n >> 8)). */
/* func_8009CDA8 */
s16 _SsVmGetSeqLVol(s16 seq_sep_num)
{
    SsScore *score;

    score = &D_800E9628[seq_sep_num & 0xFF][(seq_sep_num & 0xFF00) >> 8];
    D_800EA6F0.seq_sep = seq_sep_num;
    return score->voll;
}
/* cc97_lib: _SsVmGetSeqRVol */
/* gnu_as_loads: _SsVmGetSeqRVol */

/* libsnd _SsVmGetSeqRVol (VM_SEQ.OBJ): record seq_sep_num as the current
   access number (_svm_cur.seq_sep) and return the right volume of the
   score it names (_ss_score[n & 0xFF] + (n >> 8)). */
/* func_8009CDF0 */
s16 _SsVmGetSeqRVol(s16 seq_sep_num)
{
    SsScore *score;

    score = &D_800E9628[seq_sep_num & 0xFF][(seq_sep_num & 0xFF00) >> 8];
    D_800EA6F0.seq_sep = seq_sep_num;
    return score->volr;
}
/* cc97_lib: _SsVmSeqKeyOff */
/* gnu_as_loads: _SsVmSeqKeyOff */
/* return_slot_fill: _SsVmSeqKeyOff */

/* libsnd _SsVmSeqKeyOff (VM_SEQ.OBJ): key off at once every voice that plays
   score seq_sep_num (seq | sep << 8).  Voices masked in _snd_vmask
   (D_800B79C0) are skipped; for each other voice below _SsVmMaxVoice
   (D_800EA6E4) whose record names that score (_svm_voice[i].seq_sep),
   _svm_cur.voice = i and _SsVmKeyOffNow(0) is called.
   Callers: _SsClose, _SsSndPause, _SsSeqGetEof (twice) and _SsSndStop. */
/* func_8009CE38 */
void _SsVmSeqKeyOff(s16 seq_sep_num)
{
    u8 i;

    for (i = 0; i < D_800EA6E4; i++) {
        if ((D_800B79C0 & (1 << i)) == 0 && D_800E9EC0[i].seq_sep == seq_sep_num) {
            D_800EA6F0.voice = i;
            _SsVmKeyOffNow(0);
        }
    }
}
