/* PSY-Q 4.5 libspu, member s_sva.o: SpuSetVoiceAttr. The first split had it
   in psyq_misc_aj.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */
/* far_syms: D_800B7BB0 */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        
/* cc97_lib: SpuSetVoiceAttr */
/* return_slot_fill: SpuSetVoiceAttr */
/* far_syms: D_800B7B6C D_800B7BB0 */
extern u16 D_800B7B6C[];
unsigned short _spu_note2pitch(long, long, long, long);
long _spu_FsetRXXa(long, unsigned long);

/* libspu SpuSetVoiceAttr (S_SVA.OBJ): write each attribute the mask selects (mask 0 =
   all) to every voice set in arg->voice. The voice's eight register words are
   _spu_RXX's raw[voice * 8 ...]; D_800B7B6C is _spu_voice_centerNote[24];
   _spu_note2pitch, _spu_FsetRXXa. The function ends in
   the library's short delay loop on two volatile locals.
   MARKED SPELLING: each ADSR read-modify-write reads the register into the u16 local
   `rr` first (sotn-decomp's s_sva.c does the same with its `adsr`); written on the
   register directly, `raw[n] = (raw[n] & m) | v`, it is 362/382. The original construct
   is not known. */
/* func_800A0588 */
void SpuSetVoiceAttr(SpuVoiceAttr *arg)
{
    long voice;
    long ch;
    unsigned long mask;
    long all;
    unsigned long vol;
    unsigned long mode;
    unsigned long amode;
    u16 rr;

    mask = arg->mask;
    all = mask == 0;
    for (voice = 0; voice < 24; voice++) {
        if ((arg->voice & (1 << voice)) == 0) {
            continue;
        }
        ch = voice * 8;
        if (all || (mask & 0x10)) {
            D_800B7BB0->raw[ch + 2] = arg->pitch;
        }
        if (all || (mask & 0x40)) {
            D_800B7B6C[voice] = arg->sample_note;
        }
        if (all || (mask & 0x20)) {
            D_800B7BB0->raw[ch + 2] = _spu_note2pitch(D_800B7B6C[voice] >> 8, D_800B7B6C[voice] & 0xFF,
                                               arg->note >> 8, arg->note & 0xFF);
        }
        if (all || (mask & 0x1)) {
            vol = arg->volume.left & 0x7FFF;
            mode = 0;
            if (all || (mask & 0x4)) {
                switch (arg->volmode.left) {
                case 1:
                    mode = 0x8000;
                    break;
                case 2:
                    mode = 0x9000;
                    break;
                case 3:
                    mode = 0xA000;
                    break;
                case 4:
                    mode = 0xB000;
                    break;
                case 5:
                    mode = 0xC000;
                    break;
                case 6:
                    mode = 0xD000;
                    break;
                case 7:
                    mode = 0xE000;
                    break;
                }
            }
            if (mode) {
                if (arg->volume.left >= 0x80) {
                    vol = 0x7F;
                } else if (arg->volume.left < 0) {
                    vol = 0;
                }
            }
            D_800B7BB0->raw[ch] = vol | mode;
        }
        if (all || (mask & 0x2)) {
            vol = arg->volume.right & 0x7FFF;
            mode = 0;
            if (all || (mask & 0x8)) {
                switch (arg->volmode.right) {
                case 1:
                    mode = 0x8000;
                    break;
                case 2:
                    mode = 0x9000;
                    break;
                case 3:
                    mode = 0xA000;
                    break;
                case 4:
                    mode = 0xB000;
                    break;
                case 5:
                    mode = 0xC000;
                    break;
                case 6:
                    mode = 0xD000;
                    break;
                case 7:
                    mode = 0xE000;
                    break;
                }
            }
            if (mode) {
                if (arg->volume.right >= 0x80) {
                    vol = 0x7F;
                } else if (arg->volume.right < 0) {
                    vol = 0;
                }
            }
            D_800B7BB0->raw[ch + 1] = vol | mode;
        }
        if (all || (mask & 0x80)) {
            _spu_FsetRXXa(ch | 3, arg->addr);
        }
        if (all || (mask & 0x10000)) {
            _spu_FsetRXXa(ch | 7, arg->loop_addr);
        }
        if (all || (mask & 0x20000)) {
            D_800B7BB0->raw[ch + 4] = arg->adsr1;
        }
        if (all || (mask & 0x40000)) {
            D_800B7BB0->raw[ch + 5] = arg->adsr2;
        }
        if (all || (mask & 0x800)) {
            vol = arg->ar;
            if (vol >= 0x80) {
                vol = 0x7F;
            }
            amode = 0;
            if (all || (mask & 0x100)) {
                if (arg->a_mode == 5) {
                    amode = 0x80;
                }
            }
            rr = D_800B7BB0->raw[ch + 4];
            D_800B7BB0->raw[ch + 4] = (rr & 0xFF) | ((vol | amode) << 8);
        }
        if (all || (mask & 0x1000)) {
            vol = arg->dr;
            if (vol >= 0x10) {
                vol = 0xF;
            }
            rr = D_800B7BB0->raw[ch + 4];
            D_800B7BB0->raw[ch + 4] = (rr & 0xFF0F) | (vol << 4);
        }
        if (all || (mask & 0x2000)) {
            vol = arg->sr;
            if (vol >= 0x80) {
                vol = 0x7F;
            }
            amode = 0x100;
            if (all || (mask & 0x200)) {
                switch (arg->s_mode) {
                case 1:
                    amode = 0;
                    break;
                case 5:
                    amode = 0x200;
                    break;
                case 7:
                    amode = 0x300;
                    break;
                }
            }
            rr = D_800B7BB0->raw[ch + 5];
            D_800B7BB0->raw[ch + 5] = (rr & 0x3F) | ((vol | amode) << 6);
        }
        if (all || (mask & 0x4000)) {
            vol = arg->rr;
            if (vol >= 0x20) {
                vol = 0x1F;
            }
            amode = 0;
            if (all || (mask & 0x400)) {
                switch (arg->r_mode) {
                case 3:
                    amode = 0;
                    break;
                case 7:
                    amode = 0x20;
                    break;
                }
            }
            rr = D_800B7BB0->raw[ch + 5];
            D_800B7BB0->raw[ch + 5] = (rr & 0xFFC0) | (vol | amode);
        }
        if (all || (mask & 0x8000)) {
            vol = arg->sl;
            if (vol >= 0x10) {
                vol = 0xF;
            }
            rr = D_800B7BB0->raw[ch + 4];
            D_800B7BB0->raw[ch + 4] = (rr & 0xFFF0) | vol;
        }
    }
    {
        volatile long i;
        volatile long j;

        j = 1;
        for (i = 0; i < 2; i = i + 1) {
            j = j * 13;
        }
    }
}

INCLUDE_PAD(8);
