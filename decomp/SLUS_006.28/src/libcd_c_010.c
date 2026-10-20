/* PSY-Q 4.5 libcd, member c_010.o: StSetMask. The first split had it in
   psyq_spu_c.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s32 D_800E70F4;
extern void (*D_800E9570)(), (*D_800E9574)();

/* far_syms: D_800E70D4 (target reaches it %hi/%lo) */
/* far_syms: D_800E70F4 (target reaches it %hi/%lo) */
/* far_syms: D_800E70F8 (target reaches it %hi/%lo) */
/* far_syms: D_800E70D4 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800E70F4 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800E70F8 (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 D_800E70D4;
extern s32 D_800E70F8;

/* cc97_lib: StSetMask */
/* return_slot_fill: StSetMask */
/* libcd StSetMask(mask, start, end) (C_010.OBJ): store the three streaming words:
   D_800E70F8 = mask, D_800E70D4 = start (the first frame wanted), StEndFrame
   (D_800E70F4) = end. */
/* func_80093D08 */
void StSetMask(u_long mask, u_long start, u_long end) {
    D_800E70F8 = mask;
    D_800E70D4 = start;
    D_800E70F4 = end;
}
INCLUDE_PAD(4);
