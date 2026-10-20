/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libetc/intr_vb.c) and psy-q-decomp (src/etc/intr_vb.c), all
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libetc, member intr_vb.o, part 2 of 2: func_800953C0,
   setIntrVSync, func_80095458. The first split had it in psyq_spu_i.c. The
   tree compiles this run of the member's functions with other flags than
   the runs beside it (config/cc_flags.txt), so the member is in several
   files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800B7970 (target reaches it %hi/%lo) */
typedef void (*Callback)();     /* libetc INTR_VB.OBJ: a vertical-blank callback */
extern Callback D_800B7950[8];          /* INTR_VB.OBJ .data+0: the vertical-blank callback slots */
extern volatile s32 D_800B7970;         /* Vcount (INTR_VB.OBJ .data+0x20): the vertical-blank count */
/* cc97_lib: func_800953C0 */
/* return_slot_fill: func_800953C0 */

/* libetc trapIntrVSync (INTR_VB.OBJ, static): the handler startIntrVSync
    installs on interrupt 0, the vertical blank.  Adds one
   to Vcount (D_800B7970), then calls each of the eight callback slots
   (D_800B7950) that is not null, in order. */
void func_800953C0(void)
{
    int i;

    D_800B7970++;
    for (i = 0; i < 8; i++) {
        if (D_800B7950[i] != 0) {
            D_800B7950[i]();
        }
    }
}

/* cc97_lib: setIntrVSync */

/* libetc setIntrVSync (INTR_VB.OBJ, static): install `callback` in vertical-blank
   callback slot `index` (D_800B7950) and return the one it replaces; nothing is
   written when it is the same.  startIntrVSync returns this function's address. */
/* func_8009542C */
Callback setIntrVSync(int index, Callback callback)
{
    Callback prev = D_800B7950[index];

    if (callback != prev) {
        D_800B7950[index] = callback;
    }
    return prev;
}
/* cc97_lib: func_80095458 */

/* libetc memclr (INTR_VB.OBJ, static): clear `size` words from `ptr`.
   startIntrVSync clears the eight callback slots with it. */
void func_80095458(s32 *ptr, s32 size)
{
    s32 i;

    for (i = size - 1; i != -1; i--) {
        *ptr++ = 0;
    }
}
INCLUDE_PAD(12);
