/* PSY-Q 4.5 libgpu, member sys.o, part 3 of 6: func_8008D99C,
   func_8008DC4C, func_8008DEAC, func_8008DFFC. The first split had it in
   psyq_gpu_d.c. The tree compiles this run of the member's functions with
   other flags than the runs beside it (config/cc_flags.txt), so the member
   is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: func_8008DC4C */
/* gnu_as_loads: func_8008DC4C */
/* return_slot_fill: func_8008DC4C */
/* far_syms: D_800B60A8 (target reaches it %hi/%lo) */
/* far_syms: D_800B60B4 (target reaches it %hi/%lo) */
/* far_syms: D_800B60C8 (target reaches it %hi/%lo) */
/* far_syms: D_800B60CC (target reaches it %hi/%lo) */
/* far_syms: D_800B60D4 (target reaches it %hi/%lo) */
extern GpuEnv D_800B5FA0;
extern volatile u_long *D_800B60A8;
extern volatile u_long *D_800B60B4;
extern volatile int D_800B60C8;
extern volatile int D_800B60CC;
extern int D_800B60D4;
extern volatile GpuQue D_800E5718[];
extern int SetIntrMask(int);
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern Callback DMACallback(s32 dma, Callback f);
/* cc97_lib: func_8008D99C */
/* gnu_as_loads: func_8008D99C */
/* return_slot_fill: func_8008D99C */
/* far_syms: D_800B60D0 (target reaches it %hi/%lo) */
extern int D_800B60D0;
extern void set_alarm(void);
extern int func_8008E16C(void);
int func_8008DC4C(void);  /* TU defines this below */

/* libgpu _addque2(func, p1, len, p2) (SYS.OBJ, static; the GPU function table's +8
   entry, the one every drawing call goes through): run the GPU routine func(p1, p2)
   now, or queue it. While the queue is full ((_qin + 1) & 0x3F == _qout; _qin
   D_800B60C8, _qout D_800B60CC) it runs _exeque (func_8008DC4C) under the alarm
   (set_alarm set_alarm, get_alarm func_8008E16C) and returns -1 on the timeout.
   Then, with interrupts masked (SetIntrMask, the old mask kept in
   D_800B60D0) and GEnv.cb_pending set: if queueing is off (GEnv.queue == 0), or the
   queue is empty, no DMA transfer is in flight (DMA2_CHCR bit 24, through
   D_800B60B4) and no DrawSync callback is installed, it waits for GPUSTAT bit 26
   (through D_800B60A8), calls func(p1, p2) at once and returns 0. Otherwise it makes
   _exeque the DMA-2 callback (DMACallback) and fills the entry at
   _qin: with len != 0 the len / 4 words at p1 are copied into the entry's own
   buffer and p1 becomes that buffer, else p1 is kept as given; then p2 and func. It
   advances _qin modulo 64, restores the mask, runs _exeque once and returns the
   number of entries still queued. */
int func_8008D99C(int (*func)(u_long p1, u_long p2), u_long p1, int len, u_long p2)
{
    int i;

    set_alarm();
    while (((D_800B60C8 + 1) & 0x3F) == D_800B60CC) {
        if (func_8008E16C()) {
            return -1;
        }
        func_8008DC4C();
    }
    D_800B60D0 = SetIntrMask(0);
    D_800B5FA0.cb_pending = 1;
    if (D_800B5FA0.queue == 0 ||
        (D_800B60C8 == D_800B60CC && !(*D_800B60B4 & 0x01000000) && D_800B5FA0.drawsync == 0)) {
        while (!(*D_800B60A8 & 0x04000000)) {
        }
        func(p1, p2);
        SetIntrMask(D_800B60D0);
        return 0;
    }
    DMACallback(2, (Callback)func_8008DC4C);
    if (len) {
        for (i = 0; i < len / 4; i++) {
            D_800E5718[D_800B60C8].buf[i] = ((u_long *)p1)[i];
        }
        D_800E5718[D_800B60C8].p1 = (u_long)D_800E5718[D_800B60C8].buf;
    } else {
        D_800E5718[D_800B60C8].p1 = p1;
    }
    D_800E5718[D_800B60C8].p2 = p2;
    D_800E5718[D_800B60C8].func = func;
    D_800B60C8 = (D_800B60C8 + 1) & 0x3F;
    SetIntrMask(D_800B60D0);
    func_8008DC4C();
    return (D_800B60C8 - D_800B60CC) & 0x3F;
}

/* libgpu _exeque (SYS.OBJ, static; the GPU function table's +0x24 entry): runs the
   command queue. Returns 1 at once while a GPU DMA transfer is in flight (DMA2_CHCR
   bit 24, through the pointer D_800B60B4). Otherwise, with interrupts masked
   (SetIntrMask, the old mask kept in D_800B60D4), it executes entries
   from _qout (D_800B60CC) until the queue is empty (_qin D_800B60C8) or an entry
   has started a DMA transfer: before the LAST entry it removes the DMA-2 callback
   (DMACallback(2, 0)) unless a DrawSync callback is installed, waits
   for GPUSTAT bit 26 (ready for a command word, through D_800B60A8), calls
   _que[_qout].func(p1, p2) and advances _qout modulo 64. With the queue drained and
   no transfer running it fires the DrawSync callback once (GEnv.cb_pending cleared
   first). Returns the number of entries still queued. */
