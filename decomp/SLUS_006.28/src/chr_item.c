/* Lunar SSSC — chr_item: a character's items: given, waiting, removed, room
   and holding checked, and a worn item set. The first split had it in
   misc_l.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern s32 g_ChrItemWaitingList;
void ChrItemWaitingAdd(u8 item);
s32 ChrItemGive(u16, u8);

extern s32 g_PartyIds;
extern s32 g_ChrRecords;
s32 StoryBitClear(u16 bit);  /* extern */
s32 StoryBitCheck(u32 flag);  /* extern */

/* far_syms: g_ChrItemNoRoomItem g_ChrItemNoRoomState */

extern u8 *g_ChrItemList0C;
extern u8 g_PartyCount;
extern s32 g_ChrItemNoRoomItem;
extern s32 g_ChrItemNoRoomState;
extern s16 g_MessageRow;
extern s32 g_ChrItemNoRoomString[];
void ChrItemListSort(u16);

/* The INVENTORY ADD: give `item` to member `who`, or
   with who == 0 to the first party member that has room. Callers: the shop
   (ShopDealRun), the item pages (MenuItemGiveStep / _02, MenuWornItemChange),
   ChrItemWaitingGive (the pending queue below) and the script
   commands ScriptCommandChrItemGive / ScriptCommandChrLevelItemSet. One item per cell: nothing
   stacks and there are no counts.

   keep = ChrItemUseGet(0x10, item, 1): 0 when bit 15 of the item record's
          mask (u16 at +0x10 of its 0x12-byte row in *g_ChrItemRows) is SET,
          -1 when it is clear (-2 for an id outside 1..0xB6).

   For each field-party slot i < g_PartyCount, id = ((u16 *)g_PartyIds)[i]:
     - ids 8, 9 and 10 are skipped;
     - with who != 0 only the slot whose id == who is looked at;
     - ChrItemRoomCheck(id) == 0 skips the slot (it returns 1 when that
       member -- or, for 0xC, the shared list -- has an empty cell).
     id == 0xC, the shared list (0x60 cells at g_ChrItemList0C): the item goes
       into the FIRST empty cell; ChrItemListSort(0xC) (the list's sort),
       StoryBitClear(0x2D6) (event flag 0x2D6 cleared), return 1.
     any other id: the member's six cells are bytes +0x24..+0x29 of its
       0x40 record in g_ChrRecords (the six before them, +0x1E.., are the
       equipment slots). At the FIRST empty cell j:
         - if keep == 0 and the member is neither id 1 nor id 0xB, the
           shared list is searched from cell 0x5F DOWN to 0 for a cell
           whose content x gives ChrItemUseGet(0x10, x, 1) != 0 -- an item
           without mask bit 15, or no item at all (an empty cell: -2), so
           the LAST such cell. The new item is stored THERE, the list is
           sorted
           (ChrItemListSort(0xC)), and what that cell held -- possibly
           nothing -- becomes the thing given to the member;
         - cell j = the item (or the displaced one, or 0),
           ChrItemListSort(id), StoryBitClear(0x2D6), return 1.
       (A slot whose cells turn out full after all just lets the scan go
       on; ChrItemRoomCheck has already ruled that out.)
   So a mask-bit-15 item never lands in the cells of a member other than
   ids 1 and 0xB while the shared list has a cell to trade for it.

   Nobody took it: if event flag 0x2D6 is set (StoryBitCheck) the item is
   remembered in g_ChrItemNoRoomItem. Then, by g_ChrItemNoRoomState:
     0     -> g_MessageRow = 0x14, MessageStringDisp(g_ChrItemNoRoomString, 1) (a field
              message), MenuRun(0x16) (the field menu on page 0x16,
              where the player makes room -- menu-ui.md), and the result
              is 1 when flag 0x2D6 is clear afterwards, else 0;
     else  -> ChrItemWaitingAdd(item) queues it in the first free of the eight
              cells at g_ChrItemWaitingList; return 1.

   No RNG draw and no frame wait of its own; the only frames are inside
   MessageStringDisp and MenuRun on the nobody-took-it path.

   Shape: who / item are u16 / u8 parameters (the raw argument registers
   are kept and masked at each use), id is an int. ChrItemListSort's (u16)
   prototype is in scope: the member arm's argument keeps its `andi`, the
   list arm's loses it. The member's cell is ((u8 *)(id * 0x40 + base) +
   j)[0x24] -- `[j + 0x24]` on the same base swaps the addu's operands, and
   base[id * 0x40 + j + 0x24] strength-reduces the row and the cell into
   one register. The tail is `if (F70 == 0) { …; if (flag == 0) return 1; }
   else { …; return 1; } return 0;`: loop.c moves the list arm's exit block
   to the barrier in front of that last `return 0`, which makes it the last
   `return 1` in the function, the copy cross-jumping keeps. */
