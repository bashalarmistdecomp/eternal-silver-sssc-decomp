#!/usr/bin/env python3
"""Say where the bytes of a build came from.

    python3 recipe/sources.py <build folder> <decomp tree>      (make sources)

What is C comes from this repository. Everything else is a byte range some
stub names, and each stub the assembler read is listed in the build's
dependency files. This adds those ranges up by what they are (a routine, a
jump table, data) and by where they were read from (your disc's program
image, Sony's SDK libraries), plus the padding that needs no source and the
fields the linker fills (a stub that reads a .LIB file leaves those blank).
"""
import collections
import re
import sys
from pathlib import Path

INCBIN = re.compile(r'^\s*\.incbin\s+"([^"]+)",\s*0x([0-9A-Fa-f]+),\s*0x([0-9A-Fa-f]+)', re.M)
SPACE = re.compile(r'^\s*\.space\s+0x([0-9A-Fa-f]+)', re.M)
BLANK = re.compile(r'^\s*\.byte\s+(0(?:\s*,\s*0)*)\s*$', re.M)


def main():
    build, tree = Path(sys.argv[1]), Path(sys.argv[2])
    deps = list(build.rglob("*.d"))
    if not deps:
        sys.exit(f"{build}: no build there yet")
    stubs = set()
    for d in deps:
        for word in d.read_text().replace("\\\n", " ").split():
            p = Path(word.rstrip(":"))
            if p.suffix == ".s" and tree in p.parents and "asm" in p.parts:
                stubs.add(p)
    total = collections.Counter()
    files = collections.Counter()
    for p in sorted(stubs):
        kind = ("routines" if "nonmatchings" in p.parts
                else "jump tables" if "jtbl" in p.parts else "data")
        text = p.read_text(encoding="latin-1")
        seen = set()
        for target, _off, length in INCBIN.findall(text):
            src = "your disc" if target == "expected/code.bin" else "Sony's SDK"
            total[kind, src] += int(length, 16)
            seen.add(src)
        for length in SPACE.findall(text):
            total[kind, "padding"] += int(length, 16)
        for zeros in BLANK.findall(text):
            total[kind, "linker"] += zeros.count("0")
        for src in seen:
            files[kind, src] += 1
    print(f"{build}: bytes that are not compiled from C, by source")
    for kind in ("routines", "jump tables", "data"):
        parts = [f"{total[kind, src]:>8,} from {src} ({files[kind, src]} files)"
                 for src in ("your disc", "Sony's SDK") if total[kind, src]]
        if total[kind, "padding"]:
            parts.append(f"{total[kind, 'padding']:,} of zero padding")
        if total[kind, "linker"]:
            parts.append(f"{total[kind, 'linker']:,} filled in by the linker")
        print(f"  {kind:12} " + "; ".join(parts or ["none"]))


if __name__ == "__main__":
    main()
