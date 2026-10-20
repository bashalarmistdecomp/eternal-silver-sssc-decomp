/* PSY-Q 4.5 libpad, member padentry.o, part 2 of 2: PadInfoMode,
   PadInfoAct, PadInfoComb, PadSetActAlign, PadSetMainMode, PadSetAct. The
   first split had it in psyq_misc_aal.c, psyq_misc_aam.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* far_syms: D_800B849C (target reaches it %hi/%lo) */
extern long HWD0, VWD0;
/* One actuator of a controller as PadInfoAct reports it (libpad.h InfoActFunc ..
   InfoActSign, in that order). */

extern PadInfo *(*D_800B849C)(s32);

/* cc97_lib: PadInfoMode */
/* return_slot_fill: PadInfoMode */
/* libpad PadInfoMode (PADENTRY.OBJ): one item of the controller's mode information
   (libpad.h: 1 InfoModeCurID, 2 InfoModeCurExID, 3 InfoModeCurExOffs, 4 InfoModeIdTable).
   For the id table, offs < 0 asks for the number of entries and an offs past the end
   gives 0. Term 100 returns the record's word at +0x4C. Any other term gives 0. */
/* func_800A4E70 */
s32 PadInfoMode(s32 port, s32 term, s32 offs)
{
    PadInfo *p = D_800B849C(port);

    switch (term) {
    case 1:
        return p->cur_id;
    case 2:
        return p->cur_ex_id;
    case 3:
        return p->cur_ex_offs;
    case 4:
        if (offs < 0) {
            return p->id_num;
        }
        if (offs < p->id_num) {
            return p->id_table[offs];
        }
        break;
    case 100:
        return p->fail_count;
    }
    return 0;
}
/* cc97_lib: PadInfoAct */
/* return_slot_fill: PadInfoAct */
/* far_syms: D_800B849C */
extern PadInfo *(*D_800B849C)(s32);

/* libpad PadInfoAct (PADENTRY.OBJ): one item of the controller's actuator information.
   actno < 0 asks for the number of actuators (the record's +0xE9); for an actuator the
   controller has, term picks a byte of its 5-byte record at +0x04 (libpad.h: 1
   InfoActFunc, 2 InfoActSub, 3 InfoActSize, 4 InfoActCurr, 5 InfoActSign). An actno past
   the end or any other term gives 0. The port's record comes through _padFuncPort2Info
   (D_800B849C). */
/* func_800A4F68 */
s32 PadInfoAct(s32 port, s32 actno, s32 term)
{
    PadInfo *p = D_800B849C(port);
    PadActInfo *act;

    if (actno < 0) {
        return p->act_num;
    }
    if (actno < p->act_num) {
        act = &p->act_table[actno];
        switch (term) {
        case 1:
            return act->func;
        case 2:
            return act->sub;
        case 3:
            return act->size;
        case 4:
            return act->curr;
        case 5:
            return act->sign;
        }
    }
    return 0;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80013C24_pad);



/* Sony libpad, PADENTRY.OBJ from +0x344 (0x800A503C..0x800A51B7 of the image; the object's
   first seven functions are libpad_padentry_1.c and libpad_padentry_2.c). Every word equals the on-disk
   SDK 4.4 object's, relocation fields apart; the release linked is SDK 4.5's, which is the same
   code (the two LIBPAD.LIB differ only in member dates and in the version word of PADMAIN.OBJ's
   .data, and the image holds 4.5's, 0x00450000 at 0x800B8484). These functions were `.word`
   rows of asm/data/data_tables.data.s: the first split cut text from data at 0x800A5000, and
   the image's text runs to 0x800A7D98. No config/cc_flags.txt entry: the object's converted
   functions take per-function markers (cc97_lib, and return_slot_fill by the epilogue rule). */

/* libpad PadInfoComb (PADENTRY.OBJ +0x344), 0xA8 bytes. */
/* cc97_lib: PadInfoComb */
/* return_slot_fill: PadInfoComb */
extern PadInfo *(*D_800B849C)(s32);

