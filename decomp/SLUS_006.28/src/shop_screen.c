/* Lunar SSSC — shop_screen: the shop's screen opened and closed, its steps
   (buy or sell, member, item, deal), its list, line-up, faces, money and
   text, its frame and animations, and its memory. The first split had it in
   misc_c.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "path.h"
#include "shop.h"
#include "btl.h"
#include "map.h"
#include "sprite.h"
#include "menu.h"

/* far_syms: g_BtlMenuPadRecord (carried from the first split's misc.c) */
extern u8 g_ShopMemberCursor;
extern u8 g_ShopBuySell;  /* the shop's BUY / SELL choice: 0 = BUY, nonzero = SELL */
extern s32 g_ShopShapes;
extern s32 g_ShopShapeEntries;
extern s32 g_LinePaths;
extern u8 g_BtlMenuSoundRequest;
extern u16 g_MessageFaceFirst;
extern s32 g_PartyIds;
extern u8 g_ShopStep;
extern s32 g_ShopCurrentEntries;
extern s32 g_ShopWantedEntries;
extern u16 g_ShopOrigin[2];
extern u8 g_ShopStockRow;
extern s32 g_ShopKeptIds;
extern s32 g_ShopObjects;
extern s32 g_ShopKeptRecords;
extern s32 g_ShopListEntries;
extern u8 g_BtlMenuPadRecord[];
extern u8 count;
extern u8 entity_ai_table[];
extern s16 flags;
s32 xmalloc_heap_free(s32);
s32 font_target_clear(s32, s32);
s32 ShopOpen();
s32 ShopClose();
s32 CellNumberSet(s32, s32, s32, s32, s32, s32, s32, s32); /* extern */
extern u16 val;
/* far_syms: D_800B8EEC D_800B8964 g_PadHeldButtons g_PadSecondHeldButtons */
extern u8 g_ShopCursors_s;
extern s32 g_ChrItemList0C;
void ShopItemDisp();
void ShopBuySellDisp();
extern u8 g_ShopCursors[3];
extern u8 g_ShopDealWearable;
extern s32 g_ShopItemRow;
extern s16 g_MessageFacePlaces;
extern s16 g_MessageRow;
extern s32 g_ShopTextBlockCount;
extern u16 g_MapViewX;
extern u16 g_MapViewY;
extern u16 g_ShopOrigin[2];
extern u8 g_PartyCount;
extern s32 g_MapPartyRecords;
extern s32 g_MapPartyTwins;
extern s32 D_800B8B2C;
extern s32 D_800B8B28;
extern u8 g_ShopPlaceCount;
extern u8 D_800E4E70[];
extern s32 D_800B8EEC;
extern s16 D_800B8964;
extern s16 g_PadHeldButtons;
extern s16 g_PadSecondHeldButtons;
extern u8 D_800E4768[];
extern u16 g_MapObjectsSpriteBits;
extern s8 g_ShopStringIds[];
extern s32 g_ShopStringBlocks[];
extern s32 *g_GameMoney;
void sprite_run_off(s32, u32);
void ShopAlloc(void);  /* TU defines this below */
void ShopObjectInit(s32, s32, s32, s32, s32, s32);
void line_path_make(s16 *, s16, s16, s16, s16, s32);
void MapAnimationPoseSet(void *, s32, u8);
void CellAllClear(s32);
void BtlCellRowsUpdateCopy();  /* TU defines this below */
void layer_mix_switch_on(s32);
void layer_mix_set(s32, u16);
void layer_order_set(s32, s32);
s32 SoundPlay(u8, s32);
void ShopTextStringReport();  /* TU defines this below */
void ShopTextStringReportCopy();  /* TU defines this below */
void MapObjectsAnimationStep(void);
void ShopAnimationStep(s32, s32, s32);
s32 font_code_put(s32, s32);
void ShopFrameStep();
s32 font_col_set(s32);
void font_target_set(u8 *);
void font_string_put(s32, s32, s32);
void CellTextDisp(s32, s32, s32, s32, s32, s32, s32);
void ShopMoneyDisp(s32 amount, s32 column, s32 row, u16 col);  /* TU defines this below */
void ShopBuySellOpen(void);  /* TU defines this below */

/* Entry 3's y in the chained store is a pointer access through the
   member's address (*(s16 *)&...[3].y): written as a member store, GCC
   orders the chained store differently (the function grows by 0x10
   bytes). */
/* func_800506EC */
s32 ShopOpen(void)
{
    s32 i;
    s32 slot;
    s32 qx;
    s32 vx;
    s32 tx;
    s32 ty;
    s32 xo;
    s32 j;
    u8 c;
    u16 id;
    SpritePiece *pan;
    SpriteEntry *win;
    MenuScreenBits *ui;
    s32 *mus;
    s16 rc[4];

    g_MessageFacePlaces = 2;
    g_MessageFaceFirst = 0;
    g_MessageRow = 0x12;
    g_MessageFaceFirst = 0;
    g_MessageFacePlaces = 0;
    g_ShopTextBlockCount = 0;
    sprite_run_off(0x46, 6);
    g_ShopOrigin[0] = g_MapViewX;
    g_ShopOrigin[1] = g_MapViewY;
    ShopAlloc();
    for (i = 0; i < 12; i++) {
        ((ShopListEntry *)g_ShopListEntries)[i].unk_08 = 0x200000;
        AS(s16, ((ShopListEntry *)g_ShopListEntries)[i].column) = (i % 6) * 4 + 0xC;
        AS(s16, ((ShopListEntry *)g_ShopListEntries)[i].row) = (i / 6) * 4 + 0xB;
    }
    for (i = 0; i < g_PartyCount; i++) {
        ((u8 *)g_ShopKeptIds)[i] = ((u16 *)g_PartyIds)[i];
        ((MapRecord *)g_ShopKeptRecords)[i] = ((MapRecord *)g_MapPartyRecords)[i];
    }
    slot = 1;
    for (i = 0; i < g_PartyCount; i++) {
        c = ((u8 *)g_ShopKeptIds)[i];
        if (c >= 0x1F || c == 8 || c == 9 || c == 10) {
            ((u16 *)g_PartyIds)[5] = ((u8 *)g_ShopKeptIds)[i];
            ((MapRecord *)g_MapPartyRecords)[g_PartyCount - 1] = ((MapRecord *)g_ShopKeptRecords)[i];
            ((MapRecord *)g_MapPartyRecords)[g_PartyCount - 1].hold_definition.hold = 1;
            *(u16 *)(g_MapPartyRecords + g_PartyCount * 0x3C - 0x2E) |= 0x8000;
        } else if (c >= 1 && c <= 11) {
            ((u16 *)g_PartyIds)[slot] = ((u8 *)g_ShopKeptIds)[i];
            ((MapRecord *)g_MapPartyRecords)[slot] = ((MapRecord *)g_ShopKeptRecords)[i];
            AS(u16, ((MapRecord *)g_MapPartyRecords)[slot].sprite.depth) = 0x1000;
            ((MapRecord *)g_MapPartyRecords)[slot].sprite.flags |= 0x7000;
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[slot], 1, 5);
            xo = slot * 0x20 + 0x70;
            line_path_make((s16 *)((PathPoint *)g_LinePaths + slot * 16), ((MapRecord *)g_ShopKeptRecords)[i].sprite.x,
                          ((MapRecord *)g_ShopKeptRecords)[i].sprite.y, g_ShopOrigin[0] + xo,
                          g_ShopOrigin[1] + 0x28, 0x10);
            slot++;
        } else if (c == 12) {
            ((u16 *)g_PartyIds)[0] = ((u8 *)g_ShopKeptIds)[i];
            ((MapRecord *)g_MapPartyRecords)[0] = ((MapRecord *)g_ShopKeptRecords)[i];
            AS(u16, ((MapRecord *)g_MapPartyRecords)[0].sprite.depth) = 0x1000;
            ((MapRecord *)g_MapPartyRecords)[0].sprite.flags |= 0x7000;
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[0], 1, 5);
            qx = g_ShopOrigin[0];
            qx += 0x70;
            line_path_make((s16 *)g_LinePaths, ((MapRecord *)g_ShopKeptRecords)[i].sprite.x, ((MapRecord *)g_ShopKeptRecords)[i].sprite.y,
                          qx, g_ShopOrigin[1] + 0x28, 0x10);
        }
        ((MapRecord *)g_MapPartyTwins)[i].on_kind.on = 0;
    }
    g_ShopPlaceCount = slot;
    ShopObjectInit(g_ShopObjects, 0, D_800B8B2C, D_800B8B28, 0x7F0, 0);
    vx = g_ShopOrigin[0] + 0x20;
    ((MapRecord *)g_ShopObjects)[0].sprite.x = vx;
    ((MapRecord *)g_ShopObjects)[0].sprite.y = g_ShopOrigin[1] + 0x10;
    tx = D_800B8B2C;
    ty = D_800B8B28;
    ShopObjectInit(g_ShopObjects + 0x3c, 0, tx, ty, 0x7f0, 1);
    vx = g_ShopOrigin[0] + 0x20;
    ((MapRecord *)g_ShopObjects)[1].sprite.x = vx;
    ((MapRecord *)g_ShopObjects)[1].sprite.y = g_ShopOrigin[1] + 0x28;
    tx = D_800B8B2C;
    ty = D_800B8B28;
    ShopObjectInit(g_ShopObjects + 0x78, 0, tx, ty, 0x760, 12);
    tx = D_800B8B2C;
    ty = D_800B8B28;
    ShopObjectInit(g_ShopObjects + 0xb4, 0, tx, ty, 0x760, 8);
    tx = D_800B8B2C;
    ty = D_800B8B28;
    ShopObjectInit(g_ShopObjects + 0xf0, 0, tx, ty, 0x760, 10);
    tx = D_800B8B2C;
    ty = D_800B8B28;
    ShopObjectInit(g_ShopObjects + 0x12c, 0, tx, ty, 0x760, 4);
    vx = g_ShopOrigin[0] + 0x120;
    ((MapRecord *)g_ShopObjects)[5].sprite.x = vx;
    ((MapRecord *)g_ShopObjects)[5].sprite.y = g_ShopOrigin[1] + 0x58;
    tx = D_800B8B2C;
    ty = D_800B8B28;
    ShopObjectInit(g_ShopObjects + 0x168, 0, tx, ty, 0x760, 5);
    ((MapRecord *)g_ShopObjects)[6].sprite.x = g_ShopOrigin[0] + 0x120;
    ((MapRecord *)g_ShopObjects)[6].sprite.y = g_ShopOrigin[1] + 0x88;
    pan = (SpritePiece *)g_ShopShapes;
    win = (SpriteEntry *)g_ShopShapeEntries;
    i = 0;
    do {
        i++;
        AS(s16, win->on) = 0;
        win->depth = 0x1000;
        AS(s16, win->attr) = 0;
        AS(s16, win->flags) = 0;
        win->pose_table = 0;
        AS(s16, win->pose) = 0;
        win->piece_table = (s32)pan;
        AS(s16, win->scaled) = 0;
        win++;
        AS(s16, pan->kind) = 0x5000;
        pan->height = 0;
        pan->width = 0;
        pan++;
    } while (i < 4);
    ((SpriteEntry *)g_ShopShapeEntries)[0].y = g_ShopOrigin[1] + 0xB0;
    AS(s16, ((SpritePiece *)g_ShopShapes)[0].texel_col.col) = 0x361;
    ((SpritePiece *)g_ShopShapes)[0].y3 = 0x10;
    ((SpritePiece *)g_ShopShapes)[0].y2 = 0x10;
    ((SpritePiece *)g_ShopShapes)[0].y1 = 0;
    ((SpritePiece *)g_ShopShapes)[0].y0 = 0;
    ((SpritePiece *)g_ShopShapes)[0].x3 = 0;
    ((SpritePiece *)g_ShopShapes)[0].x0 = 0;
    AS(s16, ((SpritePiece *)g_ShopShapes)[0].kind) = 0x4000;
    ((SpriteEntry *)g_ShopShapeEntries)[1].x = ((SpriteEntry *)g_ShopShapeEntries)[2].x = ((SpriteEntry *)g_ShopShapeEntries)[3].x = g_ShopOrigin[0] + 0x60;
    ((SpriteEntry *)g_ShopShapeEntries)[1].y = *(s16 *)&((SpriteEntry *)g_ShopShapeEntries)[3].y = g_ShopOrigin[1] + 0x58;
    ((SpriteEntry *)g_ShopShapeEntries)[2].y = g_ShopOrigin[1] + 0x78;
    pan = (SpritePiece *)g_ShopShapes + 1;
    i = 1;
    do {
        i++;
        AS(s16, pan->texel_col.col) = 0x35E;
        pan->y1 = -3;
        pan->y0 = -3;
        pan->x3 = -3;
        pan->x0 = -3;
        pan->x2 = 0xBB;
        pan->x1 = 0xBB;
        pan++;
    } while (i < 4);
    ((SpritePiece *)g_ShopShapes)[2].y3 = 0x1A;
    ((SpritePiece *)g_ShopShapes)[2].y2 = 0x1A;
    ((SpritePiece *)g_ShopShapes)[1].y3 = 0x1A;
    ((SpritePiece *)g_ShopShapes)[1].y2 = 0x1A;
    ((SpritePiece *)g_ShopShapes)[3].y3 = 0x3A;
    ((SpritePiece *)g_ShopShapes)[3].y2 = 0x3A;
    for (i = 0; i < g_ShopPlaceCount; i++) {
        ((ShopLineUpEntry *)g_ShopWantedEntries)[i].row = ((ShopLineUpEntry *)g_ShopCurrentEntries)[i].row = 0;
        ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = ((ShopLineUpEntry *)g_ShopCurrentEntries)[i].pose = 1;
        ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing = ((ShopLineUpEntry *)g_ShopCurrentEntries)[i].facing = 5;
    }
    CellAllClear(0x30);
    BtlCellRowsUpdateCopy(0, 0, 0x20, 0x30);
    ui = (MenuScreenBits *)D_800E4E70;
    D_800B8EEC |= 1;
    ui->unk_0e = (ui->unk_0e & 0xFFCF) | 0x10;
    layer_mix_switch_on(0x10);
    layer_mix_set(0x10, 0xF);
    layer_order_set(0x800, 4);
    D_800B8964 = 0x10;
    g_PadHeldButtons = 0;
    g_PadSecondHeldButtons = 0;
    SoundPlay(5, 4);
    slot = 0;
    ShopTextStringReport(0xED5C, 0x604, slot, 0xA);
    ShopTextStringReport(0xED8C, 0x608, slot, 0xA);
    ShopTextStringReport(0xEDC8, 0x60A, slot, 0xA);
    ShopTextStringReport(0xEE10, 0x60D, slot, 0xA);
    ShopTextStringReport(0xED50, 0x603, slot, 0xA);
    ShopTextStringReportCopy(0xEDAC, 0x609, slot, 0xA);
    ShopTextStringReportCopy(0xEDF4, 0x60C, slot, 0xA);
    do {
        for (i = 0; i < g_ShopPlaceCount; i++) {
            ((MapRecord *)g_MapPartyRecords)[i].sprite.x = AS(s16, ((PathPoint *)g_LinePaths)[slot + i * 16].x);
            ((MapRecord *)g_MapPartyRecords)[i].sprite.y = AS(s16, ((PathPoint *)g_LinePaths)[slot + i * 16].y);
        }
        if (slot < 8) {
            ShopTextStringReport((u16)(slot * 16 - 0x11C0), (u16)(slot + 0x619), 0, 4);
        }
        switch (slot) {
        case 3:
            g_MapObjectsSpriteBits = 0x8000;
            MapObjectsAnimationStep();
            id = ((u16 *)g_PartyIds)[g_PartyCount - 1];
            if (id >= 0x1F || id == 8 || id == 9 || id == 10) {
                ShopAnimationStep((s32)&((MapRecord *)g_MapPartyRecords)[g_PartyCount - 1], g_PartyCount - 1, 1);
            }
            break;
        case 9:
            ShopTextStringReport(0xED68, 0x605, 0, 2);
            break;
        case 10:
            ShopTextStringReport(0xEE38, 0x614, 0, 2);
            break;
        case 11:
            ShopTextStringReport(0xEDEC, 0x60B, 0, 2);
            break;
        case 12:
            ShopTextStringReport(0xED74, 0x606, 0, 3);
            break;
        case 13:
            ShopTextStringReport(0xED80, 0x607, 0, 3);
            break;
        case 14:
            ShopTextStringReport(0xEE2C, 0x60E, 0, 2);
            font_target_clear(0x801CBB00 + g_ShopTextBlockCount * 0x80, 1);
            font_code_put(0xA, 0);
            g_ShopTextBlockCount++;
            mus = (s32 *)D_800E4768;
            mus[3] = g_ShopTextBlockCount << 7;
            break;
        }
        ShopFrameStep();
        slot++;
    } while (slot < 0x10);
    font_target_clear(0x801CBB00, 0x80);
    font_col_set(0);
    font_target_set((u8 *)0x801CC500);
    for (j = 0; j < 0x16; j++) {
        font_target_set((u8 *)(0x801CC500 + g_ShopStringBlocks[j] * 0x80));
        font_string_put((u16)(g_ShopStringIds[j] + 0x600), 0, 1);
    }
    CellTextDisp(0xC, 6, 8, 1, 8, 0xDEAD2001, 0xED00);
    CellTextDisp(0xC, 8, 0xC, 1, 0xC, 0xDEAD2001, 0xED20);
    CellTextDisp(3, 0xD, 3, 1, 3, 0xDEAD2001, 0xED80);
    BtlCellRowsUpdateCopy(0, 0, 0x20, 0x30);
    ShopMoneyDisp(*g_GameMoney, 3, 0xF, 0x3000);
    ShopBuySellOpen();
}

