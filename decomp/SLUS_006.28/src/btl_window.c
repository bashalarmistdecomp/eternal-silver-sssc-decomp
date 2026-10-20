/* Lunar SSSC — btl_window: the battle's two windows and what stands in
   them: window 1's icon, the action's string, the acting slot's face, the
   text grid's cells cleared and set, the boxes off, and the windows' set-
   up, sizes, moves and sprites. The first split had it in misc.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlRecords */
/* far_syms: CharStats */
/* far_syms: g_BtlSlotIds */
/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuItems (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuItemCount (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuItems */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "mem.h"
#include "btl.h"
#include "sprite.h"
/* far_syms: g_BtlActingSlot (carried from the first split's misc.c) */
/* far_syms: g_BtlActionKind (carried from the first split's misc.c) */
/* far_syms: g_BtlActionNumber (carried from the first split's misc.c) */

extern s32 CharStats;
extern u8 g_BtlActingSlot;
s32 BtlWindowsMoveWait();                                  /* extern */
s32 BtlMenuTextImageLoad(s32, s32, s32);                       /* extern */
s32 BtlFrameStep();                                  /* extern */
extern void *g_BtlMenuItems;
s32 ankprint_rows_clear();                                  /* extern */
extern u8 g_BtlSlotIds[];
/* The party HUD's per-member record BtlPartyPanelDisp builds on the stack and
   BtlPartyPanelMemberDisp draws: a used flag, three status bytes (CharStats +0x1E & 4,
   +0x15, +0x14) and the hp / max hp / mp / max mp words (CharStats +0..+6). */
extern u8 g_BtlPartyNotHeldFlags[];
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlWindowRectangles[];
extern u8 g_BtlWindowOutlines[];
extern s32 g_SpriteList;
s32 BtlWindow0Set(s32, s32, s32, s32);
s32 BtlMenuIconCellsSet(s32, s32);

s32 BtlWindowSet(s32, s32, s32, s32, s32);
extern u8 g_BtlMenuItemCount;  /* TU declares this below */

/* func_80049740 */
void BtlWindow1IconDisp(void)
{
  BtlWindowSet(1, 0x21, 1, 5, 5);
  BtlWindowsMoveWait();
  ((BtlMenuItem *)g_BtlMenuItems)->on = 1;
  ((BtlMenuItem *)g_BtlMenuItems)->disabled = 0;
  g_BtlMenuItemCount = 1;
  ((BtlMenuItem *)g_BtlMenuItems)->attr = 0x20200000;
  ((BtlMenuItem *)g_BtlMenuItems)->icon = 0xED;
  ((BtlMenuItem *)g_BtlMenuItems)->x = 0x22;
  ((BtlMenuItem *)g_BtlMenuItems)->y = 2;
  BtlMenuIconCellsSet(0, 3);
}

/* far_syms: g_BtlWindowMoves (target reaches it %hi/%lo) */
extern u8 g_BtlWindowMoves[];
s32 BtlCellRowsUpdate(s32, s32, s32, s32);
s32 BtlCellAreaClear(s32 column, s32 row, s32 unused, s32 width, s32 a4, s32 a5);  /* TU defines this below */
s32 BtlWindowSet(s32, s32, s32, s32, s32);
s32 BtlShowFrameStep();

/* func_800497D4 */
void BtlWindow1IconClose(void)
{
  /* CODEGEN FORCING DEVICE (address of a global in a local), not recovered source:
     `window_moves` only holds the address of `g_BtlWindowMoves`, through the type `u8
     (*)[]`; behaviour is the same with the address written where it is used, but then 6
     of this function's 41 instruction lines differ. */
  u8 (*window_moves)[];
  /* CODEGEN FORCING DEVICE (copy of a copy), not recovered source:
     `loop_window_moves` only copies `window_moves`, which is not assigned again afterwards; behaviour
     is the same with `window_moves` used directly, but then 3 of this function's 41
     instruction lines differ. */
  u8 (*loop_window_moves)[];
  BtlCellAreaClear((s16) ((BtlMenuItem *)g_BtlMenuItems)->x, (s16) AS(s32, ((BtlMenuItem *)g_BtlMenuItems)->y), 0x200900, 3, 3, 0x10);
  BtlCellRowsUpdate(2, 0x80, 3, 0x10);
  BtlWindowSet(1, 0x21, 1, 0, 0);
  window_moves = &g_BtlWindowMoves;
  if (((BtlWindowMove *)window_moves)[1].moving != 0)
  {
    loop_window_moves = window_moves;
    do
    {
      BtlShowFrameStep();
    }
    while (((BtlWindowMove *)loop_window_moves)[1].moving != 0);
  }
}

extern s32 g_GameOptions;
extern u8 g_BtlActionKind;
extern u8 g_BtlActionNumber;

/* func_8004987C */
void BtlActionStringDisp(void)
{
    u8 *ptr;

    ptr = (u8 *)g_GameOptions;
    if (ptr[6] == 0)
        return;
    if (g_BtlActionKind == 3) {
        BtlMenuTextStringPut(0, g_BtlActionNumber + 0x1B6, 0, 8);
    } else if (g_BtlActingSlot < 6) {
        if (g_BtlActionKind == 4) {
            BtlMenuTextStringPut(0, g_BtlActionNumber | 0x100, 0, 8);
        } else {
            return;
        }
    } else {
        if (g_BtlActionKind == 7) {
            BtlMenuTextStringPut(0, g_BtlActionNumber | 0x300, 0, 8);
        } else {
            return;
        }
    }
    BtlWindowSet(0, 2, 1, 0x12, 4);
    BtlWindowsMoveWait();
    BtlMenuTextImageLoad(0, 0, 8);
    BtlMenuTextCellsSet(3, 2, 8, 1, 0x80300000, 0);
    BtlCellRowsUpdate(2, 0x80, 2, 0x20);
}

s32 BtlCellRowsUpdate(s32, s32, s32, s32);                        /* extern */
s32 BtlCellAreaClear(s32, s32, s32, s32, s32, s32);              /* extern */
s32 BtlWindowSet(s32, s32, s32, s32, s32);                   /* extern */

/* func_8004999C */
void BtlActionStringClose(void) {
    BtlCellAreaClear(3, 2, 0xCC00, 0x10, 2, 0x20);
    BtlCellRowsUpdate(2, 0x80, 2, 0x20);
    BtlWindowSet(0, 2, 1, 0, 0);
    BtlWindowsMoveWait();
}

/* The party mask BtlMenuReset and BtlMenuFirstOpen store in g_BtlMenuPendingMask: one bit
   per slot 0..4, set unless g_BtlSlotActiveFlags[slot] is 0, the slot's CharStats status
   (+0x15) has 0x04 or 0x08 (read as the +0x14 word & 0xC00), both of 0x10/0x20,
   or 0x40, or g_BtlPartyNotHeldFlags[slot] is 0. CharStats is read inside the loop: loop.c
   then hoists the invariants in the order the body first uses them. */
/* func_80049A0C */
s32 BtlMenuPendingMaskGet(void)
{
  u16 i;
  u8 mask;

  for (i = 0, mask = 0; i < 5; i++)
  {
    if ((g_BtlSlotActiveFlags[i] == 0) || (*(u16 *) (i * 0x3C + CharStats + 0x14) & 0xC00) || ((*(u8 *) (i * 0x3C + CharStats + 0x15) & 0x30) == 0x30) || (*(u8 *) (i * 0x3C + CharStats + 0x15) & 0x40) || (g_BtlPartyNotHeldFlags[i] == 0))
    {
      mask |= 1 << i;
    }
  }
  mask ^= 0x1F;
  return mask;
}

extern MemRequest g_MemRequest;
extern s32 g_PaletteBuf;

/* Called once at the top of BtlMenuOpen. Copies the 16-bit word at
   g_PaletteBuf + 0xFC0 to g_PaletteBuf + 0x202 (g_PaletteBuf is the u16 colour
   buffer base palette_buf_address_set sets to 0x801D5B80: entry 0x7E0 -> entry
   0x101), posts a mode-2 request {0x102C00, 0x25E74000, 0x20, 0x20} in
   g_MemRequest, then runs BtlWindowsMoveWait (wait for the battle windows to
   settle).

   g_MemRequest must be declared as the struct it is: the target forms the
   block's address once (lui $a0 / addiu $v0,$a0,%lo), stores the four
   words through $v0 and the mode byte through %lo($a0). A far scalar plus
   an (s32 *) alias of its address stores the byte through $at instead and
   materialises the address a second time (14/23). */
/* Ruling of 2026-10-14 (docs/naming.md): Reached, and does work: three things that nothing read gives one purpose. */
void func_80049AC4(void) {
    u16 *p = (u16 *)g_PaletteBuf;

    p[0x101] = p[0x7E0];
    g_MemRequest.source_fill.fill = 0x102C00;
    g_MemRequest.destination = 0x25E74000;
    g_MemRequest.row_words = 0x20;
    g_MemRequest.rows = 0x20;
    g_MemRequest.kind = 2;
    BtlWindowsMoveWait();
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its one call passes nothing, and nothing read says what it was for. */
void func_80049B20() {

}

/* The battle windows. g_SpriteList + 0x900 holds a 0x48-byte record per
   window (BtlWindowSprites): two 0x24-byte halves (SpriteEntry) copied from the templates
   g_BtlWindowOutlines (first half) and g_BtlWindowBoxes (second half; g_BtlWindowBoxes[1]
   doubles as window 0's alternate first half). Each half's first u16 is its
   "on" flag. The frames are 24-byte quads (SpritePiece): two corner quads per
   window in g_BtlWindowOutlinePieces, one per window in g_BtlWindowBoxPieces. g_BtlWindowRectangles is a
   14-byte rectangle record per window (BtlWindowRectangle); g_BtlWindowMoves a 0x1C-byte
   motion state per window (BtlWindowMove) whose first byte is also the mode
   BtlWindowSpritesSet tests (1, 2). One typedef per record: every function below
   uses these (the tables themselves are still declared u8[] where the TU
   had them so, and reached through a cast). */

/* A 24-byte window quad (g_BtlWindowBoxPieces, g_BtlWindowOutlinePieces). Its first eight bytes
   are the four halfwords BtlWindowsInit stores; a POLY_F4 reading
   (tag, r0 g0 b0, code) does not fit those stores. */
/* One per window (g_BtlWindowOutlinePoseRows[2], small data): the g_BtlWindowOutlines row's +0x10. */
/* A 0x24-byte window block: a g_BtlWindowBoxes / g_BtlWindowOutlines row and each half of
   a BtlWindowSprites. As BtlWindowsInit writes it: +0x04 is 0x500 on the g_BtlWindowBoxes
   rows and 0x1000 on the g_BtlWindowOutlines rows, +0x08 is 0x8000 on the
   g_BtlWindowBoxes rows, +0x10 points at the row's g_BtlWindowOutlinePoseRows pair (g_BtlWindowOutlines
   rows only) and +0x14 at the row's quad(s). 4-aligned, so a whole-block
   assignment is GCC's word copy. */
/* The 14-byte window rectangle record BtlWindowSet opens (g_BtlWindowRectangles). */
/* The 0x1C-byte motion state per battle window (g_BtlWindowMoves[2]): the frame
   rectangle (x0,y0)-(x1,y1) that BtlWindowSpritesSet draws, its target
   rectangle, and a per-step velocity (8.8 fixed point) for the right and
   bottom edges. */
extern SpriteEntry g_BtlWindowBoxes[];

/* Ruling of 2026-10-14 (docs/naming.md): Does what BtlWindowBoxesOff does, its two stores in the other order; nothing read says why there are two. */
/* func_80049B28 */
void BtlWindowBoxesOffCopy(void) {
    g_BtlWindowBoxes[1].on = 0;
    g_BtlWindowBoxes[0].on = 0;
}

/* func_80049B3C */
void BtlWindowBoxesOff(void) {
    g_BtlWindowBoxes[0].on = 0;
    g_BtlWindowBoxes[1].on = 0;
}

extern u8 g_BtlMenuItemCount;  
/* func_80049B50 */
void BtlBlackLinesAdd(s32 base_end_x, s32 offsets) {
    s32 y;

    y = 0;
    do {
        packet_black_line_add(base_end_x + *(s8 *)offsets, y++);
        offsets++;
    } while (y < 0xE0);
}

/* far_syms: g_BtlRunning (target reaches it %hi/%lo) */
/* far_syms: g_BtlRunning (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 g_BtlRunning;

/* func_80049BA8 */
void BtlBlackLinesRun(void)
{
    s32 i;
    s8 buf[0xE0];

    for (i = 0; i < 0xE0; i++)
    {
        buf[i] = rng_below(0x40);
    }

    g_BtlRunning = 0;
    layer_col_set(0, 0x13C, 0);
    g_BtlRunning = 1;
    BtlBlackLinesAdd(0x140, buf);
    BtlFrameStep();
    i = 0x140;
    screen_disp_mask_set(1);
    do
    {
        BtlBlackLinesAdd(i, buf);
        BtlFrameStep();
        i -= 0x18;
    } while (i >= -0x40);
}

/* no_schedule_insns: func_80049C50 */
s32 xmalloc_heap_free(s32);
s32 xmalloc_heap_alloc(s32);

/* Ruling of 2026-10-14 (docs/naming.md): Nothing reaches it, and the numbers it works on are used by nothing: nothing supports a name. */
void func_80049C50(void)
{
  u16 *p;
  s32 v;
  s32 r;
  s32 i;
  s32 j;
  p = xmalloc_heap_alloc(0x380);
  MapFrameEndCopy();
  layer_col_set(0, 0x13C, 0);
  for (i = 0; i < 0xE0; i++)
  {
    p[i << 1] = 0x140 - rng_mask_below(0x1F, 0x1F);
    p[(i << 1) + 1] = 0x140;
  }

  for (i = 0; i < 0x10; i++)
  {
    for (j = 0; j < 0xE0; j++)
    {
      v = p[j << 1] - 0x14;
      r = 0;
      if (v >= 0)
      {
        r = v;
      }
      p[j << 1] = r;
    }

    BtlFrameStep();
  }

  xmalloc_heap_free(p);
}

/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 g_BtlRecords;

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_80049D2C */
void BtlRecords14Step(void) {
    BtlAnimationStep(g_BtlRecords, 0, 0x14);
    BtlSlot5RoamStep();
}

/* func_80049D60 */
s32 BtlMenuActingFaceSet(s32 column, s32 row)
{
	return FaceCellSet(column, row, (g_BtlSlotIds[g_BtlActingSlot] - 1) * 5 + 1);
}

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_80049DA8 */
void BtlCellAllClear(s32 kind_mask) {
    ankprint_rows_clear(0, 0x20, kind_mask);
}

/* A pass-through thunk to ankprint_rows_clear (clear cell rows). */
/* func_80049DD0 */
void BtlCellRowsClear(s32 first_row, s32 row_count, s32 kind_mask) {
    ankprint_rows_clear(first_row, row_count, kind_mask);
}

s32 font_cell_run_set(s32, s32, s32, s32);

/* func_80049DF0 */
void BtlMenuTextCellsSet(s32 column, s32 first_row, s32 row_block_count, s32 text_row_count, s32 pad4_0, u16 first_place)
{
  s32 text_row;
  s32 row;
  s32 place;
  text_row = 0;
  if (text_row_count > text_row)
  {
    place = first_place & 0xFFFF;
    row = first_row;
    do
    {
      font_cell_run_set(column, row, row_block_count, place);
      place += row_block_count;
      text_row += 1;
      row += 2;
    }
    while (text_row < text_row_count);
  }
}

/* func_80049E7C */
s32 BtlCellAreaClear(s32 column, s32 row, s32 unused, s32 width, s32 a4, s32 a5) {
    return ankprint_area_clear(column, row, width, a4, a5);
}

/* Puts a 3 x 3 icon into a grid of 32-bit cells the caller hands in.
   Nothing in the image calls it (no jal, no j, no data word, no address
   formed), so the grid it was written for is not identified: by the bytes
   it is 64 cells (0x100 bytes) a row, which is not the 40 x 28 grid of u16
   cells at g_AnkprintGrid that the live draw writes.

   (x, y, cells, icon, attr): the block starts at cells[y * 64 + x].  The
   first cell is (icon * 18 + 0x200900) | attr.  With icon != 0 every cell
   after it is 2 on from the one before, counted on across the three rows
   (+0, +2, +4 / +6, +8, +10 / +12, +14, +16: an icon is nine cell values
   two apart, 18 values an icon).  With icon == 0 all nine cells get
   0x200900 | attr.  attr is ORed in whole, once, before the stepping.

   Reads nothing but its arguments, writes only the nine cells, calls
   nothing, no RNG draw, no frame wait, no return value.

   The argument list is the one cell_text.c's CellIconSet has -- (x, y, an
   argument it drops, icon, attr) -> IconCellSet(x, y, icon, attr), the live
   icon draw (icon.c).  0x200900, the icon-0 cell here, is also the
   third argument the two 3 x 3 clears still pass and their thunks drop:
   BtlWindow1IconClose's BtlCellAreaClear(x, y, 0x200900, 3, 3, 0x10) just above
   and ShopListDisp's BtlCellAreaClearCopy(x, y, 0x200900, 3, 3, 0x10).

   Signature from the body alone: x, y and icon are words (no mask, no
   extension), attr is a word read from the fifth argument's slot, cells is
   scaled by 4.  u32 or s32 cells give the same bytes.

   Shape: the store is indexed, cells[j].  The built code then walks a
   pointer of its own and still counts j up (slti 3), while the outer
   counter, which nothing reads, comes out counting 2..0 (bgez) whichever
   way it is written.  Walked as a pointer in the source the inner counter
   comes out counting down as well (15/27).  y * 64 comes first in the
   offset (24/27 the other way) and attr last in the OR (19/27).  The
   compiler pass behind the counter directions was not checked in a dump. */
/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_80049EAC */
void IconSquareSet(s32 x, s32 y, u32 *cells, s32 icon, s32 attr)
{
    s32 val;
    s32 i;
    s32 j;

    cells += y * 64 + x;
    val = (icon * 18 + 0x200900) | attr;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            cells[j] = val;
            if (icon != 0) {
                val += 2;
            }
        }
        cells += 64;
    }
}

/* func_80049F18 */
void BtlRecordClear(void *record) {
    if (record != 0) {
        ((BtlRecord *)record)->sprite.on = 0;
        ((BtlRecord *)record)->on = 0;
        ((BtlRecord *)record)->sprite.piece_table = 0;
        ((BtlRecord *)record)->sprite.pose_table = 0;
        ((BtlRecord *)record)->animation_table = 0;
        ((BtlRecord *)record)->step_list = 0;
    }
}

extern SpritePiece g_BtlWindowOutlinePieces[];
extern SpritePiece g_BtlWindowBoxPieces[];

/* la_absolute: BtlWindowsInit */
extern SpritePoseRow g_BtlWindowOutlinePoseRows[2];

/* Battle-window init (BtlMenuInit calls it once per battle UI setup):
   clears the "on" word of the first two 0x24-byte blocks at
   g_SpriteList + 0x900 (window 0's two halves) and resets every per-window
   table. For each of the two windows: the motion state's on/moving bytes
   and the whole rectangle record are zeroed; the g_BtlWindowBoxes row becomes
   {on 0, +4 0x500, +8 0x8000, quad &g_BtlWindowBoxPieces[i]} and its quad
   {kind 0x4000, +6 0x3E4, corners 0}; the g_BtlWindowOutlines row becomes
   {on 0, +4 0x1000, pair &g_BtlWindowOutlinePoseRows[i], quads &g_BtlWindowOutlinePieces[i * 2]} and its
   pair {2, 0}. Then the four corner quads get kind 0x5000 and zero corners,
   and +6 is 0x3E6 on each window's first quad, 0x3E4 on its second.
   The runs of zero stores are chained assignments: a chain forms its
   addresses left to right and stores right to left, which is what puts
   g_BtlWindowMoves's address ahead of g_BtlWindowRectangles's among the hoisted ones while
   g_BtlWindowRectangles's bytes are stored first. `pp` is assigned inside the loop: the
   table address is then formed before the index shift (la; sll; addu) and
   is not worth hoisting to loop.c. */
/* func_80049F40 */
void BtlWindowsInit(void)
{
    SpriteEntry *rec;
    SpritePoseRow *pp;
    u16 i;

    rec = (SpriteEntry *)(g_SpriteList + 0x900);
    for (i = 0; i < 2; i++) {
        rec[i].on = 0;
        ((BtlWindowMove *)g_BtlWindowMoves)[i].mode = ((BtlWindowMove *)g_BtlWindowMoves)[i].moving =
            ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].mode = ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].open = 0;
        ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].x = ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].y =
            ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].width_before = ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].height_before =
            ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].width = ((BtlWindowRectangle *)g_BtlWindowRectangles)[i].height = 0;
        g_BtlWindowBoxes[i].depth = 0x500;
        g_BtlWindowBoxes[i].attr = 0x8000;
        g_BtlWindowBoxes[i].on = 0;
        g_BtlWindowBoxes[i].x = 0;
        g_BtlWindowBoxes[i].y = 0;
        g_BtlWindowBoxes[i].flags = 0;
        g_BtlWindowBoxes[i].pose = 0;
        g_BtlWindowBoxes[i].texel_records = 0;
        g_BtlWindowBoxes[i].pose_table = 0;
        g_BtlWindowBoxes[i].scaled = 0;
        g_BtlWindowBoxes[i].piece_table = (s32)&g_BtlWindowBoxPieces[i];
        g_BtlWindowBoxPieces[i].kind = 0x4000;
        g_BtlWindowBoxPieces[i].width = g_BtlWindowBoxPieces[i].height = 0;
        g_BtlWindowBoxPieces[i].texel_col.col = 0x3E4;
        g_BtlWindowBoxPieces[i].x0 = g_BtlWindowBoxPieces[i].y0 =
            g_BtlWindowBoxPieces[i].x1 = g_BtlWindowBoxPieces[i].y1 =
            g_BtlWindowBoxPieces[i].x2 = g_BtlWindowBoxPieces[i].y2 =
            g_BtlWindowBoxPieces[i].x3 = g_BtlWindowBoxPieces[i].y3 = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].depth = 0x1000;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].on = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].x = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].y = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].attr = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].flags = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].pose = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].texel_records = 0;
        pp = g_BtlWindowOutlinePoseRows;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].pose_table = (s32)&pp[i];
        ((SpriteEntry *)g_BtlWindowOutlines)[i].scaled = 0;
        ((SpriteEntry *)g_BtlWindowOutlines)[i].piece_table = (s32)&g_BtlWindowOutlinePieces[i * 2];
        pp[i].count = 2;
        pp[i].first = 0;
    }
    for (i = 0; i < 4; i++) {
        g_BtlWindowOutlinePieces[i].kind = 0x5000;
        g_BtlWindowOutlinePieces[i].width = g_BtlWindowOutlinePieces[i].height = 0;
        g_BtlWindowOutlinePieces[i].x0 = g_BtlWindowOutlinePieces[i].y0 =
            g_BtlWindowOutlinePieces[i].x1 = g_BtlWindowOutlinePieces[i].y1 =
            g_BtlWindowOutlinePieces[i].x2 = g_BtlWindowOutlinePieces[i].y2 =
            g_BtlWindowOutlinePieces[i].x3 = g_BtlWindowOutlinePieces[i].y3 = 0;
    }
    g_BtlWindowOutlinePieces[2].texel_col.col = 0x3E6;
    g_BtlWindowOutlinePieces[0].texel_col.col = 0x3E6;
    g_BtlWindowOutlinePieces[3].texel_col.col = 0x3E4;
    g_BtlWindowOutlinePieces[1].texel_col.col = 0x3E4;
}

