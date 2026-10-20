/* Lunar SSSC — btl_frame: one frame of the battle and slot 5's roam it
   steps. The first split had it in game_state.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

/* far_syms: g_BtlRecords */
/* far_syms: g_BtlWalkBlock[0] (target reaches it %hi/%lo) */
/* far_syms: g_PadActuatorListWait (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadActuatorList (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_PadActuatorRestFlags (auto: target reaches it %hi/%lo — addressing row 3) */
/* far_syms: g_BtlRecords g_PadActuatorList g_PadActuatorRestFlags g_PadActuatorListWait */
/* far_syms: g_PadActuatorListWait */
/* far_syms: g_PadActuatorList */
/* far_syms: g_PadActuatorRestFlags */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "chr.h"
#include "btl.h"
extern u8 g_BtlSlot5Roaming;
s32 SoundPlay(s32, s32);
extern u8 g_BtlActuatorFFList[];
extern s16 g_PadActuatorListWait;
extern s32 g_PadActuatorList;
extern s8 g_PadActuatorRestFlags;
extern long HWD0;
extern long VWD0;
extern s32 g_BtlRecords;
 
extern u8 g_BtlSlotIds[];
s32 BtlFrameStep();
s32 rng_draw();
s32 BtlFrameStep();
s32 rng_draw();
s32 BtlAnimationSet();
extern long HWD0, VWD0;
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
/* far_syms: g_BtlSlotIds (target reaches it %hi/%lo) */
/* far_syms: g_BtlRecords (auto: target reaches it %hi/%lo — addressing row 3) */
s32 BtlFrameStep();
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern long HWD0, VWD0;
extern void (*g_BtlActingShowFunctions[])(void);
s32 BtlPadHeldCheck(s32 button_mask);  /* extern */

/* far_syms: g_PadActuatorLevels g_PadActuator1Level g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlRecords g_BtlAutoEndAllowed g_BtlPartyMode */
extern s8 g_PadActuatorLevels;
extern s8 g_PadActuator1Level;
extern s32 g_PadPressedButtons;
extern s32 g_GameOptions;
extern u8 g_BtlAutoEndAllowed;
extern u8 g_BtlPartyMode;
void BtlWindow1IconClose();
s32 BtlSlot5RoamStep();
void BtlMixFlashStep();
void BtlAnimationStep();
void BtlExtraRecordsStep();
void BtlAmountRecordsStep();
void BtlMenuOutlineStep();
void BtlShowVoiceStep();
void BtlFrameDraw();
void tile_layer_step();
s32 sound_music_level_step();

/* func_80038D2C */
s32 BtlFrameStep()
{

	if (g_PadPressedButtons == 2)
	{
		((u8 *)g_GameOptions)[5] ^= 1;

		if (((u8 *)g_GameOptions)[5])
		{
			g_PadActuatorList = (s32)g_BtlActuatorFFList;
			g_PadActuatorRestFlags = 0x3F;
			g_PadActuatorListWait = 0;
		}
		else
		{
			g_PadActuatorList = 0;
			g_PadActuatorRestFlags = 0;
			g_PadActuatorListWait = 0;
			g_PadActuatorLevels = 0;
			g_PadActuator1Level = 0;
		}

		SoundPlay(3 - ((u8 *)g_GameOptions)[5], 1);
	}

	if (g_PadPressedButtons == 1)
	{
		((u8 *)g_GameOptions)[6] ^= 1;
		SoundPlay(3 - ((u8 *)g_GameOptions)[6], 1);
	}

	if (g_BtlAutoEndAllowed)
	{
		if (g_BtlPartyMode == 1)
		{
			if (BtlPadHeldCheck(0x10) & 0xFF)
			{
				g_BtlPartyMode = 0;
				BtlWindow1IconClose();
			}
		}
	}

	if (g_BtlSlotIds[5] == 0xC)
	{
		BtlSlot5RoamStep();
	}

	BtlMixFlashStep();
	BtlAnimationStep(g_BtlRecords, 0, 0x1C);
	BtlExtraRecordsStep();
	BtlAmountRecordsStep();
	BtlMenuOutlineStep();
	BtlShowVoiceStep();
	BtlFrameDraw();
	tile_layer_step();

	return sound_music_level_step();
}
extern u8 g_BtlSlot5RoamYWait;
extern u16 g_BtlSlot5RoamAnimationWait;
extern u16 g_BtlSlot5RoamXCount;
extern u16 g_BtlSlot5RoamYCount;
extern u8 g_BtlSlot5RoamAnimationState;
extern u8 g_BtlSlot5RoamXState;
extern u8 g_BtlSlot5RoamYState;

