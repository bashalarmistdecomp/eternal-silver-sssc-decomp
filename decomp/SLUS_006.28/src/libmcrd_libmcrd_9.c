/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 9 of 13: func_800A24B4,
   MemCardReadFile, func_800A2708, MemCardWriteFile, func_800A2960,
   MemCardGetDirentry, MemCardCallback, MemCardSync, MemCardCreateFile,
   MemCardDeleteFile. The first split had it in psyq_misc_ax.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: func_800A24B4 */
/* return_slot_fill: func_800A24B4 */
/* far_syms: D_800B845C */
extern volatile s32 D_800EA950[];
extern McrdGlobal D_800EA960;
extern s32 D_800B845C;
s32 func_800A34BC(s32 ev);
void _clr_card_event(void);
s32 _get_card_event(void);
s32 _chk_card_event(void);

/* libmcrd _card_write_data (LIBMCRD.OBJ, static): the step function
   MemCardWriteData queues; _card_read_data's twin with write and
   its own retry count D_800B845C. State 0 only clears the count and sets state 10
   (the seek and the write start on the next call, where the read's state 0 falls
   straight into them). State 10: lseek to the saved offset until it
   answers that offset, _clr_card_event, write until accepted
   (returns 0), state = 30. State 30: wait for a card event (_chk_card_event), take it (_get_card_event); an error event goes
   back to state 10 up to three times; after that, or on success, rslt (+4) =
   _rslt_to_code (func_800A34BC) of the event and the step returns 1. The cases
   stand in the object in the order 0, 30, 10. */
s32 func_800A24B4(s32 *step)
{
    s32 ev;

    switch (*step) {
    case 0:
        D_800B845C = 0;
        *step = 10;
        break;
    case 30:
        if (_chk_card_event() == 0) {
            break;
        }
        ev = _get_card_event();
        if (ev != 0) {
            if (++D_800B845C < 4) {
                *step = 10;
                break;
            }
            D_800EA960.rslt = func_800A34BC(ev);
        } else {
            D_800EA960.rslt = func_800A34BC(0);
        }
        return 1;
    case 10:
        while (lseek(D_800EA960.fd, D_800EA960.ofs, 0) != D_800EA960.ofs) {
        }
        _clr_card_event();
        while (write(D_800EA960.fd, D_800EA960.adrs, D_800EA960.bytes) != 0) {
        }
        *step = 30;
        break;
    }
    return 0;
}
/* cc97_lib: MemCardReadFile */
/* reorder_fill: MemCardReadFile */
/* return_slot_fill: MemCardReadFile */
extern char D_80013A60[];
extern char D_80013AAC[];
extern char D_80013AD8[];
extern char D_80013B08[];
s32 printf();
s32 func_800A2708(s32 *step);
void _make_file_discrpt(s32, char *);  /* TU declares this below */
char *strcat(char *, char *);  /* TU declares this below */
void UserFuncOpen(s32 (*func)(s32 *step));

/* libmcrd MemCardReadFile (LIBMCRD.OBJ): start an asynchronous open + read + close
   of `bytes` bytes at offset `ofs` of file `file` on card `chan` into `adrs`.
   Refused with a printf and 0 when a command is running (cmd, +0,
   positive: "system busy"), when MemCardOpen holds a file (fd, +0x14, not negative:
   "file already open"), or when the size or the offset is not a multiple of 128.
   Otherwise the device name goes into the record's name buffer (+0x24):
   _make_file_discrpt (_make_file_discrpt) writes "buXX:" and strcat
   appends the file name; cmd = 3 (McFuncReadFile), rslt and done are cleared, ofs /
   adrs / bytes / chan are saved, and the step function _card_read_file
   (func_800A2708) is queued with UserFuncOpen; returns 1. */
/* func_800A25F0 */
s32 MemCardReadFile(s32 chan, char *file, u32 *adrs, s32 ofs, s32 bytes)
{
    if (D_800EA960.cmd > 0) {
        printf(D_80013B08);
        return 0;
    }
    if (D_800EA960.fd >= 0) {
        printf(D_80013A60);
        return 0;
    }
    if (bytes & 0x7F) {
        printf(D_80013AAC);
        return 0;
    }
    if (ofs & 0x7F) {
        printf(D_80013AD8);
        return 0;
    }
    _make_file_discrpt(chan, D_800EA960.file);
    strcat(D_800EA960.file, file);
    D_800EA960.cmd = 3;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.ofs = ofs;
    D_800EA960.adrs = adrs;
    D_800EA960.bytes = bytes;
    D_800EA960.chan = chan;
    UserFuncOpen(func_800A2708);
    return 1;
}
/* cc97_lib: func_800A2708 */
/* return_slot_fill: func_800A2708 */
/* far_syms: D_800B8460 */
extern s32 D_800B8460;
s32 func_800A1A10(s32 *step);
s32 func_800A22C0(s32 *step);
s32 _get_card_event_x(void);
s32 _chk_card_event_x(void);

