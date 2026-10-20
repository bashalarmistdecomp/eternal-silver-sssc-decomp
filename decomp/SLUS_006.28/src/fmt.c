/* Lunar SSSC — fmt: the developers' text output: debug_printf (empty in
   this build), printf (Sony's members call it by name), the sim:con
   console, and fmt_sprintf and fmt_vsprintf with their digit, width,
   number, string and char helpers. The first split had it in io.c. Every
   function is C.
   Default flags: the stack adjust in the `jr $ra` delay slot.
   The build must stay byte-matching. */

#include "include_asm.h"
#include "psyq_types.h"
#include "inflate.h"
#include "cd.h"
extern s32 g_SimConsoleOpen;
extern s32 g_SimConsoleDescriptor;
extern u8 g_SimConsoleFileName[];
extern void *g_FileHostBlock;
extern s8 *g_FmtWritePosition;
extern s32 D_800B86EC;
void func_800216E0(s32 arg0);
s32 fmt_digit_char(s32 digit, s32 letter);
extern long HWD0, VWD0;
extern long HWD0;
extern long VWD0;
void fmt_put_char(s32 ch);
s32 fmt_digit_value(s32 ch_then_digit);
s32 debug_printf();
extern long HWD0, VWD0;
s8 *fmt_read_width(s8 *, s32 *);
void func_800216E0(s32 arg0);
s32 fmt_digit_char(s32 digit, s32 letter);
void func_800216E0(s32 arg0);
s32 fmt_digit_char(s32 digit, s32 letter);
void printf(unsigned long long fmt, ...);
s32 fmt_sprintf();
void fmt_put_char(s32 ch);
void fmt_put_string(s8 *, s32);

/* Ruling of 2026-10-14 (docs/naming.md): Stores into a word nothing reads, and nothing reaches it: nothing supports a name. */
void func_800216E0(s32 arg0) {
    D_800B86EC = arg0;
}
/* The release build's printf stub: varargs.h style, which is what
   `f(fmt, va_alist) char *fmt; va_dcl` expands to — GCC homes $a1..$a3
   (from va_alist on) and $a0 stays put; a stdarg `(char *, ...)` homes
   $a0 too. Callers pass 1..9 args unprototyped. */
/* func_800216EC */
s32 debug_printf(fmt, __builtin_va_alist)
char *fmt;
int __builtin_va_alist;
{
    return 0;
}

/* func_80021700 */
void printf(unsigned long long fmt, ...)
{
}
s32 fmt_vsprintf(s8 *dst, s8 *fmt, s32 *args);  /* TU defines this below */

/* The game's own sprintf: formats `fmt` into `buf` with the arguments that
   follow, and answers what the vsprintf fmt_vsprintf answers (the number
   of bytes written, the closing 0 included). It hands fmt_vsprintf the
   address of its third argument word as `args`, so the conversions read
   the caller's argument words in place: the third and fourth from the
   homes the prologue fills from $a2 / $a3, the fifth on from the caller's
   outgoing area. Conversions and the "\n" -> "\r\n" rule: see
   fmt_vsprintf. It reads and writes no global itself (fmt_vsprintf
   moves the write position g_FmtWritePosition); no RNG draw, no frame wait.

   Callers: file_load (this TU, above; four arguments), card_save_title_set
   (ui, the save title, seven arguments), MenuCardListDisp (battle), overlay_movie_search
   (cdrom_load) and CellNumberSet (misc_j). None of them reads the
   result.

   The shape is the <varargs.h> one, as debug_printf above:
       sprintf(buf, fmt, va_alist) char *buf, *fmt; va_dcl
       { va_list ap; va_start(ap); return vsprintf(buf, fmt, ap); }
   GCC 2.8.1's prologue homes the argument registers from the LAST
   declared parameter's on (mips.c, mips_expand_prologue), so va_alist in
   $a2 gives the two stores ahead of the frame, `sw $a2, 8($sp)` /
   `sw $a3, 0xC($sp)`, and va_start, which is `ap = (char *)
   &__builtin_va_alist` (ginclude/va-mips.h), makes va_alist an
   address-taken parameter that the body homes once more: the third
   store, `sw $a2, 0x20($sp)`, to the same word.

   MARKED SPELLING: va_alist / va_dcl / va_start are written out as their
   expansions (`int __builtin_va_alist;`, `&__builtin_va_alist`), the tree
   having no varargs.h, as debug_printf; the K&R definition is that
   header's own form and other TUs prototype this function `(s8 *, u8 *,
   ...)` (menu_card.c, outline.c) and `(s8 *, u8 *, s32)` (cell_text.c). The stdarg
   form `(s8 *buf, s8 *fmt, ...)` with `__builtin_next_arg(fmt)` is 10/11
   (it homes $a1 too and has no third store); `(&fmt + 1)` is 9/11; a
   plain `(s8 *, s8 *, s32 arg)` passing `&arg` is 9/11 (no stores ahead
   of the frame). `(s8 *buf, s8 *fmt, s32 arg, ...)` passing `&arg` gives
   the same 11 instructions as this text: the bytes do not tell the two
   apart. */
