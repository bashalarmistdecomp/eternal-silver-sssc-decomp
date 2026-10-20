/* PSY-Q 4.5 libgpu, member p12.o: SetPolyF3. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetPolyF3 */

/* libgpu SetPolyF3(p) (P12.OBJ; PRIM.OBJ before 4.2): initialises a flat triangle:
   tag length 4 words, code 0x20. The body is libgpu.h's setPolyF3(p), written
   out: setlen(p, 4), setcode(p, 0x20). */
/* func_8008EA48 */
void SetPolyF3(POLY_F3 *p)
{
    ((P_TAG *)p)->len = (u_char)4, ((P_TAG *)p)->code = (u_char)0x20;
}

INCLUDE_PAD(12);
