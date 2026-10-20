/* Lunar SSSC — btl_boss: the bosses' shows: the breaths 0x50 to 0x53 and
   Chaos Wave, Siphon Soul and Fate Storm (0x54 to 0x56), with the party's
   move right they share. The first split had it in save.c. Every function
   is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "path.h"
#include "btl.h"
/* A display block (0x40) as BtlAnimationFramesCount reads it: +0x2C the frame table
   (u16 pairs, +2 = a frame's duration), +0x34 the tick in the frame, +0x36
   the frame's duration, +0x38 the frame index, +0x3A the frame count
   (bit 0x8000 a flag). */
extern BtlShowPulse g_BtlShowPulse;
extern u8 g_BtlSlotActiveFlags[];
extern s32 g_BtlTilePatternList;
extern s32 CharStats;
void BtlPartyRightMove();
extern u8 g_BtlActingSlot;
extern s8 g_PadActuatorLevels;
extern u8 g_PadActuator1Level;
extern u8 *g_GameOptions;
extern u8 g_BtlTargetSlot;
extern s32 g_BtlRecords;
extern u8 g_BtlShowVoiceState;
extern u8 g_BtlActuatorRiseList[];
extern u8 g_BtlShow55WaveFactors[];
extern s32 g_PaletteBuf;
extern s32 g_BtlShowRecords;
extern u8 g_BtlSlotAmountKinds[];
s32 BtlActingRestAnimationSet();                                  /* extern */
void BtlPartyRightMovePatternsRun(u8, u16, u8, u8, u8, u8);              /* extern */
s32 BtlShowPulseSet(s32, s32, s32, s32, s32, s32, s32);         /* extern */
extern u8 g_BtlActuatorFFList[];
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;

extern BtlShowSound g_BtlShowSounds[];

extern u8 g_BtlActuatorFFList[];
s32 BtlAmountShowRun();
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
/* far_syms: g_PadActuatorListWait */
/* far_syms: g_PadActuatorList */
/* far_syms: g_PadActuatorRestFlags */
extern u8 g_BtlSlotIds[];
s32 BtlShowActingPlay(s32, s32);
s32 BtlShowActingEnd();
s32 BtlAnimationSetCopy(s32, s32);
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlAnimationFramesCount(s32 blk);
void layer_mix_switch_on(s32);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_mix_fade_check(s32 layer_mask);
s32 BtlShowFrameStep();
s32 BtlShowSoundStop(s32);

/* far_syms: CharStats g_BtlActuatorRandomList g_PadActuatorLevels g_PadActuator1Level g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlActingSlot g_BtlRecords */
extern u8 g_BtlActuatorRandomList[];
void BtlFacingTowardSet(u8 *, s16);
s32 BtlShowAnimationPlay(s32, s32, s32);
void BtlTileScrollSet(s32, s32, s32);
void mem_move_16(u16 *, u16 *, s32);

/* A battle presentation shared by BtlShow50Run and BtlPartyRightMovePatternsRun: the
   party is swept along a scrolling backdrop. Every live party slot
   (g_BtlSlotActiveFlags[i]) whose status byte (CharStats record +0x15) has none of
   0x3C set turns to face right and takes anim 0x13. The vibration script
   g_BtlActuatorRandomList starts, sounds 0x77 and 0x60 are put on scheduler slots 10
   and 11 (repeating every 0x5C / 0x20 frames), the caster (g_BtlActingSlot)
   plays anim 0xE to its end, the scroll is zeroed and backdrop
   *g_BtlTilePatternList is shown for one frame. Then three phases, one frame per
   pass (BtlTileScrollStep scrolls and runs the frame wait):
     1. while colour ramp 8 (0x20 frames) runs: every slot's x shakes +-1,
        scroll -0x10;
     2. 72 frames: each slot's x moves right (+4 below 0x7E, +2 below 0x9E,
        +1 below 0x11E), shakes +-2, scroll -0x14;
     3. while the ramp back (0x28 frames) runs: as phase 1.
   Each pass also rotates the 16 colours at g_PaletteBuf + 0x220 (`cols`
   holds them twice, so a window starting at t & 0xF is a rotation). The
   end stops the vibration script and both motors, clears the two sound
   slots and puts each of those party slots back in its standing pose
   (BtlSlotActionAnimationSet). The shake and the scroll apply to all five slots, live
   or not. No RNG draw of its own.

   `t` IS READ UNINITIALISED IN THE ORIGINAL: the target never writes $s2
   before `andi $a1, $s2, 0xF`, so the palette phase and the shake's
   parity start from whatever the caller left in $s2. It steps by -0x11
   and -0x15 (both odd, so the parity flips every frame; -1 mod 16 and
   -5 mod 16 for the palette). */
/* func_80086700 */
void BtlPartyRightMove(void)
{
    u16 cols[32];
    u16 i;
    u16 j;
    s32 t;
    u16 *pal;

    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0 && !(*(u8 *)(CharStats + i * 0x3C + 0x15) & 0x3C)) {
            BtlFacingTowardSet((u8 *)(g_BtlRecords + i * 0x40), 0);
            BtlAnimationSetCopy(g_BtlRecords + i * 0x40, 0x13);
        }
    }
    g_PadActuatorList = (s32 *)g_BtlActuatorRandomList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlShowSoundSet(0xA, 0x77, 0x18, 0x5C);
    BtlShowSoundSet(0xB, 0x60, 0x30, 0x20);
    BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 0xE, 0);
    BtlTileScrollSet(8, 0, 0);
    BtlTilePatternPut(*(u8 *)g_BtlTilePatternList, 1);
    layer_mix_fade_start(8, 1, 0x20, 8);
    pal = (u16 *)(g_PaletteBuf + 0x220);
    for (i = 0; i < 0x10; i++) {
        cols[i] = cols[i + 0x10] = pal[i];
    }
    while (layer_mix_fade_check(8) & 0xFF) {
        mem_move_16(pal, &cols[t & 0xF], 0x20);
        for (i = 0; i < 5; i++) {
            if (t & 1) {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x++;
            } else {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x--;
            }
        }
        t -= 0x11;
        BtlTileScrollStep(-0x10, 0);
    }
    for (j = 0; j < 0x48; j++) {
        mem_move_16(pal, &cols[t & 0xF], 0x20);
        for (i = 0; i < 5; i++) {
            if (((BtlRecord *)g_BtlRecords)[i].sprite.x < 0x7E) {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x += 4;
            } else if (((BtlRecord *)g_BtlRecords)[i].sprite.x < 0x9E) {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x += 2;
            } else if (((BtlRecord *)g_BtlRecords)[i].sprite.x < 0x11E) {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x += 1;
            }
            if (t & 1) {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x += 2;
            } else {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x -= 2;
            }
        }
        t -= 0x15;
        BtlTileScrollStep(-0x14, 0);
    }
    layer_mix_fade_start(8, 1, 0x28, 0x1F);
    while (layer_mix_fade_check(8) & 0xFF) {
        mem_move_16(pal, &cols[t & 0xF], 0x20);
        for (i = 0; i < 5; i++) {
            if (t & 1) {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x++;
            } else {
                ((BtlRecord *)g_BtlRecords)[i].sprite.x--;
            }
        }
        t -= 0x11;
        BtlTileScrollStep(-0x10, 0);
    }
    g_PadActuatorList = 0;
    g_PadActuatorRestFlags = 0;
    g_PadActuatorListWait = 0;
    g_PadActuatorLevels = 0;
    g_PadActuator1Level = 0;
    g_BtlShowSounds[11].state = 0;
    g_BtlShowSounds[10].state = 0;
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0 && !(*(u8 *)(CharStats + i * 0x3C + 0x15) & 0x3C)) {
            BtlSlotActionAnimationSet(i & 0xFF);
        }
    }
}
/* far_syms: g_BtlActuatorRandomList g_PadActuatorLevels g_PadActuator1Level g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlActingSlot g_BtlRecords */
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
void BtlAmountShowStart(s32 mode);  /* TU defines this below */
s32 BtlShowPulseClear();
s32 layer_col_fade_check(s32);
s32 BtlAmountShowCheck();
void tile_grid_clear_copy(s32, s32);
s32 BtlTileEnd();
s32 BtlTilePatternStep(u16, s32);

