/* PSY-Q 4.5 libcd, member sys.o, part 1 of 7: CdStatus, CdMode, CdLastCom.
   The first split had it in psyq_cd_o.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* far_syms: D_800B6364 (target reaches it %hi/%lo) */
/* cc97_lib: CdStatus */

extern u8 D_800B6364;       
/* libcd CdStatus (SYS.OBJ): the drive status byte as the CD interrupt service (func_80090A68,
   libcd_bios_1.c) last recorded it, CD_status (D_800B6364). */
/* func_80090458 */
s32 CdStatus(void)
{
    return D_800B6364;
}
/* far_syms: D_800B6374 (target reaches it %hi/%lo) */
/* cc97_lib: CdMode */

extern u8 D_800B6374;       /* CD_mode */

/* libcd CdMode (SYS.OBJ): the mode byte of the last Setmode command CD_cw sent, CD_mode
   (D_800B6374). */
/* func_80090468 */
s32 CdMode(void)
{
    return D_800B6374;
}
/* far_syms: D_800B6375 (target reaches it %hi/%lo) */
/* cc97_lib: CdLastCom */

extern u8 D_800B6375;       /* CD_com */

/* libcd CdLastCom (SYS.OBJ): the last command CD_cw sent, CD_com
   (D_800B6375). */
/* func_80090478 */
s32 CdLastCom(void)
{
    return D_800B6375;
}
