/* Lunar SSSC — btl_command: the menus that pick each member's command,
   steps 3 to 14 (preset, kind, spell, item, target, walk), and what they
   share (icon cells, clear, pulse, frame, outline, the windows' wait, the
   target mask, the text, the cursor, the action check). The first split had
   it in entity.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "path.h"
#include "btl.h"
extern u8 g_BtlSlotPlannedNumbers[];
s32 BtlMenuSpellDisp(s32 *);
extern u16 g_BtlMenuTargetMask;
extern u8 g_BtlActingSlot;
extern s32 g_BtlRecords;
extern u16 g_BtlMenuCursor;
extern s32 g_BtlMenuItems;
s32 BtlMenuIconCellsSet(s32, s32);
s32 BtlMenuTextStringPut(s32, s32, s32, s32);
s32 BtlMenuTextImageLoad(s32, s32, s32);
extern u16 g_BtlMenuPadRecord[];
extern u16 g_MenuPadRecord[];
extern u8 g_BtlMenuSoundRequest;
extern s32 g_BtlMenuMarkRecords;
extern s32 g_BtlMenuCopyRecords;
/* g_BtlMenuItems: the battle command grid, 12-byte cells (BtlMenuKindOpen). */
extern s32 g_BtlMenuPulse;
extern s8 g_BtlMenuStep;
extern u8 g_BtlMenuItemCount;
extern u8 g_BtlActionNumber;
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlSlotIds[];
extern s32 CharStats;
void BtlMenuPresetClose(void);
extern u8 g_BtlMenuTargetMode;
extern u8 g_BtlSlotAmountKinds[];
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlMenuMarkKind;
s32 layer_mix_switch_on(s32);
extern u8 g_BtlPartyStandAnimations[];
extern u16 g_MenuPadRecordRepeated[];
extern u8 g_BtlMenuSpellKeptCursors[];
extern u8 g_BtlMenuItemKeptCursors[];
extern u8 g_BtlMenuPendingMask;
extern BtlMenuPresetCell *g_BtlMenuPresetCells;
extern u8 *g_BtlMenuPresetSlots;
extern u8 g_BtlPartyPreset0Kinds[];
extern u8 g_BtlPartyPreset1Kinds[];
extern u8 g_BtlPartyPreset2Kinds[];
extern u8 g_BtlPartyPreset0Numbers[];
extern u8 g_BtlPartyPreset1Numbers[];
extern u8 g_BtlPartyPreset2Numbers[];
extern s32 g_LinePaths;
extern u8 g_BtlMenuKeptCursors[2];
s32 xmalloc_heap_alloc(s32);
s32 BtlMenuActionCheck(s32, u8, u8, s32);
s32 BtlMenuTextCellsSet(s32, s32, s32, s32, s32, s32);
s32 line_path_make(u16 *, s16, s16, s16, s32, s32);
void BtlCellRowsUpdate();  /* TU defines this below */
s32 BtlWindow0Set(s32, s32, s32, s32);
void BtlMenuFrameStep(void);  /* TU defines this below */
void BtlMenuPresetRowDisp();
void BtlWindowsMoveWait(void);  /* TU defines this below */

/* func_80044FA4 */
void BtlMenuPresetOpen(void)
{
    u8 *tbl;
    u8 *tbl2;
    u8 i;
    u8 j;
    u8 n;
    BtlMenuPresetCell *e;

    g_BtlMenuStep = 4;
    g_BtlMenuPresetCells = (BtlMenuPresetCell *)xmalloc_heap_alloc(0x78);
    g_BtlMenuPresetSlots = (u8 *)xmalloc_heap_alloc(5);
    n = 0;
    for (i = 0; i < 5; i++)
    {
        if ((g_BtlMenuPendingMask >> i) & 1)
        {
            g_BtlMenuPresetSlots[n] = i;
            for (j = 0; j < 3; j++)
            {
                switch (j)
                {
                case 0:
                    tbl = g_BtlPartyPreset0Kinds;
                    tbl2 = g_BtlPartyPreset0Numbers;
                    break;
                case 1:
                    tbl = g_BtlPartyPreset1Kinds;
                    tbl2 = g_BtlPartyPreset1Numbers;
                    break;
                case 2:
                    tbl = g_BtlPartyPreset2Kinds;
                    tbl2 = g_BtlPartyPreset2Numbers;
                    break;
                }
                switch (tbl[i])
                {
                case 0:
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].icon) = 0xF0;
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 0;
                    g_BtlMenuPresetCells[j * 5 + n].kind = 0;
                    g_BtlMenuPresetCells[j * 5 + n].number = 0;
                    break;
                case 1:
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].icon) = 0xED;
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 0;
                    g_BtlMenuPresetCells[j * 5 + n].kind = 1;
                    g_BtlMenuPresetCells[j * 5 + n].number = 0;
                    break;
                case 2:
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].icon) = 0xE9;
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 0;
                    g_BtlMenuPresetCells[j * 5 + n].kind = 2;
                    g_BtlMenuPresetCells[j * 5 + n].number = 0xFF;
                    break;
                case 5:
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].icon) = 0xEB;
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 0;
                    g_BtlMenuPresetCells[j * 5 + n].kind = 5;
                    g_BtlMenuPresetCells[j * 5 + n].number = 0xFF;
                    break;
                case 6:
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].icon) = 0xEE;
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 0;
                    g_BtlMenuPresetCells[j * 5 + n].kind = 6;
                    g_BtlMenuPresetCells[j * 5 + n].number = 0xFF;
                    break;
                case 3:
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].icon) = tbl2[i];
                    if (BtlMenuActionCheck(CharStats + i * 0x3C, tbl[i], tbl2[i], 0) != 0)
                    {
                        AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 0;
                        g_BtlMenuPresetCells[j * 5 + n].kind = 3;
                        g_BtlMenuPresetCells[j * 5 + n].number = tbl2[i] + 0x4A;
                    }
                    else
                    {
                        AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 4;
                        g_BtlMenuPresetCells[j * 5 + n].kind = 5;
                        g_BtlMenuPresetCells[j * 5 + n].number = 0xFF;
                    }
                    break;
                case 4:
                    AS(s16, g_BtlMenuPresetCells[j * 5 + n].icon) = tbl2[i];
                    if (BtlMenuActionCheck(CharStats + i * 0x3C, tbl[i], tbl2[i], 0) == 0)
                    {
                        AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 4;
                        g_BtlMenuPresetCells[j * 5 + n].kind = 5;
                        g_BtlMenuPresetCells[j * 5 + n].number = 0xFF;
                    }
                    else
                    {
                        AS(s16, g_BtlMenuPresetCells[j * 5 + n].colour) = 0;
                        g_BtlMenuPresetCells[j * 5 + n].kind = 4;
                        g_BtlMenuPresetCells[j * 5 + n].number = tbl2[i];
                    }
                    break;
                }
            }
            e = (BtlMenuPresetCell *)(n * 6 + (s32)g_BtlMenuPresetCells);
            AS(s16, e[15].icon) = 0xF0;
            AS(s16, e[15].colour) = 0;
            e[15].kind = 0;
            g_BtlMenuPresetCells[n + 15].number = 0;
            n++;
        }
    }
    g_BtlMenuItemCount = n + 4;
    g_BtlMenuCursor = g_BtlMenuKeptCursors[1];
    BtlMenuTextCellsSet(2, 5, 5, 1, 0x80300000, 0);
    n = g_BtlMenuItemCount - 4;
    for (j = 0; j < n; j++)
    {
        BtlMenuTextCellsSet(0x14, j * 3 + 3, 8, 1, 0x80300000, (j << 3) | 5);
        ((BtlRecord *)g_BtlMenuCopyRecords)[j] = ((BtlRecord *)g_BtlRecords)[g_BtlMenuPresetSlots[j]];
        ((BtlRecord *)g_BtlMenuCopyRecords)[j].sprite.flags &= 0x9FFF;
        ((BtlRecord *)g_BtlMenuCopyRecords)[j].sprite.attr |= 0x2000;
        AS(s16, ((BtlRecord *)g_BtlRecords)[g_BtlMenuPresetSlots[j]].on) = 0;
        line_path_make((u16 *)(g_LinePaths + (j << 6)),
                      ((BtlRecord *)g_BtlRecords)[g_BtlMenuPresetSlots[j]].sprite.x,
                      ((BtlRecord *)g_BtlRecords)[g_BtlMenuPresetSlots[j]].sprite.y,
                      0x68, (j * 3 + 5) * 8, 0x10);
        *(u8 *)((u8 *)g_BtlMenuItems + j * 12 + 0x30) = 1;
        *(u8 *)((u8 *)g_BtlMenuItems + j * 12 + 0x31) = 0;
        *(s32 *)((u8 *)g_BtlMenuItems + j * 12 + 0x38) = 0x20200000;
        *(s16 *)((u8 *)g_BtlMenuItems + j * 12 + 0x32) = 0x10;
        *(s16 *)((u8 *)g_BtlMenuItems + j * 12 + 0x34) = j * 3 + 2;
    }
    BtlCellRowsUpdate(3, 0xC0, 0xE, 0x20);
    BtlWindow0Set(2, 1, 0x24, 0x11);
    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < n; j++)
        {
            ((BtlRecord *)g_BtlMenuCopyRecords)[j].sprite.x = AS(s16, ((PathPoint *)g_LinePaths)[j * 16 + i].x);
            ((BtlRecord *)g_BtlMenuCopyRecords)[j].sprite.y = AS(s16, ((PathPoint *)g_LinePaths)[j * 16 + i].y);
            ((BtlRecord *)g_BtlMenuCopyRecords)[j].sprite.depth = AS(s16, ((PathPoint *)g_LinePaths)[j * 16 + i].y) | 0x1000;
        }
        BtlMenuFrameStep();
    }
    for (j = 0; j < 4; j++)
    {
        ((BtlMenuItem *)g_BtlMenuItems)[j].on = 1;
        ((BtlMenuItem *)g_BtlMenuItems)[j].disabled = 0;
        ((BtlMenuItem *)g_BtlMenuItems)[j].x = (j % 2) * 5 + 3;
        ((BtlMenuItem *)g_BtlMenuItems)[j].y = (j / 2) * 5 + 2;
        ((BtlMenuItem *)g_BtlMenuItems)[j].attr = 0x200000;
    }
    AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[0].icon) = 0xF3;
    AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[1].icon) = 0xF4;
    AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[2].icon) = 0xF5;
    AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[3].icon) = 0xF0;
    BtlMenuPresetRowDisp();
    ((s16 *)g_BtlMenuMarkRecords)[0x40] = 1;
    BtlWindowsMoveWait();
}

INCLUDE_RODATA("asm/data/jtbl", jtbl_800123FC_pad);
/* far_syms: g_BtlMenuItemCount */
s32 xmalloc_heap_free(s32);
void BtlMenuClear();

/* func_8004571C */
void BtlMenuPresetClose(void)
{
    s16 i;
    s16 j;
    s16 n;
    xmalloc_heap_free((s32)g_BtlMenuPresetCells);
    i = 15;
    xmalloc_heap_free((s32)g_BtlMenuPresetSlots);
    BtlMenuClear();
    n = g_BtlMenuItemCount - 4;
    do {
        for (j = 0; j < n; j++) {
            BtlRecord *dst = (BtlRecord *)(j * 64 + g_BtlMenuCopyRecords);
            PathPoint *src = (PathPoint *)((j * 16 + i) * 4 + g_LinePaths);
            AS(u16, dst->sprite.x) = src->x;
            AS(u16, dst->sprite.y) = src->y;
            AS(u16, dst->sprite.depth) = src->y | 0x1000;
        }
        BtlMenuFrameStep();
    } while (--i >= 0);
    for (j = 0; j < 5; j++) {
        AS(s16, ((BtlRecord *)g_BtlMenuCopyRecords)[j].on) = 0;
        AS(s16, ((BtlRecord *)g_BtlRecords)[j].on) = 1;
    }
}
/* load_hazard_nop: BtlMenuPresetStep */
/* far_syms: g_BtlActingSlot */
extern u8 g_BtlMenuKeptCursors[2];
s32 BtlMenuPresetRowActionSet(s32);
s32 BtlMenuCursorMove(u8 *pad, s32 menu_items, s32 column_count, s32 row_count, s32 cur);  /* TU defines this below */

