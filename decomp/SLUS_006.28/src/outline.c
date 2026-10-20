/* Lunar SSSC — outline: the shapes and outlines the menus draw round their
   records (ShapeDraw, OutlineDraw and its two steps; func_8001B94C is
   BtlMenuOutlineStep's empty first call). The first split had it in ui.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
#include "shop.h"
/* The colour/fade ramp records (0x24 bytes): 0x800AE16C (flag 4),
   0x800AE190 (8), 0x800AE1B4 (0x100), 0x800AE1D8 (0x80). A ramp runs from
   `from` toward `cur` over `dur` with `delta = cur - from`; +0x1C..+0x22
   are the window fields layer_mix_fade_start / layer_order_set use. */
extern u8 g_OutlineMapKinds[];
extern u16 g_PlayTimeMinutes;
void func_8001B94C();

s32 bg_scroll_get(s32 *, s32 *);
void packet_box_add(s32, s32, s16, s16, s32, s32);
void packet_line_add(s32, s32, s32, s32, s32, s32, s32);
extern s32 g_ShapeOutlineCol[];
extern s32 g_ShapeBoxCol[];

inline static int inline_fn(s32 base, s16 *offset)
{
  return base + (*offset);
}
/* func_8001B754 */
void ShapeDraw(s16 x, s16 y, void *shape)
{
  s32 scroll_y;
  s32 scroll_x;
  s32 kept_y;
  s32 line4_screen_y;
  s32 kept_x;
  s32 line4_screen_x;
  s32 unused;
  s32 line1_screen_x_then_start_x;
  s32 line2_screen_x;
  s32 line3_screen_x;
  s32 line2_screen_y;
  s32 unused2;
  u16 kind;
  int line2_start_x;
  bg_scroll_get(&scroll_x, &scroll_y);
  kind = AS(s32, ((SpritePiece *)shape)->kind);
  if (kind != 0x4000)
  {
    if (kind == 0x5000)
    {
      kept_x = ((s32) (x << 0x10)) >> 0x10;
      kept_y = ((s32) (y << 0x10)) >> 0x10;
      line1_screen_x_then_start_x = kept_x - scroll_x;
      ;
      packet_line_add(inline_fn(line1_screen_x_then_start_x, (s32 *) (((u8 *) shape) + 0x8)), inline_fn(kept_y - scroll_y, (s16 *) (((u8 *) shape) + 0xa)), inline_fn(line1_screen_x_then_start_x, (s32 *) (((u8 *) shape) + 0xc)), inline_fn(kept_y - scroll_y, (s16 *) (((u8 *) shape) + 0xe)), g_ShapeOutlineCol[0], g_ShapeOutlineCol[0], 0x10);
      line2_screen_x = kept_x - scroll_x;
      line2_start_x = inline_fn(line2_screen_x, (s32 *) (((u8 *) shape) + 0xc));
      line2_screen_y = kept_y - scroll_y;
      packet_line_add(line2_start_x, inline_fn(line2_screen_y, (s16 *) (((u8 *) shape) + 0xe)), inline_fn(line2_screen_x, (s32 *) (((u8 *) shape) + 0x10)), inline_fn(line2_screen_y, (s16 *) (((u8 *) shape) + 0x12)), g_ShapeOutlineCol[0], g_ShapeOutlineCol[0], 0x10);
      line3_screen_x = kept_x - scroll_x;
      ;
      packet_line_add(inline_fn(line3_screen_x, (s32 *) (((u8 *) shape) + 0x10)), inline_fn(kept_y - scroll_y, (s16 *) (((u8 *) shape) + 0x12)), inline_fn(line3_screen_x, (s32 *) (((u8 *) shape) + 0x14)), inline_fn(kept_y - scroll_y, (s16 *) (((u8 *) shape) + 0x16)), g_ShapeOutlineCol[0], g_ShapeOutlineCol[0], 0x10);
      line4_screen_x = kept_x - scroll_x;
      line1_screen_x_then_start_x = inline_fn(line4_screen_x, (s32 *) (((u8 *) shape) + 0x14));
      line4_screen_y = kept_y - scroll_y;
      packet_line_add(line1_screen_x_then_start_x, inline_fn(line4_screen_y, (s16 *) (((u8 *) shape) + 0x16)), inline_fn(line4_screen_x, (s32 *) (((u8 *) shape) + 0x8)), inline_fn(line4_screen_y, (s16 *) (((u8 *) shape) + 0xa)), g_ShapeOutlineCol[0], g_ShapeOutlineCol[0], 0x10);
    }
  }
  else
  {
    packet_box_add(inline_fn(x - scroll_x, (s32 *) (((u8 *) shape) + 0x8)), inline_fn(y - scroll_y, (s16 *) (((u8 *) shape) + 0xa)), AS(s32, ((SpritePiece *)shape)->x2), ((SpritePiece *)shape)->y2, g_ShapeBoxCol[0], 0x40);
  }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one call passes nothing, and nothing read says what it was for. */
void func_8001B94C() {

}
/* la_absolute: OutlineDraw */

extern u8 g_OutlineStartPlaces[3][6];
extern u8 g_OutlinePieceLengths[3][9];
extern s32 g_OutlinePieceCols[9];
extern u8 g_OutlineKindSizes[3][2];

/* OutlineDraw(x0, y0, kind, frame) -- draw one frame of the animated
   outline of a selection box: a lap of 36 short coloured lines round the
   border of a w x h rectangle whose top-left pixel is (x0, y0).

   Callers, both in this TU and both once a frame per live marker:
   OutlineMapStep (the party / overworld marker: record x and y minus the
   camera offset minus 1, kind = g_OutlineMapKinds[slot], frame = tick / 7) and
   OutlineScreenStep (the battle UI record marker: x - 1, y - 1, kind =
   g_OutlineScreenKinds[slot], frame = tick / 7). frame is 0..5, kind 0..2.

   Tables, all read only:
     g_OutlineKindSizes[kind] = { w, h }: 50 x 26, 26 x 26, 18 x 26 (small data).
     g_OutlineStartPlaces[kind][frame] = how far along the top edge the lap starts:
       kind 0: 0 31 25 19 13 8, kind 1: 0 21 18 14 9 5, kind 2: 0 17 14 11
       7 4. It steps back by about a sixth of the colour cycle per frame,
       so the colour bands crawl against the drawing direction.
     g_OutlinePieceLengths[kind][seg] = the length of colour step seg (0..8):
       kind 0: 5 4 4 4 4 4 4 4 4 (37), kind 1: 5 3 2 3 2 3 2 3 2 (25),
       kind 2: 5 2 2 2 2 2 2 2 2 (21). Four cycles are exactly the
       perimeter 2 * (w - 1) + 2 * (h - 1) = 148 / 100 / 84, which is why
       the lap is 36 lines whatever the kind.
     g_OutlinePieceCols[seg] = the colour of step seg, 5 bits a channel; it is
       passed times 8 as both end colours of the line: 0x1F1F1F, 0x141F1F,
       0x041F1F, 0x041A1E, 0x02161F, 0x01121F, 0x000E1F, 0x000A1F,
       0x00071F.

   In order: the pen starts at (x0 + g_OutlineStartPlaces[kind][frame], y0) heading
   right (dir 0); the far corner is x1 = x0 + w - 1, y1 = y0 + h - 1. For
   each of the 36 lines, len = g_OutlinePieceLengths[kind][seg], then:
     - dir 0 right: if the line ends short of x1 the pen moves len right;
       otherwise it stops AT x1, dir becomes 1 and the rest of the length
       (x + len - x1, which can be 0) is kept for the next edge;
     - dir 1 down to y1, dir 2 left to x0, dir 3 up to y0 the same way
       (after dir 3 it is 0 again);
     - packet_line_add(x, y, nx, ny, colour * 8, colour * 8, 0x10) draws the
       piece (a two-colour line into ordering-table entry 0x10), the pen
       moves to its end, and a line cut at a corner goes round again with
       what is left of len, in the same colour. So a line that meets a
       corner is two calls, and one that ends exactly on it draws a second,
       zero-length piece.
     - seg steps 0..8 and wraps.
   Writes no global; its only call is packet_line_add (36 times plus one per
   corner crossed). No return value (both callers are void and end with
   this call). No frame wait and no RNG draw.

   Shapes that decided the match (each measured by removing it alone):
   - g_OutlineKindSizes is a SIZED small-data array with the la_absolute marker:
     the target's same-register `lui $v1` / `addiu $v1,$v1,%lo` pair is the
     Sony assembler's one-instruction `la` of a symbol inside the gp window
     (0x800B85B4 = gp + 0x74, six bytes and two of padding below
     g_OutlineScreenFrameCounts). As a far `u8[][2]` cc1 makes its own HIGH / LO_SUM pair
     and the function is 194/211; sized without the marker it is 200/211.
   - MARKED SPELLING: the table's address is taken into the local `box`
     before it is indexed. With `g_OutlineKindSizes[kind][0]` / `[kind][1]` written
     directly (marker and declaration kept) the `la` is emitted after the
     index and the function is 194/211; `box = g_OutlineKindSizes[kind]` is 194
     too. What the 17 rows are: local-alloc gives $v0 to whichever of the
     row pointer and the x1 temporary ranks higher by
     floor_log2(refs) * refs / life, and the two are 0.636 against 0.614
     with a split far address. One `la` set early leaves the pointer 5
     references over 10 instructions (0.500). The Japanese build (GCC 2.7.2, twin at
     0x8001D610) has the same thing: the base in a register (`lui` /
     `addiu`) before the index is scaled, where the direct spelling gives
     2.7.2 two `sym($reg)` loads; this text is 108/209 against it, the
     direct spelling 103.
   - The limit is the LEFT operand of every edge test (`x1 > x + len`,
     `x0 < x - len`): with `x + len < x1` loop.c hoists a different set of
     sign extensions and x0 is extended on entry (126/211).
   - The three far tables are indexed [kind][...]; retail is the same
     bytes with the flat `[frame + kind * 6]` spelling, the Japanese twin
     is not (101/209 flat).
   - x1's sign extension on the stack ($sp+0x28), y1's `<< 16` beside it
     ($sp+0x2C) and kind in a halfword slot ($sp+0x20) are loop.c's own
     hoists and the allocator running out of registers, not variables. */
/* func_8001B954 */
void OutlineDraw(s16 x0, s16 y0, s16 kind, s16 frame)
{
  s16 x;
  s16 y;
  s16 nx;
  s16 ny;
  s16 x1;
  s16 y1;
  s16 seg;
  s16 dir;
  s16 len;
  s16 i;
  s16 more;
  u8 (*box)[2];

  seg = 0;
  dir = 0;
  ny = y0;
  y = y0;
  box = g_OutlineKindSizes;
  x1 = x0 + box[kind][0] - 1;
  y1 = y0 + box[kind][1] - 1;
  x = nx = x0 + g_OutlineStartPlaces[kind][frame];
  for (i = 0; i < 0x24; i++)
  {
    len = g_OutlinePieceLengths[kind][seg];
    more = 1;
    while (more)
    {
      switch (dir)
      {
        case 0:
          if (x1 > x + len)
          {
            nx = x + len;
            more = 0;
          }
          else
          {
            dir++;
            len = x + len - x1;
            nx = x1;
          }
          break;
        case 1:
          if (y1 > y + len)
          {
            ny = y + len;
            more = 0;
          }
          else
          {
            dir++;
            len = y + len - y1;
            ny = y1;
          }
          break;
        case 2:
          if (x0 < x - len)
          {
            nx = x - len;
            more = 0;
          }
          else
          {
            dir++;
            len -= x - x0;
            nx = x0;
          }
          break;
        case 3:
          if (y0 < y - len)
          {
            ny = y - len;
            more = 0;
          }
          else
          {
            dir = 0;
            len -= y - y0;
            ny = y0;
          }
          break;
      }
      packet_line_add(x, y, nx, ny, g_OutlinePieceCols[seg] * 8, g_OutlinePieceCols[seg] * 8, 0x10);
      x = nx;
      y = ny;
    }
    seg = (seg < 8) ? seg + 1 : 0;
  }
}

extern u8 g_OutlineMapFrameCounts[];

/* func_8001BCA0 */
void OutlineMapStep(void *records, s32 record_number, s32 mode)
{
  s32 slot;
  u8 *cnt;
  s32 frame;
  s32 scroll_x;
  s32 scroll_y;
  slot = record_number;
  if (mode != 0)
  {
    slot = record_number + 7;
    goto check;
  }
  if (slot < 0)
  {
    goto check;
  }
  if (slot >= 5)
  {
    goto check;
  }
  if ((*((u16 *) ((((u8 *) records) + (slot * 60)) + 4))) != 0)
  {
    goto draw;
  }
check:
  if (mode != 1)
  {
    return;
  }
  if (record_number < 2)
  {
    return;
  }
  if (record_number >= 5)
  {
    return;
  }
  if ((*((u8 *) (((u8 *) records) + (record_number * 60)))) == 0)
  {
    return;
  }
draw:
  if (((*((u16 *) ((((u8 *) records) + (record_number * 60)) + 0x30))) & 1) == 0)
  {
    cnt = &g_OutlineMapFrameCounts[slot];
    *cnt = (*cnt < 0x29) ? *cnt + 1 : 0;
  }
  frame = (g_OutlineMapFrameCounts[slot] / 7) & 0xFF;
  bg_scroll_get(&scroll_x, &scroll_y);
  OutlineDraw((s16) (((*((u16 *) ((((u8 *) records) + (record_number * 60)) + 6))) - ((u16) scroll_x)) - 1), (s16) (((*((u16 *) ((((u8 *) records) + (record_number * 60)) + 0xA))) - ((u16) scroll_y)) - 1), g_OutlineMapKinds[slot], frame);
}
/* la_absolute: OutlineScreenStep */
extern u8 g_OutlineScreenFrameCounts[4];
extern u8 g_OutlineScreenKinds[4];

/* OutlineScreenStep(records, slot, unused) -- the animated marker of one battle
   UI record: OutlineMapStep's twin over 0x40-byte records, for slots 2 and 3
   only, with its own two tables and no camera offset.

   The one caller is BtlMenuOutlineStep (btl_command.c), once a frame for slot = 1, 2
   and 3 as (g_BtlMenuMarkRecords, slot, 0); slot 1 falls out at the first test. The
   third argument is never read here (it is OutlineMapStep's mode flag; the
   caller's bytes pass 0).

   In order, for slot 2 or 3 whose record's u16 +0x00 is nonzero (the record
   is live):
   1. Unless bit 0 of the record's u16 +0x30 is set (the marker is frozen),
      the slot's tick counter g_OutlineScreenFrameCounts[slot] steps: c < 0x29 ? c + 1 : 0,
      so it runs 0..0x29, 42 ticks a cycle.
   2. OutlineDraw(x - 1, y - 1, kind, frame) draws the marker: x and y are
      the record's u16 +0x06 and +0x0A (each minus 1, as s16), kind is
      g_OutlineScreenKinds[slot] and frame is the counter / 7, 0..5: six frames of
      seven ticks.
   Reads the record (+0x00, +0x06, +0x0A, +0x30) and g_OutlineScreenKinds[slot];
   reads and writes g_OutlineScreenFrameCounts[slot]. Returns nothing the caller reads. No
   RNG draw and no frame wait of its own; OutlineDraw's only call is the
   line primitive packet_line_add.

   Shapes that decided the match:
   - g_OutlineScreenFrameCounts and g_OutlineScreenKinds are SIZED small-data arrays (0x800B85BC..BF
     and ..C0..C3, inside the gp window) with the la_absolute marker: the
     target's same-register `lui $v0` / `addiu $v0,$v0,%lo` pairs are the
     assembler's `la`, one instruction to the scheduler. As far `u8[]` the
     same C is 57/59: cc1's own split pair is two instructions, so the
     second table's address is one row longer and `multu` (queued 12 cycles
     behind `mfhi`) comes ready one row late, after the first `lhu`; the
     `ori` of 0x24924925 moves with it. Sized without the marker: 50/59.
   - The divide is written inside the call (the fourth argument is evaluated
     last); as a local set before the call the whole divide is scheduled
     first (50/59).
   - The (s16) casts are written out because the TU has no prototype for
     OutlineDraw in scope; the bytes are the same under every prototype
     tried for it (s16 x 4, s32/s32/s16/s16, s16/s16/u8/u8, old-style). */
/* func_8001BE30 */
void OutlineScreenStep(void *records, s32 record_number, s32 unused)
{
  if (record_number < 2 || record_number >= 4)
  {
    return;
  }
  if (*(u16 *) ((u8 *) records + record_number * 64) == 0)
  {
    return;
  }
  if ((*(u16 *) ((u8 *) records + record_number * 64 + 0x30) & 1) == 0)
  {
    g_OutlineScreenFrameCounts[record_number] = (g_OutlineScreenFrameCounts[record_number] < 0x29) ? g_OutlineScreenFrameCounts[record_number] + 1 : 0;
  }
  OutlineDraw((s16) (*(u16 *) ((u8 *) records + record_number * 64 + 6) - 1), (s16) (*(u16 *) ((u8 *) records + record_number * 64 + 0xA) - 1), g_OutlineScreenKinds[record_number], g_OutlineScreenFrameCounts[record_number] / 7);
}

/* far_syms: g_ShopStockRows (target reaches it %hi/%lo) */
extern ShopStockRow g_ShopStockRows[];

/* func_8001BF1C */
u32 ShopStockGet(u32 stock_row)
{
  u32 base;

  stock_row &= 0xFF;
  if (stock_row >= 0x28)
  {
    return (u32)&g_ShopStockRows[0x27];
  }

  base = (u32)g_ShopStockRows;
  return stock_row * 6 + base;
}

s32 fmt_sprintf(s8 *, u8 *, ...);     /* sprintf */
int strlen(s8 *s);
extern u8 g_CardSaveTitleFormat[];
extern u8 g_CardSaveTitleWideChars[];
extern u8 *g_CardSavePlaceNames[];

/* card_save_title_set -- the memory-card save's TITLE: the 64-byte
   Shift-JIS field at +4 of the save file's header. Its one caller,
   card_save_header_set (the header builder: "SC", 0x13, 1 at +0..+3),
   passes buf = header + 4, the save slot, the location id g_MapPlaceName,
   the byte printed as the level (+0x1C of the 0x40-byte record at
   g_ChrRecords that *g_PartyIds indexes) and the play time in minutes
   g_PlayTimeMinutes; the save writer MenuSaveWrite supplies them.

     loc -= 0x51                                    (as a u16)
     sprintf(buf, "LUNAR-%02d %d:%02d L%d %s",      g_CardSaveTitleFormat
             slot + 1, time / 60, time % 60, level, g_CardSavePlaceNames[loc])
         -- the ASCII text, written straight into buf; g_CardSavePlaceNames is the
            EXE's 84 short location names, index id - 0x51 (no range check)
     buf[strlen(buf) * 2]     = 0
     buf[strlen(buf) * 2 + 1] = 0                   the two-byte terminator
                                                    of the widened string
     for (i = strlen(buf) - 1; i >= 0; i--)         widen IN PLACE, last
         c = (buf[i] - 0x20) * 2                    character first:
         buf[i * 2]     = g_CardSaveTitleWideChars[c]             ASCII -> the two-byte
         buf[i * 2 + 1] = g_CardSaveTitleWideChars[c + 1]         full-width code

   g_CardSaveTitleWideChars is 0x60 two-byte entries for 0x20 .. 0x7F, stored high byte
   first: space 81 40, '0'.. 82 4F.., 'A'.. 82 60.., 'a'.. 82 81.., the
   punctuation its full-width form ('-' 81 7C, ':' 81 46, '.' 81 44,
   '\'' 81 66), 0x7F -> 00 00. The byte is read SIGNED and nothing
   range-checks it; sprintf's output here is all 0x20 .. 0x7E.

   Three strlen calls, all on the still-ASCII text (the first terminator
   store lands past it and does not shorten it). The widened title is
   2 * len + 2 bytes: 32 characters -- a two-digit hour, a two-digit
   level and a 13-letter name -- fill the 64-byte field exactly and put
   the terminator on header +0x44; a 14-letter name (three exist) or a
   three-digit hour can go past it. The caller's bzero(header + 0x44,
   0x1C) runs after this and cuts whatever went over.

   No RNG draw, no frame wait; writes only through buf; no return value.

   Shapes the bytes depend on (each measured):
   - `loc -= 0x51;` is a statement of its own (or a u16 local set from
     it) -- 68 -> 69. Written inside the call as
     g_CardSavePlaceNames[(u16) (loc - 0x51)] the level's mask is scheduled ahead of
     the subtraction instead of between it and its mask. The parameters'
     own widths (u16 or s32 with a cast at the use) do not move that row.
   - time is a u16 and the divide and the remainder are written on it
     directly: a u16 divided by a constant is narrowed to an UNSIGNED
     divide (multu by 0x88888889, srl 5) with an andi after each result
     (s32 time: 64, u32 time: 66).
   - c holds the doubled table index, so buf[i] is read once per pass
     although two byte stores through buf follow it (the expression
     written out twice: 62). buf is signed char (u8 *: 68, an lbu).
   - strlen is called three times; one call kept in a local scores 61.
   - the pad: without it 62; any unused aggregate of 25-32 bytes gives
     the frame. */
/* func_8001BF50 */
void card_save_title_set(s8 *buf, u16 slot, u16 loc, u16 level, u16 time)
{
    u8 pad[32];     /* MARKED SPELLING (unused local): the target frame (0x48) holds
                       32 bytes at 0x20-0x3F that no instruction touches -- the
                       original's local(s), declaration unknown */
    s32 i;
    s32 c;

    loc -= 0x51;
    fmt_sprintf(buf, g_CardSaveTitleFormat, slot + 1, time / 60, time % 60, level, g_CardSavePlaceNames[loc]);
    buf[strlen(buf) * 2] = 0;
    buf[strlen(buf) * 2 + 1] = 0;
    for (i = strlen(buf) - 1; i >= 0; i--)
    {
        c = (buf[i] - 0x20) * 2;
        buf[i * 2] = g_CardSaveTitleWideChars[c];
        buf[i * 2 + 1] = g_CardSaveTitleWideChars[c + 1];
    }
}

/* far_syms: g_PlayTimeFrames (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PlayTimeMinutes (auto: target reaches it %hi/%lo — addressing row 3) */
extern u16 g_PlayTimeFrames;

/* func_8001C064 */
void PlayTimeStep(void)
{
    g_PlayTimeFrames++;
    if (g_PlayTimeFrames >= 0xE10) {
        g_PlayTimeMinutes++;
        if (0xEA5F < g_PlayTimeMinutes) {
            g_PlayTimeMinutes += 0x15A0;
        }
        g_PlayTimeFrames = 0;
    }
}
