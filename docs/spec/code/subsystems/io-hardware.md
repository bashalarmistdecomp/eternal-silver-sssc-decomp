# I/O and hardware: files, the card, text and the pad

The game's own functions that read the disc and the development host's
files, keep memory, use the memory card, format text, put pictures and
sprites straight into packets, and read the controllers, with the one
libcd function the other pages send here: the CD-ROM interrupt service.
What Sony's libraries do on their side is on the SDK pages
([psyq/index.md](psyq/index.md)).

## Who touches the hardware

The game's own code (`0x80013D08` to `0x8008B387`) touches no I/O register.
The only `0x1F80xxxx` addresses in its C are words of the scratchpad:
`g_BgSlotBlocks` (`0x1F800000`), `g_BgAttributeSlotBlocks` (`0x1F800020`),
`g_PacketOtBase` (`0x1F800040`), the tiles' colour word (`0x1F800044`),
`g_BgSecondPacketCol` (`0x1F800048`) and `g_PacketWritePointer`
(`0x1F80004C`). The GPU, DMA, SPU, controller and CD-ROM registers are
reached only by Sony's libraries (libapi, libcd, libetc, libgpu, libpad,
libsnd, libspu), and the BIOS only through their stubs
([bios-calls.md](bios-calls.md)).

## Reading the disc

### The CD read and its status: `cd_read_start`, `cd_read_check`, `cd_sector_load`

Three functions of cdread.c make the read of whole sectors.

* `void cd_read_start(s32 lba, s32 buf, s32 size)` (`0x80020DC0`) starts
  the read and returns at once. In order: `g_CdReadBuffer = buf` (the
  destination of the read in flight); `*buf = 0x5555AAA5` (the marker
  `cd_read_check` looks for); `size += 0x7FF; size /= 0x800` (sectors, a
  signed division, rounding up); `CdIntToPos(lba, &loc)` (`0x80094818`);
  `CdControl(2, &loc, 0)` (`CdlSetloc`); `CdRead(sectors, buf, 0x80)`
  (`0x800935F8`, at `CdlModeSpeed`). One caller, `cd_sector_load`. `buf`
  is an address; the C keeps it `s32` because the callers in cdread.c carry
  it that way. Sizes that reach it: 0x800 (`cd_dir_init` (`0x80020F98`),
  sector 0x12), 0x2000 (`cd_dir_read` (`0x800212EC`)), and `cd_load`'s
  clamped byte length.
* `s32 cd_read_check(s32 mode)` (`0x80020E34`) answers how that read
  stands. `r = CdReadSync(mode, 0)` (`0x80093794`). A non-zero `r` is
  returned as it is: sectors still to come (> 0), or libcd's -1 (more than
  1200 vertical blanks since `CdRead`). When `r` is 0 (libcd says the read
  is complete) but the buffer's first word `*g_CdReadBuffer` is still
  0x5555AAA5, the answer is -2: complete by libcd, no sector in the buffer.
  Otherwise 0. Mode 0 waits inside libcd (a busy loop on the vertical-blank
  count; no frame is presented); a non-zero mode returns at once.
* `void cd_sector_load(s32 sector, s32 buffer, s32 byte_count)`
  (`0x80020E88`) is the blocking loop. While `g_XaPlayCount` is not 0 it
  first calls `VoiceStep(0)` (`0x80055524`). Then `cd_read_start` and
  `cd_read_check(1)` once a frame: each frame of the wait is
  `BtlFrameStep` (`0x80038D2C`) when a battle runs and
  `g_CdWaitBtlFrame` is set, else `VSync(0)`. An answer above 0 waits on,
  up to 0x1E0 frames; 0 ends the load; anything else (or the 0x1E0 frames)
  starts the read again, with libcd's `CdInit` first when
  `CdSearchFile` (`0x80092508`) does not find `LUNADATA.FIL` or after 0x40
  tries.

`file_read_check` (`0x80021130`) would answer `cd_read_check(0)` when the
files come from the disc and 0 from the host, but nothing calls it.

Only `cd_read_start` and `cd_read_check` touch `g_CdReadBuffer`. A file
whose own first word is 0x5555AAA5 would answer -2 every time: a
consequence of the code, not observed.

### The directory table: `cd_dir_add` (`0x80021454`)

`cd_dir_add(CdDirRecord *rec)` adds one ISO 9660 directory record to the
game's own directory table. `cd_dir_read` calls it for each record of a
directory (up to four sectors, at most 0xA0 rows); `cd_dir_init` reads
sector 0x12, checks that its first entry points at sector 0x16, and starts
`cd_dir_read` on the root directory there after `g_CdDirRowCount = 0`.
The table: address in the word `g_CdDirRows` (0x80010100, set by
`system_init` in frame.c), row count `g_CdDirRowCount`, rows of 0x18 bytes
(`CdDirRow`, include/cd.h):

| Offset | Field | Meaning |
|---|---|---|
| +0x00 | `name` (`s8[14]`) | file name, 0-terminated, without the ";1" version suffix |
| +0x0E | `flags` (`s16`) | record byte +0x18 \| record byte +0x19 << 8 (0x200 = directory) |
| +0x10 | `first_sector` (`s32`) | the record's extent (halves at +2 and +4) |
| +0x14 | `size` (`s32`) | the record's data length in bytes (halves at +0xA and +0xC) |

A record whose first name character (signed, record +0x21) is 0x20 or
less is skipped: the directory's own two entries (names 0x00 and 0x01).
Otherwise the count goes up by one and the row is filled; the name is
copied for up to the signed length at record +0x20, ending early at ';'.
Both flag bytes are sign-extended before the OR. The name copy has no
bound: the three values are stored first, so a name of 14 or more
characters would overwrite them (not observed). Readers: `cd_dir_find`
(`0x80021164`, by name), `cd_load` (`0x800211D8`; +0x10, +0x14),
`cd_file_seek` (`0x80021284`; +0x10), `cd_subdir_read` (`0x800213D8`;
+0x0E & 0x200, then `cd_dir_read` on +0x10).

