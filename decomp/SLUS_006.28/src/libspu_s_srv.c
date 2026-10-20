/* PSY-Q 4.5 libspu, member s_srv.o: SpuSetReverbVoice. The first split had
   it in psyq_misc_ag.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SpuSetReverbVoice */
u32 _SpuSetAnyVoice(s32 on_off, u32 voice_bit, s32 addr1, s32 addr2);

/* libspu SpuSetReverbVoice (S_SRV.OBJ): switch reverb on (1) or off (0) for the voices in
   voice_bit, or replace the whole mask (8); returns the resulting 24-bit mask */
/* func_8009FFC8 */
unsigned long SpuSetReverbVoice(long on_off, unsigned long voice_bit) {
    return _SpuSetAnyVoice(on_off, voice_bit, 0xCC, 0xCD);
}

INCLUDE_PAD(12);