/* far_syms: g_BtlWindowRectangles (target reaches it %hi/%lo) */
/* far_syms: g_BtlWindowBoxes (target reaches it %hi/%lo) */
/* far_syms: g_BtlWindowOutlines (target reaches it %hi/%lo) */
void BtlWindowMoveStart();
s32 BtlMenuFrameStep();

/* func_8004A13C */
s32 BtlWindow0Set(s32 x, s32 y, s32 w, s32 h)
{
    u8 *p;
    u16 *q;

    q = (u16 *)g_BtlWindowRectangles;
    p = (u8 *)q;

    p[0] = 2;
    p[1] = 1;
    q[3] = q[5];
    q[4] = q[6];
    q[1] = x << 3;
    q[2] = y << 3;
    q[5] = w << 3;
    q[6] = h << 3;
    ((SpriteEntry *)g_BtlWindowBoxes)[0].on = 1;
    ((SpriteEntry *)g_BtlWindowOutlines)[0].on = 1;
    BtlWindowMoveStart(0);
    return BtlMenuFrameStep();
}

void BtlWindowMoveStart();
s32 BtlMenuFrameStep();

/* Opens battle window `id` (the cast-name window, the target window and the
   message windows all go through here): the 14-byte record at
   g_BtlWindowRectangles[id] gets state 2, the open flag, x/y in tiles (<<3), its old
   w/h saved, and the new w/h; the two 36-byte per-window rows at
   g_BtlWindowBoxes / g_BtlWindowOutlines get their first halfword set; then
   BtlWindowMoveStart and BtlMenuFrameStep run. `q` holds the table base first and
   is then reused as the record pointer: a base pointer that is assigned
   twice is not boosted by sched1, so the base
   address is formed first and the record pointer gets $v0. */
