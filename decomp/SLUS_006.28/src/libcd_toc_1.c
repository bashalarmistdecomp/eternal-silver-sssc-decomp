/* PSY-Q 4.5 libcd, member toc.o, part 1 of 2: CdGetToc. The first split had
   it in psyq_cd_m.c. The tree compiles this run of the member's functions
   with other flags than the runs beside it (config/cc_flags.txt), so the
   member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

s32 CdGetToc2(s32 session, CdlLOC *loc);
/* cc97_lib: CdGetToc */

/* libcd CdGetToc (TOC.OBJ): calls CdGetToc2 with 1 and loc and returns
   its result, the number of tracks read into loc[] or 0. */
/* func_800900A8 */
s32 CdGetToc(CdlLOC *loc)
{
    return CdGetToc2(1, loc);
}
