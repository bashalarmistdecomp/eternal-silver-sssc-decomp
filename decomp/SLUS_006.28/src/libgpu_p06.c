/* PSY-Q 4.5 libgpu, member p06.o: AddPrim. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: AddPrim */

/* libgpu AddPrim(ot, p) (P06.OBJ; PRIM.OBJ before 4.2): links the primitive p in
   at the head of the ordering-table entry ot. p's 24-bit tag address takes ot's,
   then ot's takes p; the length byte of both tags is kept. The body is libgpu.h's
   addPrim(ot, p), written out: setaddr(p, getaddr(ot)), setaddr(ot, p). */
/* func_8008E9D8 */
void AddPrim(void *ot, void *p)
{
    ((P_TAG *)p)->addr = (u_long)((P_TAG *)ot)->addr, ((P_TAG *)ot)->addr = (u_long)p;
}

INCLUDE_PAD(4);
