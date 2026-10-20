/* PSY-Q 4.5 libpad, member padif.o: func_800A6B88, func_800A6BD0,
   func_800A6CA8, func_800A6D54, func_800A6E18. The first split had it in
   psyq_misc_aap.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* Sony libpad, PADIF.OBJ whole (0x800A6B88..0x800A71B7 of the image). Every word equals the
   on-disk SDK 4.4 object's, relocation fields apart; the release linked is SDK 4.5's, which is
   the same code (the two LIBPAD.LIB differ only in member dates and in the version word of
   PADMAIN.OBJ's .data, and the image holds 4.5's, 0x00450000 at 0x800B8484). These functions
   were `.word` rows of asm/data/data_tables.data.s: the first split cut text from data at
   0x800A5000, and the image's text runs to 0x800A7D98. No config/cc_flags.txt entry: the
   object's converted functions take per-function markers (cc97_lib, and return_slot_fill by the
   epilogue rule). */

/* libpad PADIF.OBJ +0x000, a static of the object (no SDK symbol): entry 0 of D_800B8510
   [padIntFunc]. 0x48 bytes. */
/* cc97_lib: func_800A6B88 */
/* return_slot_fill: func_800A6B88 */
/* far_syms: D_800B84A0 */
/* far_syms: D_800B8508 */
extern s32 (*D_800B84A0)(PadInfo *);   /* _padFuncSendAuto */
extern s32 D_800B8508;                 /* PADIF.OBJ .data+8: the last _padFuncSendAuto result */
s32 _padSioRW(PadInfo *p, s32 data);

/* libpad PADIF.OBJ +0x000 (SDK 4.5), a static with no SDK symbol: padIntFunc[0]
   (D_800B8510), step 0 of a controller transfer, named here from its role. Asks
   _padFuncSendAuto (D_800B84A0) what the port's record has to send and keeps the answer in
   the object's static D_800B8508 (steps 1 and 2 test it), clears the first received byte
   (recv_buf[0], where step 2's exchange will leave the controller's ID), and starts the
   transfer: _padSioRW with -2, a negative argument, for which that function
   resets the record's byte counts and writes ~(-2) = 1 to the port (1 is the address byte
   of the controller protocol; that reading is protocol knowledge, not the SDK's text).
   Returns _padSioRW's result (negative = error). */
s32 func_800A6B88(PadInfo *p)
{
    D_800B8508 = D_800B84A0(p);
    p->recv_buf[0] = 0;
    return _padSioRW(p, -2);
}

/* libpad PADIF.OBJ +0x048, a static of the object (no SDK symbol): entry 1 of D_800B8510
   [padIntFunc]. 0xD8 bytes. */
/* cc97_lib: func_800A6BD0 */
/* return_slot_fill: func_800A6BD0 */
/* far_syms: D_800B84C8 */
/* far_syms: D_800B84D8 */
/* far_syms: D_800B84C4 */
/* far_syms: D_800B84B8 */
/* far_syms: D_800B84B4 */
extern void (*D_800B84B8)(void), (*D_800B84B4)(void);   /* _padFuncGetGunPos, _padFuncSetGunPort */
extern s32 D_800B84C8;                 /* _padSioChan */
extern s32 D_800B84D8;                 /* _padChanStart */
extern s32 D_800B84C4;                 /* _padGunExec */
s32 _padSioRW2(PadInfo *p, s32 data);

/* libpad PADIF.OBJ +0x048 (SDK 4.5), a static with no SDK symbol: padIntFunc[1]
   (D_800B8510), step 1 of a controller transfer, named here from its role. When the port
   being served is the first one (_padSioChan D_800B84C8 == _padChanStart D_800B84D8) and
   _padGunExec (D_800B84C4) is set, it calls the two gun hooks, _padFuncGetGunPos
   (D_800B84B8) then _padFuncSetGunPort (D_800B84B4). If step 0's _padFuncSendAuto answered
   non-zero (D_800B8508) it calls that hook for the first two records behind the port
   (tap_info[0], [1]). Then it sends the command byte through _padSioRW2:
   the queued command (cmd_queued, +0x37) if there is one, else 0x42 (the protocol's "read
   controller" command; protocol knowledge, not the SDK's text). Returns _padSioRW2's
   result (negative = error). */
s32 func_800A6BD0(PadInfo *p)
{
    if (D_800B84C8 == D_800B84D8 && D_800B84C4 != 0) {
        D_800B84B8();
        D_800B84B4();
    }
    if (D_800B8508 != 0) {
        D_800B84A0(&p->tap_info[0]);
        D_800B84A0(&p->tap_info[1]);
    }
    if (p->cmd_queued != 0) {
        return _padSioRW2(p, p->cmd_queued);
    }
    return _padSioRW2(p, 0x42);
}

/* libpad PADIF.OBJ +0x120, a static of the object (no SDK symbol): entry 2 of D_800B8510
   [padIntFunc]. 0xAC bytes. */
