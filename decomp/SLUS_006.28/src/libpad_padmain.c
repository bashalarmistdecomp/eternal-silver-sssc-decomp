/* PSY-Q 4.5 libpad, member padmain.o: PadEnableCom, _padSetVsyncParam,
   _padIsVsync, _padIntPad, _padChkVsync, _padStartCom, _padStopCom,
   _padInitSioMode, func_800A594C, _padSioRW, _padSioRW2, _padClrIntSio0,
   _padWaitRXready. The first split had it in psyq_misc_aan.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* Sony libpad, PADMAIN.OBJ whole (0x800A51B8..0x800A5F87 of the image). Every word equals the
   on-disk SDK 4.4 object's, relocation fields apart; the release linked is SDK 4.5's, which is
   the same code (the two LIBPAD.LIB differ only in member dates and in the version word of
   PADMAIN.OBJ's .data, and the image holds 4.5's, 0x00450000 at 0x800B8484). These functions
   were `.word` rows of asm/data/data_tables.data.s: the first split cut text from data at
   0x800A5000, and the image's text runs to 0x800A7D98. No config/cc_flags.txt entry: the
   object's converted functions take per-function markers (cc97_lib, and return_slot_fill by the
   epilogue rule). */

/* far_syms: D_800B8488 */
/* far_syms: D_800B848C */
/* far_syms: D_800B84A8 */
/* far_syms: D_800B84AC */
/* far_syms: D_800B84B0 */
/* far_syms: D_800B84BC */
/* far_syms: D_800B84C0 */
/* far_syms: D_800B84C8 */
/* far_syms: D_800B84CC */
/* far_syms: D_800B84D0 */
/* far_syms: D_800B84D8 */
/* far_syms: D_800B84DC */
/* far_syms: D_800B84E8 */
/* far_syms: D_800B84EC */
/* far_syms: D_800B84F0 */

/* The declarations PADMAIN.OBJ's functions share. The object was built -G0: every .data
   scalar above is reached %hi / %lo (the far_syms lines), and the two small arrays are
   declared unsized so that cc1 does not take them for small data. */

/* The interrupt controller's registers at 0x1F801070. */

/* The SIO0 (controller / memory card port) registers at 0x1F801040. */

/* An entry of the kernel's interrupt queues (SysEnqIntRP / SysDeqIntRP): the kernel calls
   func1 and, when it returns nonzero, func2. The layout libapi PAD.OBJ's entry has
   (libapi_pad_3.c). */

extern volatile PadIntr *D_800B84E8;   /* PADMAIN .data+0x68 -> 0x1F801070 (I_STAT, I_MASK) */
extern volatile PadSio *D_800B84EC;    /* PADMAIN .data+0x6C -> 0x1F801040 (SIO0) */
extern PadIntRP D_800EAD40;            /* PADMAIN .bss+0x00: its entry for the kernel's interrupt queue 2 */
extern s32 D_800EAD50[];               /* PADMAIN .bss+0x10: per port, the vertical blanks it has been switched off (up to 150) */
extern PadInfo *D_800B84BC;            /* _padInfoDir: the two ports' records */
extern s32 D_800B84C0;                 /* _padIntExec: the interrupt handler serves the ports only while this is set */
extern s32 D_800B84C8;                 /* _padSioChan: the port being served */
extern s32 D_800B84CC;                 /* _padSioState: the step of the port's transfer (index into padIntFunc) */
extern s32 D_800B84D0;                 /* _padTotalCurr */
extern s32 D_800B84D8;                 /* _padChanStart: the first port served (0, or 1 with port 0 switched off) */
extern s32 D_800B84DC;                 /* _padChanStop: the last port served (1, or 0 with port 1 switched off) */
extern s32 D_800B84E0[];               /* _padFixResult: one word per port */
extern s32 D_800B84F0;                 /* PADMAIN .data+0x70: set by the interrupt handler at each vertical blank it runs to the end; _padChkVsync returns and clears it */
extern void (*D_800B8488)(s32);        /* _padFuncNextPort */
extern void (*D_800B848C)(PadInfo *);  /* _padFuncClrInfo */
extern void (*D_800B84A8)(PadInfo *);  /* _padFuncRecvAuto */
extern void (*D_800B84AC)(PadInfo *);  /* _padFuncClrCmdNo */
extern void (*D_800B84B0)(void);       /* _padFuncIntGun: never installed in this image */
extern s32 (*D_800B8510[])(PadInfo *); /* padIntFunc (PADIF.OBJ .data+0x10): the five transfer steps */

