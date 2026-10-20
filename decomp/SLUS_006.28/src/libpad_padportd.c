/* PSY-Q 4.5 libpad, member padportd.o: PadInitDirect, func_800A7318,
   func_800A7380, func_800A7474, func_800A7484, func_800A7540,
   func_800A77BC, func_800A77F4. The first split had it in psyq_misc_aaq.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* Sony libpad, PADPORTD.OBJ whole (0x800A71B8..0x800A7817 of the image). Every word equals the
   on-disk SDK 4.4 object's, relocation fields apart; the release linked is SDK 4.5's, which is
   the same code (the two LIBPAD.LIB differ only in member dates and in the version word of
   PADMAIN.OBJ's .data, and the image holds 4.5's, 0x00450000 at 0x800B8484). These functions
   were `.word` rows of asm/data/data_tables.data.s: the first split cut text from data at
   0x800A5000, and the image's text runs to 0x800A7D98. No config/cc_flags.txt entry: the
   object's converted functions take per-function markers (cc97_lib, and return_slot_fill by the
   epilogue rule). */

/* far_syms: D_800B8488 */
/* far_syms: D_800B848C */
/* far_syms: D_800B8490 */
/* far_syms: D_800B8494 */
/* far_syms: D_800B8498 */
/* far_syms: D_800B849C */
/* far_syms: D_800B84AC */
/* far_syms: D_800B84BC */
/* far_syms: D_800B84C0 */
/* far_syms: D_800B84C8 */
/* far_syms: D_800B84CC */
/* far_syms: D_800B84D0 */
/* far_syms: D_800B84D4 */
/* far_syms: D_800B84DC */
/* far_syms: D_800B8530 */

/* The declarations PADPORTD.OBJ's functions share. The object was built -G0: every .data
   scalar above is reached %hi / %lo (the far_syms lines), and _padFixResult is declared
   unsized so that cc1 does not take it for small data. */

/* The SIO0 (controller / memory card port) registers at 0x1F801040. */

extern volatile PadSio *D_800B8530;    /* PADPORTD .data+0x00 -> 0x1F801040 (SIO0) */
extern u8 D_800EAD70[2][0x23];         /* PADPORTD .bss+0x00: the two ports' raw receive buffers */
extern u8 D_800EADB8[2][0x23];         /* PADPORTD .bss+0x48: the two ports' send buffers */
extern PadInfo D_800EAE00[2];          /* PADPORTD .bss+0x90: the two ports' records */
extern PadInfo *D_800B84BC;            /* _padInfoDir: the two ports' records */
extern s32 D_800B84C0;                 /* _padIntExec: the interrupt handler serves the ports only while this is set */
extern s32 D_800B84C8;                 /* _padSioChan: the port being served */
extern s32 D_800B84CC;                 /* _padSioState: the step of the port's transfer */
extern s32 D_800B84D0;                 /* _padTotalCurr: the actuator current granted so far in this round */
extern s32 D_800B84D4;                 /* _padModeMtap */
extern s32 D_800B84DC;                 /* _padChanStop: the last port served */
extern s32 D_800B84E0[];               /* _padFixResult: one word per port */
extern void (*D_800B8488)(s32);        /* _padFuncNextPort */
extern void (*D_800B848C)(PadInfo *);  /* _padFuncClrInfo */
extern u8 (*D_800B8490)(PadInfo *, s32); /* _padFuncGetTxd */
extern void (*D_800B8494)(PadInfo *);  /* _padFuncCurrLimit */
extern s32 (*D_800B8498)(PadInfo *);   /* _padFuncPtr2Port */
extern PadInfo *(*D_800B849C)(s32);    /* _padFuncPort2Info */
extern void (*D_800B84AC)(PadInfo *);  /* _padFuncClrCmdNo */

void bzero(void *, s32);
void _padSetVsyncParam(void);              /* _padSetVsyncParam (PADMAIN.OBJ) */
s32 _padInitSioMode(PadInfo *p);         /* _padInitSioMode (PADMAIN.OBJ) */
void _padInitDirSeq(void);              /* _padInitDirSeq (PADSEQD.OBJ) */
void _dirFailAuto(PadInfo *p, s32 result); /* _dirFailAuto (PADSEQD.OBJ) */

