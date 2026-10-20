# PS1 BIOS call sites

Where the main executable (`SLUS_006.28`) calls the PlayStation's BIOS. The
short answer: the game's own code never does; Sony's libraries do, through
small stubs.

## The game's own code makes none

The game's own code, `0x80013D08` to `0x8008B387`, is C throughout, and
GCC emits neither a jump through a register loaded with a constant nor a
`syscall` (`config/bios_thunks.txt`; the tree has no inline assembly). It
reaches the BIOS only by calling Sony's stubs below, and calls few of them
itself:

* `open`, `lseek`, `read` and `close`: the development host's file read
  `pc_load` (`0x80021584`, cdread.c);
* `open` and `write`: the host console writes `sim_console_write`
  (`0x80021740`) and `func_800217AC` (fmt.c), which nothing calls;
* `_bu_init`: the memory card's start-up `card_init` (`0x80024E3C`, card.c),
  after libmcrd's `MemCardInit` and `MemCardStart`;
* `EnterCriticalSection`, `FlushCache`, `ExitCriticalSection`: the overlay
  load `overlay_load` (`0x80023240`, overlay.c), after it has read the
  overlay to `0x80196000` and checked its build stamp;
* `Krom2RawAdd`: the menu font's glyph lookup `font_pattern_get`
  (`0x80026B1C`, font_string.c), for a character in neither of the game's
  own glyph tables: the BIOS ROM's 16 x 16 pattern for that Shift-JIS code,
  or a blank one when the ROM has none.

Everything else reaches the BIOS through the libraries' own functions
(io-hardware.md, and the SDK pages below).

## Sony's libraries: 39 stubs and two `syscall` routines

Each stub is three instructions, `addiu $t2, $zero, <table>`, `jr $t2`,
`addiu $t1, $zero, <index>`: a jump into one of the BIOS's three function
tables at 0xA0, 0xB0 and 0xC0, with the function's number in `$t1`. The
table and number of each are measured on the image
(`config/bios_thunks.txt`); the name is the library member's, proven by its
bytes (the register). All stay `INCLUDE_ASM`: C cannot spell them
([hand-assembly.md](../hand-assembly.md)).

| Address | Name | BIOS table (number) | Library member |
|---|---|---|---|
| `0x8008B388` | `InitHeap` | A0 (0x39) | libapi `c57.o` |
| `0x8008B398` | `FlushCache` | A0 (0x44) | libapi `c68.o` |
| `0x8008B3A8` | `_bu_init` | A0 (0x70) | libapi `c112.o` |
| `0x8008B3D8` | `open` | B0 (0x32) | libapi `a50.o` |
| `0x8008B3E8` | `lseek` | B0 (0x33) | libapi `a51.o` |
| `0x8008B3F8` | `read` | B0 (0x34) | libapi `a52.o` |
| `0x8008B408` | `write` | B0 (0x35) | libapi `a53.o` |
| `0x8008B418` | `close` | B0 (0x36) | libapi `a54.o` |
| `0x8008B428` | `Krom2RawAdd` | B0 (0x51) | libapi `a81.o` |
| `0x8008E7B8` | `GPU_cw` | A0 (0x49) | libapi `c73.o` |
| `0x80090448` | `DeliverEvent` | B0 (0x07) | libapi `a07.o` |
| `0x80094BB8` | `ChangeClearPAD` | B0 (0x5B) | libapi `a91.o` |
| `0x80094BC8` | `ChangeClearRCnt` | C0 (0x0A) | libapi `l10.o` |
| `0x800952A0` | `_96_remove` | A0 (0x72) | libapi `c114.o` |
| `0x800952B8` | `ReturnFromException` | B0 (0x17) | libapi `a23.o` |
| `0x800952C8` | `ResetEntryInt` | B0 (0x18) | libapi `a24.o` |
| `0x800952D8` | `HookEntryInt` | B0 (0x19) | libapi `a25.o` |
| `0x8009DE58` | `OpenEvent` | B0 (0x08) | libapi `a08.o` |
| `0x8009DE68` | `EnableEvent` | B0 (0x0C) | libapi `a12.o` |
| `0x800A01C8` | `WaitEvent` | B0 (0x0A) | libapi `a10.o` |
| `0x800A0538` | `TestEvent` | B0 (0x0B) | libapi `a11.o` |
| `0x800A14B8` | `InitPAD2` | B0 (0x12) | libapi `a18.o` |
| `0x800A14C8` | `StartPAD2` | B0 (0x13) | libapi `a19.o` |
| `0x800A14D8` | `StopPAD2` | B0 (0x14) | libapi `a20.o` |
| `0x800A14E8` | `PAD_init2` | B0 (0x15) | libapi `a21.o` |
| `0x800A14F8` | `SysEnqIntRP` | C0 (0x02) | libapi `l02.o` |
| `0x800A1508` | `SysDeqIntRP` | C0 (0x03) | libapi `l03.o` |
| `0x800A1628` | `InitCARD2` | B0 (0x4A) | libcard `a74.o` |
| `0x800A1638` | `StartCARD2` | B0 (0x4B) | libcard `a75.o` |
| `0x800A1648` | `StopCARD2` | B0 (0x4C) | libcard `a76.o` |
| `0x800A3618` | `nextfile` | B0 (0x43) | libapi `a67.o` |
| `0x800A3628` | `erase` | B0 (0x45) | libapi `a69.o` |
| `0x800A38D8` | `firstfile2` | B0 (0x42) | libapi `a66.o` |
| `0x800A3998` | `_card_info` | A0 (0xAB) | libcard `c171.o` |
| `0x800A39A8` | `_card_load` | A0 (0xAC) | libcard `c172.o` |
| `0x800A39B8` | `_card_write` | B0 (0x4E) | libcard `a78.o` |
| `0x800A39C8` | `_new_card` | B0 (0x50) | libcard `a80.o` |
| `0x800A41D8` | `CloseEvent` | B0 (0x09) | libapi `a09.o` |
| `0x800A4CE8` | `_card_read` | B0 (0x4F) | libcard `a79.o` |

`EnterCriticalSection` (`0x8008B3B8`, libapi `a36.o`) and
`ExitCriticalSection` (`0x8008B3C8`, libapi `a37.o`) enter the kernel with
a `syscall` instead ([psyq/libgpu-libgs.md](psyq/libgpu-libgs.md) §"libapi —
the critical section").

libcard's PATCH.OBJ routines also call the BIOS, in line, but they are
copied into kernel RAM and run there, not called by the program
([psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md)
§"PATCH.OBJ and END.OBJ: Sony's hand assembly").

What each library function does is on the SDK pages
([psyq/index.md](psyq/index.md)).

## Before the image: the start-up and the outer loader

`__SN_ENTRY_POINT` (`0x80013C4C`), Sony's start-up code from `2mbyte.obj`
(which starts with `__main`'s eight bytes at `0x80013C44`), clears
`0x800B8820` to `0x800EAFE8`, calls `InitHeap` (`0x8008B388`) and then
`main` (`0x80013D2C`). Its bytes stay assembly (`src/2mbyte.c`).

The executable on the disc holds this image packed: the boot file's loader
at `0x80100000` (entered at `0x801001EC`) calls `InitHeap` through a stub
of its own at `0x8010036C` (A0, 0x39), unpacks the image from `0x80100800`
with an unpacker whose words are those of the image's `lz_unpack`
(`0x80022BF8`), and jumps to `__SN_ENTRY_POINT`
([bootstrap.md](bootstrap.md)). That loader is not part of the decompiled
image.
