/* Lunar SSSC — xa: the XA sound from the disc: the file set, the play's
   start, end and stop, the drive's commands and checks, and the voices'
   places. The first split had it in cdrom_load.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

extern u8 D_80011848[];
extern s8 D_800B8A7C;
extern s32 g_XaStartSector;
extern s32 g_XaCurrentSector;
extern u8 D_80011864[];
extern u8 D_80011884[];
extern u8 D_8001181C[];
s32 cd_file_search(void *file, s32 name);  /* TU defines this below */
extern s32 g_XaPlayCount;
extern s32 D_800B8A54;
extern s32 D_800B8A80;
extern s8 g_XaFileFound;
extern u8 g_XaFile[];
extern u8 D_80011830[];
s32 func_80023564(void);  /* TU defines this below */
extern u8 g_XaMode[];
void xa_mode_set(void);  /* TU defines this below */
extern s32 g_XaEndSector;
void xa_end_set(s32 end_offset);  /* TU defines this below */
void xa_status_request(void);  /* TU defines this below */
extern int CdSync(int mode, u_char *result);
s32 xa_file_set(s32 name);
void *cd_dir_find(s32);
void xa_play_stop(void);
s32 xa_read_start(s32 sector, s32 file_number, s32 channel);
void xa_play_start(s32 offset, s32 channel);
s32 printf(s32 *);
s32 xa_command_check(void);
s32 xa_start_check(void);
s32 xa_end_check(void);

/* The .rdata between MoviePlay's switch table and the TU's next
   table (0x8001181C..0x800118C8): strings of the functions below.
   Raw bridges, pinned to this position so the first split's cdrom_load.c's
   .rodata was one block in the original order. */
INCLUDE_RODATA("asm/data/jtbl", D_8001181C);
INCLUDE_RODATA("asm/data/jtbl", D_80011830);
INCLUDE_RODATA("asm/data/jtbl", D_80011848);
INCLUDE_RODATA("asm/data/jtbl", D_80011864);
INCLUDE_RODATA("asm/data/jtbl", D_80011884);
INCLUDE_RODATA("asm/data/jtbl", D_80011898);
INCLUDE_RODATA("asm/data/jtbl", D_800118BC);
extern int CdSync(int mode, u_char *result);

/* func_8002347C */
s32 xa_file_set(s32 name)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `found` only holds the constant 1; behaviour is the same with the constant written
     where it is read, but then 4 of this function's 18 instruction lines differ. */
  int found;
  D_800B8A7C = 0;
  found = 1;
  if (cd_file_search(g_XaFile, name) != 0)
  {
    if (found)
    {
      D_800B8A7C = found;
      g_XaFileFound = 1;
      return 0;
    }
    else
    {
      D_800B8A7C = found;
      g_XaFileFound = 1;
      return 0;
    }
  }
  return 1;
}

extern int CdSync(int mode, u_char *result);

void *cd_dir_find(s32);                             /* extern */

/* func_800234C4 */
s32 cd_file_search(void *file, s32 name)
{
  s32 found;
  void *dir_entry;
  dir_entry = cd_dir_find(name);
  found = 0;
  if (dir_entry != 0)
  {
    CdIntToPos(((CdDirRow *)dir_entry)->first_sector, file);
    found = 1;
    ((CdlFILE *)file)->size = (s32) (((CdDirRow *)dir_entry)->size);
    return found;
  }
  return found;
}
/* far_syms: g_XaPlayCount (target reaches it %hi/%lo) */

