/* PSY-Q 4.5 libcd, member cdread.o, part 2 of 2: func_800931A0,
   func_80093270, CdReadBreak, CdRead, CdReadSync, CdReadCallback,
   CdReadMode. The first split had it in psyq_spu.c. The tree compiles this
   run of the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
int func_80093270(int);
/* return_slot_fill: func_800931A0 */
/* cc97_lib: func_800931A0 */
/* reorder_fill: func_800931A0 */
/* far_syms: D_800B6730 (target reaches it %hi/%lo) */
/* far_syms: func_80092EF8 (target reaches it %hi/%lo) */
extern volatile int D_800B6730[];
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
void func_80092EF8(void);            /* CDREAD.OBJ's pause sync callback (libcd_cdread_1.c) */
extern CdlCB D_800B62C0;            /* CD_cbread: CdRead's completion callback */
extern s32 D_800B62C4;              /* CD_read_dma_mode */

void func_800931A0(void)
{
  D_800B6730[2] = (s32) (D_800B6730[2] + (D_800B6730[4] * 4));
  D_800B6730[5] = (s32) (D_800B6730[5] - 1);
  D_800B6730[8] = (s32) (D_800B6730[8] + 1);
  if (D_800B6730[5] == 0)
  {
    CdReadyCallback((CdlCB) D_800B6730[11]);
    if (D_800B62C4 & 1)
    {
      CdDataCallback((Callback) D_800B6730[12]);
    }
    CdSyncCallback((CdlCB) func_80092EF8);
    CdControlF(9, 0);
    D_800B6730[9] = 1;
    if (D_800B62C0 != 0)
    {
      D_800B62C0(2, (u_char *) D_800B6730[13]);
    }
  }
}
/* cc97_lib: func_80093270 */
/* return_slot_fill: func_80093270 */
/* reorder_fill: func_80093270 */
void func_80092F2C(u8 intr, u8 *result);      /* cb_read (libcd_cdread_1.c) */
extern char D_8001373C[];
extern char D_80013754[];
s32 VSync(s32);
s32 CdStatus(void);
void puts();
s32 CdMode(void);

/* libcd cd_read_retry (CDREAD.OBJ, static; named in the SDK 3.5-4.0 objects).
   The return type is plain `int` on purpose: `return D = -1;` on the volatile state
   re-reads it only when no conversion wraps the assignment (an `s32` return type
   folds it to `D = -1, -1`, 112/122). */
int func_80093270(int retry)
{
    u8 mode;

    CdSyncCallback(0);
    CdReadyCallback(0);
    if (D_800B62C4 & 1) {
        CdDataCallback(0);
    }
    if (CdStatus() & 0x10) {
        if ((VSync(-1) & 0x3F) == 0) {
            puts(D_8001373C);
        }
        CdControlF(1, 0);
        D_800B6730[7] = VSync(-1);
        D_800B6730[5] = -1;
        return D_800B6730[5];
    }
    if (retry) {
        puts(D_80013754);
        CdControl(9, 0, 0);
        if (CdControl(2, (s8 *) CdLastPos(), 0) == 0) {
            return D_800B6730[5] = -1;
        }
    }
    CdFlush();
    mode = D_800B6730[3];
    if (mode != CdMode() || retry) {
        if (CdControl(0xE, (s8 *) &mode, 0) == 0) {
            return D_800B6730[5] = -1;
        }
    }
    D_800B6730[8] = CdPosToInt(CdLastPos());
    CdReadyCallback(func_80092F2C);
    if (D_800B62C4 & 1) {
        CdDataCallback(func_800931A0);
    }
    D_800B6730[2] = D_800B6730[1];
    CdControlF(6, 0);
    D_800B6730[5] = D_800B6730[0];
    D_800B6730[6] = VSync(-1);
    return D_800B6730[5];
}

/* cc97_lib: CdReadBreak */
/* return_slot_fill: CdReadBreak */
/* reorder_fill: CdReadBreak */
s32 VSync(s32);
s32 CdDataSync(s32);

