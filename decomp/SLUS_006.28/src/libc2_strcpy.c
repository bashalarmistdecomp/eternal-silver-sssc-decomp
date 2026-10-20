/* PSY-Q 4.5 libc2, member strcpy.o: strcpy. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* cc97_lib: strcpy */

/* libc2 strcpy(dst, src) (STRCPY.OBJ): copy the string src, terminator included,
   to dst and return dst. A null dst or a null src copies nothing and returns 0. */
/* func_8008B7F8 */
char *strcpy(char *dst, char *src)
{
    char *ret;

    if (dst == 0 || src == 0) {
        return 0;
    }
    ret = dst;
    while ((*dst++ = *src++) != 0) {
    }
    return ret;
}

INCLUDE_PAD(12);
