/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/bios.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member bios.o, part 3 of 3: CD_datasync, CD_getsector,
   CD_getsector2, CD_set_test_parmnum, func_800920F8. The first split had it
   in psyq_cd_x.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */


#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: CD_datasync */
/* reorder_fill: CD_datasync */
/* return_slot_fill: CD_datasync */
/* far_syms: D_800B6375 */
extern u8 D_800B6375;
extern s32 D_800B637C[];
extern s32 D_800B63FC[];
extern volatile u8 D_800B6634[];
extern volatile u32 *D_800B6660;    /* -> 0x1F8010B8: DMA channel 3 (CD) CHCR */
extern CdAlarm D_800E7120;
extern char D_80013400[];
extern char D_80013410[];
extern char D_8001350C[];
s32 printf();
void puts();
s32 CD_flush();
s32 VSync();

static inline void cd_set_alarm(char *name)
{
    /* arm the timeout: 960 vertical blanks (16 s at 60 Hz) from now */
    D_800E7120.time = VSync(-1) + 960;
    D_800E7120.count = 0;
    D_800E7120.name = name;
}
static inline int cd_get_alarm(void)
{
    /* -1 once the alarm's VSync count has passed or after 0x3C0000 polls: prints
       "CD timeout: " and "%s:(%s) Sync=%s, Ready=%s" (waiter, command name, the
       two interrupt-state names) and flushes the controller (CD_flush); else 0 */
    if (D_800E7120.time < VSync(-1) || D_800E7120.count++ > 0x3C0000)
    {
        puts(D_80013400);
        printf(D_80013410, D_800E7120.name, D_800B637C[D_800B6375],
                      D_800B63FC[D_800B6634[0]], D_800B63FC[D_800B6634[1]]);
        CD_flush();
        return -1;
    }
    return 0;
}
/* libcd CD_datasync(mode) (BIOS.OBJ): wait for the CD data DMA to finish. Arms the
   960-VSync alarm named "CD_datasync" (D_8001350C), then polls: -1 when the alarm fires
   (cd_get_alarm prints the timeout and flushes the controller); 0 once the busy bit
   0x01000000 of the DMA channel 3 control register (the one D_800B6660 points at,
   0x1F8010B8) is clear; otherwise mode != 0 returns 1 at once (a poll: still
   transferring) and mode == 0 keeps waiting. Called by CdDataSync (libcd_sys_7.c). */
/* func_80091D98 */
s32 CD_datasync(s32 mode)
{
    s32 ret;

    cd_set_alarm(D_8001350C);
    while (1)
    {
        if (cd_get_alarm())
        {
            ret = -1;
            break;
        }
        if (!(*D_800B6660 & 0x01000000))
        {
            ret = 0;
            break;
        }
        if (mode != 0)
        {
            ret = 1;
            break;
        }
    }
    return ret;
}

/* far_syms: D_800B661C (target reaches it %hi/%lo) */
/* far_syms: D_800B6628 (target reaches it %hi/%lo) */
/* far_syms: D_800B662C (target reaches it %hi/%lo) */
/* far_syms: D_800B6650 (target reaches it %hi/%lo) */
/* far_syms: D_800B6654 (target reaches it %hi/%lo) */
/* far_syms: D_800B6658 (target reaches it %hi/%lo) */
/* far_syms: D_800B665C (target reaches it %hi/%lo) */
/* far_syms: D_800B6660 (target reaches it %hi/%lo) */
extern volatile u8 *D_800B661C;     /* reg0 -> 0x1F801800: the CD controller's index / status register */
extern volatile u8 *D_800B6628;     /* reg3 -> 0x1F801803: request / interrupt flag */
extern volatile u32 *D_800B662C;    /* -> 0x1F801020 */
extern volatile u32 *D_800B6650;    /* -> 0x1F801018 */
extern volatile u32 *D_800B6654;    /* -> 0x1F8010F0: DPCR */
extern volatile u32 *D_800B6658;    /* -> 0x1F8010B0: DMA channel 3 (CD) MADR */
extern volatile u32 *D_800B665C;    /* -> 0x1F8010B4: DMA channel 3 BCR */

/* cc97_lib: CD_getsector */
/* PSY-Q libcd CD_getsector(madr, size) (vendor_provided.txt): with the CD
   and DMA registers reached through the pointer table D_800B661C..60,
   select CD register bank 0 and request data (reg3 = 0x80), set the
   delay/DPCR words (0x20943, 0x1323, DPCR |= 0x8000), program the CD DMA
   channel (MADR = madr, BCR = size | 0x10000), wait for the sector-ready
   bit (0x40), start it (CHCR = 0x11000000) and wait for its busy bit
   (0x1000000) to clear; restore 0x1325 and return 0. */
/* func_80091F00 */
s32 CD_getsector(u8 *a0, s32 a1)
{
    *D_800B661C = 0;
    *D_800B6628 = 0x80;
    *D_800B6650 = 0x20943;
    *D_800B662C = 0x1323;
    *D_800B6654 |= 0x8000;
    *D_800B6658 = (u32)a0;
    *D_800B665C = (u32)a1 | 0x10000;

    while ((*D_800B661C & 0x40) == 0)
        ;

    *D_800B6660 = 0x11000000;
    while ((*D_800B6660 & 0x1000000) != 0)
        ;

    *D_800B662C = 0x1325;
    return 0;
}
/* cc97_lib: CD_getsector2 */
/* return_slot_fill: CD_getsector2 */

