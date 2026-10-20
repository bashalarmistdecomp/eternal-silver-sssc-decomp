/* PSY-Q 4.5 libgpu, member p29.o: SetLineF3. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetLineF3 */

/* libgpu SetLineF3(p) (P29.OBJ; PRIM.OBJ before 4.2): initialises a flat two-segment
   line: tag length 5 words, code 0x48, and the terminator word pad = 0x55555555.
   The body is libgpu.h's setLineF3(p), written out. */
/* func_8008EAA8 */
void SetLineF3(LINE_F3 *p)
{
    ((P_TAG *)p)->len = (u_char)5, ((P_TAG *)p)->code = (u_char)0x48, p->pad = 0x55555555;
}
