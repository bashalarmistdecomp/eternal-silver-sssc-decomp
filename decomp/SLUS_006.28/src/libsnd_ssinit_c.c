/* PSY-Q 4.5 libsnd, member ssinit_c.o: SsInit. The first split had it in
   psyq_spu_n.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
void _SsInit();                                  /* extern */
/* cc97_lib: _SsInit */

void SpuInit(void);

/* cc97_lib: SsInit */
/* libsnd SsInit (SSINIT.OBJ): ResetCallback, SpuInit,
   SpuClearReverbWorkArea(7), then _SsInit. */
/* func_80095A78 */
void SsInit(void) {
    ResetCallback();
    SpuInit();
    SpuClearReverbWorkArea(7);
    _SsInit();
}
INCLUDE_PAD(8);
