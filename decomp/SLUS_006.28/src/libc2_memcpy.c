/* PSY-Q 4.5 libc2, member memcpy.o: memcpy. The first split had it in
   psyq_gpu_g.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _GPU_ResetCallback */
/* gnu_as_loads: _GPU_ResetCallback */
     /* cc97_lib: memcpy */

/* libc2 memcpy(dst, src, n) (MEMCPY.OBJ): copies n bytes from src to dst, lowest
   address first, and returns dst. A null dst copies nothing and returns 0; n <= 0
   copies nothing and returns dst. The parameter types are memory.h's (its
   prototype carries them in a comment). */
/* func_8008E7C8 */
void *memcpy(u_char *dst, u_char *src, int n)
{
    u_char *ret;

    if (dst == 0) {
        return 0;
    }
    ret = dst;
    while (n > 0) {
        *dst++ = *src++;
        n--;
    }
    return ret;
}
INCLUDE_PAD(12);
