/* PSY-Q 4.5 libgs, member gs_003.o: GsSetDrawBuffClip. The first split had
   it in psyq_cd_c.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
extern short D_800E6FD4;      /* PSDIDX */
extern RECT D_800E6FC8[];     /* CLIP2 (one RECT; unsized so cc1 does not treat it as small data) */
extern short D_800E6F38[2];   /* PSDOFSX */
extern short D_800E6F3C[2];   /* PSDOFSY */
extern DRAWENV D_800E6F50;    /* GsDRAWENV */

/* cc97_lib: GsSetDrawBuffClip */
/* gnu_as_loads: GsSetDrawBuffClip */
/* libgs GsSetDrawBuffClip (GS_00x.OBJ): the clip rectangle CLIP2 (D_800E6FC8), moved by
   the current draw buffer's frame-buffer origin (PSDOFSX / PSDOFSY[PSDIDX]), becomes
   GsDRAWENV.clip (D_800E6F50) and is sent with PutDrawEnv. */
/* func_8008F168 */
void GsSetDrawBuffClip(void)
{
    RECT *clip = D_800E6FC8;
    DRAWENV *env = &D_800E6F50;
    int offx = D_800E6F38[D_800E6FD4];
    int offy = D_800E6F3C[D_800E6FD4];
    int x, y;

    env->clip.w = clip->w;
    env->clip.h = clip->h;
    x = clip->x + offx;
    y = clip->y + offy;
    env->clip.x = x;
    env->clip.y = y;
    PutDrawEnv(env);
}
INCLUDE_PAD(8);