int func_8008DC4C(void)
{
    if (*D_800B60B4 & 0x01000000) {
        return 1;
    }
    D_800B60D4 = SetIntrMask(0);
    while (D_800B60C8 != D_800B60CC && !(*D_800B60B4 & 0x01000000)) {
        if (((D_800B60CC + 1) & 0x3F) == D_800B60C8 && D_800B5FA0.drawsync == 0) {
            DMACallback(2, 0);
        }
        while (!(*D_800B60A8 & 0x04000000)) {
        }
        D_800E5718[D_800B60CC].func(D_800E5718[D_800B60CC].p1, D_800E5718[D_800B60CC].p2);
        D_800B60CC = (D_800B60CC + 1) & 0x3F;
    }
    SetIntrMask(D_800B60D4);
    if (D_800B60C8 == D_800B60CC && !(*D_800B60B4 & 0x01000000)
        && D_800B5FA0.cb_pending && D_800B5FA0.drawsync) {
        D_800B5FA0.cb_pending = 0;
        D_800B5FA0.drawsync();
    }
    return (D_800B60C8 - D_800B60CC) & 0x3F;
}
/* cc97_lib: func_8008DEAC */
/* gnu_as_loads: func_8008DEAC */
/* return_slot_fill: func_8008DEAC */
/* far_syms: D_800B60C4 (target reaches it %hi/%lo) */
/* far_syms: D_800B60D8 (target reaches it %hi/%lo) */
extern volatile u_long *D_800B60C4;
extern int D_800B60D8;
extern u_char D_800E5610[];
extern void func_8008E790(u_char *ptr, int value, int num);
extern int func_8008E2B0(int mode);

/* libgpu _reset(mode) (SYS.OBJ, static; the GPU function table's +0x34 entry,
   ResetGraph's worker). With interrupts masked (SetIntrMask, the old
   mask kept in D_800B60D8) it empties the command queue (_qout D_800B60CC = 0,
   _qin D_800B60C8 = _qout) and then, by mode & 7:
   0 or 5: DMA2 CHCR = 0x401 (through D_800B60B4), DPCR |= 0x800 (through
     D_800B60C4), GP1 = 0 (GPUSTAT's address, through D_800B60A8), and the
     object's memset (func_8008E790) zeroes ctlbuf (D_800E5610, 0x100 bytes) and
     _que (D_800E5718, 0x1800 bytes);
   1 or 3: the same CHCR and DPCR writes, then GP1 = 0x02000000 and GP1 =
     0x01000000; ctlbuf and _que are kept;
   any other value: nothing more.
   The mask is restored. Returns _version(mode) (func_8008E2B0), the GPU type,
   when mode & 7 is 0, else 0. */
int func_8008DEAC(int mode)
{
    D_800B60D8 = SetIntrMask(0);
    D_800B60CC = 0;
    D_800B60C8 = D_800B60CC;
    switch (mode & 7) {
    case 0:
    case 5:
        *D_800B60B4 = 0x401;
        *D_800B60C4 |= 0x800;
        *D_800B60A8 = 0;
        func_8008E790(D_800E5610, 0, 0x100);
        func_8008E790((u_char *)D_800E5718, 0, 0x1800);
        break;
    case 1:
    case 3:
        *D_800B60B4 = 0x401;
        *D_800B60C4 |= 0x800;
        *D_800B60A8 = 0x02000000;
        *D_800B60A8 = 0x01000000;
        break;
    }
    SetIntrMask(D_800B60D8);
    return !(mode & 7) ? func_8008E2B0(mode) : 0;
}

/* cc97_lib: func_8008DFFC */
/* gnu_as_loads: func_8008DFFC */
/* return_slot_fill: func_8008DFFC */

/* libgpu _sync(mode) (SYS.OBJ, static; the GPU function table's +0x3C entry,
   DrawSync's worker). mode 0 waits: it arms the alarm (set_alarm, set_alarm),
   calls _exeque (func_8008DC4C) until _qin (D_800B60C8) equals _qout
   (D_800B60CC), then loops until no GPU DMA transfer is running (DMA2 CHCR bit 24
   clear, through D_800B60B4) and the GPU takes commands (GPUSTAT bit 26, through
   D_800B60A8). Each pass of either loop polls get_alarm (func_8008E16C) and
   returns -1 on the timeout; the wait returns 0. Any other mode polls: queued =
   (_qin - _qout) & 0x3F, one _exeque call if that is non-zero, and the result is
   queued, or 1 when queued is 0 but a transfer is still running or the GPU is
   not ready. */
int func_8008DFFC(int mode)
{
    int queued;

    if (mode == 0) {
        set_alarm();
        while (D_800B60C8 != D_800B60CC) {
            func_8008DC4C();
            if (func_8008E16C()) {
                return -1;
            }
        }
        while ((*D_800B60B4 & 0x01000000) || !(*D_800B60A8 & 0x04000000)) {
            if (func_8008E16C()) {
                return -1;
            }
        }
        return 0;
    }
    queued = (D_800B60C8 - D_800B60CC) & 0x3F;
    if (queued) {
        func_8008DC4C();
    }
    if ((*D_800B60B4 & 0x01000000) || !(*D_800B60A8 & 0x04000000)) {
        if (queued) {
            return queued;
        }
        return 1;
    }
    return queued;
}