/* func_80023518 */
void xa_play_stop(void) {
    if (g_XaFileFound != 0) {
        SpuSetCommonCDVolume(0, 0);
        CdControlF(9, 0);
        SpuSetCommonCDVolume(0x4000, 0x4000);
        g_XaPlayCount = 0;
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Compares two words nothing else counts, and nothing reaches it: nothing supports a name. */
s32 func_80023564(void) {
    return D_800B8A80 < D_800B8A54;
}
extern u8 g_XaMode[];

/* func_80023574 */
void xa_mode_set(void) {
    D_800B8A54 = 0x5A;
    D_800B8A80 = 0;
    SpuSetCommonCDMix(1);
    SpuSetCommonCDVolume(0x4000, 0x4000);
    CdControlB(0xE, (s32 *)g_XaMode, 0);
}

s32 xa_read_start(s32, s32, s32);                         /* extern */

/* func_800235C0 */
void xa_play_start(s32 offset, s32 channel) {
    s32 sector;

    xa_mode_set();
    if (g_XaFileFound != 0) {
        sector = CdPosToInt((s32 *) g_XaFile) + offset;
        g_XaStartSector = sector;
        g_XaCurrentSector = sector;
        SpuSetCommonCDVolume(0x4000, 0x4000);
        xa_read_start(sector, 1, channel);
    }
}

extern s32 g_XaEndSector;

/* func_80023630 */
void xa_end_set(s32 end_offset) {
    g_XaEndSector = end_offset;
    if (g_XaFileFound != 0) {
        g_XaEndSector = CdPosToInt((s32 *)g_XaFile) + end_offset;
    }
}
/* far_syms: g_XaPlayCount (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_80023674 */
s32 xa_read_start(s32 sector, s32 file_number, s32 channel) {
    u8 buf[8];
    s32 loc;

    g_XaPlayCount++;
    CdIntToPos(sector, &loc);
    buf[0] = (u8)file_number;
    buf[1] = (u8)channel;
    CdControlF(0xD, buf);
    CdControlB(0x1B, &loc, 0);
    return CdSync(0, 0);
}
/* func_800236F8 */
s32 xa_sector_set(s32 sector)
{
  CdlLOC loc;
  CdIntToPos(sector, &loc);
  CdControlB(2, (s32 *)&loc, 0);
  return CdSync(0, 0);
}

s32 printf(s32 *);

/* func_80023734 */
s32 xa_command_check(void)
{
  u8 result;
  int complete_state;
  s32 command_state;
  s32 complete;
  command_state = CdSync(1, &result);
  if (command_state == 5)
  {
    printf(D_8001181C);
    return 0;
  }
  if (command_state)
  {
    /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
       the block runs once, so behaviour is the same without the `do` / `while (0)`, but
       then 4 of this function's 21 instruction lines differ. */
    do
    {
      complete_state = 2;
      complete = 1;
    }
    while (0);
  }
  else
  {
    complete_state = 2;
    complete = 1;
  }
  if (command_state != complete_state)
  {
    complete = 0;
  }
  return complete;
}


/* func_80023788 */
void xa_status_request(void) {
    CdControlF(1, 0);
}

s32 printf(s32 *);                               /* extern */

/* func_800237AC */
s32 xa_start_check(void)
{
  u8 result;
  s32 command_state;
  u8 reading;
  command_state = CdSync(1, &result);
  if (command_state == 5)
  {
    printf((s32 *)D_8001181C);
    return 0;
  }
  reading = 0;
  if (command_state == 2)
  {
    reading = 1;
    if (!(result & 0x20))
    {
      CdControlF(1, 0);
      reading = 0;
      return reading;
    }
    return reading;
  }
  return reading;
}
/* far_syms: D_8001181C (target reaches it %hi/%lo) */

/* func_80023820 */
s32 xa_end_check(void)
{
  u8 result[0x10];
  s32 command_state;
  s32 sector;
  s32 new_sector;

  command_state = CdSync(1, result);
  if (command_state == 5)
  {
    printf((s32 *)D_8001181C);
    return 0;
  }
  if (command_state != 2)
    return 0;

  sector = g_XaCurrentSector;
  if (sector < g_XaStartSector)
    return 1;
  if (sector >= g_XaEndSector)
    return 1;

  new_sector = CdPosToInt((s32 *)&result[5]);
  if (new_sector > 0)
    g_XaCurrentSector = new_sector;

  CdControlF(0x11, 0);
  return 0;
}
/* far_syms: g_XaVoices (target reaches it %hi/%lo) */
/* far_syms: g_XaChannelEnds (target reaches it %hi/%lo) */
/* far_syms: D_800AF62C (target reaches it %hi/%lo) */
extern u8 g_XaVoices[];
extern u8 g_XaChannelEnds[];
extern u8 D_800AF62C[];

/* func_800238BC */
s32 xa_voice_get(s32 voice, s32 *offset, s32 *end_offset, s32 *channel)
{
    s32 end;
    s32 entry;
    s32 v2;
    s32 v3;
    u16 *p;

    p = (u16 *)g_XaVoices + voice;
    entry = p[0];
    v2 = entry >> 12;
    *channel = v2;
    *offset = (entry & 0xFFF) << 4;
    entry = p[1];
    v3 = entry >> 12;

    if (v3 == v2)
    {
        if (D_800AF62C[voice])
            end = (entry & 0xFFF) - 3;
        else
            end = (entry & 0xFFF) - 4;
    }
    else
    {
        if (D_800AF62C[voice])
            end = ((u16 *)g_XaChannelEnds)[v2] - 3;
        else
            end = ((u16 *)g_XaChannelEnds)[v2] - 4;
    }

    end <<= 4;
    *end_offset = end;
    return end;
}
