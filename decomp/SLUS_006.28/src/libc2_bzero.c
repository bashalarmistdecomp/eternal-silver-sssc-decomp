/* PSY-Q 4.5 libc2, member bzero.o: bzero. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* cc97_lib: bzero */

/* libc2 bzero(p, n) (BZERO.OBJ): clear n bytes at p and return p. A null p, or
   n <= 0, clears nothing and returns 0. */
/* func_8008B728 */
s8 *bzero(s8 *p, s32 n)
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
        *p++ = 0;
        n--;
    }
    return ret;
}
