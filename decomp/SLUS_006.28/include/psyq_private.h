/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/libsnd_i.h,
   src/main/psxsdk/libspu/libspu_internal.h), psyz
   (decomp/src/libgpu/libgpu_private.h, decomp/src/libgpu/sys.c,
   decomp/src/libsnd/libsnd_private.h, decomp/src/libspu/libspu_private.h) and
   psy-q-decomp (src/snd/snd_defs.h, src/spu/libspu_i.h), all MIT-licensed: the
   notices are in LICENSES/, and NOTICE.md says more. */
/* psyq_private.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef PSYQ_PRIVATE_H
#define PSYQ_PRIVATE_H

#include "layout.h"

typedef void (*IntrHandler)(void);

/* libcd ISO9660.OBJ's directory cache: one record per path-table entry, 128 of
   them at D_800E7760. CD_newmedia (func_80092800) builds it from the disc's
   path table; func_80092AC4 searches it by parent and name; CD_cachefile
   (func_80092B68) reads a directory's sector from it. */
typedef struct {
    /* +0x00 the directory's number, its 1-based place in the path table:
       CD_newmedia writes i + 1 and prints it in its debug list. */
    s32 number;
    /* +0x04 the parent directory's number (path-table record byte 6), written
       by CD_newmedia; 0 ends the table (CD_newmedia writes 0 after the last
       record). func_80092AC4 stops at 0 and matches it. */
    s32 parent;
    /* +0x08 the directory's extent, its first sector: CD_newmedia copies the
       record's four bytes there through CdlLBA (lwl/lwr); CD_cachefile reads
       that sector. */
    s32 lba;
    /* +0x0C the directory's name with its NUL: CD_newmedia memcpys the
       record's name and ends it; func_80092AC4 compares it with strcmp. */
    char name[0x20];
} CdlDIR;

LAYOUT_SIZE(CdlDIR, 0x2c);
LAYOUT_AT(CdlDIR, number, number, 0x00);
LAYOUT_AT(CdlDIR, parent, parent, 0x04);
LAYOUT_AT(CdlDIR, lba, lba, 0x08);
LAYOUT_AT(CdlDIR, name, name, 0x0c);

/* An ISO9660 32-bit field at any alignment, read as a word or as four bytes:
   copying the byte struct `i` makes the copy byte-safe (lwl/lwr). CD_newmedia
   (func_80092800) takes the path table's LBA from the primary volume
   descriptor (+0x8C) into a local and each path record's extent (+2) into
   CdlDIR.lba with it. */
typedef union {
    /* +0x00 the field as one 32-bit number: CD_newmedia passes lba.addr to the
       sector read and prints it. */
    s32 addr;
    /* +0x00 the same four bytes as a struct, copied whole (the unaligned
       copy). */
    struct { u8 b0; u8 b1; u8 b2; u8 b3; } i;
} CdlLBA;

LAYOUT_SIZE(CdlLBA, 0x04);
LAYOUT_AT(CdlLBA, addr, addr, 0x00);
LAYOUT_AT(CdlLBA, i, i, 0x00);

/* libcd's CD audio mixer setting, four volumes: CdMix (libcd_sys_6.c) passes
   it to CD_vol (libcd_bios_1.c), which writes the bytes to the CD controller's
   mixer registers; CD_initvol builds one on its stack. Sony's public type,
   from libcd.h. Its fields as this program uses them:
   - val0 (+0x00): CD_vol writes it to the mixer's index 2 register 2 (CD
     left into SPU left); CD_initvol uses 0x80.
   - val1 (+0x01): CD_vol: index 2 register 3 (CD left into SPU right);
     CD_initvol 0.
   - val2 (+0x02): CD_vol: index 3 register 1 (CD right into SPU right);
     CD_initvol 0x80.
   - val3 (+0x03): CD_vol: index 3 register 2 (CD right into SPU left);
     CD_initvol 0. */

LAYOUT_SIZE(CdlATV, 0x04);
LAYOUT_AT(CdlATV, val0, val0, 0x00);
LAYOUT_AT(CdlATV, val1, val1, 0x01);
LAYOUT_AT(CdlATV, val2, val2, 0x02);
LAYOUT_AT(CdlATV, val3, val3, 0x03);

/* libcd BIOS.OBJ's timeout alarm (D_800E7120): cd_set_alarm arms it before a
   wait, cd_get_alarm tests it in CD_sync / CD_ready / CD_cw's loops and, past
   the deadline, prints the waiter's name and flushes the controller. */
typedef struct {
    /* +0x00 the deadline in VSync counts: cd_set_alarm writes VSync(-1) + 960;
       cd_get_alarm fires once VSync(-1) passes it. */
    int time;
    /* +0x04 the polls so far: cd_set_alarm writes 0, cd_get_alarm counts it
       and fires above 0x3C0000. */
    int count;
    /* +0x08 the waiting function's name (cd_set_alarm's argument), printed by
       cd_get_alarm on a timeout. */
    char *name;
} CdAlarm;

LAYOUT_SIZE(CdAlarm, 0x0c);
LAYOUT_AT(CdAlarm, time, time, 0x00);
LAYOUT_AT(CdAlarm, count, count, 0x04);
LAYOUT_AT(CdAlarm, name, name, 0x08);

/* The header of a streamed (STR) sector in libcd's ring buffer (D_800E7100,
   0x20 bytes a sector): data_ready_callback (libcd_c_004_1.c), StFreeRing and
   StGetNext (libcd_c_009.c) and StCdInterrupt (libcd_c_011_1.c) read and mark
   it. Sony's public type, from libcd.h (also libds.h). Its fields as this
   program uses them:
   - id (+0x00): the sector's state: data_ready_callback writes 2 (through a
     u16 pointer, which the match needs), StGetNext turns 2 into 4 and 1
     into 0, StFreeRing frees a frame only at 4 (it reads the word as s16).
   - type (+0x02): the sector's type; libcd's code here does not touch it.
   - secCount (+0x04): the sector's number within its frame; libcd's code
     here does not touch it.
   - nSectors (+0x06): the frame's sector count: StFreeRing reads it (into
     an s16) and clears that many headers.
   - frameCount (+0x08): the frame number: data_ready_callback keeps it in
     D_800E9564.
   - frameSize (+0x0C): the frame's size; libcd's code here does not touch
     it.
   - width (+0x10): the picture's width; libcd's code here does not touch
     it.
   - height (+0x12): the picture's height; libcd's code here does not touch
     it.
   - dummy1 (+0x14): a word libcd's code here does not touch.
   - dummy2 (+0x18): a word libcd's code here does not touch.
   - loc (+0x1C): the sector's disc position: StCdInterrupt writes the
     position it read from the drive into the header at D_800E9578;
     data_ready_callback keeps it in D_800E9560. */

LAYOUT_SIZE(StHEADER, 0x20);
LAYOUT_AT(StHEADER, id, id, 0x00);
LAYOUT_AT(StHEADER, type, type, 0x02);
LAYOUT_AT(StHEADER, secCount, secCount, 0x04);
LAYOUT_AT(StHEADER, nSectors, nSectors, 0x06);
LAYOUT_AT(StHEADER, frameCount, frameCount, 0x08);
LAYOUT_AT(StHEADER, frameSize, frameSize, 0x0c);
LAYOUT_AT(StHEADER, width, width, 0x10);
LAYOUT_AT(StHEADER, height, height, 0x12);
LAYOUT_AT(StHEADER, dummy1, dummy1, 0x14);
LAYOUT_AT(StHEADER, dummy2, dummy2, 0x18);
LAYOUT_AT(StHEADER, loc, loc, 0x1c);

/* One root counter's registers, 0x10 bytes each from 0x1F801100 (counters 0 to
   2), reached through libapi COUNTER.OBJ's pointer D_800B6104 (_counters):
   SetRCnt and GetRCnt (libapi_counter_1.c), ResetRCnt (libapi_counter_2.c). */
typedef struct {
    /* +0x00 the current count: GetRCnt returns it, ResetRCnt
       (libapi_counter_2.c) writes 0. */
    u16 rootCounter;
    /* +0x02 the count register's upper half; nothing reads or writes it. */
    u16 pad_02;
    /* +0x04 the mode register: SetRCnt writes 0, then the mode it builds from
       the flags (0x48 and up). */
    s16 mode;
    /* +0x06 the mode register's upper half; nothing reads or writes it. */
    u16 pad_06;
    /* +0x08 the target value: SetRCnt writes its argument. */
    s16 target;
    /* +0x0A the target register's upper half; nothing reads or writes it. */
    u16 pad_0a;
    /* +0x0C the unused fourth register of the 0x10-byte block; nothing reads
       or writes it. */
    u32 pad_0c;
} Counter;

LAYOUT_SIZE(Counter, 0x10);
LAYOUT_AT(Counter, rootCounter, rootCounter, 0x00);
LAYOUT_AT(Counter, pad_02, pad_02, 0x02);
LAYOUT_AT(Counter, mode, mode, 0x04);
LAYOUT_AT(Counter, pad_06, pad_06, 0x06);
LAYOUT_AT(Counter, target, target, 0x08);
LAYOUT_AT(Counter, pad_0a, pad_0a, 0x0a);
LAYOUT_AT(Counter, pad_0c, pad_0c, 0x0c);

/* libetc INTR.OBJ's state intrEnv (D_800B68A8): the interrupt handlers, the
   masks saved around a stop, the jmp_buf HookEntryInt is given and the stack
   interrupts run on. startIntr, trapIntr, setIntr, stopIntr and restartIntr
   (libapi_a25.c) keep it; CheckCallback (libetc_intr_1.c) reads inInterrupt. */
typedef struct {
    /* +0x00 1 while the interrupt entry is hooked: startIntr and restartIntr
       set it, stopIntr clears it. */
    u16 interruptsInitialized;
    /* +0x02 1 while trapIntr runs: CheckCallback returns it. */
    u16 inInterrupt;
    /* +0x04 one handler per I_STAT bit: setIntr writes them, trapIntr calls
       them. */
    IntrHandler handlers[11];
    /* +0x30 the I_STAT bits that have a handler: setIntr sets and clears its
       bits, trapIntr masks I_STAT with it. */
    u16 enabledInterruptsMask;
    /* +0x32 I_MASK as stopIntr (func_8009515C) found it; restartIntr
       (func_800951FC) writes it back. */
    u16 savedMask;
    /* +0x34 DPCR as stopIntr found it; restartIntr writes it back. */
    s32 savedPcr;
    /* +0x38 the jmp_buf startIntr gives HookEntryInt: setjmp fills it,
       startIntr points buf[1] (the stack pointer) at stack[1004]. */
    s32 buf[12];
    /* +0x68 the stack interrupts run on (4 KiB); only its address is taken. */
    s32 stack[0x400];
} IntrEnv;

LAYOUT_SIZE(IntrEnv, 0x1068);
LAYOUT_AT(IntrEnv, interruptsInitialized, interruptsInitialized, 0x00);
LAYOUT_AT(IntrEnv, inInterrupt, inInterrupt, 0x02);
LAYOUT_AT(IntrEnv, handlers, handlers, 0x04);
LAYOUT_AT(IntrEnv, enabledInterruptsMask, enabledInterruptsMask, 0x30);
LAYOUT_AT(IntrEnv, savedMask, savedMask, 0x32);
LAYOUT_AT(IntrEnv, savedPcr, savedPcr, 0x34);
LAYOUT_AT(IntrEnv, buf, buf, 0x38);
LAYOUT_AT(IntrEnv, stack, stack, 0x68);

/* libetc INTR.OBJ's callback table (0x800B7910, reached through the pointer
   D_800B7930, pCallbacks): the API functions ResetCallback ... RestartCallback
   (libetc_intr_1.c) call through it; startIntr fills the DMA and VSync
   entries. */
