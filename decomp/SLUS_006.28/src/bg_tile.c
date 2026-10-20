/* Lunar SSSC — bg_tile: the bg: its two tile layers' draws and scrolls, the
   tile data's slots, loads, sets and copies (bg_copy, the developers' name)
   and the attribute map's twins of them. The first split had it in
   rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_PacketWritePointer (target reaches it %hi/%lo: lui/lw to load, lui $at/sw to store) */
/* far_syms: g_PacketWritePointer g_PacketOtBase */

#include "include_asm.h"
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
/* sprite_reset's decode targets for resource 2's three animation banks.
   g_SpriteDataAddresses (rodata) = { 0x800DE088, 0x800DDFB0, 0x800DF370 }. */
/* bg_first_draw's two packets: a POLY_FT4 for a mirrored tile, and a draw-mode
   word + 16x16 sprite for a plain one.  OtCell is the one-member struct view
   it reads the ordering-table pointer (scratchpad +0x40) through. */
typedef struct { u32 *ot; } OtCell;
extern u32 *g_PacketOtBase;
extern u8 g_BgSlotBlocks[];
void bg_first_draw(s32, s32);
extern u8 *g_PacketWritePointer;
/* far_syms: g_BgSecondPacketCol (scratchpad cell the symbol table names, as its two neighbours) */
extern u32 g_BgSecondPacketCol;
extern u8 secondary_tile_table[];
extern long HWD0;
extern long VWD0;
extern void *g_BgDataBuffer;
extern void *g_BgBlockEntries;
extern void *g_BgBlockBase;
extern void *g_BgAttributeDataBuffer;
extern void *g_BgAttributeBlockEntries;
extern void *g_BgAttributeBlockBase;
s32 bg_data_reset(s32);
extern s32 g_BgScrollX;
extern s32 g_BgScrollY;
extern s32 g_BgSecondScrollX;
extern s32 g_BgSecondScrollY;
extern s8 D_800B8558;
/* func_80014014 */
void bg_col_set(s32 col) {
    *(s32 *)0x1F800044 = col & 0x02FFFFFF;
}

/* func_80014030 */
void bg_scroll_get(s32 *x, s32 *y) {
    *x = g_BgScrollX;
    *y = g_BgScrollY;
}

extern s32 D_800B88A0;
extern s32 D_800B88A4;

/* Ruling of 2026-10-14 (docs/naming.md): Its two stores are read by nothing; named from what its one caller passes. */
/* func_80014048 */
void bg_party_spot_set(s32 x, s32 y) {
    D_800B88A0 = x;
    D_800B88A4 = y;
}
/* far_syms: g_PacketWritePointer g_PacketOtBase g_BgSlotBlocks */
extern s32 g_BgFirstHidden;
extern s16 g_BgCoverGrid[];
extern u16 tile_lookup_table[];
extern u32 g_BgTPages[];
extern u32 g_BgCluts[];
s32 bg_block_load(u32 tx, u32 ty);

/* Queue the background tile layer for the view whose top-left map pixel is
   (x, y): 16x16 cells from -(x & 15), -(y & 15) while sx < 0x141 and
   sy < 0xE0 (21 x 14 or 15 cells).

   The map is 128x128 tiles of one u16 word each, held in 8x8-tile pages.
   tile_lookup_table keeps sixteen pages resident in a 4x4 grid of slots
   addressed by the page's low two coordinate bits; the s16 at
   g_BgSlotBlocks[slot] names the page each slot holds, and a mismatch has
   bg_block_load() bring the wanted page in.  Cells off the map draw word 0.

   Tile word:
     0x000F / 0x00F0   texel cell u / v (x16) on a 256x256 page
     0x03FC            base-layer OT depth, +0x300
     0x0300            texture page, g_BgTPages[tp & 7]
     0x1000            CLUT pair, g_BgCluts[tp >> 3] (the overlay copy sets
                       tp's 8, taking the pair's second entry)
     0x2000            also draw an overlay copy at this cell's depth from
                       g_BgCoverGrid (one s16 per cell, row-major; >= 0x1000
                       hides it)
     0x4000 / 0x8000   mirror in x / y

   An unmirrored tile is a five-word PacketSprite16: a draw-mode word
   (0xE1000200 | page) and a 16x16 sprite (0x7C).  A mirrored one is a
   POLY_FT4 (0x2C) over the same square whose texel edges run from the far
   side back (cell + 15 down to cell - 1, clamped at 0; the unmirrored edge
   is cell .. cell + 16, clamped at 0xFF).  Both take their colour word from
   the scratchpad's +0x44 (bg_col_set sets it).  g_BgFirstHidden != 0 hides
   the base layer and leaves the overlay.

   The quad's corner temporaries are ints local to each quad block: eight
   separate three-reference registers, which is what puts the base copy's in
   $t6/$s0/$t4/$t7 and the overlay copy's in $t4/$t7/$t2/$t6 (one set shared
   by both blocks, or s16s, colours and schedules differently).  The OT
   pointer is read through a one-member struct view of its scratchpad cell:
   as a plain scalar at a fixed address GCC lets the load float above the
   packet's struct stores, and the target keeps it after the last one. */
