# Lunar SSSC (PS1) — the disc's files and LUNADATA.FIL

This page maps the first disc: its files, the executable's layout, and
LUNADATA.FIL resource by resource, with the function that reads each
resource. The EXE finds every resource through one table,
`g_ResourceSectorPairs` (`0x800AE904`), so the map below follows that
table rather than a scan of the file.

## The disc's files

```
Lunar_SSS_Disc1.bin / .cue
├── Track 1: MODE2/2352 data (553MB)
│   ├── SLUS_006.28          350KB   the executable
│   ├── SYSTEM.CNF              68B   boot config ("cdrom:\SLUS")
│   ├── MENU.BIN              76KB   the movie-player overlay
│   ├── LUNADATA.FIL        36.9MB   the game's data, read by resource number
│   ├── LUNAR_DA.PCM        22.8MB   not examined
│   ├── BLANK_DA.PCM         6.7MB   not examined
│   ├── _AUDIO.XA           38.2MB   the voices (CD-XA)
│   └── _B000–_B035.STR    ~400MB   35 movies (STR/MDEC; no _B032)
├── Track 2: CD Audio (25MB)  — Red Book music
└── Track 3: CD Audio (8MB)   — Red Book music
```

* **MENU.BIN** is raw MIPS code and data with no EXE header.
  `overlay_load` (`0x80023240`) reads it to `0x80196000`, and
  `overlay_stamp_check` (`0x800232B0`) compares its first bytes with the
  executable's own build stamp `g_BuildStamp` (`0x80011000`), the text
  `$at 99-04-19 21:32:42(dosuser)$` (Sony's `strcmp`, through
  `build_stamp_compare` (`0x80013D08`)). Its C is under `decomp/SLUS_006.28-MENU/`.
* **_AUDIO.XA** is the file `xa_file_set` (`0x8002347C`) looks up at
  start-up (`g_SystemAudioXaFileName`); the voices (`VoiceSet`
  (`0x80055470`), `xa_voice_get` (`0x800238BC`)) count from its first
  sector. The sound
  effects are not in it: they are the SPU bank in LUNADATA resource `0xE8`.
* **LUNAR_DA.PCM** and **BLANK_DA.PCM**: no name in the C refers to
  either file; what they hold was not examined.
* **The movies**: [../video.md](../video.md).

## SLUS_006.28 — the executable

```
SLUS_006.28 (350,208 bytes)
├── PS-X EXE header (2048 bytes)
│   Entry: 0x801001EC, Load: 0x80100000, Stack: 0x801FFFF0 (the loader ignores it)
├── The loader at 0x80100000 (940 bytes)
│   start-up and the unpacker
└── The packed image (file offset 0x1000, 0x54463 bytes)
    Decompresses to 690,176 bytes at 0x80010800
    ├── Inner PS-X EXE header at 0x80010800
    ├── Game code and data from 0x80011000
    ├── g_ResourceSectorPairs at 0x800AE904 (601 pairs, below)
    ├── g_CardSavePlaceNames at 0x800A8618 (84 addresses of short place names)
    └── g_FontStringWords at 0x800B386C (512 words of packed strings — text.md)
```

The bootstrap is [../code/subsystems/bootstrap.md](../code/subsystems/bootstrap.md).

## LUNADATA.FIL (36,958,208 bytes = 18,046 sectors)

### Its own table of contents

```
Offset 0x000: Archive header (32 bytes)
  Entries: 46, Total sectors: 599, File size: 36,958,208
Offset 0x020: 46 file entries × 32 bytes each
  Format: [filename:20B] [sector:4B] [size:4B] [checksum:4B]
```

The entries name SYSTEM.DAT, STFIELD.DAT, OPEN1.BIN and 43 files
MAP_xxx.BIN. The game does not read this table: `resource_load`
(`0x80020230`) reads a resource by number, from the sector pair the EXE
holds for it.

### The resource table

`g_ResourceSectorPairs` holds a pair of 16-bit words for each resource:
its first sector in LUNADATA.FIL and its length in sectors.
`resource_load(n, destination)` reads the pair and calls
`file_load("LUNADATA.FIL", destination, sector << 11, length << 11)`;
`resource_seek` (`0x80020290`) sends the drive to the first sector and
reads nothing. The table runs 0x964 bytes to the next symbol,
`g_ScriptResourcePairs` (`0x800AF268`): 601 pairs, resources 0 to `0x258`.
Nothing tests a number against that size.

