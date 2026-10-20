/* Lunar SSSC — tile_layer: the tile layer: its colour, reset and end, the
   wave's strips, the grid's cells, draw and step, scroll, resource,
   patterns, texture pages and its mix fade. The first split had it in ui.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_PacketWritePointer (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
/* far_syms: g_PacketOtBase (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
extern s8 g_TileWaveRowShifts[];
extern u8 *g_PacketWritePointer;
extern u32 *g_PacketOtBase;
extern u8 tile_lookup_table[];
extern u8 g_TilePatterns[];
extern s32 g_TileCol;
extern s8 g_TileLayerShown;
extern u8 g_TileWaveSize;
extern u8 g_TileWaveStartRow;
extern u8 g_TileWaveShown;
extern s16 g_TileScrollX;
extern s16 g_TileScrollY;
extern s8 D_800B865B;
extern u8 g_TileTextureHighest;
extern s8 g_TileSemiTransOn;
extern s8 g_TileEmptyFillOn;
extern s32 g_TileMixFadeLength;
extern u8 g_TileResourceRows[];
extern u8 g_TileTPageFirst;
extern s32 D_800B89B0;
extern s32 g_TileMixFadeEnd;
extern s32 g_TileMixFadeCount;
extern s32 g_TileMixFadeDifference;
extern s32 g_TileMixFadeStart;
s32 resource_load(s32, s32);                               /* extern */
s32 tile_tpage_first_get_copy(u8);
void tile_col_set(s32 col);
s32 tile_grid_clear(void);
void tile_layer_reset(void);
void tile_layer_end(void);
void tile_wave_set(s8 amplitude, s8 phase);
void tile_wave_switch_set(s8 on);
u16 *tile_cell_address();
s32 tile_cell_get();
void tile_layer_draw(s32, s32);
s32 tile_mix_fade_step(void);
void tile_layer_step(void);
void tile_scroll_move(s32 dx, s32 dy);
void tile_scroll_set(s16 x, s16 y);
void tile_resource_load(s32 effect_id);
void func_8001DC60(void);
s32 tile_tpage_first_get(void);
s32 tile_tpage_get(u8 texture);
void func_8001DCDC(void);
void tile_semi_trans_set(s8 on);
void tile_col_grey_set(s32 grey);
s32 BtlBgGreySet(s32);
void tile_mix_set(s32 level);
void tile_empty_fill_set(s8 on);
void tile_mix_fade_start(s32 arg0, s32 length, s32 level);
s32 tile_mix_fade_check(void);
void tile_mix_fade_stop(void);
void tile_col_set(s32 col);
s32 tile_grid_clear();
void tile_layer_reset(void);
void tile_layer_end(void);
void tile_wave_set(s8 amplitude, s8 phase);
void tile_wave_switch_set(s8 on);
u16 *tile_cell_address();
s32 tile_cell_get();
void tile_layer_draw(s32, s32);
s32 tile_mix_fade_step(void);
void tile_layer_step(void);
void tile_scroll_move(s32 dx, s32 dy);
void tile_scroll_set(s16 x, s16 y);
void tile_resource_load(s32 effect_id);
void func_8001DC60(void);
s32 tile_tpage_first_get(void);
s32 tile_tpage_get(u8 texture);
void func_8001DCDC(void);
void tile_semi_trans_set(s8 on);
void tile_col_grey_set(s32 grey);
s32 BtlBgGreySet(s32);
void tile_mix_set(s32 level);
void tile_empty_fill_set(s8 on);
void tile_mix_fade_start(s32 arg0, s32 length, s32 level);
s32 tile_mix_fade_check(void);
void tile_mix_fade_stop(void);
void tile_pattern_set(s32 pattern, s32 pattern_address);
s32 tile_pattern_get(s32 pattern);
void tile_pattern_put(s32 pattern, s32 column, s32 row, s32 width, s32 height);

/* func_8001D278 */
void tile_col_set(s32 col) {
    g_TileCol = col;
}

s32 tile_grid_clear();                                  /* extern */

/* func_8001D284 */
void tile_layer_reset(void) {
    tile_grid_clear();
    g_TileLayerShown = 0;
    g_TileEmptyFillOn = 0;
    g_TileSemiTransOn = 0;
    g_TileScrollY = 0;
    g_TileScrollX = 0;
    g_TileWaveStartRow = 0;
    g_TileWaveSize = 0;
    tile_col_set(0x808080);
}

