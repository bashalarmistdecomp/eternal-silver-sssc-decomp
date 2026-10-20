/* PSY-Q 4.5 libcd, member sys.o, part 7 of 7: CdDataCallback, CdDataSync.
   The first split had it in psyq_cd_u.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: CdDataCallback */
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
Callback DMACallback(s32 dma, Callback f);          /* libetc DMACallback */

/* libcd CdDataCallback (SYS.OBJ): installs `func` as the callback of DMA channel 3, the
   CD-ROM's data channel (DMACallback(3, func)), and returns
   DMACallback's result. The callers in libcd_cdread_2.c and libcd_cdread2_1.c pass 0 to remove the callback, keep
   the result and install it again afterwards. */
/* func_80090A20 */
void *CdDataCallback(void (*func)())
{
    return (void *)DMACallback(3, func);
}
/* cc97_lib: CdDataSync */
s32 CD_datasync(s32 mode);

/* libcd CdDataSync (SYS.OBJ): the wait for the end of a sector data transfer: returns
   BIOS.OBJ's CD_datasync(mode) (libcd_bios_3.c). */
/* func_80090A44 */
s32 CdDataSync(s32 mode)
{
    return CD_datasync(mode);
}
INCLUDE_PAD(4);
