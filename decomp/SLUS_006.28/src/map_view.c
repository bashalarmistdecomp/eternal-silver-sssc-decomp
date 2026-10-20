/* Lunar SSSC — map_view: the map's view and second view: their goals, steps
   and scrolls. The first split had it in scripting_e.c. Every function is
   C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "game.h"
#include "pad.h"
#include "bg.h"
#include "map.h"
#include "sprite.h"
extern s32 D_800B8EDC;

extern s16 g_MapLimitLeft;
extern s16 g_MapLimitTop;
extern s16 g_MapLimitRight;
extern s16 g_MapLimitBottom;
extern long HWD0;
extern long VWD0;
extern s8 g_PadActuatorLevels;
extern u8 g_PadActuator1Level;
extern s32 g_GameOptions;
extern u16 g_MapViewShakeCount;
extern s16 g_MapViewX;
extern s16 g_MapViewY;
extern u16 g_MapViewShakeWait;
extern u16 g_MapViewShakeX;
extern u16 g_MapViewShakeY;
extern u16 g_MapEffect;
extern u16 g_MapViewShakeOffsetX;
extern u16 g_MapViewShakeOffsetY;
extern s16 g_MapViewKeptX;
extern s16 g_MapViewKeptY;
extern s16 g_MapSecondViewX;
extern s16 g_MapSecondViewY;
extern s32 g_MapPartyRecords;
s32 MapViewStep(s32, s32);
s32 bg_second_scroll_set(s16, s16);
void MapSecondViewScrollSet(void);
void func_800692CC();
void MapViewScrollSet(s32 second_view_follow);
extern s32 (*vtbl_4658_table[])();
s32 MapViewStep(s32, s32);
s32 bg_second_scroll_set(s16, s16);
void MapSecondViewScrollSet(void);
void func_800692CC();
void MapViewScrollSet(s32 second_view_follow);
extern long HWD0, VWD0;
s32 MapViewStep(s32, s32);
s32 bg_second_scroll_set(s16, s16);
void MapSecondViewScrollSet(void);
void func_800692CC();
extern s16 g_MapSecondViewSpeedX;
extern s16 g_MapSecondViewSpeedY;

void MapViewGoalGet(u16 *goal);  /* TU defines this below */
extern s16 g_MapSecondViewKeptX;
extern s16 g_MapSecondViewKeptY;

/* func_80068DB8 */
s32 MapViewStep(s32 view_follow, s32 second_view_follow)
{
    u16 local[2];
    s32 result;

    if (view_follow) {
        MapViewGoalGet(local);
        g_MapViewX = local[0];
        g_MapViewY = local[1];
    }

    MapViewScrollSet(second_view_follow);

    if ((g_MapEffect & 3) == 3) {
        MapSecondViewGoalGet(local);
        if (g_MapSecondViewX != local[0] || g_MapSecondViewY != local[1]) {
            g_MapSecondViewX = local[0];
            g_MapSecondViewY = local[1];
            MapSecondViewScrollSet();
            D_800B8EDC = 1;
        }
    }

    if (g_MapEffect & 0xC) {
        func_800692CC(1);
    }

    result = (u16)g_MapViewX;
    g_MapViewKeptX = result;
    g_MapViewKeptY = g_MapViewY;
    g_MapSecondViewKeptX = g_MapSecondViewX;
    g_MapSecondViewKeptY = g_MapSecondViewY;
    return result;
}

/* far_syms: g_PadActuatorLevels (target reaches it %hi/%lo) */
/* far_syms: g_PadActuator1Level (target reaches it %hi/%lo) */
/* far_syms: g_PadActuatorLevels (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadActuator1Level (auto: target reaches it %hi/%lo — addressing row 3) */

/* func_80068E98 */
void MapViewScrollSet(s32 second_view_follow)
{
    u16 local[2];

    if (second_view_follow && (g_MapViewKeptX != g_MapViewX || g_MapViewKeptY != g_MapViewY)) {
        if ((g_MapEffect & 3) != 3) {
            MapSecondViewGoalGet(local);
            g_MapSecondViewX = local[0];
            g_MapSecondViewY = local[1];
        }
    }
    MapSecondViewScrollSet();
    if (g_MapViewShakeCount != 0) {
        if (((GameOptions *)g_GameOptions)->actuators != 0) {
            g_PadActuator1Level = 0xC0;
        }
        if (g_MapViewShakeWait == 0) {
            g_MapViewShakeCount--;
            if (g_MapViewShakeCount & 1) {
                g_MapViewShakeOffsetX = g_MapViewShakeX;
                g_MapViewShakeOffsetY = g_MapViewShakeY;
                if (((GameOptions *)g_GameOptions)->actuators != 0) {
                    g_PadActuator1Level += rng_draw() & 0x3F;
                    g_PadActuatorLevels = rng_draw() != 0;
                }
                if (rng_draw() & 1) {
                    g_MapViewShakeOffsetX = -g_MapViewShakeOffsetX;
                }
                if (rng_draw() & 1) {
                    g_MapViewShakeOffsetY = -g_MapViewShakeOffsetY;
                }
            } else {
                g_MapViewShakeOffsetY = 0;
                g_MapViewShakeOffsetX = 0;
                g_PadActuatorLevels = 0;
                if (g_MapViewShakeCount == 0) {
                    g_PadActuator1Level = 0;
                }
            }
            g_MapViewShakeWait = 2;
        }
        g_MapViewShakeWait--;
    }
    bg_scroll_set(g_MapViewX + (s16)g_MapViewShakeOffsetX, g_MapViewY + (s16)g_MapViewShakeOffsetY);
}

