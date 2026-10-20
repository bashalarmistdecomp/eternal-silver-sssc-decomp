# libspu

Part of Sony's sound-processor library as the main executable links it
(PSY-Q 4.5): `spu.o`, the library's hardware layer; `s_ini.o`, its shared
state, with `_SpuInit` and `SpuStart`; the allocator's first two members
(`SpuInitMalloc`, `SpuMalloc`); `SpuSetKey`, `SpuWrite` and
`SpuClearReverbWorkArea`; and libsnd's `SsVabFakeHead`, which sits beside
them. For each: what the functions do, the state they read and write, and who
calls them. The libsnd and libspu functions the music path reaches are mapped
in [libsnd-map.md](../../libsnd-map.md), every library function's file is in
[index.md](index.md), the libraries as a whole are in
[psyq-libs.md](../psyq-libs.md), and how Sony's objects were compiled is
[codegen.md](codegen.md).

## The members and the release

Every member below lies whole in the image, each word equal to PSY-Q 4.5's
outside the fields the linker fills; each function's row in the register names
its member and its offset in it ([naming.md](../../../../naming.md), "How the
SDK names were proven"). The libspu code of releases 4.4, 4.5 and 4.6 is the
same; the release is fixed by the version word `s_ini.o` keeps in its data,
0x00450000 at 0x800B7B34, where the 4.4, 4.6 and 4.7 objects hold their own
numbers.

| Member | In the image | Functions | File |
|---|---|---|---|
| `s_i.o` | 0x8009DCD8 | `SpuInit` | `libspu_s_i.c` |
| `s_ini.o` | 0x8009DCF8 to 0x8009DE57 | `_SpuInit`, `SpuStart` | `libspu_s_ini_1.c`, `libspu_s_ini_2.c` |
| `spu.o` | 0x8009DE78 to 0x8009E9B7 | fourteen (below) | `libspu_spu_1.c`, `libspu_spu_2.c`, `libspu_spu_3.c` |
| `s_dcb.o` | 0x8009E9B8 | `_SpuDataCallback` | `libspu_s_dcb.c` |
| `s_m_init.o` | 0x8009E9E8 | `SpuInitMalloc` | `libspu_s_m_init.c` |
| `s_m_m.o` | 0x8009EA48 | `SpuMalloc` | `libspu_s_m_m.c` |
| `s_crwa.o` | 0x800A0028 to 0x800A01C7 | `SpuClearReverbWorkArea` | `libspu_s_crwa.c` |
| `s_sk.o` | 0x800A01D8 to 0x800A0397 | `SpuSetKey` | `libspu_s_sk.c` |
| `s_w.o` | 0x800A0398 to 0x800A03F7 | `SpuWrite` | `libspu_s_r.c` |

Between them lie libapi's BIOS call stubs `OpenEvent` (`0x8009DE58`),
`EnableEvent` (`0x8009DE68`) and `WaitEvent` (`0x800A01C8`), each a member of
its own (`a08.o`, `a12.o`, `a10.o`).

## spu.o: the hardware layer

`spu.o` owns the pointers to the SPU's and the DMA controller's registers, and
the other libspu members reach the hardware through it.

| Offset | Address | Function | File |
|---|---|---|---|
| `+0x000` | `0x8009DE78` | `_spu_init` | `libspu_spu_1.c` |
| `+0x280` | `0x8009E0F8` | `func_8009E0F8` (static) | `libspu_spu_1.c` |
| `+0x440` | `0x8009E2B8` | `_spu_FiDMA` | `libspu_spu_2.c` |
| `+0x4FC` | `0x8009E374` | `_spu_Fr_` | `libspu_spu_3.c` |
| `+0x5A4` | `0x8009E41C` | `_spu_t` | `libspu_spu_3.c` |
| `+0x824` | `0x8009E69C` | `_spu_Fw` | `libspu_spu_3.c` |
| `+0x8A8` | `0x8009E720` | `_spu_Fr` | `libspu_spu_3.c` |
| `+0x90C` | `0x8009E784` | `_spu_FsetRXX` | `libspu_spu_3.c` |
| `+0x950` | `0x8009E7C8` | `_spu_FsetRXXa` | `libspu_spu_3.c` |
| `+0x9F4` | `0x8009E86C` | `_spu_FgetRXXa` | `libspu_spu_3.c` |
| `+0xA30` | `0x8009E8A8` | `_spu_FsetPCR` | `libspu_spu_3.c` |
| `+0xA88` | `0x8009E900` | `func_8009E900` (static) | `libspu_spu_3.c` |
| `+0xAB0` | `0x8009E928` | `func_8009E928` (static) | `libspu_spu_3.c` |
| `+0xAD8` | `0x8009E950` | `_spu_Fw1ts` | `libspu_spu_3.c` |

The member's `.text` is 0xB34 bytes and 12 bytes of padding. The three
statics have no symbol in the member, and neither Sony's text nor any release
from 2.6 to 4.7 names their bytes, so they keep their addresses (the
register's rows; [naming.md](../../../../naming.md), "Sony's statics").

### The member's data

The `.data` starts at 0x800B7BB0. The names are the member's symbols; a
static has none. The initial values are the image's.

| Offset | Tree symbol | Member's symbol | Initial value | Meaning |
|---|---|---|---|---|
| `+0x00` | `D_800B7BB0` | `_spu_RXX` | 0x1F801C00 | Pointer to the SPU's register block (`SpuUnion`, below). |
| `+0x04` | `D_800B7BB4` | static | 0x1F8010C0 | Pointer to DMA channel 4's address register (MADR). |
| `+0x08` | `D_800B7BB8` | static | 0x1F8010C4 | Pointer to its block size and count (BCR). |
| `+0x0C` | `D_800B7BBC` | static | 0x1F8010C8 | Pointer to its control register (CHCR). |
| `+0x10` | `D_800B7BC0` | static | 0x1F8010F0 | Pointer to DPCR, the DMA channels' enables and priorities; `_spu_init` and `_spu_FsetPCR` write it. |
| `+0x14` | `D_800B7BC4` | static | 0x1F801014 | Pointer to the SPU's bus-timing word: `func_8009E900` and `func_8009E928` clear its bits 24-27 and OR in 0x20000000 (writing) or 0x22000000 (reading). |
| `+0x18` | `D_800B7BC8` | `_spu_tsa` | 0 | `u16`: the transfer's start address in SPU units (bytes `>> _spu_mem_mode_plus`). |
| `+0x1C` | `D_800B7BCC` | `_spu_transMode` | 0 | 0: transfers go by DMA (`_spu_t`); not 0: `_spu_Fw` writes through the data port (`func_8009E0F8`). |
| `+0x20` | `D_800B7BD0` | `_spu_addrMode` | 0 | `_spu_init` clears it; nothing here reads it. |
| `+0x24` | `D_800B7BD4` | `_spu_mem_mode` | 2 | Not 0: `_spu_FsetRXXa` rounds addresses up to the unit. `_spu_init` sets 2. |
| `+0x28` | `D_800B7BD8` | `_spu_mem_mode_plus` | 3 | The shift between a byte address and SPU units. |
| `+0x2C` | `D_800B7BDC` | `_spu_mem_mode_unit` | 8 | The address unit in bytes. |
| `+0x30` | `D_800B7BE0` | `_spu_mem_mode_unitM` | 7 | The unit's mask. |
| `+0x34` | `D_800B7BE4` | `_spu_inTransfer` | 1 | Inverted sense: 1 = no transfer running, 0 = one is. `_spu_init` sets 1; `SpuWrite` clears it when no callback is set; `_spu_setInTransfer` (`0x800A0548`) sets either value; `SpuIsTransferCompleted` sets 1 once the transfer-complete event has fired. |
| `+0x38` | `D_800B7BE8` | `_spu_transferCallback` | 0 | The user's transfer-end callback, a volatile function pointer; `_spu_FiDMA` calls it. |
| `+0x3C` | `D_800B7BEC` | `_spu_IRQCallback` | 0 | The user's SPU-interrupt callback; `_spu_init` clears it. |
| `+0x40` | `D_800B7BF0` | static | sixteen bytes of 7 | The block `_spu_init` writes to SPU RAM 0x1000. |
| `+0x50` | `D_800B7C00` | static | 0 | The direction of the DMA transfer in hand: 1 = read (`_spu_Fr_`, `_spu_t(0)`), 0 = write (`_spu_t(1)`); `_spu_t(3)` and `_spu_FiDMA` read it. |
| `+0x54` | `D_800B7C04` | static | 0 | The transfer's memory address, as last given to `_spu_t(3)`. |
| `+0x58` | `D_800B7C08` | static | 0 | The transfer's size in 64-byte blocks. |
| `.bss` | `D_800EA8C0` | `_spu_RQ` | | Volatile `u16[10]`: the queued register words from 0xC4 up, two per register pair: `[0]` / `[1]` key on, `[2]` / `[3]` key off, `[4]` / `[5]` pitch modulation, `[6]` / `[7]` noise, `[8]` / `[9]` reverb. |

### The register block

`_spu_RXX` points at 0x200 bytes of SPU registers. The C reaches them as
`SpuUnion` (`include/psyq_private.h`): the registers by name (`SpuRXX`), or the
same bytes as 0x100 16-bit words for the functions that are handed a register
number (`_spu_FsetRXX`, `_spu_FsetRXXa`, `SpuSetVoiceAttr`). The registers
these members use:

| Offset | Member | Register |
|---|---|---|
| `+0x000` | `voice[24]` (`SpuVoiceReg`, 0x10 each) | volume left / right, pitch `+4`, start address `+6`, ADSR `+8` / `+0xA`, envelope `+0xC`, loop address `+0xE` |
| `+0x180` | `main_vol` | main volume, left / right |
| `+0x184` | `rev_vol` | reverb depth, left / right |
| `+0x188` | `key_on[2]` | key on, voices 0-15 and 16-23 |
| `+0x18C` | `key_off[2]` | key off |
| `+0x190` | `chan_fm[2]` | pitch modulation on |
| `+0x194` | `noise_mode[2]` | noise on |
| `+0x198` | `rev_mode[2]` | reverb on |
| `+0x1A2` | `rev_work_addr` | the reverb work area's start (word 0xD1) |
| `+0x1A6` | `trans_addr` | transfer address |
| `+0x1A8` | `trans_fifo` | transfer FIFO, the data port |
| `+0x1AA` | `spucnt` | SPUCNT |
| `+0x1AC` | `data_trans` | transfer control (4) |
| `+0x1AE` | `spustat` | SPUSTAT |
| `+0x1B0` | `cd_vol` | CD input volume |
| `+0x1B4` | `ex_vol` | external input volume |

The meanings of the SPUCNT and SPUSTAT bits are the PlayStation hardware's;
the code shows only the values it writes and tests. As the hardware has them:
SPUCNT bits 4-5 are the transfer mode (0 stop, 1 manual write, 2 DMA write,
3 DMA read), bit 7 reverb, bit 14 unmute, bit 15 enable (0xC000 at the end of
the reset); SPUSTAT bit 10 is "transfer busy".

### The functions

`_spu_init(bHot)` (`0x8009DE78`) resets the SPU. `bHot != 0` skips the part
that touches the voices. Returns 0. Caller: `_SpuInit`.

1. DPCR `|= 0xB0000`: DMA channel 4 enabled at priority 3.
2. Main volume 0, SPUCNT 0; `_spu_transMode`, `_spu_addrMode` and `_spu_tsa`
   cleared; one `_spu_Fw1ts`; main volume 0 again.
3. SPUSTAT `& 0x7FF` is polled until it reads 0. After 0xF01 polls it prints
   `"SPU:T/O [%s]\n"` with `"wait (reset)"` and goes on. The print goes to
   `printf` (`0x80021700`), which is an empty function in this program.
4. `_spu_mem_mode` 2, `_spu_mem_mode_plus` 3, `_spu_mem_mode_unit` 8,
   `_spu_mem_mode_unitM` 7.
5. Transfer control 4, reverb depth 0, key off 0xFFFF / 0xFFFF, reverb on 0,
   and the ten words of `_spu_RQ` 0.
6. Cold only (`bHot == 0`): pitch modulation and noise off, CD and external
   volume 0; `_spu_tsa` = 0x200 and `func_8009E0F8(D_800B7BF0, 0x10)`, the
   sixteen bytes to SPU RAM 0x1000; each of the 24 voices volume 0 / 0, pitch
   0x3FFF, start address 0x200 (the block just written), ADSR 0 / 0; key on
   0xFFFF / 0xFF, four `_spu_Fw1ts`, key off 0xFFFF / 0xFF, four more.
7. `_spu_inTransfer` = 1, SPUCNT = 0xC000, `_spu_transferCallback` and
   `_spu_IRQCallback` cleared.

`func_8009E0F8(addr, size)` (`0x8009E0F8`, static) writes `size` bytes at
`addr` to SPU RAM through the data port, without DMA. Callers: `_spu_init`
(the sixteen bytes) and `_spu_Fw` when `_spu_transMode` is not 0. Returns
nothing.

1. It keeps SPUSTAT `& 0x7FF`, sets the transfer address register to
   `_spu_tsa` and waits one `_spu_Fw1ts`.
2. While bytes remain, in blocks of at most 0x40: the block's 16-bit words go
   into the transfer FIFO; SPUCNT's transfer mode = 1; one `_spu_Fw1ts`;
   SPUSTAT bit 10 is polled until clear (0xF01 polls, then the time-out print
   with `"wait (wrdy H -> L)"`); two more `_spu_Fw1ts`.
3. SPUCNT's transfer mode = 0; SPUSTAT `& 0x7FF` is polled until it equals the
   value kept in step 1 (time-out print with `"wait (dmaf clear/W)"`).

A block of odd length writes the word that holds its last byte: the count
runs in steps of two below the block's length.

`_spu_FiDMA()` (`0x8009E2B8`) is the SPU's DMA-end interrupt handler. Nothing
calls it; `SpuStart` passes its address to `_SpuDataCallback`.

1. After a write (`D_800B7C00` is 0), one `_spu_Fw1ts`.
2. SPUCNT `&= ~0x30` (transfer mode 0), then a poll until those two bits read
   0, at most 0xF01 times, with no print on time-out.
3. If `_spu_transferCallback` is set, it is called. Otherwise
   `DeliverEvent(0xF0000009, 0x20)` (`0x80090448`; HwSPU, EvSpCOMP), the event
   `SpuIsTransferCompleted` tests through `_spu_EVdma`.

`_spu_Fr_(addr, tsa, blocks)` (`0x8009E374`) starts a DMA read of SPU RAM:
transfer address register = `tsa`; one `_spu_Fw1ts`; SPUCNT `|= 0x30`
(transfer mode 3); one `_spu_Fw1ts`; `func_8009E928()`; then DMA channel 4:
MADR = `addr`, BCR = `(blocks << 16) | 0x10`, `D_800B7C00` = 1, CHCR =
0x01000200. Nothing in this program calls it or takes its address; the read
path that is linked goes through `_spu_t`. The parameter names are the
tree's; the widths are what the code needs (`tsa` is stored as 16 bits,
`blocks` only shifted).

`_spu_t(mode, ...)` (`0x8009E41C`) is variadic; each call does one step of an
SPU-RAM DMA transfer.

* mode 2 `(2, address)`: `_spu_tsa = address >> _spu_mem_mode_plus`, and the
  same value to the transfer address register.
* mode 1 `(1)` (write) and mode 0 `(0)` (read): the direction goes to
  `D_800B7C00` (0 / 1); the transfer address register is polled until it reads
  back `_spu_tsa`; then SPUCNT's bits 4-5 are set, mode 1 `(spucnt & ~0x30) |
  0x20`, mode 0 `spucnt | 0x30`.
* mode 3 `(3, buffer, size)`: SPUCNT is polled until bits 4-5 equal the mode
  for the stored direction (0x30 reading, 0x20 writing); the bus timing is set
  (`func_8009E928` reading, `func_8009E900` writing); `buffer` goes to
  `D_800B7C04` and `size / 64`, plus one if `size % 64` is not 0, to
  `D_800B7C08`; then DMA channel 4: MADR = `D_800B7C04`, BCR = `(D_800B7C08 <<
  16) | 0x10`, CHCR = 0x01000200 reading or 0x01000201 writing.

Each poll gives up at its 0xF01st failed read and the function returns -2.
Every other path returns 0, a mode outside 0 to 3 included, which does
nothing. Callers: `_spu_Fw` (modes 2, 1, 3), `_spu_Fr` (modes 2, 0, 3) and
`SpuClearReverbWorkArea`. The DMA registers' meanings (address, block count,
control) and the bus timing are the hardware's.

`_spu_Fw(addr, size)` (`0x8009E69C`) writes `size` bytes at `addr` to SPU RAM
at `_spu_tsa`. With `_spu_transMode` 0 it is a DMA, by `_spu_t(2, _spu_tsa <<
_spu_mem_mode_plus)`, `_spu_t(1)` and `_spu_t(3, addr, size)`; otherwise
`func_8009E0F8(addr, size)`. Returns `size`. Caller: `SpuWrite`.

`_spu_Fr(addr, size)` (`0x8009E720`) is the same read, always by DMA (`_spu_t`
modes 2, 0, 3). Returns `size`. No caller in this program.

`_spu_FsetRXX(offset, value, mode)` (`0x8009E784`) writes SPU register word
`offset`: `value` as it is with mode 0, `value >> _spu_mem_mode_plus` with any
other. Callers: `_SpuInit`, `SpuSetReverbModeParam` and `SpuSetReverbModeType`,
all with word 0xD1 (the reverb work area's start) and mode 0.

`_spu_FsetRXXa(offset, unit)` (`0x8009E7C8`) writes a byte address to an SPU
address register.

1. If `_spu_mem_mode` is not 0 and `unit % _spu_mem_mode_unit` is not 0:
   `unit += _spu_mem_mode_unit; unit &= ~_spu_mem_mode_unitM`, rounded up to the
   next multiple of 8.
2. `value = (u16)(unit >> _spu_mem_mode_plus)`.
3. By `offset`: -1 returns `value` and writes nothing; -2 returns the rounded
   `unit` and writes nothing; any other value stores `value` in register word
   `offset` (`_spu_RXX` + `offset * 2`) and returns the rounded `unit`.

Callers: `SpuSetVoiceAttr` (words `voice * 8 + 3`, the sample's start
address, and `voice * 8 + 7`, the loop address) and `SpuSetTransferStartAddr`
(offset -1: it wants only the register value, for `_spu_tsa`).

`_spu_FgetRXXa(offset, mode)` (`0x8009E86C`) reads register word `offset`:
as read with mode -1, `<< _spu_mem_mode_plus` (a byte address) with any other.
No caller in this program.

`_spu_FsetPCR(flag)` (`0x8009E8A8`) sets DMA channel 4's priority in DPCR
(bits 16-18): cleared, then 3 when `flag` is set and 5 when it is not; the
channel's enable bit is left alone. No caller in this program.

`func_8009E900` and `func_8009E928` (static) set the SPU's bus timing for a
write and for a read, through `D_800B7BC4`.

`_spu_Fw1ts()` (`0x8009E950`) is a short busy wait: a volatile counter runs
0 to 59 while a volatile value that starts at 13 is multiplied by 13 each
turn, and the value is never used. Callers, all in `spu.o`: `_spu_init`,
`func_8009E0F8`, `_spu_FiDMA` and `_spu_Fr_`.

## s_ini.o: the shared state, `_SpuInit` and `SpuStart`

`s_ini.o` holds libspu's shared state. Its `.data` is 0x80 bytes from
0x800B7B30, equal to the release's. The names are the member's symbols.

| Offset | Tree symbol | Member's symbol | Meaning |
|---|---|---|---|
| `+0x00` | 0x800B7B30 | none | 0x00117350, then at `+4` the release word 0x00450000 |
| `+0x08` | `D_800B7B38` | `_spu_EVdma` | the transfer-complete event's descriptor; `SpuStart` opens it, `SpuClearReverbWorkArea` waits on it |
| `+0x0C` | `D_800B7B3C` | `_spu_keystat` | `s32`: `SpuSetKey`'s direct arm ORs the keyed-on mask in and clears the keyed-off one; `_SpuInit` zeroes it; nothing in the image reads it back |
| `+0x10` | `D_800B7B40` | `_spu_trans_mode` | the mode `SpuSetTransferMode` (`0x800A0458`) was given; `SpuIsTransferCompleted` (`0x800A0488`) reports a transfer complete at once while it is 1; `_SpuInit` zeroes it |
| `+0x14` | `D_800B7B44` | `_spu_rev_flag` | reverb on (1) or off (0): `SpuSetReverb` (`0x8009F438`) stores it beside SPUCNT bit 7 and returns it, and stores 0 when switching on is refused (the work area is not reserved and `_SpuIsInAllocateArea_` finds it allocated); `_SpuInit` zeroes it |
| `+0x18` | `D_800B7B48` | `_spu_rev_reserve_wa` | not 0: the reverb work area is reserved; `SpuMalloc` keeps clear of it |
| `+0x1C` | `D_800B7B4C` | `_spu_rev_offsetaddr` | the reverb work area's start in SPU units; `_SpuInit` sets `_spu_rev_startaddr[0]` |
| `+0x20` | `_spu_rev_attr` | `_spu_rev_attr` | Sony's `SpuReverbAttr`, 0x14 bytes: mask, mode, depth, delay, feedback |
| `+0x34` | `D_800B7B64` | `_spu_RQvoice` | `s32`, volatile: the voices `SpuSetKey`'s queue arm keys; `_SpuInit` zeroes it; nothing reads it back |
| `+0x38` | `D_800B7B68` | `_spu_RQmask` | `s32`, volatile: which queued register pairs wait; `SpuSetKey` sets bit 0, for key on and key off alike; `_SpuSetAnyVoice` (`0x8009F0C8`, `libspu_s_sav.c`) sets bit `(addr1 − 0xC6) >> 1` |
| `+0x3C` | `D_800B7B6C` | `_spu_voice_centerNote` | 24 `u16`, each 0xC000 in the image; `_SpuInit` sets them again on a cold start; `SpuSetVoiceAttr` (`0x800A0588`) stores a voice's sample note there and reads it back for the pitch |
| `+0x6C` | `D_800B7B9C` | `_spu_env` | bit 0: queue register writes instead of making them |
| `+0x70` | `D_800B7BA0` | `_spu_isCalled` | 1 once `SpuStart` has run |

`_spu_env` is 0 in `.data`, and in this image only `_SpuInit` stores to it,
a 0; `SpuSetEnv` is not linked. So the queue arms of `SpuSetKey` and
`_SpuSetAnyVoice` never run here.

`SpuInit()` (`0x8009DCD8`) is `_SpuInit(0)`, a cold start. Caller: `SsInit`.

`_SpuInit(bHot)` (`0x8009DCF8`) calls `ResetCallback`, then `_spu_init(bHot)`.
On a cold start the 24 words of `_spu_voice_centerNote` become 0xC000. Then
`SpuStart`, and libspu's own state is cleared: reverb off
(`_spu_rev_flag`, `_spu_rev_reserve_wa`, and the mode, depth, delay and
feedback of `_spu_rev_attr`); `_spu_rev_offsetaddr = _spu_rev_startaddr[0]`,
also written to register word 0xD1 by `_spu_FsetRXX`; no allocation table
(`D_800B7C10`, `D_800B7C14`, `D_800B7C18`); transfers by DMA
(`_spu_trans_mode`, `_spu_transMode`); no key status, an empty queue
(`_spu_RQmask`, `_spu_RQvoice`) and `_spu_env` 0.

`SpuStart()` (`0x8009DDE0`) does its work once: if `_spu_isCalled` is 0 it
sets it to 1 and, between `EnterCriticalSection` (`0x8008B3B8`) and
`ExitCriticalSection` (`0x8008B3C8`), calls `_SpuDataCallback(_spu_FiDMA)`
and opens and enables the transfer-complete event: `_spu_EVdma =
OpenEvent(0xF0000009, 0x20, 0x2000, 0)` (HwSPU, EvSpCOMP, EvMdNOINTR, no
handler), then `EnableEvent(_spu_EVdma)`. Caller: `_SpuInit`, after
`_spu_init`.

`_SpuDataCallback(func)` (`0x8009E9B8`) is `DMACallback(4, func)`: `func`
becomes DMA channel 4's interrupt callback.

## The allocator: `SpuInitMalloc` and `SpuMalloc`

The allocation table is an array of `SPU_MALLOC` records
(`include/psyq_private.h`): `addr`, the block's SPU address in bits 0-27 with
bit 31 marking a free record and bit 30 the table's end (the unallocated
tail), and `size` in bytes. Three words of libspu's data, right after
`spu.o`'s, hold it:

| Tree symbol | Member's symbol | Meaning |
|---|---|---|
| `D_800B7C10` | `_spu_AllocBlockNum` | `s32`: the records the table may hold |
| `D_800B7C14` | `_spu_AllocLastNum` | the index of the last record, the tail |
| `D_800B7C18` | `_spu_memList` | the table, in the caller's buffer |

`SpuInitMalloc(num, top)` (`0x8009E9E8`) starts the table in the caller's
buffer `top`. If `num <= 0` it returns 0 and writes nothing. Otherwise
`_spu_memList = top`; record 0 becomes the tail, `addr = 0x40001010` (the
tail flag and SPU address 0x1010) and `size = (0x10000 << _spu_mem_mode_plus)
- 0x1010` (the rest of SPU RAM, 0x7EFF0 bytes with the shift of 3);
`_spu_AllocLastNum = 0`; `_spu_AllocBlockNum = num`. Returns `num`. The buffer
must hold `num + 1` records: `num` blocks and the tail. Caller: `_SsVmInit`
(`0x8009A728`), with 32 and libsnd's own table `D_800EA7B8`.

`SpuMalloc(size)` (`0x8009EA48`) allocates `size` bytes of SPU RAM from the
table and returns the block's SPU address, or -1.

1. `reserved` is 0, or, when `_spu_rev_reserve_wa` is not 0,
   `(0x10000 - _spu_rev_offsetaddr) << _spu_mem_mode_plus`: the size in bytes
   of the reverb work area at the top of SPU RAM.
2. `size`: if `size & ~_spu_mem_mode_unitM` is not 0 the mask is added; then
   the low `_spu_mem_mode_plus` bits are cleared. With the mask 7 a size of 8
   or more is rounded up to a multiple of 8, and a size below 8 becomes 0.
3. The search. If record 0 is the tail, the found index is 0. Otherwise
   `_spu_gcSPU()` (`0x8009ED18`), then the first index below
   `_spu_AllocBlockNum` whose record is the tail, or is free with `size` bytes
   or more. None: -1.
4. The found record is the tail: if its index is below `_spu_AllocBlockNum`
   and its size less `reserved` is at least `size` (unsigned), the next record
   becomes the tail (`addr = ((found.addr & 0x0FFFFFFF) + size) | 0x40000000`,
   `size = found.size - size`), the found record becomes an allocated block
   (flags cleared, `size`), `_spu_AllocLastNum` = the next index,
   `_spu_gcSPU()` again, and the found record's address is returned. Otherwise
   -1.
5. The found record is a free block: if it is larger than `size` and
   `_spu_AllocLastNum < _spu_AllocBlockNum`, the remainder becomes a free
   record at index `_spu_AllocLastNum` (`addr = (found.addr + size) |
   0x80000000`), the record that was at that index (the tail) moves one index
   up, and `_spu_AllocLastNum` is incremented. In either case the found
   record's size becomes `size` and its flags are cleared; then `_spu_gcSPU()`
   and the found record's address is returned. Without a spare record the
   remainder is recorded nowhere.

`reserved` is applied in the tail arm only. Caller: `func_8009D71C`, the
allocator libsnd's `SsVabOpenHead` passes down. In step 4 the code reads the
found record's address a second time with no store between; the C spells
that read with a `volatile` cast, and its comment says the original construct
is not known.

## Keys, the SPU-RAM write and the reverb work area

`s_crwa.o`, libapi's `a10.o`, `s_sk.o` and `s_w.o` lie in a row from
0x800A0028 to 0x800A03F7.

State these three functions use, beside `spu.o`'s and `s_ini.o`'s above:

| Tree symbol | Member's symbol | Meaning |
|---|---|---|
| `D_800B7C20` | `_spu_zerobuf` (`zerobuf.o`, `.data`) | 0x400 bytes, all zero in the image (0x800B7C20 to 0x800B801F): the source of `SpuClearReverbWorkArea`'s blocks. The C declares it `s32[]`; nothing in the code shows the element type. |
| `D_800B8140` | `_spu_rev_startaddr[10]` | the work area's start per reverb mode, in SPU units. Image: 0xFFFE, 0xFB28, 0xFC18, 0xF6F8, 0xF204, 0xEA44, 0xE128, 0xCFF8, 0xCFF8, 0xF880. |

`SpuSetKey(on_off, voice_bit)` (`0x800A01D8`) keys the voices of a 24-bit
mask on (`on_off` 1, `SPU_ON`) or off (`on_off` 0, `SPU_OFF`). Any other
`on_off` does nothing. `lo = voice_bit & 0xFFFFFF`, `hi = lo >> 16`; a store
of `lo` into a 16-bit word keeps its low half.

* With queueing on (`_spu_env & 1`) nothing reaches the hardware:
  * key on: `_spu_RQ[0] = lo`, `_spu_RQ[1] = hi`, `_spu_RQmask |= 1`,
    `_spu_RQvoice |= lo`; then `if (_spu_RQ[2] & lo) _spu_RQ[2] &= ~lo;` and
    `if (_spu_RQ[3] & hi) _spu_RQ[3] &= ~hi;`.
  * key off: `_spu_RQ[2] = lo`, `_spu_RQ[3] = hi`, `_spu_RQmask |= 1`,
    `_spu_RQvoice &= ~lo`; then the same two clears on `_spu_RQ[0]` and
    `_spu_RQ[1]`.
* With queueing off:
  * key on: the key-on words = `lo` / `hi`, `_spu_keystat |= lo`.
  * key off: the key-off words = `lo` / `hi`, `_spu_keystat &= ~lo`.

No callees, no return value. Caller: `_SsVmFlush` (`0x8009A258`), key off
first, then key on. In this image only the direct arms run (`_spu_env`,
above).

`SpuWrite(addr, size)` (`0x800A0398`): `if (size > 0x7EFF0) size = 0x7EFF0;`
(SPU RAM's 0x80000 bytes less the first 0x1010), `_spu_Fw(addr, size)`, `if
(_spu_transferCallback == 0) _spu_inTransfer = 0;`, and returns the size
taken. The destination is the address `SpuSetTransferStartAddr` left in
`_spu_tsa`, which `_spu_Fw` reads. Caller: `SsVabTransBody` (`0x8009DBE8`),
after `SpuSetTransferMode(0)` and `SpuSetTransferStartAddr`.

`SpuRead` (`s_r.o`) is the same 23 words with `_spu_Fr` as the callee, so the
bytes alone fit both members. The register places `s_w.o` here and refutes
`s_r.o`: the one function of this shape calls `_spu_Fw` (`spu.o` `+0x824`),
and nothing in the image calls `_spu_Fr`. The C file is named for `s_r.o`
(`libspu_s_r.c`), and its header says so too; what it holds is `s_w.o`'s
`SpuWrite`.

`SpuClearReverbWorkArea(mode)` (`0x800A0028`) fills the reverb work area of
mode `mode` with zeroes by DMA and waits for it.

1. It returns -1 if `mode` is not 0 to 9 (one unsigned test) or
   `_SpuIsInAllocateArea_(_spu_rev_startaddr[mode])` (`0x8009F588`) is not 0.
2. Mode 0: length `0x10 << _spu_mem_mode_plus`, address `0xFFF0 <<
   _spu_mem_mode_plus`. Any other mode: length `(0x10000 -
   _spu_rev_startaddr[mode]) << _spu_mem_mode_plus`, address
   `_spu_rev_startaddr[mode] << _spu_mem_mode_plus`, from the mode's start to
   the end of SPU RAM.
3. It keeps `_spu_transMode` and, if it is 1, sets it to 0. If
   `_spu_transferCallback` is set, it keeps it in a local and clears the
   global.
4. One block per pass: `size` is 0x400 while more than 0x400 bytes remain,
   otherwise the rest, and that pass is the last. `_spu_t(2, address)`,
   `_spu_t(1)`, `_spu_t(3, _spu_zerobuf, size)`, `WaitEvent(_spu_EVdma)`;
   length -= 0x400, address += 0x400.
5. It restores `_spu_transMode` if step 3 changed it, and
   `_spu_transferCallback` if step 3 took it. Returns 0.

With the callback out of the way, `_spu_FiDMA` reports each block's end by
the DMA event, which is what `WaitEvent` waits on. Callers: `SsInit`
(`0x80095A78`), with mode 7; `SpuSetReverbModeParam` (`0x8009F618`) and
`SpuSetReverbModeType` (`0x800A0E68`) when the mode carries
`SPU_REV_MODE_CLEAR_WA` (0x100).

## libsnd: `SsVabFakeHead`

`SsVabFakeHead(addr, vabid, sbaddr)` (`0x8009D7BC`, `vs_vh.o` `+0x34`, file
`libsnd_vs_vh.c`) opens the VAB header at `addr` in slot `vabid` (-1 = the
first free of 16) for a sample body that is already in SPU RAM at `sbaddr`:
`return _SsVabOpenHeadWithMode(addr, vabid, func_8009D7F0, sbaddr)`
(`0x8009D7F8`). The allocator `func_8009D7F0` is the member's static that
returns the SPU address it is given, so nothing is reserved. No caller in
this program.

It is the same thirteen words as `SsVabOpenHeadSticky` (`0x8009D788`), so the
bytes alone name neither. The member decides: `vs_vh.o` lies whole at
0x8009D788 (0x460 bytes), with `SsVabOpenHeadSticky` at `+0`, `SsVabFakeHead`
at `+0x34`, the static `func_8009D7F0` at `+0x68` and `_SsVabOpenHeadWithMode`
at `+0x70`. That the two functions differ only in what the caller does next
(transfer the body, or `SsVabFakeBody`) is Sony's reference's statement, not
something this code shows.

## Function index

| Address | Name | Member | File |
|---|---|---|---|
| `0x8009D7BC` | `SsVabFakeHead` | libsnd `vs_vh.o` `+0x34` | `libsnd_vs_vh.c` |
| `0x8009DCD8` | `SpuInit` | `s_i.o` | `libspu_s_i.c` |
| `0x8009DCF8` | `_SpuInit` | `s_ini.o` | `libspu_s_ini_1.c` |
| `0x8009DDE0` | `SpuStart` | `s_ini.o` `+0xE8` | `libspu_s_ini_2.c` |
| `0x8009DE78` | `_spu_init` | `spu.o` | `libspu_spu_1.c` |
| `0x8009E0F8` | `func_8009E0F8` | `spu.o` `+0x280`, static | `libspu_spu_1.c` |
| `0x8009E2B8` | `_spu_FiDMA` | `spu.o` `+0x440` | `libspu_spu_2.c` |
| `0x8009E374` | `_spu_Fr_` | `spu.o` `+0x4FC` | `libspu_spu_3.c` |
| `0x8009E41C` | `_spu_t` | `spu.o` `+0x5A4` | `libspu_spu_3.c` |
| `0x8009E69C` | `_spu_Fw` | `spu.o` `+0x824` | `libspu_spu_3.c` |
| `0x8009E720` | `_spu_Fr` | `spu.o` `+0x8A8` | `libspu_spu_3.c` |
| `0x8009E784` | `_spu_FsetRXX` | `spu.o` `+0x90C` | `libspu_spu_3.c` |
| `0x8009E7C8` | `_spu_FsetRXXa` | `spu.o` `+0x950` | `libspu_spu_3.c` |
| `0x8009E86C` | `_spu_FgetRXXa` | `spu.o` `+0x9F4` | `libspu_spu_3.c` |
| `0x8009E8A8` | `_spu_FsetPCR` | `spu.o` `+0xA30` | `libspu_spu_3.c` |
| `0x8009E900` | `func_8009E900` | `spu.o` `+0xA88`, static | `libspu_spu_3.c` |
| `0x8009E928` | `func_8009E928` | `spu.o` `+0xAB0`, static | `libspu_spu_3.c` |
| `0x8009E950` | `_spu_Fw1ts` | `spu.o` `+0xAD8` | `libspu_spu_3.c` |
| `0x8009E9B8` | `_SpuDataCallback` | `s_dcb.o` | `libspu_s_dcb.c` |
| `0x8009E9E8` | `SpuInitMalloc` | `s_m_init.o` | `libspu_s_m_init.c` |
| `0x8009EA48` | `SpuMalloc` | `s_m_m.o` | `libspu_s_m_m.c` |
| `0x800A0028` | `SpuClearReverbWorkArea` | `s_crwa.o` | `libspu_s_crwa.c` |
| `0x800A01C8` | `WaitEvent` | libapi `a10.o` | `libapi_a10.c` |
| `0x800A01D8` | `SpuSetKey` | `s_sk.o` | `libspu_s_sk.c` |
| `0x800A0398` | `SpuWrite` | `s_w.o` | `libspu_s_r.c` |
