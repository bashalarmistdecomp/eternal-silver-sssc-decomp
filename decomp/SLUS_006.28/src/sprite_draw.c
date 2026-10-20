/* Lunar SSSC — sprite_draw: the textured rectangles the sprites are drawn
   with (plain, grey, mirrored, scaled, four-point) and the sprite pages
   they draw from (page images sent, page 4, the page list). The first split
   had it in rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_SpritePageCorners */
/* far_syms: g_SpritePageCorners (target reaches it %hi/%lo) */
/* far_syms: g_PacketWritePointer g_PacketOtBase g_BtlRunning */
/* far_syms: g_BtlRunning */

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
extern u8 g_SpritePageUseMap[];
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
extern unsigned char *g_ResourceReadBuffer;
/* far_syms: g_PacketWritePointer g_PacketOtBase */
/* far_syms: g_PacketWritePointer (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
/* far_syms: g_PacketOtBase (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
extern u16 g_SpritePageTPages[];
s32 sprite_grey_draw(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void func_80016A80(int unused, int pass_count);
extern u16 g_SpritePageTPages[];
s32 sprite_grey_draw(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern long HWD0, VWD0;
/* far_syms: g_PacketWritePointer (target reaches it %hi/%lo: lui/lw to load, lui $at/sw to store) */
void packet_string_add(s16, s16, s8 *, u16);
void SetSemiTrans(void *, s32);
s32 inflate_buffer(u8 *, u8 *);
extern long HWD0, VWD0;

/* no_schedule_insns: screen_dark_draw */
extern long HWD0, VWD0;

/* func_800169A4 */
void screen_dark_draw(s32 grey)
{
  u8 *packet;
  s32 kept_grey;
  s32 ot_base;
  s16 height;
  u8 *pv;
  packet = (u8 *) (*((s32 *) (&g_PacketWritePointer)));
  kept_grey = grey;
  SetTile(packet);
  SetSemiTrans(packet, 0x1);
  ((TILE *)packet)->w = 0x140;
  height = 0xE0;
  ((TILE *)packet)->r0 = (u8) kept_grey;
  ((TILE *)packet)->g0 = (u8) kept_grey;
  ((TILE *)packet)->b0 = (u8) kept_grey;
  ot_base = *((s32 *) (&g_PacketOtBase));
  ((TILE *)packet)->x0 = 0x0;
  ((TILE *)packet)->y0 = 0x0;
  ((TILE *)packet)->h = height;
  pv = packet;
  packet += 0x10;
  AddPrim(ot_base + 0x20, pv);
  *((s32 *) (&g_PacketWritePointer)) = (s32) packet;
  SetDrawTPage(packet, 0x0, 0x0, (u16)GetTPage(0x1, 0x2, 0x140, 0x0));
  AddPrim((*((s32 *) (&g_PacketOtBase))) + 0x20, packet);
  packet += 0x8;
  *((s32 *) (&g_PacketWritePointer)) = (s32) packet;
}

/* far_syms: g_SpriteCols (target reaches it %hi/%lo) */

/* func_80016A74 */
void sprite_col_set(s32 col)
{
    g_SpriteCols[0] = col;
}

/* Ruling of 2026-10-14 (docs/naming.md): Counts in a loop with no effect, and nothing reaches it: nothing supports a name. */
void func_80016A80(int unused, int pass_count)
{
  int i;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `zero` only holds the constant 0; behaviour is the same with the constant written
     where it is read, but then 2 of this function's 13 instruction lines differ. */
  int zero;
  int j;
  pass_count >>= 4;
  zero = 0;
  if (pass_count > zero)
  {
    for (i = 0; i < pass_count; i++)
    {
      j = 15;
      do
      {
        j--;
      }
      while (j >= 0);
    }

  }
  return 15;
}
void SetPolyF3(POLY_F3 *);

/* Draw a flat-coloured FILLED TRIANGLE with free corners, in the
   colour (r, g, b):
       (x0, y0) - (x1, y1) - (x2, y2)
   One 0x14-byte POLY_F3 packet taken from the primitive buffer g_PacketWritePointer:
   SetPolyF3 (libgpu SetPolyF3) writes its length and code bytes, then
   come the three colour bytes and the six corner shorts, and the packet is
   linked at ordering-table cell g_PacketOtBase[ot] with AddPrim.
   The packet pointer g_PacketWritePointer ends 0x14 on, stored after the AddPrim call.
   No semi-transparency argument (the rectangle pair packet_box_rgb_add /
   packet_outline_rgb_add has one).  Nothing is returned; no RNG draw, no frame wait.

   Nothing calls it: no jal, j, data word or lui + addiu / ori forming its
   address in the unwrapped image or in MENU.BIN, none in a raw scan of
   LUNADATA.FIL on either disc.  So the parameter types are the body's own
   evidence, as for packet_box_rgb_add and packet_outline_rgb_add:

   * all six coordinates are 16 bits wide.  Eleven values live across the
     SetPolyF3 call and there are nine saved registers; the two left over
     are x1 and y1, and the target keeps them in two slots of its own frame
     (`sw $a2, 0x10($sp)`, `sw $a3, 0x14($sp)`, frame 0x40).  A register
     parameter as wide as a word is tied to its incoming home instead
     (assign_parms' REG_EQUIV note, which a converted parameter does not
     get): as ints x1 / y1 go to 0x40 / 0x44 of a 0x38 frame, 26/58.  Each
     of the other four changed to int alone moves the register ranking
     (49-51/58).  The widths are proven, the signs are not (u16 coordinates
     give the same bytes);
   * ot is 16 bits and unsigned (`lhu` from its slot, masked again at the
     shift; as an int 45/58);
   * r, g and b: u8 and s32 give the same bytes (`lw` from the slot, `sb`
     into the packet).  u8 is the list of the two functions named above;
   * BOTH callees are called as void functions.  The register that carries
     x1 and y1 from their slots to the packet is $v0; reload will not take a
     register the function's RTL names, and a call to a function that
     returns a value names $v0 (the value's register, even when the result
     is dropped).  With either callee returning s32 it is $v1, 54/58.
     libgpu.h has both void, and so have libgpu_p12.c and libgpu_p06.c;
   * the stores are the colour, then libgpu.h's setXY3 order, corner by
     corner (by axis 50/58; corners first 42/58), and `p++` belongs to the
     AddPrim statement (`p` passed and `p + 1` stored after: 56/58). */
/* func_80016AB4 */
void packet_triangle_add(s16 x0, s16 y0, s16 x1, s16 y1, s16 x2, s16 y2, u8 r, u8 g, u8 b, u16 ot)
{
    POLY_F3 *p;

    p = (POLY_F3 *)g_PacketWritePointer;
    SetPolyF3(p);
    p->r0 = r;
    p->g0 = g;
    p->b0 = b;
    p->x0 = x0;
    p->y0 = y0;
    p->x1 = x1;
    p->y1 = y1;
    p->x2 = x2;
    p->y2 = y2;
    AddPrim((s32)(g_PacketOtBase + ot), p++);
    g_PacketWritePointer = (u8 *)p;
}

