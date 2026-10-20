/* PSY-Q 4.5 libcd, member cdread2.o, part 2 of 2: func_8009394C. The first
   split had it in psyq_spu_b.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: func_8009394C */

/* libcd CDREAD2.OBJ's static after CdRead2, which installs it with
   CdReadyCallback when the mode has CdlModeStream: a bare call of StCdInterrupt
   . */
void func_8009394C(void) {
    StCdInterrupt();
}

INCLUDE_PAD(12);
