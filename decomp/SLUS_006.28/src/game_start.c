/* Lunar SSSC — game_start: the game's start: its records set up and reset.
   The first split had it in misc_h.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_PlayTimeFrames (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"

/* far_syms: g_PlayTimeFrames (carried from the first split's misc_e.c) */
extern u16 g_MapPadHeldButtons;
extern u16 g_MapPadPressedButtons;
extern u16 g_PlayTimeFrames;
extern s32 g_PartyIds;
extern s32 g_ChrRecords;
/* far_syms: g_MusicKeptLevel (target reaches it %hi/%lo) */
/* far_syms: g_GameOptionByte1Start (target reaches it %hi/%lo) */
/* far_syms: g_StartFirstStep (target reaches it %hi/%lo) */
/* far_syms: g_TileAnimationTiles (target reaches it %hi/%lo) */
/* far_syms: g_PaletteAnimationCols (target reaches it %hi/%lo) */
/* far_syms: g_FaceList (target reaches it %hi/%lo) */
/* far_syms: g_GameSaveHeadText (target reaches it %hi/%lo) */
extern s32 g_GameOptions;
extern u8 g_GameSaveHeadText[];

void MenuInit(void);
s32 sound_music_level_set(s32, s32, s32);
s32 func_8002473C(s32);
s32 card_accept_wait(void);
void ankprint_clear(void);
void mem_move(void *, void *, s32);
s32 StartRun(s32);
void MapFadeOut(s32, s32, s32, s32, s32);
void MapFadeIn(s32, s32, s32, s32);
void GameEnter(s32);
void MessageReset(void);
void MapRecordsClear(void);
void sprite_init_copy(void);
void ChrRecordsReset(void);
void MapLayersInit(void);
void ScriptChange(s32);
void FaceListInit(void);
void func_8007608C(s32);
extern s32 g_TileAnimationTiles;
extern s32 g_PaletteAnimationCols;
extern u8 g_ScriptNumber;
extern s32 g_ResourceCommand01Buffer;
extern s32 g_MapObjectDefinitions;
extern s32 g_GameSaveBlock;
extern s32 g_GameMapRecord;
extern s32 g_StoryBits;
extern s32 g_MapEnterTable;
extern s32 g_MapPartyPlaceRows;
extern s32 g_MapBgChangeTable;
extern s32 g_ChrPlacePresetRecords;
extern s32 g_ChrItemList0C;
extern s32 g_ChrItemWaitingList;
extern s32 g_GameMoney;
extern s32 g_MapPartyRecords;
extern s32 g_MapPartyTwins;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern s32 g_MapSpriteDataEntries;
extern s32 g_MapObjectStepLists;
extern s32 g_ScriptBase;
extern s32 g_ScriptEnded;
extern u8 *g_FaceList;
extern u8 g_GameOptionByte1Start;
extern s32 g_StartFirstStep;
extern u8 g_MusicKeptLevel;
extern s16 g_MusicNumber;

/* func_80055B60 */
void GameStart(void)
{
  s32 r;

  MenuInit();
  FaceListInit();
  g_GameSaveBlock = 0x801DBE68;
  g_GameMapRecord = 0x801DC2E0;
  g_GameOptions = 0x801DC374;
  g_StoryBits = 0x801DC380;
  g_MapEnterTable = 0x801E6B00;
  g_MapPartyPlaceRows = 0x801E7080;
  g_MapBgChangeTable = 0x801E7500;
  g_MapObjectDefinitions = 0x801E8900;
  g_ChrRecords = 0x801DBF80;
  g_ChrPlacePresetRecords = 0x801DC280;
  g_ChrItemList0C = 0x801DC300;
  g_ChrItemWaitingList = 0x801DC360;
  g_GameMoney = 0x801DC37C;
  g_MapPartyRecords = 0x801DBB80;
  g_MapPartyTwins = 0x801DBCE8;
  g_MapObjects = 0x801D6B80;
  g_MapObjectTwins = 0x801D9B80;
  g_PartyIds = 0x801DC368;
  g_MapSpriteDataEntries = 0x801EA300;
  g_MapObjectStepLists = 0x801E9C00;
  g_ScriptBase = 0x801DCB80;
  g_TileAnimationTiles = 0x801C1B00;
  g_ScriptNumber = 0xFF;
  g_ScriptEnded = 1;
  g_PaletteAnimationCols = g_ResourceCommand01Buffer + 0x599C;
  ChrRecordsReset();
  *g_FaceList = 0xC;
  func_8007608C(0);
  mem_move((void *) g_GameSaveBlock, g_GameSaveHeadText, 0xA);
  *((u8 *) g_GameOptions + 1) = g_GameOptionByte1Start;
  *((u8 *) g_GameOptions + 2) = 0;
  *((u8 *) g_GameOptions + 3) = 1;
  *((u8 *) g_GameOptions + 4) = 0;
  *((u8 *) g_GameOptions + 0) = 0;
  *((u8 *) g_GameOptions + 5) = 1;
  *((u8 *) g_GameOptions + 6) = 1;
  MapRecordsClear();
  sprite_init_copy();
  MapLayersInit();
  MessageReset();
  if (g_StartFirstStep == 0)
  {
    func_8002473C(0);
    sound_music_level_set(0, 0, 1);
    ScriptChange(0x64);
    func_8002473C(0xF);
    sound_music_level_set(0, g_MusicKeptLevel, 1);
  }
  MapFadeIn(0x30, 1, 0, 0);
  g_StartFirstStep = 1;
  r = card_accept_wait() != 1;
  MapFadeOut(0x30, 1, -1, 0, 0);
  g_MusicNumber = 0x17;
  r = StartRun(r);
  g_MapPadHeldButtons = 0;
  g_MapPadPressedButtons = 0;
  MapRecordsClear();
  sprite_init_copy();
  MapLayersInit();
  MessageReset();
  ankprint_clear();
  GameEnter((u16) r);
}

/* func_80055E6C */
s32 MapEnterTableAddress(void) {
    return 0x801E6B00;
}

/* far_syms: g_PlayTimeMinutes (target reaches it %hi/%lo) */
/* far_syms: g_PlayTimeMinutes (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 g_StoryBits;
extern s32 g_GameSaveBlock;
extern u16 g_PlayTimeMinutes;
extern u8 g_ScriptNumber;

/* func_80055E78 */
void GameRecordsReset(void)
{
  s32 a;
  s16 *saved_minutes;
  int saved_minutes_address;
  g_ScriptNumber = 0xFF;
  saved_minutes_address = g_GameSaveBlock + 0xA;
  a = g_StoryBits;
  g_PlayTimeFrames = 0;
  g_PlayTimeMinutes = 0;
  g_StoryBits++;
  g_StoryBits--;
  saved_minutes = (s16 *) saved_minutes_address;
  *saved_minutes = 0;
  mem_clear(a, 0x800 ^ 0);
  ChrRecordsReset();
}
