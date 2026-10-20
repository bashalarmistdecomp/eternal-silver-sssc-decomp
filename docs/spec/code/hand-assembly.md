# Assembly in the original program

Some functions of the three programs were never C: Sony's kernel call stubs,
the GTE register functions, `setjmp` and `longjmp`, the start-up code, the
memory card's patch code. The tree keeps them as byte ranges (`INCLUDE_ASM`)
for good. This page says which they are and what evidence decides it, kind
by kind.

The record is `decomp/<serial>/config/asm_only.txt`, one for each program:
one row per function, with its name, its address and its evidence, in
address order. It is measured on the program image with station-tools, a
custom decomp toolkit that will be released later, and is generated, never
edited by hand. The top-level [README](../../../README.md) counts what each
program still has as assembly; this page agrees with those counts.

| Program | Still assembly | On the record | The rest |
|---|---:|---:|---|
| `SLUS_006.28`, the main executable | 65 | 65 | none |
| `SLUS_006.28-MENU`, the overlay | 3 | 2 | `movie_slice_load` (`0x801967DC`), compiled, not yet matched |
| `SLUS_009.21`, the bonus disc | 265 | 33 | 225 of Sony's compiled functions and 7 of open work |

## The rule

A function was assembly in the original only when its bytes carry something
the C compiler never emits, or when it lies in an SDK object file, proven in
place, that holds such a function. A failed match is not evidence, and
neither is a note in a text. A function with no row on the record was
compiled, however small or odd it looks, and its C is work to do.

The kinds of evidence are a closed list:

| Kind | What is measured | Why the compiler cannot have produced it |
|---|---|---|
| `insn` | `mfc0` `mtc0` `rfe`, `mfc2` `cfc2` `mtc2` `ctc2` and the GTE commands, `lwc2` `swc2`, `syscall`, the trapping `add` `addi` `sub` | cc1 has no pattern for them (it emits `addu`, `addiu`, `subu`) |
| `reg` | `$k0` or `$k1` as an operand; `$sp` stored to or loaded from memory; `$ra` or `$gp` saved off a base other than `$sp` or the frame base the function has just computed from it (`addu base, x, $sp`) | the compiler never allocates the kernel registers, and saves `$ra` only in its own frame: through `$sp`, or through `addu $t5,$t4,$sp` when the frame is over 32 K |
| `tail-jump` | no `jr $ra` anywhere; the function leaves through `jr` on another register | this compiler emits no tail calls |
| `bios-thunk` | `li $t2, 0xA0/0xB0/0xC0 ; jr $t2 ; li $t1, n` | a tail jump with a name: the kernel's table and the index identify the call |
| `not-code` | a word that is no MIPS I instruction (the list gives its offset in the function, not the word) | data placed in `.text` |
| `object-member` | the function lies inside an SDK object file pinned in the image (`config/object_pins.txt`) in which another function carries one of the five kinds above | an object file is the output of one run of one tool, the assembler or the compiler (§"Object membership") |

The first five are read off a function's own words. The sixth is not: the
object file a function lies in decides it.

### The negative control

Every function the tree compiles from C is compiler output by proof, so a
rule that fires on one of them is wrong. Each kind is admitted only because
it fires on none. The record is measured over every function of the image,
compiled or not, and in the main program the 65 functions it lists are
exactly the 65 the tree still reads as byte ranges: none of the functions
compiled from C carries evidence.

