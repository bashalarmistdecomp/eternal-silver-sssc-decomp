# The specification

What was learned about *Lunar: Silver Star Story Complete* while decompiling
it: how the program is organised, what each subsystem does, the file formats
it reads, and the parts of Sony's SDK libraries it links. The C under
`decomp/` says what the program does; these pages say what that means.

## Where these pages come from

They were written in the earlier, private repository this one was cut from,
beside tools, applications and extracted game data that stayed there.
Before they were brought out, each page under `spec/` was read again
against the C and the register: what a page says is what the C shows, and
no page points at a file of that repository. Where a fact was found with a
tool (a disassembly or cross-reference pass, an emulator trace, an asset
exporter), the page says it was found with station-tools, a custom decomp
toolkit that will be released later. [HISTORY.md](../HISTORY.md) is the
old repository's timeline, and [progress.md](progress.md), which `make
status` writes, says how much of each program is decompiled now.

The pages hold no game data: no images, no audio, no dumps. They quote a
few bytes or instructions where a format or a function is being explained.

## Names in these pages

- A function or global that has a name is cited by it, with its address
  beside it: `` `BtlRun` (`0x80027434`) ``. One that has no name is cited
  by its address (`func_8001C4E4`, `D_800B8EEC`). In the C, every named
  function keeps its address on the line above its definition, so either
  form finds it.
- Code is cited by today's files. Each file's header says which file of
  the first split its code came from, so a file named in an older text can
  still be found.
- `decomp/<serial>/config/symbol_addrs.txt` keeps every former name of a
  symbol in its row (`was:`), so a name an older text uses is found there.
  Four of Sony's names changed owner and cannot be looked up that way;
  [naming.md](naming.md) §"Where things stand" has them.

[naming.md](naming.md) has the rule for citing a function, the register's
format, and what each name rests on. [naming-log.md](naming-log.md) is the
record of how the names were given, unit by unit and wave by wave.

## The program

| Page | What it covers |
|---|---|
| [spec/overview.md](spec/overview.md) | The discs, the archive, the file inventory, the related programs |
| [spec/code-decompilation.md](spec/code-decompilation.md) | The executable: how it is packed, loaded and laid out |
| [spec/code/README.md](spec/code/README.md) | The index of the pages on the main program's code |
| [spec/code/engine-map.md](spec/code/engine-map.md) | The game engine's memory map |
| [spec/code/hand-assembly.md](spec/code/hand-assembly.md) | Which functions were assembly in the original, and the evidence |
| [spec/code/libsnd-map.md](spec/code/libsnd-map.md) | The libsnd and libspu functions the music path reaches |

## Subsystems (`spec/code/subsystems/`)

| Page | What it covers |
|---|---|
| [bootstrap.md](spec/code/subsystems/bootstrap.md) | Startup, the unpacker, initialisation |
| [battle.md](spec/code/subsystems/battle.md) | The battle system |
| [boss-ai.md](spec/code/subsystems/boss-ai.md) | The per-boss routines |
| [battle-presentations-2.md](spec/code/subsystems/battle-presentations-2.md), [spell-rng.md](spec/code/subsystems/spell-rng.md), [frame-rng.md](spec/code/subsystems/frame-rng.md) | Spell, item and boss presentations, and where they draw random numbers |
| [field-frame.md](spec/code/subsystems/field-frame.md), [map-engine.md](spec/code/subsystems/map-engine.md), [map-enemies.md](spec/code/subsystems/map-enemies.md) | The field: its frame, the walk, the map, enemies on the map |
| [npc-scripting.md](spec/code/subsystems/npc-scripting.md) | NPC logic and the event script interpreter |
| [menu-ui.md](spec/code/subsystems/menu-ui.md) | Menus and UI |
| [rendering.md](spec/code/subsystems/rendering.md) | The tile engine, GPU primitives, VRAM |
| [audio.md](spec/code/subsystems/audio.md) | Sound and music playback |
| [io-hardware.md](spec/code/subsystems/io-hardware.md), [bios-calls.md](spec/code/subsystems/bios-calls.md) | DMA, GPU registers, controller input, BIOS calls |
| [overlay.md](spec/code/subsystems/overlay.md) | `MENU.BIN`, the movie-player overlay |
| [psyq-libs.md](spec/code/subsystems/psyq-libs.md) and [psyq/](spec/code/subsystems/psyq/index.md) | The SDK library code, library by library, and how it was compiled |

## Formats and assets

| Page | What it covers |
|---|---|
| [spec/graphics.md](spec/graphics.md), [spec/formats/palettes.md](spec/formats/palettes.md) | Maps, tilesets, pixel formats, palettes |
| [spec/sprites.md](spec/sprites.md), [spec/assets/sprites.md](spec/assets/sprites.md) | Character and enemy sprites, portraits |
| [spec/assets/battle-effects.md](spec/assets/battle-effects.md), [spec/assets/battle-ui.md](spec/assets/battle-ui.md), [spec/assets/title-logo.md](spec/assets/title-logo.md) | Battle presentations, battle UI graphics, the title logo |
| [spec/battle-data.md](spec/battle-data.md) | Battle data tables |
| [spec/sound.md](spec/sound.md), [spec/formats/xa-adpcm.md](spec/formats/xa-adpcm.md) | CD audio, sound banks, XA-ADPCM |
| [spec/video.md](spec/video.md), [spec/formats/mdec.md](spec/formats/mdec.md) | Movies and the MDEC codec |
| [spec/text.md](spec/text.md) | Text and dialogue |
| [spec/formats/compression.md](spec/formats/compression.md) | The compression schemes |
| [spec/file-sector-map/](spec/file-sector-map/overview.md) | Where each file sits on the disc |

## The bonus disc

The program of "The Making of Lunar" and its hidden minigame has no page
here yet: its notes are still to be read again the way `spec/` has been, and
will be published then. `spec/` describes the main game.
