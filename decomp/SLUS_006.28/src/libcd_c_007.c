/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/c_007.c)
   and psyz (decomp/src/libcd/c_007.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member c_007.o: StFreeRing. The first split had it in
   psyq_spu_c.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s32 D_800E7100;
extern volatile s32 D_800E7104;
extern void (*D_800E9570)(), (*D_800E9574)();

/* cc97_lib: StFreeRing */
extern s32 D_800E70EC;

/* libcd StFreeRing (C_007.OBJ): give a frame's ring sectors back.  base is the frame's
   data address as StGetNext returned it; its header index is n = (base - (StRingAddr
   (D_800E7100) + StRingSize (D_800E7104) * 32)) / 504 words (the data area follows the
   StRingSize 32-byte headers, 504 words a sector).  Returns 1 if header n is not in
   state 4 (handed out); otherwise clears the id of headers n .. n + nSectors - 1,
   sets StRingIdx3 (D_800E70EC, the reader's index) to n + nSectors and returns 0.
   The ring base is an integer in the source: the header address is formed with
   integer arithmetic and `<< 5` (a pointer index or `* 32` gives other operand orders),
   and it is reloaded for every header cleared. */
/* func_80093B58 */
u_long StFreeRing(u_long *base)
{
    s32 n;
    s32 i;
    s16 nSectors;
    StHEADER *hdr;

    n = (base - (u_long *)(D_800E7100 + (D_800E7104 << 5))) / 504;
    hdr = (StHEADER *)(D_800E7100 + (n << 5));
    nSectors = hdr->nSectors;
    if ((s16)hdr->id != 4) {
        return 1;
    }
    for (i = 0; i < nSectors; i++) {
        *(s16 *)(D_800E7100 + ((i + n) << 5)) = 0;
    }
    D_800E70EC = i + n;
    return 0;
}

INCLUDE_PAD(4);
