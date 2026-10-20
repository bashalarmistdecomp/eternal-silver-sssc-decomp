/* Lunar SSSC — btl_message: the battle's messages: the end
   (BtlEndMessageDisp, the developers' name), the win, its reward and items,
   the levels and their gains, and the message's string, controls and wait.
   The first split had it in misc.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "pad.h"
#include "mem.h"
#include "btl.h"
#include "sprite.h"
/* far_syms: g_BtlActingSlot (carried from the first split's misc.c) */
/* far_syms: g_BtlSlotPlannedTargets (carried from the first split's misc.c) */
/* far_syms: g_BtlSlotPlannedKinds (carried from the first split's misc.c) */
/* far_syms: g_BtlSlotPlannedNumbers (carried from the first split's misc.c) */

extern s32 CharStats;
extern u8 g_BtlActingSlot;
s32 BtlWindowsMoveWait();                                  /* extern */
s32 BtlMenuTextImageLoad(s32, s32, s32);                       /* extern */
extern u8 g_BtlWinItems[];
s32 BtlWinItemMessageDisp(s32);
s32 BtlMenuCellsClear(s32);
extern void *g_BtlMenuItems;
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlMenuPadRecord[];
extern u8 g_BtlSlotIds[];
/* The party HUD's per-member record BtlPartyPanelDisp builds on the stack and
   BtlPartyPanelMemberDisp draws: a used flag, three status bytes (CharStats +0x1E & 4,
   +0x15, +0x14) and the hp / max hp / mp / max mp words (CharStats +0..+6). */
extern volatile char g_BtlPartyCount;
extern u8 g_BtlEnemyCount;
extern u8 g_BtlPartyNotHeldFlags[];
extern u8 g_BtlSlotActiveFlags[];
extern s32 g_BtlMenuTextBlocks;
s32 BtlWindow0Set(s32, s32, s32, s32);
s32 BtlMenuIconCellsSet(s32, s32);
extern u16 g_MessageLine;
extern s8 g_BtlEnemyChoice[3];
extern u8 D_800B8C65;
extern u16 g_BtlPartyStat08Sum;
extern s16 g_BtlEnemyStat08Sum;
extern u8 g_BtlSlotPlannedTargets[];
extern u8 g_BtlSlotPlannedNumbers[];

s32 BtlWindowSet(s32, s32, s32, s32, s32);
extern u8 g_BtlMenuItemCount;
s32 BtlCellRowsUpdate(s32, s32, s32, s32);
s32 BtlCellAreaClear(s32 column, s32 row, s32 unused, s32 width, s32 a4, s32 a5);
s32 BtlWindowSet(s32, s32, s32, s32, s32);
s32 BtlCellRowsUpdate(s32, s32, s32, s32);
s32 BtlCellAreaClear(s32, s32, s32, s32, s32, s32);
s32 BtlWindowSet(s32, s32, s32, s32, s32);
extern u8 g_BtlMenuItemCount;
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
extern s32 g_BtlRecords;
s32 BtlCellGlyphSet(s32, s32, s32, s32, s32, s32);
s32 CellNumberSet(s32, s32, s32, s32, s32, s32, s32, s32);
s32 BtlMenuFrameStep();
s32 BtlMenuFrameStep();
/* far_syms: g_BtlRecords */
/* far_syms: CharStats */
/* far_syms: g_BtlSlotIds */
void BtlWindowBoxesOff(void);  /* extern */
void BtlCellRowsClear(s32 first_row, s32 row_count, s32 kind_mask);  /* extern */
void BtlMenuTextCellsSet(s32 column, s32 first_row, s32 row_block_count, s32 text_row_count, s32 pad4_0, u16 first_place);  /* extern */

typedef struct {
    u16 a;
    u16 id;
    u16 c;
} MsgRef;
extern u8 g_BtlEndMessage[];
extern u8 D_800124AC[];
s32 debug_printf();
s32 BtlMessagePadWait(s32 clear, s32 wait);  /* TU defines this below */
s32 BtlMenuClose();
s32 BtlMessageMenuReset(void);  /* TU defines this below */
s32 BtlMenuOpen();

