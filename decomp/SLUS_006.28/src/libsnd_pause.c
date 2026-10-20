/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/pause.c) and psyz (decomp/src/libsnd/pause.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member pause.o: _SsSndPause. The first split had it in
   psyq_misc_g.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* The slice of a 0xB0-byte score record (_ss_score[a][b]) this object's
   functions use. */
extern SsScore *D_800E9628[];   /* _ss_score */
/* cc97_lib: SsSeqCalledTbyT */
/* return_slot_fill: SsSeqCalledTbyT */
void _SsSndPause(s16 seq, s16 sep);

/* cc97_lib: _SsSndPause */
/* return_slot_fill: _SsSndPause */
void _SsVmSeqKeyOff(s16);

/* libsnd _SsSndPause (PAUSE.OBJ): the tick's answer to a pause request (flag
   2): key off the score's voices (_SsVmSeqKeyOff), clear +0x14 and the
   request bit. */
/* func_800988C8 */
void _SsSndPause(s16 seq, s16 sep)
{
    SsScore *score = &D_800E9628[seq][sep];

    _SsVmSeqKeyOff(seq | (sep << 8));
    score->play_mode = 0;
    D_800E9628[seq][sep].flags &= ~2;
}
INCLUDE_PAD(8);
