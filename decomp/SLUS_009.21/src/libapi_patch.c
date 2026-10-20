/* PSY-Q 4.5 libapi, member patch.o: EnablePAD, DisablePAD, _patch_pad. The
   first split had it in main_m.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80120260 */
INCLUDE_ASM("asm/nonmatchings/main", EnablePAD);

/* func_80120274 */
INCLUDE_ASM("asm/nonmatchings/main", DisablePAD);

/* func_80120288 */
INCLUDE_ASM("asm/nonmatchings/main", _patch_pad);