/* libmcrd _card_read_file (LIBMCRD.OBJ, static; the name is the 4.0 signature's):
   the step function MemCardReadFile queues with UserFuncOpen
   ; the VSync callback calls it once a frame with its state word
   until it returns 1, and the command's answer is the record's rslt (+4).
   State 0 clears the object's static D_800B8460 (written here only), queues
   _card_exist_back (func_800A1A10) in front of itself and goes to state 10. State 10 reads
   that query's rslt: not 0 ends the command with it (return 1); else the file named
   in the record (+0x24) is opened with BIOS open, mode 0x8001 (read,
   no wait), the handle kept in fd (+0x14); a negative handle ends the command with
   rslt = 5 (McErrFileNotExist). Otherwise it falls into state 11: state = 20 and
   _card_read_data (func_800A22C0) is queued, which transfers the bytes the record
   names (ofs, bytes, adrs). State 20 reads its rslt: 3 (McErrNewCard) runs
   _clr_card_event and _card_clear(chan) and goes to
   state 22; any other result closes the file (close), sets fd = -1 and ends
   the command. State 22 waits for the clear's event (_chk_card_event_x), takes it (_get_card_event_x) and goes back to state
   11, so the transfer is queued again. Any other state returns 0. The cases stand
   in the object in the order 0, 10, 11, 20, 22 behind a 23-entry jump table. */
s32 func_800A2708(s32 *step)
{
    switch (*step) {
    case 0:
        D_800B8460 = 0;
        UserFuncOpen(func_800A1A10);
        *step = 10;
        break;
    case 10:
        if (D_800EA960.rslt != 0) {
            return 1;
        }
        D_800EA960.fd = open(D_800EA960.file, 0x8001);
        if (D_800EA960.fd < 0) {
            D_800EA960.rslt = 5;
            return 1;
        }
    case 11:
        *step = 20;
        UserFuncOpen(func_800A22C0);
        break;
    case 20:
        if (D_800EA960.rslt == 3) {
            _clr_card_event();
            _card_clear(D_800EA960.chan);
            *step = 22;
            break;
        }
        close(D_800EA960.fd);
        D_800EA960.fd = -1;
        return 1;
    case 22:
        if (_chk_card_event_x() == 0) {
            break;
        }
        _get_card_event_x();
        *step = 11;
        break;
    }
    return 0;
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80013B2C_pad);
/* cc97_lib: MemCardWriteFile */
/* reorder_fill: MemCardWriteFile */
/* return_slot_fill: MemCardWriteFile */
s32 func_800A2960(s32 *step);

/* libmcrd MemCardWriteFile (LIBMCRD.OBJ): start an asynchronous open + write +
   close of `bytes` bytes from `adrs` at offset `ofs` of file `file` on card `chan`;
   MemCardReadFile's (MemCardReadFile) twin. Refused with a printf and
   0 when a command is running (cmd, +0, positive: "system busy"), when MemCardOpen
   holds a file (fd, +0x14, not negative: "file already open"), or when the size or
   the offset is not a multiple of 128 (the size is tested first). Otherwise the
   device name goes into the record's name buffer (+0x24): _make_file_discrpt
   (_make_file_discrpt) writes "buXX:" and strcat appends the file name;
   cmd = 4 (McFuncWriteFile), rslt and done are cleared, ofs / adrs / bytes / chan
   are saved, and the step function _card_write_file (func_800A2960) is queued with
   UserFuncOpen; returns 1. */