/* func_80014058 */
void bg_first_draw(s32 x, s32 y)
{
  u32 tx0;
  u32 tx;
  u32 ty;
  s32 sx;
  s32 sy;
  u32 slot;
  u32 page;
  s32 tile;
  s32 z;
  s32 u;
  s32 v;
  s32 flip;
  s32 u0;
  s32 u1;
  s32 top_v;
  s32 bottom_v;
  s32 tp;
  s16 *depth;
  POLY_FT4 *q;
  PacketSprite16 *s;
  u32 *ot;
  depth = g_BgCoverGrid;
  ty = y >> 4;
  tx0 = x >> 4;
  z = y & 0xF;
  for (sy = -z; sy < 0xE0; sy += 0x10, ty++)
  {
    tx = tx0;
    for (sx = -(x & 0xF); sx < 0x141; sx += 0x10, tx++, depth++)
    {
      if ((tx >= 0x80) || (ty >= 0x80))
      {
        tile = 0;
      }
      else
      {
        page = (tx >> 3) + ((ty >> 3) << 4);
        slot = ((tx >> 3) & 3) | ((ty >> 1) & 0xC);
        if (page != ((s16 *) g_BgSlotBlocks)[slot])
        {
          bg_block_load(tx, ty);
        }
        tile = tile_lookup_table[(tx & 7) + ((ty & 7) << 3) + (slot << 6)];
      }
      tp = (tile & 0x1300) >> 8;
      z = ((tile >> 2) & 0xFF) + 0x300;
      u = (tile & 0xF) << 4;
      v = tile & 0xF0;
      flip = tile & 0xC000;
      if (g_BgFirstHidden == 0)
      {
        if (flip != 0)
        {
          s32 x0;
          s32 x1;
          s32 y0;
          s32 y1;
          x0 = sx;
          x1 = sx + 0x10;
          y0 = sy;
          y1 = sy + 0x10;
          if (tile & 0x4000)
          {
            u0 = u + 0xF;
            u1 = u - 1;
            if (u1 < 0)
            {
              u1 = 0;
            }
          }
          else
          {
            u0 = u;
            u1 = u0 + 0x10;
            if (u1 >= 0x100)
            {
              u1 = 0xFF;
            }
          }
          if (flip & 0x8000)
          {
            top_v = v + 0xF;
            bottom_v = v - 1;
            if (bottom_v < 0)
            {
              bottom_v = 0;
            }
          }
          else
          {
            top_v = v;
            bottom_v = top_v + 0x10;
            if (bottom_v >= 0x100)
            {
              bottom_v = 0xFF;
            }
          }
          q = (POLY_FT4 *) g_PacketWritePointer;
          AS(u32, q->r0) = (*((u32 *) 0x1F800044)) | 0x2C000000;
          q->x0 = x0;
          q->y0 = y0;
          q->x1 = x1;
          q->y1 = y0;
          q->x2 = x0;
          q->y2 = y1;
          q->x3 = x1;
          q->y3 = y1;
          q->u0 = u0;
          q->v0 = top_v;
          q->u1 = u1;
          q->v1 = top_v;
          q->u2 = u0;
          q->v2 = bottom_v;
          q->u3 = u1;
          q->v3 = bottom_v;
          q->clut = g_BgCluts[tp >> 3];
          q->tpage = g_BgTPages[tp & 7];
          z = z << 2;
          ot = (u32 *) (z + ((s32) ((OtCell *) &g_PacketOtBase)->ot));
          q->tag = (*ot) | 0x09000000;
          *ot = ((u32) q) + 0x80000000;
          q++;
          g_PacketWritePointer = (u8 *) q;
        }
        else
        {
          s = (PacketSprite16 *) g_PacketWritePointer;
          s->mode = g_BgTPages[tp & 7] | 0xE1000200;
          AS(u32, s->r0) = (*((u32 *) 0x1F800044)) | 0x7C000000;
          s->x0 = sx;
          s->y0 = sy;
          s->u0 = u;
          s->v0 = v;
          s->clut = g_BgCluts[tp >> 3];
          z = z << 2;
          ot = (u32 *) (z + ((s32) ((OtCell *) &g_PacketOtBase)->ot));
          s->tag = (*ot) | 0x04000000;
          *ot = ((u32) s) + 0x80000000;
          s++;
          g_PacketWritePointer = (u8 *) s;
        }
      }
      if (tile & 0x2000)
      {
        z = *depth;
        if (z < 0x1000)
        {
          tp |= 8;
          if (flip != 0)
          {
            s32 x0;
            s32 x1;
            s32 y0;
            s32 y1;
            x0 = sx;
            x1 = sx + 0x10;
            y0 = sy;
            y1 = sy + 0x10;
            if (flip & 0x4000)
            {
              u0 = u + 0xF;
              u1 = u - 1;
              if (u1 < 0)
              {
                u1 = 0;
              }
            }
            else
            {
              u0 = u;
              u1 = u0 + 0x10;
              if (u1 >= 0x100)
              {
                u1 = 0xFF;
              }
            }
            if (flip & 0x8000)
            {
              top_v = v + 0xF;
              bottom_v = v - 1;
              if (bottom_v < 0)
              {
                bottom_v = 0;
              }
            }
            else
            {
              top_v = v;
              bottom_v = top_v + 0x10;
              if (bottom_v >= 0x100)
              {
                bottom_v = 0xFF;
              }
            }
            q = (POLY_FT4 *) g_PacketWritePointer;
            AS(u32, q->r0) = (*((u32 *) 0x1F800044)) | 0x2C000000;
            q->x0 = x0;
            q->y0 = y0;
            q->x1 = x1;
            q->y1 = y0;
            q->x2 = x0;
            q->y2 = y1;
            q->x3 = x1;
            q->y3 = y1;
            q->u0 = u0;
            q->v0 = top_v;
            q->u1 = u1;
            q->v1 = top_v;
            q->u2 = u0;
            q->v2 = bottom_v;
            q->u3 = u1;
            q->v3 = bottom_v;
            q->clut = g_BgCluts[tp >> 3];
            q->tpage = g_BgTPages[tp & 7];
            z = z << 2;
            ot = (u32 *) (z + ((s32) ((OtCell *) &g_PacketOtBase)->ot));
            q->tag = (*ot) | 0x09000000;
            *ot = ((u32) q) + 0x80000000;
            q++;
            g_PacketWritePointer = (u8 *) q;
          }
          else
          {
            s = (PacketSprite16 *) g_PacketWritePointer;
            s->mode = g_BgTPages[tp & 7] | 0xE1000200;
            AS(u32, s->r0) = (*((u32 *) 0x1F800044)) | 0x7C000000;
            s->x0 = sx;
            s->y0 = sy;
            s->u0 = u;
            s->v0 = v;
            s->clut = g_BgCluts[tp >> 3];
            z = z << 2;
            ot = (u32 *) (z + ((s32) ((OtCell *) &g_PacketOtBase)->ot));
            s->tag = (*ot) | 0x04000000;
            *ot = ((u32) s) + 0x80000000;
            s++;
            g_PacketWritePointer = (u8 *) s;
          }
        }
      }
    }
  }
}

extern s32 g_BgScrollXLowBits;
extern s32 g_BgScrollYLowBits;

/* func_800145BC */
void bg_scroll_set(s32 x, s32 y) {
    g_BgScrollX = x;
    g_BgScrollY = y;
    g_BgScrollXLowBits = x & 0xF;
    g_BgScrollYLowBits = y & 0xF;
}

/* func_800145DC */
void bg_second_scroll_set(s32 x, s32 y) {
    g_BgSecondScrollX = x;
    g_BgSecondScrollY = y;
}
s32 bg_second_draw(s32, s32);                          /* extern */
s32 ankprint_draw(s32);                                 /* extern */

/* func_800145EC */
void bg_draw(void) {
    bg_first_draw(g_BgScrollX, g_BgScrollY);
    bg_second_draw(g_BgSecondScrollX, g_BgSecondScrollY);
    ankprint_draw(0x10);
}
/* far_syms: g_PacketWritePointer (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
/* far_syms: g_PacketOtBase (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */

/* func_8001462C */
void packet_black_line_add(s32 end_x, s32 y)
{
  s16 *packet;
  s32 ot_base;
  packet = (s16 *) *(s32 *) &g_PacketWritePointer;
  SetLineF3(packet);
  *((u8 *) packet + 0x4) = 0;
  *((u8 *) packet + 0x5) = 0;
  *((u8 *) packet + 0x6) = 0;
  ot_base = *(s32 *) &g_PacketOtBase;
  packet[4] = 0;
  packet[5] = y;
  packet[6] = end_x;
  packet[7] = y;
  packet[8] = end_x;
  packet[9] = y;
  AddPrim(ot_base + 4, packet);
  *(s32 *) &g_PacketWritePointer = (s32) (packet + 12);
}
/* One unmirrored tile as bg_first_draw's PacketSprite16: the draw-mode word for
   page tp & 7 and a 16x16 sprite at (x, y), texel (u, v), CLUT pair
   tp >> 3, linked at OT depth z.  Nothing in the image calls it. */
