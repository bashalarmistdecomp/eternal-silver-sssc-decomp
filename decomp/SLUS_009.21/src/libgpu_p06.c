/* PSY-Q 4.5 libgpu, member p06.o: AddPrim. The first split had it in
   main_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
/* redistributed from the original TU (split decl repair) */
extern int addr;
extern s32 mask;

/* redistributed from the original TU (split decl repair) */
extern void (*D_801336C8_s)();

/* AddPrim(ot, p): splice primitive p into the ordering table at ot.
 * The 24-bit address field goes through a variable rather than an
 * inline constant -- that is what puts the two masks in the target's
 * registers; a plain `x & 0xFFFFFF` is constant-propagated and
 * allocates them the other way round. */
/* func_80124D00 */
void AddPrim(void *ot, void *p) {
    int addr;
    s32 mask;

    addr = (mask = *(s32 *) ot);
    mask = 0xFFFFFF;
    *(s32 *) p = (*(s32 *) p & 0xFF000000) | (addr & mask);
    addr = mask;
    *(s32 *) ot = (*(s32 *) ot & 0xFF000000) | ((s32) p & addr);
}

INCLUDE_PAD(4);
