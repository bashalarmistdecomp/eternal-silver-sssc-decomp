/* PSY-Q 4.5 libgpu, member sys.o, part 1 of 7: ResetGraph, SetGraphDebug,
   SetGraphQueue, GetGraphDebug, DrawSyncCallback, SetDispMask, DrawSync,
   func_801217A4, ClearImage, ClearImage2, LoadImage, StoreImage, MoveImage,
   ClearOTag, ClearOTagR, DrawPrim, DrawOTag, PutDrawEnv, DrawOTagEnv,
   GetDrawEnv, PutDispEnv, GetDispEnv, GetODE, SetDrawArea, SetDrawOffset,
   SetDrawEnv, func_80122780, func_801229F0, func_80122A10, func_80122AA8,
   func_80122B40, func_80122B5C, func_80122BDC, func_80122BF4,
   func_80122CD4, func_80122F04, func_80123140, func_801233C0,
   func_801233E4, func_801233F8, func_80123438, _param. The first
   split had it in main_n.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;
extern u8 *base;
extern unsigned short new_var;

/* redistributed from the original TU (split decl repair) */
s32 ResetGraph(s32);
s32 SetGraphDebug(s32);
s32 DrawSync(s32 arg0);
s32 func_80122B40(s16 arg0, s16 arg1);  /* TU defines this below */

/* alias pair: the scalar store shape of D_801336C8 (the array form at
   its other declaration is load-bearing for the reader below) */
extern void (*D_801336C8_s)();

/* func_801213C0 */
INCLUDE_ASM("asm/nonmatchings/main", ResetGraph);

/* func_80121534 */
INCLUDE_ASM("asm/nonmatchings/main", SetGraphDebug);

/* func_80121590 */
INCLUDE_ASM("asm/nonmatchings/main", SetGraphQueue);

extern u8 D_801336CE[];

/* func_80121634 */
s32 GetGraphDebug(void) {
    return D_801336CE[0];
}

/* func_80121644 */
INCLUDE_ASM("asm/nonmatchings/main", DrawSyncCallback);

/* func_801216A4 */
INCLUDE_ASM("asm/nonmatchings/main", SetDispMask);

extern void (*D_801336C8[])(u8 *, s32);
extern u8 *D_801336C4;
extern u8 D_8010D71C[];

/* func_8012173C */
s32 DrawSync(s32 arg0) {
    if (D_801336CE[0] >= 2) {
        D_801336C8[0](D_8010D71C, arg0);
    }
    return (*(s32 (**)(s32))(D_801336C4 + 0x3C))(arg0);
}

INCLUDE_ASM("asm/nonmatchings/main", func_801217A4);

extern u8 D_8010D754[];
void func_801217A4(u8 *, s32);

/* func_801218C0 */
int ClearImage(RECT *arg0, u_char arg1, u_char arg2, u_char arg3) {
    u8 *base;

    func_801217A4(D_8010D754, arg0);
    base = D_801336C4;
    (*(void (**)(s32, s32, s32, s32))(base + 8))(
        *(s32 *)(base + 0xC), arg0, 8,
        (arg3 << 16) | (arg2 << 8) | arg1);
}

extern u8 D_8010D760[];

/* func_80121950 */
int ClearImage2(RECT *arg0, u_char arg1, u_char arg2, u_char arg3) {
    u8 *base;

    func_801217A4(D_8010D760, arg0);
    base = D_801336C4;
    (*(void (**)(s32, s32, s32, u32))(base + 8))(
        *(s32 *)(base + 0xC), arg0, 8,
        ((arg3 << 16) | 0x80000000) | (arg2 << 8) | arg1);
}

extern u8 D_8010D76C[];

/* func_801219E8 */
int LoadImage(RECT *arg0, u_long *arg1) {
    u8 *base;

    func_801217A4(D_8010D76C, arg0);
    base = D_801336C4;
    (*(void (**)(s32, s32, s32, s32))(base + 8))(
        *(s32 *)(base + 0x20), arg0, 8, arg1);
}

extern u8 D_8010D778[];

/* func_80121A48 */
int StoreImage(RECT *arg0, u_long *arg1) {
    u8 *base;

    func_801217A4(D_8010D778, arg0);
    base = D_801336C4;
    (*(void (**)(s32, s32, s32, s32))(base + 8))(
        *(s32 *)(base + 0x1C), arg0, 8, arg1);
}

/* func_80121AA8 */
INCLUDE_ASM("asm/nonmatchings/main", MoveImage);

/* func_80121B60 */
INCLUDE_ASM("asm/nonmatchings/main", ClearOTag);

/* func_80121C28 */
INCLUDE_ASM("asm/nonmatchings/main", ClearOTagR);

