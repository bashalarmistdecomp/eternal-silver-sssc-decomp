/* Lunar SSSC — chr_worn: a character's worn items (removed, changed, their
   values and stats), the questions about an item's or a spell's use, and
   the item lists closed up and sorted. The first split had it in
   scripting.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
#include "chr.h"
extern s32 g_ChrItemList0C;

s32 StoryBitSet(s32);
extern s32 g_ChrItemRows;
s32 StoryBitClear(s32);
s32 ChrWornItemValuesRemove(s32, s32);
extern s32 g_ChrRecords;

/* func_80058038 */
s32 ChrWornItemRemove(s32 chr_id, s32 worn_place)
{
  s32 masked_chr_id;
  s32 masked_item;
  u8 *worn_item;
  u8 item;
  masked_chr_id = chr_id & 0xFFFF;
  worn_item = (u8 *)(g_ChrRecords + (masked_chr_id << 6)) + ((worn_place & 0xFFFF) + 0x1D);
  item = *worn_item;
  *worn_item = 0;
  masked_item = item & 0xFF;
  ChrWornItemValuesRemove(masked_chr_id, masked_item);
  return masked_item;
}
/* The item table's QUERY: one answer about item `item`, picked by `mode`, read
   from the item's 18-byte record of g_ChrItemRows (index id - 1; the offsets
   below are from id * 18, as ChrWornItemChangeMake writes them). No call, no store,
   no RNG draw, no frame wait.

     item outside 1..0xB6  -2, whatever the mode (the table has 182 rows)
     mode 1  MAY `who` WEAR IT: 0 when bit who - 1 of the u16 mask at +0x10
             (-2) is set, else -1. The callers that pass who = 0x10 read
             mask bit 15 this way (ChrItemGive's "keep", MenuPoseWordsSet).
     mode 2  HAS IT A SALE PRICE: 0 when the u16 at +2 (-0x10) is non-zero,
             else -1 (MenuItemDropStep, the item grid's pages 0xD / 0x16,
             refuses an item that has none).
     mode 3  the HIGH nibble of the byte at +6 (-0xC), or -1 when it is 9
             (MenuItemUseStep, the item grid's USE page: negative = not
             usable, < 5 no target, >= 5 the target pick).
     mode 4  the LOW nibble of the same byte, or -1 when it is 9
             (MenuPresetDisp / MenuPresetItemStep, the command set-up page's
             item picker).
     any other mode  -1.

   Shape: `who` and `item` are narrow parameters (the entry copies $a1 and
   each arm masks its own copy; `andi $a0, 0xFFFF` at the one use of who),
   `mode` is unsigned (the switch's `sltiu`; an int mode gives `slti`,
   75/76), the cases are written 1, 2, 3, 4 (2 before 1: 54/76; 4 before 3:
   72), and the result is one variable set to -1 AFTER the range test (an
   initialised declaration: 66; `return 0;` inside an arm: 63). An if / else
   chain on mode is 49. The mask test folds to `(mask >> (who - 1)) & 1`
   (`srav`, `andi 1`) when tested directly -- written that way it is the
   same bytes; ShopItemDisp / ShopItemLineUpSet need a variable to stop the
   same fold. `(u8)(item - 1) >= 0xB6` is the same bytes as the range test
   below. */
/* func_80058088 */
s32 ChrItemUseGet(u16 who, u8 item, u32 mode)
{
    s32 ret;
    s32 v;

    if (item == 0 || item > 0xB6) {
        return -2;
    }
    ret = -1;
    switch (mode) {
    case 1:
        if (*(u16 *)(g_ChrItemRows + item * 18 - 2) & (1 << (who - 1))) {
            ret = 0;
        }
        break;
    case 2:
        if (*(u16 *)(g_ChrItemRows + item * 18 - 0x10) != 0) {
            ret = 0;
        }
        break;
    case 3:
        v = *(u8 *)(g_ChrItemRows + item * 18 - 0xC) >> 4;
        if (v != 9) {
            ret = v;
        }
        break;
    case 4:
        v = *(u8 *)(g_ChrItemRows + item * 18 - 0xC) & 0xF;
        if (v != 9) {
            ret = v;
        }
        break;
    }
    return ret;
}

