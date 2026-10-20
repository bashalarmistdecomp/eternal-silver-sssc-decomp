/* Lunar SSSC — map_attribute: what the map's attributes under the party ask
   for (a map change, a place, a bg change, a script label by kinds 4 to 6).
   The first split had it in scripting_b.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "map.h"
/* far_syms: g_MapBgChange */
extern u8 *g_MapPartyRecords;
extern s8 g_MapPartyBoxDx;
extern s8 g_MapPartyBoxDy;
extern u8 g_MapPartyBoxHalfWidth;
extern u8 g_MapPartyBoxHalfHeight;
extern u16 g_MapPadPressedButtons;
extern u8 g_MapBgChange[];
s32 MapAttributeFind(s32 x, s32 y, s32 w, s32 h);
s32 MapChange(s32, s32, s32, s32, s32, s32);
void MapPartyPlace(u16, u8 *, s32, s32, s32);
void MapBgChangeStart(s16, u8);
void MapAttributeKind4Run(s32 label);
void MapAttributeKind5Run(s32 label);
void MapAttributeKind6Run(s32 label);
s32 MapColFadeStart(s32, s32);
s32 ColMix(u8, s32);

/* The map-cell event dispatch.

   Probes one map cell — party actor 0's position (g_MapPartyRecords: s16 +6 and
   +0xA) plus the signed byte offsets g_MapPartyBoxDx / g_MapPartyBoxDy — and acts on
   the event word the probe returns: bits 7.. are the kind (1..9), the low
   7 bits its argument.  Kinds 1 and 2 are scene changes, 4 to 6 start map
   script `arg` (6 only under g_MapPadPressedButtons & 0x40), 8 passes two 4-bit
   fields, 9 sets the colour mix; kind 7 and a zero word do nothing. */
/* func_800590C0 */
void MapAttributeRun(void)
{
  s32 tile;
  u32 code;
  u32 arg;
  tile = MapAttributeFind(((MapRecord *)g_MapPartyRecords)->sprite.x + g_MapPartyBoxDx, ((MapRecord *)g_MapPartyRecords)->sprite.y + g_MapPartyBoxDy, g_MapPartyBoxHalfWidth, g_MapPartyBoxHalfHeight);
  code = tile & 0xFFFF;
  if (code != 0)
  {
    arg = tile & 0x7F;
    switch (code >> 7)
    {
      case 1:
        MapChange(arg, 0, 1, 1, 1, 0xFF);
        break;

      case 2:
        MapPartyPlace(arg, 0, 1, 1, 1);
        break;

      case 3:
        if (g_MapBgChange[0] == 0 || g_MapBgChange[0x15] != 0)
        {
          MapBgChangeStart(arg, 0);
        }
        break;

      case 4:
        MapAttributeKind4Run(arg);
        break;

      case 5:
        MapAttributeKind5Run(arg);
        break;

      case 6:
        if (g_MapPadPressedButtons & 0x40)
        {
          MapAttributeKind6Run(arg);
        }
        break;

      case 8:
        MapColFadeStart(arg >> 4, arg & 0xF);
        break;

      case 9:
        ColMix(arg, 1);
        break;
    }
  }
}