/* func_80021714 */
s32 fmt_sprintf(buf, fmt, __builtin_va_alist)
s8 *buf;
s8 *fmt;
int __builtin_va_alist;
{
    return fmt_vsprintf(buf, fmt, &__builtin_va_alist);
}
/* Write a string to the development host's console: sim_console_write(s).
   The console is the file "sim:con" (the string at g_SimConsoleFileName). It is
   opened once: while the flag g_SimConsoleOpen is 0, open(g_SimConsoleFileName,
   2) -- the BIOS `open` stub, mode 2 -- and its answer goes to g_SimConsoleDescriptor
   (the descriptor; -1 in the image); -1 is returned at once when the open
   failed (the flag stays 0, so the next call tries again), otherwise
   g_SimConsoleOpen = 1. Then write(g_SimConsoleDescriptor, s, strlen(s)):
   the BIOS `write` stub with strlen(s) bytes, whose answer is returned.
   No RNG, no frame wait, nothing else read or written.

   Nothing in the image calls it or holds its address (no jal, no j and no
   data word for 0x80021740 or 0x80021744). func_800217AC below is the
   same function behind a test.

   The flag's load is the first instruction, ahead of the `addiu $sp`; the
   plain `g_SimConsoleOpen == 0` test gives that by itself. */
/* func_80021740 */
s32 sim_console_write(u8 *s)
{
	s32 r;
	s32 len;

	if (g_SimConsoleOpen == 0) {
		r = open(g_SimConsoleFileName, 2);
		g_SimConsoleDescriptor = r;
		if (r == -1)
			return -1;
		g_SimConsoleOpen = 1;
	}

	len = strlen(s);
	return write(g_SimConsoleDescriptor, s, len);
}

/* far_syms: g_SimConsoleFileName (target reaches it %hi/%lo) */
/* Write a string to the development host's console unless the mode byte
   says not to: func_800217AC(s). When the signed byte at +0xB of the block
   g_FileHostBlock points at (FileHostBlock's unk_0b) is 5, nothing is done
   and 5 is returned. Otherwise it is sim_console_write above, word for word: the console "sim:con"
   (g_SimConsoleFileName) opened once with open(g_SimConsoleFileName, 2) while the
   flag g_SimConsoleOpen is 0 (descriptor to g_SimConsoleDescriptor, -1 returned when the
   open failed, else the flag set to 1), then write(g_SimConsoleDescriptor, s,
   strlen(s)), the BIOS `write` stub with strlen(s) bytes, whose
   answer is returned. No RNG, no frame wait.

   Nothing in the image calls it or holds its address (no jal, no j and no
   data word for 0x800217AC or 0x800217B0).

   The block pointer's load is the first instruction, ahead of the
   `addiu $sp`; reading the byte through the cast global gives that by
   itself. No marker, no forcing device.

   MARKED SPELLING: `return 5;`. The target leaves through the epilogue
   with the compare's own constant 5 still in $v0, and a bare `return;`
   gives the same 32 instructions (32/32 either way): the bytes do not say
   whether the source returned a value there. `return 5;` is kept because
   it is what the code does. */
/* Ruling of 2026-10-14 (docs/naming.md): sim_console_write behind a test of a byte nothing read explains; not a duplicate, and left. */
s32 func_800217AC(u8 *s)
{
	s32 r;
	s32 len;

	if (((FileHostBlock *)g_FileHostBlock)->unk_0b == 5)
		return 5;

	if (g_SimConsoleOpen == 0) {
		r = open(g_SimConsoleFileName, 2);
		g_SimConsoleDescriptor = r;
		if (r == -1)
			return -1;
		g_SimConsoleOpen = 1;
	}

	len = strlen(s);
	return write(g_SimConsoleDescriptor, s, len);
}

