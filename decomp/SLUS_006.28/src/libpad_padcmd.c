/* PSY-Q 4.5 libpad, member padcmd.o: _padSetAct, _padSetCmd,
   _padSendAtLoadInfo, _padRecvAtLoadInfo, _padGetActSize, _padLoadActInfo,
   func_800A63A4, func_800A644C, _padSetActAlign, func_800A68AC,
   func_800A68C8, _padSetMainMode, func_800A6A28, func_800A6A7C,
   _padCmdParaMode, func_800A6AFC, func_800A6B10, func_800A6B30,
   func_800A6B50, func_800A6B70. The first split had it in psyq_misc_aao.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* Sony libpad, PADCMD.OBJ whole (0x800A5F88..0x800A6B87 of the image). Every word equals the
   on-disk SDK 4.4 object's, relocation fields apart; the release linked is SDK 4.5's, which is
   the same code (the two LIBPAD.LIB differ only in member dates and in the version word of
   PADMAIN.OBJ's .data, and the image holds 4.5's, 0x00450000 at 0x800B8484). These functions
   were `.word` rows of asm/data/data_tables.data.s: the first split cut text from data at
   0x800A5000, and the image's text runs to 0x800A7D98. No config/cc_flags.txt entry: the
   object's converted functions take per-function markers (cc97_lib, and return_slot_fill by the
   epilogue rule). */

/* The port record as PADCMD.OBJ's twenty functions read and write it (0xF0 bytes; libpad has
   no public type for it). libpad_padentry_1.c's typedef names the members PADENTRY.OBJ touches;
   this one adds PADCMD.OBJ's: the two hooks of a command sequence (send queues the command of
   the current step; recv judges the reply and returns what is added to step: 1 = next step,
   0 = the same step again), the command a send hook queues (cmd_queued, its parameter bytes
   cmd_data / cmd_len, and cmd_param, a one-byte parameter buffer), the reply bytes (reply),
   the sequence's step, index and count, what PadSetAct, PadSetActAlign and PadSetMainMode
   left for it (act_data / act_len, align_data, mode_offs / mode_lock / mode_same), the
   alignment sent to the controller (act_align) and the buffer the ID, actuator and
   combination tables are built in (info_buf; _padRecvAtLoadInfo hands it to _padLoadActInfo,
   which points id_table / act_table / comb_table into it). unkEE is what the previous reply
   gave for the item being read (the ID itself, or 0 / 0xFFFF for same / differs): an item is
   taken when two replies in a row agree. PADCMD.OBJ only ever clears unkEB, each time an
   item is taken. Member names are the tree's, not Sony's. The two hooks declared below are
   PADMAIN.OBJ .data, reached %hi/%lo (far_syms). */
/* far_syms: D_800B848C */
/* far_syms: D_800B84A4 */
extern void (*D_800B848C)(PadInfo *);  /* _padFuncClrInfo */
extern s32 (*D_800B84A4)(PadInfo *);   /* _padFuncChkEng: non-zero = the caller queues nothing */

/* libpad _padSetAct (PADCMD.OBJ +0x000), 0xC bytes. */
/* cc97_lib: _padSetAct */
/* return_slot_fill: _padSetAct */

/* libpad _padSetAct (PADCMD.OBJ, SDK 4.5): keeps PadSetAct's actuator data for a port,
   the caller's pointer (act_data, +0x28) and its byte count (act_len, +0x34). Nothing is
   copied here. Caller: PadSetAct (PADENTRY.OBJ). */
/* func_800A5F88 */
void _padSetAct(PadInfo *p, u8 *data, s32 len)
{
    p->act_data = data;
    p->act_len = len;
}

/* libpad _padSetCmd (PADCMD.OBJ +0x00C), 0x10 bytes. */
/* cc97_lib: _padSetCmd */
/* return_slot_fill: _padSetCmd */

/* libpad _padSetCmd (PADCMD.OBJ, SDK 4.5): queues a command for a port: the command byte
   (cmd_queued, +0x37), the pointer to its parameter bytes (cmd_data, +0x2C) and their
   count (cmd_len, +0x36). Nothing in the image calls it: the object's five statics that
   queue commands 0x45, 0x46, 0x47, 0x4B and 0x4C (func_800A6AFC .. func_800A6B70) write
   the same three members themselves. With no caller, the parameter types are read from
   the stores alone (two bytes and a word). */
