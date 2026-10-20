/* Lunar SSSC — menu_card: the save and load pages: the card read, the list
   of saves shown and scrolled, a save written and a load read and laid out.
   The first split had it in battle.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "game.h"
#include "chr.h"
#include "menu.h"
#include "map.h"
#include "card.h"

extern void strcpy(void *dst, u8 *src);
extern s32 card_file_read(void *buf, s32 buffer, s32 offset, s32 byte_count);
extern u8 g_CardSaveFileName[];     /* far array (address-loaded via lui/%lo) */

extern u32 g_MenuBackOutLatch;
extern u32 g_MenuRequest;
extern u16 g_MenuPageStage;
extern u16 g_MapIdleCount;
/* 4-byte word: read as `lw` for value tests and as `lhu` (low half) where
   it feeds MenuPageDisp — one s32 declaration covers both via a cast. */
extern s32 g_MenuPage;
extern u16 g_MenuCardListRow;
extern SpriteEntry *g_MenuRenderEntries;   /* the field menu's sprite entries */
extern SpritePiece *g_MenuShapes;         /* the field menu's shape pieces */
extern s32 g_CardPort;      /* far scalar (far_syms.txt) -> lui $rt + lw */
extern void MenuPagesRun(void);
extern void sprite_run_off(s32 first, s32 count);
extern void MenuFrameStep(void);
extern void font_target_clear(u8 *target, s32 block_count);
extern void MenuPageDisp(s32 page);
extern void font_target_set(u8 *target);
extern s32 font_col_set(s32 col_bits);
extern void font_string_proportional_put(s32 string_id);
extern s32 IconCellSet(s32 x, s32 y, s32 icon, s32 attr);
extern void MenuCardRead(void);
extern void MenuCardListScrollAdjust(void);
extern void mem_clear(void *destination, u32 byte_count);
/* World/camera clip rectangle: X in [g_MapLimitLeft, g_MapLimitRight),
   Y in [g_MapLimitTop, g_MapLimitBottom).  All four gp-relative halfwords. */
extern s16 g_MapLimitLeft;
extern s16 g_MapLimitTop;
extern s16 g_MapLimitRight;
extern s16 g_MapLimitBottom;
extern void SoundPlay(s32 id, s32 ch);
extern u16 g_MusicNumber;     /* currently-loaded track id (low byte used as u8) */
extern s32 MenuLoadRead(s32 slot, s32 buffer);

extern s32 g_PartyIds;     extern s32 g_MapPartyRecords;     /* base pointer (gp-relative word) */
extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table 2 */
extern s32 g_MapObjects;     /* base of a 0x60-stride slot table (gp word) */
extern u16 g_MapNumber;     extern s32 g_MenuOnePageRunning;
extern u16 g_MenuMemberCursor;      /* gp side of the TU wall: 7 gp users vs 1 (MenuUseDrawAreaOpen) %hi/%lo */
extern u16 g_MenuRedrawRequest;
extern s16 g_MapViewX;
extern s16 g_MapViewY;
extern void MenuIconBytesDisp(u16 member_cursor, s32 mode, s32 row_count, s32 cursor);
extern s32 g_SpriteList;
extern s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
/* GCC 2.8.1 emits its 4-word block-move loop + tail word for a 0x24-byte
   struct assignment; used by MenuBtlPlacesDisp to copy BattleEntity+0x48. */
/* MenuLoadRead's two block moves. BlkC00 is byte-aligned on purpose: the
   target tests `cursor & 3` at runtime and carries BOTH an lwl/lwr and an
   lw/sw copy loop, which is what GCC's movstrsi emits when the destination
   alignment is unknown. MapObjectSaveEntry is word-aligned — that copy has no test. */
typedef struct BlkC00 { u8 b[0xC00]; } BlkC00;
extern s32 g_ChrRecords;
extern s32 g_MenuLoadSum;
extern void bzero(void *dst, s32 n);
extern s32 card_save_read(s32 slot, CardSave *save);
extern void *g_MenuRecords;    /* gp side of the TU wall */
extern void MenuCommandsSet(s32, u16);
extern void MenuCommandsDisp(s32);
extern s32 ankprint_rows_clear(s32, s32, s32);
extern void MenuIconBytesSet(s32, s32);
/* Script-side movement projection: steps (*px, *py) `dist` tiles along `dir`
   using the actor's probe rect at `probe`, returns the facing actually used. */
extern const u8 g_MenuMemberPlaces[];

extern s16 g_MenuSoundRequest;
extern s32 g_MenuKeptIds;
extern u16 g_MapPartyBlinkCount;
extern void MenuOpen(void);
extern void MenuClose(void);

/* Opens the field menu straight on page 0x11 (the save page) and runs it
   to its end.  Called by the field frame step MapPartyStep on the frame
   pad bit 0x20 is released.

   1. Page g_MenuPage = 0x11.  font_target_clear(0x801CBB00, 0x7F) points the
      text layer at that buffer and zeroes 0x7F rows of it.  The slot-list
      stage g_MenuPageStage = 0 (its first frame), the pending sound
      g_MenuSoundRequest = 2, the field's idle counter g_MapIdleCount = 0 and the
      redraw request g_MenuRedrawRequest = the page.
   2. The scratch block *g_MenuKeptIds is zeroed (0xC bytes) and the six
      party ids at *g_PartyIds are saved into it.  An id of 0x1F or more
      is blanked in the list for the menu's duration and its actor record
      at *g_MapPartyRecords is marked: the hold +1 = 1 and bit 15 of +0xE.
   3. The blink counter g_MapPartyBlinkCount is saved and zeroed, the modal flag
      g_MenuOnePageRunning = 1, the page is stored again, the party slot
      g_MenuMemberCursor = 0, and the party screen runs: MenuOpen opens it,
      MenuPagesRun is the page loop (it returns when the page is 0),
      MenuClose closes it.
   4. g_MenuOnePageRunning = 0; every id is put back and every record's hold and
      bit 15 are cleared - on all six, not only the ones step 2 marked.
      The blink counter is restored and MenuIconBytesDisp(0xFFFF, 1, 2, 0)
      draws the item grid with no member.

   The id-list stores are written as array elements: an element store is
   one the scheduler lets the following load of the scalar g_MapPartyRecords
   pass, a store through a cast integer sum is not (86/94, 91/94 and 79/94
   for the three stores spelled that way). */
/* func_80072890 */
void MenuSaveRun(void)
{
  s32 i;
  u16 saved;

  g_MenuPage = 0x11;
  font_target_clear((u8 *) 0x801CBB00, 0x7F);
  g_MenuPageStage = 0;
  g_MenuSoundRequest = 2;
  g_MapIdleCount = 0;
  g_MenuRedrawRequest = g_MenuPage;
  mem_clear((void *) g_MenuKeptIds, 0xC);
  for (i = 0; i < 6; i++)
  {
    ((u16 *) g_MenuKeptIds)[i] = ((u16 *) g_PartyIds)[i];
    if (((u16 *) g_PartyIds)[i] >= 0x1F)
    {
      ((u16 *) g_PartyIds)[i] = 0;
      ((MapRecord *) g_MapPartyRecords)[i].hold_definition.hold = 1;
      ((MapRecord *) g_MapPartyRecords)[i].sprite.flags |= 0x8000;
    }
  }
  saved = g_MapPartyBlinkCount;
  g_MenuOnePageRunning = 1;
  g_MapPartyBlinkCount = 0;
  g_MenuPage = 0x11;
  g_MenuMemberCursor = 0;
  MenuOpen();
  MenuPagesRun();
  MenuClose();
  g_MenuOnePageRunning = 0;
  for (i = 0; i < 6; i++)
  {
    ((u16 *) g_PartyIds)[i] = ((u16 *) g_MenuKeptIds)[i];
    ((MapRecord *) g_MapPartyRecords)[i].hold_definition.hold = 0;
    ((MapRecord *) g_MapPartyRecords)[i].sprite.flags &= 0x7FFF;
  }
  g_MapPartyBlinkCount = saved;
  MenuIconBytesDisp(0xFFFF, 1, 2, 0);
}

/* la_absolute: MenuPage7Step */
extern u16 g_MenuPage7Cursor;
extern u8 g_MenuCommands[8];
extern u16 g_MenuPadRecord[];
extern s32 g_MenuBackOutPattern;
extern s16 g_MenuSoundRequest;

