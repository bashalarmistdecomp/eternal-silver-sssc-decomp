/* Lunar SSSC — menu_run: the menu: its run, set-up, open and close, frame
   and records, cursors and line-up, and what its pages write (page display,
   status, money, commands and icon bytes, notices, answers), the pages' run
   and its hang. The first split had it in overworld.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_ChrItemRows (target reaches it %hi/%lo) */
/* far_syms: g_ChrItemRows */
/* far_syms: g_MenuPadRecord (target reaches it %hi/%lo) */
/* far_syms: g_MenuIconBytes (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "chr.h"
#include "map.h"
#include "sprite.h"
#include "menu.h"
extern u8 g_MenuCommands[8];
extern s32 g_SpriteList;
extern s32 g_MenuBackOutLatch;
extern u32 g_MenuBackOutPattern;
extern s32 g_PartyIds;
extern s32 g_MapPartyRecords;
extern u16 g_MapViewX;
extern u16 g_MenuPadRecord[];
extern u16 g_MapViewY;
s32 font_string_proportional_put(s32);
extern u8 g_MenuIconBytes[];
extern s32 g_ChrRecords;
extern s32 g_MenuPage;
/* far_syms: g_MapFadeState */
extern u16 g_MenuMemberPickCursor;
extern s32 g_ChrItemRows;
s32 mem_clear(s32 *, s32);
s32 StoryBitCheck(u32);
extern u16 g_MapIdleCount;
extern u16 g_MapPartyBlinkCount;
extern u8 g_ScriptNumber;
extern u8 g_MapFadeState;
extern s32 g_MapLayersMask;
extern s32 g_MenuKeptIds;
extern s32 g_MenuRequest;
void MenuOpen(void);  /* TU defines this below */
void MenuPagesRun(void);  /* TU defines this below */
void MenuClose(void);  /* TU defines this below */

/* Runs the field menu, opened on page `page`, until it closes, then acts
   on the request it left. Callers: MapPartyStep (the field's button
   handler) with 2, the member's command list; ChrItemGive (the
   inventory add with nobody having room) and ScriptLabelRun (the event
   runner, when ChrItemWaitingCheck() is non-zero after an event) with 0x16,
   the make-room page.

   Open: g_MapIdleCount (the field's idle counter) = 0. The six party ids
   (u16 g_PartyIds[0..5]) are saved in the 12-byte buffer g_MenuKeptIds, which
   is cleared first. A slot whose id is 0x1F or more is taken OUT of the
   party for the menu's duration: its id becomes 0, and its field actor
   (g_MapPartyRecords[i], 0x3C records) gets byte +1 = 1 and bit 0x8000 of its
   +0xE word. g_MapPartyBlinkCount is saved and zeroed, the page g_MenuPage =
   page & 0xFFFF.

   Run: MenuOpen() (the party screen's opening), MenuPagesRun() (the
   menu's page loop: one page call and one frame per pass until the page
   g_MenuPage is 0), MenuClose() (the closing).

   Close: all six ids come back from the buffer and every actor's +1 byte
   and 0x8000 bit are cleared -- for all six slots, not only the ones that
   were hidden. g_MapPartyBlinkCount is restored.

   The request g_MenuRequest the menu left (0 = none):
     1..4    GameEnter((u16)request) -- enter the field from a load
             result (the save pages' Load);
     5       MapRow2Change();
     6..0x50 nothing;
     >= 0x51 a field event: if event flag 0x29E is set, flag 0x397 clear
             and g_ScriptNumber == 0x2C, event 0x46 runs first
             (ScriptLabelRun(0x46)); then g_MapFadeState = 2 (the fade variant
             MapFadeIn picks, MapFadePartyIn), the transition out
             MapFadeOut(g_MapLayersMask | 0x130, 0x19, -1, 1, 1),
             ScriptChange(0x64) and ScriptLabelRun((u16)request).
   After any request g_MapPartyBlinkCount = 0 (overriding the restore) and the
   request is cleared. Last, always, StoryBit2CAUpdate().

   It returns nothing ($v0 is StoryBit2CAUpdate's leftover); no caller reads a
   result. Other TUs still declare it `s32 MenuRun(s32)`.

   Shape:
   - The id array is spelled as the TU spells it elsewhere,
     ((u16 *)g_PartyIds)[i]. That makes each access MEM_IN_STRUCT, so the
     scheduler may lift the scalar load of g_MapPartyRecords above the `= 0`
     store (loop 1) and above the restore store (loop 2), which is the
     target's order. Through a plain `u16 *p = (u16 *)(i * 2 + base)` the
     stores are not in-struct, the load stays behind them and each loop is
     one nop longer (116/129).
   - The actor rows are (MapRecord *)(i * 0x3C + g_MapPartyRecords), an integer
     base: index first in the addu, as in MenuLineUpMove.
   - `pad`: the target frame is 0x38 with five outgoing argument words
     (0x18) and three saved registers, which leaves 16 bytes at
     0x18-0x27 that no instruction touches. Without a 9..16-byte local the
     frame is 0x28 (121/129). */
/* func_8006A024 */
void MenuRun(s32 page)
{
    u8 pad[16];     /* MARKED SPELLING (unused local): the target frame holds 16
                       bytes at 0x18-0x27 that no instruction touches -- the
                       original's local(s), declaration unknown */
    s32 i;
    u16 saved;

    g_MapIdleCount = 0;
    mem_clear((s32 *)g_MenuKeptIds, 0xC);
    for (i = 0; i < 6; i++) {
        ((u16 *)g_MenuKeptIds)[i] = ((u16 *)g_PartyIds)[i];
        if (((u16 *)g_PartyIds)[i] >= 0x1F) {
            ((u16 *)g_PartyIds)[i] = 0;
            ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->hold_definition.hold = 1;
            ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.flags |= 0x8000;
        }
    }
    saved = g_MapPartyBlinkCount;
    g_MapPartyBlinkCount = 0;
    g_MenuPage = page & 0xFFFF;
    MenuOpen();
    MenuPagesRun();
    MenuClose();
    for (i = 0; i < 6; i++) {
        ((u16 *)g_PartyIds)[i] = ((u16 *)g_MenuKeptIds)[i];
        ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->hold_definition.hold = 0;
        ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.flags &= 0x7FFF;
    }
    g_MapPartyBlinkCount = saved;
    if (g_MenuRequest != 0) {
        if (g_MenuRequest < 5) {
            GameEnter((u16)g_MenuRequest);
        } else if (g_MenuRequest == 5) {
            MapRow2Change();
        } else if (g_MenuRequest >= 0x51) {
            if (StoryBitCheck(0x29E) != 0 && StoryBitCheck(0x397) == 0 && g_ScriptNumber == 0x2C) {
                ScriptLabelRun(0x46);
            }
            g_MapFadeState = 2;
            MapFadeOut(g_MapLayersMask | 0x130, 0x19, -1, 1, 1);
            ScriptChange(0x64);
            ScriptLabelRun((u16)g_MenuRequest);
        }
        g_MapPartyBlinkCount = 0;
        g_MenuRequest = 0;
    }
    StoryBit2CAUpdate();
}

extern u8 D_80012D34[];
extern u8 D_80012D40[];
extern u8 D_800E4F80[];

s32 mem_move(s32 *, s32 *, s32);                       /* extern */
extern s32 g_MenuNoticeCol8;
extern s32 g_MenuCommandCursor;
extern u16 g_MenuWornItemCursor;
extern u16 g_MenuSpellCursor;
extern u16 g_MenuItemCursor;
extern u16 g_MenuItemListCursor;
extern s32 g_MenuRenderEntries;
extern u16 g_MenuItemListScroll;
extern s32 g_MenuPoseWords;
extern s32 g_MenuFacingWords;
extern u16 g_MenuItemCommandCursor;
extern u16 g_MenuOptionRow;
extern u16 g_MenuCardListRow;
extern s32 g_MenuRequest;
extern u16 g_MenuSoundRequest;
extern s32 g_MenuCardBuffer;
extern u16 g_MenuPage7Cursor;
extern u16 g_MenuMemberCursor;
extern u16 g_MenuRedrawRequest;
extern s32 g_MenuRecords;
extern s32 g_MenuShapes;
extern u16 g_MenuPresetMemberCursor;
extern u16 g_MenuPresetChoiceCursor;
extern u16 g_MenuPresetHalf;
extern s32 g_MenuKeptIds;
extern u16 g_MenuPresetCursor;

/* func_8006A228 */
void MenuInit(void) {
    g_MenuPage = 0;
    g_MenuRequest = 0;
    g_MenuRedrawRequest = 0;
    g_MenuSoundRequest = 0;
    g_MenuMemberCursor = 1;
    g_MenuCommandCursor = 0;
    g_MenuItemListCursor = 0;
    g_MenuItemCursor = 0;
    g_MenuItemListScroll = 0;
    g_MenuItemCommandCursor = 0;
    g_MenuSpellCursor = 0;
    g_MenuWornItemCursor = 0;
    g_MenuPresetHalf = 0;
    g_MenuPresetChoiceCursor = 0;
    g_MenuPresetCursor = 0;
    g_MenuPresetMemberCursor = 1;
    g_MenuPage7Cursor = 0;
    g_MenuCardListRow = 0;
    g_MenuOptionRow = 0;
    g_MenuCardBuffer = 0x801CFB00;
    mem_move((s32 *)D_800E4F80, (s32 *)D_80012D34, 0xC);
    mem_move((s32 *)(D_800E4F80 + 0xC), (s32 *)D_80012D40, 0xB);
    (*(u8 *)((u8 *)&D_800E4F80 + 0x17)) = 0;
    (*(s32 *)((u8 *)&D_800E4F80 + 0x1c)) = 0x2000;
    g_MenuShapes = 0x801CAB00;
    g_MenuRenderEntries = 0x801CAB78;
    g_MenuRecords = 0x801CACE0;
    g_MenuKeptIds = 0x801CAE84;
    g_MenuFacingWords = 0x801CAE90;
    g_MenuPoseWords = 0x801CAEA8;
    g_MenuNoticeCol8 = 0;
}
/* far_syms: g_MoveChrViewFollow D_800B8EEC g_MenuMemberCursor_blk */
extern u16 g_MessageFaceSecond;
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFacePlaces;
extern u8 g_MoveChrViewFollow;
extern const s16 g_MenuLineUpSpotXs[];
extern u16 g_MapPartyKind;
extern s32 g_MapPartyTwins;
extern s32 g_MapLayersMask;
extern u16 g_MapObjectsSpriteBits;
extern s32 D_800E4768[];
extern u16 g_MenuMemberCursor_blk[2];
extern u8 D_800E4E70[];
extern s32 D_800B8EEC;
s32 SoundPlay(u16, s32);
s32 sprite_run_off();
void MenuRecordsInit(void);  /* TU defines this below */
void CellAllClear(s32);
s32 CellAllUpdate(s32);
void MenuTextStringsPut(void);  /* TU defines this below */
s32 layer_col_set(s32, s32, s32);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
void MapObjectsAnimationStep(void);
void MenuFrameStep(void);  /* TU defines this below */
void MapRecordSpriteDataSet(s32, s32);
void MapAnimationStart(s32, s32);
void MapAnimationPoseSet(void *, s32, s32);
void MenuPoseWordsSet(s32);
void MenuPageDisp(s32);
s32 pad_record_init(u16 *, s32, s32, s32);
void MenuMemberCursorAdjust(u16 *cursor, u16 buttons);  /* TU defines this below */
void layer_mix_switch_on(s32);
s32 layer_mix_set(s32, s32);

/* Opens the field menu's party screen: the six party actors slide from
   where they stand to their line-up slots. For each present member
   (g_PartyIds[i] != 0) it takes a 16.16 start position (x, y pairs in
   pos[]) from the actor's screen x/y - or, when g_MapPartyKind == 2, from
   member 0's, after copying that position into the actor and marking it
   (+0 = 1) - and a per-frame step (vel[]) toward slot
   (g_MenuLineUpSpotXs[i] + g_MapViewX, g_MapViewY + 0x26), a sixteenth of the way
   each. After the fade (layer_col_set / layer_col_fade_start with -100 on all
   three channels) it runs 16 frames: each sets the D_800E4768 request
   (address 0x801CBB00 + frame * 0x600, size 0x600, flag 1), waits a frame
   (MenuFrameStep) and advances every member; on frame 8, when
   g_MapPartyKind == 0, each actor is re-initialised (MapRecordSpriteDataSet,
   MapAnimationStart). The members are then snapped onto their slots and sent
   MapAnimationPoseSet(actor, 2, 5), and the page g_MenuPage is opened
   (MenuPoseWordsSet, MenuPageDisp) with the cursor state cleared. */
