/* Lunar SSSC — pad: the pad's start and stop. The first split had it in
   main.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

extern u8 D_801351F2;
extern u8 D_801351F3;

/* func_8010E1E0 */
void CastleActuatorSet(s32 arg0, s32 arg1, u8 arg2, s32 arg3, u16 arg4)
{
  s32 temp_a1;
  int new_var2;
  void *temp_t0;
  unsigned short new_var;
  if (D_801351F3 != 0)
  {
    temp_a1 = arg1 & 0xFF;
    if (D_801351F2 == 0)
    {
      temp_t0 = arg0 + temp_a1;
      if (((u8) (*((s16 *) (((u8 *) temp_t0) + 0x56a)))) < ((u32) (arg2 & 0xFF)))
      {
        *(((u8 *) temp_t0) + 0x56a) = arg2;
        *((u8 *) (((u8 *) temp_t0) + 0x56c)) = arg3;
        new_var = 0x56e;
        *((s16 *) (((u8 *) (new_var2 = arg0 + (temp_a1 * 2))) + new_var)) = (s16) arg4;
      }
    }
  }
}

extern u8 D_801353D4[];

s32 ResetCallback();
s32 StopCallback();
s32 memset(s32 *, s32, s32);

/* func_8010E230 */
void pad_start(void) {
    memset(D_801353D4, 0, 0x80);
    ResetCallback();
    PadInitMtap(D_801353D4 - 0x44, D_801353D4 - 0x22);
    PadStartCom();
    ChangeClearPAD(0);
}

/* func_8010E288 */
void pad_stop(void) {
    PadStop();
    StopCallback();
}
