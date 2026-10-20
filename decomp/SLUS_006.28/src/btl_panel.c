/* Lunar SSSC — btl_panel: the party's panel on the text grid: each member's
   column, status and numbers, with the glyph cells it is written with. The
   first split had it in misc.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "mem.h"
#include "btl.h"
#include "sprite.h"

extern s32 CharStats;
extern u8 g_BtlSlotIds[];
/* The party HUD's per-member record BtlPartyPanelDisp builds on the stack and
   BtlPartyPanelMemberDisp draws: a used flag, three status bytes (CharStats +0x1E & 4,
   +0x15, +0x14) and the hp / max hp / mp / max mp words (CharStats +0..+6). */
extern u8 g_BtlSlotActiveFlags[];

s32 BtlCellRowsUpdate(s32, s32, s32, s32);
s32 BtlCellRowsUpdate(s32, s32, s32, s32);
s32 BtlCellGlyphSet(s32, s32, s32, s32, s32, s32);
s32 CellNumberSet(s32, s32, s32, s32, s32, s32, s32, s32);
void BtlCellRowsClear(s32 first_row, s32 row_count, s32 kind_mask);  /* extern */

void ankprint_glyph_set(s32 column, s32 row, s32 code, s32 width, s32 col);

/* Put one atlas glyph (a kind-1 cell) on the 40 x 28 cell grid.
   BtlCellGlyphSet(code, x, y, width, colour, half): one call,
   ankprint_glyph_set(x, y, code', width, colour), which stores the cell word
   (width << 11) | 0x100 | code' | (colour << 2) at g_AnkprintGrid[y * 40 + x]
   and marks the row dirty (g_AnkprintRowMarks[y] = 1), both through ankprint_cell_set.
     - half == 0: code' is the code as passed (its low 16 bits).
     - half != 0: code' = ((code - 0x80) / 2) | 0x80, a signed divide.
       Three of the 14 call sites set it: BtlMenuSpellOpen and
       BtlMenuFirstOpen with code 0x9C (0x8E, the big "/" of menu-ui.md) and
       BtlWinRewardMessageDisp with 0x9E (0x8F); codes from 0x80 up are the
       16-pixel-high glyphs. A code below 0x80 would leave the divide
       negative and put its high bits into the cell; no call site does
       that.
   width is the glyph's width in 8-pixel cells (1, 2 or 4 from the
   callers); colour is 0, 0x1000, 0x2000 or 0x3000, which lands in the
   cell's top two bits (the sub-CLUT). BtlCellGlyph4Set is the same call with
   width 4 and half 0.
   Reads and writes no global itself. No return statement, no frame wait,
   no random draw (ankprint_glyph_set only builds the cell word, ankprint_cell_set
   only stores it and the row's dirty byte).
   MARKED SPELLING: an old-style definition. By the bytes the function is
   (u16, s32, s32, s32, u16, s32); this TU and four others declare
   s32 (s32, s32, s32, s32, s32, s32) and are left as they are, because the
   callers were matched under that prototype. The s32 return type is kept
   only to agree with them: nothing is returned (void matches as well, and
   `return ankprint_glyph_set(...)` does not, 21/28). The same body under a
   new-style (u16, ..., u16, s32) definition is also 28/28.
   Shape: both narrow parameters are u16, and the two calls are written
   out whole, the `half != 0` one first. The target copies the raw $a0
   into $a2 on entry and masks it in EACH arm, loads the fifth argument
   once with `lhu` and masks it again only in the plain arm; an s32 code
   masked at its two uses is 18/28, an s32 colour cast to u16 at the calls
   24/28, the arms the other way round 21/28, one call after an `if` that
   rewrites the code 11/28. */
/* func_8004B290 */
s32 BtlCellGlyphSet(code, column, row, width, col, half)
u16 code;
s32 column;
s32 row;
s32 width;
u16 col;
s32 half;
{
    if (half != 0) {
        ankprint_glyph_set(column, row, ((code - 0x80) / 2) | 0x80, width, col);
    } else {
        ankprint_glyph_set(column, row, code, width, col);
    }
}

/* func_8004B300 */
void BtlCellGlyph4Set(s32 code, s32 column, s32 row, s32 col) {
    BtlCellGlyphSet(code & 0xFFFF, column, row, 4, col & 0xFFFF, 0);
}