/* func_800A5F94 */
void _padSetCmd(PadInfo *p, s32 cmd, u8 *data, s32 len)
{
    p->cmd_queued = cmd;
    p->cmd_data = data;
    p->cmd_len = len;
}

/* libpad _padSendAtLoadInfo (PADCMD.OBJ +0x01C), 0x84 bytes. */
/* cc97_lib: _padSendAtLoadInfo */
void func_800A6AFC(PadInfo *p);  /* TU defines this below */
void func_800A6B10(PadInfo *p, s32 offs);  /* TU defines this below */
void func_800A6B50(PadInfo *p, s32 combno);  /* TU defines this below */

/* libpad _padSendAtLoadInfo (PADCMD.OBJ, SDK 4.5): the send half of the sequence that
   reads a newly found controller's counts: queues the command of the current step.
   Step 2: command 0x45, no parameter (func_800A6AFC). Step 3: command 0x4C with the
   current ID offset (cur_ex_offs, +0xE4; func_800A6B10). Step 4: command 0x47 with the
   combination number (index, +0x47; func_800A6B50). Any other step queues nothing.
   _padRecvAtLoadInfo reads the replies. Caller: PADSEQD.OBJ's
   _padFuncSendAuto target (func_800A784C), when the record has no send hook. */
/* func_800A5FA4 */
void _padSendAtLoadInfo(PadInfo *p)
{
    switch (p->seq) {
    case 2:
        func_800A6AFC(p);
        break;
    case 3:
        func_800A6B10(p, p->cur_ex_offs);
        break;
    case 4:
        func_800A6B50(p, p->index);
        break;
    }
}

/* libpad _padRecvAtLoadInfo (PADCMD.OBJ +0x0A0), 0x270 bytes. */
/* cc97_lib: _padRecvAtLoadInfo */
/* return_slot_fill: _padRecvAtLoadInfo */
s32 _padGetActSize(PadInfo *p);  /* TU defines this below */
s32 _padLoadActInfo(PadInfo *p, u8 *buf);  /* TU defines this below */

/* libpad _padRecvAtLoadInfo (PADCMD.OBJ, SDK 4.5): the receive half of the sequence that
   reads a newly found controller's counts: judges the reply to the command
   _padSendAtLoadInfo queued for the current step. The result is added to
   the step by the caller: 1 = this step is done, 0 = the same step again. Every item is
   taken only when two replies in a row agree; unkEE (+0xEE) carries what the reply
   before gave.
   Step 2 (command 0x45): ignored while reply[7] is non-zero. reply[3..6] are the ID
   count, the current ID offset, the actuator count and the combination count; they are
   stored (id_num, cur_ex_offs, act_num, comb_num; cur_ex_id and comb_size cleared), and
   the step is done when all four already equalled the record's.
   Step 3 (command 0x4C): ignored while reply[2] or reply[3] is non-zero. cur_ex_id is
   (reply[4] << 8) + reply[5]; done when it equals the reply before, and then index
   (+0x47) is cleared for step 4.
   Step 4 (command 0x47, once per combination): ignored while reply[2] or reply[3] is
   non-zero. Adds what the combination needs to comb_size: one PadCombInfo record and its
   list of reply[4] bytes rounded up to 4. After the last combination: if the three
   tables need more than 0x80 bytes (_padGetActSize; info_buf's size) the
   controller is given up (_padFuncClrInfo, D_800B848C; step 0xFE, state 2
   PadStateFindCTP1); else if the total is not the one of the pass before, the pass is
   taken again from combination 0; else the tables are laid out and their sequence
   started (_padLoadActInfo, called with step 0xFF) and the step is set
   to 2. This step always returns 0.
   Any other step returns 1. Caller: PADSEQD.OBJ's _padFuncRecvAuto target
   (func_800A795C), when the record has no receive hook.
   Three shapes decide bytes here. Step 3's two reply tests are one `||` condition (as
   two `if`s the shared `return 0` code lands at the first of them: 140/156). The 8 added
   per combination is unsigned, written as the record's size (`8U` gives the same bytes;
   a signed literal 8 is 144/156: the compiler then adds it to the rounded count, the
   target to comb_size). In step 4 the "pass again" arm returns by itself while the other
   two arms run on to the step's last `return 0` (the give-up arm with a return of its
   own: 136/156; the pass-again arm without one: 143/156). */
