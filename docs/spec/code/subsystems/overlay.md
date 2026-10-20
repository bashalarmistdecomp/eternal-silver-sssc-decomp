# The MENU.BIN overlay

This page covers `MENU.BIN`, the movie player, as an image: what is in it,
how the main program loads and checks it, its function and data map, and
the code in it that is assembly. The overlay's tree is
`decomp/SLUS_006.28-MENU/`; its [README](../../../../decomp/SLUS_006.28-MENU/README.md)
says how the tree is built and where its symbols come from, and what the
player does with a movie is on [../../video.md](../../video.md).

## What it is

- **76,096 bytes (`0x12940`)**, loaded whole to `0x80196000` before every
  movie. Only 5,508 bytes are code (`0x8019610C`–`0x80197690`); the rest
  is data (§"Function map"). The copies on the two discs are byte-identical,
  and the tree's build gives the disc's file whole
  ([../../code-decompilation.md](../../code-decompilation.md)).
- **36 functions** in three parts: the game's player `movie.c`, 17
  functions; Sony's libpress object, 17; Sony's vlc object, 2. 33 are C.
  The two of the vlc object were written in assembly
  (`config/asm_only.txt`: `addi`, `add`, `sub`, `mfc0`, `mtc0`, which the C
  compiler never emits), so 34 can be C; the one more is the player's
  slice callback `movie_slice_load` (`0x801967DC`) (§"What is assembly").
- **Compiled `-G0`**: no instruction names `$gp`, and the main program's
  globals it reads are reached `%hi`/`%lo`. The compilers of each part are
  on [psyq/codegen.md](psyq/codegen.md); libpress is built with the
  library recipe `cc272_lib`.
- **The memory it uses beside its image** (`movie_decode_init`
  (`0x801966F4`), `D_80197730`): the two run-length buffers at
  `0x80156000` and `0x8016B000`, the CD ring at `0x80180000` and the
  slice's image buffer at `0x80190000`. `0x80156000` is the main
  program's `g_ResourceReadBuffer`, and the overlay's own address is its
  `g_LoadBuffer` (`main_buffer_init` (`0x80013E04`)); the portrait cache
  just past the image, at `0x801A9B00`, is loaded again after every movie
  (video.md §"Which movie is which").

## The build stamp

The overlay's first bytes are the text `$at 99-04-19 21:32:42(dosuser)$`,
the same text as the main program's `g_BuildStamp` (`0x80011000`): the
two were built on 19 April 1999 at 21:32:42, the overlay with an older
libpress (§"The libpress object"). The stamp names no one but the
account `dosuser`.

`movie_stamp_compare` (`0x8019610C`) returns `strcmp(s, the overlay's
stamp)`. No jump reaches it and no data word holds its address; the main
program checks the stamp itself.

## Loading

- **On a disc change**, `overlay_disc_wait` (`0x80023184`) reads the first
  0x800 bytes of the new disc's `MENU.BIN` to `0x80196000` and asks for
  the disc again when `overlay_stamp_check` fails, so a disc from another
  build is refused like a wrong disc.
- **Before a movie**, `overlay_load` (`0x80023240`) reads the file whole
  (`file_load("MENU.BIN", 0x80196000, 0, -1)`), and `overlay_stamp_check`
  (`0x800232B0`) compares the two stamps:
  `build_stamp_compare_copy(0x80196000)` (`0x80013DE4`) is `strcmp` of its
  argument with `g_BuildStamp`. On a mismatch it calls
  `assert_fail("OVERLAY ERROR", "OVERLAY.C", 0xB2)` (the strings at
  `0x80011740` and `0x80011750`); `assert_fail` (`0x8001B050`) is empty in
  this build, so the overlay is used anyway. Then `FlushCache` inside
  `EnterCriticalSection` / `ExitCriticalSection`.
- **The main program calls the overlay at one address**, `movie_play(n)`
  (`0x80196B54`), from `MoviePlay` (`0x800232D4`) (overlay.c), the routine
  every movie goes through (video.md §"Which movie is which").
- **The overlay calls the main program back**: 28 of its routines, under
  the main program's own names (docs/naming.md §"Which release of Sony's
  libraries the programs link"); their rows in the overlay's register say
  `boot-exe`, and `config/label_fixups.txt` gives the link their addresses.
