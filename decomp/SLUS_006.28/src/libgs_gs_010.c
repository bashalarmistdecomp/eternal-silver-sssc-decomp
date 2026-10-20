/* PSY-Q 4.5 libgs, member gs_010.o: GsSwapDispBuff. The first split had it
   in psyq_cd_g.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: GsSwapDispBuff */
/* gnu_as_loads: GsSwapDispBuff */
extern s16 D_800E6FD4;
extern s16 D_800E6F38[];
extern s16 D_800E6F3C[];
extern DISPENV D_800E6FB0;
extern u_long D_800E6FD0;

/* libgs GsSwapDispBuff (GS_010.OBJ): shows the buffer that was just drawn and makes
   the other one the draw buffer. GsDISPENV.disp.x/y (D_800E6FB0) = the display
   offset of buffer PSDIDX (PSDOFSX/PSDOFSY, D_800E6F38 / D_800E6F3C, indexed by
   D_800E6FD4), PutDispEnv, SetDispMask(1); then libgs.h's GsIncFrame(): the frame
   counter PSDCNT (D_800E6FD0) + 1, never 0, and PSDIDX flipped 0 <-> 1; then
   GsSetDrawBuffClip and GsSetDrawBuffOffset for the
   new draw buffer. */
/* func_8008F428 */
void GsSwapDispBuff(void)
{
    D_800E6FB0.disp.x = D_800E6F38[D_800E6FD4];
    D_800E6FB0.disp.y = D_800E6F3C[D_800E6FD4];
    PutDispEnv(&D_800E6FB0);
    SetDispMask(1);
    D_800E6FD0++;
    D_800E6FD0 = D_800E6FD0 ? D_800E6FD0 : 1;
    D_800E6FD4 = (D_800E6FD4 == 0 ? 1 : 0);
    GsSetDrawBuffClip();
    GsSetDrawBuffOffset();
}

INCLUDE_PAD(12);