/* far_syms: D_800B8966 g_PadHeldButtons g_PadSecondHeldButtons D_800B8EEC */
extern u16 g_MessageFaceSecond;
extern s16 D_800B8966;
extern s32 g_MapLayersMask;
void ShopFree(void);  /* TU defines this below */

/* The SHOP screen's
   exit: ShopRun(shop) runs ShopOpen (open), ShopStepsRun (the
   shop's state loop) and this. It undoes what the opening did to the
   field actors.

   1. Text and layer reset: CellAllClear(0x30), func_800572D8(),
      g_MessageFaceSecond = g_MessageFaceFirst = g_MessageFacePlaces = 0, g_MessageRow = 0x14, the text
      buffer cleared (font_target_clear(0x801CBB00, 0x55)) and uploaded through
      the D_800E4768 request (address 0x801CBB00, size 0x2A80, flag 1),
      BtlCellRowsUpdateCopy(0, 0, 0x20, 0x30), D_800B8966 = 0x10 and
      layer_mix_set(0x10, 0x20).
   2. The shop's own objects go: the first byte of the six g_ShopObjects
      records and f0 of the four g_ShopShapeEntries windows are cleared, sprite
      slots 0x6E-0x78 freed (sprite_run_off(0x6E, 0xB)).
   3. The walk back: for each of the g_ShopPlaceCount actors the opening lined
      up (j) and each saved actor (i) with the same id (g_PartyIds[j] ==
      g_ShopKeptIds[i]), line_path_make builds 16 points in g_LinePaths row j
      from the actor's current x / y to its saved x / y. Sound
      SoundPlay(6, 4). Then 16 frames (ShopFrameStep each): frame f
      puts every lined-up actor on point f of its row; frame 10 also
      sets g_MapObjectsSpriteBits = 0 and calls MapObjectsAnimationStep().
   4. More ShopFrameStep frames while layer_col_fade_check(0) & 0xFF (the fade
      still running).
   5. layer_mix_switch_off(0x10), layer_order_set(0x800, 1), D_800B8EEC |= 1,
      g_PadHeldButtons = g_PadSecondHeldButtons = 0, bits 0x30 of the u16 at D_800E4E70 +
      0xE cleared.
   6. The actors are restored from the opening's backup: for i <
      g_PartyCount the id (g_PartyIds[i] = g_ShopKeptIds[i]) and the whole
      0x3C record (g_MapPartyRecords[i] = g_ShopKeptRecords[i]) come back and
      g_MapPartyTwins[i]'s first byte is set to 1; the rows from there up to 6
      get id 0 and both first bytes 0.
   7. ShopFree() (the shop's buffers), sprite slots 0x46-0x4B and
      0-5 freed, MessageDisp with the code string { 0xFF01, 0xFFFF } (a
      new page: the message window is cleared), and
      layer_col_set(-1, g_MapLayersMask & 0x30, 0).

   No RNG draw of its own. Its frames are the ShopFrameStep calls (16 plus
   the fade's remainder) and whatever MessageDisp runs; the only route
   to rng_draw is the frame flip's vibration driver (ShopFrameStep ->
   MapFrameEndCopy -> MapFrameEnd -> frame_end -> pad_frame_step ->
   pad_update; ShopFrameStep's own comment has the frame in full).

   Shape: (1) `msg` is an INITIALISED array - store_constructor's CLOBBER
   of the whole BLKmode slot conflicts with every $sp store, so sched2
   keeps the two `sh` after the register saves; two plain assignments
   float above them (247 -> 251). (2) The D_800E4768 request goes through
   a pointer local (`lui $t0; addiu $v0,$t0,%lo`, words 1-3 off $v0, word
   0 through %lo($t0)); direct ((s32 *)SYM)[n] bases the block at SYM + 4
   (240 -> 247). (3) The D_800E4E70 + 0xE word is a STRUCT field
   (MenuScreenBits' unk_0e): a struct store with a varying address does not
   conflict with the scalar
   g_PartyCount read of the loop's entry test, which lets sched1 lift that
   `lbu` above all four stores; as *(u16 *)(ui + 0xE) the load stays last
   and no statement order moves it (251 -> 258; 180 orders scored). */
/* func_800512C4 */
s32 ShopClose(void)
{
    s32 i;
    s32 j;
    MenuScreenBits *ui;
    s32 *mus;
    u16 msg[2] = { 0xFF01, 0xFFFF };

    CellAllClear(0x30);
    func_800572D8();
    g_MessageFaceSecond = 0;
    g_MessageFaceFirst = 0;
    g_MessageFacePlaces = 0;
    g_MessageRow = 0x14;
    font_target_clear(0x801CBB00, 0x55);
    mus = (s32 *)D_800E4768;
    mus[1] = 0x801CBB00;
    mus[2] = 0;
    mus[3] = 0x2A80;
    mus[0] = 1;
    BtlCellRowsUpdateCopy(0, 0, 0x20, 0x30);
    D_800B8966 = 0x10;
    layer_mix_set(0x10, 0x20);
    for (i = 0; i < 6; i++) {
        ((MapRecord *)g_ShopObjects)[i].on_kind.on = 0;
    }
    for (i = 3; i >= 0; i--) {
        AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[i].on) = 0;
    }
    sprite_run_off(0x6E, 0xB);
    for (j = 0; j < g_ShopPlaceCount; j++) {
        for (i = 0; i < g_PartyCount; i++) {
            if (((u16 *)g_PartyIds)[j] == ((u8 *)g_ShopKeptIds)[i]) {
                line_path_make((s16 *)((PathPoint *)g_LinePaths + j * 16), ((MapRecord *)g_MapPartyRecords)[j].sprite.x,
                              ((MapRecord *)g_MapPartyRecords)[j].sprite.y, ((MapRecord *)g_ShopKeptRecords)[i].sprite.x,
                              ((MapRecord *)g_ShopKeptRecords)[i].sprite.y, 0x10);
            }
        }
    }
    SoundPlay(6, 4);
    for (j = 0; j < 0x10; j++) {
        for (i = 0; i < g_ShopPlaceCount; i++) {
            ((MapRecord *)g_MapPartyRecords)[i].sprite.x = AS(s16, ((PathPoint *)g_LinePaths)[j + i * 16].x);
            ((MapRecord *)g_MapPartyRecords)[i].sprite.y = AS(s16, ((PathPoint *)g_LinePaths)[j + i * 16].y);
        }
        if (j == 10) {
            g_MapObjectsSpriteBits = 0;
            MapObjectsAnimationStep();
        }
        ShopFrameStep();
    }
    while (layer_col_fade_check(0) & 0xFF) {
        ShopFrameStep();
    }
    layer_mix_switch_off(0x10);
    layer_order_set(0x800, 1);
    ui = (MenuScreenBits *)D_800E4E70;
    D_800B8EEC |= 1;
    g_PadHeldButtons = 0;
    g_PadSecondHeldButtons = 0;
    ui->unk_0e &= 0xFFCF;
    for (i = 0; i < g_PartyCount; i++) {
        ((u16 *)g_PartyIds)[i] = ((u8 *)g_ShopKeptIds)[i];
        ((MapRecord *)g_MapPartyRecords)[i] = ((MapRecord *)g_ShopKeptRecords)[i];
        ((MapRecord *)g_MapPartyTwins)[i].on_kind.on = 1;
    }
    for (; i < 6; i++) {
        ((u16 *)g_PartyIds)[i] = 0;
        ((MapRecord *)g_MapPartyRecords)[i].on_kind.on = 0;
        ((MapRecord *)g_MapPartyTwins)[i].on_kind.on = 0;
    }
    ShopFree();
    sprite_run_off(0x46, 6);
    sprite_run_off(0, 6);
    MessageDisp(msg, 0);
    layer_col_set(-1, g_MapLayersMask & 0x30, 0);
}

void ShopTextStringReportCopy();  /* TU defines this below */
void ShopTextStringReport();  /* TU defines this below */
void ShopItemStringsClear(s32 unused, s32 block_count);  /* TU defines this below */
void BtlCellRowsUpdateCopy();  /* TU defines this below */
s32 BtlCellAreaClearCopy(s32, s32, s32, s32, s32, s32);
s32 CellGlyphSet(s32, s32, s32, s32, s32, s32);
void ShopMoneyDisp(s32 amount, s32 column, s32 row, u16 col);  /* TU defines this below */
s32 MessageFaceEntrySet(s32, u8, s32, u16, s32);
void ShopFaceClear(s32 keep_face);  /* TU defines this below */
s32 xmalloc_heap_alloc(s32);
void ShopAlloc(void);  /* TU defines this below */
void ShopFree(void);  /* TU defines this below */
s32 pad_record_update(s32 *, s32);
void ShopFrameStep();
s32 SoundPlay(u8, s32);

/* func_800516CC */
s32 ShopStepsRun(void)
{
  int step_index;
  g_ShopStep = 1;
  do
  {
    pad_record_update((s32 *)g_BtlMenuPadRecord, 0x10);
    step_index = g_ShopStep - 1;
    ((s32 (*)()) (*(s32 *)((step_index * 4) + entity_ai_table)))();
    if (g_BtlMenuSoundRequest != 0)
    {
      SoundPlay(g_BtlMenuSoundRequest, 3);
      g_BtlMenuSoundRequest = 0;
    }
    ShopFrameStep();
  }
  while (g_ShopStep != 0);
  return (s32) g_ShopStep;
}

void ShopLineUpMove(void);  /* TU defines this below */
void ShopItemTextDisp(void);  /* TU defines this below */
extern u8 g_ShopCursors[3];
extern u8 g_ShopMemberItemCount[2];
extern s32 g_ChrRecords;
u32 ShopStockGet(u32 stock_row);
void ShopListDisp(u16 *entry, s32 entry_count);  /* TU defines this below */

/* The shop's list: fill the 12-cell list (g_ShopListEntries, 12 bytes a cell).
   buy_sell != 0 is the list view of the member under the cursor
   (g_PartyIds[g_ShopMemberCursor]): id 0xC is the bag (*g_ChrItemList0C, 96 slots, shown
   two rows of 6 from scroll row g_ShopCursors[0]; g_ShopMemberItemCount[0] counts the
   filled slots, the scroll row is walked back off empty rows, and in list
   mode g_ShopStep >= 2 the up/down arrows g_ShopObjects[5]/[6] are switched
   on, each taking its animation's timing from the other when that one is
   showing: the other's timer and step length (MapRecord's x and y, +0x34 and
   +0x36) and a step (w, +0x38) as far from its own end (h, +0x3A) as the
   other's step is from the other's end); any
   other member shows the six carried items (record +0x1E) and the six at
   +0x24, g_ShopMemberItemCount[0] = 12 when any is set (the second loop tests
   buf[i], not buf[i + 6], as the original does). arg0 == 0 is the shop's
   stock: ShopStockGet(g_ShopStockRow)'s six ids, minus id 0xB5
   when any party member carries 0xB5 (the inner loop reuses the outer
   loop's counter, so it stops the scan after the first hit). */
