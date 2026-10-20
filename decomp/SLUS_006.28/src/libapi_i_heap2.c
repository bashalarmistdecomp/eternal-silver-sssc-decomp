/* PSY-Q 4.5 libapi, member i_heap2.o: InitHeap2. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"

/* far_syms: D_800E55B8 (target reaches it %hi/%lo) */
/* far_syms: D_800E55BC (target reaches it %hi/%lo) */
/* far_syms: D_800E55C0 (target reaches it %hi/%lo) */
/* far_syms: D_800E55C4 (target reaches it %hi/%lo) */
/* far_syms: D_800E55B8 (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800E55BC (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: D_800E55C0 (auto: target reaches it %hi/%lo — addressing row 3) */
/* The malloc2 heap's state (libapi I_HEAP2.OBJ's .bss; names from the object's
   relocations). All four point at block header words. */
extern u_long *D_800E55B8;      /* _TopAllocArea: the heap's first block */
extern u_long *D_800E55BC;      /* _AllocRestBlockTop: one word past the end-marker header,
                                   where the unallocated rest begins */
extern u_long *D_800E55C0;      /* _max_heap: the heap's end */
extern u_long *D_800E55C4;      /* _Candidate: where malloc2 starts its walk; 0 = at the
                                   first block */
/* cc97_lib: InitHeap2 */
/* return_slot_fill: InitHeap2 */

/* libapi InitHeap2(head, size) (I_HEAP2.OBJ): set up the malloc2 heap in the size
   bytes at head. The heap's first block (_TopAllocArea, D_800E55B8) is head, its
   header gets the end marker -2, the heap's end (_max_heap, D_800E55C0) is
   size / 4 + 1 words past head (size counted in whole words), the first-fit
   candidate (_Candidate, D_800E55C4) is cleared and the unallocated rest
   (_AllocRestBlockTop, D_800E55BC) starts one word past head. */
/* func_8008B438 */
void InitHeap2(u_long *head, u_long size)
{
    D_800E55BC = head;
    D_800E55B8 = head;
    *head = -2;
    D_800E55C0 = head + (size >> 2) + 1;
    D_800E55C4 = 0;
    D_800E55BC++;
}