/* func_8004BA18 */
void BtlEndMessageDisp(u8 kind)
{
    MsgRef msg;

    msg = *(MsgRef *)g_BtlEndMessage;
    debug_printf(D_800124AC, kind);
    if (kind == 0) {
        BtlMessagePadWait(1, 0);
        BtlMenuClose();
        return;
    }
    BtlMessageMenuReset();
    BtlMenuOpen();
    BtlCellRowsClear(2, 3, 0x20);
    BtlWindowSet(0, 2, 1, 0x24, 4);
    BtlWindowsMoveWait();
    if (kind == 4) {
        msg.id = 0xF669;
    } else {
        msg.id += kind - 1;
    }
    BtlMenuTextCellsSet(3, 2, 0x11, 1, 0xDEAD2001, 0);
    BtlCellRowsUpdate(2, 0x80, 4, 0x20);
    BtlMessageDisp(&msg);
    BtlWindowSet(1, 2, 1, 0, 0);
    BtlWindowsMoveWait();
}

/* far_syms: g_BtlPartyNotHeldFlags (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
s32 BtlMenuOpen();  /* TU declares this below */
s32 BtlMenuClose();
s32 BtlMessageMenuReset(void);  /* TU defines this below */
void BtlWinItemsMessageDisp(void);  /* TU defines this below */

/* func_8004BB68 */
void BtlWinMessageDisp(void)
{
  u16 i;
  u8 *ptr;

  BtlMessageMenuReset();
  for (i = 0; i < 5; i++)
  {
    if ((g_BtlSlotIds[i] == 0) || (g_BtlSlotIds[i] == 0xC))
      continue;
    if (g_BtlSlotActiveFlags[i] == 0)
      continue;
    if (g_BtlPartyNotHeldFlags[i] != 1)
      continue;
    ptr = (u8 *) ((i * 0x3C) + CharStats);
    if (*((u16 *) ptr) == 0)
      continue;
    if ((ptr[0x15] & 0x30) == 0x30)
      continue;
    BtlAnimationSet(g_BtlRecords + (i << 6), 0xF);
  }
  BtlMenuOpen();
  BtlWinRewardMessageDisp();
  BtlWinItemsMessageDisp();
  BtlMenuClose();
}

s32 BtlMenuOpen();                                  /* extern */
s32 BtlMenuClose();                                  /* extern */
s32 BtlMessageMenuReset();                                  /* extern */
void BtlLevelUpGainsDisp();                                  /* extern */

/* func_8004BC7C */
void BtlLevelUpMessageDisp(void) {
    BtlMessageMenuReset();
    BtlMenuOpen();
    BtlLevelUpGainsDisp();
    BtlMenuClose();
}

/* far_syms: g_BtlMenuItems (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuItemCount (target reaches it %hi/%lo) */

extern u8 g_BtlMenuItemCount;

/* func_8004BCB4 */
s32 BtlMessageMenuReset(void) {
    BtlWindowBoxesOff();
    ((BtlMenuItem *)g_BtlMenuItems)->on = 1;
    ((BtlMenuItem *)g_BtlMenuItems)->disabled = 0;
    g_BtlMenuItemCount = 1;
    ((BtlMenuItem *)g_BtlMenuItems)->attr = 0x20200000;
    ((BtlMenuItem *)g_BtlMenuItems)->x = 3;
    ((BtlMenuItem *)g_BtlMenuItems)->y = 2;
}

s32 font_target_clear(s32, s32);
extern u16 g_BtlWinRewardMessage[];
extern s32 g_BtlWinExperience;
extern u16 g_BtlWinMoney;

