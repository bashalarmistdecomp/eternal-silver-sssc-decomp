/* PSY-Q 4.5 libcd, member c_011.o, part 1 of 2: StCdInterrupt. The first
   split had it in main_af.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* the .s slot is 0x20; the code is 0x1C (the rest is trailing padding) */
INCLUDE_PAD(4);

/* func_80128710 */
INCLUDE_ASM("asm/nonmatchings/main", StCdInterrupt);
