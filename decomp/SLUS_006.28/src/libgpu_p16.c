/* PSY-Q 4.5 libgpu, member p16.o: SetPolyF4. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetPolyF4 */

/* libgpu SetPolyF4(p) (P16.OBJ; PRIM.OBJ before 4.2): initialises a flat quadrangle:
   tag length 5 words, code 0x28. The body is libgpu.h's setPolyF4(p), written
   out. */
/* func_8008EA68 */
void SetPolyF4(POLY_F4 *p)
{
    ((P_TAG *)p)->len = (u_char)5, ((P_TAG *)p)->code = (u_char)0x28;
}

INCLUDE_PAD(12);
