/* Lunar SSSC — castle: the Castle minigame and the loop that runs it: main
   (sound and pad started, then the movie, the title, the options or the
   demo, and the game, round and round), CastleTitleRun, CastleDemoSet,
   CastleOptionsRun, CastleBounce, CastleGameRun, CastleQuadsInit,
   CastlePadStep and CastleSpeedVary. The first split had it in main.c. C,
   with 5 routines still read as byte ranges (INCLUDE_ASM).
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;
s32 pad_buffer_decode(s32 *, s32);

extern u8 D_801352A0[];
/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

extern u8 D_8013520C[];
extern u8 D_801351F2;

/* Measured arities — outgoing arg-slot stores (sw 0x10..0x1C($sp)) at
   every call site in the two giants.
   The variadic two are per-site variant (6/7 and 4/6 args at different
   sites): unprototyped originals, so `...` is the honest model. */
void CastleBounce(s32, s32, s32, s32, s32 *, s32 *, s32);
s32 memset(s32 *, s32, s32);
void font_hex_print(u8 *arg0, s32 arg1);
s32 rand(void);
void printf();
void pad_start(void);  /* extern */
void screen_init(void);  /* extern */
void sound_init(void);  /* extern */

s32 CastleTitleRun();                                /* extern */
void CastleDemoSet(void);  /* TU defines this below */
s32 CastleOptionsRun();                                  /* extern */
s32 CastleGameRun();                                  /* extern */
s32 movie_run();                                /* extern */
extern u8 D_80135458[];

/* The image's first instruction here is `jal __main`. No call is written for it: the
   compiler puts one at the top of any function named main. */
/* func_8010E714 */
void main(void) {
    s32 var_v0;

    sound_init();
    pad_start();
    for (;;) {
        loop_1:
        if (movie_run() != 2) {
            goto loop_1;
        }
        screen_init();
        memset(D_801352A0, 0, 0xB4);
        memset(D_80135458, 0, 0x3258);
        var_v0 = CastleTitleRun();
        if (var_v0 == 1) {
            continue;
        }
        do {
            if (var_v0 != 0x64) {
                CastleOptionsRun();
            } else {
                CastleDemoSet();
            }
            CastleGameRun();
            memset(D_801352A0, 0, 0xB4);
            memset(D_80135458, 0, 0x3258);
            var_v0 = CastleTitleRun();
        } while (var_v0 != 1);
    }
}

/* func_8010E7F4 */
INCLUDE_ASM("asm/nonmatchings/main", CastleTitleRun);

/* func_8010F228 */
INCLUDE_ASM("asm/nonmatchings/main", pad_buffer_decode);

extern u8 D_800564A4[];
extern u8 D_8010D26C[];
extern u8 D_80131598[];
s32 tim_image_load(s32 arg0);  /* extern */
extern s32 D_80135248;
extern s32 D_8013524C;

/* func_8010F498 */
void CastleDemoSet(void)
{
  int new_var4;
  u8 **new_var2;
  u8 *new_var3;
  u8 *var_a1;
  s32 var_a2;
  unsigned int var_v1;
  u8 *new_var;
  printf(D_8010D26C);
  tim_image_load(D_800564A4);
  var_a2 = 0;
  new_var4 = 0xFF;
  new_var = (u8 *) (&D_80131598);
  D_801351F2 = 1;
  D_8013524C = (s32) (*((s16 *) (new_var + 0x58)));
  D_80135248 = (s32) (*((s16 *) (new_var + 0x72)));
  do
  {
    var_a1 = ((u8 *) D_801352A0) + (var_a2 * 0x14);
    var_v1 = var_a2;
    var_a1[0] = new_var4;
    *(((u8 *) (*(new_var2 = &var_a1))) + 0x1) = 1;
    if (var_a2 >= 5)
    {
      var_v1 = var_a2 + 1;
    }
    *((s8 *) (((u8 *) var_a1) + 0x2)) = var_v1;
    *((s8 *) (((u8 *) var_a1) + 0x3)) = var_a2;
    *((s8 *) (((u8 *) var_a1) + 0x5)) = 0;
    *((u8 *) (((u8 *) var_a1) + 0x10)) = var_a2;
    *((s8 *) ((new_var3 = (u8 *) var_a1) + 0x7)) = 0;
    var_a1[8] = 0;
    *(new_var3 + 0x9) = (s8) (D_8013524C * D_80135248);
    *((s32 *) (((u8 *) var_a1) + 0xC)) = 0;
    var_a1[4] = 0;
    var_a2 += 1;
  }
  while (var_a2 < 9);
}