/* func_80051768 */
void ShopListSet(u8 buy_sell)
{
    u8 buf[12];
    u16 i;
    s32 j;
    u8 *p;
    s32 k;

    if (buy_sell != 0) {
        if (((u16 *)g_PartyIds)[g_ShopMemberCursor] == 0xC) {
            g_ShopMemberItemCount[0] = 0;
            AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[2].on) = 0;
            AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[1].on) = 0;
            AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[3].on) = 1;
            for (i = 0; i < 0x60; i++) {
                if (((u8 *)g_ChrItemList0C)[i] != 0) {
                    g_ShopMemberItemCount[0]++;
                }
            }
            if (g_ShopCursors_s != 0) {
                while (((u8 *)g_ChrItemList0C)[g_ShopCursors_s * 6] == 0) {
                    g_ShopCursors_s--;
                    if (g_ShopCursors_s == 0) {
                        break;
                    }
                }
            }
            p = (u8 *)(g_ChrItemList0C + g_ShopCursors_s * 6);
            for (i = 0; i < 12; i++) {
                buf[i] = p[i];
            }
            if (g_ShopStep >= 2) {
                if (g_ShopCursors_s != 0) {
                    if (((MapRecord *)g_ShopObjects)[6].on_kind.on != 0) {
                        ((MapRecord *)g_ShopObjects)[5].timer = ((MapRecord *)g_ShopObjects)[6].timer;
                        ((MapRecord *)g_ShopObjects)[5].step_length = ((MapRecord *)g_ShopObjects)[6].step_length;
                        ((MapRecord *)g_ShopObjects)[5].step = ((MapRecord *)g_ShopObjects)[5].end -
                            (((MapRecord *)g_ShopObjects)[6].end - ((MapRecord *)g_ShopObjects)[6].step);
                    }
                    ((MapRecord *)g_ShopObjects)[5].on_kind.on = 1;
                } else {
                    ((MapRecord *)g_ShopObjects)[5].on_kind.on = 0;
                }
                if (g_ShopCursors_s < 14 && ((u8 *)g_ChrItemList0C)[g_ShopCursors_s * 6 + 0xC] != 0) {
                    if (((MapRecord *)g_ShopObjects)[5].on_kind.on != 0) {
                        ((MapRecord *)g_ShopObjects)[6].timer = ((MapRecord *)g_ShopObjects)[5].timer;
                        ((MapRecord *)g_ShopObjects)[6].step_length = ((MapRecord *)g_ShopObjects)[5].step_length;
                        ((MapRecord *)g_ShopObjects)[6].step = ((MapRecord *)g_ShopObjects)[6].end -
                            (((MapRecord *)g_ShopObjects)[5].end - ((MapRecord *)g_ShopObjects)[5].step);
                    }
                    ((MapRecord *)g_ShopObjects)[6].on_kind.on = 1;
                } else {
                    ((MapRecord *)g_ShopObjects)[6].on_kind.on = 0;
                }
            }
        } else {
            ((MapRecord *)g_ShopObjects)[5].on_kind.on = ((MapRecord *)g_ShopObjects)[6].on_kind.on = 0;
            g_ShopMemberItemCount[0] = 0;
            AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[2].on) = 1;
            AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[1].on) = 1;
            AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[3].on) = 0;
            p = (u8 *)(g_ChrRecords + ((u16 *)g_PartyIds)[g_ShopMemberCursor] * 0x40 + 0x1E);
            for (i = 0; i < 6; i++) {
                buf[i] = p[i];
                if (buf[i] != 0) {
                    g_ShopMemberItemCount[0] = 0xC;
                }
            }
            p = (u8 *)(g_ChrRecords + ((u16 *)g_PartyIds)[g_ShopMemberCursor] * 0x40 + 0x24);
            for (i = 0; i < 6; i++) {
                buf[i + 6] = p[i];
                if (buf[i] != 0) {
                    g_ShopMemberItemCount[0] = 0xC;
                }
            }
        }
    } else {
        p = (u8 *)ShopStockGet(g_ShopStockRow);
        for (i = 0; i < 6; i++) {
            buf[i] = p[i];
        }
        for (i = 6; i < 12; i++) {
            buf[i] = 0;
        }
        AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[1].on) = 1;
        AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[3].on) = 0;
        AS(s16, ((SpriteEntry *)g_ShopShapeEntries)[2].on) = 0;
        for (j = 0; j < g_PartyCount; j++) {
            k = ((u16 *)g_PartyIds)[j] * 0x40;
            for (i = 0; i < 6; i++) {
                if (*(u8 *)(k + g_ChrRecords + i + 0x24) == 0xB5) {
                    for (i = 0; i < 6; i++) {
                        if (buf[i] == 0xB5) {
                            buf[i] = 0;
                        }
                    }
                }
            }
        }
    }
    for (i = 0; i < 12; i++) {
        if (buf[i] != 0) {
            AS(s16, ((ShopListEntry *)g_ShopListEntries)[i].on) = 1;
            AS(s16, ((ShopListEntry *)g_ShopListEntries)[i].number) = buf[i];
        } else {
            AS(s16, ((ShopListEntry *)g_ShopListEntries)[i].on) = 0;
        }
    }
    ShopListDisp((u16 *)g_ShopListEntries, 0xC);
}

extern u16 g_ShopOrigin[2];

/* ShopBuySellDisp -- the shop's top choice shown: the pointer goes to the
   chosen one of the two choice objects and the other one is shaded.
   Callers: ShopBuySellOpen (which then switches records 0-2 on) and
   ShopBuySellStep, right after it flips the choice on pad bits 0x5000.

   The choice is g_ShopBuySell, the byte after the member cursor g_ShopMemberCursor:
   0 = the first object, nonzero = the second, which ShopMemberDisp and
   ShopItemDisp read as SELL mode. The objects are the
   0x3C-byte records of g_ShopObjects that ShopOpen builds: record 0 at
   (g_ShopOrigin[0] + 0x20, g_ShopOrigin[1] + 0x10), record 1 at (+ 0x20,
   + 0x28), record 2 the pointer (OutlineMapStep draws the animated marker
   over it every shop frame).

   In order:
   1. Record 2: x (+0x7E) = g_ShopOrigin[0] + 0x20 and y (+0x82) =
      g_ShopOrigin[1] + (choice * 3 + 2) * 8, that is 0x10 or 0x28 below the
      base: the place of record 0 or of record 1.
   2. choice != 0: record 0's byte +1 = 1 and bit 15 of its attr word
      (+0xE) set, then record 1's byte +1 (+0x3D) = 0 and bit 15 of its
      attr word (+0x4A) cleared. choice == 0: the same four stores with
      the two records exchanged.
   Bit 15 of the attr word is the low bit of the renderer's colour index
   (sprite_draw: col = attr >> 15 | ..., the slices drawn with
   g_SpriteCols[col]), so the object NOT chosen is drawn with colour entry 1.
   Byte +1 is MapRecord's `hold`; the shop's own tick ShopAnimationStep does not
   test it and its reader was not traced.
   No call: no frame wait and no RNG draw. No return value ($v0 is the
   last word stored and both callers ignore it).

   Shape, each measured by changing it alone:
   - In each arm the attr word of record 0 comes BEFORE the byte of
     record 1. A byte store through a pointer makes cse forget every
     remembered load (cse.c note_mem_written: a QImode store may alias a
     scalar), a halfword member store does not, so g_ShopObjects is re-read
     after each `sb` and after no `sh`: the target's `lhu 0xE($v1)` and
     `sb 0x3D($v1)` share the load that follows the first byte store.
     With both bytes first, then both words: 22/40.
   - The head. The target loads g_ShopObjects, the x base and the choice byte
     in that order ahead of the first store, and tests the choice in the
     same register after both stores. Record 2's two stores and the test
     written straight off g_ShopObjects and g_ShopBuySell give that order with
     no local.
   - The choice byte is a variable of its own. Read as an element of its
     neighbour (the byte after `&g_ShopMemberCursor`) it is an aggregate element
     (expr.c): the first `sh` makes cse forget it and it is loaded twice,
     30/40, and the text then needs a pointer local and a value local to
     get back to 40/40.
   Under the cc272 marker this text gives the Japanese twin's shape
   (0x80055148: the same order with one shared `jr $ra`), which confirms
   the arm order. */
/* func_80051C5C */
void ShopBuySellDisp(void)
{
    ((MapRecord *)g_ShopObjects)[2].sprite.x = g_ShopOrigin[0] + 0x20;
    ((MapRecord *)g_ShopObjects)[2].sprite.y = g_ShopOrigin[1] + (g_ShopBuySell * 3 + 2) * 8;
    if (g_ShopBuySell != 0) {
        ((MapRecord *)g_ShopObjects)[0].hold_definition.hold = 1;
        ((MapRecord *)g_ShopObjects)[0].sprite.flags |= 0x8000;
        ((MapRecord *)g_ShopObjects)[1].hold_definition.hold = 0;
        ((MapRecord *)g_ShopObjects)[1].sprite.flags &= 0x7FFF;
    } else {
        ((MapRecord *)g_ShopObjects)[0].hold_definition.hold = 0;
        ((MapRecord *)g_ShopObjects)[0].sprite.flags &= 0x7FFF;
        ((MapRecord *)g_ShopObjects)[1].hold_definition.hold = 1;
        ((MapRecord *)g_ShopObjects)[1].sprite.flags |= 0x8000;
    }
}

extern u8 g_ShopPlaceCount;

/* func_80051CFC */
void ShopBuySellOpen(void)
{
    u16 i;

    i = 0;
    if (g_ShopPlaceCount)
    {
        do
        {
            *(u8 *)((i << 2) + g_ShopWantedEntries) = 0;
            ((u8 *)g_ShopWantedEntries)[(i << 2) + 1] = 1;
            i++;
        } while (i < g_ShopPlaceCount);
    }

    ShopLineUpMove();
    ShopListSet(0);
    ShopBuySellDisp();

    ((u8 *)g_ShopObjects)[0] = 1;
    ((u8 *)g_ShopObjects)[0x3C] = 1;
    ((u8 *)g_ShopObjects)[0x78] = 1;
}

void ShopMemberDisp();
s32 ShopAnimationSet(s32, s32, s32);

/* func_80051D98 */
void ShopBuySellStep(void)
{
  u8 *pad_record;
  pad_record = (u8 *) (&g_BtlMenuPadRecord);
  if (((short)(AS(s32, ((PadRecord *)pad_record)->repeated))) & 0x5000)
  {
    g_BtlMenuSoundRequest = 1;
    g_ShopBuySell ^= 1;
    ShopBuySellDisp();
    return;
  }
  if (((PadRecord *)pad_record)->pressed == 0x10)
  {
    g_BtlMenuSoundRequest = 3;
    g_ShopStep = 0;
    return;
  }
  if (((PadRecord *)pad_record)->pressed == 0x40)
  {
    g_BtlMenuSoundRequest = 2;
    ShopFaceClear(1);
    ShopAnimationSet(g_ShopObjects + 0x78, 0xD, 1);
    ShopAnimationSet(g_ShopObjects + 0xB4, 8, 1);
    g_ShopStep = 2;
    ShopMemberDisp();
  }
}

extern u16 g_ShopOrigin[2];
extern u8 g_ShopMemberItemCount[2];
s32 ShopFaceSet(s32 chr_id_then_face, s32 places, s32 old_face, s32 unused);

/* ShopMemberDisp -- the shop's member pick (callers: ShopBuySellStep on the way
   into the member row, ShopMemberStep after every cursor step, ShopItemStep
   on the way back out of the item list).

   1. The member cursor g_ShopMemberCursor is clamped: at or past the line-up count
      g_ShopPlaceCount it becomes 1 (not 0, and not count - 1).
   2. The wanted line-up records (g_ShopWantedEntries, 4 bytes an actor, see
      ShopLineUpMove) are written for all g_ShopPlaceCount actors: the member under
      the cursor gets {a = 1, b = 1}, everyone else {a = 0, b = 2}, and c = 5
      for all. ShopLineUpMove() then walks them there over its four frames,
      so the picked member stands one 8-pixel row below the others.
   3. The member-cursor sprite, record 3 of g_ShopObjects, is switched on and
      put over that member: x = g_ShopOrigin[0] + (g_ShopMemberCursor * 4 + 0xD) * 8,
      y = g_ShopOrigin[1] + 0x14.
   4. g_MessageFaceFirst = ShopFaceSet(low byte of the member's id
      ((u16 *)g_PartyIds)[g_ShopMemberCursor], 1, g_MessageFaceFirst, 0).
   5. SELL mode only (g_ShopBuySell != 0): the 0x18 by 6 cell rectangle
      at cell (0xC, 0x14) is cleared (BtlCellAreaClearCopy, mode 0x20),
      ShopListSet refills the 12-cell list with what this member carries,
      and when it comes back empty (g_ShopMemberItemCount[0] == 0) the 7-cell label
      strip 0xEDF4 is drawn at (0xC, 0x14); then
      BtlCellRowsUpdateCopy(0x14, 0x500, 6, 0x20) (an empty function).
   Frames: ShopLineUpMove's four. No RNG draw of its own.

   Shape:
   - i is a plain int and the loop a `for`; the records are indexed as
     ShopLineUpEntry [i] at every store (g_ShopWantedEntries reloaded before each one, the
     i * 4 kept by strength reduction -- the `addu $v1, $a0, $zero` after
     the three hoisted constants).
   - The x term is (g_ShopMemberCursor * 4 + 0xD) * 8, ShopItemDisp's cell form.
     Written g_ShopMemberCursor * 0x20 + 0x68 the sum associates the other way
     (103/109, or 100/109 with the product and 0x68 in parentheses).
   - The id argument is the u16 entry narrowed to a byte: without the (u8)
     the load is lhu (108/109).
   - `sllv $a0, $a0, $a1` is the plain [g_ShopMemberCursor] index: the scheduler
     lifts the call's `li $a1, 1` to the top of the block and the
     post-reload cse reuses that register for the shift by 1. No variable
     shift in the source. */
