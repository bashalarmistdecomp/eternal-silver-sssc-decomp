/* PSY-Q 4.5 libcd, member c_004.o, part 2 of 2: StGetBackloc. The first
   split had it in psyq_spu_c.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern CdlLOC D_800E9560[];         /* C_004.OBJ .bss+0, static: the loc of the last completed frame's first sector
                                       (one CdlLOC; unsized so cc1 does not treat it as small data) */
CdlLOC *CdIntToPos(int i, CdlLOC *p);
extern s32 D_800E7138;              /* StMode */
extern s32 D_800E9564;              /* C_004.OBJ .bss+4, static: that frame's number */

/* cc97_lib: StGetBackloc */
/* return_slot_fill: StGetBackloc */
/* reorder_fill: StGetBackloc */
/* libcd StGetBackloc(loc) (C_004.OBJ): where the frame after the last completed one
   starts. -1 when StMode (D_800E7138) is not 0. Otherwise *loc = the position one sector
   past the loc data_ready_callback kept of the last completed frame
   (CdIntToPos(CdPosToInt(kept) + 1, loc)), and the return is that frame's number. */
/* func_80093A64 */
s32 StGetBackloc(CdlLOC *loc)
{
    if (D_800E7138 != 0) {
        return -1;
    }
    CdIntToPos(CdPosToInt(D_800E9560) + 1, loc);
    return D_800E9564;
}

INCLUDE_PAD(12);