/* One character of a decimal run: its digit value 0-9, or -1 when it is
 * not a digit. Reads and writes no global; its only caller is the
 * field-width reader below, which tests the sign of the answer.
 *
 * Shape: the early return is the NOT-a-digit case, `(u32)ch_then_digit >= 10`
 * after the subtraction (7/7). With the digit case returning first, or
 * with a result local preset to -1, the -1 is loaded ahead of the test
 * instead of in the branch's delay slot (2-3/7). */
/* func_8002182C */
s32 fmt_digit_value(s32 ch_then_digit)
{
    ch_then_digit -= '0';
    if ((u32)ch_then_digit >= 10) {
        return -1;
    }
    return ch_then_digit;
}
/* The field-width reader of this TU's printf (fmt_vsprintf, its only
 * caller): reads an optional '-' and a run of decimal digits at `str`,
 * stores the signed value through `out` and answers the position of the
 * first character after the digits. With no digit at all (after the
 * optional '-') nothing is stored, so the caller's default width stands;
 * the '-' is still skipped. fmt_digit_value classifies one character: its
 * digit value, or -1.
 *
 * Reads and writes no global. The first digit is classified twice: once
 * by the test that guards the store, once more by the loop.
 *
 * Shapes: the pointer is the parameter itself, walked in place (a local
 * copy of it puts the second parameter's entry copy first); the value is
 * scaled and added in two statements (`n = n * 10 + d;` builds the sum in
 * a scratch register and gives the pointer the first saved register:
 * 26/42); one `return` after the `if` (an early `return str;` ahead of the
 * loop lets the scheduler lift the last `move $v0` above the `mflo`:
 * 41/42).
 *
 * Both results are tested for their sign (`bltz`): fmt_digit_value is an
 * int function. (Declared `u32`, the guarding call needs an `(s32)` cast,
 * or the unsigned `>= 0` folds away: 38/42.) */
/* func_80021848 */
s8 *fmt_read_width(s8 *str, s32 *out)
{
    s32 n;
    s32 sign;
    s32 d;

    n = 0;
    sign = 1;
    if (*str == '-') {
        str++;
        sign = -1;
    }
    if (fmt_digit_value(*str) >= 0) {
        while ((d = fmt_digit_value(*str)) >= 0) {
            n = n * 10;
            n += d;
            str++;
        }
        *out = n * sign;
    }
    return str;
}

/* func_800218F0 */
s32 fmt_digit_char(s32 digit, s32 letter) {
    s32 char_offset;

    char_offset = digit;
    if (char_offset >= 0xA) {
        char_offset += letter;
    }
    return char_offset + 0x30;
}
extern s8 g_FmtNumberDigits[];

/* The number conversion of this TU's printf (fmt_vsprintf): writes n in
 * `base` through the output pointer g_FmtWritePosition, left-padded with `pad` to
 * `width`. conv is the conversion character: 'x' gives lower-case letters
 * (fmt_digit_char's offset 0x27), anything else upper-case (7). Only base
 * 10 is signed. The digits are built least significant first in the
 * scratch buffer g_FmtNumberDigits and written out backwards. A negative number
 * padded with '0' gets its '-' before the padding (and the sign counts
 * toward the width); with any other pad the '-' joins the digits, after
 * the padding.
 *
 * Declared s32 with no return statement, as an old-style definition with
 * no return type would be: no caller reads a value, but the delay-slot
 * filler treats $v0 as needed at the function's end, which is what keeps
 * `lui $v0` out of the slot of the last loop's entry branch (void: 101/102). */
