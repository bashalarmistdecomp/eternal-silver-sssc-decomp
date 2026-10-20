/* Lunar SSSC — btl_menu: the battle's menu: its run for a round, set-up,
   reset, free, open and close, the first menu (steps 1 and 2) and the
   preset rows' display and actions. The first split had it in rendering3.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
#include "menu.h"
extern u8 g_BtlActionKind;
extern u16 g_BtlMenuCursor;
extern s8 g_BtlMenuSoundRequest;
extern u8 g_BtlPartyMode;
extern s32 g_BtlMenuCopyRecords;
extern u8 g_BtlSlotPlannedNumbers[];
extern u8 g_BtlMenuStepFunctions[];
extern u8 g_BtlMenuStep;
extern u8 g_BtlMenuPendingMask;
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlActingSlot;
s32 pad_record_init(s32 *, s32, s32, s32);
/* far_syms: g_BtlActingSlot */
extern u8 g_BtlSlotActiveFlags[];
extern u8 g_BtlSlotIds[];
extern u8 g_BtlSlotActiveFlags[];
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlActingSlot;
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
s32 layer_col_set();
s32 layer_mix_set(s32, s32);
s32 sprite_run_off(s32, s32);
void BtlMenuRun(void);
void BtlMenuReset(void);
/* far_syms: g_BtlSlotPlannedTargets (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotPlannedNumbers (target reaches it %hi/%lo) */
s32 pad_record_init(s32 *, s32, s32, s32);
s32 layer_col_set();
s32 layer_mix_set(s32, s32);
s32 pad_record_init(s32 *, s32, s32, s32);
s32 sprite_run_off(s32, s32);

/* far_syms: g_BtlMenuStepFunctions (target reaches it %hi/%lo) */

/* func_80044118 */
void BtlMenuRun(void) {
    BtlWindowBoxesOff();
    BtlMenuReset();
    if (g_BtlMenuPendingMask != 0) {
        BtlMenuOpen();
        if (g_BtlMenuStep != 0) {
            do {
                ((void (**)(void))g_BtlMenuStepFunctions)[g_BtlMenuStep]();
                BtlMenuFrameStep();
            } while (g_BtlMenuStep != 0);
        }
        BtlMenuClose();
    }
    BtlWindowBoxesOffCopy();
}
/* far_syms: g_BtlShowOpen */
/* far_syms: g_BtlShowRecords */
/* far_syms: g_BtlMenuItems */
/* far_syms: g_BtlMenuItemCount */
/* far_syms: D_800B8EEC */
/* far_syms: g_BtlMenuTextBlocks */
extern s32 g_BtlShowOpen;
extern s32 g_BtlShowRecords;
extern s16 *g_BtlMenuMarkRecords;
extern s32 g_BtlMenuPulse;
extern s32 g_BtlMenuItems;
extern u8 g_BtlMenuItemCount;
extern u8 g_SpriteData2[];
extern u8 g_SpriteData0[];
extern u8 g_BtlMixFlash[];
extern s32 D_800B8EEC;
extern u8 D_800E4E70[];
extern s32 D_800E4768[];
extern s32 g_BtlMenuTextBlocks;
s32 xmalloc_heap_alloc(s32);
s32 layer_mix_switch_off(s32);
void font_target_set(u8 *);

