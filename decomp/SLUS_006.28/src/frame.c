/* Lunar SSSC — frame: the frame and the screen it is drawn on: system_init
   (the start-up of each pass of main's loop), the display mask, the screen
   modes and draw area, the packet buffers' fill and check, frame_start,
   frame_end and frame_step. The first split had it in ui.c. Every function
   is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "sprite.h"
#include "packet.h"
#include "tile.h"
#include "layer.h"
#include "cd.h"
extern s8 g_ScreenMode;
void GsInitGraph(u16, u16, u16, u16, u16);
void GsDefDispBuff(u16, u16, u16, u16);
extern volatile s32 g_FrameVSyncTotal;
s32 VSync(s32);
s32 ResetGraph(s32);
s32 bg_col_set(s32);
extern u8 g_VramStripRectangle[];
extern u8 g_FrameOts[];
extern s32 D_800B8848;
extern s32 g_ProfileMarkCount;
extern void *g_FrameOt;
extern s16 g_ScreenDrawAreaX;
extern s16 g_ScreenDrawAreaY;
extern s16 g_ScreenDrawAreaWidth;
extern s16 g_ScreenDrawAreaHeight;
extern u8 g_PacketBufs[];
extern u8 g_SystemAudioXaFileName[];
extern u8 g_ScreenClearRectangle[];
extern s8 D_800B8830;
extern s8 g_MapBtlWinForced;
extern s8 g_MapObjectTouchOn;
extern s8 g_DebugAllowed;
extern s32 g_FileHostBlock;
extern s32 g_CdDirRows;
extern s8 g_ScreenDispMask;
s32 DrawSync(s32);
void pad_frame_step(void);
void screen_disp_mask_set(s8 on);
void layer_col_fade_step();
void frame_start();
void frame_end();
void frame_step(void);
s32 sprite_mix_switch_on();
void layer_mix_switch_on(s32 layer_mask);
void layer_mix_switch_off(s32 layer_mask);
void pad_frame_step(void);
void screen_disp_mask_set(s8 on);
void layer_col_fade_step();
void frame_start();
void frame_end();
void frame_step(void);
s32 sprite_mix_switch_on();
void layer_mix_switch_on(s32 layer_mask);
void layer_mix_switch_off(s32 layer_mask);
void system_init(void);
void vram_strip_clear(void);
void screen_draw_area_set(void);
void packet_buf_check(void);
s32 GsGetActiveBuff();
void packet_buf_fill(void);
void system_init(void);
void vram_strip_clear(void);
void screen_draw_area_set(void);
void packet_buf_check(void);
s32 GsGetActiveBuff();
void system_init(void);
void func_8001B02C();  /* extern */
void rng_seed(s32 seed);  /* extern */
void layer_col_fade_step(void);  /* extern */

/* func_8001EE98 */
void pad_frame_step(void) {
    pad_update();
}

/* func_8001EEB8 */
void system_init(void) {
    g_FileHostBlock = (s32)(void *)0x80010000;
    g_CdDirRows = 0x80010100;
    ResetCallback();
    CdInit();
    SsInit();
    ResetGraph(0);
    card_init();
    card_port_set(0);
    pad_start();
    cd_dir_init();
    xmalloc_init();
    VSync(0);
    GsInitGraph(0x140, 0xF0, 4, 1, 0);
    GsDefDispBuff(0, 0, 0, 0xE0);
    ClearImage((RECT *)g_ScreenClearRectangle, 0, 0, 0);
    GsInitVcount();
    packet_buf_fill();
    rng_seed(0x3039);
    sound_init();
    xa_file_set(g_SystemAudioXaFileName);
    if (((FileHostBlock *)g_FileHostBlock)->unk_0b != 5) {
        func_8001B02C();
    }
    g_DebugAllowed = 0;
    g_MapObjectTouchOn = 1;
    g_MapBtlWinForced = 0;
    D_800B8830 = 0;
    sprite_mix_init();
    bg_col_set(0x808080);
}

/* func_8001EFEC */
void screen_disp_mask_set(s8 on) {
    g_ScreenDispMask = on;
}
/* far_syms: g_VramStripRectangle (target reaches it %hi/%lo) */

/* func_8001EFF8 */
void vram_strip_clear(void) {
    ClearImage(g_VramStripRectangle, 0, 0, 0);
}
/* func_8001F028 */
void packet_buf_fill(void)
{
  s32 words_done;
  s32 buffer;
  s32 offset;
  buffer = 0;
  do
  {
    words_done = 0;
    offset = buffer << 0x10;
    ;
    loop_2:
    *((s32 *) (offset + g_PacketBufs)) = 0xDEAFBEEF;

    words_done += 1;
    offset += 4;
    if (words_done < 0x4000)
    {
      goto loop_2;
    }
    buffer = buffer + 1;
    words_done = 0;
  }
  while (buffer < 2);
}

