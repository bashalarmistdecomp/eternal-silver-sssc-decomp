# Building

```sh
recipe/toolchain.sh                           # 1. the toolchain, once
python3 recipe/extract.py <your disc images>  # 2. the program images, once
make programs                                 # 3. build all three and compare
```

The third step must print three lines:

```
OK: 0xa8000 bytes, sha1 900ce6a9dfa9563ef7e0a129602945d1303436d8     SLUS_006.28
OK: 0x12940 bytes, sha1 166b669ceacf42fa09dda45a8c5974e2a0f28fe9     SLUS_006.28-MENU
OK: 0x125800 bytes, sha1 58b2052cc554e050489ed7729b92c858443e9baf    SLUS_009.21
```

**The rule:** a change is right only if `make` still ends with OK. The built
program is byte-identical to the original or the change is wrong.

Everything runs from the repository root, on x86-64 Linux.

## 1. The toolchain

`recipe/toolchain.sh` puts everything in `toolchain/`, which is not in git.
Each piece comes from where its authors publish it, at a pinned version, and
is checked against a hash recorded in the script before it is used.

| Piece | What it is | From |
|---|---|---|
| `binutils/` | GNU binutils 2.42 for `mipsel-linux-gnu`: assembler, linker, objcopy | built from the GNU source tarball |
| `gcc-2.8.1-psx/` | The game's compiler, GCC 2.8.1 | [decompals/old-gcc](https://github.com/decompals/old-gcc) release 0.17 |
| `gcc-2.7.2-psx/` | GCC 2.7.2, for SDK library objects built with the older compiler | the same release |
| `gcc-970318-psx/` | The compiler of the SDK 4.5 and later libraries: GCC's development tree of 1997-03-18 | built in docker by `recipe/gcc-970318/build.sh` |
| `maspsx/` | Adapts the old compilers' output to GNU as, the way Sony's ASPSX assembled it | [mkst/maspsx](https://github.com/mkst/maspsx), one pinned commit |
| `psyq/4.4/` | Sony's PSY-Q SDK headers, release 4.7 (the directory keeps its source's folder name) | a public tree, one pinned commit (below) |
| `psyq-4.5/lib/` | Five files of Sony's PSY-Q 4.5, the release these programs link, as Sony shipped them: four libraries and the start-up object. Optional: only `make CODE=sdk` reads them | `recipe/toolchain.sh sdk`, from a public tree at one pinned commit (further below) |

It needs `curl`, `tar`, `git`, `python3`, `make` and a C compiler (for
binutils), and `docker` with BuildKit plus `bison` 3 (for the 1997 snapshot).
The compilers are static 32-bit x86 programs, so the kernel must be able to
run those.

- `recipe/toolchain.sh check` reports what is there and builds nothing.
- `recipe/toolchain.sh sdk` fetches one more piece, five of Sony's SDK
  files.
  Only `make CODE=sdk` needs it ("A second source for the routines", below).
- `recipe/toolchain.sh binutils` (or `gcc`, `gcc-970318`, `maspsx`, `psyq`)
  does one piece.
- `TOOLCHAIN_DIR=/some/where` keeps the toolchain outside the checkout. Set
  it for `make` too: the Makefile and the build scripts read the same
  variable.

A whole run took 32 seconds on the machine this was written on, with
docker's cache already holding the snapshot compiler's build. A first build
of that compiler takes longer; it was not timed cold.

### Two things the script does that a download would not

**It fixes one test in the 2.8.1 compiler.** GCC 2.8.1 rewrites `x + reg`
into `x - reg'` when the register holds a negative constant, and its source
excludes the one value that is its own negation, `0x80000000`. The released
binary was built with a modern GCC that optimised that exclusion away, so it
compiles `x + 0x80000000` to a subtract where the game has an add.
`recipe/patch_cc1.py` restores the test in eight bytes, refuses any binary
whose hash it does not know, and proves the result by compiling a probe.

