/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member cc_121.o: _SsContResetAll. The first split had
   it in psyq_math.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* libsnd's score record (0xB0 bytes, one per sequence: _ss_score[seq_no][sep_no]); the
   members the SEQ control-change handlers of this TU use. */

/* The nine shorts a tone's two ADSR words are split into (ccadsr.o's record:
   _SsUtResolveADSR fills it, _SsUtBuildADSR packs it back).  _SsUtBuildADSR reads the
   five rates unsigned and the four mode flags signed. */

extern SsScore *D_800E9628[];                    /* _ss_score */
s32 _SsReadDeltaValue(s16 seq_no, s16 sep_no);
void _SsVmDamperOff(void);

/* cc97_lib: _SsContResetAll */
/* gnu_as_loads: _SsContResetAll */
/* return_slot_fill: _SsContResetAll */

/* libsnd _SsContResetAll(seq_no, sep_no): SEQ control change 0x79 (reset all controllers); cc_121.o.
   SsUtReverbOff, _SsVmDamperOff, then on
   score = &_ss_score[seq_no][sep_no] (D_800E9628), for the current channel (re-read from
   the score for each store): programs[channel] = channel, rpn_lsb = rpn_msb = 0,
   vol[channel] = 0x7F, panpot[channel] = 0x40, delta_value = _SsReadDeltaValue
   .  The recorded size ends in two pad nops, hence INCLUDE_PAD(8). */
/* func_800968B8 */
void _SsContResetAll(s16 seq_no, s16 sep_no)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    SsUtReverbOff();
    _SsVmDamperOff();
    score->programs[score->channel] = score->channel;
    score->rpn_lsb = 0;
    score->rpn_msb = 0;
    score->vol[score->channel] = 0x7F;
    score->panpot[score->channel] = 0x40;
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(8);