/* far_syms: g_PacketBufs (target reaches it %hi/%lo) */
void screen_draw_area_set(void);  /* TU defines this below */

/* func_8001F074 */
void packet_buf_check(void)
{
  int i;
  int j;
  s32 off;
  for (i = 0; i < 2; i++)
  {
    j = 0x3FFF;
    off = (i << 16) | 0xFFFC;
    while (j > 0)
    {
      if ((*((s32 *) (g_PacketBufs + off))) != 0xDEAFBEEF)
      {
        break;
      }
      j--;
      off -= 4;
    }

  }

  return 0x3FFF;
}
/* la_absolute: screen_mode_set */
extern s32 g_ScreenModeWidths[];
extern s32 g_ScreenModeHeights[];
extern s32 D_800AE230[];
extern s32 g_ScreenModeRowsShown[];
extern RECT g_ScreenClipRectangle;
extern s32 D_800B8838;
extern s32 g_ScreenWidth;
extern s32 g_ScreenHeight;
extern s32 g_ScreenRowsShown;
extern GsOT_TAG g_FrameOtTables[2][0x400];
extern DRAWENV D_800E6F50;
extern DISPENV D_800E6FB0;
void SetDispMask(s32);
void GsSetClip2D(RECT *);

/* Screen-mode set-up: the main loop (main) calls it with 1 after
   system_init (the boot init) and main_buffer_init. arg0 (0..3) picks one
   entry of four
   word tables, copied to four globals, and is kept in g_ScreenMode (the frame
   flip frame_end skips its DrawSync in mode 3):
     g_ScreenModeWidths -> g_ScreenWidth  display width   256, 320, 640, 640
     g_ScreenModeHeights -> g_ScreenHeight  display height  240, 240, 240, 480
     D_800AE230 -> D_800B8838                  224, 224, 448, 448
     g_ScreenModeRowsShown -> g_ScreenRowsShown  visible height  224, 224, 224, 448
   Then, in order: VSync(0), SetDispMask(0),
   ClearImage of g_ScreenClearRectangle (0, 0, 320, 448: both buffers) to black,
   DrawSync(0), GsInitGraph(width, height, mode, 1, 0)
    with mode 4 (GsOFSGPU), or 5 (GsOFSGPU | GsINTER) when the
   height is 480, and GsDefDispBuff: both buffers at (0, 0)
   for 480 lines, the second one at y = 224 otherwise.
   The two ordering tables at g_FrameOts (GsOT[2], one per buffer; the TU
   declares the symbol u8[]) get length 10 and their 1024-entry row of
   g_FrameOtTables, and GsClearOt links each. offset = i * 224,
   point = 0 and tag are stored by hand first; GsClearOt(0, 0, ot) then
   stores offset 0, point 0 and the same tag again.
   Last: GsDISPENV (D_800E6FB0) screen.y = 8, screen.h = disp.h = the visible
   height; the clip rectangle g_ScreenClipRectangle (0, 0, 640, 480 in the image) gets
   w = the width and h = the visible height and goes to GsSetClip2D
   ; GsDRAWENV.isbg (D_800E6F50 + 0x18 = D_800E6F68) = 1, the
   background clear.

   Shape notes. The four globals are words (sw / lw); the lhu loads are the
   u_short parameters of GsInitGraph and the short members they are stored
   to. `mode` is the u_short the prototype asks for (an int local would be
   masked at the call). The same `== 0x1E0` test twice keeps the constant in
   a saved register. In the loop the member order decides which register
   each strength-reduced value gets: length first, offset before org, org
   before tag (point anywhere between length and tag), and the tag is
   spelled row + 0x400 - 1, which keeps the `- 4` inside the loop
   (&row[0x3FF] folds it into the start value: 111/115). g_ScreenClipRectangle is a
   small RECT whose address is taken with `la` (the la_absolute marker)
   while its members go through $gp; D_800E6F68 is a member of the far
   GsDRAWENV, which is what makes the compiler split its address. */
/* func_8001F0C8 */
void screen_mode_set(s32 screen_mode)
{
    GsOT *ot;
    u16 mode;
    s32 i;

    g_ScreenMode = screen_mode;
    g_ScreenWidth = g_ScreenModeWidths[screen_mode];
    g_ScreenHeight = g_ScreenModeHeights[screen_mode];
    D_800B8838 = D_800AE230[screen_mode];
    g_ScreenRowsShown = g_ScreenModeRowsShown[screen_mode];
    mode = 4;
    if (g_ScreenHeight == 0x1E0) {
        mode = 5;
    }
    VSync(0);
    SetDispMask(0);
    ClearImage((RECT *) g_ScreenClearRectangle, 0, 0, 0);
    DrawSync(0);
    GsInitGraph(g_ScreenWidth, g_ScreenHeight, mode, 1, 0);
    if (g_ScreenHeight == 0x1E0) {
        GsDefDispBuff(0, 0, 0, 0);
    } else {
        GsDefDispBuff(0, 0, 0, 0xE0);
    }
    for (i = 0; i < 2; i++) {
        ot = &((GsOT *) g_FrameOts)[i];
        ot->length = 10;
        ot->offset = i * 0xE0;
        ot->point = 0;
        ot->org = g_FrameOtTables[i];
        ot->tag = g_FrameOtTables[i] + 0x400 - 1;
    }
    GsClearOt(0, 0, &((GsOT *) g_FrameOts)[0]);
    GsClearOt(0, 0, &((GsOT *) g_FrameOts)[1]);
    D_800E6FB0.screen.y = 8;
    D_800E6FB0.screen.h = g_ScreenRowsShown;
    D_800E6FB0.disp.h = g_ScreenRowsShown;
    g_ScreenClipRectangle.w = g_ScreenWidth;
    g_ScreenClipRectangle.h = g_ScreenRowsShown;
    GsSetClip2D(&g_ScreenClipRectangle);
    D_800E6F50.isbg = 1;
}

