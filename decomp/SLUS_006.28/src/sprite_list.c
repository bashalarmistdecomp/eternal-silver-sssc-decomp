/* Lunar SSSC — sprite_list: the sprite list: its set-up and reset, each
   sprite's pieces drawn, boxed and outlined, the list's draw, and the mix
   (levels, colours, the mix palette). The first split had it in
   rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
/* Sony's libgpu.h declares GetClut as returning u_short, and a caller compiled under that
   masks the result to 16 bits before using it as a word. sprite_draw uses it unmasked
   (`move a2,v0`), so the original of this file called GetClut with no such prototype in
   scope. The header's declaration is renamed out of the way for this file, and the function
   is declared below as it is called here. */
#define GetClut GetClut_sdk_prototype
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
#undef GetClut
s32 GetClut(s32, s32);
/* sprite_reset's decode targets for resource 2's three animation banks.
   g_SpriteDataAddresses (rodata) = { 0x800DE088, 0x800DDFB0, 0x800DF370 }. */
extern u8 g_BtlRunning;
extern u32 *g_PacketOtBase;
extern u32 g_SpriteCols[];
extern s32 g_SpriteMixLevelGrey;
extern s32 g_SpriteMixCopyCol;
extern u8 *g_PacketWritePointer;
extern u8 g_SpritePageCorners[];
extern s32 g_LoadBuffer;
extern long HWD0;
extern long VWD0;
extern s32 g_SpriteList;
extern s8 g_SpriteMixPaletteLoaded;
extern u8 D_800B890B[];
extern u16 g_SpriteMixCol;
extern unsigned char *g_ResourceReadBuffer;
extern u8 g_SpriteMixLevels[];
extern s32 g_PadButtons;
extern s8 g_SpriteMixOn;
/* far_syms: g_PacketWritePointer g_PacketOtBase */
/* far_syms: g_PacketWritePointer (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
/* far_syms: g_PacketOtBase (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
void MessageShadeDraw(void);
void frame_start();
extern u16 g_SpritePageTPages[];
s32 sprite_list_draw();
s32 frame_end();
void sprite_mix_switch_on(void);
void sprite_mix_switch_off(void);
s32 resource_load(s32, u16 *);
s8 *debug_digits_put(s8 *units_char, s32 number, s32 digit_count);
void sprite_piece_edges_get(void *piece, void *edges);
void sprite_mix_set(s32 group, s8 level);
void sprite_run_off(s32 first, u32 count);
int sprite_mix_level_conv(int level);
void sprite_mix_col_set(s16 col);
void sprite_palette_load(u16 *palette_place, u32 palette);
unsigned int sprite_mix_col_invert(unsigned int a);
void palette_rows_load_start(s32 first_row, s32 row_count, s32 cols);
void sprite_mix_init(void);
void func_80019AB4(void *entry);
void MessageShadeDraw(void);
void frame_start();
extern u16 g_SpritePageTPages[];
s32 sprite_list_draw();
s32 frame_end();
void sprite_mix_switch_on(void);
void sprite_mix_switch_off(void);
extern long HWD0, VWD0;
/* far_syms: g_PacketWritePointer (target reaches it %hi/%lo: lui/lw to load, lui $at/sw to store) */
void packet_string_add(s16, s16, s8 *, u16);
void SetSemiTrans(void *, s32);
s32 DrawSync(s32);
s32 inflate_buffer(u8 *, u8 *);
void sprite_page_effect_load(void *images);
extern long HWD0, VWD0;
void frame_start();
s32 resource_load(s32, u16 *);
s8 *debug_digits_put(s8 *units_char, s32 number, s32 digit_count);
void sprite_piece_edges_get(void *piece, void *edges);
void sprite_mix_set(s32 group, s8 level);
extern s32 g_PadPressedButtons;
extern long HWD0, VWD0;
/* far_syms: g_SpriteCols (target reaches it %hi/%lo) */
void sprite_run_off(s32 first, u32 count);
int sprite_mix_level_conv(int level);
void sprite_mix_col_set(s16 col);
void sprite_plain_draw(s16, s16, u8, u8, s32, s32, u16, u16, s32, s32, u16);
void sprite_mirror_draw(s16 x, s16 y, s32 tu, s32 tv, s32 tw, s32 th, u16 tp, u16 clut, s32 semi, s32 col, u16 ot, s32 mir);
void sprite_quad_draw(s16, s16, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, u16, u16, s32, s32, u16, s32, s32, s32, s32);
void sprite_scale_draw(s16, s16, u8 *, u16, u16, u16, u16, s32, s32, u16, s32, s32, s32);
extern u16 g_SpritePageTPages[];
s32 sprite_list_draw();
s32 frame_end();
/* far_syms: g_SpritePageCorners */
s32 resource_load(s32, u16 *);
void bg_scroll_get(s32 *x, s32 *y);  /* extern */

/* far_syms: g_SpritePageCorners (target reaches it %hi/%lo) */

/* func_80018AE0 */
void sprite_page_effect_load(void *images)
{
  u16 *image;
  s32 corner_offset;
  s32 i;
  u32 corners;
  RECT *rect_address;
  RECT rect;
  s32 image_length;
  i = 0;
  corners = (u32) g_SpritePageCorners;
  image = (u16 *) images;
  corner_offset = 0x1C;
  loop_1:
  if (image[0] != 0)
  {
    inflate_buffer(g_ResourceReadBuffer + (image[1] * 4), g_LoadBuffer);
    rect.x = *((u16 *) (corner_offset + corners));
    rect.y = *((u16 *) ((corner_offset + corners) + 2));
    rect.w = 0x80;
    image_length = image[0];
    rect_address = &rect;
    if (image_length < 0)
    {
      image_length += 0x3F;
    }
    image += 2;
    corner_offset += 4;
    i += 1;
    rect.h = image_length >> 6;
    LoadImage(rect_address, (u32 *) g_LoadBuffer);
    if (i < 3)
    {
      goto loop_1;
    }
  }

}

/* func_80018BAC */
void sprite_palette_load(u16 *palette_place, u32 palette)
{
  s32 first_col;
  first_col = ((u32 *) g_ResourceReadBuffer) + (*palette_place);
  *((u16 *) first_col) = 0;
  LoadClut((void *) (((u32 *) g_ResourceReadBuffer) + (*palette_place)), 0, 0x1F4 + ((palette >> 8) & 7));
}
extern s16 D_800B88E8;
extern s16 D_800B88EA;
extern s16 D_800B88EC;
extern s16 D_800DDD50[];
void sprite_reset(s32 y);  /* TU defines this below */