/* func_80051E60 */
void ShopMemberDisp(void)
{
    s32 i;

    if (g_ShopMemberCursor >= g_ShopPlaceCount) {
        g_ShopMemberCursor = 1;
    }
    for (i = 0; i < g_ShopPlaceCount; i++) {
        if (i != g_ShopMemberCursor) {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].row = 0;
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = 2;
        } else {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].row = 1;
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = 1;
        }
        ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing = 5;
    }
    ShopLineUpMove();
    ((MapRecord *)g_ShopObjects)[3].on_kind.on = 1;
    ((MapRecord *)g_ShopObjects)[3].sprite.x = g_ShopOrigin[0] + (g_ShopMemberCursor * 4 + 0xD) * 8;
    ((MapRecord *)g_ShopObjects)[3].sprite.y = g_ShopOrigin[1] + 0x14;
    g_MessageFaceFirst = ShopFaceSet((u8)((u16 *)g_PartyIds)[g_ShopMemberCursor], 1, g_MessageFaceFirst, 0);
    if (g_ShopBuySell != 0) {
        BtlCellAreaClearCopy(0xC, 0x14, 0xCC00, 0x18, 6, 0x20);
        ShopListSet(g_ShopBuySell);
        if (g_ShopMemberItemCount[0] == 0) {
            CellTextDisp(0xC, 0x14, 7, 1, 7, 0xDEAD2001, 0xEDF4);
        }
        BtlCellRowsUpdateCopy(0x14, 0x500, 6, 0x20);
    }
}

extern long HWD0, VWD0;
extern u16 g_ShopOrigin[2];
extern u8 g_ShopCursors[3];
extern u16 g_ShopOrigin[2];
extern u8 g_ShopCursors[3];
extern u8 g_ShopMemberItemCount[2];
extern u8 g_ShopCursors[3];
extern u8 g_ShopMemberItemCount[2];
extern u8 g_ShopCursors[3];
extern u8 g_ShopMemberItemCount[2];
extern u8 g_ShopCursors[3];
void ShopListCursorAdjust(s32 stepped_right);  /* TU defines this below */

/* func_80052014 */
void ShopMemberStep(void)
{
  u8 *pad;
  u16 btn;
  pad = g_BtlMenuPadRecord;
  btn = ((PadRecord *)pad)->repeated;
  if (btn & 0xA00C)
  {
    g_BtlMenuSoundRequest = 1;
    if (btn & 0x2008)
    {
      if (g_ShopMemberCursor >= (g_ShopPlaceCount - 1))
      {
        g_ShopMemberCursor = 0;
      }
      else
      {
        g_ShopMemberCursor++;
      }
    }
    else
      if (g_ShopMemberCursor == 0)
    {
      g_ShopMemberCursor = g_ShopPlaceCount - 1;
    }
    else
    {
      g_ShopMemberCursor--;
    }
    ShopMemberDisp();
    return;
  }
  btn = ((PadRecord *)pad)->pressed;
  if (btn & 0x10)
  {
    g_BtlMenuSoundRequest = 3;
    g_ShopStep = 1;
    ShopAnimationSet(g_ShopObjects + 0x78, 0xC, 1);
    ((u8 *) g_ShopObjects)[0xB4] = (((u8 *) g_ShopObjects)[0x12C] = (((u8 *) g_ShopObjects)[0x168] = 0));
    BtlCellAreaClearCopy(0xC, 0x14, 0xCC00, 0x18, 6, 0x20);
    BtlCellRowsUpdateCopy(0x14, 0x500, 6, 0x20);
    ShopFaceClear(0);
    ShopBuySellOpen();
    return;
  }
  if (btn & 0x40)
  {
    g_BtlMenuSoundRequest = 2;
    g_ShopStep = 3;
    if (g_ShopBuySell != 0)
    {
      ShopListSet(g_ShopBuySell);
      ShopMemberLineUpSet();
    }
    else
    {
      ShopListCursorAdjust(0);
      CellTextDisp(3, 9, 3, 1, 3, 0xDEAD2001, 0xEE2C);
      BtlCellRowsUpdateCopy(9, 0x240, 2, 0x20);
    }
    ShopItemDisp();
    ShopAnimationSet(g_ShopObjects + 0xB4, 9, 1);
    ShopAnimationSet(g_ShopObjects + 0xF0, 0xA, 1);
    return;
  }
}

extern u16 g_ShopOrigin[2];

/* The shop's BUY-mode line-up
   refresh: every lined-up member reacts to the item under the list cursor.
   Callers: ShopItemDisp once it has worked out the purchase flags,
   ShopItemStep and ShopDealRun after a refusal message (ShopRefusalDisp).

   1. The wanted line-up records (g_ShopWantedEntries, 4 bytes an actor, see
      ShopLineUpMove) are written for all g_ShopPlaceCount actors; id =
      ((u16 *)g_PartyIds)[i]:
        a  1 for the member under the member cursor g_ShopMemberCursor, else 0 (the
           picked member stands one 8-pixel row below the others);
        b  the item is equipment (kind, byte +4 of the g_ShopItemRow record,
           < 5): 1 when this member may equip it (bit id - 1 of the u16
           who mask at +0x10), else 2; any other kind: 1;
        c  5 when ChrItemRoomCheck(id) == 1 (the member has an empty inventory
           cell), else 1.
      b and c are what ShopLineUpMove hands to MapAnimationPoseSet(actor,
      b, c) once the walk is over.
   2. ShopLineUpMove() walks them there over its four frames.
   3. The member-cursor sprite, record 3 of g_ShopObjects, is switched on and
      put over the picked member: x = g_ShopOrigin[0] + (g_ShopMemberCursor * 4 +
      0xD) * 8, y = g_ShopOrigin[1] + 0x14.
   4. g_MessageFaceFirst = ShopFaceSet(low byte of the picked member's id, 1,
      g_MessageFaceFirst, 0).
   Steps 3 and 4 are ShopMemberDisp's, statement for statement.
   Frames: ShopLineUpMove's four. No RNG draw of its own.

   Shape:
   - i is a u16 (the andi 0xFFFF at each use; its zero-extension is the
     value kept in $s0 across the ChrItemRoomCheck call) and the loop a
     `for`. As a u8 108/113, s16 92, s32 57.
   - Every 1 is a plain literal; the target's two hoisted registers come
     out by themselves ($s3 for `.a = 1` and the other-kind `.b = 1`; $s2
     for the shift's 1, the may-equip `.b = 1`, the `== 1` compare and
     `.c = 1`). No variable holds a 1.
   - `can` is a variable: testing the `&` directly lets fold rewrite it
     as (who >> (id - 1)) & 1 (94/113; ShopItemDisp has the same).
   - The cursor sprite's x and y are STRUCT MEMBER stores. g_MessageFaceFirst, the
     call's third argument, is a scalar at a fixed address, so the
     scheduler may lift its load above a varying-address store that is "in
     a struct" (true_dependence in sched.c), and it lands before the y
     store. Written *(s16 *)(g_ShopObjects + 0xBE) the store is not in a
     struct, the load has to stay below it and trades places with the
     index shift (111/113; the same with only the x store written that
     way).
   - The x term is the cell form (g_ShopMemberCursor * 4 + 0xD) * 8, as in
     ShopMemberDisp and ShopItemDisp: 0x68 is added to the shifted index,
     the base last. Written g_ShopMemberCursor * 0x20 + 0x68 no order of the three
     terms matches (106 to 110/113: 0x68 is added to g_ShopOrigin[0]); with
     << 5, g_ShopMemberCursor is loaded twice as well (101).
   - The id argument is the u16 entry narrowed to a byte: without the (u8)
     the load is lhu (112/113).
   - `i != g_ShopMemberCursor` comes first (112/113 the other way round), and the
     room test is `== 1` (104/113 as `!= 0`).
   - ChrItemRoomCheck is called undeclared, as everywhere in this file: the
     u16 id is promoted, not masked. */
/* func_800521F8 */
void ShopItemLineUpSet(void)
{
    u16 i;
    s32 can;

    for (i = 0; i < g_ShopPlaceCount; i++) {
        if (i != g_ShopMemberCursor) {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].row = 0;
        } else {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].row = 1;
        }
        if (((BtlItemRow *)g_ShopItemRow)->kind < 5) {
            can = ((BtlItemRow *)g_ShopItemRow)->wearers & (1 << (((u16 *)g_PartyIds)[i] - 1));
            if (can) {
                ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = 1;
            } else {
                ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = 2;
            }
        } else {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = 1;
        }
        if (ChrItemRoomCheck(((u16 *)g_PartyIds)[i]) == 1) {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing = 5;
        } else {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing = 1;
        }
    }
    ShopLineUpMove();
    ((MapRecord *)g_ShopObjects)[3].on_kind.on = 1;
    ((MapRecord *)g_ShopObjects)[3].sprite.x = g_ShopOrigin[0] + (g_ShopMemberCursor * 4 + 0xD) * 8;
    ((MapRecord *)g_ShopObjects)[3].sprite.y = g_ShopOrigin[1] + 0x14;
    g_MessageFaceFirst = ShopFaceSet((u8)((u16 *)g_PartyIds)[g_ShopMemberCursor], 1, g_MessageFaceFirst, 0);
}

extern u16 g_ShopOrigin[2];

/* ShopMemberLineUpSet -- the shop's SELL-mode line-up refresh (callers:
   ShopMemberStep on confirm and ShopItemStep after a member-cursor step, both
   only when g_ShopBuySell != 0 and right after ShopListSet has refilled
   the item list). The wanted line-up records g_ShopWantedEntries are written as
   ShopMemberDisp writes them -- the member under the cursor a = 1, b = 1,
   everyone else a = 0, b = 2, c = 5 for all -- then the shuffle
   ShopLineUpMove, the member-cursor sprite (record 3 of g_ShopObjects) and the
   ShopFaceSet call, statement for statement ShopMemberDisp's and
   ShopItemLineUpSet's. No clamp of the cursor and no panel redraw here.

   Shape: the cursor sprite's x and y are MapRecord member stores, for the
   reason ShopItemLineUpSet's are (written as casts through g_ShopObjects + K
   two rows differ, 64/66). The counter is a u16 (s32: 54/66) and
   c = 5 is written in each arm (once after the if: 56/66). */
/* func_800523BC */
void ShopMemberLineUpSet(void)
{
    u16 i;

    for (i = 0; i < g_ShopPlaceCount; i++) {
        if (i != g_ShopMemberCursor) {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].row = 0;
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = 2;
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing = 5;
        } else {
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].row = 1;
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose = 1;
            ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing = 5;
        }
    }
    ShopLineUpMove();
    ((MapRecord *)g_ShopObjects)[3].on_kind.on = 1;
    ((MapRecord *)g_ShopObjects)[3].sprite.x = g_ShopOrigin[0] + (g_ShopMemberCursor * 4 + 0xD) * 8;
    ((MapRecord *)g_ShopObjects)[3].sprite.y = g_ShopOrigin[1] + 0x14;
    g_MessageFaceFirst = ShopFaceSet((u8)((u16 *)g_PartyIds)[g_ShopMemberCursor], 1, g_MessageFaceFirst, 0);
}

static inline unsigned char inline_fn_func_800524C4(s32 count)
{
  return (unsigned char) count;
}
/* func_800524C4 */
void ShopRefusalDisp(int first_code, s32 second_code)
{
  s32 val;
  s32 count;
  u8 *p;
  count = 0;
  if (((first_code & 0xFFFF) != 0) || ((second_code & 0xFFFF) != 0))
  {
    BtlCellAreaClearCopy(0xC, 0x14, 0xCC00, 0x18, 6, 0x20);
  }
  if ((first_code & 0xFFFF) != 0)
  {
    switch (first_code & 0xFFFF)
    {
      case 0xEDC8:
        val = 9;
        break;

      case 0xEDAC:

      case 0xEE10:
        val = 7;
        break;

      default:
        break;

    }

    CellTextDisp(0xC, 0x14, val & 0xFF, 1, val & 0xFF, 0xDEAD2001, first_code & 0xFFFF);
    count++;
  }
  if ((second_code & 0xFFFF) != 0)
  {
    val = ((count & 0xFF) << 1) + 0x14;
    CellTextDisp(0xC, val, 7, 1, 7, 0xDEAD2001, second_code & 0xFFFF);
    count++;
  }
  if (((unsigned int) (count & 0xFF)) != 0)
  {
    BtlCellRowsUpdateCopy(0x14, 0x500, 6, 0x20);
  }
  g_BtlMenuSoundRequest = 4;
  *((u16 *) g_ShopShapeEntries) = 0;
  SoundPlay(4, 3);
  count = 0;
  p = g_BtlMenuPadRecord;
  g_BtlMenuSoundRequest = 0;
  ((PadRecord *)p)->pressed = 0;
  while (inline_fn_func_800524C4(count) < ((unsigned int) 0x80))
  {
    pad_record_update((s32 *) g_BtlMenuPadRecord, 0x10);
    ShopFrameStep();
    count++;
    if ((*(&((PadRecord *)g_BtlMenuPadRecord)->pressed)) != 0)
    {
      break;
    }
  }

  g_BtlMenuSoundRequest = 3;
  SoundPlay(3, 3);
  g_BtlMenuSoundRequest = 0;
}

/* far_syms: g_BtlMenuPadRecord */
extern u8 g_ShopCursors[3];
extern u8 g_ShopMemberItemCount[2];
/* Scalar view of g_ShopCursors[0] (the list's scroll row). The original read
   it as its own scalar: a plain variable's access is its DECL_RTL, while an
   array element's address goes through memory_address -> force_reg, and
   cse.c's use_related_value then rewrites it as `cursor address - 1` (an
   `la g_ShopCursors+1` held live; the target loads it %gp_rel directly). */
void ShopDealOpen(void);  /* TU defines this below */

/* The shop's step 3, the choice of an item (entity_ai_table[2], run by
   ShopStepsRun's loop while g_ShopStep == 3). pad+4 = repeat buttons,
   pad+2 = new presses (PSY-Q PadRead bits). L1/R1 cycle the member,
   the d-pad walks a 6-column grid of cells, Triangle backs out, Cross
   picks. */
