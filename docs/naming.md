# Names

How this repository gives a function a name, where the name is recorded, and
what backs it. Begun 2026-10-13, with the SDK library functions of the main
executable and of the bonus disc; the overlay's library and the bonus disc's
own code under the game followed on 2026-10-18.

## The rule

**A function's address is its identity. Its name is a claim about it.**

`func_80016608` is not a placeholder waiting to be replaced: it is the
address `0x80016608`, and it can never be wrong or go out of date. The
specification under `docs/` gives the address beside every function it
names, and Eternal Silver's engine cites functions the same way. Those
citations stay valid whatever a function is called later.

A real name can be wrong. Three game functions here carried Sony's names
`ResetGraph`, `SetDefDrawEnv` and `SetDefDispEnv` until 2026-10-08, and the
first naming pass found four more wrong names (below). So a name is never
given without a record of where it came from.

- **To cite a function**, write its address form (`func_80016608`), or its
  name with the address beside it: `layer_mix_fade_start` (`0x8001EBA4`).
  Never the name alone.
- **To find a function from an old citation**, search the C for the address
  form: every function that has been given a name carries it on the line
  above its definition (next section).
- **To find what a name in an old page means**, look it up in the register:
  every former name of a symbol is kept in its row.

## The address stays in the C

A function named for its address says where it is. Once it is called
`strcpy`, the C no longer does. So a function whose name has a recorded
source carries its address on the line above its definition, in the form the
old citations use:

```c
/* func_8008B7F8 */
char *strcpy(char *dst, char *src)
```

and above its `INCLUDE_ASM` line when it is still a byte range. The tag is
generated from the register and says nothing the register does not. A
function with an `inherited` name has none: its old citations use that name,
which is still its name.

## The register

`decomp/<serial>/config/symbol_addrs.txt` is the register. The build reads
the `name = 0xADDRESS;` part of each row (`recipe/compile.py`); the rest of
the row is the record:

```
strcpy = 0x8008B7F8; // type:func src:sdk-bytes on:2026-10-13 was:func_8008B7F8 ev: PSY-Q 4.5 libc2/strcpy.o lies whole at 0x8008B7F8 (between, exact, large); the C's comment agrees
```

| Field | Meaning |
|---|---|
| `type:`, `size:` | What the first split recorded |
| `src:` | Where the name came from (the table below) |
| `on:` | The day the name, or its source, was recorded |
| `was:` | Every name the symbol had before, newest first |
| `ev:` | The evidence, to the end of the line |

A function with **no row** is named for its address, and nothing is claimed
about it. Every function whose name is not its address has a row.

| `src:` | What backs the name |
|---|---|
| `sdk-bytes` | Its bytes are a member of Sony's library, in the release this program links, outside the fields the linker fills. The name is Sony's symbol. |
| `sdk-link` | A member proven that way calls it by this name. |
| `sdk-later` | It is part of a member proven that way, which gives it no public name; a later release of Sony's libraries exports the identical bytes under this name. |
| `sdk-earlier` | It is part of a member proven that way, which gives it no public name; an earlier release of Sony's libraries exports the identical bytes under this name (the psx_psyq_signatures set, commit `e9e46e7e`). |
| `sdk-text` | It is part of a member proven that way, and Sony's own text in that member names it: a string the function prints begins with the name (`MDEC_rest:bad option`), or a message of the member names it as the function it calls (`Read error in cd_read(PVD)`). Spelled as printed. |
| `original` | The developers' own name: the function's own assertion message begins with it, or its own debug print does and passes the function's own arguments. |
| `public-source` | A function of a published source file that the developers compiled into the program: the same parameters, and the published body statement by statement. The row names the file and lists what the program's copy leaves out or replaces. The name is the published one; where the developers' file has a prefix of its own, the published name stands behind it (`MTH_maketable`). |
| `port` | Eternal Silver's name for its transcription of the function. None yet. |
| `coined` | Given here, by reading the C. The row says what was read. |
| `inherited` | Came with the first split or its automatic naming, and has not been read since. |