/* cc97_lib: func_800A6CA8 */
/* return_slot_fill: func_800A6CA8 */
/* far_syms: D_800B84D4 */
/* far_syms: D_800B8500 */
extern s32 D_800B84D4;                 /* _padModeMtap */
extern s32 D_800B8500;                 /* PADIF.OBJ .data+0: bytes left in this transfer */

/* libpad PADIF.OBJ +0x120 (SDK 4.5), a static with no SDK symbol: padIntFunc[2]
   (D_800B8510), step 2 of a controller transfer, named here from its role. If step 0's
   _padFuncSendAuto answered non-zero (D_800B8508) it calls that hook for the last two
   records behind the port (tap_info[2], [3]). Then _padSioRW2 sends the
   third byte, 0 while a command is queued (cmd_queued, +0x37) and _padModeMtap
   (D_800B84D4) otherwise, and returns the byte received in answer to step 1's command
   (_padSioRW2 reads the port before it writes, and this byte is the first it stores:
   recv_buf[0], the controller's ID). The object's static D_800B8500, the count of bytes
   still to move, becomes twice the ID's low nibble, or 0x20 when the nibble is 0.
   Returns 0, or _padSioRW2's negative error. */
s32 func_800A6CA8(PadInfo *p)
{
    s32 ret;

    if (D_800B8508 != 0) {
        D_800B84A0(&p->tap_info[2]);
        D_800B84A0(&p->tap_info[3]);
    }
    ret = _padSioRW2(p, p->cmd_queued != 0 ? 0 : D_800B84D4);
    if (ret >= 0) {
        D_800B8500 = (ret & 0xF) * 2;
        if (D_800B8500 == 0) {
            D_800B8500 = 0x20;
        }
        ret = 0;
    }
    return ret;
}

/* The four void hooks the object's last two steps call, declared once for both. (Placed by a
   TU edit: the paste drops an attempt's `extern void (*x)(...)` line once the TU has one.) */
extern void (*D_800B8494)(PadInfo *);   /* _padFuncCurrLimit */
extern void (*D_800B84A8)(PadInfo *);   /* _padFuncRecvAuto */
extern void (*D_800B84AC)(PadInfo *);   /* _padFuncClrCmdNo */
extern void (*D_800B8488)(s32);         /* _padFuncNextPort */

/* libpad PADIF.OBJ +0x1CC, a static of the object (no SDK symbol): entry 3 of D_800B8510
   [padIntFunc]. 0xC4 bytes. */
/* cc97_lib: func_800A6D54 */
/* return_slot_fill: func_800A6D54 */
/* far_syms: D_800B850C */
/* far_syms: D_800B8494 */
/* far_syms: D_800B8490 */
extern u8 (*D_800B8490)(PadInfo *, s32);    /* _padFuncGetTxd */
extern s32 D_800B850C;                      /* PADIF.OBJ .data+0xC: a multi tap answered this transfer */

/* libpad PADIF.OBJ +0x1CC (SDK 4.5), a static with no SDK symbol: padIntFunc[3]
   (D_800B8510), step 3 of a controller transfer, named here from its role. Decides whether
   a multi tap is answering and records it in the object's static D_800B850C: 1 when
   _padModeMtap (D_800B84D4) is set, the ID byte received (recv_buf[0]) has high nibble 8
   and no command is queued (cmd_queued, +0x37); else 0. Without a multi tap it calls
   _padFuncCurrLimit (D_800B8494) for the record now (with one, step 4 does). Then it sends
   the byte _padFuncGetTxd (D_800B8490) gives for the record through _padSioRW2
   , which returns the byte received in answer to step 2's. Returns that
   byte when it is 0x5A or 0, a negative error unchanged, and -4 for any other byte (0x5A is
   what a controller answers at this point of the protocol; protocol knowledge, not the
   SDK's text). */
s32 func_800A6D54(PadInfo *p)
{
    s32 mtap;
    s32 ret;

    mtap = 0;
    if (D_800B84D4 != 0 && (p->recv_buf[0] >> 4) == 8) {
        mtap = p->cmd_queued == 0;
    }
    D_800B850C = mtap;
    if (mtap == 0) {
        D_800B8494(p);
    }
    ret = _padSioRW2(p, D_800B8490(p, D_800B850C));
    if (ret == 0x5A || ret == 0 || ret < 0) {
        return ret;
    }
    return -4;
}

/* libpad PADIF.OBJ +0x290, a static of the object (no SDK symbol): entry 4 of D_800B8510
   [padIntFunc]. 0x394 bytes. Followed by 3 zero words, the object's .text padded to 8 bytes
   (the rows after its endlabel). */
