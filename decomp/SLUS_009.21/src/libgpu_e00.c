/* PSY-Q 4.5 libgpu, member e00.o: LoadTPage. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124A40 */
INCLUDE_ASM("asm/nonmatchings/main", LoadTPage);
