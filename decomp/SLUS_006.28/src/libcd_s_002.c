/* SPDX-License-Identifier: MIT
   Parts of this file match code of sotn-decomp (src/main/psxsdk/libcd/sys.c)
   and psyz (decomp/src/libcd/sys.c), all MIT-licensed: the notices are in
   LICENSES/, and NOTICE.md says more. */
/* PSY-Q 4.5 libcd, member s_002.o: CdIntToPos, CdPosToInt. The first split
   had it in psyq_spu_e.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: func_80094670 */
/* gnu_as_loads: func_80094670 */
/* return_slot_fill: func_80094670 */
/* reorder_fill: func_80094670 */

/* cc97_lib: CdIntToPos */

static inline int cd_itob(int n)
{
    return ((n / 10) << 4) + (n % 10);
}
/* libcd CdIntToPos (SYS.OBJ): absolute sector number to a BCD minute / second / sector
   position.  i += 150 (the two-second lead-in), then p->sector = BCD(i % 75),
   p->second = BCD(i / 75 % 60), p->minute = BCD(i / 75 / 60); returns p.  The BCD
   encoding ((n / 10) << 4) + n % 10 is an inline function in the source (psyz and
   sotn-decomp have it as a nested one): written as a macro the three nested divisions
   fold into one another. */
/* func_80094818 */
CdlLOC *CdIntToPos(int i, CdlLOC *p)
{
    i += 150;
    p->sector = cd_itob(i % 75);
    p->second = cd_itob(i / 75 % 60);
    p->minute = cd_itob(i / 75 / 60);
    return p;
}

/* cc97_lib: CdPosToInt */

/* libcd CdPosToInt (SYS.OBJ): a BCD minute / second / sector position to the absolute
   sector number: (minute * 60 + second) * 75 + sector - 150 (the two-second lead-in),
   CdIntToPos's inverse. Each BCD byte is decoded with libcd.h's btoi(b). */
/* func_8009491C */
int CdPosToInt(CdlLOC *p)
{
    return (btoi(p->minute) * 60 + btoi(p->second)) * 75 + btoi(p->sector) - 150;
}

INCLUDE_PAD(12);
