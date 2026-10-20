# Lunar SSSC (PS1) — Sound & Music

This page covers the sound of the main executable (`SLUS_006.28`) and the
files it plays. It says where the music, the sound effects and the voices are
on the disc; how a sound bank (a VAB sample bank and its SEQ sequences in
`LUNADATA.FIL`) is laid out and loaded, and the VAB, SPU-ADPCM and SEQ formats
as the game's data uses them; what the game's own sound code does (the
developers' `SOUND.C`, today's `sound.c`, with its neighbours `music_play.c`,
`music_bank.c`, `music_voice.c`, `xa.c` and `cd_track.c`): the bank slots and
sequence slots, the reverb, the sound requests, and the music's load, seek,
level and fade; which music plays for a music number; the battle's sound
requests; the CD audio track; and the XA voice clips.
[audio.md](code/subsystems/audio.md) maps that code by file. Sony's side of
the path — libsnd, which reads the SEQ data and drives the voices, and libspu,
which writes the SPU — is described on [libsnd-map.md](code/libsnd-map.md)
(every library function the music path reaches, and the call graph from boot,
bank load and play down to the per-tick service),
[libetc-libsnd.md](code/subsystems/psyq/libetc-libsnd.md),
[libspu.md](code/subsystems/psyq/libspu.md) and
[psyq-libs.md](code/subsystems/psyq-libs.md) (libsnd's score record, the voice
manager and its volume law, and libspu's reverb). The XA sector format is
[xa-adpcm.md](formats/xa-adpcm.md).

The counts, lengths and censuses of the disc's data on this page were measured
with station-tools, a custom decomp toolkit that will be released later.

| | |
|---|---|
| Where | `LUNADATA.FIL`, resource sector-table indices **232–358** |
| Sectors | 4568–12670 — **16.6 MB**, the archive's largest single region |
| Contents | 127 banks · **1,029** SPU-ADPCM samples (9m 51s) · **412** sequences (1h 34m) |

---

## 1. Audio sources on the disc

Lunar has **two independent music systems**, and conflating them is the main
trap in this area:

* **Sequenced audio** — VAB banks and SEQ sequences in `LUNADATA.FIL`, played
  note by note on the SPU through Sony's libsnd by the game's `sound.c`. This
  is the music of every music number but 0x3C, and every sound effect.
* **Red Book CD audio** — track 2 of the disc, raw stereo PCM the drive plays
  into the mixer with no SPU voice involved. The game plays it for one music
  number, 0x3C (§"BGM numbers").

The voices are a third source: XA-ADPCM clips the drive streams from
`_AUDIO.XA` (§"The XA voice clips").

| Source | Format | Contents |
|---|---|---|
| **LUNADATA.FIL res 232–358** | VAB + SEQ | **All sequenced music and every sound effect** |
| CD Audio track 2 | Red Book, 44.1 kHz stereo | 2m28.6s, 25 MB: music number 0x3C (§"CD audio") |
| CD Audio track 3 | Red Book, 44.1 kHz stereo | 0m43.6s, 7 MB. No code of the executable asks for it: the one caller of `cd_track_play` (`0x8001B684`), `cd_track_switch` (`0x8001B71C`), asks for track 2 |
| `_AUDIO.XA` (38 MB) | CD-XA ADPCM | Voice clips, 16 channels interleaved (§"The XA voice clips") |
| `_SECOND.XA` (disc 2) | CD-XA ADPCM | Nine channels the game plays after its end (§"The XA voice clips") |
| `LUNAR_DA.PCM` (22 MB) | — | **All zero bytes** — see below |
| `BLANK_DA.PCM` (6.7 MB) | — | **All zero bytes** — see below |

### The `.PCM` files are empty

Both `.PCM` files read from the ISO as *nothing but zeros* — checked byte for
byte, every 4 KB block, in both files. They are not a sample bank: they are
MODE2 sector reservations that an ISO9660 (MODE1) read returns as blank, and
no C names either file. Whatever occupies those sectors has to come out of a
raw image of the disc.

---

## 2. Container: the sound-bank command list

A sound bank is an ordinary resource **command list** — the same
`[u32 type][u32 size][payload]` stream `resource_command_run` (`0x800202FC`)
walks for map data ([overview.md](overview.md) §"The command list format"). Four of
its command types are sound, and their handlers are in `sound.c`:

| Cmd | Handler | What it does |
|---|---|---|
| **13** | `sound_vab_head_open` (`0x80024314`) | VAB header (VH). Opens the next bank record, `g_SoundBanks[g_SoundBankCount]` (the heap pointer as it stands, the first SEQ index `g_SoundSeqCount`, a count of 0), copies the header to the sound heap (`sound_buf_add` (`0x80024298`)) and opens it with Sony's `SsVabOpenHead` (`0x8009D6E8`). The record keeps the VAB id and the resource being loaded (`g_SoundBankLoadResource`). An id of -1 fails the check "SsVabOpenHead : failed !!!" (line 0x363). |
| **14** | `sound_vab_body_transfer` (`0x800243D0`) | VAB body (VB). Sent from the loaded resource to SPU RAM by `SsVabTransBody` (`0x8009DBE8`) under the bank's VAB id; an answer other than that id fails the check "SsVabTransBody : failed !!!" (line 0x37C). `SsVabTransCompleted(1)` (`0x8009DCA8`) waits for the transfer, and `g_SoundBankCount` goes up by one. |
| **15** | `sound_seq_add` (`0x80024464`) | One SEQ sequence (below). |
| **17** | `sound_id_table_set` (`0x800244E4`) | Copies sixteen 16-bit sound numbers to `g_SoundIds`: the numbers the sequences of bank 1 answer to (§6, "Sound effects"). |

The three checks pass `D_800B8744` = `"SOUND.C"` to `assert_fail`
(`0x8001B050`), naming the developers' source file. `assert_fail` is empty in
this build, so a failed check reports nothing and the load goes on.

**Command 15 — `sound_seq_add(cmd, bytes, payload)` (`0x80024464`).** One
command 15 is one sequence. Unless the SEQ address table is full
(`g_SoundSeqCount >= 0x80`: the sequence is then dropped — not copied, not
counted, nothing reported):

1. `p = sound_buf_add(payload, bytes)` — the SEQ is copied in whole words to
   the sound heap at `g_SoundBufNext` (0x801EB300 at boot). An overrun past
   0x801FE2FF calls `assert_fail` with "SOUND_BUF was exhaust!" (file
   "SOUND.C", line 0x334), and the copy is made all the same;
2. `g_SoundSeqs[g_SoundSeqCount++] = p` — its address into the SEQ address
   table (0x80 words), `g_SoundSeqCount` = fill;
3. `g_SoundBanks[g_SoundBankCount - 1].seq_count++` — counted into the newest
   bank record (`+9`).

Command 14 has already counted the bank that command 13 opened with
`first_seq = g_SoundSeqCount` and `seq_count = 0`, hence the `- 1`: a bank's
sequences are the `seq_count` table entries from its `first_seq`, and
`sound_seq_play(bank, k, slot)` (`0x80023B24`) opens
`g_SoundSeqs[first_seq + k]`. A SEQ ahead of any bank is not guarded against
(it would increment the byte at 0x800E2CC5, just below the table). No RNG
draw, no frame wait. Arithmetic on the table below, not checked against what
is ever loaded together: 23 (resource 232) + 96 (resource 358) + 9 (the
largest other bank) = 0x80, the table's size.

The VAB header and the sequences stay in the sound heap for as long as their
bank is loaded; the VAB body is not copied: `SsVabTransBody` sends it from the
load buffer to SPU RAM.

So a bank reads:

```
[cmd 13][size][ VH bytes  ]      VAB header
[cmd 14][size][ VB bytes  ]      VAB body — raw SPU-ADPCM
[cmd 15][size][ SEQ bytes ]      ONE sequence; a bank of n sequences has n of these
...
[0]                              zero word ends the list; sector tail is padding
```

Every bank is self-consistent, which is how the layout was confirmed rather
than assumed:

* the **cmd 13 size equals the VH size** implied by the header's own program
  count — `32 + 2048 + 512·ps + 512` — for all 127 banks;
* the **cmd 14 size equals the sum of the VAG size table** for all 127;
* the parsed VAG count equals the header's declared `vs` for all 127;
* all 412 `SEQp` chunks in the whole archive fall inside these 127 banks, and
  the 127 resource spans are exactly contiguous, sectors 4568–12670.

### How the banks are distributed

What each resource is for follows from the code that loads it (§6 and
§"BGM numbers"):

| Resources | Bank slot | What they are |
|---|---|---|
| 232 (0xE8) | 0 | the common bank, loaded at boot (`sound_common_load` (`0x80023E00`)) |
| 233–293 | 2 | the music banks, one per music number (`sound_music_load`, §"BGM numbers") |
| 294–357 | 1 | the areas' banks (`sound_area_load`, `g_SoundAreaBankPairs`) |
| 358 (0x166) | 1 | the battle's bank ([battle.md](code/subsystems/battle.md) §"The sound in battle") |

By size:

| Sequences per bank | Banks | Which |
|---|---|---|
| 1 | 79 | one long sequence each, 40–150 s: the 61 music banks and 18 of the area banks |
| 2–9 | 46 | area banks, resources 294–356 |
| 23 | 1 | resource **232**, the common bank |
| 96 | 1 | resource **358**, the battle's bank |

---

## 3. VAB — the sample bank

The struct layout is Sony's `libsnd.h` (`VabHdr`, `ProgAtr`, `VagAtr`), one of
the PSY-Q headers the build fetches ([BUILD.md](../../BUILD.md) §"Sony's
headers"), which is the authority on field widths.

```
VabHdr    32 bytes           form 'pBAV', ver, id, fsize, ps, ts, vs, mvol, pan
ProgAtr   16 × 128           always 128 slots, whatever `ps` says
VagAtr    32 × 16 × ps       16 tone slots per *declared* program
u16       × 256              VAG sizes in 8-byte units; entry 0 is a dummy
```

The magic on disc is the four bytes `70 42 41 56` — `pBAV`. That is `'VABp'`
stored as a little-endian `long`, which is what the Sony header declares; a
reader that expects the literal text `VABp` finds nothing.

Two fields are easy to get wrong, and third-party rippers commonly do:
`VagAtr.prog` and `VagAtr.vag` are **signed shorts**, and the eight bytes
after them are `reserved[4]` — *not* a tone-name string.

A tone's vibrato and portamento fields (`vibW`, `vibT`, `porW`, `porT`) are
copied by libsnd's tone-attribute functions (`SsUtGetVagAtr`, `SsUtSetVagAtr`,
the NRPN handlers for attributes 13 and 14) and read by no voice code: the
libsnd the game links plays no vibrato and no portamento.

libsnd finds a program's tones by the program's place among the VAB's
programs that have tones, not by the program's number:
`_SsVabOpenHeadWithMode` (`0x8009D7F8`), which `SsVabOpenHead` calls, writes
that place into each program's `ProgAtr.reserved1`, and the voice manager uses
it as the row of the tone table (`fake_prog` in `_svm_cur`, `psyq_private.h`).
Every Lunar VAB has its used programs contiguous from 0, so here the place and
the number agree.

### Lunar's VAB values

| Field | Value across the 127 banks |
|---|---|
| `ver` | 7 |
| `id` | 0 |
| `ps` (programs) | 6–23 |
| `vs` (samples) | 4–54, 1,029 total |
| `mvol` | 127 (SFX bank), 80 (most music) |
| `pan` | 64 |

### Sample rate

A VAG carries no sample rate — pitch is a property of the *tone* that plays
it. The pitch a tone sounds at when struck on its own centre note comes from
libsnd's `SsPitchFromNote` (`0x8009B71C`), which `note2pitch` (`0x8009B668`)
calls with the tone's centre note and its fine tune `shift` (in 1/128ths of a
semitone, taken as 127 when it is larger). With the note equal to the centre,
the semitone part is 0 and the octave −2, which leaves the fine-tune ratio
from `vm_n2p.o`'s table of 128 ratios `0x8000 · 2^(k/1536)`: the pitch is
about `0x1000 · 2^(shift/1536)`. Pitch `0x1000` is 44100 Hz, so

