/* PSY-Q 4.5 libpad, member padseqd.o: _padInitDirSeq, func_800A784C,
   func_800A795C, _dirFailAuto, func_800A7CA0. The first split had it in
   psyq_misc_aar.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* Sony libpad, PADSEQD.OBJ whole (0x800A7818..0x800A7CD7 of the image). Every word equals the
   on-disk SDK 4.4 object's, relocation fields apart; the release linked is SDK 4.5's, which is
   the same code (the two LIBPAD.LIB differ only in member dates and in the version word of
   PADMAIN.OBJ's .data, and the image holds 4.5's, 0x00450000 at 0x800B8484). These functions
   were `.word` rows of asm/data/data_tables.data.s: the first split cut text from data at
   0x800A5000, and the image's text runs to 0x800A7D98. No config/cc_flags.txt entry: the
   object's converted functions take per-function markers (cc97_lib, and return_slot_fill by the
   epilogue rule). */

/* libpad _padInitDirSeq (PADSEQD.OBJ +0x000), 0x34 bytes. */
/* cc97_lib: _padInitDirSeq */
/* return_slot_fill: _padInitDirSeq */
/* far_syms: D_800B84A0 */
/* far_syms: D_800B84A4 */
/* far_syms: D_800B84A8 */
/* far_syms: D_800B848C */
extern s32 (*D_800B84A0)(PadInfo *), (*D_800B84A4)(PadInfo *);
extern void (*D_800B84A8)(PadInfo *), (*D_800B848C)(PadInfo *);
s32 func_800A784C(PadInfo *p);
void func_800A795C(PadInfo *p);
s32 func_800A7CA0(PadInfo *p);

/* libpad _padInitDirSeq (PADSEQD.OBJ, SDK 4.5): installs the direct ports' sequencer.
   Points three of libpad's hooks at this object's statics: _padFuncSendAuto
   (D_800B84A0) at func_800A784C, _padFuncChkEng (D_800B84A4) at func_800A7CA0 and
   _padFuncRecvAuto (D_800B84A8) at func_800A795C. Caller: PadInitDirect
   . The declarations above it are the object's: the hooks (the three
   stored here and _padFuncClrInfo, D_800B848C, which the functions below call) and
   the port record, libpad_padentry_1.c's PadInfo with the members this object touches
   named in place of padding (send_func, recv_func, rx_len, recv_buf, recv_len, seq,
   unk47, retry, unk50; the names are the tree's, inferred from use). */
/* func_800A7818 */
void _padInitDirSeq(void)
{
    D_800B84A0 = func_800A784C;
    D_800B84A4 = func_800A7CA0;
    D_800B84A8 = func_800A795C;
}

/* libpad PADSEQD.OBJ +0x034, a static of the object (no SDK symbol): its address is stored in
   D_800B84A0 [_padFuncSendAuto]. 0x110 bytes. */
/* cc97_lib: func_800A784C */
/* return_slot_fill: func_800A784C */
void _padCmdParaMode(PadInfo *p, s32 mode);
void _padSendAtLoadInfo(PadInfo *p);

/* libpad PADSEQD.OBJ +0x034 (SDK 4.5), the function _padInitDirSeq installs as
   _padFuncSendAuto (D_800B84A0); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: what a direct port sends next, by the record's sequence
   step (seq, +0x46). When the last reply's first byte (recv_buf, +0x3C) is 0xF3: with
   no current ID (cur_id, +0xE8) or with the sequence finished (seq 0xFF) it queues
   _padCmdParaMode(p, 0) (command 0x43, one parameter byte) and is
   done; otherwise a record in state 2 (PadStateFindCTP1) is cleared through
   _padFuncClrInfo (D_800B848C). Then by step: 0 and 0xFF send nothing, 1 queues
   _padCmdParaMode(p, 1), 0xFE queues _padCmdParaMode(p, 0), and every other step runs
   the queued command's send step (send_func, +0x14) or, when there is none,
   _padSendAtLoadInfo. Always returns 0. */
s32 func_800A784C(PadInfo *p)
{
    if (*p->recv_buf == 0xF3) {
        if (p->cur_id == 0) {
            _padCmdParaMode(p, 0);
            return 0;
        }
        if (p->seq == 0xFF) {
            _padCmdParaMode(p, 0);
            return 0;
        }
        if (p->state == 2) {
            D_800B848C(p);
        }
    }
    switch (p->seq) {
    case 0:
        break;
    case 1:
        _padCmdParaMode(p, 1);
        break;
    case 0xFE:
        _padCmdParaMode(p, 0);
        break;
    case 0xFF:
        break;
    default:
        if (p->send_func != 0) {
            p->send_func(p);
        } else {
            _padSendAtLoadInfo(p);
        }
        break;
    }
    return 0;
}

/* libpad PADSEQD.OBJ +0x144, a static of the object (no SDK symbol): its address is stored in
   D_800B84A8 [_padFuncRecvAuto]. 0x264 bytes. */
/* cc97_lib: func_800A795C */
/* return_slot_fill: func_800A795C */
s32 _padRecvAtLoadInfo(PadInfo *p);

