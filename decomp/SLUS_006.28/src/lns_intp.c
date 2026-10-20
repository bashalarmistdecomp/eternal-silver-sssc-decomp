/* Lunar SSSC — lns_intp: the script reader's commands and what they need:
   the error hang and report, the command pointer's advance and list count,
   the answers, pages and notices commands 7, 28, 29, 31 and 32 show, the
   labels, story bits, money, items, actuators, party, map objects, map,
   fades, battle, movie, shop, music, voices and the rest. The name is the
   developers' own: LNS_INTP.C, the source file ScriptErrorHang's assertion
   names ('TextError!', line 0x66A); the file's other functions have it by
   being of its group. The first split had it in scripting_c.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "script.h"
#include "pad.h"
#include "game.h"
#include "chr.h"
#include "map.h"
#include "sprite.h"

s32 ColMix(u8, s32);
void MapBgChangeStart(s16, u8);
extern s16 g_Voice;
extern u16 g_ScriptKeptBlinkCount;
s32 layer_col_set(s32, s32, s32 *);
s32 MapColFadeStart(s32, s32);
extern u8 g_ScriptLabelCallDepth;
extern u8 g_ScriptLabelCallKeptLabels[];
extern u8 g_ScriptLabelCallKeptPointers[];
extern u8 g_MoveChrSecondViewFollow;
extern u8 g_MoveChrViewFollow;
extern long HWD0;
extern long VWD0;
extern s32 g_MessageCursorEntry;
extern s32 g_MessageCursorPiece;
extern s16 g_MessageNoticeBand;
extern s32 g_ScriptEnded;
extern u16 g_ScriptLabel;
extern s32 g_ChrItemNoRoomState;
extern s16 g_MapPlaceName;
extern s8 g_GameEnterRequest;
extern s32 g_MenuPage;
extern u8 g_ScriptActuatorLists[];
extern s16 g_PadActuatorListWait;
extern s32 g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern u8 g_PartyCount;
/* g_MapCol: declared once, as MapCol, beside that typedef below (an
   unsized array here would make the object far for the whole TU) */
extern u8 g_MapFadeState[];
extern u8 g_MapDrawAreaClosed[];
extern unsigned int g_ScriptSoundNextSlot;
extern u8 g_MessageKeptSpeed;
extern u8 g_MessagePressWait;
extern u8 g_MessageKeptPressWait;
extern s32 g_PartyIds;
extern s32 g_ChrRecords;
extern u32 *g_GameMoney;
extern u8 g_MessageSpeed;
extern u8 g_ScriptNumber;
extern s32 g_ScriptCommandPointer;
extern s16 g_ScriptErrorNumber;
extern u16 g_MapPartyWalkStepCount;
extern u16 g_MapPartyKind;
extern s32 g_MapObjects;
extern s32 g_MapObjectTwins;
extern s32 g_MapLayersMask;
extern u16 g_MapNumber;
extern u16 g_MusicNumber;
extern u16 g_MusicKind;
extern s32 g_ScriptBase;
s32 ScriptPointerAdvance(u8);  /* u8: the callee masks its argument on entry (andi 0xFF); every caller passes 2 or 4 */
/* far_syms: g_MapFadeState_s */
extern u16 g_ScriptPageRingIndex;
extern u8 g_ScriptPageRingEntries[];
extern long HWD0, VWD0;		 
extern u8 g_MapFadeState_s;
extern s32 g_MapTalkObjectDefinition;
s32 SoundPlay(u16, s32);
void MapFadeOut(s32, s32, s32, s32, s32);
void MapFadeIn(s32, s32, s32, s32);
void MapBgChangeStoryBitsCopy(void);  /* extern */
void MapPadDirectionsSet(s32);
void MapSpriteDataFree();
s32 MapViewStep(s32, s32);
s32 debug_printf();
void ScriptChange(u8 script_number);
extern MapCol g_MapCol;
extern void *g_MessageArrowRecord;
extern s32 g_MapPartyRecords;
extern u8 *g_FaceList;
extern s32 g_MapSpriteDataMarks;
s32 ScriptFrameStep(s32);
extern long HWD0, VWD0;
s32 SoundPlay(u16, s32);
s32 MapDrawAreaClose(s32 layer_mask, u32 step_count, s32 unused, s32 view_follow, s32 second_view_follow);
s32 MapDrawAreaOpen(s32 layer_mask, u32 step_count, s32 unused, s32 view_follow, s32 second_view_follow);
s32 MapViewStep(s32, s32);
s32 StoryBitSet(u16);
s32 SoundPlay(u16, s32);
/* far_syms: g_MapBgChange[0] (target reaches it %hi/%lo) */
s32 StoryBitCheck(s32);
s32 MapViewStep(s32, s32);
void MapColSet(s32, s32);
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFaceSecond;
extern u8 g_MessagePadRecord[];
s32 pad_record_init(s32 *, s32, s32, s32);
s32 font_target_clear(s32, s32);
extern u16 g_MessageLine;
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFaceSecond;
s32 ScriptStateReport(u8, u16, s32, s32, s32, u16 *);
s32 StoryBitCheck(s32);
void ScriptMusicWait(void);
s32 MessageRecordsInit();
void ScriptErrorHang();
extern void *g_GameOptions;
extern u16 g_ScriptCommandWord;
extern s32 g_MapTalkObjectDefinition;
extern u8 g_MessageTimed;
/* far_syms: g_MoveChrSecondViewFollow */
/* far_syms: g_MoveChrViewFollow */
s32 ScriptFrameStep(s32);
extern s32 g_ScriptMusicLevelRequest;
s32 MapBgChangeEndWait();
s32 MapStatusClear();
s32 MoveChrCommandEndWait(s32);
s32 mem_clear(s32, s32);
extern u16 g_MapViewX;
extern u16 g_MapViewY;
/* far_syms: g_MenuPage (target reaches it %hi/%lo) */
s32 MusicLevelStep(s32, s32);
void MessageLinesClear(void);
extern u16 g_MapIdleCount;
void MessageLineDisp(s32);
extern u8 g_MessageTimed;
void CellAllUpdate();
void MessageCursorSet(s16 width, s16 height, s16 col, s16 entry_word);
s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
s32 CellRowsClear(s32, s32, s32);
s32 CellNumberSet(s32, s32, s32, s32, s32, s32, s32, s32);
s32 CellGlyphSet(s32, s32, s32, s32, s32, s32);
void MessageNoticeDisp(u16, u16);
void MessageNoticeNumberDisp(s32 amount, s32 notice_kind);
void MessageLinesClearQuick(void);
void ScriptAnswerRun();
void ScriptPagesRun();
void ScriptCommandAnswerRun(void);
void ScriptCommandStoryBits580Clear(void);
void ScriptCommandStoryBits480Set(void);
void ScriptCommandStoryBits480Clear(void);
void func_8005E608();
s32 VoiceStart();
void ScriptErrorHang();
void ScriptCommandVoiceStart(void);
void ScriptCommandVoiceEndWait(void);
void ScriptErrorReport();
s32 mem_clear(s32, s32);
void MapSpriteDataMarksClear(void);
s32 ScriptListCount(s32, s32);
void MapPartyBoxSet(void);
void MapReload(void);
void ScriptCommandScriptChange(void);
s32 rng_draw();
void ScriptCommandLabelJump();
void ScriptCommandLabelPick(void);
s32 StoryBitSet(u16);
void ScriptCommandStoryBitsSet(void);
s32 StoryBitClear(u16);
void ScriptCommandStoryBitsClear(void);
void ScriptCommandStoryBits580Set(void);
s32 StoryBitCheck(s32);
void ScriptCommandStoryBits580AllCheck(void);
void ScriptCommandStoryBits580NoneCheck(void);
void ScriptCommandStoryBits580AnyCheck(void);
void ScriptCommandStoryBitPairsSet(void);
void ScriptCommandStoryBitPairsClear(void);
void ScriptCommandStoryBitPairsAllCheck(void);
void ScriptCommandStoryBitPairsAnyCheck(void);
void ScriptCommandMoneyTake(void);
void ScriptCommandMoneyAboveJump(void);
s32 ChrItemRemove(s32, s32);
s32 MessageNoticeItemDisp(s32, s32);
void ScriptCommandChrItemRemove(void);
void ScriptCommandActuatorListStart(void);
s32 PartyPlaceFind(u32);
s32 MoveChrPartyLeave(s32, s32, s32);
void ScriptCommandPartyLeave(void);
void ScriptCommandMapBgChangeStart(void);
void MapColSet(s32, s32);
volatile char ScriptCommandMapColSet(void);
void ScriptCommandColMix(void);
s32 ShopRun(s32);
s32 MessageRecordsInit();
void ScriptCommandShopRun(void);
s32 SoundPlay(u16, s32);
void ScriptCommandSoundPlay(void);
void ScriptCommandMusicLevelSet(void);
void ScriptCommandMessageTimedSet(void);
void ScriptCommandPartyMpClear(void);
void ScriptCommandMoneyAdd(void);
s32 ChrItemGive(s32, s32);
void ScriptCommandChrItemGive(void);
void ScriptCommandPlaceNameSet(void);
void ScriptCommandGameEnterRequest(void);
/* far_syms: g_FontStringTable */
extern s32 g_FontStringTable;
s32 pad_record_update(u8 *, s32);
void font_target_set(u8 *target);
/* la_absolute: ScriptCommandMapColSet */
void MapChange();  /* extern */
void MapPartyPlace(u16 idx, MapPartyPlaceRow *src, s32 run, s32 view_follow, s32 second_view_follow);  /* extern */
void MapColFlashRun(s32 length, s32 *target_col);  /* extern */
void ScriptMoveEndWait(void);  /* extern */
s32 MessageRecordsInit(void);  /* extern */
s32 MapStatusClear(void);  /* extern */

extern u8 D_8001296C[];
extern u8 D_80012978[];
void assert_fail();

/* ScriptErrorHang -- THE SCRIPT ERROR REPORTER: line 1642 of LNS_INTP.C.
   It never returns.

   1. The debug print ScriptStateReport with six values: the script number
      g_ScriptNumber (u8), the label being run g_ScriptLabel (u16), g_ScriptCommandWord
      (u16, where the fetch loop keeps the command word), the word
      g_MapTalkObjectDefinition (a map-object row's byte +1 while that object's event runs,
      otherwise 0xFF), the script's base g_ScriptBase and the cursor
      g_ScriptCommandPointer minus 2. It is the print the interpreter's fetch loop in
      ScriptLabelRun makes for every command, here with the cursor stepped
      back one word. ScriptStateReport is empty in this build (main.c).
   2. The assert assert_fail with D_8001296C, D_80012978 and 0x66A: the
      strings "TextError!" and "LNS_INTP.C" (both in boot_data's .rodata,
      still asm) and the line number 1642. assert_fail is empty in this
      build too (exception.c).
   3. An endless empty loop, a `j` to itself. With both callees empty the
      whole effect in the retail game is that it stops here.

   It reads the six globals above and writes nothing. The error code the
   callers store in g_ScriptErrorNumber just before the call is NOT read here and is
   not among the print's arguments; it is only there for whoever looks at
   memory after the hang.

   Reached two ways: fifteen `jal` sites, all in the scripting segment
   (ScriptLabelRun's missing label, ScriptCommandMapObjectsAdd's four checks, ScriptCommandVoiceStart
   and the rest), and the command table g_ScriptCommandFunctions, whose entries 0, 1, 6
   and 0x1B are this function, so those four command ids are errors.
   No RNG draw. No frame wait: nothing is called inside the loop.

   Shape: the loop is this function's own. Every other converted call of
   assert_fail (ankprint.c, cdread.c, face.c, icon.c, lns_move.c, mth_curv.c,
   overlay.c, sound.c, xmalloc.c) carries on after it, so the hang is not
   part of an assert macro. Without the loop the function gets an epilogue:
   27/28 (ScriptPointerAdvance's 8 rows counted in).
   Return type: void. The function has no exit, so the bytes say nothing
   about its type, and no caller reads $v0 (`s32` gives the same bytes). */
/* func_8005B568 */
void ScriptErrorHang(void)
{
    ScriptStateReport(g_ScriptNumber, g_ScriptLabel, g_ScriptCommandWord, g_MapTalkObjectDefinition, g_ScriptBase, (u16 *)(g_ScriptCommandPointer - 2));
    assert_fail(D_8001296C, D_80012978, 0x66A);
    for (;;) {
    }
}
/* ScriptPointerAdvance -- STEPS THE SCRIPT CURSOR TO THE NEXT MULTIPLE OF
   n AND RETURNS IT:
       g_ScriptCommandPointer = (g_ScriptCommandPointer & -n) + n;
   the cursor rounded DOWN to a multiple of n, plus n. A cursor already on a
   multiple of n still moves on by n: with n = 2 on a command word c (always
   even) it is c + 2, "skip the command id, give me the operands"; with
   n = 4 on a command word it is the first 4-aligned address past the 2-byte
   id, c + 2 or c + 4; from an odd address n = 2 gives the next even one.
   n is one byte wide. All 38 callers pass a constant: 2 (34 sites) or 4
   (4 sites). It reads and writes g_ScriptCommandPointer and nothing else, and calls
   nothing: no RNG draw, no frame wait.

   Shape (the scores count ScriptErrorHang's 20 rows in): the `&` first and
   n added second (n written first in the sum: 23/28; the cursor plus n and
   then the `&`, the same value for a power of two: 25/28).
   The `andi $a0, 0xFF` at entry is the `u8` parameter (an `s32` one loses
   it: 27/28). Every caller passes a constant, so the (u8) prototypes move
   no caller's bytes. */
/* func_8005B5B8 */
s32 ScriptPointerAdvance(u8 n)
{
    g_ScriptCommandPointer = (g_ScriptCommandPointer & -n) + n;
    return g_ScriptCommandPointer;
}

/* func_8005B5D8 */
s32 ScriptListCount(s32 kind, s32 step)
{
  s32 masked_kind;
  s32 count;
  s32 odd;
  u8 *next_word_entry;
  u8 *word_entry;
  u8 *next_command;
  u8 *byte_entry;
  masked_kind = kind & 0xFF;
  count = 0;
  if (masked_kind == 1)
  {
    byte_entry = g_ScriptCommandPointer;
    odd = ((s32) byte_entry) & 1;
    if ((*byte_entry) != 0)
    {
      do
      {
        byte_entry = &byte_entry[step & 0xFF];
        count += 1;
      }
      while ((*byte_entry) != 0);
      odd = ((s32) byte_entry) & 1;
    }
    if ((((s32) byte_entry) & 1) != 0)
    {
      next_command = byte_entry + 1;
    }
    else
    {
      next_command = byte_entry + 2;
    }
    goto block_13;
  }
  if (masked_kind == 2)
  {
    word_entry = g_ScriptCommandPointer;
    if (*(u16 *)word_entry != 0)
    {
      do
      {
        count += 1;
        next_word_entry = word_entry + ((step & 0xFF) * 2);
        g_ScriptCommandPointer = next_word_entry;
        word_entry = next_word_entry;
      }
      while (*(u16 *)next_word_entry != 0);
    }
    next_command = g_ScriptCommandPointer + 2;
    block_13:
    g_ScriptCommandPointer = next_command;

  }
  return count;
}

/* func_8005B694 */
void MessageCursorSet(s16 width, s16 height, s16 col, s16 entry_word) {
    u16 *p;
    u16 *q;

    width--;
    p = (u16 *)g_MessageCursorPiece;
    q = (u16 *)g_MessageCursorEntry;
    height--;
    p[3] = col;
    p[6] = width;
    p[8] = width;
    p[9] = height;
    p[11] = height;
    q[4] = entry_word;
}
extern s32 g_SpriteList;
void sprite_run_off(s32, u32);
s32 ankprint_rows_clear(s32, s32, s32);
void font_string_put(s32, s32, s32);

/* The field message window's two-answer prompt (one caller, ScriptAnswerRun,
   which passes the answer to start on and two MessageAnswer on its stack).
   Returns the answer chosen, 0 or 1.

   1. The two answers are drawn into the window's rows 3 and 4: row counter
      g_MessageLine = 3, pen font_target_set(0x801CD480), string
      font_string_put(str[0].id, str[0].arg, 1), ankprint_rows_clear(2, 1, 0x20),
      MessageLineDisp(1); then the same with row 4, pen 0x801CDD00, str[1]
      and ankprint_rows_clear(4, 1, 0x20).
   2. The cursor: MessageCursorSet(0x110, 0x10, 0x7B1, 0), then the render
      entry *g_MessageCursorEntry gets x = g_MapViewX + 0x18, +4 = g_MapViewY + 0xE0
      and y = g_MapViewY + sel * 16 + 16.
   3. The pad record g_MessagePadRecord is started (pad_record_init(rec, 0x14,
      0x5040, 0)) and one frame runs per pass until its press word has
      bit 0x40: a repeat word with bit 0x1000 or 0x4000 flips sel with
      sound 1 (SoundPlay(1, 1)); the cursor's y is set again and the
      entry is copied (0x24 bytes) to g_SpriteList + 0x14D0 (by offset,
      entry 13 of the copies other files keep from +0x12FC); ScriptFrameStep(1); pad_record_update(rec, 0x14).
   4. Sound 2, sprite_run_off(0x94, 1) (sprite slot 0x94 cleared) and
      MessageLinesClearQuick() (MessageLineDisp(-1) for each row).
   No RNG.

   Shape: sel is the u8 parameter itself (masked at each use, flipped with
   a bare xori). The row offset goes through an s16 local before it is
   added to g_MapViewY: written inside the sum, fold moves the constant 16
   onto g_MapViewY (`addiu` on the wrong operand, 97/117). The copy is a
   struct assignment (GCC's 16-byte block-move loop and a tail word). */
