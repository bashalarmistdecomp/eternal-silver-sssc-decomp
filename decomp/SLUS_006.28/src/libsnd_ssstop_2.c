/* PSY-Q 4.5 libsnd, member ssstop.o, part 2 of 2: SsSeqStop, SsSepStop. The
   first split had it in psyq_misc_j.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SsSeqStop */
void _SsSndStop(s16, s16);

/* libsnd SsSeqStop (SSSTOP.OBJ): stop the SEQ seq_access_num at once:
   _SsSndStop on record 0 of its row. */
/* func_80099368 */
void SsSeqStop(s16 seq_access_num)
{
    _SsSndStop(seq_access_num, 0);
}
/* cc97_lib: SsSepStop */

/* libsnd SsSepStop (SSSTOP.OBJ): stop sequence seq_no of the SEP
   sep_access_num at once: _SsSndStop on that record. */
/* func_80099390 */
void SsSepStop(s16 sep_access_num, s16 seq_no)
{
    _SsSndStop(sep_access_num, seq_no);
}

INCLUDE_PAD(12);
