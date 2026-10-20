/* Lunar SSSC — pad_update: the pad's work for one frame: the actuators'
   command list and the reading of both ports. The first split had it in
   ui.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
s32 rng_draw(void);

/* load_hazard_nop: pad_update */
/* la_absolute: pad_update */
/* far_syms: g_GameOptions */
extern u8 g_PadActuatorLevels[2];
extern u8 g_PadActuatorAlignedFlags[2];
extern u8 g_PadActuatorAlignBytes[6];
extern u8 g_PadActuatorIntervals[2];
extern u8 g_PadActuatorWays[2];
extern u8 g_PadActuatorUpperLimits[2];
extern u8 g_PadActuatorLowerLimits[2];
extern u8 g_PadActuatorSteps[2];
extern s16 g_PadActuatorCountdowns[2];
extern s16 g_PadActuatorListWait;
extern u8 *g_PadActuatorList;
extern u8 *g_PadActuatorPreviousList;
extern s32 g_PadActuatorListWaitFrame;
extern s32 g_PadActuatorListRepeatsLeft;
extern u8 *g_PadActuatorListRepeatStart;
extern u8 g_PadActuatorRestFlags;
extern u8 *g_PadPortBuffer;
extern s32 D_800B89FC;
extern s32 g_PadPreviousButtons;
extern s32 g_PadPressedButtons;
extern s32 g_PadButtons;
extern s32 g_MovieFrameNumber;
extern u8 g_PadPort00Buffer[];
extern u8 g_PadPort10Buffer[];
extern u8 D_800E0AE8[];
extern u8 *g_GameOptions;
s32 PadGetState(s32);
s32 PadInfoMode(s32, s32, s32);
s32 PadSetActAlign(s32, u8 *);