```
rate ≈ 44100 × 2^(shift / 1536)
```

which is 44100 Hz for `shift == 0`. The largest `shift` of any tone in the
game's banks is 122, which gives about 46.6 kHz.

---

## 4. SPU-ADPCM

16-byte blocks: a shift/filter byte, a flag byte, then 28 samples packed two
nibbles per byte, low nibble first.

```
sample = sign_extend((nibble << 12)) >> shift
sample += (hist1 · filter_pos + hist2 · filter_neg + 32) >> 6
```

with the hardware coefficient table `pos = 0, 60, 115, 98, 122` and
`neg = 0, 0, -52, -55, -60`, and the result clamped to signed 16 bits. The
SPU decodes it in hardware; no code of the game reads these blocks.

### The terminator block must not be decoded

Flag bits are `1 = loop end`, `2 = loop repeat`, `4 = loop start`. Decoding
has to **stop after the block that sets loop-end**, which is where the SPU
stops or jumps back.

This is not a detail. 704 of the 1,029 samples are followed by the encoder's
canonical one-shot end marker:

```
00 07 77 77 77 77 77 77 77 77 77 77 77 77 77 77
```

Shift 0 and nibble 7 decode that to a burst at **±28672** — a full-scale
click on the end of every one-shot sample. Hardware never plays it: the block
exists so the SPU's repeat address has somewhere silent to point.