/* libcd CD_getsector2(madr, size) (BIOS.OBJ): CD_getsector's twin that starts the sector
   DMA and returns without waiting for it. With the CD and DMA registers reached through
   the pointer table D_800B661C..60: select CD register bank 0 and request data
   (reg3 = 0x80), write 0x21020843 to 0x1F801018 and 0x1325 to 0x1F801020 (CD_getsector
   writes 0x20943 and 0x1323 there), DPCR |= 0x8000, program the CD DMA channel
   (MADR = madr, BCR = size | 0x10000), wait for the sector-ready bit (0x40) of register
   0, start the channel with CHCR = 0x11400100 (CD_getsector: 0x11000000) and read CHCR
   back once into a volatile local. Returns 0 with the transfer still running (a caller
   waits with CD_datasync). Called by CdGetSector2 (libcd_sys_6.c:
   `return CD_getsector2() == 0;`). */
/* func_80092000 */
s32 CD_getsector2(u8 *madr, s32 size)
{
    volatile s32 chcr;

    *D_800B661C = 0;
    *D_800B6628 = 0x80;
    *D_800B6650 = 0x21020843;
    *D_800B662C = 0x1325;
    *D_800B6654 |= 0x8000;
    *D_800B6658 = (u32)madr;
    *D_800B665C = (u32)size | 0x10000;

    while ((*D_800B661C & 0x40) == 0)
        ;

    *D_800B6660 = 0x11400100;
    chcr = *D_800B6660;
    return 0;
}

/* cc97_lib: CD_set_test_parmnum */
/* return_slot_fill: CD_set_test_parmnum */
extern s32 D_800B659C[];

/* libcd CD_set_test_parmnum (BIOS.OBJ): set how many parameter bytes CD_cw sends for the
   Test command (0x19): entry 25 of the per-command parameter-count table D_800B659C
   (0x800B6600; the table CD_cw reads to refuse a command without its parameters and to
   count the bytes written to register 2). */
/* func_800920EC */
void CD_set_test_parmnum(s32 parmNum)
{
    D_800B659C[25] = parmNum;
}

/* cc97_lib: func_800920F8 */
/* reorder_fill: func_800920F8 */
/* return_slot_fill: func_800920F8 */
/* far_syms: D_800B6358 D_800B635C D_800B661C */
extern CdlCB D_800B6358;            /* CD_cbsync: the user's sync callback */
extern CdlCB D_800B635C;            /* CD_cbready: the user's ready callback */
extern u8 D_800E7108[];             /* the sync result (8 bytes) */
extern u8 D_800E7110[];             /* the ready result (8 bytes) */
s32 func_80090A68(void);                /* BIOS.OBJ's getintr (libcd_bios_1.c) */

/* libcd callback() (BIOS.OBJ, static; word for word the 4.6 bios_1.o text at +0x14A4): the
   CD-ROM interrupt handler, which CD_initintr and CD_init install with
   InterruptCallback(2, func_800920F8) (libcd_bios_2.c); nothing calls it
   directly. It saves the low two bits (the register bank index) of CD register 0
   (*D_800B661C -> 0x1F801800), then services every pending CD interrupt: func_80090A68() (the
   object's getintr, libcd_bios_1.c) is called until it returns 0, and for each non-zero
   return, bit 2 (4: getintr set the ready state, on a DataReady, DataEnd or DiskError
   cause) calls the user's ready callback D_800B635C (CD_cbready), when one is set, with
   (the ready state D_800B6634[1], the ready result buffer D_800E7110), and bit 1 (2: the
   sync state became Complete or DiskError; an Acknowledge that leaves the command
   running returns 1 and calls nothing) calls the sync callback D_800B6358 (CD_cbsync),
   when set, with (the sync state D_800B6634[0], the sync result D_800E7108); ready
   before sync when a return has both (6, the DiskError cause). Last the saved index goes
   back to register 0, which getintr leaves at 1 or 0. No return value, no frame wait.
   The same text is libcd_bios_1.c's inline cd_callback, which CD_sync, CD_ready and CD_cw
   run by hand when they are themselves inside a callback. The two callback pointers and
   the register pointer are read again before every use (far scalars; the state bytes
   are volatile); the first two instructions, the load of the register pointer, sit
   ahead of the prologue. */
void func_800920F8(void)
{
    int intr;
    u8 reg;

    reg = *D_800B661C & 3;
    while (1)
    {
        intr = func_80090A68();
        if (intr == 0)
        {
            break;
        }
        if ((intr & 4) && D_800B635C != 0)
        {
            D_800B635C(D_800B6634[1], D_800E7110);
        }
        if ((intr & 2) && D_800B6358 != 0)
        {
            D_800B6358(D_800B6634[0], D_800E7108);
        }
    }
    *D_800B661C = reg;
}

INCLUDE_PAD(8);
