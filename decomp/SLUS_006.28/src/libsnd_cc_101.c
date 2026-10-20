/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member cc_101.o: _SsContRpn2. The first split had it in
   psyq_math.c.
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

/* cc97_lib: _SsContRpn2 */
/* gnu_as_loads: _SsContRpn2 */
/* return_slot_fill: _SsContRpn2 */

/* libsnd _SsContRpn2(seq_no, sep_no, data): SEQ control change 0x65 (RPN, second byte); cc_101.o.
   score = &_ss_score[seq_no][sep_no] (D_800E9628): rpn_msb = data, rpn_count += 1,
   delta_value = _SsReadDeltaValue.  The recorded size ends in a pad nop,
   hence INCLUDE_PAD(4). */
/* func_80096848 */
void _SsContRpn2(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    score->rpn_msb = data;
    score->rpn_count++;
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(4);
