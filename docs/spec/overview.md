# Lunar: Silver Star Story Complete (PS1): overview

The game's discs and their files, the data archive `LUNADATA.FIL` and the
resources the program reads from it, and the related program on the bonus
disc. Where each file and resource lies, with the function that reads each
resource, is in [file-sector-map/overview.md](file-sector-map/overview.md);
how the executable is packed is in
[code-decompilation.md](code-decompilation.md).

## The game

- **Title**: Lunar: Silver Star Story Complete
- **Platform**: PlayStation
- **Developer**: Game Arts / Kadokawa / ESP
- **Publisher**: Working Designs (USA). The rights lines the title screen
  prints are in [NOTICE.md](../../NOTICE.md).
- **Disc labels**: `LUNAR_THE_SILVER_STAR_STORY_1` and `_2`
- **Executable**: `SLUS_006.28` (disc 2 carries the same file as
  `SLUS_008.99`)

## The discs

Each game disc is a multi-track BIN/CUE. Sizes of files and tracks below
are in MB of 1,048,576 bytes. Disc 1:

| Track | Type | Size | Contents |
|---|---|---|---|
| 1 | MODE2/2352 | 553 MB | Data (the file system) |
| 2 | AUDIO | 25 MB | Red Book audio: the one CD track the game plays, for music number 0x3C |
| 3 | AUDIO | 7.7 MB | Red Book audio that no code of the executable plays |

What the program does with the two audio tracks is in [sound.md](sound.md)
§"CD audio". Disc 2's audio tracks, and the bonus disc's, are the same
sizes.

### Disc 1's files (42)

| File | Size | What it is |
|---|---|---|
| `SLUS_006.28` | 350,208 bytes | The boot file: a loader and the packed executable ([code-decompilation.md](code-decompilation.md)) |
| `SYSTEM.CNF` | 68 bytes | The boot configuration (`BOOT = cdrom:\SLUS_006.28;1`) |
| `MENU.BIN` | 76,096 bytes | The movie-player overlay, code and data with no header ([code/subsystems/overlay.md](code/subsystems/overlay.md)) |
| `LUNADATA.FIL` | 36,958,208 bytes | The game's data, read by resource number (below) |
| `LUNAR_DA.PCM` | 21.8 MB | No name in the C refers to it; read as a file, its bytes are all zero ([sound.md](sound.md) §"The `.PCM` files are empty") |
| `BLANK_DA.PCM` | 6.4 MB | The same |
| `_AUDIO.XA` | 36.4 MB | CD-XA audio: the voices ([sound.md](sound.md) §"The XA voice clips"). The sound effects are not in it: they are the sound bank of resource 232 |
| `_B000.STR` – `_B035.STR` | 1.9–45.2 MB each | The movies, STR/MDEC: 35 files, for there is no `_B032.STR` ([video.md](video.md)) |

### Disc 2 (`SLUS_008.99`)

The disc's serial is the one its own `SYSTEM.CNF` boots
(`BOOT = cdrom:\SLUS_008.99;1`). It has 22 files, and the ones it shares
with disc 1 are the same bytes (sha1):

- **The executable.** `SLUS_008.99` is `SLUS_006.28` byte for byte; only
  the file's name and the `SYSTEM.CNF` line differ. So the matching build
  proves both discs' code.
- `LUNADATA.FIL`, `MENU.BIN`, `_AUDIO.XA`, `LUNAR_DA.PCM`, `BLANK_DA.PCM`
  and `_B000.STR`. Every resource of the archive is on both discs.

The files only disc 2 has:

| File | Size | What it is |
|---|---|---|
| `_SECOND.XA` | 53.6 MB | A second CD-XA file. The one reader in the C is `GameRun` (`0x800559F4`): after the game's end it plays the file's nine channels, 21.8 minutes in all ([code/subsystems/bootstrap.md](code/subsystems/bootstrap.md) §"The run loop and the end of the game"). What they hold is not established |
| `_B036.STR` – `_B054.STR` | 13 files, up to 79.0 MB | Disc 2's movies: `_B036`–`_B038`, `_B040`, `_B041`, `_B043`, `_B044` and `_B049`–`_B054`. With `_B000`, the movie each disc boots into, disc 2 has 14 ([video.md](video.md)) |

## The bonus disc

