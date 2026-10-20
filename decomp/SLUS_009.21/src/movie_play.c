/* Lunar SSSC — movie_play: the movie player: the stream played, the decode
   set up, the stream started, the slices loaded, the frames decoded and
   waited for, the read started, and the picture shown at the end. The first
   split had it in main_b.c. C, with 1 routine still read as byte ranges
   (INCLUDE_ASM).
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern u8 D_801386F0[];
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;
extern s32 arg3;
s32 ResetGraph(s32);
s32 SetGraphDebug(s32);
s32 DrawSync(s32 arg0);
extern unsigned short new_var;
extern int new_var2;
extern u8 *new_var3;
extern int new_var4;
extern SpuVoiceAttr sp10;
extern RECT sp18;
extern u8 *var_a1;
extern u8 *var_s1;
extern unsigned int var_v1;
extern void (*D_801336C8_s)();

/* func_8011E390 */
INCLUDE_ASM("asm/nonmatchings/main", movie_play);

extern u8 D_801486F0[];
extern u8 D_801706F0[];
extern u8 D_801986F0[];
extern u8 D_8019FEF0[];

extern s32 D_8013521C;

/* func_8011E8F0 */
void movie_decode_init(void *arg0, u16 arg1, s16 arg2, s16 arg3, s32 arg4, void *arg5) {
    s16 var_v1;

    if (D_8013521C == 1) {
        (*(s32 *)((u8 *)arg0 + 0x0)) = D_801486F0;
        (*(s32 *)((u8 *)arg0 + 0x4)) = D_801706F0;
        (*(s32 *)((u8 *)arg0 + 0xc)) = D_801986F0;
        (*(s32 *)((u8 *)arg0 + 0x8)) = 0;
        (*(s32 *)((u8 *)arg0 + 0x10)) = D_8019FEF0;
        (*(s32 *)((u8 *)arg0 + 0x14)) = 0;
        (*(s32 *)((u8 *)arg0 + 0x28)) = 0;
        (*(s32 *)((u8 *)arg0 + 0x34)) = 0;
        D_8013521C = 0;
    }
    (*(s16 *)((u8 *)arg0 + 0x18)) = arg1;
    (*(s16 *)((u8 *)arg0 + 0x1a)) = arg2;
    (*(s16 *)((u8 *)arg0 + 0x20)) = arg3;
    (*(s16 *)((u8 *)arg0 + 0x22)) = (s16) arg4;
    var_v1 = 0x10;
    if ((*(s32 *)((u8 *)arg5 + 0x4)) != 0) {
        var_v1 = 0x18;
    }
    (*(s16 *)((u8 *)arg0 + 0x30)) = var_v1;
    (*(s32 *)((u8 *)arg0 + 0x38)) = (s32) (*(s32 *)((u8 *)arg5 + 0x4));
    if ((*(s32 *)((u8 *)arg0 + 0x28)) == 0) {
        (*(s16 *)((u8 *)arg0 + 0x2c)) = arg1;
        (*(s16 *)((u8 *)arg0 + 0x2e)) = arg2;
        return;
    }
    (*(s16 *)((u8 *)arg0 + 0x2c)) = arg3;
    (*(s16 *)((u8 *)arg0 + 0x2e)) = (s16) arg4;
}

s32 VSync(s32);
void movie_read_start(s32);                               /* extern */
void DecDCTReset(s32);                                 /* extern */
s32 DecDCToutCallback(s32);                                 /* extern */

/* func_8011E99C */
void movie_stream_start(s32 arg0, s32 arg1, void *arg2) {
    DecDCTReset(0);
    DecDCToutCallback(arg1);
    StSetRing(D_801386F0, 0x20);
    StSetStream((*(s32 *)((u8 *)arg2 + 0x4)), (*(s32 *)((u8 *)arg2 + 0x8)), -1, 0, 0);
    movie_read_start(arg0);
}

extern s32 D_80135290;
extern s32 D_801386E8;
extern s32 D_801BE4CC;
typedef struct {
    u8 pad0[0xC];
    s32 bufs[2];
    s32 f14;
    s16 recs[2][4];
    s32 f28;
    RECT rect;
    s32 f34;
} EA14View;
extern EA14View D_801386B0_v;

