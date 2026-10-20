/* PSY-Q 4.5 libsnd, member vm_n2p.o: note2pitch, note2pitch2,
   SsPitchFromNote. The first split had it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SvmCur D_800EA6F0;
u16 note2pitch2(u16 note, u16 fine);
extern VagAtr *D_800EA6E0;

/* cc97_lib: note2pitch */
/* gnu_as_loads: note2pitch */
u16 SsPitchFromNote(s16 note, s16 fine, u8 center, u8 shift);

/* libsnd note2pitch (VM_N2P.OBJ): the SPU pitch of the note being set up:
   SsPitchFromNote(_svm_cur.note, 0, _svm_cur.centre, _svm_cur.shift), with a
   fine tune above 127 taken as 127. */
/* func_8009B668 */
s32 note2pitch(void)
{
    u8 shift;

    shift = D_800EA6F0.shift;
    if (shift >= 0x80) {
        shift = 0x7F;
    }
    return SsPitchFromNote(D_800EA6F0.note, 0, D_800EA6F0.centre, shift);
}
/* cc97_lib: note2pitch2 */
/* gnu_as_loads: note2pitch2 */

/* libsnd note2pitch2 (VM_N2P.OBJ): the SPU pitch of note + fine (in 1/128
   notes) on the tone _svm_cur points at: SsPitchFromNote(note, fine, the
   tone's centre note, the tone's fine tune), the tone being
   _svm_tn[_svm_cur.fake_prog * 16 + _svm_cur.tone].  _SsVmPBVoice calls it
   with the bent note after storing the voice's tone in _svm_cur.tone. */
/* func_8009B6B4 */
u16 note2pitch2(u16 note, u16 fine)
{
    s16 tone;

    tone = D_800EA6F0.fake_prog * 16 + D_800EA6F0.tone;
    return SsPitchFromNote(note, fine, D_800EA6E0[tone].center, D_800EA6E0[tone].shift);
}
/* cc97_lib: SsPitchFromNote */
/* gnu_as_loads: SsPitchFromNote */
extern u16 D_800B7A10[];  /* vm_n2p.o's own .data: 12 semitone ratios, 0x8000 * 2^(k/12) */
extern u16 D_800B7A28[];  /* vm_n2p.o's own .data + 0x18: 128 fine ratios, 0x8000 * 2^(k/1536) */

/* libsnd SsPitchFromNote (VM_N2P.OBJ): the SPU pitch (0x1000 = the sample's
   own rate) that plays a tone recorded at note `center`, fine tune `shift`,
   as note `note` plus `fine` 1/128ths of a semitone.
   f = fine + shift; n = note - center + f / 128; f %= 128, brought into
   0..127 by borrowing a semitone when it is negative.  n splits into an
   octave (n / 12 - 2) and a semitone 0..11 (a negative remainder borrows an
   octave).  The pitch is the product of the two 0.15 fixed-point ratios
   (semitone table x fine table) >> 16, which is 0x4000 at ratio 1.0; it is
   then shifted right by -octave with rounding.  From two octaves above the
   centre (octave >= 0) the result is the SPU's maximum, 0x3FFF. */
/* func_8009B71C */
u16 SsPitchFromNote(s16 note, s16 fine, u8 center, u8 shift)
{
    s16 n;
    s16 f;
    s16 octave;
    s16 semi;
    u32 pitch;

    f = fine + shift;
    n = note + f / 128 - center;
    f %= 128;
    if (f < 0) {
        f += 128;
        n--;
        n += f / 128;
    }
    octave = n / 12 - 2;
    semi = n % 12;
    if (semi < 0) {
        semi += 12;
        octave--;
    }
    pitch = (D_800B7A10[semi] * D_800B7A28[f]) >> 16;
    if (octave >= 0) {
        pitch = 0x3FFF;
    } else {
        pitch += 1 << (-octave - 1);
        pitch >>= -octave;
    }
    return pitch;
}

INCLUDE_PAD(12);
