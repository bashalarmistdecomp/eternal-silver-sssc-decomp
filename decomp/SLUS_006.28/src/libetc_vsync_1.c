/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libetc/vsync.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libetc, member vsync.o, part 1 of 2: VSync. The first split had
   it in psyq_spu_e.c. The tree compiles this run of the member's functions
   with other flags than the runs beside it (config/cc_flags.txt), so the
   member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
extern volatile s32 *D_800B6838;
extern volatile s32 *D_800B683C;
extern volatile s32 D_800B6840;
extern volatile s32 D_800B6844;
extern volatile s32 D_800B7970;
/* cc97_lib: func_80094670 */
/* gnu_as_loads: func_80094670 */
/* return_slot_fill: func_80094670 */
/* reorder_fill: func_80094670 */
static inline int cd_itob(int n)
{
    return ((n / 10) << 4) + (n % 10);
}

/* cc97_lib: VSync */
/* return_slot_fill: VSync */
/* far_syms: D_800B6838 (target reaches it %hi/%lo) */
/* far_syms: D_800B683C (target reaches it %hi/%lo) */
/* far_syms: D_800B6840 (target reaches it %hi/%lo) */
/* far_syms: D_800B6844 (target reaches it %hi/%lo) */
/* far_syms: D_800B7970 (target reaches it %hi/%lo) */
void func_80094B20(s32, s32);

/* libetc VSync (VSYNC.OBJ): wait for vertical sync.  Reads GPUSTAT through D_800B6838
   (-> 0x1F801814) and the horizontal-blank counter through D_800B683C (-> root counter
   1, 0x1F801110), the counter twice until two reads agree.  ret = (counter - Hcount
   (D_800B6840)) & 0xFFFF, the horizontal blanks since the last VSync.  mode < 0:
   returns Vcount (D_800B7970, the vertical-blank count the interrupt handler keeps).
   mode == 1: returns ret without waiting.  Otherwise v_wait (func_80094B20) until
   Vcount reaches the last VSync's count (D_800B6844) for mode 0, or that count
   + mode - 1 for mode n (time-out mode - 1 or 0), then v_wait for one more vertical
   blank; in interlace (GPUSTAT bit 22) it then spins until the field bit (31)
   differs from the one read before that blank.  Saves Vcount in D_800B6844 and the
   counter in Hcount (again read until stable) and returns ret.
   MARKED SPELLING: `n = 1;` and `D_800B6844 - n + mode` hold the constant 1 in a
   variable.  It is psyz's spelling of this function (sotn-decomp's is `waitCount = 1;
   waitCount = (prev - waitCount) + mode;`) and is semantically inert.  A literal 1
   scores 80/94: fold-const reassociates (prev - 1) + mode to prev + (mode - 1), which
   cse shares with the time-out argument, while the target computes prev - 1 first.
   The original construct is not known. */
/* func_800949A8 */
s32 VSync(s32 mode)
{
    volatile s32 h;
    s32 stat;
    s32 ret;
    s32 v;
    s32 timeout;
    s32 n;

    stat = *D_800B6838;
    do {
        h = *D_800B683C;
    } while (h != *D_800B683C);
    ret = (h - D_800B6840) & 0xFFFF;
    if (mode < 0) {
        return D_800B7970;
    }
    if (mode == 1) {
        return ret;
    }
    n = 1;
    v = mode > 0 ? D_800B6844 - n + mode : D_800B6844;
    timeout = mode > 0 ? mode - n : 0;
    func_80094B20(v, timeout);
    stat = *D_800B6838;
    func_80094B20(D_800B7970 + 1, 1);
    if (stat & 0x400000 && (stat ^ *D_800B6838) >= 0) {
        do {
        } while (!((stat ^ *D_800B6838) & 0x80000000));
    }
    D_800B6844 = D_800B7970;
    do {
        D_800B6840 = *D_800B683C;
    } while (D_800B6840 != *D_800B683C);
    return ret;
}
