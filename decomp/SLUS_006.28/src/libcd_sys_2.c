/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/sys.c)
   and psyz (decomp/src/libcd/sys.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member sys.o, part 2 of 7: CdLastPos, CdReset. The first
   split had it in psyq_cd_p.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
extern u8 D_800B6370[];

/* func_80090488 */
CdlLOC *CdLastPos(void) {
    return (CdlLOC *)D_800B6370;
}
/* cc97_lib: CdReset */
/* return_slot_fill: CdReset */
void CD_initintr(void);
s32 CD_init(void);
s32 CD_initvol(void);

/* libcd CdReset(mode) (SYS.OBJ): initialise the CD subsystem. mode 2 (CdlModeAP) only
   re-installs the interrupt handler (CD_initintr) and returns 1. Otherwise
   CD_init() resets the controller and the drive (non-zero: return 0), and mode 1
   (CdlModeDA) also opens the CD audio path through the SPU (CD_initvol;
   non-zero: return 0). Returns 1 on success. */
/* func_80090494 */
s32 CdReset(s32 mode)
{
    if (mode == 2)
    {
        CD_initintr();
        return 1;
    }
    if (CD_init())
    {
        return 0;
    }
    if (mode == 1 && CD_initvol())
    {
        return 0;
    }
    return 1;
}
