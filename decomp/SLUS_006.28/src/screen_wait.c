/* Lunar SSSC — screen_wait: the display switched on or off and two blanks
   waited. The first split had it in rendering2.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "tile.h"
#include "pad.h"
#include "palette.h"
#include "chr.h"
#include "btl.h"
#include "sprite.h"
s32 screen_disp_mask_set(s32);                                 /* extern */
s32 VSync(s32);                                 /* extern */

/* func_8002603C */
void screen_on_wait(void) {
    screen_disp_mask_set(1);
    VSync(0);
    VSync(0);
}

/* func_8002606C */
void screen_off_wait(void) {
    screen_disp_mask_set(0);
    VSync(0);
    VSync(0);
}
