/* PSY-Q 4.5 libspu, member s_grv.o: SpuGetReverbVoice. The first split had
   it in psyq_misc_ag.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SpuSetReverbVoice */

/* cc97_lib: SpuGetReverbVoice */
s32 _SpuGetAnyVoice(s32, s32);

/* libspu SpuGetReverbVoice() (S_GRV.OBJ): the 24-bit mask of the voices that feed the
   reverb, read from the SPU's reverb-enable register pair (words 0xCC and 0xCD):
   _SpuGetAnyVoice(0xCC, 0xCD). Caller: _SsVmFlush, which keeps the bits
   above libsnd's voice count (_SsVmMaxVoice) as they are. */
/* func_8009FFF8 */
unsigned long SpuGetReverbVoice(void) {
    return _SpuGetAnyVoice(0xCC, 0xCD);
}
INCLUDE_PAD(12);
