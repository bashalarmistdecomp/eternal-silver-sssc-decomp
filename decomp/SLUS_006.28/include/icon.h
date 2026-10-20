/* icon.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef ICON_H
#define ICON_H

#include "layout.h"

/* IconSlot: one of the 20 slots of 8 bytes at g_IconSlots, each holding one
   icon's image in video memory for the cells of the text grid to draw from
   (ICON.C). */
typedef struct {
    /* +0x00 the x of the slot's image in its texture page, the texel position
       IconImageLoad writes with the icon; IconCellDraw draws the 24 x 24
       sprite from it. */
    s16 texel_x;
    /* +0x02 the y of the slot's image in its texture page; IconImageLoad
       writes it, IconCellDraw reads it. */
    s16 texel_y;
    /* +0x04 the number of the icon the slot holds, 0 for a free slot.
       IconImageLoad writes it; IconSlotReset sets 0 in all 20 and
       IconGarbageCollect in each slot no cell uses; IconSlotFind looks for a
       number and IconSlotFindFree for 0; IconCellDraw draws nothing for 0. */
    s16 icon;
    /* +0x06 the collector's mark: IconGarbageCollect sets it to 0 in all 20,
       to 1 in each slot a cell of kind 3 uses, and then frees the slots left
       at 0. */
    u16 mark;
} IconSlot;

LAYOUT_SIZE(IconSlot, 0x08);
LAYOUT_AT(IconSlot, texel_x, texel_x, 0x00);
LAYOUT_AT(IconSlot, texel_y, texel_y, 0x02);
LAYOUT_AT(IconSlot, icon, icon, 0x04);
LAYOUT_AT(IconSlot, mark, mark, 0x06);

#endif