/* func_800A6028 */
s32 _padRecvAtLoadInfo(PadInfo *p)
{
    switch (p->seq) {
    case 2:
        if (p->recv_buf[7] != 0) {
            return 0;
        }
        if (p->id_num == p->recv_buf[3] && p->cur_ex_offs == p->recv_buf[4] && p->act_num == p->recv_buf[5] && p->comb_num == p->recv_buf[6]) {
            p->prev_reply = 0;
        } else {
            p->prev_reply = 0xFFFF;
        }
        p->id_num = p->recv_buf[3];
        p->cur_ex_offs = p->recv_buf[4];
        p->cur_ex_id = 0;
        p->act_num = p->recv_buf[5];
        p->comb_num = p->recv_buf[6];
        p->comb_size = 0;
        if (p->prev_reply != 0) {
            return 0;
        }
        p->unk_eb = 0;
        break;
    case 3:
        if (p->recv_buf[2] != 0 || p->recv_buf[3] != 0) {
            return 0;
        }
        p->cur_ex_id = (p->recv_buf[4] << 8) + p->recv_buf[5];
        if (p->prev_reply != p->cur_ex_id) {
            p->prev_reply = p->cur_ex_id;
            return 0;
        }
        p->prev_reply = 0xFFFF;
        p->unk_eb = 0;
        p->index = 0;
        break;
    case 4:
        if (p->recv_buf[2] != 0 || p->recv_buf[3] != 0) {
            return 0;
        }
        p->comb_size += sizeof(PadCombInfo) + ((p->recv_buf[4] + 3) & ~3);
        p->index++;
        if (p->index < p->comb_num) {
            return 0;
        }
        if (_padGetActSize(p) > 0x80) {
            D_800B848C(p);
            p->seq = 0xFE;
            p->state = 2;
        } else {
            if (p->prev_reply != p->comb_size) {
                p->prev_reply = p->comb_size;
                p->index = 0;
                p->comb_size = 0;
                return 0;
            }
            p->prev_reply = 0;
            p->unk_eb = 0;
            p->seq = 0xFF;
            _padLoadActInfo(p, p->info_buf);
            p->seq = 2;
        }
        return 0;
    }
    return 1;
}

/* libpad _padGetActSize (PADCMD.OBJ +0x310), 0x38 bytes. */
/* cc97_lib: _padGetActSize */
/* return_slot_fill: _padGetActSize */

/* libpad _padGetActSize (PADCMD.OBJ, SDK 4.5): the bytes the controller's three tables
   need in the record's buffer: 4, the ID table (id_num 16-bit entries rounded up to
   whole words), the actuator table (act_num 5-byte records rounded up to whole words)
   and the combination records with their lists (comb_size, +0xEC, summed by
   _padRecvAtLoadInfo). The 4 covers the alignment of the buffer's start (inferred: the
   buffer is info_buf, 0x80 bytes at the odd offset +0x63, and _padLoadActInfo rounds its
   address up to 4). Caller: _padRecvAtLoadInfo, which gives the
   controller up when the result is above 0x80. The 4 is written first: the target adds
   it to the actuator term, which is where the compiler moves a leading constant
   (written last 12/14, between the actuator term and comb_size 3/14).
   sizeof(PadActInfo) is the 5; a literal gives the same bytes. */
/* func_800A6298 */
s32 _padGetActSize(PadInfo *p)
{
    return 4 + ((p->id_num + 1) >> 1) * 4 + ((p->act_num * sizeof(PadActInfo) + 3) >> 2) * 4 + p->comb_size;
}

/* libpad _padLoadActInfo (PADCMD.OBJ +0x348), 0xD4 bytes. */
/* cc97_lib: _padLoadActInfo */
/* return_slot_fill: _padLoadActInfo */
void func_800A63A4(PadInfo *p);  /* TU defines this below */
s32 func_800A644C(PadInfo *p);  /* TU defines this below */

