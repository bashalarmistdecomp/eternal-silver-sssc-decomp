/* Lunar SSSC — map_step: the parts of a map frame: the pad's, the party's
   and the objects', the party's status on the grid and its colour, and what
   a frame counts. The first split had it in misc_h.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern u16 D_800B8DF0[2];
s32 MapPadStep();                                /* extern */

/* far_syms: g_PlayTimeFrames (carried from the first split's misc_e.c) */
extern u8 D_8001282C[];
extern u8 D_80012840[];
extern u8 D_80012854[];
extern s8 g_DebugAllowed;
extern u16 g_MapStatusRequest;
extern u16 g_MapPadPreviousHeldButtons;
extern u16 g_MapIdleCount;
extern u16 g_MapPadHeldButtons;
extern u16 g_MapPadPressedButtons;
extern u16 g_PlayTimeFrames;
extern u16 g_MapPartyWalkStepCount;
extern s32 g_PartyIds;
extern s32 g_ChrRecords;
void MapStatusDisp();
s32 MapPartyAnimationStep();

extern s32 g_MapPartyRecords;
/* far_syms: g_PlayTimeMinutes (target reaches it %hi/%lo) */
/* far_syms: g_PlayTimeMinutes (auto: target reaches it %hi/%lo — addressing row 3) */
extern u16 g_PlayTimeMinutes;

/* far_syms: g_MapPadDirectionsCol (target reaches it %hi/%lo) */
/* far_syms: g_MapCol (target reaches it %hi/%lo) */
/* far_syms: g_MapPadDirectionsBoxLeft (target reaches it %hi/%lo) */
/* far_syms: g_MapPadDirectionsBoxRight (target reaches it %hi/%lo) */
/* far_syms: g_MapPadDirectionsBoxTop (target reaches it %hi/%lo) */
/* far_syms: g_MapPadDirectionsBoxBottom (target reaches it %hi/%lo) */
/* far_syms: g_MapPartyHpTakeOn (target reaches it %hi/%lo) */
/* far_syms: g_MapPartyHpTakeWait (target reaches it %hi/%lo) */
/* far_syms: g_MapPadDirectionsState (target reaches it %hi/%lo) */
/* far_syms: g_MapPadDirections (target reaches it %hi/%lo) */
/* far_syms: g_PlayTimeFrames (target reaches it %hi/%lo) */
/* far_syms: g_MapPadDirectionsBoxLeft (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MapPadDirectionsBoxRight (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MapPadDirectionsBoxTop (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MapPadDirectionsBoxBottom (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MapPartyHpTakeOn (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MapPartyHpTakeWait (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_MapPadDirectionsState (auto: target reaches it %hi/%lo — addressing row 3) */
extern s16 g_MapCol;
extern u16 g_PadHeldButtons;
extern u8 g_MapPadDirectionsCol[];
extern s32 g_MapLayersMask;
extern s16 g_MapPadDirectionsBoxLeft;
extern s16 g_MapPadDirectionsBoxRight;
extern s16 g_MapPadDirectionsBoxTop;
extern s16 g_MapPadDirectionsBoxBottom;
extern u8 g_MapPartyHpTakeOn;
extern u8 g_MapPartyHpTakeWait;
extern u8 g_MapPadDirectionsState;
extern u8 g_MapPadDirections[];

/* func_80055EC4 */
s32 MapPadStep(void)
{
  s32 pad;
  s32 prev;
  u16 facing;
  s32 dir;
  s16 *first_record;
  void *target_col;

  pad = pad_buttons_get();
  prev = g_MapPadHeldButtons;
  g_PadHeldButtons = pad;
  g_MapPadHeldButtons = pad;
  g_MapPadPreviousHeldButtons = prev;
  g_MapPadPressedButtons = pad & (prev ^ pad);
  switch (pad & 0xF000)
  {
    case 0x1000:
      dir = 1;
      break;
    case 0x3000:
      dir = 2;
      break;
    case 0x2000:
      dir = 3;
      break;
    case 0x6000:
      dir = 4;
      break;
    case 0x4000:
      dir = 5;
      break;
    case 0xC000:
      dir = 6;
      break;
    case 0x8000:
      dir = 7;
      break;
    case 0x9000:
      dir = 8;
      break;
    default:
      dir = 0;
      break;
  }
  facing = *((u16 *) (((u8 *) g_MapPadDirections) + (dir * 2)));
  if (facing == 0)
  {
    if (g_MapIdleCount < 0x12C)
    {
      g_MapIdleCount++;
      if (g_MapIdleCount >= 0x12C)
      {
        g_MapStatusRequest = 1;
      }
    }
  }
  else if (g_MapIdleCount != 0)
  {
    if ((g_MapPartyHpTakeOn == 0) || (g_MapPartyHpTakeWait < 0xF))
    {
      g_MapIdleCount = 0;
      g_MapStatusRequest = 2;
    }
  }
  if (g_MapPartyHpTakeOn != 0)
  {
    if (g_MapPartyHpTakeWait == 0)
    {
      MapPartyHpTake();
      g_MapIdleCount = 0x12C;
      g_MapStatusRequest = 1;
    }
    else if (g_MapPartyHpTakeWait < 0x1D)
    {
      layer_col_set(0, g_MapLayersMask | 0x100, &g_MapCol);
    }
    g_MapPartyHpTakeWait--;
  }
  else if (g_MapPartyHpTakeWait != 0)
  {
    g_MapPartyHpTakeWait = 0;
    layer_col_set(0, g_MapLayersMask | 0x100, &g_MapCol);
  }
  if (g_MapPadDirectionsState != 0)
  {
    first_record = (s16 *) g_MapPartyRecords;
    if ((first_record[3] < g_MapPadDirectionsBoxLeft) || (first_record[3] >= g_MapPadDirectionsBoxRight) || (first_record[4] < g_MapPadDirectionsBoxTop) || (first_record[4] >= g_MapPadDirectionsBoxBottom))
    {
      if (!(layer_col_fade_check(0) & 0xFF))
      {
        MapPadDirectionsSet(0);
        layer_col_set(0, g_MapLayersMask | 0x100, &g_MapCol);
        g_MapPadDirectionsState = 0;
      }
    }
    else if (!(layer_col_fade_check(0) & 0xFF))
    {
      if (g_MapPadDirectionsState == 1)
      {
        target_col = g_MapPadDirectionsCol;
        g_MapPadDirectionsState = 2;
      }
      else
      {
        target_col = &g_MapCol;
        g_MapPadDirectionsState = 1;
      }
      layer_col_fade_start(0, g_MapLayersMask | 0x100, 1, 0x18, target_col);
    }
  }
  return facing;
}

