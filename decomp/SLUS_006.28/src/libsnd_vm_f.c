/* PSY-Q 4.5 libsnd, member vm_f.o: _SsVmFlush. The first split had it in
   psyq_misc_q.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
extern SvmVoice D_800E9EC0[];    /* _svm_voice */
extern s8 D_800EA6E4;            /* _SsVmMaxVoice */
extern void (*D_800EA418)(s32);  /* _autopan */
extern void (*D_800EA5A0)(s32);  /* _autovol */
/* cc97_lib: _SsVmFlush */
/* gnu_as_loads: _SsVmFlush */
/* return_slot_fill: _SsVmFlush */
extern u16 D_800E9EB0;           /* _svm_okon1: voices 0-15 to key on */
extern u16 D_800E9EB2;           /* _svm_okon2: voices 16-23 */
extern u16 D_800E9EB4;           /* _svm_orev1: reverb on, voices 0-15 */
extern u16 D_800E9EB6;           /* _svm_orev2 */
extern u16 D_800E9EB8;           /* _svm_onos1: noise on, voices 0-15 */
extern u16 D_800E9EBA;           /* _svm_onos2 */
extern u8 D_800EA400[];          /* _svm_sreg_dirty[24] */
extern u16 D_800EA420[];         /* _svm_sreg_buf[24 * 8] */
extern s32 D_800EA5A4;           /* _svm_envx_ptr */
extern s32 D_800EA5A8[];         /* _svm_envx_hist[16] */
extern u16 D_800EA5E8;           /* _svm_okof1: voices 0-15 to key off */
extern u16 D_800EA5EA;           /* _svm_okof2 */
extern s8 D_800EA720;            /* _svm_auto_kof_mode */
void SpuSetVoiceAttr(SpuVoiceAttr *);

/* libsnd _SsVmFlush: send the voice manager's pending SPU state to the
   hardware through libspu.  Runs at the start of every sound tick
   (SsSeqCalledTbyT) and at the end of _SsVmInit.
   1. Advance the 16-slot ring _svm_envx_hist and, for each of the
      _SsVmMaxVoice voices, read its SPU envelope level into
      _svm_voice[i].unk6; a silent voice sets its bit in the new slot.
   2. Unless _svm_auto_kof_mode is set: a voice whose bit is set in slots
      0-14 is retired (key state = 0); if it was the noise voice (key
      state 2) its SPU noise bit is switched off first.
   3. Voices keyed off since the last flush leave the key-on words.
   4. For all 24 voices call the installed _autovol / _autopan steppers
      on those whose auto_vol / auto_pan field is nonzero.
   5. For all 24 voices send the dirty shadow registers (_svm_sreg_buf,
      8 per voice) through SpuSetVoiceAttr: dirty bit 0 = volume L/R,
      2 = pitch, 3 = the sample start address (kept / 8), 4 = ADSR1/2;
      then clear the dirty byte.
   6. Key off, then key on (24-bit voice words), and set the reverb and
      noise bits of the managed voices (mask = the low _SsVmMaxVoice
      bits), keeping the other voices' bits as libspu reports them.
   7. Clear the key-off, key-on and noise words (the reverb words stay). */
/* func_8009A258 */
void _SsVmFlush(void)
{
    SpuVoiceAttr attr;
    s32 i;
    u32 env_mask;
    s16 lo;
    u8 hi;
    s32 mask;
    u32 bits;

    D_800EA5A4 = (D_800EA5A4 + 1) & 0xF;
    D_800EA5A8[D_800EA5A4] = 0;
    for (i = 0; i < D_800EA6E4; i++) {
        SpuGetVoiceEnvelope(i, &D_800E9EC0[i].envx);
        if (D_800E9EC0[i].envx == 0) {
            D_800EA5A8[D_800EA5A4] |= 1 << i;
        }
    }
    if (D_800EA720 == 0) {
        env_mask = 0xFFFFFFFF;
        for (i = 0; i < 15; i++) {
            env_mask &= D_800EA5A8[i];
        }
        for (i = 0; i < D_800EA6E4; i++) {
            if (env_mask & (1 << i)) {
                if (D_800E9EC0[i].key_stat == 2) {
                    if (i < 16) {
                        lo = 1 << i;
                        hi = 0;
                    } else {
                        lo = 0;
                        hi = 1 << (i - 16);
                    }
                    SpuSetNoiseVoice(0, (hi << 16) | lo);  /* SPU_OFF */
                }
                D_800E9EC0[i].key_stat = 0;
            }
        }
    }
    D_800E9EB0 &= ~D_800EA5E8;
    D_800E9EB2 &= ~D_800EA5EA;
    for (i = 0; i < 24; i++) {
        if (D_800E9EC0[i].auto_vol != 0) {
            D_800EA5A0(i);
        }
        if (D_800E9EC0[i].auto_pan != 0) {
            D_800EA418(i);
        }
    }
    for (i = 0; i < 24; i++) {
        attr.mask = 0;
        attr.voice = 1 << i;
        if (D_800EA400[i] & 1) {
            attr.mask = 3;          /* SPU_VOICE_VOLL | SPU_VOICE_VOLR */
            attr.volume.left = D_800EA420[i * 8];
            attr.volume.right = D_800EA420[i * 8 + 1];
        }
        if (D_800EA400[i] & 4) {
            attr.mask |= 0x10;      /* SPU_VOICE_PITCH */
            attr.pitch = D_800EA420[i * 8 + 2];
        }
        if (D_800EA400[i] & 8) {
            attr.mask |= 0x80;      /* SPU_VOICE_WDSA */
            attr.addr = D_800EA420[i * 8 + 3] << 3;
        }
        if (D_800EA400[i] & 0x10) {
            attr.mask |= 0x60000;   /* SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2 */
            attr.adsr1 = D_800EA420[i * 8 + 4];
            attr.adsr2 = D_800EA420[i * 8 + 5];
        }
        if (attr.mask != 0) {
            SpuSetVoiceAttr(&attr);
        }
        D_800EA400[i] = 0;
    }
    SpuSetKey(0, ((D_800EA5EA & 0xFF) << 16) | D_800EA5E8);  /* SPU_OFF */
    SpuSetKey(1, ((D_800E9EB2 & 0xFF) << 16) | D_800E9EB0);  /* SPU_ON */
    mask = 0xFFFFFF >> (24 - D_800EA6E4);
    bits = ((D_800E9EB6 << 16) | D_800E9EB4) & mask;
    SpuSetReverbVoice(8, bits | (SpuGetReverbVoice() & ~mask));  /* SPU_BIT */
    bits = ((D_800E9EBA << 16) | D_800E9EB8) & mask;
    SpuSetNoiseVoice(8, bits | (SpuGetNoiseVoice() & ~mask));  /* SPU_BIT */
    D_800EA5E8 = 0;
    D_800EA5EA = 0;
    D_800E9EB0 = 0;
    D_800E9EB2 = 0;
    D_800E9EB8 = 0;
    D_800E9EBA = 0;
}

INCLUDE_PAD(12);