typedef struct {
    /* +0x00 INTR.OBJ's version string (intr.c 1.75); not read. */
    char *rcsid;
    /* +0x04 0 until startIntr stores startIntrDMA's result; DMACallback calls
       it. */
    void *(*DMACallback)();
    /* +0x08 setIntr (func_80095014); InterruptCallback calls it. */
    void *(*InterruptCallback)();
    /* +0x0C startIntr (func_80094D6C); ResetCallback calls it. */
    void *(*ResetCallback)();
    /* +0x10 stopIntr (func_8009515C); StopCallback calls it. */
    void *(*StopCallback)();
    /* +0x14 0 until startIntr stores startIntrVSync's result; VSyncCallback
       and VSyncCallbacks call it. */
    void *(*VSyncCallbacks)();
    /* +0x18 restartIntr (func_800951FC); RestartCallback calls it. */
    void *(*RestartCallback)();
    /* +0x1C &intrEnv (D_800B68A8). */
    IntrEnv *intrEnv;
} IntrCallbacks;

LAYOUT_SIZE(IntrCallbacks, 0x20);
LAYOUT_AT(IntrCallbacks, rcsid, rcsid, 0x00);
LAYOUT_AT(IntrCallbacks, DMACallback, DMACallback, 0x04);
LAYOUT_AT(IntrCallbacks, InterruptCallback, InterruptCallback, 0x08);
LAYOUT_AT(IntrCallbacks, ResetCallback, ResetCallback, 0x0c);
LAYOUT_AT(IntrCallbacks, StopCallback, StopCallback, 0x10);
LAYOUT_AT(IntrCallbacks, VSyncCallbacks, VSyncCallbacks, 0x14);
LAYOUT_AT(IntrCallbacks, RestartCallback, RestartCallback, 0x18);
LAYOUT_AT(IntrCallbacks, intrEnv, intrEnv, 0x1c);

/* An entry of the kernel's interrupt queues (SysEnqIntRP / SysDeqIntRP): the
   kernel calls func1 and, when it returns non-zero, func2. Two of them: libapi
   PAD.OBJ's D_800EA8E0 (queue 1, libapi_pad_3.c) and libpad PADMAIN.OBJ's
   D_800EAD40 (queue 2, libpad_padmain.c). */
typedef struct {
    /* +0x00 the queue's link, the kernel's: pad_set_vsync (libapi_pad_3.c) and
       _padSetVsyncParam (libpad_padmain.c) write 0 before the entry is queued. */
    s32 next;
    /* +0x04 the handler the kernel runs when func1 says so (func_800A140C;
       _padIntPad). */
    s32 (*func2)(void);
    /* +0x08 the verifier (func_800A1474; _padIsVsync). */
    s32 (*func1)(void);
    /* +0x0C the entry's fourth word: both entries are given 0 before
       enqueueing; nothing here reads it. */
    s32 unk_0c;
} PadIntRP;

LAYOUT_SIZE(PadIntRP, 0x10);
LAYOUT_AT(PadIntRP, next, next, 0x00);
LAYOUT_AT(PadIntRP, func2, func2, 0x04);
LAYOUT_AT(PadIntRP, func1, func1, 0x08);
LAYOUT_AT(PadIntRP, unk_0c, unk_0c, 0x0c);

/* The interrupt controller's two registers at 0x1F801070, as libpad
   PADMAIN.OBJ reaches them through D_800B84E8 (libpad_padmain.c). */
typedef struct {
    /* +0x00 I_STAT, the pending interrupts: _padIsVsync tests bit 0, the SIO
       loops wait for bit 0x80, and writes of ~1 / ~0x80 acknowledge them
       (_padStartCom, _padSioRW). */
    u32 stat;
    /* +0x04 I_MASK, the enabled interrupts: _padIsVsync tests bit 0,
       _padStartCom sets it. */
    u32 mask;
} PadIntr;

LAYOUT_SIZE(PadIntr, 0x08);
LAYOUT_AT(PadIntr, stat, stat, 0x00);
LAYOUT_AT(PadIntr, mask, mask, 0x04);

/* The SIO0 (controller and memory-card port) registers at 0x1F801040, as
   libpad reaches them through D_800B84EC / D_800B8504 / D_800B8530
   (libpad_padmain.c, libpad_padif.c, libpad_padportd.c). */
typedef struct {
    /* +0x00 the data register: _padInitSioMode (1, 0x42) and _padSioRW write
       the bytes to send; _padSioRW and func_800A6E18 (libpad_padif.c) read the
       replies. */
    u8 data;
    /* +0x01 the rest of the data word; nothing reads or writes it. */
    u8 pad_01[3];
    /* +0x04 the status register's low half: the pad code tests its bits (0x200
       in _padInitSioMode). */
    u16 stat;
    /* +0x06 the status register's upper half; nothing reads or writes it. */
    u16 pad_06;
    /* +0x08 the mode register: _padInitSioMode writes 0xD. */
    u16 mode;
    /* +0x0A the control register: _padInitSioMode writes 0x40, 0, then 0x1003
       / 0x3003; _padIntPad tests bit 2 and writes 0. */
    u16 ctrl;
    /* +0x0C a register nothing here reads or writes. */
    u16 pad_0c;
    /* +0x0E the baud-rate register: _padInitSioMode and _padIntPad write 0x88;
       the multitap code compares it with 0x22. */
    u16 baud;
} PadSio;

LAYOUT_SIZE(PadSio, 0x10);
LAYOUT_AT(PadSio, data, data, 0x00);
LAYOUT_AT(PadSio, pad_01, pad_01, 0x01);
LAYOUT_AT(PadSio, stat, stat, 0x04);
LAYOUT_AT(PadSio, pad_06, pad_06, 0x06);
LAYOUT_AT(PadSio, mode, mode, 0x08);
LAYOUT_AT(PadSio, ctrl, ctrl, 0x0a);
LAYOUT_AT(PadSio, pad_0c, pad_0c, 0x0c);
LAYOUT_AT(PadSio, baud, baud, 0x0e);

/* libgpu SYS.OBJ's GPU environment GEnv (D_800B5FA0): the GPU type, the queue
   and debug modes, the frame-buffer size, the DrawSync callback and the
   drawing and display environments in force. ResetGraph and the SetGraph*
   functions (libgpu_sys_1.c), _addque2 and _exeque (libgpu_sys_3.c), StoreImage2
   ... DrawOTag2 (libgpu_sys_5.c). */
typedef struct {
    /* +0x00 the GPU type, _reset()'s result: ResetGraph writes it; it indexes
       the size tables. */
    u_char type;
    /* +0x01 the queue mode: ResetGraph writes 1, SetGraphQueue its argument;
       _addque2 reads it. */
    u_char queue;
    /* +0x02 the debug level: SetGraphDebug writes it; the functions print at 2
       and above. */
    u_char level;
    /* +0x03 the display-reverse flag: SetGraphDebug prints it, PutDispEnv
       reads it; nothing in this program writes it. */
    u_char reverse;
    /* +0x04 the frame buffer's width for the GPU type: ResetGraph writes it
       from D_800B6020. */
    short w;
    /* +0x06 its height: ResetGraph writes it from D_800B602C. */
    short h;
    /* +0x08 _addque2 sets it to 1; _exeque clears it when it calls the
       DrawSync callback. */
    volatile int cb_pending;
    /* +0x0C the DrawSync callback: DrawSyncCallback writes it, _exeque calls
       it. */
    void (*drawsync)(void);
    /* +0x10 the drawing environment in force: ResetGraph fills it with 0xFF,
       PutDrawEnv copies the caller's in, GetDrawEnv copies it out. */
    DRAWENV draw;
    /* +0x6C the display environment in force: ResetGraph and SetDispMask(0)
       fill it with 0xFF; PutDispEnv compares the caller's with it field by
       field and copies it in. */
    DISPENV disp;
} GpuEnv;

LAYOUT_SIZE(GpuEnv, 0x80);
LAYOUT_AT(GpuEnv, type, type, 0x00);
LAYOUT_AT(GpuEnv, queue, queue, 0x01);
LAYOUT_AT(GpuEnv, level, level, 0x02);
LAYOUT_AT(GpuEnv, reverse, reverse, 0x03);
LAYOUT_AT(GpuEnv, w, w, 0x04);
LAYOUT_AT(GpuEnv, h, h, 0x06);
LAYOUT_AT(GpuEnv, cb_pending, cb_pending, 0x08);
LAYOUT_AT(GpuEnv, drawsync, drawsync, 0x0c);
LAYOUT_AT(GpuEnv, draw, draw, 0x10);
LAYOUT_AT(GpuEnv, disp, disp, 0x6c);

/* libgpu SYS.OBJ's function table (D_800B5F58, reached through the pointer
   D_800B5F98): every libgpu entry point calls through it (libgpu_sys_1.c,
   libgpu_sys_5.c). */
typedef struct {
    /* +0x00 the object's version string; not read. */
    char *ver;
    /* +0x04 queue a routine with two arguments. */
    int (*addque)(int (*exec)(u_long p1, u_long p2), u_long p1, u_long p2);
    /* +0x08 queue a routine with a copy of len words of data (_addque2,
       libgpu_sys_3.c). */
    int (*addque2)(int (*exec)(u_long p1, u_long p2), u_long p1, int len, u_long p2);
    /* +0x0C clear a rectangle to a colour. */
    int (*clr)(RECT *rect, u_long color);
    /* +0x10 write one GP1 word. Returns int: PutDispEnv matches only with its
       three calls taking a value. */
    int (*ctl)(u_long word);
    /* +0x14 write n GP0 words. */
    int (*cwb)(u_long *data, int n);
    /* +0x18 send a primitive list. */
    void (*cwc)(u_long *p);
    /* +0x1C read a rectangle from the frame buffer. */
    int (*drs)(RECT *rect, u_long *p);
    /* +0x20 write a rectangle to the frame buffer. */
    int (*dws)(RECT *rect, u_long *p);
    /* +0x24 run the queue (_exeque). */
    int (*exeque)(void);
    /* +0x28 read a GPU status item. */
    int (*getctl)(int n);
    /* +0x2C clear an ordering table. */
    int (*otc)(u_long *ot, int n);
    /* +0x30 read a GPU parameter. */
    int (*param)(int x);
    /* +0x34 reset the GPU (ResetGraph calls it). */
    int (*reset)(int mode);
    /* +0x38 read GPUSTAT. */
    u_long (*status)(void);
    /* +0x3C wait for the GPU (_sync). */
    int (*sync)(int mode);
} GpuTable;

LAYOUT_SIZE(GpuTable, 0x40);
LAYOUT_AT(GpuTable, ver, ver, 0x00);
LAYOUT_AT(GpuTable, addque, addque, 0x04);
LAYOUT_AT(GpuTable, addque2, addque2, 0x08);
LAYOUT_AT(GpuTable, clr, clr, 0x0c);
LAYOUT_AT(GpuTable, ctl, ctl, 0x10);
LAYOUT_AT(GpuTable, cwb, cwb, 0x14);
LAYOUT_AT(GpuTable, cwc, cwc, 0x18);
LAYOUT_AT(GpuTable, drs, drs, 0x1c);
LAYOUT_AT(GpuTable, dws, dws, 0x20);
LAYOUT_AT(GpuTable, exeque, exeque, 0x24);
LAYOUT_AT(GpuTable, getctl, getctl, 0x28);
LAYOUT_AT(GpuTable, otc, otc, 0x2c);
LAYOUT_AT(GpuTable, param, param, 0x30);
LAYOUT_AT(GpuTable, reset, reset, 0x34);
LAYOUT_AT(GpuTable, status, status, 0x38);
LAYOUT_AT(GpuTable, sync, sync, 0x3c);

/* One entry of libgpu's command queue _que (D_800E5718, 64 entries, 0x1800
   bytes of bss): _addque2 (libgpu_sys_3.c) fills it, _exeque (libgpu_sys_3.c,
   libgpu_sys_4.c) runs it. */