Pair 0 is (0, 599), the span of the archive's own table of contents.
From resource 1 on the pairs are contiguous: each starts where the one
before ends, from sector 16 to sector 18045, and pair `0x258` is
(18046, 0). So every sector after the header belongs to exactly one
resource.

### The resources, by number

| resources | sectors | what | read by |
|---|---|---|---|
| 0 | 0–598 | the span of the table of contents | — |
| 1 | 16–48 | SYSTEM.DAT: the string table, the glyph atlas, three UI palettes (below) | `main` → `resource_load_run(1)` (`0x800202CC`) |
| 2 | 49–53 | STFIELD.DAT: the common sprite sheet — palette row 507, three sprite data blocks, a 4-bit page at (960, 0) | `sprite_reset` (`0x80018DBC`) |
| 3 | 54–122 | OPEN1.BIN: the title screen's command list | `StartResourceLoad` (`0x8004409C`) |
| 4–100 | 123–3667 | the 97 maps, one command list each | `g_MapResourcePairs` (`0x800AF364`), `MapResourceFind` (`0x80020CD0`) |
| 101–162 | 3668–4245 | the 62 event scripts | `g_ScriptResourcePairs`, `ScriptResourceLoad` (`0x80020D20`) |
| `0xA3`–`0xBA` | 4246–4340 | the portrait files 1 to 24 (FACE blocks, five faces each) | `FaceDataLoad` (`0x8001B1C0`) |
| `0xBB`–`0xE4` | 4341–4564 | the 42 scene portrait blocks (FACE blocks, ten faces each) | `FaceSlot6Load` (`0x8001B50C`) |
| `0xE5`, `0xE6` | 4565, 4566 | the field sprite palettes of rows 504 and 505 | `sprite_init` (`0x80018BFC`) |
| `0xE7` | 4567 | the party's starting records | `ChrRecordsReset` (`0x80063B30`) |
| `0xE8` | 4568–4590 | the sound-effect bank | `sound_common_load` (`0x80023E00`) |
| `0xE9`–`0x166` | 4591–12669 | the music banks (`0xE9` + n, `sound_music_load` (`0x80023E9C`)) and the area banks (`0x126`–`0x165`, `g_SoundAreaBankPairs`, `sound_area_load` (`0x80023F78`)), and the battle's bank `0x166` (`sound_btl_seek` (`0x80024044`)) | `sound_bank_load` (`0x80024138`) |
| `0x167`–`0x196` | 12670–14485 | the 48 battle graphics sets | `BtlDataLoad` (`0x80018694`), `g_BtlDataResources` |
| `0x197` | 14486–14549 | the battle's party bundle | `BtlPartyDataLoad` (`0x80018A78`), `BtlPartyLoad` |
| `0x198`–`0x1D7` | 14550–15418 | the spell and item effects | `BtlEffectLoad` (`0x800897DC`), `g_BtlEffectResourceNumbers` |
| `0x1D8`–`0x1E7` | 15419–15491 | the effects' background layers | `tile_resource_load` (`0x8001DAE4`), `g_TileResourceRows` |
| `0x1E8` | 15492–15570 | the party's field sprite sheets | `MapPartySpriteDataLoad` (`0x80063D48`) |
| `0x1E9`–`0x249` | 15571–17559 | the 97 map object sets ([lunadata-sprites.md](lunadata-sprites.md)) | `MapObjectsDataLoad` (`0x80065418`) |
| `0x24A`–`0x256` | 17560–17968 | the 13 bromides, two TIMs each | `PictureShow` (`0x80024A7C`) |
| `0x257` | 17969–18045 | the disc-swap screen | `resource_load_run(0x257)` in map_frame.c and overlay.c |

Most resources are command lists that `resource_command_run`
(`0x800202FC`) walks: a command word (type in the low byte, a sub-number in
bits 8 to 15), a byte count, the payload; a command word of 0 ends the list
([../graphics.md](../graphics.md) §"A map is a command list"). The scripts,
the portrait blocks, the sprite palettes, the party records, resource 2 and
the graphics bundles of the battle and the field are not: their readers
take them as they lie.