/* The backdrop sweep's long form, called by BtlShow51Run as
   (0, 2, 0x73, 0x74, 0, 0) and by BtlShow52Run as (1, 6, 0, 0, 0x79, 0x51).
   After the shared sweep (BtlPartyRightMove) the backdrop cursor g_BtlTilePatternList
   steps one entry, the caster (g_BtlActingSlot) plays anims 8 and 9 to their
   ends, the vibration script g_BtlActuatorRandomList starts and scheduler slots 1..4
   take sounds first_sound..arg5 (BtlShowSoundSet(slot, sound, 1, repeat) with
   repeats 0x1C, 0, 0, 0x24). BtlTileScrollSet(8, ..) gets (0x7C, 0x9C) when
   scroll_switch is set and (0x70, 0x90) otherwise, and backdrop *g_BtlTilePatternList is
   shown for one frame. layer_mix_set(8, 8) and layer_col_set(0, 0xC, 0)
   follow, then layer_col_fade_start(0, 0xC, 1, 0x10, g_BtlShowPulse.colour_1) and
   g_BtlShowPulse.on is set to 1 (BtlShow50Run makes the same pair; the block is
   BtlShowPulseSet's window pulse).
   Then: pattern_count more backdrops, 4 frames each, the cursor stepping past
   every one; 0x20 passes of BtlTilePatternStep(t, 4) (`t` is its running
   result, starting at 0); BtlShowPulseClear() and BtlAmountShowStart(1); more
   passes while layer_mix_fade_check(8) (colour ramp 8) or layer_col_fade_check(0)
   reports busy. The close clears the state of show sound slots 1 to 4
   (g_BtlShowSounds[1].state to [4].state),
   starts the ramp back (layer_mix_fade_start(8, 1, 0x20, 0x1F)), calls
   layer_col_fade_start(0, 0xC, 1, 0x10, 0), stops the vibration script and both
   motors, and keeps stepping while BtlAmountShowCheck(), the ramp or
   layer_col_fade_check(0) is busy; tile_grid_clear_copy(0, 1) and BtlTileEnd() end
   it. No RNG draw of its own.

   Source shape: the two zero inits stand after the one-frame
   BtlTilePatternPut call (anywhere from there to the loop matches; before it
   they trade places with that call's `li $a1, 1`); the two
   BtlTileScrollSet calls are separate statements, one per arm. */
/* func_80086B64 */
void BtlPartyRightMovePatternsRun(u8 scroll_switch, u16 pattern_count, u8 first_sound, u8 second_sound, u8 third_sound, u8 fourth_sound)
{
    u16 i;
    u16 t;

    BtlPartyRightMove();
    g_BtlTilePatternList++;
    BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 8, 0);
    BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 9, 0);
    g_PadActuatorList = (s32 *)g_BtlActuatorRandomList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlShowSoundSet(1, first_sound, 1, 0x1C);
    BtlShowSoundSet(2, second_sound, 1, 0);
    BtlShowSoundSet(3, third_sound, 1, 0);
    BtlShowSoundSet(4, fourth_sound, 1, 0x24);
    if (scroll_switch) {
        BtlTileScrollSet(8, 0x7C, 0x9C);
    } else {
        BtlTileScrollSet(8, 0x70, 0x90);
    }
    BtlTilePatternPut(*(u8 *)g_BtlTilePatternList, 1);
    layer_mix_set(8, 8);
    layer_col_set(0, 0xC, 0);
    layer_col_fade_start(0, 0xC, 1, 0x10, (s32)g_BtlShowPulse.colour_1);
    g_BtlShowPulse.on = 1;
    i = 0;
    t = 0;
    while (i < pattern_count) {
        BtlTilePatternPut(*(u8 *)(g_BtlTilePatternList++), 4);
        i++;
    }
    for (i = 0; i < 0x20; i++) {
        t = BtlTilePatternStep(t, 4);
    }
    BtlShowPulseClear();
    BtlAmountShowStart(1);
    while ((layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF)) {
        t = BtlTilePatternStep(t, 4);
    }
    g_BtlShowSounds[4].state = 0;
    g_BtlShowSounds[3].state = 0;
    g_BtlShowSounds[2].state = 0;
    g_BtlShowSounds[1].state = 0;
    layer_mix_fade_start(8, 1, 0x20, 0x1F);
    layer_col_fade_start(0, 0xC, 1, 0x10, 0);
    g_PadActuatorList = 0;
    g_PadActuatorRestFlags = 0;
    g_PadActuatorListWait = 0;
    g_PadActuatorLevels = 0;
    g_PadActuator1Level = 0;
    while (BtlAmountShowCheck() != 0 || (layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF)) {
        t = BtlTilePatternStep(t, 4);
    }
    tile_grid_clear_copy(0, 1);
    BtlTileEnd();
}

