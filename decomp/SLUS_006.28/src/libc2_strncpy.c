/* PSY-Q 4.5 libc2, member strncpy.o: strncpy. The first split had it in
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
char *strncpy(char *, char *, s32);

/* cc97_lib: strncpy */

/* libc2 strncpy (STRNCPY.OBJ): copy at most n characters of src to dst. Once the
   terminating NUL has been copied the rest of the n characters are zero filled; when
   src is n characters or longer dst is left unterminated. Returns dst, or 0 without
   touching anything when either pointer is null. n <= 0 copies nothing and returns
   dst. Caller in the image: _card_create2 (libcard _card_create2), which copies the
   file name into a directory frame with n = 20. */
/* func_800A4C78 */
char *strncpy(char *dst, char *src, s32 n)
{
    char *ret;
    s32 i;

    if (dst == 0 || src == 0) {
        return 0;
    }
    ret = dst;
    for (i = 0; i < n; i++) {
        if ((*dst++ = *src++) == 0) {
            while (++i < n) {
                *dst++ = 0;
            }
            return ret;
        }
    }
    return ret;
}
