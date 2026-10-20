# History of the decompilation

This repository begins with no git history of its own. The work in it was done
between 18 March and 11 October 2026 in an earlier, private repository of 4,181
commits. That repository also held tools, applications and notes that were not
brought here, so its history stayed behind. This page records that history as
far as it concerns the three programs kept here.

The page has two parts: a dated list of milestones, and tables of how many
functions of each program were still assembly, week by week. Each milestone date
is the date of a commit in the old repository, and the short hash beside it
names that commit. These hashes belong to the pre-split repository, which is not
published; they are here so that anyone who holds it can check a line. Every
count in the tables was measured from the old commits by a script (the method is
below); none was copied from a commit message or a document.

## The three programs

| Tree | What it is |
|---|---|
| `SLUS_006.28` | The main executable of *Lunar: Silver Star Story Complete* (PlayStation) |
| `SLUS_006.28-MENU` | `MENU.BIN`, the movie-player overlay that the main executable loads |
| `SLUS_009.21` | The bonus disc, *The Making of Lunar*, which holds the hidden minigame *Lords of Lunar* |

A name such as `SLUS_006.28` is a serial: the product code that names a disc's
boot file. The overlay's tree is keyed by the serial of the program that loads
it, with a suffix.

## Terms

- **Byte-matching.** A byte-matching decompilation is C source that, built with
  the original compiler and options, produces exactly the bytes of the original
  program. The build proves it on every run: it links the whole image and
  compares it with the original.
- **Still assembly.** A function that has not been written in C yet is kept as
  one assembly file in a directory named `nonmatchings`, and a C source file
  pulls it in with an `INCLUDE_ASM` line. Converting a function means replacing
  that line with C that still matches, and deleting the file.
- **Segment.** A contiguous range of a program that the splitting tool, splat,
  turned into one source file and one `nonmatchings` directory.
- **Translation unit.** One C source file as the compiler sees it; it becomes
  one object file. The original programs were built from many translation units,
  and the compiler makes some choices per unit, so the source files here had to
  be cut where the original ones were.
- **PSY-Q.** Sony's development kit for the PlayStation. Part of every program
  here is PSY-Q library code, linked from object files that came with the kit.
- **Standing up a tree.** Splitting a program into per-function assembly so that
  it builds and matches before any function is converted.

## Milestones

### March 2026: before the decompilation

- **2026-03-18** (`357be8cf`). The first commit: a viewer for the assets on the
  game's disc, and four pages of format notes.
- **2026-03-18** (`aa25305a`). Code analysis is added: a description of the
  executable, a symbol database and notes on each subsystem. The disc file is a
  small loader and a compressed image; unpacked, the image is the 688,128 bytes
  (0xA8000) that the build later reproduces. There is no decompilation tree yet,
  and no commit follows until 2026-07-23.

### 23 July 2026: the decompilation enters the history

- **2026-07-23** (`59282aa7`). The first commit that holds a decompilation tree,
  then under `lunar-source-code/`. It arrives with work already done, which the
  history cannot date more finely than this commit. The build links the whole
  image of the main executable and compares all 0xA8000 bytes with the original.
  The build notes record the original compiler as identified: GCC 2.8.1 for the
  PlayStation at `-O2 -G8`, in the build published by decompals/old-gcc, with
  mkst/maspsx adapting its output for the GNU assembler. And 28 functions of the
  battle segment are matching C; the notes name `battle_apply_damage` as the
  first function matched. The tree has 23 source files and 1,716 assembly files.
- **2026-07-23** (`9e901f2a`). The tree moves to `decomp/`, and the assembly for
  the data sections is tracked from this commit.
- **2026-07-23** (`85e02481`). The first conversions made inside the history:
  five battle functions. At the end of the day 1,678 files remain, and no commit
  follows until 2026-08-02.

### 2 to 5 August 2026: the main program takes its shape

- **2026-08-02** (`ca092850`). The battle source file is cut into two
  translation units. Functions whose logic was right had failed to match because
  one file can give a global variable only one addressing mode, and the original
  used both, in different units. The first functions outside the battle segment
  are converted the same day (`f18fbe0d`).
