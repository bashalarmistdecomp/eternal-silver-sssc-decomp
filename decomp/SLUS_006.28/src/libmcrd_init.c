/* PSY-Q 4.5 libmcrd, member init.o: MemCardInit, MemCardEnd. The first
   split had it in psyq_misc_ak.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"

/* libmcrd MemCardInit(val): InitCARD(val), StartCARD, _bu_init. card.c calls it
   with 0. This and MemCardEnd sit between libspu's last object and libcard's INIT.OBJ,
   and their bodies are BIOS.OBJ's _card_open / _card_close word for word, so the bodies
   do not say which pair this is; the place does. PSY-Q 4.7's libmcrd init.o, which holds
   exactly these two, lies whole here and nowhere else (config/symbol_addrs.txt). */
/* func_800A1068 */
void MemCardInit(long val) {
    InitCARD(val);
    StartCARD();
    _bu_init();
}

/* libmcrd MemCardEnd: StopCARD. */
/* func_800A1098 */
void MemCardEnd(void) {
    StopCARD();
}
