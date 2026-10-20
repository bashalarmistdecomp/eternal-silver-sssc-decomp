# libetc and libsnd

This page covers part of Sony's PSY-Q 4.5 library code in the main
executable (`SLUS_006.28`): libetc's vertical-blank wait, interrupt entry and
DMA interrupt (members `vsync.o`, `intr.o`, `intr_dma.o`); libc2's `setjmp`
and `longjmp`; and the libsnd functions that close, reset and open scores,
fill the SEQ reader's handler table, read control changes and two tone
attributes, parts of the voice manager (pitch, key-off, tone selection, the
VAB header opens, stereo), the reverb type, the damper, play, pause, stop
and the volume read-back, and the size of the tick setting. The rest of
libsnd is in [psyq-libs.md](../psyq-libs.md), and its call graph in
[libsnd-map.md](../../libsnd-map.md).

Every member named here is proven in place in the image at the address
given, and the register (`decomp/SLUS_006.28/config/symbol_addrs.txt`)
records the proof for each of its functions;
[naming.md](../../../../naming.md) says how members were proven and how names
are given. A function Sony's library gives no name keeps its address
(`func_80095758`). Sony's variables are not named in the tree yet: the C
calls them by address (`D_800E9620`), and this page gives Sony's symbol
beside the address where the library's objects name it (`_snd_openflag`).
How these objects were compiled, and which C shapes decide their bytes, is
in [codegen.md](codegen.md).

## libetc — VSync's wait (`vsync.o`)

Release 4.5's `vsync.o` lies whole at `0x800949A8` to `0x80094BB7` (132
words). It holds two functions: `VSync` (`0x800949A8`, `+0x000`,
`libetc_vsync_1.c`) and, at `+0x178`, the wait it calls, `func_80094B20`
(`libetc_vsync_2.c`), which the library gives no name. `VSync` is described
step by step in [libcd.md](libcd.md), section "libetc: `VSync`".

**`func_80094B20(count, max)`.** Spins until the vertical-blank count
`D_800B7970` reaches `count`. The spin is bounded: a volatile local starts at
`max << 15` and is counted down once per turn; when it runs out the function
calls `puts("VSync: timeout\n")` (`puts` (`0x800921D8`), the string at
`D_80013794`), `ChangeClearPAD(0)` (`0x80094BB8`) and `ChangeClearRCnt(3, 0)`
(`0x80094BC8`), and returns. By Sony's library reference, `ChangeClearPAD(0)`
makes the kernel's pad driver pass the vertical-blank interrupt on without
completing it; that this is what lets the count advance again is an
inference. `VSync` is its only caller, twice: first with
`(mode > 0 ? last + mode - 1 : last, mode > 0 ? mode - 1 : 0)`, `last` being
`D_800B6844`, then with `(D_800B7970 + 1, 1)`.

The state the object's two functions use:

| Address | Where | Meaning |
|---|---|---|
| `D_800B7970` | `intr_vb.o` `.data+0x20`; Sony's `Vcount` | The vertical-blank count. `func_800953C0`, `intr_vb.o`'s vertical-blank handler, adds one to it. |
| `D_800B6838` | `vsync.o` `.data+0x08` | Pointer, 0x1F801814 (the GPU status port). |
| `D_800B683C` | `vsync.o` `.data+0x0C` | Pointer, 0x1F801110 (root counter 1's count). |
| `D_800B6840` | `vsync.o` `.data+0x10` | Root counter 1 at the last `VSync`. |
| `D_800B6844` | `vsync.o` `.data+0x14` | The vertical-blank count at the last `VSync`. |

The first two words of the object's `.data` (`0x800B6830`) hold 0x00067350
and 0x00450000. libetc is one of the five libraries that keep their release
number in a word of data ([naming.md](../../../../naming.md)), and the second
word is that number, 4.5.

## libetc — the interrupt entry (`intr.o`)

Release 4.5's `intr.o` lies whole at `0x80094BD8` to `0x80095297` (432
words). Its source is `intr.c` revision 1.75 of 1997/02/07: the object's RCS
id string, at `0x800137A4`, is the first word of its callback table. None of
it is sound code; the register's data label `spu_cmd_table` (`0x800B7918`,
inherited from the first split) is word +8 of that table. `ResetCallback`
to `GetIntrMask` are in `libetc_intr_1.c`; `SetIntrMask` and the six
functions the library gives no name are in `libetc_intr_2.c`.

| Offset | Function | What it does |
|---|---|---|
| `+0x000` | `ResetCallback` (`0x80094BD8`) | Calls the table's `ResetCallback` entry, `func_80094D6C`. |
| `+0x030` | `InterruptCallback` (`0x80094C08`) | Calls the `InterruptCallback` entry, `func_80095014`: installs a handler on an interrupt and returns the one it replaces. |
| `+0x060` | `DMACallback` (`0x80094C38`) | Calls the `DMACallback` entry, `intr_dma.o`'s `func_80095654` once `func_80094D6C` has run. |
| `+0x090` | `VSyncCallback` (`0x80094C68`) | Calls the `VSyncCallbacks` entry with slot 4. |
| `+0x0C4` | `VSyncCallbacks` (`0x80094C9C`) | Calls the `VSyncCallbacks` entry, `intr_vb.o`'s `setIntrVSync` (`0x8009542C`) once `func_80094D6C` has run. |
| `+0x0F4` | `StopCallback` (`0x80094CCC`) | Calls the `StopCallback` entry, `func_8009515C`. |
| `+0x124` | `RestartCallback` (`0x80094CFC`) | Calls the `RestartCallback` entry, `func_800951FC`. |
| `+0x154` | `CheckCallback` (`0x80094D2C`) | Returns `inInterrupt`: non-zero while the dispatcher runs the handlers. |
| `+0x164` | `GetIntrMask` (`0x80094D3C`) | Returns I_MASK. |
| `+0x17C` | `SetIntrMask` (`0x80094D54`) | Stores a new I_MASK and returns the old one. |
| `+0x194` | `func_80094D6C` | Hooks the interrupt entry. |
| `+0x26C` | `func_80094E44` | The interrupt dispatcher. |
| `+0x43C` | `func_80095014` | Installs or removes the handler of one interrupt. |
| `+0x584` | `func_8009515C` | Unhooks the interrupt entry. |
| `+0x624` | `func_800951FC` | Hooks it again after `func_8009515C`. |
| `+0x69C` | `func_80095274` | Zeroes a number of words. |

The object's whole state is in `.data` (it has no small-data or bss section;
[codegen.md](codegen.md) has what that means for its code), 0x1100 bytes
from `0x800B6850`:

| `.data` | Address | Meaning |
|---|---|---|
| `+0x0000` | `0x800B6850` | The string "Library Programs (c) 1993-1997 Sony Computer Entertainment Inc., All Rights Reserved." |
| `+0x0058` | `D_800B68A8` | The interrupt environment, an `IntrEnv` of 0x1068 bytes (below). |
| `+0x10C0` | `0x800B7910` | The callback table, an `IntrCallbacks` of 0x20 bytes (below). |
| `+0x10E0` | `D_800B7930` | Pointer to the callback table; every API function calls through it. |
| `+0x10E4` | `D_800B7934` | Pointer, 0x1F801070 (I_STAT); its target is volatile. |
| `+0x10E8` | `D_800B7938` | Pointer, 0x1F801074 (I_MASK); its target is volatile. |
| `+0x10EC` | `D_800B793C` | Pointer, 0x1F8010F0 (DPCR, the DMA control register); its target is volatile. |
| `+0x10F0` | `D_800B7940` | The dispatcher's count of passes in a row that ended with an interrupt still pending. |

