# Lunar SSSC (PS1) — video and movies

This page covers the game's movies: the `.STR` files on the two discs and
their measured format, how a frame is decoded, `MoviePlay`, the routine
every movie and bromide goes through, the movie player in the `MENU.BIN`
overlay and its loop, the rumble scripts that run beside ten movies and
the RNG draws they make, and the theater and bromides that replay them.
The overlay's own map is
[code/subsystems/overlay.md](code/subsystems/overlay.md); the codec is
[formats/mdec.md](formats/mdec.md).

## The files

FMV is the majority of both discs: **409 MB across 35 files on disc 1**,
**450 MB across 14 files on disc 2**. Disc 1 ships `_B000.STR` through
`_B031.STR` and `_B033`–`_B035` (`_B032` does not exist); disc 2 ships
`_B000` plus `_B036`–`_B054`, not contiguously.

`_B000.STR` is **byte-identical on both discs** (`cmp` = 0, 35.3 MB) — it is
the front-of-disc sequence each one boots into, shipped twice rather than
referenced across discs.

## Format — measured

Measured on the files of both discs with station-tools, a custom decomp
toolkit that will be released later:

| | disc 1 | disc 2 |
|---|---|---|
| Files | 35 | 14 |
| Total | 409 MB | 450 MB |
| Resolution | **224 x 192**, every file | **224 x 192**, every file |
| Sectors per frame | 6 (x26), 12 (x8), 10 (x1) | 6 (x13), 12 (x1) |
| Sector size | 2048 B user data | 2048 B user data |
| StTYPE | 0x8001 (system-defined, MDEC) | 0x8001 |

- **224 x 192.** Read from `StWIDTH`/`StHEIGHT` in the sector header of
  every file on both discs. The player reads the same two fields: each
  frame, `movie_frame_next` (`0x80196954`) takes the header's width
  (+0x10) and height (+0x12) and sizes its two frame buffers to
  `width * 3 / 2` VRAM words by `height` rows (24-bit pixels in 16-bit
  words), and `movie_frames_play` (`0x80196414`) sets the display to
  0xE0 × 0xC0 at screen offset (13, 24). The game does not scale the
  frame: it is shown at the size it is stored.
- **Sectors are counted as 2048-byte user data.** `_B000.STR` is 18,096
  such sectors; dividing its size by the raw 2352-byte CD sector instead
  gives 15,757, about 15% low.
- **Sectors per frame varies per file**, so frame count is
  `sectors / StSECTOR_SIZE` per stream and cannot be assumed from a global
  frame rate. `_B000.STR` = 3,016 frames. The overlay keeps one number of
  its own, 10 sectors per frame at `0x80197724`, which it multiplies by the
  first frame to find where to start reading (`movie_stream_play`
  (`0x8019638C`)); `movie_play` always passes a first frame of 0, so the
  number never matters.
- A `.STR` also holds **interleaved XA audio sectors with no STR header**;
  a reader tells them from the video sectors by their structure.

For comparison, the bonus disc's single `_B000.STR` (The Making of Lunar)
is 320 x 240 / 10 sectors per frame / 529 MB; Lunar 2's scene streams,
measured the same way, are 320 x 224.

## Decoding

**The MDEC bitstream decodes to pixels.** The codec is VLC run/level pairs,
MPEG-1 intra dequantization, 8x8 IDCT, 4:2:0 chroma, column-major
macroblocks and YCbCr to RGB; format details, the two traps (byte order
and macroblock order) and the verification are in
**[formats/mdec.md](formats/mdec.md)**. Measured with station-tools'
decoder: **960 frames from all eight disc dumps of four programs decode
with zero invariant violations**, block counts exact. SSSC's `_B000.STR`
renders the Working Designs title card.

**In the game the work is split between Sony's code and the hardware.**
`MENU.BIN` links Sony's libpress, release 4.1 (code/subsystems/overlay.md;
the `MDEC_rest`, `MDEC_in_sync`, `MDEC_out_sync` debug strings are its).
`DecDCTvlc` (`0x80197350`), which Sony wrote in assembly, turns a frame's
bitstream into the MDEC's run-level codes on the CPU; `DecDCTin`
(`0x80196D40`) hands those codes to the console's MDEC *hardware*, and
`DecDCTout` (`0x80196DBC`) reads the pixels back a slice at a time. The
dequantization, IDCT and colour conversion are the hardware's, which is why
station-tools' decoder is a transcription of documented hardware behaviour
rather than a port of a game function.