s32 GsGetActiveBuff();

/* func_8001F294 */
void frame_start(void)
{
  s32 ot_base;
  s32 other_buffer;
  u8 *packet_areas;
  u8 *frame_ots;
  s32 active_buffer;
  void *ot;
  g_ProfileMarkCount = 0;
  active_buffer = GsGetActiveBuff();
 /* CODEGEN FORCING DEVICE (do-while(0) wrapper), not recovered source: the
    block runs once, so behaviour is the same without the `do` / `while (0)`, but then 11
    of this function's 28 instruction lines differ. */
 do {  other_buffer = active_buffer ^ 1; frame_ots = g_FrameOts; } while (0);
  ot = (other_buffer * 0x14) + frame_ots;
  ot_base = AS(s32, ((GsOT *)ot)->org);
  packet_areas = g_PacketBufs;
  *((void **) 0x1F80004C) = (other_buffer << 0x10) + packet_areas;
  g_FrameOt = ot;
  D_800B8848 = active_buffer;
  *((s32 *) 0x1F800040) = ot_base;
  GsClearOt(0, 0, ot);
}
/* far_syms: g_ProfileShown */
extern RECT g_ScreenDrawAreaX_blk;
extern s32 g_ProfileShown;
extern s32 g_FrameVSyncCount;
extern s32 D_800B8864;
extern s32 g_FramePreviousVSyncTotal;
void packet_draw_area_add(RECT *);
void profile_mark_add(s32);
void profile_start_set(void);
void profile_draw(void);
void SetDispMask(s32);

/* func_8001F30C */
void frame_end(void)
{
  RECT rect;
  s32 buf;
  buf = GsGetActiveBuff();
  rect = g_ScreenDrawAreaX_blk;
  if (buf == 0)
  {
    rect.y += 0xE0;
  }
  packet_draw_area_add(&rect);
  profile_mark_add(0x2100D000);
  if (g_ScreenMode != 3)
  {
    DrawSync(0);
  }
  profile_mark_add(0x210000FF);
  VSync(0);
  g_FrameVSyncTotal = VSync(-1);
  g_FrameVSyncCount = g_FrameVSyncTotal - g_FramePreviousVSyncTotal;
  g_FramePreviousVSyncTotal = g_FrameVSyncTotal;
  if (g_FrameVSyncCount <= 0)
  {
    g_FrameVSyncCount = 1;
  }
  if (g_FrameVSyncCount >= 4)
  {
    g_FrameVSyncCount = 4;
  }
  D_800B8864++;
  profile_start_set();
  pad_frame_step();
  ResetGraph(1);
  if (g_ProfileShown != 0)
  {
    profile_draw();
  }
  GsSwapDispBuff();
  SetDispMask(g_ScreenDispMask);
  GsDrawOt(g_FrameOt);
}

void layer_col_fade_step();                                  /* extern */
void frame_start();                                  /* extern */
void frame_end();                                  /* extern */

/* func_8001F458 */
void frame_step(void) {
    layer_col_fade_step();
    frame_end();
    frame_start();
}

/* func_8001F488 */
void screen_draw_area_set(void) {
    s32 *p;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;

    p = (s32 *)__builtin_frame_address(0);
    x0 = p[4];
    y0 = p[5];
    x1 = p[6];
    y1 = p[7];

    g_ScreenDrawAreaX = x0;
    g_ScreenDrawAreaY = y0;
    g_ScreenDrawAreaWidth = x1 - x0;
    g_ScreenDrawAreaHeight = y1 - y0;
}

s32 sprite_mix_switch_on();                                  /* extern */

/* func_8001F4B8 */
void layer_mix_switch_on(s32 layer_mask) {
    if (layer_mask == 0x100) {
        sprite_mix_switch_on();
    }
}
/* func_8001F4E0 */
void layer_mix_switch_off(s32 layer_mask) {
    if (layer_mask == 0x100) {
        sprite_mix_switch_off();
    }
}
