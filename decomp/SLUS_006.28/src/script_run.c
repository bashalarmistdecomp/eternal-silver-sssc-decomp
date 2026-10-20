/* Lunar SSSC — script_run: the script run from a label (ScriptLabelRun, the
   reader's loop) and the frames it lets go by and waits on (voice, music,
   moves, bg change, the colour flash). The first split had it in
   scripting_c.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_MenuPage (target reaches it %hi/%lo) */
/* far_syms: g_MenuPage */
/* far_syms: g_ChrItemNoRoomState (target reaches it %hi/%lo) */
/* far_syms: g_MoveChrSecondViewFollow g_MoveChrViewFollow */
/* far_syms: g_MoveChrSecondViewFollow (target reaches it %hi/%lo) */
/* far_syms: g_MoveChrSecondViewFollow (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MoveChrViewFollow (auto: target reaches it %hi/%lo — addressing row 3) */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "pad.h"
#include "game.h"
#include "chr.h"
#include "map.h"
#include "sprite.h"

extern u16 g_MapPartyBlinkCount;
extern u8 g_MapBgChange[];
extern s16 g_Voice;
extern u16 g_ScriptKeptBlinkCount;
extern s32 g_MessageOpenAllowed;
s32 layer_col_fade_start(s32, s32, s32, s32, s32 *);               /* extern */
extern u8 g_ScriptLabelCallDepth;
extern u8 g_MoveChrSecondViewFollow;
extern u8 g_MoveChrViewFollow;
extern long HWD0;
extern long VWD0;
extern s16 g_MessageNoticeBand;
extern s32 g_ScriptEnded;
extern u16 g_ScriptLabel;
extern s32 g_ChrItemNoRoomState;
extern s32 g_MenuPage;
/* g_MapCol: declared once, as MapCol, beside that typedef below (an
   unsized array here would make the object far for the whole TU) */
extern u8 g_MessagePressWait;
void MessageOpenClear(void);
extern u8 g_MessageSpeed;
extern u8 g_ScriptNumber;
extern s32 g_ScriptCommandPointer;
extern s16 g_ScriptErrorNumber;
extern s32 g_MapLayersMask;
extern s32 g_ScriptBase;
/* far_syms: g_MapBgChange */
extern long HWD0, VWD0;		 
extern s32 g_MapTalkObjectDefinition;
void MessageReset(void);  /* TU defines this below */
s32 MapViewStep(s32, s32);
void ScriptLabelRun(s32);
extern MapCol g_MapCol;
s32 ScriptFrameStep(s32);
extern long HWD0, VWD0;
s32 MapViewStep(s32, s32);
/* far_syms: g_MapBgChange[0] (target reaches it %hi/%lo) */
s32 StoryBitCheck(s32);
s32 MapViewStep(s32, s32);
void ScriptLabelRun(s32);
void ScriptLabelRun(s32);
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFaceSecond;
extern u16 g_MessageFacePlaces;
/* la_absolute: MapColFlashRun */
void MapBgChangeStep(void);  /* extern */

/* far_syms: g_FontStringWords (target reaches it %hi/%lo) */
extern u8 g_FontStringWords[];

/* func_80059F4C */
signed char *font_string_word_get(int word_number)
{
    signed char *p = (signed char *)g_FontStringWords;

    while (word_number > 0) {
        if (*p++ == 0)
            word_number--;
    }

    return p;
}

extern u8 g_MessagePadRecord[];

s32 pad_record_init(s32 *, s32, s32, s32);                      /* extern */
s32 font_target_clear(s32, s32);                              /* extern */
s32 func_800572D8();                                  /* extern */
extern u16 g_MessageLine;
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFaceSecond;
extern u16 g_MessageFacePlaces;

/* func_80059F7C */
void MessageReset(void) {
    g_MessageLine = 0;
    g_MessageFaceSecond = 0;
    g_MessageFaceFirst = 0;
    g_MessageFacePlaces = 0;
    font_target_clear(0x801CBB00, 0x55);
    func_800572D8();
    MessageOpenClear();
    pad_record_init((s32 *)g_MessagePadRecord, 0x14, 0xD0, 0);
}

