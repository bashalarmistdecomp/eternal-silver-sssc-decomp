# The main program's code

The pages under `spec/code/` describe the main executable, `SLUS_006.28`,
as its C reads: the game's subsystems, the engine's memory map, and the
parts of Sony's PSY-Q libraries it links. The movie-player overlay,
`MENU.BIN`, has one page here ([subsystems/overlay.md](subsystems/overlay.md));
the bonus disc's program has none yet.
How the executable is packed on the disc and unpacked into memory is in
[../code-decompilation.md](../code-decompilation.md).

## The image

| | |
|---|---|
| File | `SLUS_006.28`, the image the disc's executable unpacks |
| Architecture | MIPS R3000, little-endian |
| Load address | `0x80011000` |
| Size | 688,128 bytes |
| Entry point | `0x80013C4C`, `__SN_ENTRY_POINT` (`2mbyte.c`) |
| `$gp` | `0x800B8540` |

Its C is in `decomp/SLUS_006.28/src/`: the game's own code in files cut
where the subsystem changes, and Sony's libraries one file per member. Its names, with what each
rests on, are in `decomp/SLUS_006.28/config/symbol_addrs.txt`.
[../../../README.md](../../../README.md) gives the counts of functions by
the kind of name they have, and [../../naming.md](../../naming.md) how the
names were given. [../../../BUILD.md](../../../BUILD.md) says how the
build reproduces the image.

## Pages

| Page | What it covers |
|---|---|
| [engine-map.md](engine-map.md) | The game engine's memory map |
| [hand-assembly.md](hand-assembly.md) | Which functions were assembly in the original, and the evidence |
| [libsnd-map.md](libsnd-map.md) | The libsnd and libspu functions the music path reaches |

## Subsystems (`subsystems/`)

| Page | What it covers |
|---|---|
| [bootstrap.md](subsystems/bootstrap.md) | Startup, the unpacker, initialisation |
| [battle.md](subsystems/battle.md) | The battle system |
| [boss-ai.md](subsystems/boss-ai.md) | The per-boss routines |
| [battle-presentations-2.md](subsystems/battle-presentations-2.md) | Item, weapon and boss presentations |
| [spell-rng.md](subsystems/spell-rng.md), [frame-rng.md](subsystems/frame-rng.md) | Where the battle draws random numbers |
| [field-frame.md](subsystems/field-frame.md) | The field's frame: the walk, collision, the camera, the map's tile animation, fades |
| [map-engine.md](subsystems/map-engine.md) | The map |
| [map-enemies.md](subsystems/map-enemies.md) | Enemies on the map: roam, sight, chase, the touch, after the fight |
| [npc-scripting.md](subsystems/npc-scripting.md) | NPC logic and the event script interpreter |
| [menu-ui.md](subsystems/menu-ui.md) | Menus and UI |
| [rendering.md](subsystems/rendering.md) | The tile engine, GPU primitives, VRAM |
| [audio.md](subsystems/audio.md) | Sound and music playback |
| [io-hardware.md](subsystems/io-hardware.md) | DMA, GPU registers, controller input |
| [bios-calls.md](subsystems/bios-calls.md) | The BIOS calls |
| [overlay.md](subsystems/overlay.md) | `MENU.BIN`, the movie-player overlay |

## Sony's libraries (`subsystems/psyq-libs.md`, `subsystems/psyq/`)

| Page | What it covers |
|---|---|
| [psyq-libs.md](subsystems/psyq-libs.md) | The libraries the program links, which release, and the subjects no library's own page holds |
| [psyq/index.md](subsystems/psyq/index.md) | Every library function by address: its name, member, file and recipe |
| [psyq/codegen.md](subsystems/psyq/codegen.md) | How the library objects were compiled, and which shapes of C decide their bytes |
| [psyq/libgpu-libgs.md](subsystems/psyq/libgpu-libgs.md) | libgpu and libgs, with libapi's heap and libc2's strings |
| [psyq/libcd.md](subsystems/psyq/libcd.md) | libcd, with the libgs, libgte, libc2 and libetc members among it |
| [psyq/libetc-libsnd.md](subsystems/psyq/libetc-libsnd.md) | libetc's vertical blank and interrupts, and libsnd |
| [psyq/libspu.md](subsystems/psyq/libspu.md) | libspu |
| [psyq/libpad.md](subsystems/psyq/libpad.md) | libpad |
| [psyq/libapi-libcard-libmcrd.md](subsystems/psyq/libapi-libcard-libmcrd.md) | The memory card: libapi's pad interrupt entry, libcard, libmcrd |
