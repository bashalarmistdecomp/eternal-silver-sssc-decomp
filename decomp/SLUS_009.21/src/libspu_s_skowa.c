/* PSY-Q 4.5 libspu, member s_skowa.o: SpuSetKeyOnWithAttr. The first split
   had it in main_aq.c.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern volatile int arg0;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_8012B720 */
void SpuSetKeyOnWithAttr(SpuVoiceAttr *arg0) {
    SpuSetVoiceAttr(arg0);
    SpuSetKey(1, arg0->voice);
}
