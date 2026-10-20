/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_vol.o: _SsVmSetVol. The first split had it in
   psyq_misc_s.c.
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
/* far_syms: D_800B79C0 (target reaches it %hi/%lo) */

/* cc97_lib: _SsVmSetVol */
/* gnu_as_loads: _SsVmSetVol */
/* return_slot_fill: _SsVmSetVol */

/* _SsVmSetVol: a channel's volume or pan changed.  Make the VAB current,
   record the access number, and recompute the SPU volume of every voice
   that plays this score, program and VAB (voices masked in _snd_vmask are
   skipped): voice vol * vol / 127 * VAB master * 0x3FFF / 16129 * prog
   master * tone vol / 16129, split by the score's L/R volumes, panned by
   the tone, the program and the pan argument (< 64 scales R by pan / 63,
   else L by (127 - pan) / 63), forced equal (the louder) in mono, squared
   / 16383 into _svm_sreg_buf[voice] L/R, voice marked dirty (bits 0, 1).
   A vol or pan of 0 counts as 1, and a channel volume of 0 that differs
   from vol becomes 1.  Returns the number of voices updated.
   Three compensations are marked in the body: they emit nothing and
   only steer loop.c and global-alloc.  The tone record is written out
   at each use, as in _SsVmSetSeqVol, not as an integer sum. */
/* func_8009CFB8 */
int _SsVmSetVol(short seq_sep_num, short vab_id, short prog, unsigned short vol, unsigned short pan)
{
    SsScore *score;
    int count;
    short voice;
    int v;
    int vm;
    unsigned int vl;
    unsigned int vr;
    unsigned int l;
    unsigned int r;
    u8 pv;

    count = 0;
    score = &D_800E9628[seq_sep_num & 0xFF][(seq_sep_num & 0xFF00) >> 8];
    _SsVmVSetUp(vab_id, prog);
    D_800EA6F0.seq_sep = seq_sep_num;
    if (pan == 0) {
        pan = 1;
    }
    if (vol == 0) {
        vol = 1;
    }
    for (voice = 0; voice < D_800EA6E4; voice++) {
        if ((D_800B79C0 & (1 << voice)) == 0
            && D_800E9EC0[voice].seq_sep == seq_sep_num
            && D_800E9EC0[voice].pg == prog
            && D_800E9EC0[voice].vab_id == vab_id) {
            /* Dead copy of the three statements after the channel fix: with
               the signed /127 and /16129 magics seen twice loop.c hoists
               them, as the target does (original construct unknown). */
            v = (D_800E9EC0[voice].vol * vol) / 127;
            vm = D_800EA6DC->mvol * 0x3FFF;
            vl = (v * vm) / 16129;
            if (score->vol[score->channel] != vol && score->vol[score->channel] == 0) {
                score->vol[score->channel] = 1;
            }
            v = (D_800E9EC0[voice].vol * vol) / 127;
            vm = D_800EA6DC->mvol * 0x3FFF;
            vl = (v * vm) / 16129;
            vl = (vl * D_800EA6D4[prog].mvol
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
            /* The two double do-while wrappers add no code; they weight the
               references to voice and pan for global-alloc's ordering
               (original construct unknown). */
            /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
               the block runs once, so behaviour is the same without the
               `do` / `while (0)`, but then 20 of this function's 348 instruction lines
               differ. */
            do {
                /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
                   the block runs once, so behaviour is the same without
                   the `do` / `while (0)`, but then 20 of this function's 348 instruction
                   lines differ. */
                do {
                    pv = D_800EA6D4[D_800E9EC0[voice].pg].mpan;
                } while (0);
            } while (0);
            if (pv < 64) {
                r = (r * pv) / 63;
            } else {
                l = (l * (127 - pv)) / 63;
            }
            /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
               the block runs once, so behaviour is the same without the
               `do` / `while (0)`, but then 11 of this function's 348 instruction lines
               differ. */
            do {
                /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
                   the block runs once, so behaviour is the same without
                   the `do` / `while (0)`, but then 11 of this function's 348 instruction
                   lines differ. */
                do {
                    pv = pan;
                } while (0);
            } while (0);
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
            count++;
            __asm__ volatile(""); /* zero-byte barrier: one more insn for a global-alloc tie - original construct unknown */
        }
    }
    return count;
}