typedef struct {
    /* +0x00 the deferred GPU routine: _addque2 writes it, _exeque calls it
       with p1 and p2. */
    int (*func)(u_long p1, u_long p2);
    /* +0x04 its first argument (written by _addque2). */
    u_long p1;
    /* +0x08 its second argument (written by _addque2). */
    u_long p2;
    /* +0x0C _addque2's copy of the caller's data, up to 21 words; p1 then
       points here. */
    u_long buf[21];
} GpuQue;

LAYOUT_SIZE(GpuQue, 0x60);
LAYOUT_AT(GpuQue, func, func, 0x00);
LAYOUT_AT(GpuQue, p1, p1, 0x04);
LAYOUT_AT(GpuQue, p2, p2, 0x08);
LAYOUT_AT(GpuQue, buf, buf, 0x0c);

/* libsnd's score record, the playback state of one sequence: _ss_score
   (D_800E9628) holds a pointer per access number to an array of them, one per
   sequence of a SEP (record 0 for a SEQ). _SsInitSoundSeq opens it, the SEQ
   reader and its event handlers (libsnd's de_, cc_ and midi members,
   midiread.o, next.o, replay.o) read it, the play-mode, crescendo, pause,
   tempo, volume and close functions (playmode.o, cres.o, pause.o, tempo.o,
   ssvol.o, sstable.o, ssclose.o) and the voice manager (the vm_ members) use
   it. */
typedef struct {
    /* +0x00 the next unread byte of the SEQ data: every handler that takes
       data advances it; _SsSeqGetEof rewinds it. */
    u8 *read_pos;
    /* +0x04 the first event (after the header and the first delta time):
       _SsInitSoundSeq writes it; the rewind point at the end of a track unless
       flag 0x400 is set (_SsSeqGetEof, SsSeqPlay's replay). */
    u8 *start_pos;
    /* +0x08 where the NRPN loop jumps back to: _SsContNrpn2 stores read_pos
       here and loads it back; starts as start_pos, reset at the end of a
       track. */
    u8 *loop_pos;
    /* +0x0C the rewind point while flag 0x400 is set (_SsSeqGetEof);
       _SsInitSoundSeq starts it as start_pos. */
    u8 *rewind_pos;
    /* +0x10 the end point while flag 0x400 is set: _SsGetSeqData ends the
       track once it has read the byte there; _SsInitSoundSeq writes 0. */
    u8 *end_pos;
    /* +0x14 1 while playing: Snd_SetPlayMode, _SsSndNextSep and _SsSndReplay
       write 1; _SsSndPause, _SsSndStop, the end of the track and
       _SsInitSoundSeq 0. */
    u8 play_mode;
    /* +0x15 1 once a repeat count has been taken for the armed loop
       (_SsContDataEntry). */
    u8 loop_on;
    /* +0x16 the running status: _SsGetSeqData keeps the last status byte's
       high nibble (0x90, 0xB0, 0xC0, 0xE0) or 0xFF after a meta event and
       dispatches on it; _SsInitSoundSeq writes 0. */
    u8 running_status;
    /* +0x17 the MIDI channel of the event being read: the index into panpot,
       programs and vol. */
    u8 channel;
    /* +0x18 the RPN number's low byte (control change 100). */
    u8 rpn_lsb;
    /* +0x19 the RPN number's high byte (control change 101). */
    u8 rpn_msb;
    /* +0x1A the NRPN control change 98 value: the attribute handler's index. */
    u8 nrpn_lsb;
    /* +0x1B the NRPN control change 99 value: the tone number. */
    u8 nrpn_msb;
    /* +0x1C 1 while a loop start has been seen and its repeat count not taken
       yet (_SsContNrpn2). */
    u8 loop_armed;
    /* +0x1D the loop's repeat count, taken from a data entry. */
    u8 loop_count;
    /* +0x1E the RPN bytes received (2 = both). */
    u8 rpn_count;
    /* +0x1F the NRPN bytes received (2 = both). */
    u8 nrpn_count;
    /* +0x20 the song repetitions wanted: SsSetLoop and Snd_SetPlayMode write
       it; 0 plays for ever; _SsInitSoundSeq writes 1. */
    s8 l_count;
    /* +0x21 the repetitions played: counted at each end of a track and
       compared, signed, with l_count. */
    s8 l_count_done;
    /* +0x22 the score to start when this one ends (_SsSndNextSep); -1 = none. */
    s8 next_seq;
    /* +0x23 that score's sequence. */
    s8 next_sep;
    /* +0x24 the SEQ header's time signature, numerator (header byte 13). */
    u8 rhythm_n;
    /* +0x25 its denominator (header byte 14). */
    u8 rhythm_d;
    /* +0x26 the VAB the score plays. */
    s8 vab_id;
    /* +0x27 per channel, the pan: 0x40 at open, the pan handler writes it. */
    u8 panpot[16];
    /* +0x37 per channel, the program: the channel number at open, the program
       change writes it. */
    u8 programs[16];
    /* +0x47 a byte nothing reads or writes. */
    u8 pad_47;
    /* +0x48 the crescendo's whole volume change, signed. */
    s16 cres_vol;
    /* +0x4A the part of the crescendo applied so far: _SsSndCrescendo reads it
       signed and adds to it; SsSetTableSize and _SsClose write 0. */
    s16 cres_done;
    /* +0x4C SsSetTableSize and _SsClose (func_80095758) write 0; nothing read
       here says more. */
    s16 unk_4c;
    /* +0x4E the tempo ramp: above 0, one tempo unit every that many ticks; 0
       or below, the negated step per tick (_SsSndTempo). */
    s16 tempo_step;
    /* +0x50 delta ticks per quarter note (SEQ header bytes 8-9). */
    s16 resolution;
    /* +0x52 _SsSeqPlay's countdown, reloaded from tick_step; -1 = none. */
    s16 tick_wait;
    /* +0x54 the delta time _SsSeqPlay takes per service tick: _SsInitSoundSeq
       and _SsSndTempo work it out. */
    s16 tick_step;
    /* +0x56 tick_step as the header's tempo gives it: _SsInitSoundSeq keeps
       it, the replay puts it back into tick_step. */
    s16 hdr_tick_step;
    /* +0x58 the score's left volume. */
    u16 voll;
    /* +0x5A the score's right volume. */
    u16 volr;
    /* +0x5C the left volume read back at the end of each crescendo tick. */
    u16 cres_voll;
    /* +0x5E the right one. */
    u16 cres_volr;
    /* +0x60 per channel, the volume: 0x7F at open, the main-volume handler
       writes it, the voice manager scales the note's volume by it. */
    s16 vol[16];
    /* +0x80 the channel mute mask, a bit per channel: _SsInitSoundSeq writes
       0, _SsNoteOn plays a channel only while its bit is clear. */
    s16 mute_mask;
    /* +0x82 a half-word nothing reads or writes. */
    s16 pad_82;
    /* +0x84 the first event's delta time: _SsInitSoundSeq keeps it, the replay
       puts it back into delta_value. */
    s32 start_delta;
    /* +0x88 the sum of every delta time read (_SsReadDeltaValue adds to it; 0
       at open and at a rewind). */
    s32 delta_sum;
    /* +0x8C the header's tempo in quarter notes a minute: _SsInitSoundSeq
       works it out (signed divisions). */
    s32 hdr_tempo;
    /* +0x90 the delta time left until the next event, in tenths of a tick:
       _SsReadDeltaValue's result. */
    s32 delta_value;
    /* +0x94 the current tempo: _SsInitSoundSeq writes hdr_tempo; _SsSndTempo
       steps it towards tempo_goal (its unsigned divide needs u32). */
    u32 tempo;
    /* +0x98 the play-flag word: 1 playing, 2 pause, 4 stop, 8 replay
       requested; 0x10 / 0x20 a volume ramp, 0x40 / 0x80 a tempo ramp running;
       0x200 set when the score ends; 0x400 the point-to-point mode. */
    s32 flags;
    /* +0x9C the crescendo's length in ticks. */
    s32 cres_time;
    /* +0xA0 the crescendo ticks gone: _SsSndCrescendo counts it. */
    s32 cres_tick;
    /* +0xA4 SsSetTableSize and _SsClose write 0; nothing read here says more. */
    s32 unk_a4;
    /* +0xA8 the tempo ramp's ticks left, signed. */
    s32 tempo_count;
    /* +0xAC the tempo the ramp ends at. */
    u32 tempo_goal;
} SsScore;

LAYOUT_SIZE(SsScore, 0xb0);
LAYOUT_AT(SsScore, read_pos, read_pos, 0x00);
LAYOUT_AT(SsScore, start_pos, start_pos, 0x04);
LAYOUT_AT(SsScore, loop_pos, loop_pos, 0x08);
LAYOUT_AT(SsScore, rewind_pos, rewind_pos, 0x0c);
LAYOUT_AT(SsScore, end_pos, end_pos, 0x10);
LAYOUT_AT(SsScore, play_mode, play_mode, 0x14);
LAYOUT_AT(SsScore, loop_on, loop_on, 0x15);
LAYOUT_AT(SsScore, running_status, running_status, 0x16);
LAYOUT_AT(SsScore, channel, channel, 0x17);
LAYOUT_AT(SsScore, rpn_lsb, rpn_lsb, 0x18);
LAYOUT_AT(SsScore, rpn_msb, rpn_msb, 0x19);
LAYOUT_AT(SsScore, nrpn_lsb, nrpn_lsb, 0x1a);
LAYOUT_AT(SsScore, nrpn_msb, nrpn_msb, 0x1b);
LAYOUT_AT(SsScore, loop_armed, loop_armed, 0x1c);
LAYOUT_AT(SsScore, loop_count, loop_count, 0x1d);
LAYOUT_AT(SsScore, rpn_count, rpn_count, 0x1e);
LAYOUT_AT(SsScore, nrpn_count, nrpn_count, 0x1f);
LAYOUT_AT(SsScore, l_count, l_count, 0x20);
LAYOUT_AT(SsScore, l_count_done, l_count_done, 0x21);
LAYOUT_AT(SsScore, next_seq, next_seq, 0x22);
LAYOUT_AT(SsScore, next_sep, next_sep, 0x23);
LAYOUT_AT(SsScore, rhythm_n, rhythm_n, 0x24);
LAYOUT_AT(SsScore, rhythm_d, rhythm_d, 0x25);
LAYOUT_AT(SsScore, vab_id, vab_id, 0x26);
LAYOUT_AT(SsScore, panpot, panpot, 0x27);
LAYOUT_AT(SsScore, programs, programs, 0x37);
LAYOUT_AT(SsScore, pad_47, pad_47, 0x47);
LAYOUT_AT(SsScore, cres_vol, cres_vol, 0x48);
LAYOUT_AT(SsScore, cres_done, cres_done, 0x4a);
LAYOUT_AT(SsScore, unk_4c, unk_4c, 0x4c);
LAYOUT_AT(SsScore, tempo_step, tempo_step, 0x4e);
LAYOUT_AT(SsScore, resolution, resolution, 0x50);
LAYOUT_AT(SsScore, tick_wait, tick_wait, 0x52);
LAYOUT_AT(SsScore, tick_step, tick_step, 0x54);
LAYOUT_AT(SsScore, hdr_tick_step, hdr_tick_step, 0x56);
LAYOUT_AT(SsScore, voll, voll, 0x58);
LAYOUT_AT(SsScore, volr, volr, 0x5a);
LAYOUT_AT(SsScore, cres_voll, cres_voll, 0x5c);
LAYOUT_AT(SsScore, cres_volr, cres_volr, 0x5e);
LAYOUT_AT(SsScore, vol, vol, 0x60);
LAYOUT_AT(SsScore, mute_mask, mute_mask, 0x80);
LAYOUT_AT(SsScore, pad_82, pad_82, 0x82);
LAYOUT_AT(SsScore, start_delta, start_delta, 0x84);
LAYOUT_AT(SsScore, delta_sum, delta_sum, 0x88);
LAYOUT_AT(SsScore, hdr_tempo, hdr_tempo, 0x8c);
LAYOUT_AT(SsScore, delta_value, delta_value, 0x90);
LAYOUT_AT(SsScore, tempo, tempo, 0x94);
LAYOUT_AT(SsScore, flags, flags, 0x98);
LAYOUT_AT(SsScore, cres_time, cres_time, 0x9c);
LAYOUT_AT(SsScore, cres_tick, cres_tick, 0xa0);
LAYOUT_AT(SsScore, unk_a4, unk_a4, 0xa4);
LAYOUT_AT(SsScore, tempo_count, tempo_count, 0xa8);
LAYOUT_AT(SsScore, tempo_goal, tempo_goal, 0xac);