/* func_80018BFC */
void sprite_init(void)
{
  u16 *base;
  u16 *p;
  s16 *src;
  s16 *dst;
  s16 *dst2;
  s16 *corners;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `eight_bit_page_count` only holds the constant 0xA; behaviour is the same with the constant
     written where it is read, but then 3 of this function's 112 instruction lines
     differ. */
  int eight_bit_page_count;
  s32 i;
  int bit15;
  base = g_LoadBuffer;
  resource_load(0xE5, base);
  bit15 = -0x8000;
  i = 1;
  p = base + 1;
  base[0] = 0;
  do
  {
    i++;
    *p = (((*p) << 8) | ((*p) >> 8)) | bit15;
    p++;
  }
  while (i < 0x100);
  D_800B88EA = LoadClut(g_LoadBuffer, 0, 0x1F8);
  i = 1;
  DrawSync(0);
  resource_load(0xE6, g_LoadBuffer);
  bit15 = -0x8000;
  p = base + 1;
  base[0] = 0;
  do
  {
    i++;
    *p = (((*p) << 8) | ((*p) >> 8)) | bit15;
    p++;
  }
  while (i < 0x100);
  D_800B88E8 = LoadClut(g_LoadBuffer, 0, 0x1F9);
  i = 0;
  DrawSync(0);
  dst2 = D_800DDD50;
  src = g_SpritePageCorners;
  dst = g_SpritePageTPages;
gl0:
  *dst = GetTPage(1, 0, src[0], src[1]);
  i++;
  *dst2 = GetTPage(1, 0, src[0] + 0x40, src[1]);
  src += 2;
  dst++;
  dst2++;
  eight_bit_page_count = 0xA;
  if (i < eight_bit_page_count)
  {
    goto gl0;
  }
  i = eight_bit_page_count;
  dst = g_SpritePageTPages + eight_bit_page_count;
  corners = g_SpritePageCorners;
  src = corners + 0x14;
  do
  {
    *dst = GetTPage(0, 0, src[0], src[1]);
    src += 2;
    i++;
    dst++;
  }
  while (i < 0xC);
  D_800B88EC = GetClut(0, 0x1FA);
  sprite_reset(0);
}

extern SpriteDataList g_SpriteDataAddresses;
extern s32 D_800B890C;
void mem_clear(s32, s32);

/* func_80018DBC */
void sprite_reset(s32 y)
{
  RECT rect;
  SpriteDataList dst;
  u16 *res;
  u16 *clut;
  s32 i;
  s32 k;
  s32 n;
  u8 *d;
  u8 *q;
  dst = g_SpriteDataAddresses;
  g_SpriteList = 0x801D3B80;
  mem_clear(0x801D3B80, 0x1518);
  sprite_list_draw();
  res = (u16 *) g_ResourceReadBuffer;
  D_800B890C = 0;
  resource_load(2, res);
  clut = (u16 *) (g_ResourceReadBuffer + res[0] * 4);
  for (i = 15; i >= 0; i--)
  {
    clut[i * 16] = 0;
  }
  LoadClut(g_ResourceReadBuffer + res[0] * 4, 0, 0x1FB);
  for (i = 0; i < 0x100; i++)
  {
    ((u16 *) 0x801D6980)[i] = clut[i];
  }
  for (i = 0; i < 3; i++)
  {
    inflate_buffer(g_ResourceReadBuffer + res[1 + i * 2] * 4, dst.data[i]);
    d = dst.data[i];
    q = d + ((SpriteDataHead *) d)->texel_records * 4;
    n = (u32) (((SpriteDataHead *) d)->texel_records_end - ((SpriteDataHead *) d)->texel_records) / 3;
    for (k = 0; k < n; k++)
    {
      ((SpriteTexelRecord *) q)->page = 10;
      q += 12;
    }
  }
  rect.x = 0x3C0;
  rect.y = y;
  rect.w = 0x40;
  rect.h = 0x100 - y;
  inflate_buffer(g_ResourceReadBuffer + res[7] * 4, (u8 *) g_LoadBuffer);
  LoadImage(&rect, (u32 *) (g_LoadBuffer + (u16) y * 128));
}

/* func_80018FA4 */
s8 *debug_digits_put(s8 *units_char, s32 number, s32 digit_count)
{
  s32 next_power;
  s32 digit;
  s32 power;
  s32 i;
  s8 *digit_char;
  digit_char = units_char;
  power = 1;
  i = 0;
  while (i < digit_count)
  {
    next_power = power * 0xA;
    digit = ((s32) (number % next_power)) / power;
    i += 1;
    power = next_power;
    *digit_char = digit + 0x30;
    digit_char -= 1;
  }
  return digit_char;
}
typedef struct
{
  u8 c[8];
} Str8;
void sprite_piece_box_draw(s32, s32, s32, s32, u8 *, s32, u16);
void sprite_piece_outline_draw(s32, s32, s32, s32, u8 *, s32, u16);
void sprite_mix_palette_load(void);
extern u8 D_800B857C[];
extern u16 g_DebugPaletteCols[];
void sprite_plain_draw(s16, s16, u8, u8, s32, s32, u16, u16, s32, s32, u16);

/* Debug VRAM / CLUT viewer.  Every frame draws texture pages
   g_SpritePageTPages[page] and [page + 1] at (ox, oy).  With `show` on it reads
   the 256-pixel VRAM row (cx, cy) into g_DebugPaletteCols (StoreImage) whenever
   the row moves, draws it as a 16x16 grid of 7x7 TILEs coloured from each
   15-bit pixel, puts a translucent box behind the grid and another behind
   the page / row text (debug_digits_put formats both numbers into buf), each
   followed by a draw-mode word pair (0x01000000, 0xE1000200).  Pad
   (g_PadPressedButtons): 0x800 leaves, 0x80 toggles the grid, 0x100 resets.  With
   hold (g_PadButtons) bit 4 the d-pad moves the row (cx 0..0xFF in steps of
   16, cy clamped to the CLUT rows 0x1F4..0x1FF); with bit 1 it steps the
   page (+-1, +-5, clamped 0..11); otherwise it pans (ox, oy).

   CODEGEN FORCING DEVICES, not recovered source:
   lit_0xC, lit_0x200 and lit_0xE1000200 (locals holding constants; reload
   rematerializes the first two, which is why the target shows immediates),
   ot_base / tag_index (copies), the empty-statement label in the tile loop,
   and the two do-while(0) wrappers.  Behaviour-identical.  They exist
   because the tile loop needs all nine callee-saved registers, so the
   seven whole-function locals (cy..show), loop.c's hoisted y copy and the
   hoisted constant 7 live on the stack, and WHICH pseudos lose — and
   whether loop.c hoists the 7 at all — hangs on global-alloc priorities
   and loop sizes that these devices tilt to the target's choice.  The
   device-free C scores 235-250/393 (it spills `r` and keeps the y copy in
   $s2).  Replace them if the original shape turns up.  Recovered source,
   by contrast: AddPrim(ot, t++) and AddPrim(ot, q++) -- the increment
   inside the argument is what places the a1 copy after the colour stores
   (390 -> 393). */