s32 xmalloc_heap_alloc(s32);
s32 xmalloc_heap_free(s32);
s32 line_path_make(u16 *, s16, s16, s16, s32, s32);
s32 BtlShowAnimationPlay(s32, s32, s32);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
void BtlAmountShowStart(s32 mode);  /* TU defines this below */
s32 BtlShowPulseClear();
s32 layer_col_fade_check(s32);
s32 BtlAmountShowCheck();
void tile_grid_clear_copy(s32, s32);
s32 BtlTileEnd();

/* Presentation 0x50: a stream of 20 copies of the caster (g_BtlActingSlot)
   flies from screen point (0x8C, 0x48) to the target (g_BtlTargetSlot). The
   path length m is the Manhattan distance over 8 (at least 1); object i
   starts 2*i frames after object i-1 (j = frame - 2*i): at j == 0 it is
   cloned from the caster and launched (0x11) one line below the target's
   y, while 0 <= j < m it follows the path (re-animated 0x10 at step 8,
   0xF at step 16), at j == m it lands (sound i % 10 with 0x70, anim 0x12)
   and at j == m + 16 it is switched off. While g_GameOptions[5] is set every
   step also drives the screen shake/flash (g_PadActuatorLevels / g_PadActuator1Level).

   Source shape: `n` is the raw 16-bit step count and `m` its
   int copy taken after the minimum clamp (the delay-slot `sra` is reorg's
   copy of the join); `end` is s16 (compared as halfwords); `lim` is
   computed inside the entry test like the original; each block forms its
   own record pointer (a shared one would be a multi-block variable and
   lose the target's register ties). */
/* func_80086E84 */
void BtlShow50Run(void)
{
    u8 *tg;
    s32 dx;
    s16 n;
    s32 m;
    s16 i;
    s16 j;
    s16 frame;
    s16 end;
    s32 lim;
    u16 *path;
    u8 *rec;
    u8 *dst;

    BtlPartyRightMove();
    tg = (u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords);
    dx = ((BtlRecord *)tg)->sprite.x - 0x8C;
    if (dx < 0) {
        dx = 0x8C - ((BtlRecord *)tg)->sprite.x;
    }
    n = (((((BtlRecord *)tg)->sprite.y - 0x48) >= 0)
             ? (dx + (((BtlRecord *)tg)->sprite.y - 0x48))
             : (dx + (0x48 - ((BtlRecord *)tg)->sprite.y))) / 8;
    if (n <= 0) {
        n = 1;
    }
    m = n;
    path = (u16 *)xmalloc_heap_alloc(m * 4);
    line_path_make(path, 0x8C, 0x48, *(s16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 6),
                  *(s16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 0xA), m);
    BtlShowPulseSet(4, 0x80, 0x60, 0, 0x40, 0x30, 0x30);
    g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlShowSoundSet(0xB, 0x6E, 0x10, 0);
    BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot * 0x40), 2, 0);
    layer_col_fade_start(0, 0xC, 1, 0x10, (s32)g_BtlShowPulse.colour_1);
    g_BtlShowPulse.on = 1;
    frame = 0;
    end = n + 0x28;
    if (end > 0) {
        lim = m + 0x10;
        do {
            for (i = 0; i < 0x14; i++) {
                j = frame - i * 2;
                if (g_GameOptions[5] != 0) {
                    g_PadActuatorLevels = i >= 6;
                    g_PadActuator1Level = (i * 0xFF) / 20;
                }
                if (j == 0) {
                    *(BtlRecord *)((i * 0x40) + g_BtlShowRecords) = *(BtlRecord *)((g_BtlActingSlot * 0x40) + g_BtlRecords);
                    rec = (u8 *)((i * 0x40) + g_BtlShowRecords);
                    *(s16 *)rec = 1;
                    ((BtlRecord *)rec)->sprite.depth = *(s16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 0xA) + 1;
                    BtlAnimationSetCopy((s32)rec, 0x11);
                }
                if (j >= 0) {
                    if (j < m) {
                        if (j == 8) {
                            BtlAnimationSetCopy(g_BtlShowRecords + (i * 0x40), 0x10);
                        }
                        if (j == 0x10) {
                            BtlAnimationSetCopy(g_BtlShowRecords + (i * 0x40), 0xF);
                        }
                        dst = (u8 *)((i * 0x40) + g_BtlShowRecords);
                        ((BtlRecord *)dst)->sprite.x = path[j * 2];
                        ((BtlRecord *)dst)->sprite.y = path[j * 2 + 1];
                    } else if (j == m) {
                        g_PadActuatorList = (s32 *)g_BtlActuatorFFList;
                        g_PadActuatorRestFlags = 0x3F;
                        g_PadActuatorListWait = 0;
                        BtlShowSoundSet(i % 10, 0x70, 0, 0);
                        BtlAnimationSetCopy(g_BtlShowRecords + (i * 0x40), 0x12);
                    } else if (j == lim) {
                        *(s16 *)((i * 0x40) + g_BtlShowRecords) = 0;
                    }
                }
            }
            BtlShowFrameStep();
            frame++;
        } while (frame < end);
    }
    BtlAmountShowStart(1);
    BtlShowPulseClear();
    BtlShowSoundStop(0xB);
    xmalloc_heap_free((s32)path);
    while ((u8)layer_col_fade_check(0) != 0) {
        BtlShowFrameStep();
    }
    layer_col_fade_start(0, 0xC, 1, 0x10, 0);
    while (BtlAmountShowCheck() != 0 || (u8)layer_col_fade_check(0) != 0) {
        BtlShowFrameStep();
    }
    BtlActingRestAnimationSet();
    tile_grid_clear_copy(0, 1);
    BtlTileEnd();
}

/* far_syms: g_BtlRecords */

s32 BtlShowAnimationPlay(s32, s32, s32);                         /* extern */

/* func_80087384 */
void BtlShow51Run(void) {
    BtlShowPulseSet(2, 0x78, 0x78, 0, 0x3C, 0x3C, 0);
    BtlPartyRightMovePatternsRun(0, 2, 0x73, 0x74, 0, 0);
    BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 0xA, 0);
    BtlActingRestAnimationSet();
}

/* func_80087408 */
void BtlShow52Run(void) {
    BtlShowPulseSet(2, 0, 0, 0xA0, 0, 0, 0x50);
    BtlPartyRightMovePatternsRun(1, 6, 0, 0, 0x79, 0x51);
    BtlActingRestAnimationSet();
}
/* far_syms: g_BtlActuatorFFList (target reaches it %hi/%lo) */
extern long HWD0, VWD0;
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 layer_mix_fade_check(s32 layer_mask);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
s32 BtlTilePatternStep(u16, s32);
s32 BtlTileEnd();
void tile_grid_clear_copy(s32, s32);
s32 layer_col_fade_check(s32);
void BtlAmountShowStart(s32);
s32 BtlAmountShowCheck();