Flag census across all 1,029 samples:

| Flags | Blocks | Meaning |
|---|---|---|
| 0 | 510,824 | ordinary |
| 2 | 418,055 | ordinary, inside a loop region |
| 4 | 704 | loop start (one-shot samples) |
| 6 | 325 | loop start (looping samples) |
| 1 | 704 | end, no repeat |
| 3 | 326 | end + repeat — a real loop |
| 7 | 704 | the one-shot terminator above |

giving **325 looping and 704 one-shot** samples. Judging by `loop start`
alone would call all 1,029 of them looping; the *terminating* block's flags
are what decide.

### On bit-exactness

Implementations disagree about the last bit of the predictor sum: ffmpeg's
`adpcm_psx` truncates toward zero, PSY-Z's SPU code rounds each product
separately, and psx-spx describes rounding the sum to nearest. Against ffmpeg
over 85 samples the residual of a round-to-nearest decode is **20 RMS,
−64 dBFS** with no structural divergence, and across all 1,029 samples the
four candidate roundings clip 23–30 times out of 26 million — statistically
indistinguishable. The choice is inaudible, and none of them can be called
bit-exact without hardware to compare against.

---

## 5. SEQ — the sequences

```
'pQES'          4 bytes — 'SEQp' as a little-endian long
version         u32 big-endian   (always 1)
resolution      u16 big-endian   (always 480 ticks per quarter note)
tempo           u24 big-endian   microseconds per quarter note
rhythm          2 bytes
events          MIDI event stream
```

Every multi-byte field is **big-endian**, unlike the VAB header beside it.
libsnd's `_SsInitSoundSeq` (`0x80097C48`), which `SsSeqOpen` calls, reads the
header this way: a first byte of `'S'` or `'p'`, byte 7 equal to 1, the
resolution from bytes 8–9, the tempo from bytes 10–12 (turned into quarter
notes per minute, rounded to nearest), the rhythm bytes 13–14, and the first
event after them. The event stream is MIDI: variable-length delta times,
running status, and the ordinary channel voice messages.

### Meta events carry no length byte

This is the one place SEQ departs from MIDI, and getting it wrong loses the
rest of the sequence. `FF 51` is followed by three raw tempo bytes, *not* by
`03` and then the tempo: `_SsGetMetaEvent` (`0x80097878`) takes the three
big-endian bytes straight after the type. Read the MIDI way, the length is
taken from the first tempo byte (`0x0d` = 13 in the case that exposed it) and
the stream desynchronises.

Exactly two meta types occur across all 412 sequences:

| Meta | Count | Payload |
|---|---|---|
| `FF 2F 00` | 412 | end of track — one per sequence, all 412 terminate cleanly |
| `FF 51 tt tt tt` | 103 | set tempo, three raw bytes |

Because the format is MIDI with a different header, a sequence becomes a
format-0 standard MIDI file by making the header tempo a leading Set Tempo
event, giving the meta events their length byte back and expanding running
status; the results load in ordinary MIDI software.

### What the sequences use

Tempos range 70–220 BPM. Resolution is 480 ticks per quarter throughout. The
long BGM sequences run 12–13 channels with a program change per channel. The
controllers used are only 6, 7, 10, 11 and 99, beside 9,823 pitch bends;
[libsnd-map.md](code/libsnd-map.md) §"What the game's SEQ data uses" has the
counts, and the handlers the music never reaches.

Controllers 99 and 6 are only ever the loop markers: **68 sequences** loop
with `99=20, 6=127 … 99=30` (NRPN loop start, an endless count, loop end). The
game plays every SEQ with `SsSeqPlay(seq, 1, 1)` — one pass (`sound_seq_play`,
§6) — so that NRPN loop is the only thing that loops a piece.

libsnd steps a score once per vertical blank, in tenths of a tick, against
`VBLANK_MINUS` = 60, and keeps the tempo in whole quarter notes per minute
(rounded at open, truncated at a tempo event); [libsnd-map.md](code/libsnd-map.md)
§"The tick". Worked out over the 412 sequences at 59.94 Hz, that plays them
between **−0.53 %** and **+0.89 %** of their header tempo (median +0.12 %;
BGM 1 +0.07 %).

### One real data inconsistency

Bank **248** defines 5 programs, but its sequence sends a program change to
program 5. No bank's `ProgAtr` table claims tones beyond its own `ps`, so
those 64 note-ons fall on a program with no tones, and libsnd keys only the
tones a program holds (`_SsVmSelectToneAndVag` (`0x8009CEE8`)): they sound
nothing. The sequence asks for something the bank does not contain.

---

## 6. The game's sound code — banks, slots and sounds

### Boot — `sound_init()` (`0x80023D54`)

One caller, `system_init` (`0x8001EEB8`) (frame.c), once, after `SsInit` and
`card_init` (`0x80024E3C`) and just before `xa_file_set` (`0x8002347C`) names
"_AUDIO.XA". In order:

1. `SsSetTableSize(g_SoundScoreTable, 16, 1)`: libsnd's score table. 16
   sequences × 1 track × 176 bytes = 0xB00 bytes, so `g_SoundScoreTable` is
   the block from 0x800E2EF8 up to `g_SoundSeqSlots` (the sixteen sequence
   slots).
2. `SsSetTickMode(SS_TICK60)` (1).
3. `g_SoundBankCount = 0` (banks loaded) and `g_SoundBufNext = 0x801EB300`,
   the sound heap's allocation pointer at its start. `sound_buf_add` checks
   for an overrun past 0x801FE2FF: the heap is 0x801EB300..0x801FE2FF,
   0x13000 bytes.
4. The three bank records `g_SoundBanks[0..2]` are emptied: heap pointer 0,
   VAB id −1, resource −1, first SEQ index 0, SEQ count 0. Bytes `+0xA` and
   `+0xB` are never written.
5. `sound_seq_init()` (`0x80023978`): `g_SoundSeqs` (0x80 SEQ addresses)
   zeroed, `g_SoundSeqCount = 0`, `g_SoundSeqSlots[0..15] = -1`,
   `g_SoundSeqNextSlot = 1`.