**An `inherited` name is not evidence of what a function does**, so no
function has one any more. The first split left 324 of them in the main
executable: `scene_state_00`, `menu_page_02`, `FACE`, `Garbage`. About a
hundred were known to be wrong (the first split's file `menu.c` and its
`menu_*` functions are the battle's spell presentations), and nothing said which of the rest
were right. On 2026-10-13 all 324 went back to their address names. Each
keeps a row: the name it had (`was:`), and what could be checked about it.
For 193 of them that is a fact: `scene_state_03` was "entry 3 of the table
at `0x800B2EF8`", and the image's table holds that address there. A name
comes back when someone reads the function and records a real source. 36
data labels (`scene_state_table`) still carry inherited names; data has not
been looked at.

Text that cites a function by one of those names (the specification does,
about 2,000 times, and Eternal Silver's engine about 7,500) is not a reason
to keep the name: it is resolved through `was:`, and corrected when its
turn comes.

A guess is allowed. It is recorded as what it is (`coined`, with the
reasoning in `ev:`), and never as more.

### Spelling

- Sony's names and the developers' own names are spelled exactly as their
  source spells them: `ResetGraph`, `_SsVmKeyOn`, `strcpy`.
- A coined name is spelled the way the developers spelled theirs (ruled
  2026-10-13; until then coined names were all `snake_case`). They spelled
  them two ways, and which one a function gets depends on where it is:

  | Spelling | The developers' names that show it | Where |
  |---|---|---|
  | `lower_case_underscore` | `bg_copy`, `cd_load`, `xa_play`; and run together, `printmes`, `xmalloc` | the code under the game: video, CD, memory, printing |
  | `CapitalisedWords` | `MoveChrAction`, `BtlEndMessageDisp`, `ColMix`, `MapEffectSet`, `OverCheck`, `MessageConv` | the game's own code: movement, battle, map, script |
  | `CapitalisedWords` | `MemoryBreaker`, `PSchk` | memory, CD |
  | a file prefix in capitals, then a capitalised word | `MTH_Div` | math (`MTH_CURV.C`) |

  These fourteen are every developers' function name the main executable
  holds. Nine are proven to name one function (`original`); `xa_play`,
  `xmalloc`, `MessageConv`, `MemoryBreaker` and `PSchk` are in the program's
  text and are tied to no one function.

  How a unit's spelling is chosen:

  1. A developers' name proven inside the unit sets it: the functions
     beside `MoveChrAction` are spelled as it is.
  2. Otherwise by what the unit's functions work on. Sony's libraries, the
     kernel, memory, files, text output and the helpers those use:
     `lower_case_underscore`. The game's characters, battles, maps, scripts
     and menus: `CapitalisedWords`.

  The paragraph that introduces a unit under "Names given by reading" says
  which it is and why.
- In either spelling the thing comes first and the action last, as in ten
  of the developers' fourteen (`MapEffectSet`, `OverCheck`, `bg_copy`,
  `cd_load`), and the developers' abbreviation is used where they have one:
  `Btl`, `Chr`, `Disp`, `Conv`, `mes`. "Check" is spelled out: their text
  has it both ways (`OverCheck`, proven; `PSchk`, tied to no function), and
  coined names follow the proven one.
- A coined name therefore looks like one of theirs. The spelling does not
  say who gave a name; the row in the register and the address tag do.
- Types are `PascalCase` as they are in the tree already (`BattleActor`);
  constants and macros are `UPPER_SNAKE_CASE`.

## Which release of Sony's libraries the programs link

A name taken from a library is only as good as the choice of library: Sony
renamed functions between releases without changing their code. So this was
measured before anything else was trusted. (It was not measured first. The
first pass compared against release 4.7, the only one the build recipe
fetched then, and was redone.)

**The main executable links PlayStation Runtime Library release 4.5 (Sony's
change log for it is dated February 1999), every library of it.**

- *Sixteen releases, 2.6 to 4.7, laid over the image* with the published
  signature set [psx_psyq_signatures](https://github.com/lab313ru/psx_psyq_signatures).
  217 library members of 24 bytes or more lie in the image at exactly one
  address. Release 4.5 fits all 217. No other release does: 4.4 has a
  member of the same name that does not lie there in 11 cases, 4.6 in 23,
  4.7 in 24.
- *Sony's own 4.5 library files.* 264 of their members lie in the image at
  exactly one address, and they account for 116,912 of the 117,264 bytes of
  the image's library code (`0x8008B388` to `0x800A7D98`). The other 352
  bytes are nine members of the same release so small and so alike that each
  lies at several addresses. Nothing is left over. Every jump inside every
  placed member lands where the member's own relocation record says.
- *Data as well as code.* A member's code says where its data went. All 50
  data sections that could be located that way are equal to release 4.5's,
  word for word. Five libraries keep their release number in a word of data,
  and the image holds `0x00450000` in each (libetc, libgpu, libpad, libsnd,
  libspu), where the 4.4, 4.6 and 4.7 files hold their own number.

How much of each library's bytes each release accounts for:

| Library | Bytes | 4.4 | 4.5 | 4.6 | 4.7 |
|---|---:|---:|---:|---:|---:|
| libapi | 3,296 | 99.0% | 100% | 75.7% | 75.7% |
| libc2 | 1,840 | 100% | 100% | 99.6% | 99.6% |
| libcard | 1,136 | 70.4% | 100% | 100% | 100% |
| libcd | 17,760 | 55.2% | 100% | 66.9% | 66.9% |
| libetc | 3,232 | 100% | 100% | 100% | 100% |
| libgpu | 12,832 | 100% | 100% | 5.9% | 5.9% |
| libgs | 4,528 | 98.2% | 100% | 99.6% | 99.6% |
| libgte | 624 | 76.9% | 100% | 76.9% | 76.9% |
| libmcrd | 12,224 | 16.9% | 100% | 39.2% | 39.2% |
| libpad | 12,448 | 100% | 100% | 11.8% | 11.8% |
| libsnd | 33,888 | 96.4% | 100% | 94.9% | 94.9% |
| libspu | 13,104 | 100% | 100% | 100% | 98.4% |
| All | 116,912 | 82.9% | 100% | 66.6% | 66.4% |

Code pins libapi, libcd, libgs, libgte, libmcrd and libsnd to 4.5; the
release number pins libetc, libgpu, libpad and libspu. Two libraries cannot
be pinned, because the members this program links are byte for byte the same
in neighbouring releases: libc2 (4.1 to 4.5) and libcard (4.5 to 4.7). No
name depends on which.

**The bonus disc** (`SLUS_009.21`) links release 4.5 too: 124 members cover
its library code with no gap, all 49 located data sections are equal, and it
holds the same release numbers.

**The movie-player overlay** (`SLUS_006.28-MENU`) does not. Its two libpress
members fit release 4.1 of the signature set and no other, 4.5 included, and
all 19 of its library function starts fall on that release's labels. Its
build stamp reads 99-04-19, so it was built beside the main program with an
older libpress. This rests on the signature set alone: no 4.1 library file
was at hand. That set is the release's bytes with the linker's fields left
out, which is what `sdk-bytes` asks for, so it names them (2026-10-18, from
lab313ru's `psx_psyq_signatures` at commit `e9e46e7e`, `410/LIBPRESS.LIB.json`):
all 1,567 known bytes of `LIBPRESS.OBJ` equal the overlay's at `0x80196BDC` and
all 872 of `VLC.OBJ` at `0x8019731C`, and their labels give twelve names,
`DecDCTReset` to `DecDCToutCallback`, `DecDCTvlcSize` and `DecDCTvlc`.
`DecDCTvlcSize` begins four bytes into its member, after the size word it
keeps in its own `.text`, which the tree now labels as data (`D_8019731C`).
Of the seven static helpers of `LIBPRESS.OBJ`, the three that print their
own names (`MDEC_rest`, `MDEC_in_sync`, `MDEC_out_sync`) have them since
2026-10-19 (`sdk-text`, "Sony's statics" below); the other four keep their
addresses, with rows saying where they lie. The overlay's 28 names for routines of the main
program are the main program's own (`0x80090874`, which it called
`CdControlF`, is `CdControlB`). Its player, `movie.c`, is named by reading
(the rulings of 2026-10-18, under the bonus disc): `movie_play`, the entry
the main program's `MoviePlay` calls, and the `movie_` routines under it.

What the images do not show: which release's compiler and headers the game's
own code was built with.

**Where the library files came from.** Release 4.5's `.LIB` files were read
from [FoxdieTeam/psyq_sdk](https://github.com/FoxdieTeam/psyq_sdk)
(`psyq_4.5/lib`, commit `91719fa5`), and split into one object per member
with [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux)'s
`psyq-obj-parser`. They were checked against the signature set first: all
1,763 of their members with code equal its release 4.5, none differs. They
are Sony's and they are not in this repository. The build recipe fetches
five of the release's files from the same place, unchanged, for
`make CODE=sdk` (`recipe/toolchain.sh sdk`): four libraries and the start-up
object. The rest were read for this measurement only. The converter cannot
read the start-up object, so that one was read as it is.

## How the SDK names were proven

A library member that lies in the program image names the functions it
defines. Byte equality alone is not enough: a four-instruction getter is the
same bytes as every other getter once its address field is masked, and Sony
ships the same code under two names (`SpuRead` and `SpuWrite`). So a member
counts as proven in place only when all of this holds:

1. Its whole `.text` lies in the image at exactly one address, every word
   equal outside the fields its own relocation records say the linker fills.
2. It does not lie inside a bigger member that is proven.
3. At least one of: it has no relocation records, so nothing was masked
   (`exact`); 16 or more of its words are untouched by a relocation
   (`large`); another proven member jumps into it by one of its names
   (`called`); it touches a proven member of the same library, with nothing
   but zero padding between (`between`).

and a function gets the member's name for it only when

4. No proven member refers to that address by a different name.

Two members proven at the same place under different names are twins. A
twin stands only if one of its own calls lands on what an untwinned member
proves to carry that name, and every other twin is refuted the same way:
`SpuWrite` over `SpuRead`, because one calls `_spu_Fw` and the other
`_spu_Fr`; `GsDrawOt` over `GsDrawOtIO`. Otherwise neither counts, as a name
or as a witness for anything else.

The same two tests place **a member that lies at several addresses**.
libsnd's `_SsSetNrpnVabAttr15` to `19` are five members whose bytes are the
same outside the one field the linker fills, so each lies at each of the
others' addresses and the bytes alone place none of them. Such a member
stands at an address only when one of these holds there, and nothing proven
contradicts it:

- **its own call lands on what it names** (`calls`): `…15` jumps to
  `SsUtSetReverbType`, `…17` to `SsUtSetReverbFeedback`, and the image's
  field at each address holds one target, which is proven;
- **a proven member refers to it by name** (`referred`): `SsSeqOpen` fills
  a table with the addresses of all twenty by name, so the image holds one
  address for `_SsSetNrpnVabAttr18` and another for `…19`, the two that even
  call the same function.

Ten members are placed that way, `SpuInit` among them (its 32 bytes are
also `SsUtReverbOff`'s; it calls `_SpuInit`, and `SsInit` calls it by name).
A member that lies at one address but is too small to count by itself is
held to the same two tests.

A proven member also names what it calls: the address in the image's field
is what the original link resolved the symbol to. That is `sdk-link`. It is
accepted from two or more callers, or from one that is `exact` or `large`.

Three more witnesses were read, and none may disagree:

- The comment on the function's C definition, where it opens with
  `lib<x> <Name>`. It cannot name a function; it can only agree or object.
  It agrees on 390 of the 477.
- `config/vendor_provided.txt`, the old repository's byte comparison against
  release 4.6 and 4.7 objects.
- The stubs under `from-sdk/`. `make CODE=sdk` reads those bytes from a
  named place in one of release 4.5's own `.LIB` files and the image still
  matches.

Against release 4.5, 266 members are proven in place, and they name 475
functions by their bytes. Two more are named only by a call: `main`, which
the start-up object calls, and `printf`, the game's own function that
eleven of Sony's members call by that name. One of the 266 is not in a
library: the start-up object `2mbyte.obj`, which Sony ships beside them.

The scripts that measured this are working files, not part of the
repository. The method above is complete enough to redo.

## A later release's name

A function inside a proven member has Sony's code and, often, no name: the
member does not export it. A later release sometimes does. Such a function
is named `sdk-later` when a later release's library of the same name exports
a symbol whose code is the function's bytes for the function's whole length,
every later release that has one gives the same name, and no other name
fits.

That is what the label says and no more. It is not what release 4.5 called
the function: Sony reorganised libpad in 4.6 and renamed as it went (4.5's
`_padChkVsync` is 4.7's `PadChkVsync`, its `setRC2wait` is `_padSetRC2wait`).
Nor does it mean the program links a second release. The question was put
directly: no member of release 4.4, 4.6 or 4.7 lies anywhere in either
program's library code where a 4.5 member does not, and the 4.6 and 4.7
object that exports these two names (`pdresres.o`) lies nowhere in either
image.

Of the main executable's 120 unnamed functions inside proven members, two
qualify, `_padIsVsync` and `_padIntPad` (`0x800A52FC`, `0x800A5364`), and the
same two on the bonus disc. One more matches four later names at once and
is left alone.

## The developers' own names

The game's assertions hand a message and a source-file name to one routine:
`("MTH_Div() by zero", "MTH_CURV.C", line)`. A function is named `original`
when it makes such a call, its message begins with an identifier followed at
once by `()` or `:`, and no other function's message begins with that
identifier. A looser rule was tried first and thrown out: it would have
named functions after `sim:` (a device path) and `ERR:` (a tag).

Thirteen of the main executable's own functions make an assertion call.
Three name themselves: `MTH_Div`, `printmes` and `MoveChrAction`. The other
ten messages name nothing ("Garbage Error!") or name what was called
("SsVabOpenHead : failed").

Every one of the thirteen says something else for certain: the developers'
source file. Ten are named (`ANKPRINT.C`, `CDREAD.C`, `FACE.C`, `ICON.C`,
`LNS_INTP.C`, `LNS_MOVE.C`, `MTH_CURV.C`, `OVERLAY.C`, `SOUND.C`,
`XMALLOC.C`), and each function's row records its own. The bonus disc and
the overlay make no such calls.

**Debug prints name functions too** (ruled 2026-10-13). The developers'
debug lines go to one routine, `debug_printf`, which does nothing in this
build, and some lines trace the function they are in:
`debug_printf("bg_copy(%d,%d,%d,%d,%d,%d)\n", dx, dy, sx, sy, w, h)`. A
function is named `original` from its own debug line when all of this
holds:

- after the marks the developers start many lines with (a star, in
  Shift-JIS), the line begins with an identifier followed at once by `(`;
- the values inside the parentheses are the function's own arguments, in
  the order of its parameters, or the parentheses are empty, as in the
  assertions;
- no other function's line begins with that identifier.

The second point is read in the C, and each row says what was read: which
parameters are printed and which are not. Six functions are named this way:

| Name | Address | Its line | What it passes |
|---|---|---|---|
| `bg_copy` | `0x80015224` | `bg_copy(%d,%d,%d,%d,%d,%d)` | its six parameters, in order |
| `BtlEndMessageDisp` | `0x8004BA18` | `BtlEndMessageDisp(%d)` | its one parameter |
| `ColMix` | `0x80059E04` | `ColMix(no=%d,sw=%d)` | its two parameters, in order |
| `MapEffectSet` | `0x80068AC0` | `MapEffectSet(tbl=%x,sw=%d)` | the first and third of three |
| `OverCheck` | `0x800695A8` | `OverCheck(%d,%d) %d, …` | its first two inside the parentheses, the rest after |
| `cd_load` | `0x800211D8` | `cd_load()：` and "file %s not found" | its first parameter, when the file is not found |

Three lines hold a name and name nothing here, because the values they
print are not the function's arguments: `MessageConv` (`0x8005AC44`),
`PSchk` (`0x80024CE0`) and `MemoryBreaker` (`0x80022EE4`). The line may give
the function's own name, the name of something it calls, or a description;
the image does not say which. Each row records its line. `xa_play:` is
printed by three functions and names none of them.

## Names given by reading

A `coined` name is a reading, not a proof, and its row has to let the next
reader check it: which file was read, what the function was seen to do, and
who else reads it the same way (the specification, Eternal Silver's
transcription). Eternal Silver's own name is evidence, not the name: it is
TypeScript's, for the engine's shape of the code (`Ran55.next`).

Six units were read this way one at a time, and the rest in waves of
several readers (below). Each unit and wave is in
[naming-log.md](naming-log.md).

## Readings made by several readers at once

From 2026-10-13 groups of functions are read five at a time, each group by
its own reader (a separate AI session), and applied by one session. A
reader changes nothing in the tree. It writes a proposal: a name and what
was read for each function it can read, a row saying why for each it
cannot name. Before a proposal is applied:

- a script checks every row against the image and the tree: that the
  function exists and is still unnamed, the spelling, that the name is free
  (in the tree, in Sony's headers, in the other proposals), the number of
  jumps that reach the function and which functions they come from, that
  every function the text names exists, and that text quoted from the
  program is in the image;
- the applying session reads a sample of each group's functions in the C
  and compares them with the rows;
- the readers' words for the same thing are made one, and a name whose row
  rests on something its reader did not read is changed or withheld.

[naming-log.md](naming-log.md) has every wave: the groups, the names, and
what the applying session changed before applying.

**The functions first left unnamed.** After the last wave 105 of the
game's functions were read and not named. The owner ruled on them on
2026-10-14, by kind, and two readers went over all 105 again:

| Kind | Ruling | Named | Left |
|---|---|---:|---:|
| Called, and empty (or one store nothing reads, or a constant) | named from what the callers pass and do round the call, as `assert_fail` and `debug_printf` were, where the call sites make it clear | 7 | |
| Does real work, and nothing reaches it | named for what it does | 14 | |
| Does what a named function does, and nothing says why there are two | the twin's name and `Copy` (`rng_draw_copy`, `FaceSlotResetCopy`) | 18 | |
| Close to a named function but not the same (an argument cut to a byte, the arguments one place along) | named for what it does (`MusicBankLoad`, `BtlTileScrollSet`, `BtlTileScrollMove`) | 3 | |
| Its purpose became readable meanwhile | named as its neighbours are (`resource_map_enter_table_set`) | 1 | |
| Everything else: empty and reached by nothing, or what it is for is said nowhere | left | | 62 |

The called empties are named `...Report` or `...Update` where every caller
hands them what it has just done: `ScriptStateReport` gets the script
reader's state before each command and when it stops on an error,
`BtlCellRowsUpdate` gets the rows of the text grid a caller has just
changed. Several of these look like the hooks of the developers' own
tools, emptied for the release.

Each of the 105 carries the decision twice: its row in the register ends
with the ruling it was named or left under, and a comment above it in the C
says the same in a line, beginning "Ruling of 2026-10-14".

## Conventions settled while naming

The rules above were applied group by group; these were settled on the way,
and every name since follows them. [naming-log.md](naming-log.md) has where
each came from.

- **Spelling by layer**, as the developers spelled their own names (see
  "Spelling" above). Sound is spelled both ways on purpose: `sound_`,
  `xa_` and `cd_track_` drive Sony's library and the drive; `Music`,
  `Voice` and `SoundPlay` are the game's side, which takes the numbers the
  script, the map and the battle hold.
- **`Disp`** ends the name of a function that writes text on the screen,
  the developers' word from `BtlEndMessageDisp`. `Draw` is for pictures.
- **The thing first, the action last**: `BtlRun`, not `BtlMain`.
- **`Chr`** begins a function that works on the characters' own records
  and is not the battle's alone (`ChrLevelUp`).
- **A table that the C tells apart only by number is named by number**,
  and each entry's row says what it does: the battle's shows
  (`BtlShow01Run` to `BtlShowBCRun`), the enemies of the battles numbered
  `0xEB` and up (`BtlEnemy6FActionSet`), four script commands
  (`ScriptCommand36Skip`). One function that is several entries is named
  by the lowest, and its row lists the others.
- **Numbers in names.** A number that picks an entry of a table, a byte
  value or a show's number, is written with two hexadecimal digits:
  `BtlShow04Run`, `BtlShow0BRun`, `MapObjectPartyLook0CStep`, as
  `BtlShow97Run`, `BtlEnemy6FActionSet` and `ScriptCommand36Skip` already
  were. One digit would make `BtlShowBRun`, which reads as a word. A bit or
  an offset keeps its own digits: `BtlTargetStatus4Set`, `Stat0E`.
- **A script command** is `ScriptCommand` and then the name of the
  function it calls (`ScriptCommandMapChange` calls `MapChange`).
- **A status is named by its bit and a stat by its place**
  (`BtlTargetStatus4Set`, `BtlStat0EHitAmountSet`): the C does not say
  what each is.
- **Words from the developers' text** that names no function by the rule
  may give a coined name its word (`MapPartyTalkCheck` after their line
  "PartyTalkCheck1"); the row says where the word comes from.
## Global data

Global data is named on the same register, with the same evidence, from
2026-10-14. The owner ruled its spelling that day: **`g_` and then
CapitalisedWords**, in every layer of the program (`g_BtlActingSlot`,
`g_BtlSlotIds`, `g_XmallocTop`). The group comes first, with the first word
of the functions that use the global, and the name says what the global
holds: plural for an array or a table, singular for one value. A row is
`type:data`, and its evidence names the functions that read and write it.

Sony's own variables keep Sony's names, as its functions do (`_max_heap`,
`_TopAllocArea`, `_stacksize`). A value the developers' debug text prints
with a label takes that label, capitalised after `g_`, and its row says
`src:original` (the owner's ruling of 2026-10-14): `g_MapMix` is printed
as `mapmix=%d` by GameEnter, `g_MapEffect` as `mapeffect=0x%x` by the
developers' MapEffectSet, and VoiceStep prints `voice=0` right after it
stores 0 in `g_Voice`. A label that is a bare word takes the group first,
as the `g_` rule asks: the heap's line prints `max=` and `last=`, and they
are `g_XmallocMax` and `g_XmallocLast` (the owner's ruling of the same day).

The ruling on twin functions holds for data (the owner, 2026-10-14): a
global that holds the same bytes as a named one, where nothing read says
why there are two, takes its name with `Copy` (`g_MoveChrMoveToListCopy`).

A variable of published source the game carries takes its published
name, capitalised after `g_`, and its row says `src:public-source` (the
owner's ruling of 2026-10-14): gzip's table `mask_bits` is `g_MaskBits`,
and the work arrays `h` and `d` of Okumura's `maketable` are `g_MthH` and
`g_MthD`, with `Mth` for the `MTH_` its functions carry.

## Parameters and locals

Inside the functions the names are the decompiler's (`arg0`, `temp_v0`,
`var_s1`, `sp10`) until they are read. The owner ruled on 2026-10-14:

- **lower_case**, words joined by `_` (`slot`, `item_count`); a plain
  counter may be `i`, `j` or `k`. A name says what the value holds, in the
  words the program's names already use.
- **The C is the record.** A parameter's or a local's name is read against
  its own function, which is a few lines away; it gets no row in the
  register. Every rename is proven by the byte-identical build, and the
  naming log records each wave.
- **A value a developers' debug print labels takes the label**
  (`ColMix(u8 no, s32 sw)`, `MapLoad(void *tbl, s32 sw)`,
  `MessageNoticeDisp`'s `no1`), as globals and functions do. Okumura's
  published routines keep the names their readers gave them (`knots`,
  `x_table`), not the one-letter names of the published code.
- **Sony's functions take Sony's parameter names** where Sony's own
  prototypes give them (`SetVideoMode(long mode)`, `_card_clear(long
  chan)`); only the names are taken. Most of Sony's prototypes give types
  only, and those parameters keep their decompiler's names.

## Structs and their fields

The owner ruled on 2026-10-14:

- **Types are CapitalisedWords, group first** (`BtlRecord`, `MapObject`,
  `SpriteRecord`), as the game's functions and globals are; **fields are
  lower_case** (`x`, `attr`, `step_count`), as locals are.
- **Each struct is defined once**, in a header by subsystem under
  `decomp/<serial>/include/` (`btl.h`, `map.h`, `menu.h`, `sprite.h`...);
  the per-file copies the first split made are removed and the files
  include the header.
- **One true layout per piece of memory.** Where the match needs another
  reading of a field (a load unsigned where the field is signed, a
  narrower width), the access casts (`*(u16 *)&record->y0`) with a short
  note saying the match needs it. Where a field really holds different
  things at different times, the struct has a union with a member for each.
  A whole-struct view (a record read as sixteen words) is kept only where
  the build proves the true layout cannot reproduce the code, and it is
  named for what the code does with it.
- **A comment on each field is its record**: its offset, what it holds and
  who writes it. Each header also states its struct's size and every
  field's offset with `LAYOUT_SIZE` and `LAYOUT_AT` (`include/layout.h`), so
  a wrong layout stops the build, and a port that includes the header
  cannot drift from the game's memory.

Sony's library code keeps Sony's types: each of its structs is defined
once, in `include/psyq_private.h`, under Sony's name where Sony's own
headers or symbols give one (the name only, no text of the headers), and
otherwise under the name the tree already gives it (the owner's ruling of
2026-10-14). The per-file copies the first split made are removed. A type
Sony's public headers declare (`POLY_FT4`, `GsOT`, `SpuReverbAttr`,
`DECDCTENV`) is never defined in the tree at all: the C has it from the
headers `psyq_types.h` includes, and a file that needs a header beyond those
includes it by name (the overlay's `libpress_libpress.c`). The overlay shares the
main program's `psyq_private.h` (a link, as `psyq_types.h` is).

The cast an odd access takes is written `AS(type, field)`
(`AS(u16, record->sprite.depth)`, `AS(u8 *, record->sprite.pose_table)`):
it changes the access's type and keeps it a member access. A plain cast
(`*(u16 *)&record->sprite.depth`) is a pointer access to GCC 2.8, which
schedules it differently against the stores round it, and the pilot found
functions that then no longer match.

**Raw offsets.** The first split reached most memory by offset
(`*(s16 *)((u8 *)p + 0x56)`, `((u8 *)cmd)[3]`). From 2026-10-17 those
accesses were written as members of their true types, and on 2026-10-19
the last 254 outside the script commands followed (three readers by subsystem, then the sites that waited on the
types they added). They brought eleven new types, among them
`SpriteDataHead`, `MapBgChange`, `MenuScreenBits` and `FileHostBlock`, and
two new headers, `face.h` and `font.h`. Where the plain member and `AS()`
both change the code, the access is the plain cast on the member
(`*(u16 *)&e->record.sprite.depth`), which GCC 2.8 schedules as it did the
raw offset, with a note saying so: no offset is left raw for that reason.
The 79 raw offsets left are the event script's operand bytes: a command is
a byte stream its handler reads at offsets from the command pointer, and
by the owner's ruling of 2026-10-17 ("all but commands") they stay as
they are.

The memory card save is `CardSave` (`card.h`, 2026-10-19), 0x2000 bytes:
the card file's head, the game's 0xC00-byte block of records at +0x200,
the map objects' flags at +0xE00 and their packed entries at +0xF80, and
the sum in the last word. The head is the one every memory-card file
begins with, which the C only writes and the console reads: by the owner's
ruling of 2026-10-19 its parts take the card's file format's words
(`magic`, `icon_frame_count`, `block_count`, `reserved`, `icon_palette`,
`icon_frames`), and the title is the C's own word (`card_save_title_set`).

## Comments in the C

The comments were written while the C was being matched, in the older
repository. By the owner's ruling of 2026-10-14 the log of that work was
taken out of them (match scores of the function itself, dates, batch
numbers, the old repository's commit ids), and what a reader needs was
kept: what a function does, and why the C is written the way it is (the
forcing devices, the forms that were tried and what they gave).

## A published source file's names

The game unpacks most of its data with code that is not the developers'
own. Ruled 2026-10-13: such a function gets the published name if it is
that function, and not otherwise.

"Is that function" means all of this:

- the program holds the source file's constant tables, value for value;
- the function takes the published function's parameters, in their order;
- its body is the published body, statement by statement in the same
  order. What the program's copy leaves out or replaces is listed in the
  row.

The file is Mark Adler's `inflate.c`, which says of itself that it is not
copyrighted, as gzip 1.2.4 ships it (`ftp.gnu.org`, `gzip-1.2.4.tar.gz`;
`inflate.c` sha256 `d63333e2ec1614af06877bad89695ed192f0e75b868a9f8f9a8ccc579532ee18`).
It was fetched to be read beside the C and is not in the repository. All
six of its tables (`border`, `cplens`, `cplext`, `cpdist`, `cpdext`,
`mask_bits`) lie in the image at `0x800AF4FC` to `0x800AF608`, in the
file's order, and its two tuning numbers (9 and 6) are at `0x800B8700`.

Five functions do the unpacking. Two are the published ones:

| Name | Address | What the program's copy changes |
|---|---|---|
| `huft_build` | `0x800222C4` | two error returns gone; tables come from a pool, not `malloc` |
| `inflate_codes` | `0x80022840` | input 16 bits at a time; two error returns gone; the window is the output itself |

Three are not, and have `coined` names that say what they are:

| Name | Address | Why it is not the published function |
|---|---|---|
| `huft_pool_reset` | `0x8002282C` | called where the file calls `huft_free`, but it only puts the pool pointer back; nothing of `huft_free`'s body is here |
| `inflate_block_dynamic` | `0x80021DD8` | `inflate_block`'s first statements and then `inflate_dynamic`'s body in one function. The block type is read and not looked at, so every block is decoded as a dynamic one; it takes no argument and returns the last-block flag |
| `inflate_buffer` | `0x80021D6C` | stands where `inflate()` stands, with other arguments (a source and a destination), another result (the bytes written) and no flush |

A sixth function beside them, `lz_unpack` (`0x80022BF8`), is a second,
smaller unpacker (flag bits, then a literal byte or a match of one or two
bytes) with nothing of `inflate.c` in it. It is `coined`.

### Haruhiko Okumura's routines

The developers' math file is `MTH_CURV.C` (the name is in `MTH_Div`'s
assertion). Five of its functions are not the developers' own. They are
routines of Haruhiko Okumura's dictionary of algorithms in C (奥村晴彦
『C言語による最新アルゴリズム事典』, 技術評論社, 1991; revised in 2018 as
『C言語による標準アルゴリズム事典』), with the arithmetic changed from
`double` to 16.16 fixed point.

How it was found: on 2026-10-13 the reader of the math group recognised
the four spline functions from memory and said so, with the marks it
remembered. The files were then fetched and read beside the C by the
applying session. What follows is that comparison, not the recollection.

The files compared are the author's own, from the support repository of
the 2018 edition (`github.com/okumuralab/algo-c`, commit
`feef3a0a18e058fb3a780c113d475014cbe89978`), whose README says the author
claims no rights over the source code. They were fetched to be read and
are not in the repository.

| File | sha256 |
|---|---|
| `src/spline.c` | `beec458c951d62b959d635feec21a017d9d01ead60e1140e84dde29d7dcb1d5e` |
| `src/spline2.c` | `2c49ffe9505a7d8e1d13562af36ed472623d7c2e17f09a41660f9cf22d9a23b2` |
| `src/sqrt.c` | `1d140747b55bfafbc1845aefa3cd1fe4dc6e7088a93422538b2592759e351eec` |

| Address | Name here | Published function | What the program's copy changes |
|---|---|---|---|
| `0x8001C9BC` | `MTH_maketable` | `maketable(x, y, z)`, `spline.c` | the constant `N` is a fourth parameter; the static work arrays `h` and `d` are the arrays at `g_MthH` and `g_MthD` |
| `0x8001CC10` | `MTH_spline` | `spline(t, x, y, z)`, `spline.c` | `N` is a fifth parameter |
| `0x8001CD50` | `MTH_maketable2` | `maketable2(p, x, y, a, b)`, `spline2.c` | `N` is a sixth parameter; `sqrt(t1 * t1 + t2 * t2)` is `MTH_Hypot(t1, t2)` |
| `0x8001CE7C` | `MTH_spline2` | `spline2(t, px, py, p, x, y, a, b)`, `spline2.c` | `N` is a ninth parameter |
| `0x8001C7B0` | `MTH_mysqrt` | `mysqrt(x)`, `sqrt.c` | for a negative argument it returns -1.0, where the file prints an error and returns 0 |

In all five every multiplication and division of two `double`s is a call
of `MTH_Mul` or `MTH_Div`. Otherwise each function has the published
function's statements in the published order. Two of `maketable`'s lines
are ones a second writer would not arrive at, and both are in the program:
`z[1] = d[2] - d[1] - h[0] * z[0]` and `z[N - 2] -= h[N - 2] * z[N - 1]`
each multiply by a value the function has set to 0 a few lines before.

What is not known:

- **The 1991 edition's text.** It is the edition the developers can have
  had, and it was not seen. The comparison is with the 2018 edition's
  files. `mysqrt` in particular is the 2018 file's name for the function.
- **What the developers called them.** Their own function in this file is
  `MTH_Div`, so they may have renamed the book's routines to match.

The rest of the group is the developers' own, built on these:
`MTH_CurvePointsMake` (`0x8001CFB0`) checks its counts, sets the work
arrays and calls the two `spline2.c` routines; `MTH_Hypot` stands in for
the book's `sqrt` of a sum of squares; `MTH_Mul`, `MTH_Div` and the two
conversions are the fixed-point arithmetic.

The file has no constant tables, so the first point of the test above has
nothing to check here.

**The names** (ruled 2026-10-13): the developers' prefix for this file,
`MTH_`, and then the author's name as he spells it: `MTH_maketable`,
`MTH_spline`, `MTH_maketable2`, `MTH_spline2`, `MTH_mysqrt`. The prefix
says whose file the function is in; the rest says whose routine it is. Each
carries a comment in the C saying what it is, and its row is
`public-source`.

## Found while naming

**What looks like remains of another machine's version.** One reader
noticed three things in the battle's menu code that have no use on this
console, and said what it remembered about them without having checked
it. The three, as the C has them:

- `0x80049AC4` stores `0x25E74000` in a block as an address. The console
  has no memory there. The one function that would use the block
  (`0x80054B30`, now `mem_request_run`) is reached by nothing.
- `0x80048C9C` is an empty function with 27 calls. Each call passes a row
  of the text grid and that row times `0x40`, though the grid this program
  draws has `0x28` cells to a row.
- `0x80049EAC`, which nothing reaches, writes a 3 by 3 square into a grid
  of 32-bit words with 64 words to a row. No grid of that shape is in the
  program.

The reader's recollection, and the applying session's too: the game was
first released for the Sega Saturn, and on that machine `0x25E00000` to
`0x25E7FFFF` is the memory of its second video chip, where a map 64 cells
wide of two-word cells is a usual layout. If that is right these are
pieces of that version's screen code that were emptied or left behind when
the game was moved to this console. It has not been checked against
Sega's documents or against the Saturn game, and nothing is named on it:
the three have rows saying what they do.

## Where things stand

Of the main executable's 2,320 function symbols, and one routine the build
labels as data (`__main`):

| | Functions |
|---|---:|
| Sony's name, `sdk-bytes` | 475 |
| Sony's name, `sdk-link` | 2 |
| A later release's name, `sdk-later` | 2 |
| An earlier release's name, `sdk-earlier` | 11 |
| Sony's own text, `sdk-text` | 3 |
| The developers' own name, `original` | 9 |
| A published source file's name, `public-source` | 7 |
| Given by reading, `coined` | 1,645 |
| Named for their address | 167 |

All three programs build and match, and so does `make CODE=sdk`.

**The start-up code is Sony's too.** `2mbyte.obj` lies whole at
`0x80013C44`. It names its own two routines, `__main` and
`__SN_ENTRY_POINT` (the tree's `game_entry` until then), and it calls
`0x80013D2C` by the name `main`. That name has a check of its own: the
compiler puts a call to `__main` at the top of any function called `main`,
and the image has exactly that call there. The C had it written out by hand
while the function was `func_80013D2C`; as `main` it is not written, and the
build still matches.

**Release 4.7 calls three of them something else.** `_padChkVsync`,
`_padStartCom` and `_padStopCom` (`0x800A54F4`, `0x800A5508`, `0x800A55D4`)
are `PadChkVsync`, `PadStartCom` and `PadStopCom` in 4.7, where in this
program those names belong to the wrappers at `0x800A4CF8`, `0x800A4D18` and
`0x800A4D38`. Their rows say so. No other function is named differently by
the two releases.

**Four inherited names were wrong**, and the tree's own comments had said so
without anything being renamed:

| Address | Was | Is |
|---|---|---|
| `0x80090740` | `CdControlB` | `CdControlF` |
| `0x80090874` | `CdControlF` | `CdControlB` |
| `0x800909E0` | `CdReadSync` | `CdGetSector` |
| `0x80090A00` | `CdReadCallback` | `CdGetSector2` |

**These four old names cannot be resolved from the register alone**, because
each is another function's name today (`CdReadSync` is `0x80093794`,
`CdReadCallback` is `0x80093898`). A page written before 2026-10-13 that
uses one of them without an address means the *old* symbol;
`subsystems/psyq/libcd.md` discusses them at length.

Two functions the C called "not identified" are `MemCardInit` and
`MemCardEnd` (`0x800A1068`, `0x800A1098`).

### Sony's prototypes

Every C file includes Sony's headers, so once a function carries Sony's name
its prototype is in scope everywhere.

- 159 local declarations the headers made redundant were removed.
- 35 definitions were given Sony's signature (`long` for `s32`, `char` for
  `s8`, `int` for a pointer the header declares as `int`).
- `_card_open` and `MemCardInit` now take the argument they pass on to
  `InitCARD`, and `func_80023A20` and `func_8001F294` became `void`: each
  used the value of a function Sony declares `void`.

In some files the image shows a call that Sony's prototype cannot produce,
so the original was compiled with no such prototype in scope. There the
header's declaration is renamed out of the way with a `#define` before the
include, and the function is declared as it is called. The first split had
the first two in `battle_b.c` and `rendering.c`; since the game's files were
cut (2026-10-18) the files that call them keep it:

| File | Function | What the image shows |
|---|---|---|
| `menu_list.c` | `SpuSetCommonCDVolume(short, short)` | Two ints passed with no sign extension |
| `btl_draw.c`, `sprite_draw.c`, `sprite_list.c` | `GetClut`, returning `u_short` | The result used as a word, unmasked |
| `libapi_counter_1.c` | `SetRCnt(.., unsigned short, ..)` | `-1` passed as a whole word, in Sony's own libgs |

### The bonus disc

`SLUS_009.21` links the same release, so the same proof names it. Of its 404
functions, 268 carry Sony's name by their bytes, `main` by the start-up
object's call, two a later release's name, and 133 are named for their
address. It had no inherited
names. Getting there took the same work on Sony's prototypes: 113 redundant
declarations removed, 33 definitions given Sony's signature, and `main`'s
written-out call to `__main` removed.

The proof also corrected the tree's record of how much of that program is
Sony's. `config/vendor_provided.txt` listed 90 routines, from a comparison
against releases 4.6 and 4.7; against 4.5, 258 of the 265 routines the tree
still reads as byte ranges lie inside a proven member of Sony's. Seven
functions are left to decompile, not 144.

Its own code under the game was named on 2026-10-18 by reading it, in the
main program's words: `pad_start` and `pad_stop`, `screen_init`,
`sound_init` and the voice and noise routines (`sound_voice_play`,
`sound_noise_step`), `tim_image_load` and `tim_image_load_at`,
`font_hex_print`: 15 functions. The other 20 followed the owner's rulings
of 2026-10-18 (below): its movie player (`movie_run`, `movie_play` and the
`movie_` routines under it), `pad_buffer_decode`, and the hidden minigame
under the word `Castle`, the options' own word (`Castle Width:`):
`CastleTitleRun`, `CastleDemoSet`, `CastleOptionsRun`, `CastleGameRun` and
five helpers. Every function of the disc's own is named now; 83 of Sony's,
statics inside proven members, are named for their address (98 until the
ruling of 2026-10-19 below).

**Ruled 2026-10-18**, for the overlay and the bonus disc:

- The two movie players are named by reading, in lower case under
  `movie_`. The C's comments liken them to Sony's SDK movie samples
  (`strNext`, `strSync`, `anim()`); no sample was compared, and none will
  be fetched to compare, so the rows say so and the names describe the code.
- The minigame's two giants, `CastleGameRun` (34,928 bytes) and
  `CastleOptionsRun` (20,988), are still byte ranges and were not read
  through. They are named by their place in `main`'s loop, and their rows
  say what was measured and what was not.
- Sony's static functions that print their own name (`MDEC_rest:bad
  option`, `MDEC_in_sync`, `MDEC_out_sync` in the overlay's libpress) keep
  their addresses: all of Sony's statics are decided under one rule. That
  rule was given on 2026-10-19 (next section).

## Known, and not yet given

Nothing is held. The six functions that were (`SpuInit` and
`_SsSetNrpnVabAttr15` to `19`) are proven by the tests for members that lie
at several addresses.

### Sony's statics (ruled 2026-10-19)

223 functions inside proven members of Sony's were named for their address
(118 in the main program, 98 on the bonus disc, 7 in the overlay): parts
the library's symbol table gives no name, most of them `static`. Their C
comments gave 75 of the main program's names anyway, from an older release,
from sotn-decomp or psyz, or with no source at all. A survey put the
evidence for each together, and the owner ruled that a static is named only
by what is explicit:

- **Sony's own text** (`sdk-text`): a string the function prints begins
  with its name, or a message of the member names it as the function it
  calls. `CD_newmedia` and `CD_cachefile` print their own names, and
  `CD_newmedia`'s message names `cd_read`; the libpress statics print
  `MDEC_rest`, `MDEC_in_sync` and `MDEC_out_sync`. `MDEC_rest` is spelled
  as printed; the comment had read it `MDEC_reset`. Twelve functions, the
  disc's copies included.
- **An earlier release's bytes** (`sdk-earlier`, the mirror of
  `sdk-later`): a release before the linked one exports the function's
  identical bytes, its whole length outside the fields relocations fill,
  under one name and no other. Compared against every library of releases
  2.6 to 4.4 in psx_psyq_signatures (commit `e9e46e7e`, the copy the libpress
  proof used): eleven names for twenty functions (`_ExpAndAlloc`,
  `_make_file_discrpt`, `RemovePatchPad`, `_Pad1`, `_IsVSync`, `def_cbsync`,
  `_cmp`, `setIntrVSync`, `_param`, `_addque`, `set_alarm`). Of the sixteen
  comments that said a name came from an older release, only
  `_ExpAndAlloc`'s bytes match; the other fifteen names stand in that
  member over other code, so the release's function of that name is not
  this one.
- **Everything else keeps its address**, with a row saying what it does,
  the name its comment gives and where the comment says the name comes
  from, and that no Sony text names it and no release from 2.6 to 4.7
  exports its bytes under one name. Held, with that said in the row:
  libcd `event.o`'s two functions that releases 3.7 and 4.0 export as
  `def_cbready` and `def_cbread`, the same code under both names; libgpu's
  that releases 2.6 to 4.0 export as `_status`, and libpress's as
  `MDEC_status`, each the other's bytes with only four of six words free of
  relocations; and libcd `cdread2.o`'s 32-byte bare call, whose bytes six
  libcd names and 23 others fit.

That leaves 106 of Sony's functions named for their address in the main
program, 83 on the bonus disc and 4 in the overlay, each with a row. Six
of the main program's are not functions: two eight-byte version stamps and
four pieces of patch code nothing calls.

## What is not checked

Nothing in this repository checks the register. A row can go stale with no
build failing: the build reads only `name = address`.

The check belongs to the tools repository and is deferred to it. It has to
say, after a build:

- every function is named for its own address, or has a row at that address
  under that name;
- no name stands at two addresses, and no row disagrees with the build;
- every name that is not an address has a source, and every source other
  than `inherited` has a date and evidence;
- every such function has its address tag on the line above its definition,
  and no tag stands anywhere else;
- every `sdk-bytes` and `sdk-link` row still holds when the comparison is
  run again, against the release the program links;
- the record lists the build does not read (`asm_only.txt`,
  `vendor_provided.txt`, `bios_thunks.txt`, `object_pins.txt`) name only
  symbols that exist.

Four rows that failed the second of those before any naming were removed on
2026-10-13: `gpu_primitive_builder` and `gpu_texquad_format` named places
inside `map_display` that nothing refers to, and `main_bootstrap` and
`static_init` are in the disc's outer loader, not in this image
(`spec/code/subsystems/bootstrap.md` has them).

## Open

- **`__main` is still data to the tree.** Its eight bytes (`0x80013C44`)
  sit at the end of a data stub, so they are read from the disc with the
  data even under `make CODE=sdk`, and the function counts in `README.md`
  leave it out: there are 66 routines that were never C, not 65. Making it
  a routine means moving the start of the program's code by eight bytes in
  the linker script.
- **`_stacksize`.** The start-up object refers to the word at `0x800B8714`
  by that name. Data has not been named yet; it is `D_800B8714`.
- **The proof cannot be rerun from the repository alone.** The recipe
  fetches the five files the build needs. Proving names needs every library
  of the release and a reader of their symbol tables, which is the tools
  repository's work.
- **Source-file names.** The assertions name ten of the developers' files.
  Since 2026-10-18 the file holding each one's group of functions has its
  name in lower case (`ankprint.c`, `cdread.c`, `face.c`, `icon.c`,
  `lns_intp.c`, `lns_move.c`, `mth_curv.c`, `overlay.c`, `sound.c`,
  `xmalloc.c`); its header says which function's assertion names it, and
  the file's other functions have it by being of its group.
- **Game code.** Six units read one at a time (the generator, the
  allocator, the developers' checks and prints, file reading, the script
  actors' movement commands, the two unpackers) and eleven waves of
  readers. Every function of the game's own code has been read. 61 are
  not named, by the owner's ruling on the functions first left unnamed
  (above), each with a row saying what it does and why it has no name; a
  reader went over all 62 again on 2026-10-18 and named one,
  `resource_command01_set`.
  The 106 other functions named for their address are Sony's private
  parts of proven library members (the statics, under "Known, and not
  yet given").
- **Okumura's routines**: assumed to be from the book's 1991 edition, the
  one the developers could have had; checked against and named after its
  2018 edition, whose C files are public. Compare them if a copy of the 1991
  edition turns up: `MTH_mysqrt`'s name in particular is the 2018 file's.
- **Data**: four waves named 874 globals; the game's own C calls 97% of
  its uses of a global by name (2026-10-18). Thirteen of those names were
  places inside a record with a name of its own (`g_BtlWalkBlockTarget`, +4
  of `g_BtlWalkBlock`). On 2026-10-19 twelve of them, and two such places
  still named for their address (`D_800B5B88`, `D_800A8552`), became members
  of their records (`g_BtlWalkBlock.target`, `g_BtlShowSounds[4].state`,
  `g_ShopStockRows[0x27]`): the labels and their rows are gone, the rows'
  facts are in the fields' comments, and every object is unchanged.
  `g_BtlWalkBlockReach` stays a label: read as the member, the compiler
  moves a load into a branch's delay slot and the bytes change. The 175
  globals the C still uses by address have fact rows saying what is known
  and why no name was given: entries of one table told apart only by
  number, words only written, and Sony's library variables. The two names
  held until 2026-10-19, the save icon's palettes and frames, were given
  then (`g_CardSaveIconPalettes`, `g_CardSaveIconFrames`): the owner ruled
  that the memory card file's head takes the card's file format's words.
- **Files.** Both kinds are re-cut (the owner's rulings of 2026-10-18).
  Sony's: each proven member in a file of its own,
  `lib<library>_<member>.c`, in numbered parts where the tree compiles a
  member's functions with different per-file flags. The game's: cut where
  the subsystem changes, each piece a run of one old file's functions in
  their order, named for the developers' source where an assertion names it
  and else for the subsystem, the group's own first word first (`btl_`,
  `map_`, `menu_`, `shop_`, `chr_`; `btl_show.c` and `btl_item.c` where a
  group spans several). The main program's 38 game files are 142, the bonus
  disc's two are seven. Each new file's header says what it holds and the
  first split's file its code came from, so a file named in an older text
  can still be found. In the C, the headers and the register, every note
  that cited a first-split file was read again on 2026-10-19: it names
  today's files (counts by file made again over them), or, where it speaks
  of the first split's file as such (its declarations, its translation
  unit, what a tool did to it), says so (`the first split's battle_b.c`).
  The specification was brought up to date the same way (`docs/README.md`).
  Each file carries only what its own code uses: the cuts had copied every declaration, `far_syms` entry and marker of
  the old file into each piece, and 37,276 declarations, 510 `far_syms.txt`
  entries and 3,092 markers that named nothing a piece uses were removed,
  every object unchanged. Since 2026-10-19 no file keeps a first-split
  name. The last five that did were `psyq_*.c` files: three empty ones
  (`psyq_cd_y.c`, `psyq_gpu.c`, `psyq_misc.c`), which went, and two in
  which the re-cut had left five of libsnd's members together,
  `psyq_math_b.c` (`de_17.o`, `de_18.o`, `de_19.o`) and `psyq_misc_p.c`
  (`ut_roff.o`, `ut_ron.o`); each of those members is now a file of its
  own. The same day the headers
  of 55 member files whose own names end in digits (`de_16.o`, `gs_007.o`,
  `cc_10.o`) stopped calling them "part 16 of 17" of a member in several
  files: the re-cut had read the digits as a part number.
- **Sony's statics** were decided on 2026-10-19 ("Known, and not yet
  given"): 32 named by Sony's text or an earlier release's bytes; 106 in the
  main program, 83 on the bonus disc and 4 in the overlay keep their
  addresses, each with a row.
- **The bonus disc's Sony routines still come from the disc.** It has no
  `from-sdk/` folder; the main executable's stubs show how.
- **Text that names functions**: on 2026-10-19 every page under
  `docs/spec/` was read again against the C and the register. They cite
  functions and globals by today's names and code by today's files, and
  keep no dated report; fenced code may still give an address where a name
  exists.