/* Draw a string of up to eight characters in the 8 x 8 debug font:
   the debug page viewer debug_sprite_page_show is its only caller, with the
   "page / row" text debug_digits_put has just formatted, at (0x10, 0x10),
   ordering-table cell 0x10.

   For each character until a zero byte or the eighth: one 0x18-byte
   PacketSprite packet from the primitive buffer g_PacketWritePointer -- draw-mode word
   0xE1000200 | g_SpritePageTPages[0xB] (page slot 11, one of the two 4-bit pages),
   an 8 x 8 SPRT (code 0x64) of neutral colour 0x808080 at (x + i * 8, y),
   CLUT GetClut(0xE0, 0x1FB) -- linked at g_PacketOtBase[ot] with
   AddPrim.  The glyph for character c is at
       u = ((c - 0x20) & 0x1F) * 8,   v = 0xC0 + (c - 0x20) / 31 * 8
   so the font is 32 glyphs a row from v = 0xC0, but the row is found by
   dividing by 31, not 32 (the target's reciprocal multiply 0x84210843 and
   shift 4): characters 0x3F, 0x5E, 0x5F and 0x7D..0x7F land one row down
   from where the column says they are.  Code-proven, not seen on screen.
   The characters are signed (`lb`), and the cap of eight is the length of
   the caller's buffer, which has no terminator.

   What the bytes pin (each changed alone in this text): the x of a
   character is `x + i * 8`, an induction variable the loop pass reduces --
   its start copy `move $s3, $a0` sits after the hoisted invariants and its
   `+ 8` beside `i++` (an `x += 8` statement puts the copy first in the
   prologue, 82/85); y is 16 bits wide (as an int its entry copy is made
   before str's and it trades registers with the hoisted &g_SpritePageTPages,
   79/85; x may be either); u is a multiply (`<< 3` is 84/85); the string
   is s8 (plain char is unsigned here, 62/85; u8 with a cast at each read
   84/85); ot is narrow (masked once, ahead of the loop). */
/* func_80016B9C */
void packet_string_add(s16 x, s16 y, s8 *str, u16 ot)
{
    PacketSprite *p;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (str[i] == 0) {
            break;
        }
        p = (PacketSprite *)g_PacketWritePointer;
        p->tag = 0x05000000;
        *(u32 *)&p->r0 = 0x64808080;
        p->w = 8;
        p->h = 8;
        p->clut = GetClut(0xE0, 0x1FB);
        p->mode = g_SpritePageTPages[0xB] | 0xE1000200;
        p->x0 = x + i * 8;
        p->y0 = y;
        p->u0 = ((str[i] - 0x20) & 0x1F) * 8;
        p->v0 = (str[i] - 0x20) / 31 * 8 + 0xC0;
        AddPrim((s32)(g_PacketOtBase + ot), p++);
        g_PacketWritePointer = (u8 *)p;
    }
}

/* sprite_plain_draw's prototype sits ABOVE its (old-style) definition, like
   sprite_quad_draw's: w and h are ints in the prototype (sprite_piece_draw passes
   them with lh, debug_sprite_page_show as constants) and s16 in the definition, which
   narrows them on entry -- the plain `lw` into $t1/$t2 before the first
   store.  A prototype-style `s32 w` (with or without a local s16 copy) is
   read from its stack slot where it is used (71/104, two instructions
   short); a prototype-style `s16 w` matches too but contradicts the
   prototype the converted callers were matched under. */
void sprite_plain_draw(s16, s16, u8, u8, s32, s32, u16, u16, s32, s32, u16);

/* Draw one sprite: the plain (unmirrored, unscaled) actor-frame slice
   (battle.md "The actor renderer"; sprite_piece_draw's kind 0), the battle
   arena's two halves (sprite_grey_draw) and the debug page viewer's pages
   (debug_sprite_page_show).  One 0x18-byte PacketSprite packet taken from the
   primitive buffer g_PacketWritePointer: a draw-mode word 0xE1000200 | tp (tp is the
   texture-page word itself -- the caller has already been through
   g_SpritePageTPages) and a w x h SPRT at (x, y) with texels from (u, v), CLUT clut
   and colour g_SpriteCols[col]; semi sets the semi-transparency bit.

   When the tint g_SpriteMixLevelGrey is not neutral grey (0x808080) a second packet
   follows it in the buffer: a copy of the first with the tint colour
   g_SpriteMixCopyCol, CLUT GetClut(0, 0x1FD), blend mode 1 (mode | 0x20) and
   the semi-transparency bit.

   Both are linked at ordering-table cell g_PacketOtBase[ot] with AddPrim
   (which puts the packet at the HEAD of the cell's chain): the tint
   copy FIRST, the sprite itself LAST.  The chain then reads cell -> sprite
   -> tint copy -> whatever was linked there before, so the sprite is drawn
   first, its tint copy over it, and both UNDER every packet an earlier call
   linked at the same cell. */
/* func_80016CF0 */
void sprite_plain_draw(x, y, u, v, w, h, tp, clut, semi, col, ot)
s16 x;
s16 y;
u8 u;
u8 v;
s16 w;
s16 h;
u16 tp;
u16 clut;
s32 semi;
s32 col;
u16 ot;
{
    PacketSprite *p;
    PacketSprite *q;

    p = (PacketSprite *)g_PacketWritePointer;
    p->tag = 0x05000000;
    *(u32 *)&p->r0 = g_SpriteCols[col] | 0x64000000;
    if (semi != 0) {
        p->code |= 2;
    }
    p->w = w;
    p->h = h;
    p->clut = clut;
    p->mode = tp | 0xE1000200;
    p->x0 = x;
    p->y0 = y;
    p->u0 = u;
    p->v0 = v;
    q = p;
    p++;
    g_PacketWritePointer = (u8 *)p;
    if (g_SpriteMixLevelGrey != 0x808080) {
        q[1] = q[0];
        q[1].tag = 0x05000000;
        *(u32 *)&q[1].r0 = g_SpriteMixCopyCol | 0x64000000;
        q[1].mode |= 0x20;
        q[1].clut = GetClut(0, 0x1FD);
        q[1].code |= 2;
        AddPrim((s32)(g_PacketOtBase + ot), p++);
        g_PacketWritePointer = (u8 *)p;
    }
    AddPrim((s32)(g_PacketOtBase + ot), q);
}
/* no_schedule_insns: sprite_grey_draw */

