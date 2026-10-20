/* Lunar SSSC — start_screen: what the program shows from its start until
   the player has chosen: StartRun and its steps (picture, movie, screen,
   menu), the cards looked at for the start menu, the start's set-up,
   resource and records. The first split had it in rendering3.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "btl.h"
extern s32 g_PadPressedButtons;
s32 SoundPlay(s32, s32);
extern u16 g_BtlPadRecord[];
extern s8 g_BgMode;
/* far_syms: g_BtlRecords (target reaches it %hi/%lo) */
s32 MoviePlay(s32);
extern s16 g_MovieSkipAllowed;
void StartMoviePlay(void);  /* TU defines this below */
s32 tpage_word_make(s32 pixel_mode, s32 semi_rate, s32 vram_x, s32 vram_y);  /* TU defines this below */
s32 card_accept_poll();
extern s32 g_CardPort;
extern s32 g_StartMenuCardMask;
extern s8 g_StartMenuCursor;
void StartMenuCardUpdate(void);  /* TU defines this below */
s32 pad_record_init(s32 *, s32, s32, s32);
s32 StartRecordsClear();
void StartInit(void);  /* TU defines this below */
s32 bg_data_reset();
s32 bg_attribute_data_reset();
s32 resource_load_run(s32);
void StartResourceLoad(void);  /* TU defines this below */
extern s32 g_BtlRecords;
/* far_syms: g_BtlRecords */
s32 screen_fade_start(s32, s32);
s32 layer_col_set();
s32 layer_mix_set(s32, s32);
s32 layer_col_fade_check(s32);
s32 frame_step();
void StartScreenInit();
void StartScreenDraw(s32, s32, s32);
s32 MusicPlay(s32, s32, s32);
s32 SoundMapSet(s32);
s32 StartScreenShow(void);
s32 sprite_run_off(s32, s32);
s32 mem_clear(s32, s32);

/* far_syms: g_StartFirstStep (target reaches it %hi/%lo) */
/* far_syms: g_StartFirstStep (auto: target reaches it %hi/%lo — addressing row 3) */
extern u8 g_StartFirstStep;

/* func_8004345C */
s32 StartRun(s32 card_mask)
{
    s32 state;
    s32 result;
    s32 val;

    g_StartMenuCardMask = card_mask;
    layer_order_set(0x10C, 1);
    layer_mix_set(4, 0x20);
    layer_mix_set(8, 0);
    StartInit();

    state = g_StartFirstStep - 1;
    while ((state & 0xFF) != 0xFF)
    {
        switch (state & 0xFF)
        {
            case 0:
                state = (StartPictureLoadShow() & 0xFF) ? 2 : 1;
                break;
            case 1:
                StartMoviePlay();
                state = 2;
                break;
            case 2:
                state = (StartScreenShow() & 0xFF) ? 0xFF : 3;
                break;
            case 3:
                StartResourceLoad();
                state = 1;
                break;
        }
    }

    result = StartMenuRun() & 0xFF;
    MusicPlay(0, 2, 0);

    val = -1;
    if (result == 4)
        val = -2;

    layer_col_fade_start(0, 0x8013C, 1, 0x32, val);
    return result;
}

s32 StartPictureShow(s32);

static inline int inline_fn(s32 picture)
{
  return picture & 0xFF;
}
/* func_800435A4 */
s32 StartPictureLoadShow(void)
{
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `zero` only holds the constant 0; behaviour is the same with the constant
     written where it is read, but then 3 of this function's 22 instruction lines differ. */
  int zero;
  s32 picture;
  s32 passes_done;
  passes_done = 0;
  StartResourceLoad();
  picture = 0;
  loop_1:
  passes_done += 1;

  if (StartPictureShow(inline_fn(picture)) != 0)
  {
    zero = 0;
    return 1;
 do { } while (zero);
  }
  picture = passes_done;
  if (passes_done & 0xFF)
  {
    SetDispMask(0);
    return 0;
  }
  goto loop_1;
}

s32 MoviePlay(s32);                                 /* extern */
extern s16 g_MovieSkipAllowed;

