/* PSY-Q 4.5 libgpu, member e04.o: SetDefDispEnv. The first split had it in
   main_x.c.
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

/* func_80124C60 */
DISPENV *SetDefDispEnv(DISPENV *arg0, int arg1, int arg2, int arg3, int arg4) {
    (*(s16 *)((u8 *)arg0 + 0x0)) = (s16) arg1;
    (*(s16 *)((u8 *)arg0 + 0x2)) = (s16) arg2;
    (*(s16 *)((u8 *)arg0 + 0x4)) = (s16) arg3;
    (*(s16 *)((u8 *)arg0 + 0x8)) = 0;
    (*(s16 *)((u8 *)arg0 + 0xa)) = 0;
    (*(s16 *)((u8 *)arg0 + 0xc)) = 0;
    (*(s16 *)((u8 *)arg0 + 0xe)) = 0;
    (*(u8 *)((u8 *)arg0 + 0x11)) = 0;
    (*(s8 *)((u8 *)arg0 + 0x10)) = 0;
    (*(u8 *)((u8 *)arg0 + 0x13)) = 0;
    (*(s8 *)((u8 *)arg0 + 0x12)) = 0;
    (*(s16 *)((u8 *)arg0 + 0x6)) = (s16) arg4;
    return arg0;
}

INCLUDE_PAD(4);