The sound banks are command lists of types 13 (a VAB header,
`sound_vab_head_open`), 14 (a VAB body), 15 (a sequence) and 17 (a table of
sound ids). Scanned by byte pattern, their command-13 headers show up as
127 blocks whose first word is `0x0000000D` and whose payload begins with
the VAB magic `"pBAV"`: one for each of the 127 banks `0xE8` to `0x166`.

### SYSTEM.DAT — resource 1

Loaded at boot: `main` runs `resource_load_run(1)` once, after
`sprite_reset(0)`. Its commands, in order:

```
type 1      the string table and UI data, inflated by resource_command01_set
            (0x80020480) into g_ResourceCommand01Buffer (87,856 bytes):
              +0x0000-0x5999  u16 offset table + 1,216 strings — every name
                              and system message (text.md §"The string table")
              +0x599C         the palette animations' colour frames
                              (g_PaletteAnimationCols, set by GameStart)
              +0x899C         the icon table and its LZSS streams (IconInit)
type 2      the glyph atlas TIM (16,416 bytes): 256 × 128 4-bit, VRAM (960, 384)
type 12 ×3  palettes for rows 502 (icons), 503 (portraits), 507 (text)
```

Details: [../assets/battle-ui.md](../assets/battle-ui.md) §"Resource 1 (SYSTEM.DAT)".

### STFIELD.DAT — resource 2

Its TOC name says nothing of what it holds; `sprite_reset` reads it as an
8-word header of offsets (in 4-byte units): a 256-colour palette for row
507, three packed sprite data blocks (`g_SpriteData0` to `g_SpriteData2`)
and a packed 4-bit page sent to (960, y). Details:
[../assets/battle-ui.md](../assets/battle-ui.md) §"Resource 2 (the common FX sheet)".

### OPEN1.BIN — resource 3

The title screen, loaded by `StartResourceLoad`: a command list that
begins with a type-3 command (`word0 = 3`), 15 packed streams in all:

```
Streams 0–2:   65,536 bytes each — 256 × 256 8-bit texture pages
Stream 3:      1,024 bytes — a palette
Streams 4–5:   15 bytes each
Stream 6:      4,096 bytes
Streams 7–11:  72,224 bytes each
Streams 12–14: 66,080 bytes each
```

72,224 and 66,080 bytes are exactly the sizes of 8-bit TIMs with a
256-colour CLUT, 320 × 224 and 256 × 256 (32 bytes of headers and 512 of
CLUT besides the pixels). The logo is a TIM at VRAM (384, 0)
([../assets/title-logo.md](../assets/title-logo.md)); the title's draw is
in [../code/subsystems/menu-ui.md](../code/subsystems/menu-ui.md)
§"The title screen's data and draw".

### The maps — resources 4 to 100

97 maps, each a command list of the same shape: texture pages, palettes,
the tile layout, the attribute plane, the second tile layer
([../graphics.md](../graphics.md)). The TOC names 43 of them
(MAP_xxx.BIN); the others have no TOC entry and are found only through
`g_MapResourcePairs`. None of them is a battle background: a battle's arena
is drawn from its graphics set's pages
([../code/subsystems/battle.md](../code/subsystems/battle.md) §"The arena").

### The portraits — resources `0xA3` to `0xE4`

66 FACE blocks: 24 portrait files of five 64 × 64 8-bit faces (faces 1 to
120) and 42 scene blocks of up to ten copies of those faces. Their palette
is row 503 (resource 1). Layout and numbering:
[../assets/sprites.md](../assets/sprites.md) §"Portraits by face number".

### The bromides — resources `0x24A` to `0x257`

`PictureShow(k)` reads resource `0x24A + k` for k = 0 to 12 (`MoviePlay`
(`0x800232D4`) passes play-event numbers `0x37` to `0x43` less `0x37`) and
loads its two TIMs; resource `0x257` is the disc-swap screen. Image sizes
seen in them: 320 × 256, 320 × 240 and 320 × 224 at 8 bits.
[../sprites.md](../sprites.md) §"Pictures: the TIMs".