void func_800A7318(PadInfo *p);
void func_800A7380(s32 result);
void func_800A7474(PadInfo *p);
u8 func_800A7484(PadInfo *p, s32 mtap);
void func_800A7540(PadInfo *p);
s32 func_800A77BC(PadInfo *p);
PadInfo *func_800A77F4(s32 port);

/* libpad PadInitDirect (PADPORTD.OBJ +0x000), 0x160 bytes. */
/* cc97_lib: PadInitDirect */
/* return_slot_fill: PadInitDirect */

/* libpad PadInitDirect (PADPORTD.OBJ, SDK 4.5): sets libpad up for two controllers on
   the console's own ports; pad1 / pad2 are the caller's receive buffers (34 bytes each).
   With the interrupt work switched off (_padIntExec, D_800B84C0, cleared here and set at
   the end) and the multi-tap mode cleared (_padModeMtap, D_800B84D4) it has
   _padInitDirSeq install the sequencer's hooks, points _padInfoDir
   (D_800B84BC) at this object's two port records and installs its own seven hooks:
   _padFuncNextPort func_800A7380, _padFuncClrInfo func_800A7318, _padFuncGetTxd
   func_800A7484, _padFuncCurrLimit func_800A7540, _padFuncPtr2Port func_800A77BC,
   _padFuncPort2Info func_800A77F4, _padFuncClrCmdNo func_800A7474. The records are
   zeroed; each gets the caller's buffer (its first two bytes set to 0xFF, 0), itself as
   head, no multi-tap records, its raw receive and send buffers (0x23 bytes each,
   D_800EAD70 / D_800EADB8) and an empty actuator alignment (six 0xFF). Last comes
   _padSetVsyncParam. Caller in the image: pad_start. */
/* func_800A71B8 */
void PadInitDirect(u8 *pad1, u8 *pad2)
{
    PadInfo *p;
    u8 *align;
    s32 i;
    s32 j;

    D_800B84C0 = 0;
    D_800B84D4 = 0;
    _padInitDirSeq();
    D_800B84BC = D_800EAE00;
    D_800B8488 = func_800A7380;
    D_800B848C = func_800A7318;
    D_800B8490 = func_800A7484;
    D_800B8494 = func_800A7540;
    D_800B8498 = func_800A77BC;
    D_800B849C = func_800A77F4;
    D_800B84AC = func_800A7474;
    bzero(D_800EAE00, sizeof(D_800EAE00));
    D_800EAE00[0].rx_buf = pad1;
    D_800EAE00[1].rx_buf = pad2;
    for (i = 0, p = D_800EAE00; i < 2; i++, p++) {
        p->tap_info = 0;
        p->head = p;
        p->rx_buf[0] = 0xFF;
        p->rx_buf[1] = 0;
        p->recv_buf = D_800EAD70[i];
        p->send_buf = D_800EADB8[i];
        align = p->act_align;
        for (j = 0; j < 6; j++) {
            *align++ = 0xFF;
        }
    }
    _padSetVsyncParam();
    D_800B84C0 = 1;
}

/* libpad PADPORTD.OBJ +0x160, a static of the object (no SDK symbol): its address is stored in
   D_800B848C [_padFuncClrInfo]. 0x68 bytes. */
/* cc97_lib: func_800A7318 */

/* libpad PADPORTD.OBJ +0x160 (SDK 4.5), the function PadInitDirect installs as
   _padFuncClrInfo (D_800B848C); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: forgets the controller a port record describes. A record
   in state 0 (PadStateDiscon) is left alone. Otherwise the state and the sequence step
   (seq, +0x46) go to 0, the queued command's send and receive steps (+0x14, +0x18) are
   dropped, the ID table, the actuator table and the combination table are forgotten
   with their counts, the current extended ID and its offset are cleared (the extended
   ID twice: the target stores it at 0x800A733C and again at 0x800A7350) and the
   actuator alignment (+0x5D) is emptied, six 0xFF. */
void func_800A7318(PadInfo *p)
{
    u8 *align;
    s32 i;

    if (p->state != 0) {
        p->state = 0;
        p->seq = 0;
        p->cur_ex_id = 0;
        p->send_func = 0;
        p->recv_func = 0;
        p->id_num = 0;
        p->cur_ex_offs = 0;
        p->cur_ex_id = 0;
        p->act_num = 0;
        p->comb_num = 0;
        p->id_table = 0;
        p->act_table = 0;
        p->comb_table = 0;
        align = p->act_align;
        for (i = 0; i < 6; i++) {
            *align++ = 0xFF;
        }
    }
}