/* Menu page 7 (battle_phase_pre_table[7], 0x800B5A14 -- the only reference
   to this function): the sub-menu behind entry 4 of the member's command
   list (MenuCommandStep sends cursor + 3 here) -- input step and
   redraw. Its three entries lead to pages 0x11, 0x12 and 0x13: the two
   save-page handlers (MenuSaveStep, MenuLoadStep) and the Options page
   (MenuOptionStep).

   The cursor is g_MenuPage7Cursor over the entry bytes g_MenuCommands (the list page 4
   walks with its own cursor). A cursor of 3 or more is repaired first:
   MenuCursorMove(cursor, g_MenuCommands, 0x4000) steps it down the list.

   Then, first match wins:
   - g_MenuPadRecord[2] & 0x5000 (up / down): the cursor steps through
     MenuCursorMove(cursor, g_MenuCommands, bits); SE request 1, redraw.
     (L1 / R1 and left / right do nothing on this page.)
   - else g_MenuPadRecord[1] & 0x130, or the latch g_MenuBackOutLatch & g_MenuBackOutPattern:
     g_MenuBackOutLatch = 1 on 0x100, 2 on 0x20; g_MenuBackOutPattern = 0x30B30B30; back to
     page 2 (g_MenuPage = 2); SE request 3; actors 3 and 0 of g_MenuRecords
     get +4 = 0; redraw of page 2. Unlike page 4 there is no
     MenuPageDisp(2) here.
   - else g_MenuPadRecord[1] & 0x40 (Cross): actor 0 to state 0xB
     (MapAnimationSet), actors 3 and 0 +4 = 0, the page becomes cursor +
     0x11, font_target_clear(0x801CBB00, 0x7F) clears the text buffer, the
     stage g_MenuPageStage = 0 (what the Options page tests to open itself);
     SE request 2, and the new page is marked for redraw. No frame passes.

   Redraw (g_MenuRedrawRequest == 7): who = the member in the selected party slot
   (*(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds)),
   MenuCommandsSet(who, page), MenuCommandsDisp(cursor), MenuIconBytesSet(who,
   page), MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, 0) (the item grid with the
   member's face), MenuCursorTextDisp(g_MenuCommands[cursor], 0),
   MenuPageDisp(page), actor 0 to state 0xA, g_MenuRedrawRequest = 0, actors 3 and
   0 +4 = 1. Every call ends with MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0).

   No frame wait and no RNG draw of its own.

   Codegen notes: the page-4 shape carried over unchanged. The pad record
   is read as g_MenuPadRecord[n] at each use (cse keeps the far address in $v1
   for the [2] and [1] reads on the fall-through path); the sized small
   array g_MenuCommands[8] gives the assembler's same-register lui/addiu under
   la_absolute; g_MenuPage is this TU's s32, read (u16) where it feeds a
   call or g_MenuRedrawRequest; the actors are this TU's MapRecord through the void *
   g_MenuRecords; MenuCursorMove, MapAnimationSet, MenuCursorTextDisp and
   MenuLineUpDisp are called undeclared, as elsewhere in the first split's
   battle.c. */
