/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 7 of 13: func_800A22C0. The
   first split had it in psyq_misc_av.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_800A22C0 */
/* return_slot_fill: func_800A22C0 */
/* far_syms: D_800B8458 */
extern McrdGlobal D_800EA960;
extern s32 D_800B8458;
s32 func_800A34BC(s32 ev);
void _clr_card_event(void);
s32 _get_card_event(void);
s32 _chk_card_event(void);

/* libmcrd _card_read_data (LIBMCRD.OBJ, static): the step function MemCardReadData
   queues; called from the VSync callback with its state word, returns 1 when the
   command is finished. State 0: clear the retry count D_800B8458 and go on as state
   10. State 10: lseek the open file to the saved offset until it
   answers that offset, clear the card events (_clr_card_event),
   issue the read until it is accepted (returns 0), state = 30.
   State 30: nothing until a card event has arrived (_chk_card_event); then take it (_get_card_event): an error event is
   retried from state 10 up to three times; after that, or on success, the record's
   rslt (+4) = _rslt_to_code (func_800A34BC) of the event and the step is done. */
s32 func_800A22C0(s32 *step)
{
    s32 ev;

    switch (*step) {
    case 0:
        D_800B8458 = 0;
        *step = 10;
    case 10:
        while (lseek(D_800EA960.fd, D_800EA960.ofs, 0) != D_800EA960.ofs) {
        }
        _clr_card_event();
        while (read(D_800EA960.fd, D_800EA960.adrs, D_800EA960.bytes) != 0) {
        }
        *step = 30;
        break;
    case 30:
        if (_chk_card_event() == 0) {
            break;
        }
        ev = _get_card_event();
        if (ev != 0) {
            if (++D_800B8458 < 4) {
                *step = 10;
                break;
            }
            D_800EA960.rslt = func_800A34BC(ev);
        } else {
            D_800EA960.rslt = func_800A34BC(0);
        }
        return 1;
    }
    return 0;
}
