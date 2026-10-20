/* Lunar SSSC — menu_page: the menu's pages, one frame each, and their own
   displays: commands, items (use, give, drop), spells, worn items, the
   character record, the presets and the battle places' step (whose display,
   open and close are menu_places.c). The first split had it in overworld.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "chr.h"
#include "map.h"
#include "sprite.h"
extern u16 D_800B8EF4[2];
extern u8 g_MenuCommands[8];
extern s32 g_ChrItemNoRoomState;
extern s32 g_SpriteList;
extern s32 g_MenuBackOutLatch;
extern u32 g_MenuBackOutPattern;
extern s32 g_PartyIds;
extern s32 g_MapPartyRecords;
extern u16 g_MapViewX;
extern u16 g_MenuPadRecord[];
extern s32 g_ChrPlacePresetRecords;
extern u16 g_MapViewY;
s32 font_string_proportional_put(s32);
extern u8 g_MenuIconBytes[];
extern s32 g_ChrRecords;
extern s32 g_MenuPage;
extern s32 g_ChrItemRows;
s32 mem_clear(s32 *, s32);
s32 StoryBitCheck(u32);
s32 mem_move(s32 *, s32 *, s32);
extern s32 g_MenuCommandCursor;
extern u16 g_MenuWornItemCursor;
extern u16 g_MenuSpellCursor;
extern u16 g_MenuItemCursor;
extern u16 g_MenuItemListCursor;
extern s32 g_MenuRenderEntries;
extern u16 g_MenuItemListScroll;
extern u16 g_MenuItemCommandCursor;
extern u16 g_MenuSoundRequest;
extern u16 g_MenuMemberCursor;
extern u16 g_MenuRedrawRequest;
extern s32 g_MenuRecords;
extern s32 g_MenuShapes;
extern u16 g_MenuPresetMemberCursor;
extern u16 g_MenuPresetChoiceCursor;
extern u16 g_MenuPresetHalf;
extern u16 g_MenuPresetCursor;
s32 SoundPlay(u16, s32);
s32 sprite_run_off();
s32 CellAllUpdate(s32);
void MenuFrameStep(void);
void MenuPageDisp(s32);
s32 font_string_fixed_put(s32);
s32 font_target_clear(s32, s32);
s32 font_target_set(s32);
void MenuLineUpDisp(u16, s32, s32, s32);
s32 MenuMemberPickRun(s32);
s32 IconCellSet(s32, s32, s32, s32);
void MenuPresetClose(void);
s32 MenuCursorMove(u16, u8 *, s32);
void MenuIconBytesDisp(u16, s32, s32, s32);
void MenuCommandsDisp(s32);
void MenuCommandsSet(s32, u16);
void MapAnimationSet(MapRecord *, u16);
extern const u8 g_MenuMemberPlaces[];
s32 ChrItemUseGet();
s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
s32 MenuTallNumberSet(s32, s32, s32, s32);
s32 MenuTallGlyphSet(s32, s32, s32);
s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
s32 MenuTallNumberSet(s32, s32, s32, s32);
s32 MenuTallGlyphSet(s32, s32, s32);
s32 CellNumberSet(u32, s32, s32, s32, s32, s32, s32, s32);
s32 CellAllUpdate(s32);
extern u8 g_MenuCommands[8];
s32 mem_clear(s32 *, s32);
extern s32 g_ChrItemList0C;
void MenuCursorTextDisp(s32 icon, char col_bits);
/* far_syms: g_MenuMemberPlaces (target reaches it %hi/%lo) */
/* far_syms: g_MenuPadRecord */
s32 SoundPlay(u16, s32);
void MenuMemberCursorMove(u16 *cursor, s32 buttons);  /* extern */
void MenuNumberCompareDisp(s32 column, s32 row, u32 first_number, u32 second_number);  /* extern */
void MenuIconsClear(s32 column, s32 row, s32 column_count, s32 row_count);  /* extern */
void MenuIconBytesSet(s32 chr_id, unsigned short page);  /* extern */
void MenuTextPartsLoad(s32 part_bits);  /* extern */
s32 MenuAnswerRun(s32 string_id, s32 unused, s32 column, s32 row, s32 line_count);  /* extern */
void MenuLineUpMove(s16 *xs, s16 *ys);  /* extern */
void MenuLineUpSpotsMove(void);  /* extern */

/* load_hazard_nop: MenuCommandStep */
/* la_absolute: MenuCommandStep */

/* Menu page 2 (battle_phase_pre_table[2]; splat's name): the member's
   command list - input step and redraw.

   who is the member in the selected party slot
   (*(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds)); the cursor is
   g_MenuCommandCursor over the command bytes g_MenuCommands. A stale close latch
   g_MenuBackOutLatch == 2 is cleared on entry.

   By g_MenuPadRecord[2] (pad repeat), first match wins:
   - 0x5000 (up / down): the cursor steps through MenuCursorMove(cursor,
     g_MenuCommands, bits); SE request 1, redraw.
   - 0xA00C (left / right / L1 / R1): MenuMemberCursorMove(&g_MenuMemberCursor, bits)
     cycles the member; SE request 1, redraw.
   - else g_MenuPadRecord[1] & 0x130 (newly pressed), or the latch g_MenuBackOutLatch &
     g_MenuBackOutPattern & 1: g_MenuBackOutLatch = 1 on 0x100, 2 on 0x20; g_MenuBackOutPattern =
     0x30B30B30; back to page 0 (g_MenuPage = 0, actors 1 and 0 e.h0 = 0,
     MenuCommandsSet(0, 0), MenuCommandsDisp(4), MenuIconBytesSet(0, 0),
     MenuIconBytesDisp(0xFFFF, 1, 2, 0), MenuPageDisp(0xFFFF)); SE request 6.
   - else g_MenuPadRecord[1] & 0x40 (Cross): members 8..10 are refused commands
     1 and 2 (SE request 4). Otherwise SE request 2 and the next page is
     cursor + 7 for member 0xC with the cursor off 1, cursor + 3
     otherwise. A member other than 0xC on command 0 sends actor 0 of
     g_MenuRecords to state 0xB. When the request is 2: actors 1 and 0 e.h0 =
     0, font_target_clear(0x801CD500, 0xC), MenuTextPartsLoad(1), MenuPageDisp(1),
     MenuCommandsSet(who, page), and the new page is marked for redraw.

   Redraw (g_MenuRedrawRequest == 2): who is read again, MenuCommandsSet(who, page),
   MenuCommandsDisp(cursor), MenuIconBytesSet(who, page), MenuIconBytesDisp(slot, 1,
   2, 0xC), then MenuCursorTextDisp(command, bad) with bad = 4 for the refused
   pairs above; actor 0 to state 0xA, g_MenuRedrawRequest = 0, actors 1 and 0 e.h0 =
   1. Every call ends with MenuLineUpDisp(g_MenuMemberCursor, 1, 0, 0).

   No RNG draw of its own.

   Codegen notes: &g_MenuMemberCursor and the sized small array g_MenuCommands[8] are
   the assembler's same-register lui/addiu under la_absolute (197 -> 212;
   an unsized g_MenuCommands[] scores the same 197; an `la` cannot sit in a
   jal's delay slot - both calls keep a nop there); the lhu / sh pairs of
   `g_MenuRedrawRequest = g_MenuPage` need load_hazard_nop (208 -> 212). who is an
   s32 local (205 -> 212): as a u16 the `== 2` constant and the g_MenuBackOutLatch
   load were scheduled above who's lhu and the redraw's range test took
   $s0. */
/* func_8006DA44 */
void MenuCommandStep(void)
{
    s32 who;
    u16 pad;
    s32 r;

    who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
    if (g_MenuBackOutLatch == 2) {
        g_MenuBackOutLatch = 0;
    }
    pad = g_MenuPadRecord[2];
    if (pad & 0x5000) {
        g_MenuCommandCursor = (u16)MenuCursorMove(g_MenuCommandCursor, g_MenuCommands, pad & 0x5000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad & 0xA00C) {
        MenuMemberCursorMove(&g_MenuMemberCursor, pad & 0xA00C);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern & 1)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuPage = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[1].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 0;
        MenuCommandsSet(0, 0);
        MenuCommandsDisp(4);
        MenuIconBytesSet(0, 0);
        MenuIconBytesDisp(0xFFFF, 1, 2, 0);
        MenuPageDisp(0xFFFF);
        g_MenuSoundRequest = 6;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        if ((u32)(who - 8) < 3 && (u32)(g_MenuCommandCursor - 1) < 2) {
            g_MenuSoundRequest = 4;
        } else if (who == 0xC && g_MenuCommandCursor != 1) {
            g_MenuPage = g_MenuCommandCursor + 7;
            g_MenuSoundRequest = 2;
        } else {
            g_MenuSoundRequest = 2;
            g_MenuPage = g_MenuCommandCursor + 3;
        }
        if (who != 0xC && g_MenuCommandCursor == 0) {
            MapAnimationSet((MapRecord *)g_MenuRecords, 0xB);
        }
        if (g_MenuSoundRequest == 2) {
            AS(s16, ((MapRecord *)g_MenuRecords)[1].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 0;
            font_target_clear(0x801CD500, 0xC);
            MenuTextPartsLoad(1);
            MenuPageDisp(1);
            MenuCommandsSet(who, g_MenuPage);
            g_MenuRedrawRequest = g_MenuPage;
        }
    }
    if (g_MenuRedrawRequest == 2) {
        who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
        MenuCommandsSet(who, g_MenuPage);
        MenuCommandsDisp(g_MenuCommandCursor);
        MenuIconBytesSet(who, g_MenuPage);
        MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, 0xC);
        r = 0;
        if ((u32)(who - 8) < 3) {
            r = ((u32)(g_MenuCommandCursor - 1) < 2) << 2;
        }
        MenuCursorTextDisp(g_MenuCommands[g_MenuCommandCursor], r);
        MapAnimationSet((MapRecord *)g_MenuRecords, 0xA);
        g_MenuRedrawRequest = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[1].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 1, 0, 0);
}
/* la_absolute: MenuItemCommandStep */
extern void MenuItemIconBytesDisp(s32, s32, s32);

/* Menu page 4 (battle_phase_pre_table[4]; splat's name): the sub-menu
   behind command 1 of the member's command list (MenuCommandStep sends
   cursor + 3 here) - input step and redraw. Its entries lead to the item
   grid pages 0xB.. (MenuItemUseStep / _01 / _02, whose shared pad step
   MenuItemPadStep backs out to page 4).

   The cursor is g_MenuItemCommandCursor over the entry bytes g_MenuCommands. By
   g_MenuPadRecord[2] (pad repeat), first match wins:
   - 0x5000 (up / down): the cursor steps through MenuCursorMove(cursor,
     g_MenuCommands, bits); SE request 1, redraw.
   - 0xC (L1 / R1): MenuMemberCursorMove(&g_MenuMemberCursor, bits) cycles the member,
     again while the new member is 8, 9 or 10 (the pad word is re-read
     each pass); SE request 2, redraw. (Left / right do nothing here -
     page 2 takes 0xA00C.)
   - else g_MenuPadRecord[1] & 0x130 (newly pressed), or the latch g_MenuBackOutLatch &
     g_MenuBackOutPattern (no `& 1` here): g_MenuBackOutLatch = 1 on 0x100, 2 on 0x20;
     g_MenuBackOutPattern = 0x30B30B30; back to page 2 (g_MenuPage = 2, actors 6, 5,
     3 and 0 e.h0 = 0, MenuPageDisp(2)); SE request 3, redraw of page 2.
   - else g_MenuPadRecord[1] & 0x40 (Cross): entry 0 is REFUSED while event
     flag 0x3D9 is set (StoryBitCheck(0x3D9) != 0 - the flag that turns
     Items off in battle): SE request 4, nothing else, no redraw request.
     Otherwise actors 3 and 0 e.h0 = 0, actor 0 to state 0xB
     (MapAnimationSet), the page becomes cursor + 0xB,
     font_target_clear(0x801CD500, 0x14), MenuTextPartsLoad(3), MenuPageDisp(page),
     ONE FRAME (MenuFrameStep), SE request 2, and the new page is marked
     for redraw.

   Redraw (g_MenuRedrawRequest == 4): who = the member in the selected party slot
   (*(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds)),
   MenuCommandsSet(who, page), MenuCommandsDisp(cursor), MenuItemIconBytesDisp(who,
   who == 0xC, 0x2000), MenuCursorTextDisp(g_MenuCommands[cursor], bad) with bad = 4
   (the dim style) for entry 0 under flag 0x3D9, MenuPageDisp(page), actor
   0 to state 0xA, g_MenuRedrawRequest = 0, actors 3 and 0 e.h0 = 1. Every call ends
   with MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0).

   No RNG draw of its own; the confirm's one frame goes through
   MenuFrameStep -> ... -> frame_end (the vibration driver,
   frame-rng.md draw site 2).

   Codegen notes: the confirm arm is written with the ACCEPTED case as the
   `if` (cursor != 0 || flag clear) and the refusal as its `else` - the
   target falls through into the accepted block and parks the lone
   `g_MenuSoundRequest = 4` store after the shared tail (the other polarity:
   170/178). &g_MenuMemberCursor and the sized g_MenuCommands[8] are the assembler's
   same-register lui/addiu under la_absolute (165/178 without); unlike
   pages 2 and 3 no load_hazard_nop is needed - every `g_MenuRedrawRequest =
   g_MenuPage` here shares one cross-jumped tail whose lhu sits a jump away
   from its sh. MenuPageDisp takes the page as (u16)g_MenuPage (an lhu of
   the s32). who is an s32 and `which` a local set before the first call
   ($s0 across two calls). */