| Disc | What it is |
|---|---|
| `SLUS_009.21`, The Making of Lunar | 5 files: its own executable of 1,204,224 bytes, a plain one (a 2,048-byte header and a 1,202,176-byte image; nothing is packed, unlike the game's), the making-of video (`_B000.STR`, 529.2 MB), two `.PCM` files of the same names and sizes as the game's, and Red Book tracks 2 and 3. Its executable also holds the hidden minigame, Lords of Lunar. It is the third program of this repository, `decomp/SLUS_009.21/` |

## LUNADATA.FIL

The archive holds all of the game's data but the movies and the audio of
the XA files and the CD tracks. The program does not read the archive's
own table of contents: it reads a resource by number, from a table of
sector pairs in the executable.

### Its table of contents

- **Offset 0x00**: the header entry, 32 bytes
  - Bytes 0–3: the number of entries (uint32 LE): **46**
  - Bytes 4–19: zeros
  - Bytes 20–23: 599 (uint32 LE). What it counts is not established; the
    executable's table gives resource 0 the same length in sectors, 599,
    from sector 0
  - Bytes 24–27: the file's size (uint32 LE): 36,958,208
  - Bytes 28–31: zeros
- **Offset 0x20**: the file entries, 32 bytes each
  - Bytes 0–19: the file name (ASCII, zero-padded to 20 bytes)
  - Bytes 20–23: the first sector (uint32 LE; times 2048 for the offset)
  - Bytes 24–27: the size in bytes (uint32 LE)
  - Bytes 28–31: the packed file's modification time (uint32 LE, seconds
    since 1970). The newest entry, `OPEN1.BIN`, reads 1999-04-16 03:06 as
    UTC, three days before the `99-04-19` of the program's build stamp; the
    43 `MAP_*.BIN` entries all read 1998-01-17

| # | File name | Offset | Size | What it is |
|---|---|---|---|---|
| 0 | `SYSTEM.DAT` | `0x8000` | 64 KB | Resource 1: the string table, the glyph atlas, three UI palettes |
| 1 | `STFIELD.DAT` | `0x18800` | 9 KB | Resource 2: the common sprite sheet |
| 2 | `OPEN1.BIN` | `0x1B000` | 137 KB | Resource 3: the title screen |
| 3–45 | `MAP_001.BIN` – `MAP_056.BIN` | varies | 19–135 KB | Resources 4 to 46: 43 of the 97 maps. The names' numbers skip; the other 54 maps have no entry |

### The resource sector table

The table is `g_ResourceSectorPairs` (`0x800AE904`), pairs of 16-bit
words: for resource n, word 2n is its first sector in `LUNADATA.FIL` and
word 2n + 1 its length in sectors. `resource_load(n, destination)` (`0x80020230`, `resource_load.c`)
calls `file_load("LUNADATA.FIL", destination, first << 11, sectors << 11)`
and returns `sectors << 11`; nothing tests n against the table's size.
`resource_load_run(n)` (`0x800202CC`) reads resource n to `0x80156000` and
runs its list of commands (§"The command list format"). The table runs 0x964
bytes to the next symbol: 601 pairs.

By region, with each region's size in the archive (its sectors times
2048, in millions of bytes):

| Resources | Sectors | Size | What | Read by |
|---|---|---|---|---|
| 0 | 0–598 | | The pair (0, 599): the table of contents and, past it, the first resources again | — |
| 1–3 | 16–122 | 0.22 MB | SYSTEM.DAT, STFIELD.DAT and OPEN1.BIN | `main` (resource 1), `sprite_reset` (`0x80018DBC`, 2), `StartResourceLoad` (`0x8004409C`, 3) |
| 4–100 | 123–3667 | 7.26 MB | The 97 maps, a command list each | `MapResourceLoad` (`0x80020D70`), by `g_MapResourcePairs` |
| 101–162 | 3668–4245 | 1.18 MB | The 62 event scripts, each beginning with its table of 16-bit label offsets | `ScriptResourceLoad` (`0x80020D20`), by `g_ScriptResourcePairs` |
| 163–228 | 4246–4564 | 0.65 MB | The portraits, FACE blocks: the portrait files 1 to 24 (163–186) and the 42 scene portrait blocks (187–228) | `FaceDataLoad` (`0x8001B1C0`); `FaceSlot6Load` (`0x8001B50C`) |
| 229–231 | 4565–4567 | | The field sprite palettes of rows 504 and 505 (229, 230); the party's starting records (231) | `sprite_init` (`0x80018BFC`); `ChrRecordsReset` (`0x80063B30`) |
| 232–358 | 4568–12669 | 16.59 MB | The sound: 127 banks of VAB samples and SEQ sequences. 232 is the common bank, 233–293 the music's, 294–357 the areas', 358 the battle's ([sound.md](sound.md) §"How the banks are distributed") | `sound_bank_load` (`0x80024138`) |
| 359–406 | 12670–14485 | 3.72 MB | The 48 battle graphics sets | `BtlDataLoad` (`0x80018694`), by `g_BtlDataResources` |
| 407 | 14486–14549 | 0.13 MB | The battle's party bundle | `BtlPartyDataLoad` (`0x80018A78`) |
| 408–471 | 14550–15418 | 1.78 MB | The spell and item effects | `BtlEffectLoad` (`0x800897DC`), by `g_BtlEffectResourceNumbers` |
| 472–487 | 15419–15491 | 0.15 MB | The effects' background layers: command lists, whose command 3 sends a texture page and command 18 keeps a pattern's address | `tile_resource_load` (`0x8001DAE4`), by `g_TileResourceRows` |
| 488 | 15492–15570 | 0.16 MB | The party's field sprite sheets | `MapPartySpriteDataLoad` (`0x80063D48`) |
| 489–585 | 15571–17559 | 4.07 MB | The 97 map object sets | `MapObjectsDataLoad` (`0x80065418`), by `g_MapObjectsDataResourceNumbers` |
| 586–598 | 17560–17968 | 0.84 MB | The 13 bromides, two TIMs each | `PictureShow(k)` (`0x80024A7C`) reads 0x24A + k |
| 599 | 17969–18045 | 0.16 MB | Pictures 0 to 3 are the disc-change screens; `GameRun` shows pictures 4 and 5 after the game's end | `overlay_disc_wait` (`0x80023184`), `GameRun` (`0x800559F4`) |
| 600 | | | The pair (18046, 0) | — |