/* func_8005B6C0 */
s32 MessageAnswerRun(u8 sel, MessageAnswer *str)
{
    PadRecord *pad;
    s16 y;

    g_MessageLine = 3;
    font_target_set((u8 *)0x801CD480);
    font_string_put(str[0].string_id, str[0].string, 1);
    ankprint_rows_clear(2, 1, 0x20);
    MessageLineDisp(1);
    g_MessageLine = 4;
    font_target_set((u8 *)0x801CDD00);
    font_string_put(str[1].string_id, str[1].string, 1);
    ankprint_rows_clear(4, 1, 0x20);
    MessageLineDisp(1);
    MessageCursorSet(0x110, 0x10, 0x7B1, 0);
    pad = (PadRecord *)g_MessagePadRecord;
    ((SpriteEntry *)g_MessageCursorEntry)->x = g_MapViewX + 0x18;
    y = sel * 16 + 16;
    ((SpriteEntry *)g_MessageCursorEntry)->y = g_MapViewY + y;
    ((SpriteEntry *)g_MessageCursorEntry)->depth = g_MapViewY + 0xE0;
    pad_record_init((s32 *)pad, 0x14, 0x5040, 0);
    while (!(pad->pressed & 0x40)) {
        if (pad->repeated & 0x5000) {
            sel ^= 1;
            SoundPlay(1, 1);
        }
        y = sel * 16 + 16;
        ((SpriteEntry *)g_MessageCursorEntry)->y = g_MapViewY + y;
        *(SpriteEntry *)(g_SpriteList + 0x14D0) = *(SpriteEntry *)g_MessageCursorEntry;
        ScriptFrameStep(1);
        pad_record_update((u8 *)pad, 0x14);
    }
    SoundPlay(2, 1);
    sprite_run_off(0x94, 1);
    MessageLinesClearQuick();
    return sel;
}
/* The field message window's four-way prompt (one caller, ScriptPagesRun,
   which passes its current answer and two MessageAnswer on its stack).
   Returns 0..3 by the button that ended it. The answer passed in is not
   read: sel is set to 0 on entry.

   1. The two strings are drawn into the window's rows 4 and 3, in that
      order: row counter g_MessageLine = 4, pen font_target_set(0x801CDD00),
      string font_string_put(str[0].id, str[0].arg, 1), MessageLineDisp(-1);
      then row 3, pen 0x801CD480, str[1] and MessageLineDisp(-1). (The
      two-answer prompt MessageAnswerRun calls ankprint_rows_clear and
      MessageLineDisp(1) here.)
   2. The cursor: MessageCursorSet(0x110, 0x10, 0x7B1, 0), then the render
      entry *g_MessageCursorEntry gets x = g_MapViewX + 0x18, +4 = g_MapViewY + 0xE0
      and y = g_MapViewY + sel * 16 + 16 (sel is 0: the first row).
   3. The pad record g_MessagePadRecord is started (pad_record_init(rec, 0x14,
      0x5040, 0)) and one frame runs per pass until its REPEAT word has
      one of four bits, tested in this order: 0x4000 -> 0, 0x40 -> 1,
      0x1000 -> 2, 0x10 -> 3. A pass without one sets the cursor's y
      again (sel is still 0, so it never moves), copies the entry
      (0x24 bytes) to g_SpriteList + 0x14D0, ScriptFrameStep(1),
      pad_record_update(rec, 0x14).
   4. Sound 1 (SoundPlay(1, 1)), sprite_run_off(0x94, 1) (sprite slot
      0x94 cleared) and MessageLinesClearQuick() (MessageLineDisp(-1) for each row).
   No RNG.

   Shape: (1) the four exits are one if / else-if chain whose last else
   holds the frame and `continue`s, with a single `break` after the
   chain. With `if (...) { sel = 0; break; }` first in the loop the
   compiler copies the first test to the loop's bottom (70/119); the
   target tests only at the top and ends the body with a plain `j`.
   (2) sel is u8 (the caller passes `andi a0,s1,0xFF`, as MessageAnswerRun's
   does). As an s32 its zero is reused for the two `0` arguments
   (`move a3,s1` twice, 117/119); no mask appears on the u8 because
   every value it takes is 0..3. A separate u8 local beside an unused u8
   parameter gives the same bytes.
   (3) No pointer local for the pad record: each test reads
   ((PadRecord *)g_MessagePadRecord)->repeat and both calls take the symbol. The
   member reads force the record's address into a register; loop.c
   (-dL) moves that and its `high` half out of the loop and matches the
   in-loop call's `high` to it, which is the saved-register half behind
   `addiu a0,s3,%lo`. With a pointer local the call's `high` is the only
   movable ("savings 1 not desirable") and the loop rebuilds the address
   each pass (104/119).
   (4) The row offset through an s16 local, as in MessageAnswerRun (117/119
   inside the sum). */
/* func_8005B894 */
s32 MessagePageRun(u8 sel, MessageAnswer *str)
{
    s16 y;

    sel = 0;
    g_MessageLine = 4;
    font_target_set((u8 *)0x801CDD00);
    font_string_put(str[0].string_id, str[0].string, 1);
    MessageLineDisp(-1);
    g_MessageLine = 3;
    font_target_set((u8 *)0x801CD480);
    font_string_put(str[1].string_id, str[1].string, 1);
    MessageLineDisp(-1);
    MessageCursorSet(0x110, 0x10, 0x7B1, 0);
    ((SpriteEntry *)g_MessageCursorEntry)->x = g_MapViewX + 0x18;
    y = sel * 16 + 16;
    ((SpriteEntry *)g_MessageCursorEntry)->y = g_MapViewY + y;
    ((SpriteEntry *)g_MessageCursorEntry)->depth = g_MapViewY + 0xE0;
    pad_record_init((s32 *)g_MessagePadRecord, 0x14, 0x5040, 0);
    for (;;) {
        if (((PadRecord *)g_MessagePadRecord)->repeated & 0x4000) {
            sel = 0;
        } else if (((PadRecord *)g_MessagePadRecord)->repeated & 0x40) {
            sel = 1;
        } else if (((PadRecord *)g_MessagePadRecord)->repeated & 0x1000) {
            sel = 2;
        } else if (((PadRecord *)g_MessagePadRecord)->repeated & 0x10) {
            sel = 3;
        } else {
            y = sel * 16 + 16;
            ((SpriteEntry *)g_MessageCursorEntry)->y = g_MapViewY + y;
            *(SpriteEntry *)(g_SpriteList + 0x14D0) = *(SpriteEntry *)g_MessageCursorEntry;
            ScriptFrameStep(1);
            pad_record_update(g_MessagePadRecord, 0x14);
            continue;
        }
        break;
    }
    SoundPlay(1, 1);
    sprite_run_off(0x94, 1);
    MessageLinesClearQuick();
    return sel;
}

s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);         /* extern */
s32 CellRowsClear(s32, s32, s32);                           /* extern */
s32 CellNumberSet(s32, s32, s32, s32, s32, s32, s32, s32);  /* extern */
s32 CellGlyphSet(s32, s32, s32, s32, s32, s32);              /* extern */
void MessageNoticeDisp(u16, u16);                       /* extern */

/* func_8005BA70 */
void MessageNoticeNumberDisp(s32 amount, s32 notice_kind) {
    MapStatusClear();
    g_MessageNoticeBand = 2;
    ScriptFrameStep(1);
    CellRowsClear(2, 4, 0x20);
    CellTextDisp(0x11, 4, 7, 1, 0x11, 0xDEAD2001, 0xEE10);
    CellNumberSet(amount, 0xB, 4, 5, 0xDEAD2001, 0x3000, 1, 0);
    CellGlyphSet(0xF, 0x10, 4, 0xDEAD2001, 0x1000, 1);
    CellAllUpdate(0x20);
    ScriptFrameStep(1);
    MessageNoticeDisp(0, (notice_kind + 0x65A) & 0xFFFF);
    g_MessageNoticeBand = 0;
}
extern s32 D_800E4768[];
s32 CellIconSet(s32, s32, s32, s32, s32);

/* MessageNoticeItemDisp(item, n) -- the field's item notice: the item's ICON beside
   the one-line notice "<verb> <item name>", with the notice's button wait.
   Callers: ScriptCommandChrItemGive with (item, k + 1) and ScriptCommandChrItemRemove with
   (item, k), both only when k != 1; MessageNoticeNumberDisp just above is its twin
   for a number instead of an item. n picks the verb: message 0x65A + n
   (0x65A.. are Lost / Got / Use / Gave / Bet); the item's name is string
   (item & 0xFF) | 0x100. In order:
     1. MapStatusClear(): when g_MapIdleCount has reached 0x12C or g_MapStatusRequest
        is non-zero -- g_MapIdleCount = 0, g_MapStatusRequest = 2, MapStatusDisp()
        and one field frame. Otherwise nothing.
     2. g_MessageNoticeBand = 1 -- the band every field frame draws from now on
        (ScriptFrameStep: packet_box_add(0, 0x14, 0x140, 0x20, 0x2000000,
        0x40), i.e. y 0x14, 0x140 wide, 0x20 high; MessageNoticeNumberDisp uses 2,
        the 0x18-high band at y 0x1C). n += 0x65A. One field frame.
     3. CellRowsClear(3, 3, 0x10): rows 3..5 of the 40 x 28 cell grid
        lose their cells of class ((cell >> 8) & 7) >= 3, the icon cells
        (ankprint_area_clear(0, 3, 0x28, 3, 0x10)). Then CellIconSet(4, 3,
        0xDEAD0001, item & 0xFF, 0), which is IconCellSet(4, 3, item, 0):
        icon `item` (24 x 24) at grid cell (4, 3), not dimmed.
        CellAllUpdate(0x10) is empty.
     4. D_800E4768[3] = 0xA00: the SIZE word of the upload request block,
        and only that word -- neither the address word [1] nor the pending
        flag [0] is written here. One field frame.
     5. CellRowsClear(2, 4, 0x20): grid rows 2..5 lose their cells of
        class < 3 (the label cells). CellTextDisp(8, 4, 0xF, 1, 0x11,
        0xDEAD2001, 0xEE10): 0x11 cells of the text buffer 0x801CDD00
        (0x801CBB00 + ((0xEE10 - 0xED00) << 5), text row 4's buffer) are
        uploaded to label strip 4 from its cell 4 on (font_image_load(buf,
        0x44, 0x11)) and a run of 0xF of them is put on the grid at
        (8, 4) (font_cell_run_set(8, 4, 0xF, 0x44)). Nothing in this function
        writes that buffer first. CellAllUpdate(0x20) is empty. One field
        frame.
     6. MessageNoticeDisp((item & 0xFF) | 0x100, n): the notice on text
        row 5 -- message n, a space, the item's name -- drawn at grid
        (8, 4), then one frame per pass until the pad record's press word
        has bit 0x40 (no timeout), then its own close (rows cleared,
        CellRowsClear(2, 4, 0x20), CellTextDisp(3, 2, 0x11, 2, 0x11,
        0xDEAD2001, 0xEDCC), a frame).
     7. CellRowsClear(3, 3, 0x10) again: the icon's cells are taken off.
        g_MessageNoticeBand = 0 (no band). One field frame.
   Frames: four ScriptFrameStep(1) of its own (steps 2, 4, 5, 7), none or
   one in MapStatusClear, and MessageNoticeDisp's (unbounded: it waits for
   the button). It does not call the RNG itself; whether a field frame
   does was not checked. Returns nothing: the epilogue sets no $v0 and
   neither caller reads one (the TU's prototypes say s32, kept).

   Matching.
   - `u8 item`: the two uses each mask the saved register again (andi
     0xFF at the icon call, andi 0xFF + ori 0x100 at the notice) and the
     item's entry copy comes AFTER n's (addu $s1,$a1 first, addu $s0,$a0
     in the first call's delay slot) -- a narrow parameter's conversion is
     emitted after all the parameter moves. With `s32 item` and
     `item & 0xFF` written twice cse keeps one masked copy and the entry
     copies come in argument order (61/66); with the two masks spelled
     differently only the entry order is left (63/66).
   - The definition is K&R because the TU's three prototypes above say
     (s32, s32): a u8 promotes to int, so they stay valid and no call
     site changes. `u16 n` matches as well; nothing asks for it (the
     andi 0xFFFF at the notice call is MessageNoticeDisp's u16
     parameter).
   - `n += 0x65A` is a statement of its own after the g_MessageNoticeBand store:
     the target adds it in the first frame call's delay slot and keeps
     the sum in $s1 across five calls. Written inside the notice call
     (the twin's spelling) it is computed there instead.
   - The size word is D_800E4768[3], the request block as the other TUs
     declare it (an incomplete array, reached lui/lo), not a scalar
     D_800E4774. */
/* func_8005BB6C */
s32 MessageNoticeItemDisp(item, n)
u8 item;
s32 n;
{
    MapStatusClear();
    g_MessageNoticeBand = 1;
    n += 0x65A;
    ScriptFrameStep(1);
    CellRowsClear(3, 3, 0x10);
    CellIconSet(4, 3, 0xDEAD0001, item, 0);
    CellAllUpdate(0x10);
    D_800E4768[3] = 0xA00;
    ScriptFrameStep(1);
    CellRowsClear(2, 4, 0x20);
    CellTextDisp(8, 4, 0xF, 1, 0x11, 0xDEAD2001, 0xEE10);
    CellAllUpdate(0x20);
    ScriptFrameStep(1);
    MessageNoticeDisp(item | 0x100, n);
    CellRowsClear(3, 3, 0x10);
    g_MessageNoticeBand = 0;
    ScriptFrameStep(1);
}

/* far_syms: g_MenuPage */

/* func_8005BC74 */
void MessageLinesClear(void) {
    s32 line_count;
    u16 next_line;

    g_MessageLine = 0;
    line_count = 5;
    if (g_MenuPage != 0) {
        line_count = 3;
    }
    font_target_clear(0x801CBB00, (line_count * 0x10) | line_count);
    if ((s32) g_MessageLine < line_count) {
        do {
            MessageLineDisp(1);
            next_line = g_MessageLine + 1;
            g_MessageLine = next_line;
        } while ((next_line & 0xFFFF) < line_count);
    }
    g_MessageLine = 0;
}

/* func_8005BCFC */
void MessageLinesClearQuick(void) {
    s32 line_count;
    u16 next_line;

    g_MessageLine = 0;
    line_count = 5;
    if (g_MenuPage != 0) {
        line_count = 3;
    }
    font_target_clear(0x801CBB00, (line_count * 0x10) | line_count);
    if ((s32) g_MessageLine < line_count) {
        do {
            MessageLineDisp(-1);
            next_line = g_MessageLine + 1;
            g_MessageLine = next_line;
        } while ((next_line & 0xFFFF) < line_count);
    }
    g_MessageLine = 0;
}
extern u8 D_80012984[];
extern u8 D_800B87E4[];
void font_string_proportional_put(s32 string_id);
void font_bytes_put(u8 *string, s32 mode);
void font_string_put(s32, s32, s32);
s32 ankprint_rows_clear(s32, s32, s32);

/* MessageNoticeDisp — the field's one-line notice "<verb> <name>" with a
   button wait: MessageNoticeItemDisp calls it with (item | 0x100, message)
   and MessageNoticeNumberDisp with (0, 0x65A + n) after drawing a number
   itself (messages 0x65A.. are Lost / Got / Use / Gave / Bet). Both
   arguments are string ids of the offset table at g_FontStringTable.
     - text row g_MessageLine = 5, sound 0xC on channel 1
       (SoundPlay(0xC, 1)); the pen goes to that row's buffer,
       0x801CBB00 + 5 * 0x880, with 0x10 cells of 0x80 bytes cleared
       (font_target_clear, then font_target_set on the same address);
     - no1 == 0: message msg_id alone (font_string_proportional_put);
     - no1 != 0 and its table entry is non-zero:
       debug_printf("XX no1=%x\n", no1), message msg_id, a frame
       (ScriptFrameStep(1)), then the one-character string at D_800B87E4 (a
       space; font_bytes_put(.., 1)), the name's text
       (font_string_put(0, text, 1)) and another frame. The text pointer is
       tested against 0 first (table base + a non-zero offset);
     - no1 != 0 with a zero table entry: nothing is drawn at all, not
       even the message;
     - no1 != 0 (either way): ankprint_rows_clear(4, 1, 0x20);
     - MessageLineDisp(1) (draw the row), then the pad record g_MessagePadRecord is
       started (pad_record_init(rec, 0x14, 0x40, 0)) and the function loops
       one frame per pass (ScriptFrameStep(1), pad_record_update(rec, 0x14))
       until the record's press word (rec+2) has bit 0x40. No timeout;
     - the close: MessageLinesClear() (rows cleared), CellRowsClear(2, 4,
       0x20), CellTextDisp(3, 2, 0x11, 2, 0x11, 0xDEAD2001, 0xEDCC),
       CellAllUpdate(0x20), one more frame.
   No RNG. Returns nothing: the epilogue sets no $v0 and neither caller
   reads one.
   Shape:
   - u16 parameters: every use re-masks the saved register (andi 0xFFFF,
     six times) and both callers pass values already cut to 16 bits.
   - The table entry is written out twice, once in the test and once in
     the sum, as MessageControlRun writes its splice. cse then keeps the
     entry's ADDRESS in its own register ($v1) for the second read; with
     the entry held in a local the address and the value share $v0
     (116/118).
   - debug_printf takes the format and the id only. The table base sits
     in $a2 across the call because that is where local-alloc puts it,
     not because it is a third argument.
   - `pad`: the frame is 0x40 with seven outgoing argument words (0x20
     after rounding) and four saved registers, which leaves 16 bytes at
     0x20-0x2F that no instruction touches. Without a 9..16-byte local the
     frame is 0x30 (108/118). */
/* func_8005BD84 */
void MessageNoticeDisp(u16 no1, u16 msg_id)
{
    u8 *name;
    u8 *rec;
    u8 pad[16];     /* MARKED SPELLING (unused local): the target frame (0x40) holds
                       16 bytes at 0x20-0x2F that no instruction touches -- the
                       original's local(s), declaration unknown */

    g_MessageLine = 5;
    SoundPlay(0xC, 1);
    font_target_clear(0x801CBB00 + g_MessageLine * 0x880, 0x10);
    font_target_set((u8 *)0x801CBB00 + g_MessageLine * 0x880);
    if (no1) {
        if (*(u16 *)(no1 * 2 + g_FontStringTable) != 0) {
            name = (u8 *)(g_FontStringTable + *(u16 *)(no1 * 2 + g_FontStringTable));
            debug_printf(D_80012984, no1);
            font_string_proportional_put(msg_id);
            ScriptFrameStep(1);
            if (name != 0) {
                font_bytes_put(D_800B87E4, 1);
                font_string_put(0, (s32)name, 1);
                ScriptFrameStep(1);
            }
        }
    } else {
        font_string_proportional_put(msg_id);
    }
    if (no1) {
        ankprint_rows_clear(4, 1, 0x20);
    }
    MessageLineDisp(1);
    rec = g_MessagePadRecord;
    pad_record_init((s32 *)rec, 0x14, 0x40, 0);
    while (!(((PadRecord *)rec)->pressed & 0x40)) {
        ScriptFrameStep(1);
        pad_record_update(rec, 0x14);
    }
    MessageLinesClear();
    CellRowsClear(2, 4, 0x20);
    CellTextDisp(3, 2, 0x11, 2, 0x11, 0xDEAD2001, 0xEDCC);
    CellAllUpdate(0x20);
    ScriptFrameStep(1);
}

