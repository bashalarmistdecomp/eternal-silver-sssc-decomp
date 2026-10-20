/* PSY-Q 4.5 libcard, member a79.o: _card_read. The first split had it in
   psyq_misc_aaj.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
s32 _get_card_event_x(void);
/* cc97_lib: _get_card_event */
/* return_slot_fill: _get_card_event */
void bzero(void *, s32);
void _clr_card_event(void);
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

/* func_800A4CE8 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", _card_read);
