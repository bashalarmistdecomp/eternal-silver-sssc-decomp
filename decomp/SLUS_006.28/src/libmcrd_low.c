/* PSY-Q 4.5 libmcrd, member low.o: _card_format2, _card_create2. The first
   split had it in psyq_misc_aaj.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
s32 _get_card_event_x(void);
s32 _get_card_event(void);
/* cc97_lib: _get_card_event */
/* return_slot_fill: _get_card_event */

/* One directory frame of a memory card, as _card_create2 keeps the 15 of
   them (32 of each frame's 128 bytes): state 0x51/0x52/0x53 = first /
   middle / last block of a file, 0xA0..0xAF = free; next = the file's
   following block or 0xFFFF. */

extern u8 D_800EACC0[];
extern CardDir D_800EAA80[];
extern s8 D_800EACB0[];
extern char D_80013C14[];

void bzero(void *, s32);
void _clr_card_event(void);

/* Read frame `blk` of card `chan` into `buf`: up to 8 tries, and after a
   try that ends in status 4, _card_clear on the card before the next.
   Returns the last status; 0 = read. */
static inline s32 card_frame_read(s32 chan, s32 blk, u8 *buf)
{
    s32 i = 0;
    s32 ret;

    bzero(buf, 0x80);
    for (; i < 8; i++) {
        _clr_card_event();
        _card_read(chan, blk, buf);
        ret = _get_card_event_x();
        if (ret == 0) {
            break;
        }
        if (ret == 4) {
            _clr_card_event();
            _card_clear(chan);
            _get_card_event_x();
        }
    }
    return ret;
}

/* Write `buf` as frame `blk` of card `chan` with its XOR checksum in
   byte 127; same retry rule as card_frame_read. */
static inline s32 card_frame_write(s32 chan, s32 blk, u8 *buf)
{
    s32 i = 0;
    u8 *p = buf;
    u8 sum = 0;
    s32 ret = 0;
    s32 c;

    for (c = 0; c < 0x7F; c++) {
        sum ^= *p++;
    }
    *p = sum;
    do {
        _clr_card_event();
        _card_write(chan, blk, buf);
        ret = _get_card_event_x();
        if (ret == 0) {
            break;
        }
        if (ret == 4) {
            _clr_card_event();
            _card_clear(chan);
            _get_card_event_x();
        }
        i++;
    } while (i < 8);
    return ret;
}
/* cc97_lib: _card_format2 */
/* gnu_as_loads: _card_format2 */
/* return_slot_fill: _card_format2 */
extern s32 D_800EAC60[];

/* _card_format2 (PSY-Q libmcrd, LOW.OBJ): format card `chan`. Writes the 15
   directory frames 1..15, each a cleared entry with state 0xA0 (free), size
   0 and next 0xFFFF, into the table D_800EAA80 and out to the card; then
   the 20 broken-sector frames 16..35, each holding one -1 from D_800EAC60;
   then the header frame 0, "MC". Every frame goes out through
   card_frame_write (XOR checksum in byte 127, up to 8 tries, a try that ends
   in status 4 clears the card first). Last, _card_load +
   _get_card_event, up to 8 times: 0 as soon as one succeeds;
   after a failure _card_clear and _get_card_event_x
    before the next try. Returns the first failing
   frame write's status, or the last load status. */
/* func_800A41E8 */
s32 _card_format2(s32 chan)
{
    s32 k = 0;
    s32 i;
    s32 ret;

    for (i = 0; i < 15; i++) {
        bzero(D_800EACC0, 0x80);
        bzero(&D_800EAA80[i], 0x20);
        D_800EAA80[i].state = 0xA0;
        D_800EAA80[i].size = 0;
        D_800EAA80[i].next = 0xFFFF;
        memcpy(D_800EACC0, &D_800EAA80[i], 0x20);
        ret = card_frame_write(chan, i + 1, D_800EACC0);
        if (ret != 0) {
            goto end;
        }
    }
    for (i = 0; i < 20; i++) {
        D_800EAC60[i] = -1;
        bzero(D_800EACC0, 0x80);
        memcpy(D_800EACC0, &D_800EAC60[i], 4);
        ret = card_frame_write(chan, i + 0x10, D_800EACC0);
        if (ret != 0) {
            goto end;
        }
    }
    bzero(D_800EACC0, 0x80);
    D_800EACC0[0] = 'M';
    D_800EACC0[1] = 'C';
    ret = card_frame_write(chan, 0, D_800EACC0);
    if (ret != 0) {
        goto end;
    }
    do {
        _clr_card_event();
        _card_load(chan);
        ret = _get_card_event();
        if (ret == 0) {
            return 0;
        }
        _clr_card_event();
        _card_clear(chan);
        _get_card_event_x();
        k++;
    } while (k < 8);
end:
    return ret;
}

