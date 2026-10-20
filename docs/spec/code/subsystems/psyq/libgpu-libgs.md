# libgpu and libgs, with the libapi heap and libc2 strings before them

This page covers the first stretch of Sony's library code in the main
executable, from `0x8008B388` to `0x8008F047`: libapi's critical-section
calls and its second heap (`i_heap2.o`, `malloc2.o`, `free2.o`), libc2's
string and memory functions, most of libgpu's `sys.o` (the settings, the
drawing-environment builder, the routines behind the GPU function table, the
command queue's front door, reset, sync, the timeout and the four unqueued
`*2` calls), libgpu's `LoadClut` and libgs's `gs_001.o`. The program links
release 4.5 of Sony's libraries, and every member named here lies whole in
the image ([docs/naming.md](../../../../naming.md) says how that was
measured). Each function's file and recipe are in [index.md](index.md); how
the library code is compiled, and which C shapes decide its bytes, is in
[codegen.md](codegen.md). The rest of `sys.o` (`ResetGraph`, `PutDispEnv`,
the queue's runner and the environment `GEnv`) and libgs's double buffer are
in [psyq-libs.md](../psyq-libs.md).

Statements about the GPU's and the DMA controller's commands (what GP1
`0x10` with parameter 3 answers, what a CHCR word starts) come from the
PlayStation's documented command set and are marked "inferred": the code only
shows which words it writes.

## Before the library code

The game's own code ends at `0x8008B387`, with `btl_effect.c`. Its
`BtlMixColNumberSet` (`0x8008B27C`), right before libapi, is game code and
not a library function: the battle's effect tint, a switch from an effect
number to a 15-bit colour handed to `sprite_mix_col_set` (`0x8001A904`).
[battle.md](../battle.md) §"Tint by effect" has the mapping.

## libapi — the critical section (`a36.o`, `a37.o`)

`EnterCriticalSection` (`0x8008B3B8`, `libapi_a36.c`) and
`ExitCriticalSection` (`0x8008B3C8`, `libapi_a37.c`) are four words each:
`li $a0,1` (2 for the exit), `syscall`, `jr $ra`, `nop`. `syscall` has no C
spelling, so both stay `INCLUDE_ASM`, and `config/asm_only.txt` and
`config/bios_thunks.txt` list them as `syscall` routines
([hand-assembly.md](../../hand-assembly.md)). They sit among libapi's kernel
call stubs, `0x8008B388` to `0x8008B437`: `InitHeap`, `FlushCache`,
`_bu_init`, these two, then `open`, `lseek`, `read`, `write`, `close` and
`Krom2RawAdd`.

By the SDK reference the enter call disables interrupts and returns 1 when
they were enabled (0 inside a critical section), and the exit call enables
them. The C's callers use the result that way: libcard's `InitCARD` and
`StartCARD` (`libcard_init.c`) and libmcrd's `_card_start` (`0x800A3BF8`,
`libmcrd_bios_3.c`) keep it and call `ExitCriticalSection` only when it was
1. What the kernel does with the call is not read in this repository.

## libapi — the second heap (`i_heap2.o`, `malloc2.o`, `free2.o`)

Three members back to back, `0x8008B438` to `0x8008B6E7`: `i_heap2.o` (20
words), `malloc2.o` (136) and `free2.o` (16).

| Address | Name | Member | File |
|---|---|---|---|
| `0x8008B438` | `InitHeap2(head, size)` | `i_heap2.o` | `libapi_i_heap2.c` |
| `0x8008B488` | `malloc2(size)` | `malloc2.o` `+0x000` | `libapi_malloc2.c` |
| `0x8008B640` | `_ExpAndAlloc(size)` | `malloc2.o` `+0x1B8` | `libapi_malloc2.c` |
| `0x8008B6A8` | `free2(p)` | `free2.o` | `libapi_free2.c` |

`_ExpAndAlloc` is a static of `malloc2.o` that release 4.5 gives no name. It
is named `sdk-earlier`: releases 3.7 and 4.0 export its identical bytes under
that name, and no other name fits (the register's row).

The heap's state is the four words of `i_heap2.o`'s `.bss`. The tree declares
them as pointers to header words; the names are the ones the member's
relocations give them:

| Address | Sony's name | What it holds (read from the four functions) |
|---|---|---|
| `D_800E55B8` | `_TopAllocArea` | The heap's first block header (`InitHeap2`'s `head`). |
| `D_800E55BC` | `_AllocRestBlockTop` | The first word of the unallocated rest: the word behind the end marker. |
| `D_800E55C0` | `_max_heap` | The heap's end: `head` plus `size` rounded down to a word, plus 4. |
| `D_800E55C4` | `_Candidate` | The lowest block header known to be free, or 0: where the next search starts. |

The heap is a run of blocks. A block is one header word and its user words.
The header holds the user size in bytes (a multiple of 4), with bit 0 set
while the block is free; the header value -2 (`0xFFFFFFFE`) is the end
marker, and everything behind it up to `_max_heap` is the unallocated rest.

- **`InitHeap2(head, size)`** writes the end marker at `head`, sets
  `_TopAllocArea` to `head`, `_max_heap` to `size / 4 + 1` words past `head`,
  clears `_Candidate` and sets `_AllocRestBlockTop` to `head + 4`.
