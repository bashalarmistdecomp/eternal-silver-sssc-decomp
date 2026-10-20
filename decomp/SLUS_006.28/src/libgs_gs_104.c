/* PSY-Q 4.5 libgs, member gs_104.o: GsInit3D. The first split had it in
   psyq_cd_g.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: GsSwapDispBuff */
/* gnu_as_loads: GsSwapDispBuff */
/* cc97_lib: GsInit3D */
extern long D_800E6FD8;
extern long D_800E6FDC;
extern _GsPOSITION D_800E6F48[];
extern long D_800E6FE0;
extern int D_800E6FE4;
extern long D_800E6FE8;

/* libgs GsInit3D (GS_104.OBJ): sets the 3D defaults. The 2D offset POSITION
   (D_800E6F48) becomes the screen centre, HWD0 / 2 and VWD0 / 2 (D_800E6FD8 /
   D_800E6FDC), and GsSetDrawBuffOffset applies it; then the near
   clip GsCLIP3near (D_800E6FE8) = 10, the lighting mode GsLIGHT_MODE (D_800E6FE4)
   = 0 and the far clip GsCLIP3far (D_800E6FE0) = 0x3FFF. */
/* func_8008F578 */
void GsInit3D(void)
{
    D_800E6F48[0].offx = D_800E6FD8 / 2;
    D_800E6F48[0].offy = D_800E6FDC / 2;
    GsSetDrawBuffOffset();
    D_800E6FE8 = 10;
    D_800E6FE4 = 0;
    D_800E6FE0 = 0x3FFF;
}
INCLUDE_PAD(12);
