/* Lunar SSSC — palette_animation: the palette animations: a set started or
   copied, the buffer's address, and the frame's step. The first split had
   it in rendering2.c. Every function is C.
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
extern s32 g_PaletteAnimationCols;
extern s32 g_PaletteBuf;
void palette_buf_address_set(void);
void mem_move(u8 *dest, u8 *src, u32 count);  /* extern */

extern PaletteAnimationCopyRow g_PaletteAnimationCopyRows[];
extern PaletteAnimationCopyRow *g_PaletteAnimationCopyRow;
extern s8 g_PaletteAnimationCopyId;
extern s8 g_PaletteAnimationCopyFirst;
extern s8 g_PaletteAnimationCopyCount;

/* load_hazard_nop: palette_animation_copy_set */
/* Look up the row-508 CLUT copy request of palette-animation set `id`.
   g_PaletteAnimationCopyRows is a table of three-byte rows {id, first, count} ended by a row
   whose id is 0 (twelve rows in the image, ids 9, 12, 13, 18, 21, 23, 24, 26,
   30, 33, 39 and 40). The walk's cursor is the global g_PaletteAnimationCopyRow, which
   nothing else in the image reads: it is reset to the table's start on entry
   and stepped a row at a time.
     - On the row whose id equals `id`: g_PaletteAnimationCopyId = id, g_PaletteAnimationCopyFirst = its
       first, g_PaletteAnimationCopyCount = its count, and the function returns with the
       cursor left on that row. These are the three request bytes
       palette_update (palette_buf.c) reads every frame: while g_PaletteAnimationCopyId is non-zero
       it copies `count` 16-colour CLUTs, from CLUT `first` on, out of row 1
       of the colour buffer into VRAM row 508 with the STP bit set.
     - With no such row (the cursor ends on the terminator) nothing is
       written; the one caller, palette_animation_set, has zeroed the three bytes
       just before, so the request stays off.
   `id` is an int and is compared with the row's signed id byte; the caller
   passes its own u8 argument, never 0. No return value. A leaf: no callee,
   no frame wait, no RNG draw.
   Shapes: the table is an unsized (far) array of structs and the cursor a
   pointer global named at every use, so the first test reads the table
   through the `lui` of the address being stored and the loop reloads the
   cursor at its top. Holding the cursor in a pointer local inside the loop
   is 19/28 (no reload on the back edge); the table's first byte declared
   a far scalar is 27/28: it is then read through a second `lui`. The row's first and count match as u8 too; s8 is what palette_buf.c declares
   the request bytes. The two `lbu` / `sb %gp_rel` pairs need the assembler
   marker below (23/28 without it): the request bytes are small data defined
   in another object here, and maspsx cannot see that an extern is
   gp-relative. */
/* func_8002628C */
void palette_animation_copy_set(s32 id)
{
    g_PaletteAnimationCopyRow = g_PaletteAnimationCopyRows;
    while (g_PaletteAnimationCopyRow->id != 0) {
        if (g_PaletteAnimationCopyRow->id == id) {
            g_PaletteAnimationCopyId = id;
            g_PaletteAnimationCopyFirst = g_PaletteAnimationCopyRow->first;
            g_PaletteAnimationCopyCount = g_PaletteAnimationCopyRow->count;
            return;
        }
        g_PaletteAnimationCopyRow++;
    }
}

extern PaletteAnimation g_PaletteAnimations[];

extern u8 g_PaletteAnimationSets[];
extern PaletteAnimationSeed g_PaletteAnimationRows[];