/* func_8004584C */
void BtlMenuPresetStep(void)
{
    if (g_MenuPadRecordRepeated[0] == 0x40)
    {
        g_BtlMenuSoundRequest = 2;
        BtlMenuPresetRowActionSet((s32)&g_BtlMenuPresetCells[g_BtlMenuCursor * 5]);
        BtlMenuPresetClose();
        if (g_BtlMenuPendingMask & 0x1F)
        {
            g_BtlActingSlot = 0;
            g_BtlMenuStep = 5;
        }
        else
        {
            g_BtlMenuStep = 0;
        }
    }
    else if (g_MenuPadRecordRepeated[0] == 0x10)
    {
        g_BtlMenuSoundRequest = 3;
        g_BtlMenuStep = 1;
        BtlMenuPresetClose();
    }
    else if (g_BtlMenuPadRecord[2] & 0xF000)
    {
        s32 c = g_BtlMenuCursor;
        g_BtlMenuSoundRequest = 1;
        g_BtlMenuCursor = BtlMenuCursorMove((u8 *)g_BtlMenuPadRecord, g_BtlMenuItems, 2, 2, c) & 0xFF;
        if (g_BtlMenuKeptCursors[1] != g_BtlMenuCursor)
        {
            g_BtlMenuKeptCursors[1] = g_BtlMenuCursor;
            BtlMenuPresetRowDisp();
        }
    }
}

/* far_syms: g_BtlMenuItems (target reaches it %hi/%lo) */

/* func_80045960 */
void BtlMenuKindDisp(void)
{
  s32 col;
  u16 icon;
  void *menu_item;
  { s32 idx = g_BtlMenuCursor * 0xC; menu_item = (u8 *)(idx + g_BtlMenuItems); }
  icon = AS(s16, ((BtlMenuItem *)menu_item)->icon);
  col = (((BtlMenuItem *)menu_item)->disabled != 0) * 4;
  BtlMenuTextStringPut(5, (icon + 0x100) & 0xFFFF, col, 5);
  BtlMenuTextImageLoad(5, 5, 5);
  BtlMenuIconCellsSet(1, 8);
}
/* far_syms: g_BtlMenuItems */
/* load_hazard_nop: BtlMenuKindOpen */
/* far_syms: g_BtlActionKind */
/* far_syms: g_BtlActionNumber */
/* far_syms: g_BtlTargetSlot */
/* far_syms: CharStats */
/* far_syms: g_BtlSlotIds */
/* far_syms: g_BtlSlotPlannedKinds */
/* far_syms: g_BtlSlotPlannedNumbers */
extern u8 g_BtlMenuPendingMask;
extern u8 g_BtlActionKind;
extern u8 g_BtlMenuKindKeptCursors[];
s32 BtlSlotActionAnimationSet(u8);
void BtlMenuPulseSet(void *pulse, s32 mode, s32 layer_mask, s32 top);  /* TU defines this below */
s32 BtlMenuTextCellsSet(s32, s32, s32, s32, s32, s32);
void BtlCellRowsUpdate();  /* TU defines this below */
s32 BtlWindow0Set(s32, s32, s32, s32);
s32 BtlMenuActingFaceSet(s32, s32, s32, s32, s32);
void func_80048C94();  /* TU defines this below */
s32 StoryBitCheck(u32);
void BtlWindowsMoveWait(void);  /* TU defines this below */

/* func_800459D4 */
void BtlMenuKindOpen(void)
{
  u16 i;
  BtlMenuItem *cell;
  BtlMenuItem *cells;
  u8 m;
  u8 a;
  if (g_BtlActingSlot >= 5)
  {
    g_BtlMenuStep = 0;
    return;
  }
  if (!((g_BtlMenuPendingMask >> g_BtlActingSlot) & 1))
  {
    g_BtlActingSlot += 1;
    return;
  }
  g_BtlMenuStep = 6;
  g_BtlSlotPlannedKinds[g_BtlActingSlot] = g_BtlActionKind = 0xB;
  a = g_BtlActingSlot;
  g_BtlSlotPlannedTargets[a] = g_BtlTargetSlot = 0;
  g_BtlSlotPlannedNumbers[a] = g_BtlActionNumber = 0;
  BtlSlotActionAnimationSet(g_BtlActingSlot);
  BtlMenuPulseSet((void *) g_BtlMenuPulse, 2, 0x400, 6);
  BtlMenuTextCellsSet(3, 5, 5, 1, 0x80300000, 0);
  BtlMenuTextCellsSet(0x1B, 5, 5, 1, 0x80300000, 5);
  BtlCellRowsUpdate(5, 0x140, 2, 0x20);
  BtlWindow0Set(2, 1, 0x24, 0xA);
  BtlMenuActingFaceSet(0xD, 2, 0x80320000, 1, 0x20200000);
  func_80048C94((u16) ((g_BtlSlotIds[g_BtlActingSlot] - 1) * 5 + 1), 0);
  for (i = 0; i < 6; i++)
  {
    *(u8 *) (i * 12 + g_BtlMenuItems) = 1;
    *(u8 *) (i * 12 + g_BtlMenuItems + 1) = 0;
    ((BtlMenuItem *) g_BtlMenuItems)[i].attr = 0x200000;
    ((BtlMenuItem *) g_BtlMenuItems)[i].x = (u16) (i % 3) * 5 + 0x17;
    ((BtlMenuItem *) g_BtlMenuItems)[i].y = (u16) (i / 3) * 5 + 2;
  }
  g_BtlMenuItemCount = 0xA;
  for (i = 0; i < 4; i++)
  {
    *(u8 *) (i * 12 + g_BtlMenuItems + 0x48) = 1;
    *(u8 *) (i * 12 + g_BtlMenuItems + 0x49) = 1;
    cell = (BtlMenuItem *) (i * 12 + g_BtlMenuItems);
    cell[6].x = (i % 2) * 5 + 3;
    cell[6].y = (i / 2) * 5 + 2;
    cell[6].attr = 0x200000;
  }
  cells = (BtlMenuItem *) g_BtlMenuItems;
  cells[7].attr = 0x20200000;
  cells[7].disabled = 0;
  ((BtlRecord *)g_BtlMenuMarkRecords)[3].sprite.x = 0x40;
  ((BtlRecord *)g_BtlMenuMarkRecords)[3].sprite.y = 0x10;
  cells = (BtlMenuItem *) g_BtlMenuItems;
  AS(s16, cells[0].icon) = 0xED;
  AS(s16, cells[1].icon) = 0xEA;
  AS(s16, cells[2].icon) = 0xEB;
  AS(s16, cells[3].icon) = 0xE9;
  AS(s16, cells[4].icon) = 0xEC;
  AS(s16, cells[5].icon) = 0xEE;
  AS(s16, cells[6].icon) = 0xED;
  AS(s16, cells[7].icon) = 0xF0;
  AS(s16, cells[8].icon) = 0xEF;
  AS(s16, cells[9].icon) = 0xEE;
  for (i = 0; (i < 8) && (((u8 *) (g_BtlActingSlot * 0x3C + CharStats) + i)[0x34] == 0); i++)
  {
  }
  m = g_BtlSlotIds[g_BtlActingSlot];
  if ((m == 0) || (m == 0xC) || (i >= 8) || (((u8 *) (CharStats + g_BtlActingSlot * 0x3C))[0x15] & 0x80))
  {
    ((BtlMenuItem *) g_BtlMenuItems)[1].disabled = 1;
  }
  if (StoryBitCheck(0x3D9))
  {
    ((BtlMenuItem *) g_BtlMenuItems)[4].disabled = 1;
  }
  g_BtlMenuCursor = g_BtlMenuKindKeptCursors[g_BtlSlotIds[g_BtlActingSlot]];
  BtlMenuTextStringPut(0, (u16) (AS(s16, ((BtlMenuItem *) g_BtlMenuItems)[7].icon) + 0x100), 0, 5);
  BtlMenuTextImageLoad(0, 0, 5);
  BtlMenuKindDisp();
  ((BtlRecord *)g_BtlMenuMarkRecords)[3].on = 1;
  ((BtlRecord *)g_BtlMenuMarkRecords)[2].on = 1;
  BtlWindowsMoveWait();
}

void func_80048C94();  /* TU defines this below */
void BtlCellRowsUpdate();  /* TU defines this below */

void BtlMenuClear();                                  /* extern */

/* func_80045E98 */
void BtlMenuKindClose(void) {
    ((BtlRecord *)g_BtlMenuMarkRecords)[3].on = 0;
    BtlMenuClear();
    (*(s16 *)((u8 *)((g_BtlActingSlot << 6) + g_BtlRecords) + 0x4)) = 1;
}
/* far_syms: g_BtlRecords */
/* far_syms: g_MenuPadRecordRepeated */
/* far_syms: g_BtlMenuPadRecord */
void BtlMenuPulseStep();  /* TU defines this below */
u16 BtlMenuTargetMaskGet(u8 arg);  /* TU defines this below */
void BtlPartyActionSet(s32);

/* UI state 6: the input of one party member's command grid in battle
   (docs/spec/code/subsystems/menu-ui.md, "The battle command windows").
   g_BtlActingSlot is the member's slot, g_BtlMenuCursor the cursor (0..5),
   g_BtlMenuStep the next UI state, g_BtlMenuSoundRequest the menu sound request.

   - CROSS (press word g_MenuPadRecordRepeated[0] == 0x40): the cursor picks the command
     id g_BtlActionKind (cursor 0..5 -> 1, 3, 5, 2, 4, 6). A cell that is
     switched off (cells[cursor].off == 1), or a command
     BtlMenuActionCheck(the member's record, command, 0, 1) refuses, asks for
     sound 4 and only redraws (the tail). Otherwise the command goes to
     g_BtlSlotPlannedKinds[slot], sound 2, BtlMenuKindClose(), and by command: 1 runs
     BtlPartyActionSet(0) and, like 6, sets state 5 and moves on to the next
     slot; 3 -> state 7; 5 -> state 0xD; 4 -> state 9; 2 -> state 0xB with
     g_BtlMenuTargetMode = 1 and g_BtlMenuTargetMask = BtlMenuTargetMaskGet(1).
   - TRIANGLE (press word == 0x10): sound 3, state 5, BtlMenuKindClose(), then
     one step back: the slot goes down to the nearest earlier one whose bit
     is set in the pending mask g_BtlMenuPendingMask. With none before it (slot 0
     reached with its bit clear, or the slot already 0) the state is 1.
   - A direction (repeat word g_BtlMenuPadRecord[2] & 0xF000): sound 1, the cursor
     through BtlMenuCursorMove(pad record, cells, 3, 2, cursor); when it
     differs from the member's remembered one
     (g_BtlMenuKindKeptCursors[g_BtlSlotIds[slot]]) that is stored and BtlMenuKindDisp()
     redraws.
   - The tail (after a direction, and after a refused confirm):
     BtlMenuPulseStep(g_BtlMenuPulse, 0), and the slot's battle object takes the
     halfword at g_BtlMenuPulse + 4 into its +4.
   No RNG draw and no frame wait of its own.

   MARKED SPELLING (a stand-in, on the owner's decision): the test
   `if (g_BtlMenuPendingMask == r)` in the cancel arm, and the variable `r` that
   holds the 3. Its two outcomes are the same on purpose. What the bytes
   prove is that the original had a conditional on a variable holding 3,
   read after `jal BtlMenuKindClose`, whose two
   outcomes were the same instructions: the 3 sits in $s0, which is
   saved and restored for nothing the final code reads, and the last bit
   test takes its mask and slot in $v1 / $v0. What the bytes cannot show
   is what that conditional said or which arm it was in; eight plain
   forms give these same bytes. THIS WORDING IS OURS, NOT THE ORIGINAL'S.
   Without it the function is 200/223. The US demo of three weeks
   earlier, the Japanese build and the sibling states were all checked
   and none shows it.

   MARKED SPELLING: the pointer `menu` to g_BtlMenuItems for BtlMenuCursorMove's
   second argument. The target loads that argument straight into $a1
   after the cursor; with the global written as the argument the compiler
   loads it into a temporary first (three rows off). The bytes need the
   argument read through an address held in a register; the pointer's
   name and type are ours (the same finding as BtlMenuFirstStep). */
