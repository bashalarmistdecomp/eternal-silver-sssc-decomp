/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/bios.c)
   and psyz (decomp/src/libcd/bios.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member bios.o, part 1 of 3: func_80090A68, CD_sync,
   CD_ready, CD_cw, CD_vol, CD_flush, CD_initvol. The first split had it in
   psyq_cd_v.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* far_syms: D_800B6360 */
/* far_syms: D_800B6364 */
/* far_syms: D_800B6368 */
/* far_syms: D_800B636C */
/* far_syms: D_800B6375 */
/* far_syms: D_800B6634 */
/* cc97_lib: func_80090A68 */
/* gnu_as_loads: func_80090A68 */
/* reorder_fill: func_80090A68 */
/* return_slot_fill: func_80090A68 */
extern volatile u8 *D_800B661C;     /* reg0 -> 0x1F801800: the CD controller's index / status register */
extern volatile u8 *D_800B6620;     /* reg1 -> 0x1F801801: command / response FIFO */
extern volatile u8 *D_800B6624;     /* reg2 -> 0x1F801802: parameter FIFO / interrupt enable */
extern volatile u8 *D_800B6628;     /* reg3 -> 0x1F801803: request / interrupt flag */
extern s32 D_800B6360;
extern s32 D_800B6364;
extern s32 D_800B6368;
extern s32 D_800B636C;
extern u8 D_800B6375;
extern char *D_800B637C[];
extern s32 D_800B641C[];
extern s32 D_800B651C[];
extern volatile u8 D_800B6634[];
extern u8 D_800E7108[];
extern u8 D_800E7110[];
extern u8 D_800E7118[];
extern char D_8001342C[];
extern char D_80013438[];
extern char D_80013454[];
extern char D_80013468[];
s32 printf();
void puts();

static inline void cd_memcpy(void *dst, void *src, u32 n)
{
    /* BIOS.OBJ's inlined result copy: nothing for a null destination */
    u8 *d = (u8 *)dst;
    u8 *s = (u8 *)src;

    if (d == 0)
    {
        return;
    }
    while (n--)
    {
        *d++ = *s++;
    }
}
/* func_80090A68 — libcd's CD-ROM interrupt service, named for its printf. The
 * cause is reg3 & 7 (*D_800B6628), re-read until two reads agree, and
 * the response FIFO (reg1, while reg0 bit 5 says it is non-empty) is
 * drained into an 8-byte result, zero-padded. Then the controller is
 * acknowledged (reg0 = 1, reg3 = reg2 = 7). Unless the cause is an
 * Acknowledge (3) of a command whose D_800B651C flag is clear, result[0]/
 * [1] become the drive status D_800B6364/D_800B6368, a fresh lid-open bit
 * (0x10) counts in D_800B636C, and status & 0x1D is the error flag. A
 * DiskError (5) prints "DiskError: com=%s,code=(%02x:%02x)" at debug
 * level D_800B6360 >= 3, the command named by D_800B637C[D_800B6375].
 * Per cause, the sync/ready/end state bytes D_800B6634[0..2] are set
 * (2 Complete, 3 Acknowledge, 1 DataReady, 4 DataEnd, 5 error), the
 * result is copied to the sync (D_800E7108), ready (D_800E7110) or end
 * (D_800E7118) buffer; the return is the case's code (0 = no interrupt
 * or an unknown cause, which prints "CDROM: unknown intr(%d)"). This is
 * BIOS.OBJ's getintr (sotn-decomp's bios.c has the text); each result copy
 * is the object's inline memcpy (cd_memcpy). */
s32 func_80090A68(void)
{
    volatile u8 intr;
    volatile u8 result[8];
    s32 i;
    s32 j;
    s32 err;

    *D_800B661C = 1;
    intr = *D_800B6628 & 7;
    err = 0;
    if (intr == 0)
    {
        return 0;
    }
    while (intr != (*D_800B6628 & 7))
    {
        intr = *D_800B6628 & 7;
    }
    for (i = 0; i < 8; i++)
    {
        if (!(*D_800B661C & 0x20))
        {
            break;
        }
        result[i] = *D_800B6620;
    }
    for (j = i; j < 8; j++)
    {
        result[j] = 0;
    }
    *D_800B661C = 1;
    *D_800B6628 = 7;
    *D_800B6624 = 7;
    if ((intr != 3) || (D_800B651C[D_800B6375] != 0))
    {
        if (!(D_800B6364 & 0x10) && (result[0] & 0x10))
        {
            D_800B636C++;
        }
        D_800B6364 = result[0];
        D_800B6368 = result[1];
        err = D_800B6364 & 0x1D;
    }
    if ((intr == 5) && (D_800B6360 >= 3))
    {
        printf(D_8001342C);
        if (D_800B6360 >= 3)
        {
            printf(D_80013438, D_800B637C[D_800B6375], D_800B6364,
                          D_800B6368);
        }
    }
    switch (intr)
    {
    case 3:
        if (err)
        {
            D_800B6634[0] = 5;
            cd_memcpy(D_800E7108, result, 8);
            return 2;
        }
        if (D_800B641C[D_800B6375])
        {
            D_800B6634[0] = 3;
            cd_memcpy(D_800E7108, result, 8);
            return 1;
        }
        D_800B6634[0] = 2;
        cd_memcpy(D_800E7108, result, 8);
        return 2;
    case 2:
        D_800B6634[0] = err ? 5 : 2;
        cd_memcpy(D_800E7108, result, 8);
        return 2;
    case 1:
        if (err && (i == 1))
        {
            err = 0;
        }
        D_800B6634[1] = err ? 5 : 1;
        cd_memcpy(D_800E7110, result, 8);
        *D_800B661C = 0;
        *D_800B6628 = 0;
        return 4;
    case 4:
        D_800B6634[1] = D_800B6634[2] = 4;
        cd_memcpy(D_800E7118, result, 8);
        cd_memcpy(D_800E7110, result, 8);
        return 4;
    case 5:
        D_800B6634[0] = D_800B6634[1] = 5;
        cd_memcpy(D_800E7108, result, 8);
        cd_memcpy(D_800E7110, result, 8);
        return 6;
    default:
        puts(D_80013454);
        printf(D_80013468, intr);
        return 0;
    }
}

/* cc97_lib: CD_cw */
/* gnu_as_loads: CD_cw */
/* reorder_fill: CD_cw */
/* return_slot_fill: CD_cw */
/* far_syms: D_800B6358 D_800B635C D_800B6360 D_800B6374 D_800B6375 */
/* far_syms: D_800B661C D_800B6620 D_800B6624 */
extern CdlCB D_800B6358;            /* CD_cbsync: the user's sync callback */
extern CdlCB D_800B635C;            /* CD_cbready: the user's ready callback */
extern u8 D_800B6370[];
extern u8 D_800B6374;
extern s32 D_800B63FC[];
extern s32 D_800B649C[];
extern s32 D_800B659C[];
extern CdAlarm D_800E7120;
extern char D_80013400[];
extern char D_80013410[];
extern char D_8001349C[];
extern char D_800134A4[];
extern char D_800134B4[];
s32 CD_sync();
s32 CD_flush();
s32 VSync();
s32 CheckCallback();

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
static inline void cd_callback(void)
{
    int intr;
    u8 reg;

    /* BIOS.OBJ's interrupt callback, run by hand when CheckCallback() says the
       caller is itself inside a callback (interrupts cannot arrive): service
       every pending CD interrupt (func_80090A68 = the object's getintr) and call the
       user's ready / sync callbacks; the index register's low two bits are
       saved and restored around it */
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
/* cc97_lib: CD_sync */
/* reorder_fill: CD_sync */
/* return_slot_fill: CD_sync */
extern char D_80013488[];

/* libcd CD_sync(mode, result) (BIOS.OBJ): wait for the command in flight to finish.
   Arms the 960-VSync alarm named "CD_sync" (D_80013488), then polls: -1 when the alarm
   fires (cd_get_alarm prints the timeout and flushes the controller); the interrupt
   callback is run by hand when CheckCallback() says interrupts cannot
   arrive; once the sync state D_800B6634[0] is Complete (2) or DiskError (5) it is reset
   to Complete, the 8 result bytes D_800E7108 are copied to result (if not null) and that
   state is returned. Otherwise mode != 0 returns 0 at once (a poll: still busy) and
   mode == 0 keeps waiting. */
/* func_80090FCC */
s32 CD_sync(s32 mode, u_char *result)
{
    s32 sync;

    cd_set_alarm(D_80013488);
    while (1)
    {
        if (cd_get_alarm())
        {
            return -1;
        }
        if (CheckCallback())
        {
            cd_callback();
        }
        sync = D_800B6634[0];
        if (sync == 2 || sync == 5)
        {
            D_800B6634[0] = 2;
            cd_memcpy(result, D_800E7108, 8);
            return sync;
        }
        if (mode != 0)
        {
            return 0;
        }
    }
}
/* cc97_lib: CD_ready */
/* reorder_fill: CD_ready */
/* return_slot_fill: CD_ready */
extern char D_80013490[];

/* libcd CD_ready(mode, result) (BIOS.OBJ): wait for data from the drive.
   Arms the 960-VSync alarm named "CD_ready" (D_80013490), then polls: -1 when the alarm
   fires (cd_get_alarm prints the timeout and flushes the controller); the interrupt
   callback is run by hand when CheckCallback() says interrupts cannot
   arrive. A pending end state D_800B6634[2] (DataEnd 4, set by the interrupt service
   together with the ready state) is taken first: cleared, the 8 bytes of the end result
   buffer D_800E7118 copied to result (if not null), and returned. Else a non-zero ready
   state D_800B6634[1] (DataReady 1, DataEnd 4 or DiskError 5) is cleared, the ready
   result buffer D_800E7110 copied, and returned. Otherwise mode != 0 returns 0 at once
   (a poll: nothing yet) and mode == 0 keeps waiting. */
/* func_8009124C */
s32 CD_ready(s32 mode, u_char *result)
{
    s32 c;
    s32 ready;

    cd_set_alarm(D_80013490);
    while (1)
    {
        if (cd_get_alarm())
        {
            return -1;
        }
        if (CheckCallback())
        {
            cd_callback();
        }
        c = D_800B6634[2];
        if (c != 0)
        {
            D_800B6634[2] = 0;
            cd_memcpy(result, D_800E7118, 8);
            return c;
        }
        ready = D_800B6634[1];
        if (ready != 0)
        {
            D_800B6634[1] = 0;
            cd_memcpy(result, D_800E7110, 8);
            return ready;
        }
        if (mode != 0)
        {
            return 0;
        }
    }
}

/* libcd CD_cw(com, param, result, arg3) (BIOS.OBJ): write one command to the CD
   controller and, unless arg3 is set, wait for its result. Debug level D_800B6360
   >= 2 prints "%s..." with the command's name (D_800B637C[com]). A command whose
   D_800B659C entry (its parameter count) is non-zero needs param: without it, -2
   (and "%s: no param" at debug level 1). Then CD_sync(0, 0) waits out the previous
   command; Setloc (2) remembers its four parameter bytes in CD_pos D_800B6370 and
   Setmode (0xE) its mode byte in CD_mode D_800B6374; the sync state D_800B6634[0]
   is cleared, the ready state [1] too for a command flagged in D_800B649C; index
   register = 0, the parameter bytes go to register 2 (the count is read as
   D_800B649C[com + 0x40], which is D_800B659C[com]), CD_com D_800B6375 = com, and
   the command byte to register 1 starts it. arg3 != 0 returns 0 here without waiting.
   Otherwise it polls with a 960-VSync alarm named "CD_cw" (-1 on timeout) until
   the interrupt service sets the sync state, running the interrupt callback by
   hand when CheckCallback() says interrupts cannot arrive; then
   the 8 result bytes D_800E7108 are copied to result (if not null) and the
   return is -1 for DiskError (5), else 0. */
/* func_80091514 */
int CD_cw(u_char com, u_char *param, u_char *result, int arg3)
{
    int i;

    if (D_800B6360 > 1)
    {
        printf(D_8001349C, D_800B637C[com]);
    }
    if (D_800B659C[com] != 0 && param == 0)
    {
        if (D_800B6360 > 0)
        {
            printf(D_800134A4, D_800B637C[com]);
        }
        return -2;
    }
    CD_sync(0, 0);
    if (com == 2)
    {
        for (i = 0; i < 4; i++)
        {
            D_800B6370[i] = param[i];
        }
    }
    if (com == 0xE)
    {
        D_800B6374 = *param;
    }
    D_800B6634[0] = 0;
    if (D_800B649C[com])
    {
        D_800B6634[1] = 0;
    }
    *D_800B661C = 0;
    for (i = 0; i < D_800B649C[com + 0x40]; i++)
    {
        *D_800B6624 = param[i];
    }
    D_800B6375 = com;
    *D_800B6620 = com;
    if (arg3 != 0)
    {
        return 0;
    }
    cd_set_alarm(D_800134B4);
    while (D_800B6634[0] == 0)
    {
        if (cd_get_alarm())
        {
            return -1;
        }
        if (CheckCallback())
        {
            cd_callback();
        }
    }
    cd_memcpy(result, D_800E7108, 8);
    return -(D_800B6634[0] == 5);
}

/* far_syms: D_800B661C (target reaches it %hi/%lo) */
/* far_syms: D_800B6620 (target reaches it %hi/%lo) */
/* far_syms: D_800B6624 (target reaches it %hi/%lo) */
/* far_syms: D_800B6628 (target reaches it %hi/%lo) */

/* cc97_lib: CD_vol */
/* libcd CD_vol(vol) (BIOS.OBJ): write the four CdlATV mixer bytes to the CD controller:
   index 2, reg2 = val0, reg3 = val1; index 3, reg1 = val2, reg2 = val3, reg3 = 0x20
   (apply). Returns 0. CdMix calls it. */
/* func_80091920 */
s32 CD_vol(CdlATV *vol)
{
    *D_800B661C = 2;
    *D_800B6624 = vol->val0;
    *D_800B6628 = vol->val1;
    *D_800B661C = 3;
    *D_800B6620 = vol->val2;
    *D_800B6624 = vol->val3;
    *D_800B6628 = 0x20;
    return 0;
}
/* cc97_lib: CD_flush */
/* return_slot_fill: CD_flush */
/* far_syms: D_800B662C */
extern volatile s32 *D_800B662C;    /* -> 0x1F801020 */

/* libcd CD_flush (BIOS.OBJ): drop every pending CD interrupt and reset the object's
   interrupt state. Index register = 1; while the cause register (reg3 & 7) is non-zero:
   index = 1, reg3 = 7 (acknowledge all causes), reg2 = 7 (enable them). Then the end and
   ready states D_800B6634[2] / [1] are cleared, the sync state [0] becomes 2 (Complete, "no
   command outstanding"), index = 0, reg3 = 0, and 0x1325 goes to the register D_800B662C
   points at. Called by the alarm (cd_get_alarm) after a "CD timeout", and by CD_init's
   sibling CdFlush. Declared s32 like the SDK's `int CD_flush()`; it returns nothing. */
/* func_800919A8 */
s32 CD_flush(void)
{
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
}
/* cc97_lib: CD_initvol */
/* return_slot_fill: CD_initvol */
/* far_syms: D_800B6630 */
extern volatile SpuRXX *D_800B6630;

/* libcd CD_initvol (BIOS.OBJ): open the CD audio path at full volume. Through the SPU
   register pointer D_800B6630: if the current main volume (0x1B8 / 0x1BA) reads zero on
   both sides, the main volume (0x180 / 0x182) is set to 0x3FFF; the CD input volume
   (0x1B0 / 0x1B2) is set to 0x3FFF and SPUCNT (0x1AA) to 0xC001 (SPU enable, unmute, CD
   audio enable). Then the CD controller's audio mixer gets the identity CdlATV
   {0x80, 0, 0x80, 0}: index register = 2, reg2 = L->L, reg3 = L->R; index = 3,
   reg1 = R->R, reg2 = R->L, reg3 = 0x20 (apply). This is CD_vol inlined on a local.
   Returns 0. */
/* func_80091A7C */
s32 CD_initvol(void)
{
    CdlATV vol;

    if (D_800B6630->main_volx.left == 0 && D_800B6630->main_volx.right == 0)
    {
        D_800B6630->main_vol.left = 0x3FFF;
        D_800B6630->main_vol.right = 0x3FFF;
    }
    D_800B6630->cd_vol.left = 0x3FFF;
    D_800B6630->cd_vol.right = 0x3FFF;
    D_800B6630->spucnt = 0xC001;
    vol.val0 = vol.val2 = 0x80;
    vol.val1 = vol.val3 = 0;
    *D_800B661C = 2;
    *D_800B6624 = vol.val0;
    *D_800B6628 = vol.val1;
    *D_800B661C = 3;
    *D_800B6620 = vol.val2;
    *D_800B6624 = vol.val3;
    *D_800B6628 = 0x20;
    return 0;
}
