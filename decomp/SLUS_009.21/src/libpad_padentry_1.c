/* PSY-Q 4.5 libpad, member padentry.o, part 1 of 3: PadChkVsync,
   PadStartCom, PadStopCom, PadChkMtap. The first split had it in main_as.c.
   The tree compiles this run of the member's functions with other flags
   than the runs beside it (config/cc_flags.txt), so the member is in
   several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
void PadStopCom(void);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* epilogue_load_slot: PadChkVsync */
/* func_8012C4E0 */
int PadChkVsync(void) {
    _padChkVsync();
}

/* epilogue_load_slot: PadStartCom */
/* func_8012C500 */
void PadStartCom(void) {
    return _padStartCom();
}

/* epilogue_load_slot: PadStopCom */
/* func_8012C520 */
void PadStopCom(void) {
    _padStopCom();
}

/* func_8012C540 */
INCLUDE_ASM("asm/nonmatchings/main", PadChkMtap);