/* func_800146B4 */
void bg_tile_draw(s32 x, s32 y, s32 tp, s32 u, s32 v, s32 z)
{
  PacketSprite16 *s;
  u32 *ot;
  s = (PacketSprite16 *) g_PacketWritePointer;
  s->mode = g_BgTPages[tp & 7] | 0xE1000200;
  AS(u32, s->r0) = (*((u32 *) 0x1F800044)) | 0x7C000000;
  s->x0 = x;
  s->y0 = y;
  s->u0 = u;
  s->v0 = v;
  s->clut = g_BgCluts[tp >> 3];
  ot = (u32 *) ((z << 2) + (s32) g_PacketOtBase);
  s->tag = (*ot) | 0x04000000;
  *ot = ((u32) s) + 0x80000000;
  s++;
  g_PacketWritePointer = (u8 *) s;
}
/* One MIRRORED tile as bg_first_draw draws one: a POLY_FT4 (0x2C) over the
   16x16 square at (x, y), CLUT pair tp >> 3, texture page tp & 7, linked
   at OT depth z, colour word from the scratchpad's +0x44.  flip & 0x4000
   mirrors in x, flip & 0x8000 in y: a mirrored edge runs from texel
   cell + 15 back to cell - 1 (not below 0), an unmirrored one from cell to
   cell + 16 (0xFF when that reaches 0x100) -- the rule bg_first_draw applies
   inline.  The companion of bg_tile_draw above (the unmirrored tile as a
   sprite); like it, nothing in the image or in MENU.BIN calls it. */
/* func_80014770 */
void bg_tile_mirror_draw(s32 x, s32 y, s32 tp, s32 u, s32 v, s32 z, s32 flip)
{
  POLY_FT4 *q;
  u32 *ot;
  s32 x1;
  s32 y1;
  s32 u0;
  s32 u1;
  s32 top_v;
  s32 bottom_v;
  x1 = x + 0x10;
  y1 = y + 0x10;
  if (flip & 0x4000)
  {
    u0 = u + 0xF;
    u1 = u - 1;
    if (u1 < 0)
    {
      u1 = 0;
    }
  }
  else
  {
    u0 = u;
    u1 = u0 + 0x10;
    if (u1 >= 0x100)
    {
      u1 = 0xFF;
    }
  }
  if (flip & 0x8000)
  {
    top_v = v + 0xF;
    bottom_v = v - 1;
    if (bottom_v < 0)
    {
      bottom_v = 0;
    }
  }
  else
  {
    top_v = v;
    bottom_v = top_v + 0x10;
    if (bottom_v >= 0x100)
    {
      bottom_v = 0xFF;
    }
  }
  q = (POLY_FT4 *) g_PacketWritePointer;
  AS(u32, q->r0) = (*((u32 *) 0x1F800044)) | 0x2C000000;
  q->x0 = x;
  q->y0 = y;
  q->x1 = x1;
  q->y1 = y;
  q->x2 = x;
  q->y2 = y1;
  q->x3 = x1;
  q->y3 = y1;
  q->u0 = u0;
  q->v0 = top_v;
  q->u1 = u1;
  q->v1 = top_v;
  q->u2 = u0;
  q->v2 = bottom_v;
  q->u3 = u1;
  q->v3 = bottom_v;
  q->clut = g_BgCluts[tp >> 3];
  q->tpage = g_BgTPages[tp & 7];
  ot = (u32 *) ((z << 2) + (s32) g_PacketOtBase);
  q->tag = (*ot) | 0x09000000;
  *ot = ((u32) q) + 0x80000000;
  q++;
  g_PacketWritePointer = (u8 *) q;
}

extern s32 g_BgSecondCol;

/* func_800148C8 */
void bg_second_col_set(s32 col) {
    g_BgSecondCol = col & 0xFFFFFF;
}
extern s32 g_BgSecondPacketTPage;
extern s8 g_BgMode;
extern s32 g_BgSecondBuffer;
void bg_second_screen_draw();
s32 layer_mix_mode_get(void);
s32 packet_tpage_add(s32, s32);

/* Queue the second tile layer (8-pixel cells) for the view whose top-left
   layer pixel is (x, y): rows from -(y & 7) while sy < 0xE0, columns from
   -(x & 15) while sx < 0x140, two cells per 16-pixel step.  It scrolls on its
   own: bg_draw passes g_BgSecondScrollX / g_BgSecondScrollY (bg_second_scroll_set sets
   them), not the map's scroll.

   The layer is g_BgSecondBuffer (the map file's type-7 block, 0x4000 bytes): 128
   rows of 64 s16 cells, wrapping both ways.  g_BgMode & 0x80 hands the
   whole job to bg_second_screen_draw.  Otherwise the layer mode layer_mix_mode_get()
   picks the OT depth base and the colour word (scratchpad +0x48, from
   g_BgSecondCol):
     0  depth 0x3C0
     1  depth 0x100, semi-transparent (colour word | 0x02000000)
     3  depth 0x100
     5  depth 0x100, semi-transparent, and g_BgSecondPacketTPage (the texture-page
        word) = 0x3F: page 0x1F with blend mode 1 (B + F) instead of 0
     4  draws nothing
   Mode 2, or anything above 5, leaves the depth base unset and the colour
   word as it was — the original's own fall-through (jtbl_80011020).

   Cell word (-1 = empty):
     0x001F         texel u, x8
     0x01E0         texel v, x8 (>> 2)
     0x01F8 >> 3    depth offset 0..63, added to the base
     0x0200         first cell of a wide tile: ONE sprite 8 high and 16 wide,
                    or 32 wide when the next cell is non-zero; it takes 2 or
                    4 cells, and the mirror bits are not read
     0x0400/0x0800  mirror in x / y (8x8 cells)
     0xF000         CLUT word 0x7F00 + n

   A plain cell is a PacketSprite (draw-mode word 0xE1000200 | g_BgSecondPacketTPage and
   a 0x64 sprite); a mirrored one is a POLY_FT4 (0x2C) through AddPrim
   , its texel edges running from the far side back (cell + 7
   down to cell - 1, clamped at 0; unmirrored cell .. cell + 8, clamped at
   0xFF).

   CODEGEN FORCING DEVICE (spelling), not recovered source: the
   sprite sizes go through block-local variables — `h = 8` in the wide block,
   `w = 8` in the 8x8 one — where the original may have had literals.
   Behaviour-identical.  A constant held in a variable is what loop.c hoists
   to the while and k loops' preheaders ($t8, caller-saved around AddPrim,
   and $s7); those two call-crossing registers are what leave x and col0 on
   the stack.  A literal 8 in either place costs 45 rows.

   The other shapes the match needs: each packet block declares its own
   pointer (three different registers); the corner temporaries are ints (as
   s16 the scheduler reorders their definitions); the colour word is written
   through a cast of &r0, which is not a struct-member store, so it keeps
   its place ahead of the global loads that follow; the quad's XYs are
   stored before its UVs. */
