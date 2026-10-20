/* SPDX-License-Identifier: MIT
   Parts of this file match code of psy-q-decomp (src/snd/ssinit.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ssinit.o: _SsInit. The first split had it in
   psyq_spu_n.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
void _SsInit();                                  /* extern */
/* cc97_lib: _SsInit */
extern s16 D_800B79CC[16];                      /* SSINIT.OBJ .data+0xC: reset values of the 16 SPU common registers from 0x1F801D80 (0x3FFF, 0x3FFF, 0 ...) */
extern SsMarkCallbackProc D_800E96A8[32][16];   /* _SsMarkCallback */
extern s32 D_800E9624;                          /* VBLANK_MINUS */
extern u32 D_800E9620;                          /* _snd_openflag */
extern s32 D_800E961C;                          /* _snd_ev_flag */
void _SsVmInit(s8 maxVoice);

/* libsnd _SsInit (SSINIT.OBJ): the sound system's reset, called by SsInit
    after SpuInit.  Writes the sixteen SPU common registers
   from 0x1F801D80 (main volume left / right 0x3FFF, the rest 0: reverb
   volume, key on / off, pitch-modulation, noise and reverb masks ...) from
   the object's table, resets the voice manager for 24 voices (_SsVmInit),
   clears the 32 x 16 mark callbacks (_SsMarkCallback), sets VBLANK_MINUS to
   60 and clears _snd_openflag (no access number open) and _snd_ev_flag (no
   tick running). */
/* func_800959D8 */
void _SsInit(void)
{
    s16 *ptr;
    int i, j;

    ptr = (s16 *)0x1F801D80;
    for (i = 0; i < 16; i++) {
        *ptr++ = D_800B79CC[i];
    }

    _SsVmInit(24);
    for (j = 0; j < 32; j++) {
        for (i = 0; i < 16; i++) {
            D_800E96A8[j][i] = 0;
        }
    }

    D_800E9624 = 60;
    D_800E9620 = 0;
    D_800E961C = 0;
}