/* The equipment-change preview: what `who`'s stats become when the
   worn item `oldItem` is swapped for `newItem` (either may be 0 = none).
   Callers: ShopItemStatsDisp (the shop) and MenuWornItemDisp (the field menu),
   both with a 0x18-byte buffer on their stack.

   `out` is up to four rows of three u16 {stat kind, value now, value
   after}, cleared first (mem_clear). A row is filled by
   ChrWornItemChangeRowSet(who, row, add, sub): it reads the member's stat `kind`
   (1..8; the 0x40 record in g_ChrRecords) into row[1] and writes
   row[1] - sub + add into row[2].

   An item is an 18-byte record of g_ChrItemRows, index id - 1. Byte +12 holds
   two stat kinds, low nibble and high nibble; byte +10 is the bonus to the
   low-nibble stat, byte +11 the bonus to the high-nibble stat. (The
   offsets below are from id * 18: -6, -8, -7.)

   Low-nibble stat, rows 0 and 1 (out[0], out[3] hold the two kinds):
     new kind == old kind  one row: now -> now - oldBonus + newBonus
     both, different       row 0: now + newBonus; row 1 (old kind):
                           now - oldBonus; the next rows start at 6
     new only              row 0: now + newBonus
     new kind 0            the old kind moves to row 0: now - oldBonus --
                           filled even when the old kind is 0 too (kind 0
                           reads no stat: the row is {0, 0, -oldBonus})
   i = the u16 index where the high-nibble rows start: 6 after two rows,
   otherwise 3.

   High-nibble stat, rows at out[i] and out[i + 3], the same four cases
   with bonus byte +11 -- except that nothing is written when both kinds
   are 0, and the equal case clears the old kind AFTER its call.

   Nothing here draws from the RNG or waits a frame; both callees are
   leaves.

   Shape: newItem / oldItem are u8 parameters (a plain move at entry, an
   `andi 0xFF` in every block that reads them); a and b are u8 locals --
   combine drops their zero-extension where the byte load is in another
   block (every set of them is a byte load or 0), and keeps it in the last
   arm, where the load is in the call's own block with two stores in
   between (`andi $a3, $s1, 0xFF`). The low-nibble half holds `out + 3` in
   a variable (q, $s3); the high-nibble half has NO pointer variable: the
   target's index-first `addu $s0, $s3, $s2` is cse's shared MEM address
   for out[i], and `&out[i + 3]` as an argument stays `out + (i * 2 + 6)`.
   In the `new kind 0` arm `i = 3` is written AFTER the call: the block
   then does not end in the call, so sched1 places it by RTL order -- the
   target's `li $s0, 3` between the load and the store. Written before the
   call it sorts to the top of the block (192/194). */
/* func_800581B8 */
void ChrWornItemChangeMake(s32 who, u16 *out, u8 newItem, u8 oldItem)
{
    u8 a;
    u8 b;
    s32 i;
    u16 *q;

    mem_clear(out, 0x18);
    if (newItem == 0) {
        if (oldItem == 0) {
            return;
        }
        a = 0;
    } else {
        a = *(u8 *)(g_ChrItemRows + newItem * 18 - 8);
    }
    if (oldItem != 0) {
        b = *(u8 *)(g_ChrItemRows + oldItem * 18 - 8);
    } else {
        b = 0;
    }
    i = 0;
    if (newItem != 0) {
        out[0] = *(u8 *)(g_ChrItemRows + newItem * 18 - 6) & 0xF;
    }
    if (oldItem != 0) {
        out[3] = *(u8 *)(g_ChrItemRows + oldItem * 18 - 6) & 0xF;
    }
    if (out[0] != 0) {
        q = out + 3;
        if (out[0] == out[3]) {
            out[3] = 0;
            ChrWornItemChangeRowSet(who, (s32)out, a, b);
        } else {
            ChrWornItemChangeRowSet(who, (s32)out, a, 0);
            if (out[3] != 0) {
                i = 3;
                ChrWornItemChangeRowSet(who, (s32)q, 0, b);
            }
        }
        i += 3;
    } else {
        out[0] = out[3];
        out[3] = 0;
        ChrWornItemChangeRowSet(who, (s32)out, 0, b);
        i = 3;
    }
    if (newItem != 0) {
        out[i] = *(u8 *)(g_ChrItemRows + newItem * 18 - 6) >> 4;
    }
    if (oldItem != 0) {
        out[i + 3] = *(u8 *)(g_ChrItemRows + oldItem * 18 - 6) >> 4;
    }
    if (out[i] != 0) {
        a = *(u8 *)(g_ChrItemRows + newItem * 18 - 7);
        if (out[i + 3] != 0) {
            b = *(u8 *)(g_ChrItemRows + oldItem * 18 - 7);
            if (out[i] == out[i + 3]) {
                ChrWornItemChangeRowSet(who, (s32)&out[i], a, b);
                out[i + 3] = 0;
            } else {
                ChrWornItemChangeRowSet(who, (s32)&out[i], a, 0);
                ChrWornItemChangeRowSet(who, (s32)&out[i + 3], 0, b);
            }
        } else {
            ChrWornItemChangeRowSet(who, (s32)&out[i], a, 0);
        }
    } else if (out[i + 3] != 0) {
        b = *(u8 *)(g_ChrItemRows + oldItem * 18 - 7);
        out[i] = out[i + 3];
        out[i + 3] = 0;
        ChrWornItemChangeRowSet(who, (s32)&out[i], 0, b);
    }
}

