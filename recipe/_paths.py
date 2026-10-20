"""Build paths. The ONLY file under recipe/ that spells the decomp tree,
build/ or the toolchain — every other script imports from here.

  decomp/<target>/     THE PROGRAM BEING MATCHED. src/ asm/ include/
                       config/ expected/ splat.yaml — every byte of it
                       describes ONE executable image, so it is keyed by
                       that program's serial-form name.
  build/<target>/      EPHEMERAL. make output; mirrors Makefile BUILD_DIR.
  toolchain/           NOT IN GIT. The compilers, the assembler and Sony's
                       SDK headers, put there by recipe/toolchain.sh.

DEFAULT_TARGET is the single declaration of WHICH PROGRAM `make` builds by
default; the Makefile's SERIAL fallback must agree with it, because a
disagreement does not fail loudly — it splits build output in two, or
reads a tree nothing is building.

TARGET_SERIAL in the environment retargets make and these scripts at
another program in step: the Makefile reads the same variable.
"""

from __future__ import annotations

import os
import re
from pathlib import Path

ROOT: Path = Path(__file__).resolve().parents[1]

#: The program this tree builds BY DEFAULT, named by its serial-form EXE
#: name — Sony names a program after its boot file.
DEFAULT_TARGET: str = "SLUS_006.28"

_ENV_TARGET = os.environ.get("TARGET_SERIAL")
if _ENV_TARGET is not None and not _ENV_TARGET.strip():
    # SET BUT EMPTY is a mistake, never an intent: `TARGET_SERIAL=$S ...`
    # with $S unset would otherwise answer confidently about the default
    # program. In a tree where every path is serial-keyed, silently
    # retargeting is worse than dying.
    raise SystemExit(
        "TARGET_SERIAL is set but EMPTY — an unset shell variable?\n"
        "  Pass the program serial explicitly (TARGET_SERIAL=SLUS_009.21 …),\n"
        f"  or unset it entirely to use the default ({DEFAULT_TARGET}).")
TARGET: str = _ENV_TARGET or DEFAULT_TARGET

# An override naming a tree we do not hold must die HERE, at import: a
# typo'd serial makes every glob come back empty, which reads as "nothing
# to build" rather than as an error.
if not (ROOT / "decomp" / TARGET / "splat.yaml").is_file():
    raise SystemExit(
        f"TARGET_SERIAL={TARGET}: decomp/{TARGET}/splat.yaml does not exist "
        f"— pass a PROGRAM serial this repository holds "
        f"({', '.join(sorted(p.name for p in (ROOT / 'decomp').iterdir() if p.is_dir()))})")

# -- the decomp tree (one PROGRAM, one subtree) --------------------------
DECOMP: Path = ROOT / "decomp" / TARGET
DECOMP_SRC: Path = DECOMP / "src"
DECOMP_ASM: Path = DECOMP / "asm"
DECOMP_INCLUDE: Path = DECOMP / "include"
DECOMP_CONFIG: Path = DECOMP / "config"
DECOMP_EXPECTED: Path = DECOMP / "expected" / "code.bin"
SPLAT_YAML: Path = DECOMP / "splat.yaml"
SYMBOL_ADDRS: Path = DECOMP_CONFIG / "symbol_addrs.txt"
NONMATCHINGS: Path = DECOMP_ASM / "nonmatchings"
PROGRAM_LD: Path = DECOMP_CONFIG / "program.ld"


def _splat_option(key: str) -> int:
    """One `key: 0x…` option out of splat.yaml, without a YAML dependency.

    The two numbers below are PROGRAM facts, not script constants.
    splat.yaml declares both, so it is the source and this is the reader.

    There is deliberately NO fallback. A guessed load address does not
    fail — it silently slices the image at the wrong offset and every
    address reported is off by a constant.
    """
    for line in SPLAT_YAML.read_text().splitlines():
        m = re.match(rf"\s*{re.escape(key)}:\s*(0x[0-9A-Fa-f]+)", line)
        if m:
            return int(m.group(1), 16)
    raise SystemExit(f"{SPLAT_YAML} declares no `{key}:` — the build "
                     f"cannot place this program's image without it")


#: Where this program's code image is loaded — the first code segment's
#: `vram`, i.e. `file offset + VRAM_BASE == address`.
VRAM_BASE: int = _splat_option("vram")

#: This program's `_gp`. Every gp-window decision (which externs may be
#: %gp_rel) is measured against it.
GP_VALUE: int = _splat_option("gp_value")

# -- ephemeral -----------------------------------------------------------
#: make exports BUILD_DIR: a build that takes some routines from another
#: source than the disc (CODE=sdk) writes to a folder of its own.
BUILD: Path = ROOT / (os.environ.get("BUILD_DIR") or f"build/{TARGET}")

#: Object paths AS THEY APPEAR IN program.ld — repo-RELATIVE strings, not
#: Paths: `make` links from the repo root, so the linker script must stay
#: relative or a moved checkout stops linking.
OBJ_SRC: str = f"build/{TARGET}/src"
OBJ_ASM: str = f"build/{TARGET}/asm"

# -- the toolchain (not in git; recipe/toolchain.sh puts it there) -------
#: Repo-relative by default, like the Makefile's CROSS: make runs from the
#: repo root. TOOLCHAIN_DIR points both at a toolchain kept elsewhere.
TOOLCHAIN: str = os.environ.get("TOOLCHAIN_DIR") or "toolchain"

#: Sony's PSY-Q headers (release 4.7, in a directory that keeps its source's
#: folder name, 4.4), which psyq_types.h includes by name. They are
#: Sony's and are not in this repository; PSYQ_INCLUDE names a copy of
#: your own instead of the one toolchain.sh fetches.
PSYQ_INCLUDE: str = os.environ.get("PSYQ_INCLUDE") or f"{TOOLCHAIN}/psyq/4.4"
