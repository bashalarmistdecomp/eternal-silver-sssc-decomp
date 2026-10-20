/* PSY-Q 4.5 libcd, member sys.o, part 5 of 7: CdSync, CdReady. The first
   split had it in psyq_cd_s.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

extern s32 CD_sync(s32 mode, u8 *result);
extern s32 CD_ready(s32 mode, u8 *result);

/* cc97_lib: CdSync */
/* libcd CdSync / CdReady (SYS.OBJ): the API names of BIOS.OBJ's CD_sync / CD_ready. */
/* func_8009059C */
s32 CdSync(s32 mode, u8 *result) {
    return CD_sync(mode, result);
}

/* cc97_lib: CdReady */
/* func_800905BC */
s32 CdReady(s32 mode, u8 *result) {
    return CD_ready(mode, result);
}