/* far_syms: CharStats */
/* far_syms: g_BtlSlotIds */
void BtlPartyPanelMemberDisp(u16 col, u16 kind, u8 *p);

/* func_8004B330 */
void BtlPartyPanelDisp(void)
{
  BtlPartyPanelMember rec[5];
  u16 ids[5];
  s32 i;
  s32 n;
  s32 x;
  u16 *src;
  BtlCellRowsClear(0x17, 4, 0x20);
  i = 0;
  n = 0;
  for (; i < 5; i++)
  {
    if (g_BtlSlotIds[i] != 0)
    {
      if ((g_BtlSlotActiveFlags[i] != 0) || ((*(u8 *) (i * 0x3C + CharStats + 0x15) & 0x30) == 0x30) || (*(u16 *) (i * 0x3C + CharStats) == 0))
      {
        ids[n] = g_BtlSlotIds[i];
        rec[n].on = 1;
        rec[n].flags_04 = *(u8 *) (i * 0x3C + CharStats + 0x1E) & 4;
        rec[n].status = *(u8 *) (i * 0x3C + CharStats + 0x15);
        rec[n].level = *(u8 *) (i * 0x3C + CharStats + 0x14);
        src = (u16 *) (i * 0x3C + CharStats);
        rec[n].hp = src[0];
        rec[n].hp_max = src[1];
        rec[n].mp = src[2];
        rec[n].mp_max = src[3];
        n++;
      }
    }
  }
  for (; n < 5; n++)
  {
    ids[n] = 0;
  }
  i = 4;
  n = 4;
  do
  {
    if ((ids[i] != 0) && (ids[i] != 0xC))
    {
      BtlPartyPanelMemberDisp(n--, ids[i], (u8 *) &rec[i]);
    }
    i--;
  }
  while (i + 1 > 0);
  n++;
  if (n != 5)
  {
    x = n * 6;
    BtlCellGlyphSet(0x5D, x + 4, 0x19, 1, 0x3000, 0);
    BtlCellGlyphSet(0x5F, x + 5, 0x19, 1, 0x3000, 0);
    BtlCellGlyphSet(0x5E, x + 4, 0x1A, 2, 0x3000, 0);
    BtlCellRowsUpdate(0x17, 0x5C0, 4, 0x20);
  }
}

void BtlPartyPanelStatusDisp(s32 member_record, s32 column, s32 row, s32 col);  /* TU defines this below */
void BtlPartyPanelNumberDisp(s32 level, s32 value, s32 column, s32 row, u16 col);  /* TU defines this below */

/* Draws one party member's HUD block at column `col` (6 cells per member)
   from the BtlPartyPanelMember BtlPartyPanelDisp built: the icon for `kind` (0 and 0xC draw
   nothing; 0xB uses icon 1), BtlPartyPanelStatusDisp's draw of the record, then the
   hp and mp gauges. A gauge's colour comes from current/max: <= 1/4 -> 0,
   <= 1/2 -> 0x1000, below max -> 0x3000, otherwise 0x2000. The mp colour
   tests "full" first and leaves cm unset if mp exceeds max mp. The name
   tint cn is 0 at 0 hp, else 0x1000, or 0x3000 when status bytes 1 and 2
   are both clear. */
