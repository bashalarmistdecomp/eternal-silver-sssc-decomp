/* PSY-Q 4.5 libsnd, member vm_aloc2.o: _SsVmDoAllocate. The first split had
   it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SvmVoice D_800E9EC0[];  /* _svm_voice[24] */
extern SvmCur D_800EA6F0;
/* cc97_lib: _SsVmDoAllocate */
/* gnu_as_loads: _SsVmDoAllocate */
/* return_slot_fill: _SsVmDoAllocate */
extern u8 D_800EA400[];
extern u16 D_800EA420[];     /* _svm_sreg_buf[24 * 8] */
extern s32 D_800EA5A8[];
extern s16 D_800EA688;
extern ProgAtr *D_800EA6D4;  /* _svm_pg */
extern VagAtr *D_800EA6E0;   /* _svm_tn */

/* _SsVmDoAllocate: set up the SPU register shadow of the voice just
   allocated for the note in _svm_cur.  The voice's unk6 becomes 0x7FFF and
   its bit is cleared in all 16 words of _svm_envx_hist.  Then, in the
   voice's 8 shadow registers (_svm_sreg_buf[voice * 8 + n]): 3 = the start
   address of the tone's VAG, from the VAB's address table; 4 = the tone's
   ADSR1; 5 = its ADSR2 with the release rate (the low 5 bits) raised by
   _svm_damper and held at 31.  _svm_sreg_dirty[voice] gets bits 3, 4, 5. */
/* func_8009B498 */
void _SsVmDoAllocate(void)
{
    s32 i;
    s16 pos;
    s32 idx;
    u16 adsr2;
    s16 rr;

    pos = D_800EA6F0.voice << 3;
    D_800E9EC0[D_800EA6F0.voice].envx = 0x7FFF;
    for (i = 0; i < 16; i++) {
        D_800EA5A8[i] &= ~(1 << D_800EA6F0.voice);
    }
    if ((D_800EA6F0.vag & 1) > 0) {
        D_800EA420[pos + 3] = AS(SvmVagAddrs, D_800EA6D4[(D_800EA6F0.vag - 1) / 2].reserved2).odd;
        D_800EA400[D_800EA6F0.voice] |= 8;
    } else {
        D_800EA420[pos + 3] = AS(SvmVagAddrs, D_800EA6D4[(D_800EA6F0.vag - 1) / 2].reserved2).even;
        D_800EA400[D_800EA6F0.voice] |= 8;
    }
    idx = D_800EA6F0.fake_prog * 16 + D_800EA6F0.tone;
    D_800EA420[pos + 4] = D_800EA6E0[idx].adsr1;
    adsr2 = D_800EA6E0[idx].adsr2;
    rr = (adsr2 & 0x1F) + D_800EA688;
    adsr2 &= 0xFFE0;
    if (rr >= 0x20) {
        rr = 0x1F;
    }
    rr |= adsr2;
    D_800EA420[pos + 5] = rr;
    D_800EA400[D_800EA6F0.voice] |= 0x30;
}

INCLUDE_PAD(8);
