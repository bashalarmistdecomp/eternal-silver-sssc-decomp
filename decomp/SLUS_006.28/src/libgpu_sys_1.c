/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libgpu/sys.c),
   psyz (decomp/src/libgpu/sys.c) and psy-q-decomp (src/etc/intr_vb.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libgpu, member sys.o, part 1 of 6: ResetGraph, SetGraphDebug,
   SetGraphQueue, GetGraphDebug, DrawSyncCallback, SetDispMask, DrawSync,
   func_8008BC6C, ClearImage, ClearImage2, LoadImage, StoreImage, MoveImage,
   ClearOTag, ClearOTagR, DrawPrim, DrawOTag, PutDrawEnv, DrawOTagEnv,
   GetDrawEnv, PutDispEnv, GetDispEnv, GetODE, SetDrawArea, SetDrawOffset,
   SetDrawEnv, func_8008CC48, func_8008CEB8, func_8008CED8, func_8008CF70,
   func_8008D008, func_8008D024, func_8008D0A4, func_8008D0BC,
   func_8008D19C, func_8008D3CC, func_8008D608, func_8008D888,
   func_8008D8AC, func_8008D8C0, func_8008D900, _param. The first
   split had it in psyq_gpu_b.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"
extern volatile u_long *D_800B60A8;
extern volatile u_long **D_800B60B8;
extern volatile u_long *D_800B60BC;
extern volatile u_long *D_800B60C0;
extern volatile u_long *D_800B60C4;
extern volatile u_long *D_800B60A4;
extern volatile u_long **D_800B60AC;
extern volatile u_long *D_800B60B0;
extern volatile u_long *D_800B60B4;
extern u_long D_800B604C[];
extern u_long D_800B6060[];

/* cc97_lib: ResetGraph */
/* gnu_as_loads: ResetGraph */
/* reorder_fill: ResetGraph */
/* return_slot_fill: ResetGraph */
/* far_syms: D_800B5F98 (target reaches it %hi/%lo) */
/* far_syms: D_800B5F9C (target reaches it %hi/%lo) */
extern GpuTable D_800B5F58;
extern GpuTable *D_800B5F98;
extern int (*D_800B5F9C)();
extern u8 D_80012FDC[];
extern u8 D_80012FFC[];
extern GpuEnv D_800B5FA0;
extern int D_800B6020[];
extern int D_800B602C[];
extern int printf();
extern void func_8008E790(u_char *ptr, int value, int num);
extern void *memcpy(u_char *dst, u_char *src, int n);
extern int GPU_cw(u_long);
extern int func_8008DEAC(int);

/* libgpu ResetGraph(mode) (SYS.OBJ). Modes 0 and 3 are the full reset: print
   "ResetGraph:jtb=%08x,env=%08x" (the function table, GEnv), then, as mode 5 does
   without the print: clear GEnv, ResetCallback, GPU_cw(the table pointer's low 24
   bits), GEnv.type = _reset(mode), queue mode 1, the frame-buffer size for that GPU
   type, and both cached environments filled with 0xFF so the next PutDrawEnv /
   PutDispEnv sends everything; returns the GPU type. Any other mode (1: cancel the
   current drawing and flush the queue) prints "ResetGraph(%d)..." at debug level 2
   and returns the table's reset entry (+0x34) called with 1. */
/* func_8008B888 */
int ResetGraph(int mode)
{
    switch (mode & 7) {
    case 0:
    case 3:
        printf(D_80012FDC, &D_800B5F58, &D_800B5FA0);
    case 5:
        func_8008E790((u_char *)&D_800B5FA0, 0, sizeof(GpuEnv));
        ResetCallback();
        GPU_cw((u_long)D_800B5F98 & 0xFFFFFF);
        D_800B5FA0.type = func_8008DEAC(mode);
        D_800B5FA0.queue = 1;
        D_800B5FA0.w = D_800B6020[D_800B5FA0.type];
        D_800B5FA0.h = D_800B602C[D_800B5FA0.type];
        func_8008E790((u_char *)&D_800B5FA0.draw, -1, sizeof(DRAWENV));
        func_8008E790((u_char *)&D_800B5FA0.disp, -1, sizeof(DISPENV));
        return D_800B5FA0.type;
    }
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_80012FFC, mode);
    }
    return D_800B5F98->reset(1);
}
/* cc97_lib: SetGraphDebug */
/* gnu_as_loads: SetGraphDebug */
/* reorder_fill: SetGraphDebug */
/* return_slot_fill: SetGraphDebug */
extern char D_80013010[];

/* libgpu SetGraphDebug(level) (SYS.OBJ): set the debug level GEnv.level and return
   the level it replaces. A non-zero level prints
   "SetGraphDebug:level:%d,type:%d reverse:%d" (the level as stored, the GPU type,
   the reverse flag) through GPU_printf. */
/* func_8008B9FC */
int SetGraphDebug(int level)
{
    u_char prev = D_800B5FA0.level;

    D_800B5FA0.level = level;
    if (D_800B5FA0.level) {
        D_800B5F9C(D_80013010, D_800B5FA0.level, D_800B5FA0.type, D_800B5FA0.reverse);
    }
    return prev;
}
/* cc97_lib: SetGraphQueue */
/* gnu_as_loads: SetGraphQueue */
/* return_slot_fill: SetGraphQueue */
extern u8 D_8001303C[];
typedef void (*Callback)();     /* a libetc interrupt, DMA or VSync callback (libetc_intr_1.c's) */
extern Callback DMACallback(s32 dma, Callback f);

/* libgpu SetGraphQueue(mode) (SYS.OBJ): set the queue mode GEnv.queue (GEnv + 1;
   1 after ResetGraph) and return the mode it replaces. Prints "SetGrapQue(%d)..."
   at debug level 2. When mode differs from the current one it calls the GPU
   table's reset entry (+0x34) with 1 (ResetGraph's mode 1: by the SDK reference,
   cancel the current drawing and flush the command buffer), stores the new mode
   and removes the DMA-2 (GPU) callback: DMACallback(2, 0). */
/* func_8008BA58 */
int SetGraphQueue(int mode)
{
    u_char prev = D_800B5FA0.queue;

    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_8001303C, mode);
    }
    if (mode != D_800B5FA0.queue) {
        D_800B5F98->reset(1);
        D_800B5FA0.queue = mode;
        DMACallback(2, 0);
    }
    return prev;
}

/* cc97_lib: GetGraphDebug */
/* gnu_as_loads: GetGraphDebug */

/* libgpu GetGraphDebug() (SYS.OBJ): the debug level GEnv.level. */
/* func_8008BAFC */
int GetGraphDebug(void)
{
    return D_800B5FA0.level;
}
/* cc97_lib: DrawSyncCallback */
/* gnu_as_loads: DrawSyncCallback */
/* return_slot_fill: DrawSyncCallback */
extern u8 D_80013050[];

/* libgpu DrawSyncCallback(func) (SYS.OBJ): install func as the DrawSync callback
   (GEnv + 0xC, the word _exeque calls once the command queue has drained; 0
   removes it) and return the function it replaces. Prints
   "DrawSyncCallback(%08x)..." at debug level 2. */
