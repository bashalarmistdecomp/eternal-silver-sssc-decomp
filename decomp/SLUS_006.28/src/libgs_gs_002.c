/* PSY-Q 4.5 libgs, member gs_002.o: GsGetActiveBuff, GsSetDrawBuffOffset.
   The first split had it in psyq_cd.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
extern s16 D_800E6FD4;

/* cc97_lib: GsGetActiveBuff */
/* libgs GsGetActiveBuff (GS_002.OBJ): returns PSDIDX (D_800E6FD4), the index of the
   current draw buffer. */
/* func_8008F048 */
int GsGetActiveBuff(void) {
    return D_800E6FD4;
}


/* cc97_lib: GsSetDrawBuffOffset */
/* gnu_as_loads: GsSetDrawBuffOffset */
/* return_slot_fill: GsSetDrawBuffOffset */
extern s16 D_800E6FD6;
extern s16 D_800E6FD4;
extern s16 D_800E6F38[];
extern s16 D_800E6F3C[];
extern _GsPOSITION D_800E6F48[];
extern DRAWENV D_800E6F50;
extern s16 D_800E6FC4;
extern s16 D_800E6FC6;
extern void SetGeomOffset(long ofx, long ofy);

/* libgs GsSetDrawBuffOffset (GS_002.OBJ): applies the 2D drawing offset POSITION
   (D_800E6F48: offx, offy; unsized here so cc1 does not take it for small data) to
   the current draw buffer. With PSDGPU set (D_800E6FD6: the offset is done by the
   GPU) GsORGOFSX/Y (D_800E6FC4 / D_800E6FC6) are 0 and GsDRAWENV.ofs (D_800E6F50) =
   POSITION + the draw buffer's own origin PSDOFSX/Y[PSDIDX] (D_800E6F38 /
   D_800E6F3C by D_800E6FD4), sent with PutDrawEnv. Otherwise the
   offset goes to the GTE: SetGeomOffset with POSITION + the origin
   of buffer (PSDIDX == 0 ? 1 : 0), and GsORGOFSX/Y remember it. */
/* func_8008F058 */
void GsSetDrawBuffOffset(void)
{
    if (D_800E6FD6) {
        D_800E6FC4 = D_800E6FC6 = 0;
        D_800E6F50.ofs[0] = D_800E6F48[0].offx + D_800E6F38[D_800E6FD4];
        D_800E6F50.ofs[1] = D_800E6F48[0].offy + D_800E6F3C[D_800E6FD4];
        PutDrawEnv(&D_800E6F50);
    } else {
        long x, y;

        x = D_800E6F48[0].offx + D_800E6F38[D_800E6FD4 == 0 ? 1 : 0];
        y = D_800E6F48[0].offy + D_800E6F3C[D_800E6FD4 == 0 ? 1 : 0];
        SetGeomOffset(x, y);
        D_800E6FC4 = x;
        D_800E6FC6 = y;
    }
}
INCLUDE_PAD(8);
