/* PSY-Q 4.5 libpad, member padcmd.o, part 7 of 7: _padCmdParaMode,
   func_8012E2E4, func_8012E2F8, func_8012E318, func_8012E338,
   func_8012E358. The first split had it in main_aag.c. The tree compiles
   this run of the member's functions with other flags than the runs beside
   it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern unsigned short new_var;

/* redistributed from the original TU (split decl repair) */
void _padCmdParaMode(void *arg0, s8 arg1);  /* TU defines this below */
void func_8012E2E4(void *arg0);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* func_8012E2C4 */
void _padCmdParaMode(void *arg0, s8 arg1)
{
  if (1)
  {
    *((u8 *) (((u8 *) arg0) + 0x37)) = 0x43;
  }
  *((s32 *) (((u8 *) arg0) + 0x2c)) = (u32) (arg0 + 0x24);
  *(((u8 *) arg0) + 0x24) = arg1;
  *(((u8 *) arg0) + 0x36) = 1;
}

void func_8012E2E4(void *arg0)
{
  *((u8 *) (((u8 *) arg0) + 0x37)) = 0x45;
  *((s32 *) (((u8 *) arg0) + 0x2c)) = 0;
  *(((u8 *) arg0) + 0x36) = 0;
}

void func_8012E2F8(void *arg0, char arg1)
{
  *((u8 *) (((u8 *) arg0) + 0x37)) = 0x4C;
  *((s32 *) (((u8 *) arg0) + 0x2c)) = (u32) (arg0 + 0x24);
  if (1)
  {
    *(((u8 *) arg0) + 0x24) = arg1;
    *(((u8 *) arg0) + 0x36) = 1;
  }
}

void func_8012E318(void *arg0, s8 arg1)
{
  *((u8 *) (((u8 *) arg0) + 0x37)) = 0x46;
  *((s32 *) (((u8 *) arg0) + 0x2c)) = (u32) (arg0 + 0x24);
  *(((u8 *) arg0) + 0x24) = arg1;
  *(((u8 *) arg0) + 0x36) = 1;
}

void func_8012E338(void *arg0, s8 arg1)
{
  u8 *new_var;
  new_var = (s32 *) (((u8 *) arg0) + 0x24);
  *((u8 *) (((u8 *) arg0) + 0x37)) = 0x47;
  *((s32 *) (((u8 *) arg0) + 0x2c)) = (u32) (arg0 + 0x24);
  *new_var = arg1;
  new_var = ((u8 *) arg0) + 0x36;
  *new_var = 1;
}

void func_8012E358(void *arg0)
{
  *((u8 *) (((u8 *) arg0) + 0x37)) = 0x4B;
  *((s32 *) (((u8 *) arg0) + 0x2c)) = 0;
  *(((u8 *) arg0) + 0x36) = 0;
}

INCLUDE_PAD(4);
