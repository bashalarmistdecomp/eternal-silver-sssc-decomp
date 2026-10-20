/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 11 of 13: MemCardUnformat,
   func_800A34BC. The first split had it in psyq_misc_az.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: MemCardUnformat */
/* reorder_fill: MemCardUnformat */
/* return_slot_fill: MemCardUnformat */
extern McrdGlobal D_800EA960;
extern char D_80013B08[];
s32 printf();
void _clr_card_event(void);
s32 _get_card_event_x(void);

/* libmcrd MemCardUnformat(chan) (LIBMCRD.OBJ in this SDK 4.5 build; its own
   object from 4.6 on): wipe the card's directory. While a command is running (cmd,
   +0, not 0) it prints "Access Denied. : system busy" (D_80013B08) and returns -1.
   Otherwise it fills a 128-byte frame with 0xFF and writes it to blocks 0..14 of
   the card, each time _clr_card_event, _new_card,
   _card_write(chan, block, frame) and _get_card_event_x
   : the first write whose event is not 0 ends it with 0; all fifteen
   written returns 1. */
/* func_800A3408 */
s32 MemCardUnformat(s32 chan)
{
    s8 buf[128];
    s32 i;

    if (D_800EA960.cmd != 0) {
        printf(D_80013B08);
        return -1;
    }
    for (i = 0; i < 128; i++) {
        buf[i] = 0xFF;
    }
    for (i = 0; i < 15; i++) {
        _clr_card_event();
        _new_card();
        _card_write(chan, i, buf);
        if (_get_card_event_x() != 0) {
            return 0;
        }
    }
    return 1;
}

/* cc97_lib: func_800A34BC */

/* libmcrd _rslt_to_code(rslt) (LIBMCRD.OBJ, static; the name is the 4.0 object's
   signature label): a card event number, as _get_card_event_x
   returns it, to the library's result code: 0 -> McErrNone (0), 1 ->
   McErrCardInvalid (2), 2 -> McErrCardNotExist (1), 4 -> McErrNewCard (3), anything
   else -> the number | McErrExtend (0x8000). The object's recipe (plain cc97_lib; a
   frameless leaf with nothing for the jr slot).
   MARKED SPELLING: the case order 0, 2, 4, 1, default. 45 of the 120 orders of the
   five labels give these bytes, ascending order (0, 1, 2, 4, default) scores 15/21;
   the order the original was written in is not known. */
s32 func_800A34BC(s32 rslt)
{
    s32 ret;

    ret = 0;
    switch (rslt) {
    case 0:
        break;
    case 2:
        ret = 1;
        break;
    case 4:
        ret = 3;
        break;
    case 1:
        ret = 2;
        break;
    default:
        ret = rslt | 0x8000;
        break;
    }
    return ret;
}
