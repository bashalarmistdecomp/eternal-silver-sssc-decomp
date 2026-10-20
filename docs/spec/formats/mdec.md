# PS1 MDEC video — the codec inside an STR frame

This page covers the coding of one movie frame: the frame header, the
bitstream, the block and macroblock coding, the colour conversion, the
two traps a reader meets, and how a decode is checked against the shipped
streams. It holds no game knowledge beyond which streams were measured.
The files and the player are on [../video.md](../video.md); the audio
sectors of the same streams are on [xa-adpcm.md](xa-adpcm.md). The
decoder this page was checked with is station-tools', a custom decomp
toolkit that will be released later.

The MDEC is PlayStation *hardware*. In this game the movie overlay's
copy of Sony's libpress does the CPU's part: `DecDCTvlc` (`0x80197350`),
which Sony wrote in assembly, turns a frame's bitstream into the MDEC's
run-level codes, and `DecDCTin` / `DecDCTout` hand them to the hardware
and take the pixels back. The dequantization, IDCT and colour conversion
are the hardware's, so there is no game function to decompile for them.
What follows is the hardware's documented behaviour, and every claim below
was checked against the shipped streams by decoding them — see
§Verification.

## Pipeline

```
sectors -> one frame's bitstream
  frame header             8 bytes: code bound, 0x3800, q_scale, version
  bitstream                16-bit LITTLE-ENDIAN words, bits read MSB-first
  per 8x8 block            DC (10-bit, v2), then AC run/level VLC pairs, EOB
  dequantize               MPEG-1 default intra matrix, AC scaled by q_scale
  IDCT                     8x8, separable
  macroblock               Cr, Cb, Y1..Y4 — 4:2:0, 16x16 luma per chroma pair
  frame                    macroblocks down COLUMNS, then rightwards
  colour                   YCbCr -> RGB, luma offset +128
```

## Frame header (8 bytes)

| offset | type | meaning |
|---|---|---|
| 0 | u16 | MDEC code count **in pairs** — multiply by 2 |
| 2 | u16 | magic, always `0x3800` |
| 4 | s16 | `q_scale`, the AC quantizer scale |
| 6 | u16 | version — **2** in every stream measured here |

The game's C does not read the header: `DecDCTvlc` does, and it is Sony's
assembly. What the C does read is the run-level buffer `DecDCTvlc` writes:
`DecDCTin` (`0x80196D40`) takes that buffer's first word as the MDEC's
command, sends `*buf & 0xFFFF` 32-bit words of it (two 16-bit codes each),
and sets the command's output depth from its mode (the player passes 3,
24-bit output).

**The code count is a bound, not an exact count.** Measured over 324
frames spanning SSSC, The Making of Lunar, Lunar 2 and the Lunar 2 demo: the
doubled value is always a multiple of 32, is never below the number of
codes the bitstream actually produces, and is never more than 63 above it.
Rounding the produced count up to a multiple of 32 does **not** reproduce it
(148 of 324), so the slack is the encoder's own buffering and the honest
invariant is the bound. A check that asserts equality would be a plausible
rule that quietly fails on half the frames.

## The two traps

**Byte order.** The bitstream is a sequence of little-endian 16-bit words
and the bits within each word run MSB-first. Reading the bytes in file order
decodes correctly for a while and then desyncs, which is the worst failure
mode available: it looks like a codec bug rather than a byte-order bug.
Swap each 16-bit word once per frame before reading bits.

**Macroblock order is column-major** — down a column of 16x16 macroblocks,
then one column to the right. Row-major produces a picture whose content is
recognisable but scrambled into vertical strips. The game's player reads
the MDEC's output the same way: its slice is 0x18 VRAM words wide (16
pixels at 24 bits) and as high as the frame, and its callback
`movie_slice_load` (`0x801967DC`) loads each slice and moves the next one
right by its width (video.md §"The movie loop steps the vibration
driver").

## Coding details

- **DC (v2)** is a straight 10-bit two's-complement value, no prediction and
  no VLC. It is dequantized by `QUANT_TABLE[0]` (= 2) **only** — `q_scale`
  does not apply to DC.
- **AC** uses MPEG-1's Table B-14 run/level codes, each followed by one sign
  bit, with `10` for end-of-block and `000001` escaping to a literal 6-bit
  run plus a **10-bit** signed level (MPEG-1's escape level is narrower).
  Dequantization is `(level * QUANT_TABLE[i] * q_scale + 4) >> 3` at zigzag
  position `i`.
- **Quantizer matrix** is MPEG-1's default *intra* matrix, in zigzag order.
  It is the table the game itself loads into the MDEC: libpress's
  `MDEC_rest(0)` (`0x80196E9C`) sends the packet at `0x80197734` (command
  0x40000001, then the luma and the chroma table, which are the same), and
  read from the image, each is that matrix in zigzag order with entry 0
  replaced by 2, the DC's scale above.
- **Colour**: `R = Y + 1.402*Cr`, `G = Y - 0.3437*Cb - 0.7143*Cr`,
  `B = Y + 1.772*Cb`, with the luma IDCT output offset by +128 and each
  channel clamped to 0..255.

## Versions

Only **v2** occurs, in every stream on every disc measured. A v3 stream
codes DC differentially with its own VLC tables; decoding it as v2 would
produce a plausible-looking wrong picture, so a v3 frame is to be refused,
not decoded as v2.

## Verification

Correctness here is falsifiable **against the files themselves**. No
reference decoder was consulted and no emulator capture was involved —
which matters, because an output that resembles a capture of video memory
proves little: the resemblance is read backwards from the answer.

A decode is held to three bounds that all come out of the stream:

1. the frame geometry fixes the block count exactly
   (`ceil(w/16) * ceil(h/16) * 6`);
2. the header's code count bounds the codes produced (0..63 of slack);
3. the bitstream must be consumed to within its trailing padding.

The third is the sharp one. A single wrong VLC entry, a swapped bit order or
a wrong block order desyncs the stream, and a desynced stream does not land
within one word of the end after a thousand blocks — it runs off the end or
leaves hundreds of bits unread.

**Measured: 960 frames across eight disc dumps and four distinct programs
decode with zero violations**, block counts exact in every one, and the
unread tail always between 10 and 41 bits. The rendered frames are the
expected images (SSSC's Working Designs card, Lunar 2's Blue Star opening).

Synthetic tests cannot catch a VLC table that is self-consistently wrong —
it would still round-trip its own encoder. The bitstream-tail bound against
real streams is what covers that.

## Playback rate

Frame rate is not stored in the stream — it falls out of `StSECTOR_SIZE`
(sectors per frame) against the disc's transfer rate, so it is an
**estimate on a documented assumption**: 2x CD, 150 sectors/s (the game's
player reads at double speed, `CdRead2(0x1C0)`). The measured spread is
real and is why one hardcoded rate is wrong:

| sectors/frame | implied fps | seen on |
|---|---|---|
| 6 | 25 | SSSC, Lunar 2 discs 1-3, the Lunar 2 demo |
| 7 | 21 | some Lunar 2 scenes |
| 10 | 15 | The Making of Lunar, the Lunar 2 making-of, one SSSC stream |
| 12 | 12.5 | some SSSC streams |