/* libpad PadInfoComb (PADENTRY.OBJ, SDK 4.5): which actuators of the controller on
   `port` may run at the same time. listno < 0 asks for the number of combination
   lists (the record's comb_num, +0xEA). For a list the controller has, offs < 0 asks
   for the number of actuators in it and an offs below that for the actuator number
   at that place; the lists are the 8-byte records at comb_table (+0x08), a count byte
   and a pointer to the numbers. A listno or offs past the end gives 0. The port's
   record comes through _padFuncPort2Info (D_800B849C). Nothing in the image calls it.
   The record typedef is libpad_padentry_1.c's with comb_table typed as the list records
   it points at (there it is a bare `u8 *`: no PADENTRY.OBJ function but this one
   reads it; PADCMD.OBJ, libpad_padcmd.c, builds the lists and has the same
   PadCombInfo). */
/* func_800A503C */
s32 PadInfoComb(s32 port, s32 listno, s32 offs)
{
    PadInfo *p = D_800B849C(port);
    PadCombInfo *comb;

    if (listno < 0) {
        return p->comb_num;
    }
    if (listno < p->comb_num) {
        comb = &p->comb_table[listno];
        if (offs < 0) {
            return comb->num;
        }
        if (offs < comb->num) {
            return comb->list[offs];
        }
    }
    return 0;
}

/* libpad PadSetActAlign (PADENTRY.OBJ +0x3EC), 0x38 bytes. */
/* cc97_lib: PadSetActAlign */
/* return_slot_fill: PadSetActAlign */
extern PadInfo *(*D_800B849C)(s32);
s32 _padSetActAlign(PadInfo *p, u8 *data);

/* libpad PadSetActAlign (PADENTRY.OBJ, SDK 4.5): tells the controller on `port` where
   in the transmit buffer each actuator's parameter sits (`data`, six bytes of actuator
   numbers). Hands the port's record (through _padFuncPort2Info, D_800B849C) and the
   array to _padSetActAlign (PADCMD.OBJ) and returns its result: 1 when
   the command was queued, 0 when _padFuncChkEng refused it. Caller in the image:
   pad_update. */
/* func_800A50E4 */
s32 PadSetActAlign(s32 port, u8 *data)
{
    return _padSetActAlign(D_800B849C(port), data);
}

/* libpad PadSetMainMode (PADENTRY.OBJ +0x424), 0x48 bytes. */
/* cc97_lib: PadSetMainMode */
/* return_slot_fill: PadSetMainMode */
extern PadInfo *(*D_800B849C)(s32);
s32 _padSetMainMode(PadInfo *p, u8 offs, u8 lock);

/* libpad PadSetMainMode (PADENTRY.OBJ, SDK 4.5): switches the controller on `port` to
   the mode at place `offs` of its ID table and locks or frees its mode button (`lock`:
   bit 1 = change the lock, bit 0 = locked). Hands the port's record (through
   _padFuncPort2Info, D_800B849C) and the two values, each cut to a byte, to
   _padSetMainMode (PADCMD.OBJ) and returns its result: 1 when the
   command was queued, 0 when _padFuncChkEng refused it. Nothing in the image calls
   it. */
/* func_800A511C */
s32 PadSetMainMode(s32 port, s32 offs, s32 lock)
{
    return _padSetMainMode(D_800B849C(port), offs, lock);
}

/* libpad PadSetAct (PADENTRY.OBJ +0x46C), 0x48 bytes. Followed by 3 zero words, the object's
   .text padded to 8 bytes (the rows after its endlabel). */
/* cc97_lib: PadSetAct */
/* return_slot_fill: PadSetAct */
extern PadInfo *(*D_800B849C)(s32);
void _padSetAct(PadInfo *p, u8 *data, s32 len);

/* libpad PadSetAct (PADENTRY.OBJ, SDK 4.5): registers the transmit buffer of the
   controller on `port`, `len` bytes at `data` (the actuator parameters libpad sends at
   each vertical blank). Hands the port's record (through _padFuncPort2Info,
   D_800B849C) and both to _padSetAct (PADCMD.OBJ), which stores them
   in the record. Returns nothing (libpad.h). Callers in the image: pad_start,
   pad_update. */
/* func_800A5164 */
void PadSetAct(s32 port, u8 *data, s32 len)
{
    _padSetAct(D_800B849C(port), data, len);
}
INCLUDE_PAD(12);
