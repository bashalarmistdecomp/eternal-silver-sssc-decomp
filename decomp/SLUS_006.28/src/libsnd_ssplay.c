/* PSY-Q 4.5 libsnd, member ssplay.o: SsSeqPlay, SsSepPlay. The first split
   had it in psyq_math_f.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

void Snd_SetPlayMode(s16 sep_no, s16 seq_no, s8 play_mode, s16 l_count);   
/* cc97_lib: SsSeqPlay */

/* libsnd SsSeqPlay(seq_access_num, play_mode, l_count) (ssplay.o): start or pause a SEQ:
   Snd_SetPlayMode(seq_access_num, 0, play_mode, l_count), record 0 of
   the access number.  play_mode is libsnd.h's plain char, signed in this build. */
/* func_80097F98 */
void SsSeqPlay(short seq_access_num, char play_mode, short l_count)
{
    Snd_SetPlayMode(seq_access_num, 0, play_mode, l_count);
}
/* cc97_lib: SsSepPlay */

/* libsnd SsSepPlay(sep_access_num, seq_num, play_mode, l_count) (ssplay.o): the same for
   sequence seq_num of a SEP: Snd_SetPlayMode(sep_access_num, seq_num,
   play_mode, l_count).  The recorded size ends in three pad nops, hence INCLUDE_PAD(12). */
/* func_80097FD0 */
void SsSepPlay(short sep_access_num, short seq_num, char play_mode, short l_count)
{
    Snd_SetPlayMode(sep_access_num, seq_num, play_mode, l_count);
}
INCLUDE_PAD(12);