/* func_80045EDC */
void BtlMenuKindStep(void)
{
    BtlMenuItem **menu = (BtlMenuItem **)&g_BtlMenuItems;
    s32 r;
    if (g_MenuPadRecordRepeated[0] == 0x40) {
        switch (g_BtlMenuCursor) {
        case 0:
            g_BtlActionKind = 1;
            break;
        case 1:
            g_BtlActionKind = 3;
            break;
        case 2:
            g_BtlActionKind = 5;
            break;
        case 3:
            g_BtlActionKind = 2;
            break;
        case 4:
            g_BtlActionKind = 4;
            break;
        case 5:
            g_BtlActionKind = 6;
            break;
        }
        if (((BtlMenuItem *)g_BtlMenuItems)[g_BtlMenuCursor].disabled == 1
            || BtlMenuActionCheck(CharStats + g_BtlActingSlot * 0x3C, g_BtlActionKind, 0, 1) == 0) {
            g_BtlMenuSoundRequest = 4;
            goto tail;
        }
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = g_BtlActionKind;
        g_BtlMenuSoundRequest = 2;
        BtlMenuKindClose();
        switch (g_BtlActionKind) {
        case 1:
            BtlPartyActionSet(0);
        case 6:
            g_BtlMenuStep = 5;
            g_BtlActingSlot++;
            return;
        case 3:
            g_BtlMenuStep = 7;
            return;
        case 5:
            g_BtlMenuStep = 0xD;
            return;
        case 2:
            g_BtlMenuStep = 0xB;
            g_BtlMenuTargetMode = 1;
            g_BtlMenuTargetMask = BtlMenuTargetMaskGet(1);
            return;
        case 4:
            g_BtlMenuStep = 9;
            return;
        }
        return;
    } else if (g_MenuPadRecordRepeated[0] == 0x10) {
        r = 3;
        g_BtlMenuSoundRequest = r;
        g_BtlMenuStep = 5;
        BtlMenuKindClose();
        if (g_BtlActingSlot != 0) {
            g_BtlActingSlot--;
            while (g_BtlActingSlot != 0 && !((g_BtlMenuPendingMask >> g_BtlActingSlot) & 1)) {
                g_BtlActingSlot--;
            }
            if (g_BtlActingSlot != 0) {
                return;
            }
            if ((g_BtlMenuPendingMask >> g_BtlActingSlot) & 1) {
                return;
            }
            if (g_BtlMenuPendingMask == r) {
                g_BtlMenuStep = 1;
                return;
            }
            g_BtlMenuStep = 1;
            return;
        }
        g_BtlMenuStep = 1;
        return;
    } else if (g_BtlMenuPadRecord[2] & 0xF000) {
        g_BtlMenuSoundRequest = 1;
        g_BtlMenuCursor = BtlMenuCursorMove((u8 *)g_BtlMenuPadRecord, (s32)*menu, 3, 2, g_BtlMenuCursor) & 0xFF;
        if (g_BtlMenuKindKeptCursors[g_BtlSlotIds[g_BtlActingSlot]] != g_BtlMenuCursor) {
            g_BtlMenuKindKeptCursors[g_BtlSlotIds[g_BtlActingSlot]] = g_BtlMenuCursor;
            BtlMenuKindDisp();
        }
    }
tail:
    BtlMenuPulseStep(g_BtlMenuPulse, 0);
    *(u16 *)((g_BtlActingSlot << 6) + g_BtlRecords + 4) = AS(u16, ((BtlMenuPulse *)g_BtlMenuPulse)->down);
}

/* far_syms: g_BtlSpellRows (target reaches it %hi/%lo) */
/* One 12-byte row of the spell table (records.spells): g_BtlSpellRows[id - 1] is
   spell id's row. Named as the first split's game_state.c named it; the fields
   that TU did not read carry records.spells' names. */
extern BtlSpellRow g_BtlSpellRows[];
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
s32 BtlSlotActionAnimationSet(u8);
s32 BtlCellAreaClear(s32, s32, s32, s32, s32, s32);
s32 CellNumberSet(u32, s32, s32, s32, s32, s32, s32, s32);

static inline u8 *spell_row_ptr(s32 icon, u8 *spell_rows)
{
  return ((icon - 0xB7) * 0xC) + spell_rows;
}
/* func_80046258 */
s32 BtlMenuSpellDisp(s32 *unused)
{
  s32 col;
  s32 masked_icon;
  s32 spell_number;
  u16 icon;
  u8 cost;
  icon = *((s16 *) (((u8 *) ((g_BtlMenuCursor * 0xC) + g_BtlMenuItems)) + 0x6));
  spell_number = icon + 0x4A;
  g_BtlActionNumber = spell_number;
  *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = spell_number;
  BtlSlotActionAnimationSet(g_BtlActingSlot);
  col = ((*((u8 *) (((u8 *) ((g_BtlMenuCursor * 0xC) + g_BtlMenuItems)) + 0x1))) != 0) * 4;
  BtlMenuTextStringPut(9, (icon + 0x100) & 0xFFFF, col, 0xC);
  BtlMenuTextStringPut(0x15, (icon + 0x200) & 0xFFFF, col, 0xC);
  BtlMenuTextImageLoad(9, 9, 0x18);
  masked_icon = 0xFFFF;
  masked_icon = icon & masked_icon;
  if (masked_icon != 0)
  {
    g_BtlTargetSlot = g_BtlActingSlot;
    if (BtlTargetTraitCheck(0, 0x1B, 0, 0) != 0)
    {
      cost = ((u32) ((*((u8 *) (((u8 *) spell_row_ptr(masked_icon, (u8 *) g_BtlSpellRows)) + 0x4))) + 1)) >> 1;
    }
    else
    {
      cost = (u32) (*((s32 *) (((u8 *) spell_row_ptr(masked_icon, (u8 *) g_BtlSpellRows)) + 0x4)));
    }
  }
  else
  {
    cost = 0;
  }
  BtlCellAreaClear(0x1D, 0xC, 0xCC00, 3, 2, 0x20);
  CellNumberSet(cost, 0x1D, 0xC, 3, 0x80300000, 0x3000, 1, 0);
  BtlCellRowsUpdate(0xC, 0x300, 2, 0x20);
  BtlMenuIconCellsSet(1, 0xC);
}
/* far_syms: g_BtlActingSlot g_BtlActionNumber g_BtlMenuItems g_BtlMenuItemCount CharStats g_BtlSlotIds g_BtlSlotPlannedKinds g_BtlSlotPlannedNumbers g_BtlMenuSpellKeptCursors */
s32 BtlCellGlyphSet(s32, s32, s32, s32, s32, s32);

/* The magic-command grid: cells 0..7 from the caster's spell list
   (CharStats+0x34), four to a row (x = col*4 + 12, y = row*4 + 7); a cell
   is greyed (off) when BtlMenuActionCheck says the spell cannot be cast now.
   The cursor (g_BtlMenuCursor) starts at the remembered cell (g_BtlMenuSpellKeptCursors[...]),
   walking down then up to the nearest live cell; cell 8 is the fixed back
   button (the Magic icon). Then the "MP" caption (string 0x611), a big "/"
   (glyph 0x9C), the caster's MP (CharStats+4; colour 0x2000 at max MP,
   CharStats+6, else 0x3000) and the cursor spell's name/description
   (BtlMenuSpellDisp, which reads g_BtlMenuCursor itself: no argument). */
/* func_80046434 */
void BtlMenuSpellOpen(void)
{
    u16 i;
    s32 color;
    u8 *st;

    g_BtlMenuStep = 8;
    g_BtlActionNumber = 0;
    g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0;
    BtlMenuTextCellsSet(6, 3, 5, 1, 0x80300000, 0);
    BtlMenuTextCellsSet(0x1C, 0xA, 4, 1, 0x80300000, 5);
    BtlMenuTextCellsSet(0xC, 2, 0xC, 2, 0x80300000, 9);
    BtlCellRowsUpdate(2, 0x80, 4, 0x20);
    BtlWindow0Set(2, 1, 0x24, 0xE);
    BtlMenuActingFaceSet(3, 6, 0x80320000, 1, 0x20200000);
    func_80048C94((u16)((g_BtlSlotIds[g_BtlActingSlot] - 1) * 5 + 1), 0);
    for (i = 0; i < 8; i++) {
        ((BtlMenuItem *)g_BtlMenuItems)[i].x = (i % 4) * 4 + 0xC;
        ((BtlMenuItem *)g_BtlMenuItems)[i].y = (i / 4) * 4 + 7;
        if (*(u8 *)(CharStats + g_BtlActingSlot * 0x3C + i + 0x34) != 0) {
            ((BtlMenuItem *)g_BtlMenuItems)[i].on = 1;
            AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[i].icon) = *(u8 *)(CharStats + g_BtlActingSlot * 0x3C + i + 0x34) + 0xB6;
            ((BtlMenuItem *)g_BtlMenuItems)[i].attr = 0x20200000;
            if (BtlMenuActionCheck(CharStats + g_BtlActingSlot * 0x3C, g_BtlSlotPlannedKinds[g_BtlActingSlot],
                              AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[i].icon), 0) != 0) {
                ((BtlMenuItem *)g_BtlMenuItems)[i].disabled = 0;
            } else {
                ((BtlMenuItem *)g_BtlMenuItems)[i].disabled = 1;
            }
        } else {
            ((BtlMenuItem *)g_BtlMenuItems)[i].on = 0;
            AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[i].icon) = ((BtlMenuItem *)g_BtlMenuItems)[i].attr = 0;
        }
    }
    for (i = g_BtlMenuSpellKeptCursors[g_BtlSlotIds[g_BtlActingSlot]]; i != 0 && ((BtlMenuItem *)g_BtlMenuItems)[i].on == 0; i--) {
    }
    if (((BtlMenuItem *)g_BtlMenuItems)[i].on == 0) {
        for (i = g_BtlMenuSpellKeptCursors[g_BtlSlotIds[g_BtlActingSlot]]; i < 8 && ((BtlMenuItem *)g_BtlMenuItems)[i].on == 0; i++) {
        }
        if (((BtlMenuItem *)g_BtlMenuItems)[i].on == 0) {
            g_BtlMenuCursor = 0;
            goto done;
        }
    }
    g_BtlMenuCursor = i;
done:
    ((BtlMenuItem *)g_BtlMenuItems)[8].on = 1;
    ((BtlMenuItem *)g_BtlMenuItems)[8].disabled = 0;
    g_BtlMenuItemCount = 9;
    AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[8].icon) = 0xEA;
    ((BtlMenuItem *)g_BtlMenuItems)[8].attr = 0x20200000;
    ((BtlMenuItem *)g_BtlMenuItems)[8].x = 3;
    ((BtlMenuItem *)g_BtlMenuItems)[8].y = 2;
    BtlMenuTextStringPut(0, (u16)(AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[8].icon) + 0x100), 0, 5);
    BtlMenuTextStringPut(5, 0x611, 0, 4);
    BtlCellGlyphSet(0x9C, 0x20, 0xC, 1, 0x3000, 1);
    st = (u8 *)(g_BtlActingSlot * 0x3C + CharStats);
    color = 0x3000;
    if (((BtlStats *)st)->mp_max == ((BtlStats *)st)->mp) {
        color = 0x2000;
    }
    CellNumberSet(((BtlStats *)st)->mp, 0x21, 0xC, 3, 0x80300000, color, 1, 0);
    BtlMenuTextImageLoad(0, 0, 9);
    ((void (*)())BtlMenuSpellDisp)();
    ((BtlRecord *)g_BtlMenuMarkRecords)[2].on = 1;
    BtlWindowsMoveWait();
}

/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuSpellKeptCursors (target reaches it %hi/%lo) */
/* far_syms: g_MenuPadRecordRepeated (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */
s32 BtlActingRestAnimationSet();
s32 BtlMenuCursorMove(u8 *, s32, s32, s32, s32);
s32 BtlMenuActionCheck(s32, u8, u8, s32);