s32 debug_printf(s32 *);                                /* extern */
s32 profile_mark_add(s32);                               /* extern */
s32 MapAttributeRun();                                  /* extern */
s32 MapPartyWalkStep(s32);                             /* extern */
s32 MapPartyTalkCheck();                                  /* extern */
s32 MenuRun(s32);                                 /* extern */
void MenuSaveRun(void);                             /* extern */
extern u16 g_PadHeldButtons; /* far_syms: g_PadHeldButtons */

/* func_80056248 */
void MapPartyStep(void) {
    s32 facing;
    s32 steps_done;
    s32 step_count;

    facing = MapPadStep();
    if (facing & 0xFFFF) {
        step_count = g_MapPartyWalkStepCount;
        steps_done = 0;
        if (step_count != 0) {
loop_2:
            if (MapPartyWalkStep(facing & 0xFFFF) & 0xFFFF) {
                steps_done += 1;
                if ((steps_done & 0xFFFF) < (s32) step_count) {
                    goto loop_2;
                }
            }
        }
    }
    profile_mark_add(0x210000FF);
    MapAttributeRun();
    if ((g_DebugAllowed == 0) || !(g_MapPadHeldButtons & 0x100)) {
        switch (g_MapPadPressedButtons) {                       /* irregular */
        case 0x40:
            debug_printf((s32 *)D_8001282C);
            MapPartyTalkCheck();
            break;
        case 0x4:
            /* fallthrough */
        case 0x8:
            debug_printf((s32 *)D_80012840);
            g_MapIdleCount = 0x12C;
            g_MapStatusRequest = 1;
            break;
        case 0x80:
            /* fallthrough */
        case 0x100:
            debug_printf((s32 *)D_80012854);
            MenuRun(2);
            break;
        }
        if ((g_MapPadPreviousHeldButtons & 0x20) && !(g_PadHeldButtons & 0x20)) {
            MenuSaveRun();
        }
    }
    MapPartyAnimationStep();
}

s32 profile_mark_add(s32);                                 /* extern */
s32 MapObjectsAnimationStep();                                  /* extern */
s32 MapObjectsThink();                                  /* extern */

/* func_800563D4 */
void MapObjectsStep(void) {
    MapObjectsThink();
    profile_mark_add(0x21008F8F);
    MapObjectsAnimationStep();
}

