/* PSY-Q 4.5 libgpu, member p33.o: SetDrawTPage. The first split had it in
   psyq_gpu_h.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: LoadClut */
/* return_slot_fill: LoadClut */

/* cc97_lib: SetDrawTPage */

/* libgpu SetDrawTPage(p, dfe, dtd, tpage) (P33.OBJ; in PRIM.OBJ from 3.5 to 4.1):
   initialises a texture-page change primitive: tag length 1 word, and the one
   command word 0xE1000000 | 0x200 when dtd (dithering) is set | 0x400 when dfe
   (drawing to the displayed area) is set | tpage & 0x9FF. The body is libgpu.h's
   setDrawTPage(p, dfe, dtd, tpage), written out; the macro itself writes the
   command word as ((u_long *)(p))[1]. */
/* func_8008EAC8 */
void SetDrawTPage(DR_TPAGE *p, int dfe, int dtd, int tpage)
{
    ((P_TAG *)p)->len = (u_char)1,
    ((u_long *)p)[1] = 0xe1000000 | (dtd ? 0x0200 : 0) | (dfe ? 0x0400 : 0) | (tpage & 0x9ff);
}

INCLUDE_PAD(4);