/* func_8001D2CC */
void tile_layer_end(void) {
    g_TileLayerShown = 0;
    tile_col_set(0x808080);
}

/* func_8001D2F4 */
void tile_wave_set(s8 amplitude, s8 phase) {
    g_TileWaveSize = amplitude;
    g_TileWaveStartRow = phase;
}

/* func_8001D304 */
void tile_wave_switch_set(s8 on) {
    g_TileWaveShown = on;
}

/* One sheared strip of a wave-layer tile, rows yo .. yo + h - 1 of its 16:
   xa / xb are the x offsets of the strip's top and bottom edge, h == 0 a
   strip that is not drawn. tile_wave_strips_make fills the three at g_TileWaveStrips,
   tile_layer_draw draws them. Declared here, above its first user. */
void tile_wave_strips_make();
extern TileWaveStrip g_TileWaveStrips[];
extern s8 g_TileWaveTable[];

/* tile_wave_strips_make -- the WAVE layer's shear set-up: tile_layer_draw
   (its only caller in the image) runs it once per layer draw when
   g_TileWaveShown && g_TileWaveSize, before it draws each 16 x 16 tile as up to
   three sheared POLY_FT4 strips from g_TileWaveStrips.

   The wave is the 16-entry triangle table g_TileWaveTable (0 1 2 3 4 3 2 1 0
   -1 -2 -3 -4 -3 -2 -1), read from the phase g_TileWaveStartRow on, and scaled by
   the amplitude g_TileWaveSize (tile_wave_set(amp, phase) sets both; this
   function advances neither):

     for row i = 0..15 of a tile
       v = g_TileWaveTable[(g_TileWaveStartRow + i) % 16]
       v == 4 or v == -4: peak[n++] = i        (the wave's two turning rows)
       g_TileWaveRowShifts[i] = (v * g_TileWaveSize + 2) >> 2     the row's x offset

   A triangle wave is straight between its turning rows, so a tile needs at
   most three strips {xa = x offset of the top edge, xb = of the bottom
   edge, yo = first row, h = rows}; all three h are zeroed first (h == 0
   is a strip the drawer skips):

     peak[0] != 0:  { off[0],       off[peak[0]], 0,       peak[0]           }
     always:        { off[peak[0]], off[peak[1]], peak[0], peak[1] - peak[0] }
     always:        { off[peak[1]], off[0],       peak[1], 16 - peak[1]      }

   The last strip's bottom edge is off[0]: row 16 is row 0 of the tile
   below, one period on. With peak[0] == 0 only two strips are filled and
   the third keeps h = 0. Every rotation of the table has exactly two
   entries of magnitude 4, so two peak rows are always found.

   No RNG draw, no frame wait, no calls; nothing is drawn here.

   Shapes the bytes depend on (each measured):
   - `v` is reused for the scaled value and the store reads it back: the
     result then stays in v's register, the compare for the loop no longer
     clobbers it and the store fills the branch's delay slot (72 -> 94).
     Written `g_TileWaveRowShifts[i] = (v * amp + 2) >> 2` the value is a temporary
     in $v0 and the store sits before the slti.
   - the three zero stores go through the ARRAY (g_TileWaveStrips[k].h = 0) and
     the walking pointer is set after them (94 -> 98). cse gives the three
     constant addresses one single-set register with a known value, so the
     scheduler can lift the load of peak[0] above the stores; through `w`
     (set three times, no known value) the load stays behind them and a
     nop follows it. A second pointer `b = g_TileWaveStrips; b[k].h = 0; w = b;`
     gives the same bytes.
   - `% 16` on the int sum (the bgez / addiu 0xF / sra / sll / subu
     sequence); `& 15` is one andi (90).
   - v is an int (s8: 87); peak[2] and peak[8] give the same 8-byte frame
     (16 does not); g_TileWaveRowShifts as u8 or s8 the same loads (a byte copied
     to a byte is an lbu either way). */