6. `sound_reverb_set(4, 0x30)` (`0x800241F4`) (§"The game's reverb").
7. `SsStart2()`: the sequencer's tick goes onto the vertical blank.
8. `SpuSetReverbModeDepth(0x2800, 0x2800)`, which replaces the depth step 6
   set.
9. `SpuSetCommonMasterVolume(0x3F80, 0x3F80)`.

It reads nothing; no RNG draw and no frame wait of its own.

### The bank table and its three slots

`g_SoundBanks` (0x800E2CC8) holds three 12-byte `SoundBank` records
([sound.h](../../decomp/SLUS_006.28/include/sound.h)): `+0` the heap pointer
as it was when the bank was opened (`buf_start`), `+4` the VAB id, `+6` the
LUNADATA resource, `+8` the first index into the SEQ address table
`g_SoundSeqs`, `+9` the SEQ count. `g_SoundBankCount` counts the records in
use. The slots are fixed by position, and the code counts on it:

* **slot 0**, the common bank, resource 232: `main` (`0x80013D2C`) calls
  `sound_common_load()` (`0x80023E00`) and right after it
  `sound_bank_empty_fill(1)` (`0x8002415C`), so slot 0 is in use from boot on
  whether or not that load opened a bank;
* **slot 1**, the area's bank (below);
* **slot 2**, the music's bank (§"BGM numbers").

`sound_bank_load(r)` (`0x80024138`) is `g_SoundBankLoadResource = r;
resource_load_run()` (`0x800202CC`), with `r` still in the first argument
register: the resource's command list is read from the CD and run, which
opens the VAB and fills the next bank record. The CD read and its waits are
under it.

**Dropping banks — `sound_bank_close(bank)` (`0x80024064`).** Every sequence
slot playing from that bank or above is stopped (`sound_bank_seq_stop`
(`0x80023A84`), two frames after each), and when the bank is the last one
loaded (`g_SoundBankCount == bank + 1`): its VAB is closed (`SsVabClose`
(`0x8009D638`); VAB id and resource become −1, both only when the id is ≥ 0),
`g_SoundSeqCount` and `g_SoundBufNext` go back to the values the record saved
at `+8` and `+0`, and `g_SoundBankCount` goes down by one.
`sound_bank_close_from(first)` (`0x800240EC`) closes bank 2, then 1, down to
`first`.

**The empty bank — `sound_bank_empty_fill(n)` (`0x8002415C`).** While
`g_SoundBankCount < n`, record `g_SoundBanks[g_SoundBankCount]` gets
`buf_start` = `g_SoundBufNext` (the heap pointer as it stands), `first_seq` =
`g_SoundSeqCount` (the next free entry of `g_SoundSeqs`), `seq_count` = 0, VAB
id −1, resource −1, and `g_SoundBankCount` goes up by one. Such a record owns
no heap and no sequences: `sound_seq_play` starts nothing from it (`k <
seq_count` fails), and dropping it closes no VAB and puts back the two values
it saved. `main`'s call with 1 is the only one. With `g_SoundBankCount >= n`
it writes nothing. It has no upper bound of its own (`sound_init` empties
three records, 0x800E2CC8..0x800E2CEB). No callee, no RNG draw, no frame
wait.

**The area's bank — `sound_area_load()` (`0x80023F78`).** The resource is
`sound_pair_find(g_SoundAreaBankPairs, sound_area_get(map))`:

* `sound_area_get(n)` (`0x80024784`) is byte `n` of `g_SoundMapAreas` for a
  map number below 0x9A, and `n` itself otherwise. The C calls it with no
  argument, so it reads what the caller left in the first argument register:
  `SoundMapSet(map)` (`0x800555CC`) passes the low byte of its own argument
  through `sound_area_load_copy` (`0x800245B0`). `MapSet` (`0x80068780`) and
  `MapReload` (`0x800686EC`) call `SoundMapSet` with the map's number,
  `MapBtlRun` (`0x8007AE58`) with 0 before a battle, `StartScreenShow`
  (`0x80043C64`) with 1.
* `g_SoundAreaBankPairs` is a list of `{area, resource}` 16-bit pairs ended by
  a pair whose resource is 0; `sound_pair_find` (`0x80023E5C`) returns the
  resource of the first pair whose area matches, or that of the last pair
  when none does. The pairs run 2 → 294 … 199 → 357, 999 → 357.

