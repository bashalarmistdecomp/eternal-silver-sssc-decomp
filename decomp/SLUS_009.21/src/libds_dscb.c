/* PSY-Q 4.5 libds, member dscb.o: DsSyncCallback, DsReadyCallback,
   DsStartCallback, DsDataCallback. The first split had it in main_ak.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern unsigned short new_var;

/* redistributed from the original TU (split decl repair) */
s32 DMACallback(s32, s32);
extern void (*D_801336C8_s)();

/* func_80129E80 */
INCLUDE_ASM("asm/nonmatchings/main", DsSyncCallback);

/* func_80129E94 */
INCLUDE_ASM("asm/nonmatchings/main", DsReadyCallback);

/* func_80129EA8 */
INCLUDE_ASM("asm/nonmatchings/main", DsStartCallback);

/* func_80129EBC */
void DsDataCallback(unsigned int arg0)
{
  int new_var2;
  int new_var;
  if (new_var2)
  {
    new_var = 3;
    new_var2 = new_var;
    DMACallback(new_var2, arg0);
  }
  else
  {
    new_var = 3;
    new_var2 = new_var;
    DMACallback(new_var2, arg0);
  }
}
