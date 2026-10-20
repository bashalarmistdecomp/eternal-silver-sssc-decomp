/* Lunar SSSC — screen_fade: the fade of the whole screen, its start and its
   frame. The first split had it in ui.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
extern s16 g_ScreenFadeFramesLeft;
extern s16 g_ScreenFadeLength;
extern u8 g_ScreenFadeMode;
extern s16 g_ScreenFadeColByte0;
extern s16 g_ScreenFadeColByte1;
extern s16 g_ScreenFadeColByte2;
void screen_fade_start(s16 length, s8 mode);
void packet_box_add(s32, s32, s16, s16, s32, s32);

/* func_8001C0CC */
void screen_fade_start(s16 length, s8 mode) {
    g_ScreenFadeFramesLeft = length;
    g_ScreenFadeLength = length;
    g_ScreenFadeMode = mode;
    g_ScreenFadeColByte0 = 0xFF;
    g_ScreenFadeColByte1 = 0xFF;
    g_ScreenFadeColByte2 = 0xFF;
}
s32 packet_tpage_add(s32, s32);
volatile int value_div(s32, s16);

static inline int inline_fn_func_8001C0F0()
{
  return ((u8) g_ScreenFadeMode) & 2;
}
/* func_8001C0F0 */
void screen_fade_step(void)
{
  s32 tpage;
  s32 green;
  s32 step;
  s32 blue;
  s32 red;
  s32 col;
  if (g_ScreenFadeFramesLeft != 0)
  {
    red = g_ScreenFadeColByte0;
    green = g_ScreenFadeColByte1;
    blue = g_ScreenFadeColByte2;
    if (g_ScreenFadeLength <= 0)
    {
      g_ScreenFadeLength = 1;
    }
    step = (((u8) g_ScreenFadeMode) & 1) ? ((g_ScreenFadeLength - g_ScreenFadeFramesLeft) - 1) : g_ScreenFadeFramesLeft;
    if (((!(((u8) g_ScreenFadeMode) & 1)) || (g_ScreenFadeFramesLeft != 1)) && (g_ScreenFadeLength != 0))
    {
      red = value_div(red * step, g_ScreenFadeLength);
      green = value_div(green * step, g_ScreenFadeLength);
      blue = value_div(blue * step, g_ScreenFadeLength);
    }
    if (red < 0)
    {
      red = 0;
    }
    if (green < 0)
    {
      green = 0;
    }
    if (blue < 0)
    {
      blue = 0;
    }
    if (red >= 0x101)
    {
      red = 0xFF;
    }
    if (green >= 0x101)
    {
      green = 0xFF;
    }
    if (blue >= 0x101)
    {
      blue = 0xFF;
    }
    col = (red | (green << 8)) | (blue << 0x10);
    if (((u8) g_ScreenFadeMode) & 2)
    {
      if (col == 0xFFFFFF)
      {
        packet_box_add(0, 0, 0x140, 0xE0, 0, 8);
      }
      else
      {
        goto block_25;
      }
    }
    else
    {
      block_25:
      packet_box_add(0, 0, 0x140, 0xE0, col | 0x02000000, 8);

      tpage = 0x220;
      if (inline_fn_func_8001C0F0())
      {
        tpage = 0x240;
      }
      packet_tpage_add(tpage, 8);
    }
    g_ScreenFadeFramesLeft = ((u16) g_ScreenFadeFramesLeft) - 1;
  }
}
