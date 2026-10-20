/* Lunar SSSC — map_fade: the map faded out and in (with the party's part of
   each) and its picture closed in and opened out. The first split had it in
   misc_j.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "map.h"

extern u8 g_MapFadeState;
extern u8 g_MapDrawAreaClosed;
extern s32 g_PartyIds;
s32 sprite_run_off(s32, s32);
s32 layer_col_fade_start(s32, s32, s32, s32, s32 *);  /* TU declares this below */
s32 layer_col_fade_check(s32);
s32 MapFrameStep(s32, s32);  /* TU declares this below */
void MapFadePartyOut(s32, u32, s32, s32);

/* func_800568D4 */
void MapFadeOut(s32 layer_mask, s32 length, s32 *target_col, s32 view_follow, s32 second_view_follow)
{
  loop_1:
  if (layer_col_fade_check(0) & 0xFF)
  {
    MapFrameStep(view_follow, second_view_follow);
    goto loop_1;
  }

  if (g_MapFadeState == 2)
  {
    MapFadePartyOut(layer_mask, length, view_follow, second_view_follow);
  }
  else
  {
    g_MapFadeState = 1;
  }
  layer_col_fade_start(0, layer_mask | 0x80000, 3, length, target_col);
  loop_7:
  if (layer_col_fade_check(0) & 0xFF)
  {
    MapFrameStep(view_follow, second_view_follow);
    goto loop_7;
  }

  MapFrameStep(view_follow, second_view_follow);
  MapFrameStep(view_follow, second_view_follow);
  if (layer_mask & 0x100)
  {
    sprite_run_off(0, 0x96);
  }
}

/* no_split_addresses: MapFadeIn */
extern u8 g_MapCol[];

s32 layer_col_fade_start(s32, s32, s32, s32, s32 *);                 /* extern */
s32 layer_col_set(s32, s32, s32);                           /* extern */
s32 layer_col_fade_check(s32);                               /* extern */
s32 screen_disp_mask_set(s32);                                 /* extern */
s32 MapFrameStep(s32, s32);                              /* extern */
void MapFadePartyIn(s32, s32, s32, s32);                      /* extern */

/* func_800569E0 */
void MapFadeIn(s32 layer_mask, s32 length, s32 view_follow, s32 second_view_follow) {
loop_1:
    if (layer_col_fade_check(0) & 0xFF) {
        MapFrameStep(view_follow, second_view_follow);
        goto loop_1;
    }
    if (g_MapFadeState == 2) {
        screen_disp_mask_set(1);
        MapFadePartyIn(layer_mask, length, view_follow, second_view_follow);
    } else {
        layer_col_fade_start(0, layer_mask | 0x80000, 1, length, g_MapCol);
loop_6:
        if (layer_col_fade_check(0) & 0xFF) {
            MapFrameStep(view_follow, second_view_follow);
            screen_disp_mask_set(1);
            goto loop_6;
        }
    }
    g_MapFadeState = 0;
    g_MapDrawAreaClosed = 0;
    layer_col_set(-1, 0x80030, 0);
}

/* empty (jr $ra / nop) — the body proves nothing about the signature, so
   the arg list comes from the two call sites in menu_nowalk.c, which
   pass two. */
/* Ruling of 2026-10-14 (docs/naming.md): Empty in this build; its two calls pass (0x30, 1) and (0x30, 0), which do not say what for. */
void func_80056AE4() {

}

/* A party actor row as the two glow fades below read it (MapFadePartyOut,
   MapFadePartyIn), and the globals both of them use. */