/* Draw one plain sprite in a GREY of a given level: sprite_plain_draw's
   sprite with colour-table entry 0 replaced for the length of the call.
   BtlFrameDraw, the battle arena's frame draw, is its only caller, twice a
   frame while the shake amount g_BtlBgWave is 0 -- the arena's two texture
   pages as flat sprites:
       (0,     0, 0, 0, 0x100, 0xF0, g_SpritePageTPages[0], clut, 0, g_BtlBgGrey, 0x300)
       (0x100, 0, 0, 0, 0x50,  0xF0, g_SpritePageTPages[1], clut, 0, g_BtlBgGrey, 0x300)
   with clut = GetClut(0, 0x1F4).

   The colour word is built from the LOW BYTE of grey only, the same level in
   all three channels: grey | grey << 8 | grey << 16.  The old g_SpriteCols[0]
   is read, the grey word stored there, sprite_plain_draw called with x, y, u, v,
   w, h, tp, clut, semi, 0, ot -- col 0, so its SPRT takes g_SpriteCols[0] |
   0x64000000 -- and the old word put back.  x, y, u, v, w, h, tp, clut, semi
   and ot are sprite_plain_draw's and are passed through unchanged.  (The shear
   draw BtlBgWaveDraw, used instead while the arena shakes, stores the whole
   word g_BtlBgGrey in the same entry, not its low byte three times: the two
   draws put the same colour there only when that word is itself a grey.)

   What the bytes pin.  The parameters are NARROW in an old-style definition,
   like sprite_plain_draw's: the stack words are loaded on entry (`lw` for w and
   h, `lhu` for tp, clut and ot) and extended again at the call (`sll` / `sra`,
   `andi 0xFFFF`), because the store to g_SpriteCols[0] stands between the
   loads and the call and combine may not carry a load across it; grey's
   `lbu` has no mask because its use comes before that store.  With int
   parameters and casts at the call the loads fold into the
   extensions: `lh` / `lhu` alone, 38 instructions, 20/45.  The grey word is
   built in a local BEFORE the old entry is read: under the marker below, old
   entry first is 44/45, the `lbu` three rows late (without the marker the
   two orders give the same bytes).

   MARKED SPELLING: the compile marker `no_schedule_insns` (this TU already
   carries it for screen_dark_draw).  Without it this text is 39/45 and the six
   rows are one register swap: the grey byte in $v1 and the colour word in
   $v0, where the target has $v0 and $v1.  Measured in the .sched / .lreg
   dumps and with allocwhy: local-alloc ranks the two block-local values,
   six references each, by the length of their lives; the first scheduling
   pass always places the outgoing `sw $zero,0x24($sp)` (priority 1, and
   every incoming-argument load must stay ahead of it) between the `lbu` and
   the first `sll`, and nothing between the last `or` and the store, so the
   grey byte lives 14 against 12 (priority 0.857 against 1.000) and loses
   $v0.  None of about 170 texts passes 39/45 without the marker, and the 45
   or so read row by row at 39 all have this swap: both statement orders,
   with and without the local, narrow and prototype-style parameters, casts
   and masks, the callee prototyped with ints, with shorts or not at all,
   the entry reached through a cast, the word built in an inline helper.
   With the pass off the block is in source order when the
   registers are chosen, the grey byte's life is the shorter one, and all 45
   words match.  What the marker stands for is NOT known: the pass was on
   for this TU (with it off sprite_plain_draw falls from 104/104 to 40 and
   sprite_mirror_draw from 160/160 to 134).  The Japanese build's twin
   (0x80018410) makes the same $v0 / $v1 choice.

   MARKED SPELLING: an old-style definition that returns s32, to agree with
   this TU's forward declarations (s32, eleven s32 arguments), under which
   BtlFrameDraw was matched; nothing is returned and the caller does not
   read $v0.  By the bytes the function returns nothing and its arguments
   are s16 x, s16 y, u8 u, u8 v, s16 w, s16 h, u16 tp, u16 clut, s32 semi,
   u8 grey, u16 ot; `void`, or a prototype-style definition with these
   types, scores the same: 45/45 with the marker, 39/45 without.

   (This comment keeps a function's name clear of an opening parenthesis:
   the tree's definition finder reads `name (...) words ... ;` up to the
   next body as an old-style definition of that name.) */
/* func_80016E90 */
s32 sprite_grey_draw(x, y, u, v, w, h, tp, clut, semi, grey, ot)
s16 x;
s16 y;
u8 u;
u8 v;
s16 w;
s16 h;
u16 tp;
u16 clut;
s32 semi;
u8 grey;
u16 ot;
{
    u32 old;
    u32 c;

    c = grey | (grey << 8) | (grey << 16);
    old = g_SpriteCols[0];
    g_SpriteCols[0] = c;
    sprite_plain_draw(x, y, u, v, w, h, tp, clut, semi, 0, ot);
    g_SpriteCols[0] = old;
}

/* sprite_mirror_draw's prototype sits ABOVE its (old-style) definition, like
   sprite_quad_draw's and sprite_scale_draw's: sprite_piece_draw passes tw/th/mir as
   ints, the definition declares them u16 and narrows them on entry.  tu/tv
   are ints on both sides (they are decremented and tested < 0 unmasked).
   The prototype names its parameters only so that the paste can read the
   types (an old-style identifier list carries none). */
void sprite_mirror_draw(s16 x, s16 y, s32 tu, s32 tv, s32 tw, s32 th, u16 tp, u16 clut, s32 semi, s32 col, u16 ot, s32 mir);

/* Draw one kind-0 actor-frame slice MIRRORED, unscaled (battle.md "The actor
   renderer"; sprite_piece_draw calls it when mir = actor bits ^ slice bits is
   not 0 and the list entry's +0x1C is not 1 -- the plain slice goes to
   sprite_plain_draw's SPRT, which cannot flip, the scaled one to sprite_scale_draw).
   One POLY_FT4 covering the tw x th rectangle whose top-left is (x, y):
   vertices (x0,y0) (x1,y0) (x0,y1) (x1,y1) with texels (u0,v0) (u1,v0)
   (u0,v1) (u1,v1).  The mirror is done by the GEOMETRY, the texel edges stay
   in ascending order and are shifted down by one:
     X plain     (mir & 1) == 0: x0 = x,      x1 = x + tw, u0 = tu,     u1 = tu + tw
     X mirrored  (mir & 1) != 0: x0 = x + tw, x1 = x,      u0 = tu - 1, u1 = tu + tw - 1
     Y the same on mir & 2 with y/th/tv.
   u0 / v0 are clamped to 0 (a slice at texel 0 keeps u0 = 0 and loses one
   texel of width instead: tu = 0, tw--), u1 / v1 to 0xFF.  tp is the INDEX
   into the tpage table g_SpritePageTPages[] (sprite_plain_draw and sprite_scale_draw are
   handed the looked-up word), clut the CLUT; the colour word is
   g_SpriteCols[col] | 0x2C000000, semi sets the semi-transparency bit.  The
   quad is linked at ordering-table cell g_PacketOtBase[ot] with AddPrim
   (the new primitive goes to the HEAD of the cell's chain).  The
   tint tail is sprite_quad_draw's: when g_SpriteMixLevelGrey is not neutral grey
   (0x808080) a second copy is linked into the same cell FIRST, so it is
   drawn after (over) the quad itself -- colour g_SpriteMixCopyCol, CLUT
   GetClut(0, 0x1FD), blend mode 1 (tpage | 0x20), semi-transparent.

   tw and mir must be NARROW in the old-style definition (u16 here; s16 and,
   for mir, u8 give the same bytes): the int is loaded whole, copied to the
   narrow parameter and the first use still reads the int (lw $t1 / addu
   $t6,$t1 for tw, lw $v0 / addu $t3,$v0 / andi $v0,$v0,1 for mir).  An s32
   mir is loaded straight into its register, which frees $v0 in the first
   block and moves x and four more registers (120/160). */