/* func_80072A08 */
void MenuPage7Step(void)
{
    s32 who;

    if (g_MenuPage7Cursor >= 3) {
        g_MenuPage7Cursor = MenuCursorMove(g_MenuPage7Cursor, g_MenuCommands, 0x4000);
    }
    if (g_MenuPadRecord[2] & 0x5000) {
        g_MenuPage7Cursor = MenuCursorMove(g_MenuPage7Cursor, g_MenuCommands, g_MenuPadRecord[2] & 0x5000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuPage = 2;
        g_MenuSoundRequest = 3;
        ((MapRecord *)g_MenuRecords)[3].sprite.on = 0;
        ((MapRecord *)g_MenuRecords)[0].sprite.on = 0;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        MapAnimationSet(g_MenuRecords, 0xB);
        ((MapRecord *)g_MenuRecords)[3].sprite.on = 0;
        ((MapRecord *)g_MenuRecords)[0].sprite.on = 0;
        g_MenuPage = g_MenuPage7Cursor + 0x11;
        font_target_clear((u8 *)0x801CBB00, 0x7F);
        g_MenuPageStage = 0;
        g_MenuSoundRequest = 2;
        g_MenuRedrawRequest = g_MenuPage;
    }
    if (g_MenuRedrawRequest == 7) {
        who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
        MenuCommandsSet(who, (u16)g_MenuPage);
        MenuCommandsDisp(g_MenuPage7Cursor);
        MenuIconBytesSet(who, (u16)g_MenuPage);
        MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, 0);
        MenuCursorTextDisp(g_MenuCommands[g_MenuPage7Cursor], 0);
        MenuPageDisp((u16)g_MenuPage);
        MapAnimationSet(g_MenuRecords, 0xA);
        g_MenuRedrawRequest = 0;
        ((MapRecord *)g_MenuRecords)[3].sprite.on = 1;
        ((MapRecord *)g_MenuRecords)[0].sprite.on = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
}

void MenuSaveOpen(void);  /* TU defines this below */
void MenuLoadOpen(void);  /* TU defines this below */
extern u16 g_MenuPadRecord[];
s32 card_result_get(void);
s32 card_format();
s32 card_dir_read();
s32 pad_record_update();
s32 MenuAnswerRun();
s32 MenuNoticeDisp();
void MenuCardListOpen();
s32 MenuCardListRead();
extern s32 g_MenuCardResult;
extern s32 g_MenuCardFreeBlockCount;
extern s32 g_MenuCardReadFailed;
extern s32 g_MenuNoticeCol8;
extern s32 g_MenuCardBuffer;
extern s16 g_MenuCardListLast;
extern s32 g_MenuCardKeptPort;
extern s32 g_MenuCardKeptSlot;
extern s16 g_MenuCardListScroll;
extern MenuCardSlotFlags g_MenuCardSlotFlags;
extern u8 g_MenuCardListSlots[];

/* The save/load slot list's card read: MenuSaveOpen runs it when the screen
   opens, MenuLoadOpen is its other caller. From `retry`: the 0x20-byte
   table g_MenuCardSlotFlags is cleared, MenuCardListOpen runs, ankprint_rows_clear(0x14, 6,
   0x20) and MenuNoticeDisp(0x675, 0xC, 0x16, 1, 0) put up the first notice
   (0x675..0x67E read as message ids, like the prompt's 0x670), two frames
   pass (MenuFrameStep), the result g_MenuCardResult and g_MenuCardReadFailed are zeroed and
   g_MenuCardFreeBlockCount = card_dir_read() reads the card in port g_CardPort.
   g_MenuCardFreeBlockCount >= 0: MenuCardListRead(g_MenuCardBuffer) == 0 with g_MenuCardFreeBlockCount == 0 is
   result 1, and g_MenuCardReadFailed == 1 afterwards starts over. Then the cursor
   (row g_MenuCardListRow + scroll g_MenuCardListScroll): on another port than the one
   recorded in g_MenuCardKeptPort it is zeroed; on that port it stays where
   g_MenuCardListSlots[row + scroll] is still the value recorded in g_MenuCardKeptSlot, and is
   otherwise zeroed and walked forward (the row up to 2, then the scroll)
   until that value is under it, giving up past g_MenuCardListLast - or at it, unless
   the mode g_MenuPage is 0x11.
   g_MenuCardFreeBlockCount < 0: on the recorded port g_MenuCardKeptSlot is dropped, then the card
   status card_result_get() decides. 1 -> result 2, 2 -> result 3, both
   flipping the port (g_CardPort ^= 0x10). 4 -> g_MenuNoticeCol8 = 1 and notice
   0x67E at row 0x16 (0x14 in mode 0x11); outside mode 0x11 that is result 4,
   in it the two-choice prompt 0x670 follows: 0 puts up 0x679, lets two frames
   pass, runs card_format(), re-arms the pad record
   (pad_record_update(g_MenuPadRecord, 0x14)) and starts over; 2 starts over; 1 is
   result 4; any other answer returns with result 0. Other statuses return
   with result 0.
   Source shape: starting over is a label and gotos (a for (;;) with continue
   scores 152/207); the status is unsigned (sltiu in the case tree; s32 is
   206) and result 4 is stored as `= r` (`= 4` is 168). */
/* func_80072C1C */
void MenuCardRead(void)
{
    u32 r;
    s32 y;

retry:
    mem_clear(&g_MenuCardSlotFlags, 0x20);
    MenuCardListOpen();
    ankprint_rows_clear(0x14, 6, 0x20);
    MenuNoticeDisp(0x675, 0xC, 0x16, 1, 0);
    MenuFrameStep();
    MenuFrameStep();
    g_MenuCardResult = 0;
    g_MenuCardReadFailed = 0;
    g_MenuCardFreeBlockCount = card_dir_read();
    if (g_MenuCardFreeBlockCount >= 0) {
        if (MenuCardListRead(g_MenuCardBuffer) == 0 && g_MenuCardFreeBlockCount == 0) {
            g_MenuCardResult = 1;
        }
        if (g_MenuCardReadFailed == 1) {
            goto retry;
        }
        if (g_CardPort == g_MenuCardKeptPort) {
            if (g_MenuCardListSlots[g_MenuCardListScroll + g_MenuCardListRow] != g_MenuCardKeptSlot) {
                g_MenuCardListScroll = 0;
                g_MenuCardListRow = 0;
                while (g_MenuCardListSlots[g_MenuCardListScroll + g_MenuCardListRow] != g_MenuCardKeptSlot) {
                    if (g_MenuCardListLast < g_MenuCardListScroll + g_MenuCardListRow) {
                        break;
                    }
                    if (g_MenuPage != 0x11 && g_MenuCardListScroll + g_MenuCardListRow == g_MenuCardListLast) {
                        break;
                    }
                    if (g_MenuCardListRow < 2) {
                        g_MenuCardListRow++;
                    } else {
                        g_MenuCardListScroll++;
                    }
                }
            }
        } else {
            g_MenuCardListScroll = 0;
            g_MenuCardListRow = 0;
        }
    } else {
        if (g_CardPort == g_MenuCardKeptPort) {
            g_MenuCardKeptSlot = 0;
        }
        r = card_result_get();
        switch (r) {
        case 1:
            g_MenuCardResult = 2;
            g_CardPort ^= 0x10;
            break;
        case 2:
            g_MenuCardResult = 3;
            g_CardPort ^= 0x10;
            break;
        case 4:
            g_MenuNoticeCol8 = 1;
            y = 0x16;
            if (g_MenuPage == 0x11) {
                y = 0x14;
            }
            MenuNoticeDisp(0x67E, 0xC, y, 2, 0);
            if (g_MenuPage == 0x11) {
                switch (MenuAnswerRun(0x670, 0, 0x19, 0x18, 1)) {
                case 0:
                    ankprint_rows_clear(0x14, 6, 0x20);
                    MenuNoticeDisp(0x679, 0xC, y, 2, 0);
                    MenuFrameStep();
                    MenuFrameStep();
                    card_format();
                    pad_record_update(g_MenuPadRecord, 0x14);
                    goto retry;
                case 2:
                    goto retry;
                case 1:
                    break;
                default:
                    return;
                }
            }
            g_MenuCardResult = r;
            break;
        }
    }
}

/* func_80072F58 */
void MenuSaveOpen(void) {
    u16 counter;
    IconCellSet(9, 2, 0x2B, g_CardPort << 24);
    IconCellSet(0x12, 2, 0x49, (g_CardPort ^ 0x10) << 24);
    IconCellSet(0xC, 2, 0x7C, 0);
    IconCellSet(0xF, 2, 0xB6, 0);
    MenuCardRead();
    if (g_MenuCardSlotFlags.pickable_count == 0) {
        g_MenuCardListRow = 0;
    } else {
        if (g_MenuCardListRow >= 3) {
            g_MenuCardListRow = 0;
        }
        MenuCardListScrollAdjust();
    }
    counter = g_MenuPageStage;
    g_MenuPageStage = counter + 1;
    if (g_MenuCardSlotFlags.pickable_count != 0) {
        g_MenuPageStage = counter + 2;
        g_MenuRenderEntries[2].on = 1;
        g_MenuShapes[2].texel_col.col = 0x76A;
    }
}
/* far_syms: g_PlayTimeMinutes g_CardPort */
/* load_hazard_nop: MenuSaveStep */
extern s16 g_MenuCardListScroll;
extern s16 g_MenuCardListLast;
extern u16 g_PlayTimeMinutes;
extern u16 g_MenuKeptPlayTimeMinutes[2];
extern u16 g_MenuPadRecord[];
extern s32 g_MenuBackOutPattern;
extern s32 g_MenuCardResult;
extern s32 g_MenuCardAcceptResult;
extern s16 g_MenuSoundRequest;
extern u16 g_MenuPadRecordPressed[];
void MenuCardListClose(s32 page);  /* TU defines this below */
void MenuSaveWrite(void);  /* TU defines this below */
s32 card_change_check(void);

/* The save/load slot-list screen's per-frame input (the list MenuCardListDisp
   draws; MenuLoadStep is its twin for the other list). Stage g_MenuPageStage:
   0 = first frame (MenuSaveOpen builds the panels, the window for mode
   g_MenuPage opens), 2 = the list is active.
   Pad word g_MenuPadRecord[2]: 0x1000 steps the cursor up (row g_MenuCardListRow first,
   then the scroll g_MenuCardListScroll) and then pulls row + scroll back down to
   g_MenuCardListLast; 0x4000 steps down (rows 0..2, then the scroll, bounded by the
   slot count g_MenuCardSlotFlags.pickable_count and g_MenuCardListLast). g_MenuPadRecord[1] & 0x110, the
   g_MenuBackOutLatch & g_MenuBackOutPattern pair, or a fresh 0x20 answer the prompt
   (g_MenuBackOutLatch = 1 for 0x100, 2 for 0x20) and close through MenuCardListClose.
   g_MenuPadRecordPressed[0] & 0x40 confirms: with no slots the result g_MenuSoundRequest is 4,
   otherwise 2, and MenuSaveWrite runs on the picked slot (stage 2) or the
   stage advances. 4 / 8 pick g_CardPort = 0 / 0x10 and restart the screen.
   Every change republishes the mode through g_MenuRedrawRequest; 0x11 there redraws
   the windows.
   Source shape: MenuCardListClose's argument is an if/else of two calls (the
   a0 = 7; if (...) a0 = 0; form swaps the constant's and g_MenuOnePageRunning's
   registers); the down test reads row + scroll <= g_MenuCardListLast + 1. */
/* func_80073054 */
void MenuSaveStep(void)
{
    if (g_MenuPageStage == 0) {
        MenuSaveOpen();
        MenuPageDisp((u16)g_MenuPage);
        g_MenuKeptPlayTimeMinutes[0] = g_PlayTimeMinutes;
    }
    if (g_MenuKeptPlayTimeMinutes[0] != g_PlayTimeMinutes) {
        g_MenuKeptPlayTimeMinutes[0] = g_PlayTimeMinutes;
        g_MenuRedrawRequest = g_MenuPage;
    }
    if (g_MenuPadRecord[2] & 0x1000) {
        if (g_MenuPageStage == 2) {
            if (g_MenuCardListRow != 0) {
                g_MenuCardListRow--;
            } else if (g_MenuCardListScroll > 0) {
                g_MenuCardListScroll--;
            }
            while (g_MenuCardListLast < g_MenuCardListRow + g_MenuCardListScroll) {
                if (g_MenuCardListRow != 0) {
                    g_MenuCardListRow--;
                } else if (g_MenuCardListScroll > 0) {
                    g_MenuCardListScroll--;
                } else {
                    break;
                }
            }
            g_MenuSoundRequest = 1;
            g_MenuRedrawRequest = g_MenuPage;
            MenuCardListScrollAdjust();
        }
    } else if (g_MenuPadRecord[2] & 0x4000) {
        if (g_MenuPageStage == 2) {
            if (g_MenuCardListRow < 2) {
                g_MenuCardListRow++;
            } else if (g_MenuCardListLast < g_MenuCardSlotFlags.pickable_count - 1) {
                if (g_MenuCardListRow + g_MenuCardListScroll <= g_MenuCardListLast + 1) {
                    g_MenuCardListScroll++;
                }
            } else if (g_MenuCardListRow + g_MenuCardListScroll < g_MenuCardListLast) {
                g_MenuCardListScroll++;
            }
            g_MenuSoundRequest = 1;
            g_MenuRedrawRequest = g_MenuPage;
            MenuCardListScrollAdjust();
        }
    } else if ((g_MenuPadRecord[1] & 0x110) || (g_MenuBackOutLatch & g_MenuBackOutPattern) ||
               ((g_MenuPadRecord[3] & 0x20) && !(g_MenuPadRecord[0] & 0x20))) {
        if (g_MenuPadRecordPressed[0] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecordPressed[0] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        if (g_MenuOnePageRunning != 0) {
            MenuCardListClose(0);
        } else {
            MenuCardListClose(7);
        }
        g_MenuSoundRequest = 3;
    } else if (g_MenuPadRecordPressed[0] & 0x40) {
        if (g_MenuCardSlotFlags.pickable_count == 0) {
            g_MenuSoundRequest = 4;
        } else {
            g_MenuSoundRequest = 2;
            if (g_MenuPageStage == 2) {
                g_MenuShapes->texel_col.col = 0x76A;
                MenuSaveWrite();
                g_MenuShapes->texel_col.col = 0x761;
            } else {
                MenuCardListScrollAdjust();
                g_MenuRenderEntries[2].on = 1;
                g_MenuShapes[2].texel_col.col = 0x76A;
                g_MenuPageStage++;
            }
            g_MenuRedrawRequest = g_MenuPage;
            MenuCardListScrollAdjust();
        }
    } else {
        if ((g_MenuPadRecordPressed[0] & 4) && g_CardPort != 0) {
            g_CardPort = 0;
            goto toggled;
        }
        if ((g_MenuPadRecordPressed[0] & 8) && g_CardPort != 0x10) {
            g_CardPort = 0x10;
        toggled:
            g_MenuPageStage = 0;
            card_change_check();
            g_MenuRedrawRequest = g_MenuPage;
            SoundPlay(1, 1);
        }
    }
    if (g_MenuRedrawRequest == 0x11) {
        MenuIconBytesSet(0xC, (u16)g_MenuPage);
        MenuIconBytesDisp(g_MenuMemberCursor, 1, 1, 0xC);
        MenuPageDisp((u16)g_MenuPage);
        g_MenuRedrawRequest = 0;
    }
    if (card_change_check() != 0) {
        g_MenuPageStage = 0;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuCardResult == 2 && g_MenuPageStage != 0) {
        g_CardPort ^= 0x10;
    }
    if (g_MenuCardResult == 0 && g_MenuCardAcceptResult != 0 && g_MenuCardAcceptResult != 3) {
        g_MenuPageStage = 0;
        g_MenuRedrawRequest = g_MenuPage;
    }
}

extern u8 D_800B8808[];
extern u8 D_800B880C[];     /* "-" */
extern u8 D_800B8810[];     /* "%d" */
extern u8 g_MenuCardListSlots[];
extern MenuCardSlotFlags g_MenuCardSlotFlags;
extern GameSaveHead g_MenuCardSlotHeads[];
extern s16 g_MenuCardListScroll;
extern s16 g_MenuCardListLast;
extern u16 g_MapPlaceName;
extern u16 g_PlayTimeMinutes;
s32 fmt_sprintf(s8 *, u8 *, ...);     /* sprintf */
void font_bytes_put(u8 *string, s32 mode);
void font_string_fixed_put(s32 string_id);
s32 MenuTallGlyphSet(s32, s32, s32);
s32 MenuTallNumberSet(s32, s32, s32, s32);
s32 CellNumberSet(s32, s32, s32, s32, s32, s32, s32, s32);
/* The save/load screen's slot list. For each of the three visible slots
   (n = g_MenuCardListScroll + i) it prints the slot label (sprintf "-" for an empty
   index, else "%d" of n + 1) and, when the slot holds a save
   (g_MenuCardSlotFlags.flags[g_MenuCardListSlots[n]] neither 0xFF nor 0), that save header's byte
   +0xF, its u16 +0xA split /60 and %60, and the glyph 0x400 + byte +0xE.
   Then the current game's line (the lead record's byte +0x1C, g_PlayTimeMinutes
   /60 %60) unless g_MenuPage is 0x14, and the window row heights.

   Source shapes the codegen pins:
   - the "-" arm calls sprintf with TWO arguments: a third one makes the two
     calls identical and jump2 cross-jumps them into one;
   - page/row/col are per-iteration values assigned at the top in that order
     and row9 = i * 9 is set inside the save-present block, right before the
     g_MenuPageStage test that uses it: set there (conditional, used in another
     block) it is not replaceable, so loop.c reduces 9i itself and lists it
     first;
   - i holds 0x1FF/0x1FE before the loop (it is $s1 in both roles);
   - h is a u8: its extra RTL insn keeps loop.c from hoisting g_MenuCardSlotFlags. */
/* func_800734F0 */
void MenuCardListDisp(void)
{
    s8 name[8];
    s32 i;
    s32 col;
    s32 row;
    s32 row9;
    s32 page;
    s32 idx;
    s32 v;
    s32 old;
    u8 h;
    s32 y;

    font_target_clear((u8 *)0x801CCD80, 0x24);
    font_target_clear((u8 *)0x801CCC00, 3);
    i = 0x1FF;
    if (g_MenuPage == 0x11) {
        i = 0x1FE;
    }
    font_string_proportional_put(i);
    font_target_clear((u8 *)0x801CDF80, 8);
    font_target_set((u8 *)0x801CDF80);
    font_string_fixed_put(0x617);
    font_target_clear((u8 *)0x801CDF80, 3);
    font_target_set((u8 *)0x801CE280);
    font_bytes_put(D_800B8808, 1);
    font_target_clear((u8 *)0x801CE380, 0x24);
    i = 0;
    do {
        page = (i * 9 + 0x25) << 7;
        row = i * 4 + 8;
        col = i * 4 + 6;
        font_target_set((u8 *)(((i + 0x49) << 7) + 0x801CBB00));
        idx = g_MenuCardListScroll + i;
        if (g_MenuCardListSlots[idx] == 0xFF) {
            fmt_sprintf(name, D_800B880C);
        } else {
            fmt_sprintf(name, D_800B8810, idx + 1);
        }
        font_bytes_put((u8 *)name, 1);
        CellTextDisp(0xC, col, 1, 1, 1, 0xDEAD2001, (i * 4 - 0x11DC) & 0xFFFF);
        MenuTallGlyphSet(0xC, 0xE, col);
        font_target_set((u8 *)(page + 0x801CBB00));
        if (g_MenuCardListScroll + i <= g_MenuCardListLast) {
            h = g_MenuCardSlotFlags.flags[g_MenuCardListSlots[g_MenuCardListScroll + i]];
            if (h != 0xFF && h != 0) {
                MenuTallGlyphSet(0xA, 0x10, row);
                MenuTallGlyphSet(0xB, 0x11, row);
                MenuTallNumberSet(((GameSaveHead *)g_MenuCardSlotHeads)[g_MenuCardListSlots[g_MenuCardListScroll + i]].level, 0x12, row, 2);
                MenuTallNumberSet((((GameSaveHead *)g_MenuCardSlotHeads)[g_MenuCardListSlots[g_MenuCardListScroll + i]].play_time_minutes / 60) & 0xFFFF, 0x15, row, 3);
                CellTextDisp(0x18, row, 2, 1, 2, 0xDEAD2001, 0xEE34);
                CellNumberSet((((GameSaveHead *)g_MenuCardSlotHeads)[g_MenuCardListSlots[g_MenuCardListScroll + i]].play_time_minutes % 60) & 0xFFFF, 0x1C, row, 2,
                              0xDEAD2001, 0x3000, 1, 1);
                CellTextDisp(0x1E, row, 1, 1, 1, 0xDEAD2001, 0xEE30);
                v = ((GameSaveHead *)g_MenuCardSlotHeads)[g_MenuCardListSlots[g_MenuCardListScroll + i]].place_name + 0x400;
                old = font_col_set(0);
                font_string_proportional_put(v);
                row9 = i * 9;
                if (g_MenuPageStage != 0) {
                    CellTextDisp(0x10, col, 9, 1, 9, 0xDEAD2001, (row9 * 4 - 0x126C) & 0xFFFF);
                }
                font_col_set(old & 0xFF);
            }
        }
        i++;
    } while (i < 3);
    if (g_MenuPage == 0x12 || g_MenuPage == 0x14) {
        font_target_clear((u8 *)0x801CF500, 8);
        font_target_set((u8 *)0x801CF500);
        font_string_proportional_put(0x634);
        CellTextDisp(0x10, 0x12, 8, 1, 8, 0xDEAD2001, 0xEED0);
    }
    CellTextDisp(4, 5, 3, 1, 3, 0xDEAD2001, 0xED88);
    if (g_MenuPage != 0x14) {
        CellTextDisp(0xC, 0x16, 2, 1, 2, 0xDEAD2001, 0xEE3C);
        MenuTallGlyphSet(0xC, 0x10, 0x16);
        font_target_set((u8 *)0x801CDB00);
        font_string_proportional_put(g_MapPlaceName + 0x400);
        CellTextDisp(0x12, 0x16, 9, 1, 9, 0xDEAD2001, 0xEE00);
        MenuTallGlyphSet(0xA, 0x12, 0x18);
        MenuTallGlyphSet(0xB, 0x13, 0x18);
        MenuTallNumberSet(*(u8 *)((*(u16 *)g_PartyIds << 6) + g_ChrRecords + 0x1C), 0x14, 0x18, 2);
        MenuTallNumberSet((g_PlayTimeMinutes / 60) & 0xFFFF, 0x17, 0x18, 3);
        CellTextDisp(0x1A, 0x18, 2, 1, 2, 0xDEAD2001, 0xEE34);
        CellNumberSet((g_PlayTimeMinutes % 60) & 0xFFFF, 0x1E, 0x18, 2, 0xDEAD2001, 0x3000, 1, 1);
        CellTextDisp(0x20, 0x18, 1, 1, 1, 0xDEAD2001, 0xEE30);
    }
    y = g_MenuCardListRow * 0x20 + 0x30;
    g_MenuRenderEntries[2].y = (u16)g_MapViewY + y;
    if (g_MenuCardListRow != 3) {
        g_MenuShapes->x2 = 0x8F;
        g_MenuShapes->x1 = 0x8F;
        g_MenuShapes->y3 = 0x1F;
        g_MenuShapes->y2 = 0x1F;
    } else {
        g_MenuShapes->x2 = 0x4F;
        g_MenuShapes->x1 = 0x4F;
        g_MenuShapes->y3 = 0xF;
        g_MenuShapes->y2 = 0xF;
    }
    *(SpriteEntry *)((u8 *)g_SpriteList + 0x12FC) = *(SpriteEntry *)((u8 *)g_MenuRenderEntries + 0x48);
}
/* The save/load slot list's scroll clamp and its two scroll markers: run
   when the list opens (MenuSaveOpen, MenuLoadOpen) and after every cursor
   step and pick (MenuSaveStep, MenuLoadStep). Row g_MenuCardListRow, scroll
   g_MenuCardListScroll, last list index g_MenuCardListLast; the markers are the +4 halfwords
   of actors 5 and 6 of the g_MenuRecords block (+0x130, +0x16C; "up" and
   "down" are this reading, from what switches them).
   Page g_MenuPage == 0x11 (three rows): a row of 3 or more becomes row 2
   with the scroll one further. The scroll is then pulled back, one at a
   time and never below 0, while scroll + 2 is past g_MenuCardListLast + 1. Marker
   6 is off when g_MenuCardListLast < scroll + 2 or scroll + 2 is 0xE (the third
   row is then the fifteenth entry), on otherwise.
   Other pages: when the card read g_MenuCardFreeBlockCount is -1 the row is 3, the
   scroll 0 and both markers off, and that is all. Otherwise a row of 3 or
   more is 3 (MenuLoadStep steps to row 3 below the third row), the
   scroll is pulled back the same way, and marker 6 is off on row 3, on
   otherwise.
   Both list arms end with marker 5 on while the scroll is not 0.
   Source shape: each arm is written out in full (cross-jumping merges the
   tails; one shared 0x130 test after the chain is 78/95), and the loop
   test reads scroll + 2 > last + 1 (last + 1 < scroll + 2 is 80/95: its
   entry guard then cross-jumps into the bottom test). */
/* func_80073BDC */
void MenuCardListScrollAdjust(void)
{
    if (g_MenuPage == 0x11) {
        if (g_MenuCardListRow >= 3) {
            g_MenuCardListRow = 2;
            g_MenuCardListScroll++;
        }
        while (g_MenuCardListScroll + 2 > g_MenuCardListLast + 1 && g_MenuCardListScroll != 0) {
            g_MenuCardListScroll--;
        }
        if (g_MenuCardListLast < g_MenuCardListScroll + 2 || g_MenuCardListScroll + 2 == 0xE) {
            ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
        } else {
            ((MapRecord *)g_MenuRecords)[6].sprite.on = 1;
        }
        if (g_MenuCardListScroll != 0) {
            ((MapRecord *)g_MenuRecords)[5].sprite.on = 1;
        } else {
            ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
        }
    } else if (g_MenuCardFreeBlockCount == -1) {
        g_MenuCardListRow = 3;
        g_MenuCardListScroll = 0;
        ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
        ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
    } else {
        if (g_MenuCardListRow >= 3) {
            g_MenuCardListRow = 3;
        }
        while (g_MenuCardListScroll + 2 > g_MenuCardListLast + 1 && g_MenuCardListScroll != 0) {
            g_MenuCardListScroll--;
        }
        if (g_MenuCardListRow == 3) {
            ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
        } else {
            ((MapRecord *)g_MenuRecords)[6].sprite.on = 1;
        }
        if (g_MenuCardListScroll != 0) {
            ((MapRecord *)g_MenuRecords)[5].sprite.on = 1;
        } else {
            ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
        }
    }
}

extern u8 g_MenuCommands[8];

/* The save / load slot list's screen set-up: MenuCardRead (the list's card
   read) runs it at each `retry`, after clearing the slot table g_MenuCardSlotFlags
   and before its first notice. No frame wait and no RNG draw of its own.

   In order:
   - anim actors 5 and 6 of the g_MenuRecords block (the 0x3C-byte records
     MenuRecordsInit builds with anims 4 and 5) get their +4 word cleared and
     are placed at x = g_MapViewX + 0x108, y = g_MapViewY + 0x28 and + 0x90;
   - MenuPageDisp redraws the body of the page g_MenuPage: on the save
     pages 0x11 / 0x12 / 0x14 that is the save list and its notice;
   - sprite_run_off frees sprite slots 0x87..0x8E;
   - render entry 2 of g_MenuRenderEntries (the one that draws slice 0, the cursor
     highlight) has its +0 word cleared and its x set to g_MapViewX + 0x80;
     slice 2 of g_MenuShapes (its 0x18-byte record at +0x30) gets both y
     extents +0x12 / +0x16 = 0xF, 16 rows tall; render entry 4, the one
     that draws slice 2, goes to y = g_MapViewY + 0x18;
   - MenuCommandsSet(0, page) clears the six command bytes g_MenuCommands and
     stores the page's own byte in [0] (pages 0x11..0x13: g_MenuPage7Commands[page -
     0x11]; 0x14: 0xFF; 0x15: 0xFD), and MenuCommandsDisp(0) draws the four
     command cells (IconCellSet at column 4, rows 2 / 5 / 8 / 0xB) and puts
     actor 0 on row 0;
   - byte 0 of the display record (g_MapPartyRecords, 0x3C stride) of every party
     slot whose id in g_PartyIds is non-zero is cleared;
   - the text target becomes 0x801CCC00 (font_target_set), string
     g_MenuCommands[0] | 0x100 is rendered there (font_string_proportional_put) and
     CellTextDisp puts those cells on screen as a 3-cell strip at column
     4, row 5: its code 0xED88 names that same buffer, 0x801CBB00 + (0x88
     << 5).

   Source shape (what the bytes need): every store is an aggregate MEMBER
   store, and in each stretch the flag clear is written FIRST, the x next.
   Member stores let the scheduler lift the loads of the scalar globals
   (the pointers, g_MapViewX / g_MapViewY, the call's g_MenuPage argument)
   above them; a cast store `*(s16 *)((u8 *)p + N)` does not (-9 to -12
   rows per group). The clear-first order decides where sched1 leaves the
   g_MapViewY load and the g_MenuRenderEntries load, and through those lifetimes
   which of two values local-alloc ranks first: the window pointer in $a1
   and g_MenuRenderEntries in $a3 (the rows' own order, x before the clear, is -10
   and -7). The chained assignments store right to left, which is the
   descending order of the two pairs. */
/* func_80073D58 */
void MenuCardListOpen(void)
{
    s32 i;

    ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
    ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
    AS(u16, ((MapRecord *)g_MenuRecords)[5].sprite.x) = AS(u16, ((MapRecord *)g_MenuRecords)[6].sprite.x) = g_MapViewX + 0x108;
    AS(u16, ((MapRecord *)g_MenuRecords)[5].sprite.y) = g_MapViewY + 0x28;
    AS(u16, ((MapRecord *)g_MenuRecords)[6].sprite.y) = g_MapViewY + 0x90;
    MenuPageDisp((u16) g_MenuPage);
    sprite_run_off(0x87, 8);
    AS(s16, g_MenuRenderEntries[2].on) = 0;
    g_MenuRenderEntries[2].x = g_MapViewX + 0x80;
    g_MenuShapes[2].y2 = g_MenuShapes[2].y3 = 0xF;
    g_MenuRenderEntries[4].y = g_MapViewY + 0x18;
    MenuCommandsSet(0, (u16) g_MenuPage);
    MenuCommandsDisp(0);
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            ((MapRecord *)g_MapPartyRecords)[i].on_kind.on = 0;
        }
    }
    font_target_set((u8 *) 0x801CCC00);
    font_string_proportional_put(g_MenuCommands[0] | 0x100);
    CellTextDisp(4, 5, 3, 1, 3, 0xDEAD2001, 0xED88);
}