/* func_800148E0 */
s32 bg_second_draw(s32 x, s32 y)
{
    s32 row;
    s32 col0;
    s32 col;
    s32 sx;
    s32 sy;
    s32 z;
    s16 *line;
    s32 t;
    s32 u;
    s32 v;
    s32 clut;
    s32 pri;
    s32 n;
    s32 k;
    s32 flip;
    s32 zz;
    s32 u0;
    s32 u1;
    s32 top_v;
    s32 bottom_v;
    s32 x0;
    s32 x1;
    s32 y0;
    s32 y1;

    row = y >> 3;
    g_BgSecondPacketTPage = 0x1F;
    col0 = (x >> 4) * 2;
    if (g_BgMode & 0x80) {
        g_BgSecondPacketCol = g_BgSecondCol;
        bg_second_screen_draw();
        return;
    }
    switch (layer_mix_mode_get()) {
    case 0:
        z = 0x3C0;
        g_BgSecondPacketCol = g_BgSecondCol;
        break;
    case 1:
        z = 0x100;
        g_BgSecondPacketCol = g_BgSecondCol | 0x2000000;
        break;
    case 3:
        z = 0x100;
        g_BgSecondPacketCol = g_BgSecondCol;
        break;
    case 5:
        z = 0x100;
        g_BgSecondPacketTPage = 0x3F;
        g_BgSecondPacketCol = g_BgSecondCol | 0x2000000;
        break;
    case 4:
        return;
    }
    packet_tpage_add(0x1F, 0x100);
    for (sy = -(y & 7); sy < 0xE0; sy += 8, row++) {
        col = col0;
        sx = -(x & 0xF);
        line = (s16 *)(g_BgSecondBuffer + ((row & 0x7F) << 7));
        while (sx < 0x140) {
            t = line[col & 0x3F];
            if (t != -1 && (t & 0x200)) {
                PacketSprite *p;
                u32 *ot;
                s32 h;

                u = (t & 0x1F) << 3;
                v = (t & 0x1E0) >> 2;
                clut = (t & 0xF000) >> 12;
                pri = (t & 0x1FF) >> 3;
                if (line[(col + 1) & 0x3F] != 0) {
                    n = 4;
                } else {
                    n = 2;
                }
                h = 8;
                p = (PacketSprite *)g_PacketWritePointer;
                p->mode = g_BgSecondPacketTPage | 0xE1000200;
                *(u32 *)&p->r0 = g_BgSecondPacketCol | 0x64000000;
                p->u0 = u;
                p->v0 = v;
                p->clut = clut + 0x7F00;
                p->x0 = sx;
                p->y0 = sy;
                p->w = n * 8;
                p->h = h;
                sx += n * 8;
                col += n;
                ot = (u32 *)(((z + pri) << 2) + (s32)g_PacketOtBase);
                p->tag = *ot | 0x05000000;
                *ot = (u32)p + 0x80000000;
                p++;
                g_PacketWritePointer = (u8 *)p;
            } else {
                for (k = 0; k < 2; k++) {
                    t = line[col & 0x3F];
                    if (t != -1) {
                        u = (t & 0x1F) << 3;
                        v = (t & 0x1E0) >> 2;
                        clut = (t & 0xF000) >> 12;
                        pri = (t & 0x1FF) >> 3;
                        flip = t & 0xC00;
                        if (flip != 0) {
                            POLY_FT4 *p;

                            zz = z + pri;
                            x0 = sx;
                            x1 = sx + 8;
                            y0 = sy;
                            y1 = sy + 8;
                            if (t & 0x400) {
                                u0 = u + 7;
                                u1 = u - 1;
                                if (u1 < 0) {
                                    u1 = 0;
                                }
                            } else {
                                u0 = u;
                                u1 = u0 + 8;
                                if (u1 >= 0x100) {
                                    u1 = 0xFF;
                                }
                            }
                            if (flip & 0x800) {
                                top_v = v + 7;
                                bottom_v = v - 1;
                                if (bottom_v < 0) {
                                    bottom_v = 0;
                                }
                            } else {
                                top_v = v;
                                bottom_v = top_v + 8;
                                if (bottom_v >= 0x100) {
                                    bottom_v = 0xFF;
                                }
                            }
                            p = (POLY_FT4 *)g_PacketWritePointer;
                            p->tag = 0x09000000;
                            *(u32 *)&p->r0 = g_BgSecondPacketCol | 0x2C000000;
                            p->x0 = x0;
                            p->y0 = y0;
                            p->x1 = x1;
                            p->y1 = y0;
                            p->x2 = x0;
                            p->y2 = y1;
                            p->x3 = x1;
                            p->y3 = y1;
                            p->u0 = u0;
                            p->v0 = top_v;
                            p->u1 = u1;
                            p->v1 = top_v;
                            p->u2 = u0;
                            p->v2 = bottom_v;
                            p->u3 = u1;
                            p->v3 = bottom_v;
                            p->clut = clut + 0x7F00;
                            p->tpage = g_BgSecondPacketTPage;
                            AddPrim((s32)g_PacketOtBase + (zz << 2), p);
                            p++;
                            g_PacketWritePointer = (u8 *)p;
                        } else {
                            PacketSprite *p;
                            u32 *ot;
                            s32 w;

                            w = 8;
                            p = (PacketSprite *)g_PacketWritePointer;
                            p->mode = g_BgSecondPacketTPage | 0xE1000200;
                            *(u32 *)&p->r0 = g_BgSecondPacketCol | 0x64000000;
                            p->u0 = u;
                            p->v0 = v;
                            p->clut = clut + 0x7F00;
                            p->x0 = sx;
                            p->y0 = sy;
                            p->w = w;
                            p->h = w;
                            ot = (u32 *)(((z + pri) << 2) + (s32)g_PacketOtBase);
                            p->tag = *ot | 0x05000000;
                            *ot = (u32)p + 0x80000000;
                            p++;
                            g_PacketWritePointer = (u8 *)p;
                        }
                    }
                    sx += 8;
                    col++;
                }
            }
        }
    }
}
/* Queue the second tile layer as a fixed screen of 16 x 16 tiles: 14 rows of
   21 columns at (col * 16, row * 16), no scroll.  bg_second_draw hands the
   layer here when g_BgMode & 0x80 (StartResourceLoad sets it to -0x80 for the
   title screen's planets), after putting the layer colour g_BgSecondCol in the
   scratchpad colour cell (+0x48); it is the only caller, passes nothing and
   reads nothing back.

   The cells are the same block the scrolled path reads, g_BgSecondBuffer (the map
   file's type-7 block), but taken as rows of 32 s16: cell = row * 32 + col.
   A cell of 0 draws nothing (the scrolled path's empty cell is -1).  Only
   its low byte is read, as a tile number on a 16 x 16-tile page:
     0x000F   texel u = n * 16
     0x00F0   texel v, as it stands
   Each cell that is not 0 takes one 0x18-byte PacketSprite from the primitive
   buffer g_PacketWritePointer: draw-mode word 0xE100029D (texture page 0x9D -- VRAM
   (832, 256), 8-bit CLUT, blend mode 0 -- with the dither bit 0x200), a
   0x64 sprite in the colour of scratchpad +0x48, 16 x 16, CLUT 0x7D40 (VRAM
   (0, 501), the second half of the map's 1024-byte palette).  It is linked
   by hand at ordering-table cell 0x2FE of g_PacketOtBase, the way the scrolled
   path links its own: tag = old head | 0x05000000, head = packet + 0x80000000
   (the low 24 bits of a KSEG0 address).  No callee, no RNG draw, no frame
   wait.

   MARKED SPELLING: three constants are held in locals, and the bytes need
   each one.  `z` (the ordering-table depth 0x2FE, set ahead of the loops as
   bg_second_draw sets its own z): the target adds a register holding 0xBF8 to
   the table base (`addu $a1, $t4, $a1`) where a literal folds into the load's
   offset -- 48/59.  `w` and `mode` (the sprite size and the draw-mode word,
   set at the top of the block): as literals at their stores 58/59 each,
   57/59 both.  The evidence is the order of the constants ahead of the
   loops, `li 0x10`, `li 0xE100029D`, `lui 0x6400`, `li 0xBF8`: the loop pass
   moves invariants out in the order it meets them, so 16 and then the mode
   word existed before the colour word's 0x64000000, yet the colour word is
   the first thing stored and the size the last but one.  The Japanese build
   (0x80015F68, GCC 2.7.2) has the same order and scores the same way (56/59
   with the two literals; 58/59 with the locals, the last row being
   g_BgSecondBuffer's own gp offset).  What put them in variables in the original
   -- plain locals as here, or a macro's parameters -- the bytes do not say.

   The colour cell is read through a symbol of its own, g_BgSecondPacketCol (added
   for this function).  Written as a constant address, `*(u32 *)0x1F800048`,
   the address is a loop invariant in this call-free loop: it is moved into
   a register ahead of the loops and takes the colour constant's place
   there -- 51/59.  The target reads it with `lui` / `lw` in place, exactly
   as it reads the two named cells g_PacketOtBase and g_PacketWritePointer, so the
   original addressed all three through symbols.  (Reaching the cell as
   `((u32 *)&g_PacketOtBase)[2]`, through the neighbour's symbol, gives the
   same bytes.)

   What else the bytes pin (each changed alone in this text): `w` is set
   before `mode` (58/59 swapped) and both inside the inner loop (set ahead of
   the loops with z, 14/59); the CLUT is a literal at its store (through a
   local set with the others it is moved out of the loop too, 41/59); the
   depth is added product-first, `(z << 2) + base` (58/59 the other way
   round); the cell is signed (49/59 read as u16) and held in an int (37/59
   in an s16); the colour word is the first store (48/59 with the mode word
   ahead of it) and the CLUT comes after y and after the size.  Not pinned:
   u and v match computed at their stores as well (they are locals here as
   bg_second_draw has them), the return type may be s32 or void, and fourteen
   of the store orders tried match, here and on the Japanese build alike
   (mode, x, y, u / v and w / h may move among themselves); this one is the
   packet's own layout but for the colour word leading. */
