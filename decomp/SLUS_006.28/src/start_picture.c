/* Lunar SSSC — start_picture: the start's full-screen picture, its show and
   its frame. The first split had it in rendering.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "bg.h"
#include "icon.h"
#include "sprite.h"
#include "packet.h"
extern long HWD0;
extern long VWD0;
extern s32 g_PadButtons;
s32 StartPictureStep();
s32 screen_fade_step();
s32 layer_col_fade_check(s32);
s32 frame_step();
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 screen_fade_start(s32, s32);
s32 screen_disp_mask_set(s32);
void frame_start();
s32 resource_tim_load();
s32 StartPictureShow(void);
s32 screen_fade_step();
s32 layer_col_fade_check(s32);
s32 frame_step();
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 screen_fade_start(s32, s32);
s32 screen_disp_mask_set(s32);
void frame_start();
s32 resource_tim_load();
s32 StartPictureShow(void);
extern long HWD0, VWD0;
extern long HWD0, VWD0;
s32 screen_fade_step();

s32 layer_col_fade_check(s32);                                 /* extern */
s32 frame_step();                                /* extern */
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32); /* extern */

/* func_80015F94 */
s32 StartPictureStep(void) {
    packet_sprite_add(0, 0, 0, 0, 0x80, 0xE0, 0x88, 0x7800, 0x10);
    packet_sprite_add(0x80, 0, 0x80, 0, 0x80, 0xE0, 0x88, 0x7800, 0x10);
    packet_sprite_add(0x100, 0, 0, 0, 0x40, 0xE0, 0x8A, 0x7800, 0x10);
    screen_fade_step();
    layer_col_fade_check(0);
    return frame_step();
}

s32 screen_fade_start(s32, s32);                              /* extern */
s32 screen_disp_mask_set(s32);                                 /* extern */
void frame_start();                                  /* extern */
s32 resource_tim_load();                                  /* extern */

/* func_8001607C */
s32 StartPictureShow(void) {
    s32 shown_frames;
    s32 fade_frames;
    s32 ended_by_pad;

    ended_by_pad = 0;
    resource_tim_load();
    screen_disp_mask_set(1);
    shown_frames = 0;
    frame_start();
    screen_fade_start(0x20, 2);
loop_1:
    StartPictureStep();
    if (shown_frames >= 0x12C) {
        shown_frames += 1;
        if (g_PadButtons & 0x800) {
            ended_by_pad = 1;
        } else {
            goto block_5;
        }
    } else {
        shown_frames += 1;
block_5:
        if (shown_frames < 0x1F4) {
            goto loop_1;
        }
    }
    screen_fade_start(0x20, 3);
    fade_frames = 0;
    do {
        fade_frames += 1;
        StartPictureStep();
    } while (fade_frames < 0x20);
    return ended_by_pad;
}
