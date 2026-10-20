/* layer.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef LAYER_H
#define LAYER_H

#include "layout.h"

/* One of the four 0x24-byte layer records the layer_ functions (layer_col.c) keep,
   each the colour fade, the mix and the order of one layer:
   g_LayerBgFirstRecord (0x800AE16C, mask bit 4, the bg's first layer),
   g_LayerBgSecondRecord (0x800AE190, bit 8, the bg's second layer),
   g_LayerSpriteRecord (0x800AE1B4, bit 0x100, the sprites' colour),
   g_LayerFlatRecord (0x800AE1D8, bit 0x80, the flat rectangle over the
   screen). A fade runs value from start toward target over length steps; only
   the first and the flat records use all three channels, the second and the
   sprite records channel 0. */
typedef struct {
    /* +0x00 the fade's step: layer_col_fade_record_set sets 1 when it starts a
       fade; layer_col_fade_step works the values out for it and counts it up
       while it is not above length; layer_col_fade_check answers 1 (a fade is
       running) while one record's step is not above its length. */
    s16 step;
    /* +0x02 the fade's length in steps, layer_col_fade_record_set's argument;
       read by layer_col_fade_step and layer_col_fade_check. */
    s16 length;
    /* +0x04..+0x09 the fade's start, three channels: layer_col_fade_record_set
       copies the old target here. */
    s16 start[3];
    /* +0x0A..+0x0F the value now, three channels: layer_col_fade_step sets
       start + difference * step / length; layer_col_apply multiplies the map's
       colour by it (the first record's three channels to bg_col_set, the
       second record's channel 0 to bg_second_col_set, the sprite record's
       channel 0 to sprite_col_set); BtlBgGreySet takes the first record's
       channel 0 in a battle; the flat record's three go to layer_flat_draw
       while one is not 0. */
    s16 value[3];
    /* +0x10..+0x15 the fade's target, three channels:
       layer_col_fade_record_set stores the values it is given here (through
       layer_col_conv, or cut to 0..0xFF for the flat record). */
    s16 target[3];
    /* +0x16..+0x1B target less start, three channels, which
       layer_col_fade_step spreads over the steps (layer_col_fade_record_set). */
    s16 difference[3];
    /* +0x1C the layer's mix level (first and second records):
       layer_mix_fade_start stores a level below 0x20 here; layer_mix_mode_get
       picks the mix mode from the second record's (0xE to 0x12 mode 1, 1 to
       0x1F mode 5); layer_mix_weights_get gives the two weights from it and
       0x20 less it. */
    s16 mix;
    /* +0x1E the layer's order number (first and second records):
       layer_order_set stores order & 0xFF; layer_mix_mode_get answers mode 3
       when the first record's is below the second's. */
    s16 order;
    /* +0x20 1 while the layer has a mix level below 0x20 (first and second
       records): layer_mix_fade_start sets 1 when it stores a level and 0 for a
       level of 0x20 or more. Nothing found reads it. */
    s16 mix_on;
    /* +0x22 nothing reads or writes it. */
    s16 pad_22;
} LayerRecord;

LAYOUT_SIZE(LayerRecord, 0x24);
LAYOUT_AT(LayerRecord, step, step, 0x00);
LAYOUT_AT(LayerRecord, length, length, 0x02);
LAYOUT_AT(LayerRecord, start, start, 0x04);
LAYOUT_AT(LayerRecord, value, value, 0x0a);
LAYOUT_AT(LayerRecord, target, target, 0x10);
LAYOUT_AT(LayerRecord, difference, difference, 0x16);
LAYOUT_AT(LayerRecord, mix, mix, 0x1c);
LAYOUT_AT(LayerRecord, order, order, 0x1e);
LAYOUT_AT(LayerRecord, mix_on, mix_on, 0x20);
LAYOUT_AT(LayerRecord, pad_22, pad_22, 0x22);

#endif