/* func_800577C4 */
s32 ChrItemGive(u16 who, u8 item)
{
    s32 i;
    s32 j;
    s32 id;
    s32 keep;
    u8 *p;
    u8 t;

    keep = ChrItemUseGet(0x10, item, 1);
    for (i = 0; i < g_PartyCount; i++) {
        id = ((u16 *)g_PartyIds)[i];
        if (id >= 8 && id < 11) {
            continue;
        }
        if (who != 0 && id != who) {
            continue;
        }
        if (ChrItemRoomCheck(id) == 0) {
            continue;
        }
        if (id == 0xC) {
            for (j = 0; j < 0x60; j++) {
                if (g_ChrItemList0C[j] == 0) {
                    g_ChrItemList0C[j] = item;
                    ChrItemListSort(id);
                    StoryBitClear(0x2D6);
                    return 1;
                }
            }
        } else {
            for (j = 0; j < 6; j++) {
                if (((u8 *)(id * 0x40 + g_ChrRecords) + j)[0x24] == 0) {
                    if (keep == 0 && id != 1 && id != 0xB) {
                        for (p = g_ChrItemList0C + 0x5F; p >= g_ChrItemList0C; p--) {
                            if (ChrItemUseGet(0x10, *p, 1) != 0) {
                                t = *p;
                                *p = item;
                                item = t;
                                ChrItemListSort(0xC);
                                break;
                            }
                        }
                    }
                    ((u8 *)(id * 0x40 + g_ChrRecords) + j)[0x24] = item;
                    ChrItemListSort(id);
                    StoryBitClear(0x2D6);
                    return 1;
                }
            }
        }
    }
    if (StoryBitCheck(0x2D6) != 0) {
        g_ChrItemNoRoomItem = item;
    }
    if (g_ChrItemNoRoomState == 0) {
        g_MessageRow = 0x14;
        MessageStringDisp(g_ChrItemNoRoomString, 1);
        MenuRun(0x16);
        if (StoryBitCheck(0x2D6) == 0) {
            return 1;
        }
    } else {
        ChrItemWaitingAdd(item);
        return 1;
    }
    return 0;
}

/* func_80057A18 */
void ChrItemWaitingAdd(u8 item) {
    s32 i;
    u8 *waiting_item;

    i = 0;
loop_1:
    waiting_item = (u8 *)g_ChrItemWaitingList + i;
    i += 1;
    if (*waiting_item == 0) {
        *waiting_item = item;
        return;
    }
    if (i >= 8) {
        return;
    }
    goto loop_1;
}

s32 ChrWornItemRemove(s32 chr_id, s32 worn_place);
void ChrItemListGapClose(u16 list, u8 cell);

