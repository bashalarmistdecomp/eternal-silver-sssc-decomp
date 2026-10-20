/* PSY-Q 4.5 libgte, member patchgte.o: _patch_gte, func_80090044. The first
   split had it in psyq_cd_l.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_8008FF98 */
INCLUDE_ASM("asm/nonmatchings/psyq_cd", _patch_gte);

INCLUDE_ASM("asm/nonmatchings/psyq_cd", func_80090044);