**The audio.** A `.STR` interleaves CD-XA ADPCM sectors — mono 37800 Hz
4-bit on every Lunar stream, measured. The game does not decode them
itself: `movie_read_start` (`0x80196B0C`) reads the stream with
`CdRead2(0x1C0)` (streaming, double speed, real-time ADPCM), and
`movie_stream_play` (`0x8019638C`) turns the CD audio mix on
(`SpuSetCommonCDMix(1)`) at volume 0x7000 (`SpuSetCommonCDVolume`), so the
drive plays the stream's own XA sectors while the player reads the video.
To decode the audio outside the console, **an extracted `.STR` does not
contain usable audio**: Form 2 sectors carry 2324 bytes and an ISO
extractor keeps 2048, so every audio sector of an extracted file is
truncated and its subheader is gone. Audio comes from the disc image, at
raw 2352-byte sectors — details and the measured verification in
**[formats/xa-adpcm.md](formats/xa-adpcm.md)**.

## Which movie is which — `MoviePlay`

Every movie plays through **`MoviePlay(n)` (`0x800232D4`)** (overlay.c),
the "play event n" routine. It has three callers: the story script's
command `ScriptCommandMoviePlay` (`0x8005E2B4`), the opening
(`StartMoviePlay` (`0x800435FC`), n = 0) and the theater and bromides
(`MenuUseMoviePlay` (`0x800778C8`)). No battle code calls it.

1. `func_800230D4()` (`MovieScreenClose` (`0x8005901C`), `MusicStop`
   (`0x800557AC`)), then story bit `n + 0x154` is set (`StoryBitSet`
   (`0x800576EC`)) — so a scene seen in the story is unlocked in the
   theater;
2. `n` in `0x37..0x43` → a **bromide**, `PictureShow(n − 0x37)`
   (`0x80024A7C`), then step 4 (bromides: below);
3. otherwise `overlay_disc_wait(n)` (`0x80023184`) checks that the right
   disc is in: `overlay_movie_search` (`0x8002311C`) sprintfs `"_B%03d.STR"`
   and looks the name up; when it is missing, resource 0x257 is loaded and
   the disc-swap screen asks for disc 1 (`n < 0x24`) or disc 2 (from 0x24),
   and after each change the new disc's `LUNADATA` and movie are looked up
   and the first 0x800 bytes of its `MENU.BIN` are read to check their
   build stamp. Then `overlay_load` (`0x80023240`) loads `MENU.BIN` at
   0x80196000, the switch on `n` (its jump table at `0x80011760`) starts a
   DualShock rumble script for ten movies (n = 4, 17, 21, 24, 29, 36, 37,
   38, 41, 50; below), and **`movie_play(n)` (`0x80196B54`)** (in
   `MENU.BIN`) plays `"_B%03d.STR" % n` to 5 frames before the frame in its
   u16 table at `0x8019769C` (`MENU.BIN` offset 0x169C; zero = no movie; an
   n of 100 or more would play `_C%03d.STR` of n − 100 to half that frame
   — no `_C` file is on either disc). In story mode
   (`g_MovieFromMenu == 0`; `MenuUseMoviePlay` sets it to 1 around the
   call), after `n == 0x23` it asks for disc 2 (`overlay_disc_wait(0x24)`).
   Then `FaceSlotReset()` (`0x8001B178`) forgets the six portrait-cache
   slot ids and `FaceListLoad()` (`0x8007601C`) loads the party's portrait
   blocks again (`g_FaceList[0..5]`, then `[6]`) — the cache starts at
   0x801A9B00, just past `MENU.BIN`'s 0x12940-byte image;
4. in every case `func_800230FC()` (which calls the empty `func_800590B8`)
   and the vibration state is dropped: `g_PadActuatorList`,
   `g_PadActuatorRestFlags`, `g_PadActuatorListWait` and the actuator bytes
   `g_PadActuatorLevels` / `g_PadActuator1Level` = 0.

The function itself calls no RNG and waits no frame; both happen in its
callees.