### The by-number resource read: `resource_load` (`0x80020230`)

`s32 resource_load(s32 resource, u32 dst)` reads LUNADATA resource
`resource` whole and raw to the address `dst`:

    idx = resource * 2;
    len = g_ResourceSectorPairs[idx + 1] << 11;
    file_load(g_ResourceLunadataFileName, dst, g_ResourceSectorPairs[idx] << 11, len);
    return len;

* `g_ResourceSectorPairs` is the resource sector table (overview.md
  §"The resource sector table"): u16 pairs, word `resource * 2` the resource's
  first sector in LUNADATA.FIL and word `resource * 2 + 1` its length in
  sectors. Entry 0 is {0, 599}, entry 1 {16, 33}.
* `g_ResourceLunadataFileName` is the string "LUNADATA.FIL". The call is
  `file_load(name, dst, first << 11, sectors << 11)`: the byte offset and
  the byte length of 2048-byte sectors.
* It returns the byte length `sectors << 11`; `file_load`'s own result is
  dropped. `dst` is passed through untouched. Nothing bounds the resource
  number. No frame wait of its own, no RNG draw.

21 calls in eleven files: btl_draw.c 2, btl_show_frame.c 1, chr_level.c 1,
face.c 2, lns_intp.c 2, map_object.c 2, map_resource.c 1, map_sprite.c 5,
resource_load.c 1 (`resource_load_run` (`0x800202CC`)), sprite_list.c 3,
tile_layer.c 1. Several files declare it with another pointer or integer
type for the second argument, with no effect on the bytes.

`resource_seek(resource)`, the function after it, is the seek without a
read: `file_seek("LUNADATA.FIL", first << 11)` (`0x800210FC`) from the same
table (sound.md §"BGM numbers" has what that seek is for). The two are the
only users of `g_ResourceSectorPairs`. `resource_seek` is defined `void`,
declared `s32` in sound.c, and called undeclared by map_load.c's `MapSeek`
(`0x8006891C`), which returns its value.

### The named-file read: `file_load` (`0x80021034`)

`s32 file_load(u8 *name, u32 dst, s32 offset, s32 len)` reads `len` bytes
at byte offset `offset` of the file `name` to `dst`. Callers: `resource_load`
(resource_load.c), `overlay_disc_wait` (`0x80023184`) and `overlay_load`
(`0x80023240`) in overlay.c; none reads the result. The mode is
`host_mode` (+0xA, signed) of the block `g_FileHostBlock` points at
(`FileHostBlock`, include/cd.h, at 0x80010000 below the program; nothing in
the program writes it, and the boot file's loader zeroes it, so the game on
the disc reads from the disc: [bootstrap.md](bootstrap.md)):

* `<= 0` (the disc): `cd_load(name, dst, offset, len)` (`0x800211D8`),
  which finds the file's row, clamps the length to the file's, runs
  `cd_sector_load` from the file's first sector + `offset >> 11`, and
  answers the file's length, or -1 for a name not in the table.
* `> 0` (the development host PC): (1) `cd_load(g_FileLunadataFileName,
  dst, D_800B86E0 << 24, 1)`, a one-sector disc read at byte offset 0 or
  0x1000000 of the archive into `dst`; (2) `D_800B86E0 ^= 1` (a word used by
  nothing else), so successive loads read the two places in turn; (3)
  `fmt_sprintf(path, "%s%s", "sim:\LUNA\MASTER\", name)` (`0x80021714`;
  `g_FileHostPathFormat`, `g_FileHostDirectory`) into a 64-byte stack
  buffer, unbounded; (4) `pc_load(path, dst, offset, len)` (`0x80021584`),
  the host read, over what (1) left. What (1) and (2) are for is not in the
  code.

It returns its last call's answer (`cd_load`'s, or `pc_load`'s masked
length). No frame wait of its own, no RNG draw.

### The development host's file read and console: `pc_load`, `sim_console_write`, `func_800217AC`

Three functions of cdread.c and fmt.c talk to a development host PC
through the BIOS file stubs `open` (`0x8008B3D8`), `lseek` (`0x8008B3E8`),
`read` (`0x8008B3F8`), `write` (`0x8008B408`) and `close` (`0x8008B418`).

**`s32 pc_load(u8 *name, u8 *dst, s32 offset, s32 len)` (`0x80021584`)**
is CDREAD.C's read of part of a file from the host: the twin of `cd_load`,
with the same four arguments. Its one caller is `file_load`, in host mode.

1. `g_XaPlayCount != 0` (the count `xa_read_start` (`0x80023674`) raises
   and `xa_play_stop` (`0x80023518`) clears, xa.c): `VoiceStep(0)` first,
   as `cd_sector_load` opens.
2. `fd = open(name, 1)`. On -1: `debug_printf(D_80011608, name)` (a
   Shift-JIS "file not found (%s)" line) and `assert_fail("PC open ERROR",
   "CDREAD.C", 0x2AC)` (`0x8001B050`). Both are empty in this build, so
   execution goes on with `fd = -1`.
3. `len &= 0xFFFFFF`; the masked length is the return value.
4. `offset != 0`: an offset that is not a multiple of 0x800 gives
   `assert_fail("lseek OFFSET error", "CDREAD.C", 0x2B1)`; then
   `lseek(fd, offset, 0)`. Offset 0 makes no call.
5. While `len != 0`: `n = len`, or 0x8000 when `len > 0x7FFF`;
   `r = read(fd, dst, n)`. `r == 0` ends the loop; `r == -1` gives
   `assert_fail("PC read ERROR", "CDREAD.C", 0x2BA)` and the loop goes on;
   then `len -= n`, `dst += n` (by the request, not by what was read).
6. `close(fd)`; return the masked length.

It reads `g_XaPlayCount` and writes no global. No frame wait, no RNG draw.