/* Puts the card pages away and opens the page it is given, page (0 or 7): blanks the
   four IconCellSet cells, moves the list's two marks in the g_MenuRecords
   record block back (X = g_MapViewX+0x120, Y = g_MapViewY+0x98 / +0xC8,
   flags cleared), gives the party ids back to byte 0 of their records in
   the 0x3C-stride table unless the page is 0x14, and sets and shows the
   new page g_MenuPage = page.

   The six window-block stores are struct-member stores in the target's row
   order (actors 5 and 6 of the 0x3C-byte records: x, y, x, y, then the two
   +4 clears). A member store cannot alias a scalar global, so sched1 lifts
   the g_MapViewX / g_MapViewY loads above the first store, as the target
   does. The same stores as `*(s16 *)((u8 *)p + N)` casts need two hoisted
   temps (`p16E`, `y2`) to pull those loads up (84/94 without them). */
/* func_80073E84 */
void MenuCardListClose(s32 page) {
    s32 place;
    s32 record_offset;
    u16 *party_id;

    IconCellSet(9, 2, 0, 0);
    IconCellSet(0x12, 2, 0, 0);
    IconCellSet(0xC, 2, 0, 0);
    IconCellSet(0xF, 2, 0, 0);
    AS(u16, ((MapRecord *)g_MenuRecords)[5].sprite.x) = g_MapViewX + 0x120;
    AS(u16, ((MapRecord *)g_MenuRecords)[5].sprite.y) = g_MapViewY + 0x98;
    AS(u16, ((MapRecord *)g_MenuRecords)[6].sprite.x) = g_MapViewX + 0x120;
    AS(u16, ((MapRecord *)g_MenuRecords)[6].sprite.y) = g_MapViewY + 0xC8;
    ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
    ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
    sprite_run_off(0x87, 7);
    if (g_MenuPage != 0x14) {
        place = 0;
        record_offset = 0;
        do {
            party_id = (u16 *)(place * 2 + g_PartyIds);
            if (*party_id != 0) {
                (*(u8 *)(record_offset + g_MapPartyRecords)) = (u8) *party_id;
            }
            place += 1;
            record_offset += 0x3C;
        } while (place < 6);
    }
    font_target_clear((u8 *)0x801CBB00, 0x80);
    sprite_run_off(0x87, 8);
    MenuCommandsSet(0, 0);
    MenuCommandsDisp(0);
    g_MenuPage = page & 0xFFFF;
    MenuPageDisp((s32) (u16) g_MenuPage);
    ankprint_rows_clear(0, 0x1C, 0x20);
    g_MenuRedrawRequest = (u16) g_MenuPage;
    g_MenuShapes[2].texel_col.col = 0x761;
    g_MenuShapes->texel_col.col = 0x761;
    g_MenuRenderEntries[2].on = 1;
}
/* The sum of a save (CardSave, card.h): its first 0x7FF 32-bit words, all
   of it up to CardSave.sum at +0x1FFC, read as words, which is why the
   argument is a word pointer and not the save's type. */
