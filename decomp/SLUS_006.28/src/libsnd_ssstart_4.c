/* PSY-Q 4.5 libsnd, member ssstart.o, part 4 of 4: func_800983F4. The first
   split had it in psyq_misc_f.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* far_syms: D_800B7A04 (target reaches it %hi/%lo) */

extern SndSeqTickEnv D_800B79F0;    /* _snd_seq_tick_env */
extern s32 D_800B7A04;              /* ssstart.c's own variable: the half-rate tick's phase */

/* cc97_lib: func_800983F4 */
/* libsnd _SsSeqCalledTbyT_1per2 (SSSTART.OBJ; the name is sotn-decomp's): the handler
   _SsStart installs for a tick rate below 70 Hz, where the root counter runs at twice
   the rate: every second call it runs the tick function. The phase word D_800B7A04 is
   not a member of _snd_seq_tick_env: it is the word after the struct in ssstart.o's
   .data, no other function touches it, and the target loads its address at each of its
   three uses (members of one struct share a base register, as in _SsStart; written as
   a member this function scores 9 of 18). */
void func_800983F4(void) {
    if (D_800B7A04 == 0) {
        D_800B7A04 = 1;
        return;
    }
    D_800B7A04 = 0;
    D_800B79F0.tick_cb();
}
INCLUDE_PAD(12);
