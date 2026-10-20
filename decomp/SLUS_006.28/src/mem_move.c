/* Lunar SSSC — mem_move: memory cleared and moved in bytes, 16-bit and
   32-bit units. The first split had it in rendering2.c. Every function is
   C.
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

/* func_8002609C */
void mem_clear(void *destination, u32 byte_count) {
    s32 write_address;
    u32 word_count;
    u32 bytes_done;
    u32 words_done;

    write_address = (s32)destination;
    if ((write_address & 3) || (byte_count & 3)) {
        bytes_done = 0;
        if (byte_count != 0) {
            do {
                (*(u8 *)write_address) = 0;
                bytes_done += 1;
                write_address += 1;
            } while (bytes_done < byte_count);
        }
    } else {
        word_count = byte_count >> 2;
        words_done = 0;
        if (word_count != 0) {
            do {
                (*(s32 *)write_address) = 0;
                words_done += 1;
                write_address += 4;
            } while (words_done < word_count);
        }
    }
}

/* func_800260FC */
void mem_move(u8 *dest, u8 *src, u32 count)
{
  u32 i;

  if (((u32) dest) < ((u32) src))
  {
    goto copy_forward;
  }
  if (((u32) dest) < (((u32) src) + count))
  {
    goto copy_backward;
  }
  copy_forward:
  if (count != 0)
  {
    i = 0;
    do
    {
      *dest = *src;
      src++;
      i++;
      dest++;
    }
    while (i < count);
  }

  return;
  copy_backward:
  src += count;

  dest += count;
  i = 0;
  if (count != i)
  {
    i = 0;
    do
    {
      dest--;
      src--;
      *dest = *src;
      i++;
    }
    while (i < count);
    return;
  }
}

/* func_80026178 */
void mem_move_16(u16 *dest, u16 *src, u32 count)
{
  u32 i;
  count >>= 1;
  if (((u32) dest) < ((u32) src))
  {
    goto copy_forward;
  }
  if (((u32) dest) < (((u32) src) + (count * 2)))
  {
    goto copy_backward;
  }
  copy_forward:
  if (count != 0)
  {
    i = 0;
    do
    {
      *dest = *src;
      src++;
      i++;
      dest++;
    }
    while (i < count);
  }

  return;
  copy_backward:
  src += count;

  dest += count;
  i = 0;
  if (count != i)
  {
    i = 0;
    do
    {
      dest--;
      src--;
      *dest = *src;
      i++;
    }
    while (i < count);
    return;
  }
}
/* func_800261FC */
void mem_move_32(u32 *dest, u32 *src, u32 count)
{
  u32 i;
  count >>= 2;
  if (((u32) dest) < ((u32) src))
  {
    goto copy_forward;
  }
  if (((u32) dest) < (((u32) src) + (count * 4)))
  {
    goto copy_backward;
  }
  copy_forward:
  if (count != 0)
  {
    i = 0;
    do
    {
      *dest = *src;
      src++;
      i++;
      dest++;
    }
    while (i < count);
  }

  return;
  copy_backward:
  src += count;

  dest += count;
  i = 0;
  if (count != i)
  {
    i = 0;
    do
    {
      dest--;
      src--;
      *dest = *src;
      i++;
    }
    while (i < count);
    return;
  }
}
extern s32 D_800E4768[];

/* Clear the pending flag (word 0) of the request block at D_800E4768 —
   the block menu_run.c, menu_list.c, shop_screen.c and btl_menu.c fill as
   [1] = address, [3] = size, [0] = 1.  Called once, by func_80025FE0. */
/* Ruling of 2026-10-14 (docs/naming.md): Its store is read by nothing, and its one caller says nothing of it. */
void func_80026280(void)
{
  D_800E4768[0] = 0;
}
