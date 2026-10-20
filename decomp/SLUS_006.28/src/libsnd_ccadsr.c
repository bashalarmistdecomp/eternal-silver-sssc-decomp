/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libsnd/adsr.c)
   and psy-q-decomp (src/snd/adsr.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ccadsr.o: _SsUtResolveADSR, _SsUtBuildADSR. The
   first split had it in psyq_math.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* The nine shorts a tone's two ADSR words are split into (ccadsr.o's record:
   _SsUtResolveADSR fills it, _SsUtBuildADSR packs it back).  _SsUtBuildADSR reads the
   five rates unsigned and the four mode flags signed. */

void _SsUtResolveADSR(u16 adsr1, u16 adsr2, SsAdsr *adsr);      /* _SsUtResolveADSR; TU defines this below */
void _SsUtBuildADSR(SsAdsr *adsr, u16 *adsr1, u16 *adsr2);

/* cc97_lib: _SsUtResolveADSR */

/* libsnd _SsUtResolveADSR(adsr1, adsr2, adsr) (ccadsr.o): split a tone's two ADSR words
   (VagAtr adsr1 / adsr2) into the nine shorts of *adsr.  The four mode flags keep their
   bit where it is (0 or the mask); the five rates are shifted down. */
/* func_80096C98 */
void _SsUtResolveADSR(u16 adsr1, u16 adsr2, SsAdsr *adsr)
{
    adsr->a_mode = adsr1 & 0x8000;
    adsr->s_mode = adsr2 & 0x8000;
    adsr->s_dir = adsr2 & 0x4000;
    adsr->r_mode = adsr2 & 0x20;
    adsr->ar = (adsr1 >> 8) & 0x7F;
    adsr->dr = (adsr1 >> 4) & 0xF;
    adsr->sl = adsr1 & 0xF;
    adsr->sr = (adsr2 >> 6) & 0x7F;
    adsr->rr = adsr2 & 0x1F;
}
/* cc97_lib: _SsUtBuildADSR */

/* libsnd _SsUtBuildADSR(adsr, adsr1, adsr2) (ccadsr.o): pack the nine shorts of *adsr
   back into a tone's two ADSR words.  A mode flag is set when its short is not 0; each
   rate is shifted into place and masked to its field. */
/* func_80096CF4 */
void _SsUtBuildADSR(SsAdsr *adsr, u16 *adsr1, u16 *adsr2)
{
    u16 low;
    u16 high;

    if (adsr->s_mode != 0) {
        high = 0x8000;
    } else {
        high = 0;
    }
    if (adsr->a_mode != 0) {
        low = 0x8000;
    } else {
        low = 0;
    }
    if (adsr->s_dir != 0) {
        high |= 0x4000;
    }
    if (adsr->r_mode != 0) {
        high |= 0x20;
    }
    low = low | ((adsr->ar << 8) & 0x7F00) | ((adsr->dr << 4) & 0xF0) | (adsr->sl & 0xF);
    high = high | ((adsr->sr << 6) & 0x1FC0) | (adsr->rr & 0x1F);
    *adsr1 = low;
    *adsr2 = high;
}
