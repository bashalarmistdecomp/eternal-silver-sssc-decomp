/* PSY-Q 4.5 libpress, member libpress.o, part 4 of 4: MDEC_in_sync,
   MDEC_out_sync, func_80129978, func_80129990. The first split had it in
   main_aj.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
s32 MDEC_in_sync();
s32 MDEC_out_sync();
u32 func_80129978();

/* func_80129850 */
INCLUDE_ASM("asm/nonmatchings/main", MDEC_in_sync);

/* func_801298E4 */
INCLUDE_ASM("asm/nonmatchings/main", MDEC_out_sync);

INCLUDE_ASM("asm/nonmatchings/main", func_80129978);

INCLUDE_ASM("asm/nonmatchings/main", func_80129990);