/* func_8006A328 */
void MenuOpen(void)
{
    s16 c[3];
    s32 pos[12];
    s32 vel[12];
    s32 i;
    s32 j;
    MenuScreenBits *ui;

    SoundPlay(5, 1);
    sprite_run_off(0x87, 0xF);
    MenuRecordsInit();
    g_MessageFaceSecond = 0;
    g_MessageFaceFirst = 0;
    g_MessageFacePlaces = 0;
    g_MoveChrViewFollow = 0;
    CellAllClear(0x30);
    CellAllUpdate(0x20);
    MenuTextStringsPut();
    for (i = 0; i < 6; i++) {
        if (*(u16 *)(i * 2 + g_PartyIds) != 0) {
            if (g_MapPartyKind == 2) {
                pos[i * 2] = ((MapRecord *)g_MapPartyRecords)->sprite.x << 16;
                pos[i * 2 + 1] = ((MapRecord *)g_MapPartyRecords)->sprite.y << 16;
                ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.x = ((MapRecord *)g_MapPartyRecords)->sprite.x;
                ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.y = ((MapRecord *)g_MapPartyRecords)->sprite.y;
                ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.depth = ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.y + 0xE0;
                ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->on_kind.on = 1;
            } else {
                pos[i * 2] = ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.x << 16;
                pos[i * 2 + 1] = ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.y << 16;
            }
            vel[i * 2] = (((g_MenuLineUpSpotXs[i] + (s16)g_MapViewX) << 16) - pos[i * 2]) >> 4;
            vel[i * 2 + 1] = ((((s16)g_MapViewY + 0x26) << 16) - pos[i * 2 + 1]) >> 4;
            ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.flags |= 0x6000;
            *(u8 *)(i * 0x3C + g_MapPartyTwins) = 0;
        }
    }
    c[0] = c[1] = c[2] = -0x64;
    layer_col_set(-1, 0x100, 0);
    layer_col_fade_start(0, g_MapLayersMask | 0x80000, 1, 0x10, (s32)c);
    g_MapObjectsSpriteBits = 0x8000;
    MapObjectsAnimationStep();
    for (j = 0; j < 0x10; j++) {
        D_800E4768[1] = 0x801CBB00 + j * 0x600;
        D_800E4768[3] = 0x600;
        D_800E4768[0] = 1;
        MenuFrameStep();
        for (i = 0; i < 6; i++) {
            if (*(u16 *)(i * 2 + g_PartyIds) != 0) {
                if (j == 8 && g_MapPartyKind == 0) {
                    MapRecordSpriteDataSet(g_MapPartyRecords + i * 0x3C, *(u16 *)(i * 2 + g_PartyIds));
                    MapAnimationStart(g_MapPartyRecords + i * 0x3C, 1);
                }
                pos[i * 2] += vel[i * 2];
                pos[i * 2 + 1] += vel[i * 2 + 1];
                ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.x = pos[i * 2] >> 16;
                ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.y = pos[i * 2 + 1] >> 16;
                ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.depth = ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.y + 0xE0;
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (*(u16 *)(i * 2 + g_PartyIds) != 0) {
            ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.x = g_MenuLineUpSpotXs[i] + g_MapViewX;
            ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.y = g_MapViewY + 0x26;
            ((MapRecord *)(i * 0x3C + g_MapPartyRecords))->sprite.depth = g_MapViewY + 0x106;
            MapAnimationPoseSet((void *)(i * 0x3C + g_MapPartyRecords), 2, 5);
        }
    }
    MenuPoseWordsSet(g_MenuPage);
    MenuPageDisp((u16)g_MenuPage);
    pad_record_init(g_MenuPadRecord, 0x14, 0xF0D0, 0);
    MenuMemberCursorAdjust(g_MenuMemberCursor_blk, 0x8000);
    layer_mix_switch_on(0x10);
    ui = (MenuScreenBits *)D_800E4E70;
    D_800B8EEC |= 1;
    ui->unk_0e = (ui->unk_0e & 0xFFCF) | 0x10;
    layer_mix_set(0x10, 0x10);
    g_MenuSoundRequest = 0;
    g_MenuRedrawRequest = g_MenuPage;
}

s32 layer_order_set(s32, s32);                              /* extern */
s32 font_string_fixed_put(s32);                               /* extern */
s32 font_target_clear(s32, s32);                              /* extern */
s32 font_target_set(s32);                                 /* extern */

/* func_8006A7B0 */
void MenuTextStringsPut(void) {
    s32 relative_string_id;

    layer_order_set(0x800, 5);
    layer_order_set(0x100, 4);
    font_target_clear(0x801CBB00, 0x80);
    font_target_set(0x801CBB00);
    relative_string_id = 0x16;
    do {
        font_string_fixed_put(relative_string_id + 0x600);
        relative_string_id += 1;
    } while (relative_string_id < 0x23);
}
/* far_syms: g_MoveChrViewFollow D_800B8EEC g_MapCol */
extern s16 g_MapCol[3];
void layer_mix_switch_off(s32);
void MapPartyRingEntryGet(u16, MapPartyRingEntry *);

/* Closes the field menu's party screen: the mirror of MenuOpen. The
   five g_MenuRecords records' e.h0 are cleared and the screen fades
   (layer_mix_set / layer_mix_switch_off, colours g_MapCol). Each present member
   (g_PartyIds[i] != 0) asks MapPartyRingEntryGet for its field position - member
   0's when g_MapPartyKind == 2 - takes a 16.16 start (pos[]) from where its
   actor stands and a per-frame step (vel[]) a sixteenth of the way there,
   and is sent MapAnimationPoseSet(actor, 1, spot.b5). Then 16 frames:
   each sets the D_800E4768 request (address 0x801CBB00 + frame * 0x600,
   size 0x600, flag 1), waits a frame (MenuFrameStep) and advances every
   member; on frame 8 g_MapObjectsSpriteBits is cleared, MapObjectsAnimationStep runs and, when
   g_MapPartyKind == 0, each actor is re-initialised (MapRecordSpriteDataSet with its id
   + 0xF, MapAnimationStart). Finally the members are snapped onto their spots
   (x, y - b4, the unshifted y in +8) and take the spot's h6 bits into fE;
   with g_MapPartyKind == 2 every member but 0 is hidden instead, otherwise a
   non-zero g_MapPartyKind also wakes the member's g_MapPartyTwins twin. The spot of
   a hidden member is the one the previous call left in `spot` - the
   original's own reuse. Ends by redrawing page g_MenuPage and setting
   g_MoveChrViewFollow. */
/* func_8006A81C */
void MenuClose(void)
{
    MapPartyRingEntry spot;
    s32 pos[12];
    s32 vel[12];
    s32 i;
    s32 j;
    MenuScreenBits *ui;

    sprite_run_off(0x87, 0xF);
    for (i = 4; i >= 0; i--) {
        AS(s16, ((MapRecord *)g_MenuRecords)[i].sprite.on) = 0;
    }
    ui = (MenuScreenBits *)D_800E4E70;
    D_800B8EEC |= 1;
    ui->unk_0e &= 0xFFCF;
    layer_mix_set(0x10, 0);
    layer_mix_set(0x10, 0x20);
    layer_mix_switch_off(0x10);
    layer_order_set(0x800, 4);
    layer_order_set(0x100, 2);
    CellAllClear(0x10);
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            if (g_MapPartyKind == 2) {
                MapPartyRingEntryGet(0, &spot);
            } else {
                MapPartyRingEntryGet(i, &spot);
            }
            pos[i * 2] = ((MapRecord *)g_MapPartyRecords)[i].sprite.x << 16;
            pos[i * 2 + 1] = ((MapRecord *)g_MapPartyRecords)[i].sprite.y << 16;
            vel[i * 2] = ((spot.x << 16) - pos[i * 2]) >> 4;
            vel[i * 2 + 1] = ((spot.y << 16) - pos[i * 2 + 1]) >> 4;
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[i], 1, spot.facing);
        }
    }
    MenuFrameStep();
    font_target_clear(0x801CBB00, 0x60);
    layer_col_fade_start(0, g_MapLayersMask | 0x80000, 1, 0x10, (s32)g_MapCol);
    for (j = 0; j < 0x10; j++) {
        D_800E4768[1] = 0x801CBB00 + j * 0x600;
        D_800E4768[3] = 0x600;
        D_800E4768[0] = 1;
        if (j == 8) {
            g_MapObjectsSpriteBits = 0;
            MapObjectsAnimationStep();
        }
        MenuFrameStep();
        for (i = 0; i < 6; i++) {
            if (((u16 *)g_PartyIds)[i] != 0) {
                if (j == 8 && g_MapPartyKind == 0) {
                    MapRecordSpriteDataSet((s32)&((MapRecord *)g_MapPartyRecords)[i], ((u16 *)g_PartyIds)[i] + 0xF);
                    MapAnimationStart((s32)&((MapRecord *)g_MapPartyRecords)[i], 1);
                }
                pos[i * 2] += vel[i * 2];
                pos[i * 2 + 1] += vel[i * 2 + 1];
                ((MapRecord *)g_MapPartyRecords)[i].sprite.x = pos[i * 2] >> 16;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.y = pos[i * 2 + 1] >> 16;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.depth = ((MapRecord *)g_MapPartyRecords)[i].sprite.y + 0xE0;
            }
        }
    }
    layer_col_set(0, 0x100, (s32)g_MapCol);
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            if (g_MapPartyKind == 2 && i != 0) {
                ((MapRecord *)g_MapPartyRecords)[i].on_kind.on = ((MapRecord *)g_MapPartyTwins)[i].on_kind.on = 0;
            } else {
                ((MapRecord *)g_MapPartyRecords)[i].on_kind.on = 1;
                MapPartyRingEntryGet(i, &spot);
                ((MapRecord *)g_MapPartyRecords)[i].sprite.x = spot.x;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.depth = spot.y;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.y = spot.y - spot.height;
                if (g_MapPartyKind != 0) {
                    ((MapRecord *)g_MapPartyTwins)[i].on_kind.on = 1;
                    MapAnimationPoseSet(&((MapRecord *)g_MapPartyTwins)[i], 1, spot.facing);
                }
            }
            ((MapRecord *)g_MapPartyRecords)[i].sprite.flags &= 0x9FFF;
            ((MapRecord *)g_MapPartyRecords)[i].sprite.flags |= spot.class;
        }
    }
    MenuPageDisp((u16)g_MenuPage);
    MenuFrameStep();
    g_MoveChrViewFollow = 1;
}

s32 sprite_list_draw();                                  /* extern */
s32 card_accept_poll();                                /* extern */
s32 palette_animation_step();                                  /* extern */
s32 tile_animation_step();                                  /* extern */
s32 MapFrameEndCopy();                                  /* extern */
s32 MapBgChangeStep();                                  /* extern */
s32 MapPartyAnimationStep();                                  /* extern */
s32 MapViewStep(s32, s32);                              /* extern */
void MenuRecordsAnimationStep();                                  /* extern */
extern s32 g_MenuCardAcceptResult;

/* func_8006ACB0 */
void MenuFrameStep(void) {
    MapPartyAnimationStep();
    MapViewStep(0, 1);
    palette_animation_step();
    tile_animation_step();
    MenuRecordsAnimationStep();
    sprite_list_draw();
    if ((g_MenuPage == 0x12) || (g_MenuPage == 0x14) || (g_MenuPage == 0x11)) {
        g_MenuCardAcceptResult = card_accept_poll();
    }
    MapFrameEndCopy();
    MapBgChangeStep();
}
void MenuLineUpDisp(u16, s32, s32, s32);
s32 MenuMemberPickRun(s32);
s32 IconCellSet(s32, s32, s32, s32);
s32 MenuCursorMove(u16, u8 *, s32);
void MenuIconBytesDisp(u16, s32, s32, s32);
void MenuCommandsDisp(s32);
void MenuCommandsSet(s32, u16);
extern u16 g_SpriteData0[];
void MapAnimationSet(MapRecord *, u16);

/* The sprite data head's five words are pointer accesses through their
   members' addresses (*(u16 *)&bank->texel_records), as in ShopObjectInit:
   as member loads the function grows by 0x10 bytes. */
