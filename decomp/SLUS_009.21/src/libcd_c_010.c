/* PSY-Q 4.5 libcd, member c_010.o: StSetMask. The first split had it in
   main_ae.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern s32 arg2;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

extern s32 D_801BE4D4;
extern s32 D_801BE4D0;
extern s32 D_801BE4A8;

/* StSetMask (libstr) — PSY-Q EXACT, written from libobj's reference asm.
   The last store belongs in the `jr $ra` delay slot: config/return_slot_fill.txt */
/* func_801286F0 */
void StSetMask(u_long arg0, u_long arg1, u_long arg2) {
    D_801BE4D4 = arg0;
    D_801BE4D0 = arg1;
    D_801BE4A8 = arg2;
}