/* func_8001F508 */
void pad_update(void)
{
  s32 v;
  s32 i;
  s32 idx;
  s32 state;
  s32 buttons;
  s32 shift;
  u8 *q;
  s32 prev;
  u8 *p;
  u8 cmd;
  s16 *tm;
  g_PadButtons = 0;
  g_PadPortBuffer = g_PadPort00Buffer;
  if (g_PadActuatorRestFlags & 0x10)
  {
    if (g_PadActuatorRestFlags & 0x12)
    {
      g_PadActuatorLevels[0] = 0;
      g_PadActuatorCountdowns[0] = 0;
      g_PadActuatorIntervals[0] = 0;
      g_PadActuatorUpperLimits[0] = 0xFF;
      g_PadActuatorLowerLimits[0] = 0;
      g_PadActuatorSteps[0] = 0;
      g_PadActuatorWays[0] = 0x40;
    }
    if (g_PadActuatorRestFlags & 0x11)
    {
      g_PadActuatorLevels[1] = 0;
      g_PadActuatorCountdowns[1] = 0;
      g_PadActuatorIntervals[1] = 0;
      g_PadActuatorUpperLimits[1] = 0xFF;
      g_PadActuatorLowerLimits[1] = 0;
      g_PadActuatorSteps[1] = 0;
      g_PadActuatorWays[1] = 0x40;
    }
    g_PadActuatorRestFlags &= 0xEC;
  }
  if (g_PadActuatorList != 0)
  {
    if (g_PadActuatorListWait > 0)
    {
      if (g_PadActuatorListWaitFrame == 0)
      {
        g_PadActuatorListWait--;
      }
      else
        if (g_MovieFrameNumber >= g_PadActuatorListWaitFrame)
      {
        g_PadActuatorListWaitFrame = 0;
        g_PadActuatorListWait = 0;
      }
    }
    else
      if (g_PadActuatorListWait == 0)
    {
      if ((*g_PadActuatorList) != 0)
      {
        while (((g_PadActuatorList != 0) && ((*g_PadActuatorList) != 0)) && (g_PadActuatorListWait == 0))
        {
          p = g_PadActuatorList;
          cmd = *p;
          switch (cmd & 0xF0)
          {
            case 0x10:
              {
                u8 *tq = g_PadActuatorLevels;
                tq[cmd & 1] = p[1];
              }
              if ((*g_PadActuatorList) == 3)
            {
              g_PadActuatorLevels[0] = g_PadActuatorList[1];
            }
              g_PadActuatorList += 2;
              break;

            case 0x20:
              {
                u8 *tq = g_PadActuatorUpperLimits;
                tq[cmd & 1] = p[1];
              }
              if ((*g_PadActuatorList) == 3)
            {
              g_PadActuatorUpperLimits[0] = g_PadActuatorList[1];
            }
              g_PadActuatorList += 2;
              break;

            case 0x30:
              {
                u8 *tq = g_PadActuatorLowerLimits;
                tq[cmd & 1] = p[1];
              }
              if ((*g_PadActuatorList) == 3)
            {
              g_PadActuatorLowerLimits[0] = g_PadActuatorList[1];
            }
              g_PadActuatorList += 2;
              break;

            case 0x50:
              g_PadActuatorIntervals[cmd & 1] = p[1];
              g_PadActuatorCountdowns[(*g_PadActuatorList) & 1] = g_PadActuatorIntervals[(*g_PadActuatorList) & 1];
              if ((*g_PadActuatorList) == 3)
            {
              g_PadActuatorIntervals[0] = g_PadActuatorList[1];
              g_PadActuatorCountdowns[0] = g_PadActuatorIntervals[(*g_PadActuatorList) & 1];
            }
              g_PadActuatorList += 2;
              break;

            case 0x40:
              {
                u8 *tq = g_PadActuatorWays;
                tq[cmd & 1] = cmd & 0x4E;
              }
              g_PadActuatorList += 1;
              break;

            case 0x00:
              g_PadActuatorList = 0;
              g_PadActuatorListWait = 0;
              break;

            case 0x60:
              g_PadActuatorCountdowns[cmd & 1] = g_PadActuatorIntervals[cmd & 1];
              g_PadActuatorListWait = p[1];
              if ((*p) == 3)
            {
              g_PadActuatorCountdowns[0] = g_PadActuatorIntervals[(*p) & 1];
              g_PadActuatorListWait = p[1];
            }
              g_PadActuatorList = p + 2;
              break;

            case 0xA0:
              g_PadActuatorCountdowns[cmd & 1] = g_PadActuatorIntervals[cmd & 1];
              g_PadActuatorListWaitFrame = (p[1] << 8) + p[2];
              if ((*p) == 3)
            {
              g_PadActuatorCountdowns[0] = g_PadActuatorIntervals[(*p) & 1];
            }
              g_PadActuatorList = p + 3;
              g_PadActuatorListWait = 1;
              break;

            case 0x80:
              {
                u8 *tq = g_PadActuatorSteps;
                tq[cmd & 1] = p[1];
              }
              if ((*g_PadActuatorList) == 3)
            {
              g_PadActuatorSteps[0] = g_PadActuatorList[1];
            }
              g_PadActuatorList += 2;
              break;

            case 0x70:

            case 0x90:
              if ((*g_PadActuatorList) & 2)
            {
              g_PadActuatorLevels[0] = 0;
              g_PadActuatorCountdowns[0] = 0;
              g_PadActuatorIntervals[0] = 0;
              g_PadActuatorUpperLimits[0] = 0xFF;
              g_PadActuatorLowerLimits[0] = 0;
              g_PadActuatorSteps[0] = 0;
              g_PadActuatorWays[0] = 0x40;
            }
              if ((*g_PadActuatorList) & 1)
            {
              g_PadActuatorLevels[1] = 0;
              g_PadActuatorCountdowns[1] = 0;
              g_PadActuatorIntervals[1] = 0;
              g_PadActuatorUpperLimits[1] = 0xFF;
              g_PadActuatorLowerLimits[1] = 0;
              g_PadActuatorSteps[1] = 0;
              g_PadActuatorWays[1] = 0x40;
            }
              g_PadActuatorList += 1;
              break;

            case 0xC0:
              g_PadActuatorList = p + 1;
              g_PadActuatorListRepeatsLeft = *(g_PadActuatorList++);
              g_PadActuatorListRepeatStart = g_PadActuatorList;
              break;

            case 0xB0:
              if (g_PadActuatorListRepeatsLeft != 0)
            {
              g_PadActuatorListRepeatsLeft--;
              g_PadActuatorList = g_PadActuatorListRepeatStart;
            }
            else
            {
              g_PadActuatorList = p + 1;
            }
              break;

          }

        }

      }
      else
      {
        g_PadActuatorList = 0;
        g_PadActuatorListWait = 0;
        if (g_PadActuatorRestFlags & 0x20)
        {
          if (g_PadActuatorRestFlags & 0x28)
          {
            g_PadActuatorLevels[0] = 0;
            g_PadActuatorCountdowns[0] = 0;
            g_PadActuatorIntervals[0] = 0;
            g_PadActuatorUpperLimits[0] = 0xFF;
            g_PadActuatorLowerLimits[0] = 0;
            g_PadActuatorSteps[0] = 0;
            g_PadActuatorWays[0] = 0x40;
          }
          if (g_PadActuatorRestFlags & 0x24)
          {
            g_PadActuatorLevels[1] = 0;
            g_PadActuatorCountdowns[1] = 0;
            g_PadActuatorIntervals[1] = 0;
            g_PadActuatorUpperLimits[1] = 0xFF;
            g_PadActuatorLowerLimits[1] = 0;
            g_PadActuatorSteps[1] = 0;
            g_PadActuatorWays[1] = 0x40;
          }
          g_PadActuatorRestFlags &= 0xD3;
        }
      }
    }
    if (g_PadActuatorListWait > 0)
    {
      for (i = 0; i < 2; i++)
      {
        tm = g_PadActuatorCountdowns;
        if (tm[i] > 0)
        {
          tm[i]--;
        }
        else
        {
          tm[i] = g_PadActuatorIntervals[i];
          switch (g_PadActuatorWays[i])
          {
            case 0x40:
              g_PadActuatorLevels[i] = 0;
              break;

            case 0x44:
              v = g_PadActuatorLevels[i] + g_PadActuatorSteps[i];
              if (v < g_PadActuatorUpperLimits[i])
            {
              g_PadActuatorLevels[i] = v;
            }
            else
            {
              g_PadActuatorLevels[i] = g_PadActuatorUpperLimits[i];
            }
              break;

            case 0x46:
              v = g_PadActuatorLevels[i] - g_PadActuatorSteps[i];
              if (g_PadActuatorLowerLimits[i] < v)
            {
              g_PadActuatorLevels[i] = v;
            }
            else
            {
              g_PadActuatorLevels[i] = g_PadActuatorLowerLimits[i];
            }
              break;

            case 0x4A:
              if (g_PadActuatorLevels[i] != 0)
            {
              g_PadActuatorLevels[i] = g_PadActuatorLowerLimits[i];
            }
            else
            {
              g_PadActuatorLevels[i] = g_PadActuatorUpperLimits[i];
            }
              break;

            case 0x4C:
              if (g_PadActuatorLevels[i] == g_PadActuatorUpperLimits[i])
            {
              g_PadActuatorLevels[i] = g_PadActuatorLowerLimits[i];
            }
            else
            {
              g_PadActuatorLevels[i] = g_PadActuatorUpperLimits[i];
            }
              break;

            case 0x48:
              g_PadActuatorLevels[i] += (rng_draw() % (g_PadActuatorSteps[i] * 2)) - g_PadActuatorSteps[i];
              if (g_PadActuatorLevels[i] < g_PadActuatorLowerLimits[i])
            {
              g_PadActuatorLevels[i] = g_PadActuatorUpperLimits[i];
            }
            else
              if (g_PadActuatorUpperLimits[i] < g_PadActuatorLevels[i])
            {
              g_PadActuatorLevels[i] = g_PadActuatorLowerLimits[i];
            }
              break;

          }

        }
      }

    }
  }
  else
    if (g_PadActuatorPreviousList != 0)
  {
    if (g_PadActuatorRestFlags & 0x28)
    {
      g_PadActuatorLevels[0] = 0;
      g_PadActuatorCountdowns[0] = 0;
      g_PadActuatorIntervals[0] = 0;
      g_PadActuatorUpperLimits[0] = 0xFF;
      g_PadActuatorLowerLimits[0] = 0;
      g_PadActuatorSteps[0] = 0;
      g_PadActuatorWays[0] = 0x40;
    }
    if (g_PadActuatorRestFlags & 0x24)
    {
      g_PadActuatorLevels[1] = 0;
      g_PadActuatorCountdowns[1] = 0;
      g_PadActuatorIntervals[1] = 0;
      g_PadActuatorUpperLimits[1] = 0xFF;
      g_PadActuatorLowerLimits[1] = 0;
      g_PadActuatorSteps[1] = 0;
      g_PadActuatorWays[1] = 0x40;
    }
    g_PadActuatorRestFlags &= 0xD3;
  }
  g_PadActuatorPreviousList = g_PadActuatorList;
  shift = 0;
  if (g_GameOptions[5] == 0)
  {
    g_PadActuatorLevels[0] = 0;
    g_PadActuatorLevels[1] = 0;
  }
  g_PadPortBuffer = g_PadPort10Buffer;
  for (i = 0x10, idx = 1; i >= 0; idx--, i -= 0x10)
  {
    state = PadGetState(i);
    if (state == 0)
    {
      if (i > 0)
      {
        g_PadButtons = 0;
        buttons = 0xFFFF;
      }
    }
    else
    {
      switch (PadInfoMode(i, 1, 0))
      {
        case 4:
          D_800E0AE8[idx * 0xA0] = 0;
          buttons = g_PadPortBuffer[3] | (g_PadPortBuffer[2] << 8);
          if (buttons != 0xFFFF)
        {
          g_PadButtons = (buttons ^ 0xFFFF) << shift;
        }
          break;

        case 5:
          D_800E0AE8[idx * 0xA0] = 0;
          if (g_PadPortBuffer[6] < 0x50)
        {
          buttons &= 0x7FFF;
        }
        else
          if (g_PadPortBuffer[6] >= 0xB1)
        {
          buttons &= 0xDFFF;
        }
          if (g_PadPortBuffer[7] < 0x50)
        {
          buttons &= 0xEFFF;
        }
        else
          if (g_PadPortBuffer[7] >= 0xB1)
        {
          buttons &= 0xBFFF;
        }
          if (buttons != 0xFFFF)
        {
          g_PadButtons = (buttons ^ 0xFFFF) << shift;
        }
          break;

        case 7:
          D_800E0AE8[idx * 0xA0] = 0;
          buttons = g_PadPortBuffer[3] | (g_PadPortBuffer[2] << 8);
          if (g_PadPortBuffer[6] < 0x50)
        {
          buttons &= 0x7FFF;
        }
        else
          if (g_PadPortBuffer[6] >= 0xB1)
        {
          buttons &= 0xDFFF;
        }
          if (g_PadPortBuffer[7] < 0x50)
        {
          buttons &= 0xEFFF;
        }
        else
          if (g_PadPortBuffer[7] >= 0xB1)
        {
          buttons &= 0xBFFF;
        }
          if (buttons != 0xFFFF)
        {
          g_PadButtons = (buttons ^ 0xFFFF) << shift;
        }
          if (state == 1)
        {
          g_PadActuatorAlignedFlags[idx] = 0;
        }
          q = &g_PadActuatorAlignedFlags[idx];
          if ((*q) == 0)
        {
          PadSetAct(i, g_PadActuatorLevels, 2);
          if (state == 2)
          {
            *q = 1;
          }
          if ((state == 6) && (PadSetActAlign(i, g_PadActuatorAlignBytes) != 0))
          {
            *q = 1;
          }
        }
          PadSetAct(i, g_PadActuatorLevels, 2);
          break;

      }

    }
    shift = 0;
    g_PadPortBuffer = g_PadPort00Buffer;
  }

  D_800B89FC++;
  if (g_PadButtons != 0)
  {
    D_800B89FC = 0;
  }
  prev = g_PadPreviousButtons;
  g_PadPreviousButtons = g_PadButtons;
  g_PadPressedButtons = (~prev) & g_PadButtons;
}