/* func_8008BB0C */
u_long DrawSyncCallback(void (*func)(void))
{
    void (*prev)(void);

    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_80013050, func);
    }
    prev = D_800B5FA0.drawsync;
    D_800B5FA0.drawsync = func;
    return (u_long)prev;
}
/* cc97_lib: SetDispMask */
/* gnu_as_loads: SetDispMask */
/* return_slot_fill: SetDispMask */
extern u8 D_8001306C[];

/* libgpu SetDispMask(mask) (SYS.OBJ): switch the display on (mask != 0) or off
   (mask == 0). Prints "SetDispMask(%d)..." at debug level 2. Switching off first
   fills the cached display environment GEnv.disp with 0xFF bytes (memset
   func_8008E790, 0x14 bytes), so the next PutDispEnv finds every cached field
   different and sends everything. Then the GPU table's ctl entry (+0x10) writes
   the control word 0x03000000 (display on) or 0x03000001 (display off). */
/* func_8008BB6C */
void SetDispMask(int mask)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_8001306C, mask);
    }
    if (mask == 0) {
        func_8008E790((u_char *)&D_800B5FA0.disp, -1, sizeof(DISPENV));
    }
    D_800B5F98->ctl(mask ? 0x03000000 : 0x03000001);
}

/* cc97_lib: DrawSync */
/* gnu_as_loads: DrawSync */
/* return_slot_fill: DrawSync */
extern char D_80013080[];

/* libgpu DrawSync(mode) (SYS.OBJ): prints "DrawSync(%d)..." at debug level 2 and
   returns the GPU table's sync entry (+0x3C) called with mode (0: wait until the
   command queue has drained and the GPU is idle; otherwise the number of queued
   requests). */
/* func_8008BC04 */
int DrawSync(int mode)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_80013080, mode);
    }
    return D_800B5F98->sync(mode);
}
/* cc97_lib: func_8008BC6C */
/* gnu_as_loads: func_8008BC6C */
/* reorder_fill: func_8008BC6C */
/* return_slot_fill: func_8008BC6C */
extern u8 D_80013094[];
extern u8 D_800130A0[];
extern u8 D_800130B4[];

/* libgpu checkRECT(log, r) (SYS.OBJ; a static from SDK 4.1 on, named in the 3.30
   to 4.00 objects): the debug check the image functions (ClearImage, ClearImage2,
   LoadImage, StoreImage, MoveImage and the three ...Image2 forms) make on their
   rectangle; log is the caller's name. At debug level 1 it prints "<log>:bad RECT" and "(x,y)-(w,h)" when the
   rectangle is wider than the frame buffer (w > GEnv.w), ends right of it
   (w + x > GEnv.w), starts below it (y > GEnv.h), ends below it (y + h > GEnv.h),
   or has w <= 0, x < 0, y < 0 or h <= 0. At level 2 it prints "<log>:" and the
   rectangle every time. At any other level (0 included) it does nothing. */
void func_8008BC6C(char *log, RECT *r)
{
    switch (D_800B5FA0.level) {
    case 1:
        if (r->w > D_800B5FA0.w || r->w + r->x > D_800B5FA0.w || r->y > D_800B5FA0.h ||
            r->y + r->h > D_800B5FA0.h || r->w <= 0 || r->x < 0 || r->y < 0 ||
            r->h <= 0) {
            D_800B5F9C(D_80013094, log);
            D_800B5F9C(D_800130A0, r->x, r->y, r->w, r->h);
        }
        break;
    case 2:
        D_800B5F9C(D_800130B4, log);
        D_800B5F9C(D_800130A0, r->x, r->y, r->w, r->h);
        break;
    }
}

/* cc97_lib: ClearImage */
/* gnu_as_loads: ClearImage */
/* return_slot_fill: ClearImage */
extern char D_800130B8[];

/* libgpu ClearImage(rect, r, g, b) (SYS.OBJ): fill the frame-buffer rectangle rect
   with the colour (r, g, b). checkRECT("ClearImage", rect), then queue the GPU
   table's clr entry (+0x0C) through its addque2 entry (+8) with rect copied (8
   bytes) and the colour word (b << 16) | (g << 8) | r as the second argument;
   returns the queue position addque2 returns. */
/* func_8008BD88 */
int ClearImage(RECT *rect, u_char r, u_char g, u_char b)
{
    func_8008BC6C(D_800130B8, rect);
    return D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->clr, (u_long)rect, sizeof(RECT), (b << 16) | (g << 8) | r);
}
/* cc97_lib: ClearImage2 */
/* gnu_as_loads: ClearImage2 */
/* return_slot_fill: ClearImage2 */
extern char D_800130C4[];

/* libgpu ClearImage2(rect, r, g, b) (SYS.OBJ): ClearImage with bit 31 of the colour
   word set (_clr turns it into bit 10 of the draw-mode word it sends ahead of the
   fill). checkRECT's name string D_800130C4 is this function's own. */
/* func_8008BE18 */
int ClearImage2(RECT *rect, u_char r, u_char g, u_char b)
{
    func_8008BC6C(D_800130C4, rect);
    return D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->clr, (u_long)rect, sizeof(RECT), 0x80000000 | (b << 16) | (g << 8) | r);
}

/* cc97_lib: LoadImage */
/* gnu_as_loads: LoadImage */
/* return_slot_fill: LoadImage */
extern char D_800130D0[];

/* libgpu LoadImage(rect, p) (SYS.OBJ): write the pixel data at p into the
   frame-buffer rectangle rect. checkRECT("LoadImage", rect), then queue the GPU
   table's dws entry (+0x20) through its addque2 entry (+8) with rect copied (8
   bytes) and p as the second argument; returns the queue position addque2 returns. */
/* func_8008BEB0 */
int LoadImage(RECT *rect, u_long *p)
{
    func_8008BC6C(D_800130D0, rect);
    return D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->dws, (u_long)rect, sizeof(RECT), (u_long)p);
}
/* cc97_lib: StoreImage */
/* gnu_as_loads: StoreImage */
/* return_slot_fill: StoreImage */
extern char D_800130DC[];

/* libgpu StoreImage(rect, p) (SYS.OBJ): read the frame-buffer rectangle rect into
   p. checkRECT("StoreImage", rect), then queue the GPU table's drs entry (+0x1C)
   through its addque2 entry (+8) with rect copied (8 bytes) and p as the second
   argument; returns the queue position addque2 returns. */
/* func_8008BF10 */
int StoreImage(RECT *rect, u_long *p)
{
    func_8008BC6C(D_800130DC, rect);
    return D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->drs, (u_long)rect, sizeof(RECT), (u_long)p);
}
/* cc97_lib: MoveImage */
/* gnu_as_loads: MoveImage */
/* return_slot_fill: MoveImage */
extern char D_800130E8[];
extern u_long D_800B6038[5];