/* libpad _padLoadActInfo (PADCMD.OBJ, SDK 4.5): starts the command sequence that fills
   the controller's ID, actuator and combination tables, and lays the three tables out in
   `buf`. Returns 0 and changes nothing when buf is null, when the record already has an
   actuator table, or when _padFuncChkEng (D_800B84A4) says no. Otherwise: state 4
   (PadStateReqInfo), step 1, the object's two statics as the sequence's hooks (send
   func_800A63A4, receive func_800A644C), index 0; id_table is buf rounded up to 4 bytes,
   act_table follows id_num 16-bit entries rounded up to whole words, comb_table follows
   act_num 5-byte records rounded up to whole words; returns 1. Caller:
   _padRecvAtLoadInfo, with the record's own info_buf.
   The two id_table statements stand directly after `p->state = 4`, and each table's
   address is its own variable: that is what puts the rounding's add and shift before the
   state store and its second shift at the id_table store (one statement lower 51/53, one
   higher 52/53; the address in one variable that is added to twice 49/53; read back
   through p 38/53; rounded with `& ~3` 49/53). sizeof(PadActInfo) is the 5; a literal
   gives the same bytes. */
/* func_800A62D0 */
s32 _padLoadActInfo(PadInfo *p, u8 *buf)
{
    u16 *id;
    PadActInfo *act;

    if (buf == 0 || p->act_table != 0 || D_800B84A4(p) != 0) {
        return 0;
    }
    p->state = 4;
    id = (u16 *)((((s32)buf + 3) >> 2) << 2);
    p->id_table = id;
    p->seq = 1;
    p->send_func = func_800A63A4;
    p->recv_func = func_800A644C;
    p->index = 0;
    act = (PadActInfo *)((u32 *)id + ((p->id_num + 1) >> 1));
    p->act_table = act;
    p->comb_table = (PadCombInfo *)((u32 *)act + ((p->act_num * sizeof(PadActInfo) + 3) >> 2));
    return 1;
}

/* libpad PADCMD.OBJ +0x41C, a static of the object (no SDK symbol): its address is taken in
   _padLoadActInfo [_padLoadActInfo]. 0xA8 bytes. */
/* cc97_lib: func_800A63A4 */
void func_800A6B30(PadInfo *p, s32 actno);  /* TU defines this below */
void func_800A6B70(PadInfo *p);  /* TU defines this below */

/* libpad PADCMD.OBJ +0x41C (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by its role: the send hook _padLoadActInfo
   installs at +0x14. It queues the command of the current step of the sequence that
   fills the controller's three tables. Step 2: command 0x4C for ID table entry `index`
   (func_800A6B10). Step 3: command 0x46 for actuator `index` (func_800A6B30). Step 4:
   command 0x47 for combination `index` (func_800A6B50) while no bytes of its list are
   owed (count, +0x48, zero), else command 0x4B for the next bytes of the list
   (func_800A6B70). Any other step queues nothing. func_800A644C, the receive hook,
   reads the replies. Not static here: _padLoadActInfo names it by address. */
void func_800A63A4(PadInfo *p)
{
    switch (p->seq) {
    case 2:
        func_800A6B10(p, p->index);
        break;
    case 3:
        func_800A6B30(p, p->index);
        break;
    case 4:
        if (p->count == 0) {
            func_800A6B50(p, p->index);
        } else {
            func_800A6B70(p);
        }
        break;
    }
}

/* libpad PADCMD.OBJ +0x4C4, a static of the object (no SDK symbol): its address is taken in
   _padLoadActInfo [_padLoadActInfo]. 0x3F8 bytes. */
/* cc97_lib: func_800A644C */
/* lib_g0: func_800A644C */
extern u8 *D_800EAD60;