From resource 1 on, each pair starts where the one before ends, from
sector 16 to sector 18045, the file's last: every sector from 16 on belongs
to exactly one of resources 1 to 599, and the file holds nothing else.

Measured over the archive's contents: 14.91 MB of the sound is VAB bodies,
that is SPU-ADPCM samples (1,029 of them, and 412 sequences); 5.17 MB of
the maps is texture pages
(command 3), 388 pages of which 282 differ, which cut into 8 × 8 tiles hold
142,147 different tiles that are not blank, counting a tile and its flips
as one (145,152 without the flips). Command-list chunks that repeat another
exactly are rare: 13% of the maps' chunk bytes, 2% of the sound's. The
archive is not padded with copies for each map.

### The command list format

Most resources are lists of commands that `resource_command_run`
(`0x800202FC`, `resource_load.c`) walks:

```
word 0   the command word: the low byte is the command's number, the second
         byte a sub-number the handlers read; a command word of 0 ends the list
word 1   the payload's length in bytes
then     the payload; the next command starts (length >> 2) words after it
```

Before the first command it empties the list of kept pictures
(`resource_tim_reset`). Each handler is called with the command word, the
length and the payload's address. The switch's jump table is the 18 words
at `0x80011558`.

| Command | Handler | What it does with the payload |
|---|---|---|
| 1 | `resource_command01_set` (`0x80020480`) | Unpacks it (`inflate_buffer`) into `g_ResourceCommand01Buffer`, a block of 0x15828 bytes: the string table, the palette animations' colours, the icons |
| 2 | `resource_tim_add` (`0x800208A0`) | A packed TIM picture. Sub-number 0: unpacked and sent to video memory at once; otherwise its address is kept for `resource_tim_unpack` and `resource_tim_load` |
| 3 | `resource_tpage_load` (`0x8002097C`) | A texture page, unpacked and sent: 256 × 256 at 8 bits for sub-numbers 0 to 2 and 0x80 to 0x82, 128 × 128 for 3 to 0x7F |
| 4 | `resource_palette_load` (`0x80020A08`) | Palettes, unpacked: two rows of 256 colours for sub-number 0, one row for 1 to 0x80 |
| 5 | `resource_bg_set` (`0x80020B34`) | Copied to `g_BgDataBuffer` (0x8000 bytes): the map's tile layout, which `bg_copy` and `bg_block_load` read |
| 6 | `resource_bg_attribute_set` (`0x80020B60`) | Copied to `g_BgAttributeDataBuffer` (0xA000 bytes): the attribute map |
| 7 | `resource_bg_second_set` (`0x80020BD8`) | Unpacked to `g_BgSecondBuffer` (0x4000 bytes): the second tile layer, which `bg_second_draw` draws |
| 9 | `resource_map_enter_table_set` (`0x80020B8C`) | Copied to the map's enter table at `0x801E6B00` (`MapEnterTableAddress`) when `g_MapResourceSw` is not 0 |
| 12 | `resource_palette_row_load` (`0x80020ACC`) | One palette row, the sub-number's, unpacked and sent |
| 13 | `sound_vab_head_open` (`0x80024314`) | A VAB header: copied to the sound buffer and opened with `SsVabOpenHead` |
| 14 | `sound_vab_body_transfer` (`0x800243D0`) | A VAB body, sent to SPU RAM with `SsVabTransBody` from where it was read |
| 15 | `sound_seq_add` (`0x80024464`) | One SEQ sequence, copied to the sound buffer |
| 16 | `resource_tile_animation_set` (`0x80020BFC`) | Unpacked to `0x801C1B00`: the tile images of the map's tile animation. 12 bytes is a typical payload |
| 17 | `sound_id_table_set` (`0x800244E4`) | Sixteen 16-bit numbers copied to `g_SoundIds`: the sound numbers the area bank's sequences answer to |
| 18 | `resource_tile_pattern_set` (`0x80020C24`) | Kept where it lies: its address goes to `g_TilePatterns[sub-number]` |
| 8, 10, 11 and any other | none | Stepped over |

