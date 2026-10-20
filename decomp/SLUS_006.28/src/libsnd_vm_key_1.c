/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_key.o, part 1 of 2: _SsVmKeyOn, _SsVmKeyOff.
   The first split had it in psyq_misc_q.c. The tree compiles this run of
   the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern u16 D_800EA708;
extern VagAtr *D_800EA6E0;    /* _svm_tn: the current VAB's tone table */
s32 _SsVmVSetUp(s16, s16);
extern SvmVoice D_800E9EC0[];
extern s8 D_800EA6E4;
extern void (*D_800EA418)(s32);
extern void (*D_800EA5A0)(s32);
void _SsVmKeyOffNow();

/* cc97_lib: _SsVmKeyOn */
/* gnu_as_loads: _SsVmKeyOn */
/* return_slot_fill: _SsVmKeyOn */
extern s32 D_800E9628[];                /* _ss_score */
extern s32 D_800EA6D4;                  /* _svm_pg */
extern s32 D_800EA6DC;                  /* _svm_vh */
extern SvmCur D_800EA6F0;               /* _svm_cur */
s32 _SsVmKeyOff(s16 seq_sep, s16 vab, s16 prog, u16 note);  /* TU defines this below */
u8 _SsVmAlloc(s32);
void _SsVmDoAllocate(void);
s32 note2pitch(void);
void vmNoiseOn(u8);
void _SsVmKeyOnNow();
u8 _SsVmSelectToneAndVag(u8 *, u8 *);

/* libsnd _SsVmKeyOn: key a note on for score seq_sep (seq | sep << 8, or
   0x21 for a direct key-on from SsUtKeyOn) with program prog of VAB vabId.
   _SsVmVSetUp makes the VAB current (else -1).  _svm_cur takes the score,
   the note, the volume (voll scaled by the score's volume for the MIDI
   channel being read, / 127; unscaled for a direct key-on), the pan and
   the program's master volume, pan and tone count; a program index at or
   past the VAB header's program count is -1.  voll == 0 is a note off:
   the result is _SsVmKeyOff's.  Otherwise _SsVmSelectToneAndVag lists the
   program's tones that cover the note, and for each: _svm_cur takes the
   tone's VAG number and its priority, volume, pan, centre, shift and mode;
   _SsVmAlloc picks a voice; the voice record is filled (key state 1, age
   0, score, VAB, program index, program, volume and MIDI channel unless a
   direct key-on, pan, note volume, tone, note, priority, VAG number);
   _SsVmDoAllocate sets up its shadow registers; and the voice starts as
   the noise voice (VAG 0xFF: vmNoiseOn) or at the note's pitch
   (_SsVmKeyOnNow(count, note2pitch())).  Returns the mask of the voices
   keyed on, or -1 once any tone found no voice. */