`IntrEnv` (`decomp/SLUS_006.28/include/psyq_private.h`), the record at
`D_800B68A8`:

| Offset | Member | Meaning |
|---|---|---|
| `+0x00` | `u16 interruptsInitialized` | The interrupt entry is hooked: set by `func_80094D6C` and `func_800951FC`, cleared by `func_8009515C`. |
| `+0x02` | `u16 inInterrupt` | The dispatcher is running; `CheckCallback` returns it. |
| `+0x04` | `handlers[11]` | One handler per bit of I_STAT. |
| `+0x30` | `u16 enabledInterruptsMask` | The bits that have a handler. |
| `+0x32` | `u16 savedMask` | I_MASK as `func_8009515C` found it. |
| `+0x34` | `s32 savedPcr` | DPCR as `func_8009515C` found it. |
| `+0x38` | `s32 buf[12]` | The `jmp_buf` given to `HookEntryInt`; `buf[1]` is its `$sp`. |
| `+0x68` | `s32 stack[0x400]` | The stack interrupts run on; `buf[1]` = `&stack[1004]`. |

The callback table (`0x800B7910`, `IntrCallbacks`), with what the image
holds:

| Offset | Member | Value in the image | Called by |
|---|---|---|---|
| `+0x00` | `rcsid` | `0x800137A4`, the RCS id string of `intr.c` 1.75 | |
| `+0x04` | `DMACallback` | 0; `func_80094D6C` stores `startIntrDMA()`'s result (`func_80095654`) | `DMACallback` |
| `+0x08` | `InterruptCallback` | `func_80095014` | `InterruptCallback` |
| `+0x0C` | `ResetCallback` | `func_80094D6C` | `ResetCallback` |
| `+0x10` | `StopCallback` | `func_8009515C` | `StopCallback` |
| `+0x14` | `VSyncCallbacks` | 0; `func_80094D6C` stores `startIntrVSync()`'s result (`setIntrVSync`) | `VSyncCallback` (with slot 4), `VSyncCallbacks` |
| `+0x18` | `RestartCallback` | `func_800951FC` | `RestartCallback` |
| `+0x1C` | `intrEnv` | `0x800B68A8` | |

The object calls `setjmp` (`0x800952E8`, libc2), `HookEntryInt`
(`0x800952D8`, B0:19), `ResetEntryInt` (`0x800952C8`, B0:18),
`ReturnFromException` (`0x800952B8`, B0:17), `_96_remove` (`0x800952A0`,
A0:72), `EnterCriticalSection` (`0x8008B3B8`) and `ExitCriticalSection`
(`0x8008B3C8`), `ChangeClearPAD` (`0x80094BB8`), `ChangeClearRCnt`
(`0x80094BC8`), `printf` (`0x80021700`), `startIntrVSync` (`0x80095368`) and
`startIntrDMA` (`0x80095488`). `printf` is the game's own and empty in this
build, so the object's two messages below are never shown.

- **`func_80095014(irq, handler)`**, `InterruptCallback`'s worker. Reads the
  handler installed for bit `irq`. When the new one differs and the entry is
  hooked, it saves I_MASK and writes 0 to it, stores the handler (or 0) in
  `handlers[irq]`, sets or clears the bit in the saved mask and in
  `enabledInterruptsMask`, then writes the mask back. For `irq` 0 it also
  calls `ChangeClearPAD(handler == 0)` and `ChangeClearRCnt(3, handler ==
  0)`, and for `irq` 4, 5 and 6 `ChangeClearRCnt(0, 1 or 2, handler == 0)`:
  while libetc has a handler, the kernel's own handlers leave the interrupt
  uncompleted (the library reference's wording for `ChangeClearPAD`; for
  `ChangeClearRCnt`, which the reference does not document, an inference).
  Returns the previous handler in every case.
- **`func_8009515C()`**, `StopCallback`'s worker. Returns 0 when the entry is
  not hooked. Otherwise it calls `EnterCriticalSection()`, saves I_MASK in
  `savedMask` and DPCR in `savedPcr`, writes 0 to I_MASK and I_STAT, clears
  bit 3 of every nibble of DPCR (`&= 0x77777777`: the channel enables),
  calls `ResetEntryInt()`, clears `interruptsInitialized` and returns
  `&intrEnv`. It does not leave the critical section; `func_800951FC` does.
- **`func_800951FC()`**, `RestartCallback`'s worker. Returns 0 when the entry
  is hooked. Otherwise it calls `HookEntryInt(buf)`, sets
  `interruptsInitialized`, writes `savedMask` and `savedPcr` back, calls
  `ExitCriticalSection()` and returns `&intrEnv`.
- **`func_80094D6C()`**, `ResetCallback`'s worker. Returns 0 when the entry
  is hooked. Otherwise it writes 0 to I_MASK and I_STAT and 0x33333333 to
  DPCR, clears the environment (`func_80095274`, 0x41A words) and calls
  `setjmp(buf)`: every later interrupt returns from that `setjmp` with a
  non-zero value and calls `func_80094E44`. On the first return it replaces
  the saved `$sp` (`buf[1]`) with `&stack[1004]`, calls `HookEntryInt(buf)`,
  sets `interruptsInitialized`, stores `startIntrVSync()` in the table's
  `VSyncCallbacks` and `startIntrDMA()` in its `DMACallback`, calls
  `_96_remove()` and `ExitCriticalSection()`, and returns `&intrEnv`.
- **`func_80094E44()`**, the dispatcher, entered from `func_80094D6C`'s
  `setjmp` on every interrupt, on the environment's stack. With the entry not
  hooked it calls `printf("unexpected interrupt(%04x)\n", I_STAT)` and
  `ReturnFromException()`. Otherwise it sets `inInterrupt` and, while
  `enabledInterruptsMask & I_STAT & I_MASK` is non-zero, walks its bits from
  0 up (at most 11): for each set bit it writes `~(1 << i)` to I_STAT (the
  acknowledge) and calls `handlers[i]` when there is one. Then, if `I_STAT &
  I_MASK` is still non-zero, it counts the pass in `D_800B7940`; when the
  count was already above 0x800 it prints `"intr timeout(%04x:%04x)\n"` with
  I_STAT and I_MASK, clears the count and writes 0 to I_STAT. Otherwise it
  clears the count. Last it clears `inInterrupt` and calls
  `ReturnFromException()`. [libsnd-map.md](../../libsnd-map.md) traces the
  sound tick from here: `func_80094E44`, then `intr_vb.o`'s handler
  `func_800953C0`, then `SsSeqCalledTbyT` (`0x80098448`).
- **`func_80095274(ptr, size)`** zeroes `size` words from `ptr`; nothing is
  done for a size of 0.

Which bit is which device is read from this code no further than what it
does: bit 0 is the one `startIntrVSync` hooks and the pad driver shares, bit
3 the one `startIntrDMA` hooks, bits 4 to 6 pair with root counters 0 to 2.

## libetc — the DMA interrupt (`intr_dma.o`)

Release 4.5's `intr_dma.o` lies whole at `0x80095488` to `0x80095727` (168
words). `startIntrDMA` is its one public function (`libetc_intr_dma_1.c`);
the library gives the other three no name (`libetc_intr_dma_2.c`).