/* func_8005BF5C */
void ScriptCommandLabelJump()
{
  s32 command;
  u16 label_offset;
  u16 label;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = 2 + command;
  label = *((u16 *) (command + 2));
  label_offset = *((u16 *) ((label * 2) + g_ScriptBase));
  if (1)
  {
    if (label_offset == 0)
    {
      g_ScriptErrorNumber = 0;
      ScriptErrorHang();
    }
    g_ScriptLabel = *((u16 *) g_ScriptCommandPointer);
  }
  g_ScriptCommandPointer = g_ScriptBase + (label_offset * 2);
  return g_ScriptCommandPointer;
}

void ScriptAnswerRun();                                  /* extern */
void ScriptPagesRun();                                  /* extern */
void ScriptCommandAnswerRun(void);
void ScriptCommandStoryBits580Clear(void);
void ScriptCommandStoryBits480Set(void);
void ScriptCommandStoryBits480Clear(void);
void func_8005E608();
s32 VoiceStart();                                  /* extern */
void ScriptErrorHang();                                  /* extern */
void ScriptCommandVoiceStart(void);
void ScriptCommandVoiceEndWait(void);
void ScriptErrorReport();
s32 mem_clear(s32, s32);                            /* extern */
void MapSpriteDataMarksClear(void);
s32 ScriptListCount(s32, s32);                              /* extern */
void MapPartyBoxSet(void);
void MapReload(void);
void ScriptChange(u8 script_number);

/* func_8005BFCC */
void ScriptCommandScriptChange(void) {
    s32 command;
    s32 masked_label;
    u16 label_offset;
    u16 label;

    command = g_ScriptCommandPointer;
    g_ScriptCommandPointer = command + 2;
    label = (*(s16 *)((u8 *)command + 0x2));
    g_ScriptCommandPointer = command + 4;
    ScriptChange((*(s32 *)((u8 *)command + 0x4)));
    masked_label = label & 0xFFFF;
    if (masked_label == 0) {
        g_ScriptEnded = 1;
        return;
    }
    label_offset = (*(s16 *)((masked_label * 2) + g_ScriptBase));
    if (label_offset == 0) {
        g_ScriptErrorNumber = 0;
        ScriptErrorHang();
    }
    g_ScriptLabel = label;
    g_ScriptCommandPointer = g_ScriptBase + (label_offset * 2);
}

/* func_8005C06C */
void ScriptCommandLabelCall(void) {
    s32 depth;

    if (g_ScriptLabelCallDepth >= 8) {
        g_ScriptErrorNumber = 1;
        ScriptErrorHang();
    }
    depth = g_ScriptLabelCallDepth;
    ((s32 *)g_ScriptLabelCallKeptPointers)[depth] = g_ScriptCommandPointer + 4;
    g_ScriptLabelCallDepth = depth + 1;
    ((u16 *)g_ScriptLabelCallKeptLabels)[depth] = g_ScriptLabel;
    ScriptCommandLabelJump();
}

/* far_syms: g_ScriptLabelCallKeptLabels (target reaches it %hi/%lo) */
/* far_syms: g_ScriptLabelCallKeptPointers (target reaches it %hi/%lo) */

/* func_8005C0E4 */
void ScriptCommandLabelReturn(void) {
    if (g_ScriptLabelCallDepth == 0) {
        g_ScriptEnded = 1;
        return;
    }

    g_ScriptLabelCallDepth--;
    g_ScriptCommandPointer = ((s32 *)g_ScriptLabelCallKeptPointers)[g_ScriptLabelCallDepth];
    g_ScriptLabel = ((u16 *)g_ScriptLabelCallKeptLabels)[g_ScriptLabelCallDepth];
}

/* func_8005C144 */
s32 ScriptStringEndFind(s32 even) {
    s32 string_end;
    string_end = g_ScriptCommandPointer;
    while (*(u8 *)string_end != 0xFF)
        string_end++;
    string_end++;
    if (even && (string_end & 1))
        string_end++;
    return string_end;
}
/* ScriptAnswerRun -- script command 7 (SELECT) in its TWO-ANSWER form:
   ScriptCommandAnswerRun calls it for every script number but 0 and 0xC8
   (those get the paged form, ScriptPagesRun above). The entry is the same:
   [u16 7][u16 label][u8 start answer][pad], then two strings that each end
   on 0xFF.

   ScriptMoveEndWait() once; p walks the entry: the command's address is kept,
   the cursor g_ScriptCommandPointer goes to cmd + 4 and the start answer is the byte
   there; ScriptPointerAdvance(2) moves the cursor on to the strings; two MessageAnswer
   on the stack, both ids 0, each arg = the cursor at its string, the cursor
   stepped past each with ScriptStringEndFind(0) / (1). Then the two-way prompt
   MessageAnswerRun(sel, str) -- which, unlike the paged form's MessagePageRun,
   DOES start on the entry's start answer -- and on answer 1 the cursor goes
   back to the command and ScriptCommandLabelJump() jumps to the entry's label. Any
   other answer returns with the cursor on the next command.

   Shape: the cursor is a byte pointer local walked from
   the command (p += 2, then p + 2 for the new cursor and p[2] for the start
   answer): the target loads the cursor into $v0, copies it to $s1 for the
   command's address and adds the call's hoisted 2 twice. With the two sums
   written from `cmd` (the paged form's spelling) it is 32/40; the cursor
   bumped through the global 32-35. */
/* func_8005C198 */
void ScriptAnswerRun(void)
{
    MessageAnswer str[2];
    s32 cmd;
    u8 *p;
    u8 sel;

    ScriptMoveEndWait();
    p = (u8 *)g_ScriptCommandPointer;
    cmd = (s32)p;
    p += 2;
    g_ScriptCommandPointer = (s32)(p + 2);
    sel = p[2];
    ScriptPointerAdvance(2);
    str[0].string_id = str[1].string_id = 0;
    str[0].string = g_ScriptCommandPointer;
    g_ScriptCommandPointer = ScriptStringEndFind(0);
    str[1].string = g_ScriptCommandPointer;
    g_ScriptCommandPointer = ScriptStringEndFind(1);
    if ((u8)MessageAnswerRun(sel, str) == 1) {
        g_ScriptCommandPointer = cmd;
        ScriptCommandLabelJump();
    }
}

extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;

/* func_8005C238 */
void ScriptPagePush(s32 command)
{
  int page_byte_offset;
  u8 *ring;
  u16 ring_index;
  s32 page_offset;
  u16 next_ring_index;
  ring_index = g_ScriptPageRingIndex;
  page_byte_offset = command - g_ScriptBase;
  page_offset = page_byte_offset;
  next_ring_index = ring_index + 1;
  g_ScriptPageRingIndex = next_ring_index;
  page_offset >>= 1;
  ring = g_ScriptPageRingEntries;
  *((u16 *) (ring + (ring_index * 2))) = page_offset;
  g_ScriptPageRingIndex = next_ring_index & 0x7F;
}

/* far_syms: g_ScriptPageRingEntries (target reaches it %hi/%lo) */

/* func_8005C278 */
s32 ScriptPagePop(void) {
    u16 page_offset;

    g_ScriptPageRingIndex = (g_ScriptPageRingIndex - 1) & 0x7F;
    page_offset = *(u16 *)(g_ScriptPageRingEntries + (g_ScriptPageRingIndex * 2));
    if (page_offset == 0) {
        return 0;
    }
    return g_ScriptBase + (page_offset * 2);
}
/* Script command 7 (SELECT, g_ScriptCommandFunctions[7] = ScriptCommandAnswerRun) in its PAGED
   form: ScriptCommandAnswerRun calls this when the script number g_ScriptNumber is 0
   or 0xC8, and the two-answer form ScriptAnswerRun otherwise. The entry is
   the same for both: [u16 7][u16 label][u8][pad], then two strings that
   each end on 0xFF, the second end rounded up to even.

   ScriptMoveEndWait() once (what the TEXT command ScriptCommandMessageDisp calls before
   a message; its MoveChrCommandEndWait(0x3FFFF) waits out every mover slot).
   Then, per page:
   1. cmd = the command's address; the cursor g_ScriptCommandPointer goes to cmd + 4,
      the byte there is the answer to start on (the four-way prompt does
      not read it), and ScriptPointerAdvance(2) moves the cursor on to cmd + 6.
   2. Two MessageAnswer, both with id 0: str[0].arg = the first string,
      str[1].arg = the second (ScriptStringEndFind(0): one past the first
      0xFF); ScriptStringEndFind(1) then leaves the cursor on the next command.
   3. Unless skipping (below), sel = MessagePageRun(sel, str), the
      four-way prompt: 0..3 by the pad's repeat bit, 0x4000 -> 0,
      0x40 -> 1, 0x1000 -> 2, 0x10 -> 3.
      0  ScriptPagePush(cmd): this page's offset goes onto the 128-entry
         ring g_ScriptPageRingEntries, and the script runs on with the next command.
      1  the cursor is put back on the command and ScriptCommandLabelJump() jumps
         to its label (the u16 at cmd + 2).
      2  ScriptPagePop() takes the last offset off the ring: 0 (nothing
         there) lets the script run on; otherwise that page is shown
         again.
      3  this page goes onto the ring and, while the next command's id
         is 7 too, the following pages are walked WITHOUT a prompt
         (skip = 1 makes sel 3 again), each going onto the ring; at the
         first other command the last page is taken back off the ring
         and shown with its prompt.
   No RNG draw of its own; the frames are ScriptMoveEndWait's wait and the
   prompt's.

   Shape: (1) the loop is a label and gotos. Written `for (;;)` with
   `continue` the body is the same instructions with skip and back in
   each other's registers (83/91): global.c ranks by refs / live length,
   and flow counts a reference inside a loop note twice, which puts
   back (0.667) above skip (0.298); with no loop note they are 0.167 and
   0.170, skip first, the target's $s2 / $s3.
   (2) `skip = 0` is written AFTER the null test of the taken-off page.
   Before it, the tails of answers 2 and 3 are equal and cross-jumping
   merges them (78/91); the target keeps both, with the clear in the
   test's delay slot.
   (3) sel is u8: the call's argument and the switch are each masked
   (`andi a0,s1,0xFF`). As an s32 switched on `sel & 0xFF` the call's
   mask is a plain move (90/91).
   (4) cmd is reused for the page taken off the ring, which is why the
   command's own address is kept a second time in back for answer 1. A
   separate local for the taken-off page with cmd used for answer 1 is
   58/91.
   (5) The two id stores are one chained assignment, so str[1].id is
   stored first as in the target; id / arg written record by record is
   90/91. */
/* func_8005C2C0 */
void ScriptPagesRun(void)
{
    MessageAnswer str[2];
    s32 skip;
    s32 cmd;
    s32 back;
    u8 sel;

    skip = 0;
    ScriptMoveEndWait();
again:
    cmd = g_ScriptCommandPointer;
    back = cmd;
    g_ScriptCommandPointer = cmd + 4;
    sel = *(u8 *)(cmd + 4);
    ScriptPointerAdvance(2);
    str[0].string_id = str[1].string_id = 0;
    str[0].string = g_ScriptCommandPointer;
    g_ScriptCommandPointer = ScriptStringEndFind(0);
    str[1].string = g_ScriptCommandPointer;
    g_ScriptCommandPointer = ScriptStringEndFind(1);
    if (skip == 0) {
        sel = MessagePageRun(sel, str);
    } else {
        sel = 3;
    }
    switch (sel) {
    case 0:
        ScriptPagePush(cmd);
        break;
    case 1:
        g_ScriptCommandPointer = back;
        ScriptCommandLabelJump();
        break;
    case 2:
        cmd = ScriptPagePop();
        if (cmd == 0) {
            break;
        }
        g_ScriptCommandPointer = cmd;
        goto again;
    case 3:
        ScriptPagePush(cmd);
        skip = 1;
        if (*(u16 *)g_ScriptCommandPointer == 7) {
            goto again;
        }
        cmd = ScriptPagePop();
        if (cmd == 0) {
            break;
        }
        skip = 0;
        g_ScriptCommandPointer = cmd;
        goto again;
    }
}

void ScriptAnswerRun();                                  /* extern */
void ScriptPagesRun();                                  /* extern */

/* func_8005C42C */
void ScriptCommandAnswerRun(void) {
    if ((g_ScriptNumber == 0) || (g_ScriptNumber == 0xC8)) {
        ScriptPagesRun();
        return;
    }
    ScriptAnswerRun();
}

s32 rng_draw();                                /* extern */
void ScriptCommandLabelJump();                                  /* extern */

/* func_8005C470 */
void ScriptCommandLabelPick(void)
{
  s32 pick;
  s32 label_count;
  s32 command;
  s32 pick_offset;
  command = g_ScriptCommandPointer;
  ScriptPointerAdvance(2);
  label_count = ScriptListCount(2, 1);
  pick = rng_draw() % ((s32) (label_count + 1));
  if (pick != 0)
  {
    pick_offset = (pick * 2) - 2;
    g_ScriptCommandPointer = command + pick_offset;
    ScriptCommandLabelJump();
  }
}

s32 StoryBitSet(u16);                               /* extern */

/* func_8005C504 */
void ScriptCommandStoryBitsSet(void) {
    s32 bit_count;
    s32 i;
    s32 bits;
    u16 bit;

    bits = ScriptPointerAdvance(2);
    bit_count = ScriptListCount(2, 1);
    i = 0;
    if (bit_count > 0) {
        do {
            bit = (*(s32 *)bits);
            bits += 2;
            i += 1;
            StoryBitSet(bit);
        } while (i < bit_count);
    }
}

s32 StoryBitClear(u16);                               /* extern */

/* func_8005C570 */
void ScriptCommandStoryBitsClear(void) {
    s32 bit_count;
    s32 i;
    s32 bits;
    u16 bit;

    bits = ScriptPointerAdvance(2);
    bit_count = ScriptListCount(2, 1);
    i = 0;
    if (bit_count > 0) {
        do {
            bit = (*(s32 *)bits);
            bits += 2;
            i += 1;
            StoryBitClear(bit);
        } while (i < bit_count);
    }
}

