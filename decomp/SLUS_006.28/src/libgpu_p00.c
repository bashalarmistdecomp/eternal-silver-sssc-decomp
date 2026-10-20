/* PSY-Q 4.5 libgpu, member p00.o: GetTPage. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: GetTPage */

/* libgpu GetTPage(tp, abr, x, y) (P00.OBJ; PRIM.OBJ before 4.2): the texture-page
   id for pixel mode tp, semi-transparency rate abr and the page at frame-buffer
   position (x, y): tp in bits 7-8, abr in bits 5-6, y's bit 8 in bit 4, x / 64
   in bits 0-3, y's bit 9 in bit 11. The expression is libgpu.h's getTPage(tp,
   abr, x, y), written out (the header's macros are not visible to the
   per-function re-scoring of this file). */
/* func_8008E978 */
u_short GetTPage(int tp, int abr, int x, int y)
{
    return ((tp & 0x3) << 7) | ((abr & 0x3) << 5) | ((y & 0x100) >> 4) | ((x & 0x3ff) >> 6) |
           ((y & 0x200) << 2);
}

INCLUDE_PAD(4);
