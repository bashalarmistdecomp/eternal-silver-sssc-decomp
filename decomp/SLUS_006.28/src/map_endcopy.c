/* Lunar SSSC — map_endcopy: MapFrameEndCopy (one call of MapFrameEnd, which
   is map_frame.c's) and an empty function. The first split had it in
   misc_i.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

void MapFrameEnd(void);

/* A bare call-through to MapFrameEnd; its callers (one each in btl_window.c,
   map_fade.c, map_frame.c, menu_run.c, movie.c, profile.c and shop_screen.c)
   pass nothing and do not read a result. */
/* Ruling of 2026-10-14 (docs/naming.md): Does what MapFrameEnd does; nothing read says why there are two. */
/* func_800568AC */
void MapFrameEndCopy(void)
{
    MapFrameEnd();
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_800568CC() {
}
