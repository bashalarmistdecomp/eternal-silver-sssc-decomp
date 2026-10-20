/* PSY-Q 4.5 libgpu, member p26.o: SetTile. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetTile */

/* libgpu SetTile(p) (P26.OBJ; PRIM.OBJ before 4.2): initialises a free-size tile:
   tag length 3 words, code 0x60. The body is libgpu.h's setTile(p), written out. */
/* func_8008EA88 */
void SetTile(TILE *p)
{
    ((P_TAG *)p)->len = (u_char)3, ((P_TAG *)p)->code = (u_char)0x60;
}

INCLUDE_PAD(12);
