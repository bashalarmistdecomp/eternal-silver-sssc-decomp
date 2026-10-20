/* PSY-Q 4.5 libspu, member s_dcb.o: _SpuDataCallback. The first split had
   it in main_ao.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern unsigned short new_var;

/* redistributed from the original TU (split decl repair) */
s32 DMACallback(s32, s32);
extern void (*D_801336C8_s)();
/* func_8012ABC0 */
void _SpuDataCallback(s32 arg0)
{
  int new_var;
  if (new_var)
  {
    new_var = 4;
    DMACallback(new_var, arg0);
  }
  else
  {
    new_var = 4;
    DMACallback(new_var, arg0);
  }
}

INCLUDE_PAD(12);