/* func_8001D310 */
void tile_wave_strips_make(void)
{
    u8 peak[2];
    s32 i;
    u8 n;
    s32 v;
    TileWaveStrip *w;

    n = 0;
    for (i = 0; i < 16; i++) {
        v = g_TileWaveTable[(g_TileWaveStartRow + i) % 16];
        if (v == 4 || v == -4) {
            peak[n] = i;
            n++;
        }
        v = (v * g_TileWaveSize + 2) >> 2;
        g_TileWaveRowShifts[i] = v;
    }
    g_TileWaveStrips[0].row_count = 0;
    g_TileWaveStrips[1].row_count = 0;
    g_TileWaveStrips[2].row_count = 0;
    w = g_TileWaveStrips;
    if (peak[0] != 0) {
        w->top_shift = g_TileWaveRowShifts[0];
        w->bottom_shift = g_TileWaveRowShifts[peak[0]];
        w->first_row = 0;
        w->row_count = peak[0];
        w++;
    }
    w->top_shift = g_TileWaveRowShifts[peak[0]];
    w->bottom_shift = g_TileWaveRowShifts[peak[1]];
    w->first_row = peak[0];
    w->row_count = peak[1] - peak[0];
    w++;
    w->top_shift = g_TileWaveRowShifts[peak[1]];
    w->bottom_shift = g_TileWaveRowShifts[0];
    w->first_row = peak[1];
    w->row_count = 0x10 - peak[1];
}

/* func_8001D498 */
u16 *tile_cell_address(column, row) s32 column; s32 row;
{
  int masked_column;
  u8 *cells;
  int cell_index;
  masked_column = column & 0x1F;
  cell_index = ((row & 0x1F) << 5) + masked_column;

  cells = tile_lookup_table;
  return (cell_index * 2) + cells;
}

u16 *tile_cell_address();                               /* extern */

/* func_8001D4BC */
s32 tile_cell_get() {
    return *tile_cell_address();
}
/* far_syms: g_PacketWritePointer */
/* far_syms: g_PacketOtBase */
extern u32 g_TileTPages[];
extern u32 g_TileCluts[];

/* The 16x16 tile-map layer drawer. arg0/scroll_y = the layer's pixel scroll (x, y):
   for 15 rows x columns 0..0x14 (-2..0x18 after tile_wave_strips_make when the wave layer
   g_TileWaveShown && g_TileWaveSize is on), tile = tile_cell_get(col + (x >> 4), row + (y >> 4));
   0xFFFF = empty (drawn as tile 0 when g_TileEmptyFillOn). Per tile: v = tile & 0xF0,
   u = (tile & 0xF) << 4, flip = tile & 0xC000, pal = ((g_TileTPageFirst - 0x19) / 2 + 4 +
   ((tile & 0x1FF) >> 8)) | 0x20, clut = g_TileCluts[pal >> 3], tpage =
   g_TileTPages[pal & 7]; one of three packets into the scratchpad buffer g_PacketWritePointer,
   linked at OT slot z = 0x300 of g_PacketOtBase:
     wave layer -> up to 3 POLY_FT4 strips from g_TileWaveStrips[3] {xa, xb, yo, h} (h == 0
                   skips the strip; xa/xb shear the top/bottom edge, yo/h cut the rows);
     flipped    -> one POLY_FT4 (u/v swapped per 0x4000/0x8000, far edge clamped 0..0xFF);
     else       -> DR_MODE word (tpage | 0xE1000200) + SPRT_16.
   g_TileCol = the RGB word, g_TileSemiTransOn = semi-transparent (code |= 2, tpage |= 0x20).
   No RNG.
   Load-bearing shapes (each costs rows if "tidied"): the strip loop is a real for
   loop that reads g_TileTPages[pal & 7] INSIDE it while clutp is taken before it (i = 0
   first); the flipped and sprite paths share one `tb = g_TileTPages` variable; the
   sprite's DR_MODE constant is parked in the dead `tile` before `tb` is set; the
   per-path copies (vt/xl/xr, xl2/xr2/yt2/yb2) and one block-local `ot` per tail. */