/* func_8006AD38 */
void MenuRecordsInit(void) {
    s32 i;
    SpriteDataHead *bank;

    mem_clear((s32 *)g_MenuShapes, 0x78);
    mem_clear((s32 *)g_MenuRenderEntries, 0x168);
    mem_clear((s32 *)g_MenuRecords, 0x1A4);

    AS(s16, ((SpritePiece *)g_MenuShapes)->kind) = 0x4000;
    AS(s16, ((SpritePiece *)g_MenuShapes)->texel_col.col) = 0x761;
    ((SpritePiece *)g_MenuShapes)->x1 = 0x1F;
    ((SpritePiece *)g_MenuShapes)->x2 = 0x1F;
    ((SpritePiece *)g_MenuShapes)->x0 = 0;
    ((SpritePiece *)g_MenuShapes)->y0 = 0;
    ((SpritePiece *)g_MenuShapes)->y1 = 0;
    ((SpritePiece *)g_MenuShapes)->y2 = 0xF;
    ((SpritePiece *)g_MenuShapes)->x3 = 0;
    ((SpritePiece *)g_MenuShapes)->y3 = 0xF;
    for (i = 1; i < 5; i++) {
        ((SpritePiece *)g_MenuShapes)[i] = ((SpritePiece *)g_MenuShapes)[0];
    }

    AS(s16, ((SpritePiece *)g_MenuShapes)[3].kind) = 0x5000;
    AS(s16, ((SpritePiece *)g_MenuShapes)[3].texel_col.col) = 0x7E5;
    ((SpritePiece *)g_MenuShapes)[3].x0 = -3;
    ((SpritePiece *)g_MenuShapes)[3].y0 = -3;
    ((SpritePiece *)g_MenuShapes)[3].x1 = 0xE2;
    ((SpritePiece *)g_MenuShapes)[3].y1 = -3;
    ((SpritePiece *)g_MenuShapes)[3].x2 = 0xE2;
    ((SpritePiece *)g_MenuShapes)[3].y2 = 0x1A;
    ((SpritePiece *)g_MenuShapes)[3].x3 = -3;
    ((SpritePiece *)g_MenuShapes)[3].y3 = 0x1A;
    AS(s16, ((SpritePiece *)g_MenuShapes)[4].texel_col.col) = 0x76A;
    ((SpritePiece *)g_MenuShapes)[4].x1 = 0x3F;
    ((SpritePiece *)g_MenuShapes)[4].x2 = 0x3F;

    bank = (SpriteDataHead *)g_SpriteData0;
    AS(s16, ((MapRecord *)g_MenuRecords)->sprite.on) = 0;
    AS(s16, ((MapRecord *)g_MenuRecords)->sprite.attr) = 0;
    ((MapRecord *)g_MenuRecords)->sprite.flags = 0x6360;
    ((MapRecord *)g_MenuRecords)->sprite.texel_records = *(u16 *)&bank->texel_records * 4 + (s32)bank;
    ((MapRecord *)g_MenuRecords)->sprite.pose_table = *(u16 *)&bank->pose_table * 4 + (s32)bank;
    ((MapRecord *)g_MenuRecords)->sprite.piece_table = *(u16 *)&bank->piece_table * 4 + (s32)bank;
    ((MapRecord *)g_MenuRecords)->animation_table = *(u16 *)&bank->animation_table * 4 + (s32)bank;
    ((MapRecord *)g_MenuRecords)->step_list = *(u16 *)&bank->step_list * 4 + (s32)bank;
    ((MapRecord *)g_MenuRecords)->sprite.x = g_MapViewX + 0x20;
    ((MapRecord *)g_MenuRecords)->sprite.y = g_MapViewY + 0x10;
    ((MapRecord *)g_MenuRecords)->animation_before = 0xFFFF;
    for (i = 1; i < 7; i++) {
        ((MapRecord *)g_MenuRecords)[i] = ((MapRecord *)g_MenuRecords)[0];
    }

    ((MapRecord *)g_MenuRecords)[1].sprite.x = g_MapViewX + 0x68;
    ((MapRecord *)g_MenuRecords)[1].sprite.y = g_MapViewY + 0x13;
    ((MapRecord *)g_MenuRecords)[2].sprite.x = g_MapViewX + 0x60;
    ((MapRecord *)g_MenuRecords)[2].sprite.y = g_MapViewY + 0x98;
    ((MapRecord *)g_MenuRecords)[3].sprite.x = g_MapViewX + 0x68;
    ((MapRecord *)g_MenuRecords)[3].sprite.y = g_MapViewY + 0x13;
    ((MapRecord *)g_MenuRecords)[4].sprite.x = g_MapViewX + 0x60;
    ((MapRecord *)g_MenuRecords)[4].sprite.y = g_MapViewY + 0x10;
    *(SpriteEntry *)g_MenuRenderEntries = ((MapRecord *)g_MenuRecords)->sprite;
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[0], 0xA);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[1], 8);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[2], 0xA);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[3], 9);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[4], 0xA);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[5], 4);
    MapAnimationSet(&((MapRecord *)g_MenuRecords)[6], 5);

    AS(s16, ((SpriteEntry *)g_MenuRenderEntries)->on) = 1;
    ((SpriteEntry *)g_MenuRenderEntries)->flags = 0x6350;
    ((SpriteEntry *)g_MenuRenderEntries)->x = g_MapViewX + 0x3A;
    ((SpriteEntry *)g_MenuRenderEntries)->y = g_MapViewY + 0x38;
    ((SpriteEntry *)g_MenuRenderEntries)->depth = g_MapViewY + 0xE0;
    ((SpriteEntry *)g_MenuRenderEntries)->pose = *(u16 *)(*(u16 *)(((MapRecord *)g_MenuRecords)->animation_table + 0x3A) * 4 + ((MapRecord *)g_MenuRecords)->step_list);
    ((SpriteEntry *)g_MenuRenderEntries)[1] = ((SpriteEntry *)g_MenuRenderEntries)[0];

    ((SpriteEntry *)g_MenuRenderEntries)[1].x = g_MapViewX + 0xAA;
    AS(s16, ((SpriteEntry *)g_MenuRenderEntries)[2].on) = 1;
    ((SpriteEntry *)g_MenuRenderEntries)[2].flags = 0;
    ((SpriteEntry *)g_MenuRenderEntries)[2].piece_table = g_MenuShapes;
    ((SpriteEntry *)g_MenuRenderEntries)[2].pose_table = 0;
    ((SpriteEntry *)g_MenuRenderEntries)[2].x = g_MapViewX + 0x38;
    ((SpriteEntry *)g_MenuRenderEntries)[2].y = g_MapViewY + 0x40;
    for (i = 3; i < 7; i++) {
        ((SpriteEntry *)g_MenuRenderEntries)[i] = ((SpriteEntry *)g_MenuRenderEntries)[2];
        ((SpriteEntry *)g_MenuRenderEntries)[i].piece_table = (s32)&((SpritePiece *)g_MenuShapes)[i - 2];
    }

    ((SpriteEntry *)g_MenuRenderEntries)[3].x = g_MapViewX + 0x38;
    ((SpriteEntry *)g_MenuRenderEntries)[3].y = g_MapViewY + 0x40;
    ((SpriteEntry *)g_MenuRenderEntries)[4].x = g_MapViewX + 0x38;
    ((SpriteEntry *)g_MenuRenderEntries)[4].y = g_MapViewY + 0x40;
    ((SpriteEntry *)g_MenuRenderEntries)[5].x = g_MapViewX + 0x38;
    ((SpriteEntry *)g_MenuRenderEntries)[5].y = g_MapViewY + 0x40;
    ((SpriteEntry *)g_MenuRenderEntries)[6].x = g_MapViewX + 0x60;
    ((SpriteEntry *)g_MenuRenderEntries)[6].y = g_MapViewY + 0x10;
    for (i = 7; i < 9; i++) {
        ((SpriteEntry *)g_MenuRenderEntries)[i] = ((SpriteEntry *)g_MenuRenderEntries)[6];
    }
    ((SpriteEntry *)g_MenuRenderEntries)[9] = ((SpriteEntry *)g_MenuRenderEntries)[5];
    ((SpriteEntry *)g_MenuRenderEntries)[9].y = g_MapViewY + 0x60;
}

extern const u8 g_MenuMemberPlaces[];

/* func_8006B2B4 */
void MenuMemberCursorMove(u16 *cursor, s32 buttons)
{
  s32 flagA;
  s32 flagB;
  u16 val;
  flagA = buttons & 0x6008;
  flagB = buttons & 0x9004;
  while (1)
  {
    if (flagA)
    {
      if ((*cursor) < 5)
        val = (*cursor) + 1;
      else
        val = 0;
      *cursor = val;
    }
    else
      if (flagB)
    {
      if ((*cursor) == 0)
        val = 5;
      else
        val = (*cursor) - 1;
      *cursor = val;
    }
    else
    {
      break;
    }
    if ((*((u16 *) ((g_MenuMemberPlaces[*cursor] * 2) + g_PartyIds))) == 0)
    {
      continue;
    }
    break;
  }

}
/* MenuCursorMove -- the menus' CURSOR STEP in a row of six:
   MenuCursorMove(cell, cells, bits) answers the cell the cursor moves to.
   cell = row * 6 + column; only the column changes, and it wraps inside
   0..5.

     bits & 0x6000 (right / down)       step +1
     else bits & 0x9000 (left / up)     step -1
     else                               the cell is answered as it is

   Each pass steps the column once (5 -> 0 for +1, 0 -> 5 for -1).
   - cells == 0 (no list): the answer is that first step, untested.
   - the column is back where it started (a whole lap): the answer is the
     starting cell when cells[cell] is non-zero, and the ROW'S FIRST cell
     (column 0) when it is zero -- an empty row parks the cursor on its
     first cell.
   - otherwise the pass's cell is the answer when cells[row * 6 + column]
     is non-zero, else the next pass.
   So with a list the cursor skips empty cells. A leaf: no call, no store,
   no global; no RNG draw and no frame wait.

   Shape: the answer is a 16-bit value (`andi $v0, 0xFFFF` at all three
   returns) but every caller masks or stores it itself, i.e. the callers
   took the function as returning int -- so under this TU's one prototype
   the definition is s32 and each return carries the (u16); a u16 return
   type gives the same 84 instructions here and takes seven instructions
   out of the callers (TU .text -0x1C). The start column is `cursor % 6` written AGAIN in
   the loop's test: the target divides twice (once in the entry block,
   once in the loop's pre-header) -- kept in a local it is 29/84. The NULL
   test has a return of its own (as a break 64/84) while the lap arm leaves
   by the break (as a return 66/84): the target keeps three `jr $ra`. The
   two direction tests are two ifs, not an else-if (78/84), and each wrap is
   if / else, not step-then-test (78/84). col and row are u16 (ints: 66 /
   67 of 84). */
/* func_8006B33C */
s32 MenuCursorMove(u16 cursor, u8 *entries, s32 buttons)
{
    u16 col;
    u16 row;
    s32 dir;

    col = cursor % 6;
    row = cursor / 6;
    if (buttons & 0x6000) {
        dir = 1;
    } else if (buttons & 0x9000) {
        dir = -1;
    } else {
        return (u16)(col + row * 6);
    }
    do {
        if (dir == 1) {
            if (col == 5) {
                col = 0;
            } else {
                col++;
            }
        }
        if (dir == -1) {
            if (col == 0) {
                col = 5;
            } else {
                col--;
            }
        }
        if (entries == 0) {
            return (u16)(col + row * 6);
        }
        if (col == cursor % 6) {
            if (entries[cursor] == 0) {
                col = 0;
            }
            break;
        }
    } while (entries[row * 6 + col] == 0);
    return (u16)(col + row * 6);
}

s32 ChrItemRoomCheck();
s32 ChrItemUseGet();

/* MenuPoseWordsSet -- the field menu's LINE-UP POSES: fills the two per-row
   words MenuLineUpDisp hands to MapAnimationPoseSet(actor, state, facing)
   for the six party rows - state ((s32 *)g_MenuPoseWords)[row] and facing
   ((s32 *)g_MenuFacingWords)[row] - by the open page g_MenuPage. Facing 5 is
   also what the member pick MenuMemberPickRun accepts a row on. Callers:
   MenuOpen (the party screen's opening, with the page itself as the
   argument) and MenuLineUpDisp, which passes its fourth argument when that
   is >= 0 - on the Give page the item under the cursor.

     page 0xC (the Give page; `item` = the item being given): for each
       row i until the first EMPTY one (id = ((u16 *)g_PartyIds)[i] == 0
       ends the scan; the rows after it keep what they had):
       facing = 1 (cannot take it) when the member is id 8, 9 or 10, or
         ChrItemRoomCheck(id) == 0 (no empty cell), or the item is 0xA8;
         else 5 when ChrItemUseGet(0x10, item, 1) != 0 (the item's mask
         has no bit 15), or the member's id is outside 2..10 (ids 1 and
         0xB up); else 1. These are ChrItemGive's own rules for where
         an item may land.
       state = 2 when the item's kind (byte +4 of its 0x12-byte record in
         *g_ChrItemRows, index id - 1) is < 5 - a thing that is worn - and
         ChrItemUseGet(id, item, 1) != 0; else 1. The record is read for
         any `item`, 0 included (the 0x12 bytes before the table).
     pages 9, 0xE, 0xF, 0x10: every row facing 5, state 1.
     page 0xA:                every row facing 7, state 2.
     any other page (0xB and 0xD through the table, the rest through the
       range test):           every row facing 5, state 2.

   No frames, no RNG, no sound.

   Shape: the Give arm is two plain if / else-if
   chains with a literal store in EVERY arm - four `= 1` and two `= 5` in
   the first, two `= 1` and one `= 2` in the second. loop.c matches the
   equal constant loads and hoists 1 ($s3) and 5 ($s5) out of the loop -
   one `= 5` alone is not worth a register to it, and with the tests
   folded into one condition (one store per value) it hoists item * 18
   instead (73/116) - and cross-jumping then merges the equal stores into
   the last of each, which is why every surviving store re-reads its
   table. `who` is an int (a u16 local is re-masked: 77/116); both callees
   are called through unprototyped declarations (ChrItemRoomCheck's u16
   prototype would mask the argument). The tables are indexed as arrays
   throughout (the integer-base spelling i * 4 + g_MenuFacingWords scores the
   same in the Give arm); in the fill loop after the switch that is what
   lets loop.c hoist both table addresses. g_ChrItemRows is a far scalar
   (same-register lui / lw). */
/* func_8006B48C */
void MenuPoseWordsSet(s32 item)
{
    s32 i;
    s32 a;
    s32 b;
    s32 who;

    switch (g_MenuPage) {
    case 12:
        for (i = 0; i < 6; i++) {
            who = ((u16 *)g_PartyIds)[i];
            if (who == 0) {
                break;
            }
            if (who >= 8 && who <= 10) {
                ((s32 *)g_MenuFacingWords)[i] = 1;
            } else if (ChrItemRoomCheck(who) == 0) {
                ((s32 *)g_MenuFacingWords)[i] = 1;
            } else if (item == 0xA8) {
                ((s32 *)g_MenuFacingWords)[i] = 1;
            } else if (ChrItemUseGet(0x10, item & 0xFF, 1) != 0) {
                ((s32 *)g_MenuFacingWords)[i] = 5;
            } else if (who < 2 || who > 10) {
                ((s32 *)g_MenuFacingWords)[i] = 5;
            } else {
                ((s32 *)g_MenuFacingWords)[i] = 1;
            }
            if (*(u8 *)(item * 0x12 + g_ChrItemRows - 0xE) >= 5) {
                ((s32 *)g_MenuPoseWords)[i] = 1;
            } else if (ChrItemUseGet(who, item & 0xFF, 1) == 0) {
                ((s32 *)g_MenuPoseWords)[i] = 1;
            } else {
                ((s32 *)g_MenuPoseWords)[i] = 2;
            }
        }
        return;
    case 9:
    case 14:
    case 15:
    case 16:
        a = 5;
        b = 1;
        break;
    case 10:
        a = 7;
        b = 2;
        break;
    default:
        a = 5;
        b = 2;
        break;
    }
    for (i = 0; i < 6; i++) {
        ((s32 *)g_MenuFacingWords)[i] = a;
        ((s32 *)g_MenuPoseWords)[i] = b;
    }
}

extern s32 g_MenuCardResult;

/* func_8006B65C */
void MenuCardResultDisp(void)
{
  if (g_MenuCardResult > 0)
  {
    ankprint_rows_clear(0x14, 0x6, 0x20);
    g_MenuNoticeCol8 = 1;
  }

  switch (g_MenuCardResult)
  {
    case 1:
      MenuNoticeDisp(0x673, 0xC, 0x16, 0x2, 0);
      break;

    case 2:
      MenuNoticeDisp(0x66D, 0xC, 0x16, 0x2, 0);
      break;

    case 3:
      MenuNoticeDisp(0x671, 0xC, 0x16, 0x2, 0);
      break;

    case 4:
      MenuNoticeDisp(0x67E, 0xC, 0x16, 0x2, 0);
      break;

    default:
      break;
  }
}
s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
void MenuTextPairDisp(s32 column, s32 row);  /* TU defines this below */
void MenuPartyStatusDisp(void);  /* TU defines this below */
s32 MenuTallNumberSet(s32, s32, s32, s32);
s32 MenuTallGlyphSet(s32, s32, s32);
void MenuChrRecordTextDisp(void);  /* TU defines this below */

