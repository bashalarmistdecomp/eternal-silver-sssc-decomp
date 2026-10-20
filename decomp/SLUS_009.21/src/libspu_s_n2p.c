/* PSY-Q 4.5 libspu, member s_n2p.o: _spu_note2pitch, _spu_pitch2note. The
   first split had it in main_as.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012C280 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_note2pitch);

/* func_8012C350 */
INCLUDE_ASM("asm/nonmatchings/main", _spu_pitch2note);
