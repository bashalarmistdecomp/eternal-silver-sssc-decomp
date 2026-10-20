/* Lunar SSSC — story: the story bits set, cleared, checked and a run of
   them cleared. The first split had it in misc_l.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern u8 *g_StoryBits;  /* TU declares this below */

/* func_800576EC */
s32 StoryBitSet(u16 bit)
{
  s32 bit_in_byte;
  u32 masked_bit;
  u8 *byte;
  u8 old_byte;
  masked_bit = bit & 0xFFFF;
  byte = g_StoryBits + (masked_bit >> 3);
  old_byte = *byte;
  bit_in_byte = masked_bit & 7;
  if (!((((s32) old_byte) >> bit_in_byte) & 1))
  {
    if (byte)
    {
      *byte = old_byte | (1 << bit_in_byte);
      return 1;
    }
    else
    {
      *byte = old_byte | (1 << bit_in_byte);
      return 1;
    }
  }
  return 0;
}

extern u8 *g_StoryBits;  /* TU declares this below */

/* func_80057730 */
s32 StoryBitClear(u16 bit)
{
  s32 bit_in_byte;
  u32 masked_bit;
  u8 *byte;
  u8 old_byte;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `was_set` only holds the constant 0; behaviour is the same with the constant
     written where it is read, but then 3 of this function's 18 instruction lines differ. */
  int was_set;
  masked_bit = bit & 0xFFFF;
  byte = g_StoryBits + (masked_bit >> 3);
  old_byte = *byte;
  bit_in_byte = 7;
  bit_in_byte = masked_bit & bit_in_byte;
  if (!((((s32) old_byte) >> bit_in_byte) & 1))
  {
    was_set = 0;
    return was_set;
 do { } while (was_set);
  }
  *byte = old_byte & (~(1 << bit_in_byte));
  return 1;
}

extern u8 *g_StoryBits;

/* Story/event flag getter: bit `flag` of the g_StoryBits bitmap.
   31 callers — this is the game's general "is flag N set?" primitive. */
/* func_80057778 */
s32 StoryBitCheck(u32 flag) {
    flag &= 0xFFFF;
    return (g_StoryBits[flag >> 3] >> (flag & 7)) & 1;
}

s32 mem_clear(s32, s32);                            /* extern */

/* func_8005779C */
void StoryBits3E8Clear(void) {
    mem_clear((s32)g_StoryBits + 0x7D, 0x13);
}
