# libapi pad code, libcard, libmcrd

This page covers the memory-card code of the main executable (`SLUS_006.28`)
that is Sony's: libapi's PAD.OBJ (the kernel's pad interrupt entry), libcard's
INIT, PATCH and END objects (they start the BIOS card driver and patch the
kernel), libmcrd's LIBMCRD.OBJ, USERFUNC.OBJ and BIOS.OBJ (the `MemCard*`
functions the game calls), and libc2's `strcat`, which only libmcrd uses. Every
object here is a member of PSY-Q release 4.5 that lies whole in the image; how
that was proven, and how the names are given, is in
[naming.md](../../../../naming.md). Each member's code is in
`decomp/SLUS_006.28/src/lib<library>_<member>.c` (a member the tree compiles
in several runs is split into `_1`, `_2` ...). How the objects were compiled,
and which C shapes decide their bytes, is in [codegen.md](codegen.md). The
overview of all of Sony's libraries in this program is
[psyq-libs.md](../psyq-libs.md).

Sony's private functions that no release names keep their addresses
(`func_800A1D94`). Kernel addresses and routines below are those of the
SCPH-1001 kernel, read as a reference; the program does not carry it.

## Where the objects lie

| Object | Range | Files | Functions |
|---|---|---|---|
| libmcrd INIT.OBJ | 0x800A1068 to 0x800A10B7 | `libmcrd_init.c` | `MemCardInit`, `MemCardEnd` |
| libcard INIT.OBJ | 0x800A10B8 to 0x800A11B7 | `libcard_init.c` | `InitCARD`, `StartCARD`, `StopCARD` |
| libapi PAD.OBJ | 0x800A11B8 to 0x800A14B7 | `libapi_pad_1.c` to `libapi_pad_4.c` | ten (below) |
| libapi A18 to A21, L02, L03 | 0x800A14B8 to 0x800A1517 | `libapi_a18.c` ... `libapi_l03.c` | BIOS stubs |
| libapi PATCH.OBJ, CHCLRPAD.OBJ | 0x800A1518 to 0x800A1627 | `libapi_patch.c`, `libapi_chclrpad.c` | [psyq-libs.md](../psyq-libs.md) §"PATCH.OBJ and CHCLRPAD.OBJ" |
| libcard A74 to A76 | 0x800A1628 to 0x800A1657 | `libcard_a74.c` ... `libcard_a76.c` | BIOS stubs |
| libcard PATCH.OBJ | 0x800A1658 to 0x800A1877 | `libcard_patch.c` | seven pieces (below) |
| libcard END.OBJ | 0x800A1878 to 0x800A18F7 | `libcard_end.c` | `_ExitCard` |
| libmcrd LIBMCRD.OBJ | 0x800A18F8 to 0x800A3617 | `libmcrd_libmcrd_1.c` to `_13.c` | 26 (below) |
| libc2 STRCAT.OBJ | 0x800A38E8 to 0x800A3997 | `libc2_strcat.c` | `strcat` |
| libmcrd USERFUNC.OBJ | 0x800A3A18 to 0x800A3B27 | `libmcrd_userfunc_1.c` to `_3.c` | four |
| libmcrd BIOS.OBJ | 0x800A3B28 to 0x800A41D7 | `libmcrd_bios_1.c` to `_7.c` | sixteen |
| libmcrd LOW.OBJ | from 0x800A41E8 | `libmcrd_low.c` | `_card_format2`, `_card_create2` |

The BIOS stubs each load a function number and jump into the kernel's A0, B0
or C0 table. Those this page meets, with the number each stub loads:

| Stub | Address | Table:number |
|---|---|---|
| `InitPAD2`, `StartPAD2`, `StopPAD2`, `PAD_init2` | `0x800A14B8`, `0x800A14C8`, `0x800A14D8`, `0x800A14E8` | B0:12, B0:13, B0:14, B0:15 |
| `SysEnqIntRP`, `SysDeqIntRP` | `0x800A14F8`, `0x800A1508` | C0:02, C0:03 |
| `InitCARD2`, `StartCARD2`, `StopCARD2` | `0x800A1628`, `0x800A1638`, `0x800A1648` | B0:4A, B0:4B, B0:4C |
| `open`, `lseek`, `read`, `write`, `close` | `0x8008B3D8` to `0x8008B418` | B0:32 to B0:36 |
| `nextfile`, `erase` | `0x800A3618`, `0x800A3628` | B0:43, B0:45 |
| `_card_info`, `_card_load` | `0x800A3998`, `0x800A39A8` | A0:AB, A0:AC |
| `_card_write`, `_new_card` | `0x800A39B8`, `0x800A39C8` | B0:4E, B0:50 |
| `_card_read` | `0x800A4CE8` | B0:4F |

`_card_clear(chan)` (`0x800A39D8`, libcard CARD.OBJ, `libcard_card.c`) is
`_new_card()` then `_card_write(chan, 0x3F, 0)`.

## libapi — PAD.OBJ

| Offset | Address | Function | File |
|---|---|---|---|
| `+0x000` | `0x800A11B8` | `SetInitPadFlag` | `libapi_pad_1.c` |
| `+0x00C` | `0x800A11C4` | `ReadInitPadFlag` | `libapi_pad_1.c` |
| `+0x01C` | `0x800A11D4` | `PAD_init` | `libapi_pad_1.c` |
| `+0x0AC` | `0x800A1264` | `InitPAD` | `libapi_pad_1.c` |
| `+0x13C` | `0x800A12F4` | `StartPAD` | `libapi_pad_1.c` |
| `+0x16C` | `0x800A1324` | `StopPAD` | `libapi_pad_2.c` |
| `+0x1A4` | `0x800A135C` | `func_800A135C` (static) | `libapi_pad_3.c` |
| `+0x21C` | `0x800A13D4` | `RemovePatchPad` (static) | `libapi_pad_3.c` |
| `+0x254` | `0x800A140C` | `_Pad1` (static) | `libapi_pad_4.c` |
| `+0x2BC` | `0x800A1474` | `_IsVSync` (static) | `libapi_pad_4.c` |

`RemovePatchPad`, `_Pad1` and `_IsVSync` carry the names releases 3.7 and 4.0
export for the same bytes (`sdk-earlier`). `func_800A135C` has none: no Sony text names it, and no release exports its
bytes (the register).

The game starts its controllers with libpad (`PadInitDirect`, in
`pad_start.c`; [libpad.md](libpad.md)), and nothing in the program calls
`PAD_init`, `InitPAD`, `StartPAD`, `StopPAD` or `SetInitPadFlag`. Of the object
only `ReadInitPadFlag` runs, called by `InitCARD`.

