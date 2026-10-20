/* SPDX-License-Identifier: MIT
   Parts of this file match code of psyz (decomp/src/libcd/c_009.c),
   MIT-licensed: the notices are in LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member c_009.o: StGetNext. The first split had it in
   psyq_spu_c.c.
   The build must stay byte-matching. */

/* far_syms: D_800E70F4 (target reaches it %hi/%lo) */
/* far_syms: D_800E70F4 (auto: target reaches it %hi/%lo — addressing row 3) */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s32 D_800E70F4;
extern s32 D_800E7100;
extern volatile s32 D_800E7104;
extern void (*D_800E9570)(), (*D_800E9574)();
extern s32 D_800E70EC;

/* cc97_lib: StGetNext */

/* libcd StGetNext (C_009.OBJ): hand the next complete frame to the caller.  Looks at
   the header at StRingIdx3 (D_800E70EC) in the ring StRingAddr (D_800E7100).  State 1
   (the writer wrapped here): StRingIdx3 = 0, the header is freed (id = 0) when
   StEndFrame (D_800E70F4) is set, and the header at the new index is looked at.
   State 2 (frame complete): id = 4 (handed out), *addr = the frame's data, StRingAddr
   + StRingSize (D_800E7104) * 32 + StRingIdx3 * 2016 bytes, *header = the header,
   returns 0.  Any other state returns 1 (no frame yet).  The header pointer and
   StRingSize are volatile (sotn-decomp's c_009.c declares the same two): the load of
   StRingSize stays behind the store of id = 4. */
/* func_80093C48 */
u_long StGetNext(u_long **addr, u_long **header)
{
    volatile StHEADER *ptr;

    ptr = (volatile StHEADER *)(D_800E7100 + (D_800E70EC << 5));
    if (ptr->id == 1) {
        D_800E70EC = 0;
        if (D_800E70F4 != 0) {
            ptr->id = 0;
        }
        ptr = (volatile StHEADER *)(D_800E7100 + (D_800E70EC << 5));
    }
    if (ptr->id == 2) {
        ptr->id = 4;
        *addr = (u_long *)(D_800E7100 + (D_800E7104 << 5)) + D_800E70EC * 0x1F8;
        *header = (u_long *)ptr;
        return 0;
    } else {
        return 1;
    }
}

INCLUDE_PAD(8);