/* func_8004A1C0 */
s32 BtlWindowSet(s32 id, s32 x, s32 y, s32 w, s32 h)
{
    u8 *p;
    u16 *q;

    q = (u16 *)g_BtlWindowRectangles;
    p = (u8 *)q + id * 14;
    q = (u16 *)p;

    p[0] = 2;
    p[1] = 1;
    q[1] = x << 3;
    q[2] = y << 3;
    q[3] = q[5];
    q[4] = q[6];
    q[5] = w << 3;
    q[6] = h << 3;
    ((SpriteEntry *)g_BtlWindowBoxes)[id].on = 1;
    ((SpriteEntry *)g_BtlWindowOutlines)[id].on = 1;
    BtlWindowMoveStart(id);
    return BtlMenuFrameStep();
}

/* Starts battle window `idx`'s open / resize / close motion: seeds the
   motion state g_BtlWindowMoves[idx] (BtlWindowMove) that BtlWindowsMoveStep then steps
   once per battle frame, from the rectangle record g_BtlWindowRectangles[idx]
   (BtlWindowRectangle) its caller has just written. The two callers are the window
   openers: BtlWindow0Set (window 0, passes 0) and BtlWindowSet (passes its
   window `id`).

   A window whose motion is already running (moving != 0) is left alone:
   the record's new geometry is not picked up. Otherwise, in this order:
     on     = the record's state byte (2 as both openers write it; the mode
              BtlWindowSpritesSet tests);
     step   = 0;
     the start rectangle: (x0, y0) = (x, y), (x1, y1) = (x + ow, y + oh),
              the corner plus the PREVIOUS size;
     the target rectangle: (tx0, ty0) = (x, y), (tx1, ty1) = (x + w, y + h);
     vx, vy = (s16)(w - ow) << 5 and (s16)(h - oh) << 5: the change of size
              in 8.8 fixed point over eight steps (x 256 / 8), which is what
              BtlWindowsMoveStep adds as (v * step) >> 8;
     moving = the record's open flag (1 as the openers write it), last.
   The corner does not move; only the right and bottom edges do. A leaf: no
   calls, no frame wait, no random draw, no return value.

   Matching. Every access indexes its table afresh, as BtlWindowsMoveStep and
   BtlWindowsInit do in this TU: cse then keeps each table's address in a
   register of its own, formed before the index product (la; shifts; addu
   product, base). With two record pointers (`m = &table[idx]`) the product
   comes first and the registers differ (29/57). Each difference is taken
   into an s16 local AHEAD of its axis' first store and stored after the
   target edge's loads: the target reads w and ow before it stores x0 and
   stores vx four load groups later, and a load through one table does not
   cross a store through the other (written inside the vx / vy stores:
   31/57; an s32 local: 51/57). Statement order is the target's store
   order except `step = 0`, which the scheduler lifts into the state
   byte's load delay (written first: 55/57); vx goes after tx1 (before it:
   24/57). `<< 5` is not `(d << 8) / 8`, whose signed divide rounds
   (25/57). */
