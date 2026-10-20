/* PSY-Q 4.5 libspu, member s_sccm.o: SpuSetCommonCDMix. The first split had
   it in main_as.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

extern u8 *D_80134E8C;

/* func_8012C4A0 */
void SpuSetCommonCDMix(long on) {
    u8 *p;
    u16 v;
    if (on == 0) {
        p = D_80134E8C;
        v = (u16) (*(u16 *) (p + 0x1AA) & 0xFFFE);
    } else {
        p = D_80134E8C;
        v = (u16) (*(u16 *) (p + 0x1AA) | 1);
    }
    *(u16 *) (p + 0x1AA) = v;
}