/* func_8005C5DC */
void ScriptCommandStoryBitsAllCheck(void)
{
  s32 command;
  s32 bit_count;
  s32 i;
  s32 all_set;
  u16 *bits;
  u16 bit;
  all_set = 1;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  bits = ScriptPointerAdvance(2);
  i = 0;
  bit_count = ScriptListCount(2, 1);
  while (i < bit_count)
  {
    bit = *bits;
    bits += 1;
    i += 1;
    all_set &= StoryBitCheck(bit);
  }
  if (all_set != 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005C684 */
void ScriptCommandStoryBitsNoneCheck(void)
{
  s32 command;
  s32 bit_count;
  s32 any_set;
  s32 i;
  u16 *bits;
  u16 bit;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  bits = ScriptPointerAdvance(2);
  any_set = 0;
  i = 0;
  bit_count = ScriptListCount(2, 1);
  while (i < bit_count)
  {
    bit = *bits;
    bits += 1;
    i += 1;
    any_set |= StoryBitCheck(bit);
  }
  if (any_set == 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005C72C */
void ScriptCommandStoryBitsAnyCheck(void)
{
  s32 command;
  s32 bit_count;
  s32 any_set;
  s32 i;
  u16 *bits;
  u16 bit;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  bits = ScriptPointerAdvance(2);
  any_set = 0;
  i = 0;
  bit_count = ScriptListCount(2, 1);
  while (i < bit_count)
  {
    bit = *bits;
    bits += 1;
    i += 1;
    any_set |= StoryBitCheck(bit);
  }
  if (any_set != 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005C7D4 */
void ScriptCommandStoryBits580Set(void) {
    s32 bit_count;
    s32 i;
    u8 *bit_offsets;
    u8 bit_offset;

    bit_offsets = (u8 *)ScriptPointerAdvance(2);
    bit_count = ScriptListCount(1, 1);
    i = 0;
    if (bit_count > 0) {
        do {
            bit_offset = *bit_offsets;
            bit_offsets += 1;
            i += 1;
            StoryBitSet(bit_offset + 0x580);
        } while (i < bit_count);
    }
}

/* func_8005C844 */
void ScriptCommandStoryBits580Clear(void) {
    s32 bit_count;
    s32 i;
    u8 *bit_offsets;
    u8 bit_offset;

    bit_offsets = (u8 *)ScriptPointerAdvance(2);
    bit_count = ScriptListCount(1, 1);
    i = 0;
    if (bit_count > 0) {
        do {
            bit_offset = *bit_offsets;
            bit_offsets += 1;
            i += 1;
            StoryBitClear(bit_offset + 0x580);
        } while (i < bit_count);
    }
}

s32 MusicLevelStep(s32, s32);
s32 StoryBitCheck(s32);

/* func_8005C8B4 */
void ScriptCommandStoryBits580AllCheck(void)
{
  s32 command;
  s32 bit_count;
  s32 i;
  s32 all_set;
  u8 *bit_offsets;
  u8 bit_offset;
  all_set = 1;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  i = 0;
  bit_offsets = ScriptPointerAdvance(2);
  bit_count = (unsigned long) ScriptListCount(1, 1);
  while (i < bit_count)
  {
    bit_offset = *bit_offsets;
    bit_offsets += 1;
    i += 1;
    all_set &= StoryBitCheck(bit_offset + 0x580);
  }
  if (all_set != 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

s32 MusicLevelStep(s32, s32);
s32 StoryBitCheck(s32);

/* func_8005C960 */
void ScriptCommandStoryBits580NoneCheck(void)
{
  s32 command;
  s32 bit_count;
  s32 any_set;
  s32 i;
  u8 *bit_offsets;
  u8 bit_offset;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  bit_offsets = ScriptPointerAdvance(2);
  any_set = 0;
  i = 0;
  bit_count = ScriptListCount(1, 1);
  while (i < bit_count)
  {
    bit_offset = *bit_offsets;
    bit_offsets += 1;
    i += 1;
    any_set |= StoryBitCheck(bit_offset + 0x580);
  }
  if (any_set == 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005CA0C */
void ScriptCommandStoryBits580AnyCheck(void)
{
  s32 command;
  s32 bit_count;
  s32 any_set;
  s32 i;
  u8 *bit_offsets;
  u8 bit_offset;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  bit_offsets = ScriptPointerAdvance(2);
  any_set = 0;
  i = 0;
  bit_count = ScriptListCount(1, 1);
  while (i < bit_count)
  {
    bit_offset = *bit_offsets;
    bit_offsets += 1;
    i += 1;
    any_set |= StoryBitCheck(bit_offset + 0x580);
  }
  if (any_set != 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005CAB8 */
void ScriptCommandStoryBitPairsSet(void)
{
  s32 pair_count;
  s32 i;
  s32 low_byte;
  u8 high_byte;
  u8 *pair;
  s32 high_part;
  pair = ScriptPointerAdvance(2);
 /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
    block runs once, so behaviour is the same without the `do` / `while (0)`, but then 7
    of this function's 32 instruction lines differ. */
 do { pair_count = ScriptListCount(1, 2); i = 0; } while (0);
  if (pair_count > 0)
  {
    do
    {
      low_byte = pair[1];
      high_byte = pair[0];
      pair += 2;
      i += 1;
      high_part = (high_byte << 7) - 0x2C00;
      StoryBitSet((low_byte + high_part) & 0xFFFF);
    }
    while (i < pair_count);
  }
}

/* func_8005CB38 */
void ScriptCommandStoryBitPairsClear(void)
{
  s32 pair_count;
  s32 count_then_i;
  s32 low_byte;
  u8 high_byte;
  u8 *pair;
  s32 high_part;
  pair = ScriptPointerAdvance(2);
  count_then_i = ScriptListCount(1, 2);
  pair_count = count_then_i;
  count_then_i = 0;
  if (pair_count > 0)
  {
    do
    {
      low_byte = pair[1];
      high_byte = pair[0];
      pair += 2;
      count_then_i += 1;
      high_part = (high_byte << 7) - 0x2C00;
      StoryBitClear((low_byte + high_part) & 0xFFFF);
    }
    while (count_then_i < pair_count);
  }
}

/* func_8005CBB8 */
void ScriptCommandStoryBitPairsAllCheck(void)
{
  s32 command;
  s32 pair_count;
  s32 pair;
  s32 i;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `bit_base` only holds the constant 0x2C00; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 47 instruction lines
     differs. */
  int bit_base;
  s32 all_set;
  s32 low_byte;
  u8 high_byte;
  all_set = 1;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  i = 0;
  pair = ScriptPointerAdvance(2);
  pair_count = ScriptListCount(1, 2);
  while (i < pair_count)
  {
    low_byte = *((u8 *) (((u8 *) pair) + 0x1));
    high_byte = *((s32 *) (((u8 *) pair) + 0x0));
    pair += 2;
    i += 1;
    bit_base = 0x2C00;
    all_set &= StoryBitCheck((low_byte + ((high_byte << 7) - bit_base)) & 0xFFFF);
  }
  if (all_set != 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005CC74 */
void ScriptCommandStoryBitPairsNoneCheck(void)
{
  s32 pair_count;
  s32 any_set;
  s32 i;
  s32 low_byte;
  s32 command;
  u8 high_byte;
  u8 *pair;
  s32 high_part;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  any_set = 0;
  pair = ScriptPointerAdvance(2);
  pair_count = ScriptListCount(1, 2);
  i = 0;
  while (i < pair_count)
  {
    low_byte = pair[1];
    high_byte = pair[0];
    pair += 2;
    i += 1;
    high_part = (high_byte << 7) - 0x2C00;
    any_set |= StoryBitCheck((low_byte + high_part) & 0xFFFF);
  }
  if (any_set == 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005CD30 */
void ScriptCommandStoryBitPairsAnyCheck(void)
{
  s32 pair_count;
  s32 any_set;
  s32 i;
  s32 low_byte;
  s32 command;
  u8 high_byte;
  u8 *pair;
  s32 high_part;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  any_set = 0;
  pair = ScriptPointerAdvance(2);
  pair_count = ScriptListCount(1, 2);
  i = 0;
  while (i < pair_count)
  {
    low_byte = pair[1];
    high_byte = pair[0];
    pair += 2;
    i += 1;
    high_part = (high_byte << 7) - 0x2C00;
    any_set |= StoryBitCheck((low_byte + high_part) & 0xFFFF);
  }
  if (any_set != 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* func_8005CDEC */
void ScriptCommandStoryBits480Set(void) {
    s32 bit_count;
    s32 i;
    u8 *bit_offsets;
    u8 bit_offset;

    bit_offsets = (u8 *)ScriptPointerAdvance(2);
    bit_count = ScriptListCount(1, 1);
    i = 0;
    if (bit_count > 0) {
        do {
            bit_offset = *bit_offsets;
            bit_offsets += 1;
            i += 1;
            StoryBitSet(bit_offset + 0x480);
        } while (i < bit_count);
    }
}

/* func_8005CE5C */
void ScriptCommandStoryBits480Clear(void) {
    s32 bit_count;
    s32 i;
    u8 *bit_offsets;
    u8 bit_offset;

    bit_offsets = (u8 *)ScriptPointerAdvance(2);
    bit_count = ScriptListCount(1, 1);
    i = 0;
    if (bit_count > 0) {
        do {
            bit_offset = *bit_offsets;
            bit_offsets += 1;
            i += 1;
            StoryBitClear(bit_offset + 0x480);
        } while (i < bit_count);
    }
}

/* func_8005CECC */
void ScriptCommandStoryBitPairs2620Set(void)
{
  s32 shifted_high_part;
  s32 pair_count;
  s32 list_count;
  s32 i;
  int low_part;
  s32 high_part_then_pair_size;
  u8 *pair;
  int high_byte_then_bit;
  pair = ScriptPointerAdvance(2);
  pair_count = (list_count = ScriptListCount(1, 2));
  i = 0;
  if (pair_count > 0)
  {
    do
    {
      low_part = pair[1] + 0x2620;
      if (((u8) pair[0]) >= 0x65U)
      {
        high_part_then_pair_size = (shifted_high_part = pair[0] - 0x35);
      }
      else
      {
        high_byte_then_bit = pair[0];
        high_part_then_pair_size = high_byte_then_bit - 0x15;
      }
      shifted_high_part = high_part_then_pair_size << 5;
      high_part_then_pair_size = 2;
      high_byte_then_bit = low_part + shifted_high_part;
      StoryBitSet(high_byte_then_bit & 0xFFFF);
      i += 1;
      pair += high_part_then_pair_size;
    }
    while (i < pair_count);
  }
}

void ScriptCommandMoneyTake(void);  /* TU defines this below */
void ScriptCommandMoneyAboveJump(void);  /* TU defines this below */
s32 ChrItemRemove(s32, s32);
s32 MessageNoticeItemDisp(s32, s32);
void ScriptCommandChrItemRemove(void);  /* TU defines this below */
void ScriptCommandActuatorListStart(void);  /* TU defines this below */
s32 PartyPlaceFind(u32);
s32 MoveChrPartyLeave(s32, s32, s32);
void ScriptCommandPartyLeave(void);  /* TU defines this below */
void MapBgChangeStart(s16, u8);
void ScriptCommandMapBgChangeStart(void);  /* TU defines this below */
s32 layer_col_set(s32, s32, s32 *);
s32 MapColFadeStart(s32, s32);
void MapColSet(s32, s32);
volatile char ScriptCommandMapColSet(void);  /* TU defines this below */
s32 ColMix(u8, s32);
void ScriptCommandColMix(void);  /* TU defines this below */
s32 ShopRun(s32);
s32 MessageRecordsInit();
void ScriptCommandShopRun(void);  /* TU defines this below */
s32 SoundPlay(u16, s32);
void ScriptCommandSoundPlay(void);  /* TU defines this below */
void ScriptCommandMusicLevelSet(void);  /* TU defines this below */
void ScriptCommandMessageTimedSet(void);  /* TU defines this below */
void ScriptCommandPartyMpClear(void);  /* TU defines this below */

/* func_8005CF6C */
void ScriptCommandMoneyAdd(void)
{
  u32 money;
  u32 amount;
  u32 new_money;
  u8 notice_kind;
  void *command;
  command = (void *) g_ScriptCommandPointer;
  g_ScriptCommandPointer = ((s32) command) + 2;
  notice_kind = *((u8 *) (((u8 *) command) + 0x2));
  amount = *((s32 *) ScriptPointerAdvance(4));
  money = *g_GameMoney;
  if (9999999 < ((u32) (money + amount)))
  {
    amount = 9999999 - money;
  }
  new_money = money + amount;
  *g_GameMoney = new_money;
  if ((notice_kind & 0xFF) != 1)
  {
    MessageNoticeNumberDisp(amount, (short) ((notice_kind & 0xFF) + 1));
  }
  g_ScriptCommandPointer += 4;
}

s32 MusicLevelStep(s32, s32);

/* func_8005CFF8 */
void ScriptCommandMoneyTake(void)
{
  s32 masked_notice_kind;
  u32 money;
  u32 amount;
  u32 new_money;
  u8 notice_kind;
  void *command;
  command = (void *)g_ScriptCommandPointer;
  g_ScriptCommandPointer = (s32)command + 2;
  notice_kind = *((s16 *) (((u8 *) command) + 0x2));
  amount = (*(s32 *)ScriptPointerAdvance(4));
  money = *g_GameMoney;
  if (money < amount)
  {
    amount = money;
    new_money = money * 0;
  }
  new_money = money - amount;
  *g_GameMoney = new_money;
  masked_notice_kind = notice_kind & 0xFF;
  if (masked_notice_kind != 1)
  {
    MessageNoticeNumberDisp(amount, masked_notice_kind);
  }
  g_ScriptCommandPointer += 4;
}

s32 MusicLevelStep(s32, s32);

/* func_8005D078 */
void ScriptCommandMoneyAboveJump(void)
{
  s32 command;
  u32 amount;
  command = g_ScriptCommandPointer;
  command++;
  command--;
  g_ScriptCommandPointer += 2;
  amount = (*(u32 *)ScriptPointerAdvance(4));
  g_ScriptCommandPointer += 4;
  if (amount < ((u32) (*g_GameMoney)))
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump(g_GameMoney);
  }
}

/* far_syms: g_ChrItemNoRoomState (target reaches it %hi/%lo) */

s32 ChrItemGive(s32, s32);                          /* extern */

/* func_8005D0E0 */
void ScriptCommandChrItemGive(void) {
    u8 notice_kind;
    u8 item;
    void *operands;

    operands = ScriptPointerAdvance(2);
    notice_kind = (*(u8 *)((u8 *)operands + 0x1));
    item = (*(s32 *)((u8 *)operands + 0x0));
    if (notice_kind != 1) {
        MessageNoticeItemDisp(item, notice_kind + 1);
    }
    if ((ChrItemGive(0, item & 0xFF) == 0) && (g_ChrItemNoRoomState != 1)) {
        g_ChrItemNoRoomState = 2;
    }
    g_ScriptCommandPointer += 2;
}

s32 ChrItemRemove(s32, s32);                            /* extern */
s32 MessageNoticeItemDisp(s32, s32);                          /* extern */

/* func_8005D164 */
void ScriptCommandChrItemRemove(void) {
    s32 masked_notice_kind;
    s32 item;
    u8 notice_kind;
    void *operands;

    operands = (void *)ScriptPointerAdvance(2);
    notice_kind = (*(u8 *)((u8 *)operands + 0x1));
    item = (*(volatile u8 *)((u8 *)operands + 0x0)) & 0xFF;
    ChrItemRemove(0, item);
    masked_notice_kind = notice_kind & 0xFF;
    if (masked_notice_kind != 1) {
        MessageNoticeItemDisp(item, masked_notice_kind);
    }
    g_ScriptCommandPointer += 2;
}

/* func_8005D1CC */
void ScriptCommandChrItemHeldJump(void)
{
  s32 command;
  s32 command_then_held;
  u8 *operands;
  u8 item;
  u8 worn_only;

  command_then_held = g_ScriptCommandPointer;
  command = command_then_held;
  command_then_held = command_then_held + 2;
  g_ScriptCommandPointer = command_then_held;
  operands = ScriptPointerAdvance(2);
  item = operands[0];
  worn_only = operands[1];
  g_ScriptCommandPointer = g_ScriptCommandPointer + 2;
  command_then_held = ChrItemHeldCheck(0, item, worn_only);
  if (command_then_held != 0)
  {
    g_ScriptCommandPointer = command;
    ScriptCommandLabelJump();
  }
}

/* far_syms: g_PadActuatorLevels (target reaches it %hi/%lo) */
/* far_syms: g_PadActuator1Level (target reaches it %hi/%lo) */
/* far_syms: g_PadActuatorLevels (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadActuator1Level (auto: target reaches it %hi/%lo — addressing row 3) */
extern s8 g_PadActuatorLevels;
extern s8 g_PadActuator1Level;

/* func_8005D23C */
void ScriptCommandActuatorLevelsSet(void) {
    void *levels;

    levels = ScriptPointerAdvance(2);
    if (((GameOptions *)g_GameOptions)->actuators != 0) {
        g_PadActuatorLevels = (*(u8 *)levels);
        g_PadActuator1Level = *(u8 *)(levels + 1);
    }
    g_ScriptCommandPointer += 2;
}

/* func_8005D294 */
void ScriptCommandActuatorListStart(void)
{
  s32 *list_entry;
  u8 list_number;
  list_number = (*(s32 *)ScriptPointerAdvance(2));
  list_entry = (list_number * 4) + g_ScriptActuatorLists;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  g_PadActuatorList = *list_entry;
  g_ScriptCommandPointer += 2;
}
extern s32 g_ChrItemList0C;
s32 resource_load(s32, s32);
void mem_move();
void ChrLevelAdd(s32, s32);
s32 ChrItemUseGet();
s32 ChrWornItemRemove(s32 chr_id, s32 worn_place);
void ChrWornItemSet();

/* ScriptCommandChrLevelItemSet -- event-script command 0x22
   (g_ScriptCommandFunctions[0x22], slot 0x800B33C4), SET A MEMBER'S LEVEL OR GEAR: a
   6-byte command, the id word then who (a member id; 0xC is the shared item
   list), kind, val and a pad byte. The three bytes are read at
   ScriptPointerAdvance(2) and the script pointer g_ScriptCommandPointer then moves on
   by 4.

     kind 0   set level val. Below the level of who's persistent record
              (the 0x40-byte ChrRecord at g_ChrRecords + who * 0x40, +0x1C) the
              record is first reloaded raw from resource 0xE7
              (resource_load(0xE7, 0x801CFB00), 0x40 bytes at who * 0x40); then,
              if the level still differs, ChrLevelAdd(who, val - level)
              levels it and HP / MP are set to their maxima. A val equal to
              the level does nothing at all, not even the refill.
     kind 7   val != 0: ChrItemGive(who, val), the inventory add.
              val == 0: who's six carried cells (+0x24) are cleared -- for
              who == 0xC the 0x60 cells of the shared list *g_ChrItemList0C.
     other    kind is a worn slot (1..6), val the item: ChrItemUseGet(who,
              val, 1) when val != 0 (its result is not looked at), then
              ChrWornItemRemove(who, kind) takes off what is worn there (the
              item it returns is dropped, not put back in the carried
              cells) and ChrWornItemSet(who, kind, val) puts val on.

   No RNG draw and no frame wait of its own.

   who, kind and val are u8 locals read with `*p++` twice and `*p`: kind's
   and who's zero-extensions sit in the entry block and val's is redone in
   each arm (an int `kind` is 87/96, an int `val` 72, `kind = *p; val =
   p[1];` 87). The level is read from the record each time it is wanted and
   HP / MP are member stores (one read kept in a local is 86/96; the two
   stores through `*(u16 *)` casts 88: the record address is then re-formed
   between them). ChrWornItemSet is called without a prototype: chr_item.c's
   definition takes `s8 arg2`, and under that prototype the item would be
   sign-extended (68/96); the target passes it zero-extended.
   MARKED SPELLING: `if (who == 0xC) { }`, a test with nothing in it, ahead
   of the kind test. The target forms that compare's two operands in the
   entry block -- who's zero-extension right after its load and the
   constant 12 in $a0 (the kind test's delay slot) -- and only branches on
   them three tests later, in the kind 7 / val 0 arm, with nothing of the
   compare left beside the `beqz` in front of that branch. Without the
   early test who is extended in each arm and the 12 is loaded where it is
   used (60/96; an int `who` with the test is 90). The Japanese build
   (SLPS_013.97 0x80060674) has the same two rows in the same places, and
   this text under the cc272 marker aligns with it 94/98 against 65/98
   without the test. `!=` for `==`, and the test placed after who's or
   kind's read instead, give the same bytes, and so does any body that
   emits no code (`;`, `(void)0;`, a `switch (who)` with one empty case);
   a body with a store in it is 82/96 (the branch then outlives cse and
   cuts the entry block), and `who > 0xC` (no register constant) 87/96.
   What the original guarded there compiled to nothing in both builds and
   is not known. */
/* func_8005D2F0 */
void ScriptCommandChrLevelItemSet(void)
{
    u8 *p;
    u8 who;
    u8 kind;
    u8 val;

    p = (u8 *)ScriptPointerAdvance(2);
    who = *p++;
    kind = *p++;
    val = *p;
    if (who == 0xC) {
    }
    if (kind == 0) {
        if (val < ((ChrRecord *)((who << 6) + g_ChrRecords))->level) {
            resource_load(0xE7, 0x801CFB00);
            mem_move(g_ChrRecords + (who << 6), (who << 6) + 0x801CFB00, 0x40);
        }
        if (((ChrRecord *)((who << 6) + g_ChrRecords))->level != val) {
            ChrLevelAdd(who, val - ((ChrRecord *)((who << 6) + g_ChrRecords))->level);
            ((ChrRecord *)((who << 6) + g_ChrRecords))->hp = ((ChrRecord *)((who << 6) + g_ChrRecords))->hp_max;
            ((ChrRecord *)((who << 6) + g_ChrRecords))->mp = ((ChrRecord *)((who << 6) + g_ChrRecords))->mp_max;
        }
    } else if (kind == 7) {
        if (val != 0) {
            ChrItemGive(who, val);
        } else if (who == 0xC) {
            mem_clear(g_ChrItemList0C, 0x60);
        } else {
            mem_clear((s32)((ChrRecord *)(g_ChrRecords + (who << 6)))->items, 6);
        }
    } else {
        if (val != 0) {
            ChrItemUseGet(who, val, 1);
        }
        ChrWornItemRemove(who, kind);
        ChrWornItemSet(who, kind, val);
    }
    g_ScriptCommandPointer += 4;
}

/* far_syms: g_FaceList */
extern s32 g_ChrPlacePresetRecords;

/* Event-script command 0x43 (g_ScriptCommandFunctions[0x43], the index the runner
 * dispatches on; it is 0x27 only when counted from the slice at
 * 0x800B33AC; the splitter's move_cmd_table[0xF]): switch the lead
 * between character ids 1 and 0xB.
 * One operand, the byte ip[2]; the command is 4 bytes. Non-zero makes id
 * 0xB the lead from id 1, zero id 1 from id 0xB; nothing happens when the
 * lead (the first u16 of the party list g_PartyIds) already is the new id.
 * Otherwise the party list's first entry and the first byte of actor row 0
 * (g_MapPartyRecords) take the new id, the old id's whole 0x40 persistent record
 * (g_ChrRecords) is copied over the new id's, and so is its 8-byte record
 * (g_ChrPlacePresetRecords: formation cell and preset commands) - nothing of the
 * destination is kept. Then: the party's field sheets are reloaded when
 * MapPartySpriteDataMissingCount finds one missing; the portrait slot that shows the new id
 * (g_FaceList[0..7]) is swapped with slot 0; actor row 0 is bound to the
 * new id's sheet (id + 0xF when g_MapPartyKind == 0); event flag old + 0x2BC is
 * cleared and flag 0x2BC + that same sheet number is set.
 *
 * The two pointers are read before either store (two locals); the 0x40
 * record is a struct of words (word loop), the 8-byte one a struct of
 * bytes (lwl / lwr). */
/* func_8005D470 */
void ScriptCommandChr1ChrBChange(void)
{
    u8 *ip;
    u8 flag;
    s32 new;
    s32 old;
    s32 i;
    u16 *lead;
    u8 *actor;

    ip = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer = (s32)(ip + 2);
    flag = ip[2];
    g_ScriptCommandPointer = (s32)(ip + 4);
    if (flag != 0) {
        if (*(u16 *)g_PartyIds == 0xB) {
            return;
        }
        old = 1;
        new = 0xB;
    } else {
        if (*(u16 *)g_PartyIds == 1) {
            return;
        }
        old = 0xB;
        new = 1;
    }
    lead = (u16 *)g_PartyIds;
    actor = (u8 *)g_MapPartyRecords;
    *lead = new;
    *actor = new;
    *(ChrRecord *)((new << 6) + g_ChrRecords) = *(ChrRecord *)((old << 6) + g_ChrRecords);
    *(ChrPlacePresetRecord *)(new * 8 + g_ChrPlacePresetRecords) = *(ChrPlacePresetRecord *)(old * 8 + g_ChrPlacePresetRecords);
    if (MapPartySpriteDataMissingCount() != 0) {
        MapPartySpriteDataLoad(0);
    }
    for (i = 0; i < 8; i++) {
        if (g_FaceList[i] == new) {
            break;
        }
    }
    if (i < 8) {
        FaceListSwap(0, i);
    }
    if (g_MapPartyKind == 0) {
        new += 0xF;
    }
    MapRecordSpriteDataSet(g_MapPartyRecords, new);
    StoryBitClear(old + 0x2BC);
    StoryBitSet(new + 0x2BC);
}
u8 MoveChrRowIdGet(u8 id);
void MoveChrPartyJoin(u32 chr_id, u32 row_number);

/* ScriptCommandPartyJoin -- event-script command 0x23 (g_ScriptCommandFunctions[0x23],
   the index the runner dispatches on, slot 0x800B33C8; entry 7 of the
   splitter's script_cmd_table slice at 0x800B33AC): A MEMBER JOINS THE
   FIELD PARTY. The twin of ScriptCommandPartyLeave right below (command
   0x24, a member leaves). Four bytes: the id word, then `from` (ip[2]) and
   `arg` (ip[3]). Nothing calls it with a jal.

   In order:
   1. MoveChrCommandEndWait(0x7E) waits, a frame per pass, until none of busy
      bits 1..6 of g_MoveChrBusyBits (the follower movers) is left.
   2. The script pointer is read once and moved on by 4.
   3. from != 0: arg is a script slot (a map object's) and the member id is
      the one that object shows, MoveChrRowIdGet(arg) -- 0xFF when the slot
      is not live; no check here.
      from == 0: arg is the id itself and the slot is 0; 0x10..0x1E is
      folded to 1..0xF (id + 0xF is the number ScriptCommandChr1ChrBChange binds a
      lead's sheet by; PartyPlaceFind makes the same fold on its own copy).
   4. g_PartyCount (the party's head count) >= 6: script error 5 through
      g_ScriptErrorNumber / ScriptErrorHang, which never returns.
   5. PartyPlaceFind(id) >= 0, the id is already in the party list
      g_PartyIds: nothing more happens.
   6. Otherwise: an id below 0x1F has its persistent record refilled --
      ChrRecord at g_ChrRecords + id * 0x40, hp = hpmax and mp = mpmax -- and
      event flag 0x2BC + id is set (StoryBitSet); an id of 0x1F or more
      touches no record and sets flag 0x2BC + 0xD. Then
      MoveChrPartyJoin(id, slot) (lns_move.c) puts the id in the
      party: at its party spot when slot is 0, in place of the map object
      when it is not.
   There is no range check between 0xC and 0x1E: the table holds twelve
   records (ids 0..11), so such an id stores its two words past them --
   for 0xC, a party id with no record, in the first eight bytes of
   g_ChrPlacePresetRecords, which GameStart places right behind the table.

   No RNG draw of its own. Frame waits: step 1's; the insert helper's own
   path (it can reload the party's sheets) was not read for this.

   id is one u8 variable used as it is at every site -- the compiler makes
   the one masked copy ($s0) that the party test, the 0x1F test and the
   record address share, ahead of the head-count test. A second int local
   `who = id;` set before that test is 54/64 as s32 (slti, and the two
   saved registers change places) and 54/64 as u32 (the registers only);
   as u8, or as u32 set after the test, it gives the same bytes as none. */
/* func_8005D60C */
void ScriptCommandPartyJoin(void)
{
    u8 *ip;
    u8 id;
    u8 slot;
    u8 flag;
    ChrRecord *rec;

    MoveChrCommandEndWait(0x7E);
    ip = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer = (s32)(ip + 4);
    if (ip[2] != 0) {
        slot = ip[3];
        id = MoveChrRowIdGet(slot);
    } else {
        id = ip[3];
        slot = 0;
        if (id >= 0x10 && id < 0x1F) {
            id -= 0xF;
        }
    }
    if (g_PartyCount >= 6) {
        g_ScriptErrorNumber = 5;
        ScriptErrorHang();
    }
    if (PartyPlaceFind(id) < 0) {
        if (id >= 0x1F) {
            flag = 0xD;
        } else {
            rec = (ChrRecord *)((id << 6) + g_ChrRecords);
            rec->hp = rec->hp_max;
            rec->mp = rec->mp_max;
            flag = id;
        }
        StoryBitSet(flag + 0x2BC);
        MoveChrPartyJoin(id, slot);
    }
}

s32 PartyPlaceFind(u32);                             /* extern */
s32 MoveChrPartyLeave(s32, s32, s32);            /* extern */

/* func_8005D70C */
void ScriptCommandPartyLeave(void) {
    s32 command;
    s32 place;
    u32 masked_chr_id;
    u8 row_number;
    u8 definition_number;
    u8 chr_id;
    u8 bit_offset;

    MoveChrCommandEndWait(0x7E);
    command = g_ScriptCommandPointer;
    g_ScriptCommandPointer = command + 6;
    chr_id = (*(s16 *)((u8 *)command + 0x2));
    row_number = (*(u8 *)((u8 *)command + 0x3));
    definition_number = (*(s32 *)((u8 *)command + 0x4));
    if ((u32) (chr_id - 0x10) < 0xFU) {
        chr_id -= 0xF;
    }
    if ((u8) g_PartyCount < 2U) {
        g_ScriptErrorNumber = 6;
        ScriptErrorHang();
    }
    masked_chr_id = chr_id & 0xFF;
    place = PartyPlaceFind(masked_chr_id);
    if (place >= 0) {
        bit_offset = chr_id;
        if (masked_chr_id >= 0x1FU) {
            bit_offset = 0xD;
        }
        StoryBitClear((bit_offset & 0xFF) + 0x2BC);
        MoveChrPartyLeave(place, row_number & 0xFF, definition_number & 0xFF);
    }
}

extern s32 g_MapSpriteDataEntries;
extern s32 g_MapObjectDefinitions;
s16 MapObjectFind(u8 definition_number);
s16 MapObjectFreeFind(void);
void MapObjectInit(void *, void *, void *, u8);
void MapSpriteDataMarkSet(s32 sprite_data_number, u8 mark);

/* ScriptCommandMapObjectsAdd -- script command 0xD of move_cmd_table
   (0x800B340C + 0xD * 4): SPAWN MAP OBJECTS from their definitions. Its
   operand is a 0-terminated list of definition ids, one byte each, right
   after the 2-byte command word: the script pointer g_ScriptCommandPointer is moved
   onto the list, ScriptListCount(1, 1) counts it and leaves the pointer
   past its terminator (rounded up to an even address), and the loop walks
   the list through a local.

   Per id (info = its 0x26-byte definition in g_MapObjectDefinitions):
   - info[1] (the sprite sheet) == 0: error 0xA;
   - the sheet's g_MapSpriteDataEntries record (0x10 bytes) has no data pointer:
     error 9 (the sheet is not loaded);
   - MapObjectFind(id) >= 0 -- a live g_MapObjects row already carries this
     id: error 0xB;
   - idx = MapObjectFreeFind(), the first free row of the 0x80; none: error 2.
   Errors go through g_ScriptErrorNumber / ScriptErrorHang, the script error
   reporter: it prints the error (ScriptStateReport), asserts (assert_fail,
   line 0x66A) and then spins forever -- a debug hang. It is not declared
   noreturn, so the compiler lays the rest of the pass out after each call
   (the reload of info[1], the delay-slot constants), but none of it is
   reached at run time once an error fires.
   - The object's event flag is cleared (StoryBitClear) when the
     definition's byte +2 is non-zero: flag 0x580 + that byte for kinds
     1..3 ((info[0] & 0x3F) + 1 < 4), flag id + 0x680 + ((map - 0x65) << 7)
     for any other kind (map = g_MapNumber) -- the two forms
     MoveChrPartyLeave uses, which clears without looking at +2.
   - MapObjectInit(row idx of g_MapObjects, its twin in g_MapObjectTwins,
     info, id) spawns it, and MapSpriteDataMarkSet(sheet, 0) zeroes the sheet's
     byte(s) in the *g_MapSpriteDataMarks table.
   No frame wait and no RNG draw of its own.

   Shape: idx is ONE s16 variable for both call results (the test of
   MapObjectFind's result folds into the call's $v0; what it changes is
   the pseudo's reference count, which is what puts it in $s0 ahead of ip
   and info -- with the first result tested directly the three registers
   rotate, 108/125). Both callees are declared as returning s16, as
   lns_move.c declares them (an s32 return, or none, scores 103). kind
   and flag are u8 locals read BEFORE the test, in that order: the u8
   compare is    the target's sltiu, and flag's zero-extension is dropped in the arm
   that also adds 0x580 and kept (andi 0xFF) in the other. The id is
   re-read through ip at every use; `*ip++` is the spawn's last argument. */
/* func_8005D7E0 */
void ScriptCommandMapObjectsAdd(void)
{
    u8 *ip;
    u8 *info;
    s32 i;
    s32 n;
    s16 idx;
    u8 kind;
    u8 flag;

    g_ScriptCommandPointer += 2;
    ip = (u8 *)g_ScriptCommandPointer;
    n = ScriptListCount(1, 1);
    for (i = 0; i < n; i++) {
        info = (u8 *)g_MapObjectDefinitions + *ip * 0x26;
        if (info[1] == 0) {
            g_ScriptErrorNumber = 0xA;
            ScriptErrorHang();
        }
        if (((s32 *)g_MapSpriteDataEntries)[info[1] * 4] == 0) {
            g_ScriptErrorNumber = 9;
            ScriptErrorHang();
        }
        idx = MapObjectFind(*ip);
        if (idx >= 0) {
            g_ScriptErrorNumber = 0xB;
            ScriptErrorHang();
        }
        idx = MapObjectFreeFind();
        if (idx < 0) {
            g_ScriptErrorNumber = 2;
            ScriptErrorHang();
        }
        kind = (info[0] & 0x3F) + 1;
        flag = ((u8 *)g_MapObjectDefinitions + *ip * 0x26)[2];
        if (kind < 4) {
            if (flag != 0) {
                StoryBitClear(flag + 0x580);
            }
        } else {
            if (flag != 0) {
                StoryBitClear((u16)(*ip + 0x680 + ((g_MapNumber - 0x65) << 7)));
            }
        }
        MapObjectInit((u8 *)g_MapObjects + idx * 0x60, (u8 *)g_MapObjectTwins + idx * 0x3C, info, *ip++);
        MapSpriteDataMarkSet(info[1], 0);
    }
}
/* ScriptCommandMapObjectsRemove -- event-script command 0x25
   (g_ScriptCommandFunctions[0x25], the index the runner dispatches on; the splitter's
   script_cmd_table[9] at 0x800B33AC; nothing calls it with a jal): REMOVE
   MAP OBJECTS, the inverse of command 0x41 (ScriptCommandMapObjectsAdd
   above, the splitter's move_cmd_table[0xD]). The operand is the same
   0-terminated list of definition ids, one byte each, right after the
   2-byte command word: the script pointer g_ScriptCommandPointer is moved onto the
   list, ScriptListCount(1, 1) counts it and leaves the pointer past its
   terminator, and the loop walks the list through a local.

   Per id:
   - idx = MapObjectFind(id), the live g_MapObjects row (0x60 bytes) whose
     byte +1 is this id; none: error 0xA through g_ScriptErrorNumber /
     ScriptErrorHang, the script error reporter (see ScriptCommandMapObjectsAdd).
   - The object's event flag is SET (StoryBitSet) when its definition's
     byte +2 is non-zero -- the 0x26-byte definition of g_MapObjectDefinitions indexed
     by the ROW's byte +1: flag 0x580 + that byte when the row's kind
     (byte +0 without bit 0x80, the bound-to-a-script-slot bit) is below
     4, flag id + 0x680 + ((map - 0x65) << 7) for any other kind (map =
     g_MapNumber). These are the two flags the spawn clears.
   - The row's first byte and the first byte of its twin in g_MapObjectTwins
     (0x3C bytes) are zeroed: both are free again. Nothing else of the row
     is touched and no callee is told (a script slot bound to the row is
     not released here).
   No frame wait and no RNG draw of its own.

   Shape: idx is an s16 variable; the target sign-extends the saved result
   once for the test and the row, and again after the flag call for the
   two clears, so the clears re-derive their addresses from idx (`row[0] =
   0` keeps the pointer in a saved register, 67/90; an s32 idx is 71). The
   row pointer is written product-first (base-first: 75). kind and flag
   are u8 (s32: 74 and 82), and the kind test is the outer one (flag
   outside: 65). The definition is indexed through the row's +1, not the
   list byte (85), though MapObjectFind has just matched the two. */
/* func_8005D9D4 */
void ScriptCommandMapObjectsRemove(void)
{
    u8 *ip;
    u8 *row;
    s32 i;
    s32 n;
    s16 idx;
    u8 kind;
    u8 flag;

    g_ScriptCommandPointer += 2;
    ip = (u8 *)g_ScriptCommandPointer;
    n = ScriptListCount(1, 1);
    for (i = 0; i < n; i++) {
        idx = MapObjectFind(*ip);
        if (idx < 0) {
            g_ScriptErrorNumber = 0xA;
            ScriptErrorHang();
        }
        row = (u8 *)(idx * 0x60 + g_MapObjects);
        kind = row[0] & 0x7F;
        flag = ((u8 *)(row[1] * 0x26 + g_MapObjectDefinitions))[2];
        if (kind < 4) {
            if (flag != 0) {
                StoryBitSet(flag + 0x580);
            }
        } else {
            if (flag != 0) {
                StoryBitSet((u16)(*ip + 0x680 + ((g_MapNumber - 0x65) << 7)));
            }
        }
        *(u8 *)(idx * 0x60 + g_MapObjects) = 0;
        *(u8 *)(idx * 0x3C + g_MapObjectTwins) = 0;
        ip++;
    }
}
/* far_syms: g_MoveChrSecondViewFollow g_MoveChrViewFollow */
void MoveChrSecondViewFollowSet(void);

/* ScriptCommandMapChange -- event-script opcode 0x26 (slot 0x800B33D4 of
   the handler table, 0x800B33AC[10]): CHANGE SCENE, then go on at a label
   (looked up in the offset table at g_ScriptBase after the change). A command
   id is a u16 dispatched through g_ScriptCommandFunctions[id]; nothing calls this with a
   jal.

   The command, c = its first byte:
     c+2  u16  label -- 0 ends the script
     c+4  u8   idx   -- entry of the scene table *g_MapEnterTable; 0 = the
                        entry follows inline
     c+5  u8   flag  -- 0 = MapChange also runs its closing fade
                        (MapFadeIn), non-zero = it does not
     idx != 0 (8 bytes):   c+6 = the byte handed on as MapChange's
                           last argument
     idx == 0 (0x12 bytes): c+6..c+0x10 = the 11 entry bytes in the order
                           b0, b2, b4, b5, b6, b7, b8, b9, b10, b3, b1
                           (map, transition, the clip rectangle, the start
                           position, then b3 and the object set b1), and
                           c+0x11 = that last argument

   Steps:
   - ScriptMusicWait(): while g_ScriptMusicLevelRequest is set it runs field frames until
     ScriptMusicStep() answers 0, and one more;
   - the script pointer g_ScriptCommandPointer is left at c+4 (it is replaced below);
   - MapChange(idx, &e, flag == 0, g_MoveChrViewFollow, g_MoveChrSecondViewFollow, last): the
     scene change itself (npc-scripting.md); g_MoveChrViewFollow / g_MoveChrSecondViewFollow are
     the camera's and the second scroll's "released" flags;
   - g_ScriptKeptBlinkCount = 0;
   - g_MoveChrSecondViewFollow == 0 (read again after the call): MoveChrSecondViewFollowSet(), which
     releases the second scroll's mover slot unless the map's scroll mode
     (g_MapEffect & 3) is 1 or 2;
   - MapSpriteDataMarksClear();
   - label == 0: g_ScriptEnded = 1, the script is over. Otherwise the label's
     entry of the u16 offset table at the script base g_ScriptBase is taken;
     a zero entry is script error 0 (g_ScriptErrorNumber = 0, ScriptErrorHang -- the
     reporter that never comes back at run time); g_ScriptLabel = label and
     the script pointer = base + entry * 2. This is ScriptCommandScriptChange's tail
     (opcode 0x42) word for word.
   Frames: ScriptMusicWait's and MapChange's. No RNG draw of its own.

   Shape:
   - `last` (the pointer to the final operand) is set to c+6 BEFORE the
     idx test and overwritten in the inline arm; set in an else arm
     instead it is 91/96.
   - The final operand is read into an int local before the call. Passed
     as `*last` in the argument list it is loaded after g_MoveChrSecondViewFollow has
     been stored to its stack slot (93/96); the target loads both first,
     into two registers. A u8 local is 89/96.
   - The flag test is a local too (`run`); written in the argument list
     it is 74/96.
   - The label is read through the script pointer after the first bump
     (`*(u16 *)g_ScriptCommandPointer`, or `*(u16 *)(ip + 2)`); `((u16 *)ip)[1]` is
     92/96. Operand bytes are indexed from the command's first byte: with
     the pointer taken after the two bumps (ip[0], ip[1], ...) it is
     68/96.
   - idx is passed whole: MapChange is a K&R definition above, so the
     u8 is promoted, not masked. */
/* func_8005DB3C */
void ScriptCommandMapChange(void)
{
    MapEnterRow e;
    u8 *ip;
    u8 *last;
    u16 label;
    u16 off;
    s32 idx;
    s32 run;
    s32 music_number;

    ScriptMusicWait();
    ip = (u8 *)g_ScriptCommandPointer;
    last = ip + 6;
    g_ScriptCommandPointer += 2;
    label = *(u16 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer += 2;
    idx = ip[4];
    run = ip[5] == 0;
    if (idx == 0) {
        e.map_number = ip[6];
        e.fade = ip[7];
        e.limit_left = ip[8];
        e.limit_right = ip[9];
        e.limit_top = ip[10];
        e.limit_bottom = ip[11];
        e.x = ip[12];
        e.y = ip[13];
        e.facing = ip[14];
        e.sound = ip[15];
        e.object_set = ip[16];
        last = ip + 0x11;
    }
    music_number = *last;
    MapChange(idx, &e, run, g_MoveChrViewFollow, g_MoveChrSecondViewFollow, music_number);
    g_ScriptKeptBlinkCount = 0;
    if (g_MoveChrSecondViewFollow == 0) {
        MoveChrSecondViewFollowSet();
    }
    MapSpriteDataMarksClear();
    if (label == 0) {
        g_ScriptEnded = 1;
        return;
    }
    off = *(u16 *)(label * 2 + g_ScriptBase);
    if (off == 0) {
        g_ScriptErrorNumber = 0;
        ScriptErrorHang();
    }
    g_ScriptLabel = label;
    g_ScriptCommandPointer = g_ScriptBase + off * 2;
}

/* func_8005DCBC */
void ScriptCommandMapPartyPlace(void) {
    s32 command;
    u8 row_number;
    u8 no_open;
    s32 run;
    s32 next_command;
    MapPartyPlaceRow buf;

    command = g_ScriptCommandPointer;
    next_command = command + 4;
    g_ScriptCommandPointer = command + 2;
    no_open = *(u8 *)(command + 3);
    row_number = *(u8 *)(command + 2);
    run = no_open < 1;
    if (row_number == 0) {
        buf.unk_00 = *(u8 *)(command + 4);
        buf.limit_left = *(u8 *)(command + 5);
        buf.limit_right = *(u8 *)(command + 6);
        buf.limit_top = *(u8 *)(command + 7);
        buf.limit_bottom = *(u8 *)(command + 8);
        buf.x = *(u8 *)(command + 9);
        buf.y = *(u8 *)(command + 0xA);
        buf.facing = *(u8 *)(command + 0xB);
        next_command = command + 0xE;
        buf.sound = *(u8 *)(command + 0xC);
    }
    g_ScriptCommandPointer = next_command;
    MapPartyPlace(row_number, &buf, run, g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
}

void MapBgChangeStart(s16, u8);                            /* extern */

/* func_8005DD84 */
void ScriptCommandMapBgChangeStart(void) {
    s32 command;

    MapBgChangeEndWait();
    command = g_ScriptCommandPointer;
    g_ScriptCommandPointer = command + 4;
    MapBgChangeStart(*(u8 *)((u8 *)command + 0x2), *(u8 *)((u8 *)command + 0x3));
}

s32 layer_col_set(s32, s32, s32 *);
s32 MapColFadeStart(s32, s32);
void MapColSet(s32, s32);

/* func_8005DDC4 */
volatile char ScriptCommandMapColSet(void)
{
  u8 row;
  u8 scale;
  s32 command;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 6;
  row = *((s16 *) (((u8 *) command) + 0x2));
  scale = *((u8 *) (((u8 *) command) + 0x3));
  if ((*((u8 *) (((u8 *) command) + 0x4))) != 0)
  {
    MapColSet(row, scale);

    if ((g_MapFadeState[0] == 0) && ((*(&g_MapDrawAreaClosed[0])) == 0))
    {
      layer_col_set(0, g_MapLayersMask | 0x100, (s32 *)&g_MapCol);
    }
  }
  else
  {
    MapColFadeStart(row & 0xFF, scale & 0xFF);
  }
}

s32 ColMix(u8, s32);                           /* extern */

/* func_8005DE54 */
void ScriptCommandColMix(void) {
    s32 command;

    command = g_ScriptCommandPointer;
    g_ScriptCommandPointer = command + 4;
    ColMix((*(s16 *)((u8 *)command + 0x2)), (*(u8 *)((u8 *)command + 0x3)) == 0);
}
/* far_syms: g_MapFadeState_s g_MoveChrSecondViewFollow g_MoveChrViewFollow */

/* ScriptCommandMapFadeOut -- event-script command 0x29 (slot 0x800B33E0 of
   the handler table g_ScriptCommandFunctions, = 0x800B33AC[13]), 4 bytes: FADE THE SCREEN
   OUT over a length the script gives. Nothing calls it with a jal. Its
   partner is command 0x2A (ScriptCommandMapFadeIn below), which fades back
   in with MapFadeIn.

   The command, c = its first byte (the script pointer g_ScriptCommandPointer on
   entry; it is moved on to c+4 first):
     c+2  u8  len  -- the ramp's length in frames (MapFadeOut's second
                      argument; the scene change MapChange passes a
                      fixed 0x19 there)
     c+3  u8  kind -- read ONLY while the screen is not already faded out
                      (g_MapFadeState == 0):
                        1 = the colour argument is -2 instead of -1;
                        2 = g_MapFadeState = 2, which makes MapFadeOut (and
                            the fade in that follows) take the party-glow
                            variant (MapFadePartyOut / MapFadePartyIn);
                        anything else = the plain fade, colour -1.

   Then MapFadeOut(g_MapLayersMask | 0x130, len, colour, g_MoveChrViewFollow,
   g_MoveChrSecondViewFollow) -- the transition out (map_fade.c): it waits out a ramp that
   is still running, starts layer_col_fade_start(0, mask | 0x80000, 3, len,
   colour), waits for that one and runs two more field frames.
   g_MoveChrViewFollow / g_MoveChrSecondViewFollow (the camera's and the second scroll's
   "released" flags) are the two arguments of every field frame it runs
   (MapFrameStep). The colour is layer_col_fade_start's fifth argument, a
   sentinel there: -1 = the s16 triple g_LayerColLowest (-0x100 three times),
   -2 = g_LayerColHighest (0x100 three times).

   g_MapFadeState is the fade's state byte: MapFadeOut sets it to 1 unless
   it is 2, MapFadeIn clears it. So a second fade-out command before a
   fade in ignores its kind byte and asks for -1 again. The kind test is
   the one MapChange makes on a scene entry's byte +2 (-1 / -2 / the
   glow), word for word.

   Reads: g_ScriptCommandPointer, the two operand bytes, g_MapFadeState, g_MapLayersMask,
   g_MoveChrSecondViewFollow, g_MoveChrViewFollow. Writes: g_ScriptCommandPointer (+4), g_MapFadeState (2, kind 2
   only). No RNG draw of its own. Frames: MapFadeOut's (the two waits
   and the two frames after them).

   Shape:
   - the colour is set to -1 BEFORE the flag test (the target has it in
     the test's delay slot, so it is set on both paths); set inside the
     `if` it is 26/32 and unset on the already-faded path;
   - len is a u8 local read before the test: the target masks it again at
     the call (`andi $a1, 0xFF`). An s32 local is 31/32;
   - the flag byte is the TU's far scalar g_MapFadeState_s (the alias
     MapChange uses): the target has a `lui` of its own for the load
     and `lui $at` for the store. Through the TU's `u8 g_MapFadeState[]` the
     two share one address register, 26/32;
   - the two kind tests are two separate `if`s, both made (`else if` for
     the second: 30/32);
   - the store is the constant 2; it leaves in kind's register (`sb $v1`)
     because kind is known to be 2 there. `= kind` with the u8 local is
     29/32. An s32 kind, `g_ScriptCommandPointer += 4` and an int return type each
     give the same 32 rows. */
/* func_8005DE88 */
void ScriptCommandMapFadeOut(void)
{
    u8 *ip;
    u8 len;
    u8 kind;
    s32 colour;

    ip = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer = (s32)(ip + 4);
    len = ip[2];
    colour = -1;
    if (g_MapFadeState_s == 0) {
        kind = ip[3];
        if (kind == 1) {
            colour = -2;
        }
        if (kind == 2) {
            g_MapFadeState_s = 2;
        }
    }
    MapFadeOut(g_MapLayersMask | 0x130, len, colour, g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
}

/* func_8005DF08 */
void ScriptCommandMapFadeIn(void) {
    u8 c;
    g_ScriptCommandPointer += 2;
    c = *(u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer += 2;
    MapFadeIn(g_MapLayersMask | 0x130, c, g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
}

/* func_8005DF58 */
s32 ScriptCommandMapDrawAreaClose(void) {
    s32 base;

    base = g_ScriptCommandPointer;
    g_ScriptCommandPointer = base + 4;

    return MapDrawAreaClose( g_MapLayersMask | 0x130, *(u8 *)(base + 2), *(u8 *)(base + 3), g_MoveChrViewFollow, g_MoveChrSecondViewFollow );
}

/* far_syms: g_MoveChrSecondViewFollow (target reaches it %hi/%lo) */
/* far_syms: g_MoveChrSecondViewFollow (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MoveChrViewFollow (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_8005DFA4 */
s32 ScriptCommandMapDrawAreaOpen(void) {
    s32 base;

    base = g_ScriptCommandPointer;
    g_ScriptCommandPointer = base + 4;

    return MapDrawAreaOpen( g_MapLayersMask | 0x130, *(u8 *)(base + 2), *(u8 *)(base + 3), g_MoveChrViewFollow, g_MoveChrSecondViewFollow );
}
/* far_syms: g_ScriptMapBtlRows g_MapSpriteDataMarks g_MapTouchedObject g_BtlSlotIds g_MoveChrSecondViewFollow g_MoveChrViewFollow */
/* load_hazard_nop: ScriptCommandMapBtlRun */
extern s16 g_MapTouchedObject;
extern u8 g_BtlSlotIds[];
extern u8 g_ScriptMapBtlRows[];

/* ScriptCommandMapBtlRun -- the script's BATTLE command (8 bytes): run an encounter
   and branch on how it ended.

   Operands, at the script pointer g_ScriptCommandPointer (advanced by 8):
     +2 u16  label: where the script goes when the fight returns 1 (0 = no
             label -- see below)
     +4 u8   row + 1 of the 8-byte encounter rows at g_ScriptMapBtlRows:
             [0] the encounter (MapBtlRun's arg0), [1] its arg1,
             [2..7] a party list, 0-terminated (empty = the current party)
     +5 u8   MapBtlRun's arg3 (2 = no map restore after the fight,
             0 = fade back in)
     +6 u8   MapBtlRun's arg6 (the music request)
     +7 u8   BGM id, 0xFF = keep: g_MusicNumber = id, g_MusicKind =
             MusicKindGet(id) & 0xFF (the store is 16 bits, the re-read a
             byte -- `*(u8 *)&g_MusicNumber`, as ScriptCommandMoviePlay)

   The battle's party list g_BtlSlotIds (6 bytes) is cleared and filled:
   - row[2] == 0: from the field party ids g_PartyIds (u16[6]), stopping at
     the first 0: ids below 0xC in order, id 0xC into slot 5, any other
     id (0xD and up) not copied;
   - otherwise from the row's own bytes (at most six, to the first 0):
     0xC into slot 5, anything else in order.

   ret = MapBtlRun(row[0], row[1], label, +5, g_MoveChrViewFollow, g_MoveChrSecondViewFollow, +6):
   - 1 with label == 0: the 0x100 bytes at *g_MapSpriteDataMarks are cleared,
     g_ScriptKeptBlinkCount = 0, g_ScriptEnded = 1;
   - 1 with a label: the script pointer is put back on this command and
     ScriptCommandLabelJump (the label jump) reads +2 as its operand;
   - 2: g_ScriptKeptBlinkCount = 0x12C, MapFadeIn(g_MapLayersMask | 0x130, 0x19,
     g_MoveChrViewFollow, g_MoveChrSecondViewFollow), g_ScriptEnded = 1;
   - anything else: MapObjectTouchRemove(g_MoveChrViewFollow, g_MoveChrSecondViewFollow) when the touched
     object g_MapTouchedObject is >= 0.

   Shape: the row index and the encounter are ONE variable (enc, $s3), the
   loop counter and the result are one (i, $a1); the table address goes
   through a pointer local (tbl) so that it is formed BEFORE the index
   shift -- reorg then steals its `lui` into the 0xFF test's delay slot
   (with `&g_ScriptMapBtlRows[enc * 8]` the shift comes first and is stolen
   instead: 159/161). g_BtlSlotIds is written directly; the `$a0 = $s2` copy
   ahead of each loop is loop.c hoisting the address cse had already tied
   to the clear call's argument. `label` is a u16 local: its one `andi`
   feeds both the call and the `== 0` test. */
/* func_8005DFF0 */
void ScriptCommandMapBtlRun(void)
{
    s32 base;
    u16 label;
    s32 b5;
    s32 b6;
    s32 enc;
    s32 lvl;
    u8 *p;
    u8 *tbl;
    s32 i;
    s32 n;
    s32 id;

    ScriptMusicWait();
    base = g_ScriptCommandPointer;
    g_ScriptCommandPointer = base + 2;
    label = *(u16 *)(base + 2);
    g_ScriptCommandPointer = base + 8;
    enc = *(u8 *)(base + 4) - 1;
    b5 = *(u8 *)(base + 5);
    b6 = *(u8 *)(base + 6);
    if (*(u8 *)(base + 7) != 0xFF) {
        g_MusicNumber = *(u8 *)(base + 7);
        g_MusicKind = MusicKindGet(*(u8 *)&g_MusicNumber) & 0xFF;
    }
    tbl = g_ScriptMapBtlRows;
    p = tbl + enc * 8;
    enc = *p++;
    lvl = *p++;
    mem_clear(g_BtlSlotIds, 6);
    n = 0;
    if (*p == 0) {
        for (i = 0; i < 6; i++) {
            id = ((u16 *)g_PartyIds)[i];
            if (id == 0) {
                break;
            }
            if (id < 0x1F) {
                if (id < 0xC) {
                    g_BtlSlotIds[n] = id;
                    n++;
                } else if (id == 0xC) {
                    g_BtlSlotIds[5] = id;
                }
            }
        }
    } else {
        for (i = 0; i < 6; i++) {
            id = *p++;
            if (id == 0) {
                break;
            }
            if (id != 0xC) {
                g_BtlSlotIds[n] = id;
                n++;
            } else {
                g_BtlSlotIds[5] = id;
            }
        }
    }
    i = MapBtlRun(enc, lvl, label, b5, g_MoveChrViewFollow, g_MoveChrSecondViewFollow, b6);
    if (i == 1) {
        if (label == 0) {
            mem_clear(g_MapSpriteDataMarks, 0x100);
            g_ScriptKeptBlinkCount = 0;
            g_ScriptEnded = 1;
        } else {
            g_ScriptCommandPointer = base;
            ScriptCommandLabelJump();
        }
    } else if (i == 2) {
        g_ScriptKeptBlinkCount = 0x12C;
        MapFadeIn(g_MapLayersMask | 0x130, 0x19, g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
        g_ScriptEnded = 1;
    } else {
        if (g_MapTouchedObject >= 0) {
            MapObjectTouchRemove(g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
        }
    }
}

/* load_hazard_nop: ScriptCommandMapViewShakeStart */
extern s16 g_MapViewShakeCount;
extern s16 g_MapViewShakeWait;
extern s16 g_MapViewShakeX;
extern s16 g_MapViewShakeY;

static inline s16 inline_fn(u8 shake)
{
  return (s16) shake;
}
/* func_8005E274 */
void ScriptCommandMapViewShakeStart(void)
{
  void *command;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 6;
  g_MapViewShakeX = inline_fn(*((s16 *) (((u8 *) command) + 0x2)));
  g_MapViewShakeY = inline_fn(*((u8 *) (((u8 *) command) + 0x3)));
  g_MapViewShakeCount = (*((u8 *) (((u8 *) command) + 0x4))) * 2;
  g_MapViewShakeWait = 0;
}

extern s16 g_MovieSkipAllowed;

/* load_hazard_nop: ScriptCommandMoviePlay */
/* func_8005E2B4 */
void ScriptCommandMoviePlay(void) {
    s32 base;
    s32 a;
    s32 c;

    ScriptMusicWait();
    base = g_ScriptCommandPointer;
    g_ScriptCommandPointer = base + 6;
    a = *(u8 *)(base + 2);
    c = *(u8 *)(base + 3);

    if (*(u8 *)(base + 4) != 0xFF) {
        g_MusicNumber = *(u8 *)(base + 4);
        g_MusicKind = MusicKindGet(*(u8 *)&g_MusicNumber) & 0xFF;
    }

    g_MovieSkipAllowed = 0;
    MoviePlay(a);

    if (c != 2) {
        MapReload();
        MapBgChangeStoryBitsCopy();
        MapViewStep(g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
    }

    if (c == 0) {
        MapFadeIn(g_MapLayersMask | 0x130, 0x19, g_MoveChrViewFollow, g_MoveChrSecondViewFollow);
    }
}

s32 ShopRun(s32);
s32 MessageRecordsInit();

/* func_8005E384 */
void ScriptCommandShopRun(void)
{
  void *command;
  char shop_number;
  MapStatusClear();
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  shop_number = *(&(*((s16 *) (((u8 *) command) + 0x2))));
  g_ScriptCommandPointer = command + 4;
  ShopRun(shop_number & 0xFF);
  MessageRecordsInit();
}

/* no_split_addresses: ScriptCommandPartyHpMpFill */
/* far_syms: g_MapFadeState (target reaches it %hi/%lo) */
/* far_syms: g_MapDrawAreaClosed (target reaches it %hi/%lo) */

/* func_8005E3D0 */
void ScriptCommandPartyHpMpFill(void)
{
  u16 var[3];
  s32 i;
  s32 base;
  u16 *ptr;
  s32 value;
  ChrRecord *p;
  g_ScriptCommandPointer += 2;
  value = g_MapFadeState[0];
  if ((value == 0) && (g_MapDrawAreaClosed[0] == 0))
  {
    MapStatusClear();
    var[0] = 0;
    var[2] = 0x78;
    var[1] = 0x78;
    SoundPlay(0x18, 1);
    MapColFlashRun(0x20, var);
  }
  i = 0;
  base = g_ChrRecords;
  ptr = (u16 *) g_PartyIds;
  while (i < 6)
  {
    value = *ptr;
    if (value == 0)
    {
      break;
    }
    if (value < 0xC)
    {
      p = (ChrRecord *) ((value << 6) + base);
      p->hp = p->hp_max;
      p->mp = p->mp_max;
    }
    i++;
    ptr++;
  }

}

extern s32 g_MusicWaitFrameKind;

/* load_hazard_nop: ScriptCommandMusicPlay */
/* func_8005E494 */
void ScriptCommandMusicPlay(void)
{
  u8 *ptr;
  u8 time;
  void *command;

  ScriptMusicWait();
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 4;
  ptr = (u8 *)command;
  g_MusicNumber = ptr[2];
  time = ptr[3];
  if (g_MusicNumber == 0)
  {
    if (time != 0)
    {
      g_ScriptMusicLevelRequest = 1;
    }
  }
  else
  {
    g_MusicWaitFrameKind = 1;
    g_MusicKind = MusicKindGet((u8)g_MusicNumber) & 0xFF;
  }
  MusicPlay((u8)g_MusicNumber, (u8)g_MusicKind, time & 0xFF);
  g_MusicWaitFrameKind = 0;
  ScriptFrameStep(1);
}

s32 SoundPlay(u16, s32);

/* func_8005E524 */
void ScriptCommandSoundPlay(void)
{
  u16 sound;
  void *command;
  s32 has_sound;
  command = (void *)g_ScriptCommandPointer;
  g_ScriptCommandPointer = (s32)command + 2;
  sound = *((s16 *) (((u8 *) command) + 0x2));
  has_sound = sound != 0;
  g_ScriptCommandPointer = (s32)command + 4;
  if (has_sound)
  {
    SoundPlay(sound, (char)((g_ScriptSoundNextSlot + 4) & 0xFF));
    g_ScriptSoundNextSlot = (s32) (((s32) g_ScriptSoundNextSlot) ^ 1);
    return;
  }
  SoundPlay(0U, 4);
  SoundPlay(0U, 5);
  g_ScriptSoundNextSlot = 0;
}

/* func_8005E5A4 */
void ScriptCommandMusicLevelSet(void)
{
  u8 *operands;
  u8 time;
  u8 level;
  void *command;
  ScriptMusicWait();
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 4;
  level = *((s16 *) (operands = ((u8 *) command) + 0x2));
  time = *((u8 *) (((u8 *) command) + 0x3));
  g_ScriptMusicLevelRequest = 2;
  if (!(MusicLevelStep(level, time & 0xFF) & 0xFF))
  {
    g_ScriptMusicLevelRequest = 0;
  }
  ScriptFrameStep(1);
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_8005E608() {

}
/* far_syms: g_FaceList g_FaceListEntryRows */
extern u8 g_FaceListEntryRows[];

/* ScriptCommandFaceListSet -- a script command that loads the portraits a scene
   needs into the eight portrait slots *g_FaceList (bytes 0..5 the small
   ones, 6 the wide one, 7 its second half / a spare; face_list.c's
   FaceListEntriesSet rebuilds the 0x28-byte strip g_FaceListEntries from them).

   Operands: the byte list at ScriptPointerAdvance(2), ScriptListCount(1, 1) bytes
   long. Byte 0 is the wide portrait (0xFF = leave it): when it differs
   from slot 6, slot 6 takes it, slot 7 is cleared and the wide one is
   marked for loading; a non-zero byte +5 in its 10-byte g_FaceListEntryRows row
   also marks slot 7. Every further byte is a character id:
   - the slot cursor first skips filled slots -- `ids[slot] != 0 &&
     slot < 6`, in that order, so slot 6 is READ once the six are full;
   - an id PartyPlaceFind finds in the field party needs no slot;
   - otherwise it takes the free slot (marked, `any` set). With no slot
     free (slot == 6): if slot 7 was already marked the command stops
     here; if not, slot 6's id moves to slot 7, the new id takes slot 6,
     the wide one is marked for loading again, and the command stops.

   Then: with the wide one marked, func_8007608C(6) (an empty function),
   FaceSlot6Load(slot 6's id) -- the file load -- and func_8007608C(7)
   when slot 7 is marked and non-zero; with nothing marked at all the
   function returns here. Each marked slot 0..5 is loaded with
   FaceListByteLoad(i), FaceListEntriesSet() rebuilds the strip, and the message
   window's two portrait handles g_MessageFaceSecond / g_MessageFaceFirst are zeroed.

   Shape: the `slot >= 6` test is written TWICE, around the two stores --
   cse (-fcse-skip-blocks) carries the first compare's result over the
   skipped block, which is the target's `slti $a0` reused by `beqz $a0`
   (a local holding the compare computes it in $v0 and copies it). In the
   no-slot block `used[7] = 1` comes BEFORE `wide = 1`: the other way
   round cse stores wide's register, as the first block's `sb $s3` does,
   and loop.c has no second constant to hoist with `used[slot] = 1`'s
   (the target's $s4). The first byte is a u8 local: stored raw, masked
   for the two compares. */
/* func_8005E610 */
void ScriptCommandFaceListSet(void)
{
    u8 used[8];
    u8 *ops;
    s32 count;
    s32 wide;
    s32 any;
    s32 slot;
    s32 i;
    u8 c;

    ops = (u8 *)ScriptPointerAdvance(2);
    count = ScriptListCount(1, 1);
    mem_clear(used, 8);
    any = 0;
    c = *ops;
    wide = 0;
    if (c != 0xFF && g_FaceList[6] != c) {
        g_FaceList[6] = c;
        used[6] = 1;
        g_FaceList[7] = 0;
        wide = 1;
        if (g_FaceListEntryRows[(g_FaceList[6] - 1) * 0xA + 5] != 0) {
            g_FaceList[7] = 0;
            used[7] = 1;
        }
    }
    if (count > 1) {
        slot = 0;
        for (i = 0; i < count - 1; i++) {
            while (g_FaceList[slot] != 0 && slot < 6) {
                slot++;
            }
            ops++;
            if (PartyPlaceFind(*ops) < 0) {
                if (slot >= 6) {
                    if (used[7] != 0) {
                        break;
                    }
                    g_FaceList[7] = g_FaceList[6];
                    used[7] = 1;
                    wide = 1;
                }
                g_FaceList[slot] = *ops;
                used[slot] = 1;
                if (slot >= 6) {
                    break;
                }
                any = 1;
            }
        }
    }
    if (wide == 0) {
        if (any == 0) {
            return;
        }
    } else {
        func_8007608C(6);
        FaceSlot6Load(g_FaceList[6]);
        if (used[7] != 0 && g_FaceList[7] != 0) {
            func_8007608C(7);
        }
    }
    if (any != 0) {
        for (i = 0; i < 6; i++) {
            if (used[i] != 0) {
                FaceListByteLoad(i);
            }
        }
    }
    FaceListEntriesSet();
    g_MessageFaceSecond = 0;
    g_MessageFaceFirst = 0;
}

extern s32 g_MapPartyTwins;

/* func_8005E890 */
void ScriptCommandMapPartyKindSet()
{
  s32 i;
  s32 offset;
  s32 j;
  s32 id;
  u8 byte;
  u8 buf[8];
  g_ScriptCommandPointer += 2;
  byte = *((u8 *) g_ScriptCommandPointer);
  g_ScriptCommandPointer += 2;
  if (g_MapPartyKind == byte)
  {
    return;
  }
  if (byte == 3)
  {
    g_MapPartyWalkStepCount = 1;
    return;
  }
  i = 0;
  g_MapPartyKind = byte;
  offset = 0;
  for (; 6 > i; i++)
  {
    id = *((u16 *) (g_PartyIds + (i * 2)));
    if (id == 0)
    {
      break;
    }
    if (g_MapPartyKind == 0)
    {
      id += 15;
    }
    MapRecordSpriteDataSet(g_MapPartyRecords + offset, id);
    MapAnimationStart(g_MapPartyRecords + offset, 1);
    offset += 0x3C;
  }
  MapPartyBoxSet();
  if (g_MapPartyKind == 2)
  {
    j = 0x3C;
    for (i = 1; i < 6; i++)
    {
      *((u8 *) (j + g_MapPartyRecords)) = 0;
      *((u8 *) (j + g_MapPartyTwins)) = 0;
      j += 0x3C;
    }
  }
  else
  {
    for (i = 1; i < 6; i++)
    {
      MapPartyRingEntryGet((u16) i, buf);
      MapPartyRecordSpotSet(i, buf);
    }
  }
}

/* func_8005E9D4 */
void ScriptCommandPlaceNameSet(void) {
    u8 *command;

    command = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer = (s32)(command + 2);
    g_MapPlaceName = command[2];
    g_ScriptCommandPointer = (s32)(command + 4);
}

s32 VoiceSet(u8);                                /* extern */

/* load_hazard_nop: ScriptCommandVoiceSet */
/* func_8005E9FC */
void ScriptCommandVoiceSet(void) {
    ScriptCommandVoiceEndWait();
    g_Voice = (s16) *(u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer += 2;
    VoiceSet((u8) g_Voice);
}

s32 VoiceStart();                                  /* extern */
void ScriptErrorHang();                                  /* extern */

/* func_8005EA44 */
void ScriptCommandVoiceStart(void) {
    g_ScriptCommandPointer += 2;
    if (g_Voice == 0) {
        g_ScriptErrorNumber = 9;
        ScriptErrorHang();
    }
    VoiceStart();
    g_Voice = -1;
}

/* func_8005EA8C */
void ScriptCommandVoiceEndWait(void)
{
  s32 frames_done;
  g_ScriptCommandPointer += 2;
  frames_done = 0;
  while (g_Voice == (-1))
  {
    ScriptFrameStep(1);
    frames_done += 1;
    if (frames_done >= 0x12C0)
    {
      g_Voice = 0;
      return;
    }
  }
}

/* func_8005EB04 */
void ScriptCommandMessageTimedSet(void)
{
  int on;
  u8 old_speed;
  u8 old_press_wait;
  void *command;
  command = g_ScriptCommandPointer;
  g_ScriptCommandPointer = command + 2;
  on = (*((u8 *) (((u8 *) command) + 0x2))) != 0;
  g_ScriptCommandPointer = command + 4;
  if (on)
  {
    if (g_MessageTimed == 0)
    {
      old_speed = g_MessageSpeed;
      old_press_wait = g_MessagePressWait;
      g_MessageSpeed = 0;
      g_MessagePressWait = 0;
      g_MessageTimed = 1;
      g_MessageKeptSpeed = old_speed;
      g_MessageKeptPressWait = old_press_wait;
      ((MapRecord *)g_MessageArrowRecord)->sprite.on = 0;
    }
  }
  else
    if (g_MessageTimed != 0)
  {
    g_MessageSpeed = g_MessageKeptSpeed;
    g_MessagePressWait = g_MessageKeptPressWait;
    g_MessageTimed = 0;
    ((MapRecord *)g_MessageArrowRecord)->sprite.on = 1;
  }
}
/* far_syms: g_MapPadDirectionsBoxLeft g_MapPadDirectionsBoxRight g_MapPadDirectionsBoxTop g_MapPadDirectionsBoxBottom g_MapPadDirectionsState */
extern s16 g_MapPadDirectionsBoxLeft;
extern s16 g_MapPadDirectionsBoxRight;
extern s16 g_MapPadDirectionsBoxTop;
extern s16 g_MapPadDirectionsBoxBottom;
extern u8 g_MapPadDirectionsState;

/* ScriptCommandMapPadDirectionsChange -- script command 0x56
   (g_ScriptCommandFunctions[0x56]; four bytes: the id word, a radius byte at +2, and a
   byte at +3 that is not read): TURN THE D-PAD AND ARM THE BOX AROUND THE
   LEAD. Its one reference in the image is the command table's entry at
   0x800B3494; no argument, nothing returned.

   1. The script pointer g_ScriptCommandPointer steps over the id word (stored), the
      radius byte is read at it, and it steps over the operand word
      (stored again): cmd + 4 on return. r = the byte * 8.
   2. MapPadDirectionsSet((rng_draw() & 3) + 1): ONE RNG draw picks row
      1..4 of the field's D-pad map -- a quarter turn either way or one of
      the two mirrors (the rows are listed above MapPadDirectionsSet,
      map_walk.c).
   3. g_MapPadDirectionsState = 1: the box is armed.
   4. The box, from the lead's record (record 0 of g_MapPartyRecords), its u16
      words +6 and +8, called x and y here:
        g_MapPadDirectionsBoxLeft = x - r      g_MapPadDirectionsBoxRight = x + r
        g_MapPadDirectionsBoxTop = y - r      g_MapPadDirectionsBoxBottom = y + r
      four 16-bit stores.
   The reader is MapPadStep (map_step.c), every field frame while
   g_MapPadDirectionsState is non-zero: it tests g_MapPadDirectionsBoxLeft <= +6 < g_MapPadDirectionsBoxRight and
   g_MapPadDirectionsBoxTop <= +8 < g_MapPadDirectionsBoxBottom (signed); outside the box it puts row 0
   back (MapPadDirectionsSet(0)) and clears g_MapPadDirectionsState, inside it flips
   g_MapPadDirectionsState between 1 and 2 and starts a window ramp each time. So the
   turned pad lasts until the lead has left the square of half-side r
   around where it stood when the command ran.

   No frame wait: MapPadDirectionsSet calls nothing and the RNG does not wait.

   Source shapes the bytes need (each scored by changing it alone):
   - Each coordinate is in TWO locals, the value read and a copy of it,
     and each of the two is then changed in place: the target's
     `move $v1, $v0`, `subu $v0, $v0, $s0`, `addu $v1, $v1, $s0`. With one
     local per coordinate (`D = x - r; D = x + r`) no copy is made: 30/38,
     two instructions short. With no local, the word is read again for
     each store: 25/38, three instructions long.
   - The copy is the chained assignment's (`x1 = x0 = read`), not a
     statement of its own (`x0 = read; x1 = x0;`: 26/38). Chained, the
     copy and the sum made from it are each set once, and the first
     scheduling pass places an instruction that sets such a register
     directly ahead of the last instruction that needs it; as two
     statements x1 is set twice and is ordered like any other.
   - y is read AFTER the store to g_MapPadDirectionsBoxLeft and BEFORE the store to
     g_MapPadDirectionsBoxRight. The read is not a struct member and so is ordered after
     every store ahead of it in the source: with x finished first
     (`... g_MapPadDirectionsBoxRight = x1;` then the y read) the second `lhu` stays below
     both x stores, 35/38; with both reads ahead of the stores all four
     values are live at once and a fourth register is used, 25/38. The
     order below is the low sides, then the high sides; four other orders
     that keep the read between those two stores give the same 38 words,
     and the bytes do not choose among them.
   - The flag is stored before the box (after it: 27/38), the reads are
     unsigned (s16: 36/38, `lh`) and through a cast, not an index
     (`((u16 *)g_MapPartyRecords)[3]`: 25/38 -- that form is a struct access to
     this compiler and its loads move above the stores). The locals are
     ints (s16 or u16: 30/38).
   - Same bytes: a pointer local for the lead's record; the operand read
     through a pointer local (`p = cursor; cursor = p + 2; r = p[2] * 8;
     cursor = p + 4;`, ScriptCommandPlaceNameSet's spelling); an int return type with
     no return statement.
   The Japanese build's twin (0x80061F38, GCC 2.7.2) has the same body
   with its own epilogue; this text compiled with that compiler gives its
   40 instructions, and so do the other four orders. */
/* The first record of g_MapPartyRecords is read through its members'
   addresses (*(u16 *)&...->sprite.x, the x, and sprite.depth, the ground
   y): as member loads (AS(u16, ...)) the x is loaded above the store to
   g_MapPadDirectionsState and the ground y above the store to
   g_MapPadDirectionsBoxLeft, and the code no longer matches. */
/* func_8005EB98 */
void ScriptCommandMapPadDirectionsChange(void)
{
    s32 r;
    s32 x0;
    s32 x1;
    s32 y0;
    s32 y1;

    g_ScriptCommandPointer += 2;
    r = *(u8 *)g_ScriptCommandPointer * 8;
    g_ScriptCommandPointer += 2;
    MapPadDirectionsSet((rng_draw() & 3) + 1);
    g_MapPadDirectionsState = 1;
    x1 = x0 = *(u16 *)&((MapRecord *)g_MapPartyRecords)->sprite.x;
    x0 -= r;
    g_MapPadDirectionsBoxLeft = x0;
    y1 = y0 = *(u16 *)&((MapRecord *)g_MapPartyRecords)->sprite.depth;
    y0 -= r;
    g_MapPadDirectionsBoxTop = y0;
    x1 += r;
    g_MapPadDirectionsBoxRight = x1;
    y1 += r;
    g_MapPadDirectionsBoxBottom = y1;
}

/* ScriptCommandChrLevelGapAdd -- script command 0x57 (g_ScriptCommandFunctions[0x57],
   slot 0x800B3498, the table's only reference to it; four bytes: the id
   word, who, mode): RAISE A MEMBER TO THE LEVEL OF THE PARTY'S FIRST SLOT.

   1. cmd = the command's address; the script pointer g_ScriptCommandPointer goes to
      cmd + 4.
   2. who = cmd[2], a member id; mode = cmd[3].
   3. n = the level (ChrRecord +0x1C) of the member whose id is the first
      of the party's slot ids (the u16 at *g_PartyIds; the id is used as
      read, with no test for an empty slot), own = who's level. Both are
      records of the persistent party table, 0x40 bytes each at
      g_ChrRecords + id * 0x40.
   4. n < own (who is ahead of the first slot): nothing more happens.
   5. Otherwise n -= own, the levels who is behind, and with mode == 1 it
      is halved (a signed / 2; n is never negative here). Any other mode
      byte gives the whole gap.
   6. ChrLevelAdd(who, n) gives who n levels (chr_levelup.c; it stops at
      level 99), then who's HP = max HP and MP = max MP (+0 = +2,
      +4 = +6; g_ChrRecords is read again after the call). With n == 0 the
      call is still made and the refill still happens.

   No return value. No RNG draw and no frame wait of its own; the callee
   chain ChrLevelAdd -> ChrLevelUp -> ChrLevelGainGet / ChrLevelGainAdd
   names neither rng_draw nor a wait (ChrLevelGainAdd's body not read).

   Shapes the bytes pin (each scored as a one-change variant of this
   text; the second number is the Japanese build's twin, SLPS_013.97
   0x80061FD8 under cc272, where 33/39 is the ceiling: the other six rows
   are that build's own gp offsets and call address):
   - ONE variable holds the first slot's level and then the gap,
     `n -= own`: the target subtracts into the register the level was
     loaded in ($a1, the argument's). The gap in a variable of its own,
     `d = n - own`, is 29/38 (24/39): the level then sits in $a2 and who
     in $a3.
   - mode is read into a local AHEAD of the level test, as the target's
     `lbu` above the `slt` shows; tested as `cmd[3] == 1` inside it is
     29/38 (25/39). It is an int: as a u8 retail is still 38/38 but the
     twin is 24/39 (GCC 2.7.2 masks it again at the test).
   - `n /= 2`, the signed divide (`srl 31` / `addu` / `sra 1`); `n >>= 1`
     is 33/38 (28/39).
   - The record address is product first, `(who << 6) + g_ChrRecords`;
     base first at its three places is 35/38 (30/39). HP is refilled
     before MP (MP first: 34/38, 29/39). The function is void (int with
     no return: 37/38, 32/39).
   Same bytes in both builds, so not decided by them: who or own as u8;
   no `own` local, the level read from the record at the test and again
   at the subtraction; the refill through the cast at each member in
   place of `rec`; who read after n; mode read next to who; the first
   slot's id or g_ChrRecords in a local of its own. */
/* func_8005EC30 */
void ScriptCommandChrLevelGapAdd(void)
{
    u8 *cmd;
    s32 who;
    s32 mode;
    s32 n;
    s32 own;
    ChrRecord *rec;

    cmd = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer = (s32)(cmd + 4);
    who = cmd[2];
    n = ((ChrRecord *)((*(u16 *)g_PartyIds << 6) + g_ChrRecords))->level;
    own = ((ChrRecord *)((who << 6) + g_ChrRecords))->level;
    mode = cmd[3];
    if (n >= own) {
        n -= own;
        if (mode == 1) {
            n /= 2;
        }
        ChrLevelAdd(who, n);
        rec = (ChrRecord *)((who << 6) + g_ChrRecords);
        rec->hp = rec->hp_max;
        rec->mp = rec->mp_max;
    }
}

/* ScriptCommandPartyMpHalve -- script command 0x58 (g_ScriptCommandFunctions[0x58], two
   bytes: the id word and no operand): HALVE THE PARTY'S MP after a
   300-frame wait.

   1. The script pointer g_ScriptCommandPointer steps over the id word.
   2. Three stack arrays are zeroed (mem_clear): half (6 u16), mp20
      (6 s32), lost (6 s32).
   3. The field's idle counter g_MapIdleCount = 0x12C, its limit: the pad
      step (map_step.c) counts it up, and raises the status-strip request
      g_MapStatusRequest = 1 on reaching the limit, only while it is BELOW
      0x12C, so that count-up does not run while the counter stays there.
   4. For each party slot i (the six u16 ids at g_PartyIds; a zero id ends
      the walk, an id of 0xC or more is skipped) with the member's 0x40
      record at g_ChrRecords + id * 0x40 and its MP word at +4:
      mp20[i] = mp * 20, half[i] = mp >> 1, lost[i] = mp - half[i].
      mp20 and lost are written and never read (the function has no
      other use of them); 20 is the outer count of the wait below.
   5. 20 x 15 = 300 field frames, ScriptFrameStep(1) each.
   6. The same walk again (both globals re-read): the member's MP word =
      half[i]. Max MP (+6), HP and everything else are left alone.
   7. MapStatusClear(): when the counter is still at 0x12C (or a request
      is pending) it zeroes the counter, sets the request g_MapStatusRequest = 2
      (clear the strip), calls MapStatusDisp and waits one more frame.
   No RNG draw of its own.

   Shapes the bytes pin (each scored as a one-change variant of this
   text): the wait is two nested loops (one loop to 300 is 84/90); id is
   an int (u16 71/90); the MP word is read through the record address
   three times, not once into a local (56/90: the target reloads it after
   each store), mp20 first (half first 86/90); the two pointer globals go
   into locals before each walk AFTER the counter store (before it
   89/90) and the list is INDEXED, ptr[i] -- a `*ptr` / `ptr++` walk
   through one variable in both loops gives that variable one register
   for both (83/90; the target has it in $a2 then $a1), while an index is
   a fresh induction register per loop. In the last walk the locals are
   what the bytes need: the MP store could alias either global, and the
   target does not reload them (read from the globals there: 67/90). */
/* func_8005ECC8 */
void ScriptCommandPartyMpHalve(void)
{
    u16 half[6];
    s32 mp20[6];
    s32 lost[6];
    s32 i;
    s32 j;
    s32 k;
    s32 base;
    u16 *ptr;
    s32 id;

    g_ScriptCommandPointer += 2;
    mem_clear((s32)half, 0xC);
    mem_clear((s32)mp20, 0x18);
    mem_clear((s32)lost, 0x18);
    g_MapIdleCount = 0x12C;
    base = g_ChrRecords;
    ptr = (u16 *)g_PartyIds;
    for (i = 0; i < 6; i++) {
        id = ptr[i];
        if (id == 0) {
            break;
        }
        if (id < 0xC) {
            mp20[i] = *(u16 *)(id * 0x40 + base + 4) * 20;
            half[i] = *(u16 *)(id * 0x40 + base + 4) >> 1;
            lost[i] = *(u16 *)(id * 0x40 + base + 4) - half[i];
        }
    }
    for (j = 0; j < 0x14; j++) {
        for (k = 0; k < 0xF; k++) {
            ScriptFrameStep(1);
        }
    }
    base = g_ChrRecords;
    ptr = (u16 *)g_PartyIds;
    for (i = 0; i < 6; i++) {
        id = ptr[i];
        if (id == 0) {
            break;
        }
        if (id < 0xC) {
            *(u16 *)(id * 0x40 + base + 4) = half[i];
        }
    }
    MapStatusClear();
}
/* far_syms: g_ChrSpellRows (target reaches it %hi/%lo) */
extern s32 g_ChrSpellRows;

/* ScriptCommandChrSpellSet -- event-script command 0x59 (g_ScriptCommandFunctions[0x59],
   slot 0x800B34A0, the function's only reference in the image), PUT A
   SPELL IN A MEMBER'S SPELL LIST: a 4-byte command, the id word, then who
   (a member id) and row (a spell-table row number).

   1. ip = the command's address; the script pointer g_ScriptCommandPointer moves on
      by 4.
   2. who = byte +2, row = byte +3. The row's 12-byte record is
      g_ChrSpellRows + row * 12 - 12: g_ChrSpellRows holds g_BtlSpellRows
      (font_string.c sets it), one record past the EXE table at 0x800AA728,
      so this is that table's row `row` (records.spells).
   3. The record's learner byte (+7, the character id the row belongs to)
      must equal who, where who 0xB counts as 1 -- id 0xB takes id 1's
      rows, as in the level-up (ChrLevelUp). If it does not: error 0xC
      through g_ScriptErrorNumber / ScriptErrorHang, the script error reporter, which
      never returns (see ScriptCommandMapObjectsAdd).
   4. slot = the HIGH NIBBLE of the record's byte +3; the row number is
      stored at spells[slot] (+0x2A + slot) of the member's 0x40-byte
      ChrRecord at g_ChrRecords + who * 0x40 -- the record of who itself (0xB
      keeps its own record; only the learner test maps it to 1).
   Nothing checks who, row or slot against a range: a row of 0 reads the
   bytes before the table g_ChrSpellRows points at (the EXE table's all-zero
   row 0), and a nibble of 8..15 would store past the eight-slot list into
   +0x32..+0x39. In this image rows 1..50 carry nibbles 0..7 only, and for
   learners 1..6 they are the slots the level-up takes from jtbl_800118DC.
   No RNG draw and no frame wait; ScriptErrorHang is the only call.

   Shapes the bytes pin (each scored as a one-change variant of this
   text; the Japanese build's twin, 45 instructions under cc272, has the
   same body and scores 36/45 on this text, the nine other rows being its
   own addresses for the four globals and the callee):
   - ONE int variable, n, is first the id the learner byte is compared
     with and then the list slot; the target has both in $a0. With a
     second local for the slot (32/44; 25/45 on the twin) the member id's
     load lands in a saved register and $a0 gets the copy, and in the last
     block the nibble stays in the loaded byte's register, the g_ChrRecords
     load moves up into the table pointer's load-delay slot (a nop in the
     target) and the build is one instruction short. The slot expression
     written inside spells[...] is 28/44. With the reuse, who = ip[2];
     n = who and n = ip[2]; who = n give the same bytes.
   - `if (n == 0xB) n = 1;` after the copy (who == 0xB ? 1 : who 36/44,
     an if / else 42/44).
   - row, who and n are ints: a u8 row swaps $s0 / $s1 (37/44; 29/45 on
     the twin), a u8 n is 35/44, a u8 who 43/44. The row byte is read
     after the 0xB test (before it: 35/44).
   - The member record's address is product first, (who << 6) +
     g_ChrRecords (base first 42/44).
   g_ChrSpellRows is a far scalar, as in chr_worn.c (ChrSpellUseGet): the
   target's `lui $v1` / `lw $v1, %lo(...)($v1)` on one register is the
   assembler's expansion of a single load. */
/* func_8005EE30 */
void ScriptCommandChrSpellSet(void)
{
    u8 *ip;
    s32 who;
    s32 n;
    s32 row;

    ip = (u8 *)g_ScriptCommandPointer;
    g_ScriptCommandPointer = (s32)(ip + 4);
    who = ip[2];
    n = who;
    if (n == 0xB) {
        n = 1;
    }
    row = ip[3];
    if (*(u8 *)(g_ChrSpellRows + row * 12 - 5) != n) {
        g_ScriptErrorNumber = 0xC;
        ScriptErrorHang();
    }
    n = *(u8 *)(g_ChrSpellRows + row * 12 - 9) >> 4;
    ((ChrRecord *)((who << 6) + g_ChrRecords))->spells[n] = row;
}

/* func_8005EEE0 */
void ScriptCommandPartyMpClear(void)
{
  s32 i;
  s32 base;
  u16 *ptr;
  s32 value;
  ChrRecord *p;
  i = 0;
  base = g_ChrRecords;
  ptr = (u16 *) g_PartyIds;
  while (i < 6)
  {
    value = *ptr;
    if (value == 0)
      break;
    if (value < 0xC)
    {
      if (value != 8)
      {
        if (value != 9)
        {
          p = (ChrRecord *) ((value << 6) + base);
          p->experience_next = 0;
          p->mp_max = 0;
          p->mp = 0;
        }
      }
    }
    i++;
    ptr++;
  }

  g_ScriptCommandPointer += 2;
}
/* far_syms: g_GameEnded */
extern s8 g_GameEnded;
extern s32 g_StoryBits;

/* ScriptCommandStoryBitsItemsReset -- event-script command 0x5C
   (g_ScriptCommandFunctions[0x5C], the index the runner dispatches on, slot 0x800B34AC;
   it is 0x40 only when counted from the slice at 0x800B33AC): CLEAR THE
   EVENT FLAGS AND PUT EVERY MEMBER'S CARRIED ITEMS AND WORN GEAR BACK TO
   THE STARTING SET. Two bytes: the id word and no operand. Nothing calls it
   with a jal.

   In order:
   1. event flag 1 set (StoryBitCheck(1)): g_ScriptEnded = 1 (the runner's
      "this script is over") and g_GameEnded = 1 -- the run loop's END FLAG,
      the one GameRun (map_frame.c) spins on before the disc-2 / ending
      sequence. This is the only store of a non-zero value to it in the
      image. The rest of the command still runs.
   2. g_ScriptCommandPointer += 2 (the script pointer past the command).
   3. mem_clear(g_StoryBits, 0x800): the whole event-flag bitmap, 0x800
      bytes, is zeroed -- flag 1 with it.
   4. resource_load(0xE7, 0x801CFB00): resource 0xE7 (the level-1 party records)
      is read to the fixed buffer ScriptCommandChrLevelItemSet and ChrLevelSet use.
   5. For member id 1..11 (id 0 is skipped):
      - ChrWornItemRemove(id, slot) for slot 1..6: each worn slot (record + 0x1D
        + slot) is zeroed and its bonus taken back; the items it returns are
        dropped, not put in the carried cells;
      - mem_move: the six carried-item cells (ChrRecord.items, +0x24)
        of the persistent record g_ChrRecords + id * 0x40 are overwritten
        with those of 0xE7's record id;
      - ChrWornItemSet(id, slot, item) for slot 1..6: the starting gear is
        put on, bonuses added. The item bytes are 0xE7's table at +0x300,
        six per member from id 1 on, read through one pointer that runs
        across all 66 bytes (the same table ChrRecordsReset equips from).
      Nothing else of the record is touched: level, HP / MP and the stats
      stay (the stats lose the old gear's bonuses and gain the new one's),
      and so do the spells.
   6. mem_clear(g_ChrItemList0C, 0x68): the shared item list is zeroed,
      0x68 bytes (ChrRecordsReset's count; ScriptCommandChrLevelItemSet clears 0x60).
   7. MapSpriteDataFree(1): every loaded field sheet is freed and the party
      sheet list g_MapPartySpriteDataIds cleared. The definition (map_sprite.c) takes
      no parameter; the 1 is loaded and ignored.

   No RNG draw. No frame wait of its own; resource_load is the disc read.

   The two walking pointers: `worn` is set before the loop and `rec` in the
   for's own init, after i (`rec` set before the for, beside `worn`: 66/68,
   its `ori` and i's `li` change places). `rec` indexed instead of walked
   ((i << 6) + 0x801CFB00) is 54/68. The 0xFFFF masks are written at the
   calls: this TU's prototypes for the two helpers are int / unprototyped. */
/* func_8005EF58 */
void ScriptCommandStoryBitsItemsReset(void)
{
    s32 i;
    s32 j;
    u8 *worn;
    ChrRecord *rec;

    if (StoryBitCheck(1) != 0) {
        g_ScriptEnded = 1;
        g_GameEnded = 1;
    }
    g_ScriptCommandPointer += 2;
    mem_clear(g_StoryBits, 0x800);
    resource_load(0xE7, 0x801CFB00);
    worn = (u8 *)0x801CFE00;
    for (i = 1, rec = (ChrRecord *)0x801CFB40; i < 0xC; i++, rec++) {
        for (j = 1; j < 7; j++) {
            ChrWornItemRemove(i & 0xFFFF, j & 0xFFFF);
        }
        mem_move(((ChrRecord *)(g_ChrRecords + (i << 6)))->items, rec->items, 6);
        for (j = 1; j < 7; j++) {
            ChrWornItemSet(i & 0xFFFF, j & 0xFFFF, *worn++);
        }
    }
    mem_clear(g_ChrItemList0C, 0x68);
    MapSpriteDataFree(1);
}

/* func_8005F068 */
void ScriptCommandMapScriptChange(void) {
    s32 command;
    u16 label;
    u8 script_number;
    s32 no_fade_in;
    MapEnterRow e;
    s32 music_number;
    s32 masked_label;
    u16 label_offset;

    command = g_ScriptCommandPointer;
    g_ScriptCommandPointer = command + 2;
    label = (*(u16 *)((u8 *)command + 0x2));
    g_ScriptCommandPointer = command + 4;
    script_number = (*(u8 *)((u8 *)command + 0x4));
    no_fade_in = (*(u8 *)((u8 *)command + 0x5));
    e.map_number = (*(u8 *)((u8 *)command + 0x6));
    e.fade = (*(u8 *)((u8 *)command + 0x7));
    e.limit_left = (*(u8 *)((u8 *)command + 0x8));
    e.limit_right = (*(u8 *)((u8 *)command + 0x9));
    e.limit_top = (*(u8 *)((u8 *)command + 0xA));
    e.limit_bottom = (*(u8 *)((u8 *)command + 0xB));
    e.x = (*(u8 *)((u8 *)command + 0xC));
    e.y = (*(u8 *)((u8 *)command + 0xD));
    e.facing = (*(u8 *)((u8 *)command + 0xE));
    e.sound = (*(u8 *)((u8 *)command + 0xF));
    e.object_set = (*(u8 *)((u8 *)command + 0x10));
    music_number = (*(u8 *)((u8 *)command + 0x11));
    MapChange(0, &e, no_fade_in < 1, g_MoveChrViewFollow, g_MoveChrSecondViewFollow, music_number);
    g_ScriptKeptBlinkCount = 0;
    MapSpriteDataMarksClear();
    ScriptChange(script_number);
    masked_label = label & 0xFFFF;
    label_offset = (*(u16 *)((masked_label * 2) + g_ScriptBase));
    if (label_offset == 0) {
        g_ScriptErrorNumber = 0;
        ScriptErrorHang();
    }
    g_ScriptLabel = label;
    g_ScriptCommandPointer = g_ScriptBase + (label_offset * 2);
}

/* far_syms: g_GameEnterRequest (target reaches it %hi/%lo) */
/* far_syms: g_GameEnterRequest (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_8005F1A8 */
void ScriptCommandGameEnterRequest(void) {
    g_ScriptEnded = 1;
    g_GameEnterRequest = 1;
}

/* func_8005F1C0 */
void ScriptErrorReport() {

}
