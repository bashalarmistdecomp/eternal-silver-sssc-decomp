/* PSY-Q 4.5 libgs, member gs_001.o, part 2 of 3: GsInitGraph2. The first
   split had it in psyq_gpu_i.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: GsInitGraph2 */
/* gnu_as_loads: GsInitGraph2 */
extern DRAWENV D_800E6F50;
extern DISPENV D_800E6FB0;
extern void func_8008ECF0(u_short x, u_short y);
extern short D_800E6FD6;

/* libgs GsInitGraph2(x, y, intmode, dith, varmmode) (GS_001.OBJ): GsInitGraph
   without the GPU reset and without gte_init. It only re-sets the members of the
   two environments that the arguments decide, and sends neither to the GPU:
   GsDRAWENV (D_800E6F50) tpage = 0, dtd = dith, dfe = 0, isbg = 0; GsDISPENV
   (D_800E6FB0) disp.w = x, disp.h = y, isinter = intmode & 1 (GsINTER), isrgb24
   = varmmode; PSDGPU (D_800E6FD6) = intmode & 4 (GsOFSGPU). Then the object's
   variable half (func_8008ECF0) with x and y. */
/* func_8008EC88 */
void GsInitGraph2(u_short x, u_short y, u_short intmode, u_short dith, u_short varmmode)
{
    D_800E6F50.tpage = 0;
    D_800E6F50.dtd = dith;
    D_800E6F50.dfe = 0;
    D_800E6F50.isbg = 0;
    D_800E6FB0.disp.w = x;
    D_800E6FB0.disp.h = y;
    D_800E6FB0.isinter = intmode & 1;
    D_800E6FD6 = intmode & 4;
    D_800E6FB0.isrgb24 = varmmode;
    func_8008ECF0(x, y);
}