/* func_80016F44 */
void sprite_mirror_draw(x, y, tu, tv, tw, th, tp, clut, semi, col, ot, mir)
s16 x;
s16 y;
s32 tu;
s32 tv;
u16 tw;
u16 th;
u16 tp;
u16 clut;
s32 semi;
s32 col;
u16 ot;
u16 mir;
{
    POLY_FT4 *p;
    POLY_FT4 *q;
    s16 x0;
    s16 x1;
    s16 y0;
    s16 y1;
    s16 u1;
    s16 vb;

    p = (POLY_FT4 *)g_PacketWritePointer;
    tp = g_SpritePageTPages[tp];
    if (mir & 1) {
        x0 = x + tw;
        tu--;
        x1 = x;
        if (tu < 0) {
            tu = 0;
            tw--;
        }
    } else {
        x0 = x;
        x1 = x + tw;
    }
    if (mir & 2) {
        tv--;
        y0 = y + th;
        y1 = y;
        if (tv < 0) {
            tv = 0;
            th--;
        }
    } else {
        y0 = y;
        y1 = y + th;
    }
    u1 = tw + tu;
    if (u1 >= 0x100) {
        u1 = 0xFF;
    }
    vb = th + tv;
    if (vb >= 0x100) {
        vb = 0xFF;
    }
    p->tag = 0x09000000;
    *(u32 *)&p->r0 = g_SpriteCols[col] | 0x2C000000;
    if (semi != 0) {
        p->code |= 2;
    }
    p->x0 = x0;
    p->y0 = y0;
    p->x1 = x1;
    p->y1 = y0;
    p->x2 = x0;
    p->y2 = y1;
    p->x3 = x1;
    p->y3 = y1;
    p->u0 = tu;
    p->v0 = tv;
    p->u1 = u1;
    p->v1 = tv;
    p->u2 = tu;
    p->v2 = vb;
    p->u3 = u1;
    p->v3 = vb;
    p->clut = clut;
    p->tpage = tp;
    q = p;
    p++;
    g_PacketWritePointer = (u8 *)p;
    if (g_SpriteMixLevelGrey != 0x808080) {
        *p = *q;
        p->tag = 0x09000000;
        p->clut = GetClut(0, 0x1FD);
        *(u32 *)&p->r0 = g_SpriteMixCopyCol | 0x2C000000;
        p->tpage |= 0x20;
        p->code |= 2;
        AddPrim((s32)(g_PacketOtBase + ot), p++);
        g_PacketWritePointer = (u8 *)p;
    }
    AddPrim((s32)(g_PacketOtBase + ot), q);
}
/* sprite_mirror_scale_draw's prototype sits ABOVE its (old-style) definition, like its
   neighbours' (sprite_mirror_draw, sprite_quad_draw): mir is an int in the prototype
   and u16 in the definition, which narrows it on entry.  NOTHING CALLS THIS
   FUNCTION -- the image holds no jal to 0x800171C4 and no pointer to it -- so
   there is no call site to prove the passing types: a prototype-style
   definition and a definition with no prototype give the same bytes.  The
   form is the family's. */
void sprite_mirror_scale_draw(s32 x, s32 y, s32 ox, s32 oy, s32 tu, s32 tv, s32 tw, s32 th, u32 hx, u32 hy, u16 tp, u16 clut, s32 semi, s32 col, u16 ot, s32 mir);

/* Draw one SCALED quad whose mirror is done by the TEXELS -- the unreferenced
   member of the actor-slice family (sprite_plain_draw plain SPRT, sprite_mirror_draw
   mirrored by geometry, sprite_quad_draw free quad, sprite_scale_draw scaled).  One
   POLY_FT4 whose top-left is (x + ((ox * hx) >> 8), y + ((oy * hy) >> 8)) and
   whose size is ((tw * hx) >> 8) x ((th * hy) >> 8): hx/hy are 8.8 fixed
   (0x100 = 1:1), nothing is clamped or widened.  Vertices (x0,y0) (x1,y0)
   (x0,y1) (x1,y1) with texels (u0,vt) (u1,vt) (u0,vb) (u1,vb), on mir (bit 0
   X, bit 1 Y) by sprite_quad_draw's sflip rule:
     plain     u0 = tu,           u1 = tu + tw - 1 (clamped to 0xFF)
     mirrored  u0 = tu + tw - 1 (clamped to 0xFF), u1 = tu - 1 (clamped to 0)
     and the same for vt / vb with tv / th.
   tp is the INDEX into the tpage table g_SpritePageTPages[] (as sprite_mirror_draw's),
   clut the CLUT; the colour word is g_SpriteCols[col] | 0x2C000000, semi sets
   the semi-transparency bit.  The quad is linked at ordering-table cell
   g_PacketOtBase[ot] with AddPrim, behind the family's tint
   tail: when g_SpriteMixLevelGrey is not neutral grey (0x808080) a second copy is
   linked into the same cell FIRST -- colour g_SpriteMixCopyCol, CLUT
   GetClut(0, 0x1FD), blend mode 1 (tpage | 0x20), semi-transparent.

   What the bytes fix:
   - x and y are INTS here (s16 in the neighbours): the scaled offset is
     added into the parameter's own register (addu $t5,$t5,$v0).  s16 x/y
     put the sums in registers of their own (120/203).
   - hx/hy are UNSIGNED: all four scalings shift logically (srl).  Signed
     hx/hy with a (u32) cast on each product give the same bytes.
   - mir is narrow (u16), as in sprite_mirror_draw: an s32 mir is loaded straight
     into its register (143/203).
   - `u0 = tu` comes FIRST in the plain X arm, like `vt = tv` in the Y arm.
     Placed after the clamp it compiles to the same instructions in that arm
     (it lands in the same delay slot) but u0 is then live for 77 insns
     instead of 85, outranks x in the global allocator (priority 0.130
     against x's 0.121; 0.118 as written) and takes $t5 (193/203). */
/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_800171C4 */
void sprite_mirror_scale_draw(x, y, ox, oy, tu, tv, tw, th, hx, hy, tp, clut, semi, col, ot, mir)
s32 x;
s32 y;
s32 ox;
s32 oy;
s32 tu;
s32 tv;
s32 tw;
s32 th;
u32 hx;
u32 hy;
u16 tp;
u16 clut;
s32 semi;
s32 col;
u16 ot;
u16 mir;
{
    POLY_FT4 *p;
    POLY_FT4 *q;
    s16 u0;
    s16 u1;
    s16 vt;
    s16 vb;
    s16 x1;
    s16 y1;

    p = (POLY_FT4 *)g_PacketWritePointer;
    tp = g_SpritePageTPages[tp];
    if (mir & 1) {
        u0 = tu + tw - 1;
        u1 = tu - 1;
        if (u0 >= 0x100) {
            u0 = 0xFF;
        }
        if (u1 < 0) {
            u1 = 0;
        }
    } else {
        u0 = tu;
        u1 = tu + tw - 1;
        if (u1 >= 0x100) {
            u1 = 0xFF;
        }
    }
    if (mir & 2) {
        vt = tv + th - 1;
        vb = tv - 1;
        if (vt >= 0x100) {
            vt = 0xFF;
        }
        if (vb < 0) {
            vb = 0;
        }
    } else {
        vt = tv;
        vb = tv + th - 1;
        if (vb >= 0x100) {
            vb = 0xFF;
        }
    }
    p->tag = 0x09000000;
    *(u32 *)&p->r0 = g_SpriteCols[col] | 0x2C000000;
    x += (ox * hx) >> 8;
    y += (oy * hy) >> 8;
    tw = (tw * hx) >> 8;
    th = (th * hy) >> 8;
    if (semi != 0) {
        p->code |= 2;
    }
    x1 = x + tw;
    y1 = y + th;
    p->x0 = x;
    p->y0 = y;
    p->x1 = x1;
    p->y1 = y;
    p->x2 = x;
    p->y2 = y1;
    p->x3 = x1;
    p->y3 = y1;
    p->u0 = u0;
    p->v0 = vt;
    p->u1 = u1;
    p->v1 = vt;
    p->u2 = u0;
    p->v2 = vb;
    p->u3 = u1;
    p->v3 = vb;
    p->clut = clut;
    p->tpage = tp;
    q = p;
    p++;
    g_PacketWritePointer = (u8 *)p;
    if (g_SpriteMixLevelGrey != 0x808080) {
        *p = *q;
        p->tag = 0x09000000;
        p->clut = GetClut(0, 0x1FD);
        *(u32 *)&p->r0 = g_SpriteMixCopyCol | 0x2C000000;
        p->tpage |= 0x20;
        p->code |= 2;
        AddPrim((s32)(g_PacketOtBase + ot), p++);
        g_PacketWritePointer = (u8 *)p;
    }
    AddPrim((s32)(g_PacketOtBase + ot), q);
}

