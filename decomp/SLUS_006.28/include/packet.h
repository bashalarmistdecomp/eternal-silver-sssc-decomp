/* packet.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef PACKET_H
#define PACKET_H

#include "layout.h"

/* PacketSprite: a six-word GPU packet of the game's own, written at the packet
   write pointer g_PacketWritePointer: a draw-mode word (GP0 E1h, 0xE1000200 |
   the texture page) and then a free-size textured sprite in the layout of
   Sony's SPRT (libgpu.h) less its tag, so that one packet sets the texture
   page and draws. packet_sprite_add, packet_sprite_col_add (packet_sprite.c),
   bg_second_draw, bg_second_screen_draw, packet_string_add and
   sprite_plain_draw (sprite_draw.c) build it; the sprite fields keep Sony's SPRT
   names. */
typedef struct {
    /* +0x00 the packet's tag: the link to the next packet in the low 24 bits,
       the length 5 in the top byte. packet_sprite_add, packet_sprite_col_add,
       bg_second_draw (its sprites) and bg_second_screen_draw write the
       ordering-table entry's old link | 0x05000000 and put the packet's
       address + 0x80000000 in the entry; packet_string_add and
       sprite_plain_draw write 0x05000000 and link the packet with AddPrim. */
    u32 tag;
    /* +0x04 the draw-mode word, GP0 E1h: 0xE1000200 (dither on) | the texture
       page word. packet_sprite_add and packet_sprite_col_add take the page
       from their caller, bg_second_draw from g_BgSecondPacketTPage (0x1F, or
       0x3F in mode 5), bg_second_screen_draw writes 0xE100029D,
       packet_string_add g_SpritePageTPages[0xB], sprite_plain_draw its tp,
       with blend mode 1 (| 0x20) on its tint copy. */
    u32 mode;
    /* +0x08 the sprite's red (r0, g0, b0 and code are Sony's SPRT names).
       Every builder writes +0x08..+0x0B as one 32-bit word, the colour | the
       command 0x64000000 (a free-size textured sprite): packet_sprite_add
       0x64808080, packet_sprite_col_add its colour argument, bg_second_draw
       and bg_second_screen_draw g_BgSecondPacketCol, packet_string_add
       0x808080, sprite_plain_draw g_SpriteCols[col], and g_SpriteMixCopyCol on
       its tint copy. */
    u8 r0;
    /* +0x09 the green; written with r0 as one word. */
    u8 g0;
    /* +0x0A the blue; written with r0 as one word. */
    u8 b0;
    /* +0x0B the GPU command, 0x64, written with r0 as one word.
       sprite_plain_draw sets its semi-transparency bit (code |= 2) when semi
       is set, and always on its tint copy. */
    u8 code;
    /* +0x0C x on the screen of the sprite's top left: packet_sprite_add and
       packet_sprite_col_add from their caller, bg_second_draw the cell's
       column (sx), bg_second_screen_draw col * 16, packet_string_add x + i *
       8, sprite_plain_draw its x. */
    s16 x0;
    /* +0x0E y on the screen of the top left (bg_second_draw the row's sy,
       bg_second_screen_draw row * 16). */
    s16 y0;
    /* +0x10 the texel u in the page: bg_second_draw (cell & 0x1F) * 8,
       bg_second_screen_draw (cell & 0xF) * 16, packet_string_add the glyph's
       column * 8, the others their argument. */
    u8 u0;
    /* +0x11 the texel v in the page (bg_second_draw (cell & 0x1E0) >> 2;
       packet_string_add 0xC0 + the glyph's row * 8). */
    u8 v0;
    /* +0x12 the palette word: bg_second_draw 0x7F00 + (cell >> 12),
       bg_second_screen_draw 0x7D40, packet_string_add GetClut(0xE0, 0x1FB),
       sprite_plain_draw its clut and GetClut(0, 0x1FD) on its tint copy,
       packet_sprite_add and packet_sprite_col_add their argument. */
    u16 clut;
    /* +0x14 the sprite's width in pixels: bg_second_draw 8, or 16 or 32 for a
       wide cell, bg_second_screen_draw 16, packet_string_add 8, the others
       their argument. */
    s16 w;
    /* +0x16 the sprite's height in pixels (bg_second_draw 8,
       bg_second_screen_draw 16, packet_string_add 8). */
    s16 h;
} PacketSprite;

