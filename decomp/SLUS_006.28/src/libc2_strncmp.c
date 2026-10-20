/* PSY-Q 4.5 libc2, member strncmp.o: strncmp. The first split had it in
   psyq_cd_aa.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

s32 strncmp();

/* cc97_lib: strncmp */

/* libc2 strncmp (STRNCMP.OBJ; word for word the 4.4 object): compare at most n characters.
   Sony's addition to the classic loop is the null-pointer preamble: two null pointers are
   equal (0), a null s1 sorts first (-1), a null s2 last (1). Then, while n lasts and the
   characters agree, a NUL ends it with 0; the result is the difference of the first
   differing pair as signed chars, or 0 when n ran out. Plain char is s8 here: the library
   was built with signed char (the target reads with lb). */
/* func_80092E78 */
s32 strncmp(s8 *s1, s8 *s2, s32 n)
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
    while (--n >= 0 && *s1 == *s2++) {
        if (*s1++ == '\0') {
            return 0;
        }
    }
    return n < 0 ? 0 : *s1 - *--s2;
}
