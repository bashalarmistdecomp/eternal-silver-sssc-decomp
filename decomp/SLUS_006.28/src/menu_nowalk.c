/* Lunar SSSC — menu_nowalk: the menu run from a page with no walk of the
   party's records, its open and close. The first split had it in battle.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "game.h"
#include "chr.h"
#include "menu.h"
#include "map.h"

/* --- Battle segment externs (accumulate here) ---
   NOTE: these gp-relative (small, <=8 byte) declarations reflect the FIRST
   battle translation unit (funcs up to ~0x75E34).  A later TU (MenuUsePlaceNameRun
   and on, 0x76968+) accesses several of the same globals via %hi/%lo; those
   functions cannot match against these declarations and live in
   menu_list.c, map_look.c and map_btl.c.
   See docs/spec/code/subsystems/battle.md. */
extern s32 g_ProfileShown;
extern u32 g_MenuBackOutLatch;
extern u32 g_MenuRequest;
extern u16 g_MenuPageStage;
extern u16 g_MapIdleCount;
/* 4-byte word: read as `lw` for value tests and as `lhu` (low half) where
   it feeds MenuPageDisp — one s32 declaration covers both via a cast. */
extern s32 g_MenuPage;
extern u8 g_MoveChrViewFollow;       
extern void FaceListEntriesSet(void);
extern void sprite_reset(s32 y);
extern void MenuPagesRun(void);
extern void MenuNoWalkOpen(void);
extern void MenuNoWalkClose(void);
extern void CellAllClear(s32 kind_mask);
extern void CellAllUpdate(s32 kind_mask);
extern void sprite_run_off(s32 first, s32 count);
extern void MenuFrameStep(void);
extern void font_target_clear(u8 *target, s32 block_count);
extern void MenuPageDisp(s32 page);
extern void font_target_set(u8 *target);
extern void font_string_proportional_put(s32 string_id);

extern s32 g_PartyIds;     extern s32 (*g_MapObjectPartyLookFunctions[])(MapObject *row, s32 row_number);  /* overworld handler table */
extern s32 (*g_MapObjectPartySeenFunctions[])(MapObject *row, s32 row_number);  extern s32 g_MenuOnePageRunning;
extern u16 g_MenuMemberCursor;      /* gp side of the TU wall: 7 gp users vs 1 (MenuUseDrawAreaOpen) %hi/%lo */
extern u16 g_MenuRedrawRequest;
extern s16 g_MapViewX;
extern s16 g_MapViewY;
extern s16 g_MapSecondViewX;
extern s16 g_MapSecondViewY;
extern s16 g_MapViewKeptX;
extern s16 g_MapViewKeptY;
extern u16 g_MapEffect;
extern void func_80056AE4(s32 arg0, s32 arg1);
extern void layer_order_set(s32 layer_mask, s32 order);
extern void *g_MenuRecords;    /* Script-side movement projection: steps (*px, *py) `dist` tiles along `dir`
   using the actor's probe rect at `probe`, returns the facing actually used. */
extern s16 g_MenuSoundRequest;
extern u16 g_MenuPadRecord[];
extern s16 g_MenuSoundRequest;
extern u16 g_MenuPadRecord[];
extern u16 g_MenuPadRecord[];
extern s16 g_MenuSoundRequest;
extern u16 g_MenuPadRecord[];
extern s16 g_MenuSoundRequest;

/* func_80075BD8 */
u16 MenuNoWalkRun(void) {
    u16 result;
    g_MenuBackOutLatch = 0;
    sprite_reset(0x40);
    g_MenuPageStage = 0;
    MenuNoWalkOpen();
    MenuPagesRun();
    MenuNoWalkClose();
    result = g_MenuRequest;
    g_MenuRequest = 0;
    g_MapIdleCount = 0;
    g_MenuBackOutLatch = 0;
    return result;
}
/* Runs one page of the menu, the one given as page (0x14 the load, 0x15 the options),
   for the start screen: raises the priority, clears the scroll and cursor
   state, sets the "modal" flag g_MenuOnePageRunning so MenuLoadOpen re-issues its
   window, hands control to MenuNoWalkRun, then puts the layers back and
   returns its result. */
/* func_80075C28 */
s32 StartMenuPageRun(s32 page) {
    u16 request;

    g_MenuBackOutLatch = 0;
    func_80056AE4(0x30, 1);
    layer_order_set(0x30, 5);
    layer_order_set(0x800, 5);
    layer_order_set(0x100, 4);
    g_MenuPage = page & 0xFFFF;
    g_MapEffect = 0;
    g_MapSecondViewY = 0;
    g_MapSecondViewX = 0;
    g_MapViewY = 0;
    g_MapViewX = 0;
    g_MapViewKeptY = 0;
    g_MapViewKeptX = 0;
    g_MenuOnePageRunning = 1;
    FaceListEntriesSet();
    request = MenuNoWalkRun();
    g_MenuOnePageRunning = 0;
    layer_order_set(0x930, 3);
    func_80056AE4(0x30, 0);
    g_MenuBackOutLatch = 0;
    return request & 0xFFFF;
}
/* far_syms: D_800B8EEC */
extern u16 g_MessageFaceSecond;
extern u16 g_MessageFaceFirst;
extern u16 g_MessageFacePlaces;
extern s32 D_800B8EEC;
extern u8 D_800E4E70[];
extern void MenuRecordsInit(void);
extern s32 pad_record_init(u16 *, s32, s32, s32);
extern s32 layer_mix_set(s32, s32);