/* MenuPageDisp -- the field menu's PAGE BODY: draw the fixed furniture of
   page `page` (callers pass (u16)g_MenuPage, a constant page, or 0xFFFF =
   nothing). It is bracketed by CellAllClear(0x20) / CellAllUpdate(0x20)
   (the second is an empty function) and switches on page & 0xFFFF:

     1 2 3 4 7 0xB 0xC 0xD 0x16   the party page: MenuPartyStatusDisp() (the five
                                  members' boxes + the silver line) and
                                  MenuTextPairDisp(0xC, 0xE) (two label strips,
                                  0xEE00 and 0xEDD0).
     5                            one member's box: who = the member in the
                                  selected slot (g_PartyIds[g_MenuMemberPlaces[
                                  g_MenuMemberCursor]]); MenuTextPairDisp(0xC, 0x10);
                                  CellChrGlyphSet(who, 3, 0x10, 0xDEAD2001,
                                  0x3000, 1) (the name); glyphs 0xA at (7,
                                  0x10) and 0xB at (8, 0x10) (MenuTallGlyphSet);
                                  the record's byte +0x1C as a 2-digit number
                                  at (9, 0x10) (MenuTallNumberSet); strips 0xEEE0
                                  (3 cells at col 2 row 8) and 0xEEEC (2 cells
                                  at col 2 row 0xC).
     6                            MenuChrRecordTextDisp().
     9 0xE 0xF 0x10               the command set-up page: one 8-cell strip
                                  per PRESENT member other than 0xC, in slot
                                  order 2, 3, 4, 5, 0, 1 (g_MenuMemberPlaces[(i + 2)
                                  % 6]) -- the n-th strip found is 0xEE20 +
                                  n * 0x20 at col 0x10, row 3 + n * 3 -- then
                                  MenuTextPairDisp(0xC, 0x12).
     0x11 0x12 0x14               the save pages: MenuCardListDisp() (the save
                                  list) and MenuCardResultDisp() (the notice for
                                  g_MenuCardResult -- which runs frames through
                                  MenuNoticeDisp when g_MenuCardResult is 1..4).
     0x13 0x15                    MenuOptionTextDisp() (redraw the 20 sprite-table
                                  entries).
     0                            func_800572D8() (an empty function).
     8 0xA, >= 0x17               nothing.

   No frame wait and no RNG draw of its own; the only frames are
   MenuCardResultDisp's (pages 0x11 / 0x12 / 0x14 with a notice pending).

   Shape: `who` is an int (one pseudo for both uses -- $s1 in the loop too,
   although nothing there crosses a call -- and no re-mask after the lhu).
   The strip's row and code are DERIVED from the count n, not variables of
   their own: loop.c reduces `n * 3 + 3` and `(u16)(n * 0x20 + 0xEE20)` to
   two registers, initialises them AFTER the hoisted &g_MenuMemberPlaces (the
   target's order: i, the table, code, row) and drops n. The (u16) cast
   narrows the sum to HImode, which is why the code's start is `addiu
   -0x11E0` and it is masked at the call; declared `row` / `u16 code`
   variables come out before the hoist and start with `ori 0xEE20`
   (157-161/165). The cases are in the target's address order, case 0
   last (it falls into the tail). */
/* func_8006B710 */
void MenuPageDisp(s32 page)
{
    s32 i;
    s32 who;
    s32 n;

    CellAllClear(0x20);
    switch (page & 0xFFFF) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
    case 0xB:
    case 0xC:
    case 0xD:
    case 0x16:
        MenuPartyStatusDisp();
        MenuTextPairDisp(0xC, 0xE);
        break;
    case 5:
        who = *(u16 *)(g_MenuMemberPlaces[g_MenuMemberCursor] * 2 + g_PartyIds);
        MenuTextPairDisp(0xC, 0x10);
        CellChrGlyphSet(who, 3, 0x10, 0xDEAD2001, 0x3000, 1);
        MenuTallGlyphSet(0xA, 7, 0x10);
        MenuTallGlyphSet(0xB, 8, 0x10);
        MenuTallNumberSet(*(u8 *)((who << 6) + g_ChrRecords + 0x1C), 9, 0x10, 2);
        CellTextDisp(2, 8, 3, 1, 3, 0xDEAD2001, 0xEEE0);
        CellTextDisp(2, 0xC, 2, 1, 2, 0xDEAD2001, 0xEEEC);
        break;
    case 6:
        MenuChrRecordTextDisp();
        break;
    case 9:
    case 0xE:
    case 0xF:
    case 0x10:
        n = 0;
        for (i = 0; i < 6; i++) {
            who = *(u16 *)(g_MenuMemberPlaces[(i + 2) % 6] * 2 + g_PartyIds);
            if (who != 0 && who != 0xC) {
                CellTextDisp(0x10, n * 3 + 3, 8, 1, 8, 0xDEAD2001, (u16)(n * 0x20 + 0xEE20));
                n++;
            }
        }
        MenuTextPairDisp(0xC, 0x12);
        break;
    case 0x11:
    case 0x12:
    case 0x14:
        MenuCardListDisp();
        MenuCardResultDisp();
        break;
    case 0x13:
    case 0x15:
        MenuOptionTextDisp();
        break;
    case 0:
        func_800572D8();
        break;
    }
    CellAllUpdate(0x20);
}

s32 func_80016608();                                  /* extern */
s32 CellTextDisp(s32, s32, s32, s32, s32, s32, s32);     /* extern */

/* func_8006B9A4 */
void MenuTextPairDisp(s32 column, s32 row) {
    func_80016608();
    CellTextDisp(column, row, 8, 1, 8, 0xDEAD2001, 0xEE00);
    CellTextDisp(column, row + 2, 0xC, 1, 0xC, 0xDEAD2001, 0xEDD0);
}
void MenuMoneyDisp(s32 column, s32 row);  /* TU defines this below */

/* Four MenuGlyphSet calls ((0x5C, 0xA, 9), (0x5E, 0xB, 9), (0x5D, 0xA,
   0xB), (0x5E, 0xB, 0xB)); then for i 0..4 the u16 at g_PartyIds indexed
   by g_MenuMemberPlaces[i + 1] goes to MenuMemberStatusDisp(v, i * 5 + 0xC, 7) unless
   it is 0 or 0xC; then MenuMoneyDisp(3, 0xF). The column i * 5 + 0xC is
   loop.c's induction variable: its 0xC start lands after the hoisted
   invariants, where post-reload cse turns it into a copy of the hoisted
   0xC compare constant (the target's `addu $s1, $s2, $zero`). */
/* func_8006BA34 */
void MenuPartyStatusDisp(void)
{
  s32 i;
  u16 v;
  MenuGlyphSet(0x5C, 0xA, 0x9);
  MenuGlyphSet(0x5E, 0xB, 0x9);
  MenuGlyphSet(0x5D, 0xA, 0xB);
  MenuGlyphSet(0x5E, 0xB, 0xB);
  for (i = 0; i < 5; i++)
  {
    v = *((u16 *) ((g_MenuMemberPlaces[i + 1] * 2) + g_PartyIds));
    if ((v != 0) && (v != 0xC))
    {
      MenuMemberStatusDisp(v, i * 5 + 0xC, 7);
    }
  }
  MenuMoneyDisp(3, 0xF);
}

/* func_8006BB0C */
void MenuMemberStatusDisp(s32 chr_id, s32 column, s32 row)
{
  CellChrGlyphSet(chr_id, column, row, 0xDEAD2001, 0x3000, 0);
  MenuGlyphSet(0xA, column, row + 1);
  MenuGlyphSet(0xB, column + 1, row + 1);
  MenuNumberSet(*((u8 *) (((chr_id << 6) + g_ChrRecords) + 0x1C)), column + 2, row + 1, 2);
  MenuNumberSet(*((u16 *) (((chr_id << 6) + g_ChrRecords) + 0)), column + 1, row + 2, 3);
  MenuGlyphSet(0xE, column, row + 3);
  MenuNumberSet(*((u16 *) (((chr_id << 6) + g_ChrRecords) + 2)), column + 1, row + 3, 3);
  MenuNumberSet(*((u16 *) (((chr_id << 6) + g_ChrRecords) + 4)), column + 1, row + 4, 3);
  MenuGlyphSet(0xE, column, row + 5);
  MenuNumberSet(*((u16 *) (((chr_id << 6) + g_ChrRecords) + 6)), column + 1, row + 5, 3);
}

s32 CellGlyphSet(s32, s32, s32, s32, s32, s32);            /* extern */
s32 MenuTallNumberSet(s32, s32, s32, s32);                    /* extern */
extern s32 *g_GameMoney;

/* func_8006BC4C */
void MenuMoneyDisp(s32 column, s32 row) {
    MenuTallNumberSet(*g_GameMoney, column, row, 7);
    CellGlyphSet(0xF, column + 7, row, 0xDEAD2001, 0x2000, 1);
}

s32 MenuTallGlyphSet(s32, s32, s32);                           /* extern */

/* func_8006BCB8 */
void MenuChrRecordTextDisp(void) {
    MenuTallGlyphSet(0xA, 0xC, 0xE);
    MenuTallGlyphSet(0xB, 0xD, 0xE);
    CellTextDisp(6, 0x12, 3, 1, 3, 0xDEAD2001, 0xEDE0);
    CellTextDisp(6, 0x16, 5, 1, 5, 0xDEAD2001, 0xEDEC);
    CellTextDisp(6, 0x18, 2, 1, 2, 0xDEAD2001, 0xED00);
}
s32 CellNumberSet(u32, s32, s32, s32, s32, s32, s32, s32); /* extern */

/* func_8006BD74 */
void MenuNumberCompareDisp(s32 column, s32 row, u32 first_number, u32 second_number) {
    s32 col;

    MenuTallNumberSet(first_number, column, row, 3);
    if (first_number == second_number) {
        col = 0x3000;
    } else {
        col = (first_number < second_number) << 0xD;
    }
    CellNumberSet(second_number, column + 5, row, 3, 0xDEAD2001, col, 1, 0);
}
void OutlineMapStep(void *records, s32 record_number, s32 mode);

/* The field menu's seven anim actors (the 0x3C-byte g_MenuRecords records
   MenuRecordsInit builds: cursors, markers): the per-frame animation tick
   and draw, MenuFrameStep's fifth step. It is MapPartyAnimationStep's and
   MoveChrAction's counterpart for this table, with no "live" or "held"
   test - all seven records are ticked and copied every frame, and
   sprite_list_draw skips a slot whose first halfword (e.h0) is 0.

   Sprite slots 0x8F..0x95 are freed first (sprite_run_off(0x8F, 7)). Then
   for each record i:
   - the frame timer h34 advances; at the frame's duration h36 the frame
     index h38 steps. Past the animation's end h3A it either holds the
     last frame (bit 15 of the animation entry, the u16 at w28 + h30 * 4:
     frame--, timer 0, the frame's duration) or restarts the animation
     (MapAnimationStart(p, 1)); otherwise timer 0 and the next frame's
     duration (the u16 at w2C + frame * 4 + 2);
   - the record's 0x24-byte sprite image e goes to sprite slot 0x95 - i of
     g_SpriteList with the current frame's tile (the u16 at w2C + frame * 4)
     in its h18;
   - OutlineMapStep(g_MenuRecords, i, 0): the per-slot animated marker of
     outline.c, drawn at the record's e.h2 / e.h6 when i < 5 and its e.h0 is
     nonzero.
   No frame wait and no RNG draw.

   Shape: one counter, the slot spelled 0x95 - i both times (a second,
   decrementing counter is strength-reduced to a 0x24 step and loses the
   target's two per-pass multiplies of the slot by 0x24), and the row
   pointer taken as i * 0x3C + base, the spelling MenuOpen uses for
   g_MapPartyRecords (as &base[i] the first add comes out base + offset). */
/* func_8006BE10 */
void MenuRecordsAnimationStep(void) {
    s32 i;
    MapRecord *p;

    sprite_run_off(0x8F, 7);
    for (i = 0; i < 7; i++) {
        p = (MapRecord *)(i * 0x3C + g_MenuRecords);
        if (++p->timer >= p->step_length) {
            if (++p->step >= p->end) {
                if (*(u16 *)(p->animation * 4 + p->animation_table) & 0x8000) {
                    p->step--;
                    p->timer = 0;
                    p->step_length = *(u16 *)(p->step * 4 + p->step_list + 2);
                } else {
                    MapAnimationStart((s32)p, 1);
                }
            } else {
                p->timer = 0;
                p->step_length = *(u16 *)(p->step * 4 + p->step_list + 2);
            }
        }
        ((SpriteEntry *)g_SpriteList)[0x95 - i] = ((MapRecord *)g_MenuRecords)[i].sprite;
        ((SpriteEntry *)g_SpriteList)[0x95 - i].pose = *(u16 *)(((MapRecord *)g_MenuRecords)[i].step * 4 + ((MapRecord *)g_MenuRecords)[i].step_list);
        OutlineMapStep((MapRecord *)g_MenuRecords, i, 0);
    }
}