extern u8 g_MapActuatorOneFrameList[];
extern u8 g_BtlActuatorToggleList[];
extern s16 g_PadActuatorListWait;
extern s32 *g_PadActuatorList;
extern u8 g_PadActuatorRestFlags;
extern s32 g_MapPartyRecords;
extern s32 g_MapPartyTwins;
/* The g_MapFadeState == 2 branch of the fade MapFadeOut starts (its only
   caller): the party's glow is brought up before the fade proper. arg0 =
   layer_col_fade_start's mask argument, length = the duration in frames (unsigned,
   like MapDrawAreaClose's and MapDrawAreaOpen's), view_follow / second_view_follow = the field
   frame's two arguments (MapFrameStep). MapFadePartyIn is the same thing
   for MapFadeIn, and the two share steps 1, 2 (in part) and 7.

   1. Vibration script g_BtlActuatorToggleList (g_PadActuatorList = the script, g_PadActuatorRestFlags =
      0x3F, g_PadActuatorListWait = 0).
   2. Every present party member (g_PartyIds[i] != 0): fE |= 0x6000 on
      both its actor rows (g_MapPartyRecords's, then g_MapPartyTwins's). Nothing else:
      no +4 clear, no spot, no dispatch.
   3. arg0 loses bit 0x100 and gains 0x80000; layer_col_set(-1, 0x100, 0),
      the fade layer_col_fade_start(0, layer_mask, 1, length, -1) (-1 where MapFadePartyIn
      passes the colour g_MapCol), and the glow switch on
      (layer_mix_switch_on(0x100)).
   4. arg1 field frames.
   5. layer_mix_fade_start(0xD00, 1, length, 0x1F): layers 0x100 | 0x400 | 0x800
      ramp over length frames; one field frame per pass while
      layer_mix_fade_check(0xD00) & 0xFF (the ramp still running).
   6. MapPartyPose4Play() (every member's rows sent message 4, then 60 field
      frames), g_MapFadeState = 2 again, the three layers' levels set
      (layer_mix_set: 0x100 -> 0, 0x400 -> 0xC, 0x800 -> 0x12) and the glow
      switch off (layer_mix_switch_off(0x100)).
   7. Vibration script g_MapActuatorOneFrameList.

   No RNG draw of its own; its frames are MapFrameStep's and
   MapPartyPose4Play's.

   Shape notes. The rows as struct array elements ((MapRecord *)p)[i].fE with a
   signed i give the three walked pointers of the first loop (the two row
   bases and the member table, each loaded from gp once before it) and
   `slti`; the same i against the unsigned length gives the second loop's
   `sltu` with the `beqz length` guard in front.
   `layer_mask &= ~0x100; layer_mask |= 0x80000;` are two statements, as in
   MapFadePartyIn. */
/* func_80056AEC */
void MapFadePartyOut(s32 layer_mask, u32 length, s32 view_follow, s32 second_view_follow)
{
    s32 i;

    g_PadActuatorList = (s32 *)g_BtlActuatorToggleList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            ((MapRecord *)g_MapPartyRecords)[i].sprite.flags |= 0x6000;
            ((MapRecord *)g_MapPartyTwins)[i].sprite.flags |= 0x6000;
        }
    }
    layer_mask &= ~0x100;
    layer_mask |= 0x80000;
    layer_col_set(-1, 0x100, 0);
    layer_col_fade_start(0, layer_mask, 1, length, (s32 *)-1);
    layer_mix_switch_on(0x100);
    for (i = 0; i < length; i++) {
        MapFrameStep(view_follow, second_view_follow);
    }
    layer_mix_fade_start(0xD00, 1, length, 0x1F);
    while (layer_mix_fade_check(0xD00) & 0xFF) {
        MapFrameStep(view_follow, second_view_follow);
    }
    MapPartyPose4Play();
    g_MapFadeState = 2;
    layer_mix_set(0x100, 0);
    layer_mix_set(0x400, 0xC);
    layer_mix_set(0x800, 0x12);
    layer_mix_switch_off(0x100);
    g_PadActuatorList = (s32 *)g_MapActuatorOneFrameList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
}

/* la_absolute: MapFadePartyIn */
extern s16 g_MapCol_blk[3];