/* Start palette-animation set `id` (0 = none): stop the eight palette
   animations palette_animation_step steps every frame, then load the set's own. The
   one caller in the image is the scene set-up MapLoad (map_load.c),
   which passes byte +5 of its scene record. In order:
   1. The row-508 CLUT copy request is switched off: g_PaletteAnimationCopyId, g_PaletteAnimationCopyFirst
      and g_PaletteAnimationCopyCount = 0 (the three bytes palette_update, palette_buf.c, acts on every
      frame).
   2. All eight slots of g_PaletteAnimations get mode = 0 (palette_animation_step skips a slot
      whose mode is 0); the compiled loop goes from slot 7 down to slot 0.
      Nothing else of a slot is cleared.
   3. id == 0 returns here.
   4. palette_animation_copy_set(id) sets the three request bytes again when the set has a
      row in g_PaletteAnimationCopyRows.
   5. The set's animations are loaded. g_PaletteAnimationSets holds one byte pair a set,
      {first seed row, count}, at [(id - 1) * 2] (40 pairs in the image,
      counts 1 to 7); g_PaletteAnimationRows holds the 8-byte seed rows. Seed rows
      first .. first + count - 1 go to slots 0 .. count - 1: the eight bytes
      as they are (a seed row is the head of a PaletteAnimation), then timer = 0,
      frame = 0 and the slot's mode word = its byte 0, the seed's mode. Slots
      count .. 7 stay stopped.
   Nothing is returned and the caller does not read $v0. Callee
   palette_animation_copy_set only: no frame wait, no RNG draw.
   Shapes. The pair index is computed back into the u8 parameter: its mask
   is then `andi $a0, $a0, 0xFF` on the parameter's own register (a u8 local
   for it: 30/57), and it is `(id - 1) * 2`, not `<< 1` (51/57). Both bytes
   are read by index, first then count, which makes two equal `id + table`
   sums; the second is the target's `addu $a0, $a1, $zero` (still a sum after
   global allocation, a copy of the first in the second scheduling pass's
   dump: post-reload cse, possible only while the first sum is still in its
   register). The second loop indexes the slot table by i (41/57 with a
   walked slot pointer: indexed, the reduced pointer is set up after the
   loop's entry test, as the target has it), steps the seed pointer in its
   body (55/57 with `src++` in the for header) and reads the mode back from
   the slot (55/57 from the seed row). The three request bytes are cleared
   before the first loop (54/57 after it). The first loop matches counting
   up or down: the compiler reverses it to 7 .. 0 either way.
   MARKED SPELLING: the pointer local `tab`. The seed table's address is
   taken into it after the two byte reads and the seed pointer is
   `&tab[first]`. With `src = &g_PaletteAnimationRows[first];` the address is formed
   after the shift and the first scheduling pass leaves the first byte's
   load directly behind its address sum; seven values then end in other
   registers (the first sum, the table's address and `first` in local
   allocation; i, the count, the seed pointer and the second loop's slot
   pointer in global allocation after them) and the second sum stays a sum,
   because the first sum's register holds `first` by then: 27/57. Whether
   the original had such a local, or another text that forms the address
   before the shift, the bytes do not say. */
/* func_800262FC */
void palette_animation_set(u8 id)
{
    s32 i;
    PaletteAnimationSeed *src;
    PaletteAnimationSeed *tab;
    s32 n;
    s32 first;

    g_PaletteAnimationCopyId = 0;
    g_PaletteAnimationCopyFirst = 0;
    g_PaletteAnimationCopyCount = 0;
    for (i = 0; i < 8; i++) {
        g_PaletteAnimations[i].mode = 0;
    }
    if (id == 0) {
        return;
    }
    palette_animation_copy_set(id);
    id = (id - 1) * 2;
    first = g_PaletteAnimationSets[id];
    n = g_PaletteAnimationSets[id + 1];
    tab = g_PaletteAnimationRows;
    src = &tab[first];
    for (i = 0; i < n; i++) {
        *(PaletteAnimationSeed *)&g_PaletteAnimations[i] = *src;
        src++;
        g_PaletteAnimations[i].timer = 0;
        g_PaletteAnimations[i].frame = 0;
        g_PaletteAnimations[i].mode = g_PaletteAnimations[i].seed.mode;
    }
}

/* func_800263E0 */
void palette_buf_address_set(void) {
    g_PaletteBuf = 0x801D5B80;
}
void palette_row_mark(s32, s32);

/* func_800263F4 */
void palette_animation_step(void)
{
  PaletteAnimation *r;
  s32 i;
  u16 *q;
  s32 n;
  s32 k;
  u16 t;
  g_PaletteBuf = 0x801D5B80;
  r = g_PaletteAnimations;
  for (i = 0; i < 8; i++, r++)
  {
    k = r->mode == 0;
    if (k)
    {
      continue;
    }
    if ((++r->timer) < r->seed.delay)
    {
      continue;
    }
    r->timer = 0;
    if ((++r->frame) >= r->seed.frames)
    {
      r->frame = 0;
    }
    q = (u16 *) ((g_PaletteBuf + (r->seed.first << 1)) + 0x200);
    switch (r->mode)
    {
      case 1:
        k = r->frame * r->seed.count;
        mem_move((u8 *) q, (u8 *) ((g_PaletteAnimationCols + (r->seed.source << 1)) + (k << 1)), r->seed.count << 1);
        break;
      case 2:
        n = r->seed.count - 1;
        t = q[n];
        while (n > 0)
      {
        q[n] = q[n - 1];
        n--;
      }
        q[n] = t;
        break;
      case 3:
        t = q[0];
        for (n = 0; n < (r->seed.count - 1); n++)
      {
        q[n] = q[n + 1];
      }
        q[n] = t;
        break;
    }
    palette_row_mark(r->seed.first | 0x100, r->seed.count);
  }
}
