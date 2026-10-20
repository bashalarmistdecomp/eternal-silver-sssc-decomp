/* Lunar SSSC — palette_buf: the palette buffer: colours put in it, rows
   marked, and the marked rows sent to video memory once a frame. The first
   split had it in io.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
s32 palette_rows_load();                                  /* extern */
s32 palette_buf_put(s32, s32, s32);                   
/* far_syms: g_PaletteBuf (target reaches it %hi/%lo) */
s32 palette_buf_address_set();
extern s32 g_PaletteBuf;

/* func_800200B0 */
s32 palette_buf_put(s32 cols, s32 col_index, s32 col_count)
{
    u16 *src;
    s32 count;
    u16 *dst;

    palette_buf_address_set();
    src = (u16 *)cols;
    count = col_count;
    dst = (u16 *)(g_PaletteBuf + col_index * 2);
    while (count != 0) {
        *dst++ = *src++;
        count--;
    }
}

extern u8 g_PaletteRowMarks[];
/* func_80020118 */
void palette_row_mark(s32 col_index) {
    *((col_index >> 8) + g_PaletteRowMarks) = 1;
}
extern s8 g_PaletteAnimationCopyId;
extern s8 g_PaletteAnimationCopyFirst;
extern s8 g_PaletteAnimationCopyCount;
extern u16 g_PaletteAnimationCopyCols[];
void palette_row_part_load(s32 col_index, s32 col_count, s32 cols);  /* extern */

/* palette_update -- the per-frame CLUT upload. Its one caller in the image
   is the field frame's tail MapFrameEnd (map_frame.c), between the two tile
   layers (bg_draw) and tile_animation_load, i.e. before the present.

   The colour buffer is *g_PaletteBuf (0x801D5B80, set by palette_buf_address_set):
   eight rows of 256 colours, 0x200 bytes each, row r living in VRAM at
   (0, 500 + r).

   1. Dirty rows. For r = 0..7 in that order: if the flag byte
      g_PaletteRowMarks[r] is non-zero it is cleared and the row is uploaded,
      palette_rows_load(r, 1, buf + r * 0x200) = LoadImage of the rectangle
      (x 0, y 500 + r, w 256, h 1) followed by DrawSync(0). The flags are
      set by palette_row_mark(colour index), which marks row index >> 8
      (the palette animations palette_animation_step call it with clut | 0x100,
      so they dirty row 1).

   2. The row-508 copy of an animated CLUT range, EVERY frame while the
      request byte g_PaletteAnimationCopyId is non-zero (nothing here clears it:
      palette_animation_set zeroes the three bytes, palette_animation_copy_set(id) fills them
      from the {id, first, count} triples at g_PaletteAnimationCopyRows). With
      first = g_PaletteAnimationCopyFirst and count = g_PaletteAnimationCopyCount, both in 16-colour CLUTs:
        - count * 16 colours are copied from row 1 of the buffer
          (colour 0x100 + first * 16 on) to the scratch row g_PaletteAnimationCopyCols
          with bit 15 (STP) set on each,
        - colour 0 of each CLUT in the scratch row is then zeroed
          (transparent),
        - palette_row_part_load(first * 16 + 0x800, count * 16, g_PaletteAnimationCopyCols) =
          LoadImage of (x (first * 16) & 0xF0, y 500 + 8 + (first >> 4),
          w count * 16, h 1), then DrawSync(0).
      That is the same STP-set, colour-0-clear copy palette_bg_second_row_set makes of
      a whole second palette half for row 508 (resource_palette_load's load);
      this keeps the animated CLUTs of row 508 in step with row 501.

   A count of 0 still makes the call (LoadImage of a 0-wide rectangle).
   The scratch row is 0x80 bytes (g_PaletteRowMarks follows it), so it holds at
   most four CLUTs; the table's largest count is 3.

   Waits: every upload is followed by DrawSync(0) inside
   the two callees, so the function blocks until the GPU queue drains; it
   does not wait for a frame (no VSync, no present). No RNG draw.

   Shapes the bytes depend on (each measured):
   - the copy loop is INDEXED on both sides (`buf[j] = src[j] | 0x8000`):
     the compiler then walks src in its own register and a copy of buf
     made inside the loop's guard, as the target does. Written with two
     stepped pointers (`*dst++ = *src++ | 0x8000`, dst set before the
     loop) the copy of buf is made ahead of the two byte loads and the
     whole set-up block is scheduled differently (53 -> 63).
   - the second half has its own counter j: reusing the first loop's i
     keeps it in the saved register $s0 (39 -> 47).
   - the clear loop indexes buf[j] with j += 16 (47 -> 51 against a
     stepped pointer).
   - the source address is u16-pointer arithmetic on the buffer,
     `(u16 *)g_PaletteBuf + (x + 0x100)`: the offset is then formed first
     and added to the base (51 -> 53 against
     `g_PaletteBuf + ((g_PaletteAnimationCopyFirst << 5) + 0x200)` as integers).
   - the flag bytes are read signed (lb) through a cast; the TU's
     declaration of g_PaletteRowMarks stays u8. */
/* func_80020134 */
void palette_update(void) {
    s32 i;
    s32 j;
    s32 x;
    s32 n;
    u16 *src;
    u16 *buf;

    for (i = 0; i < 8; i++) {
        if ((s8)g_PaletteRowMarks[i] != 0) {
            g_PaletteRowMarks[i] = 0;
            palette_rows_load(i, 1, g_PaletteBuf + (i << 9));
        }
    }
    if (g_PaletteAnimationCopyId != 0) {
        x = g_PaletteAnimationCopyFirst * 16;
        n = g_PaletteAnimationCopyCount * 16;
        src = (u16 *)g_PaletteBuf + (x + 0x100);
        buf = g_PaletteAnimationCopyCols;
        for (j = 0; j < n; j++) {
            buf[j] = src[j] | 0x8000;
        }
        for (j = 0; j < n; j += 16) {
            buf[j] = 0;
        }
        palette_row_part_load(x + 0x800, n, (s32)buf);
    }
}