### Who loads what

```
Resources with a list of commands (the maps, resource 1, the bromides, ...):
  resource_load_run(n)              0x800202CC
    resource_load(n, 0x80156000)    0x80020230: the pair's sectors of LUNADATA.FIL
    resource_command_run(...)       0x800202FC: the handlers above

A sound bank (resources 232 to 358):
  sound_common_load()               0x80023E00: resource 232, bank 0, once in main's pass
  sound_music_load(n)               0x80023E9C: the bank of music number n, bank 2
  sound_area_load()                 0x80023F78: the area's bank (g_SoundAreaBankPairs), bank 1
    sound_bank_load(resource)       0x80024138
      resource_load_run(resource)
        command 13 sound_vab_head_open, 14 sound_vab_body_transfer,
        15 sound_seq_add, 17 sound_id_table_set

A bromide (resources 586 to 598):
  PictureShow(k)                    0x80024A7C
    resource_load_run(k + 0x24A)    0x24A = 586
```

Commands 13 and 14 check Sony's answer (`SsVabOpenHead : failed !!!`,
`SsVabTransBody : failed !!!`), and the copy to the sound buffer checks for
an overrun; the checks pass the developers' file name `SOUND.C` to
`assert_fail` (`0x8001B050`), which is empty in this build, so a failed
check reports nothing and the load goes on. A bank's commands, its three
slots and which bank a music number or an area selects are in
[sound.md](sound.md) §"Container: the sound-bank command list", §"The bank
table and its three slots" and §"BGM numbers", and in
[code/subsystems/audio.md](code/subsystems/audio.md).

## Formats

| What | Format | Described in |
|---|---|---|
| The archive | `LUNADATA.FIL`: its own table of contents of 32-byte entries, and the executable's table of sector pairs | above |
| Map texture pages | 8-bit indexed, 256 × 256, packed (command 3) | [graphics.md](graphics.md) |
| Map palettes | 16-bit colours (PS1 1555), 512 + 256 per map (command 4) | [formats/palettes.md](formats/palettes.md) |
| Map tile layout | 16-bit tile words in blocks of 8 × 8 cells, each block packed for `lz_unpack`, in a map of 128 × 128 cells (command 5) | [graphics.md](graphics.md), [code/subsystems/rendering.md](code/subsystems/rendering.md) |
| The attribute map | 256 × 256 attributes, one for 8 × 8 pixels, in blocks of 16 × 16 packed for `lz_unpack` (command 6); the walk tests it, and bits 0x3800 and 0x4000 stop a box | [code/subsystems/field-frame.md](code/subsystems/field-frame.md) |
| The second tile layer | 0x80 rows of 64 16-bit cells, packed (command 7) | [code/subsystems/rendering.md](code/subsystems/rendering.md) |
| Event scripts | resources 101–162 | [code/subsystems/npc-scripting.md](code/subsystems/npc-scripting.md) |
| Map objects | resources 489–585 | [file-sector-map/lunadata-sprites.md](file-sector-map/lunadata-sprites.md) |
| UI graphics | SYSTEM.DAT: the glyph atlas and palettes; OPEN1.BIN: the title screen | [assets/battle-ui.md](assets/battle-ui.md), [assets/title-logo.md](assets/title-logo.md) |
| Movies | PS1 STR/MDEC (`.STR` files) | [video.md](video.md), [formats/mdec.md](formats/mdec.md) |
| Music and sound effects | VAB banks and SEQ sequences, resources 232–358 | [sound.md](sound.md) |
| Voices | CD-XA ADPCM (`_AUDIO.XA`) | [sound.md](sound.md), [formats/xa-adpcm.md](formats/xa-adpcm.md) |
| CD audio | Red Book, tracks 2 and 3 | [sound.md](sound.md) |
| The packing | gzip 1.2.4's inflate reading 16-bit words (`inflate_buffer`, `0x80021D6C`) for most payloads; `lz_unpack`'s LZ for the executable, the tile layout's and the attribute map's blocks, and the icons | [formats/compression.md](formats/compression.md) |

## References

- **[studio-lucia/lunardata](https://github.com/studio-lucia/lunardata)**:
  community notes on Lunar's file formats, mostly of the Saturn version.
- How this repository reads the discs: `recipe/extract.py` and
  [../../BUILD.md](../../BUILD.md), step 2.
