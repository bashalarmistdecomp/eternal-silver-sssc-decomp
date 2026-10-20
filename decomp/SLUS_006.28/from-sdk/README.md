# from-sdk/ — the same routines, read from Sony's SDK files

`../asm/` holds this program's routines that are not C as byte ranges of
your disc's program image. This folder holds all 65 of them a second time,
as byte ranges of Sony's own PSY-Q files: the routines that were assembly in
the SDK and that the game links unchanged. Why each was never C is in
`../config/asm_only.txt` and `docs/spec/code/hand-assembly.md`.

`make CODE=sdk` looks here before it looks in `../asm/`, so these 65 come
from the SDK and none from the disc. The build must end with the same OK
line either way, and it does: the SDK's copies are the same bytes.
`BUILD.md` ("A second source for the routines") says how to fetch the files
and what they are.

Nothing here is Sony's. A stub names one of Sony's files, offsets and
lengths, and, where the routine refers to something else, which of this
tree's symbols stands at that address (shortened here):

```
glabel InitGeom
    .reloc InitGeom + 0x8, R_MIPS_26, _patch_gte
    .incbin "lib/libgte.lib", 0x4C5A, 0x2
    .byte 0, 0, 0
    .incbin "lib/libgte.lib", 0x4C5F, 0x5
endlabel InitGeom
    .space 0x8
```

The files are PSY-Q 4.5's, the release this program links, in Sony's own
formats and unchanged: four libraries (`.lib`) and the start-up object
`2mbyte.obj`. Their sha256 are in `recipe/toolchain.sh`, and the offsets are
offsets in those files. Sony's files keep a placeholder in every field the
linker fills (the target of a jump, the halves of an address), so a stub
does not read those bytes: zeros stand there, and the `.reloc` line gives
the field its value. Another release, or the same one repacked, has the
routines elsewhere.

One routine of Sony's is not here because the tree does not treat it as a
routine: `__main`, the eight bytes before `__SN_ENTRY_POINT` in the same
object. The first split took them for data, and they are still read from
the disc with the data (`docs/naming.md`, "Open").

## The routines

The tree calls each of these by the SDK's own name
(`../config/symbol_addrs.txt` has the evidence for each), except 6
pieces of a member that have no exported name.

"At" is where the routine starts in the member's code. "Bytes" is the
routine's length: what the stub reads from Sony's file plus the fields it
leaves to the linker. "Pad" is zero padding after the routine, which no
source supplies. "Refs" counts those fields, each pointed at one of this
tree's symbols.