/* func_800584C0 */
s32 ChrWornItemChangeRowSet(s32 chr_id, s32 row_address, s32 new_bonus, s32 old_bonus)
{
  u16 *p;
  s32 base;
  s32 v;
  p = (u16 *)row_address;
  switch (p[0])
  {
    case 1:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->stat_08;
      break;
    case 2:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->stat_0a;
      break;
    case 3:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->stat_0c;
      break;
    case 4:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->stat_0e;
      break;
    case 5:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->stat_10;
      break;
    case 6:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->attack_count;
      break;
    case 7:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->stat_1d;
      break;
    case 8:
      base = (chr_id << 6) + g_ChrRecords;
      p[1] = ((ChrRecord *)base)->walk_reach;
      break;
  }
  v = p[1] - (old_bonus & 0xFF);
  p[2] = (new_bonus & 0xFF) + v;
  return v;
}

extern s32 g_PartyIds;
extern s32 g_ChrRecords;

/* func_800585D0 */
void StoryBit2CAUpdate(void)
{
  s32 place;
  s32 masked_chr_id;
  u16 chr_id;
  place = 0;
  loop_1:
  chr_id = *((s16 *) ((place * 2) + g_PartyIds));

  if (chr_id != 0)
  {
    masked_chr_id = (s32) chr_id;
    if (masked_chr_id < 0xC)
    {
      place += 1;
      if ((*(((u8 *) ((chr_id << 6) + g_ChrRecords)) + 0x1e)) != 0)
      {
        StoryBitSet(0x2CA);
        return;
      }
      goto block_6;
    }
    place += 1;
    block_6:
    if (place >= 6)
    {
      goto block_7;
    }

    goto loop_1;
  }
  block_7:
  StoryBitClear(0x2CA);

}

/* func_80058654 */
s32 ChrSpellAnyCheck(s32 chr_id)
{
  s32 masked_chr_id;
  s32 spell_index;
  u8 *chr_record;
  masked_chr_id = chr_id & 0xFFFF;
  spell_index = 0;
  chr_record = (u8 *) ((masked_chr_id << 6) + g_ChrRecords);
  do
  {
    if (*(chr_record + spell_index + 0x2A) != 0)
    {
      return 1;
    }
    if (chr_record)
    {
      spell_index += 1;
    }
    else
    {
      spell_index += 1;
    }
  }
  while (spell_index < 8);
  return 0;
}
/* far_syms: g_ChrSpellRows (target reaches it %hi/%lo) */
extern s32 g_ChrSpellRows;