/* The INVENTORY REMOVE, twin of ChrItemGive (the
   add) above: take ONE copy of `item` away from member `who`, or with
   who == 0 from wherever the first copy is found. Callers: the shop's sell
   (ShopDealRun), the item use MenuUseRun(who, item, target) once the
   item's effect has taken, the item pages (MenuItemGiveStep Give,
   MenuItemDropStep, MenuWornItemChange) and the script command ScriptCommandChrItemRemove
   (who == 0). Returns 1 when a copy was removed, 0 when nobody searched
   holds one.

   Search order -- every list is walked from its LAST cell DOWN to cell 0,
   so the copy removed is the last one in the first list that has any:

   1. who == 0 only: the eight pending cells at g_ChrItemWaitingList (the queue
      ChrItemGive's "nobody took it" path feeds), cell 7 down to 0.
      Found: the cell is zeroed, ChrItemListGapClose(0xD, cell), return 1.
   2. Each field-party slot i < g_PartyCount in order, id =
      ((u16 *)g_PartyIds)[i]:
        - ids 8, 9 and 10 are skipped;
        - with who != 0 only the slot whose id == who is looked at;
      id == 0xC, the shared list (0x60 cells at g_ChrItemList0C): cell 0x5F
        down to 0. Found: zeroed, ChrItemListGapClose(0xC, cell), return 1.
      any other id, its 0x40 record in g_ChrRecords:
        a. the six CARRIED cells +0x24..+0x29, cell 5 down to 0. Found:
           zeroed, ChrItemListGapClose(id, cell), return 1.
        b. then the six WORN cells +0x1E..+0x23, cell 5 down to 0. Found:
           ChrWornItemRemove(id, cell + 1) (the unequip: it zeroes the worn
           cell itself and hands the old item to ChrWornItemValuesRemove, which
           takes the item's two stat bonuses back off the record and
           clears the item's event flag), return 1. Nothing is written to
           the worn cell here.
      A slot without the item lets the scan go on to the next slot.

   What a removal leaves behind: ChrItemListGapClose(list, cell) closes the gap.
   From cell + 1 up it copies each cell down one place, stopping after it
   has copied an empty one (or at the list's end: 8, 0x60 or 6 cells), and
   zeroes the last cell it moved from. The order of what remains is kept;
   nothing is re-sorted (ChrItemListSort, the add's sort, is not called).
   The worn cells are not compacted: an unequipped slot just stays 0.

   No frame wait and no random draw, here or in the callees.

   Shape notes. The three ChrItemListGapClose calls are written whole, one per
   find; cross-jumping merges them into the target's single jal with the
   list id set in three delay slots. The callee's prototype above is (u16
   list, u8 cell) -- its own first two instructions mask $a0 with 0xFFFF
   and $a1 with 0xFF -- and that prototype is where the target's two andi
   at the call come from. Called undeclared with no masks: 89/101.
   `who` is a u16 and `item` a u8 PARAMETER -- the target keeps both raw
   and masks them at each use, as in the twin. */
/* func_80057A54 */
s32 ChrItemRemove(u16 who, u8 item)
{
    s32 i;
    s32 j;
    s32 id;
    u8 *p;

    if (who == 0) {
        for (j = 7; j >= 0; j--) {
            p = (u8 *)g_ChrItemWaitingList + j;
            if (*p == item) {
                *p = 0;
                ChrItemListGapClose(0xD, j);
                return 1;
            }
        }
    }
    for (i = 0; i < g_PartyCount; i++) {
        id = ((u16 *)g_PartyIds)[i];
        if (id >= 8 && id < 11) {
            continue;
        }
        if (who != 0 && id != who) {
            continue;
        }
        if (id == 0xC) {
            for (j = 0x5F; j >= 0; j--) {
                p = g_ChrItemList0C + j;
                if (*p == item) {
                    *p = 0;
                    ChrItemListGapClose(0xC, j);
                    return 1;
                }
            }
        } else {
            for (j = 5; j >= 0; j--) {
                p = (u8 *)(id * 0x40 + g_ChrRecords) + j;
                if (p[0x24] == item) {
                    p[0x24] = 0;
                    ChrItemListGapClose(id, j);
                    return 1;
                }
            }
            p = (u8 *)(g_ChrRecords + (id << 6)) + 0x1E;
            for (j = 5; j >= 0; j--) {
                if (p[j] == item) {
                    ChrWornItemRemove(id & 0xFFFF, (j + 1) & 0xFFFF);
                    return 1;
                }
            }
        }
    }
    return 0;
}

extern long HWD0, VWD0;

