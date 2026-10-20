/* font.h: written from the readings of 2026-10-19 (docs/naming.md,
   "Structs and their fields"). */
#ifndef FONT_H
#define FONT_H

#include "layout.h"

/* FontLine: one of the eight 8-byte records at g_FontLines (0x800A7DC8, never
   written), a line of the font's text blocks: where in video memory its
   sixteen blocks of 16 x 16 pixels lie side by side (64 words by 16 rows),
   and the texture page a cell of kind 2 is drawn from. font_line_image_load
   sends a run of blocks to a line's place; font_cell_draw draws a cell from
   the line its word picks (bits 11 to 13). In the image the eight places are
   (0, 0x1C0), (0, 0x1D1), (0, 0x1E2), (0x40, 0x1C0), (0x40, 0x1D1),
   (0x40, 0x1E2), (0x3C0, 0x1DE) and (0x3C0, 0x1EF), with pages 0x10, 0x11 and
   0x1F. The glossary's line; docs/spec/text.md's label strip. */
typedef struct {
    /* +0x00 x of the line's place in video memory, in 16-bit words:
       font_line_image_load sends block k of the line to x + k * 4. Data,
       never written. */
    s16 x;
    /* +0x02 y of the line's place in video memory: font_line_image_load's
       rows; font_cell_draw reads its low byte as the texture's v (the place's
       row within its texture page). */
    s16 y;
    /* +0x04 the texture page word font_cell_draw hands packet_sprite_add for
       a cell of the line. */
    s16 tpage;
    /* +0x06 nothing reads or writes it. */
    s16 pad_06;
} FontLine;

LAYOUT_SIZE(FontLine, 0x08);
LAYOUT_AT(FontLine, x, x, 0x00);
LAYOUT_AT(FontLine, y, y, 0x02);
LAYOUT_AT(FontLine, tpage, tpage, 0x04);
LAYOUT_AT(FontLine, pad_06, pad_06, 0x06);

#endif
