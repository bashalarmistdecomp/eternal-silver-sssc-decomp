/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member midibend.o: _SsSetPitchBend. The first split had
   it in psyq_math_c.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

INCLUDE_PAD(12);
/* cc97_lib: _SsSetPitchBend */
/* gnu_as_loads: _SsSetPitchBend */
/* return_slot_fill: _SsSetPitchBend */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  /* _ss_score */
s32 _SsReadDeltaValue(s16, s16);
s32 _SsVmPitchBend(s16, s16, s16, s16);

/* libsnd _SsSetPitchBend(seq_no, sep_no): the SEQ reader's pitch-bend event (MIDI status
   0xE0; midibend.o). score = &_ss_score[seq_no][sep_no] (D_800E9628, 0xB0-byte records),
   ch = the current channel: takes one data byte from the read pointer, _SsVmPitchBend
   (seq_no | sep_no << 8, vab_id, programs[ch], data), then
   delta_value = _SsReadDeltaValue. */
/* func_80097598 */
void _SsSetPitchBend(s16 seq_no, s16 sep_no)
{
    SsScore *score;
    u8 ch;
    u8 data;

    score = &D_800E9628[seq_no][sep_no];
    ch = score->channel;
    data = *score->read_pos++;
    _SsVmPitchBend(seq_no | (sep_no << 8), score->vab_id, score->programs[ch], data);
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}
