/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c), MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member cc_0.o: _SsContBankChange. The first split had
   it in psyq_spu_o.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* libsnd's score record (0xB0 bytes, one per sequence: _ss_score[access number][sequence]);
   the members the functions of this TU use. */
extern SsScore *D_800E9628[];
void _SsContBankChange(s16 seq_no, s16 sep_no);

/* cc97_lib: _SsContBankChange */
/* gnu_as_loads: _SsContBankChange */
/* return_slot_fill: _SsContBankChange */
s32 _SsReadDeltaValue(s16 seq_no, s16 sep_no);

/* libsnd _SsContBankChange(seq_no, sep_no) (cc_0.o): SEQ control change 0 (bank change),
   the SsFCALL.control[CC_BANKCHANGE] slot.  On score = &_ss_score[seq_no][sep_no]
   (D_800E9628): vab_id = the next byte at read_pos, which is advanced past it, then
   delta_value = _SsReadDeltaValue.  _SsSetControlChange does controller 0
   in line and never calls this slot.  The recorded size is followed by three pad nops,
   hence INCLUDE_PAD(12). */
/* func_80095DC8 */
void _SsContBankChange(s16 seq_no, s16 sep_no)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    score->vab_id = *score->read_pos++;
    score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
}

INCLUDE_PAD(12);