LAYOUT_SIZE(PacketSprite, 0x18);
LAYOUT_AT(PacketSprite, tag, tag, 0x00);
LAYOUT_AT(PacketSprite, mode, mode, 0x04);
LAYOUT_AT(PacketSprite, r0, r0, 0x08);
LAYOUT_AT(PacketSprite, g0, g0, 0x09);
LAYOUT_AT(PacketSprite, b0, b0, 0x0a);
LAYOUT_AT(PacketSprite, code, code, 0x0b);
LAYOUT_AT(PacketSprite, x0, x0, 0x0c);
LAYOUT_AT(PacketSprite, y0, y0, 0x0e);
LAYOUT_AT(PacketSprite, u0, u0, 0x10);
LAYOUT_AT(PacketSprite, v0, v0, 0x11);
LAYOUT_AT(PacketSprite, clut, clut, 0x12);
LAYOUT_AT(PacketSprite, w, w, 0x14);
LAYOUT_AT(PacketSprite, h, h, 0x16);

/* PacketSprite16: a five-word GPU packet of the game's own: a draw-mode word
   (GP0 E1h) and then a 16 x 16 textured sprite in the layout of Sony's SPRT_16
   (libgpu.h) less its tag, linked by hand at the head of an ordering-table
   entry (tag = the entry's old link | 0x04000000). The bg's first layer draws
   each unmirrored tile with one (bg_first_draw, bg_tile_draw, bg_tile.c),
   and so does the tile-map layer drawer (tile_layer_draw, tile_layer.c); the sprite
   fields keep Sony's SPRT_16 names. */
typedef struct {
    /* +0x00 the packet's tag: the entry's old link | 0x04000000 (length 4);
       the entry gets the packet's address + 0x80000000. Written by
       bg_first_draw (OT entry z, the tile's depth or its cover's),
       bg_tile_draw and tile_layer_draw (entry 0x300). */
    u32 tag;
    /* +0x04 the draw-mode word, 0xE1000200 | the texture page word:
       bg_first_draw and bg_tile_draw g_BgTPages[tp & 7], tile_layer_draw
       D_800AE0F8[pal & 7], with | 0x20 (blend mode 1) when g_TileSemiTransOn
       is set. */
    u32 mode;
    /* +0x08 the sprite's red (r0, g0, b0 and code are Sony's SPRT_16 names).
       Each builder writes +0x08..+0x0B as one 32-bit word, the colour | the
       command 0x7C000000 (a 16 x 16 textured sprite): bg_first_draw and
       bg_tile_draw the scratchpad's colour word at 0x1F800044 (bg_col_set),
       tile_layer_draw g_TileCol. */
    u8 r0;
    /* +0x09 the green; written with r0 as one word. */
    u8 g0;
    /* +0x0A the blue; written with r0 as one word. */
    u8 b0;
    /* +0x0B the GPU command, 0x7C, written with r0 as one word;
       tile_layer_draw sets its semi-transparency bit (code |= 2) when
       g_TileSemiTransOn is set. */
    u8 code;
    /* +0x0C x on the screen of the tile's top left (bg_first_draw: the
       column's sx, from -(x & 15) in steps of 16). */
    s16 x0;
    /* +0x0E y on the screen of the tile's top left. */
    s16 y0;
    /* +0x10 the texel u, the tile word's (word & 0xF) * 16. */
    u8 u0;
    /* +0x11 the texel v, the tile word's word & 0xF0. */
    u8 v0;
    /* +0x12 the palette word: bg_first_draw and bg_tile_draw g_BgCluts[tp >>
       3], tile_layer_draw D_800AE114[pal >> 3]. */
    u16 clut;
} PacketSprite16;

LAYOUT_SIZE(PacketSprite16, 0x14);
LAYOUT_AT(PacketSprite16, tag, tag, 0x00);
LAYOUT_AT(PacketSprite16, mode, mode, 0x04);
LAYOUT_AT(PacketSprite16, r0, r0, 0x08);
LAYOUT_AT(PacketSprite16, g0, g0, 0x09);
LAYOUT_AT(PacketSprite16, b0, b0, 0x0a);
LAYOUT_AT(PacketSprite16, code, code, 0x0b);
LAYOUT_AT(PacketSprite16, x0, x0, 0x0c);
LAYOUT_AT(PacketSprite16, y0, y0, 0x0e);
LAYOUT_AT(PacketSprite16, u0, u0, 0x10);
LAYOUT_AT(PacketSprite16, v0, v0, 0x11);
LAYOUT_AT(PacketSprite16, clut, clut, 0x12);

#endif