/* libpad PADCMD.OBJ +0x4C4 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by its role: the receive hook _padLoadActInfo
   installs at +0x18. It takes in the replies to the commands func_800A63A4 sends and
   fills the controller's three tables; its result is added to the sequence step, so 1
   means this step's table is complete and 0 asks the same step again. Every item is
   read until two replies in a row agree: unkEE (+0xEE) holds what the reply before
   gave (the ID itself at step 2; 0 for "same as the table already holds", 0xFFFF for
   "differs" at steps 3 and 4). A reply whose byte 2 or 3 is non-zero is ignored.
   Step 2 (command 0x4C): entry `index` of the ID table is (reply[4] << 8) + reply[5];
   after the last of id_num entries, index 0 and result 1.
   Step 3 (command 0x46): actuator `index` is func reply[4], sub reply[5] & 0x7F, size
   reply[6], curr reply[7], sign reply[5] bit 7; after the last of act_num actuators,
   index and count 0 and result 1.
   Step 4 (commands 0x47 and 0x4B): combination `index`. With no bytes owed (count
   +0x48 zero) the reply opens the combination: reply[4] is the length of its list,
   kept in count and in the record's num; its list starts behind the last combination
   record for the first one and behind the previous list, rounded up to 4 bytes, for
   the others, and three list bytes follow at reply[5]. With bytes owed the reply
   carries five more at reply[3]. D_800EAD60 (the object's only .bss word) is the
   write position in the list; a list that would pass the end of the record's buffer
   (info_buf, +0x63, 0x80 bytes) restarts the step from combination 0. When the list
   is complete and agreed, the next combination; after the last of comb_num the
   record's state becomes 6 (PadStateStable) and the sequence ends (step 0xFE). This
   step always returns 0.
   `n` is a plain int: through the s32 typedef the compiler does not turn `n-- != 0`
   into the decrement-then-compare-with--1 the target has.
   MARKED SPELLING: the two sign extractions go through the int local `sub_sign`
   (`(sub_sign = p->reply[5]) >> 7`, and the two statements before the unkEE test).
   The target shifts the byte arithmetically (`lbu` / `sra 7`), which needs an int
   operand: written on the u8 itself (`p->reply[5] >> 7`, `/ 128`) the compiler narrows
   the operation to the unsigned byte and emits `srl` (252/254 with `/ 128`, the two
   `sra` rows; 227/254 with `>> 7`, which also shares the load with the `& 0x7F`
   above it). What the source's int operand was is not known; PADMAIN.OBJ's
   _padSioRW has the same `lbu` / `sra` on a reply byte. */
s32 func_800A644C(PadInfo *p)
{
    PadActInfo *act;
    PadCombInfo *comb;
    u8 *src;
    s32 sub_sign;
    int n;

    switch (p->seq) {
    case 2:
        if (p->recv_buf[2] != 0 || p->recv_buf[3] != 0) {
            return 0;
        }
        p->id_table[p->index] = (p->recv_buf[4] << 8) + p->recv_buf[5];
        if (p->prev_reply != p->id_table[p->index]) {
            p->prev_reply = p->id_table[p->index];
            return 0;
        }
        p->prev_reply = 0;
        p->unk_eb = 0;
        if (++p->index < p->id_num) {
            return 0;
        }
        p->index = 0;
        break;
    case 3:
        if (p->recv_buf[2] != 0 || p->recv_buf[3] != 0) {
            return 0;
        }
        act = &p->act_table[p->index];
        if (act->func == p->recv_buf[4] && act->sub == (p->recv_buf[5] & 0x7F) && act->size == p->recv_buf[6]
            && act->curr == p->recv_buf[7] && act->sign == (sub_sign = p->recv_buf[5]) >> 7) {
            p->prev_reply = 0;
        } else {
            p->prev_reply = 0xFFFF;
        }
        act->func = p->recv_buf[4];
        act->sub = p->recv_buf[5] & 0x7F;
        act->size = p->recv_buf[6];
        act->curr = p->recv_buf[7];
        sub_sign = p->recv_buf[5];
        act->sign = sub_sign >> 7;
        if (p->prev_reply != 0) {
            return 0;
        }
        p->unk_eb = 0;
        if (++p->index < p->act_num) {
            return 0;
        }
        p->index = 0;
        p->count = 0;
        break;
    case 4:
        if (p->recv_buf[2] != 0) {
            p->count = 0;
            return 0;
        }
        comb = &p->comb_table[p->index];
        if (p->count == 0) {
            p->count = p->recv_buf[4];
            comb->num = p->count;
            src = p->recv_buf + 5;
            n = 3;
            if (p->index == 0) {
                D_800EAD60 = comb->list = (u8 *)&p->comb_table[p->comb_num];
            } else {
                D_800EAD60 = comb->list = comb[-1].list + ((comb[-1].num + 3) & ~3);
            }
        } else {
            n = 5;
            src = p->recv_buf + 3;
        }
        while (n-- != 0) {
            if (p->count == 0) {
                break;
            }
            if (D_800EAD60 >= p->info_buf + sizeof(p->info_buf)) {
                p->index = 0;
                p->count = 0;
                return 0;
            }
            if (*D_800EAD60 != *src) {
                p->prev_reply = 0xFFFF;
            }
            *D_800EAD60++ = *src++;
            p->count--;
        }
        if (p->count != 0) {
            return 0;
        }
        if (p->prev_reply != 0) {
            p->prev_reply = 0;
            p->count = 0;
            return 0;
        }
        if (++p->index >= p->comb_num) {
            p->state = 6;
            p->seq = 0xFE;
            p->unk_eb = 0;
            return 0;
        }
        p->count = 0;
        p->unk_eb = 0;
        return 0;
    }
    return 1;
}

