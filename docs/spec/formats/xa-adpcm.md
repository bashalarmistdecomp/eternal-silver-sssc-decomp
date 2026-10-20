# CD-XA ADPCM — the audio in a PS1 movie

This page describes CD-XA ADPCM, the sound format of a PlayStation disc's
streams: the sector it rides in, the sound group and its header, the decode,
and the measurement that checks it against the disc's own clock. It is
PlayStation knowledge, not the game's, and it covers the audio sectors of the
game's movies (`.STR`) and of its `.XA` files alike. The movies themselves are
[video.md](../video.md), their picture side [mdec.md](mdec.md), and the
game's XA voice clips [sound.md](../sound.md) §"The XA voice clips". The
measurements below were made with station-tools, a custom decomp toolkit that
will be released later.

No code of the game decodes these sectors: the CD drive plays them itself.
The movie player in `MENU.BIN` starts its stream with `CdRead2(0x1C0)`
(`movie_read_start` (`0x80196B0C`), the overlay's movie.c: streaming, double
speed, real-time ADPCM), and the main executable plays its voice clips with
the drive's mode byte 0xC8 (`xa_mode_set` (`0x80023574`): double speed,
ADPCM playback, the file/channel filter on).

A movie on a PS1 disc is ONE sector stream carrying both tracks, interleaved
so a double-speed drive delivers them in real time. The video sectors are
MDEC frames; the rest is this.

## The trap: audio does not survive file extraction

A CD-XA sector is 2352 bytes and comes in two forms. **Form 1** carries 2048
bytes of user data with ECC — that is what a filesystem read returns.
**Form 2** carries **2324** bytes and no ECC, and that is where audio rides.

An ISO extractor reads 2048 bytes from every sector, so in an extracted
`.STR`:

- every audio sector is **truncated** by 276 bytes, and
- the **subheader is gone entirely** — which is where the sample rate,
  channel count and bit depth are declared.

So audio can only be decoded from the **disc image**, at raw 2352-byte
sectors, from the file's place (LBA) in the disc's directory.

## Layout

```
raw sector    2352 B: 12 sync + 4 header + 8 subheader + 2324 data + 4 EDC
subheader     file, channel, submode (bit 5 = Form 2, bit 2 = audio), coding
coding info   bits 0-1 mono/stereo · 2-3 37800/18900 Hz · 4-5 4/8-bit
user data     18 sound groups x 128 B  (2304 used, 20 spare)
sound group   16 B parameters + 112 B samples
block         28 samples; 8 blocks per group at 4-bit, 4 at 8-bit
```

A block's **sound parameter** byte is `filter << 4 | shift`, and decoding is

```
sample = (nibble << 12) >> shift                      # sign-extended
out    = clamp16(sample + (K0[f]*prev1 + K1[f]*prev2 + 32) >> 6)
```

with `K0 = (0, 60, 115, 98)` and `K1 = (0, 0, -52, -55)`, pre-scaled by 64.

**The filter is recursive and its history must persist across blocks, groups
AND sectors.** Resetting per sector clicks every 106 ms.

### The sound-group header — measured, not cited

Published descriptions of the 16-byte header disagree with each other, and
reading the wrong four bytes is a *silent* corruption. It was settled by
measurement over **4,018 non-silent sound groups** from SSSC's `_B000.STR`:

| comparison | agreement |
|---|---|
| bytes 0..3 == bytes 4..7 | **100 %** |
| bytes 0..3 == bytes 8..11 | 3.2 % |
| bytes 4..7 == bytes 12..15 | 3.2 % |

So 4..7 is the redundant copy of 0..3, and 8..11 (copied to 12..15) is the
second parameter set — the eight real parameters are bytes **4..11**.

**Block order and parameters.** The eight blocks of a group play in TIME
order 0..7, and:

```
block b  parameter  = header byte 4 + b
         samples    = nibble (low if b is even, high if odd) of data byte
                      16 + (b >> 1) + 4*i,  i = 0..27
```

so the data is interleaved in lanes of two blocks (lane0-low, lane0-high,
lane1-low, ...) and the filter history runs through the blocks in that
order. This is ffmpeg's `adpcm_xa`: decoded this way, the `_AUDIO.XA` voice
clip 0x2A is **bit-identical** to ffmpeg's decode (`-f psxstr` on the clip's
raw sectors: 120,960 samples, max difference 0). The copy measurement above
cannot tell one assignment of parameters to blocks from another; that
comparison is what settles it.

Two notes on getting this right. **Silence proves nothing**: in a silent
group all 16 bytes are identical, so a first pass over the file's lead-in
"confirms" every candidate layout equally — the measurement above excludes
uniform groups. And the decoded signal shows that each half of the group has
its own parameter set: layouts that give each half its own set are
**continuous across block boundaries** (mean |delta| ratio 1.038
edge-to-inner) while layouts reusing one set for all eight blocks are not
(1.071); ADPCM predicts from the previous two samples, so wrong parameters
jump every 28th sample. (That ratio cannot rank assignments — wrong ones also
give ~1.0.)

8-bit streams have four blocks and only the first parameter set. **None of
the discs here ship one**, so that half is unverified by measurement.
**Emphasis** (coding info bit 6) is not covered here.

## Verification

The decode is checked against the disc's own clock, not against a reference
decoder. Audio sectors appear **1 in 16** in these streams; one Form 2
sector yields `18 x 8 x 28 = 4032` mono samples, which at 37800 Hz is
0.10667 s — and 16 sectors at 150 sectors/s is **0.10667 s**. Measured on
SSSC's `_B000.STR`: 100 audio sectors out of 1600 raw, 10.667 s of audio
against 10.667 s of disc time. **Exact.**

That single agreement pins the sample rate, the samples-per-sector, the
interleave and the 2x-CD transfer rate all at once, and it lets a player use
the sector index as one clock for both tracks: sector N plays at N/150 s
whichever track it belongs to, so A/V sync needs no negotiation. Played that
way, the movies of every disc drift by under 0.03 s.