/* func_800441B4 */
void BtlMenuInit(void)
{
    s32 p;
    s16 *q;
    u8 *e;
    u16 *r;
    MenuScreenBits *m;
    u16 i;

    layer_order_set(0x204, 1);
    layer_order_set(0x508, 2);
    layer_order_set(0x30, 3);
    i = 0;
    BtlWindowsInit();
    g_BtlShowOpen = 0;
    g_BtlShowRecords = 0;
    p = xmalloc_heap_alloc(0x240);
    g_BtlMenuCopyRecords = p;
    g_BtlMenuMarkRecords = (s16 *) (p + 0x140);
    g_BtlMenuPulse = xmalloc_heap_alloc(0x10);
    g_BtlMenuItems = xmalloc_heap_alloc(0x180);
    for (; i < 5; i++)
    {
        AS(s16, ((BtlRecord *) g_BtlMenuCopyRecords)[i].on) = 0;
    }
    BtlRecordSpriteDataSet(g_BtlMenuMarkRecords, g_SpriteData2, 0x730, 6);
    BtlRecordSpriteDataSet(g_BtlMenuMarkRecords + 0x20, g_SpriteData0, 0x760, 2);
    BtlRecordSpriteDataSet(g_BtlMenuMarkRecords + 0x40, g_SpriteData0, 0x760, 0xA);
    BtlRecordSpriteDataSet(g_BtlMenuMarkRecords + 0x60, g_SpriteData0, 0x760, 0xB);
    r = (u16 *) g_BtlMenuMarkRecords;
    r[0x24] = r[0x44] = r[0x64] = 0xFFF;
    g_BtlMenuItemCount = 0;
    r[6] = r[0x26] = r[0x46] = r[0x66] = 0;
    r[0x27] &= 0x9FFF;
    r[0x47] &= 0x9FFF;
    r[0x67] &= 0x9FFF;
    for (; i < 0x20; i++)
    {
        e = (u8 *) (i * 12 + g_BtlMenuItems);
        *(u8 *) e = ((BtlMenuItem *)e)->x = ((BtlMenuItem *)e)->y = AS(s16, ((BtlMenuItem *)e)->icon) = ((BtlMenuItem *)e)->attr = 0;
    }
    g_BtlMixFlash[8] = g_BtlMixFlash[9] = 8;
    g_BtlMixFlash[0] = g_BtlMixFlash[1] = 0;
    D_800B8EEC |= 1;
    m = (MenuScreenBits *)D_800E4E70;
    m->unk_0e &= 0xFFF3;
    layer_mix_switch_off(8);
    BtlCellRowsClear(0, 0x20, 0x30);
    D_800E4768[1] = 0x80320000;
    D_800E4768[3] = 0x2000;
    D_800E4768[2] = 0;
    D_800E4768[0] = 1;
    D_800E4768[1] = 0x80300000;
    D_800E4768[2] = 0;
    D_800E4768[3] = 0x1000;
    D_800E4768[0] = 1;
    g_BtlMenuTextBlocks = 0x801CBB00;
    font_target_set((u8 *) 0x801CBB00);
}

/* far_syms: g_BtlPartyMode (target reaches it %hi/%lo) */
/* far_syms: g_BtlPartyMode (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_80044430 */
void BtlMenuReset(void)
{
  u16 i;
  s16 *ptr;
  g_BtlMenuSoundRequest = 0;
  g_BtlPartyMode = 0;
  g_BtlMenuCursor = 0;
  g_BtlMenuPendingMask = 0;
  g_BtlActingSlot = 0;
  g_BtlMenuStep = 1;
  i = 0;
  do
  {
    g_BtlSlotPlannedKinds[i] = 0xB;
    *(i + g_BtlSlotPlannedNumbers) = (g_BtlSlotPlannedTargets[i] = 0);
    ptr = (s16 *) ((i << 6) + g_BtlMenuCopyRecords);
    i++;
    *ptr = 0;
  }
  while (i < 5);
  g_BtlMenuPendingMask = BtlMenuPendingMaskGet();
}
/* far_syms: g_BtlMenuItems (target reaches it %hi/%lo) */
extern u8 D_800E4E70[];
s32 layer_mix_switch_off(s32);
s32 xmalloc_heap_free(s32);
s32 BtlMenuCellsClear(s32);
s32 BtlCellRowsUpdate(s32, s32, s32, s32);
extern s32 g_BtlMenuPulse;
extern s32 g_BtlMenuItems;
extern s32 D_800B8EEC;
extern s32 g_MenuPadRecord;

static inline s16 *inline_fn_func_800444D0(MenuScreenBits *ui)
{
  return (s16 *) &ui->unk_0e;
}
/* The two words of D_800E4E70 (MenuScreenBits' unk_0e and unk_0a) are
   signed pointer accesses through their members' addresses: as member
   stores the first of the two stores to D_800B8EEC is dropped (8 bytes
   shorter); with unk_0a alone a member store, its load and store move
   above unk_0e's; without the inline the base register is formed at +0xE. */