- **Sony's stream library is the main program's.** `StSetRing`,
  `StSetStream`, `StGetNext`, `StFreeRing`, `StCdInterrupt` and `CdRead2`
  are linked into the main program with its PSY-Q 4.5 libcd, and nothing
  in the main program's code calls them; only the overlay does.

## Function map

The function starts were read from the disc file at `0x80196000`, every
internal `jal` target and the first word after each `jr $ra`: 38 starts.
Two of them are branch targets after a `jr` and are not functions: the
tail at `0x80197340` that `DecDCTvlcSize`'s `blez` at `0x8019732C` reaches,
and the state-save exit at `0x8019765C` that `DecDCTvlc`'s `bgez` at
`0x80197518` reaches.

| range | segment | contents |
|---|---|---|
| `0x80196000`–`0x8019610C` | data `head_rodata` | the build stamp and the format strings (`MOV.STR pos (%x:%x:%x)`, `time out in decoding !`, `_B%03d.STR`, libpress's `MDEC_*` messages) |
| `0x8019610C`–`0x80196BDC` | `movie` | the game's movie player, 17 functions (below) |
| `0x80196BDC`–`0x8019731C` | `libpress` | Sony libpress's `libpress` object, 17 functions (below) |
| `0x8019731C`–`0x80197690` | `vlc` | Sony libpress's `vlc` object, assembly in the original: the size word `D_8019731C` that `DecDCTvlcSize` reads and writes, `DecDCTvlcSize` (`0x80197320`), `DecDCTvlc` (`0x80197350`) (its decoder state in `D_80197884`) |
| `0x8019769C` | data | the u16 end frame of each movie n (video.md §"Which movie is which") |
| `0x80197720`–`0x80197734` | data | the screen offset `D_80197720` / `D_80197721` (signed bytes, 13 and 24), `D_80197724` (10: the sectors per frame the start frame is multiplied by), the frame's width and height `D_80197728` / `D_8019772C` as last read from a sector header, and `D_80197730`, the ring's address |
| `0x80197734`–`0x8019783C` | data | libpress's two MDEC command packets: `0x40000001` and the two 64-byte quantizer tables, `0x60000000` and the 64 IDCT coefficients ([../../formats/mdec.md](../../formats/mdec.md) §"Coding details") |
| `0x80197844`–`0x80197880` | data | libpress's hardware register pointers (DMA0/DMA1 MADR, BCR, CHCR; MDEC0/MDEC1; DPCR) |
| `0x801980B4`–`0x801A80B4` | data | the 64 KB table `DecDCTvlc` indexes |
| `0x801A88B4`–`0x801A8940` | data | the player's variables: the stream's file record `D_801A88B4`, the start position `D_801A88CC`, the start and end frame `D_801A88D0` / `D_801A88D4`, the skipped flag `D_801A88D8`, the decode state `D_801A88E4` (0x30 bytes), the ended flag `D_801A8914`, the movie's file record `D_801A8924`, the frame clock `g_MovieFrameNumber` (`0x801A893C`) |

### The movie player (`movie.c`)

The routines are named by reading (docs/naming.md); where the C's comment
likens one to a routine of Sony's SDK movie samples, the table says so.
No sample was compared. The player has one image buffer. A frame's walk
through it is in video.md §"The movie loop steps the vibration driver".

| address | name | the C likens it to | what it does |
|---|---|---|---|
| `0x8019610C` | `movie_stamp_compare` | — | `strcmp(s, the overlay's stamp)`; nothing calls it |
| `0x80196130` | `movie_file_search` | — | 1 when the movie file is not in the game's table of the disc's files (the main program's `cd_file_search` fills `D_801A8924`) |
| `0x80196158` | `movie_vram_clear` | — | `ClearImage` of a rectangle to black, `DrawSync(0)` |
| `0x8019619C` | `movie_vram_block_move` | — | `MoveImage` of a 16 × 16 block of video memory |
| `0x801961D8` | `movie_vram_save` | — | before a movie: moves the column of video memory at x 0x140 (the frames, 0x150 wide, cover it) to the rows at y 0xD0 and 0x1B0, and clears both frame areas |
| `0x80196278` | `movie_vram_restore` | — | after it: moves the column back and clears |
| `0x80196304` | `movie_file_play` | — | finds and plays one movie; returns −1 (no file) or the skipped flag |
| `0x8019638C` | `movie_stream_play` | — | the start position (the file's sector + start frame × `D_80197724`), the CD audio mix on at volume 0x7000, then the play loop |
| `0x80196414` | `movie_frames_play` | `anim` | the play loop |
| `0x801966F4` | `movie_decode_init` | — | the run-length buffers `0x80156000` / `0x8016B000`, the image buffer `0x80190000`, the frame rectangles 0x150 × 0xC0, the slice 0x18 × 0xE0 |
| `0x80196768` | `movie_stream_start` | `strInit` | `DecDCTReset`, `DecDCToutCallback`, `StSetRing` (0x20 sectors), `StSetStream`, then `movie_read_start` |
| `0x801967DC` | `movie_slice_load` | `strCallback` | the MDEC slice callback (assembly, §"What is assembly") |
| `0x801968D0` | `movie_frame_decode` | `strNextVlc` | the next frame from the ring, `DecDCTvlc`, `StFreeRing` |
| `0x80196954` | `movie_frame_next` | `strNext` | `StGetNext`; the frame clock, the ended flag, the frame's size |
| `0x80196A4C` | `movie_decode_wait` | `strSync` | waits for the frame's last slice; the decoding timeout |
| `0x80196B0C` | `movie_read_start` | `strKickCD` | CdlSeekL, then `CdRead2` |
| `0x80196B54` | `movie_play` | — | **the entry**: `_B%03d.STR` to the table's end frame − 5 (n of 100 or more: `_C%03d.STR` of n − 100 to half of it; no such file is on either disc) |

### The libpress object

Both libpress members are **release 4.1's**, older than the 4.5 the main
program links. They fit release 4.1 of the PSY-Q signature set and no
other: all 1,567 known bytes of `LIBPRESS.OBJ` equal the overlay's at
`0x80196BDC`, and all 872 of `VLC.OBJ` at `0x8019731C`. No 4.1 library file
was compared, so this rests on the signature set (docs/naming.md §"Which
release of Sony's libraries the programs link"). All 17 functions of the
libpress object are C, built with `cc272_lib`.

The ten exported functions, named by those bytes: `DecDCTReset`
(`0x80196BDC`), `DecDCTGetEnv` (`0x80196C10`), `DecDCTPutEnv`
(`0x80196C9C`), `DecDCTBufSize` (`0x80196D34`), `DecDCTin` (`0x80196D40`),
`DecDCTout` (`0x80196DBC`), `DecDCTinSync` (`0x80196DDC`), `DecDCToutSync`
(`0x80196E18`), `DecDCTinCallback` (`0x80196E54`), `DecDCToutCallback`
(`0x80196E78`). Its seven static helpers: the three that print their own
names, `MDEC_rest` (`0x80196E9C`), `MDEC_in_sync` (`0x801970A8`) and
`MDEC_out_sync` (`0x8019713C`), and four unnamed ones — `func_80196F8C`
sends a buffer to the MDEC's input, `func_8019701C` takes its output,
`func_801971D0` reads its status word, and `func_801971E8` prints the
timeout report. `DecDCTReset` calls the main program's `ResetCallback`
(`0x80094BD8`), and the two callback setters its `DMACallback`
(`0x80094C38`), both of libetc 4.5.

## What is assembly

- **`DecDCTvlcSize` (`0x80197320`) and `DecDCTvlc` (`0x80197350`)**, Sony's
  assembly: they stay assembly.
- **`movie_slice_load` (`0x801967DC`)**, the one function of the game's
  player left as assembly. What it does is read from its bytes
  (video.md §"The movie loop steps the vibration driver"); what holds it
  back is addressing. The original forms `&dec.slice` first and reaches
  `imgbuf`, `rectid`, `isdone` and the slice's fields as offsets from it,
  deriving `&dec` only for the `rect[]` index. GCC 2.8.1 makes that code
  only when the members' addresses are constant expressions, spelled
  `*(&dec.m)`; a plain `dec.m` puts the struct's base in a register first.
  The Japanese build's `MENU.BIN`, built by another compiler, has the same
  addressing. The `*(&dec.m)` spelling gives the bytes but is a forcing
  device ([psyq/codegen.md](psyq/codegen.md) §"Forcing devices: what they
  stood for"), so the tree keeps the assembly.
