/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member midiprog.o: _SsSetProgramChange. The first split
   had it in psyq_math_e.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _SsSetProgramChange */
/* gnu_as_loads: _SsSetProgramChange */
/* return_slot_fill: _SsSetProgramChange */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
s32 _SsReadDeltaValue(s16, s16);

/* libsnd _SsSetProgramChange(seq_no, sep_no, prog): SEQ status 0xC0; midiprog.o.
   score = &_ss_score[seq_no][sep_no] (D_800E9628, 0xB0-byte records):
   programs[channel] = prog, delta_value = _SsReadDeltaValue. The recorded size ends in a pad nop, hence INCLUDE_PAD(4). */
/* func_80097B28 */
void _SsSetProgramChange(s16 seq_no, s16 sep_no, u8 prog)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    score->programs[score->channel] = prog;
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(4);