/* THE SPELL CHECK: may member `who` use spell `id`, and with which target mode?
   Four jal sites, all in menu_page.c and all with id = (u8)(list entry +
   0x4A), i.e. entry - 0xB6: the field menu's Magic page
   (MenuSpellStep, twice) with mode 2, and the command set-up page's
   row greying (MenuPresetDisp) and spell picker (MenuPresetSpellStep) with mode
   4. Nothing passes 3.

     id outside 1..0x32   -> -2
     mode 4               -> the LOW nibble of the spell row's target byte
     mode 2               -> the MP test below, then as mode 3
     mode 3               -> the HIGH nibble of that byte
     any other mode       -> -1
     a nibble of 9        -> -1 (not usable there)

   The row is the 12-byte record id - 1 of the table g_ChrSpellRows points at:
   +1 the target byte, +4 the MP cost. Mode 2's MP test: when the member
   wears item 0x88 in either of the last two worn slots (+0x22 / +0x23 of
   the 0x40 record in g_ChrRecords) the cost is halved rounding up; current
   MP (+4) below the cost answers -1. Nothing is stored; no call, no RNG
   draw, no frame wait.

   What the bytes pin, one change each: the cases are written 4, 2, 3 (in
   the order 2, 3, 4: 62/78), 4 and 3 each with its own `if (v != 9)`,
   which cross-jumping merges into one (a single test after the switch,
   with returns inside it: 71/78, each return keeps a `jr $ra`); the party
   record's address is formed again at each use, product first (a record
   pointer local 65/78, base first 75/78); `mode` is unsigned (signed
   77/78: the switch's range test is sltiu) and so is `cost` (s32 76/78:
   srl, sltu). g_ChrSpellRows is a far scalar like this TU's g_ChrItemRows: the
   target's `lui $v1` / `lw $v1, %lo(...)($v1)` on ONE register is the
   assembler's expansion of a single load (declared `extern s32
   g_ChrSpellRows[]` and read `[0]`, the lui gets a register of its own and
   the allocation moves: 58/78). */
/* func_80058698 */
s32 ChrSpellUseGet(u16 who, u8 id, u32 mode)
{
    s32 ret;
    u32 cost;
    s32 v;

    if (id == 0 || id > 0x32) {
        return -2;
    }
    ret = -1;
    switch (mode) {
    case 4:
        v = *(u8 *)(g_ChrSpellRows + id * 12 - 0xB) & 0xF;
        if (v != 9) {
            ret = v;
        }
        break;
    case 2:
        cost = *(u8 *)(g_ChrSpellRows + id * 12 - 8);
        if (((ChrRecord *)((who << 6) + g_ChrRecords))->worn_items[4] == 0x88
            || ((ChrRecord *)((who << 6) + g_ChrRecords))->worn_items[5] == 0x88) {
            cost = (cost + 1) >> 1;
        }
        if (((ChrRecord *)((who << 6) + g_ChrRecords))->mp < cost) {
            break;
        }
    case 3:
        v = *(u8 *)(g_ChrSpellRows + id * 12 - 0xB) >> 4;
        if (v != 9) {
            ret = v;
        }
        break;
    }
    return ret;
}

void ChrWornItemStatAdd(s32, u32, s32);

/* func_800587D0 */
void ChrWornItemValuesAdd(s32 chr_id, s32 item)
{
  s32 masked_item;
  s32 chr_record;
  void *item_row;
  masked_item = item & 0xFF;
  if (masked_item != 0)
  {
    chr_record = g_ChrRecords + ((chr_id & 0xFFFF) << 6);
    item_row = g_ChrItemRows + ((short)((unsigned short) ((masked_item * 0x12) - 0x12)));
    ChrWornItemStatAdd(chr_record, ((u8) (AS(s32, ((BtlItemRow *)item_row)->bonus_stats))) >> 4, ((BtlItemRow *)item_row)->bonus_2);
    ChrWornItemStatAdd(chr_record, (*(((u8 *) item_row) + 0xc)) & 0xF, ((BtlItemRow *)item_row)->bonus_1_power.bonus_1);
    if (((BtlItemRow *)item_row)->story_bit != 0)
    {
      if (g_ChrItemRows)
      {
        StoryBitSet(((BtlItemRow *)item_row)->story_bit + 0x2BC);
      }
      else
      {
        StoryBitSet(((BtlItemRow *)item_row)->story_bit + 0x2BC);
      }
    }
  }
}

/* far_syms: g_ChrItemRows (target reaches it %hi/%lo) */
void ChrWornItemStatAdd(s32, u32, s32);