/* func_800444D0 */
void BtlMenuFree(void)
{
  s16 *ui_word;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `keep_mask` only holds the constant 0xFFCF; behaviour is the same with the constant
     written where it is read, but then 9 of this function's 53 instruction lines differ. */
  int keep_mask;
  s32 new_value;
  BtlMenuCellsClear(0x30);
  BtlCellRowsUpdate(0, 0, 0x1C, 0x30);
  {
    layer_mix_switch_off(0x100);
    layer_mix_switch_off(4);
    layer_mix_switch_off(8);
    layer_mix_switch_off(0x10);
    layer_mix_set(0x10, 0xFF);
  }
  new_value = D_800B8EEC | 1;
  D_800B8EEC = new_value;
  keep_mask = 0xFFCF;
  ui_word = inline_fn_func_800444D0((MenuScreenBits *) D_800E4E70);
  *ui_word = (u16) ((*inline_fn_func_800444D0((MenuScreenBits *) D_800E4E70)) & keep_mask);
  D_800B8EEC = new_value;
  *(s16 *) &((MenuScreenBits *) D_800E4E70)->unk_0a = (u16) ((*(s16 *) &((MenuScreenBits *) D_800E4E70)->unk_0a) & 0xFFCF);
  pad_record_init((s32) (&g_MenuPadRecord), 0x10, 0, 0);
  xmalloc_heap_free(g_BtlMenuItems);
  xmalloc_heap_free(g_BtlMenuPulse);
  xmalloc_heap_free(g_BtlMenuCopyRecords);
  sprite_run_off(0, 0x96);
}

/* far_syms: g_BtlMenuTextBlocks (target reaches it %hi/%lo) */
/* far_syms: g_PadHeldButtons (target reaches it %hi/%lo) */
/* far_syms: g_PadSecondHeldButtons (target reaches it %hi/%lo) */
/* far_syms: D_800B8EEC (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuPadRecord (target reaches it %hi/%lo) */
/* far_syms: g_MenuPadRecord (target reaches it %hi/%lo) */
/* far_syms: D_800E4E70 (target reaches it %hi/%lo) */
extern u8 D_800E4E70[];
s32 xmalloc_heap_alloc(s32);
s32 font_target_clear(s32, s32);
s32 BtlMenuTextImageLoad(s32, s32, s32);
s32 func_80049AC4();
s32 BtlMixColNumberSet(s32);
extern s32 g_BtlMenuTextBlocks;
extern s16 g_PadHeldButtons;
extern s16 g_PadSecondHeldButtons;
extern s32 g_LinePaths;
extern s32 D_800B8EEC;
extern u16 g_BtlMenuPadRecord[];
extern s32 g_MenuPadRecord;

/* The two words of D_800E4E70 (MenuScreenBits' unk_0e and unk_0a) are
   signed pointer accesses through their members' addresses
   (*(s16 *)&ui->unk_0e): as member accesses (plain, or AS(s16, ...) on
   the loads or on both), or through u16 pointers, the first of the two
   stores to D_800B8EEC is dropped and the function shrinks by 8 bytes. */
/* func_800445B8 */
void BtlMenuOpen(void)
{
  unsigned int keep_mask_then_block_count;
  int new_value_then_ui_word;
  s32 first_block;
  s32 new_value_then_step;
  MenuScreenBits *ui;
  u32 i;
  func_80049AC4();
  BtlMixColNumberSet(0xE1);
  new_value_then_ui_word = D_800B8EEC | 1;
  D_800B8EEC = (new_value_then_step = new_value_then_ui_word);
  ui = (MenuScreenBits *) (&D_800E4E70);
  *(s16 *) &ui->unk_0e = (new_value_then_ui_word = (u16) (((*(s16 *) &ui->unk_0e) & (keep_mask_then_block_count = 0xFFCF)) | 0x10));
  D_800B8EEC = new_value_then_step;
  keep_mask_then_block_count = 0x40;
  *(s16 *) &ui->unk_0a = (u16) (((*(s16 *) &ui->unk_0a) & 0xFFCF) | 0x10);
  layer_col_set(-1, 0x13C, 0, new_value_then_step);
  if (1)
  {
    g_PadHeldButtons = 0;
    i = (g_PadSecondHeldButtons = 0);
    pad_record_init(&g_BtlMenuPadRecord, 0x10, keep_mask_then_block_count, 0);
 pad_record_init(&g_MenuPadRecord, 0x20, 0x40, 0);  
    font_target_clear(*(&g_BtlMenuTextBlocks), keep_mask_then_block_count);
  }
  first_block = 0x10;
  first_block = 0 * first_block;
  do
  {
    first_block = (4 * i) * 4;
    BtlMenuTextImageLoad(first_block, first_block, 0x10);
    new_value_then_step = 1;
    if (1)
    {
      i += new_value_then_step;
    }
  }
  while (i < 4U);
  g_LinePaths = xmalloc_heap_alloc(0x380);
}

