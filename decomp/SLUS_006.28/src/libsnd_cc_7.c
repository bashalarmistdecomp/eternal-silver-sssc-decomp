/* PSY-Q 4.5 libsnd, member cc_7.o: _SsContMainVol. The first split had it
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
int _SsVmSetVol(short seq_sep_num, short vab_id, short prog, unsigned short vol, unsigned short pan);

/* cc97_lib: _SsContMainVol */
/* gnu_as_loads: _SsContMainVol */
/* return_slot_fill: _SsContMainVol */

/* libsnd _SsContMainVol(seq_no, sep_no, data) (cc_7.o): SEQ control change 7 (channel
   volume).  score = &_ss_score[seq_no][sep_no] (D_800E9628), ch = score->channel:
   _SsVmSetVol(seq_no | sep_no << 8, vab_id, programs[ch], data,
   panpot[ch]), then vol[ch] = data and delta_value = _SsReadDeltaValue.
   The recorded size ends in a pad nop, hence INCLUDE_PAD(4). */
/* func_800961C8 */
void _SsContMainVol(s16 seq_no, s16 sep_no, u8 data)
{
    SsScore *score;
    u8 ch;

    score = &D_800E9628[seq_no][sep_no];
    ch = score->channel;
    _SsVmSetVol(seq_no | (sep_no << 8), score->vab_id, score->programs[ch], data, score->panpot[ch]);
    score->vol[ch] = data;
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(4);
