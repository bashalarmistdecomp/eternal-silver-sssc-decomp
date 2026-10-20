/* PSY-Q 4.5 libsnd, member vm_no1.o: vmNoiseOn. The first split had it in
   psyq_misc_s.c.
   The build must stay byte-matching. */

/* far_syms: D_800B79C0 (target reaches it %hi/%lo) */

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

/* cc97_lib: vmNoiseOn */
/* gnu_as_loads: vmNoiseOn */
/* return_slot_fill: vmNoiseOn */
long SpuSetNoiseClock(long n_clock);

/* vmNoiseOn: start the note described by _svm_cur as NOISE on the given
   SPU voice.  Volume as in _SsVmKeyOnNow: note vol * VAB
   master * 0x3FFF / 16129 * prog master * tone vol / 16129, split by the
   score's L/R volumes unless the note is a direct key-on (seq_sep 0x21),
   panned by the tone, the program and the note pans (< 64 scales R by
   pan / 63, else L by (127 - pan) / 63), forced equal (the louder) in
   mono, and squared / 16383 (again not for a direct key-on).  The SPU
   noise clock is set from the note: (note - the tone's centre note) & 63.
   The voice's shadow registers take L and R (_svm_sreg_buf[voice * 8 + 0,
   1], dirty bits 0-1) and its record gets unk4 = 10 and key state 2 (the
   noise voice); every other unmasked voice in key state 2 or 3 drops
   back to 0 or 1 (state &= 1), so only one voice is the noise voice.
   Then the voice's bit in the pending SPU masks: key-on set (_svm_okon),
   every pending key-on cleared from key-off (_svm_okof), reverb on or
   off by the tone's mode bit 2 (_svm_orev), and the noise words
   (_svm_onos) are set to this voice's bit alone. */
/* func_8009B848 */
void vmNoiseOn(u8 voice)
{
    SsScore *score;
    s16 i;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll, volr;
    u32 voll_t, volr_t;
    u32 temp;
    s32 mvol_scaled;
    s32 pos;

    score = &D_800E9628[D_800EA6F0.seq_sep & 0xFF][(D_800EA6F0.seq_sep & 0xFF00) >> 8];
    mvol_scaled = D_800EA6DC->mvol * 0x3FFF;
    voll_t = (D_800EA6F0.voll * mvol_scaled) / 16129;
    voll_t = (voll_t * D_800EA6F0.mvol * D_800EA6F0.vol) / 16129;
    volr_t = voll_t;
    if (D_800EA6F0.seq_sep != 0x21) {
        voll_t = (voll_t * score->voll) / 127;
        volr_t = (volr_t * score->volr) / 127;
    }
    temp = D_800EA6F0.pan;
    if (temp < 64) {
        voll = voll_t;
        volr = (volr_t * temp) / 63;
    } else {
        voll = (voll_t * (127 - temp)) / 63;
        volr = volr_t;
    }
    temp = D_800EA6F0.mpan;
    if (temp < 64) {
        volr = (volr * temp) / 63;
    } else {
        voll = (voll * (127 - temp)) / 63;
    }
    temp = D_800EA6F0.note_pan;
    if (temp < 64) {
        volr = (temp * volr) / 63;
    } else {
        voll = (voll * (127 - temp)) / 63;
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
    pos = voice * 8;
    SpuSetNoiseClock((D_800EA6F0.note - D_800EA6F0.centre) & 0x3F);
    D_800EA420[pos + 0] = voll;
    D_800EA420[pos + 1] = volr;
    D_800EA400[voice] |= 3;
    if (voice < 16) {
        bitsLower = 1 << voice;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (voice - 16);
    }
    D_800E9EC0[voice].pitch = 10;
    for (i = 0; i < D_800EA6E4; i++) {
        if ((D_800B79C0 & (1 << i)) == 0) {
            D_800E9EC0[i].key_stat &= 1;
        }
    }
    D_800E9EC0[voice].key_stat = 2;
    D_800E9EB0 |= bitsLower;
    D_800E9EB2 |= bitsUpper;
    D_800EA5E8 &= ~D_800E9EB0;
    D_800EA5EA &= ~D_800E9EB2;
    if (D_800EA6F0.mode & 4) {
        D_800E9EB4 |= bitsLower;
        D_800E9EB6 |= bitsUpper;
    } else {
        D_800E9EB4 &= ~bitsLower;
        D_800E9EB6 &= ~bitsUpper;
    }
    D_800E9EB8 = bitsLower;
    D_800E9EBA = bitsUpper;
}

INCLUDE_PAD(8);