/* func_8010F55C */
INCLUDE_ASM("asm/nonmatchings/main", CastleOptionsRun);

extern s32 D_80135270;
extern s32 D_80135274;
extern s32 D_80135288;
extern s32 D_8013528C;

/* func_80114780 */
void CastleBounce(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 *arg4,
                   s32 *arg5, s32 arg6)
{
    u8 yUnderMax;
    u8 yOverMin;
    u8 xUnderMax;
    u8 xOverMin;
    u8 otherAbove;
    u8 otherBelow;
    u8 otherLeft;
    u8 otherRight;
    u8 clearAbove;
    u8 clearBelow;
    u8 clearLeft;
    u8 clearRight;
    s32 v;

    yUnderMax = arg1 < 0x1C0 - D_80135274;
    yOverMin = D_80135274 < arg1;
    xUnderMax = arg0 < 0x26C - D_80135270;
    xOverMin = D_80135270 < arg0;
    otherAbove = arg3 < arg1;
    otherBelow = arg1 < arg3;
    otherLeft = arg2 < arg0;
    otherRight = arg0 < arg2;
    clearAbove = arg3 < arg1 - D_8013528C;
    clearBelow = arg1 + D_8013528C < arg3;
    clearLeft = arg2 < arg0 - D_80135288;
    clearRight = arg0 + D_80135288 < arg2;

    if (clearAbove && clearLeft && yOverMin && xOverMin) {
        if (arg6 == 0) {
            v = *arg4;
            if (v > 0) {
                *arg4 = -v;
            }
        } else {
            v = *arg5;
            if (v > 0) {
                *arg5 = -v;
            }
        }
        return;
    }
    if (clearAbove && clearRight && yOverMin && xUnderMax) {
        if (arg6) {
            v = *arg5;
            if (v > 0) {
                *arg5 = -v;
            }
        } else {
            v = *arg4;
            if (v < 0) {
                *arg4 = -v;
            }
        }
        return;
    }
    if (clearBelow && clearLeft && yUnderMax && xOverMin) {
        if (arg6 == 0) {
            v = *arg4;
            if (v > 0) {
                *arg4 = -v;
            }
        } else {
            v = *arg5;
            if (v < 0) {
                *arg5 = -v;
            }
        }
        return;
    }
    if (clearBelow && clearRight && yUnderMax && xUnderMax) {
        if (arg6) {
            v = *arg5;
            if (v < 0) {
                *arg5 = -v;
            }
        } else {
            v = *arg4;
            if (v < 0) {
                *arg4 = -v;
            }
        }
        return;
    }
    if (otherBelow && yUnderMax && arg6 == 1) {
        v = *arg5;
        if (v < 0) {
            *arg5 = -v;
        }
    }
    if (otherAbove && yOverMin && arg6 == 1) {
        v = *arg5;
        if (v > 0) {
            *arg5 = -v;
        }
    }
    if (otherLeft && xOverMin && arg6 == 0) {
        v = *arg4;
        if (v > 0) {
            *arg4 = -v;
        }
    }
    if (otherRight && xUnderMax && arg6 == 0) {
        v = *arg4;
        if (v < 0) {
            *arg4 = -v;
        }
    }
}

/* func_80114A0C */
INCLUDE_ASM("asm/nonmatchings/main", CastleGameRun);

