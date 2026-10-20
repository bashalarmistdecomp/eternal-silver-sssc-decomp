/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libsnd/ut_rdep.c), psyz (decomp/src/libsnd/ut_rdep.c) and
   psy-q-decomp (src/snd/ut_rdep.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libsnd, member ut_rdep.o: SsUtSetReverbDepth. The first split
   had it in psyq_misc_n.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: SsUtSetReverbDelay */
extern SpuReverbAttr D_800EA5F0;     /* _svm_rattr: the reverb attributes libsnd hands to SpuSetReverbModeParam */
long SpuSetReverbModeParam(SpuReverbAttr *);

/* cc97_lib: SsUtSetReverbDepth */

/* libsnd SsUtSetReverbDepth (UT_RDEP.OBJ): _svm_rattr.mask = SPU_REV_DEPTHL |
   SPU_REV_DEPTHR (6), .depth = the two 0..127 depths scaled to 0..0x7FFF, then
   SpuSetReverbModeParam(&_svm_rattr). */
/* func_80099EA8 */
void SsUtSetReverbDepth(s16 ldepth, s16 rdepth)
{
    D_800EA5F0.mask = 6;
    D_800EA5F0.depth.left = (ldepth * 0x7FFF) / 127;
    D_800EA5F0.depth.right = (rdepth * 0x7FFF) / 127;
    SpuSetReverbModeParam(&D_800EA5F0);
}
INCLUDE_PAD(4);
