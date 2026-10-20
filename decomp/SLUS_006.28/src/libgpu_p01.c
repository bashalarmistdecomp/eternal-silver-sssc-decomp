/* PSY-Q 4.5 libgpu, member p01.o: GetClut. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */
u_short GetClut(int x, int y);
/* cc97_lib: GetClut */

/* libgpu GetClut(x, y) (P01.OBJ; PRIM.OBJ before 4.2): the CLUT id of the colour
   table at frame-buffer position (x, y): y in bits 6-15, x / 16 in bits 0-5.
   The expression is libgpu.h's getClut(x, y), written out; the 16-bit result
   type is what masks it. */
/* func_8008E9B8 */
u_short GetClut(int x, int y)
{
    return (y << 6) | ((x >> 4) & 0x3f);
}

INCLUDE_PAD(8);