s32 xmalloc_heap_free(s32);
s32 BtlMenuFrameStep(u32);
s32 BtlWindowsMoveWait();
s32 BtlMenuCellsClear(s32);
s32 BtlCellRowsUpdate(s32, s32, s32, s32);
s32 func_80049B20();
s32 BtlWindow0Set(s32, s32, s32, s32);
extern s32 g_LinePaths;
extern s16 *g_BtlMenuMarkRecords;

/* func_800446B4 */
void BtlMenuClose(void)
{
  u32 i;
  u32 j;
  u8 *mark_then_copy_record;
  s16 *mark_record;
  i = 0;
  mark_record = g_BtlMenuMarkRecords;
  do
  {
    {
      i += 1;
      *(mark_record = (mark_then_copy_record = mark_record)) = 0;
    }
  }
  while (i < 4U);
  BtlMenuCellsClear(0x30);
  BtlCellRowsUpdate(0, 0, 0x1C, 0x30);
  BtlWindow0Set(2, 1, 0, 0);
  BtlWindowsMoveWait();
  func_80049B20();
  layer_col_set(-1, 0x8013D, 0);
  layer_mix_set(0x80F35, 0xFF);
  j = 0;
  mark_then_copy_record = g_BtlMenuCopyRecords;
  do
  {
    ((BtlRecord *)mark_then_copy_record)->sprite.on = 0;
    (*(u16 *)mark_then_copy_record) = 0;
    mark_then_copy_record += 0x40;
    j += 1;
  }
  while (j <= (5U - 1));
  BtlMenuFrameStep(j);
  xmalloc_heap_free(g_LinePaths);
}
/* far_syms: g_BtlActionKind */
/* far_syms: g_BtlPartyMode */
/* far_syms: g_BtlSlotPlannedKinds */
/* far_syms: g_BtlSlotIds */
/* far_syms: g_BtlSlotActiveFlags */
extern u8 g_BtlMenuKeptCursors;
void memset(u8 *, s32, s32);
s32 BtlMenuPendingMaskGet();
s32 BtlSlotActionAnimationSet(u8);
s32 BtlMenuTextCellsSet(s32, s32, s32, s32, s32, s32);
s32 BtlMenuTextStringPut(s32, u16, u8, s32);
s32 BtlCellGlyphSet(s32, s32, s32, s32, s32, s32);
s32 CellNumberSet(s32, s32, s32, s32, s32, s32, s32, s32);
s32 BtlMenuIconCellsSet(s32, s32);