/* far_syms: g_ScriptCommandFunctions (target reaches it %hi/%lo) */
/* far_syms: g_ChrItemNoRoomString (target reaches it %hi/%lo) */
/* load_hazard_nop: ScriptLabelRun */
extern u8 g_ScriptCommandFunctions[];
extern u8 g_ChrItemNoRoomString[];
s32 ScriptStateReport(u8, u16, s32, s32, s32, u16 *);
s32 layer_order_set(s32, s32);
s32 StoryBitCheck(s32);
s32 ChrItemWaitingCheck();
void ScriptMusicWait(void);  /* TU defines this below */
s32 MessageRecordsInit();
void MessageClose(void);  /* extern */
s32 MessageStringDisp(s32 *, s32);
void ScriptErrorHang();
s32 MoveChrInit();
s32 MoveChrEnd();
s32 MenuRun(s32);
extern void *g_GameOptions;
extern u16 g_ScriptCommandWord;
extern u16 g_MessageRow;  /* TU declares this below */
extern s32 g_MapTalkObjectDefinition;
extern s32 D_800B8E20;
extern u8 g_MessageTimed;  /* TU declares this below */

/* func_80059FDC */
void ScriptLabelRun(label)
    u16 label;
{
  s32 block_count;
  s32 target;
  u16 command_word;
  u16 label_offset;
  u16 blink_count;
  u8 press_wait;
  if (StoryBitCheck(0x2D6) != 0)
  {
    g_ChrItemNoRoomState = 0;
  }
  else
  {
    g_ChrItemNoRoomState = 1;
  }
  g_MessageLine = 0;
  g_Voice = 0;
  D_800B8E20 = 0;
  g_MessageSpeed = AS(s32, ((GameOptions *)g_GameOptions)->message_speed);
  press_wait = ((GameOptions *)g_GameOptions)->press_wait;
  blink_count = g_MapPartyBlinkCount;
  g_MessageTimed = 0;
  g_MessageFaceSecond = 0;
  g_MessageFaceFirst = 0;
  g_MessageRow = 0x14;
  g_MapPartyBlinkCount = 0;
  g_ScriptKeptBlinkCount = blink_count;
  g_MessagePressWait = press_wait;
  if (g_MenuPage != 0)
  {
    g_MessageOpenAllowed = 0;
    target = 0x801CBB00;
    block_count = 0x33;
    font_target_clear(target, block_count);
  }
  else
  {
    g_MessageOpenAllowed = 1;
    layer_order_set(0x800, 4);
    block_count = 0x55;
    target = (g_MessageLine * 0x880) + 0x801CBB00;
    font_target_clear(target, block_count);
  }
  MessageRecordsInit();
  MoveChrInit();
  label_offset = *((s16 *) (((label & 0xFFFF) * 2) + g_ScriptBase));
  if (label_offset == 0)
  {
    g_ScriptLabel = label;
    g_ScriptCommandWord = 0;
    g_ScriptErrorNumber = 0;
    ScriptErrorHang();
  }
  g_ScriptLabelCallDepth = 0;
  g_ScriptEnded = 0;
  g_ScriptLabel = label;
  g_ScriptCommandPointer = g_ScriptBase + (label_offset * 2);
  do
  {
    command_word = *(u16 *)g_ScriptCommandPointer;
    g_ScriptCommandWord = command_word;
    ScriptStateReport(g_ScriptNumber, g_ScriptLabel, command_word & 0xFFFF, g_MapTalkObjectDefinition, g_ScriptBase, (u16 *)g_ScriptCommandPointer);
    ((s32 (*)()) (*(s32 *)(((*(u16 *)g_ScriptCommandPointer) * 4) + g_ScriptCommandFunctions)))();
  }
  while (g_ScriptEnded == 0);
  ScriptMusicWait();
  MoveChrEnd();
  MessageClose();
  g_MapPartyBlinkCount = g_ScriptKeptBlinkCount;
  if (ChrItemWaitingCheck() != 0)
  {
    g_MessageRow = 0x14;
    MessageStringDisp(g_ChrItemNoRoomString, 1);
    MenuRun(0x16);
  }
}
/* far_syms: g_MoveChrSecondViewFollow */
/* far_syms: g_MoveChrViewFollow */
void packet_box_add(s32, s32, s32, s32, s32, s32);
void MoveChrUpdate(void);
s32 MapPartyAnimationStep();
s32 MapObjectsAnimationStep();
s32 sprite_list_draw();
s32 MapFrameEnd();
s32 palette_animation_step();
s32 tile_animation_step();
void ScriptVoiceStep(void);  /* TU defines this below */