/* libgpu MoveImage(rect, x, y) (SYS.OBJ): copy the frame-buffer rectangle rect to
   (x, y). checkRECT("MoveImage", rect); returns -1 when rect->w or rect->h is 0.
   Otherwise it fills the static 5-word primitive D_800B6038 ({0x04FFFFFF,
   0x80000000 (GP0 0x80, frame-buffer copy), source xy, destination xy, size}):
   source = rect's x / y word, destination = (y << 16) | (x & 0xFFFF), size =
   rect's w / h word, and queues the GPU table's cwc entry (+0x18) through its
   addque2 entry (+8) with the primitive copied (its 0x14 bytes) and 0 as the
   second argument; returns what addque2 returns. */
/* func_8008BF70 */
int MoveImage(RECT *rect, int x, int y)
{
    func_8008BC6C(D_800130E8, rect);
    if (rect->w == 0 || rect->h == 0) {
        return -1;
    }
    D_800B6038[2] = *(u_long *)&rect->x;
    D_800B6038[3] = (y << 16) | (x & 0xFFFF);
    D_800B6038[4] = *(u_long *)&rect->w;
    return D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->cwc, (u_long)D_800B6038, sizeof(D_800B6038), 0);
}
/* cc97_lib: ClearOTag */
/* gnu_as_loads: ClearOTag */
/* return_slot_fill: ClearOTag */
extern u8 D_800130F4[];

/* libgpu ClearOTag(ot, n) (SYS.OBJ): initialise a forward ordering table of n
   entries. Prints "ClearOTag(%08x,%d)..." at debug level 2, then links each of the
   first n - 1 entries to the next one with length 0 (setlen / setaddr on the
   P_TAG), sets the tag of the static primitive D_800B6060 to 4 words linked to
   the terminator primitive D_800B604C, points the last entry at D_800B6060 and
   returns the address of that last entry (ot + n - 1). n = 0 is not guarded: the
   pre-decrement loop then runs until n wraps. */
/* func_8008C028 */
u_long *ClearOTag(u_long *ot, int n)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_800130F4, ot, n);
    }
    while (--n) {
        ((P_TAG *)ot)->len = 0;
        ((P_TAG *)ot)->addr = (u_long)(ot + 1);
        ot++;
    }
    D_800B6060[0] = ((u_long)D_800B604C & 0xFFFFFF) | 0x04000000;
    *ot = (u_long)D_800B6060 & 0xFFFFFF;
    return ot;
}

/* cc97_lib: ClearOTagR */
/* gnu_as_loads: ClearOTagR */
/* return_slot_fill: ClearOTagR */
extern u8 D_8001310C[];

/* libgpu ClearOTagR(ot, n) (SYS.OBJ): initialise a reverse ordering table of n
   entries. Prints "ClearOTagR(%08x,%d)..." at debug level 2, has the GPU table's
   otc entry (+0x2C) link the entries (the DMA-6 clear: each entry points at the one
   before it), then sets the tag of the static primitive D_800B6060 to 4 words
   linked to the terminator primitive D_800B604C, points ot[0] (the list's last
   entry) at D_800B6060 and returns ot. */
/* func_8008C0F0 */
u_long *ClearOTagR(u_long *ot, int n)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_8001310C, ot, n);
    }
    D_800B5F98->otc(ot, n);
    D_800B6060[0] = ((u_long)D_800B604C & 0xFFFFFF) | 0x04000000;
    *ot = (u_long)D_800B6060 & 0xFFFFFF;
    return ot;
}

/* cc97_lib: DrawPrim */
/* gnu_as_loads: DrawPrim */
/* return_slot_fill: DrawPrim */

/* libgpu DrawPrim(p) (SYS.OBJ): draw one primitive at once, outside the command
   queue. Takes the primitive's length byte, has the GPU table's sync entry (+0x3C)
   wait for the queue to drain (mode 0), then its cwb entry (+0x14) writes the
   primitive's words (from the one after the tag) to GP0 by hand. */
/* func_8008C19C */
void DrawPrim(void *p)
{
    int len = ((P_TAG *)p)->len;

    D_800B5F98->sync(0);
    D_800B5F98->cwb((u_long *)&((P_TAG *)p)->r0, len);
}
/* cc97_lib: DrawOTag */
/* gnu_as_loads: DrawOTag */
/* return_slot_fill: DrawOTag */
extern char D_80013124[];

/* libgpu DrawOTag(p) (SYS.OBJ): draw the ordering table p. Prints
   "DrawOTag(%08x)..." at debug level 2, then queues the GPU table's cwc entry
   (+0x18) through its addque2 entry (+8) with p passed as it is (length 0: no
   copy) and 0 as the second argument. */
/* func_8008C1F8 */
void DrawOTag(u_long *p)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_80013124, p);
    }
    D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->cwc, (u_long)p, 0, 0);
}
/* cc97_lib: PutDrawEnv */
/* gnu_as_loads: PutDrawEnv */
/* return_slot_fill: PutDrawEnv */
extern u8 D_80013138[];
void func_8008CC48(DR_ENV *dr_env, DRAWENV *env);  /* TU defines this below */

/* libgpu PutDrawEnv(env) (SYS.OBJ): make env the drawing environment. Prints
   "PutDrawEnv(%08x)..." at debug level 2, has func_8008CC48 (SetDrawEnv2) build
   the environment primitive env->dr_env from env, terminates it (termPrim: tag
   address 0xFFFFFF), queues the GPU table's cwc entry (+0x18) through its addque2
   entry (+8) with the DR_ENV copied (0x40 bytes), then copies env over the cached
   DRAWENV (GEnv + 0x10, 0x5C bytes) and returns env. */
/* func_8008C268 */
DRAWENV *PutDrawEnv(DRAWENV *env)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_80013138, env);
    }
    func_8008CC48(&env->dr_env, env);
    ((P_TAG *)&env->dr_env)->addr = 0xFFFFFFFF;
    D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->cwc, (u_long)&env->dr_env, sizeof(DR_ENV), 0);
    memcpy((u_char *)&D_800B5FA0.draw, (u_char *)env, sizeof(DRAWENV));
    return env;
}
/* cc97_lib: DrawOTagEnv */
/* gnu_as_loads: DrawOTagEnv */
/* return_slot_fill: DrawOTagEnv */
extern u8 D_80013150[];

/* libgpu DrawOTagEnv(p, env) (SYS.OBJ): draw the ordering table p under the
   drawing environment env in one queued request. Prints
   "DrawOTagEnv(%08x,&08x)..." at debug level 2, has func_8008CC48 (SetDrawEnv2)
   build the environment primitive env->dr_env from env, links that primitive to p
   (setaddr: the tag's low 24 bits, its length byte kept), queues the GPU table's
   cwc entry (+0x18) through its addque2 entry (+8) with the DR_ENV copied (0x40
   bytes), then copies env over the cached DRAWENV (GEnv + 0x10, 0x5C bytes). */
/* func_8008C328 */
void DrawOTagEnv(u_long *p, DRAWENV *env)
{
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_80013150, p, env);
    }
    func_8008CC48(&env->dr_env, env);
    ((P_TAG *)&env->dr_env)->addr = (u_long)p;
    D_800B5F98->addque2((int (*)(u_long, u_long))D_800B5F98->cwc, (u_long)&env->dr_env, sizeof(DR_ENV), 0);
    memcpy((u_char *)&D_800B5FA0.draw, (u_char *)env, sizeof(DRAWENV));
}