The object's data: `D_800B8440` (`.data+0`) is the pad-initialised flag,
`D_800B8444` (`.data+4`) a pointer to the controller port at 0x1F801040
(`[5]`, `+0xA`, is its control register) and `D_800B8448` (`.data+8`) a
pointer to the interrupt controller at 0x1F801070 (`[0]` I_STAT, `[1]` I_MASK).

* **`SetInitPadFlag(flag)`** stores `flag` in `D_800B8440`;
  **`ReadInitPadFlag()`** returns it.
* **`PAD_init(a, b, c, d)`**: `_remove_ChgclrPAD()`; `_patch_pad()` inside a
  critical section; `ChangeClearPAD(0)`; `func_800A135C()`; the BIOS
  `PAD_init2(a, b, c, d)`; then the flag = 1. **`InitPAD`** is the same with
  `InitPAD2` in place of `PAD_init2`. `_remove_ChgclrPAD` and `_patch_pad` are
  libapi's CHCLRPAD.OBJ and PATCH.OBJ ([psyq-libs.md](../psyq-libs.md)).
* **`StartPAD()`**: the BIOS `StartPAD2()`, `ChangeClearPAD(0)`, `EnablePAD()`;
  returns 1.
* **`StopPAD()`**: `DisablePAD()`, the BIOS `StopPAD2()`, `RemovePatchPad()`,
  then the flag = 0.
* **`func_800A135C()`** (static). Inside `EnterCriticalSection` /
  `ExitCriticalSection` it fills the object's interrupt-queue entry (`func2` =
  `_Pad1`, `func1` = `_IsVSync`, `next` = 0, the fourth word 0) and puts it at
  the head of the kernel's queue 1: `SysDeqIntRP(1, &entry)`, then
  `SysEnqIntRP(1, &entry)`. Returns 1. Called by `PAD_init` and `InitPAD`. The
  four stores are a function expanded in line in the original; the C writes
  them as the inline helper `pad_set_vsync` ([codegen.md](codegen.md)
  §"Functions expanded in line").
* **`RemovePatchPad()`** (static). `SysDeqIntRP(1, &entry)` inside a critical
  section; returns 1.
* **`_IsVSync()`** (static), the entry's test: 1 when the vertical-blank
  interrupt (bit 0) is both enabled in I_MASK and pending in I_STAT, else 0.
* **`_Pad1()`** (static), the entry's handler: writes 0 to the controller
  port's control register, waits ten turns of a loop, returns 0.

