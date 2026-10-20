/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 5 of 13: func_800A1D94,
   MemCardOpen, MemCardClose. The first split had it in psyq_misc_at.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_800A1D94 */
/* reorder_fill: func_800A1D94 */
/* return_slot_fill: func_800A1D94 */
extern s32 D_800EA938;
extern s32 D_800EA93C;
extern s32 D_800EA940;
extern s32 D_800EA944;
extern s32 D_800EA948;
extern McrdGlobal D_800EA960;
s32 func_800A1A10(s32 *state);
void UserFuncOpen(s32 (*func)(s32 *step));
void _clr_card_event(void);
s32 _get_card_event(void);
s32 _get_card_event_x(void);
s32 _chk_card_event(void);
s32 _chk_card_event_x(void);
s32 func_800A34BC(s32 ev);

/* libmcrd _card_accept_back (LIBMCRD.OBJ, static): the step function MemCardAccept
    queues with UserFuncOpen; MemCardOpen and MemCardDeleteFile queue
   it too through their inlined MemCardAccept. The VSync callback calls it once a
   frame with the step's state word until it returns 1; the answer is left in the
   record's rslt (+4).
   State 0 clears the object's five statics (D_800EA93C the retry count, D_800EA948
   the new-card flag, D_800EA944 the last card event, D_800EA940 and D_800EA938
   written here only) and falls into state 1, which queues _card_exist_back (func_800A1A10)
   in front of itself and goes to state 10. State 10 reads that query's rslt: 0
   (McErrNone) goes to state 30; 3 (McErrNewCard) sets the new-card flag and the
   record's bit for the card (+0xC |= 1 << chan), _clr_card_event,
   _card_clear(chan) and goes to state 21; any other result is the
   answer (return 1). State 21 waits for the clear's event (_chk_card_event_x; _get_card_event_x) and falls into state 30:
   _clr_card_event, _card_load(chan), state 31. State 31 waits for
   the load's event (_chk_card_event; _get_card_event)
   and decides on it: 0 answers 3 (McErrNewCard) if the new-card flag is set, else
   0; 4 clears the events, _card_info(chan), state 50; 2 goes back
   to state 1; 1 loads again from state 30, up to 16 times; after that, and for any
   other event, the answer is _rslt_to_code (func_800A34BC) of the event (1 gives
   McErrCardInvalid, 2 McErrCardNotExist, 4 McErrNewCard). State 50 waits for the
   info's event: 0 answers 4 (McErrNotFormat), anything else goes back to state 1.
   The event lives in a local and is copied to the static, which only the
   _rslt_to_code call reads back. */
s32 func_800A1D94(s32 *state)
{
    s32 ev;

    switch (*state) {
    case 0:
        D_800EA93C = 0;
        D_800EA940 = 0;
        D_800EA938 = 0;
        D_800EA948 = 0;
        D_800EA944 = 0;
        (*state)++;
    case 1:
        UserFuncOpen(func_800A1A10);
        *state = 10;
        break;
    case 10:
        switch (D_800EA960.rslt) {
        case 3:
            D_800EA948 = 1;
            D_800EA960.mask |= 1 << D_800EA960.chan;
            _clr_card_event();
            _card_clear(D_800EA960.chan);
            *state = 21;
            break;
        case 0:
            *state = 30;
            break;
        case 1:
        default:
            return 1;
        }
        break;
    case 21:
        if (_chk_card_event_x() == 0) {
            break;
        }
        _get_card_event_x();
        *state = 30;
    case 30:
        _clr_card_event();
        _card_load(D_800EA960.chan);
        (*state)++;
        break;
    case 31:
        if (_chk_card_event() == 0) {
            return 0;
        }
        ev = _get_card_event();
        D_800EA944 = ev;
        switch (ev) {
        case 0:
            D_800EA960.rslt = D_800EA948 ? 3 : 0;
            return 1;
        case 4:
            _clr_card_event();
            _card_info(D_800EA960.chan);
            *state = 50;
            break;
        case 2:
            *state = 1;
            break;
        case 1:
            if (++D_800EA93C < 17) {
                *state = 30;
                break;
            }
        default:
            D_800EA960.rslt = func_800A34BC(D_800EA944);
            return 1;
        }
        break;
    case 50:
        if (_chk_card_event() == 0) {
            return 0;
        }
        ev = _get_card_event();
        D_800EA944 = ev;
        if (ev == 0) {
            D_800EA960.rslt = 4;
            return 1;
        }
        *state = 1;
        break;
    }
    return 0;
}
/* cc97_lib: MemCardOpen */
/* reorder_fill: MemCardOpen */
/* return_slot_fill: MemCardOpen */
extern void (*D_800EA9B8)(s32, s32), (*MemCardCallback(void (*func)(s32, s32)))(s32, s32);
extern char D_80013964[];
extern char D_80013A60[];
s32 printf();
s32 MemCardSync(s32 mode, s32 *cmds, s32 *rslt);
void _make_file_discrpt(s32 chan, char *buf);
char *strcat(char *dst, char *src);

