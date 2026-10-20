/* Lunar SSSC — packet_shape: the packet primitives in one colour: boxes,
   four-colour boxes, triangles, lines and the draw area, each taken from
   the packet pointer and added to the ordering table. The first split had
   it in rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_PacketWritePointer g_PacketOtBase */

#include "include_asm.h"
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
extern u32 *g_PacketOtBase;
extern u8 *g_PacketWritePointer;
extern long HWD0;
extern long VWD0;
/* far_syms: g_PacketWritePointer (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
/* far_syms: g_PacketOtBase (scratchpad cell the symbol table names: lui/lw to load, lui $at/sw to store) */
void packet_box_add(s32 left_x, s32 top_y, s32 width, s32 height, s32 col, s32 z);
void *packet_box_take();
void *packet_box_col4_take();
void packet_box_col4_add(s32 left_x, s16 top_y, s32 width, s32 height, s32 top_left_col, s32 top_right_col, s32 bottom_left_col, s32 bottom_right_col, s32 z);
void *packet_line_take();
void packet_line_add(s32 start_x, s32 start_y, s32 end_x, s32 end_y, s32 start_col, s32 end_col, s32 z);
void packet_box_add(s32, s32, s32, s32, s32, s32);
extern long HWD0, VWD0;

/* far_syms: g_PacketWritePointer (target reaches it %hi/%lo: lui/lw to load, lui $at/sw to store) */

/* func_800159E0 */
void *packet_box_take(void)
{
    u8 *p = g_PacketWritePointer;
    g_PacketWritePointer = p + 0x18;
    return p;
}

/* func_80015A00 */
void *packet_box_col4_take(void)
{
    u8 *p = g_PacketWritePointer;
    g_PacketWritePointer = p + 0x24;
    return p;
}

/* func_80015A20 */
void *packet_triangle_take(void)
{
    u8 *p = g_PacketWritePointer;
    g_PacketWritePointer = p + 0x14;
    return p;
}

/* func_80015A40 */
void *packet_line_take(void)
{
    u8 *p = g_PacketWritePointer;
    g_PacketWritePointer = p + 0x14;
    return p;
}
void *packet_box_take();

/* func_80015A60 */
void packet_box_add(s32 left_x, s32 top_y, s32 width, s32 height, s32 col, s32 z)
{
  void *packet;
  s16 bottom_y;
  s16 *corner_coordinate;
  long kept_width;
  packet = packet_box_take();
  kept_width = width;
  ;
  *(corner_coordinate = &((POLY_F4 *)packet)->y0) = (s16) top_y;
  corner_coordinate = &((POLY_F4 *)packet)->x2;
  ((POLY_F4 *)packet)->y1 = (s16) top_y;
  bottom_y = top_y + height;
  ((POLY_F4 *)packet)->tag = 0x05000000;
  ((POLY_F4 *)packet)->x0 = (s16) left_x;
  ((POLY_F4 *)packet)->x1 = left_x + kept_width;
  *corner_coordinate = (s16) left_x;
  ((POLY_F4 *)packet)->y2 = bottom_y;
  ((POLY_F4 *)packet)->x3 = left_x + kept_width;
  ((POLY_F4 *)packet)->y3 = bottom_y;
  /* the colour and the code as one word, a pointer access through r0's
     address: as AS(s32, packet->r0), a member access, GCC's code differs */
  *(s32 *)&((POLY_F4 *)packet)->r0 = col | 0x28000000;
  AddPrim((*((s32 *) 0x1F800040)) + (z * 4), packet);
}

void packet_triangle_down_add(s32 x, s32 y, s32 w, s32 h, s32 color, s32 ot);

void *packet_box_col4_take();                              /* extern */

