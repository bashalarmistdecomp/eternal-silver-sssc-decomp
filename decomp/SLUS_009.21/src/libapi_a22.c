/* PSY-Q 4.5 libapi, member a22.o: PAD_dr. The first split had it in
   main_j.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
s32 PAD_dr();
extern void (*D_801336C8_s)();

/* func_8011FE80 */
INCLUDE_ASM("asm/nonmatchings/main", PAD_dr);
