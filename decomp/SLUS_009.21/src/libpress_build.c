/* PSY-Q 4.5 libpress, member build.o: DecDCTvlcBuild. The first split had
   it in main_ak.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80129D90 */
INCLUDE_ASM("asm/nonmatchings/main", DecDCTvlcBuild);