/* func_800435FC */
void StartMoviePlay(void) {
    g_MovieSkipAllowed = 0x800;
    MoviePlay(0);
}
/* func_80043624 */
s32 tpage_word_make(s32 pixel_mode, s32 semi_rate, s32 vram_x, s32 vram_y) {
    return ((pixel_mode & 3) << 7) | ((semi_rate & 3) << 5) | ((u32) (vram_y & 0x100) >> 4) | ((u32) (vram_x & 0x3FF) >> 6);
}
s32 card_accept_poll();                                /* extern */
extern s32 g_CardPort;
extern s32 g_StartMenuCardMask;
extern s8 g_StartMenuCursor;

/* func_80043654 */
void StartMenuCardUpdate(void) {
    if (card_accept_poll() == 1) {
        g_StartMenuCardMask &= 1 << ((s32) g_CardPort >> 4);
        g_CardPort ^= 0x10;
    } else {
        g_StartMenuCardMask |= 1 << ((s32) g_CardPort >> 4);
    }
    if ((g_StartMenuCardMask == 0) && (g_StartMenuCursor == 5)) {
        g_StartMenuCursor = 4;
    }
}
extern s16 g_MapViewX;
extern s16 g_MapViewY;
extern s16 g_MapSecondViewX;
extern s16 g_MapSecondViewY;
extern s16 g_MapViewKeptX;
extern s16 g_MapViewKeptY;
extern u16 g_MapEffect;
s32 DrawSync(s32);
s32 MapViewStep(s32, s32);
void MapFrameEnd(void);

/* The title screen's white-flash palettes: StartScreenShow (the logo step)
   calls it before its loop.

   1. StoreImage reads the 256-entry CLUT row at VRAM
      (0, 0x1F8) into buf[0]; DrawSync(0).
   2. For i = 1..32: every entry but entry 0 is blended toward white by
      i / 32, per 5-bit channel (ch * (32 - i) + 31 * i) >> 5, and gets
      bit 15; entry 0 is copied as read. The row is written to VRAM
      (0x200, 0xDF + i) with LoadImage, then DrawSync(0). So rows
      0xE0..0xFF at x 0x200 hold the palette from 1/32 white to all white:
      StartScreenDraw draws its phase-2 sprite with CLUT row 0xFF - frame
      there (((0xFF - frame) << 6) + 0x20) and with 0x7E00, the source
      row, in the later phases.
   3. The map-effect flags g_MapEffect, the second scroll g_MapSecondViewX /
      g_MapSecondViewY, the camera g_MapViewX / g_MapViewY and the recorded
      camera g_MapViewKeptX / g_MapViewKeptY are cleared; MapViewStep(0, 1)
      (the second scroll's update) and MapFrameEnd() (one frame).

   No RNG draw. 33 DrawSync(0) calls and one frame.

   Shape: the two rows are ONE array, buf[2][0x100], so the source and the
   destination share a walked pointer (lhu 0(p) / sh 0x200(p)); two arrays
   get a pointer each. The three channels are masked off first with
   c >>= 5 between them and blended afterwards, 31 * i written before the
   product. The constant 0x8000 is written ahead of (g << 5): fold moves it
   onto the next operand, which is where the target ORs it in. */
/* func_800436F0 */
void StartScreenInit(void)
{
    u16 buf[2][0x100];
    RECT rect;
    s32 i;
    s32 j;
    u32 c;
    u32 r;
    u32 g;
    u32 b;

    rect.x = 0;
    rect.y = 0x1F8;
    rect.w = 0x100;
    rect.h = 1;
    StoreImage(&rect, (u_long *)buf[0]);
    DrawSync(0);
    rect.x = 0x200;
    for (i = 1; i < 0x21; i++) {
        for (j = 0; j < 0x100; j++) {
            c = buf[0][j];
            if (j > 0) {
                r = c & 0x1F;
                c >>= 5;
                g = c & 0x1F;
                c >>= 5;
                b = c & 0x1F;
                r = (i * 0x1F + r * (0x20 - i)) >> 5;
                g = (i * 0x1F + g * (0x20 - i)) >> 5;
                b = (i * 0x1F + b * (0x20 - i)) >> 5;
                c = 0x8000 | r | (g << 5) | (b << 10);
            }
            buf[1][j] = c;
        }
        rect.y = i + 0xDF;
        LoadImage(&rect, (u_long *)buf[1]);
        DrawSync(0);
    }
    g_MapEffect = 0;
    g_MapSecondViewY = 0;
    g_MapSecondViewX = 0;
    g_MapViewY = 0;
    g_MapViewX = 0;
    g_MapViewKeptY = 0;
    g_MapViewKeptX = 0;
    MapViewStep(0, 1);
    MapFrameEnd();
}

