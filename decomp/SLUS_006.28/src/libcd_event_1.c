/* PSY-Q 4.5 libcd, member event.o, part 1 of 2: CdInit. The first split had
   it in psyq_cd_n.c. The tree compiles this run of the member's functions
   with other flags than the runs beside it (config/cc_flags.txt), so the
   member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: CdGetToc2 */
/* reorder_fill: CdGetToc2 */
/* return_slot_fill: CdGetToc2 */
s32 printf();

/* cc97_lib: CdInit */
/* reorder_fill: CdInit */
/* return_slot_fill: CdInit */
/* far_syms: D_800B6358 D_800B635C D_800B62C0 D_800B62C4 */
extern CdlCB D_800B6358;            /* CD_cbsync */
extern CdlCB D_800B635C;            /* CD_cbready */
extern CdlCB D_800B62C0;            /* CD_cbread */
extern s32 D_800B62C4;              /* CD_read_dma_mode */
extern char D_80013294[];
s32 func_80090388(void);
void def_cbsync(u_char intr, u_char *result);     /* EVENT.OBJ's default sync callback */
void func_800903EC(u_char intr, u_char *result);     /* its default ready callback */
void func_80090414(u_char intr, u_char *result);     /* its default read callback */

/* libcd CdInit (EVENT.OBJ): brings the CD subsystem up, five tries. Each try is the
   object's static reset func_80090388 (CD_init, then CD_initvol; 1 when both succeed).
   On the first success it installs the object's default callbacks, which deliver the
   BIOS CD-ROM events: CD_cbsync (D_800B6358) = def_cbsync, CD_cbready (D_800B635C) =
   func_800903EC, CD_cbread (D_800B62C0) = func_80090414, clears CD_read_dma_mode
   (D_800B62C4) and returns 1. After five failures it prints "CdInit: Init failed"
   (D_80013294, printf) and returns 0. */
/* func_800902F8 */
s32 CdInit(void)
{
    s32 i;

    i = 4;
    while (func_80090388() != 1) {
        if (--i == -1) {
            printf(D_80013294);
            return 0;
        }
    }
    D_800B6358 = def_cbsync;
    D_800B635C = func_800903EC;
    D_800B62C0 = func_80090414;
    D_800B62C4 = 0;
    return 1;
}
