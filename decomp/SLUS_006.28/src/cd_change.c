/* Lunar SSSC — cd_change: the change of discs: the drive's status and seek
   check and the wait for the lid, with screen_picture_put (the picture it
   shows) and vsync_wait (its waits) lone inside. The first split had it in
   cdrom_load.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "cd.h"
#include "packet.h"
#include "sound.h"

extern u8 D_80011884[];
s32 VSync(s32);
void vsync_wait(s32 vblank_count);  /* TU defines this below */
void ankprint_clear(void);  /* extern */
extern int CdSync(int mode, u_char *result);
s32 frame_step();
s32 screen_disp_mask_set(s32);
s32 pad_frame_step();
s32 CdStatus();
s32 cd_status_get(void);
s32 cd_seek_check(void);
extern int CdSync(int mode, u_char *result);
extern int CdSync(int mode, u_char *result);
s32 VSync(s32);
void screen_picture_put();
s32 frame_step();
s32 screen_disp_mask_set(s32);

s32 VSync(s32);                                 /* extern */

/* func_80024B70 */
void vsync_wait(s32 vblank_count) {
    s32 i;

    i = 0;
    if (vblank_count > 0) {
        do {
            VSync(0);
            i += 1;
        } while (i < vblank_count);
    }
}
s32 pad_frame_step();
s32 CdStatus();

/* func_80024BB8 */
s32 cd_status_get(void)
{
  /* CODEGEN FORCING DEVICE (copy of a copy), not recovered source:
     `sync_mode` only copies `zero`, which is not assigned again afterwards; behaviour
     is the same with `zero` used directly, but then 4 of this function's 19
     instruction lines differ. */
  long long sync_mode;
  /* CODEGEN FORCING DEVICE (constant in a local), not recovered source:
     `zero` only holds the constant 0; behaviour is the same with the constant
     written where it is read, but then 4 of this function's 19 instruction lines differ. */
  int zero;
  u8 result;
  zero = 0;
  VSync(zero);
  pad_frame_step();
  CdControl(1U, zero, zero);
  sync_mode = zero;
  CdSync(sync_mode, &result);
  return CdStatus();  /* the CD status: cd_change_wait tests its bits */
}
extern RECT g_ScreenPictureRowRectangle[];
extern s32 g_LoadBuffer;
s32 GsGetActiveBuff(void);
s32 DrawSync(s32);

/* screen_picture_put -- PUT THE FULL-SCREEN PICTURE IN THE LOAD BUFFER ON
   THE SCREEN: the 320 x 224 8-bit picture at g_LoadBuffer is turned into
   15-bit colour one row at a time, and each row is loaded into the frame
   buffer that is on show.

   Three call sites: cd_change_wait (below; the "insert disc" screen of the
   disc check) and GameRun twice (map_frame.c; entries 4 and 5 of
   resource 599 after the end of the game). Each one first inflates the
   picture into the buffer g_LoadBuffer points at with resource_tim_unpack(n) and
   calls screen_disp_mask_set(1) and SetDispMask(1) afterwards. No arguments,
   nothing returned (no caller reads $v0).

   The buffer is read at two offsets only, those of a TIM with one
   256-colour CLUT: +0x14 (8 bytes of file header and the CLUT block's 12)
   the 256 u16 colours, +0x220 (the image block's 12 bytes after them) the
   pixels, one byte each, 320 to a row. No header field is read: the size
   320 x 224 is in the code.

   buf = GsGetActiveBuff()        libgs PSDIDX, the draw buffer's index, read
                                once
   for y = 0 .. 223:
     rect = {0, y, 320, 1}      the stack copy of g_ScreenPictureRowRectangle = {0, 0, 320,
                                1}, with y written each row; y + 224 when
                                buf is 0. The game's two buffers are at
                                (0, 0) and (0, 224) (frame.c's
                                GsDefDispBuff(0, 0, 0, 0xE0)), so the row
                                goes into the buffer that is NOT being
                                drawn, the one on show.
     line[x] = clut[pixels[y * 320 + x]]  for x = 0 .. 319 (u16 line[320]
                                on the stack)
     LoadImage(&rect, line)
     DrawSync(0)           DrawSync(0): wait for that load

   Reads g_ScreenPictureRowRectangle, g_LoadBuffer and the buffer; writes VRAM only. No RNG
   draw; no frame wait of its own (224 DrawSync(0) waits).

   Shapes the bytes depend on (each measured by changing it alone):
   - g_ScreenPictureRowRectangle is an unsized array read `[0]`, as icon.c's
     g_IconImageRectangle: the address is then cc1's split `lui $v0` /
     `addiu $t2,$v0,%lo`. A sized `extern RECT` with far_syms is one `la`
     on $t2, 53/55. In the original this is most likely the initialised
     local `RECT rect = {0, 0, 320, 1};`, whose constant GCC puts in
     .sdata under -G8 and still addresses as a far label; it stays a copy
     from the block while the data lives in the asm data file.
   - both pointers are set to the buffer and then stepped in place, in
     this order: `src += 0x220; pal += 10; src += y * 0x140;`. That is the
     target's `addu $a2,$v1,$zero` / `addiu $v1,$v1,0x220` /
     `addiu $a2,$a2,0x14` / `addu $v1,$v1,$s1`. With the offsets written
     into the assignments (`pal = (u16 *)(g_LoadBuffer + 0x14)`) the copy is
     folded into one `addiu $a2,$v1,0x14`, 46-48/55; `pal += 10` ahead of
     `src += 0x220` is 52/55. `pal = (u16 *)src` for the second read of
     the global builds the same bytes.
   - `dst = line` ahead of the other two (after them: 54/55).
   - the row is copied through stepped pointers (`line[x] = pal[src[x]]`:
     36/55, the loop pass keeps the row's address in a saved register).
   - `rect.y = y;` and then the `buf == 0` overwrite (if / else: 52/55).
   - buf is an int and GsGetActiveBuff returns int, as libgs.h's
     GsGetActiveBuff does (the `s16 (void)` prototype other TUs give it:
     54/55, and the same for an `s16 buf`). */
