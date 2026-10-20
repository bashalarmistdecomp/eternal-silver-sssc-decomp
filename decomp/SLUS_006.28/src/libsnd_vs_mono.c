/* PSY-Q 4.5 libsnd, member vs_mono.o: SsSetMono, SsSetStereo. The first
   split had it in psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
extern s16 D_800EA6D0;       /* _svm_stereo_mono */
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */

/* return_slot_fill: SsSetMono */
/* func_8009D618 */
void SsSetMono(void) {
    D_800EA6D0 = 1;
}
/* cc97_lib: SsSetStereo */
/* return_slot_fill: SsSetStereo */

/* libsnd SsSetStereo (VS_MONO.OBJ): select stereo output: _svm_stereo_mono
   (D_800EA6D0) = 0.  While the flag is 1 (SsSetMono) the
   volume code gives both channels the louder of the two. */
/* func_8009D628 */
void SsSetStereo(void)
{
    D_800EA6D0 = 0;
}

INCLUDE_PAD(4);
