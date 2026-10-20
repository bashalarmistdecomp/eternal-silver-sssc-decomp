/* PSY-Q 4.5 libcd, member cdrom.o: StSetRing. The first split had it in
   main_y.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;
extern s32 arg1;
extern void (*D_801336C8_s)();

/* redistributed from the original TU (split decl repair) */

extern s32 *D_801BE4BC;
extern s32 D_801BE4C8;

/* StSetRing (libstr) — PSY-Q EXACT, written from libobj's reference asm. */
/* func_80124FE0 */
void StSetRing(u_long *arg0, u_long arg1) {
    D_801BE4BC = arg0;
    D_801BE4C8 = arg1;
    StClearRing();
}
