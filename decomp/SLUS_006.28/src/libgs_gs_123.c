/* PSY-Q 4.5 libgs, member gs_123.o: Gssub_make_matrix. The first split had
   it in psyq_cd_k.c.
   Built with -fno-delayed-branch @delay-fill (config/cc_flags.txt): this object puts the stack adjust BEFORE `jr $ra`.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"

/* cc97_lib: GsDrawOt */

/* cc97_lib: Gssub_make_matrix */
extern MATRIX D_800E7050;

/* libgs Gssub_make_matrix (GS_123.OBJ): builds the rotation matrix about one axis from a
   sine and a cosine. *m = GsIDMATRIX (D_800E7050, the identity), then by the axis letter,
   either case: 'X' m[1][1] = m[2][2] = c, m[1][2] = -s, m[2][1] = s; 'Y' m[0][0] =
   m[2][2] = c, m[0][2] = s, m[2][0] = -s; 'Z' m[0][0] = m[1][1] = c, m[0][1] = -s,
   m[1][0] = s. Any other letter leaves the identity. */
/* func_8008FD38 */
void Gssub_make_matrix(MATRIX *m, short s, short c, s8 axis)
{
    *m = D_800E7050;
    switch (axis) {
    case 'X':
    case 'x':
        m->m[1][1] = c;
        m->m[2][2] = c;
        m->m[1][2] = -s;
        m->m[2][1] = s;
        break;
    case 'Y':
    case 'y':
        m->m[0][0] = c;
        m->m[2][2] = c;
        m->m[0][2] = s;
        m->m[2][0] = -s;
        break;
    case 'Z':
    case 'z':
        m->m[0][0] = c;
        m->m[1][1] = c;
        m->m[0][1] = -s;
        m->m[1][0] = s;
        break;
    }
}

INCLUDE_PAD(8);
