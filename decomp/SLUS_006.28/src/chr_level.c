/* Lunar SSSC — chr_level: a character record's level read and set. The
   first split had it in misc_h.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern s32 g_PartyIds;
extern s32 g_ChrRecords;

void mem_move(void *, void *, s32);

/* func_80056750 */
u16 PartyIdGet(s32 place)
{
    return ((u16 *)g_PartyIds)[place];
}

/* func_80056768 */
s32 ChrLevelGet(s32 chr_id)
{
  return *((u8 *) (((chr_id << 6) + g_ChrRecords) + 0x1C));
}

/* func_80056780 */
void ChrLevelSet(s32 chr_id, s32 level)
{
  u8 buffer[0x10];
  s32 i;
  s32 record_level;
  u16 *p;

  record_level = *((u8 *) (((chr_id << 6) + g_ChrRecords) + 0x1C));
  if (level < record_level)
  {
    for (i = 0; i < 6; i++)
    {
      buffer[i] = ChrWornItemRemove(chr_id & 0xFFFF, (i + 1) & 0xFFFF);
    }
    resource_load(0xE7, 0x801CFB00);
    mem_move(g_ChrRecords + (chr_id << 6), (chr_id << 6) + 0x801CFB00, 0x40);
    for (i = 0; i < 6; i++)
    {
      ChrWornItemSet(chr_id & 0xFFFF, (i + 1) & 0xFFFF, buffer[i]);
    }
  }

  record_level = *((u8 *) (((chr_id << 6) + g_ChrRecords) + 0x1C));
  if (record_level != level)
  {
    ChrLevelAdd(chr_id & 0xFFFF, (level - record_level) & 0xFF);
    p = (u16 *) ((chr_id << 6) + g_ChrRecords);
    p[0] = p[1];
    p[2] = p[3];
  }
}