/* func_8006DD94 */
void MenuItemCommandStep(void)
{
    s32 who;
    s32 which;
    s32 r;

    if (g_MenuPadRecord[2] & 0x5000) {
        g_MenuItemCommandCursor = MenuCursorMove(g_MenuItemCommandCursor, g_MenuCommands, g_MenuPadRecord[2] & 0x5000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[2] & 0xC) {
        do {
            MenuMemberCursorMove(&g_MenuMemberCursor, g_MenuPadRecord[2] & 0xC);
            who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
        } while ((u32)(who - 8) < 3);
        g_MenuSoundRequest = 2;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuPage = 2;
        AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 0;
        MenuPageDisp((u16)g_MenuPage);
        g_MenuSoundRequest = 3;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        if (g_MenuItemCommandCursor != 0 || StoryBitCheck(0x3D9) == 0) {
            AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 0;
            MapAnimationSet((MapRecord *)g_MenuRecords, 0xB);
            g_MenuPage = g_MenuItemCommandCursor + 0xB;
            font_target_clear(0x801CD500, 0x14);
            MenuTextPartsLoad(3);
            MenuPageDisp((u16)g_MenuPage);
            MenuFrameStep();
            g_MenuSoundRequest = 2;
            g_MenuRedrawRequest = g_MenuPage;
        } else {
            g_MenuSoundRequest = 4;
        }
    }
    if (g_MenuRedrawRequest == 4) {
        who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
        which = who == 0xC;
        MenuCommandsSet(who, g_MenuPage);
        MenuCommandsDisp(g_MenuItemCommandCursor);
        MenuItemIconBytesDisp(who, which, 0x2000);
        r = 0;
        if (g_MenuItemCommandCursor == 0) {
            r = (StoryBitCheck(0x3D9) != 0) << 2;
        }
        MenuCursorTextDisp(g_MenuCommands[g_MenuItemCommandCursor], r);
        MenuPageDisp((u16)g_MenuPage);
        MapAnimationSet((MapRecord *)g_MenuRecords, 0xA);
        g_MenuRedrawRequest = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
}
/* la_absolute: MenuItemUseStep */
s32 MenuItemPadStep(s32 *chr_id, s32 *on_list);  /* TU defines this below */
s32 ChrItemUseGet();
void MenuUseRun(s32, s32, s32);

/* Menu page 0xB (battle_init_table[0]; splat's name): the item grid's USE
   step and its redraw. Page 4 opens it (page = cursor + 0xB).

   MenuItemPadStep reads the pad and gives who (the member in the selected
   party slot) and which (who == 0xC, the scrolling list); the cursor is
   (&g_MenuItemCursor)[which] over the cells g_MenuIconBytes. When it consumed nothing
   and Cross is newly pressed (g_MenuPadRecord[1] & 0x40): sound 2, then
   r = ChrItemUseGet(who, item, 3), the usable check in mode 3. Two items
   need an event flag as well: 0xA4 (Dragonfly Wing) flag 0x2D2 and 0x90
   (Dragon Wings) flag 0x2D3 - with the flag clear (StoryBitCheck == 0) r
   becomes -1. A negative r is refused with sound 4. Otherwise actor 2 of
   g_MenuRecords goes to state 0xB; r < 5 needs no target (0xC), r >= 5 runs
   the target pick MenuMemberPickRun(0xC). A non-zero target plays and clears
   the pending sound, MenuUseRun(who, item, target) applies the item,
   the page is marked for redraw (g_MenuRedrawRequest = g_MenuPage) and the redraw
   direction becomes 0x8000 (0x2000 otherwise); a cancelled pick (0) does
   none of that. Actor 2 returns to state 0xA either way.

   Redraw (g_MenuRedrawRequest == 0xB): MenuItemIconBytesDisp(who, which, dir), then
   MenuCursorTextDisp(cell, bad) with bad = 4 (dimmed) when ChrItemUseGet(who,
   cell, 3) is -1 or the cell is one of the two flagged items with its
   flag clear, else 0. g_MenuRedrawRequest = 0 and actors 3, 2 and 0 get e.h0 = 1.
   Every call ends with MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0).

   No RNG draw of its own; its frames are the target pick's and whatever
   MenuUseRun runs.

   Codegen notes: `item` is an int (as a u8 it gets a masked copy and a
   fourth saved register, 109/145); r serves the use result, the target
   and the redraw's dim value ($s0 throughout). The redraw test is written
   `!= -1` with the lone `r = 4` in the else arm (142 -> 145): written
   `== -1` first, jump.c hoists `r = 4` above the call instead of
   cross-jumping it with the 0x90 arm's. Both flag arms are plain
   `if (flag == 0) r = 4;` - the 0xA4 arm's sltiu / sll is jump.c's
   store-flag form of it. The cursor array is (&g_MenuItemCursor)[which] under
   la_absolute. */
/* func_8006E05C */
void MenuItemUseStep(void)
{
    s32 who;
    s32 which;
    s32 dir;
    s32 r;
    s32 item;

    dir = 0x2000;
    if (MenuItemPadStep(&who, &which) == 0 && (g_MenuPadRecord[1] & 0x40)) {
        g_MenuSoundRequest = 2;
        item = g_MenuIconBytes[(&g_MenuItemCursor)[which]];
        r = ChrItemUseGet((u16)who, item, 3);
        if (item == 0xA4) {
            if (StoryBitCheck(0x2D2) == 0) {
                r = -1;
            }
        } else if (item == 0x90) {
            if (StoryBitCheck(0x2D3) == 0) {
                r = -1;
            }
        }
        if (r >= 0) {
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xB);
            if (r < 5) {
                r = 0xC;
            } else {
                r = MenuMemberPickRun(0xC);
            }
            if (r != 0) {
                SoundPlay(g_MenuSoundRequest, 1);
                g_MenuSoundRequest = 0;
                MenuUseRun(who, item, r);
                g_MenuRedrawRequest = g_MenuPage;
                dir = 0x8000;
            }
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
        } else {
            g_MenuSoundRequest = 4;
        }
    }
    if (g_MenuRedrawRequest == 0xB) {
        MenuItemIconBytesDisp(who, which, dir);
        item = g_MenuIconBytes[(&g_MenuItemCursor)[which]];
        if (ChrItemUseGet((u16)who, item, 3) != -1) {
            r = 0;
            if (item == 0xA4) {
                if (StoryBitCheck(0x2D2) == 0) {
                    r = 4;
                }
            } else if (item == 0x90) {
                if (StoryBitCheck(0x2D3) == 0) {
                    r = 4;
                }
            }
        } else {
            r = 4;
        }
        MenuCursorTextDisp(g_MenuIconBytes[(&g_MenuItemCursor)[which]], r);
        g_MenuRedrawRequest = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
}
/* la_absolute: MenuItemGiveStep */
extern u8 g_PartyCount;
s32 ChrWornItemRemove(s32 chr_id, s32 worn_place);
s32 ChrItemRemove(s32, s32);
s32 ChrItemGive(s32, s32);

/* Menu page 0xC (battle_init_table[1]; splat's name): the item grid's
   GIVE step - hand the item under the cursor to another member - and its
   redraw. Page 4 opens it (page = cursor + 0xB).

   MenuItemPadStep reads the pad and gives who and which (who == 0xC, the
   scrolling list); the cursor is (&g_MenuItemCursor)[which] over the cells
   g_MenuIconBytes. When it consumed nothing and Cross is newly pressed
   (g_MenuPadRecord[1] & 0x40): sound 2. With fewer than two in the party
   (g_PartyCount < 2) or an empty cell, sound 4 and nothing else. Otherwise
   actor 2 of g_MenuRecords goes to state 0xB and the member pick
   MenuMemberPickRun(who) runs. A non-zero pick takes the item off the giver -
   a member's own cells 0..5 (which == 0, cursor < 6) through
   ChrWornItemRemove(who, cursor + 1), the unequip, anything else through
   ChrItemRemove(who, item) - and adds it to the picked member with
   ChrItemGive(to, item) (whose result is not looked at); the page is
   marked for redraw (g_MenuRedrawRequest = g_MenuPage) and the redraw direction
   becomes 0x8000 (0x2000 otherwise). Actor 2 returns to state 0xA either
   way.

   Redraw (g_MenuRedrawRequest == 0xC): MenuItemIconBytesDisp(who, which, dir),
   MenuCursorTextDisp(cell, 0) - never dimmed - g_MenuRedrawRequest = 0 and actors 0, 2
   and 3 get e.h0 = 1. Every call ends with MenuLineUpDisp(g_MenuMemberCursor, 0, 0,
   cell): this page passes the cell under the cursor as the fourth argument.

   No RNG draw of its own; its frames are the pick's.

   Codegen notes: MenuItemUseStep's shape. `item` is an int read twice into
   the same variable (before the party test and again after the pick; the
   second read goes through the two addresses the first left in $s3 / $s2).
   `to` is an int cast (u16) at ChrItemGive; the cursor array is
   (&g_MenuItemCursor)[which] under la_absolute. */
/* func_8006E2A0 */
void MenuItemGiveStep(void)
{
    s32 who;
    s32 which;
    s32 dir;
    s32 item;
    s32 to;

    dir = 0x2000;
    if (MenuItemPadStep(&who, &which) == 0 && (g_MenuPadRecord[1] & 0x40)) {
        g_MenuSoundRequest = 2;
        item = g_MenuIconBytes[(&g_MenuItemCursor)[which]];
        if (g_PartyCount >= 2 && item != 0) {
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xB);
            to = MenuMemberPickRun(who);
            if (to != 0) {
                item = g_MenuIconBytes[(&g_MenuItemCursor)[which]];
                if (which == 0 && g_MenuItemCursor < 6) {
                    ChrWornItemRemove((u16)who, (u16)(g_MenuItemCursor + 1));
                } else {
                    ChrItemRemove((u16)who, item);
                }
                ChrItemGive((u16)to, item);
                g_MenuRedrawRequest = g_MenuPage;
                dir = 0x8000;
            }
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
        } else {
            g_MenuSoundRequest = 4;
        }
    }
    if (g_MenuRedrawRequest == 0xC) {
        MenuItemIconBytesDisp(who, which, dir);
        MenuCursorTextDisp(g_MenuIconBytes[(&g_MenuItemCursor)[which]], 0);
        g_MenuRedrawRequest = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, g_MenuIconBytes[(&g_MenuItemCursor)[which]]);
}

/* la_absolute: MenuItemDropStep */
/* load_hazard_nop: MenuItemDropStep */
extern s32 g_ChrItemNoRoomItem;
s32 ChrItemWaitingGive(void);
s32 MenuItemPadStep(s32 *chr_id, s32 *on_list);  /* TU defines this below */
extern void MenuItemIconBytesDisp(s32, s32, s32);
void MenuLineUpDisp(u16, s32, s32, s32);
s32 ChrItemUseGet();
s32 ChrWornItemRemove(s32 chr_id, s32 worn_place);
s32 ChrItemRemove(s32, s32);
s32 ChrItemGive(s32, s32);
s32 StoryBitClear(s32);

/* Menu pages 0xD and 0x16 (battle_phase_pre_table[0xD] and [0x16]; splat's
   name): the item grid's confirm step and its redraw.

   Entry, page 0x16 only: while g_ChrItemNoRoomState < 2, MenuCommandsSet(0, 0x16),
   MenuCommandsDisp(0) and g_ChrItemNoRoomState += 2 (the pad step's back-out takes the
   2 off again).

   MenuItemPadStep reads the pad and gives who (the member in the selected
   party slot) and which (who == 0xC, the scrolling list); the cursor is
   (&g_MenuItemCursor)[which] over the cells g_MenuIconBytes. When it consumed nothing
   and Cross is newly pressed (g_MenuPadRecord[1] & 0x40): sound 2, then
   ChrItemUseGet(who, cell, 2). A negative result, or a member's own cells
   0..5 on page 0x16, is refused with sound 4. Otherwise actor 2 of
   g_MenuRecords goes to state 0xB and the two-choice prompt
   MenuAnswerRun(0x65F, 0x801CC380, 0xC, 0xE, 2) runs; on 0:
   - a member's cells 0..5 (which == 0, cursor < 6): ChrWornItemRemove(who,
     cursor + 1), and the cursor steps through MenuCursorMove(cursor,
     g_MenuIconBytes, 0x2000);
   - anything else: ChrItemRemove(who, cell), and on page 0x16 either
     (g_ChrItemNoRoomState == 2) ChrItemGive(0, (u8)g_ChrItemNoRoomItem), g_ChrItemNoRoomItem = 0,
     StoryBitClear(0x2D6) and sound 0xC, or g_ChrItemNoRoomState = 2 when
     ChrItemWaitingGive() is non-zero;
   and the redraw direction becomes 0x8000 (0x2000 otherwise). Actor 2
   returns to state 0xA and the page is marked for redraw (g_MenuRedrawRequest =
   g_MenuPage) whatever the prompt returned.

   Redraw (g_MenuRedrawRequest is 0xD or 0x16): MenuItemIconBytesDisp(who, which, dir), then
   MenuCursorTextDisp(cell, bad) with bad = 4 unless ChrItemUseGet(who, cell, 2)
   is not -1 and (the redraw is not 0x16, or who == 0xC, or the cursor is
   >= 6). Actor 0's e.h0 = 1 except on 0x16, g_MenuRedrawRequest = 0, actors 3 and 2
   e.h0 = 1. Every call ends with MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0).

   No RNG draw of its own; its frames are the prompt's (MenuAnswerRun).

   Codegen notes: the cursor array is (&g_MenuItemCursor)[n] under la_absolute
   (202 -> 219: the three `la`s are lui/addiu pairs, one of them held in
   $s1 across the second ChrItemUseGet); the lhu / sh pair of
   `g_MenuRedrawRequest = g_MenuPage` needs load_hazard_nop (213 -> 219). who is an
   s32 passed as (u16) to ChrItemUseGet, ChrWornItemRemove and ChrItemRemove
   (lhu from the slot) and whole to MenuItemIconBytesDisp (lw); the (u8) on
   g_ChrItemNoRoomItem makes its read an lbu (218 without). `bad = 4` follows the
   MenuItemIconBytesDisp call (before it: 211); one `dir = 0x8000` closes the
   prompt's block - the compiler copies it into three delay slots and
   parks the constant 2 of `g_ChrItemNoRoomState == 2` in dir's dead $s0. */
/* func_8006E4A0 */
void MenuItemDropStep(void)
{
    s32 who;
    s32 which;
    s32 dir;
    s8 bad;

    if (g_MenuPage == 0x16 && g_ChrItemNoRoomState < 2) {
        MenuCommandsSet(0, 0x16);
        MenuCommandsDisp(0);
        g_ChrItemNoRoomState += 2;
    }
    dir = 0x2000;
    if (MenuItemPadStep(&who, &which) == 0 && (g_MenuPadRecord[1] & 0x40)) {
        g_MenuSoundRequest = 2;
        if (ChrItemUseGet((u16)who, g_MenuIconBytes[(&g_MenuItemCursor)[which]], 2) >= 0
            && (which != 0 || g_MenuItemCursor >= 6 || g_MenuPage != 0x16)) {
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xB);
            if (MenuAnswerRun(0x65F, 0x801CC380, 0xC, 0xE, 2) == 0) {
                if (which == 0 && g_MenuItemCursor < 6) {
                    ChrWornItemRemove((u16)who, (u16)(g_MenuItemCursor + 1));
                    g_MenuItemCursor = MenuCursorMove(g_MenuItemCursor, g_MenuIconBytes, 0x2000);
                } else {
                    ChrItemRemove((u16)who, g_MenuIconBytes[(&g_MenuItemCursor)[which]]);
                    if (g_MenuPage == 0x16) {
                        if (g_ChrItemNoRoomState == 2) {
                            ChrItemGive(0, (u8)g_ChrItemNoRoomItem);
                            g_ChrItemNoRoomItem = 0;
                            StoryBitClear(0x2D6);
                            g_MenuSoundRequest = 0xC;
                        } else if (ChrItemWaitingGive() != 0) {
                            g_ChrItemNoRoomState = 2;
                        }
                    }
                }
                dir = 0x8000;
            }
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
            g_MenuRedrawRequest = g_MenuPage;
        } else {
            g_MenuSoundRequest = 4;
        }
    }
    if (g_MenuRedrawRequest == 0xD || g_MenuRedrawRequest == 0x16) {
        MenuItemIconBytesDisp(who, which, dir);
        bad = 4;
        if (ChrItemUseGet((u16)who, g_MenuIconBytes[(&g_MenuItemCursor)[which]], 2) != -1) {
            if (g_MenuRedrawRequest != 0x16 || who == 0xC || (&g_MenuItemCursor)[which] >= 6) {
                bad = 0;
            }
        }
        MenuCursorTextDisp(g_MenuIconBytes[(&g_MenuItemCursor)[which]], bad);
        if (g_MenuRedrawRequest != 0x16) {
            AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 1;
        }
        g_MenuRedrawRequest = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
}

/* la_absolute: MenuItemPadStep */
/* load_hazard_nop: MenuItemPadStep */
extern s32 g_ChrItemWaitingList;

/* The item grid's shared pad step, called first thing by the three menu
   page handlers MenuItemUseStep / _01 / _02 (splat's names). Returns 1 when
   it consumed the input, 0 when the caller should go on (confirm etc.).

   Out: *chr_id = who, the member in the selected party slot
   (g_PartyIds[g_MenuMemberPlaces[g_MenuMemberCursor]], u16); *on_list = which = (who == 0xC).
   Member 0xC is the scrolling list (MenuIconBytesSet fills its page from the
   six-byte rows of g_ChrItemList0C at scroll g_MenuItemListScroll); every other member
   shows one page of twelve. g_MenuIconBytes is the page's twelve cells (two rows
   of six, 0 = empty) and (&g_MenuItemCursor)[which] the cursor in it (g_MenuItemCursor
   for a member, g_MenuItemListCursor for the list).

   By g_MenuPadRecord[2] (pad, standard bit layout), first match wins:
   - 0xA000 left / right: unless the cursor is 0 on an empty cell,
     MenuCursorMove steps it one column (wrapping in its row of six) until
     it lands on a non-empty cell; redraw (g_MenuRedrawRequest = the page
     g_MenuPage). Sound 1 either way.
   - 0x1000 up: cursor >= 6 moves up a row; otherwise, in the list only, the
     scroll g_MenuItemListScroll goes down by one while it is non-zero. Redraw on a
     change; sound 1 either way.
   - 0x4000 down: cursor < 6 moves down a row when the second row's first
     cell (g_MenuIconBytes[6]) is non-empty; otherwise, in the list only, the
     scroll goes up by one while it is < 0xF and the row two below it
     (g_ChrItemList0C + scroll * 6 + 0xC) is non-empty. Redraw on a change;
     sound 1 either way.
   - 0xC (L1 / R1): MenuMemberCursorMove steps the party slot g_MenuMemberCursor, again
     while who comes out 8, 9 or 10; *chr_id and *on_list are rewritten; sound 2,
     redraw.
   - else g_MenuPadRecord[1] & 0x130 (newly pressed), or the latch g_MenuBackOutLatch &
     g_MenuBackOutPattern: back out. g_MenuBackOutLatch = 1 for bit 0x100, 2 for bit 0x20,
     g_MenuBackOutPattern = 0x30B30B30, actors 3, 2 and 0 of g_MenuRecords get e.h0 = 0.
     On page 0x16: with g_ChrItemNoRoomState == 3 only sound 4 + redraw (return 1
     at once); otherwise actors 6 and 5 too, g_ChrItemNoRoomState -= 2, eight bytes
     at g_ChrItemWaitingList cleared, MenuCommandsSet(0, 0), MenuCommandsDisp(0),
     MenuIconBytesSet(0, 0), MenuIconBytesDisp(0xFFFF, 1, 2, 0),
     MenuPageDisp(0xFFFF), and the page becomes 0. On any other page the
     page becomes 4. Sound 3, g_MenuRedrawRequest = the new page.

   No RNG draw and no frame wait of its own; the only callees that can run
   frames are the page-0x16 back-out's MenuPageDisp (-> MenuCardResultDisp ->
   MenuNoticeDisp -> MenuFrameStep) and MenuIconBytesDisp (a portrait load).

   Codegen notes: g_MenuMemberPlaces must be `const` - it sits in .rodata, and an
   unchanging read does not depend on the prologue's register saves in
   sched2 (sched.c true_dependence), which is what lets four body
   instructions sit above `addiu sp` at the entry (242 -> 248; nothing in
   the body's C moves it). `ret = 1` precedes `*on_list = ...` in the L1/R1
   arm: reload_cse then compares with the register (`sltu v0,v0,a2`).
   The cursor array is (&g_MenuItemCursor)[n] under la_absolute, and the
   lhu / sh pair of the left/right arm needs load_hazard_nop. */
/* func_8006E80C */
s32 MenuItemPadStep(s32 *chr_id, s32 *on_list)
{
    s32 ret;

    ret = 0;
    *chr_id = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
    *on_list = *chr_id == 0xC;
    if (g_MenuPadRecord[2] & 0xA000) {
        if ((&g_MenuItemCursor)[*on_list] != 0 || g_MenuIconBytes[(&g_MenuItemCursor)[*on_list]] != 0) {
            do {
                (&g_MenuItemCursor)[*on_list] = MenuCursorMove((&g_MenuItemCursor)[*on_list], g_MenuIconBytes, g_MenuPadRecord[2] & 0xA000);
            } while (g_MenuIconBytes[(&g_MenuItemCursor)[*on_list]] == 0);
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
        ret = 1;
    } else if (g_MenuPadRecord[2] & 0x1000) {
        if ((&g_MenuItemCursor)[*on_list] >= 6) {
            (&g_MenuItemCursor)[*on_list] -= 6;
            g_MenuRedrawRequest = g_MenuPage;
        } else if (*on_list == 1) {
            if (g_MenuItemListScroll != 0) {
                g_MenuItemListScroll--;
                g_MenuRedrawRequest = g_MenuPage;
            }
        }
        g_MenuSoundRequest = 1;
        ret = 1;
    } else if (g_MenuPadRecord[2] & 0x4000) {
        if ((&g_MenuItemCursor)[*on_list] < 6) {
            if (g_MenuIconBytes[6] != 0) {
                (&g_MenuItemCursor)[*on_list] += 6;
                g_MenuRedrawRequest = g_MenuPage;
            }
        } else if (*on_list == 1) {
            if (g_MenuItemListScroll < 0xF) {
                if (*(u8 *)(g_ChrItemList0C + g_MenuItemListScroll * 6 + 0xC) != 0) {
                    g_MenuItemListScroll++;
                    g_MenuRedrawRequest = g_MenuPage;
                }
            }
        }
        g_MenuSoundRequest = 1;
        ret = 1;
    } else if (g_MenuPadRecord[2] & 0xC) {
        do {
            MenuMemberCursorMove(&g_MenuMemberCursor, g_MenuPadRecord[2] & 0xC);
            *chr_id = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
        } while ((u32)(*chr_id - 8) < 3);
        ret = 1;
        *on_list = *chr_id == 0xC;
        g_MenuSoundRequest = 2;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 0;
        if (g_MenuPage == 0x16) {
            if (g_ChrItemNoRoomState == 3) {
                g_MenuSoundRequest = 4;
                g_MenuRedrawRequest = g_MenuPage;
                return 1;
            }
            AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
            g_ChrItemNoRoomState -= 2;
            mem_clear((s32 *)g_ChrItemWaitingList, 8);
            MenuCommandsSet(0, 0);
            MenuCommandsDisp(0);
            MenuIconBytesSet(0, 0);
            MenuIconBytesDisp(0xFFFF, 1, 2, 0);
            MenuPageDisp(0xFFFF);
            g_MenuPage = 0;
        } else {
            g_MenuPage = 4;
        }
        ret = 1;
        g_MenuSoundRequest = 3;
        g_MenuRedrawRequest = g_MenuPage;
    }
    return ret;
}
/* la_absolute: MenuItemIconBytesDisp */

/* MenuItemIconBytesDisp -- the item grid's page refresh: it loads the page
   of twelve cells, sets the two scroll marks, repairs a cursor that sits on
   an empty cell, and draws the grid. Callers: MenuItemCommandStep (who,
   who == 0xC, 0x2000), MenuItemUseStep / _01 / _02 (who, which, dir) and the
   battle's MenuUseMoviePlay (g_MenuUseUserId, id == 0xC, 0).

   who   = the member (0xC = the scrolling list), which = (who == 0xC) as the
           callers pass it -- it picks the page source AND the cursor
           (&g_MenuItemCursor)[which]; dir = the pad direction bits handed to
           MenuCursorMove for the cursor repair.

   1. Actors 5 and 6 of g_MenuRecords (the scroll marks) go to x = g_MapViewX +
      0x120, y = g_MapViewY + 0x98 / + 0xC8; MenuPageDisp(page).
   2. The page source p: which == 0 -> the member's twelve bytes at +0x1E of
      its 64-byte record (g_ChrRecords); otherwise the list's six-byte rows,
      g_ChrItemList0C + scroll * 6 (scroll = g_MenuItemListScroll). While the page's SECOND
      row is empty (p[6] == 0) and scroll != 0 the list scrolls back a row
      (scroll--, p -= 6) and a list cursor still in the top row
      (g_MenuItemListCursor < 6) moves down with its item (+= 6).
   3. Both marks off; for who == 0xC actor 5 (more above) is on when scroll
      != 0, and actor 6 (more below) when scroll < 0xF and the row under
      the page (p[0xC]) is not empty.
   4. MenuIconBytesSet(who, page), then the twelve bytes at p are copied to
      g_MenuIconBytes (mem_move) -- whatever MenuIconBytesSet put there.
   5. Cursor repair, only when the cell under (&g_MenuItemCursor)[which] is empty:
      cur = MenuCursorMove(cursor, cells, dir). Still empty -> for the list,
      cur -= 6 when it is in the second row; for a member, the OTHER row is
      tried through the member cursor g_MenuItemCursor: from the top row
      MenuCursorMove(g_MenuItemCursor + 6, ...) and 0 if that cell is empty too,
      from the second row MenuCursorMove(g_MenuItemCursor - 6, ...) unchecked. The
      result is stored back.
   6. MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, cursor) draws the grid -- with cell
      0xC (off the grid: no cursor) on page 4.

   No RNG draw and no frame wait of its own; MenuPageDisp and
   MenuIconBytesDisp (a portrait load) can run frames.

   Shape: OLD-STYLE definition with `u16 dir` -- the callers pass the word
   whole (MenuItemDropStep: `lw`) and the callee narrows it: s3 keeps the raw
   argument and the single `andi $s0, $s3, 0xFFFF` at the first
   MenuCursorMove call is the HImode parameter's promotion, reused by the
   other two calls. A prototyped `u16 dir` gives the same bytes but would
   change the callers' prototype; an `s32 dir` masked by hand scores 152
   (the prologue's `sw s3` / `move s3, a2` come out four rows early and the
   actor base lands in $a2). cur is ONE s32 variable, also the fourth
   argument of MenuIconBytesDisp, which is what puts it in $a3 throughout (a
   `?:` in the argument list leaves it in $a0: 137). The cursor array is
   (&g_MenuItemCursor)[which] under la_absolute; the member-row arms read the
   scalar g_MenuItemCursor (%gp_rel, re-read after the test). */
/* func_8006EBEC */
void MenuItemIconBytesDisp(who, which, dir)
s32 who;
s32 which;
u16 dir;
{
    u8 *p;
    s32 cur;

    ((MapRecord *)g_MenuRecords)[5].sprite.x = g_MapViewX + 0x120;
    ((MapRecord *)g_MenuRecords)[5].sprite.y = g_MapViewY + 0x98;
    ((MapRecord *)g_MenuRecords)[6].sprite.x = g_MapViewX + 0x120;
    ((MapRecord *)g_MenuRecords)[6].sprite.y = g_MapViewY + 0xC8;
    MenuPageDisp((u16)g_MenuPage);
    if (which == 0) {
        p = (u8 *)(g_ChrRecords + (who << 6) + 0x1E);
    } else {
        p = (u8 *)(g_ChrItemList0C + g_MenuItemListScroll * 6);
        while (p[6] == 0 && g_MenuItemListScroll != 0) {
            g_MenuItemListScroll--;
            p -= 6;
            if (g_MenuItemListCursor < 6) {
                g_MenuItemListCursor += 6;
            }
        }
    }
    AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
    AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
    if (who == 0xC) {
        if (g_MenuItemListScroll != 0) {
            AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 1;
        }
        if (g_MenuItemListScroll < 0xF && p[0xC] != 0) {
            AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 1;
        }
    }
    MenuIconBytesSet(who, g_MenuPage);
    mem_move((s32 *)g_MenuIconBytes, (s32 *)p, 0xC);
    if (g_MenuIconBytes[(&g_MenuItemCursor)[which]] == 0) {
        cur = MenuCursorMove((&g_MenuItemCursor)[which], g_MenuIconBytes, dir) & 0xFFFF;
        if (g_MenuIconBytes[cur] == 0) {
            if (which == 0) {
                if (g_MenuItemCursor < 6) {
                    cur = g_MenuItemCursor + 6;
                    cur = MenuCursorMove(cur, g_MenuIconBytes, dir) & 0xFFFF;
                    if (g_MenuIconBytes[cur] == 0) {
                        cur = 0;
                    }
                } else {
                    cur = g_MenuItemCursor - 6;
                    cur = MenuCursorMove(cur, g_MenuIconBytes, dir) & 0xFFFF;
                }
            } else if (cur >= 6) {
                cur -= 6;
            }
        }
        (&g_MenuItemCursor)[which] = cur;
    }
    if (g_MenuPage == 4) {
        cur = 0xC;
    } else {
        cur = (&g_MenuItemCursor)[which];
    }
    MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, cur);
}

/* load_hazard_nop: MenuSpellStep */
/* la_absolute: MenuSpellStep */
extern s32 g_ChrSpellRows[];
void MenuPageDisp(s32);
s32 ChrSpellUseGet(s32, s32, s32);
s32 StoryBitCheck(u32);
s32 MenuMemberPickRun(s32);
void MenuUseRun(s32, s32, s32);
void MenuLineUpDisp(u16, s32, s32, s32);

/* func_8006EE68 */
void MenuSpellStep(void)
{
    u16 who;
    u16 pad;
    s32 r;
    u8 item;
    s32 val;
    u16 cur;
    s32 idx;
    s32 off;

    who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
    pad = g_MenuPadRecord[2];
    if (pad & 0xA000) {
        g_MenuSpellCursor = MenuCursorMove(g_MenuSpellCursor, g_MenuIconBytes, pad & 0xA000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad & 0x1000) {
        if (g_MenuSpellCursor >= 6) {
            g_MenuSpellCursor -= 6;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if (pad & 0x4000) {
        if (g_MenuSpellCursor < 6) {
            g_MenuSpellCursor += 6;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if (pad & 0xC) {
        do {
            MenuMemberCursorMove(&g_MenuMemberCursor, g_MenuPadRecord[2] & 0xC);
            who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
        } while (who == 0xC);
        g_MenuSoundRequest = 2;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 0;
        MapAnimationSet((MapRecord *)g_MenuRecords, 0xA);
        g_MenuPage = 2;
        MenuPageDisp((u16)g_MenuPage);
        g_MenuSoundRequest = 3;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        g_MenuSoundRequest = 2;
        item = g_MenuIconBytes[g_MenuSpellCursor];
        if (item != 0) {
            item += 0x4A;
        }
        r = ChrSpellUseGet(who, item, 2);
        if (g_MenuIconBytes[g_MenuSpellCursor] == 0xCA && StoryBitCheck(0x2D2) == 0) {
            r = -1;
        }
        if (r >= 0) {
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xB);
            if (r < 5) {
                r = 0xC;
            } else {
                r = MenuMemberPickRun(0xC);
            }
            if (r != 0) {
                SoundPlay(g_MenuSoundRequest, 1);
                g_MenuSoundRequest = 0;
                MenuUseRun(who, g_MenuIconBytes[g_MenuSpellCursor], r);
                g_MenuRedrawRequest = g_MenuPage;
            }
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
        } else {
            g_MenuSoundRequest = 4;
        }
    }
    if (g_MenuRedrawRequest == 3) {
        MenuIconBytesSet(who, g_MenuPage);
        if (g_MenuIconBytes[g_MenuSpellCursor] == 0) {
            idx = MenuCursorMove(g_MenuSpellCursor, g_MenuIconBytes, 0x8000);
            if (g_MenuIconBytes[(u16)idx] == 0) {
                cur = g_MenuSpellCursor;
                if (cur < 6) {
                    idx = cur + 6;
                } else {
                    idx = cur - 6;
                }
                if (g_MenuIconBytes[(u16)idx] == 0) {
                    idx = MenuCursorMove(idx, g_MenuIconBytes, 0x8000);
                    if (g_MenuIconBytes[(u16)idx] == 0) {
                        idx = 0;
                    }
                }
            }
            g_MenuSpellCursor = idx;
        }
        MenuPageDisp((u16)g_MenuPage);
        item = g_MenuIconBytes[g_MenuSpellCursor];
        if (item != 0) {
            item += 0x4A;
            CellTextDisp(0x1D, 0x15, 4, 1, 4, 0xDEAD2001, 0xED08);
            off = item * 12;
            val = ((u8 *)g_ChrSpellRows[0])[off - 8];
            if (*(u8 *)((who << 6) + g_ChrRecords + 0x22) == 0x88 || *(u8 *)((who << 6) + g_ChrRecords + 0x23) == 0x88) {
                val = (val + 1) >> 1;
            }
            MenuTallNumberSet(val, 0x1E, 0x17, 3);
            MenuTallGlyphSet(0xE, 0x21, 0x17);
            idx = 0x3000;
            if (*(u16 *)((who << 6) + g_ChrRecords + 4) == *(u16 *)((who << 6) + g_ChrRecords + 6)) {
                idx = 0x2000;
            }
            CellNumberSet(*(u16 *)((who << 6) + g_ChrRecords + 4), 0x22, 0x17, 3, 0xDEAD2001, (u16)idx, 1, 0);
        }
        MenuFrameStep();
        MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, g_MenuSpellCursor);
        if (item != 0) {
            MenuTallNumberSet(val, 0x1E, 0x17, 3);
        }
        r = ChrSpellUseGet(who, item, 2);
        if (r != -1) {
            r = 0;
        } else {
            r = 4;
        }
        if (r == 0 && g_MenuIconBytes[g_MenuSpellCursor] == 0xCA && StoryBitCheck(0x2D2) == 0) {
            r = 4;
        }
        MenuCursorTextDisp(g_MenuIconBytes[g_MenuSpellCursor], (u8)r);
        g_MenuRedrawRequest = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[0].sprite.on) = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
}
/* la_absolute: MenuWornItemStep */
extern u16 g_MenuWornItemOpen;

/* Menu page 5 (battle_phase_pre_table[5]): the member's GEAR page - input
   step and redraw; MenuCommandStep sends command cursor + 3 here.

   First call (g_MenuWornItemOpen == 0): the two tab labels are printed -
   font_target_clear(0x801CF700, 5) clears, font_string_proportional_put(0x687) prints string
   0x687 "Gear", font_target_set(0x801CF880) selects the next buffer,
   font_string_proportional_put(0x1EC) prints "Items" - and g_MenuWornItemOpen = 1.

   The cursor is g_MenuWornItemCursor over a 2 x 6 grid. By g_MenuPadRecord[2] (pad
   repeat), first match wins:
   - 0xA000 (left / right): MenuCursorMove(cursor, 0, bits) - a NULL list,
     so no cell is skipped; SE request 1, redraw.
   - 0x1000 (up): cursor >= 6 goes up a row and redraws; SE request 1
     either way.
   - 0x4000 (down): cursor < 6 goes down a row and redraws; SE request 1
     either way.
   - 0xC (L1 / R1): MenuMemberCursorMove(&g_MenuMemberCursor, bits) cycles the member,
     again while the new member is 8, 9, 10 or 0xC (the pad word is re-read
     each pass); SE request 2, redraw.
   - else g_MenuPadRecord[1] & 0x130 (newly pressed), or the latch g_MenuBackOutLatch &
     g_MenuBackOutPattern: g_MenuBackOutLatch = 1 on 0x100, 2 on 0x20; g_MenuBackOutPattern =
     0x30B30B30; actors 3 and 2 e.h0 = 0; MenuIconBytesSet(member, 0);
     MenuIconBytesDisp(slot, 1, 2, 0xC); sprite_run_off(0x87, 8) frees sprite
     slots 0x87-0x8E; back to page 2 (MenuPageDisp(2)); g_MenuWornItemOpen = 0 (the
     labels are printed again next time); SE request 3, redraw of page 2.
   - else g_MenuPadRecord[1] & 0x40 (Cross): MenuWornItemChange() (asm) - the pick.

   Redraw (g_MenuRedrawRequest == 5): MenuWornItemDisp() (asm) draws the page,
   g_MenuRedrawRequest = 0, actors 3 and 2 e.h0 = 1. Every call ends with
   MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0).

   No frame wait and no RNG draw of its own.

   Codegen notes: the member id is read as an ARRAY element,
   ((u16 *)g_PartyIds)[g_MenuMemberPlaces[slot]] - a pointer-plus-index
   INDIRECT_REF is MEM_IN_STRUCT, so the load cannot alias the scalar store
   to g_MenuBackOutPattern and sched1 is free to leave that store last before the
   call; the 0x30B30B30 pseudo then lives across the argument registers and
   takes $a2. Spelled *(u16 *)(g_MenuMemberPlaces[slot] * 2 + g_PartyIds) (an
   integer sum cast to a pointer, the TU's other pages) the load depends on
   the store, the store is scheduled first and the constant gets $v1:
   162/165. The pad record is read as g_MenuPadRecord[n] at every use (cse keeps
   the base in $a0, loop.c's copy in $s1 for the L1/R1 loop). &g_MenuMemberCursor
   is the assembler's same-register lui/addiu under la_absolute. */
/* func_8006F408 */
void MenuWornItemStep(void)
{
    s32 who;

    if (g_MenuWornItemOpen == 0) {
        font_target_clear(0x801CF700, 5);
        font_string_proportional_put(0x687);
        font_target_set(0x801CF880);
        font_string_proportional_put(0x1EC);
        g_MenuWornItemOpen = 1;
    }
    if (g_MenuPadRecord[2] & 0xA000) {
        g_MenuWornItemCursor = MenuCursorMove(g_MenuWornItemCursor, 0, g_MenuPadRecord[2] & 0xA000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[2] & 0x1000) {
        if (g_MenuWornItemCursor >= 6) {
            g_MenuWornItemCursor -= 6;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if (g_MenuPadRecord[2] & 0x4000) {
        if (g_MenuWornItemCursor < 6) {
            g_MenuWornItemCursor += 6;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if (g_MenuPadRecord[2] & 0xC) {
        do {
            MenuMemberCursorMove(&g_MenuMemberCursor, g_MenuPadRecord[2] & 0xC);
            who = ((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]];
        } while ((u32)(who - 8) < 3 || who == 0xC);
        g_MenuSoundRequest = 2;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 0;
        MenuIconBytesSet(((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]], 0);
        MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, 0xC);
        sprite_run_off(0x87, 8);
        g_MenuPage = 2;
        MenuPageDisp((u16)g_MenuPage);
        g_MenuWornItemOpen = 0;
        g_MenuSoundRequest = 3;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        MenuWornItemChange();
    }
    if (g_MenuRedrawRequest == 5) {
        MenuWornItemDisp();
        g_MenuRedrawRequest = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[3].sprite.on) = 1;
        AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 1;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 0, 0, 0);
}
/* The GEAR page's pick (menu page 5): MenuWornItemStep calls it when Cross is
   newly pressed. who = the member in the selected party slot,
   ((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]]; the cursor g_MenuWornItemCursor is on
   one of the twelve cells of g_MenuIconBytes - 0..5 the worn slots (record
   +0x1E..+0x23), 6..11 the carried items (+0x24..+0x29).

   The low byte of MenuWornItemColGet(who) is 4 when the cell cannot be
   acted on: SE request 4 (g_MenuSoundRequest) and nothing else. Otherwise one of
   three arms, then SE request 2 and a redraw of the page
   (g_MenuRedrawRequest = g_MenuPage):

   - a WORN cell (cursor < 6) - take it off: ChrWornItemRemove(who, cursor + 1)
     unequips slot cursor + 1 and returns what was worn, and
     ChrItemGive(who, that) puts it with the member's carried items.
   - a CARRIED cell - put it on. item = the cell; kind = byte +4 of its
     0x12-byte record in *g_ChrItemRows (read at -0xE from the record after
     it, as in MenuWornItemDisp).
       kind != 4: the slot is kind + 1. old = ChrWornItemRemove(who, slot)
         takes off what is worn there.
       kind == 4, the kind with two slots (5 and 6; record +0x22 and
         +0x23): old = 0. If +0x23 holds something, old =
         ChrWornItemRemove(who, 5) takes off slot 5, +0x23's item moves down
         to +0x22 and +0x23 is cleared. The slot is then 6 when +0x22
         holds something, else 5 - the two slots are a queue, the new item
         always goes behind what is worn and the oldest comes off.
     Then ChrWornItemSet(who, slot, item) wears it, ChrItemRemove(who,
     item) takes it out of the carried cells and ChrItemGive(who, old)
     puts the old item (possibly none) there.

   Nothing looks at a result of ChrItemGive, ChrItemRemove or
   ChrWornItemSet. No frame wait and no RNG draw of its own.

   Shape: every arm is written whole, down to its own ChrItemGive call;
   cross-jumping then merges the two put-it-on tails at ChrWornItemSet's jal
   and all three at ChrItemGive's. A tail shared in the source needs
   (u16)who again after the join, a fourth andi with who still live beside
   it: 84/109 with the three put-it-on calls shared, 82/109 with the last
   two. who is an int: MenuWornItemColGet, still undeclared here, takes it whole
   (cast: 84/109) and the helpers take (u16)who (a u16 local with bare
   arguments: 83/109). Each arm's first cast is an andi the arm's later
   calls reuse, and as who is not read again in that arm it is done in
   place in $s0. The cast at ChrWornItemRemove(who, 5) is the only one in its
   block and leaves no andi (bare who there gives the same bytes). One
   local holds the item's kind and then the slot: it sits in $a1 from the
   lbu on (its one move into a register is the two-slot arm's `a1 = slot`),
   and kind + 1 is a separate value in $s1. A kind local of its own shares
   $s1 with kind + 1: 106/109. The two-slot arm's slot is 6 overwritten by
   5 (`== 0 ? 5 : 6` gives the same bytes); `!= 0 ? 6 : 5`, or an if / else
   in that order, makes 5 the branch's delay-slot default: 107/109. item
   matches as an int or as a u8. */
/* func_8006F69C */
void MenuWornItemChange(void)
{
    s32 who;
    s32 item;
    s32 slot;
    s32 old;

    who = ((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]];
    if ((u8)MenuWornItemColGet(who) == 4) {
        g_MenuSoundRequest = 4;
        return;
    }
    if (g_MenuWornItemCursor < 6) {
        old = ChrWornItemRemove((u16)who, (u16)(g_MenuWornItemCursor + 1));
        ChrItemGive((u16)who, (u8)old);
    } else {
        item = g_MenuIconBytes[g_MenuWornItemCursor];
        slot = *(u8 *)(item * 0x12 + g_ChrItemRows - 0xE);
        if (slot != 4) {
            old = ChrWornItemRemove((u16)who, slot + 1);
            ChrWornItemSet((u16)who, slot + 1, item);
            ChrItemRemove((u16)who, item);
            ChrItemGive((u16)who, (u8)old);
        } else {
            old = 0;
            if (*(u8 *)((who << 6) + g_ChrRecords + 0x23) != 0) {
                old = ChrWornItemRemove((u16)who, 5);
                *(u8 *)((who << 6) + g_ChrRecords + 0x22) = *(u8 *)((who << 6) + g_ChrRecords + 0x23);
                *(u8 *)((who << 6) + g_ChrRecords + 0x23) = 0;
            }
            slot = 6;
            if (*(u8 *)((who << 6) + g_ChrRecords + 0x22) == 0) {
                slot = 5;
            }
            ChrWornItemSet((u16)who, slot, item);
            ChrItemRemove((u16)who, item);
            ChrItemGive((u16)who, (u8)old);
        }
    }
    g_MenuSoundRequest = 2;
    g_MenuRedrawRequest = g_MenuPage;
}

/* far_syms: g_ChrItemRows (target reaches it %hi/%lo) */
void MenuChangeRowsDisp(s32 unused, u16 *list);  /* TU defines this below */

/* The GEAR page's draw (menu page 5; MenuWornItemStep calls it when
   g_MenuRedrawRequest == 5). who = the member in the selected party slot; the
   cursor g_MenuWornItemCursor walks the twelve cells of g_MenuIconBytes - 0..5 the worn
   slots (record +0x1E..+0x23), 6..11 the carried items (+0x24..+0x29).

   1. Entries 5 and 9 of g_MenuRenderEntries are copied to sprite slots 0x8A and
      0x8E (g_SpriteList + 0x1368 / + 0x13F8; 0x24 bytes each).
   2. The page is drawn: MenuIconBytesSet(who, page), the grid
      MenuIconBytesDisp(slot, 1, 2, cursor) (page 5's wide grid), the box
      MenuCommandsSet(who, page), MenuCommandsDisp(0), the furniture
      MenuPageDisp(page).
   3. bad = the low byte of MenuWornItemColGet(who): 4 when the cell under
      the cursor cannot be acted on, else 0.
   4. The equipment-change preview's two items:
      - a WORN cell (cursor < 6): new = nothing (0), old = that worn item -
        what taking it off would change;
      - a CARRIED cell: new = that item, old = the worn item in the slot
        its kind names (byte +4 of its 0x12-byte record in *g_ChrItemRows,
        read at -0xE from the record AFTER it, so an empty cell reads the
        0x12 bytes before the table) - cell `kind`, and for kind 4 (the
        two-slot kind) cell 5 when the member's +0x23 slot is empty, else
        cell 4. The same rule as the shop panel ShopItemStatsDisp.
      The lookup is made whether or not the preview is shown.
   5. bad == 0: ChrWornItemChangeMake(who, buf, new, old) fills the rows and
      MenuChangeRowsDisp(who, buf) draws them.
   6. MenuCursorTextDisp(the cell's item, bad) prints the item's name and
      description, dimmed (style 4) when bad.

   No frame wait and no RNG draw of its own.

   Shape: bad, the two items and the kind are u8 locals. bad keeps the raw
   call result in $a1 until its first use after the if/else, where it is
   masked once into $s1; the items come from lbu loads or the constant 0,
   so the undeclared ChrWornItemChangeMake call passes them unmasked. The item
   record is reached as an integer sum (index first), like the member
   record; g_ChrItemRows is a far scalar (same-register lui / lw). */
/* func_8006F850 */
void MenuWornItemDisp(void)
{
    u16 buf[12];
    s32 who;
    u8 bad;
    u8 newItem;
    u8 oldItem;
    u8 kind;

    *(SpriteEntry *)(g_SpriteList + 0x1368) = ((SpriteEntry *)g_MenuRenderEntries)[5];
    *(SpriteEntry *)(g_SpriteList + 0x13F8) = ((SpriteEntry *)g_MenuRenderEntries)[9];
    who = ((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]];
    MenuIconBytesSet(who, g_MenuPage);
    MenuIconBytesDisp(g_MenuMemberCursor, 1, 2, g_MenuWornItemCursor);
    MenuCommandsSet(who, g_MenuPage);
    MenuCommandsDisp(0);
    MenuPageDisp((u16)g_MenuPage);
    bad = MenuWornItemColGet(who);
    if (g_MenuWornItemCursor < 6) {
        oldItem = g_MenuIconBytes[g_MenuWornItemCursor];
        newItem = 0;
    } else {
        newItem = g_MenuIconBytes[g_MenuWornItemCursor];
        kind = *(u8 *)(newItem * 0x12 + g_ChrItemRows - 0xE);
        if (kind == 4 && *(u8 *)((who << 6) + g_ChrRecords + 0x23) == 0) {
            kind = 5;
        }
        oldItem = g_MenuIconBytes[kind];
    }
    if (bad == 0) {
        ChrWornItemChangeMake(who, buf, newItem, oldItem);
        MenuChangeRowsDisp(who, buf);
    }
    MenuCursorTextDisp(g_MenuIconBytes[g_MenuWornItemCursor], bad);
}
/* far_syms: g_ChrItemRows */

/* The GEAR page's REFUSAL TEST (menu page 5): may the cell under the cursor
   be acted on? Returns 4 when it may not and 0 when it may; both callers
   keep only the low byte - MenuWornItemChange (the pick, Cross) answers 4 with
   SE request 4 and nothing else, MenuWornItemDisp (the draw) passes it on as
   the dimmed style of MenuCursorTextDisp and skips the equipment-change preview.
   Both pass who, the member in the selected party slot.

   The cursor g_MenuWornItemCursor is on one of the twelve cells of g_MenuIconBytes: 0..5
   the worn slots, 6..11 the carried items. In order:

   1. item = the cell. An EMPTY cell: 4.
   2. A WORN cell (cursor < 6) - taking it off needs somewhere to put it:
      the carried cells are scanned from 11 down to 6 and the first empty
      one answers 0; all six full: 4. Nothing else is looked at (not who,
      not the item).
   3. A CARRIED cell - putting it on:
      - the item's kind (byte +4 of its 0x12-byte record in *g_ChrItemRows,
        read at -0xE from the record after it) is 0 and event flag 0x2D4
        is set (StoryBitCheck(0x2D4) != 0): 4. The flag is asked for only
        when the kind is 0. What sets the flag was not looked at.
      - else ChrItemUseGet(who, item, 1), the item table's "may who wear
        it" (0 when bit who - 1 of the record's mask is set, else -1; -2
        for an item outside 1..0xB6): 0 when it is 0, else 4. The cell is
        read again from g_MenuIconBytes[g_MenuWornItemCursor] for this call.

   Reads only; writes nothing. Callees StoryBitCheck and ChrItemUseGet. No
   frame wait and no RNG draw (neither callee has one).

   Shape: the carried arm is the ELSE of the worn test and the function
   ends in ONE `return 4;` that the loop falls out to. With that the last
   test's two returns are not a lone if / else on constants: the first
   jump pass turns `if (r != 0) goto last; v0 = 0; goto end; last: v0 = 4;`
   into `v0 = 0; if (r == 0) goto end; last: v0 = 4;` (seen in the .jump
   dump), and as the label `last` is still used by the loop's exit the
   store-flag rewrite, which wants the store directly after the branch
   (jump.c), does not apply. So the result of ChrItemUseGet is alive while
   $v0 is set - it is copied to $v1 - and the 0 ends in the branch's delay
   slot. Written flat, with `return 4;` after the loop and the last test as
   `if (...) return 0; return 4;` (either way round, or as a conditional
   expression), the tail becomes `sltu $v0, $zero, $v0; sll $v0, $v0, 2`
   and the flag arm's `return 4` shares its branch's delay slot: 48/59.
   The empty-cell test must stay its own `return 4;`: folded in as
   `if (item != 0) { ... } return 4;` it is 54/59 (one instruction short),
   and with the flag arm also sent to the last return, as an `||` around
   the last test or by goto, the cells' address is kept in $s0 across the
   calls: 31-32/59.
   who is an int and is passed as (u16)who to the unprototyped
   ChrItemUseGet, as everywhere in this TU: the andi sits in the delay
   slots of the two branches that reach the call. A u16 parameter passed
   bare, or a (u16, u8, u32) prototype for the callee with who passed
   bare, give the same bytes; the callers pass who whole with no
   declaration in sight, which is why it is an int here. item matches as
   an int or as a u8, and the record address with either operand first. */
/* func_8006FA50 */
s32 MenuWornItemColGet(s32 chr_id)
{
    s32 item;
    s32 i;

    item = g_MenuIconBytes[g_MenuWornItemCursor];
    if (item == 0) {
        return 4;
    }
    if (g_MenuWornItemCursor < 6) {
        for (i = 0xB; i >= 6; i--) {
            if (g_MenuIconBytes[i] == 0) {
                return 0;
            }
        }
    } else {
        if (*(u8 *)(item * 0x12 + g_ChrItemRows - 0xE) == 0 && StoryBitCheck(0x2D4) != 0) {
            return 4;
        }
        if (ChrItemUseGet((u16)chr_id, g_MenuIconBytes[g_MenuWornItemCursor], 1) == 0) {
            return 0;
        }
    }
    return 4;
}

/* func_8006FB3C */
void MenuChangeRowsDisp(s32 unused, u16 *list)
{
  u16 *p;
  s32 i;
  s32 y;
  u16 *q;
  s32 tag;
  p = list;
  i = 0;
  tag = 0xDEAD2001;
  y = 0x14;
  q = list + 2;
  do
  {
    if ((*p) == 0)
    {
      return;
    }
    font_target_clear(((*p) << 9) + 0x801CDD00, 4);
    i++;
    font_string_proportional_put((*p) + 0x618);
    CellTextDisp(0xC, y, 4, 1, 4, tag, (((*p) << 4) - 0x11F0) & 0xFFFF);
    font_target_clear(0x801CBB00, 8);
    font_string_fixed_put(0x616);
    CellTextDisp(0x19, y, 1, 1, 1, tag, 0xED1C);
    MenuTallGlyphSet(0xC, 0x14, y);
    MenuNumberCompareDisp(0x16, y, (u16) (q[-1]), (u16) (q[0]));
    q += 3;
    y += 2;
    /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
       the block runs once, so behaviour is the same without the `do` / `while (0)`, but
       then 3 of this function's 78 instruction lines differ. */
    do
    {
      p += 3;
    }
    while (0);
  }
  while (i < 3);
}
/* la_absolute: MenuChrRecordStep */
void MenuChrRecordDisp(u16 tab, s32 chr_id);  /* TU defines this below */

/* Menu page 6 (battle_phase_pre_table[6]): a member's STATUS page - the
   input step and redraw; MenuChrRecordDisp draws it. The tab D_800B8EF4[0] is
   MenuChrRecordDisp's mode: 1 the status sheet (MenuChrNumbersDisp), 2 the six
   equipment slots, 4 the eight spells. Mode 3 (the six carried items) is
   never shown here: both steps skip it.

   First call (D_800B8EF4[0] == 0): MenuChrRecordOpen() sets the page up,
   the tab becomes 1, redraw.

   The member is read up front: ((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]].
   By g_MenuPadRecord[2] (pad repeat), first match wins:
   - 0x8000 (left): tab - 1, wrapping below 2 to 4 (0 and 1 both go to 4),
     and a tab of 3 steps on to 2; SE request 1, redraw.
   - 0x2000 (right): tab + 1, wrapping from 4 (and above) to 1, and a tab of
     3 steps on to 4; SE request 1, redraw.
   - 0xC (L1 / R1): MenuMemberCursorMove(&g_MenuMemberCursor, bits) cycles the member,
     again while the new member is 0xC (the pad word is re-read each pass);
     SE request 2, redraw. Members 8, 9 and 10 are NOT skipped here (pages
     4 and 5 skip them).
   - else g_MenuPadRecord[1] & 0x130 (newly pressed), or the latch g_MenuBackOutLatch &
     g_MenuBackOutPattern: g_MenuBackOutLatch = 1 on 0x100, 2 on 0x20; g_MenuBackOutPattern =
     0x30B30B30; MenuChrRecordClose() takes the page down; back to page 2
     with the tab cleared (so the set-up runs again next time);
     MenuPageDisp(2); SE request 3, redraw of page 2.
   Up / down and Cross do nothing on this page.

   Redraw (g_MenuRedrawRequest == 6): MenuChrRecordDisp(tab, member) - which clears actor
   [1]'s e.h0 - then actor [1] e.h0 = 1 unless the tab is 4 (the spell list
   has no such mark); g_MenuRedrawRequest = 0. Every call ends with
   MenuLineUpDisp(g_MenuMemberCursor, 1, 0, 0).

   No frame wait and no RNG draw of its own.

   Codegen notes: the tab is element [0] of the small-data pair D_800B8EF4
   (element [1] is phase 9's first-call flag), under la_absolute. Its reads
   fold to %gp_rel; the address formed for the first test stays in $s1
   across MenuChrRecordOpen, and each `?:` store takes a copy of it ($a0) before
   the conditional and stores through it after the join - the aggregate-
   element shape of MenuOptionStep. The `== 3` fix-
   ups store %gp_rel again. The first arm's condition is written `>= 2` so
   the fall-through arm is `tab - 1` and the branch goes to the constant.
   The member id is the ARRAY spelling (MEM_IN_STRUCT), the pad record is
   g_MenuPadRecord[n] at every use (loop.c's copies of it and of &g_MenuMemberPlaces take
   $s2 / $s1 for the L1/R1 loop), and &g_MenuMemberCursor is the assembler's
   same-register lui/addiu. MenuChrRecordDisp needs its (u16, s32) prototype in
   scope: it is defined below this function. */
/* func_8006FC74 */
void MenuChrRecordStep(void)
{
    s32 who;

    if (D_800B8EF4[0] == 0) {
        MenuChrRecordOpen();
        D_800B8EF4[0] = 1;
        g_MenuRedrawRequest = g_MenuPage;
    }
    who = ((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]];
    if (g_MenuPadRecord[2] & 0x8000) {
        D_800B8EF4[0] = (D_800B8EF4[0] >= 2) ? (D_800B8EF4[0] - 1) : 4;
        if (D_800B8EF4[0] == 3) {
            D_800B8EF4[0]--;
        }
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[2] & 0x2000) {
        D_800B8EF4[0] = (D_800B8EF4[0] < 4) ? (D_800B8EF4[0] + 1) : 1;
        if (D_800B8EF4[0] == 3) {
            D_800B8EF4[0]++;
        }
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[2] & 0xC) {
        do {
            MenuMemberCursorMove(&g_MenuMemberCursor, g_MenuPadRecord[2] & 0xC);
            who = ((u16 *)g_PartyIds)[g_MenuMemberPlaces[g_MenuMemberCursor]];
        } while (who == 0xC);
        g_MenuSoundRequest = 2;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        MenuChrRecordClose();
        g_MenuPage = 2;
        D_800B8EF4[0] = 0;
        MenuPageDisp((u16)g_MenuPage);
        g_MenuSoundRequest = 3;
        g_MenuRedrawRequest = g_MenuPage;
    }
    if (g_MenuRedrawRequest == 6) {
        MenuChrRecordDisp(D_800B8EF4[0], who);
        if (D_800B8EF4[0] != 4) {
            AS(s16, ((MapRecord *)g_MenuRecords)[1].sprite.on) = 1;
        }
        g_MenuRedrawRequest = 0;
    }
    MenuLineUpDisp(g_MenuMemberCursor, 1, 0, 0);
}

/* A member's page of the menu: clears record 1's +4 halfword of
   g_MenuRecords (0x3C stride, so +0x40), redraws the frame, then member chr_id's
   header from its 64-byte persistent record at g_ChrRecords - CellChrGlyphSet
   at (6, 0xE), then MenuTallNumberSet numbers: level (+0x1C, 2 digits), total
   EXP (+0x38, 7) and EXP to next (+0x3C, 6) - and the list the mode tab
   names:

     1  MenuChrNumbersDisp(chr_id) draws the body instead;
     2  the 6 equipment slots  (+0x1E), col = 5;
     3  the 6 carried items    (+0x24), col = 5;
     4  the 8 spells           (+0x2A), col = 2, a non-zero id drawn as
        id - 0x4A.

   Entry i of the list is rendered into its own 0x400-byte cell of the
   buffer at 0x801CDB00 (font_target_set, font_string_proportional_put with 0x100 | id) and
   handed to IconCellSet(0x12, col + 3 * i, id, 0); after MenuTextPartsLoad(0x1FE)
   each entry gets CellTextDisp(0x15, col + 3 * i + 1, 8, 1, 8, 0xDEAD2001,
   0xEE00 + 0x20 * i). Before the list, the six party actors at g_MapPartyRecords
   (0x3C stride) take the low byte of their member id from g_PartyIds in
   byte 0, or 0 in mode 4; an empty party slot is left alone. Any other
   mode reads n, col and list unset, as the original does. No RNG.

   tab is a u16 formal: the target copies $a0 to $s2 unmasked and masks at
   each of the three tests (and hoists the loop's), which is what this
   compiler does for a narrow parameter. Declared s32 with the masks
   written out, the same code allocates tab and chr_id the other way round
   ($s3/$s2, 200/212). */
/* func_8006FEBC */
void MenuChrRecordDisp(u16 tab, s32 chr_id)
{
    s32 i;
    s32 n;
    s32 col;
    u8 *list;
    u8 c;

    ((MapRecord *)g_MenuRecords)[1].sprite.on = 0;
    MenuIconsClear(0x12, 2, 1, 8);
    MenuIconBytesSet(0, 0);
    MenuIconBytesDisp(g_MenuMemberCursor, 4, 1, 0xC);
    MenuPageDisp((u16)g_MenuPage);
    CellChrGlyphSet(chr_id, 6, 0xE, 0xDEAD2001, 0x3000, 1);
    MenuTallNumberSet(*(u8 *)((chr_id << 6) + g_ChrRecords + 0x1C), 0xE, 0xE, 2);
    MenuTallNumberSet(*(s32 *)((chr_id << 6) + g_ChrRecords + 0x38), 9, 0x14, 7);
    MenuTallNumberSet(*(s32 *)((chr_id << 6) + g_ChrRecords + 0x3C), 0xA, 0x18, 6);
    if (tab != 4) {
        for (i = 0; i < 6; i++) {
            if (*(u16 *)(i * 2 + g_PartyIds) != 0) {
                *(u8 *)(i * 0x3C + g_MapPartyRecords) = *(u16 *)(i * 2 + g_PartyIds);
            }
        }
    } else {
        for (i = 0; i < 6; i++) {
            if (*(u16 *)(i * 2 + g_PartyIds) != 0) {
                *(u8 *)(i * 0x3C + g_MapPartyRecords) = 0;
            }
        }
    }
    if (tab == 1) {
        MenuChrNumbersDisp(chr_id);
    } else {
        font_target_clear(0x801CDB00, 0x40);
        switch (tab) {
        case 2:
            n = 6;
            col = 5;
            list = (u8 *)(g_ChrRecords + (chr_id << 6)) + 0x1E;
            break;
        case 3:
            n = 6;
            col = 5;
            list = (u8 *)(g_ChrRecords + (chr_id << 6)) + 0x24;
            break;
        case 4:
            n = 8;
            col = 2;
            list = (u8 *)(g_ChrRecords + (chr_id << 6)) + 0x2A;
            break;
        }
        for (i = 0; i < n; i++) {
            font_target_set(0x801CDB00 + i * 0x400);
            c = list[i];
            if (tab == 4 && c != 0) {
                c -= 0x4A;
            }
            font_string_proportional_put(c | 0x100);
            IconCellSet(0x12, col + i * 3, c, 0);
        }
        MenuTextPartsLoad(0x1FE);
        for (i = 0; i < n; i++) {
            CellTextDisp(0x15, col + i * 3 + 1, 8, 1, 8, 0xDEAD2001, (u16)(i * 0x20 - 0x1200));
        }
        CellAllUpdate(0x10);
    }
    CellAllUpdate(0x20);
}
/* MenuChrNumbersDisp -- the body of a member's page in mode 1
   (MenuChrRecordDisp calls it instead of drawing a list): the status sheet
   of member chr_id, from its 64-byte persistent record at g_ChrRecords.

   Labels: four pre-rendered strips (CellTextDisp, col 0x12) - 0xED40 at
   row 5 (2 rows high), 0xED60 at row 0xA (6 rows), 0xEDD0 at row 0x16 and
   0xEDC0 at row 0x18. Then a separator on every value row - glyph 0xC at
   col 0x1A (MenuTallGlyphSet) plus a one-cell strip at col 0x1E: 0xED18 on
   rows 5 and 7, 0xED1C on the eight rows 0xA, 0xC .. 0x18.

   Numbers (3 digits, MenuTallNumberSet): record +0 at (0x1B, 5), +2 at
   (0x20, 5), +4 at (0x1B, 7), +6 at (0x20, 7) - the two current / maximum
   pairs. Then eight MenuNumberCompareDisp(0x1B, row, a, b) pairs - it prints a at
   col 0x1B and b at col 0x20, b in colour 0x3000 when equal, else 0x2000
   when a < b, else 0 - on rows 0xA .. 0x18:
       (+0x14, +8 u16) (+0x15, +0xA u16) (+0x16, +0xC u16) (+0x17, +0xE u16)
       (+0x18, +0x10 u16) (+0x19, +0x12 u8) (+0x1A, +0x13 u8) (+0x1B, +0x1D u8)
   No frame wait and no RNG draw of its own.

   Shape: the row is `i * 2 + 5` / `i * 2 + 0xA` written out at both uses,
   not a second counter. loop.c strength-reduces it to its own register and
   puts that giv's initialisation AFTER the hoisted invariants (1,
   0xDEAD2001, the strip code), which is the target's order; a separate
   `y` variable initialised beside `i` lands BEFORE them (184/186). The
   record address is spelled out at every use; cse shares (chr_id << 6) across
   the calls and the base within one. */
/* func_8007020C */
void MenuChrNumbersDisp(s32 chr_id)
{
    s32 i;

    CellTextDisp(0x12, 5, 4, 2, 4, 0xDEAD2001, 0xED40);
    CellTextDisp(0x12, 0xA, 4, 6, 4, 0xDEAD2001, 0xED60);
    CellTextDisp(0x12, 0x16, 4, 1, 4, 0xDEAD2001, 0xEDD0);
    CellTextDisp(0x12, 0x18, 4, 1, 4, 0xDEAD2001, 0xEDC0);
    for (i = 0; i < 2; i++) {
        MenuTallGlyphSet(0xC, 0x1A, i * 2 + 5);
        CellTextDisp(0x1E, i * 2 + 5, 1, 1, 1, 0xDEAD2001, 0xED18);
    }
    for (i = 0; i < 8; i++) {
        MenuTallGlyphSet(0xC, 0x1A, i * 2 + 0xA);
        CellTextDisp(0x1E, i * 2 + 0xA, 1, 1, 1, 0xDEAD2001, 0xED1C);
    }
    MenuTallNumberSet(*(u16 *)((chr_id << 6) + g_ChrRecords + 0), 0x1B, 5, 3);
    MenuTallNumberSet(*(u16 *)((chr_id << 6) + g_ChrRecords + 2), 0x20, 5, 3);
    MenuTallNumberSet(*(u16 *)((chr_id << 6) + g_ChrRecords + 4), 0x1B, 7, 3);
    MenuTallNumberSet(*(u16 *)((chr_id << 6) + g_ChrRecords + 6), 0x20, 7, 3);
    MenuNumberCompareDisp(0x1B, 0xA, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x14), *(u16 *)((chr_id << 6) + g_ChrRecords + 8));
    MenuNumberCompareDisp(0x1B, 0xC, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x15), *(u16 *)((chr_id << 6) + g_ChrRecords + 0xA));
    MenuNumberCompareDisp(0x1B, 0xE, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x16), *(u16 *)((chr_id << 6) + g_ChrRecords + 0xC));
    MenuNumberCompareDisp(0x1B, 0x10, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x17), *(u16 *)((chr_id << 6) + g_ChrRecords + 0xE));
    MenuNumberCompareDisp(0x1B, 0x12, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x18), *(u16 *)((chr_id << 6) + g_ChrRecords + 0x10));
    MenuNumberCompareDisp(0x1B, 0x14, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x19), *(u8 *)((chr_id << 6) + g_ChrRecords + 0x12));
    MenuNumberCompareDisp(0x1B, 0x16, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x1A), *(u8 *)((chr_id << 6) + g_ChrRecords + 0x13));
    MenuNumberCompareDisp(0x1B, 0x18, *(u8 *)((chr_id << 6) + g_ChrRecords + 0x1B), *(u8 *)((chr_id << 6) + g_ChrRecords + 0x1D));
}
/* MenuChrRecordOpen -- the STATUS page's set-up: page 6's handler
   MenuChrRecordStep calls it once, on its first call
   (tab D_800B8EF4[0] == 0), before the tab becomes 1. It takes nothing
   and returns nothing.

   1. The frame is cleared to the bare page: MenuPageDisp(0xFFFF) (the
      page body with nothing in it), MenuCommandsSet(0, g_MenuPage) (the
      page's command list), MenuCommandsDisp(0) (the command column, cursor
      0), MenuIconBytesSet(0, 0) (the twelve grid cells g_MenuIconBytes zeroed),
      MenuIconBytesDisp(0xFFFF, 1, 2, 0) (the lower grid, two rows, no face)
      and MenuIconBytesDisp(g_MenuMemberCursor, 4, 2, 0) (the upper grid with the
      selected slot's face).
   2. Actors [5] and [6] of g_MenuRecords, two markers at the page's left
      and right edges (their art was not looked up): [5] to x =
      g_MapViewX + 0x10, [6] to x = g_MapViewX + 0x128, both to y =
      g_MapViewY + 0x68; MapAnimationSet plays animation 3 on [5] and 2 on
      [6]; then both are switched on (e.h0 = 1, [6] first). MenuChrRecordClose,
      the page's take-down, switches the same two off.
   3. The sheet's labels are printed into the text buffer (0x80-byte cells
      from 0x801CBB00): the pen goes to cell 0 (font_target_set) and strings
      0x616, 0x617 and 0x618 are printed one after the other
      (font_string_fixed_put: 64 blanks and four glyphs, "123MH-", and a strip of
      blanks with two glyph pairs); then for i = 0x19..0x21 the four cells
      from cell 0x18 + (i - 0x19) * 4 are cleared and made the target
      (font_target_clear(cell, 4)) and string 0x600 + i is printed there
      (font_string_proportional_put) -- "Attack", "Defense", "Agility", "Wisdom", "Magic
      Endr", "#Attack", "Luck", "Range", "Exp"; last the pen goes to cell
      0x3B (0x801CD880, the fourth cell of "Exp"'s block, not cleared) and
      string 0x622, "Next Level", is printed.

   No frame wait and no RNG draw of its own (MenuIconBytesDisp can run frames
   when the face has to come off the disc).

   MARKED SPELLING: `pad`. The target frame is 0x40 -- the four saved
   registers at 0x30-0x3C over 0x20 bytes at 0x10-0x2F that no instruction
   touches (no call here passes more than four arguments). Without a
   local the frame is 0x20 and the ten prologue / epilogue rows differ
   (69/79); every other row matches without it. Any unused
   aggregate local of 25..32 bytes gives the target frame (u8[25], u8[32],
   s32[8], s16[16], two u8[16]; 24 bytes gives 0x38, 33 gives 0x48), and
   so does a call with twelve arguments in dead code (its outgoing words
   alone). What the original declared is not recoverable from the bytes,
   and the Japanese build has no twin of this function to say. It is the
   same thing as MenuRun's `pad` in this file.

   Shape: the two markers' fields are MapRecord members at every store (as
   casts through g_MenuRecords + K: 71/79), in the order x[5], y[5], x[6], y[6] (71 the other
   way) with [6].e.h0 before [5].e.h0 (77). The label address is written
   in cells, 0x801CBB00 + (...) * 0x80: the multiply on top keeps fold from
   merging the base into the induction variable, so the base stays a
   hoisted constant ($s2) added in the loop, as in MenuNoticeDisp;
   distributed (`0x801CBB00 + (i * 0x200 - 0x2600)`) it is one giv
   (71/79). The three loop registers are loaded right after the first
   call by the scheduler; the source has a plain `for`. */