/* func_800469AC */
void BtlMenuSpellStep(void)
{
  s32 offset_then_cursor;
  u16 old_cursor;
  u16 new_cursor;
  void *menu_item;
  s32 *menu_items;
  u8 acting_slot;
  if (g_MenuPadRecordRepeated[0] == 0x40)
  {
    menu_item = (6 * (g_BtlMenuCursor * 2)) + g_BtlMenuItems;
    if ((((BtlMenuItem *)menu_item)->on == 0) || ((short) (((BtlMenuItem *)menu_item)->disabled != 0)))
    {
      g_BtlMenuSoundRequest = 4;
      return;
    }
    g_BtlMenuSoundRequest = 2;
    BtlMenuActionCheck(CharStats + (g_BtlActingSlot * 0x3C), *(g_BtlActingSlot + g_BtlSlotPlannedKinds), AS(s16, ((BtlMenuItem *)menu_item)->icon), 0);
    acting_slot = g_BtlActingSlot;
    *(acting_slot + g_BtlSlotPlannedNumbers) = (*((s16 *) (((u8 *) ((g_BtlMenuCursor * 0xC) + g_BtlMenuItems)) + 0x6))) + 0x4A;
    if (g_BtlMenuTargetMask == 0)
    {
      if ((*(g_BtlActingSlot + g_BtlSlotPlannedNumbers)) != 0x1E)
      {
        g_BtlMenuStep = 5;
        g_BtlActingSlot += 1;
      }
      else
      {
        goto block_8;
      }
    }
    else
    {
      block_8:
      g_BtlMenuStep = 0xB;

    }
    BtlMenuClear();
    return;
  }
  menu_items = &g_BtlMenuItems;
  if (g_MenuPadRecordRepeated[0] == 0x10)
  {
    g_BtlMenuSoundRequest = 3;
    g_BtlMenuStep = 5;
    BtlMenuClear();
    BtlActingRestAnimationSet();
    return;
  }
  offset_then_cursor = 0x4;
  if ((*((u16 *) (((u8 *) g_BtlMenuPadRecord) + offset_then_cursor))) & 0xF000)
  {

    old_cursor = g_BtlMenuCursor;
    offset_then_cursor = (s32) g_BtlMenuCursor;
    g_BtlMenuSoundRequest = 1;
    new_cursor = BtlMenuCursorMove(g_BtlMenuPadRecord, *menu_items, 4, 2, offset_then_cursor) & 0xFF;
    g_BtlMenuCursor = new_cursor;
    if ((old_cursor & 0xFFFF) != new_cursor)
    {
      *((*(g_BtlActingSlot + g_BtlSlotIds)) + g_BtlMenuSpellKeptCursors) = (u8) g_BtlMenuCursor;
      BtlMenuSpellDisp(g_BtlMenuSpellKeptCursors);
    }
  }
}

s32 pad_record_update(s32 *, s32);
void BtlAnimationStep();
s32 BtlFrameStep();
void BtlWindowsMoveStep(void);
s32 SoundPlay(u8, s32);
void BtlMenuFrameStep(void);  /* TU defines this below */
s32 func_8001B94C();
s32 OutlineScreenStep(s32, s32, s32);
s32 PlayTimeStep();
void BtlMenuOutlineStep(void);  /* TU defines this below */
void BtlCellRowsClear(s32, s32, s32);
void BtlMenuCellsClear(s32 kind_mask);  /* TU defines this below */
void func_80048C94();  /* TU defines this below */
void BtlCellRowsUpdate();  /* TU defines this below */
void BtlWindowsMoveWait(void);  /* TU defines this below */

/* func_80046BB4 */
void BtlMenuItemDisp()
{
  s32 col;
  u16 icon;
  void *menu_item;
  { s32 idx = g_BtlMenuCursor * 0xC; menu_item = (u8 *)(idx + g_BtlMenuItems); }
  if ((*(((u8 *) menu_item) + 0x0)) != 0)
  {
    icon = AS(s16, ((BtlMenuItem *)menu_item)->icon);
    col = (((BtlMenuItem *)menu_item)->disabled != 0) * 4;
    BtlMenuTextStringPut(5, (icon + 0x100) & 0xFFFF, col, 0xC);
    BtlMenuTextStringPut(0x11, (icon + 0x200) & 0xFFFF, col, 0xC);
    BtlMenuTextImageLoad(5, 5, 0x18);
  }
  else
  {
    BtlMenuTextStringPut(5, 0x60C, 0, 0x18);
    BtlMenuTextImageLoad(5, 5, 0x18);
    g_BtlMenuTargetMode = 9;
    g_BtlMenuTargetMask = 0;
  }
  BtlWindowsMoveWait();
  BtlMenuIconCellsSet(1, 0xC);
}
/* far_syms: g_BtlActingSlot g_BtlActionNumber g_BtlMenuItems g_BtlMenuItemCount CharStats g_BtlSlotIds g_BtlSlotPlannedKinds g_BtlSlotPlannedNumbers g_BtlMenuItemKeptCursors */
/* The items of the menu's second row, read from item i: one array whose element 6
   on is the row's first cell, so that each access keeps one constant offset (0x48 +
   the field's) as the match needs; indexing the item pointer itself makes GCC
   form the element's address first. */
typedef struct { BtlMenuItem item[7]; } BtlMenuItemRun;

/* The item-command grid: cells 0..5 from the carried items
   (CharStats+0x22), cells 6..11 from the second row (+0x2E); a cell is
   greyed (off) when BtlMenuActionCheck says the item cannot be used now. An
   empty grid shows the "no items" caption. The cursor (g_BtlMenuCursor) starts
   at the remembered cell (g_BtlMenuItemKeptCursors[...]), walking down then up to the
   nearest live cell; cell 12 is the fixed back button. */
/* func_80046C98 */
void BtlMenuItemOpen(void)
{
    u32 i;
    s32 flag;
    u8 *items;

    g_BtlMenuStep = 0xA;
    g_BtlActionNumber = 0;
    g_BtlSlotPlannedNumbers[g_BtlActingSlot] = 0;
    BtlMenuTextCellsSet(5, 3, 5, 1, 0x80300000, 0);
    BtlMenuTextCellsSet(0xC, 2, 0xC, 2, 0x80300000, 5);
    BtlCellRowsUpdate(2, 0x80, 4, 0x20);
    BtlWindow0Set(2, 1, 0x24, 0xE);
    BtlMenuActingFaceSet(3, 6, 0x80320000, 1, 0x20200000);
    func_80048C94((u16)((g_BtlSlotIds[g_BtlActingSlot] - 1) * 5 + 1), 0);
    flag = 0;
    items = (u8 *)(CharStats + g_BtlActingSlot * 0x3C) + 0x22;
    for (i = 0; i < 6; i++) {
        if (items[i] != 0) {
            ((BtlMenuItem *)g_BtlMenuItems)[i].on = 1;
            AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[i].icon) = items[i];
            ((BtlMenuItem *)g_BtlMenuItems)[i].attr = 0x20200000;
            ((BtlMenuItem *)g_BtlMenuItems)[i].x = i * 4 + 0xD;
            ((BtlMenuItem *)g_BtlMenuItems)[i].y = 7;
            if (BtlMenuActionCheck(CharStats + g_BtlActingSlot * 0x3C, g_BtlSlotPlannedKinds[g_BtlActingSlot],
                              AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[i].icon), 0) != 0) {
                ((BtlMenuItem *)g_BtlMenuItems)[i].disabled = 0;
            } else {
                ((BtlMenuItem *)g_BtlMenuItems)[i].disabled = 1;
            }
            flag = 1;
        } else {
            ((BtlMenuItem *)g_BtlMenuItems)[i].on = 0;
            AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[i].icon) = ((BtlMenuItem *)g_BtlMenuItems)[i].attr =
                ((BtlMenuItem *)g_BtlMenuItems)[i].x = ((BtlMenuItem *)g_BtlMenuItems)[i].y = 0;
        }
    }
    items = (u8 *)(CharStats + g_BtlActingSlot * 0x3C) + 0x2E;
    for (i = 0; i < 6; i++) {
        if (items[i] != 0) {
            ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].on = 1;
            AS(s16, ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].icon) = items[i];
            ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].attr = 0x20200000;
            ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].x = i * 4 + 0xD;
            ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].y = 0xB;
            if (BtlMenuActionCheck(CharStats + g_BtlActingSlot * 0x3C, g_BtlSlotPlannedKinds[g_BtlActingSlot],
                              AS(s16, ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].icon), 0) != 0) {
                ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].disabled = 0;
            } else {
                ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].disabled = 1;
            }
            flag = 1;
        } else {
            ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].on = 0;
            AS(s16, ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].icon) = ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].attr =
                ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].x = ((BtlMenuItemRun *)(i * 12 + g_BtlMenuItems))->item[6].y = 0;
        }
    }
    if (flag == 0) {
        BtlMenuTextStringPut(5, 0x60C, 0, 0x18);
        BtlMenuTextImageLoad(5, 5, 0x18);
        ((BtlRecord *)g_BtlMenuMarkRecords)[2].sprite.x = 0x68;
        ((BtlRecord *)g_BtlMenuMarkRecords)[2].sprite.y = 0x38;
    }
    for (i = g_BtlMenuItemKeptCursors[g_BtlSlotIds[g_BtlActingSlot]]; i != 0 && ((BtlMenuItem *)g_BtlMenuItems)[i].on == 0; i--) {
    }
    if (((BtlMenuItem *)g_BtlMenuItems)[i].on == 0) {
        for (i = g_BtlMenuItemKeptCursors[g_BtlSlotIds[g_BtlActingSlot]]; i < 12 && ((BtlMenuItem *)g_BtlMenuItems)[i].on == 0; i++) {
        }
        if (((BtlMenuItem *)g_BtlMenuItems)[i].on == 0) {
            g_BtlMenuItemKeptCursors[g_BtlSlotIds[g_BtlActingSlot]] = 0;
            g_BtlMenuCursor = 0;
            goto done;
        }
    }
    g_BtlMenuCursor = i;
done:
    ((BtlMenuItem *)g_BtlMenuItems)[12].on = 1;
    ((BtlMenuItem *)g_BtlMenuItems)[12].disabled = 0;
    g_BtlMenuItemCount = 0xD;
    AS(s16, ((BtlMenuItem *)g_BtlMenuItems)[12].icon) = 0xEC;
    ((BtlMenuItem *)g_BtlMenuItems)[12].attr = 0x20200000;
    ((BtlMenuItem *)g_BtlMenuItems)[12].x = 3;
    ((BtlMenuItem *)g_BtlMenuItems)[12].y = 2;
    BtlMenuTextImageLoad(0, 0, 5);
    BtlMenuItemDisp();
    ((BtlRecord *)g_BtlMenuMarkRecords)[2].on = 1;
    BtlWindowsMoveWait();
}

/* far_syms: g_BtlMenuItemKeptCursors (target reaches it %hi/%lo) */

