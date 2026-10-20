/* Lunar SSSC — packet_sprite: the textured packets: the texture page word
   and the sprites (plain, coloured, 8 x 8). The first split had it in
   cdrom_load.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);

/* far_syms: g_PacketWritePointer g_PacketOtBase (target reaches them %hi/%lo) */
extern u8 *g_PacketWritePointer;
extern u32 *g_PacketOtBase;

/* Link a DR_TPAGE packet into ordering-table entry z. The OT link is the
   packet address + 0x80000000 (24-bit), an addu only since patch_cc1. */
/* func_8002566C */
void packet_tpage_add(u32 tpage, s32 z)
{
    u32 *p;
    u32 *ot;
    ot = (u32 *)((z << 2) + (s32)g_PacketOtBase);
    p = (u32 *)g_PacketWritePointer;
    p[1] = tpage | 0xE1000000;
    p[0] = *ot | 0x01000000;
    *ot = (u32)p + 0x80000000;
    g_PacketWritePointer = (u8 *)(p + 2);
}
/* far_syms: g_PacketWritePointer */

/* The draw-mode word and the colour word are stored as pointer accesses
   through their members' addresses (*(s32 *)&p->mode, *(s32 *)&p->r0):
   written as member stores (with AS() too), GCC orders the stores
   differently and the code no longer matches. The other fields are
   members. */
/* func_800256C0 */
s32 packet_sprite_add(s32 x, s32 y, s32 u, s32 v, s32 w, s32 h, s32 tpage, s32 clut, s32 z)
{
  s32 *ot_entry;
  void *packet;
  s32 ot_base;
  s16 *packet_y;
  void *link;
  packet = g_PacketWritePointer;
  *(s32 *)&((PacketSprite *) packet)->mode = (s32) (tpage | 0xE1000200);
  *(s32 *)&((PacketSprite *) packet)->r0 = 0x64808080;
  ((PacketSprite *) packet)->u0 = (s8) u;
  ((PacketSprite *) packet)->v0 = (s8) v;
  ot_base = *((s32 *) 0x1F800040);
  packet_y = &((PacketSprite *) packet)->y0;
  ((PacketSprite *) packet)->x0 = (s16) x;
  *packet_y = (s16) y;
  ((PacketSprite *) packet)->w = (s16) w;
  ((PacketSprite *) packet)->h = (s16) h;
  ((PacketSprite *) packet)->clut = (s16) clut;
  ot_entry = (z * 4) + ot_base;
  ((PacketSprite *) packet)->tag = (*ot_entry) | 0x05000000;
  link = packet + 0x80000000;
  *ot_entry = (s32) link;
  g_PacketWritePointer = (packet = packet + 0x18);
  return (s32) link;
}
/* far_syms: g_PacketWritePointer g_PacketOtBase */

/* Draw a w x h textured sprite at (x, y) from texel (u, v) of texture page
   `tpage` with palette `clut`, modulated by `color` (0x808080 = unchanged),
   at ordering-table depth z.  packet_sprite_add with the colour as a tenth
   argument; its one caller is IconCellDraw (icon.c), which draws a
   24 x 24 icon at half brightness (0x404040) or full.

   Builds one PacketSprite at the packet cursor g_PacketWritePointer: word 1 is the
   draw-mode command 0xE1000200 | tpage (GP0 E1h with dither on), word 2
   the SPRT command 0x64000000 | color (GP0 64h, a free-size textured
   rectangle), then x0 / y0, u0 / v0 / clut and w / h.  Links it at the
   head of entry z of the ordering table g_PacketOtBase: the tag is the entry's
   old link with length 5 in the top byte, and the entry gets the packet
   address + 0x80000000 (the 24-bit link, as packet_tpage_add).  Advances the
   cursor by the packet's 0x18 bytes.  No callee.

   Shapes: the colour store is written second, right after the draw-mode
   store and before x / y / u / v (22/38 with it after v0, where the target
   emits it: sched1 then has no non-memory insn to pick between the x0
   store and the draw-mode store, and the five stack loads go above both);
   the entry address is integer arithmetic with the product first, as in
   packet_tpage_add (29/38 as `g_PacketOtBase + z`, the form packet_sprite8_add has). */
/* func_80025754 */
void packet_sprite_col_add(s32 x, s32 y, s32 u, s32 v, s32 w, s32 h, s32 tpage, s32 clut, s32 z, s32 color)
{
    PacketSprite *p;
    u32 *ot;

    p = (PacketSprite *)g_PacketWritePointer;
    AS(u_long, p->mode) = tpage | 0xE1000200;
    *(u32 *)&AS(u_char, p->r0) = color | 0x64000000;
    AS(short, p->x0) = x;
    AS(short, p->y0) = y;
    AS(u_char, p->u0) = u;
    AS(u_char, p->v0) = v;
    ot = (u32 *)((z << 2) + (s32)g_PacketOtBase);
    AS(short, p->w) = w;
    AS(short, p->h) = h;
    AS(u_short, p->clut) = clut;
    AS(u_long, p->tag) = *ot | 0x05000000;
    *ot = (u32)p + 0x80000000;
    g_PacketWritePointer = (u8 *)(p + 1);
}
void AddPrim(void *ot, void *p);

/* Draw an 8 x 8 textured sprite at (x, y) from texel (u, v) with palette
   `clut`, at ordering-table depth z, in the texture page that is current
   (no draw-mode word of its own, unlike packet_sprite_add / packet_sprite_col_add).
   Nothing calls it: the image has no jal to it and no reference to its
   address.

   Builds one libgpu SPRT_8 at the packet cursor g_PacketWritePointer: the tag
   0x03000000 (length 3, link clear), the command word 0x74808080 (GP0 74h
   with the neutral colour 0x808080), x0 / y0, u0 / v0 and clut.  Hands it
   to AddPrim (libgpu AddPrim) with entry z of the ordering table
   g_PacketOtBase, then advances the cursor by the packet's 0x10 bytes.

   Shapes: the cursor is stepped with `p++` as its own statement, as in
   profile_draw (26/30 as `g_PacketWritePointer = (u8 *)(p + 1)`: with one
   assignment to p its load is a birthing insn for sched1 and goes below
   the two stack loads); the entry address is pointer arithmetic, base
   first. */
/* func_800257EC */
void packet_sprite8_add(s32 x, s32 y, s32 u, s32 v, s32 clut, s32 z)
{
    SPRT_8 *p;

    p = (SPRT_8 *)g_PacketWritePointer;
    p->tag = 0x03000000;
    *(u32 *)&p->r0 = 0x74808080;
    p->x0 = x;
    p->y0 = y;
    p->u0 = u;
    p->v0 = v;
    p->clut = clut;
    AddPrim(g_PacketOtBase + z, p);
    p++;
    g_PacketWritePointer = (u8 *)p;
}
