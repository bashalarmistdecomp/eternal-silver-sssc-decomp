/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ssplay.c), psyz (decomp/src/libsnd/playmode.c) and
   psy-q-decomp (src/snd/ssplay.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member playmode.o: Snd_SetPlayMode. The first split had
   it in psyq_misc_b.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: Snd_SetPlayMode */
/* The slice of a 0xB0-byte score record (_ss_score[a][b]) this object's
   function uses. */
extern SsScore *D_800E9628[];               /* _ss_score */
s16 _SsVmSetSeqVol(s16, u16, u16, s16);

/* libsnd Snd_SetPlayMode (PLAYMODE.OBJ): start the score
   _ss_score[sep_no][seq_no] from its first event; SsSeqPlay
    calls it with seq_no 0.  The read pointer, the loop point and the rewind
   position all go back to the first event (+4), the finished (0x200) and
   stop-request (4) flags are cleared and the repetition count is taken.
   play_mode 1 (SSPLAY_PLAY) sets the playing flag, clears the repetitions
   played, sets +0x14 and re-applies the score's volume
   (_SsVmSetSeqVol(sep_no | seq_no << 8, voll, volr, 1)); play_mode 0
   (SSPLAY_PAUSE) sets the pause-request flag (2), which the next tick
   serves through _SsSndPause; any other value does neither. */
/* func_80098018 */
void Snd_SetPlayMode(s16 sep_no, s16 seq_no, s8 play_mode, s16 l_count)
{
    SsScore *score = &D_800E9628[sep_no][seq_no];

    score->read_pos = score->start_pos;
    score->loop_pos = score->start_pos;
    score->rewind_pos = score->start_pos;
    D_800E9628[sep_no][seq_no].flags &= ~0x200;
    D_800E9628[sep_no][seq_no].flags &= ~4;
    score->l_count = l_count;
    if (play_mode == 1) {
        D_800E9628[sep_no][seq_no].flags |= 1;
        score->l_count_done = 0;
        score->play_mode = 1;
        _SsVmSetSeqVol(sep_no | (seq_no << 8), score->voll, score->volr, 1);
    } else if (play_mode == 0) {
        D_800E9628[sep_no][seq_no].flags |= 2;
    }
}
INCLUDE_PAD(8);