/* func_80019044 */
void debug_sprite_page_show(void)
{
  s32 cy;
  s32 pcy;
  s32 cx;
  s32 pcx;
  s32 ox;
  s32 oy;
  s32 show;
  s32 page;
  s32 i;
  s32 j;
  s32 pad;
  s32 hold;
  s16 py;
  u16 *c;
  u8 r;
  u8 g;
  u32 *ot_base;
  u8 b;
  RECT rect;
  Str8 buf;
  int tag_index;
  TILE *t;
  u32 *dm;
  TILE *q;
  s32 lit_0xC;
  s32 lit_0x200;
  s32 lit_0xE1000200;
  cy = 0x1F4;
  page = 0;
  pcy = 0;
  cx = 0;
  pcx = 0;
  ox = 0;
  oy = 0;
  show = 0;
  buf = *((Str8 *) D_800B857C);
  lit_0xC = 0xC;
  lit_0x200 = 0x200;
  for (;;)
  {
    frame_start();
    py = oy;
    sprite_plain_draw(ox, py, 0, 0, 0x100, 0x100, g_SpritePageTPages[page], GetClut(cx, cy), 0, 0, 0x300);
    sprite_plain_draw(ox + 0x100, py, 0, 0, 0x50, 0x100, g_SpritePageTPages[page + 1], GetClut(cx, cy), 0, 0, 0x300);
    if (show)
    {
      if ((cy != pcy) || (cx != pcx))
      {
        rect.x = cx;
        rect.y = cy;
        rect.w = 0x100;
        rect.h = 1;
        StoreImage(&rect, g_DebugPaletteCols);
        pcy = cy;
        pcx = cx;
      }
      for (i = 0; i < 0x10; i++)
      {
        for (j = 0; j < 0x10; j++)
        {
          c = &g_DebugPaletteCols[(i * 16) + j];
          t = (TILE *) g_PacketWritePointer;
          r = (*c) << 3;
          g = ((*c) >> 2) & 0xF8;
          b = ((*c) >> 7) & 0xF8;
          SetTile(t);
          t->r0 = r;
          t->g0 = g;
          t->b0 = b;
          t->x0 = (j * 8) + 0x10;
          t->y0 = (i * 8) + 0x20;
          t->w = 7;
          dummy_label_654055:
          ;
          ;
          ;
          ;
          ;
          ;
          ;
          ;
          t->h = 7;
          AddPrim((s32) (g_PacketOtBase + lit_0x200), t++);
          g_PacketWritePointer = (u8 *) t;
        }
      }
      SetTile(t);
      SetSemiTrans(t, 1);
      t->r0 = 1;
      t->g0 = 1;
      t->b0 = 1;
      t->x0 = lit_0xC;
      t->y0 = 0x1C;
      t->w = 0x88;
      t->h = 0x88;
      AddPrim((s32) (g_PacketOtBase + lit_0x200), t);
      dm = (u32 *) (t + 1);
      ot_base = g_PacketOtBase;
      g_PacketWritePointer = (u8 *) dm;
      dm[0] = 0x01000000;
      lit_0xE1000200 = 0xE1000200;
      do
      {
        dm[1] = lit_0xE1000200;
      }
      while (0);
      AddPrim((s32) (ot_base + lit_0x200), dm);
      g_PacketWritePointer = ((u8 *) t) + 0x18;
      debug_digits_put(&buf.c[2], page, 2);
      debug_digits_put(&buf.c[6], cy - 0x1F4, 2);
      packet_string_add(0x10, 0x10, buf.c, 0x10);
      q = (TILE *) g_PacketWritePointer;
      SetTile(q);
      SetSemiTrans(q, 1);
      q->r0 = 1;
      q->g0 = 1;
      q->b0 = 1;
      q->x0 = 0xE;
      q->y0 = 0xE;
      q->w = 0x3C;
      q->h = lit_0xC;
      AddPrim((s32) (g_PacketOtBase + lit_0x200), q++);
      g_PacketWritePointer = (u8 *) q;
      tag_index = 0;
      ((u32 *) q)[tag_index] = 0x01000000;
      ((u32 *) q)[1] = 0xE1000200;
      AddPrim((s32) (g_PacketOtBase + lit_0x200), q);
 do { } while (0);
      q = (TILE *) (((u8 *) q) + 8);
      g_PacketWritePointer = (u8 *) q;
    }
    frame_end();
    pad = g_PadPressedButtons;
    if (pad & 0x800)
    {
      return;
    }
    if (pad & 0x80)
    {
      show ^= 1;
    }
    if (pad & 0x100)
    {
      cy = 0x1F4;
      page = 0;
      oy = 0;
      ox = 0;
    }
    hold = g_PadButtons;
    if (hold & 4)
    {
      if (pad & 0x8000)
      {
        cx -= 0x10;
      }
      if (pad & 0x2000)
      {
        cx += 0x10;
      }
      if (pad & 0x1000)
      {
        cy--;
      }
      if (pad & 0x4000)
      {
        cy++;
      }
      cy = (cy < 0x1F4) ? (0x1F4) : ((cy < lit_0x200) ? (cy) : (0x1FF));
      cx = (cx < 0) ? (0) : ((cx >= 0x100) ? (0xFF) : (cx));
    }
    else
      if (hold & 1)
    {
      if (pad & 0x8000)
      {
        page--;
      }
      if (pad & 0x2000)
      {
        page++;
      }
      if (pad & 0x1000)
      {
        page -= 5;
      }
      if (pad & 0x4000)
      {
        page += 5;
      }
      page = (page < 0) ? (0) : ((page < lit_0xC) ? (page) : (0xB));
    }
    else
    {
      if (hold & 0x8000)
      {
        ox--;
      }
      if (hold & 0x2000)
      {
        ox++;
      }
      if (hold & 0x1000)
      {
        oy--;
      }
      if (hold & 0x4000)
      {
        oy++;
      }
    }
  }
}
/* Draw a flat-coloured filled rectangle: the kind-0x4000 actor-frame slice
   (battle.md "The actor renderer").  sprite_piece_draw is the only caller: in
   battle (g_BtlRunning == 1) as (+08, +0A, +10 - +08, +12 - +0A, frame, semi,
   ot), otherwise as (x, y, +10 + 2, +12 + 2, frame, semi, ot).  Twin of
   sprite_piece_outline_draw below (the kind-0x5000 outline).

   One TILE (SetTile is libgpu SetTile; 0x10 bytes) at (x, y), w by h.
   Colour: the 15-bit word at frame +6 itself when its bit 15 is set, else
   entry [frame +6] of the u16 table at 0x801D5B80; r = (low byte & 0x1F)
   << 3, g = (word & 0x3E0) >> 2, b = (word & 0x7C00) >> 7.  It is linked at
   g_PacketOtBase[ot] (AddPrim).

   semi != 0 does two things: SetSemiTrans(p, 1) on the
   TILE, and a DR_TPAGE packet after it -- tag 0x01000000 (length 1), code
   0xE1000200 (the draw-mode command, dither bit set, texture page 0 = blend
   mode 0, background / 2 + colour / 2) -- linked at the SAME ordering-table
   entry.  AddPrim puts a packet at the head of its entry, so the DR_TPAGE,
   added second, is sent FIRST: it sets the blend mode the TILE is then
   drawn with.  The tag is written as a whole word, not with setlen().
   The packet pointer g_PacketWritePointer ends 0x10 on, or 0x18 with semi.  No frame
   wait, no RNG draw (the three callees are libgpu's SetTile, SetSemiTrans
   and AddPrim).

   Codegen: the twin's shape -- the colour pointer chosen
   through the named `pidx` (sprite_piece_outline_draw's header has what the other two
   spellings cost).  h stays in its argument home ($s0-$s7 and $fp are all
   taken) and is read back with lhu for the s16 store; `ot << 2` is one
   value kept across the first AddPrim for the second. */