/* func_800704F4 */
void MenuChrRecordOpen(void)
{
    u8 pad[32];     /* unused: the target frame (0x40) holds 32 bytes at
                       0x10-0x2F that no instruction touches -- the
                       original's local(s), declaration unknown */
    s32 i;

    MenuPageDisp(0xFFFF);
    MenuCommandsSet(0, g_MenuPage);
    MenuCommandsDisp(0);
    MenuIconBytesSet(0, 0);
    MenuIconBytesDisp(0xFFFF, 1, 2, 0);
    MenuIconBytesDisp(g_MenuMemberCursor, 4, 2, 0);
    ((MapRecord *)g_MenuRecords)[5].sprite.x = g_MapViewX + 0x10;
    ((MapRecord *)g_MenuRecords)[5].sprite.y = g_MapViewY + 0x68;
    ((MapRecord *)g_MenuRecords)[6].sprite.x = g_MapViewX + 0x128;
    ((MapRecord *)g_MenuRecords)[6].sprite.y = g_MapViewY + 0x68;
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[5], 3);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[6], 2);
    AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 1;
    AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 1;
    font_target_set(0x801CBB00);
    font_string_fixed_put(0x616);
    font_string_fixed_put(0x617);
    font_string_fixed_put(0x618);
    for (i = 0x19; i < 0x22; i++) {
        font_target_clear(0x801CBB00 + ((i - 0x19) * 4 + 0x18) * 0x80, 4);
        font_string_proportional_put(i + 0x600);
    }
    font_target_set(0x801CD880);
    font_string_proportional_put(0x622);
}
/* MenuChrRecordClose -- the STATUS page's take-down, the counterpart of
   MenuChrRecordOpen: page 6's handler MenuChrRecordStep calls it once, when
   the page is left (pad bits 0x130 newly pressed, or the latch g_MenuBackOutLatch &
   g_MenuBackOutPattern), just before it goes back to page 2. It takes nothing and
   returns nothing.

   1. Actors [6], [5] and [2] of g_MenuRecords (0x3C stride) are switched off
      (e.h0 = 0, in that order): the two scroll marks, which MenuChrRecordOpen
      had borrowed as this page's left / right edge markers, and the icon
      grid's cursor.
   2. The two marks go back to the item list's places, the ones
      MenuItemIconBytesDisp gives them: [5] (more above) to x = g_MapViewX +
      0x120, y = g_MapViewY + 0x98; [6] (more below) to the same x, y =
      g_MapViewY + 0xC8 (e.h2 / e.h6). All seven stores are made before
      the first call.
   3. MapAnimationSet plays animation 4 on [5] and 5 on [6] -- the two
      MenuRecordsInit gave them when the menu was built (MenuChrRecordOpen had
      put 3 and 2 on them for this page).
   4. MenuIconsClear(0x12, 2, 1, 8) clears one column of eight icon cells,
      IconCellSet(0x12, 2 + 3 * i, 0, 0): the cells MenuChrRecordDisp's lists draw
      their icons in (IconCellSet(0x12, col + 3 * i, id, 0)).
   5. MenuIconBytesDisp(0xFFFF, 4, 2, 0) redraws the upper icon grid (text row
      2, two rows of six from g_MenuIconBytes) with NO face -- the face slot
      g_MessageFaceFirst gets MessageFaceEntrySet(0, ...) -- and puts the cursor actor
      [2] on cell 0.
   6. MenuPageDisp(0xFFFF): the page body with nothing in it.
   7. For the six party rows i: a present member (the u16 at g_PartyIds +
      i * 2 != 0) gets the low byte of its id back in byte 0 of its field
      actor (g_MapPartyRecords + i * 0x3C), which MenuChrRecordDisp's spell tab (mode
      4) zeroes; an empty row is left alone.

   No frame wait and no RNG draw of its own. MapAnimationSet (asm) calls
   only MapAnimationStart. Whether MenuIconBytesDisp's MessageFaceEntrySet(0, ...) can
   run frames (MessageFaceSet under it is the portrait loader) was not
   checked.

   Shape: nothing had to be searched for; these are the spellings of
   MenuChrRecordOpen and MenuChrRecordDisp. The x sum is written out at both
   stores (cse keeps one copy, in $a2) and g_MenuRecords is read again at
   every use (one load before the first call, one after it). $a2 and $a3
   at the first jal hold the x and the second y: temporaries, not
   arguments -- MapAnimationSet takes two. In the loop the row addresses are
   product first on the s32 bases, as in MenuChrRecordDisp, and the id is the
   u16 read stored through a u8 lvalue, which narrows to an lbu of the
   address the test loaded from. */