/* func_8005886C */
s32 ChrWornItemValuesRemove(s32 chr_id, s32 item)
{
  int item_row_offset;
  s32 masked_item;
  s32 chr_record;
  int negated_bonus;
  masked_item = item & 0xFF;
  if (masked_item != 0)
  {
    void *item_row;
    chr_record = g_ChrRecords + ((chr_id & 0xFFFF) << 6);
    item_row_offset = masked_item * 0x12;
    item_row_offset = item_row_offset - 0x12;
    item_row = g_ChrItemRows + item_row_offset;
    ChrWornItemStatAdd(chr_record, ((u8) (AS(s32, ((BtlItemRow *)item_row)->bonus_stats))) >> 4, negated_bonus = -((s32) (((BtlItemRow *)item_row)->bonus_2)));
    ChrWornItemStatAdd(chr_record, (*(((u8 *) item_row) + 0xc)) & 0xF, -((s32) (((BtlItemRow *)item_row)->bonus_1_power.bonus_1)));
    if (((BtlItemRow *)item_row)->story_bit != 0)
    {
      if (g_ChrItemRows)
      {
        StoryBitClear(((BtlItemRow *)item_row)->story_bit + 0x2BC);
      }
      else
      {
        StoryBitClear(((BtlItemRow *)item_row)->story_bit + 0x2BC);
      }
    }
  }
}

/* func_80058910 */
void ChrWornItemStatAdd(s32 chr_record, u32 stat_kind, s32 bonus)
{
	s32 v;

	switch (stat_kind & 0xFF) {
	case 0:
		break;
	case 1:
		v = ((ChrRecord *)chr_record)->stat_08 + bonus;
		((ChrRecord *)chr_record)->stat_08 = v;
		return;
	case 2:
		v = ((ChrRecord *)chr_record)->stat_0a + bonus;
		((ChrRecord *)chr_record)->stat_0a = v;
		return;
	case 3:
		v = ((ChrRecord *)chr_record)->stat_0c + bonus;
		((ChrRecord *)chr_record)->stat_0c = v;
		return;
	case 4:
		v = ((ChrRecord *)chr_record)->stat_0e + bonus;
		((ChrRecord *)chr_record)->stat_0e = v;
		return;
	case 5:
		v = ((ChrRecord *)chr_record)->stat_10 + bonus;
		((ChrRecord *)chr_record)->stat_10 = v;
		return;
	case 6:
		v = bonus + ((ChrRecord *)chr_record)->attack_count;
		((ChrRecord *)chr_record)->attack_count = v;
		return;
	case 7:
		v = bonus + ((ChrRecord *)chr_record)->stat_1d;
		((ChrRecord *)chr_record)->stat_1d = v;
		return;
	case 8:
		v = bonus + ((ChrRecord *)chr_record)->walk_reach;
		((ChrRecord *)chr_record)->walk_reach = v;
		return;
	}
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_8001288C_pad);
extern s32 g_ChrItemWaitingList;

/* CLOSES THE GAP a removed item leaves in one of the
   three item lists: every cell after `cell` moves down one place, up to
   and including the first empty one, and the cell the walk stops on gets
   a 0 below it. Only caller: ChrItemRemove (chr_item.c, the inventory
   remove), which has already zeroed the cell.

   list:  0xD = the eight pending cells at g_ChrItemWaitingList;
          0xC = the shared bag, 0x60 cells at g_ChrItemList0C;
          any other value = that member id's six carried cells, the items
          array at +0x24 of its 0x40 record in g_ChrRecords.

   for (i = cell + 1; i < N; i++) { c[i - 1] = c[i]; if (c[i] == 0) break; }
   c[i - 1] = 0;

   So with the list full to its end the last cell is cleared (i == N on
   exit), and with an empty cell at i the copy has already put the 0 at
   i - 1 and the final store writes it again. The walk does not look at
   what was in `cell` itself. No call, no RNG draw, no frame wait.

   Shape notes. The bag and the pending cells are byte pointers held in
   s32 globals; there the store's and the load's address share one
   register (i + list, offsets -1 and 0). The member's cells do NOT share
   one: the target computes record + i for the load and i + record for
   the store, which is what an ARRAY MEMBER gives -- the store's index
   i - 1 is expanded as a value of its own, so cse cannot merge the two
   addresses and combine folds the -1 into the offset afterwards (0x23).
   Written through a byte pointer (`((u8 *)rec)[i + 0x23]`) the two
   addresses merge (85/87). ChrRecord is chr.h's typedef of the
   record, member for member. Parameters are narrow (u16, u8) as the
   caller's prototype in chr_item.c has them; the counter is an int. */
