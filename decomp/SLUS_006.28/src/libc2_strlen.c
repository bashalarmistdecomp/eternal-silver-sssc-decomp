/* PSY-Q 4.5 libc2, member strlen.o: strlen. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* cc97_lib: strlen */

/* libc2 strlen(s) (STRLEN.OBJ): the number of bytes before the terminating 0 of
   the string s; 0 for a null s. */
/* func_8008B848 */
int strlen(s8 *s)
{
    int n = 0;

    if (s == 0) {
        return 0;
    }
    while (*s++) {
        n++;
    }
    return n;
}

INCLUDE_PAD(12);
