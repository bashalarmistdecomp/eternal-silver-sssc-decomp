/* Lunar SSSC — map_frame: a frame of the map: with no pad, under the pad,
   and its end. The first split had it in misc_g.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

extern u8 g_GameEnterRequest;
extern s8 g_MapObjectTouchOn;
extern s16 g_MapCol;
extern s16 g_MapColSecond;
extern s16 g_MapColThird;
extern s32 g_MoveChrLiveBits;
extern s32 g_MenuOnePageRunning;
extern s32 g_MenuPage;
extern s32 D_800B8AD0;
extern u16 g_PlayTimeFrames;
s32 xa_play_start(s32, s32);
s32 sprite_list_draw();
s32 palette_animation_step();
s32 tile_animation_step();
s32 MapFrameEndCopy();
s32 MoveChrUpdate();
s32 MapPartyAnimationStep();
s32 MapObjectsAnimationStep();
s32 MapViewStep(s32, s32);
void MapFrameStep(s32 view_follow, s32 second_view_follow);
s32 profile_mark_add(s32);
s32 MusicPlay(s32, s32, s32);
void MapPartyStep(void);
void MapObjectsStep(void);
s32 MapStatusDisp();
s32 GameEnter(s32);
s32 MapBgChangeStep();
s32 MapDataReload();
s32 MenuNoWalkRun();
s32 FaceListIdMove(s32, s32);
s32 MapObjectTouchRun();
void MapFrameUpdate(void);
s32 bg_draw();
void layer_col_fade_step();
void frame_start();
s32 frame_end();
void palette_update();
s32 sound_music_level_step();
s32 tile_animation_load();
void MapFrameCount();
void MapFrameEnd(void);

s32 sprite_list_draw();                                  /* extern */
s32 palette_animation_step();                                  /* extern */
s32 tile_animation_step();                                  /* extern */
s32 MapFrameEndCopy();                                  /* extern */
s32 MoveChrUpdate();                                  /* extern */
s32 MapPartyAnimationStep();                                  /* extern */
s32 MapObjectsAnimationStep();                                  /* extern */
s32 MapViewStep(s32, s32);                            /* extern */

/* func_800557D4 */
void MapFrameStep(s32 view_follow, s32 second_view_follow) {
    MoveChrUpdate();
    MapPartyAnimationStep();
    MapObjectsAnimationStep();
    MapViewStep(view_follow, second_view_follow);
    palette_animation_step();
    tile_animation_step();
    sprite_list_draw();
    MapFrameEndCopy();
}

/* far_syms: g_GameEnterRequest (target reaches it %hi/%lo) */
/* far_syms: g_MoveChrLiveBits (target reaches it %hi/%lo) */
/* far_syms: g_MenuOnePageRunning (target reaches it %hi/%lo) */
/* far_syms: g_MenuPage (target reaches it %hi/%lo) */

s32 profile_mark_add(s32);                                 /* extern */
s32 MusicPlay(s32, s32, s32);                           /* extern */
void MapPartyStep(void);  /* extern */
void MapObjectsStep(void);  /* extern */
s32 MapStatusDisp();                                  /* extern */
s32 GameEnter(s32);                               /* extern */
s32 MapBgChangeStep();                                  /* extern */
s32 MapDataReload();                                  /* extern */
s32 MenuNoWalkRun();                                /* extern */
s32 FaceListIdMove(s32, s32);                              /* extern */
s32 MapObjectTouchRun();                                  /* extern */

/* func_80055840 */
void MapFrameUpdate(void) {
    s32 request;

    profile_mark_add(0x210000FF);
    MapBgChangeStep();
    profile_mark_add(0x2100FF00);
    MapPartyStep();
    profile_mark_add(0x21FF0000);
    MapObjectsStep();
    profile_mark_add(0x2100FFFF);
    MapViewStep(1, 1);
    if (g_MapObjectTouchOn != 0) {
        MapObjectTouchRun();
    }
    profile_mark_add(0x21FF00FF);
    if (g_GameEnterRequest != 0) {
        MapDataReload();
        MusicPlay(0x17, 0, 1);
        FaceListIdMove(0xC, 0);
        g_MapColThird = -0x38;
        g_MapColSecond = -0x38;
        g_MapCol = -0x38;
        g_MenuPage = 0x14;
        g_MoveChrLiveBits = 0;
        g_MenuOnePageRunning = 1;
        request = MenuNoWalkRun();
        g_MenuOnePageRunning = 0;
        GameEnter(request & 0xFFFF);
        g_GameEnterRequest = 0;
    }
    palette_animation_step();
    tile_animation_step();
    MapStatusDisp();
}