s32 EnterCriticalSection(void);
s32 ChangeClearRCnt(s32, s32);
s32 SysEnqIntRP(s32, PadIntRP *);
s32 SysDeqIntRP(s32, PadIntRP *);
void setRC2wait(u32);               /* setRC2wait (WAITRC2.OBJ) */
s32 chkRC2wait(void);               /* chkRC2wait (WAITRC2.OBJ) */

u32 PadEnableCom(u32 mode);
void _padSetVsyncParam(void);
s32 _padIsVsync(void);
s32 _padIntPad(void);
s32 _padChkVsync(void);
void _padStartCom(void);
void _padStopCom(void);
s32 _padInitSioMode(PadInfo *p);
void func_800A594C(PadInfo *p);
s32 _padClrIntSio0(void);
void _padWaitRXready(void);

/* libpad PadEnableCom (PADMAIN.OBJ +0x000), 0x118 bytes. */
/* cc97_lib: PadEnableCom */
/* return_slot_fill: PadEnableCom */

/* libpad PadEnableCom (PADMAIN.OBJ, SDK 4.5): switch communication on or off per port; bit 0
   of `mode` is port 0, bit 1 port 1, 1 = on. Returns the setting it found, read back from
   _padChanStart (D_800B84D8, 0 while port 0 is served) and _padChanStop (D_800B84DC, 1 while
   port 1 is), and changes nothing when `mode` is that setting. Otherwise, with _padIntExec
   (D_800B84C0) at 0 so that the interrupt handler _padIntPad starts no transfer
   meanwhile: a port switched on has its record cleared through _padFuncClrInfo (D_800B848C)
   if it had been off for 150 vertical blanks or more (the count _padIntPad keeps in
   D_800EAD50[port]) and its count zeroed; a port switched off only moves the bound. Nothing
   in the image calls it. */
/* func_800A51B8 */
u32 PadEnableCom(u32 mode)
{
    u32 old = (D_800B84DC << 1) | (D_800B84D8 == 0);

    if (old == mode) {
        return old;
    }
    D_800B84C0 = 0;
    if (mode & 1) {
        D_800B84D8 = 0;
        if (D_800EAD50[0] >= 150) {
            D_800B848C(&D_800B84BC[0]);
        }
        D_800EAD50[0] = 0;
    } else {
        D_800B84D8 = 1;
    }
    if (mode & 2) {
        D_800B84DC = 1;
        if (D_800EAD50[1] >= 150) {
            D_800B848C(&D_800B84BC[1]);
        }
        D_800EAD50[1] = 0;
    } else {
        D_800B84DC = 0;
    }
    D_800B84C0 = 1;
    return old;
}

/* libpad _padSetVsyncParam (PADMAIN.OBJ +0x118), 0x2C bytes. */
/* cc97_lib: _padSetVsyncParam */
/* return_slot_fill: _padSetVsyncParam */

/* libpad _padSetVsyncParam (PADMAIN.OBJ, SDK 4.5): fill PADMAIN's entry for the kernel's
   interrupt queue, D_800EAD40: the handler (func2) is _padIntPad, the verifier (func1)
   _padIsVsync, next and pad zero. _padStartCom puts the entry in queue 2.
   Called from PadInitDirect. */
/* func_800A52D0 */
void _padSetVsyncParam(void)
{
    D_800EAD40.func2 = _padIntPad;
    D_800EAD40.func1 = _padIsVsync;
    D_800EAD40.next = 0;
    D_800EAD40.unk_0c = 0;
}

/* libpad PADMAIN.OBJ +0x144, a static of the object (no SDK symbol): its address is taken in
   _padSetVsyncParam [_padSetVsyncParam]. 0x68 bytes. */
/* cc97_lib: _padIsVsync */

