/* Lunar SSSC — movie_run: the movie's run: the CD started, the stream
   played until the code is entered or Select pressed. The first split had
   it in main.c. C, with 1 routine still read as byte ranges (INCLUDE_ASM).
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* Measured arities — outgoing arg-slot stores (sw 0x10..0x1C($sp)) at
   every call site in the two giants.
   The variadic two are per-site variant (6/7 and 4/6 args at different
   sites): unprototyped originals, so `...` is the honest model. */
s32 movie_run();
void main(void);  /* extern */

/* func_8011E2F0 */
INCLUDE_ASM("asm/nonmatchings/main", movie_run);