s32 layer_col_fade_step();
s32 bg_draw();
s32 MessageShadeDraw();
s32 packet_sprite_add(s32, s32, s32, s32, s32, s32, s32, s32, s32);

/* func_80043848 */
void StartScreenDraw(s32 phase, s32 frame, s32 cursor)
{
    s32 u;
    s32 t;
    s32 x;

    layer_col_fade_step();
    bg_draw();
    MessageShadeDraw();
    packet_sprite_add(0xF, 0xBC, 0, 0, 0x100, 0x30, tpage_word_make(1, 0, 0x180, 0), 0x7E00, 0x10);
    packet_sprite_add(0x10F, 0xBC, 0, 0x90, 0x20, 0x30, tpage_word_make(1, 0, 0x180, 0), 0x7E00, 0x10);
    if (phase >= 0x10) {
        packet_sprite_add(0x69, 0x90, 0, 0x10, 0x68, 0x10, tpage_word_make(1, 0, 0x380, 0), 0x7E40, 0x10);
        if (g_StartMenuCardMask != 0) {
            packet_sprite_add(0x69, 0x9D, 0x80, 0x10, 0x78, 0x10, tpage_word_make(1, 0, 0x380, 0), 0x7E40, 0x10);
        } else {
            packet_sprite_add(0x71, 0x9D, 0x80, 0x20, 0x70, 0x10, tpage_word_make(1, 0, 0x380, 0), 0x7E40, 0x10);
        }
        packet_sprite_add(0x69, 0xAA, 0, 0x30, 0x50, 0x10, tpage_word_make(1, 0, 0x380, 0), 0x7E40, 0x10);
        t = frame % 40;
        if (t >= 0x14) {
            t = 0x27 - t;
        }
        x = (cursor - 4) * 0xD;
        packet_sprite_add(0x5D, x + 0x90, (t / 2) * 0x10, 0, 0x10, 0x10, tpage_word_make(1, 0, 0x380, 0), 0x7E40, 0x10);
    }
    if (phase == 3) {
        u = frame % 90;
        if (u < 0xE) {
            u = (u + 1) * 0x10;
        } else {
            u = 0;
        }
        packet_sprite_add(0x2D, 0x99, 0, u, 0xF8, 0x10, tpage_word_make(1, 0, 0x300, 0), 0x7E00, 0x10);
    }
    if (phase == 1) {
        packet_sprite_add(0x2D, 0x34, 0, 0x30, 0xF0, 0x58, tpage_word_make(1, 0, 0x180, 0), ((frame + 0xE0) << 6) + 0x20, 0x10);
    } else if (phase == 2) {
        packet_sprite_add(0x2D, 0x34, 0, 0x30, 0xF0, 0x58, tpage_word_make(1, 0, 0x180, 0), ((0xFF - frame) << 6) + 0x20, 0x10);
    } else {
        packet_sprite_add(0x2D, 0x34, 0, 0x30, 0xF0, 0x58, tpage_word_make(1, 0, 0x180, 0), 0x7E00, 0x10);
    }
}

