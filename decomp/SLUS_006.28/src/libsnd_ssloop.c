/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/seqread.c) and psyz (decomp/src/libsnd/ssloop.c),
   all MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ssloop.o: SsSetLoop, SsIsEos. The first split
   had it in psyq_spu_o.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* libsnd's score record (0xB0 bytes, one per sequence: _ss_score[access number][sequence]);
   the members the functions of this TU use. */
extern SsScore *D_800E9628[];   /* _ss_score */

/* cc97_lib: SsSetLoop */
/* gnu_as_loads: SsSetLoop */

/* libsnd SsSetLoop(access_num, seq_num, l_count) (ssloop.o): set a song's repetition
   count and reset its current repetition count. */
/* func_80095AB8 */
void SsSetLoop(s16 access_num, s16 seq_num, s16 l_count)
{
    SsScore *score;

    score = &D_800E9628[access_num][seq_num];
    score->l_count = l_count;
    score->l_count_done = 0;
}

/* cc97_lib: SsIsEos */
/* gnu_as_loads: SsIsEos */

/* libsnd SsIsEos(access_num, seq_num) (ssiseos.o): return the byte at +0x14 of the score
   _ss_score[access_num][seq_num]. */
/* func_80095AF8 */
s16 SsIsEos(s16 access_num, s16 seq_num)
{
    SsScore *score;

    score = &D_800E9628[access_num][seq_num];
    return score->play_mode;
}