When `g_SoundBankCount < 2` or `g_SoundBanks[1].resource` is another
resource: `sound_bank_close_from(1)` and `sound_bank_load(resource)`. Its
seeking half, `sound_area_seek()` (`0x80023FE0`), makes the same test and
then only sends the drive there (`resource_seek`, below) and answers 1;
otherwise 0. In battle, slot 1 holds resource **358**: `MapBtlRun` sends the
drive ahead with `sound_btl_seek` (`0x80024044`) = `resource_seek(0x166)` and
then calls `SoundMapSet(0)` ([battle.md](code/subsystems/battle.md) §"The sound
in battle").

### Sequence slots — `sound_seq_play(bank, k, slot)` (`0x80023B24`)

`g_SoundSeqSlots` is sixteen words, each the libsnd access number of the
sequence open on that slot or −1; `g_SoundSeqSlotBanks` holds, for each slot,
the bank it plays from. Slot 0 is the music's. A sound effect plays on the
slot its request names (§"Sound effects").

`sound_seq_play` starts SEQ `k` of loaded bank `bank` on sequence slot
`slot`. Nothing at all happens unless `bank < g_SoundBankCount` and `k <` the
bank record's `seq_count`. Then:

1. `sound_seq_stop(slot)` (`0x80023A20`): `SsSeqStop` and `SsSeqClose` of what
   the slot held, and the slot is set to −1;
2. `SsSeqOpen(g_SoundSeqs[first_seq + k], vab_id)` — its result is never
   tested;
3. `SsSeqSetVol(seq, 0x50, 0x50)`;
4. `SsSeqPlay(seq, 1, 1)` (`SSPLAY_PLAY`, one pass);
5. `g_SoundSeqSlots[slot]` = the access number, `g_SoundSeqSlotBanks[slot]` =
   the bank;
6. slot 0 (the music): `sound_music_level_apply()` (`0x800246B0`) applies the
   music's level in place of step 3's 0x50 (§"The music's level").

The SEQ is already in RAM: no frame wait, no CD call, no RNG draw anywhere on
its call tree. A `slot` below 0 would mean the rotating slot
`g_SoundSeqNextSlot`, which `sound_seq_slot_step` (`0x800239D0`) moves round 1
to 15 after such a play; the one function that passes −1,
`sound_seq_rotate_play(bank, k)` (`0x80023CEC`), is called by nothing in the
image, so the rotation is dead code as shipped. The other entries:
`sound_music_play()` (`0x80023D0C`) is `sound_seq_play(2, 0, 0)` and
`sound_music_stop()` (`0x80023D34`) is `sound_seq_stop(0)`.

### Sound effects — `SoundPlay(id, ch)` (`0x800555F0`)

Every sound effect, in the field and in battle, goes through `SoundPlay`
(music_voice.c, 136 calls in the image). It turns a sound number into a kind
and a sequence number for `sound_effect_play(ch, kind, n)` (`0x80024514`) and
plays it on sequence slot `ch`, a byte, so never the rotating slot.
`sound_effect_play` sends kind 1 to bank 0 (`n < 0x16`), kind 2 to bank 1
(`n < 0x60`) and kind 3 to bank 1 (`n < 8`); any other `n` plays nothing.

| `id` | Kind, sequence |
|---|---|
| 0 | stop slot `ch` (`sound_seq_stop_copy` (`0x80024590`)) |
| 1..0xA | kind 1, `id − 1` |
| 0xB..0x82, while `g_SoundMapNumber` is 0 or 0x32 | 0x82: kind 2, 1; 0x18: kind 1, 0x17; 0x1F and up: kind 2, `id − 0x1F`; the rest refused |
| 0xB..0x82, any other map number | below 0x1F: kind 1, `id − 1`; the rest refused |
| 0x83..0x12A | kind 3, the place of `id` among the sixteen numbers of `g_SoundIds` (command 17), only while `g_SoundMapNumber` is not 0; a number not in the table plays nothing |
| 0x12B and up | nothing |

A refused number plays kind 1, sequence 1. A kind-1 sequence number from 9 to
0x19 is lowered by 2 before it is played. `SoundPlay` answers 1, or 2 for a
number it refused. `g_SoundMapNumber` is the map number `SoundMapSet` was last
given: 0 in battle, so there ids 0x1F and up play the battle bank's sequence
`id − 0x1F`. [npc-scripting.md](code/subsystems/npc-scripting.md) §"The item /
number notice — frames and layout" follows sounds 0xB and 0xC through this
table: kind 1, sequences 8 and 9 of bank 0, and refused while the map number
is 0 or 0x32.

### The music's level

`sound_music_volume_set(level)` (`0x80023C4C`) is `SsSeqSetVol(slot 0, v, v)`
with `v = level × 0x50 / 0xFF`, at most 0x50; libsnd applies it to the
playing score through `_SsVmSetSeqVol`. `g_SoundMusicLevel` (0..0xFF) is the
level now. `sound_music_level_set(0, level, time)` (`0x800246D4`) sets it at
once for `time` 0, and otherwise arms a fade from the level now to `level`
over `time × 30` steps, which `sound_music_level_step` (`0x800245F0`) takes
one at a time: `MapFrameEnd` (`0x8005596C`) and `BtlFrameStep` (`0x80038D2C`)
call it once a frame. The music code's and the script's requests built on it
are on [audio.md](code/subsystems/audio.md).

Two settings outside the sequences: `SoundMonoSet` (`0x800550D4`) calls
`SsSetMono` or `SsSetStereo` by the options' mono byte; and the SPU's master
volume is 0x3F80 from `sound_init` until the CD audio track is first played,
when `cd_track_play` sets 0x3FFF, which nothing sets back.

---

## 7. The game's reverb

**What the game sets.** `sound_init` (`0x80023D54`), the one caller, calls
`sound_reverb_set(4, 0x30)` (`0x800241F4`), and `sound_reverb_set(type,
depth)` is, with `d = (s16)(depth << 8)`:

    type >= 0:  SpuSetReverbModeType(type | 0x100);
                SpuSetReverbModeDepth(d, (s16)(d | 0x8000));
                SpuSetReverb(1);
    type < 0:   SpuSetReverbModeDepth(d, (s16)(d | 0x8000));

So it sets **`SPU_REV_MODE_STUDIO_C`** (mode 4; `0x100` = clear the work
area) with depths 0x3000 and 0xB000 — read as signed 16-bit, a right side
phase-inverted at −0x5000. That is not the depth the game runs at: after
`SsStart2`, `sound_init` calls `SpuSetReverbModeDepth(0x2800, 0x2800)`
itself, before any bank is loaded. `SpuSetReverbModeDepth` (`0x800A1038`)
stores both arguments to the SPU's reverb volume registers (+0x184/+0x186)
and to `_spu_rev_attr.depth`, so the game's reverb depth is **0x2800 /
0x2800, in phase**. No SEQ sends controller 91 (reverb depth), so the music
never changes it.

**Where the preset comes from.** Sony's `SpuSetReverbModeType` (`0x800A0E68`,
libspu `s_srmt.o`) takes the mode's record from `_spu_rev_param` =
`D_800B8170` (10 records × 0x44: a u32 mask and the 32 reverb registers SPU
+0x1C0..+0x1FE in hardware order) and its work-area start from
`_spu_rev_startaddr` = `D_800B8140` (10 × u32 `mBASE`, 8-byte units). It
copies record `mode` to the stack, zeroes the mask (= write every field),
zeroes the reverb depth, and calls `_spu_setReverbAttr` (`0x8009FAF8`,
libspu `s_sra.o`), which writes field i to `_spu_RXX + 0x1C0 + 2i`;
[psyq-libs.md](code/subsystems/psyq-libs.md) §"libspu — reverb" has the
register names. STUDIO_C's record starts
`00E3 00A9 6F60 4FA8 BCE0 4510 BEF0 A680 ...`; its `mBASE = 0xF204` puts the
work area at 0x79020, 0x6FE0 bytes.

**Which voices feed it.** `_SsVmKeyOnNow` (`0x8009BEB8`) turns a voice's
reverb on when its tone's `VagAtr.mode` has bit 2 (`mode & 4`; `vmNoiseOn`
(`0x8009B848`) does the same for the noise tone). 886 of the 13,456 tones
across all banks set it — and they are the music's instruments: 72–100 % of
the notes in the first BGM tracks are sent.

---

## BGM numbers — what plays for music number n

`MusicPlay(n, _, time)` (`0x80054F2C`) (music_play.c) is the one "play BGM n"
entry; field scripts (`ScriptCommandMusicPlay` (`0x8005E494`)), battle code,
map entry (`MapSet` (`0x80068780`) → `MapMusicSet` (`0x80058EE4`)) and the
jukebox call it:

