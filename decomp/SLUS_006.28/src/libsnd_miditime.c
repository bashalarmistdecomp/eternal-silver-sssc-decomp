/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c) and psyz (decomp/src/libsnd/miditime.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member miditime.o: _SsReadDeltaValue. The first split
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

/* cc97_lib: _SsReadDeltaValue */
/* gnu_as_loads: _SsReadDeltaValue */

/* libsnd _SsReadDeltaValue(seq_no, sep_no) (MIDITIME.OBJ; instruction-identical to the 4.6
   and 4.7 library objects). score = &_ss_score[seq_no][sep_no] (D_800E9628, 0xB0-byte
   records). Reads one MIDI variable-length quantity from the score's read pointer
   (advanced past it): a zero first byte returns 0; otherwise 7 bits per byte while bit 7
   is set, and the value times 10 is added to unk88 (+0x88, the running sum of the delta
   times read) and returned. The recorded size ends in three pad nops, hence
   INCLUDE_PAD(12). */
/* func_80097B98 */
s32 _SsReadDeltaValue(s16 seq_no, s16 sep_no)
{
    SsScore *score;
    s32 d;
    s32 ret;
    u8 c;

    score = &D_800E9628[seq_no][sep_no];
    d = *score->read_pos++;
    if (d == 0) {
        return 0;
    }
    if (d & 0x80) {
        d &= 0x7F;
        do {
            c = *score->read_pos++;
            d = (d << 7) + (c & 0x7F);
        } while (c & 0x80);
    }
    ret = d * 10;
    score->delta_sum += ret;
    return ret;
}

INCLUDE_PAD(12);