The `reg` kind leaves out one shape because the control showed the compiler
emits it. For a frame over 32 K, cc1 saves `$ra` off a base it has just
computed from `$sp`: `li $t4, size ; subu $sp,$sp,$t4 ; addu $t5,$t4,$sp ;
sw $ra,-8($t5)`, and the mirror image in the epilogue (compiled from
`char b[40000]` with the tree's cc1 at `-O2 -G8`). No function of the main
image has such a frame. On the bonus disc three do: `CastleOptionsRun`
(`0x8010F55C`), `CastleGameRun` (`0x80114A0C`) and `Krom2Tim`
(`0x80120EAC`). None of the three carries evidence.

A C body that is one `asm` string of the original instructions is assembly
in a C wrapper, and it counts as assembly. No function of the three trees is
written that way. The one `asm` statement in the C is an empty barrier,
`__asm__ volatile("")`, in `_SsVmSetVol` (`0x8009CFB8`,
`libsnd_vm_vol.c`), and it emits no code.

### A rule that is not used

"A temporary register is read before it is written" was tried over `$at`,
`$v0`, `$v1` and `$t0` to `$t9`, the registers the calling convention leaves
undefined at entry, in three readings, each measured over 2,227 functions
compiled from C:

| Reading of "before" | Compiled functions it fires on | Functions with no other evidence it fires on |
|---|---|---|
| on every path from the entry, nothing has written the register | none | none: it fires on `func_800A169C` and `func_800A16C8`, which object membership already decides, and on `func_80090044`, which has `reg` and `insn` rows |
| on some path from the entry | `BtlSlotNearestFind` (`0x80039838`, `$t4`), `_spu_pitch2note` (`0x800A0C58`, `$t2`, `$t1`), `MapSpotSideAlignCount` (`0x80066E64`, `$v1`) | none beside the two patch bodies |
| no word at a lower address writes it | `resource_pair_find` (`0x80020C58`), `resource_pair_key_find` (`0x80020C94`), `fmt_vsprintf` (`0x80021B90`), `inflate_codes` (`0x80022840`), `sound_pair_find` (`0x80023E5C`) | none beside the two patch bodies |

The second reading's hits are compiled C; in the first two, a variable
assigned on one path is read after the paths join. The third reading's are loops entered at the
bottom, where the test at a higher address writes what the body at a lower
address reads. The strict reading has no hit but adds no function, so
nothing is lost by leaving the rule out, and no ruling of the owner admits
it.

## The main program: 65 functions

39 are kernel call stubs (`bios-thunk`): `InitHeap`, `open`, `read`,
`OpenEvent`, `InitPAD2`, `_card_read` and the rest, each a jump into the
kernel's table A0, B0 or C0 with the function's number. The record gives
each one's table and number (`bios-thunk:B0:32`), and
[subsystems/bios-calls.md](subsystems/bios-calls.md) lists them with their
library members.

The other 26:

| Function | Whose code (library, member) | Evidence |
|---|---|---|
| `__SN_ENTRY_POINT` (`0x80013C4C`) | the start-up object `2mbyte.obj` | `$ra` saved off `$at` and loaded off `$ra`, `addi`, and four data words `0x00200000` in its text (`not-code`) |
| `EnterCriticalSection` (`0x8008B3B8`), `ExitCriticalSection` (`0x8008B3C8`) | libapi `A36`, `A37` | `syscall` |
| `func_8008FE08` | libgte `MSC00`: the 8 bytes at the member's head, the release's stamp ("Ps" and 0x45) | `not-code` x2 |
| `InitGeom` (`0x8008FE10`) | libgte `MSC00` | `mfc0`, `mtc0`, `ctc2` x7, `$ra` saved off `$at` |
| `SquareRoot0` (`0x8008FE98`) | libgte `MSC01` | `mtc2`, `mfc2`, `sub` x2, `addi` x2 |
| `SetColorMatrix` (`0x8008FF28`) | libgte `MTX_11` | `ctc2` x5 |
| `SetFarColor` (`0x8008FF58`) | libgte `REG11` | `ctc2` x3 |
| `SetGeomOffset` (`0x8008FF78`) | libgte `REG12` | `ctc2` x2 |
| `_patch_gte` (`0x8008FF98`) | libgte `PATCHGTE` | `$ra` saved off `$at` |
| `func_80090044` | libgte `PATCHGTE`: two fragments of exception-entry code after `_patch_gte` | `$k0` x8, `$ra` saved off `$k0` x2, `mfc0` x3 |
| `func_80095298` | libapi `C114`: the release's stamp at the member's head, ahead of `_96_remove` | `not-code` x2 |
| `setjmp` (`0x800952E8`), `longjmp` (`0x80095324`) | libc2 `SETJMP` | `$ra`, `$gp` and `$sp` saved or loaded through `$a0` |
| `EnablePAD` (`0x800A1518`), `DisablePAD` (`0x800A152C`) | libapi `PATCH` | `tail-jump` (`jr $t1`) |
| `_patch_pad` (`0x800A1540`) | libapi `PATCH` | `$ra` saved off `$at`, `addi` x2 |
| `_remove_ChgclrPAD` (`0x800A15B8`) | libapi `CHCLRPAD` | `$ra` saved off `$at`, `addi` |
| `_patch_card_info` (`0x800A1658`), `func_800A169C`, `func_800A16C8`, `func_800A170C`, `_patch_card` (`0x800A1734`), `_patch_card2` (`0x800A17C8`), `_copy_memcard_patch` (`0x800A1838`) | libcard `PATCH` | piece by piece in §"The pinned object: libcard's PATCH" |
| `_ExitCard` (`0x800A1878`) | libcard `END` | `$ra` saved off `$at` |

The identity column is the register's (`config/symbol_addrs.txt`; by
address in [subsystems/psyq/index.md](subsystems/psyq/index.md) §"Still
assembly"). The evidence column is the record's. The two are independent:
identity says whose code a function is, evidence says it was not compiled.
They agree in a third way too: `make CODE=sdk` reads all 65 from PSY-Q 4.5's
own files and the image still matches
([../../../decomp/SLUS_006.28/from-sdk/README.md](../../../decomp/SLUS_006.28/from-sdk/README.md)).

No function of the game's own code is on the record: every function the
developers wrote was compiled. `__SN_ENTRY_POINT`, the one row outside the
libraries' range of the image, is Sony's start-up object. No game function
carries a GTE instruction, so there is no C here written with the SDK's
inline GTE macros.

One more routine was never C and is not on the record, because the tree has
no function there: `__main`, the eight bytes of `2mbyte.obj` at
`0x80013C44`, ahead of `__SN_ENTRY_POINT`. The tree still reads them as data
([../../naming.md](../../naming.md) §"Open"), so the program has 66 routines
that were never C.

## Object membership

**The rule.** `config/object_pins.txt` is a register of SDK object files
pinned in the image. A row gives the library file, the member, the sha256 of
the member's payload, the length of its `.text`, the image address where
that `.text` begins, and how a relocated word is masked. Each row is
measured again whenever the record is: the member's payload hash in the
library file, the `.text` length, and the `.text` against the image word for
word with only the masked bits aside. The masked bits are, for each
patch-record type the object carries, the bits the linker fills: the low 16
for `HI16` and `LO16`, the low 26 for `J26`. A row may not mask more than
that, and an object with a patch type the row does not list refuses the row.
A row that fails grants nothing. A row that verifies grants
`object-member:<LIB>/<MEMBER>` to every function inside its range only if at
least one function inside the range carries one of the five primary kinds.
The functions that carry a primary kind keep it, and `object-member` never
counts as the evidence that lets an object grant. The owner ruled that this
kind is admissible, and the ruling covers object membership and nothing
else.

**Why the compiler cannot have produced it.** An object file is the output
of one run of one tool: the assembler writes it from an assembly source, or
the compiler from a C source. A function that shares an object with one the
compiler cannot have emitted came out of the assembler with it. The premise
has one way to fail, a C source with `asm` statements in it, and that is
what the control looks for.

**The control.** Two measurements:

- No function the tree compiles from C may lie in a pinned range, wholly or
  in part; a hit refuses the row. The one pinned object holds 7 functions,
  none compiled from C.
- The survey. Every member of release 4.6's 23 libraries beside the pinned
  one (2,216 members) was laid over the image the same way. 2,005 have a
  `.text` of six words or more that parses (2 do not). 208 lie exactly once
  in the image (28 more lie at several places and are left out). 198 of
  those hold a function the tree compiles from C (318 functions), 10 hold a
  function with a primary kind, and none holds both.

**The evidence side is thin.** Only four of the ten objects with evidence
hold more than one function: libapi `PATCH` (3), libc2 `SETJMP` (2), libgte
`PATCHGTE` (2) and libcard `PATCH` (7), the object the kind was admitted
for. So the control shows that the rule is never contradicted, not that it
is often confirmed, and libcard's `PATCH` is the only object in which it
decides anything. In the survey six functions with a primary kind lay in no
object found once (`__SN_ENTRY_POINT`, `EnterCriticalSection`,
`ExitCriticalSection`, `func_8008FE08`, `InitGeom`, `func_80095298`). The
survey used release 4.6, and the program links 4.5
([../../naming.md](../../naming.md) §"Which release of Sony's libraries the
programs link"), so a member that 4.6 changed is not found by it; release
4.5's own files hold all 65 functions of the record.

**What the kind does not do.** An object nobody pinned grants nothing. A row
is added by hand, only with the measurement that produced it, and never to
take a function out of the work. The register says where an object lies by
address.

### The pinned object: libcard's PATCH

Release 4.6's `LIBCARD.LIB` (6,148 bytes, sha256 `3310af56…c8ba`), member
`PATCH`: payload 1,052 bytes, sha256 `176617a0…6bea`; `.text` 544 bytes, 136
words. It lies at `0x800A1658`..`0x800A1877`. The 107 words without a patch
record are equal, and the 29 patched words are equal outside the masked
bits. The 12 fields patched with `.text + K` were also computed from the base
address and found equal, so nothing is masked there. No word of the object
uses `$sp`, and none of its seven pieces is compiled from C:

| Piece | Text offset | Function | Evidence |
|---|---|---|---|
| `_patch_card_info` | +0x000 | `0x800A1658` | `reg`, `insn` |
| the first patch body (no symbol) | +0x044 | `func_800A169C` | `object-member` |
| the second patch body (no symbol) | +0x070 | `func_800A16C8` | `object-member` |
| two five-word stubs (no symbol) | +0x0B4 | `func_800A170C` | `tail-jump` |
| `_patch_card` | +0x0DC | `0x800A1734` | `reg` |
| `_patch_card2` | +0x170 | `0x800A17C8` | `reg` |
| `_copy_memcard_patch` | +0x1E0 | `0x800A1838` | `object-member` |

The two bodies are code kept as data: `_copy_memcard_patch` copies the 28
words from `0x800A169C` to kernel RAM at `0xDF80`, where the second body
runs at `0xDFAC`. What the pieces do is in
[subsystems/psyq/libapi-libcard-libmcrd.md](subsystems/psyq/libapi-libcard-libmcrd.md).

libapi's pad patch is a different object. `_patch_pad` (`0x800A1540`) lies
in libapi's `PATCH`, at `0x800A1518`..`0x800A15B7` with `EnablePAD` and
`DisablePAD`; `_remove_ChgclrPAD` is libapi's `CHCLRPAD`. Neither is pinned,
and their functions are on the record by their own marks.

### An observation that is not a kind: kernel ROM words

The first five words of `func_800A169C` occur at exactly one place in the
SCPH-1001 kernel ROM image (524,288 bytes, sha256 `71af94d1…99d3`):
+0x14898, kernel RAM `0x4D98`. The first three words of `func_800A16C8`
occur at exactly one place too: +0x15F44, kernel RAM `0x6444`.
The first six words of `_copy_memcard_patch` occur nowhere in it. So both
patch bodies open with words of the kernel routine they are patched into.
This is an observation. "A copy of kernel ROM words" is not a kind of
evidence: nothing ruled on it, it has no negative control, and the kernel
image is a reference, never a source.

## The other two programs

**The overlay** has two rows, both libpress (release 4.1's, as the overlay
links it): `DecDCTvlcSize` (`0x80197320`, `addi`) and `DecDCTvlc`
(`0x80197350`, `add` x22, `addi` x31, `sub` x2, `mfc0`, `mtc0`). No object is
pinned. Its third function still in assembly, `movie_slice_load`
(`0x801967DC`), has no row: it was compiled, and matching it is open work
([../../../README.md](../../../README.md) §"Outstanding").

**The bonus disc** has 33 rows: 22 kernel call stubs, and `__SN_ENTRY_POINT`
(`0x8011F064`), `EnterCriticalSection`, `ExitCriticalSection`,
`EnablePAD`, `DisablePAD`, `_patch_pad`, `_remove_ChgclrPAD`,
`DecDCTvlcSize2`, `DecDCTvlc2`, `setjmp` and `longjmp`, each with the kinds
of evidence its namesake has in the main program or the overlay. No object
is pinned. Its
other 232 byte ranges carry no evidence: 225 are Sony's compiled code, byte
for byte (`decomp/SLUS_009.21/config/vendor_provided.txt`), and 7 are open
work.

## Compiled, though they look otherwise

Three functions of the main program have been taken for assembly and are C.
Each carries no evidence:

- `StSetRing` (`0x80090078`), libcd's `cdrom.o`: eleven instructions, with
  its own prologue and return (`libcd_cdrom.c`).
- `func_800920F8`, the CD-ROM interrupt handler of libcd's `bios.o`, which
  `CD_init` and `CD_initintr` install: its first two instructions sit ahead
  of its prologue (`libcd_bios_3.c`).
- `_SsVmSetVol` (`0x8009CFB8`), libsnd's `vm_vol.o`: it has a compiler
  frame. Its C matches only with three constructs that emit no code (a dead
  copy of three statements, two nested `do { } while (0)` pairs, the empty
  `asm` statement above), so how its source was spelled is not known. That
  is open, and it does not make the function assembly.