void *memcpy(void *, const void *, unsigned int);
/* func_8004BD1C */
void BtlWinRewardMessageDisp(void)
{
  u16 buf[5];
  u16 n;
  u16 m;
  s32 row;

  m = 0;
  memcpy(buf, g_BtlWinRewardMessage, sizeof(buf));
  n = 1;
  if ((g_BtlWinExperience != 0) || (g_BtlWinMoney != 0))
  {
    font_target_clear(g_BtlMenuTextBlocks, 0x22);
    BtlMenuTextImageLoad(0, 0, 0x22);
    BtlMenuCellsClear(0x30);
    if ((g_BtlWinExperience != 0) && (g_BtlWinMoney != 0))
    {
      BtlWindow0Set(2, 1, 0x24, 6);
    }
    else
    {
      BtlWindow0Set(2, 1, 0x24, 4);
    }
    if (g_BtlWinExperience != 0)
    {
      BtlMenuTextCellsSet(9, 2, 0x11, 1, 0xDEAD2001, 0);
      CellNumberSet(g_BtlWinExperience, 4, 2, 5, 0xDEAD2001, 0x3000, 1, 0);
      m++;
      buf[n++] = 0xF623;
      buf[n++] = 0xFF02;
    }
    if (g_BtlWinMoney != 0)
    {
      row = m * 2 + 2;
      BtlMenuTextCellsSet(0xA, row, 0x11, 1, 0xDEAD2001, (u16)(m * 0x11));
      CellNumberSet(g_BtlWinMoney, 4, row, 5, 0xDEAD2001, 0x3000, 1, 0);
      BtlCellGlyphSet(0x9E, 9, row, 1, 0x2000, 1);
      buf[n++] = 0xF624;
    }
    buf[n] = 0xFFFF;
    BtlCellRowsUpdate(2, 0x80, 4, 0x30);
    BtlMessageDisp(buf);
    BtlMessagePadWait(1, 1);
  }
}

/* far_syms: g_BtlWinItemMessage (target reaches it %hi/%lo) */
/* far_syms: g_BtlWinItems (target reaches it %hi/%lo) */

extern u16 g_BtlWinItemMessage[];

/* func_8004BF8C */
s32 BtlWinItemMessageDisp(s32 item_index)
{
  u16 local[4];

  memcpy(local, g_BtlWinItemMessage, 8);
  BtlMenuCellsClear(0x30);
  BtlWindow0Set(2, 1, 0x24, 5);
  ((BtlMenuItem *)g_BtlMenuItems)->icon = g_BtlWinItems[item_index & 0xFF];
  BtlMenuIconCellsSet(0, 3);
  local[1] = g_BtlWinItems[item_index & 0xFF] | 0xF100;
  BtlMenuTextCellsSet(9, 3, 0x11, 1, 0xDEAD2001, 0);
  BtlCellRowsUpdate(2, 0x80, 4, 0x30);
  return BtlMessageDisp(local);
}

s32 BtlMessagePadWait(s32, s32);

/* func_8004C068 */
void BtlWinItemsMessageDisp(void)
{
  u8 item_index;
  s32 i;
  i = (unsigned int) 0;
  item_index = 0 & 0xFF;
  do
  {
    item_index = i & 0xFF;
    if ((*(item_index + g_BtlWinItems)) != 0)
    {
      BtlWinItemMessageDisp((unsigned long) item_index);
      BtlMessagePadWait(1, 1);
    }
    i += 1;
  }
  while (((u32) (i & 0xFF)) < 8U);
}

/* far_syms: g_BtlLevelUpGainMessage D_800B879C g_FontPen g_FontTarget D_800E49D1 g_BtlMenuTextBlocks g_BtlMenuItems */
extern u16 g_BtlLevelUpGainMessage[];
extern u8 D_800B879C[];
extern u8 g_FontPen;
extern s32 g_FontTarget;
extern u8 D_800E49D1[];
s32 font_col_set(s32);
s32 font_string_put(s32, s32, s32);
void font_bytes_put(u8 *string, s32 mode);

