/* PSY-Q 4.5 libpad, member padif.o: func_8012E370, func_8012E3B8,
   func_8012E490, func_8012E53C, func_8012E600. The first split had it in
   main_aag.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

INCLUDE_ASM("asm/nonmatchings/main", func_8012E370);

INCLUDE_ASM("asm/nonmatchings/main", func_8012E3B8);

INCLUDE_ASM("asm/nonmatchings/main", func_8012E490);

INCLUDE_ASM("asm/nonmatchings/main", func_8012E53C);

INCLUDE_ASM("asm/nonmatchings/main", func_8012E600);