/* func_80019668 */
void sprite_piece_box_draw(s32 x, s32 y, s32 w, s32 h, u8 *frame, s32 semi, u16 ot)
{
  u16 *pidx;
  u16 *rgb;
  TILE *p;
  DR_TPAGE *dm;
  s32 r;
  s32 g;
  s32 b;

  pidx = (u16 *) (frame + 6);
  rgb = ((*pidx) & 0x8000) ? (pidx) : (((u16 *) 0x801D5B80) + (*pidx));
  p = (TILE *) g_PacketWritePointer;
  r = ((*((u8 *) rgb)) & 0x1F) << 3;
  g = ((*rgb) & 0x3E0) >> 2;
  b = ((*rgb) & 0x7C00) >> 7;
  SetTile(p);
  if (semi)
  {
    SetSemiTrans(p, 1);
  }
  p->r0 = r;
  p->g0 = g;
  p->b0 = b;
  p->x0 = x;
  p->y0 = y;
  p->w = w;
  p->h = h;
  AddPrim((s32) (g_PacketOtBase + ot), p++);
  g_PacketWritePointer = (u8 *) p;
  if (semi)
  {
    dm = (DR_TPAGE *) p;
    dm->tag = 0x01000000;
    dm->code[0] = 0xE1000200;
    AddPrim((s32) (g_PacketOtBase + ot), dm);
    g_PacketWritePointer = (u8 *) (dm + 1);
  }
}

/* Draw a flat-coloured rectangle OUTLINE: the kind-0x5000 actor-frame slice
   OUTSIDE battle (battle.md "The actor renderer").  sprite_piece_draw is the
   only caller, on g_BtlRunning != 1, as (x - 3, y - 3, frame +0x10 + 3,
   frame +0x12 + 3, frame, semi, ot); in battle it builds ONE packet itself
   from the slice's three points.

   Two LINE_F3 packets -- SetLineF3 is libgpu SetLineF3, and the 0x18
   stride is LINE_F3's (two connected lines + the terminator word), not a
   quad's -- which together close the box:
     packet 1: (x, y) -> (x + w, y) -> (x + w, y + h)       top, right
     packet 2: (x + w, y + h) -> (x, y + h) -> (x, y)       bottom, left

   Colour: the 15-bit word at frame +6 itself when its bit 15 is set, else
   entry [frame +6] of the u16 table at 0x801D5B80 (the in-battle packet in
   sprite_piece_draw indexes 0x801D6380).  r = (low byte & 0x1F) << 3,
   g = (word & 0x3E0) >> 2, b = (word & 0x7C00) >> 7, worked out once before
   the first call and stored into both packets.  semi != 0 sets the
   semi-transparency bit on each (SetSemiTrans(p, 1)); both
   are linked at g_PacketOtBase[ot] (AddPrim), packet 1 first,
   so packet 2 is drawn first; the packet pointer g_PacketWritePointer ends 0x30 on.

   Codegen: the colour pointer is chosen through the named `pidx`, the
   caller's own spelling.  The same conditional on (u16 *) (frame + 6)
   written out gives the reversed branch (99/104), and `rgb = frame + 6;
   if (!(*rgb & 0x8000)) rgb = table + *rgb;` gives the target's branch but
   one counted insn fewer before r is set, which ties x and r in global's
   priority (10/120 = 3/36) and hands x $s6 (93/104).  h stays in its
   argument home: all nine call-saved registers are taken. */
/* func_800197CC */
void sprite_piece_outline_draw(s32 x, s32 y, s32 w, s32 h, u8 *frame, s32 semi, u16 ot)
{
  u16 *pidx;
  u16 *rgb;
  LINE_F3 *p;
  s32 r;
  s32 g;
  s32 b;
  s32 x2;
  s32 y2;

  pidx = (u16 *) (frame + 6);
  rgb = ((*pidx) & 0x8000) ? (pidx) : (((u16 *) 0x801D5B80) + (*pidx));
  p = (LINE_F3 *) g_PacketWritePointer;
  r = ((*((u8 *) rgb)) & 0x1F) << 3;
  g = ((*rgb) & 0x3E0) >> 2;
  b = ((*rgb) & 0x7C00) >> 7;
  SetLineF3(p);
  if (semi)
  {
    SetSemiTrans(p, 1);
  }
  x2 = x + w;
  p->r0 = r;
  p->g0 = g;
  p->b0 = b;
  p->x0 = x;
  p->y0 = y;
  p->x1 = x2;
  p->y1 = y;
  p->x2 = x2;
  y2 = y + h;
  p->y2 = y2;
  AddPrim((s32) (g_PacketOtBase + ot), p++);
  SetLineF3(p);
  if (semi)
  {
    SetSemiTrans(p, 1);
  }
  p->r0 = r;
  p->g0 = g;
  p->b0 = b;
  p->x0 = x2;
  p->y0 = y2;
  p->x1 = x;
  p->y1 = y2;
  p->x2 = x;
  p->y2 = y;
  AddPrim((s32) (g_PacketOtBase + ot), p++);
  g_PacketWritePointer = (u8 *) p;
}
/* Draw a flat-coloured FILLED rectangle the size a frame record gives:
   sprite_piece_box_draw above (the kind-0x4000 actor-frame slice, battle.md "The
   actor renderer") without the caller's width and height and without
   semi-transparency.  Nothing in the image calls it.

   One TILE (SetTile = libgpu SetTile; 0x10 bytes) at (x, y), its
   width the u16 at frame +0x10 and its height the u16 at frame +0x12, both
   read before SetTile is called.  Colour exactly as in sprite_piece_box_draw and
   sprite_piece_outline_draw: the 15-bit word at frame +6 itself when its bit 15 is set,
   else entry [frame +6] of the u16 table at 0x801D5B80; r = (low byte &
   0x1F) << 3, g = (word & 0x3E0) >> 2, b = (word & 0x7C00) >> 7.  It is
   linked at g_PacketOtBase[ot] (AddPrim) and the packet pointer
   g_PacketWritePointer ends 0x10 on.  No SetSemiTrans and no DR_TPAGE packet.  No
   frame wait, no RNG draw (the two callees are libgpu's SetTile and
   AddPrim).

   The live sprite_piece_box_draw is called outside battle as (x, y, +0x10 + 2,
   +0x12 + 2, frame, semi, ot): this is that draw with the size taken
   straight from the record (no + 2) and the blend left alone.

   Signature, from the body alone (there is no caller): ot is read lhu from
   the FIFTH argument's slot, so a fourth argument exists and is never
   read -- its type is not provable (word, u8 and s16 give the same bytes)
   and neither is its meaning; it sits where sprite_piece_box_draw's list has semi
   once w and h are dropped, which is why it is spelled a word here.  x and
   y are words (16-bit ones cost 8 rows), unlike packet_outline_rgb_add's.

   Codegen: the colour pointer is an if / else here.  sprite_piece_box_draw's and
   sprite_piece_outline_draw's `?:` through the named pidx gives this function the
   reversed branch (65/68). */
