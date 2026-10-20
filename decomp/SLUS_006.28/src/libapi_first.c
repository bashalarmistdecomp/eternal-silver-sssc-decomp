/* PSY-Q 4.5 libapi, member first.o: firstfile, func_800A37D4. The first
   split had it in psyq_misc_aab.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _make_file_discrpt */
/* cc97_lib: firstfile */
/* return_slot_fill: firstfile */
/* reorder_fill: firstfile */
extern int (*D_800EA9C0)();
extern s8 D_800EA9C8[];
s32 strcmp(char *, char *);
int func_800A37D4(struct iob *io, int arg1, int arg2);
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
/* libapi firstfile(name, dir) (FIRST.OBJ): the library's wrapper round the BIOS
   firstfile (firstfile2, B(42h)). It copies the leading characters of name that
   are above ':' (signed compare, so the copy stops at the first digit, ':' or NUL:
   "func_800A3510:..." gives "bu") into D_800EA9C8, finds the device of that name in the
   kernel's device table, saves its firstfile function in D_800EA9C0 (returns 0 when
   there is no such device), installs func_800A37D4 in its place and calls the BIOS
   firstfile with the same two arguments. The only caller is MemCardGetDirentry
   . */
/* func_800A3638 */
struct DIRENTRY *firstfile(char *name, struct DIRENTRY *dir)
{
    s8 *s;
    s8 *d;

    s = name;
    d = D_800EA9C8;
    while (*s > ':') {
        *d++ = *s++;
    }
    *d = 0;
    if (dt_get_firstfile() == 0) {
        return 0;
    }
    dt_set_firstfile(func_800A37D4);
    return firstfile2((char *)name, dir);
}
/* cc97_lib: func_800A37D4 */
/* return_slot_fill: func_800A37D4 */
/* reorder_fill: func_800A37D4 */

/* libapi FIRST.OBJ's static device hook: firstfile installs it as the
   firstfile function (device_table +0x34) of the device named in D_800EA9C8, so the
   BIOS firstfile calls it with the file's I/O block and two more arguments. It sets
   the block's i_flgs (+0) to 1 when that is 0, puts the device's own firstfile
   function (saved in D_800EA9C0) back with the same walk of the kernel's device
   table (dt_set_firstfile: the table's address at 0x150, its size in bytes at
   0x154, 0x50-byte records, the device found by strcmp on dt_string), and calls
   that function with the three arguments unchanged, returning its result. */
int func_800A37D4(struct iob *io, int arg1, int arg2)
{
    if (io->i_flgs == 0) {
        io->i_flgs = 1;
    }
    dt_set_firstfile(D_800EA9C0);
    return (*D_800EA9C0)(io, arg1, arg2);
}

INCLUDE_PAD(4);