/* func_80021908 */
s32 fmt_put_number(s32 n, s32 width, u32 base, s32 conv, s32 pad)
{
    s32 neg;
    s32 letter;
    s32 i;
    s32 ch;

    neg = 0;
    letter = 7;
    if (conv == 'x') {
        letter = 0x27;
    }
    if (base == 10 && n < 0) {
        n = -n;
        neg = 1;
    }
    i = 0;
    do {
        g_FmtNumberDigits[i] = fmt_digit_char(n % base, letter);
        n = n / base;
        i++;
    } while (n != 0);
    if (neg && pad == '0') {
        *g_FmtWritePosition++ = '-';
        for (; i + 1 < width; width--) {
            *g_FmtWritePosition++ = pad;
        }
    } else {
        if (neg) {
            g_FmtNumberDigits[i++] = '-';
        }
        for (; i < width; width--) {
            *g_FmtWritePosition++ = pad;
        }
    }
    while (i > 0) {
        i--;
        /* through an int: the buffer is read with a sign-extending lb */
        ch = g_FmtNumberDigits[i];
        *g_FmtWritePosition++ = ch;
    }
}
/* The %s conversion of this TU's printf (fmt_vsprintf, its only caller):
 * appends the string `str` through the output pointer g_FmtWritePosition in a
 * field of `width` characters, padding with spaces. A positive width
 * right-justifies: `width - len` spaces come first when the string is
 * shorter than the field. A negative width left-justifies: after the
 * string come `-width - len` spaces when the string is shorter than
 * `-width`. A string as long as the field or longer is written whole; no
 * closing 0 is written (the caller appends it at the end of the format).
 *
 * Reads the string's bytes; writes through g_FmtWritePosition and steps it once
 * per character. Callee: strlen, called with no
 * declaration in scope as everywhere in this TU. No RNG draw, no frame
 * wait.
 *
 * Shapes: one counter for both pads, and the trailing pad's count written
 * as `-width - len` right under the `len < -width` test. The counter then
 * sits in $a1, the register `width` arrived in: the negation's temporary
 * is set where `width` dies and takes over its preference for $a1, and the
 * counter takes it from the temporary (global.c expand_preferences; the
 * -dg dump lists "preferences: 5" for both). Assigned `-width` itself
 * (`n = -width; if (len < n) { n -= len; ...`) the counter conflicts with
 * `width` across the first loop, inherits nothing and lands in $v1:
 * 39/53. `width = -width;` first is 37/53, a second counter for the
 * trailing pad 47/53. Each byte of the string is read twice, once
 * sign-extended by the loop test and once for the store. */
/* func_80021AA0 */
void fmt_put_string(s8 *str, s32 width)
{
    s32 len;
    s32 n;

    len = strlen(str);
    if (len < width) {
        for (n = width - len; n != 0; n--) {
            *g_FmtWritePosition++ = ' ';
        }
    }
    while (*str != 0) {
        *g_FmtWritePosition++ = *str++;
    }
    if (width < 0) {
        if (len < -width) {
            for (n = -width - len; n != 0; n--) {
                *g_FmtWritePosition++ = ' ';
            }
        }
    }
}

/* func_80021B74 */
void fmt_put_char(s32 ch) {
    s8 *write_position = g_FmtWritePosition;

    *write_position = ch;
    g_FmtWritePosition = write_position + 1;
}

/* The game's own vsprintf: formats `fmt` into `dst`, taking the arguments
   as words from `args`, and answers the number of bytes written, the
   closing 0 included. The write position is g_FmtWritePosition; the helpers above
   append through it (fmt_put_number a number, fmt_put_string a string,
   fmt_put_char one character).

   After a '%': the field width starts at 1, a '0' right after the '%'
   makes the pad character '0' instead of ' ', and fmt_read_width reads an
   optional width ('-' and digits) into `width` and answers the position of
   the conversion letter. %d %D are decimal and %x %X hexadecimal
   (fmt_put_number gets the letter too: 'x' is the lower-case one), %s a
   string, %c a character, %% a percent sign; any other letter is dropped
   together with its width. A '\n' of the format is written as "\r\n".

   Callers: fmt_sprintf (the sprintf form, its own variable arguments as
   `args`) and printmes (into g_PrintmesBuffer, then drawn character by
   character).

   The copied character goes through an int (`ch`): the target reads it
   sign-extended. fmt_put_char takes an int: %c passes the argument word
   as it was loaded. */
/* func_80021B90 */
s32 fmt_vsprintf(s8 *dst, s8 *fmt, s32 *args)
{
    s32 width;
    s32 pad;
    s32 type;
    s32 ch;
    s8 *p;

    g_FmtWritePosition = dst;
    while (*fmt != 0) {
        if (*fmt == '%') {
            width = 1;
            p = fmt + 1;
            pad = ' ';
            if (*p == '0') {
                pad = '0';
            }
            fmt = fmt_read_width(p, &width);
            type = *fmt++;
            switch (type) {
            case 'd':
            case 'D':
                fmt_put_number(*args++, width, 10, type, pad);
                break;
            case 'x':
            case 'X':
                fmt_put_number(*args++, width, 16, type, pad);
                break;
            case 's':
                fmt_put_string((s8 *)*args++, width);
                break;
            case 'c':
                fmt_put_char(*args++);
                break;
            case '%':
                fmt_put_char('%');
                break;
            }
        } else {
            if (*fmt == '\n') {
                *g_FmtWritePosition++ = '\r';
            }
            ch = *fmt++;
            *g_FmtWritePosition++ = ch;
        }
    }
    *g_FmtWritePosition++ = 0;
    return g_FmtWritePosition - dst;
}