s32 pad_record_init(s32 *, s32, s32, s32);
s32 StartRecordsClear();
void StartInit(void);  /* TU defines this below */
s32 bg_data_reset();
s32 bg_attribute_data_reset();
s32 resource_load_run(s32);
void StartResourceLoad(void);  /* TU defines this below */
s32 screen_fade_start(s32, s32);
s32 layer_col_set();
s32 layer_mix_set(s32, s32);
s32 layer_col_fade_check(s32);
s32 frame_step();
void StartScreenInit();
void StartScreenDraw(s32, s32, s32);
s32 MusicPlay(s32, s32, s32);
s32 SoundMapSet(s32);

/* func_80043C64 */
s32 StartScreenShow(void)
{
  s32 fading;
  s32 phase;
  s32 frame;
  unsigned short first_phase;
  int prompt_phase;
  s32 prompt_phase_then_frame;
  StartScreenInit();
  prompt_phase_then_frame = 3;
  layer_mix_set(4, 0x20);
  layer_mix_set(8, 0x20);
  layer_col_set(0, 0xC, 0);
  frame = 0x20;
  screen_fade_start(frame, 2);
  phase = 0;
  frame = 0;
  prompt_phase = prompt_phase_then_frame;
  loop_1:
  StartScreenDraw(phase, frame, 4);

  if (phase > 0)
  {
    if (phase >= 3)
    {
      if (phase == prompt_phase)
      {
        prompt_phase_then_frame = 0x1388;
        prompt_phase_then_frame = frame;
        prompt_phase_then_frame = prompt_phase_then_frame < prompt_phase_then_frame;
        if (frame == 0)
        {
          SoundMapSet(1);
          MusicPlay(0x17, 0, 0);
          ;
        }
        if (((long long) (frame < 0x1388)) == 0)
        {
          frame = -1;
          phase = 4;
          MusicPlay(0, 2, 0);
          screen_fade_start(0x20, 3);
        }
      }
    }
    else
      if (frame >= 0x1F)
    {
      frame = -1;
      phase += 1;
    }
  }
  fading = layer_col_fade_check(0) & 0xFF;
  if (phase == 0)
  {
    if (fading == 0)
    {
      phase = (first_phase = 1);
      frame = -1;
      goto block_20;
    }
    goto block_14;
  }
  block_14:
  if (((phase == 0x11) && (fading == 0)) && (frame >= 0x2E))
  {
    return first_phase;
    prompt_phase = 3;
  }

  if ((phase != 4) || (fading != 0))
  {
    block_20:
    frame_step();

    if (((g_PadPressedButtons & 0x800) && (phase == prompt_phase)) && (frame > 0))
    {
      SoundPlay(2, 3);
      return 1;
    }
    frame += 1;
    if (phase < 0)
    {
      return 0;
    }
    goto loop_1;
  }
  return 0;
}
/* StartMenuRun -- the TITLE SCREEN's menu step. GameStart (the game's
   start: heap pointers, the opening, then this screen, then
   GameEnter enters the field with the result) calls StartRun,
   which runs the logo / prompt steps (StartScreenShow) and then this; the
   draw is StartScreenDraw(0x10, frame, row).

   Ten VSyncs (VSync(0)) each followed by a pad read flush the pad
   record g_BtlPadRecord, then screen_fade_start(0, 0). The cursor g_StartMenuCursor is
   the row, 4..6, starting on 5 when g_StartMenuCardMask is non-zero and on 4
   otherwise. g_StartMenuCardMask is the mask StartMenuCardUpdate keeps from the
   memory-card poll card_accept_poll (one bit per port; StartRun seeds
   it with card_accept_wait() != 1). The middle row can be selected only
   while it is non-zero; StartScreenDraw draws its other variant otherwise.

   One frame per pass while the row is 4..6: StartScreenDraw draws,
   StartMenuCardUpdate polls the cards (and moves the cursor off row 5 if the
   mask went to 0), frame_end / frame_start flip the frame,
   pad_record_update reads the pad. By the new presses g_BtlPadRecord[1]:
     0x840 (Start or Cross, by the standard pad bits): sound 2, by row --
        4: return 4 (new game: GameEnter's id 4);
        5: StartMenuPageRun(0x14), the load page run modally; a non-zero
           result (& 0xFF) is returned, 0 comes back to this menu;
        6: StartMenuPageRun(0x15), the Options page, then back here.
     0x1000 (up):   sound 1; 4 -> 6, 5 -> 4, 6 -> 5 (4 without a card).
     0x4000 (down): sound 1; 4 -> 5 (6 without a card), 5 -> 6, 6 -> 4.
   The confirm and the move are tested on the same frame's word, so a
   press that returns from a sub-page can also move the cursor.
   Leaving the loop any other way returns 4.

   No RNG draw of its own; its frames are the flips (the vibration driver
   runs under frame_end -- frame-rng.md draw site 2).

   Shape: the two-armed `if (g_StartMenuCardMask != 0) g_StartMenuCursor = 5; else
   g_StartMenuCursor = 4;` leaves two stores for cross-jumping to merge after cse,
   which is why the loop test RELOADS the byte it just stored; the loop
   test reads the s8 as lbu and the call's argument re-extends it
   (sll / sra) -- both fall out of the plain range test. */
