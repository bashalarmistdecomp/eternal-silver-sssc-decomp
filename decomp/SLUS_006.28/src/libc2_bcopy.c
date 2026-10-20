/* PSY-Q 4.5 libc2, member bcopy.o: bcopy. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* cc97_lib: bcopy */

/* libc2 bcopy(src, dst, n) (BCOPY.OBJ): copy n bytes from src to dst, low address
   first, and return src. A null src copies nothing and returns 0. */
/* func_8008B6E8 */
u8 *bcopy(u8 *src, u8 *dst, s32 n)
{
    u8 *ret;

    if (src == 0) {
        return 0;
    }
    ret = src;
    while (n > 0) {
        *dst++ = *src++;
        n--;
    }
    return ret;
}

INCLUDE_PAD(12);