/* func_8004C0D8 */
void BtlLevelUpGainDisp(s32 gain_then_column)
{
    u16 local[4];
    u8 *p;
    u8 *gain_byte;
    s32 div;
    s32 first_line_target;

    memcpy(local, g_BtlLevelUpGainMessage, 8);
    gain_byte = D_800E49D1 + gain_then_column;
    p = (u8 *)local;
    if (*gain_byte == 0)
        return;
    if (gain_then_column != 9)
    {
        font_target_clear(g_BtlMenuTextBlocks + 0x880, 0x14);
        font_col_set(0);
        font_string_put((gain_then_column + 0x628) & 0xFFFF, 0, 1);
        font_bytes_put(D_800B879C, 1);
        font_string_put(0x626, 0, 1);
        BtlMenuTextImageLoad(0, 0, 0x22);
        first_line_target = g_FontTarget - 0x880;
        div = (first_line_target - g_BtlMenuTextBlocks) / 64;
        gain_then_column = div + 5;
        if (g_FontPen)
            gain_then_column = div + 6;
        BtlCellAreaClear(0x10, 4, 0xCC00, 0x14, 2, 0x20);
        CellNumberSet(*gain_byte, gain_then_column, 4, 2, 0xDEAD2001, 0x2000, 1, 0);
        BtlCellRowsUpdate(2, 0x80, 4, 0x30);
        return;
    }
    font_target_clear(g_BtlMenuTextBlocks, 0x33);
    BtlMenuTextImageLoad(0, 0, 0x22);
    BtlMenuCellsClear(0x30);
    BtlCellRowsUpdate(2, 0x80, 4, 0x30);
    BtlWindow0Set(2, 1, 0x24, 5);
    BtlMenuTextCellsSet(9, 3, 0x11, 1, 0xDEAD2001, 0);
    ((BtlMenuItem *)g_BtlMenuItems)->icon = *gain_byte + 0xB6;
    BtlMenuIconCellsSet(0, 3);
    BtlCellRowsUpdate(2, 0x80, 4, 0x30);
    local[1] = *gain_byte - 0xE4A;
    BtlMessageDisp((s32)p);
}

/* far_syms: g_ChrLevelUpGains g_BtlMenuTextBlocks */
typedef struct {
    u16 h[5];
} Msg10;
extern Msg10 g_BtlLevelUpGainsMessage;
extern u8 g_ChrLevelUpGains[];

/* func_8004C324 */
void BtlLevelUpGainsDisp(void)
{
    Msg10 m;
    u8 i;

    m = g_BtlLevelUpGainsMessage;
    if (g_ChrLevelUpGains[0] == 0)
        return;
    font_target_clear(g_BtlMenuTextBlocks, 0x22);
    BtlMenuTextImageLoad(0, 0, 0x22);
    BtlMenuCellsClear(0x30);
    BtlCellRowsUpdate(2, 0x80, 4, 0x30);
    BtlWindow0Set(2, 1, 0x24, 6);
    BtlMenuTextCellsSet(4, 2, 0x11, 2, 0xDEAD2001, 0);
    BtlCellRowsUpdate(2, 0x80, 4, 0x30);
    if (g_ChrLevelUpGains[0] == 0xB)
        m.h[1] = 0xF001;
    else
        m.h[1] = g_ChrLevelUpGains[0] | 0xF000;
    BtlMessageDisp((s32)&m);
    for (i = 0; i < 10; i++)
    {
        BtlLevelUpGainDisp(i);
        BtlMessagePadWait(0, 1);
    }
    font_target_clear(g_BtlMenuTextBlocks, 0x22);
    BtlMenuTextImageLoad(0, 0, 0x22);
    BtlMenuCellsClear(0x30);
    BtlCellRowsUpdate(2, 0x80, 4, 0x30);
}

s32 font_code_put(s32, s32);                            /* extern */
s32 font_target_clear(s32, s32);