/* libpad _padSetActAlign (PADCMD.OBJ +0x8BC), 0x68 bytes. */
/* cc97_lib: _padSetActAlign */
/* return_slot_fill: _padSetActAlign */
void func_800A68AC(PadInfo *p);  /* TU defines this below */
s32 func_800A68C8(PadInfo *p);

/* libpad _padSetActAlign (PADCMD.OBJ, SDK 4.5): starts the command sequence that sends
   PadSetActAlign's six alignment bytes to the controller. Returns 0 and changes nothing
   when _padFuncChkEng (D_800B84A4) says no; otherwise sets the sequence step to 1,
   installs this object's two statics as the sequence's hooks (send func_800A68AC,
   receive func_800A68C8), keeps the caller's pointer (align_data, +0x20) and returns 1.
   Caller: PadSetActAlign. */
/* func_800A6844 */
s32 _padSetActAlign(PadInfo *p, u8 *data)
{
    if (D_800B84A4(p) != 0) {
        return 0;
    }
    p->seq = 1;
    p->send_func = func_800A68AC;
    p->align_data = data;
    p->recv_func = func_800A68C8;
    return 1;
}

/* libpad PADCMD.OBJ +0x924, a static of the object (no SDK symbol): its address is taken in
   _padSetActAlign [_padSetActAlign]. 0x1C bytes. */
/* cc97_lib: func_800A68AC */
/* return_slot_fill: func_800A68AC */

/* libpad PADCMD.OBJ +0x924 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by its role: the send hook _padSetActAlign
   installs at +0x14. Queues command 0x4D with the caller's six alignment bytes
   (align_data, +0x20) as its parameter. */
void func_800A68AC(PadInfo *p)
{
    p->cmd_queued = 0x4D;
    p->cmd_data = p->align_data;
    p->cmd_len = 6;
}

/* libpad PADCMD.OBJ +0x940, a static of the object (no SDK symbol): its address is taken in
   _padSetActAlign [_padSetActAlign]. 0xC8 bytes. */
/* cc97_lib: func_800A68C8 */

/* libpad PADCMD.OBJ +0x940 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by its role: the receive hook _padSetActAlign
   installs at +0x18. Builds the record's own copy of the alignment (act_align, +0x5D,
   six bytes) from the caller's (align_data, +0x20): for each actuator the controller
   has, it counts the positions that name it; where there are fewer of them than the
   actuator's data size (act_table[i].size, a size of 0 counting as 1) those positions
   become 0xFF, otherwise they keep the actuator's number. Positions naming no actuator
   of the controller are left as they were. Then ends the sequence (step 0xFE) and
   returns 0. */
s32 func_800A68C8(PadInfo *p)
{
    s32 i;
    s32 j;
    s32 n;
    s32 size;
    u8 *a;

    for (i = 0; i < p->act_num; i++) {
        a = p->align_data;
        n = 0;
        for (j = 0; j < 6; j++) {
            if (*a++ == i) {
                n++;
            }
        }
        size = p->act_table[i].size;
        a = p->align_data;
        if (size == 0) {
            size = 1;
        }
        for (j = 0; j < 6; j++) {
            if (*a++ == i) {
                if (n < size) {
                    p->act_align[j] = 0xFF;
                    n--;
                } else {
                    p->act_align[j] = i;
                }
            }
        }
    }
    p->seq = 0xFE;
    return 0;
}

/* libpad _padSetMainMode (PADCMD.OBJ +0xA08), 0x98 bytes. */
/* cc97_lib: _padSetMainMode */
/* return_slot_fill: _padSetMainMode */
void func_800A6A28(PadInfo *p);  /* TU defines this below */
s32 func_800A6A7C(PadInfo *p);  /* TU defines this below */