/* libpad PADMAIN.OBJ +0x144 (SDK 4.5), a static of the object with no symbol there; SDK 4.6
   and later name it _padIsVsync. The verifier of PADMAIN's interrupt-queue entry
   (D_800EAD40.func1): 0 unless the vertical-blank interrupt (bit 0) is enabled in I_MASK and
   pending in I_STAT; then it calls the light-gun hook _padFuncIntGun (D_800B84B0) when one
   is installed (this image installs none) and returns 1, on which the kernel calls the
   handler _padIntPad. */
/* func_800A52FC */
s32 _padIsVsync(void)
{
    if ((D_800B84E8->mask & 1) == 0 || (D_800B84E8->stat & 1) == 0) {
        return 0;
    }
    if (D_800B84B0 != 0) {
        D_800B84B0();
    }
    return 1;
}

/* libpad PADMAIN.OBJ +0x1AC, a static of the object (no SDK symbol): its address is taken in
   _padSetVsyncParam [_padSetVsyncParam]. 0x190 bytes. */
/* cc97_lib: _padIntPad */
/* return_slot_fill: _padIntPad */

/* libpad PADMAIN.OBJ +0x1AC (SDK 4.5), a static of the object with no symbol there; SDK 4.6
   and later name it _padIntPad. The handler of PADMAIN's interrupt-queue entry
   (D_800EAD40.func2), run at every vertical blank. If the controller port still drives its
   select line (SIO0 control bit 1: the last frame's transfer never ended) it clears the
   control register and does nothing more. Otherwise it sets the flag _padChkVsync reports
   (D_800B84F0), counts the frame for each port PadEnableCom has switched off
   (D_800EAD50[port], up to 150) and, while _padIntExec (D_800B84C0) is set and a port is
   on, serves the ports: _padSioState (D_800B84CC) = 0, _padSioChan (D_800B84C8) =
   _padChanStart (D_800B84D8), _padInitSioMode on that port's record (a 0
   result is reported to _padFuncNextPort, D_800B8488, as 0xFFFF), _padTotalCurr
   (D_800B84D0) = 0, then func_800A594C on record _padSioChan for as long as _padSioChan has
   not passed _padChanStop (D_800B84DC); the callee's hooks step it. Last it puts SIO0's
   baud reload back to 0x88. Returns 0. */
/* func_800A5364 */
s32 _padIntPad(void)
{
    if (D_800B84EC->ctrl & 2) {
        D_800B84EC->ctrl = 0;
        return 0;
    }
    D_800B84F0 = 1;
    if (D_800B84D8 != 0) {
        if (D_800EAD50[0] < 150) {
            D_800EAD50[0]++;
        }
    }
    if (D_800B84DC == 0) {
        if (D_800EAD50[1] < 150) {
            D_800EAD50[1]++;
        }
    }
    if (D_800B84C0 != 0) {
        if (D_800B84D8 <= D_800B84DC) {
            D_800B84CC = 0;
            D_800B84C8 = D_800B84D8;
            if (_padInitSioMode(&D_800B84BC[D_800B84C8]) == 0) {
                D_800B8488(0xFFFF);
            }
            D_800B84D0 = 0;
            while (D_800B84C8 <= D_800B84DC) {
                func_800A594C(&D_800B84BC[D_800B84C8]);
            }
            D_800B84EC->baud = 0x88;
        }
    }
    return 0;
}

/* libpad _padChkVsync (PADMAIN.OBJ +0x33C), 0x14 bytes. */
/* cc97_lib: _padChkVsync */
/* return_slot_fill: _padChkVsync */

/* libpad _padChkVsync (PADMAIN.OBJ, SDK 4.5): return the flag the interrupt handler
   _padIntPad sets at each vertical blank it is not locked out of (D_800B84F0), and clear
   it. Called from PadChkVsync. */
/* func_800A54F4 */
s32 _padChkVsync(void)
{
    s32 ret = D_800B84F0;

    D_800B84F0 = 0;
    return ret;
}

/* libpad _padStartCom (PADMAIN.OBJ +0x350), 0xCC bytes. */
/* cc97_lib: _padStartCom */
/* return_slot_fill: _padStartCom */