- **2026-08-02** (`59948745`). Seven segments that the splitter had written as
  one assembly file each are split into one file per function. None of their
  functions could be converted before this. The count of files rises from 1,659
  to 2,174, the highest it reaches.
- **2026-08-02** (`9996ad9c`, `3db71eef`, `48378e9d`). Part of the library code
  is found to have been built with one optimisation pass turned off. Compiler
  options become settable per source file, and the library segments are cut into
  one source file per original object. The main program's 23 source files are
  150 by the end of the day.
- **2026-08-02** (`aa6db5be`) and **2026-08-05** (`580a31c9`). The PSY-Q library
  code is set aside as a later job, and three days later it is brought back into
  scope.
- **2026-08-03** (`ecb9777f`). Functions that the splitter had cut in two at
  round-number segment boundaries are found. Fourteen are merged back into one
  file each the same day.
- **2026-08-05** (`35cf8d92`). The compiler identification is checked against
  the build of a finished PlayStation decompilation of another game,
  mgs_reversing, which uses the same options.

### 6 to 12 August 2026: one tree per program, and the bonus disc

- **2026-08-06** (`9dd5839b`, `1be2c815`). `decomp/` is keyed by program: the
  main executable's tree moves to `decomp/SLUS_006.28/`, and the build takes the
  program's serial as a parameter, so one recipe builds any tree.
- **2026-08-06** (`4773f7ba`). `decomp/SLUS_009.21/` is stood up: a per-function
  split of the bonus disc's executable, whose image of 0x125800 bytes matches.
  One function is converted in the same commit, and 403 files remain.
- **2026-08-10** (`31e01aa6`). The bonus disc's single source file is cut into
  61, so that functions built with different compiler options sit in different
  files.
- **2026-08-10** (`00477bcb`). The compiler of the PSY-Q library objects is
  identified as GCC 2.7.2, and it becomes selectable per function.
- **2026-08-10** (`ab5a4989`). Functions whose bytes are an exact copy of a
  PSY-Q library object are listed as provided by the vendor and set apart from
  the conversion work.
- **2026-08-11** (`f91e886a`). The last conversion in the bonus disc's tree. Its
  count is 265 and stays there until the cut-over.

No commit of any kind is dated between 2026-08-12 (`d95112e9`) and 2026-10-02
(`13938fef`), a gap of seven weeks. September 2026 has none.

### 2 to 7 October 2026: work resumes

- **2026-10-02** (`e78dfe66`, `e1617692`). Twenty-six assembly files that each
  held more than one function are split, which adds 38 files.
- **2026-10-03** (`13711255`). The stubs that call the console's BIOS are
  recognised as hand-written assembly from the development kit and taken off the
  conversion list.
- **2026-10-06** (`33bb48e4`). Jump tables start to move out of the data file at
  the head of the image and into the read-only data of the source file whose
  function uses them, one converted function at a time.
- **2026-10-07** (`c1c22230`). A fault in the rebuilt compiler is repaired. The
  GCC 2.8.1 binary had itself been compiled by a modern GCC, which removed one
  overflow guard from it, so it emitted a subtraction where the game has an
  addition. A patch step restores the guard.

### 8 to 10 October 2026: the main program closes

- **2026-10-08** (`85328c75`, `d86575cd`). A commit made from a nearly empty
  index deletes 3,074 files, the decompilation trees among them. The next
  commit, dated the same minute, restores them from the parent commit.
- **2026-10-08** (`8f489817`, `2609c353`, `064dbee7`). The build recipe gains
  three settings read from the bytes: the behaviour of version 2.60 of the
  original assembler, and two more compilers for newer library objects, a
  variant of GCC 2.7.2 and a GCC development snapshot of early 1997.
- **2026-10-08** (`afea351c`). The menu segment is the first whose
  `nonmatchings` directory empties. The save segment follows the same day
  (`3e9c4368`).
- **2026-10-08** (`58834c0b`). A second tree, `documented/`, is started for
  readable source that must compile to the same objects as the matching one. It was
  removed on 2026-10-14, when names were being given in `src/` itself: one
  file had been carried over, and its record types are kept for the pass
  that names struct fields.
