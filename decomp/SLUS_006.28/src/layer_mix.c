/* Lunar SSSC — layer_mix: the layers' mix switched on and off bit by bit.
   The first split had it in misc_e.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"

/* func_80054C9C */
void line_path_make(s16 *path, s16 start_x, s16 start_y, s16 end_x, s16 end_y, s32 point_count)
{
  s16 *ptr;
  s16 i;
  s32 dx;
  s32 dy;
  ptr = path;
  i = 1;
  dx = end_x - start_x;
  dy = end_y - start_y;
  if (point_count > 0)
  {
    do
    {
      *ptr = start_x + (dx * i) / point_count;
      *(ptr + 1) = start_y + (dy * i) / point_count;
      ptr += 2;
      i++;
    } while (i <= point_count);
  }
  ptr -= 2;
  *ptr = end_x;
  *(ptr + 1) = end_y;
}

/* func_80054DAC */
void layer_mix_bits_switch_on(s32 layer_mask) {
    if (layer_mask & 0x100) {
        layer_mix_switch_on(0x100);
    }
    if (layer_mask & 0x4) {
        layer_mix_switch_on(0x4);
    }
    if (layer_mask & 0x8) {
        layer_mix_switch_on(0x8);
    }
    if (layer_mask & 0x10) {
        layer_mix_switch_on(0x10);
    }
    if (layer_mask & 0x20) {
        layer_mix_switch_on(0x20);
    }
    if (layer_mask & 0x1) {
        layer_mix_switch_on(0x1);
    }
    if (layer_mask & 0x2) {
        layer_mix_switch_on(0x2);
    }
    if (layer_mask & 0x80) {
        layer_mix_switch_on(0x80);
    }
}

/* func_80054E68 */
void layer_mix_bits_switch_off(s32 layer_mask)
{
    if (layer_mask & 0x100) {
        layer_mix_switch_off(0x100);
    }
    if (layer_mask & 0x4) {
        layer_mix_switch_off(0x4);
    }
    if (layer_mask & 0x8) {
        layer_mix_switch_off(0x8);
    }
    if (layer_mask & 0x10) {
        layer_mix_switch_off(0x10);
    }
    if (layer_mask & 0x20) {
        layer_mix_switch_off(0x20);
    }
    if (layer_mask & 0x1) {
        layer_mix_switch_off(0x1);
    }
    if (layer_mask & 0x2) {
        layer_mix_switch_off(0x2);
    }
    if (layer_mask & 0x80) {
        layer_mix_switch_off(0x80);
    }
}