/* func_8004A26C */
void BtlWindowMoveStart(s32 idx)
{
    s16 dx;
    s16 dy;

    if (((BtlWindowMove *)g_BtlWindowMoves)[idx].moving == 0) {
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].mode = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].mode;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].step = 0;
        dx = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].width - ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].width_before;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].left = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].x;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].right = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].x + ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].width_before;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].left_to = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].x;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].right_to = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].x + ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].width;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].speed_x = dx << 5;
        dy = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].height - ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].height_before;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].top = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].y;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].bottom = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].y + ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].height_before;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].top_to = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].y;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].bottom_to = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].y + ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].height;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].speed_y = dy << 5;
        ((BtlWindowMove *)g_BtlWindowMoves)[idx].moving = ((BtlWindowRectangle *)g_BtlWindowRectangles)[idx].open;
    }
}

void BtlWindowSpritesSet(s32, s16, s16, s16, s16);

/* The battle windows' per-frame motion (called each battle frame): for
   each of the two windows that is on and moving, the right/bottom edges
   are the start edge + velocity * step >> 8, clamped at the target (which
   also snaps that axis' left/top edge to its target and zeroes the
   velocity); at step 8, or once both axes have arrived, the target
   rectangle is committed and the motion stops — a window that ends with
   x0 == x1 and y0 == y1 has closed, and its g_BtlWindowOutlines row is switched
   off; otherwise the step advances. BtlWindowSpritesSet then rebuilds the
   window's frame primitives from the rectangle. Every access indexes the
   table afresh (the target recomputes &g_BtlWindowMoves[i] at each join), and
   the commit's stores run x0/y0/x1 first, then step/moving, then y1:
   fields stored before the step/moving stores are re-read for the
   compares, y1 (stored after them) compares from its register;
   the two compares are nested ifs (an && of the four adjacent fields is
   folded into one word compare). */
