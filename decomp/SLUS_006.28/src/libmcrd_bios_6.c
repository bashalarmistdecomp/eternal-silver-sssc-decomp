/* PSY-Q 4.5 libmcrd, member bios.o, part 6 of 7: _clr_card_event. The first
   split had it in psyq_misc_aai.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800EAA40 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA44 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA48 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA4C (target reaches it %hi/%lo) */
/* far_syms: D_800EAA50 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA54 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA58 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA5C (target reaches it %hi/%lo) */
/* far_syms: D_800EAA60 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA64 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA68 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA6C (target reaches it %hi/%lo) */
/* far_syms: D_800EAA70 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA74 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA78 (target reaches it %hi/%lo) */
/* far_syms: D_800EAA7C (target reaches it %hi/%lo) */
/* far_syms: D_800EAA40 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA44 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA48 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA4C (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA50 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA54 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA58 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA5C (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA60 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA64 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA68 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA6C (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA70 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA74 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA78 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800EAA7C (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 D_800EAA40;
extern s32 D_800EAA44;
extern s32 D_800EAA48;
extern s32 D_800EAA4C;
extern s32 D_800EAA50;
extern s32 D_800EAA54;
extern s32 D_800EAA58;
extern s32 D_800EAA5C;
extern volatile s32 D_800EAA60;
extern volatile s32 D_800EAA64;
extern volatile s32 D_800EAA68;
extern volatile s32 D_800EAA6C;
extern volatile s32 D_800EAA70;
extern volatile s32 D_800EAA74;
extern volatile s32 D_800EAA78;
extern volatile s32 D_800EAA7C;

/* cc97_lib: _clr_card_event */
/* libmcrd _clr_card_event (BIOS.OBJ): drop every pending card event. TestEvent
    on the eight event ids (the four SwCARD events D_800EAA40 .. 4C, then
   the four HwCARD events D_800EAA50 .. 5C), then both sets of handler flags are
   cleared (D_800EAA60 .. 6C and D_800EAA70 .. 7C; each chain stores the last flag and
   reads every stored flag back for the next, the flags being volatile). */
/* func_800A3EA8 */
void _clr_card_event(void)
{
    TestEvent(D_800EAA40);
    TestEvent(D_800EAA44);
    TestEvent(D_800EAA48);
    TestEvent(D_800EAA4C);
    TestEvent(D_800EAA50);
    TestEvent(D_800EAA54);
    TestEvent(D_800EAA58);
    TestEvent(D_800EAA5C);
    D_800EAA60 = D_800EAA64 = D_800EAA68 = D_800EAA6C = 0;
    D_800EAA70 = D_800EAA74 = D_800EAA78 = D_800EAA7C = 0;
}