/* sprite_quad_draw's prototype sits ABOVE its (old-style) definition: the texture
   height is passed as an int here and declared u16 there, which is what makes
   the callee re-read it with lhu while sprite_piece_draw passes it with lh. */
void sprite_quad_draw(s16, s16, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, u16, u16, s32, s32, u16, s32, s32, s32, s32);

/* Draw one actor-frame slice as a free quad (battle.md "The actor renderer"):
   a POLY_FT4 at (x, y) + four corner offsets -- (x0,y0) (x1,y1) top, (x2,y2)
   (x3,y3) bottom -- textured from the tw x th texel rectangle at (tu, tv) on
   page g_SpritePageTPages[tp].  hx/hy scale the corner offsets (8.8 fixed, 0x100 =
   1:1).  sflip bits 1/2 mirror the texel rectangle in u/v (the far edge is
   clamped to 0xFF, the mirrored near edge to 0); aflip bits 1/2 negate the
   corner x/y offsets (x also steps one pixel left when the texels are
   mirrored too).  Each right/bottom edge is then widened by one pixel.  The
   colour word is g_SpriteCols[col]; semi sets the semi-transparency bit.  When
   the tint g_SpriteMixLevelGrey is not neutral grey (0x808080) a second copy of the
   quad is queued in front: tint colour g_SpriteMixCopyCol, CLUT GetClut(0,
   0x1FD), blend mode 1 (tpage | 0x20), semi-transparent.

   An OLD-STYLE definition under the prototype above: th is an int in the
   prototype (sprite_piece_draw passes it with lh) and u16 here, so it is narrowed
   on entry (lhu) into its own spill slot -- a prototype-style `u16 th` makes
   the caller pass lhu, and an int parameter with a local u16 copy costs 6
   rows. */
/* func_800174F0 */
void sprite_quad_draw(x, y, x0, y0, x1, y1, x3, y3, x2, y2, tu, tv, tw, th, tp, clut, semi, col, ot, sflip, aflip, hx, hy)
s16 x;
s16 y;
s32 x0;
s32 y0;
s32 x1;
s32 y1;
s32 x3;
s32 y3;
s32 x2;
s32 y2;
s32 tu;
s32 tv;
s32 tw;
u16 th;
u16 tp;
u16 clut;
s32 semi;
s32 col;
u16 ot;
s32 sflip;
s32 aflip;
s32 hx;
s32 hy;
{
    POLY_FT4 *p;
    POLY_FT4 *q;
    s16 u0;
    s16 u1;
    s16 vt;
    s16 vb;

    if (hx != 0x100) {
        x0 = (x0 * hx) >> 8;
        x1 = (x1 * hx) >> 8;
        x3 = (x3 * hx) >> 8;
        x2 = (x2 * hx) >> 8;
    }
    if (hy != 0x100) {
        y0 = (y0 * hy) >> 8;
        y1 = (y1 * hy) >> 8;
        y3 = (y3 * hy) >> 8;
        y2 = (y2 * hy) >> 8;
    }
    p = (POLY_FT4 *)g_PacketWritePointer;
    tp = g_SpritePageTPages[tp];
    if (sflip & 1) {
        u0 = tw + tu - 1;
        u1 = tu - 1;
        if (u0 >= 0x100) {
            u0 = 0xFF;
        }
        if (u1 < 0) {
            u1 = 0;
        }
    } else {
        u1 = tw + tu - 1;
        if (u1 >= 0x100) {
            u1 = 0xFF;
        }
        u0 = tu;
    }
    if (sflip & 2) {
        vt = th + tv - 1;
        vb = tv - 1;
        if (vt >= 0x100) {
            vt = 0xFF;
        }
        if (vb < 0) {
            vb = 0;
        }
    } else {
        vb = th + tv - 1;
        if (vb >= 0x100) {
            vb = 0xFF;
        }
        vt = tv;
    }
    if (aflip & 1) {
        x0 = -x0;
        x1 = -x1;
        x3 = -x3;
        x2 = -x2;
        if (sflip & 1) {
            x0--;
            x1--;
            x3--;
            x2--;
        }
    }
    if (aflip & 2) {
        y0 = -y0;
        y1 = -y1;
        y3 = -y3;
        y2 = -y2;
    }
    p->tag = 0x09000000;
    *(u32 *)&p->r0 = g_SpriteCols[col] | 0x2C000000;
    if (semi != 0) {
        p->code |= 2;
    }
    if (x0 > x1) {
        x0++;
    } else {
        x1++;
    }
    if (x2 > x3) {
        x2++;
    } else {
        x3++;
    }
    if (y0 > y2) {
        y0++;
    } else {
        y2++;
    }
    if (y1 > y3) {
        y1++;
    } else {
        y3++;
    }
    p->x0 = x + x0;
    p->y0 = y + y0;
    p->x1 = x + x1;
    p->y1 = y + y1;
    p->x2 = x + x2;
    p->y2 = y + y2;
    p->x3 = x + x3;
    p->y3 = y + y3;
    p->u0 = u0;
    p->v0 = vt;
    p->u1 = u1;
    p->v1 = vt;
    p->u2 = u0;
    p->v2 = vb;
    p->u3 = u1;
    p->v3 = vb;
    p->clut = clut;
    p->tpage = tp;
    q = p;
    p++;
    g_PacketWritePointer = (u8 *)p;
    if (g_SpriteMixLevelGrey != 0x808080) {
        *p = *q;
        p->tag = 0x09000000;
        p->clut = GetClut(0, 0x1FD);
        *(u32 *)&p->r0 = g_SpriteMixCopyCol | 0x2C000000;
        p->tpage |= 0x20;
        p->code |= 2;
        AddPrim((s32)(g_PacketOtBase + ot), p++);
        g_PacketWritePointer = (u8 *)p;
    }
    AddPrim((s32)(g_PacketOtBase + ot), q);
}

/* sprite_scale_draw's prototype sits ABOVE its (old-style) definition, like
   sprite_quad_draw's: sprite_piece_draw passes mir/hx/hy as ints, the definition
   declares them u16/s16/s16 and narrows them on entry. */
void sprite_scale_draw(s16, s16, u8 *, u16, u16, u16, u16, s32, s32, u16, s32, s32, s32);