/* func_8011EA14 */
void movie_slice_load(void)
{
  RECT sp10;
  u8 pad18[8];
  s32 temp_hi;
  s32 temp_s0;
  u32 temp_v0_2;
  if (D_801386E8 != 0)
  {
    if (D_801BE4CC != 0)
    {
      StCdInterrupt();
      D_801BE4CC = 0;
    }
  }
  temp_s0 = D_801386B0_v.f14;
  sp10 = D_801386B0_v.rect;
  D_801386B0_v.f14 = D_801386B0_v.f14 == 0;
  if (D_80135290 != 0)
  {
    temp_hi = D_801386B0_v.recs[D_801386B0_v.f28][2] % ((s16) D_801386B0_v.rect.w);
    if (temp_hi != 0)
    {
      D_80135290 = 0;
      D_801386B0_v.rect.x = D_801386B0_v.rect.x + temp_hi;
    }
    else
    {
      goto block_8;
    }
  }
  else
  {
    block_8:
    D_801386B0_v.rect.x = D_801386B0_v.rect.x + ((u16) D_801386B0_v.rect.w);

  }
  if (((s16) D_801386B0_v.rect.x) < (D_801386B0_v.recs[D_801386B0_v.f28][0] + D_801386B0_v.recs[D_801386B0_v.f28][2]))
  {
    DecDCTout(D_801386B0_v.bufs[D_801386B0_v.f14], ((((s32) D_801386B0_v.rect.w) << 4) * (((((s32) D_801386B0_v.rect.h) - 1) / 16) + 1)) >> 1);
  }
  else
  {
    D_801386B0_v.f34 = 1;
    temp_v0_2 = ((u32) D_801386B0_v.f28) < 1U;
    D_801386B0_v.f28 = temp_v0_2;
    D_801386B0_v.rect.x = (u16) D_801386B0_v.recs[temp_v0_2][0];
    D_80135290 = 1;
    D_801386B0_v.rect.y = (u16) D_801386B0_v.recs[temp_v0_2][1];
  }
  LoadImage((s32 *) (&sp10), D_801386B0_v.bufs[temp_s0]);
}

extern u8 D_801A76F0[];

s32 movie_frame_next(void *arg0, void *arg1);  /* TU defines this below */
s32 DecDCTvlc2(s32, s32, s32 *);                     /* extern */

/* func_8011EBE4 */
s32 movie_frame_decode(void *arg0, s32 arg1)
{
  s32 temp_v0;
  s32 temp_v0_2;
  s32 var_s1;
  var_s1 = 0x7D0;
  loop_1:
  temp_v0 = movie_frame_next(arg0, arg1);

 do { if (temp_v0 == 0) { var_s1 -= 1; if (var_s1 != 0) { goto loop_1; } return -1; } temp_v0_2 = (*((s32 *) (((u8 *) arg0) + 0x8))) == 0; *((s32 *) (((u8 *) arg0) + 0x8)) = temp_v0_2; } while (0);
  DecDCTvlc2(temp_v0, *((s32 *) (((char) (temp_v0_2 * 4)) + ((u8 *) arg0))), D_801A76F0);
  StFreeRing(temp_v0);
  return;
}

extern s32 D_80135220;
extern s32 D_80135224;
extern s32 D_80135294;

/* func_8011EC80 */
s32 movie_frame_next(void *arg0, void *arg1)
{
  void *sp1C;
  volatile unsigned short sp10;
  volatile s16 sp12;
  volatile unsigned short sp14;
  volatile unsigned short sp16;
  s32 sp18;
  void *new_var3;
  s32 var_a1;
  s16 *new_var;
  s32 var_s0;
  s32 new_var4;
  u32 temp_v1;
  s32 new_var2;
  u32 temp_v1_2;
  u32 var_a0;
  u32 var_v1;
  var_s0 = 0x7D0;
  loop_1:
  if (StGetNext(&sp18, &sp1C) != 0)
  {
    var_s0 -= 1;
    if (var_s0 != 0)
    {
      goto loop_1;
    }
    return 0;
  }

  if (((u32) (*((s32 *) (((u8 *) sp1C) + 0x8)))) >= ((u32) (*((s32 *) (((u8 *) arg1) + 0xc)))))
  {
    do
    {
    }
    while (0);
    D_80135294 = 1;
  }
  if ((D_80135220 != (*((u16 *) (((u8 *) sp1C) + 0x10)))) || (D_80135224 != (*((u16 *) (((u8 *) sp1C) + 0x12)))))
  {
    do
    {
      sp10 = 0;
      sp12 = 0;
      if ((*((s32 *) (arg1 + 0x4))) != 0)
      {
        temp_v1 = (*((s32 *) (((u8 *) arg1) + 0x18))) * 3;
        var_v1 = ((u32) (temp_v1 + (temp_v1 >> 0x1F))) >> 1;
      }
      else
      {
        var_v1 = (u32) ((u16) (*((s32 *) (((u8 *) arg1) + 0x18))));
      }
      sp14 = (u16) var_v1;
      sp16 = (*((s32 *) (((u8 *) arg1) + 0x1c))) * 2;
      if ((*((s32 *) (((u8 *) arg1) + 0x4))) != 0)
      {
        var_a1 = 0;
        ClearImage(&sp10, var_a1, var_a1, var_a1);
      }
      else
      {
        var_a1 = 0x40;
        ClearImage(&sp10, var_a1, var_a1, var_a1);
      }
      new_var4 = (s32) (*((u16 *) (((u8 *) sp1C) + 0x12)));
      D_80135220 = (s32) (*((u16 *) (((u8 *) sp1C) + 0x10)));
      D_80135224 = new_var4;
    }
    while (0);
  }
  if ((*((s32 *) (((u8 *) arg1) + 0x4))) != 0)
  {
    temp_v1_2 = D_80135220 * 3;
    var_a0 = ((u32) (temp_v1_2 + (temp_v1_2 >> 0x1F))) >> 1;
  }
  else
  {
    var_a0 = (u32) ((u16) D_80135220);
  }
  new_var3 = arg0;
  new_var2 = sp18;
  *((s16 *) (((u8 *) arg0) + 0x26)) = (u16) D_80135224;
  new_var = (s16 *) (((u8 *) arg0) + 0x32);
  *((s16 *) (((u8 *) new_var3) + 0x24)) = var_a0;
  *((s16 *) (((u8 *) new_var3) + 0x1c)) = var_a0;
  *((s16 *) (((u8 *) new_var3) + 0x1e)) = (u16) D_80135224;
  *new_var = (u16) D_80135224;
  return new_var2;
}

