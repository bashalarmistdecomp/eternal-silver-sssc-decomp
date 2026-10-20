/* PSY-Q 4.5 libpad, member padportm.o: PadInitMtap, func_8012EBD8,
   func_8012EC4C, func_8012ED64, func_8012EDA8, func_8012EEB8,
   func_8012F134, func_8012F1B0. The first split had it in main_aag.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012E9A0 */
INCLUDE_ASM("asm/nonmatchings/main", PadInitMtap);

INCLUDE_ASM("asm/nonmatchings/main", func_8012EBD8);

INCLUDE_ASM("asm/nonmatchings/main", func_8012EC4C);

INCLUDE_ASM("asm/nonmatchings/main", func_8012ED64);

INCLUDE_ASM("asm/nonmatchings/main", func_8012EDA8);

INCLUDE_ASM("asm/nonmatchings/main", func_8012EEB8);

INCLUDE_ASM("asm/nonmatchings/main", func_8012F134);

INCLUDE_ASM("asm/nonmatchings/main", func_8012F1B0);
