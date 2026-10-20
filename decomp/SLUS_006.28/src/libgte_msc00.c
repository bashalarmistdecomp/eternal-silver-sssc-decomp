/* PSY-Q 4.5 libgte, member msc00.o: func_8008FE08, InitGeom. The first
   split had it in psyq_cd_k.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: GsDrawOt */

INCLUDE_RODATA("asm/data/jtbl", jtbl_800131C4_pad);

INCLUDE_ASM("asm/nonmatchings/psyq_cd", func_8008FE08);

/* func_8008FE10 */
INCLUDE_ASM("asm/nonmatchings/psyq_cd", InitGeom);
