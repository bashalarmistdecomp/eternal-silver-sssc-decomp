/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libspu/s_sk.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_sk.o: SpuSetKey. The first split had it in
   psyq_misc_ah.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SpuClearReverbWorkArea */
/* return_slot_fill: SpuClearReverbWorkArea */
/* cc97_lib: SpuSetKey */
/* far_syms: D_800B7B3C D_800B7B64 D_800B7B68 D_800B7B9C D_800B7BB0 */
extern s32 D_800B7B3C;                      /* _spu_keystat */
extern volatile s32 D_800B7B64;             /* _spu_RQvoice */
extern volatile s32 D_800B7B68;             /* _spu_RQmask: which queued register pairs are dirty */
extern s32 D_800B7B9C;                      /* _spu_env: bit 0 = SPU_ENV_EVENT_QUEUEING */
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        /* _spu_RXX -> 0x1F801C00 */
extern volatile u16 D_800EA8C0[];           /* _spu_RQ[10]: [0..1] key on, [2..3] key off */

/* libspu SpuSetKey (S_SK.OBJ): key the voices in voice_bit (24 bits) on
   (on_off 1, SPU_ON) or off (on_off 0, SPU_OFF); any other on_off does
   nothing.  With event queueing on (_spu_env bit 0) nothing reaches the
   hardware: the mask goes into the queue's key-on words _spu_RQ[0..1] (key
   off: [2..3]), bit 0 of _spu_RQmask is set (the one bit serves both
   pairs), _spu_RQvoice gains (loses) the bits, and each word of the
   opposite pair that has any of the bits set has them cleared.  Without
   queueing the mask is written to the SPU's KON words 0x188 / 0x18A (KOFF:
   0x18C / 0x18E) and _spu_keystat gains (loses) the bits. */
/* func_800A01D8 */
void SpuSetKey(long on_off, unsigned long voice_bit)
{
    u32 lo;
    u32 hi;

    lo = voice_bit & 0xFFFFFF;
    hi = lo >> 16;
    switch (on_off) {
    case 1:
        if (D_800B7B9C & 1) {
            D_800EA8C0[0] = lo;
            D_800EA8C0[1] = hi;
            D_800B7B68 |= 1;
            D_800B7B64 |= lo;
            if (D_800EA8C0[2] & lo) {
                D_800EA8C0[2] &= ~lo;
            }
            if (D_800EA8C0[3] & hi) {
                D_800EA8C0[3] &= ~hi;
            }
        } else {
            D_800B7BB0->rxx.key_on[0] = lo;
            D_800B7BB0->rxx.key_on[1] = hi;
            D_800B7B3C |= lo;
        }
        break;
    case 0:
        if (D_800B7B9C & 1) {
            D_800EA8C0[2] = lo;
            D_800EA8C0[3] = hi;
            D_800B7B68 |= 1;
            D_800B7B64 &= ~lo;
            if (D_800EA8C0[0] & lo) {
                D_800EA8C0[0] &= ~lo;
            }
            if (D_800EA8C0[1] & hi) {
                D_800EA8C0[1] &= ~hi;
            }
        } else {
            D_800B7BB0->rxx.key_off[0] = lo;
            D_800B7BB0->rxx.key_off[1] = hi;
            D_800B7B3C &= ~lo;
        }
        break;
    }
}

INCLUDE_PAD(4);
