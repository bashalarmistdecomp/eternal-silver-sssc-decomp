# libcd, with the libgs, libgte, libc2 and libetc members beside it

This page describes libcd's members in the main executable, `SLUS_006.28`:
`event.o` (`CdInit`), `toc.o` (`CdGetToc2`), `sys.o` (the public layer,
the `CdControl` family among it), `bios.o`'s sector DMA and interrupt
handler, `cdread.o`'s first two functions, the streaming members (`cdread2.o`,
`cdrom.o`, `c_002.o` to `c_011.o`) and `s_002.o` (`CdIntToPos`). It also
covers the members of other libraries that lie among them: libgs `gs_122.o`
and `gs_123.o`, libgte `msc00.o`, `reg11.o`, `reg12.o` and `patchgte.o`,
libc2 `puts.o`, `putchar.o` and `strncmp.o`, and libetc's `VSync`. Every
member named here is release 4.5's and lies whole in the image; the register
(`decomp/SLUS_006.28/config/symbol_addrs.txt`) has each proof, and
[naming.md](../../../../naming.md) says how the release was measured and the
names were proven. The rest of libcd (`iso9660.o`'s file search, `CD_cw`,
`bios.o`'s interrupt state and start-up, `cdread.o`'s state record and its
public functions) is in [psyq-libs.md](../psyq-libs.md).
[index.md](index.md) gives each function's file and recipe markers, and
[codegen.md](codegen.md) how the library code was compiled.

## The names of `sys.o`'s command and sector functions

| Address | Name | What it does |
|---|---|---|
| `0x80090740` | `CdControlF` | Issues a command and returns without waiting for it to complete. |
| `0x80090874` | `CdControlB` | Issues a command and blocks until it has completed. |
| `0x800909E0` | `CdGetSector` | Copies sector data out of the drive by DMA and waits for it (`CD_getsector`). |
| `0x80090A00` | `CdGetSector2` | Starts the same DMA and returns at once (`CD_getsector2`). |

`CdReadSync` is `0x80093794` and `CdReadCallback` `0x80093898`, both in
`cdread.o`. Release 4.5's `sys.o` defines the four names above at those
offsets, and `cdread.o`'s calls name them the same way (the register). The
first split had these four the other way: the two `CdControl` names swapped,
and `CdReadSync` and `CdReadCallback` on the two `CdGetSector` functions.

## libgs: TIM headers and the one-axis rotation matrix

Two one-function members, `gs_122.o` (0xF0 bytes, at `0x8008FC48`) and
`gs_123.o` (0xD0 bytes, at `0x8008FD38`), in `libgs_gs_122.c` and
`libgs_gs_123.c`.

* **`GsGetTimInfo(tim, im)`** (`0x8008FC48`). Reads a TIM header into a
  `GsIMAGE`. `tim` points at the TIM's flag word: the one caller,
  `tim_image_load` (`0x8002073C`, `palette_row.c`), passes the file's
  address + 4. `im->pmode` = the flag word. With the CLUT bit (bit 3) set,
  the next block is the CLUT: its byte length, then x, y, width and height as
  16-bit pairs (`cx`, `cy`, `cw`, `ch`) and the data pointer (`clut`); the
  pixel block starts that byte length after the length word. The pixel block
  has the same layout: byte length, `px`, `py`, `pw`, `ph`, then the data
  pointer (`pixel`). Without the CLUT bit the pixel block follows the flag
  word directly and the five CLUT members are not written.
* **`Gssub_make_matrix(m, s, c, axis)`** (`0x8008FD38`). A libgs routine
  with no public prototype; the parameter types are the ones the bytes fit
  (`MATRIX *`, `short`, `short`, signed `char`). `*m = GsIDMATRIX`
  (`D_800E7050`), then by the axis letter, either case:

  | Axis | Cosine cells | Sine cells |
  |---|---|---|
  | `'X'` | `m[1][1]`, `m[2][2]` | `m[1][2] = -s`, `m[2][1] = s` |
  | `'Y'` | `m[0][0]`, `m[2][2]` | `m[0][2] = s`, `m[2][0] = -s` |
  | `'Z'` | `m[0][0]`, `m[1][1]` | `m[0][1] = -s`, `m[1][0] = s` |

  Any other letter leaves the identity. The switch's jump table
  (`jtbl_800131C4`) has 35 entries, `'X'` to `'z'`. Nothing in the image
  calls the function: no `jal`, data word or address load reaches it.

## libgte: the screen offset and the exception-handler patch

These members are Sony's hand-written assembly. The tree keeps their code as
assembly (`libgte_msc00.c`, `libgte_reg11.c`, `libgte_reg12.c`,
`libgte_patchgte.c`), and [hand-assembly.md](../../hand-assembly.md) says
what in it no compiler emits.

* **`SetGeomOffset(ofx, ofy)`** (`0x8008FF78`, `reg12.o`). Shifts both
  arguments left by 16 and writes them with `ctc2` to GTE control registers
  24 and 25 (OFX, OFY): the screen offset the perspective transform adds, in
  16.16 fixed point. The member is the function and two pad words. Callers:
  `GsSetDrawBuffOffset` (`0x8008F058`) with the drawing offset, and
  `gte_init` (`0x8008FBF8`) with (0, 0).