| n | plays |
|---|---|
| 0 | stop sequence slot 0 (`sound_music_stop` (`0x80023D34`)) |
| ≥ 0x45 | nothing (the answer is 2) |
| **0x3C** | **CD audio track 2**, through `cd_track_switch` (`0x8001B71C`): a pause first (`cd_track_switch(0)`, `CdlPause`) when `g_MusicTrackPlaying` says the track was started, then `cd_track_switch(2)` → `cd_track_play(2)` (`0x8001B684`) (§"CD audio") |
| other | `MusicLoadStart(n, time)` (`0x80055118`): `m = g_MusicBankNumbers[n − 1]`; `sound_music_load(m)` (`0x80023E9C`): m ≥ 0x44 → 0x41, **bank = m − 1 − #(`g_SoundMusicIndexSkips` entries < m) + 0xE9**, loaded into bank slot 2 (`sound_bank_load` (`0x80024138`) → `resource_load_run` (`0x800202CC`)), and its **sequence 0** played (`sound_music_play` (`0x80023D0C`) → `sound_seq_play(2, 0, 0)` (`0x80023B24`)) |

`MusicPlay` has a second arm for the last row: when the empty function
`func_80055468` answers 1 and `n` is not the piece kept in
`g_MusicKeptNumber`, it loads the bank itself (`MusicBankLoad` (`0x80055420`)),
runs the fade loop (`MusicFadeStep` (`0x80055214`), with script or battle
frames meanwhile) and starts the piece with `MusicStart` (`0x800551C8`).
`func_80055468` sets no result, and in the built program `$v0` holds 0x3C at
that test (the register's row for `func_80055468`), so that arm is never taken
and every sequenced piece goes through `MusicLoadStart`. `MusicLoadStart` sets the level
(the fade of `MusicFadeStep(1, time)`, or `g_MusicKeptLevel` at once for
`time` 0), keeps `n` in `g_MusicKeptNumber`, loads the bank through
`MusicBankLoadCopy` (`0x80055400`) and `MusicBankLoad` (`0x80055420`), calls
the empty `func_80024734(0, 0)`, starts sequence 0, and sets the level again
the same way.

`g_MusicBankNumbers` (u8[0x44]) is the identity except 28, 29, 50, 53, 56 → 8
and 60 → 3; `g_SoundMusicIndexSkips` = {25, 28, 50, 53, 56, 60} — numbers with
no bank of their own (`sound_music_index_get` (`0x80023E20`) counts the
leading entries below m). Result: n = 1..24 → banks 233..256; 25, 26 → 257;
27 → 258; 28/29/50/53/56 → 240 (n 8's); 30..49 → 260..279; 51, 52 → 280,
281; 54, 55 → 282, 283; 57..59 → 284..286; 61..67 → 287..293; **68 → 291**
(the clamp: the same bank as 65). Bank 259 is reached by no number.
`sound_music_load` (`0x80023E9C`) has exactly one call site in the image: the
wrapper `MusicBankLoad` (`0x80055420`) (music_bank.c, `void f(u8 m) {
sound_music_load(m); }`, the bank number narrowed to a byte), itself called
from `MusicPlay` (`0x80054F2C`) and `MusicBankLoadCopy` (`0x80055400`).

**The seek ahead — `sound_music_seek(n)` (`0x80023F0C`).** A scene change
sends the CD head on its way before anything is loaded. `MapSeek(map, music)`
(`0x8006891C`) (map_load.c) asks in order: `sound_area_seek()` (`0x80023FE0`)
for the area sound bank (bank slot 1); then, when `music != 0`,
`sound_music_seek(g_MusicBankNumbers[music - 1])` (`0x80023F0C`) for the music
bank (slot 2); then the map's own file (`MapResourceFind(map)` (`0x80020CD0`)
→ `resource_seek`). It returns at once with the 1 of the first bank seek that
was needed. A `music` of 0xFF is first replaced by byte 15 of the map's
16-byte record `MapTblGet(map)` (`0x80068618`), and a 6 becomes 7 when that
record's byte 0 is 0 and `StoryBitCheck(0x3C1)` (`0x80057778`) answers
non-zero.

`s32 sound_music_seek(s32 n)` (`0x80023F0C`) answers whether music number `n`
needs a bank other than the one in bank slot 2, and if so sends the CD head
towards it:

1. `n <= 0`: 0, nothing else.
2. `n >= 0x44`: `n = 0x41`, the same clamp as `sound_music_load` (`0x80023E9C`).
3. `r = sound_music_index_get(n) + 0xE9` (`0x80023E20`), the bank's LUNADATA
   resource by the rule in the table above.
4. `g_SoundBankCount == 3` (three banks are loaded, so slot 2 is in use) and
   `g_SoundBanks[2].resource == r` (the resource slot 2 was loaded from, the
   word at 0x800E2CE6): 0.
5. Otherwise `resource_seek(r)` and 1.

`resource_seek(r)` (`0x80020290`) (resource_load.c) is
`file_seek("LUNADATA.FIL", first sector of r << 11)` (`0x800210FC`): unless
the game reads its files from the developers' host PC (the byte at
0x8001000A), and while no XA play is running (`g_XaPlayCount == 0`),
`cd_file_seek` (`0x80021284`) does `CdControl(CdlSetloc)` +
`CdControl(CdlSeekL)` to the resource. Nothing is read. `sound_music_seek`
reads `g_SoundBankCount` and `g_SoundBanks[2].resource` and writes nothing of
its own; no RNG draw, no frame wait.

**The load — `sound_music_load(n)` (`0x80023E9C`).** One caller
(`MusicBankLoad` (`0x80055420`), the byte-narrowing wrapper). It makes
`sound_music_seek`'s test and then loads instead of seeking:

1. `n <= 0`: nothing.
2. `n >= 0x44`: `n = 0x41`.
3. `r = sound_music_index_get(n) + 0xE9` (`0x80023E20`).
4. `g_SoundBankCount == 3` and `g_SoundBanks[2].resource == r`: nothing, the
   bank is already loaded.
5. Otherwise `sound_bank_close_from(2)` (`0x800240EC`) — that is
   `sound_bank_close(2)` (§6) — and `sound_bank_load(r)` (`0x80024138`).

It reads `g_SoundBankCount` and `g_SoundBanks[2].resource` and writes nothing
of its own; no RNG draw. Its waits are those of `sound_bank_close` (two frames
for each slot it stops) and of `sound_bank_load` (the CD read).
Slot 1's "already loaded" test (`sound_area_load` / `sound_area_seek`) is
`g_SoundBankCount >= 2 && g_SoundBanks[1].resource == r`.

