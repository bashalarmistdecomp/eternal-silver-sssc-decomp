/* Lunar SSSC — message_draw: what the message window draws: its shade and
   the arrow that bobs while it waits. The first split had it in
   rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
extern long HWD0;
extern long VWD0;
extern s8 g_MessageShadeHeight;
s32 packet_tpage_add(s32, s32);
void packet_box_add(s32 left_x, s32 top_y, s32 width, s32 height, s32 col, s32 z);
s32 screen_fade_step();
s32 packet_tpage_add(s32, s32);
void MessageShadeDraw(void);
void MessageShadeSet(s8 height);
s32 MessageArrowPhaseFold(s32 phase);
void packet_box_add(s32, s32, s32, s32, s32, s32);
s32 screen_fade_step();
s32 packet_tpage_add(s32, s32);
void MessageShadeDraw(void);
void MessageShadeSet(s8 height);
s32 MessageArrowPhaseFold(s32 phase);
extern long HWD0, VWD0;
void packet_triangle_down_add(s32 x, s32 y, s32 w, s32 h, s32 color, s32 ot);
extern long HWD0, VWD0;

void packet_box_add(s32, s32, s32, s32, s32, s32);          /* extern */
s32 screen_fade_step();                                  /* extern */
s32 packet_tpage_add(s32, s32);                              /* extern */

/* func_80015E88 */
void MessageShadeDraw(void) {
    if (g_MessageShadeHeight != 0) {
        packet_box_add(0, 0xB8 - (g_MessageShadeHeight * 4), 0x140, g_MessageShadeHeight * 8, 0x02000000, 0x40);
        packet_tpage_add(0x1F, 0x40);
    }
    screen_fade_step();
}

/* func_80015EE8 */
void MessageShadeSet(s8 height) {
    g_MessageShadeHeight = height;
}
/* func_80015EF4 */
s32 MessageArrowPhaseFold(s32 phase) {
    s32 folded_phase;

    folded_phase = phase;
    if (folded_phase >= 0x20) {
        folded_phase = 0x40 - folded_phase;
    }
    return folded_phase;
}
extern s32 g_MessageArrowPhase;

/* The message window's "more" arrow: a small bobbing triangle.

   g_MessageArrowPhase is its phase, 0..0x3F, stepped once per call and private to
   this function.  MessageArrowPhaseFold folds the phase into a triangle wave
   (0..0x20..0), so y gains 0..4 and loses it again over 64 calls.
   packet_triangle_down_add queues the flat triangle (x, y) - (x + 2w, y) -
   (x + w, y + h): 8 wide, 5 high, colour 0x20F0F0, OT slot 0x10. */
/* func_80015F0C */
void MessageArrowDraw(s32 x, s32 y)
{
  s32 color = 0x20F0F0;

  y += MessageArrowPhaseFold(g_MessageArrowPhase) >> 3;
  packet_triangle_down_add(x, y, 4, 5, color, 0x10);
  g_MessageArrowPhase++;
  if (g_MessageArrowPhase >= 0x40)
  {
    g_MessageArrowPhase = 0;
  }
}