/* far_syms: g_PlayTimeFrames (target reaches it %hi/%lo) */

s32 bg_draw();                                  /* extern */
void layer_col_fade_step();                                  /* extern */
void frame_start();                                  /* extern */
s32 frame_end();                                  /* extern */
void palette_update();                                  /* extern */
s32 sound_music_level_step();                                  /* extern */
s32 tile_animation_load();                                  /* extern */
void MapFrameCount();                                  /* extern */

/* func_8005596C */
void MapFrameEnd(void) {
    layer_col_fade_step();
    profile_mark_add(0x21FFFF00);
    bg_draw();
    palette_update();
    tile_animation_load();
    frame_end();
    frame_start();
    sound_music_level_step();
    g_PlayTimeFrames += 1;
    if (D_800B8AD0 != 0) {
        D_800B8AD0 = 0;
    }
    MapFrameCount();
}

/* far_syms: g_GameEnded */
extern u8 g_GameEnded;
extern u16 g_GameSecondXaChannelFrames[9];
extern u8 g_GameSecondXaFileName[];
void *memcpy(void *, const void *, unsigned int);
void overlay_disc_wait(s32);
void resource_load_run(s32);
s32 resource_tim_unpack();
void screen_picture_put();
void screen_disp_mask_set(s8);
void SetDispMask(s32);
s32 xa_file_set(s32);
s32 VSync(s32);

/* The game's run loop and what follows the end of the game. The main loop
   main calls it last in its pass; it never returns, so that
   loop's `while (1)` never comes round again.

   1. The end flag g_GameEnded = 0, then one frame per pass - the frame's
      work MapFrameUpdate, sprite_list_draw, the frame's tail MapFrameEnd -
      until the flag is set. In the whole image one other instruction
      stores to it: ScriptCommandStoryBitsItemsReset (lns_intp.c).
   2. overlay_disc_wait(0x33): the disc check for movie 0x33, i.e. disc 2 (video.md; it
      holds the disc-swap screen until that disc is in).
   3. resource_load_run(0x257) loads resource 599 (the disc-swap screen's
      bundle) and resource_tim_unpack(4) inflates the entry its command list
      saved as number 4 (g_ResourceTims[4]) into the buffer at g_LoadBuffer;
      then screen_picture_put(), screen_disp_mask_set(1) (g_ScreenDispMask = 1) and
      SetDispMask(1).
   4. xa_file_set("_SECOND.XA") (g_GameSecondXaFileName) finds the second XA file,
      the one only disc 2 carries. For channel i = 0..8:
      xa_play_start(0, i) starts that channel from the file's first sector
      and frames[i] VSyncs (VSync(0)) are waited out:
      10972, 9016, 9557, 8275, 9584, 9501, 8776, 8661, 4223 (the table at
      g_GameSecondXaChannelFrames; 183 s down to 70 s at 60 Hz, 21.8 minutes in all).
   5. Resource 599 again, its entry 5, the same three calls, and then
      VSync forever.

   No RNG draw of its own. What entries 4 and 5 show and what the nine
   channels hold was not read here.

   Shape: nothing needed shaping. `frames` is the original's initialised
   local array; while its data lives in the asm data file it is a memcpy
   from the block, as in btl_message.c's BtlWinRewardMessageDisp. The block must be
   declared u16 (2-aligned): the copy's tail is `lh` / `sh`, and a u8[]
   block gives byte moves there (88/91). The flag is u8 here (`lbu`;
   90/91 as s8) and the frame counter an int (66/91 as u16). */
/* func_800559F4 */
void GameRun(void)
{
    u16 frames[9];
    s32 i;
    s32 j;

    g_GameEnded = 0;
    do {
        MapFrameUpdate();
        sprite_list_draw();
        MapFrameEnd();
    } while (g_GameEnded == 0);
    memcpy(frames, g_GameSecondXaChannelFrames, sizeof(frames));
    overlay_disc_wait(0x33);
    resource_load_run(0x257);
    resource_tim_unpack(4);
    screen_picture_put();
    screen_disp_mask_set(1);
    SetDispMask(1);
    xa_file_set((s32)g_GameSecondXaFileName);
    for (i = 0; i < 9; i++) {
        xa_play_start(0, i);
        for (j = 0; j < frames[i]; j++) {
            VSync(0);
        }
    }
    resource_load_run(0x257);
    resource_tim_unpack(5);
    screen_picture_put();
    screen_disp_mask_set(1);
    SetDispMask(1);
    for (;;) {
        VSync(0);
    }
}
