/* PSY-Q 4.5 libsnd, member ut_ron.o: SsUtReverbOn. The first split had it
   in psyq_misc_p.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: SsUtReverbOn */
/* libsnd SsUtReverbOn (UT_RON.OBJ): SpuSetReverb(SPU_ON). */
/* func_8009A048 */
void SsUtReverbOn(void) {
    SpuSetReverb(1);
}
