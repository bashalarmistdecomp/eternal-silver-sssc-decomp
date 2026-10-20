/* PSY-Q 4.5 libspu, member s_snv.o: SpuSetNoiseVoice. The first split had
   it in psyq_misc_ac.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SpuFree */

/* cc97_lib: SpuSetNoiseVoice */
u32 _SpuSetAnyVoice(s32 on_off, u32 voice_bit, s32 reg_lo, s32 reg_hi);

/* libspu SpuSetNoiseVoice (S_SNV.OBJ) */
/* func_8009F098 */
unsigned long SpuSetNoiseVoice(long on_off, unsigned long voice_bit) {
    return _SpuSetAnyVoice(on_off, voice_bit, 0xCA, 0xCB);
}
INCLUDE_PAD(12);