/* func_80070630 */
void MenuChrRecordClose(void)
{
    s32 i;

    AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
    AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
    AS(s16, ((MapRecord *)g_MenuRecords)[2].sprite.on) = 0;
    ((MapRecord *)g_MenuRecords)[5].sprite.x = g_MapViewX + 0x120;
    ((MapRecord *)g_MenuRecords)[5].sprite.y = g_MapViewY + 0x98;
    ((MapRecord *)g_MenuRecords)[6].sprite.x = g_MapViewX + 0x120;
    ((MapRecord *)g_MenuRecords)[6].sprite.y = g_MapViewY + 0xC8;
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[5], 4);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[6], 5);
    MenuIconsClear(0x12, 2, 1, 8);
    MenuIconBytesDisp(0xFFFF, 4, 2, 0);
    MenuPageDisp(0xFFFF);
    for (i = 0; i < 6; i++) {
        if (*(u16 *)(i * 2 + g_PartyIds) != 0) {
            *(u8 *)(i * 0x3C + g_MapPartyRecords) = *(u16 *)(i * 2 + g_PartyIds);
        }
    }
}

/* la_absolute: MenuPresetStep */
void MenuPresetDisp(s32 chr_id);  /* TU defines this below */

/* MenuPresetStep -- menu phase 9, one frame: the SLOT step of the command
   set-up page (entry 9 of the phase handler table that starts at
   battle_phase_pre_table; MenuPresetKindStep = phase 0xE the kind row,
   MenuPresetSpellStep / _05 = phases 0xF / 0x10 the spell / item pickers, which
   return here). Two cursors: the party slot g_MenuPresetMemberCursor and the command
   entry g_MenuPresetCursor (a member's 8-byte record at g_ChrPlacePresetRecords + who * 8 holds
   {kind, value} byte pairs from +2 -- MenuPresetSet writes them). who =
   g_PartyIds[g_MenuMemberPlaces[slot]].
   First call (D_800B8EF4[1] == 0): MenuPresetOpen() sets the page up, the
   flag becomes 1 and a redraw is requested.
   Pad (g_MenuPadRecord[2] repeat, [1] new press):
   - 0xA000: the entry cursor steps through MenuCursorMove(cursor,
     g_MenuIconBytes + 6, bits); SE 1, redraw.
   - 0x5000: MenuMemberCursorMove(&g_MenuPresetMemberCursor, bits) steps the party slot, again
     while the member there is 0xC (an EMPTY slot, 0, is accepted here --
     the redraw below moves off it); SE 1, redraw.
   - cancel (0x130, or the g_MenuBackOutLatch & g_MenuBackOutPattern auto-cancel): the latch
     is set (1 for bit 0x100, 2 for bit 0x20), g_MenuBackOutPattern = 0x30B30B30, the
     first-call flag is cleared, MenuPresetClose() tears the page down, the
     phase becomes 2 (MenuPageDisp(2)); SE 3.
   - 0x40: the kind-row cursor g_MenuPresetChoiceCursor takes the kind byte stored in the
     chosen entry, actor 4's flag is cleared, MapAnimationSet(&actor[2], 0xB),
     the phase becomes 0xE, SE 2, and actors 5 / 6 (the more-above /
     more-below marks) get y = g_MapViewY + ((slot - 1) * 3 + 2) * 8 and
     + ((slot - 1) * 3 + 4) * 8. ORIGINAL BEHAVIOUR: the guard ahead of it,
     `while (who == 0 || who == 0xC) MenuMemberCursorMove(&g_MenuPresetMemberCursor, 0x8000);`,
     never re-reads who -- confirming on an empty or 0xC slot would step the
     slot forever. (The redraw's copy of the loop does re-read it, and a
     redraw always runs before the next input, so the slot is never one of
     those when 0x40 is read.)
   Redraw (g_MenuRedrawRequest == 9): the slot is stepped (bits 0x8000) until its
   member is neither 0 nor 0xC, then MenuPresetDisp(who) draws the page (it
   runs one frame). Every call ends with MenuLineUpDisp(slot, 1, 0, 0). No
   RNG of its own.
   Shapes: who is an int (a u16 is re-masked at every test, 146/178); the
   pad record is read as g_MenuPadRecord[n] at every use -- a pointer local
   becomes one callee-saved pseudo, where the target keeps the base in $v1
   and the 0x5000 loop's hoisted copy in $s2; the entry's address is spelled
   `who * 8`, not `who << 3` (168 -> 178: with the shift the two temporaries
   swap $v0 / $v1); both `while (a || b)` loops keep their entry jump
   because a test with an internal label is never duplicated
   (duplicate_loop_exit_test returns 0 on a CODE_LABEL). */