/* libpad PADPORTD.OBJ +0x1C8, a static of the object (no SDK symbol): its address is stored in
   D_800B8488 [_padFuncNextPort]. 0xF4 bytes. */
/* cc97_lib: func_800A7380 */
/* return_slot_fill: func_800A7380 */

/* libpad PADPORTD.OBJ +0x1C8 (SDK 4.5), the function PadInitDirect installs as
   _padFuncNextPort (D_800B8488); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: closes the transfer with the port being served
   (_padSioChan, D_800B84C8) and starts the next port's. result is how the transfer
   ended: 0 clears the port's _padFixResult word (D_800B84E0), -9 changes nothing, and
   anything else is a failure, handed with the port's record to _dirFailAuto
    and followed by this object's ClrCmdNo (func_800A7474). Then
   _padSioState (D_800B84CC) and the SIO0 control register are cleared and _padSioChan
   steps on. Past _padChanStop (D_800B84DC) the round is over; otherwise
   _padInitSioMode starts the new port, and when that returns 0 the
   same is done again for it with result 0xFFFF. */
void func_800A7380(s32 result)
{
    PadInfo *p;
    s32 done;

    do {
        p = &D_800EAE00[D_800B84C8];
        if (result != -9) {
            if (result == 0) {
                D_800B84E0[D_800B84C8] = 0;
            } else {
                _dirFailAuto(p, result);
                func_800A7474(p);
            }
        }
        D_800B84CC = 0;
        D_800B8530->ctrl = 0;
        D_800B84C8++;
        done = (D_800B84C8 > D_800B84DC) ? 1 : _padInitSioMode(&D_800EAE00[D_800B84C8]);
        result = 0xFFFF;
    } while (done == 0);
}

/* libpad PADPORTD.OBJ +0x2BC, a static of the object (no SDK symbol): its address is stored in
   D_800B84AC [_padFuncClrCmdNo]; called from func_800A7380. 0x10 bytes. */
/* cc97_lib: func_800A7474 */

/* libpad PADPORTD.OBJ +0x2BC (SDK 4.5), the function PadInitDirect installs as
   _padFuncClrCmdNo (D_800B84AC); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: the queued command (cmd_queued, +0x37, set by
   _padSetCmd) moves to cmd_taken (+0x38) and the queue byte is cleared. Also called
   directly by func_800A7380. */
void func_800A7474(PadInfo *p)
{
    u8 cmd = p->cmd_queued;

    p->cmd_queued = 0;
    p->cmd_taken = cmd;
}

/* libpad PADPORTD.OBJ +0x2CC, a static of the object (no SDK symbol): its address is stored in
   D_800B8490 [_padFuncGetTxd]. 0xBC bytes. */
/* cc97_lib: func_800A7484 */

/* libpad PADPORTD.OBJ +0x2CC (SDK 4.5), the function PadInitDirect installs as
   _padFuncGetTxd (D_800B8490); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: the byte to send at the transfer's current position,
   send_len (+0x45), which counts the three header bytes. With no command queued it is
   the caller's actuator data (act_data / act_len, PadSetAct's): 0 past its end, and 0
   for any of the first six bytes func_800A7540 has not enabled (act_enable, +0x57).
   With a command queued it is the command's parameter bytes (cmd_data / cmd_len),
   padded with 0xFF for command 0x4D and with 0 for any other. The hook's second
   argument (PADIF.OBJ passes one, and the multi-tap object's version reads it) is not
   used here. */
u8 func_800A7484(PadInfo *p, s32 mtap)
{
    s32 n = p->send_len - 3;

    switch (p->cmd_queued) {
    case 0:
        if (n < 6 && p->act_enable[n] == 0) {
            return 0;
        }
        return (n < p->act_len) ? p->act_data[n] : 0;
    case 0x4D:
        return (n < p->cmd_len) ? p->cmd_data[n] : 0xFF;
    default:
        return (n < p->cmd_len) ? p->cmd_data[n] : 0;
    }
}

/* libpad PADPORTD.OBJ +0x388, a static of the object (no SDK symbol): its address is stored in
   D_800B8494 [_padFuncCurrLimit]. 0x27C bytes. */
/* cc97_lib: func_800A7540 */
/* return_slot_fill: func_800A7540 */

