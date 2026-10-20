/* Lunar SSSC — layer_grey: the layers' grey: the two levels, their mode,
   share and apply. The first split had it in ui.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
s32 bg_col_set(s32);
s32 sprite_col_set(s32);
s32 layer_grey_set(u8, u8);                          /* extern */
extern u8 g_LayerGreyMode;
extern u8 g_LayerGreyFirstShare;
extern s8 g_LayerGreyLevel;
s32 layer_grey_apply();                                  /* extern */
extern u8 g_LayerGreySecondShare;
void layer_grey_mode_set(void);
void func_8001C4E4();
void layer_grey_level_set(s8 level);

/* func_8001C2C8 */
s32 layer_grey_set(u8 first_level, u8 second_level) {
    s32 first_col;
    s32 second_col;

    first_col = first_level;
    first_col = first_col | (first_col << 8) | (first_col << 16);
    second_col = second_level;
    second_col = second_col | (second_col << 8) | (second_col << 16);
    bg_col_set(first_col);
    sprite_col_set(first_col);
    return bg_second_col_set(second_col);
}

extern u8 g_LayerGreySecondShare;

/* Ruling of 2026-10-14 (docs/naming.md): Only code that nothing reaches calls this function; named for what it does. */
/* func_8001C32C */
void layer_grey_mode_set(void)
{
  s32 second_share;
  if ((((u32) (g_LayerGreySecondShare - 1)) >= 0x1FU) && (g_LayerGreyMode == 1))
  {
    g_LayerGreyMode = 3;
  }
  if (((u32) (g_LayerGreySecondShare - 0xE)) < 5U)
  {
    g_LayerGreyMode = 1;
  }
  second_share = 0xFF;
  second_share = g_LayerGreySecondShare & second_share;
  if (second_share == 1)
  {
    g_LayerGreyMode = 4;
  }
  if ((second_share == 0x10) && (g_LayerGreyMode == 4))
  {
    g_LayerGreyMode = 1;
  }
}
void func_8001C4E4();  
extern u8 g_LayerGreyFirstLevel;
extern u8 g_LayerGreySecondLevel;
extern s16 g_MapCol; /* far_syms: g_MapCol */

/* func_8001C3B4 */
s32 layer_grey_apply() {
    s32 level;

    layer_grey_mode_set();
    level = (u8) g_LayerGreyLevel;
    if (g_MapCol != 0) {
        level = (s32) (level * ((s32) (g_MapCol + 0x100) >> 2)) >> 7;
    }
    switch (g_LayerGreyMode) {                           /* irregular */
    case 1:
        g_LayerGreyFirstLevel = (u8) ((s32) (level * g_LayerGreyFirstShare) >> 4);
        g_LayerGreySecondLevel = (u8) ((s32) (level * g_LayerGreySecondShare) >> 4);
        break;
    case 5:
        g_LayerGreyFirstLevel = (u8) ((s32) (level * g_LayerGreyFirstShare) >> 5);
        g_LayerGreySecondLevel = (u8) ((s32) (level * g_LayerGreySecondShare) >> 5);
        break;
    default:
        g_LayerGreyFirstLevel = level;
        g_LayerGreySecondLevel = level;
        break;
    }
    return layer_grey_set(g_LayerGreyFirstLevel, g_LayerGreySecondLevel);
}

void func_8001C4E4();  
/* func_8001C4A0 */
void layer_grey_share_set(s32 share) {
    s32 first_share;

    first_share = share;
    if (first_share < 0) {
        first_share = 0;
    }
    if (first_share >= 0x20) {
        first_share = 0x20;
    }
    g_LayerGreyFirstShare = first_share;
    g_LayerGreySecondShare = 0x20 - first_share;
    layer_grey_apply(first_share);
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; the 5 or 3 its caller passes is not explained by the code that passes it. */
void func_8001C4E4() {

}

/* func_8001C4EC */
void layer_grey_level_set(s8 level) {
    g_LayerGreyLevel = level;
    layer_grey_apply();
}
extern s16 D_800B8964[];

/* Ruling of 2026-10-14 (docs/naming.md): Stores into words nothing reads, and nothing reaches it: nothing supports a name. */
volatile unsigned short func_8001C510(s32 unused, s32 unused2, s32 which)
{
  if (which < 2)
  {
    if (which >= 0)
    {
      D_800B8964[1 - which] = 0x10;
    }
  }
}
