/* PSY-Q 4.5 libmcrd, member userfunc.o, part 1 of 3: UserFuncInit. The
   first split had it in psyq_misc_aab.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _make_file_discrpt */
extern int (*D_800EA9C0)();
extern s8 D_800EA9C8[];
s32 strcmp(char *, char *);
struct DIRENTRY *firstfile2(char *, struct DIRENTRY *);
static inline int dt_get_firstfile(void)
{
    struct device_table *dt;
    struct device_table *base;
    int n;

    base = *(struct device_table **)0x150;
    n = *(unsigned long *)0x154 / sizeof(struct device_table);
    for (dt = base; dt < base + n; dt++) {
        if (dt->dt_string != 0 && strcmp(dt->dt_string, (char *)D_800EA9C8) == 0) {
            D_800EA9C0 = dt->dt_firstfile;
            return 1;
        }
    }
    return 0;
}
static inline void dt_set_firstfile(int (*func)())
{
    struct device_table *dt;
    struct device_table *base;
    int n;

    base = *(struct device_table **)0x150;
    n = *(unsigned long *)0x154 / sizeof(struct device_table);
    for (dt = base; dt < base + n; dt++) {
        if (dt->dt_string != 0 && strcmp(dt->dt_string, (char *)D_800EA9C8) == 0) {
            dt->dt_firstfile = func;
            break;
        }
    }
}

/* cc97_lib: UserFuncInit */
/* return_slot_fill: UserFuncInit */
/* far_syms: D_800B8470 */
extern s32 D_800B8470;

/* libmcrd UserFuncInit (USERFUNC.OBJ): empty the stack of step functions that
   UserFuncOpen pushes on and UserFuncExecute runs
   from: the top index D_800B8470 = -1. MemCardStart calls it. */
/* func_800A3A18 */
void UserFuncInit(void)
{
    D_800B8470 = -1;
}
