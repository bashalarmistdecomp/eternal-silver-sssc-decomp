#!/usr/bin/env python3
"""Restore the INT_MIN guard GCC 9 compiled out of our GCC 2.8.1 cc1.

Usage: python3 recipe/patch_cc1.py            # status (exit 1 = unpatched)
       python3 recipe/patch_cc1.py --apply    # patch in place, keep a backup
       ... [--cc1 PATH]   # act on another copy (e.g. to prove the patch first)

recipe/toolchain.sh runs it; there is no need to run it by hand.

Why (2026-10-07): `x + 0x80000000` compiled to
`lui; subu` where the original has `lui; addu` — the "subu wall" that
held ten functions. GCC 2.8.1's cse.c (fold_rtx, case PLUS) rewrites
`x + reg` into `x - reg'` when reg holds a negative constant and reg'
the negated one, guarded so INT_MIN (its own negation) is left alone:

    && INTVAL (const_arg1) < 0
    && - INTVAL (const_arg1) >= 0      /* false only for INT_MIN */

decompals/old-gcc built our cc1 with GCC 9, which may assume signed
negation never overflows and folded the guard to `INTVAL <= 0` —
always true here — so INT_MIN gets rewritten. Sony's own CC1PSX
(PSY-Q 4.3 and 4.4, run under wine) emits `addu` on the same input,
and the retail image has `lui 0x8000` + `addu` 15 times, `subu` never.

The patch swaps the folded test for the one the source means, at the
same length and with the same branch target:

    test %eax,%eax ; jg  skip     ->     neg %eax ; js skip

(`neg` of INT_MIN stays negative, so INT_MIN skips; every other
negative value negates to positive and proceeds, as before.) It changes
cc1's output ONLY for an add of INT_MIN held in a register.

GCC 2.7.2's cse.c has no such guard at all, so its cc1 rewrites INT_MIN
in the original too — gcc-2.7.2-psx is faithful as shipped and is not
patched. A cc1 whose sha256 is not one listed below is refused.
"""
import hashlib
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import _paths

CC1 = _paths.ROOT / _paths.TOOLCHAIN / "gcc-2.8.1-psx/cc1"

#: decompals/old-gcc release 0.17 gcc-2.8.1-psx.tar.gz's cc1.
#: offset = file offset of `test %eax,%eax` (vaddr 0x0811b153, in fold_rtx)
KNOWN = {
    "1a48ec5ac87a146a074a9d66d7a67734a4d1ad56553d02d91e8b71c7fecc2d52": {
        "offset": 0xD3153,
        "old": bytes.fromhex("85c00f8f88010000"),   # test eax,eax; jg +0x188
        "new": bytes.fromhex("f7d80f8888010000"),   # neg eax;      js +0x188
    },
}

PROBE = ("typedef unsigned int u32;\n"
         "u32 patch_cc1_probe(u32 p) { return p + 0x80000000; }\n")


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def state():
    """('original'|'patched'|'unknown'|'missing', sha256, KNOWN entry).
    'patched' is recognised by undoing the patch and finding a KNOWN sha,
    so no patched checksum is ever typed in by hand."""
    if not CC1.exists():
        return "missing", None, None
    data = CC1.read_bytes()
    sha = hashlib.sha256(data).hexdigest()
    if sha in KNOWN:
        return "original", sha, KNOWN[sha]
    for orig, p in KNOWN.items():
        o, n = p["offset"], len(p["new"])
        if data[o:o + n] == p["new"]:
            undone = bytearray(data)
            undone[o:o + n] = p["old"]
            if hashlib.sha256(undone).hexdigest() == orig:
                return "patched", sha, p
    return "unknown", sha, None


def probe():
    """Compile `p + 0x80000000` with the cc1 on disk: 'addu' or 'subu'."""
    with tempfile.TemporaryDirectory() as td:
        src, out = os.path.join(td, "probe.c"), os.path.join(td, "probe.s")
        with open(src, "w") as f:
            f.write(PROBE)
        r = subprocess.run([str(CC1), "-quiet", "-O2", "-G8", src,
                            "-o", out], capture_output=True, text=True)
        if r.returncode:
            return "cc1 failed: " + r.stderr.strip()
        with open(out) as f:
            asm = f.read()
    if "\taddu\t" in asm and "\tsubu\t" not in asm:
        return "addu"
    if "\tsubu\t" in asm:
        return "subu"
    return "unrecognised output"


def apply():
    st, sha, p = state()
    if st == "patched":
        print(f"already patched: {CC1} ({sha[:12]})")
        return 0
    if st != "original":
        print(f"refusing: {CC1} is {st} (sha256 {sha}) — not a build "
              f"this tool knows; see the docstring", file=sys.stderr)
        return 2
    o, n = p["offset"], len(p["old"])
    data = bytearray(CC1.read_bytes())
    if data[o:o + n] != p["old"]:
        print(f"refusing: bytes at {o:#x} are {data[o:o + n].hex()}, "
              f"expected {p['old'].hex()}", file=sys.stderr)
        return 2
    backup = CC1.with_name("cc1.unpatched")
    if not backup.exists():
        shutil.copy2(CC1, backup)
    data[o:o + n] = p["new"]
    tmp = CC1.with_name("cc1.patching")
    tmp.write_bytes(bytes(data))
    shutil.copymode(CC1, tmp)
    os.replace(tmp, CC1)
    got = probe()
    if got != "addu":
        shutil.copy2(backup, CC1)
        print(f"probe after patching gave {got!r}; restored the backup",
              file=sys.stderr)
        return 2
    print(f"patched {CC1}: {sha[:12]} -> {sha256(CC1)[:12]} "
          f"(backup {backup.name}); probe: p + 0x80000000 -> addu")
    return 0


def main(argv):
    global CC1
    if "-h" in argv or "--help" in argv:
        print(__doc__)
        return 0
    if "--cc1" in argv:
        CC1 = Path(argv[argv.index("--cc1") + 1]).resolve()
    if "--apply" in argv:
        return apply()
    st, sha, _ = state()
    print(f"{CC1}: {st}" + (f" (sha256 {sha[:12]})" if sha else ""))
    if st == "original":
        print("  unpatched: x + 0x80000000 compiles to subu — run "
              "patch_cc1.py --apply")
    return 0 if st == "patched" else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