/* func_80087470 */
void BtlShow53Run(void)
{
  s16 win[3];
  u16 i;
  u16 var;
  g_BtlShowVoiceState = 1;
  BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 0x10, 0);
  i = 0;
  var = 0;
  BtlActingRestAnimationSet();
  BtlFrameWait(0x10);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x5A, 0, 0);
  layer_mix_set(8, 8);
  layer_col_set(0, 0xC, 0);
  win[0] = 0xC0;
  win[1] = 0x48;
  win[2] = 0;
  layer_col_fade_start(0, 0xC, 1, 0xC, (s32) win);
  do
  {
    BtlTilePatternPut(*((u8 *) (g_BtlTilePatternList++)), 4);
    i++;
  }
  while (i < 6);
  while (1)
  {
    if (((u8) layer_mix_fade_check(8)) != 0)
    {
      var = BtlTilePatternStep((u16) var, 4);
      continue;
    }
    if (((u8) layer_col_fade_check(0)) == 0)
    {
      break;
    }
    var = BtlTilePatternStep((u16) var, 4);
  }
  win[0] = 0x80;
  win[1] = 0x30;
  win[2] = 0;
  layer_col_fade_start(0, 0xC, 1, 4, (s32) win);
  while (1)
  {
    if (((u8) layer_mix_fade_check(8)) != 0)
    {
      var = BtlTilePatternStep((u16) var, 4);
      continue;
    }
    if (((u8) layer_col_fade_check(0)) == 0)
    {
      break;
    }
    var = BtlTilePatternStep((u16) var, 4);
  }
  BtlShowSoundSet(1, 0x5B, 0, 0);
  BtlShowSoundSet(2, 0x5C, 0, 0);
  layer_mix_fade_start(8, 1, 0x80, 0x1F);
  layer_col_fade_start(0, 0xC, 1, 0x80, 0);
  BtlAmountShowStart(1);
  while (1)
  {
    if (BtlAmountShowCheck() != 0)
    {
      var = BtlTilePatternStep((u16) var, 4);
      continue;
    }
    if (((u8) layer_mix_fade_check(8)) != 0)
    {
      var = BtlTilePatternStep((u16) var, 4);
      continue;
    }
    if (((u8) layer_col_fade_check(0)) == 0)
    {
      break;
    }
    var = BtlTilePatternStep((u16) var, 4);
  }
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}
/* far_syms: g_BtlActingSlot */
/* g_BtlShowPulse: the window pulse BtlShowPulseSet arms (btl_effect.c's
   BtlShowPulse): +0 on, +0xC / +0x12 the window triples layer_col_fade_start takes. */
extern BtlShowPulse g_BtlShowPulse;
extern u8 g_BtlActuatorFFTwiceList[];
s32 BtlRecordPlaceCopy(s32, s16 *);
s32 rng_mask_below(s32, s32);

/* A scatter effect over the 20 display blocks at g_BtlShowRecords: the caster's
   block (g_BtlRecords + g_BtlActingSlot * 0x40) plays 0xD/0xE, sounds 0x72/0x73,
   the window pulse arms; 6 random points (x 0x10 + rng_mask_below(0x3F, 0x30),
   y 0x70 + rng_mask_below(0x1F, 0x1F)) are drawn up front and one more per
   frame until 20 while the windows settle. Each block is then a copy of
   the template at +0x500 (+0 = 1, +0xE = 0x200): slots 0..5 marked in
   g_BtlSlotAmountKinds take that member's own position (BtlRecordPlaceCopy, kind 0), the
   rest sit at their point + (i % 6) * 0x30 / (i % 2) * 0x20 (kind 1).
   Sounds 0x74/0x75, the impact (BtlAmountShowStart), BtlAnimationFramesCount's frame count
   of waiting, all 20 cleared, the windows close, wait for BtlAmountShowCheck. */
/* func_800876E4 */
void BtlShow54Run(void)
{
  PathPoint pts[20];
  u16 i;
  u16 k;
  u16 seed;
  u16 n;
  s32 blk;
  u16 y;
  s32 d;
  g_BtlShowVoiceState = 1;
  BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 0xD, 0);
  i = 0;
  k = 0;
  seed = 0;
  BtlAnimationSetCopy(g_BtlRecords + (g_BtlActingSlot << 6), 0xE);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFTwiceList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  BtlShowSoundSet(1, 0x72, 0, 0);
  BtlShowSoundSet(2, 0x73, 0x11, 0);
  layer_mix_set(8, 8);
  layer_col_set(0, 0xC, 0);
  BtlShowPulseSet(8, 0x70, 0x80, 0, 0x38, 0x40, 0);
  layer_col_fade_start(0, 0xC, 1, 0x18, (s32) g_BtlShowPulse.colour_1);
  g_BtlShowPulse.on = 1;
  do
  {
    BtlTilePatternPut(*((u8 *) (g_BtlTilePatternList++)), 4);
    i++;
    pts[k].x = rng_mask_below(0x3F, 0x30) + 0x10;
    pts[k++].y = rng_mask_below(0x1F, 0x1F) + 0x70;
  }
  while (i < 6);
  while ((k < 0x14) || (layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 4);
    if (k < 0x14)
    {
      pts[k].x = rng_mask_below(0x3F, 0x30) + 0x10;
      pts[k++].y = rng_mask_below(0x1F, 0x1F) + 0x70;
    }
  }
  i = 0;
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  ((BtlRecord *)g_BtlShowRecords)[20].on = 1;
  ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x200;
  do
  {
    *(BtlRecord *) ((i << 6) + g_BtlShowRecords) = *(BtlRecord *) (g_BtlShowRecords + 0x500);
    if ((i < 6) && (g_BtlSlotAmountKinds[i] == 1))
    {
      BtlRecordPlaceCopy(g_BtlRecords + (i << 6), (s16 *) (g_BtlShowRecords + (i << 6)));
      BtlAnimationSetCopy(g_BtlShowRecords + (i << 6), 0);
    }
    else
    {
      g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
      g_PadActuatorRestFlags = 0x3F;
      g_PadActuatorListWait = 0;
      blk = (i << 6) + g_BtlShowRecords;
      ((BtlRecord *)blk)->sprite.x = pts[i].x + (u16) (i % 6) * 0x30;
      ((BtlRecord *)blk)->sprite.y = y = pts[i].y + (i % 2) * 0x20;
      ((BtlRecord *)blk)->sprite.depth = y + 1;
      BtlAnimationSetCopy(blk, 1);
    }
    i++;
  }
  while (i < 0x14);
  ((BtlRecord *)g_BtlShowRecords)[20].on = 0;
  BtlShowSoundSet(3, 0x74, 0, 0);
  BtlShowSoundSet(4, 0x75, 0, 0x1E);
  BtlShowPulseClear();
  i = 0;
  BtlAmountShowStart(1);
  n = BtlAnimationFramesCount(g_BtlShowRecords);
  while ((i < n) || (layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 4);
    i++;
  }
  i = 0;
  d = g_BtlShowRecords;
  do
  {
    *(s16 *) ((i << 6) + d) = 0;
    i++;
  }
  while (i < 0x14);
  layer_mix_fade_start(8, 1, 0x40, 0x1F);
  layer_col_fade_start(0, 0xC, 1, 0x40, 0);
  g_PadActuatorList = (s32 *) g_BtlActuatorFFList;
  g_PadActuatorRestFlags = 0x3F;
  g_PadActuatorListWait = 0;
  while ((BtlAmountShowCheck() != 0) || (layer_mix_fade_check(8) & 0xFF) || (layer_col_fade_check(0) & 0xFF))
  {
    seed = BtlTilePatternStep(seed, 4);
  }
  g_BtlShowSounds[4].state = 0;
  BtlActingRestAnimationSet();
  tile_grid_clear_copy(0, 1);
  BtlTileEnd();
}

