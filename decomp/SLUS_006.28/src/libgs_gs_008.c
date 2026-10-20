/* PSY-Q 4.5 libgs, member gs_008.o: GsGetVcount. The first split had it in
   psyq_cd_f.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

extern long HWD0, VWD0;
/* cc97_lib: GsGetVcount */
/* return_slot_fill: GsGetVcount */

/* libgs GsGetVcount (GS_008.OBJ): the value of the vertical retrace counter GsInitVcount
    starts: GetRCnt on root counter 1 (RCntCNT1
   0xF2000001), read again until two consecutive readings agree. The reading is kept in a
   volatile local: the target stores it to the frame and loads it back twice, once before
   the second call and once for the return value.
   MARKED SPELLING: `prev = cnt;`, a second local that takes the first reading before the
   second call. Comparing the volatile directly (`while (cnt != GetRCnt(...))`)
   scores 12/19: the target loads the first reading into a saved register BEFORE the
   second call, and a volatile compared directly is read after it. The original construct
   is not known. */
/* func_8008F3D8 */
long GsGetVcount(void)
{
    volatile s32 cnt;
    s32 prev;

    do {
        cnt = GetRCnt(0xF2000001);
        prev = cnt;
    } while (prev != GetRCnt(0xF2000001));
    return cnt;
}
INCLUDE_PAD(4);
