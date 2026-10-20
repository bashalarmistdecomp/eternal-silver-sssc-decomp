/* PSY-Q 4.5 libc2, member puts.o: puts. The first split had it in
   psyq_cd_x.c.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: CD_datasync */
/* reorder_fill: CD_datasync */
/* return_slot_fill: CD_datasync */
/* far_syms: D_800B6375 */
extern u8 D_800B6375;
extern s32 D_800B637C[];
extern s32 D_800B63FC[];
extern volatile u8 D_800B6634[];
extern CdAlarm D_800E7120;
extern char D_80013400[];
extern char D_80013410[];
s32 printf();
void puts();
s32 CD_flush();
s32 VSync();

static inline void cd_set_alarm(char *name)
{
    /* arm the timeout: 960 vertical blanks (16 s at 60 Hz) from now */
    D_800E7120.time = VSync(-1) + 960;
    D_800E7120.count = 0;
    D_800E7120.name = name;
}
static inline int cd_get_alarm(void)
{
    /* -1 once the alarm's VSync count has passed or after 0x3C0000 polls: prints
       "CD timeout: " and "%s:(%s) Sync=%s, Ready=%s" (waiter, command name, the
       two interrupt-state names) and flushes the controller (CD_flush); else 0 */
    if (D_800E7120.time < VSync(-1) || D_800E7120.count++ > 0x3C0000)
    {
        puts(D_80013400);
        printf(D_80013410, D_800E7120.name, D_800B637C[D_800B6375],
                      D_800B63FC[D_800B6634[0]], D_800B63FC[D_800B6634[1]]);
        CD_flush();
        return -1;
    }
    return 0;
}

/* cc97_lib: puts */
/* return_slot_fill: puts */
/* reorder_fill: puts */
extern s8 D_80013524[];             /* "<NULL>" */
void _putchar(s8 c);
void _putchar_flash(void);  /* TU defines this below */

/* libc2 puts (PUTS.OBJ; word for word the 4.4 object): write a string to the console
   through PUTCHAR.OBJ's line buffer: each character goes to _putchar (which
   expands tabs, puts a CR before each LF and buffers 32 bytes), then _putchar_flash
    writes the buffer out. A null pointer prints "<NULL>". No newline is
   added (this is not ANSI puts). Plain char is s8: the library was built with signed
   char (lbu, then a sign extension for the argument). */
/* func_800921D8 */
void puts(s8 *s)
{
    s8 c;

    if (s == 0) {
        s = D_80013524;
    }
    while ((c = *s++) != 0) {
        _putchar(c);
    }
    _putchar_flash();
}

INCLUDE_PAD(12);