- **`malloc2(size)`** returns 0 for a size of 0. It rounds `size` up to a
  word and walks the blocks from `_Candidate`, or from `_TopAllocArea` when
  `_Candidate` is 0:
  - a free block: `_Candidate` is set to it when it was 0. A block of exactly
    `size` is taken (bit 0 cleared). A larger one is split: its header becomes
    `size`, and a new header behind the user words gets the remainder less the
    header word, still marked free. A smaller one is looked at together with
    its successor: if the successor is the end marker, the block is dropped
    back into the rest (its header becomes the marker, `_AllocRestBlockTop`
    the word behind it); if the successor is free, the two are merged (header
    + 4 + the successor's size); otherwise the walk moves on to the successor.
    After a drop or a merge the same block is examined again.
  - an allocated block: stepped over.
  - the end marker: `_ExpAndAlloc(size)`. On success `_Candidate` becomes the
    new end marker's header (`_AllocRestBlockTop - 4`) and the new block's
    user area is returned. On failure the walk runs once more from
    `_TopAllocArea` with `_Candidate` cleared, so that free blocks below the
    old candidate are merged and tried; a second failure returns 0.

  The result is the address of the user words (header + 4). A request that
  fits nowhere leaves the heap as the two walks left it: merged, with a
  trailing free block dropped into the rest.
- **`_ExpAndAlloc(size)`** carves a block from the rest. It returns 0 when
  `size` is larger than the rest, taken as `_max_heap - _AllocRestBlockTop`
  in whole words, less 8 bytes. Otherwise it writes `size` into the header at
  `_AllocRestBlockTop - 4` (replacing the end marker), moves
  `_AllocRestBlockTop` past the user words and one header, writes the marker
  -2 into that header and returns the address of the block's header, not of
  its user words: `malloc2` adds the 4.
- **`free2(p)`** sets bit 0 in the header below `p` and, when that header
  lies below `_Candidate`, makes it the candidate.

The heap's only user is the game's `xmalloc.c`: `xmalloc_heap_reset`
(`0x80022E9C`) calls `InitHeap2`, `xmalloc_heap_alloc` (`0x80022F4C`) is the
one caller of `malloc2`, `xmalloc_heap_free` (`0x80022F88`) calls `free2`,
`xmalloc_heap_set_limit` (`0x80022FA8`) stores into `_max_heap`, and
`xmalloc_heap_check_start` (`0x80022EE4`) tests `_TopAllocArea`.

## libc2 — the string and memory functions

Six libc2 members follow the heap, `0x8008B6E8` to `0x8008B887`, each in its
own file `libc2_<member>.c`. These are libc2's compiled C functions, not the
four-word kernel call stubs that libc ships under the same names. A seventh,
`memcpy` (`0x8008E7C8`, 16 words, `libc2_memcpy.c`), lies after libgpu's
`sys.o` and libapi's `GPU_cw` (`0x8008E7B8`); `sys.o`'s `PutDrawEnv`,
`DrawOTagEnv` and `GetDrawEnv` copy environments with it.

| Address | Name | Member | Words |
|---|---|---|---|
| `0x8008B6E8` | `bcopy(src, dst, n)` | `bcopy.o` | 16 |
| `0x8008B728` | `bzero(p, n)` | `bzero.o` | 12 |
| `0x8008B758` | `memset(p, c, n)` | `memset.o` | 12 |
| `0x8008B788` | `strcmp(s1, s2)` | `strcmp.o` | 28 |
| `0x8008B7F8` | `strcpy(dst, src)` | `strcpy.o` | 20 |
| `0x8008B848` | `strlen(s)` | `strlen.o` | 16 |

Each member's size is a multiple of 16 bytes, so `bcopy`, `strcmp`,
`strcpy` and `strlen` end in 12 bytes of padding. The library's plain `char`
is signed: `strlen` reads with `lb` ([codegen.md](codegen.md)). None of the
six reads or writes a global.

- **`bcopy(src, dst, n)`** copies `n` bytes from `src` to `dst`, low address
  first, and returns `src`; a null `src` copies nothing and returns 0.
- **`bzero(p, n)`** clears `n` bytes at `p` and **`memset(p, c, n)`** fills
  them with `c`; each returns `p`, or 0 for a null `p` or `n <= 0`.
- **`strcmp(s1, s2)`** compares two strings as signed chars. A null pointer
  is handled before anything is read: both null gives 0, `s1` null -1, `s2`
  null 1. Otherwise it returns 0 at the shared terminator, or the difference
  of the first pair of characters that differ. It is called by the
  device-table walks of libapi's `firstfile` (`libapi_first.c`) and of the
  libapi, libcard and libmcrd members that carry the same walk, libcd's
  directory search (`func_80092AC4`, `libcd_iso9660.c`), libmcrd's
  `_card_create2` (`0x800A45B8`), and the game's `card_dir_find`
  (`0x80025040`), `cd_dir_find` (`0x80021164`) and `build_stamp_compare`
  (`0x80013D08`).
- **`strcpy(dst, src)`** copies `src`, terminator included, to `dst` and
  returns `dst`; a null `dst` or `src` copies nothing and returns 0. Its
  callers are `card_save_read` (`0x80025110`), `card_save_write`
  (`0x800252B0`) and `card_save_records_head_read` (`0x8007473C`).
- **`strlen(s)`** returns the number of bytes before the terminating 0 of
  `s`, and 0 for a null `s`. Its callers are `card_save_title_set`
  (`0x8001BF50`, three sites), `sim_console_write` (`0x80021740`),
  `func_800217AC` (the second host-console write), `fmt_put_string`
  (`0x80021AA0`) and `strcat` (`0x800A38E8`, two sites).

## libgpu — `sys.o`

### The member

Release 4.5's `sys.o` lies whole at `0x8008B888`; its text runs to
`0x8008E7B7`, and its version string (`.rdata` at `0x80012FA4`) names
revision 1.140 of `sys.c`. Release 4.4's SYS.OBJ is the same bytes. The 4.6
and 4.7 `sys.o` carry the same revision but are twelve words shorter: their
counterparts of `func_8008D888` and `func_8008D8AC` keep no shadow byte (four
and three words shorter) and their counterpart of `func_8008DEAC` does not
clear the shadow table (five words shorter), so from `func_8008D888` on their
offsets are lower than this program's. The tree compiles runs of the member
with different flags, so it is in six files, `libgpu_sys_1.c` to
`libgpu_sys_6.c` (each header lists its functions; `config/cc_flags.txt` has
the flags).

The member exports these functions: `ResetGraph` (`0x8008B888`),
`SetGraphDebug` (`0x8008B9FC`), `SetGraphQueue` (`0x8008BA58`),
`GetGraphDebug` (`0x8008BAFC`), `DrawSyncCallback` (`0x8008BB0C`),
`SetDispMask` (`0x8008BB6C`), `DrawSync` (`0x8008BC04`), `ClearImage`
(`0x8008BD88`), `ClearImage2` (`0x8008BE18`), `LoadImage` (`0x8008BEB0`),
`StoreImage` (`0x8008BF10`), `MoveImage` (`0x8008BF70`), `ClearOTag`
(`0x8008C028`), `ClearOTagR` (`0x8008C0F0`), `DrawPrim` (`0x8008C19C`),
`DrawOTag` (`0x8008C1F8`), `PutDrawEnv` (`0x8008C268`), `DrawOTagEnv`
(`0x8008C328`), `GetDrawEnv` (`0x8008C400`), `PutDispEnv` (`0x8008C434`),
`GetDispEnv` (`0x8008C92C`), `GetODE` (`0x8008C960`), `SetDrawArea`
(`0x8008C990`), `SetDrawOffset` (`0x8008CA10`), `SetDrawEnv` (`0x8008CA50`),
`LoadImage2` (`0x8008E350`), `StoreImage2` (`0x8008E43C`), `MoveImage2`
(`0x8008E528`), `DrawOTag2` (`0x8008E66C`) and `_GPU_ResetCallback`
(`0x8008E768`).

Everything else in it is static and has no name in release 4.5. Three
statics carry an earlier release's name (`sdk-earlier`): releases 3.3 to 4.0
export the identical bytes of `_param` (`0x8008D948`), `_addque`
(`0x8008D978`) and `set_alarm` (`0x8008E138`) under those names. The others
keep their addresses. The C's comments give most of them the name of a
SYS.OBJ function of releases 2.6 to 4.0, but in those releases each such
name stands over other code, and no release from 2.6 to 4.7 exports these
bytes under any name. One more
is held: releases 2.6 to 4.0 export the bytes of `func_8008D0A4` as
`_status`, but only four of its six words are free of relocations and
libpress has the same bytes as `MDEC_status`.

### Its data

The member's `.data` is `0x800B5F50` to `0x800B60EF` (0x1A0 bytes); its second
word holds the release number, `0x00450000`. Its public data are
`GPU_printf` (`D_800B5F9C`), `GEnv` (`D_800B5FA0`), `_qin` (`D_800B60C8`)
and `_qout` (`D_800B60CC`); the rest has no name of Sony's.

| Address | What it is |
|---|---|
| `D_800B5F58` | The GPU function table, a `GpuTable` (`psyq_private.h`); its first word is the version string's address. |
| `D_800B5F98` | The pointer to that table, through which every entry point calls. |
| `D_800B5F9C` | `GPU_printf`, the debug print hook; it holds `printf` (`0x80021700`). |
| `D_800B5FA0` | `GEnv`, the GPU environment, a `GpuEnv` of 0x80 bytes. |
| `D_800B6020`, `D_800B602C` | The frame-buffer width and height per GPU type: 1024, 1024, 1024 and 512, 512, 1024. |
| `D_800B6038` | `MoveImage`'s five-word frame-buffer-copy primitive. |
| `D_800B60A4` to `D_800B60C4` | Pointers to the hardware registers (below). |
| `D_800B60C8`, `D_800B60CC` | `_qin` and `_qout`, the command queue's write and read indices, modulo 64. |
| `D_800B60D0` | The interrupt mask `func_8008D99C` saves. |
| `D_800B60D4` | The interrupt mask the queue's runner `func_8008DC4C` saves. |
| `D_800B60D8` | The interrupt mask `func_8008DEAC` saves (and `func_8008E16C`, which carries `func_8008DEAC`'s statements in line). |
| `D_800B60DC` | The timeout's deadline: `VSync(-1) + 240` at the last `set_alarm`. |
| `D_800B60E0` | The timeout's poll counter: 0 at `set_alarm`, counted up by `func_8008E16C`. |

The function table, as the image holds it:

| Offset | `GpuTable` member | Function |
|---|---|---|
| `+0x00` | `ver` | The version string; not read. |
| `+0x04` | `addque` | `_addque` (`0x8008D978`) |
| `+0x08` | `addque2` | `func_8008D99C` |
| `+0x0C` | `clr` | `func_8008D19C` |
| `+0x10` | `ctl` | `func_8008D888` |
| `+0x14` | `cwb` | `func_8008D8C0` |
| `+0x18` | `cwc` | `func_8008D900` |
| `+0x1C` | `drs` | `func_8008D608` |
| `+0x20` | `dws` | `func_8008D3CC` |
| `+0x24` | `exeque` | `func_8008DC4C` |
| `+0x28` | `getctl` | `func_8008D8AC` |
| `+0x2C` | `otc` | `func_8008D0BC` |
| `+0x30` | `param` | `_param` (`0x8008D948`) |
| `+0x34` | `reset` | `func_8008DEAC` |
| `+0x38` | `status` | `func_8008D0A4` |
| `+0x3C` | `sync` | `func_8008DFFC` |

`GEnv`'s members used on this page are `type` (`+0x00`, the GPU type),
`queue` (`+0x01`, the queue mode), `level` (`+0x02`, the debug level), `w`
and `h` (`+0x04`, `+0x06`, the frame-buffer size for the GPU type),
`cb_pending` (`+0x08`), `drawsync` (`+0x0C`, the DrawSync callback) and
`disp` (`+0x6C`, the display environment in force); `psyq_private.h` has the
struct with a comment per member.

The hardware pointers hold these addresses, and the code loads the pointer
before every access:

| Address | Points at |
|---|---|
| `D_800B60A4` | `0x1F801810`: GP0 when written, GPUREAD when read. |
| `D_800B60A8` | `0x1F801814`: GP1 when written, GPUSTAT when read. |
| `D_800B60AC`, `D_800B60B0`, `D_800B60B4` | `0x1F8010A0`, `A4`, `A8`: DMA channel 2 (the GPU) MADR, BCR, CHCR. |
| `D_800B60B8`, `D_800B60BC`, `D_800B60C0` | `0x1F8010E0`, `E4`, `E8`: DMA channel 6 (the ordering-table clear) MADR, BCR, CHCR. |
| `D_800B60C4` | `0x1F8010F0`: DPCR, the DMA control register. |

Bits these functions test: GPUSTAT bit 26 (`0x04000000`), ready to receive a
command word; GPUSTAT bit 27 (`0x08000000`), ready to send frame-buffer data
to the CPU; DMA channel 2's CHCR bit 24 (`0x01000000`), a transfer in flight.

The member's `.bss` begins at `0x800E55C8`:

| Address | `.bss` | What it is |
|---|---|---|
| `D_800E55C8` | `+0x00` | `func_8008D19C`'s first packet: a tag and up to eight words. |
| `D_800E55F0` | `+0x28` | `func_8008D19C`'s second packet: a tag and three words. |
| `D_800E5610` | `+0x48` | The shadow table, 0x100 bytes: one byte per GP1 command. `func_8008D888` stores the low byte of each word it sends at the index `word >> 24`; `func_8008D8AC` reads it back. |
| `D_800E5718` | after it | The command queue, 64 entries of 0x60 bytes (`GpuQue`: `func`, `p1`, `p2`, `buf[21]`). |

The two packets are two objects in the C, because the image takes each
address with its own `la` ([codegen.md](codegen.md) §"Symbols: one or
several"); their real types and sizes are not known.

The debug strings these functions print are in the member's `.rdata`:

| Address | Text |
|---|---|
| `D_8001303C` | `"SetGrapQue(%d)...\n"` (the spelling is the image's) |
| `D_80013050` | `"DrawSyncCallback(%08x)...\n"` |
| `D_8001306C` | `"SetDispMask(%d)...\n"` |
| `D_80013094` | `"%s:bad RECT"` |
| `D_800130A0` | `"(%d,%d)-(%d,%d)\n"` |
| `D_800130B4` | `"%s:"` |
| `D_80013124` | `"DrawOTag(%08x)...\n"` |
| `D_80013184` | `"GPU timeout:que=%d,stat=%08x,chcr=%08x,madr=%08x\n"` |
| `D_800131B8` | `"LoadImage2"` |

### Settings: `SetGraphQueue`, `DrawSyncCallback`, `SetDispMask` (`libgpu_sys_1.c`)

- **`SetGraphQueue(mode)`** (`0x8008BA58`) sets the queue mode `GEnv.queue`
  and returns the previous one, zero-extended. At debug level 2 or more it
  prints `"SetGrapQue(%d)..."` through `GPU_printf`. When `mode` differs from
  `GEnv.queue` (read again after the print), it calls the table's `reset`
  entry with 1, stores the mode and calls `DMACallback(2, 0)` (`0x80094C38`),
  which removes DMA channel 2's callback. With the mode 0,
  `func_8008D99C` runs each request at once instead of queueing it;
  `ResetGraph` sets the mode to 1. Nothing in this program calls
  `SetGraphQueue`.
- **`DrawSyncCallback(func)`** (`0x8008BB0C`) installs `func` as the DrawSync
  callback, `GEnv.drawsync`, the word the queue's runner calls once the queue
  has drained, and returns the function it replaces as a `u_long`; 0 removes
  it. At debug level 2 or more it first prints its line. Nothing in this
  program calls it.
- **`SetDispMask(mask)`** (`0x8008BB6C`) switches the display on (`mask !=
  0`) or off (`mask == 0`). At debug level 2 or more it prints its line. With
  `mask == 0` it first fills `GEnv.disp` (0x14 bytes) with `0xFF` through the
  member's own memset (`func_8008E790`), so the next `PutDispEnv` finds every
  field different and sends all of them. Then the table's `ctl` entry writes
  `0x03000000` for `mask != 0` and `0x03000001` for `mask == 0`; the on and
  off reading of the two words is the SDK reference's and the GPU's
  display-enable command (inferred). Its callers are `screen_mode_set`
  (`0x8001F0C8`), `frame_end` (`0x8001F30C`), `PictureShow` (`0x80024A7C`),
  `cd_change_wait` (`0x80024D2C`, twice), `StartPictureLoadShow`
  (`0x800435A4`), `GameRun` (`0x800559F4`, twice) and libgs's
  `GsSwapDispBuff` (`0x8008F428`).

### The rectangle check: `func_8008BC6C` (`libgpu_sys_1.c`)

`func_8008BC6C(log, r)` (`sys.o` `+0x3E4`) is the debug check the member's
image functions make on their rectangle before anything is sent or queued:
`ClearImage`, `ClearImage2`, `LoadImage`, `StoreImage`, `MoveImage`,
`LoadImage2`, `StoreImage2` and `MoveImage2`. `log` is the caller's name
string. A switch on `GEnv.level`:

- **1**: prints `"<log>:bad RECT"` and then `"(x,y)-(w,h)\n"` when any of these
  holds, tested in this order: `r->w > GEnv.w`, `r->w + r->x > GEnv.w`,
  `r->y > GEnv.h`, `r->y + r->h > GEnv.h`, `r->w <= 0`, `r->x < 0`, `r->y < 0`,
  `r->h <= 0`.
- **2**: prints `"<log>:"` and the rectangle every time.
- any other level, 0 included: nothing.

It returns nothing and changes nothing: a bad rectangle is reported, not
refused, and the caller goes on with it.

### The drawing environment: `SetDrawEnv`, `func_8008CC48` and the setting words (`libgpu_sys_1.c`)

Seven consecutive functions, `sys.o` `+0x11C8` to `+0x181B` (`0x8008CA50` to
`0x8008D0A3`), turn a `DRAWENV` into the words of its `DR_ENV` primitive.

| Address | Name | Called by |
|---|---|---|
| `0x8008CA50` | `SetDrawEnv(dr_env, env)` | nothing in this program |
| `0x8008CC48` | `func_8008CC48(dr_env, env)` | `PutDrawEnv`, `DrawOTagEnv` |
| `0x8008CEB8` | `func_8008CEB8(dfe, dtd, tpage)` | the two builders |
| `0x8008CED8` | `func_8008CED8(x, y)` | `SetDrawArea`, the two builders |
| `0x8008CF70` | `func_8008CF70(x, y)` | `SetDrawArea`, the two builders |
| `0x8008D008` | `func_8008D008(x, y)` | `SetDrawOffset`, the two builders |
| `0x8008D024` | `func_8008D024(rect)` | the two builders |

The only global any of them reads is `GEnv`'s `w` and `h`, the frame-buffer
size for the GPU type. None writes a global.

A `DR_ENV` is a tag and up to fifteen words. Both builders write the same
first six:

| Word | Value | GP0 command (inferred) |
|---|---|---|
| `code[0]` | `func_8008CED8(clip.x, clip.y)` | `0xE3`, drawing area top-left |
| `code[1]` | `func_8008CF70(clip.x + clip.w - 1, clip.y + clip.h - 1)` | `0xE4`, drawing area bottom-right |
| `code[2]` | `func_8008D008(ofs[0], ofs[1])` | `0xE5`, drawing offset |
| `code[3]` | `func_8008CEB8(dfe, dtd, tpage)` | `0xE1`, draw mode |
| `code[4]` | `func_8008D024(&tw)` | `0xE2`, texture window |
| `code[5]` | `0xE6000000` | `0xE6`, mask bits, both off |
| `code[6]` to `[8]` | only with `isbg` set: the background clear, below | `0x60` flat rectangle, or `0x02` frame-buffer fill |

The tag's length byte is set to the number of words written, 6 or 9. Neither
builder touches the tag's address field: `PutDrawEnv` terminates it and
`DrawOTagEnv` points it at the ordering table.

The setting words:

- **`func_8008CED8(x, y)`** clamps `x` to 0 .. `GEnv.w - 1` and `y` to 0 ..
  `GEnv.h - 1` and returns `0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF)`.
- **`func_8008CF70(x, y)`** is the same with the command byte `0xE4`.
- **`func_8008D008(x, y)`** returns `0xE5000000 | ((y & 0x7FF) << 11) | (x &
  0x7FF)`, with no clamp.
- **`func_8008CEB8(dfe, dtd, tpage)`** returns `(dtd ? 0xE1000200 :
  0xE1000000) | (dfe ? 0x400 : 0) | (tpage & 0x9FF)`.
- **`func_8008D024(rect)`** returns 0 for a null `rect` (both builders pass
  `&env->tw`, never null). Otherwise each field is cut to 8 bits and counted
  in 8-pixel units: `0xE2000000 | (((rect->y & 0xFF) >> 3) << 15) |
  (((rect->x & 0xFF) >> 3) << 10) | (((-rect->h & 0xFF) >> 3) << 5) |
  ((-rect->w & 0xFF) >> 3)`. Read against the GPU's texture-window command
  (inferred): offset y, offset x, mask y, mask x, five bits each.

Each word has one packing for every GPU type.

**`SetDrawEnv(dr_env, env)`** (`0x8008CA50`) writes the six words from
`env`. With `env->isbg` set it appends the background clear as a flat
rectangle:

1. A copy of `env->clip` gets `w` clamped to 0 .. `GEnv.w - 1` and `h` to 0 ..
   `GEnv.h - 1`.
2. Its position becomes `x - env->ofs[0]`, `y - env->ofs[1]`.
3. `code[6]` = `0x60000000 | (b0 << 16) | (g0 << 8) | r0`; `code[7]` = the
   copy's x / y word; `code[8]` = its w / h word.

The subtraction undoes the drawing offset that `code[2]` of the same
primitive sets, so the rectangle lands on the clip area (inferred from the
GPU applying the offset to rectangles; the code only shows the subtraction).

**`func_8008CC48(dr_env, env)`** (`0x8008CC48`) is the builder `PutDrawEnv`
(`0x8008C268`) and `DrawOTagEnv` (`0x8008C328`) use. The six words and the
clamp are `SetDrawEnv`'s. With `env->isbg` set it picks the clear by
alignment, testing the clip's `x` and the clamped `w`:

- `(x & 0x3F) || (w & 0x3F)`: `SetDrawEnv`'s rectangle, command `0x60` at `x -
  ofs[0]`, `y - ofs[1]`;
- both multiples of 64: `code[6]` = `0x02000000 | (b0 << 16) | (g0 << 8) |
  r0`, `code[7]` = the copy's x / y word unchanged, `code[8]` = its w / h
  word. No offset is subtracted on this arm.

Both builders' C copies `dr_env` into a local that every store goes through,
a marked spelling ([codegen.md](codegen.md) §"Forcing devices: what
they stood for").

### The hardware routines behind the table (`libgpu_sys_1.c`)

The rest of the member reaches these through the function table; only
`func_8008D19C` calls two of them, `_param` and `func_8008D900`, by `jal`.

- **`func_8008D0A4()`** (`+0x38` `status`) returns GPUSTAT.
- **`func_8008D888(word)`** (`+0x10` `ctl`) writes `word` to GP1 and keeps
  its low byte in the shadow table `D_800E5610` at `word >> 24`.
  **`func_8008D8AC(n)`** (`+0x28` `getctl`) returns that byte for command
  `n`.
- **`func_8008D8C0(data, n)`** (`+0x14` `cwb`) writes `n` command words to
  the GPU by hand: GP1 `0x04000000` (DMA off), then the words at `data` to
  GP0 one after another. It returns 0. `DrawPrim` (`0x8008C19C`) calls it
  through the table after `sync(0)`, with the address of a primitive's first
  data word and its length byte.
- **`func_8008D900(p)`** (`+0x18` `cwc`) sends the primitive list at `p` by
  DMA: GP1 `0x04000002` (DMA direction CPU to GP0), then channel 2's MADR =
  `p`, BCR = 0, CHCR = `0x01000401` (start, linked-list mode, from memory;
  inferred). It returns at once with the transfer running. `DrawOTag`,
  `PutDrawEnv`, `DrawOTagEnv` and `MoveImage` queue it; `MoveImage2` and
  `DrawOTag2` call it through the table without queueing, and
  `func_8008D19C` calls it by `jal`.
- **`_param(x)`** (`0x8008D948`, `+0x30` `param`) writes GP1 `0x10000000 |
  x` (get GPU information) and returns the low 24 bits read from GPUREAD.
  `func_8008D19C` asks for 3, 4 and 5 (inferred: the drawing area's
  top-left, its bottom-right, the drawing offset).
- **`func_8008D0BC(ot, n)`** (`+0x2C` `otc`; reached only through the table,
  by `ClearOTagR`, `0x8008C0F0`) builds a reverse ordering table of `n`
  entries with DMA channel 6:
  1. sets DPCR bit 27 (`*D_800B60C4 |= 0x08000000`);
  2. clears the channel's CHCR;
  3. MADR = `ot + n - 1`, the table's last entry; BCR = `n`;
  4. CHCR = `0x11000002`;
  5. arms the timeout (`set_alarm`) and waits for CHCR bit 24 to clear.
     It returns `n`, or -1 when `func_8008E16C` reports the timeout first.

  The meaning of the CHCR word (start, backward step from the last entry,
  each entry linked to the one below and entry 0 terminated) is the DMA
  channel's documented ordering-table mode (inferred).
- **`func_8008D19C(rect, color)`** (`+0x0C` `clr`; queued by `ClearImage`
  and `ClearImage2`) fills a frame-buffer rectangle with the 24-bit colour in
  `color`.
  1. It clamps `rect->w` to 0 .. `GEnv.w - 1` and `rect->h` to 0 .. `GEnv.h -
     1`, in place.
  2. If `rect->x` or the clamped `rect->w` is not a multiple of 64, it writes
     two primitives. `D_800E55C8`: tag = 8 words, linked to `D_800E55F0`; then
     `0xE3000000`, `0xE4FFFFFF`, `0xE5000000`, `0xE6000000`, the draw-mode
     word (below), `0x60000000 | (color & 0xFFFFFF)`, the x / y word, the w /
     h word. `D_800E55F0`: tag `0x03FFFFFF` (three words, terminated); then
     `0xE3000000 | _param(3)`, `0xE4000000 | _param(4)`, `0xE5000000 |
     _param(5)`.
  3. Otherwise one primitive in `D_800E55C8`: tag `0x05FFFFFF` (five words,
     terminated); `0xE6000000`, the draw-mode word, `0x02000000 | (color &
     0xFFFFFF)`, the x / y word, the w / h word.
  4. It sends the list with `func_8008D900` and returns 0.

  The draw-mode word is `0xE1000000 | (GPUSTAT & 0x7FF) | ((color >> 31) <<
  10)`: the current mode bits read back from the status register, with bit 10
  taken from bit 31 of `color`. `ClearImage2` sets that bit in the colour it
  passes, `ClearImage` does not. Read against the GPU's commands (inferred):
  the first arm opens the drawing area to the whole frame buffer with offset
  0, draws a flat rectangle, and the second packet puts back the drawing area
  and offset that `_param` reads from the GPU; the second arm is the
  frame-buffer fill, which needs no drawing-area change; bit 10 of the `0xE1`
  word allows drawing to the displayed area.
- **`func_8008D608(rect, data)`** (`+0x1C` `drs`; queued by `StoreImage`,
  called unqueued by `StoreImage2`) reads a frame-buffer rectangle into
  `data`.
  1. `set_alarm()`.
  2. `rect->w` is clamped to 0 .. `GEnv.w` and `rect->h` to 0 .. `GEnv.h`,
     written back in place. When the request was queued, `rect` is the queue
     entry's 8-byte copy; on `func_8008D99C`'s direct path it is the caller's
     `RECT`.
  3. `words = (w * h + 1) / 2`: the 16-bit pixels in 32-bit words, rounded
     up. It returns -1 when that is not positive.
  4. It waits for GPUSTAT bit 26, and returns -1 when `func_8008E16C` reports
     the timeout.
  5. GP1 `0x04000000` (DMA off). GP0: `0x01000000` (clear cache),
     `0xC0000000` (copy frame buffer to CPU), the rectangle's x / y word, its
     w / h word.
  6. It waits for GPUSTAT bit 27; -1 on the timeout.
  7. It reads the first `words % 16` words from GPUREAD by hand.
  8. If `words / 16` is not 0: GP1 `0x04000003` (DMA direction GPUREAD to
     CPU), MADR = the advanced `data`, BCR = `(words / 16) << 16 | 0x10`
     (blocks of 16 words), CHCR = `0x01000200`. It returns 0 with that
     transfer still running.
- **`func_8008D3CC(rect, data)`** (`+0x20` `dws`; queued by `LoadImage`,
  called unqueued by `LoadImage2`) writes pixel data into a frame-buffer
  rectangle: `func_8008D608` with the direction turned round. It arms the
  timeout, clamps the rectangle the same way, returns -1 for no pixels or on
  the timeout of the GPUSTAT bit 26 wait, sends GP1 `0x04000000`, GP0
  `0x01000000`, `0xA0000000` (copy CPU to frame buffer), the x / y word and
  the w / h word, writes the first `words % 16` words to GP0 by hand and
  leaves the rest to channel 2: GP1 `0x04000002`, MADR = the read pointer,
  BCR = `(blocks << 16) | 0x10`, CHCR = `0x01000201`. It returns 0 with that
  transfer still running. Unlike `func_8008D608` it does not wait for GPUSTAT
  bit 27 between the command words and the data. The C picks the GP0 word
  through a local that is always 0 (`mode ? 0xB0000000 : 0xA0000000`), a
  marked spelling ([codegen.md](codegen.md) §"Forcing devices: what they
  stood for").

### The queue's front door: `_addque` and `func_8008D99C` (`libgpu_sys_2.c`, `libgpu_sys_3.c`)

**`_addque(func, p1, p2)`** (`0x8008D978`, table `+0x04`) is
`func_8008D99C(func, p1, 0, p2)`: it queues a routine with `p1` passed as it
is, and returns what that returns.

**`func_8008D99C(func, p1, len, p2)`** (table `+0x08`) is the front door of
the command queue: every queued drawing call of the member goes through it.
It runs `func(p1, p2)` now or queues it.

1. `set_alarm()`. While the queue is full, `((_qin + 1) & 0x3F) == _qout`, it
   returns -1 when `func_8008E16C` reports the timeout, else runs the queue
   (`func_8008DC4C`) and tests again.
2. `D_800B60D0 = SetIntrMask(0)` (`0x80094D54`); `GEnv.cb_pending = 1`.
3. **Direct path.** If `GEnv.queue` is 0 (queueing off), or if the queue is
   empty (`_qin == _qout`), no DMA transfer is in flight (channel 2's CHCR bit
   24 clear) and no DrawSync callback is installed (`GEnv.drawsync == 0`): it
   waits for GPUSTAT bit 26 (with no timeout on this wait), calls `func(p1,
   p2)`, restores the interrupt mask and returns 0.
4. **Queued path.** `DMACallback(2, func_8008DC4C)`: the end of a channel 2
   transfer runs the queue. The entry at `_qin` is filled: with `len != 0`
   the `len / 4` words at `p1` are copied into the entry's `buf` and the
   entry's `p1` is the address of `buf`; with `len == 0` the entry's `p1` is
   `p1` itself. Then `p2`, then `func`. `_qin = (_qin + 1) & 0x3F`, the mask
   is restored, the queue is run once, and the function returns `(_qin -
   _qout) & 0x3F`, the number of entries still queued.

So a caller that passes a length (8 for a `RECT`, 0x14 for `MoveImage`'s
primitive, 0x40 for a `DR_ENV`) may reuse its data as soon as the call
returns, and one that passes 0 (`DrawOTag`, `_addque`) must keep it alive
until the queue has run it. `len` is not checked against `buf`'s 21 words.
With queueing on and a DrawSync callback installed, a request is queued even
when the queue is idle: the queue's runner is what fires the callback, when
the queue drains with `GEnv.cb_pending` set ([psyq-libs.md](../psyq-libs.md)
describes the runner, `func_8008DC4C`).

### Reset, sync and the GPU type: `func_8008DEAC`, `func_8008DFFC`, `func_8008E2B0` (`libgpu_sys_3.c`, `libgpu_sys_4.c`)

**`func_8008DEAC(mode)`** (`sys.o` `+0x2624`) is the table's `+0x34` entry
and `ResetGraph`'s worker; `ResetGraph` also calls it by `jal`.

1. `D_800B60D8 = SetIntrMask(0)`.
2. `_qout = 0; _qin = _qout;`: the queue is emptied by its indices.
3. By `mode & 7`:

   | `mode & 7` | Writes |
   |---|---|
   | 0, 5 | channel 2's CHCR = `0x401`; DPCR \|= `0x800`; GP1 = 0; the shadow table (0x100 bytes) and the queue (0x1800 bytes) zeroed with `func_8008E790` |
   | 1, 3 | channel 2's CHCR = `0x401`; DPCR \|= `0x800`; GP1 = `0x02000000`; GP1 = `0x01000000` |
   | 2, 4, 6, 7 | none |

4. `SetIntrMask(D_800B60D8)`.
5. It returns `func_8008E2B0(mode)` when `mode & 7` is 0, else 0.

`ResetGraph` stores the result in `GEnv.type` for its modes 0, 3 and 5, so
only mode 0 can store a non-zero type. GP1 command meanings (inferred): `0x00`
resets the GPU, `0x01` resets the command buffer, `0x02` acknowledges the GPU
interrupt; DPCR bit 11 enables DMA channel 2.

**`func_8008E2B0(mode)`** (`+0x2A28`) is the GPU type probe, called only by
`func_8008DEAC`:

1. GP1 = `0x10000007`.
2. If `(GPUREAD & 0xFFFFFF) != 2`: GP0 = `0xE1001000 | (GPUSTAT & 0x3FFF)`,
   one more read of GPUREAD with the value dropped, and it returns **0**.
3. Otherwise it returns **1** when `mode & 8` is clear;
4. with `mode & 8` set, GP1 = `0x09000001`, and it returns **2**.

The three results index `D_800B6020` and `D_800B602C`, the frame-buffer size
per GPU type: 1024 x 512, 1024 x 512, 1024 x 1024. Inferred from the GPU's
command set: GP1 `0x10` with parameter 7 asks for the GPU's version, which
answers 2 on the later GPU; GP1 `0x09` turns on the texture-disable feature.

**`func_8008DFFC(mode)`** (`+0x2774`; reached only through the table's
`+0x3C` entry) is `DrawSync`'s worker.

- **`mode == 0`, wait.** `set_alarm()`; while `_qin != _qout`: run the queue,
  then return -1 if `func_8008E16C` reports the timeout; then, while a GPU DMA
  transfer is running (channel 2's CHCR bit 24) or the GPU does not take
  commands (GPUSTAT bit 26 clear): return -1 on the timeout. It returns 0.
- **Any other mode, poll.** `queued = (_qin - _qout) & 0x3F`; one run of the
  queue if that is non-zero (its result is dropped and `queued` is not
  recomputed). The result is `queued`, except that 0 becomes 1 while a
  transfer is still running or the GPU is not ready.

### The timeout: `set_alarm` and `func_8008E16C` (`libgpu_sys_4.c`)

Every busy-wait in `sys.o` is `set_alarm()` once, then `if
(func_8008E16C()) return -1;` on each pass.

- **`set_alarm()`** (`0x8008E138`): `D_800B60DC = VSync(-1) + 240`
  (`0x800949A8`), `D_800B60E0 = 0`. The functions the member defines before
  it (`func_8008D0BC`, `func_8008D3CC`, `func_8008D608`, `func_8008D99C`,
  `func_8008DFFC`) call it; the four `*2` functions, defined after it, have
  its two statements in line ([codegen.md](codegen.md) §"Functions expanded
  in line").
- **`func_8008E16C()`** (`+0x28E4`) returns 0 while both hold: `VSync(-1)` has
  not passed `D_800B60DC`, and `D_800B60E0`, counted up on each call that gets
  that far, was not above `0xF0000`. Otherwise it is a timeout:
  1. one read of GPUSTAT, with the value dropped;
  2. `printf(D_80013184, (_qin - _qout) & 0x3F, GPUSTAT, channel 2's CHCR,
     channel 2's MADR)`: `printf` (`0x80021700`) called directly, not through
     `GPU_printf`, so the message does not depend on the debug level;
  3. `func_8008DEAC(1)`'s statements in line: `D_800B60D8 = SetIntrMask(0)`,
     the queue indices cleared, channel 2's CHCR = `0x401`, DPCR |= `0x800`,
     GP1 = `0x02000000`, GP1 = `0x01000000`, `SetIntrMask(D_800B60D8)`;
  4. it returns -1.

  It does not re-arm the timeout: with the deadline passed, every further call
  reports the timeout, and resets again, until a caller runs `set_alarm`. It is
  polled by `func_8008D0BC`, `func_8008D3CC`, `func_8008D99C`,
  `func_8008D608` and `func_8008DFFC` (the last two in two loops each) and by
  the four `*2` functions.

### The unqueued calls: `LoadImage2`, `StoreImage2`, `MoveImage2`, `DrawOTag2` (`libgpu_sys_5.c`, `libgpu_sys_6.c`)

`LoadImage2` (`0x8008E350`), `StoreImage2` (`0x8008E43C`), `MoveImage2`
(`0x8008E528`) and `DrawOTag2` (`0x8008E66C`) do what `LoadImage`,
`StoreImage`, `MoveImage` and `DrawOTag` do, but call the table's routine
themselves instead of handing it to the queue. Each one:

1. checks its argument: `func_8008BC6C` with a name string, or, in
   `DrawOTag2`, `DrawOTag`'s debug print (`"DrawOTag(%08x)...\n"` at level 2
   or more). `StoreImage2` and `MoveImage2` pass their queued twins' name
   strings, so they report the twin's name; `LoadImage2` passes its own,
   `"LoadImage2"` (`D_800131B8`);
2. arms the timeout with `set_alarm`'s two statements in line;
3. waits until no GPU DMA transfer is running (channel 2's CHCR bit 24) and
   the GPU takes commands (GPUSTAT bit 26), returning -1 when
   `func_8008E16C` reports the timeout. The queue (`_qin`, `_qout`) is not
   looked at: only the hardware is waited on;
4. calls `DMACallback(2, _GPU_ResetCallback)`;
5. calls the table entry directly and returns 0, dropping the entry's own
   result.

The entries: `LoadImage2(rect, p)` calls `dws` (`+0x20`), `StoreImage2(rect,
p)` calls `drs` (`+0x1C`), `DrawOTag2(p)` calls `cwc` (`+0x18`) with the
ordering table. `MoveImage2(rect, x, y)` returns -1 after step 4 when
`rect->w` or `rect->h` is 0, so an empty rectangle still waits and still
replaces the DMA callback; otherwise it fills `D_800B6038` as `MoveImage`
does (source = the rectangle's x / y word, destination = `(y << 16) | (x &
0xFFFF)`, size = its w / h word) and calls `cwc` on it.

**`_GPU_ResetCallback()`** (`0x8008E768`, `libgpu_sys_6.c`) is
`DMACallback(2, func_8008DC4C)`: called as channel 2's callback after one of
the four transfers, it makes the queue's runner the callback again.

None of the four has a caller in this program; they are linked with the rest
of the member.

**`func_8008E790(ptr, value, num)`** (`+0x2F08`, `libgpu_sys_6.c`), the
member's last function, is its own byte fill: it stores `value` at `ptr[0]`
to `ptr[num - 1]` and returns nothing. `num` is not guarded: 0 stores
nothing, a negative count runs on. `ResetGraph` and `SetDispMask` call it on
`GEnv` and its cached environments, and `func_8008DEAC` on the shadow table
and the queue.

## libgpu — `LoadClut` (`e01.o`, `libgpu_e01.c`)

**`LoadClut(clut, x, y)`** (`0x8008E808`) loads a 256-entry colour table:
`LoadImage(&rect, clut)` with `rect = {x, y, 256, 1}`, then it returns
`GetClut(x, y)` (`0x8008E9B8`: `(y << 6) | ((x >> 4) & 0x3F)`). The transfer is
queued, not waited for. Release 4.5's `e01.o` lies whole at `0x8008E808`: 25
instructions and three words of padding. The C's comment says earlier
releases called the member EXT.OBJ.

Seven call sites, all with `x` = 0 and `y` from `0x1F4` to `0x1FB`:
`BtlDataLoad` (`0x80018694`, two), `BtlPartyDataLoad` (`0x80018A78`),
`sprite_palette_load` (`0x80018BAC`), `sprite_init` (`0x80018BFC`, two) and
`sprite_reset` (`0x80018DBC`).

## libgs — `gs_001.o`: `GsInitGraph` and its two statics

Release 4.5's `gs_001.o` lies whole at `0x8008EAF8` (340 words, to
`0x8008F047`). Its five functions:

| Offset | Address | Name | File |
|---|---|---|---|
| `+0x000` | `0x8008EAF8` | `GsInitGraph` | `libgs_gs_001_1.c` |
| `+0x074` | `0x8008EB6C` | `func_8008EB6C` | `libgs_gs_001_1.c` |
| `+0x190` | `0x8008EC88` | `GsInitGraph2` | `libgs_gs_001_2.c` |
| `+0x1F8` | `0x8008ECF0` | `func_8008ECF0` | `libgs_gs_001_3.c` |
| `+0x418` | `0x8008EF10` | `GsSortClear` | `libgs_gs_001_3.c` |

The two statics have no name in release 4.5. The C's comments name them
after functions of GS_001.OBJ in releases 3.3 to 4.0, but there those names
stand over other code, and no release exports these bytes, so they keep
their addresses.

The libgs variables they set are named here as the libgs files' comments
name them, libgs's own names for these words; the register leaves the words
unnamed until the relocation records that prove the names are read.

| Address | libgs's name | Type | What it is |
|---|---|---|---|
| `D_800E6F50` | `GsDRAWENV` | `DRAWENV` | libgs's drawing environment. |
| `D_800E6FB0` | `GsDISPENV` | `DISPENV` | libgs's display environment. |
| `D_800E6FD4` | `PSDIDX` | `short` | The double-buffer index. |
| `D_800E6FD6` | `PSDGPU` | `short` | `intmode & 4` (`GsOFSGPU`). |
| `D_800E6FD8`, `D_800E6FDC` | `HWD0`, `VWD0` | `long` | The screen's width and height. |
| `D_800E7050` | `GsIDMATRIX` | `MATRIX` | The unit matrix. |
| `D_800E7090` | `GsIDMATRIX2` | `MATRIX` | The unit matrix with the pixel aspect in `m[1][1]`. |
| `D_800E6FF0` | `GsLIGHTWSMATRIX` | `MATRIX` | The light matrix; zeroed here, and `GsSetFlatLight` (`0x8008F5F8`) fills its rows. |
| `D_800E7010` | `_LC` | `MATRIX` | The local colour matrix; zeroed here, and `GsSetFlatLight` fills its columns. |
| `D_800E6F40`, `D_800E6F44` | `PSDBASEX`, `PSDBASEY` | `short[2]` | The two buffers' bases; zeroed here. |
| `D_800E6F48` | `POSITION` | `_GsPOSITION` | Its `offx`, `offy` zeroed here. |
| `D_800E6FC8` | `CLIP2` | `RECT` | Set to `{0, 0, HWD0, VWD0}`. |
| `D_800E6FD0` | `PSDCNT` | `u_long` | Set to 1. |
| `D_800E6F18` | none (the member's own `.bss`) | two `TILE`s | The screen-clear pair, set up here, filled and linked by `GsSortClear`. |

**`GsInitGraph(x, y, intmode, dith, varmmode)`** (`0x8008EAF8`) calls, in
order, `func_8008EB6C(x, y, intmode, dith, varmmode)`, `gte_init()`
(`0x8008FBF8`, libgs `gs_121.o`: `InitGeom()`, `SetFarColor(0, 0, 0)`,
`SetGeomOffset(0, 0)`, then `D_800E6FC6` and `D_800E6FC4`, which the comments
call `GsORGOFSY` and `GsORGOFSX`, cleared), sets `PSDIDX` to 0, and calls
`func_8008ECF0(x, y)`, `GsSetDrawBuffClip()` and `GsSetDrawBuffOffset()`. The
game calls it twice: `GsInitGraph(0x140, 0xF0, 4, 1, 0)` in `system_init`
(`0x8001EEB8`; 320 x 240, `GsOFSGPU`, dither on, 16-bit), and
`GsInitGraph(g_ScreenWidth, g_ScreenHeight, mode, 1, 0)` in `screen_mode_set`
(`0x8001F0C8`).

**`GsInitGraph2(x, y, intmode, dith, varmmode)`** (`0x8008EC88`) is the same
without resetting the GPU and without `gte_init`, and sends nothing: it only
writes `GsDRAWENV`'s `tpage` (0), `dtd` (`dith`), `dfe` (0) and `isbg` (0),
`GsDISPENV`'s `disp.w` and `disp.h` (`x`, `y`), `isinter` and `isrgb24`, and
`PSDGPU`, then calls `func_8008ECF0(x, y)`. Nothing in this program calls it.

**`func_8008EB6C(x, y, intmode, dith, varmmode)`** (`+0x74`) is
`GsInitGraph`'s GPU half. All five parameters are `unsigned short`.

1. `ResetGraph(3)` when `(intmode >> 4) & 3` is 3 (`GsRESET3`), else
   `ResetGraph(0)`.
2. `GsDRAWENV`: `ofs[0]` = `ofs[1]` = 0, `tw` = {0, 0, 0, 0}, `tpage` = 0, `dtd`
   = `dith`, `dfe` = 0, `isbg` = 0; `PutDrawEnv(&GsDRAWENV)`. The clip
   rectangle is not touched here (`GsSetDrawBuffClip` sets it later in
   `GsInitGraph`), so this first `PutDrawEnv` sends whatever `clip` held.
3. `GsDISPENV`: `disp` = {0, 0, x, y}, `screen` = {0, 0, 0, 0}; under PAL
   (`GetVideoMode() == 1`) `screen.y` = 24 and `pad0` = 1.
4. `isinter` = `intmode & 1` (`GsINTER`); `PSDGPU` = `intmode & 4`
   (`GsOFSGPU`); `isrgb24` = `varmmode`; `PutDispEnv(&GsDISPENV)`.

**`func_8008ECF0(x, y)`** (`+0x1F8`) sets every libgs variable a screen size
decides. Both parameters are `unsigned short`; its callers are `GsInitGraph`
and `GsInitGraph2`.

1. `HWD0` = x, `VWD0` = y.
2. `GsIDMATRIX` = the unit matrix: 4096 on the diagonal, 0 elsewhere, `t` =
   {0, 0, 0}.
3. `GsIDMATRIX2` = `GsIDMATRIX` with `m[1][1]` = `4096 * 4 * VWD0 / HWD0 / 3`
   (two signed divisions, in that order): the unit matrix corrected for the
   pixel aspect. 4096 for 320 x 240, 8192 for 320 x 480, 2048 for 640 x 240.
4. `GsLIGHTWSMATRIX` = `GsIDMATRIX` with the diagonal set to 0, so all zero;
   `_LC` = a copy of it.
5. `PSDBASEX[0..1]` = `PSDBASEY[0..1]` = 0; `POSITION.offx` = `.offy` = 0;
   `CLIP2` = {0, 0, HWD0, VWD0}.
6. Both screen-clear primitives (`D_800E6F18[0..1]`): tag length 3, code 2
   (GP0 `0x02`, the frame-buffer fill).
7. `PSDCNT` = 1.

The function is one basic block that the scheduler reorders, and several
orders of steps 3 to 7 give the same bytes; the C keeps one of them.

`GsSortClear` (`0x8008EF10`) and the rest of libgs's double buffer are in
[psyq-libs.md](../psyq-libs.md).
