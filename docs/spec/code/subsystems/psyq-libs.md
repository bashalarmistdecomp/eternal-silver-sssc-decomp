# PSY-Q library code: the libraries the program links

This page is the overview of Sony's PSY-Q library code in the main
executable, `SLUS_006.28`: which libraries it links, which release, and
where each library's page is. It also holds the subjects no library's own
page holds: libsnd's score record, SEQ reader, tick and voice manager;
libspu's reverb, allocation table, voice masks, pitch tables and transfer
check; libcd's file search, `bios.o`'s interrupt state, start-up and command
writer, and `cdread.o`; libgs's double buffer, 3D defaults and flat lights;
libgpu's environment `GEnv`, `ResetGraph`, `PutDispEnv`, the command queue's
runner and the small drawing calls; libgte's `SquareRoot0`; and libapi's
root counters, pad patch and `firstfile`.

## The libraries and their pages

The program links release 4.5 of Sony's PlayStation libraries, every library
of it. [docs/naming.md](../../../naming.md) ("Which release of Sony's
libraries the programs link") says how that was measured, library by
library, and how each of Sony's names was proven. The library code fills the
end of the executable's code, `0x8008B388` to `0x800A7D98`, after the game's
own code. Sony's start-up object, `2mbyte.obj`, is not a library member: it
lies at `0x80013C44`, before the game's code, and stays assembly in
`src/2mbyte.c`. [psyq/index.md](psyq/index.md) lists every library function
by address, with its member, its file and its recipe markers, and
[psyq/codegen.md](psyq/codegen.md) says how the objects were compiled and
which shapes of C decide their bytes.

| Library | Where its members are described |
|---|---|
| libapi | The BIOS stubs: [bios-calls.md](bios-calls.md). The critical section and the second heap: [psyq/libgpu-libgs.md](psyq/libgpu-libgs.md). `PAD.OBJ`: [psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md). The root counters (`counter.o`), `PATCH.OBJ` and `CHCLRPAD.OBJ`, and `first.o`: this page. |
| libc2 | The string and memory functions: [psyq/libgpu-libgs.md](psyq/libgpu-libgs.md). `puts`, `putchar` and `strncmp`: [psyq/libcd.md](psyq/libcd.md). `setjmp` and `longjmp`: [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md). `strncpy`: [psyq/libpad.md](psyq/libpad.md). `strcat`: [psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md). |
| libcard | [psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md). |
| libcd | `event.o`, `toc.o`, `sys.o`, `bios.o`'s sector DMA and interrupt handler, `cdread.o`'s first two functions, the stream library and `s_002.o`: [psyq/libcd.md](psyq/libcd.md). `iso9660.o`'s file search, `bios.o`'s interrupt state, start-up and `CD_cw`, and `cdread.o`'s state and public functions: this page. |
| libetc | `vsync.o`, `intr.o` and `intr_dma.o`: [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md). `VSync`: [psyq/libcd.md](psyq/libcd.md). |
| libgpu | Most of `sys.o` (its data and function table, the settings, the drawing-environment builder, the hardware routines, the queue's front door, reset and sync, the timeout, the unqueued `*2` calls) and `LoadClut`: [psyq/libgpu-libgs.md](psyq/libgpu-libgs.md). `GEnv`, `ResetGraph`, `PutDispEnv`, the queue's runner and the small drawing calls: this page. |
| libgs | `gs_001.o` (`GsInitGraph`): [psyq/libgpu-libgs.md](psyq/libgpu-libgs.md). `gs_122.o` and `gs_123.o`: [psyq/libcd.md](psyq/libcd.md). The double buffer, the 3D defaults and the flat lights: this page. |
| libgte | `msc00.o`, `reg11.o`, `reg12.o` and `patchgte.o`: [psyq/libcd.md](psyq/libcd.md). `SquareRoot0` (`msc01.o`): this page. |
| libmcrd | [psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md). |
| libpad | [psyq/libpad.md](psyq/libpad.md). |
| libsnd | Closing, resetting and opening a score, the SEQ reader's handler table, the control-change event, tone attributes 11 and 12, parts of the voice manager, mono and stereo, the VAB header's public entries, the reverb type, the damper, play, pause, stop and the size of the tick setting: [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md). `SsVabFakeHead`: [psyq/libspu.md](psyq/libspu.md). The score record, the SEQ reader, the tick and the voice manager: this page. The functions the music path reaches, as a call graph: [libsnd-map.md](../libsnd-map.md). |
| libspu | `spu.o`, `s_ini.o`, the allocator, `SpuSetKey`, `SpuWrite` and `SpuClearReverbWorkArea`: [psyq/libspu.md](psyq/libspu.md). The reverb, the allocation table's collector and tests, the voice masks, the pitch tables and the transfer check: this page. |

A member the table does not name (libetc's `intr_vb.o` and `vmode.o`, most of
libgs, much of libsnd and libspu) is listed in [psyq/index.md](psyq/index.md),
with its file and recipe markers.

The movie-player overlay, `MENU.BIN` (`SLUS_006.28-MENU`), links one more
library, libpress, and from release 4.1, not 4.5
([docs/naming.md](../../../naming.md) has the measurement). Its members are
in [overlay.md](overlay.md). The stream library is linked into the main
program, but only the overlay's `movie.c` drives it
([psyq/libcd.md](psyq/libcd.md), "libcd: the streaming ring").

Most of the libraries' debug messages go to `printf` (`0x80021700`). That
`printf` is the game's own, in `fmt.c`, and its body is empty, so those
messages are never printed, whatever debug level a library is set to. A few
(among them libcd's time-outs and libetc's) go through libc2's `puts` to the
console, file descriptor 1 ([psyq/libcd.md](psyq/libcd.md), "libc2: the
console and `strncmp`").

## Conventions

- A function is cited by its name with its address beside it. A function
  inside a member that gives it no public name keeps its address
  (`func_8008DC4C`), unless Sony's own text in the member or another
  release's identical bytes name it.
- Most library data has no name in the register. Such a global is its `D_`
  symbol; a name in brackets after it, or in a table's "The C's comment"
  column, is the one the C's comments give it.
- Members are named as release 4.5 names them (`sys.o`, `midiread.o`). Each
  has its own file, `lib<library>_<member>.c`.
- Struct and field names are those of
  `decomp/SLUS_006.28/include/psyq_private.h`.
- Offsets are hexadecimal and relative to the record named.

The SEQ and VAB file formats are in [sound.md](../../sound.md).

## libsnd — the score record (`D_800E9628`)

`D_800E9628` (`_ss_score`) is an array of pointers, each to a row of 0xB0-byte
score records, the type `SsScore` in `include/psyq_private.h`. One record is
the playback state of one sequence. A score is `D_800E9628[a][b]`: `a` is the
access number of an opened SEQ or SEP and `b` the sequence inside a SEP (a SEQ
opened by `SsSeqOpen` (`0x80095B38`) uses record 0). The SDK's parameter names
for the pair change from object to object (`(seq_no, sep_no)` in the reader's
handlers, `(sep_no, seq_no)` in `ssvol.o`); the first is always the row. The
voice manager carries both numbers in one short, `seq_sep = a | b << 8`, and
finds the record as `D_800E9628[seq_sep & 0xFF] + ((seq_sep & 0xFF00) >> 8)`.
`seq_sep` 0x21 names no score: it is the pseudo-score of a direct key-on
(§"The volume law").

| Offset | Type | Name in the C | Meaning |
|---|---|---|---|
| `+0x00` | `u8 *` | `read_pos` | The next unread byte of the SEQ data. Every handler that takes data advances it. |
| `+0x04` | `u8 *` | `start_pos` | The first event: the read pointer after the header and the first delta time (`_SsInitSoundSeq` (`0x80097C48`)). A start (`Snd_SetPlayMode` (`0x80098018`), `_SsSndNextSep` (`0x80099088`)) and a stop (`_SsSndStop` (`0x800991E8`)) put `read_pos` back here. So does the end of a track (`_SsSeqGetEof` (`0x80098A94`)) when the song repeats or plays for ever, unless flag 0x400 is set. |
| `+0x08` | `u8 *` | `loop_pos` | Where the NRPN loop jumps back to: `_SsContNrpn2` (`0x80096698`) stores `read_pos` here at a loop start and loads it back at a loop end. A start (`Snd_SetPlayMode`) and a stop (`_SsSndStop`) set it to `start_pos`; `_SsSeqGetEof` sets it to the rewind point at a repeat and at the final end, but not when the song plays for ever. `_SsSndNextSep` leaves it. |
| `+0x0C` | `u8 *` | `rewind_pos` | The rewind point while flag 0x400 is set. `_SsInitSoundSeq` and `Snd_SetPlayMode` set it to `start_pos`. |
| `+0x10` | `u8 *` | `end_pos` | The end point while flags 1 and 0x400 are both set: `_SsGetSeqData` (`0x80098CD8`) ends the track once it has read the byte there. `_SsInitSoundSeq` writes 0. |
| `+0x14` | `u8` | `play_mode` | 1 while the score plays: `Snd_SetPlayMode` (for play), `_SsSndNextSep` and `_SsSndReplay` (`0x80099188`) write 1; `_SsSndPause` (`0x800988C8`), `_SsSndStop`, the end of the track and `_SsInitSoundSeq` write 0. `SsIsEos` (`0x80095AF8`) returns it. |
| `+0x15` | `u8` | `loop_on` | 1 once a repeat count has been taken for the armed loop (`_SsContDataEntry` (`0x80095E48`)); `_SsContNrpn2` clears it when the loop is done. |
| `+0x16` | `u8` | `running_status` | The last status byte's high nibble (0x90, 0xB0, 0xC0, 0xE0), or 0xFF after a meta event (`_SsGetSeqData`). |
| `+0x17` | `u8` | `channel` | The MIDI channel of the event being read: the index into `panpot`, `programs` and `vol`. |
| `+0x18` | `u8` | `rpn_lsb` | The RPN number's low byte: control change 100. |
| `+0x19` | `u8` | `rpn_msb` | The RPN number's high byte: control change 101. |
| `+0x1A` | `u8` | `nrpn_lsb` | The NRPN control change 98 value: the index into `D_800E9588.ccentry`. |
| `+0x1B` | `u8` | `nrpn_msb` | The NRPN control change 99 value: the tone number (16 means tone 0), or 20, 30 and 40 for a loop start, a loop end and a mark. |
| `+0x1C` | `u8` | `loop_armed` | 1 while a loop start has been seen and its repeat count not taken yet. |
| `+0x1D` | `u8` | `loop_count` | The loop's repeat count, taken from a data entry. |
| `+0x1E` | `u8` | `rpn_count` | RPN bytes received; 2 = both. |
| `+0x1F` | `u8` | `nrpn_count` | NRPN bytes received; 2 = both. |
| `+0x20` | `s8` | `l_count` | The song repetitions wanted (`SsSetLoop` (`0x80095AB8`), `Snd_SetPlayMode`); 0 plays for ever (`_SsSeqGetEof`); `_SsInitSoundSeq` and `_SsSndNextSep` write 1. |
| `+0x21` | `s8` | `l_count_done` | Repetitions played: counted at each end of a track and compared, signed, with `l_count`. 0 after `SsSetLoop`, a start, a stop and `_SsInitSoundSeq`. |
| `+0x22`, `+0x23` | `s8` | `next_seq`, `next_sep` | The score to start when this one finishes (`_SsSndNextSep`); `next_seq` -1 = none. Nothing in the image writes anything but -1 and 0 (`SsSetTableSize` (`0x800993C8`), `func_80095758`). |
| `+0x24`, `+0x25` | `u8` | `rhythm_n`, `rhythm_d` | SEQ header bytes 13 and 14, the time signature. |
| `+0x26` | `s8` | `vab_id` | The VAB the score plays. Control change 0 (bank change) rewrites it. |
| `+0x27` | `u8[16]` | `panpot` | Per channel, the pan; 0x40 at open. |
| `+0x37` | `u8[16]` | `programs` | Per channel, the program; the channel number at open. |
| `+0x47` | `u8` | `pad_47` | Nothing reads or writes it. |
| `+0x48` | `s16` | `cres_vol` | Volume ramp: the whole change, signed. |
| `+0x4A` | `s16` | `cres_done` | Volume ramp: the part of it already applied. |
| `+0x4C` | `s16` | `unk_4c` | `SsSetTableSize` and `func_80095758` write 0; nothing reads it. |
| `+0x4E` | `s16` | `tempo_step` | Tempo ramp: above 0, one tempo unit every that many ticks; 0 or below, the negated size of the step per tick (`_SsSndTempo` (`0x800998F8`)). |
| `+0x50` | `s16` | `resolution` | Delta ticks per quarter note (SEQ header bytes 8-9). |
| `+0x52` | `s16` | `tick_wait` | `_SsSeqPlay` (`0x80098998`)'s countdown, reloaded from `tick_step`; -1 = no countdown. |
| `+0x54` | `s16` | `tick_step` | The delta time `_SsSeqPlay` takes per tick, in tenths of a delta tick, or the countdown's length. |
| `+0x56` | `s16` | `hdr_tick_step` | `tick_step` as derived from the header tempo; `_SsSndStop` puts it back into `tick_step`. |
| `+0x58`, `+0x5A` | `u16` | `voll`, `volr` | The score's left and right volume. |
| `+0x5C`, `+0x5E` | `u16` | `cres_voll`, `cres_volr` | The volume pair read back at the end of each ramp tick; 127 after `SsSetTableSize` and a stop. |
| `+0x60` | `s16[16]` | `vol` | Per channel, the volume; 0x7F at open. |
| `+0x80` | `s16` | `mute_mask` | The channel mute mask, a bit per channel: `_SsNoteOn` (`0x80097A48`) keys a note on only while its channel's bit is clear. Only `_SsInitSoundSeq` writes it, with 0. |
| `+0x82` | `s16` | `pad_82` | Nothing reads or writes it. |
| `+0x84` | `s32` | `start_delta` | The first event's delta time, kept at open; `_SsSndStop` puts it back into `delta_value`. |
| `+0x88` | `s32` | `delta_sum` | The sum of every delta time read (each already times 10); 0 at open, at a repeat and at a stop. |
| `+0x8C` | `s32` | `hdr_tempo` | The header's tempo, quarter notes per minute; `_SsSndStop` puts it back into `tempo`. |
| `+0x90` | `s32` | `delta_value` | The delta time left until the next event, in tenths of a delta tick. |
| `+0x94` | `u32` | `tempo` | The current tempo, quarter notes per minute. |
| `+0x98` | `s32` | `flags` | The play-flag word (below). |
| `+0x9C` | `s32` | `cres_time` | Volume ramp: its length in ticks. |
| `+0xA0` | `s32` | `cres_tick` | Volume ramp: the ticks gone. |
| `+0xA4` | `s32` | `unk_a4` | `SsSetTableSize` and `func_80095758` write 0; nothing reads it. |
| `+0xA8` | `s32` | `tempo_count` | Tempo ramp: the ticks left, signed. |
| `+0xAC` | `u32` | `tempo_goal` | Tempo ramp: the tempo to reach. |

Notes on the table:

* `+0x60` is `s16` in the header. The functions that use the array do not
  settle the original sign: an unsigned element gives the same code in each
  of them.
* `+0x1C` / `+0x1D`: the arming event is NRPN 99 = 20 and a count of 127
  repeats for ever; both are in `_SsContNrpn2` (§"The NRPN loop").

**The play-flag word `+0x98`.** A bit field. The value 1 exactly means
"playing and nothing else": the three volume setters test `flags == 1`. The
tick, `SsSeqCalledTbyT` (`0x80098448`), serves the bits (§"libsnd — the
tick").

| Bit | Meaning | Set by | Served by |
|---|---|---|---|
| 1 | playing | `Snd_SetPlayMode` for play, `_SsSndNextSep` | `_SsSndPlay` (`0x80098968`), every tick |
| 2 | pause requested | `Snd_SetPlayMode` for pause | `_SsSndPause` |
| 4 | stop requested | the end of a track (`_SsSeqGetEof`) and `_SsSndStop` itself | `_SsSndStop`, after which the whole word is cleared |
| 8 | replay requested | nothing in the image | `_SsSndReplay` |
| 0x10, 0x20 | a volume ramp is running | nothing in the image | `_SsSndCrescendo` (`0x800986B8`), once for each bit |
| 0x40, 0x80 | a tempo ramp is running | nothing in the image | `_SsSndTempo`, once for each bit |
| 0x100 | not known | nothing; `_SsSndNextSep` clears it | — |
| 0x200 | the score has finished | `_SsSeqGetEof` | — |
| 0x400 | point to point: `rewind_pos` and `end_pos` take the place of `start_pos` and the track's end | nothing in the image; `_SsSndStop` clears it | `_SsGetSeqData`, `_SsSeqGetEof` |

In this game a score only plays, pauses, stops and ends. No function in the
image sets flags 8, 0x10 to 0x80, 0x100 or 0x400, so `_SsSndReplay`,
`_SsSndCrescendo` and `_SsSndTempo` never run, and the point-to-point paths of
`_SsGetSeqData` and `_SsSeqGetEof` are dead. The libsnd.h calls that would set
them (`SsSeqPlayPtoP` and the crescendo, accelerando and replay calls) have no
row in the register. The same holds for the mute mask (`SsChannelMute`) and
the chained score (`next_seq`).

## libsnd — the SEQ reader

The reader does not call its event handlers by name: it goes through the
function table `D_800E9588` (`SsFCALL`), which `SsSeqOpen` (`0x80095B38`)
fills every time it opens a SEQ. The table, slot by slot, and `SsSeqOpen`
itself are in [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md), "libsnd — opening a SEQ
and the handler table (`ssopenq.o`)". Its last slots, `ccentry[20]`, are the
tone-attribute handlers (§"The tone-attribute handlers").

### The SEQ header and delta times

* **`_SsInitSoundSeq` (`0x80097C48`)**, `(seq_no, vab_id, addr)`
  (`seqinit.o`). `SsSeqOpen`'s worker. Resets record 0 of
  `D_800E9628[seq_no]`: `vab_id` from the argument; `resolution`, the RPN and
  NRPN bytes and counters, `channel`, the loop bytes (`loop_on`,
  `loop_armed`, `loop_count`), `play_mode`, `running_status`,
  `l_count_done`, `mute_mask`, `start_delta`, `delta_sum`, `hdr_tempo`,
  `hdr_tick_step`, `delta_value` and the rhythm bytes all 0; `l_count` = 1;
  for each of the 16 channels `programs[c]` = c, `panpot[c]` = 0x40,
  `vol[c]` = 0x7F; `tick_wait` = 1; `read_pos` = `addr`. It does not touch
  the volume pair, the ramp fields or the play flags. Then the SEQ header.
  The first byte must be `'S'` or `'p'` (the tag reads "pQES"); otherwise it
  prints "This is an old SEQ Data Format.\n" (`D_8001388C`) and returns 0, so
  `SsSeqOpen` hands out an access number for data it did not read. Byte 7,
  the version, must be 1; otherwise "This is not SEQ Data.\n" (`D_80013874`)
  and -1. `resolution` = bytes 8-9, big-endian. Bytes 10-12, big-endian, are
  microseconds per quarter note: `hdr_tempo` = 60000000 / that, rounded to
  nearest, and `tempo` takes the same value. Bytes 13-14 go to `rhythm_n` /
  `rhythm_d`. `delta_value` = `start_delta` = `_SsReadDeltaValue`
  (`0x80097B98`)`(seq_no, 0)`; the read pointer, now on the first event, is
  saved in `start_pos`, `loop_pos` and `rewind_pos`, and `end_pos` = 0. The
  play step is then derived from `hdr_tempo` exactly as `_SsGetMetaEvent`
  (`0x80097878`) derives it from a tempo event (§"The event handlers"), and
  `hdr_tick_step` keeps a copy of `tick_step`. Returns 0. The messages go
  through `printf` (`0x80021700`).

* **`_SsReadDeltaValue` (`0x80097B98`)**, `(seq_no, sep_no)`
  (`miditime.o`). Reads one MIDI variable-length quantity at `read_pos` and
  advances `read_pos` past it. A zero first byte returns 0 and leaves
  `delta_sum` alone. Otherwise it takes seven bits per byte while bit 7 is
  set; the value **times 10** is added to `delta_sum` and returned. Delta
  times are therefore held in tenths of a delta tick everywhere in the score
  record.

### The per-tick step and the event reader

* **`_SsSeqPlay` (`0x80098998`)**, `(seq_no, sep_no)` (`midiread.o`). One
  tick of one score; its only caller is `_SsSndPlay` (`0x80098968`), which
  does nothing else. When `delta_value - tick_step > 0` the next event is
  more than one step away: `tick_wait > 0` decrements `tick_wait`;
  `tick_wait == 0` reloads it from `tick_step` and takes 1 off
  `delta_value`; `tick_wait < 0` takes `tick_step` off `delta_value`.
  Otherwise (`delta_value <= tick_step`) it reads events: `_SsGetSeqData`
  (`0x80098CD8`) is called, and called again while it leaves `delta_value`
  at 0; each new `delta_value` is added to the old one until the sum reaches
  `tick_step`, and `delta_value` becomes the sum less `tick_step`.

* **`_SsGetSeqData` (`0x80098CD8`)**, `(seq_no, sep_no)` (`midiread.o`).
  Reads one SEQ event and hands it to its handler through `D_800E9588`. It
  returns a value; its one caller `_SsSeqPlay` ignores it.
  1. `data = *score->read_pos++`; `ret = 0`.
  2. If `(flags & 0x401) == 0x401` and `read_pos == end_pos + 1` (the byte
     just read was the one at the end point): `_SsSeqGetEof` (`0x80098A94`)
     `(seq_no, sep_no, end_pos[1])` and **return -1**.
  3. `data & 0x80`, a status byte: `channel` = `data & 0x0F`, then by
     `data & 0xF0`:
     * 0x90: `running_status` = 0x90; note and velocity are read;
       `delta_value` = `_SsReadDeltaValue` (`0x80097B98`); `SsFCALL.noteon(seq,
       sep, note, vel)`.
     * 0xB0: `running_status` = 0xB0; one byte;
       `D_800E9588.control[CC_NUMBER](seq, sep, byte)`.
     * 0xC0: `running_status` = 0xC0; one byte; `SsFCALL.programchange(seq,
       sep, byte)`.
     * 0xE0: `running_status` = 0xE0; one byte skipped;
       `D_800E9588.pitchbend(seq, sep)`.
     * 0xF0: `running_status` = 0xFF; one byte, the meta type. 0x2F:
       `ret = 1` and `_SsSeqGetEof(seq, sep, 0x2F)`. Otherwise
       `D_800E9588.metaevent(seq, sep, type)`.
     * any other status: nothing.
  4. Otherwise `data` is the first data byte under `running_status`: the
     same five arms, with `data` as the note, control, program or meta type
     (0xE0 skips nothing).
  5. **return ret**: 1 at an end-of-track meta event, 0 otherwise.

  The handlers are called through libsnd.h's unprototyped `void (*)()`
  members.

### The end of a track

**`_SsSeqGetEof` (`0x80098A94`)**, `(seq_no, sep_no, type)` (`midiread.o`).
Its only caller is `_SsGetSeqData` (`0x80098CD8`). The third argument is never
read. `l_count_done` is incremented, then:

* `l_count` == 0, play for ever: `delta_sum` = 0, `loop_armed` = 0,
  `delta_value` = 0, `read_pos` = the rewind point.
* `l_count_done < l_count` (a signed char compare): the same, and `loop_pos`
  also takes the rewind point.
* otherwise the score is finished: flags `&= ~1`, `&= ~8`, `&= ~2`,
  `|= 0x200`, `|= 4`; `play_mode` = 0 and `loop_pos` = the rewind point; if
  `next_seq` is not -1, `_SsSndNextSep` (`0x80099088`)`(next_seq, next_sep)`
  and `_SsVmSeqKeyOff` (`0x8009CE38`)`(seq_no | sep_no << 8)`; then
  `_SsVmSeqKeyOff` again unconditionally (twice when a score is chained);
  `delta_value = tick_step`.

The rewind point is `rewind_pos` when flag 0x400 is set, else `start_pos`.
Flag 4 makes the next tick call `_SsSndStop` (`0x800991E8`), which rewinds the
score and keys off its voices again, and then clear the whole flag word
(§"libsnd — the tick"). Nothing in the image sets 0x400 or a `next_seq` other
than -1, so in this game a finished score only stops.

### The event handlers

Every handler takes `(seq_no, sep_no)` first and works on `score =
&_ss_score[seq_no][sep_no]`; `ch` below is `score->channel`. Each one except
`_SsNoteOn` (`0x80097A48`) ends by storing `_SsReadDeltaValue`
(`0x80097B98`)`(seq_no, sep_no)` in `delta_value`: reading the next delta
time is the handler's job (`_SsGetSeqData` (`0x80098CD8`) reads it before the
note handler).

* **`_SsGetMetaEvent` (`0x80097878`)**, `(seq_no, sep_no, type)`
  (`midimeta.o`). The tempo meta event (FF 51). The reader calls it through
  `D_800E9588.metaevent` for every meta type except 0x2F, which goes to
  `_SsSeqGetEof` (`0x80098A94`); the type arrives as the third argument and
  is never read, so every such event is read as a tempo. It takes three
  bytes at `read_pos`, big-endian (microseconds per quarter note), and sets
  `tempo` = 60000000 / that, truncated (only `_SsInitSoundSeq` (`0x80097C48`)
  rounds). Then the play step, with n = `resolution * tempo` (delta ticks
  per minute) and V = `D_800E9624` (`VBLANK_MINUS`, the sound service's
  ticks per second):
  - if n * 10 < V * 60: `tick_wait` = `tick_step` = V * 600 / n (the
    countdown path of `_SsSeqPlay` (`0x80098998`));
  - otherwise `tick_wait` = -1 and `tick_step` = n * 10 / (V * 60), plus 1
    when V * 30 < (n * 10) % (V * 60), that is, rounded to nearest: the
    delta time one tick takes, in tenths of a delta tick.

* **`_SsNoteOn` (`0x80097A48`)**, `(seq_no, sep_no, note, vol)`
  (`midinote.o`). The note event (status 0x90). `vol != 0`: unless bit `ch`
  of `mute_mask` is set, `_SsVmKeyOn` (`0x8009AA78`)`(seq_no | sep_no << 8,
  vab_id, programs[ch], note, vol, panpot[ch])`. `vol == 0` is a note off:
  `_SsVmKeyOff` (`0x8009AF94`)`(seq_no | sep_no << 8, vab_id, programs[ch],
  note)`, whether the channel is muted or not. It is the one event handler
  that does not read the next delta time.

* **`_SsSetProgramChange` (`0x80097B28`)**, `(seq_no, sep_no, prog)`
  (`midiprog.o`). Status 0xC0: `programs[ch]` = `prog`.

* **`_SsSetPitchBend` (`0x80097598`)**, `(seq_no, sep_no)` (`midibend.o`).
  Status 0xE0: takes one data byte from `read_pos` and calls
  `_SsVmPitchBend` (`0x8009C574`)`(seq_no | sep_no << 8, vab_id,
  programs[ch], data)`, the per-voice loop over `_SsVmPBVoice`
  (`0x8009C388`).

* **`_SsSetControlChange` (`0x80097648`)**, `(seq_no, sep_no, control)`
  (`midicc.o`). Status 0xB0, the `control[CC_NUMBER]` slot. It takes the
  value byte from `read_pos` and hands it, by controller number, to one of
  the handlers below. Controller 0, the bank change, is done in line
  (`vab_id` = the value), and a controller it does not know is dropped; in
  both cases it reads the next delta time itself. The table of controllers
  is in [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md), "libsnd — the
  control-change event (`midicc.o`)".

The control-change handlers take `(seq_no, sep_no, data)`:

* **`_SsContBankChange` (`0x80095DC8`)**, `(seq_no, sep_no)` (`cc_0.o`). The
  `control[CC_BANKCHANGE]` slot: `vab_id` = the next byte at `read_pos`.
  `_SsSetControlChange` does controller 0 in line and never calls this slot,
  so it never runs.

* **`_SsContMainVol` (`0x800961C8`)** (`cc_7.o`). Control change 7, the
  channel volume: `_SsVmSetVol` (`0x8009CFB8`)`(seq_no | sep_no << 8,
  vab_id, programs[ch], data, panpot[ch])`, then `vol[ch]` = data.

* **`_SsContPanpot` (`0x80096298`)** (`cc_10.o`). Control change 10, the
  pan: `_SsVmSetVol(seq_no | sep_no << 8, vab_id, programs[ch], vol[ch],
  data)`, then `panpot[ch]` = data.

* **`_SsContExpression` (`0x80096368`)** (`cc_11.o`). Control change 11:
  `_SsVmSetProgVol` (`0x8009C658`)`(vab_id, programs[ch], data)` (it writes
  `ProgAtr.mvol` of that program in the VAB), then `_SsVmSetVol(seq_no |
  sep_no << 8, vab_id, programs[ch], vol[ch], panpot[ch])`. The data byte is
  not stored in the score.

* **`_SsContDamper` (`0x80096458`)** (`cc_64.o`). Control change 64, the
  sustain pedal: data < 0x40 calls `_SsVmDamperOff` (`0x8009A238`), anything
  else `_SsVmDamperOn` (`0x8009A248`) ([psyq/libetc-libsnd.md](psyq/libetc-libsnd.md),
  "libsnd — the damper (`vm_doff.o`, `vm_don.o`)").

* **`_SsContExternal` (`0x80096508`)** (`cc_91.o`). Control change 91, the
  reverb depth: `SsUtSetReverbDepth` (`0x80099EA8`)`(data, data)`.

* **`_SsContNrpn1` (`0x80096598`)** (`cc_98.o`). Control change 98, the
  NRPN's low byte. When `nrpn_msb` is 40 (a mark), it calls
  `D_800E96A8` (`_SsMarkCallback`)`[seq_no][sep_no]`, when that is not 0,
  with `(seq_no, sep_no, data)`. Nothing in the image sets an entry: `_SsInit`
  (`0x800959D8`) zeroes the table and libsnd.h's `SsSetMarkCallback` has no
  row in the register, so no callback runs in this game. Unless `nrpn_msb` is
  20, 30 or 40: `nrpn_lsb` = data, `loop_armed` = 0, `nrpn_count` += 1.

* **`_SsContNrpn2` (`0x80096698`)** (`cc_99.o`). Control change 99, the
  NRPN's high byte, and the NRPN loop (§"The NRPN loop").

* **`_SsContRpn1` (`0x800967D8`)** (`cc_100.o`). Control change 100:
  `rpn_lsb` = data; `rpn_count` += 1.

* **`_SsContRpn2` (`0x80096848`)** (`cc_101.o`). Control change 101:
  `rpn_msb` = data; `rpn_count` += 1.

* **`_SsContResetAll` (`0x800968B8`)**, `(seq_no, sep_no)` (`cc_121.o`).
  Control change 121: `SsUtReverbOff` (`0x8009A028`), `_SsVmDamperOff`, then
  for the current channel: `programs[ch]` = ch, `rpn_lsb` = `rpn_msb` = 0,
  `vol[ch]` = 0x7F, `panpot[ch]` = 0x40. It takes no data argument.

* **`_SsContDataEntry` (`0x80095E48`)** (`cc_6.o`). Control change 6, the
  data entry. It first reads the channel's program header with
  `SsUtGetProgAtr` (`0x80099B18`)`(vab_id, programs[ch], &progAtr)` and then
  takes exactly one of four paths:
  1. **Loop repeat count.** `loop_armed == 1` and `loop_on == 0`:
     `loop_count` = data, `loop_armed` = 0, `loop_on` = 1.
  2. **RPN.** `rpn_count == 2`. Only when `rpn_msb == 0`: for every tone i
     below `progAtr.tones`, `SsUtGetVagAtr` (`0x80099C28`)`(vab_id,
     programs[ch], i, &vagAtr)`, a switch on `rpn_lsb`, and `SsUtSetVagAtr`
     (`0x8009A068`) with the same arguments. RPN 0 (bend range) sets
     `pbmin` = `pbmax` = data & 0x7F. RPN 1 (fine tune) and RPN 2 (coarse
     tune) change nothing: they add a 16-bit value whose low byte is always
     0 to the one-byte `shift` / `center`, so the field keeps its value; the
     tone is still read and written back. The code keeps a comparison of
     `data` with 0x40 in each of the two arms whose result it never uses.
     Then `rpn_count` = 0.
  3. **NRPN.** `nrpn_count == 2`: calls `D_800E9588.ccentry[nrpn_lsb]` with
     `(vab_id, programs[ch], tone, vagAtr by value, nrpn_lsb, data)`, where
     `tone` is `nrpn_msb`, or 0 when `nrpn_msb` is 16. One call is made
     either way. Nothing fills `vagAtr` on this path: the attribute handler
     receives **uninitialised stack**. Then `nrpn_count` = 0.
  4. Otherwise nothing.

### The NRPN loop

A SEQ marks a loop with NRPN messages. Control change 99 with the value 20
marks the loop start and 30 the loop end; the data entry (control change 6,
`_SsContDataEntry` (`0x80095E48`)) that follows the start gives the repeat
count, and 127 or more repeats for ever. Control change 98 after a value of
20, 30 or 40 does not count as an NRPN byte (`_SsContNrpn1` (`0x80096598`)).

**`_SsContNrpn2` (`0x80096698`)**, `(seq_no, sep_no, data)` (`cc_99.o`). The
`control[CC_NRPN2]` slot. Every case stores `data` in `nrpn_msb` and reads
the next delta time with `_SsReadDeltaValue` (`0x80097B98`).

* **data 0x14 (20), loop start.** `loop_armed` = 1; `delta_value` = the
  delta read; then `loop_pos` = `read_pos`, the position of the loop body's
  first event.
* **data 0x1E (30), loop end**, by `loop_count`:
  * 0: `loop_on` = 0; `delta_value` = the delta read.
  * 1 to 126: `loop_count -= 1`; `delta_value` = the delta read; if the
    count is still nonzero `read_pos` = `loop_pos` (jump back), else
    `loop_on` = 0.
  * 127 or more (for ever): the delta is read and dropped, `delta_value` =
    0, `read_pos` = `loop_pos`.
* **any other data.** `nrpn_count` += 1; `delta_value` = the delta read.
  This value is the tone number `_SsContDataEntry` later hands to the
  attribute handlers.

`nrpn_count` is not incremented for 20 and 30. `loop_on` is only cleared
here; `_SsContDataEntry` sets it when it takes the count.

### The tone-attribute handlers

One function per attribute of a VAB tone, `_SsSetNrpnVabAttr0` (`0x80096978`)
to `_SsSetNrpnVabAttr19` (`0x80097568`) (`de_0.o` to `de_19.o`). They are the
`ccentry[nrpn_lsb]` slots of `D_800E9588` (+ 0x44) that
`_SsContDataEntry` (`0x80095E48`) calls. One signature, with the `VagAtr`
passed **by value** and used as scratch:

```c
void f(s16 vabId, s16 prog, s16 tone, VagAtr vagatr, s16 fn_idx, u8 data)
```

`fn_idx` (the NRPN number) is never read. Handlers 0 to 14 first fill
`vagatr` with `SsUtGetVagAtr` (`0x80099C28`)`(vabId, prog, tone, &vagatr)`,
change it, and write the tone back with `SsUtSetVagAtr` (`0x8009A068`).
Handlers 15 to 19 do not touch the tone: they set the reverb. No handler
touches a global directly.

| Attr | Handler | Slot (the C's comment) | What `data` does |
|---|---|---|---|
| 0 | `_SsSetNrpnVabAttr0` (`0x80096978`) | `DE_PRIORITY` | `vagatr.prior` (`+0x00`), the priority |
| 1 | `_SsSetNrpnVabAttr1` (`0x80096A08`) | `DE_MODE` | `vagatr.mode` (`+0x01`); then data 0 calls `SsUtReverbOff` (`0x8009A028`), data 4 calls `SsUtReverbOn` (`0x8009A048`), anything else nothing |
| 2 | `_SsSetNrpnVabAttr2` (`0x80096AC8`) | `DE_LIMITL` | `vagatr.min` (`+0x06`), the low note of the key range |
| 3 | `_SsSetNrpnVabAttr3` (`0x80096B58`) | `DE_LIMITH` | `vagatr.max` (`+0x07`), the high note of the key range |
| 4 | `_SsSetNrpnVabAttr4` (`0x80096BE8`) | `DE_ADSR_AR_L` | attack rate, linear: `a_mode` = 0, `ar` = data |
| 5 | `_SsSetNrpnVabAttr5` (`0x80096D98`) | `DE_ADSR_AR_E` | attack rate, exponential: `a_mode` = 1, `ar` = data |
| 6 | `_SsSetNrpnVabAttr6` (`0x80096E58`) | `DE_ADSR_DR` | decay rate: `dr` = data |
| 7 | `_SsSetNrpnVabAttr7` (`0x80096F08`) | `DE_ADSR_SL` | sustain level: `sl` = data |
| 8 | `_SsSetNrpnVabAttr8` (`0x80096FB8`) | `DE_ADSR_SR_L` | sustain rate, linear: `s_mode` = 0, `sr` = data |
| 9 | `_SsSetNrpnVabAttr9` (`0x80097068`) | `DE_ADSR_SR_E` | sustain rate, exponential: `s_mode` = 1, `sr` = data |
| 10 | `_SsSetNrpnVabAttr10` (`0x80097128`) | `DE_ADSR_RR_L` | release rate, linear: `r_mode` = 0, `rr` = data |
| 11 | `_SsSetNrpnVabAttr11` (`0x800971D8`) | `DE_ADSR_RR_E` | release rate, exponential: `r_mode` = 1, `rr` = data (no split first) |
| 12 | `_SsSetNrpnVabAttr12` (`0x80097288`) | `DE_ADSR_SR` | sustain direction: data 1 to 0x3F sets `s_dir` = 0, 0x40 to 0x7F sets 1, anything else leaves it (no split first) |
| 13 | `_SsSetNrpnVabAttr13` (`0x80097368`) | `DE_VIB_TIME` | `vagatr.vibT` (`+0x09`), the vibrato time |
| 14 | `_SsSetNrpnVabAttr14` (`0x80097408`) | `DE_PORTA_DEPTH` | `vagatr.porW` (`+0x0A`), the portamento depth |
| 15 | `_SsSetNrpnVabAttr15` (`0x800974A8`) | `DE_REV_TYPE` | `SsUtSetReverbType` (`0x80099F38`)`(data)` |
| 16 | `_SsSetNrpnVabAttr16` (`0x800974D8`) | `DE_REV_DEPTH` | `SsUtSetReverbDepth` (`0x80099EA8`)`(data, data)` |
| 17 | `_SsSetNrpnVabAttr17` (`0x80097508`) | `DE_ECHO_FB` | `SsUtSetReverbFeedback` (`0x80099FE8`)`(data)` |
| 18 | `_SsSetNrpnVabAttr18` (`0x80097538`) | `DE_ECHO_DELAY` | `SsUtSetReverbDelay` (`0x80099E68`)`(data)` |
| 19 | `_SsSetNrpnVabAttr19` (`0x80097568`) | `DE_DELAY` | `SsUtSetReverbDelay(data)`, the same call as 18 |

The ADSR handlers go through a local `SsAdsr` (`include/psyq_private.h`):
`_SsUtResolveADSR` (`0x80096C98`)`(vagatr.adsr1, vagatr.adsr2, &adsr)`
(`ccadsr.o`) splits the tone's two ADSR words (`VagAtr` `+0x10` / `+0x12`),
the handler sets one or two members, and `_SsUtBuildADSR`
(`0x80096CF4`)`(&adsr, &vagatr.adsr1, &vagatr.adsr2)` packs them back:

| Member | Type | Meaning | Source bits |
|---|---|---|---|
| `ar` | `u16` | attack rate | `adsr1` bits 8-14 |
| `dr` | `u16` | decay rate | `adsr1` bits 4-7 |
| `sl` | `u16` | sustain level | `adsr1` bits 0-3 |
| `sr` | `u16` | sustain rate | `adsr2` bits 6-12 |
| `rr` | `u16` | release rate | `adsr2` bits 0-4 |
| `a_mode` | `s16` | attack mode | `adsr1 & 0x8000` |
| `s_mode` | `s16` | sustain mode | `adsr2 & 0x8000` |
| `r_mode` | `s16` | release mode | `adsr2 & 0x20` |
| `s_dir` | `s16` | sustain direction | `adsr2 & 0x4000` |

Handlers 4 to 10 split the words first. Handlers 11 to 14 call
`_SsUtBuildADSR` without a `_SsUtResolveADSR` call, so the tone's `adsr1`
and `adsr2` are rebuilt from members the handler did not set
([psyq/libetc-libsnd.md](psyq/libetc-libsnd.md), "libsnd — tone attributes 11 and
12 (`de_11.o`, `de_12.o`)").

### Volume ramp, loop count, score volume

* **`_SsSndCrescendo` (`0x800986B8`)**, `(seq, sep)` (`cres.o`). One tick of
  a score's volume ramp. `cres_tick` += 1. If `cres_time < cres_tick` the
  ramp is over and flag 0x10 is cleared. Otherwise d = `cres_vol *
  cres_tick / cres_time` - `cres_done`, the change due by now less the change
  already made; when d is not 0: `cres_done` += d; `_SsVmGetSeqVol`
  (`0x8009CD44`)`(seq | sep << 8, &l, &r)`; d is added to each and the
  results clamped to 0..127; `_SsVmSetSeqVol` (`0x8009C7D8`)`(seq | sep << 8,
  l, r, 1)`; when both are 127 or both are 0, flag 0x10 is cleared. In every
  case it ends with `_SsVmGetSeqVol(seq | sep << 8, &cres_voll, &cres_volr)`.
  `SsSeqCalledTbyT` (`0x80098448`) calls it for flag 0x10 and again for flag
  0x20; the function itself only ever clears 0x10. Nothing in the image sets
  either flag, so it never runs in this game. The fourth argument passed to
  `_SsVmSetSeqVol`, here and in the volume setters, is the SDK's own
  mismatch: the callee takes three.

* **`SsSetLoop` (`0x80095AB8`)**, `(access_num, seq_num, l_count)`
  (`ssloop.o`). On `D_800E9628[access_num][seq_num]`: `l_count` = the
  argument, `l_count_done` = 0. Nothing in the image calls it.

* **`SsIsEos` (`0x80095AF8`)**, `(access_num, seq_num)` (`ssloop.o`).
  Returns `play_mode` of `D_800E9628[access_num][seq_num]`: 1 while the score
  plays, 0 once it has ended, paused or stopped. The game's
  `sound_music_play_check` (`0x80023CC0`) asks it about the score in its
  first sequence slot.

* **`SsSeqSetVol` (`0x800997C8`)**, `(seq_access_num, voll, volr)`
  (`ssvol.o`). On record 0 of `D_800E9628[seq_access_num]`: when the flag
  word is exactly 1, `_SsVmSetSeqVol` (`0x8009C7D8`)`(seq_access_num, voll,
  volr, 1)`, which stores the pair, clamps it and re-voices the sounding
  notes; otherwise it only stores `voll` / `volr`. Callers:
  `sound_seq_play` (`0x80023B24`), which sets the volume before it starts a
  sequence, and `sound_music_volume_set` (`0x80023C4C`), which applies the
  music level.

* **`_SsSndSetVol` (`0x80099738`)**, `(sep_no, seq_no, voll, volr)`
  (`ssvol.o`). The same for `D_800E9628[sep_no][seq_no]`, calling
  `_SsVmSetSeqVol(sep_no | seq_no << 8, voll, volr, 1)`. Nothing in the
  image calls it.

* **`SsSepSetVol` (`0x80099830`)**, `(sep_access_num, seq_no, voll, volr)`
  (`ssvol.o`). The public twin of `_SsSndSetVol`, the same code. Nothing in
  the image calls it.

`SsSeqGetVol` (`0x800998C0`), which reads the pair back, is in
[psyq/libetc-libsnd.md](psyq/libetc-libsnd.md), "libsnd — play, pause, stop and the
volume read-back".

## libsnd — the tick

### The tick setting `D_800B79F0` (`_snd_seq_tick_env`)

`D_800B79F0` (`_snd_seq_tick_env`) is a 0x14-byte record, the type
`SndSeqTickEnv` in `include/psyq_private.h`. `SsSetTickMode` (`0x800995E8`)
chooses the tick, `func_80098138` installs it, the two handlers and `SsEnd`
(`0x80095928`) read it.

| Offset | Type | Name in the C | Meaning |
|---|---|---|---|
| `+0x00` | `s32` | `tick_mode` | `SsSetTickMode`'s mode; 60 in the image's data. |
| `+0x04` | `s32` | `manual_tick` | Nonzero: the program calls the tick itself (`SS_NOTICK`); 1 in the image's data. |
| `+0x08` | `void (*)(void)` | `tick_cb` | The tick function: `SsSeqCalledTbyT` (`0x80098448`) in the image's data. Nothing writes it. |
| `+0x0C` | `void (*)(void)` | `vsync_cb` | The interrupt-0 handler `func_80098138` displaced; `func_800983A8` calls it first, and `SsEnd` puts it back. |
| `+0x10` | `s8` | `vsync_tick` | 1 while the tick rides `VSyncCallback` (`0x80094C68`). |
| `+0x11` | `s8` | `half_tick` | 1 while the half-rate handler `func_800983F4` is installed (a rate below 70 Hz). |
| `+0x12` | `s8` | `alarm_tick` | The interrupt the tick hangs on: 6 root counter 2, 0 VSync, 127 none. `SsEnd` tests it and writes 127. |
| `+0x13` | `s8` | `pad_13` | Nothing reads or writes it. |

The word after the record, `D_800B7A04`, is not part of it: it is
`ssstart.o`'s own variable, the half-rate handler's phase.
[psyq/libetc-libsnd.md](psyq/libetc-libsnd.md), "libsnd — the tick setting
`_snd_seq_tick_env` (`ssstart.o`)", shows why, and describes that handler,
`func_800983F4`.

### `SsSetTickMode` (`0x800995E8`), `(tick_mode)` (`sstick.o`)

Calls `GetVideoMode` (`0x8009573C`) (0 NTSC, 1 PAL). If `tick_mode & 0x1000`
(`SS_NOTICK`): `manual_tick` = 1 and `tick_mode` = `tick_mode & 0xFFF`;
otherwise `manual_tick` = 0 and the mode is stored whole. Then `D_800E9624`
(`VBLANK_MINUS`) is set from the stored mode:

| Stored mode | `D_800E9624` | `tick_mode` rewritten to |
|---|---|---|
| 4 (`SS_TICK50`) | 50 | 5 on PAL, else 50 |
| 1 (`SS_TICK60`) | 60 | 5 on NTSC, else 60 |
| 3 (`SS_TICK120`) | 120 | unchanged |
| 2 (`SS_TICK240`) | 240 | unchanged |
| 5 (`SS_TICKVSYNC`), 0 (`SS_NOTICK0`) | 50 if PAL, else 60 | unchanged |
| 6 and up | the mode value itself | unchanged |
| negative | 60 | unchanged |

The switch goes through a jump table at `0x800138B4` (cases 0 to 5). The
game's `sound_init` (`0x80023D54`) calls `SsSetTickMode(1)`, so on an NTSC
console the stored mode becomes 5.

### `func_80098138`, `(use_vsync)` (`ssstart.o`)

Installs the tick according to the record above. It is a static of
`ssstart.o`. `SsStart` (`0x80098368`) calls it with 1, `SsStart2`
(`0x80098388`) with 0.

1. A busy wait of 1000 turns.
2. Reset: `vsync_tick` = 0, `alarm_tick` = 6, `half_tick` = 0, `vsync_cb` =
   0; the counter spec is 0xF2000002 (root counter 2) and its target 17640
   (4233600 / 240).
3. By `tick_mode`:
   * 0 (`SS_NOTICK0`): `alarm_tick` = 127 and return with nothing installed.
   * 5 (`SS_TICKVSYNC`): `alarm_tick` = 0. With `use_vsync == 0`,
     `vsync_tick` = 1; otherwise spec 0xF2000003 (root counter 3), target 1.
   * 3 (`SS_TICK120`): target 35280 (4233600 / 120). 2 (`SS_TICK240`):
     target stays 17640.
   * any other value, a rate in Hz: return if `manual_tick` is nonzero.
     Below 70 Hz the target is 2116800 / rate and `half_tick` is set: the
     counter runs at twice the rate and the half-rate handler is installed
     (4233600 / rate would not fit the 16-bit target below 65 Hz); otherwise
     4233600 / rate.
4. Install, inside `EnterCriticalSection` (`0x8008B3B8`) /
   `ExitCriticalSection` (`0x8008B3C8`): with `vsync_tick` set,
   `VSyncCallback` (`0x80094C68`)`(tick_cb)`; otherwise `ResetRCnt`
   (`0x8008F3A0`)`(spec)`, `SetRCnt` (`0x8008F268`)`(spec, target, 0x1000)`
   and `InterruptCallback` (`0x80094C08`) on `alarm_tick`: with
   `func_800983A8` when `alarm_tick` is 0 (after saving the displaced
   handler in `vsync_cb`), with `tick_cb` when `half_tick` is 0, and with
   the half-rate handler `func_800983F4` otherwise.

The game's `sound_init` (`0x80023D54`) calls `SsStart2`. On an NTSC console
the mode is 5, so it takes the `VSyncCallback(tick_cb)` path:
`SsSeqCalledTbyT` (`0x80098448`) runs once per vertical blank.

### `func_800983A8` (`ssstart.o`), `()`

The handler `func_80098138` installs on interrupt 0 in `SS_TICKVSYNC` mode
when it is called through `SsStart` (`0x80098368`): `if (env.vsync_cb !=
0) env.vsync_cb(); env.tick_cb();`. The game never calls `SsStart`, so it
is not on this game's path. The half-rate handler, `func_800983F4`, runs
`tick_cb` on every second call.

### `SsSeqCalledTbyT` (`0x80098448`), `()` (`sscall.o`)

The sequencer tick, the function `tick_cb` points at.

1. If `D_800E961C` (`_snd_ev_flag`) is 1, return (a re-entry guard);
   otherwise set it to 1.
2. `_SsVmFlush` (`0x8009A258`)`()`: the voice manager's pending register
   writes.
3. For each access number `i` below `D_800E9EA8` (`_snd_seq_s_max`) whose bit
   is set in `D_800E9620` (`_snd_openflag`), and each `j` below `D_800E9EAA`
   (`_snd_seq_t_max`), serve `D_800E9628[i][j].flags`:
   * bit 1 (playing): `_SsSndPlay` (`0x80098968`)`(i, j)`; then 0x10 and
     again 0x20 call `_SsSndCrescendo` (`0x800986B8`), 0x40 and again 0x80
     call `_SsSndTempo` (`0x800998F8`);
   * then, playing or not: 2 calls `_SsSndPause` (`0x800988C8`), 8 calls
     `_SsSndReplay` (`0x80099188`), 4 calls `_SsSndStop` (`0x800991E8`) and
     then clears the whole flag word.
4. `D_800E961C` = 0.

What the served functions do:

| Function | What it does |
|---|---|
| `_SsSndReplay` (`0x80099188`) | `play_mode` = 1, flag 8 cleared. |
| `_SsSndStop` (`0x800991E8`) | Flags 1, 2, 8 and 0x400 cleared and 4 set; `_SsVmSeqKeyOff(seq \| sep << 8)` and `_SsVmDamperOff` (`0x8009A238`). Then the reader's state goes back to what `_SsInitSoundSeq` (`0x80097C48`) left: `play_mode`, `delta_sum`, the RPN and NRPN bytes and counters, `channel`, `l_count_done`, the loop bytes and `running_status` = 0; `delta_value` = `start_delta`, `tempo` = `hdr_tempo`, `tick_step` = `hdr_tick_step`, `read_pos` = `loop_pos` = `start_pos`; every channel's program = its number, pan = 64, volume = 127; `cres_voll` = `cres_volr` = 127. `SsSeqStop` (`0x80099368`) and `SsSepStop` (`0x80099390`) call it directly. |

`_SsSndPlay` (`0x80098968`), the step for a playing score, and `_SsSndPause`
(`0x800988C8`), the answer to flag 2, are in
[psyq/libetc-libsnd.md](psyq/libetc-libsnd.md), "libsnd — play, pause, stop and the
volume read-back", with `Snd_SetPlayMode` (`0x80098018`), which starts a
score. The common variables the tick reads (`D_800E961C`, `D_800E9620`,
`D_800E9624`, `D_800E9EA8`, `D_800E9EAA`) are in the same page's table of
`ssinit.o`'s data. `D_800E9624` (`VBLANK_MINUS`) is `u32` in the files that
read it (`libsnd_seqinit.c`, `libsnd_midimeta.c`, `libsnd_tempo.c`) and `s32`
in the two that only write it (`libsnd_ssinit.c`, `libsnd_sstick.c`).

### `SsSetTableSize` (`0x800993C8`), `(table, s_max, t_max)` (`sstable.o`)

Hands the library its score table. One caller, `sound_init` (`0x80023D54`),
with 16 rows of 1 record.

1. Stores `s_max` in `D_800E9EA8` (`_snd_seq_s_max`) and `t_max` in
   `D_800E9EAA` (`_snd_seq_t_max`).
2. For `i` below `s_max`: `D_800E9628[i]` = `table + i * t_max * 0xB0`.
3. For `i` from `s_max` to 31: sets bit `i` of `D_800E9620`
   (`_snd_openflag`), so those access numbers are never handed out:
   `SsSeqOpen` (`0x80095B38`) takes the lowest clear bit.
4. For every record, in this order: `flags` = 0; `next_seq` = -1;
   `next_sep` = 0; `cres_vol`, `cres_done`, `cres_time`, `cres_tick` = 0;
   `unk_4c` = 0; `tempo_goal` = 0; `tempo_count` = 0; `unk_a4` = 0;
   `tempo_step` = 0; `voll`, `volr`, `cres_voll`, `cres_volr` = 127.

### `_SsSndTempo` (`0x800998F8`), `(seq_no, sep_no)` (`tempo.o`)

One tick of a tempo ramp; `SsSeqCalledTbyT` (`0x80098448`) calls it for flag
0x40 and again for 0x80. Nothing in the image sets either flag, so it never
runs in this game.

1. `tempo_count` (signed) is decremented. If it is now below zero, flags
   0x40 and 0x80 are cleared and it returns.
2. `tempo_step` > 0: when `tempo_count % tempo_step` is nonzero it returns
   at once. Otherwise `tempo` moves one toward `tempo_goal`.
3. `tempo_step` <= 0: `tempo` moves by `-tempo_step` toward the goal and is
   clamped to it. The step is the negated size.
4. After a move: `tick_step` = `resolution * tempo * 10 / (VBLANK_MINUS *
   60)`, an unsigned division, forced to 1 when not positive. If
   `tempo_count == 0` or `tempo == tempo_goal`, flags 0x40 and 0x80 are
   cleared.

Tempo and goal compare unsigned; the count is signed.

## libsnd — the voice manager

The voice manager turns note events into SPU register writes. It works in
three stages. A note's parameters are gathered in one record, `D_800EA6F0`
(`_svm_cur`). A voice is chosen, and its record in `D_800E9EC0`
(`_svm_voice`) and its shadow registers are filled. Once per sound tick
`_SsVmFlush` (`0x8009A258`) sends the shadow registers and the pending
key-on, key-off, reverb and noise masks to libspu. Its members are
`vm_init.o`, `vm_f.o`, `vm_key.o`, `vm_aloc1.o`, `vm_aloc2.o`, `vm_nowon.o`,
`vm_nowof.o`, `vm_no1.o`, `vm_noff.o`, `vm_n2p.o`, `vm_vol.o`, `vm_seq.o`,
`vm_prog.o`, `vm_pb.o`, `vm_stav.o`, `vm_vsu.o`, `vm_don.o` and `vm_doff.o`;
the VAB side adds `vs_vh.o`, `vs_vh2.o`, `vs_vtb.o`, `vs_vtc.o`, `vs_vab.o`,
`vs_mono.o`, `ut_gva.o` and `ut_sva.o`.

### `D_800EA6F0` (`_svm_cur`) — the note being set up

The record is 0x1A bytes, the size of Sony's object (`SvmCur`).

| Offset | Type | Field | Meaning |
|---|---|---|---|
| `+0x00` | `s8` | `tones` | The program's tone count (`ProgAtr.tones`). `_SsVmSelectToneAndVag` (`0x8009CEE8`) reads it signed. |
| `+0x01` | `s8` | `vab_id` | The current VAB (`_SsVmVSetUp` (`0x8009D558`)). |
| `+0x02` | `s8` | `note` | The note number. |
| `+0x03` | `s8` | `unk_03` | `_SsVmKeyOn` (`0x8009AA78`) writes 0; nothing reads it. |
| `+0x04` | `s8` | `voll` | The note's volume. Key-on copies it to the voice's `note_vol`. |
| `+0x05` | `s8` | `note_pan` | The note's pan. |
| `+0x06` | `s8` | `prog` | The program number (`_SsVmVSetUp`). |
| `+0x07` | `s8` | `fake_prog` | The program's index among the VAB's programs that have tones (the low byte of `ProgAtr.reserved1`). A tone record is `D_800EA6E0[fake_prog * 16 + tone]`. `SsUtGetVagAtr` (`0x80099C28`) and `SsUtSetVagAtr` (`0x8009A068`) reach it as `D_800EA6F7`. |
| `+0x08` | `u8[2]` | `pad_08` | Nothing reads or writes these two bytes. |
| `+0x0A` | `s8` | `mvol` | The program's master volume. |
| `+0x0B` | `s8` | `mpan` | The program's pan. |
| `+0x0C` | `s8` | `tone` | The tone within the program. |
| `+0x0D` | `s8` | `vol` | The tone's volume. |
| `+0x0E` | `s8` | `pan` | The tone's pan. |
| `+0x0F` | `s8` | `prior` | The tone's priority, which `_SsVmAlloc` (`0x8009B228`) compares. |
| `+0x10` | `s8` | `centre` | The tone's centre note. |
| `+0x11` | `u8` | `shift` | The tone's fine tune. `note2pitch` (`0x8009B668`) holds it to 127. |
| `+0x12` | `s8` | `mode` | The tone's mode; bit 2 = reverb on. |
| `+0x13` | `u8` | `pad_13` | Nothing reads or writes it. |
| `+0x14` | `s16` | `seq_sep` | The score playing the note, access number \| sequence << 8; 0x21 = none, a direct key-on. |
| `+0x16` | `s16` | `vag` | The tone's VAG number, 1-based; 0xFF = a noise tone. `_SsVmDoAllocate` (`0x8009B498`) finds the VAG's address by it. |
| `+0x18` | `s16` | `voice` | The SPU voice being set up. `_SsVmInit` (`0x8009A728`) and `_SsVmKeyOff` (`0x8009AF94`) write it as `D_800EA708`. |

### `D_800E9EC0` (`_svm_voice`) — 24 records of 0x38 bytes

| Offset | Type | Field | Meaning |
|---|---|---|---|
| `+0x00` | `s16` | `vag` | The tone's VAG number, stored at key-on; 0xFF marks a noise voice. `_SsVmInit` (`0x8009A728`) sets 0xFF, a key-off 0. |
| `+0x02` | `s16` | `age` | +1 on every unmasked voice at each allocation; 0 on the voice allocated; 24 after `_SsVmInit`. |
| `+0x04` | `s16` | `pitch` | The SPU pitch at key-on (`_SsVmKeyOnNow` (`0x8009BEB8`)); 10 on a noise voice (`vmNoiseOn` (`0x8009B848`)); 0 at key-off. Nothing reads it. |
| `+0x06` | `u16` | `envx` | The SPU envelope level read at the last flush (0 = silent); 0x7FFF from allocation until then. |
| `+0x08` | `s16` | `vol` | The volume argument of `_SsVmKeyOn` (`0x8009AA78`) (not written for a direct key-on). |
| `+0x0A` | `u8` | `pan` | The note's pan. |
| `+0x0B` | `u8` | `pad_0b` | Nothing touches it. |
| `+0x0C` | `s16` | `channel` | The score's MIDI channel at key-on (not written for a direct key-on). |
| `+0x0E` | `s16` | `note` | The note the voice was keyed on with. |
| `+0x10` | `s16` | `seq_sep` | The owning score. |
| `+0x12` | `s16` | `prog` | `D_800EA6F0.fake_prog` at key-on: the tone-table row. |
| `+0x14` | `s16` | `pg` | The program number: the index into `D_800EA6D4` (`_svm_pg`). |
| `+0x16` | `s16` | `tone` | The tone within the program. |
| `+0x18` | `s16` | `vab_id` | The VAB. |
| `+0x1A` | `s16` | `prior` | The priority. |
| `+0x1C` | `u8` | `pad_1c` | Nothing touches it. |
| `+0x1D` | `s8` | `key_stat` | Key state: 0 idle, 1 keyed on, 2 sounding as the noise voice. |
| `+0x1E` | `s16` | `auto_vol` | Nonzero: `_SsVmFlush` (`0x8009A258`) calls the function at `D_800EA5A0` (`_autovol`) with the voice. |
| `+0x20` to `+0x26` | `s16` | `unk_20`, `unk_22`, `unk_24`, `unk_26` | `_SsVmInit` writes 0; nothing reads them. |
| `+0x28` | `s16` | `pad_28` | Nothing touches it. |
| `+0x2A` | `s16` | `auto_pan` | Nonzero: `_SsVmFlush` calls the function at `D_800EA418` (`_autopan`) with the voice. |
| `+0x2C` to `+0x32` | `s16` | `unk_2c`, `unk_2e`, `unk_30`, `unk_32` | `_SsVmInit` writes 0; nothing reads them. |
| `+0x34` | `s16` | `pad_34` | Nothing touches it. |
| `+0x36` | `s16` | `note_vol` | `D_800EA6F0.voll` at key-on. Nothing reads it. |

`prog` and `pg` are easy to swap. `prog` is the tone-table row; `pg` is the
program number that `_SsVmKeyOff`, `_SsVmSetVol` (`0x8009CFB8`) and
`_SsVmPBVoice` (`0x8009C388`) compare with their `prog` argument.

The automatic volume and pan never run in this program. The C writes
`auto_vol` and `auto_pan` only to clear them (`_SsVmAlloc` (`0x8009B228`)
and `_SsVmInit`), and nothing in the C writes `D_800EA5A0` or `D_800EA418`,
which lie in .bss.

### Shadow registers and pending masks

`D_800EA420` (`_svm_sreg_buf`) holds eight `u16` per voice: slot n of voice
v is `D_800EA420[v * 8 + n]`. `D_800EA400` (`_svm_sreg_dirty`) holds one
byte per voice saying which slots the next flush must send.

| Slot n | Content | Dirty bit the flush tests | Written by |
|---|---|---|---|
| 0, 1 | volume left, right | 0x01 (the writers set 0x03) | `_SsVmKeyOnNow` (`0x8009BEB8`), `vmNoiseOn` (`0x8009B848`), `_SsVmSetVol` (`0x8009CFB8`), `_SsVmSetSeqVol` (`0x8009C7D8`) |
| 2 | pitch | 0x04 | `_SsVmKeyOnNow`, `_SsVmPBVoice` (`0x8009C388`) |
| 3 | sample start address, kept divided by 8 | 0x08 | `_SsVmDoAllocate` (`0x8009B498`) |
| 4, 5 | ADSR1, ADSR2 | 0x10 (the allocator sets 0x30) | `_SsVmDoAllocate` |

Nothing writes slots 6 and 7 except `_SsVmInit` (`0x8009A728`), which
clears all 192 words. The four pending masks (key-on, key-off, reverb,
noise) are pairs of `u16`: the first word holds voices 0-15, the second
voices 16-23 (§"Globals").

### Globals

| Symbol | The C's comment | Type | Role |
|---|---|---|---|
| `D_800E9628` | `_ss_score` | `SsScore *[]` | The score records. The voice manager reads a score's `vol[channel]`, `voll` and `volr`. |
| `D_800EA6F0` | `_svm_cur` | `SvmCur` | The note being set up. |
| `D_800E9EC0` | `_svm_voice[24]` | `SvmVoice[24]` | Per-voice state. |
| `D_800EA6E4` | `_SsVmMaxVoice` | `s8` | Number of voices the manager runs. |
| `D_800B79C0` | `_snd_vmask` | `u32` | One bit per voice. A set bit makes the allocator, the key-off loops, the volume loops and `vmNoiseOn`'s key-state pass skip the voice. Nothing in the C writes it. |
| `D_800EA6DC` | `_svm_vh` | `VabHdr *` | The current VAB's header. |
| `D_800EA6D4` | `_svm_pg` | `ProgAtr *` | The current VAB's program table. |
| `D_800EA6E0` | `_svm_tn` | `VagAtr *` | The current VAB's tone table. |
| `D_800EA648` | `_svm_vab_vh[16]` | `VabHdr *[16]` | Header per VAB slot. |
| `D_800EA608` | `_svm_vab_pg[16]` | `ProgAtr *[16]` | Program table per slot. |
| `D_800EA690` | `_svm_vab_tn[16]` | `VagAtr *[16]` | Tone table per slot. |
| `D_800EA710` | `_svm_vab_used[16]` | `u8[16]` | Slot state: 0 free; 2 header opened, body not sent; 1 open and usable. |
| `D_800EA770` | `_svm_vab_start[16]` | `s32[16]` | SPU address of the slot's sample block. |
| `D_800EA728` | `_svm_vab_total` (in the comment on `_SsVabOpenHeadWithMode` (`0x8009D7F8`)) | `s32[16]` | Byte size of the slot's samples (unrounded). |
| `D_800EA768` | `_svm_vab_count` | `u16` | Slots in use. |
| `D_800EA6E8` | `_svm_vab_not_send_size` | `s32` | Zeroed by `_SsVmInit` (`0x8009A728`) and when a header is opened; nothing reads it. |
| `D_800EA6D2` | `kMaxPrograms` | `s16` | 128 or 64, set each time a header is opened; 128 after `_SsVmInit`. |
| `D_800EA6D0` | `_svm_stereo_mono` | `s16` | 1 = mono (`SsSetMono` (`0x8009D618`)), 0 = stereo (`SsSetStereo` (`0x8009D628`), `_SsVmInit`). |
| `D_800EA688` | `_svm_damper` | `s16` | Added to a voice's release rate at allocation: 2 after `_SsVmDamperOn` (`0x8009A248`), 0 after `_SsVmDamperOff` (`0x8009A238`) and `_SsVmInit`. |
| `D_800EA400` | `_svm_sreg_dirty[24]` | `u8[24]` | Dirty bits per voice. |
| `D_800EA420` | `_svm_sreg_buf[24 * 8]` | `u16[192]` | Shadow registers. |
| `D_800E9EB0`, `D_800E9EB2` | `_svm_okon1`, `_svm_okon2` | `u16` | Voices to key on. |
| `D_800EA5E8`, `D_800EA5EA` | `_svm_okof1`, `_svm_okof2` | `u16` | Voices to key off. |
| `D_800E9EB4`, `D_800E9EB6` | `_svm_orev1`, `_svm_orev2` | `u16` | Voices with reverb on. |
| `D_800E9EB8`, `D_800E9EBA` | `_svm_onos1`, `_svm_onos2` | `u16` | Voices with noise on. |
| `D_800EA5A8` | `_svm_envx_hist[16]` | `s32[16]` | Ring of "voice was silent at this flush" masks. |
| `D_800EA5A4` | `_svm_envx_ptr` | `s32` | The ring's index. |
| `D_800EA720` | `_svm_auto_kof_mode` | `s8` | Nonzero switches the flush's automatic key-off off. `_SsVmInit` clears it; nothing sets it. |
| `D_800EA5A0` | `_autovol` | `void (*)(s32)` | Per-voice volume stepper; nothing writes it. |
| `D_800EA418` | `_autopan` | `void (*)(s32)` | Per-voice pan stepper; nothing writes it. |
| `D_800EA5F0` | `_svm_rattr` | `SpuReverbAttr` | The reverb attributes libsnd hands to `SpuSetReverbModeParam` (`0x8009F618`). |
| `D_800EA7B8` | `_ss_spu_vm_rec` | `u8[]` | `vm_init.o`'s own .bss + 8: libspu's allocation table for 32 blocks. |

Two files declare some of these differently. `libsnd_vm_key_1.c` declares
`D_800EA6D4` and `D_800EA6DC` as `s32` and `D_800E9628` as `s32[]`, and
casts at each use. `D_800EA688` is `s16` in `libsnd_vm_aloc2.c`, the one
file that reads it, and `u16` in the files that only store it.

### Reset

**`_SsVmInit` (`0x8009A728`)**, `(maxVoice)`; `vm_init.o`. Resets the voice
manager. Its one caller is `_SsInit` (`0x800959D8`), with 24.

1. `_spu_setInTransfer(0)` (`0x800A0548`); `D_800EA688` (`_svm_damper`) = 0;
   `SpuInitMalloc(32, D_800EA7B8)` (`0x8009E9E8`).
2. Clears the 192 words of `D_800EA420` (`_svm_sreg_buf`), the 24 bytes of
   `D_800EA400` (`_svm_sreg_dirty`), `D_800EA768` (`_svm_vab_count`) and the
   16 bytes of `D_800EA710` (`_svm_vab_used`).
3. `D_800EA6E4` (`_SsVmMaxVoice`) = `maxVoice`, or 24 when it is not in 0
   to 23 (the test is unsigned on the sign-extended char, so a negative
   count gives 24).
4. For each voice below `D_800EA6E4`:
   * resets `D_800E9EC0[i]`, in this store order: `age` = 24; `vag` = 0xFF;
     `key_stat` = 0; `pitch` = 0; `envx` = 0; `seq_sep` = -1; `prog` = 0;
     `pg` = 0; `tone` = 0xFF; `vol` = 0; `channel` = 0; `pan` = 0x40;
     `note_vol` = 0; `auto_vol` = 0; `unk_20`, `unk_22`, `unk_24` = 0;
     `auto_pan` = 0; `unk_2c`, `unk_2e`, `unk_30`, `unk_32` = 0; `unk_26` =
     0. Not written: `note`, `vab_id`, `prior`, `pad_0b`, `pad_1c`,
     `pad_28`, `pad_34`.
   * `SpuSetVoiceAttr(&attr)` (`0x800A0588`): voice `1 << i`, mask 0x60093
     (both volumes, pitch, start address, both ADSR words), volume 0 / 0,
     pitch 0x1000, address 0x1000, `adsr1` 0x80FF, `adsr2` 0x4000.
   * `D_800EA6F0.voice` = i (written as `D_800EA708`), then
     `_SsVmKeyOffNow(1)` (`0x8009BDF8`), which does not read its argument.
5. `D_800EA5F0` (`_svm_rattr`, a `SpuReverbAttr`): mask = 0, depth left =
   right = 0x3FFF, mode = 0.
6. Clears the key-on words (`D_800E9EB0`, `D_800E9EB2`), the first key-off
   word (`D_800EA5E8`), the reverb and noise words, `D_800EA720`
   (`_svm_auto_kof_mode`), `D_800EA6D0` (`_svm_stereo_mono`) and
   `D_800EA6E8` (`_svm_vab_not_send_size`); `D_800EA6D2` (`kMaxPrograms`) =
   0x80; then `_SsVmFlush()` (`0x8009A258`). The second key-off word
   (`D_800EA5EA`) is not cleared here.

### Key-on

**`_SsVmKeyOn` (`0x8009AA78`)**, `(seq_sep, vabId, prog, note, voll, pan)`;
`vm_key.o`. `_SsNoteOn` (`0x80097A48`) calls it for each sequenced note;
`_SsVmSeKeyOn` (`0x8009B0F4`) calls it with `seq_sep` = 0x21 for a direct
key-on. It returns a mask of the voices keyed on, or -1.

1. `_SsVmVSetUp(vabId, prog)` (`0x8009D558`); a failure returns -1.
2. `D_800EA6F0` (`_svm_cur`) takes `seq_sep`, `note`, `unk_03` = 0, the
   volume, the pan (`note_pan`) and, from `D_800EA6D4[prog]` (`_svm_pg`),
   the program's master volume, pan and tone count. The volume is `voll *
   score->vol[score->channel] / 127` for the score `seq_sep` names, or
   `voll` unscaled for a direct key-on.
3. `fake_prog >= D_800EA6DC->ps` (`_svm_vh`) returns -1.
4. `voll == 0` is a note off: the result is `_SsVmKeyOff(seq_sep, vabId,
   prog, note)` (`0x8009AF94`).
5. Otherwise `_SsVmSelectToneAndVag(toneIdx, vagIdx)` (`0x8009CEE8`) fills
   two 0x80-byte stack arrays with the program's tones whose key range
   (`min` to `max`) holds the note and their VAG numbers, and returns the
   count. For each entry, `vag` and `tone` come from the arrays and
   `prior`, `vol`, `pan`, `centre`, `shift` and `mode` from
   `D_800EA6E0[tone + fake_prog * 16]` (`_svm_tn`); then `voice` =
   `_SsVmAlloc(0)` (`0x8009B228`).
   - A voice below `D_800EA6E4`: its record is filled (`key_stat` 1, `age`
     0, `seq_sep`, `vab_id`, `prog` = `fake_prog`, `pg` = `prog`, `vol` =
     the `voll` argument and `channel` = the score's channel unless this is
     a direct key-on, `pan`, `note_vol` = `D_800EA6F0.voll`, `tone`,
     `note`, `prior`, `vag`); `_SsVmDoAllocate()` (`0x8009B498`); then
     `vmNoiseOn(voice)` (`0x8009B848`) when the VAG number is 0xFF, else
     `_SsVmKeyOnNow(count, note2pitch() & 0xFFFF)` (`_SsVmKeyOnNow`
     (`0x8009BEB8`), `note2pitch` (`0x8009B668`)). The result gains bit
     `voice`.
   - No voice: the result becomes -1, and stays -1.

**`_SsVmAlloc` (`0x8009B228`)**, `(arg0)`; `vm_aloc1.o`. Picks the SPU
voice for the note in `D_800EA6F0`. It scans voices 0 to `D_800EA6E4` - 1,
skipping `D_800B79C0` (`_snd_vmask`) bits. A voice with `key_stat` 0 and
`envx` 0 is idle and is taken at once. Otherwise it tracks the candidate of
lowest `prior` not above the note's; among equal priorities the lowest
`envx`, and among those the greatest `age`. With no idle voice and no
candidate it returns `D_800EA6E4`, meaning no voice. On success every
unmasked voice's `age` goes up by one, and the chosen voice gets `age` 0,
the note's priority, and `auto_pan` and `auto_vol` cleared. Its argument is
unused (the caller passes 0); it calls nothing.

**`_SsVmDoAllocate` (`0x8009B498`)**; `vm_aloc2.o`. Sets up the shadow
registers of the voice just allocated; no arguments, no result, no callees.
The voice's `envx` = 0x7FFF. The voice's bit is cleared in all 16 words of
`D_800EA5A8` (`_svm_envx_hist`). Slot 3 = the VAG's address word from the
program table (§"The VAB side"), dirty bit 0x08. Slot 4 = the tone's
`adsr1`. Slot 5 = its `adsr2` with the low five bits (the release rate)
raised by `D_800EA688` (`_svm_damper`) and held at 31; dirty bits 0x30.

**`_SsVmKeyOnNow` (`0x8009BEB8`)**, `(vagCount, pitch)`; `vm_nowon.o`. The
last step for a sampled tone. It computes the volume law below from
`D_800EA6F0`, then: slot 2 = `pitch`, slots 0 and 1 = left and right,
dirty bits 0x07, and the voice's `pitch` = `pitch`. Pending masks, for the
voice's bit: reverb set when the tone's mode has bit 2, cleared otherwise;
noise cleared; key-on set; then every pending key-on is removed from
key-off (key-off words `&= ~` key-on words). The C declares both
parameters `short`; the first is never read. The caller passes the count
from a `u8` and the pitch masked to 16 bits.

**`vmNoiseOn` (`0x8009B848`)**, `(voice)`; `vm_no1.o`. The last step for a
noise tone (VAG number 0xFF). The same volume law.
`SpuSetNoiseClock((note - centre) & 0x3F)` (`0x8009F3E8`) with
`D_800EA6F0`'s note and the tone's centre note. Slots 0 and 1 = left and
right, dirty bits 0x03; the voice's `pitch` = 10. Every unmasked voice's
`key_stat` is ANDed with 1, then this voice's becomes 2, so only one voice
is the noise voice. Pending masks in this order: key-on gains the bit;
key-off words `&= ~` key-on words; reverb set or cleared by mode bit 2; the
noise words (`D_800E9EB8`, `D_800E9EBA`) are **assigned** the voice's bit,
not ORed.

### The volume law

One computation serves key-on and both re-apply functions. With v0 the
note-level volume:

```
a = v0 * D_800EA6DC->mvol * 0x3FFF / 16129          (signed divide)
b = a * ProgAtr.mvol * VagAtr.vol / 16129           (unsigned divide)
L = b * score->voll / 127        R = b * score->volr / 127
for p in tone pan, program pan, third pan:
    p < 64:     R = R * p / 63
    otherwise:  L = L * (127 - p) / 63
if D_800EA6D0 == 1:  L = R = the larger of the two
L = L * L / 16383               R = R * R / 16383
```

16129 is 127 squared; 16383 is 0x3FFF. `D_800EA6DC` is the current VAB's
header and `D_800EA6D0` the mono flag (§"Globals"). L and R go to shadow slots 0 and 1. What
each caller feeds in:

| Caller | v0 | Program record | Tone record | Third pan |
|---|---|---|---|---|
| `_SsVmKeyOnNow` (`0x8009BEB8`), `vmNoiseOn` (`0x8009B848`) | `D_800EA6F0.voll`, already `voll * score->vol[score->channel] / 127` from `_SsVmKeyOn` (`0x8009AA78`) | `D_800EA6F0.mvol`, `.mpan` (from `D_800EA6D4[prog]`) | `D_800EA6F0.vol`, `.pan` | `D_800EA6F0.note_pan` |
| `_SsVmSetSeqVol` (`0x8009C7D8`) | voice `vol` * `score->vol[voice.channel]` / 127 | `D_800EA6D4[voice.pg]` | `D_800EA6E0[voice.prog * 16 + voice.tone]` | voice `pan` |
| `_SsVmSetVol` (`0x8009CFB8`) | voice `vol` * the `vol` argument / 127 | `mvol` from `D_800EA6D4[prog]` (the argument), `mpan` from `D_800EA6D4[voice.pg]` | `D_800EA6E0[voice.prog * 16 + voice.tone]` | the `pan` argument |

**The direct key-on.** When `D_800EA6F0.seq_sep` is 0x21 (the pseudo-score
`_SsVmSeKeyOn` (`0x8009B0F4`) and `_SsVmSeKeyOff` (`0x8009B1E0`) use) the
two key-on functions skip the score split and skip the square: L = R = b
goes straight into the pan stages and the result is stored unsquared.
`_SsVmKeyOn` likewise leaves `voll` unscaled for it and does not write the
voice's `vol` and `channel`.

### Re-applying volume

**`_SsVmSetVol` (`0x8009CFB8`)**, `(seq_sep, vab_id, prog, vol, pan)`;
`vm_vol.o`. Runs when a channel's volume or pan changes. Its callers are
`_SsContMainVol` (`0x800961C8`, CC 7), `_SsContPanpot` (`0x80096298`, CC
10) and `_SsContExpression` (`0x80096368`, CC 11). `_SsVmVSetUp(vab_id,
prog)` (`0x8009D558`); `D_800EA6F0.seq_sep` = `seq_sep`; a `vol` or `pan`
of 0 counts as 1. Then for every voice below `D_800EA6E4` that is not
masked and whose `seq_sep`, `pg` and `vab_id` match: if the score's
`vol[score->channel]` differs from `vol` and is 0 it becomes 1; the law is
applied with the inputs above; the voice's dirty bits 0x03 are set. Returns
the number of voices updated. Its C keeps three constructs that emit no
code and change no behaviour: a dead copy of three statements, two double
`do { } while (0)` wrappers and an empty `__asm__`. They are marked in the
source; [psyq/codegen.md](psyq/codegen.md) says what they steer.

**`_SsVmSetSeqVol` (`0x8009C7D8`)**, `(seq_sep, voll, volr)`; `vm_seq.o`.
Runs when a score's volume changes. Its callers are `_SsSndSetVol`
(`0x80099738`), `SsSeqSetVol` (`0x800997C8`) and `SsSepSetVol`
(`0x80099830`) on a playing score, `_SsSndCrescendo` (`0x800986B8`),
`Snd_SetPlayMode` (`0x80098018`) and `func_80095758`, the worker of
`SsSeqClose` (`0x800958D4`) and `SsSepClose` (`0x800958F8`), which sets 0 /
0 before keying the score off. Every caller declares a fourth parameter and
passes 1; the definition has three. It stores `voll` / `volr` in the score,
each capped at 127. Then, for every unmasked voice below `D_800EA6E4` whose
`seq_sep` matches and whose `vab_id` is the score's, it calls
`_SsVmVSetUp(voice.vab_id, voice.prog)`, passing the tone-table row where
that function takes a program number, applies the law and sets dirty bits
0x03. Returns `seq_sep`.

**`_SsVmGetSeqVol` (`0x8009CD44`)**, `(seq_sep, &voll, &volr)`, **`_SsVmGetSeqLVol`
(`0x8009CDA8`)**, `(seq_sep)`, and **`_SsVmGetSeqRVol` (`0x8009CDF0`)**,
`(seq_sep)`; `vm_seq.o`. Each stores `seq_sep` in `D_800EA6F0.seq_sep`.
`_SsVmGetSeqVol` writes out the score's `voll` (`+0x58`) and `volr`
(`+0x5A`) and returns `seq_sep`; `_SsVmGetSeqLVol` and `_SsVmGetSeqRVol`
return one of them. `_SsVmGetSeqVol`'s callers are `_SsSndCrescendo` and
`SsSeqGetVol` (`0x800998C0`); nothing in the C calls the other two.

**`_SsVmSetProgVol` (`0x8009C658`)**, `(vabId, prog, vol)`, and
**`_SsVmSetProgPan` (`0x8009C718`)**, `(vabId, prog, pan)`; `vm_prog.o`.
After `_SsVmVSetUp(vabId, prog)` each writes the program's `mvol` or `mpan`
in `D_800EA6D4[prog]` and returns the member read back, or -1 when
`_SsVmVSetUp` refuses. `_SsVmGetProgVol` (`0x8009C6C8`) and
`_SsVmGetProgPan` (`0x8009C788`) return the member the same way. None of
the four touches a voice. Only `_SsVmSetProgVol` has a caller,
`_SsContExpression`, which then calls `_SsVmSetVol`.

### Key-off

**`_SsVmKeyOff` (`0x8009AF94`)**, `(seq_sep, vab, prog, note)`; `vm_key.o`.
Keys off every voice playing that note and returns how many. A voice below
`D_800EA6E4` qualifies when it is not masked and its `note`, `pg`,
`seq_sep` and `vab_id` all match. If its `vag` is 0xFF (a noise voice) it
calls `vmNoiseOff(voice)` (`0x8009BDB8`); otherwise `D_800EA6F0.voice` =
the voice (written as `D_800EA708`) and `_SsVmKeyOffNow(0)`
(`0x8009BDF8`). Callers: `_SsVmKeyOn` (`0x8009AA78`) and `_SsNoteOn`
(`0x80097A48`) for volume 0, and `_SsVmSeKeyOff` (`0x8009B1E0`), which
passes `seq_sep` 0x21.

`_SsVmKeyOffNow` (`0x8009BDF8`), which keys off the voice
`D_800EA6F0.voice` at once, is in [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md),
"libsnd — the voice manager: pitch, key-off, tone selection", with
`_SsVmSeqKeyOff` (`0x8009CE38`), which keys off every voice of one score.

**`vmNoiseOff` (`0x8009BDB8`)**, `(voice)`; `vm_noff.o`. Sets the voice's
`key_stat`, `vag` and `pitch` to 0. It touches no pending mask and no SPU
register. `_SsVmKeyOff` calls it in place of `_SsVmKeyOffNow` for a voice
whose `vag` is 0xFF.

### The direct key-on

**`_SsVmSeKeyOn` (`0x8009B0F4`)**, `(vabId, prog, note, unused, voll,
volr)`; `vm_key.o`. Keys a note on outside any score. With `voll == volr`:
vol = `voll`, pan = 64. With `volr < voll`: vol = `voll`, pan = `volr * 64
/ voll`. Otherwise vol = `volr`, pan = `127 - voll * 64 / volr`. Returns
`_SsVmKeyOn(0x21, vabId, prog, note, vol, pan)` (`0x8009AA78`); 0x21 is the
"no score" number. The fourth parameter is never read.

**`_SsVmSeKeyOff` (`0x8009B1E0`)**, `(vabId, prog, note)`; `vm_key.o`.
`_SsVmKeyOff(0x21, vabId, prog, note)` (`0x8009AF94`), its result dropped.

Nothing in the C calls either. The member's last function, `KeyOnCheck`
(`0x8009B214`), is empty and has no caller.

### Pitch bend

`_SsVmPitchBend` (`0x8009C574`), `vm_pb.o`'s entry, sets up the VAB and
sums `_SsVmPBVoice` over the voices; it is in
[psyq/libetc-libsnd.md](psyq/libetc-libsnd.md). Its caller is `_SsSetPitchBend`
(`0x80097598`), with the score's VAB and the channel's program.

**`_SsVmPBVoice` (`0x8009C388`)**, `(voice, seq_sep, vabId, prog, pbend)`;
`vm_pb.o`. Applies a bend value (0..127, 64 = centre) to one voice. It
returns 0 and does nothing unless the voice's `seq_sep`, `vab_id` and `pg`
match; it does not test `D_800B79C0`. With bend = `pbend` - 64, the tone
record `D_800EA6E0[voice.tone + D_800EA6F0.fake_prog * 16]` and note = the
voice's key-on note:

* bend > 0: note += bend * `pbmax` / 63; fine = (bend * `pbmax` % 63) * 2;
* bend < 0: note = note + bend * `pbmin` / 64 - 1; fine = (bend * `pbmin`
  % 64) * 2 + 127;
* bend == 0: fine = 0.

Then `D_800EA6F0.tone` = the voice's tone (its low byte),
`D_800EA6F0.voice` = the voice, slot 2 = `note2pitch2(note, fine)`
(`0x8009B6B4`), dirty bit 0x04, and the result is 1.

### Pitch from note

`note2pitch` (`0x8009B668`), the key-on's pitch, and `note2pitch2`
(`0x8009B6B4`), the bend's, are in [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md):
each passes a note, a fine step and the tone's centre note and fine tune to
`SsPitchFromNote`.

**`SsPitchFromNote` (`0x8009B71C`)**, `(note, fine, center, shift)`;
`vm_n2p.o`. Returns the SPU pitch (0x1000 = the sample's own rate) for a
tone recorded at note `center` with fine tune `shift`, played as `note`
plus `fine` 1/128ths of a semitone:

```
f = fine + shift;  n = note + f / 128 - center;  f %= 128;
if (f < 0) { f += 128; n--; n += f / 128; }
octave = n / 12 - 2;  semi = n % 12;
if (semi < 0) { semi += 12; octave--; }
pitch = (D_800B7A10[semi] * D_800B7A28[f]) >> 16;
if (octave >= 0) pitch = 0x3FFF;
else { pitch += 1 << (-octave - 1); pitch >>= -octave; }
```

`D_800B7A10` is `vm_n2p.o`'s .data + 0: 12 `u16` semitone ratios, 0x8000 *
2^(k/12), 0x8000 to 0xF1A1. `D_800B7A28` is .data + 0x18: 128 `u16` fine
ratios, 0x8000 * 2^(k/1536), 0x8000 to 0x878C. From two octaves above the
centre the result is the maximum, 0x3FFF.

### `_SsVmFlush`

**`_SsVmFlush` (`0x8009A258`)**, `()`; `vm_f.o`. Sends the pending state to
the hardware through libspu. `SsSeqCalledTbyT` (`0x80098448`) calls it
first thing each sound tick and `_SsVmInit` (`0x8009A728`) calls it last.
Seven steps:

1. **Envelope history.** `D_800EA5A4` (`_svm_envx_ptr`) = (`D_800EA5A4` +
   1) & 15, and that slot of `D_800EA5A8` (`_svm_envx_hist`) = 0. For each
   voice below `D_800EA6E4`: `SpuGetVoiceEnvelope` (`0x800A0D88`) writes
   the voice's envelope level into its `envx`; a level of 0 sets bit i in
   the new slot.
2. **Automatic key-off**, unless `D_800EA720` (`_svm_auto_kof_mode`) is
   nonzero. mask = the AND of history slots 0 to 14. Each voice in it gets
   `key_stat` 0; one in key state 2 (the noise voice) first has its SPU
   noise bit switched off with `SpuSetNoiseVoice(SPU_OFF, bit)`
   (`0x8009F098`).
3. **Key-on trim.** `D_800E9EB0 &= ~D_800EA5E8`; `D_800E9EB2 &=
   ~D_800EA5EA` (key-on words lose the pending key-offs).
4. **Automatic volume and pan**, for all 24 voices: a nonzero `auto_vol`
   calls the function at `D_800EA5A0` (`_autovol`) with i; a nonzero
   `auto_pan` calls the one at `D_800EA418` (`_autopan`). In this program
   neither ever runs (§"`D_800E9EC0` (`_svm_voice`) — 24 records of 0x38 bytes").
5. **Dirty registers**, for all 24 voices. A `SpuVoiceAttr` on the stack
   with `voice` = 1 << i and `mask` = 0 is filled by the dirty byte: bit
   0x01, VOLL and VOLR from slots 0 and 1; bit 0x04, PITCH from slot 2; bit
   0x08, WDSA with `addr` = slot 3 << 3; bit 0x10, ADSR1 and ADSR2 from
   slots 4 and 5. `SpuSetVoiceAttr(&attr)` (`0x800A0588`) is called when
   the mask is not 0, and the dirty byte is cleared.
6. **Key and mode words.** `SpuSetKey(SPU_OFF, (D_800EA5EA & 0xFF) << 16 |
   D_800EA5E8)` (`0x800A01D8`), then `SpuSetKey(SPU_ON, ...)` with the
   key-on words the same way. With mask = 0xFFFFFF >> (24 - `D_800EA6E4`):
   `SpuSetReverbVoice(SPU_BIT, ((D_800E9EB6 << 16 | D_800E9EB4) & mask) |
   (SpuGetReverbVoice() & ~mask))` (`SpuSetReverbVoice` (`0x8009FFC8`),
   `SpuGetReverbVoice` (`0x8009FFF8`)), and the same for noise with `D_800E9EB8` / `D_800E9EBA` (`SpuSetNoiseVoice`
   (`0x8009F098`), `SpuGetNoiseVoice` (`0x8009F388`)). Voices the manager
   does not run keep the bits libspu reports.
7. **Clear.** The key-off, key-on and noise words = 0. The reverb words
   persist.

### The VAB side

A VAB lives in one of 16 slots. `D_800EA710[slot]` (`_svm_vab_used`) is 0
while the slot is free, 2 once its header is opened and SPU RAM reserved,
and 1 once the sample body has been sent; only state 1 is usable by
`_SsVmVSetUp`.

**`_SsVmVSetUp` (`0x8009D558`)**, `(vabId, prog)`; `vm_vsu.o`. Makes one
VAB's tables current. Returns -1 when `(u16)vabId >= 16`, when
`D_800EA710[vabId] != 1`, or when `prog >= D_800EA6D2` (`kMaxPrograms`).
Otherwise `D_800EA6DC` / `D_800EA6D4` / `D_800EA6E0` (`_svm_vh` / `_svm_pg`
/ `_svm_tn`) = `D_800EA648` / `D_800EA608` / `D_800EA690[vabId]`
(`_svm_vab_vh` / `_svm_vab_pg` / `_svm_vab_tn`), `D_800EA6F0.vab_id` =
`vabId`, `.prog` = `prog`, `.fake_prog` = the low byte of
`D_800EA6D4[prog].reserved1`, and the result is 0. It has 11 call sites:
`_SsVmKeyOn` (`0x8009AA78`), `_SsVmSetVol` (`0x8009CFB8`), `_SsVmSetSeqVol`
(`0x8009C7D8`), `_SsVmPitchBend` (`0x8009C574`), `_SsVmSetProgVol`
(`0x8009C658`), `_SsVmGetProgVol` (`0x8009C6C8`), `_SsVmSetProgPan`
(`0x8009C718`), `_SsVmGetProgPan` (`0x8009C788`), `SsUtGetProgAtr`
(`0x80099B18`), `SsUtGetVagAtr` (`0x80099C28`) and `SsUtSetVagAtr`
(`0x8009A068`).

**`_SsVabOpenHeadWithMode` (`0x8009D7F8`)**, `(addr, vabid, alloc,
sbaddr)`; `vs_vh.o`. Registers a VAB header and reserves SPU RAM for its
samples; `alloc` is `s32 (*)(s32 size, s32 sbaddr, s16 slot)`. It is the
body of the public entries, and lies in `vs_vh.o` at `+0x70`.
`SsVabOpenHead` (`0x8009D6E8`, `vs_vh2.o`) passes `func_8009D71C`, its
`SpuMalloc` helper; `SsVabOpenHeadSticky` (`0x8009D788`) and
`SsVabFakeHead` (`0x8009D7BC`) pass `func_8009D7F0`, which returns its
second argument (`sbaddr`). Only `SsVabOpenHead` has a caller, the game's
`sound_vab_head_open` (`0x80024314`). The entries are in
[psyq/libetc-libsnd.md](psyq/libetc-libsnd.md) and [psyq/libspu.md](psyq/libspu.md).

1. `_spu_getInTransfer()` (`0x800A0570`) == 1 returns -1; otherwise
   `_spu_setInTransfer(1)` (`0x800A0548`).
2. `vabid >= 16` fails. `vabid == -1` takes the first slot whose state is
   0; any other `vabid` takes that slot if its state is 0. Taking a slot
   sets its state to 1 and adds one to `D_800EA768` (`_svm_vab_count`). No
   slot fails.
3. `D_800EA648[slot]` = `addr`; `D_800EA6E8` (`_svm_vab_not_send_size`) =
   0. The header's `form >> 8` must be 0x564142 ("VAB"). `D_800EA6D2`
   (`kMaxPrograms`) = 128 when the low byte of `form` is `'p'` and `ver >=
   5`, else 64. `ps > D_800EA6D2` fails.
4. The program table is at `addr + 0x20`, `D_800EA6D2` records of 0x10
   bytes: `D_800EA608[slot]`. Each `ProgAtr.reserved1` is set to the count
   of earlier programs with `tones != 0`: the program's index among the
   programs that have tones.
5. The tone table follows it: `D_800EA690[slot]`, `ps * 0x200` bytes (16
   `VagAtr` per used program). After that comes the VAG size table, 256
   `u16`: entries 0 to `vs` are taken as sizes in units of 8 bytes when
   `ver >= 5` and 4 bytes otherwise.
6. The sum, rounded up to a multiple of 64, is passed to `alloc(size,
   sbaddr, slot)`, which returns the block's SPU address. -1 from it is
   returned as is. A block ending past 0x80000 fails.
7. `D_800EA770[slot]` (`_svm_vab_start`) = the block. For each entry i from
   0 to `vs` the running end address (block + the sizes of entries 0 to i)
   >> 3 is written into the program table as a `u16`: record i / 2, `+0x0C`
   for even i and `+0x0E` for odd i. `D_800EA728[slot]` (`_svm_vab_total`)
   = the unrounded sum; the slot's state becomes 2; the slot number is
   returned.

Every failure in steps 2 to 6 except the allocator's -1 calls
`_spu_setInTransfer(0)` and returns -1. The three after the slot is taken
(bad tag, too many programs, block past 0x80000) also set the slot's state
back to 0 and take one off `D_800EA768`. The allocator's -1 is passed up
without that clean-up here; `func_8009D71C` does the same clean-up itself
when `SpuMalloc` (`0x8009EA48`) fails.

**Where VAG addresses are kept.** They live in the last word of the
`ProgAtr` records themselves: `ProgAtr.reserved2`, read through
`SvmVagAddrs`, holds two addresses per 16-byte record, `odd` at `+0x0C` and
`even` at `+0x0E`, each an SPU address divided by 8. `_SsVmDoAllocate`
(`0x8009B498`) reads the word for VAG number n (1-based,
`D_800EA6F0.vag`) from record (n - 1) / 2, `odd` for odd n and `even` for
even n: that is the opener's entry i = n - 1. The value goes to shadow
slot 3, and the flush shifts it back left by 3 into `SpuVoiceAttr.addr`.

**`SsVabTransBody` (`0x8009DBE8`)**, `(addr, vabid)`; `vs_vtb.o`. Uploads a
VAB's sample body to the SPU RAM reserved for it. `vabid` must be within
0..16 and the slot's state must be 2. `SpuSetTransferMode(0)`
(`0x800A0458`); if `SpuSetTransferStartAddr(D_800EA770[vabid])`
(`0x800A03F8`) returns nonzero, `SpuWrite(addr, D_800EA728[vabid])`
(`0x800A0398`), the state becomes 1 and `vabid` is returned. Every failure
path calls `_spu_setInTransfer(0)` and returns -1. The range test lets
`vabid` 16 through although the slot tables have 16 entries; the state
byte it then reads, `D_800EA710[16]`, is `D_800EA720`
(`_svm_auto_kof_mode`), which only `_SsVmInit` (`0x8009A728`) writes (0),
so the call fails. Its caller is the game's `sound_vab_body_transfer` (`0x800243D0`),
which then calls `SsVabTransCompleted(1)`.

**`SsVabTransCompleted` (`0x8009DCA8`)**, `(immediateFlag)`; `vs_vtc.o`.
Returns `SpuIsTransferCompleted(immediateFlag)` (`0x800A0488`) cut to a
`short`: 1 = finished; with 1 as the flag the call waits for the end.

**`SsVabClose` (`0x8009D638`)**, `(vabId)`; `vs_vab.o`. For `(u16)vabId <
16` and a slot in state 1 or 2: `SpuFree(D_800EA770[vabId])` (`0x8009F018`),
state 0, one off `D_800EA768`, and the transfer flag cleared if it is set.
Its caller is the game's `sound_bank_close` (`0x80024064`).

**`SsUtGetVagAtr` (`0x80099C28`)**, `(vabId, progNum, toneNum,
vagatrptr)`; `ut_gva.o`, and **`SsUtSetVagAtr` (`0x8009A068`)**, the same
signature; `ut_sva.o`. Read and overwrite one tone's attributes in an open
VAB. Both return -1 unless `D_800EA710[vabId] == 1`, then call
`_SsVmVSetUp(vabId, progNum)` (`0x8009D558`) and address the tone as
`D_800EA6E0[toneNum + D_800EA6F7 * 16]`, `D_800EA6F7` being
`D_800EA6F0.fake_prog`. The copy is field by field: `prior`, `mode`,
`vol`, `pan`, `center`, `shift`, `max`, `min`, `vibW`, `vibT`, `porW`,
`porT`, `pbmin`, `pbmax`, `adsr1`, `adsr2`, `prog`, `vag`; the reserved
fields are not copied. Both return 0. Each has 16 call sites:
`_SsContDataEntry` (`0x80095E48`) and `_SsSetNrpnVabAttr0` (`0x80096978`)
to `_SsSetNrpnVabAttr14` (`0x80097408`), each of which calls both.

## libspu — reverb

`s_srmp.o` sets the reverb's mode, depth, delay time and feedback in one
call; `s_srmt.o` sets the mode alone. Both
work on this state:

| Symbol | The C's comment | Meaning |
|---|---|---|
| `D_800B7B4C` | `_spu_rev_offsetaddr` | The current mode's work-area start address. |
| `_spu_rev_attr` (`0x800B7B50`) | — | The current reverb attributes, a `SpuReverbAttr` of 0x14 bytes: `.mode`, `.depth.left`, `.depth.right`, `.delay`, `.feedback`. |
| `D_800B8140` | `_spu_rev_startaddr[10]` | The work-area start address of each mode. |
| `D_800B8170` | `_spu_rev_param[10]` | One preset per mode, a `SpuRevParam` of 0x44 bytes: a `u32` `flags` word, then the 32 reverb registers. |
| `D_800B7BB0` | `_spu_RXX -> 0x1F801C00` | Pointer to the SPU registers, a `SpuUnion`: `rxx` is the `SpuRXX` block (reverb volume left / right at `+0x184` / `+0x186`, SPUCNT at `+0x1AA` with bit 7 the reverb enable, the 32 reverb registers from `+0x1C0`), `raw` the same 0x200 bytes as 0x100 words. |

The preset's registers, in order (bit k of `flags` selects register k; a
`flags` of 0 selects all 32): `dAPF1`, `dAPF2`, `vIIR`, `vCOMB1` to
`vCOMB4`, `vWALL`, `vAPF1`, `vAPF2`, `mLSAME`, `mRSAME`, `mLCOMB1`,
`mRCOMB1`, `mLCOMB2`, `mRCOMB2`, `dLSAME`, `dRSAME`, `mLDIFF`, `mRDIFF`,
`mLCOMB3`, `mRCOMB3`, `mLCOMB4`, `mRCOMB4`, `dLDIFF`, `dRDIFF`, `mLAPF1`,
`mRAPF1`, `mLAPF2`, `mRAPF2`, `vLIN`, `vRIN`.

**`SpuSetReverbModeParam` (`0x8009F618`)**, `(attr)`. Sets the fields
`attr->mask` names: 0x01 mode, 0x02 depth left, 0x04 depth right, 0x08 delay,
0x10 feedback; a mask of 0 means all five. It returns 0, or -1 for a mode it
refuses. In order:

1. **Mode** (mask 0x01, or 0). mode = `attr->mode`; if it carries 0x100
   (`SPU_REV_MODE_CLEAR_WA`) the bit is taken off and a clear flag kept. The
   mode must be 0 to 9 and `_SpuIsInAllocateArea_` (`0x8009F588`) of
   `D_800B8140[mode]` must be 0; otherwise the function returns -1
   at once. Then `_spu_rev_attr.mode` = mode, `D_800B7B4C` =
   `D_800B8140[mode]`, the mode's preset, its `flags` word with it,
   is copied to a record on the stack, and delay / feedback are reset: mode 7
   (ECHO) 0x7F / 0x7F, mode 8 (DELAY) 0x7F / 0, any other 0 / 0.
2. **Delay** (0x08, or 0); acts in modes 7 and 8 only. If the mode was not
   also set, the preset is copied and the record's `flags` becomes
   0x0C011C00, the six registers that follow. `_spu_rev_attr.delay` =
   `attr->delay`, and with t = (delay << 12) / 127: `mLSAME` = (delay << 13)
   / 127 - `dAPF1`; `mRSAME` = t - `dAPF2`; `mLCOMB1` = t + `mRCOMB1`;
   `dLSAME` = t + `dRSAME`; `mLAPF1` = t + `mLAPF2`; `mRAPF1` = t + `mRAPF2`.
3. **Feedback** (0x10, or 0); modes 7 and 8 only. If neither the mode nor the
   delay was set, the preset is copied and the record's `flags` becomes 0x80
   (`vWALL`); if only the delay was, 0x80 is ORed in.
   `_spu_rev_attr.feedback` = `attr->feedback`; `vWALL` = feedback * 0x8100
   / 127.
4. **Reverb off for a mode change.** If the mode was set and SPUCNT bit 7 is
   on, the bit is cleared and remembered.
5. **Depth.** If the mode was not set: depth left (0x02, or 0) and depth right
   (0x04, or 0) are written to the SPU reverb volume and to
   `_spu_rev_attr.depth`. If it was set, the reverb volume and
   `_spu_rev_attr.depth` are both zeroed whatever the mask says.
6. `_spu_setReverbAttr` (`0x8009FAF8`) on the record, when the mode, the delay
   or the feedback was set: it writes reverb register k for each bit k of the
   record's `flags`, or all 32 when `flags` is 0.
7. `SpuClearReverbWorkArea` (`0x800A0028`) on the mode, when the clear flag
   was given.
8. If the mode was set: `_spu_FsetRXX` (`0x8009E784`) `(0xD1, D_800B7B4C,
   0)`, then SPUCNT bit 7 is restored if step 4 cleared it.

Callers, all in libsnd: `SsUtSetReverbDelay` (`0x80099E68`),
`SsUtSetReverbDepth` (`0x80099EA8`), `SsUtSetReverbType` (`0x80099F38`) and
`SsUtSetReverbFeedback` (`0x80099FE8`).

**`SpuSetReverbModeType` (`0x800A0E68`)**, `(mode)`; `s_srmt.o`. The mode
half of `SpuSetReverbModeParam` (`0x8009F618`) as its own entry point. Bit
0x100 of `mode` asks for the work area to be cleared. Returns -1 when the
mode, compared unsigned, is 10 or more, or when `_SpuIsInAllocateArea_`
(`0x8009F588`) finds the mode's work area in allocated memory. Otherwise:
stores the mode in `_spu_rev_attr` (`0x800B7B50`) and the work-area start in
`D_800B7B4C`; copies the mode's 0x44-byte preset and zeroes its `flags`, so
all 32 registers are written; resets delay / feedback (mode 7 0x7F / 0x7F,
mode 8 0x7F / 0, others 0 / 0); switches reverb off in SPUCNT (bit 7) if it
was on; zeroes the reverb volume and the depth; `_spu_setReverbAttr`
(`0x8009FAF8`) on the preset; `SpuClearReverbWorkArea` (`0x800A0028`) if
asked; `_spu_FsetRXX` (`0x8009E784`) `(0xD1, start, 0)`; switches reverb
back on if it was. Returns 0. Its one caller is the game's
`sound_reverb_set` (`0x800241F4`), which passes the type with 0x100 set.

## libspu — the allocation table, voice masks, pitch, transfer

### The allocation table

| Symbol | The C's comment | Meaning |
|---|---|---|
| `D_800B7C18` | `_spu_memList` | Pointer to the table: `SPU_MALLOC` records `{ u32 addr; u32 size; }`. |
| `D_800B7C14` | `_spu_AllocLastNum` | The index of the last record. |
| `D_800B7BD8` | `_spu_mem_mode_plus` | The shift that turns an address in SPU register units into a byte address. |

`addr`'s low 28 bits are the SPU address. `addr & 0x80000000` marks a free
block, `addr & 0x40000000` the last record (the unallocated tail), and the
value 0x2FFFFFFF a deleted record.

**`_spu_gcSPU` (`0x8009ED18`)**, `()`; `s_m_int.o`. Collects the table in five
passes: merge each free record with a free successor that starts where it
ends (the successor becomes deleted); turn records of size 0 into deleted
ones; exchange-sort by `addr & 0x0FFFFFFF` (deleted records sort last); copy
the last record over the first deleted one and make that the last index;
fold trailing free records into the tail. Callers: `SpuMalloc`
(`0x8009EA48`) and `SpuFree` (`0x8009F018`).

**`_SpuIsInAllocateArea` (`0x8009F508`)**, `(addr)`; `s_m_util.o`. Returns 0
when the table pointer is null. Otherwise it walks the records: a free record
is skipped, the last record ends the walk with 0, and any other record
returns 1 if `addr` is at or below its start or below its start plus its
size. Nothing in this program calls it.

**`_SpuIsInAllocateArea_` (`0x8009F588`)**, `(addr)`; `s_m_util.o`. The same
test, written out again, on `addr << D_800B7BD8`. Callers:
`SpuSetReverb` (`0x8009F438`), `SpuSetReverbModeParam` (`0x8009F618`),
`SpuSetReverbModeType` (`0x800A0E68`) and `SpuClearReverbWorkArea`
(`0x800A0028`).

### Two-word voice masks

A 24-bit voice mask lives in two consecutive SPU register words. With bit 0
of `D_800B7B9C` set, writes go to an event queue instead of the hardware:

| Symbol | The C's comment | Meaning |
|---|---|---|
| `D_800B7B9C` | `_spu_env: bit 0 = SPU_ENV_EVENT_QUEUEING` | Bit 0: queue the write. |
| `D_800EA8C0` | `_spu_RQ` | `u16` array, the queued copy of the SPU words from 0xC4 up: `D_800EA8C0[word - 0xC4]`. |
| `D_800B7B68` | `_spu_RQmask` | One dirty bit per queued register pair: bit `(addr1 - 0xC6) >> 1`. |
| `D_800B7BB0` | `_spu_RXX -> 0x1F801C00` | The registers, read and written as words through `raw`. |

The C writes `D_800B7B9C` only in `_SpuInit` (`0x8009DCF8`), which stores 0,
so the queue arms of `_SpuSetAnyVoice` and `SpuSetKey` (`0x800A01D8`) never
run in this program.

**`_SpuSetAnyVoice` (`0x8009F0C8`)**, `(on_off, voice_bit, addr1, addr2)`;
`s_sav.o`. `addr1` and `addr2` are the numbers of the pair's low and high
register words. It reads `((w[addr2] & 0xFF) << 16) | w[addr1]` from
the queue or the registers, then by `on_off`: 1 sets the bits of
`voice_bit`, 0 clears them, 8 (`SPU_BIT`) assigns both words; any other value
writes nothing. The queue arm also sets the pair's bit in `D_800B7B68`. It
returns the resulting mask, 24 bits.

**`_SpuGetAnyVoice` (`0x8009F3B8`)**, `(addr1, addr2)`; `s_gav.o`. The
read alone, always from the hardware registers. Callers: `SpuGetNoiseVoice`
(`0x8009F388`), `_SpuGetAnyVoice(0xCA, 0xCB)`, and `SpuGetReverbVoice`
(`0x8009FFF8`), `_SpuGetAnyVoice(0xCC, 0xCD)`. Both are called by libsnd's
`_SsVmFlush` (`0x8009A258`).

**`SpuSetNoiseVoice` (`0x8009F098`)**, `(on_off, voice_bit)`; `s_snv.o`:
`_SpuSetAnyVoice(on_off, voice_bit, 0xCA, 0xCB)`, the noise-mode pair
(0x1F801D94).

**`SpuSetReverbVoice` (`0x8009FFC8`)**, `(on_off, voice_bit)`; `s_srv.o`: the
same with 0xCC, 0xCD, the reverb-on pair (0x1F801D98).

**`SpuSetNoiseClock` (`0x8009F3E8`)**, `(n_clock)`; `s_snc.o`. Clamps
`n_clock` to 0 to 0x3F and writes it into SPUCNT bits 8-13 (word 0xD5,
0x1F801DAA); returns the clamped value.

### Pitch — `s_n2p.o`

| Symbol | Meaning |
|---|---|
| `D_800B8020` | `s_n2p.o`'s .data + 0: 12 semitone ratios, 0x8000 * 2^(k/12), the same as libsnd's `D_800B7A10`. |
| `D_800B8038` | .data + 0x18: 128 fine ratios, 0x8000 * 2^(f/1536), the same as libsnd's `D_800B7A28`. |

**`_spu_note2pitch` (`0x800A0B88`)**, `(cen_note_high, cen_note_low,
note_high, note_low)`. fine = `note_low + cen_note_low` (unsigned 16-bit);
note = `note_high + fine / 128 - cen_note_high`; fine %= 128. The octave is
note / 12 - 2 and the semitone note % 12, a negative semitone wrapped up by
12 with the octave one lower. The pitch is (semitone ratio * fine ratio) >>
16; an octave of 0 or more gives 0x3FFF, a negative one shifts right by
-octave with rounding. The same note as the centre gives 0x1000. Caller:
`SpuSetVoiceAttr` (`0x800A0588`).

**`_spu_pitch2note` (`0x800A0C58`)**, `(cen_note_high, cen_note_low,
pitch)`. The inverse: clamp `pitch` to 0x3FFF; find its highest set bit
(bits 0 to 13) and shift the pitch up to 0x8000..0xFFFF; search the semitone
table downward for the first entry not above it; divide by that entry;
search the fine table the same way; add the centre (`cen_note_low + 1` to
the fine part, `cen_note_high + (bit - 12) * 12` to the note, the fine
part's carry into the note); return `(note << 8) | fine`, the fine part with
bit 0 clear. A pitch of 0 has no set bit, stays 0, and matches no entry of
either table; the semitone and fine results are then read uninitialised. No
caller in this program.

### The transfer check

**`SpuIsTransferCompleted` (`0x800A0488`)**, `(flag)`; `s_itc.o`. Returns 1 at
once when `D_800B7B40` (`_spu_trans_mode`) is 1 (I/O transfer) or
`D_800B7BE4` (`_spu_inTransfer`) is 1. Otherwise it tests the DMA-end event
`D_800B7B38` (`_spu_EVdma`) with `TestEvent` (`0x800A0538`): with
`flag == 1` it polls until the event fires, then stores 1 in
`D_800B7BE4` and returns 1; with any other flag it returns the test's
result, storing 1 when it is 1. `D_800B7BE4` has the inverted sense: 1
means no transfer is running. `_spu_setInTransfer` (`0x800A0548`) and
`_spu_getInTransfer` (`0x800A0570`) (`s_it.o`) store and read it. Caller:
libsnd's `SsVabTransCompleted` (`0x8009DCA8`).

## libcd — the file search

`iso9660.o`'s `CdSearchFile` (`0x80092508`) resolves a path through two
caches: the disc's directories, built once per disc from the path table, and
the files of one directory at a time.

| Symbol | The C's comment | Meaning |
|---|---|---|
| `D_800E7760` | — | The directory cache: 128 `CdlDIR` of 0x2C bytes: `+0x00` `number`, the directory's number (1-based); `+0x04` `parent`, its parent's number (0 ends the table); `+0x08` `lba`, the extent LBA; `+0x0C` `name`, 0x20 bytes. |
| `D_800E7160` | `the cached directory's files` | The file cache: 64 `CdlFILE` (0x18 bytes: `pos`, `size`, `name`) for one directory; an empty name ends the list. |
| `D_800E8D60` | `load_buf: one sector` | One 0x800-byte sector buffer. |
| `D_800B6360` | `CD_debug` | The debug level; it gates every print below. |
| `D_800B636C` | `CD_nopen: lid-open count` | Counted by the interrupt service (§"libcd — BIOS.OBJ's interrupt state and start-up"). |
| `D_800B6724` | `the CD_nopen the directory cache was built at` | A change of `D_800B636C` means a new disc. |
| `D_800B6720` | `the directory number D_800E7160 holds` | 0 = none. |

Helpers: `cd_read` (`0x80092E04`), `(sectors, lba, buf)`, converts the LBA
with `CdIntToPos` (`0x80094818`), sends `CdlSetloc` through `CdControl`
(`0x80090604`), calls `CdRead` (`0x800935F8`) `(sectors, buf, 0x80)` and
returns `CdReadSync` (`0x80093794`) `(0, 0) == 0`. `_cmp` (`0x800927E0`),
`(a, b)`, is `strncmp(a, b, 12) == 0`. The library calls are `strncmp`
(`0x80092E78`), `strcmp` (`0x8008B788`), `memcpy` (`0x8008E7C8`) and
`printf` (`0x80021700`).

**`CD_newmedia` (`0x80092800`)**, `()`. A static of `iso9660.o` (at `+0x2F8`;
the object exports no name for it). It rebuilds the directory cache:

1. `cd_read(1, 16, D_800E8D60)` reads the primary volume descriptor. On failure:
   "CD_newmedia: Read error in cd_read(PVD)\n" (`D_800135C0`) and 0.
2. `strncmp(&D_800E8D60[1], "CD001", 5)` (the string is `D_800135EC`) must be 0.
   Otherwise: "CD_newmedia: Disc format error in cd_read(PVD)\n"
   (`D_800135F4`) and 0.
3. The little-endian path table's LBA is the unaligned four bytes at
   descriptor `+0x8C`. That **one sector** is read over the same buffer. On
   failure: "CD_newmedia: Read error (PT:%08x)\n" (`D_80013624`) and 0.
4. It walks the path-table records (`+0` name length, `+1` extended-attribute
   length, `+2` extent LBA, unaligned, `+6` parent number, `+8` name, padded
   to an even length). Cache record i gets the LBA, the byte at `+6` as the
   parent (the low byte of the parent number), i + 1 as the number, and the
   name, copied and NUL-terminated. The step is 8 + length + (length & 1). It
   stops at a zero name length, at the end of the sector, or after 128
   records; a table shorter than 128 is ended by a record with parent 0, the
   terminator `func_80092AC4` tests.
5. `D_800B6720` = 0, so the file cache matches no directory. Returns 1.

The three error prints need `D_800B6360` > 0. At `D_800B6360` > 1 it also prints
"CD_newmedia: sarching dir..\n" (`D_80013648`, the library's spelling), one
"\t%08x,%04x,%04x,%s\n" line per directory (`D_80013668`) and "CD_newmedia: %d
dir entries found\n" (`D_8001367C`).

**`func_80092AC4`**, `(parent, name)`. Walks the directory cache. A record
with parent 0 ends the table and returns -1. A record whose parent equals the
argument and whose name compares equal with `strcmp` returns its number, the
1-based index. After 128 records: -1.

**`CD_cachefile` (`0x80092B68`)**, `(dir)`. A static of `iso9660.o` (at
`+0x660`). Loads the files of directory number `dir` into the file cache. If
`dir` equals `D_800B6720` it returns 1 at once. Otherwise `cd_read` reads one
sector at the extent LBA of directory-cache record `dir - 1` into `D_800E8D60`;
on failure it prints "CD_cachefile: dir not found\n" (`D_800136A0`, at
`D_800B6360` > 0) and returns -1, leaving the cache as it was. It then walks
the ISO9660 directory records of that **one sector**: `+0` record length (0
ends the walk), `+2` extent LBA (unaligned), turned into `file[i].pos` with
`CdIntToPos`, `+10` size (unaligned) into `file[i].size`, `+0x20` name
length, `+0x21` name. Records 0 and 1 are named "." (`D_800136DC`) and ".."
(`D_800136E0`); the others have their name copied and NUL-terminated. It
advances by the record length and stops at 64 files or the end of the
sector. Then `D_800B6720` = `dir`, the list is ended with an empty name when
it holds fewer than 64 files, and the result is 1. At `D_800B6360` > 1:
"CD_cachefile: searching...\n" (`D_800136C0`), one
"\t(%02x:%02x:%02x) %8d %s\n" line per file (`D_800136E4`) and
"CD_cachefile: %d files found\n" (`D_80013700`).

**`CdSearchFile` (`0x80092508`)**, `(fp, name)`. Resolves an absolute path to
its directory entry. Its only caller is the game's `cd_sector_load`
(`0x80020E88`, `cdread.c`).

1. If `D_800B636C` differs from `D_800B6724`, `CD_newmedia()` rebuilds the
   directory cache (a 0 from it returns 0) and `D_800B6724` = `D_800B636C`.
2. `name` must begin with `\`; otherwise it returns 0.
3. Starting from directory 1 and from `name[0]`, each `\`-terminated component
   is copied to a 32-byte stack buffer and looked up with
   `func_80092AC4(dir, buffer)`. The leading `\` therefore counts as a
   first, empty, component, which matches the path table's root record (its
   name is a single zero byte and its parent 1). What follows the last `\`
   is the file name, left in the buffer. The loop runs at most 8 times and
   the file name takes the last turn, so a path holds at most seven `\`:
   the root's and six directories'.
4. Failures so far return 0 and print at `D_800B6360` > 0: an eighth `\`,
   "%s: path level (%d) error\n" (`D_80013534`); a component `func_80092AC4`
   does not find, "%s: dir was not found\n" (`D_80013550`).
5. `CD_cachefile(dir)`; a result of 0 would print "CdSearchFile: disc
   error\n" (`D_80013568`) and return 0.
6. At `D_800B6360` > 1: "CdSearchFile: searching %s...\n" (`D_80013584`). It
   walks the file cache, up to 64 entries or an empty name, comparing each
   name with `_cmp`. A hit copies the 0x18-byte `CdlFILE` to `*fp` and returns
   the address of the cache entry ("%s:  found\n", `D_800135A4`, at level >
   1). No hit: "%s: not found\n" (`D_800135B0`) at level > 0 and 0.

**Limits.** Only the first sector of the path table and the first sector of
each directory are read, so at most 128 directories are known (fewer if the
table's first sector holds fewer) and at most 64 files per directory are seen
(fewer if they do not fit one sector); a path may name six directories
below the root; a component is copied into 32 bytes with no length check;
names are compared over 12 characters.

**A library behaviour.** `CD_cachefile` returns 1 or -1, never 0, and
`CdSearchFile` tests its result with `== 0`. A failed directory read is
therefore not caught: the "disc error" branch never runs and the search goes
on over whatever the file cache held before.

## libcd — BIOS.OBJ's interrupt state and start-up

`bios.o` is libcd's lowest layer. It writes commands to the CD controller,
services the controller's interrupt, and keeps the state that `sys.o`'s
`CdControl` family and `cdread.o` wait on. The object's source is `bios.c`
revision 1.86 of 1997/03/28; its revision string is in the image at
0x800134BC, the last of the six pointers in `D_800B6638`.

| Symbol | The C's comment | Meaning |
|---|---|---|
| `D_800B661C` | `reg0 -> 0x1F801800: the CD controller's index / status register` | Bits 0-1 select the register bank; bit 5 says the response FIFO holds bytes. |
| `D_800B6620` | `reg1 -> 0x1F801801: command / response FIFO` | |
| `D_800B6624` | `reg2 -> 0x1F801802: parameter FIFO / interrupt enable` | |
| `D_800B6628` | `reg3 -> 0x1F801803: request / interrupt flag` | Bits 0-2 are the interrupt cause. |
| `D_800B662C` | `-> 0x1F801020` | The register `CD_flush` and `CD_init` write 0x1325 to. |
| `D_800B6630` | — | Pointer to the SPU registers (0x1F801C00) as a `SpuRXX`; `CD_initvol` uses it. |
| `D_800B6634` | — | The interrupt state, three volatile bytes: [0] sync, [1] ready, [2] end. The values are 0 none, 1 DataReady, 2 Complete, 3 Acknowledge, 4 DataEnd, 5 DiskError. |
| `D_800B6638` | — | The object's pointer block: `D_800B6634`, `D_800E7108`, `D_800B6375`, `D_800B6364`, `D_800B6370` and the revision string. `CD_init` prints its address. |
| `D_800B6358` | `CD_cbsync: the user's sync callback` | |
| `D_800B635C` | `CD_cbready: the user's ready callback` | |
| `D_800B6364` | `CD_status` | Result byte 0 of the last interrupt that updates the status: the drive status. |
| `D_800B6368` | `CD_status1` | Result byte 1 of the same interrupt. |
| `D_800B636C` | `CD_nopen: lid-open count` | Counts each time the lid-open bit (0x10) appears in a new status. |
| `D_800B6370` | — | The last Setloc position, 4 bytes (`CD_pos` in `CD_cw`'s comment). `CdLastPos` (`0x80090488`) returns its address. |
| `D_800B6374` | `CD_mode` | The last Setmode byte. |
| `D_800B6375` | `CD_com` | The last command written. |
| `D_800B637C` | — | 32 command names, by command number. |
| `D_800B63FC` | — | 8 interrupt-state names, by state. |
| `D_800B641C` | — | Per command: an Acknowledge without error leaves it running (sync state 3) instead of completing it. |
| `D_800B649C` | — | Per command: clear the ready state when writing it. |
| `D_800B651C` | — | Per command: whether its Acknowledge updates `D_800B6364` / `D_800B6368` (every other cause does). |
| `D_800B659C` | — | Per command: the parameter count (Setloc 3, Setfilter 2, Setmode 1, command 0x12 1, GetTD 1). It is `D_800B649C[com + 0x40]`, which is how `CD_cw` reads it. |
| `D_800E7108` | `the sync result (8 bytes)` | |
| `D_800E7110` | `the ready result (8 bytes)` | |
| `D_800E7118` | — | The end result, 8 bytes. |
| `D_800E7120` | — | The alarm, a `CdAlarm` `{ time, count, name }`. |

**The alarm.** `CD_sync`, `CD_ready`, `CD_cw` and `CD_datasync` arm
`D_800E7120` with `VSync(-1) + 960` (`VSync` (`0x800949A8`); 16 seconds at
60 Hz) and their own name. It fires when the VSync count passes that time or
after 0x3C0000 polls: it prints "CD timeout: " and "%s:(%s) Sync=%s,
Ready=%s" (the waiter's name, the command's name from `D_800B637C`, the sync
and ready state names from `D_800B63FC`), calls `CD_flush` (`0x800919A8`),
and the waiter returns -1.

**`func_80090A68`**, `()`. The CD-ROM interrupt service, at `bios.o` `+0`;
the object exports no name for it. With the index register at 1 it reads the
cause, `reg3 & 7`, again until two reads agree; a cause of 0 returns 0. It
reads up to 8 result bytes from register 1 while register 0's bit 5 is set
and zero-fills the rest, then acknowledges the controller (index = 1,
reg3 = 7, reg2 = 7). Unless the cause is an Acknowledge of a command whose
`D_800B651C` entry is 0, result bytes 0 and 1 become `D_800B6364` /
`D_800B6368`, a lid-open bit that was clear before counts in `D_800B636C`, and
`status & 0x1D` is the error flag. A DiskError cause at `D_800B6360` >= 3
prints the command's name and the two status bytes. Per cause:

| Cause | State set | Result copied to | Return |
|---|---|---|---|
| 3 Acknowledge | sync 5 on error; else 3 when `D_800B641C[com]`, else 2 | sync | 2; 1 for state 3 |
| 2 Complete | sync 5 on error, else 2 | sync | 2 |
| 1 DataReady | ready 5 on error (not when only one result byte came), else 1; then index = 0, reg3 = 0 | ready | 4 |
| 4 DataEnd | ready and end 4 | end and ready | 4 |
| 5 DiskError | sync and ready 5 | sync and ready | 6 |
| other | — | — | 0, after printing the cause |

Bit 2 of the return asks for the ready callback, bit 1 for the sync callback.

**`func_800920F8`**, `()`. The CD-ROM interrupt handler, at `bios.o`
`+0x1690`. It calls `func_80090A68` until that returns 0 and, per return,
the ready and sync callbacks (`D_800B635C`, `D_800B6358`);
[psyq/libcd.md](psyq/libcd.md) describes it. `CD_sync`, `CD_ready` and
`CD_cw` run the same loop in line when `CheckCallback` (`0x80094D2C`) says
they are themselves inside a callback, where the interrupt cannot arrive.

**`CD_sync` (`0x80090FCC`)**, `(mode, result)`. Waits for the command in
flight, with the alarm named "CD_sync". Each pass: the alarm check (-1); the
handler loop when `CheckCallback()` is nonzero; then one read of the sync
state. 2 (Complete) or 5 (DiskError) resets it to 2, copies the 8 result
bytes `D_800E7108` to `result` if not null, and returns the state. Otherwise
`mode != 0` returns 0 (a poll) and `mode == 0` loops.

**`CD_ready` (`0x8009124C`)**, `(mode, result)`. The same wait for drive data,
with the alarm named "CD_ready": a nonzero end state is cleared and returned
with `D_800E7118`'s bytes; else a nonzero ready state is cleared and returned
with `D_800E7110`'s.

### `CD_cw` (BIOS.OBJ)

**`CD_cw` (`0x80091514`)**, `(com, param, result, arg3)`. Writes one command
to the CD controller and, unless `arg3` is set, waits for its first response.

1. At `D_800B6360` >= 2 it prints "%s..." with the command's name,
   `D_800B637C[com]`.
2. A command whose parameter count `D_800B659C[com]` is nonzero, given a null
   `param`, returns -2 ("%s: no param" at level >= 1).
3. `CD_sync(0, 0)` waits out the previous command.
4. Setloc (command 2) copies four parameter bytes to `D_800B6370` (`CD_pos`).
   Setmode (0x0E) stores its byte in `D_800B6374` (`CD_mode`), before the
   command is written.
5. The sync state is cleared, and the ready state too when `D_800B649C[com]`
   is set.
6. Register 0 = 0; the parameter bytes go to register 2; `D_800B6375`
   (`CD_com`) = `com`; the command byte goes to register 1.
7. `arg3 != 0` returns 0 here, without waiting.
8. Otherwise it arms the alarm, named "CD_cw", and polls until the
   interrupt service sets the sync state, running the handler loop when
   `CheckCallback()` is nonzero; the alarm returns -1.
9. The 8 result bytes `D_800E7108` are copied to `result` if it is not null.
   The return is -1 when the sync state is 5 (DiskError), else 0.

### Volume, flush and start-up

**`CD_vol` (`0x80091920`)**, `(vol)`. Writes the four `CdlATV` mixer bytes:
index = 2, reg2 = `val0`, reg3 = `val1`; index = 3, reg1 = `val2`, reg2 =
`val3`, reg3 = 0x20 (apply). Returns 0. `CdMix` (`0x800909C0`) calls it.

**`CD_flush` (`0x800919A8`)**, `()`. Drops every pending CD interrupt: index =
1; while `reg3 & 7` is nonzero, index = 1, reg3 = 7, reg2 = 7. Then the end
and ready states are cleared and the sync state set to 2 (Complete: no
command outstanding); index = 0, reg3 = 0, and 0x1325 through `D_800B662C`.
Callers: the alarm and `CdFlush` (`0x80090500`).

**`CD_initvol` (`0x80091A7C`)**, `()`. Opens the CD audio path through the
SPU register pointer `D_800B6630`: if the current main volume (0x1B8 /
0x1BA) reads 0 on both sides the main volume (0x180 / 0x182) is set to
0x3FFF; the CD input volume (0x1B0 / 0x1B2) is set to 0x3FFF; SPUCNT
(0x1AA) = 0xC001. Then the CD mixer, `CD_vol` written out on {0x80, 0, 0x80,
0}: index = 2, reg2 = 0x80, reg3 = 0; index = 3, reg1 = 0x80, reg2 = 0,
reg3 = 0x20. Returns 0. Callers: `CdReset` (`0x80090494`) for mode 1, and
`func_80090388`, the reset of `event.o` that `CdInit` (`0x800902F8`)
retries.

**`CD_initintr` (`0x80091B6C`)**, `()`. Clears the two callbacks and the two
status words, then `ResetCallback()` (`0x80094BD8`) and
`InterruptCallback(2, func_800920F8)`. `CdReset(2)` calls it.

**`CD_init` (`0x80091BB8`)**, `()`. Prints "CD_init:" and the pointer
block's address; clears `D_800B6375` and `D_800B6374`; does what `CD_initintr`
does, written out; flushes as `CD_flush` does, written out; then
`CD_cw(1 Nop)`, a second Nop if the lid-open bit 0x10 is set,
`CD_cw(0xA Init)` and `CD_cw(0xC Demute)` (each returning -1 on failure), and
`CD_sync(0, 0)` must return 2. Returns 0 or -1. Callers: `CdReset` for
modes other than 2, and `func_80090388`.

**The sector DMA.** `CD_getsector` (`0x80091F00`), called by `CdGetSector`
(`0x800909E0`), takes a sector's data by DMA channel 3 and waits for it;
`CD_getsector2` (`0x80092000`), called by `CdGetSector2` (`0x80090A00`),
starts the same transfer and returns at once; `CD_datasync` (`0x80091D98`),
called by `CdDataSync` (`0x80090A44`), waits for it under the alarm. All
three are in [psyq/libcd.md](psyq/libcd.md).

**`CD_set_test_parmnum` (`0x800920EC`)**, `(parmNum)`. `D_800B659C[25] =
parmNum`: how many parameter bytes `CD_cw` sends for the Test command (0x19).

## libcd — CDREAD.OBJ

`cdread.o` starts at 0x80092EF8. It reads a run of sectors while the
program goes on: `CdRead` starts the read, the CD interrupts carry it, and
`CdReadSync` polls it. Its state is the record `D_800B6730`, volatile, indexed by word:

| Word | Meaning |
|---|---|
| [0] | Sectors requested. |
| [1] | Destination buffer. |
| [2] | Current write pointer. |
| [3] | Mode byte for `CdlSetmode` (the caller's mode with 0x20 set). |
| [4] | Sector size in words: 0x200, 0x249 or 0x246. |
| [5] | Sectors remaining: above 0 reading, 0 done, -1 error. |
| [6] | VSync count of the last activity. |
| [7] | VSync count at `CdRead`'s start (the 1200-vblank total timeout); set again when the lid is found open. |
| [8] | Expected sector number. |
| [9] | Pause pending: 1 after `CdlPause` is sent, cleared by the pause's sync callback or after 120 vblanks. |
| [10], [11], [12] | The caller's sync, ready and data callbacks, saved. |
| [13] | Result pointer handed to the user callback. |

| Symbol | The C's comment | Meaning |
|---|---|---|
| `D_800B62C0` | `CD_cbread: the user's read callback` | Called when a read ends, with 2 (Complete) or 5 (DiskError) and the result. |
| `D_800B62C4` | `CD_read_dma_mode` | Bit 0: sectors are taken by DMA with a data callback. |

The object sends its commands through `sys.o`'s `CdControlF`
(`0x80090740`), which returns without waiting, `CdControl` (`0x80090604`)
and `CdControlB` (`0x80090874`), which waits for completion
([psyq/libcd.md](psyq/libcd.md) has the family). `cdread.o` calls
`CdControlF` five times (each `CdlPause`, the Nop and `CdlReadN`) and
`CdControlB` once (the pause in `CdRead`).

The pause wait is an inline helper with a return value: 1 when nothing was
pending; 0 when the pause's sync callback cleared word [9]; -1 after 120
vblanks, when it puts the saved sync callback back and clears the flag
itself. `CdRead` and `CdReadBreak` use it.

**`func_80092EF8`**, `()`, and **`func_80092F2C`**, `(intr, result)`, the
object's first two functions, are in [psyq/libcd.md](psyq/libcd.md):
`func_80092EF8` is the sync callback installed with every `CdlPause` (it
puts the caller's sync callback, word [10], back and clears word [9]);
`func_80092F2C` is `CdRead`'s ready callback, which takes each sector into
word [2], checks a 2048-byte sector's header against word [8], counts word
[5] down, and on the end or the 1200-vblank timeout puts the caller's
callbacks back, pauses the drive and calls `D_800B62C0` with 2 (Complete)
or 5 (DiskError).

**`func_800931A0`**, `()`. The data callback in DMA mode: [2] advances by [4]
words, [5] counts down, [8] up. When [5] reaches 0 it ends the read as
`func_80092F2C` does and, when installed, calls `D_800B62C0` with 2 and word
[13].

**`func_80093270`**, `(retry)`. Starts or restarts the read. It clears the
sync, ready and (in DMA mode) data callbacks. With the lid open (`CdStatus`
(`0x80090458`) bit 0x10) it prints "CdRead: Shell open..." when the VSync
count is a multiple of 64, sends a Nop through `CdControlF`, restarts the
total timeout [7], and sets and returns -1 in [5]. With `retry` it prints
"CdRead: retry...", sends `CdlPause` and then `CdlSetloc` to the last
position (`CdLastPos` (`0x80090488`)) through `CdControl`; a failed seek
sets and returns -1. Then `CdFlush()` (`0x80090500`), and a `CdlSetmode`
with word [3] when it differs from `CdMode()` (`0x80090468`) or on a retry
(a failure sets and returns -1). [8] = the last position as a sector
number; it installs `func_80092F2C` as the ready callback (and
`func_800931A0` as the data callback in DMA mode), sets [2] = [1], sends
`CdlReadN` through `CdControlF`, sets [5] = [0] and [6] = `VSync(-1)`, and
returns [5].

**`CdReadBreak` (`0x80093458`)**, `()`. Stops a read: `CdDataSync(0)` in DMA
mode, [5] = 0, and, unless a pause already pending completes in time, puts
the callbacks back, installs `func_80092EF8`, sends `CdlPause` through
`CdControlF`, sets [9] and waits for the pause.

**`CdRead` (`0x800935F8`)**, `(sectors, buf, mode)`. Waits for a pending
pause; stores the mode and takes the sector size from `mode & 0x30` (0:
0x200 words, 0x20: 0x249, any other: 0x246), then sets 0x20 in the stored
mode; stores `buf` and `sectors`; saves and clears the sync, ready and (in
DMA mode) data callbacks into [10] to [12]; records the start time [7];
pauses the drive through `CdControlB` if `CdStatus() & 0xE0`; returns
`func_80093270(0) > 0`.

**`CdReadSync` (`0x80093794`)**, `(mode, result)`. Each pass: -1 if 1200
vblanks have passed since the start; if the remaining count is negative or
60 vblanks have passed since the last activity, `func_80093270(1)` and the
count is the full request; else the remaining count. With `mode == 0` it
loops while that is above 0, or 0 with the pause still pending. Then
`CdReady` (`0x800905BC`) `(1, result)`; it returns 1 instead of 0 while the
pause is pending.

**`CdReadCallback` (`0x80093898`)**, `(func)`. Installs `func` as
`D_800B62C0` and returns the one it replaces.

**`CdReadMode` (`0x800938AC`)**, `(mode)`. Sets `D_800B62C4` and
returns the old value.

## libgs — the double buffer

libgs keeps two display buffers in the frame buffer and swaps them once a
frame. Its state is a set of globals with no register rows; they are cited by
address, with the name the C's comments give them (libgs.h's).

| Symbol | The C's comment | Type | Meaning |
|---|---|---|---|
| `D_800E6F18` | — | `TILE[2]` | Each buffer's own screen-clear primitive (`GsSortClear`'s). |
| `D_800E6F38` | `PSDOFSX` | `short[2]` | Each buffer's frame-buffer x origin. |
| `D_800E6F3C` | `PSDOFSY` | `short[2]` | Each buffer's frame-buffer y origin. |
| `D_800E6F40` | `PSDBASEX` | `short[2]` | Each buffer's drawing base x: 0 when `PSDGPU` is set, the origin otherwise. |
| `D_800E6F44` | `PSDBASEY` | `short[2]` | The same for y. |
| `D_800E6F48` | `POSITION` | `_GsPOSITION` | The 2D drawing offset: `offx`, `offy`. |
| `D_800E6F50` | `GsDRAWENV` | `DRAWENV` | The drawing environment libgs sends. |
| `D_800E6FB0` | `GsDISPENV` | `DISPENV` | The display environment libgs sends. |
| `D_800E6FC4`, `D_800E6FC6` | `GsORGOFSX`, `GsORGOFSY` | `short` | The offset handed to `SetGeomOffset`; 0 when `PSDGPU` is set. |
| `D_800E6FC8` | `CLIP2` | `RECT` | The 2D clip rectangle. |
| `D_800E6FD0` | `PSDCNT` | `u_long` | Frame counter; never 0. |
| `D_800E6FD4` | `PSDIDX` | `short` | The current draw buffer, 0 or 1. |
| `D_800E6FD6` | `PSDGPU` | `short` | Nonzero: the GPU applies the drawing offset (`GsInitGraph`'s `intmode & 4`). |
| `D_800E6FD8` | `HWD0` | `long` | Display width. |
| `D_800E6FDC` | `VWD0` | `long` | Display height. |

`gs_001.o` holds `GsInitGraph` (`0x8008EAF8`), which sets the environments,
`PSDGPU`, the buffers and the clear primitives up, its two statics and
`GsSortClear` (`0x8008EF10`); [psyq/libgpu-libgs.md](psyq/libgpu-libgs.md)
describes all but `GsSortClear`. The game calls `GsInitGraph(320, 240, 4,
1, 0)` from `system_init` (`0x8001EEB8`), so `PSDGPU` is set, and calls it
again from `screen_mode_set` (`0x8001F0C8`).

* **`GsGetActiveBuff` (`0x8008F048`, `gs_002.o`).** Returns `PSDIDX`.

* **`GsSwapDispBuff` (`0x8008F428`, `gs_010.o`).** Shows the buffer just
  drawn and makes the other one the draw buffer. `GsDISPENV.disp.x` / `.y` =
  `PSDOFSX[PSDIDX]` / `PSDOFSY[PSDIDX]`; `PutDispEnv(&GsDISPENV)`
  (`PutDispEnv` (`0x8008C434`)); `SetDispMask(1)` (`SetDispMask`
  (`0x8008BB6C`)); then libgs.h's `GsIncFrame()`: `PSDCNT` + 1, made 1 if that
  gives 0, and `PSDIDX` flipped between 0 and 1; then `GsSetDrawBuffClip()`
  and `GsSetDrawBuffOffset()` for the new draw buffer. Its one caller is
  `frame_end` (`0x8001F30C`).

* **`GsSetDrawBuffClip` (`0x8008F168`, `gs_003.o`).** Copies the 2D clip
  rectangle into the drawing environment, moved to the draw buffer's origin:
  `GsDRAWENV.clip.w` / `.h` = `CLIP2.w` / `.h`; `.clip.x` / `.y` = `CLIP2.x` /
  `.y` + `PSDOFSX[PSDIDX]` / `PSDOFSY[PSDIDX]`; `PutDrawEnv(&GsDRAWENV)`
  (`PutDrawEnv` (`0x8008C268`)). Callers: `GsSwapDispBuff`, `GsDefDispBuff`
  (`0x8008F4D8`), `GsInitGraph`.

* **`GsSetDrawBuffOffset` (`0x8008F058`, `gs_002.o`).** Applies the 2D
  drawing offset `POSITION`. With `PSDGPU` nonzero: `GsORGOFSX` = `GsORGOFSY`
  = 0; `GsDRAWENV.ofs[0]` / `[1]` = `POSITION.offx` / `.offy` +
  `PSDOFSX[PSDIDX]` / `PSDOFSY[PSDIDX]`; `PutDrawEnv(&GsDRAWENV)`. Otherwise x
  / y = `POSITION.offx` / `.offy` + the origin of buffer `PSDIDX == 0 ? 1 :
  0`, the buffer that is not `PSDIDX`; `SetGeomOffset(x, y)` (`SetGeomOffset`
  (`0x8008FF78`)); `GsORGOFSX` / `GsORGOFSY` = x / y. Callers: `GsInitGraph`,
  `GsSwapDispBuff`, `GsDefDispBuff`, `GsInit3D` (`0x8008F578`).

* **`GsSortClear` (`0x8008EF10`, `gs_001.o`).** `GsSortClear(r, g, b,
  otp)` registers a screen clear in an ordering table. The draw buffer's own
  primitive `D_800E6F18[PSDIDX]` gets the colour, the origin `PSDOFSX[PSDIDX]`
  / `PSDOFSY[PSDIDX]`, the height `VWD0` and the width `HWD0`, or `HWD0 * 3 /
  2` when `GsDISPENV.isrgb24` is set; it is then linked with
  `AddPrim(otp->tag, &D_800E6F18[PSDIDX])` (`AddPrim` (`0x8008E9D8`); `GsOT.tag` is at
  `+0x10`). The primitive has `TILE`'s layout, but `func_8008ECF0` gives it
  length 3 and GPU command 2, the frame-buffer rectangle fill, so its x0 / y0
  are frame-buffer coordinates. Nothing in this program calls it.

## libgs — 3D defaults, GsDefDispBuff, flat lights

| Symbol | The C's comment | Type | Meaning |
|---|---|---|---|
| `D_800E6FE0` | `GsCLIP3far` | `long` | The far clip; 0x3FFF after `GsInit3D`. |
| `D_800E6FE4` | `GsLIGHT_MODE` | `int` | The lighting mode; 0 after `GsInit3D`. |
| `D_800E6FE8` | `GsCLIP3near` | `long` | The near clip; 10 after `GsInit3D`. |
| `D_800E6FF0` | `GsLIGHTWSMATRIX` | `MATRIX` | The flat lights' directions, a row per light; all zero after `GsInitGraph`. |
| `D_800E7010` | `_LC` | `MATRIX` | The local colour matrix, a column per light; all zero after `GsInitGraph`. |
| `D_800E7050` | `GsIDMATRIX` | `MATRIX` | The unit matrix (4096 on the diagonal, no translation). |
| `D_800E7090` | `GsIDMATRIX2` | `MATRIX` | The unit matrix with `m[1][1]` = 4096 * 4 * `VWD0` / `HWD0` / 3, corrected for the aspect ratio (4096 for 320 × 240). |

* **`GsInit3D` (`0x8008F578`, `gs_104.o`).** `POSITION.offx` / `.offy` =
  `HWD0 / 2`, `VWD0 / 2`, applied with `GsSetDrawBuffOffset`; then
  `GsCLIP3near` = 10, `GsLIGHT_MODE` = 0, `GsCLIP3far` = 0x3FFF. Nothing in
  this program calls it.

* **`GsDefDispBuff` (`0x8008F4D8`, `gs_103.o`).** `GsDefDispBuff(x0, y0, x1,
  y1)`: `PSDOFSX[0]` / `[1]` = x0 / x1 and `PSDOFSY[0]` / `[1]` = y0 / y1.
  The drawing bases `PSDBASEX` and `PSDBASEY` are 0 when `PSDGPU` is set and
  the same origins otherwise. Then `GsSetDrawBuffClip` and
  `GsSetDrawBuffOffset`. Callers: `system_init`, as `GsDefDispBuff(0, 0, 0,
  224)`, and `screen_mode_set`.

* **`GsSetFlatLight` (`0x8008F5F8`, `gs_107.o`).** `GsSetFlatLight(id, lt)`
  sets flat light `id` (0 to 2). `len = SquareRoot0(vx*vx + vy*vy + vz*vz)`
  (`SquareRoot0` (`0x8008FE98`)); when it is 0 the function returns -1 with
  nothing written. Row `id` of `GsLIGHTWSMATRIX` becomes the unit vector
  `-(vx, vy, vz) << 12` over `len`, in 4.12 fixed point. Column `id` of `_LC`
  becomes `(r, g, b) << 12` over 255; `_LC` is read with `func_8008FB14` and
  written back, and sent to the GTE, with `func_8008FAB0`. Returns 0. An `id`
  above 2 rewrites both matrices unchanged. Nothing in this program calls it.

* **`func_8008FAB0`** (static in `gs_107.o`). `_LC = *m;
  SetColorMatrix(m)` (`SetColorMatrix` (`0x8008FF28`)).

* **`func_8008FB14`** (static in `gs_107.o`). `*m = _LC`.

The one caller of each of the two statics is `GsSetFlatLight`.

## libgpu — GEnv

libgpu's `sys.o` lies whole at `0x8008B888`. Its data, its function table
`D_800B5F58` (a `GpuTable`, whose entries the sections below name), the
hardware pointers and the statics behind the table are in
[psyq/libgpu-libgs.md](psyq/libgpu-libgs.md), "libgpu — `sys.o`".

`D_800B5FA0` [`GEnv`] is libgpu's 0x80-byte environment, `GpuEnv` in
`include/psyq_private.h`:

| Offset | Type | Member | Meaning |
|---|---|---|---|
| `+0x00` | `u_char` | `type` | The GPU type, the `reset` entry's result; indexes the size tables. |
| `+0x01` | `u_char` | `queue` | The queue mode: 1 after `ResetGraph`, `SetGraphQueue`'s argument otherwise. |
| `+0x02` | `u_char` | `level` | The debug level (`SetGraphDebug`); the functions print at 2 and above. |
| `+0x03` | `u_char` | `reverse` | The display-reverse flag: `PutDispEnv` reads it, `SetGraphDebug` prints it, nothing in this program writes it. |
| `+0x04`, `+0x06` | `short` | `w`, `h` | The frame buffer's size for the GPU type. |
| `+0x08` | `volatile int` | `cb_pending` | Set to 1 by `func_8008D99C`; cleared by `func_8008DC4C` when it calls the DrawSync callback. |
| `+0x0C` | `void (*)(void)` | `drawsync` | The DrawSync callback; `DrawSyncCallback` (`0x8008BB0C`) writes it. |
| `+0x10` | `DRAWENV` | `draw` | The drawing environment in force. |
| `+0x6C` | `DISPENV` | `disp` | The display environment in force. |

`D_800B6020` and `D_800B602C` give the frame buffer's width and height per
GPU type (1024, 1024, 1024 and 512, 512, 1024).

## libgpu — ResetGraph

**`ResetGraph` (`0x8008B888`).** `ResetGraph(mode)` is a switch on `mode &
7`:

* 0 or 3: prints "ResetGraph:jtb=%08x,env=%08x\n" (`D_80012FDC`) with the
  function table and `GEnv`, through `printf` itself and at any debug level,
  then goes on as 5.
* 5: clears `GEnv` (0x80 bytes, `func_8008E790`); `ResetCallback()`;
  `GPU_cw` with the low 24 bits of the table pointer; `GEnv.type` = the
  table's `reset` entry (`func_8008DEAC`) called with `mode`; `GEnv.queue` =
  1; `GEnv.w` / `.h` = `D_800B6020[type]` / `D_800B602C[type]`; fills both
  cached environments, `GEnv.draw` and `GEnv.disp`, with 0xFF bytes, which
  makes the next `PutDispEnv` find every cached field different; returns the
  GPU type. `func_8008DEAC` measures the type only when `mode & 7` is 0, so
  `ResetGraph(3)` and `ResetGraph(5)` leave it 0.
* any other value: prints "ResetGraph(%d)...\n" (`D_80012FFC`) through
  `GPU_printf` when the debug level is 2 or more, and returns the `reset`
  entry called with 1, which stops the GPU's DMA, resets its command buffer
  and empties the queue, and keeps `GEnv`.

It is called from three places. `system_init` (`0x8001EEB8`) calls
`ResetGraph(0)`. `func_8008EB6C`, under `GsInitGraph`, calls `ResetGraph(0)`
or `ResetGraph(3)`. `frame_end` (`0x8001F30C`) calls `ResetGraph(1)` every
frame, after its `DrawSync(0)` (skipped when `g_ScreenMode` is 3) and
`VSync(0)` and before `GsSwapDispBuff`.

## libgpu — PutDispEnv

**`PutDispEnv` (`0x8008C434`).** `PutDispEnv(env)` makes `env` the display
environment and returns `env`. Every GP1 word goes through the function
table's `ctl` entry.

1. At debug level 2 or more: "PutDispEnv(%08x)...\n" (`D_8001316C`) through
   `GPU_printf`.
2. **Always** GP1 0x05, display area start: `0x05000000 | (disp.y & 0x3FF) <<
   10 | (disp.x & 0x3FF)`.
3. **Display mode**, when the four-byte word `isinter` / `isrgb24` / `pad0` /
   `pad1` or any field of the `disp` rectangle differs from the cached
   `GEnv.disp`. `env->pad0` = `GetVideoMode()` (0 NTSC, 1 PAL). GP1 0x08 is
   built from: 0x08 if PAL; 0x10 if `isrgb24`; 0x20 if `isinter`; 0x80 if
   `GEnv.reverse`; the width bits for `disp.w` (table below); 0x24 (480 lines,
   interlaced) when `disp.h` is over 256 (NTSC) or 288 (PAL). The word is sent
   and `env->pad0` = 8, which forces step 4.
4. **Display ranges**, when any field of the `screen` rectangle differs
   from the cache or `env->pad0` is 8. `env->pad0` = `GetVideoMode()`.
   v_start = `screen.y` + 16 (NTSC) or 19 (PAL); v_end = v_start +
   `screen.h`, or + 240 when `screen.h` is 0. With c the width class of
   `disp.w`: h_start = `D_800B6074[pad0][c]` start + `screen.x` *
   `D_800B609C[c]`; len = the table's end - start; h_end = h_start + len,
   or h_start + (len * `screen.w` >> 8) when `screen.w` is not 0. Clamps,
   PAL: h_start to 540..3220, h_end to at least h_start + 4 * the divisor
   and at most 3260, v_start to 19..303, v_end to at least v_start + 2 and
   at most 305. NTSC: h_start to 500..3250, h_end to at least h_start + 4 *
   the divisor and at most 3290, v_start to 16..257, v_end to at least
   v_start + 2 and at most 258. Then GP1 0x06, `0x06000000 | (h_end &
   0xFFF) << 12 | (h_start & 0xFFF)`, and GP1 0x07, `0x07000000 | (v_end &
   0x3FF) << 10 | (v_start & 0x3FF)`.
5. `env` is copied over the cached `GEnv.disp` (20 bytes, `memcpy`).

| `disp.w` | Class | Resolution | GP1 0x08 bits | Divisor `D_800B609C` | NTSC {start, end} | PAL {start, end} |
|---|---|---|---|---|---|---|
| <= 280 | 0 | 256 | 0 | 10 | 590, 3150 | 610, 3170 |
| <= 352 | 1 | 320 | 0x01 | 8 | 600, 3160 | 624, 3184 |
| <= 400 | 2 | 368 | 0x40 | 7 | 539, 3227 | 560, 3248 |
| <= 560 | 3 | 512 | 0x02 | 5 | 615, 3175 | 635, 3195 |
| over 560 | 4 | 640 | 0x03 | 4 | 620, 3180 | 640, 3200 |

`DISPENV.pad0` is thus libgpu's own scratch: it caches the video mode (0 or
1), and 8 means "the mode word was just rewritten, redo the ranges".
`GEnv.reverse` is never written in this program: `SetGraphReverse` is not
linked.

## libgpu — the command queue

Drawing requests go through `addque2`. The queue `_que` (`D_800E5718`) is a
ring of 64 entries indexed by `_qin` / `_qout` modulo 64, and is full at 63.
An entry (`GpuQue` in `include/psyq_private.h`, 0x60 bytes): `+0x00` `func`,
the deferred routine `int (*)(u_long p1, u_long p2)`; `+0x04` `p1`; `+0x08`
`p2`; `+0x0C` `buf[21]`, the queue's copy of the caller's data.

The front door is `func_8008D99C` (the table's `addque2`). It runs a
routine at once when queueing is off, or when the queue is empty, no GPU DMA
transfer is in flight and no DrawSync callback is installed; otherwise it
queues the routine at `_qin`, with a copy of its data, and runs
`func_8008DC4C` once. It, the
wait `func_8008DFFC` (the table's `sync`, `DrawSync`'s worker), the reset
`func_8008DEAC`, the GPU-type probe `func_8008E2B0` and the timeout
(`set_alarm` (`0x8008E138`), `func_8008E16C`) are in
[psyq/libgpu-libgs.md](psyq/libgpu-libgs.md).

* **`func_8008DC4C`** (static; the table's `exeque`). Runs the queue and
  returns the number of entries still queued, `(_qin - _qout) & 0x3F`.
  Callers: `func_8008D99C` and `func_8008DFFC`; it is also the DMA channel 2
  callback.

  1. While a GPU DMA transfer is in flight (bit 24 of channel 2's CHCR,
     `*D_800B60B4 & 0x01000000`) it returns 1 at once.
  2. `SetIntrMask(0)`; the old mask is kept in `D_800B60D4`.
  3. It executes entries from `_qout` until the queue is empty (`_qin ==
     _qout`) or an entry has started a DMA transfer. Per entry: when it is
     the last one queued and no DrawSync callback is installed,
     `DMACallback(2, 0)`; a busy-wait for GPUSTAT bit 26 (`*D_800B60A8 &
     0x04000000`); `_que[_qout].func(p1, p2)`; `_qout` = (`_qout` + 1) &
     0x3F.
  4. `SetIntrMask` restores the saved mask.
  5. If the queue is drained, no transfer is running, `GEnv.cb_pending` is
     set and a DrawSync callback exists: `cb_pending` = 0 and the callback is
     called.

## libgpu — sys.o's small API

`func_8008BC6C`, the object's check of a rectangle, and `func_8008CC48`,
which builds the drawing-environment words, are in
[psyq/libgpu-libgs.md](psyq/libgpu-libgs.md).

Three static primitives sit in the object's `.data`:

* `D_800B6038`, {0x04FFFFFF, 0x80000000, 0, 0, 0}: the frame-buffer copy
  (GP0 0x80) that `MoveImage` and `MoveImage2` fill and send.
* `D_800B604C`, {0x04FFFFFF, 0, 0, 0, 0}: the terminator primitive.
* `D_800B6060`, {tag, 0x80000000, 0, 0, 0x00010002}: a frame-buffer copy of
  a 2 × 1 rectangle from (0, 0) to (0, 0), whose tag is written at run time.
  `ClearOTag` and `ClearOTagR` put it at the end of every ordering table they
  clear.

* **`StoreImage` (`0x8008BF10`).** `StoreImage(rect, p)`: `func_8008BC6C`,
  then `addque2(drs, rect, 8, p)`; the queue keeps its own copy of the
  rectangle. The game calls it from three places.
* **`MoveImage` (`0x8008BF70`).** `MoveImage(rect, x, y)`: `func_8008BC6C`;
  returns -1 when the rectangle's width or height is 0; fills words 2 to 4 of
  `D_800B6038` with the source position, `(y << 16) | (x & 0xFFFF)` and the
  size; returns `addque2(cwc, D_800B6038, 0x14, 0)`. Nothing in this program
  calls it.
* **`ClearOTag` (`0x8008C028`).** `ClearOTag(ot, n)` links the first n - 1
  entries each to the next, then the tail: `D_800B6060`'s tag is linked to
  the terminator and the last entry points at `D_800B6060`. It returns the
  address of the last entry, not the table's start. `n = 0` is not guarded:
  the pre-decrement makes it -1 and the loop runs until it wraps. Nothing in
  this program calls it.
* **`ClearOTagR` (`0x8008C0F0`).** `ClearOTagR(ot, n)` calls the table's
  `otc` entry with `(ot, n)`, writes the same two-step tail into `ot[0]` and
  returns `ot`. Its caller is libgs's `GsClearOt` (`0x8008FB98`).
* **`DrawPrim` (`0x8008C19C`).** Draws one primitive outside the queue: the
  table's `sync` entry waits (mode 0), then its `cwb` entry writes the
  primitive's words after the tag. Nothing in this program calls it.
* **`DrawOTag` (`0x8008C1F8`).** `addque2(cwc, p, 0, 0)`: the ordering table
  is queued as it is, not copied. Its caller is libgs's `GsDrawOt`
  (`0x8008FB68`).
* **`PutDrawEnv` (`0x8008C268`).** Builds the environment primitive at
  `env->dr_env` (`func_8008CC48`), terminates its tag, queues it with
  `addque2(cwc, &env->dr_env, 0x40, 0)`, copies `env` into `GEnv.draw` (0x5C
  bytes) and returns `env`. Its callers are libgs's `gs_001.o`, `gs_002.o` and
  `gs_003.o`.
* **`DrawOTagEnv` (`0x8008C328`).** The same, with the primitive's tag
  pointing at ordering table `p` instead of the terminator, so the table is
  drawn under the new environment in one queued request. Nothing in this
  program calls it.

`ClearOTag`, `ClearOTagR`, `DrawOTag`, `PutDrawEnv` and `DrawOTagEnv` print
their name and arguments through `GPU_printf` when `GEnv.level` is 2 or more;
`StoreImage` and `MoveImage` print only through `func_8008BC6C`; `DrawPrim`
prints nothing.

## libgte — SquareRoot0

**`SquareRoot0` (`0x8008FE98`, `msc01.o`).** Sony hand assembly, not
compiled C: it stays an `INCLUDE_ASM` in `libgte_msc01.c` and is listed in
`config/asm_only.txt`. It uses the GTE's leading-bit counter (`mtc2 $a0,$30` /
`mfc2 $v0,$31`), and its table load is a `lui` / `addu` / `lh` triple, the
assembler's expansion of `lh t5,SQRT(t4)` in hand-written source. It computes
the integer square root of a non-negative 32-bit value by table:

```
lz = LZCR(a)            leading bits equal to the sign bit, 1..32
if lz == 32: return 0
lz &= ~1                an even shift
m  = lz >= 24 ? a << (lz - 24) : a >> (24 - lz)       0x40..0xFF
return (u32)(D_800B6130[m - 0x40] << ((31 - lz) >> 1)) >> 12
```

`D_800B6130` is libgte's square-root table: 192 `s16`, sqrt((i + 0x40) /
64) in 4.12 fixed point (4096 to 8175), ending at 0x800B62B0. Its one caller
is `GsSetFlatLight`, which passes a sum of three squares and is itself never
called.

`SetColorMatrix` (`0x8008FF28`, `mtx_11.o`), which `func_8008FAB0` calls,
is Sony hand assembly too and stays an `INCLUDE_ASM`. libgte's other
members are in [psyq/libcd.md](psyq/libcd.md), "libgte: the screen offset
and the exception-handler patch".

## libapi — root counters, the pad patch, firstfile

### counter.o (from `0x8008F268`)

`D_800B6104` points at the root counters (0x1F801100, 0x10 bytes each;
`Counter` in `include/psyq_private.h`), `D_800B6100` at the interrupt
registers (0x1F801070: status, then mask) and `D_800B6108` holds the
counters' interrupt bits {0x10, 0x20, 0x40, 1}. Each function takes the
counter number as `spec & 0xFFFF`.

* **`SetRCnt` (`0x8008F268`).** `SetRCnt(spec, target, flags)` programs a
  counter: for counter 3 and above it writes nothing and returns 0.
  Otherwise it clears the counter's mode word, writes the target and builds
  a new mode word from 0x48 (counters 0 and 1: `RCntMdGATE` makes it 0x49,
  `RCntMdSC` clear adds 0x100; counter 2: `RCntMdSC` clear makes it 0x248;
  any counter: `RCntMdINTR` adds 0x10), writes it and returns 1. The types
  its declaration carries are explained in [codegen.md](psyq/codegen.md).
* **`GetRCnt` (`0x8008F304`).** The current 16-bit count, or 0 for counter
  3 and above.
* **`StartRCnt` (`0x8008F33C`)**, **`StopRCnt` (`0x8008F36C`).** Set and
  clear the counter's bit (`D_800B6108[n]`) in the interrupt mask, for any
  number: neither checks it before reading the table. `StartRCnt` returns 1
  when the number is below 3, else 0; `StopRCnt` always returns 1.
* **`ResetRCnt` (`0x8008F3A0`).** Sets the count to 0 and returns 1, or 0
  with nothing written for counter 3 and above.

The callers are libgs's `GsInitVcount` (`0x8008F228`, `gs_007.o`), which
programs counter 1, the horizontal-sync counter, with target 0xFFFF and no
interrupt and starts it; libgs's `GsGetVcount` (`0x8008F3D8`, `gs_008.o`),
which reads that counter until two readings agree; and libsnd's
`func_80098138` (§"libsnd — the tick"), which calls `ResetRCnt` and
`SetRCnt`. `system_init` calls `GsInitVcount`; the game's profiler
(`profile.c`) calls `GsGetVcount`. Nothing calls `StopRCnt`.

### PATCH.OBJ and CHCLRPAD.OBJ

libapi's `patch.o` (`0x800A1518`) and `chclrpad.o` (`0x800A15B8`) are four
functions of Sony hand assembly, equal to the PSY-Q 4.5 objects: they save
`$ra` in `.bss`, call the BIOS in line with `jalr`, use the trapping `addi`,
and two are tail jumps through `$t1`. Each stays an `INCLUDE_ASM` and is
listed in `config/asm_only.txt`. Their callers are in libapi's `PAD.OBJ`
([psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md)), which
nothing in this program calls, so none of them runs. The kernel readings
below are of the SCPH-1001 BIOS, used as a reference.

* **`_patch_pad` (`0x800A1540`, `patch.o` `+0x28`).** Calls
  `EnterCriticalSection` (its callers leave the section), reads B0-table
  entry 0x5B (the address of the kernel's `ChangeClearPad`) through
  B(57h), stores that + 0x884 at 0x800EA8F8 and + 0x894 at 0x800EA8FC,
  zeroes 11 instruction words of the kernel at + 0x594 and calls
  `FlushCache`. In the SCPH-1001 kernel the 11 words are the control-register
  write and wait in the failure path of the per-port pad read; `_Pad1`
  performs that pair instead.
* **`EnablePAD` (`0x800A1518`)**, **`DisablePAD` (`0x800A152C`)**
  (`patch.o` `+0x00`, `+0x14`). Each jumps to an address `_patch_pad` saved,
  0x800EA8F8 and 0x800EA8FC: in SCPH-1001 the two routines set and clear the
  kernel's pad-enable flag.
* **`_remove_ChgclrPAD` (`0x800A15B8`, `chclrpad.o`).** Inside a critical
  section it zeroes the 9 instruction words at `ChangeClearPad` + 0x62C and
  calls `FlushCache`. In SCPH-1001 those words acknowledge the vertical-blank
  interrupt in I_STAT; after the patch the kernel no longer does. Callers:
  `PAD_init` and `InitPAD`.

libcard has a member of the same name, `patch.o` at `0x800A1658`. Its stubs
`func_800A169C`, `func_800A16C8` and `func_800A170C` are kernel patch code
that `_copy_memcard_patch` and `_patch_card` form the address of, not
functions ([psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md)).

### first.o (`0x800A3638` to `0x800A38D7`)

`D_800EA9C0` holds the device's own firstfile function while the hook is in
place and `D_800EA9C8` is a device-name buffer. `firstfile2` (`0x800A38D8`,
`a66.o`) is the BIOS B(42h) stub.

* **`firstfile` (`0x800A3638`).** `firstfile(name, dir)` copies the leading
  characters of `name` above `':'` (a signed compare) into the name buffer
  (`"bu00:..."` gives `"bu"`); walks the kernel's device table (address at
  0x150, size in bytes at 0x154, 0x50-byte records) for the device of that
  name by `strcmp`; saves its `dt_firstfile` (`+0x34`) in `D_800EA9C0` and
  installs `func_800A37D4` in its place; returns the BIOS firstfile's result
  for the same two arguments, or 0 if no device has the name. Its one caller
  is `MemCardGetDirentry` (`0x800A2AA0`).
* **`func_800A37D4`** (static). The installed hook, called by the BIOS
  firstfile with the file's I/O block and two more arguments: sets the
  block's flags word (`+0`) to 1 when it is 0, puts the device's own
  firstfile back into the device table, calls it with the three arguments
  unchanged and returns its result.

## Open ends

* `+0x60` (`vol`): the header has `s16`, and the functions that use the
  array do not settle the original sign.
* `+0x4C` (`unk_4c`) and `+0xA4` (`unk_a4`): only zeroed, never read; their
  role is not known.
* Flag 0x100: only `_SsSndNextSep` clears it and nothing sets it; its role is
  not known.
* Flag 0x400 with `rewind_pos` / `end_pos` reads as `SsSeqPlayPtoP`'s start
  and end points from the API's description ("plays the interval between two
  points"); `SsSeqPlayPtoP` is not in the image, so no caller confirms it.
* Flag 0x20: `_SsSndCrescendo` is called for it but clears only 0x10. What
  would set 0x20, and what would end a ramp started under it, is not in the
  image.
* `_SsContDataEntry`: the dead comparison in the RPN 1 and RPN 2 arms stands
  for a source expression that cannot be recovered (several give the same
  bytes).
* `_SsSetNrpnVabAttr11` to `14` rebuild the tone's ADSR words from stack the
  handler did not fill, and `_SsContDataEntry`'s NRPN path passes an
  uninitialised `VagAtr`. Whether any SEQ in this game reaches them is not
  known.
* `D_800EA6F0` `unk_03`: `_SsVmKeyOn` writes 0 and nothing reads it; what
  the library keeps there is not known.
* `D_800E9EC0` `unk_20` to `unk_26` and `unk_2c` to `unk_32`: eight shorts
  that `_SsVmInit` clears and nothing in the C reads. Their role is not
  known.
* `auto_vol`, `auto_pan`, `D_800EA5A0` and `D_800EA418`: only their use as
  the flush's gates and callbacks is read. Nothing in this program sets
  them, so what the callbacks would do is not known.
* `_SsVmKeyOnNow`'s parameter types: the C declares `(short, short)` and
  never reads the first; the library's own types are not settled.
* `_SsVmSetVol`'s three constructs stand for an original construct that is
  not known ([psyq/codegen.md](psyq/codegen.md)).
* `_SsVmSetSeqVol`'s fourth argument: every caller declares and passes one
  (1); the definition has three parameters, and the object's bytes are
  reproduced with three. Whether the library's function takes a fourth is
  not settled.
* `_spu_gcSPU`'s first pass has an inner loop with a constant condition,
  `for (j = i + 1; 1; j++)`. The C marks it as a spelling whose original
  construct is not known.
* `func_800A37D4`'s second and third arguments, which the BIOS firstfile
  passes and the hook hands on unchanged, have no recorded name or meaning.
* `func_8008FAB0` passes `m` to `SetColorMatrix` cast to `const int *`,
  where libgte.h's prototype takes `MATRIX *`; which type the library's own
  source passed is not settled.
* The roles of the kernel words `_patch_pad` and `_remove_ChgclrPAD` zero, and
  of the two routines `EnablePAD` / `DisablePAD` jump to, rest on a reading
  of the SCPH-1001 kernel; the program's own bytes show only the offsets.

## The pages beside this one

| Page | What it covers |
|---|---|
| [psyq/index.md](psyq/index.md) | Every library function by address, with its name, member, file and recipe markers; the routines that are still assembly, and why. |
| [psyq/codegen.md](psyq/codegen.md) | How the library objects were compiled, which recipe reproduces them, and which shapes of C decide their bytes. |
| [psyq/libgpu-libgs.md](psyq/libgpu-libgs.md) | libapi's critical section and second heap, libc2's string and memory functions, most of libgpu's `sys.o`, libgpu's `LoadClut` and libgs's `gs_001.o`. |
| [psyq/libcd.md](psyq/libcd.md) | libcd's `event.o` (`CdInit` (`0x800902F8`)), `toc.o` (`CdGetToc2` (`0x800900CC`)), `sys.o`, `bios.o`'s sector DMA and interrupt handler, `cdread.o`'s first two functions, the stream library (`cdrom.o`, `cdread2.o`, `c_002.o` to `c_011.o`) and `s_002.o`; the libgs, libgte, libc2 and libetc members that lie among them. |
| [psyq/libetc-libsnd.md](psyq/libetc-libsnd.md) | libetc's `vsync.o`, `intr.o` and `intr_dma.o`; libc2's `setjmp` and `longjmp`; libsnd's close, reset and open, the handler table `SsFCALL`, the control-change event, tone attributes 11 and 12, parts of the voice manager, mono and stereo, the VAB header's entries, the reverb type, the damper, play, pause and stop, and the size of the tick setting. |
| [psyq/libspu.md](psyq/libspu.md) | libspu's `spu.o`, `s_ini.o`, the allocator, `SpuSetKey` (`0x800A01D8`), `SpuWrite` (`0x800A0398`) and `SpuClearReverbWorkArea` (`0x800A0028`); libsnd's `SsVabFakeHead`. |
| [psyq/libapi-libcard-libmcrd.md](psyq/libapi-libcard-libmcrd.md) | libapi's `PAD.OBJ`, libcard's objects (the card driver's start and the kernel patches), libmcrd's objects (the `MemCard*` functions) and libc2's `strcat`. |
| [psyq/libpad.md](psyq/libpad.md) | libpad: its seven members, `0x800A4CF8` to `0x800A7D97`, how it serves the two controller ports, what this game runs, its hooks and state, the port record, a section per member, and libc2's `strncpy`. |
| [libsnd-map.md](../libsnd-map.md) | The libsnd and libspu functions the music path reaches, as a call graph. |
| [bios-calls.md](bios-calls.md) | Where the program calls the BIOS: libapi's stubs and the members that call them. |
