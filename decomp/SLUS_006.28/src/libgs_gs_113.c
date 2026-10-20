/* PSY-Q 4.5 libgs, member gs_113.o: GsClearOt. The first split had it in
   psyq_cd_k.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: GsDrawOt */

/* cc97_lib: GsClearOt */
extern u_long *ClearOTagR(u_long *ot, int n);      /* libgpu ClearOTagR */

/* libgs GsClearOt (GS_113.OBJ): initialises an ordering table whose length (log2 of the
   entry count) and org (the entry array) are already set. Stores the caller's offset and
   point (the table's Z offset and its representative Z, per the SDK reference), points
   tag at the last entry, org + (1 << length) - 1, which is where GsDrawOt
    starts, and has ClearOTagR link the 1 << length
   entries as a reverse ordering table. */
/* func_8008FB98 */
void GsClearOt(unsigned short offset, unsigned short point, GsOT *otp)
{
    otp->offset = offset;
    otp->point = point;
    otp->tag = otp->org + (1 << otp->length) - 1;
    ClearOTagR((u_long *)otp->org, 1 << otp->length);
}

INCLUDE_PAD(8);