/* func_8001D4E0 */
void tile_layer_draw(s32 scroll_x, s32 scroll_y)
{
  s32 x0;
  s32 y0;
  s32 fx;
  s32 fy;
  s32 start;
  s32 end;
  s32 row;
  s32 col;
  s32 tile;
  s32 v;
  s32 flip;
  s32 z;
  s32 pal;
  s32 u;
  s32 x;
  s32 y;
  s32 i;
  u32 *clutp;
  TileWaveStrip *wave;
  s32 uL;
  s32 uR;
  s32 vT;
  s32 vB;
  POLY_FT4 *p;
  PacketSprite16 *s;
  u32 *tb;
  start = 0;
  end = 0x14;
  if ((g_TileWaveShown != 0) && (g_TileWaveSize != 0))
  {
    tile_wave_strips_make();
    start = -2;
    end = 0x18;
  }
  x0 = scroll_x >> 4;
  y0 = scroll_y >> 4;
  fx = scroll_x & 0xF;
  fy = scroll_y & 0xF;
  for (row = 0; row < 15; row++)
  {
    for (col = start; col <= end; col++)
    {
      tile = tile_cell_get(col + x0, row + y0);
      if ((g_TileEmptyFillOn != 0) && (tile == 0xFFFF))
      {
        tile = 0;
      }
      if (tile == 0xFFFF)
      {
        continue;
      }
      v = tile & 0xF0;
      flip = tile & 0xC000;
      z = 0x300;
      pal = ((g_TileTPageFirst - 0x19) / 2) + 4;
      pal += (tile & 0x1FF) >> 8;
      pal |= 0x20;
      u = (tile & 0xF) << 4;
      x = (col << 4) - fx;
      y = (row << 4) - fy;
      if ((g_TileWaveShown != 0) && (g_TileWaveSize != 0))
      {
        i = 0;
        clutp = &g_TileCluts[pal >> 3];
        for (; i < 3; i++)
        {
          s32 h;
          s32 yo;
          s32 xa;
          s32 xb;
          s32 ya;
          s32 xl;
          s32 xr;
          s32 yb;
          s32 vt;
          wave = &g_TileWaveStrips[i];
          if (wave->row_count != 0)
          {
            h = wave->row_count;
            vt = v;
            xl = x;
            xr = x + 16;
            yo = wave->first_row;
            xa = wave->top_shift;
            xb = wave->bottom_shift;
            ya = y + yo;
            yb = ya + h;
            if (flip & 0x4000)
            {
              uR = u - 1;
              uL = u + 15;
              if (uR < 0)
              {
                uR = 0;
              }
            }
            else
            {
              uR = u + 16;
              uL = u;
              if (uR >= 0x100)
              {
                uR = 0xFF;
              }
            }
            if (flip & 0x8000)
            {
              vt += yo;
              vT = (vt + h) - 1;
              vB = vt - 1;
              if (vB < 0)
              {
                vB = 0;
              }
            }
            else
            {
              vT = vt + yo;
              vB = vT + h;
              if (vB >= 0x100)
              {
                vB = 0xFF;
              }
            }
            p = (POLY_FT4 *) g_PacketWritePointer;
            *((u32 *) (&p->r0)) = g_TileCol | 0x2C000000;
            p->x0 = xl + xa;
            p->y0 = ya;
            p->x1 = xr + xa;
            p->y1 = ya;
            p->x2 = xl + xb;
            p->y2 = yb;
            p->x3 = xr + xb;
            p->y3 = yb;
            p->u0 = uL;
            p->v0 = vT;
            p->u1 = uR;
            p->v1 = vT;
            p->u2 = uL;
            p->v2 = vB;
            p->u3 = uR;
            p->v3 = vB;
            p->clut = *clutp;
            p->tpage = g_TileTPages[pal & 7];
            if (g_TileSemiTransOn != 0)
            {
              p->code |= 2;
              p->tpage |= 0x20;
            }
            {
              u32 *ot;
              ot = (u32 *) ((z << 2) + ((s32) g_PacketOtBase));
              p->tag = (*ot) | 0x09000000;
              *ot = ((u32) p) + 0x80000000;
            }
            p++;
            g_PacketWritePointer = (u8 *) p;
          }
        }
      }
      else
      {
        if (flip != 0)
        {
          s32 xl2;
          s32 xr2;
          s32 yt2;
          s32 yb2;
          xl2 = x;
          xr2 = xl2 + 16;
          yt2 = y;
          yb2 = yt2 + 16;
          if (flip & 0x4000)
          {
            uR = u - 1;
            uL = u + 15;
            if (uR < 0)
            {
              uR = 0;
            }
          }
          else
          {
            uL = u;
            uR = uL + 16;
            if (uR >= 0x100)
            {
              uR = 0xFF;
            }
          }
          if (flip & 0x8000)
          {
            vB = v - 1;
            vT = v + 15;
            if (vB < 0)
            {
              vB = 0;
            }
          }
          else
          {
            vT = v;
            vB = vT + 16;
            if (vB >= 0x100)
            {
              vB = 0xFF;
            }
          }
          p = (POLY_FT4 *) g_PacketWritePointer;
          *((u32 *) (&p->r0)) = g_TileCol | 0x2C000000;
          p->x0 = xl2;
          p->y0 = yt2;
          p->x1 = xr2;
          p->y1 = yt2;
          p->x2 = xl2;
          p->y2 = yb2;
          p->x3 = xr2;
          p->y3 = yb2;
          p->u0 = uL;
          p->v0 = vT;
          p->u1 = uR;
          p->v1 = vT;
          p->u2 = uL;
          p->v2 = vB;
          p->u3 = uR;
          p->v3 = vB;
          p->clut = g_TileCluts[pal >> 3];
          tb = g_TileTPages;
          p->tpage = tb[pal & 7];
          if (g_TileSemiTransOn != 0)
          {
            p->code |= 2;
            p->tpage |= 0x20;
          }
          {
            u32 *ot;
            ot = (u32 *) ((z << 2) + ((s32) g_PacketOtBase));
            p->tag = (*ot) | 0x09000000;
            *ot = ((u32) p) + 0x80000000;
          }
          p++;
          g_PacketWritePointer = (u8 *) p;
        }
        else
        {
          s = (PacketSprite16 *) g_PacketWritePointer;
          tile = 0xE1000200;
          tb = g_TileTPages;
          s->mode = tb[pal & 7] | tile;
          *((u32 *) (&s->r0)) = g_TileCol | 0x7C000000;
          AS(short, s->x0) = x;
          AS(short, s->y0) = y;
          s->u0 = u;
          s->v0 = v;
          s->clut = g_TileCluts[pal >> 3];
          if (g_TileSemiTransOn != 0)
          {
            s->mode |= 0x20;
            s->code |= 2;
          }
          {
            u32 *ot;
            ot = (u32 *) ((z << 2) + ((s32) g_PacketOtBase));
            s->tag = (*ot) | 0x04000000;
            *ot = ((u32) s) + 0x80000000;
          }
          s++;
          g_PacketWritePointer = (u8 *) s;
        }
      }
    }
  }
}