/* The nine values a tone's two ADSR words are split into: _SsUtResolveADSR
   fills it from adsr1 / adsr2, _SsUtBuildADSR packs it back; the ADSR handlers
   _SsSetNrpnVabAttr4 to 8 (libsnd_de_4.c) keep one on their stacks to change
   one value of a tone's envelope. */
typedef struct {
    /* +0x00 the attack rate, adsr1 bits 8-14. */
    u16 ar;
    /* +0x02 the decay rate, adsr1 bits 4-7. */
    u16 dr;
    /* +0x04 the sustain level, adsr1 bits 0-3. */
    u16 sl;
    /* +0x06 the sustain rate, adsr2 bits 6-12. */
    u16 sr;
    /* +0x08 the release rate, adsr2 bits 0-4. */
    u16 rr;
    /* +0x0A the attack mode, adsr1 & 0x8000 (_SsUtBuildADSR reads the four
       modes signed). */
    s16 a_mode;
    /* +0x0C the sustain mode, adsr2 & 0x8000. */
    s16 s_mode;
    /* +0x0E the release mode, adsr2 & 0x20. */
    s16 r_mode;
    /* +0x10 the sustain direction, adsr2 & 0x4000. */
    s16 s_dir;
} SsAdsr;

LAYOUT_SIZE(SsAdsr, 0x12);
LAYOUT_AT(SsAdsr, ar, ar, 0x00);
LAYOUT_AT(SsAdsr, dr, dr, 0x02);
LAYOUT_AT(SsAdsr, sl, sl, 0x04);
LAYOUT_AT(SsAdsr, sr, sr, 0x06);
LAYOUT_AT(SsAdsr, rr, rr, 0x08);
LAYOUT_AT(SsAdsr, a_mode, a_mode, 0x0a);
LAYOUT_AT(SsAdsr, s_mode, s_mode, 0x0c);
LAYOUT_AT(SsAdsr, r_mode, r_mode, 0x0e);
LAYOUT_AT(SsAdsr, s_dir, s_dir, 0x10);

/* libsnd's sequencer tick setting _snd_seq_tick_env (D_800B79F0):
   SsSetTickMode (libsnd_sstick.c) chooses it, _SsStart (libsnd_ssstart_1.c)
   installs the tick by it, the VSync and half-rate handlers
   (libsnd_ssstart_3.c, libsnd_ssstart_4.c) and SsEnd (libsnd_ssend.c) read it.
   The half-rate handler's phase at +0x14 (D_800B7A04) is SSSTART.OBJ's own
   variable and not part of it (libsnd_ssstart_4.c). */
typedef struct {
    /* +0x00 SsSetTickMode's mode (60 in the image): _SsStart divides the root
       counter's rate by it. */
    s32 tick_mode;
    /* +0x04 non-zero when the program calls SsSeqCalledTbyT itself
       (SS_NOTICK): SsSetTickMode writes it; _SsStart and SsEnd then install
       nothing. */
    s32 manual_tick;
    /* +0x08 the tick function (SsSeqCalledTbyT), which the handlers call. */
    void (*tick_cb)(void);
    /* +0x0C the VSync handler _SsStart displaced; _SsTrapIntrVSync calls it
       first. */
    void (*vsync_cb)(void);
    /* +0x10 1 while the tick rides VSyncCallback: _SsStart sets it, SsEnd
       clears it. */
    s8 vsync_tick;
    /* +0x11 1 while the half-rate handler is installed (a mode below 70 Hz):
       _SsStart sets it, SsEnd clears it. */
    s8 half_tick;
    /* +0x12 the interrupt the tick hangs on: 6 root counter 2, 0 VSync, 127
       none (_SsStart writes it, SsEnd tests it and writes 127). */
    s8 alarm_tick;
    /* +0x13 a byte nothing reads or writes. */
    s8 pad_13;
} SndSeqTickEnv;

LAYOUT_SIZE(SndSeqTickEnv, 0x14);
LAYOUT_AT(SndSeqTickEnv, tick_mode, tick_mode, 0x00);
LAYOUT_AT(SndSeqTickEnv, manual_tick, manual_tick, 0x04);
LAYOUT_AT(SndSeqTickEnv, tick_cb, tick_cb, 0x08);
LAYOUT_AT(SndSeqTickEnv, vsync_cb, vsync_cb, 0x0c);
LAYOUT_AT(SndSeqTickEnv, vsync_tick, vsync_tick, 0x10);
LAYOUT_AT(SndSeqTickEnv, half_tick, half_tick, 0x11);
LAYOUT_AT(SndSeqTickEnv, alarm_tick, alarm_tick, 0x12);
LAYOUT_AT(SndSeqTickEnv, pad_13, pad_13, 0x13);

/* libsnd's voice manager's note being set up, _svm_cur (D_800EA6F0):
   _SsVmKeyOn, _SsVmSelectToneAndVag, _SsVmVSetUp (libsnd_vm_key_1.c) and
   _SsVmAlloc, _SsVmDoAllocate, note2pitch (libsnd_vm_n2p.c) fill and read it.
   Sony's library gives the object 0x1A bytes (libsnd's _svm_cur): the copies'
   last six bytes (pad1A) are not part of it. */
typedef struct {
    /* +0x00 the program's tone count (ProgAtr.tones), read with lb by
       _SsVmSelectToneAndVag. */
    s8 tones;
    /* +0x01 the current VAB (_SsVmVSetUp). */
    s8 vab_id;
    /* +0x02 the note number (note2pitch). */
    s8 note;
    /* +0x03 _SsVmKeyOn writes 0; nothing read here says more. */
    s8 unk_03;
    /* +0x04 the note's volume; key-on copies it to the voice's note_vol. */
    s8 voll;
    /* +0x05 the note's pan. */
    s8 note_pan;
    /* +0x06 the program number (_SsVmVSetUp). */
    s8 prog;
    /* +0x07 the program's index among the VAB's programs that have tones
       (ProgAtr.reserved1's low byte): the tone table's row. Also reached as
       D_800EA6F7. */
    s8 fake_prog;
    /* +0x08 two bytes nothing reads or writes. */
    u8 pad_08[2];
    /* +0x0A the program's master volume. */
    s8 mvol;
    /* +0x0B the program's pan. */
    s8 mpan;
    /* +0x0C the tone within the program. */
    s8 tone;
    /* +0x0D the tone's volume. */
    s8 vol;
    /* +0x0E the tone's pan. */
    s8 pan;
    /* +0x0F the tone's priority, which _SsVmAlloc compares (also D_800EA6FF). */
    s8 prior;
    /* +0x10 the tone's centre note (note2pitch). */
    s8 centre;
    /* +0x11 the tone's fine tune (note2pitch holds it to 127; also
       D_800EA701). */
    u8 shift;
    /* +0x12 the tone's mode; bit 2 = reverb on. */
    s8 mode;
    /* +0x13 a byte nothing reads or writes. */
    u8 pad_13;
    /* +0x14 the score playing the note, access number | sequence << 8; 0x21 =
       none, a direct key-on (also D_800EA704). */
    s16 seq_sep;
    /* +0x16 the tone's VAG number, 1-based; 0xFF = noise: _SsVmDoAllocate
       finds the VAG's address by it. */
    s16 vag;
    /* +0x18 the SPU voice being set up (also D_800EA708). */
    s16 voice;
} SvmCur;

LAYOUT_SIZE(SvmCur, 0x1a);
LAYOUT_AT(SvmCur, tones, tones, 0x00);
LAYOUT_AT(SvmCur, vab_id, vab_id, 0x01);
LAYOUT_AT(SvmCur, note, note, 0x02);
LAYOUT_AT(SvmCur, unk_03, unk_03, 0x03);
LAYOUT_AT(SvmCur, voll, voll, 0x04);
LAYOUT_AT(SvmCur, note_pan, note_pan, 0x05);
LAYOUT_AT(SvmCur, prog, prog, 0x06);
LAYOUT_AT(SvmCur, fake_prog, fake_prog, 0x07);
LAYOUT_AT(SvmCur, pad_08, pad_08, 0x08);
LAYOUT_AT(SvmCur, mvol, mvol, 0x0a);
LAYOUT_AT(SvmCur, mpan, mpan, 0x0b);
LAYOUT_AT(SvmCur, tone, tone, 0x0c);
LAYOUT_AT(SvmCur, vol, vol, 0x0d);
LAYOUT_AT(SvmCur, pan, pan, 0x0e);
LAYOUT_AT(SvmCur, prior, prior, 0x0f);
LAYOUT_AT(SvmCur, centre, centre, 0x10);
LAYOUT_AT(SvmCur, shift, shift, 0x11);
LAYOUT_AT(SvmCur, mode, mode, 0x12);
LAYOUT_AT(SvmCur, pad_13, pad_13, 0x13);
LAYOUT_AT(SvmCur, seq_sep, seq_sep, 0x14);
LAYOUT_AT(SvmCur, vag, vag, 0x16);
LAYOUT_AT(SvmCur, voice, voice, 0x18);

/* libsnd's voice manager's state of one SPU voice, _svm_voice[24]
   (D_800E9EC0): _SsVmInit resets it, key-on and _SsVmAlloc / _SsVmDoAllocate
   fill it, _SsVmFlush, key-off and the volume and pitch functions read it
   (libsnd_vm_f.c, libsnd_vm_aloc2.c). */
typedef struct {
    /* +0x00 the tone's VAG number, stored at key-on; 0xFF marks the noise
       voice (_SsVmInit writes 0xFF). */
    s16 vag;
    /* +0x02 +1 on every unmasked voice at each allocation, 0 on the voice
       allocated (_SsVmAlloc); 24 after _SsVmInit. */
    s16 age;
    /* +0x04 the SPU pitch at key-on (_SsVmKeyOnNow); 10 on a noise voice
       (vmNoiseOn). */
    s16 pitch;
    /* +0x06 the SPU envelope level read at the last flush (0 = silent); 0x7FFF
       from allocation until then (_SsVmDoAllocate). */
    u16 envx;
    /* +0x08 the volume argument of _SsVmKeyOn. */
    s16 vol;
    /* +0x0A the note's pan (0x40 after _SsVmInit). */
    u8 pan;
    /* +0x0B a byte nothing reads or writes. */
    u8 pad_0b;
    /* +0x0C the score's MIDI channel at key-on. */
    s16 channel;
    /* +0x0E the note the voice was keyed on with. */
    s16 note;
    /* +0x10 the owning score (-1 after _SsVmInit). */
    s16 seq_sep;
    /* +0x12 _svm_cur.fake_prog at key-on: the tone table's program index. */
    s16 prog;
    /* +0x14 the program number, the index into _svm_pg, which key-off and the
       volume functions compare with their prog. */
    s16 pg;
    /* +0x16 the tone within the program (0xFF after _SsVmInit). */
    s16 tone;
    /* +0x18 the VAB. */
    s16 vab_id;
    /* +0x1A the priority (_SsVmAlloc copies _svm_cur.prior). */
    s16 prior;
    /* +0x1C a byte nothing reads or writes. */
    u8 pad_1c;
    /* +0x1D the key state: 0 idle, 1 keyed on, 2 sounding as the noise voice. */
    s8 key_stat;
    /* +0x1E non-zero: _SsVmFlush calls (*_autovol)(voice); cleared at
       allocation and by _SsVmInit. */
    s16 auto_vol;
    /* +0x20 _SsVmInit writes 0; nothing read here says more. */
    s16 unk_20;
    /* +0x22 _SsVmInit writes 0. */
    s16 unk_22;
    /* +0x24 _SsVmInit writes 0. */
    s16 unk_24;
    /* +0x26 _SsVmInit writes 0. */
    s16 unk_26;
    /* +0x28 a half-word nothing reads or writes. */
    s16 pad_28;
    /* +0x2A non-zero: _SsVmFlush calls (*_autopan)(voice); cleared at
       allocation and by _SsVmInit. */
    s16 auto_pan;
    /* +0x2C _SsVmInit writes 0. */
    s16 unk_2c;
    /* +0x2E _SsVmInit writes 0. */
    s16 unk_2e;
    /* +0x30 _SsVmInit writes 0. */
    s16 unk_30;
    /* +0x32 _SsVmInit writes 0. */
    s16 unk_32;
    /* +0x34 a half-word nothing reads or writes. */
    s16 pad_34;
    /* +0x36 the note's volume at key-on (_svm_cur.voll, _SsVmKeyOn); 0 after
       _SsVmInit. */
    s16 note_vol;
} SvmVoice;