/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_8001996C */
void sprite_piece_box_opaque_draw(s32 x, s32 y, u8 *frame, s32 unused, u16 ot)
{
  u16 *pidx;
  u16 *rgb;
  TILE *p;
  s32 r;
  s32 g;
  s32 b;
  s32 w;
  s32 h;

  pidx = (u16 *) (frame + 6);
  if ((*pidx) & 0x8000)
  {
    rgb = pidx;
  }
  else
  {
    rgb = ((u16 *) 0x801D5B80) + (*pidx);
  }
  p = (TILE *) g_PacketWritePointer;
  r = ((*((u8 *) rgb)) & 0x1F) << 3;
  g = ((*rgb) & 0x3E0) >> 2;
  b = ((*rgb) & 0x7C00) >> 7;
  w = AS(u16, ((SpritePiece *)frame)->x2);
  h = AS(u16, ((SpritePiece *)frame)->y2);
  SetTile(p);
  p->r0 = r;
  p->g0 = g;
  p->b0 = b;
  p->x0 = x;
  p->y0 = y;
  p->w = w;
  p->h = h;
  AddPrim((s32) (g_PacketOtBase + ot), p++);
  g_PacketWritePointer = (u8 *) p;
}

/* func_80019A7C */
void sprite_run_off(s32 first, u32 count) {
    u32 i;
    u16 *ptr;

    i = 0;
    ptr = (u16 *)((u8 *)g_SpriteList + first * 36);
    while (i < count) {
        *ptr = 0;
        ptr += 18;
        i++;
    }
}
int sprite_mix_level_conv(int level);  /* TU defines this below */
unsigned int sprite_mix_col_invert(unsigned int a);  /* TU defines this below */
void palette_rows_load_start(s32 first_row, s32 row_count, s32 cols);  /* TU defines this below */
void sprite_mix_col_set(s16 col);  /* TU defines this below */
void sprite_mix_init(void);  /* TU defines this below */

/* Ruling of 2026-10-14 (docs/naming.md): Walks a pose's pieces doing nothing, and nothing reaches it: nothing supports a name. */
void func_80019AB4(void *entry)
{
  int count;
  int poses;
  int pose_then_row;
  int i;
  count = 1;
  pose_then_row = *((unsigned short *) (((char *) entry) + (i = 0x18)));
  poses = *((int *) (((char *) entry) + 0x10));
  pose_then_row = ((pose_then_row * 2) * 2) + poses;
  if (poses)
  {
    count = *((unsigned short *) pose_then_row);
  }
  {
    i = 0;
    while (i < count)
    {
      i++;
    }

    i++;
  }
}

/* func_80019AF4 */
int sprite_mix_level_conv(int level)
{
    int v;
    int a;
    int b;
    int c;

    v = 0x20 - (signed char)level;
    a = v << 2;
    b = v << 18;
    c = v << 10;

    return b | c | a;
}
/* func_80019B1C */
unsigned int sprite_mix_col_invert(unsigned int a)
{
    unsigned int red;
    unsigned int green;
    unsigned int v2;

    red = a & 0xFF;
    green = (a >> 8) & 0xFF;
    v2 = (a >> 16) & 0xFF;

    red = 0x80 - red;
    green = 0x80 - green;
    v2 = 0x80 - v2;

    return (v2 << 16) | (green << 8) | red;
}
/* func_80019B54 */
void palette_rows_load_start(s32 first_row, s32 row_count, s32 cols)
{
    s16 sp[4];

    sp[1] = first_row + 0x1F4;
    sp[3] = row_count;
    sp[0] = 0;
    sp[2] = 0x100;
    LoadImage(sp, cols);
}
extern u16 g_SpriteMixPalette[];

/* func_80019B90 */
void sprite_mix_palette_load(void)
{
  /* Build the flat-colour CLUT and upload it: entry 0 transparent,
     entries 1..255 = g_SpriteMixCol | 0x8000 (the colour sprite_mix_col_set set,
     STP bit on), LoadImage'd by palette_rows_load_start to VRAM (0, 509) 256x1. */
  s32 i;
  u16 *p;
  p = g_SpriteMixPalette;
  *p++ = 0;
  for (i = 1; i < 256; i++)
  {
    *p++ = g_SpriteMixCol | 0x8000;
  }
  palette_rows_load_start(9, 1, (s32) g_SpriteMixPalette);
}

/* func_80019BEC */
void sprite_piece_edges_get(void *piece, void *edges)
{
  s16 left;
  s16 right;
  s16 top;
  s16 bottom;
  s32 x_mode;
  s32 y_mode;
  u8 modes;
  u32 left_width;
  u32 right_width;
  u32 top_height;
  u32 bottom_height;
  modes = ((u32) (((SpritePiece *)piece)->kind & 0xF00)) >> 8;
  x_mode = modes & 3;
  if (modes == 0)
  {
    ((s32 *) edges)[0] = (s32) ((SpritePiece *)piece)->x0;
    ((s32 *) edges)[1] = (s32) ((SpritePiece *)piece)->y0;
    ((s32 *) edges)[2] = (s32) ((SpritePiece *)piece)->x2;
    ((s32 *) edges)[3] = (s32) ((SpritePiece *)piece)->y2;
    return;
  }
  y_mode = modes >> 2;
  switch (x_mode)
  {
    case 1:
      left = ((SpritePiece *)piece)->x0;
      ((s32 *) edges)[0] = (s32) left;
      ((s32 *) edges)[2] = (s32) (left + ((SpritePiece *)piece)->x1);
      break;

    case 2:
      left_width = ((SpritePiece *)piece)->x1 + 1;
      ((s32 *) edges)[0] = (s32) (((SpritePiece *)piece)->x0 - (((s32) (left_width + (left_width >> 0x1F))) >> 1));
      right_width = ((SpritePiece *)piece)->x1 + 1;
      ((s32 *) edges)[2] = (s32) (((SpritePiece *)piece)->x0 + (((s32) (right_width + (right_width >> 0x1F))) >> 1));
      break;

    case 3:
      right = ((SpritePiece *)piece)->x0;
      ((s32 *) edges)[2] = (s32) right;
      ((s32 *) edges)[0] = (s32) (right - ((SpritePiece *)piece)->x1);
      break;

  }

  switch (y_mode)
  {
    case 1:
      top = ((SpritePiece *)piece)->y0;
      ((s32 *) edges)[1] = (s32) top;
      ((s32 *) edges)[3] = (s32) (top + ((SpritePiece *)piece)->y1);
      return;

    case 2:
      top_height = ((SpritePiece *)piece)->y1 + 1;
      ((s32 *) edges)[1] = (s32) (((SpritePiece *)piece)->y0 - (((s32) (top_height + (top_height >> 0x1F))) >> 1));
      bottom_height = ((SpritePiece *)piece)->y1 + 1;
      ((s32 *) edges)[3] = (s32) (((SpritePiece *)piece)->y0 + (((s32) (bottom_height + (bottom_height >> 0x1F))) >> 1));
      return;

    case 3:
      bottom = ((SpritePiece *)piece)->y0;
      ((s32 *) edges)[3] = (s32) bottom;
      ((s32 *) edges)[1] = (s32) (bottom - ((SpritePiece *)piece)->y1);
      return;

  }

}
/* far_syms: g_PacketWritePointer g_PacketOtBase g_BtlRunning */
void sprite_scale_draw(s16, s16, u8 *, u16, u16, u16, u16, s32, s32, u16, s32, s32, s32);
void sprite_mirror_draw(s16, s16, s32, s32, s32, s32, u16, u16, s32, s32, u16, s32);
void bg_cover_box_set(s32, s32, u16, u16, s32);