/* func_8004C498 */
void BtlMessageDisp(s32 message) {
    u16 *ptr;
    s32 saved;
    s32 first_block;

    saved = g_MessageLine;
    g_MessageLine = 0;
    font_target_clear(0x801CBB00, 0x33);
    ptr = (u16 *)(((message - 2) & ~1) + 2);
    while (*ptr != 0xFFFF) {
        if (*ptr <= 0xEFFF) {
            font_code_put(*ptr, 1);
            first_block = g_MessageLine * 0x11;
            BtlMenuTextImageLoad(first_block, first_block, 0x11);
        } else {
            BtlMessageControlRun(ptr);
        }
        ptr++;
    }
    g_MessageLine = saved;
}

s32 font_code_put(s32, s32);                            /* extern */

/* func_8004C56C */
void BtlMessageStringCodePut(s32 code) {
    s32 first_block;

    font_code_put(code & 0xFFFF, 1);
    first_block = g_MessageLine * 0x11;
    BtlMenuTextImageLoad(first_block, first_block, 0x11);
}

/* func_8004C5A8 */
void BtlMessageStringControlRun(s32 code)
{
    u16 var[2];

    if ((code & 0xFFFFu) > 0xEFFFu)
    {
        var[1] = 0xFFFFu;
        var[0] = (u16)code;
        BtlMessageControlRun(var);
    }
}

/* far_syms: g_FontStringTable */
extern s32 g_FontStringTable;
s32 mem_clear(s32, s32);
s32 pad_record_init(s32 *, s32, s32, s32);
void font_target_set(u8 *target);
void font_pen_move(s32);
s32 font_string_walk(s32, void (*)(s32), void (*)(s32));

/* BtlMessageControlRun — one script command word (*cmd), dispatched on
   its high byte (*cmd & 0xFF00):
     0xF0xx / 0xF1xx / 0xF4xx / 0xF5xx / 0xF6xx  text: entry (*cmd & 0xFFF) of
         the u16 offset table at g_FontStringTable, i.e. g_FontStringTable + tbl[n], run
         through font_string_walk(text, BtlMessageStringCodePut, BtlMessageStringControlRun). (A case
         0xF080 is also present; it can never equal a 0xFF00-masked value.)
     0xF8nn  wait nn frames (BtlMenuFrameStep per frame; nn re-read each pass).
     0xF9nn  font_pen_move(nn).
     0xFAnn / 0xFBnn  nothing.
     0xFCnn  font_col_set((nn << 2) & 0xFC).
     0xFF00  start the g_BtlMenuPadRecord transfer (pad_record_init(blk, 0x10, 0x40, 0))
             and wait frames until its +4 halfword has bit 0x40.
     0xFF01  reset the 3-step g_MessageLine sequence: clear 0x33 bytes at
             0x801CBB00 (mem_clear), BtlMenuTextImageLoad(17k, 17k, 0x11) for
             k = 0..2, then font_target_set(0x801CBB00).
     0xFF02  advance it: ++g_MessageLine < 3 -> font_target_set(0x801CBB00 +
             g_MessageLine * 0x880); the third advance falls into 0xFF01's reset.
     0xFF03  BtlWindow0Set(2, 1, 0, 0).
   Anything else: nothing. No RNG.
   What decided the match: the empty 0xFB00 case (GCC's case tree gives node
   0xFC00 a left child whose only code is a jump to the exit, which jump
   optimisation deletes, leaving the target's slt/beqz range check), and the
   0xF8 bound written `*cmd & 0xFF` (cse reuses the switch's halfword load,
   kept in $a0 for it; the in-loop reload is narrowed to lbu by combine) —
   `(u8)*cmd` folds to a byte load up front and loses both. */