/* func_80073FFC */
s32 card_save_sum_get(s32 *save) {
    s32 sum = 0;
    u32 i = 0;
    do {
        sum += *save;
        save += 1;
        i += 1;
    } while (i < 0x7FF);
    return sum;
}
/* far_syms: g_PlayTimeMinutes g_CardPort D_800B8AF0 g_MapCol_blk */
extern s32 g_MenuKeptIds;
extern GameSaveHead *g_GameSaveBlock;
extern u16 g_MapPlaceName;
extern u16 g_PlayTimeMinutes;
extern GameMapRecord *g_GameMapRecord;
extern u16 g_MapPartyKind;
extern u16 g_MapMix;
extern u8 g_PartyCount;
extern MapCol g_MapCol_blk;
extern u8 g_PartyLevelLeast;
extern u8 g_MapObjectsDataNumber;
extern u8 g_ScriptNumber;
extern s32 g_MenuCardBuffer;
extern MenuCardSlotFlags g_MenuCardSlotFlags;
extern s16 g_MenuCardListScroll;
extern s16 g_MenuCardListLast;
extern u8 g_MenuCardListSlots[];
extern s32 g_MenuCardKeptSlot;
extern s32 g_MenuCardKeptPort;
extern u8 D_800E4F80[];
extern s32 D_800B8AF0;
extern GameSaveHead g_MenuCardSlotHeads[];
extern s32 g_MenuNoticeCol8;
s32 MenuAnswerRun();
void MapSpriteDataLoadedMark(void);
s32 MapPartyRingEntryGet(s32, s32 *);
s32 card_save_header_set();
s32 mem_move_32();
s32 MenuNoticeDisp();
s32 card_accept_wait(void);
s32 card_save_write(s32 slot, CardSave *save);
s32 card_result_get(void);
s32 MenuPadWait(void);

