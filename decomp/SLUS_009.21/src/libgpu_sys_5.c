/* PSY-Q 4.5 libgpu, member sys.o, part 5 of 7: LoadImage2, StoreImage2,
   MoveImage2, DrawOTag2. The first split had it in main_r.c. The tree
   compiles this run of the member's functions with other flags than the
   runs beside it (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* func_80123E88 */
INCLUDE_ASM("asm/nonmatchings/main", LoadImage2);

/* func_80123F74 */
INCLUDE_ASM("asm/nonmatchings/main", StoreImage2);

/* func_80124060 */
INCLUDE_ASM("asm/nonmatchings/main", MoveImage2);

/* func_801241A4 */
INCLUDE_ASM("asm/nonmatchings/main", DrawOTag2);
