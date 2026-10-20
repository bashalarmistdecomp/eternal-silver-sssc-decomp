#!/usr/bin/env python3
"""Make decomp/<serial>/expected/code.bin from your own disc images.

    python3 recipe/extract.py <disc image, .cue, or folder> [more...]
    python3 recipe/extract.py --check      # only verify what is already there

`expected/code.bin` is the program image a tree is matched against: the
build links to it byte for byte, and what is still assembly is assembled
from byte ranges of it. It is the game's code, so it is never in this
repository; this script cuts it out of a disc you own.

Nothing here is keyed by a disc's name or serial. Each image is opened,
its boot file and MENU.BIN are read, and a result is kept only when its
sha1 is the one a tree's splat.yaml declares — so a wrong disc, another
region's pressing or a bad dump writes nothing at all.

What each tree's image is:

  SLUS_006.28        The boot file of either game disc is a small stub
                     followed by an LZSS stream (file offset 0x1000). The
                     stream unpacks to a second PS-X EXE; the image is that
                     EXE's body.
  SLUS_006.28-MENU   MENU.BIN from either game disc, whole. It has no
                     header: the game loads it as it is.
  SLUS_009.21        The bonus disc's boot file is a plain PS-X EXE; the
                     image is its body.

Reads raw 2352-byte images (the usual .bin of a .bin/.cue pair) and
2336- or 2048-byte ones. Needs nothing but Python 3.
"""
from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

SECTOR = 2048                     # user data per sector
PSX_EXE_MAGIC = b"PS-X EXE"
PSX_HEADER_SIZE = 0x800           # the container header is one sector
PACKED_PAYLOAD_OFFSET = 0x1000    # SLUS_006.28's LZSS stream, in its boot file

#: (bytes per stored sector, offset of the 2048 user bytes inside one)
LAYOUTS = ((2352, 24), (2352, 16), (2336, 8), (2048, 0))