/* The save is built as a CardSave (card.h) at the address in
   g_MenuCardBuffer, each part reached through the cast word: the records
   at +0x200, the object flags at +0xE00, the object entries from +0xF80.
   Its sum, CardSave.sum (+0x1FFC), is stored through the member's address
   as a u32 (*(u32 *)&save->sum), which GCC takes as a plain pointer store:
   as the member itself, as AS(u32, save->sum), or as *(s32 *)&save->sum
   (which GCC folds back to the member) it is a member store, and GCC
   orders the stores round it otherwise (53 bytes change; each form
   measured). */
/* func_80074024 */
void MenuSaveWrite(void)
{
    s32 i;
    s32 j;
    s32 slot;
    s32 pos;
    s32 n;
    s32 sum;
    MapObject *src;
    MapObjectSaveEntry *dst;
    MapObject *p;
    s8 *flags;
    CardSave *save;
    u8 pad8[8];     /* MARKED SPELLING (unused local): the target frame (0x30) holds an
                       8-byte aggregate local that no instruction touches -- the
                       original's, contents unknown */

    if (MenuAnswerRun(0x638, 0, 0x19, 0x14, 1) != 1) {
        MapSpriteDataLoadedMark();
        i = 0;
        j = 0;
        do {
            ((u16 *)g_PartyIds)[i] = ((u16 *)g_MenuKeptIds)[i];
            ((MapRecord *)(j + g_MapPartyRecords))->hold_definition.hold = 0;
            ((MapRecord *)(j + g_MapPartyRecords))->sprite.flags &= 0x7FFF;
            i++;
            j += 0x3C;
        } while (i < 6);
        g_GameSaveBlock->play_time_minutes = g_PlayTimeMinutes;
        g_GameSaveBlock->place_name = g_MapPlaceName;
        g_GameSaveBlock->level = ((ChrRecord *)((*(u16 *)g_PartyIds << 6) + g_ChrRecords))->level;
        g_GameMapRecord->map = g_MapNumber;
        g_GameMapRecord->party_kind = g_MapPartyKind;
        g_GameMapRecord->mix = g_MapMix;
        g_GameMapRecord->col = g_MapCol_blk;
        g_GameMapRecord->limit_left = g_MapLimitLeft;
        g_GameMapRecord->limit_right = g_MapLimitRight;
        g_GameMapRecord->limit_top = g_MapLimitTop;
        g_GameMapRecord->limit_bottom = g_MapLimitBottom;
        g_GameMapRecord->party_count = g_PartyCount;
        g_GameMapRecord->party_level_least = g_PartyLevelLeast;
        g_GameMapRecord->objects_data_number = g_MapObjectsDataNumber;
        g_GameMapRecord->script_number = g_ScriptNumber;
        g_GameMapRecord->music_number = g_MusicNumber;
        MapPartyRingEntryGet(0, (s32 *)&g_GameMapRecord->ring_entry);
        mem_clear((void *)g_MenuCardBuffer, sizeof(CardSave));
        for (i = 0; i < 0xF; i++) {
            if (g_MenuCardSlotFlags.flags[i] == 0 && g_MenuCardSlotFlags.flags[i + 0xF] == 1) {
                break;
            }
        }
        slot = i;
        sum = g_MenuCardListRow + g_MenuCardListScroll;
        if (g_MenuCardListLast >= sum || g_MenuCardListLast == 0xF) {
            slot = g_MenuCardListSlots[sum];
        }
        g_MenuCardKeptSlot = slot;
        g_MenuCardKeptPort = g_CardPort;
        card_save_header_set((CardSave *)g_MenuCardBuffer, (u16)slot, g_PlayTimeMinutes, g_MapPlaceName,
                      ((ChrRecord *)((*(u16 *)g_PartyIds << 6) + g_ChrRecords))->level);
        mem_move_32(((CardSave *)g_MenuCardBuffer)->records, g_GameSaveBlock, 0xC00);
        src = (MapObject *)g_MapObjects;
        flags = ((CardSave *)g_MenuCardBuffer)->object_flags;
        dst = ((CardSave *)g_MenuCardBuffer)->object_entries;
        /* CODEGEN FORCING DEVICE (shape), not recovered logic.
           The target runs a pass over the 0x80 records that leaves only its
           counter (counting UP) and a dead pointer stepping 0x60 -- the
           original's body is gone and unknown.  This body reproduces how it
           got that way: the pointer test keeps p live through flow, `n = i`
           keeps loop.c from reversing the counter, flow then deletes the dead
           store (n is reset to 0 before its real use below), and the
           post-reload jump pass deletes the now-empty branch, leaving p's
           increment.  Behaviour-identical: p is never 0 inside the record
           table and n is overwritten. */
        for (i = 0, p = src; i < 0x80; i++, p++) {
            if (p == 0) {
                n = i;
            }
        }
        for (i = 0; i < 0x80; i++, src++) {
            flags[i] = src->record.on_kind.kind;
            if (src->record.on_kind.kind != 0) {
                dst->kind = src->record.on_kind.kind;
                dst->definition = src->record.hold_definition.definition;
                dst->x = AS(u16, src->record.sprite.x);
                dst->depth = AS(u16, src->record.sprite.depth);
                dst->y = AS(u16, src->record.sprite.y);
                dst->animation = src->record.animation;
                dst->number = src->number;
                dst->facing = src->facing;
                dst->speed = src->speed;
                dst->interval = src->interval;
                dst->turn_count = src->turn_count;
                dst->look = src->look;
                dst->sight_width = src->sight_width;
                dst->sight_length = src->sight_length;
                dst->length = src->length;
                dst->count = src->count;
                dst->state = src->state;
                dst->thinks_left = src->thinks_left;
                dst->facing_kept = src->facing_kept;
                dst->turn_plan = src->turn_plan;
                dst->target[0] = AS(u16, src->target.spot.x);
                dst->target[1] = AS(u16, src->target.spot.y);
                dst->move[0] = AS(u16, src->move.bounds.left);
                dst->move[1] = AS(u16, src->move.bounds.right);
                dst->move[2] = AS(u16, src->move.bounds.top);
                dst->move[3] = AS(u16, src->move.bounds.bottom);
                dst->step_list = src->step_list;
                dst++;
            }
        }
        save = (CardSave *)g_MenuCardBuffer;
        *(u32 *)&save->sum = card_save_sum_get((s32 *)save);
        D_800B8AF0 = 0;
        D_800E4F80[0xA] = (g_MenuCardListRow + g_MenuCardListScroll + 1) % 10 + 0x30;
        D_800E4F80[9] = (g_MenuCardListRow + g_MenuCardListScroll + 1) / 10 + 0x30;
        ankprint_rows_clear(0x14, 6, 0x20);
        MenuNoticeDisp(0x683, 0xC, 0x15, 2, 0);
        MenuFrameStep();
        MenuFrameStep();
        if (card_accept_wait() == 0) {
            card_save_write(slot, (CardSave *)g_MenuCardBuffer);
        }
        D_800B8AF0 = 1;
        g_MenuCardSlotHeads[slot] = *(GameSaveHead *)g_GameSaveBlock;
        g_MenuCardSlotFlags.flags[slot] = 1;
        i = 0;
        do {
            ((u16 *)g_MenuKeptIds)[i] = ((u16 *)g_PartyIds)[i];
            if (((u16 *)g_PartyIds)[i] >= 0x1F) {
                ((u16 *)g_PartyIds)[i] = 0;
                ((MapRecord *)g_MapPartyRecords)[i].hold_definition.hold = 1;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.flags |= 0x8000;
            }
            i++;
        } while (i < 6);
        if (card_result_get() != 0) {
            ankprint_rows_clear(0x14, 6, 0x20);
            g_MenuNoticeCol8 = 1;
            MenuNoticeDisp(0x677, 0xC, 0x16, 1, 0);
            SoundPlay(4, 1);
            if (MenuPadWait() != 0) {
                g_MenuPageStage = 0;
                MenuSaveOpen();
            } else {
                MenuCardListClose(7);
                g_MenuPageStage = 0;
            }
        }
        n = 0;
        for (i = 0; i < 0xF; i++) {
            if (i == slot) {
                pos = n;
            }
            if (g_MenuCardSlotFlags.flags[i] == 1) {
                g_MenuCardListSlots[n] = i;
                n++;
            }
        }
        g_MenuCardListLast = n - 1;
        if (g_MenuCardListRow + g_MenuCardListScroll != pos) {
            g_MenuCardListRow = 0;
            g_MenuCardListScroll = 0;
            if (pos != 0) {
                do {
                    if (g_MenuCardListRow < 2) {
                        g_MenuCardListRow++;
                    } else {
                        g_MenuCardListScroll++;
                    }
                } while (g_MenuCardListRow + g_MenuCardListScroll != pos);
            }
        }
    }
}