/* func_800A2848 */
s32 MemCardWriteFile(s32 chan, char *file, u32 *adrs, s32 ofs, s32 bytes)
{
    if (D_800EA960.cmd > 0) {
        printf(D_80013B08);
        return 0;
    }
    if (D_800EA960.fd >= 0) {
        printf(D_80013A60);
        return 0;
    }
    if (bytes & 0x7F) {
        printf(D_80013AAC);
        return 0;
    }
    if (ofs & 0x7F) {
        printf(D_80013AD8);
        return 0;
    }
    _make_file_discrpt(chan, D_800EA960.file);
    strcat(D_800EA960.file, file);
    D_800EA960.cmd = 4;
    D_800EA960.rslt = 0;
    D_800EA960.done = 0;
    D_800EA960.ofs = ofs;
    D_800EA960.adrs = adrs;
    D_800EA960.bytes = bytes;
    D_800EA960.chan = chan;
    UserFuncOpen(func_800A2960);
    return 1;
}
/* cc97_lib: func_800A2960 */
/* return_slot_fill: func_800A2960 */
/* far_syms: D_800B8464 */
extern s32 D_800B8464;

/* libmcrd _card_write_file (LIBMCRD.OBJ, static; the name is the 4.0 signature's):
   the step function MemCardWriteFile queues; _card_read_file's
   (func_800A2708) twin, statement for statement, with its own static D_800B8464
   (cleared in state 0, written here only) and _card_write_data (func_800A24B4)
   as the transfer step. State 0 queues _card_exist_back (func_800A1A10) and goes to state
   10; state 10 ends the command with that query's rslt (+4) when it is not 0, else
   opens the record's file (+0x24) with BIOS open - mode 0x8001,
   the same as the read step's - into fd (+0x14), ending with rslt = 5
   (McErrFileNotExist) on a negative handle, and falls into state 11: state = 20,
   _card_write_data queued. State 20: rslt 3 (McErrNewCard) runs _clr_card_event
    and _card_clear(chan) and goes to state 22; any
   other result closes the file (close), sets fd = -1 and ends the command.
   State 22 waits for the clear's event (_chk_card_event_x), takes it
   (_get_card_event_x) and queues the transfer again through state
   11. The cases stand in the object in the order 0, 10, 11, 20, 22 behind a
   23-entry jump table. */
s32 func_800A2960(s32 *step)
{
    switch (*step) {
    case 0:
        D_800B8464 = 0;
        UserFuncOpen(func_800A1A10);
        *step = 10;
        break;
    case 10:
        if (D_800EA960.rslt != 0) {
            return 1;
        }
        D_800EA960.fd = open(D_800EA960.file, 0x8001);
        if (D_800EA960.fd < 0) {
            D_800EA960.rslt = 5;
            return 1;
        }
    case 11:
        *step = 20;
        UserFuncOpen(func_800A24B4);
        break;
    case 20:
        if (D_800EA960.rslt == 3) {
            _clr_card_event();
            _card_clear(D_800EA960.chan);
            *step = 22;
            break;
        }
        close(D_800EA960.fd);
        D_800EA960.fd = -1;
        return 1;
    case 22:
        if (_chk_card_event_x() == 0) {
            break;
        }
        _get_card_event_x();
        *step = 11;
        break;
    }
    return 0;
}

typedef void (*MemCB)(s32, s32);    /* libmcrd.h: the function MemCardCallback installs */
extern MemCB D_800EA9B8;
extern char D_80013964[];
s32 func_800A1D94(s32 *state);
MemCB MemCardCallback(MemCB func);