/* func_800472B4 */
void BtlMenuItemStep(void)
{
  s8 next_step;
  /* CODEGEN FORCING DEVICE (address of a global in a local), not recovered source:
     `pad_record` only holds the address of `g_BtlMenuPadRecord`, through the type
     `u8 (*)[]`; behaviour is the same with the address written where it is used, but
     then 3 of this function's 102 instruction lines differ. */
  u8 (*pad_record)[];
  s32 *menu_items;
  u16 old_cursor;
  u16 new_cursor;
  void *menu_item_or_pad;
  if ((g_MenuPadRecordRepeated[0]) == 0x40)
  {
    menu_item_or_pad = (g_BtlMenuCursor * 0xC) + g_BtlMenuItems;
    if ((((BtlMenuItem *) menu_item_or_pad)->on == 0) || (((BtlMenuItem *) menu_item_or_pad)->disabled != 0))
    {
      g_BtlMenuSoundRequest = 4;
      return;
    }
    g_BtlMenuSoundRequest = 2;
    *(g_BtlActingSlot + g_BtlSlotPlannedNumbers) = ((BtlMenuItem *) menu_item_or_pad)->icon;
    BtlMenuActionCheck(CharStats + (g_BtlActingSlot * 0x3C), *(g_BtlActingSlot + g_BtlSlotPlannedKinds), *(g_BtlActingSlot + g_BtlSlotPlannedNumbers), 0);
    if (g_BtlMenuTargetMask == 0)
    {
      g_BtlMenuStep = 5;
      g_BtlActingSlot += 1;
    }
    else
    {
      next_step = 0xB;
      goto block_8;
    }
    goto block_9;
  }
  if ((g_MenuPadRecordRepeated[0]) == 0x10)
  {
    g_BtlMenuSoundRequest = 3;
    next_step = 5;
    block_8:
    g_BtlMenuStep = next_step;

    block_9:
    BtlMenuClear();

    if (g_BtlMenuCursor)
    {
      return;
    }
    else
    {
      return;
    }
  }
  pad_record = &g_BtlMenuPadRecord;
  menu_item_or_pad = (u8 *) (*pad_record);
  if (((PadRecord *) menu_item_or_pad)->repeated & 0xF000)
  {
    old_cursor = g_BtlMenuCursor;
    g_BtlMenuSoundRequest = 1;
    menu_items = &g_BtlMenuItems;
    new_cursor = BtlMenuCursorMove(*pad_record, *menu_items, 6, 2, (s32) g_BtlMenuCursor) & 0xFF;
    g_BtlMenuCursor = new_cursor;
    if ((old_cursor & 0xFFFF) != new_cursor)
    {
      *((*(g_BtlActingSlot + g_BtlSlotIds)) + g_BtlMenuItemKeptCursors) = (u8) g_BtlMenuCursor;
      BtlMenuItemDisp(g_BtlMenuItemKeptCursors);
    }
  }
}
/* far_syms: g_BtlActingStatus (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */

extern s8 g_BtlActingStatus;
extern s16 g_BtlAreaHalfWidth;
extern s16 g_BtlAreaHalfHeight;

/* load_hazard_nop: BtlMenuTargetMark */
void BtlTargetRowsMark();
void BtlTargetAreaMark();

/* func_8004746C */
void BtlMenuTargetMark(void)
{
  u8 *spells;
  u8 *r;
  u8 *row;
  s16 i;
  s16 n;
  r = (u8 *) ((g_BtlMenuCursor << 6) + g_BtlRecords);
  ((BtlRecord *)g_BtlMenuMarkRecords)->sprite.x = ((BtlRecord *)r)->sprite.x;
  ((BtlRecord *)g_BtlMenuMarkRecords)->sprite.y = ((BtlRecord *)r)->sprite.y - AS(s16, ((BtlRecord *)r)->height);
  ((BtlRecord *)g_BtlMenuMarkRecords)->sprite.depth = (((BtlRecord *)r)->sprite.y + 1) | 0x1000;
  if (((BtlRecord *)g_BtlMenuMarkRecords)->sprite.y < 0x28)
  {
    ((BtlRecord *)g_BtlMenuMarkRecords)->sprite.y = 0x28;
  }
  n = 0;
  for (i = 4; i >= 0; i--)
  {
    if (g_BtlMenuCursor == i)
    {
      break;
    }
    if ((g_BtlSlotIds[i] != 0) && (((g_BtlSlotActiveFlags[i] != 0) || ((((u8 *) ((i * 0x3C) + CharStats))[0x15] & 0x30) == 0x30)) || ((*((u16 *) ((i * 0x3C) + CharStats))) == 0)))
    {
      n++;
    }
  }

  ((BtlRecord *)g_BtlMenuMarkRecords)[1].sprite.x = (0x1F - (n * 6)) * 8;
  ((BtlRecord *)g_BtlMenuMarkRecords)[1].sprite.y = 0xB8;
  for (i = 0; i < 0xE; i++)
  {
    g_BtlSlotAmountKinds[i] = 0;
  }

  if (g_BtlMenuMarkKind != 0)
  {
    spells = (u8 *) g_BtlSpellRows;
    g_BtlActingStatus = 0;
    g_BtlTargetSlot = g_BtlMenuCursor;
    row = ((g_BtlActionNumber - 1) * 12) + spells;
    g_BtlAreaHalfWidth = row[9];
    g_BtlAreaHalfHeight = row[10];
    if (g_BtlMenuMarkKind == 1)
    {
      BtlTargetAreaMark(g_BtlMenuTargetMode & 0xF);
    }
    else
    {
      BtlTargetRowsMark();
    }
  }
  for (i = 0; i < 0xE; i++)
  {
    if ((g_BtlMenuMarkKind == 0) ? (i == g_BtlMenuCursor) : (g_BtlSlotAmountKinds[i] != 0))
    {
      ((BtlRecord *) g_BtlRecords)[i].sprite.flags = (((BtlRecord *) g_BtlRecords)[i].sprite.flags & 0xE3FF) | 0x800;
    }
    else
    {
      ((BtlRecord *) g_BtlRecords)[i].sprite.flags &= 0xE3FF;
    }
  }

}

/* far_syms: CharStats g_BtlRecords g_BtlMenuItems g_BtlMenuItemCount g_BtlSlotIds g_BtlSlotPlannedKinds g_BtlSlotPlannedNumbers g_BtlSlotActiveFlags g_BtlActingSlot g_BtlActionNumber */
/* load_hazard_nop: BtlMenuTargetOpen */

/* func_80047714 */
void BtlMenuTargetOpen(void)
{
    s16 i;
    s16 best;
    s16 x;
    s16 y;
    s16 dx;
    s16 dy;
    u32 mode;
    u8 *tg;
    s32 base;
    s16 *p;
    s32 d;

    g_BtlMenuStep = 0xC;
    BtlMenuTextCellsSet(7, 3, 8, 1, 0x80300000, 0);
    BtlCellRowsUpdate(3, 0xC0, 2, 0x20);
    BtlWindow0Set(2, 1, 0x22, 5);
    layer_mix_switch_on(0x100);
    BtlMenuPulseSet((void *)g_BtlMenuPulse, 1, 0x400, 0xC);
    g_BtlMenuMarkKind = 0;
    switch (g_BtlSlotPlannedKinds[g_BtlActingSlot]) {
    case 2:
        ((BtlMenuItem *)g_BtlMenuItems)->icon = 0xE9;
        break;
    case 3:
        ((BtlMenuItem *)g_BtlMenuItems)->icon = g_BtlSlotPlannedNumbers[g_BtlActingSlot] + 0xB6;
        if (g_BtlActionNumber == 2 || g_BtlActionNumber == 0xA || g_BtlActionNumber == 0xC || g_BtlActionNumber == 0x16 ||
            g_BtlActionNumber == 0x1A || g_BtlActionNumber == 0x1E || g_BtlActionNumber == 0x2B || g_BtlActionNumber == 0x2C ||
            g_BtlActionNumber == 0x2D || g_BtlActionNumber == 0x2F) {
            g_BtlMenuMarkKind = 1;
        } else if (g_BtlActionNumber == 0x1F) {
            g_BtlMenuMarkKind = 2;
        }
        break;
    case 4:
        ((BtlMenuItem *)g_BtlMenuItems)->icon = g_BtlSlotPlannedNumbers[g_BtlActingSlot];
        break;
    }
    ((BtlMenuItem *)g_BtlMenuItems)->on = 1;
    ((BtlMenuItem *)g_BtlMenuItems)->disabled = 0;
    g_BtlMenuItemCount = 1;
    ((BtlMenuItem *)g_BtlMenuItems)->attr = 0x20200000;
    ((BtlMenuItem *)g_BtlMenuItems)->x = 3;
    ((BtlMenuItem *)g_BtlMenuItems)->y = 2;
    BtlMenuTextStringPut(0, (u16)(((BtlMenuItem *)g_BtlMenuItems)->icon + 0x100), 0, 8);
    BtlMenuTextImageLoad(0, 0, 8);
    p = (s16 *)g_BtlMenuMarkRecords;
    mode = g_BtlMenuTargetMode & 0xF;
    p[0] = 1;
    best = 0x7FFF;
    if (mode == 1) {
        tg = (u8 *)((g_BtlActingSlot << 6) + g_BtlRecords);
        base = g_BtlRecords;
        x = AS(u16, ((BtlRecord *)tg)->sprite.x) + 7;
        y = AS(u16, ((BtlRecord *)tg)->sprite.y) + 7;
        for (i = 0; i < 0xE; i++) {
            if ((g_BtlMenuTargetMask >> i) & 1) {
                dx = (x - *(s16 *)((i << 6) + base + 6)) < 0 ? -(x - *(s16 *)((i << 6) + base + 6)) : (x - *(s16 *)((i << 6) + base + 6));
                dy = (y - *(s16 *)((i << 6) + base + 0xA)) < 0 ? -(y - *(s16 *)((i << 6) + base + 0xA)) : (y - *(s16 *)((i << 6) + base + 0xA));
                if (best > dx + dy) {
                    g_BtlMenuCursor = i;
                    best = dx + dy;
                }
            }
        }
    } else {
        p[0x20] = 1;
        switch (mode) {
        case 3:
            for (i = 0; i < 5; i++) {
                if (g_BtlSlotIds[i] != 0 && *(u16 *)(i * 0x3C + CharStats) == 0) {
                    break;
                }
            }
            i = g_BtlActingSlot;
            break;
        case 4:
            if (*(u8 *)(g_BtlActingSlot * 0x3C + CharStats + 0x15) != 0 && g_BtlSlotActiveFlags[g_BtlActingSlot] != 0) {
                i = g_BtlActingSlot;
                break;
            }
            for (i = 0; i < 5; i++) {
                if (*(u8 *)(i * 0x3C + CharStats + 0x15) != 0 && g_BtlSlotActiveFlags[i] != 0) {
                    break;
                }
            }
            i = g_BtlActingSlot;
            break;
        case 5:
            i = 0;
            y = 0;
            x = 0;
            for (; i < 5; i++) {
                d = *(u16 *)(i * 0x3C + CharStats + 2) - *(u16 *)(i * 0x3C + CharStats);
                if (y < d) {
                    if (g_BtlSlotActiveFlags[i] != 0) {
                        x = i;
                        y = d;
                    }
                }
            }
            i = x;
            break;
        case 6:
            i = 0;
            y = 0;
            x = 0;
            for (; i < 5; i++) {
                d = *(u16 *)(i * 0x3C + CharStats + 6) - *(u16 *)(i * 0x3C + CharStats + 4);
                if (y < d) {
                    if (g_BtlSlotActiveFlags[i] != 0) {
                        x = i;
                        y = d;
                    }
                }
            }
            i = x;
            break;
        case 0:
        case 2:
            i = g_BtlActingSlot;
            break;
        }
        g_BtlMenuCursor = i;
    }
    BtlMenuTargetMark();
    BtlMenuIconCellsSet(0, 3);
    BtlWindowsMoveWait();
}

/* far_syms: g_BtlSlotAmountKinds (target reaches it %hi/%lo) */
s32 layer_mix_set();  /* TU declares this below */
s32 layer_mix_switch_off(s32);
void BtlMenuClear();

/* func_80047CEC */
void BtlMenuTargetClose(void)
{
  s32 i;
  u16 slot;
  void *cursor_record;
  void *record;
  s32 lit_0xE3FF;
  /* CODEGEN FORCING DEVICE (address of a global in a local), not recovered source:
     `amount_kinds` only holds the address of `g_BtlSlotAmountKinds`, through the type
     `u8 (*)[]`; behaviour is the same with the address written where it is used, but
     then 3 of this function's 39 instruction lines differ. */
  u8 (*amount_kinds)[];
  i = 0;
  ((BtlRecord *)g_BtlMenuMarkRecords)[1].on = 0;
  ((BtlRecord *)g_BtlMenuMarkRecords)->on = 0;
  cursor_record = (g_BtlMenuCursor << 6) + g_BtlRecords;
  ((BtlRecord *)cursor_record)->sprite.depth = (u16) (((BtlRecord *)cursor_record)->sprite.y);
  slot = 0 & 0xFFFF;
  do
  {
    slot = i & 0xFFFF;
    lit_0xE3FF = 0xE3FF;
    *(slot + (*(amount_kinds = &g_BtlSlotAmountKinds))) = 0;
    record = (slot << 6) + g_BtlRecords;
    i += 1;
    ((BtlRecord *)record)->sprite.flags = (u16) (AS(s16, ((BtlRecord *)record)->sprite.flags) & lit_0xE3FF);
  }
  while (((u32) (i & 0xFFFF)) < 0xEU);
  layer_mix_set(0x400, 0xFF, *amount_kinds);
  layer_mix_switch_off(0x100);
  BtlMenuClear();
}
/* far_syms: g_MenuPadRecordRepeated g_BtlMenuPadRecord g_BtlSlotPlannedTargets g_BtlSlotPlannedKinds g_BtlActingSlot */
void BtlMenuPulseStep();  /* TU defines this below */