/* Read the records head of save file slot into buffer. The file's name is
   the template g_CardSaveFileName ("BASLUS-00628LUNAR000" in the EXE image)
   copied into the local with slot's hundreds, tens and ones digits added
   into its last three characters (0x11, 0x12, 0x13), as card_save_read and
   card_save_write build it; card_file_read(name, buffer, offset, 0x80) then
   reads 0x80 bytes at the offset of CardSave.records (0x200) in that file
   from the card in port g_CardPort (MemCardReadFile), the first 0x80 bytes
   of the game's records, and leaves the card status card_command_wait(3) as
   the result: 0 = read (MenuCardListRead skips a slot on 5 and gives up on
   anything else). The buffer holds only those 0x80 bytes, at its start, so
   it is no CardSave. */
/* func_8007473C */
s32 card_save_records_head_read(s32 slot, s32 buffer) {
    u8 buf[0x14];

    strcpy(buf, g_CardSaveFileName);
    buf[0x13] += slot % 10;
    buf[0x12] += (slot / 10) % 10;
    buf[0x11] += (slot / 10 / 10) % 10;
    return card_file_read(buf, buffer, LAYOUT_OFFSET(CardSave, records), 0x80);
}
/* The save is read as a CardSave (card.h) into the fixed buffer at
   0x801A7B00 and its parts reached as members, but for its sum: that is
   read by its own label, g_MenuLoadSum (0x801A9AFC, the save's +0x1FFC).
   Read as the member (save->sum, *(s32 *)&save->sum, *(u32 *)&save->sum or
   ((CardSave *)0x801A7B00)->sum; each measured, each moves the code after
   this function), GCC reaches it from the register that holds the save's
   address for the flags and entries (lw 0x1FFC) and keeps that register
   live across the calls: as save->sum the frame takes one more saved
   register (0x28 becomes 0x30) and the program is 0x10 bytes longer.
   The buffer the save is laid out in (the argument: g_MenuCardBuffer from
   func_8007515C) is no CardSave but the game's records (0xC00 bytes) and
   then an entry for each of the 0x80 object rows, as GameEnter takes it. */
/* func_80074824 */
s32 MenuLoadRead(s32 slot, s32 buffer)
{
    s32 ret;
    s32 i;
    s32 chk;
    CardSave *save;
    s8 *flags;
    MapObjectSaveEntry *src;
    MapObjectSaveEntry *dst;

    save = (CardSave *)0x801A7B00;
    bzero(save, sizeof(CardSave));
    ret = card_save_read(slot, save);
    if (ret == 0) {
        chk = g_MenuLoadSum;
        if (chk == card_save_sum_get((s32 *)save)) {
            *(BlkC00 *)buffer = *(BlkC00 *)save->records;
            flags = save->object_flags;
            src = save->object_entries;
            dst = (MapObjectSaveEntry *)(buffer + 0xC00);
            i = 0;
            do {
                if (flags[i] != 0) {
                    *dst = *src;
                    src++;
                } else {
                    bzero(dst, 0x28);
                }
                i++;
                dst++;
            } while (i < 0x80);
        } else {
            ret = -1;
        }
    }
    return ret;
}

/* Sets the load pages' list up, from the card: redraws the four IconCellSet
   cells (the two ports' icons), clamps the 4-entry cursor g_MenuCardListRow,
   redraws the icons of member g_MenuMemberCursor when g_MenuOnePageRunning is set, then runs
   the card read (MenuCardRead) and MenuCardListScrollAdjust, and sets the
   stage to 2 and the redraw request to the page. */
/* func_800749F4 */
void MenuLoadOpen(void) {
    IconCellSet(9, 2, 0x2B, g_CardPort << 0x18);
    IconCellSet(0x12, 2, 0x49, (g_CardPort ^ 0x10) << 0x18);
    IconCellSet(0xC, 2, 0x7C, 0);
    IconCellSet(0xF, 2, 0xB6, 0);
    if ((u16) g_MenuCardListRow >= 4U) {
        g_MenuCardListRow = 0;
    }
    if (g_MenuOnePageRunning != 0) {
        MenuIconBytesDisp(g_MenuMemberCursor, 1, 1, 0xC);
    }
    MenuCardRead();
    MenuCardListScrollAdjust();
    g_MenuPageStage = 2;
    g_MenuRenderEntries[2].on = 1;
    g_MenuShapes[2].texel_col.col = 0x76A;
    g_MenuRedrawRequest = g_MenuPage;
}
s32 func_8007515C(s32 buffer, s32 slot);  /* TU defines this below */
extern u16 g_MenuKeptPlayTimeMinutes[2];
extern u16 g_MenuPadRecord[];
s32 card_change_check(void);
extern s32 g_MenuBackOutPattern;
extern s32 g_MenuCardResult;
extern s32 g_MenuCardAcceptResult;
extern s32 g_MenuCardFreeBlockCount;
extern s16 g_MenuSoundRequest;
extern u16 g_MenuPadRecordPressed[];