/* cc97_lib: GetDrawEnv */
/* gnu_as_loads: GetDrawEnv */
/* return_slot_fill: GetDrawEnv */

/* libgpu GetDrawEnv(env) (SYS.OBJ): copy the cached drawing environment (GEnv +
   0x10, 0x5C bytes; what the last PutDrawEnv / DrawOTagEnv stored) to env and
   return env. */
/* func_8008C400 */
DRAWENV *GetDrawEnv(DRAWENV *env)
{
    memcpy((u_char *)env, (u_char *)&D_800B5FA0.draw, sizeof(DRAWENV));
    return env;
}
/* cc97_lib: PutDispEnv */
/* gnu_as_loads: PutDispEnv */
/* return_slot_fill: PutDispEnv */
extern u8 D_8001316C[];
extern u_short D_800B6074[2][5][2];
extern u_char D_800B609C[];

/* libgpu PutDispEnv(env) (SYS.OBJ): make env the display environment. Prints
   "PutDispEnv(%08x)..." at debug level 2, then always sends GP1 0x05 (display area
   start: disp.x in bits 0-9, disp.y in bits 10-19). If the interlace / 24-bit word or
   the disp rectangle differs from the cached environment it rebuilds GP1 0x08 (display
   mode): bit 3 PAL (env->pad0 = GetVideoMode()), bit 4 isrgb24, bit 5 isinter, bit 7
   GEnv.reverse, the horizontal resolution from disp.w (<= 280: 256, <= 352: 320,
   <= 400: 368 (bit 6), <= 560: 512, else 640) and 0x24 (480 lines, interlaced) when
   disp.h is over 256 (NTSC) / 288 (PAL); it then sets env->pad0 = 8 to force the next
   step. If the screen rectangle differs (or pad0 is 8) it recomputes the display
   ranges: vertical start screen.y + 16 (NTSC) / 19 (PAL), 240 lines when screen.h is 0;
   horizontal start = the range table's start for [video mode][width class] +
   screen.x * the dot-clock divisor, length = the table's span, scaled by screen.w / 256
   when screen.w is non-zero; each clamped (PAL 540..3220 / to 3260, lines 19..303 /
   to 305; NTSC 500..3250 / to 3290, lines 16..257 / to 258), then GP1 0x06 and 0x07.
   Finally it copies env over the cached DISPENV (GEnv + 0x6C) and returns env.
   MARKED SPELLING: the cached rectangles are read through the local
   `volatile RECT *r` (the target's lhu + sll / sra where a plain read gives lh,
   and its second base register for the fields after x). Plain member reads are
   270 of 318, a plain `RECT *r` 271, the GEnv member itself volatile 314. psyz's
   4.0 source has the same reads as `(volatile RECT *)` casts inside a RECT_EQ
   macro, which gives these bytes too; the original construct is not known.
   MARKED SPELLING: `if (h <= (pad0 ? 288 : 256)) { } else { mode |= 0x24; }`, the
   empty arm first, is sotn's and psyz's spelling (313 of 318 as `if (h > ...)`);
   semantically inert, the original construct is not known. */
/* func_8008C434 */
DISPENV *PutDispEnv(DISPENV *env)
{
    int h_start;
    int v_start;
    int mode;
    int v_end;
    int h_end;
    int clk;
    int len;
    volatile RECT *r;

    mode = 0x08000000;
    if (D_800B5FA0.level >= 2) {
        D_800B5F9C(D_8001316C, env);
    }
    D_800B5F98->ctl(0x05000000 | ((env->disp.y & 0x3FF) << 10) | (env->disp.x & 0x3FF));
    r = &D_800B5FA0.disp.disp;
    if (*(long *)&D_800B5FA0.disp.isinter != *(long *)&env->isinter ||
        r->x != env->disp.x || r->y != env->disp.y ||
        r->w != env->disp.w || r->h != env->disp.h) {
        env->pad0 = GetVideoMode();
        if (env->pad0 == 1) {
            mode |= 8;
        }
        if (env->isrgb24) {
            mode |= 0x10;
        }
        if (env->isinter) {
            mode |= 0x20;
        }
        if (D_800B5FA0.reverse) {
            mode |= 0x80;
        }
        if (env->disp.w > 280) {
            if (env->disp.w <= 352) {
                mode |= 1;
            } else if (env->disp.w <= 400) {
                mode |= 0x40;
            } else if (env->disp.w <= 560) {
                mode |= 2;
            } else {
                mode |= 3;
            }
        }
        if (env->disp.h <= (env->pad0 ? 288 : 256)) {
        } else {
            mode |= 0x24;
        }
        D_800B5F98->ctl(mode);
        env->pad0 = 8;
    }
    r = &D_800B5FA0.disp.screen;
    if (r->x != env->screen.x || r->y != env->screen.y ||
        r->w != env->screen.w || r->h != env->screen.h ||
        env->pad0 == 8) {
        env->pad0 = GetVideoMode();
        v_start = env->screen.y + (env->pad0 ? 19 : 16);
        v_end = v_start + (env->screen.h ? env->screen.h : 240);
        if (env->disp.w <= 280) {
            clk = 0;
        } else if (env->disp.w <= 352) {
            clk = 1;
        } else if (env->disp.w <= 400) {
            clk = 2;
        } else if (env->disp.w <= 560) {
            clk = 3;
        } else {
            clk = 4;
        }
        h_start = D_800B6074[env->pad0][clk][0] + env->screen.x * D_800B609C[clk];
        len = D_800B6074[env->pad0][clk][1] - D_800B6074[env->pad0][clk][0];
        h_end = h_start + (env->screen.w ? (len * env->screen.w) >> 8 : len);
        if (env->pad0) {
            h_start = h_start < 540 ? 540 : (h_start > 3220 ? 3220 : h_start);
            h_end = h_end < h_start + D_800B609C[clk] * 4 ? h_start + D_800B609C[clk] * 4 : (h_end > 3260 ? 3260 : h_end);
            v_start = v_start < 19 ? 19 : (v_start > 303 ? 303 : v_start);
            v_end = v_end < v_start + 2 ? v_start + 2 : (v_end > 305 ? 305 : v_end);
        } else {
            h_start = h_start < 500 ? 500 : (h_start > 3250 ? 3250 : h_start);
            h_end = h_end < h_start + D_800B609C[clk] * 4 ? h_start + D_800B609C[clk] * 4 : (h_end > 3290 ? 3290 : h_end);
            v_start = v_start < 16 ? 16 : (v_start > 257 ? 257 : v_start);
            v_end = v_end < v_start + 2 ? v_start + 2 : (v_end > 258 ? 258 : v_end);
        }
        D_800B5F98->ctl(0x06000000 | ((h_end & 0xFFF) << 12) | (h_start & 0xFFF));
        D_800B5F98->ctl(0x07000000 | ((v_end & 0x3FF) << 10) | (v_start & 0x3FF));
    }
    memcpy((u_char *)&D_800B5FA0.disp, (u_char *)env, sizeof(DISPENV));
    return env;
}