/* func_80014D74 */
void bg_second_screen_draw(void)
{
    s32 row;
    s32 col;
    s32 t;
    s32 u;
    s32 v;
    s32 w;
    s32 z;
    u32 mode;
    PacketSprite *p;
    u32 *ot;

    z = 0x2FE;
    for (row = 0; row < 14; row++) {
        for (col = 0; col < 21; col++) {
            t = ((s16 *)g_BgSecondBuffer)[row * 32 + col];
            if (t != 0) {
                u = (t & 0xF) << 4;
                v = t & 0xF0;
                w = 16;
                mode = 0xE100029D;
                p = (PacketSprite *)g_PacketWritePointer;
                *(u32 *)&p->r0 = g_BgSecondPacketCol | 0x64000000;
                p->mode = mode;
                p->x0 = col * 16;
                p->y0 = row * 16;
                p->u0 = u;
                p->v0 = v;
                p->w = w;
                p->h = w;
                p->clut = 0x7D40;
                ot = (u32 *)((z << 2) + (s32)g_PacketOtBase);
                p->tag = *ot | 0x05000000;
                *ot = (u32)p + 0x80000000;
                p++;
                g_PacketWritePointer = (u8 *)p;
            }
        }
    }
}

/* far_syms: g_BgSlotBlocks (target reaches it %hi/%lo) */
s32 bg_copy(s32, s32, s32, s32, s32, s32);
s32 bg_attribute_copy(s32, s32, s32, s32, s32, s32);
void bg_both_copy(s32 dx, s32 dy, s32 sx, s32 sy, s32 w, s32 h);  /* TU defines this below */
void bg_effect_flags_set(s8 effect_flags);  /* TU defines this below */
s32 packet_tpage_add(s32, s32);
void bg_data_init(void);  /* TU defines this below */
s32 bg_attribute_data_reset(s32);
void bg_attribute_data_init(void);  /* TU defines this below */
extern s32 g_BgBlockFillLimit;
extern s32 g_BgBlockPlainLimit;
extern s32 g_BgNextBlockEntry;

/* func_80014E60 */
s32 bg_data_reset(s32 next_entry_then_no_block)
{
  s32 first_limit_then_slot;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `last_slot_offset` only holds the constant 0x1E; behaviour is the same with the constant
     written where it is read, but then 3 of this function's 23 instruction lines differ. */
  char last_slot_offset;
  s16 *slot_block;
  next_entry_then_no_block = ((BgBlockTable *)g_BgBlockEntries)->plain_limit;
  first_limit_then_slot = ((BgBlockTable *)g_BgBlockEntries)->fill_limit;
  g_BgBlockFillLimit = first_limit_then_slot;
  g_BgBlockPlainLimit = next_entry_then_no_block;
  g_BgNextBlockEntry = next_entry_then_no_block;
  if (next_entry_then_no_block & 1)
  {
    g_BgNextBlockEntry = next_entry_then_no_block + 1;
  }
  next_entry_then_no_block = -1;
  first_limit_then_slot = 0xF;
  last_slot_offset = 0x1E;
  slot_block = (s16 *) (g_BgSlotBlocks + last_slot_offset);
  do
  {
    *slot_block = next_entry_then_no_block;
    first_limit_then_slot--;
    slot_block--;
  }
  while (first_limit_then_slot >= 0);
  return (s32) slot_block;
}
/* func_80014EBC */
void bg_slot_reset(void)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `no_block` only holds the constant -1; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 11 instruction lines
     differs. */
  int no_block;
  s16 *slot_block;
  s32 slot;
  int last_slot;
  no_block = -1;
  last_slot = 0x1E;
  last_slot = 0xF;
  slot = last_slot;
  slot_block = g_BgSlotBlocks;
  slot_block = slot_block + last_slot;
  do
  {
    *slot_block = no_block;
    slot -= 1;
    slot_block -= 1;
  }
  while (slot >= 0);
}

