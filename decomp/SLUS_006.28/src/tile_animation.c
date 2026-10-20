/* Lunar SSSC — tile_animation: the tile animations: a set started, its
   frame, and the tiles' images loaded. The first split had it in
   rendering2.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "tile.h"
#include "pad.h"
#include "palette.h"
#include "chr.h"
#include "btl.h"
#include "sprite.h"
extern s32 g_TileAnimationTiles;
extern u8 g_TileAnimations[];
extern long HWD0;
extern long VWD0;

/* far_syms: g_TileAnimations */

extern u8 g_TileAnimationSets[];
extern TileAnimationSeed g_TileAnimationRows[];

/* Start animated-cell set `id` (0 = none): stop the four animated-cell
   records tile_animation_step steps every frame, then load the set's own. This is
   what fills g_TileAnimations (tile_animation_step steps the records, tile_animation_load
   uploads the ones marked pending). The one caller in the image is the scene
   set-up MapLoad (map_load.c), right after its palette_animation_set call;
   it passes byte +4 of its scene record and two more values, its $s3 and its
   $s1 (`move $a1, $s3` / `move $a2, $s1` at the call). This function reads
   neither. In order:
   1. All four 0x18-byte records of g_TileAnimations get mode (+0x0A) = 0; the
      compiled loop goes from record 3 down to record 0. Nothing else of a
      record is cleared, the pending flag (+0x0B) included.
   2. id == 0 returns here.
   3. The set's animations are loaded. g_TileAnimationSets holds one byte pair a set,
      {first seed row, count}, at [(id - 1) * 2] (35 pairs in the image, then
      a 0, 0 pair that pads the table to 0x48 bytes; counts 1 to 3, so the
      four records are never overrun); g_TileAnimationRows, directly behind it, holds
      the 48 eight-byte seed rows (up to g_PaletteAnimationCopyRows). Seed rows first ..
      first + count - 1 go to records 0 .. count - 1: the eight bytes as they
      are (a seed row is the head of a record), then timer (+0x08) = 0, frame
      (+0x09) = 0, mode (+0x0A) = the record's byte 0 (the seed's mode),
      pending (+0x0B) = 0, the first-cell word (+0x10) = the seed's u16 +4
      and the cell-count word (+0x14) = the seed's byte +3. The source
      address (+0x0C) is not written: tile_animation_step sets it at the record's
      first step. Records count .. 3 stay stopped. A count of 0 loads
      nothing.
   Nothing is returned (no value is set up in $v0). No callee, no frame
   wait, no RNG draw.
   The TU declares g_TileAnimations as bytes (tile_animation_step and tile_animation_load walk
   it with a pointer into each record, + 0xB and + 0x14), so the record type is a cast here.
   Shapes, each measured by changing it alone in this text. The pair lookup is
   palette_animation_set's: the index is computed back into the u8 parameter with
   `(id - 1) * 2` (52/53 with `<< 1`, 39/53 with a u8 local) and both bytes
   are read by index, first then count (45/53 count first), which makes two
   equal `id + table` sums, the second of them the target's
   `addu $a0, $a1, $zero` (palette_animation_set's comment has the mechanism; it was
   not read again in a dump here). Unlike palette_animation_set the second loop walks
   a record pointer that is set before the loop and stepped in the for header
   (indexed by i: 25/53; `r++` as the body's last statement: 46/53): the
   target forms the record table's address ahead of the pair reads and only
   the `+0x14` pointer after the loop's entry test. The mode is read back
   from the record (33/53 from the seed row), and the mode store is written
   before the pending store (52/53 the other way round), though the compiled
   order has it after. No return type (52/53 as `s32` with no return
   statement). The first loop matches counting up or down, the seed pointer
   stepped inside the copy statement or after it, and the record pointer set
   before the pair reads, after them or in the for header.
   MARKED SPELLING: the pointer local `tab`, as in palette_animation_set. The seed
   table's address is taken into it after the two byte reads and the seed
   pointer is `&tab[first]`; with `src = &g_TileAnimationRows[first];` the first sum
   is in $v1 and the second stays a sum: 46/53. Twenty-four other texts
   without the local, scored on an earlier spelling of the same logic (the
   sum written either way round, a byte-pointer sum, the seed pointer set to
   the table and then advanced, a u8 or u32 `first`, the seed row indexed
   straight from the pair byte, the two tables as one struct) are 41 to 46
   of 53. Whether the original had such a local the bytes do not say.
   MARKED SPELLING: the two parameters nothing reads. They are here because
   the one call site passes three values and map_load.c declares three
   (`s32 tile_animation_set(u8, s32, s32 *)`); the same 53/53 with `(u8 id)`
   alone, so these bytes do not say which the original had. */
