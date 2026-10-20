/* Lunar SSSC — picture: a picture's show: its XA sound, video memory saved
   and restored, its bands drawn, its pan stepped. The first split had it in
   cdrom_load.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

extern RECT g_PictureVramRectangle;
extern u8 g_PictureVoices[];
extern s32 g_PicturePanRow;
extern s32 g_PictureVramBuffer;
extern s32 D_800B8A9C;
extern s32 g_PicturePanSin;
extern s32 g_PicturePanCos;
void PictureStep(void);                                  /* extern */
s32 PictureBandDraw(s32, s32, s32, s32);                  /* extern */
s32 resource_load_run();
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);
void PictureDraw(s32 first_row);  /* TU defines this below */
s32 PictureXaPlay(s32 picture);
s32 VoiceSet(u8);
s32 VoiceStart();
extern s16 g_Voice;
s32 screen_fade_step();
s32 layer_col_fade_check(s32);
s32 frame_step();
s32 ScriptVoiceStep();
s32 screen_fade_start(s32, s32);
s32 screen_disp_mask_set(s32);
s32 resource_tim_load(s32);
void PictureVramSave();
s32 PictureVramRestore();
void PictureShow(s32 picture);
s32 PictureXaPlay(s32);
s32 resource_load_run();

/* far_syms: g_PictureVoices (target reaches it %hi/%lo) */
/* far_syms: g_Voice (target reaches it %hi/%lo) */

s32 VoiceSet(u8);                                /* extern */
s32 VoiceStart();                                  /* extern */
extern s16 g_Voice;

/* func_800247AC */
s32 PictureXaPlay(s32 picture) {
    s32 voice;

    voice = *(picture + g_PictureVoices);
    g_Voice = (s16) voice;
    VoiceSet(voice);
    VoiceStart();
    g_Voice = -1;
}
/* la_absolute: PictureVramSave */

/* Save the screen rectangle g_PictureVramRectangle from VRAM into the buffer
   g_PictureVramBuffer points at (StoreImage).  PictureVramRestore puts it back
   (LoadImage); PictureShow sets the buffer to 0x8016E000 first. */
/* func_800247F4 */
void PictureVramSave(void)
{
  StoreImage(&g_PictureVramRectangle, (u_long *) g_PictureVramBuffer);
}

/* la_absolute: PictureVramRestore */

/* Put the screen rectangle g_PictureVramRectangle back from the buffer g_PictureVramBuffer
   points at (LoadImage); PictureVramSave saved it there. */
/* func_80024820 */
s32 PictureVramRestore()
{
  return LoadImage(&g_PictureVramRectangle, (u_long *) g_PictureVramBuffer);
}

s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32); /* extern */

/* func_8002484C */
s32 PictureBandDraw(s32 screen_row, s32 texture_row, s32 height, s32 tpage) {
    s32 sprite_tpage;
    s32 band_height;

    band_height = height;
    if (band_height >= 0x100) {
        band_height = 0xFF;
    }
    sprite_tpage = tpage | 0x80;
    packet_sprite_add(0, screen_row, 0, texture_row, 0x80, band_height, sprite_tpage, 0x7F40, 0x10);
    packet_sprite_add(0x80, screen_row, 0x80, texture_row, 0x80, band_height, sprite_tpage, 0x7F40, 0x10);
    return packet_sprite_add(0x100, screen_row, 0, texture_row, 0x40, band_height, (tpage + 2) | 0x80, 0x7F40, 0x10);
}

/* func_80024950 */
void PictureDraw(s32 first_row) {
    s32 upper_row_count;

    upper_row_count = 0x100 - first_row;
    PictureBandDraw(0, first_row, upper_row_count, 5);
    if (first_row >= 0x20) {
        PictureBandDraw(upper_row_count, 0, first_row - 0x20, 0x15);
    }
}
s32 screen_fade_step();
s32 layer_col_fade_check(s32);
s32 frame_step();
s32 ScriptVoiceStep();

/* func_800249B0 */
void PictureStep(void)
{
  s32 pan_cos;
  s32 pan_sin;
  s32 pan_row;
  pan_sin = g_PicturePanSin;
  g_PicturePanSin = pan_sin + (g_PicturePanCos / 120);
  pan_cos = g_PicturePanCos - (pan_sin / 120);
  g_PicturePanCos = pan_cos;
  pan_row = 0x7C - (pan_cos >> 8);
  g_PicturePanRow = pan_row;
  if (pan_row < 0)
  {
    g_PicturePanRow = 0;
  }
  if (g_PicturePanRow >= 0x100)
  {
    g_PicturePanRow = 0xFF;
  }
  PictureDraw(g_PicturePanRow);
  screen_fade_step();
  layer_col_fade_check(0);
  ScriptVoiceStep();
  frame_step();
}

s32 screen_fade_start(s32, s32);                              /* extern */
s32 screen_disp_mask_set(s32);                                 /* extern */
s32 resource_tim_load(s32);                                 /* extern */
void PictureVramSave();                                  /* extern */
s32 PictureVramRestore();                                  /* extern */
extern s16 g_Voice; /* far_syms: g_Voice */

/* func_80024A7C */
void PictureShow(s32 picture) {
    s32 shown_frames;
    s32 fade_frames;
    s32 wait_frames;

    g_PictureVramBuffer = 0x8016E000;
    PictureVramSave();
    resource_load_run(picture + 0x24A);
    resource_tim_load(0);
    resource_tim_load(1);
    D_800B8A9C = 0;
    g_PicturePanSin = 0;
    g_PicturePanCos = 0x8000;
    PictureXaPlay(picture);
    screen_fade_start(0x20, 2);
    shown_frames = 0;
    do {
        shown_frames += 1;
        PictureStep();
        screen_disp_mask_set(1);
    } while (shown_frames < 0x2D0);
    screen_fade_start(0x20, 3);
    fade_frames = 0;
    do {
        fade_frames += 1;
        PictureStep();
    } while (fade_frames < 0x20);
    SetDispMask(0);
    screen_disp_mask_set(0);
    wait_frames = 0;
    PictureStep();
loop_5:
    if (g_Voice != 0) {
        wait_frames += 1;
        PictureStep();
        if (wait_frames < 0x80) {
            goto loop_5;
        }
    }
    PictureVramRestore();
}