/* The field menu's LINE-UP REFRESH: poses the six party actors and puts
   the two member markers on their members. Every page's frame call ends
   with it: (g_MenuMemberCursor, 0 or 1, 0, 0) from the pages - MenuItemGiveStep
   (page 0xC, Give) passes the cell under its cursor as the fourth
   argument instead of 0 - (g_MenuPresetMemberCursor, 1, 0, 0) from the command set-up
   pages, (g_MenuMemberPickCursor, 1, 1, -1) from the member pick MenuMemberPickRun, and
   (g_MenuBtlPlacesState[1], 1, 0, 0) or (0, 0, 0, 0) from MenuBtlPlacesStep. 17 call
   sites: 13 in menu_page.c, one in menu_run.c, and one each in the menu
   pages of menu_card.c, menu_option.c and menu_list.c (menu_card.c and
   menu_option.c call it undeclared).

   item >= 0: MenuPoseWordsSet(item) first - the per-row pose words are
   refilled for the open page g_MenuPage (on the Give page item is the
   item being given; the other pages pass 0, and the member pick -1,
   which keeps the words as they are).

   Then, for each of the six rows i that holds a member
   (((u16 *)g_PartyIds)[i] != 0; an empty row is skipped, not the end):
   - group = ((s32 *)g_MenuPoseWords)[i], facing = ((s32 *)g_MenuFacingWords)[i] -
     both read BEFORE the actor is written;
   - the row's field actor g_MapPartyRecords[i] (0x3C records) is HELD: byte
     +1 = 1, so the party actors' tick MapPartyAnimationStep leaves its
     animation timer alone;
   - cursor_pose != 0 and i is the row of the page's own cursor
     (g_MenuMemberPlaces[g_MenuMemberCursor]): group = 1;
   - i is the row of the page's own cursor (whatever cursor_pose is): actor 3
     of g_MenuRecords is put on the member - e.h2 = actor x (+6) - 8,
     e.h6 = actor y (+0xA) - 0x15;
   - i is the row of slot member_cursor (g_MenuMemberPlaces[member_cursor]): group = 1 unless the
     page is 0xC (the Give page), and when pick_marker != 0 the actor's hold is
     released (+1 = 0, it alone animates) and actor 1 of g_MenuRecords (the
     pick's marker) is put on it the same way (x - 8, y - 0x15);
   - MapAnimationPoseSet(actor, (s16)group, (u16)facing) sets the
     animation.
   The two markers are only MOVED here; switching them on and off
   (e.h0) is the callers'. g_MenuMemberPlaces[] (slot -> row; its six bytes are
   1, 0, 2, 3, 4, 5) is indexed with member_cursor and with g_MenuMemberCursor unchecked.

   No frame wait, no RNG, no sound.

   Shape: member_cursor is the TU's u16 parameter, masked where it is used; the two
   cursor tests are two plain ifs in a row - jump threading sends a failed
   `cursor_pose != 0 && i == row` straight past the second, equal test, which is
   why the target re-reads g_MenuMemberCursor only on the path that falls into it.
   The actor is g_MapPartyRecords indexed as an array at every use, so loop.c
   reduces i * 0x3C to a register of its own and initialises it AFTER the
   two addresses it hoists: &g_MenuMemberPlaces ($s2), &g_MenuMemberPlaces[member_cursor] ($s3), then
   `addu $s1, $s0, $zero` - $s1 being member_cursor's register, free once $s3 is
   formed. An offset variable in the source is set before the hoists and
   keeps member_cursor out of $s1 (99/110); a `tbl` pointer local is loaded before
   `i = 0` (108/110, and 103/110 with a `sel` local beside it); the integer
   base i * 0x3C + g_MapPartyRecords that other functions here use flips the call's
   `addu $a0, $a0, $s1` (109/110). The stores into g_MenuRecords are in-struct
   halfword stores, so neither g_MapPartyRecords nor g_MenuRecords is read again
   between the two of a pair; the byte store `+1 = 0` does force the second
   read of g_MapPartyRecords. MapAnimationPoseSet is (void *, s16 group, u16
   facing) in its own TU and (void *, s32, s32) here, hence the two casts at
   the call. */
/* func_8006BFC0 */
void MenuLineUpDisp(u16 member_cursor, s32 pick_marker, s32 cursor_pose, s32 item)
{
    s32 i;
    s32 group;
    s32 facing;

    if (item >= 0) {
        MenuPoseWordsSet(item);
    }
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            group = ((s32 *)g_MenuPoseWords)[i];
            facing = ((s32 *)g_MenuFacingWords)[i];
            ((MapRecord *)g_MapPartyRecords)[i].hold_definition.hold = 1;
            if (cursor_pose != 0 && i == g_MenuMemberPlaces[g_MenuMemberCursor]) {
                group = 1;
            }
            if (i == g_MenuMemberPlaces[g_MenuMemberCursor]) {
                ((MapRecord *)g_MenuRecords)[3].sprite.x = ((MapRecord *)g_MapPartyRecords)[i].sprite.x - 8;
                ((MapRecord *)g_MenuRecords)[3].sprite.y = ((MapRecord *)g_MapPartyRecords)[i].sprite.y - 0x15;
            }
            if (i == g_MenuMemberPlaces[member_cursor]) {
                if (g_MenuPage != 0xC) {
                    group = 1;
                }
                if (pick_marker != 0) {
                    ((MapRecord *)g_MapPartyRecords)[i].hold_definition.hold = 0;
                    ((MapRecord *)g_MenuRecords)[1].sprite.x = ((MapRecord *)g_MapPartyRecords)[i].sprite.x - 8;
                    ((MapRecord *)g_MenuRecords)[1].sprite.y = ((MapRecord *)g_MapPartyRecords)[i].sprite.y - 0x15;
                }
            }
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[i], (s16)group, (u16)facing);
        }
    }
}
/* la_absolute: MenuCommandsDisp */

/* MenuCommandsDisp -- the menu's COMMAND COLUMN draw: the four command
   icons down the left edge and the command cursor's height.
   MenuCommandsDisp(cursor); 16 call sites, in the field-menu pages of menu_page.c
   (nine) and menu_run.c (one) and the menu pages of menu_card.c (three),
   menu_list.c (two) and menu_option.c (one), every one right after
   MenuCommandsSet has written the page's command bytes. They pass 0 (the
   first command), the page's saved cursor (g_MenuCommandCursor, g_MenuItemCommandCursor,
   g_MenuPage7Cursor), or 4 -- three sites, each after MenuCommandsSet(0, 0), where
   all the bytes are 0 and the four cells are therefore cleared.

   For i = 0..3, top to bottom:
     IconCellSet(4, 2 + 3 * i, g_MenuCommands[i], attr) -- the icon whose number is
     command byte i, at cell column 4, rows 2 / 5 / 8 / 0xB of the 40 x 28
     grid. attr is 0 for the cursor's row (i == cursor) and 0x10000000 for
     the other three; IconCellSet only tests it for non-zero, which DIMS the
     icon. A command byte of 0 clears the cell whatever attr is. Only four
     of the six bytes MenuCommandsSet clears are ever drawn.
   Then CellAllUpdate(0x10) (an empty function in this build), and last
   actor 0 of g_MenuRecords (the command cursor; 0x3C records) gets
   e.h6 (+0xA, its y) = g_MapViewY + 0x10 + cursor * 0x18: 0x18 pixels are the
   three 8-pixel cells between icons and 0x10 is cell row 2. arg0 is not
   range-checked: with 4 nothing is lit and the cursor's y is one row
   below the fourth icon. The actor's on / off word e.h0 is the callers'.

   Reads g_MenuCommands[0..3], g_MapViewY, g_MenuRecords; writes that one
   halfword (and, through IconCellSet, the icon cells and the icon cache).
   No return value, no RNG draw, no frame wait: IconCellSet on a cache miss
   uploads the icon and waits in DrawSync(0) for the GPU, not for a frame.

   Shape, each measured against the matching text:
   - the "not this row" flag is a LOCAL, shifted at the call (34/41 with
     `(cursor != i) << 28` written in the argument, and the same 34 for the
     `? 0x10000000 : 0`, `* 0x10000000` and `(u8)` spellings). fold()
     rewrites a binary operator whose first operand is a bare comparison
     into a conditional expression (fold-const.c, the `'<'` arm before
     the switch), so every one of those reaches expand as
     `test ? 0x10000000 : 0` and comes out as negu / and with the constant
     hoisted into a fifth saved register; with the comparison's value in
     a variable the shift stays the target's sltu / sll 28. s32, u32 and
     u8 locals give the same bytes.
   - `cursor != i`, not `i != cursor` (40/41: the xor's operands swap).
   - the y sum as `cursor * 0x18 + (g_MapViewY + 0x10)` (37/41 without the
     parentheses, 36/41 with the product last).
   - &g_MenuCommands hoisted out of the loop is the assembler's same-register
     lui / addiu, la_absolute (39/41 without, under the TU's sized
     extern). The row 2 + 3 * i is written as that expression; loop.c
     makes the second counter. No device, no pad, no redundant cast. */
/* func_8006C178 */
void MenuCommandsDisp(s32 cursor)
{
    s32 i;
    s32 dim;

    for (i = 0; i < 4; i++) {
        dim = cursor != i;
        IconCellSet(4, i * 3 + 2, g_MenuCommands[i], dim << 28);
    }
    CellAllUpdate(0x10);
    ((MapRecord *)g_MenuRecords)->sprite.y = cursor * 0x18 + (g_MapViewY + 0x10);
}

/* la_absolute: MenuIconBytesDisp */

/* MenuIconBytesDisp -- the field menu's ITEM GRID draw: the page's icons,
   the cursor sprite and the member's face portrait. 24 call sites; every
   one passes a constant mode.

   member_cursor  party slot whose face is shown (0xFFFF = none)
   mode  mode: bit 0 -> grid at text row 0x13, face slot g_MessageFaceFirst, side 0;
         else bit 2 -> row 2, g_MessageFaceFirst, side 0, and with a slot given the
         flags gain g_MessageFacePlaces & 8; else -> row 0xB, face slot g_MessageFaceSecond,
         side 0x8000, flags gain g_MessageFacePlaces & 4 with a slot given
   row_count  rows of six cells to draw (1 or 2)
   cursor  the cursor's cell

   Icons: cell [i * 6 + j] of g_MenuIconBytes goes to IconCellSet(j * 4 + 0xC,
   row + i * 4, icon, 0) (IconCellSet(x, y, icon, attr) draws a 24x24 icon; 0
   clears). On page 5 (g_MenuPage) that cell is CLEARED instead and the
   icon is drawn on a wider grid, IconCellSet(j * 5 + 7, i * 4 + 8, icon, 0).
   Cursor: actor [2] of g_MenuRecords goes to x = (cursor % 6) * 0x20 +
   g_MapViewX + 0x60, y = (cursor / 6) * 0x20 + g_MapViewY + 0x98; on page 5
   x = (cursor % 6) * 0x28 + g_MapViewX + 0x38, y = (cursor / 6) * 0x20 +
   g_MapViewY + 0x40.
   Face: *slot = MessageFaceEntrySet(n, flags, *slot, side, 0) with n =
   g_MenuMemberPlaces[member_cursor] * 5 + 1 (0 for no member) -- a 1-based record number
   into the 5-byte rows of g_FaceListEntries; MessageFaceSet under it is the
   portrait loader (-> CellFacePlacesSet -> FaceSlotGet -> resource_load), so a face that
   has to come off the disc runs frames inside this call. No RNG draw and
   no frame wait of its own.

   Shape: the definition keeps the tree's prototype (u16, s32, s32, s32) --
   menu_card.c, menu_option.c and menu_places.c declare it so, and
   menu_option.c passes a variable as mode -- and narrows the
   mode into a u16 local on entry (`sh $a1, 0x20($sp)`); the tests and the
   `or` read the incoming register. A u16 mode or an old-style definition
   compile to the same bytes. All nine callee-saved registers go to the
   loops, so slot, flags, the face pointer and the side live on the stack
   in declaration order. `n` is a u16: its mask at the call folds away
   (the value is at most 0x4FC) and leaves the `move $a0, $v0`; an s32 n is
   computed straight into $a0 (174/177). The x / y bases are (s16)
   expressions so fold does not move the constant past the product (as in
   MenuAnswerRun). IconCellSet and MessageFaceEntrySet are called undeclared.
   &g_MessageFaceFirst / &g_MessageFaceSecond are the assembler's same-register lui/addiu
   (la_absolute). */
/* func_8006C21C */
void MenuIconBytesDisp(u16 member_cursor, s32 mode, s32 row_count, s32 cursor)
{
    u16 flags;
    u16 *face;
    u16 side;
    s32 row;
    s32 i;
    s32 j;
    u16 n;

    flags = mode;
    if (mode & 1) {
        face = &g_MessageFaceFirst;
        row = 0x13;
        side = 0;
    } else if (mode & 4) {
        face = &g_MessageFaceFirst;
        side = 0;
        if (member_cursor != 0xFFFF) {
            flags = mode | (g_MessageFacePlaces & 8);
        }
        row = 2;
    } else {
        face = &g_MessageFaceSecond;
        side = 0x8000;
        if (member_cursor != 0xFFFF) {
            flags = mode | (g_MessageFacePlaces & 4);
        }
        row = 0xB;
    }
    for (i = 0; i < row_count; i++) {
        for (j = 0; j < 6; j++) {
            if (g_MenuPage == 5) {
                IconCellSet(j * 4 + 0xC, row + i * 4, 0, 0);
                IconCellSet(j * 5 + 7, i * 4 + 8, g_MenuIconBytes[i * 6 + j], 0);
            } else {
                IconCellSet(j * 4 + 0xC, row + i * 4, g_MenuIconBytes[i * 6 + j], 0);
            }
        }
    }
    if (g_MenuPage != 5) {
        ((MapRecord *)g_MenuRecords)[2].sprite.x = (cursor % 6) * 0x20 + (s16)(g_MapViewX + 0x60);
        ((MapRecord *)g_MenuRecords)[2].sprite.y = (cursor / 6) * 0x20 + (s16)(g_MapViewY + 0x98);
    } else {
        ((MapRecord *)g_MenuRecords)[2].sprite.x = (cursor % 6) * 0x28 + (s16)(g_MapViewX + 0x38);
        ((MapRecord *)g_MenuRecords)[2].sprite.y = (cursor / 6) * 0x20 + (s16)(g_MapViewY + 0x40);
    }
    if (member_cursor != 0xFFFF) {
        n = g_MenuMemberPlaces[member_cursor] * 5 + 1;
    } else {
        n = 0;
    }
    *face = MessageFaceEntrySet(n, flags, *face, side, 0);
}

