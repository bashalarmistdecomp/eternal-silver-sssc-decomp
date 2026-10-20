/* PSY-Q 4.5 libmcrd, member bios.o, part 1 of 7: funcEvSpIOE,
   funcEvSpError, funcEvSpTimeout, funcEvSpNewcard, funcEvSpIOEx,
   funcEvSpErrorx, funcEvSpTimeoutx, funcEvSpNewcardx. The first split had
   it in psyq_misc_aad.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: UserFuncComplete */

extern s32 D_800EAA60;

/* cc97_lib: funcEvSpIOE */
/* libmcrd BIOS.OBJ's eight card-event handlers (funcEvSpIOE, funcEvSpError,
   funcEvSpTimeout, funcEvSpNewcard for the SwCARD events, then the same four with an x
   for the HwCARD events; _card_start opens the events with them): each sets its own flag
   word (D_800EAA60 ... D_800EAA7C), which _get_card_event / _get_card_event_x and their
   _chk twins poll, and returns 0. */
/* func_800A3B28 */
s32 funcEvSpIOE(void) {
    D_800EAA60 = 1;
    return 0;
}
extern s32 D_800EAA64;

/* cc97_lib: funcEvSpError */
/* func_800A3B3C */
s32 funcEvSpError(void) {
    D_800EAA64 = 1;
    return 0;
}
extern s32 D_800EAA68;

/* cc97_lib: funcEvSpTimeout */
/* func_800A3B50 */
s32 funcEvSpTimeout(void) {
    D_800EAA68 = 1;
    return 0;
}
extern s32 D_800EAA6C;

/* cc97_lib: funcEvSpNewcard */
/* func_800A3B64 */
s32 funcEvSpNewcard(void) {
    D_800EAA6C = 1;
    return 0;
}
extern s32 D_800EAA70;

/* cc97_lib: funcEvSpIOEx */
/* func_800A3B78 */
s32 funcEvSpIOEx(void) {
    D_800EAA70 = 1;
    return 0;
}
extern s32 D_800EAA74;

/* cc97_lib: funcEvSpErrorx */
/* func_800A3B8C */
s32 funcEvSpErrorx(void) {
    D_800EAA74 = 1;
    return 0;
}
extern s32 D_800EAA78;

/* cc97_lib: funcEvSpTimeoutx */
/* func_800A3BA0 */
s32 funcEvSpTimeoutx(void) {
    D_800EAA78 = 1;
    return 0;
}
extern s32 D_800EAA7C;

/* cc97_lib: funcEvSpNewcardx */
/* func_800A3BB4 */
s32 funcEvSpNewcardx(void) {
    D_800EAA7C = 1;
    return 0;
}