/* func_80044780 */
void BtlMenuFirstOpen(void)
{
    u8 ids[4];
    u8 counts[4];
    u16 i;
    u16 j;
    s32 k;

    memset(ids, 0, 4);
    memset(counts, 0, 4);
    g_BtlMenuStep = 2;
    g_BtlMenuPendingMask = BtlMenuPendingMaskGet();
    for (i = 0; i < 5; i++) {
        if ((g_BtlMenuPendingMask >> i) & 1) {
            g_BtlActingSlot = i;
            g_BtlSlotPlannedKinds[g_BtlActingSlot] = g_BtlActionKind = 0xB;
            BtlSlotActionAnimationSet(g_BtlActingSlot);
        }
    }
    g_BtlActingSlot = 0;
    g_BtlPartyMode = 0;
    g_BtlMenuCursor = g_BtlMenuKeptCursors;
    BtlWindow0Set(2, 1, 0x24, 0xA);
    for (i = 0; i < 8; i++) {
        k = i + 6;
        if (g_BtlSlotIds[k] == 0 || g_BtlSlotActiveFlags[k] == 0) {
            continue;
        }
        for (j = 0; j < 4; j++) {
            if (ids[j] != g_BtlSlotIds[k]) {
                if (ids[j] != 0) {
                    continue;
                }
                ids[j] = g_BtlSlotIds[k];
            }
            counts[j]++;
            j = 4;
        }
    }
    g_BtlMenuItemCount = 4;
    j = 0;
    do {
        ((BtlMenuItem *)g_BtlMenuItems)[j].on = 1;
        ((BtlMenuItem *)g_BtlMenuItems)[j].disabled = 0;
        ((BtlMenuItem *)g_BtlMenuItems)[j].x = (j % 2) * 5 + 3;
        ((BtlMenuItem *)g_BtlMenuItems)[j].y = (j / 2) * 5 + 2;
        ((BtlMenuItem *)g_BtlMenuItems)[j].attr = 0x200000;
        j++;
    } while (j < g_BtlMenuItemCount);
    ((BtlMenuItem *)g_BtlMenuItems)[0].icon = 0xED;
    ((BtlMenuItem *)g_BtlMenuItems)[1].icon = 0xF0;
    ((BtlMenuItem *)g_BtlMenuItems)[2].icon = 0xEF;
    ((BtlMenuItem *)g_BtlMenuItems)[3].icon = 0xEE;
    BtlMenuTextCellsSet(3, 5, 5, 1, 0x80300000, 0);
    BtlMenuTextCellsSet(0xD, 2, 8, 4, 0x80300000, 5);
    BtlMenuTextStringPut(0, ((BtlMenuItem *)g_BtlMenuItems)[g_BtlMenuCursor].icon + 0x100, 0, 5);
    BtlMenuTextImageLoad(0, 0, 5);
    for (j = 0; j < 4; j++) {
        if (ids[j] == 0) {
            break;
        }
        BtlMenuTextStringPut(j * 8 + 5, ids[j] + 0x80, 0, 8);
        BtlCellGlyphSet(0x9C, 0x20, j * 2 + 2, 1, 0x3000, 1);
        CellNumberSet(counts[j], 0x22, j * 2 + 2, 1, 0x80300000, 0x3000, 1, 0);
        BtlMenuTextImageLoad(j * 8 + 5, j * 8 + 5, 8);
    }
    BtlCellRowsUpdate(2, 0x80, 8, 0x20);
    BtlMenuIconCellsSet(1, 8);
    g_BtlMenuMarkRecords[0x40] = 1;
    BtlWindowsMoveWait();
}
/* far_syms: g_BtlPartyMode g_BtlMenuItems */
extern u16 g_MenuPadRecordRepeated[];

/* UI state 2: the main menu's input (BtlMenuFirstOpen, state 1, opens it;
   menu-ui.md "The battle command windows"). The cursor g_BtlMenuCursor is 0..3 on
   the 2 x 2 grid of 12-byte cells at *g_BtlMenuItems: AI, Command, Tactics, Run.

   CROSS (the press word g_MenuPadRecordRepeated[0] == 0x40): sound request 2
   (g_BtlMenuSoundRequest), the grid's reset BtlMenuClear, then by the cursor:
     0  AI       g_BtlPartyMode = 1, state g_BtlMenuStep = 0, BtlPartyActionSet(1)
     1  Command  g_BtlPartyMode = 0, state 5
     2  Tactics  g_BtlPartyMode = 0, state 3
     3  Run      g_BtlPartyMode = 2, state 0
   Any other cursor value leaves both alone.

   Otherwise a direction (the repeat word g_BtlMenuPadRecord[2] & 0xF000): sound
   request 1 and the cursor moves through BtlMenuCursorMove(pad record, cells,
   2, 2, cursor), whose low byte is the new cursor. When that differs from
   the one last drawn (g_BtlMenuKeptCursors) it is remembered and the top label is
   redrawn: BtlMenuTextStringPut(0, the cell's icon (+6) + 0x100, 0, 5),
   BtlMenuTextImageLoad(0, 0, 5), BtlMenuIconCellsSet(1, 8). A direction that does not
   move the cursor still asks for the sound.

   No RNG draw and no frame wait of its own.

   Shape   - MARKED SPELLING: the cell table is read through a pointer to
     g_BtlMenuItems, both times. The pointer's name and type are ours; that
     the call's argument was read through a pointer variable is what the
     bytes need. Measured on this text: the global written at both uses
     89/92; a local holding the table's address (the global's value) 77
     set at the top, 79 set in the else arm; the pointer for the argument
     alone, the index read plain, 92/92 as well.
     A call argument that is a register-addressed load is expanded
     straight into its argument register; a symbol-addressed one is
     "expensive" to calls.c and goes through a pseudo first (cse gives the
     load its symbol back either way). Loaded straight into $a1 the load
     is not the birth of a single-set pseudo, so sched1 does not hold it
     against the call: the order becomes `lw $a1`, the cursor's `lhu`, the
     request's `li` / `sb`, the stack store, and the cursor, alive across
     the request, takes $v1. With `g_BtlMenuItems` written as the argument:
     89/92 (the request first, then `lhu $v0`).
.
   - g_BtlMenuCursor is u16 (every load of it is `lhu`); no cast at its uses.
   - The pad record is the unsized u16 array the other TUs declare: the
     address is taken once (`lui $v0` / `addiu $a0, $v0`) for the repeat
     word's test and stays in $a0 as the call's first argument. */