/* func_8011D27C */
void CastleQuadsInit(void *arg0)
{
  u8 sp10[0x40];
  s32 var_s3;
  u8 *var_s1;
  u8 *var_s2;
  s32 lit_0xE8;
  *((u8 *) (((u8 *) arg0) + 0x18)) = 1;
  *((u8 *) (((u8 *) arg0) - (-0x19))) = 0;
  *((s8 *) (((u8 *) arg0) + 0x1a)) = 0;
  *((u8 *) (((u8 *) arg0) + 0x1b)) = 0;
  *((u16 *) (((u8 *) arg0) + 0x14)) = GetTPage(0, 1, 0x280, 0);
  var_s2 = arg0 + 0xC090;
  var_s3 = 0;
  ;
  do
  {
    SetPolyFT4(var_s2);
    *((u8 *) (((u8 *) (var_s2 + 0xE)) - 0xA)) = 0x80;
    *((u8 *) (((u8 *) (var_s2 + 0xE)) - 0x9)) = 0x80;
    *((u8 *) (((u8 *) (var_s2 + 0xE)) - 0x8)) = 0x80;
    SetSemiTrans(var_s2, 0);
    *(((u8 *) (var_s2 + 0xE)) - 0x2) = 0;
    *(((u8 *) (var_s2 + 0xE)) - 0x1) = 0xE0;
    (var_s2 + 0xE)[6] = 8;
    lit_0xE8 = 0xE8;
    (var_s2 + 0xE)[7] = 0xE0;
    (var_s2 + 0xE)[14] = 0;
    (var_s2 + 0xE)[15] = lit_0xE8;
    (var_s2 + 0xE)[22] = 8;
    (var_s2 + 0xE)[23] = lit_0xE8;
    *((u16 *) (var_s2 + 0xE + 8)) = GetTPage(0, 0, 0x380, 0);
    *((u16 *) (var_s2 + 0xE)) = GetClut(0, 0x1FF);
    var_s3 += 1;
    var_s1 += 0x28;
    var_s2 += 0x28;
  }
  while (var_s3 < 8);
}

/* func_8011D39C */
void font_hex_print(u8 *arg0, s32 arg1)
{
  s32 var_s0;
  u8 *var_s1;
  var_s1 = arg0;
  var_s0 = arg1;
  if (var_s0 > 0)
  {
    do
    {
 do { FntPrint(D_8013520C, *var_s1); } while (0);
      var_s0 -= 1;
      var_s1 += 1;
    }
    while (var_s0 > 0);
  }
}

/* func_8011D3F0 */
INCLUDE_ASM("asm/nonmatchings/main", CastlePadStep);

/* func_8011DED4 */
s16 CastleSpeedVary(s32 arg0, s32 arg1)
{
  s16 var_lo;
  s32 temp_a0;
  s32 var_s0;
  s32 var_s1;
  s32 var_v0;
  s32 var_v0_2;
  s32 var_v0_3;
  s32 var_v1;
  s32 ra_e_0;
  s32 ra_e_1;
  var_s0 = arg1;
  var_s1 = 1;
  if (var_s0 < 0)
  {
    var_s1 = -1;
    var_s0 = -var_s0;
  }
  switch (arg0 & 0x1C)
  {
    case 4:
      if (var_v0_3)
    {
      var_v0 = var_s0;
    }
    else
    {
      var_v0 = var_s0;
    }
      if (var_s0 < 0)
    {
      var_v0 = -var_v0;
    }
      var_v0_2 = var_s0 < 0x601;
      if (var_v0 >= 0x111)
    {
      ra_e_0 = var_s0 - 0x11F;
      var_s0 = ra_e_0 + (rand() & 0x3F);
      block_21:
      var_v0_2 = var_s0 < 0x601;

    }
      break;

    case 8:
      var_v0_2 = var_s0 < 0x601;
      if (var_s0 < 0x300)
    {
      var_s0 = (var_s0 + (var_v0 = 0xC1)) + (rand() & 0x7F);
      goto block_21;
    }
      break;

    case 16:
      var_lo = 0xF9;
      if (var_v0_3)
    {
      var_v1 = var_s0;
    }
    else
    {
      var_v1 = var_s0;
    }
      if (var_s0 < 0)
    {
      var_v1 = -var_v1;
    }
      if (var_v1 < 0x300)
    {
      var_s0 = (rand() & 0x7F) + 0x2C1;
      goto block_21;
    }
      var_v0_2 = var_s0 < 0x601;
      if (var_v1 < 0x600)
    {
      var_s0 = (var_s0 + var_lo) + (rand() & 0xF);
      goto block_21;
    }
      break;

    default:
      var_s0 += rand() & 0xFF;
      goto block_21;

  }

  var_v0_3 = var_s0 < 0x110;
  if ((var_s0 < 0x601) == 0)
  {
    var_s0 = 0x600 - (rand() & 0x7F);
    ;
  }
  ra_e_1 = var_v0_3 != 0;
  if ((var_s0 < 0x110) != 0)
  {
    var_s0 = (rand() & 0x3F) + 0x110;
  }
  var_v0_3 = var_s0 * var_s1;
  return var_v0_3;
}
