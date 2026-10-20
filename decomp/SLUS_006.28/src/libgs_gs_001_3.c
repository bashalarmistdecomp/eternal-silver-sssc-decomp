/* PSY-Q 4.5 libgs, member gs_001.o, part 3 of 3: func_8008ECF0,
   GsSortClear. The first split had it in psyq_gpu_j.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: func_8008ECF0 */
/* gnu_as_loads: func_8008ECF0 */
extern TILE D_800E6F18[];
extern short D_800E6F40[];
extern short D_800E6F44[];
extern _GsPOSITION D_800E6F48[];
extern RECT D_800E6FC8[];
extern u_long D_800E6FD0;
extern long D_800E6FD8;
extern long D_800E6FDC;
extern MATRIX D_800E6FF0;
extern MATRIX D_800E7010;
extern MATRIX D_800E7050;
extern MATRIX D_800E7090;

/* libgs GS_001.OBJ's static after GsInitGraph2 (valiable_init in the 3.3 to 4.0
   objects): the libgs variables a screen size decides, for GsInitGraph and
   GsInitGraph2. HWD0 / VWD0 (D_800E6FD8 / D_800E6FDC) = x / y. GsIDMATRIX
   (D_800E7050) = the unit matrix (4096 on the diagonal, no translation);
   GsIDMATRIX2 (D_800E7090) = the same with m[1][1] = 4096 * 4 * VWD0 / HWD0 / 3,
   the aspect-corrected unit matrix (4096 for 320 x 240); GsLIGHTWSMATRIX
   (D_800E6FF0) = the unit matrix with a zero diagonal, i.e. all zero, and _LC
   (D_800E7010), the local colour matrix, a copy of that. PSDBASEX / PSDBASEY
   (D_800E6F40 / D_800E6F44) and POSITION (D_800E6F48) = 0; CLIP2 (D_800E6FC8) =
   {0, 0, HWD0, VWD0}; both of the object's screen-clear primitives (D_800E6F18,
   GsSortClear's) get length 3 and GPU command 2, the frame-buffer fill; PSDCNT
   (D_800E6FD0) = 1. The primitive lines are libgpu.h's setlen / setcode. */
void func_8008ECF0(u_short x, u_short y)
{
    D_800E6FD8 = x;
    D_800E6FDC = y;
    D_800E7050.m[2][2] = D_800E7050.m[1][1] = D_800E7050.m[0][0] = 4096;
    D_800E7050.m[0][1] = D_800E7050.m[0][2] = 0;
    D_800E7050.m[1][0] = D_800E7050.m[1][2] = 0;
    D_800E7050.m[2][0] = D_800E7050.m[2][1] = 0;
    D_800E7050.t[0] = D_800E7050.t[1] = D_800E7050.t[2] = 0;
    D_800E7090 = D_800E7050;
    D_800E6FF0 = D_800E7050;
    D_800E6FF0.m[0][0] = D_800E6FF0.m[1][1] = D_800E6FF0.m[2][2] = 0;
    D_800E7010 = D_800E6FF0;
    D_800E7090.m[1][1] = 4096 * 4 * D_800E6FDC / D_800E6FD8 / 3;
    D_800E6F40[0] = 0;
    D_800E6F40[1] = 0;
    D_800E6F44[0] = 0;
    D_800E6F44[1] = 0;
    D_800E6F48->offx = D_800E6F48->offy = 0;
    D_800E6FC8->x = D_800E6FC8->y = 0;
    D_800E6FC8->w = D_800E6FD8;
    D_800E6FC8->h = D_800E6FDC;
    (((P_TAG *)(&D_800E6F18[0]))->len = (u_char)(3));
    (((P_TAG *)(&D_800E6F18[0]))->code = (u_char)(2));
    (((P_TAG *)(&D_800E6F18[1]))->len = (u_char)(3));
    (((P_TAG *)(&D_800E6F18[1]))->code = (u_char)(2));
    D_800E6FD0 = 1;
}

/* cc97_lib: GsSortClear */
/* gnu_as_loads: GsSortClear */
extern TILE D_800E6F18[];
extern short D_800E6F38[];
extern short D_800E6F3C[];
extern DISPENV D_800E6FB0;
extern short D_800E6FD4;
extern long D_800E6FD8;
extern long D_800E6FDC;
extern void AddPrim(void *ot, void *p);

/* libgs GsSortClear(r, g, b, otp) (GS_001.OBJ): registers a screen clear in the
   ordering table. The draw buffer's own primitive (D_800E6F18[PSDIDX]) gets the
   colour, the buffer's frame-buffer origin (PSDOFSX/PSDOFSY[PSDIDX]) and the
   display size (HWD0 x VWD0; the width times 3/2 in 24-bit mode,
   GsDISPENV.isrgb24), and is linked at otp->tag with AddPrim. The pair has TILE's
   layout, but func_8008ECF0 (GsInitGraph's setup, same object) gives both length 3
   and GPU command 2, the frame-buffer rectangle fill, so x0/y0 are VRAM
   coordinates. */
/* func_8008EF10 */
void GsSortClear(u_char r, u_char g, u_char b, GsOT *otp)
{
    D_800E6F18[D_800E6FD4].r0 = r;
    D_800E6F18[D_800E6FD4].g0 = g;
    D_800E6F18[D_800E6FD4].b0 = b;
    D_800E6F18[D_800E6FD4].x0 = D_800E6F38[D_800E6FD4];
    D_800E6F18[D_800E6FD4].y0 = D_800E6F3C[D_800E6FD4];
    D_800E6F18[D_800E6FD4].h = D_800E6FDC;
    if (D_800E6FB0.isrgb24) {
        D_800E6F18[D_800E6FD4].w = D_800E6FD8 * 3 / 2;
    } else {
        D_800E6F18[D_800E6FD4].w = D_800E6FD8;
    }
    AddPrim(otp->tag, &D_800E6F18[D_800E6FD4]);
}
