/* PSY-Q 4.5 libsnd, member ut_rdel.o: SsUtSetReverbDelay. The first split
   had it in psyq_misc_n.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SsUtSetReverbDelay */
extern SpuReverbAttr D_800EA5F0;     /* _svm_rattr: the reverb attributes libsnd hands to SpuSetReverbModeParam */
long SpuSetReverbModeParam(SpuReverbAttr *);

/* libsnd SsUtSetReverbDelay (UT_RDEL.OBJ): _svm_rattr.mask = SPU_REV_DELAYTIME (8),
   .delay = delay, then SpuSetReverbModeParam(&_svm_rattr). */
/* func_80099E68 */
void SsUtSetReverbDelay(s16 delay)
{
    D_800EA5F0.mask = 8;
    D_800EA5F0.delay = delay;
    SpuSetReverbModeParam(&D_800EA5F0);
}

INCLUDE_PAD(4);
