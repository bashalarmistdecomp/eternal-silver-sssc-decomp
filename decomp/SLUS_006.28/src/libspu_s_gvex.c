/* PSY-Q 4.5 libspu, member s_gvex.o: SpuGetVoiceEnvelope. The first split
   had it in psyq_misc_aj.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's. */
extern SpuUnion *D_800B7BB0;        
/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

/* cc97_lib: SpuGetVoiceEnvelope */
/* return_slot_fill: SpuGetVoiceEnvelope */
/* libspu SpuGetVoiceEnvelope(vNum, envx) (S_GVEX.OBJ): *envx = voice vNum's current
   envelope level, the SPU's ENVX register: word 6 of the voice's eight, by register
   number like the rest of the library's voice access. */
/* func_800A0D88 */
void SpuGetVoiceEnvelope(int vNum, short *envx)
{
    *envx = D_800B7BB0->raw[vNum * 8 + 6];
}

INCLUDE_PAD(4);