/* func_8005268C */
void ShopItemStep(void)
{
    u8 *pad;
    u16 btn;
    s32 moved;
    u16 msg[2];
    u8 cur;
    s32 col;
    u16 row;
    u8 flags;

    pad = g_BtlMenuPadRecord;
    btn = ((PadRecord *)pad)->repeated;
    if (btn & 0xC) {
        g_BtlMenuSoundRequest = 1;
        if (btn & 8) {
            if (g_ShopMemberCursor >= g_ShopPlaceCount - 1) {
                g_ShopMemberCursor = 0;
            } else {
                g_ShopMemberCursor++;
            }
        } else {
            if (g_ShopMemberCursor == 0) {
                g_ShopMemberCursor = g_ShopPlaceCount - 1;
            } else {
                g_ShopMemberCursor--;
            }
        }
        if (g_ShopBuySell != 0) {
            ShopListSet(g_ShopBuySell);
            ShopMemberLineUpSet();
        }
        ShopItemDisp();
        return;
    }
    if (btn & 0xF000) {
        g_BtlMenuSoundRequest = 1;
        moved = 0;
        if (btn & 0x8000) {
            if ((u8)(g_ShopCursors[1] % 6) != 0) {
                g_ShopCursors[1] = g_ShopCursors[1] - 1;
            } else {
                g_ShopCursors[1] = (u8)(g_ShopCursors[1] / 6) * 6 + 5;
            }
        } else if (btn & 0x2000) {
            if ((u8)(g_ShopCursors[1] % 6) < 5) {
                g_ShopCursors[1] = g_ShopCursors[1] + 1;
            } else {
                g_ShopCursors[1] = (u8)(g_ShopCursors[1] / 6) * 6;
            }
            moved = 1;
        } else if (btn & 0x1000) {
            if (g_ShopBuySell != 0) {
                if (((u16 *)g_PartyIds)[g_ShopMemberCursor] == 0xC) {
                    cur = g_ShopCursors[1];
                    row = (u8)(cur / 6);
                    col = (u8)(cur % 6);
                    if (row != 0) {
                        row--;
                    } else if (g_ShopCursors_s != 0) {
                        g_ShopCursors_s--;
                    }
                    g_ShopCursors[1] = col + row * 6;
                } else if (g_ShopCursors[1] >= 6) {
                    g_ShopCursors[1] -= 6;
                }
            } else {
                if (g_ShopCursors[1] >= 6) {
                    g_ShopCursors[1] -= 6;
                } else if ((&g_ShopMemberCursor)[3] != 0) {
                    (&g_ShopMemberCursor)[3]--;
                }
            }
        } else {
            if (g_ShopBuySell != 0 && ((u16 *)g_PartyIds)[g_ShopMemberCursor] == 0xC) {
                cur = g_ShopCursors[1];
                row = (u8)(cur / 6);
                col = (u8)(cur % 6);
                if (row != 0) {
                    if (g_ShopCursors_s < 14 && ((u8 *)g_ChrItemList0C)[g_ShopCursors_s * 6 + 0xC] != 0) {
                        g_ShopCursors_s++;
                    }
                } else {
                    row++;
                }
                g_ShopCursors[1] = col + row * 6;
            } else if (g_ShopCursors[1] < 6) {
                g_ShopCursors[1] += 6;
            }
        }
        if (((PadRecord *)g_BtlMenuPadRecord)->repeated & 0x5000) {
            ShopListSet(g_ShopBuySell);
        }
        ShopListCursorAdjust(moved);
        ShopItemDisp();
        return;
    }
    btn = ((PadRecord *)pad)->pressed;
    if (btn & 0x10) {
        g_BtlMenuSoundRequest = 3;
        g_ShopStep = 2;
        ShopAnimationSet(g_ShopObjects + 0xB4, 8, 1);
        ((u8 *)g_ShopObjects)[0xF0] = 0;
        ShopItemStringsClear(0xED00, 0x14);
        BtlCellAreaClearCopy(3, 9, 0xCC00, 8, 4, 0x20);
        BtlCellAreaClearCopy(0xC, 0x14, 0xCC00, 0x18, 6, 0x20);
        BtlCellRowsUpdateCopy(9, 0x240, 0x11, 0x20);
        ShopMemberDisp();
        return;
    }
    if (btn & 0x40) {
        msg[1] = 0;
        msg[0] = 0;
        if (g_ShopBuySell != 0) {
            if (*(u16 *)(g_ShopCursors[1] * 12 + g_ShopListEntries) == 0) {
                ShopRefusalDisp(0, 0);
                return;
            }
            if (((u8 *)g_ShopItemRow)[4] == 6 || ((BtlItemRow *)g_ShopItemRow)->sell_price == 0) {
                ShopRefusalDisp(0xEDAC, 0);
                ShopItemDisp();
                return;
            }
        } else {
            flags = g_ShopMemberItemCount[1];
            if (flags & 0x20) {
                msg[0] = 0xEDC8;
            }
            if ((flags & 0x11) == 0x10) {
                if ((flags & 0x42) == 0x42) {
                    msg[1] = 0xEE10;
                }
            } else if (flags & 0x40) {
                msg[1] = 0xEE10;
            }
            if (msg[0] != 0 || msg[1] != 0) {
                ShopRefusalDisp(msg[0], msg[1]);
                ShopItemLineUpSet();
                ShopItemTextDisp();
                return;
            }
        }
        g_BtlMenuSoundRequest = 2;
        g_ShopStep = 4;
        ShopDealOpen();
    }
}

/* far_syms: g_ChrItemRows (target reaches it %hi/%lo) */
extern u16 g_ShopOrigin[2];
extern u8 g_ShopCursors[3];
extern s32 g_ChrItemRows;
extern u8 g_ShopItemFlags;  /* the shop's purchase flags: a scalar of its own (= g_ShopMemberItemCount + 1) */

/* ShopItemDisp -- the shop list's selection refresh (callers: ShopMemberStep,
   ShopItemStep after a cursor move, ShopDealClose on the way back from the
   Buy? / Sell? prompt).

   1. ShopListCursorAdjust(0) snaps the list cursor g_ShopCursors[1] (row * 6 + col)
      onto a filled cell.
   2. The cursor sprite, record 4 of g_ShopObjects, is switched on and put on
      the cell: x = g_ShopOrigin[0] + (col * 4 + 0xC) * 8, y = g_ShopOrigin[1] +
      (row * 4 + 0xB) * 8 -- the cell positions ShopOpen gave the list.
   3. g_ShopItemRow = the cell's item record: 0x12-byte rows of the table
      *g_ChrItemRows, 1-based on the u16 item id at cell + 6 (+0 u16 price, +2
      u16 sale price, +4 u8 kind, +0x10 u16 who-can-equip mask, bit id - 1).
   4. BUY mode only (g_ShopBuySell == 0): the purchase flags g_ShopItemFlags
      are worked out for the member under the cursor (id =
      g_PartyIds[g_ShopMemberCursor], rec = its 0x40 record in g_ChrRecords), the
      "Price" label (strip 0xEE2C) is drawn and BtlCellRowsUpdateCopy(9, 0x240, 2,
      0x20) / ShopItemLineUpSet() run:
        0x20  the price is more than the silver *g_GameMoney (unsigned
              compare; never set when g_ShopStockRow + 1 == 0x22)
        0x10  the item is equipment (kind < 5). Then:
        0x01  this member cannot equip it (bit id - 1 of the who mask is
              clear); 0x02 is cleared with it
        0x02  it can, and the slot is taken: kind 4 needs BOTH +0x22 and
              +0x23 full, any other kind looks at rec[0x1E + kind]
        0x40  no inventory room (ChrItemRoomCheck(id) == 0)
      ShopItemTextDisp and ShopItemStep read them: 0x20 dims the price, 0x01
      shows "Can't equip this!", 0x40 "Inventory full!".
   5. ShopItemTextDisp() redraws the cell's panel, in either mode.
   No frame wait and no RNG draw of its own.

   Shape:
   - g_ShopItemFlags is a SCALAR. Written as the array element g_ShopMemberItemCount[1] the
     byte store to it is "in a struct" for GCC, the struct-typed mask load
     ((BtlItemRow *)g_ShopItemRow)->who may then alias it, and the scheduler
     cannot lift that load above `|= 0x10` (167/168: the lhu lands two rows
     late). As a scalar at a fixed address the pair is exempt
     (true_dependence in sched.c) and the natural statement order matches.
     So these two shop bytes were separate variables in the original, like
     the list's scroll row (g_ShopCursors_s).
   - `can` is a variable: testing the `&` directly lets fold rewrite it as
     (who >> (id - 1)) & 1 (ShopDealOpen has the same).
   - `it` points one record PAST the item: the target keeps table + id *
     0x12 in $a1 and reaches the record at -0x12 / -0xE; g_ShopItemRow gets
     it - 1. Reading the two fields back through g_ShopItemRow, or recomputing
     the address, keeps a second register (161/168).
   - The three 0x20 arms and the 0x02 arms are written out one by one:
     the target has a store per arm, merged afterwards by cross-jumping
     (the byte is reloaded after each merge). Only the cannot-equip arm
     sets and clears in one expression (a single store).
   - cell is read once into a u8; col and row go through a u8 (the andi
     0xFF on each). */
/* func_80052C0C */
void ShopItemDisp(void)
{
    u16 id;
    u8 *rec;
    u16 kind;
    u8 cell;
    s32 can;
    BtlItemRow *it;

    ShopListCursorAdjust(0);
    ((MapRecord *)g_ShopObjects)[4].on_kind.on = 1;
    cell = g_ShopCursors[1];
    ((MapRecord *)g_ShopObjects)[4].sprite.x = g_ShopOrigin[0] + ((u8)(cell % 6) * 4 + 0xC) * 8;
    ((MapRecord *)g_ShopObjects)[4].sprite.y = g_ShopOrigin[1] + ((u8)(cell / 6) * 4 + 0xB) * 8;
    it = (BtlItemRow *)g_ChrItemRows + *(u16 *)(g_ShopListEntries + cell * 12 + 6);
    g_ShopItemRow = (s32)(it - 1);
    if (g_ShopBuySell == 0) {
        kind = it[-1].kind;
        id = ((u16 *)g_PartyIds)[g_ShopMemberCursor];
        rec = (u8 *)g_ChrRecords + (id << 6);
        if (g_ShopStockRow + 1 == 0x22) {
            g_ShopItemFlags &= 0xDF;
        } else if ((u32)*g_GameMoney >= it[-1].price) {
            g_ShopItemFlags &= 0xDF;
        } else {
            g_ShopItemFlags |= 0x20;
        }
        if (((BtlItemRow *)g_ShopItemRow)->kind < 5) {
            g_ShopItemFlags |= 0x10;
            can = ((BtlItemRow *)g_ShopItemRow)->wearers & (1 << (((u16 *)g_PartyIds)[g_ShopMemberCursor] - 1));
            if (can) {
                g_ShopItemFlags &= 0xFE;
                if (((BtlItemRow *)g_ShopItemRow)->kind == 4) {
                    if (rec[0x22] != 0 && rec[0x23] != 0) {
                        g_ShopItemFlags |= 2;
                    } else {
                        g_ShopItemFlags &= 0xFD;
                    }
                } else {
                    if (rec[kind + 0x1E] != 0) {
                        g_ShopItemFlags |= 2;
                    } else {
                        g_ShopItemFlags &= 0xFD;
                    }
                }
            } else {
                g_ShopItemFlags = (g_ShopItemFlags | 1) & 0xFD;
            }
        } else {
            g_ShopItemFlags &= 0xEF;
        }
        if (ChrItemRoomCheck(id) == 0) {
            g_ShopItemFlags |= 0x40;
        } else {
            g_ShopItemFlags &= 0xBF;
        }
        CellTextDisp(3, 9, 3, 1, 3, 0xDEAD2001, 0xEE2C);
        BtlCellRowsUpdateCopy(9, 0x240, 2, 0x20);
        ShopItemLineUpSet();
    }
    ShopItemTextDisp();
}

/* func_80052EAC */
void ShopDealOpen(void)
{
  s32 wearable;
  g_ShopCursors[2] = 0;
  g_ShopDealWearable = 0;
  BtlCellAreaClearCopy(0xC, 0x14, 0xCC00, 0x18, 6, 0x20);
  if ((*((u8 *) (((u8 *) (&g_ShopMemberCursor)) + 1))) != 0)
  {
    CellTextDisp(0xC, 0x14, 3, 1, 3, 0xDEAD2001, 0xED50);
    CellTextDisp(0xC, 0x16, 2, 1, 2, 0xDEAD2001, 0xEDEC);
    CellTextDisp(0x13, 0x16, 3, 1, 3, 0xDEAD2001, 0xED74);
  }
  else
  {
    CellTextDisp(0xC, 0x14, 3, 1, 3, 0xDEAD2001, 0xED5C);
    if (((BtlItemRow *)g_ShopItemRow)->kind < 5)
    {
      wearable = ((BtlItemRow *)g_ShopItemRow)->wearers & (1 << ((*((u16 *) (((u8 *) g_PartyIds) + (g_ShopMemberCursor * 2)))) - 1));
      if (wearable)
      {
        g_ShopDealWearable = 1;
      }
    }
    if (g_ShopDealWearable != 0)
    {
      CellTextDisp(0xC, 0x16, 3, 1, 3, 0xDEAD2001, 0xED68);
    }
    CellTextDisp((g_ShopDealWearable * 7) + 0xC, 0x16, 2, 1, 2, 0xDEAD2001, 0xEE38);
    CellTextDisp((g_ShopDealWearable * 7) + 0x13, 0x16, 3, 1, 3, 0xDEAD2001, 0xED74);
  }
  BtlCellRowsUpdateCopy(0x14, 0x500, 6, 0x20);
  ShopAnimationSet(g_ShopObjects + 0xF0, 0xB, 1);
  *((s16 *) g_ShopShapeEntries) = 1;
  ShopDealDisp();
}

/* func_800530E8 */
void ShopDealDisp(void)
{
  ((SpriteEntry *)g_ShopShapeEntries)->x = (s16) (g_ShopOrigin[0] + ((((*(((u8 *) (&g_ShopCursors)) + 0x2)) * 7) + 0xC) * 8));
  ((SpritePiece *)g_ShopShapes)->x2 = 0x30;
  ((SpritePiece *)g_ShopShapes)->x1 = 0x30;
}

s32 ShopAnimationSet(s32, s32, s32);
void BtlCellRowsUpdateCopy();  /* TU defines this below */
s32 BtlCellAreaClearCopy(s32, s32, s32, s32, s32, s32);

/* func_80053120 */
void ShopDealClose(void)
{
  int record;
  record = 0xF0 + g_ShopObjects;
  *(s16 *)g_ShopShapeEntries = 0;
  ShopAnimationSet(record, 0xA, 1);
  BtlCellAreaClearCopy(0xC, 0x14, 0xCC00, 0x18, 6, 0x20);
  BtlCellRowsUpdateCopy(0x14, 0x500, 6, 0x20);
  ShopItemDisp();
}

extern u8 g_ShopCursors[3];

