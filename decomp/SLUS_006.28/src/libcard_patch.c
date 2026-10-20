/* PSY-Q 4.5 libcard, member patch.o: _patch_card_info, func_800A169C,
   func_800A16C8, func_800A170C, _patch_card, _patch_card2,
   _copy_memcard_patch. The first split had it in psyq_misc_ao.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: _Pad1 */

static inline void pad_wait(volatile s32 n)
{
    while (--n != -1) {
    }
}

/* func_800A1658 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", _patch_card_info);

INCLUDE_ASM("asm/nonmatchings/psyq_misc", func_800A169C);

INCLUDE_ASM("asm/nonmatchings/psyq_misc", func_800A16C8);

INCLUDE_ASM("asm/nonmatchings/psyq_misc", func_800A170C);

/* func_800A1734 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", _patch_card);

/* func_800A17C8 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", _patch_card2);

/* func_800A1838 */
INCLUDE_ASM("asm/nonmatchings/psyq_misc", _copy_memcard_patch);