**Titles:** the jukebox (item "Alex's Ocarina", `MenuUseMusicRun`
(`0x80077190`)) lists numbers 1..0x44 (`MenuUseMusicListSet` (`0x80077380`))
whose story bits `0x19A + n` are set — no C sets those bits by that number —
and captions number n with string `0x546 + n` (`MenuUseMusicListDisp`
(`0x800774EC`)) — so **BGM n is named by string 0x546 + n**: 1 "Fighting
Spirits" (bank 233), 8 "Burg" (240), 60 "Thoughts Far Away" (CD track 2), 62
"Victory" (288), … Once, when story bit 0x387 is set and 0x47F is not,
`MenuUseMusicRun` instead sets 0x47F, stops the music and plays the CD track
for about 0x2F8 frames, then steps its CD volume down from 0x7BBB by 0x444 a
frame and switches it off.

**Which map plays what:** every map entry hands its area record's byte 0xF
(or the caller's number) to `MapMusicSet` (`0x80058EE4`) as a BGM number
([text.md](text.md) §"Locations"). `MapMusicSet` sorts it by `MusicKindGet`
(`0x80055448`) — kind 1 for 0x3C, 2 for 0x44 and up, 0 otherwise — and calls
`MusicPlay` for kinds 0 and 2. For 0x3C it starts nothing: the map entry
never starts the CD track. Its callers keep its answer in `g_MusicKind` and
hand it back the next time; when that kind is 1 it first calls
`MusicPlay(0, 1, 0)`, which stops sequence slot 0 (`sound_music_stop`);
nothing on this path pauses the CD track. A BGM number of 0 stops the music
and answers 0.

---

## CD audio

Tracks 2 and 3 are Red Book stereo PCM, 2352 bytes per sector, no encoding.
`cd_toc_read` (`0x8001B63C`) has Sony's `CdGetToc` fill `g_CdTocEntries`
(4 bytes a track) and keeps the track count. `cd_track_play(track)`
(`0x8001B684`) sets the master volume to 0x3FFF, the CD volume to 0x7FFF, CD
reverb off and CD mix on, sends `CdlSetmode` with `g_CdTrackMode`, and, when
the track is not past the count, `CdlSeekP` to the track's TOC entry and
`CdlPlay`. `cd_track_switch(on)` (`0x8001B71C`) is `cd_track_play(2)` for a
non-zero `on` and `CdlPause` otherwise; `MusicStop` (`0x800557AC`), which the
movie code calls, pauses it too.

The detail that matters for reading the tracks from a disc image: a track's
`INDEX 00` is its pregap and `INDEX 01` is where the audio starts. The rip
measured here is split-BIN — one file per track — with a **two-second
(150-sector) pregap at the head of each file**, so reading from byte zero yields a silent
lead-in that is not part of the music. Reading from `INDEX 01` gives 2m28.6s
on track 2 and 0m43.6s on track 3. A CD-DA track is not a file in the
filesystem: it is reachable only from the disc image and its cue sheet.

---

## The battle's sound requests

Every `SoundPlay` (`0x800555F0`) call in the battle and its UI, grouped by
what asks. Slot 3 is the battle's one effect slot: a new request replaces what
plays there, and `SoundPlay(0, 3)` stops it.

| who | id / slot | when |
|---|---|---|
| `BtlMenuFrameStep` (`0x80048964`) (the UI frame) | `g_BtlMenuSoundRequest` / 3 | the menus' sounds 1 to 4 ([menu-ui.md](code/subsystems/menu-ui.md) §"The step table") |
| `BtlFrameStep` (`0x80038D2C`) | 3 − option / 1 | the vibration and action-name toggles (options `+5`, `+6`) |
| `BtlWalkPathFollow` (`0x8002CA78`) (every walk) | the walk block's `+0x10` / 3, then 0 / 3 at its end | 0x2A for a party member, CharStats `+0x1E` for an enemy (`BtlWalkSet` (`0x8002BAA4`)); 0xFF none. A handler may set its own (`BtlShowA3Run` (`0x8003C3D8`): the action's sound) |
| `BtlAttackAnimationRun` (`0x800315A8`) | CharStats `+0x19` / 3 | the swing |
| `BtlAttackHitShow` (`0x800314B8`), `BtlTargetOutcomeShow` (`0x80041484`) | 0x38 / 3 | a hit — always 0x38 ([battle.md](code/subsystems/battle.md) §"The enemy actions": the 0x23 arm tests an address) |
| `BtlTargetBackMove` (`0x8002FD1C`), `BtlTargetMissShow` (`0x80036838`) | 0x24 (0x25 for enemy 0x3E) / 3 | the dodge |
| `BtlFallRun` (`0x800385B4`) | 0x28 / 3 | an enemy falls (not ids 0x70 / 0x76) |
| `BtlSlotLeave` (`0x8002E398`), `BtlPartyEscapeTry` (`0x80027DAC`) | 0x26 / 3, then 0 / 3 | a member, or the party, gets away |
| `BtlEscapeFailMove` (`0x80036BE4`) | 0x27 / 3, then 0 / 3 | a failed escape's stagger |
| `BtlWinRun` (`0x80028140`) | 0x1F / 3 | a level-up |
| `BtlTargetFrontMark` (`0x8003568C`) | 0x68 / 3 (four sites) | the lane stance's wind-up anims |
| `BtlRun` (`0x80027434`) | 0 / 3 | the fight ends (then the BGM, below) |
| `BtlActingShowRun` (`0x8003A158`)'s handlers | `g_BtlActionSound` (CharStats `+0x19` for an action of kind 2, else `+0x1F`) and their own ids / 3 | the enemy actions |
| `BtlShowSoundSet` (`0x80089EF8`) → `BtlShowSoundsStep` (`0x80089E34`) | id / slot + 3 | a presentation's scheduled sounds |

The shop's screen shares the request byte: `ShopStepsRun` (`0x800516CC`) plays
`g_BtlMenuSoundRequest` on slot 3, `ShopRefusalDisp` (`0x800524C4`) plays 4
and 3, and `ShopOpen` (`0x800506EC`) and `ShopClose` (`0x800512C4`) play 5
and 6 on slot 4 as the screen opens and closes.

In battle the ids from 0x1F up are sequence `id − 0x1F` of the battle bank,
358 (§6, "Sound effects"). The music around a fight is on
[battle.md](code/subsystems/battle.md) §"The sound in battle". The XA voices
the battle plays are next.

---

## The XA voice clips