/* The shop's confirm step: ShopDealStep calls it when Cross
   is pressed on the "Buy?" / "Sell?" prompt ShopDealOpen opened. The
   labels are the pre-rendered strings 0x603.. (ShopOpen; code =
   0xED50 + 4 * row): 0xEE10 is "Inventory full!".

   id   = the member under the cursor (((u16 *)g_PartyIds)[g_ShopMemberCursor]),
          rec = its 0x40 record in g_ChrRecords (+0x1E..+0x23 = equipment
          slots 1..6; ChrWornItemRemove(id, slot) unequips and returns the
          item, ChrWornItemSet(id, slot, item) equips).
   kind = the shop item's slot kind (byte +4 of the g_ShopItemRow record;
          +0 u16 the price, +2 u16 the sale price).
   sel  = the prompt cursor g_ShopCursors[2], plus 1 when there is no "Equip"
          option (g_ShopDealWearable == 0): 0 Equip, 1 Buy / Sell, 2 Cancel.

   0 Equip (buy and wear). kind 4 has two slots, 5 and 6 (+0x22 / +0x23):
       both full -> needs inventory room (ChrItemRoomCheck, else fail), the
       slot-5 item goes to the inventory (ChrItemGive), slot 6's item
       moves to +0x22, and the new item is equipped in slot 6; only +0x23
       full -> it moves to +0x22, new item in slot 6; +0x22 full only ->
       slot 6; both empty -> slot 5. Other kinds: a full slot (rec[0x1E +
       kind]) needs inventory room (else fail); the old item is unequipped
       into the inventory -- that pair of calls runs even when the slot is
       empty -- and the new one equipped in slot kind + 1. Then pay.
   1 with g_ShopBuySell != 0 (SELL): list cells 0..5 are the member's
       worn items -- unless the member is id 0xC, cell g_ShopCursors[1] < 6
       is unequipped (ChrWornItemRemove(id, cell + 1)); otherwise the item
       T[cell].b6 (12-byte g_ShopListEntries rows) leaves the inventory
       (ChrItemRemove). Silver (*g_GameMoney) += the sale price, capped at
       9,999,999 (0x98967F); ShopListSet redraws the list. No payment.
   1 otherwise (BUY): needs inventory room (else fail), ChrItemGive
       adds T[cell].b6, then pay.
   2 Cancel: g_BtlMenuSoundRequest = 3 -- which the common tail below overwrites
       with 2, so Cancel sounds like a confirm (original behaviour).
   pay:  silver -= the price, unless g_ShopStockRow + 1 == 0x22.

   fail -> message 0xEE10 "Inventory full!" (ShopRefusalDisp, ShopItemLineUpSet)
   and the prompt again (ShopDealOpen). Otherwise g_BtlMenuSoundRequest = 2, the
   silver readout is redrawn (ShopMoneyDisp), g_ShopStep = 3 and
   ShopDealClose returns to the list.

   Shape: id, kind and sel are u16 locals (HImode): the `andi 0xFFFF` at
   each call site is the default promotion of id, kind's survives only
   where its zero-extension has two uses (== 4 and the +0x1E index) --
   `kind + 1`, used once per cse temp, loses it to combine because the
   only set of kind is an lbu. The callees are called undeclared. The pay
   block and `fail = 1; break;` are written out at each site; cross-jumping
   keeps the last copy of each. */
/* func_80053194 */
void ShopDealRun(void)
{
    u16 id;
    u8 *rec;
    u16 kind;
    u16 sel;
    s32 fail;

    id = ((u16 *)g_PartyIds)[g_ShopMemberCursor];
    rec = (u8 *)g_ChrRecords + (id << 6);
    kind = ((BtlItemRow *)g_ShopItemRow)->kind;
    fail = 0;
    if (g_ShopDealWearable != 0) {
        sel = g_ShopCursors[2];
    } else {
        sel = g_ShopCursors[2] + 1;
    }
    switch (sel) {
    case 0:
        if (kind == 4) {
            if (rec[0x22] != 0) {
                if (rec[0x23] != 0) {
                    if (ChrItemRoomCheck(id) == 0) {
                        fail = 1;
                        break;
                    }
                    ChrItemGive(id, ChrWornItemRemove(id, 5) & 0xFF);
                    rec[0x22] = rec[0x23];
                }
                ChrWornItemSet(id, 6, *(u8 *)(g_ShopCursors[1] * 12 + g_ShopListEntries + 6));
            } else if (rec[0x23] != 0) {
                rec[0x22] = rec[0x23];
                ChrWornItemSet(id, 6, *(u8 *)(g_ShopCursors[1] * 12 + g_ShopListEntries + 6));
            } else {
                ChrWornItemSet(id, 5, *(u8 *)(g_ShopCursors[1] * 12 + g_ShopListEntries + 6));
            }
        } else {
            if (rec[kind + 0x1E] != 0 && ChrItemRoomCheck(id) == 0) {
                fail = 1;
                break;
            }
            ChrItemGive(id, ChrWornItemRemove(id, kind + 1) & 0xFF);
            ChrWornItemSet(id, kind + 1, *(u8 *)(g_ShopCursors[1] * 12 + g_ShopListEntries + 6));
        }
        if (g_ShopStockRow + 1 != 0x22) {
            *g_GameMoney -= *(u16 *)g_ShopItemRow;
        }
        break;
    case 1:
        if (g_ShopBuySell != 0) {
            if (id != 0xC && g_ShopCursors[1] < 6) {
                ChrWornItemRemove(id, g_ShopCursors[1] + 1);
            } else {
                ChrItemRemove(id, *(u8 *)(g_ShopCursors[1] * 12 + g_ShopListEntries + 6));
            }
            if ((u32)(0x98967F - *g_GameMoney) < ((BtlItemRow *)g_ShopItemRow)->sell_price) {
                *g_GameMoney = 0x98967F;
            } else {
                *g_GameMoney += ((BtlItemRow *)g_ShopItemRow)->sell_price;
            }
            ShopListSet(g_ShopBuySell);
        } else {
            if (ChrItemRoomCheck(id) == 0) {
                fail = 1;
                break;
            }
            ChrItemGive(id, *(u8 *)(g_ShopCursors[1] * 12 + g_ShopListEntries + 6));
            if (g_ShopStockRow + 1 != 0x22) {
                *g_GameMoney -= *(u16 *)g_ShopItemRow;
            }
        }
        break;
    case 2:
        g_BtlMenuSoundRequest = 3;
        break;
    }
    if (fail != 0) {
        ShopRefusalDisp(0xEE10, 0);
        ShopItemLineUpSet();
        ShopDealOpen();
    } else {
        g_BtlMenuSoundRequest = 2;
        ShopMoneyDisp(*g_GameMoney, 3, 0xF, 0x3000);
        g_ShopStep = 3;
        ShopDealClose();
    }
}

/* func_80053534 */
void ShopDealStep(void)
{
  u8 *pad_record;
  u16 repeat_buttons;
  u16 pressed_buttons;

  pad_record = (u8 *) (&g_BtlMenuPadRecord);
  repeat_buttons = ((PadRecord *)pad_record)->repeated;
  if (repeat_buttons & 0xA000)
  {
    g_BtlMenuSoundRequest = 1;
    if (repeat_buttons & 0x2000)
    {
      if (g_ShopDealWearable < g_ShopCursors[2])
      {
        g_ShopCursors[2] = 0;
      }
      else
      {
        g_ShopCursors[2] = g_ShopCursors[2] + 1;
      }
    }
    else
    {
      if (g_ShopCursors[2] != 0)
      {
        g_ShopCursors[2] = g_ShopCursors[2] - 1;
      }
      else
      {
        g_ShopCursors[2] = g_ShopDealWearable + 1;
      }
    }
    ShopDealDisp();
    return;
  }
  pressed_buttons = ((PadRecord *)pad_record)->pressed;
  if (pressed_buttons & 0x10)
  {
    g_BtlMenuSoundRequest = 3;
    g_ShopStep = 3;
    ShopDealClose();
    return;
  }
  if (pressed_buttons & 0x40)
  {
    ShopDealRun();
    if (*((u16 *) ((s32) (g_ShopListEntries + g_ShopCursors[1] * 12) + 6)) == 0xB5)
    {
      ShopListSet(0);
      ShopItemTextDisp();
    }
  }
}

/* func_80053648 */
s32 ShopAnimationSet(s32 record, s32 animation, s32 unused) {
    s32 old_animation_then_length;
    u16 first_step;

    old_animation_then_length = ((MapRecord *)record)->animation;
    ((MapRecord *)record)->animation = animation;
    ((MapRecord *)record)->animation_before = old_animation_then_length;
    first_step = *(u16 *)((u8 *)((((MapRecord *)record)->animation * 4) + ((MapRecord *)record)->animation_table + 0x2));
    ((MapRecord *)record)->step = first_step;
    ((MapRecord *)record)->end = (s16) (first_step + (*(s16 *)((((MapRecord *)record)->animation * 4) + ((MapRecord *)record)->animation_table)));
    ((MapRecord *)record)->timer = 0;
    old_animation_then_length = *(u16 *)((u8 *)((((MapRecord *)record)->step * 4) + ((MapRecord *)record)->step_list + 0x2));
    ((MapRecord *)record)->step_length = old_animation_then_length;
    return old_animation_then_length;
}

extern s32 g_SpriteList;
void OutlineMapStep(void *records, s32 record_number, s32 mode);
void ShapeDraw(s16 x, s16 y, void *shape);
void palette_animation_step(void);
void tile_animation_step(void);
s32 sprite_list_draw(void);
void MapFrameEndCopy(void);

/* The SHOP's one frame: every wait of the
   shop screen is a call of this (ShopOpen the opening, ShopClose
   the exit - 16 walk-back frames and the fade - ShopStepsRun the state
   loop, ShopRefusalDisp, and ShopLineUpMove's four line-up frames). In
   order:

   1. ShopAnimationStep(g_MapPartyRecords, 0, g_ShopPlaceCount): the lined-up actors (the
      first g_ShopPlaceCount records of g_MapPartyRecords) are ticked one animation
      step and drawn into sprite slots 0.. (below).
   2. OutlineMapStep(g_ShopObjects, i, 1) for i = 0..6: the per-slot animated
      marker of outline.c over each of the seven shop objects (mode 1: marker
      slot i + 7).
   3. ShopAnimationStep(g_ShopObjects, 0x72, 7): the seven shop objects (cursors,
      markers) are ticked and drawn into sprite slots 0x72..0x78.
   4. sprite_run_off(0x6E, 4) frees sprite slots 0x6E..0x71, then each of
      the four windows g_ShopShapeEntries[i] whose first halfword is nonzero is
      copied whole (0x24 bytes) into sprite slot 0x6E + i and its pane is
      drawn: ShapeDraw(window x, window y, &g_ShopShapes[i]) (the
      0x18-byte pane records).
   5. The frame itself, as the field frame runs it: palette_animation_step() (the
      eight palette animations), tile_animation_step() (the four g_TileAnimations
      animated-cell records), sprite_list_draw() (the sprite pass over
      g_SpriteList). Then, if g_ShopTextBlockCount != 0 - the count of text cells the
      shop has rendered at 0x801CBB00 since the last frame (ShopOpen
      bumps it and sets the request's size word) - the upload request
      D_800E4768 is armed (word 0 = 1). Last MapFrameEndCopy() =
      MapFrameEnd(), the frame's tail: layer_col_fade_step, the two tile
      layers (bg_draw), palette_update, tile_animation_load, the frame
      PRESENT frame_end (the VSync wait and, through pad_frame_step,
      the vibration driver), frame_start, sound_music_level_step, the frame
      counter g_PlayTimeFrames++ and MapFrameCount.
   6. g_ShopTextBlockCount = 0.

   Frames: exactly one, in MapFrameEndCopy -> MapFrameEnd ->
   frame_end. (sprite_list_draw presents a frame of its own only on its
   pad-gated path to debug_sprite_page_show: g_DebugAllowed set, pad bits 5 held and
   g_PadPressedButtons & 0x800.) No RNG draw of its own; the one route to
   rng_draw is that frame present's vibration driver
   (pad_update), which draws only while a script with the random
   motion runs (frame-rng.md) - none is started here.

   Nothing is returned and no caller reads $v0: `void`, where the tree's
   declarations said `s32 ShopFrameStep()`; as `s32` with no return
   statement it compiles to the same 92 instructions.

   Shape: a u16 counter in both loops (the `andi 0xFFFF` at each use;
   the windows and panes are indexed by it, which is why the target
   multiplies by 0x24 and 0x18 on every pass) and the sprite slot a walked
   pointer. The window's first halfword is unsigned here (lhu). */
/* func_800536AC */
void ShopFrameStep(void)
{
    SpriteEntry *spr;
    u16 i;

    ShopAnimationStep(g_MapPartyRecords, 0, g_ShopPlaceCount);
    for (i = 0; i < 7; i++) {
        OutlineMapStep((void *)g_ShopObjects, i, 1);
    }
    ShopAnimationStep(g_ShopObjects, 0x72, 7);
    sprite_run_off(0x6E, 4);
    spr = (SpriteEntry *)g_SpriteList + 0x6E;
    for (i = 0; i < 4; i++, spr++) {
        if (((SpriteEntry *)g_ShopShapeEntries)[i].on != 0) {
            *spr = ((SpriteEntry *)g_ShopShapeEntries)[i];
            ShapeDraw(((SpriteEntry *)g_ShopShapeEntries)[i].x, ((SpriteEntry *)g_ShopShapeEntries)[i].y, &((SpritePiece *)g_ShopShapes)[i]);
        }
    }
    palette_animation_step();
    tile_animation_step();
    sprite_list_draw();
    if (g_ShopTextBlockCount != 0) {
        *(s32 *)D_800E4768 = 1;
    }
    MapFrameEndCopy();
    g_ShopTextBlockCount = 0;
}

void MapAnimationStart(void *record, s32 restart);