/* cc97_lib: GetDispEnv */
/* gnu_as_loads: GetDispEnv */
/* return_slot_fill: GetDispEnv */

/* libgpu GetDispEnv(env) (SYS.OBJ): copy the cached display environment (GEnv +
   0x6C, 0x14 bytes; what the last PutDispEnv stored) to env and return env. */
/* func_8008C92C */
DISPENV *GetDispEnv(DISPENV *env)
{
    memcpy((u_char *)env, (u_char *)&D_800B5FA0.disp, sizeof(DISPENV));
    return env;
}

/* cc97_lib: GetODE */
/* gnu_as_loads: GetODE */

/* libgpu GetODE() (SYS.OBJ): bit 31 of GPUSTAT, read through the GPU table's
   status entry (+0x38): 1 while the GPU draws the odd lines of an interlaced
   frame. */
/* func_8008C960 */
int GetODE(void)
{
    return D_800B5F98->status() >> 31;
}

u_long func_8008CED8(short, short);                        /* extern */
u_long func_8008CF70(short, short);                        /* extern */

/* cc97_lib: SetDrawArea */
/* gnu_as_loads: SetDrawArea */
/* return_slot_fill: SetDrawArea */

/* libgpu SetDrawArea(p, r) (SYS.OBJ): build the drawing-area primitive p for the
   rectangle r: length 2 (setlen on the P_TAG), the top-left setting word
   (func_8008CED8, get_cs, at r->x, r->y) and the bottom-right one (func_8008CF70,
   get_ce, at r->x + r->w - 1, r->y + r->h - 1). */
/* func_8008C990 */
void SetDrawArea(DR_AREA *p, RECT *r)
{
    ((P_TAG *)p)->len = 2;
    p->code[0] = func_8008CED8(r->x, r->y);
    p->code[1] = func_8008CF70(r->x + r->w - 1, r->y + r->h - 1);
}
u_long func_8008D008(short, short);                        /* extern */

/* cc97_lib: SetDrawOffset */
/* gnu_as_loads: SetDrawOffset */
/* return_slot_fill: SetDrawOffset */

/* libgpu SetDrawOffset(p, ofs) (SYS.OBJ): build the drawing-offset primitive p:
   length 2 (setlen on the P_TAG), the offset setting word (func_8008D008,
   get_ofs, at ofs[0], ofs[1]) and a 0 word. */
/* func_8008CA10 */
void SetDrawOffset(DR_OFFSET *p, u_short *ofs)
{
    ((P_TAG *)p)->len = 2;
    p->code[0] = func_8008D008(ofs[0], ofs[1]);
    p->code[1] = 0;
}
/* cc97_lib: SetDrawEnv */
/* gnu_as_loads: SetDrawEnv */
/* return_slot_fill: SetDrawEnv */
u_long func_8008CEB8(int dfe, int dtd, int tpage);  /* TU defines this below */
u_long func_8008D024(RECT *rect);  /* TU defines this below */

/* libgpu SetDrawEnv(dr_env, env) (SYS.OBJ): build the drawing-environment
   primitive dr_env from env. Six setting words follow the tag: drawing area
   top-left (func_8008CED8, get_cs, at clip.x, clip.y) and bottom-right
   (func_8008CF70, get_ce, at clip.x + clip.w - 1, clip.y + clip.h - 1), drawing
   offset (func_8008D008, get_ofs), draw mode (func_8008CEB8, get_mode: dfe,
   dtd, tpage), texture window (func_8008D024, get_tw) and 0xE6000000 (GP0
   0xE6, both mask bits off). With env->isbg set, three more words clear the
   clip area: a GP0 0x60 flat rectangle in env->r0 / g0 / b0, its position
   taken relative to the drawing offset (clip.x - ofs[0], clip.y - ofs[1]), its
   w and h clamped to 0 .. GEnv.w - 1 and 0 .. GEnv.h - 1. The tag's length
   byte becomes the word count, 6 or 9; its address field is left to the caller.
   Nothing in this image calls it: PutDrawEnv and DrawOTagEnv use func_8008CC48.
   MARKED SPELLING: `dr = dr_env;`, an inert local that every store goes through
   (psyz's spelling). With the parameter used directly the prologue saves and
   copies dr_env before env: 124 of 126. The original construct is not known. */
