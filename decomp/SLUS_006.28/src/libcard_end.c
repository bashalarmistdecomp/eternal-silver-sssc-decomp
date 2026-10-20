/* PSY-Q 4.5 libcard, member end.o: _ExitCard. The first split had it in
   psyq_misc_ao.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _Pad1 */

static inline void pad_wait(volatile s32 n)
{
    while (--n != -1) {
    }
}

/* func_800A1878 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", _ExitCard);
