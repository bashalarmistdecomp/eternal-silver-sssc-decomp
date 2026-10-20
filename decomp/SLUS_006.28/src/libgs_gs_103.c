/* PSY-Q 4.5 libgs, member gs_103.o: GsDefDispBuff. The first split had it
   in psyq_cd_g.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: GsSwapDispBuff */
/* gnu_as_loads: GsSwapDispBuff */
extern s16 D_800E6F38[];
extern s16 D_800E6F3C[];

/* cc97_lib: GsDefDispBuff */
extern s16 D_800E6FD6;
extern s16 D_800E6F40[];
extern s16 D_800E6F44[];

/* libgs GsDefDispBuff (GS_103.OBJ): defines the double buffer. The frame-buffer
   origins of buffer 0 and buffer 1 go to PSDOFSX / PSDOFSY (D_800E6F38 /
   D_800E6F3C). The drawing bases PSDBASEX / PSDBASEY (D_800E6F40 / D_800E6F44) are
   0 when PSDGPU (D_800E6FD6: the drawing offset is applied by the GPU) is set and
   the same origins otherwise. Then GsSetDrawBuffClip and
   GsSetDrawBuffOffset re-apply the clip and the offset. */
/* func_8008F4D8 */
void GsDefDispBuff(u16 x0, u16 y0, u16 x1, u16 y1)
{
    D_800E6F38[0] = x0;
    D_800E6F38[1] = x1;
    D_800E6F3C[0] = y0;
    D_800E6F3C[1] = y1;
    if (D_800E6FD6) {
        D_800E6F40[0] = 0;
        D_800E6F40[1] = 0;
        D_800E6F44[0] = 0;
        D_800E6F44[1] = 0;
    } else {
        D_800E6F40[0] = x0;
        D_800E6F40[1] = x1;
        D_800E6F44[0] = y0;
        D_800E6F44[1] = y1;
    }
    GsSetDrawBuffClip();
    GsSetDrawBuffOffset();
}

INCLUDE_PAD(8);
