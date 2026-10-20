/* Lunar SSSC — font_string: the font's characters written into a target: a
   code's pattern and outline, font_code_put, the strings by id and by
   bytes, the target, colour and pen, and the controls. The first split had
   it in rendering2.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "tile.h"
#include "pad.h"
#include "palette.h"
#include "chr.h"
#include "btl.h"
#include "sprite.h"
extern u8 g_FontOutlinePattern[];
u8 *font_pattern_get(s32);
extern s32 g_FontStringMode;
extern u8 g_FontPen;
extern s32 g_FontTarget;
extern u8 g_FontColBits;
void font_control_run(s32 code, s32 mode);
void func_80026B14();
extern long HWD0;
extern long VWD0;
s32 font_code_put(s32, s32);
void font_string_code_put(s32 code);
void font_string_control_run(s32 code);
void font_bytes_put(u8 *string, s32 mode);
void font_string_put(s32, s32, s32);
void font_string_fixed_put(s32 string_id);
void font_string_proportional_put(s32 string_id);
void font_target_set(u8 *target);
s32 font_col_set(s32 col_bits);
void font_pen_move(s32 pixels);
extern long HWD0, VWD0;
void mem_clear(void *destination, u32 byte_count);  /* extern */

/* far_syms: g_BtlItemTable (target reaches it %hi/%lo) */
/* far_syms: g_BtlSpellRows (target reaches it %hi/%lo) */
/* far_syms: g_PlayTimeFrames (target reaches it %hi/%lo) */
/* far_syms: g_PlayTimeMinutes (target reaches it %hi/%lo) */
/* far_syms: g_PlayTimeFrames (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PlayTimeMinutes (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 g_BtlItemTable[];
extern u8 g_BtlSpellRows[];
extern s32 g_ResourceCommand01Buffer;
extern u16 g_PlayTimeFrames;
extern u16 g_PlayTimeMinutes;
extern s32 g_FontStringTable;
extern s32 g_ChrItemRows;
extern s32 g_ChrSpellRows;

/* Ruling of 2026-10-14 (docs/naming.md): Reached, and does work: six stores with no one subject found. */
void func_80026AB0(void)
{
    g_ChrItemRows = (s32)g_BtlItemTable;
    g_ChrSpellRows = (s32)g_BtlSpellRows;
    g_PlayTimeMinutes = 0;
    g_PlayTimeFrames = 0;
    g_FontStringTable = g_ResourceCommand01Buffer;
    func_800506DC();
}
/* load_hazard_nop: font_string_table_set */
/* func_80026B00 */
void font_string_table_set(void) {
    g_FontStringTable = g_ResourceCommand01Buffer;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_80026B14() {

}
/* la_absolute: font_pattern_get */

extern u8 g_FontPatterns20[];
extern u8 g_FontPatterns9908[];
extern u8 g_FontBlankPattern[8];
long Krom2RawAdd(unsigned long);

/* Menu-font glyph select: the address of the 32-byte glyph (16 rows of 16
   pixels, 1 bit a pixel, two bytes a row, high byte first) for character
   `code`. The one caller is font_pattern_outline, which reads the first 15 rows
   (30 bytes) and renders them with an outline into g_FontOutlinePattern.
     - 0x20..0x7A:     g_FontPatterns20 + (code - 0x20) * 32   (0x5B glyphs)
     - 0x9908..0x99F8: g_FontPatterns9908 + (code - 0x9908) * 32 (0xF1 glyphs; the
       table ends where g_FontPatterns20 starts)
     - anything else:  Krom2RawAdd(code), which is libapi's Krom2RawAdd
       (the BIOS B(0x51) thunk: the address of the 16x16 pattern for a
       Shift-JIS code in the BIOS kanji ROM, or -1 when it has none; the
       prototype here is libapi.h's). On -1 the answer is g_FontBlankPattern.
   g_FontBlankPattern is an 8-byte small-data object, all zero in the image, and
   nothing else in the program names it. The caller reads 30 bytes, so for
   a character with no glyph anywhere rows 4 to 14 come from the small-data
   variables behind it (g_FontPen, g_MusicWaitFrameKind, g_ScriptNumber, ...).
   Reads no other state and writes none. No RNG draw (rng_draw) and no
   frame wait in this function; the BIOS routine behind Krom2RawAdd is
   not in this tree.

   Shapes that decided the match:
     - g_FontBlankPattern is declared SIZED (8 bytes, the distance to the next
       symbol) and the function carries la_absolute: the target builds its
       address as `lui v1; addiu v1, v1, %lo` on ONE register, which is the
       Sony assembler's `la` of a small-data symbol. An unsized (far)
       declaration makes cc1 split the address and put the `lui` in the
       first free register, $v0: 29/31.
     - each table arm takes the table's address into `tbl` first, scales
       the index in its own variable (`i <<= 5`) and returns the sum; the
       two tails then use the same registers and cross jumping merges them
       as the target has them. `tbl + (i << 5)` in one expression is 23/31.
   MARKED SPELLING: the `tbl` pointer local. With the table named in the
   return (`return g_FontPatterns20 + i;` after `i <<= 5;`) the shift is emitted
   ahead of the address and the score is 27/31.
   MARKED SPELLING: `u8 [8]` for g_FontBlankPattern. The bytes prove a small-data
   object (1 to 8 bytes); the element type is not known. */
/* func_80026B1C */
u8 *font_pattern_get(s32 code)
{
    u32 i;
    u8 *tbl;
    u8 *p;

    i = code - 0x20;
    if (i < 0x5B) {
        tbl = g_FontPatterns20;
        i <<= 5;
        return tbl + i;
    }
    i = code - 0x9908;
    if (i < 0xF1) {
        tbl = g_FontPatterns9908;
        i <<= 5;
        return tbl + i;
    }
    p = (u8 *)Krom2RawAdd(code);
    if (p == (u8 *)-1) {
        p = g_FontBlankPattern;
    }
    return p;
}

extern s32 D_800E48A4[];

/* func_80026B98 */
void font_pattern_outline(s32 code)
{
  u16 b[17];
  u16 a[17];
  u8 *g;
  u8 *dst;
  s32 i;
  s32 j;
  s32 x;
  s32 mask;
  s32 out;
  s32 px;
  u16 ra;
  u16 rb;
  D_800E48A4[0] = 0;
  g = font_pattern_get(code);
  a[0] = 0;
  a[16] = 0;
  for (i = 0; i < 0xF; i++)
  {
    a[i + 1] = (g[0] << 8) | g[1];
    g += 2;
  }
  for (i = 0; i < 0xF; i++)
  {
    b[i] = a[i + 1];
  }
  b[15] = 0;
  b[16] = 0;
  for (i = 0; i < 0xF; i++)
  {
    b[i + 2] |= a[i + 1];
  }
  for (i = 1; i < 0x10; i++)
  {
    x = a[i];
    b[i] |= x | (x << 1) | (x >> 1);
  }
  dst = g_FontOutlinePattern;
  for (i = 0; i < 0x11; i++)
  {
    mask = 0x8000;
    out = 0;
    ra = a[i];
    rb = b[i];
    for (j = 15; j >= 0; j--)
    {
      out <<= 2;
      px = 0;
      if (ra & mask)
      {
        px = 1;
      }
      else if (rb & mask)
      {
        px = 3;
      }
      out |= px;
      mask >>= 1;
    }
    x = out;
    *dst++ = x >> 24;
    *dst++ = x >> 16;
    *dst++ = x >> 8;
    *dst++ = x;
  }
}
extern s8 g_FontColumnShifts[];

/* Draw one label-font glyph at the pen. font_pattern_outline renders glyph code into
   g_FontOutlinePattern: 16 rows of 16 pixels, 2 bits a pixel (0 empty, 1 ink, 3 outline),
   each row a word whose column c is at bit g_FontColumnShifts[c] (the rows are stored
   big-endian and read here as little-endian words). The glyph's width is its
   rightmost used column + 1 (a blank glyph: 5). arg1 == 0 gives every glyph its
   own 16-pixel cell: the pen first moves up to the next cell (font_pen_move),
   then right by half of the unused columns, which centres the glyph. arg1 != 0
   is the proportional run: the glyph starts where the pen is.
   Each column is OR-ed into the 16 x 16 4bpp cell at g_FontTarget (8 bytes a row;
   g_FontPen is the pen's x in the cell: byte x / 2, the high nibble for an odd
   x), a used pixel together with the colour bits g_FontColBits, and the pen moves
   one pixel right.
   MARKED SPELLING: an old-style definition. By the bytes the function is
   void (u16, s32); nine TUs declare s32 (s32, s32) and are left as they are,
   because the callers were matched under those prototypes. The s32 return
   type is kept only to agree with them: nothing is returned. */
/* func_80026D64 */
s32 font_code_put(code, mode)
  u16 code;
  s32 mode;
{
  u32 *rows;
  u32 used;
  s32 i;
  s32 j;
  s32 width;
  s32 shift;
  s32 off;
  u8 px;

  if ((mode == 0) && (g_FontPen != 0))
  {
    font_pen_move((0x10 - g_FontPen) & 0xFF);
  }
  font_pattern_outline(code);
  used = 0;
  rows = (u32 *) g_FontOutlinePattern;
  for (i = 0; i < 0x10; i++)
  {
    used |= rows[i];
  }
  for (i = 0; i < 0x10; i++)
  {
    if ((used >> g_FontColumnShifts[0xF - i]) & 3)
    {
      break;
    }
  }
  width = 0x10 - i;
  if (width == 0)
  {
    width = 5;
  }
  if (mode == 0)
  {
    font_pen_move((i / 2) & 0xFF);
  }
  for (i = 0; i < width; i++)
  {
    shift = g_FontColumnShifts[i];
    off = g_FontPen >> 1;
    for (j = 0; j < 0x10; j++)
    {
      px = (rows[j] >> shift) & 3;
      if (px != 0)
      {
        px |= g_FontColBits;
      }
      if (g_FontPen & 1)
      {
        *((u8 *) (g_FontTarget + off)) |= px << 4;
      }
      else
      {
        *((u8 *) (g_FontTarget + off)) |= px;
      }
      off += 8;
    }
    font_pen_move(1);
  }
}

s32 font_code_put(s32, s32);                          /* extern */

/* func_80026F30 */
void font_string_code_put(s32 code) {
    font_code_put(code & 0xFFFF, g_FontStringMode);
}

/* func_80026F54 */
void font_string_control_run(s32 code) {
    font_control_run(code & 0xFFFF, g_FontStringMode);
}
/* far_syms: font_string_code_put (target reaches it %hi/%lo) */
/* far_syms: font_string_control_run (target reaches it %hi/%lo) */

/* func_80026F78 */
void font_string_put(s32 string_id, s32 string, s32 mode)
{
  u16 *entry_or_string;
  entry_or_string = (u16 *) string;
  if (string_id & 0xFFFF)
  {
    entry_or_string = (u16 *) (((string_id & 0xFFFF) * 2) + g_FontStringTable);
    if ((*entry_or_string) == 0)
    {
      return 0;
    }
    entry_or_string = (u16 *) (g_FontStringTable + (*entry_or_string));
  }
  g_FontStringMode = mode;
  return font_string_walk(entry_or_string, font_string_code_put, font_string_control_run);
}

/* func_80026FD4 */
void font_bytes_put(u8 *string, s32 mode) {
    u8 *next_byte;
    u8 code;

    next_byte = string;
    if (*next_byte != 0) {
        do {
            code = *next_byte;
            next_byte += 1;
            font_code_put((s32) code, mode);
        } while (*next_byte != 0);
    }
}

void font_string_put(s32, s32, s32);                         /* extern */

/* func_8002702C */
void font_string_fixed_put(s32 string_id) {
    font_string_put(string_id & 0xFFFF, 0, 0);
}

/* func_80027054 */
void font_string_proportional_put(s32 string_id) {
    font_string_put(string_id & 0xFFFF, 0, 1);
}
/* func_8002707C */
void font_target_clear(u8 *target, u16 block_count) {
    g_FontTarget = (s32) target;
    g_FontPen = 0;
    g_FontColBits = 0;
    mem_clear(target, block_count << 7);
}

/* target is u8* per the declaration in menu_card.c, menu_nowalk.c,
   menu_option.c and menu_places.c; the store is the same
   instruction either way. */
/* func_800270AC */
void font_target_set(u8 *target) {
    g_FontTarget = (s32) target;
    g_FontPen = 0;
}

/* func_800270BC */
s32 font_col_set(s32 col_bits) {   /* s32 to agree with menu_card.c and menu_option.c */
    u8 old_col_bits;

    old_col_bits = g_FontColBits;
    g_FontColBits = col_bits;
    return old_col_bits & 0xFF;
}

/* func_800270CC */
void font_pen_move(s32 pixels) {
    u32 masked_new_pen;
    u8 new_pen;

    new_pen = g_FontPen + pixels;
    masked_new_pen = new_pen & 0xFF;
    g_FontPen = new_pen;
    if (masked_new_pen >= 0x10U) {
        g_FontPen = new_pen & 0xF;
        g_FontTarget += (masked_new_pen >> 4) << 7;
    }
}
/* func_8002710C */
void font_control_run(s32 code, s32 mode) {
    s32 high_byte;

    if ((code > 0xEFFF) && (high_byte = code & 0xFF00, (code != 0xFFFF))) {
        switch (high_byte) {                          /* irregular */
        case 0xF300:
        case 0xF600:
        case 0xF400:
        case 0xF500:
        case 0xF080:
        case 0xF100:
        case 0xF200:
        case 0xF000:
            font_string_put(code & 0xFFF, 0, mode);
            return;
        case 0xF900:
            font_pen_move(code & 0xFF);
            return;
        case 0xFC00:
            font_col_set((code >> 6) & 0xFC);
            break;
        }
    }
}