void DecDCTout(s32, s32);  /* extern */
extern u8 D_8010D4B8[];
void printf();  /* extern */

/* func_8011EE08 */
void movie_decode_wait(void *arg0)
{
  volatile unsigned long sp10;
  unsigned int temp_v0;
  s32 temp_v0_2;
  s32 ra_e_0;
  s32 new_var2;
  u8 *new_var;
  s32 ra_e_1;
  new_var2 = *((s32 *) (((u8 *) arg0) + 0x34));
  if (temp_v0_2)
  {
    sp10 = 0x800000;
  }
  else
  {
    sp10 = 0x800000;
  }
  if (new_var2 == 0)
  {
    do
    {
      temp_v0 = (sp10 = sp10 - 1);
      if (temp_v0 == 0)
      {
        printf(D_8010D4B8);
        *((s32 *) (((u8 *) arg0) + 0x34)) = 1;
        if (1)
        {
          temp_v0_2 = (*((s32 *) (((u8 *) arg0) + 0x28))) == 0;
          *((s32 *) (((u8 *) arg0) + 0x28)) = temp_v0_2;
          ra_e_0 = ((u8 *) (arg0 + (temp_v0_2 * 8))) + 0x18;
        }
        *((s16 *) (((u8 *) arg0) + 0x2c)) = (u16) (*((s32 *) ra_e_0));
        ra_e_1 = ((u8 *) (arg0 + ((*((s32 *) (((u8 *) arg0) + 0x28))) * 8))) + 0x1a;
        *((s16 *) (new_var = ((u8 *) arg0) + 0x2e)) = (u16) (*((s16 *) ra_e_1));
      }
    }
    while ((*((s32 *) (((u8 *) arg0) + 0x34))) == 0);
  }
  *((s32 *) (((u8 *) arg0) + 0x34)) = 0;
}

/* func_8011EEC8 */
void movie_read_start(s32 arg0) {
    u8 sp10;

    sp10 = 0x80;
    do {
loop_2:
        if (CdControl(2, arg0, 0) == 0) {
            goto loop_2;
        }
loop_4:
        if (CdControl(0xE, &sp10, 0) == 0) {
            goto loop_4;
        }
        VSync(3);
    } while (CdRead2(0x1E0) == 0);
}

extern u8 D_80010114[];
extern u8 D_80135390[];
s32 pad_buffer_decode(s32 *, s32);
extern s32 D_801351D4;

/* func_8011EF38 */
void movie_picture_show(void)
{
  s16 sp18[0x14];
  u8 sp40[0x14];
  s32 var_s1;
  var_s1 = 0;
  SetDispMask(var_s1);
  ResetGraph(var_s1);
  SetGraphDebug(var_s1);
  sp18[0] = 0;
  sp18[1] = 0;
  sp18[2] = 0x100;
  sp18[3] = 0x200;
  ClearImage(sp18, var_s1, var_s1, var_s1);
  VSync(var_s1);
  sp18[0] = 0;
  sp18[1] = 0;
  sp18[2] = 0x100;
  sp18[3] = 0x100;
  LoadImage(sp18, D_80010114);
  DrawSync(var_s1);
  VSync(var_s1);
  SetDefDispEnv(sp40, var_s1, var_s1, 0x100, 0xF0);
  SetDispMask(1);
  do
  {
    loop_1:
    D_801351D4 = 0;

    pad_buffer_decode(D_80135390, 0);
    pad_buffer_decode(D_80135390 + 0x22, 4);
    PutDispEnv(sp40);
    VSync(0);
    var_s1 += 1;
    if (!(D_801351D4 & 0x40))
    {
      if (var_s1 < 0xE11)
      {
        goto loop_1;
      }
    }
  }
  while (0);
}
