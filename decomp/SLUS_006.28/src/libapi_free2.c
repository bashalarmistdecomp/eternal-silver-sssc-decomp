/* PSY-Q 4.5 libapi, member free2.o: free2. The first split had it in
   psyq_gpu_b.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
#include "path.h"
#include "btl.h"
/* far_syms: D_800E55C4 (target reaches it %hi/%lo) */
extern u_long *D_800E55C4;

/* cc97_lib: free2 */

/* libapi free2(p) (FREE2.OBJ): free a malloc2 block. Sets bit 0 (free) in the
   header word below p and, when that header lies below the first-fit candidate
   (_Candidate, D_800E55C4), makes it the candidate. */
/* func_8008B6A8 */
void free2(u_long *p)
{
    p[-1] |= 1;
    p--;
    if (p < D_800E55C4) {
        D_800E55C4 = p;
    }
}

INCLUDE_PAD(8);
