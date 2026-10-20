# Notices — eternal-silver-sssc-decomp

eternal-silver-sssc-decomp is a matching decompilation of *Lunar: Silver Star
Story Complete* for the PlayStation, with its build recipe and the
specification written alongside it. Its author's work in it is MIT-licensed
(`LICENSE`), © 2026 Bash Alarmist; "What the licence covers", below, says what that is
and what it is not. This page credits what the work stands on.

## The game

*Lunar: Silver Star Story Complete* is its owners': © 1992–1998 GAME ARTS;
© 1996–1998 Kadokawa Shoten / ESP / JAM; English translation © 1999 Working
Designs — as the original's title screen prints it. The game is its program
as much as its pictures, its music and its words.

This repository holds none of the game's files. There is no disc image, no
file of the disc and no program image in it. What is still assembly is kept
as labels and byte ranges (`decomp/<serial>/asm/`), and the build reads those
bytes from an image the builder cuts out of their own disc
(`recipe/extract.py`).

The C's comments, the notes in the symbol registers
(`decomp/<serial>/config/symbol_addrs.txt`) and the specification quote short
messages the programs print: debug and error texts of the game and of Sony's
libraries. Each is quoted where it is the evidence for a name or a reading.

## What the licence covers

The MIT licence is its author's grant, and covers what is its author's to
grant: the build recipe (`recipe/`, the `Makefile`), the prose of the
specification (`docs/`), `README.md`, `BUILD.md`, `HISTORY.md`, and whatever
in the rest is the author's own.

Files that open with an `SPDX-License-Identifier: MIT` line hold, besides
the author's work, parts that match other people's MIT-licensed
decompilations of Sony's libraries ("Other decompilations", below). Those
parts are their authors', under the notices in `LICENSES/`.

It does not cover the game's program. The C under `decomp/<serial>/src/` is a decompilation: it was written to
compile to the program's exact bytes, function by function, and it derives
from that program. So do the layout facts under `decomp/<serial>/config/`.
The rights in the program stay its owners', and the licence grants nothing
in it.