/* Draw one kind-0 actor-frame slice SCALED (battle.md "The actor renderer";
   sprite_piece_draw calls it when the list entry's +0x1C is 1, with hx/hy =
   entry +0x20/+0x22, 8.8 fixed): always a POLY_FT4, where the unscaled path
   draws sprite_plain_draw's SPRT or, mirrored, sprite_mirror_draw's quad.  frame is
   the 0x18-byte slice record: +2/+4 the texel size tw x th, +8/+0xA the
   offset from the actor origin.  Offset and size are scaled
   ((v * h) >> 8), the size clamped to at least 2.  The texels are ALWAYS
   (tu,tv)..(tu+tw-1, tv+th-1), the far edge clamped to 0xFF -- mirroring is
   done by the geometry alone (x1 left of x0 / y1 above y0), on mir = the
   actor bits ^ the slice bits (bit 0 X, bit 1 Y).

   Two placements, on g_BtlRunning (1 = battle):
     battle  X: x0 = x + dx, x1 = x0 + w; mirrored: x0 = x - dx, x1 = x0 - w
             Y the same with dy/h.  The scaled offset is NEGATED, so a
             mirrored slice flips about the actor origin, and nothing is
             widened.
     other   the size is widened by one (w + 1, h + 1) and the offset is not
             negated: x0 = x + dx, x1 = x0 + w + 1; mirrored: x1 = x + dx - 1,
             x0 = x1 + w - 1 (flipped in place, one pixel left, one narrower);
             y1 = y0 + h + 1, mirrored: y0 = y + dy + h + 2, y1 = y + dy.
   Vertices (x0,y0) (x1,y0) (x0,y1) (x1,y1).  tp is the tpage word itself
   (the caller looks g_SpritePageTPages[] up), clut the CLUT; the colour word is
   g_SpriteCols[col], semi sets the semi-transparency bit.  The tint tail is
   sprite_quad_draw's: when g_SpriteMixLevelGrey is not neutral grey (0x808080) a second
   copy is queued in front -- colour g_SpriteMixCopyCol, CLUT GetClut(0, 0x1FD),
   blend mode 1 (tpage | 0x20), semi-transparent.

   tw/th are read into locals FIRST and the width multiply uses the local:
   combine then folds that load into the multiply's lh and re-emits the HImode
   load after th's, which is the order sched1 keeps before the first branch
   (reading frame +2 in the multiply and again later puts the two lhu the
   other way round).  `x0 += w - 1` is the form that computes x0 - 1 first. */
/* func_80017988 */
void sprite_scale_draw(x, y, frame, tu, tv, tp, clut, semi, col, ot, mir, hx, hy)
s16 x;
s16 y;
u8 *frame;
u16 tu;
u16 tv;
u16 tp;
u16 clut;
s32 semi;
s32 col;
u16 ot;
u16 mir;
s16 hx;
s16 hy;
{
    POLY_FT4 *p;
    POLY_FT4 *q;
    s16 dx;
    s16 dy;
    s16 w;
    s16 h;
    s16 u1;
    s16 vb;
    s16 x0;
    s16 x1;
    s16 y0;
    s16 y1;
    s16 tw;
    s16 th;

    tw = ((SpritePiece *)frame)->width;
    th = ((SpritePiece *)frame)->height;
    dx = (((SpritePiece *)frame)->x0 * hx) >> 8;
    w = (tw * hx) >> 8;
    dy = (((SpritePiece *)frame)->y0 * hy) >> 8;
    if (w < 2) {
        w = 2;
    }
    h = (th * hy) >> 8;
    if (h < 2) {
        h = 2;
    }
    u1 = tu + tw - 1;
    if (u1 >= 0x100) {
        u1 = 0xFF;
    }
    vb = tv + th - 1;
    if (vb >= 0x100) {
        vb = 0xFF;
    }
    if (g_BtlRunning != 1) {
        w++;
        h++;
        if (mir & 1) {
            w--;
            dx--;
        }
        if (mir & 2) {
            h++;
        }
        x0 = x1 = x + dx;
        y0 = y1 = y + dy;
        if (mir & 1) {
            x0 += w - 1;
        } else {
            x1 = w + x1;
        }
        if (mir & 2) {
            y0 += h;
        } else {
            y1 += h;
        }
    } else {
        if (mir & 1) {
            x0 = x - dx;
            x1 = x0 - w;
        } else {
            x0 = x + dx;
            x1 = w + x0;
        }
        if (mir & 2) {
            y0 = y - dy;
            y1 = y0 - h;
        } else {
            y0 = y + dy;
            y1 = h + y0;
        }
    }
    p = (POLY_FT4 *)g_PacketWritePointer;
    p->tag = 0x09000000;
    *(u32 *)&p->r0 = g_SpriteCols[col] | 0x2C000000;
    if (semi != 0) {
        p->code |= 2;
    }
    p->x0 = x0;
    p->y0 = y0;
    p->x1 = x1;
    p->y1 = y0;
    p->x2 = x0;
    p->y2 = y1;
    p->x3 = x1;
    p->y3 = y1;
    p->u0 = tu;
    p->v0 = tv;
    p->u1 = u1;
    p->v1 = tv;
    p->u2 = tu;
    p->v2 = vb;
    p->u3 = u1;
    p->v3 = vb;
    p->clut = clut;
    p->tpage = tp;
    q = p;
    p++;
    g_PacketWritePointer = (u8 *)p;
    if (g_SpriteMixLevelGrey != 0x808080) {
        *p = *q;
        p->tag = 0x09000000;
        p->clut = GetClut(0, 0x1FD);
        *(u32 *)&p->r0 = g_SpriteMixCopyCol | 0x2C000000;
        p->tpage |= 0x20;
        p->code |= 2;
        AddPrim((s32)(g_PacketOtBase + ot), p++);
        g_PacketWritePointer = (u8 *)p;
    }
    AddPrim((s32)(g_PacketOtBase + ot), q);
}

/* Put a sheet's page into VRAM page slot 5 or 6 -- sprite_page_image_add's
   placement (below) on a DIFFERENT directory layout, and without the
   descriptor fix-up. Its one caller is MapPartyImagesSend (map_sprite.c),
   which no jal and no pointer word in the image reaches, nor MENU.BIN: as
   far as those go this is dead code, kept by the linker.

   id is a row of the directory at g_ResourceReadBuffer read as 16-BYTE rows: +8 a
   u32 offset in BYTES from the directory's start, +0xC a u32 size in
   bytes, 0x100 a line (the page is 0x80 VRAM words wide). The live
   loaders (MapPartySpriteDataLoad, sprite_page_image_add, sprite_page4_image_load) read the same
   block -- resource 0x1E8 -- as 8-byte rows: a u16 size at +4, 64 bytes a
   line, and a u16 offset in words at +6. So this function fits another
   layout of that directory than the shipped loaders use. arg1 is what the
   caller passes second (the sheet block); it is never read.

   The page needs n = (size / 256 + 7) / 8 eight-line units (unsigned).
   g_SpritePageUseMap is sprite_page_image_add's occupancy map, two columns of 0x20 units,
   column c being page slot 5 + c: column 0 then column 1 is tried, a
   column fitting when its first free unit `row` has row + n <= 0x20. When
   neither fits the loops run out with the column at 2 and column 1's row,
   and the page is loaded there regardless. It is inflated (inflate_buffer)
   from g_ResourceReadBuffer + offset into g_LoadBuffer and LoadImage'd at
   g_SpritePageCorners[5 + column] + (0, row * 8), 0x80 x size / 256; its units are
   marked id + 1. There is no id == -1 path and nothing is returned.

   Shape: sprite_page_image_add's skeleton (`end` set after the
   inner search, the mark loop written `k < row + n`, the directory base an
   integer local read once while the inflate call reads g_ResourceReadBuffer again).
   The return type is int with NO return statement, as the twins have it:
   as `void` reorg fills the mark loop's two branch slots with the next
   `addu $v0, ...` (84/88) -- in an int function $v0 is live at the end,
   so those slots stay nops, which is the target. */
