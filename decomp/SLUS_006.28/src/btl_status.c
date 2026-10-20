/* Lunar SSSC — btl_status: a status given to the target, one function a bit
   (4, 1, 8, 0x10, 0x30, 0x40, 0x80). The first split had it in game_misc.c.
   Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlTargetSlot */
/* far_syms: CharStats g_BtlTargetSlot */
/* far_syms: g_BtlSlotPlannedKinds (target reaches it %hi/%lo) */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
/* far_syms: g_BtlSlotActiveFlags (carried from the first split's game_misc.c) */

extern u8 g_BtlSlotStatus4Rounds[];
extern u8 g_BtlSlotPlannedKinds[];
extern u8 g_BtlSlotStatus8Rounds[];
extern u8 g_BtlSlotStatus40Rounds[];
extern u8 g_BtlSlotStatus80Rounds[];
extern u8 g_BtlTargetSlot;
extern u8 g_BtlSlotActiveFlags[];
extern long HWD0, VWD0;
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */
extern long HWD0;
extern long VWD0;
s32 BtlTargetTraitCheck(s32, s32, s32, s32);
extern s32 CharStats;
void BtlTargetStatus8Set(void);
void BtlTargetStatus40Set(void);
void BtlTargetStatus80Set(void);
/* far_syms: g_BtlTargetSlot g_BtlSlotPlannedKinds g_BtlSlotActiveFlags CharStats */
/* far_syms: g_BtlSlotActiveFlags */
void BtlTargetStatus4Set(void);
/* far_syms: g_BtlTargetSlot CharStats */
/* far_syms: g_BtlSlotActiveFlags (target reaches it %hi/%lo) */
/* far_syms: CharStats g_BtlSlotActiveFlags g_BtlSlotPlannedKinds */

/* far_syms: CharStats (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotStatus4Rounds (target reaches it %hi/%lo) */

s32 BtlTargetTraitCheck(s32, s32, s32, s32);                      /* extern */
extern s32 CharStats;

/* func_8002F74C */
void BtlTargetStatus4Set(void)
{
  void *stats;
  s32 stats_address;
  s32 status;
  stats_address = CharStats + (g_BtlTargetSlot * 0x3C);
  if ((!(((BtlStats *)stats_address)->status & 0x34)) && ((BtlTargetTraitCheck(0x200, 0x21, 0x26, 0) & 0xFF) != 1))
  {
    status = ((u8 *) (CharStats + (g_BtlTargetSlot * 0x3C))) + 0x15;
    *((u8 *) status) = 4;
    *(g_BtlTargetSlot + g_BtlSlotStatus4Rounds) = 0;
    if (((u8) g_BtlTargetSlot) < 5U)
    {
      stats = (g_BtlTargetSlot * 0x3C) + CharStats;
      ((BtlStats *)stats)->side.party.flags = (u8) (AS(s16, ((BtlStats *)stats)->side.party.flags) & 0xFB);
    }
  }
}

/* far_syms: g_BtlPartyStatus1Shares (target reaches it %hi/%lo) */
extern u8 g_BtlPartyStatus1Shares[];

/* func_8002F848 */
void BtlTargetStatus1Set(void)
{
  if (g_BtlTargetSlot < 5)
  {
    if (((*(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x15)) & 0x35) == 0)
    {
      if ((BtlTargetTraitCheck(0x100, 0x21, 0x26, 0) & 0xFF) != 1)
      {
        *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x15) = 1;
        *(g_BtlTargetSlot + g_BtlPartyStatus1Shares) = 0xA;
        if (g_BtlTargetSlot < 5)
          *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1E) =
            *(u8 *)(CharStats + g_BtlTargetSlot * 0x3C + 0x1E) & 0xFB;
      }
    }
  }
}

s32 BtlTargetTraitCheck(s32, s32, s32, s32);                      /* extern */
extern s32 CharStats; /* far_syms: CharStats */

/* func_8002F958 */
void BtlTargetStatus8Set(void) {
    void *stats;

    stats = (void *)CharStats + (g_BtlTargetSlot * 0x3C);
    if (!(((BtlStats *)stats)->status & 0x35) && ((BtlTargetTraitCheck(0x400, 0x21, 0x26, 0) & 0xFF) != 1)) {
        ((BtlStats *)stats)->status = 8U;
        *(g_BtlTargetSlot + g_BtlSlotStatus8Rounds) = 0;
        g_BtlSlotPlannedKinds[g_BtlTargetSlot] = 0xB;
    }
}

/* far_syms: g_BtlSlotStatus8Rounds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotStatus40Rounds (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotStatus80Rounds (target reaches it %hi/%lo) */

/* func_8002FA00 */
void BtlTargetStatus10Set(void)
{
  u8 idx;
  if ((BtlTargetTraitCheck(0x800, 0x25, 0x26, 0) & 0xFF) != 1)
  {
    *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x15)) = 0x10;
    idx = g_BtlTargetSlot;
    *(idx + g_BtlSlotStatus80Rounds) = 0;
    *(idx + g_BtlSlotStatus40Rounds) = 0;
    *(idx + g_BtlSlotStatus8Rounds) = 0;
    *(idx + g_BtlSlotStatus4Rounds) = 0;
    if ((u8)g_BtlTargetSlot < 5U)
    {
      *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1E)) =
        *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1E)) & 0xFB;
    }
  }
}

/* func_8002FAFC */
void BtlTargetStatus30Set(void)
{
  u8 idx;
  if ((BtlTargetTraitCheck(0x800, 0x25, 0x26, 0) & 0xFF) != 1)
  {
    *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x15)) = 0x30;
    idx = g_BtlTargetSlot;
    *(idx + g_BtlSlotStatus80Rounds) = 0;
    *(idx + g_BtlSlotStatus40Rounds) = 0;
    *(idx + g_BtlSlotStatus8Rounds) = 0;
    *(idx + g_BtlSlotStatus4Rounds) = 0;
    *(idx + g_BtlSlotActiveFlags) = 0;
    if ((u8)g_BtlTargetSlot < 5U)
    {
      *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1E)) =
        *((u8 *)((u8 *)(CharStats + (g_BtlTargetSlot * 0x3C)) + 0x1E)) & 0xFB;
    }
  }
}

/* func_8002FC08 */
void BtlTargetStatus40Set(void) {
    void *stats;

    { s32 idx = g_BtlTargetSlot * 0x3C; stats = (u8 *)CharStats + idx; }
    if (!(((BtlStats *)stats)->status & 0x3D) && ((BtlTargetTraitCheck(0x1000, 0x25, 0x26, 0) & 0xFF) != 1)) {
        ((BtlStats *)stats)->status = 0x40U;
        *(g_BtlTargetSlot + g_BtlSlotStatus40Rounds) = 0;
    }
}

/* func_8002FC94 */
void BtlTargetStatus80Set(void) {
    void *stats;

    stats = (void *)CharStats + (g_BtlTargetSlot * 0x3C);
    if ((((BtlStats *)stats)->status == 0) && ((BtlTargetTraitCheck(0x2000, 0x25, 0x26, 0) & 0xFF) != 1)) {
        ((BtlStats *)stats)->status = 0x80U;
        *(g_BtlTargetSlot + g_BtlSlotStatus80Rounds) = 0;
    }
}
