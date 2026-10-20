/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp
   (src/main/psxsdk/libspu/s_stm.c), psyz (decomp/src/libspu/s_stm.c) and
   psy-q-decomp (src/spu/s_stm.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libspu, member s_stm.o: SpuSetTransferMode. The first split had
   it in main_as.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();
extern s32 D_80134E1C;
extern s32 D_80134EA8;

/* func_8012B810 */
long SpuSetTransferMode(long mode) {
    s32 t;
    switch (mode) {
    case 0:
        t = 0;
        break;
    case 1:
        t = 1;
        break;
    default:
        t = 0;
        break;
    }
    D_80134E1C = mode;
    D_80134EA8 = t;
}
