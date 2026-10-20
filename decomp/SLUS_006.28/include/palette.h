/* palette.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef PALETTE_H
#define PALETTE_H

#include "layout.h"

/* One three-byte row of the table at D_800B0334 (ended by an id of 0): the
   colour tables a palette-animation set copies to video memory row 508.
   palette_animation_copy_set walks it with the cursor
   g_PaletteAnimationCopyRow. */
typedef struct {
    /* +0x00 the palette-animation set the row is for; 0 ends the table.
       palette_animation_copy_set compares it with its argument and copies it
       to g_PaletteAnimationCopyId. */
    s8 id;
    /* +0x01 the first 16-colour table of the range, copied to
       g_PaletteAnimationCopyFirst (palette_update copies from it). */
    s8 first;
    /* +0x02 the number of 16-colour tables, copied to
       g_PaletteAnimationCopyCount. */
    s8 count;
} PaletteAnimationCopyRow;

LAYOUT_SIZE(PaletteAnimationCopyRow, 0x03);
LAYOUT_AT(PaletteAnimationCopyRow, id, id, 0x00);
LAYOUT_AT(PaletteAnimationCopyRow, first, first, 0x01);
LAYOUT_AT(PaletteAnimationCopyRow, count, count, 0x02);

/* One eight-byte seed row of D_800AFD9C: a palette animation as a set starts
   it. palette_animation_set copies a set's rows whole into the first bytes of
   g_PaletteAnimations' records. */
typedef struct {
    /* +0x00 the animation's mode: 1 a strip of frames copied in, 2 the colours
       turned one up, 3 one down; palette_animation_set copies it to the
       record's mode word. */
    u8 mode;
    /* +0x01 frames between two steps (palette_animation_step counts timer up
       to it). */
    u8 delay;
    /* +0x02 steps before the frame counter wraps. */
    u8 frames;
    /* +0x03 the first colour of the range, in row 1 of the colour buffer;
       palette_row_mark gets it | 0x100. */
    u8 first;
    /* +0x04 the number of colours in the range. */
    u16 count;
    /* +0x06 for mode 1 the first colour of the strip at D_800B8B04 (frame *
       count on from it). */
    u16 source;
} PaletteAnimationSeed;

LAYOUT_SIZE(PaletteAnimationSeed, 0x08);
LAYOUT_AT(PaletteAnimationSeed, mode, mode, 0x00);
LAYOUT_AT(PaletteAnimationSeed, delay, delay, 0x01);
LAYOUT_AT(PaletteAnimationSeed, frames, frames, 0x02);
LAYOUT_AT(PaletteAnimationSeed, first, first, 0x03);
LAYOUT_AT(PaletteAnimationSeed, count, count, 0x04);
LAYOUT_AT(PaletteAnimationSeed, source, source, 0x06);

/* One of the eight palette-animation records at g_PaletteAnimations
   (0x800E4808): a seed row and its running state, which palette_animation_step
   steps every frame. The glossary's palette animation. */
typedef struct {
    /* +0x00..+0x07 the seed row the set started it from
       (PaletteAnimationSeed), copied whole by palette_animation_set;
       palette_animation_step reads its delay, frames, first, count and source. */
    PaletteAnimationSeed seed;
    /* +0x08 frames since the last step: palette_animation_step counts it up to
       delay and sets 0; palette_animation_set sets 0. */
    u8 timer;
    /* +0x09 the step the animation is on, 0 again at frames; for mode 1 it
       picks the strip's frame. */
    u8 frame;
    /* +0x0A the record's mode word: 0 off (palette_animation_step passes it
       over), else the seed's mode, which palette_animation_set copies here;
       palette_animation_set zeroes all eight first. */
    u16 mode;
} PaletteAnimation;

LAYOUT_SIZE(PaletteAnimation, 0x0c);
LAYOUT_AT(PaletteAnimation, seed, seed, 0x00);
LAYOUT_AT(PaletteAnimation, seed_mode, seed.mode, 0x00);
LAYOUT_AT(PaletteAnimation, seed_delay, seed.delay, 0x01);
LAYOUT_AT(PaletteAnimation, seed_frames, seed.frames, 0x02);
LAYOUT_AT(PaletteAnimation, seed_first, seed.first, 0x03);
LAYOUT_AT(PaletteAnimation, seed_count, seed.count, 0x04);
LAYOUT_AT(PaletteAnimation, seed_source, seed.source, 0x06);
LAYOUT_AT(PaletteAnimation, timer, timer, 0x08);
LAYOUT_AT(PaletteAnimation, frame, frame, 0x09);
LAYOUT_AT(PaletteAnimation, mode, mode, 0x0a);

#endif
