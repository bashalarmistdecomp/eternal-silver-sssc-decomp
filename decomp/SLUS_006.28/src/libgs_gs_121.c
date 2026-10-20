/* PSY-Q 4.5 libgs, member gs_121.o: gte_init. The first split had it in
   psyq_cd_k.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: GsDrawOt */

/* cc97_lib: gte_init */
extern void InitGeom();                              /* libgte InitGeom */
extern void SetFarColor(long rfc, long gfc, long bfc);  /* libgte SetFarColor */
extern void SetGeomOffset(long ofx, long ofy);            /* libgte SetGeomOffset */
extern s16 D_800E6FC4;                                    /* GsORGOFSX */
extern s16 D_800E6FC6;                                    
/* libgs gte_init (GS_121.OBJ; the name is the 4.6 object's symbol): the GTE set-up
   GsInitGraph makes. InitGeom, SetFarColor(0, 0, 0)
   , SetGeomOffset(0, 0), then the screen origin offsets
   GsORGOFSY (D_800E6FC6) and GsORGOFSX (D_800E6FC4) cleared. */
/* func_8008FBF8 */
void gte_init(void)
{
    InitGeom();
    SetFarColor(0, 0, 0);
    SetGeomOffset(0, 0);
    D_800E6FC6 = 0;
    D_800E6FC4 = 0;
}

INCLUDE_PAD(4);