Not a bank sound: `VoiceSet(n)` (`0x80055470`) / `VoiceStart` (`0x800554C4`) /
`VoiceStep(1)` (`0x80055524`) (music_voice.c) play clip ("voice", the
developers' word) n of `_AUDIO.XA` through the CD drive, and `SoundPlay`
(`0x800555F0`) never sees them.

* `VoiceSet(n)` keeps n in `g_VoiceKeptNumber`, looks the clip up
  (`xa_voice_get`, below), sends the drive `CdlSetloc` to the clip's start
  (`xa_sector_set` (`0x800236F8`)) and a status request, and waits until the
  drive answers. The start it sends is counted from the disc's first sector,
  not the file's: `VoiceSet` adds no file position. No seek or read follows
  it, and `VoiceStart`'s read carries its own position, so this changes
  nothing that plays.
* `VoiceStart` sets the clip's end (`xa_end_set` (`0x80023630`): the file's
  first sector + end) and starts it with `xa_play_start(start, channel)`
  (`0x800235C0`): `xa_mode_set` (`0x80023574`) turns CD mix on, sets the CD
  volume to 0x4000 and sends `CdlSetmode` with `g_XaMode` (0xC8: double
  speed, the drive's ADPCM playback on, the file/channel filter on); then
  `xa_read_start` (`0x80023674`) sends `CdlSetfilter` (file 1, the clip's
  channel) and `CdlReadS` at the file's first sector + start, and counts the
  play in `g_XaPlayCount`.
* `VoiceStep(1)` answers 1 while the clip plays. Once the drive reads, it
  polls `xa_end_check` (`0x80023820`) each call: that function asks `CdSync`
  without waiting, takes the drive's position from the last `CdlGetlocP`
  result and sends another, and answers 1 when the position is below the
  start or has reached the end. Then `xa_play_stop` (`0x80023518`) (CD volume
  0, `CdlPause`, CD volume 0x4000, `g_XaPlayCount = 0`) ends it and the
  answer is 0. `VoiceStep(0)` stops a clip at once.

While a clip plays (`g_XaPlayCount != 0`) `cd_file_seek` sends the drive
nowhere, and `cd_sector_load` (`0x80020E88`) and `pc_load` (`0x80021584`)
stop it first (`VoiceStep(0)`).

**Callers.** In battle: `BtlShowOpen` (`0x8007B7C8`) loads a presentation's
set-up clip (byte 2 of the effect's row `g_BtlEffectRow`, or byte 3 for
acting ids 0xB / 0x7C / 0x7D; none when byte 2 is 0), and `BtlShowVoiceStep`
(`0x80089D78`) starts it on the effect's cue and polls until it is done; `BtlActingShowRun` (`0x8003A158`)
(slot 6 in encounters 0xF5 → 0x3A, 0xF6 → 0x3B, 0xF7 / 0xFE → 0x38) starts
one and waits frame by frame; `BtlSlot5Revive(1)` (`0x80028E5C`) (Nall's
revive) loads 0x3D, starts it after his walk, and waits max(0x3C frames, the
clip). The battle asks for 17 clips in all: the set-up shouts (ids between
0x2A and 0x39), the four boss encounters' voices 0x38, 0x3A, 0x3B and
Nall's 0x3D. Outside battle: the script command `ScriptCommandVoiceSet`
(`0x8005E9FC`) (lns_intp.c) with its operand
([npc-scripting.md](code/subsystems/npc-scripting.md) has the three voice
commands), and `PictureXaPlay`
(`0x800247AC`) (picture.c), which plays voice 100..103 from its table
`g_PictureVoices` with a picture. They use the same table.

**The mapping — `xa_voice_get(n, &start, &end, &channel)` (`0x800238BC`)**
(xa.c): `g_XaVoices` is a u16 per clip (ids 0..104; `g_XaChannelEnds`
follows it).

    v = `g_XaVoices`[n]    channel = v >> 12 (0..15)   start = (v & 0xFFF) << 4
    w = `g_XaVoices`[n+1]  same channel: end = ((w & 0xFFF) - k) << 4
                         else:         end = (`g_XaChannelEnds`[channel] - k) << 4
    k = 3 if D_800AF62C[n] else 4

The file's place is the record `g_XaFile`, which `xa_file_set` (`0x8002347C`)
fills from the game's table of the disc's files (`cd_file_search` →
`cd_dir_find` (`0x80021164`)): `system_init` and `cd_dir_init` (`0x80020F98`)
pass "_AUDIO.XA" (`g_SystemAudioXaFileName`, `g_CdDirAudioXaFileName`).

**`_SECOND.XA`**, disc 2's file, is named by one function only: `GameRun`
(`0x800559F4`), the run loop's tail, which plays its channels 0..8 from the
file's first sector after the game has ended, each for a fixed frame count
([bootstrap.md](code/subsystems/bootstrap.md) §"The run loop and the end of
the game").

**On the disc:** `_AUDIO.XA` (LBA 18108, 18,640 sectors) is 16 channels
interleaved round-robin (file sector i carries channel i % 16 for the audio
stretch), every audio sector file 1, submode 0x64 (0xE4 on a channel's last),
coding 0 = mono 37,800 Hz 4-bit; the rest are empty sectors (submode 0). So
`start` / `end` are in units of 16 file sectors = one sector of each channel,
a channel at 9.375 sectors (4,032 samples each) a second at double speed — a
clip lasts (end − start) / 150 s, which is also how long the game waits. A
channel holds about `g_XaChannelEnds`[c] × 16 audio sectors (channel 13: 1,149
audio sectors against 0x48D = 1,165 units; the rest of the span is the empty
sectors). A clip ends 4 (3) units before the next one of its channel starts,
and the speech inside is often shorter than the span — several clips are
silent for their last 0.1–0.4 s (clip 0x2E: speech to 0.85 s of 1.28 s) — so
the game waits for the span, not the speech. Lengths: the set-up shouts
0.2–3.2 s, the boss voices 1.7 / 1.6 / 2.0 s, Nall 2.0 s.

**Decoding:** the drive decodes the clips in hardware. Read from a disc
image, they decode from the RAW sectors (an extracted file has lost the
Form 2 tail), with the filter state kept across the clip's sectors
([xa-adpcm.md](formats/xa-adpcm.md)). About 1% of samples sit at full scale:
the recordings are hot as shipped.

---

## Open questions

- [ ] **The `rhythm` field.** Values seen are `(1,4)`, `(1,6)`, `(6,1)` —
      recorded verbatim rather than interpreted as a time signature.
      `_SsInitSoundSeq` stores the two bytes in the score (`+0x24`, `+0x25`)
      and no C reads them back.
- [ ] What occupies the `.PCM` files' sectors in a raw image of the disc.
