/* PSY-Q 4.5 libapi, member a53.o: write. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* func_8008B408 */
INCLUDE_ASM("asm/nonmatchings/psyq_gpu", write);