s32 bg_copy(s32, s32, s32, s32, s32, s32);
s32 bg_attribute_copy(s32, s32, s32, s32, s32, s32);
void bg_both_copy(s32 dx, s32 dy, s32 sx, s32 sy, s32 w, s32 h);  /* TU defines this below */
void bg_effect_flags_set(s8 effect_flags);  /* TU defines this below */
s32 packet_tpage_add(s32, s32);

/* func_80014EE8 */
void bg_data_init(void)
{
  s16 *word;
  s32 entry_number;
  int step;
  entry_number = 0xFF;
  word = g_BgDataBuffer + 0x1FE;
  g_BgBlockEntries = g_BgDataBuffer;
  g_BgBlockBase = g_BgDataBuffer;
  do
  {
    *word = 0;
    step = 2;
    step = 1;
    entry_number -= step;
    word -= step;
  }
  while (entry_number >= 0);
  word = (s16 *)(s32 *) (((u8 *) g_BgBlockEntries) + 0x200);
  *word = 0x10;
  ((BgBlockTable *)g_BgBlockEntries)->plain_limit = 0x200;
  bg_data_reset(entry_number);
}
void hex_dump();
s32 lz_unpack();

/* Bring the block of cell (tx, ty) of the PRIMARY tile map (128 x 128
   cells in 8 x 8-cell blocks, a 16 x 16 grid of blocks) into the block
   cache. The cache is sixteen 64-word slots at tile_lookup_table, a 4 x 4
   grid indexed by the block's low two bits each way:
   slot = ((tx >> 3) & 3) | ((ty >> 1) & 0xC). The s16 at g_BgSlotBlocks[slot]
   is set to the block's number, (tx >> 3) + (ty >> 3) * 16, BEFORE the slot
   is filled; bg_first_draw and bg_copy compare it and call this on a
   mismatch, and bg_tile_set sets it to -1 after a write.

   The block's entry in the layout table, the u16 at g_BgBlockEntries +
   block * 2, is an offset `off`:
     0                    64 zero words
     < g_BgBlockFillLimit         64 copies of the one u16 at (off & 0xFFFE) + table
                          (read once, before the loop)
     >= g_BgBlockPlainLimit        64 raw u16s from g_BgBlockBase + off; an ODD offset
                          first calls hex_dump(table, 0x210)
     otherwise            lz_unpack(g_BgBlockBase + off, slot)
   g_BgBlockFillLimit / g_BgBlockPlainLimit are the table's own words at +0x200 / +0x202
   (bg_data_reset). Both callers ignore the result. No RNG, no frame wait.

   Shape: the function returns s32 -- the target returns 0 for a cell
   outside the map and nothing otherwise, as its twin
   bg_attribute_block_load does (as a `void` function this text is 77/84);
   the offset is an int (as a u16 the compares are unsigned and the
   register is re-masked, 77/84); the fill word has a local (read inside
   the loop it is 74/84); the slot address is taken through a byte pointer
   to the cache (indexed as u16 words it is 83/84). */
/* func_80014F3C */
s32 bg_block_load(u32 tx, u32 ty)
{
    u32 bx;
    u32 q;
    u16 *dst;
    u16 *src;
    s32 off;
    s32 i;
    u16 v;
    u8 *tbl;

    if (tx >= 0x80 || ty >= 0x80) {
        return 0;
    }
    bx = tx >> 3;
    q = (bx & 3) | ((ty >> 1) & 0xC);
    tbl = (u8 *)tile_lookup_table;
    dst = (u16 *)(tbl + q * 128);
    bx += (ty >> 3) * 16;
    off = *(u16 *)((u8 *)g_BgBlockEntries + bx * 2);
    ((s16 *)g_BgSlotBlocks)[q] = bx;
    if (off == 0) {
        for (i = 0; i < 0x40; i++) {
            *dst++ = 0;
        }
    } else if (off < g_BgBlockFillLimit) {
        v = *(u16 *)((off & 0xFFFE) + (s32)g_BgBlockEntries);
        for (i = 0; i < 0x40; i++) {
            *dst++ = v;
        }
    } else if (off >= g_BgBlockPlainLimit) {
        if (off & 1) {
            hex_dump(g_BgBlockEntries, 0x210);
        }
        src = (u16 *)((u8 *)g_BgBlockBase + off);
        for (i = 0; i < 0x40; i++) {
            *dst++ = *src++;
        }
    } else {
        lz_unpack((u8 *)g_BgBlockBase + off, dst);
    }
}

s32 lz_unpack();

/* far_syms: g_BgSlotBlocks */
/* Set cell (x, y) of the PRIMARY tile map (128 x 128 cells, 8 x 8-cell
   blocks) to `val`. A block that is not already raw -- its table offset
   below g_BgBlockPlainLimit -- is first materialised as a new 64-word raw block at
   the allocation cursor g_BgNextBlockEntry: zeroed for offset 0, filled with the
   one word at (off & 0xFFFE) + table for a fill offset (< g_BgBlockFillLimit),
   LZSS-unpacked otherwise; the table entry is repointed to it and the
   cursor advances 0x80. The cell's slot in the 16-slot block cache
   (g_BgSlotBlocks, indexed like bg_block_load's) is then marked stale (-1)
   so the next read re-fetches the block. Returns 0 for a cell outside
   the map; otherwise the value is whatever the last store left ($v0 =
   -1) -- bg_copy ignores it. No bound on the cursor: the caller's arena
   is trusted. */
/* func_8001508C */
s32 bg_tile_set(s32 x, s32 y, s32 val)
{
    s32 bx;
    s32 by;
    s32 cx;
    s32 cy;
    s32 bi;
    s32 off;
    s32 i;
    u16 *blk;
    u16 *tbl;

    if (x < 0 || y < 0 || x >= 0x80 || y >= 0x80) {
        return 0;
    }
    bx = x >> 3;
    by = y >> 3;
    cx = x & 7;
    cy = y & 7;
    bi = bx + (by << 4);
    tbl = (u16 *)g_BgBlockEntries;
    off = tbl[bi];
    if (off >= g_BgBlockPlainLimit) {
        blk = (u16 *)((u8 *)g_BgBlockBase + off);
    } else {
        blk = (u16 *)((u8 *)g_BgBlockBase + g_BgNextBlockEntry);
        if (off == 0) {
            for (i = 0; i < 0x40; i++) {
                blk[i] = 0;
            }
        } else if (off < g_BgBlockFillLimit) {
            for (i = 0; i < 0x40; i++) {
                blk[i] = *(u16 *)((off & 0xFFFE) + (s32)tbl);
            }
        } else {
            lz_unpack((u8 *)g_BgBlockBase + off, blk);
        }
        ((u16 *)g_BgBlockEntries)[bi] = g_BgNextBlockEntry;
        g_BgNextBlockEntry += 0x80;
    }
    blk[cy * 8 + cx] = val;
    ((s16 *)g_BgSlotBlocks)[((x >> 3) & 3) | ((y >> 1) & 0xC)] = -1;
}
extern u8 D_80011038[];
s32 debug_printf();