/* libpad _padStartCom (PADMAIN.OBJ, SDK 4.5): start controller communication. With
   _padIntExec (D_800B84C0) at 0 and inside a critical section it puts PADMAIN's entry
   D_800EAD40 at the head of the kernel's interrupt queue 2 (SysDeqIntRP, then SysEnqIntRP),
   acknowledges the vertical-blank interrupt in I_STAT, enables it in I_MASK and calls
   ChangeClearRCnt(3, 0) (inferred from the call's name and from libetc's use of it: the
   kernel stops acknowledging the vertical-blank interrupt by itself). Then it clears both
   ports' records through _padFuncClrInfo (D_800B848C), zeroes the two switched-off frame
   counts (D_800EAD50) and sets _padIntExec to 1. Called from PadStartCom. */
/* func_800A5508 */
void _padStartCom(void)
{
    D_800B84C0 = 0;
    EnterCriticalSection();
    SysDeqIntRP(2, &D_800EAD40);
    SysEnqIntRP(2, &D_800EAD40);
    D_800B84E8->stat = ~1;
    D_800B84E8->mask |= 1;
    ChangeClearRCnt(3, 0);
    ExitCriticalSection();
    D_800B848C(&D_800B84BC[0]);
    D_800B848C(&D_800B84BC[1]);
    D_800EAD50[0] = D_800EAD50[1] = 0;
    D_800B84C0 = 1;
}

/* libpad _padStopCom (PADMAIN.OBJ +0x41C), 0x44 bytes. */
/* cc97_lib: _padStopCom */

/* libpad _padStopCom (PADMAIN.OBJ, SDK 4.5): stop controller communication: inside a
   critical section, ChangeClearRCnt(3, 1) (inferred, as in _padStartCom: the kernel
   acknowledges the vertical-blank interrupt by itself again) and SysDeqIntRP takes
   PADMAIN's entry D_800EAD40 out of the kernel's interrupt queue 2. Called from PadStopCom
   . */
/* func_800A55D4 */
void _padStopCom(void)
{
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &D_800EAD40);
    ExitCriticalSection();
}

/* libpad _padInitSioMode (PADMAIN.OBJ +0x460), 0x334 bytes. */
/* cc97_lib: _padInitSioMode */
/* return_slot_fill: _padInitSioMode */
/* reorder_fill: _padInitSioMode */

/* libpad _padInitSioMode (PADMAIN.OBJ, SDK 4.5): prepare SIO0 for a transfer with the port
   _padSioChan (D_800B84C8) names, whose record is p. It resets the port (control 0x40, then
   0), sets mode 0xD and baud reload 0x88, arms the root-counter-2 time-out (setRC2wait: 80 when the record's current ID is 8, else 145) and writes control 0x1003
   for port 0 or 0x3003 for port 1. Then it pays out _padFixResult (D_800B84E0[_padSioChan]):
   a positive count is stepped down to 0, calling _padFuncRecvAuto (D_800B84A8) on record
   p->tap_info[count] after each step; at 0 the word becomes -1 and _padFuncRecvAuto and
   _padFuncClrCmdNo (D_800B84AC) are called on p; a negative word is left alone. If SIO0's
   status shows an interrupt request (0x200) it acknowledges it (control |= 0x10). If the
   request is gone, it acknowledges the controller interrupt in I_STAT (bit 7) too. If the
   request stays, it waits until chkRC2wait reports the armed time over,
   clocks out the three bytes 1, 0x42, 1 (each: write, setRC2wait, _padClrIntSio0
   , _padWaitRXready, read the answer and drop it; before the
   second and third byte it waits for I_STAT bit 7 and gives up when chkRC2wait reports the
   time over) and returns 0, as it does whenever _padClrIntSio0 returns 0. Otherwise it
   returns 0 when the record's +0x50 byte and a queued command (+0x37) are both set, and 1
   if not. Callers: _padIntPad and func_800A7380 (the function behind _padFuncNextPort). */