/* func_800589E0 */
void ChrItemListGapClose(u16 list, u8 cell)
{
    s32 i;

    if (list == 0xD) {
        for (i = cell + 1; i < 8; i++) {
            ((u8 *)g_ChrItemWaitingList)[i - 1] = ((u8 *)g_ChrItemWaitingList)[i];
            if (((u8 *)g_ChrItemWaitingList)[i] == 0) {
                break;
            }
        }
        ((u8 *)g_ChrItemWaitingList)[i - 1] = 0;
    } else if (list == 0xC) {
        for (i = cell + 1; i < 0x60; i++) {
            ((u8 *)g_ChrItemList0C)[i - 1] = ((u8 *)g_ChrItemList0C)[i];
            if (((u8 *)g_ChrItemList0C)[i] == 0) {
                break;
            }
        }
        ((u8 *)g_ChrItemList0C)[i - 1] = 0;
    } else {
        for (i = cell + 1; i < 6; i++) {
            ((ChrRecord *)(list * 0x40 + g_ChrRecords))->items[i - 1] = ((ChrRecord *)(list * 0x40 + g_ChrRecords))->items[i];
            if (((ChrRecord *)(list * 0x40 + g_ChrRecords))->items[i] == 0) {
                break;
            }
        }
        ((ChrRecord *)(list * 0x40 + g_ChrRecords))->items[i - 1] = 0;
    }
}

/* far_syms: g_ChrItemRows */
extern u8 *g_GameOptions;
void ChrItemListPartSort(u8 *list, s32 first_index, s32 last_index);  /* TU defines this below */

/* Sorts one item list: the party bag (who == 0xC, slots 0..0x5F) or a
   character's six item slots (record +0x24). Lists with fewer than two
   items are left alone. Trailing empty slots are trimmed, then the items
   are partitioned by their item-table kind (byte 4 of the 18-byte record):
   kind >= 5 first when the config byte g_GameOptions[2] is set, kind < 5 first
   otherwise; ChrItemListPartSort then sorts each half. */
/* func_80058B3C */
void ChrItemListSort(u16 who)
{
    u8 *list;
    s32 n;
    s32 i;
    s32 j;
    u8 t;

    if (who == 0xC) {
        list = (u8 *)g_ChrItemList0C;
        n = 0x5F;
    } else {
        n = 5;
        list = (u8 *)(g_ChrRecords + who * 64 + 0x24);
    }
    if (list[0] != 0 && list[1] != 0) {
        for (; n > 0; n--) {
            if (list[n] != 0) break;
        }
        i = 0;
        j = n + 1;
        if (g_GameOptions[2] != 0) {
            for (;;) {
                while (*(u8 *)(g_ChrItemRows + list[i] * 18 - 14) >= 5) {
                    if (i >= j) break;
                    i++;
                }
                while (*(u8 *)(g_ChrItemRows + list[j - 1] * 18 - 14) < 5) {
                    if (i >= j) break;
                    j--;
                }
                if (i == j) break;
                t = list[i];
                list[i] = list[j - 1];
                list[j - 1] = t;
            }
        } else {
            for (;;) {
                while (*(u8 *)(g_ChrItemRows + list[i] * 18 - 14) < 5) {
                    if (i >= j) break;
                    i++;
                }
                while (*(u8 *)(g_ChrItemRows + list[j - 1] * 18 - 14) >= 5) {
                    if (i >= j) break;
                    j--;
                }
                if (i == j) break;
                t = list[i];
                list[i] = list[j - 1];
                list[j - 1] = t;
            }
        }
        if (i >= 2) {
            ChrItemListPartSort(list, 0, i - 1);
        }
        if (i < n) {
            ChrItemListPartSort(list, i, n);
        }
    }
}

/* far_syms: g_ChrItemSortRanks (target reaches it %hi/%lo) */
extern u8 g_ChrItemSortRanks[];

/* func_80058E18 */
void ChrItemListPartSort(u8 *list, s32 first_index, s32 last_index)
{
  s32 i;
  u8 val1;
  u8 val2;
  u8 saved_item;

  while (first_index < last_index)
  {
    for (i = first_index; i < last_index; i++)
    {
      val1 = *(u8 *)(g_ChrItemSortRanks + *(u8 *)(g_ChrItemRows + list[i] * 18 - 14));
      val2 = *(u8 *)(g_ChrItemSortRanks + *(u8 *)(g_ChrItemRows + list[i + 1] * 18 - 14));
      if (val1 >= val2 && (val1 != val2 || list[i + 1] < list[i]))
      {
        saved_item = list[i];
        list[i] = list[i + 1];
        list[i + 1] = saved_item;
      }
    }
    last_index--;
  }
}
