/* PSY-Q 4.5 libsnd, member ut_roff.o: SsUtReverbOff. The first split had it
   in psyq_misc_p.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: SsUtReverbOff */
/* libsnd SsUtReverbOff (UT_ROFF.OBJ): SpuSetReverb(SPU_OFF). */
/* func_8009A028 */
void SsUtReverbOff(void) {
    SpuSetReverb(0);
}
