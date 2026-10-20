/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libcd/c_002.c), psyz (decomp/src/libcd/c_002.c) and
   psy-q-decomp (src/cd/c_002.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member c_002.o: StClearRing. The first split had it in
   psyq_spu_b.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: func_8009394C */

/* cc97_lib: StClearRing */

void init_ring_status(s32 start, u32 count);
extern s32 D_800E70C0;
extern s16 D_800E70C4;
extern s32 D_800E70CC;
extern s32 D_800E70DC;
extern s32 D_800E70E4;
extern s32 D_800E70E8;
extern s32 D_800E70EC;
extern s32 D_800E7104;

/* libcd StClearRing (C_002.OBJ): empty the streaming ring. The ring indices StRingIdx3 /
   2 / 1 (D_800E70EC / E8 / E4) and StFinalSector (D_800E70DC) are zeroed,
   init_ring_status zeroes the first word of all StRingSize (D_800E7104)
   sector headers from slot 0, then StCdIntrFlag (D_800E70CC), Stsector_offset
   (D_800E70C4) and Stframe_no (D_800E70C0) are zeroed. */
/* func_80093978 */
void StClearRing(void)
{
    D_800E70EC = 0;
    D_800E70E8 = 0;
    D_800E70E4 = 0;
    D_800E70DC = 0;
    init_ring_status(0, D_800E7104);
    D_800E70CC = 0;
    D_800E70C4 = 0;
    D_800E70C0 = 0;
}