/* libpad PADPORTD.OBJ +0x388 (SDK 4.5), the function PadInitDirect installs as
   _padFuncCurrLimit (D_800B8494); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: decides which of a port's six actuator bytes may be
   sent this round, within the budget of 60 that _padTotalCurr (D_800B84D0) counts.
   The six enables (act_enable, +0x57) are cleared first. A controller with an extended
   ID and actuator data is taken actuator by actuator: when one of the first act_len
   (at most six) data bytes aligned to it (act_align, +0x5D) asks for it (non-zero; bit
   0 alone for an actuator of size 0) and its current (the table's curr) still fits,
   the current is added and every byte aligned to that actuator is enabled. Otherwise:
   with no extended ID and at least two data bytes, IDs 4, 5 and 7 get bytes 0 and 1
   for 10 when data[0] has 0x40 of its top two bits and data[1] bit 0 and the budget
   allows; failing that, ID 3 gets byte 0; failing that, a record with no extended ID
   gets all six. */
void func_800A7540(PadInfo *p)
{
    s32 i;
    s32 j;
    s32 found;
    s32 n;
    s32 mask;
    u8 *align;
    u8 *bp;

    bzero(p->act_enable, 6);
    if (p->cur_ex_id != 0 && p->act_data != 0) {
        n = (p->act_len < 7) ? p->act_len : 6;
        for (i = 0; i < p->act_num; i++) {
            found = 0;
            mask = (p->act_table[i].size != 0) ? 0xFF : 1;
            align = p->act_align;
            bp = p->act_data;
            for (j = 0; j < n; j++) {
                if (*align == i && (*bp & mask)) {
                    found = 1;
                    break;
                }
                align++;
                bp++;
            }
            if (found) {
                if (D_800B84D0 + p->act_table[i].curr < 61) {
                    D_800B84D0 += p->act_table[i].curr;
                } else {
                    found = 0;
                }
            }
            if (found) {
                align = p->act_align;
                bp = p->act_enable;
                for (j = 0; j < n; j++) {
                    if (*align++ == i) {
                        *bp = 1;
                    }
                    bp++;
                }
            }
        }
    } else if ((p->cur_id == 4 || p->cur_id == 5 || p->cur_id == 7) && p->cur_ex_id == 0 && p->act_len >= 2) {
        if ((p->act_data[0] & 0xC0) == 0x40 && (p->act_data[1] & 1) && D_800B84D0 + 10 < 61) {
            p->act_enable[0] = p->act_enable[1] = 1;
            D_800B84D0 += 10;
        }
    } else if (p->cur_id == 3) {
        p->act_enable[0] = 1;
    } else if (p->cur_ex_id == 0) {
        for (j = 0; j < 6; j++) {
            p->act_enable[j] = 1;
        }
    }
}

/* libpad PADPORTD.OBJ +0x604, a static of the object (no SDK symbol): its address is stored in
   D_800B8498 [_padFuncPtr2Port]. 0x38 bytes. */
/* cc97_lib: func_800A77BC */

/* libpad PADPORTD.OBJ +0x604 (SDK 4.5), the function PadInitDirect installs as
   _padFuncPtr2Port (D_800B8498); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: the number of the port a record belongs to, 0x10 for
   the first of this object's two records and 0x20 for the second; 0xFF for a pointer
   that is neither. */
s32 func_800A77BC(PadInfo *p)
{
    s32 i;

    for (i = 0; i < 2; i++) {
        if (p == &D_800EAE00[i]) {
            return (i + 1) << 4;
        }
    }
    return 0xFF;
}

/* libpad PADPORTD.OBJ +0x63C, a static of the object (no SDK symbol): its address is stored in
   D_800B849C [_padFuncPort2Info]. 0x20 bytes. Followed by 1 zero word, the object's .text
   padded to 8 bytes (the row after its endlabel). */
/* cc97_lib: func_800A77F4 */

/* libpad PADPORTD.OBJ +0x63C (SDK 4.5), the function PadInitDirect installs as
   _padFuncPort2Info (D_800B849C); a static of the object, no SDK symbol in 4.4 or 4.7,
   described here by that role: the record of a port number, the first of this object's
   two for 0x00..0x0F and the second for a number with any of the bits 0xF0. The public
   entry points (PadGetState, PadInfoMode ...) reach their record through it. */
PadInfo *func_800A77F4(s32 port)
{
    PadInfo *p = D_800EAE00;

    if (port & 0xF0) {
        p++;
    }
    return p;
}
INCLUDE_PAD(4);
