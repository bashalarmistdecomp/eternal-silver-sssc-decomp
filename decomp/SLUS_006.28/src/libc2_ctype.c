/* PSY-Q 4.5 libc2, member ctype.o: toupper, tolower. The first split had it
   in psyq_cd_aa.c.
   The build must stay byte-matching. */


#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"

/* far_syms: D_800B6691 (target reaches it %hi/%lo) */
extern u8 D_800B6691[];

/* cc97_lib: toupper */
/* libc2 toupper (TOUPPER.OBJ): c - 0x20 when (_ctype_ + 1)[c] (D_800B6691) has the
   lower-case bit (2), else c. */
/* func_80092498 */
s8 toupper(u8 c)
{
	s8 v;

	v = c;
	if (D_800B6691[c] & 2)
		v = c - 0x20;

	return v;
}
/* cc97_lib: tolower */

/* libc2 tolower (TOLOWER.OBJ): c + 0x20 when (_ctype_ + 1)[c] has the upper-case bit
   (1), else c. toupper's twin. */
/* func_800924CC */
s8 tolower(u8 c)
{
	s8 v;

	v = c;
	if (D_800B6691[c] & 1)
		v = c + 0x20;

	return v;
}

INCLUDE_PAD(8);
