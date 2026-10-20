/* PSY-Q 4.5 libc2, member strcmp.o: strcmp. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* cc97_lib: strcmp */

/* libc2 strcmp (STRCMP.OBJ): compare two strings as signed chars. A null pointer
   is handled before anything is read: both null = 0, s1 null = -1, s2 null = 1.
   Otherwise the result is 0 at the shared terminator or the difference of the
   first pair of characters that differ. */
/* func_8008B788 */
s32 strcmp(s8 *s1, s8 *s2)
{
    if (s1 == 0 || s2 == 0) {
        if (s1 == s2) {
            return 0;
        }
        if (s1 == 0) {
            return -1;
        }
        return 1;
    }
    while (*s1 == *s2++) {
        if (*s1++ == 0) {
            return 0;
        }
    }
    return *s1 - *--s2;
}

INCLUDE_PAD(12);
