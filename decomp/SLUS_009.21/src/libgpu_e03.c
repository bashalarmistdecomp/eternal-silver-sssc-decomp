/* PSY-Q 4.5 libgpu, member e03.o: SetDefDrawEnv. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;
extern s32 var_v0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80124BA0 */
DRAWENV *SetDefDrawEnv(DRAWENV *arg0, int arg1, int arg2, int arg3, int arg4)
{
  s32 temp_v0;
  s8 var_v0;
  temp_v0 = GetVideoMode();
  *((s16 *) (((u8 *) arg0) + 0x0)) = (s16) arg1;
  *((s16 *) (((u8 *) arg0) + 0x2)) = (s16) arg2;
  *((s16 *) (((u8 *) arg0) + 0x4)) = (s16) arg3;
  *((s16 *) (((u8 *) arg0) + 0xc)) = 0;
  *((s16 *) (((u8 *) arg0) + 0xe)) = 0;
  *((s16 *) (((u8 *) arg0) + 0x10)) = 0;
  *((s16 *) (((u8 *) arg0) + 0x12)) = 0;
  *((u8 *) (((u8 *) arg0) + 0x19)) = 0;
  *((s8 *) (((u8 *) arg0) + 0x1a)) = 0;
  *((u8 *) (((u8 *) arg0) + 0x1b)) = 0;
  *((s8 *) (((u8 *) arg0) + 0x16)) = 1;
  *((s16 *) (((u8 *) arg0) + 0x6)) = (s16) arg4;
  if (temp_v0 != 0)
  {
    var_v0 = arg4 < 0x121;
    *((u8 *) (((u8 *) arg0) + 0x17)) = var_v0;
  }
  else
  {
    var_v0 = arg4 < 0x101;
    *((u8 *) (((u8 *) arg0) + 0x17)) = var_v0;
  }
  *((s16 *) (((u8 *) arg0) + 0x8)) = (s16) arg1;
  *((s16 *) (((u8 *) arg0) + 0xa)) = (s16) arg2;
  *((s16 *) (((u8 *) arg0) + 0x14)) = 0xA;
  *((s8 *) (((u8 *) arg0) + 0x18)) = 0;
  return arg0;
}

INCLUDE_PAD(12);
