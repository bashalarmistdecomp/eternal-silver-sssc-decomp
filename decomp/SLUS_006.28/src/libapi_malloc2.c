/* PSY-Q 4.5 libapi, member malloc2.o: malloc2, _ExpAndAlloc. The first
   split had it in psyq_gpu_b.c.
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
extern u_long *D_800E55B8;
extern u_long *D_800E55BC;
extern u_long *D_800E55C0;
extern u_long *D_800E55C4;

/* cc97_lib: malloc2 */
/* return_slot_fill: malloc2 */
u_long *_ExpAndAlloc(u_long size);  /* TU defines this below */

/* libapi malloc2 (MALLOC2.OBJ): first-fit allocation from the InitHeap2 heap.
   A block is one header word and its user words; the header holds the user size
   in bytes with bit 0 set while the block is free, and -2 marks the end of the
   list. size is rounded up to a word. The walk starts at _Candidate (the heap's
   first block when that is 0). A free block of exactly size is taken; a larger
   one is split, the remainder staying free; a smaller one is merged with a free
   successor, or dropped back into the unallocated rest when it is the last block
   (its header becomes the end marker), or stepped over. At the end marker
   _ExpAndAlloc (_ExpAndAlloc) carves a new block from the rest; when that fails
   the walk is run once more from the heap's first block, then the result is 0.
   Returns the user area. */
/* func_8008B488 */
u_long *malloc2(u_long size)
{
    u_long *p;
    u_long *q;
    u_long *r;
    u_long f;
    s32 retry;

    retry = 1;
    if (size == 0) {
        return 0;
    }
    size += 3;
    size &= ~3;
    f = size | 1;
    p = D_800E55C4;
    if (p == 0) {
        p = D_800E55B8;
    }
    for (;;) {
        if (*p & 1) {
            if (D_800E55C4 == 0) {
                D_800E55C4 = p;
            }
            if (*p == f) {
                *p &= ~1;
                q = p + 1;
                break;
            }
            if (*p > size) {
                q = p + 1;
                r = p + (size >> 2) + 1;
                *r = *p - size - 4;
                *p = size;
                break;
            }
            if (*p < size) {
                q = p + (*p >> 2) + 1;
                if (*q == -2) {
                    *p = -2;
                    D_800E55BC = p + 1;
                } else if (*q & 1) {
                    *p += (*q & ~1) + 4;
                } else {
                    p = q;
                }
            }
        } else if (*p != -2) {
            p = p + (*p >> 2) + 1;
        } else {
            r = _ExpAndAlloc(size);
            if (r != 0) {
                D_800E55C4 = D_800E55BC - 1;
                q = r + 1;
                break;
            }
            if (--retry < 0) {
                q = 0;
                break;
            }
            p = D_800E55B8;
            D_800E55C4 = 0;
        }
    }
    return q;
}

/* cc97_lib: _ExpAndAlloc */

/* libapi _ExpAndAlloc (MALLOC2.OBJ; a static from SDK 4.1 on, named in the 3.6.1 to
   4.0 signatures):
   carve a block of size bytes from the unallocated rest of the heap. Returns 0
   when the rest (its whole words, less two header words) is smaller than size;
   otherwise writes size into the header one word below _AllocRestBlockTop, moves
   _AllocRestBlockTop past the block and the next header, writes the end marker
   -2 into that header and returns the new block's header. */
/* func_8008B640 */
u_long *_ExpAndAlloc(u_long size)
{
    u_long *p;
    u_long *top;

    if (size > (D_800E55C0 - D_800E55BC) * sizeof(u_long) - 8) {
        return 0;
    }
    top = D_800E55BC;
    p = top - 1;
    *p = size;
    top += (size >> 2) + 1;
    D_800E55BC = top;
    top[-1] = -2;
    return p;
}

INCLUDE_PAD(8);
