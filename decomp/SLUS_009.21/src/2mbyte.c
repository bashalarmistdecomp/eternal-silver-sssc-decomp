/* PSY-Q 4.5 obj, member 2mbyte.obj: __main, __SN_ENTRY_POINT. The first
   split had it in main_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();
/* empty leaf: an empty body byte-matches under ANY signature, so a
   prototype here is an unverifiable guess that rejects the next
   conversion in this TU. Unprototyped is byte-neutral. */
/* func_8011F05C */
void __main() {
}

/* func_8011F064 */
INCLUDE_ASM("asm/nonmatchings/main", __SN_ENTRY_POINT);