/* bg_copy(dx, dy, sx, sy, w, h) -- the developers' own name: it prints
   "bg_copy(%d,%d,%d,%d,%d,%d)\n" (D_80011038) with its six arguments on
   entry. Copy a w x h rectangle of cells of the PRIMARY tile map (128 x
   128 cells of 16 px) from (sx, sy) to (dx, dy). The one caller is
   bg_both_copy, which passes its own six arguments halved; the
   attribute-plane half is bg_attribute_copy, this function's twin.

   Rows top to bottom (sy + j, dy + j), cells left to right (i), one cell
   at a time: read the source word, then write it with bg_tile_set(dx +
   i, dy + j, word).
     - A source cell outside 0..0x7F in either coordinate (unsigned
       compares, so a negative one too) is NOT skipped: it reads as word 0
       and 0 is written to its destination.
     - Inside the map it is read as bg_first_draw reads it, through the
       16-slot page cache: page = (x >> 3) + ((y >> 3) << 4), slot =
       ((x >> 3) & 3) | ((y >> 1) & 0xC); a slot that does not hold the
       page has bg_block_load(x, y) bring it in; the word is
       tile_lookup_table[(x & 7) + ((y & 7) << 3) + (slot << 6)].
     - A destination outside the map is bg_tile_set's business: it
       returns 0 and writes nothing.
   There is no overlap handling: the order is always increasing x, then
   increasing y, each source read just before its write, and every write
   marks the destination's cache slot stale so the next read from that
   slot re-fetches the block WITH the write in it. A destination that
   lies ahead of its source (right of it on the same rows, or below it)
   therefore re-reads cells this call has already overwritten -- a forward
   memcpy, not a memmove.
   w <= 0 or h <= 0 copies nothing. No return statement (the caller
   ignores the value), no frame wait, no random draw.

   Matching. All nine call-saved registers are taken (x, slot, i, sy, dy,
   j, the two hoisted row terms and w), so dx, sx and h stay in their
   argument homes and are reloaded through $t0 at each use, and the two
   table addresses loop.c hoists get no register either and are
   rematerialised through $t0 inside the loop. The row terms are loop.c's
   own hoist out of the cell loop, in the order the body first computes
   them: `key` (the page number with its row part, (sy >> 3) << 4) is
   written BEFORE `slot` (whose row part is (sy >> 1) & 0xC), which puts
   the first in $s7 and the second in $s6. With the compare written as
   `page + ... != cache[slot]` after the slot line the two are computed
   and allocated the other way round (90/95). The out-of-map test is
   spelled as the `||` with the zero arm first: that is the target's
   block order (the lookup falls into the call, the zero arm jumps). */
/* func_80015224 */
s32 bg_copy(s32 dx, s32 dy, s32 sx, s32 sy, s32 w, s32 h)
{
    s32 i;
    s32 j;
    u32 x;
    u32 page;
    u32 key;
    s32 slot;
    s32 val;

    debug_printf(D_80011038, dx, dy, sx, sy, w, h);
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            x = sx + i;
            if (x >= 0x80 || (u32)sy >= 0x80) {
                val = 0;
            } else {
                page = x >> 3;
                key = page + (((u32)sy >> 3) << 4);
                slot = (page & 3) | (((u32)sy >> 1) & 0xC);
                if (key != ((s16 *)g_BgSlotBlocks)[slot]) {
                    bg_block_load(x, sy);
                }
                val = tile_lookup_table[(x & 7) + ((sy & 7) << 3) + (slot << 6)];
            }
            bg_tile_set(dx + i, dy, val);
        }
        sy++;
        dy++;
    }
}

s32 bg_copy(s32, s32, s32, s32, s32, s32);            /* extern */
s32 bg_attribute_copy(s32, s32, s32, s32, s32, s32); /* extern */

/* func_800153A0 */
void bg_both_copy(s32 dx, s32 dy, s32 sx, s32 sy, s32 w, s32 h) {
    bg_copy(dx >> 1, dy >> 1, sx >> 1, sy >> 1, w >> 1, h >> 1);
    bg_attribute_copy(dx, dy, sx, sy, w, h);
}
/* far_syms: g_BgAttributeSlotBlocks (target reaches it %hi/%lo) */
extern u8 g_BgAttributeSlotBlocks[];
extern s32 g_BgAttributeBlockFillLimit;
extern s32 g_BgAttributeBlockPlainLimit;
extern s32 g_BgAttributeNextBlockEntry;

/* func_8001543C */
s32 bg_attribute_data_reset(s32 unused)
{
  s32 next_entry_then_no_block;
  s32 first_limit_then_slot;
  s16 *slot_block;

  next_entry_then_no_block = ((BgBlockTable *)g_BgAttributeBlockEntries)->plain_limit;
  first_limit_then_slot = ((BgBlockTable *)g_BgAttributeBlockEntries)->fill_limit;
  g_BgAttributeBlockFillLimit = first_limit_then_slot;
  g_BgAttributeBlockPlainLimit = next_entry_then_no_block;
  g_BgAttributeNextBlockEntry = next_entry_then_no_block;
  if (next_entry_then_no_block & 1)
    g_BgAttributeNextBlockEntry = next_entry_then_no_block + 1;

  next_entry_then_no_block = -1;
  first_limit_then_slot = 0xF;
  slot_block = (s16 *)g_BgAttributeSlotBlocks;
  slot_block += 0xF;
  do
  {
    *slot_block = next_entry_then_no_block;
    first_limit_then_slot -= 1;
    slot_block -= 1;
  }
  while (first_limit_then_slot >= 0);

  return (s32)slot_block;
}

s32 bg_attribute_data_reset(s32);

/* func_80015498 */
void bg_attribute_data_init(void)
{
  s16 *word;
  s32 entry_number;
  entry_number = 0xFF;
  word = g_BgAttributeDataBuffer + 0x1FE;
  g_BgAttributeBlockEntries = g_BgAttributeDataBuffer;
  g_BgAttributeBlockBase = g_BgAttributeDataBuffer;
  do
  {
    *word = 0;
    entry_number -= 1;
    word -= 1;
  }
  while (entry_number >= 0);
  word = (s16 *)(s32 *) (((u8 *) g_BgAttributeBlockEntries) + 0x200);
  *word = 0x10;
  ((BgBlockTable *)g_BgAttributeBlockEntries)->plain_limit = 0x200;
  bg_attribute_data_reset(entry_number);
}

