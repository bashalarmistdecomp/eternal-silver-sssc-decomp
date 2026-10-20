/* Lunar SSSC — tim: a TIM sent to video memory, at its own place or at a
   given one. The first split had it in main.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;

extern s32 D_8010D424[];
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

s32 DrawSync(s32 arg0);

/* Measured arities — outgoing arg-slot stores (sw 0x10..0x1C($sp)) at
   every call site in the two giants.
   The variadic two are per-site variant (6/7 and 4/6 args at different
   sites): unprototyped originals, so `...` is the honest model. */
s32 tim_image_load_at(s32, s16, s32, s16, s32, s32);
s32 VSync(s32);
s32 tim_image_load(s32 arg0);

/* func_8011E044 */
s32 tim_image_load_at(s32 arg0, s16 arg1, s32 arg2, s16 arg3, s32 arg4, s32 arg5)
{
  typedef struct { s32 w[4]; } B16_8011E044;
  s16 sp10[4];
  u8 sp18[0x20];
  s32 sp38[4];
  *((B16_8011E044 *) sp38) = *((B16_8011E044 *) D_8010D424);
  GsGetTimInfo(arg0 + 4, sp18);
  sp10[0] = arg1;
  sp10[1] = arg2;
  sp10[2] = *((u16 *) (sp18 + 0x8));
  sp10[3] = *((u16 *) (sp18 + 0xA));
  LoadImage(sp10, *((s32 *) (sp18 + 0xC)));
  DrawSync(0);
  VSync(0);
  if ((arg3 > 0) && (((*((u32 *) sp18)) >> 3) & 1))
  {
    do
    {
      if (arg3 < 2)
      {
        sp10[0] = *((u16 *) (sp18 + 0x10));
        sp10[1] = *((u16 *) (sp18 + 0x12));
      }
      else
      {
        sp10[0] = (u16) arg4;
        sp10[1] = (u16) arg5;
      }
      sp10[2] = *((u16 *) (sp18 + 0x14));
      sp10[3] = *((u16 *) (sp18 + 0x16));
      LoadImage(sp10, *((s32 *) (sp18 + 0x18)));
      DrawSync(0);
      VSync(0);
    }
    while (0);
  }
  do
  {
  }
  while (DrawSync(1) != 0);
  return (sp38[(*((u32 *) sp18)) & 7] * (*((u16 *) (sp18 + 0x8)))) / 12;
}

/* 16-byte block copy (lwl/lwr x2) of the bytes-per-8-pixels table at
 * D_8010D424, indexed by GsIMAGE.pmode — attempt-local type, carried
 * with the paste (paste_chunk excludes typedef bodies by design) */
typedef struct { long a[4]; } B16;

/* func_8011E1C8 */
s32 tim_image_load(s32 arg0)
{
  RECT sp10;
  GsIMAGE sp18;
  B16 sp38;
  sp38 = *((B16 *) D_8010D424);
  GsGetTimInfo(arg0 + 4, &sp18);
  sp10.x = sp18.px;
  sp10.y = sp18.py;
  sp10.w = sp18.pw;
  sp10.h = sp18.ph;
  LoadImage(&sp10, sp18.pixel);
  DrawSync(0);
  VSync(0);
  if ((sp18.pmode >> 3) & 1)
  {
    sp10.x = sp18.cx;
    sp10.y = sp18.cy;
    sp10.w = sp18.cw;
    sp10.h = sp18.ch;
    LoadImage(&sp10, sp18.clut);
    DrawSync(0);
    VSync(0);
  }
  do
  {
  }
  while (DrawSync(1) != 0);
  return (sp38.a[sp18.pmode & 7] * (sp18.pw)) / 12;
}
