/* Lunar SSSC — cell_face: a face put at each place of the grid a word's
   bits name, and an empty function. The first split had it in misc_k.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* func_80057608 */
void CellFacePlacesSet(s32 places, s32 unused, s32 face)
{
  s32 places_left;
  u32 place;
  s32 place_base;
  s32 kept_face;
  kept_face = face;
  places_left = places;
  place_base = 0;
  if (places_left & 0xFFFF)
  {
    do
    {
      if (places_left & 1)
      {
        place = place_base + 0x483;
        places_left &= 0xFFFE;
        goto block_10;
      }
      if (places_left & 2)
      {
        place = place_base + 0x49C;
        places_left &= 0xFFFD;
        goto block_10;
      }
      if (places_left & 0x10)
      {
        place = place_base + 0x481;
        places_left &= 0xFFEF;
        goto block_10;
      }
      if (places_left & 0x20)
      {
        place = place_base + 0x49F;
        places_left &= 0xFFDF;
        goto block_10;
      }
      if (places_left & 4)
      {
        place = place_base + 0x146;
        places_left &= 0xFFFB;
        goto block_10;
      }
      if (places_left & 8)
      {
        place = place_base + 0x2C4;
        places_left &= 0xFFF7;
      }
      block_10:
      FaceCellSet(place & 0x3F, place >> 6, kept_face);
    }
    while (places_left & 0xFFFF);
  }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_800576E4() {
}
