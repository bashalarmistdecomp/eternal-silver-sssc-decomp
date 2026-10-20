/* Lunar SSSC — sound: the disc's sound: its set-up, the voices played, set,
   stopped and stepped, and the noise. The first split had it in main.c.
   Every function is C.
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
extern int addr;
extern s32 mask;
extern void (*D_801336C8_s)();

extern s32 D_801351E8;
extern s32 D_801351EC;

/* Measured arities — outgoing arg-slot stores (sw 0x10..0x1C($sp)) at
   every call site in the two giants.
   The variadic two are per-site variant (6/7 and 4/6 args at different
   sites): unprototyped originals, so `...` is the honest model. */
void sound_noise_start(long n_clock);
void sound_noise_clock_set(long n_clock);

void sound_voice_stop(s32 arg0);  /* TU defines this below */
void sound_noise_start(long n_clock);
void sound_noise_clock_set(long n_clock);
void sound_noise_stop(void);  /* TU defines this below */
void SpuInit(void);  /* extern */
extern u8 D_8010BDF0[];
extern u8 D_8010C820[];
extern u8 D_80135358[];
extern s32 D_8013525C;
extern s32 D_80135264;

/* func_8010E350 */
void sound_init(void)
{
  SpuVoiceAttr sp10;
  SpuCommonAttr sp50;
  s32 temp_v0;
  s32 temp_v0_2;
  SpuInit();
  SpuInitMalloc(2, D_80135358);
  sp50.mask = 3;
  sp50.mvol.left = 0x3FFF;
  sp50.mvol.right = 0x3FFF;
  SpuSetCommonAttr((s32 *) &sp50);
  SpuSetTransferMode(0);
  temp_v0 = SpuMalloc(0xA30);
  D_8013525C = temp_v0;
  SpuSetTransferStartAddr(temp_v0);
  SpuWrite(D_8010C820, 0xA30);
  SpuIsTransferCompleted(1);
  temp_v0_2 = SpuMalloc(0x22BC);
  D_80135264 = temp_v0_2;
  SpuSetTransferStartAddr(temp_v0_2);
  SpuWrite(D_8010BDF0, 0x22BC);
  SpuIsTransferCompleted(1);
  sp10.mask = 0xFF93;
  sp10.volume.left = 0x1FFF;
  sp10.volume.right = 0x1FFF;
  sp10.pitch = 0x1000;
  sp10.voice = 1;
  sp10.a_mode = 1;
  sp10.s_mode = 1;
  sp10.r_mode = 3;
  sp10.ar = 0;
  sp10.dr = 0;
  sp10.sr = 0;
  sp10.rr = 0;
  sp10.sl = 0xF;
  sp10.addr = D_80135264;
  SpuSetVoiceAttr(&sp10);
  SpuSetNoiseVoice(1, 1);
  sp10.mask = 0xFFE3;
  sp10.voice = 0x1FE;
  sp10.note = 0x6000;
  sp10.volume.left = 0x3FFF;
  sp10.volume.right = 0x3FFF;
  sp10.a_mode = 1;
  sp10.s_mode = 1;
  sp10.r_mode = 3;
  sp10.ar = 0;
  sp10.dr = 0;
  sp10.sr = 0;
  sp10.rr = 0;
  sp10.sl = 0xF;
  sp10.sample_note = 0x2800;
  sp10.addr = D_80135264;
  SpuSetVoiceAttr(&sp10);
}

extern s32 D_80135370[];  /* TU declares this below */

/* func_8010E4D0 */
void sound_voice_play(s32 arg0, s16 arg1, s32 arg2, s32 arg3)
{
  u8 sp10[0x40];
  s32 v0c;
  s32 va3;
  s32 mask;
  v0c = arg0;
  va3 = arg3;
  mask = 1 << (v0c + 1);
  *((u32 *) sp10) = mask;
  *((u32 *) (sp10 + 0x4)) = 0xA0;
  *((s16 *) (sp10 + 0x16)) = arg1;
  *((s32 *) (sp10 + 0x1C)) = arg2;
  SpuSetKeyOnWithAttr(sp10);
  *((u32 *) (((u8 *) D_80135370) + (v0c * 4))) = va3;
}

/* func_8010E538 */
void sound_voice1_set(s16 arg0, s32 arg1) {
    SpuVoiceAttr sp10;

    sp10.voice = 2;
    sp10.pitch = arg0;
    sp10.mask = 0x90;
    sp10.addr = arg1;
    SpuSetVoiceAttr(&sp10);
}

/* func_8010E570 */
void sound_voice_stop(s32 arg0) {
    SpuSetKey(0, 1 << (arg0 + 1));
}

extern s32 D_80135370[];

/* func_8010E59C */
void sound_voices_step(void) {
    s32 *p;
    s32 i;
    s32 v;

    i = 0;
    p = D_80135370;
    do {
        v = *p;
        if (v == 0) {
            sound_voice_stop(i);
        } else {
            *p = v - 1;
        }
        p++;
        i++;
    } while (i < 8);
}

/* The noise clock is the caller's argument, passed through. */
/* func_8010E604 */
void sound_noise_start(long n_clock) {
    SpuSetNoiseClock(n_clock);
    SpuSetKey(1, 1);
}

/* func_8010E630 */
void sound_noise_clock_set(long n_clock) {
    SpuSetNoiseClock(n_clock);
}

/* func_8010E650 */
void sound_noise_stop(void) {
    D_801351EC = 0;
    D_801351E8 = 0;
    SpuSetKey(0, 1);
}

extern u8 D_8010D250[];
extern u8 D_8010D25C[];
void printf();  /* TU declares this below */

/* func_8010E67C */
void sound_noise_step(void)
{
  if (D_801351E8 != 0)
  {
    SpuSetNoiseClock(D_801351E8);
    if (D_801351EC == 0)
    {
      SpuSetKey(1, 1);
      printf(D_8010D250);
    }
    D_801351EC = D_801351E8--;
    return;
  }
  if (D_801351EC != 0)
  {
    D_801351EC = 0;
    SpuSetKey(0, 1);
    printf(D_8010D25C);
  }
}