/* UI state 12, the target window's input (state 11, BtlMenuTargetOpen, opens
   it). The cursor g_BtlMenuCursor is a battle slot 0..13; g_BtlMenuTargetMask is the
   candidate mask, bit n = slot n may be chosen.

   CROSS (g_MenuPadRecordRepeated[0] == 0x40) stores the cursor as the acting character's
   target, g_BtlSlotPlannedTargets[g_BtlActingSlot], and moves on to the next character
   (state 5). TRIANGLE (0x10) goes back to the step that opened the window,
   by the character's command kind g_BtlSlotPlannedKinds: 2 -> state 5, 3 -> 7,
   4 -> 9; any other kind leaves the state alone. Both close the window
   through BtlMenuTargetClose.

   LEFT (pad word g_BtlMenuPadRecord[2] == 0x8000) walks down from cursor - 1 while
   the slot is above 0 and not a candidate, and takes where it stops if that
   is a candidate (so slot 0 is tried last). Otherwise (slot 0 not one, or
   -1 from a cursor at 0) it wraps: from slot 13 down to the first candidate
   above the cursor. RIGHT (0x2000) walks up from cursor + 1 while the slot
   is below 13 and not a candidate, takes a candidate it stops on (slot 13
   last), and otherwise wraps from slot 0 up to the first candidate below
   the cursor. A wrap that finds nothing ends on the cursor itself, so
   nothing moves. The & 0xA000 test passes any word with either bit while
   the compares want exactly 0x8000 or 0x2000: any other such word (both
   directions, a direction with another bit) leaves the cursor alone and
   still requests the move sound (g_BtlMenuSoundRequest = 1). A moved cursor re-places
   the marks (BtlMenuTargetMark). Every frame ends with the window pulse
   (BtlMenuPulseStep).

   The four walks are `for` loops with BOTH tests in the condition and an
   empty body. That spelling is what gives the target's two peeled
   iterations ahead of each loop: the first jump pass copies the exit test
   twice for it and once for a `break` in the body (that form builds 168
   instructions against 223) or a `while`. Each walk re-reads the cursor global for
   its start, which is the right arm's second `lhu g_BtlMenuCursor`; the left
   arm's is shared with `i = g_BtlMenuCursor`. */
/* func_80047D90 */
void BtlMenuTargetStep(void)
{
    s16 i;

    if (g_MenuPadRecordRepeated[0] == 0x40) {
        g_BtlMenuSoundRequest = 2;
        g_BtlMenuStep = 5;
        g_BtlSlotPlannedTargets[g_BtlActingSlot] = g_BtlMenuCursor;
        BtlMenuTargetClose();
        g_BtlActingSlot++;
    } else if (g_MenuPadRecordRepeated[0] == 0x10) {
        g_BtlMenuSoundRequest = 3;
        switch (g_BtlSlotPlannedKinds[g_BtlActingSlot]) {
        case 2:
            g_BtlMenuStep = 5;
            break;
        case 3:
            g_BtlMenuStep = 7;
            break;
        case 4:
            g_BtlMenuStep = 9;
            break;
        }
        BtlMenuTargetClose();
    } else if (g_BtlMenuPadRecord[2] & 0xA000) {
        g_BtlMenuSoundRequest = 1;
        i = g_BtlMenuCursor;
        if (g_BtlMenuPadRecord[2] == 0x8000) {
            for (i = g_BtlMenuCursor - 1; i > 0 && !((g_BtlMenuTargetMask >> i) & 1); i--) {
            }
            if (i < 0 || !((g_BtlMenuTargetMask >> i) & 1)) {
                for (i = 13; g_BtlMenuCursor < i && !((g_BtlMenuTargetMask >> i) & 1); i--) {
                }
            }
        } else if (g_BtlMenuPadRecord[2] == 0x2000) {
            for (i = g_BtlMenuCursor + 1; i < 13 && !((g_BtlMenuTargetMask >> i) & 1); i++) {
            }
            if (!((g_BtlMenuTargetMask >> i) & 1)) {
                for (i = 0; i < g_BtlMenuCursor && !((g_BtlMenuTargetMask >> i) & 1); i++) {
                }
            }
        }
        if (g_BtlMenuCursor != i) {
            g_BtlMenuCursor = i;
            BtlMenuTargetMark();
        }
    }
    BtlMenuPulseStep(g_BtlMenuPulse, 0);
}

/* far_syms: g_MenuPadRecord (target reaches it %hi/%lo) */
s32 pad_record_init(u8 *, s32, s32, s32);
s32 BtlAnimationSet();

/* func_8004810C */
void BtlMenuWalkOpen(void)
{
    BtlRecord *d;
    BtlRecord *s;

    g_BtlMenuStep = 0xE;
    BtlMenuPulseSet(g_BtlMenuPulse, 1, 0x400, 0xC);
    pad_record_init(g_BtlMenuPadRecord, 0, 0x40, 0);
    pad_record_init(g_MenuPadRecord, 0x20, 0x40, 0);
    BtlMenuTextCellsSet(7, 3, 4, 1, 0x80300000, 0);
    BtlCellRowsUpdate(3, 0xC0, 2, 0x20);
    BtlMenuTextStringPut(0, 0x1EB, 0, 4);
    BtlMenuTextImageLoad(0, 0, 4);
    *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlMenuCopyRecords) =
        *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    d = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlMenuCopyRecords);
    s = (BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
    AS(s16, d->on) = 1;
    d->sprite.flags = (d->sprite.flags & 0xE3FF) | 0x800;
    d->sprite.x = s->sprite.x;
    d->sprite.y = s->sprite.y;
    d->sprite.depth = s->sprite.y + 1;
    BtlWindow0Set(2, 1, 0x22, 5);
    layer_mix_switch_on(0x100);
    ((BtlMenuItem *)g_BtlMenuItems)->on = 1;
    ((BtlMenuItem *)g_BtlMenuItems)->disabled = 0;
    g_BtlMenuItemCount = 1;
    AS(s16, ((BtlMenuItem *)g_BtlMenuItems)->icon) = 0xEB;
    ((BtlMenuItem *)g_BtlMenuItems)->attr = 0x20200000;
    ((BtlMenuItem *)g_BtlMenuItems)->x = 3;
    ((BtlMenuItem *)g_BtlMenuItems)->y = 2;
    BtlMenuIconCellsSet(0, 3);
    BtlWindowsMoveWait();
    if (g_BtlMenuPadRecord[0] & 0xF000)
    {
        BtlAnimationSet(g_BtlMenuCopyRecords + (g_BtlActingSlot << 6), (u16)(g_BtlPartyStandAnimations[g_BtlActingSlot] - 5));
    }
    else
    {
        BtlAnimationSet(g_BtlMenuCopyRecords + (g_BtlActingSlot << 6), 0x13);
    }
}

/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuPadRecord (target reaches it %hi/%lo) */
s32 layer_mix_set();
s32 layer_mix_switch_off(s32);
s32 pad_record_init(u8 *, s32, s32, s32);
void BtlMenuClear();

/* func_8004834C */
void BtlMenuWalkClose()
{
  u8 *copy_palette;
  s32 record_offset_then_mask;
  void *copy_record;
  void *record;
  s32 records;
  pad_record_init(g_BtlMenuPadRecord, 0x10, 0x40, 0);
  layer_mix_set(0x400, 0xFF);
  layer_mix_switch_off(0x100);
  record_offset_then_mask = g_BtlActingSlot << 6;
  copy_record = record_offset_then_mask + g_BtlMenuCopyRecords;
  copy_palette = ((u8 *) copy_record) + 0xe;
  record = record_offset_then_mask + g_BtlRecords;
  record_offset_then_mask = 0xE3FF;
  *((s16 *) copy_palette) = (u16) ((*((s16 *) copy_palette)) & record_offset_then_mask);
  records = g_BtlRecords;
  ((BtlRecord *)copy_record)->on = 0;
  ((BtlRecord *)record)->sprite.flags = (u16) (AS(s16, ((BtlRecord *)record)->sprite.flags) & record_offset_then_mask);
  BtlMenuClear(copy_record, records);
}
s32 BtlAnimationSet();
void BtlMenuPulseStep();  /* TU defines this below */

/* func_800483D4 */
void BtlMenuWalkStep(void)
{
  int x;
  s32 y;
  s32 record_offset;
  s8 *planned_target;
  void *left_copy_record;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `x_zero` only holds the constant 0; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 166 instruction lines
     differs. */
  int x_zero;
  u8 *planned_number;
  void *up_copy_record;
  u8 *stand_animations;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `y_zero` only holds the constant 0; behaviour is the same with the constant
     written where it is read, but then 1 of this function's 166 instruction lines
     differs. */
  int y_zero;
  unsigned short held_buttons;
  void *right_copy_record;
  void *down_copy_record;
  void *copy_record;
  s16 *x_pad_then_depth;
  if (g_MenuPadRecordRepeated[0] == 0x40)
  {
    g_BtlMenuSoundRequest = 2;
    g_BtlMenuStep = 5;
    x = *((s16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlMenuCopyRecords)) + 0x6));
    x_zero = 0;
    planned_number = g_BtlActingSlot + g_BtlSlotPlannedNumbers;
    y_zero = 0;
    if (x < x_zero)
    {
      x += 7;
    }
    *planned_number = (s8) (x >> 3);
    y = *((s16 *) (((u8 *) ((g_BtlActingSlot << 6) + g_BtlMenuCopyRecords)) + 0xa));
    planned_target = g_BtlActingSlot + g_BtlSlotPlannedTargets;
    if (((long) y) < y_zero)
    {
      y = y + 7;
    }
    *planned_target = (s8) (y >> 3);
    BtlMenuWalkClose(planned_target);
    g_BtlActingSlot += 1;
    return;
  }
  if (g_MenuPadRecordRepeated[0] == 0x10)
  {
    g_BtlMenuSoundRequest = 3;
    g_BtlMenuStep = 5;
    BtlMenuWalkClose(y_zero);
    return;
  }
  held_buttons = AS(s32, ((PadRecord *)g_BtlMenuPadRecord)->held) & 0xF000;
  if (held_buttons)
  {
    if (((PadRecord *)g_BtlMenuPadRecord)->pressed & 0xF000)
    {
      record_offset = g_BtlActingSlot << 6;
      BtlAnimationSet(g_BtlMenuCopyRecords + record_offset, ((*(g_BtlActingSlot + (stand_animations = g_BtlPartyStandAnimations))) - 5) & 0xFFFF, record_offset);
    }
    if (held_buttons = AS(s32, ((PadRecord *)g_BtlMenuPadRecord)->held) & 0x8000)
    {
      left_copy_record = (g_BtlActingSlot << 6) + g_BtlMenuCopyRecords;
      ((BtlRecord *)left_copy_record)->sprite.attr = 0x2020;
      if (((BtlRecord *)left_copy_record)->sprite.x >= 0x1A)
      {
        x_pad_then_depth = (s16 *) (((u8 *) left_copy_record) + 0x6);
        *x_pad_then_depth = (s16) (((u16) (*x_pad_then_depth)) - 2);
      }
    }
    held_buttons = AS(s32, ((PadRecord *)g_BtlMenuPadRecord)->held) & 0x1000;
    if (held_buttons)
    {
      up_copy_record = (g_BtlActingSlot << 6) + g_BtlMenuCopyRecords;
      if (((BtlRecord *)up_copy_record)->sprite.y >= 0x72)
      {
        ((BtlRecord *)up_copy_record)->sprite.y = (s16) (((u16) (((BtlRecord *)up_copy_record)->sprite.y)) - 2);
      }
    }
    if (((unsigned short)(AS(s32, ((PadRecord *)g_BtlMenuPadRecord)->held))) & 0x2000)
    {
      right_copy_record = (g_BtlActingSlot << 6) + g_BtlMenuCopyRecords;
      ((BtlRecord *)right_copy_record)->sprite.attr = 0x20;
      if (((BtlRecord *)right_copy_record)->sprite.x < 0x127)
      {
        ((BtlRecord *)right_copy_record)->sprite.x = (s16) (((u16) (((BtlRecord *)right_copy_record)->sprite.x)) + 2);
      }
    }
    if ((*(x_pad_then_depth = (s32 *) (((u8 *) g_BtlMenuPadRecord) + 0x0))) & 0x4000)
    {
      down_copy_record = (g_BtlActingSlot << 6) + g_BtlMenuCopyRecords;
      if (0xAF > ((BtlRecord *)down_copy_record)->sprite.y)
      {
        ((BtlRecord *)down_copy_record)->sprite.y = (s16) (((u16) (((BtlRecord *)down_copy_record)->sprite.y)) + 2);
      }
    }
    copy_record = (g_BtlActingSlot << 6) + g_BtlMenuCopyRecords;
    x_pad_then_depth = ((u8 *) copy_record) + 0x8;
    *x_pad_then_depth = (s16) (((BtlRecord *)copy_record)->sprite.y + 1);
  }
  else
  {
    held_buttons = AS(s16, ((PadRecord *)g_BtlMenuPadRecord)->held_before);
    if (held_buttons != 0)
    {
      BtlAnimationSet(g_BtlMenuCopyRecords + (g_BtlActingSlot << 6), 0x13, x_zero);
    }
  }
  BtlMenuPulseStep(g_BtlMenuPulse, 0);
}

