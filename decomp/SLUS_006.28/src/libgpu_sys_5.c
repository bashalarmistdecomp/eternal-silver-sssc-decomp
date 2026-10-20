/* PSY-Q 4.5 libgpu, member sys.o, part 5 of 6: LoadImage2, StoreImage2,
   MoveImage2, DrawOTag2. The first split had it in psyq_gpu_f.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern volatile u_long *D_800B60A8;
extern volatile u_long *D_800B60B4;
/* cc97_lib: StoreImage2 */
/* gnu_as_loads: StoreImage2 */
/* return_slot_fill: StoreImage2 */
/* far_syms: D_800B5F98 (target reaches it %hi/%lo) */
/* far_syms: D_800B60A8 (target reaches it %hi/%lo) */
/* far_syms: D_800B60B4 (target reaches it %hi/%lo) */
/* far_syms: D_800B60DC (target reaches it %hi/%lo) */
extern GpuTable *D_800B5F98;
extern int D_800B60DC;
extern int D_800B60E0[];
extern char D_800130DC[];
extern void func_8008BC6C(char *, RECT *);
extern int func_8008E16C(void);
extern void _GPU_ResetCallback(void);
extern int VSync(int);
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern Callback DMACallback(s32 dma, Callback f);

static inline void gpu_set_alarm(void)
{
    /* sys.o's set_alarm (out of line at set_alarm), inlined by the functions
       defined after it: arm the GPU timeout 240 vertical blanks from now */
    D_800B60DC = VSync(-1) + 0xF0;
    D_800B60E0[0] = 0;
}
/* cc97_lib: LoadImage2 */
/* gnu_as_loads: LoadImage2 */
/* return_slot_fill: LoadImage2 */
extern char D_800131B8[];

/* libgpu LoadImage2(rect, p) (SYS.OBJ): LoadImage without the command queue.
   checkRECT("LoadImage2", rect) (func_8008BC6C; the string D_800131B8 is this
   function's own, where StoreImage2 and MoveImage2 pass their queued twins'
   names); arms the alarm (set_alarm, inlined) and waits until no GPU DMA transfer
   is running (DMA2 CHCR bit 24 clear, through D_800B60B4) and the GPU takes
   commands (GPUSTAT bit 26, through D_800B60A8), returning -1 if get_alarm
   (func_8008E16C) reports the timeout; then DMACallback(2, _GPU_ResetCallback)
   (_GPU_ResetCallback) and the GPU table's dws entry (+0x20) called
   directly with (rect, p). Returns 0; dws's own result is dropped. */
/* func_8008E350 */
int LoadImage2(RECT *rect, u_long *p)
{
    func_8008BC6C(D_800131B8, rect);
    gpu_set_alarm();
    while ((*D_800B60B4 & 0x01000000) || !(*D_800B60A8 & 0x04000000)) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    DMACallback(2, _GPU_ResetCallback);
    D_800B5F98->dws(rect, p);
    return 0;
}

/* libgpu StoreImage2(rect, p) (SYS.OBJ): StoreImage without the command queue.
   checkRECT("StoreImage", rect); arms the alarm (set_alarm, inlined) and waits
   until no GPU DMA transfer is running (DMA2 CHCR bit 24 clear, through
   D_800B60B4) and the GPU takes commands (GPUSTAT bit 26, through D_800B60A8),
   returning -1 if get_alarm (func_8008E16C) reports the timeout; then
   DMACallback(2, _GPU_ResetCallback) and the GPU table's drs entry (+0x1C) called
   directly with (rect, p). Returns 0; drs's own result is dropped. */
/* func_8008E43C */
int StoreImage2(RECT *rect, u_long *p)
{
    func_8008BC6C(D_800130DC, rect);
    gpu_set_alarm();
    while ((*D_800B60B4 & 0x01000000) || !(*D_800B60A8 & 0x04000000)) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    DMACallback(2, _GPU_ResetCallback);
    D_800B5F98->drs(rect, p);
    return 0;
}
/* cc97_lib: MoveImage2 */
/* gnu_as_loads: MoveImage2 */
/* return_slot_fill: MoveImage2 */
extern char D_800130E8[];
extern u_long D_800B6038[5];

/* libgpu MoveImage2(rect, x, y) (SYS.OBJ): MoveImage without the command queue.
   checkRECT("MoveImage", rect); the same alarm-guarded wait as StoreImage2 (-1 on
   the timeout) and DMACallback(2, _GPU_ResetCallback); then -1 when rect->w or rect->h
   is 0. Otherwise it fills the static frame-buffer-copy primitive D_800B6038
   (5 words; MoveImage's): source = rect's x / y word, destination =
   (y << 16) | (x & 0xFFFF), size = rect's w / h word, and calls the GPU table's
   cwc entry (+0x18) on it directly. Returns 0. */
/* func_8008E528 */
int MoveImage2(RECT *rect, int x, int y)
{
    func_8008BC6C(D_800130E8, rect);
    gpu_set_alarm();
    while ((*D_800B60B4 & 0x01000000) || !(*D_800B60A8 & 0x04000000)) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    DMACallback(2, _GPU_ResetCallback);
    if (rect->w == 0 || rect->h == 0) {
        return -1;
    }
    D_800B6038[2] = *(u_long *)&rect->x;
    D_800B6038[3] = (y << 16) | (x & 0xFFFF);
    D_800B6038[4] = *(u_long *)&rect->w;
    D_800B5F98->cwc(D_800B6038);
    return 0;
}
/* cc97_lib: DrawOTag2 */
/* gnu_as_loads: DrawOTag2 */
/* return_slot_fill: DrawOTag2 */
/* far_syms: D_800B5F9C (target reaches it %hi/%lo) */
extern int (*D_800B5F9C)();
extern GpuEnv D_800B5FA0;
extern char D_80013124[];

/* libgpu DrawOTag2(p) (SYS.OBJ): DrawOTag without the command queue. At debug
   level 2 or more it prints "DrawOTag(%08x)...\n" through GPU_printf; then the
   same alarm-guarded wait as StoreImage2 (-1 on the timeout),
   DMACallback(2, _GPU_ResetCallback) (_GPU_ResetCallback, itself
   DMACallback(2, _exeque)), and the GPU table's cwc entry (+0x18) called
   directly with the ordering table p. Returns 0. */
/* func_8008E66C */
int DrawOTag2(u_long *p)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_80013124, p);
    }
    gpu_set_alarm();
    while ((*D_800B60B4 & 0x01000000) || !(*D_800B60A8 & 0x04000000)) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    DMACallback(2, _GPU_ResetCallback);
    D_800B5F98->cwc(p);
    return 0;
}
