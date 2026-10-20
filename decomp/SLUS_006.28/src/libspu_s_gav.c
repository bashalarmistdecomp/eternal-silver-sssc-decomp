/* PSY-Q 4.5 libspu, member s_gav.o: _SpuGetAnyVoice. The first split had it
   in psyq_misc_af.c.
   The build must stay byte-matching. */

/* far_syms: D_800B7BB0 */
/* far_syms: D_800B7BB0 (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libspu's register block as the library declares it (psyz's libspu_private.h, `union
   SpuUnion`): the registers by name, or as 0x100 16-bit words for the functions that are
   handed a register number. SpuRXX is libspu_spu_1.c's with the reverb registers named. */
extern SpuUnion *D_800B7BB0;        /* _spu_RXX -> 0x1F801C00 */
/* cc97_lib: _SpuGetAnyVoice */

/* libspu _SpuGetAnyVoice (S_GAV.OBJ) */
/* func_8009F3B8 */
s32 _SpuGetAnyVoice(s32 addr1, s32 addr2) {
    return (D_800B7BB0->raw[addr2] & 0xFF) << 16 | D_800B7BB0->raw[addr1];
}
