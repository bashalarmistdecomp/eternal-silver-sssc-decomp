/* Lunar SSSC — tpage: a texture page's place and its pixels sent to video
   memory. The first split had it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"

/* func_800204D0 */
void tpage_position_set(tpage, rect)
u16 tpage;
void *rect;
{
  s16 *rect_x_then_y;
  s32 tpage_bits;
  tpage_bits = tpage & 0x1F;
  rect_x_then_y = (s16 *)(s32 *) (((u8 *) rect) + 0x0);
  *rect_x_then_y = (s16) ((tpage_bits & 0xF) << 6);
  *(rect_x_then_y = (s16 *) (((u8 *) rect) + 0x2)) = (s16) ((tpage_bits & 0x10) * 0x10);
}
extern long HWD0, VWD0;

/* func_800204F0 */
s32 tpage_image_load(s32 tpage, void *pixels)
{
  u16 rect[4];
  tpage_position_set(tpage, rect);
  rect[2] = 0x80;
  rect[3] = 0x100;
  LoadImage(rect, pixels);
  return DrawSync(0);
}
/* func_8002053C */
s32 tpage_small_image_load(s32 tpage, void *pixels)
{
  u16 rect[4];
  tpage_position_set(tpage, rect);
  rect[2] = 0x40;
  rect[3] = 0x80;
  LoadImage(rect, pixels);
  return DrawSync(0);
}