/* func_800265BC */
void tile_animation_set(u8 id, s32 unused, s32 *unused2)
{
    s32 i;
    TileAnimation *r;
    TileAnimationSeed *src;
    TileAnimationSeed *tab;
    s32 n;
    s32 first;

    for (i = 0; i < 4; i++) {
        ((TileAnimation *)g_TileAnimations)[i].mode = 0;
    }
    if (id == 0) {
        return;
    }
    id = (id - 1) * 2;
    first = g_TileAnimationSets[id];
    n = g_TileAnimationSets[id + 1];
    tab = g_TileAnimationRows;
    src = &tab[first];
    r = (TileAnimation *)g_TileAnimations;
    for (i = 0; i < n; i++, r++) {
        r->seed = *src++;
        r->timer = 0;
        r->frame = 0;
        r->mode = r->seed.mode;
        r->pending = 0;
        r->tile = r->seed.tile;
        r->count = r->seed.count;
    }
}

/* far_syms: g_TileAnimations (target reaches it %hi/%lo) */
extern long HWD0, VWD0;

/* The loop's pointer is each TileAnimation's pending byte (+0xB), and the
   fields are reached through the record cast back from it (r, a cast
   macro for (TileAnimation *)(ptr - 0xB), not a local): the code steps a
   register holding the record's address + 0xB. Written with a record
   pointer local (stepped or indexed, or the global indexed by i), the code
   is the same but for that register, which then holds the record's own
   address (one instruction fewer). The start stays the sum + 0xB: as the
   pending member's address (&...->pending) the function changes size. */
/* func_80026690 */
void tile_animation_step(void)
{
#define r ((TileAnimation *)(ptr - 0xB))
  u8 *ptr;
  u8 *base;
  s32 i;
  s32 c;
  i = 0;
  c = 1;
  ptr = (base = g_TileAnimations) + 0xB;
  if (i < 4)
  {
    gl0:
    if (r->mode != 0)
    {
      r->timer++;
      if (r->timer >= r->seed.delay)
      {
        r->timer = 0;
        r->frame++;
        if (r->frame >= r->seed.frames)
        {
          r->frame = 0;
        }
        if (r->mode == c)
        {
          s32 val = r->seed.source + (r->frame * r->seed.count);
          r->source = g_TileAnimationTiles + (val << 8);
        }
        r->pending = c;
      }
    }
    i++;
    ptr += sizeof(TileAnimation);
    if (i < 4)
    {
      goto gl0;
    }
  }
#undef r
}
/* func_80026754 */
void tile_image_load(s32 tile, s32 source)
{
    s16 sp[4];

    sp[0] = ((tile & 0x300) >> 1) + 0x240 + ((tile & 0xF) << 3);
    sp[1] = (tile & 0xF0) + 0x100;
    sp[2] = 8;
    sp[3] = 0x10;
    LoadImage(sp, source);
}
/* func_800267AC */
void tile_image_run_load(s32 tile, s32 source, s32 tile_count)
{
    s32 i;

    for (i = 0; i < tile_count; i++) {
        tile_image_load(tile, source);
        tile++;
        source += 0x100;
    }
}

/* The loop's pointer is each TileAnimation's count word (+0x14), and the
   fields are reached through the record cast back from it (r, a cast
   macro, (TileAnimation *)(p - 0x14)), as in tile_animation_step. */
/* func_80026814 */
void tile_animation_load(void)
{
#define r ((TileAnimation *)(p - 0x14))
  u8 *p;
  u8 *animations;
  int i;
  u8 **p_home;
  i = 0;
  p = (animations = g_TileAnimations) + 0x14;
  for (; i < 4; i++)
  {
    if (((TileAnimation *)(*(p_home = &p) - 0x14))->pending)
    {
      r->pending = 0;
      tile_image_run_load(r->tile, r->source, r->count);
    }
    p += sizeof(TileAnimation);
  }
#undef r
}