**It builds a compiler that was never released.** The later SDK libraries
were built with neither 2.7.2 nor 2.8.0 but with something between them.
`recipe/gcc-970318/build.sh` has the measurements that date it to GCC's
development tree of 1997-03-18. The script checks the result against the
hash of the build those measurements were made with and says so if yours
differs; `make` ending with OK is what decides.

### Sony's headers

`decomp/SLUS_006.28/include/psyq_types.h` includes eleven PSY-Q headers by
name (`libgte.h`, `libgpu.h`, `libspu.h`, `libetc.h`, `libpad.h`, `libsnd.h`,
`libmath.h`, `libgs.h`, `libapi.h`, `kernel.h`, `libcd.h`), `ankprint.c` includes
`stdarg.h`, and the overlay's `libpress_libpress.c` and `movie.c` include
`libpress.h`. They are Sony's and this repository does
not carry them.

By default the script fetches them from
[Zackmon/lunar2-psx-decomp](https://github.com/Zackmon/lunar2-psx-decomp)
(`include/psyq4.4`, commit `3798b47d`), the tree the match was proven with,
and checks all 56 files against a recorded hash. The folder's name says 4.4,
but its headers are release 4.7: each says so in its `$PSLibId` line. The
programs link release 4.5, and 4.5's headers build the same three images
(tried on 2026-10-20 with FoxdieTeam/psyq_sdk's `psyq_4.5/include`, once its
line ends were made LF: the old preprocessor does not join a line that ends
in `\` and a carriage return).

To use a copy of your own, set `PSYQ_INCLUDE` to its directory for both the
script and `make`:

```sh
PSYQ_INCLUDE=/path/to/psyq/include recipe/toolchain.sh
PSYQ_INCLUDE=/path/to/psyq/include make programs
```

## 2. The program images

`decomp/<serial>/expected/code.bin` is the image a tree is matched against.
It is the game's code, so it is never in this repository.

```sh
python3 recipe/extract.py <image, .cue or folder> [more...]
python3 recipe/extract.py --check
```

Give it either game disc and the bonus disc. It reads raw 2352-byte images
(the usual `.bin` of a `.bin`/`.cue` pair) and 2336- or 2048-byte ones, and
needs nothing but Python 3.

Nothing is keyed by a disc's name. The script reads each disc's boot file
and `MENU.BIN`, and keeps a result only when its sha1 is the one the tree's
`splat.yaml` declares. A wrong disc, another region's pressing or a bad dump
writes nothing.

| Tree | Its image is |
|---|---|
| `SLUS_006.28` | The boot file of either game disc is a small stub followed by a compressed stream at file offset `0x1000`. The stream unpacks to a second executable; the image is that executable's body. |
| `SLUS_006.28-MENU` | `MENU.BIN` from either game disc, whole. It has no header. |
| `SLUS_009.21` | The bonus disc's boot file is a plain executable; the image is its body. |

## 3. make

```sh
make                                   # the main executable (SLUS_006.28)
make TARGET_SERIAL=SLUS_006.28-MENU    # the movie-player overlay
make TARGET_SERIAL=SLUS_009.21         # the bonus disc
make programs                          # all three, stopping at the first failure
make clean                             # remove one program's build output
make CODE=sdk programs                 # the same builds, taking from Sony's SDK what it holds
make sources                           # where the last build's bytes came from
make status                            # build all three, then measure them (docs/progress.md)
```

`make -j16 programs` from nothing took 2.9 seconds on the same machine. The compilers
print warnings for some files; they are the 1997 compilers' opinions of
decompiled C and do not affect the result.

If a build stops with `expected/code.bin is missing`, do step 2. If it stops
with a compiler that does not exist, do step 1.

## How a file is built

`recipe/compile.py` turns one C file into assembly, and GNU as assembles it:

```
cpp | cc1 (GCC 2.8.1, -O2 -G8) | maspsx | [delay-slot passes] | mipsel-linux-gnu-as
```

A C file that holds nothing but `INCLUDE_ASM(...)` lines skips the compiler
and becomes a list of `.include` lines.

The original was not built with one compiler and one set of flags, so the
recipe is not either:

- **Per file**, `config/cc_flags.txt` adds compiler flags and two markers:
  `@delay-fill` runs the assembler-style delay-slot fill
  (`recipe/delayfill.py`), `@cc272` compiles the file with GCC 2.7.2.
- **Per function**, a comment in the C or a list under `config/` picks a
  recipe: `/* cc97_lib: func_800953C0 */` compiles that one function with
  the 1997 snapshot and splices it into the file's output. The other names
  are in `compile.py`, each with the measurement that justified it:
  `cc272`, `cc272_db`, `cc272_lib`, `no_delayed_branch`,
  `epilogue_load_slot`, `return_slot_fill`, `arg_slot_fill`,
  `load_hazard_nop`, `la_absolute`, `gnu_as_loads`, `zero_remat`,
  `reorder_fill`, `no_split_addresses`, `no_schedule_insns`, `lib_g0`,
  `inline_functions`.
- **Per symbol**, `config/far_syms.txt` (or a `/* far_syms: ... */` comment)
  names small globals the original reached with a full address where the
  compiler would have used `$gp`.

One recipe has no compiler here: `cc272_lib45` needs a patched 2.7.2 that
`toolchain.sh` does not build, because no function in these three trees uses
it. `compile.py` says so if one ever does.

## What is still assembly

A function that is not C yet is a file under `asm/nonmatchings/`, and the C
file it belongs to pulls it in with `INCLUDE_ASM`. Data that no C file
defines is under `asm/data/`. Neither holds the game's bytes:

```
nonmatching movie_slice_load, 0xF4

glabel movie_slice_load
    .incbin "expected/code.bin", 0x7DC, 0xF4
endlabel movie_slice_load
```

The numbers are an offset into your `expected/code.bin` and a length. The
assembler finds the file because the Makefile passes `-I decomp/<serial>`.

To convert a function, replace its `INCLUDE_ASM` line with C, delete its
`.s` file, and run `make`. To read a function's instructions, disassemble
the range from your own image, for example:

```sh
toolchain/binutils/bin/mipsel-linux-gnu-objdump -D -b binary -m mips:3000 -EL \
    --adjust-vma=0x80196000 --start-address=0x801967DC --stop-address=0x801968D0 \
    decomp/SLUS_006.28-MENU/expected/code.bin
```

(`--adjust-vma` is the tree's load address, the `vram` in its `splat.yaml`.)

## A second source for the routines: Sony's SDK

A PlayStation program links Sony's PSY-Q libraries into itself, and the
routines of those libraries that Sony wrote in assembly (the BIOS call
stubs, the coprocessor routines, `setjmp`) are in the game exactly as they
are in the SDK. So there are two places to read those bytes from: your disc,
and the SDK.

```sh
recipe/toolchain.sh sdk     # once: fetch the libraries
make CODE=sdk programs      # build, reading from them what they hold
```

The second command must print the same three OK lines as `make programs`,
and does. That is the proof that the two sources hold the same bytes.

**What is fetched.** Five files of PSY-Q 4.5, in Sony's own formats and as
Sony shipped them: the libraries `libapi.lib`, `libc2.lib`, `libcard.lib`
and `libgte.lib`, and the start-up object `2mbyte.obj` (389 KB together),
from the public tree
[FoxdieTeam/psyq_sdk](https://github.com/FoxdieTeam/psyq_sdk), folder
`psyq_4.5/lib`, at one pinned commit. The script checks each file's sha256.
They are Sony's and are never in this repository. 4.5 is the release these
programs link ([docs/naming.md](docs/naming.md) has the measurement), so the
routines are read from the very files the game was linked from.

**A copy of your own.** Set `PSYQ_SDK` to a folder that holds those five
files in `lib/`, under those lower-case names, for the script and for
`make`. They have to be these same files: the stubs name offsets inside
them, and the script refuses another.

**How it is laid out.** `decomp/<serial>/asm/` is the disc as a source.
`decomp/<serial>/from-sdk/asm/` holds the same paths for the routines the
SDK can supply, as byte ranges of its files, with the references each
routine makes pointed at this tree's symbols. A `.LIB` file keeps a
placeholder in every field the linker fills, so a stub does not read those
bytes: it leaves zeros there (`.byte 0, 0`) and a `.reloc` line says what
the field points at. `CODE=sdk` puts that folder
first in the assembler's search, so a routine it holds is read from the SDK
and any other still comes from the disc. The C does not change. A build
with `CODE` set writes to a folder of its own (`build/<serial>-sdk/`), so
objects made from one source are never linked into the other's image.

**What it covers.** In the main executable, all 65 routines that were never
C: 2,304 bytes, of which 2,083 are read from Sony's files and 221 are fields
the linker fills. They are listed in
`decomp/SLUS_006.28/from-sdk/README.md`. `make sources CODE=sdk` prints the
split:

```
build/SLUS_006.28-sdk: bytes that are not compiled from C, by source
  routines        2,083 from Sony's SDK (65 files); 252 of zero padding; 221 filled in by the linker
  jump tables       220 from your disc (10 files)
  data           74,670 from your disc (23 files)
```

No routine comes from the disc. The jump tables and the data still do, and
with the data come eight bytes that are really a routine of Sony's:
`__main`, which the first split took for data (`docs/naming.md`, "Open").

The overlay and the bonus disc have no `from-sdk/` folder yet, so `CODE=sdk`
builds them from the disc alone.

`CODE` is a list. A further source would be another `from-<name>/` folder
and another name in the Makefile's `KNOWN_SOURCES`; the disc stays the last
place looked in.

## The files under config/

| File | Read by | What it says |
|---|---|---|
| `program.ld` | the linker, `compile.py` | Which object goes where. Hand-maintained; it also declares `_gp`. |
| `symbol_addrs.txt` | `compile.py` | Every named symbol's address. The build reads `name = address`; the rest of each row is the register of names: where a name came from, what backs it, what the symbol was called before ([docs/naming.md](docs/naming.md)). |
| `undefined_funcs_auto.txt`, `undefined_syms_auto.txt` | the linker | Addresses of symbols no object defines. |
| `label_fixups.txt`, `alias_syms.txt` | the linker | Labels reached across objects; second names for one address. |
| `cc_flags.txt` | `compile.py` | Per-file compiler flags and markers. |
| `far_syms.txt` | `compile.py` | Small globals addressed without `$gp`. |
| `cc272.txt`, `no_delayed_branch.txt`, `epilogue_load_slot.txt`, `return_slot_fill.txt`, `arg_slot_fill.txt`, `load_hazard_nop.txt` | `compile.py` | Per-function recipe lists (a tree has only the ones it uses). |

Four more files there are records, not inputs: nothing in the build reads
them. `asm_only.txt` lists the functions that were assembly in the original
program, each with its evidence; `vendor_provided.txt` those whose bytes are
an exact copy of a Sony library object; `bios_thunks.txt` the kernel call
stubs; `object_pins.txt` the SDK object files found whole in the image. They
were generated by tools that are not in this repository.

`splat.yaml` is not a build step. It records the first split of the image
and is where the build reads the load address, `$gp` and the image's sha1.
The C, the assembly stubs and the linker script have been maintained by hand
since; running splat over a tree would undo that.

## What OK proves, and what it does not

OK proves that the bytes this repository's C compiles to, laid out by this
linker script, equal the original image, and that every byte range the
assembly stubs name is placed where it came from.

It does not prove that the C means what its names and types say. A byte
match proves the bytes: a wrong name or a wrong struct layout that happens
to produce the same instructions passes. The specification under `docs/`
records what was established about meaning, and how.
