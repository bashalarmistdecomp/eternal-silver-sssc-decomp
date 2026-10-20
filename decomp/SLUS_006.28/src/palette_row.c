/* Lunar SSSC — palette_row: palette rows sent and set (whole, in part, for
   the bg's second layer and its cover) and the colour fixes (bit 15, zeros,
   the glyph colours). The first split had it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
s32 palette_rows_load();                                  /* extern */
s32 palette_buf_put(s32, s32, s32);                   /* extern */
s32 palette_bg_second_row_set(s32, s32, s32);
s32 palette_glyph_zero_set(s32, s32);
void palette_zero_fix(s16 *, s32, s32);
void palette_bit15_set(u16 *, s32, s32);
extern s32 g_PaletteBgCoverCount;
s32 palette_rows_set(s32, s32, s32);
void palette_row_part_load(s32 col_index, s32 col_count, s32 cols);
s32 tim_image_load(s32);
extern long HWD0, VWD0;

/* func_80020588 */
s32 palette_rows_load(first_row, row_count, cols)
s32 first_row;
s32 row_count;
s32 cols;
{
  s16 sp[4];
  int load_result;
  sp[1] = first_row + 0x1F4;
  sp[3] = row_count;
  sp[0] = 0;
  sp[2] = 0x100;
  load_result = LoadImage(sp, cols);
  return DrawSync(0);
  sp[2] = load_result;
}

/* func_800205CC */
void palette_row_part_load(s32 col_index, s32 col_count, s32 cols)
{
    s16 sp[4];

    sp[0] = col_index & 0xF0;
    sp[1] = (col_index >> 8) + 0x1F4;
    sp[2] = col_count;
    sp[3] = 1;
    LoadImage(sp, cols);
    DrawSync(0);
}

/* func_8002061C */
s32 palette_rows_set(s32 first_row, s32 row_count, s32 cols) {
    s32 result;

    palette_rows_load();
    result = first_row < 8;
    if (result != 0) {
        result = palette_buf_put(cols, first_row << 8, row_count << 8);
    }
    return result;
}
extern long HWD0;
extern long VWD0;
s32 palette_bg_cover_rows_set(s32, s32);

/* func_80020670 */
s32 palette_bg_second_row_set(s32 first_row, s32 palette_row_count, s32 cols)
{
  unsigned short *src;
  s32 i;
  unsigned short buf[0x100];
  s32 mask;
  src = (unsigned short *) (cols + 0x200);
  for (i = 0; i < 0x100; i++)
  {
    mask = -0x8000;
    buf[i] = (*src) | mask;
    if ((i & 0xF) == 0)
    {
      buf[i] = 0;
    }
    src++;
  }

  return palette_rows_set(first_row, palette_row_count, (s32) buf);
}
extern u8 g_PaletteBgCoverRectangles[];

/* func_800206D4 */
s32 palette_bg_cover_rows_set(s32 x, s32 y)
{
  s32 n = g_PaletteBgCoverCount;
  s32 i;
  s32 unused;
  u8 *rects;
  ;
  for (i = 0; i < 0x200; i++)
  {
    if ((i & 0xF) < n)
    {
      ((u16 *) y)[i] = 0;
    }
  }

  rects = g_PaletteBgCoverRectangles;
  LoadImage((void *) (rects + (x * 8)), (u32 *) y);
  return DrawSync(0);
}
extern long HWD0, VWD0;

/* func_8002073C */
s32 tim_image_load(s32 tim_address)
{
  u16 rect[4];
  u32 tim[8];
  s16 width;
  s16 height;
  GsGetTimInfo((void *) (tim_address + 4), (void *) tim);
  rect[0] = AS(u16, ((GsIMAGE *)tim)->px);
  rect[1] = AS(u16, ((GsIMAGE *)tim)->py);
  height = (rect[3] = ((GsIMAGE *)tim)->ph);
  width = ((GsIMAGE *)tim)->pw;
  rect[2] = width;
  if (width)
  {
    if (height)
    {
      LoadImage((void *) rect, tim[3]);
    }
  }
  if ((tim[0] >> 3) & 1)
  {
    rect[0] = AS(u16, ((GsIMAGE *)tim)->cx);
    rect[1] = AS(u16, ((GsIMAGE *)tim)->cy);
    rect[2] = ((GsIMAGE *)tim)->cw;
    rect[3] = ((GsIMAGE *)tim)->ch;
    LoadImage((void *) rect, tim[6]);
  }
  return DrawSync(0);
}

/* func_800207E8 */
void palette_bit15_set(u16 *cols, s32 col_count, s32 index_mask) {
    s32 col_index;
    s16 *col;

    col = cols;
    col_index = 0;
    if (col_count > 0) {
        do {
            if (!(col_index & index_mask)) {
                *col = 0;
            } else {
                *col |= ~0x7FFF;
            }
            col_index += 1;
            col += 1;
        } while (col_index < col_count);
    }
}
/* func_80020830 */
void palette_zero_fix(s16 *cols, s32 col_count, s32 index_mask) {
    s16 *col;
    s32 col_index;

    col = cols;
    col_index = 0;
    if (col_count > 0) {
        do {
            if (!(col_index & index_mask)) {
                *col = 0;
            } else if (*col == 0) {
                *col = 1;
            }
            col_index += 1;
            col += 1;
        } while (col_index < col_count);
    }
}

/* func_8002087C */
s32 palette_glyph_zero_set(s32 cols, s32 unused)
{
  s16 *col;
  u32 col_index;
  int first_col_then_step_then_index;
  col_index = 0xB0;
  first_col_then_step_then_index = cols + 0x160;
  col = (s16 *)first_col_then_step_then_index;
  do
  {
    first_col_then_step_then_index = 0x20;
    *col = 0;
    first_col_then_step_then_index = 0x10;
    col_index += first_col_then_step_then_index;
    col += first_col_then_step_then_index;
  }
  while ((first_col_then_step_then_index = col_index) < 0xF0);
}
