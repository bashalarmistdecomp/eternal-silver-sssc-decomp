/* PSY-Q 4.5 libapi, member a25.o: HookEntryInt. The first split had it in
   psyq_spu_g.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

s32 HookEntryInt();
s32 HookEntryInt();

/* func_800952D8 */
INCLUDE_ASM("asm/nonmatchings/psyq_spu", HookEntryInt);
