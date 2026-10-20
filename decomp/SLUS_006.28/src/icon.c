/* Lunar SSSC — icon: the icons on the 40 x 28 cell grid: their 20 slots,
   garbage collection, image loads and cells. The name is the developers'
   own: ICON.C, the source file IconCellSet's assertion names ('Garbage
   Error!', line 0x132); the file's other functions have it by being of its
   group. The first split had it in rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
extern long HWD0;
extern long VWD0;
extern u8 g_IconSlots[];
extern s32 g_IconOffsets;
extern s32 g_IconBase;
s32 IconSlotReset(void);
void IconInit(s32 icon_data);
s32 IconSlotFindFree(void);
s32 IconSlotFind(s32 icon);
s32 IconSlotReset();
void IconInit(s32 icon_data);
s32 lz_unpack();
s32 lz_unpack();
extern long HWD0, VWD0;
s32 lz_unpack();
s32 DrawSync(s32);
void IconCellDraw(s32 column, s32 row, s32 cell);
extern long HWD0, VWD0;
s32 IconSlotFindFree(void);
s32 IconSlotFind(s32 icon);
s32 ankprint_cell_get(s32, s32);
extern long HWD0, VWD0;

/* func_8001ABA4 */
s32 IconSlotReset(void)
{
  u8 *last_slot_entry;
  u8 *slots;
  s32 slot;
  u8 *slot_entry;
  slots = g_IconSlots;
  slot = 0x13;
  slot_entry = (last_slot_entry = slots + 0x98);
  do
  {
    ((IconSlot *)slot_entry)->icon = 0;
    slot -= 1;
    slot_entry -= 8;
  }
  while (slot >= 0);
  return (s32) slot_entry;
}

s32 IconSlotReset();                                  /* extern */

/* func_8001ABCC */
void IconInit(s32 icon_data) {
    g_IconOffsets = icon_data;
    g_IconBase = icon_data;
    IconSlotReset();
}

/* func_8001ABF4 */
s32 IconSlotFindFree(void)
{
  s32 slot;
  u8 *slot_entry;
  for (slot = 0, slot_entry = g_IconSlots; slot < 0x14; slot++, slot_entry += 8)
  {
    if (((IconSlot *)slot_entry)->icon == 0)
    {
      return slot;
    }
  }
  return -1;
}
/* func_8001AC30 */
s32 IconSlotFind(s32 icon)
{
  s32 slot;
  u8 *slot_entry;
  for (slot = 0, slot_entry = g_IconSlots; slot < 0x14; slot++, slot_entry += 8)
  {
    if (((IconSlot *)slot_entry)->icon == icon)
    {
      return slot;
    }
  }
  return -1;
}
/* far_syms: g_IconSlots */
s32 ankprint_cell_get(s32, s32);

/* func_8001AC6C */
void IconGarbageCollect(void)
{
  u8 *p;
  u8 *base;
  s32 i;
  s32 y;
  s32 x;
  s32 c;
  u8 *slot;
  u32 cell;
  i = 0x13;
  base = g_IconSlots;
  p = base + 0x98;
  do
  {
    ((IconSlot *)p)->mark = 0;
    i--;
    p -= 8;
  }
  while (i >= 0);
  y = 0;
  while (y < 0x1C)
  {
    x = 0;
    while (x < 0x28)
    {
      c = ankprint_cell_get(x, y);
      if ((((c >> 8) & 7) == 3) && ((0x7F & c) < 0x14))
      {
        cell = (u32) (g_IconSlots + ((c & 0x7F) * 8));
        ((IconSlot *)cell)->mark = 1;
      }
      x++;
    }
    y++;
  }
  i = 0;
  slot = g_IconSlots;
next:
  if (AS(s16, ((IconSlot *)slot)->mark) == 0)
  {
    ((IconSlot *)slot)->icon = 0;
  }
  i++;
  slot += 8;
  if (i < 0x14)
    goto next;
}

/* far_syms: g_IconSlots (target reaches it %hi/%lo) */

/* func_8001AD60 */
void IconCellDraw(s32 column, s32 row, s32 cell)
{
  s32 color;
  int x;
  u8 *base;
  u8 *ptr;
  s16 val;
  s16 val0;
  s16 val2;
  if (cell & 0x80)
  {
    color = 0x404040;
  }
  else
  {
    color = 0x808080;
  }
  cell &= 0x7F;
  base = g_IconSlots;
  x = column * 8;
  ptr = base + (cell * 8);
  val = ((IconSlot *)ptr)->icon;
  if (val != 0)
  {
    val0 = ((IconSlot *)ptr)->texel_x;
    val2 = ((IconSlot *)ptr)->texel_y;
    packet_sprite_col_add(x, row * 8, val0, val2, 0x18, 0x18, 0x92, 0x7D80, 0x20, color);
  }
}
extern RECT g_IconImageRectangle[];
extern u8 g_IconImageBuffer[];

