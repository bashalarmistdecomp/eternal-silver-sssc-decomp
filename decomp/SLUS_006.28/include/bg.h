/* bg.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef BG_H
#define BG_H

#include "layout.h"

/* The 16 signed 16-bit words at g_BgAttributeSlotBlocks (0x1F800020, the CPU's
   scratchpad), one for each of the 16 slots of 256 attributes in
   secondary_tile_table (BgAttributeSlots): the number of the attribute map's
   16 x 16 block the slot holds, -1 for none. */
typedef struct {
    /* +0x00..+0x1F slot q's block number ((x >> 4) + (y & 0xF0)) or -1:
       bg_attribute_data_reset stores -1 in all 16, bg_attribute_block_load the
       block's number before it fills the slot, bg_attribute_set -1 in the slot
       of a block it changed; bg_attribute_copy, bg_attribute_get,
       MapAttributeFreeCheck, MapAttributeClassGet, MapAttributeFind and
       MapAttributeLineCheck compare it with the block they want, slot ((x >>
       4) & 3) | ((y >> 2) & 0xC), and call bg_attribute_block_load when it is
       another. */
    s16 blocks[16];
} BgAttributeSlotBlocks;

LAYOUT_SIZE(BgAttributeSlotBlocks, 0x20);
LAYOUT_AT(BgAttributeSlotBlocks, blocks, blocks, 0x00);

/* secondary_tile_table (0x800DBAC0): 16 slots of 256 attributes of the
   attribute map (16 x 16 attributes, one block, each), which
   bg_attribute_block_load fills from the attribute map's data and the
   attribute readers read through the slot numbers at g_BgAttributeSlotBlocks. */
typedef struct {
    /* +0x0000..+0x1FFF slot q's attribute of (tx, ty) at [(tx & 0xF) + ((ty &
       0xF) << 4) + (q << 8)]: bits 0x3800 a shape, 0x4000 solid, 0x8000 the
       class's cover, the low 11 bits an attribute's kind (bits 7 to 10) and
       number (MapAttributeFind). bg_attribute_block_load fills a slot (256
       copies of one word, 256 plain words, or lz_unpack), bg_attribute_set
       changes one; bg_attribute_copy, bg_attribute_get, MapAttributeFreeCheck,
       MapAttributeClassGet, MapAttributeFind and MapAttributeLineCheck read
       them. */
    u16 attributes[4096];
} BgAttributeSlots;

LAYOUT_SIZE(BgAttributeSlots, 0x2000);
LAYOUT_AT(BgAttributeSlots, attributes, attributes, 0x00);

/* BgBlockTable: the head of the bg's data (at g_BgBlockEntries) and of the
   attribute map's (at g_BgAttributeBlockEntries): one entry for each block of
   the 16 x 16, and the two limits that say how an entry is read; the blocks
   follow. */
typedef struct {
    /* +0x000 one 16-bit entry for each block, number (x >> 3) + (y >> 3) * 16:
       0 a block of zeros; below fill_limit, the one word at that place
       repeated; plain_limit or more, plain words at the base + entry; between,
       packed words for lz_unpack. bg_data_init and bg_attribute_data_init set
       all to 0; bg_tile_set and bg_attribute_set point one at a new block;
       bg_block_load and bg_attribute_block_load read them; a resource's
       command 5 or 6 copies new data over the whole. */
    u16 entry[256];
    /* +0x200 the first limit: an entry below it is a block of one word
       repeated. bg_data_init and bg_attribute_data_init set 0x10;
       bg_data_reset and bg_attribute_data_reset copy it to D_800B88C0 and
       D_800B88D8 for the block loaders. */
    u16 fill_limit;
    /* +0x202 the second limit: an entry of it or more is a block of plain
       words. The inits set 0x200; the resets copy it to D_800B88C4 and
       D_800B88DC and make it, rounded up to even, where the next block made
       goes (g_BgNextBlockEntry, g_BgAttributeNextBlockEntry). */
    u16 plain_limit;
} BgBlockTable;

LAYOUT_SIZE(BgBlockTable, 0x204);
LAYOUT_AT(BgBlockTable, entry, entry, 0x00);
LAYOUT_AT(BgBlockTable, fill_limit, fill_limit, 0x200);
LAYOUT_AT(BgBlockTable, plain_limit, plain_limit, 0x202);

#endif
