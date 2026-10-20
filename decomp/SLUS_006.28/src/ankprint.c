/* Lunar SSSC — ankprint: the text grid: its cells set, read and cleared,
   printmes, and the cells drawn by kind. The name is the developers' own:
   ANKPRINT.C, the source file printmes's assertion names (its message names
   printmes itself); the file's other functions have it by being of its
   group. The first split had it in cdrom_load.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

extern u8 g_AnkprintRowMarks[];
extern u8 g_AnkprintGrid[];
s32 ankprint_rows_clear(s32, s32, s32);                           /* extern */
s32 assert_fail(s32 *, s32 *, s32);
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 ankprint_area_clear(s32, s32, s32, s32, s32);
void ankprint_clear(void);  /* TU defines this below */
void ankprint_cell_set(s32 column, s32 row, s32 cell);
u16 ankprint_cell_get(s32 column, s32 row);
s32 assert_fail(s32 *, s32 *, s32);
s32 assert_fail(s32 *, s32 *, s32);
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ankprint_cell_set(s32 column, s32 row, s32 cell);
u16 ankprint_cell_get(s32 column, s32 row);

/* far_syms: g_AnkprintRowMarks (target reaches it %hi/%lo) */

/* func_80025864 */
void ankprint_cell_set(s32 column, s32 row, s32 cell)
{
    *(u16 *)(g_AnkprintGrid + column * 2 + row * 80) = cell;
    g_AnkprintRowMarks[row] = 1;
}

/* far_syms: g_AnkprintGrid (target reaches it %hi/%lo) */

/* func_800258A0 */
u16 ankprint_cell_get(s32 column, s32 row)
{
    return *(u16 *)(g_AnkprintGrid + column * 2 + row * 80);
}
/* func_800258CC */
s32 ankprint_area_clear(s32 x, s32 y, s32 w, s32 h, s32 mode)
{
  s32 i;
  s32 j;
  s32 c;
  s32 val;
  if (y + h >= 0x1D)
  {
    h = 0x1B - y;
  }
  if (x + w >= 0x29)
  {
    w = 0x27 - x;
  }
  for (i = 0; i < h; i++)
  {
    c = x;
    for (j = 0; j < w; j++)
    {
      val = (*(u16 *) (g_AnkprintGrid + y * 80 + c * 2) >> 8) & 7;
      if (mode == 0x30)
      {
        ankprint_cell_set(c, y, 0);
      }
      else
      {
        if ((mode == 0x10) && (val >= 3))
        {
          ankprint_cell_set(c, y, 0);
        }
        if ((mode == 0x20) && (val < 3))
        {
          ankprint_cell_set(c, y, 0);
        }
      }
      c++;
    }
    if ((x == 0) && (w == 0x28) && (mode == 0x30))
    {
      g_AnkprintRowMarks[y] = 0;
    }
    y++;
  }
}

s32 ankprint_area_clear(s32, s32, s32, s32, s32);             /* extern */

/* func_80025A68 */
s32 ankprint_rows_clear(s32 first_row, s32 row_count, s32 kind_mask) {
    return ankprint_area_clear(0, first_row, 0x28, row_count, kind_mask);
}

/* func_80025A9C */
void ankprint_clear(void) {
    ankprint_rows_clear(0, 0x1C, 0x30);
}
extern u8 D_80011898[];
extern u8 D_800118BC[];
extern u8 g_PrintmesBuffer[];
s32 fmt_vsprintf(s8 *dst, s8 *fmt, s32 *args);

