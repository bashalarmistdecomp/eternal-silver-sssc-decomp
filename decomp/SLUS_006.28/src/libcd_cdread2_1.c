/* PSY-Q 4.5 libcd, member cdread2.o, part 1 of 2: CdRead2. The first split
   had it in psyq_spu.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* return_slot_fill: func_800931A0 */
/* cc97_lib: func_800931A0 */
/* reorder_fill: func_800931A0 */
/* far_syms: D_800B6730 (target reaches it %hi/%lo) */
extern volatile int D_800B6730[];
typedef void (*Callback)();
s32 VSync(s32);
s32 VSync(s32);
static inline s32 cd_read_wait(void)
{
    /* CDREAD.OBJ's inlined wait for a pause it sent (CdReadBreak, CdRead): 1 at once when
       none is pending (state word [9]); else spin until the pause's sync callback
       (func_80092EF8) clears the flag, 0; after 120 vertical blanks give up: put the saved
       sync callback [10] back, clear the flag, -1 */
    s32 t;

    if (D_800B6730[9] == 0) {
        return 1;
    }
    t = VSync(-1);
    while (D_800B6730[9] != 0) {
        if ((u32) (VSync(-1) - t) > 120) {
            CdSyncCallback((CdlCB) D_800B6730[10]);
            D_800B6730[9] = 0;
            return -1;
        }
    }
    return 0;
}
s32 VSync(s32);
s32 VSync(s32);

/* cc97_lib: CdRead2 */
/* reorder_fill: CdRead2 */
/* return_slot_fill: CdRead2 */
void func_8009394C(void);           /* CDREAD2.OBJ's static: a bare call of StCdInterrupt */
void data_ready_callback(void);           /* data_ready_callback (C_004.OBJ) */
extern s32 D_800E7138;              /* StMode */

/* libcd CdRead2(mode) (CDREAD2.OBJ): start a streaming read. CdlSetmode (0xE) with the
   mode's low byte; when the mode has CdlModeStream (0x100): StMode (D_800E7138) = 0 if the
   mode has bit 0x20, else 1, data_ready_callback becomes the data
   callback (CdDataCallback) and the object's static func_8009394C the
   ready callback (CdReadyCallback); then CdlReadS (0x1B). */
/* func_800938C8 */
int CdRead2(long mode)
{
    u8 param;

    param = mode;
    CdControl(0xE, (s8 *)&param, 0);
    if (mode & 0x100) {
        if (mode & 0x20) {
            D_800E7138 = 0;
        } else {
            D_800E7138 = 1;
        }
        CdDataCallback((Callback)data_ready_callback);
        CdReadyCallback((CdlCB)func_8009394C);
    }
    return CdControl(0x1B, 0, 0);
}