/* Area (re)entry refresh: by g_MessageNoticeBand, window 0x14x0x140 at y 0x20
   (1) or 0x1C / 0x18 (2) via packet_box_add(0, .., 0x2000000, 0x40);
   then, when run is set, MoveChrUpdate and MapObjectsAnimationStep around
   MapPartyAnimationStep, MapViewStep(g_MoveChrViewFollow, g_MoveChrSecondViewFollow) and the fixed
   tail of refresh calls. The two switch arms are separate calls whose
   tails cross-jump into one. */
/* func_8005A1F0 */
s32 ScriptFrameStep(s32 run)
{
  switch (g_MessageNoticeBand)
  {
    case 1:
      packet_box_add(0, 0x14, 0x140, 0x20, 0x2000000, 0x40);
      break;
    case 2:
      packet_box_add(0, 0x1C, 0x140, 0x18, 0x2000000, 0x40);
      break;
  }
  if (run != 0)
  {
    MoveChrUpdate();
  }
  MapPartyAnimationStep();
  if (run != 0)
  {
    MapObjectsAnimationStep();
  }
  MapViewStep(g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
  sprite_list_draw();
  MapFrameEnd();
  MapBgChangeStep();
  palette_animation_step();
  tile_animation_step();
  ScriptVoiceStep();
}

/* func_8005A2D4 */
void ScriptFrameWait(s32 run, s32 frame_count) {
    s32 frames_done;

    frames_done = 0;
    if (frame_count > 0) {
        do {
            ScriptFrameStep(run);
            frames_done += 1;
        } while (frames_done < frame_count);
    }
}

s32 VoiceStep(s32);                               /* extern */

/* func_8005A328 */
void ScriptVoiceStep(void) {
    if ((g_Voice == -1) && !(VoiceStep(1) & 0xFF)) {
        g_Voice = 0;
    }
}

s32 ScriptMusicStep();                                /* extern */
s32 ScriptFrameStep(s32);                                 /* extern */
extern s32 g_ScriptMusicLevelRequest;

/* func_8005A364 */
void ScriptMusicWait(void) {
    if (g_ScriptMusicLevelRequest != 0) {
loop_1:
        if (ScriptMusicStep() != 0) {
            ScriptFrameStep(1);
            goto loop_1;
        }
        ScriptFrameStep(1);
    }
}

s32 layer_col_fade_check(s32);                               /* extern */
s32 func_8001EE60(s32, s32, s32 *);                       /* extern */

/* func_8005A3AC */
void MapColFlashRun(s32 length, s32 *target_col) {
loop_1:
    if (layer_col_fade_check(0) & 0xFF) {
        ScriptFrameStep(1);
        goto loop_1;
    }
    layer_col_fade_start(0, g_MapLayersMask | 0x100, 1, length, target_col);
loop_4:
    if (layer_col_fade_check(0) & 0xFF) {
        ScriptFrameStep(1);
        goto loop_4;
    }
    layer_col_fade_start(0, g_MapLayersMask | 0x100, 1, length, (s32 *)&g_MapCol);
loop_7:
    if (layer_col_fade_check(0) & 0xFF) {
        ScriptFrameStep(1);
        goto loop_7;
    }
loop_9:
    if (layer_col_fade_check(0) & 0xFF) {
        ScriptFrameStep(1);
        goto loop_9;
    }
    func_8001EE60(1, length, target_col);
loop_12:
    if (layer_col_fade_check(0) & 0xFF) {
        ScriptFrameStep(1);
        goto loop_12;
    }
    func_8001EE60(0, length, target_col);
loop_15:
    if (layer_col_fade_check(0) & 0xFF) {
        ScriptFrameStep(1);
        goto loop_15;
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_8005A500() {

}

s32 MapBgChangeEndWait();                                  /* extern */
s32 MapStatusClear();                                  /* extern */
s32 MoveChrCommandEndWait(s32);                                 /* extern */

/* func_8005A508 */
void ScriptMoveEndWait(void) {
    MapStatusClear();
    MapBgChangeEndWait();
    MoveChrCommandEndWait(0x3FFFF);
}

/* far_syms: g_MapBgChange (target reaches it %hi/%lo) */

/* func_8005A53C */
s32 MapBgChangeEndWait()
{
    if (g_MapBgChange[0]) {
        do {
            ScriptFrameStep(1);
        } while (g_MapBgChange[0]);
    }
}
