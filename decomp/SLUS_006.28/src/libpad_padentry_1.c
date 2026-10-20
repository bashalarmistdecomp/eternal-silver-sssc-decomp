/* PSY-Q 4.5 libpad, member padentry.o, part 1 of 2: PadChkVsync,
   PadStartCom, PadStopCom, PadChkMtap, PadGetState. The first split had it
   in psyq_misc_aak.c. The tree compiles this run of the member's functions
   with other flags than the runs beside it (config/cc_flags.txt), so the
   member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: PadChkVsync */
s32 _padChkVsync(void);

/* libpad PadChkVsync (PADENTRY.OBJ): returns _padChkVsync() (PADMAIN.OBJ),
   which returns a flag word of PADMAIN.OBJ's data and clears it. Nothing in the image
   calls it. */
/* func_800A4CF8 */
s32 PadChkVsync(void)
{
    return _padChkVsync();
}

/* cc97_lib: PadStartCom */
s32 _padStartCom();                                  /* extern */

/* libpad PadStartCom (PADENTRY.OBJ): calls _padStartCom (PADMAIN.OBJ). */
/* func_800A4D18 */
void PadStartCom(void) {
    _padStartCom();
}
/* cc97_lib: PadStopCom */
s32 _padStopCom();                                  /* extern */

/* libpad PadStopCom (PADENTRY.OBJ): calls _padStopCom (PADMAIN.OBJ). */
/* func_800A4D38 */
void PadStopCom(void) {
    _padStopCom();
}
/* cc97_lib: PadChkMtap */
/* far_syms: D_800B84BC (target reaches it %hi/%lo) */
/* far_syms: D_800B84D4 (target reaches it %hi/%lo) */
/* far_syms: D_800B84D4 (auto: target reaches it %hi/%lo — addressing row 3) */
extern PadInfo *D_800B84BC;          /* _padInfoDir: the two ports' records (0xF0 bytes each) */
extern s32 D_800B84D4;                /* _padModeMtap */

/* libpad PadChkMtap(port) (PADENTRY.OBJ): 1 when a multi tap answers on the port.
   0 unless _padModeMtap (D_800B84D4) is set; then whether the current ID (cur_id,
   +0xE8) of record port >> 4 of _padInfoDir (D_800B84BC) is 8. Nothing in the image
   calls it. */
/* func_800A4D58 */
s32 PadChkMtap(s32 port)
{
    if (D_800B84D4 != 0) {
        return D_800B84BC[port >> 4].cur_id == 8;
    }
    return 0;
}
/* cc97_lib: PadGetState */
/* far_syms: D_800B849C */
extern PadInfo *(*D_800B849C)(s32);

/* libpad PadGetState (PADENTRY.OBJ): the connection state of the controller on `port`
   (libpad.h: 0 PadStateDiscon, 1 FindPad, 2 FindCTP1, 3 FindCTP2, 4 ReqInfo, 5 ExecCmd,
   6 Stable). Gets the port's record through _padFuncPort2Info (D_800B849C) and returns
   its state byte (+0x49), except while the port is busy or failing: a command queued
   (+0x37) or taken (+0x38), the +0x39 byte set on a record other than the one its +0x10
   points at, or a non-zero first byte in the receive buffer (+0x30). Then FindCTP1 and
   FindCTP2 are reported as FindPad and Stable as ReqInfo; the other states pass
   unchanged. Caller in the image: pad_update. */
/* func_800A4DA4 */
s32 PadGetState(s32 port)
{
    PadInfo *p = D_800B849C(port);

    if (p->cmd_queued != 0 || p->cmd_taken != 0 || (p != p->head && p->unk_39 != 0) || *p->rx_buf != 0) {
        switch (p->state) {
        case 2:
            return 1;
        case 3:
            return 1;
        case 6:
            return 4;
        }
    }
    return p->state;
}
