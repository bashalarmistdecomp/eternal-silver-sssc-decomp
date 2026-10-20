/* PSY-Q 4.5 libgs, member gs_107.o, part 1 of 3: GsSetFlatLight. The first
   split had it in psyq_cd_h.c. The tree compiles this run of the member's
   functions with other flags than the runs beside it (config/cc_flags.txt),
   so the member is in several files.
   The build must stay byte-matching. */



#include "include_asm.h"
#include "psyq_types.h"
/* cc97_lib: GsSetFlatLight */
/* return_slot_fill: GsSetFlatLight */
extern MATRIX D_800E6FF0;
extern void func_8008FB14(MATRIX *m);
extern void func_8008FAB0(MATRIX *m);

/* libgs GsSetFlatLight (GS_107.OBJ): sets flat light source `id` (0 to 2) from
   lt. Row `id` of the light matrix GsLIGHTWSMATRIX (D_800E6FF0) becomes the unit
   vector -(vx, vy, vz) / |v| in 4.12 fixed point (|v| from SquareRoot0), and column `id` of the local colour matrix _LC (D_800E7010, read
   with func_8008FB14 and written back, and sent to the GTE, with func_8008FAB0)
   becomes the colour (r, g, b) * 4096 / 255. Returns 0, or -1 for a zero vector,
   in which case nothing is written. An id above 2 rewrites both matrices
   unchanged. */
/* func_8008F5F8 */
int GsSetFlatLight(int id, GsF_LIGHT *lt)
{
    MATRIX lm;
    MATRIX cm;
    u_char r, g, b;
    long len;

    r = lt->r;
    g = lt->g;
    b = lt->b;
    lm = D_800E6FF0;
    func_8008FB14(&cm);
    len = SquareRoot0(lt->vx * lt->vx + lt->vy * lt->vy + lt->vz * lt->vz);
    if (len == 0) {
        return -1;
    }
    switch (id) {
    case 0:
        lm.m[0][0] = (-lt->vx << 12) / len;
        lm.m[0][1] = (-lt->vy << 12) / len;
        lm.m[0][2] = (-lt->vz << 12) / len;
        cm.m[0][0] = (r << 12) / 255;
        cm.m[1][0] = (g << 12) / 255;
        cm.m[2][0] = (b << 12) / 255;
        break;
    case 1:
        lm.m[1][0] = (-lt->vx << 12) / len;
        lm.m[1][1] = (-lt->vy << 12) / len;
        lm.m[1][2] = (-lt->vz << 12) / len;
        cm.m[0][1] = (r << 12) / 255;
        cm.m[1][1] = (g << 12) / 255;
        cm.m[2][1] = (b << 12) / 255;
        break;
    case 2:
        lm.m[2][0] = (-lt->vx << 12) / len;
        lm.m[2][1] = (-lt->vy << 12) / len;
        lm.m[2][2] = (-lt->vz << 12) / len;
        cm.m[0][2] = (r << 12) / 255;
        cm.m[1][2] = (g << 12) / 255;
        cm.m[2][2] = (b << 12) / 255;
        break;
    }
    D_800E6FF0 = lm;
    func_8008FAB0(&cm);
    return 0;
}
