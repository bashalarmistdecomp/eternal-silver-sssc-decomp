/* PSY-Q 4.5 libmcrd, member bios.o, part 7 of 7: _get_card_event,
   _get_card_event_x, _chk_card_event, _chk_card_event_x. The first split
   had it in psyq_misc_aaj.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
s32 _get_card_event_x(void);
extern volatile s32 D_800EAA70;
extern volatile s32 D_800EAA74;
extern volatile s32 D_800EAA78;
extern volatile s32 D_800EAA7C;
s32 _get_card_event(void);
/* cc97_lib: _get_card_event */
/* return_slot_fill: _get_card_event */
extern s32 D_800EAA50;
extern s32 D_800EAA54;
extern s32 D_800EAA58;
extern s32 D_800EAA5C;
extern volatile s32 D_800EAA60;
extern volatile s32 D_800EAA64;
extern volatile s32 D_800EAA68;
extern volatile s32 D_800EAA6C;

/* libmcrd _get_card_event (BIOS.OBJ): wait for a card event of the first set and take
   it. It spins until one of the four flags D_800EAA60 / 64 / 68 / 6C is set (the
   handlers of the SwCARD events for I/O end, error, time-out and new card,
   funcEvSpIOE .. funcEvSpNewcard, set them), calls TestEvent on the
   four HwCARD event ids D_800EAA50 .. 5C, clears the four flags and returns the flags
   as a bit mask (bit 0 I/O end, 1 error, 2 time-out, 3 new card) shifted right once:
   0 = I/O end, 1 = error, 2 = time-out, 4 = new card. */
/* func_800A3FB0 */
s32 _get_card_event(void)
{
    s32 ret;

    do {
        ret = D_800EAA60 + D_800EAA64 * 2 + D_800EAA68 * 4 + D_800EAA6C * 8;
    } while (ret == 0);
    TestEvent(D_800EAA50);
    TestEvent(D_800EAA54);
    TestEvent(D_800EAA58);
    TestEvent(D_800EAA5C);
    D_800EAA60 = D_800EAA64 = D_800EAA68 = D_800EAA6C = 0;
    return ret >> 1;
}
/* cc97_lib: _get_card_event_x */
/* return_slot_fill: _get_card_event_x */
extern s32 D_800EAA40;
extern s32 D_800EAA44;
extern s32 D_800EAA48;
extern s32 D_800EAA4C;

/* libmcrd _get_card_event_x (BIOS.OBJ): wait for a card event of the second set and
   take it. It spins until one of the four flags D_800EAA70 / 74 / 78 / 7C is set (the
   handlers of the HwCARD events for I/O end, error, time-out and new card,
   funcEvSpIOEx .. funcEvSpNewcardx, set them), calls TestEvent on the
   four SwCARD event ids D_800EAA40 .. 4C, clears the four flags and returns the flags
   as a bit mask (bit 0 I/O end, 1 error, 2 time-out, 3 new card) shifted right once:
   0 = I/O end, 1 = error, 2 = time-out, 4 = new card. */
/* func_800A4088 */
s32 _get_card_event_x(void)
{
    s32 ret;

    do {
        ret = D_800EAA70 + D_800EAA74 * 2 + D_800EAA78 * 4 + D_800EAA7C * 8;
    } while (ret == 0);
    TestEvent(D_800EAA40);
    TestEvent(D_800EAA44);
    TestEvent(D_800EAA48);
    TestEvent(D_800EAA4C);
    D_800EAA70 = D_800EAA74 = D_800EAA78 = D_800EAA7C = 0;
    return ret >> 1;
}

/* cc97_lib: _chk_card_event */
extern volatile s32 D_800EAA60;
extern volatile s32 D_800EAA64;
extern volatile s32 D_800EAA68;
extern volatile s32 D_800EAA6C;

/* libmcrd _chk_card_event (BIOS.OBJ): the pending card events as a bit mask, without
   waiting and without clearing them: the four flags the event handlers
   funcEvSpIOE / funcEvSpError / funcEvSpTimeout / funcEvSpNewcard set
   (D_800EAA60 / 64 / 68 / 6C) as bits 0 / 1 / 2 / 3. 0 = nothing has arrived. */
/* func_800A4160 */
s32 _chk_card_event(void)
{
    return D_800EAA60 + D_800EAA64 * 2 + D_800EAA68 * 4 + D_800EAA6C * 8;
}

/* cc97_lib: _chk_card_event_x */

/* libmcrd _chk_card_event_x (BIOS.OBJ): _chk_card_event for the second set of flags,
   the ones the handlers funcEvSpIOEx / funcEvSpErrorx / funcEvSpTimeoutx /
   funcEvSpNewcardx set (D_800EAA70 / 74 / 78 / 7C). */
/* func_800A419C */
s32 _chk_card_event_x(void)
{
    return D_800EAA70 + D_800EAA74 * 2 + D_800EAA78 * 4 + D_800EAA7C * 8;
}
