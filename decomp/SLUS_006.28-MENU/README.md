# MENU.BIN — SLUS_006.28's movie-player overlay

The boot EXE (`decomp/SLUS_006.28/`) loads `MENU.BIN` whole to
`0x80196000` before every movie (`overlay_load`) and calls
`movie_play(n)` (`0x80196B54`). That code is a second executable image, so it has a
tree of its own, keyed `SLUS_006.28-MENU`; one variable aims make at it:

```
make TARGET_SERIAL=SLUS_006.28-MENU
    OK: 0x12940 bytes, sha1 166b669ceacf42fa09dda45a8c5974e2a0f28fe9
```

`expected/code.bin` is the disc file itself (no PS-X EXE header, nothing
to unpack) and, like every tree's, is not in git. `recipe/extract.py`
makes it from either game disc; the two discs' copies are byte-identical.

## What is here

| segment | range | contents |
|---|---|---|
| `head_rodata` (data) | `0x80196000`–`0x8019610C` | build stamp + format strings |
| `movie` | `0x8019610C`–`0x80196BDC` | the game's movie player, 17 functions |
| `libpress` | `0x80196BDC`–`0x8019731C` | Sony libpress's `libpress` object, 17 functions |
| `vlc` | `0x8019731C`–`0x80197690` | Sony libpress's `vlc` object, 2 functions, assembly in the original (`docs/spec/code/subsystems/overlay.md`) |
| `tail_data` (data) | `0x80197690`–`0x801A8940` | end-frame table, library tables, the 64 KB VLC table, variables |

- **Compiled `-G0`.** No instruction in the image names `$gp`; the boot
  EXE's globals it reads are `%hi/%lo` here. `config/cc_flags.txt` passes
  `-G0` per file.
- **The boot EXE's symbols** the overlay calls and reads are listed in
  `config/symbol_addrs.txt` (their lines say `boot-exe`), named as the
  boot EXE's tree names them, and defined for the link by
  `config/label_fixups.txt`.
- **The libpress objects are release 4.1's**, older than the 4.5 the boot
  EXE links. Both members fit 4.1 of the PSY-Q signature set and no other
  release; no 4.1 library file has been compared with them, so this rests
  on the signatures alone (`docs/naming.md`, "Which release of Sony's
  libraries the programs link"). The `libpress` object's ten exported
  functions (`DecDCTReset` … `DecDCToutCallback`) and the `vlc` object's
  two carry Sony's names by those bytes; three of the static helpers print
  their own names (`MDEC_rest`, `MDEC_in_sync`, `MDEC_out_sync`) and carry
  them; the other four keep their addresses.
- Function map, the split corrections and what the player does:
  `docs/spec/code/subsystems/overlay.md` and `docs/spec/video.md`.
