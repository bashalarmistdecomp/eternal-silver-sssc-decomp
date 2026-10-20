/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/replay.c) and psyz (decomp/src/libsnd/replay.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member replay.o: _SsSndReplay. The first split had it
   in psyq_misc_i.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 bytes each;
   psyz's and sotn-decomp's struct SeqStruct, at this library version's
   offsets).  The names are docs/spec/code/subsystems/psyq-libs.md's. */
extern SsScore *D_800E9628[];  
/* cc97_lib: _SsSndReplay */

/* libsnd _SsSndReplay (REPLAY.OBJ): resume a paused score: unk14 = 1 and
   flag 8 (replay requested) is cleared.  SsSeqCalledTbyT
   calls it for a score whose flag 8 is set. */
/* func_80099188 */
void _SsSndReplay(s16 seq_no, s16 sep_no)
{
    SsScore *score;

    score = &D_800E9628[seq_no][sep_no];
    score->play_mode = 1;
    D_800E9628[seq_no][sep_no].flags &= ~8;
}
