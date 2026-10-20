/* PSY-Q 4.5 libgs, member gs_107.o, part 3 of 3: func_8008FB14. The first
   split had it in psyq_cd_j.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: func_8008FB14 */
/* return_slot_fill: func_8008FB14 */
extern MATRIX D_800E7010;

/* libgs GS_107.OBJ, static (no SDK name): the getter of the object's local colour
   matrix _LC (D_800E7010). Copies _LC into *m. Its one caller is GsSetFlatLight
   , which edits one column of the copy and stores it back with
   func_8008FAB0. */
void func_8008FB14(MATRIX *m)
{
    *m = D_800E7010;
}
INCLUDE_PAD(8);