The entry is `D_800EA8E0` (PAD.OBJ `.bss+0`, 16 bytes; the C's comment notes
that release 4.6's object calls it `_apiPadPatch`), of type `PadIntRP`
(`include/psyq_private.h`):

| Offset | Member | Meaning |
|---|---|---|
| `+0x0` | `next` | The queue's link: `SysEnqIntRP` writes the old head here. |
| `+0x4` | `func2` | The handler, called with `func1`'s result when that is non-zero: `_Pad1`. |
| `+0x8` | `func1` | The test, called first: `_IsVSync`. |
| `+0xC` | `unk_0c` | Written 0; nothing reads it. |

The order of `func1` and `func2` is the kernel's: its exception handler's
queue walk (SCPH-1001 0xDE8) loads `+8` and `+4`, calls the word at `+8` when
it is non-zero and, when that returns non-zero and the word at `+4` is
non-zero, calls it with the result. `SysEnqIntRP` (kernel 0x1420) puts the
entry at the head of chain `prio` (`[0x100] + prio * 8`) and stores the old
head in the entry's `+0`.

## libcard — starting the card driver and patching the kernel

PATCH.OBJ here is the 544-byte form, with `_patch_card_info`, that releases
4.5, 4.6 and 4.7 carry; release 4.4's has no `_patch_card_info`.

### INIT.OBJ

* **`InitCARD(val)`** (`0x800A10B8`). `ChangeClearPAD(0)`, `VSync(0)`, then
  `EnterCriticalSection()`; `val` becomes 0 unless `ReadInitPadFlag()` is
  non-zero; the BIOS `InitCARD2(val)`; then PATCH.OBJ's four routines in the
  order `_copy_memcard_patch()`, `_patch_card()`, `_patch_card2()`,
  `_patch_card_info()`; and `ExitCriticalSection()` only if the enter returned
  1. Callers: `MemCardInit` and BIOS.OBJ's `_card_open`.
* **`StartCARD()`** (`0x800A1148`). The BIOS `StartCARD2()` and
  `ChangeClearPAD(0)` inside a critical section (left only if the enter
  returned 1); returns 0.
* **`StopCARD()`** (`0x800A1190`). The BIOS `StopCARD2()`, then `_ExitCard()`;
  returns 0.

### PATCH.OBJ and END.OBJ: Sony's hand assembly

The two objects are Sony's assembly, not compiled C, and the tree keeps them
as byte ranges (`INCLUDE_ASM`). [hand-assembly.md](../../hand-assembly.md) has
the evidence piece by piece: `_patch_card_info`, the two stubs, `_patch_card`,
`_patch_card2` and `_ExitCard` by their own marks, the two bodies and
`_copy_memcard_patch` by object membership.

| Object offset | Address | Piece | What it is |
|---|---|---|---|
| PATCH `+0x000` | `0x800A1658` | `_patch_card_info` | routine |
| PATCH `+0x044` | `0x800A169C` | `func_800A169C` | the first patch body; not a function |
| PATCH `+0x070` | `0x800A16C8` | `func_800A16C8` | the second patch body; not a function |
| PATCH `+0x0B4` | `0x800A170C` | `func_800A170C` | two five-word stubs; not a function |
| PATCH `+0x0DC` | `0x800A1734` | `_patch_card` | routine |
| PATCH `+0x170` | `0x800A17C8` | `_patch_card2` | routine |
| PATCH `+0x1E0` | `0x800A1838` | `_copy_memcard_patch` | routine |
| END `+0x00` | `0x800A1878` | `_ExitCard` | routine |

The library gives the bodies and the stubs no symbol: they are code it keeps
as data. Nothing calls them in the program; they are copied into kernel RAM
and run there on the kernel's registers. The two bodies are one 28-word block
of the object (`+0x44` to `+0xB3`); the tree splits it at its two `jr $ra`,
not at a symbol. `func_800A170C` is the object's two stubs, and `_patch_card`
begins at its own `$ra` save after them.

The four routines save `$ra` in `.bss` or have no frame, call the BIOS in line
(`li $t1,n` / `li $t2,0xB0` / `jalr $t2`), use the trapping `addi`, and share
one copy loop (pointers in `$t2` / `$t1`, a `nop` in the load-delay slot). The
bodies take their inputs in `$v1` and `$v0` and use `$t7`, `$t8`, `$t9` and
`$t0` as temporaries, where a C function's parameters arrive in `$a0` and
`$a1`.

State:

| Symbol | Where | Meaning |
|---|---|---|
| `D_800EA910` | PATCH.OBJ `.bss+0` | The saved `$ra` of `_patch_card_info`, `_patch_card` and `_patch_card2`. |
| `D_800EA920` | END.OBJ `.bss+0` | The saved `$ra` of `_ExitCard`. |
| `D_800A18E8` | END.OBJ text `+0x70` | Three zero words: what `_ExitCard` copies. |
| `D_800A18F4` | END.OBJ text `+0x7C` | The copy's end label (one pad word). |
| 0xDF80 to 0xDFEF (kernel RAM) | | Where `_copy_memcard_patch` puts the two bodies: 0xDF80 and 0xDFAC. |
| 0xDFFC (kernel RAM) | | Written by `_patch_card`: the address the second body jumps back to. |

What each does (SCPH-1001 addresses: B0 table 0x874, C0 table 0x674,
`ChangeClearPad` 0x43D0, the exception handler 0xC80):

* **`_copy_memcard_patch()`** (`0x800A1838`). Copies the 28 words 0x800A169C
  to 0x800A170B (the two bodies) to 0xDF80. It does not flush the cache: the
  stubs reach the bodies through the uncached mirror (0xA000DF80, 0xA000DFAC).
* **`_patch_card()`** (`0x800A1734`). Enters a critical section, takes the C0
  table from B0:56, and reads entry 6, the exception handler. The handler's
  `+0x70` / `+0x74` hold the `lui` / `addiu` pair that the kernel's InitCard
  (B0:4A) put there: the address of the kernel's early memory-card routine
  (0x641C). `_patch_card` rebuilds that address from the two words, copies the
  first stub (`lui $v0,0xA001 / addiu $v0,$v0,0xDFAC / jr $v0 / nop / nop`)
  over the five words at routine + 0x28 (the routine's I_MASK test), stores
  routine + 0x3C in 0xDFFC and flushes the cache.
* **`func_800A16C8`, the second body** (runs at 0xDFAC, with `$v1` =
  0x1F800000 from the kernel routine). Repeats the I_MASK test the stub
  replaced (bit 7, the controller and card interrupt; when it is not enabled,
  `jr $ra`, as the kernel's exit does), then waits while bit 7 of the port's
  status register 0x1F801044 is set, then jumps to the address in 0xDFFC. So
  the kernel's early card routine does not go on to handle and acknowledge the
  interrupt while that bit is set. (Bit 7 is the port's /ACK input as the
  hardware is publicly documented; the code shows only the bit.)
* **`_patch_card2()`** (`0x800A17C8`). Enters a critical section (it does not
  leave it; `InitCARD` does), reads B0-table entry 0x5B (`ChangeClearPad`) and
  copies the second stub (`lui $t0,0xA001 / addiu $t0,$t0,0xDF80 / jalr $t0 /
  nop / nop`) over the five words at `ChangeClearPad` + 0x9C8, then flushes
  the cache. Those words (0x4D98) are in the kernel's memory-card interrupt
  handler 0x4D6C (the `func2` that InitCard installs in the card's queue entry
  at 0x7540): `lhu $t7,0xA($v1) / lui $t0,0 / or $t8,$t7,$v0 / ori
  $t9,$t8,0x12 / sh $t9,0xA($v1)`, that is control register 0x1F80104A |=
  select | 0x12.
* **`func_800A169C`, the first body** (runs at 0xDF80, with the kernel's `$v1`
  = 0x1F801040 and `$v0` = 0 or 0x2000). The same five kernel instructions in
  the same registers, then a countdown of 0x28 turns in `$t0`, which ends at
  0, the value the kernel's next `lw $t0,0x7514($t0)` needs: a delay between
  selecting the card and the step that talks to it.
* **`_patch_card_info()`** (`0x800A1658`). Reads B0-table entry 0x5B and
  zeroes one instruction word at `ChangeClearPad` + 0x1988, then flushes the
  cache. That word (0x5D58) is `sb $zero,0($v0)`, with `$v0` = 0x1F801040, in
  the fourth step of the kernel's card status sequence (the steps at 0x5BA4,
  0x5C24, 0x5C58 and 0x5D48 send 0x81 + port, send 0x52, read the flag byte,
  and read the byte that must be 0x5A): after the patch the last step no
  longer writes one more byte to the data register.
* **`_ExitCard()`** (`0x800A1878`). Inside a critical section it reads C0-table
  entry 6 (the exception handler), copies the three zero words at `D_800A18E8`
  over handler + 0x70, + 0x74 and + 0x78, and flushes the cache. That is the
  start of the handler's 16-word patch area (0xCF0; `nop`s in the ROM), which
  InitCard (B0:4A, routine 0x63BC) fills with `lui $v0,0 / addiu
  $v0,$v0,0x641C / jalr $v0 / nop`: `_ExitCard` takes the call to the early
  card routine out again. Its one caller is `StopCARD`.

The first body's five words occur once in the SCPH-1001 kernel, at kernel RAM
0x4D98, where the second stub goes; the second body's first three words occur
once, at kernel RAM 0x6444, where the first stub goes (routine 0x641C +
0x28). So each body begins with the kernel instructions its stub overwrites.

## libmcrd

The game drives the memory card through `card.c`, which calls nine of
libmcrd's functions: `MemCardInit` and `MemCardStart` (from `card_init`,
`0x80024E3C`), `MemCardAccept`, `MemCardSync`, `MemCardFormat`,
`MemCardGetDirentry`, `MemCardReadFile`, `MemCardCreateFile` and
`MemCardWriteFile`. The objects' other functions are linked with them, and the
game does not call them itself.

### INIT.OBJ

* **`MemCardInit(val)`** (`0x800A1068`): `InitCARD(val)`, `StartCARD()`,
  `_bu_init()`. `card_init` calls it with 0.
* **`MemCardEnd()`** (`0x800A1098`): `StopCARD()`.

Their bodies are word for word those of BIOS.OBJ's `_card_open` and
`_card_close` (below); the bytes alone do not say which pair is which, the
place does ([naming.md](../../../../naming.md) proves the member in place).

### How a command runs

A memory-card command is carried out by a **step function**, `s32 step(s32
*work)`, which the command pushes with `UserFuncOpen` and returns from at
once. `func_800A3510`, the VSync callback `MemCardStart` installs, runs the
top step once per vertical blank until it returns non-zero. A step can push
another step, which then runs to completion first: that is how
`MemCardReadFile`'s step runs the existence query and then the transfer. When
the last step returns, the command is complete: its number and result move to
`D_800EA950` and the user's callback is called with them. `MemCardSync` waits
for or polls that completion. The synchronous commands (`MemCardGetDirentry`,
`MemCardCreateFile`, `MemCardDeleteFile`, `MemCardFormat`, `MemCardUnformat`)
do their work in the call itself and refuse while an asynchronous one runs.

### The record `D_800EA960` (`McrdGlobal`)

LIBMCRD.OBJ's state is one record at `.bss+0x30`, 0x58 bytes, of type
`McrdGlobal` (`include/psyq_private.h`). Every file of the object declares it
through that type.

| Offset | Member | Volatile | Meaning |
|---|---|---|---|
| `+0x00` | `cmd` | yes | The running command, libmcrd.h's `McFunc*`: 1 `MemCardExist`, 2 `MemCardAccept`, 3 `MemCardReadFile`, 4 `MemCardWriteFile`, 5 `MemCardReadData`, 6 `MemCardWriteData`. 0 = idle. Each asynchronous command stores its number and refuses while it is positive; each synchronous one refuses while it is not 0. `func_800A3510` moves it to `D_800EA950[0]` and clears it. |
| `+0x04` | `rslt` | yes | The command's result (`McErr*`), written by the step functions, mostly as `func_800A34BC(event)`; `func_800A3510` moves it to `D_800EA950[1]`. |
| `+0x08` | `done` | yes | 1 when the command has completed (`func_800A3510`); `MemCardStart` and each command clear it, and so does `MemCardSync` when it reports. |
| `+0x0C` | `mask` | yes | A bit per card, `1 << chan`. `MemCardStart` clears it; `func_800A1D94` sets the card's bit when the existence query answers `McErrNewCard`; `MemCardGetDirentry`, `MemCardCreateFile` and `MemCardDeleteFile` set it before they touch the card. While the bit is clear, `func_800A1A10` reports a completed query as `McErrNewCard`. |
| `+0x10` | `chan` | no | The card the command works on, port << 4 \| card. |
| `+0x14` | `fd` | no | The file `MemCardOpen` holds open, or the file a file step has open; -1 = none (`MemCardStart`, `MemCardClose` and the file steps write -1). |
| `+0x18` | `ofs` | no | The transfer's byte offset in the file. |
| `+0x1C` | `bytes` | no | The transfer's length. |
| `+0x20` | `adrs` | no | The transfer's buffer. |
| `+0x24` | `file[0x20]` | no | The device and file name, `"buXX:"` and the name (`_make_file_discrpt`, then `strcat`). |
| `+0x44` | `func` | no | The `MemCardCallback` function, called by `func_800A3510` with (command, result). |
| `+0x48` | `new_card[2]` | no | A flag per port, indexed `chan >> 4`. `MemCardStart` writes 1 to both; `func_800A1A10` writes 1 after a new-card or no-card result and 0 after any other. |
| `+0x50` | `frames[2]` | yes | A counter per port: `func_800A3510` counts both up every vertical blank; `func_800A1A10` takes the port's (into `D_800B8450`) and zeroes it when a query starts; `MemCardStart` writes 0. |

The functions store the transfer's three parameters in the order `ofs`,
`adrs`, `bytes`, though the record lays them out `ofs`, `bytes`, `adrs`.

Which members are volatile is fixed by the bytes, not chosen: `MemCardOpen`
and `MemCardGetDirentry` match only with `cmd`, `rslt` and `done` volatile and
`chan` and `fd` plain; `MemCardStart` fixes `fd` and `new_card` as plain and
`mask` and `frames` as volatile; `func_800A1D94` fixes `rslt` and `mask` as
volatile and `chan` as plain; the file steps need `rslt` volatile and `fd`
plain; `MemCardCallback` needs `func` plain. No function decides `ofs`,
`bytes` or `adrs`, which are declared plain. How a member's kind shows in the
object's code is in [codegen.md](codegen.md) §"Volatile".

The object's other data:

| Symbol | Where | Meaning |
|---|---|---|
| `D_800EA930` | `.bss+0x00` | `func_800A1A10`'s retry count. |
| `D_800EA934` | `.bss+0x04` | `func_800A1A10`'s card event. |
| `D_800EA938` | `.bss+0x08` | Cleared by `func_800A1D94` in state 0; nothing in the image reads it. |
| `D_800EA93C` | `.bss+0x0C` | `func_800A1D94`'s retry count for the card load. |
| `D_800EA940` | `.bss+0x10` | Cleared by `func_800A1D94` in state 0; nothing in the image reads it. |
| `D_800EA944` | `.bss+0x14` | `func_800A1D94`'s last card event; read back only as `func_800A34BC`'s argument. |
| `D_800EA948` | `.bss+0x18` | `func_800A1D94`'s new-card flag: 1 once the existence query answered `McErrNewCard`. |
| `D_800EA950` | `.bss+0x20` | The last completed command `[0]` and its result `[1]`, which `MemCardSync` reports. |
| `D_800EA9B8` | `.bss+0x88`, after the record | The user's callback while `MemCardOpen`, `MemCardGetDirentry` or `MemCardDeleteFile` waits for its own check: `MemCardCallback(0)`'s return value, put back afterwards. A variable of its own, not a member: the code reaches it through its own symbol. |
| `D_800B8450` | `.data+0x00` | The port's `frames` count, taken when `func_800A1A10` starts a query. |
| `D_800B8454` | `.data+0x04` | The port's `new_card` flag, taken when `func_800A1A10`'s event arrives. |
| `D_800B8458` | `.data+0x08` | `func_800A22C0`'s retry count. |
| `D_800B845C` | `.data+0x0C` | `func_800A24B4`'s retry count. |
| `D_800B8460` | `.data+0x10` | Cleared by `func_800A2708` in state 0; nothing in the image reads it. |
| `D_800B8464` | `.data+0x14` | Cleared by `func_800A2960` in state 0; nothing in the image reads it. |

The object's messages, all printed through `printf`, are `D_80013964`
`"Access Denied. : event multiple open\n"`, `D_80013A60` `"… file already
open.\n"`, `D_80013A88` `"… file not open.\n"`, `D_80013AAC` `"… invalid data
size align\n"`, `D_80013AD8` `"… invalid offset value align\n"` and
`D_80013B08` `"… system busy\n"`; `func_800A1A10` prints `D_8001398C`
`"error"`.

### LIBMCRD.OBJ

| Offset | Address | Function | File |
|---|---|---|---|
| `+0x000` | `0x800A18F8` | `MemCardStart` | `libmcrd_libmcrd_1.c` |
| `+0x070` | `0x800A1968` | `MemCardStop` | `libmcrd_libmcrd_2.c` |
| `+0x0B0` | `0x800A19A8` | `MemCardExist` | `libmcrd_libmcrd_2.c` |
| `+0x118` | `0x800A1A10` | `func_800A1A10` (static; `MemCardExist`'s step) | `libmcrd_libmcrd_3.c` |
| `+0x434` | `0x800A1D2C` | `MemCardAccept` | `libmcrd_libmcrd_4.c` |
| `+0x49C` | `0x800A1D94` | `func_800A1D94` (static; `MemCardAccept`'s step) | `libmcrd_libmcrd_5.c` |
| `+0x744` | `0x800A203C` | `MemCardOpen` | `libmcrd_libmcrd_5.c` |
| `+0x8CC` | `0x800A21C4` | `MemCardClose` | `libmcrd_libmcrd_5.c` |
| `+0x910` | `0x800A2208` | `MemCardReadData` | `libmcrd_libmcrd_6.c` |
| `+0x9C8` | `0x800A22C0` | `func_800A22C0` (static; `MemCardReadData`'s step) | `libmcrd_libmcrd_7.c` |
| `+0xB04` | `0x800A23FC` | `MemCardWriteData` | `libmcrd_libmcrd_8.c` |
| `+0xBBC` | `0x800A24B4` | `func_800A24B4` (static; `MemCardWriteData`'s step) | `libmcrd_libmcrd_9.c` |
| `+0xCF8` | `0x800A25F0` | `MemCardReadFile` | `libmcrd_libmcrd_9.c` |
| `+0xE10` | `0x800A2708` | `func_800A2708` (static; `MemCardReadFile`'s step) | `libmcrd_libmcrd_9.c` |
| `+0xF50` | `0x800A2848` | `MemCardWriteFile` | `libmcrd_libmcrd_9.c` |
| `+0x1068` | `0x800A2960` | `func_800A2960` (static; `MemCardWriteFile`'s step) | `libmcrd_libmcrd_9.c` |
| `+0x11A8` | `0x800A2AA0` | `MemCardGetDirentry` | `libmcrd_libmcrd_9.c` |
| `+0x1404` | `0x800A2CFC` | `MemCardCallback` | `libmcrd_libmcrd_9.c` |
| `+0x1418` | `0x800A2D10` | `MemCardSync` | `libmcrd_libmcrd_9.c` |
| `+0x1534` | `0x800A2E2C` | `MemCardCreateFile` | `libmcrd_libmcrd_9.c` |
| `+0x1630` | `0x800A2F28` | `MemCardDeleteFile` | `libmcrd_libmcrd_9.c` |
| `+0x1A9C` | `0x800A3394` | `MemCardFormat` | `libmcrd_libmcrd_10.c` |
| `+0x1B10` | `0x800A3408` | `MemCardUnformat` | `libmcrd_libmcrd_11.c` |
| `+0x1BC4` | `0x800A34BC` | `func_800A34BC` (static) | `libmcrd_libmcrd_11.c` |
| `+0x1C18` | `0x800A3510` | `func_800A3510` (static; the VSync callback) | `libmcrd_libmcrd_12.c` |
| `+0x1CC4` | `0x800A35BC` | `_make_file_discrpt` (static) | `libmcrd_libmcrd_13.c` |

`_make_file_discrpt` carries the name release 4.0 exports for the same bytes
(`sdk-earlier`). The other eight statics have no name and keep their
addresses: no Sony text names them and no release exports their bytes. The
words the C's comments use for them are not names; release 4.0's functions
of those words are other code (the register's row for each says so).

In this release `MemCardUnformat` is part of LIBMCRD.OBJ; from release 4.6 on
it is an object of its own (`unformat.o`). Release 4.6's `delete.o` is a
different build of `MemCardDeleteFile`.

#### Starting and stopping

* **`MemCardStart()`**. Clears `mask` and `func`; `UserFuncInit()`; clears
  `cmd`, `rslt` and `done`; zeroes both `frames`; `fd` = -1; sets both
  `new_card` to 1; `_card_start()`; then `VSyncCallbacks(7, func_800A3510)`
  (libetc INTR.OBJ), which is what makes the queue run. Its one caller is
  `card_init` (`0x80024E3C`).
* **`MemCardStop()`**. Waits until `cmd` is 0 (`func_800A3510` clears it), then
  `VSyncCallbacks(7, 0)` removes the callback and `_card_stop()` stops the
  driver.

#### The asynchronous commands

Each refuses with `D_80013964` ("event multiple open") or another message and
returns 0 while `cmd` is positive, and otherwise stores its number in `cmd`,
clears `rslt` and `done`, saves its arguments in the record, pushes its step
function and returns 1.

* **`MemCardExist(chan)`**: `cmd` = 1, `chan` saved, `func_800A1A10` pushed.
* **`MemCardAccept(chan)`**: `cmd` = 2, `chan` saved, `func_800A1D94` pushed.
  `MemCardOpen`, `MemCardGetDirentry` and `MemCardDeleteFile` carry this body
  expanded in line (the C's helper `mcrd_accept`).
* **`MemCardReadData(adrs, ofs, bytes)`** reads from the file `MemCardOpen`
  holds. It refuses, in this order, when no file is open (`fd` negative, "file
  not open"), when a command is running, when `bytes` is not a multiple of 128
  (one card frame) and when `ofs` is not; otherwise `cmd` = 5, `ofs`, `adrs`,
  `bytes` saved, `func_800A22C0` pushed.
* **`MemCardWriteData(adrs, ofs, bytes)`** is its twin: the same four refusals
  in the same order, `cmd` = 6, `func_800A24B4` pushed.
* **`MemCardReadFile(chan, file, adrs, ofs, bytes)`** opens, reads and closes
  a file in one command. It refuses, in this order, when a command is running
  ("system busy"), when `MemCardOpen` holds a file ("file already open"), and
  on a size or an offset that is not a multiple of 128. Otherwise it writes
  the device name into the record's `file` (`_make_file_discrpt(chan, …)`,
  then `strcat` of the name), sets `cmd` = 3, saves `ofs`, `adrs`, `bytes` and `chan`, and
  pushes `func_800A2708`. The game's `card_file_read` (`0x800250B8`) and
  `card_save_read` (`0x80025110`) call it.
* **`MemCardWriteFile(chan, file, adrs, ofs, bytes)`** is its twin, with `cmd`
  = 4 and `func_800A2960`. The game's `card_file_write` (`0x8002520C`) and
  `card_save_write` (`0x800252B0`) call it.
* **`MemCardOpen(chan, file, flag)`** is synchronous: it returns -1 with "file
  already open" when `fd` is not negative. Otherwise it writes the device name
  into the record's `file`, saves `chan`, and test-opens the name read-only (`open(name,
  1)`). While that fails it checks the card: the user's callback is taken out
  with `MemCardCallback(0)` and kept in `D_800EA9B8`, `MemCardAccept(chan)`
  runs (expanded in line), `MemCardSync(0, 0, &rslt)` waits for it, and the
  callback is put back. Result 3 (`McErrNewCard`) tries the open again with no
  limit; 2 (`McErrCardInvalid`) tries again up to four more times; any other
  result is returned, 0 as 5 (`McErrFileNotExist`: the card is fine and the
  open still failed). When the test open succeeds its handle is closed,
  `_clr_card_event()` runs, `fd` = `open(name, flag | 0x8000)` (the BIOS's
  no-wait bit: later reads and writes return at once and finish by event) and
  0 is returned. `fd` is not checked after the second open.
* **`MemCardClose()`**: if `fd` is not negative, `close(fd)` and `fd` = -1.

#### The step functions

Each takes a pointer to its four work words (`USERFUNC.OBJ` zeroes them) and
keeps its state number in the first. Each returns 1 when the command has its
answer in `rslt`, 0 while it is waiting.

* **`func_800A1A10`**, `MemCardExist`'s step, also pushed by the other steps
  before they touch the card.
  * State 0: clears its event and retry count, takes the port's `frames` count
    into `D_800B8450` and zeroes it, and falls into state 10.
  * State 10: `_clr_card_event()`, `_card_info(chan)`, state 11.
  * State 11: waits for a card event (`_chk_card_event`), takes it
    (`_get_card_event`) and keeps the port's `new_card` flag in `D_800B8454`.
    On new card (4): when the flag was clear and the taken count is below
    0x80, the card is cleared (`_clr_card_event()`, `_card_clear(chan)`) and
    the step goes to state 21; otherwise `new_card` = 1 and the answer is
    `McErrNewCard`. On I/O end (0): `new_card` = 0, and the answer is
    `McErrNone`, or `McErrNewCard` when the card's bit in `mask` is clear. On
    time-out (2): `_card_info` again from state 10 until the third failure,
    then `new_card` = 1 and `McErrCardNotExist`. On error (1): again until the
    seventeenth failure, then `McErrCardInvalid` and `new_card` = 0; time-outs
    and errors count on the one counter. Any other event: `new_card` = 0 and
    the answer is the event `| 0x8000`.
  * State 21: waits for the clear's event in the second set
    (`_chk_card_event_x`, `_get_card_event_x`, value dropped) and starts over
    at state 0.
  * Any other state prints `"error"`.
* **`func_800A1D94`**, `MemCardAccept`'s step (the switch's jump table is the
  51 words at 0x80013994).
  * State 0: clears the five statics `D_800EA938` to `D_800EA948` and falls
    into state 1.
  * State 1: pushes `func_800A1A10`, so the existence query runs first; state
    10.
  * State 10, on the query's `rslt`: 0 (`McErrNone`) goes to state 30. 3
    (`McErrNewCard`) sets the new-card flag and the card's bit in `mask`, runs
    `_clr_card_event()` and `_card_clear(chan)`, and goes to state 21.
    Anything else is the answer as it stands.
  * State 21: waits for the clear's event (`_chk_card_event_x`,
    `_get_card_event_x`, value dropped) and falls into state 30.
  * State 30: `_clr_card_event()`, `_card_load(chan)`, state 31.
  * State 31: waits for the load's event (`_chk_card_event`,
    `_get_card_event`). 0 answers 3 (`McErrNewCard`) if the new-card flag is
    set, else 0. 4 runs `_clr_card_event()` and `_card_info(chan)` and goes to
    state 50. 2 goes back to state 1 (the whole query again). 1 loads again
    from state 30, 16 times; after that, and for any other event, the answer
    is `func_800A34BC(event)`.
  * State 50: waits as in 31. Event 0 answers 4 (`McErrNotFormat`); anything
    else goes back to state 1.
  * Any other state returns 0 and changes nothing.
* **`func_800A22C0`**, `MemCardReadData`'s step. State 0 clears the retry
  count `D_800B8458` and falls into state 10. State 10: `lseek(fd, ofs, 0)`
  until it returns `ofs`, `_clr_card_event()`, `read(fd, adrs, bytes)` until
  it returns 0 (accepted), state 30. State 30 returns 0 until
  `_chk_card_event()` is non-zero, then takes the event with
  `_get_card_event()`: a non-zero (error) event goes back to state 10 while
  the retry count, counted first, is below 4, so three retries; after that, or
  on event 0, `rslt` = `func_800A34BC(event)` and the step is done.
* **`func_800A24B4`**, `MemCardWriteData`'s step: the read step with `write`
  and its own retry count `D_800B845C`. Two differences: state 0 only clears
  the count and sets state 10, so the seek and the write start one call later
  than a read's; and the cases stand in the object in the order 0, 30, 10.
* **`func_800A2708`**, `MemCardReadFile`'s step (jump table: 23 words at
  0x80013B2C).
  * State 0: clears `D_800B8460`, pushes `func_800A1A10`, state 10.
  * State 10: a non-zero `rslt` from that query ends the command with it.
    Otherwise the file named in the record is opened with `open(file, 0x8001)`
    (read, no wait) into `fd`; a negative handle ends the command with `rslt`
    = 5 (`McErrFileNotExist`). Otherwise it falls into state 11.
  * State 11: state 20, `func_800A22C0` pushed (the transfer of `bytes` at
    `ofs` into `adrs`).
  * State 20: `rslt` 3 (`McErrNewCard`) runs `_clr_card_event()` and
    `_card_clear(chan)` and goes to state 22; any other result closes the
    file, sets `fd` = -1 and ends the command.
  * State 22: waits for the clear's event (`_chk_card_event_x`,
    `_get_card_event_x`, value dropped) and goes to state 11: the transfer is
    pushed again.
  * Any other state returns 0.
* **`func_800A2960`**, `MemCardWriteFile`'s step (jump table: 23 words at
  0x80013B8C): `func_800A2708` statement for statement, with `func_800A24B4`
  as the transfer and `D_800B8464` as the word cleared in state 0. It opens
  the file with the read step's mode, 0x8001.

The C emits the two file steps' jump tables from `libmcrd_libmcrd_9.c`, with
the four bytes between them as a data stub.

#### The synchronous commands

Each returns -1 with "system busy" (`D_80013B08`) while `cmd` is not 0.

* **`MemCardGetDirentry(chan, name, dir, files, ofs, max)`** lists the files
  whose names fit the pattern `name`. It builds `"buXX:"` + `name` in a local
  buffer, sets the card's bit in `mask`, and walks the directory for at most
  `ofs + max` entries: the first with `firstfile` (`0x800A3638`, libapi
  FIRST.OBJ, after `_clr_card_event()`), the others with `nextfile` on the
  same 40-byte `DIRENTRY`; a null answer ends the walk. Entries with index
  `ofs` or more are copied to `dir[]` (when `dir` is not null) and counted;
  the count goes to `*files` (when `files` is not null) and the function
  returns 0. A null `firstfile` is looked into first: the second set's card
  event (`_get_card_event_x`) through `func_800A34BC` into a local result; 0
  means the card answered and has no such file, and the walk ends. Otherwise
  `firstfile` is tried again, four times in all; after the fourth failure the
  card is checked as `MemCardOpen` checks it (the callback taken out, an
  expanded `MemCardAccept(chan)`, a real call of `MemCardSync(0, 0, &result)`,
  the callback put back) and that result is returned, with `*files` not
  written. The game's `card_dir_read` (`0x80024F70`) calls it with `ofs` 0 and
  `max` 15.
* **`MemCardCreateFile(chan, file, blocks)`** builds `"buXX:"` + `file` in a
  32-byte local that nothing reads afterwards, sets the card's bit in `mask`,
  and calls `_card_create2(chan, file, blocks)` with the bare name. Result 0
  returns 0; -1 gives 7 (`McErrBlockFull`); -2 gives 4 (`McErrNotFormat`); -3
  gives 6 (`McErrAlreadyExist`); 4 gives 2 (`McErrCardInvalid`); anything else
  gives `func_800A34BC(result)`. The C has the call and the mapping inside a
  loop statement that never comes round, because the object's code has the
  layout of a loop ([codegen.md](codegen.md) §"Loops"); release 4.0's build
  of the function has a real retry loop at this place.
* **`MemCardDeleteFile(chan, file)`** builds `"buXX:"` + `file`, sets the
  card's bit in `mask` and tries `open(path, 1)` up to four times. If it never
  opens, the card is checked with the callback taken out (an expanded
  `MemCardAccept(chan)` and an expanded `MemCardSync(0, 0, &rslt)`); a result
  of 0 or 3 becomes 5 (`McErrFileNotExist`), any other is returned. If it
  opens, it is closed, and a loop runs `_clr_card_event()` and `erase(path)`:
  1 means done and returns 0. Otherwise the second set's event decides: 0
  checks the card the same way and returns 4 (`McErrNotFormat`) if that is the
  result, else 0; 1 runs `_clr_card_event()`, `_card_info(chan)` and
  `_get_card_event_x()` twice, checks the card and tries again, and the fourth
  such round in a row returns 2 (`McErrCardInvalid`); 4 clears the error count,
  clears the card (`_card_clear(chan)`, event dropped) and tries again; any
  other event counts an error, and the fourth returns `func_800A34BC(event)`.
* **`MemCardFormat(chan)`**: `_card_format2(chan)`; 0 gives 0, 4 gives 2
  (`McErrCardInvalid`), anything else `func_800A34BC(result)`.
* **`MemCardUnformat(chan)`** fills a 128-byte frame with 0xFF and writes it
  to blocks 0 to 14 of the card, each with `_clr_card_event()`,
  `_new_card()`, `_card_write(chan, block, frame)` and `_get_card_event_x()`.
  The first non-zero event returns 0; fifteen clean writes return 1.

`MemCardSync` and `MemCardAccept` each stand once as functions and are
expanded in line in some of the functions after them: `MemCardOpen` expands
`MemCardAccept` and calls `MemCardSync`, `MemCardGetDirentry` the same,
`MemCardDeleteFile` expands both; `MemCardCallback` is called everywhere. The
C carries the expanded bodies as the inline helpers `mcrd_accept` and
`mcrd_sync` ([codegen.md](codegen.md) §"Functions expanded in line").

#### The helpers

* **`MemCardCallback(func)`** stores `func` in the record's `func` and returns
  the value that was there; 0 means no callback.
* **`MemCardSync(mode, cmds, rslt)`** returns -1 when nothing is registered
  (`cmd` 0 and `done` clear). Mode 0 waits in a busy loop for `done`; any
  other mode returns 0 while `done` is clear, after storing the running
  command and its result so far through the non-null pointers. Once `done` is
  set, `*rslt` and `*cmds` are filled from `D_800EA950` (`[1]`, then `[0]`),
  `done` is cleared and 1 is returned. The wait spins on a flag the VSync
  callback sets; it does not wait for frames.
* **`func_800A34BC(event)`** turns a card event into the library's result
  code: 0 gives `McErrNone` (0), 1 `McErrCardInvalid` (2), 2
  `McErrCardNotExist` (1), 4 `McErrNewCard` (3), anything else the event |
  0x8000 (`McErrExtend`).
* **`func_800A3510()`**, the VSync callback. While `UserFuncComplete()` is 0 it
  runs one step with `UserFuncExecute()`; when the stack is then empty, the
  command is complete: `done` = 1, `cmd` and `rslt` move to `D_800EA950` and
  are cleared, and the user's `func`, if set, is called with the pair. Every
  call counts both `frames` up.
* **`_make_file_discrpt(chan, buf)`** copies the six bytes of `"bu00:"`
  (`D_80013BE8`, with its terminator) into `buf` and sets `buf[2]` = `chan /
  16 + '0'` and `buf[3]` = `chan % 16 + '0'`: the device prefix its callers
  append a file name to.

### USERFUNC.OBJ: the step-function stack

The object keeps a stack of up to four step functions. `UserFuncOpen` is
called from sixteen places in LIBMCRD.OBJ, counting the expanded copies of
`MemCardAccept`.

| Symbol | Where | Meaning |
|---|---|---|
| `D_800B8470` | `.data+0`, initially -1 | The index of the top entry; -1 = the stack is empty. |
| `D_800EA9F0` | `.bss+0x00`, `s32[4][4]` | Each entry's four work words; the step function gets a pointer to its entry's four. |
| `D_800EAA30` | `.bss+0x40`, `s32 (*[4])(s32 *)` | Each entry's step function. |
| `D_80013BF4` | `.rdata+0` | `"libmcrd: event overflow\n"` |

The object's `.bss` is 0x50 bytes and ends at 0x800EAA40, where BIOS.OBJ's
begins.

* **`UserFuncInit()`** (`0x800A3A18`): `D_800B8470` = -1. Called by
  `MemCardStart`.
* **`UserFuncOpen(func)`** (`0x800A3A28`) pushes `func`: with `n` =
  `D_800B8470 + 1`, if `n` is 4 or more it prints "libmcrd: event overflow"
  through `printf` (`0x80021700`) and pushes nothing; otherwise `D_800B8470`
  = `n`, `D_800EAA30[n]` = `func`, and `D_800EA9F0[n][3]` down to `[0]` are
  zeroed.
* **`UserFuncExecute()`** (`0x800A3AA4`) runs the top entry's function once
  with its work words and pops the entry when the function returns non-zero.
  Nothing happens while the stack is empty.
* **`UserFuncComplete()`** (`0x800A3B10`): 1 when the stack is empty, else 0.

`func_800A3510` is the caller of the last two.

### BIOS.OBJ: the card driver's events

Game address = 0x800A3B28 + the offset:

| Offset | Functions | File |
|---|---|---|
| 0x000, 0x014, 0x028, 0x03C | `funcEvSpIOE`, `funcEvSpError`, `funcEvSpTimeout`, `funcEvSpNewcard` | `libmcrd_bios_1.c` |
| 0x050, 0x064, 0x078, 0x08C | `funcEvSpIOEx`, `funcEvSpErrorx`, `funcEvSpTimeoutx`, `funcEvSpNewcardx` | `libmcrd_bios_1.c` |
| 0x0A0 | `_card_open` | `libmcrd_bios_2.c` |
| 0x0D0 | `_card_start` | `libmcrd_bios_3.c` |
| 0x2AC | `_card_close` | `libmcrd_bios_4.c` |
| 0x2CC | `_card_stop` | `libmcrd_bios_5.c` |
| 0x380 | `_clr_card_event` | `libmcrd_bios_6.c` |
| 0x488, 0x560, 0x638, 0x674 | `_get_card_event`, `_get_card_event_x`, `_chk_card_event`, `_chk_card_event_x` | `libmcrd_bios_7.c` |

Its state is sixteen words of `.bss`, in four groups of four. Within each
group the order is I/O end, error, time-out, new card (the event specs
`EvSpIOE` 0x0004, `EvSpERROR` 0x8000, `EvSpTIMOUT` 0x0100, `EvSpNEW` 0x2000):

| Symbols | Where | Meaning |
|---|---|---|
| `D_800EAA40` to `D_800EAA4C` | `.bss+0x00` | The ids of the four **SwCARD** events (class 0xF4000001 = `DescSW \| 0x01`), opened by `_card_start` with mode 0x1000 (`EvMdINTR`) and the handlers `funcEvSpIOE` to `funcEvSpNewcard`. |
| `D_800EAA50` to `D_800EAA5C` | `.bss+0x10` | The ids of the four **HwCARD** events (class 0xF0000011 = `DescHW \| 0x11`), handlers `funcEvSpIOEx` to `funcEvSpNewcardx`. |
| `D_800EAA60` to `D_800EAA6C` | `.bss+0x20` | The first set of flags: each SwCARD handler stores 1 in its own. Volatile. |
| `D_800EAA70` to `D_800EAA7C` | `.bss+0x30` | The second set of flags: each HwCARD handler stores 1 in its own. Volatile. |

A set of flags is read as a bit mask: flag 0 + flag 1 × 2 + flag 2 × 4 + flag 3
× 8, so bit 0 = I/O end, bit 1 = error, bit 2 = time-out, bit 3 = new card.

* **The eight handlers** each store 1 in their own flag and return 0.
* **`_card_open(val)`** (`0x800A3BC8`): `InitCARD(val)`, `StartCARD()`,
  `_bu_init()`. **`_card_close()`** (`0x800A3DD4`): `StopCARD()`. Nothing in
  the program calls either.
* **`_card_start()`** (`0x800A3BF8`). Inside a critical section (left only if
  the enter returned 1) it opens the eight events with `OpenEvent(class,
  spec, 0x1000, handler)`, keeps their ids, enables each with `EnableEvent`,
  and runs `_clr_card_event()`.
* **`_card_stop()`** (`0x800A3DF4`). Inside a critical section, `CloseEvent`
  on the eight ids.
* **`_clr_card_event()`** (`0x800A3EA8`). Drops every pending card event:
  `TestEvent` (`0x800A0538`) on all eight ids, then both sets of flags are
  cleared.
* **`_get_card_event()`** (`0x800A3FB0`). Spins until the first set's mask is
  non-zero, calls `TestEvent` on the four HwCARD ids, clears the first set's
  four flags and returns the mask shifted right once: **0 = I/O end, 1 =
  error, 2 = time-out, 4 = new card**. These are the event numbers the step
  functions and `func_800A34BC` work with. Seven calls in the image:
  `func_800A1A10`, `func_800A1D94` (twice), `func_800A22C0`, `func_800A24B4`,
  and LOW.OBJ's `_card_format2` and `_card_create2` after `_card_load`.
* **`_get_card_event_x()`** (`0x800A4088`). The same on the second set: spins
  on `D_800EAA70` to `D_800EAA7C`, calls `TestEvent` on the four SwCARD ids,
  clears the second set and returns its mask shifted right once. 23 calls in
  the image; 14 of them are in LOW.OBJ, which waits with it after every frame
  it reads or writes.
* **`_chk_card_event()`** (`0x800A4160`) and **`_chk_card_event_x()`**
  (`0x800A419C`): the first or second set's mask, unshifted, without waiting
  and without clearing; 0 = nothing has arrived.

Each of the two waits tests the events of the other class: `_get_card_event`
waits on the SwCARD handlers' flags and calls `TestEvent` on the HwCARD ids,
and `_get_card_event_x` the reverse. Both are busy loops with no time-out:
they return only when a handler has run.

### LOW.OBJ

* **`_card_format2(chan)`** (`0x800A41E8`) formats a card: the 15 directory
  frames 1 to 15, each a cleared entry with state 0xA0 (free), size 0 and next
  0xFFFF, kept in `D_800EAA80` and written out; the 20 frames 16 to 35, each
  holding one -1 from `D_800EAC60`; then the header frame 0, "MC"; last
  `_card_load` and `_get_card_event`, up to 8 times. Each frame goes out with
  an XOR checksum in byte 127 and up to 8 tries.
* **`_card_create2(chan, name, blocks)`** (`0x800A45B8`) reads the header
  frame (it must begin "MC") and the 15 directory frames, marks the blocks of
  every live file and frees the rest, refuses a name that exists (-3, with a
  message) or too few free blocks (-1), claims the first free blocks for the
  new file, writes the claimed frames back, and ends with `_card_load` and
  `_get_card_event`, up to 8 times. A frame that fails to read or write
  returns its error; a header without "MC" returns -2.

## libc2 — STRCAT.OBJ

* **`strcat(dst, src)`** (`0x800A38E8`, `libc2_strcat.c`). Appends `src`,
  terminator included, to the end of `dst` and returns `dst`. It returns 0
  without writing when `dst` or `src` is null, and when `dst + strlen(dst) ==
  src + strlen(src)` (`strlen` is `0x8008B848`): the two strings then end at
  the same byte, so `src` lies inside `dst`, and the copy would overwrite its
  own terminator and not stop. The end of `dst` is found again by a scan, not
  taken from the `strlen` result, and the strings are read as signed chars.
  Compiled C, not hand assembly. Its six callers are LIBMCRD.OBJ's
  (`MemCardOpen`, `MemCardReadFile`, `MemCardWriteFile`, `MemCardGetDirentry`,
  `MemCardCreateFile`, `MemCardDeleteFile`), each appending a file name to the
  `"buXX:"` prefix `_make_file_discrpt` wrote.

## Open questions

* **`D_800EA938`, `D_800EA940`, `D_800B8460` and `D_800B8464`** are cleared by
  the step functions and read by nothing in the image; what Sony's source
  meant them for is not known.
* **The two waits test the other class's events** (BIOS.OBJ above); why is not
  known.
* **`func_800A2960` opens its file with mode 0x8001**, the read step's.
  Whether the BIOS card driver looks at the access bits is not known.
