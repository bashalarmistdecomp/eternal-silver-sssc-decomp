# Progress

How much of each program is decompiled, measured by `make status` on 2026-10-20
from the three default builds, each of which matched its original byte for
byte. `make status` writes this whole page ([recipe/status.py](../recipe/status.py)
says how each figure is counted); edit that script, not the page.

## What counts

A program's functions are the ones its linked image defines from C plus the
routines still read from the original as byte ranges. Two kinds of routine
among those are not open work:

- **Assembly in the original program.** No compiler produced it, so there is
  no C to recover. [spec/code/hand-assembly.md](spec/code/hand-assembly.md) has
  the rule and the evidence each such routine needs.
- **Copies of Sony's library code.** A routine whose bytes are, byte for byte,
  an object of Sony's PSY-Q libraries is Sony's compiled code. Converting it
  is optional. Its bytes could come from Sony's own library files instead of
  the disc, as `make CODE=sdk` does for the main program's assembly; no
  program's copies are read that way yet.

Each share is counted two ways, by functions and by code bytes (the bytes
the linker placed, which the C compiles to or the build reads from the
original), and out of two wholes: *all* of a program's code, and the code
*to decompile*, all of it but those two kinds. Bytes show how much is left
by size, since one function can be a thousand times the size of another.

## The programs

### By functions

| Program | Functions | To decompile | In C | Share of all | Share of those to decompile |
|---|---:|---:|---:|---:|---:|
| Main executable (`SLUS_006.28`) | 2,318 | 2,253 | 2,253 | 97.20% | 100.00% |
| Overlay (`SLUS_006.28-MENU`) | 36 | 34 | 33 | 91.67% | 97.06% |
| Bonus disc (`SLUS_009.21`) | 404 | 146 | 139 | 34.41% | 95.21% |
| All three | 2,758 | 2,433 | 2,425 | 87.93% | 99.67% |

### By code bytes

| Program | Code bytes | To decompile | In C | Share of all | Share of those to decompile |
|---|---:|---:|---:|---:|---:|
| Main executable (`SLUS_006.28`) | 606,540 | 603,984 | 603,984 | 99.58% | 100.00% |
| Overlay (`SLUS_006.28-MENU`) | 5,508 | 4,624 | 4,380 | 79.52% | 94.72% |
| Bonus disc (`SLUS_009.21`) | 143,808 | 75,188 | 11,672 | 8.12% | 15.52% |
| All three | 755,856 | 683,796 | 620,036 | 82.03% | 90.68% |

## What is left

Functions, with their code bytes in brackets.

| Program | Still assembly | Assembly in the original | Copies of Sony's code | Open work |
|---|---:|---:|---:|---:|
| Main executable | 65 (2,556) | 65 (2,556) | 0 (0) | 0 (0) |
| Overlay | 3 (1,128) | 2 (884) | 0 (0) | 1 (244) |
| Bonus disc | 265 (132,136) | 33 (2,144) | 225 (66,476) | 7 (63,516) |

- **Main executable.** 65 functions are still assembly, and none is open work:
  all 65 were assembly in the original program
  (`decomp/SLUS_006.28/config/asm_only.txt` lists each with its evidence).
- **Overlay.** 3 functions are still assembly. 1 is open work:
  `movie_slice_load` (244 bytes); 2 were assembly in the original program
  (`decomp/SLUS_006.28-MENU/config/asm_only.txt` lists each with its
  evidence).
- **Bonus disc.** 265 functions are still assembly. 7 are open work:
  `CastleGameRun` (34,928 bytes), `CastleOptionsRun` (21,028 bytes),
  `CastlePadStep` (2,788 bytes), `CastleTitleRun` (2,612 bytes), `movie_play`
  (1,376 bytes), `pad_buffer_decode` (624 bytes), `movie_run` (160 bytes); 33
  were assembly in the original program
  (`decomp/SLUS_009.21/config/asm_only.txt` lists each with its evidence); 225
  are Sony's library code, byte for byte, that Sony compiled
  (`decomp/SLUS_009.21/config/vendor_provided.txt` names the library member
  each lies in).

## The whole image

Beside its code, an image holds jump tables and data. This repository has no
initialised data in C: the build reads every data byte from the image, so an
image's share from C stays well below its code's.

| Program | Image bytes | From C | Share | Routines read as bytes | Jump tables | Data |
|---|---:|---:|---:|---:|---:|---:|
| Main executable | 688,128 | 610,422 | 88.71% | 2,556 | 220 | 74,930 |
| Overlay | 76,096 | 4,399 | 5.78% | 1,128 | 0 | 70,569 |
| Bonus disc | 1,202,176 | 11,943 | 0.99% | 132,136 | 0 | 1,058,097 |

## History

Each program's four shares, on each day `make status` found its numbers
changed ([progress-history.csv](progress-history.csv) has every count).
The decompilation's earlier course is in [HISTORY.md](../HISTORY.md). Until
2026-10-13 the bonus disc's open work was counted as 144 functions: the
comparison had been made against releases 4.6 and 4.7 of Sony's libraries,
and the disc links 4.5.

| Date | Program | All functions | Functions to decompile | All code bytes | Code bytes to decompile |
|---|---|---:|---:|---:|---:|
| 2026-10-20 | Main executable | 97.20% | 100.00% | 99.58% | 100.00% |
| 2026-10-20 | Overlay | 91.67% | 97.06% | 79.52% | 94.72% |
| 2026-10-20 | Bonus disc | 34.41% | 95.21% | 8.12% | 15.52% |