static inline s32 mcrd_accept(s32 chan)
{
    if (D_800EA960.cmd > 0) {
        printf(D_80013964);
        return 0;
    }
    D_800EA960.cmd = 2;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.chan = chan;
    UserFuncOpen(func_800A1D94);
    return 1;
}
/* libmcrd MemCardOpen (LIBMCRD.OBJ): open file `file` on card `chan` for
   MemCardReadData / MemCardWriteData; blocks until it knows. Returns -1 with a
   printf ("file already open") when a file is open (fd, +0x14, not
   negative). Otherwise the device name goes into the record's name buffer (+0x24:
   _make_file_discrpt, _make_file_discrpt, then strcat), chan (+0x10) is
   saved, and the file is test-opened read-only (open, mode 1). While
   that fails: the user's callback is taken out (MemCardCallback, the
   old one kept in D_800EA9B8), MemCardAccept runs on the card (inlined here as
   mcrd_accept, the body of MemCardAccept: cmd = 2, rslt and done cleared, chan
   saved, _card_accept_back func_800A1D94 queued), MemCardSync waits
   for its result and the callback is put back; result 3 (McErrNewCard) tries again,
   2 (McErrCardInvalid) tries again up to four more times, any other result is
   returned, 0 as 5 (McErrFileNotExist). When the test open succeeds it is closed
   (close), the card events are cleared (_clr_card_event),
   fd = open(name, flag | 0x8000) and 0 is returned. */
/* func_800A203C */
s32 MemCardOpen(s32 chan, char *file, s32 flag)
{
    s32 rslt;
    s32 retry;
    s32 fd;

    retry = 0;
    if (D_800EA960.fd >= 0) {
        printf(D_80013A60);
        return -1;
    }
    _make_file_discrpt(chan, D_800EA960.file);
    strcat(D_800EA960.file, file);
    D_800EA960.chan = chan;
    for (;;) {
        fd = open(D_800EA960.file, 1);
        if (fd < 0) {
            D_800EA9B8 = MemCardCallback(0);
            mcrd_accept(chan);
            MemCardSync(0, 0, &rslt);
            MemCardCallback(D_800EA9B8);
            if (rslt == 3) {
                continue;
            }
            if (rslt == 2) {
                if (++retry < 5) {
                    continue;
                }
            }
            if (rslt == 0) {
                rslt = 5;
            }
            return rslt;
        }
        close(fd);
        _clr_card_event();
        D_800EA960.fd = open(D_800EA960.file, flag | 0x8000);
        break;
    }
    return 0;
}

/* cc97_lib: MemCardClose */
/* return_slot_fill: MemCardClose */

/* libmcrd MemCardClose (LIBMCRD.OBJ): close the file MemCardOpen holds, if any
   (close), and mark the record's fd (+0x14) free again with -1. */
/* func_800A21C4 */
void MemCardClose(void)
{
    if (D_800EA960.fd >= 0) {
        close(D_800EA960.fd);
        D_800EA960.fd = -1;
    }
}