| Offset | Address | Function |
|---|---|---|
| `+0x000` | `0x80095488` | `startIntrDMA` |
| `+0x04C` | `0x800954D4` | `func_800954D4`, the interrupt-3 handler |
| `+0x1CC` | `0x80095654` | `func_80095654`, installs one channel's callback |
| `+0x278` | `0x80095700` | `func_80095700`, zeroes a number of words |

The object's data, by its relocations (initial values are the image's):

| Address | Object | Type in the C | Initial value | Meaning |
|---|---|---|---|---|
| `D_800B7980` | `.data+0x00` | `volatile u_long *` | 0x1F8010F4 | Pointer to DICR, the DMA interrupt register. |
| `D_800B7984` | `.data+0x04` | `Callback[8]` (`void (*)()`) | 0 | The DMA callback slots, one per channel. `startIntrDMA` clears all eight; `func_800954D4` serves channels 0 to 6. |
| `D_800B79A4` | `.data+0x24` | `u_long *` | 0x1F801080 | Pointer to the DMA channel registers; word `4 * n` is channel n's address register (MADR). |
| `D_80013814` | `.rdata+0x00` | string | | "DMA bus error: code=%08x\n" |
| `D_80013830` | `.rdata+0x1C` | string | | "MADR[%d]=%08x\n" |

DICR as these functions use it: bit 15 is the bit the handler reports as a
bus error, bits 16 to 22 are the per-channel interrupt enables, bit 23 the
master enable, bits 24 to 30 the per-channel flags (writing a 1 acknowledges
a flag), bit 31 the master flag. Which bits the code tests, sets and clears
is read from it; the names of the bits are the hardware's documented ones.

- **`startIntrDMA()`** clears the eight slots (`func_80095700`), writes 0 to
  DICR, installs `func_800954D4` on interrupt 3 with `InterruptCallback` and
  returns the address of `func_80095654`. `intr.o`'s `func_80094D6C` stores
  that address in the callback table's `DMACallback` entry, so
  `DMACallback(dma, f)` reaches `func_80095654`.
- **`func_800954D4()`**, the interrupt-3 handler.
  1. While `(DICR >> 24) & 0x7F` is non-zero it walks the flags from channel
     0, stopping when no flag is left or after channel 6. For a channel whose
     flag is set it writes `DICR &= 0xFFFFFF | (1 << (i + 24))` (the low 24
     bits are kept and, of the flags, only this channel's is written back as
     1, which acknowledges it), then calls the channel's callback
     `D_800B7984[i]()` when the slot is not null. The flags are read again
     after each walk.
  2. When the top byte of DICR is exactly 0x80 (the master flag with no
     channel flag) or bit 15 is set, it prints "DMA bus error: code=%08x"
     with DICR, then "MADR[%d]=%08x" for channels 0 to 6, through the empty
     `printf`.
- **`func_80095654(index, callback)`** returns the slot's previous callback
  and writes nothing when `callback` equals it. Otherwise a non-null
  `callback` is stored and DICR becomes `(DICR & 0xFFFFFF) | 0x800000 | (1 <<
  (index + 16))`; a null one clears the slot and DICR becomes `((DICR &
  0xFFFFFF) | 0x800000) & ~(1 << (index + 16))`. Both writes carry a zero
  flag byte, so they acknowledge nothing.
- **`func_80095700(ptr, size)`** zeroes `size` words from `ptr`.

## libc2 — `setjmp` and `longjmp` (`setjmp.o`)

Release 4.5's `setjmp.o` lies whole at `0x800952E8` to `0x80095367` (32
words; it has no relocations). Both functions are Sony's hand assembly, each
twelve stores or loads of `$ra`, `$gp`, `$sp`, `$fp` and `$s0` to `$s7` on
the caller's buffer with no frame, and they stay `INCLUDE_ASM` in
`libc2_setjmp.c` ([hand-assembly.md](../../hand-assembly.md)).

- **`setjmp(env)`** (`0x800952E8`, `+0x00`) saves the caller's resume point
  in `env` (setjmp.h's `jmp_buf`, twelve ints: `JB_PC` 0 = `$ra`, `JB_SP` 1,
  `JB_FP` 2, `JB_S0` to `JB_S7` 3 to 10, `JB_GP` 11) and returns 0. Its one
  caller is `intr.o`'s `func_80094D6C` (the `jal` at `0x80094DCC`), which
  gives the buffer to `HookEntryInt` as the kernel's exception exit.
- **`longjmp(env, val)`** (`0x80095324`, `+0x3C`) restores the twelve
  registers from `env` and returns through the restored `$ra` with `val` in
  `$v0` (not forced to non-zero). Nothing in the C or the remaining assembly
  calls it.

## libsnd — closing a score and resetting the library (`ssclose.o`, `ssinit.o`, `ssinit_c.o`)

Release 4.5's `ssclose.o` lies whole at `0x80095758`: `func_80095758` at
`+0x000` (`libsnd_ssclose_1.c`), `SsSeqClose` (`0x800958D4`) at `+0x17C` and
`SsSepClose` (`0x800958F8`) at `+0x1A0` (`libsnd_ssclose_2.c`), then three
pad words.

**`func_80095758(seq_sep_num)`**, the library's unnamed worker of
`SsSeqClose` and `SsSepClose`, which only call it.

1. `_SsVmSetSeqVol(n, 0, 0, 1)` (`0x8009C7D8`), then `_SsVmSeqKeyOff(n)`
   (`0x8009CE38`).
2. `D_800E9620` (`_snd_openflag`) `&= ~(1 << n)`: the tick no longer serves
   the row, and `SsSeqOpen` can hand the access number out again.
3. Every record `j` below `D_800E9EAA` (`_snd_seq_t_max`) of
   `D_800E9628[n]` (`_ss_score`) is reset, in this store order:

| Offset | Member (`SsScore`) | Value |
|---|---|---|
| `+0x98` | `flags` | 0 |
| `+0x22` | `next_seq` | -1 (none) |
| `+0x23` | `next_sep` | 0 |
| `+0x48` | `cres_vol` | 0 |
| `+0x4A` | `cres_done` | 0 |
| `+0x9C` | `cres_time` | 0 |
| `+0xA0` | `cres_tick` | 0 |
| `+0x4C` | `unk_4c` | 0 |
| `+0xAC` | `tempo_goal` | 0 |
| `+0xA8` | `tempo_count` | 0 |
| `+0xA4` | `unk_a4` | 0 |
| `+0x4E` | `tempo_step` | 0 |
| `+0x58` | `voll` | 127 |
| `+0x5A` | `volr` | 127 |

The read pointers, the per-channel arrays, the loop state and the tempo are
not written. `SsScore` is the score record of `psyq_private.h`, 0xB0 bytes.

**`_SsInit()`** (`0x800959D8`, `ssinit.o`, `libsnd_ssinit.c`) is the sound
system's reset. `SsInit` (`0x80095A78`, its own member `ssinit_c.o`,
`libsnd_ssinit_c.c`) calls `ResetCallback`, `SpuInit` (`0x8009DCD8`) and
`SpuClearReverbWorkArea(7)` (`0x800A0028`), then `_SsInit`.

1. Copies the object's sixteen-short table `D_800B79CC` to the SPU common
   registers from 0x1F801D80: 0x3FFF, 0x3FFF (main volume left and right),
   then fourteen zeros.
