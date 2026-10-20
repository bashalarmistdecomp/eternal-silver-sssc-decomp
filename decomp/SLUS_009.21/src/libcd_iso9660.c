/* PSY-Q 4.5 libcd, member iso9660.o: CdSearchFile, _cmp,
   CD_newmedia, func_801274AC, CD_cachefile, cd_read. The first
   split had it in main_ac.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* func_80126EF0 */
INCLUDE_ASM("asm/nonmatchings/main", CdSearchFile);

/* func_801271C8 */
INCLUDE_ASM("asm/nonmatchings/main", _cmp);

/* func_801271E8 */
INCLUDE_ASM("asm/nonmatchings/main", CD_newmedia);

INCLUDE_ASM("asm/nonmatchings/main", func_801274AC);

/* func_80127550 */
INCLUDE_ASM("asm/nonmatchings/main", CD_cachefile);

/* func_801277EC */
INCLUDE_ASM("asm/nonmatchings/main", cd_read);