/* func_800A5618 */
s32 _padInitSioMode(PadInfo *p)
{
    D_800B84EC->ctrl = 0x40;
    D_800B84EC->ctrl = 0;
    D_800B84EC->mode = 0xD;
    D_800B84EC->baud = 0x88;
    setRC2wait(p->cur_id == 8 ? 80 : 145);
    D_800B84EC->ctrl = D_800B84C8 != 0 ? 0x3003 : 0x1003;
    if (D_800B84E0[D_800B84C8] >= 0) {
        while (D_800B84E0[D_800B84C8] > 0) {
            D_800B84E0[D_800B84C8]--;
            D_800B84A8(&p->tap_info[D_800B84E0[D_800B84C8]]);
        }
        if (D_800B84E0[D_800B84C8] == 0) {
            D_800B84E0[D_800B84C8] = -1;
            D_800B84A8(p);
            D_800B84AC(p);
        }
    }
    if (D_800B84EC->stat & 0x200) {
        D_800B84EC->ctrl |= 0x10;
        if (D_800B84EC->stat & 0x200) {
            while (chkRC2wait() == 0) {
            }
            D_800B84EC->data = 1;
            setRC2wait(2000);
            if (_padClrIntSio0() == 0) {
                return 0;
            }
            _padWaitRXready();
            D_800B84EC->data;
            setRC2wait(430);
            while (!(D_800B84E8->stat & 0x80)) {
                if (chkRC2wait() != 0) {
                    return 0;
                }
            }
            D_800B84EC->data = 0x42;
            setRC2wait(60);
            if (_padClrIntSio0() == 0) {
                return 0;
            }
            _padWaitRXready();
            D_800B84EC->data;
            setRC2wait(430);
            while (!(D_800B84E8->stat & 0x80)) {
                if (chkRC2wait() != 0) {
                    return 0;
                }
            }
            D_800B84EC->data = 1;
            setRC2wait(60);
            if (_padClrIntSio0() == 0) {
                return 0;
            }
            _padWaitRXready();
            D_800B84EC->data;
            return 0;
        }
        D_800B84E8->stat = ~0x80;
    }
    if (p->unk_50 != 0 && p->cmd_queued != 0) {
        return 0;
    }
    return 1;
}

/* libpad PADMAIN.OBJ +0x794, a static of the object (no SDK symbol): called from _padIntPad.
   0xF0 bytes. */
/* cc97_lib: func_800A594C */
/* return_slot_fill: func_800A594C */
/* lib_g0: func_800A594C */

/* libpad PADMAIN.OBJ +0x794 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by its role: one step of the transfer with the controller on
   the port being served. Runs the step function padIntFunc[_padSioState] (D_800B8510,
   PADIF.OBJ's five) on the port's record and moves _padSioState (D_800B84CC) on. A
   negative result ends the transfer: it goes to _padFuncNextPort (D_800B8488).
   Otherwise, unless the step left the state at 0 or the state is now 3 with 0x80 as the
   first reply byte (a multi tap, whose interrupt _padSioRW2 has acknowledged already),
   it starts a wait of 0x3C with setRC2wait and acknowledges the port's
   interrupt with _padClrIntSio0; when that times out the transfer ends
   with _padFuncNextPort(-3). A state past the last step (5 or more) is put back by one,
   so the last step, PADIF.OBJ's fifth, runs for every further byte. Caller:
   _padIntPad (the interrupt handler; 4.6+ _padIntPad). */
void func_800A594C(PadInfo *p)
{
    s32 ret = D_800B8510[D_800B84CC++](p);

    if (ret >= 0) {
        if (D_800B84CC != 0) {
            if (D_800B84CC != 3 || *p->recv_buf != 0x80) {
                setRC2wait(0x3C);
                if (_padClrIntSio0() == 0) {
                    D_800B8488(-3);
                }
            }
        }
        if (D_800B84CC >= 5) {
            D_800B84CC--;
        }
    } else {
        D_800B8488(ret);
    }
}

/* libpad _padSioRW (PADMAIN.OBJ +0x884), 0x210 bytes. */
/* cc97_lib: _padSioRW */
/* return_slot_fill: _padSioRW */
extern u32 D_800EAFE0;
extern u32 D_800EAFE4;

