/* PSY-Q 4.5 libgpu, member sys.o, part 2 of 6: _addque. The first
   split had it in psyq_gpu_c.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _addque */
/* gnu_as_loads: _addque */
extern int func_8008D99C(int (*func)(u_long p1, u_long p2), u_long p1, int len, u_long p2);

/* libgpu _addque(func, p1, p2) (SYS.OBJ, static; the GPU function table's +4
   entry): queue the GPU routine func with arguments p1 and p2, p1 passed as it is
   (no copy of the data it points at). It is _addque2 (func_8008D99C) with length
   0, and returns what that returns. */
/* func_8008D978 */
int _addque(int (*func)(u_long p1, u_long p2), u_long p1, u_long p2)
{
    return func_8008D99C(func, p1, 0, p2);
}