/* MapStatusDisp -- The field's idle PARTY STATUS STRIP, drawn or
   cleared by the request g_MapStatusRequest (MapFrameUpdate, the field frame's
   tail, and MapStatusClear call it; MapFrameCount clears the request every
   frame). The field's pad step MapPadStep raises it: 1 when its
   per-direction lookup ((u16 *)g_MapPadDirections)[dir] has been 0 for 0x12C
   calls (the counter g_MapIdleCount; two other paths there set the counter to
   0x12C and the request to 1 directly), 2 when the lookup turns non-zero
   with the counter running; MapStatusClear asks for 2 as well.

   request 0: nothing.
   request != 0: CellRowsClear(0x16, 4, 0x20) first - text rows 0x16..0x19,
       0x20 cells.
   request 1: for party slot i = 5 down to 0 whose id (u16 g_PartyIds[i])
       is 1..0xB, at column x (0x20 for the first member found, 6 less for
       each further one - the strip fills from the right), from the
       member's 0x40 record in g_ChrRecords:
         row 0x16  CellChrGlyphSet(id, x, 0x16, 0xDEAD2001, 0x3000, 0), the name
         row 0x17  glyphs 0xA at x and 0xB at x + 1 (CellGlyphSet), then
                   the byte +0x1C (the level) as 2 digits at x + 2
         row 0x18  the u16 +0 as 3 digits at x + 1, colour
                   MapStatusColGet(+0, +2)   (current / maximum)
         row 0x19  the u16 +4 as 3 digits at x + 1, colour
                   MapStatusColGet(+4, +6)
       MapStatusColGet(cur, max), just below: 0x2000 when cur == max, 0x3000
       when cur > max / 2, 0x1000 when cur > max / 4, else 0.
       After the loop, left of the last member drawn (x is already 6 less):
       glyphs 0x5C at x + 2 and 0x5E at x + 3 on row 0x18, 0x5D at x + 2
       and 0x5E at x + 3 on row 0x19 - the two row labels.
   any other request (2): func_800572D8(), an empty function - the clear
       above is all it does.
   No frame wait and no RNG draw, here or in its 20-function call tree.

   Shape: everything is spelled out at each use and left to cse - `x + 1`
   is one register across four calls, `x + 2` / `x + 3` after the loop
   likewise, the record address is `(id << 6) + g_ChrRecords` every time.
   `x -= 6` is the LAST statement of the body: x crosses calls, so sched1
   may lift it over them, and it ends up in the first MapStatusColGet call's
   delay slot. The colour is cast (u16): CellNumberSet's sixth parameter is
   a u16 (cell_text.c's prototype) and no prototype is in scope here. The
   callees are called undeclared.

   func_800572D8() is called in the else arm only: called on every path
   (`return func_800572D8();` after the if) it loses the target's `j` over
   the else arm, and an `s8` column variable adds an sll/sra pair that
   hides the difference in the instruction count (the two together are
   152/155). */
/* func_80056408 */
void MapStatusDisp(void)
{
    s32 i;
    s32 x;
    s32 id;

    if (g_MapStatusRequest != 0) {
        CellRowsClear(0x16, 4, 0x20);
        x = 0x20;
        if (g_MapStatusRequest == 1) {
            for (i = 5; i >= 0; i--) {
                id = *(u16 *)(i * 2 + g_PartyIds);
                if (id != 0 && id < 0xC) {
                    CellChrGlyphSet(id, x, 0x16, 0xDEAD2001, 0x3000, 0);
                    CellGlyphSet(0xA, x, 0x17, 0xDEAD2001, 0x3000, 0);
                    CellGlyphSet(0xB, x + 1, 0x17, 0xDEAD2001, 0x3000, 0);
                    CellNumberSet(*(u8 *)((id << 6) + g_ChrRecords + 0x1C), x + 2, 0x17, 2, 0xDEAD2001, 0x3000, 0, 0);
                    CellNumberSet(*(u16 *)((id << 6) + g_ChrRecords), x + 1, 0x18, 3, 0xDEAD2001,
                                  (u16)MapStatusColGet(*(u16 *)((id << 6) + g_ChrRecords), *(u16 *)((id << 6) + g_ChrRecords + 2)), 0, 0);
                    CellNumberSet(*(u16 *)((id << 6) + g_ChrRecords + 4), x + 1, 0x19, 3, 0xDEAD2001,
                                  (u16)MapStatusColGet(*(u16 *)((id << 6) + g_ChrRecords + 4), *(u16 *)((id << 6) + g_ChrRecords + 6)), 0, 0);
                    x -= 6;
                }
            }
            CellGlyphSet(0x5C, x + 2, 0x18, 0xDEAD2001, 0x3000, 0);
            CellGlyphSet(0x5E, x + 3, 0x18, 0xDEAD2001, 0x3000, 0);
            CellGlyphSet(0x5D, x + 2, 0x19, 0xDEAD2001, 0x3000, 0);
            CellGlyphSet(0x5E, x + 3, 0x19, 0xDEAD2001, 0x3000, 0);
        } else {
            func_800572D8();
        }
    }
}

/* func_80056674 */
int MapStatusColGet(int current, int maximum)
{
    if (current == maximum)
        return 0x2000;

    maximum >>= 1;

    if (maximum < current)
        goto L3000;

    maximum >>= 1;

    return (maximum < current) << 12;

L3000:
    return 0x3000;
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty, and nothing reaches it: nothing in the program supports a name. */
void func_800566A4() {
}

/* no_schedule_insns: MapFrameCount */
/* func_800566AC */
void MapFrameCount()
{
    MapBgChangeCopy();
    if (g_PlayTimeFrames >= 0xE10) {
        g_PlayTimeMinutes++;
        if (0xEA5F < g_PlayTimeMinutes) {
            g_PlayTimeMinutes += 0x15A0;
        }
        g_PlayTimeFrames = 0;
    }
    if (g_MapStatusRequest)
        g_MapStatusRequest = 0;
    D_800B8DF0[0]++;
}

/* Ruling of 2026-10-14 (docs/naming.md): Adds 1 to a word nothing reads, and nothing reaches it: nothing says what it counts. */
void func_80056738(void)
{
  return ++D_800B8DF0[1];
}