s32 bg_second_scroll_set(s16, s16);                          /* extern */

/* func_80069058 */
void MapSecondViewScrollSet(void) {
    bg_second_scroll_set(g_MapSecondViewX, g_MapSecondViewY);
}

/* func_80069080 */
void MapViewGoalGet(u16 *goal) {
    u16 *ptr;

    ptr = (u16 *)g_MapPartyRecords;
    goal[0] = ptr[3];
    goal[1] = ptr[4];
    if (goal[0] < (s16)g_MapLimitLeft + 0xA0) {
        goal[0] = g_MapLimitLeft;
    }
    else if ((s16)g_MapLimitRight - 0xA0 < goal[0]) {
        goal[0] = (s16)g_MapLimitRight - 0x140;
    }
    else {
        goal[0] = goal[0] - 0xA0;
    }
    if (goal[1] < (s16)g_MapLimitTop + 0x78) {
        goal[1] = g_MapLimitTop;
    }
    else if ((s16)g_MapLimitBottom - 0x68 < goal[1]) {
        goal[1] = (s16)g_MapLimitBottom - 0xE0;
    }
    else {
        goal[1] = goal[1] - 0x78;
    }
}
extern u32 g_MapSecondViewDriftCount;

/* MapSecondViewGoalGet -- the second scroll's GOAL: the position the
   second background layer (g_MapSecondViewX / g_MapSecondViewY) should take, written to *goal, by the
   scroll mode in the low two bits of the map-effect flags g_MapEffect
   (MapEffectSet sets them from the map header's +0xB). No path
   clamps anything. Three callers: MapViewStep (mode 3, every call) and
   MapViewScrollSet (the other modes, when the camera g_MapViewX / g_MapViewY
   has left the last recorded g_MapViewKeptX / g_MapViewKeptY), both with a u16[2]
   on their stack, and MoveChrFollowStart (lns_move.c, mask bit 17:
   the goal becomes mover slot 17's destination).

     mode 0  nothing is written (MapViewScrollSet then stores its own
             uninitialised pair)
     mode 1  goal = the camera (g_MapViewX, g_MapViewY); D_800B8EDC = 1
     mode 2  flag 0x40 set: goal = camera / 2 (signed, toward zero);
             clear: goal = camera * 2; D_800B8EDC = 1
     mode 3  goal = second scroll + (camera - last camera) + drift.
             The call counter g_MapSecondViewDriftCount is incremented first. On an odd
             count, per axis with the speed v = g_MapSecondViewSpeedX (x) /
             g_MapSecondViewSpeedY (y) (MapEffectSet: the s8 table g_MapSecondViewSpeeds,
             -8..8): v outside -1..1 adds v / 2; v inside adds v, and
             only once the counter has reached 3. Then a counter >= 3 is
             reset to 0 -- so it runs 1, 2, 3: a fast axis moves v / 2 on
             counts 1 and 3, a slow one v on count 3, v per three calls
             either way. D_800B8EDC is left to MapViewStep.

   Matching: goal as a pointer to a two-s16 struct -- a member store
   (MEM_IN_STRUCT) does not conflict with the scalar loads, so both loads
   are scheduled ahead of both stores and modes 1 and 3 cross-jump on the
   store pair; through `s16 *` the goal[0] store pins the second load
   behind it. ONE s16 local per axis, updated in place: cse folds the x
   uses onto the SImode difference inside its extended block (hence two
   registers and the copy in the delay slot), the y uses lie past a join
   label (one register). A separate dx local reschedules mode 3's first
   block, which moves three registers (86/103); s32 locals load the
   globals sign-extended. */
/* func_80069130 */
void MapSecondViewGoalGet(MapSpot *goal)
{
    s32 mode;
    s16 x;
    s16 y;

    mode = g_MapEffect & 3;
    if (mode == 1) {
        goal->x = g_MapViewX;
        goal->y = g_MapViewY;
        D_800B8EDC = 1;
    } else if (mode == 2) {
        if (g_MapEffect & 0x40) {
            goal->x = g_MapViewX / 2;
            goal->y = g_MapViewY / 2;
        } else {
            goal->x = g_MapViewX * 2;
            goal->y = g_MapViewY * 2;
        }
        D_800B8EDC = 1;
    } else if (mode == 3) {
        x = g_MapViewX - g_MapViewKeptX;
        y = g_MapViewY - g_MapViewKeptY;
        g_MapSecondViewDriftCount++;
        if (g_MapSecondViewDriftCount & 1) {
            if ((u16)(g_MapSecondViewSpeedX + 1) >= 3) {
                x += g_MapSecondViewSpeedX / 2;
            } else if (g_MapSecondViewDriftCount >= 3) {
                x += g_MapSecondViewSpeedX;
            }
            if ((u16)(g_MapSecondViewSpeedY + 1) >= 3) {
                y += g_MapSecondViewSpeedY / 2;
            } else if (g_MapSecondViewDriftCount >= 3) {
                y += g_MapSecondViewSpeedY;
            }
            if (g_MapSecondViewDriftCount >= 3) {
                g_MapSecondViewDriftCount = 0;
            }
        }
        goal->x = g_MapSecondViewX + x;
        goal->y = g_MapSecondViewY + y;
    }
}

/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its two callers pass 0 and 1 by an effect flag, which does not say what for. */
void func_800692CC() {

}