/* Ruling of 2026-10-14 (docs/naming.md): Its store is read by nothing; named from what its one caller passes. */
/* func_800154EC */
void bg_effect_flags_set(s8 effect_flags) {
    D_800B8558 = effect_flags;
}
/* far_syms: g_BgAttributeSlotBlocks secondary_tile_table */
extern long HWD0, VWD0;		 
void hex_dump();
s32 lz_unpack();

/* func_800154F8 */
s32 bg_attribute_block_load(u32 x, u32 y)
{
    u32 bx;
    u32 q;
    u16 *dst;
    u16 *src;
    s32 off;
    s32 i;
    u16 v;
    u8 *tbl;

    if (x >= 0x100 || y >= 0x100) {
        return 0;
    }
    bx = x >> 4;
    q = (bx & 3) | ((y >> 2) & 0xC);
    tbl = secondary_tile_table;
    dst = (u16 *)(tbl + q * 512);
    bx += y & 0xF0;
    off = *(u16 *)((u8 *)g_BgAttributeBlockEntries + bx * 2);
    ((s16 *)g_BgAttributeSlotBlocks)[q] = bx;
    if (off == 0) {
        for (i = 0; i < 0x100; i++) {
            *dst++ = 0;
        }
    } else if (off < g_BgAttributeBlockFillLimit) {
        v = *(u16 *)((off & 0xFFFE) + (s32)g_BgAttributeBlockEntries);
        for (i = 0; i < 0x100; i++) {
            *dst++ = v;
        }
    } else if (off >= g_BgAttributeBlockPlainLimit) {
        if (off & 1) {
            hex_dump(g_BgAttributeBlockEntries, 0x210);
        }
        src = (u16 *)((u8 *)g_BgAttributeBlockBase + off);
        for (i = 0; i < 0x100; i++) {
            *dst++ = *src++;
        }
    } else {
        lz_unpack((u8 *)g_BgAttributeBlockBase + off, dst);
    }
}
/* far_syms: g_BgAttributeSlotBlocks */
/* Set cell (x, y) of the SECONDARY tile map (256 x 256 cells, 16 x 16-
   cell blocks) to `val` -- bg_tile_set's twin. A block that is not
   already raw -- its table offset below g_BgAttributeBlockPlainLimit -- is first
   materialised as a new 256-word raw block at the allocation cursor
   g_BgAttributeNextBlockEntry: zeroed for offset 0, filled with the one word at
   (off & 0xFFFE) + table for a fill offset (< g_BgAttributeBlockFillLimit), LZSS-unpacked
   otherwise; the table entry is repointed to it and the cursor advances
   0x200. The cell's quadrant slot in g_BgAttributeSlotBlocks (indexed as
   bg_attribute_block_load indexes it) is then marked stale (-1) so the
   next read re-fetches the block into secondary_tile_table. Returns 0
   for a cell outside the map; otherwise the value is whatever the last
   store left ($v0 = -1) -- the caller ignores it. */
/* func_80015644 */
s32 bg_attribute_set(s32 x, s32 y, s32 val)
{
    s32 bx;
    s32 by;
    s32 cx;
    s32 cy;
    s32 bi;
    s32 off;
    s32 i;
    u16 *blk;
    u16 *tbl;

    if (x < 0 || y < 0 || x >= 0x100 || y >= 0x100) {
        return 0;
    }
    bx = x >> 4;
    by = y >> 4;
    cx = x & 0xF;
    cy = y & 0xF;
    bi = bx + (by << 4);
    tbl = (u16 *)g_BgAttributeBlockEntries;
    off = tbl[bi];
    if (off >= g_BgAttributeBlockPlainLimit) {
        blk = (u16 *)((u8 *)g_BgAttributeBlockBase + off);
    } else {
        blk = (u16 *)((u8 *)g_BgAttributeBlockBase + g_BgAttributeNextBlockEntry);
        if (off == 0) {
            for (i = 0; i < 0x100; i++) {
                blk[i] = 0;
            }
        } else if (off < g_BgAttributeBlockFillLimit) {
            for (i = 0; i < 0x100; i++) {
                blk[i] = *(u16 *)((off & 0xFFFE) + (s32)tbl);
            }
        } else {
            lz_unpack((u8 *)g_BgAttributeBlockBase + off, blk);
        }
        ((u16 *)g_BgAttributeBlockEntries)[bi] = g_BgAttributeNextBlockEntry;
        g_BgAttributeNextBlockEntry += 0x200;
    }
    blk[cy * 16 + cx] = val;
    ((s16 *)g_BgAttributeSlotBlocks)[((x >> 4) & 3) | ((y >> 2) & 0xC)] = -1;
}

/* func_800157DC */
s32 bg_attribute_copy(s32 dx, s32 dy, s32 sx, s32 sy, s32 w, s32 h)
{
  s32 slot;
  s32 i;
  s32 j;
  s32 row_offset;
  s32 slot_row_bits;
  u32 block_column;
  u32 attribute;
  int block;
  j = 0;
  while (j < h)
  {
    i = 0;
    if (w > 0)
    {
      slot_row_bits = (((u32) sy) >> 2) & 0xC;
      row_offset = (sy & 0xF) << 4;
      do
      {
        if ((((u32) (sx + i)) < 0x100) && (((u32) sy) < 0x100))
        {
          goto do_tile_lookup;
        }
        attribute = 0;
        goto call_render;
        do_tile_lookup:
        block_column = ((u32) (sx + i)) >> 4;

        block = block_column + (sy & 0xF0);
        slot = (block_column & 0x3) | slot_row_bits;
        if (block != (*((s16 *) (g_BgAttributeSlotBlocks + (slot << 1)))))
        {
          bg_attribute_block_load(sx + i, sy);
        }
        attribute = *((u16 *) (secondary_tile_table + (((((sx + i) & 0xF) + row_offset) + (slot << 8)) << 1)));
        call_render:
        bg_attribute_set(dx + i, dy, attribute);

        i++;
      }
      while (i < w);
    }
    sy++;
    dy++;
    j++;
  }

}

/* far_syms: secondary_tile_table (target reaches it %hi/%lo) */

/* func_80015928 */
u32 bg_attribute_get(u32 x, u32 y)
{
  u8 *p1;
  u8 *p2;
  u32 block;
  u32 block_row_bits;
  u32 slot;
  s16 slot_block;

  if (x >= 0x100)
    return 0;
  if (y >= 0x100)
    return 0;

  block = x >> 4;
  block_row_bits = y & 0xF0;
  slot = (block & 0x3) | ((y >> 2) & 0xC);
  p1 = g_BgAttributeSlotBlocks;
  slot_block = *(s16 *)(p1 + (slot << 1));
  block += block_row_bits;
  if (block != slot_block)
    bg_attribute_block_load(x, y);
  p2 = secondary_tile_table;
  return *(u16 *)(p2 + (((x & 0xF) + ((y & 0xF) << 4) + (slot << 8)) << 1));
}