class Disc:
    """An ISO 9660 disc image, read by seeking: only a few sectors are
    wanted out of some 700 MB."""

    def __init__(self, path: Path):
        self.path = path
        self._fh = path.open("rb")
        for size, skip in LAYOUTS:
            self._size, self._skip = size, skip
            if self.sector(16)[1:6] == b"CD001":
                break
        else:
            self._fh.close()
            raise ValueError("no ISO 9660 volume descriptor at sector 16")
        pvd = self.sector(16)
        root = pvd[156:190]
        self._root = (struct.unpack_from("<I", root, 2)[0],
                      struct.unpack_from("<I", root, 10)[0])

    def sector(self, lba: int) -> bytes:
        self._fh.seek(lba * self._size + self._skip)
        return self._fh.read(SECTOR).ljust(SECTOR, b"\x00")

    def read(self, lba: int, size: int) -> bytes:
        count = (size + SECTOR - 1) // SECTOR
        return b"".join(self.sector(lba + i) for i in range(count))[:size]

    def root_files(self) -> dict[str, tuple[int, int]]:
        """NAME -> (lba, size) for the files of the root directory."""
        lba, size = self._root
        data = self.read(lba, size)
        out, pos = {}, 0
        while pos < len(data):
            rec_len = data[pos]
            if rec_len == 0:                       # records do not span sectors
                pos = (pos // SECTOR + 1) * SECTOR
                continue
            rec = data[pos:pos + rec_len]
            pos += rec_len
            if len(rec) < 34 or rec[25] & 0x02:    # short, or a directory
                continue
            name = rec[33:33 + rec[32]].decode("ascii", "replace")
            out[name.split(";")[0].upper()] = (
                struct.unpack_from("<I", rec, 2)[0],
                struct.unpack_from("<I", rec, 10)[0])
        return out

    def close(self):
        self._fh.close()


def lzss_unpack(src: bytes) -> bytes:
    """SLUS_006.28's unpacker, as the boot stub runs it.

    Control bits are read LSB first, eight to a byte:
      1      literal: one byte copied from the input
      0 1    short reference, one byte: top 2 bits = length - 2, low 6 bits
             = a negative distance
      0 0    long reference, two bytes big-endian: top 4 bits = length - 2,
             low 12 bits = a negative distance. A zero length field means
             the length is the next byte, plus 2 — and a zero there ends
             the stream.
    A reference reaching before the start of the output reads zeros.
    """
    out = bytearray()
    si = flags = bits = 0

    def bit() -> int:
        nonlocal si, flags, bits
        if bits == 0:
            flags, si, bits = src[si], si + 1, 8
        b = flags & 1
        flags >>= 1
        bits -= 1
        return b

    while si < len(src):
        if bit():
            out.append(src[si])
            si += 1
            continue
        if bit():
            b = src[si]
            si += 1
            length, dist = (b >> 6) + 2, (b & 0x3F) - 0x40
        else:
            word = (src[si] << 8) | src[si + 1]
            si += 2
            length, dist = word >> 12, (word & 0xFFF) - 0x1000
            if length == 0:
                length = src[si]
                si += 1
                if length == 0:
                    break
            length += 2
        pos = len(out) + dist
        for _ in range(length):
            out.append(out[pos] if 0 <= pos < len(out) else 0)
            pos += 1
    return bytes(out)


def exe_body(exe: bytes) -> bytes | None:
    """The body of a PS-X EXE (what its header loads at t_addr), or None."""
    if exe[:8] != PSX_EXE_MAGIC or len(exe) < PSX_HEADER_SIZE:
        return None
    t_size = struct.unpack_from("<I", exe, 0x1C)[0]
    body = exe[PSX_HEADER_SIZE:PSX_HEADER_SIZE + t_size]
    return body if len(body) == t_size else None


def candidates(disc: Disc):
    """(what it is, bytes) for everything on the disc that could be a
    tree's image."""
    files = disc.root_files()
    boot = None
    if "SYSTEM.CNF" in files:
        cnf = disc.read(*files["SYSTEM.CNF"]).decode("ascii", "replace")
        m = re.search(r"BOOT\s*=\s*cdrom:\\?([^;\s]+)", cnf, re.I)
        if m:
            boot = m.group(1).upper()
    if boot in files:
        exe = disc.read(*files[boot])
        body = exe_body(exe)
        if body is not None:
            yield f"{boot}, body", body
        if exe[:8] == PSX_EXE_MAGIC and len(exe) > PACKED_PAYLOAD_OFFSET:
            try:
                inner = exe_body(lzss_unpack(exe[PACKED_PAYLOAD_OFFSET:]))
            except IndexError:                     # not a packed boot file
                inner = None
            if inner is not None:
                yield f"{boot}, unpacked body", inner
    if "MENU.BIN" in files:
        yield "MENU.BIN", disc.read(*files["MENU.BIN"])


def wanted() -> dict[str, str]:
    """sha1 -> serial, from every tree's splat.yaml."""
    out = {}
    for yaml in sorted((ROOT / "decomp").glob("*/splat.yaml")):
        m = re.search(r"^sha1:\s*([0-9a-f]{40})\s*$", yaml.read_text(), re.M)
        if not m:
            sys.exit(f"{yaml} declares no sha1")
        out[m.group(1)] = yaml.parent.name
    return out


def has_volume(path: Path) -> bool:
    try:
        Disc(path).close()
        return True
    except (OSError, ValueError):
        return False


def images(args: list[str]):
    """The image files the arguments name: a .cue's first track, the data
    tracks of a folder (its audio tracks are passed over), or the file
    itself."""
    for arg in args:
        p = Path(arg)
        if p.is_dir():
            found = [q for q in sorted(p.rglob("*"))
                     if q.suffix.lower() in (".bin", ".iso", ".img")
                     and has_volume(q)]
            if not found:
                print(f"{p}: no disc image with a data track inside")
            yield from found
        elif p.suffix.lower() == ".cue":
            m = re.search(r'^\s*FILE\s+"([^"]+)"', p.read_text(errors="replace"),
                          re.M)
            if m:
                yield p.parent / m.group(1)
            else:
                print(f"{p}: no FILE line")
        elif p.is_file():
            yield p
        else:
            print(f"{p}: not found")


def status(want: dict[str, str]) -> int:
    missing = 0
    for sha, serial in sorted(want.items(), key=lambda kv: kv[1]):
        dst = ROOT / "decomp" / serial / "expected" / "code.bin"
        if not dst.is_file():
            print(f"  {serial:18} MISSING  decomp/{serial}/expected/code.bin")
            missing += 1
        elif hashlib.sha1(dst.read_bytes()).hexdigest() != sha:
            print(f"  {serial:18} WRONG    its sha1 is not splat.yaml's {sha}")
            missing += 1
        else:
            print(f"  {serial:18} ok       {dst.stat().st_size:#x} bytes, "
                  f"sha1 {sha}")
    return missing


def main(argv: list[str]) -> int:
    if not argv or "-h" in argv or "--help" in argv:
        print(__doc__)
        return 0 if argv else 2
    want = wanted()
    if argv != ["--check"]:
        for path in images(argv):
            try:
                disc = Disc(path)
            except (OSError, ValueError) as e:
                print(f"{path.name}: skipped ({e})")
                continue
            hits = 0
            try:
                for what, blob in candidates(disc):
                    serial = want.get(hashlib.sha1(blob).hexdigest())
                    if serial is None:
                        continue
                    dst = ROOT / "decomp" / serial / "expected" / "code.bin"
                    dst.parent.mkdir(parents=True, exist_ok=True)
                    if not dst.is_file() or dst.read_bytes() != blob:
                        dst.write_bytes(blob)
                    print(f"{path.name}: {what} is {serial}'s image")
                    hits += 1
            finally:
                disc.close()
            if not hits:
                print(f"{path.name}: nothing on it is an image a tree here "
                      f"is matched against")
    print("program images:")
    return 1 if status(want) else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