2. `_SsVmInit(24)` (`0x8009A728`).
3. Clears the 32 x 16 mark callbacks `D_800E96A8` (`_SsMarkCallback`).
4. `D_800E9624` (`VBLANK_MINUS`) = 60; `D_800E9620` (`_snd_openflag`) = 0;
   `D_800E961C` (`_snd_ev_flag`) = 0.

It writes no per-voice registers: `_SsVmInit` sets the voices up.

`ssinit.o`'s `.data` (`0x800B79C0`) and the library's common variables, which
the symbol table of release 4.6's `ssinit.o` gives to this object:

| Address | Sony's symbol | Size | Meaning |
|---|---|---|---|
| `D_800B79C0` | `_snd_vmask` | 4 | `.data+0x0`; 0 in the image. The voice manager tests `_snd_vmask & (1 << voice)` (`_SsVmSeqKeyOff` among others). |
| `0x800B79C4` | | 4 | `.data+0x4`: 0x00107350. Nothing in the C reads it. |
| `0x800B79C8` | | 4 | `.data+0x8`: 0x00450000 in the image, 0x00460000 in release 4.6's object: libsnd's release number ([naming.md](../../../../naming.md)). |
| `D_800B79CC` | | 0x20 | `.data+0xC`: the sixteen register reset values. |
| `D_800E9588` | `SsFCALL` | 0x94 | The SEQ reader's handler table (below). |
| `D_800E961C` | `_snd_ev_flag` | 4 | 1 while a tick runs (`SsSeqCalledTbyT`). |
| `D_800E9620` | `_snd_openflag` | 4 | Bit a = access number a is open. |
| `D_800E9624` | `VBLANK_MINUS` | 4 | 60 after `_SsInit`; `SsSetTickMode` (`0x800995E8`) rewrites it (50 in tick mode 4). |
| `D_800E9628` | `_ss_score` | 0x80 | 32 row pointers. |
| `D_800E96A8` | `_SsMarkCallback` | 0x800 | 32 x 16 mark callbacks (`SsMarkCallbackProc`). |
| `D_800E9EA8` | `_snd_seq_s_max` | 2 | Rows of `_ss_score`. |
| `D_800E9EAA` | `_snd_seq_t_max` | 2 | Records per row. |

The eight common variables are contiguous in the image, `0x800E9588` to
`0x800E9EAB`, in the order of the table.

## libsnd — opening a SEQ and the handler table (`ssopenq.o`)

### `SsFCALL` (`D_800E9588`)

The SEQ reader does not call its event handlers by name. `SsSeqOpen` fills
the table (`_SsFCALL` in libsnd.h, 0x94 bytes) and the reader goes through
it: `_SsGetSeqData` (`0x80098CD8`) for the four events and
`control[CC_NUMBER]`, `_SsSetControlChange` for the other control slots,
`_SsContDataEntry` for `ccentry`. The member names and the `CC_*` and `DE_*`
slot constants are libsnd.h's; the handler in each row is what `SsSeqOpen`
stores there.

| Offset | Member | Handler |
|---|---|---|
| `+0x00` | `noteon` | `_SsNoteOn` (`0x80097A48`) |
| `+0x04` | `programchange` | `_SsSetProgramChange` (`0x80097B28`) |
| `+0x08` | `pitchbend` | `_SsSetPitchBend` (`0x80097598`) |
| `+0x0C` | `metaevent` | `_SsGetMetaEvent` (`0x80097878`) |
| `+0x10` | `control[CC_NUMBER]` | `_SsSetControlChange` (`0x80097648`) |
| `+0x14` | `control[CC_BANKCHANGE]` | `_SsContBankChange` (`0x80095DC8`) |
| `+0x18` | `control[CC_DATAENTRY]` | `_SsContDataEntry` (`0x80095E48`) |
| `+0x1C` | `control[CC_MAINVOL]` | `_SsContMainVol` (`0x800961C8`) |
| `+0x20` | `control[CC_PANPOT]` | `_SsContPanpot` (`0x80096298`) |
| `+0x24` | `control[CC_EXPRESSION]` | `_SsContExpression` (`0x80096368`) |
| `+0x28` | `control[CC_DAMPER]` | `_SsContDamper` (`0x80096458`) |
| `+0x2C` | `control[CC_NRPN1]` | `_SsContNrpn1` (`0x80096598`) |
| `+0x30` | `control[CC_NRPN2]` | `_SsContNrpn2` (`0x80096698`) |
| `+0x34` | `control[CC_RPN1]` | `_SsContRpn1` (`0x800967D8`) |
| `+0x38` | `control[CC_RPN2]` | `_SsContRpn2` (`0x80096848`) |
| `+0x3C` | `control[CC_EXTERNAL]` | `_SsContExternal` (`0x80096508`) |
| `+0x40` | `control[CC_RESETALL]` | `_SsContResetAll` (`0x800968B8`) |
| `+0x44` | `ccentry[DE_PRIORITY]` | `_SsSetNrpnVabAttr0` (`0x80096978`) |
| `+0x48` | `ccentry[DE_MODE]` | `_SsSetNrpnVabAttr1` (`0x80096A08`) |
| `+0x4C` | `ccentry[DE_LIMITL]` | `_SsSetNrpnVabAttr2` (`0x80096AC8`) |
| `+0x50` | `ccentry[DE_LIMITH]` | `_SsSetNrpnVabAttr3` (`0x80096B58`) |
| `+0x54` | `ccentry[DE_ADSR_AR_L]` | `_SsSetNrpnVabAttr4` (`0x80096BE8`) |
| `+0x58` | `ccentry[DE_ADSR_AR_E]` | `_SsSetNrpnVabAttr5` (`0x80096D98`) |
| `+0x5C` | `ccentry[DE_ADSR_DR]` | `_SsSetNrpnVabAttr6` (`0x80096E58`) |
| `+0x60` | `ccentry[DE_ADSR_SL]` | `_SsSetNrpnVabAttr7` (`0x80096F08`) |
| `+0x64` | `ccentry[DE_ADSR_SR_L]` | `_SsSetNrpnVabAttr8` (`0x80096FB8`) |
| `+0x68` | `ccentry[DE_ADSR_SR_E]` | `_SsSetNrpnVabAttr9` (`0x80097068`) |
| `+0x6C` | `ccentry[DE_ADSR_RR_L]` | `_SsSetNrpnVabAttr10` (`0x80097128`) |
| `+0x70` | `ccentry[DE_ADSR_RR_E]` | `_SsSetNrpnVabAttr11` (`0x800971D8`) |
| `+0x74` | `ccentry[DE_ADSR_SR]` | `_SsSetNrpnVabAttr12` (`0x80097288`) |
| `+0x78` | `ccentry[DE_VIB_TIME]` | `_SsSetNrpnVabAttr13` (`0x80097368`) |
| `+0x7C` | `ccentry[DE_PORTA_DEPTH]` | `_SsSetNrpnVabAttr14` (`0x80097408`) |
| `+0x80` | `ccentry[DE_REV_TYPE]` | `_SsSetNrpnVabAttr15` (`0x800974A8`) |
| `+0x84` | `ccentry[DE_REV_DEPTH]` | `_SsSetNrpnVabAttr16` (`0x800974D8`) |
| `+0x88` | `ccentry[DE_ECHO_FB]` | `_SsSetNrpnVabAttr17` (`0x80097508`) |
| `+0x8C` | `ccentry[DE_ECHO_DELAY]` | `_SsSetNrpnVabAttr18` (`0x80097538`) |
| `+0x90` | `ccentry[DE_DELAY]` | `_SsSetNrpnVabAttr19` (`0x80097568`) |

