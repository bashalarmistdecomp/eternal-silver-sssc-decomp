/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/vmanager.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member vm_pb.o: _SsVmPBVoice, _SsVmPitchBend. The first
   split had it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SvmVoice D_800E9EC0[];  /* _svm_voice[24] */
extern s8 D_800EA6E4;       /* _SsVmMaxVoice */
extern SvmCur D_800EA6F0;
u16 note2pitch2(u16 note, u16 fine);
extern u8 D_800EA400[];
extern u16 D_800EA420[];
extern VagAtr *D_800EA6E0;

/* cc97_lib: _SsVmPBVoice */
/* gnu_as_loads: _SsVmPBVoice */
/* return_slot_fill: _SsVmPBVoice */

/* _SsVmPBVoice: apply a pitch-bend value (0..127, 64 = centre) to one SPU
   voice.  Does nothing and returns 0 unless the voice is playing the given
   seq/sep, VAB and program.  Otherwise the bend is scaled by the playing
   tone's bend range (VagAtr.pbmax semitones upward for values above 64,
   pbmin downward below it) into whole notes added to the voice's key-on
   note plus a fine part in 1/128 notes; _svm_cur gets the voice and its
   tone, note2pitch2 turns note and fine part into the SPU
   pitch, which goes to the voice's shadow pitch register
   (_svm_sreg_buf[voice * 8 + 2]) with dirty bit 2, and the result is 1.
   MARKED SPELLING: the three loads are separate statements and the
   fake_prog load has a local of its own that is shifted in place (`fake
   <<= 4; tone += fake;`).  The plain `tone = voice.tone; tone +=
   _svm_cur.fake_prog * 16;` is 121/123 and sotn-decomp's one-statement
   sum 120/123: the three loads come out in another order.  The original
   construct is not known. */
/* func_8009C388 */
s16 _SsVmPBVoice(s16 voice, s16 seq_sep_no, s16 vabId, s16 prog, u16 pbend)
{
    s16 bend;
    u16 tone;
    u16 note;
    u16 fine;
    s32 fake;

    bend = pbend - 64;
    if (D_800E9EC0[voice].seq_sep == seq_sep_no
        && D_800E9EC0[voice].vab_id == vabId
        && D_800E9EC0[voice].pg == prog) {
        fake = D_800EA6F0.fake_prog;
        tone = D_800E9EC0[voice].tone;
        note = D_800E9EC0[voice].note;
        fake <<= 4;
        tone += fake;
        if (bend > 0) {
            note += bend * D_800EA6E0[tone].pbmax / 63;
            fine = (bend * D_800EA6E0[tone].pbmax % 63) * 2;
        } else if (bend < 0) {
            note = note + bend * D_800EA6E0[tone].pbmin / 64 - 1;
            fine = (bend * D_800EA6E0[tone].pbmin % 64) * 2 + 127;
        } else {
            fine = 0;
        }
        D_800EA6F0.tone = D_800E9EC0[voice].tone;
        D_800EA6F0.voice = voice;
        D_800EA420[voice * 8 + 2] = note2pitch2(note, fine);
        D_800EA400[voice] |= 4;
        return 1;
    }
    return 0;
}
/* cc97_lib: _SsVmPitchBend */
/* gnu_as_loads: _SsVmPitchBend */
/* return_slot_fill: _SsVmPitchBend */
s32 _SsVmVSetUp(s16 vabId, s16 prog);  /* TU defines this below */

/* libsnd _SsVmPitchBend (VM_PB.OBJ): apply a pitch-bend value (0..127, 64 =
   centre) to every voice that plays program prog of VAB vabId for score
   seq_sep_no (seq | sep << 8).  _SsVmVSetUp makes the VAB's
   tables current, _svm_cur.seq_sep takes the score, and _SsVmPBVoice
    is tried on each voice below
   _SsVmMaxVoice (D_800EA6E4).  Returns the number of voices bent. */
/* func_8009C574 */
s32 _SsVmPitchBend(s16 seq_sep_no, s16 vabId, s16 prog, s16 pitch)
{
    s16 voice;
    s32 count;

    _SsVmVSetUp(vabId, prog);
    D_800EA6F0.seq_sep = seq_sep_no;
    count = 0;
    for (voice = 0; voice < D_800EA6E4; voice++) {
        count += _SsVmPBVoice(voice, seq_sep_no, vabId, prog, pitch);
    }
    return count;
}

INCLUDE_PAD(8);
