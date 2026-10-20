/* PSY-Q 4.5 libsnd, member play.o: _SsSndPlay. The first split had it in
   psyq_misc_h.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _SsSndPlay */
void _SsSeqPlay(s16, s16);

/* libsnd _SsSndPlay (PLAY.OBJ): the tick's step for a playing score (flag
   1): _SsSeqPlay(seq, sep), which counts the delta time down and reads the
   events that are due. */
/* func_80098968 */
void _SsSndPlay(s16 seq, s16 sep)
{
    _SsSeqPlay(seq, sep);
}
INCLUDE_PAD(4);