/* func_8004C5E4 */
void BtlMessageControlRun(u16 *cmd)
{
    u16 i;
    u8 *p;

    switch (*cmd & 0xFF00) {
    case 0xFF00:
        switch (*cmd) {
        case 0xFF02:
            if (++g_MessageLine < 3) {
                font_target_set((u8 *)0x801CBB00 + g_MessageLine * 0x880);
                break;
            }
        case 0xFF01:
            g_MessageLine = 0;
            mem_clear(0x801CBB00, 0x33);
            for (; g_MessageLine < 3; g_MessageLine++) {
                BtlMenuTextImageLoad(g_MessageLine * 17, g_MessageLine * 17, 0x11);
            }
            g_MessageLine = 0;
            font_target_set((u8 *)0x801CBB00);
            break;
        case 0xFF00:
            p = g_BtlMenuPadRecord;
            pad_record_init((s32 *)p, 0x10, 0x40, 0);
            while (!(((PadRecord *)p)->repeated & 0x40)) {
                BtlMenuFrameStep();
            }
            break;
        case 0xFF03:
            BtlWindow0Set(2, 1, 0, 0);
            break;
        }
        break;
    case 0xF900:
        font_pen_move(*(u8 *)cmd);
        break;
    case 0xFC00:
        font_col_set((*(u8 *)cmd << 2) & 0xFC);
        break;
    case 0xF800:
        for (i = 0; i < (*cmd & 0xFF); i++) {
            BtlMenuFrameStep();
        }
        break;
    case 0xFA00:
        break;
    case 0xFB00:
        break;
    case 0xF000:
    case 0xF080:
    case 0xF100:
    case 0xF400:
    case 0xF500:
    case 0xF600:
        font_string_walk(g_FontStringTable + *(u16 *)((*cmd & 0xFFF) * 2 + g_FontStringTable), BtlMessageStringCodePut, BtlMessageStringControlRun);
        break;
    }
}

/* far_syms: g_BtlMenuTextBlocks (target reaches it %hi/%lo) */
/* far_syms: g_BtlMenuPadRecord (target reaches it %hi/%lo) */
s32 font_target_clear(s32, s32);
s32 BtlMenuFrameStep();

/* func_8004C8B0 */
s32 BtlMessagePadWait(s32 clear, s32 wait)
{
  s32 frames_done;
  u8 *pad_record;
  frames_done = (wait == 0) << 7;
  pad_record = (u8 *) (&g_BtlMenuPadRecord);
  loop_1:
  BtlMenuFrameStep();

  if (((PadRecord *)pad_record)->repeated != 0x40)
  {
    if (((u32) (frames_done & 0xFFFF)) < 0x80U)
    {
      frames_done += 1;
      goto loop_1;
    }
  }
  if (clear != 0)
  {
    font_target_clear(g_BtlMenuTextBlocks, 0x22);
    BtlMenuTextImageLoad(0, 0, 0x22);
    BtlMenuCellsClear(0x30);
    BtlCellRowsUpdate(2, 0x80, 4, 0x30);
  }
}

u8 BtlEnemyStat08Sum(void);
u8 BtlPartyStat08Sum(void);
void BtlEnemyActionPick(void);

/* func_8004C958 */
void BtlEnemyActionSet(void)
{
  u8 *rec;
  g_BtlEnemyChoice[0] = 0;
  g_BtlEnemyChoice[1] = 0;
  g_BtlEnemyChoice[2] = 0;
  g_BtlPartyCount = 0;
  g_BtlEnemyCount = 0;
  g_BtlPartyStat08Sum = 0;
  g_BtlEnemyStat08Sum = 0;
  D_800B8C65 = 0;
  BtlEnemyStat08Sum();
  BtlPartyStat08Sum();
  for (g_BtlActingSlot = 6; g_BtlActingSlot < 14; g_BtlActingSlot++)
  {
    if (g_BtlSlotIds[g_BtlActingSlot] != 0)
    {
      rec = (u8 *) (g_BtlActingSlot * 0x3C + CharStats);
      if ((*(u16 *) rec != 0) && (rec[0x15] != 4) && (rec[0x15] != 8) && (rec[0x15] != 0x30))
      {
        BtlEnemyActionPick();
        g_BtlSlotPlannedKinds[g_BtlActingSlot] = ((u8 *) g_BtlEnemyChoice)[0];
        g_BtlSlotPlannedNumbers[g_BtlActingSlot] = ((u8 *) g_BtlEnemyChoice)[1];
        g_BtlSlotPlannedTargets[g_BtlActingSlot] = ((u8 *) g_BtlEnemyChoice)[2];
      }
    }
  }
}