/* cc97_lib: _card_create2 */
/* gnu_as_loads: _card_create2 */
/* return_slot_fill: _card_create2 */
s32 strcmp(char *, char *);
char *strncpy(char *, char *, s32);
s32 printf();
s32 _get_card_event(void);

/* _card_create2 (PSY-Q libmcrd, SDK 4.5+): create `name` on card `chan`
   with `blocks` 8 KB blocks. Reads the header frame (must start "MC") and
   the 15 directory frames, marks every block reachable from a first block
   (state 0x51) through the next links as used and frees the rest (0xA0);
   a live file of the same name prints D_80013C14 and returns -3, too few
   free blocks returns -1. Then claims the first free blocks (0x51 first,
   0x52 middle, 0x53 last when more than one, next = 0xFFFF at the end),
   writes every claimed frame back, last to first, and finishes with
   _card_load + _get_card_event, retried up to 8 times. A failed frame
   read or write returns that error. */
/* func_800A45B8 */
s32 _card_create2(s32 chan, char *name, s32 blocks)
{
    s32 n = 0;
    s32 k = 0;
    s32 i;
    s32 j;
    s32 ret;
    s32 prev;

    bzero(D_800EACC0, 0x80);
    ret = card_frame_read(chan, 0, D_800EACC0);
    if (ret != 0) {
        return ret;
    }
    if (D_800EACC0[0] != 'M' || D_800EACC0[1] != 'C') {
        return -2;
    }
    for (i = 0; i < 15; i++) {
        D_800EACB0[i] = 0;
        bzero(D_800EACC0, 0x80);
        ret = card_frame_read(chan, i + 1, D_800EACC0);
        if (ret != 0) {
            return ret;
        }
        memcpy(&D_800EAA80[i], D_800EACC0, 0x20);
    }
    for (i = 0; i < 15; i++) {
        if (D_800EAA80[i].state == 0x51) {
            D_800EACB0[i] = 1;
            j = i;
            while (D_800EAA80[j].next != 0xFFFF) {
                j = D_800EAA80[j].next;
                if ((u32)j >= 15) {
                    break;
                }
                D_800EACB0[j] = 1;
            }
        }
    }
    for (i = 0; i < 15; i++) {
        if (D_800EACB0[i] == 0) {
            D_800EAA80[i].state = 0xA0;
        }
    }
    for (i = 0; i < 15; i++) {
        if (D_800EAA80[i].state == 0x51 && strcmp(D_800EAA80[i].name, name) == 0) {
            printf(D_80013C14, D_800EAA80[i].name, name);
            return -3;
        }
    }
    for (i = 0; i < 15; i++) {
        D_800EACB0[i] = 0;
        if ((D_800EAA80[i].state & 0xF0) == 0xA0) {
            n++;
        }
    }
    if (n < blocks) {
        return -1;
    }
    n = 0;
    prev = 0;
    for (i = 0; i < 15; i++) {
        if ((D_800EAA80[i].state & 0xF0) == 0xA0) {
            if (n == 0) {
                D_800EAA80[i].state = 0x51;
                D_800EAA80[i].size = blocks << 13;
                strncpy(D_800EAA80[i].name, name, 0x14);
                prev = i;
                D_800EACB0[i] = 1;
            } else {
                D_800EAA80[prev].next = i;
                D_800EAA80[i].state = 0x52;
                prev = i;
                D_800EACB0[i] = 1;
            }
            n++;
            if (n >= blocks) {
                D_800EAA80[i].next = 0xFFFF;
                if (n >= 2) {
                    D_800EAA80[i].state = 0x53;
                }
                break;
            }
        }
    }
    for (i = 14; i >= 0; i--) {
        if (D_800EACB0[i] != 0) {
            bzero(D_800EACC0, 0x80);
            memcpy(D_800EACC0, &D_800EAA80[i], 0x20);
            ret = card_frame_write(chan, i + 1, D_800EACC0);
            if (ret != 0) {
                return ret;
            }
        }
    }
    do {
        _clr_card_event();
        _card_load(chan);
        ret = _get_card_event();
        if (ret == 0) {
            return 0;
        }
        _clr_card_event();
        _card_clear(chan);
        _get_card_event_x();
        k++;
    } while (k < 8);
    return ret;
}

INCLUDE_PAD(12);
