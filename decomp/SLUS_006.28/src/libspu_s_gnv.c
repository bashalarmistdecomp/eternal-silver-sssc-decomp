/* PSY-Q 4.5 libspu, member s_gnv.o: SpuGetNoiseVoice. The first split had
   it in psyq_misc_ae.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SpuGetNoiseVoice */
s32 _SpuGetAnyVoice(s32, s32);

/* libspu SpuGetNoiseVoice() (S_GNV.OBJ): the 24-bit mask of the voices whose source is
   the noise generator, read from the SPU's noise-enable register pair (words 0xCA and
   0xCB): _SpuGetAnyVoice(0xCA, 0xCB). Caller: _SsVmFlush, which keeps the
   bits above libsnd's voice count (_SsVmMaxVoice) as they are. */
/* func_8009F388 */
unsigned long SpuGetNoiseVoice(void) {
    return _SpuGetAnyVoice(0xCA, 0xCB);
}
INCLUDE_PAD(12);