/* libpad _padSetMainMode (PADCMD.OBJ, SDK 4.5): starts the command sequence that
   switches the controller's main mode. Returns 0 and changes nothing when
   _padFuncChkEng (D_800B84A4) says no; otherwise sets the sequence step to 1, installs
   this object's two statics as the sequence's hooks (send func_800A6A28, receive
   func_800A6A7C), keeps the ID-table offset and the lock flag (mode_offs +0x51,
   mode_lock +0x52), notes whether that offset is already the current one (mode_same
   +0x53 = offs == cur_ex_offs) and returns 1. Caller: PadSetMainMode,
   which masks both arguments to a byte. */
/* func_800A6990 */
s32 _padSetMainMode(PadInfo *p, u8 offs, u8 lock)
{
    if (D_800B84A4(p) != 0) {
        return 0;
    }
    p->seq = 1;
    p->send_func = func_800A6A28;
    p->recv_func = func_800A6A7C;
    p->mode_offs = offs;
    p->mode_lock = lock;
    p->mode_same = offs == p->cur_ex_offs;
    return 1;
}

/* libpad PADCMD.OBJ +0xAA0, a static of the object (no SDK symbol): its address is taken in
   _padSetMainMode [_padSetMainMode]. 0x54 bytes. */
/* cc97_lib: func_800A6A28 */

/* libpad PADCMD.OBJ +0xAA0 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by its role: the send hook _padSetMainMode
   installs at +0x14. At step 2 it queues command 0x44 with two parameter bytes, the
   ID-table offset and the lock flag PadSetMainMode was given (mode_offs +0x51,
   mode_lock +0x52); at step 3 command 0x4D with the six alignment bytes the record
   keeps (act_align, +0x5D). Other steps queue nothing. */
void func_800A6A28(PadInfo *p)
{
    switch (p->seq) {
    case 2:
        p->cmd_queued = 0x44;
        p->cmd_data = &p->mode_offs;
        p->cmd_len = 2;
        break;
    case 3:
        p->cmd_queued = 0x4D;
        p->cmd_data = p->act_align;
        p->cmd_len = 6;
        break;
    }
}

/* libpad PADCMD.OBJ +0xAF4, a static of the object (no SDK symbol): its address is taken in
   _padSetMainMode [_padSetMainMode]. 0x60 bytes. */
/* cc97_lib: func_800A6A7C */

/* libpad PADCMD.OBJ +0xAF4 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by its role: the receive hook _padSetMainMode
   installs at +0x18; its result is added to the sequence step. When the mode asked for
   is the one the controller is in (mode_same, +0x53): 1 after step 2, so that step 3
   sends the alignment, and at any later step the sequence is ended (step 0xFE),
   result 0. When the mode changes, the record's state and information are cleared
   through _padFuncClrInfo (D_800B848C), result 0. */
s32 func_800A6A7C(PadInfo *p)
{
    if (p->mode_same != 0) {
        if (p->seq == 2) {
            return 1;
        }
        p->seq = 0xFE;
    } else {
        D_800B848C(p);
    }
    return 0;
}

/* libpad _padCmdParaMode (PADCMD.OBJ +0xB54), 0x20 bytes. */
/* cc97_lib: _padCmdParaMode */
/* return_slot_fill: _padCmdParaMode */

/* libpad _padCmdParaMode (PADCMD.OBJ, SDK 4.5): queues command 0x43 with the one
   parameter byte `mode`, kept in the record's parameter buffer (cmd_param, +0x24).
   Caller: PADSEQD.OBJ's send step func_800A784C, with mode 1 at step 1 of a sequence
   and mode 0 at its end (step 0xFE), and mode 0 again when a reply's first byte is 0xF3
   on a record with no current ID or with its sequence finished. */
/* func_800A6ADC */
void _padCmdParaMode(PadInfo *p, s32 mode)
{
    p->cmd_param = mode;
    p->cmd_queued = 0x43;
    p->cmd_data = &p->cmd_param;
    p->cmd_len = 1;
}

/* libpad PADCMD.OBJ +0xB74, a static of the object (no SDK symbol): called from _padSendAtLoadInfo
   [_padSendAtLoadInfo]. 0x14 bytes. */
