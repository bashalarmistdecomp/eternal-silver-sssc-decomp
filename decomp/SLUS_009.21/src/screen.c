/* Lunar SSSC — screen: the screen's set-up (display, graph, clear, the
   debug font). The first split had it in main.c. Every function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

void SetDumpFnt(s32 arg0);  /* extern */
s32 FntOpen(s32, s32, s32, s32, s32, s32);
s32 ResetGraph(s32);
s32 SetGraphDebug(s32);

/* func_8010E2B0 */
void screen_init(void)
{
  RECT sp18;
  SetDispMask(0);
  ResetGraph(0);
  SetGraphDebug(0);
  ResetGraph(0);
  SetGraphDebug(0);
  sp18.w = 0x280;
  sp18.x = 0;
  sp18.y = 0;
  sp18.h = 0x1E0;
  ClearImage((s16 *) &sp18, 0, 0, 0);
  FntLoad(0x300, 0x100);
  SetDumpFnt(FntOpen(0x40, 0x2C, 0x100, 0x100, 0, 0x200));
}
