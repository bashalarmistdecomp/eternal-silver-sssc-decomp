/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/bios.c)
   and psyz (decomp/src/libcd/bios.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member bios.o, part 2 of 3: CD_initintr, CD_init. The
   first split had it in psyq_cd_w.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: CD_initintr */
/* far_syms: D_800B6358 D_800B635C D_800B6364 D_800B6368 */
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern void func_800920F8(void);        /* BIOS.OBJ's static interrupt callback (libcd_bios_3.c) */
Callback InterruptCallback(s32 irq, Callback f);    /* libetc InterruptCallback */
extern CdlCB D_800B6358;                /* CD_cbsync: the user's sync callback */
extern CdlCB D_800B635C;                /* CD_cbready: the user's ready callback */
extern s32 D_800B6364;                  /* CD_status */
extern s32 D_800B6368;                  /* CD_status1 */

/* libcd CD_initintr (BIOS.OBJ): the CD interrupt set-up CdReset(2) calls
   by itself and CD_init carries in line. Clears the ready and sync callbacks (D_800B635C,
   D_800B6358) and the two status words (D_800B6368, D_800B6364), then ResetCallback()
   and InterruptCallback(2, callback) (IRQ 2 = CD-ROM, func_800920F8 = BIOS.OBJ's static
   callback). */
/* func_80091B6C */
void CD_initintr(void)
{
    D_800B635C = 0;
    D_800B6358 = 0;
    D_800B6368 = 0;
    D_800B6364 = 0;
    ResetCallback();
    InterruptCallback(2, func_800920F8);
}
/* cc97_lib: CD_init */
/* reorder_fill: CD_init */
/* far_syms: D_800B6374 D_800B6375 D_800B661C D_800B6624 D_800B6628 D_800B662C */
extern u8 D_800B6374;
extern u8 D_800B6375;
extern volatile u8 *D_800B661C;         /* -> 0x1F801800, the CD controller's index / status */
extern volatile u8 *D_800B6624;         /* -> 0x1F801802, its register 2 */
extern volatile u8 *D_800B6628;         /* -> 0x1F801803, its register 3 */
extern volatile s32 *D_800B662C;        /* -> 0x1F801020 */
extern volatile u8 D_800B6634[];
extern u8 D_800B6638[];
extern char D_800134F4[];
extern char D_80013500[];
s32 printf();
void puts();
s32 CD_cw();
s32 CD_sync();

/* libcd CD_init (BIOS.OBJ): bring the CD subsystem up. Prints "CD_init:" and
   "addr=%08x" with the address of the object's pointer block D_800B6638; clears CD_com
   D_800B6375 and CD_mode D_800B6374; then CD_initintr written out: the ready and sync
   callbacks D_800B635C / D_800B6358 and the two status words D_800B6368 / D_800B6364
   cleared, ResetCallback(), InterruptCallback(2, callback)
   (IRQ 2 = CD-ROM, func_800920F8 = BIOS.OBJ's callback); then CD_flush
   written out: index register = 1, while the cause register (reg3 & 7) is non-zero
   index = 1, reg3 = 7, reg2 = 7; end and ready states D_800B6634[2] / [1] = 0, sync state
   [0] = 2 (Complete), index = 0, reg3 = 0, 0x1325 to the register D_800B662C points at.
   Then the drive: CD_cw Nop (1) to read the status, a second Nop when
   the status has the lid-open bit (0x10; the first one clears a latched lid change),
   Init (0xA) and Demute (0xC), each -1 on failure, and CD_sync(0, 0) must report
   Complete (2). Returns 0, or -1 on any failure. */
/* func_80091BB8 */
s32 CD_init(void)
{
    puts(D_800134F4);
    printf(D_80013500, D_800B6638);
    D_800B6375 = 0;
    D_800B6374 = 0;
    D_800B635C = 0;
    D_800B6358 = 0;
    D_800B6368 = 0;
    D_800B6364 = 0;
    ResetCallback();
    InterruptCallback(2, func_800920F8);
    *D_800B661C = 1;
    while (*D_800B6628 & 7)
    {
        *D_800B661C = 1;
        *D_800B6628 = 7;
        *D_800B6624 = 7;
    }
    D_800B6634[1] = D_800B6634[2] = 0;
    D_800B6634[0] = 2;
    *D_800B661C = 0;
    *D_800B6628 = 0;
    *D_800B662C = 0x1325;
    CD_cw(1, 0, 0, 0);
    if (D_800B6364 & 0x10)
    {
        CD_cw(1, 0, 0, 0);
    }
    if (CD_cw(0xA, 0, 0, 0))
    {
        return -1;
    }
    if (CD_cw(0xC, 0, 0, 0))
    {
        return -1;
    }
    if (CD_sync(0, 0) != 2)
    {
        return -1;
    }
    return 0;
}