/* no_split_addresses: BtlMenuIconCellsSet */
/* far_syms: g_BtlMenuItemCount (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuItemCount (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_80048698 */
s32 BtlMenuIconCellsSet(s32 show_cursor, s32 row_count)
{
  u8 *ptr;
  unsigned int i;
  s32 attr;
  /* CODEGEN FORCING DEVICE (copy), not recovered source: `kept_row_count` only
     copies `row_count`, which is not assigned again afterwards; behaviour is the same with `row_count`
     used directly, but then 6 of this function's 60 instruction lines differ. */
  s32 kept_row_count;
  ptr = (u8 *) g_BtlMenuItems;
  i = 0;
  kept_row_count = row_count;
  if (g_BtlMenuItemCount != 0)
  {
    do
    {
      if ((*ptr) != 0)
      {
        if ((show_cursor != 0) && (i == g_BtlMenuCursor))
        {
          ((BtlRecord *)g_BtlMenuMarkRecords)[2].sprite.x = ((BtlMenuItem *)ptr)->x * 8;
          ((BtlRecord *)g_BtlMenuMarkRecords)[2].sprite.y = ((BtlMenuItem *)ptr)->y * 8;
          attr = 0x20200000;
        }
        else
        {
          attr = ((BtlMenuItem *)ptr)->attr;
        }
        IconCellSet(((BtlMenuItem *)ptr)->x, ((BtlMenuItem *)ptr)->y, ((BtlMenuItem *)ptr)->icon, attr ^ 0x20200000);
      }
      i++;
      ptr += 12;
    }
    while (i < g_BtlMenuItemCount);
  }
  BtlCellRowsUpdate(2, 0x80, kept_row_count & 0xFF, 0x10);
}
/* far_syms: g_BtlMenuTextBlocks g_BtlMenuItems g_BtlMenuItemCount */
extern s32 g_BtlMenuTextBlocks;
s32 font_target_clear(s32, s32);

/* Reset the battle command grid.

   Clears 0x40 entries at g_BtlMenuTextBlocks (font_target_clear), runs
   BtlMenuTextImageLoad(i * 16, i * 16, 16) for i = 0..3, then empties the first
   g_BtlMenuItemCount cells of the 12-byte grid at g_BtlMenuItems: attr, icon, y, x and
   the `on` byte (`off` is left as it was).  Finally zeroes the s16s at
   g_BtlMenuMarkRecords + 0xC0 and + 0x80 and issues the two (2, 0xF) / (2, 0x80, 0xF)
   call pairs for rows 0x10 and 0x20. */
/* func_80048794 */
void BtlMenuClear()
{
  u32 i;
  BtlMenuItem *cell;

  font_target_clear(g_BtlMenuTextBlocks, 0x40);
  for (i = 0; i < 4; i++)
  {
    BtlMenuTextImageLoad(i << 4, i << 4, 0x10);
  }
  cell = (BtlMenuItem *) g_BtlMenuItems;
  i = 0;
  while (i < g_BtlMenuItemCount)
  {
    i++;
    cell->attr = 0;
    AS(s16, cell->icon) = 0;
    cell->y = 0;
    cell->x = 0;
    cell->on = 0;
    cell++;
  }
  ((BtlRecord *)g_BtlMenuMarkRecords)[3].on = 0;
  ((BtlRecord *)g_BtlMenuMarkRecords)[2].on = 0;
  BtlCellRowsClear(2, 0xF, 0x10);
  BtlCellRowsUpdate(2, 0x80, 0xF, 0x10);
  BtlCellRowsClear(2, 0xF, 0x20);
  BtlCellRowsUpdate(2, 0x80, 0xF, 0x20);
}

s32 pad_record_update(s32 *, s32);
void BtlAnimationStep();
s32 BtlFrameStep();
void BtlWindowsMoveStep(void);
s32 SoundPlay(u8, s32);
void BtlMenuFrameStep(void);  /* TU defines this below */
s32 func_8001B94C();
s32 OutlineScreenStep(s32, s32, s32);
s32 PlayTimeStep();
void BtlMenuOutlineStep(void);  /* TU defines this below */
void BtlCellRowsClear(s32, s32, s32);
void BtlMenuCellsClear(s32 kind_mask);  /* TU defines this below */
void func_80048C94();  /* TU defines this below */
void BtlCellRowsUpdate();  /* TU defines this below */

/* func_8004888C */
void BtlMenuPulseSet(void *pulse, s32 mode, s32 layer_mask, s32 top)
{
  s16 *level_then_top;
  ((BtlMenuPulse *)pulse)->mask = layer_mask;
  ((BtlMenuPulse *)pulse)->mode = mode;
  ((BtlMenuPulse *)pulse)->down = 1;
  level_then_top = (s16 *)(s32 *) (((u8 *) pulse) + 0xc);
  *level_then_top = top;
  level_then_top = (s16 *) (((u8 *) pulse) + 0xe);
  *level_then_top = top;
}
/* func_800488A8 */
void BtlMenuPulseStep(void *pulse)
{
  u8 *ptr = (u8 *)pulse;

  if (*(s32 *)ptr != 0) {
    if (((BtlMenuPulse *)ptr)->down != 0) {
      ((BtlMenuPulse *)ptr)->level = (s16)(AS(u16, ((BtlMenuPulse *)ptr)->level) - 1);
      if (((BtlMenuPulse *)ptr)->level <= 0) {
        ((BtlMenuPulse *)ptr)->level = 0;
        ((BtlMenuPulse *)ptr)->down ^= 1;
      }
    } else {
      ((BtlMenuPulse *)ptr)->level = (s16)(AS(u16, ((BtlMenuPulse *)ptr)->level) + 1);
      if (((BtlMenuPulse *)ptr)->level >= ((BtlMenuPulse *)ptr)->top) {
        ((BtlMenuPulse *)ptr)->level = ((BtlMenuPulse *)ptr)->top;
        ((BtlMenuPulse *)ptr)->down ^= 1;
      }
    }
    if (*(s32 *)ptr == 1) {
      layer_mix_set(((BtlMenuPulse *)ptr)->mask, (((BtlMenuPulse *)ptr)->level << 1) & 0xFE);
    }
  }
}

s32 pad_record_update(s32 *, s32);                            /* extern */
void BtlAnimationStep();                                     /* extern */
s32 BtlFrameStep();                                  /* extern */
void BtlWindowsMoveStep(void);                             /* extern */
s32 SoundPlay(u8, s32);                             /* extern */

/* func_80048964 */
void BtlMenuFrameStep(void) {
    pad_record_update((s32 *)g_BtlMenuPadRecord, 0x10);
    pad_record_update((s32 *)g_MenuPadRecord, 0x20);
    if (g_BtlMenuSoundRequest != 0) {
        SoundPlay(g_BtlMenuSoundRequest, 3);
        g_BtlMenuSoundRequest = 0;
    }
    BtlAnimationStep(g_BtlMenuCopyRecords, 0x1C, 6);
    BtlAnimationStep(g_BtlMenuMarkRecords + 0x40, 0x6E, 3);
    BtlWindowsMoveStep();
    BtlFrameStep();
}

s32 func_8001B94C();                                  /* extern */
s32 OutlineScreenStep(s32, s32, s32);                       /* extern */
s32 PlayTimeStep();                                  /* extern */

/* func_800489F0 */
void BtlMenuOutlineStep(void) {
    s32 record_number;
    s32 next_record_number;

    func_8001B94C();
    next_record_number = 1;
    record_number = 1;
    do {
        next_record_number += 1;
        OutlineScreenStep(g_BtlMenuMarkRecords, record_number, 0);
        record_number = next_record_number;
    } while (next_record_number < 4);
    PlayTimeStep();
}
extern u8 g_BtlWindowMoves[];  /* field group base (target width u8; reabsorbed: pointer offsets) */

extern u8 D_800B8D34;
extern u8 D_800B8D35;
extern u8 D_800B8D38;
extern u8 D_800B8D45;

/* func_80048A3C */
void BtlWindowsMoveWait(void)
{
  if ((D_800B8D35 | (D_800B8D34 | (D_800B8D38 | (D_800B8D45 | (g_BtlWindowMoves[1] | g_BtlWindowMoves[29]))))) != 0)
  {
    {
      do
      {
        BtlMenuFrameStep();
      }
      while ((D_800B8D35 | (D_800B8D34 | (D_800B8D38 | (D_800B8D45 | (g_BtlWindowMoves[1] | g_BtlWindowMoves[29]))))) != 0);
    }
  }
  else
  {
    BtlMenuFrameStep();
  }
}
/* The slot mask both callers store in g_BtlMenuTargetMask, from the kind byte they
   keep in g_BtlMenuTargetMode. Kind 0 or 9: no slot. Low nibble 1: slots 6..13, a bit
   for each with g_BtlSlotIds[slot] set, a non-zero first u16 of its CharStats
   record and g_BtlSlotActiveFlags[slot] set. Any other kind: slots 0..4, a bit for each
   whose g_BtlSlotIds[slot] is neither 0 nor 0xC and that has g_BtlSlotActiveFlags[slot]
   set, both of 0x10/0x20 in its status byte (+0x15), or a zero first u16.
   CharStats is read inside each loop: loop.c then hoists the invariants in
   the order the body first uses them. One return serves both loops. */
/* func_80048AE8 */
u16 BtlMenuTargetMaskGet(u8 arg)
{
    u16 i;
    u16 mask;

    mask = 0;
    if (arg == 0 || arg == 9) {
        return 0;
    }
    if ((arg & 0xF) == 1) {
        for (i = 6; i < 14; i++) {
            if (g_BtlSlotIds[i] != 0 && *(u16 *)(i * 0x3C + CharStats) != 0 && g_BtlSlotActiveFlags[i] != 0) {
                mask |= 1 << i;
            }
        }
    } else {
        for (i = 0; i < 5; i++) {
            if (g_BtlSlotIds[i] != 0 && g_BtlSlotIds[i] != 0xC
                && (g_BtlSlotActiveFlags[i] != 0
                    || (*(u8 *)(i * 0x3C + CharStats + 0x15) & 0x30) == 0x30
                    || *(u16 *)(i * 0x3C + CharStats) == 0)) {
                mask |= 1 << i;
            }
        }
    }
    return mask;
}

void BtlCellRowsClear(s32, s32, s32);                           /* extern */

