/* PSY-Q 4.5 libsnd, member ut_rfb.o: SsUtSetReverbFeedback. The first split
   had it in psyq_misc_p.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SsUtSetReverbFeedback */
extern SpuReverbAttr D_800EA5F0;     /* _svm_rattr: the reverb attributes libsnd hands to SpuSetReverbModeParam */
long SpuSetReverbModeParam(SpuReverbAttr *);

/* libsnd SsUtSetReverbFeedback (UT_RFB.OBJ): _svm_rattr.mask = SPU_REV_FEEDBACK (0x10),
   .feedback = feedback, then SpuSetReverbModeParam(&_svm_rattr). */
/* func_80099FE8 */
void SsUtSetReverbFeedback(s16 feedback)
{
    D_800EA5F0.mask = 0x10;
    D_800EA5F0.feedback = feedback;
    SpuSetReverbModeParam(&D_800EA5F0);
}

INCLUDE_PAD(4);
