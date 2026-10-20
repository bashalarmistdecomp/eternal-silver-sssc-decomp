/* PSY-Q 4.5 libmcrd, member libmcrd.o, part 13 of 13: _make_file_discrpt. The
   first split had it in psyq_misc_aab.c. The tree compiles this run of the
   member's functions with other flags than the runs beside it
   (config/cc_flags.txt), so the member is in several files.
   The build must stay byte-matching. */


#include "include_asm.h"
#include "psyq_types.h"
#include "psyq_private.h"
/* cc97_lib: _make_file_discrpt */
extern char D_80013BE8[];
void *memcpy(void *, const void *, unsigned int);

/* libmcrd _make_file_discrpt(chan, buf) (LIBMCRD.OBJ, static): write the memory-card
   device prefix for port chan into buf: the string "func_800A3510:" (D_80013BE8, copied with
   its NUL), then its two digits replaced by chan / 16 and chan % 16 as ASCII, so
   chan 0x00 gives "func_800A3510:" and chan 0x10 "bu10:". The callers (MemCardOpen,
   MemCardReadFile / WriteFile, MemCardGetDirentry, MemCardCreateFile,
   MemCardDeleteFile) strcat the file name onto it. */
/* func_800A35BC */
void _make_file_discrpt(s32 chan, char *buf)
{
    memcpy(buf, D_80013BE8, 6);
    buf[2] = chan / 16 + '0';
    buf[3] = chan % 16 + '0';
}
