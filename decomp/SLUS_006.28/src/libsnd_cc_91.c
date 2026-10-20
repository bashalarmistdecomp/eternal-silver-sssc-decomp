/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member cc_91.o: _SsContExternal. The first split had it
   in psyq_math.c.
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
void SsUtSetReverbDepth(s16 ldepth, s16 rdepth);

/* cc97_lib: _SsContExternal */
/* gnu_as_loads: _SsContExternal */
/* return_slot_fill: _SsContExternal */

/* libsnd _SsContExternal(seq_no, sep_no, data) (cc_91.o): the SEQ control change that
   sets the reverb depth (0x5B in the SEQ reader): SsUtSetReverbDepth
   (data, data), then on score = &_ss_score[seq_no][sep_no] (D_800E9628) delta_value =
   _SsReadDeltaValue.  The recorded size ends in three pad nops, hence
   INCLUDE_PAD(12). */
/* func_80096508 */
void _SsContExternal(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    SsUtSetReverbDepth(data, data);
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(12);
