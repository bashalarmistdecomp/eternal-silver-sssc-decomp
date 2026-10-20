/* PSY-Q 4.5 libc2, member memset.o: memset. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* cc97_lib: memset */

/* libc2 memset(p, c, n) (MEMSET.OBJ): fill n bytes at p with c and return p. A
   null p, or n <= 0, writes nothing and returns 0. */
/* func_8008B758 */
s8 *memset(s8 *p, s8 c, s32 n)
{
    s8 *ret;

    if (p == 0) {
        return 0;
    }
    ret = p;
    if (n <= 0) {
        return 0;
    }
    while (n > 0) {
        *p++ = c;
        n--;
    }
    return ret;
}
