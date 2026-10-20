/* Lunar SSSC — font_cell: the font's blocks on the text grid: the cells
   that show them, their image loads to video memory, lines and runs, and
   the lines viewer. The first split had it in rendering.c. Every function
   is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
#include "font.h"
extern u8 g_FontLineCells[];
extern long HWD0;
extern long VWD0;
extern u8 g_FontLines[];
extern s8 D_800B8566;
extern s8 g_FontCellOriginColumn;
extern s8 g_FontCellOriginRow;
extern s8 g_FontCellsShown;
s32 ankprint_draw(s32);
s32 frame_step();
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
void font_cell_switch_set(s8 on);
void font_place_split(s32 *place, s32 *line, s32 *block);
void packet_box_col4_add(s32 left_x, s16 top_y, s32 width, s32 height, s32 top_left_col, s32 top_right_col, s32 bottom_left_col, s32 bottom_right_col, s32 z);
void packet_line_add(s32 start_x, s32 start_y, s32 end_x, s32 end_y, s32 start_col, s32 end_col, s32 z);
void font_cell_draw(s32 column, s32 row, s32 cell);
void font_block_image_load(s32 source, s32 x, s32 y);
void font_line_image_load(s32, s32, s32, s32);
void font_image_load(s32 source, s32 place, s32 block_count);
void font_cell_origin_set(s8 column, s8 row, s8 arg2);
void font_part_image_load(s32 part, s32 source);
s32 frame_step();
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
void font_cell_switch_set(s8 on);
void font_place_split(s32 *place, s32 *line, s32 *block);
extern long HWD0, VWD0;
s32 DrawSync(s32);
void font_line_show(s32 source, s32 line);
s32 func_80016608(s32 column, s32 row);
extern long HWD0, VWD0;
s32 frame_step();
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);

/* func_80016128 */
void font_cell_switch_set(s8 on) {
    g_FontCellsShown = on;
}

/* func_80016134 */
void font_cell_draw(s32 column, s32 row, s32 cell)
{
  int width;
  s32 line;
  int u;
  unsigned int v;
  u8 *line_records;
  s32 draw_row;
  s32 draw_column;
  void *line_record;
  draw_column = column;
  draw_row = row;
  if (g_FontCellsShown != 0)
  {
    line = (cell >> 0xB) & 7;
    u = cell & 0xF0;
    width = ((cell & 0xF) + 1) * 0x10;
    line_records = g_FontLines;
    line_record = (line * 8) + line_records;
    v = (s32) AS(u8, ((FontLine *) line_record)->y);
    if (cell & 0x8000)
    {
      draw_column = g_FontCellOriginColumn;
      draw_row = g_FontCellOriginRow + (line * 2);
    }
    packet_sprite_add(draw_column * 8, draw_row * 8, u, v, width, 0x10, (s32) ((FontLine *) line_record)->tpage, 0x7ECE, 0x38);
  }
}

/* func_800161CC */
s32 font_cell_set(column, row, line, first_block, block_count, unused, unused2, unused3, unused4)
s32 column;
s32 row;
s32 line;
s32 first_block;
s32 block_count;
s32 unused;
s32 unused2;
s32 unused3;
s32 unused4;
{
  s32 var;
  s32 line_bits;
  line_bits = line << 11;
  var = (first_block << 4) | 0x200;
  return ankprint_cell_set(column, row, (line_bits | var) | (block_count - 1), var);
}

/* func_80016204 */
void font_place_split(s32 *place, s32 *line, s32 *block) {
    s32 masked_place;

    masked_place = *place & 0x7F;
    *place = masked_place;
    *line = masked_place >> 4;
    *block = *place & 0xF;
}
/* func_80016230 */
void font_cell_run_set(s32 x, s32 y, s32 n, s32 pos)
{
  s32 col;
  s32 row;
  s32 k;
  font_place_split(&pos, &col, &row);
  if (row + n >= 0x10)
  {
    k = 0x10 - row;
    font_cell_set(x, y, col, row, k);
    x += k * 2;
    if (x >= 0x28)
    {
      x -= 0x28;
      y++;
    }
    font_cell_set(x, y, (col + 1) & 7, 0, n - k);
  }
  else
  {
    font_cell_set(x, y, col, row, n);
  }
}

