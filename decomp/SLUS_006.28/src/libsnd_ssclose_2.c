/* PSY-Q 4.5 libsnd, member ssclose.o, part 2 of 2: SsSeqClose, SsSepClose.
   The first split had it in psyq_spu_l.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SsSeqClose */
/* cc97_lib: SsSepClose */
void func_80095758(s16);    
/* libsnd SsSeqClose (SSCLOSE.OBJ): close the SEQ opened under access number
   seq_access_num.  All the work is _SsClose's (func_80095758): the voices are
   silenced and keyed off, the number is freed in _snd_openflag and the row's
   score records are reset. */
/* func_800958D4 */
void SsSeqClose(s16 seq_access_num)
{
    func_80095758(seq_access_num);
}

/* libsnd SsSepClose (SSCLOSE.OBJ): the same for a SEP's access number. */
/* func_800958F8 */
void SsSepClose(s16 sep_access_num)
{
    func_80095758(sep_access_num);
}
INCLUDE_PAD(12);