/* far_syms: g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlActingSlot g_BtlRecords */
extern u16 g_BtlSlotAmounts[];
s32 BtlAnimationSetCopy(s32, s32);
s32 BtlShowSoundSet(s32, s32, s32, s32);
void BtlFacingTowardSet(u8 *, s16);
s32 line_path_make(u16 *, s16, s16, s16, s32, s32);
s32 arc_path_make(s16, s16, s16, s16, s32, s32, PathPoint *);
s32 xmalloc_heap_free(s32);
s32 xmalloc_heap_alloc(s32);
s32 layer_mix_fade_check(s32 layer_mask);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
s32 BtlTilePatternStep(u16, s32);
s32 BtlShowWaveSet(s32, s32, s32, s32, s32);
void BtlTileScrollSet(s32, s32, s32);
s32 BtlTileEnd();
void tile_grid_clear_copy(s32, s32);
void mem_move_16(u16 *, u16 *, s32);
s32 layer_col_fade_check(s32);
void BtlAmountShowStart(s32);
s32 BtlAmountShowCheck();

/* func_80087C4C */
void BtlShow55Run(void)
{
    u16 cols[32];
    s16 c[3];
    s16 i;
    s16 cnt;
    u8 state;
    s16 frame;
    u16 timer;
    u8 flag;
    u8 v;
    s32 off;
    u16 keepC0;
    u8 keepF0;
    PathPoint *buf;
    PathPoint *buf2;
    u16 *pal;
    BtlRecord *rec;
    BtlRecord *act;

    g_PadActuatorList = (s32 *)g_BtlActuatorRiseList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlShowWaveSet(8, 1, 1, 1, (s32)g_BtlShow55WaveFactors);
    c[0] = c[1] = c[2] = -0x3C;
    layer_col_fade_start(0, 0xC, 1, 0x10, (s32)c);
    buf = (PathPoint *)xmalloc_heap_alloc(0x280);
    buf2 = buf + 0x50;
    i = 0;
    flag = 0;
    ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.flags &= 0x9FFF;
    for (; i < 5; i++) {
        v = g_BtlSlotAmountKinds[i];
        if (v == 1 || v == 3 || v == 4) {
            *(BtlRecord *)((i << 6) + g_BtlShowRecords) = *(BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords);
            off = i << 6;
            ((BtlRecord *)(off + g_BtlShowRecords))->on = 0;
            AS(u16, ((BtlRecord *)(off + g_BtlShowRecords))->sprite.depth) = ((BtlRecord *)(off + g_BtlRecords))->sprite.y;
            BtlAnimationSetCopy(off + g_BtlShowRecords, 0x17);
            BtlFacingTowardSet((u8 *)(g_BtlRecords + off), ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x);
            arc_path_make(((BtlRecord *)(off + g_BtlRecords))->sprite.x,
                          ((BtlRecord *)(off + g_BtlRecords))->sprite.y - (((BtlRecord *)(off + g_BtlRecords))->height >> 1),
                          ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x,
                          ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y - (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->height >> 1),
                          0x10, 0x48, (PathPoint *)((u8 *)buf + off));
            line_path_make((u16 *)((u8 *)buf2 + off),
                          ((BtlRecord *)(off + g_BtlRecords))->sprite.x, ((BtlRecord *)(off + g_BtlRecords))->sprite.y,
                          ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.x,
                          ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.y, 0x10);
            act = (BtlRecord *)(off + g_BtlRecords);
            rec = (BtlRecord *)(off + g_BtlShowRecords);
            rec->sprite.x = act->sprite.x;
            rec->sprite.y = act->sprite.y - (act->height >> 1);
            flag = 1;
            AS(u16, rec->sprite.depth) = AS(u16, act->sprite.depth) + 1;
        }
    }
    keepF0 = g_BtlSlotAmountKinds[6];
    keepC0 = g_BtlSlotAmounts[6];
    g_BtlSlotAmounts[6] = 0;
    g_BtlSlotAmountKinds[6] = 0;
    BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 0x10, 0);
    BtlShowSoundSet(1, 0x55, 1, 0x24);
    i = 0;
    frame = 0;
    timer = 0;
    pal = (u16 *)(g_PaletteBuf + 0x220);
    for (; i < 0x10; i++) {
        cols[i] = cols[i + 0x10] = pal[i];
    }
    layer_mix_fade_start(8, 1, 0x20, 0x10);
    BtlTileScrollSet(8, 0x20, 0);
    g_BtlShowVoiceState = 1;
    BtlShowAnimationPlay(g_BtlRecords + (g_BtlActingSlot << 6), 0x11, 0);
    state = 1;
    do {
        switch (state) {
        case 1:
            if ((u8)layer_mix_fade_check(8) == 0) {
                for (i = 0; i < 5; i++) {
                    v = g_BtlSlotAmountKinds[i];
                    if (v == 1 || v == 3 || v == 4) {
                        ((BtlRecord *)g_BtlShowRecords)[i].on = 1;
                    }
                }
                if (flag != 0) {
                    BtlShowSoundSet(2, 0x22, 1, 0x40);
                }
                cnt = 0;
                state++;
            }
            break;
        case 2:
            if (cnt < 0x20) {
                cnt++;
            } else {
                BtlAmountShowStart(1);
                cnt = 0;
                state++;
            }
            break;
        case 3:
            if (cnt < 0x10) {
                for (i = 0; i < 5; i++) {
                    ((BtlRecord *)g_BtlShowRecords)[i].sprite.x = buf[i * 16 + cnt].x;
                    ((BtlRecord *)g_BtlShowRecords)[i].sprite.y = buf[i * 16 + cnt].y;
                    AS(u16, ((BtlRecord *)g_BtlShowRecords)[i].sprite.depth) = buf2[i * 16 + cnt].y + 1;
                }
                BtlAmountShowCheck();
                cnt++;
            } else if (BtlAmountShowCheck() == 0) {
                xmalloc_heap_free((s32)buf);
                for (i = 0; i < 5; i++) {
                    g_BtlSlotAmounts[i] = 0;
                    g_BtlSlotAmountKinds[i] = 0;
                }
                g_BtlSlotAmountKinds[6] = keepF0;
                g_BtlSlotAmounts[6] = keepC0;
                BtlAmountShowStart(1);
                g_BtlShowSounds[2].state = 0;
                BtlShowSoundSet(4, 0x5D, 0, 0);
                cnt = 0;
                state++;
            }
            break;
        case 4:
            if (cnt < 0x20 && BtlAmountShowCheck() != 0) {
                cnt++;
            } else {
                g_BtlShowSounds[1].state = 0;
                BtlAnimationSetCopy(g_BtlRecords + (g_BtlActingSlot << 6), 0x12);
                for (cnt = i = 0; i < 5; i++) {
                    ((BtlRecord *)g_BtlShowRecords)[i].on = 0;
                }
                state++;
            }
            break;
        case 5:
            if (cnt < 0x10) {
                cnt++;
            } else {
                state++;
                BtlActingRestAnimationSet();
                layer_mix_fade_start(8, 1, 0x30, 0x1F);
                layer_col_fade_start(0, 0xC, 1, 0x30, 0);
            }
            break;
        case 6:
            if ((u8)layer_mix_fade_check(8) == 0 && (u8)layer_col_fade_check(0) == 0) {
                state = 0;
            }
            break;
        }
        mem_move_16(pal, &cols[frame & 0xF], 0x20);
        frame++;
        timer = BtlTilePatternStep(timer, 4);
    } while (state != 0);
    ((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.flags =
        (((BtlRecord *)((g_BtlActingSlot << 6) + g_BtlRecords))->sprite.flags & 0x9FFF) | 0x2000;
    tile_grid_clear_copy(0, 1);
    BtlTileEnd();
}

/* far_syms: g_PadActuatorLevels g_PadActuator1Level g_PadActuatorListWait g_PadActuatorList g_PadActuatorRestFlags g_BtlActingSlot g_BtlRecords g_BtlTargetSlot */
s32 BtlAnimationSetCopy(s32, s32);
s32 BtlShowSoundSet(s32, s32, s32, s32);
s32 BtlAnimationSet(s32, s32);
void BtlFacingTowardSet(u8 *, s16);
s32 BtlRecordPlaceCopy(s32, s16 *);
s32 line_path_make(u16 *, s16, s16, s16, s32, s32);
void BtlAnimationEndWait(void *record);
s32 xmalloc_heap_free(s32);
s32 xmalloc_heap_alloc(s32);
s32 layer_mix_fade_check(s32 layer_mask);
s32 layer_mix_fade_start(s32, s32, s32, s8);
s32 layer_col_fade_start(s32, s32, s32, s32, s32);
s32 rng_draw();
void memset(u8 *, s32, s32);
s32 BtlTilePatternStep(u16, s32);
s32 BtlShowActingEnd();
s32 BtlShowActingPlay(s32, s32);
s32 BtlShowWaveSet(s32, s32, s32, s32, s32);
void BtlTileScrollSet(s32, s32, s32);
s32 BtlTileEnd();
void tile_grid_clear_copy(s32, s32);
void BtlWalkEdgeRun(s32);
void mem_move_16(u16 *, u16 *, s32);
s32 layer_col_fade_check(s32);

/* A battle effect that hurls the target (g_BtlTargetSlot) away from the caster
   (g_BtlActingSlot). The effect object (g_BtlShowRecords, actor-sized records; record
   20 at +0x500 is its saved copy) starts on the target; every live party
   slot on the far side of the caster (by its facing bit 0x2000) is marked
   and recoils (0x13). The step count is the Manhattan distance from the
   target to the effect over 8; the first point of that path is where the
   target lands. States: 1 wait for the screen effect, 2 shake the marked
   actors +-2 px for 32 frames, 3 two steps of the target along the short
   path then restore the effect object, 4 a 32-frame flash, 5 a 24-frame
   flicker then wind down, 6 wait for both animations; a 32-colour palette
   cycles every frame. A target left in status 6 runs BtlAmountShowRun.

   The locals are reused the way the original reuses them, and the
   register allocation depends on it: `cnt` is the
   step count, then the landing y, then the state counter; `i` is the
   landing x before it is the palette index; `frame` is 16-bit (its HImode
   zero is what reload_cse substitutes into `i` and `timer` but not into
   BtlTileScrollSet's int 0). */
/* func_800883F8 */
void BtlShow56Run(void)
{
    u16 cols[32];
    u8 hit[5];
    s16 c[3];
    s16 i;
    s16 cnt;
    u8 state;
    s16 frame;
    u16 timer;
    s32 dx;
    u16 *path;
    u16 *pal;
    u8 *tg;
    u8 *base;

    memset(hit, 0, 5);
    g_PadActuatorList = (s32 *)g_BtlActuatorRiseList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    BtlShowSoundSet(1, 0x68, 0, 0);
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot * 0x40), 8);
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0x15);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot * 0x40)));
    BtlWalkEdgeRun(1);
    BtlFacingTowardSet((u8 *)(g_BtlRecords + (g_BtlActingSlot * 0x40)), *(s16 *)(((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 6));
    BtlFacingTowardSet((u8 *)(g_BtlRecords + 0x1C0), *(s16 *)(((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 6));
    BtlShowSoundSet(1, 0x68, 0, 0);
    BtlRecordPlaceCopy(g_BtlRecords + (g_BtlActingSlot * 0x40), (s16 *)(g_BtlRecords + 0x1C0));
    BtlAnimationSet(g_BtlRecords + (g_BtlActingSlot * 0x40), 9);
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0x16);
    BtlAnimationEndWait((void *)(g_BtlRecords + (g_BtlActingSlot * 0x40)));
    BtlAnimationSet(g_BtlRecords + 0x1C0, 0xB);
    BtlShowWaveSet(8, 1, 1, 1, (s32)g_BtlShow55WaveFactors);
    c[0] = c[1] = c[2] = -0x3C;
    layer_col_fade_start(0, 0xC, 1, 0x10, (s32)c);
    ((BtlRecord *)g_BtlShowRecords)[20].sprite.flags = 0x200;
    BtlRecordPlaceCopy(g_BtlRecords + (g_BtlActingSlot * 0x40), (s16 *)(g_BtlShowRecords + 0x500));
    BtlFacingTowardSet((u8 *)(g_BtlShowRecords + 0x500), *(s16 *)(((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 6));
    BtlShowSoundSet(1, 0x55, 1, 0x24);
    BtlShowActingPlay(0, 0);
    g_BtlShowVoiceState = 1;
    BtlAnimationSetCopy(g_BtlShowRecords, 1);
    for (i = 0; i < 5; i++) {
        if (g_BtlSlotActiveFlags[i] != 0) {
            if (*(u16 *)((u8 *)((g_BtlActingSlot * 0x40) + g_BtlRecords) + 0xC) & 0x2000) {
                if (*(s16 *)((u8 *)((g_BtlActingSlot * 0x40) + g_BtlRecords) + 6) > *(s16 *)(((i * 0x40) + g_BtlRecords) + 6)) {
                    hit[i] = 1;
                }
            } else if (*(s16 *)((u8 *)((g_BtlActingSlot * 0x40) + g_BtlRecords) + 6) < *(s16 *)(((i * 0x40) + g_BtlRecords) + 6)) {
                hit[i] = 1;
            }
        }
        if (hit[i] != 0) {
            BtlFacingTowardSet((u8 *)(g_BtlRecords + (i * 0x40)), ((BtlRecord *)g_BtlShowRecords)[20].sprite.x);
            BtlAnimationSetCopy(g_BtlRecords + (i * 0x40), 0x13);
        }
    }
    tg = (u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords);
    dx = ((BtlRecord *)tg)->sprite.x - ((BtlRecord *)g_BtlShowRecords)[20].sprite.x;
    if (dx < 0) {
        dx = ((BtlRecord *)g_BtlShowRecords)[20].sprite.x - ((BtlRecord *)tg)->sprite.x;
    }
    cnt = (((((BtlRecord *)tg)->sprite.y - ((BtlRecord *)g_BtlShowRecords)[20].sprite.y) >= 0)
               ? (dx + (((BtlRecord *)tg)->sprite.y - ((BtlRecord *)g_BtlShowRecords)[20].sprite.y))
               : (dx + (((BtlRecord *)g_BtlShowRecords)[20].sprite.y - ((BtlRecord *)tg)->sprite.y))) / 8;
    path = (u16 *)xmalloc_heap_alloc(cnt * 4);
    line_path_make(path, *(s16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 6),
                  *(s16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 0xA),
                  ((BtlRecord *)g_BtlShowRecords)[20].sprite.x, ((BtlRecord *)g_BtlShowRecords)[20].sprite.y, cnt);
    i = path[2];
    cnt = path[3];
    xmalloc_heap_free((s32)path);
    path = (u16 *)xmalloc_heap_alloc(8);
    line_path_make(path, *(s16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 6),
                  *(s16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 0xA), i, cnt, 2);
    BtlTileScrollSet(8, 0x20, 0);
    frame = 0;
    timer = frame;
    pal = (u16 *)(g_PaletteBuf + 0x220);
    for (i = 0; i < 0x10; i++) {
        cols[i] = cols[i + 0x10] = pal[i];
    }
    layer_mix_fade_start(8, 1, 0x20, 0x10);
    state = 1;
    do {
        switch (state) {
        case 1:
            if ((u8)layer_mix_fade_check(8) == 0) {
                cnt = 0;
                state++;
            }
            break;
        case 2:
            if (cnt < 0x20) {
                if (g_GameOptions[5] != 0) {
                    g_PadActuator1Level += rng_draw() & 0x3F;
                }
                if (cnt & 1) {
                    base = (u8 *)g_BtlRecords;
                    for (i = 0; i < 5; i++) {
                        if (hit[i] != 0) {
                            *(s16 *)((base + (i * 0x40)) + 6) -= 2;
                        }
                    }
                } else {
                    base = (u8 *)g_BtlRecords;
                    for (i = 0; i < 5; i++) {
                        if (hit[i] != 0) {
                            *(s16 *)((base + (i * 0x40)) + 6) += 2;
                        }
                    }
                }
                cnt++;
            } else {
                cnt = 0;
                state++;
            }
            break;
        case 3:
            if (cnt < 2) {
                if (g_GameOptions[5] != 0) {
                    g_PadActuator1Level = 0xFF;
                    g_PadActuatorLevels = 1;
                }
                *(u16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 6) = path[cnt * 2];
                *(u16 *)((u8 *)((g_BtlTargetSlot * 0x40) + g_BtlRecords) + 0xA) = path[(cnt * 2) + 1];
                cnt++;
            } else {
                g_PadActuator1Level = 0;
                g_PadActuatorLevels = 0;
                xmalloc_heap_free((s32)path);
                *(BtlRecord *)g_BtlShowRecords = *(BtlRecord *)(g_BtlShowRecords + 0x500);
                cnt = 0;
                state++;
                BtlRecordPlaceCopy(g_BtlRecords + (g_BtlTargetSlot * 0x40), (s16 *)g_BtlShowRecords);
                BtlAnimationSetCopy(g_BtlShowRecords, 2);
                BtlShowSoundSet(2, 0x6C, 1, 0x40);
            }
            break;
        case 4:
            if (cnt < 0x20) {
                if (g_GameOptions[5] != 0) {
                    g_PadActuator1Level += 3;
                }
                cnt++;
            } else {
                cnt = 0;
                BtlAnimationSetCopy(g_BtlShowRecords, 3);
                state++;
                g_BtlShowSounds[2].state = 0;
                BtlShowSoundSet(3, 0x66, 0, 0);
            }
            break;
        case 5:
            if (cnt < 0x18) {
                if (g_GameOptions[5] != 0) {
                    g_PadActuator1Level += rng_draw() & 0x3F;
                }
                cnt++;
            } else {
                BtlAnimationSetCopy(g_BtlShowRecords + 0x500, 1);
                state++;
                BtlShowActingEnd();
                layer_mix_fade_start(8, 1, 0x40, 0x1F);
                layer_col_fade_start(0, 0xC, 1, 0x40, 0);
                g_BtlShowSounds[1].state = 0;
            }
            break;
        case 6:
            g_PadActuator1Level = 0;
            g_PadActuatorLevels = 0;
            if ((u8)layer_mix_fade_check(8) == 0 && (u8)layer_col_fade_check(0) == 0) {
                state = 0;
            }
            break;
        }
        mem_move_16(pal, &cols[frame & 0xF], 0x20);
        frame++;
        timer = BtlTilePatternStep(timer, 4);
    } while (state != 0);
    g_PadActuatorList = 0;
    g_PadActuatorRestFlags = 0;
    g_PadActuatorListWait = 0;
    g_PadActuatorLevels = 0;
    g_PadActuator1Level = 0;
    tile_grid_clear_copy(0, 1);
    BtlTileEnd();
    if (g_BtlSlotAmountKinds[g_BtlTargetSlot] == 6) {
        BtlAmountShowRun();
    }
}
/* func_80088DF4 */
s32 BtlAnimationFramesCount(s32 blk)
{
  BtlRecord b;
  u16 n;
  u16 last;
  u16 *tbl;
  b = *(BtlRecord *) blk;
  n = 0;
  last = b.end & 0x7FFF;
  tbl = AS(u16 *, b.step_list);
  for (;;)
  {
    if (++b.timer >= b.step_length)
    {
      if (++b.step >= last)
      {
        return n;
      }
      b.timer = 0;
      b.step_length = tbl[b.step * 2 + 1];
    }
    n++;
  }
}
/* far_syms: g_BtlAmountRecordsRunning */
/* far_syms: g_BtlAmountRecords */
/* far_syms: CharStats */
extern u8 g_BtlSlotStrongMarks[];
extern u8 g_BtlPartyHitAnimations[];
extern u16 g_BtlAmountShowLength;
extern s16 g_BtlAmountShowFrameCount;
extern s32 g_BtlAmountShowLongestRecord;
extern s32 g_BtlAmountRecordsRunning;
extern s32 g_BtlAmountRecords;
extern u8 g_SpriteData2[];
s32 sprite_mix_col_set_copy(s32);
s32 BtlMixFlashSet(s32, s32);
s32 BtlAmountRecordSet(s32);
void BtlRecordSpriteDataSet(void *record, void *sprite_data, u32 palette, u32 animation);

/* func_80088EA8 */
void BtlAmountShowStart(s32 mode)
{
  u16 i;
  u16 k;
  u16 n;
  s32 fx;
  s32 blk;
  s32 rec;
  s32 b2;
  if (mode != 0)
  {
    sprite_mix_col_set_copy(0x1F);
    if ((g_BtlSlotIds[6] == 0x77) || ((g_BtlSlotIds[6] == 0x7C) && (((BtlStats *)CharStats)[7].hp != 0)))
    {
      BtlMixFlashSet(0x400, 3);
    }
    else
    {
      BtlMixFlashSet(0x400, 2);
    }
  }
  else
  {
    sprite_mix_col_set_copy(0x7FFF);
    BtlMixFlashSet(0x400, 1);
  }
  i = 0;
  layer_mix_switch_on(0x100);
  k = i;
  g_BtlAmountShowLongestRecord = 0;
  g_BtlAmountShowLength = 0;
  g_BtlAmountShowFrameCount = 0;
  fx = g_BtlRecords + 0x3C0;
  for (; i < 0xE; i++)
  {
    if (i == 5)
    {
      continue;
    }
    BtlAmountRecordSet(i);
    rec = (k << 6) + g_BtlAmountRecords;
    if (*(u16 *) rec != 0)
    {
      g_BtlAmountRecordsRunning = 1;
      n = BtlAnimationFramesCount(rec);
      if (g_BtlAmountShowLength < n)
      {
        g_BtlAmountShowLength = n;
        g_BtlAmountShowLongestRecord = g_BtlAmountRecords + (k << 6);
      }
    }
    switch (g_BtlSlotAmountKinds[i])
    {
      case 6:
        BtlShowSoundSet(0xB, 0x62, 0, 0);
        BtlRecordSpriteDataSet((void *) (fx + (k << 6)), g_SpriteData2, 0x720, 5);
        break;
      case 8:
        BtlRecordSpriteDataSet((void *) (fx + (k << 6)), g_SpriteData2, 0x7C0, (u16) (g_BtlSlotAmountKinds[i] - 1));
        break;
      case 9:
        BtlRecordSpriteDataSet((void *) (fx + (k << 6)), g_SpriteData2, 0x7B0, (u16) (g_BtlSlotAmountKinds[i] - 1));
        break;
      case 1:
      case 3:
      case 4:
        if (i < 5)
        {
          BtlAnimationSetCopy(g_BtlRecords + (i << 6), g_BtlPartyHitAnimations[i]);
        }
        if ((g_BtlSlotIds[6] == 0x7C) && (i == 7))
        {
          BtlMixFlashSet(0x400, 0);
          BtlShowSoundSet(0xA, 0x62, 0, 0);
          *(BtlRecord *) ((k << 6) + fx) = *(BtlRecord *) (g_BtlRecords + 0x180);
          b2 = fx + (k << 6);
          BtlAnimationSetCopy(b2, 0xD);
          BtlRecordPlaceCopy((void *) (g_BtlRecords + 0x180), (void *) b2);
          n = BtlAnimationFramesCount(b2);
          if (g_BtlAmountShowLength < n)
          {
            g_BtlAmountShowLongestRecord = b2;
            g_BtlAmountShowLength = n;
          }
          goto next;
        }
        if (mode != 0)
        {
          ((BtlRecord *) ((i << 6) + g_BtlRecords))->sprite.flags = (((BtlRecord *) ((i << 6) + g_BtlRecords))->sprite.flags & 0xCBFF) | 0x4800;
          k++;
          continue;
        }
        if (g_BtlSlotStrongMarks[i] != 0)
        {
          ((BtlRecord *) ((i << 6) + g_BtlRecords))->sprite.flags = (((BtlRecord *) ((i << 6) + g_BtlRecords))->sprite.flags & 0xCBFF) | 0x4800;
          BtlRecordSpriteDataSet((void *) (fx + (k << 6)), g_SpriteData2, 0x710, 1);
        }
        else
        {
          BtlRecordSpriteDataSet((void *) (fx + (k << 6)), g_SpriteData2, 0x700, 0);
        }
        break;
      default:
        k++;
        continue;
    }
    blk = (k << 6) + fx;
    ((BtlRecord *) blk)->on = 1;
    ((BtlRecord *) blk)->sprite.attr &= 0xFF9F;
    BtlRecordPlaceCopy((void *) (g_BtlRecords + (i << 6)), (void *) blk);
    if ((u8) (g_BtlSlotAmountKinds[i] - 8) < 2)
    {
      ((BtlRecord *) blk)->sprite.depth = 0x1000;
    }
    n = BtlAnimationFramesCount(blk);
    if (g_BtlAmountShowLength < n)
    {
      g_BtlAmountShowLongestRecord = blk;
      g_BtlAmountShowLength = n;
    }
  next:
    k++;
  }
}