/* libpad _padSioRW (PADMAIN.OBJ, SDK 4.5): exchanges one byte with the controller on the
   port being served and returns the byte that came in. data < 0 starts a transfer: the
   byte waiting in SIO0's data register is read (and returned), the record's counters
   are reset (recv_len 0xFF, the start value _padSioRW2 tests before it keeps a byte;
   send_len 1), ~data (the address byte: -2 gives 0x01) goes to the head of send_buf,
   and once the port can send (status bit 0) and the wait the caller started
   is over (chkRC2wait) it is written to the data register. Otherwise:
   picks the baud reload value (0x88, or 0x22 once more than eight bytes of a multi
   tap's reply, first byte 0x8n, are in), starts a wait of 0x1AE (setRC2wait's two
   statements: _waitTime D_800EAFE4, _startTime D_800EAFE0), waits for a received byte
   (status bit 1), reads it, sets the baud register, then waits for the port's interrupt
   (bit 7 of the interrupt status) and returns -0x14 when the wait runs out first. Then
   writes `data`; at the fast rate it acknowledges the interrupt at once (interrupt
   status bit 7 cleared, control bit 4 set). Counts the byte sent (send_len), stores the
   byte read at recv_buf[recv_len] and counts it. Callers: PADIF.OBJ's step functions.
   MARKED SPELLING: the reply's first byte goes through the int local `id` before
   its high nibble is tested. The binary shifts it arithmetically (`lbu` then `sra`), and
   the C front end shifts arithmetically only an int operand: `*p->recv_buf >> 4`
   written directly is narrowed to an unsigned char shift (`srl`, 131/132; PADIF.OBJ and
   PADSEQD.OBJ have `srl` for the same test). So the source had the byte in an int
   there; whether as a local, a macro's or an inline function's parameter is not
   known. */
/* func_800A5A3C */
s32 _padSioRW(PadInfo *p, s32 data)
{
    s32 baud;
    s32 ret;
    s32 id;

    if (data < 0) {
        ret = D_800B84EC->data;
        p->recv_len = 0xFF;
        p->send_len = 1;
        *p->send_buf = ~data;
        while (!(D_800B84EC->stat & 1)) {
        }
        while (!chkRC2wait()) {
        }
        D_800B84EC->data = ~data;
    } else {
        id = *p->recv_buf;
        baud = 0x88;
        if ((id >> 4) == 8 && p->recv_len >= 9) {
            baud = 0x22;
        }
        D_800EAFE4 = 0x1AE;
        D_800EAFE0 = *(volatile u16 *)0x1F801120;
        while (!(D_800B84EC->stat & 2)) {
        }
        ret = D_800B84EC->data;
        D_800B84EC->baud = baud;
        while (!(D_800B84E8->stat & 0x80)) {
            if (chkRC2wait()) {
                return -0x14;
            }
        }
        D_800B84EC->data = data;
        if (baud == 0x22) {
            D_800B84E8->stat = ~0x80;
            D_800B84EC->ctrl |= 0x10;
        }
        p->send_len++;
        p->recv_buf[p->recv_len] = ret;
        p->recv_len++;
    }
    return ret;
}

/* libpad _padSioRW2 (PADMAIN.OBJ +0xA94), 0x278 bytes. */
/* cc97_lib: _padSioRW2 */
/* return_slot_fill: _padSioRW2 */

/* libpad _padSioRW2 (PADMAIN.OBJ, SDK 4.5): exchanges one byte with the controller on
   the port being served and returns the byte that came in, or -2 when the port's
   interrupt does not come in time; the form the first steps of a transfer use. Picks
   the baud reload value as _padSioRW does (0x88, or 0x22 past the eighth byte of a
   multi tap's reply), waits for a received byte (status bit 1), starts a wait of 0x190
   with setRC2wait and reads the byte. When it is the reply's first
   (recv_len 0) and 0x8n, a multi tap's ID, the baud register is set to 0x22 at once,
   otherwise to the value picked. Then it waits for bit 7 of the interrupt status,
   running chkRC2wait's test in line (root counter 2 against _startTime D_800EAFE0 and
   _waitTime D_800EAFE4). For a controller that is not a multi tap (cur_id not 8) at
   transfer step 2 (_padSioState, D_800B84CC) it waits a further 0x3C. Writes `data`;
   at step 3 with 0x80 just read it acknowledges the interrupt at once (interrupt status
   bit 7 cleared, control bit 4 set). Counts the byte sent (send_len), stores the byte
   read at recv_buf[recv_len] unless recv_len is still the start value 0xFF, and counts
   it. Callers: PADIF.OBJ's step functions.
   MARKED SPELLING: the reply's first byte goes through the int local `id` before
   its high nibble is tested. The binary shifts it arithmetically (`lbu` then `sra`), and
   the C front end shifts arithmetically only an int operand: `*p->recv_buf >> 4`
   written directly is narrowed to an unsigned char shift (`srl`, 157/158; PADIF.OBJ and
   PADSEQD.OBJ have `srl` for the same test). So the source had the byte in an int
   there; whether as a local, a macro's or an inline function's parameter is not
   known. */