**`s32 sim_console_write(u8 *s)` (`0x80021740`)** writes a string to the
host's console, the file "sim:con" (`g_SimConsoleFileName`). It is opened
once: while the flag `g_SimConsoleOpen` (0 in the image) is 0,
`open(g_SimConsoleFileName, 2)` and its answer goes to
`g_SimConsoleDescriptor` (-1 in the image). On -1 it returns -1 and the
flag stays 0, so the next call tries again; otherwise `g_SimConsoleOpen =
1`. Then `write(g_SimConsoleDescriptor, s, strlen(s))` (`strlen`
(`0x8008B848`)), whose answer is returned.

**`s32 func_800217AC(u8 *s)`** is the same write behind a test: when
`unk_0b` (+0xB, signed) of the block `g_FileHostBlock` points at is 5 it
does nothing and returns 5; otherwise it is `sim_console_write` word for
word. What 5 there means is not known. The C writes `return 5;` there,
and says in its comment that a bare `return;` gives the same bytes.

Nothing in the program calls either console write: no `jal`, no `j` and no
data word holds 0x80021740 or 0x800217AC. Their entry addresses rest on the
bodies: each loads the global it tests one instruction ahead of its
prologue.

## Inflate's table builder: `huft_build` (`0x800222C4`)

`huft_build` is the table builder of gzip's `inflate.c`, called three times
by the 16-bit-word inflate `inflate_block_dynamic` (`0x80021DD8`) with the
seven arguments `(b, n, s, d, e, t, m)`. It counts the code lengths, finds
the minimum and maximum, builds the bit-offset table, sorts the values by
length, then lays down multi-level tables of 8-byte `Huft` entries (`e`,
`b`, `v`; include/inflate.h). It returns 1 for an incomplete set unless
`g == 1`. The one change from gzip: `malloc` is a bump arena.
`g_HuftPoolNext` advances `z + 1` entries per table, `g_Hufts` counts the
entries (gzip's `hufts`), and `inflate_buffer` (`0x80021D6C`) and
`huft_pool_reset` (`0x8002282C`) put the arena back to `g_HuftPool`.

## XMALLOC.C's permanent allocator: `xmalloc_top_alloc` (`0x80022E1C`)

`s32 xmalloc_top_alloc(s32 size)` takes `size` bytes, rounded up to a
multiple of 4, off the top of the free gap and returns the new top, which
is the block's address. Nothing gives a block back. The source file is
XMALLOC.C by its own assert string.

| Symbol | Meaning |
|---|---|
| `g_XmallocTop` | the top of the free gap: an address that moves down |
| `g_XmallocTopStart` | its start and reset value, 0x80156000, set by `xmalloc_init` (`0x80022DBC`); `xmalloc_top_reset` (`0x80022D98`) puts the top back there and `xmalloc_top_set(top)` (`0x80022D8C`) sets it directly |
| `g_XmallocHeapStart` | where the malloc2 heap starts (the end of the program's bss, rounded up to 16); it grows up (`xmalloc_heap_reset` (`0x80022E9C`) is `InitHeap2(g_XmallocHeapStart, g_XmallocLast - g_XmallocHeapStart)`) |
| `g_XmallocLast` | the heap's nominal end, `g_XmallocHeapStart + 0x40000` |
| `D_800E55C0` | malloc2's `_max_heap` |

`g_XmallocTop -= size`. Then, only when the new top is below
`g_XmallocLast` (an unsigned compare):

* `xmalloc_heap_find_end()` (`0x80022FB8`) walks the heap's block headers
  to its end marker and returns that address. A top below it means the
  block just taken overlaps what malloc2 has handed out, and
  `assert_fail("MEMORY STOP!", "XMALLOC.C", 146)` is called. It is empty in
  this build, so the function goes on.
* `xmalloc_heap_set_limit(top - 0x100)` (`0x80022FA8`) stores
  `D_800E55C0`: the heap may from now on grow to 0x100 bytes short of the
  top.

A top at or above `g_XmallocLast` leaves `_max_heap` alone. No frame wait,
no RNG draw.

Callers: the five calls of `main_buffer_init` (`0x80013E04`, main.c):
0x15828 → `g_ResourceCommand01Buffer`, 0x8000 → `g_BgDataBuffer`, 0x4000 →
`g_BgSecondBuffer`, 0xA000 → `g_BgAttributeDataBuffer`, and a last call
with size 0, which allocates nothing and only runs the limit step.

## The memory card

### The directory and the free blocks: `card_dir_read` (`0x80024F70`)

`s32 card_dir_read(void)` reads the card's directory and answers how many
8 KB blocks are free, or -1. One caller: the save and load list's card read
`MenuCardRead` (`0x80072C1C`, menu-ui.md), which keeps the answer in
`g_MenuCardFreeBlockCount`.

1. `card_accept_wait()` (`0x80024F04`): a pending accept is settled
   (`card_accept_finish` (`0x80024ED8`)), libmcrd's `MemCardAccept`
   (`0x800A1D2C`) on the port `g_CardPort`, and its result waited for
   (`card_command_wait(2)` (`0x80024E78`), a loop on `MemCardSync`) into
   `g_CardAcceptResult` and the status `g_CardResult`. Not 0: return -1.
2. `card_accept_finish()` once more, then `g_CardAcceptPreviousResult =
   g_CardAcceptResult`: the accept result `card_change_check`
   (`0x80025638`) compares the next one with.
3. `g_CardResult = MemCardGetDirentry(port, "*.*", g_CardDirEntries,
   &g_CardDirEntryCount, 0, 15)` (`0x800A2AA0`; the pattern is the 4 bytes
   at `g_CardDirPattern`). Not 0: return -1.
4. `g_CardUsedBlockCount` = the sum over the entries of `(size + 0x1FFF) /
   0x2000` (signed); the answer is `15 - g_CardUsedBlockCount`. No frame
   wait of its own (`card_command_wait` spins on `MemCardSync`), no RNG
   draw.

| Symbol | Type | Meaning |
|---|---|---|
| `g_CardDirEntries` | `struct DIRENTRY [15]` (0x28 bytes each, 0x258 in all, up to `g_PrintmesBuffer`) | the card's directory: name[20] +0x00, attr +0x14, size in bytes +0x18, next +0x1C, head +0x20, system[4] +0x24 |
| `g_CardDirEntryCount` | s32 | files in the table (written by `MemCardGetDirentry`) |
| `g_CardUsedBlockCount` | s32 | blocks those files take |
| `g_CardResult` | s32 | the last card call's libmcrd result (0 none, 1 no card, 2 invalid, 3 new card, 4 not formatted) |

### Is a file on the card: `card_dir_find` (`0x80025040`)

`s32 card_dir_find(char *name)` looks a file name up in the table
`card_dir_read` left: for each of the `g_CardDirEntryCount` entries it
compares with `strcmp` (`0x8008B788`). It answers 0 when an entry has
exactly this name and -1 when none has (also for an empty table). It does
not read the card and writes nothing; the count is read again on every
pass.

Callers, both in card.c:

- `card_save_write(slot, buf)` (`0x800252B0`), the save, called by the save
  list (menu_card.c): the name is `BASLUS-00628LUNAR` and three digits of
  `slot` (template `g_CardSaveFileName`); when `card_dir_find` answers not
  0 the file is made with 1 block (`MemCardCreateFile` (`0x800A2E2C`));
  then `sizeof(CardSave)` = 0x2000 bytes are written at offset 0
  (`MemCardWriteFile` (`0x800A2848`)), and the status
  `g_CardResult = card_command_wait(4)` is returned.
- `card_file_write(name, buf, ofs, size)` (`0x8002520C`) does the same for
  any name, with `(size + 0x1FFF) / 0x2000` blocks and the length rounded
  up to a multiple of 0x80. Nothing calls it.

So a save file is made only if the last directory read did not list its
name.

Sony's side of making the file, libmcrd's `_card_create2` (`0x800A45B8`,
libmcrd_low.c), is in
[psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md)
§"LOW.OBJ". Its C keeps the frame read and write retry loops as two
`static inline` helpers, `card_frame_read` and `card_frame_write`, beside
the `CardDir` type (include/psyq_private.h); it is built with the library
compiler ([psyq/codegen.md](psyq/codegen.md)).

## Text output

`printf` (`0x80021700`, fmt.c), which Sony's libraries call by name, has an
empty body in this build, and the game's `debug_printf` (`0x800216EC`)
returns 0 doing nothing. So no debug line is printed anywhere, libcd's
included.

### The game's own vsprintf: `fmt_vsprintf` (`0x80021B90`)

`s32 fmt_vsprintf(s8 *dst, s8 *fmt, s32 *args)` formats `fmt` into `dst`,
reading each argument as one word from `args`, and returns the number of
bytes written including the closing 0. The write position is the global
`g_FmtWritePosition` (`s8 *`), set to `dst` on entry; the helpers above it
in fmt.c append through the same pointer.

Per format character:

* `%`: `width = 1`; the pad character is `' '`, or `'0'` when the character
  right after the `%` is `'0'`; `fmt_read_width(position after '%',
  &width)` (`0x80021848`) reads an optional width and returns the position
  of the conversion letter. Then by letter: `d` and `D` →
  `fmt_put_number(arg, width, 10, letter, pad)` (`0x80021908`); `x` and `X`
  → the same with base 16 (`'x'` selects lower-case digits there); `s` →
  `fmt_put_string((s8 *)arg, width)` (`0x80021AA0`); `c` →
  `fmt_put_char(arg)` (`0x80021B74`); `%` → `fmt_put_char('%')`. Any other
  letter writes nothing: the letter and its width are consumed and no
  argument is taken.
* `'\n'`: `'\r'` is written first, then the `'\n'`.
* anything else is copied.

At the end a 0 is written and the pointer stepped past it; the result is
`g_FmtWritePosition - dst`. Callers: `fmt_sprintf` (below; it passes the
address of its third argument word as `args`) and `printmes` (below).
`fmt_put_char(c)` is the one-character append, called only from here; its
parameter is `s32` (the `%c` case passes the argument word as loaded).

**The width reader, `s8 *fmt_read_width(s8 *str, s32 *out)`
(`0x80021848`).** It reads and writes no global.

* An optional leading `'-'` is skipped and makes the sign -1.
* `fmt_digit_value(c)` (`0x8002182C`) classifies one character: `c - '0'`
  for a digit, -1 otherwise.
* If the next character is not a digit the function returns its position
  and stores nothing: the caller's `width = 1` stands (the `'-'` stays
  skipped).
* Otherwise the digit run is accumulated (`n = n * 10; n += d;`),
  `*out = n * sign`, and the position of the first non-digit (the
  conversion letter) is returned. The first digit is classified twice, once
  by the guard and once by the loop.
* The `'0'` pad flag is not skipped by the caller, so it is read here as
  the width's leading digit ("%08d" → 8). A negative width means left
  justification for `%s` only (`fmt_put_string` pads after the string);
  `fmt_put_number` pads only while `digits < width`, so "%-5d" is not
  padded at all.

**The string conversion, `void fmt_put_string(s8 *str, s32 width)`
(`0x80021AA0`).**

* `len = strlen(str)`.
* `len < width`: `width - len` spaces first (right-justified).
* then the string, without its closing 0;
* `width < 0` and `len < -width`: `-width - len` spaces after it
  (left-justified). This is the only conversion that honours a negative
  width.
* It never truncates. The pad is always a space: the `'0'` flag reaches
  only the number conversion.
* The default width is 1, so `%s` of an empty string writes one space.

**The integer conversion, `fmt_put_number(n, width, base, conv, pad)`
(`0x80021908`).** It is this printf's `%d` and `%x`, not a decompressor,
whatever its place beside the inflate code suggests.

* Digits are made least significant first into the scratch buffer
  `g_FmtNumberDigits` by an unsigned divide loop, each through
  `fmt_digit_char(n % base, letter)` (`0x800218F0`); the letter offset is
  0x27 (lower case) when `conv == 'x'`, else 7 (upper case). So `%X`, and
  `%d`, which never needs it, take the upper-case path.
* Only base 10 is signed. A negative number with the pad `'0'` gets its
  `'-'` written first and is then padded while `digits + 1 < width`; with
  any other pad the `'-'` joins the digits, after the padding.
* Left padding only (no `-` flag, no precision); then the buffer is
  written backwards through `g_FmtWritePosition`.
* Two things the bytes need that say something about the source: the
  function has an `int` return type and no `return` statement (as an
  old-style definition with no declared type would; as `void` one delay
  slot differs), and the output loop reads each byte through an `int`
  local (the sign-extending `lb`).

### The game's own sprintf: `fmt_sprintf` (`0x80021714`)

`fmt_sprintf(buf, fmt, ...)` passes `buf`, `fmt` and the address of its
third argument word to `fmt_vsprintf` and returns that function's result.
The conversions therefore read the caller's argument words in place: words
3 and 4 from the homes the prologue fills from `$a2` and `$a3`, word 5 on
from the caller's outgoing argument area. It reads and writes no global
itself. No frame wait, no RNG draw.

Callers: `file_load` (cdread.c, four arguments), `card_save_title_set`
(`0x8001BF50`, outline.c, the save title, seven arguments),
`MenuCardListDisp` (`0x800734F0`, menu_card.c), `overlay_movie_search`
(`0x8002311C`, overlay.c) and `CellNumberSet` (`0x80057400`, cell_text.c).
None reads the result. As callers see it the signature is `s32
fmt_sprintf(s8 *buf, s8 *fmt, ...)`; menu_card.c and outline.c declare
`(s8 *, u8 *, ...)`, cell_text.c `(s8 *, u8 *, s32)` (the same caller
bytes), and overlay.c calls it unprototyped.

The definition is the `<varargs.h>` form, as `debug_printf` two functions
above it: `(buf, fmt, va_alist)` with `va_alist` the last declared
parameter and `va_start` taking its address. The compiler's prologue
stores every argument register from the last declared parameter's on to
its home before the frame is opened: the two stores ahead of `addiu $sp`
(`$a2`, `$a3`). The third store of `$a2`, after the frame and to the same
word, is the body homing a parameter whose address is taken. The stdarg
form with `__builtin_next_arg(fmt)` homes `$a1` too and has no third store;
`(s8 *buf, s8 *fmt, s32 arg, ...)` passing `&arg` gives the same
instructions as the varargs form, so the bytes do not tell those two apart.
The C writes the header's macros out as their expansions
(`int __builtin_va_alist;`, `&__builtin_va_alist`), the tree having no
varargs.h, and marks that spelling in its comment.

### The printf onto the cell grid: `printmes` (`0x80025AC4`)

`void printmes(u32 x, u32 y, s8 *fmt, ...)` is ANKPRINT.C's printf onto
the 40 x 28 cell grid (menu-ui.md, "The cell grid"). The developers' name
comes from its own assert text, "printmes()'s x,y parameter error"
(`D_80011898`; the file name string `D_800118BC` is "ANKPRINT.C").

Nothing in the program calls it: no `jal` or `j` reaches 0x80025AC4 and no
data word holds it.

1. `args` = the address of the argument word after `fmt`. The prologue has
   homed `$a2` and `$a3`; from the fifth word on, the caller's outgoing
   area is read in place.
2. Unless `x < 0x28 && y < 0x1D` (unsigned): `assert_fail(D_80011898,
   D_800118BC, 0x10F)`, the report with line 271. It is empty in this
   build and is not a return: the print goes on with the same x and y.
3. `fmt_vsprintf(g_PrintmesBuffer, fmt, args)`. The buffer is 0x40 bytes
   (it ends where the grid `g_AnkprintGrid` starts); no length is passed or
   tested.
4. For each byte of the buffer up to the 0: `ankprint_cell_set(x, y, ch)`
   (`0x80025864`), x one further each time. That is cell (x, y) = `ch` and
   `g_AnkprintRowMarks[y] = 1` (row dirty). A cell below 0x100 is kind 0,
   the 8 x 8 glyph `packet_sprite_add` (`0x800256C0`) draws every frame. x
   is not tested again as it advances.

Read off the addresses: the bound on y admits 28, one past the grid's 28
rows, and `g_AnkprintGrid + 28 * 80` is `g_AnkprintRowMarks`, the dirty-row
bytes. No frame wait, no RNG draw, no result.

The definition is a stdarg one (`...` after `fmt`): the prologue homes the
argument registers from the last named parameter's on, so `$a2` and `$a3`
are stored before the frame is made. Its `va_start` takes the address of
`fmt`, which is why `fmt` is stored a second time and loaded from there for
the call. The `<varargs.h>` form `fmt_sprintf` has would home `$a3` alone
here, one instruction line short.

## Straight to the frame buffer and the packets

### The full-screen picture put: `screen_picture_put` (`0x80024C04`)

`void screen_picture_put(void)` draws the picture in the load buffer
straight into the frame buffer on show. Three calls: `cd_change_wait`
(`0x80024D2C`, the disc check's "insert disc" screen) and `GameRun`
(`0x800559F4`) twice, after the field loop ends (entries 4 and 5 of
resource 599). Each caller first inflates the picture with
`resource_tim_unpack(n)` (`0x80020918`) into the buffer `g_LoadBuffer`
points at, and calls `screen_disp_mask_set(1)` (`0x8001EFEC`) and
`SetDispMask(1)` afterwards.

- The buffer is read as a TIM with one 256-colour CLUT, at two fixed
  offsets and no header field: `+0x14` the 256 `u16` colours, `+0x220` the
  pixels, one byte each, 320 to a row, 224 rows (the size is in the code).
- `rect` = `g_ScreenPictureRowRectangle`, the constant `RECT {0, 0, 320,
  1}` at 0x800B8758, between `g_PictureVramRectangle` (`RECT {320, 0, 160,
  480}`, `PictureVramSave` (`0x800247F4`)'s save rectangle) and
  `g_CdChangeMode`; `buf` = `GsGetActiveBuff()` (`0x8008F048`, the draw
  buffer's index), read once.
- For y = 0..223: `rect.y` = y, or y + 224 when `buf` is 0. The buffers
  are at (0, 0) and (0, 224) (`GsDefDispBuff(0, 0, 0, 0xE0)`), so the row
  lands in the buffer that is not being drawn. The row's 320 bytes are
  looked up in the CLUT into a `u16[320]` on the stack; `LoadImage(&rect,
  line)`; `DrawSync(0)`.
- 224 one-row loads, each waited out. No frame wait, no RNG draw, no
  result.

### The tinted sprite: `packet_sprite_col_add` (`0x80025754`)

`void packet_sprite_col_add(s32 x, s32 y, s32 u, s32 v, s32 w, s32 h, s32
tpage, s32 clut, s32 z, s32 color)` queues one textured sprite modulated by
`color`. It is `packet_sprite_add` (colour fixed at 0x808080) with the
colour as a tenth argument. Its one caller is the cell grid's icon drawer
`IconCellDraw` (`0x8001AD60`, kind 3): 24 x 24, tpage 0x92, CLUT 0x7D80,
entry 0x20, colour 0x404040 for a dimmed cell and 0x808080 otherwise.

One 0x18-byte packet at the packet cursor `g_PacketWritePointer` (a
draw-mode word and a SPRT body as one primitive of length 5,
`PacketSprite`):

| Offset | Field | Value |
|---|---|---|
| +0x00 | tag | old link of entry z `\| 0x05000000` |
| +0x04 | draw mode | `tpage \| 0xE1000200` (GP0 E1h, dither on) |
| +0x08 | r0 g0 b0 code | `color \| 0x64000000` (GP0 64h, free-size textured rectangle) |
| +0x0C, +0x0E | x0, y0 (s16) | x, y |
| +0x10, +0x11, +0x12 | u0, v0 (u8), clut (u16) | u, v, clut |
| +0x14, +0x16 | w, h (s16) | w, h |

Entry z of the ordering table `g_PacketOtBase` then gets the packet's
address + 0x80000000 (its 24-bit link) by hand, not through `AddPrim`, as
`packet_tpage_add` (`0x8002566C`) and `packet_sprite_add` do; the cursor
moves on 0x18. Nothing is clamped.

### An 8 x 8 sprite nothing draws: `packet_sprite8_add` (`0x800257EC`)

`void packet_sprite8_add(s32 x, s32 y, s32 u, s32 v, s32 clut, s32 z)`
queues one 8 x 8 textured sprite: a libgpu `SPRT_8` (0x10 bytes) at
`g_PacketWritePointer`, linked at entry z of `g_PacketOtBase` with
`AddPrim` (`0x8008E9D8`); the cursor moves on 0x10 after the call. No
draw-mode word: it draws from the texture page that is current.

| Offset | Field | Value |
|---|---|---|
| +0x00 | tag | 0x03000000 (length 3, link clear; `AddPrim` fills the link) |
| +0x04 | r0 g0 b0 code | 0x74808080 (GP0 74h, 8 x 8 textured rectangle; colour 0x808080) |
| +0x08, +0x0A | x0, y0 (s16) | x, y |
| +0x0C, +0x0D | u0, v0 (u8) | u, v |
| +0x0E | clut (u16) | clut |

Nothing calls it: no `jal`, no word holding the address and no `lui` /
`addiu` pair forming it. The 8 x 8 glyphs of the cell grid go through
`packet_sprite_add` (a SPRT with w = h = 8 and its own draw-mode word)
instead. The parameter count (six) is proven by the body; the parameter
widths are not, and there is no call to read them from.

## Controller input

Sony's libpad serves the two ports at each vertical blank and leaves each
controller's reply in a 0x40-byte buffer of the game's
(`g_PadPort00Buffer`, `g_PadPort10Buffer`, handed to `PadInitDirect` by
`pad_start` (`0x8002003C`)). Its members, how it serves the ports and the
port record are in [psyq/libpad.md](psyq/libpad.md). The game's side reads
those buffers once a frame.

### Reading the controllers: `pad_update` (`0x8001F508`)

`void pad_update(void)` (pad_update.c) is called through `pad_frame_step`
(`0x8001EE98`, frame.c) by the present `frame_end` and by the disc wait
in cd_change.c; the movie overlay calls it too (video.md). It first steps
the vibration script and its two motors (battle.md §"The vibration scripts
— `g_PadActuatorList`"; its random way draws numbers: frame-rng.md
§"Draw site 2 — the vibration driver `pad_update` (`0x8001F508`)"), zeroes
both motor levels `g_PadActuatorLevels` while option byte 5 of
`g_GameOptions` (the vibration switch, which `BtlFrameStep` flips) is 0,
and then reads the ports: port 0x10 first, port 0x00 second, each through
its own buffer (`g_PadPortBuffer` points at the one being read).
`g_PadButtons` is 0 at the start of the frame.

For each port it asks `PadGetState(port)` (`0x800A4DA4`). State 0 (no
controller): on port 0x10 the word stays 0; on port 0x00 nothing is done.
Otherwise `PadInfoMode(port, 1, 0)` (`0x800A4E70`), the controller's
current ID, decides what is read:

| ID | What is read |
|---|---|
| 4 | buffer bytes 2 and 3, the buttons |
| 5 | bytes 6 and 7 alone, the stick, applied to the word as it stands (below) |
| 7 | bytes 2 and 3, then bytes 6 and 7; and the actuators |
| any other | nothing: the port is ignored |

4 is the digital pad, 5 the analog joystick and 7 the analog controller in
analog mode, as the controller IDs are generally documented. For ID 4, 5
and 7 it also stores 0 at `D_800E0AE8[index × 0xA0]` (index 1 for port
0x10, 0 for port 0x00), a byte nothing reads.

The button bytes are active low. The word is `byte 3 | byte 2 << 8`; a word
of 0xFFFF (no button down) leaves `g_PadButtons` as it is, and any other
word becomes `g_PadButtons = word ^ 0xFFFF`. So both ports drive the game:
port 0x00's buttons replace port 0x10's whenever one of them is down, and
port 0x10's stand while port 0x00 has none down.

The stick: byte 6 below 0x50 presses left (clears bit 0x8000 of the
active-low word), 0xB1 or more right (0x2000); byte 7 below 0x50 up
(0x1000), 0xB1 or more down (0x4000). 0x50..0xB0 on an axis is the dead
zone. For ID 5 the C reads no button bytes: the stick bits are cleared
from whatever the word holds, port 0x10's word on port 0x00's pass (0xFFFF
when port 0x10 is empty), and on port 0x10's own pass a value nothing set
(the C's local is unset there; in the original it is register `$s3` as the
caller left it).

The actuators, for ID 7 alone: state 1 (`PadStateFindPad`) clears the
port's byte of `g_PadActuatorAlignedFlags`. While that byte is 0,
`PadSetAct(port, g_PadActuatorLevels, 2)` (`0x800A5164`), and then state 2
(`PadStateFindCTP1`) sets the byte, and state 6 (`PadStateStable`) calls
`PadSetActAlign(port, g_PadActuatorAlignBytes)` (`0x800A50E4`) and sets the
byte when that answers not 0. Then `PadSetAct(port, g_PadActuatorLevels,
2)` once more, every frame. A controller of ID 4 or 5 gets no actuator call
from the game.

After both ports: `D_800B89FC` counts the frames with no button down (0
when `g_PadButtons` is not 0; nothing reads it), `g_PadPreviousButtons`
keeps the word, and `g_PadPressedButtons` = the bits down now that were not
down the frame before. `pad_buttons_get` (`0x80020030`) returns
`g_PadButtons`; the pad records and the field's poll below read it.

The word's bits, as the controller's two button bytes are generally
documented (the d-pad's four are also what the stick sets above):

| Bit | Button | Bit | Button |
|---|---|---|---|
| 0x0001 | L2 | 0x0100 | SELECT |
| 0x0002 | R2 | 0x0200 | L3 |
| 0x0004 | L1 | 0x0400 | R3 |
| 0x0008 | R1 | 0x0800 | START |
| 0x0010 | TRIANGLE | 0x1000 | up |
| 0x0020 | CIRCLE | 0x2000 | right |
| 0x0040 | CROSS | 0x4000 | down |
| 0x0080 | SQUARE | 0x8000 | left |

### The pad record: `pad_record_update` (`0x80026930`)

`void pad_record_update(PadRecord *rec, u8 delay)` refreshes one
caller-owned pad record; it is the game's edge and auto-repeat layer over
the button word. One call per poll:

1. `pad = pad_buttons_get()`.
2. `rec->held_before = rec->held; rec->held = pad; g_PadHeldButtons = pad`.
3. `rec->held_before_2 = rec->held_2; rec->held_2 = g_PadSecondHeldButtons`.
4. `pressed = held & (held ^ held_before)`, `pressed_2` likewise, both
   repeat words 0.
5. For each bit 0..15 of each word: not held, its timer = 0; just went
   down, timer = `delay` and the repeat bit is set; still held with a
   non-zero timer, timer--; still held at 0, timer = `delay >> 1` and the
   repeat bit is set.

So `repeated` is the typematic word: set on the press, again after
`delay + 1` calls, then every `delay / 2 + 1`. With `delay` 0 it equals
`held`.

| Offset | Field | Meaning |
|---|---|---|
| +0x00 | `u16 held` | held now |
| +0x02 | `u16 pressed` | went down this call |
| +0x04 | `u16 repeated` | went down, or auto-repeats |
| +0x06 | `u16 held_before` | held at the previous call |
| +0x08..+0x0E | `held_2`, `pressed_2`, `repeated_2`, `held_before_2` | the same four for `g_PadSecondHeldButtons` |
| +0x10 | `u8 timers[16]` | per bit, calls until the next repeat |
| +0x20 | `u8 timers_2[16]` | the same for the second word |

The record (`PadRecord`, include/pad.h) is 0x30 bytes. The callers' records:
`g_BtlMenuPadRecord` (delay 0x10; shop_screen.c twice, btl_command.c),
`g_MenuPadRecord` (0x20 in btl_command.c, 0x14 in the menu files) and
`g_BtlPadRecord` (0; btl_slot.c, btl_move.c, start_screen.c); the script's
and the message window's records take 0x14 (`g_MessagePadRecord`, text.md).
20 calls in all, none reading a return value.

`g_PadHeldButtons` (the mirror of the word) is also written by
`MapPadStep` (below) and zeroed in btl_init.c, btl_menu.c and
shop_screen.c. `g_PadSecondHeldButtons` is read only here and otherwise
only zeroed in those three files, so by symbol references the second
word's half is dead; a write through a pointer was not ruled out.

`void pad_record_init(PadRecord *rec, u8 delay, u16 held, u16 held2)`
(`0x8002687C`) starts a record before `pad_record_update` polls it: it
zeroes the 0x30 bytes with `mem_clear` (`0x8002609C`), sets `held` and
`held_2`, and gives every bit set in `held` or `held2` a timer of `delay >>
1`, so a button still down from the previous screen is not a new press.
All 16 calls pass `held2 = 0`, and every call passes constants.

### The field's pad poll: `MapPadStep` (`0x80055EC4`)

`s32 MapPadStep(void)` is called once per pass by the field's frame step
`MapPartyStep` (`0x80056248`) (field-frame.md §"One field frame"). It works
on the button word, not on a pad record, and returns a direction:

1. `pad = pad_buttons_get()`; `prev = g_MapPadHeldButtons`; then
   `g_PadHeldButtons = pad`, `g_MapPadHeldButtons = pad`,
   `g_MapPadPreviousHeldButtons = prev`, `g_MapPadPressedButtons = pad &
   (prev ^ pad)` (newly pressed).
2. The d-pad nibble `pad & 0xF000` becomes an index: 0x1000 → 1, 0x3000 →
   2, 0x2000 → 3, 0x6000 → 4, 0x4000 → 5, 0xC000 → 6, 0x8000 → 7, 0x9000 →
   8, anything else 0. The return value is the u16
   `g_MapPadDirections[index]`, a table `MapPadDirectionsSet`
   (`0x80066F04`, map_walk.c) fills from one of six rows
   (npc-scripting.md §"The field's d-pad map"); `MapAttributeFind`
   (`0x80069A20`, map_over.c) turns entries 3 and 7 into diagonals for an
   attribute of kind 11. `MapPartyStep` hands a non-zero return to
   `MapPartyWalkStep` (`0x800664D4`) up to `g_MapPartyWalkStepCount` times,
   stopping at the first call that returns 0.
3. The idle count `g_MapIdleCount`. With a zero return it counts up to 300
   (0x12C) and sets `g_MapStatusRequest = 1` on reaching it. With a
   non-zero return and a non-zero count it is reset to 0 and
   `g_MapStatusRequest = 2`, unless `g_MapPartyHpTakeOn` is set and
   `g_MapPartyHpTakeWait` is 0xF or more.
4. The HP take (an attribute of kind 10 under the lead). While
   `g_MapPartyHpTakeOn` is set: with `g_MapPartyHpTakeWait` at 0,
   `MapPartyHpTake()` (`0x80059E84`, which sets the wait to 0x1E), the
   count forced to 300 and `g_MapStatusRequest = 1`; with a wait below
   0x1D, `layer_col_set(0, g_MapLayersMask | 0x100, &g_MapCol)`
   (`0x8001EB74`), the map's colour put back at once; either way the wait
   then goes down by 1 (so 0x1D right after a take). With
   `g_MapPartyHpTakeOn` clear and the wait not 0, the wait is zeroed and
   the same `layer_col_set` call made once.
5. The box of script command 0x56. While `g_MapPadDirectionsState` is not
   0, the lead's s16 words `+6` and `+8` (record 0 of `g_MapPartyRecords`)
   are tested against `g_MapPadDirectionsBoxLeft <= +6 <
   g_MapPadDirectionsBoxRight`, `g_MapPadDirectionsBoxTop <= +8 <
   g_MapPadDirectionsBoxBottom`, and nothing more happens while
   `layer_col_fade_check(0) & 0xFF` (`0x8001EDDC`) is not 0. Outside the
   box: `MapPadDirectionsSet(0)` (the d-pad answering what was pressed), the
   `layer_col_set` call above, `g_MapPadDirectionsState = 0`. Inside:
   `g_MapPadDirectionsState` flips between 1 and 2 and
   `layer_col_fade_start(0, g_MapLayersMask | 0x100, 1, 0x18, p)`
   (`0x8001EA74`) runs, with `p` the three s16 numbers of
   `g_MapPadDirectionsCol` (0x30, 0x18, 0x30) going 1 → 2 and `&g_MapCol`
   going back to 1.

Script command 0x56, `ScriptCommandMapPadDirectionsChange` (`0x8005EB98`,
lns_intp.c), gives `MapPadDirectionsSet` a random row 1..4 (a turned or
mirrored d-pad), sets `g_MapPadDirectionsState = 1` and stores the square
of half-side `radius byte * 8` around the lead's +6 and +8 words in the
four box words. These are layer colour fades, not windows (the "Window
ramps (ui)" item of battle.md; field-frame.md §"There is no fade layer:
four layer records"): `layer_col_fade_check` answers whether a ramp of the
four layer records still runs and `layer_col_fade_start` starts one. So
while the lead stands in the box, step 5 alternates 24-frame ramps of every
layer between `g_MapPadDirectionsCol` and the map's colour, each once the
one before has finished; leaving the box puts the colour and the d-pad back.

## libcd's interrupt service: `func_80090A68`

`func_80090A68` is a static of libcd's BIOS.OBJ (libcd_bios_1.c), named for
its address: no Sony text names it. It is the CD-ROM interrupt service.
libcd's interrupt callback `func_800920F8` (libcd_bios_3.c), which
`CD_initintr` and `CD_init` install with `InterruptCallback(2, ...)`, calls
it until it returns 0, and calls the ready and the sync callbacks by the
bits of its answer (psyq/libcd.md).

The cause is reg3 & 7, read again until two reads agree; 0 returns 0 at
once. While reg0 bit 5 is set the response FIFO (reg1) is drained into an
8-byte `volatile` result, zero-padded. Then reg0 = 1, reg3 = reg2 = 7.
Unless the cause is an Acknowledge (3) of a command whose `D_800B651C` flag
is clear, the drive status `D_800B6364` and `D_800B6368` come from result[0]
and result[1], a lid-open bit (0x10) newly set counts in `D_800B636C`, and
the error is `status & 0x1D`. For a disk error (cause 5) at debug level
`D_800B6360` of 3 or more it calls `printf` with "DiskError:
com=%s,code=(%02x:%02x)", which prints nothing in this build (above). Per
cause it sets the sync, ready or end state byte `D_800B6634[0..2]` (2
complete, 3 acknowledge, 1 data ready, 4 data end, 5 error) and copies the
result (an inlined copy) into `D_800E7108`, `D_800E7110` or `D_800E7118`;
the answer is the case's code, with 0 also for an unknown cause. It is
built with the library compiler and the assembler marks of
[psyq/codegen.md](psyq/codegen.md) §"The library's assembler"
(`reorder_fill` among them).
