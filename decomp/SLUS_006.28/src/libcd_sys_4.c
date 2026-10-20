/* PSY-Q 4.5 libcd, member sys.o, part 4 of 7: CdSetDebug, CdComstr,
   CdIntstr. The first split had it in psyq_cd_r.c. The tree compiles this
   run of the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
extern u8 D_800132B4[];
/* far_syms: D_800B6360 (target reaches it %hi/%lo) */

extern s32 D_800B6360;              /* CD_debug */

/* cc97_lib: CdSetDebug */
/* return_slot_fill: CdSetDebug */
/* libcd CdSetDebug(level) (SYS.OBJ): set the debug level CD_debug (D_800B6360) and
   return the old one. */
/* func_80090520 */
s32 CdSetDebug(s32 level)
{
    s32 old;

    old = D_800B6360;
    D_800B6360 = level;
    return old;
}
/* cc97_lib: CdComstr */
/* far_syms: D_800132B4 */
extern char *D_800B637C[];

/* libcd CdComstr(com) (SYS.OBJ): the name of a CD command for debug prints: entry com of
   the 32-pointer name table D_800B637C ("CdlSync", "CdlNop", "CdlSetloc", ...; the table
   BIOS.OBJ's timeout print also reads), or "none" (D_800132B4) for com above 0x1B
   (CdlReadS, the last named command). */
/* func_80090534 */
char *CdComstr(u8 com)
{
    if (com > 0x1B)
    {
        return (char *)D_800132B4;
    }
    return D_800B637C[com];
}

/* far_syms: D_800132B4 (target reaches it %hi/%lo) */
/* far_syms: D_800B63FC (target reaches it %hi/%lo) */
extern char *D_800B63FC[];

/* cc97_lib: CdIntstr */
/* libcd CdIntstr(intr) (SYS.OBJ): the name of a CD interrupt state for debug prints:
   entry intr of the name table D_800B63FC (the one BIOS.OBJ's timeout print reads for
   its Sync= / Ready= fields), or "none" (D_800132B4) for intr above 6. CdComstr's twin. */
/* func_80090568 */
char *CdIntstr(u8 intr)
{
    if (intr > 6)
    {
        return (char *)D_800132B4;
    }
    return D_800B63FC[intr];
}