/* func_80015B14 */
void packet_box_col4_add(s32 left_x, s16 top_y, s32 width, s32 height, s32 top_left_col, s32 top_right_col, s32 bottom_left_col, s32 bottom_right_col, s32 z) {
    u16 bottom_y;
    s32 right_x;
    void *packet;

    packet = packet_box_col4_take();
    right_x = left_x + width;
    ((POLY_G4 *)packet)->y0 = top_y;
    ((POLY_G4 *)packet)->y1 = top_y;
    bottom_y = top_y + height;
    ((POLY_G4 *)packet)->tag = 0x08000000;
    ((POLY_G4 *)packet)->x0 = left_x;
    ((POLY_G4 *)packet)->x1 = right_x;
    ((POLY_G4 *)packet)->x2 = left_x;
    ((POLY_G4 *)packet)->y2 = bottom_y;
    ((POLY_G4 *)packet)->x3 = right_x;
    ((POLY_G4 *)packet)->y3 = bottom_y;
    /* the four colour words (the first with the code), each a pointer access
       through its r field's address: as AS(s32, packet->r0), a member
       access, GCC's code differs */
    *(s32 *)&((POLY_G4 *)packet)->r0 = (s32) (top_left_col | 0x38000000);
    *(s32 *)&((POLY_G4 *)packet)->r1 = top_right_col;
    *(s32 *)&((POLY_G4 *)packet)->r2 = bottom_left_col;
    *(s32 *)&((POLY_G4 *)packet)->r3 = bottom_right_col;
    AddPrim(*(s32 *)0x1F800040 + (z * 4), packet);
}
/* Queue one flat triangle (libgpu's POLY_F3) with a horizontal top edge:
       (x, y) - (x + 2w, y) - (x + w, y + h)
   i.e. 2w wide, its third corner h below the middle of that edge.  The
   message window's "more" arrow MessageArrowDraw is its only caller (w 4, h 5,
   colour 0x20F0F0, ordering-table cell 0x10).  Nothing is returned (the
   caller reads no $v0).

   The packet is the 0x14-byte one packet_triangle_take takes off the primitive
   buffer (it moves the cursor g_PacketWritePointer on 0x14 = sizeof(POLY_F3)), and it
   is linked at ordering-table cell g_PacketOtBase[ot] with AddPrim
   .  No other state is read or written; no RNG draw, no frame wait.

   Every header byte is written twice.  The length byte 4 and the code byte
   0x20 go in first (libgpu.h's setPolyF3(p), written out below as the tree
   writes it in libgpu_p12.c); then the whole tag word 0x04000000 (length 4,
   a null link AddPrim replaces) and the whole colour word color | 0x20000000
   are stored over them.  The colour is 0x00BBGGRR; the OR means a caller's
   top byte would be merged into the command byte (bit 25 = semi-transparent),
   and the one caller passes a clean 24-bit colour.  The target has all four
   stores (sb +3, sb +7, sw +0, sw +4).

   What the bytes pin (each changed alone in this text):
   * the six vertex stores are in libgpu.h's setXY3 order, vertex by vertex
     (`setXY3(p, x, y, x + w + w, y, x + w, y + h)` itself is 48/48; by axis
     39/48; vertex 2 before vertex 1 35/48);
   * x1 is `x + w + w`, the sum x2 stores plus w again (`x + w * 2` 27/48,
     `w + x + w` 47/48), and y2 is `y + h` (`h + y` 34/48);
   * the tag word is stored before vertex 1.  This text has it straight after
     the two header bytes; it may also follow x0, or x0 and y0, or the colour
     word may sit between it and the vertices (all 48/48 -- retail cannot
     tell these apart).  After all six vertex stores it is 46/48 (the code
     byte's `sb` and the tag's `lui` sink below the y stores), ahead of the
     header bytes 45/48;
   * the header bytes are length first (code first 46/48), and the colour is
     one word store (three byte stores of the components 44/48).  */
/* func_80015BF8 */
void packet_triangle_down_add(s32 x, s32 y, s32 w, s32 h, s32 color, s32 ot)
{
    POLY_F3 *p;

    p = packet_triangle_take();
    ((P_TAG *)p)->len = 4;
    ((P_TAG *)p)->code = 0x20;
    p->tag = 0x04000000;
    p->x0 = x;
    p->y0 = y;
    p->x1 = x + w + w;
    p->y1 = y;
    p->x2 = x + w;
    p->y2 = y + h;
    *(u32 *)&p->r0 = color | 0x20000000;
    AddPrim((s32)(g_PacketOtBase + ot), p);
}

/* One flat triangle with a vertical left edge, (x, y) - (x, y + 2h) -
   (x + w, y + h): packet_triangle_down_add's packet turned a quarter. A 0x14-byte
   POLY_F3 from packet_triangle_take, length 4 and code 0x20 stored as bytes
   (libgpu.h's setPolyF3), tag word 0x04000000, the three corners, the colour
   word `color | 0x20000000`, linked at ordering-table cell g_PacketOtBase[ot]
   with AddPrim. No other state, no RNG draw, no frame wait. */
/* func_80015CB8 */
void packet_triangle_right_add(s32 x, s32 y, s32 w, s32 h, s32 color, s32 ot)
{
    POLY_F3 *p;

    p = packet_triangle_take();
    ((P_TAG *)p)->len = 4;
    ((P_TAG *)p)->code = 0x20;
    p->tag = 0x04000000;
    p->x0 = x;
    p->y0 = y;
    p->x1 = x;
    p->y1 = y + h + h;
    p->x2 = x + w;
    p->y2 = y + h;
    *(u32 *)&p->r0 = color | 0x20000000;
    AddPrim((s32)(g_PacketOtBase + ot), p);
}

void *packet_line_take();

/* func_80015D78 */
void packet_line_add(s32 start_x, s32 start_y, s32 end_x, s32 end_y, s32 start_col, s32 end_col, s32 z)
{
  s16 *point_coordinate;
  void *packet;
  packet = packet_line_take();
  point_coordinate = &((LINE_G2 *)packet)->x0;
  ((LINE_G2 *)packet)->tag = 0x04000000;
  *point_coordinate = start_x;
  ((LINE_G2 *)packet)->y0 = start_y;
  ((LINE_G2 *)packet)->x1 = end_x;
  point_coordinate = &((LINE_G2 *)packet)->y1;
  *point_coordinate = end_y;
  /* the two colour words (the first with the code), each a pointer access
     through its r field's address: as AS(s32, packet->r0), a member access,
     GCC's code differs */
  *(s32 *)&((LINE_G2 *)packet)->r0 = (s32) (start_col | 0x50000000);
  *(s32 *)&((LINE_G2 *)packet)->r1 = end_col;
  AddPrim((*((s32 *) 0x1F800040)) + (z * 4), packet);
}
extern long HWD0, VWD0;		 

/* func_80015E24 */
void packet_draw_area_add(RECT *rect)
{
    u8 *p;

    p = g_PacketWritePointer;
    g_PacketWritePointer = p + 0xC;
    if (rect->h >= 0xE0) {
        rect->h = 0xE0;
    }
    SetDrawArea(p, rect);
    AddPrim((s32)g_PacketOtBase + 0xFFC, p);
}