/* func_8007070C */
void MenuPresetStep(void)
{
    s32 who;

    if (D_800B8EF4[1] == 0) {
        MenuPresetOpen();
        D_800B8EF4[1] = 1;
        g_MenuRedrawRequest = g_MenuPage;
    }
    if (g_MenuPadRecord[2] & 0xA000) {
        g_MenuPresetCursor = MenuCursorMove(g_MenuPresetCursor, g_MenuIconBytes + 6, g_MenuPadRecord[2] & 0xA000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[2] & 0x5000) {
        do {
            MenuMemberCursorMove(&g_MenuPresetMemberCursor, g_MenuPadRecord[2] & 0x5000);
            who = *(u16 *)(g_MenuMemberPlaces[g_MenuPresetMemberCursor] * 2 + g_PartyIds);
        } while (who == 0xC);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        D_800B8EF4[1] = 0;
        MenuPresetClose();
        g_MenuPage = 2;
        MenuPageDisp((u16)g_MenuPage);
        g_MenuSoundRequest = 3;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        who = *(u16 *)(g_MenuMemberPlaces[g_MenuPresetMemberCursor] * 2 + g_PartyIds);
        while (who == 0 || who == 0xC) {
            MenuMemberCursorMove(&g_MenuPresetMemberCursor, 0x8000);
        }
        g_MenuPresetChoiceCursor = *(u8 *)(g_MenuPresetCursor * 2 + (who * 8 + g_ChrPlacePresetRecords) + 2);
        AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
        MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xB);
        g_MenuPage = 0xE;
        g_MenuSoundRequest = 2;
        ((MapRecord *)g_MenuRecords)[5].sprite.y = g_MapViewY + ((g_MenuPresetMemberCursor - 1) * 3 + 2) * 8;
        ((MapRecord *)g_MenuRecords)[6].sprite.y = g_MapViewY + ((g_MenuPresetMemberCursor - 1) * 3 + 4) * 8;
        g_MenuRedrawRequest = g_MenuPage;
    }
    if (g_MenuRedrawRequest == 9) {
        while ((who = *(u16 *)(g_MenuMemberPlaces[g_MenuPresetMemberCursor] * 2 + g_PartyIds)) == 0 || who == 0xC) {
            MenuMemberCursorMove(&g_MenuPresetMemberCursor, 0x8000);
        }
        MenuPresetDisp(who);
        g_MenuRedrawRequest = 0;
    }
    MenuLineUpDisp(g_MenuPresetMemberCursor, 1, 0, 0);
}