### The movie rumble scripts

The switch of step 3 starts one of ten scripts from the pointer table
`g_MovieActuatorLists` with the usual trio (`g_PadActuatorList` = script,
`g_PadActuatorRestFlags` = 0x3F, `g_PadActuatorListWait` = 0; battle.md
§"The vibration scripts"). Movie 0x25 takes the last slot:

| n | movie | slot | script | bytes | clock waits: count, frame range | movie's end frame | random motion |
|---|---|---|---|---|---|---|---|
| 4 | `_B004` | 0 | `D_800AE420` | 62 | 8, 175 .. 363 | 930 | **yes** (one op 0x49) |
| 0x11 | `_B017` | 1 | `D_800AE460` | 41 | 5, 414 .. 928 | 984 | no |
| 0x15 | `_B021` | 2 | `D_800AE48C` | 223 | 31, 568 .. 2153 | 2621 | **yes** (two) |
| 0x18 | `_B024` | 3 | `D_800AE56C` | 39 | 2, 384 .. 470 | 480 | no |
| 0x1D | `_B029` | 4 | `D_800AE594` | 15 | 2, 228 .. 244 | 251 | no |
| 0x24 | `_B036` | 5 | `D_800AE5A4` | 187 | 20, 193 .. 1888 | 1895 | **yes** (two) |
| 0x25 | `_B037` | 9 | `D_800AE774` | 251 | 34, 109 .. 3161 | 3368 | no |
| 0x26 | `_B038` | 6 | `D_800AE660` | 98 | 11, 2604 .. 6460 | 6473 | **yes** (two) |
| 0x29 | `_B041` | 7 | `D_800AE6C4` | 49 | 5, 571 .. 1200 | 1212 | **yes** (one) |
| 0x32 | `_B050` | 8 | `D_800AE6F8` | 124 | 18, 108 .. 3926 | 4865 | **yes** (one) |

Entries 1 to 25 of `g_ScriptActuatorLists` (`0x800AE870`) are the other
scripts: battle code starts some of them by address (battle.md §"The
vibration scripts"), and the event script's `ScriptCommandActuatorListStart`
(`0x8005D294`) can start any of them. Two things set these ten apart from
those 25:

- **All ten use op 0xA0, none of the 25 does.** In `pad_update`
  (`0x8001F508`) (pad_update.c) op 0xA0 stores `(p[1] << 8) + p[2]` — a
  big-endian u16 — in `g_PadActuatorListWaitFrame` and the script then holds
  until `g_MovieFrameNumber >= g_PadActuatorListWaitFrame`.
  `g_MovieFrameNumber` (`0x801A893C`) is the last word of `MENU.BIN`'s image
  (file offset 0x1293C of 0x12940), and every script's operands stay below
  its own movie's end frame (table above): the clock is the movie's frame
  number, kept by the overlay, and each script is a rumble track cut to its
  movie.
- **Six of the ten put motor 1 on the random motion** (op 0x49, motion
  0x48), the one op that calls `rng_draw` (`0x8001C770`) — the battle RNG
  (frame-rng.md, draw site 2). **Those draws are made**: the overlay's
  movie loop steps the driver once per movie frame (next section).

### The movie loop steps the vibration driver

The player is `movie.c` in `decomp/SLUS_006.28-MENU/`. Its C's comments
liken its routines to Sony's SDK movie samples (`anim`, `strInit`,
`strNext`, `strNextVlc`, `strSync`, `strKickCD`, `strCallback`); no sample
was compared, and the routines are named by reading (docs/naming.md).
`movie_play(n)` (`0x80196B54`) → `movie_file_play(name, 0, end − 5)`
(`0x80196304`) (file lookup, display off, the VRAM strip at x = 320 saved,
play, restored) → `movie_stream_play` (`0x8019638C`) (start and end frame
into `D_801A88D0` / `D_801A88D4`, the clock `g_MovieFrameNumber` = 0, CD
audio volume 0x7000, the start sector into the CdlFILE `D_801A88B4`) → the
play loop `movie_frames_play(CdlFILE *)` (`0x80196414`). The loop sets up
the decode environment `D_801A88E4` (`DECENV` in movie.c, 0x30 bytes, one
image buffer: vlcbuf[2] +0, vlcid +8, imgbuf +0xC, rect[2] +0x10, rectid
+0x20, slice +0x24, isdone +0x2C; `movie_decode_init` (`0x801966F4`)),
starts the stream with `movie_slice_load` (`0x801967DC`) as the MDEC slice
callback (`movie_stream_start` (`0x80196768`): `DecDCTReset`,
`DecDCToutCallback`, `StSetRing`, `StSetStream`, then CdlSeekL and
`CdRead2` in `movie_read_start` (`0x80196B0C`)) and VLC-decodes the first
frame (`movie_frame_decode` (`0x801968D0`)). One pass:

1. `DecDCTin` (`0x80196D40`, mode 3, 24-bit output) of the frame decoded
   the pass before, and `DecDCTout` (`0x80196DBC`) of its first slice
   (0x18 VRAM words wide, 16 pixels at 24 bits, and the frame's height:
   one column of macroblocks); from there the callback `movie_slice_load`
   (`0x801967DC`) loads each slice with `LoadImage`, moves the slice right
   by its width and starts the next, and after the last sets `isdone` and
   flips `rectid`;
2. `movie_frame_decode` (`0x801968D0`) fetches the next frame — it polls
   `movie_frame_next` (`0x80196954`) up to 0x800000 times, and that
   routine polls `StGetNext` up to 0x800000 times, stores the STR header's
   frame number in `g_MovieFrameNumber`, sets the ended flag
   `D_801A8914 = 1` when that number is at or past `D_801A88D4`
   (unsigned), and sizes the frame rects from the header — then
   VLC-decodes it into the other buffer (`DecDCTvlc`) and frees the ring
   slot (`StFreeRing`);
3. `movie_decode_wait` (`0x80196A4C`) waits for `isdone` (every 0x800000
   polls it prints "time out in decoding !" and makes the callback's switch
   itself), `VSync(0)`, and the buffer not being decoded into is shown:
   `SetDefDispEnv` / `SetDefDrawEnv` on `rect[rectid ? 0 : 1]`, a 24-bit
   display (`isrgb24`, `disp.w * 2 / 3`) at screen (`D_80197720`,
   `D_80197721`, signed bytes, 13 and 24 in the image) 0xE0 × 0xC0,
   `PutDispEnv` / `PutDrawEnv`, `SetDispMask(1)`;
4. ended flag set → the loop is left, **before** the driver call;
5. **`pad_frame_step()` (`0x8001EE98`)**, the one-line wrapper round
   `pad_update` (`0x8001F508`) — its only call in the overlay, which never
   calls the field / battle frame flip `frame_end` (`0x8001F30C`);
6. the pad word the driver has just read can end the movie early, and
   either way sets the skipped flag `D_801A88D8 = 1`: bit 0x800 of
   `g_PadButtons` while the halfword `g_MovieSkipAllowed` is non-zero, or
   the low halfword of `g_PadButtons` equal to 0x4F. `StartMoviePlay` sets
   `g_MovieSkipAllowed` to 0x800, `MenuUseMoviePick` and `MenuUsePictureRun`
   to 0x8D0, and `ScriptCommandMoviePlay` to 0, so the opening and the
   theater's movies can be left on bit 0x800 and the story's cannot.

On the way out: CD audio volume 0, CdlMute, CdlSetmode 0x80 (double speed),
`DecDCToutCallback(0)`, the CD data and ready callbacks cleared, CdlPause,
CdlDemute, and the clock set to 0xFFFF. `movie_file_play` (`0x80196304`)
returns the skipped flag, and `MoviePlay` (`0x800232D4`) zeroes the script
state right after (step 4 above).

So the driver runs once per movie frame shown, and op 0xA0's clock is the
STR frame number of the frame fetched in the same pass. One frame is
fetched before the loop, so driver call k (0-based) sees clock `k + 2`.
The loop is left in the pass that fetches frame E = the table's end frame
− 5 (`movie_play` passes `end − 5`), so a movie whose table end frame is N
gets `N − 7` driver calls. Every draw falls well before frame E; only
`_B036`'s script is cut short — it would end at call 1888, and calls
0..1887 are all it gets. Replaying the driver's C over each script with
that clock gives the draws a movie makes when it is watched to its end
(each of the six files carries frames 1..N with no gap, N = the table's
end frame):