static inline s32 mcrd_accept(s32 chan)
{
    /* MemCardAccept's source (MemCardAccept), inlined where the object has it
       inlined: once in MemCardGetDirentry, three times in
       MemCardDeleteFile. */
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
/* cc97_lib: MemCardGetDirentry */
/* reorder_fill: MemCardGetDirentry */
/* return_slot_fill: MemCardGetDirentry */
s32 MemCardSync(s32 mode, s32 *cmds, s32 *rslt);  /* TU defines this below */
struct DIRENTRY *nextfile(struct DIRENTRY *dir);

/* libmcrd MemCardGetDirentry(chan, name, dir, files, ofs, max) (LIBMCRD.OBJ): list
   the files of the card in port chan whose names fit the pattern `name`,
   synchronously. While a command is running (cmd, +0, not 0) it prints "Access
   Denied. : system busy" (D_80013B08) and returns -1. Otherwise the pattern becomes
   "buXX:" + name in a local buffer (_make_file_discrpt, _make_file_discrpt; strcat) and the record's bit for the card is set (+0xC |= 1 << chan).

   The directory is then walked for at most ofs + max entries: the first with
   firstfile after _clr_card_event, the others with
   nextfile on the same DIRENTRY. The walk ends at the first null
   answer. An entry whose index is ofs or more is copied to dir[] (when dir is not
   null) and counted. At the end the count goes to *files (when files is not null)
   and the function returns 0 (McErrNone).

   A null firstfile is looked into: the card event (_get_card_event_x) through _rslt_to_code (func_800A34BC) into rslt; 0 means the card
   is fine and has no such file (the walk ends, 0 files). Otherwise firstfile is
   tried again, four times in all; after the fourth failure the card is checked the
   way MemCardOpen and MemCardDeleteFile do it - the user's MemCardCallback masked
   (MemCardCallback(0), the old one kept in D_800EA9B8 and put back after), an inlined
   MemCardAccept(chan), MemCardSync(0, 0, &rslt) (a real call here:
   the object defines it after this function) - and that result is returned; *files
   is not written on that path.

   Shape: MemCardAccept is INLINED in the original (its chan store goes through the
   symbol, lui $at / sw, between register-relative stores; written in line it is
   133/151), so the TU's helper mcrd_accept is used. The retry count is set before
   the walk's index and the index is tested before dir. */
/* func_800A2AA0 */
s32 MemCardGetDirentry(s32 chan, char *name, struct DIRENTRY *dir, s32 *files, s32 ofs, s32 max)
{
    char path[32];
    struct DIRENTRY ent;
    s32 rslt;
    struct DIRENTRY *p;
    s32 retry;
    s32 i;
    s32 n;

    if (D_800EA960.cmd != 0) {
        printf(D_80013B08);
        return -1;
    }
    _make_file_discrpt(chan, path);
    strcat(path, name);
    retry = 0;
    rslt = 0;
    D_800EA960.mask |= 1 << chan;
    n = 0;
    for (i = 0; i < ofs + max; i++) {
        if (i == 0) {
            for (;;) {
                _clr_card_event();
                p = firstfile((s8 *)path, &ent);
                if (p != 0) {
                    break;
                }
                rslt = func_800A34BC(_get_card_event_x());
                if (rslt == 0) {
                    break;
                }
                retry++;
                if (retry >= 4) {
                    D_800EA9B8 = MemCardCallback(0);
                    mcrd_accept(chan);
                    MemCardSync(0, 0, &rslt);
                    MemCardCallback(D_800EA9B8);
                    return rslt;
                }
            }
        } else {
            p = nextfile(&ent);
        }
        if (p == 0) {
            break;
        }
        if (i >= ofs && dir != 0) {
            dir[n] = ent;
            n++;
        }
    }
    if (files != 0) {
        *files = n;
    }
    return 0;
}

/* cc97_lib: MemCardCallback */

/* libmcrd MemCardCallback (LIBMCRD.OBJ): install `func` as the function the VSync
   callback (func_800A3510) calls with (command, result) when a command completes - the
   record's func member (+0x44) - and return the one that was installed before.
   MemCardOpen, MemCardGetDirentry and MemCardDeleteFile call it with 0 to keep
   the user's function quiet while they wait for their own MemCardAccept, and put
   the old value (parked in D_800EA9B8) back afterwards. */
/* func_800A2CFC */
MemCB MemCardCallback(MemCB func)
{
    MemCB old;

    old = D_800EA960.func;
    D_800EA960.func = func;
    return old;
}

/* cc97_lib: MemCardSync */

/* libmcrd MemCardSync(mode, cmds, rslt) (LIBMCRD.OBJ): wait for, or poll, the
   command the last asynchronous call started. Returns -1 when none is registered
   (cmd, +0, is 0 and the completion flag done, +8, is clear). mode 0 waits in a
   busy loop for the VSync callback to set done; any other mode returns 0 while it
   is clear, after storing the running command and its result so far (read from
   the record before the test) through the non-null pointers. Once done is set:
   the finished command and its result are copied from the last-result pair
   D_800EA950 (rslt first, then cmds), done is cleared and the function returns 1.
   MemCardDeleteFile carries the same statements inlined. */
/* func_800A2D10 */
s32 MemCardSync(s32 mode, s32 *cmds, s32 *rslt)
{
    s32 cmd;
    s32 res;

    if (D_800EA960.cmd == 0 && D_800EA960.done == 0) {
        return -1;
    }
    cmd = D_800EA960.cmd;
    res = D_800EA960.rslt;
    if (mode == 0) {
        while (D_800EA960.done == 0) {
        }
        if (rslt != 0) {
            *rslt = D_800EA950[1];
        }
        if (cmds != 0) {
            *cmds = D_800EA950[0];
        }
        D_800EA960.done = 0;
        return 1;
    }
    if (D_800EA960.done == 0) {
        if (rslt != 0) {
            *rslt = res;
        }
        if (cmds != 0) {
            *cmds = cmd;
        }
        return 0;
    }
    if (rslt != 0) {
        *rslt = D_800EA950[1];
    }
    if (cmds != 0) {
        *cmds = D_800EA950[0];
    }
    D_800EA960.done = 0;
    return 1;
}
/* cc97_lib: MemCardCreateFile */
/* reorder_fill: MemCardCreateFile */
/* return_slot_fill: MemCardCreateFile */
s32 _card_create2(s32 chan, char *name, s32 blocks);

/* libmcrd MemCardCreateFile(chan, file, blocks) (LIBMCRD.OBJ): create a file of
   `blocks` blocks on the card in port chan, synchronously. While a command is
   running (cmd, +0, not 0) it prints "Access Denied. : system busy" (D_80013B08)
   and returns -1. Otherwise it builds "buXX:" + file in a local buffer
   (_make_file_discrpt, _make_file_discrpt; strcat) which nothing reads
   afterwards, sets the record's bit for the card (+0xC |= 1 << chan) and calls
   _card_create2(chan, file, blocks) with the bare name. Result 0
   returns 0 (McErrNone); -1 gives 7 (McErrBlockFull), -2 gives 4 (McErrNotFormat),
   -3 gives 6 (McErrAlreadyExist), 4 gives 2 (McErrCardInvalid); anything else goes
   through _rslt_to_code (func_800A34BC).
   MARKED SPELLING: `while ((rslt = _card_create2(...)) != 0) { ...every path
   returns... } return 0;`, a loop statement that never comes round. The call and
   the tests written straight-line score 46/63. The object has the three
   `return 7 / 4 / 6` blocks moved out in front of the buffer code, which loop.c
   does only for a block that leaves a loop, and keeps the mask store ahead of the
   call's argument moves, which the scheduler does only across a loop's start; the
   4.0 build of the function has a real retry loop at this spot, as
   MemCardDeleteFile still has its four-try open() loop. What the
   4.5 source's loop statement looked like is not known. */
/* func_800A2E2C */
s32 MemCardCreateFile(s32 chan, char *file, s32 blocks)
{
    char name[32];
    s32 rslt;

    if (D_800EA960.cmd != 0) {
        printf(D_80013B08);
        return -1;
    }
    _make_file_discrpt(chan, name);
    strcat(name, file);
    D_800EA960.mask |= 1 << chan;
    while ((rslt = _card_create2(chan, file, blocks)) != 0) {
        if (rslt == -1) {
            return 7;
        }
        if (rslt == -2) {
            return 4;
        }
        if (rslt == -3) {
            return 6;
        }
        if (rslt == 4) {
            return 2;
        }
        return func_800A34BC(rslt);
    }
    return 0;
}

/* cc97_lib: MemCardDeleteFile */
/* reorder_fill: MemCardDeleteFile */
/* return_slot_fill: MemCardDeleteFile */
extern char D_80013B08[];
s32 printf();
void _make_file_discrpt(s32, char *);
char *strcat(char *, char *);

static inline s32 mcrd_sync(s32 mode, s32 *cmds, s32 *rslt)
{
    /* MemCardSync's source (MemCardSync), inlined at its three uses. */
    s32 c;
    s32 r;

    if (D_800EA960.cmd == 0 && D_800EA960.done == 0) {
        return -1;
    }
    c = D_800EA960.cmd;
    r = D_800EA960.rslt;
    if (mode == 0) {
        while (D_800EA960.done == 0) {
        }
        if (rslt != 0) {
            *rslt = D_800EA950[1];
        }
        if (cmds != 0) {
            *cmds = D_800EA950[0];
        }
        D_800EA960.done = 0;
        return 1;
    }
    if (D_800EA960.done == 0) {
        if (rslt != 0) {
            *rslt = r;
        }
        if (cmds != 0) {
            *cmds = c;
        }
        return 0;
    }
    if (rslt != 0) {
        *rslt = D_800EA950[1];
    }
    if (cmds != 0) {
        *cmds = D_800EA950[0];
    }
    D_800EA960.done = 0;
    return 1;
}
/* libmcrd MemCardDeleteFile(chan, file) (LIBMCRD.OBJ, SDK 4.5 build): delete file
   `file` on the card in port chan, synchronously. Returns -1 (with a printf of
   D_80013B08) when a command is running (cmd, +0), else a McErr code.

   The path is _make_file_discrpt's "buXY:" (_make_file_discrpt) with the name appended
   (strcat); bit chan of the state's +0xC mask is set. The file is
   opened for writing (BIOS open, mode 1), up to four tries. If it
   never opens the card is checked: with the user's MemCardCallback masked
   (MemCardCallback(0), the old one kept in D_800EA9B8 and put back after) an inlined
   MemCardAccept(chan) is run and waited for with an inlined MemCardSync(0, 0, &rslt);
   a result of 0 (McErrNone) or 3 (McErrNewCard) then becomes 5 (McErrFileNotExist),
   any other result is returned as it is.

   Otherwise the file is closed (close) and the delete loop runs:
   _clr_card_event, BIOS delete (erase) - 1 means done,
   return 0 - then the card event is fetched (_get_card_event_x) and decides:
     0  the same masked MemCardAccept + MemCardSync; returns 4 (McErrNotFormat) if
        that is its result, else 0;
     1  twice { _clr_card_event; _card_info(chan); fetch the event },
        then the masked MemCardAccept + MemCardSync, and the delete is tried again -
        on the fourth such round in a row it returns 2 (McErrCardInvalid);
     4  the error count is cleared, _clr_card_event, _card_clear(chan)
        , the event is fetched and dropped, and it tries again;
     any other event counts an error: the fourth returns _rslt_to_code(event)
     (func_800A34BC). Events 0, 4 and the others clear the round counter of event 1.

   Shape: MemCardAccept and MemCardSync are INLINED in the original (the dead
   volatile reads of cmd and rslt, the `if (rslt != 0)` store with a known pointer);
   MemCardCallback is a real call. The two helpers above carry their source: the
   printf arm first, which is also what the standalone MemCardAccept compiles from.
   `chan` (+0x10) is the one plain member of the state record - inlined, its store
   goes through the symbol (lui $at / sw), where the volatile members keep the
   base register. The open retry tests the assignment's value, so the test after
   the loop stays (written `fd = open(); if (fd >= 0) break;` it is folded away and
   fd moves to $v0). Every way out of the switch that is not a retry `break`s to
   one `return rslt;`, which is why the result is stored and read back. */
/* func_800A2F28 */
s32 MemCardDeleteFile(s32 chan, char *file)
{
    char path[32];
    s32 rslt;
    s32 nerr;
    s32 nretry;
    s32 i;
    s32 fd;

    nerr = 0;
    nretry = 0;
    rslt = 0;
    if (D_800EA960.cmd != 0) {
        printf(D_80013B08);
        return -1;
    }
    _make_file_discrpt(chan, path);
    strcat(path, file);
    D_800EA960.mask |= 1 << chan;
    for (i = 0; i < 4; i++) {
        if ((fd = open(path, 1)) >= 0) {
            break;
        }
    }
    if (fd < 0) {
        D_800EA9B8 = MemCardCallback(0);
        mcrd_accept(chan);
        mcrd_sync(0, 0, &rslt);
        MemCardCallback(D_800EA9B8);
        if (rslt == 0 || rslt == 3) {
            rslt = 5;
        }
        return rslt;
    }
    close(fd);
    for (;;) {
        _clr_card_event();
        rslt = erase(path);
        if (rslt == 1) {
            return 0;
        }
        rslt = _get_card_event_x();
        switch (rslt) {
        case 0:
            nretry = 0;
            D_800EA9B8 = MemCardCallback(0);
            mcrd_accept(chan);
            mcrd_sync(0, 0, &rslt);
            MemCardCallback(D_800EA9B8);
            if (rslt != 4) {
                rslt = 0;
            }
            break;
        case 1:
            for (i = 0; i < 2; i++) {
                _clr_card_event();
                _card_info(chan);
                rslt = _get_card_event_x();
            }
            D_800EA9B8 = MemCardCallback(0);
            mcrd_accept(chan);
            mcrd_sync(0, 0, &rslt);
            MemCardCallback(D_800EA9B8);
            nretry++;
            if (nretry < 4) {
                continue;
            }
            rslt = 2;
            break;
        case 4:
            nretry = 0;
            nerr = 0;
            _clr_card_event();
            _card_clear(chan);
            _get_card_event_x();
            continue;
        default:
            nretry = 0;
            nerr++;
            if (nerr < 4) {
                continue;
            }
            rslt = func_800A34BC(rslt);
            break;
        }
        break;
    }
    return rslt;
}
