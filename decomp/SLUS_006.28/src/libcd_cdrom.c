/* PSY-Q 4.5 libcd, member cdrom.o: StSetRing. The first split had it in
   psyq_cd_m.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* far_syms: D_800E7100 (target reaches it %hi/%lo) */
/* far_syms: D_800E7104 (target reaches it %hi/%lo) */
extern u_long *D_800E7100;          /* StRingAddr */
extern u_long D_800E7104;           /* StRingSize */

/* libcd StSetRing (cdrom.o in the 4.6 / 4.7 libraries: the same eleven instructions,
   relocations StRingAddr, StRingSize, StClearRing): record the streaming ring buffer's
   address and its size in sectors, then clear the ring. Two separate globals: the
   target loads each address on its own. */
/* func_80090078 */
void StSetRing(u_long *ring_addr, u_long ring_size) {
    D_800E7100 = ring_addr;
    D_800E7104 = ring_size;
    StClearRing();
}

INCLUDE_PAD(4);