/* libpad PADSEQD.OBJ +0x144 (SDK 4.5), the function _padInitDirSeq installs as
   _padFuncRecvAuto (D_800B84A8); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: takes in a direct port's reply (recv_buf, +0x3C;
   recv_len bytes, +0x44) and advances the sequence step (seq, +0x46).
   A first byte with a zero high nibble is no controller: the caller's buffer (rx_buf,
   +0x30) gets 0xFF, 0, the current ID (cur_id, +0xE8) and rx_len (+0x35) are zeroed
   and the record is cleared through _padFuncClrInfo (D_800B848C). Otherwise the high
   nibble becomes the current ID, except 0xF, which keeps the old one; for any other ID
   the reply is handed to the caller: rx_buf[0] = 0, rx_buf[1] = the first byte,
   rx_len = recv_len and bytes 2 .. recv_len - 1 copied. The record is cleared when the
   ID changed, or when the reply's second byte is 0 while unk50 (+0x50) is 0 and the
   step is not 1 with no send step set (send_func, +0x14). The failure count (retry,
   +0x4A) is zeroed. Then nothing more happens at step 0xFF, at a non-zero step with no
   command queued (cmd_queued, +0x37), and at steps 2 .. 0xFD when the first byte is
   not 0xF3 (there the record is cleared first). What is left steps the sequence:
   0 sets state 1 (PadStateFindPad) and goes to 1; 1 zeroes unk47 (+0x47) and goes to
   2; 0xFE goes to 0xFF; every other step adds the result of the queued command's
   receive step (recv_func, +0x18) or, when there is none, of _padRecvAtLoadInfo
   . */
void func_800A795C(PadInfo *p)
{
    u8 old_id;
    s32 i;

    if ((*p->recv_buf & 0xF0) == 0) {
        p->rx_buf[0] = 0xFF;
        p->rx_buf[1] = 0;
        p->cur_id = 0;
        p->rx_len = 0;
        D_800B848C(p);
        return;
    }
    old_id = p->cur_id;
    p->cur_id = *p->recv_buf >> 4;
    if (p->cur_id == 0xF) {
        p->cur_id = old_id;
    } else {
        p->rx_buf[0] = 0;
        p->rx_buf[1] = *p->recv_buf;
        p->rx_len = p->recv_len;
        for (i = 2; i < p->recv_len; i++) {
            p->rx_buf[i] = p->recv_buf[i];
        }
    }
    if ((p->recv_buf[1] == 0 && (p->seq != 1 || p->send_func != 0) && p->unk_50 == 0) || p->cur_id != old_id) {
        D_800B848C(p);
    }
    p->retry = 0;
    if (p->seq == 0xFF) {
        return;
    }
    if (p->seq != 0 && p->cmd_queued == 0) {
        return;
    }
    if (p->seq >= 2 && p->seq < 0xFE && *p->recv_buf != 0xF3) {
        D_800B848C(p);
        return;
    }
    switch (p->seq) {
    case 0:
        p->state = 1;
        p->seq++;
        break;
    case 1:
        p->index = 0;
        p->seq++;
        break;
    case 0xFE:
        p->seq = 0xFF;
        break;
    default:
        if (p->recv_func != 0) {
            p->seq += p->recv_func(p);
        } else {
            p->seq += _padRecvAtLoadInfo(p);
        }
        break;
    }
}

/* libpad _dirFailAuto (PADSEQD.OBJ +0x3A8), 0xE0 bytes. */
/* cc97_lib: _dirFailAuto */
/* return_slot_fill: _dirFailAuto */

/* libpad _dirFailAuto (PADSEQD.OBJ, SDK 4.5): a transfer on a direct port failed.
   Counts it in the record's +0x4C word (the one PadInfoMode term 100 returns), then
   by the sequence step (seq, +0x46): at step 1 the first eleven failures in a row
   (retry, +0x4A) are only counted and the twelfth sets state 2 (PadStateFindCTP1)
   and ends the sequence (seq 0xFF); at any later step the first eleven are only
   counted too, and the twelfth clears a record whose state is not 0 through
   _padFuncClrInfo (D_800B848C). That twelfth failure, and every failure at step 0,
   reports no controller unless the last reply's first byte (recv_buf, +0x3C) is
   0xF3: the caller's buffer (rx_buf, +0x30) gets 0xFF, 0 and the current ID (cur_id,
   +0xE8) and rx_len (+0x35) are zeroed. Caller: func_800A7380, PADPORTD.OBJ's
   next-port hook, which passes its own transfer result as a second argument; the
   body never reads it (the parameter is the caller's evidence: func_800A7380 keeps
   the value in $a1 up to this call). */
/* func_800A7BC0 */
void _dirFailAuto(PadInfo *p, s32 result)
{
    p->fail_count++;
    switch (p->seq) {
    case 0:
        break;
    case 1:
        if (p->retry < 11) {
            p->retry++;
        } else {
            p->state = 2;
            p->seq = 0xFF;
        }
        return;
    default:
        if (p->retry < 11) {
            p->retry++;
            return;
        }
        if (p->state != 0) {
            D_800B848C(p);
        }
        break;
    }
    if (*p->recv_buf != 0xF3) {
        p->rx_buf[0] = 0xFF;
        p->rx_buf[1] = 0;
        p->cur_id = 0;
        p->rx_len = 0;
    }
}

/* libpad PADSEQD.OBJ +0x488, a static of the object (no SDK symbol): its address is stored in
   D_800B84A4 [_padFuncChkEng]. 0x2C bytes. Followed by 3 zero words, the object's .text padded
   to 8 bytes (the rows after its endlabel). */
/* cc97_lib: func_800A7CA0 */

/* libpad PADSEQD.OBJ +0x488 (SDK 4.5), the function _padInitDirSeq installs as
   _padFuncChkEng (D_800B84A4); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: 0 when the record has an extended ID (cur_ex_id,
   +0xE6) and its sequence is finished (seq, +0x46, is 0xFF), else 1. PADCMD.OBJ's
   _padSetActAlign and _padSetMainMode return 0 without queueing on a non-zero result,
   and _padLoadActInfo tests it too. */
s32 func_800A7CA0(PadInfo *p)
{
    if (p->cur_ex_id == 0 || p->seq != 0xFF) {
        return 1;
    }
    return 0;
}
INCLUDE_PAD(12);
