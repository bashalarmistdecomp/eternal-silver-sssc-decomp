# libpad

Sony's controller library as the main executable links it (PSY-Q 4.5): its
seven members, `0x800A4CF8` to `0x800A7D97`, 64 functions, all of them C.
This page says how the library serves the two controller ports at every
vertical blank, what its hooks and state hold, what the port record holds,
and what each function does. It ends with libc2's `strncpy`, which lies just
before the library. Every library function's file is in
[index.md](index.md), the libraries as a whole are in
[psyq-libs.md](../psyq-libs.md), libapi's own controller code (`PAD.OBJ`) and
libcard are in [libapi-libcard-libmcrd.md](libapi-libcard-libmcrd.md), how
Sony's objects were compiled is in [codegen.md](codegen.md), and how the
names were proven is in [naming.md](../../../../naming.md).

## Where it lies

| Member | Image range | File | Functions | Data |
|---|---|---|---|---|
| `padentry.o` | `0x800A4CF8` to `0x800A51B7` | `libpad_padentry_1.c` (`+0x000` to `+0x0AC`), `libpad_padentry_2.c` (from `+0x178`) | 11 | `PadInfoAct`'s jump table in `.rodata` at `0x80013C24` |
| `padmain.o` | `0x800A51B8` to `0x800A5F87` | `libpad_padmain.c` | 13 | `.data` `0x800B8480` to `0x800B84FF`; `.bss` from `0x800EAD40` |
| `padcmd.o` | `0x800A5F88` to `0x800A6B87` | `libpad_padcmd.c` | 20 | `.bss` `0x800EAD60`, 0x10 bytes |
| `padif.o` | `0x800A6B88` to `0x800A71B7` | `libpad_padif.c` | 5 | `.data` `0x800B8500` to `0x800B852F` |
| `padportd.o` | `0x800A71B8` to `0x800A7817` | `libpad_padportd.c` | 8 | `.data` `0x800B8530`; `.bss` `0x800EAD70` to `0x800EAFDF` |
| `padseqd.o` | `0x800A7818` to `0x800A7CD7` | `libpad_padseqd.c` | 5 | none |
| `waitrc2.o` | `0x800A7CD8` to `0x800A7D97` | `libpad_waitrc2.c` | 2 | `_startTime`, `_waitTime` (`0x800EAFE0`, `0x800EAFE4`) |

The seven members are 12,448 bytes, which is libpad's whole share of the
image's library code in naming.md's table. Each member's last function is
followed by its padding: three zero words after `padentry.o`, `padmain.o`,
`padif.o` and `padseqd.o`, one after `padcmd.o` and `padportd.o`, none after
`waitrc2.o` (`INCLUDE_PAD` in the C). `padentry.o` is cut in two files
because the tree compiles its first five functions with other flags than the
rest (`config/cc_flags.txt`; [codegen.md](codegen.md)). The library begins
right after libcard's `_card_read` (`0x800A4CE8`, member `a79.o`).

Which release: the word 0x00450000 at `0x800B8484`, in `padmain.o`'s
`.data`, is release 4.5's number, and it is what pins libpad to 4.5. The code
alone does not: release 4.4's members are the same bytes, while 4.6 and 4.7
share only 11.8% of them (naming.md, "Which release of Sony's libraries the
programs link"). Sony reorganised the library in 4.6 and renamed as it went,
so the names here are 4.5's: 4.5's `_padChkVsync` is 4.7's `PadChkVsync`,
and 4.5's `setRC2wait` is 4.7's `_padSetRC2wait`. Two of 4.5's private functions take a later
release's name (`sdk-later`): `_padIsVsync` (`0x800A52FC`) and `_padIntPad`
(`0x800A5364`). The other 27 functions without a symbol in their member
have no name anywhere: no Sony text names them, and no release from 2.6 to
4.7 exports their bytes under any name (the register's readings). They keep
their addresses, and this page describes each by its role.

The library's multi-tap and light-gun members are not linked. The game
starts its controllers with `PadInitDirect` only (`pad_start` (`0x8002003C`)),
and the seven members above account for all of libpad's bytes in the image.

Data symbols in the tree are addresses (`D_800B8488`). The tables below pair
each with the symbol its member gives it, and the prose uses the member's
symbol where the C's comments do.

## How the library serves the ports

**Start-up.** `pad_start` (`0x8002003C`, `pad_start.c`) clears the game's two
0x40-byte buffers, then calls `PadInitDirect(g_PadPort00Buffer,
g_PadPort10Buffer)`, `PadSetAct(0, g_PadActuatorLevels, 2)` and
`PadStartCom()`. `PadInitDirect` has `_padInitDirSeq` install the three
sequencer hooks, installs its own seven hooks, sets up the two port records
and calls `_padSetVsyncParam`, which fills `padmain.o`'s entry for the
kernel's interrupt queue (`D_800EAD40`, a `PadIntRP`): the verifier
`_padIsVsync` and the handler `_padIntPad`. `PadStartCom` →
`_padStartCom` puts the entry in the kernel's interrupt queue 2, acknowledges
and enables the vertical-blank interrupt, and clears both records through
`_padFuncClrInfo`.

**Each vertical blank.** The kernel calls `_padIsVsync`, which answers 1 when
the vertical-blank interrupt is enabled and pending; then it calls
`_padIntPad`. If SIO0's control register still has bit 1 set (the last
transfer never ended) the handler writes 0 there and stops. Otherwise it sets
the flag `PadChkVsync` reports, counts the frame for each port `PadEnableCom`
has switched off, and, while `_padIntExec` is set, serves the ports from
`_padChanStart` to `_padChanStop`: `_padInitSioMode` on the first port's
record, `_padTotalCurr` = 0, then `func_800A594C` again and again until
`_padSioChan` has passed `_padChanStop`. The whole poll of both ports runs
inside this one interrupt, waiting on the hardware between bytes; every wait
has a time-out on root counter 2 (`setRC2wait` / `chkRC2wait`).

**One transfer.** `func_800A594C` runs one step, `padIntFunc[_padSioState]`
(`D_800B8510`, `padif.o`'s five functions), and moves `_padSioState` on; the
increment happens before the call, so a step runs with `_padSioState` one
above its own index. A negative result ends the transfer through
`_padFuncNextPort`.

Each exchange reads the port before it writes, so the byte a step reads is
the answer to the byte the step before sent.

| Step | Function | Byte read first | Byte sent |
|---|---|---|---|
| 0 | `func_800A6B88` | whatever the data register holds, not kept | 1, through `_padSioRW(p, -2)`; before it `_padFuncSendAuto` queues the command of the record's sequence step |
| 1 | `func_800A6BD0` | the answer to 1, not stored (`recv_len` is 0xFF and wraps to 0) | the queued command (`cmd_queued`), or 0x42 |
| 2 | `func_800A6CA8` | the controller's ID, `recv_buf[0]`: its low nibble × 2 is the count of bytes still to move (0x20 for 0) | 0 with a command queued, else `_padModeMtap` |
| 3 | `func_800A6D54` | `recv_buf[1]`, which must be 0x5A or 0, else the step fails with -4 | `_padFuncGetTxd`'s byte |
| 4 | `func_800A6E18` | the rest, each stored in `recv_buf` | `_padFuncGetTxd`'s byte, for every remaining byte |

Step 4 does not return between bytes: it polls the port itself and ends with
`_padFuncNextPort(0)`. That hook (`func_800A7380`) closes the transfer and
starts the next port's (`_padInitSioMode`); after the last port the
handler's loop ends. The protocol reading of the bytes (1 the controller's
address, 0x42 "read the controller", 0x5A the answer after the ID) is the
console's controller protocol as generally documented, not something the
library's code or Sony's text shows.

**Judging the reply.** A transfer that ends well leaves its port's
`_padFixResult` word at 0 (`func_800A7380`). The reply is judged later, by
`_padFuncRecvAuto` and then `_padFuncClrCmdNo` on the record, in whichever of
two places comes first: in step 4 of the other port's transfer, one item per
payload byte while more than one byte is left, or at the start of the port's
own next transfer (`_padInitSioMode`). Then the word is -1. A failed transfer
goes to `_dirFailAuto` and `_padFuncClrCmdNo` at once and leaves the word as
it was. In this game the word is only ever 0 or -1: the positive values the
two places also handle (multi-tap records still to judge) are stored by no
linked function.

**The sequence.** Each record has a sequence step (`seq`, `+0x46`). The send
step `func_800A784C` (`_padFuncSendAuto`, called in step 0) queues the
command for the current step; the receive step `func_800A795C`
(`_padFuncRecvAuto`) copies the reply to the game's buffer and moves `seq`
on. A newly found controller goes through: step 0 (state 1,
`PadStateFindPad`); step 1, command 0x43 with 1; steps 2 to 4, the counts
(`_padSendAtLoadInfo` / `_padRecvAtLoadInfo`); then the tables
(`func_800A63A4` / `func_800A644C`, state 4 while they run), after which
the state is 6 (`PadStateStable`); step 0xFE, command 0x43 with 0; step
0xFF, done. `PadSetActAlign` and `PadSetMainMode` start their own short
sequences the same way. Twelve failures in a row give the controller up (`_dirFailAuto`).

## What this game runs

Only the direct ports' code runs. The light-gun paths never do:
`_padGunExec` (`D_800B84C4`) is 0 in `.data` and no function stores to it,
and no function stores the three gun hooks. The multi-tap paths never do
either: the only store to `_padModeMtap` (`D_800B84D4`) is `PadInitDirect`'s
0, the only store to a record's `tap_info` is `PadInitDirect`'s 0, and the
`_padFuncSendAuto` the game installs (`func_800A784C`) always returns 0. So
`padif.o`'s statics `D_800B8508` and `D_800B850C` stay 0, steps 1 and 2 never
call the hook for `tap_info`, step 2's third byte is 0, step 4 starts at its
second part, and `PadChkMtap` always returns 0. These are read from the C;
they were not traced in an emulator.

The game's side is `pad_start` and `pad_update` (`0x8001F508`,
`pad_update.c`). `pad_update` serves port 0x10, then port 0x00: it reads
`PadGetState` and `PadInfoMode(port, 1, 0)` (the current ID), and for a
controller of ID 7 it calls `PadSetAct(port, g_PadActuatorLevels, 2)` and,
in state 6, `PadSetActAlign(port, g_PadActuatorAlignBytes)`. No other entry
point is called.