/* func_80044B5C */
void BtlMenuFirstStep(void)
{
    BtlMenuItem **menu = (BtlMenuItem **)&g_BtlMenuItems;

    if (g_MenuPadRecordRepeated[0] == 0x40) {
        g_BtlMenuSoundRequest = 2;
        BtlMenuClear();
        switch (g_BtlMenuCursor) {
        case 0:
            g_BtlPartyMode = 1;
            g_BtlMenuStep = 0;
            BtlPartyActionSet(1);
            break;
        case 1:
            g_BtlPartyMode = 0;
            g_BtlMenuStep = 5;
            break;
        case 2:
            g_BtlPartyMode = 0;
            g_BtlMenuStep = 3;
            break;
        case 3:
            g_BtlPartyMode = 2;
            g_BtlMenuStep = 0;
            break;
        }
    } else if (g_BtlMenuPadRecord[2] & 0xF000) {
        g_BtlMenuSoundRequest = 1;
        g_BtlMenuCursor = BtlMenuCursorMove(g_BtlMenuPadRecord, *menu, 2, 2, g_BtlMenuCursor) & 0xFF;
        if (g_BtlMenuKeptCursors != g_BtlMenuCursor) {
            g_BtlMenuKeptCursors = g_BtlMenuCursor;
            BtlMenuTextStringPut(0, (*menu)[g_BtlMenuCursor].icon + 0x100, 0, 5);
            BtlMenuTextImageLoad(0, 0, 5);
            BtlMenuIconCellsSet(1, 8);
        }
    }
}

extern BtlMenuPresetCell *g_BtlMenuPresetCells;

/* The Tactics menu's redraw for the item under the cursor (g_BtlMenuCursor,
   0..3: the four items BtlMenuPresetOpen puts in menu records 0..3).
   BtlMenuPresetOpen calls it once the menu is set up and BtlMenuPresetStep when
   the cursor has moved to another item.
     - label slot 0, five cells: the cursor item's name, string
       g_BtlMenuItems[cursor].f6 + 0x100 in style 0 (BtlMenuTextStringPut), then
       BtlMenuTextImageLoad(0, 0, 5);
     - for each listed member i = 0 .. g_BtlMenuItemCount - 5 (g_BtlMenuItemCount is the
       member count + 4): its cell of the Tactics grid,
       g_BtlMenuPresetCells[cursor * 5 + i]. Label slot i * 8 + 5, eight cells, gets
       string cell.f0 + 0x100 in style cell.f2 (BtlMenuPresetOpen stores 0, or
       4 for an action the member cannot take); menu record 4 + i's icon
       (+6) becomes cell.f0; then BtlMenuTextImageLoad(i * 8 + 5, i * 8 + 5, 8);
     - BtlMenuIconCellsSet(1, 0xF), the items' draw.
   No frame wait, no RNG draw, no sound.
   Shape:
   - `i` is u16: the raw counter stays in one saved register and its
     masked copy in another (s32: 68/86, u8: 83/86).
   - The cell is indexed again at each of its three uses; the target
     rebuilds its address after the first call.
   - No cast anywhere: g_BtlMenuCursor is u16 (every load of it is `lhu`), and
     the style is the 16-bit f2 handed to BtlMenuTextStringPut's u8 parameter,
     which is the target's `lbu` of that field with no mask. */
