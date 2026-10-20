/* PSY-Q 4.5 libsnd, member vm_noff.o: vmNoiseOff. The first split had it in
   psyq_misc_s.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* libsnd's per-voice state (_svm_voice[24], 0x38 each). */
/* libsnd's current-note parameters (_svm_cur). */
/* libsnd's per-score sequence state (_ss_score[seq][sep], 0xB0 each). */
extern SvmVoice D_800E9EC0[];  
/* cc97_lib: vmNoiseOff */
/* gnu_as_loads: vmNoiseOff */
/* return_slot_fill: vmNoiseOff */

/* libsnd vmNoiseOff (VM_NOFF.OBJ): retire a voice that sounds as the noise
   voice.  Its record goes idle: key state (+0x1D) = 0, the VAG number
   (+0x00, 0xFF on a noise voice) = 0 and the pitch word (+0x04) = 0.  It
   touches no pending mask and no SPU register itself.  Its one caller is
   _SsVmKeyOff, which calls it in place of _SsVmKeyOffNow
   for a voice whose VAG number is 0xFF. */
/* func_8009BDB8 */
void vmNoiseOff(u8 voice)
{
    D_800E9EC0[voice].key_stat = 0;
    D_800E9EC0[voice].vag = 0;
    D_800E9EC0[voice].pitch = 0;
}

INCLUDE_PAD(8);