/* func_8004B5B8 */
void BtlPartyPanelMemberDisp(u16 col, u16 kind, u8 *p)
{
    u16 cn;
    u16 ch;
    u16 cm;
    s32 x;
    u16 mp;
    u32 mx;

    if (kind == 0 || kind == 0xC) {
        return;
    }
    cn = 0;
    if (((BtlPartyPanelMember *)p)->hp != 0) {
        cn = 0x1000;
        if (((BtlPartyPanelMember *)p)->flags_04 == 0 && ((BtlPartyPanelMember *)p)->status == 0) {
            cn = 0x3000;
        }
    }
    if ((((BtlPartyPanelMember *)p)->hp_max >> 2) >= ((BtlPartyPanelMember *)p)->hp) {
        ch = 0;
    } else if ((((BtlPartyPanelMember *)p)->hp_max >> 1) >= ((BtlPartyPanelMember *)p)->hp) {
        ch = 0x1000;
    } else if (((BtlPartyPanelMember *)p)->hp < ((BtlPartyPanelMember *)p)->hp_max) {
        ch = 0x3000;
    } else {
        ch = 0x2000;
    }
    mp = ((BtlPartyPanelMember *)p)->mp;
    if (mp == ((BtlPartyPanelMember *)p)->mp_max) {
        cm = 0x2000;
    } else {
        mx = ((BtlPartyPanelMember *)p)->mp_max;
        if ((mx >> 2) >= mp) {
            cm = 0;
        } else if ((mx >> 1) >= mp) {
            cm = 0x1000;
        } else if (mp < mx) {
            cm = 0x3000;
        }
    }
    if (kind == 0xB) {
        kind = 1;
    }
    x = col * 6;
    BtlCellGlyphSet((u16)(kind * 4 + 0xC), x + 8, 0x17, 4, cn, 0);
    BtlPartyPanelStatusDisp((s32)p, (u16)(x + 8), 0x18, cn);
    BtlPartyPanelNumberDisp(0, ((BtlPartyPanelMember *)p)->hp, (u16)(x + 9), 0x19, ch);
    BtlPartyPanelNumberDisp(((BtlPartyPanelMember *)p)->level, ((BtlPartyPanelMember *)p)->mp, (u16)(x + 9), 0x1A, cm);
}

/* func_8004B788 */
void BtlPartyPanelStatusDisp(s32 member_record, s32 column, s32 row, s32 col) {
    s32 kept_column;
    s32 kept_row;
    s32 kept_col;
    u8 status;

    if (((BtlPartyPanelMember *)member_record)->hp == 0) {
        BtlCellGlyph4Set(0x58, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
        return;
    }
    if (((BtlPartyPanelMember *)member_record)->flags_04 != 0) {
        BtlCellGlyph4Set(0x3C, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
        return;
    }
    status = ((BtlPartyPanelMember *)member_record)->status;
    switch (status) {
        case 0:
            kept_column = column & 0xFFFF;
            kept_row = row & 0xFFFF;
            kept_col = col & 0xFFFF;
            BtlCellGlyphSet(0xB, kept_column, kept_row, 2, kept_col, 0);
            if (((BtlPartyPanelMember *)member_record)->level != 0xFF) {
                CellNumberSet(((BtlPartyPanelMember *)member_record)->level, kept_column + 2, kept_row, 2, 0xDEAD2001, kept_col, 0, 0);
            } else {
                BtlCellGlyphSet(0x5C, kept_column + 2, kept_row, 1, kept_col, 0);
                BtlCellGlyphSet(0x5C, kept_column + 3, kept_row, 1, kept_col, 0);
            }
            break;
        case 1:
        case 2:
        case 3:
            BtlCellGlyph4Set(0x40, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
            break;
        case 4:
            BtlCellGlyph4Set(0x44, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
            break;
        case 8:
            BtlCellGlyph4Set(0x48, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
            break;
        case 0x10:
        case 0x20:
        case 0x30:
            BtlCellGlyph4Set(0x4C, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
            break;
        case 0x40:
            BtlCellGlyph4Set(0x50, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
            break;
        case 0x80:
            BtlCellGlyph4Set(0x54, column & 0xFFFF, row & 0xFFFF, col & 0xFFFF);
            break;
        default:
            break;
    }
}

/* func_8004B944 */
void BtlPartyPanelNumberDisp(s32 level, s32 value, s32 column, s32 row, u16 col) {
    s32 kept_row;
    s32 kept_column;

    if ((level & 0xFF) != 0xFF) {
        CellNumberSet(value & 0xFFFF, column & 0xFFFF, row & 0xFFFF, 3, 0xDEAD2001, col & 0xFFFF, 0, 0);
        return;
    }
    kept_column = column & 0xFFFF;
    kept_row = row & 0xFFFF;
    BtlCellGlyphSet(0x5C, kept_column, kept_row, 1, 0x3000, 0);
    BtlCellGlyphSet(0x5C, kept_column + 1, kept_row, 1, 0x3000, 0);
    BtlCellGlyphSet(0x5C, kept_column + 2, kept_row, 1, 0x3000, 0);
}