extern u8 g_MenuPresetKindIcons[];
s32 sprite_run_off();
s32 ChrItemAnyCheck();
s32 ChrItemHeldCheck();
s32 ChrItemUseGet();
s32 ChrSpellAnyCheck();
s32 MenuPresetMemberDisp();

/* MenuPresetDisp — draws member chr_id's list page for list mode g_MenuPage
   (9, 0xE, 0xF or 0x10) and greys out the highlighted entry. Mode 0xF
   loads the four bytes at +0x2A + g_MenuPresetHalf*4 of the member's 64-byte
   record into g_MenuIconBytes (nonzero ids biased +0xB6); mode 0x10 copies the
   six-byte row g_MenuPresetHalf + 5. The six command slots g_MenuMemberPlaces[(i+1) % 6]
   are drawn through MenuPresetMemberDisp (command ids 0 and 0xC skipped); the slot
   whose 1-based number equals g_MenuPresetMemberCursor gives the highlighted id and row.
   The grey flag (4) comes from: mode 9, the kind byte (+2) of entry
   g_MenuPresetCursor in the member's g_ChrPlacePresetRecords record — kind 3 compares the cost
   byte (-8 into the 12-byte record id-0xB6 of g_ChrSpellRows[0], halved rounding
   up when byte +0x22 or +0x23 is 0x88) against the member's +4 halfword,
   kind 4 asks ChrItemHeldCheck; mode 0xE, entry g_MenuPresetChoiceCursor of g_MenuPresetKindIcons with
   checks ChrSpellAnyCheck (row 3) / ChrItemAnyCheck (row 4); mode 0xF,
   ChrSpellUseGet then the same cost test; mode 0x10, ChrItemUseGet. Then the
   cursor actors are placed and, in mode 9, entry 2 of g_MenuRenderEntries is copied
   to g_SpriteList + 0x12FC. No RNG.
   Decisive shapes: the cost record's i*12 computed as its own statement
   (an address-context multiply is expanded after the %hi load); the call
   result reusing v, the variable passed in $a1; the row-3/4 test on a u16
   copy of the cursor (combine folds its zero-extend into a plain copy);
   the 0xF6 store before t = g_MenuPresetCursor * 2; the g_MenuPresetChoiceCursor cursor and the
   g_MenuRenderEntries entry pointer held in variables; the g_MapViewX + 0x60 sum
   held in y (inline, fold moves the + 0x60 onto the shift); bad = 0
   restated for kinds other than 3/4; g_MenuIconBytes[i] = b = ... (store
   address first). */
/* func_800709D4 */
void MenuPresetDisp(s32 chr_id)
{
  s32 i;
  s32 k;
  s32 n;
  s32 sel;
  s32 cur;
  s8 bad;
  u8 b;
  u16 page;
  s32 v;
  u8 *p;
  s32 idx;
  s32 t;
  u16 row;
  s32 y;
  u16 cf8;
  SpriteEntry *src;
  sprite_run_off(0x87, 8);
  font_target_clear(0x801CDF00, 0x28);
  MenuIconBytesSet(chr_id, g_MenuPage);
  if (chr_id == 0xC)
  {
    page = g_MenuMemberCursor;
  }
  else
  {
    page = g_MenuPresetMemberCursor;
  }
  MenuIconBytesDisp(page, 1, 2, g_MenuPresetCursor + 6);
  MenuPageDisp((u16) g_MenuPage);
  MenuFrameStep();
  if (g_MenuPage == 0xF)
  {
    for (i = 0; i < 4; i++)
    {
      g_MenuIconBytes[i] = b = *(u8 *) (((chr_id << 6) + g_ChrRecords) + ((g_MenuPresetHalf * 4) + i) + 0x2A);
      if (b != 0)
      {
        g_MenuIconBytes[i] = b + 0xB6;
      }
    }
  }
  else if (g_MenuPage == 0x10)
  {
    /* A ChrRecord's worn items and items are two rows of six: list mode 0x10
       copies the row g_MenuPresetHalf picks. Read as a member array, so that
       the row's offset stays one addend (half * 6 + 0x1E) as the match needs;
       a plain pointer cast moves the + 0x1E out. */
    mem_move((s32 *) g_MenuIconBytes, (s32 *) ((struct { u8 v[2][6]; } *)&((ChrRecord *) g_ChrRecords)[chr_id].worn_items)->v[g_MenuPresetHalf], 6);
  }
  bad = 0;
  sel = 0;
  n = 0;
  cur = g_MenuIconBytes[g_MenuPresetCursor + 6];
  for (i = 0; i < 6; i++)
  {
    k = i + 1;
    v = *(u16 *) (g_PartyIds + (g_MenuMemberPlaces[k % 6] * 2));
    if ((v != 0) && (v != 0xC))
    {
      v = MenuPresetMemberDisp(n, v, chr_id);
      if (k == g_MenuPresetMemberCursor)
      {
        cur = v;
        sel = n;
      }
      n++;
    }
  }
  MenuCommandsSet(chr_id, (u16) g_MenuPage);
  MenuCommandsDisp(0);
  MenuTextPartsLoad(0x7C);
  if (g_MenuPage == 9)
  {
    ((MapRecord *) g_MenuRecords)[4].sprite.x = g_MapViewX + 0x60;
    t = g_MenuPresetCursor * 2;
    b = *(u8 *) (t + ((chr_id << 3) + g_ChrPlacePresetRecords) + 2);
    if (b == 3)
    {
      i = cur - 0xB6;
      p = (u8 *) ((i * 12) + g_ChrSpellRows[0]);
      i = p[-8];
      if ((*(u8 *) (((chr_id << 6) + g_ChrRecords) + 0x22) == 0x88) || (*(u8 *) (((chr_id << 6) + g_ChrRecords) + 0x23) == 0x88))
      {
        i = (i + 1) >> 1;
      }
      bad = (*(u16 *) (((chr_id << 6) + g_ChrRecords) + 4) < i) << 2;
    }
    else if (b == 4)
    {
      bad = (ChrItemHeldCheck((u16) chr_id, (u8) cur, 0) == 0) << 2;
    }
    else
    {
      bad = 0;
    }
  }
  else
  {
    idx = g_MenuPresetChoiceCursor;
    ((MapRecord *) g_MenuRecords)[4].sprite.x = (idx << 5) + (y = g_MapViewX + 0x60);
    switch (g_MenuPage)
    {
      case 0xE:
        cur = g_MenuPresetKindIcons[idx];
        row = idx;
        if (row == 3)
        {
          if (ChrSpellAnyCheck((u16) chr_id) == 0)
          {
            bad = 4;
          }
        }
        else if (row == 4)
        {
          if (ChrItemAnyCheck((u16) chr_id) == 0)
          {
            bad = 4;
          }
        }
        break;
      case 0xF:
        cur = g_MenuIconBytes[idx];
        if (ChrSpellUseGet((u16) chr_id, (u8) (cur + 0x4A), 4) < 0)
        {
          bad = 4;
        }
        else
        {
          i = cur - 0xB6;
          p = (u8 *) ((i * 12) + g_ChrSpellRows[0]);
          i = p[-8];
          if ((*(u8 *) (((chr_id << 6) + g_ChrRecords) + 0x22) == 0x88) || (*(u8 *) (((chr_id << 6) + g_ChrRecords) + 0x23) == 0x88))
          {
            i = (i + 1) >> 1;
          }
          if (*(u16 *) (((chr_id << 6) + g_ChrRecords) + 4) < i)
          {
            bad = 4;
          }
        }
        break;
      case 0x10:
        cur = g_MenuIconBytes[idx];
        if (ChrItemUseGet((u16) chr_id, cur, 4) < 0)
        {
          bad = 4;
        }
        break;
    }
  }
  MenuCursorTextDisp(cur, bad);
  cf8 = g_MapViewY;
  ((MapRecord *) g_MenuRecords)[4].sprite.y = (sel * 24) + (cf8 + 0x10);
  AS(s16, ((MapRecord *) g_MenuRecords)[4].sprite.on) = 1;
  AS(s16, ((MapRecord *) g_MenuRecords)[2].sprite.on) = 1;
  AS(s16, ((MapRecord *) g_MenuRecords)[1].sprite.on) = 1;
  ((SpriteEntry *) g_MenuRenderEntries)[2].y = cf8 + ((((g_MenuPresetMemberCursor - 1) * 3) + 3) * 8);
  src = &((SpriteEntry *) g_MenuRenderEntries)[2];
  if (g_MenuPage == 9)
  {
    *(SpriteEntry *) (g_SpriteList + 0x12FC) = *src;
  }
}
/* MenuPresetMemberDisp -- draws ONE member's row of the command set-up page
   (menu phases 9, 0xE, 0xF, 0x10); MenuPresetDisp calls it once per present
   member other than 0xC, with n = the row (0..), who = the member's id and
   sel = the member the page is open on. Returns the row's command id, which
   the caller keeps for the highlighted row.

   The member's preset command is the {kind, value} byte pair of entry
   g_MenuPresetCursor in its 8-byte g_ChrPlacePresetRecords record (bytes +2 + entry*2 and
   +3 + entry*2; MenuPresetSet writes them). It only READS those two bytes:
   the formation cell at +0/+1 is not touched here, and neither are the
   battle home tiles g_BtlPartyPlaceXCells / g_BtlPartyPlaceYCells.
     kind 3 (a spell): id = value; grey (style 4) when the member's MP
          (+4 of its 64-byte g_ChrRecords record) is below the cost -- byte +4
          of the 12-byte g_ChrSpellRows[0] record id - 0xB7, halved rounding up
          when the member's +0x22 or +0x23 byte is 0x88.
     kind 4 (an item): id = value; grey when ChrItemHeldCheck(who, id, 0) is 0.
     other kinds: id = g_MenuPresetKindIcons[kind], never grey.
   The row's text buffer is 0x801CDB00 + (n + 1) * 0x400 (font_target_set).
     phase 9: the name is printed (string id | 0x100) in the grey/normal
          style -- font_col_set returns the old style, which is put back --
          and the icon drawn at column 0xC, row n * 3 + 2 (IconCellSet's attr
          0x10000000); for the open member the five cells to its right
          (columns 0x10..0x20, where a picker's icons were) are cleared
          first -- MenuIconsClear(0x10, row, 5, 1) -- and the attr is 0.
     other phases, the open member: no name; six icons across the row at
          columns 0xC, 0x10, .. -- the kind list g_MenuPresetKindIcons in phase 0xE,
          the loaded page g_MenuIconBytes in phases 0xF / 0x10.
     other phases, any other member: the name and the icon as in phase 9
          (attr 0x10000000), in the grey/normal style.
   No frame wait and no RNG draw of its own.

   Shape: kind is an int and value a u8 (the target re-masks only the value,
   `andi 0xFF`, in each arm); the entry address is spelled out at both reads
   (a pointer variable shared with the list pointer takes a callee-saved
   register and a fifth save: 118/149); `s` is ONE variable -- the cost, the
   grey flag, the saved style and IconCellSet's attr all live in $s0; the list
   test is written != 0xE with g_MenuIconBytes first; the column is the derived
   expression i * 4 + 0xC. font_col_set is called undeclared, as elsewhere
   in this TU. */