/* The g_MapFadeState == 2 variant of the fade MapFadeIn runs (its only
   caller; MapFadePartyOut is the same branch of MapFadeOut). arg0 =
   layer_col_fade_start's mask argument, length = the duration, view_follow / second_view_follow = the
   field frame's two arguments (MapFrameStep).

   1. Vibration script g_BtlActuatorToggleList (motion 0x4A on both motors under a
      180-frame wait - not the random motion, so no RNG draw), the glow
      switch on (layer_mix_switch_on(0x100)) and layer_mix_set(0xD00, 0x1F):
      level 0x1F on layers 0x100 | 0x400 | 0x800.
   2. Every present party member (g_PartyIds[i] != 0): the +4 halfword of
      both its actor rows (g_MapPartyTwins's, then g_MapPartyRecords's) is cleared,
      both rows get fE |= 0x6000, and both are sent
      MapAnimationPoseSet(row, 0xD, spot.b5) - with the spot of member
      ZERO (MapPartyRingEntryGet(0, &spot)) for every member.
   3. layer_col_set(-1, 0x100, -1), one field frame, MapPartyOnPlay().
   4. arg0 loses bit 0x100 and gains 0x80000; layer_mix_fade_start(layer, 1,
      length, level) for layers 0x100 / 0x400 / 0x800 with levels 0 / 0xC /
      0x12; then the fade layer_col_fade_start(0, layer_mask, 1, length, g_MapCol).
   5. One field frame per pass while layer_col_fade_check(0) & 0xFF.
   6. The glow switch off (layer_mix_switch_off(0x100)), layer_col_set(0, 0x100,
      g_MapCol), and every present member's two rows take their OWN
      spot's h6 bits back into fE (MapPartyRingEntryGet(i, &spot); fE = spot.h6 |
      (fE & 0x9FFF)).
   7. layer_mix_set(0x80F35, 0xFF) and the vibration script g_MapActuatorOneFrameList (a
      one-frame level on motor 0).

   No RNG draw of its own; its frames are MapFrameStep's.

   Shape notes. Struct-typed rows (a[i].f) give the `addu $v0,$s1,$v0`
   operand order with i * 0x3C strength-reduced, and let the scheduler
   lift the gp loads of g_MapPartyRecords / g_MapPartyTwins over the halfword stores;
   the second dispatch's `addu $a0,$a0,$s1` is the same &a[i] in a value
   context. `layer_mask &= ~0x100; layer_mask |= 0x80000;` are two statements (one
   expression builds the AND in $v0). (u16)i is MapPartyRingEntryGet's
   prototype-less u16 argument.

   g_MapCol (the fade colour, three s16) is reached here through its
   alias g_MapCol_blk as a SIZED small array under la_absolute: the
   target's `lui $a2 / addiu $a2,$a2` before `jal layer_col_set` with
   `li $a1` in the delay slot is the assembler's `la` of a small-data
   symbol (cc1's split address puts the addiu in the slot: 167/169), while
   g_MapActuatorOneFrameList's halves are split across the epilogue - so the function is
   not no_split_addresses. The alias is only a name: this TU declares
   `extern u8 g_MapCol[]` with no_split_addresses markers for
   MapFadeIn and MapDrawAreaOpen, and a sized re-declaration would move
   those two. Measured on a scratch copy of the TU: `extern u8
   g_MapCol[6]` with la_absolute on all three functions matches all
   three (65/65, 108/108, 169/169) - that is the right model for the TU. */
/* func_80056CB8 */
void MapFadePartyIn(s32 layer_mask, s32 length, s32 view_follow, s32 second_view_follow)
{
    MapPartyRingEntry spot;
    s32 i;

    g_PadActuatorList = (s32 *)g_BtlActuatorToggleList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
    layer_mix_switch_on(0x100);
    layer_mix_set(0xD00, 0x1F);
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            AS(s16, ((MapRecord *)g_MapPartyTwins)[i].sprite.on) = 0;
            AS(s16, ((MapRecord *)g_MapPartyRecords)[i].sprite.on) = 0;
            MapPartyRingEntryGet(0, &spot);
            ((MapRecord *)g_MapPartyRecords)[i].sprite.flags |= 0x6000;
            ((MapRecord *)g_MapPartyTwins)[i].sprite.flags |= 0x6000;
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyRecords)[i], 0xD, spot.facing);
            MapAnimationPoseSet(&((MapRecord *)g_MapPartyTwins)[i], 0xD, spot.facing);
        }
    }
    layer_col_set(-1, 0x100, -1);
    MapFrameStep(view_follow, second_view_follow);
    MapPartyOnPlay();
    layer_mask &= ~0x100;
    layer_mask |= 0x80000;
    layer_mix_fade_start(0x100, 1, length, 0);
    layer_mix_fade_start(0x400, 1, length, 0xC);
    layer_mix_fade_start(0x800, 1, length, 0x12);
    layer_col_fade_start(0, layer_mask, 1, length, (s32 *)g_MapCol_blk);
    while (layer_col_fade_check(0) & 0xFF) {
        MapFrameStep(view_follow, second_view_follow);
    }
    layer_mix_switch_off(0x100);
    layer_col_set(0, 0x100, (s32)g_MapCol_blk);
    for (i = 0; i < 6; i++) {
        if (((u16 *)g_PartyIds)[i] != 0) {
            MapPartyRingEntryGet((u16)i, &spot);
            ((MapRecord *)g_MapPartyRecords)[i].sprite.flags = spot.class | (((MapRecord *)g_MapPartyRecords)[i].sprite.flags & 0x9FFF);
            ((MapRecord *)g_MapPartyTwins)[i].sprite.flags = spot.class | (((MapRecord *)g_MapPartyTwins)[i].sprite.flags & 0x9FFF);
        }
    }
    layer_mix_set(0x80F35, 0xFF);
    g_PadActuatorList = (s32 *)g_MapActuatorOneFrameList;
    g_PadActuatorRestFlags = 0x3F;
    g_PadActuatorListWait = 0;
}