/* The generic animation tick and draw for a
   run of 0x3C-byte actor records: MapPartyAnimationStep's (party field actors),
   MoveChrAction's (script actors) and MenuRecordsAnimationStep's (menu actors)
   counterpart for a caller-given table.

       ShopAnimationStep(rec, slot, n)

   ticks the n records from `rec` on and draws them into the n sprite
   slots from `slot` on. Callers: the shop frame ShopFrameStep (the lined-up
   actors g_MapPartyRecords into slots 0.., count g_ShopPlaceCount; the seven shop
   objects g_ShopObjects into slots 0x72..0x78), the shop opening
   ShopOpen (one actor), and MenuUseShowPlacesPlay (btl_show.c: the six
   g_MenuUseShowRecords field-effect records into slots 0x6E..0x73).

   1. sprite_run_off(slot, n) frees the n sprite slots.
   2. For each record with a nonzero first byte (a dead record is skipped
      and its slot stays free; there is no "held" test here, unlike
      MapPartyAnimationStep):
      - the frame timer (+0x34) advances; when it reaches the frame's
        duration (+0x36) the frame index (+0x38) steps. Then, by bit 15
        of the END word (+0x3A): set, an index that has reached
        end & 0x7FFF goes back by one (the last frame is held); clear, an
        index that has reached the end restarts the animation
        (MapAnimationStart(rec, 1)). In EVERY one of those cases - a plain
        step too - the timer is then zeroed and the duration reloaded
        from the frame table (+0x2C, the u16 at frame * 4 + 2); after
        MapAnimationStart that repeats what it has just stored.
        The hold flag is bit 15 of the end word here, where the three
        sibling ticks test bit 15 of the animation entry (+0x28). It gets
        there through ShopAnimationSet, the animation setter of the shop
        objects and of btl_show.c's records, which stores first frame + the
        entry's raw count word; MapAnimationStart stores first frame +
        (count & 0x7FFF), so a record it set never holds in this tick.
      - the record's 0x24-byte sprite image (+4) is copied to its sprite
        slot and the slot's tile (+0x18) is the current frame's (the u16
        at frame * 4 of the frame table).
   No frame wait and no RNG draw (sprite_run_off and MapAnimationStart reach
   neither).

   Shape: the record PARAMETER itself is walked, with every field read
   straight off it and no local pointer. loop.c then makes the animation
   fields' addresses givs of that biv and reduces them to one register
   based on the LAST one it meets, the frame-table pointer at +0x2C -- the
   target's second walking pointer ($s0 = rec + 0x2C, stepped just before
   the record's own). Spellings that do not match: the record indexed
   rec[i] (74-92/99: its address giv takes everything onto one register,
   or computes rec + 0x2C inside the loop -- `(rec + i * 0x3C) + 0x2C` is
   not a giv for loop.c, a register-plus-constant add_val is refused); a
   local pointer copied from the parameter (97/99: the prologue's save
   order moves); a walked pointer to the animation block (72/99: the
   reduced register comes out at +0x38). The sprite slot is a walked
   pointer too.

   The parameter keeps the tree's type: shop_screen.c and btl_show.c declare the
   function (s32, s32, s32) and every caller passes an integer-typed
   global, so the record is the s32 `record`, read through the `rec` view
   and stepped by sizeof(MapRecord). Written as `(MapRecord *rec, s32 slot,
   u32 n)` with `rec++` it compiles to the same 99 instructions -- that is
   the natural signature, and switching to it needs only the two forward
   declarations retyped and the (s32) cast at ShopOpen's call
   dropped. */
/* func_8005381C */
void ShopAnimationStep(s32 record, s32 slot, s32 n)
{
#define rec ((MapRecord *)record)
    SpriteEntry *spr;
    u32 i;

    sprite_run_off(slot, n);
    spr = (SpriteEntry *)g_SpriteList + slot;
    for (i = 0; i < n; i++, record += sizeof(MapRecord), spr++) {
        if (rec->on_kind.on != 0) {
            rec->timer++;
            if (rec->timer >= rec->step_length) {
                rec->step++;
                if (rec->end & 0x8000) {
                    if (rec->step >= (rec->end & 0x7FFF)) {
                        rec->step--;
                    }
                } else if (rec->step >= rec->end) {
                    MapAnimationStart(rec, 1);
                }
                rec->timer = 0;
                rec->step_length = AS(SpriteStep *, rec->step_list)[rec->step].length;
            }
            *spr = rec->sprite;
            spr->pose = AS(SpriteStep *, rec->step_list)[rec->step].pose;
        }
    }
#undef rec
}

/* Ruling of 2026-10-14 (docs/naming.md): Does what ShopTextStringReport does; nothing read says why there are two. */
/* func_800539A8 */
void ShopTextStringReportCopy() {

}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; named from what its callers pass: a string and its place in the shop's text. */
/* func_800539B0 */
void ShopTextStringReport() {

}

/* Two parameters, and the FIRST is unused: 0x800539BC-C4 loads the
   constant 0x801CBB00 over $a0 and masks $a1 (`andi $a1, $a1, 0xFFFF`)
   for font_target_clear's second argument. Both callsites set $a0 and $a1
   (ShopItemStep, ShopItemTextDisp). */
/* func_800539B8 */
void ShopItemStringsClear(s32 unused, s32 block_count)
{
  font_target_clear(0x801CBB00, block_count & 0xFFFF);
  CellTextDisp(0xC, 6, 8, 1, 8, 0xDEAD2001, 0xED00);
  CellTextDisp(0xC, 8, 0xC, 1, 0xC, 0xDEAD2001, 0xED20);
}

/* empty (jr $ra / nop). An empty leaf emits the same two instructions
   whatever its parameter list, so the bytes do not give one, and a
   `(void)` would REJECT every real caller: ShopMoneyDisp passes four
   arguments. Unprototyped so any arity compiles. */
/* Ruling of 2026-10-14 (docs/naming.md): Does what BtlCellRowsUpdate does; nothing read says why there are two. */
/* func_80053A3C */
void BtlCellRowsUpdateCopy() {

}

s32 BtlCellAreaClearCopy(s32, s32, s32, s32, s32, s32);          /* extern */
s32 CellGlyphSet(s32, s32, s32, s32, s32, s32);          /* extern */

/* func_80053A44 */
void ShopMoneyDisp(s32 amount, s32 column, s32 row, u16 col) {
    BtlCellAreaClearCopy(column, row, 0xCC00, 7, 2, 0x20);
    CellNumberSet(amount, column, row, 7, 0xDEAD2001, col & 0xFFFF, 1, 0);
    CellGlyphSet(0xF, column + 7, row, 0xDEAD2001, 0x2000, 1);
    BtlCellRowsUpdateCopy(row, row << 6, 2, 0x20);
}

extern u8 g_ShopMemberItemCount[2];  /* gp-indexed in target: sized small array (addressing row 2) */

/* func_80053B18 */
void ShopItemTextDisp(void)
{
  s32 col;
  u16 amount;
  BtlCellAreaClearCopy(0xC, 0x14, 0xCC00, 0x18, 6, 0x20);
  BtlCellAreaClearCopy(3, 9, 0xCC00, 8, 4, 0x20);
  col = 0;
  if ((*((u16 *) (((u8 *) g_ShopListEntries) + (g_ShopCursors[1] * 0xC)))) == 0)
  {
    ShopItemStringsClear(0xED00, 0x14);
    CellTextDisp(0xC, 0x14, 7, 1, 7, 0xDEAD2001, 0xEDF4);
    BtlCellRowsUpdateCopy(9, 0x240, 0x11, 0x20);
    return;
  }
  if ((*((u8 *) (((u8 *) (&g_ShopMemberCursor)) + 1))) != 0)
  {
    if ((((BtlItemRow *)g_ShopItemRow)->kind == 6) || (((BtlItemRow *)g_ShopItemRow)->sell_price == 0))
    {
      col = 4;
    }
    else
    {
      CellTextDisp(3, 9, 3, 1, 3, 0xDEAD2001, 0xEE2C);
      ShopMoneyDisp(((BtlItemRow *)g_ShopItemRow)->sell_price, 3, 0xB, 0x3000);
      if (g_ShopCursors[1] < 6)
      {
        ShopItemStatsDisp();
      }
    }
    BtlCellRowsUpdateCopy(9, 0x240, 4, 0x20);
  }
  else
  {
    if (g_ShopMemberItemCount[1] & 0x20)
    {
      col = 4;
    }
    if (((BtlItemRow *)g_ShopItemRow)->kind < 5)
    {
      if (g_ShopMemberItemCount[1] & 1)
      {
        CellTextDisp(0xC, 0x14, 8, 1, 8, 0xDEAD2001, 0xED8C);
      }
      else
      {
        ShopItemStatsDisp();
      }
    }
    else
      if (g_ShopMemberItemCount[1] & 0x40)
    {
      CellTextDisp(0xC, 0x14, 7, 1, 7, 0xDEAD2001, 0xEE10);
    }
    CellTextDisp(3, 9, 3, 1, 3, 0xDEAD2001, 0xEE2C);
    if ((((u8) g_ShopStockRow) + 1) == 0x22)
    {
      amount = 0;
      ShopMoneyDisp(amount, 3, 0xB, 0x3000);
    }
    else
    {
      amount = *((u16 *) g_ShopItemRow);
      ShopMoneyDisp(amount, 3, 0xB, 0x3000);
    }
  }
  BtlCellRowsUpdateCopy(0x14, 0x500, 6, 0x20);
  ShopItemStringsDisp(*((u16 *) ((((u8 *) g_ShopListEntries) + (g_ShopCursors[1] * 0xC)) + 6)), col);
}

extern u8 g_ShopCursors[3];
extern u16 g_ShopKindTextCodes[];
extern u16 g_ShopKindTextWidths[];

/* The shop's stat-comparison panel: what the item
   under the list cursor would change for the member under the cursor.
   ShopItemTextDisp (the shop's item-info redraw) calls it for a worn item in
   sell mode (list cell < 6) and for an equippable item (kind < 5) in buy
   mode. Nothing is drawn for member id 0xC.

   id   = the member (((u16 *)g_PartyIds)[g_ShopMemberCursor]), rec = its 0x40
          record in g_ChrRecords (+0x1E.. = the equipment slots).
   item = byte +6 of the cursor's 12-byte list cell (g_ShopListEntries +
          g_ShopCursors[1] * 12); kind = byte +4 of the g_ShopItemRow item record.
   p    = the equipment slot the item would go to: rec + 0x1E + kind, and
          for kind 4 (two slots, +0x22 / +0x23) +0x23 when it is empty,
          else +0x22.

   ChrWornItemChangeMake(id, buf, a, b) fills three u16 triples {label, now, then}:
     sell (g_ShopBuySell != 0):  (id, buf, 0, item)  -- taking it off
     buy:                           (id, buf, item, *p) -- the item against
                                    what the slot holds now
   Then for each triple i = 0..2 whose label is non-zero, on row
   y = i * 2 + 0x14:
     - the label strip: CellTextDisp(0xC, y, w, 1, w, 0xDEAD2001, code)
       with w = g_ShopKindTextWidths[label - 1], code = g_ShopKindTextCodes[label - 1] (eight
       entries: codes 0xEE40 0xEE50 0xEE60 0xEE70 0xEE80 0xEE98 0xEEA8
       0xEEB4, widths 4 4 4 4 6 4 3 3 -- consecutive strips of 4 codes a
       cell; by the widths and ShopOpen's stub calls they are strings
       0x619.. Attack / Defense / Agility / Wisdom / Magic Endr / #Attack /
       Luck / Range, which is an inference, not read from a renderer);
     - CellGlyphSet(0xC, 0x16, y, 0xDEAD2001, 0x3000, 1);
     - the one-cell strip 0xEEC0 at column 0x1E (the code right after the
       eight labels);
     - `now` as 3 digits at column 0x1A in colour 0x3000 and `then` at
       column 0x21 (CellNumberSet(v, x, y, 3, 0xDEAD2001, colour, 1, 0)),
       `then` in 0x2000 when it is higher than `now`, 0 when lower, 0x3000
       when equal.
   No frame wait and no RNG draw, here or in its call tree.

   Shape: id is a u16 and item a u8 local, ChrWornItemChangeMake is called
   undeclared -- the sell call's arguments come out as plain register
   copies and the buy call's keep their `andi` masks from that alone. buf
   is a flat u16[9] indexed i * 3 + n, and the row is the expression
   i * 2 + 0x14 at every call. `p = rec + 0x23; if (rec[0x23] != 0) p = rec
   + 0x22;` is the form that puts 0x23 in the branch's delay slot; an
   if / else or a ?: of the same choice puts 0x22 there (166/169). */
/* func_80053E0C */
void ShopItemStatsDisp(void)
{
    u16 buf[9];
    u16 id;
    u8 *rec;
    u8 *p;
    u8 item;
    u8 kind;
    u16 i;
    s32 col;

    id = ((u16 *)g_PartyIds)[g_ShopMemberCursor];
    if (id != 0xC) {
        rec = (u8 *)g_ChrRecords + id * 0x40;
        item = *(u8 *)(g_ShopCursors[1] * 12 + g_ShopListEntries + 6);
        kind = ((BtlItemRow *)g_ShopItemRow)->kind;
        if (kind == 4) {
            p = rec + 0x23;
            if (rec[0x23] != 0) {
                p = rec + 0x22;
            }
        } else {
            p = &rec[kind + 0x1E];
        }
        if (g_ShopBuySell != 0) {
            ChrWornItemChangeMake(id, buf, 0, item);
        } else {
            ChrWornItemChangeMake(id, buf, item, *p);
        }
        for (i = 0; i < 3; i++) {
            if (buf[i * 3] != 0) {
                if (buf[i * 3 + 1] < buf[i * 3 + 2]) {
                    col = 0x2000;
                } else if (buf[i * 3 + 2] < buf[i * 3 + 1]) {
                    col = 0;
                } else {
                    col = 0x3000;
                }
                CellTextDisp(0xC, i * 2 + 0x14, g_ShopKindTextWidths[buf[i * 3] - 1], 1, g_ShopKindTextWidths[buf[i * 3] - 1], 0xDEAD2001, g_ShopKindTextCodes[buf[i * 3] - 1]);
                CellGlyphSet(0xC, 0x16, i * 2 + 0x14, 0xDEAD2001, 0x3000, 1);
                CellTextDisp(0x1E, i * 2 + 0x14, 1, 1, 1, 0xDEAD2001, 0xEEC0);
                CellNumberSet(buf[i * 3 + 1], 0x1A, i * 2 + 0x14, 3, 0xDEAD2001, 0x3000, 1, 0);
                CellNumberSet(buf[i * 3 + 2], 0x21, i * 2 + 0x14, 3, 0xDEAD2001, col, 1, 0);
            }
        }
    }
}

/* func_800540B0 */
void ShopListDisp(u16 *entry, s32 entry_count)
{
  u8 masked_entry_count;
  u8 i;
  i = 0;
  masked_entry_count = (u8) entry_count;
  while (i < masked_entry_count)
  {
    if (entry[0])
    {
      IconCellSet(entry[1], entry[2], entry[3], 0);
    }
    else
    {
      BtlCellAreaClearCopy(entry[1], entry[2], 0x200900, 3, 3, 0x10);
    }
    i++;
    entry += 6;
  }

  BtlCellRowsUpdateCopy(0xB, 0x2C0, 7, 0x10);
}

