/* PSY-Q 4.5 libapi, member a24.o: ResetEntryInt. The first split had it in
   main_j.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8011FEA0 */
INCLUDE_ASM("asm/nonmatchings/main", ResetEntryInt);