LAYOUT_SIZE(SvmVoice, 0x38);
LAYOUT_AT(SvmVoice, vag, vag, 0x00);
LAYOUT_AT(SvmVoice, age, age, 0x02);
LAYOUT_AT(SvmVoice, pitch, pitch, 0x04);
LAYOUT_AT(SvmVoice, envx, envx, 0x06);
LAYOUT_AT(SvmVoice, vol, vol, 0x08);
LAYOUT_AT(SvmVoice, pan, pan, 0x0a);
LAYOUT_AT(SvmVoice, pad_0b, pad_0b, 0x0b);
LAYOUT_AT(SvmVoice, channel, channel, 0x0c);
LAYOUT_AT(SvmVoice, note, note, 0x0e);
LAYOUT_AT(SvmVoice, seq_sep, seq_sep, 0x10);
LAYOUT_AT(SvmVoice, prog, prog, 0x12);
LAYOUT_AT(SvmVoice, pg, pg, 0x14);
LAYOUT_AT(SvmVoice, tone, tone, 0x16);
LAYOUT_AT(SvmVoice, vab_id, vab_id, 0x18);
LAYOUT_AT(SvmVoice, prior, prior, 0x1a);
LAYOUT_AT(SvmVoice, pad_1c, pad_1c, 0x1c);
LAYOUT_AT(SvmVoice, key_stat, key_stat, 0x1d);
LAYOUT_AT(SvmVoice, auto_vol, auto_vol, 0x1e);
LAYOUT_AT(SvmVoice, unk_20, unk_20, 0x20);
LAYOUT_AT(SvmVoice, unk_22, unk_22, 0x22);
LAYOUT_AT(SvmVoice, unk_24, unk_24, 0x24);
LAYOUT_AT(SvmVoice, unk_26, unk_26, 0x26);
LAYOUT_AT(SvmVoice, pad_28, pad_28, 0x28);
LAYOUT_AT(SvmVoice, auto_pan, auto_pan, 0x2a);
LAYOUT_AT(SvmVoice, unk_2c, unk_2c, 0x2c);
LAYOUT_AT(SvmVoice, unk_2e, unk_2e, 0x2e);
LAYOUT_AT(SvmVoice, unk_30, unk_30, 0x30);
LAYOUT_AT(SvmVoice, unk_32, unk_32, 0x32);
LAYOUT_AT(SvmVoice, pad_34, pad_34, 0x34);
LAYOUT_AT(SvmVoice, note_vol, note_vol, 0x36);

/* One record of libspu's allocation table, whose address _spu_memList
   (D_800B7C18) holds: SpuInitMalloc (libspu_s_m_init.c) sets it up, SpuMalloc
   (libspu_s_m_m.c), SpuFree (libspu_s_m_f.c) and _SpuIsInAllocateArea
   (libspu_s_m_util.c) walk it; _SpuInit (libspu_s_ini_1.c) clears the pointer. */
typedef struct {
    /* +0x00 the block's SPU address in bits 0-27; bit 31 marks a free record,
       bit 30 the table's end. */
    u32 addr;
    /* +0x04 the block's size in bytes. */
    u32 size;
} SPU_MALLOC;

LAYOUT_SIZE(SPU_MALLOC, 0x08);
LAYOUT_AT(SPU_MALLOC, addr, addr, 0x00);
LAYOUT_AT(SPU_MALLOC, size, size, 0x04);

/* One SPU voice's eight registers, 24 of them from 0x1F801C00: SpuRXX.voice.
   libspu writes them by name (_spu_init, libspu_spu_1.c) or as raw words
   (SpuSetVoiceAttr, libspu_s_sva.c). */
typedef struct {
    /* +0x00 the left volume. */
    unsigned short volL;
    /* +0x02 the right volume. */
    unsigned short volR;
    /* +0x04 the pitch (0x3FFF at _spu_init). */
    unsigned short pitch;
    /* +0x06 the start address in SPU units (0x200 at _spu_init). */
    unsigned short addr;
    /* +0x08 the first envelope word. */
    unsigned short adsr1;
    /* +0x0A the second envelope word. */
    unsigned short adsr2;
    /* +0x0C the current envelope level (ENVX). */
    unsigned short volumex;
    /* +0x0E the loop address. */
    unsigned short loop_addr;
} SpuVoiceReg;

LAYOUT_SIZE(SpuVoiceReg, 0x10);
LAYOUT_AT(SpuVoiceReg, volL, volL, 0x00);
LAYOUT_AT(SpuVoiceReg, volR, volR, 0x02);
LAYOUT_AT(SpuVoiceReg, pitch, pitch, 0x04);
LAYOUT_AT(SpuVoiceReg, addr, addr, 0x06);
LAYOUT_AT(SpuVoiceReg, adsr1, adsr1, 0x08);
LAYOUT_AT(SpuVoiceReg, adsr2, adsr2, 0x0a);
LAYOUT_AT(SpuVoiceReg, volumex, volumex, 0x0c);
LAYOUT_AT(SpuVoiceReg, loop_addr, loop_addr, 0x0e);

/* The SPU's registers, 0x200 bytes from 0x1F801C00, by name: SpuUnion.rxx,
   reached through libspu's _spu_RXX (D_800B7BB0), and libcd BIOS.OBJ's own
   pointer to them D_800B6630 (CD_initvol, libcd_bios_1.c). Six of the first
   split's copies stopped at 0x1B8 (they named no register past ex_vol); one
   named the whole block. */
typedef struct {
    /* +0x000 the 24 voices' registers. */
    SpuVoiceReg voice[24];
    /* +0x180 the main volume, left and right: _spu_init writes 0,
       SpuSetCommonMasterVolume the caller's, CD_initvol 0x3FFF. */
    SpuVolume main_vol;
    /* +0x184 the reverb depth: _spu_init and SpuSetReverbModeType write 0,
       SpuSetReverbModeParam and SpuSetReverbModeDepth the caller's. */
    SpuVolume rev_vol;
    /* +0x188 key on, voices 0-15 and 16-23. */
    u16 key_on[2];
    /* +0x18C key off. */
    u16 key_off[2];
    /* +0x190 pitch modulation on. */
    u16 chan_fm[2];
    /* +0x194 noise on. */
    u16 noise_mode[2];
    /* +0x198 reverb on. */
    u16 rev_mode[2];
    /* +0x19C the voices' end flags (ENDX). */
    u16 chan_on[2];
    /* +0x1A0 a register no access names. */
    u16 pad_1a0;
    /* +0x1A2 the reverb work area's start (register word 0xD1, written through
       _spu_FsetRXX). */
    u16 rev_work_addr;
    /* +0x1A4 the IRQ address. */
    u16 irq_addr;
    /* +0x1A6 the transfer address. */
    u16 trans_addr;
    /* +0x1A8 the transfer FIFO: the I/O transfer (func_8009E0F8,
       libspu_spu_1.c) writes its words here. */
    u16 trans_fifo;
    /* +0x1AA SPUCNT: bit 7 reverb master enable, bits 8-13 the noise clock;
       CD_initvol writes 0xC001. */
    u16 spucnt;
    /* +0x1AC the transfer control (4 at _spu_init). */
    u16 data_trans;
    /* +0x1AE SPUSTAT: _spu_init and the I/O transfer wait on its bits. */
    u16 spustat;
    /* +0x1B0 the CD input volume: _spu_init writes 0, SpuSetCommonCDVolume the
       caller's, CD_initvol 0x3FFF. */
    SpuVolume cd_vol;
    /* +0x1B4 the external input volume: _spu_init writes 0. */
    SpuVolume ex_vol;
    /* +0x1B8 the current main volume: CD_initvol reads both halves unsigned
       (lhu), so its two reads take AS(u16, ...). */
    SpuVolume main_volx;
    /* +0x1BC a register pair no access names. */
    SpuVolume pad_1bc;
    /* +0x1C0 reverb register 0 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 dAPF1;
    /* +0x1C2 reverb register 1 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 dAPF2;
    /* +0x1C4 reverb register 2 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vIIR;
    /* +0x1C6 reverb register 3 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vCOMB1;
    /* +0x1C8 reverb register 4 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vCOMB2;
    /* +0x1CA reverb register 5 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vCOMB3;
    /* +0x1CC reverb register 6 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vCOMB4;
    /* +0x1CE reverb register 7 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vWALL;
    /* +0x1D0 reverb register 8 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vAPF1;
    /* +0x1D2 reverb register 9 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vAPF2;
    /* +0x1D4 reverb register 10 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLSAME;
    /* +0x1D6 reverb register 11 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRSAME;
    /* +0x1D8 reverb register 12 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLCOMB1;
    /* +0x1DA reverb register 13 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRCOMB1;
    /* +0x1DC reverb register 14 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLCOMB2;
    /* +0x1DE reverb register 15 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRCOMB2;
    /* +0x1E0 reverb register 16 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 dLSAME;
    /* +0x1E2 reverb register 17 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 dRSAME;
    /* +0x1E4 reverb register 18 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLDIFF;
    /* +0x1E6 reverb register 19 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRDIFF;
    /* +0x1E8 reverb register 20 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLCOMB3;
    /* +0x1EA reverb register 21 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRCOMB3;
    /* +0x1EC reverb register 22 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLCOMB4;
    /* +0x1EE reverb register 23 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRCOMB4;
    /* +0x1F0 reverb register 24 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 dLDIFF;
    /* +0x1F2 reverb register 25 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 dRDIFF;
    /* +0x1F4 reverb register 26 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLAPF1;
    /* +0x1F6 reverb register 27 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRAPF1;
    /* +0x1F8 reverb register 28 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mLAPF2;
    /* +0x1FA reverb register 29 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 mRAPF2;
    /* +0x1FC reverb register 30 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vLIN;
    /* +0x1FE reverb register 31 of 32: _spu_setReverbAttr copies the
       SpuRevParam field of the same name into it. */
    u16 vRIN;
} SpuRXX;

