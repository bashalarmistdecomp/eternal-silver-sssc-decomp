/* menu.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef MENU_H
#define MENU_H

#include "layout.h"

/* One of the 0x14 ten-byte entries of g_MenuOptionEntries (0x800B5A54, never
   written), the layout of the menu's option page: entry 0 the page's title,
   entries 1 to 6 its six rows, the rest the rows' values. MenuOptionOpen
   writes the rows' names, MenuOptionValuesPut a row's values,
   MenuOptionValueBoxesDraw a box behind each chosen value, MenuOptionTextDisp
   shows all 0x14 entries' text on the text grid. */
typedef struct {
    /* +0x00 the entry's column on the text grid, in cells of 8 pixels:
       MenuOptionTextDisp hands it to font_run_show; MenuOptionValueBoxesDraw
       puts the value's box at column * 8. Data, never written. */
    u8 column;
    /* +0x01 the entry's row on the text grid (font_run_show; the box at row *
       8, MenuOptionValueBoxesDraw). */
    u8 row;
    /* +0x02 the entry's length in text blocks: font_run_show's block count
       (MenuOptionTextDisp), font_target_clear's for a value's text
       (MenuOptionValuesPut); MenuOptionValuesPut also moves its pen on by the
       block_count of entry row * 2 + 5 + i after each value. */
    u8 block_count;
    /* +0x03 a width in pixels: the width of the box MenuOptionValueBoxesDraw
       puts behind a row's chosen value (0x12 high). */
    u8 width;
    /* +0x04 the entry's block in the menu's text: its text is written at
       0x801CD500 + block * 0x80 (MenuOptionOpen, MenuOptionValuesPut) and
       shown from block + 0x34 of 0x801CBB00, the same place
       (MenuOptionTextDisp). */
    u8 block;
    /* +0x05 nothing reads or writes it. */
    u8 pad_05;
    /* +0x06 the number of the entry's string, which
       font_string_proportional_put writes: the rows' names (MenuOptionOpen,
       entries 1 to 6) and the values (MenuOptionValuesPut). */
    u16 string;
    /* +0x08 in entries 1 to 6 (the rows): the number of the row's first value
       entry; the value shown for option byte n is entry first_value + n
       (MenuOptionValueBoxesDraw), and MenuOptionValuesPut writes entries
       first_value on. */
    u8 first_value;
    /* +0x09 in entries 1 to 6: how many value entries the row has
       (MenuOptionValuesPut writes that many). */
    u8 value_count;
} MenuOptionEntry;

LAYOUT_SIZE(MenuOptionEntry, 0x0a);
LAYOUT_AT(MenuOptionEntry, column, column, 0x00);
LAYOUT_AT(MenuOptionEntry, row, row, 0x01);
LAYOUT_AT(MenuOptionEntry, block_count, block_count, 0x02);
LAYOUT_AT(MenuOptionEntry, width, width, 0x03);
LAYOUT_AT(MenuOptionEntry, block, block, 0x04);
LAYOUT_AT(MenuOptionEntry, pad_05, pad_05, 0x05);
LAYOUT_AT(MenuOptionEntry, string, string, 0x06);
LAYOUT_AT(MenuOptionEntry, first_value, first_value, 0x08);
LAYOUT_AT(MenuOptionEntry, value_count, value_count, 0x09);

/* The table of the memory card's fifteen slots, g_MenuCardSlotFlags
   (0x800E08D8, 0x20 bytes), for the save and load pages' list. MenuCardRead
   and MenuCardListRead clear all of it before each read of the card;
   MenuCardListRead fills it, MenuSaveWrite marks the slot it writes, and the
   pages read it. Flags because each byte is 0 or 1, the last one a count. */
typedef struct {
    /* +0x00..+0x1D two runs of fifteen bytes, one byte a slot, which the code
       indexes as one array (flags[i] and flags[i + 0xF]; as two arrays of
       fifteen the loops of MenuSaveWrite and MenuCardListRead compute their
       addresses otherwise and the bytes change).
       flags[i] (+0x00..+0x0E) is 1 when slot i holds a save, 0 when it is
       empty: MenuCardListRead sets it for each slot whose header it reads
       (card_save_records_head_read answers 0), MenuSaveWrite for the slot it
       has written; MenuCardListDisp shows a save's level and time for a
       listed slot whose byte is neither 0 nor 0xFF, and MenuSaveWrite lists
       again the slots that hold a save.
       flags[i + 0xF] (+0x0F..+0x1D) is 1 when slot i can be picked: it holds a
       save, or it is empty and a free block is left for it (MenuCardListRead's
       second pass grants empty slots while fewer than g_MenuCardFreeBlockCount
       have been); MenuSaveWrite looks for the first empty slot whose byte is
       1. */
    u8 flags[30];
    /* +0x1E nothing reads or writes it but the clears of the whole table. */
    u8 pad_1e;
    /* +0x1F the count of the slots that can be picked (the 1s of the second
       run of flags): MenuCardListRead counts it up as it sets them.
       MenuSaveOpen puts the list's row at 0 when it is 0 and steps the page's
       stage by 2 when it is not; MenuSaveStep keeps the cursor within it and,
       with it at 0, answers a pick with sound 4. */
    u8 pickable_count;
} MenuCardSlotFlags;

LAYOUT_SIZE(MenuCardSlotFlags, 0x20);
LAYOUT_AT(MenuCardSlotFlags, flags, flags, 0x00);
LAYOUT_AT(MenuCardSlotFlags, pad_1e, pad_1e, 0x1e);
LAYOUT_AT(MenuCardSlotFlags, pickable_count, pickable_count, 0x1f);

/* MenuScreenBits: the 0x10 bytes at D_800E4E70 (up to g_MenuCardSlotHeads),
   of which only two 16-bit words are touched, each by changing bits in
   place as a menu screen opens and closes, always beside D_800B8EEC |= 1:
   the field menu's (MenuOpen, MenuClose, MenuNoWalkOpen), the shop's
   (ShopOpen, ShopClose) and the battle menu's (BtlMenuInit, BtlMenuOpen,
   BtlMenuFree). Nothing in the C or the image's other functions reads them,
   so what the words and their bits are for is not known (the register row
   of D_800E4E70). Named for its users, the menu screens, and for what they
   do to it. */
typedef struct {
    /* +0x00..+0x09 nothing reads or writes them. */
    u8 pad_00[10];
    /* +0x0A changed only by the battle's menu: BtlMenuOpen sets its bits
       0x30 to 0x10 and BtlMenuFree clears them, as for unk_0e. Nothing reads
       it. */
    u16 unk_0a;
    /* +0x0C..+0x0D nothing reads or writes them. */
    u8 pad_0c[2];
    /* +0x0E bits 0x30 are set to 0x10 as a menu screen opens (MenuOpen,
       MenuNoWalkOpen, ShopOpen, BtlMenuOpen, each beside a layer_mix_set of
       layer 0x10 in the field menu's) and cleared as it closes (MenuClose,
       ShopClose, BtlMenuFree); BtlMenuInit clears its bits 0xC. Nothing
       reads it. */
    u16 unk_0e;
} MenuScreenBits;

LAYOUT_SIZE(MenuScreenBits, 0x10);
LAYOUT_AT(MenuScreenBits, pad_00, pad_00, 0x00);
LAYOUT_AT(MenuScreenBits, unk_0a, unk_0a, 0x0a);
LAYOUT_AT(MenuScreenBits, pad_0c, pad_0c, 0x0c);
LAYOUT_AT(MenuScreenBits, unk_0e, unk_0e, 0x0e);

#endif