/* func_80038EE8 */
s32 BtlSlot5RoamStep(void)
{
    BtlRecord *p;

    if (g_BtlSlot5Roaming != 0) {
        switch (g_BtlSlot5RoamAnimationState) {
        case 0:
            if (((BtlRecord *)g_BtlRecords)[5].animation == 0) {
                if (rng_draw() & 1) {
                    BtlAnimationSet(g_BtlRecords + 0x140, 2);
                    g_BtlSlot5RoamAnimationState = 1;
                    g_BtlSlot5RoamAnimationWait = (((BtlRecord *)g_BtlRecords)[5].end & 0x7FFF) - 1;
                } else {
                    g_BtlSlot5RoamAnimationWait = rng_draw() & 0x7F;
                    g_BtlSlot5RoamAnimationState = 3;
                }
            } else {
                if (rng_draw() & 1) {
                    BtlAnimationSet(g_BtlRecords + 0x140, 3);
                    g_BtlSlot5RoamAnimationState = 2;
                    g_BtlSlot5RoamAnimationWait = (((BtlRecord *)g_BtlRecords)[5].end & 0x7FFF) - 1;
                } else {
                    g_BtlSlot5RoamAnimationWait = rng_draw() & 0x7F;
                    g_BtlSlot5RoamAnimationState = 3;
                }
            }
            break;
        case 1:
            p = (BtlRecord *)g_BtlRecords + 5;
            if (((BtlRecord *)g_BtlRecords)[5].step == g_BtlSlot5RoamAnimationWait) {
                BtlAnimationSet(p, 1);
                g_BtlSlot5RoamAnimationState = 0;
            }
            break;
        case 2:
            p = (BtlRecord *)g_BtlRecords + 5;
            if (((BtlRecord *)g_BtlRecords)[5].step == g_BtlSlot5RoamAnimationWait) {
                BtlAnimationSet(p, 0);
                g_BtlSlot5RoamAnimationState = 0;
            }
            break;
        case 3:
            if (--g_BtlSlot5RoamAnimationWait == 0xFFFF) {
                g_BtlSlot5RoamAnimationState = 0;
            }
            break;
        }

        switch (g_BtlSlot5RoamXState) {
        case 0:
            switch (rng_draw() & 3) {
            case 0:
                ((BtlRecord *)g_BtlRecords)[5].sprite.attr = 0x20;
                g_BtlSlot5RoamXCount = rng_draw() & 0x7F;
                g_BtlSlot5RoamXState = 1;
                break;
            case 1:
                ((BtlRecord *)g_BtlRecords)[5].sprite.attr = 0x2020;
                g_BtlSlot5RoamXCount = rng_draw() & 0x7F;
                g_BtlSlot5RoamXState = 2;
                break;
            case 2:
            case 3:
                g_BtlSlot5RoamXCount = rng_draw() & 0x7F;
                g_BtlSlot5RoamXState = 3;
                break;
            }
            break;
        case 1:
            if (((BtlRecord *)g_BtlRecords)[5].sprite.x < 0x130) {
                ((BtlRecord *)g_BtlRecords)[5].sprite.x++;
                if (--g_BtlSlot5RoamXCount == 0xFFFF) {
                    g_BtlSlot5RoamXState = 0;
                }
            } else {
                g_BtlSlot5RoamXState = 0;
            }
            break;
        case 2:
            if (((BtlRecord *)g_BtlRecords)[5].sprite.x >= 0x11) {
                ((BtlRecord *)g_BtlRecords)[5].sprite.x--;
                if (--g_BtlSlot5RoamXCount == 0xFFFF) {
                    g_BtlSlot5RoamXState = 0;
                }
            } else {
                g_BtlSlot5RoamXState = 0;
            }
            break;
        case 3:
            if (--g_BtlSlot5RoamXCount == 0xFFFF) {
                g_BtlSlot5RoamXState = 0;
            }
            break;
        }

        switch (g_BtlSlot5RoamYState) {
        case 0:
            switch (rng_draw() & 3) {
            case 0:
                g_BtlSlot5RoamYCount = rng_draw() & 0xF;
                g_BtlSlot5RoamYWait = 7;
                g_BtlSlot5RoamYState = 1;
                break;
            case 1:
                g_BtlSlot5RoamYCount = rng_draw() & 0xF;
                g_BtlSlot5RoamYWait = 7;
                g_BtlSlot5RoamYState = 2;
                break;
            case 2:
            case 3:
                g_BtlSlot5RoamYCount = rng_draw() & 0x7F;
                g_BtlSlot5RoamYState = 3;
                break;
            }
            break;
        case 1:
            if (((BtlRecord *)g_BtlRecords)[5].sprite.y >= 0x59) {
                if (--g_BtlSlot5RoamYWait == 0) {
                    g_BtlSlot5RoamYWait = 7;
                    ((BtlRecord *)g_BtlRecords)[5].sprite.y--;
                    if (--g_BtlSlot5RoamYCount == 0xFFFF) {
                        g_BtlSlot5RoamYState = 0;
                    }
                }
            } else {
                g_BtlSlot5RoamYState = 0;
            }
            break;
        case 2:
            if (((BtlRecord *)g_BtlRecords)[5].sprite.y < 0x68) {
                if (--g_BtlSlot5RoamYWait == 0) {
                    g_BtlSlot5RoamYWait = 7;
                    ((BtlRecord *)g_BtlRecords)[5].sprite.y++;
                    if (--g_BtlSlot5RoamYCount == 0xFFFF) {
                        g_BtlSlot5RoamYState = 0;
                    }
                }
            } else {
                g_BtlSlot5RoamYState = 0;
            }
            break;
        case 3:
            if (--g_BtlSlot5RoamYCount == 0xFFFF) {
                g_BtlSlot5RoamYState = 0;
            }
            break;
        }
    }
}