## The hooks and the library's state

`padmain.o`'s `.data` (`0x800B8480`). The words at `+0x00` and `+0x04` are
0x000C7350 and the release word 0x00450000; no code reads them. All of it
lies inside the `$gp` window, but the library reaches every word by full
address (it was built without small data; [codegen.md](codegen.md), "No small
data").

| Offset | Tree symbol | Member's symbol | Initial | What it holds |
|---|---|---|---|---|
| `+0x08` | `D_800B8488` | `_padFuncNextPort` | 0 | `void (*)(s32)`: `func_800A7380`. Ends a transfer with its result and starts the next port's. |
| `+0x0C` | `D_800B848C` | `_padFuncClrInfo` | 0 | `void (*)(PadInfo *)`: `func_800A7318`. Forgets the controller a record describes. |
| `+0x10` | `D_800B8490` | `_padFuncGetTxd` | 0 | `u8 (*)(PadInfo *, s32)`: `func_800A7484`. The byte to send at the transfer's position. |
| `+0x14` | `D_800B8494` | `_padFuncCurrLimit` | 0 | `void (*)(PadInfo *)`: `func_800A7540`. Which actuator bytes may go out this round. |
| `+0x18` | `D_800B8498` | `_padFuncPtr2Port` | 0 | `s32 (*)(PadInfo *)`: `func_800A77BC`. Nothing calls it. |
| `+0x1C` | `D_800B849C` | `_padFuncPort2Info` | 0 | `PadInfo *(*)(s32)`: `func_800A77F4`. The record of a port number; every entry point of `padentry.o` but the first four reaches its record through it. |
| `+0x20` | `D_800B84A0` | `_padFuncSendAuto` | 0 | `s32 (*)(PadInfo *)`: `func_800A784C`, the send step. |
| `+0x24` | `D_800B84A4` | `_padFuncChkEng` | 0 | `s32 (*)(PadInfo *)`: `func_800A7CA0`. Non-zero makes `_padLoadActInfo`, `_padSetActAlign` and `_padSetMainMode` refuse. |
| `+0x28` | `D_800B84A8` | `_padFuncRecvAuto` | 0 | `void (*)(PadInfo *)`: `func_800A795C`, the receive step. |
| `+0x2C` | `D_800B84AC` | `_padFuncClrCmdNo` | 0 | `void (*)(PadInfo *)`: `func_800A7474`. Moves the queued command to `cmd_taken`. |
| `+0x30` | `D_800B84B0` | `_padFuncIntGun` | 0 | `void (*)(void)`: called by `_padIsVsync` when set; never set. |
| `+0x34` | `D_800B84B4` | `_padFuncSetGunPort` | 0 | `void (*)(void)`: called by step 1 with `_padGunExec` set; never set. |
| `+0x38` | `D_800B84B8` | `_padFuncGetGunPos` | 0 | `void (*)(void)`: as `_padFuncSetGunPort`. |
| `+0x3C` | `D_800B84BC` | `_padInfoDir` | 0 | `PadInfo *`: `PadInitDirect` points it at the two records `D_800EAE00`. |
| `+0x40` | `D_800B84C0` | `_padIntExec` | 0 | The handler serves the ports only while it is set. `PadInitDirect`, `_padStartCom` and `PadEnableCom` hold it at 0 while they work, then set 1. |
| `+0x44` | `D_800B84C4` | `_padGunExec` | 0 | Read by step 1; never written. |
| `+0x48` | `D_800B84C8` | `_padSioChan` | 0 | The port being served; `_padFuncNextPort` steps it. |
| `+0x4C` | `D_800B84CC` | `_padSioState` | 0 | The transfer's step: the index into `padIntFunc`. |
| `+0x50` | `D_800B84D0` | `_padTotalCurr` | 0 | The actuator current granted so far in the round; the handler zeroes it, `func_800A7540` adds to it up to 60. |
| `+0x54` | `D_800B84D4` | `_padModeMtap` | 0 | Multi-tap mode; only `PadInitDirect` stores to it, a 0. |
| `+0x58` | `D_800B84D8` | `_padChanStart` | 0 | The first port served: 0, or 1 while port 0 is switched off. |
| `+0x5C` | `D_800B84DC` | `_padChanStop` | 1 | The last port served: 1, or 0 while port 1 is switched off. |
| `+0x60` | `D_800B84E0` | `_padFixResult` | -1, -1 | One word per port: 0 = a reply waits to be judged, -1 = none (above). |
| `+0x68` | `D_800B84E8` | static | 0x1F801070 | `volatile PadIntr *`: the interrupt controller's I_STAT and I_MASK. |
| `+0x6C` | `D_800B84EC` | static | 0x1F801040 | `volatile PadSio *`: the SIO0 registers. |
| `+0x70` | `D_800B84F0` | static | 0 | 1 once the handler has passed its busy test at a vertical blank; `_padChkVsync` returns and clears it. |

The other members' data:

| Address | Tree symbol | Member and place | What it holds |
|---|---|---|---|
| `0x800EAD40` | `D_800EAD40` | `padmain.o` `.bss+0x00`, static | The interrupt-queue entry, a `PadIntRP` (16 bytes): `next`, the handler `_padIntPad`, the verifier `_padIsVsync`, a zero word. |
| `0x800EAD50` | `D_800EAD50` | `padmain.o` `.bss+0x10`, static | `s32[2]`: vertical blanks each port has been switched off, counted to 150 by the handler. |
| `0x800EAD60` | `D_800EAD60` | `padcmd.o` `.bss+0x00`, static | `u8 *`: the write position in the combination list `func_800A644C` is filling; the only word of the member's 0x10 bytes that code names. |
| `0x800B8500` | `D_800B8500` | `padif.o` `.data+0x0`, static | Bytes still to move in the transfer: step 2 sets it, step 4 counts it down. |
| `0x800B8504` | `D_800B8504` | `padif.o` `.data+0x4`, static | `volatile PadSio *`, 0x1F801040: step 4 reads `data` and `baud`. |
| `0x800B8508` | `D_800B8508` | `padif.o` `.data+0x8`, static | Step 0's `_padFuncSendAuto` result: non-zero makes steps 1 and 2 call that hook for `tap_info[0]` to `[3]`. |
| `0x800B850C` | `D_800B850C` | `padif.o` `.data+0xC`, static | Step 3's verdict: a multi tap answers and no command is queued. |
| `0x800B8510` | `D_800B8510` | `padif.o` `.data+0x10`, `padIntFunc` | The five step functions, then three zero words; only `func_800A594C` reads it. |
| `0x800B8530` | `D_800B8530` | `padportd.o` `.data+0x0`, static | `volatile PadSio *`, 0x1F801040: `func_800A7380` clears `ctrl`. |
| `0x800EAD70` | `D_800EAD70` | `padportd.o` `.bss+0x00`, static | `u8[2][0x23]`: the two ports' raw receive buffers (`recv_buf`). |
| `0x800EADB8` | `D_800EADB8` | `padportd.o` `.bss+0x48`, static | `u8[2][0x23]`: the two ports' send buffers (`send_buf`). |
| `0x800EAE00` | `D_800EAE00` | `padportd.o` `.bss+0x90`, static | `PadInfo[2]`: the two port records, 0x1E0 bytes. |
| `0x800EAFE0` | `D_800EAFE0` | `waitrc2.o`, `_startTime` | Root counter 2's count when the wait began. |
| `0x800EAFE4` | `D_800EAFE4` | `waitrc2.o`, `_waitTime` | The wait's length, in ticks of the system clock / 8. |

## The port record

`PadInfo` (`include/psyq_private.h`, 0xF0 bytes) is one controller port's
record. No Sony header declares it; the member names are the tree's, read
from use. Its size is `PadInitDirect`'s (it clears two records, 0x1E0 bytes)
and `PadChkMtap`'s index scale.

| Offset | Member | What it holds |
|---|---|---|
| `+0x00` | `id_table` (`u16 *`) | The controller's extended IDs, in `info_buf`: `_padLoadActInfo` points it, `func_800A644C` fills it, `PadInfoMode` term 4 reads it. |
| `+0x04` | `act_table` (`PadActInfo *`) | Its actuator records, behind the IDs. |
| `+0x08` | `comb_table` (`PadCombInfo *`) | Its combination records, behind the actuators; the lists follow the last record. |
| `+0x0C` | `tap_info` (`PadInfo *`) | A multi tap's four further records; the direct ports store 0. |
| `+0x10` | `head` (`PadInfo *`) | The port's own first record; `PadInitDirect` stores the record itself. |
| `+0x14` | `send_func` | The send hook of the sequence in force; 0 = `_padSendAtLoadInfo`. |
| `+0x18` | `recv_func` | Its receive hook, whose result is added to `seq` (1 = next step, 0 = the same again); 0 = `_padRecvAtLoadInfo`. |
| `+0x20` | `align_data` (`u8 *`) | `PadSetActAlign`'s six bytes. |
| `+0x24` | `cmd_param` (`u8`) | The parameter byte of a one-byte command; `cmd_data` points at it. |
| `+0x28` | `act_data` (`u8 *`) | `PadSetAct`'s data. |
| `+0x2C` | `cmd_data` (`u8 *`) | The queued command's parameter bytes. |
| `+0x30` | `rx_buf` (`u8 *`) | The game's buffer. Byte 0 is 0 when a reply is there and 0xFF when none is, byte 1 the reply's first byte (the ID), then the reply's data. |
| `+0x34` | `act_len` (`u8`) | `PadSetAct`'s length. |
| `+0x35` | `rx_len` (`u8`) | Bytes copied into `rx_buf`; 0 when no controller answers. |
| `+0x36` | `cmd_len` (`u8`) | `cmd_data`'s count. |
| `+0x37` | `cmd_queued` (`u8`) | The command to send in the next transfer, 0 = none. |
| `+0x38` | `cmd_taken` (`u8`) | The command of the last transfer finished with; `func_800A7474` moves `cmd_queued` here. |
| `+0x39` | `unk_39` (`u8`) | Read by `PadGetState` on a record that is not its `head`; no linked function writes it. |
| `+0x3C` | `recv_buf` (`u8 *`) | The bytes received in the transfer; `[0]` is the ID. |
| `+0x40` | `send_buf` (`u8 *`) | The bytes sent; `_padSioRW` writes the address byte at its head. |
| `+0x44` | `recv_len` (`u8`) | Bytes received; 0xFF at the start of a transfer means "do not store the next". |
| `+0x45` | `send_len` (`u8`) | Bytes sent, the three header bytes included. |
| `+0x46` | `seq` (`u8`) | The sequence step: 0 new, 1 enter, 2 to 0xFD a command sequence, 0xFE leave, 0xFF done. |
| `+0x47` | `index` (`u8`) | The ID, actuator or combination a step is on. |
| `+0x48` | `count` (`u8`) | Bytes of the current combination's list still to come. |
| `+0x49` | `state` (`u8`) | `PadState*`, below. |
| `+0x4A` | `retry` (`u8`) | Failures in a row; a reply zeroes it. |
| `+0x4C` | `fail_count` (`s32`) | Every failed transfer; `PadInfoMode` term 100 returns it. |
| `+0x50` | `unk_50` (`u8`) | Tested by `_padInitSioMode` and `func_800A795C`; no linked function writes it. |
| `+0x51` | `mode_offs` (`u8`) | The ID-table place `PadSetMainMode` asked for. |
| `+0x52` | `mode_lock` (`u8`) | Its lock argument. |
| `+0x53` | `mode_same` (`u8`) | 1 when that place is the current one (`cur_ex_offs`). |
| `+0x57` | `act_enable[6]` | Per transmit byte: 1 = the actuator byte may be sent this round. |
| `+0x5D` | `act_align[6]` | Per transmit byte: the actuator it drives, 0xFF = none. |
| `+0x63` | `info_buf[0x80]` | Where the three tables and the lists are built. |
| `+0xE3` | `id_num` (`u8`) | Entries in `id_table`. |
| `+0xE4` | `cur_ex_offs` (`u8`) | The current ID's place in `id_table`. |
| `+0xE6` | `cur_ex_id` (`u16`) | The current extended ID. |
| `+0xE8` | `cur_id` (`u8`) | The current ID, the reply's first byte >> 4. |
| `+0xE9` | `act_num` (`u8`) | Actuators. |
| `+0xEA` | `comb_num` (`u8`) | Combinations. |
| `+0xEB` | `unk_eb` (`u8`) | `padcmd.o` clears it each time an item is taken; nothing reads it. |
| `+0xEC` | `comb_size` (`u16`) | Bytes the combination records and lists need. |
| `+0xEE` | `prev_reply` (`u16`) | What the reply before gave for the item being read (the value, or 0 / 0xFFFF for "same" / "differs"). |

The other bytes (`+0x1C`, `+0x25`, `+0x3A`, `+0x4B`, `+0x54`, `+0xE5`) are
padding that nothing reads or writes.

`state` takes libpad.h's values: 0 `PadStateDiscon`, 1 `PadStateFindPad`, 2
`PadStateFindCTP1`, 3 `PadStateFindCTP2`, 4 `PadStateReqInfo`, 5
`PadStateExecCmd`, 6 `PadStateStable`. The linked code stores 0
(`func_800A7318`), 1 (`func_800A795C`), 2 (`_dirFailAuto`,
`_padRecvAtLoadInfo`), 4 (`_padLoadActInfo`) and 6 (`func_800A644C`); no
linked function stores 3 or 5.

`PadActInfo` (5 bytes) is one actuator: `func`, `sub`, `size`, `curr` (its
current drain) and `sign`, `PadInfoAct` terms 1 to 5. `PadCombInfo` (8
bytes) is one combination: `num`, the actuators in it, and at `+4` `list`,
their numbers. The hardware structs beside them in the same header are
`PadSio` (SIO0 at 0x1F801040: `data`, `stat`, `mode`, `ctrl`, `baud`),
`PadIntr` (I_STAT and I_MASK at 0x1F801070) and `PadIntRP` (an entry of the
kernel's interrupt queues, shared with libapi's `PAD.OBJ`).

## padentry.o: the entry points

| Offset | Function | Address |
|---|---|---|
| `+0x000` | `PadChkVsync` | `0x800A4CF8` |
| `+0x020` | `PadStartCom` | `0x800A4D18` |
| `+0x040` | `PadStopCom` | `0x800A4D38` |
| `+0x060` | `PadChkMtap` | `0x800A4D58` |
| `+0x0AC` | `PadGetState` | `0x800A4DA4` |
| `+0x178` | `PadInfoMode` | `0x800A4E70` |
| `+0x270` | `PadInfoAct` | `0x800A4F68` |
| `+0x344` | `PadInfoComb` | `0x800A503C` |
| `+0x3EC` | `PadSetActAlign` | `0x800A50E4` |
| `+0x424` | `PadSetMainMode` | `0x800A511C` |
| `+0x46C` | `PadSetAct` | `0x800A5164` |

Each from `PadGetState` on fetches the port's record through
`_padFuncPort2Info` and reads it, or hands it to the worker of the same name
in `padcmd.o`.

- `PadChkVsync()` returns `_padChkVsync()`. Nothing in the image calls it.
- `PadStartCom()` and `PadStopCom()` call `_padStartCom` and `_padStopCom`.
  `pad_start` calls `PadStartCom`; nothing calls `PadStopCom`.
- `PadChkMtap(port)`: 0 unless `_padModeMtap` is set; then whether record
  `port >> 4` of `_padInfoDir` has current ID 8. Nothing calls it.
- `PadGetState(port)` returns the record's `state`, except while the port is
  busy: a command queued or taken, `unk_39` set on a record that is not its
  `head`, or a non-zero `rx_buf[0]`. Then `PadStateFindCTP1` and
  `PadStateFindCTP2` are reported as `PadStateFindPad`, and `PadStateStable`
  as `PadStateReqInfo`. Caller: `pad_update`.
- `PadInfoMode(port, term, offs)`: term 1 `cur_id`, 2 `cur_ex_id`, 3
  `cur_ex_offs`; 4 the ID table, where `offs < 0` gives `id_num` and an `offs`
  below it the entry; 100 `fail_count`. Anything else, or an `offs` past the
  end, gives 0. Caller: `pad_update`, term 1.
- `PadInfoAct(port, actno, term)`: `actno < 0` gives `act_num`; for an
  actuator the controller has, terms 1 to 5 give `func`, `sub`, `size`,
  `curr`, `sign`. Anything else gives 0. Nothing calls it.
- `PadInfoComb(port, listno, offs)`: `listno < 0` gives `comb_num`; for a list
  the controller has, `offs < 0` gives its count and an `offs` below that the
  actuator number at that place. Anything else gives 0. Nothing calls it.
- `PadSetActAlign(port, data)` returns `_padSetActAlign(record, data)`: 1 when
  the sequence was started, 0 when `_padFuncChkEng` refused. `data` is six
  actuator numbers, one per transmit byte. Caller: `pad_update`.
- `PadSetMainMode(port, offs, lock)` returns `_padSetMainMode(record, offs,
  lock)`, both cut to a byte: the mode at place `offs` of the ID table, and
  the lock of the mode button (the C's comment: bit 1 = change the lock, bit
  0 = locked). Nothing calls it.
- `PadSetAct(port, data, len)` calls `_padSetAct(record, data, len)`, which
  keeps the pointer and the length; nothing is copied. Returns nothing.
  Callers: `pad_start`, `pad_update`.

## padmain.o: start, stop, the interrupt and the byte exchange

| Offset | Function | Address |
|---|---|---|
| `+0x000` | `PadEnableCom` | `0x800A51B8` |
| `+0x118` | `_padSetVsyncParam` | `0x800A52D0` |
| `+0x144` | `_padIsVsync` | `0x800A52FC` |
| `+0x1AC` | `_padIntPad` | `0x800A5364` |
| `+0x33C` | `_padChkVsync` | `0x800A54F4` |
| `+0x350` | `_padStartCom` | `0x800A5508` |
| `+0x41C` | `_padStopCom` | `0x800A55D4` |
| `+0x460` | `_padInitSioMode` | `0x800A5618` |
| `+0x794` | `func_800A594C` | `0x800A594C` |
| `+0x884` | `_padSioRW` | `0x800A5A3C` |
| `+0xA94` | `_padSioRW2` | `0x800A5C4C` |
| `+0xD0C` | `_padClrIntSio0` | `0x800A5EC4` |
| `+0xD9C` | `_padWaitRXready` | `0x800A5F54` |

- `PadEnableCom(mode)`: bit 0 switches port 0, bit 1 port 1, 1 = on.
  Returns the setting it found, `(_padChanStop << 1) | (_padChanStart == 0)`,
  and changes nothing when `mode` is that. Otherwise, with `_padIntExec` at 0
  meanwhile, a port switched on moves its bound back, has its record cleared
  through `_padFuncClrInfo` if it had been off for 150 vertical blanks or
  more (`D_800EAD50[port]`), and has its count zeroed; a port switched off
  only moves its bound. Nothing calls it.
- `_padSetVsyncParam()` fills `D_800EAD40`: handler `_padIntPad`, verifier
  `_padIsVsync`, the other two words 0. Caller: `PadInitDirect`.
- `_padIsVsync()` (no symbol in 4.5's member): 0 unless bit 0 (vertical
  blank) is set in both I_MASK and I_STAT; then it calls `_padFuncIntGun` if
  that is set and returns 1.
- `_padIntPad()` (no symbol in 4.5's member): the handler, as above. After
  serving the ports it puts SIO0's baud register back to 0x88. A 0 from `_padInitSioMode` on the
  first port goes to `_padFuncNextPort(0xFFFF)`. Returns 0.
- `_padChkVsync()` returns `D_800B84F0` and clears it. Caller: `PadChkVsync`.
- `_padStartCom()`: `_padIntExec` = 0; in a critical section,
  `SysDeqIntRP(2, &D_800EAD40)`, `SysEnqIntRP(2, &D_800EAD40)`, I_STAT = ~1,
  I_MASK |= 1, `ChangeClearRCnt(3, 0)`; then `_padFuncClrInfo` on both
  records, both counts of `D_800EAD50` zeroed, `_padIntExec` = 1. Caller:
  `PadStartCom`.
- `_padStopCom()`: in a critical section, `ChangeClearRCnt(3, 1)` and
  `SysDeqIntRP(2, &D_800EAD40)`. It leaves I_MASK and `_padIntExec` as they
  are. Caller: `PadStopCom`. What `ChangeClearRCnt`'s second argument does
  is inferred from the call's name and libetc's use of it: with 0 the kernel
  stops acknowledging the vertical-blank interrupt by itself.
- `_padInitSioMode(p)` prepares SIO0 for the port `_padSioChan` names: control
  0x40 then 0, mode 0xD, baud 0x88; `setRC2wait(80)` when the record's
  `cur_id` is 8, else `setRC2wait(145)`; control 0x1003 for port 0, 0x3003 for
  port 1. Then it pays out the port's `_padFixResult` word (above). If SIO0's
  status has 0x200 it sets control bit 4; if the request is then gone it
  acknowledges I_STAT bit 7; if it stays, it waits out the time-out, clocks
  out the bytes 1, 0x42, 1 (each: write, `setRC2wait`, `_padClrIntSio0`,
  `_padWaitRXready`, read and drop the answer; before the second and third it
  waits for I_STAT bit 7 under the time-out) and returns 0, as it also does
  when `_padClrIntSio0` returns 0. Otherwise it returns 0 when `unk_50` and
  `cmd_queued` are both set, and 1 if not. Callers: `_padIntPad`,
  `func_800A7380`.
- `func_800A594C(p)` (static) runs one step of the transfer, as above.
  Unless the state is 0 after the step, or is 3 with 0x80 as `recv_buf[0]`
  (the case `_padSioRW2` acknowledges by itself), it calls `setRC2wait(0x3C)`
  and `_padClrIntSio0`, whose 0 ends the transfer with
  `_padFuncNextPort(-3)`. A state of 5 or more is put back by one, so step 4
  is the last. Caller: `_padIntPad`.
- `_padSioRW(p, data)` exchanges one byte and returns the byte read. With
  `data < 0` it starts a transfer: reads the data register (the result), sets
  `recv_len` 0xFF and `send_len` 1, stores `~data` at `send_buf[0]`, waits for
  status bit 0 and for the caller's time-out, and writes `~data`. Otherwise it
  picks the baud value (0x88, or 0x22 when `recv_buf[0]` is 0x8n and
  `recv_len` is 9 or more), starts a wait of 0x1AE with `setRC2wait`'s two
  statements in line, waits for status bit 1, reads the byte, writes the baud
  register, waits for I_STAT bit 7 (-0x14 when the wait runs out first),
  writes `data`, acknowledges at once at the fast rate, and stores the byte
  at `recv_buf[recv_len++]`. This form does not test `recv_len` for 0xFF;
  by the order of the steps it is first reached in step 4, after the
  `_padSioRW2` calls of steps 1 to 3 have brought `recv_len` from 0xFF to 2.
  Callers: `padif.o`.
- `_padSioRW2(p, data)` exchanges one byte for the first steps and returns
  the byte read, or -2 on a time-out. It picks the baud value as above, waits
  for status bit 1, `setRC2wait(0x190)`, reads the byte; when `recv_len` is
  0 and the byte is 0x8n it sets baud 0x22 at once. It waits for I_STAT bit 7
  with `chkRC2wait`'s test in line; for a controller whose `cur_id` is not 8,
  with `_padSioState` at 2, it waits a further 0x3C. It writes `data`; with
  `_padSioState` at 3 and 0x80 just read it acknowledges at once. It stores
  the byte unless `recv_len` is 0xFF, and counts it either way. Callers:
  `padif.o`.
- `_padClrIntSio0()` writes ~0x80 to I_STAT, waits for SIO0 status bit 7 to
  drop and returns 0 when the time-out comes first; else sets control bit 4
  and returns 1. Callers: `_padInitSioMode`, `func_800A594C`, `padif.o`.
- `_padWaitRXready()` waits, with no time limit, for SIO0 status bit 1.
  Callers: `_padInitSioMode`, step 4.

The register values are the code's. Their meanings are the console's
hardware as generally documented, not something the library shows: control
0x40 resets the port, 0x1003 / 0x3003 enable sending with the select line of
port 0 / port 1 driven and the acknowledge interrupt on, bit 4 resets the
port's interrupt flag; status bit 0 is "ready to send", bit 1 "a byte
received", bit 7 the acknowledge line, 0x200 an interrupt request; I_STAT
bit 0 is the vertical blank, bit 7 the controller and memory card port. The
baud value 0x22 is a quarter of 0x88, so four times the rate.

## padcmd.o: queued commands and the command sequences

| Offset | Function | Address |
|---|---|---|
| `+0x000` | `_padSetAct` | `0x800A5F88` |
| `+0x00C` | `_padSetCmd` | `0x800A5F94` |
| `+0x01C` | `_padSendAtLoadInfo` | `0x800A5FA4` |
| `+0x0A0` | `_padRecvAtLoadInfo` | `0x800A6028` |
| `+0x310` | `_padGetActSize` | `0x800A6298` |
| `+0x348` | `_padLoadActInfo` | `0x800A62D0` |
| `+0x41C` | `func_800A63A4` | `0x800A63A4` |
| `+0x4C4` | `func_800A644C` | `0x800A644C` |
| `+0x8BC` | `_padSetActAlign` | `0x800A6844` |
| `+0x924` | `func_800A68AC` | `0x800A68AC` |
| `+0x940` | `func_800A68C8` | `0x800A68C8` |
| `+0xA08` | `_padSetMainMode` | `0x800A6990` |
| `+0xAA0` | `func_800A6A28` | `0x800A6A28` |
| `+0xAF4` | `func_800A6A7C` | `0x800A6A7C` |
| `+0xB54` | `_padCmdParaMode` | `0x800A6ADC` |
| `+0xB74` | `func_800A6AFC` | `0x800A6AFC` |
| `+0xB88` | `func_800A6B10` | `0x800A6B10` |
| `+0xBA8` | `func_800A6B30` | `0x800A6B30` |
| `+0xBC8` | `func_800A6B50` | `0x800A6B50` |
| `+0xBE8` | `func_800A6B70` | `0x800A6B70` |

Nothing here touches the hardware. A function queues a command by writing
`cmd_queued`, `cmd_data` and `cmd_len`; step 1 of the next transfer sends the
command byte and `_padFuncGetTxd` the parameter bytes. A command sequence is
`seq` with the hooks `send_func` and `recv_func`: three functions start one
(`_padLoadActInfo`, `_padSetActAlign`, `_padSetMainMode`), each by setting
`seq` = 1 and installing two of the object's statics, and the sequencer of
`padseqd.o` drives it. A hook ends its sequence by storing 0xFE.

| Command | Parameter bytes | Queued by | Its reply is read by |
|---|---|---|---|
| 0x43 | 1, the mode, in `cmd_param` | `_padCmdParaMode`: 1 at `seq` 1, 0 at `seq` 0xFE | `func_800A795C` |
| 0x44 | 2, `mode_offs` and `mode_lock` | `func_800A6A28`, step 2 | `func_800A6A7C` |
| 0x45 | none | `func_800A6AFC`, from `_padSendAtLoadInfo` step 2 | `_padRecvAtLoadInfo` |
| 0x46 | 1, an actuator number | `func_800A6B30`, from `func_800A63A4` step 3 | `func_800A644C` step 3 |
| 0x47 | 1, a combination number | `func_800A6B50`, from `_padSendAtLoadInfo` step 4 and from `func_800A63A4` step 4 with no list bytes owed | `_padRecvAtLoadInfo`; `func_800A644C` step 4 |
| 0x4B | none | `func_800A6B70`, from `func_800A63A4` step 4 with list bytes owed | `func_800A644C` step 4 |
| 0x4C | 1, a place in the ID table | `func_800A6B10`, from `_padSendAtLoadInfo` step 3 (the current place) and `func_800A63A4` step 2 (each place in turn) | `_padRecvAtLoadInfo`; `func_800A644C` step 2 |
| 0x4D | 6, an actuator alignment, padded with 0xFF | `func_800A68AC` (the caller's bytes); `func_800A6A28` step 3 (the record's `act_align`) | `func_800A68C8`; `func_800A6A7C` |

The command byte is all the code shows. Read against the DualShock protocol
as generally documented, not Sony's text: 0x43 enters (1) or leaves (0) the
configuration mode, 0x45 asks for the model and mode, 0x4C for an entry of
the mode table, 0x46 for an actuator, 0x47 for a combination.

**Every item is read twice.** No sequence takes a value from one reply.
`prev_reply` holds what the reply before gave for the item, and the item is
taken when the next reply agrees.

- `_padSetAct(p, data, len)` stores `act_data` and `act_len`. Caller:
  `PadSetAct`.
- `_padSetCmd(p, cmd, data, len)` stores `cmd_queued`, `cmd_data` and
  `cmd_len`. Nothing calls it; the five builders at the end of the object
  write the same three members themselves.
- `_padSendAtLoadInfo(p)`, the counts sequence's send step: `seq` 2 queues
  0x45, 3 queues 0x4C with `cur_ex_offs`, 4 queues 0x47 with `index`.
  Caller: `func_800A784C`, when the record has no `send_func`.
- `_padRecvAtLoadInfo(p)`, its receive step. A reply is ignored (result 0)
  while `recv_buf[7]` is non-zero at `seq` 2, or `recv_buf[2]` or `[3]` at
  `seq` 3 and 4.
  - `seq` 2: `recv_buf[3]` to `[6]` are `id_num`, `cur_ex_offs`, `act_num` and
    `comb_num`; they are stored, `cur_ex_id` and `comb_size` cleared, and the
    result is 1 when all four already equalled the record's.
  - `seq` 3: `cur_ex_id` = `(recv_buf[4] << 8) + recv_buf[5]`; 1 when it
    equals the reply before, and `index` is then cleared.
  - `seq` 4, once per combination: `comb_size` += 8 + `recv_buf[4]` rounded up
    to 4. After the last: when `_padGetActSize(p)` is above 0x80 the
    controller is given up (`_padFuncClrInfo`, `seq` 0xFE, `state` 2); when
    `comb_size` differs from the pass before, the pass is taken again; else
    `seq` = 0xFF, `_padLoadActInfo(p, p->info_buf)`, `seq` = 2. This step
    always returns 0.
  - Any other `seq` returns 1.

  `_padLoadActInfo` is called with `seq` at 0xFF, where `_padFuncChkEng`
  answers 0 for a record with a non-zero `cur_ex_id`. When it answers 1,
  nothing is installed and the counts sequence runs again from step 2. When
  it installs the tables sequence, the `seq` = 2 that follows starts that
  sequence at step 2, with no command 0x43 sent between the two.
- `_padGetActSize(p)` returns `4 + ((id_num + 1) >> 1) * 4 + ((act_num * 5 +
  3) >> 2) * 4 + comb_size`: the ID table and the actuator table each rounded
  up to whole words, the combinations with their lists, and 4 (inferred to
  cover rounding `info_buf`, at the odd offset `+0x63`, up to a word).
- `_padLoadActInfo(p, buf)` returns 0 and changes nothing when `buf` is null,
  when `act_table` is already set, or when `_padFuncChkEng(p)` is non-zero.
  Otherwise: `state` 4, `id_table` = `buf` rounded up to 4, `seq` 1, the hooks
  `func_800A63A4` / `func_800A644C`, `index` 0, `act_table` behind the IDs,
  `comb_table` behind the actuators; returns 1. Caller: `_padRecvAtLoadInfo`.
- `func_800A63A4` (static), the tables sequence's send step: `seq` 2 queues
  0x4C for ID entry `index`, 3 queues 0x46 for actuator `index`, 4 queues 0x47
  for combination `index` while `count` is 0, else 0x4B.
- `func_800A644C` (static), its receive step. A reply with `recv_buf[2]` or
  `[3]` non-zero is ignored.
  - `seq` 2: `id_table[index]` = `(recv_buf[4] << 8) + recv_buf[5]`; when it
    equals `prev_reply` the entry is taken, and after the last of `id_num` the
    result is 1 with `index` 0.
  - `seq` 3: actuator `index` gets `func` = `recv_buf[4]`, `sub` =
    `recv_buf[5] & 0x7F`, `sign` = bit 7 of `recv_buf[5]`, `size` =
    `recv_buf[6]`, `curr` = `recv_buf[7]`; `prev_reply` = 0 when all five were
    already so, else 0xFFFF. After the last agreed actuator: `index` and
    `count` 0, result 1.
  - `seq` 4: combination `index`. With `count` 0 the reply opens it: `count`
    and `num` = `recv_buf[4]`, the list's length; `list` lies behind the last
    combination record for the first combination and behind the previous
    list, rounded up to 4, for the others; three bytes come from
    `recv_buf[5..7]`. With `count` non-zero the reply brings five more from
    `recv_buf[3..7]`. `D_800EAD60` is the write position; a list that would
    pass the end of `info_buf` restarts the step from combination 0. A list
    that differed is read again; after the last agreed combination `state` =
    6 and `seq` = 0xFE. This step always returns 0.
- `_padSetActAlign(p, data)`: unless `_padFuncChkEng` refuses, `seq` 1, hooks
  `func_800A68AC` / `func_800A68C8`, `align_data` = `data`; returns 1.
  Caller: `PadSetActAlign`.
- `func_800A68AC` (static) queues 0x4D with the six bytes at `align_data`.
- `func_800A68C8` (static) builds `act_align` from `align_data`: for each
  actuator `i` of the controller it counts the places that name `i`; when
  there are fewer than the actuator's `size` (0 counting as 1) those places
  become 0xFF, otherwise `i`. Places that name no actuator keep what they
  held. Then `seq` = 0xFE, result 0.
- `_padSetMainMode(p, offs, lock)`: unless `_padFuncChkEng` refuses, `seq` 1,
  hooks `func_800A6A28` / `func_800A6A7C`, `mode_offs` and `mode_lock`, and
  `mode_same` = `offs == cur_ex_offs`; returns 1. Caller: `PadSetMainMode`.
- `func_800A6A28` (static): `seq` 2 queues 0x44 with `mode_offs`,
  `mode_lock`; `seq` 3 queues 0x4D with `act_align`.
- `func_800A6A7C` (static): with `mode_same` set, 1 at `seq` 2 (so step 3
  sends the alignment) and at any other step `seq` = 0xFE, result 0. With it
  clear, the record is cleared through `_padFuncClrInfo`, result 0: a changed
  mode makes the library read the controller from the start.
- `_padCmdParaMode(p, mode)` queues 0x43 with `mode`. Caller: `func_800A784C`.
- `func_800A6AFC`, `func_800A6B10`, `func_800A6B30`, `func_800A6B50`,
  `func_800A6B70` (statics) queue 0x45, 0x4C, 0x46, 0x47 and 0x4B, as in the
  table; the one-parameter ones keep their byte in `cmd_param`.

## padif.o: the five steps of a transfer

| Offset | Function | Address |
|---|---|---|
| `+0x000` | `func_800A6B88` | `0x800A6B88` |
| `+0x048` | `func_800A6BD0` | `0x800A6BD0` |
| `+0x120` | `func_800A6CA8` | `0x800A6CA8` |
| `+0x1CC` | `func_800A6D54` | `0x800A6D54` |
| `+0x290` | `func_800A6E18` | `0x800A6E18` |

All five are statics, the five entries of `padIntFunc`. Each takes the
port's record and returns a negative value on an error. `_padSioRW2` reads
the port before it writes, so each step's result is the answer to the byte
the step before sent.

- `func_800A6B88`, step 0: `D_800B8508` = `_padFuncSendAuto(p)`;
  `recv_buf[0]` = 0; returns `_padSioRW(p, -2)`, which sends 1.
- `func_800A6BD0`, step 1: when the port is the first served and
  `_padGunExec` is set, calls `_padFuncGetGunPos` and `_padFuncSetGunPort`;
  when `D_800B8508` is set, `_padFuncSendAuto` for `tap_info[0]` and `[1]`.
  Sends `cmd_queued`, or 0x42 when none is queued.
- `func_800A6CA8`, step 2: when `D_800B8508` is set, `_padFuncSendAuto` for
  `tap_info[2]` and `[3]`. Sends 0 with a command queued, else
  `_padModeMtap`. The byte received is the ID: `D_800B8500` = `(ID & 0xF) *
  2`, or 0x20 when that is 0; returns 0.
- `func_800A6D54`, step 3: `D_800B850C` = 1 when `_padModeMtap` is set, the
  ID's high nibble is 8 and no command is queued; else 0, and then
  `_padFuncCurrLimit(p)`. Sends `_padFuncGetTxd(p, D_800B850C)`. Returns the
  byte received when it is 0x5A or 0, an error as it is, and -4 for any other
  byte.
- `func_800A6E18`, step 4, the payload. After each byte `setRC2wait(0x3C)`
  and `_padClrIntSio0`, whose 0 ends the transfer with -3; an error of
  `_padSioRW` / `_padSioRW2` is returned as it is. `D_800B8500` counts the
  bytes left.
  1. With a multi tap (`D_800B850C`): `_padFuncCurrLimit(p)`, then up to five
     bytes through `_padSioRW2` with `_padFuncGetTxd(p, 1)`, calling
     `_padFuncCurrLimit` for `tap_info[0]` to `[3]` before the second to
     fifth.
  2. While more than one byte is left, one step per byte of the OTHER port's
     `_padFixResult` word `c` (`c` = 1 when `_padSioChan` is 0, else 0):
     negative ends this part; positive `n` calls `_padFuncRecvAuto` for
     `_padInfoDir[c].tap_info[n - 1]`, then 4 becomes 3, 3 calls it for the
     record before and becomes 1, 2 stays; 0 or 1 calls `_padFuncRecvAuto`
     and `_padFuncClrCmdNo` for `_padInfoDir[c]` and becomes -1. The byte
     goes through `_padSioRW`.
  3. The rest of the bytes through `_padSioRW`; the acknowledge wait is
     skipped while SIO0's baud register reads 0x22.
  4. `_padWaitRXready`, the last byte appended to `recv_buf`,
     `_padFuncNextPort(0)`. Returns 0.

## padportd.o: the two direct ports

| Offset | Function | Address | Installed as |
|---|---|---|---|
| `+0x000` | `PadInitDirect` | `0x800A71B8` | |
| `+0x160` | `func_800A7318` | `0x800A7318` | `_padFuncClrInfo` |
| `+0x1C8` | `func_800A7380` | `0x800A7380` | `_padFuncNextPort` |
| `+0x2BC` | `func_800A7474` | `0x800A7474` | `_padFuncClrCmdNo` |
| `+0x2CC` | `func_800A7484` | `0x800A7484` | `_padFuncGetTxd` |
| `+0x388` | `func_800A7540` | `0x800A7540` | `_padFuncCurrLimit` |
| `+0x604` | `func_800A77BC` | `0x800A77BC` | `_padFuncPtr2Port` |
| `+0x63C` | `func_800A77F4` | `0x800A77F4` | `_padFuncPort2Info` |

- `PadInitDirect(pad1, pad2)` (libpad.h: `void PadInitDirect(unsigned char
  *, unsigned char *)`; the two are the caller's receive buffers, 34 bytes
  each by the C's comment): `_padIntExec` = 0, `_padModeMtap` = 0,
  `_padInitDirSeq()`, `_padInfoDir` = `D_800EAE00`, the seven hooks of the
  table; both records zeroed; `rx_buf` = `pad1` / `pad2`. Then for each
  record: `tap_info` 0, `head` the record itself, `rx_buf[0]` 0xFF and
  `rx_buf[1]` 0, `recv_buf` = `D_800EAD70[i]`, `send_buf` = `D_800EADB8[i]`,
  six 0xFF in `act_align`. Last `_padSetVsyncParam()` and `_padIntExec` = 1.
  Caller: `pad_start`.
- `func_800A7318(p)` forgets a controller. A record in state 0 is left alone.
  Otherwise `state`, `seq`, `send_func`, `recv_func`, `id_num`,
  `cur_ex_offs`, `cur_ex_id` (stored twice), `act_num`, `comb_num` and the
  three table pointers become 0, and `act_align` six 0xFF. `cur_id`, the
  buffers and the queued command are left as they are. Callers, through the
  hook: `padseqd.o` (five places), `padcmd.o` (two) and `padmain.o`.
- `func_800A7380(result)` closes the transfer with port `_padSioChan`: 0
  clears its `_padFixResult` word, -9 changes nothing, and any other value is
  a failure, handed to `_dirFailAuto(p, result)` and then `func_800A7474(p)`.
  Then `_padSioState` = 0, SIO0 `ctrl` = 0 and `_padSioChan` steps on. Past
  `_padChanStop` it returns; else it calls `_padInitSioMode` on the new port,
  and a 0 from that goes round again with result 0xFFFF. The results that
  reach it are 0 and 0xFFFF and the errors -2, -3, -4 and -0x14; no linked
  function produces -9.
- `func_800A7474(p)`: `cmd_taken` = `cmd_queued`, `cmd_queued` = 0.
- `func_800A7484(p, mtap)` returns the byte to send at place `n` =
  `send_len - 3`. With no command queued: 0 when `n` is below 6 and
  `act_enable[n]` is 0; else `act_data[n]` below `act_len`, else 0. With 0x4D
  queued: `cmd_data[n]` below `cmd_len`, else 0xFF. Any other command: the
  same, else 0. It does not read `mtap`; every call in `padif.o` passes one.
- `func_800A7540(p)` sets `act_enable` within the round's budget:
  `_padTotalCurr` may not pass 60. First the six bytes are cleared. Then the
  first case that applies:
  - `cur_ex_id` and `act_data` both set: with `n` = `act_len`, at most 6, for
    each actuator `i` below `act_num`, when one of the first `n` bytes
    aligned to `i` asks for it (non-zero; bit 0 alone for an actuator of size
    0) and `_padTotalCurr + curr` is below 61, the current is added and every
    byte aligned to `i` is enabled;
  - `cur_id` 4, 5 or 7, `cur_ex_id` 0 and `act_len` at least 2: when
    `(act_data[0] & 0xC0) == 0x40`, `act_data[1] & 1` and `_padTotalCurr +
    10` is below 61, bytes 0 and 1 are enabled and 10 is added; otherwise
    nothing;
  - `cur_id` 3: byte 0 is enabled;
  - `cur_ex_id` 0: all six are enabled.

  The handler zeroes `_padTotalCurr` before its port loop, so the budget is
  per vertical blank over both ports.
- `func_800A77BC(p)` returns 0x10 for the first record, 0x20 for the second,
  0xFF for any other pointer. It numbers the ports from 0x10, where
  `func_800A77F4` and the game number them 0x00 and 0x10. Nothing calls it;
  `PadInitDirect` only stores it.
- `func_800A77F4(port)` returns the first record, or the second when `port &
  0xF0` is non-zero.

## padseqd.o: the direct ports' sequencer

| Offset | Function | Address | Installed as |
|---|---|---|---|
| `+0x000` | `_padInitDirSeq` | `0x800A7818` | |
| `+0x034` | `func_800A784C` | `0x800A784C` | `_padFuncSendAuto` |
| `+0x144` | `func_800A795C` | `0x800A795C` | `_padFuncRecvAuto` |
| `+0x3A8` | `_dirFailAuto` | `0x800A7BC0` | |
| `+0x488` | `func_800A7CA0` | `0x800A7CA0` | `_padFuncChkEng` |

| `seq` | Send step (`func_800A784C`) | Receive step (`func_800A795C`) |
|---|---|---|
| 0 | nothing | `state` 1, `seq` 1 |
| 1 | `_padCmdParaMode(p, 1)` | `index` 0, `seq` 2 |
| 2 to 0xFD | `send_func`, or `_padSendAtLoadInfo` | the record is cleared when the reply's first byte is not 0xF3; else `seq` += `recv_func`'s result, or `_padRecvAtLoadInfo`'s |
| 0xFE | `_padCmdParaMode(p, 0)` | `seq` 0xFF |
| 0xFF | nothing | nothing |

At every step but 0 the receive step stops after its common part unless a
command is queued. Read against the protocol, not Sony's text: 0xF3 is the
ID a controller answers with in configuration mode, so steps 1 and 0xFE
bracket the queries.

- `_padInitDirSeq()` points `_padFuncSendAuto`, `_padFuncChkEng` and
  `_padFuncRecvAuto` at the three statics. Caller: `PadInitDirect`.
- `func_800A784C(p)`, the send step; always returns 0. When `recv_buf[0]` is
  0xF3: with `cur_id` 0, or with `seq` 0xFF, it queues `_padCmdParaMode(p,
  0)` and is done; otherwise a record in state 2 is cleared. Then it acts by
  `seq` as in the table.
- `func_800A795C(p)`, the receive step.
  1. A first byte with a zero high nibble is no controller: `rx_buf` gets
     0xFF, 0; `cur_id` and `rx_len` 0; the record is cleared; return.
  2. The high nibble becomes `cur_id`, except 0xF, which keeps the old ID.
     For any other ID the reply goes to the game: `rx_buf[0]` 0, `rx_buf[1]`
     the first byte, `rx_len` = `recv_len`, and bytes 2 to `recv_len - 1`
     copied.
  3. The record is cleared when the ID changed, or when `recv_buf[1]` is 0
     while `unk_50` is 0 and the step is not "1 with no `send_func`".
  4. `retry` = 0.
  5. It returns at `seq` 0xFF, and at a non-zero `seq` with no command
     queued; at `seq` 2 to 0xFD with a first byte other than 0xF3 it clears
     the record and returns.
  6. It steps the sequence as in the table.
- `_dirFailAuto(p, result)`, a failed transfer: `fail_count` += 1. At `seq`
  1, the first eleven failures in a row only count (`retry`); the twelfth
  sets `state` 2 and `seq` 0xFF and returns. At any later step the first
  eleven only count; the twelfth clears a record whose state is not 0. That
  twelfth failure, and every failure at `seq` 0, then reports no controller
  unless `recv_buf[0]` is 0xF3: `rx_buf` gets 0xFF, 0 and `cur_id` and
  `rx_len` become 0. `result` is never read; its one caller,
  `func_800A7380`, passes it.
- `func_800A7CA0(p)` returns 0 when `cur_ex_id` is non-zero and `seq` is
  0xFF, else 1. What "Eng" in the hook's symbol stands for is not
  established.

## waitrc2.o: the time-out on root counter 2

| Offset | Function | Address |
|---|---|---|
| `+0x00` | `setRC2wait` | `0x800A7CD8` |
| `+0x20` | `chkRC2wait` | `0x800A7CF8` |

The member reads the counter at its literal addresses: 0x1F801120 the
count, 0x1F801124 the mode, 0x1F801128 the target.

- `setRC2wait(wait)`: `_waitTime` = `wait`, `_startTime` = the count.
  Callers: `padmain.o`, `padif.o`.
- `chkRC2wait()` returns 1 when the wait is over. A count below `_startTime`
  means the counter wrapped, and the target is added, or 0x10000 when the
  target is 0. The time gone by is the difference when mode bit 9 is set
  (the counter runs on the system clock / 8) and the difference / 8 when it
  is clear, so the unit is always eight system-clock ticks.

`_padSioRW` writes `setRC2wait`'s two statements out in line, and
`_padSioRW2` `chkRC2wait`'s test. The waits used are 80 or 145, then 2000,
430 and 60 in its three-byte exchange (`_padInitSioMode`), 0x1AE
(`_padSioRW`), 0x190 and 0x3C (`_padSioRW2`) and 0x3C (`func_800A594C`,
step 4).

## Beside libpad: libc2 `strncpy`

`strncpy(dst, src, n)` (`0x800A4C78`, libc2 `strncpy.o`, `libc2_strncpy.c`)
returns 0 without touching anything when either pointer is null. Otherwise
it copies at most `n` characters, fills the rest of the `n` with zeroes once
the NUL has been copied, and returns `dst`; a `src` of `n` characters or
more leaves `dst` unterminated, and `n <= 0` copies nothing. Its one caller
is libmcrd's `_card_create2` (`0x800A45B8`), which copies the file name into
a directory frame with `n` = 20. The 4.6 and 4.7 libraries' `strncpy` is a
BIOS call (A0:1A); the game links libc2's compiled one.

## Not established

- `unk_39` and `unk_50`: what they stand for. The linked code only reads
  them, so in this game both stay 0.
- `unk_eb`: only ever cleared.
- `_padFuncChkEng`'s "Eng".
- What a controller does with each command byte. The protocol readings above
  (0x01, 0x42, 0x5A, 0x43, 0x45, 0x46, 0x47, 0x4C, 0xF3) are the protocol as
  generally documented; 0x4B's meaning there is not given here.
- The meaning of `ChangeClearRCnt`'s second argument, inferred from its name.
