/* PSY-Q 4.5 libgpu, member p10.o: SetSemiTrans. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetSemiTrans */

/* libgpu SetSemiTrans(p, abe) (P10.OBJ; PRIM.OBJ before 4.2): sets (abe != 0) or
   clears bit 1 of the primitive's code byte, the semi-transparency bit. The body
   is libgpu.h's setSemiTrans(p, abe), written out. */
/* func_8008EA18 */
void SetSemiTrans(void *p, int abe)
{
    abe ? (((P_TAG *)p)->code = (u_char)((u_char)((P_TAG *)p)->code | 0x02))
        : (((P_TAG *)p)->code = (u_char)((u_char)((P_TAG *)p)->code & ~0x02));
}

INCLUDE_PAD(8);