/* Ruling of 2026-10-14 (docs/naming.md): Only code that nothing reaches calls this function; named for what it does. */
/* func_80017D50 */
s32 sprite_page_image_send(s32 id, s32 unused)
{
    RECT rect;
    s32 row;
    s32 i;
    s32 end;
    s32 n;
    s32 k;
    s32 base;
    u32 *e;
    u16 *pos;
    u8 *tbl;

    base = (s32)g_ResourceReadBuffer;
    n = ((*(u32 *)(id * 16 + base + 0xC) >> 8) + 7) >> 3;
    for (i = 0; i < 2; i++) {
        for (row = 0; row < 0x20; row++) {
            if (g_SpritePageUseMap[row + (i << 5)] == 0) {
                break;
            }
        }
        end = row + n;
        if (end <= 0x20) {
            break;
        }
    }
    tbl = g_SpritePageCorners;
    pos = (u16 *)(tbl + (i + 5) * 4);
    rect.x = pos[0];
    rect.y = pos[1] + row * 8;
    rect.w = 0x80;
    e = (u32 *)(id * 16 + base);
    rect.h = e[3] >> 8;
    inflate_buffer(g_ResourceReadBuffer + e[2], (u8 *)g_LoadBuffer);
    LoadImage(&rect, (u_long *)g_LoadBuffer);
    for (k = row; k < row + n; k++) {
        g_SpritePageUseMap[k + (i << 5)] = id + 1;
    }
}

extern u8 g_SpritePage4UseMap[];

/* One g_SpritePageCorners entry: where a page slot sits in VRAM. */

/* One 12-byte descriptor of a sprite block: +0x02 the page slot,
   +0x0A v. */

/* Put a FIELD sprite sheet's page into VRAM page slot 4 and point the
   sheet's descriptors at it (MapPartySpriteDataLoad calls it for a party
   member's second sheet, id + 0xF; sprite_page_image_add is the same for slots 5
   and 6, BtlPartyImageLoad for the battle party).

   id is the row of the sheet directory (resource 0x1E8, read into
   g_ResourceReadBuffer; 8-byte rows): +4 the page's size, which the C uses as
   rows -- size / 64 rows of 0x80 words, the rectangle LoadImage sends --
   and +6 its offset in words. arg1 is the inflated sheet block.

   g_SpritePage4UseMap is slot 4's occupancy map, twelve 8-line units. id == -1
   clears it and returns 0. Otherwise the page needs
   n = (size / 64 + 7) / 8 units and takes the first FREE unit `row`
   with row + n <= 12 (again only the first unit is tested); when there
   is none the loop runs out with row = 12 and the page is loaded there
   regardless (its units are then marked past the map's twelve bytes).
   The page is inflated (inflate_buffer) into g_LoadBuffer and
   LoadImage'd at g_SpritePageCorners[4] + (0, row * 8), 0x80 x size / 64; its
   units are marked id + 1; and every descriptor of the block -- the
   12-byte records from blk + texel_records * 4, (texel_records_end -
   texel_records) / 3 of them (SpriteDataHead) -- gets slot = 4 and
   v += row * 8. Nothing is returned on this path.

   Shape:
   - The clear loop counts UP in source: with its single store loop.c
     reverses it (check_dbra_loop) and must then reduce the address, which
     is the target's `s0 = 0xB; p = &map + s0; *p-- = 0` -- a loop written
     counting down keeps the indexed store.
   - Both divisions are SIGNED (bgez / addiu / sra): the u16 fields are
     read into int locals first -- dividing the u16 itself is shortened to
     an unsigned shift -- and each has its OWN local: one variable for
     both leaves the dividend's copy in a second register (measured on
     sprite_page_image_add, 124 -> 128/139).
   - The directory base is an integer local read once (`id * 8 + base`,
     index first); the inflate call reads g_ResourceReadBuffer again.
   - `end` is a variable, and the mark loop's counter is NOT the
     descriptor loop's: with one counter for both, cse makes it the
     canonical register of `k = row` and the entry test compares k, where
     the target compares row (110 -> 119/119).
   - The descriptors are indexed (`desc[d]`), as in BtlPartyImageLoad.
   - blk is a cast macro, not a local: a local copy of the parameter
     takes the callee-saved register ahead of id in sprite_page_image_add. */
/* func_80017EB0 */
s32 sprite_page4_image_load(s32 id, s32 sprite_data_address)
{
#define blk ((SpriteDataHead *)sprite_data_address)
    RECT rect;
    s32 row;
    s32 n;
    s32 k;
    s32 d;
    s32 end;
    s32 count;
    s32 size;
    s32 h;
    s32 base;
    u16 *e;
    SpriteTexelRecord *desc;
    SpritePageCorner *pos;

    if (id == -1) {
        for (row = 0; row < 0xC; row++) {
            g_SpritePage4UseMap[row] = 0;
        }
        return 0;
    }
    base = (s32)g_ResourceReadBuffer;
    size = *(u16 *)(id * 8 + base + 4);
    n = (size / 64 + 7) / 8;
    for (row = 0; row < 0xC; row++) {
        if (g_SpritePage4UseMap[row] == 0 && row + n <= 0xC) {
            break;
        }
    }
    pos = (SpritePageCorner *)g_SpritePageCorners;
    rect.x = pos[4].x;
    rect.y = pos[4].y + row * 8;
    rect.w = 0x80;
    e = (u16 *)(id * 8 + base);
    h = e[2];
    rect.h = h / 64;
    inflate_buffer(g_ResourceReadBuffer + e[3] * 4, (u8 *)g_LoadBuffer);
    LoadImage(&rect, (u_long *)g_LoadBuffer);
    end = row + n;
    for (k = row; k < end; k++) {
        g_SpritePage4UseMap[k] = id + 1;
    }
    desc = (SpriteTexelRecord *)((u8 *)blk + blk->texel_records * 4);
    count = (u32)(blk->texel_records_end - blk->texel_records) / 3;
    for (d = 0; d < count; d++) {
        desc[d].page = 4;
        desc[d].v += row * 8;
    }
#undef blk
}