s32 CellAllUpdate(s32);

/* func_8006C4E0 */
void MenuIconsClear(s32 column, s32 row, s32 column_count, s32 row_count)
{
  s32 cell_row_offset;
  s32 cell_column;
  s32 j;
  s32 i;
  s32 row_offset;
  i = 0;
  if (row_count > 0)
  {
    row_offset = i;
    do
    {
      j = 0;
      if (column_count > 0)
      {
        cell_row_offset = row_offset;
        cell_column = column;
        do
        {
          IconCellSet(cell_column, row + cell_row_offset, 0, 0);
          j += 1;
          cell_column += 4;
        }
        while (j < column_count);
      }
      /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
         the block runs once, so behaviour is the same without the `do` / `while (0)`,
         but then 10 of this function's 49 instruction lines differ. */
      do
      {
        i += 1;
      }
      while (0);
      row_offset += 3;
    }
    while (i < row_count);
  }
  CellAllUpdate(0x10);
}
/* la_absolute: MenuCommandsSet */
/* load_hazard_nop: MenuCommandsSet */
extern u8 g_MenuCommands[8];
extern u8 g_MenuMemberCommands[];
extern u8 g_MenuCommands0C[];
extern u8 g_MenuItemCommands[];
extern u8 g_MenuPage7Commands[];

/* MenuCommandsSet -- the menu's COMMAND LIST for a page: the six command
   bytes g_MenuCommands are cleared (mem_clear, a zero fill) and the page's own
   are written. A command byte is an icon / string id: the label is string
   byte | 0x100 (names below are the string table's). Seventeen call sites,
   as (who, page); who matters on pages 1 and 2 only.

     page 0, 3, 8, 0xB, 0xC, 0xD   nothing more: all six stay 0
     page 1, 2      four commands (mem_move, a copy): who != 0xC a
                    member's g_MenuMemberCommands = EA EC FB FA (Magic, Items,
                    Equipment, Status); who == 0xC the party entry's
                    g_MenuCommands0C = FC EC E7 F1 (System, Items, Tactics,
                    Formation)
     page 4         three: g_MenuItemCommands = F7 F8 F9 (Use Item, Give Item,
                    Drop Item)
     page 5, 6      one: g_MenuMemberCommands[g_MenuCommandCursor], the member command under
                    the command cursor
     page 9, 0xE, 0xF, 0x10   one: g_MenuCommands0C[g_MenuCommandCursor], the party
                    command under the cursor
     page 0xA       one: 0xF1 (Formation)
     page 0x11..0x13   one: g_MenuPage7Commands[page - 0x11] = FE / FF / FD (Save,
                    Load, Setup)
     page 0x14      one: 0xFF (Load)
     page 0x15      one: 0xFD (Setup)
     page 7         three: g_MenuPage7Commands = FE FF FD (Save, Load, Setup)
     page 0x16      one: g_MenuItemCommands[2] = 0xF9 (Drop Item)
     page > 0x16    nothing more

   No frame wait and no random draw: its two callees are the byte fill and
   the byte copy.

   Shape notes. The cases stand in the target's ADDRESS order, which is
   not numeric: 0x15 before 0x14 (the other way round: 81/85), and 7 after
   them. `case 0: break;` is in the source -- without it the table starts
   at case 1 and the dispatch gains a subtract (71/85). The who test is
   `!= 0xC` first (the opposite spelling flips the branch: 80/85). page is
   a u16 PARAMETER kept raw in $s0 and masked at each use (the dispatch and
   the 0x11..0x13 index). The two mem_move pairs are written whole;
   cross-jumping merges each pair's jal. la_absolute gives g_MenuCommands's
   lui / addiu as an argument (61/85 without) while the store of byte 0
   stays gp-relative; load_hazard_nop gives the nop between page 0x16's
   lbu and the shared `sb` (79/85 without). No device, no pad, no
   redundant cast. */
/* func_8006C5A4 */
void MenuCommandsSet(s32 who, u16 page)
{
    mem_clear((s32 *)g_MenuCommands, 6);
    switch (page) {
    case 0:
        break;
    case 1:
    case 2:
        if (who != 0xC) {
            mem_move((s32 *)g_MenuCommands, (s32 *)g_MenuMemberCommands, 4);
        } else {
            mem_move((s32 *)g_MenuCommands, (s32 *)g_MenuCommands0C, 4);
        }
        break;
    case 4:
        mem_move((s32 *)g_MenuCommands, (s32 *)g_MenuItemCommands, 3);
        break;
    case 5:
    case 6:
        g_MenuCommands[0] = g_MenuMemberCommands[g_MenuCommandCursor];
        break;
    case 9:
    case 0xE:
    case 0xF:
    case 0x10:
        g_MenuCommands[0] = g_MenuCommands0C[g_MenuCommandCursor];
        break;
    case 0xA:
        g_MenuCommands[0] = 0xF1;
        break;
    case 0x11:
    case 0x12:
    case 0x13:
        g_MenuCommands[0] = g_MenuPage7Commands[page - 0x11];
        break;
    case 0x15:
        g_MenuCommands[0] = 0xFD;
        break;
    case 0x14:
        g_MenuCommands[0] = 0xFF;
        break;
    case 7:
        mem_move((s32 *)g_MenuCommands, (s32 *)g_MenuPage7Commands, 3);
        break;
    case 0x16:
        g_MenuCommands[0] = g_MenuItemCommands[2];
        break;
    }
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_80012DDC_pad);
s32 mem_clear(s32 *, s32);
void MenuIconBytesSpellsSet(s32 chr_id);  /* TU defines this below */
extern s32 g_ChrItemList0C;

/* func_8006C6F8 */
void MenuIconBytesSet(s32 chr_id, unsigned short page)
{
  u8 *list_row;
  s32 page_then_scroll;
  int top_scroll;
  mem_clear(g_MenuIconBytes, 0xC);
  page_then_scroll = page & 0xFFFF;
  switch (page_then_scroll & 0xFFFFu)
  {
    case 2:
      if (chr_id == 0xC)
    {
      page_then_scroll = (u16) g_MenuItemListScroll;
      list_row = g_ChrItemList0C + (page_then_scroll * 6);
      loop_4:
      if ((list_row[6] == 0) && (((u16) g_MenuItemListScroll) != (top_scroll = 0)))
      {
        g_MenuItemListScroll = ((u16) g_MenuItemListScroll) - 1;
        list_row -= 6;
        if (((u16) g_MenuItemListCursor) < 6U)
        {
          /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
             the block runs once, so behaviour is the same without the `do`
             / `while (0)`, but then 3 of this function's 85 instruction lines differ. */
          do
          {
            g_MenuItemListCursor = ((u16) g_MenuItemListCursor) + 6;
          }
          while (0);
        }
        goto loop_4;
      }

      mem_move(g_MenuIconBytes, list_row, 0xC);
      return;
    }
      if (g_MenuCommandCursor == 0)
    {
      case 3:
        MenuIconBytesSpellsSet(chr_id);

      return;
    }
      block_12:
    mem_move(g_MenuIconBytes, (g_ChrRecords + (chr_id << 6)) + 0x1E, 0xC);

      return;

    case 5:
      goto block_12;

    case 9:
      list_row -= 6;

    case 14:

    case 15:

    case 16:
      g_MenuIconBytes[6] = 0xE8;
      g_MenuIconBytes[7] = 0xF2;
      g_MenuIconBytes[8] = 0xF6;

    default:
      return;

  }

}

void MenuCursorTextDisp(s32 icon, char col_bits);  /* TU defines this below */

/* func_8006C84C */
void MenuIconBytesSpellsSet(s32 chr_id)
{
  s32 i;
  s32 j;
  s32 j_copy_then_k;
  u8 *top_icon;
  u8 *column_icons;
  u8 *icon;
  u8 top_spell;
  u8 bottom_spell;
  u8 spell;
  i = 0;
  do
  {
    top_icon = &g_MenuIconBytes[i];
    top_spell = *((s16 *) (((u8 *) (((chr_id << 6) + g_ChrRecords) + i)) + 0x2a));
    i += 1;
    *top_icon = top_spell;
  }
  while (i < 4);
  j = 0;
  do
  {
    column_icons = &g_MenuIconBytes[j];
    j_copy_then_k = j;
    bottom_spell = *((s16 *) (((u8 *) (j_copy_then_k + ((chr_id << 6) + g_ChrRecords))) + 0x2e));
    j += 1;
    column_icons[6] = bottom_spell;
  }
  while (j < 4);
  j_copy_then_k = 0;
  icon = g_MenuIconBytes;
  do
  {
    icon = &g_MenuIconBytes[j_copy_then_k];
    spell = *icon;
    if (spell != 0)
    {
      *icon = spell + 0xB6;
    }
    j_copy_then_k += 1;
  }
  while (j_copy_then_k < 0xC);
}

/* func_8006C8F0 */
void MenuCursorTextDisp(s32 icon, char col_bits)
{
  s32 old_col_bits;
  font_target_clear(0x801CD500, 0x14);
  old_col_bits = font_col_set(col_bits & 0xFF);
  font_string_proportional_put(icon + 0x200);
  font_target_set(0x801CDB00);
  font_string_proportional_put(icon + 0x100);
  font_col_set(old_col_bits & 0xFF);
  MenuTextPartsLoad(3);
}
s32 font_part_image_load(s32, s32, s32);

/* func_8006C964 */
void MenuTextPartsLoad(s32 part_bits)
{
  s32 previous_part;
  s32 byte_count;
  s32 part;
  s32 source;
  s32 part_bits_left;
  s32 first_part_bits;
  part_bits_left = part_bits;
  if (part_bits_left & 0xFFFF)
  {
    ;
    do
    {
      if ((part_bits_left & 3) != 0)
      {
        source = 0x801CD500;
        part = 0;
        byte_count = 0xA00;
        part_bits_left &= 0xFFFC;
        goto block_10;
      }
      if (part_bits_left & 0xC)
      {
        source = 0x801CDF00;
        part = 1;
        byte_count = 0x800;
        part_bits_left &= 0xFFF3;
        goto block_10;
      }
      if (part_bits_left & 0x30)
      {
        source = 0x801CE700;
        part = 2;
        byte_count = 0x800;
        part_bits_left &= 0xFFCF;
        goto block_10;
      }
      previous_part = part;
      if (part_bits_left & 0x1C0)
      {
        source = 0x801CEF00;
        part = 3;
        byte_count = 0xC00;
        part_bits_left &= 0xFE3F;
        block_10:
        ;

      }
      font_part_image_load(part, source, byte_count);
      ;
      first_part_bits = part_bits_left & 3;
    }
    while (part_bits_left & 0xFFFF);
  }
}
/* la_absolute: MenuMemberPickRun */
s32 pad_record_update(s32 *, s32);

/* The field menu's member pick: the cursor g_MenuMemberPickCursor is a party slot
   (0..5, in the line-up order g_MenuMemberPlaces[slot] -> row of g_PartyIds) and
   `excluded_id` is the one member id that cannot be picked - 0xC from the Magic
   page (MenuSpellStep) and the item Use page (MenuItemUseStep), the
   giver from the Give page (MenuItemGiveStep). MenuMemberOtherCheck(excluded_id, slot) is
   non-zero when the slot's member is not excluded_id; MenuMemberCursorMove steps a slot
   over the empty ones.

   Entry: the pending SE g_MenuSoundRequest is played and cleared, the cursor is
   stepped forward (0x2000) off excluded_id's slot, and actor 1 of g_MenuRecords (the
   pick's marker) is switched on (e.h0 = 1).

   Each pass: MenuLineUpDisp(slot, 1, 1, -1) poses the line-up and puts the
   marker on the slot's member, ONE menu frame (MenuFrameStep), then the
   pad (pad_record_update(g_MenuPadRecord, 0x14)); first match wins:
   - [2] & 0x8004: the slot steps back (0x8000) until it is off excluded_id's
     member; SE request 1.
   - [2] & 0x2008: the same forward (0x2000); SE request 1.
   - [1] & 0x130, or the latch g_MenuBackOutLatch & g_MenuBackOutPattern: cancel - the latch
     is set as on the pages (g_MenuBackOutLatch = 1 for bit 0x100, 2 for bit 0x20,
     g_MenuBackOutPattern = 0x30B30B30), SE request 3 is left PENDING (the caller's
     next page call plays it) and 0 is returned.
   - [1] & 0x40: the slot's row k = g_MenuMemberPlaces[slot] is accepted only when
     ((s32 *)g_MenuFacingWords)[k] == 5 - the per-row value MenuLineUpDisp hands
     to MapAnimationPoseSet as the member's facing, set by
     MenuPoseWordsSet - then SE request 2 is left pending and the
     member id ((u16 *)g_PartyIds)[k] is returned; otherwise SE request 4.
   A pending SE is played and cleared at the end of every pass that stays
   in the loop. On the way out the marker is switched off (e.h0 = 0).

   Frames and RNG: one MenuFrameStep per pass, the first before any input
   is read; no RNG draw of its own.

   Shape: the pad record is read as g_MenuPadRecord[n]
   at every use - loop.c hoists its address ($s1) and &g_MenuMemberPlaces ($s2);
   both step arms carry their own `g_MenuSoundRequest = 1` and the refused confirm
   its `= 4` - cross-jumping merges the three stores into the one before
   the play test, which is why the target re-reads g_MenuSoundRequest there;
   &g_MenuMemberPickCursor is the assembler's same-register lui/addiu under
   la_absolute; the row is a u8 local and both tables are indexed
   integer-base (k * 4 + g_MenuFacingWords, k * 2 + g_PartyIds). MenuMemberOtherCheck
   (defined below in the TU), MenuLineUpDisp are called undeclared. */
