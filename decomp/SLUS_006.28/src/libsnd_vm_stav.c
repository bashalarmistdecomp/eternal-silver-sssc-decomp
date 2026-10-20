/* PSY-Q 4.5 libsnd, member vm_stav.o: _SsVmSelectToneAndVag. The first
   split had it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SvmCur D_800EA6F0;
extern VagAtr *D_800EA6E0;

/* cc97_lib: _SsVmSelectToneAndVag */
/* gnu_as_loads: _SsVmSelectToneAndVag */

/* libsnd _SsVmSelectToneAndVag (VM_STAV.OBJ): list the tones of the current
   program whose key range holds the note in _svm_cur.  For each tone i below
   _svm_cur.tones with VagAtr.min <= _svm_cur.note <= VagAtr.max (the tone
   being _svm_tn[_svm_cur.fake_prog * 16 + i]), vagIdx[n] = the tone's VAG
   number (its low byte) and toneIdx[n] = i.  Returns n, the number of tones
   listed; _SsVmKeyOn then keys each of them on. */
/* func_8009CEE8 */
u8 _SsVmSelectToneAndVag(u8 *toneIdx, u8 *vagIdx)
{
    s8 i;
    u8 vagCount;

    vagCount = 0;
    for (i = 0; i < D_800EA6F0.tones; i++) {
        if (D_800EA6E0[D_800EA6F0.fake_prog * 16 + i].min <= D_800EA6F0.note
            && D_800EA6F0.note <= D_800EA6E0[D_800EA6F0.fake_prog * 16 + i].max) {
            vagIdx[vagCount] = D_800EA6E0[D_800EA6F0.fake_prog * 16 + i].vag;
            toneIdx[vagCount++] = i;
        }
    }
    return vagCount;
}

INCLUDE_PAD(12);