/* func_8008CA50 */
void SetDrawEnv(DR_ENV *dr_env, DRAWENV *env)
{
    DR_ENV *dr;
    int len;
    RECT rect;
    u_short w, h;

    len = 1;
    dr = dr_env;
    ((u_long *)dr)[len++] = func_8008CED8(env->clip.x, env->clip.y);
    ((u_long *)dr)[len++] = func_8008CF70(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    ((u_long *)dr)[len++] = func_8008D008(env->ofs[0], env->ofs[1]);
    ((u_long *)dr)[len++] = func_8008CEB8(env->dfe, env->dtd, env->tpage);
    ((u_long *)dr)[len++] = func_8008D024(&env->tw);
    ((u_long *)dr)[len++] = 0xE6000000;
    if (env->isbg) {
        rect.x = env->clip.x;
        rect.y = env->clip.y;
        rect.w = env->clip.w;
        rect.h = env->clip.h;
        if (rect.w >= 0) {
            if (D_800B5FA0.w - 1 < rect.w) {
                w = D_800B5FA0.w - 1;
            } else {
                w = rect.w;
            }
        } else {
            w = 0;
        }
        rect.w = w;
        if (rect.h >= 0) {
            if (D_800B5FA0.h - 1 < rect.h) {
                h = D_800B5FA0.h - 1;
            } else {
                h = rect.h;
            }
        } else {
            h = 0;
        }
        rect.h = h;
        rect.x -= env->ofs[0];
        rect.y -= env->ofs[1];
        ((u_long *)dr)[len++] = 0x60000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
        ((u_long *)dr)[len++] = *(u_long *)&rect.x;
        ((u_long *)dr)[len++] = *(u_long *)&rect.w;
    }
    ((P_TAG *)dr)->len = (u_char)(len - 1);
}
/* cc97_lib: func_8008CC48 */
/* gnu_as_loads: func_8008CC48 */
/* return_slot_fill: func_8008CC48 */

/* libgpu SetDrawEnv2(dr_env, env) (SYS.OBJ; a static from SDK 4.1 on, named in
   the 3.7 and 4.0 objects): SetDrawEnv as PutDrawEnv and
   DrawOTagEnv use it. The same six setting words (drawing area, drawing
   offset, draw mode, texture window, 0xE6000000) and the same clamp of the
   clear rectangle's w and h to 0 .. GEnv.w - 1 and 0 .. GEnv.h - 1. With
   env->isbg set it picks the clear by alignment: when clip.x and the clamped
   w are both multiples of 64, a GP0 0x02 frame-buffer fill at clip.x, clip.y
   as they are; otherwise SetDrawEnv's GP0 0x60 flat rectangle at clip.x -
   ofs[0], clip.y - ofs[1]. Either is three words, in env->r0 / g0 / b0; the
   tag's length byte becomes 6 or 9 and its address field is left to the caller.
   MARKED SPELLING: `dr = dr_env;`, as in SetDrawEnv (154 of 156 with the
   parameter used directly). The original construct is not known. */
void func_8008CC48(DR_ENV *dr_env, DRAWENV *env)
{
    DR_ENV *dr;
    int len;
    RECT rect;
    u_short w, h;

    len = 1;
    dr = dr_env;
    ((u_long *)dr)[len++] = func_8008CED8(env->clip.x, env->clip.y);
    ((u_long *)dr)[len++] = func_8008CF70(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    ((u_long *)dr)[len++] = func_8008D008(env->ofs[0], env->ofs[1]);
    ((u_long *)dr)[len++] = func_8008CEB8(env->dfe, env->dtd, env->tpage);
    ((u_long *)dr)[len++] = func_8008D024(&env->tw);
    ((u_long *)dr)[len++] = 0xE6000000;
    if (env->isbg) {
        rect.x = env->clip.x;
        rect.y = env->clip.y;
        rect.w = env->clip.w;
        rect.h = env->clip.h;
        if (rect.w >= 0) {
            if (D_800B5FA0.w - 1 < rect.w) {
                w = D_800B5FA0.w - 1;
            } else {
                w = rect.w;
            }
        } else {
            w = 0;
        }
        rect.w = w;
        if (rect.h >= 0) {
            if (D_800B5FA0.h - 1 < rect.h) {
                h = D_800B5FA0.h - 1;
            } else {
                h = rect.h;
            }
        } else {
            h = 0;
        }
        rect.h = h;
        if (rect.x & 0x3F || rect.w & 0x3F) {
            rect.x -= env->ofs[0];
            rect.y -= env->ofs[1];
            ((u_long *)dr)[len++] = 0x60000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
            ((u_long *)dr)[len++] = *(u_long *)&rect.x;
            ((u_long *)dr)[len++] = *(u_long *)&rect.w;
        } else {
            ((u_long *)dr)[len++] = 0x02000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
            ((u_long *)dr)[len++] = *(u_long *)&rect.x;
            ((u_long *)dr)[len++] = *(u_long *)&rect.w;
        }
    }
    ((P_TAG *)dr)->len = (u_char)(len - 1);
}

/* cc97_lib: func_8008CEB8 */
/* gnu_as_loads: func_8008CEB8 */

/* libgpu get_mode(dfe, dtd, tpage) (SYS.OBJ; a static from SDK 4.1 on, named in
   the 2.60 to 4.00 objects): the draw-mode setting word, GP0 0xE1: dither (dtd)
   in bit 9, drawing to the display area allowed (dfe) in bit 10, and tpage's
   bits 0-8 and 11. */
u_long func_8008CEB8(int dfe, int dtd, int tpage)
{
    return (dtd ? 0xE1000200 : 0xE1000000) | (dfe ? 0x400 : 0) | (tpage & 0x9FF);
}
/* cc97_lib: func_8008CED8 */
/* gnu_as_loads: func_8008CED8 */
/* return_slot_fill: func_8008CED8 */

/* libgpu get_cs(x, y) (SYS.OBJ; a static from SDK 4.1 on, named in the 2.60 to
   4.00 objects): the drawing-area top-left setting word, GP0 0xE3, with x
   clamped to 0 .. GEnv.w - 1 and y to 0 .. GEnv.h - 1 (the frame-buffer size
   for the GPU type): x in bits 0-9, y in bits 10-19. */
u_long func_8008CED8(short x, short y)
{
    x = x < 0 ? 0 : (x > D_800B5FA0.w - 1 ? D_800B5FA0.w - 1 : x);
    y = y < 0 ? 0 : (y > D_800B5FA0.h - 1 ? D_800B5FA0.h - 1 : y);
    return 0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
}
/* cc97_lib: func_8008CF70 */
/* gnu_as_loads: func_8008CF70 */
/* return_slot_fill: func_8008CF70 */

/* libgpu get_ce(x, y) (SYS.OBJ; a static from SDK 4.1 on, named in the 2.60 to
   4.00 objects): the drawing-area bottom-right setting word, GP0 0xE4, with x
   clamped to 0 .. GEnv.w - 1 and y to 0 .. GEnv.h - 1 (the frame-buffer size
   for the GPU type): x in bits 0-9, y in bits 10-19. */
u_long func_8008CF70(short x, short y)
{
    x = x < 0 ? 0 : (x > D_800B5FA0.w - 1 ? D_800B5FA0.w - 1 : x);
    y = y < 0 ? 0 : (y > D_800B5FA0.h - 1 ? D_800B5FA0.h - 1 : y);
    return 0xE4000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
}

/* cc97_lib: func_8008D008 */
/* gnu_as_loads: func_8008D008 */
/* return_slot_fill: func_8008D008 */

/* libgpu get_ofs(x, y) (SYS.OBJ; a static from SDK 4.1 on, named in the 2.60 to
   4.00 objects): the drawing-offset setting word, GP0 0xE5 with x in bits 0-10
   and y in bits 11-21. No clamp: each is cut to its 11 bits. */
u_long func_8008D008(short x, short y)
{
    return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
}
/* cc97_lib: func_8008D024 */
/* gnu_as_loads: func_8008D024 */
/* return_slot_fill: func_8008D024 */

/* libgpu get_tw(rect) (SYS.OBJ; a static from SDK 4.1 on, named in the 2.60 to
   4.00 objects): the texture-window setting word, GP0 0xE2, from a RECT in
   pixels. Each field is cut to 8 bits and counted in 8-pixel units: mask x =
   (-w & 0xFF) >> 3 in bits 0-4, mask y = (-h & 0xFF) >> 3 in bits 5-9, offset
   x = (x & 0xFF) >> 3 in bits 10-14, offset y = (y & 0xFF) >> 3 in bits 15-19.
   A null rect returns 0, no setting word at all. */
u_long func_8008D024(RECT *rect)
{
    u_long code[4];

    if (rect) {
        code[0] = (rect->x & 0xFF) >> 3;
        code[2] = (-rect->w & 0xFF) >> 3;
        code[1] = (rect->y & 0xFF) >> 3;
        code[3] = (-rect->h & 0xFF) >> 3;
        return 0xE2000000 | (code[1] << 15) | (code[0] << 10) | (code[3] << 5) | code[2];
    }
    return 0;
}

/* far_syms: D_800B60A8 (target reaches it %hi/%lo) */
/* cc97_lib: func_8008D0A4 */
/* gnu_as_loads: func_8008D0A4 */

/* libgpu _status() (SYS.OBJ, static; the GPU function table's +0x38 entry): the
   GPU status word, GPUSTAT, read through D_800B60A8. */
u_long func_8008D0A4(void)
{
    return *D_800B60A8;
}
/* cc97_lib: func_8008D0BC */
/* gnu_as_loads: func_8008D0BC */
/* return_slot_fill: func_8008D0BC */
/* far_syms: D_800B60B8 (target reaches it %hi/%lo) */
/* far_syms: D_800B60BC (target reaches it %hi/%lo) */
/* far_syms: D_800B60C0 (target reaches it %hi/%lo) */
/* far_syms: D_800B60C4 (target reaches it %hi/%lo) */
extern void set_alarm(void);
extern int func_8008E16C(void);

/* libgpu _otc(ot, n) (SYS.OBJ, static; the GPU function table's +0x2C entry,
   called by ClearOTagR): build a reverse ordering table of n entries with DMA
   channel 6. Sets DPCR bit 27 (channel 6 enable, through D_800B60C4), clears the
   channel's CHCR (D_800B60C0), sets MADR (D_800B60B8) to the table's LAST entry,
   ot + n - 1, BCR (D_800B60BC) to n and CHCR to 0x11000002 (start, trigger,
   backward step: each entry is linked to the one below it and entry 0 gets the
   terminator). Arms the alarm (set_alarm, set_alarm) and waits for CHCR bit
   24 to clear; returns n, or -1 when get_alarm (func_8008E16C) reports the
   timeout first. */
int func_8008D0BC(u_long *ot, int n)
{
    *D_800B60C4 |= 0x08000000;
    *D_800B60C0 = 0;
    *D_800B60B8 = ot - 1 + n;
    *D_800B60BC = n;
    *D_800B60C0 = 0x11000002;
    set_alarm();
    while (*D_800B60C0 & 0x01000000) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    return n;
}
/* cc97_lib: func_8008D19C */
/* gnu_as_loads: func_8008D19C */
/* return_slot_fill: func_8008D19C */
/* reorder_fill: func_8008D19C */
extern u_long D_800E55C8[];
extern u_long D_800E55F0[];
void func_8008D900(u_long *p);  /* TU defines this below */
int _param(int x);  /* TU defines this below */

/* libgpu _clr(rect, color) (SYS.OBJ, static; the GPU function table's +0x0C
   entry, queued by ClearImage and ClearImage2): fill the frame-buffer rectangle
   rect with the 24-bit colour in color. Clamps rect->w to 0..GEnv.w - 1 and
   rect->h to 0..GEnv.h - 1 IN PLACE, builds a primitive list in the static arrays
   D_800E55C8 / D_800E55F0 and sends it with _cwc (func_8008D900); returns 0.
   When rect->x and the clamped rect->w are both multiples of 64 the list is one
   5-word primitive, terminated: 0xE6000000 (mask setting off), the draw-mode word
   0xE1000000 | GPUSTAT's low 11 bits | bit 10 from color's bit 31 (ClearImage2
   sets that bit), GP0 0x02 (frame-buffer fill) with the colour, the x / y word
   and the w / h word. Otherwise it is two primitives. D_800E55C8, 8 words linked
   to D_800E55F0: 0xE3000000, 0xE4FFFFFF and 0xE5000000 (drawing area opened to
   everything, offset 0), 0xE6000000, the same draw-mode word, GP0 0x60 (flat
   rectangle) with the colour, the x / y word, the w / h word. D_800E55F0, 3
   words, terminated: 0xE3 / 0xE4 / 0xE5 with the values _param (_param)
   reads back from the GPU for 3, 4 and 5, which puts the caller's drawing area
   and offset back after the rectangle. */
int func_8008D19C(RECT *rect, u_long color)
{
    rect->w = rect->w < 0 ? 0 : (rect->w > D_800B5FA0.w - 1 ? D_800B5FA0.w - 1 : rect->w);
    rect->h = rect->h < 0 ? 0 : (rect->h > D_800B5FA0.h - 1 ? D_800B5FA0.h - 1 : rect->h);
    if ((rect->x & 0x3F) || (rect->w & 0x3F)) {
        D_800E55C8[0] = ((u_long)D_800E55F0 & 0xFFFFFF) | 0x08000000;
        D_800E55C8[1] = 0xE3000000;
        D_800E55C8[2] = 0xE4FFFFFF;
        D_800E55C8[3] = 0xE5000000;
        D_800E55C8[4] = 0xE6000000;
        D_800E55C8[5] = 0xE1000000 | (*D_800B60A8 & 0x7FF) | ((color >> 31) << 10);
        D_800E55C8[6] = (color & 0xFFFFFF) | 0x60000000;
        D_800E55C8[7] = *(u_long *)&rect->x;
        D_800E55C8[8] = *(u_long *)&rect->w;
        D_800E55F0[0] = 0x03FFFFFF;
        D_800E55F0[1] = _param(3) | 0xE3000000;
        D_800E55F0[2] = _param(4) | 0xE4000000;
        D_800E55F0[3] = _param(5) | 0xE5000000;
    } else {
        D_800E55C8[0] = 0x05FFFFFF;
        D_800E55C8[1] = 0xE6000000;
        D_800E55C8[2] = 0xE1000000 | (*D_800B60A8 & 0x7FF) | ((color >> 31) << 10);
        D_800E55C8[3] = (color & 0xFFFFFF) | 0x02000000;
        D_800E55C8[4] = *(u_long *)&rect->x;
        D_800E55C8[5] = *(u_long *)&rect->w;
    }
    func_8008D900(D_800E55C8);
    return 0;
}
/* cc97_lib: func_8008D3CC */
/* gnu_as_loads: func_8008D3CC */
/* return_slot_fill: func_8008D3CC */

/* libgpu _dws(rect, data) (SYS.OBJ, static; the GPU function table's +0x20 entry,
   queued by LoadImage): write the pixel data at data into the frame-buffer
   rectangle rect. Arms the alarm (set_alarm, set_alarm), clamps rect->w to
   0..GEnv.w and rect->h to 0..GEnv.h IN PLACE, and returns -1 when the rectangle
   holds no pixel. The pixel pairs make (w * h + 1) / 2 words. It waits for
   GPUSTAT bit 26 (ready for a command; -1 when get_alarm, func_8008E16C, reports
   the timeout), sends GP1 0x04000000 (DMA off) and the GP0 words 0x01000000
   (clear cache), 0xA0000000 (copy CPU to VRAM; 0xB0000000 if the local mode were
   set, which nothing does), the x / y word and the w / h word, writes the first
   words % 16 words to GP0 one by one, and leaves the other words / 16 blocks of
   16 words to DMA channel 2: GP1 0x04000002 (DMA CPU to GP0), MADR = the read
   pointer, BCR = blocks << 16 | 0x10, CHCR = 0x01000201. Returns 0 with that
   transfer still running.
   MARKED SPELLING: the local `mode`, set to 0 and never changed, picks the GP0
   word through `mode ? 0xB0000000 : 0xA0000000` (psyz's var_s4; the target keeps
   the 0 in $s5 and tests it). With the constant written: 123 of 143. The
   original construct is not known. */
int func_8008D3CC(RECT *rect, u_long *data)
{
    int words;
    int blocks;
    int rest;
    int mode;

    mode = 0;
    set_alarm();
    rect->w = rect->w < 0 ? 0 : (rect->w > D_800B5FA0.w ? D_800B5FA0.w : rect->w);
    rect->h = rect->h < 0 ? 0 : (rect->h > D_800B5FA0.h ? D_800B5FA0.h : rect->h);
    words = (rect->w * rect->h + 1) / 2;
    if (words <= 0) {
        return -1;
    }
    rest = words % 16;
    blocks = words / 16;
    while (!(*D_800B60A8 & 0x04000000)) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    *D_800B60A8 = 0x04000000;
    *D_800B60A4 = 0x01000000;
    *D_800B60A4 = mode ? 0xB0000000 : 0xA0000000;
    *D_800B60A4 = *(u_long *)&rect->x;
    *D_800B60A4 = *(u_long *)&rect->w;
    while (rest--) {
        *D_800B60A4 = *data++;
    }
    if (blocks) {
        *D_800B60A8 = 0x04000002;
        *D_800B60AC = data;
        *D_800B60B0 = (blocks << 16) | 0x10;
        *D_800B60B4 = 0x01000201;
    }
    return 0;
}

/* cc97_lib: func_8008D608 */
/* gnu_as_loads: func_8008D608 */
/* return_slot_fill: func_8008D608 */
/* far_syms: D_800B60A4 (target reaches it %hi/%lo) */
/* far_syms: D_800B60AC (target reaches it %hi/%lo) */
/* far_syms: D_800B60B0 (target reaches it %hi/%lo) */
/* far_syms: D_800B60B4 (target reaches it %hi/%lo) */
extern void set_alarm(void);
extern int func_8008E16C(void);

/* libgpu _drs(rect, data) (SYS.OBJ, static; the GPU function table's +0x1C entry,
   queued by StoreImage): read the frame-buffer rectangle rect into data. Arms the
   alarm (set_alarm, set_alarm), clamps rect->w to 0..GEnv.w and rect->h to
   0..GEnv.h IN PLACE, and returns -1 when the rectangle holds no pixel. The pixel
   pairs make (w * h + 1) / 2 words. It waits for GPUSTAT bit 26 (ready for a
   command; -1 when get_alarm, func_8008E16C, reports the timeout), sends GP1
   0x04000000 (DMA off) and the GP0 words 0x01000000 (clear cache), 0xC0000000
   (copy VRAM to CPU), the x / y word and the w / h word, waits for GPUSTAT bit 27
   (ready to send VRAM data; -1 on the timeout), reads the first words % 16 words
   from GPUREAD one by one, and leaves the other words / 16 blocks of 16 words to
   DMA channel 2: GP1 0x04000003 (DMA GPUREAD to CPU), MADR = the write pointer,
   BCR = blocks << 16 | 0x10, CHCR = 0x01000200. Returns 0 with that transfer
   still running. */
int func_8008D608(RECT *rect, u_long *data)
{
    int words;
    int blocks;
    int rest;

    set_alarm();
    rect->w = rect->w < 0 ? 0 : (rect->w > D_800B5FA0.w ? D_800B5FA0.w : rect->w);
    rect->h = rect->h < 0 ? 0 : (rect->h > D_800B5FA0.h ? D_800B5FA0.h : rect->h);
    words = (rect->w * rect->h + 1) / 2;
    if (words <= 0) {
        return -1;
    }
    rest = words % 16;
    blocks = words / 16;
    while (!(*D_800B60A8 & 0x04000000)) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    *D_800B60A8 = 0x04000000;
    *D_800B60A4 = 0x01000000;
    *D_800B60A4 = 0xC0000000;
    *D_800B60A4 = *(u_long *)&rect->x;
    *D_800B60A4 = *(u_long *)&rect->w;
    while (!(*D_800B60A8 & 0x08000000)) {
        if (func_8008E16C()) {
            return -1;
        }
    }
    while (rest--) {
        *data++ = *D_800B60A4;
    }
    if (blocks) {
        *D_800B60A8 = 0x04000003;
        *D_800B60AC = data;
        *D_800B60B0 = (blocks << 16) | 0x10;
        *D_800B60B4 = 0x01000200;
    }
    return 0;
}

/* far_syms: D_800E5610 (target reaches it %hi/%lo) */
/* cc97_lib: func_8008D888 */
/* gnu_as_loads: func_8008D888 */
/* return_slot_fill: func_8008D888 */
extern u_char D_800E5610[];

/* libgpu _ctl(word) (SYS.OBJ, static; the GPU function table's +0x10 entry):
   write the control word to GP1 (through D_800B60A8) and keep its low byte in
   the shadow table ctlbuf (D_800E5610, 0x100 bytes) at the word's command byte
   (bits 24-31), where _getctl reads it back. */
void func_8008D888(u_long word)
{
    *D_800B60A8 = word;
    D_800E5610[word >> 24] = word;
}

/* cc97_lib: func_8008D8AC */
/* gnu_as_loads: func_8008D8AC */

/* libgpu _getctl(n) (SYS.OBJ, static; the GPU function table's +0x28 entry): the
   low byte of the last control word _ctl sent with command byte n, from the
   shadow table ctlbuf (D_800E5610). */
int func_8008D8AC(int n)
{
    return D_800E5610[n];
}
/* cc97_lib: func_8008D8C0 */
/* gnu_as_loads: func_8008D8C0 */

/* libgpu _cwb(data, n) (SYS.OBJ, static; the GPU function table's +0x14 entry):
   write n command words to the GPU by hand. Sends GP1 0x04000000 (DMA off), then
   the n words at data to GP0 one after another. Returns 0. */
int func_8008D8C0(u_long *data, int n)
{
    int i;

    *D_800B60A8 = 0x04000000;
    for (i = n - 1; i != -1; i--) {
        *D_800B60A4 = *data++;
    }
    return 0;
}
/* cc97_lib: func_8008D900 */
/* gnu_as_loads: func_8008D900 */
/* return_slot_fill: func_8008D900 */

/* libgpu _cwc(p) (SYS.OBJ, static; the GPU function table's +0x18 entry): send
   the primitive list at p to the GPU by DMA. GP1 0x04000002 (DMA CPU to GP0), then
   DMA channel 2: MADR = p, BCR = 0, CHCR = 0x01000401 (start, linked-list mode,
   from memory). It returns at once, with the transfer running. */
void func_8008D900(u_long *p)
{
    *D_800B60A8 = 0x04000002;
    *D_800B60AC = p;
    *D_800B60B0 = 0;
    *D_800B60B4 = 0x01000401;
}
/* cc97_lib: _param */
/* gnu_as_loads: _param */

/* libgpu _param(x) (SYS.OBJ, static; the GPU function table's +0x30 entry): read
   GPU parameter x. Sends GP1 0x10000000 | x (get GPU info) and returns the low 24
   bits of the answer read from GPUREAD. _clr asks for 3, 4 and 5: the drawing
   area's top-left, its bottom-right and the drawing offset. */
/* func_8008D948 */
int _param(int x)
{
    *D_800B60A8 = x | 0x10000000;
    return *D_800B60A4 & 0xFFFFFF;
}