void tile_layer_draw(s32, s32);                          /* extern */
s32 tile_mix_fade_step();                                  /* extern */

/* func_8001DA78 */
void tile_layer_step(void) {
    if ((u8) g_TileLayerShown == 1) {
        tile_layer_draw((u16) g_TileScrollX, (u16) g_TileScrollY);
    }
    tile_mix_fade_step();
}
/* func_8001DAB4 */
void tile_scroll_move(s32 dx, s32 dy) {
    g_TileScrollX = (u16) g_TileScrollX + dx;
    g_TileScrollY = (u16) g_TileScrollY + dy;
}

/* func_8001DAD4 */
void tile_scroll_set(s16 x, s16 y) {
    g_TileScrollX = x;
    g_TileScrollY = y;
}

/* func_8001DAE4 */
void tile_resource_load(s32 effect_id)
{
  TileResourceRow *row;
  row = (TileResourceRow *) g_TileResourceRows;
  loop_1:
  if (row->effect != effect_id)
  {
    row++;
    goto loop_1;
  }

  resource_load(row->resource, 0x80156000);
  resource_command_run(0x80156000);
  g_TileTPageFirst = 0x19;
}
/* far_syms: g_TilePatterns (target reaches it %hi/%lo) */

/* func_8001DB38 */
void tile_pattern_set(s32 pattern, s32 pattern_address)
{
    ((s32 *)g_TilePatterns)[pattern] = pattern_address;
}
/* func_8001DB50 */
s32 tile_pattern_get(s32 pattern)
{
    return ((s32 *)g_TilePatterns)[pattern];
}

/* func_8001DB6C */
void tile_pattern_put(s32 pattern, s32 column, s32 row, s32 width, s32 height)
{
  s32 i;
  u16 *src;
  u16 *dst;
  s32 j;
  i = 0;
  src = (u16 *) tile_pattern_get(pattern);
  if (height > i)
  {
    i = 0;
    do
    {
      dst = tile_cell_address(column, row + i);
      j = 0;
      if (width > 0)
      {
        do
        {
          *dst = *src;
          src++;
          j++;
          dst++;
        }
        while (j < width);
      }
      i++;
    }
    while (i < height);
  }
}