/* func_80016308 */
void font_block_image_load(s32 source, s32 x, s32 y)
{
    s16 sp[4];

    sp[2] = 4;
    sp[0] = x;
    sp[1] = y;
    sp[3] = 0x10;
    LoadImage(sp, source);
    DrawSync(0);
}
/* func_80016350 */
void font_line_image_load(s32 source, s32 line, s32 block, s32 block_count)
{
  u8 *e;
  s32 x;
  u8 *line_records;
  s32 y;
  s32 i;
  line_records = g_FontLines;
  e = line_records + (line * 8);
  y = ((FontLine *) e)->y;
  x = ((FontLine *) e)->x + (block * 4);
  for (i = 0; i < block_count; i++)
  {
    font_block_image_load(source, x, (s16) (y));
    source += 0x80;
    block++;
    x += 4;
    if (block >= 0x10)
    {
      line++;
      block = 0;
      if (line >= 8)
      {
        line = 0;
      }
      e = g_FontLines + (line * 8);
      x = ((FontLine *) e)->x;
      y = ((FontLine *) e)->y;
    }
  }
}

void font_line_image_load(s32, s32, s32, s32);                  /* extern */

/* func_80016438 */
void font_image_load(s32 source, s32 place, s32 block_count) {
    s32 block;
    s32 line;

    font_place_split(&place, &line, &block);
    font_line_image_load(source, line, block, block_count);
}
/* far_syms: g_FontLineCells (target reaches it %hi/%lo) */

/* func_8001648C */
void font_line_show(s32 source, s32 line) {
    s32 result;
    s32 cell_line;
    s8 *p;
    s8 *base;

    base = (s8 *)g_FontLineCells;
    p = base + line * 2;
    font_line_image_load(source, line, 0, 0x10);
    if (line < 3) {
        line |= 0x10;
    }
    cell_line = line;
    result = font_cell_set(p[0], p[1], cell_line, 0, 0x10);
}

/* func_800164FC */
void font_cell_origin_set(s8 column, s8 row, s8 arg2) {
    g_FontCellOriginColumn = column;
    g_FontCellOriginRow = row;
    D_800B8566 = arg2;
}
/* func_80016510 */
void font_run_show(s32 column, s32 row, s32 block_count, s32 place, s32 source) {
    font_image_load(source, place, block_count);
    font_cell_run_set(column, row, block_count, place);
}

/* func_80016574 */
void font_part_image_load(s32 part, s32 source)
{
  volatile int pad;
  switch (part)
  {
    case 0:
      font_image_load(source, 0x34, 0x14);
      break;

    case 1:
      font_image_load(source, 0x48, 0x10);
      break;

    case 2:
      font_image_load(source, 0x58, 0x10);
      break;

    case 3:
      font_image_load(source, 0x68, 0x18);
      break;

    default:
      break;

  }

}
/* Ruling of 2026-10-14 (docs/naming.md): Reached, and does work its one caller undoes at once: nothing read says what it is for. */
s32 func_80016608(s32 column, s32 row) {
    s32 result;

    result = font_cell_set(column, row + 2, 0, 0, 0xC);
    result = font_cell_set(column, row, 0, 0xC, 4);
    return result;
}
s32 ankprint_cell_get(s32, s32);

/* func_80016668 */
void ankprint_grid_get(s16 *cells)
{
  s32 y;
  s32 x;

  for (y = 0; y < 0x1C; y++)
  {
    for (x = 0; x < 0x28; x++)
    {
      *cells++ = ankprint_cell_get(x, y);
    }
  }
}
/* Write a whole 40 x 28 screen of u16 cells: cell (x, y) of the row-major
   block at src goes to ankprint_cell_set(x, y, cell), which stores it in the
   40-column cell buffer g_AnkprintGrid and marks row y dirty.

   It returns nothing ($v0 is left as the last loop test).  The s32 return
   type is there only because its one caller, font_lines_view, ends with
   `return ankprint_grid_set(..)`. */
