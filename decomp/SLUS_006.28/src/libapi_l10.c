/* PSY-Q 4.5 libapi, member l10.o: ChangeClearRCnt. The first split had it
   in psyq_spu_f.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_80094B20 */
/* reorder_fill: func_80094B20 */
s32 ChangeClearRCnt(s32, s32);

/* func_80094BC8 */
INCLUDE_ASM("asm/nonmatchings/psyq_spu", ChangeClearRCnt);
