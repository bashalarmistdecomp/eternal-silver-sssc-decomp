/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_nowon.o: _SsVmKeyOnNow. The first split had
   it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s16 D_800EA6D0;       /* _svm_stereo_mono */
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SsScore *D_800E9628[];  /* _ss_score */
extern SvmVoice D_800E9EC0[];  /* _svm_voice[24] */
extern SvmCur D_800EA6F0;
extern u16 D_800E9EB0;
extern u16 D_800E9EB2;
extern u16 D_800E9EB4;
extern u16 D_800E9EB6;
extern u16 D_800E9EB8;
extern u16 D_800E9EBA;
extern u16 D_800EA5E8;
extern u16 D_800EA5EA;
extern VabHdr *D_800EA6DC;
extern u8 D_800EA400[];
extern u16 D_800EA420[];

/* cc97_lib: _SsVmKeyOnNow */
/* gnu_as_loads: _SsVmKeyOnNow */
/* return_slot_fill: _SsVmKeyOnNow */

/* _SsVmKeyOnNow: start the note described by _svm_cur on the voice it was
   allocated (_svm_cur.voice) at the given SPU pitch.  Volume = note vol *
   VAB master * 0x3FFF / 16129 * prog master * tone vol / 16129, split by
   the score's L/R volumes unless the note is a direct key-on (seq_sep
   0x21), panned by the tone, the program and the note pans (< 64 scales R
   by pan / 63, else L by (127 - pan) / 63), forced equal (the louder) in
   mono, and squared / 16383 (again not for a direct key-on).  The voice's
   shadow registers take L, R and the pitch (_svm_sreg_buf[voice * 8 + 0,
   1, 2], dirty bits 0-2) and the voice record remembers the pitch.  Then
   the voice's bit in the pending SPU masks: reverb on or off by the
   tone's mode bit 2 (_svm_orev), noise off (_svm_onos), key-on set
   (_svm_okon), and every pending key-on cleared from key-off (_svm_okof). */
/* func_8009BEB8 */
void _SsVmKeyOnNow(short vagCount, short pitch)
{
    SsScore *score;
    u16 pos;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll, volr;
    u32 voll_t, volr_t;
    s32 mvol_scaled;
    u8 pv;

    pos = D_800EA6F0.voice * 8;
    score = &D_800E9628[D_800EA6F0.seq_sep & 0xFF][(D_800EA6F0.seq_sep & 0xFF00) >> 8];
    mvol_scaled = D_800EA6DC->mvol * 0x3FFF;
    voll_t = (D_800EA6F0.voll * mvol_scaled) / 16129;
    voll_t = (voll_t * D_800EA6F0.mvol * D_800EA6F0.vol) / 16129;
    volr_t = voll_t;
    if (D_800EA6F0.seq_sep != 0x21) {
        voll_t = (voll_t * score->voll) / 127;
        volr_t = (volr_t * score->volr) / 127;
    }
    pv = D_800EA6F0.pan;
    if (pv < 64) {
        voll = voll_t;
        volr = (volr_t * pv) / 63;
    } else {
        voll = (voll_t * (127 - pv)) / 63;
        volr = volr_t;
    }
    pv = D_800EA6F0.mpan;
    if (pv < 64) {
        volr = (volr * pv) / 63;
    } else {
        voll = (voll * (127 - pv)) / 63;
    }
    pv = D_800EA6F0.note_pan;
    if (pv < 64) {
        volr = (volr * pv) / 63;
    } else {
        voll = (voll * (127 - pv)) / 63;
    }
    if (D_800EA6D0 == 1) {
        if (voll < volr) {
            voll = volr;
        } else {
            volr = voll;
        }
    }
    if (D_800EA6F0.seq_sep != 0x21) {
        voll = (voll * voll) / 16383;
        volr = (volr * volr) / 16383;
    }
    D_800EA420[pos + 2] = pitch;
    D_800EA420[pos + 0] = voll;
    D_800EA420[pos + 1] = volr;
    D_800EA400[D_800EA6F0.voice] |= 7;
    D_800E9EC0[D_800EA6F0.voice].pitch = pitch;
    if (D_800EA6F0.voice < 16) {
        bitsLower = 1 << D_800EA6F0.voice;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (D_800EA6F0.voice - 16);
    }
    if (D_800EA6F0.mode & 4) {
        D_800E9EB4 |= bitsLower;
        D_800E9EB6 |= bitsUpper;
    } else {
        D_800E9EB4 &= ~bitsLower;
        D_800E9EB6 &= ~bitsUpper;
    }
    D_800E9EB8 &= ~bitsLower;
    D_800E9EBA &= ~bitsUpper;
    D_800E9EB0 |= bitsLower;
    D_800E9EB2 |= bitsUpper;
    D_800EA5E8 &= ~D_800E9EB0;
    D_800EA5EA &= ~D_800E9EB2;
}

INCLUDE_PAD(8);
