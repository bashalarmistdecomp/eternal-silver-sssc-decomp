/* PSY-Q 4.5 libapi, member counter.o, part 2 of 2: StartRCnt, StopRCnt,
   ResetRCnt. The first split had it in psyq_cd_f.c. The tree compiles this
   run of the member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* far_syms: D_800B6100 (target reaches it %hi/%lo) */
/* far_syms: D_800B6108 (target reaches it %hi/%lo) */
extern volatile s32 *D_800B6100;        /* COUNTER.OBJ's pointer to the interrupt registers
                                           (0x1F801070: status, then mask) */
extern s32 D_800B6108[];                /* the counters' interrupt bits: 0x10, 0x20, 0x40, 1 */

/* cc97_lib: StartRCnt */
/* gnu_as_loads: StartRCnt */
/* return_slot_fill: StartRCnt */

/* libapi StartRCnt (COUNTER.OBJ): enables root counter spec & 0xFFFF's interrupt by
   setting its bit (D_800B6108[index]) in the interrupt mask register. The index is not
   checked before the table is read; the return value is 1 when it is below 3, else 0. */
/* func_8008F33C */
long StartRCnt(unsigned long spec)
{
    s32 i = spec & 0xFFFF;

    D_800B6100[1] |= D_800B6108[i];
    return i < 3;
}
/* cc97_lib: StopRCnt */
/* gnu_as_loads: StopRCnt */
/* return_slot_fill: StopRCnt */

/* libapi StopRCnt (COUNTER.OBJ): disables root counter spec & 0xFFFF's interrupt by
   clearing its bit (D_800B6108[index]) in the interrupt mask register. Always returns 1. */
/* func_8008F36C */
long StopRCnt(unsigned long spec)
{
    s32 i = spec & 0xFFFF;

    D_800B6100[1] &= ~D_800B6108[i];
    return 1;
}

/* far_syms: D_800B6104 (target reaches it %hi/%lo) */

/* One root counter's registers, 0x10 bytes each from 0x1F801100 (the layout and the names
   are sotn-decomp's libapi/counter.c `Counter`, in psyq_private.h). */
extern volatile Counter *D_800B6104;    /* COUNTER.OBJ's `_counters` pointer (0x1F801100) */

/* cc97_lib: ResetRCnt */

/* libapi ResetRCnt (COUNTER.OBJ): sets the count of root counter spec & 0xFFFF to 0.
   Returns 1, or 0 with nothing written when the index is 3 or more. */
/* func_8008F3A0 */
long ResetRCnt(unsigned long spec)
{
    s32 i = spec & 0xFFFF;

    if (i >= 3) {
        return 0;
    }
    D_800B6104[i].rootCounter = 0;
    return 1;
}

INCLUDE_PAD(4);