s32 layer_col_set(s32, s32, s32);                         /* extern */
s32 screen_disp_mask_set(s32);                                 /* extern */
s32 screen_draw_area_set(s32, s32, s32, s32, s32, s32, s32, s32); /* extern */
s32 MapFrameStep(s32, s32);                            /* extern */

/* func_80056F5C */
void MapDrawAreaClose(s32 layer_mask, u32 step_count, s32 unused, s32 view_follow, s32 second_view_follow) {
    s32 half_height;
    s32 step;
    s32 half_width;
    u32 steps_left;

    g_MapDrawAreaClosed = 1;
    if (g_MapFadeState == 0) {
        steps_left = step_count;
        if (steps_left & 0xFFFF) {
            do {
                step = steps_left & 0xFFFF;
                half_width = (0xA0U / step_count) * step;
                half_height = (0x70U / step_count) * step;
                screen_draw_area_set(0, 0, layer_mask, layer_mask, 0xA0 - half_width, 0x70 - half_height, half_width + 0xA0, half_height + 0x70);
                steps_left -= 1;
                MapFrameStep(view_follow, second_view_follow);
            } while (steps_left & 0xFFFF);
        }
        layer_col_set(0, layer_mask, -1);
        screen_draw_area_set(0, 0, 0, 0, 0, 0, 0x13F, 0xDF);
        MapFrameStep(view_follow, second_view_follow);
        screen_disp_mask_set(0);
        MapFrameStep(view_follow, second_view_follow);
    }
}

/* no_split_addresses: MapDrawAreaOpen */
s32 MapFrameEndCopy();
s32 StoryBitClear(s32);

/* func_800570D8 */
void MapDrawAreaOpen(s32 layer_mask, u32 step_count, s32 unused, s32 view_follow, s32 second_view_follow)
{
  s32 half_height;
  s32 step;
  s32 half_width;
  u16 steps_done;
  s32 fade_state;
  StoryBitClear(0x331);
  fade_state = g_MapFadeState;
  g_MapDrawAreaClosed = 0;
  if (fade_state == 0)
  {
    screen_draw_area_set(0, 0, layer_mask, layer_mask, 0xA0, 0x70, 0xA0, 0x70);
    steps_done = 0;
    MapFrameEndCopy();
    screen_disp_mask_set(1);
    layer_col_set(0, layer_mask, g_MapCol);
    layer_col_set(-1, 0x30, 0);
    if (fade_state < step_count)
    {
      do
      {
        step = steps_done & 0xFFFF;
        half_width = (0xA0U / step_count) * step;
        half_height = (0x70U / step_count) * step;
        screen_draw_area_set(0, 0, layer_mask, layer_mask, 0x9F - half_width, 0x6F - half_height, half_width + 0xA0, half_height + 0x70);
        steps_done += 1;
        MapFrameStep(view_follow, second_view_follow);
      }
      while (((u32) (steps_done & 0xFFFF)) < step_count);
    }
    screen_draw_area_set(0, 0, 0, 0, 0, 0, 0x13F, 0xDF);
  }
}