/* func_80048C44 */
void BtlMenuCellsClear(s32 kind_mask) {
    if (kind_mask & 0x10) {
        BtlCellRowsClear(0, 0x20, 0x10);
    }
    if (kind_mask & 0x20) {
        BtlCellRowsClear(0, 0x14, 0x20);
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its callers pass the face just put on the grid, which does not say what for. */
void func_80048C94() {

}
/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; named from what its callers pass: the rows of the cell grid they have just changed. */
/* func_80048C9C */
void BtlCellRowsUpdate() {

}

s32 font_string_put(s32, s32, s32);                         /* extern */
s32 font_target_clear(s32, s32);                          /* extern */
s32 font_col_set(s32);                               /* extern */
extern s32 g_BtlMenuTextBlocks; /* far_syms: g_BtlMenuTextBlocks */

/* func_80048CA4 */
s32 BtlMenuTextStringPut(s32 first_block, s32 string_id, s32 col, s32 block_count) {
    font_target_clear(g_BtlMenuTextBlocks + (first_block << 7), block_count & 0xFFFF);
    font_col_set(col & 0xFF);
    font_string_put(string_id & 0xFFFF, 0, 1);
}

/* func_80048D00 */
s32 BtlMenuTextImageLoad(s32 first_block, s32 place, s32 block_count) {
    return font_image_load(g_BtlMenuTextBlocks + (first_block << 7));
}
/* Moves a cursor over a grid of `cols` x `rows` 12-byte cells at menu_items
   (byte 0 = occupied) by the pad's d-pad bits at +4 (0x8000 left, 0x2000
   right, 0x1000 up, else down) and returns the new index (u8). Left/right
   step within the row (wrapping) and skip empty cells, wrapping once more
   past the row end; up/down step a row (wrapping top/bottom) and then search
   left along that row, falling back to `cur` if the row's first column is
   reached. The do-while(0) arms and the block-scoped `last`/`last2` are the
   original's macro shape: the loop notes stop CSE carrying the button load
   into the next test (each test reloads +4), and each `cols - 1` is its own
   pseudo ($a3 / $a0). */
/* func_80048D2C */
s32 BtlMenuCursorMove(u8 *pad, s32 menu_items, s32 column_count, s32 row_count, s32 cur)
{
    u32 cols = column_count;
    u32 rows = row_count;
    u32 t;
    u32 x;
    u8 *p;
    u8 *q;
    u8 *cell;
    u8 v;

    if (((PadRecord *)pad)->repeated & 0xF000) {
        t = cur;
        if (((PadRecord *)pad)->repeated & 0x8000) {
          /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
             the block runs once, so behaviour is the same without the `do`
             / `while (0)`, but then 78 of this function's 435 instruction lines differ. */
          do {
            if (cur % cols != 0) {
                t = cur - 1;
            } else {
                x = cur - 1;
                t = x + cols;
            }
            cell = (u8 *)(t * 12 + menu_items);
            if (*cell == 0) {
                p = cell;
            lloop1:
                if (t % cols != 0) {
                    p -= 12;
                    t--;
                    if (*p == 0) goto lloop1;
                }
                if (*(u8 *)(t * 12 + menu_items) == 0 && t % cols == 0) {
                    x = t - 1;
                    t = x + cols;
                    q = (u8 *)(t * 12 + menu_items);
                    v = *q;
                loop:
                    if (v == 0) {
                        q -= 12;
                        if (t % cols != 0) {
                            v = *q;
                            t--;
                            goto loop;
                        }
                    }
                }
            }
          } while (0);
        } else if (((PadRecord *)pad)->repeated & 0x2000) {
          /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source:
             the block runs once, so behaviour is the same without the `do`
             / `while (0)`, but then 26 of this function's 435 instruction lines differ. */
          do {
            if (t % cols != cols - 1) {
                t = t + 1;
            } else {
                x = t + 1;
                t = x - cols;
            }
            cell = (u8 *)(t * 12 + menu_items);
            if (*cell == 0) {
                u32 last = cols - 1;
                p = cell;
            rloop1:
                if (t % cols < last) {
                    p += 12;
                    t++;
                    if (*p == 0) goto rloop1;
                }
                if (*(u8 *)(t * 12 + menu_items) == 0 && t % cols == last) {
                    x = t + 1;
                    t = x - cols;
                    q = (u8 *)(t * 12 + menu_items);
                    if (*q == 0) {
                        u32 last2 = cols - 1;
                    rloop2:
                        if (t % cols < last2) {
                            q += 12;
                            t++;
                            if (*q != 0) goto done;
                            goto rloop2;
                        }
                    }
                }
            }
          } while (0);
        } else if (((PadRecord *)pad)->repeated & 0x1000) {
          if (t / cols != 0) {
              t -= cols;
              cell = (u8 *)(t * 12 + menu_items);
              if (*cell == 0) {
                  p = cell;
              uloop2:
                  if (t % cols != 0) {
                      p -= 12;
                      t--;
                      if (*p == 0) goto uloop2;
                  }
                  if (*(u8 *)(t * 12 + menu_items) == 0 && t % cols == 0) {
                      x = t - 1;
                      t = x + cols;
                      q = (u8 *)(t * 12 + menu_items);
                      if (*q == 0) {
                      vloop2:
                          if (t % cols != 0) {
                              q -= 12;
                              t--;
                              if (*q == 0) goto vloop2;
                          }
                      }
                      if (t % cols == 0) t = cur;
                  }
              }
          } else {
              t += cols * (rows - 1);
              cell = (u8 *)(t * 12 + menu_items);
              if (*cell == 0) {
                  p = cell;
              uloop3:
                  if (t % cols != 0) {
                      p -= 12;
                      t--;
                      if (*p == 0) goto uloop3;
                  }
                  if (*(u8 *)(t * 12 + menu_items) == 0 && t % cols == 0) {
                      x = t - 1;
                      t = x + cols;
                      q = (u8 *)(t * 12 + menu_items);
                      if (*q == 0) {
                      vloop3:
                          if (t % cols != 0) {
                              q -= 12;
                              t--;
                              if (*q == 0) goto vloop3;
                          }
                      }
                      if (t % cols == 0) t = cur;
                  }
              }
          }
        } else {
          if (t / cols < rows - 1) {
              t += cols;
              cell = (u8 *)(t * 12 + menu_items);
              if (*cell == 0) {
                  p = cell;
              uloop4:
                  if (t % cols != 0) {
                      p -= 12;
                      t--;
                      if (*p == 0) goto uloop4;
                  }
                  if (*(u8 *)(t * 12 + menu_items) == 0 && t % cols == 0) {
                      x = t - 1;
                      t = x + cols;
                      q = (u8 *)(t * 12 + menu_items);
                      if (*q == 0) {
                      vloop4:
                          if (t % cols != 0) {
                              q -= 12;
                              t--;
                              if (*q == 0) goto vloop4;
                          }
                      }
                      if (t % cols == 0) t = cur;
                  }
              }
          } else {
              t %= cols;
              cell = (u8 *)(t * 12 + menu_items);
              if (*cell == 0) {
                  p = cell;
              uloop5:
                  if (t % cols != 0) {
                      p -= 12;
                      t--;
                      if (*p == 0) goto uloop5;
                  }
                  if (*(u8 *)(t * 12 + menu_items) == 0 && t % cols == 0) {
                      x = t - 1;
                      t = x + cols;
                      q = (u8 *)(t * 12 + menu_items);
                      if (*q == 0) {
                      vloop5:
                          if (t % cols != 0) {
                              q -= 12;
                              t--;
                              if (*q == 0) goto vloop5;
                          }
                      }
                      if (t % cols == 0) t = cur;
                  }
              }
          }
        }
done:
        return (u8)t;
    }
    return (u8)cur;
}

/* One 18-byte row of the item table (records.items): g_BtlItemTable[id - 1]. */
extern BtlItemRow g_BtlItemTable[];

/* Can the character whose 0x3C-byte record is at `c` use command `cmd` now
   (the battle command grid and the spell / item lists ask)? Returns 0 or 1.
   0, 1: always. 2, 5, 6: unless the status byte +0x15 has a 0x4C bit or
   both 0x30 bits. 3 (magic): the status must also be clear of 0x80; `flag`
   then answers 1 at once; otherwise the spell id = item + 0x4A must not be
   blocked (status 0x20 and the row's blocked_by_status_20), must be in the
   eight-slot list at +0x34, and the MP word at +4 must cover the row's mp,
   halved rounding up when BtlTargetTraitCheck(0, 0x1B, 0, 0) holds for the actor
   g_BtlTargetSlot = g_BtlActingSlot. 4 (item): StoryBitCheck(0x3D9) must be 0;
   `flag` then answers 1; otherwise the item must be in the six-slot list at
   +0x22 or the one at +0x2E and its row's `usable` set. Cases 3 and 4 then
   store g_BtlMenuTargetMode = the row's target_mode & 0xF and g_BtlMenuTargetMask =
   BtlMenuTargetMaskGet(it), the mask of slots it may be aimed at: usable when the
   mode is 0 or that mask is non-zero. A command of 7 or more hangs (the
   default is a self-jump). */
/* func_800493F8 */
s32 BtlMenuActionCheck(s32 c, u8 cmd, u8 item, s32 flag)
{
  u32 i;
  s32 cost;
  u8 *p;
  switch (cmd)
  {
    case 0:
    case 1:
      return 1;
    case 2:
    case 5:
    case 6:
      if ((((u8 *) c)[0x15] & 0x4C) || (((u8 *) c)[0x15] & 0x30) == 0x30)
      {
        return 0;
      }
      return 1;
    case 3:
      if ((((u8 *) c)[0x15] & 0xCC) || (((u8 *) c)[0x15] & 0x30) == 0x30)
      {
        return 0;
      }
      if (flag & 0xFF)
      {
        return 1;
      }
      item += 0x4A;
      if (((u8 *) c)[0x15] & 0x20)
      {
        if (g_BtlSpellRows[item - 1].blocked_by_status_20)
        {
          return 0;
        }
      }
      i = 0;
      p = (u8 *) c + 0x34;
      for (; i < 8; i++)
      {
        if (p[i] == item)
        {
          break;
        }
      }
      g_BtlTargetSlot = g_BtlActingSlot;
      if (BtlTargetTraitCheck(0, 0x1B, 0, 0))
      {
        cost = (u32) (g_BtlSpellRows[item - 1].cost + 1) >> 1;
      }
      else
      {
        cost = g_BtlSpellRows[item - 1].cost;
      }
      if ((i >= 8) || (((BtlStats *)c)->mp < cost))
      {
        return 0;
      }
      g_BtlMenuTargetMode = g_BtlSpellRows[item - 1].target_modes & 0xF;
      g_BtlMenuTargetMask = BtlMenuTargetMaskGet(g_BtlMenuTargetMode);
      if ((g_BtlMenuTargetMode == 0) || (g_BtlMenuTargetMask != 0))
      {
        return 1;
      }
      return 0;
    case 4:
      if ((((u8 *) c)[0x15] & 0x4C) || (((u8 *) c)[0x15] & 0x30) == 0x30 || StoryBitCheck(0x3D9))
      {
        return 0;
      }
      if (flag & 0xFF)
      {
        return 1;
      }
      i = 0;
      p = (u8 *) c + 0x22;
      for (; i < 6; i++)
      {
        if (p[i] == item)
        {
          break;
        }
      }
      if (i >= 6)
      {
        i = 0;
        p = (u8 *) c + 0x2E;
        for (; i < 6; i++)
        {
          if (p[i] == item)
          {
            break;
          }
        }
        if (i >= 6)
        {
          return 0;
        }
      }
      if (g_BtlItemTable[item - 1].function_number == 0)
      {
        return 0;
      }
      g_BtlMenuTargetMode = g_BtlItemTable[item - 1].target_modes & 0xF;
      g_BtlMenuTargetMask = BtlMenuTargetMaskGet(g_BtlMenuTargetMode);
      if ((g_BtlMenuTargetMode == 0) || (g_BtlMenuTargetMask != 0))
      {
        return 1;
      }
      return 0;
    default:
      for (;;)
      {
      }
  }
  return 1;
}
