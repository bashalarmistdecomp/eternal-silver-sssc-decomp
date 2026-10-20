# PSY-Q library code: how it was compiled

This page says how the Sony PSY-Q library objects linked into `SLUS_006.28`
(and the movie overlay's libpress) were compiled, which of this repository's
recipes reproduces them, and which shapes of C decide their bytes. It gives
the rules and the compiler mechanisms behind them, each shown on named
functions; a function's own measurements are in the comment above it in the
C. The pipeline and the recipe names are in
[BUILD.md](../../../../../BUILD.md) ("How a file is built"); each recipe's
definition and the measurement that justified it are in `recipe/compile.py`
and `recipe/delayfill.py`. A score such as 38/59 counts matching instruction
rows out of the function's length; +16 is rows gained.

Where a mechanism was read in GCC's source or in cc1's `-da` dumps, the page
says so; the rest was measured by compiling variants and comparing them with
the target.

## The compilers

| Code | Compiler | Recipe |
|---|---|---|
| The game's own code; every file by default | GCC 2.8.1, `-O2 -G8`, assembled as Sony's ASPSX did (maspsx) | none |
| The PSY-Q 4.5 libraries of the main executable | GCC's development tree of 1997-03-18, with `-mhard-float -fno-exceptions`, its own delayed branches and GNU as's conventions | `cc97_lib`, per function |
| libpress 4.1 in `MENU.BIN` | GCC 2.7.2 with its own delayed branches and `-fno-strength-reduce`, then the assembler's fill; the whole overlay `-G0` | `cc272_lib`, per function |

The main executable links release 4.5 ([docs/naming.md](../../../../naming.md)
has the measurement); several of its members are word for word the 4.4
objects. `recipe/gcc-970318/build.sh` dates the snapshot: the same C of
`_card_create2` (`0x800A45B8`, libmcrd LOW.OBJ) fails under the tree one
commit before 1997-02-05's "Don't always align reload stack slots", matches
from it to 1997-03-18, and fails one commit after. That compiler has 4-byte
reload slots, homes a spilled register parameter in the callee's own frame,
and carries a 1996 loop.c that no 2.7.2 release has.

Its flags: soft-float marks the 32 floating-point registers fixed, which
halves loop.c's hoisting threshold (see Loops), so `-mhard-float` matters
with no floating point in sight (`_card_create2` matches only with it). The
tree of that month writes an exception table for every function with a call,
hence `-fno-exceptions`. `-fno-strength-reduce`, part of the 2.7.2 library
recipes, is not a 4.5 library flag: a loop that walks a pointer beside its
counter rules it out (`_SsSndStop` (`0x800991E8`) walks one that the flag
turns back into `vol[i]`).

All three compilers treat plain `char` as unsigned (probed: `*p` on a
`char *` is `lbu`). The library's byte data is signed: `strlen`
(`0x8008B848`) reads with `lb` (12/13 on `u8 *`), and a fill of 0xFF stores
`li -1`. So the C writes `s8`.

Of the 529 library functions of the main executable that are C, 516 carry
`cc97_lib`. `SsVabClose` (`0x8009D638`) and `func_8009D71C` (VS_VH2.OBJ's
allocator) carry `cc272_lib` and give the same bytes under `cc97_lib`.
Eleven small ones carry no compiler marker and are built by 2.8.1:
`StSetRing` (`0x80090078`), `CdLastPos` (`0x80090488`), `_cmp`
(`0x800927E0`), `SetVideoMode` (`0x80095728`), `GetVideoMode`
(`0x8009573C`), `KeyOnCheck` (`0x8009B214`), `SsSetMono` (`0x8009D618`),
`func_8009E900`, `func_8009E928`, `MemCardInit` (`0x800A1068`) and
`MemCardEnd` (`0x800A1098`). `CdLastPos` and `_cmp` do not match under
`cc97_lib` with any marker (2/3, 7/8). `CdLastPos` returns an address as
`lui` / `jr $ra` / `addiu`, the low half in the return slot; why the library
compiler does not give that is not settled.

### Telling the compilers apart

2.8.1 against the library compilers:

- 2.8.1 turns a jump to the epilogue into a `jr $ra` in every arm; 2.7.2 and
  the snapshot keep one epilogue and jump to it (`strcmp` (`0x8008B788`) is
  23/25 under 2.8.1). No 2.8.1 flag changes this.
- A wrapper around one call: 2.8.1 in a `-fno-delayed-branch @delay-fill`
  file stores `$ra` after the argument extensions and leaves the `jal` slot
  empty; the library compilers store `$ra` right after the stack adjust and
  put the last `sra` in the slot (`_SsSndPlay` (`0x80098968`), `SsSeqStop`
  (`0x80099368`), `SpuGetNoiseVoice` (`0x8009F388`)).

2.7.2 against the snapshot; each is a shape only the snapshot reproduces:

- Loops: 2.7.2 strength-reduces index loops the snapshot leaves alone, and
  the reverse. 2.7.2 leaves an 8-byte frame hole for a `for` to a variable
  bound (`_SsVmPitchBend` (`0x8009C574`): frame 0x40 under 2.7.2, 0x38 in
  the target). Small loops come out of 2.7.2 with every row present and two
  words too many (`func_80094644`, the three libetc `memclr` copies).
- A stack-passed parameter: 2.7.2 copies it into a saved register; the
  snapshot reads it from its home at each use (`func_80094670`, libcd
  C_011.OBJ's DMA starter: 2.7.2 builds a 0x38 frame where the target has
  0x30, 79/106).
- Member addresses (cse `find_best_addr`, read in both trees' cse.c): 2.7.2
  folds a register-relative address back to the symbol whenever the result
  is valid, so stores after the first member's are `sw $v0,S+8` macros; the
  snapshot keeps a fold only when it is cheaper and stores at `4(r)`
  (`func_800A135C`, libapi PAD.OBJ: the whole 29/30 of every 2.7.2 recipe).
- Two reads of one short member: 2.7.2's cse merges them, the snapshot keeps
  an `lh` for a signed compare and an `lhu` for the copy (`_SsVmAlloc`
  (`0x8009B228`)). A nested `?:` clamp on shorts gives the same pair, each
  load through its own `lui`, and stays frameless where 2.7.2 builds a frame
  (libgpu SYS.OBJ's `func_8008CED8`; `SetDrawEnv` (`0x8008CA50`), 105/126
  under 2.7.2).
- A short member stored into an unsigned char is loaded with `lbu` by the
  snapshot and `lhu` by 2.7.2 (`_SsVmSelectToneAndVag` (`0x8009CEE8`)).
- `c / 128` on an `int` read from a byte keeps `bgez` / `addiu 127` / shift
  under the snapshot; 2.7.2 and 2.8.1 give `srl` (`func_800A644C`, libpad
  PADCMD.OBJ).
- A guarded leaf loop: the snapshot branches to the return with
  `move $v0,$zero` in the slot; 2.7.2 threads the null test into the loop
  entry (`strlen`, `bzero` (`0x8008B728`), `memset` (`0x8008B758`)).
- A call's result tested: the snapshot and 2.8.1 copy it first
  (`move $v1,$v0` / `bnez $v1`), 2.7.2 tests `$v0` (`CdReset`
  (`0x80090494`)).

### One recipe per object

One function seldom picks the compiler. Small ones match under several, and
some objects under every recipe tried (libgpu's E04 and several of P00 to
P33). The recipe is the object's, taken from a function that only one
compiler reproduces; a function of the object that also matches under
2.8.1 (`SetDefDrawEnv` (`0x8008E878`), `GsInitGraph` (`0x8008EAF8`)) still
carries it. Every
library has such a witness for the snapshot; the scores are the snapshot's
against the best 2.7.2 and 2.8.1 recipes:

| Library | Witness |
|---|---|
| libapi | `func_800A135C` (PAD.OBJ): only under `cc97_lib` with `return_slot_fill`; 2.7.2 at best 29/30 |
| libc2 | `strcmp` 25 against 18 and 23; `strncpy` (`0x800A4C78`) 28 against 15 and 17 |
| libcd | `StCdInterrupt` (`0x80093D28`) 583 against 552 and 523; `CdGetToc2` (`0x800900CC`) 137 against 125 and 124 |
| libetc | `func_80095014` (INTR.OBJ) 82 against 74 and 49 |
| libgpu | `SetGraphDebug` (`0x8008B9FC`) 23 against 13 and 19 |
| libgs | `func_8008ECF0` (GS_001.OBJ) 136 against 20 and 75 |
| libmcrd | `MemCardSync` (`0x800A2D10`) 71 against 24 and 63 |
| libpad | `_padSetVsyncParam` (`0x800A52D0`) 11 against 1 |
| libsnd | `Snd_SetPlayMode` (`0x80098018`) 70 against 59 and 64 |
| libspu | `SpuSetKey` (`0x800A01D8`) 111 against 77 and at most 96 |

Sony changed compilers and kept the source. Compiled by 2.7.2, the C of
`func_800A135C` gives SDK 3.7's and 4.0's PAD.OBJ word for word (their
signature sets), and by the snapshot the game's. `strlen` is 2.7.2's output
on a `u8 *` in releases 3.3 to 4.0 and the snapshot's on an `s8 *` from 4.1.

`config/cc_flags.txt` gives many library files `-fno-delayed-branch
@delay-fill`. A library recipe drops `-fno-delayed-branch` and does its own
fill, so those lines change nothing for its functions: a file whose
functions all carry one builds the same text without them. They decide the
2.8.1 functions in such files (`MemCardInit` and `MemCardEnd` in
`libmcrd_init.c`).

## The library's assembler

The libraries were assembled by GNU as, the game by ASPSX. Each library
recipe applies three GNU as facts to every function it compiles:

- **Delay slots.** The compiler fills what it can; the slots it leaves empty
  get the assembler's fill (the instruction before the branch, when safe),
  and a slot cc1 filled is never touched.
- **A load-delay nop at a label.** GNU as emits the nop and then the label
  (`lw` / `nop` / `L:` / use); ASPSX puts the label first. It holds for a
  loop head and a forward join alike (`StCdInterrupt`, `CdSearchFile`
  (`0x80092508`)): every such label in library code has the nop first, every
  one in game code the label first.
- **The `div` macro.** GNU as's ends at `mflo` with no nop before an
  instruction that reads the result; ASPSX's has one. Game code has the nop
  at 28 of 28 sites, library code at 0 of 10 (`GsSetFlatLight`
  (`0x8008F5F8`) six times, `func_80098138` (libsnd SSSTART.OBJ),
  `_SsSndTempo` (`0x800998F8`) twice, `_spu_FsetRXXa` (`0x8009E7C8`)).

Four more depend on the function and are markers:

- **`gnu_as_loads`.** For an indexed load from a far symbol,
  `lw $rd,SYM($rs)`, GNU as uses the destination as the temporary
  (`lui $rd` / `addu $rd,$rd,$rs` / `lw $rd,%lo(SYM)($rd)`) unless it is the
  base; ASPSX always uses `$at`. The marker changes bytes exactly where such
  a load's result and index registers differ (`_SsVmAlloc` +30,
  `func_8008D8AC` in libgpu SYS.OBJ 2/5 to 5/5) and nothing elsewhere. A
  numeric offset outside 16 bits is the same macro, with the base added
  second by GNU as and first by ASPSX; the image has it only in
  `func_80094670`, in GNU form.
- **`return_slot_fill`.** The target's `jr $ra` slot holds the instruction
  before it, unless a label stands between or the move would leave `lw $ra`
  directly before the jump. So in a framed function, `lw $s0` (any restore
  but `$ra`) directly before the stack adjust wants the marker, and `lw $ra`
  directly before it does not (the marker costs one or two rows there). In
  a frameless one the marker matters where the last instruction is a store
  to a far symbol, a two-word macro cc1 cannot put in a slot
  (`lui $at` / `jr $ra` / `sw %lo(S)($at)`); where cc1 has already filled
  the slot it changes nothing. Measure both ways.
- **`reorder_fill`.** The assembler splits an `la` around a branch whose
  slot was left empty: `lui $r,%hi(S)` / `jal`, `j` or `bltz` /
  `addiu $r,%lo(S)`. The usual cases are a string passed to `printf` or
  `puts` (`func_80094B20`, libetc VSYNC.OBJ, +5) and several
  `printf(msg); return 0;` exits merged by cross-jumping into one call that
  each reaches by `j` (`MemCardReadData` (`0x800A2208`) +7). It applies only
  where cc1 emitted an `la`: a sized scalar under `far_syms` is already two
  instructions ahead of the call (`StGetBackloc` (`0x80093A64`) needs the
  unsized `extern CdlLOC D_800E9560[]`), and it is not needed where a store,
  not the `la`, is the last instruction before the call.
- **`far_syms`.** The libraries never use `$gp`, so a library global of 8
  bytes or less inside the `$gp` window (`_gp` is `0x800B8540`, the window
  `0x800B0540` to `0x800C053F`) needs the marker; outside the window
  `compile.py` addresses it in full by itself. It acts on the assembler
  only; cc1 still treats the symbol as small data (next section).

## The library's build options

### No small data (`-G0`)

The libraries were built with no small-data section. Under the tree's
`-G8`, cc1 takes a global of 8 bytes or less as small data and prices its
address like a register (cse `find_best_addr` with `mips_address_cost`:
register 1, symbol 2, small-data symbol 1, a tie going to the symbol). Where
cc1 itself puts a scalar's address in a register, as `expand_increment`
does for a post-increment whose value is used, the library keeps the
register and the `-G8` build goes back to the symbol. Read in GCC's expr.c,
cse.c and mips.c and in the snapshot's dumps. The target shows
`la r,S` / `lw x,0(r)` / ... / `sw x,0(r)` beside plain `%hi`/`%lo` accesses
of the same scalar. `far_syms` cannot produce it, since it acts after cc1.

The marker `lib_g0` compiles one function at `-G0`: `_putchar`
(`0x80092238`, `buf[n++] = c`: 59/63 to a match), `func_80094E44` (libetc
INTR.OBJ's interrupt dispatcher, a counter post-incremented in a test),
`func_800A594C` and `func_800A644C` (libpad), and `GsSetClip2D`
(`0x8008F1E8`, an 8-byte `RECT` stored member by member: 3/13 at `-G8`).

An unsized array declaration, `extern T D[];`, also prices the address at
2, and two-word library arrays are declared so (a sized `[2]` costs a
`lui $at` per element in `_padStartCom` (`0x800A5508`) and
`_padInitSioMode` (`0x800A5618`)). It agrees with `-G0` for reads and plain
stores; it can part where a post-increment's value is used, because an
array element's address always goes through `memory_address` and a
scalar's does not. `func_80094E44`'s counter matches only under `lib_g0`
(116, against 104 as an unsized array and 109 as a scalar with
`far_syms`), while `func_8008E16C`'s `D_800B60E0[0]++ > 0xF0000` matches as
an unsized array.

### Functions expanded in line

The libraries expand some functions in line into the functions of the same
object defined after them, constants folded, and still emit them out of
line; functions defined before them call them with `jal`. libgpu SYS.OBJ's
`set_alarm` (`0x8008E138`) is called by the functions above it and in line
in every one below it; libcd SYS.OBJ's three `CdControl`s share an inlined
command loop; libapi PAD.OBJ's `func_800A135C`, libmcrd's `MemCardOpen`
(`0x800A203C`), `MemCardGetDirentry` (`0x800A2AA0`) and `MemCardDeleteFile`
(`0x800A2F28`), and libc2's `putchar` (`0x80092370`) carry other functions'
bodies. Not every earlier function is expanded (`func_8008E16C` is called by
libgpu's `*2` functions). Over LIBMCRD, `-finline-functions` expands one
function too many (`MemCardCallback` (`0x800A2CFC`) into
`MemCardDeleteFile`, which the target calls), so there the functions to
expand were chosen in the source; for PUTCHAR.OBJ, `-finline-functions`
and `inline` give the same bytes.

The tree cannot write `inline` on a global definition: GCC 2.8.1, whose
output keeps the file's function order, defers every inline global to the
end of the file (toplev.c, read in source). So a file carries a
`static inline` copy of the callee beside the real definition
(`gpu_set_alarm` in `libgpu_sys_5.c`, `cd_set_alarm` in `libcd_bios_1.c`,
`cd_cw` in `libcd_sys_6.c`, `mcrd_accept` in `libmcrd_libmcrd_9.c`,
`pad_set_vsync` in `libapi_pad_3.c`), or the function takes the
`inline_functions` marker (`putchar`). Writing the statements out in the
caller gives the same bytes, except as below.

Tells: a store to another function's static; a straight run that is one arm
of another function's switch; no `jal` where a reference source calls; and,
among stores to one static's members, a constant stored straight to the
symbol (`sw $zero,S+12`) beside register values stored through a shared base
(`la $v1,S+4` / `sw $a0,0($v1)`). The last is the inliner's: integrate.c
substitutes a constant back into an address when the result is a valid
instruction, which a constant store is and a register store is not (read in
source and the `-dr` dump). Written as plain statements, `func_800A135C` is
25/30 in every order of them.

## Declarations that decide bytes

### Volatile

- Hardware registers are reached through pointers to volatile, declared as
  the SDK declares them (`extern volatile u_long *D_800B60A8;`, GPUSTAT),
  not through a cast at the use. Tells: `(*p >> 24) & 0x7F` stays `lw` /
  `srl 24` / `andi` where a plain pointee narrows to `lbu 3(p)`; every read
  is its own `lw`; a statement that only reads (`*GPU_STATUS;`) emits a load
  (`func_800954D4`, libetc INTR_DMA.OBJ, +19; `func_8008E16C` 73/81 without
  its bare read).
- reorg keeps volatile accesses out of delay slots, and its note of one is
  assigned, not accumulated: `mark_set_resources` sets
  `res->volatil = MEM_VOLATILE_P (x)`, so the last store of a run decides
  (read in the snapshot's and 2.8.1's reorg.c, checked in dumps). Two tells
  follow. A conditional branch whose slot stays `nop` while an instruction
  it could take stands nearby means every store between the block head and
  the branch is volatile (`func_8008D3CC`, libgpu SYS.OBJ). A `jal` slot
  that stays `nop` before a store through a hardware pointer means the
  pointee is volatile; making the first of three such pointers volatile is
  enough (`_spu_Fr_` (`0x8009E374`) +2). A volatile store that does sit in a
  `jal` slot was moved there by the assembler (`_spu_init` (`0x8009DE78`)).
- In sched (read in the snapshot's `.sched` dumps) a volatile load is
  ordered after every earlier volatile store of its block and after no plain
  one; two volatile stores keep their order and a plain store between them
  can move. So a read-modify-write of a volatile scalar stays where the
  source has it, and of a plain one moves ahead of the register stores
  (`SpuSetKey`: `_spu_RQvoice` and its neighbours volatile, +23;
  `_spu_keystat` plain).
- The value of an assignment to a volatile is read back: `a = b = 0;` is
  store, load, store on volatiles and two stores of one register on plain
  objects (libmcrd BIOS.OBJ's event flags; INTR.OBJ's interrupt status and
  mask).
- A record shared with an interrupt handler has volatile and plain members,
  and a member's kind shows in its first access in a block (probed at
  `-O2 -G0`): a volatile read is `la r,S+N` / `lw 0(r)`, a plain one
  `lw x,S+N`; a volatile store is `la r,S` / `sw x,N(r)`, a plain one
  `sw x,S+N`. For a volatile field expr.c forces the record's address into a
  register before adding the offset (read in source); later accesses are
  rebased by cse. libmcrd's record `D_800EA960` has `cmd`, `rslt` and
  `done` volatile and `chan` and `fd` plain: one of the 32 combinations
  matches `MemCardOpen` (77/98 with all five volatile), while all 128
  combinations match `MemCardReadData`. Measure on a function with a loop.
- Volatile locals: an 8-byte frame in a leaf, written once, is one
  (`CD_getsector2` (`0x80092000`)); so is a value kept in the frame and read
  twice (`SpuClearReverbWorkArea` (`0x800A0028`)). Two volatile locals take
  their slots in declaration order (`_spu_Fw1ts` (`0x8009E950`)).
- A volatile narrow value widened keeps its own `andi 0xFFFF` after `lhu`:
  combine does not merge a volatile load into an extension (`func_80095014`
  82/82, 60 with a `u16` local).
- A clearing loop that runs upward is over volatile memory; loop.c reverses
  a plain one (`_spu_init`, 153/160 plain).

### Struct members and casts

GCC's alias test takes a store to a struct member at a varying address as
independent of a load from a fixed-address scalar (sched.c `true_dependence`
with `MEM_IN_STRUCT_P`), and cse keeps its scalar equivalences across such a
store (cse.c `note_mem_written`); both read in source. A store through a cast
or a plain pointer gets neither, and any byte store through a pointer counts
as writing all memory, so pointer globals are reloaded after it. So
`p->member` and `*(T *)(p + 6)` schedule and reload differently:

- `hdr->id = 2` lets a later scalar load pass it and swaps two load/store
  pairs; through `u16 *` it matches (`data_ready_callback` (`0x800939D8`),
  29/35 as the member; a marked spelling).
- A volatile register read as a member passes a plain scalar store, through
  a cast it does not (`_spu_t` (`0x8009E41C`) 145 to 160 with the struct).
- A store through a member lets loop.c hoist a scalar load out of the loop,
  through a cast it does not (`StFreeRing` (`0x80093B58`)).
- A hardware register block is a struct reached by member through a
  `volatile T *`, not a `volatile u32 *` indexed (`_padClrIntSio0`
  (`0x800A5EC4`) 29 to 36).

Other shapes:

- `(&S.sub)->m`, the expansion of the SDK's `setRECT`-style macros, keeps
  the member offset in the address, so cse keeps a register for `&S.sub`
  and stores at 2, 4, 6 off it; `S.sub.m` folds to `S+N` (`func_8008EB6C`,
  60/71 written with members; read in the RTL and cse dumps).
- The first address formed in a block becomes its base register; in
  `a = b = c = K` that is the outermost destination, so an `la` of a
  symbol inside a struct names the destination of the block's first
  statement (`func_8008ECF0`, `_padSetVsyncParam`).
- A record copy clears cse's table: loads after it are reloads and say where
  the copy stood.
- Stepping a pointer (`p++`) over records gives a second base register at
  the last member touched; `&A[i]` and `A[i].m` do not (loop.c
  `combine_givs` needs a constant step; `PadInitDirect` (`0x800A71B8`) 67 to
  88).
- A record that a reference source reaches through a pointer local is
  sometimes written out at each use: the local loads the base before the
  index (`_SsVmSetVol` (`0x8009CFB8`) 350 to 360, `_SsVmSelectToneAndVag`
  39 to 49).

### Types

- **A typedef can change a loop.** `while (n--)` on an `int`, `unsigned` or
  `u32` compiles to a decrement and a compare with -1, fold-const having
  rewritten `n-- != 0` as `--n != -1`; on the tree's `s32` (`typedef signed
  int`) it compiles to copy, test, decrement. Probed on all three compilers.
  The rewrite needs its operand to be the post-decrement itself
  (fold-const.c); with `s32` a conversion stands between, with `u32` it does
  not (probed, not traced). `cd_cw` in `libcd_sys_6.c` is written
  `for (count = 3; count != -1; count--)` on an `s32`, which gives the bytes
  of `int count = 4; while (count--)`.
- **Narrow parameters.** Callers mask a prototyped `u8`, `u16` or `short`
  argument; in the callee such a parameter lives in two pseudos, so a
  `move $s3,$s1` is not a local copy. A `u_short` parameter of a static is
  masked on entry only when a use needs the full word. `short` parameters
  keep their argument registers across a struct copy where `int` ones do not
  (`Gssub_make_matrix` (`0x8008FD38`) 48/50 against 12/50). A `u_short`
  return type is the trailing `andi 0xFFFF` (`GetClut` (`0x8008E9B8`)).
- **The prototype in scope at a call decides the argument.** `li $a1,-1` for
  the second parameter of `SetRCnt` (`0x8008F268`) needs it to be `s16`
  (`GsInitVcount` (`0x8008F228`); `u16`, as libapi.h has it, gives
  `ori 0xFFFF`), so `libgs_gs_007.c` declares `SetRCnt` with the types its
  caller shows. A function-pointer member's return type moves registers at the call
  (local-alloc offers a call's value register to its address operand):
  `GpuTable.ctl` must return `int` (`PutDispEnv` (`0x8008C434`) 259/318 as
  `void`). A declaration as `long long` takes `$v0` and `$v1` and pushes the
  next value to `$a0`: `func_80094D6C` (libetc INTR.OBJ) matches only with
  `long long startIntrDMA();`, although `startIntrDMA` (`0x80095488`)
  returns a pointer (a marked spelling). A parameter the callee never reads
  changes no byte of the callee; a caller's argument for it still pins a
  register.
- **Signedness shows in the load:** `lb`/`lh` signed, `lbu`/`lhu` unsigned,
  and a store of -1 as `addiu $t0,$zero,-1` where `li 0xFF` would be
  unsigned. So `SsScore.cres_done` is `s16` (`_SsSndCrescendo`
  (`0x800986B8`) 128/130 as `u16`) and libsnd's `vab_id` is `s8`.
- **A shift of a `u8` is always `srl`,** whatever casts, masks or `?:`
  surround it: the front end narrows it through casts (c-typeck.c, read in
  source). `lbu` then `sra` means an `int` object held the byte, and the C
  writes an `int` local (`_padSioRW` (`0x800A5A3C`), `func_800A644C`; marked
  spellings).
- **`sizeof` is unsigned.** `p->comb_size += sizeof(PadCombInfo) + X` keeps
  the constant on the accumulator, where a signed 8 does not
  (`_padRecvAtLoadInfo` (`0x800A6028`) +12). `(max - top) * sizeof(u_long)`
  keeps a pointer difference's `sra 2` and `sll 2`, which `* 4` cancels
  (`_ExpAndAlloc` (`0x8008B640`), 22/24 with `* 4`); an arithmetic `sra 2`
  on the difference of two globals says they are pointers.

### Symbols: one or several

- Separate symbols get separate `lui`s; one struct or array is one base.
  Two `la` of addresses N bytes apart are two objects in the source, in the
  order the source first names them (`func_8008D19C` in libgpu SYS.OBJ:
  98 to 102 of 140 as one object). `GsORGOFSX` and `GsORGOFSY` are two `s16`
  scalars (`gte_init` (`0x8008FBF8`) 13/19 as one array). An unsized array
  both read and written gets one `la` shared by cse, where a scalar keeps
  its `%hi`/`%lo` pair (`StClearRing` (`0x80093978`): 14/24 on arrays, a
  match on scalars).
- A second symbol for a member of `GEnv` changes a relocation's addend, not
  the linked bytes: only a comparison of resolved relocations judges such
  an edit. The reverse also occurs: `func_800983F4` (libsnd SSSTART.OBJ)
  loads the address of `D_800B7A04`, the word after the record `D_800B79F0`,
  at each use, so it is a variable of its own (9/18 written as a member).
- On an integer base, `base + ((i + n) << 5)` computes the shift first and
  adds the base last; on a pointer, or with `* 32`, the base is loaded first
  (`StFreeRing` +4; read in the `.rtl` dump). libcd's ring base is an
  integer in C_007, C_008 and C_009.

## Statements that decide bytes

### Loops

- **Hoisting.** loop.c moves an invariant out when threshold × savings ×
  lifetime is at least the loop's instruction count; the threshold starts at
  (1 + the number of non-fixed hard registers), doubled for a loop without a
  call, and drops by 3 after each hoist in loop order (the snapshot's
  `move_movables`, read in source; `-dL` prints each loop's count and each
  movable's savings and life). Under `-mhard-float` the starting value for a
  loop with a call is 61. Consequences:
  - The same code hoists inside a short loop and not after it:
    `MemCardOpen` with its success path after its retry loop hoists three
    constants (84/98), with it inside and a `break` only one (98/98). An
    unhoisted `li 3` beside a hoisted `li $s1,2` was written inside the
    loop.
  - A constant used once is hoisted only while the loop is short.
    `_SsVmSetVol`'s division constants need savings × lifetime of at least
    3, which only a second use gives.
  - A loop entered twice is not optimised at all ("ignored due to multiple
    entry points" in `-dL`): a `return` as its last statement, or success
    code after it with a failure `return` inside, makes one (`CdInit`
    (`0x800902F8`): 11/36 with the success block in the loop, 36/36 after).
- **Blocks moved out.** `find_and_verify_loops` moves a block that leaves
  the loop (a `break` or `return` arm skipped by a branch) to the nearest
  barrier outside, in front of the loop head, and inverts the branch. So
  `return K` blocks in front of the code that branches to them come from a
  loop, and `malloc2` (`0x8008B488`)'s three `break` arms sit above its
  loop. `MemCardCreateFile` (`0x800A2E2C`) has such blocks and no loop that
  repeats: its C is `while ((rslt = _card_create2(...)) != 0)` around a body
  in which every path returns (63/63; straight-line 46/63), and SDK 4.0's
  object has a real retry loop there.
- **Reversal.** A counted loop whose counter has no other use runs
  backwards: an ascending `for` gives the target's countdown (`_SsInit`
  (`0x800959D8`), `MemCardUnformat` (`0x800A3408`), `_SpuInit`
  (`0x8009DCF8`)'s fill from the table's last word). Write it ascending.
- **Entry tests.** The snapshot copies every `while`'s test to the loop
  entry (jump.c `duplicate_loop_exit_test`); cross-jumping after reload
  deletes the copy when the two are identical. A missing copy does not mean
  `do`/`while`, and a present one still counts in live lengths.
- **Rotation.** A signed division in the exit test (`len / 4`) leaves the
  loop un-rotated, the test at the top (`func_8008D99C`, libgpu SYS.OBJ);
  `len >> 2` rotates it. Before writing `for (init; 1; incr)`, look for a
  branch inside the exit test.
- **After the loop.** cse's block ends at the loop's end note, so the code
  after a loop starts with an empty table (cse.c, read in source). A
  register reused after a loop means the code was inside it.
- **Saved registers.** A loop variable of its own, not a stepped parameter,
  changes the saved-register order (`CdGetToc2` +22).

### Switches

- The compare tree is balanced over all the case values, the ones that share
  the default's code and emit no compare included (stmt.c
  `balance_case_nodes` splits a run at (n + 1) / 2). The root and sub-roots
  give the number of values: `BtlMixColNumberSet` (`0x8008B27C`, game code)
  needs an eleventh value with the default's colour (38/59 without).
- Case bodies follow source order behind a tree sorted by value; the body
  the dispatch's last test falls into is the first case written.
  `func_800A34BC` (libmcrd) fixes only part of its order: 45 of the 120
  orders of its five labels give its bytes.
- A dispatch through a function table has the call written in every case;
  jump2 merges them into one `jalr`. A statement two cases share is written
  in both (`_SsSetControlChange` (`0x80097648`)). Break-only cases are
  written out, each alone.
- In 2.8.1 game code, a switch of constant assignments joined at a call
  wants `default:` written last (`BtlMixColNumberSet` +16). reorg leaves a
  marker after the join's label that makes the argument register live on
  every path entering by that label; with the default last, a new label
  stands between, the register is dead, and each case's `li` is stolen into
  its `beq` slot (read in an instrumented build of reorg.c). Tells: some
  leaf branches go straight to the join with the case value in the slot;
  exactly one, the last by address, has `beq` / `nop` and keeps its block;
  every `j` after a leaf carries the default's value.

### Returns, joins and cross-jumping

- jump2 merges identical tails after reload, from a single matching
  instruction (jump.c `find_cross_jump`). So a constant argument on one path
  into a shared `jal` means the call was written in both arms (`func_800A22C0`
  in libmcrd: 71/79 with one call after the `if`), and an argument register
  cleared in the entry block and overwritten after a branch means one call
  with a conditional argument (`func_8008EB6C`).
- Which `return 0;` survives: a copy with a label above it merges into any
  copy; a plain `if (c) return 0;` merges only into one with a jump above
  it; a copy preceded by a store only receives (`_padRecvAtLoadInfo` +16).
- A redundant `move $aN,$sN` right after the prologue made that copy means a
  label stood there when jump2 ran: its no-op-move pass gives up at a label
  and runs before cross-jumping, so an early return was merged away
  (`strcat` (`0x800A38E8`); read in jump.c and dumps).
- One `return ret;` after an if/else, not one per arm, can be worth a
  register order (`_padSioRW` 111 to 132; see Allocation). In a chain of
  returns, an arm that is only `return K;` becomes a lone `li` above the
  epilogue, reached on the source's own condition (`func_8008DFFC`,
  `func_8008E2B0`, libgpu SYS.OBJ).
- combine rewrites `x == 0` as `x + 1` when `x` can only be 0 or -1
  (`return cd_cw(...) == 0;` in `CdControl` (`0x80090604`); read in
  combine.c).
- Whether an early `return 0;` or a nested `if (ok) { ... } return 0;` is
  the source differs per function; it is not a rule.

### Scheduling

- sched1 schedules a block backwards. A pseudo set once is "birthing" and is
  placed right above its use; a value set more than once floats to the top
  of its block; stores keep source order (the snapshot's sched.c, read in
  source and dumps). So a value built in two statements (`m = 1; m <<= n;`)
  lands elsewhere than one expression (`func_80094670` +21), a computation's
  head beside an unrelated store means its statement came right after that
  store, and a `li $v0` in the middle of a block is a variable set more than
  once. After reload nothing is birthing, and sched2 moves late pure
  computations up into load-delay gaps.
- A parameter's entry copy stays at the top of the first block only up to
  the first instruction that is not a copy, so a local's initialisation can
  stand above `move $a2,$a0`. The fix is the parameter's one use being an
  assignment in the entry block (`SsSeqOpen` (`0x80095B38`), the marked
  `seq = (u8 *)addr;`).
- The stack adjust has the lowest priority in sched2, fills a load-delay gap
  and can then be taken into a branch's slot, so a function's first rows can
  end in a branch with the `addiu $sp` beyond it (`CD_getsector2`,
  `data_ready_callback`).
- A long straight-line function is one block that sched1 reorders: find its
  statement order by trying positions, not by reading the listing. Several
  positions often give the same bytes.

### Allocation

- global-alloc ranks a pseudo by floor_log2(refs) × refs / live_length and
  gives it the lowest free saved register (global.c, read in source and
  `.greg` dumps). A pure rotation of `$s` registers is a reference count: an
  early return, a second test or a used copy changes it (`PadEnableCom`
  (`0x800A51B8`) +9 with an early `return old;`).
- Register pressure looks like source and is not. A constant that lost
  global allocation is re-materialised by reload into the first unused
  call-clobbered register (`li $t0,K` before a compare in `CdControl`: ten
  values live across calls, nine saved registers); a parameter is read from
  its incoming slot at each use; a hoisted pointer lives in a frame slot.
  Count the values live across calls first.
- One local with two live ranges keeps one hard register: an unexplained
  `move $v1,$v0` after a call is `ev = f(); D = ev;` written in both arms
  (`func_800A1D94`, libmcrd, +17).
- local-alloc ties an operation's result to its first operand, which
  decides what a delay slot can take: the operand order of a sum or an `|`
  is the source's.

### Expressions

- fold-const re-associates `prev - 1 + mode` into `prev + (mode - 1)`:
  `VSync` (`0x800949A8`) holds the 1 in a variable (`n = 1;`, a marked
  spelling; 80/94 with the literal). Nested constant divisions are merged
  through a macro and kept through a `static inline` function
  (`CdIntToPos` (`0x80094818`) +46); `CdPosToInt` (`0x8009491C`) needs the
  opposite.
- A signed power-of-two `/` and `%` behind a `<= 0` early return keep no
  sign fix-up, and `/16` merges with `/2` into `sra 5` (`func_8008D608`,
  libgpu SYS.OBJ; read in the `.cse` and `.combine` dumps).
- `p + (*p >> 2) + 1` scales and adds twice, the 1 becoming a load offset;
  `&p[(*p >> 2) + 1]` folds the 1 into the index (`malloc2`). An address
  computed as a value puts the pointer first in its `addu`, one inside an
  indirection the scaled index.
- `(x >> 2) << 2` stays `sra`/`sll`, `& ~3` is `li -4`/`and`; on a count
  derived from a byte both become an `andi` whose mask shows the type
  (0xFFC, 0x1FC, 0xFFFC).
- `x = x | A | B | C` and `x |= A | B | C` build different trees
  (`_SsUtBuildADSR` (`0x80096CF4`) +16). `x[n++] = v` copies the old count,
  `x[n] = v; n++;` does not.
- A `?:` loads its false arm's constant first and replaces it behind the
  branch (`SetDispMask` (`0x8008BB6C`)). `if (c) x = K; else { x = y; f =
  0; }` puts `li x,K` in the branch's slot, `x = y; if (c) x = K; else f =
  0;` keeps a `j` (`SpuClearReverbWorkArea` +4; the jump pass, read in
  dumps).
- A store to a local nothing reads is still emitted: if the target lacks it,
  so did the source.
- GCC treats a call as returning twice by the callee's name (calls.c:
  `setjmp`, `sigsetjmp`, `savectx`, `qsetjmp`, `vfork`) and keeps values
  out of hard registers across it. `func_80094D6C` reloads a base after
  calling `setjmp` (`0x800952E8`) only because of the name (36/54 under
  another).

## Forcing devices: what they stood for

Library C fitted under 2.8.1 or a 2.7.2 recipe needed devices:
`do {} while (0)` wrappers, `if (1) {}`, `new_var` temporaries, twin arms on
an uninitialised test, byte-offset casts, second symbols for members,
use-site `volatile` casts, asm barriers, parameters that existed to put a
value in a register, hand-written pointer walks. Under the object's recipe,
with the SDK's declarations, plain C matched, nearly always at the first
compile. The devices stood for:

- **The wrong compiler.** The `do {} while (0)` in front of a wrapper's one
  call stood for 2.8.1's schedule (above); `new_var` chains, twin arms,
  preloaded constants and pointer walks stood for 2.8.1's or 2.7.2's
  allocation and loop optimiser (`SetGraphDebug`, `_SsContNrpn1`
  (`0x80096598`), `SpuFree` (`0x8009F018`), `_SpuInit`, `_SsVmSetSeqVol`
  (`0x8009C7D8`)). Locals named after registers, together with a recipe
  that is not the object's, are the sign.
- **Missing types.** Byte-offset casts and second symbols stood for struct
  members (`GEnv`, `SsScore`, `StHEADER`, libmcrd's record); integer heap
  globals with casts at each use for `u_long *`; `extern s32 D[]` read
  `[0]` for a scalar pointer (`_spu_RXX`: with a typed pointer, `_spu_Fr_`
  and `_spu_t` match even under 2.8.1).
- **A wrong declaration.** Use-site `(volatile T *)` casts stood for a
  volatile declaration, m2c's `& 0xFFFF`s for a `u_short` return, `void`
  results for the SDK's `int`, and extra parameters for a pointer local
  that lands in `$a3` by itself.
- **Nothing.** Some devices were left from a search: `setIntrVSync`
  (`0x8009542C`) and the libetc `memclr` copies match as plain text even
  under 2.8.1. A device in matching C is no evidence that the plain
  form fails.

A wrong compiler can also hide the right declaration. Under 2.7.2,
`StCdInterrupt`'s true `extern volatile u16 *D_800E9578` costs rows
(563/583), so C fitted under it used a plain pointer, a use-site cast and
a `frame` local; under the snapshot the declaration alone matches. A function
still built by a 2.7.2 recipe, or fitted with a device, is worth one plain
retry under `cc97_lib`.

What is left:

- **Marked spellings.** Where no plain spelling gives the bytes and the
  original is unknown, the C keeps the fitted one, and its comment says
  `MARKED SPELLING` with the plain alternative's score. They are
  semantically inert. The kinds: a constant held in a variable (`VSync`'s
  `n = 1`, `func_8008D3CC`'s `mode`), a parameter copied into a local
  (`SetDrawEnv`'s `dr = dr_env`, `SsSeqOpen`'s `seq`, `_SsNoteOn`
  (`0x80097A48`)'s `voll`), an `int` local for a byte shifted
  arithmetically, a case order (`func_800A34BC`), a loop that never comes
  round (`MemCardCreateFile`), a dead statement (`_SsContDataEntry`
  (`0x80095E48`)), a narrow view for one access (`StCdInterrupt`'s
  `*(s16 *)`, `data_ready_callback`'s `u16 *`), a second read through
  `*(volatile int *)` (`SpuMalloc` (`0x8009EA48`)), `long long`
  `startIntrDMA`, an eleventh case label (`BtlMixColNumberSet`).
  `grep 'MARKED SPELLING'` over the C lists them: 25 in library files, the
  rest in game code.
- **One function with devices.** `_SsVmSetVol` keeps four: a dead copy of
  three statements, which gives loop.c's hoist the second use it needs (see
  Loops); two double `do {} while (0)` wrappers, tagged `CODEGEN FORCING
  DEVICE` in the C; and an empty `__asm__ volatile ("")`. The wrappers and
  the barrier tip global-alloc's ties. No natural text was found: the best
  without them is 290/360, and leaving out one at a time gives 317 (the
  copy), 340 and 350 (the wrappers) and 354 (the barrier).

## Methods that held

- Take the recipe from the object, and score plain C under it before
  fitting anything.
- Read the compiler rather than guess. cc1 does not run the preprocessor:
  run the toolchain's `cpp -P` first, then the snapshot's
  `cc1 -quiet -O2 -G8 -mhard-float -fno-exceptions` with `-da` or single
  dumps (`-dr`, `-dj`, `-ds`, `-dc`, `-dL`). The loop dump prints each
  loop's instruction count and each movable's savings and life.
- Compare objects keeping zero words (`objdump -z`): dropped nops make a
  function look shifted. Compare resolved relocations when an edit replaces
  a symbol by a member offset.
- Earlier SDK releases show the source: `MemCardCreateFile`'s fossil loop is
  a real loop in 4.0, and a helper inlined in libapi PAD.OBJ is an
  out-of-line function in LIBTAP 3.3. The oldest signature sets are the
  place to look for a small named function with a residue's instructions. A
  version string does not pin an object: 4.4, 4.6 and 4.7 carry the same
  `sys.c` 1.140 and differ in code.
- An SDK header's disabled statement list gives an object's order: libsnd.h's
  `jt_SsInit` block is the order of `SsSeqOpen`'s 37 stores into the
  reader's function table.

## Hand assembly

Some objects are Sony's assembly, not compiled C
([hand-assembly.md](../../hand-assembly.md)). The marks met in the library:
`ctc2`; `$ra` stored in `.bss` with no frame; an in-line BIOS call
(`li $t1,N` / `li $t2,0xB0` / `jalr $t2`); data words after `jr`; a
frameless function that stores the callee-saved registers, `$sp` and `$gp`
through its argument (`setjmp`, `longjmp` (`0x80095324`); no C reading
comes near them under any recipe); dead instructions (`li $t2,9` in
`_patch_card_info` (`0x800A1658`)). A copy loop's load-delay nop and its
order are not a mark: cc1 with both scheduling passes off prints the same.
