/* PSY-Q 4.5 libgs, member gs_007.o: GsInitVcount. The first split had it in
   psyq_cd_e.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
/* Sony's libapi.h declares SetRCnt(unsigned long, unsigned short, long). libgs's
   GsInitVcount, below, passes -1 as the target and the image loads it as a whole word
   (`li a1,-1`); under that prototype it would be 0xFFFF. So Sony's own libgs was compiled
   with no such prototype in scope. The header's declaration is renamed out of the way for
   this file, and SetRCnt is declared and defined below with the types its caller shows. */
#define SetRCnt SetRCnt_sdk_prototype
#include "psyq_types.h"
#include "psyq_private.h"
#undef SetRCnt
s32 SetRCnt(s32, s16, s32);
/* cc97_lib: GsInitVcount */

/* libgs GsInitVcount (GS_007.OBJ): initialises and starts the vertical retrace counter
   GsGetVcount reads. SetRCnt on root counter 1
   (RCntCNT1 0xF2000001, the horizontal-sync counter) with target 0xFFFF and mode
   RCntMdNOINTR (0x2000), then StartRCnt on the same counter. */
/* func_8008F228 */
void GsInitVcount(void)
{
    SetRCnt(0xF2000001, -1, 0x2000);
    StartRCnt(0xF2000001);
}

INCLUDE_PAD(8);