/* Opens the menu screen when it is run on its own, outside the field menu:
   MenuNoWalkRun calls it once, before the page loop MenuPagesRun and the
   close MenuNoWalkClose. MenuNoWalkRun's callers are StartMenuPageRun (the
   title screen's Continue and Options, pages 0x14 / 0x15), the fight
   wrapper MapBtlRun after a fight that returned 1, and the field frame
   MapFrameUpdate when g_GameEnterRequest is set (those two on page 0x14, with
   music 0x17). It is the twin of the field menu's opening MenuOpen,
   without the party walk. No RNG draw; two frame passes.

   In order:
   - the text window state is cleared (face slots g_MessageFaceSecond / g_MessageFaceFirst,
     flags g_MessageFacePlaces) and so is g_MoveChrViewFollow, the camera group's flag of
     the script movement code;
   - anim actors 5 and 6 of the g_MenuRecords block get their +4 word cleared
     and are placed at x = g_MapViewX + 0x108, y = g_MapViewY + 0x28 and
     + 0x90, the menu origin being the one StartMenuPageRun zeroed;
   - sprite_run_off frees sprite slots 0x87..0x95 and MenuRecordsInit rebuilds
     the menu's display layer: the slices g_MenuShapes, the seven actors of
     g_MenuRecords and the render entries g_MenuRenderEntries;
   - layer_order_set(0x800, 5) and (0x100, 4), the pair StartMenuPageRun issued
     on the way in; CellAllClear(0x30) and the empty CellAllUpdate(0x20),
     the pair the field opening makes;
   - font_target_clear makes 0x801CBB00 the text target and clears 0xC0 << 7
     bytes of it, a frame passes (MenuFrameStep), font_target_set rewinds the
     target and font_string_proportional_put renders the thirteen strings 0x616..0x622
     into it, one after the other;
   - the first id of the list g_PartyIds becomes 0xC, the selected slot
     g_MenuMemberCursor becomes 1 and the page g_MenuPage is published in
     g_MenuRedrawRequest;
   - a second frame passes, pad_record_init re-arms the pad block g_MenuPadRecord
     with (0x14, 0xF0D0, 0) as the field opening does, and the three lines
     every menu opening ends on follow: D_800B8EEC |= 1, bits 0x30 of the
     word at D_800E4E70 + 0xE set to 0x10, and layer_mix_set called with
     0x10 and 0x10;
   - the pending menu sound g_MenuSoundRequest is cleared.

   Source shape (what the bytes need): the six window-block stores are
   aggregate MEMBER stores, written in field order: the two +4 clears
   first, then the x pair, then the y stores. Member stores let the
   scheduler lift the g_MapViewY load and the call's two argument
   constants above them (a cast store `*(u16 *)((u8 *)p + N)` does not:
   -12 rows). The clears-first order is what leaves the window pointer in
   $a2 and the y base in $v1 (the rows' own order, x before the clears, is
   -10): it puts the pointer's first use right after the four global zero
   stores, sched1 sinks the pointer's load above all of them, and the
   longer life ranks it below the y base in local-alloc. The chained x
   assignment stores right to left, +0x16E before +0x132. */
/* func_80075CE0 */
void MenuNoWalkOpen(void)
{
    s32 i;
    MenuScreenBits *ui;

    g_MessageFaceSecond = 0;
    g_MessageFaceFirst = 0;
    g_MessageFacePlaces = 0;
    g_MoveChrViewFollow = 0;
    ((MapRecord *)g_MenuRecords)[5].sprite.on = 0;
    ((MapRecord *)g_MenuRecords)[6].sprite.on = 0;
    AS(u16, ((MapRecord *)g_MenuRecords)[5].sprite.x) = AS(u16, ((MapRecord *)g_MenuRecords)[6].sprite.x) = g_MapViewX + 0x108;
    AS(u16, ((MapRecord *)g_MenuRecords)[5].sprite.y) = g_MapViewY + 0x28;
    AS(u16, ((MapRecord *)g_MenuRecords)[6].sprite.y) = g_MapViewY + 0x90;
    sprite_run_off(0x87, 0xF);
    MenuRecordsInit();
    layer_order_set(0x800, 5);
    layer_order_set(0x100, 4);
    CellAllClear(0x30);
    CellAllUpdate(0x20);
    font_target_clear((u8 *) 0x801CBB00, 0xC0);
    MenuFrameStep();
    font_target_set((u8 *) 0x801CBB00);
    for (i = 0x16; i < 0x23; i++) {
        font_string_proportional_put(i + 0x600);
    }
    *(u16 *)g_PartyIds = 0xC;
    g_MenuMemberCursor = 1;
    g_MenuRedrawRequest = g_MenuPage;
    MenuFrameStep();
    pad_record_init(g_MenuPadRecord, 0x14, 0xF0D0, 0);
    ui = (MenuScreenBits *)D_800E4E70;
    D_800B8EEC |= 1;
    ui->unk_0e = (ui->unk_0e & 0xFFCF) | 0x10;
    layer_mix_set(0x10, 0x10);
    g_MenuSoundRequest = 0;
}

/* func_80075E34 */
void MenuNoWalkClose(void) {
    CellAllClear(0x10);
    CellAllUpdate(0x10);
    sprite_run_off(0x87, 0xF);
    MenuFrameStep();
    font_target_clear((u8 *)0x801CBB00, 0x80);
    MenuPageDisp((u16)g_MenuPage);
    MenuFrameStep();
    g_MoveChrViewFollow = 1;
}
/* func_80075E9C */
void profile_draw_flip(void) {
    g_ProfileShown ^= 1;
}