`control[CC_BANKCHANGE]` is filled, but nothing in the C reads that slot:
`_SsSetControlChange` handles controller 0 in line.

### `SsSeqOpen` (`0x80095B38`)

`SsSeqOpen(addr, vab_id)` (`libsnd_ssopenq.c`) opens a SEQ for playing and
returns its access number.

1. If `D_800E9620` (`_snd_openflag`, unsigned) is 0xFFFFFFFF, all 32 access
   numbers are taken: it prints "Can't Open Sequence data any more\n\n"
   (`D_80013844`) through the empty `printf` and returns -1.
2. The access number is the lowest clear bit of `_snd_openflag`, found by a
   scan from bit 0; that bit is set.
3. `_SsInitSoundSeq(access number, vab_id, addr)` (`0x80097C48`) sets up
   record 0 of `_ss_score[access number]` from the SEQ header.
4. All 37 slots of `SsFCALL` are stored, on every call, in the order of
   libsnd.h's own `jt_SsInit` listing (a block under `#if 0` in the 4.4
   header on the include path): `noteon`, `programchange`, `metaevent`,
   `pitchbend`, `control[0]`, `[1]`, `[3]` to `[12]`, `[2]`, then
   `ccentry[0]` to `[19]`.
5. Returns -1 when `_SsInitSoundSeq` returned -1 (byte 7 of the header is
   not 1), else the access number. On that failure the access number's bit
   stays set in `_snd_openflag`: step 2 set it and nothing clears it. That is
   read from the code; no failing open was observed.

## libsnd — the control-change event (`midicc.o`)

`_SsSetControlChange(seq_no, sep_no, control)` (`0x80097648`,
`libsnd_midicc.c`) is the SEQ reader's control-change event (MIDI status
0xB0). `_SsGetSeqData` calls it through `SsFCALL.control[CC_NUMBER]`
(`D_800E9588 + 0x10`) with the controller number as the third argument. On
`score = &_ss_score[seq_no][sep_no]`:

1. `data = *score->read_pos++`: the controller's value byte is taken here,
   before the dispatch, for every controller.
2. A switch on the controller number hands `(seq_no, sep_no, data)` to one
   of the other `SsFCALL.control` slots; each of those handlers reads the
   next delta time itself.

| Controller | What happens | Slot |
|---|---|---|
| 0 | in line: `vab_id` (`+0x26`) = data, then `delta_value` (`+0x90`) = `_SsReadDeltaValue(seq_no, sep_no)` (`0x80097B98`) | none |
| 6 | data entry, `_SsContDataEntry` | `control[CC_DATAENTRY]`, `+0x18` |
| 7 | channel volume, `_SsContMainVol` | `control[CC_MAINVOL]`, `+0x1C` |
| 10 (0x0A) | pan, `_SsContPanpot` | `control[CC_PANPOT]`, `+0x20` |
| 11 (0x0B) | expression, `_SsContExpression` | `control[CC_EXPRESSION]`, `+0x24` |
| 64 (0x40) | damper, `_SsContDamper` | `control[CC_DAMPER]`, `+0x28` |
| 91 (0x5B) | reverb depth, `_SsContExternal` | `control[CC_EXTERNAL]`, `+0x3C` |
| 98 (0x62) | NRPN, first byte, `_SsContNrpn1` | `control[CC_NRPN1]`, `+0x2C` |
| 99 (0x63) | NRPN, second byte, `_SsContNrpn2` | `control[CC_NRPN2]`, `+0x30` |
| 100 (0x64) | RPN, `_SsContRpn1` | `control[CC_RPN1]`, `+0x34` |
| 101 (0x65) | RPN, `_SsContRpn2` | `control[CC_RPN2]`, `+0x38` |
| 121 (0x79) | reset all, `_SsContResetAll`: called with `(seq_no, sep_no)` only, the value byte is dropped | `control[CC_RESETALL]`, `+0x40` |
| any other | the value byte is dropped; `delta_value` = `_SsReadDeltaValue(seq_no, sep_no)` | none |

- Controller 0 (bank change) does not go through the table.
  `SsFCALL.control[CC_BANKCHANGE]` holds `_SsContBankChange`, whose own C has
  the same two effects, but this function does not call it.
- The handlers are called unprototyped, as libsnd.h declares the table
  (`void (*control[13]) ()`).
- The data byte each control handler receives as its third argument is the
  one read in step 1.

## libsnd — tone attributes 11 and 12 (`de_11.o`, `de_12.o`)

The NRPN data-entry handlers for tone attributes 11 and 12 are
`_SsSetNrpnVabAttr11` (`0x800971D8`, `libsnd_de_11.c`, 41 words) and
`_SsSetNrpnVabAttr12` (`0x80097288`, `libsnd_de_12.c`, 54 words). Each reads
the tone's attributes into its by-value `VagAtr` with `SsUtGetVagAtr`
(`0x80099C28`), changes the nine-short ADSR record (`SsAdsr` in
`psyq_private.h`), packs it with `_SsUtBuildADSR` (`0x80096CF4`) into
`adsr1` and `adsr2` (`+0x10`, `+0x12`) and writes the tone back with
`SsUtSetVagAtr` (`0x8009A068`). `fn_idx` is not used.

| Attr | Handler | What `data` sets |
|---|---|---|
| 11 | `_SsSetNrpnVabAttr11` | the release rate with an exponential release: `r_mode` (short 7) = 1, `rr` (short 4) = data |
| 12 | `_SsSetNrpnVabAttr12` | the sustain direction `s_dir` (short 8): data 1 to 0x3F 0, data 0x40 to 0x7F 1, any other data nothing |

Attributes 11 to 14 call `_SsUtBuildADSR` on the nine-short record without a
`_SsUtResolveADSR` (`0x80096C98`) call first, where attributes 4 to 10 do.
Attributes 13 and 14 never write the record (they set the `VagAtr`'s `vibT`
and `porW`); 11 writes two of the nine shorts, 12 at most one.
`_SsUtBuildADSR` builds both ADSR words from all nine, so the shorts these
handlers do not set are rebuilt from whatever the stack held. That is read
from the code of four of Sony's members; whether any SEQ of this game sends
NRPN attributes 11 to 14 was not checked.

`s_dir` is the short `_SsUtBuildADSR` turns into bit 0x4000 of `adsr2`;
which value means "increase" is not read from this code.

## libsnd — the voice manager: pitch, key-off, tone selection