LAYOUT_SIZE(SpuRXX, 0x200);
LAYOUT_AT(SpuRXX, voice, voice, 0x00);
LAYOUT_AT(SpuRXX, main_vol, main_vol, 0x180);
LAYOUT_AT(SpuRXX, rev_vol, rev_vol, 0x184);
LAYOUT_AT(SpuRXX, key_on, key_on, 0x188);
LAYOUT_AT(SpuRXX, key_off, key_off, 0x18c);
LAYOUT_AT(SpuRXX, chan_fm, chan_fm, 0x190);
LAYOUT_AT(SpuRXX, noise_mode, noise_mode, 0x194);
LAYOUT_AT(SpuRXX, rev_mode, rev_mode, 0x198);
LAYOUT_AT(SpuRXX, chan_on, chan_on, 0x19c);
LAYOUT_AT(SpuRXX, pad_1a0, pad_1a0, 0x1a0);
LAYOUT_AT(SpuRXX, rev_work_addr, rev_work_addr, 0x1a2);
LAYOUT_AT(SpuRXX, irq_addr, irq_addr, 0x1a4);
LAYOUT_AT(SpuRXX, trans_addr, trans_addr, 0x1a6);
LAYOUT_AT(SpuRXX, trans_fifo, trans_fifo, 0x1a8);
LAYOUT_AT(SpuRXX, spucnt, spucnt, 0x1aa);
LAYOUT_AT(SpuRXX, data_trans, data_trans, 0x1ac);
LAYOUT_AT(SpuRXX, spustat, spustat, 0x1ae);
LAYOUT_AT(SpuRXX, cd_vol, cd_vol, 0x1b0);
LAYOUT_AT(SpuRXX, ex_vol, ex_vol, 0x1b4);
LAYOUT_AT(SpuRXX, main_volx, main_volx, 0x1b8);
LAYOUT_AT(SpuRXX, pad_1bc, pad_1bc, 0x1bc);
LAYOUT_AT(SpuRXX, dAPF1, dAPF1, 0x1c0);
LAYOUT_AT(SpuRXX, dAPF2, dAPF2, 0x1c2);
LAYOUT_AT(SpuRXX, vIIR, vIIR, 0x1c4);
LAYOUT_AT(SpuRXX, vCOMB1, vCOMB1, 0x1c6);
LAYOUT_AT(SpuRXX, vCOMB2, vCOMB2, 0x1c8);
LAYOUT_AT(SpuRXX, vCOMB3, vCOMB3, 0x1ca);
LAYOUT_AT(SpuRXX, vCOMB4, vCOMB4, 0x1cc);
LAYOUT_AT(SpuRXX, vWALL, vWALL, 0x1ce);
LAYOUT_AT(SpuRXX, vAPF1, vAPF1, 0x1d0);
LAYOUT_AT(SpuRXX, vAPF2, vAPF2, 0x1d2);
LAYOUT_AT(SpuRXX, mLSAME, mLSAME, 0x1d4);
LAYOUT_AT(SpuRXX, mRSAME, mRSAME, 0x1d6);
LAYOUT_AT(SpuRXX, mLCOMB1, mLCOMB1, 0x1d8);
LAYOUT_AT(SpuRXX, mRCOMB1, mRCOMB1, 0x1da);
LAYOUT_AT(SpuRXX, mLCOMB2, mLCOMB2, 0x1dc);
LAYOUT_AT(SpuRXX, mRCOMB2, mRCOMB2, 0x1de);
LAYOUT_AT(SpuRXX, dLSAME, dLSAME, 0x1e0);
LAYOUT_AT(SpuRXX, dRSAME, dRSAME, 0x1e2);
LAYOUT_AT(SpuRXX, mLDIFF, mLDIFF, 0x1e4);
LAYOUT_AT(SpuRXX, mRDIFF, mRDIFF, 0x1e6);
LAYOUT_AT(SpuRXX, mLCOMB3, mLCOMB3, 0x1e8);
LAYOUT_AT(SpuRXX, mRCOMB3, mRCOMB3, 0x1ea);
LAYOUT_AT(SpuRXX, mLCOMB4, mLCOMB4, 0x1ec);
LAYOUT_AT(SpuRXX, mRCOMB4, mRCOMB4, 0x1ee);
LAYOUT_AT(SpuRXX, dLDIFF, dLDIFF, 0x1f0);
LAYOUT_AT(SpuRXX, dRDIFF, dRDIFF, 0x1f2);
LAYOUT_AT(SpuRXX, mLAPF1, mLAPF1, 0x1f4);
LAYOUT_AT(SpuRXX, mRAPF1, mRAPF1, 0x1f6);
LAYOUT_AT(SpuRXX, mLAPF2, mLAPF2, 0x1f8);
LAYOUT_AT(SpuRXX, mRAPF2, mRAPF2, 0x1fa);
LAYOUT_AT(SpuRXX, vLIN, vLIN, 0x1fc);
LAYOUT_AT(SpuRXX, vRIN, vRIN, 0x1fe);

/* libspu's register block as its functions reach it through _spu_RXX
   (D_800B7BB0): the registers by name, or as 0x100 16-bit words for the
   functions handed a register number (_SpuGetAnyVoice, _spu_FsetRXX,
   SpuSetVoiceAttr). */
typedef union {
    /* +0x000 the registers by name. */
    volatile SpuRXX rxx;
    /* +0x000 the same 0x200 bytes as 0x100 words, indexed by register number. */
    volatile u16 raw[0x100];
} SpuUnion;

LAYOUT_SIZE(SpuUnion, 0x200);
LAYOUT_AT(SpuUnion, rxx, rxx, 0x00);
LAYOUT_AT(SpuUnion, raw, raw, 0x00);

/* One reverb preset: libspu's table _spu_rev_param (D_800B8170) holds one for
   each of the ten reverb modes. SpuSetReverbModeParam (libspu_s_srmp.c) and
   SpuSetReverbModeType (libspu_s_srmt.c) copy a preset to the stack, adjust it
   and hand it to _spu_setReverbAttr (libspu_s_sra.c), which writes the
   registers flags selects. */
typedef struct {
    /* +0x00 bit i selects reverb register i to write; 0 writes all 32.
       SpuSetReverbModeParam sets it for the registers it changed (0x0C011C00
       for the delay, 0x80 for the feedback). */
    u32 flags;
    /* +0x04 reverb register 0 (SpuRXX +0x1C0). */
    u16 dAPF1;
    /* +0x06 reverb register 1 (SpuRXX +0x1C2). */
    u16 dAPF2;
    /* +0x08 reverb register 2 (SpuRXX +0x1C4). */
    u16 vIIR;
    /* +0x0A reverb register 3 (SpuRXX +0x1C6). */
    u16 vCOMB1;
    /* +0x0C reverb register 4 (SpuRXX +0x1C8). */
    u16 vCOMB2;
    /* +0x0E reverb register 5 (SpuRXX +0x1CA). */
    u16 vCOMB3;
    /* +0x10 reverb register 6 (SpuRXX +0x1CC). */
    u16 vCOMB4;
    /* +0x12 reverb register 7 (SpuRXX +0x1CE). */
    u16 vWALL;
    /* +0x14 reverb register 8 (SpuRXX +0x1D0). */
    u16 vAPF1;
    /* +0x16 reverb register 9 (SpuRXX +0x1D2). */
    u16 vAPF2;
    /* +0x18 reverb register 10 (SpuRXX +0x1D4). */
    u16 mLSAME;
    /* +0x1A reverb register 11 (SpuRXX +0x1D6). */
    u16 mRSAME;
    /* +0x1C reverb register 12 (SpuRXX +0x1D8). */
    u16 mLCOMB1;
    /* +0x1E reverb register 13 (SpuRXX +0x1DA). */
    u16 mRCOMB1;
    /* +0x20 reverb register 14 (SpuRXX +0x1DC). */
    u16 mLCOMB2;
    /* +0x22 reverb register 15 (SpuRXX +0x1DE). */
    u16 mRCOMB2;
    /* +0x24 reverb register 16 (SpuRXX +0x1E0). */
    u16 dLSAME;
    /* +0x26 reverb register 17 (SpuRXX +0x1E2). */
    u16 dRSAME;
    /* +0x28 reverb register 18 (SpuRXX +0x1E4). */
    u16 mLDIFF;
    /* +0x2A reverb register 19 (SpuRXX +0x1E6). */
    u16 mRDIFF;
    /* +0x2C reverb register 20 (SpuRXX +0x1E8). */
    u16 mLCOMB3;
    /* +0x2E reverb register 21 (SpuRXX +0x1EA). */
    u16 mRCOMB3;
    /* +0x30 reverb register 22 (SpuRXX +0x1EC). */
    u16 mLCOMB4;
    /* +0x32 reverb register 23 (SpuRXX +0x1EE). */
    u16 mRCOMB4;
    /* +0x34 reverb register 24 (SpuRXX +0x1F0). */
    u16 dLDIFF;
    /* +0x36 reverb register 25 (SpuRXX +0x1F2). */
    u16 dRDIFF;
    /* +0x38 reverb register 26 (SpuRXX +0x1F4). */
    u16 mLAPF1;
    /* +0x3A reverb register 27 (SpuRXX +0x1F6). */
    u16 mRAPF1;
    /* +0x3C reverb register 28 (SpuRXX +0x1F8). */
    u16 mLAPF2;
    /* +0x3E reverb register 29 (SpuRXX +0x1FA). */
    u16 mRAPF2;
    /* +0x40 reverb register 30 (SpuRXX +0x1FC). */
    u16 vLIN;
    /* +0x42 reverb register 31 (SpuRXX +0x1FE). */
    u16 vRIN;
} SpuRevParam;

LAYOUT_SIZE(SpuRevParam, 0x44);
LAYOUT_AT(SpuRevParam, flags, flags, 0x00);
LAYOUT_AT(SpuRevParam, dAPF1, dAPF1, 0x04);
LAYOUT_AT(SpuRevParam, dAPF2, dAPF2, 0x06);
LAYOUT_AT(SpuRevParam, vIIR, vIIR, 0x08);
LAYOUT_AT(SpuRevParam, vCOMB1, vCOMB1, 0x0a);
LAYOUT_AT(SpuRevParam, vCOMB2, vCOMB2, 0x0c);
LAYOUT_AT(SpuRevParam, vCOMB3, vCOMB3, 0x0e);
LAYOUT_AT(SpuRevParam, vCOMB4, vCOMB4, 0x10);
LAYOUT_AT(SpuRevParam, vWALL, vWALL, 0x12);
LAYOUT_AT(SpuRevParam, vAPF1, vAPF1, 0x14);
LAYOUT_AT(SpuRevParam, vAPF2, vAPF2, 0x16);
LAYOUT_AT(SpuRevParam, mLSAME, mLSAME, 0x18);
LAYOUT_AT(SpuRevParam, mRSAME, mRSAME, 0x1a);
LAYOUT_AT(SpuRevParam, mLCOMB1, mLCOMB1, 0x1c);
LAYOUT_AT(SpuRevParam, mRCOMB1, mRCOMB1, 0x1e);
LAYOUT_AT(SpuRevParam, mLCOMB2, mLCOMB2, 0x20);
LAYOUT_AT(SpuRevParam, mRCOMB2, mRCOMB2, 0x22);
LAYOUT_AT(SpuRevParam, dLSAME, dLSAME, 0x24);
LAYOUT_AT(SpuRevParam, dRSAME, dRSAME, 0x26);
LAYOUT_AT(SpuRevParam, mLDIFF, mLDIFF, 0x28);
LAYOUT_AT(SpuRevParam, mRDIFF, mRDIFF, 0x2a);
LAYOUT_AT(SpuRevParam, mLCOMB3, mLCOMB3, 0x2c);
LAYOUT_AT(SpuRevParam, mRCOMB3, mRCOMB3, 0x2e);
LAYOUT_AT(SpuRevParam, mLCOMB4, mLCOMB4, 0x30);
LAYOUT_AT(SpuRevParam, mRCOMB4, mRCOMB4, 0x32);
LAYOUT_AT(SpuRevParam, dLDIFF, dLDIFF, 0x34);
LAYOUT_AT(SpuRevParam, dRDIFF, dRDIFF, 0x36);
LAYOUT_AT(SpuRevParam, mLAPF1, mLAPF1, 0x38);
LAYOUT_AT(SpuRevParam, mRAPF1, mRAPF1, 0x3a);
LAYOUT_AT(SpuRevParam, mLAPF2, mLAPF2, 0x3c);
LAYOUT_AT(SpuRevParam, mRAPF2, mRAPF2, 0x3e);
LAYOUT_AT(SpuRevParam, vLIN, vLIN, 0x40);
LAYOUT_AT(SpuRevParam, vRIN, vRIN, 0x42);