/* func_8006CA40 */
s32 MenuMemberPickRun(s32 excluded_id)
{
    s32 ret;
    u8 k;

    SoundPlay(g_MenuSoundRequest, 1);
    g_MenuSoundRequest = 0;
    while (MenuMemberOtherCheck(excluded_id, g_MenuMemberPickCursor) == 0) {
        MenuMemberCursorMove(&g_MenuMemberPickCursor, 0x2000);
    }
    AS(s16, ((MapRecord *)g_MenuRecords)[1].sprite.on) = 1;
    for (;;) {
        MenuLineUpDisp(g_MenuMemberPickCursor, 1, 1, -1);
        MenuFrameStep();
        pad_record_update((s32 *)g_MenuPadRecord, 0x14);
        if (g_MenuPadRecord[2] & 0x8004) {
            do {
                MenuMemberCursorMove(&g_MenuMemberPickCursor, 0x8000);
            } while (MenuMemberOtherCheck(excluded_id, g_MenuMemberPickCursor) == 0);
            g_MenuSoundRequest = 1;
        } else if (g_MenuPadRecord[2] & 0x2008) {
            do {
                MenuMemberCursorMove(&g_MenuMemberPickCursor, 0x2000);
            } while (MenuMemberOtherCheck(excluded_id, g_MenuMemberPickCursor) == 0);
            g_MenuSoundRequest = 1;
        } else {
            if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
                if (g_MenuPadRecord[1] & 0x100) {
                    g_MenuBackOutLatch = 1;
                } else if (g_MenuPadRecord[1] & 0x20) {
                    g_MenuBackOutLatch = 2;
                }
                g_MenuBackOutPattern = 0x30B30B30;
                g_MenuSoundRequest = 3;
                ret = 0;
                break;
            }
            if (g_MenuPadRecord[1] & 0x40) {
                k = g_MenuMemberPlaces[g_MenuMemberPickCursor];
                if (*(s32 *)(k * 4 + g_MenuFacingWords) == 5) {
                    g_MenuSoundRequest = 2;
                    ret = *(u16 *)(k * 2 + g_PartyIds);
                    break;
                }
                g_MenuSoundRequest = 4;
            }
        }
        if (g_MenuSoundRequest != 0) {
            SoundPlay(g_MenuSoundRequest, 1);
            g_MenuSoundRequest = 0;
        }
    }
    AS(s16, ((MapRecord *)g_MenuRecords)[1].sprite.on) = 0;
    return ret;
}

/* far_syms: g_MenuMemberPlaces (target reaches it %hi/%lo) */

/* func_8006CC50 */
u8 MenuMemberOtherCheck(s32 excluded_id, s32 cursor)
{
  return ((*(u16 *) ((g_MenuMemberPlaces[cursor & 0xFFFF] * 2) + g_PartyIds)) ^ excluded_id) != 0;
}
/* far_syms: D_800E4768 g_MenuPadRecord */
extern u16 g_MenuPageStage;
s32 CellRowsClear(s32, s32, s32);
s32 pad_record_update(s32 *, s32);
s32 card_change_check(void);
void MenuSaveOpen(void);

/* The menu's two-choice prompt. Opens a window at cell (column, row) with
   message string_id and, under it, message 0x637 (the two choices, side by
   side), then runs its own frame loop until the player answers. Returns
   the choice: 0 = left, 1 = right (also what cancelling returns), 2 = the
   page-0x11 case below. arg1 is not read (callers pass 0 or 0x801CC380);
   line_count is the message's line count (callers pass 1 or 2).

   Set-up: plays and clears the pending menu sound (g_MenuSoundRequest through
   SoundPlay), CellRowsClear(row, line_count * 2, 0x20), MenuMoneyDisp(3,
   0xF) when row < 0x12, CellAllUpdate(0x20), ONE frame (MenuFrameStep),
   font_target_clear(0x801CBB00, 0x22), then message string_id into the buffer
   0x801CBB00 and 0x637 into 0x801CC380 (font_target_set selects the buffer,
   font_string_proportional_put takes the string id), and two CellTextDisp strips - at
   (0xC, row) 14 wide and at (column, row + (line_count - 1) * 2) 6 wide. The
   D_800E4768 request (address, size 0x880, flag 1) is written by a
   two-pass loop whose passes overwrite each other, so the second pass's
   address 0x801CBB00 + 0x880 is what stays. The cursor, entry 3 of
   g_MenuRenderEntries, gets y = g_MapViewY + row * 8, two cells lower when
   line_count != 1.

   Loop, the selection starting on the RIGHT choice (sel = 1):
   - pad_record_update(g_MenuPadRecord, 0x14) reads the pad.
   - g_MenuPadRecord[2] & 0xA000 (left / right): sound 1, sel ^= 1.
   - else g_MenuPadRecord[1] & 0x130, or the latch g_MenuBackOutLatch & g_MenuBackOutPattern:
     cancel - g_MenuBackOutLatch = 1 for bit 0x100, 2 for bit 0x20, g_MenuBackOutPattern =
     0x30B30B30, sound 3, return 1. No frame passes.
   - else g_MenuPadRecord[1] & 0x40: confirm - sound 2 on the left choice, 3 on
     the right, return sel. No frame passes.
   - otherwise (and after a move) the cursor is placed: slice 1's hC and
     h10 = 0x19 and x = column * 8 + g_MapViewX - 1 on the left choice, 0x16
     and x = column * 8 + g_MapViewX + 0x20 on the right; entry 3 is copied
     to sprite slot 0x88 (g_SpriteList + 0x1320); the pending sound is played
     and cleared; ONE frame (MenuFrameStep).
   - then, by the open page g_MenuPage: on 0x11, card_change_check() != 0
     returns 2 when string_id is 0x670, and otherwise sets g_MenuPageStage = 0,
     g_MenuRedrawRequest = the page, calls MenuSaveOpen (which runs frames of its
     own through MenuCardRead) and returns 1; on 0x12 / 0x14,
     card_change_check() != 0 sets g_MenuPageStage = 0 and returns 1.

   Exit: sprite_run_off(0x87, 8) frees sprite slots 0x87-0x8E (the cursor's
   among them), font_target_clear(0x801CBB00, 0x22) again, and the D_800E4768
   request is rewritten the same way.

   Frames and RNG: one MenuFrameStep before the loop and one per pass that
   does not answer. Nothing here draws from the RNG directly; the only
   route to rng_draw is the frame itself (MenuFrameStep ->
   sprite_list_draw -> debug_sprite_page_show -> frame_end -> pad_frame_step ->
   pad_update, the vibration driver - frame-rng.md draw site 2).

   Codegen notes: sel is a u16 and the result a separate s32 (the target
   zero-extends s0 in place at `ret = sel` and returns it unmasked). The
   slice stores must be struct-typed, or the scheduler cannot lift the
   g_MapViewX load above them. Each arm carries the whole cursor
   statement, and the (s16) cast on the x term keeps `- 1` an addiu: left
   as u16 arithmetic it becomes `ori 0xFFFF; addu`, and a signed
   g_MapViewX lets fold reassociate the constant onto column * 8. */
/* func_8006CC84 */
s32 MenuAnswerRun(s32 string_id, s32 unused, s32 column, s32 row, s32 line_count)
{
    s32 i;
    u16 sel;
    s32 ret;

    SoundPlay(g_MenuSoundRequest, 1);
    g_MenuSoundRequest = 0;
    CellRowsClear(row, line_count * 2, 0x20);
    if (row < 0x12) {
        MenuMoneyDisp(3, 0xF);
    }
    CellAllUpdate(0x20);
    MenuFrameStep();
    font_target_clear(0x801CBB00, 0x22);
    font_target_set(0x801CBB00);
    font_string_proportional_put((u16)string_id);
    font_target_set(0x801CC380);
    font_string_proportional_put(0x637);
    CellTextDisp(0xC, row, 0xE, 1, 0x11, 0xDEAD2001, 0xED00);
    CellTextDisp(column, row + (line_count - 1) * 2, 6, 1, 0x11, 0xDEAD2001, 0xED44);
    for (i = 0; i < 2; i++) {
        D_800E4768[1] = 0x801CBB00 + i * 0x880;
        D_800E4768[3] = 0x880;
        D_800E4768[0] = 1;
    }
    if (line_count == 1) {
        ((SpriteEntry *)g_MenuRenderEntries)[3].y = g_MapViewY + row * 8;
    } else {
        ((SpriteEntry *)g_MenuRenderEntries)[3].y = g_MapViewY + (row + 2) * 8;
    }
    sel = 1;
    for (;;) {
        pad_record_update((s32 *)g_MenuPadRecord, 0x14);
        if (g_MenuPadRecord[2] & 0xA000) {
            g_MenuSoundRequest = 1;
            sel ^= 1;
        } else {
            if ((g_MenuPadRecord[1] & 0x130) || (g_MenuBackOutLatch & g_MenuBackOutPattern)) {
                if (g_MenuPadRecord[1] & 0x100) {
                    g_MenuBackOutLatch = 1;
                } else if (g_MenuPadRecord[1] & 0x20) {
                    g_MenuBackOutLatch = 2;
                }
                g_MenuBackOutPattern = 0x30B30B30;
                ret = 1;
                g_MenuSoundRequest = 3;
                break;
            }
            if (g_MenuPadRecord[1] & 0x40) {
                ret = sel;
                if (ret == 0) {
                    g_MenuSoundRequest = 2;
                } else {
                    g_MenuSoundRequest = 3;
                }
                break;
            }
        }
        if (sel == 0) {
            ((SpritePiece *)g_MenuShapes)[1].x1 = 0x19;
            ((SpritePiece *)g_MenuShapes)[1].x2 = 0x19;
            ((SpriteEntry *)g_MenuRenderEntries)[3].x = column * 8 + (s16)(g_MapViewX - 1);
        } else {
            ((SpritePiece *)g_MenuShapes)[1].x1 = 0x16;
            ((SpritePiece *)g_MenuShapes)[1].x2 = 0x16;
            ((SpriteEntry *)g_MenuRenderEntries)[3].x = column * 8 + (s16)(g_MapViewX + 0x20);
        }
        *(SpriteEntry *)(g_SpriteList + 0x1320) = ((SpriteEntry *)g_MenuRenderEntries)[3];
        if (g_MenuSoundRequest != 0) {
            SoundPlay(g_MenuSoundRequest, 1);
            g_MenuSoundRequest = 0;
        }
        MenuFrameStep();
        if (g_MenuPage == 0x11) {
            if ((u16)string_id == 0x670) {
                if (card_change_check() != 0) {
                    ret = 2;
                    break;
                }
            } else if (card_change_check() != 0) {
                g_MenuPageStage = 0;
                g_MenuRedrawRequest = g_MenuPage;
                MenuSaveOpen();
                ret = 1;
                break;
            }
        }
        if ((g_MenuPage == 0x12 || g_MenuPage == 0x14) && card_change_check() != 0) {
            g_MenuPageStage = 0;
            ret = 1;
            break;
        }
    }
    sprite_run_off(0x87, 8);
    font_target_clear(0x801CBB00, 0x22);
    for (i = 0; i < 2; i++) {
        D_800E4768[1] = 0x801CBB00 + i * 0x880;
        D_800E4768[3] = 0x880;
        D_800E4768[0] = 1;
    }
    return ret;
}
/* far_syms: g_MenuPadRecord */

/* The menu's notice: prints the `lines` strings msg, msg + 1, .. as a
   12-cell strip each, at text column 0xC, rows row, row + 2, ..
   (MenuNoticeDisp(0x675, 0xC, 0x16, 1, 0) is the save writer's first
   notice). With `wait` set it then holds until the pad's [1] word has
   0x40 or 0x10 and takes the notice down again; with msg == 0 it only
   takes it down.

   msg != 0: the pending menu sound is played and cleared (g_MenuSoundRequest
   through SoundPlay), CellRowsClear(row, lines * 2, 0x20) clears the
   rows, CellAllUpdate(0x20) (an empty function), and the lines' cells are
   cleared: the text buffer is 0x80-byte cells from 0x801CBB00, a line is
   12 of them, and the notice uses cells 0x50 on (font_target_clear(0x801CE300,
   lines * 0xC)). When g_MenuNoticeCol8 is set (MenuCardResultDisp, the save pages'
   pending notice) the glyph style is 8 for the print and put back after
   (font_col_set returns the old style), and the flag is cleared. Line i:
   the pen goes to cell 0x50 + i * 0xC (font_target_set), string msg + i is
   printed (font_string_proportional_put), and the strip is placed with
   CellTextDisp(0xC, row + i * 2, 0xC, 1, 0xC, 0xDEAD2001, 0xEE40 +
   i * 0x30) -- the code of cell 0x50 + i * 0xC.

   wait != 0 (and msg != 0): one frame (MenuFrameStep) per pass until
   pad_record_update(g_MenuPadRecord, 0x14) leaves 0x40 or 0x10 in g_MenuPadRecord[1];
   then sound 2.

   wait != 0, or msg == 0: font_target_clear(0x801CBB00, lines * 0x11) and
   one strip over the whole notice, CellTextDisp(col, row, 0xC, lines,
   0xC, 0xDEAD2001, code) -- with the code of cell 0x50 + i * 0xC for the
   print loop's FINAL i, i.e. the cells after the last line. `col` is read
   only here. ORIGINAL BEHAVIOUR: with msg == 0 the loop never ran and `i`
   is uninitialised -- the target computes the code from whatever the
   caller left in $s3.

   Nothing is returned (the tree declares it s32; no caller reads $v0).

   Shape: msg is a u16 PARAMETER -- the raw argument stays in $s4 and is
   masked at each of its four uses. The pen address must be written as
   cells, 0x801CBB00 + (i * 0xC + 0x50) * 0x80: a multiply on top keeps
   fold from associating the two constants, so loop.c reduces only
   i * 0x600 + 0x2800 ($s0) and the base is added in the loop, as in the
   target; `0x801CBB00 + (i * 0x600 + 0x2800)` folds to one giv from
   0x801CE300 (131/139). The strip code is (u16)(i * 0x30 + 0xEE40), cast
   on the sum (MenuPageDisp's lesson), written again after the loop. */