/* func_80044CCC */
void BtlMenuPresetRowDisp(void)
{
    u16 i;

    BtlMenuTextStringPut(0, ((BtlMenuItem *)g_BtlMenuItems)[g_BtlMenuCursor].icon + 0x100, 0, 5);
    BtlMenuTextImageLoad(0, 0, 5);
    for (i = 0; i < g_BtlMenuItemCount - 4; i++) {
        BtlMenuTextStringPut(i * 8 + 5, AS(s16, g_BtlMenuPresetCells[g_BtlMenuCursor * 5 + i].icon) + 0x100,
                      AS(s16, g_BtlMenuPresetCells[g_BtlMenuCursor * 5 + i].colour), 8);
        ((BtlMenuItem *)g_BtlMenuItems)[i + 4].icon = AS(s16, g_BtlMenuPresetCells[g_BtlMenuCursor * 5 + i].icon);
        BtlMenuTextImageLoad(i * 8 + 5, i * 8 + 5, 8);
    }
    BtlMenuIconCellsSet(1, 0xF);
}

/* far_syms: g_BtlMenuItemCount g_BtlActingSlot */
extern u8 *g_BtlMenuPresetSlots;
void BtlPartyActionSet(s32);

/* Tactics, confirm: hand one row of the Tactics grid to the party as this
   round's commands. BtlMenuPresetStep calls it on CROSS with
   &g_BtlMenuPresetCells[cursor * 5], the row's 6-byte cells, one per listed member.

   For member j = 0 .. g_BtlMenuItemCount - 5 (BtlMenuPresetOpen sets g_BtlMenuItemCount to
   the listed members plus the page's four tactic cells, and lists in
   g_BtlMenuPresetSlots the slots whose g_BtlMenuPendingMask bit was set):
     - the current actor g_BtlActingSlot becomes the member's battle slot
       g_BtlMenuPresetSlots[j], and its target g_BtlSlotPlannedTargets[slot] is cleared to 0xFF;
     - by the cell's kind (+4):
         0       nothing more: the member keeps its bit in g_BtlMenuPendingMask, so
                 the command menu still asks for it;
         1       BtlPartyActionSet(0), the command collection for the current
                 slot;
         2 .. 6  the kind itself is the command, g_BtlSlotPlannedKinds[slot], and the
                 cell's +5 its argument, g_BtlSlotPlannedNumbers[slot];
         7 up    nothing is stored;
     - for every kind but 0 the slot's bit leaves the pending mask:
       g_BtlMenuPendingMask &= (1 << slot) ^ 0x1F.
   Answers g_BtlMenuPendingMask; the caller does not use the answer, it reads the
   global again.

   No frame wait and no RNG draw of its own (BtlPartyActionSet's are its own).

   Shape: nothing needed shaping. The counter and the count are s8 (the
   target compares them shifted left by 24), the kinds are a switch whose
   case 0 is `continue`, and the cell is indexed again inside case 2..6. The
   parameter stays s32 because that is what btl_command.c declares and passes. */
/* func_80044E24 */
s32 BtlMenuPresetRowActionSet(s32 cells)
{
    s8 i;
    s8 n;

    n = g_BtlMenuItemCount - 4;
    for (i = 0; i < n; i++) {
        g_BtlActingSlot = g_BtlMenuPresetSlots[i];
        g_BtlSlotPlannedTargets[g_BtlActingSlot] = 0xFF;
        switch (((BtlMenuPresetCell *)cells)[i].kind) {
        case 0:
            continue;
        case 1:
            BtlPartyActionSet(0);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            g_BtlSlotPlannedKinds[g_BtlActingSlot] = ((BtlMenuPresetCell *)cells)[i].kind;
            g_BtlSlotPlannedNumbers[g_BtlActingSlot] = ((BtlMenuPresetCell *)cells)[i].number;
            break;
        }
        g_BtlMenuPendingMask &= (1 << g_BtlActingSlot) ^ 0x1F;
    }
    return g_BtlMenuPendingMask;
}
