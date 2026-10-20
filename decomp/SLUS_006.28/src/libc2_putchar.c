/* PSY-Q 4.5 libc2, member putchar.o: _putchar, _putchar_flash, putchar. The
   first split had it in psyq_cd_x.c.
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
void _putchar(s8 c);
void _putchar_flash(void);

/* cc97_lib: _putchar */
/* return_slot_fill: _putchar */
/* lib_g0: _putchar */
/* far_syms: D_800B6680 (target reaches it %hi/%lo) */
extern s32 D_800B6680;              /* col: the console column (PUTCHAR.OBJ .data + 0) */
extern s32 D_800B6684;              /* n: bytes waiting in the line buffer (.data + 4) */
extern u8 D_800E7140[];             /* buf[32] (PUTCHAR.OBJ .bss) */
extern s8 D_800B6691[];             /* _ctype_ + 1 (ctype.h: `extern char _ctype_[]`, signed here) */

/* libc2 _putchar (PUTCHAR.OBJ; the object's three functions are word for word the 4.4
   object's, 152 of 152 at 0x80092238): one character to the console's line buffer. '\n'
   first sends '\r' through itself and resets the column; '\t' sends spaces through itself
   until the column is a multiple of 8 and returns; any other character counts a column
   when isprint() ((_ctype_ + 1)[c] & (_P | _U | _L | _N | _B) = 0x97). Then the byte is
   appended to buf[32], after a write(1, buf, n) when the buffer is full.
   `lib_g0`: the object was built -O3 -G0, and only as a FAR scalar does n keep the
   address register its used post-increment forces (la / lw 0(r) / sw 0(r) in
   `buf[n++] = c`; 59/63 at the pipeline's -G8, where cse folds the register away). */
/* func_80092238 */
void _putchar(s8 c)
{
    switch (c) {
    case '\n':
        _putchar('\r');
        D_800B6680 = 0;
        break;
    case '\t':
        do {
            _putchar(' ');
        } while (D_800B6680 & 7);
        return;
    default:
        if (D_800B6691[(u8) c] & 0x97) {
            D_800B6680++;
        }
        break;
    }
    if (D_800B6684 >= 32) {
        write(1, D_800E7140, D_800B6684);
        D_800B6684 = 0;
    }
    D_800E7140[D_800B6684++] = c;
}

/* cc97_lib: _putchar_flash */
/* far_syms: D_800B6684 (target reaches it %hi/%lo) */
/* far_syms: D_800E7140 (target reaches it %hi/%lo) */
extern u8 D_800E7140[];

extern s32 D_800B6684;

/* libc2 _putchar_flash (PUTCHAR.OBJ, +0xFC in the 4.4 object): write the line buffer's
   waiting bytes out, if any: write(1, buf, n); n = 0. Called by puts, and expanded in line
   in putchar. The object's own recipe (cc97_lib, the jr slot left to the lw $ra rule). */
/* func_80092334 */
void _putchar_flash(void) {
    if (D_800B6684 > 0) {
        write(1, D_800E7140, D_800B6684);
        D_800B6684 = 0;
    }
}
/* cc97_lib: putchar */
/* return_slot_fill: putchar */
/* inline_functions: putchar */

/* libc2 putchar (PUTCHAR.OBJ): _putchar(c); _putchar_flash(); with both bodies expanded in
   line by the compiler (`inline_functions`: the object was built -O3; the recursive calls
   inside the first stay calls of the out-of-line _putchar). So one character is
   buffered as _putchar does and the buffer is written out at once. */
/* func_80092370 */
void putchar(s8 c)
{
    _putchar(c);
    _putchar_flash();
}