/* func_8006D06C */
s32 MenuNoticeDisp(u16 first_string_id, s32 column, s32 row, s32 line_count, s32 wait)
{
    s32 i;
    s32 old;

    if (first_string_id != 0) {
        SoundPlay(g_MenuSoundRequest, 1);
        g_MenuSoundRequest = 0;
        CellRowsClear(row, line_count * 2, 0x20);
        CellAllUpdate(0x20);
        font_target_clear(0x801CE300, (u16)(line_count * 0xC));
        if (g_MenuNoticeCol8 != 0) {
            old = font_col_set(8) & 0xFF;
        }
        for (i = 0; i < line_count; i++) {
            font_target_set(0x801CBB00 + (i * 0xC + 0x50) * 0x80);
            font_string_proportional_put(first_string_id + i);
            CellTextDisp(0xC, row + i * 2, 0xC, 1, 0xC, 0xDEAD2001, (u16)(i * 0x30 + 0xEE40));
        }
        if (g_MenuNoticeCol8 != 0) {
            font_col_set(old);
            g_MenuNoticeCol8 = 0;
        }
    }
    if (wait != 0) {
        if (first_string_id != 0) {
            for (;;) {
                pad_record_update((s32 *)g_MenuPadRecord, 0x14);
                if (g_MenuPadRecord[1] & 0x50) {
                    break;
                }
                MenuFrameStep();
            }
            g_MenuSoundRequest = 2;
            SoundPlay(g_MenuSoundRequest, 1);
            g_MenuSoundRequest = 0;
        }
    }
    if (wait != 0 || first_string_id == 0) {
        font_target_clear(0x801CBB00, (u16)(line_count * 0x11));
        CellTextDisp(column, row, 0xC, line_count, 0xC, 0xDEAD2001, (u16)(i * 0x30 + 0xEE40));
    }
}

/* func_8006D298 */
s32 MenuMemberCursorFind(s32 chr_id)
{
  s32 i;
  s32 var;
  var = PartyPlaceFind(chr_id & 0xFF);
  if (var < 0)
    return var;
  for (i = 0; i < 6; i++)
  {
    if (var == g_MenuMemberPlaces[i])
      return i;
  }

  return i;
}
/* The field menu's party actors slide to new positions: xs / ys are
   six x and six y values, one per party row. Callers: MenuPresetOpen and
   MenuBtlPlacesOpen (the formation editor's set-up). The page is first cleared
   and redrawn (MenuCommandsSet(0, 0), MenuCommandsDisp(4), MenuIconBytesSet(0, 0),
   MenuIconBytesDisp(0xFFFF, 1, 2, 0), MenuPageDisp(0xFFFF)) and the text
   buffer at 0x801CD500 cleared (font_target_clear(.., 0x4C)).

   For each present member (g_PartyIds[i] != 0) the start is the actor's
   own x / y as 16.16 (pos[]) and the step an EIGHTH of the way to
   (xs[i], ys[i]) (vel[]). Then 8 frames (MenuFrameStep each): an even
   frame j first files the D_800E4768 request (address 0x801CDB00 +
   j * 0x400, size 0x800, flag 1), frame 7 files (0x801CD500, 0x600, 1),
   the other odd frames file nothing; after the frame every present member
   advances one step (x, y, and +8 = y + 0xE0). Last, each present member
   is snapped onto its target and sent MapAnimationPoseSet(actor, -1,
   5). No RNG draw of its own; 8 frame waits.

   Shape: MenuOpen's (the party screen's opening) - flat s32 pos[12] /
   vel[12] indexed i * 2 and i * 2 + 1, and `f8 = y + 0xE0` re-reading the
   field just stored (writing `ys[i] + 0xE0` reloads ys[i]). */
/* func_8006D2F4 */
void MenuLineUpMove(s16 *xs, s16 *ys)
{
    s32 pos[12];
    s32 vel[12];
    s32 i;
    s32 j;

    MenuCommandsSet(0, 0);
    MenuCommandsDisp(4);
    MenuIconBytesSet(0, 0);
    MenuIconBytesDisp(0xFFFF, 1, 2, 0);
    MenuPageDisp(0xFFFF);
    font_target_clear(0x801CD500, 0x4C);
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            pos[i * 2] = ((MapRecord *)g_MapPartyRecords)[i].sprite.x << 16;
            pos[i * 2 + 1] = ((MapRecord *)g_MapPartyRecords)[i].sprite.y << 16;
            vel[i * 2] = ((xs[i] << 16) - pos[i * 2]) >> 3;
            vel[i * 2 + 1] = ((ys[i] << 16) - pos[i * 2 + 1]) >> 3;
        }
    }
    for (j = 0; j < 8; j++) {
        if (!(j & 1)) {
            D_800E4768[1] = 0x801CDB00 + (j << 10);
            D_800E4768[3] = 0x800;
            D_800E4768[0] = 1;
        } else if (j == 7) {
            D_800E4768[1] = 0x801CD500;
            D_800E4768[3] = 0x600;
            D_800E4768[0] = 1;
        }
        MenuFrameStep();
        for (i = 0; i < 6; i++) {
            if (((u16 *)g_PartyIds)[i] != 0) {
                pos[i * 2] += vel[i * 2];
                pos[i * 2 + 1] += vel[i * 2 + 1];
                ((MapRecord *)g_MapPartyRecords)[i].sprite.x = pos[i * 2] >> 16;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.y = pos[i * 2 + 1] >> 16;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.depth = ((MapRecord *)g_MapPartyRecords)[i].sprite.y + 0xE0;
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            ((MapRecord *)g_MapPartyRecords)[i].sprite.x = xs[i];
            ((MapRecord *)g_MapPartyRecords)[i].sprite.y = ys[i];
            ((MapRecord *)g_MapPartyRecords)[i].sprite.depth = ((MapRecord *)g_MapPartyRecords)[i].sprite.y + 0xE0;
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[i], -1, 5);
        }
    }
}
/* MenuLineUpMove's return trip: the field menu's party actors slide back
   to their line-up slots (g_MenuLineUpSpotXs[i] + g_MapViewX, g_MapViewY + 0x26)
   - the slots MenuOpen puts them on. Callers: MenuPresetClose and
   MenuBtlPlacesClose. The text buffer at 0x801CD500 is cleared first
   (font_target_clear(.., 0x1C)).

   For each present member (g_PartyIds[i] != 0) the start is the actor's
   own x / y as 16.16 (pos[]) and the step an EIGHTH of the way to its slot
   (vel[]). Then 8 frames (MenuFrameStep each): frame 2 first files the
   D_800E4768 request (address 0x801CDB00, word 2 = 0x40, size 0x800,
   flag 1), frame 4 files (0x801CD500, 0x34, 0x600, 1), the others file
   nothing; after the frame every present member advances one step (x, y,
   and +8 = y + 0xE0). Last, each present member is snapped onto its slot
   (+8 = g_MapViewY + 0x106) and sent MapAnimationPoseSet(actor, -1, 5).
   No RNG draw of its own; 8 frame waits.

   Shape: MenuLineUpMove's and MenuOpen's. g_MenuLineUpSpotXs must be declared
   `const` (it is at a .rodata address): its load then does not conflict
   with the store to pos[], so sched1 lifts it into the y load's delay
   slot. With the non-const declaration that one load stays below the
   store (159/166). MenuOpen is byte-identical under either. */
/* func_8006D5CC */
void MenuLineUpSpotsMove(void)
{
    s32 pos[12];
    s32 vel[12];
    s32 i;
    s32 j;

    font_target_clear(0x801CD500, 0x1C);
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            pos[i * 2] = ((MapRecord *)g_MapPartyRecords)[i].sprite.x << 16;
            pos[i * 2 + 1] = ((MapRecord *)g_MapPartyRecords)[i].sprite.y << 16;
            vel[i * 2] = (((g_MenuLineUpSpotXs[i] + (s16)g_MapViewX) << 16) - pos[i * 2]) >> 3;
            vel[i * 2 + 1] = ((((s16)g_MapViewY + 0x26) << 16) - pos[i * 2 + 1]) >> 3;
        }
    }
    for (j = 0; j < 8; j++) {
        if (j == 2) {
            D_800E4768[1] = 0x801CDB00;
            D_800E4768[2] = 0x40;
            D_800E4768[3] = 0x800;
            D_800E4768[0] = 1;
        } else if (j == 4) {
            D_800E4768[1] = 0x801CD500;
            D_800E4768[2] = 0x34;
            D_800E4768[3] = 0x600;
            D_800E4768[0] = 1;
        }
        MenuFrameStep();
        for (i = 0; i < 6; i++) {
            if (((u16 *)g_PartyIds)[i] != 0) {
                pos[i * 2] += vel[i * 2];
                pos[i * 2 + 1] += vel[i * 2 + 1];
                ((MapRecord *)g_MapPartyRecords)[i].sprite.x = pos[i * 2] >> 16;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.y = pos[i * 2 + 1] >> 16;
                ((MapRecord *)g_MapPartyRecords)[i].sprite.depth = ((MapRecord *)g_MapPartyRecords)[i].sprite.y + 0xE0;
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            ((MapRecord *)g_MapPartyRecords)[i].sprite.x = g_MenuLineUpSpotXs[i] + g_MapViewX;
            ((MapRecord *)g_MapPartyRecords)[i].sprite.y = g_MapViewY + 0x26;
            ((MapRecord *)g_MapPartyRecords)[i].sprite.depth = g_MapViewY + 0x106;
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[i], -1, 5);
        }
    }
}

/* func_8006D864 */
void MenuMemberCursorAdjust(u16 *cursor, u16 buttons)
{
    if ((*((u16 *) ((g_MenuMemberPlaces[*cursor] * 2) + g_PartyIds))) != 0)
        return;
    while (1)
    {
        MenuMemberCursorMove(cursor, buttons & 0xFFFF);
        if ((*((u16 *) ((g_MenuMemberPlaces[*cursor] * 2) + g_PartyIds))) == 0)
        {
            continue;
        }
        break;
    }
}

extern u8 battle_phase_pre_table[];
s32 pad_record_update(s32 *, s32);
s32 SoundPlay(u16, s32);

/* func_8006D900 */
void MenuPagesRun(void)
{
  g_MenuBackOutLatch = 0;
  if (g_MenuPage != 0)
  {
    do
    {
      pad_record_update((s32 *)g_MenuPadRecord, 0x14);
      ((s32 (*)()) (*(s32 *)((g_MenuPage * 4) + battle_phase_pre_table)))();
      MenuFrameStep();
      if (((u16) g_MenuSoundRequest) != 0)
      {
        SoundPlay((u16) g_MenuSoundRequest, 1);
        g_MenuSoundRequest = 0;
      }
      g_MenuBackOutPattern = ((u32) g_MenuBackOutPattern) >> 2;
    }
    while (g_MenuPage != 0);
  }
  g_MenuBackOutLatch = 0;
}
extern const u16 g_MenuPageNumberMessage[6];
void *memcpy(void *, const void *, unsigned int);
s32 MessageDisp(s32 message, s32 open_allowed);

/* Menu pages 0, 1 and 8 (battle_phase_pre_table[0], [1] and [8] all hold
   this address; splat's name): the page table's "no such page" slot. It
   reports the page number in the message window and hangs. The field
   menu's twin of MenuUseNumberHang (menu_use.c), which reports a bad battle
   state the same way; a development-time assert, not game flow.

   In order: MenuPageDisp(0) (page body 0 - the 0x20 bracket around an
   empty function), ONE menu frame (MenuFrameStep), then the 12-byte code
   string at g_MenuPageNumberMessage is copied to the stack and its two digit words
   filled in from the open page g_MenuPage:
     { 0xFF01, 0xF612, page / 10, page % 10, 0xFF00, 0xFFFF }
   = new page, string 0x612 ("MenuError"), the page number's two decimal
   digits as codes 0..9, the button prompt, end (text.md's control
   table). MessageDisp(msg, 0) (MessageCodesConv, then the field message
   MessageStringDisp) shows it, and the function ends in `for (;;)`: no
   epilogue, it never returns.

   Reached only through the table. MenuPagesRun leaves its loop on page 0
   without dispatching it, and MenuCommandStep steps round page 8
   (member 0xC on command 1 goes to cursor + 3 = 4, not cursor + 7).
   Whether anything can store a 1 was not checked.

   Frames and RNG: the one MenuFrameStep, and the frames MessageDisp
   runs for the prompt. No RNG draw of its own; the only route to
   rng_draw is the frame itself (the vibration driver, as in
   MenuAnswerRun's comment above).

   Shape: nothing needed shaping. One finding: g_MenuPageNumberMessage is NOT this
   function's own initialiser. A local `u16 msg[6] = { ... }` compiles to
   the same 39 instructions, but GCC emits such a constant just ahead of
   its function, i.e. after the jump tables of every earlier function
   (measured on a two-function file: the table's `.align 3` block, then
   `$LC0`). The template sits BEFORE MenuPoseWordsSet's table in .rodata
   (0x80012D4C; four pad bytes; the table at 0x80012D5C), so it was a
   named const object defined ahead of that function, and the copy from
   it is a memcpy as written here. The 12 bytes move as three lwl / lwr
   words whether the two sides are declared u16, s16 or u8, and with or
   without `const`. */
/* func_8006D9A8 */
void MenuPageNumberHang(void)
{
    u16 msg[6];

    memcpy(msg, g_MenuPageNumberMessage, sizeof(msg));
    MenuPageDisp(0);
    MenuFrameStep();
    msg[2] = g_MenuPage / 10;
    msg[3] = g_MenuPage % 10;
    MessageDisp((s32)msg, 0);
    for (;;) {
    }
}
