/* PSY-Q 4.5 libapi, member a81.o: Krom2RawAdd. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* func_8008B428 */
INCLUDE_ASM("asm/nonmatchings/psyq_gpu", Krom2RawAdd);