/* Ruling of 2026-10-14 (docs/naming.md): Does what BtlCellAreaClear does; nothing read says why there are two. */
/* func_8005417C */
s32 BtlCellAreaClearCopy(s32 column, s32 row, s32 unused, s32 width, s32 a4, s32 a5) {
    return ankprint_area_clear(column, row, width, a4, a5);
}

extern u16 g_ShopOrigin[2];

/* The shop's line-up shuffle: every lined-up actor steps to
   the row its WANTED record asks for and takes the wanted pose.

   The g_ShopPlaceCount lined-up actors are rows 0.. of g_MapPartyRecords (0x3C records).
   Each has two 4-byte records {a, b, c, d}: the wanted one in g_ShopWantedEntries and
   the current one in g_ShopCurrentEntries (ShopOpen starts both at {0, 1, 5}).
   The callers write the wanted records and call this: ShopBuySellOpen
   (a = 0, b = 1 for everyone), ShopMemberDisp, ShopItemLineUpSet and
   ShopMemberLineUpSet.

   1. Per actor i: if wanted.a differs from current.a it is put on its
      walk first (MapAnimationPoseSet(actor, 1, wanted.c)); then
      line_path_make builds a 4-point path in g_LinePaths row i (16 points of
      {x, y} a row) from the actor's (x, y) to (x, g_ShopOrigin[1] +
      (wanted.a + 4) * 8) -- x unchanged, so `a` is a row offset in 8-pixel
      cells below the line-up's base row. The path is built for every
      actor, moved or not.
   2. Four frames (ShopFrameStep each): frame j puts every actor on point
      j of its path.
   3. Per actor: MapAnimationPoseSet(actor, wanted.b, wanted.c), and the
      wanted record becomes the current one (a whole 4-byte copy).

   Frames: exactly four, through ShopFrameStep. No RNG draw of its own.

   Shape: both counters are s16 (the sll/sra pairs); every address is
   spelled out at its use and left to cse -- (i << 2) is the one value the
   target keeps across the first loop's calls ($s2) and across the last
   loop's call ($s0). The 4 of `wanted.a + 4` and the point count share a
   register with no shaping. */
/* func_800541AC */
void ShopLineUpMove(void)
{
    s16 i;
    s16 j;

    for (i = 0; i < g_ShopPlaceCount; i++) {
        if (((ShopLineUpEntry *)g_ShopWantedEntries)[i].row != ((ShopLineUpEntry *)g_ShopCurrentEntries)[i].row) {
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[i], 1, ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing);
        }
        line_path_make((s16 *)((PathPoint *)g_LinePaths + i * 16), ((MapRecord *)g_MapPartyRecords)[i].sprite.x,
                      ((MapRecord *)g_MapPartyRecords)[i].sprite.y, ((MapRecord *)g_MapPartyRecords)[i].sprite.x,
                      g_ShopOrigin[1] + (((ShopLineUpEntry *)g_ShopWantedEntries)[i].row + 4) * 8, 4);
    }
    for (j = 0; j < 4; j++) {
        for (i = 0; i < g_ShopPlaceCount; i++) {
            ((MapRecord *)g_MapPartyRecords)[i].sprite.x = AS(s16, ((PathPoint *)g_LinePaths)[j + i * 16].x);
            ((MapRecord *)g_MapPartyRecords)[i].sprite.y = AS(s16, ((PathPoint *)g_LinePaths)[j + i * 16].y);
        }
        ShopFrameStep();
    }
    for (i = 0; i < g_ShopPlaceCount; i++) {
        MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[i], ((ShopLineUpEntry *)g_ShopWantedEntries)[i].pose,
                               ((ShopLineUpEntry *)g_ShopWantedEntries)[i].facing);
        ((ShopLineUpEntry *)g_ShopCurrentEntries)[i] = ((ShopLineUpEntry *)g_ShopWantedEntries)[i];
    }
}

s32 MessageFaceEntrySet(s32, u8, s32, u16, s32);                /* extern */

/* func_800543CC */
void ShopFaceClear(s32 keep_face) {
    if (keep_face == 0) {
        MessageFaceEntrySet(0, (*(s16 *)((g_ShopMemberCursor * 2) + g_PartyIds)), 1, g_MessageFaceFirst, 0);
    }
}

extern u8 g_ShopCursors[3];

/* Field item/magic menu: snap the list cursor g_ShopCursors[1] (row * 6 + col on the
   2 x 6 grid of 12-byte cells at g_ShopListEntries) onto a filled cell — one whose first
   word, the `on` flag ShopListSet writes, is non-zero. Called after every d-pad
   step (ShopItemStep: stepped_right = 1 only for a step right) and with 0 by ShopItemDisp
   (cursor sprite placement) and ShopMemberStep. Search order, first hit wins:
     1. the cursor's own cell;
     2. along its row in the step direction (stepped_right != 0: right to column 5,
        otherwise left to column 0);
     3. the same row from the far end (right: columns 0..5, left: columns 5..0);
     4. the other row, from the cursor's column leftwards to column 0;
     5. the other row, from the cursor's column rightwards to column 5;
     6. nothing filled: cell 0.
   No RNG draw, no frame wait, no calls. */
/* func_80054410 */
void ShopListCursorAdjust(s32 stepped_right)
{
    u16 row;
    u16 col;
    u16 lim;

    row = g_ShopCursors[1] / 6;
    col = g_ShopCursors[1] % 6;
    if (stepped_right != 0) {
        if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
            goto done;
        }
        if (col < 5) {
            do {
                col++;
            } while (col < 5 && *(u16 *)(g_ShopListEntries + (col + row * 6) * 12) == 0);
        }
        if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
            goto done;
        }
        col = 0;
        /* CODEGEN FORCING DEVICE, not recovered source.
           `lim` is always 5, so the test is `col < 5` — behaviour is unchanged
           (cse folds the constant back in: the loop compares with `sltiu 5`).
           The assignment is there for its one extra RTL insn: it takes this
           loop's exit test from 20 insns to 22, over the 20-insn limit of
           jump.c's duplicate_loop_exit_test in the FIRST jump pass, so the test
           is duplicated by the post-loop jump pass instead — after loop.c has
           hoisted the row offset and the list pointer. That is the target's
           shape (hoisted copies, then the entry test for column 0, and the
           loop bottom cross-jumped into the entry branch). The original
           spelling of this test is unknown; a plain `col < 5` scores 270/283. */
        while ((lim = 5, col < lim) && *(u16 *)(g_ShopListEntries + (col + row * 6) * 12) == 0) {
            col++;
        }
    } else {
        if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
            goto done;
        }
        if (col != 0) {
            do {
                col--;
            } while (col != 0 && *(u16 *)(g_ShopListEntries + (col + row * 6) * 12) == 0);
        }
        if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
            goto done;
        }
        col = 5;
        if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
            goto done;
        }
        do {
            col--;
        } while (col != 0 && *(u16 *)(g_ShopListEntries + (col + row * 6) * 12) == 0);
    }
    if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
        goto done;
    }
    col = g_ShopCursors[1] % 6;
    row ^= 1;
    if (col != 0) {
        if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
            goto done;
        }
        do {
            col--;
        } while (col != 0 && *(u16 *)(g_ShopListEntries + (col + row * 6) * 12) == 0);
    }
    if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
        goto done;
    }
    col = g_ShopCursors[1] % 6;
    if (col < 5) {
        if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
            goto done;
        }
        do {
            col++;
        } while (col < 5 && *(u16 *)(g_ShopListEntries + (col + row * 6) * 12) == 0);
    }
    if (*(u16 *)(g_ShopListEntries + (col + row * 6) * 12) != 0) {
        goto done;
    }
    col = 0;
    row = 0;
done:
    g_ShopCursors[1] = col + row * 6;
}

s32 xmalloc_heap_alloc(s32);                               /* extern */

/* func_8005487C */
void ShopAlloc(void) {
    s32 records;
    s32 entries;

    g_ShopListEntries = xmalloc_heap_alloc(0x90);
    g_ShopKeptIds = xmalloc_heap_alloc(6);
    records = xmalloc_heap_alloc(0x30C);
    g_ShopKeptRecords = records;
    g_ShopObjects = records + 0x168;
    g_LinePaths = xmalloc_heap_alloc(0x180);
    entries = xmalloc_heap_alloc(0x30);
    g_ShopWantedEntries = entries;
    g_ShopCurrentEntries = entries + 0x18;
    g_ShopShapeEntries = xmalloc_heap_alloc(0x90);
    g_ShopShapes = xmalloc_heap_alloc(0x60);
}

/* func_800548F4 */
void ShopFree(void) {
    xmalloc_heap_free(g_ShopListEntries);
    xmalloc_heap_free(g_ShopKeptIds);
    xmalloc_heap_free(g_ShopKeptRecords);
    xmalloc_heap_free(g_LinePaths);
    xmalloc_heap_free(g_ShopWantedEntries);
}

extern u16 g_SpriteData0[];

/* Set up ONE shop object: a 0x3C-byte animated
   actor record drawing from the menu's sprite bank g_SpriteData0, left
   switched off and standing on the first frame of an animation.

       ShopObjectInit(rec, 0, D_800B8B2C, D_800B8B28, clut, anim)

   The one caller is the shop opening ShopOpen, seven calls for the
   seven records of g_ShopObjects (clut, anim): (0x7F0, 0), (0x7F0, 1),
   (0x760, 12), (0x760, 8), (0x760, 10), (0x760, 4), (0x760, 5). The
   second, third and fourth arguments are never read here (the caller
   passes 0 and the two words D_800B8B2C / D_800B8B28 every time).

   What it stores, in the written order:
     - +0x00 (the record's on byte) = 0: the object is OFF. ShopAnimationStep,
       which ticks and draws these seven into sprite slots 0x72..0x78,
       skips a record whose first byte is 0; whoever shows an object sets
       the byte.
     - the 0x24-byte sprite image at +4: its on (+4) = 1, +8 = 0x1000,
       +0xC = 0, +0xE = (clut & 0x3FF) | 0x6000 (0x63F0 or 0x6360).
     - five addresses into the bank, whose head (SpriteDataHead) holds
       places in 4-byte units from the bank's own start: +0x10 = bank + 4 *
       texel_records, +0x14 = bank + 4 * pose_table, +0x18 = bank + 4 *
       piece_table (the image's three tables), +0x28 = bank + 4 *
       animation_table, +0x2C = bank + 4 * step_list.
     - +0x24 = 0 (word), +0x20 = 0, +0x1C = 0 (the image's tile).
     - +0x30 (anim id) = anim, and +0x32 (the id before it) = 1 when anim
       is 0, else 0: an id that is never the current one. Both setters
       overwrite it with the outgoing id (ShopAnimationSet always,
       MapAnimationSet on a change) and neither tests it first; any other
       reader of +0x32 was not looked for.
     - MapAnimationStart(rec, 1) starts the animation: +0x38 = the anim
       entry's first frame, +0x3A = first frame + (count & 0x7FFF), +0x34
       = 0, +0x36 = that frame's duration.
   Not written here: +1..+3, x (+6), y (+0xA), +0x1E, +0x22.
   ShopOpen stores x and y right after the call for records 0, 1, 5
   and 6 only.
   No return value, no frame wait and no RNG draw (MapAnimationStart is a
   leaf).

   Matching.
   - The head's five words are pointer accesses through their members'
     addresses (`*(u16 *)&bank->texel_records`): as member loads the
     function grows by 0x10 bytes; MessageRecordsInit and MenuRecordsInit
     read them the same way. Word 0 is read through the pointer local like the other
     four, as MessageRecordsInit has it: a plain load at a fixed
     address, which the member stores do not pin, so it is loaded second
     (`lhu $v1, %lo(g_SpriteData0)($v1)`, off the %hi register) and its sum
     stored late. As the array's own element (`*g_SpriteData0`): 28/48.
     Without the pointer local at all (g_SpriteData0[n] * 4 +
     (s32)g_SpriteData0): 28/48.
   - The statement order of the five sums is +0x10, +0x14, +0x18, +0x28,
     +0x2C, the order MenuUseShowPlacesPlay, MenuRecordsInit and MessageRecordsInit fill
     the same fields in; +0x28 before +0x18 is 45/48.
   - The three zero stores are written +0x24, +0x20, +0x1C (MenuUseShowPlacesPlay's
     order; ascending is 46/48) and come BEFORE the anim id (after it:
     47/48).
   - The "id before" is tested on the member just stored (`rec->anim ==
     0`), which is the target's `andi 0xFFFF` on the stored register;
     `(u16)animation == 0` is the same bytes, `animation == 0` is 47/48.
   - Neutral (all 48/48): on and +4 either way round, the record through a
     pointer local or through casts of record, a pointer sum for the
     addresses, an int return type.
   - The record gets a struct of its own because MapRecord's SpriteEntry types +0xC
     and +0x20 as words and this function stores halfwords at +0xC, +0xE
     and +0x20. The first parameter keeps the tree's type (the TU's
     prototype is all s32 and the caller passes the s32 g_ShopObjects plus an
     offset). */
/* func_80054948 */
void ShopObjectInit(s32 record, s32 unused, s32 unused2, s32 unused3, s32 palette, s32 animation)
{
    MapRecord *rec;
    SpriteDataHead *bank;

    rec = (MapRecord *)record;
    bank = (SpriteDataHead *)g_SpriteData0;
    rec->on_kind.on = 0;
    rec->sprite.on = 1;
    AS(u16, rec->sprite.depth) = 0x1000;
    rec->sprite.attr = 0;
    rec->sprite.flags = (palette & 0x3FF) | 0x6000;
    rec->sprite.texel_records = *(u16 *)&bank->texel_records * 4 + (s32)bank;
    rec->sprite.pose_table = *(u16 *)&bank->pose_table * 4 + (s32)bank;
    rec->sprite.piece_table = *(u16 *)&bank->piece_table * 4 + (s32)bank;
    rec->animation_table = *(u16 *)&bank->animation_table * 4 + (s32)bank;
    rec->step_list = *(u16 *)&bank->step_list * 4 + (s32)bank;
    AS(s32, rec->sprite.scale_x) = 0;
    rec->sprite.scaled = 0;
    rec->sprite.pose = 0;
    rec->animation = animation;
    rec->animation_before = rec->animation == 0;
    MapAnimationStart(rec, 1);
}
