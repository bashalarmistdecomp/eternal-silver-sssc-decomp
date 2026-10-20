/* PSY-Q 4.5 libgs, member gs_107.o, part 2 of 3: func_8008FAB0. The first
   split had it in psyq_cd_i.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: func_8008FAB0 */
extern MATRIX D_800E7010;

/* libgs GS_107.OBJ, static (no SDK name): the setter of the object's local colour
   matrix _LC (D_800E7010). Copies *m into _LC and loads the same matrix into the
   GTE with SetColorMatrix. Its one caller is GsSetFlatLight
   . */
void func_8008FAB0(MATRIX *m)
{
    D_800E7010 = *m;
    SetColorMatrix((const int *) m);
}
