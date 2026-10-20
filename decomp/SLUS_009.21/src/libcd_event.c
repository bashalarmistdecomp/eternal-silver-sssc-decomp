/* PSY-Q 4.5 libcd, member event.o: CdInit, func_801250A0, def_cbsync,
   func_80125104, func_8012512C. The first split had it in main_aa.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern unsigned short new_var;
extern int new_var2;

/* the .s slot is 0x30; the code is 0x2C (the rest is trailing padding) */
INCLUDE_PAD(4);
/* redistributed from the original TU (split decl repair) */
void def_cbsync(void);  /* TU defines this below */
void func_80125104(void);  /* TU defines this below */
extern void (*D_801336C8_s)();

/* func_80125010 */
INCLUDE_ASM("asm/nonmatchings/main", CdInit);

s32 CD_initvol();
s32 CD_init();

s32 func_801250A0(void)
{
  s32 new_var;
  s32 new_var2;
  new_var2 = CD_init();
  if (new_var2 == 0)
  {
    if (new_var2)
    {
      new_var = CD_initvol();
      return new_var == 0;
    }
    else
    {
      new_var = CD_initvol();
      return new_var == 0;
    }
  }
  return 0;
}

/* epilogue_load_slot: def_cbsync */
/* func_801250DC */
void def_cbsync(void) {
    do { DeliverEvent(0xF0000003, 0x20); } while (0);
}

/* epilogue_load_slot: func_80125104 */
void func_80125104(void) {
    do { DeliverEvent(0xF0000003, 0x40); } while (0);
}

void func_8012512C(void)
{
 do { } while (0);
  DeliverEvent(-0x0FFFFFFD, 0x40);
}

INCLUDE_PAD(12);