* **`_patch_gte()`** (`0x8008FF98`, `patchgte.o`). Patches the kernel's
  exception handler. `EnterCriticalSection()` (`0x8008B3B8`); the BIOS call
  B0:0x56 in line (it returns the kernel's C0 table); entry 6 of that table
  + 0x28 bytes is the patch point. If the six words there equal the member's
  first pattern (at `0x80090044`: `sw $at`, `sw $v0`, `sw $v1`, `sw $ra`
  through `$k0`, `mfc0 $v1,$14`, `nop`), the six words of the second pattern
  (at `0x8009005C`: the same saves with `mfc0 $v0,$13` added and the `nop`
  gone) are copied over them, so the handler has the Cause register in
  `$v0`. Then `FlushCache()` (`0x8008B398`) and `ExitCriticalSection()`
  (`0x8008B3C8`). The compare stops at the first differing word, so an
  unexpected kernel is left alone. `D_800E70B0` is the member's one `.bss`
  word, where the routine keeps `$ra` (it has no stack frame). One caller:
  `InitGeom`. The member is the 43 instructions, the two patterns and one pad
  word.

  `func_80090044` is therefore not a function: its twelve words are the two
  patterns, which are only compared and copied, and nothing calls them.
  `0x80090074` is the end of the second pattern.

The rest of these members:

* **`InitGeom`** (`0x8008FE10`, `msc00.o` + 8) calls `_patch_gte`, then sets
  the coprocessor up (`mfc0` / `mtc0` of the status register, `ctc2` to the
  GTE's control registers).
* **`func_8008FE08`** is not a function: it is the first eight bytes of
  `msc00.o`, ahead of `InitGeom`. They read "Ps" and `0x00450000`, a stamp
  that carries the release number; the 4.4 and 4.6 members hold
  `0x00440000` and `0x00460000` there.
* **`SetFarColor`** (`0x8008FF58`, `reg11.o`) shifts its three arguments
  left by 4 and writes them with `ctc2` to control registers 21 to 23.

## libcd `event.o`: `CdInit` and its default callbacks

`event.o` is `0x800902F8` to `0x80090447` (0x150 bytes: five functions and
three pad words), in `libcd_event_1.c` and `libcd_event_2.c`. Only `CdInit`
has a public name:

| Offset | Address | Name | What it is |
|---|---|---|---|
| `+0x00` | `0x800902F8` | `CdInit` | `CdInit` |
| `+0x90` | `0x80090388` | `func_80090388` | The reset `CdInit` retries: `CD_init() == 0 && CD_initvol() == 0`, which is `CdReset` with the mode fixed at 1. |
| `+0xCC` | `0x800903C4` | `def_cbsync` | The default sync callback: `DeliverEvent(0xF0000003, 0x20)`. |
| `+0xF4` | `0x800903EC` | `func_800903EC` | The default ready callback: `DeliverEvent(0xF0000003, 0x40)`. |
| `+0x11C` | `0x80090414` | `func_80090414` | The default read callback: `DeliverEvent(0xF0000003, 0x40)`, the same event. |

`def_cbsync` has its name because releases 3.7 and 4.0 export its exact
bytes under that name (`sdk-earlier`). The two other callbacks keep their
addresses: those releases export their bytes, which are the same for both,
under two names, so no one name fits (the register). `DeliverEvent` is
libapi's (`0x80090448`, `a07.o`). 0xF0000003 is kernel.h's `HwCdRom`; 0x20
and 0x40 are its `EvSpCOMP` (command completed) and `EvSpDR` (data ready).

| Global | Sony's symbol | Meaning |
|---|---|---|
| `D_800B62C0` | `CD_cbread` | The read callback. Defined by `event.o` (its `.data` + 0). |
| `D_800B62C4` | `CD_read_dma_mode` | Defined by `event.o` (its `.data` + 4). |
| `D_80013294` | — | `event.o`'s `.rdata`: "CdInit: Init failed\n". |

* **`CdInit()`** (`0x800902F8`). Up to five tries of `func_80090388`. On
  the first that returns 1 it sets `CD_cbsync` (`D_800B6358`) =
  `def_cbsync`, `CD_cbready` (`D_800B635C`) = `func_800903EC`, `CD_cbread` =
  `func_80090414`, clears `CD_read_dma_mode` and returns 1. After five
  failures it prints "CdInit: Init failed" and returns 0. Callers:
  `system_init` (`0x8001EEB8`, `frame.c`) and `cd_sector_load`
  (`0x80020E88`, `cdread.c`).

## libcd `toc.o`: the table of contents

`toc.o` is `0x800900A8` to `0x800902F7` (0x250 bytes): `CdGetToc`
(`0x800900A8`) at +0, which is `return CdGetToc2(1, loc);`, `CdGetToc2`
(`0x800900CC`) at +0x24 and two pad words, in `libcd_toc_1.c` and
`libcd_toc_2.c`. Its `.rdata` is three format strings: `D_80013254`
"track=%d,%d\n", `D_80013264` "CdGetToc2: %02x:%02x:00\n", `D_80013280`
"CdGetToc2: error\n". `CdGetToc`'s one caller is `cd_toc_read`
(`0x8001B63C`, `cd_track.c`).

* **`CdGetToc2(session, loc)`** (`0x800900CC`). Reads the disc's table of
  contents into `loc[]` (`CdlLOC`: minute, second, sector, track). `session`
  is not read: a parameter byte is set to 1 and never sent. With the sync
  callback removed (`CdSyncCallback(0)`, restored on every exit):
  1. `CdlGetTN` (0x13): the first and last track numbers, BCD, in
     `result[1]` and `result[2]`.
  2. `CdlGetTD` (0x14) with parameter 0: `loc[0]` = `result[1]`,
     `result[2]`, sector 0. What the drive answers for parameter 0 is not in
     this code; by the CD command set's convention it is the end of the disc.
  3. One `CdlGetTD` per track, its number as BCD: `loc[1]`, `loc[2]`, ... the
     same way.

  Each command goes through `CdControlB` (`0x80090874`). Returns the number
  of tracks read, or 0 as soon as a command fails. With `CD_debug`
  (`D_800B6360`) at 2 or more it prints the track range and every entry; at
  any nonzero level, the error line.

## libcd `sys.o`: the public layer

`sys.o` is libcd's public layer over `bios.o`. In the image it is
`0x80090458` to `0x80090A63` (0x60C bytes of code, one word of padding after
it), 21 functions, in `libcd_sys_1.c` to `libcd_sys_7.c`. Release 4.5's
member is 1,552 bytes, and all 1,349 of them outside its relocation fields
equal the image at `0x80090458`. The 4.1 to 4.4 members have the same first
0x60C bytes and then `CdIntToPos` and `CdPosToInt`; 4.5 has those two in a
member of their own, `s_002.o`, which the image has at `0x80094818`. The
3.5, 3.7 and 4.0 members differ from +0x36 on.

The functions by offset (game address = `0x80090458` + offset), with what
each one's relocations reach:

| Offset | Address | Name | Reaches |
|---|---|---|---|
| `+0x000` | `0x80090458` | `CdStatus` | `CD_status` |
| `+0x010` | `0x80090468` | `CdMode` | `CD_mode` |
| `+0x020` | `0x80090478` | `CdLastCom` | `CD_com` |
| `+0x030` | `0x80090488` | `CdLastPos` | `CD_pos` |
| `+0x03C` | `0x80090494` | `CdReset` | `CD_initintr`, `CD_init`, `CD_initvol` |
| `+0x0A8` | `0x80090500` | `CdFlush` | `CD_flush` |
| `+0x0C8` | `0x80090520` | `CdSetDebug` | `CD_debug` |
| `+0x0DC` | `0x80090534` | `CdComstr` | `CD_comstr`, the member's "none" |
| `+0x110` | `0x80090568` | `CdIntstr` | `CD_intstr`, "none" |
| `+0x144` | `0x8009059C` | `CdSync` | `CD_sync` |
| `+0x164` | `0x800905BC` | `CdReady` | `CD_ready` |
| `+0x184` | `0x800905DC` | `CdSyncCallback` | `CD_cbsync` |
| `+0x198` | `0x800905F0` | `CdReadyCallback` | `CD_cbready` |
| `+0x1AC` | `0x80090604` | `CdControl` | the member's table `D_800B62D0`, `CD_cbsync`, `CD_status`, `CD_cw` |
| `+0x2E8` | `0x80090740` | `CdControlF` | the same |
| `+0x41C` | `0x80090874` | `CdControlB` | the same and `CD_sync` |
| `+0x568` | `0x800909C0` | `CdMix` | `CD_vol` |
| `+0x588` | `0x800909E0` | `CdGetSector` | `CD_getsector` |
| `+0x5A8` | `0x80090A00` | `CdGetSector2` | `CD_getsector2` |
| `+0x5C8` | `0x80090A20` | `CdDataCallback` | `DMACallback` |
| `+0x5EC` | `0x80090A44` | `CdDataSync` | `CD_datasync` |

The functions they call are `bios.o`'s: `CD_initintr` (`0x80091B6C`),
`CD_init` (`0x80091BB8`), `CD_initvol` (`0x80091A7C`), `CD_flush`
(`0x800919A8`), `CD_sync` (`0x80090FCC`), `CD_ready` (`0x8009124C`), `CD_cw`
(`0x80091514`), `CD_vol` (`0x80091920`), `CD_getsector` (`0x80091F00`),
`CD_getsector2` (`0x80092000`) and `CD_datasync` (`0x80091D98`); and
libetc's `DMACallback` (`0x80094C38`).

The state these functions touch:

| Global | Sony's symbol | Meaning |
|---|---|---|
| `D_800B6358` | `CD_cbsync` | The user's sync callback (`bios.o`'s). The command loop below saves it, zeroes it for its preliminary commands and restores it. |
| `D_800B635C` | `CD_cbready` | The user's ready callback. |
| `D_800B6360` | `CD_debug` | The debug print level. |
| `D_800B6364` | `CD_status` | The drive status. `sys.o` reads its low byte (`lbu`); bit 0x10 is the lid-open bit (`CdlStatShellOpen`). `bios.o` declares it as a word. |
| `D_800B6370`, `D_800B6374`, `D_800B6375` | `CD_pos`, `CD_mode`, `CD_com` | The last position, mode and command. |
| `D_800B62D0` | — (the member's static `.data`) | 32 ints, one per command: 1 = send `CdlSetloc` with the parameter first. Set in the image for 0x03 `CdlPlay`, 0x06 `CdlReadN`, 0x15 `CdlSeekL`, 0x16 `CdlSeekP`, 0x1B `CdlReadS`. It ends at `0x800B6350`; `CD_cbsync` follows at `0x800B6358`. |
| `D_800B637C` | `CD_comstr` | 32 command-name pointers, in the image: "CdlSync", "CdlNop", "CdlSetloc", "CdlPlay", "CdlForward", "CdlBackward", "CdlReadN", "CdlStandby", "CdlStop", "CdlPause", "CdlReset", "CdlMute", "CdlDemute", "CdlSetfilter", "CdlSetmode", "?", "CdlGetlocL", "CdlGetlocP", "?", "CdlGetTN", "CdlGetTD", "CdlSeekL", "CdlSeekP", "?", "?", "?", "?", "CdlReadS", "?" x 4. |
| `D_800B63FC` | `CD_intstr` | 8 interrupt-name pointers: "NoIntr", "DataReady", "Complete", "Acknowledge", "DataEnd", "DiskError", "?", "?". |
| `D_800132B4` | — (the member's `.rdata`) | "none". |

The tree has not named data yet, so the C uses the addresses; the symbols in
these tables are the ones Sony's members give these addresses in their
relocation records, and the C's comments carry them.

### The command loop

The three `CdControl`s share one command loop, which the library's compiler
expanded in line into each ([codegen.md](codegen.md), "Functions expanded in
line"). The C writes it once, as the `static inline` helper `cd_cw(com,
param, result, arg3)` in `libcd_sys_6.c`; it emits no symbol. It saves
`CD_cbsync`, then tries up to four times:

1. `CD_cbsync` = 0.
2. If `com` is not `CdlNop` (1) and `CD_status & 0x10` (lid open):
   `CD_cw(CdlNop, 0, 0, 0)`.
3. If `param` is not null and `D_800B62D0[com]` is set: `CD_cw(CdlSetloc,
   param, result, 0)`; a non-zero result ends this try.
4. `CD_cbsync` = the saved callback, then `CD_cw(com, param, result, arg3)`;
   0 from it returns 0.

After four failed tries it restores `CD_cbsync` and returns -1. So the
user's sync callback never sees the preliminary `CdlNop` / `CdlSetloc`, only
the command itself. `arg3` is `CD_cw`'s fourth argument: non-zero returns
right after the command byte is written.

### The functions

* **`CdReset(mode)`** (`0x80090494`). Mode 2 (`CdlModeAP`) calls
  `CD_initintr()` only and returns 1. Otherwise `CD_init()`; a non-zero
  result returns 0. For mode 1 (`CdlModeDA`) it then calls `CD_initvol()`;
  non-zero returns 0. Else 1. No `jal`, data word or address load in the
  image refers to it. `CD_init` and `CD_initvol` each have one other `jal`,
  in `func_80090388`; `CD_initintr` has none.
* **`CdComstr(com)`** (`0x80090534`). Returns `D_800B637C[com]`, or "none"
  (`D_800132B4`) for `com` above 0x1B. Nothing in the image refers to it
  (`bios.o`'s timeout print indexes the table itself).
* **`CdControl(com, param, result)`** (`0x80090604`). `cd_cw(com, param,
  result, 0) == 0`: the command is issued and `CD_cw` waits for it; 1 on
  success, 0 after four failed tries. 11 `jal`s in the image: 5 in game
  code, 1 in `cd_read` (`0x80092E04`, `iso9660.o`), 3 in `func_80093270`
  (`cdread.o`) and 2 in `CdRead2` (`0x800938C8`).
* **`CdControlF(com, param)`** (`0x80090740`). `cd_cw(com, param, 0, 1) ==
  0`: no result buffer, and `CD_cw` returns without waiting for the command;
  1 when it was written. 11 `jal`s: 6 in game code, 5 in `cdread.o`
  (`func_80092F2C`, `func_800931A0`, two in `func_80093270`, `CdReadBreak`
  (`0x80093458`)).
* **`CdControlB(com, param, result)`** (`0x80090874`). If `cd_cw(com,
  param, result, 0)` fails it returns 0; otherwise `CD_sync(0, result) ==
  2`: it blocks until the command's completion interrupt and returns 1 for
  Complete, 0 for DiskError or a timeout. 13 `jal`s: 9 in game code, 3 in
  `CdGetToc2` and 1 in `CdRead` (`0x800935F8`).
* **`CdGetSector(madr, size)`** (`0x800909E0`) and **`CdGetSector2(madr,
  size)`** (`0x80090A00`) return `CD_getsector(madr, size) == 0` and
  `CD_getsector2(madr, size) == 0`. Their callers are the two in
  `func_80092F2C` each (below).

## libcd `bios.o`: the sector DMA and the interrupt handler

`bios.o` (its version string gives `bios.c` revision 1.86) is
`libcd_bios_1.c` to `libcd_bios_3.c`. This section has its sector DMA and
its interrupt handler; its interrupt state, its start-up and `CD_cw` are in
[psyq-libs.md](../psyq-libs.md), "libcd — BIOS.OBJ's interrupt state and
start-up" and "`CD_cw` (BIOS.OBJ)".

The member's pointer table continues past the CD registers (values read from
the image; the register meanings in the last column are the usual
PlayStation memory map's, not names read from the SDK):

| Global | Holds | Meaning |
|---|---|---|
| `D_800B6650` | 0x1F801018 | `CD_getsector` writes 0x20943 here, `CD_getsector2` 0x21020843 (the CD-ROM bus delay / size word). |
| `D_800B6654` | 0x1F8010F0 | The DMA control register (DPCR); both set bit 0x8000 (channel 3's enable). |
| `D_800B6658` | 0x1F8010B0 | DMA channel 3 (CD-ROM) MADR: the destination address. |
| `D_800B665C` | 0x1F8010B4 | DMA channel 3 BCR: `size | 0x10000`. |
| `D_800B6660` | 0x1F8010B8 | DMA channel 3 CHCR: the start value, and the busy bit 0x01000000. |
| `D_8001350C` | "CD_datasync" | The alarm name `CD_datasync` arms. |

* **`CD_getsector2(madr, size)`** (`0x80092000`). `CD_getsector`'s twin
  that starts the sector DMA and returns without waiting for it. CD index
  register = 0, register 3 = 0x80 (request data); 0x21020843 through
  `D_800B6650` and 0x1325 through `D_800B662C` (`CD_getsector`: 0x20943 and
  0x1323, with 0x1325 restored at its end); `*D_800B6654 |= 0x8000`; MADR =
  `madr`, BCR = `size | 0x10000`; waits for bit 0x40 of the CD index
  register; CHCR = 0x11400100 (`CD_getsector`: 0x11000000); reads CHCR back
  once into a volatile local; returns 0. Its one caller is `CdGetSector2`
  (`0x80090A00`); `CD_getsector`'s is `CdGetSector` (`0x800909E0`).
* **`CD_datasync(mode)`** (`0x80091D98`). Waits for that DMA. It arms the
  alarm `D_800E7120` (a `CdAlarm`: the VSync count + 960, named
  "CD_datasync"). Each pass: if the alarm has fired (the VSync count has
  passed it, or 0x3C0000 polls) it prints "CD timeout: " and the command,
  sync and ready names, calls `CD_flush` and returns -1; if CHCR's busy bit
  0x01000000 is clear it returns 0; otherwise `mode != 0` returns 1 (a poll)
  and `mode == 0` loops. Its one caller is `CdDataSync` (`0x80090A44`). The
  C writes the alarm's arming and test as two `static inline` helpers,
  `cd_set_alarm` and `cd_get_alarm`, in each file that uses them.

### The interrupt handler, `func_800920F8`

`func_800920F8` is `bios.o`'s CD-ROM interrupt handler, a static the member
gives no public name, and no Sony text or release names it (the register).
`CD_initintr` (`0x80091B6C`) and `CD_init` (`0x80091BB8`) install it with
`InterruptCallback(2, func_800920F8)` (`0x80094C08`; IRQ 2 is the CD-ROM's);
the address is formed at `0x80091B98` and `0x80091C18`. Nothing calls it
directly.

1. `reg = *D_800B661C & 3`: it saves the register bank index, the low two
   bits of CD register 0 (`D_800B661C` -> 0x1F801800).
2. It calls `func_80090A68` (`bios.o`'s interrupt service, described in
   [io-hardware.md](../io-hardware.md)) until it returns 0. Per non-zero
   return:
   * bit 2 (4: the service set the ready state, on a DataReady, DataEnd or
     DiskError cause) with `CD_cbready` (`D_800B635C`) non-null:
     `CD_cbready(D_800B6634[1], D_800E7110)` = (the ready state, the ready
     result buffer);
   * bit 1 (2: the sync state became Complete or DiskError) with
     `CD_cbsync` (`D_800B6358`) non-null: `CD_cbsync(D_800B6634[0],
     D_800E7108)` = (the sync state, the sync result buffer).

   A return of 1 (an Acknowledge that leaves the command running) calls
   nothing; 6 (the DiskError cause) calls ready, then sync.
3. `*D_800B661C = reg`: the saved index goes back, which the service leaves
   at 1 or 0.

No return value, no frame wait, no RNG draw. The two callback pointers and
the register pointer are read again from memory before every use. The same
body is the `static inline` helper `cd_callback` in `libcd_bios_1.c`, which
`CD_sync`, `CD_ready` and `CD_cw` run by hand when they are themselves
inside a callback.

The first two instructions, the load of `D_800B661C`, come before the stack
adjustment; nothing in the image jumps to the instruction after them. Under
the game's compiler, GCC 2.8.1, the same C gives 49 of the 54 instructions:
it hoists only the `lui` of `D_800B6634`, where the library's compiler
hoists `&D_800B6634[1]` whole and forms `&D_800B6634[0]` as that register
minus 1, as the image has. That was read off the built assembly; the
compiler pass that does it was not traced.

## libcd `cdread.o`: the first two functions

Release 4.5's `cdread.o` lies whole at `0x80092EF8` (628 words). Its first
two functions are in `libcd_cdread_1.c`, the rest in `libcd_cdread_2.c`.
Its state is the record `D_800B6730`, whose words
[psyq-libs.md](../psyq-libs.md), "libcd — CDREAD.OBJ", lists, with the
member's public functions. Its `CdReadSync` (`0x80093794`) is what the game
asks through `cd_read_check` (`0x80020E34`, [io-hardware.md](../io-hardware.md)).
`D_80013724` is "CdRead: sector error\n".

Neither function has a public name, and no Sony text or release names them
(the register).

* **`func_80092EF8`**: the pause's sync callback.
  `CdSyncCallback(state[10])`, then `state[9] = 0`: when the `CdlPause` the
  member sent completes, the caller's sync callback is put back and the
  pause-pending flag the pause wait spins on is cleared. `CdReadBreak`,
  `func_80092F2C` and `func_800931A0` install it (by address; nothing calls
  it) right before each sends `CdlPause`.
* **`func_80092F2C(intr, result)`**: `CdRead`'s ready callback, installed by
  `func_80093270`.
  - `state[13] = result`.
  - On `intr == 1` (DataReady) with `state[5] > 0`: when the sector size
    `state[4]` is 0x200 words (2048-byte sectors) it first reads the 12-byte
    sector header into a local and compares `CdPosToInt` of it with the
    expected sector `state[8]`; a mismatch prints "CdRead: sector error" and
    sets `state[5] = -1`. Then it takes the sector into the write pointer
    `state[2]` (`state[4]` words).
  - Both reads go through `CdGetSector` normally and `CdGetSector2` in DMA
    mode (`CD_read_dma_mode & 1`). In DMA mode the header read is bracketed
    by `CdDataCallback(0)`, `CdDataSync(0)`, `CdDataCallback(func_800931A0)`,
    and the bookkeeping (`state[2] += state[4] * 4`, `state[5]--`,
    `state[8]++`) is left to `func_800931A0`, the data callback, which runs
    when the sector's DMA ends; otherwise this function does it itself.
  - Any other interrupt: `state[5] = -1`.
  - Then `state[6] = VSync(-1)`; a negative count calls `func_80093270(1)`;
    more than 1200 vblanks after `state[7]` the count becomes -1.
  - When the count is 0 (done), or the 1200 vblanks have passed:
    `CdReadyCallback(state[11])`, in DMA mode `CdDataCallback(state[12])`,
    `CdSyncCallback(func_80092EF8)`, `CdlPause` through `CdControlF`,
    `state[9] = 1`, and the user's callback `CD_cbread` (`D_800B62C0`), if
    set, is called with `CdlComplete` (2) for a count of 0, else
    `CdlDiskError` (5), and `result`.

## libc2: the console and `strncmp`

Three libc2 members: `puts.o` at `0x800921D8` (24 words), `putchar.o` at
`0x80092238` (152 words) and `strncmp.o` at `0x80092E78` (32 words), in
`libc2_puts.c`, `libc2_putchar.c` and `libc2_strncmp.c`; `ctype.o`
(`toupper`, `tolower`) lies at `0x80092498`. libc2's members are the same
bytes in releases 4.1 to 4.5, so which of those the program took cannot be
told ([naming.md](../../../../naming.md)); in releases 4.6 and 4.7 `puts` is
a three-instruction BIOS stub in libc instead. How `putchar.o` was compiled
(a counter only a `-G0` build gives, and two functions expanded in line into
a third) is in [codegen.md](codegen.md).

`putchar.o`'s state (`.data` 0x10 bytes, `.bss` 0x20):

| Global | Meaning |
|---|---|
| `D_800B6680` | The console column (`.data` + 0): reset by `'\n'`, counted for printable characters, read by the tab expansion. |
| `D_800B6684` | Bytes waiting in the line buffer (`.data` + 4). |
| `D_800E7140` | The 32-byte line buffer (`.bss`). |
| `D_800B6691` | `_ctype_ + 1`: the ctype table past its EOF entry. `putchar.o` reads it as signed bytes (`lb`), `ctype.o`'s `toupper` and `tolower` as unsigned (`lbu`). |
| `D_80013524` | "<NULL>" (`puts.o`'s string). |

The output goes to libapi's `write` (`0x8008B408`, `a53.o`) as `write(1,
buf, n)`: the console is file descriptor 1.

* **`puts(s)`** (`0x800921D8`). Each character of `s` ("<NULL>" for a null
  pointer) to `_putchar`, then `_putchar_flash()`. No newline is added.
  `void`. Callers, 10 `jal` sites: libcd's `func_80090A68`, `CD_sync`,
  `CD_ready`, `CD_cw`, `CD_init`, `CD_datasync`, `func_80092F2C` and
  `func_80093270` (twice), and libetc's `func_80094B20`, VSync's wait.
* **`_putchar(c)`** (`0x80092238`). `'\n'`: `_putchar('\r')` first, column
  = 0. `'\t'`: `_putchar(' ')` until the column is a multiple of 8, then
  return (the tab itself is not stored). Anything else: the column counts on
  when `isprint(c)` (`(_ctype_ + 1)[(u_char) c] & 0x97`). Then, if 32 bytes
  are waiting, `write(1, buf, 32)` and the count is 0; the byte is appended.
  Called by `puts`, itself and `putchar`.
* **`_putchar_flash()`** (`0x80092334`). If bytes are waiting, `write(1,
  buf, n)` and the count is 0. Called by `puts`.
* **`putchar(c)`** (`0x80092370`). `_putchar(c); _putchar_flash();` with
  both bodies expanded in line by the compiler (its two `jal _putchar` are
  the recursive calls of the expanded copy). Nothing in the image calls it:
  no `jal`, data word or address load.
* **`strncmp(s1, s2, n)`** (`0x80092E78`). Signed chars. A null pointer is
  answered before anything is read: both null 0, `s1` null -1, `s2` null 1.
  Otherwise the classic loop: 0 at a common NUL or when `n` runs out, else
  the difference of the first differing pair. Callers: `_cmp` (`0x800927E0`,
  `iso9660.o`, 12 characters) and `CD_newmedia` (`0x80092800`, its "CD001"
  test, 5 characters).

## libcd: the streaming ring

The stream library is these members:

| Member | Functions | File |
|---|---|---|
| `cdrom.o` | `StSetRing` (`0x80090078`) | `libcd_cdrom.c` |
| `cdread2.o` | `CdRead2` (`0x800938C8`), `func_8009394C` | `libcd_cdread2_1.c`, `libcd_cdread2_2.c` |
| `c_002.o` | `StClearRing` (`0x80093978`) | `libcd_c_002.c` |
| `c_004.o` | `data_ready_callback` (`0x800939D8`), `StGetBackloc` (`0x80093A64`) | `libcd_c_004_1.c`, `libcd_c_004_2.c` |
| `c_005.o` | `StSetStream` (`0x80093AC8`) | `libcd_c_005.c` |
| `c_007.o` | `StFreeRing` (`0x80093B58`) | `libcd_c_007.c` |
| `c_008.o` | `init_ring_status` (`0x80093C08`) | `libcd_c_008.c` |
| `c_009.o` | `StGetNext` (`0x80093C48`) | `libcd_c_009.c` |
| `c_010.o` | `StSetMask` (`0x80093D08`) | `libcd_c_010.c` |
| `c_011.o` | `StCdInterrupt` (`0x80093D28`), `func_80094644`, `func_80094670` | `libcd_c_011_1.c`, `libcd_c_011_2.c` |

The main executable's own code uses none of it: no `jal`, data word or
address load in the image reaches `StSetRing`, `CdRead2`, `StSetStream`,
`StGetNext`, `StFreeRing` or `StGetBackloc`. The movie player in the
overlay `MENU.BIN` drives it, calling these functions at their addresses in
the main executable (the overlay's register lists them as the main
executable's): `movie_stream_start` (`0x80196768`) calls
`StSetRing(ring, 0x20)` and `StSetStream(1, 1, -1, 0, 0)` (24-bit, from frame
1, an end frame of 0xFFFFFFFF, no callbacks), `movie_read_start`
(`0x80196B0C`) calls `CdRead2(0x1C0)` (streaming, double speed, real-time
ADPCM) until it returns non-zero, `movie_frame_next` (`0x80196954`) polls
`StGetNext`, and `movie_frame_decode` (`0x801968D0`) calls `StFreeRing`. [video.md](../../../video.md) describes the player.

### The streaming globals

The symbols are the ones Sony's members give these addresses in their
relocation records; the C's comments carry them. The meanings are what the
C of the members above does with each.

| Global | Sony's symbol | Meaning |
|---|---|---|
| `D_800E70C0` | `Stframe_no` | Frame number of the frame being read (the low half of the header's `frameCount`); 0 between frames. |
| `D_800E70C4` (s16) | `Stsector_offset` | Sector index expected next within the frame. |
| `D_800E70C8` | `StRgb24` | `StSetStream` sets it to `mode & 1`. Non-zero: `StCdInterrupt` sets the sector DMA up with 0x20943 / 0x1323 and CHCR 0x11000000 (`CD_getsector`'s words); zero: with 0x21020843 and CHCR 0x11400100 (`CD_getsector2`'s). |
| `D_800E70CC` | `StCdIntrFlag` | Set to 1 when `StCdInterrupt` returns at once because `StRgb24` is set and the busy bit 0x01000000 of the register at 0x1F801098 (DMA channel 1's CHCR in the usual memory map) is on; cleared by `StClearRing`. |
| `D_800E70D0` | `StCHANNEL` | Copied to `CChannel` at a frame's last sector. |
| `D_800E70D4` | `StStartFrame` | Frame number to wait for while `StSTART_FLAG` is 1; 0 disables the wait. |
| `D_800E70D8` | `CChannel` | The value bits 10 to 14 of a sector header's `type` must have. |
| `D_800E70DC` | `StFinalSector` | 1 while the last sector of a frame is in transfer; `StCdInterrupt` does nothing while it is set. Cleared by `data_ready_callback` and `StClearRing`. |
| `D_800E70E0` | `StEmu_Idx` | Sector index into the emulation buffer. |
| `D_800E70E4` | `StRingIdx1` | The ring's write index: the slot the next sector goes to. |
| `D_800E70E8` | `StRingIdx2` | The slot of the first sector of the frame being read. |
| `D_800E70EC` | `StRingIdx3` | The reader's slot: where `StGetNext` looks for the next frame. `StFreeRing` moves it past the frame it frees; `StGetNext` resets it to 0 at a wrap marker; `StClearRing` clears it. |
| `D_800E70F0` | `StEmu_Addr` | Emulation buffer; non-zero switches `StCdInterrupt` from DMA to memory copies. `StSetStream` clears it. |
| `D_800E70F4` | `StEndFrame` | Last frame number; 0 for none. |
| `D_800E70F8` | `StSTART_FLAG` | Set by `StSetMask` (to 1 from `StSetStream`), and to 1 by `StCdInterrupt` at the end frame or a full ring. While it is 1 and `StStartFrame` is non-zero, sectors of other frames are dropped; a sector of that frame clears it. |
| `D_800E70FC` | `StRingBase` | Where the current sector's data goes. |
| `D_800E7100` | `StRingAddr` | The ring (layout below). |
| `D_800E7104` | `StRingSize` | Number of slots. |
| `D_800E7138` | `StMode` | `CdRead2` sets it to 0 when its mode has bit 0x20, else 1. When 0, `StCdInterrupt` reads twelve bytes from the CD data register before the header: four kept as the sector's `loc`, eight discarded. |
| `D_800E9560` | static, `c_004.o` `.bss` + 0 | `CdlLOC`: the `loc` of the last completed frame's first sector header. |
| `D_800E9564` | static, `c_004.o` `.bss` + 4 | That frame's number. |
| `D_800E9570` | `StFunc1` | User callback, the fourth argument of `StSetStream`; called when a frame completes. |
| `D_800E9574` | `StFunc2` | User callback, the fifth argument of `StSetStream`; called by `StCdInterrupt` where it sets `StSTART_FLAG` (the end frame reached, or the ring full with no end frame). |
| `D_800E9578` | static, `c_011.o` `.bss` + 0 | `StCdInterrupt`'s pointer to the current sector header (a `u16 *`). |

### The ring

`StRingAddr` is `StRingSize` 32-byte headers (`StHEADER`) followed by the
data area, 504 words (2016 bytes) a slot:

| Item | Address |
|---|---|
| Header of slot `i` | `StRingAddr + i * 32` |
| Data of slot `i` | `StRingAddr + StRingSize * 32 + i * 2016` |

`c_007.o`, `c_008.o` and `c_009.o` compute these addresses with integer
arithmetic on the ring base, and the C declares `StRingAddr` as an integer
in their files and as a pointer in the others; each matches its own
functions ([codegen.md](codegen.md) has why the form decides the bytes).

The header's first halfword, `id` (+0x00), is the slot's state for the
reader:

| `id` | Meaning | Set by | Tested by |
|---|---|---|---|
| 0 | free | `StFreeRing`, `StGetNext` (the wrap marker, when `StEndFrame` is set), `init_ring_status`, `StCdInterrupt` on a dropped sector | `StCdInterrupt` (the slot it is about to fill, and slot 0 before a wrap) |
| 1 | wrap marker: a frame's first sector did not fit in the slots left (`StRingSize - StRingIdx1 - 1 < nSectors`) | `StCdInterrupt`. With an end frame it copies the header to slot 0 and goes on there; without one it sets `StSTART_FLAG`, calls `StFunc2` and takes no more sectors, since the marked slot is never free | `StGetNext` |
| 2 | first slot of a complete frame | `data_ready_callback` | `StGetNext` |
| 3 | a sector's transfer was started into this slot | `StCdInterrupt` | — |
| 4 | handed to the caller | `StGetNext` | `StFreeRing` |
| 0x160 | the sector header as it comes off the disc (its first halfword) | the disc | `StCdInterrupt` |

`nSectors` (+0x06) of a frame's first header is the number of slots the
frame occupies.

### The functions

* **`StClearRing()`** (`0x80093978`). Empties the ring: `StRingIdx3`,
  `StRingIdx2`, `StRingIdx1` and `StFinalSector` = 0; `init_ring_status(0,
  StRingSize)`; then `StCdIntrFlag`, `Stsector_offset` and `Stframe_no` = 0.
  Its one caller is `StSetRing`, which stores the ring's address and size
  first.
* **`init_ring_status(start, count)`** (`0x80093C08`). Zeroes the first
  word of `count` headers from slot `start`.
* **`data_ready_callback()`** (`0x800939D8`). The ring's "frame complete"
  hook. `CdRead2` installs it with `CdDataCallback` when the mode has
  `CdlModeStream` (0x100), and `StCdInterrupt` calls it at its end when
  `StEmu_Addr` and `StFinalSector` are both non-zero.
  1. The header at `StRingAddr + StRingIdx2 * 32` gets `id` (+0x00) = 2
     (`StCOMPLETE` in libcd.h).
  2. Its `loc` (+0x1C) is copied to `D_800E9560` and its `frameCount`
     (+0x08) to `D_800E9564`; `StGetBackloc` returns them.
  3. `StRingIdx2 = StRingIdx1`.
  4. `StFunc1` is called if non-zero.
  5. `StFinalSector = 0`.

  The `id` is stored through a `u16 *` view of the header, as
  `StCdInterrupt` reads headers; the two other accesses use `StHEADER`'s
  members. The C's comment marks that spelling ([codegen.md](codegen.md),
  "Marked spellings").
* **`StSetStream(mode, start_frame, end_frame, func1, func2)`**
  (`0x80093AC8`). Sets up a streaming session: `StSetMask(1, start_frame,
  end_frame)` (`StSTART_FLAG` = 1, `StStartFrame`, `StEndFrame`), then
  `StEmu_Addr` = 0, `StFunc1` = `func1`, `StRgb24` = `mode & 1`, `CChannel`
  = 0, `StCHANNEL` = 0, `Stsector_offset` = 0, `Stframe_no` = 0, `StFunc2` =
  `func2`. Returns nothing.
* **`StGetNext(addr, header)`** (`0x80093C48`). Hands the next complete
  frame to the caller.
  1. Looks at the header of slot `StRingIdx3`.
  2. `id == 1`: `StRingIdx3 = 0`; the marker's `id` is cleared when
     `StEndFrame` is non-zero; the header of slot 0 is looked at instead.
  3. `id == 2`: `id = 4`, `*addr` = the data address of slot `StRingIdx3`,
     `*header` = the header; returns 0.
  4. Any other state: returns 1 (no frame yet).
* **`StFreeRing(base)`** (`0x80093B58`). Gives a frame's slots back. `base`
  is the data address `StGetNext` returned; the slot is `n = (base - data
  area) / 504` in words. If header `n`'s `id` is not 4 it returns 1.
  Otherwise it clears the `id` of headers `n` to `n + nSectors - 1`, sets
  `StRingIdx3 = n + nSectors` and returns 0. The ring base is reloaded for
  every header cleared.
* **`CdRead2(mode)`** (`0x800938C8`). Sends `CdlSetmode` with the mode's
  low byte; when the mode has `CdlModeStream` (0x100) it sets `StMode`,
  makes `data_ready_callback` the data callback and `func_8009394C` the
  ready callback; then sends `CdlReadS` (0x1B) through `CdControl` and
  returns that call's result, which the movie player tests.
  `func_8009394C` is a bare call of `StCdInterrupt`: a static with no name,
  since releases 3.3 to 4.4 export its 32 bytes under six libcd names and
  more in other libraries (the register).
* **`func_80094670(ch, madr, blocks, size, chcr, mode, unused)`**: the
  static after `func_80094644` (a word copy, `StCdInterrupt`'s stand-in for
  the DMA when a stream is emulated) in `c_011.o`. Neither has a public
  name, and no Sony text or release names them (the register).
  `StCdInterrupt` calls it three times with `ch` = 3 (the CD-ROM's DMA
  channel): `(3, header, 0, 8, chcr, 0, 0)` for the 8-word sector header and
  `(3, StRingBase, 0, 0x1F8, chcr, last, 0)` for the 504-word body, `last` =
  1 for a frame's last sector.
  1. Waits while the channel's CHCR (0x1F801088 + `ch` * 16) has bit 24
     (busy) set, at most 0x10000 polls; on the last it prints "DMA STATUS
     ERROR %x" (`D_80013774`) with CHCR through `printf` (`0x80021700`) and
     goes on.
  2. `mode == 1` sets, anything else clears, bit `ch` of byte 2 of DICR (the
     channel's interrupt enable); DICR is then read into a volatile local.
  3. DPCR `|= 1 << (ch * 4 + 3)` (the channel's master enable).
  4. Writes MADR = `madr` and BCR = `(blocks << 16) | size` through a
     pointer that walks the channel's registers from 0x1F801080 + `ch` * 16.
  5. Polls the CD-ROM index / status register until bit 6 is set.
  6. CHCR = `chcr` (starts the transfer) and reads CHCR back.

  The seventh argument every caller passes is not read. The register
  meanings (busy bit, enable bits, bit 6 of the CD status register = data
  FIFO not empty) are the PlayStation's, not read from this code; the
  addresses and bit positions are. Three shapes of its C are fitted to the
  bytes and marked as such in its comment ([codegen.md](codegen.md)).

`c_011.o`'s `.data` is a block of pointers at `0x800B67E0` (values read from
the image). The rows `func_80094670` uses, and the others the same block
shows:

| Global | Offset | Holds | Used by `func_80094670` |
|---|---|---|---|
| `D_800B67E0` | +0x00 | 0x1F801800, CD index / status register | polled for bit 6 (step 5) |
| `D_800B67E4` | +0x04 | 0x1F801801 | — |
| `D_800B67E8` | +0x08 | 0x1F801802 | — (`StCdInterrupt`'s data register) |
| `D_800B67EC` | +0x0C | 0x1F801803 | — |
| `D_800B67F0` | +0x10 | 0x1F801018 | — |
| `D_800B67F4` | +0x14 | 0x1F801020 | — |
| `D_800B67F8` | +0x18 | 0x1F8010F0, DPCR | step 3 |
| `D_800B67FC` | +0x1C | 0x1F8010F4, DICR | step 2 |
| `D_800B6800` | +0x20 | 0x1F801098 | — |
| `D_800B6810` | +0x30 | 0x1F8010B8, DMA channel 3's CHCR | — (`StCdInterrupt` waits on it) |
| `D_800B6828` | +0x48 | `debug_cause` (the member's one exported datum) | — (`StCdInterrupt` leaves its exit number there) |

## libcd `s_002.o`: `CdIntToPos`

* **`CdIntToPos(i, p)`** (`0x80094818`, `libcd_s_002.c`). Absolute sector
  number to the drive's BCD position: `i += 150` (the two-second lead-in),
  `p->sector = BCD(i % 75)`, `p->second = BCD(i / 75 % 60)`, `p->minute =
  BCD(i / 75 / 60)`; returns `p`. `p->track` is not written. The BCD
  encoding `((n / 10) << 4) + n % 10` is an inline function in the source;
  the C writes it as the `static inline` helper `cd_itob`, which emits no
  symbol. Nine calls in the image: six in game code, two in `iso9660.o`
  (`CD_cachefile` (`0x80092B68`) and `cd_read` (`0x80092E04`)) and one in
  `StGetBackloc`. Its inverse, `CdPosToInt` (`0x8009491C`), is the next
  function.

## libetc: `VSync`

`VSync(mode)` (`0x800949A8`, `libetc_vsync_1.c`) is the first 94 words of
libetc's `vsync.o`; the rest of the member, its wait `func_80094B20` and its
state, is in [libetc-libsnd.md](libetc-libsnd.md). The image has 46 calls to
`VSync`.

1. Reads GPUSTAT through `D_800B6838` (-> 0x1F801814) and root counter 1
   (the horizontal-blank count) through `D_800B683C` (-> 0x1F801110), the
   counter into a volatile local again and again until two successive reads
   agree.
2. `ret = (counter - D_800B6840) & 0xFFFF`: horizontal blanks since the
   last waiting call (`D_800B6840` holds the counter as that call left it).
3. `mode < 0`: returns `Vcount` (`D_800B7970`, the vertical-blank count
   libetc's interrupt code keeps). `mode == 1`: returns `ret`.
   Neither waits nor touches the statics.
4. Otherwise `func_80094B20(last + mode - 1, mode - 1)` for `mode > 0` and
   `func_80094B20(last, 0)` for `mode == 0`, `last` = `D_800B6844`; then
   GPUSTAT is read again and `func_80094B20(Vcount + 1, 1)` waits for one
   more vertical blank.
5. If that GPUSTAT has bit 22 set (interlace) and its bit 31 (the field)
   still equals the current one, it spins until bit 31 differs.
6. `D_800B6844 = Vcount`; `D_800B6840` = the counter, stored and re-read until
   two reads agree. Returns `ret`.

So `VSync(0)` waits for the next vertical blank after the previous call's,
`VSync(n)` for `n` blanks counted from the previous call's, and both return
the horizontal blanks elapsed. That GPUSTAT bit 22 is the interlace flag and
bit 31 the odd / even field is the PlayStation's register layout, not read
from this code. The C holds the constant 1 of `last + mode - 1` in a
variable, a marked spelling: the original construct is not known
([codegen.md](codegen.md), "Expressions").