static inline s32 cd_read_wait(void)
{
    /* CDREAD.OBJ's inlined wait for a pause it sent (CdReadBreak, CdRead): 1 at once when
       none is pending (state word [9]); else spin until the pause's sync callback
       (func_80092EF8) clears the flag, 0; after 120 vertical blanks give up: put the saved
       sync callback [10] back, clear the flag, -1 */
    s32 t;

    if (D_800B6730[9] == 0) {
        return 1;
    }
    t = VSync(-1);
    while (D_800B6730[9] != 0) {
        if ((u32) (VSync(-1) - t) > 120) {
            CdSyncCallback((CdlCB) D_800B6730[10]);
            D_800B6730[9] = 0;
            return -1;
        }
    }
    return 0;
}
/* libcd CdReadBreak (CDREAD.OBJ) */
/* func_80093458 */
void CdReadBreak(void)
{
    if (D_800B62C4 & 1) {
        CdDataSync(0);
    }
    D_800B6730[5] = 0;
    if (cd_read_wait() != 0) {
        CdReadyCallback((CdlCB) D_800B6730[11]);
        if (D_800B62C4 & 1) {
            CdDataCallback((Callback) D_800B6730[12]);
        }
        CdSyncCallback((CdlCB) func_80092EF8);
        CdControlF(9, 0);
        D_800B6730[9] = 1;
        cd_read_wait();
    }
}

/* cc97_lib: CdRead */
/* return_slot_fill: CdRead */
s32 VSync(s32);
s32 CdStatus(void);

/* libcd CdRead (CDREAD.OBJ) */
/* func_800935F8 */
int CdRead(int sectors, u_long *buf, int mode)
{
    cd_read_wait();
    D_800B6730[3] = mode;
    switch (D_800B6730[3] & 0x30) {
    case 0:
        D_800B6730[4] = 0x200;
        break;
    case 0x20:
        D_800B6730[4] = 0x249;
        break;
    default:
        D_800B6730[4] = 0x246;
        break;
    }
    D_800B6730[3] |= 0x20;
    D_800B6730[1] = (s32) buf;
    D_800B6730[0] = sectors;
    D_800B6730[10] = (int) CdSyncCallback(0);
    D_800B6730[11] = (int) CdReadyCallback(0);
    if (D_800B62C4 & 1) {
        D_800B6730[12] = (int) CdDataCallback(0);
    }
    D_800B6730[7] = VSync(-1);
    if (CdStatus() & 0xE0) {
        CdControlB(9, 0, 0);
    }
    return func_80093270(0) > 0;
}

/* cc97_lib: CdReadSync */
/* return_slot_fill: CdReadSync */
s32 VSync(s32);
int func_80093270(int);
s32 CdReady(s32, u8 *);

/* libcd CdReadSync (CDREAD.OBJ) */
/* func_80093794 */
s32 CdReadSync(s32 mode, u8 *result)
{
    s32 n;

    while (1) {
        n = -1;
        if (VSync(-1) <= D_800B6730[7] + 1200) {
            if (D_800B6730[5] < 0 || VSync(-1) > D_800B6730[6] + 60) {
                func_80093270(1);
                n = D_800B6730[0];
            } else {
                n = D_800B6730[5];
            }
        }
        if (mode != 0 || !((D_800B6730[9] && n == 0) || n > 0)) {
            CdReady(1, result);
            if (D_800B6730[9] && n == 0) {
                n = 1;
            }
            return n;
        }
    }
}

/* far_syms: D_800B62C0 (target reaches it %hi/%lo) */

/* cc97_lib: CdReadCallback */
/* return_slot_fill: CdReadCallback */
/* libcd CdReadCallback(func) (CDREAD.OBJ): install func as CdRead's completion callback
   CD_cbread (D_800B62C0) and return the one it replaces. */
/* func_80093898 */
CdlCB CdReadCallback(CdlCB func)
{
    CdlCB old;

    old = D_800B62C0;
    D_800B62C0 = func;
    return old;
}
/* far_syms: D_800B62C4 (target reaches it %hi/%lo) */

/* cc97_lib: CdReadMode */
/* return_slot_fill: CdReadMode */
/* libcd CdReadMode(mode) (CDREAD.OBJ): set CD_read_dma_mode (D_800B62C4; bit 0 = CdRead
   takes its sectors by DMA with a data callback) and return the old value. */
/* func_800938AC */
s32 CdReadMode(s32 mode)
{
    s32 old;

    old = D_800B62C4;
    D_800B62C4 = mode;
    return old;
}

INCLUDE_PAD(8);
