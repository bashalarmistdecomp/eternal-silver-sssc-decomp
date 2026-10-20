/* PSY-Q 4.5 libapi, member a24.o: ResetEntryInt. The first split had it in
   psyq_spu_g.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

s32 ResetEntryInt();

/* func_800952C8 */
INCLUDE_ASM("asm/nonmatchings/psyq_spu", ResetEntryInt);
