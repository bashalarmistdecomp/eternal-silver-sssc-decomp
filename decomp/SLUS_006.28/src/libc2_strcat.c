/* PSY-Q 4.5 libc2, member strcat.o: strcat. The first split had it in
   psyq_misc_aab.c.
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

/* cc97_lib: strcat */
/* return_slot_fill: strcat */
int strlen(s8 *s);

/* libc2 strcat(dst, src) (STRCAT.OBJ): append the string src, terminator included, to
   the end of the string dst and return dst. A null dst or a null src appends nothing
   and returns 0, and so does a src that ends where dst ends (dst + strlen(dst) ==
   src + strlen(src): src lies inside dst, and appending would
   overwrite its own terminator and never stop). The end of dst is then found again
   by a scan, not taken from the strlen result. */
/* func_800A38E8 */
s8 *strcat(s8 *dst, s8 *src)
{
    s8 *ret;

    if (dst == 0 || src == 0) {
        return 0;
    }
    if (dst + strlen(dst) != src + strlen(src)) {
        ret = dst;
        while (*dst++ != 0) {
        }
        dst--;
        while ((*dst++ = *src++) != 0) {
        }
        return ret;
    }
    return 0;
}

INCLUDE_PAD(12);