- **2026-10-09** (`0b3dde30`). The first split had ended the code too early, and
  57 library functions were sitting in a data file. They become assembly files,
  and the count rises from 263 to 320 before it falls again.
- **2026-10-09** (`12640201`). The battle segment empties. Twenty of the 23
  segment directories reach their final count on this day, among them scripting,
  which had started with 324 files (`7e75903c`).
- **2026-10-09** (`88b18819`, `103749dd`). Two library functions that had been
  converted are returned to assembly, twice in one day: they were hand-written
  assembly in the original, so there is no C to recover.
- **2026-10-10** (`e2afd0f3`). The last function converted in the main program,
  `entity_update_00`. One test in it could not be recovered from the bytes, so
  the source holds a stand-in with a comment that says so. 65 files remain, and
  the count does not move again.
- **2026-10-10** (`2fb97c4d`, `25d375fb`). `decomp/SLUS_006.28-MENU/` is stood
  up with 36 function files. After the last conversion of the evening 3 remain.

Two other programs were split in the old repository, *Lunar 2: Eternal Blue
Complete* (`SLUS_010.71`, stood up on 2026-10-10) and the Japanese original
(`SLPS_013.97`, a split configuration only); both stay in the private workshop.

### 11 October 2026: the cut-over

- **2026-10-11** (`1c4789a2`). The bonus disc's tree is repaired. The commit
  records that its image had come out 16 bytes too long since 2026-10-08, when a
  change to the compiler driver (`2609c353`) was checked against the main
  program only. Two configuration lines are removed and the tree matches again.
- **2026-10-11** (`6a27816d`). The last commit of the old repository, and the
  point this repository was cut from. It records the plan to split the old
  repository.

### 14 to 19 October 2026: this repository

This repository was made from the old one's tree on 2026-10-13 and became a
git repository the next day. Its own 41 commits named the functions, the
globals, the parameters and the structs, re-cut the source files (Sony's
one library member to a file, the game's where the subsystem changes), and
brought the specification's names and files up to date; `docs/naming.md`
and `docs/naming-log.md` are the record of what each step rests on. Before
the first publication the 41 commits were squashed into one, on
2026-10-19, after five files of initialized data were found to carry about
260 bytes of the program's own data (the build stamp, debug texts and two
small tables); those bytes are now read from your image like the rest. The
commits, as they were:

| Date | Commit |
|---|---|
| 2026-10-14 | Matching decompilation of Lunar: Silver Star Story Complete, with names |
| 2026-10-14 | Name 43 of the functions first left unnamed, by the owner's ruling |
| 2026-10-14 | Bring the docs up to the names, fix comments found wrong, split the naming record |
| 2026-10-14 | Take the old work log out of the C's comments |
| 2026-10-14 | Name the 196 most-used globals |
| 2026-10-14 | Name the next 195 globals by use |
| 2026-10-14 | Name the three variables of published source the game carries |
| 2026-10-14 | Name the next 193 globals by use |
| 2026-10-14 | Name the parameters and locals of misc_j.c, and two of Sony's parameters |
| 2026-10-14 | Name the parameters and locals of the main program's game code |
| 2026-10-14 | Give printed parameters the developers' labels |
| 2026-10-14 | Sweep the last placeholder names inside the game's functions |
| 2026-10-14 | Give the battle record one layout: BtlRecord and SpriteEntry |
| 2026-10-14 | Give the map's records and objects and the characters' stats one layout each |
| 2026-10-14 | Give the game's remaining structs one layout each, by subsystem |
| 2026-10-14 | Define Sony's library structs once, in psyq_private.h |
| 2026-10-14 | Replace fifteen more of the first split's views by hand |
| 2026-10-14 | Give the battle's windows their true types |
| 2026-10-14 | Replace the last nine of the views left for a person |
| 2026-10-17 | Write 158 raw offset accesses as members of their true types |
| 2026-10-17 | Write 1,078 more raw offset accesses as members |
| 2026-10-18 | Give the last records with known layouts their types |
| 2026-10-18 | Say where psyq_types.h's Sony headers come from |
| 2026-10-18 | Give Sony's last types their own names and layouts |
| 2026-10-18 | Name the overlay's libpress by Sony's 4.1 labels |
| 2026-10-18 | Name the bonus disc's own code under the game |
| 2026-10-18 | Name the overlay's and the bonus disc's players and the disc's minigame |
| 2026-10-18 | Re-cut Sony's source files, one proven library member per file |
| 2026-10-18 | Name 287 more globals: the fourth data wave |
| 2026-10-18 | Name command 1's handler; read the 61 leftover functions again |
| 2026-10-18 | Cut the game's own files where the subsystem changes |
| 2026-10-19 | Reach labels inside records through the record; cite today's files in notes |
| 2026-10-19 | Name Sony's statics by Sony's text and earlier releases' bytes |
| 2026-10-19 | Bring the specification's names and files up to date |
| 2026-10-19 | Give the last five first-split files' code a file per member |
| 2026-10-19 | Write 220 more raw offset accesses as members of their true types |
| 2026-10-19 | Write the last raw offsets outside the script commands as members |
| 2026-10-19 | Name the main program's last argN parameters, or say why they stay |
| 2026-10-19 | Give the memory card save its type, CardSave |
| 2026-10-19 | Name the memory card file's head by the card's file format |
| 2026-10-19 | Record the outstanding tasks in README |