/* Put a FIELD sprite sheet's page into VRAM page slot 5 or 6 and point
   the sheet's descriptors at it (MapPartySpriteDataLoad calls it for a party
   member's own sheet and for the twin sheet; sprite_page4_image_load is the same
   for slot 4).

   id is the row of the sheet directory (resource 0x1E8, read into
   g_ResourceReadBuffer; 8-byte rows): +4 the page's size, which the C uses as rows
   (size / 64 rows of 0x80 words), +6 its offset in words. arg1 is the inflated sheet block.

   g_SpritePageUseMap is the occupancy map: two columns of 0x20 eight-line units,
   column c being page slot 5 + c. id == -1 clears both columns, clears
   slot 4's map too (sprite_page4_image_load(-1, 0)) and returns 0. Otherwise the
   page needs n = (size / 64 + 7) / 8 units; column 0 then column 1 is
   tried, a column fitting when its first free unit `row` has row + n <=
   0x20. When neither fits the loops run out with the column at 2 and
   the row where column 1's search stopped, and the page is loaded there
   regardless (slot 7, its units marked past the map). The page is
   inflated (inflate_buffer) into g_LoadBuffer and LoadImage'd at
   g_SpritePageCorners[5 + column] + (0, row * 8), 0x80 x size / 64; its units are
   marked id + 1; and every descriptor of the block gets slot = 5 +
   column and v += row * 8. Nothing is returned on this path.

   Shape (build notes in sprite_page4_image_load's and BtlPartyImageLoad's headers):
   `i` is one variable for the clear loop's index and the column; `end`
   is a variable set after the inner search, while the mark loop is
   written `k < row + n` -- cse turns that into a copy of `end`, which
   loop.c hoists (the target's `a1 = s2`); k is ALSO the descriptor
   loop's counter here, so the mark loop's entry test compares k; the
   two sizes are separate int locals; blk is a cast macro, because a
   local copy of the parameter takes $s3 ahead of id. */
/* func_8001808C */
s32 sprite_page_image_add(s32 id, s32 sprite_data_address)
{
#define blk ((SpriteDataHead *)sprite_data_address)
    RECT rect;
    s32 row;
    s32 i;
    s32 end;
    s32 n;
    s32 k;
    s32 count;
    s32 size;
    s32 h;
    s32 base;
    u16 *e;
    SpriteTexelRecord *desc;
    SpritePageCorner *pos;
    u8 *tbl;
    u8 *m;

    if (id == -1) {
        for (i = 0; i < 0x20; i++) {
            m = g_SpritePageUseMap + i;
            m[0x20] = 0;
            m[0] = 0;
        }
        sprite_page4_image_load(-1, 0);
        return 0;
    }
    base = (s32)g_ResourceReadBuffer;
    size = *(u16 *)(id * 8 + base + 4);
    n = (size / 64 + 7) / 8;
    for (i = 0; i < 2; i++) {
        for (row = 0; row < 0x20; row++) {
            if (g_SpritePageUseMap[row + (i << 5)] == 0) {
                break;
            }
        }
        end = row + n;
        if (end <= 0x20) {
            break;
        }
    }
    tbl = g_SpritePageCorners;
    pos = (SpritePageCorner *)(tbl + (i + 5) * 4);
    rect.x = pos->x;
    rect.y = pos->y + row * 8;
    rect.w = 0x80;
    e = (u16 *)(id * 8 + base);
    h = e[2];
    rect.h = h / 64;
    inflate_buffer(g_ResourceReadBuffer + e[3] * 4, (u8 *)g_LoadBuffer);
    LoadImage(&rect, (u_long *)g_LoadBuffer);
    for (k = row; k < row + n; k++) {
        g_SpritePageUseMap[k + (i << 5)] = id + 1;
    }
    desc = (SpriteTexelRecord *)((u8 *)blk + blk->texel_records * 4);
    count = (u32)(blk->texel_records_end - blk->texel_records) / 3;
    for (k = 0; k < count; k++) {
        desc[k].page = i + 5;
        desc[k].v += row * 8;
    }
#undef blk
}
/* Load every packed page image of the scene's object resource into the
   VRAM page slots, in file order. Its callers (map_sprite.c:
   MapSpriteDataReload when its argument is 0; map_object.c: MapObjectsDataLoad,
   MapObjectsPagesLoad) have
   just read that resource to g_ResourceReadBuffer with resource_load. The resource's
   header is s32 words: [0] the object definitions' offset and [1] the
   sheet list's (MapObjectsDataLoad and MapObjectsSpriteDataReload read those), and from
   [2] on one BYTE offset per page, a 0 word ending the list.

   Nothing is done when hdr[2] is 0. Otherwise, for the n-th offset (n = 0,
   1, ...): the page is inflated (inflate_buffer) from g_ResourceReadBuffer + offset
   into the scratch buffer g_LoadBuffer and LoadImage'd as a 0x80 x 0x100
   rectangle -- one whole page, 0x80 VRAM words by 256 lines -- at
   g_SpritePageCorners[n] (x, y). So the n-th page of the file lands in page slot n;
   there is no slot field and no count, only the 0 word stops the walk
   (g_SpritePageCorners has twelve rows: slots 0-4 at x = 320 + 128 * k, y = 0,
   slots 5-9 the same at y = 256, slots 10 and 11 at x = 960, y = 0 and
   256). g_ResourceReadBuffer and g_LoadBuffer are read again from memory at each
   call; the offsets are read through the pointer taken on entry. Writes
   VRAM and the scratch buffer only. Nothing is returned (the callers drop
   $v0; the first split's scripting_e.c's `s32 sprite_page_list_load();` was a
   generated extern). No
   RNG draw, no frame wait.

   Shape:
   - MARKED SPELLING: the walk is a label and a goto, not a loop
     statement. As `do { } while (*p != 0);` the same text is 20/41: with
     loop notes loop.c moves the two RECT constants 0x80 and 0x100 out of
     the 24-insn loop into saved registers (-dL: both "moved"), and the
     target loads both inside the loop. With the table address formed
     inside a do-while instead it is 40/41: loop.c then moves the `lui` /
     `addiu` pair (and so leaves the constants), but puts it AFTER
     `i = 0`, and the target has `i = 0` last. The Japanese build's twin
     (0x800197B0, GCC 2.7.2) has the target's rows instruction for
     instruction ahead of its own epilogue: under the cc272 marker this text gives every
     row but eight that differ in addresses only -- six gp offsets and jal
     targets, two the recipe's same-register `la` (34/42) -- and the two
     do-while texts give 10/42 and 19/42. The same author's label loops:
     btl_command.c, and BtlDataLoad.
   - The entry test reads hdr[2] and `p` is formed after it: with
     `p = hdr + 2` ahead of the test $v0 and $v1 change places on four
     rows (37/41).
   - p, the table address and `i = 0` in that order (either swap 40/41).
   - The table address is an INTEGER local and the sum is written offset
     first, `i + tbl` (`tbl + i`, or a `u8 *` local, gives
     `addu $v1, $s2, $s1`: 40/41). `i` counts bytes: nothing reduces an
     index here.
   - The inflate call adds the offset to g_ResourceReadBuffer read again; through
     `hdr` it is 28/41. `hdr` itself is a reading aid: the test and the
     walk written on `g_ResourceReadBuffer + 8` give the same bytes. */
/* func_800182B8 */
void sprite_page_list_load(void)
{
    RECT rect;
    s32 *hdr;
    s32 *p;
    s32 i;
    s32 tbl;
    SpritePageCorner *pos;

    hdr = (s32 *)g_ResourceReadBuffer;
    if (hdr[2] != 0) {
        p = hdr + 2;
        tbl = (s32)g_SpritePageCorners;
        i = 0;
    loop:
        inflate_buffer(g_ResourceReadBuffer + *p++, (u8 *)g_LoadBuffer);
        pos = (SpritePageCorner *)(i + tbl);
        rect.x = pos->x;
        rect.y = pos->y;
        rect.w = 0x80;
        rect.h = 0x100;
        LoadImage(&rect, (u_long *)g_LoadBuffer);
        i += 4;
        if (*p != 0) {
            goto loop;
        }
    }
}
