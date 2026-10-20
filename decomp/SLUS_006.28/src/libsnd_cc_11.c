/* PSY-Q 4.5 libsnd, member cc_11.o: _SsContExpression. The first split had
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
int _SsVmSetVol(short seq_sep_num, short vab_id, short prog, unsigned short vol, unsigned short pan);
s32 _SsVmSetProgVol(s16 vabId, s16 prog, u8 vol);

/* cc97_lib: _SsContExpression */
/* gnu_as_loads: _SsContExpression */
/* return_slot_fill: _SsContExpression */

/* libsnd _SsContExpression(seq_no, sep_no, data) (cc_11.o): SEQ control change 0x0B
   (expression).  score = &_ss_score[seq_no][sep_no] (D_800E9628), ch = score->channel:
   _SsVmSetProgVol(vab_id, programs[ch], data), then _SsVmSetVol
   (seq_no | sep_no << 8, vab_id, programs[ch], vol[ch], panpot[ch]), then
   delta_value = _SsReadDeltaValue.  data is not stored in the score.
   The recorded size ends in three pad nops, hence INCLUDE_PAD(12). */
/* func_80096368 */
void _SsContExpression(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;
    u8 ch;

    score = &D_800E9628[seq_no][sep_no];
    ch = score->channel;
    _SsVmSetProgVol(score->vab_id, score->programs[ch], data);
    _SsVmSetVol(seq_no | (sep_no << 8), score->vab_id, score->programs[ch], score->vol[ch], score->panpot[ch]);
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(12);
