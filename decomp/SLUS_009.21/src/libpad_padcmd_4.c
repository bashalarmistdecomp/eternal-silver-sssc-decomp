/* PSY-Q 4.5 libpad, member padcmd.o, part 4 of 7: func_8012DB8C,
   func_8012DC34. The first split had it in main_aad.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

INCLUDE_ASM("asm/nonmatchings/main", func_8012DB8C);

INCLUDE_ASM("asm/nonmatchings/main", func_8012DC34);
