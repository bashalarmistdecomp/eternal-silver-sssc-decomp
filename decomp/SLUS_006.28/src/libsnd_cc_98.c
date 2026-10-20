/* PSY-Q 4.5 libsnd, member cc_98.o: _SsContNrpn1. The first split had it in
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
extern SsMarkCallbackProc D_800E96A8[32][16];    /* _SsMarkCallback */
s32 _SsReadDeltaValue(s16 seq_no, s16 sep_no);

/* cc97_lib: _SsContNrpn1 */
/* return_slot_fill: _SsContNrpn1 */

/* libsnd _SsContNrpn1(seq_no, sep_no, data) (cc_98.o): SEQ control change 0x62 (98, NRPN
   first byte).  score = &_ss_score[seq_no][sep_no] (D_800E9628).  When the NRPN second
   byte in hand (nrpn_msb) is 40, the mark: the callback SsSetMarkCallback registered for
   this score, _SsMarkCallback[seq_no][sep_no] (D_800E96A8), is called with (seq_no,
   sep_no, data) if there is one.  Unless nrpn_msb is 30 (loop end), 20 (loop start) or
   40 (mark): nrpn_lsb = data (the DE_* attribute index _SsContDataEntry dispatches on),
   loop_armed = 0 and nrpn_count += 1.  Then delta_value = _SsReadDeltaValue
   . */
/* func_80096598 */
void _SsContNrpn1(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;
    SsMarkCallbackProc cb;

    score = &D_800E9628[seq_no][sep_no];
    if (score->nrpn_msb == 40) {
        cb = D_800E96A8[seq_no][sep_no];
        if (cb != 0) {
            cb(seq_no, sep_no, data);
        }
    }
    if (score->nrpn_msb != 30 && score->nrpn_msb != 20 && score->nrpn_msb != 40) {
        score->nrpn_lsb = data;
        score->loop_armed = 0;
        score->nrpn_count++;
    }
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}
