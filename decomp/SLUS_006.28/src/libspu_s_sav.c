/* SPDX-License-Identifier: MIT
   Parts of this file match code of psy-q-decomp (src/spu/s_sav.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_sav.o: _SpuSetAnyVoice. The first split had it
   in psyq_misc_ad.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _SpuSetAnyVoice */
/* far_syms: D_800B7B9C */
/* far_syms: D_800B7B68 */
/* far_syms: D_800B7BB0 */
extern s32 D_800B7B9C;              /* _spu_env: bit 0 = SPU_ENV_EVENT_QUEUEING */
extern volatile s32 D_800B7B68;     /* _spu_RQmask: which queued register pairs are dirty */
extern volatile u16 D_800EA8C0[]; /* _spu_RQ: the queued SPU words 0xC4.. (key on, key off, pitch LFO, noise, reverb; two words each) */
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* _spu_RXX -> 0x1F801C00 */

/* libspu _SpuSetAnyVoice (S_SAV.OBJ) */
/* func_8009F0C8 */
u32 _SpuSetAnyVoice(s32 on_off, u32 voice_bit, s32 addr1, s32 addr2) {
    u32 ret;

    if (D_800B7B9C & 1) {
        ret = ((D_800EA8C0[addr2 - 0xC4] & 0xFF) << 16) | D_800EA8C0[addr1 - 0xC4];
    } else {
        ret = ((D_800B7BB0->raw[addr2] & 0xFF) << 16) | D_800B7BB0->raw[addr1];
    }
    switch (on_off) {
    case 1:
        if (D_800B7B9C & 1) {
            D_800EA8C0[addr1 - 0xC4] |= voice_bit;
            D_800EA8C0[addr2 - 0xC4] |= (voice_bit >> 16) & 0xFF;
            D_800B7B68 |= 1 << ((addr1 - 0xC6) >> 1);
        } else {
            D_800B7BB0->raw[addr1] |= voice_bit;
            D_800B7BB0->raw[addr2] |= (voice_bit >> 16) & 0xFF;
        }
        ret |= voice_bit & 0xFFFFFF;
        break;
    case 0:
        if (D_800B7B9C & 1) {
            D_800EA8C0[addr1 - 0xC4] &= ~voice_bit;
            D_800EA8C0[addr2 - 0xC4] &= ~((voice_bit >> 16) & 0xFF);
            D_800B7B68 |= 1 << ((addr1 - 0xC6) >> 1);
        } else {
            D_800B7BB0->raw[addr1] &= ~voice_bit;
            D_800B7BB0->raw[addr2] &= ~((voice_bit >> 16) & 0xFF);
        }
        ret &= ~(voice_bit & 0xFFFFFF);
        break;
    case 8:
        if (D_800B7B9C & 1) {
            D_800EA8C0[addr1 - 0xC4] = voice_bit;
            D_800EA8C0[addr2 - 0xC4] = (voice_bit >> 16) & 0xFF;
            D_800B7B68 |= 1 << ((addr1 - 0xC6) >> 1);
        } else {
            D_800B7BB0->raw[addr1] = voice_bit;
            D_800B7BB0->raw[addr2] = (voice_bit >> 16) & 0xFF;
        }
        ret = voice_bit & 0xFFFFFF;
        break;
    }
    return ret & 0xFFFFFF;
}
INCLUDE_PAD(4);