/* func_800166D0 */
s32 ankprint_grid_set(u16 *src)
{
  s32 x;
  s32 y;

  for (y = 0; y < 0x1C; y++)
  {
    for (x = 0; x < 0x28; x++, src++)
    {
      ankprint_cell_set(x, y, *src);
    }
  }
}
/* Draw the lattice around the 16 x 8 block of 16 px cells that
   font_lines_view puts on screen (its eight font_cell_run_set runs of sixteen
   label cells, at cell column 2 and cell rows 2, 4 .. 16): nine horizontal
   lines from (16, y) to (272, y) for y = 16, 32 .. 144, then seventeen
   vertical lines from (x, 16) to (x, 144) for x = 16, 32 .. 272.  Each is
   one packet_line_add(x0, y0, x1, y1, colour0, colour1, ot entry): a
   two-colour line packet with both ends grey 0x606060, linked at
   ordering-table entry 0x2FE.

   It takes nothing, returns nothing and touches no global itself; no RNG
   draw and no frame wait.  font_lines_view is its one caller, once a frame
   between its backdrop (packet_box_col4_add) and the cell draw (ankprint_draw).

   Shapes: one counter a loop, and the coordinate written out as
   `i * 16 + 16` in both of its argument places.  The loop pass reduces it
   to a second register whose start value 16 is set AFTER the two hoisted
   constants, as the target has it; a coordinate local stepped by hand is
   set before them (44/47).  The coordinate must reach packet_line_add as an
   int: under an `s16` parameter prototype it is shifted up 16 and back
   (42/47). */
/* func_80016738 */
void font_lines_lattice_draw(void)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        packet_line_add(0x10, i * 16 + 16, 0x110, i * 16 + 16, 0x606060, 0x606060, 0x2FE);
    }
    for (i = 0; i < 17; i++) {
        packet_line_add(i * 16 + 16, 0x10, i * 16 + 16, 0x90, 0x606060, 0x606060, 0x2FE);
    }
}

void font_codes_show(s32 column, s32 row, s32 place, u8 *codes);  /* TU defines this below */
extern s32 g_PadPressedButtons;

/* func_800167F4 */
s32 font_lines_view(void)
{
  s32 line_then_bottom_col_then_frame;
  s32 row_then_top_col;
  s32 z;
  s32 kept_cells[0x230];
  ankprint_grid_get(kept_cells);
  ankprint_clear();
  line_then_bottom_col_then_frame = 0;
  row_then_top_col = 2;
  do
  {
    font_cell_run_set(2, row_then_top_col, 0x10, line_then_bottom_col_then_frame << 4);
    line_then_bottom_col_then_frame++;
    row_then_top_col += 2;
  }
  while (line_then_bottom_col_then_frame < 8);
  frame_step();
  row_then_top_col = 0x600000;
  line_then_bottom_col_then_frame = 0x200000;
  z = 0x300;
  do
  {
    pad_press_repeat();
    packet_box_col4_add(0, 0, 0x140, 0xE0, row_then_top_col, row_then_top_col, line_then_bottom_col_then_frame, line_then_bottom_col_then_frame, z);
    font_lines_lattice_draw();
    ankprint_draw(0x10);
    frame_step();
  }
  while (g_PadPressedButtons == 0);
  line_then_bottom_col_then_frame = 0;
  do
  {
    frame_step();
    line_then_bottom_col_then_frame++;
  }
  while (line_then_bottom_col_then_frame < 0xA);
  return ankprint_grid_set(kept_cells);
}

/* func_800168E0 */
void font_codes_show(s32 column, s32 row, s32 place, u8 *codes)
{
    s32 v;

    if (*codes)
    {
        do
        {
            v = (*codes << 8) | codes[1];
            font_target_clear(0x801CBB00, 1);
            font_code_put(v, 0);
            font_image_load(0x801CBB00, place, 1);
            font_cell_run_set(column, row, 1, place);
            place++;
            codes += 2;
            column += 2;
        } while (*codes);
    }
}
