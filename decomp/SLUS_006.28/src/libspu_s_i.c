/* PSY-Q 4.5 libspu, member s_i.o: SpuInit. The first split had it in
   psyq_misc_t.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

void _SpuInit(s32 bHot);

/* cc97_lib: SpuInit */
/* libspu SpuInit (S_I.OBJ): _SpuInit(0), a cold initialisation. */
/* func_8009DCD8 */
void SpuInit(void) {
    _SpuInit(0);
}