/* libmcrd LIBMCRD.OBJ's state (D_800EA960): the command running and its
   result, the card it works on, the open file and the transfer, the
   MemCardCallback function and two words a port. MemCardStart
   (libmcrd_libmcrd_1.c) sets it up, the Mem* commands (libmcrd_libmcrd_2.c ...
   az.c) fill it, the VSync callback _callback_handler (libmcrd_libmcrd_12.c)
   completes the command, the state callbacks (libmcrd_libmcrd_3.c ...) read
   it. */
typedef struct {
    /* +0x00 the command running (McFunc*), 0 = idle: each command writes its
       number and refuses while it is above 0; _callback_handler moves it to
       D_800EA950 and clears it. */
    volatile s32 cmd;
    /* +0x04 the command's result (McErr*): the state callbacks write it;
       _callback_handler moves it to D_800EA950[1]. */
    volatile s32 rslt;
    /* +0x08 1 when the command has completed (_callback_handler); MemCardStart
       and each command write 0. */
    volatile s32 done;
    /* +0x0C a bit per card, 1 << chan: MemCardStart writes 0, MemCardAccept's
       state callback (func_800A1D94, libmcrd_libmcrd_5.c) sets the card's bit
       on a new card; while it is clear, _card_exist_back (libmcrd_libmcrd_3.c)
       reports an IOE reply as a new card. */
    volatile s32 mask;
    /* +0x10 the card the command works on, port << 4 | card: each command
       writes its argument. */
    s32 chan;
    /* +0x14 the file MemCardOpen opened, -1 = none (MemCardStart and
       MemCardClose write -1). */
    s32 fd;
    /* +0x18 the transfer's byte offset in the file (MemCardReadData,
       MemCardWriteData). */
    s32 ofs;
    /* +0x1C the transfer's length. */
    s32 bytes;
    /* +0x20 the transfer's buffer. */
    u32 *adrs;
    /* +0x24 the file's name with its device, "buXX:" and the name
       (func_800A35BC, then strcat). */
    char file[0x20];
    /* +0x44 the MemCardCallback function, called by _callback_handler with the
       command and its result. */
    void (*func)(s32, s32);
    /* +0x48 a flag per port: MemCardStart writes 1 to both; _card_exist_back
       writes 1 after a new-card or no-card result and 0 after any other, and
       keeps the old value in D_800B8454. */
    s32 new_card[2];
    /* +0x50 a counter per port: _callback_handler counts both every VSync;
       _card_exist_back takes the port's (into D_800B8450) and zeroes it when a
       query starts; MemCardStart writes 0. */
    volatile s32 frames[2];
} McrdGlobal;

LAYOUT_SIZE(McrdGlobal, 0x58);
LAYOUT_AT(McrdGlobal, cmd, cmd, 0x00);
LAYOUT_AT(McrdGlobal, rslt, rslt, 0x04);
LAYOUT_AT(McrdGlobal, done, done, 0x08);
LAYOUT_AT(McrdGlobal, mask, mask, 0x0c);
LAYOUT_AT(McrdGlobal, chan, chan, 0x10);
LAYOUT_AT(McrdGlobal, fd, fd, 0x14);
LAYOUT_AT(McrdGlobal, ofs, ofs, 0x18);
LAYOUT_AT(McrdGlobal, bytes, bytes, 0x1c);
LAYOUT_AT(McrdGlobal, adrs, adrs, 0x20);
LAYOUT_AT(McrdGlobal, file, file, 0x24);
LAYOUT_AT(McrdGlobal, func, func, 0x44);
LAYOUT_AT(McrdGlobal, new_card, new_card, 0x48);
LAYOUT_AT(McrdGlobal, frames, frames, 0x50);

/* One directory frame of a memory card as libcard keeps the 15 of them
   (D_800EAA80; 32 of each frame's 128 bytes): _card_format2 (libmcrd_low.c)
   writes them free, _card_create2 searches them by name and chains a new
   file's blocks. */
typedef struct {
    /* +0x00 the block's state: 0x51 / 0x52 / 0x53 the first / middle / last
       block of a file, 0xA0..0xAF free (formatting writes 0xA0). */
    s32 state;
    /* +0x04 the file's size in bytes, on its first block (blocks << 13). */
    s32 size;
    /* +0x08 the file's following block, 0xFFFF = none. */
    u16 next;
    /* +0x0A the file's name (strcmp, strncpy). */
    char name[22];
} CardDir;

LAYOUT_SIZE(CardDir, 0x20);
LAYOUT_AT(CardDir, state, state, 0x00);
LAYOUT_AT(CardDir, size, size, 0x04);
LAYOUT_AT(CardDir, next, next, 0x08);
LAYOUT_AT(CardDir, name, name, 0x0a);

/* A record of the kernel's device table (its address at 0x150, its size in
   bytes at 0x154): libapi FIRST.OBJ's firstfile finds the memory card's
   device by name and swaps its firstfile function (libapi_first.c). Sony's
   public type, from fs.h, which psyq_types.h does not reach and which is
   included here. Its fields as this program uses them:
   - dt_string (+0x00): the device's name, compared with strcmp ("bu").
   - dt_type (+0x04): not touched by this program's code.
   - dt_bsize (+0x08): not touched by this program's code.
   - dt_desc (+0x0C): not touched by this program's code.
   - dt_init (+0x10): a function of the device; not touched by this
     program's code.
   - dt_open (+0x14): a function of the device; not touched by this
     program's code.
   - dt_strategy (+0x18): a function of the device; not touched by this
     program's code.
   - dt_close (+0x1C): a function of the device; not touched by this
     program's code.
   - dt_ioctl (+0x20): a function of the device; not touched by this
     program's code.
   - dt_read (+0x24): a function of the device; not touched by this
     program's code.
   - dt_write (+0x28): a function of the device; not touched by this
     program's code.
   - dt_delete (+0x2C): a function of the device; not touched by this
     program's code.
   - dt_undelete (+0x30): a function of the device; not touched by this
     program's code.
   - dt_firstfile (+0x34): a function of the device: firstfile saves it in
     D_800EA9C0 and puts func_800A37D4 in its place; func_800A37D4 puts it
     back and calls it.
   - dt_nextfile (+0x38): a function of the device; not touched by this
     program's code.
   - dt_format (+0x3C): a function of the device; not touched by this
     program's code.
   - dt_cd (+0x40): a function of the device; not touched by this program's
     code.
   - dt_rename (+0x44): a function of the device; not touched by this
     program's code.
   - dt_remove (+0x48): a function of the device; not touched by this
     program's code.
   - dt_else (+0x4C): a function of the device; not touched by this
     program's code. */
#include <fs.h>

typedef struct device_table device_table;

LAYOUT_SIZE(device_table, 0x50);
LAYOUT_AT(device_table, dt_string, dt_string, 0x00);
LAYOUT_AT(device_table, dt_type, dt_type, 0x04);
LAYOUT_AT(device_table, dt_bsize, dt_bsize, 0x08);
LAYOUT_AT(device_table, dt_desc, dt_desc, 0x0c);
LAYOUT_AT(device_table, dt_init, dt_init, 0x10);
LAYOUT_AT(device_table, dt_open, dt_open, 0x14);
LAYOUT_AT(device_table, dt_strategy, dt_strategy, 0x18);
LAYOUT_AT(device_table, dt_close, dt_close, 0x1c);
LAYOUT_AT(device_table, dt_ioctl, dt_ioctl, 0x20);
LAYOUT_AT(device_table, dt_read, dt_read, 0x24);
LAYOUT_AT(device_table, dt_write, dt_write, 0x28);
LAYOUT_AT(device_table, dt_delete, dt_delete, 0x2c);
LAYOUT_AT(device_table, dt_undelete, dt_undelete, 0x30);
LAYOUT_AT(device_table, dt_firstfile, dt_firstfile, 0x34);
LAYOUT_AT(device_table, dt_nextfile, dt_nextfile, 0x38);
LAYOUT_AT(device_table, dt_format, dt_format, 0x3c);
LAYOUT_AT(device_table, dt_cd, dt_cd, 0x40);
LAYOUT_AT(device_table, dt_rename, dt_rename, 0x44);
LAYOUT_AT(device_table, dt_remove, dt_remove, 0x48);
LAYOUT_AT(device_table, dt_else, dt_else, 0x4c);

/* The kernel's I/O block of an open file, as the BIOS firstfile hands it to
   a device's firstfile function: func_800A37D4 (libapi_first.c) sets
   i_flgs. Sony's public type, from fs.h (above). Its fields as this program
   uses them:
   - i_flgs (+0x00): the block's flags: func_800A37D4 writes 1 when it finds
     0.
   - i_unit (+0x04): not touched by this program's code.
   - i_ma (+0x08): not touched by this program's code.
   - i_cc (+0x0C): not touched by this program's code.
   - i_offset (+0x10): not touched by this program's code.
   - i_fstype (+0x14): not touched by this program's code.
   - i_errno (+0x18): not touched by this program's code.
   - i_dp (+0x1C): not touched by this program's code.
   - i_size (+0x20): not touched by this program's code.
   - i_head (+0x24): not touched by this program's code.
   - i_fd (+0x28): not touched by this program's code. */
typedef struct iob iob;

LAYOUT_SIZE(iob, 0x2c);
LAYOUT_AT(iob, i_flgs, i_flgs, 0x00);
LAYOUT_AT(iob, i_unit, i_unit, 0x04);
LAYOUT_AT(iob, i_ma, i_ma, 0x08);
LAYOUT_AT(iob, i_cc, i_cc, 0x0c);
LAYOUT_AT(iob, i_offset, i_offset, 0x10);
LAYOUT_AT(iob, i_fstype, i_fstype, 0x14);
LAYOUT_AT(iob, i_errno, i_errno, 0x18);
LAYOUT_AT(iob, i_dp, i_dp, 0x1c);
LAYOUT_AT(iob, i_size, i_size, 0x20);
LAYOUT_AT(iob, i_head, i_head, 0x24);
LAYOUT_AT(iob, i_fd, i_fd, 0x28);

/* One actuator record of a controller's actuator table (PadInfo.act_table),
   built by _padLoadActInfo from the controller's replies (libpad_padcmd.c) and
   read by PadInfoAct (libpad_padentry_2.c); sizeof is the 5 the table's
   arithmetic uses. */
typedef struct {
    /* +0x00 the actuator's function (PadInfoAct term 1). */
    u8 func;
    /* +0x01 its sub-function (term 2). */
    u8 sub;
    /* +0x02 its data size (term 3). */
    u8 size;
    /* +0x03 its current drain (term 4). */
    u8 curr;
    /* +0x04 its sign byte (term 5). */
    u8 sign;
} PadActInfo;

LAYOUT_SIZE(PadActInfo, 0x05);
LAYOUT_AT(PadActInfo, func, func, 0x00);
LAYOUT_AT(PadActInfo, sub, sub, 0x01);
LAYOUT_AT(PadActInfo, size, size, 0x02);
LAYOUT_AT(PadActInfo, curr, curr, 0x03);
LAYOUT_AT(PadActInfo, sign, sign, 0x04);

/* One combination record of a controller's combination table
   (PadInfo.comb_table): its actuator count and list, built by _padLoadActInfo
   (libpad_padcmd.c) and read by PadInfoComb (libpad_padentry_2.c). */
typedef struct {
    /* +0x00 the actuators in the combination. */
    u8 num;
    /* +0x01 three bytes nothing reads or writes. */
    u8 pad_01[3];
    /* +0x04 their numbers. */
    u8 *list;
} PadCombInfo;

