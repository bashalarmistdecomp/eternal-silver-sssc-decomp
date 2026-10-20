/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 3 of 13: func_800A1A10. The
   first split had it in psyq_misc_ar.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_800A1A10 */
/* reorder_fill: func_800A1A10 */
/* return_slot_fill: func_800A1A10 */
/* far_syms: D_800B8450 D_800B8454 D_800EA930 D_800EA934 (target reaches them %hi/%lo) */
extern McrdGlobal D_800EA960;
extern s32 D_800EA930;      /* .bss+0   retries of this query */
extern s32 D_800EA934;      /* .bss+4   the card event: 0 IOE, 1 ERROR, 2 TIMEOUT, 4 NEW CARD */
extern u32 D_800B8450;      /* .data+0  the port's f50 value at the start of the query */
extern s32 D_800B8454;      /* .data+4  the port's f48 flag when the event arrived */
extern char D_8001398C[];   /* "error" */
s32 printf();
s32 func_800A34BC();        /* _rslt_to_code: event -> McErr* (0 -> 0, 1 -> 2, 2 -> 1, 4 -> 3) */
void _clr_card_event();
s32 _get_card_event();
s32 _get_card_event_x();
s32 _chk_card_event();
s32 _chk_card_event_x();

/* libmcrd _card_exist_back (LIBMCRD.OBJ, static; psyz's name): MemCardExist's state
   callback -- UserFuncOpen pushes it, and every UserFuncExecute calls it with its zeroed
   state block until it returns non-zero. By *state:
   0  start: clear the event and the retry count, take and zero the port's f50.
   10 _clr_card_event(), _card_info(chan), go to 11.
   11 wait for a card event (return 0 until one is there), then by the event:
      NEW CARD (4)  the first one on this port (f48 clear) with the taken f50 below 0x80
                    is acknowledged -- _clr_card_event(), _card_clear(chan), state 21;
                    otherwise f48 = 1 and it is the result (McErrNewCard).
      IOE (0)       f48 = 0, result McErrNone -- or McErrNewCard when the port's bit is
                    not set in mask.
      TIMEOUT (2)   _card_info again (state 10) until the 3rd failure, then f48 = 1 and
                    McErrCardNotExist.
      ERROR (1)     _card_info again until the 17th failure (TIMEOUTs count on the same
                    counter), then McErrCardInvalid, f48 = 0.
      any other     the event | 0x8000, f48 = 0.
   21 wait for the _card_clear write's event (the _x set), drop it, start over at 0.
   Any other state prints "error". Returns 1 when result is set, 0 while still working. */
s32 func_800A1A10(s32 *state)
{
    switch (*state) {
    case 0:
        D_800EA934 = 0;
        D_800EA930 = 0;
        *state = 10;
        D_800B8450 = D_800EA960.frames[D_800EA960.chan >> 4];
        D_800EA960.frames[D_800EA960.chan >> 4] = 0;
    case 10:
        _clr_card_event();
        _card_info(D_800EA960.chan);
        (*state)++;
        break;
    case 11:
        if (_chk_card_event() == 0)
            return 0;
        D_800EA934 = _get_card_event();
        D_800B8454 = D_800EA960.new_card[D_800EA960.chan >> 4];
        switch (D_800EA934) {
        case 4:
            if (D_800B8454 == 0 && D_800B8450 < 0x80) {
                _clr_card_event();
                _card_clear(D_800EA960.chan);
                *state = 21;
                break;
            }
            D_800EA960.new_card[D_800EA960.chan >> 4] = 1;
            D_800EA960.rslt = func_800A34BC(D_800EA934);
            return 1;
        case 0:
            if (!(AS(volatile u32, D_800EA960.mask) & (1 << D_800EA960.chan)))
                D_800EA934 = 4;
            D_800EA960.new_card[D_800EA960.chan >> 4] = 0;
            D_800EA960.rslt = func_800A34BC(D_800EA934);
            return 1;
        case 2:
            if (++D_800EA930 < 3) {
                *state = 10;
                break;
            }
            D_800EA960.new_card[D_800EA960.chan >> 4] = 1;
            D_800EA960.rslt = func_800A34BC(2);
            return 1;
        case 1:
            if (++D_800EA930 < 0x11) {
                *state = 10;
                break;
            }
            D_800EA960.rslt = func_800A34BC(1);
            D_800EA960.new_card[D_800EA960.chan >> 4] = 0;
            return 1;
        default:
            D_800EA960.rslt = func_800A34BC(D_800EA934);
            D_800EA960.new_card[D_800EA960.chan >> 4] = 0;
            return 1;
        }
        break;
    case 21:
        if (_chk_card_event_x() == 0)
            return 0;
        _get_card_event_x();
        *state = 0;
        break;
    default:
        printf(D_8001398C);
        break;
    }
    return 0;
}