The same holds for Sony's PSY-Q libraries as the programs link them. The
files named after a library and one of its members (`libgpu_sys_1.c`,
`libapi_a67.c`, the overlay's `libpress_vlc.c`) and the start-up object's
`2mbyte.c` are these programs' own copies of functions of libapi, libc2, libcard, libcd, libetc,
libgpu, libgs, libgte, libmcrd, libpad, libpress, libsnd and libspu,
decompiled like the rest. They are Sony Computer Entertainment's code.

## Sony's SDK headers

The C includes thirteen of Sony's PSY-Q headers by name, in release 4.7 as the
build fetches them. They are © Sony Computer Entertainment Inc. / SN Systems
Ltd. and are not in this repository.
`recipe/toolchain.sh` fetches them, unmodified, from
[Zackmon/lunar2-psx-decomp](https://github.com/Zackmon/lunar2-psx-decomp)
(`include/psyq4.4`, commit `3798b47d`), or the builder supplies a copy
(`PSYQ_INCLUDE`). Where a C file declares an SDK function it calls, that one
line reads as the SDK's own prototype does. The blocks of SDK declarations
the files once carried beyond that (about 12,400 lines, copies of what the
headers already declare) were removed on 2026-10-13, every object unchanged.

## Sony's SDK libraries

`make CODE=sdk` reads the routines Sony wrote in assembly from Sony's own
library files of PSY-Q 4.5, the release these programs link. They are
© Sony Computer Entertainment Inc. / SN Systems Ltd. and are not in this
repository: `recipe/toolchain.sh sdk` fetches five of them, on request,
unchanged and in Sony's own format, from
[FoxdieTeam/psyq_sdk](https://github.com/FoxdieTeam/psyq_sdk) (its
`psyq_4.5/lib`, at one pinned commit), or the builder supplies those same
files (`PSYQ_SDK`). The stubs under `decomp/SLUS_006.28/from-sdk/` hold file
names, offsets, lengths and this tree's symbol names.

The C calls the SDK's functions by Sony's own names (`ResetGraph`,
`SpuSetReverb`, `strcpy`): 493 in the main executable, 286 on the bonus
disc and 15 in the overlay. Most are the public symbols in the symbol
tables of the release each program links (4.5; the overlay's libpress is
4.1's), each given only where the function's bytes in the program are that
library member's, or a member proven that way calls it by that name. The
others come from Sony's own work too, where the linked release gives a
function no name: a later or an earlier release that exports the same
bytes under one name, or a string of Sony's in the member that names the
function. Each tree's `config/symbol_addrs.txt` records the evidence row by
row, and [docs/naming.md](docs/naming.md) says how the releases were
identified and where the libraries and signatures were read from.

## Published code in the game

The game's developers compiled two published pieces of source code into
the main executable. This repository did not use them: its C is a
decompilation of the game's own copies, and the published files were read
beside that C only to identify the functions and name them
([docs/naming.md](docs/naming.md), "A published source file's names").
Neither file is in this repository.

- **Mark Adler's `inflate.c`**, as gzip 1.2.4 ships it. The file says of
  itself that it is not copyrighted. The game unpacks most of its data
  with its own changed copy; `huft_build` and `inflate_codes` are that
  file's functions, under its names, and `g_MaskBits`, `g_Bb`, `g_Bk`
  and `g_Wp` its variables `mask_bits`, `bb`, `bk` and `wp`.
- **Haruhiko Okumura's routines** from his dictionary of algorithms in C
  (奥村晴彦『C言語による最新アルゴリズム事典』, 技術評論社, 1991):
  `maketable`, `spline`, `maketable2`, `spline2` and `mysqrt`, which the
  developers' math file `MTH_CURV.C` carries in fixed point as
  `MTH_maketable`, `MTH_spline`, `MTH_maketable2`, `MTH_spline2` and
  `MTH_mysqrt` (`maketable`'s work arrays `h` and `d` are `g_MthH` and
  `g_MthD`, and the arrays `x` and `y` of `spline2.c` `g_MthX` and `g_MthY`). They were compared with the author's files of the 2018
  edition (`github.com/okumuralab/algo-c`), whose README says the author
  claims no rights over the source code.

## Other decompilations

Three decompilations of Sony's PSY-Q libraries are the source of much of
the C of Sony's members here. All three are MIT-licensed, and each file
that has parts matching them names them in its first lines:

- [sotn-decomp](https://github.com/Xeeynamo/sotn-decomp) (Xeeynamo and
  contributors). The project as a whole is AGPL-3.0, but each file of its
  library decompilation, `src/main/psxsdk/`, carries
  `SPDX-License-Identifier: MIT`, as it did before this work began, and
  only those files are followed here. Notice: `LICENSES/sotn-decomp.txt`.
- [psyz](https://github.com/Xeeynamo/psyz) (Xeeynamo, psyqz members and
  sotn-decomp contributors): its library decompilation, `decomp/`, under
  that folder's own MIT licence. psyz leaves its porting library
  (`psyz/include`, `psyz/src/psyq`) unlicensed; nothing here matches it.
  Notice: `LICENSES/psyz.txt`.
- [psy-q-decomp](https://github.com/sozud/psy-q-decomp) (Sozud and
  contributors). Notice: `LICENSES/psy-q-decomp.txt`.

How the parts were found: every function and struct of this repository's
C, in all three programs, was compared with every function and struct of
the three projects (sotn-decomp at commit `66a337e3`, psyz at `854d4cbb`,
psy-q-decomp at `6edf9b24`) as sequences of tokens with the names set
aside, since the C here uses its own names. 94 files have a function or
struct of 40 tokens or more that is 80% alike or more to one of theirs:
90 of the main executable's, one of the overlay's and three of the bonus
disc's. The game's own code matches none of them. Below 40 tokens a
function's text is set by its instructions and any two decompilations
agree, so short ones are not counted. Likeness is not proof of copying,
since decompilations of the same object often converge; credit is given
wherever the likeness is that high. The notes kept from the work say these
projects' texts were read beside the C, and some taken with this tree's
names.

Others informed the work without text of theirs in it:

- The PSY-Q signature database
  [psx_psyq_signatures](https://github.com/lab313ru/psx_psyq_signatures)
  (lab313ru), at commit `e9e46e7e`: its signatures are the evidence for the
  overlay's libpress names and for the `sdk-earlier` names
  (`docs/naming.md`), and were used to measure which release each program
  links.
- [ctr](https://github.com/mkst/ctr) (mkst),
  [mgs_reversing](https://github.com/FoxdieTeam/mgs_reversing) (FoxdieTeam)
  and [ygofm-decomp](https://github.com/MaChInEgUn3/ygofm-decomp)
  (MaChInEgUn3), read for technique and for corroboration of the compiler
  and its flags.

## In this repository, from other projects

- `decomp/<serial>/include/macro.inc`, `labels.inc` and `gte_macros.inc`
  are the assembler macro files [splat](https://github.com/ethteck/splat)
  (ethteck; MIT) writes for a PlayStation project, and `include_asm.h`
  follows its convention. The first split of each program was splat's, with
  [spimdisasm](https://github.com/Decompollaborate/spimdisasm).
- `recipe/gcc-970318/Dockerfile` builds its compiler the way
  [decompals/old-gcc](https://github.com/decompals/old-gcc) builds
  `gcc-2.7.2-psx`, and applies that project's patches, which the build
  fetches.

## Used to build, never in this repository

`recipe/toolchain.sh` fetches or builds each of these on the builder's
machine:

- **GCC** 2.8.1 and 2.7.2 for the PlayStation, as built and released by
  decompals/old-gcc, and GCC's development tree of 1997-03-18 from
  [gcc-mirror/gcc](https://github.com/gcc-mirror/gcc) (GPL).
- **GNU binutils** 2.42 (GPL).
- **[maspsx](https://github.com/mkst/maspsx)** (mkst; MIT).

## Used to write the C, never in this repository

[m2c](https://github.com/matt-kempster/m2c) (Matt Kempster),
[decomp-permuter](https://github.com/simonlindholm/decomp-permuter) and
[asm-differ](https://github.com/simonlindholm/asm-differ) (Simon Lindholm),
[PCSX-Redux](https://github.com/grumpycoders/pcsx-redux) (grumpycoders) and
[Ghidra](https://ghidra-sre.org/) (NSA).
