/* PSY-Q 4.5 libgs, member gs_001.o, part 1 of 3: GsInitGraph,
   func_8008EB6C. The first split had it in psyq_gpu_h.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: GsInitGraph */
/* gnu_as_loads: GsInitGraph */
/* return_slot_fill: GsInitGraph */
void func_8008EB6C(u_short x, u_short y, u_short intmode, u_short dith, u_short varmmode);  /* TU defines this below */
extern void func_8008ECF0(u_short x, u_short y);
extern void gte_init(void);
extern s16 D_800E6FD4;

/* libgs GsInitGraph(x, y, intmode, dith, varmmode) (GS_001.OBJ): initialises the
   graphics system for an x by y display. In order: the object's GPU half
   (func_8008EB6C: ResetGraph, GsDRAWENV, GsDISPENV), gte_init,
   PSDIDX (D_800E6FD4, the double-buffer index) = 0, the object's variable half
   (func_8008ECF0: HWD0 / VWD0, the identity and light matrices, the buffer
   bases, CLIP2, the two clear primitives), then GsSetDrawBuffClip
    and GsSetDrawBuffOffset. x and y go to both
   halves; intmode, dith and varmmode only to the first. */
/* func_8008EAF8 */
void GsInitGraph(u_short x, u_short y, u_short intmode, u_short dith, u_short varmmode)
{
    func_8008EB6C(x, y, intmode, dith, varmmode);
    gte_init();
    D_800E6FD4 = 0;
    func_8008ECF0(x, y);
    GsSetDrawBuffClip();
    GsSetDrawBuffOffset();
}
/* cc97_lib: func_8008EB6C */
/* gnu_as_loads: func_8008EB6C */
/* return_slot_fill: func_8008EB6C */
extern DRAWENV D_800E6F50;
extern DISPENV D_800E6FB0;
extern short D_800E6FD6;

/* libgs GS_001.OBJ's static after GsInitGraph (gpu_init in the 3.3 to 4.0
   objects): GsInitGraph's GPU half. ResetGraph(3) when intmode's bits 4-5 are both
   set (GsRESET3), else ResetGraph(0). GsDRAWENV (D_800E6F50): ofs and tw zeroed,
   tpage = 0, dtd = dith, dfe = 0, isbg = 0, sent with PutDrawEnv; the clip
   rectangle is left to GsSetDrawBuffClip. GsDISPENV (D_800E6FB0): disp =
   {0, 0, x, y}, screen = {0, 0, 0, 0}, and under PAL (GetVideoMode() == 1)
   screen.y = 24 and pad0 = 1; isinter = intmode & 1 (GsINTER), isrgb24 =
   varmmode; PSDGPU (D_800E6FD6) = intmode & 4 (GsOFSGPU); sent with PutDispEnv.
   The two rectangles are libgpu.h's setRECT, written out. */
void func_8008EB6C(u_short x, u_short y, u_short intmode, u_short dith, u_short varmmode)
{
    ResetGraph(((intmode >> 4) & 3) == 3 ? 3 : 0);
    D_800E6F50.ofs[0] = D_800E6F50.ofs[1] = 0;
    D_800E6F50.tw.x = D_800E6F50.tw.y = D_800E6F50.tw.w = D_800E6F50.tw.h = 0;
    D_800E6F50.tpage = 0;
    D_800E6F50.dtd = dith;
    D_800E6F50.dfe = 0;
    D_800E6F50.isbg = 0;
    PutDrawEnv(&D_800E6F50);
    (&D_800E6FB0.disp)->x = (0), (&D_800E6FB0.disp)->y = (0), (&D_800E6FB0.disp)->w = (x), (&D_800E6FB0.disp)->h = (y);
    (&D_800E6FB0.screen)->x = (0), (&D_800E6FB0.screen)->y = (0), (&D_800E6FB0.screen)->w = (0), (&D_800E6FB0.screen)->h = (0);
    if (GetVideoMode() == 1) {
        D_800E6FB0.screen.y = 0x18;
        D_800E6FB0.pad0 = 1;
    }
    D_800E6FB0.isinter = intmode & 1;
    D_800E6FD6 = intmode & 4;
    D_800E6FB0.isrgb24 = varmmode;
    PutDispEnv(&D_800E6FB0);
}