LAYOUT_SIZE(PadCombInfo, 0x08);
LAYOUT_AT(PadCombInfo, num, num, 0x00);
LAYOUT_AT(PadCombInfo, pad_01, pad_01, 0x01);
LAYOUT_AT(PadCombInfo, list, list, 0x04);

/* libpad's record of one controller port, 0xF0 bytes each; _padInfoDir
   (D_800B84BC) points at the direct ports' two. It holds the controller's ID,
   actuator and combination tables, the caller's reply buffer, the command
   sequence's hooks and state, the bytes sent and received and the buffer the
   tables are built in. PADMAIN.OBJ (libpad_padmain.c), PADCMD.OBJ
   (libpad_padcmd.c), the direct-port sequencer (libpad_padportd.c,
   libpad_padseqd.c), the multitap port (libpad_padif.c) and the PadInfo* calls
   (psyq_misc_aak/aal/aam.c). */
typedef struct PadInfo {
    /* +0x00 the controller's extended IDs (into info_buf): PadInfoMode term 4
       reads it. */
    u16 *id_table;
    /* +0x04 its actuator records (into info_buf, _padLoadActInfo). */
    PadActInfo *act_table;
    /* +0x08 its combination records (into info_buf, _padLoadActInfo);
       func_800A7318 (libpad_padportd.c) writes 0. Six copies declare it u8 *. */
    PadCombInfo *comb_table;
    /* +0x0C a multitap port's four sub-port records: the multitap code hands
       &tap_info[i] to the hooks; the direct port writes 0. */
    struct PadInfo *tap_info;
    /* +0x10 the port's own (first) record: PadGetState compares p with it. */
    struct PadInfo *head;
    /* +0x14 the send hook of the command sequence's current step. */
    void (*send_func)(struct PadInfo *);
    /* +0x18 the receive hook: it judges the reply and returns what is added to
       seq (1 = next step, 0 = the same again). */
    s32 (*recv_func)(struct PadInfo *);
    /* +0x1C four bytes nothing reads or writes. */
    u8 pad_1c[4];
    /* +0x20 the caller's actuator alignment: _padSetActAlign keeps it,
       func_800A68C8 works act_align out from it. */
    u8 *align_data;
    /* +0x24 a one-byte parameter buffer: the one-byte commands write their
       argument and point cmd_data at it. */
    u8 cmd_param;
    /* +0x25 three bytes nothing reads or writes. */
    u8 pad_25[3];
    /* +0x28 the caller's actuator data (_padSetAct). */
    u8 *act_data;
    /* +0x2C the queued command's parameter bytes. */
    u8 *cmd_data;
    /* +0x30 the caller's buffer, which gets the reply (0xFF, 0 and the ID when
       no controller answers). */
    u8 *rx_buf;
    /* +0x34 act_data's byte count (_padSetAct). */
    u8 act_len;
    /* +0x35 the bytes copied into rx_buf; zeroed when no controller answers. */
    u8 rx_len;
    /* +0x36 cmd_data's byte count. */
    u8 cmd_len;
    /* +0x37 the command a send hook queued, 0 = none. */
    u8 cmd_queued;
    /* +0x38 the command being sent. */
    u8 cmd_taken;
    /* +0x39 a byte PadGetState reads on a record other than its head (non-zero
       = busy); nothing read here writes it. */
    u8 unk_39;
    /* +0x3A two bytes nothing reads or writes. */
    u8 pad_3a[2];
    /* +0x3C the bytes received in the last transfer (the reply): _padSioRW
       stores them, the receive hooks read them. */
    u8 *recv_buf;
    /* +0x40 the bytes to send: _padSioRW writes the first. */
    u8 *send_buf;
    /* +0x44 the bytes received so far. */
    u8 recv_len;
    /* +0x45 the bytes sent so far. */
    u8 send_len;
    /* +0x46 the command sequence's step: the receive hook's result is added to
       it; 0xFF ends the sequence (_dirFailAuto). */
    u8 seq;
    /* +0x47 the item a step reads (PADCMD.OBJ counts it while it loads the
       tables; the direct-port sequencer writes 0). */
    u8 index;
    /* +0x48 the step's item count (PADCMD.OBJ). */
    u8 count;
    /* +0x49 the port's state (PadState*: 2 = PadStateFindCTP1 ...). */
    u8 state;
    /* +0x4A the failures in a row: _dirFailAuto counts it and gives up at the
       twelfth. */
    u8 retry;
    /* +0x4B a byte nothing reads or writes. */
    u8 pad_4b;
    /* +0x4C every failed transfer: _dirFailAuto counts it; PadInfoMode returns
       it for term 100. */
    s32 fail_count;
    /* +0x50 a byte the direct-port and PADMAIN code test (non-zero: a queued
       command waits); nothing read here says more. */
    u8 unk_50;
    /* +0x51 the mode _padSetMainMode was asked for: the command sends this
       byte and mode_lock (cmd_data = &mode_offs). */
    u8 mode_offs;
    /* +0x52 _padSetMainMode's lock argument. */
    u8 mode_lock;
    /* +0x53 1 when the mode asked for is the current one (cur_ex_offs). */
    u8 mode_same;
    /* +0x54 three bytes nothing reads or writes. */
    u8 pad_54[3];
    /* +0x57 which of the six actuator bytes may be sent (the direct-port
       sequencer clears and sets them). */
    u8 act_enable[6];
    /* +0x5D the alignment sent to the controller: func_800A68C8 writes 0xFF or
       the actuator's number. */
    u8 act_align[6];
    /* +0x63 the buffer the ID, actuator and combination tables are built in
       (_padRecvAtLoadInfo, _padLoadActInfo; sizeof is taken). */
    u8 info_buf[0x80];
    /* +0xE3 the number of extended IDs (PadInfoMode term 4). */
    u8 id_num;
    /* +0xE4 the current ID's offset (PadInfoMode term 3). */
    u8 cur_ex_offs;
    /* +0xE5 a byte nothing reads or writes. */
    u8 pad_e5;
    /* +0xE6 the current extended ID (PadInfoMode term 2). */
    u16 cur_ex_id;
    /* +0xE8 the current ID, the reply's first byte >> 4 (PadInfoMode term 1). */
    u8 cur_id;
    /* +0xE9 the number of actuators. */
    u8 act_num;
    /* +0xEA the number of combinations. */
    u8 comb_num;
    /* +0xEB PADCMD.OBJ writes 0 each time an item is taken; nothing reads it. */
    u8 unk_eb;
    /* +0xEC the combination table's size so far (PADCMD.OBJ). */
    u16 comb_size;
    /* +0xEE what the previous reply gave for the item being read (the item, or
       0 / 0xFFFF for same / differs): an item is taken when two replies in a
       row agree. */
    u16 prev_reply;
} PadInfo;

LAYOUT_SIZE(PadInfo, 0xf0);
LAYOUT_AT(PadInfo, id_table, id_table, 0x00);
LAYOUT_AT(PadInfo, act_table, act_table, 0x04);
LAYOUT_AT(PadInfo, comb_table, comb_table, 0x08);
LAYOUT_AT(PadInfo, tap_info, tap_info, 0x0c);
LAYOUT_AT(PadInfo, head, head, 0x10);
LAYOUT_AT(PadInfo, send_func, send_func, 0x14);
LAYOUT_AT(PadInfo, recv_func, recv_func, 0x18);
LAYOUT_AT(PadInfo, pad_1c, pad_1c, 0x1c);
LAYOUT_AT(PadInfo, align_data, align_data, 0x20);
LAYOUT_AT(PadInfo, cmd_param, cmd_param, 0x24);
LAYOUT_AT(PadInfo, pad_25, pad_25, 0x25);
LAYOUT_AT(PadInfo, act_data, act_data, 0x28);
LAYOUT_AT(PadInfo, cmd_data, cmd_data, 0x2c);
LAYOUT_AT(PadInfo, rx_buf, rx_buf, 0x30);
LAYOUT_AT(PadInfo, act_len, act_len, 0x34);
LAYOUT_AT(PadInfo, rx_len, rx_len, 0x35);
LAYOUT_AT(PadInfo, cmd_len, cmd_len, 0x36);
LAYOUT_AT(PadInfo, cmd_queued, cmd_queued, 0x37);
LAYOUT_AT(PadInfo, cmd_taken, cmd_taken, 0x38);
LAYOUT_AT(PadInfo, unk_39, unk_39, 0x39);
LAYOUT_AT(PadInfo, pad_3a, pad_3a, 0x3a);
LAYOUT_AT(PadInfo, recv_buf, recv_buf, 0x3c);
LAYOUT_AT(PadInfo, send_buf, send_buf, 0x40);
LAYOUT_AT(PadInfo, recv_len, recv_len, 0x44);
LAYOUT_AT(PadInfo, send_len, send_len, 0x45);
LAYOUT_AT(PadInfo, seq, seq, 0x46);
LAYOUT_AT(PadInfo, index, index, 0x47);
LAYOUT_AT(PadInfo, count, count, 0x48);
LAYOUT_AT(PadInfo, state, state, 0x49);
LAYOUT_AT(PadInfo, retry, retry, 0x4a);
LAYOUT_AT(PadInfo, pad_4b, pad_4b, 0x4b);
LAYOUT_AT(PadInfo, fail_count, fail_count, 0x4c);
LAYOUT_AT(PadInfo, unk_50, unk_50, 0x50);
LAYOUT_AT(PadInfo, mode_offs, mode_offs, 0x51);
LAYOUT_AT(PadInfo, mode_lock, mode_lock, 0x52);
LAYOUT_AT(PadInfo, mode_same, mode_same, 0x53);
LAYOUT_AT(PadInfo, pad_54, pad_54, 0x54);
LAYOUT_AT(PadInfo, act_enable, act_enable, 0x57);
LAYOUT_AT(PadInfo, act_align, act_align, 0x5d);
LAYOUT_AT(PadInfo, info_buf, info_buf, 0x63);
LAYOUT_AT(PadInfo, id_num, id_num, 0xe3);
LAYOUT_AT(PadInfo, cur_ex_offs, cur_ex_offs, 0xe4);
LAYOUT_AT(PadInfo, pad_e5, pad_e5, 0xe5);
LAYOUT_AT(PadInfo, cur_ex_id, cur_ex_id, 0xe6);
LAYOUT_AT(PadInfo, cur_id, cur_id, 0xe8);
LAYOUT_AT(PadInfo, act_num, act_num, 0xe9);
LAYOUT_AT(PadInfo, comb_num, comb_num, 0xea);
LAYOUT_AT(PadInfo, unk_eb, unk_eb, 0xeb);
LAYOUT_AT(PadInfo, comb_size, comb_size, 0xec);
LAYOUT_AT(PadInfo, prev_reply, prev_reply, 0xee);

/* SvmVagAddrs: what libsnd keeps in the reserved2 word of each of the program
   headers at _svm_pg (D_800EA6D4): the SPU addresses, in units of 8 bytes, of
   two VAGs, so the table of program headers is also the table of VAG
   addresses, VAG n (from 1) in row (n - 1) / 2. */
typedef struct {
    /* +0x00 the address / 8 of the odd-numbered VAG of the pair (VAG 2k + 1):
       SsVabTransBody's loop writes it (the start plus the lengths so far), the
       voice set-up (libsnd_vs_vtb.c) reads it into the voice's start-address
       register for an odd VAG. */
    u16 odd;
    /* +0x02 the address / 8 of the even-numbered VAG of the pair (VAG 2k + 2),
       written and read the same way. */
    u16 even;
} SvmVagAddrs;

LAYOUT_SIZE(SvmVagAddrs, 0x04);
LAYOUT_AT(SvmVagAddrs, odd, odd, 0x00);
LAYOUT_AT(SvmVagAddrs, even, even, 0x02);

#endif
