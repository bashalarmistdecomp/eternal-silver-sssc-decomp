/* tile.h: written from the readings of 2026-10-14 (docs/naming.md,
   "Structs and their fields"). */
#ifndef TILE_H
#define TILE_H

#include "layout.h"

/* One eight-byte seed row of D_800B01B4 (48 rows): a tile animation as a set
   starts it. tile_animation_set copies a set's rows whole into the first bytes
   of g_TileAnimations' records. */
typedef struct {
    /* +0x00 the animation's mode (1 in every row of the image);
       tile_animation_set copies it to the record's mode. */
    u8 mode;
    /* +0x01 frames between two steps. */
    u8 delay;
    /* +0x02 steps before the frame counter wraps. */
    u8 frames;
    /* +0x03 tiles sent each step; tile_animation_set copies it to the record's
       count word; tile_animation_step steps the source by frame * count. */
    u8 count;
    /* +0x04 the first tile written (tile_image_load's number); copied to the
       record's tile word. */
    u16 tile;
    /* +0x06 the first source tile, 0x100 bytes a tile from D_800B8AFC. */
    u16 source;
} TileAnimationSeed;

LAYOUT_SIZE(TileAnimationSeed, 0x08);
LAYOUT_AT(TileAnimationSeed, mode, mode, 0x00);
LAYOUT_AT(TileAnimationSeed, delay, delay, 0x01);
LAYOUT_AT(TileAnimationSeed, frames, frames, 0x02);
LAYOUT_AT(TileAnimationSeed, count, count, 0x03);
LAYOUT_AT(TileAnimationSeed, tile, tile, 0x04);
LAYOUT_AT(TileAnimationSeed, source, source, 0x06);

/* One of the four tile-animation records at g_TileAnimations (0x800E4778): a
   seed row and its running state; tile_animation_step steps it and marks it
   pending, tile_animation_load sends its tiles. The glossary's tile animation. */
typedef struct {
    /* +0x00..+0x07 the seed row (TileAnimationSeed), copied whole by
       tile_animation_set; tile_animation_step reads its delay, frames, count
       and source. */
    TileAnimationSeed seed;
    /* +0x08 frames since the last step; tile_animation_set sets 0. */
    u8 timer;
    /* +0x09 the step the animation is on. */
    u8 frame;
    /* +0x0A 0 stopped (tile_animation_step passes it over), else the seed's
       mode; tile_animation_set zeroes all four first. */
    u8 mode;
    /* +0x0B 1 when the step's tiles are to be sent: tile_animation_step sets
       it, tile_animation_load sends and clears it. */
    u8 pending;
    /* +0x0C the address the tiles are sent from: tile_animation_step sets
       D_800B8AFC + (source + frame * count) * 0x100 at each step
       (tile_animation_set does not write it). */
    s32 source;
    /* +0x10 the first tile written, the seed's tile. */
    s32 tile;
    /* +0x14 the number of tiles sent, the seed's count. */
    s32 count;
} TileAnimation;

LAYOUT_SIZE(TileAnimation, 0x18);
LAYOUT_AT(TileAnimation, seed, seed, 0x00);
LAYOUT_AT(TileAnimation, seed_mode, seed.mode, 0x00);
LAYOUT_AT(TileAnimation, seed_delay, seed.delay, 0x01);
LAYOUT_AT(TileAnimation, seed_frames, seed.frames, 0x02);
LAYOUT_AT(TileAnimation, seed_count, seed.count, 0x03);
LAYOUT_AT(TileAnimation, seed_tile, seed.tile, 0x04);
LAYOUT_AT(TileAnimation, seed_source, seed.source, 0x06);
LAYOUT_AT(TileAnimation, timer, timer, 0x08);
LAYOUT_AT(TileAnimation, frame, frame, 0x09);
LAYOUT_AT(TileAnimation, mode, mode, 0x0a);
LAYOUT_AT(TileAnimation, pending, pending, 0x0b);
LAYOUT_AT(TileAnimation, source, source, 0x0c);
LAYOUT_AT(TileAnimation, tile, tile, 0x10);
LAYOUT_AT(TileAnimation, count, count, 0x14);

/* One of the three strips of the tile layer's wave at g_TileWaveStrips
   (0x800E0A40): a slanted strip of a 16-row tile, the shift of its top edge
   and of its bottom edge, its first row and its number of rows (0: not drawn).
   tile_wave_strips_make fills them, tile_layer_draw draws each tile as one
   textured four-cornered polygon per strip. */
typedef struct {
    /* +0x00 the x shift of the strip's top edge, the row shift
       (g_TileWaveRowShifts) of its first row: tile_wave_strips_make writes it;
       tile_layer_draw adds it to the polygon's two top corners. */
    s8 top_shift;
    /* +0x01 the x shift of the strip's bottom edge (the row shift of the row
       after its last); tile_layer_draw adds it to the two bottom corners. */
    s8 bottom_shift;
    /* +0x02 the strip's first row of the tile (0, or a row where the wave
       turns): tile_layer_draw draws from y + first_row and takes v from it. */
    s8 first_row;
    /* +0x03 the strip's number of rows, 0 for a strip that is not drawn:
       tile_wave_strips_make zeroes all three then fills two or three;
       tile_layer_draw passes over a strip whose count is 0. */
    s8 row_count;
} TileWaveStrip;

LAYOUT_SIZE(TileWaveStrip, 0x04);
LAYOUT_AT(TileWaveStrip, top_shift, top_shift, 0x00);
LAYOUT_AT(TileWaveStrip, bottom_shift, bottom_shift, 0x01);
LAYOUT_AT(TileWaveStrip, first_row, first_row, 0x02);
LAYOUT_AT(TileWaveStrip, row_count, row_count, 0x03);

/* TileResourceRow: one 4-byte row of g_TileResourceRows (0x800AE128, never
   written): the resource of LUNADATA.FIL that holds the tile layer's data
   for an effect's number. 16 rows in the image (numbers 8, 0xB, 0x1D, 0x1E,
   0x27, 0x2A, 0x2B, 0x2C, 0x2F and 0x50 to 0x56, resources 0x1D8 to 0x1E7
   in that order), then a row beginning 0xFF. tile_resource_load walks the
   rows to the one of its number (BtlEffectLoad's), reads its resource to
   0x80156000 and runs its commands. */
typedef struct {
    /* +0x00 the effect's number tile_resource_load looks for (its loop has
       no other end). */
    u8 effect;
    /* +0x01 nothing reads it. */
    u8 pad_01;
    /* +0x02 the resource's number, which tile_resource_load hands
       resource_load. */
    u16 resource;
} TileResourceRow;

LAYOUT_SIZE(TileResourceRow, 0x04);
LAYOUT_AT(TileResourceRow, effect, effect, 0x00);
LAYOUT_AT(TileResourceRow, pad_01, pad_01, 0x01);
LAYOUT_AT(TileResourceRow, resource, resource, 0x02);

#endif
