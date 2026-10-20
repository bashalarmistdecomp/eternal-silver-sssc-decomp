/* PSY-Q 4.5 libsnd, member ut_roff.o: SpuInit. The first split had it in
   main_ak.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
void SpuInit(void);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* epilogue_load_slot: SpuInit */
/* func_80129EE0 */
void SpuInit(void) {
    _SpuInit(0);
}