| n | movie (title string 0x501 + n) | driver calls | RNG draws | at movie frames | script over at call |
|---|---|---|---|---|---|
| 4 | "Quark Awakens" | 923 | **11** | 220..240 | 367 |
| 0x15 | "Ghaleon vs Quark" | 2614 | **179** | 1117..1718 | 2154 |
| 0x24 | "The Grindery Unleashed" | 1888 | **346** | 86..1703 | 1888 |
| 0x26 | "The Dark Goddess" | 6466 | **294** | 3240..5643 | 6460 |
| 0x29 | "A Dragonmaster Reborn" | 1205 | **436** | 588..1024 | 1200 |
| 0x32 | "The Green Earth" | 4858 | **77** | 5..310 | 3929 |

The other four scripts (movies 0x11, 0x18, 0x1D, 0x25) draw nothing. What
changes a count: leaving the movie early (step 6) cuts it off at that
frame; a frame the CD ring drops moves the clock without a driver call;
and every viewing draws again, the theater's included. The vibration
setting does not matter (frame-rng.md: the draws precede the off check).
The counts are a replay of the driver's C, not a trace of the game.

### The theater

The theater is the item "Rememberizer" (item 0xB5, `+0x07` = 7 →
`MenuUseMovieRun` (`0x8007757C`)): its list (`MenuUseMovieListSet`
(`0x800776CC`)) holds entries `i` in 1..0x37 whose story bit `i + 0x153` is
set, captioned with string `0x500 + i` (`MenuUseMovieListDisp`
(`0x80077838`)); picking one runs `MenuUseMoviePick` (`0x800775D4`) →
`MenuUseMoviePlay(i − 1)` (`0x800778C8`) → `MoviePlay(i − 1)`
(`0x800232D4`). So **entry i = movie `_B{i−1:03d}.STR` = title string
`0x500 + i`**: `_B000` "LUNAR Opening", `_B001` "Alex at the Monument",
`_B004` "Quark Awakens", … `_B035` "To the Dark Frontier" (disc 1), `_B036`
"The Grindery Unleashed" … `_B051` "Ending", `_B052`-`_B054` "Althena's
Spring 2-4" (disc 2). The entries whose title is the blank glyph (`_B032`,
`_B039`, `_B042`, `_B045`-`_B048`) have table end 0 and are the files
absent from both discs. Each non-zero end frame equals the file's last
`StFRAME_NO`, except `_B007`'s: 658 in the table, 653 in the file. The
player stops at the table value minus 5 (`movie_play` (`0x80196B54`)), so
`_B007` stops on its own last frame and every other movie 5 frames before
its own. `MenuUseMoviePlay`'s C never names its argument, but nothing
writes `$a0` before its call of `MoviePlay`, which receives the number.

### Bromides

**Bromides** are event numbers `0x37 + k`, k = 0..12, reached by using a
bromide item (+0x07 low nibble 4 → `MenuUsePictureRun` (`0x8007690C`),
which passes `(+0x07 >> 4) + 0x37`): `PictureShow(k)` (`0x80024A7C`)
(picture.c) loads **LUNADATA resource 0x24A + k** (586..598) — two kept
command-2 pictures, 8bpp TIMs at VRAM (320,0) and (320,240), CLUT at
(0,509) — shows them panned for 0x2D0 frames and plays voice
`g_PictureVoices[k]` (`PictureXaPlay`). Titles are strings `0x538 + k`
("Bromide of Luna 1", "Bromide of Mia 1" …), in the same order as k.

## The XA files outside the movies

`_AUDIO.XA` ships on both discs and disc 2 adds `_SECOND.XA`: the same
CD-XA sectors, played without a movie. `system_init` (`0x8001EEB8`) hands
`"_AUDIO.XA"` to `xa_file_set` (`0x8002347C`) at start-up, and the voices
play channels of it from there (`VoiceStart`, through `xa_voice_get` and
`xa_play_start` (`0x800235C0`)); a bromide's voice is one of them. After
the game's end `GameRun` (`0x800559F4`) sets `"_SECOND.XA"` instead and
plays its nine channels in turn, each for the count of vertical blanks
in `g_GameSecondXaChannelFrames`.