/* func_8004A350 */
void BtlWindowsMoveStep(void)
{
    s16 i;
    s16 x0, y0, x1, y1;

    for (i = 0; i < 2; i++) {
        if (((BtlWindowMove *)g_BtlWindowMoves)[i].mode != 0 && ((BtlWindowMove *)g_BtlWindowMoves)[i].moving != 0) {
            x0 = ((BtlWindowMove *)g_BtlWindowMoves)[i].left;
            y0 = ((BtlWindowMove *)g_BtlWindowMoves)[i].top;
            x1 = ((BtlWindowMove *)g_BtlWindowMoves)[i].right
               + ((((BtlWindowMove *)g_BtlWindowMoves)[i].speed_x * ((BtlWindowMove *)g_BtlWindowMoves)[i].step) >> 8);
            y1 = ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom
               + ((((BtlWindowMove *)g_BtlWindowMoves)[i].speed_y * ((BtlWindowMove *)g_BtlWindowMoves)[i].step) >> 8);
            if (((BtlWindowMove *)g_BtlWindowMoves)[i].speed_x > 0) {
                if (x1 >= ((BtlWindowMove *)g_BtlWindowMoves)[i].right_to) {
                    x0 = ((BtlWindowMove *)g_BtlWindowMoves)[i].left_to;
                    x1 = ((BtlWindowMove *)g_BtlWindowMoves)[i].right_to;
                    ((BtlWindowMove *)g_BtlWindowMoves)[i].speed_x = 0;
                }
            } else if (x1 <= ((BtlWindowMove *)g_BtlWindowMoves)[i].right_to) {
                x0 = ((BtlWindowMove *)g_BtlWindowMoves)[i].left_to;
                x1 = ((BtlWindowMove *)g_BtlWindowMoves)[i].right_to;
                ((BtlWindowMove *)g_BtlWindowMoves)[i].speed_x = 0;
            }
            if (((BtlWindowMove *)g_BtlWindowMoves)[i].speed_y > 0) {
                if (y1 >= ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom_to) {
                    y0 = ((BtlWindowMove *)g_BtlWindowMoves)[i].top_to;
                    y1 = ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom_to;
                    ((BtlWindowMove *)g_BtlWindowMoves)[i].speed_y = 0;
                }
            } else if (y1 <= ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom_to) {
                y0 = ((BtlWindowMove *)g_BtlWindowMoves)[i].top_to;
                y1 = ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom_to;
                ((BtlWindowMove *)g_BtlWindowMoves)[i].speed_y = 0;
            }
            if (((BtlWindowMove *)g_BtlWindowMoves)[i].step >= 8
                || (((BtlWindowMove *)g_BtlWindowMoves)[i].speed_x == 0 && ((BtlWindowMove *)g_BtlWindowMoves)[i].speed_y == 0)) {
                ((BtlWindowMove *)g_BtlWindowMoves)[i].left = ((BtlWindowMove *)g_BtlWindowMoves)[i].left_to;
                ((BtlWindowMove *)g_BtlWindowMoves)[i].top = ((BtlWindowMove *)g_BtlWindowMoves)[i].top_to;
                ((BtlWindowMove *)g_BtlWindowMoves)[i].right = ((BtlWindowMove *)g_BtlWindowMoves)[i].right_to;
                ((BtlWindowMove *)g_BtlWindowMoves)[i].step = 0;
                ((BtlWindowMove *)g_BtlWindowMoves)[i].moving = 0;
                ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom = ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom_to;
                if (((BtlWindowMove *)g_BtlWindowMoves)[i].left == ((BtlWindowMove *)g_BtlWindowMoves)[i].right) {
                    if (((BtlWindowMove *)g_BtlWindowMoves)[i].top == ((BtlWindowMove *)g_BtlWindowMoves)[i].bottom) {
                        ((SpriteEntry *)g_BtlWindowOutlines)[i].on = 0;
                    }
                }
            } else {
                ((BtlWindowMove *)g_BtlWindowMoves)[i].step++;
            }
            BtlWindowSpritesSet(i, x0, y0, x1, y1);
        }
    }
}