/* func_80121CD4 */
void DrawPrim(void *arg0) {
    s32 v;

    v = ((u8 *) arg0)[3];
    (*(void (**)(s32))(D_801336C4 + 0x3C))(0);
    (*(void (**)(u8 *, s32))(D_801336C4 + 0x14))((u8 *) arg0 + 4, v);
}

extern u8 D_8010D7C0[];
/* zero_remat: DrawOTag */

/* func_80121D30 */
void DrawOTag(u_long *arg0) {
    u8 *base;
    if (D_801336CE[0] >= 2) {
        D_801336C8[0](D_8010D7C0, arg0);
    }
    base = D_801336C4;
    (*(void (**)(s32, s32, s32, s32))(base + 8))(
        *(s32 *)(base + 0x18), arg0, 0, 0);
}

/* func_80121DA0 */
INCLUDE_ASM("asm/nonmatchings/main", PutDrawEnv);

/* func_80121E60 */
INCLUDE_ASM("asm/nonmatchings/main", DrawOTagEnv);

extern u8 D_801336DC[];

u8 *memcpy(u8 *arg0, u8 *arg1, s32 arg2);  /* TU defines this below */

/* func_80121F38 */
DRAWENV *GetDrawEnv(DRAWENV *arg0) {
    memcpy(arg0, D_801336DC, 0x5C);
    return arg0;
}

/* func_80121F6C */
INCLUDE_ASM("asm/nonmatchings/main", PutDispEnv);

extern u8 D_80133738[];

/* func_80122464 */
DISPENV *GetDispEnv(DISPENV *arg0) {
    memcpy(arg0, D_80133738, 0x14);
    return arg0;
}

/* func_80122498 */
s32 GetODE(void) {
    return (u32)(*(s32 (**)(void))(D_801336C4 + 0x38))() >> 31;
}

s32 func_80122A10(s16, s16);                        /* extern */
s32 func_80122AA8(s16, s16);                        /* extern */

/* func_801224C8 */
void SetDrawArea(DR_AREA *arg0, RECT *arg1) {
    (*(u8 *)((u8 *)arg0 + 0x3)) = 2;
    (*(s32 *)((u8 *)arg0 + 0x4)) = func_80122A10((*(s32 *)((u8 *)arg1 + 0x0)), (*(s16 *)((u8 *)arg1 + 0x2)));
    (*(s32 *)((u8 *)arg0 + 0x8)) = func_80122AA8((s16) (((u16) (*(s32 *)((u8 *)arg1 + 0x0)) + (*(s32 *)((u8 *)arg1 + 0x4))) - 1), (s16) (((u16) (*(s16 *)((u8 *)arg1 + 0x2)) + (*(s16 *)((u8 *)arg1 + 0x6))) - 1));
}

s32 func_80122B40(s16, s16);                        /* extern */

/* func_80122548 */
void SetDrawOffset(DR_OFFSET *arg0, u_short *arg1) {
    (*(u8 *)((u8 *)arg0 + 0x3)) = 2;
    (*(s32 *)((u8 *)arg0 + 0x4)) = func_80122B40((*(s32 *)((u8 *)arg1 + 0x0)), (*(s16 *)((u8 *)arg1 + 0x2)));
    (*(s32 *)((u8 *)arg0 + 0x8)) = 0;
}

/* func_80122588 */
INCLUDE_ASM("asm/nonmatchings/main", SetDrawEnv);

INCLUDE_ASM("asm/nonmatchings/main", func_80122780);

INCLUDE_ASM("asm/nonmatchings/main", func_801229F0);

INCLUDE_ASM("asm/nonmatchings/main", func_80122A10);

INCLUDE_ASM("asm/nonmatchings/main", func_80122AA8);

s32 func_80122B40(s16 arg0, s16 arg1)
{
  unsigned int new_var;
  s32 ra_e_0;
  new_var = ((arg1 & 0x7FF) << 1) << 10;
  ra_e_0 = (arg0 & 0x7FF) | 0xE5000000;
  return new_var | ra_e_0;
}

INCLUDE_ASM("asm/nonmatchings/main", func_80122B5C);

INCLUDE_ASM("asm/nonmatchings/main", func_80122BDC);

INCLUDE_ASM("asm/nonmatchings/main", func_80122BF4);

INCLUDE_ASM("asm/nonmatchings/main", func_80122CD4);

INCLUDE_ASM("asm/nonmatchings/main", func_80122F04);

INCLUDE_ASM("asm/nonmatchings/main", func_80123140);

INCLUDE_ASM("asm/nonmatchings/main", func_801233C0);

INCLUDE_ASM("asm/nonmatchings/main", func_801233E4);

INCLUDE_ASM("asm/nonmatchings/main", func_801233F8);

INCLUDE_ASM("asm/nonmatchings/main", func_80123438);

/* func_80123480 */
INCLUDE_ASM("asm/nonmatchings/main", _param);
