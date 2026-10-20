/* PSY-Q 4.5 libsnd, member ssstart.o, part 2 of 4: SsStart, SsStart2. The
   first split had it in psyq_misc_d.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

void func_80098138(s32 use_vsync);                      /* _SsStart */

/* cc97_lib: SsStart */
/* libsnd SsStart (SSSTART.OBJ): _SsStart(1) (func_80098138). */
/* func_80098368 */
void SsStart(void) {
    func_80098138(1);
}
/* cc97_lib: SsStart2 */
/* libsnd SsStart2 (SSSTART.OBJ): _SsStart(0). */
/* func_80098388 */
void SsStart2(void) {
    func_80098138(0);
}