extern SpritePiece g_BtlWindowOutlinePieces[];
extern SpritePiece g_BtlWindowBoxPieces[];

/* func_8004A5EC */
void BtlWindowSpritesSet(s32 idx, s16 x, s16 y, s16 w, s16 h)
{
    BtlWindowSprites *rec;

    rec = (BtlWindowSprites *)(g_SpriteList + 0x900);
    rec[idx].outline = ((SpriteEntry *)g_BtlWindowOutlines)[idx];
    if (rec[idx].outline.on != 0) {
        g_BtlWindowOutlinePieces[idx * 2].x0 = g_BtlWindowOutlinePieces[idx * 2].x2 = x;
        g_BtlWindowOutlinePieces[idx * 2].y0 = g_BtlWindowOutlinePieces[idx * 2].y1 = y;
        g_BtlWindowOutlinePieces[idx * 2].x1 = w - 1;
        g_BtlWindowOutlinePieces[idx * 2].y2 = y;
        g_BtlWindowOutlinePieces[idx * 2].x3 = x;
        g_BtlWindowOutlinePieces[idx * 2].y3 = h;
        g_BtlWindowOutlinePieces[idx * 2 + 1].x0 = g_BtlWindowOutlinePieces[idx * 2 + 1].x1 = w;
        g_BtlWindowOutlinePieces[idx * 2 + 1].y0 = g_BtlWindowOutlinePieces[idx * 2 + 1].y2 = h;
        g_BtlWindowOutlinePieces[idx * 2 + 1].y1 = y;
        g_BtlWindowOutlinePieces[idx * 2 + 1].x2 = w;
        g_BtlWindowOutlinePieces[idx * 2 + 1].x3 = x + 1;
        g_BtlWindowOutlinePieces[idx * 2 + 1].y3 = h;
    }
    if (((BtlWindowMove *)g_BtlWindowMoves)[idx].mode == 1 && idx == 0) {
        rec[idx].outline = g_BtlWindowBoxes[idx + 1];
        rec[0].outline.on = 1;
        g_BtlWindowBoxPieces[0].x0 = g_BtlWindowBoxPieces[0].x3 = x;
        g_BtlWindowBoxPieces[0].y0 = g_BtlWindowBoxPieces[0].y1 = y;
        g_BtlWindowBoxPieces[0].x1 = g_BtlWindowBoxPieces[0].x2 = w;
        g_BtlWindowBoxPieces[0].y2 = g_BtlWindowBoxPieces[0].y3 = h;
    }
    if (((BtlWindowMove *)g_BtlWindowMoves)[idx].mode == 2) {
        rec[idx].box = g_BtlWindowBoxes[idx];
        g_BtlWindowBoxPieces[idx].x0 = g_BtlWindowBoxPieces[idx].x3 = x;
        g_BtlWindowBoxPieces[idx].y0 = g_BtlWindowBoxPieces[idx].y1 = y;
        g_BtlWindowBoxPieces[idx].x1 = g_BtlWindowBoxPieces[idx].x2 = w;
        g_BtlWindowBoxPieces[idx].y2 = g_BtlWindowBoxPieces[idx].y3 = h;
    }
}
