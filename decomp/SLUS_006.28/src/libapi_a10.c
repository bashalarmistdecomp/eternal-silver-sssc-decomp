/* PSY-Q 4.5 libapi, member a10.o: WaitEvent. The first split had it in
   psyq_misc_ah.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: SpuClearReverbWorkArea */
/* return_slot_fill: SpuClearReverbWorkArea */

/* func_800A01C8 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", WaitEvent);