/* func_8009AA78 */
s32 _SsVmKeyOn(s16 seq_sep, s16 vabId, s16 prog, u16 note, u16 voll, u16 pan)
{
    u8 vagIdx[0x80];
    u8 toneIdx[0x80];
    SsScore *score;
    u8 i;
    s32 ret;
    u16 tone;
    u8 vagCount;

    score = (SsScore *)D_800E9628[seq_sep & 0xFF] + ((seq_sep & 0xFF00) >> 8);
    ret = 0;
    if (_SsVmVSetUp(vabId, prog)) {
        return -1;
    }
    D_800EA6F0.seq_sep = seq_sep;
    D_800EA6F0.note = note;
    D_800EA6F0.unk_03 = 0;
    if (seq_sep == 0x21) {
        D_800EA6F0.voll = voll;
    } else {
        D_800EA6F0.voll = (voll * score->vol[score->channel]) / 127;
    }
    D_800EA6F0.note_pan = pan;
    D_800EA6F0.mvol = ((ProgAtr *)D_800EA6D4)[prog].mvol;
    D_800EA6F0.mpan = ((ProgAtr *)D_800EA6D4)[prog].mpan;
    D_800EA6F0.tones = ((ProgAtr *)D_800EA6D4)[prog].tones;
    if (D_800EA6F0.fake_prog >= ((VabHdr *)D_800EA6DC)->ps) {
        return -1;
    }
    if (voll == 0) {
        ret = _SsVmKeyOff(seq_sep, vabId, prog, note);
    } else {
        vagCount = _SsVmSelectToneAndVag(toneIdx, vagIdx);
        for (i = 0; i < vagCount; i++) {
            D_800EA6F0.vag = vagIdx[i];
            D_800EA6F0.tone = toneIdx[i];
            tone = D_800EA6F0.tone + D_800EA6F0.fake_prog * 16;
            D_800EA6F0.prior = D_800EA6E0[tone].prior;
            D_800EA6F0.vol = D_800EA6E0[tone].vol;
            D_800EA6F0.pan = D_800EA6E0[tone].pan;
            D_800EA6F0.centre = D_800EA6E0[tone].center;
            D_800EA6F0.shift = D_800EA6E0[tone].shift;
            D_800EA6F0.mode = D_800EA6E0[tone].mode;
            D_800EA6F0.voice = _SsVmAlloc(0);
            if (D_800EA6F0.voice < D_800EA6E4) {
                D_800E9EC0[D_800EA6F0.voice].key_stat = 1;
                D_800E9EC0[D_800EA6F0.voice].age = 0;
                D_800E9EC0[D_800EA6F0.voice].seq_sep = seq_sep;
                D_800E9EC0[D_800EA6F0.voice].vab_id = D_800EA6F0.vab_id;
                D_800E9EC0[D_800EA6F0.voice].prog = D_800EA6F0.fake_prog;
                D_800E9EC0[D_800EA6F0.voice].pg = prog;
                if (seq_sep != 0x21) {
                    D_800E9EC0[D_800EA6F0.voice].vol = voll;
                    D_800E9EC0[D_800EA6F0.voice].channel = score->channel;
                }
                D_800E9EC0[D_800EA6F0.voice].pan = pan;
                D_800E9EC0[D_800EA6F0.voice].note_vol = D_800EA6F0.voll;
                D_800E9EC0[D_800EA6F0.voice].tone = D_800EA6F0.tone;
                D_800E9EC0[D_800EA6F0.voice].note = note;
                D_800E9EC0[D_800EA6F0.voice].prior = D_800EA6F0.prior;
                D_800E9EC0[D_800EA6F0.voice].vag = D_800EA6F0.vag;
                _SsVmDoAllocate();
                if (D_800EA6F0.vag == 0xFF) {
                    vmNoiseOn(D_800EA6F0.voice);
                } else {
                    _SsVmKeyOnNow(vagCount, note2pitch() & 0xFFFF);
                }
                ret |= 1 << D_800EA6F0.voice;
            } else {
                ret = -1;
            }
        }
    }
    return ret;
}

/* far_syms: D_800B79C0 (target reaches it %hi/%lo) */
extern u32 D_800B79C0;
void vmNoiseOff(u8);
void _SsVmKeyOffNow();

/* cc97_lib: _SsVmKeyOff */
/* gnu_as_loads: _SsVmKeyOff */
/* return_slot_fill: _SsVmKeyOff */
/* libsnd _SsVmKeyOff(seq_sep, vab, prog, note) (VM_KEY.OBJ): key off every managed voice
   that plays this note of this program, VAB and score and whose bit in _snd_vmask
   (D_800B79C0) is clear: the noise voice (VAG number 0xFF) through vmNoiseOff, any
   other by making it the current voice (D_800EA708) and calling _SsVmKeyOffNow.
   Returns the number of voices keyed off. */
/* func_8009AF94 */
s32 _SsVmKeyOff(s16 seq_sep, s16 vab, s16 prog, u16 note)
{
    u8 voice;
    s32 n;

    voice = 0;
    n = 0;
    if (D_800EA6E4 > 0) {
        do {
            if ((D_800B79C0 & (1 << voice)) == 0
                && D_800E9EC0[voice].note == note
                && D_800E9EC0[voice].pg == prog
                && D_800E9EC0[voice].seq_sep == seq_sep
                && D_800E9EC0[voice].vab_id == vab) {
                if (D_800E9EC0[voice].vag == 0xFF) {
                    vmNoiseOff(voice);
                } else {
                    D_800EA708 = voice;
                    _SsVmKeyOffNow(0);
                }
                n++;
            }
            voice++;
        } while (voice < D_800EA6E4);
    }
    return n;
}
