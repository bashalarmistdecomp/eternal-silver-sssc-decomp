/* PSY-Q 4.5 libmcrd, member bios.o, part 3 of 7: _card_start. The first
   split had it in psyq_misc_aaf.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800EAA40 */
/* far_syms: D_800EAA44 */
/* far_syms: D_800EAA48 */
/* far_syms: D_800EAA4C */
/* far_syms: D_800EAA50 */
/* far_syms: D_800EAA54 */
/* far_syms: D_800EAA58 */
/* far_syms: D_800EAA5C */
extern s32 D_800EAA40, D_800EAA44, D_800EAA48, D_800EAA4C, D_800EAA50, D_800EAA54, D_800EAA58, D_800EAA5C;
s32 EnterCriticalSection();
s32 _clr_card_event();
void funcEvSpIOE();
void funcEvSpError();
void funcEvSpTimeout();
void funcEvSpNewcard();
void funcEvSpIOEx();
void funcEvSpErrorx();
void funcEvSpTimeoutx();
void funcEvSpNewcardx();

/* cc97_lib: _card_start */
/* return_slot_fill: _card_start */
/* libmcrd _card_start (BIOS.OBJ): memory-card event setup: in a
   critical section (EnterCriticalSection / ExitCriticalSection), open the eight card
   events with OpenEvent(class, spec, 0x1000 = EvMdINTR, handler) —
   SwCARD 0xF4000001 and HwCARD 0xF0000011, each for spec 4 / 0x8000 /
   0x100 / 0x2000 (IOE / ERROR / TIMOUT / NEW), handlers funcEvSpIOE..BB4
   — keep their ids in D_800EAA40..5C, enable each (EnableEvent), then
   _clr_card_event. _card_stop closes the same eight. */
/* func_800A3BF8 */
void _card_start(void)
{
    s32 crit;

    crit = EnterCriticalSection();
    D_800EAA40 = OpenEvent(0xF4000001, 4, 0x1000, funcEvSpIOE);
    D_800EAA44 = OpenEvent(0xF4000001, 0x8000, 0x1000, funcEvSpError);
    D_800EAA48 = OpenEvent(0xF4000001, 0x100, 0x1000, funcEvSpTimeout);
    D_800EAA4C = OpenEvent(0xF4000001, 0x2000, 0x1000, funcEvSpNewcard);
    D_800EAA50 = OpenEvent(0xF0000011, 4, 0x1000, funcEvSpIOEx);
    D_800EAA54 = OpenEvent(0xF0000011, 0x8000, 0x1000, funcEvSpErrorx);
    D_800EAA58 = OpenEvent(0xF0000011, 0x100, 0x1000, funcEvSpTimeoutx);
    D_800EAA5C = OpenEvent(0xF0000011, 0x2000, 0x1000, funcEvSpNewcardx);
    EnableEvent(D_800EAA40);
    EnableEvent(D_800EAA44);
    EnableEvent(D_800EAA48);
    EnableEvent(D_800EAA4C);
    EnableEvent(D_800EAA50);
    EnableEvent(D_800EAA54);
    EnableEvent(D_800EAA58);
    EnableEvent(D_800EAA5C);
    _clr_card_event();
    if (crit == 1) {
        ExitCriticalSection();
    }
}