| Routine | Sony's file | Member | At | Bytes | Pad | Refs |
|---|---|---|---:|---:|---:|---:|
| `__SN_ENTRY_POINT` | `lib/2mbyte.obj` |  | `.text + 0x8` | 188 | 0 | 18 |
| `InitHeap` | `lib/libapi.lib` | `C57` | `.text + 0x0` | 12 | 4 | 0 |
| `FlushCache` | `lib/libapi.lib` | `C68` | `.text + 0x0` | 12 | 4 | 0 |
| `_bu_init` | `lib/libapi.lib` | `C112` | `.text + 0x0` | 12 | 4 | 0 |
| `EnterCriticalSection` | `lib/libapi.lib` | `A36` | `.text + 0x0` | 16 | 0 | 0 |
| `ExitCriticalSection` | `lib/libapi.lib` | `A37` | `.text + 0x0` | 16 | 0 | 0 |
| `open` | `lib/libapi.lib` | `A50` | `.text + 0x0` | 12 | 4 | 0 |
| `lseek` | `lib/libapi.lib` | `A51` | `.text + 0x0` | 12 | 4 | 0 |
| `read` | `lib/libapi.lib` | `A52` | `.text + 0x0` | 12 | 4 | 0 |
| `write` | `lib/libapi.lib` | `A53` | `.text + 0x0` | 12 | 4 | 0 |
| `close` | `lib/libapi.lib` | `A54` | `.text + 0x0` | 12 | 4 | 0 |
| `Krom2RawAdd` | `lib/libapi.lib` | `A81` | `.text + 0x0` | 12 | 4 | 0 |
| `GPU_cw` | `lib/libapi.lib` | `C73` | `.text + 0x0` | 12 | 4 | 0 |
| `func_8008FE08` (no exported name) | `lib/libgte.lib` | `MSC00` | `.text + 0x0` | 8 | 0 | 0 |
| `InitGeom` | `lib/libgte.lib` | `MSC00` | `.text + 0x8` | 128 | 8 | 5 |
| `SquareRoot0` | `lib/libgte.lib` | `MSC01` | `.text + 0x0` | 132 | 12 | 2 |
| `SetColorMatrix` | `lib/libgte.lib` | `MTX_11` | `.text + 0x0` | 48 | 0 | 0 |
| `SetFarColor` | `lib/libgte.lib` | `REG11` | `.text + 0x0` | 32 | 0 | 0 |
| `SetGeomOffset` | `lib/libgte.lib` | `REG12` | `.text + 0x0` | 24 | 8 | 0 |
| `_patch_gte` | `lib/libgte.lib` | `PATCHGTE` | `.text + 0x0` | 172 | 0 | 15 |
| `func_80090044` (no exported name) | `lib/libgte.lib` | `PATCHGTE` | `.text + 0xAC` | 48 | 4 | 0 |
| `DeliverEvent` | `lib/libapi.lib` | `A07` | `.text + 0x0` | 12 | 4 | 0 |
| `ChangeClearPAD` | `lib/libapi.lib` | `A91` | `.text + 0x0` | 12 | 4 | 0 |
| `ChangeClearRCnt` | `lib/libapi.lib` | `L10` | `.text + 0x0` | 12 | 4 | 0 |
| `func_80095298` (no exported name) | `lib/libapi.lib` | `C114` | `.text + 0x0` | 8 | 0 | 0 |
| `_96_remove` | `lib/libapi.lib` | `C114` | `.text + 0x8` | 12 | 12 | 0 |
| `ReturnFromException` | `lib/libapi.lib` | `A23` | `.text + 0x0` | 12 | 4 | 0 |
| `ResetEntryInt` | `lib/libapi.lib` | `A24` | `.text + 0x0` | 12 | 4 | 0 |
| `HookEntryInt` | `lib/libapi.lib` | `A25` | `.text + 0x0` | 12 | 4 | 0 |
| `setjmp` | `lib/libc2.lib` | `SETJMP` | `.text + 0x0` | 60 | 0 | 0 |
| `longjmp` | `lib/libc2.lib` | `SETJMP` | `.text + 0x3C` | 60 | 8 | 0 |
| `OpenEvent` | `lib/libapi.lib` | `A08` | `.text + 0x0` | 12 | 4 | 0 |
| `EnableEvent` | `lib/libapi.lib` | `A12` | `.text + 0x0` | 12 | 4 | 0 |
| `WaitEvent` | `lib/libapi.lib` | `A10` | `.text + 0x0` | 12 | 4 | 0 |
| `TestEvent` | `lib/libapi.lib` | `A11` | `.text + 0x0` | 12 | 4 | 0 |
| `InitPAD2` | `lib/libapi.lib` | `A18` | `.text + 0x0` | 12 | 4 | 0 |
| `StartPAD2` | `lib/libapi.lib` | `A19` | `.text + 0x0` | 12 | 4 | 0 |
| `StopPAD2` | `lib/libapi.lib` | `A20` | `.text + 0x0` | 12 | 4 | 0 |
| `PAD_init2` | `lib/libapi.lib` | `A21` | `.text + 0x0` | 12 | 4 | 0 |
| `SysEnqIntRP` | `lib/libapi.lib` | `L02` | `.text + 0x0` | 12 | 4 | 0 |
| `SysDeqIntRP` | `lib/libapi.lib` | `L03` | `.text + 0x0` | 12 | 4 | 0 |
| `EnablePAD` | `lib/libapi.lib` | `PATCH` | `.text + 0x0` | 20 | 0 | 2 |
| `DisablePAD` | `lib/libapi.lib` | `PATCH` | `.text + 0x14` | 20 | 0 | 2 |
| `_patch_pad` | `lib/libapi.lib` | `PATCH` | `.text + 0x28` | 112 | 8 | 10 |
| `_remove_ChgclrPAD` | `lib/libapi.lib` | `CHCLRPAD` | `.text + 0x0` | 104 | 8 | 7 |
| `InitCARD2` | `lib/libcard.lib` | `A74` | `.text + 0x0` | 12 | 4 | 0 |
| `StartCARD2` | `lib/libcard.lib` | `A75` | `.text + 0x0` | 12 | 4 | 0 |
| `StopCARD2` | `lib/libcard.lib` | `A76` | `.text + 0x0` | 12 | 4 | 0 |
| `_patch_card_info` | `lib/libcard.lib` | `PATCH` | `.text + 0x0` | 68 | 0 | 5 |
| `func_800A169C` (no exported name) | `lib/libcard.lib` | `PATCH` | `.text + 0x44` | 44 | 0 | 0 |
| `func_800A16C8` (no exported name) | `lib/libcard.lib` | `PATCH` | `.text + 0x70` | 68 | 0 | 0 |
| `func_800A170C` (no exported name) | `lib/libcard.lib` | `PATCH` | `.text + 0xB4` | 36 | 4 | 0 |
| `_patch_card` | `lib/libcard.lib` | `PATCH` | `.text + 0xDC` | 148 | 0 | 10 |
| `_patch_card2` | `lib/libcard.lib` | `PATCH` | `.text + 0x170` | 112 | 0 | 10 |
| `_copy_memcard_patch` | `lib/libcard.lib` | `PATCH` | `.text + 0x1E0` | 52 | 12 | 4 |
| `_ExitCard` | `lib/libcard.lib` | `END` | `.text + 0x0` | 112 | 16 | 11 |
| `nextfile` | `lib/libapi.lib` | `A67` | `.text + 0x0` | 12 | 4 | 0 |
| `erase` | `lib/libapi.lib` | `A69` | `.text + 0x0` | 12 | 4 | 0 |
| `firstfile2` | `lib/libapi.lib` | `A66` | `.text + 0x0` | 12 | 4 | 0 |
| `_card_info` | `lib/libcard.lib` | `C171` | `.text + 0x0` | 12 | 4 | 0 |
| `_card_load` | `lib/libcard.lib` | `C172` | `.text + 0x0` | 12 | 4 | 0 |
| `_card_write` | `lib/libcard.lib` | `A78` | `.text + 0x0` | 12 | 4 | 0 |
| `_new_card` | `lib/libcard.lib` | `A80` | `.text + 0x0` | 12 | 4 | 0 |
| `CloseEvent` | `lib/libapi.lib` | `A09` | `.text + 0x0` | 12 | 4 | 0 |
| `_card_read` | `lib/libcard.lib` | `A79` | `.text + 0x0` | 12 | 4 | 0 |

65 routines, 2,304 bytes (2,083 read from Sony's files, 221 in 101 fields left to the linker), and 252 bytes of padding.
