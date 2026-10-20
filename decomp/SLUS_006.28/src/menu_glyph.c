/* Lunar SSSC — menu_glyph: the menu's glyphs and numbers on the grid (plain
   and tall) and its wait for a button. The first split had it in
   scripting_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"

extern long HWD0;
extern long VWD0;
extern s32 g_MenuBackOutLatch;
extern s32 g_MenuBackOutPattern;
extern u16 g_MenuSoundRequest;
extern u8 g_MenuPadRecord[];
s32 MenuGlyphSet(s32 glyph, s32 column, s32 row);
s32 MenuTallGlyphSet(s32 glyph, s32 column, s32 row);
s32 MenuTallNumberSet(s32 value, s32 column, s32 row, s32 digit_count);
s32 MenuPadWait(void);
extern s32 (*vtbl_4658_table[])();
s32 MenuGlyphSet(s32 glyph, s32 column, s32 row);
s32 MenuTallGlyphSet(s32 glyph, s32 column, s32 row);
s32 MenuTallNumberSet(s32 value, s32 column, s32 row, s32 digit_count);
extern long HWD0, VWD0;
s32 SoundPlay(s32, s32);
s32 SoundPlay(s32, s32);

/* func_80069E60 */
s32 MenuGlyphSet(s32 glyph, s32 column, s32 row) {
    return CellGlyphSet(glyph & 0xFFFF, column, row, 0xDEAD2001, 0x3000, 0);
}

/* func_80069E94 */
s32 MenuTallGlyphSet(s32 glyph, s32 column, s32 row) {
    s32 var;
    var = CellGlyphSet(glyph & 0xFFFF, column, row, 0xDEAD2001, 0x3000, 1);
    return var;
}

/* func_80069ECC */
s32 MenuNumberSet(s32 value, s32 column, s32 row, s32 digit_count) {
    return CellNumberSet(value, column, row, digit_count, 0xDEAD2001, 0x3000, 0, 0);
}

/* func_80069F04 */
s32 MenuTallNumberSet(s32 value, s32 column, s32 row, s32 digit_count) {
    return CellNumberSet(value, column, row, digit_count, 0xDEAD2001, 0x3000, 1, 0);
}

/* far_syms: g_MenuPadRecord (target reaches it %hi/%lo) */

/* func_80069F40 */
s32 MenuPadWait(void)
{
  u16 flags;
  s32 result;

  result = 0;
  while (1)
  {
    pad_record_update(g_MenuPadRecord, 0x14);
    flags = ((PadRecord *)g_MenuPadRecord)->pressed;
    if (flags & 0x40)
    {
      g_MenuSoundRequest = 2;
      result = 1;
      break;
    }
    if (flags & 0x10)
    {
      g_MenuSoundRequest = 3;
      result = 0;
      break;
    }
    if (flags & 0x100)
    {
      g_MenuBackOutLatch = 1;
      g_MenuBackOutPattern = 0x30B30B30;
      g_MenuSoundRequest = 3;
      result = 0;
      break;
    }
    if (flags & 0x20)
    {
      g_MenuBackOutLatch = 2;
      g_MenuBackOutPattern = 0x30B30B30;
      g_MenuSoundRequest = 3;
      result = 0;
      break;
    }
    MenuFrameStep();
  }
  SoundPlay(g_MenuSoundRequest, 1);
  g_MenuSoundRequest = 0;
  return result;
}