/* load_hazard_nop: MenuLoadStep */
/* func_80074ADC */
void MenuLoadStep(void)
{
    s32 page;
    u16 t;
    u8 *ent;

    if (g_MenuPageStage == 0) {
        MenuLoadOpen();
        g_MenuKeptPlayTimeMinutes[1] = g_PlayTimeMinutes;
    }
    if (g_MenuKeptPlayTimeMinutes[1] != g_PlayTimeMinutes) {
        g_MenuKeptPlayTimeMinutes[1] = g_PlayTimeMinutes;
        g_MenuRedrawRequest = g_MenuPage;
    }
    if (g_MenuPadRecord[2] & 0x1000) {
        if (g_MenuPageStage == 2 && g_MenuCardFreeBlockCount != -1) {
            if (g_MenuCardListRow == 0) {
                if (g_MenuCardListScroll > 0) {
                    g_MenuCardListScroll--;
                }
            } else {
                g_MenuCardListRow--;
            }
            g_MenuSoundRequest = 1;
            g_MenuRedrawRequest = g_MenuPage;
            MenuCardListScrollAdjust();
        }
    } else if (g_MenuPadRecord[2] & 0x4000) {
        if (g_MenuPageStage == 2 && g_MenuCardFreeBlockCount != -1) {
            if (g_MenuCardListRow < 2) {
                g_MenuCardListRow++;
            } else if (g_MenuCardListScroll + 2 < g_MenuCardListLast) {
                g_MenuCardListScroll++;
            } else {
                g_MenuCardListRow = 3;
            }
            g_MenuSoundRequest = 1;
            g_MenuRedrawRequest = g_MenuPage;
            MenuCardListScrollAdjust();
        }
    } else if ((g_MenuPadRecord[1] & 0x110) || (g_MenuBackOutLatch & g_MenuBackOutPattern) ||
               ((g_MenuPadRecord[3] & 0x20) && !(g_MenuPadRecord[0] & 0x20))) {
        if (g_MenuPadRecordPressed[0] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecordPressed[0] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuPageStage = 0;
        if (g_MenuPage == 0x12) {
            if (g_MenuOnePageRunning != 0) {
                MenuCardListClose(0);
            } else {
                MenuCardListClose(7);
            }
        } else {
            MenuIconBytesSet(0, (u16) (0));
            MenuIconBytesDisp(0xFFFF, 1, 2, 0);
            MenuCardListClose(0);
            MenuPageDisp(0xFFFF);
            g_MenuRequest = 0;
        }
        g_MenuSoundRequest = 3;
    } else if (g_MenuPadRecordPressed[0] & 0x40) {
        g_MenuSoundRequest = 2;
        if (g_MenuPageStage == 2) {
            if (g_MenuCardListRow == 3 || g_MenuCardSlotHeads[g_MenuCardListSlots[g_MenuCardListRow + g_MenuCardListScroll]].text[0] != 0) {
                g_MenuShapes->texel_col.col = 0x76A;
                if (MenuAnswerRun(0x636, 0, 0x19, 0x14, 1) != 1) {
                    ankprint_rows_clear(0x14, 6, 0x20);
                    if (g_MenuCardListRow == 3) {
                        goto pick;
                    }
                    MenuNoticeDisp(0x680, 0xC, 0x15, 2, 0);
                    MenuFrameStep();
                    MenuFrameStep();
                    if (card_accept_wait() == 0) {
                        if (func_8007515C(g_MenuCardBuffer, g_MenuCardListSlots[g_MenuCardListRow + g_MenuCardListScroll]) != 0) {
                            ankprint_rows_clear(0x14, 6, 0x20);
                            g_MenuNoticeCol8 = 1;
                            MenuNoticeDisp(0x67D, 0xC, 0x16, 1, 0);
                            SoundPlay(4, 1);
                            if (MenuPadWait() != 0) {
                                MenuLoadOpen();
                            } else {
                                page = 7;
                                if (g_MenuOnePageRunning != 0) {
                                    page = 0;
                                }
                                MenuCardListClose(page);
                                g_MenuPageStage = 0;
                            }
                            goto cancel;
                        }
                    pick:
                        g_MenuCardKeptSlot = g_MenuCardListSlots[g_MenuCardListRow + g_MenuCardListScroll];
                        MenuIconBytesSet(0, (u16) (0));
                        MenuIconBytesDisp(0xFFFF, 1, 2, 0);
                        MenuCardListClose(0);
                        MenuPageDisp(0xFFFF);
                        g_MenuPageStage = 0;
                        g_MenuRequest = g_MenuCardListRow + 1;
                    } else {
                        g_MenuNoticeCol8 = 1;
                        ankprint_rows_clear(0x14, 6, 0x20);
                        MenuNoticeDisp(0x678, 0xC, 0x16, 1, 0);
                        g_MenuPageStage = 0;
                        SoundPlay(4, 1);
                        if (MenuPadWait() != 0) {
                            g_MenuShapes->texel_col.col = 0x761;
                        } else if (g_MenuOnePageRunning != 0) {
                            MenuCardListClose(0);
                        } else {
                            MenuCardListClose(7);
                        }
                    }
                } else {
                cancel:
                    ent = (u8 *)g_MenuShapes;
                    t = g_MenuPage;
                    ((SpritePiece *)ent)->texel_col.col = 0x761;
                    g_MenuRedrawRequest = t;
                }
            }
        } else {
            g_MenuPageStage++;
            MenuCardListScrollAdjust();
            g_MenuRenderEntries[2].on = 1;
            g_MenuRedrawRequest = g_MenuPage;
            g_MenuShapes[2].texel_col.col = 0x76A;
        }
    } else {
        if ((g_MenuPadRecordPressed[0] & 4) && g_CardPort != 0) {
            g_CardPort = 0;
            goto toggled;
        }
        if ((g_MenuPadRecordPressed[0] & 8) && g_CardPort != 0x10) {
            g_CardPort = 0x10;
        toggled:
            g_MenuPageStage = 0;
            card_change_check();
            SoundPlay(1, 1);
        }
    }
    if (g_MenuRedrawRequest == 0x12 || g_MenuRedrawRequest == 0x14) {
        MenuIconBytesSet(0xC, (u16) (g_MenuPage));
        MenuIconBytesDisp(g_MenuMemberCursor, 1, 1, 0xC);
        MenuPageDisp((u16)g_MenuPage);
        MenuFrameStep();
        g_MenuRedrawRequest = 0;
    }
    if (card_change_check() != 0) {
        g_MenuPageStage = 0;
    } else if (g_MenuCardResult == 2 && g_MenuPageStage != 0) {
        g_CardPort ^= 0x10;
    }
    if (g_MenuCardResult == 0 && g_MenuCardAcceptResult != 0 && g_MenuCardAcceptResult != 3) {
        g_MenuPageStage = 0;
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): MenuLoadRead with its arguments turned round and save 0x10 passed over; not a duplicate, and nothing says why. */
s32 func_8007515C(s32 buffer, s32 slot) {
    if (slot == 0x10) {
        return 0;
    }
    return MenuLoadRead(slot, buffer);
}
/* The save/load screen's memory-card scan: MenuCardRead calls it with the
   work buffer g_MenuCardBuffer once card_dir_read() has read the card in port
   g_CardPort (g_MenuCardFreeBlockCount >= 0). Returns the number of saves found.
   The 0x100-byte header table g_MenuCardSlotHeads (sixteen 16-byte entries) and the
   0x20-byte slot table g_MenuCardSlotFlags are cleared. Then, for each of the 15
   save slots, card_save_records_head_read(i, buffer) reads 0x80 bytes at offset 0x200 of
   that slot's card file ("BASLUS-00628LUNAR" + three digits of i) into the
   buffer: result 0 keeps the slot (the buffer's first 16 bytes become
   g_MenuCardSlotHeads[i], g_MenuCardSlotFlags.flags[i] = 1, and the slot number is appended to the
   list g_MenuCardListSlots); result 5 (by this use: no such file) skips it; any
   other result sets g_MenuCardReadFailed = 1, which makes the caller start over, and
   ends the scan. g_MenuCardListLast = the last list index that holds a save
   (count - 1, so -1 on an empty card); a cursor (row g_MenuCardListRow + scroll
   g_MenuCardListScroll) past it is zeroed.
   The second pass builds the selectable half of the slot table,
   g_MenuCardSlotFlags.flags[i + 0xF]: 1 for a slot that holds a save, and 1 for an empty
   slot while fewer than g_MenuCardFreeBlockCount empty ones have been granted (j counts
   them); g_MenuCardSlotFlags.pickable_count (which the cursor code reads) counts
   the 1s. List entries past g_MenuCardListLast are stamped 0xFF, the "no slot"
   value MenuCardListDisp prints as "-".
   Source shapes the codegen pins: the header
   copy indexes the table (a walked pointer is 105/134), the cursor test is
   written sum first (compare first is 125, a $v0/$v1 swap), the buffer is
   a second variable set after the two clears (without it 120), and `j = 0`
   follows the cursor test (before it 131). */
/* func_80075198 */
s32 MenuCardListRead(s32 buffer)
{
    GameSaveHead *src;
    s32 kept;
    s32 i;
    s32 j;
    s32 r;

    mem_clear(g_MenuCardSlotHeads, 0x100);
    mem_clear(&g_MenuCardSlotFlags, 0x20);
    src = (GameSaveHead *)buffer;
    kept = 0;
    for (i = 0; i < 0xF; i++) {
        r = card_save_records_head_read(i, buffer);
        if (r == 0) {
            g_MenuCardSlotHeads[i] = *src;
            g_MenuCardSlotFlags.flags[i] = 1;
            g_MenuCardListSlots[kept++] = i;
        } else if (r != 5) {
            g_MenuCardReadFailed = 1;
            break;
        }
    }
    g_MenuCardListLast = kept - 1;
    if (g_MenuCardListScroll + g_MenuCardListRow > g_MenuCardListLast) {
        g_MenuCardListScroll = 0;
        g_MenuCardListRow = 0;
    }
    j = 0;
    for (i = 0; i < 0xF; i++) {
        g_MenuCardSlotFlags.flags[i + 0xF] = g_MenuCardSlotFlags.flags[i];
        if (g_MenuCardSlotFlags.flags[i] == 1) {
            g_MenuCardSlotFlags.flags[i + 0xF] = 1;
            g_MenuCardSlotFlags.pickable_count++;
        }
        if (g_MenuCardSlotFlags.flags[i] == 0 && j < g_MenuCardFreeBlockCount) {
            g_MenuCardSlotFlags.flags[i + 0xF] = 1;
            j++;
            g_MenuCardSlotFlags.pickable_count++;
        }
        if (g_MenuCardListLast < i) {
            g_MenuCardListSlots[i] = 0xFF;
        }
    }
    return kept;
}

/* Ruling of 2026-10-14 (docs/naming.md): Nothing in the program reaches this function; named for what it does. */
/* func_800753B0 */
s32 card_save_byte_sum_get(u8 *save) {
    s32 sum = 0;
    s32 i = 0x1FFF;
    do {
        sum += *save++;
        i--;
    } while (i >= 0);
    return sum & 0xFFFF;
}
/* func_800753D4 */
s32 card_save_sum_check(CardSave *save) {
    return (save->sum != card_save_sum_get((s32 *)save)) ? -1 : 0;
}
