/* PSY-Q 4.5 libcd, member cdread.o: func_80127860, func_80127894,
   func_80127B08, func_80127BD8, CdReadBreak, CdRead, CdReadSync,
   CdReadCallback, CdReadMode. The first split had it in main_ac.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

INCLUDE_ASM("asm/nonmatchings/main", func_80127860);

INCLUDE_ASM("asm/nonmatchings/main", func_80127894);

INCLUDE_ASM("asm/nonmatchings/main", func_80127B08);

INCLUDE_ASM("asm/nonmatchings/main", func_80127BD8);

/* func_80127DC0 */
INCLUDE_ASM("asm/nonmatchings/main", CdReadBreak);

/* func_80127F60 */
INCLUDE_ASM("asm/nonmatchings/main", CdRead);

/* func_801280FC */
INCLUDE_ASM("asm/nonmatchings/main", CdReadSync);

extern CdlCB D_801339AC;

/* func_80128200 */
CdlCB CdReadCallback(CdlCB func) {
    CdlCB old = D_801339AC;
    D_801339AC = func;
    return old;
}

/* func_80128214 */
INCLUDE_ASM("asm/nonmatchings/main", CdReadMode);