## How the counts were measured

A function still in assembly is one `.s` file under a `nonmatchings` directory.
A script walked all 4,181 commits of the old repository in order, listed each
commit's tree with `git ls-tree -r --name-only` (nothing was checked out), and
counted those files per program. The directory moved twice, so three paths were
counted: `lunar-source-code/asm/nonmatchings/` (the first tree commit only),
`decomp/asm/nonmatchings/` (2026-07-23 to 2026-08-06) and
`decomp/<serial>/asm/nonmatchings/` (from 2026-08-06).

A week runs from Monday to Sunday, and its row is the count at the last commit
dated on or before that Sunday. As a cross-check the script also counted
`INCLUDE_ASM(` lines in the C sources at the last commit of each day. On the 22
days that have a tree, the two counts agree on 19 and differ by one or two on
the other three.

The tables give what remains. They do not give a count of functions converted,
because the number of function files also changed when files were split or
merged, so there is no fixed total to subtract from.

## Functions still in assembly, by week

A dash means the program had no tree yet. "Commits" counts every commit of the
old repository in that week, not only those that touched the decompilation.

| Week ending (Sunday) | Commits | `SLUS_006.28` | `SLUS_006.28-MENU` | `SLUS_009.21` | Note |
|---|---:|---:|---:|---:|---|
| 2026-03-22 | 2 | - | - | - | no decompilation tree yet |
| 2026-03-29 to 2026-07-19 | 0 | - | - | - | 17 weeks without commits, shown as one row |
| 2026-07-26 | 49 | 1,678 | - | - | tree first committed on 2026-07-23 with 1,716 |
| 2026-08-02 | 115 | 1,885 | - | - | a rise; see the next section |
| 2026-08-09 | 510 | 1,569 | - | 302 | bonus-disc tree stood up on 2026-08-06 with 403 |
| 2026-08-16 | 190 | 1,566 | - | 265 | last commit on 2026-08-12 |
| 2026-08-23 | 0 | 1,566 | - | 265 | no commits; carried |
| 2026-08-30 | 0 | 1,566 | - | 265 | no commits; carried |
| 2026-09-06 | 0 | 1,566 | - | 265 | no commits; carried |
| 2026-09-13 | 0 | 1,566 | - | 265 | no commits; carried |
| 2026-09-20 | 0 | 1,566 | - | 265 | no commits; carried |
| 2026-09-27 | 0 | 1,566 | - | 265 | no commits; carried |
| 2026-10-04 | 215 | 1,420 | - | 265 | work resumed on 2026-10-02 |
| 2026-10-11 | 3,100 | 65 | 3 | 265 | overlay tree stood up on 2026-10-10 with 36 |

Commits fall on only 23 days, so the weekly rows hide most of the movement. The
same count at the last commit of each day that has commits, from the first day
with a tree:

