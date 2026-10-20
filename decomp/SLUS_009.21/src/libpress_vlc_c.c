/* PSY-Q 4.5 libpress, member vlc_c.o: DecDCTvlcSize2, DecDCTvlc2. The first
   split had it in main_ak.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
s32 DecDCTvlc2(s32, s32, s32 *);                     
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80129A10 */
INCLUDE_ASM("asm/nonmatchings/main", DecDCTvlcSize2);

/* func_80129A40 */
INCLUDE_ASM("asm/nonmatchings/main", DecDCTvlc2);
