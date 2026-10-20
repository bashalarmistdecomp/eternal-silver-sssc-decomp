/* PSY-Q 4.5 obj, member 2mbyte.obj: __SN_ENTRY_POINT. The first split had
   it in boot_init.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* The start-up code: __SN_ENTRY_POINT of Sony's 2mbyte.obj (PSY-Q 4.5), which calls
   InitHeap and then main. Hand-written assembly: splat emits it as raw .word (it
   contains invalid instructions and has no `jr`). Never convertible to C. */
/* func_80013C4C */
INCLUDE_ASM("asm/nonmatchings/boot_init", __SN_ENTRY_POINT);
