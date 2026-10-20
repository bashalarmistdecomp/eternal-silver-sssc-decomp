/* Lunar SSSC — pad_record: a pad record (repeat delay, held buttons) made
   ready and brought up to date. The first split had it in rendering2.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "tile.h"
#include "pad.h"
#include "palette.h"
#include "chr.h"
#include "btl.h"
#include "sprite.h"
extern long HWD0;
extern long VWD0;
extern long HWD0, VWD0;
void mem_clear(void *destination, u32 byte_count);  /* extern */

/* far_syms: g_PadHeldButtons g_PadSecondHeldButtons */
/* Start a pad record: the owner calls this once before it begins polling
   the record with pad_record_update (below). The whole 0x30-byte record is
   zeroed (mem_clear, this TU's zero fill), then cur = held and cur2 =
   held2, and every bit set in held / held2 has its timer / timer2 set to
   delay / 2. Nothing else is read or written; no global is touched.
   The effect is on the first poll: pad_record_update moves cur to prev, so a
   button named in `held` that is still down then is "still held" and not a
   new press (no press bit), and its repeat bit comes only once the delay /
   2 timer has run out (at once when delay < 2). A button named in `held`
   that is up at the first poll has its timer zeroed there and behaves
   normally from then on. The callers pass the buttons their own loop is
   about to test, so a button still down from the screen before does not
   answer the new prompt. `delay` is used for those timers only: with held
   = held2 = 0 the call is a plain clear whatever delay is.
   16 call sites, held2 = 0 at every one: the records g_BtlPadRecord (delay 2,
   held 0; twice), g_BtlMenuPadRecord (delay 0x10 or 0, held 0x40; four),
   g_MenuPadRecord (delay 0x10, 0x20 or 0x14, held 0, 0x40 or 0xF0D0; five) and
   the text engine's g_MessagePadRecord (delay 0x14, held 0x40, 0xD0 or 0x5040;
   five). No return value (the other TUs' prototypes say s32; no caller
   reads one). One callee, mem_clear; no frame wait and no RNG
   draw on the call path.
   Shapes: the three value parameters are narrow (u8, u16, u16) and the bit
   is a u16 variable, as in pad_record_update. A u16 parameter is copied to a
   second register at entry and the first copy is what the two `sh` store;
   with the u16 bit the 0xFFFF mask falls on the result of each AND and not
   on the shifted 1. Written `held & (1 <<
   i)` the test becomes `(held >> i) & 1` (15/45); an s32 bit is 18/45; s32
   held / held2 are 20/45, an s32 delay 35/45 (the 0xFF mask is the
   parameter's own conversion, which lands after the call). The `srlv` by
   the register that holds the loop's 1 comes from the plain `delay >> 1`
   by itself (the post-reload cse); no variable
   shift count is written. The definition also matches old-style, and with
   an s32 return type and no return statement. */
/* func_8002687C */
void pad_record_init(PadRecord *rec, u8 delay, u16 held, u16 held2)
{
    s32 i;
    u16 bit;

    mem_clear(rec, sizeof(PadRecord));
    rec->held = held;
    rec->held_2 = held2;
    for (i = 0; i < 16; i++) {
        bit = 1 << i;
        if (held & bit) {
            rec->timers[i] = delay >> 1;
        }
        if (held2 & bit) {
            rec->timers_2[i] = delay >> 1;
        }
    }
}

extern u16 g_PadHeldButtons;
extern u16 g_PadSecondHeldButtons;
s32 pad_buttons_get(void);

/* Refresh a pad record from the current pad word; whoever owns the record
   calls this once per poll. cur / prev shift along, press = the bits that
   went down since the last call, and repeat is the typematic word: a bit
   is set on the call it goes down (its timer takes `delay`), and again
   every time the timer has run out while it stays held (the timer then
   takes delay / 2, so the first repeat comes after delay + 1 calls and the
   rest every delay / 2 + 1). A released bit's timer is 0. The pad word is
   also mirrored to g_PadHeldButtons. */
/* func_80026930 */
void pad_record_update(PadRecord *rec, u8 delay)
{
    s32 i;
    u16 bit;
    s32 pad;

    pad = pad_buttons_get();
    rec->held_before = rec->held;
    rec->held = pad;
    g_PadHeldButtons = pad;
    rec->held_before_2 = rec->held_2;
    rec->held_2 = g_PadSecondHeldButtons;
    rec->repeated_2 = 0;
    rec->repeated = 0;
    rec->pressed = rec->held & (rec->held ^ rec->held_before);
    rec->pressed_2 = rec->held_2 & (rec->held_2 ^ rec->held_before_2);
    for (i = 0; i < 16; i++) {
        bit = 1 << i;
        if (rec->held & bit) {
            if (rec->held & rec->held_before & bit) {
                if (rec->timers[i] != 0) {
                    rec->timers[i]--;
                } else {
                    rec->timers[i] = delay >> 1;
                    rec->repeated |= bit;
                }
            } else {
                rec->timers[i] = delay;
                rec->repeated |= bit;
            }
        } else {
            rec->timers[i] = 0;
        }
        if (rec->held_2 & bit) {
            if (rec->held_2 & rec->held_before_2 & bit) {
                if (rec->timers_2[i] != 0) {
                    rec->timers_2[i]--;
                } else {
                    rec->timers_2[i] = delay >> 1;
                    rec->repeated_2 |= bit;
                }
            } else {
                rec->timers_2[i] = delay;
                rec->repeated_2 |= bit;
            }
        } else {
            rec->timers_2[i] = 0;
        }
    }
}