These functions keep the voice manager's note being set up in `D_800EA6F0`
(Sony's `_svm_cur`, an `SvmCur` in `psyq_private.h`). The members they read
or write:

| Offset | Type | Member | Read or written by |
|---|---|---|---|
| `+0x00` | `s8` | `tones` | read twice by `_SsVmSelectToneAndVag` (the loop bound) |
| `+0x02` | `s8` | `note` | read by `_SsVmSelectToneAndVag` (the key-range test) and `note2pitch` |
| `+0x07` | `s8` | `fake_prog` | read by `_SsVmSelectToneAndVag`, `note2pitch2` and `_SsVmPBVoice` |
| `+0x0C` | `s8` | `tone` | written by `_SsVmPBVoice`, read by `note2pitch2` |
| `+0x10` | `s8` | `centre` | read by `note2pitch` |
| `+0x11` | `u8` | `shift` | read by `note2pitch` |
| `+0x14` | `s16` | `seq_sep` | written by `_SsVmPitchBend` |
| `+0x18` | `s16` | `voice` | written by `_SsVmSeqKeyOff` and `_SsVmPBVoice`; read by `_SsVmKeyOffNow` as an unsigned short |

The tone records are reached through `D_800EA6E0` (Sony's `_svm_tn`, a
pointer to `VagAtr`), at `fake_prog * 16 + tone`; the voices' records are
`D_800E9EC0` (Sony's `_svm_voice`, 24 `SvmVoice` of 0x38 bytes), and
`D_800EA6E4` (Sony's `_SsVmMaxVoice`) is the number of voices.

### Pitch (`vm_n2p.o`)

`vm_n2p.o` (`libsnd_vm_n2p.c`) holds three functions.

- **`note2pitch()`** (`0x8009B668`) returns
  `SsPitchFromNote(_svm_cur.note, 0, _svm_cur.centre, shift)`, `shift` being
  `_svm_cur.shift` with a value above 127 taken as 127.
- **`note2pitch2(note, fine)`** (`0x8009B6B4`) returns
  `SsPitchFromNote(note, fine, the tone's centre note, the tone's fine
  tune)`, the tone being `_svm_tn[_svm_cur.fake_prog * 16 + _svm_cur.tone]`.
  `_SsVmPBVoice` calls it with the bent note.
- **`SsPitchFromNote(note, fine, center, shift)`** (`0x8009B71C`) returns the
  SPU pitch (0x1000 = the sample's own rate) that plays a tone recorded at
  note `center` and fine tune `shift` as note `note` plus `fine` 1/128ths of
  a semitone: the product of a semitone table and a fine table (`D_800B7A10`,
  `D_800B7A28`), shifted down by the octave with rounding. From two octaves
  above the centre the result is the SPU's maximum, 0x3FFF.

### `_SsVmPitchBend` (`0x8009C574`, `vm_pb.o`)

`_SsVmPitchBend(seq_sep_no, vabId, prog, pitch)` (`libsnd_vm_pb.c`) bends
every voice that plays one program of one VAB for one score; `_SsVmPBVoice`
(`0x8009C388`, the member's other function) does the work per voice. Four
short parameters, an `int` result.

1. `_SsVmVSetUp(vabId, prog)` (`0x8009D558`). Its result is not tested.
2. `_svm_cur.seq_sep` = `seq_sep_no`.
3. For `voice` = 0 to `_SsVmMaxVoice` - 1: count += `_SsVmPBVoice(voice,
   seq_sep_no, vabId, prog, pitch)`, which returns 1 when the voice was bent
   and 0 otherwise. The fifth argument goes out as an unsigned short.
4. Returns the count.

Its one caller is `_SsSetPitchBend` (`0x80097598`), the SEQ reader's 0xE0
event, with `seq_no | sep_no << 8`, the score's VAB id (`+0x26`), the
channel's program (`+0x37 + ch`) and the data byte.

### `_SsVmSeqKeyOff` (`0x8009CE38`, `vm_seq.o`)

`_SsVmSeqKeyOff(seq_sep_num)` (`libsnd_vm_seq.c`) keys off at once every
voice that plays one score. One short parameter, no result.

For `i` = 0 to `_SsVmMaxVoice` - 1 (an unsigned char counter): if bit `i` of
`D_800B79C0` (`_snd_vmask`) is clear and `_svm_voice[i].seq_sep` (`+0x10`)
equals `seq_sep_num`, then `_svm_cur.voice` = `i` and `_SsVmKeyOffNow(0)`.

Its callers are `func_80095758` (the close), `_SsSndPause` (`0x800988C8`),
`_SsSeqGetEof` (`0x80098A94`, two sites) and `_SsSndStop` (`0x800991E8`).

### `_SsVmKeyOffNow` (`0x8009BDF8`, `vm_nowof.o`)

`_SsVmKeyOffNow(mode)` (`libsnd_vm_nowof.c`) keys off the voice
`_svm_cur.voice` at once.

1. `voice` = `_svm_cur.voice` (an unsigned short).
2. `voice < 16`: lower = `1 << voice`, upper = 0. Otherwise lower = 0, upper
   = `1 << (voice - 16)`.
3. `_svm_voice[voice]`: `key_stat` (`+0x1D`) = 0, `pitch` (`+0x04`) = 0,
   `vag` (`+0x00`) = 0, in that order.
4. `D_800EA5E8` (`_svm_okof1`) |= lower; `D_800EA5EA` (`_svm_okof2`) |=
   upper.
5. `D_800E9EB0` (`_svm_okon1`) &= ~`_svm_okof1`; `D_800E9EB2` (`_svm_okon2`)
   &= ~`_svm_okof2`: every pending key-off is taken out of the pending
   key-ons.

The parameter is never read. Callers pass 0 (`_SsVmKeyOff` (`0x8009AF94`),
`_SsVmSeqKeyOff`) or 1 (`_SsVmInit`).

### `_SsVmSelectToneAndVag` (`0x8009CEE8`, `vm_stav.o`)

`_SsVmSelectToneAndVag(toneIdx, vagIdx)` (`libsnd_vm_stav.c`) lists the
tones of the current program whose key range holds the note in `_svm_cur`,
and returns the number listed, an unsigned char.

For `i` = 0 to `_svm_cur.tones` - 1 (a signed char counter), with the tone
record `_svm_tn[_svm_cur.fake_prog * 16 + i]`: if `min` (`+6`) <=
`_svm_cur.note` and `_svm_cur.note` <= `max` (`+7`), then `vagIdx[n]` = the
low byte of the record's `vag` (`+0x16`), `toneIdx[n]` = `i`, and `n` goes
up by one.

Its one caller is `_SsVmKeyOn` (`0x8009AA78`), which then keys on each listed
tone. The note is a signed char and `min` and `max` are unsigned chars; the
compares are on the promoted ints.

## libsnd — mono and stereo, opening a VAB header (`vs_mono.o`, `vs_vh2.o`, `vs_vh.o`)

`vs_mono.o` (`libsnd_vs_mono.c`) holds `SsSetMono` (`0x8009D618`, `+0x00`),
which sets `D_800EA6D0` (Sony's `_svm_stereo_mono`) to 1, and `SsSetStereo`
(`0x8009D628`, `+0x10`), which sets it to 0. While it is 1 the volume code
gives both channels the louder of the two (psyq-libs.md, "The volume law").
The game's one caller of both is `SoundMonoSet` (`0x800550D4`,
`music_play.c`).

`SsVabOpenHead` and `SsVabOpenHeadSticky` are two public entries of
`_SsVabOpenHeadWithMode` (`0x8009D7F8`), which psyq-libs.md describes. Each
returns its result as a short.

- **`SsVabOpenHead(addr, vabid)`** (`0x8009D6E8`, `vs_vh2.o`,
  `libsnd_vs_vh2.c`) = `_SsVabOpenHeadWithMode(addr, vabid, func_8009D71C,
  0)`: the library allocates the SPU RAM. `func_8009D71C`, the member's
  unnamed second function, calls `SpuMalloc` and, when that fails, frees the
  VAB slot and returns -1. The game's one caller is `sound_vab_head_open`
  (`0x80024314`, `sound.c`), with `vabid` -1 (the first free slot).
- **`SsVabOpenHeadSticky(addr, vabid, sbaddr)`** (`0x8009D788`, `vs_vh.o`,
  `libsnd_vs_vh.c`) = `_SsVabOpenHeadWithMode(addr, vabid, func_8009D7F0,
  sbaddr)`: the samples go at the caller's SPU address (`func_8009D7F0`,
  unnamed, returns its second argument). Nothing in the C calls it.
  `SsVabFakeHead` (`0x8009D7BC`), the same member's next function, is the
  same thirteen words.

## libsnd — the reverb type (`ut_rev.o`)

Release 4.5's `ut_rev.o` lies whole at `0x80099F38` and holds both
`SsUtSetReverbType` (`+0x00`) and `SsUtGetReverbType` (`0x80099FD4`,
`+0x9C`), then one pad word (`libsnd_ut_rev.c`).

**`short SsUtSetReverbType(short type)`** (`0x80099F38`):

1. A negative `type` asks for the same type with the reverb work area
   cleared: the sign is remembered and `type` is negated.
2. If the type is not in 0..9 (`SS_REV_TYPE_OFF` to `SS_REV_TYPE_PIPE`),
   the result is -1 and nothing is written.
3. `_svm_rattr.mask` = 1 (`SPU_REV_MODE`); `_svm_rattr.mode` = the type, with
   0x100 (`SPU_REV_MODE_CLEAR_WA`) set when the argument was negative.
4. Type 0 also calls `SpuSetReverb(0)` (`0x8009F438`).
5. `SpuSetReverbModeParam(&_svm_rattr)` (`0x8009F618`); the result is the
   type.

Its one caller is `_SsSetNrpnVabAttr15` (`0x800974A8`), the handler of tone
attribute 15 (`DE_REV_TYPE`), which passes the data byte.

**`SsUtGetReverbType()`** returns `_svm_rattr.mode` as a short. Nothing in
the C calls it.

`_svm_rattr` is `D_800EA5F0`, a `SpuReverbAttr` of 0x14 bytes. The members
written here:

| Offset | Member | Written by |
|---|---|---|
| `+0x00` | `mask` | `SsUtSetReverbType`: 1 (`SPU_REV_MODE`) |
| `+0x04` | `mode` | `SsUtSetReverbType`: the type, or the type \| 0x100; read by `SsUtGetReverbType` |

## libsnd — the damper (`vm_doff.o`, `vm_don.o`)

`_SsVmDamperOff()` (`0x8009A238`, `libsnd_vm_doff.c`) sets `D_800EA688`
(Sony's `_svm_damper`) to 0; `_SsVmDamperOn()` (`0x8009A248`,
`libsnd_vm_don.c`) sets it to 2. `_SsVmDoAllocate` (`0x8009B498`) adds it to
a voice's release rate (the low five bits of ADSR2) when it allocates the
voice. `_SsVmDamperOff`'s callers are `_SsContDamper` (control change 0x40
with data below 0x40; data from 0x40 calls `_SsVmDamperOn`),
`_SsContResetAll` and `_SsSndStop`; `_SsVmInit` also writes 0.

The C declares `_svm_damper` `u16` where it is written and `s16` in
`_SsVmDoAllocate`. Its writers store only the constants 0 and 2, so they do
not settle the sign.

## libsnd — play, pause, stop and the volume read-back

The score record's play-mode byte, `+0x14` `play_mode` (`SsScore`), is 1
while the score plays: `Snd_SetPlayMode` with `SSPLAY_PLAY`, `_SsSndReplay`
(`0x80099188`) and `_SsSndNextSep` (`0x80099088`) write 1; `_SsSndPause`,
`_SsSndStop`, the end of a track and `_SsInitSoundSeq` write 0. Of the
play-flag word `+0x98`, `Snd_SetPlayMode` clears 0x200 (finished) and 4
(stop requested) and sets 1 (playing) or 2 (pause requested), and the tick
(`SsSeqCalledTbyT`) serves 2 through `_SsSndPause`.

- **`Snd_SetPlayMode(sep_no, seq_no, play_mode, l_count)`** (`0x80098018`,
  `playmode.o`, `libsnd_playmode.c`) starts the score
  `_ss_score[sep_no][seq_no]` from its first event. `read_pos` (`+0x00`),
  `loop_pos` (`+0x08`, the NRPN loop point) and `rewind_pos` (`+0x0C`) are
  all set to `start_pos` (`+0x04`, the first event); flags 0x200 and 4 are
  cleared; `l_count` (`+0x20`) = `l_count`. With `play_mode` 1
  (`SSPLAY_PLAY`): flag 1 is set, `l_count_done` (`+0x21`) = 0, `play_mode`
  = 1, and `_SsVmSetSeqVol(sep_no | seq_no << 8, voll, volr, 1)` re-applies
  the score's volume. With `play_mode` 0 (`SSPLAY_PAUSE`): flag 2 is set,
  which the next tick serves through `_SsSndPause`. Any other value does
  neither. `play_mode` is a signed char. Callers: `SsSeqPlay` (`0x80097F98`)
  with `seq_no` 0, and `SsSepPlay` (`0x80097FD0`) with all four arguments.
- **`_SsSndPause(seq, sep)`** (`0x800988C8`, `pause.o`, `libsnd_pause.c`),
  the tick's answer to flag 2: `_SsVmSeqKeyOff(seq | sep << 8)` keys off the
  score's voices, `play_mode` = 0, flag 2 is cleared. The playing flag (1) is
  left as it is.
- **`_SsSndPlay(seq, sep)`** (`0x80098968`, `play.o`, `libsnd_play.c`), the
  tick's step for a playing score (flag 1): `_SsSeqPlay(seq, sep)`
  (`0x80098998`) and nothing else.
- **`SsSeqStop(seq_access_num)`** (`0x80099368`, `ssstop.o` `+0x180`,
  `libsnd_ssstop_2.c`): `_SsSndStop(seq_access_num, 0)`. The game's caller is
  `sound_seq_stop` (`0x80023A20`, `sound.c`).
- **`SsSepStop(sep_access_num, seq_no)`** (`0x80099390`, `ssstop.o`
  `+0x1A8`): `_SsSndStop(sep_access_num, seq_no)`.
- **`SsSeqGetVol(access_num, seq_no, voll, volr)`** (`0x800998C0`, `ssvol.o`
  `+0x188`, `libsnd_ssvol.c`): `_SsVmGetSeqVol(access_num | seq_no << 8,
  voll, volr)` (`0x8009CD44`), which stores the score's `voll` and `volr`
  through the two pointers. libsnd.h's signature is `(short, short, short *,
  short *)`.

`_SsSndCrescendo` (`0x800986B8`, `cres.o`) reads `cres_done` (`+0x4A`)
signed: the member is `s16` ([codegen.md](codegen.md), "Types").

## libsnd — the tick setting `_snd_seq_tick_env` (`ssstart.o`)

The tick setting `D_800B79F0` (Sony's `_snd_seq_tick_env`, `SndSeqTickEnv`
in `psyq_private.h`) is 0x14 bytes. `SsSetTickMode` chooses it,
`func_80098138` (the library's unnamed installer of the tick, called by
`SsStart` (`0x80098368`) with 1 and `SsStart2` (`0x80098388`) with 0)
installs the tick by it, the handlers `func_800983A8` and `func_800983F4`
and `SsEnd` (`0x80095928`) read it. The word after it, `D_800B7A04`, is not
a member: it is a variable of `ssstart.o`'s own, the phase of
`func_800983F4`, the handler `func_80098138` installs for a tick rate below
70 Hz, where the root counter runs at twice the rate. `func_800983F4`
(`libsnd_ssstart_4.c`) sets the phase to 1 on one call and on the next sets
it to 0 and calls `tick_cb`, so the tick runs every second call.

What shows that `+0x14` is a variable of its own and not a member:

- No other function reads or writes `+0x14`. Every reference to
  `_snd_seq_tick_env` from elsewhere (`SsEnd`, `SsSetTickMode`,
  `func_800983A8`, `func_80098138`) stays below 0x14.
- In Sony's `ssstart.o` (releases 4.6 and 4.7) the only global in `.data` is
  `_snd_seq_tick_env`, at offset 0; the handler at `.text+0x2BC`
  (`func_800983F4`) reaches `.data+0x14` three times and `.data+0x08` once,
  each through its own `lui`.
- `func_80098138` reaches four members of the struct off one base register,
  and a compiler shares a base among the members of one object; three
  unshared loads of `+0x14` mean a scalar ([codegen.md](codegen.md),
  "Symbols: one or several": written as a member, `func_800983F4` comes out
  9 of 18 instructions right).

`func_80098138` and `_SsSndTempo` (`0x800998F8`, `tempo.o`, the tempo ramp)
are described in [psyq-libs.md](../psyq-libs.md), "libsnd — the tick".

## Functions on this page

| Address | Function | Member | File |
|---|---|---|---|
| `0x800949A8` | `VSync` | libetc `vsync.o` | `libetc_vsync_1.c` |
| `0x80094B20` | `func_80094B20` | libetc `vsync.o` | `libetc_vsync_2.c` |
| `0x80094BD8` to `0x80094D3C` | `ResetCallback` to `GetIntrMask` (nine) | libetc `intr.o` | `libetc_intr_1.c` |
| `0x80094D54` | `SetIntrMask` | libetc `intr.o` | `libetc_intr_2.c` |
| `0x80094D6C` | `func_80094D6C` | libetc `intr.o` | `libetc_intr_2.c` |
| `0x80094E44` | `func_80094E44` | libetc `intr.o` | `libetc_intr_2.c` |
| `0x80095014` | `func_80095014` | libetc `intr.o` | `libetc_intr_2.c` |
| `0x8009515C` | `func_8009515C` | libetc `intr.o` | `libetc_intr_2.c` |
| `0x800951FC` | `func_800951FC` | libetc `intr.o` | `libetc_intr_2.c` |
| `0x80095274` | `func_80095274` | libetc `intr.o` | `libetc_intr_2.c` |
| `0x800952E8` | `setjmp` | libc2 `setjmp.o` | `libc2_setjmp.c` |
| `0x80095324` | `longjmp` | libc2 `setjmp.o` | `libc2_setjmp.c` |
| `0x80095488` | `startIntrDMA` | libetc `intr_dma.o` | `libetc_intr_dma_1.c` |
| `0x800954D4` | `func_800954D4` | libetc `intr_dma.o` | `libetc_intr_dma_2.c` |
| `0x80095654` | `func_80095654` | libetc `intr_dma.o` | `libetc_intr_dma_2.c` |
| `0x80095700` | `func_80095700` | libetc `intr_dma.o` | `libetc_intr_dma_2.c` |
| `0x80095758` | `func_80095758` | libsnd `ssclose.o` | `libsnd_ssclose_1.c` |
| `0x800958D4` | `SsSeqClose` | libsnd `ssclose.o` | `libsnd_ssclose_2.c` |
| `0x800958F8` | `SsSepClose` | libsnd `ssclose.o` | `libsnd_ssclose_2.c` |
| `0x800959D8` | `_SsInit` | libsnd `ssinit.o` | `libsnd_ssinit.c` |
| `0x80095A78` | `SsInit` | libsnd `ssinit_c.o` | `libsnd_ssinit_c.c` |
| `0x80095B38` | `SsSeqOpen` | libsnd `ssopenq.o` | `libsnd_ssopenq.c` |
| `0x800971D8` | `_SsSetNrpnVabAttr11` | libsnd `de_11.o` | `libsnd_de_11.c` |
| `0x80097288` | `_SsSetNrpnVabAttr12` | libsnd `de_12.o` | `libsnd_de_12.c` |
| `0x80097648` | `_SsSetControlChange` | libsnd `midicc.o` | `libsnd_midicc.c` |
| `0x80098018` | `Snd_SetPlayMode` | libsnd `playmode.o` | `libsnd_playmode.c` |
| `0x800983F4` | `func_800983F4` | libsnd `ssstart.o` | `libsnd_ssstart_4.c` |
| `0x800988C8` | `_SsSndPause` | libsnd `pause.o` | `libsnd_pause.c` |
| `0x80098968` | `_SsSndPlay` | libsnd `play.o` | `libsnd_play.c` |
| `0x80099368` | `SsSeqStop` | libsnd `ssstop.o` | `libsnd_ssstop_2.c` |
| `0x80099390` | `SsSepStop` | libsnd `ssstop.o` | `libsnd_ssstop_2.c` |
| `0x800998C0` | `SsSeqGetVol` | libsnd `ssvol.o` | `libsnd_ssvol.c` |
| `0x80099F38` | `SsUtSetReverbType` | libsnd `ut_rev.o` | `libsnd_ut_rev.c` |
| `0x80099FD4` | `SsUtGetReverbType` | libsnd `ut_rev.o` | `libsnd_ut_rev.c` |
| `0x8009A238` | `_SsVmDamperOff` | libsnd `vm_doff.o` | `libsnd_vm_doff.c` |
| `0x8009A248` | `_SsVmDamperOn` | libsnd `vm_don.o` | `libsnd_vm_don.c` |
| `0x8009B668` | `note2pitch` | libsnd `vm_n2p.o` | `libsnd_vm_n2p.c` |
| `0x8009B6B4` | `note2pitch2` | libsnd `vm_n2p.o` | `libsnd_vm_n2p.c` |
| `0x8009B71C` | `SsPitchFromNote` | libsnd `vm_n2p.o` | `libsnd_vm_n2p.c` |
| `0x8009BDF8` | `_SsVmKeyOffNow` | libsnd `vm_nowof.o` | `libsnd_vm_nowof.c` |
| `0x8009C388` | `_SsVmPBVoice` | libsnd `vm_pb.o` | `libsnd_vm_pb.c` |
| `0x8009C574` | `_SsVmPitchBend` | libsnd `vm_pb.o` | `libsnd_vm_pb.c` |
| `0x8009CE38` | `_SsVmSeqKeyOff` | libsnd `vm_seq.o` | `libsnd_vm_seq.c` |
| `0x8009CEE8` | `_SsVmSelectToneAndVag` | libsnd `vm_stav.o` | `libsnd_vm_stav.c` |
| `0x8009D618` | `SsSetMono` | libsnd `vs_mono.o` | `libsnd_vs_mono.c` |
| `0x8009D628` | `SsSetStereo` | libsnd `vs_mono.o` | `libsnd_vs_mono.c` |
| `0x8009D6E8` | `SsVabOpenHead` | libsnd `vs_vh2.o` | `libsnd_vs_vh2.c` |
| `0x8009D71C` | `func_8009D71C` | libsnd `vs_vh2.o` | `libsnd_vs_vh2.c` |
| `0x8009D788` | `SsVabOpenHeadSticky` | libsnd `vs_vh.o` | `libsnd_vs_vh.c` |
| `0x8009D7F0` | `func_8009D7F0` | libsnd `vs_vh.o` | `libsnd_vs_vh.c` |