/* printmes -- printmes(x, y, fmt, ...), ANKPRINT.C's printf onto the
   40 x 28 cell grid. The name is the function's own, from its assert
   text. 0x80025AC4 is the entry: GCC emits the two argument spills ahead
   of `addiu $sp`, so the prologue at 0x80025ACC is not where the function
   starts.

   Nothing in the image calls it: there is no jal / j to 0x80025AC4 or to
   0x80025ACC, and neither address is a data word.

   1. va_start: args = the address of the argument word after fmt
      ($sp + 0x34 in this frame). The prologue has put $a2 / $a3 in their
      homes at +0x30 / +0x34; the fifth word on is the caller's outgoing
      area, so the words are read in place.
   2. Unless x < 0x28 and y < 0x1D (both unsigned):
      assert_fail("printmes()'s x,y parameter error" D_80011898,
      "ANKPRINT.C" D_800118BC, 0x10F), the assert report with file and
      line 271. assert_fail (exception.c) is empty in this build and the
      report is not a return: the print goes on with the same x and y.
   3. fmt_vsprintf(g_PrintmesBuffer, fmt, args): fmt.c's vsprintf into the
      0x40-byte buffer g_PrintmesBuffer, which ends where the grid g_AnkprintGrid
      starts. No length is passed or tested.
   4. For each byte of the buffer up to its 0: ankprint_cell_set(x, y, ch),
      x one further each time -- cell (x, y) = ch and row y marked dirty
      (g_AnkprintRowMarks[y] = 1). A cell value below 0x100 is kind 0, the 8 x 8
      ASCII glyph packet_sprite_add draws every frame (menu-ui.md "The cell
      grid"). x is not tested again as it advances.

   The bound on y admits 28, one past the grid's 28 rows:
   g_AnkprintGrid + 28 * 80 is g_AnkprintRowMarks, the dirty-row bytes.

   Writes g_PrintmesBuffer (through fmt_vsprintf, which also moves its write
   position g_FmtWritePosition) and reads it back; the grid and g_AnkprintRowMarks through
   ankprint_cell_set. No RNG draw, no frame wait, no result.

   Shapes the bytes depend on (each measured alone on the matching text):
   - A stdarg definition, `(x, y, fmt, ...)`. mips_expand_prologue homes
     the argument registers from the LAST NAMED parameter's on, so $a2
     and $a3 are stored at 8($sp) / 0xC($sp) before the frame is made.
     Without the `...` those two rows are absent (49/51); the <varargs.h>
     form of fmt.c's fmt_sprintf (`fmt, va_alist`) homes $a3 alone
     (50/51, and 43/51 with args = &va_alist).
   - va_start is PSY-Q <stdarg.h>'s, which takes the ADDRESS of the last
     named parameter: AP = (char *)&LASTARG + __va_rounded_size(LASTARG).
     That is why fmt lives in its home slot -- stored a second time at
     0x30($sp) and loaded from there for the call -- and args is
     `addiu $s4, $sp, 0x34`. GCC's own `__builtin_next_arg(fmt)` keeps
     fmt in a register: 43/51. It is written out as in libspu_spu_3.c
     because the attempt harness (station-tools) does not carry a TU's
     #include <stdarg.h>. `(s32 *)(&fmt + 1)` is the same bytes.
   - va_start ahead of the test (after it: 45/51).
   - x and y unsigned (`sltiu`; s32 parameters 49/51). `(u32)` casts on
     s32 parameters match as well.
   - The buffer unsigned (`lbu`; s8: 48/51).
   - Both steps after the call, in the for's third clause: reorg takes
     `p++` back into the jal's delay slot and `x++` into the loop
     branch's. `*p++` as the argument is 48/51, `x++` as the argument
     49/51, both 46/51. `x++, p++` and a while loop with the two
     statements match too. */
/* func_80025AC4 */
void printmes(u32 x, u32 y, s8 *fmt, ...)
{
    void *args;
    u8 *p;

    args = (char *) &fmt + sizeof(fmt);                             /* va_start(args, fmt) */
    if (x >= 0x28 || y >= 0x1D) {
        assert_fail((s32 *)D_80011898, (s32 *)D_800118BC, 0x10F);
    }
    fmt_vsprintf((s8 *)g_PrintmesBuffer, fmt, args);
    for (p = g_PrintmesBuffer; *p != 0; p++, x++) {
        ankprint_cell_set(x, y, *p);
    }
}

/* func_80025B90 */
s32 ankprint_glyph_draw(s32 column, s32 row, s32 cell)
{
  s32 c;
  s32 h;
  s32 pal;
  s32 v;
  c = cell;
  h = 8;
  pal = (cell >> 14) + 0xB;
  v = (cell >> 8) & 0x38;
  c &= 0xFF;
  if (c & 0x80)
  {
    h = 0x10;
    if (c >= 0xA0)
    {
      c += 0x20;
    }
  }
  return packet_sprite_add(column * 8, row * 8, (c & 0x1F) * 8, ((c & 0xE0) >> 2) | 0x80, v, h, 0x1F, (pal & 0xF) + 0x7EC0, 0x30);
}

/* func_80025C20 */
s32 ankprint_cell_draw(s32 column, s32 row, s32 cell, s32 z_then_code) {
    s32 z;

    z = z_then_code;
    z_then_code = cell & 0xFF;
    switch ((cell >> 8) & 7) {
        case 0:
            return packet_sprite_add(column * 8, row * 8, z_then_code * 8, (z_then_code >> 5) * 8 + 0xB8, 8, 8, 0x1F, 0x7ECE, z);
        case 1:
            return ankprint_glyph_draw(column, row, cell);
        case 3:
            return IconCellDraw();
        case 4:
            return FaceCellDraw(column, row, cell);
        case 2:
            return font_cell_draw();
    }
}

/* func_80025CEC */
void ankprint_draw(s32 z)
{
  s32 i;
  s32 j;
  s32 flag;
  u16 *p;
  s32 cell;
  for (i = 0; i < 0x1C; i++)
  {
    if (g_AnkprintRowMarks[i])
    {
      flag = 0;
      p = (u16 *) (g_AnkprintGrid + (i * 0x50));
      for (j = 0; j < 0x28; j++)
      {
        cell = *p++;
        if (cell)
        {
          ankprint_cell_draw(j, i, cell, z);
          flag = 1;
        }
      }

      if (!flag)
      {
        g_AnkprintRowMarks[i] = 0;
      }
    }
  }

}
/* func_80025DC0 */
void ankprint_glyph_set(s32 column, s32 row, s32 code, s32 width, s32 col) {
    s32 col_bits;
    s32 kind_code;
    s32 cell;

    cell = width << 11;
    kind_code = code | 0x100;
    col_bits = col;
    cell |= kind_code;
    col_bits <<= 2;
    ankprint_cell_set(column, row, cell | col_bits);
}
