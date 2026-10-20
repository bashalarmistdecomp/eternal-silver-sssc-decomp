/* PSY-Q 4.5 libapi, member pad.o, part 4 of 4: _Pad1,
   _IsVSync. The first split had it in psyq_misc_ao.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
extern volatile s32 *D_800B8448;   /* PAD.OBJ .data+8 = 0x1F801070: [0] I_STAT, [1] I_MASK */
/* cc97_lib: _Pad1 */
/* far_syms: D_800B8444 (target reaches it %hi/%lo) */
extern volatile u16 *D_800B8444;   /* PAD.OBJ .data+4 = 0x1F801040, the controller port: [5] is +0xA, its control register */

static inline void pad_wait(volatile s32 n)
{
    while (--n != -1) {
    }
}
/* libapi _Pad1 (PAD.OBJ, static): the pad interrupt entry's handler, clears the controller
   port's control register and waits ten turns of the loop */
/* func_800A140C */
s32 _Pad1(void)
{
    D_800B8444[5] = 0;
    pad_wait(10);
    return 0;
}

/* cc97_lib: _IsVSync */
/* far_syms: D_800B8448 (target reaches it %hi/%lo) */

/* libapi _IsVSync (PAD.OBJ, static): the pad interrupt entry's verifier, nonzero when the
   vertical-blank interrupt (bit 0) is both enabled in I_MASK and pending in I_STAT */
/* func_800A1474 */
s32 _IsVSync(void)
{
    if ((D_800B8448[1] & 1) == 0 || (D_800B8448[0] & 1) == 0) {
        return 0;
    }
    return 1;
}

INCLUDE_PAD(4);
