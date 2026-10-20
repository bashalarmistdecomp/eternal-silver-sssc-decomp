/* PSY-Q 4.5 libapi, member pad.o, part 3 of 4: func_801200A4,
   RemovePatchPad. The first split had it in main_l.c. The tree compiles this
   run of the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

INCLUDE_ASM("asm/nonmatchings/main", func_801200A4);

/* func_8012011C */
INCLUDE_ASM("asm/nonmatchings/main", RemovePatchPad);