/* cc97_lib: func_800A6AFC */
/* return_slot_fill: func_800A6AFC */

/* libpad PADCMD.OBJ +0xB74 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by what it does: queues command 0x45 with no parameter bytes
   (cmd_queued +0x37, cmd_data +0x2C, cmd_len +0x36). _padSendAtLoadInfo
   sends it at step 2; _padRecvAtLoadInfo reads the ID count, the current ID offset, the
   actuator count and the combination count from its reply. */
void func_800A6AFC(PadInfo *p)
{
    p->cmd_queued = 0x45;
    p->cmd_data = 0;
    p->cmd_len = 0;
}

/* libpad PADCMD.OBJ +0xB88, a static of the object (no SDK symbol): called from _padSendAtLoadInfo
   [_padSendAtLoadInfo], func_800A63A4. 0x20 bytes. */
/* cc97_lib: func_800A6B10 */
/* return_slot_fill: func_800A6B10 */

/* libpad PADCMD.OBJ +0xB88 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by what it does: queues command 0x4C with the one parameter
   byte `offs`, an offset into the controller's ID table. _padSendAtLoadInfo
    sends it at step 3 with the current offset (cur_ex_offs, +0xE4) and
   func_800A63A4 at step 2 with each offset in turn (index, +0x47); the reply carries
   that entry's 16-bit ID. */
void func_800A6B10(PadInfo *p, s32 offs)
{
    p->cmd_param = offs;
    p->cmd_queued = 0x4C;
    p->cmd_data = &p->cmd_param;
    p->cmd_len = 1;
}

/* libpad PADCMD.OBJ +0xBA8, a static of the object (no SDK symbol): called from func_800A63A4.
   0x20 bytes. */
/* cc97_lib: func_800A6B30 */
/* return_slot_fill: func_800A6B30 */

/* libpad PADCMD.OBJ +0xBA8 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by what it does: queues command 0x46 with the one parameter
   byte `actno`, an actuator number. func_800A63A4 (the send hook _padLoadActInfo
   installs) sends it at step 3 for each actuator in turn (index, +0x47); the reply
   carries that actuator's record. */
void func_800A6B30(PadInfo *p, s32 actno)
{
    p->cmd_param = actno;
    p->cmd_queued = 0x46;
    p->cmd_data = &p->cmd_param;
    p->cmd_len = 1;
}

/* libpad PADCMD.OBJ +0xBC8, a static of the object (no SDK symbol): called from _padSendAtLoadInfo
   [_padSendAtLoadInfo], func_800A63A4. 0x20 bytes. */
/* cc97_lib: func_800A6B50 */
/* return_slot_fill: func_800A6B50 */

/* libpad PADCMD.OBJ +0xBC8 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by what it does: queues command 0x47 with the one parameter
   byte `combno`, a combination number. _padSendAtLoadInfo and
   func_800A63A4 send it at step 4 for each combination in turn (index, +0x47); the
   reply carries the length of that combination's list and its first three bytes. */
void func_800A6B50(PadInfo *p, s32 combno)
{
    p->cmd_param = combno;
    p->cmd_queued = 0x47;
    p->cmd_data = &p->cmd_param;
    p->cmd_len = 1;
}

/* libpad PADCMD.OBJ +0xBE8, a static of the object (no SDK symbol): called from func_800A63A4.
   0x14 bytes. Followed by 1 zero word, the object's .text padded to 8 bytes (the row after its
   endlabel). */
/* cc97_lib: func_800A6B70 */
/* return_slot_fill: func_800A6B70 */

/* libpad PADCMD.OBJ +0xBE8 (SDK 4.5), a static of the object with no SDK symbol in 4.4
   or 4.7, described here by what it does: queues command 0x4B with no parameter bytes.
   func_800A63A4 (the send hook _padLoadActInfo installs) sends it at step 4 while bytes
   of a combination's list are still owed (count +0x48 non-zero); func_800A644C takes
   five list bytes from each reply. The last function of PADCMD.OBJ: one zero word
   follows, the object's .text padded to 8 bytes. */
void func_800A6B70(PadInfo *p)
{
    p->cmd_queued = 0x4B;
    p->cmd_data = 0;
    p->cmd_len = 0;
}
INCLUDE_PAD(4);