/* cc97_lib: func_800A6E18 */
/* return_slot_fill: func_800A6E18 */
/* far_syms: D_800B84BC */
/* far_syms: D_800B84A8 */
/* far_syms: D_800B84AC */
/* far_syms: D_800B8504 */
/* far_syms: D_800B8488 */
extern s32 D_800B84E0[];                    /* _padFixResult: one word per port */
extern PadInfo *D_800B84BC;                 /* _padInfoDir: the two ports' records (0xF0 bytes each) */
extern volatile PadSio *D_800B8504;         /* PADIF.OBJ .data+4 = 0x1F801040, the SIO0 (controller port) registers */
void setRC2wait(u32 wait);
s32 _padClrIntSio0(void);
void _padWaitRXready(void);

/* libpad PADIF.OBJ +0x290 (SDK 4.5), a static with no SDK symbol: padIntFunc[4]
   (D_800B8510), step 4 of a controller transfer, named here from its role: the payload.
   It moves every remaining byte itself, polling instead of returning to the interrupt:
   after each byte setRC2wait(0x3C) and _padClrIntSio0,
   whose 0 (no acknowledge in time) ends the transfer with -3; a negative result of
   _padSioRW / _padSioRW2 (_padSioRW / _padSioRW2) is returned as it is. The byte
   to send is always _padFuncGetTxd's (D_800B8490). D_800B8500 counts the bytes left
   (step 2 set it).
   1. With a multi tap (D_800B850C, step 3's verdict): _padFuncCurrLimit (D_800B8494) for
      the record, then up to five bytes through _padSioRW2 with GetTxd(p, 1), calling
      _padFuncCurrLimit for tap_info[0..3] before the second to fifth.
   2. While more than one byte is left: the pending result word of the OTHER port
      (_padFixResult D_800B84E0[c], c = 1 when _padSioChan D_800B84C8 is 0, else 0) is
      worked off one step per byte. Negative ends this loop. Positive n: _padFuncRecvAuto
      (D_800B84A8) for record n - 1 behind that port (_padInfoDir[c].tap_info). Then 4
      becomes 3; 3 calls _padFuncRecvAuto for the record before that one and becomes 1;
      0 or 1 calls _padFuncRecvAuto and _padFuncClrCmdNo (D_800B84AC) for the port's own
      record and becomes -1; 2 stays. The byte goes through _padSioRW with
      GetTxd(p, mtap).
   3. The rest of the bytes through _padSioRW the same way; the acknowledge wait is
      skipped when the port's baud register (SIO0 +0xE) reads 0x22.
   4. _padWaitRXready, the last received byte is appended to recv_buf
      (recv_len counts), and _padFuncNextPort (D_800B8488) is called with 0. Returns 0.
   Why the other port's word is the one served in 2 is not established. */
s32 func_800A6E18(PadInfo *p)
{
    s32 i;
    s32 ret;
    s32 mtap;
    s32 chan;
    PadInfo *q;

    if (D_800B850C != 0) {
        D_800B8494(p);
    }
    mtap = D_800B850C;
    if (mtap != 0) {
        for (i = -1; i < 4; i++) {
            if (--D_800B8500 <= 0) {
                break;
            }
            if (i >= 0) {
                D_800B8494(&p->tap_info[i]);
            }
            ret = _padSioRW2(p, D_800B8490(p, 1));
            if (ret < 0) {
                return ret;
            }
            setRC2wait(0x3C);
            if (_padClrIntSio0() == 0) {
                return -3;
            }
        }
    }
    q = 0;
    chan = D_800B84C8 == 0;
    for (; D_800B8500 > 1; D_800B8500--) {
        if (D_800B84E0[chan] < 0) {
            break;
        }
        if (D_800B84E0[chan] > 0) {
            q = D_800B84BC[chan].tap_info + D_800B84E0[chan] - 1;
            D_800B84A8(q);
        }
        switch (D_800B84E0[chan]) {
        case 4:
            D_800B84E0[chan] = 3;
            break;
        case 3:
            D_800B84A8(q - 1);
            D_800B84E0[chan] = 1;
            break;
        case 0:
        case 1:
            q = &D_800B84BC[chan];
            D_800B84A8(q);
            D_800B84AC(q);
            D_800B84E0[chan] = -1;
            break;
        }
        ret = _padSioRW(p, D_800B8490(p, mtap));
        if (ret < 0) {
            return ret;
        }
        setRC2wait(0x3C);
        if (_padClrIntSio0() == 0) {
            return -3;
        }
    }
    while (--D_800B8500 > 0) {
        ret = _padSioRW(p, D_800B8490(p, mtap));
        if (ret < 0) {
            return ret;
        }
        if (D_800B8504->baud != 0x22) {
            setRC2wait(0x3C);
            if (_padClrIntSio0() == 0) {
                return -3;
            }
        }
    }
    _padWaitRXready();
    p->recv_buf[p->recv_len++] = D_800B8504->data;
    D_800B8488(0);
    return 0;
}
INCLUDE_PAD(12);
