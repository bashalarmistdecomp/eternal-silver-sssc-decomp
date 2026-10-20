/* PSY-Q 4.5 libgs, member gs_111.o: GsDrawOt. The first split had it in
   psyq_cd_k.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: GsDrawOt */
extern void DrawOTag(u_long *p);                 /* libgpu DrawOTag */

/* libgs GsDrawOt (GS_111.OBJ): draws an ordering table: DrawOTag on
   ot->tag, the entry GsClearOt points at the end of the table. */
/* func_8008FB68 */
void GsDrawOt(GsOT *ot)
{
    DrawOTag((u_long *)ot->tag);
}

INCLUDE_PAD(12);
