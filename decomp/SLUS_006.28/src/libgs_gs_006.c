/* PSY-Q 4.5 libgs, member gs_006.o: GsSetClip2D. The first split had it in
   psyq_cd_d.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: GsSetClip2D */
/* lib_g0: GsSetClip2D */
extern RECT D_800E6FC8;       /* CLIP2 (libgs.h: `extern RECT CLIP2;`) */

/* libgs GsSetClip2D (GS_006.OBJ): sets the 2D clipping area. Copies the caller's RECT into
   CLIP2 (D_800E6FC8) member by member: x, y, w, h. Nothing else is touched:
   GsSetDrawBuffClip is what puts CLIP2, moved by the current buffer's
   offset, into the drawing environment and sends it. */
/* func_8008F1E8 */
void GsSetClip2D(RECT *rectp)
{
    D_800E6FC8.x = rectp->x;
    D_800E6FC8.y = rectp->y;
    D_800E6FC8.w = rectp->w;
    D_800E6FC8.h = rectp->h;
}
INCLUDE_PAD(12);