/* func_80070F1C */
s32 MenuPresetMemberDisp(s32 n, s32 who, s32 sel)
{
    s32 kind;
    u8 val;
    s32 ret;
    s32 s;
    u8 *list;
    s32 i;

    kind = *(u8 *)(g_MenuPresetCursor * 2 + (who * 8 + g_ChrPlacePresetRecords) + 2);
    val = *(u8 *)(g_MenuPresetCursor * 2 + (who * 8 + g_ChrPlacePresetRecords) + 3);
    if (kind == 3) {
        u8 *q;

        ret = val;
        s = ret - 0xB6;
        q = (u8 *)(s * 12 + g_ChrSpellRows[0]);
        s = q[-8];
        if (*(u8 *)((who << 6) + g_ChrRecords + 0x22) == 0x88 || *(u8 *)((who << 6) + g_ChrRecords + 0x23) == 0x88) {
            s = (s + 1) >> 1;
        }
        s = (*(u16 *)((who << 6) + g_ChrRecords + 4) < s) << 2;
    } else if (kind == 4) {
        ret = val;
        s = (ChrItemHeldCheck((u16)who, ret, 0) == 0) << 2;
    } else {
        ret = g_MenuPresetKindIcons[kind];
        s = 0;
    }
    font_target_set(((n + 1) << 10) + 0x801CDB00);
    if (g_MenuPage == 9) {
        s = font_col_set(s & 0xFF);
        font_string_proportional_put(ret | 0x100);
        font_col_set(s & 0xFF);
        if (who == sel) {
            s = 0;
            MenuIconsClear(0x10, n * 3 + 2, 5, 1);
        } else {
            s = 0x10000000;
        }
        IconCellSet(0xC, n * 3 + 2, ret, s);
    } else if (who == sel) {
        if (g_MenuPage != 0xE) {
            list = g_MenuIconBytes;
        } else {
            list = g_MenuPresetKindIcons;
        }
        for (i = 0; i < 6; i++) {
            IconCellSet(i * 4 + 0xC, n * 3 + 2, list[i], 0);
        }
    } else {
        s = font_col_set(s & 0xFF);
        font_string_proportional_put(ret | 0x100);
        font_col_set(s & 0xFF);
        IconCellSet(0xC, n * 3 + 2, ret, 0x10000000);
    }
    return ret;
}

/* func_80071170 */
void MenuPresetSet(s32 chr_id, s32 kind, s32 number)
{
  s32 record_offset;
  record_offset = chr_id * 8;
  *(((u8 *) ((g_MenuPresetCursor * 2) + (record_offset + g_ChrPlacePresetRecords))) + 0x2) = kind;
  *((u8 *) (((u8 *) ((g_MenuPresetCursor * 2) + (record_offset + g_ChrPlacePresetRecords))) + 0x3)) = number;
}
/* MenuPresetOpen -- the set-up of the field menu's command set-up page:
   MenuPresetStep (phase 9) calls it once, on its first frame. No RNG draw;
   the frames are MenuLineUpMove's eight.

   - sprite_run_off(0x87, 8) frees sprite slots 0x87..0x8E.
   - Actors 5 and 6 of g_MenuRecords (the more-above / more-below marks) get
     x = g_MapViewX + 0x120; render entry 2 of g_MenuRenderEntries (it draws slice 0
     of g_MenuShapes, the cursor highlight) gets x = g_MapViewX + 0x80, and
     that slice's right x extents (+0xC, +0x10) become 0x7F and its lower y
     extents (+0x12, +0x16) 0xF: a 0x80 x 0x10 bar. (g_MapViewX /
     g_MapViewY are the menu origin.)
   - The party is seated in a column. Display order i picks party row
     k = g_MenuMemberPlaces[(i + 1) % 6] (rows 0, 2, 3, 4, 5, 1). An empty row (id
     0) is skipped. A real member goes to x = g_MapViewX + 0x48,
     y = g_MapViewY + 0x26 + n * 0x18, n counting the real members seated
     so far. The guest (id 0xC) keeps the position its actor already has
     (+6, +0xA) and has the actor's +4 (e.h0, the drawn word) cleared.
   - MenuLineUpMove(xs, ys) clears and redraws the page and slides the
     actors there. It reads only the entries of rows whose id is not 0, the
     ones filled here.

   Shape: MenuBtlPlacesOpen's (menu_places.c, the formation editor's set-up) for
   the loop -- s32 k and id, the 0xC arm through the indexed row. What the
   bytes pin here is that the target keeps BOTH small constants in the
   loop: the 0x2AAAAAAB of `% 6` (lui in the back branch's delay slot) and
   the 0xC of the id test. loop.c moves a life-1, savings-1 constant only
   while the loop has no more real insns than a threshold that starts at
   58 and falls by 3 with every movable it moves; the multiplier is its
   third movable (52) and the 0xC its fifth (49), so the loop had 53 or
   more insns when the loop pass saw it. It has exactly 53 when the two
   positions are computed IN the loop, the y from a row counter. With x
   and y as running locals set before the loop (`y += 0x18`) it has 45 and
   both constants are hoisted (57/79); with only y running, 47 and the
   multiplier is hoisted (68/79). The y term is `g_MapViewY + 0x26 +
   n * 0x18` in that order (0x26 last: 67/79); n is an int (u8 / s16:
   61/79); xs / ys are s16 (u16: 67/79). The 0xC arm's three accesses are
   members of one indexed record -- a cast store for +4 un-hoists the two
   table loads (50/79), a row pointer local is 62/79. */
/* func_800711A8 */
void MenuPresetOpen(void)
{
    s16 xs[6];
    s16 ys[6];
    s32 i;
    s32 k;
    s32 id;
    s32 n;

    sprite_run_off(0x87, 8);
    ((MapRecord *)g_MenuRecords)[5].sprite.x = g_MapViewX + 0x120;
    ((MapRecord *)g_MenuRecords)[6].sprite.x = g_MapViewX + 0x120;
    ((SpriteEntry *)g_MenuRenderEntries)[2].x = g_MapViewX + 0x80;
    ((SpritePiece *)g_MenuShapes)->x1 = 0x7F;
    ((SpritePiece *)g_MenuShapes)->x2 = 0x7F;
    ((SpritePiece *)g_MenuShapes)->y2 = 0xF;
    ((SpritePiece *)g_MenuShapes)->y3 = 0xF;
    n = 0;
    for (i = 0; i < 6; i++) {
        k = g_MenuMemberPlaces[(i + 1) % 6];
        id = ((u16 *)g_PartyIds)[k];
        if (id != 0) {
            if (id != 0xC) {
                xs[k] = g_MapViewX + 0x48;
                ys[k] = g_MapViewY + 0x26 + n * 0x18;
                n++;
            } else {
                xs[k] = ((MapRecord *)g_MapPartyRecords)[k].sprite.x;
                ys[k] = ((MapRecord *)g_MapPartyRecords)[k].sprite.y;
                AS(s16, ((MapRecord *)g_MapPartyRecords)[k].sprite.on) = 0;
            }
        }
    }
    MenuLineUpMove(xs, ys);
}

/* func_800712E4 */
void MenuPresetClose(void) {
    s32 i;
    s32 record;
    s32 party_id;
    u16 chr_id;
    s32 k;

    ((MapRecord *)g_MenuRecords)[5].sprite.y = (s16) (g_MapViewY + 0x98);
    ((MapRecord *)g_MenuRecords)[6].sprite.y = (s16) (g_MapViewY + 0xC8);
    ((MapRecord *)g_MenuRecords)[1].sprite.on = 0;
    ((MapRecord *)g_MenuRecords)[2].sprite.on = 0;
    ((MapRecord *)g_MenuRecords)[4].sprite.on = 0;
    sprite_run_off(0x87, 8);
    MenuIconsClear(0xC, 2, 1, 5);
    MenuFrameStep();
    MenuIconBytesSet(0, 0);
    MenuIconBytesDisp(0xFFFFU, 1, 2, 0);
    MenuPageDisp(0xFFFFU);
    MenuLineUpSpotsMove();

    i = 0;
    k = 0xC;
    record = g_MapPartyRecords;
    party_id = g_PartyIds;

loop_1:
    chr_id = (*(u16 *)party_id);
    if (chr_id == k) {
        ((MapRecord *)record)->sprite.on = chr_id;
    } else {
        record += 0x3C;
        i += 1;
        party_id += 2;
        if (i < 6) {
            goto loop_1;
        }
    }
}
/* load_hazard_nop: MenuPresetKindStep */

/* MenuPresetKindStep -- menu phase 0xE, one frame: the KIND ROW of a member's
   command set-up page (entry 14 of the phase handler table that starts at
   battle_phase_pre_table and runs on into battle_init_table; phase 9
   MenuPresetStep is the slot step that opens it, MenuPresetSpellStep / _05 =
   phases 0xF / 0x10 the spell and item pickers it opens). who = the party
   entry of slot g_MenuPresetMemberCursor (g_PartyIds[g_MenuMemberPlaces[slot]]). The cursor
   g_MenuPresetChoiceCursor walks the six-entry kind list g_MenuPresetKindIcons (F0 ED E9 EA EC EB,
   none empty), and the cursor INDEX is the kind.
   Pad (g_MenuPadRecord[2] repeat, [1] new press):
   - 0xA000 moves the cursor through MenuCursorMove(cursor, g_MenuPresetKindIcons,
     bits); SE 1, redraw.
   - cancel (0x130, or the g_MenuBackOutLatch & g_MenuBackOutPattern auto-cancel): g_MenuBackOutLatch
     = 1 for bit 0x100 / 2 for bit 0x20, g_MenuBackOutPattern = 0x30B30B30, back to
     phase 9: actor 4's flag cleared, actor 2 to state 0xA (MapAnimationSet);
     SE 3, phase 9's redraw requested.
   - 0x40 (SE 2 first), by the cursor: 3 -> ChrSpellAnyCheck(who) != 0 opens
     phase 0xF (actor 4's flag cleared, its redraw requested), == 0 only
     requests SE 4; 4 -> the same with ChrItemAnyCheck(who) and phase 0x10;
     any other kind (0, 1, 2, 5) is written at once -- MenuPresetSet(who,
     cursor, 0), value 0 -- and the page returns to phase 9 like the
     cancel, with SE 2 standing.
   g_MenuSoundRequest is the SE request, g_MenuRedrawRequest = g_MenuPage the redraw request.
   Redraw (g_MenuRedrawRequest == 0xE): MenuPresetDisp(who) draws the page (it runs
   one frame), g_MenuRedrawRequest = 0. Every call ends with
   MenuLineUpDisp(slot, 1, 0, 0). No RNG of its own.
   Decisive shapes: the cursor is read from g_MenuPresetChoiceCursor at each of its three
   uses, with NO local -- the two tests share one zero-extended load ($s0,
   which also supplies the 4 of the item arm's refusal through cse), while
   the call argument's is a fresh lhu; a u16 local adds an andi copy
   (118/122), an s32 local turns the argument into `move a1,s0` (121/122).
   The `g_MenuRedrawRequest = g_MenuPage` after MapAnimationSet in the last arm needs
   load_hazard_nop (lhu, then a gp-relative sh of the loaded register:
   116/122 without). who is an int, as in the twins. */