/* Draw one 0x18-byte frame slice of an actor (battle.md "The actor
   renderer"): kind = flags & 0x7000 -> 0 sprite (plain / X-mirrored /
   scaled), 0x1000-0x2000 anchored rect, 0x3000 free quad, 0x4000-0x5000
   untextured; mirror = actor bits (entry +0x08 >> 13) ^ slice bits.
   hx/hy are the 0x3000/rect cases' scale and case 0's screen x/y (one
   pair of variables, which is why case 0's position sits in $s3/$s1). */
/* func_80019DBC */
void sprite_piece_draw(u8 *entry, u8 *frame, s32 x, s32 y, u16 clut, s32 semi, s32 col, s32 ot)
{
  s32 c;
  s32 idx;
  u8 *desc;
  u16 *pidx;
  u16 *rgb;
  u16 fl;
  u8 abits;
  u8 sbits;
  u32 sflip;
  u32 mir;
  s32 hx;
  u32 aflip;
  s32 hy;
  s32 saved;
  s32 rect[4];
  LINE_F3 *p;
  idx = (((SpriteEntry *)entry)->flags >> 10) & 3;
  if ((g_SpriteMixOn != 0) && ((c = ((s8 *) g_SpriteMixLevels)[idx]) != (-1)))
  {
    g_SpriteMixLevelGrey = sprite_mix_level_conv(c);
    g_SpriteMixCopyCol = sprite_mix_col_invert(g_SpriteMixLevelGrey);
    if (g_SpriteMixPaletteLoaded == 0)
    {
      sprite_mix_palette_load();
      g_SpriteMixPaletteLoaded++;
    }
  }
  else
  {
    g_SpriteMixLevelGrey = 0x808080;
  }
  pidx = (u16 *) (frame + 6);
  desc = (*((u8 **) (entry + 0xC))) + ((*pidx) * 12);
  fl = *((u16 *) frame);
  abits = (((SpriteEntry *)entry)->attr >> 9) & 0x30;
  sbits = fl & 0x30;
  mir = (abits ^ sbits) >> 4;
  aflip = abits >> 4;
  sflip = sbits >> 4;
  switch (fl & 0x7000)
  {
    case 0x4000:
      if (g_BtlRunning == 1)
    {
      sprite_piece_box_draw(((SpritePiece *)frame)->x0, ((SpritePiece *)frame)->y0, ((SpritePiece *)frame)->x2 - ((SpritePiece *)frame)->x0, ((SpritePiece *)frame)->y2 - ((SpritePiece *)frame)->y0, frame, semi, ot);
    }
    else
    {
      sprite_piece_box_draw(x, y, ((SpritePiece *)frame)->x2 + 2, ((SpritePiece *)frame)->y2 + 2, frame, semi, ot);
    }
      break;

    case 0x5000:
      rgb = ((*pidx) & 0x8000) ? (pidx) : (((u16 *) 0x801D6380) + (*pidx));
      if (g_BtlRunning == 1)
    {
      /* one LINE_F3 through the slice's three points (SetLineF3 is
         libgpu SetLineF3, SetSemiTrans, AddPrim); typed POLY_F4 it gives the same bytes -- same size and the
         same offsets for every field written */
      p = (LINE_F3 *) g_PacketWritePointer;
      SetLineF3(p);
      if (semi)
      {
        SetSemiTrans(p, 1);
      }
      p->r0 = ((*((u8 *) rgb)) & 0x1F) << 3;
      p->g0 = ((*rgb) & 0x3E0) >> 2;
      p->b0 = ((*rgb) & 0x7C00) >> 7;
      p->x0 = ((SpritePiece *)frame)->x1;
      p->y0 = ((SpritePiece *)frame)->y1;
      p->x1 = ((SpritePiece *)frame)->x0;
      p->y1 = ((SpritePiece *)frame)->y0;
      p->x2 = ((SpritePiece *)frame)->x3;
      p->y2 = ((SpritePiece *)frame)->y3;
      AddPrim((s32) (g_PacketOtBase + ot), p++);
      g_PacketWritePointer = (u8 *) p;
    }
    else
    {
      sprite_piece_outline_draw(x - 3, y - 3, ((SpritePiece *)frame)->x2 + 3, ((SpritePiece *)frame)->y2 + 3, frame, semi, ot);
    }
      break;

    case 0x3000:
      if (((SpriteEntry *)entry)->scaled == 1)
    {
      hx = AS(u16, ((SpriteEntry *)entry)->scale_x);
      hy = AS(u16, ((SpriteEntry *)entry)->scale_y);
    }
    else
    {
      hx = 0x100;
      hy = 0x100;
    }
      sprite_quad_draw(x, y, ((SpritePiece *)frame)->x0, ((SpritePiece *)frame)->y0, ((SpritePiece *)frame)->x1, ((SpritePiece *)frame)->y1, ((SpritePiece *)frame)->x2, ((SpritePiece *)frame)->y2, ((SpritePiece *)frame)->x3, ((SpritePiece *)frame)->y3, ((SpriteTexelRecord *)desc)->u, ((SpriteTexelRecord *)desc)->v, ((SpritePiece *)frame)->width, ((SpritePiece *)frame)->height, ((SpriteTexelRecord *)desc)->page, clut, semi, col, ot, sflip, aflip, hx, hy);
      break;

    case 0x1000:

    case 0x2000:
      if (((SpriteEntry *)entry)->scaled == 1)
    {
      hx = AS(u16, ((SpriteEntry *)entry)->scale_x);
      hy = AS(u16, ((SpriteEntry *)entry)->scale_y);
    }
    else
    {
      hx = 0x100;
      hy = 0x100;
    }
      sprite_piece_edges_get(frame, rect);
      sprite_quad_draw(x, y, rect[0], rect[1], rect[2], rect[1], rect[2], rect[3], rect[0], rect[3], ((SpriteTexelRecord *)desc)->u, ((SpriteTexelRecord *)desc)->v, ((SpritePiece *)frame)->width, ((SpritePiece *)frame)->height, ((SpriteTexelRecord *)desc)->page, clut, semi, col, ot, sflip, aflip, hx, hy);
      break;

    case 0:
      if (aflip & 1)
    {
      hx = (x - ((SpritePiece *)frame)->x0) - AS(u16, ((SpritePiece *)frame)->width);
    }
    else
    {
      hx = x + ((SpritePiece *)frame)->x0;
    }
      if (aflip & 2)
    {
      hy = (y - ((SpritePiece *)frame)->y0) - AS(u16, ((SpritePiece *)frame)->height);
    }
    else
    {
      hy = y + ((SpritePiece *)frame)->y0;
    }
      if (((SpriteEntry *)entry)->scaled == 1)
    {
      sprite_scale_draw(x, y, frame, ((SpriteTexelRecord *)desc)->u, ((SpriteTexelRecord *)desc)->v, g_SpritePageTPages[((SpriteTexelRecord *)desc)->page], clut, semi, col, ot, mir, ((SpriteEntry *)entry)->scale_x, ((SpriteEntry *)entry)->scale_y);
    }
    else
    {
      saved = g_SpriteCols[col];
      if ((((SpriteEntry *)entry)->flags & 0x6000) == 0x6000)
      {
        g_SpriteCols[col] = 0x808080;
      }
      if (mir)
      {
        sprite_mirror_draw(hx, hy, ((SpriteTexelRecord *)desc)->u, ((SpriteTexelRecord *)desc)->v, ((SpritePiece *)frame)->width, ((SpritePiece *)frame)->height, ((SpriteTexelRecord *)desc)->page, clut, semi, col, ot, mir);
      }
      else
      {
        sprite_plain_draw(hx, hy, AS(u8, ((SpriteTexelRecord *)desc)->u), AS(u8, ((SpriteTexelRecord *)desc)->v), ((SpritePiece *)frame)->width, ((SpritePiece *)frame)->height, g_SpritePageTPages[((SpriteTexelRecord *)desc)->page], clut, semi, col, ot);
      }
      g_SpriteCols[col] = saved;
    }
      break;

    case 0x6000:
      break;

  }

  if ((g_BtlRunning == 0) && ((((SpriteEntry *)entry)->flags & 0x6000) == 0x2000))
  {
    bg_cover_box_set(x + ((SpritePiece *)frame)->x0, y + ((SpritePiece *)frame)->y0, AS(u16, ((SpritePiece *)frame)->width), AS(u16, ((SpritePiece *)frame)->height), ot);
  }
}
/* far_syms: g_BtlRunning */
extern u8 D_800B8588[];