/* func_80043E08 */
s32 StartMenuRun(void)
{
    s32 i;
    s32 r;

    for (i = 0; i < 10; i++) {
        VSync(0);
        pad_record_update(g_BtlPadRecord, 0);
    }
    screen_fade_start(0, 0);
    if (g_StartMenuCardMask != 0) {
        g_StartMenuCursor = 5;
    } else {
        g_StartMenuCursor = 4;
    }
    while (g_StartMenuCursor >= 4 && g_StartMenuCursor < 7) {
        StartScreenDraw(0x10, i, g_StartMenuCursor);
        i++;
        StartMenuCardUpdate();
        frame_end();
        frame_start();
        pad_record_update(g_BtlPadRecord, 0);
        if (g_BtlPadRecord[1] & 0x840) {
            SoundPlay(2, 3);
            switch (g_StartMenuCursor) {
            case 4:
                return 4;
            case 5:
                r = StartMenuPageRun(0x14) & 0xFF;
                if (r != 0) {
                    return r;
                }
                break;
            case 6:
                StartMenuPageRun(0x15);
                break;
            }
        }
        if (g_BtlPadRecord[1] & 0x1000) {
            SoundPlay(1, 3);
            switch (g_StartMenuCursor) {
            case 4:
                g_StartMenuCursor = 6;
                break;
            case 5:
                g_StartMenuCursor = 4;
                break;
            case 6:
                if (g_StartMenuCardMask != 0) {
                    g_StartMenuCursor = 5;
                } else {
                    g_StartMenuCursor = 4;
                }
                break;
            }
        } else if (g_BtlPadRecord[1] & 0x4000) {
            SoundPlay(1, 3);
            switch (g_StartMenuCursor) {
            case 4:
                if (g_StartMenuCardMask != 0) {
                    g_StartMenuCursor = 5;
                } else {
                    g_StartMenuCursor = 6;
                }
                break;
            case 5:
                g_StartMenuCursor = 6;
                break;
            case 6:
                g_StartMenuCursor = 4;
                break;
            }
        }
    }
    return 4;
}

s32 pad_record_init(s32 *, s32, s32, s32);                      /* extern */
s32 StartRecordsClear();                                  /* extern */

/* func_80044064 */
void StartInit(void) {
    pad_record_init((s32 *)g_BtlPadRecord, 2, 0, 0);
    StartRecordsClear();
}
s32 bg_data_reset();                                  /* extern */
s32 bg_attribute_data_reset();                                  /* extern */
s32 resource_load_run(s32);                                 /* extern */

/* func_8004409C */
void StartResourceLoad(void) {
    resource_load_run(3);
    g_BgMode = -0x80;
    bg_data_reset();
    bg_attribute_data_reset();
}
s32 sprite_run_off(s32, s32);                            /* extern */
s32 mem_clear(s32, s32);                              /* extern */

/* func_800440D4 */
s32 StartRecordsClear(void) {
    g_BtlRecords = 0x801A9B00;
    mem_clear(0x801A9B00, 0x700);
    return sprite_run_off(0, 0x96);
}
