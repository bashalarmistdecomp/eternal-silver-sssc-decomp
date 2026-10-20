/* PSY-Q 4.5 libsnd, member cc_6.o: _SsContDataEntry. The first split had it
   in psyq_spu_o.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* libsnd's score record (0xB0 bytes, one per sequence: _ss_score[access number][sequence]);
   the members the functions of this TU use. */
extern SsScore *D_800E9628[];
extern _SsFCALL D_800E9588;
void _SsContDataEntry(s16 seq_no, s16 sep_no, u8 data);
s32 _SsReadDeltaValue(s16 seq_no, s16 sep_no);

/* cc97_lib: _SsContDataEntry */
/* gnu_as_loads: _SsContDataEntry */
/* return_slot_fill: _SsContDataEntry */
extern _SsFCALL D_800E9588;
s16 SsUtGetProgAtr(s16, s16, ProgAtr *);
s16 SsUtGetVagAtr(s16, s16, s16, VagAtr *);
s16 SsUtSetVagAtr(s16, s16, s16, VagAtr *);

/* libsnd _SsContDataEntry(seq_no, sep_no, data): SEQ control change 6 (data
   entry), reached through SsFCALL.control[CC_DATAENTRY].  score =
   &_ss_score[seq_no][sep_no] (D_800E9628).  It reads the channel's program
   header (SsUtGetProgAtr) and then does ONE of:
   - a loop start is armed and no count taken yet: data is the loop's repeat
     count (loop_count), the arm flag is cleared and loop_on set;
   - both RPN bytes are in and the RPN MSB is 0: every tone of the program
     is read (SsUtGetVagAtr), changed and written back
     (SsUtSetVagAtr).  RPN 0 (bend range) sets pbmin = pbmax
     = data & 0x7F.  RPN 1 (fine tune) and RPN 2 (coarse tune) add a value
     whose low byte is always 0 to the one-byte shift / center, so they
     change nothing: the compiler dropped the add and kept the arithmetic.
     rpn_count is cleared;
   - both NRPN bytes are in: the tone attribute handler SsFCALL.ccentry
     [nrpn_lsb] (D_800E9588 + 0x44, _SsSetNrpnVabAttr0..19) gets (vab,
     program, tone, the VagAtr by value, nrpn_lsb, data), tone = nrpn_msb or
     0 when that is 16 (the 4.2 library looped over all tones there).  The
     VagAtr passed is whatever the stack holds: nothing fills it on this
     path.  nrpn_count is cleared.
   Every path then stores _SsReadDeltaValue in delta_value.
   MARKED SPELLING: in the RPN 1 and RPN 2 cases the statement `if (data <
   0x40) { tune = ...; }` in front of the if / else is dead: the if / else
   sets tune on both paths.  It stands for arithmetic the binary keeps (it
   computes `data < 0x40` into $s5 at 0x80095F44 and never reads it) whose
   source expression cannot be recovered: several expressions give the same
   bytes, and the live arm's own is written here.  Without the two dead
   statements the function scores 180/223 (159 with only RPN 1's, 149 with
   only RPN 2's).  The original construct is not known.
   The recorded size ends in one pad nop, hence INCLUDE_PAD(4). */
/* func_80095E48 */
void _SsContDataEntry(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;
    ProgAtr progAtr;
    VagAtr vagAtr;
    u8 channel;
    s32 i;
    s16 tune;

    score = &D_800E9628[seq_no][sep_no];
    channel = score->channel;
    SsUtGetProgAtr(score->vab_id, score->programs[channel], &progAtr);
    if (score->loop_armed == 1 && score->loop_on == 0) {
        score->loop_count = data;
        score->loop_armed = 0;
        score->loop_on = 1;
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
    } else if (score->rpn_count == 2) {
        if (score->rpn_msb == 0) {
            for (i = 0; i < progAtr.tones; i++) {
                SsUtGetVagAtr(score->vab_id, score->programs[channel], i, &vagAtr);
                switch (score->rpn_lsb) {
                case 0:
                    vagAtr.pbmin = vagAtr.pbmax = data & 0x7F;
                    break;
                case 1:
                    /* dead: the next statement sets tune on both paths */
                    if (data < 0x40) {
                        tune = (data * 100 / 8192) * 8192;
                    }
                    if (data > 0x40 && data < 0x80) {
                        tune = (data * 100 / 8192) * 8192;
                    } else {
                        tune = 0;
                    }
                    vagAtr.shift += tune;
                    break;
                case 2:
                    /* dead, as above */
                    if (data < 0x40) {
                        tune = data * 6400;
                    }
                    if (data >= 0x40 && data < 0x80) {
                        tune = data * 6400;
                    } else {
                        tune = 0;
                    }
                    vagAtr.center += tune;
                    break;
                }
                SsUtSetVagAtr(score->vab_id, score->programs[channel], i, &vagAtr);
            }
        }
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        score->rpn_count = 0;
    } else if (score->nrpn_count == 2) {
        if (score->nrpn_msb == 16) {
            D_800E9588.ccentry[score->nrpn_lsb](score->vab_id, score->programs[channel], 0, vagAtr, score->nrpn_lsb, data);
        } else {
            D_800E9588.ccentry[score->nrpn_lsb](score->vab_id, score->programs[channel], score->nrpn_msb, vagAtr, score->nrpn_lsb, data);
        }
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        score->nrpn_count = 0;
    } else {
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
    }
}
INCLUDE_PAD(4);