/* Draw one actor-list entry (battle.md "The actor renderer"): sprite_list_draw
   calls it for every live 0x24-byte entry of g_SpriteList, idx = its slot.
   entry: +0x02 x, +0x04 depth coordinate, +0x06 y (s16; each minus the
   camera from bg_scroll_get unless g_BtlRunning), +0x08 / +0x0A flag words,
   +0x10 pose table ({u16 count, u16 first} per pose), +0x14 slice table
   (0x18-byte records), +0x18 pose.
   Depth key: d = depth coordinate + 0x20 -> 0xFF - d inside 0..0xFF, 0xFF
   below, 0 above; slots 0x6E.. force 0 and slots 0x46..0x6D force 0xFF.
   Ordering-table index by (+0x0A & 0x6000): 0 -> 0x40; 0x2000 and 0x4000 ->
   key + 0x200; 0x6000 -> (0xFF - y) / 8 + 0x20.
   The slices go to sprite_piece_draw from the pose's LAST (first + count - 1)
   down to its first, all with that one index; a pose of more than 0x33
   slices draws none (the test breaks on the first pass).
   `str` is an unused local the original initialised from the literal "000"
   (D_800B8588, still a data-blob symbol here, hence the struct copy). */
/* func_8001A528 */
void sprite_draw(u8 *entry, s32 idx)
{
  typedef struct { u8 b[4]; } Str4;
  Str4 str = *((Str4 *) D_800B8588);
  s32 cx;
  s32 cy;
  s32 x;
  s32 y;
  s32 d;
  s32 v;
  u16 ot;
  s32 clut;
  s32 semi;
  s32 col;
  u16 *poses;
  s32 count;
  s32 i;
  x = ((SpriteEntry *)entry)->x;
  y = ((SpriteEntry *)entry)->y;
  d = ((SpriteEntry *)entry)->depth;
  g_SpriteMixPaletteLoaded = 0;
  if (g_BtlRunning == 0)
  {
    bg_scroll_get(&cx, &cy);
    x -= cx;
    y -= cy;
    d -= cy;
  }
  d += 0x20;
  if (d >= 0)
  {
    if (d < 0x100)
    {
      v = 0xFF - d;
    }
    else
    {
      v = 0;
    }
  }
  else
  {
    v = 0xFF;
  }
  d = v;
  if (idx >= 0x6E)
  {
    d = 0;
  }
  if (((u32) (idx - 0x46)) < 0x28U)
  {
    d = 0xFF;
  }
  switch (((SpriteEntry *)entry)->flags & 0x6000)
  {
    case 0:
      ot = 0x40;
      break;
    case 0x2000:
      ot = d + 0x200;
      break;
    case 0x4000:
      ot = d + 0x200;
      break;
    case 0x6000:
      ot = ((0xFF - y) / 8) + 0x20;
      break;
    default:
      ot = 0x200;
      break;
  }
  clut = GetClut(((SpriteEntry *)entry)->flags & 0xFF, ((((SpriteEntry *)entry)->flags >> 8) & 3) | 0x1F8);
  semi = ((((SpriteEntry *)entry)->flags >> 10) | (((SpriteEntry *)entry)->attr >> 15)) & 1;
  col = (((SpriteEntry *)entry)->flags >> 15) | ((((SpriteEntry *)entry)->attr >> 14) & 2);
  poses = *((u16 **) (entry + 0x10));
  if (poses != 0)
  {
    count = poses[((SpriteEntry *)entry)->pose * 2];
  }
  else
  {
    count = 1;
  }
  for (i = count - 1; i >= 0; i--)
  {
    if (i >= 0x33)
    {
      break;
    }
    sprite_piece_draw(entry, ((*((u8 **) (entry + 0x14))) + (poses[(((SpriteEntry *)entry)->pose * 2) + 1] * 0x18)) + (i * 0x18), x, y, clut, semi, col, ot);
  }
}

extern s8 g_DebugAllowed;

