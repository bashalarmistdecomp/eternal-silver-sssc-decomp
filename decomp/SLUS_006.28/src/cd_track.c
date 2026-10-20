/* Lunar SSSC — cd_track: the disc's audio tracks: the table of contents, a
   track's play and its switch. The first split had it in ui.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
extern u8 g_CdTrackMode[];
extern s32 g_CdTrackCount;
extern u8 g_CdTocEntries[];
s32 cd_track_play(s32);                                 /* extern */
void cd_track_switch(s32 on);


/* func_8001B63C */
void cd_toc_read(void)
{
  s32 i;
  g_CdTrackCount = CdGetToc(g_CdTocEntries);
  for (i = 0; i <= g_CdTrackCount; i++)
  {
  }
}


/* func_8001B684 */
s32 cd_track_play(s32 track)
{
  u8 *toc_entries;
  s32 result;
  SpuSetCommonMasterVolume(0x3FFF, 0x3FFF);
  SpuSetCommonCDVolume(0x7FFF, 0x7FFF);
  SpuSetCommonCDReverb(0);
  SpuSetCommonCDMix(1);
  CdControlB(0xE, g_CdTrackMode, 0);
  result = g_CdTrackCount < track;
  if (result == 0)
  {
    toc_entries = g_CdTocEntries;
    CdControlB(0x16, (track * 4) + toc_entries, 0);
    result = CdControl(3, 0, 0);
  }
  return result;
}


/* func_8001B71C */
void cd_track_switch(s32 on) {
    if (on != 0) {
        cd_track_play(2);
        return;
    }
    CdControlF(9, 0);
}
