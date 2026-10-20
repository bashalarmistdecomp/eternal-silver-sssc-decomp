/* Lunar SSSC — cell_text: what is put on the text grid's cells: text,
   icons, numbers and glyphs, and the cells cleared. The first split had it
   in misc_j.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "map.h"

s32 CellGlyphSet(s32, s32, s32, s32, s32, s32);

s32 CellRowsClear(s32, s32, s32);                           /* extern */

/* func_80057288 */
void CellAllClear(s32 kind_mask) {
    if (kind_mask & 0x10) {
        CellRowsClear(0, 0x20, 0x10);
    }
    if (kind_mask & 0x20) {
        CellRowsClear(0, 0x40, 0x20);
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its four callers pass nothing and do not agree on what for. */
void func_800572D8() {

}

s32 ankprint_rows_clear();
s32 CellNumberSet(s32, s32, s32, s32, s32, u16, s32, s32);
s32 font_cell_run_set(s32, s32, s32, s32);
s32 font_image_load(s32, s32, s32);

/* func_800572E0 */
void CellTextDisp(s32 column, s32 first_row, s32 shown_block_count, s32 text_row_count, s32 row_block_count, s32 pad5_0, u16 first_code)
{
  s32 row_offset;
  s32 relative_code;
  int font_target_then_step;
  s32 place;
  s32 text_row;
  s32 row;
  s32 first_block_offset;
  u8 unused_byte;
  text_row = 0;
  if (text_row_count >= (0 + 1))
  {
    relative_code = 0xFFFF1300;
    relative_code = first_code + relative_code;
    first_block_offset = relative_code << 5;
    row = first_row;
    place = relative_code >> 2;
    ;
    do
    {
      row_offset = row_block_count * (text_row << 7);
      font_image_load(first_block_offset + (row_offset + (font_target_then_step = 0x801CBB00)), place, row_block_count);
      text_row += (font_target_then_step = 1);
      font_cell_run_set(column, row, shown_block_count, place);
      row += 2;
      place += row_block_count;
    }
    while (text_row < text_row_count);
  }
}

/* func_800573B8 */
s32 CellIconSet(s32 column, s32 row, s32 unused, s32 icon, s32 a4) {
    return IconCellSet(column, row, icon, a4);
}

/* func_800573E0 */
s32 CellRowsClear(s32 first_row, s32 row_count, s32 kind_mask) {
    return ankprint_rows_clear();
}

extern u8 D_800B87D8[];
s32 fmt_sprintf(s8 *, u8 *, s32);

/* func_80057400 */
s32 CellNumberSet(s32 value, s32 x, s32 y, s32 n, s32 a4, u16 a5, s32 a6, s32 zeropad)
{
  s8 buf[16];
  s32 i;
  s32 k;
  s32 c;
  fmt_sprintf(buf, D_800B87D8, value & 0x7FFFFFFF);
  for (i = 0; i < n; i++)
  {
    k = i + 10;
    c = buf[k - n];
    if ((zeropad != 0) && (c == ' '))
    {
      c = '0';
    }
    if (c != ' ')
    {
      CellGlyphSet((u16) (c - '0'), x, y, a4, a5, a6);
    }
    x++;
  }
}

s32 CellNumberSet(s32, s32, s32, s32, s32, u16, s32, s32);
s32 ankprint_glyph_set(s32, s32, s32, s32, s32);

/* func_80057500 */
s32 CellGlyphSet(s32 glyph, s32 column, s32 row, s32 unused, s32 col, s32 tall)
{
  s32 masked_col;
  s32 code;
  int ra_arm;
  masked_col = (s32) ((u16) col);
  if (tall == 0)
  {
    code = glyph + 1;
    ra_arm = 0xFFFF;
  }
  else
  {
    code = glyph | 0x80;
    ra_arm = 0xFFFF;
  }
  return ankprint_glyph_set(column, row, code & 0xFFFF, 1, masked_col);
}

s32 CellNumberSet(s32, s32, s32, s32, s32, u16, s32, s32);

/* func_8005754C */
void CellChrGlyphSet(s32 chr_id, s32 column, s32 row, s32 unused, u16 col, s32 tall)
{
  s32 cell_column;
  s32 glyph_offset;
  int masked_col;
  s32 drawn_id;
  s32 glyph;
  s32 next_glyph;
  s32 i;

  drawn_id = chr_id;

  if (drawn_id == 0xB)
  {
    drawn_id = 1;
  }
  glyph_offset = (drawn_id - 1) * 4;
  next_glyph = glyph_offset + 0x10;
  if (tall == 0)
  {
    next_glyph = glyph_offset + 0xF;
  }
  i = 0;
  masked_col = col & 0xFFFF;
  glyph = next_glyph & 0xFFFF;
  do
  {
    next_glyph += 1;
    cell_column = column;
    column += 1;
    CellGlyphSet(glyph, cell_column, row, unused, masked_col, tall);
    i += 1;
    glyph = next_glyph & 0xFFFF;
  }
  while (i < 4);
}
