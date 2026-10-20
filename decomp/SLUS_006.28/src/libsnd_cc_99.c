/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member cc_99.o: _SsContNrpn2. The first split had it in
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
/* cc97_lib: _SsContNrpn2 */
/* return_slot_fill: _SsContNrpn2 */

/* libsnd _SsContNrpn2(seq_no, sep_no, data): SEQ control change 0x63 (99, NRPN second
   byte); cc_99.o.  score = &_ss_score[seq_no][sep_no] (D_800E9628, 0xB0-byte records).
   Every case stores data in nrpn_msb (+0x1B) and ends with _SsReadDeltaValue
   , whose result goes to delta_value (+0x90) except where noted.
   - data 0x14 (20), loop start: loop_armed (+0x1C) = 1, and after the delta time is read
     the loop position +0x08 = read_pos (+0x00): the first event of the loop body.
   - data 0x1E (30), loop end, by loop_count (+0x1D):
       0: loop_on (+0x15) = 0, the loop is over;
       1 to 126: loop_count -= 1; when it is still non-zero read_pos = the loop position
         (+0x08), when it reached 0 loop_on = 0;
       127 or more: an endless loop: the delta time is read and dropped, delta_value = 0
         and read_pos = the loop position.
   - any other data: nrpn_count (+0x1F) += 1; the value is the tone number that
     _SsContDataEntry hands to the NRPN attribute handlers.
   The recorded size ends in three pad nops, hence INCLUDE_PAD(12). */
/* func_80096698 */
void _SsContNrpn2(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    switch (data) {
    case 0x14:
        score->nrpn_msb = data;
        score->loop_armed = 1;
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        score->loop_pos = score->read_pos;
        break;
    case 0x1E:
        score->nrpn_msb = data;
        if (score->loop_count == 0) {
            score->loop_on = 0;
            score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        } else if (score->loop_count < 0x7F) {
            score->loop_count--;
            score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
            if (score->loop_count != 0) {
                score->read_pos = score->loop_pos;
            } else {
                score->loop_on = 0;
            }
        } else {
            _SsReadDeltaValue(seq_no, sep_no);
            score->delta_value = 0;
            score->read_pos = score->loop_pos;
        }
        break;
    default:
        score->nrpn_msb = data;
        score->nrpn_count++;
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        break;
    }
}

INCLUDE_PAD(12);