/* func_8001DC14 */
s32 tile_grid_clear(void)
{
  s32 rows_left;
  s32 columns_left;
  int step;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `empty_cell` only holds the constant 0xFFFF; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 19 instruction lines
     differs. */
  int empty_cell;
  u16 *cell;
  cell = tile_cell_address(0, 0);
  empty_cell = 0xFFFF;
  rows_left = (unsigned char) 0x1F;
  do
  {
    columns_left = 0x1F;
    step = 2;
    loop_2:
    *cell = empty_cell;

    columns_left -= (step = 1);
    cell += step;
    if (columns_left >= 0)
    {
      goto loop_2;
    }
    rows_left -= step;
  }
  while (rows_left >= 0);
  return (s32) cell;
}

/* Ruling of 2026-10-14 (docs/naming.md): Its two stores change nothing the program uses, and its callers do not say what for. */
void func_8001DC60(void) {
    D_800B865B = 0x19;
    g_TileTextureHighest = 0;
}

/* func_8001DC74 */
s32 tile_tpage_first_get(void) {
    D_800B865B = 0x19;
    return 0x19;
}
/* Ruling of 2026-10-14 (docs/naming.md): Does what tile_tpage_first_get does; nothing read says why there are two. */
/* func_8001DC84 */
s32 tile_tpage_first_get_copy(u8 unused) {
    D_800B865B = 0x19;
    return 0x19;
}

/* func_8001DC94 */
s32 tile_tpage_get(u8 texture) {
    s32 first_tpage;
    u32 masked_texture;

    masked_texture = texture & 0xFF;
    if (masked_texture >= (u8) g_TileTextureHighest) {
        g_TileTextureHighest = texture;
    }
    first_tpage = tile_tpage_first_get();
    g_TileTPageFirst = first_tpage;
    return (first_tpage + (masked_texture * 2)) & 0xFF;
}

/* Ruling of 2026-10-14 (docs/naming.md): Its one effect, a store nothing reads, does not say what its one call was for. */
void func_8001DCDC(void) {
    tile_tpage_first_get_copy(g_TileTextureHighest);
}

/* func_8001DD00 */
void tile_semi_trans_set(s8 on) {
    g_TileSemiTransOn = on;
}
/* func_8001DD0C */
void tile_col_grey_set(s32 grey) {
    tile_col_set(grey | (grey << 8) | (grey << 0x10));
}
s32 BtlBgGreySet(s32);                               /* extern */

/* func_8001DD38 */
void tile_mix_set(s32 level) {
    s32 applied_level;
    s32 grey;
    s8 semi_trans_on;

    applied_level = level;
    if (applied_level == -1) {
        applied_level = 0;
    }
    grey = applied_level * 4;
    if (applied_level >= 0x1F) {
        grey = 0x80;
        semi_trans_on = 0;
    } else {
        semi_trans_on = 1;
    }
    tile_semi_trans_set(semi_trans_on);
    tile_col_grey_set(grey);
    BtlBgGreySet(0x80 - grey);
}

/* func_8001DD98 */
void tile_empty_fill_set(s8 on) {
    g_TileEmptyFillOn = on;
}

/* func_8001DDA4 */
void tile_mix_fade_start(s32 arg0, s32 length, s32 level)
{
  s32 start_level;
  s32 end_level;
  end_level = 0x1F - level;
  D_800B89B0 = arg0;
  g_TileMixFadeLength = length;
  g_TileMixFadeEnd = end_level;
  g_TileMixFadeCount = 0;
  start_level = 2;
  start_level = ((s32) ((char) g_TileCol)) >> start_level;
  g_TileMixFadeStart = start_level;
  if (start_level >= 0x20)
  {
    g_TileMixFadeStart = 0x20;
  }
  g_TileMixFadeDifference = end_level - g_TileMixFadeStart;
}

/* func_8001DDF0 */
s32 tile_mix_fade_check(void) {
    return g_TileMixFadeLength != 0;
}

/* func_8001DDFC */
s32 tile_mix_fade_step(void) {
    s32 frames_done;
    s32 level;

    if (g_TileMixFadeLength != 0) {
        frames_done = g_TileMixFadeCount + 1;
        g_TileMixFadeCount = frames_done;
        level = g_TileMixFadeStart + ((s32) (g_TileMixFadeDifference * frames_done) / (s32) g_TileMixFadeLength);
        if (frames_done >= g_TileMixFadeLength) {
            level = g_TileMixFadeEnd;
            g_TileMixFadeLength = 0;
        }
        tile_mix_set(level);
    }
}

/* func_8001DE88 */
void tile_mix_fade_stop(void) {
    g_TileMixFadeLength = 0;
}