| Date | Commits | `SLUS_006.28` | `SLUS_006.28-MENU` | `SLUS_009.21` |
|---|---:|---:|---:|---:|
| 2026-07-23 | 49 | 1,678 | - | - |
| 2026-08-02 | 115 | 1,885 | - | - |
| 2026-08-03 | 82 | 1,796 | - | - |
| 2026-08-04 | 110 | 1,723 | - | - |
| 2026-08-05 | 64 | 1,668 | - | - |
| 2026-08-06 | 138 | 1,610 | - | 386 |
| 2026-08-07 | 34 | 1,587 | - | 332 |
| 2026-08-08 | 55 | 1,572 | - | 311 |
| 2026-08-09 | 27 | 1,569 | - | 302 |
| 2026-08-10 | 100 | 1,566 | - | 278 |
| 2026-08-11 | 79 | 1,566 | - | 265 |
| 2026-08-12 | 11 | 1,566 | - | 265 |
| 2026-10-02 | 99 | 1,518 | - | 265 |
| 2026-10-03 | 90 | 1,434 | - | 265 |
| 2026-10-04 | 26 | 1,420 | - | 265 |
| 2026-10-05 | 161 | 1,292 | - | 265 |
| 2026-10-06 | 307 | 1,079 | - | 265 |
| 2026-10-07 | 162 | 974 | - | 265 |
| 2026-10-08 | 1,263 | 454 | - | 265 |
| 2026-10-09 | 1,066 | 66 | - | 265 |
| 2026-10-10 | 134 | 65 | 3 | 265 |
| 2026-10-11 | 7 | 65 | 3 | 265 |

## Where the count moved without a conversion

The count is of files, and a file can appear or disappear without a function
being converted. Every rise in the series is listed here; only the first shows
in the weekly table.

- **Files split.** On 2026-08-02 seven whole-segment files became 522 function
  files (`59948745`, 515 more). On 2026-10-02 twenty-six files that held several
  functions were split (`e78dfe66`, `e1617692`, 38 more). Single files were
  split on 2026-10-07 (`f11a131d`) and twice on 2026-10-09 (`2a006644`,
  `d1a825b3`), one more each.
- **Functions found in data.** On 2026-10-09, 57 library functions past the old
  end of the code became files (`0b3dde30`).
- **Conversions undone.** One on 2026-08-04 (`b67aebcb`), and the same two
  functions twice on 2026-10-09 (`88b18819`, `103749dd`).
- **The deleted tree.** On 2026-10-08 the count is zero for one commit
  (`85328c75`) and back at 865 and 265 in the next (`d86575cd`).
- **New trees.** A tree enters the series with what it holds when first
  committed: 1,716 on 2026-07-23, 403 on 2026-08-06 and 36 on 2026-10-10.

The count also fell without a conversion when the two halves of a cut function
were merged into one file, as fourteen were on 2026-08-03.

## State at the cut-over

At the last commit of the old repository (`6a27816d`, 2026-10-11):

- **`SLUS_006.28`**: 65 function files remain, in five of the 23 segment
  directories: one in the boot segment and 64 in the four segments named for the
  PSY-Q libraries. All 65 are on the tree's own list of functions that were
  assembly in the original program (`config/asm_only.txt`), so by that record
  no function that was C is left in assembly. The source is 170 files.
- **`SLUS_006.28-MENU`**: 3 remain. Two are on its list of functions that
  were assembly in the original; one is not yet matched.
- **`SLUS_009.21`**: 265 remain. 33 are listed as assembly in the original
  (`config/asm_only.txt`) and 90 as exact copies of PSY-Q library objects
  (`config/vendor_provided.txt`), with two on both lists. The other 144 are
  open work.

The images linked from the three trees at that point carry 2,320, 36 and 402
function symbols. That figure comes from building the trees and reading the
linked files' symbol tables, not from the commit walk. Symbols are not quite
functions: the first count includes four alternate entry labels and leaves out
two routines that are labelled as data, and the last leaves out two such
routines. Counted as functions the three programs have 2,318, 36 and 404, of
which 2,253, 33 and 139 are C.