/* func_80057BE8 */
s32 ChrItemRoomCheck(u16 who)
{
  s32 i;
  s32 j;
  s32 id;
  {
    for (i = 0; i < g_PartyCount; i++)
    {
      id = ((u16 *) g_PartyIds)[i];
      if ((id >= 8) && (id < 11))
      {
        continue;
      }
      if ((who != 0) && (id != who))
      {
        continue;
      }
      if (id == 0xC)
      {
        for (j = 0; j < 0x60; j++)
        {
          if (g_ChrItemList0C[j] == 0)
          {
            return 1;
          }
        }
      }
      else
      {
        for (j = 0; j < 6; j++)
        {
          if ((((u8 *) ((id * 0x40) + g_ChrRecords)) + j)[0x24] == 0)
          {
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

s32 ChrItemWaitingCheck(void);  /* TU defines this below */

/* ChrItemAnyCheck(who) -- "Is there an item to pick": the twin of ChrItemRoomCheck (which looks for an EMPTY carried
   cell), looking for any FILLED cell instead.

   Each field-party slot i < g_PartyCount in order, id =
   ((u16 *)g_PartyIds)[i]:
     - ids 8, 9 and 10 are skipped;
     - with who != 0 only the slot whose id == who is looked at;
   id == 0xC, the shared list (0x60 cells at g_ChrItemList0C): a non-zero cell
     answers 1.
   any other id, its 0x40 record in g_ChrRecords: the twelve cells
     +0x1E..+0x29 in one run -- the six WORN cells +0x1E..+0x23, then the
     six CARRIED cells +0x24..+0x29 -- a non-zero cell answers 1.
   A slot with nothing lets the scan go on to the next slot.

   When no slot answers (also with an empty party, and for a who that is
   not in the party), the answer is ChrItemWaitingCheck()'s: 1 when any of the
   eight pending cells at g_ChrItemWaitingList is non-zero, else 0. So a member with
   nothing worn and nothing carried still answers 1 while the pending
   queue holds an item.

   Callers, both in menu_page.c: MenuPresetKindStep (the field menu's kind
   row: kind 4 opens the item picker only when this answers non-zero) and
   MenuPresetDisp (the grey flag of that row's highlighted entry).

   Reads only; no frame wait and no random draw, here or in the callee.

   Shape notes. The record's cells go through a pointer local taken as
   (u8 *)(g_ChrRecords + (id << 6)) + 0x1E, the table base first --
   ChrItemRemove's spelling for its worn-cell loop. The twin's indexed
   spelling (((u8 *)(id * 0x40 + g_ChrRecords)) + j)[0x1E] is 34/59, and the
   pointer local with the product first 43/59. The tail is
   `return ChrItemWaitingCheck();` (the call's $v0 is the result as it comes);
   an `if (...) return 1; return 0;` around it is 57/59. `who` is a u16
   PARAMETER: with no call inside the loop its mask is done once, ahead of
   it, with the constant 0xC and the two list pointers. */
/* func_80057CB8 */
s32 ChrItemAnyCheck(u16 who)
{
    s32 i;
    s32 j;
    s32 id;
    u8 *p;

    for (i = 0; i < g_PartyCount; i++) {
        id = ((u16 *)g_PartyIds)[i];
        if (id >= 8 && id < 11) {
            continue;
        }
        if (who != 0 && id != who) {
            continue;
        }
        if (id == 0xC) {
            for (j = 0; j < 0x60; j++) {
                if (g_ChrItemList0C[j] != 0) {
                    return 1;
                }
            }
        } else {
            p = (u8 *)(g_ChrRecords + (id << 6)) + 0x1E;
            for (j = 0; j < 0xC; j++) {
                if (p[j] != 0) {
                    return 1;
                }
            }
        }
    }
    return ChrItemWaitingCheck();
}

extern long HWD0;
extern long VWD0;
s32 ChrItemWaitingGive(void);  /* TU defines this below */
s32 ChrWornItemValuesAdd(s32, s32);
void ChrWornItemSet(s32 chr_id, s32 worn_place, s8 item);  /* TU defines this below */

/* func_80057DA4 */
s32 ChrItemWaitingCheck(void)
{
  s32 i;
  u8 *waiting_items;
  i = 0;
  waiting_items = (u8 *) g_ChrItemWaitingList;
  do
  {
    if (waiting_items[i] != 0)
    {
      return 1;
    }
    if (waiting_items)
    {
      i += 1;
    }
    else
    {
      i += 1;
    }
  }
  while (i < 8);
  return 0;
}

s32 ChrItemGive(u16, u8);

/* func_80057DE0 */
s32 ChrItemWaitingGive(void)
{
  s32 i;
  u8 item;
  i = 7;
  loop_1:
  item = *((s32 *) (g_ChrItemWaitingList + i));

  if (item != 0)
  {
    ChrItemGive(0, *((s32 *) (g_ChrItemWaitingList + i)));
    *((u8 *) (g_ChrItemWaitingList + i)) = 0;
  }
  else
  {
    i -= 1;
    if (i >= 0)
    {
      goto loop_1;
    }
  }
  return i == 0;
}

/* The INVENTORY HAS-ITEM test: 1
   when `item` is held, else 0 -- the lookup beside ChrItemGive (the add)
   and ChrItemRoomCheck (the has-room test), over the same three stores.

   who:       0 = anybody in the field party; 0xC = the shared list only;
              any other id = that member only.
   worn_only: 0 = carried items and equipment both count; non-zero = only
              the equipment bytes are looked at.

   1. worn_only == 0 and who is 0 or 0xC: the shared list, 0x60 cells at
      g_ChrItemList0C -- a cell == item answers 1.
   2. who == 0xC: answer 0 (the list is not a member: it has no record in
      g_ChrRecords).
   3. worn_only == 0: for each field-party slot i < g_PartyCount, id =
      ((u16 *)g_PartyIds)[i], skipping id 0xC and, with who != 0, every id
      but who: the member's six carried cells, bytes +0x24..+0x29 of its
      0x40 record in g_ChrRecords -- a cell == item answers 1.
   4. always: the same walk again over the record's six EQUIPMENT bytes,
      tested from +0x23 DOWN to +0x1E -- a byte == item answers 1.
   Unlike the add and the has-room test, ids 8, 9 and 10 are NOT skipped.
   item 0 is not special-cased: asking for 0 answers "is a cell (or an
   equipment byte) empty".

   Callers: ScriptCommandChrItemHeldJump (who 0; item and worn_only are the command's two
   operand bytes; on 1 it puts g_ScriptCommandPointer back to its value at entry and
   calls ScriptCommandLabelJump) and the command set-up page's rows (MenuPresetDisp,
   MenuPresetMemberDisp: an item entry is greyed when (who, id, 0) answers 0).

   No call, no store, no RNG draw, no frame wait.

   Shape: who / item are u16 / u8 parameters (the raw argument registers
   are kept and masked at each use). Everything after the list scan sits
   inside `if (who != 0xC) { ... }` with ONE `return 0` at the end: written
   as an early `if (who == 0xC) return 0;` the test keeps a return of its
   own and loop.c moves every loop's `return 1` block behind it (90/109).
   The six equipment tests are six separate `if (...) return 1;`
   statements: as an `||` chain (88/109) the walk is 37 insns at loop time
   and the g_PartyCount bound is hoisted into a register; with six return
   blocks still inside it is 47, the hoist is "not desirable", and the
   bound is reloaded every pass as the target does. */
/* func_80057E48 */
s32 ChrItemHeldCheck(u16 who, u8 item, s32 worn_only)
{
    s32 i;
    s32 j;
    s32 id;
    u8 *p;

    if (worn_only == 0 && (who == 0 || who == 0xC)) {
        for (j = 0; j < 0x60; j++) {
            if (g_ChrItemList0C[j] == item) {
                return 1;
            }
        }
    }
    if (who != 0xC) {
        if (worn_only == 0) {
            for (i = 0; i < g_PartyCount; i++) {
                id = ((u16 *)g_PartyIds)[i];
                if (id == 0xC) {
                    continue;
                }
                if (who != 0 && who != id) {
                    continue;
                }
                for (j = 0; j < 6; j++) {
                    if (((u8 *)(id * 0x40 + g_ChrRecords) + j)[0x24] == item) {
                        return 1;
                    }
                }
            }
        }
        for (i = 0; i < g_PartyCount; i++) {
            id = ((u16 *)g_PartyIds)[i];
            if (id == 0xC) {
                continue;
            }
            if (who != 0 && who != id) {
                continue;
            }
            p = (u8 *)(id * 0x40 + g_ChrRecords);
            if (p[0x23] == item) {
                return 1;
            }
            if (p[0x22] == item) {
                return 1;
            }
            if (p[0x21] == item) {
                return 1;
            }
            if (p[0x20] == item) {
                return 1;
            }
            if (p[0x1F] == item) {
                return 1;
            }
            if (p[0x1E] == item) {
                return 1;
            }
        }
    }
    return 0;
}

s32 ChrWornItemValuesAdd(s32, s32);

/* func_80057FFC */
void ChrWornItemSet(s32 chr_id, s32 worn_place, s8 item)
{
  s32 record_offset;
  s32 masked_chr_id;
  masked_chr_id = chr_id & 0xFFFF;
  record_offset = masked_chr_id << 6;
  *((u8 *) (((u8 *) ((worn_place & 0xFFFF) + (g_ChrRecords + record_offset))) + 0x1d)) = item;
  ChrWornItemValuesAdd(masked_chr_id, item & 0xFF);
}