/* func_800A5C4C */
s32 _padSioRW2(PadInfo *p, s32 data)
{
    s32 baud;
    s32 ret;
    s32 id;
    u32 now;

    id = *p->recv_buf;
    baud = 0x88;
    if ((id >> 4) == 8 && p->recv_len >= 9) {
        baud = 0x22;
    }
    do {
    } while (!(D_800B84EC->stat & 2));
    setRC2wait(0x190);
    ret = D_800B84EC->data;
    if (p->recv_len != 0 || (ret >> 4) != 8) {
        D_800B84EC->baud = baud;
    } else {
        D_800B84EC->baud = 0x22;
    }
    while (!(D_800B84E8->stat & 0x80)) {
        now = *(volatile u16 *)0x1F801120;
        if (now < D_800EAFE0) {
            if (*(volatile u16 *)0x1F801128 != 0) {
                now += *(volatile u16 *)0x1F801128;
            } else {
                now += 0x10000;
            }
        }
        if (*(volatile u16 *)0x1F801124 & 0x200) {
            if (now - D_800EAFE0 >= D_800EAFE4) {
                return -2;
            }
        } else {
            if ((now - D_800EAFE0) / 8 >= D_800EAFE4) {
                return -2;
            }
        }
    }
    if (p->cur_id != 8 && D_800B84CC == 2) {
        setRC2wait(0x3C);
        while (!chkRC2wait()) {
        }
    }
    D_800B84EC->data = data;
    if (D_800B84CC == 3 && ret == 0x80) {
        D_800B84E8->stat = ~0x80;
        D_800B84EC->ctrl |= 0x10;
    }
    p->send_len++;
    if (p->recv_len != 0xFF) {
        p->recv_buf[p->recv_len] = ret;
    }
    p->recv_len++;
    return ret;
}

/* libpad _padClrIntSio0 (PADMAIN.OBJ +0xD0C), 0x90 bytes. */
/* cc97_lib: _padClrIntSio0 */

/* libpad _padClrIntSio0 (PADMAIN.OBJ, SDK 4.5): acknowledges the controller port's
   interrupt. Clears bit 7 (the controller / memory card interrupt) of the interrupt
   status register, then waits for bit 7 of SIO0's status register (the level of the
   controller's acknowledge line) to drop, giving up when chkRC2wait says
   the wait the caller started with setRC2wait is over: then it returns 0. Otherwise it
   sets bit 4 of SIO0's control register (resets the port's interrupt flag) and returns 1.
   Callers: _padInitSioMode, func_800A594C, PADIF.OBJ. */
/* func_800A5EC4 */
s32 _padClrIntSio0(void)
{
    D_800B84E8->stat = ~0x80;
    while (D_800B84EC->stat & 0x80) {
        if (chkRC2wait()) {
            return 0;
        }
    }
    D_800B84EC->ctrl |= 0x10;
    return 1;
}

/* libpad _padWaitRXready (PADMAIN.OBJ +0xD9C), 0x28 bytes. Followed by 3 zero words, the
   object's .text padded to 8 bytes (the rows after its endlabel). */
/* cc97_lib: _padWaitRXready */

/* libpad _padWaitRXready (PADMAIN.OBJ, SDK 4.5): waits, with no time limit, until SIO0's
   status register has bit 1 set (a received byte is waiting). Caller: _padInitSioMode
   . */
/* func_800A5F54 */
void _padWaitRXready(void)
{
    while ((D_800B84EC->stat & 2) == 0) {
    }
}
INCLUDE_PAD(12);
