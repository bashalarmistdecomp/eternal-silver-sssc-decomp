/* shop.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef SHOP_H
#define SHOP_H

#include "layout.h"

/* One of the shop's twelve list entries (two rows of six) at the address in
   g_ShopListEntries (0x90 bytes, ShopAlloc): on, the column and row of its
   icon on the text grid, the item number shown. ShopOpen gives each its place,
   ShopListSet fills them from the stock or from what the member carries,
   ShopListDisp puts the icons up; the cursor's entry (g_ShopCursors[1]) is
   read by ShopItemDisp, ShopItemTextDisp, ShopItemStep, ShopDealRun,
   ShopDealStep, ShopItemStatsDisp and ShopListCursorAdjust. */
typedef struct {
    /* +0x00 1 when the entry shows an item, 0 when it is empty: ShopListSet
       writes it; ShopListDisp draws the icon only when it is not 0 and clears
       the 3 by 3 cells otherwise; ShopListCursorAdjust moves the cursor onto
       an entry that is on; ShopItemStep and ShopItemTextDisp test the
       cursor's. */
    u16 on;
    /* +0x02 the column of the entry's icon on the text grid: ShopOpen sets (i
       % 6) * 4 + 0xC; ShopListDisp hands it to IconCellSet and
       BtlCellAreaClearCopy. */
    u16 column;
    /* +0x04 the row of the entry's icon: ShopOpen sets (i / 6) * 4 + 0xB;
       ShopListDisp. */
    u16 row;
    /* +0x06 the number (from 1) of the item the entry shows: ShopListSet
       writes it from the stock's or the member's bytes; ShopListDisp shows its
       icon; ShopItemDisp takes the item's row from it (g_ShopItemRow);
       ShopDealRun gives, wears or takes the item by it; ShopItemTextDisp,
       ShopItemStatsDisp and the test for item 0xB5 read it. */
    u16 number;
    /* +0x08 a word ShopOpen sets to 0x200000 in every entry; nothing found
       reads it (ShopListDisp hands IconCellSet 0 as its last argument). What
       it was for is not stated. */
    s32 unk_08;
} ShopListEntry;

LAYOUT_SIZE(ShopListEntry, 0x0c);
LAYOUT_AT(ShopListEntry, on, on, 0x00);
LAYOUT_AT(ShopListEntry, column, column, 0x02);
LAYOUT_AT(ShopListEntry, row, row, 0x04);
LAYOUT_AT(ShopListEntry, number, number, 0x06);
LAYOUT_AT(ShopListEntry, unk_08, unk_08, 0x08);

/* A place's entry of the shop's line-up, four bytes for each of the
   g_ShopPlaceCount places: the wanted ones at the address in
   g_ShopWantedEntries (ShopAlloc, 0x30 bytes) and the current ones 0x18 bytes
   on at g_ShopCurrentEntries. The writers set the wanted entries (ShopOpen,
   ShopBuySellOpen, ShopMemberDisp, ShopItemLineUpSet, ShopMemberLineUpSet);
   ShopLineUpMove walks each lined-up record to its wanted row, poses it and
   copies the wanted entry over the current one. */
typedef struct {
    /* +0x00 the place's row in cells of 8 pixels below the line-up's base:
       ShopLineUpMove walks the record to y = g_ShopOrigin[1] + (row + 4) * 8
       (x unchanged), and first puts it on its walk when the wanted row is not
       the current one. ShopOpen 0; ShopBuySellOpen 0 for everyone;
       ShopMemberDisp, ShopItemLineUpSet and ShopMemberLineUpSet 1 for the
       member under the cursor, 0 for the others. */
    u8 row;
    /* +0x01 the pose ShopLineUpMove gives the record when its walk is done
       (MapAnimationPoseSet(record, pose, facing)): ShopOpen 1; ShopMemberDisp
       and ShopMemberLineUpSet 1 for the member under the cursor, 2 for the
       others; ShopItemLineUpSet 1 for a member who may wear the item (or for
       an item that is not worn), 2 for one who may not. */
    u8 pose;
    /* +0x02 the facing ShopLineUpMove gives the record for its walk and its
       pose (MapAnimationPoseSet's third argument): 5 from every writer, except
       ShopItemLineUpSet's 1 for a member with no room for the item
       (ChrItemRoomCheck not 1). */
    u8 facing;
    /* +0x03 nothing reads or writes it on its own; it goes with
       ShopLineUpMove's whole-entry copy. */
    u8 pad_03;
} ShopLineUpEntry;

LAYOUT_SIZE(ShopLineUpEntry, 0x04);
LAYOUT_AT(ShopLineUpEntry, row, row, 0x00);
LAYOUT_AT(ShopLineUpEntry, pose, pose, 0x01);
LAYOUT_AT(ShopLineUpEntry, facing, facing, 0x02);
LAYOUT_AT(ShopLineUpEntry, pad_03, pad_03, 0x03);

/* One shop's stock: one of the 0x28 six-byte rows of g_ShopStockRows
   (0x800A8468, never written). ShopRun sets the shop's row number,
   g_ShopStockRow; ShopStockGet returns the address of row n for an n below
   0x28 and of the last row, 0x27, for 0x28 and up. */
typedef struct {
    /* +0x00..+0x05 the numbers (from 1) of the items the shop sells, 0 for an
       empty place. ShopListSet (shop_screen.c) copies them into the first six
       entries of g_ShopListEntries as the entries' numbers (an entry whose
       byte is 0 is switched off). Data, never written. */
    u8 items[6];
} ShopStockRow;

LAYOUT_SIZE(ShopStockRow, 0x06);
LAYOUT_AT(ShopStockRow, items, items, 0x00);

#endif