/* func_80024C04 */
void screen_picture_put(void)
{
    RECT rect = g_ScreenPictureRowRectangle[0];
    u16 line[0x140];
    u16 *dst;
    u8 *src;
    u16 *pal;
    s32 buf;
    s32 y;
    s32 x;

    buf = GsGetActiveBuff();
    for (y = 0; y < 0xE0; y++) {
        rect.y = y;
        if (buf == 0) {
            rect.y = y + 0xE0;
        }
        dst = line;
        src = (u8 *) g_LoadBuffer;
        pal = (u16 *) g_LoadBuffer;
        src += 0x220;
        pal += 10;
        src += y * 0x140;
        for (x = 0; x < 0x140; x++) {
            *dst++ = pal[*src++];
        }
        LoadImage(&rect, (u_long *) line);
        DrawSync(0);
    }
}

/* far_syms: D_80011884 (target reaches it %hi/%lo) */

/* func_80024CE0 */
s32 cd_seek_check(void) {
    u8 D_80011884_local[0x18];

    CdIntToPos(0x16, D_80011884_local);
    CdControlB(0x15, (s32 *)D_80011884_local, (s32)&D_80011884_local[8]);
    debug_printf(D_80011884, D_80011884_local[8], D_80011884_local[9]);

    return D_80011884_local[8] & 5;
}
/* far_syms: g_CdChangeMode (target reaches it %hi/%lo) */
s32 resource_tim_unpack(s32);
extern s32 g_CdChangeMode;

/* func_80024D2C */
s32 cd_change_wait(s32 picture)
{
  s32 result[4];
  s32 lid_open_count;
  ankprint_clear();
  frame_step();
  SetDispMask(0);
  resource_tim_unpack(picture);
  screen_picture_put();
  screen_disp_mask_set(1);
  SetDispMask(1);
  do
  {
  }
  while (CdControlB(0xE, &g_CdChangeMode, (s32)result) == 0);
  vsync_wait(3);
  do
  {
  }
  while (CdControlB(8, 0, (s32)result) == 0);
  lid_open_count = 0;
  do
  {
    if (cd_status_get() & 0x10)
    {
      lid_open_count += 1;
    }
  }
  while (lid_open_count < 4);
  do
  {
  }
  while (cd_status_get() & 0x10);
  do
  {
  }
  while (!(cd_status_get() & 2));
  do
  {
  }
  while (CdControlB(0x13, 0, (s32)result) == 0);
  ankprint_clear();
  screen_disp_mask_set(0);
  frame_step();
  return 0;
}