/* func_8001ADEC */
void IconImageLoad(s32 slot, s32 icon)
{
  RECT r;
  u8 *rec;
  s32 x;
  s32 y;
  r = g_IconImageRectangle[0];
  rec = g_IconSlots + slot * 8;
  lz_unpack(g_IconBase + *(u16 *) (g_IconOffsets + icon * 2), g_IconImageBuffer);
  x = (slot % 10) * 24;
  y = (slot / 10) * 24;
  r.x = (x >> 1) + 0x80;
  r.y = y + 0x1C0;
  LoadImage(&r, (u_long *) g_IconImageBuffer);
  DrawSync(0);
  ((IconSlot *)rec)->icon = icon;
  *(u16 *) rec = x;
  ((IconSlot *)rec)->texel_y = y + 0xC0;
}
extern u8 D_80011060[];
extern u8 D_800B8594[];

/* Put ICON number `icon` on the 40 x 28 cell grid at cell (x, y): the icon
   draw every menu and the battle HUD use. ("Garbage" was the splitter's
   name, taken from this function's assert string; the routine is the icon
   cache's front door and the garbage collection is one step of it.)

     icon == 0:  ankprint_cell_set(x, y, 0) -- the cell is cleared -- return 0.

     slot = IconSlotFind(icon)        the cache slot already holding it
     if none (-1):
       slot = IconSlotFindFree()          a free slot (record +4 == 0)
       if none:
         IconGarbageCollect()            collect: free every slot that no
                                       cell of the grid still refers to
         slot = IconSlotFindFree()
         if still none:
           assert_fail("Garbage Error!", "ICON.C", 0x132)
                                       D_80011060 / D_800B8594, line 306:
                                       the assert. It is an empty function
                                       in this build, so the code goes on
                                       with slot == -1.
       IconImageLoad(slot, icon)    unpack the icon's 24 x 24 8bpp
                                       pixels (lz_unpack from
                                       the icon block g_IconOffsets /
                                       g_IconBase into g_IconImageBuffer),
                                       LoadImage them to VRAM (0x80 +
                                       12 * (slot % 10), 0x1C0 + 24 *
                                       (slot / 10)), DrawSync(0), and fill
                                       the slot's record in g_IconSlots.
     ankprint_cell_set(x, y, (attr ? 0x80 : 0) | 0x300 | slot)

   The cell word is: bits 8..10 = 3, the cell type ankprint_cell_draw hands to
   IconCellDraw (a 24 x 24 sprite from the slot's record, tpage 0x92,
   CLUT 0x7D80); bits 0..6 = the slot (0..19); bit 7 = dimmed (colour
   0x404040 for 0x808080). So `attr` is a flag: any non-zero value dims the
   icon -- which is why callers hand over things like `v ^ 0x20200000` or
   `flag << 24`.

   The cache is the 20 records of 8 bytes at g_IconSlots (+0 u, +2 v, +4
   icon number or 0 = free, +6 the collector's mark). A slot is found by
   icon number, so an icon on screen twice is unpacked once. Nothing is
   queued: on a miss the upload happens here, at once, and DrawSync(0)
   waits for the GPU to take it; on a hit only the cell is written. No
   VSync / frame-present wait on any path, no RNG draw.

   With slot == -1 (the assert's case) IconImageLoad would write the
   record BEFORE g_IconSlots and the cell would be 0xFFFFFFFF | ... -- the
   original has no guard after the assert.

   Returns 0 for icon 0; on the other path $v0 is whatever ankprint_cell_set
   leaves (it is a void function there: the store's address). CellIconSet
   (cell_text.c) passes the value on; nothing reads it.

   `r` is copied from g_IconImageRectangle -- the same {0x80, 0x1C0, 12, 24}
   IconImageLoad starts its LoadImage rectangle from -- and never used:
   the target makes the 8-byte copy to 0x10($sp) and nothing reads it.

   Shapes the bytes depend on (each measured):
   - the cell is spelled `dim | 0x300 | slot` (68 -> 70). fold() moves the
     constant of `(dim | 0x300) | slot` onto the other operand, giving
     ori $a2, slot, 0x300 / or $a2, dim, $a2. Spelled with the constant
     beside slot -- `dim | (slot | 0x300)`, `slot | 0x300 | dim` -- the
     same rule moves it the other way: ori $a2, dim, 0x300 / or $a2, $a2,
     slot.
   - `dim = 0` stands before the icon test (64 -> 70): the cleared cell's
     0 and the returned 0 are then both dim's register ($s2). Set after
     the test, both are $zero and the early return loses its jump. */
/* func_8001AF04 */
s32 IconCellSet(s32 x, s32 y, s32 icon, s32 attr)
{
  RECT r;
  s32 slot;
  s32 dim;

  r = g_IconImageRectangle[0];
  dim = 0;
  if (icon == 0)
  {
    ankprint_cell_set(x, y, 0);
    return 0;
  }
  slot = IconSlotFind(icon);
  if (slot < 0)
  {
    slot = IconSlotFindFree();
    if (slot < 0)
    {
      IconGarbageCollect();
      slot = IconSlotFindFree();
      if (slot < 0)
      {
        assert_fail(D_80011060, D_800B8594, 0x132);
      }
    }
    IconImageLoad(slot, icon);
  }
  if (attr)
  {
    dim = 0x80;
  }
  return ankprint_cell_set(x, y, dim | 0x300 | slot);
}