/* func_800713B8 */
void MenuPresetKindStep(void)
{
    s32 who;
    u16 pad;

    who = *(u16 *)(g_MenuMemberPlaces[g_MenuPresetMemberCursor] * 2 + g_PartyIds);
    pad = g_MenuPadRecord[2];
    if (pad & 0xA000) {
        g_MenuPresetChoiceCursor = MenuCursorMove(g_MenuPresetChoiceCursor, g_MenuPresetKindIcons, pad & 0xA000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuPage = 9;
        AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
        MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
        g_MenuSoundRequest = 3;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        g_MenuSoundRequest = 2;
        if (g_MenuPresetChoiceCursor == 3) {
            if (ChrSpellAnyCheck(who) != 0) {
                g_MenuPage = 0xF;
                AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
                g_MenuRedrawRequest = g_MenuPage;
            } else {
                g_MenuSoundRequest = 4;
            }
        } else if (g_MenuPresetChoiceCursor == 4) {
            if (ChrItemAnyCheck(who) != 0) {
                g_MenuPage = 0x10;
                AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
                g_MenuRedrawRequest = g_MenuPage;
            } else {
                g_MenuSoundRequest = 4;
            }
        } else {
            MenuPresetSet(who, g_MenuPresetChoiceCursor, 0);
            g_MenuPage = 9;
            AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
            g_MenuRedrawRequest = g_MenuPage;
        }
    }
    if (g_MenuRedrawRequest == 0xE) {
        MenuPresetDisp(who);
        g_MenuRedrawRequest = 0;
    }
    MenuLineUpDisp(g_MenuPresetMemberCursor, 1, 0, 0);
}

/* far_syms: g_MenuPadRecord (target reaches it %hi/%lo) */
/* far_syms: g_MenuIconBytes (target reaches it %hi/%lo) */

/* MenuPresetSpellStep — menu phase 0xF, one frame: the SPELL picker of a
   member's command set-up page (entry 15 of the phase handler table that
   starts at battle_phase_pre_table and runs on into battle_init_table;
   MenuPresetKindStep = phase 0xE the kind row, MenuPresetItemStep = phase 0x10 the
   item picker). who = the party entry of slot g_MenuPresetMemberCursor
   (g_PartyIds[g_MenuMemberPlaces[slot]]); list = who's 64-byte record + 0x2A +
   page * 4 — two pages of four spell bytes, page = g_MenuPresetHalf; the cursor
   g_MenuPresetChoiceCursor indexes g_MenuIconBytes, the page as MenuPresetDisp loaded it.
   Pad (g_MenuPadRecord[2] repeat, [1] new press): 0xA000 moves the cursor
   through MenuCursorMove; 0x1000 / 0x4000 select page 0 / 1 (redraw only on
   a change); cancel (0x130, or the g_MenuBackOutLatch & g_MenuBackOutPattern auto-cancel)
   arms g_MenuBackOutPattern = 0x30B30B30 and goes back to phase 0xE with cursor 3;
   0x40 asks ChrSpellUseGet(who, (u8)(entry + 0x4A), 4): >= 0 writes kind 3
   + the entry into who's command slot (MenuPresetSet) and returns to phase
   9 with cursor 3, < 0 only requests SE 4. g_MenuSoundRequest is the SE request
   (1 cursor, 2 confirm, 3 cancel, 4 refuse), g_MenuRedrawRequest = g_MenuPage the
   redraw request.
   Redraw (g_MenuRedrawRequest == 0xF): a page with no spell flips — page 0 to
   page 1 only when page 1 has one, page 1 to page 0 always; actor 6's
   flag is set on page 0 when page 1 has a spell (more below), actor 5's on
   page 1 when page 0 has one (more above); the page's four bytes are
   copied to g_MenuIconBytes, a cursor on an empty entry is moved by
   MenuCursorMove(cursor, g_MenuIconBytes, 0x8000), and MenuPresetDisp(who) draws
   the page (it runs one frame). Every call ends with
   MenuLineUpDisp(slot, 1, 0, 0). No RNG of its own.
   Decisive shapes: who is an int (u16 keeps re-masking it and takes a
   fourth saved register, 197/249); MenuPresetSet's value parameters must
   be int-width in the TU (an s8 prototype loads the entry with lb). */
/* func_800715A0 */
void MenuPresetSpellStep(void)
{
    s32 who;
    u8 *list;
    u16 pad;
    s32 i;

    who = *(u16 *)(g_MenuMemberPlaces[g_MenuPresetMemberCursor] * 2 + g_PartyIds);
    if (g_MenuPresetHalf == 0) {
        list = (u8 *)(g_ChrRecords + (who << 6)) + 0x2A;
    } else {
        list = (u8 *)(g_ChrRecords + (who << 6)) + 0x2E;
    }
    pad = g_MenuPadRecord[2];
    if (pad & 0xA000) {
        g_MenuPresetChoiceCursor = MenuCursorMove(g_MenuPresetChoiceCursor, g_MenuIconBytes, pad & 0xA000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad & 0x1000) {
        if (g_MenuPresetHalf != 0) {
            g_MenuPresetHalf = 0;
            list = (u8 *)(g_ChrRecords + (who << 6)) + 0x2A;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if (pad & 0x4000) {
        if (g_MenuPresetHalf != 1) {
            g_MenuPresetHalf = 1;
            list = (u8 *)(g_ChrRecords + (who << 6)) + 0x2E;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuPage = 0xE;
        g_MenuPresetChoiceCursor = 3;
        g_MenuSoundRequest = 3;
        AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        if (ChrSpellUseGet(who, (u8)(g_MenuIconBytes[g_MenuPresetChoiceCursor] + 0x4A), 4) >= 0) {
            MenuPresetSet(who, 3, g_MenuIconBytes[g_MenuPresetChoiceCursor]);
            g_MenuPage = 9;
            g_MenuPresetChoiceCursor = 3;
            AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
            g_MenuSoundRequest = 2;
            g_MenuRedrawRequest = g_MenuPage;
        } else {
            g_MenuSoundRequest = 4;
        }
    }
    if (g_MenuRedrawRequest == 0xF) {
        AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
        for (i = 0; i < 4; i++) {
            if (list[i] != 0) {
                break;
            }
        }
        if (i >= 4) {
            if (g_MenuPresetHalf == 0) {
                for (i = 4; i < 8; i++) {
                    if (list[i] != 0) {
                        break;
                    }
                }
                if (i < 8) {
                    g_MenuPresetHalf = 1;
                    list = (u8 *)(g_ChrRecords + (who << 6)) + 0x2E;
                }
            } else {
                g_MenuPresetHalf = 0;
                list = (u8 *)(g_ChrRecords + (who << 6)) + 0x2A;
            }
        }
        if (g_MenuPresetHalf == 0) {
            for (i = 4; i < 8; i++) {
                if (list[i] != 0) {
                    AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 1;
                    break;
                }
            }
        } else {
            for (i = -4; i < 0; i++) {
                if (list[i] != 0) {
                    AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 1;
                    break;
                }
            }
        }
        mem_move((s32 *)g_MenuIconBytes, (s32 *)list, 4);
        if (g_MenuIconBytes[g_MenuPresetChoiceCursor] == 0) {
            g_MenuPresetChoiceCursor = MenuCursorMove(g_MenuPresetChoiceCursor, g_MenuIconBytes, 0x8000);
        }
        MenuPresetDisp(who);
        g_MenuRedrawRequest = 0;
    }
    MenuLineUpDisp(g_MenuPresetMemberCursor, 1, 0, 0);
}
/* MenuPresetItemStep — menu phase 0x10, one frame: the ITEM picker of a
   member's command set-up page (entry 16 of the phase handler table; the
   twin of MenuPresetSpellStep, the spell picker). list = who's 64-byte record +
   0x1E + page * 6 — two pages of six item bytes (+0x1E..+0x23 and
   +0x24..+0x29), page = g_MenuPresetHalf; the cursor g_MenuPresetChoiceCursor indexes
   g_MenuIconBytes, the page as MenuPresetDisp loaded it.
   Pad as in MenuPresetSpellStep (0xA000 cursor through MenuCursorMove, 0x1000 /
   0x4000 page 0 / 1, redraw only on a change); cancel (0x130 or the
   auto-cancel) goes back to phase 0xE with cursor 4; 0x40 asks
   ChrItemUseGet(who, entry, 4): >= 0 writes kind 4 + the entry into who's
   command slot (MenuPresetSet) and returns to phase 9 with cursor 4, < 0
   only requests SE 4.
   Redraw (g_MenuRedrawRequest == 0x10): on page 0 with all six entries empty, page
   1 is taken when its FIRST byte (+0x24) is set; on page 1 with a zero
   first byte, back to page 0 — only the first entry of page 1 is tested,
   where MenuPresetSpellStep scans all four. Actor 6's flag is set on page 0
   when any of page 1's six is set (more below), actor 5's on page 1 when
   any of page 0's is (more above); six bytes are copied to g_MenuIconBytes, a
   cursor on an empty entry is moved by MenuCursorMove(cursor, g_MenuIconBytes,
   0x8000), and MenuPresetDisp(who) draws the page (it runs one frame).
   Every call ends with MenuLineUpDisp(slot, 1, 0, 0). No RNG of its own.
   Decisive shapes: MenuPresetSpellStep's, carried over unchanged; the page
   tests spell the record address out twice, (who << 6) + g_ChrRecords +
   0x24 in the test and again in the assignment (a held base pointer loses
   the add in the branch delay slot, 233/243). */
/* func_80071984 */
void MenuPresetItemStep(void)
{
    s32 who;
    u8 *list;
    u16 pad;
    s32 i;

    who = *(u16 *)(g_MenuMemberPlaces[g_MenuPresetMemberCursor] * 2 + g_PartyIds);
    if (g_MenuPresetHalf == 0) {
        list = (u8 *)(g_ChrRecords + (who << 6)) + 0x1E;
    } else {
        list = (u8 *)(g_ChrRecords + (who << 6)) + 0x24;
    }
    pad = g_MenuPadRecord[2];
    if (pad & 0xA000) {
        g_MenuPresetChoiceCursor = MenuCursorMove(g_MenuPresetChoiceCursor, g_MenuIconBytes, pad & 0xA000);
        g_MenuSoundRequest = 1;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (pad & 0x1000) {
        if (g_MenuPresetHalf != 0) {
            g_MenuPresetHalf = 0;
            list = (u8 *)(g_ChrRecords + (who << 6)) + 0x1E;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if (pad & 0x4000) {
        if (g_MenuPresetHalf != 1) {
            g_MenuPresetHalf = 1;
            list = (u8 *)(g_ChrRecords + (who << 6)) + 0x24;
            g_MenuRedrawRequest = g_MenuPage;
        }
        g_MenuSoundRequest = 1;
    } else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
        if (g_MenuPadRecord[1] & 0x100) {
            g_MenuBackOutLatch = 1;
        } else if (g_MenuPadRecord[1] & 0x20) {
            g_MenuBackOutLatch = 2;
        }
        g_MenuBackOutPattern = 0x30B30B30;
        g_MenuPage = 0xE;
        g_MenuPresetChoiceCursor = 4;
        g_MenuSoundRequest = 3;
        AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
        g_MenuRedrawRequest = g_MenuPage;
    } else if (g_MenuPadRecord[1] & 0x40) {
        if (ChrItemUseGet(who, g_MenuIconBytes[g_MenuPresetChoiceCursor], 4) >= 0) {
            MenuPresetSet(who, 4, g_MenuIconBytes[g_MenuPresetChoiceCursor]);
            g_MenuPage = 9;
            g_MenuPresetChoiceCursor = 4;
            AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MenuRecords)[4].sprite.on) = 0;
            MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
            g_MenuSoundRequest = 2;
            g_MenuRedrawRequest = g_MenuPage;
        } else {
            g_MenuSoundRequest = 4;
        }
    }
    if (g_MenuRedrawRequest == 0x10) {
        AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 0;
        AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 0;
        if (g_MenuPresetHalf == 0) {
            for (i = 0; i < 6; i++) {
                if (list[i] != 0) {
                    break;
                }
            }
            if (i >= 6) {
                if (*(u8 *)((who << 6) + g_ChrRecords + 0x24) != 0) { g_MenuPresetHalf = 1; list = (u8 *)((who << 6) + g_ChrRecords + 0x24); }
            }
        } else {
            if (*(u8 *)((who << 6) + g_ChrRecords + 0x24) == 0) { g_MenuPresetHalf = 0; list = (u8 *)((who << 6) + g_ChrRecords + 0x1E); }
        }
        if (g_MenuPresetHalf == 0) {
            for (i = 6; i < 12; i++) {
                if (list[i] != 0) {
                    AS(s16, ((MapRecord *)g_MenuRecords)[6].sprite.on) = 1;
                    break;
                }
            }
        } else {
            for (i = -6; i < 0; i++) {
                if (list[i] != 0) {
                    AS(s16, ((MapRecord *)g_MenuRecords)[5].sprite.on) = 1;
                    break;
                }
            }
        }
        mem_move((s32 *)g_MenuIconBytes, (s32 *)list, 6);
        if (g_MenuIconBytes[g_MenuPresetChoiceCursor] == 0) {
            g_MenuPresetChoiceCursor = MenuCursorMove(g_MenuPresetChoiceCursor, g_MenuIconBytes, 0x8000);
        }
        MenuPresetDisp(who);
        g_MenuRedrawRequest = 0;
    }
    MenuLineUpDisp(g_MenuPresetMemberCursor, 1, 0, 0);
}

/* far_syms: g_MenuBtlPlacesXCells g_MenuBtlPlacesYCells (sized so cc1 emits one unsplit `la`; target reaches both %hi/%lo) */
extern u16 g_MenuBtlPlacesState[2];
extern u8 g_MenuBtlPlacesXCells[6];               /* formation x cell per party slot */
extern u8 g_MenuBtlPlacesYCells[6];               /* formation y cell per party slot */
void MenuBtlPlacesOpen(void);
void MenuBtlPlacesClose(void);
void MenuCommandsSet(s32, u16);
void MenuCommandsDisp(s32);
void MenuBtlPlacesDisp(s32, u32);

/* MenuBtlPlacesStep — party formation editor, one frame (its redraw keys on phase 0xA).
 * g_MenuBtlPlacesState[0] is the mode (0 = open: MenuBtlPlacesOpen sets up, mode 1 slot 1;
 * 1 = just opened, promoted to 2 by a direction or Cross; 2 = moving a
 * member; 3 = confirm prompt),
 * g_MenuBtlPlacesState[1] the cursor (the member slot in mode 2, the prompt row 0..2
 * in mode 3). In mode 2 Left/Right step member k = g_MenuMemberPlaces[slot]'s x
 * cell g_MenuBtlPlacesXCells[k] within 0x1A..0x22 (down while >= 0x1B, up while
 * < 0x22) and Up/Down its y cell g_MenuBtlPlacesYCells[k] within 0xE..0x16 (down
 * while >= 0xF, up while < 0x16), repositioning its 60-byte display record at
 * g_MapPartyRecords + k*60 (+6 = g_MapViewX + (x-3)*8; +0xA = g_MapViewY +
 * (y-6)*8, +8 = that + 0xE0); in mode 3 Up/Down wrap the row. Cross
 * advances to the next slot (mode 3 once the slot's party entry
 * g_PartyIds[] is empty or the slot reaches 6); on the prompt row 1 goes
 * back to mode 2, row 0 writes every live member's cells into its
 * g_ChrPlacePresetRecords record (w*8 +0/+1; skipping w 0 and 0xC) and closes, row 2
 * closes (MenuBtlPlacesClose, mode 0). Cancel (0x130, or the g_MenuBackOutLatch &
 * g_MenuBackOutPattern auto-cancel) arms g_MenuBackOutPattern = 0x30B30B30 and leaves mode 2
 * for the prompt or closes. g_MenuSoundRequest is the SE request (1 cursor,
 * 2 confirm, 3 cancel), g_MenuRedrawRequest = g_MenuPage the redraw request; the
 * redraw (F5A == 0xA) repaints the panels and sets actor flag
 * g_MenuRecords+0x40 while mode < 3. Decisive shapes: the arrays indexed in
 * place (no held element pointer) with an s32 k — a u8 k or a held
 * pointer swaps k/pointer registers in all four blocks (316); the Up
 * wrap's new value a distinct variable from the cursor read (else the
 * read and result share a register, 344); the two arrays sized with a
 * far_syms marker (one `la`, 287 -> 295). No RNG. */
/* func_80071D50 */
void MenuBtlPlacesStep(void)
{
  u16 pad;
  s32 k;
  u16 v;
  s32 i;
  u16 w;
  if (g_MenuBtlPlacesState[0] == 0)
  {
    MenuBtlPlacesOpen();
    g_MenuBtlPlacesState[1] = 1;
    g_MenuBtlPlacesState[0] = 1;
    g_MenuRedrawRequest = g_MenuPage;
  }
  pad = g_MenuPadRecord[2];
  if (pad & 0x8000)
  {
    if (g_MenuBtlPlacesState[0] == 1)
    {
      g_MenuBtlPlacesState[0]++;
    }
    if (g_MenuBtlPlacesState[0] == 2)
    {
      k = g_MenuMemberPlaces[g_MenuBtlPlacesState[1]];
      if (g_MenuBtlPlacesXCells[k] >= 0x1B)
      {
        g_MenuBtlPlacesXCells[k] = g_MenuBtlPlacesXCells[k] - 1;
        *(u16 *) ((k * 60 + g_MapPartyRecords) + 6) = g_MapViewX + (g_MenuBtlPlacesXCells[k] - 3) * 8;
      }
      g_MenuSoundRequest = 1;
    }
  }
  else if (pad & 0x2000)
  {
    if (g_MenuBtlPlacesState[0] == 1)
    {
      g_MenuBtlPlacesState[0]++;
    }
    if (g_MenuBtlPlacesState[0] == 2)
    {
      k = g_MenuMemberPlaces[g_MenuBtlPlacesState[1]];
      if (g_MenuBtlPlacesXCells[k] < 0x22)
      {
        g_MenuBtlPlacesXCells[k] = g_MenuBtlPlacesXCells[k] + 1;
        *(u16 *) ((k * 60 + g_MapPartyRecords) + 6) = g_MapViewX + (g_MenuBtlPlacesXCells[k] - 3) * 8;
      }
    }
    g_MenuSoundRequest = 1;
  }
  else if (pad & 0x1000)
  {
    if (g_MenuBtlPlacesState[0] == 1)
    {
      g_MenuBtlPlacesState[0]++;
    }
    if (g_MenuBtlPlacesState[0] == 2)
    {
      k = g_MenuMemberPlaces[g_MenuBtlPlacesState[1]];
      if (g_MenuBtlPlacesYCells[k] >= 0xF)
      {
        g_MenuBtlPlacesYCells[k] = g_MenuBtlPlacesYCells[k] - 1;
        *(u16 *) ((k * 60 + g_MapPartyRecords) + 0xA) = g_MapViewY + (g_MenuBtlPlacesYCells[k] - 6) * 8;
        *(u16 *) ((k * 60 + g_MapPartyRecords) + 8) = *(u16 *) ((k * 60 + g_MapPartyRecords) + 0xA) + 0xE0;
      }
    }
    else
    {
      if (g_MenuBtlPlacesState[1] == 0)
      {
        v = 2;
      }
      else
      {
        v = g_MenuBtlPlacesState[1] - 1;
      }
      g_MenuBtlPlacesState[1] = v;
      g_MenuRedrawRequest = g_MenuPage;
    }
    g_MenuSoundRequest = 1;
  }
  else if (pad & 0x4000)
  {
    if (g_MenuBtlPlacesState[0] == 1)
    {
      g_MenuBtlPlacesState[0]++;
    }
    if (g_MenuBtlPlacesState[0] == 2)
    {
      k = g_MenuMemberPlaces[g_MenuBtlPlacesState[1]];
      if (g_MenuBtlPlacesYCells[k] < 0x16)
      {
        g_MenuBtlPlacesYCells[k] = g_MenuBtlPlacesYCells[k] + 1;
        *(u16 *) ((k * 60 + g_MapPartyRecords) + 0xA) = g_MapViewY + (g_MenuBtlPlacesYCells[k] - 6) * 8;
        *(u16 *) ((k * 60 + g_MapPartyRecords) + 8) = *(u16 *) ((k * 60 + g_MapPartyRecords) + 0xA) + 0xE0;
      }
    }
    else
    {
      if (g_MenuBtlPlacesState[1] < 2)
      {
        v = g_MenuBtlPlacesState[1] + 1;
      }
      else
      {
        v = 0;
      }
      g_MenuBtlPlacesState[1] = v;
      g_MenuRedrawRequest = g_MenuPage;
    }
    g_MenuSoundRequest = 1;
  }
  else if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern))
  {
    if (g_MenuPadRecord[1] & 0x100)
    {
      g_MenuBackOutLatch = 1;
    }
    else if (g_MenuPadRecord[1] & 0x20)
    {
      g_MenuBackOutLatch = 2;
    }
    g_MenuBackOutPattern = 0x30B30B30;
    if (g_MenuBtlPlacesState[0] == 2)
    {
      g_MenuBtlPlacesState[0]++;
      g_MenuBtlPlacesState[1] = 0;
    }
    else
    {
      MenuBtlPlacesClose();
      g_MenuBtlPlacesState[0] = 0;
    }
    g_MenuSoundRequest = 3;
    g_MenuRedrawRequest = g_MenuPage;
  }
  else if (g_MenuPadRecord[1] & 0x40)
  {
    if (g_MenuBtlPlacesState[0] == 1)
    {
      g_MenuBtlPlacesState[0]++;
    }
    if (g_MenuBtlPlacesState[0] == 2)
    {
      g_MenuBtlPlacesState[1]++;
      if ((*(u16 *) (g_MenuMemberPlaces[g_MenuBtlPlacesState[1]] * 2 + g_PartyIds) == 0) || (g_MenuBtlPlacesState[1] >= 6))
      {
        g_MenuBtlPlacesState[0]++;
        g_MenuBtlPlacesState[1] = 0;
      }
    }
    else if (g_MenuBtlPlacesState[1] == 1)
    {
      g_MenuBtlPlacesState[0] = 2;
      g_MenuBtlPlacesState[1] = 1;
    }
    else
    {
      if (g_MenuBtlPlacesState[1] == 0)
      {
        for (i = 0; i < 6; i++)
        {
          w = *(u16 *) (i * 2 + g_PartyIds);
          if ((w != 0) && (w != 0xC))
          {
            *(u8 *) (w * 8 + g_ChrPlacePresetRecords) = g_MenuBtlPlacesXCells[i];
            *(u8 *) (w * 8 + g_ChrPlacePresetRecords + 1) = g_MenuBtlPlacesYCells[i];
          }
        }
      }
      MenuBtlPlacesClose();
      g_MenuBtlPlacesState[0] = 0;
    }
    g_MenuSoundRequest = 2;
    g_MenuRedrawRequest = g_MenuPage;
  }
  if (g_MenuRedrawRequest == 0xA)
  {
    MenuCommandsSet(0, g_MenuPage);
    MenuCommandsDisp(0);
    MenuIconBytesSet(0xC, g_MenuPage);
    if (g_MenuBtlPlacesState[0] < 3)
    {
      MenuIconBytesDisp(g_MenuBtlPlacesState[1], 1, 1, 0xC);
    }
    else
    {
      MenuIconBytesDisp(g_MenuMemberCursor, 1, 1, 0xC);
    }
    MenuBtlPlacesDisp(g_MenuBtlPlacesState[0], g_MenuBtlPlacesState[1]);
    if (g_MenuBtlPlacesState[0] < 3)
    {
      ((MapRecord *)g_MenuRecords)[1].sprite.on = 1;
    }
    else
    {
      ((MapRecord *)g_MenuRecords)[1].sprite.on = 0;
    }
    g_MenuRedrawRequest = 0;
  }
  if (g_MenuBtlPlacesState[0] < 3)
  {
    MenuLineUpDisp(g_MenuBtlPlacesState[1], 1, 0, 0);
  }
  else
  {
    MenuLineUpDisp(0, 0, 0, 0);
  }
}
