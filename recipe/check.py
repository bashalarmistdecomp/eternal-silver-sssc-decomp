#!/usr/bin/env python3
"""Byte-compare the built binary against the original EXE code region."""
import hashlib
import re
import sys
import _paths

VRAM_BASE = _paths.VRAM_BASE

_SEC = re.compile(r"^\s+\.\w[\w.]*\s+0x0*([0-9a-f]+)\s+0x([0-9a-f]+)"
                  r"\s+(\S+\.o)\b")


def owner_of(vram, map_path=str(_paths.BUILD / "program.map")):
    """The object (and its .c) whose section contains `vram` — the
    one-line answer to "whose build broke": a red make used to mean
    grepping program.map by hand to learn the first diff was the OTHER
    session's segment. -> str(_paths.BUILD / "<target>/src/X.o (decomp/src/X.c)") or None."""
    try:
        best = None
        for line in open(map_path):
            m = _SEC.match(line)
            if not m:
                continue
            lo = int(m.group(1), 16)
            hi = lo + int(m.group(2), 16)
            if lo <= vram < hi:
                best = m.group(3)
        if best is None:
            return None
        m = re.search(r"([\w.]+?)(?:\.c)?\.o$", best)
        return f"{_paths.DECOMP_SRC}/{m.group(1)}.c" if m else best
    except OSError:
        return None


def main():
    built_path, target_path = sys.argv[1], sys.argv[2]
    with open(built_path, "rb") as f:
        built = f.read()
    with open(target_path, "rb") as f:
        target = f.read()

    if built == target:
        print(f"OK: {len(built):#x} bytes, "
              f"sha1 {hashlib.sha1(built).hexdigest()}")
        sys.exit(0)

    print(f"MISMATCH: built {len(built):#x} bytes, "
          f"target {len(target):#x} bytes")
    n = min(len(built), len(target))
    diffs = [i for i in range(n) if built[i] != target[i]]
    print(f"{len(diffs)} differing bytes in overlapping range")
    if diffs:
        first = diffs[0]
        last = diffs[-1]
        print(f"first diff at offset {first:#x} "
              f"(VRAM {VRAM_BASE + first:#x}), "
              f"last at {last:#x} (VRAM {VRAM_BASE + last:#x})")
        who = owner_of(VRAM_BASE + first)
        if who:
            print(f"first diff lies in {who} — if a SHIFT is detected "
                  f"below, the culprit is instead whichever object "
                  f"CHANGED SIZE (the file you or a parallel session "
                  f"just touched), not this one")

        # A section-size overrun shifts EVERYTHING after it, so the
        # first-diff address points ~anywhere downstream of the real edit
        # (measured: a 4-byte .text overrun in battle.c reported its
        # first diff 0xa5000 bytes earlier, in data). Detect a constant
        # shift and say so.
        WIN = 4096
        probe = first + 256  # clear of the local damage at the origin
        if probe + WIN < n:
            for delta in (4, 8, 12, 16, -4, -8, -12, -16):
                lo, hi = probe + max(0, delta), probe + WIN + min(0, delta)
                if built[lo - delta:hi - delta] == target[lo:hi]:
                    grew = -delta
                    word = "GREW" if grew > 0 else "SHRANK"
                    print(f"SHIFT DETECTED: from ~{probe:#x} on, built "
                          f"matches target displaced by {abs(grew):#x} "
                          f"bytes — a section upstream {word} by "
                          f"{abs(grew):#x} (usually .text of the segment "
                          f"you just edited). The first-diff address "
                          f"above is NOT where the problem is: check "
                          f"`build/<serial>/program.map` for the size of the object "
                          f"you touched.")
                    break
        start = first & ~0xF
        for row in range(start, min(start + 64, n), 16):
            print(f"  {row:06x} built  {built[row:row + 16].hex(' ')}")
            print(f"         target {target[row:row + 16].hex(' ')}")
    sys.exit(1)


if __name__ == "__main__":
    main()
