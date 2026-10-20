/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/c_005.c)
   and psyz (decomp/src/libcd/c_005.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member c_005.o: StSetStream. The first split had it in
   psyq_spu_c.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* cc97_lib: StSetStream */
/* return_slot_fill: StSetStream */
extern s32 D_800E70C0;
extern s16 D_800E70C4;
extern s32 D_800E70C8;
extern s32 D_800E70D0;
extern s32 D_800E70D8;
extern s32 D_800E70F0;
extern void (*D_800E9570)(), (*D_800E9574)();

/* libcd StSetStream (C_005.OBJ): set up a streaming session.  StSetMask(1, start_frame,
   end_frame) (StSetMask), then StEmu_Addr (D_800E70F0) = 0, StFunc1 (D_800E9570) =
   func1, StRgb24 (D_800E70C8) = mode & 1, CChannel (D_800E70D8) = 0, StCHANNEL
   (D_800E70D0) = 0, Stsector_offset (D_800E70C4, a short) = 0, Stframe_no (D_800E70C0)
   = 0 and StFunc2 (D_800E9574) = func2. */
/* func_80093AC8 */
void StSetStream(u_long mode, u_long start_frame, u_long end_frame, void (*func1)(), void (*func2)())
{
    StSetMask(1, start_frame, end_frame);
    D_800E70F0 = 0;
    D_800E9570 = func1;
    D_800E70C8 = mode & 1;
    D_800E70D8 = 0;
    D_800E70D0 = 0;
    D_800E70C4 = 0;
    D_800E70C0 = 0;
    D_800E9574 = func2;
}

INCLUDE_PAD(12);