/* func_8001A798 */
s32 sprite_list_draw(void)
{
  s32 i;
  u16 *ptr;
  if ((g_DebugAllowed && ((g_PadButtons & 5) == 5)) && (g_PadPressedButtons & 0x800))
  {
    debug_sprite_page_show();
  }
  bg_cover_reset();
  ptr = (u16 *) g_SpriteList;
  ptr = (u16 *) ((u8 *) ptr + 0x9D8);
  for (i = 0; i < 0x28; i++)
  {
    if (*ptr)
    {
      sprite_draw(ptr, i + 0x46);
    }
    ptr += 18;
  }

  ptr = (u16 *) g_SpriteList;
  for (i = 0; i < 0x46; i++)
  {
    if (*ptr)
    {
      sprite_draw(ptr, i);
    }
    ptr += 18;
  }

  ptr = (u16 *) (((u8 *) g_SpriteList) + 0xF78);
  for (i = 0; i < 0x28; i++)
  {
    if (*ptr)
    {
      sprite_draw(ptr, i + 0x6E);
    }
    ptr += 18;
  }

  MessageShadeDraw();
}

/* far_syms: D_800B890B (target reaches it %hi/%lo) */
void sprite_mix_col_set(s16 col);  /* TU defines this below */

/* func_8001A89C */
void sprite_mix_init(void)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `no_mix` only holds the constant -1; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 13 instruction lines
     differs. */
  int no_mix;
  u8 *level;
  s32 group;
  no_mix = -1;
  group = 3;
  level = D_800B890B;
  do
  {
    *level = no_mix;
    group -= 1;
    level -= 1;
  }
  while (group >= 0);
  g_SpriteMixCol = 0;
  g_SpriteMixOn = 0;
  g_SpriteMixPaletteLoaded = 0;
}

/* func_8001A8D0 */
void sprite_mix_set(s32 group, s8 level)
{
  u8 *levels;
  levels = g_SpriteMixLevels;
  *((group & 0xFF) + levels) = level;
}

/* func_8001A8E8 */
void sprite_mix_switch_on(void) {
    g_SpriteMixOn = 1;
}

/* func_8001A8F8 */
void sprite_mix_switch_off(void) {
    g_SpriteMixOn = 0;
}

/* func_8001A904 */
void sprite_mix_col_set(s16 col) {
    g_SpriteMixCol = col;
}
/* Draw a flat-coloured FILLED rectangle in the colour (r, g, b):
   sprite_piece_box_draw's one TILE packet (SetTile = libgpu SetTile) with the
   colour given as three bytes instead of looked up from a frame record, and
   the filled counterpart of packet_outline_rgb_add below (the same nine arguments,
   an outline in two LINE_F3).
     TILE at (x, y), w by h, colour (r, g, b), linked at g_PacketOtBase[ot]
     (AddPrim).
   semi != 0 does what it does in sprite_piece_box_draw: SetSemiTrans(p, 1)
    on the TILE, and a DR_TPAGE packet after it -- tag
   0x01000000 (length 1), code 0xE1000200 (draw mode: dither on, texture
   page 0 = blend mode 0, background / 2 + colour / 2) -- linked at the SAME
   ordering-table entry.  AddPrim puts a packet at the head of its entry, so
   the DR_TPAGE, added second, is sent first and sets the blend mode the
   TILE is drawn with.  The packet pointer g_PacketWritePointer ends 0x10 on, or 0x18
   with semi.  No frame wait, no RNG draw (the three callees are libgpu's
   SetTile, SetSemiTrans and AddPrim).

   Nothing in the image calls it (no jal, no data word), so the parameter
   types are the body's own evidence, as in packet_outline_rgb_add: x / y / w / h
   take $s4 / $s5 / $s7 / $fp ahead of the colours, semi is read from its
   incoming slot at both tests (a word), ot is loaded lhu. */
/* func_8001A910 */
void packet_box_rgb_add(s16 x, s16 y, s16 w, s16 h, u8 r, u8 g, u8 b, s32 semi, u16 ot)
{
  TILE *p;
  DR_TPAGE *dm;

  p = (TILE *) g_PacketWritePointer;
  SetTile(p);
  if (semi)
  {
    SetSemiTrans(p, 1);
  }
  p->r0 = r;
  p->g0 = g;
  p->b0 = b;
  p->x0 = x;
  p->y0 = y;
  p->w = w;
  p->h = h;
  AddPrim((s32) (g_PacketOtBase + ot), p++);
  g_PacketWritePointer = (u8 *) p;
  if (semi)
  {
    dm = (DR_TPAGE *) p;
    dm->tag = 0x01000000;
    dm->code[0] = 0xE1000200;
    AddPrim((s32) (g_PacketOtBase + ot), dm);
    g_PacketWritePointer = (u8 *) (dm + 1);
  }
}

/* Draw a flat-coloured rectangle OUTLINE in the colour (r, g, b):
   sprite_piece_outline_draw's two LINE_F3 packets (SetLineF3) with the
   colour given as three bytes instead of looked up from a frame record.
     packet 1: (x, y) -> (x + w, y) -> (x + w, y + h)       top, right
     packet 2: (x + w, y + h) -> (x, y + h) -> (x, y)       bottom, left
   semi != 0 sets the semi-transparency bit on each (SetSemiTrans(p, 1)); both are linked at g_PacketOtBase[ot] (AddPrim) and the packet pointer g_PacketWritePointer ends 0x30 on.

   Nothing in the image or in MENU.BIN calls it, so the parameter types are
   the body's own evidence: b sits in a stack slot of its own (0x10($sp))
   rather than in its incoming slot, and x / y / w / h outrank the colours
   for registers -- both only happen for parameters narrower than a word
   (no equivalence with their homes).  semi, read from its incoming slot at
   each test, is a word; ot is loaded lhu.  The widths are proven, the
   signs are not (u16 coordinates and s8 colours give the same bytes).
   The two corner sums are s16: as ints each would sign-extend both of its
   operands first. */
/* func_8001AA38 */
void packet_outline_rgb_add(s16 x, s16 y, s16 w, s16 h, u8 r, u8 g, u8 b, s32 semi, u16 ot)
{
  LINE_F3 *p;
  s16 x2;
  s16 y2;

  p = (LINE_F3 *) g_PacketWritePointer;
  SetLineF3(p);
  if (semi)
  {
    SetSemiTrans(p, 1);
  }
  p->r0 = r;
  p->g0 = g;
  p->b0 = b;
  x2 = x + w;
  y2 = y + h;
  p->x0 = x;
  p->y0 = y;
  p->x1 = x2;
  p->y1 = y;
  p->x2 = x2;
  p->y2 = y2;
  AddPrim((s32) (g_PacketOtBase + ot), p++);
  SetLineF3(p);
  if (semi)
  {
    SetSemiTrans(p, 1);
  }
  p->r0 = r;
  p->g0 = g;
  p->b0 = b;
  p->x0 = x2;
  p->y0 = y2;
  p->x1 = x;
  p->y1 = y2;
  p->x2 = x;
  p->y2 = y;
  AddPrim((s32) (g_PacketOtBase + ot), p++);
  g_PacketWritePointer = (u8 *) p;
}
