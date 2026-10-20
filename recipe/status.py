#!/usr/bin/env python3
"""Measure how much of each program is decompiled, and write it down.

    python3 recipe/status.py          (make status, which runs make programs first)

Reads the last default build of each program (build/<serial>/: the linked
image, its link map and its dependency files) and the tree, and writes

  README.md                   the overview between its two `progress` markers:
                              four shares per program, as badges
  docs/progress.md            every measure, what is left in each program and
                              the history; the whole page is written here
  docs/progress-history.csv   a row per program whenever its numbers change
                              (a second run on the same day replaces that day's)

What is counted:

  functions   One per function symbol the linked image defines from C, plus
              one per stub file under asm/nonmatchings/ (a routine still read
              as a byte range). Alternate entry labels inside a stub are not
              functions; two stubs per tree are labelled as data by the first
              split and are counted here as the routines they are.
  never C     Stub routines listed in config/asm_only.txt: assembly in the
              original program, so not decompilation work.
  copies      Stub routines listed in config/vendor_provided.txt and not in
              asm_only.txt: byte-for-byte copies of Sony library objects.
  code bytes  The .text the linker placed, from the link map. "In C" is
              that minus the bytes the routine stubs name.
  shares      Functions in C and code bytes in C, each out of all of them
              and out of those to decompile: all but the never-C routines
              and the copies. Bytes as well as functions, because one
              function can be a thousand times the size of another.
  image       The whole program image. "From C" is its size minus every byte
              range a stub the build read names (routines, jump tables, data).
"""
import collections
import csv
import datetime
import glob
import os
import re
import subprocess
import sys
import textwrap
from pathlib import Path

import _paths

ROOT = Path(__file__).resolve().parents[1]
README = ROOT / "README.md"
PAGE = ROOT / "docs" / "progress.md"
HISTORY = ROOT / "docs" / "progress-history.csv"
READELF = f"{_paths.TOOLCHAIN}/binutils/bin/mipsel-linux-gnu-readelf"
BEGIN = "<!-- progress: begin. `make status` writes what is between these two lines. -->"
END = "<!-- progress: end -->"

PROGRAMS = [
    ("SLUS_006.28", "Main executable",
     "The game's main executable. Both game discs boot it."),
    ("SLUS_006.28-MENU", "Overlay",
     "`MENU.BIN`, the movie player the game loads over itself before each movie."),
    ("SLUS_009.21", "Bonus disc",
     'The bonus disc, "The Making of Lunar", which also holds the hidden Lords of Lunar minigame.'),
]
FIELDS = ["functions", "in_c", "never", "copies", "open", "text", "text_c", "text_never",
          "text_copies", "image", "image_c"]
RANGE = re.compile(r'\.incbin "[^"]+", 0x[0-9A-F]+, 0x([0-9A-F]+)')


def listed(path):
    if not path.exists():
        return set()
    return {l.split()[0] for l in path.open() if l.strip() and not l.startswith("#")}


def measure(serial):
    build, tree = ROOT / "build" / serial, ROOT / "decomp" / serial
    elf, mp = build / "program.elf", build / "program.map"
    if not elf.exists():
        sys.exit(f"{elf.relative_to(ROOT)} is missing: run `make programs` first")
    out = subprocess.run([READELF, "-sW", str(elf)], capture_output=True, text=True,
                         cwd=ROOT).stdout
    funcs = {p[7] for p in (l.split() for l in out.split("\n"))
             if len(p) == 8 and p[3] == "FUNC" and p[6] != "UND"}
    in_stubs, routines = set(), {}
    for f in glob.glob(f"{tree}/asm/nonmatchings/**/*.s", recursive=True):
        t = open(f).read()
        in_stubs |= set(re.findall(r'^\s*[ga]label\s+(\S+)', t, re.M))
        routines[os.path.basename(f)[:-2]] = sum(int(x, 16) for x in RANGE.findall(t))
    in_c = len(funcs - in_stubs)
    text, started = 0, False
    for l in mp.open():
        started = started or l.startswith("Linker script and memory map")
        m = re.match(r'^\s+\.text\s+0x[0-9a-f]+\s+0x([0-9a-f]+)\s+\S+\.o\s*$', l)
        if started and m:
            text += int(m.group(1), 16)
    read = set()
    for d in glob.glob(f"{build}/**/*.d", recursive=True):
        for w in open(d).read().replace("\\\n", " ").split():
            w = w.rstrip(":")
            if w.endswith(".s") and f"decomp/{serial}/asm/" in w:
                read.add(ROOT / w)
    by_kind = collections.Counter()
    for f in read:
        kind = ("routines" if "/nonmatchings/" in str(f) else
                "jump tables" if "/jtbl/" in str(f) else "data")
        by_kind[kind] += sum(int(x, 16) for x in RANGE.findall(f.read_text(encoding="latin-1")))
    never = routines.keys() & listed(tree / "config" / "asm_only.txt")
    copies = (routines.keys() & listed(tree / "config" / "vendor_provided.txt")) - never
    open_ = sorted(((routines[x], x) for x in routines.keys() - never - copies), reverse=True)
    image = (tree / "expected" / "code.bin").stat().st_size
    return dict(functions=in_c + len(routines), in_c=in_c, never=len(never),
                copies=len(copies), open=len(open_), text=text,
                text_c=text - sum(routines.values()),
                text_never=sum(routines[x] for x in never),
                text_copies=sum(routines[x] for x in copies), image=image,
                image_c=image - sum(by_kind.values()), by_kind=dict(by_kind), open_list=open_)


def share(a, b):
    """A share in percent, rounded to two decimal places."""
    return round(100 * a / b, 2) if b else 0.0


def todo(r):
    """The functions and the code bytes there are to decompile."""
    return (r["functions"] - r["never"] - r["copies"],
            r["text"] - r["text_never"] - r["text_copies"])


HEADS = ("All functions", "Functions to decompile", "All code bytes", "Code bytes to decompile")
LABELS = ("functions in C", "functions in C", "bytes in C", "bytes in C")


def shares(r):
    """The four shares in C, in the order of HEADS."""
    functions, text = todo(r)
    return (share(r["in_c"], r["functions"]), share(r["in_c"], functions),
            share(r["text_c"], r["text"]), share(r["text_c"], text))


def badge(label, value):
    color = next(c for floor, c in ((95, "brightgreen"), (80, "green"), (60, "yellowgreen"),
                                    (40, "yellow"), (20, "orange"), (0, "red")) if value >= floor)
    def esc(s):
        return s.replace("-", "--").replace("_", "__").replace("%", "%25").replace(" ", "%20")
    message = f"{value:.2f}%"
    return f"![{message} {label}](https://img.shields.io/badge/{esc(label)}-{esc(message)}-{color})"


def n(x):
    return f"{x:,}"


def overview(rows, tot, today):
    out = ["| Tree | What it is | " + " | ".join(HEADS) + " |", "|---|---|" + "---|" * len(HEADS)]
    for tree, what, r in [(f"`decomp/{serial}/`", what, r) for (serial, _, what), r in rows] + [
            ("All three", "", tot)]:
        out.append(f"| {tree} | {what} | "
                   + " | ".join(badge(l, v) for l, v in zip(LABELS, shares(r))) + " |")
    out += ["",
            "Each badge is a share in C, counted by functions or by code bytes. *All*",
            "counts all of a program's code. *To decompile* leaves out what is not",
            "decompilation work: the routines that were assembly in the original and the",
            "copies of Sony's library code still read as bytes. Bytes show how much is",
            "left by size, since one function can be a thousand times the size of",
            "another. Each program includes the parts of Sony's PSY-Q libraries it links,",
            f"and they count like the rest. Measured by `make status` on {today};",
            "[docs/progress.md](docs/progress.md) has every count, what is left in each",
            "program, and the history."]
    return "\n".join(out)


def what_is_left(serial, name, r):
    asm = r["never"] + r["copies"] + r["open"]
    cfg = f"decomp/{serial}/config"
    if not asm:
        return f"- **{name}.** Nothing is still assembly."
    parts = []
    if r["never"]:
        parts.append(f"{n(r['never'])} {'was' if r['never'] == 1 else 'were'} assembly in the "
                     f"original program (`{cfg}/asm_only.txt` lists each with its evidence)")
    if r["copies"]:
        parts.append(f"{n(r['copies'])} {'is' if r['copies'] == 1 else 'are'} Sony's library "
                     f"code, byte for byte, that Sony compiled (`{cfg}/vendor_provided.txt` "
                     f"names the library member each lies in)")
    lead = (f"- **{name}.** {n(asm)} {'function is' if asm == 1 else 'functions are'} still "
            f"assembly")
    if not r["open"]:
        line = f"{lead}, and none is open work: " + ("all " if len(parts) == 1 else "")
        return wrap(line + " and ".join(parts) + ".")
    shown = ", ".join(f"`{x}` ({n(size)} bytes)" for size, x in r["open_list"][:10])
    more = f", and {n(r['open'] - 10)} smaller" if r["open"] > 10 else ""
    open_ = f"{n(r['open'])} {'is' if r['open'] == 1 else 'are'} open work: {shown}{more}"
    return wrap(f"{lead}. " + "; ".join([open_] + parts) + ".")


def wrap(item):
    """A list item wrapped like the rest of the docs."""
    return "\n".join(textwrap.wrap(item, 78, subsequent_indent="  ", break_long_words=False,
                                   break_on_hyphens=False))


def history(today, rows):
    """Add today's numbers to the history file; return the dated rows."""
    old = list(csv.DictReader(HISTORY.open())) if HISTORY.exists() else []
    old = [h for h in old if h["date"] != today]
    last = {}
    for h in old:
        last[h["serial"]] = h
    new = []
    for (serial, _, _), r in rows:
        row = {"date": today, "serial": serial, **{k: str(r[k]) for k in FIELDS}}
        prev = last.get(serial)
        if prev is None or any(prev[k] != row[k] for k in FIELDS):
            new.append(row)
    rows_out = old + new
    with HISTORY.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=["date", "serial"] + FIELDS, lineterminator="\n")
        w.writeheader()
        w.writerows(rows_out)
    return rows_out


def history_table(hist):
    names = {s: name for s, name, _ in PROGRAMS}
    out = ["| Date | Program | " + " | ".join(HEADS) + " |", "|---|---|" + "---:|" * len(HEADS)]
    for h in sorted(hist, key=lambda h: (h["date"], list(names).index(h["serial"]))):
        r = {k: int(h[k]) for k in FIELDS}
        out.append(f"| {h['date']} | {names[h['serial']]} | "
                   + " | ".join(f"{v:.2f}%" for v in shares(r)) + " |")
    return "\n".join(out)


def by_unit(rows, tot, unit):
    """The table of one unit, functions or code bytes, for each program and all three."""
    whole, in_c = ("functions", "in_c") if unit == "functions" else ("text", "text_c")
    i = 0 if unit == "functions" else 1
    out = ["| Program | " + ("Functions" if unit == "functions" else "Code bytes")
           + " | To decompile | In C | Share of all | Share of those to decompile |",
           "|---|---:|---:|---:|---:|---:|"]
    for ((serial, name, _), r) in rows + [(("", "All three", ""), tot)]:
        label = f"{name} (`{serial}`)" if serial else name
        s = shares(r)
        out.append(f"| {label} | {n(r[whole])} | {n(todo(r)[i])} | {n(r[in_c])} | "
                   f"{s[2 * i]:.2f}% | {s[2 * i + 1]:.2f}% |")
    return out


def page(rows, tot, today, hist):
    lines = [
        "# Progress",
        "",
        f"How much of each program is decompiled, measured by `make status` on {today}",
        "from the three default builds, each of which matched its original byte for",
        "byte. `make status` writes this whole page ([recipe/status.py](../recipe/status.py)",
        "says how each figure is counted); edit that script, not the page.",
        "",
        "## What counts",
        "",
        "A program's functions are the ones its linked image defines from C plus the",
        "routines still read from the original as byte ranges. Two kinds of routine",
        "among those are not open work:",
        "",
        "- **Assembly in the original program.** No compiler produced it, so there is",
        "  no C to recover. [spec/code/hand-assembly.md](spec/code/hand-assembly.md) has",
        "  the rule and the evidence each such routine needs.",
        "- **Copies of Sony's library code.** A routine whose bytes are, byte for byte,",
        "  an object of Sony's PSY-Q libraries is Sony's compiled code. Converting it",
        "  is optional. Its bytes could come from Sony's own library files instead of",
        "  the disc, as `make CODE=sdk` does for the main program's assembly; no",
        "  program's copies are read that way yet.",
        "",
        "Each share is counted two ways, by functions and by code bytes (the bytes",
        "the linker placed, which the C compiles to or the build reads from the",
        "original), and out of two wholes: *all* of a program's code, and the code",
        "*to decompile*, all of it but those two kinds. Bytes show how much is left",
        "by size, since one function can be a thousand times the size of another.",
        "",
        "## The programs",
        "",
        "### By functions",
        "",
        *by_unit(rows, tot, "functions"),
        "",
        "### By code bytes",
        "",
        *by_unit(rows, tot, "bytes"),
    ]
    lines += [
        "",
        "## What is left",
        "",
        "Functions, with their code bytes in brackets.",
        "",
        "| Program | Still assembly | Assembly in the original | Copies of Sony's code | Open work |",
        "|---|---:|---:|---:|---:|",
    ]
    for (serial, name, _), r in rows:
        left = r["text"] - r["text_c"]
        opened = left - r["text_never"] - r["text_copies"]
        lines.append(f"| {name} | {n(r['never'] + r['copies'] + r['open'])} ({n(left)}) | "
                     f"{n(r['never'])} ({n(r['text_never'])}) | "
                     f"{n(r['copies'])} ({n(r['text_copies'])}) | {n(r['open'])} ({n(opened)}) |")
    lines.append("")
    for (serial, name, _), r in rows:
        lines.append(what_is_left(serial, name, r))
    lines += [
        "",
        "## The whole image",
        "",
        "Beside its code, an image holds jump tables and data. This repository has no",
        "initialised data in C: the build reads every data byte from the image, so an",
        "image's share from C stays well below its code's.",
        "",
        "| Program | Image bytes | From C | Share | Routines read as bytes | Jump tables | Data |",
        "|---|---:|---:|---:|---:|---:|---:|",
    ]
    for (serial, name, _), r in rows:
        k = r["by_kind"]
        lines.append(f"| {name} | {n(r['image'])} | {n(r['image_c'])} | "
                     f"{share(r['image_c'], r['image']):.2f}% | {n(k.get('routines', 0))} | "
                     f"{n(k.get('jump tables', 0))} | {n(k.get('data', 0))} |")
    lines += [
        "",
        "## History",
        "",
        "Each program's four shares, on each day `make status` found its numbers",
        "changed ([progress-history.csv](progress-history.csv) has every count).",
        "The decompilation's earlier course is in [HISTORY.md](../HISTORY.md). Until",
        "2026-10-13 the bonus disc's open work was counted as 144 functions: the",
        "comparison had been made against releases 4.6 and 4.7 of Sony's libraries,",
        "and the disc links 4.5.",
        "",
        history_table(hist),
        "",
    ]
    return "\n".join(lines)


def main():
    today = datetime.date.today().isoformat()
    rows = [(p, measure(p[0])) for p in PROGRAMS]
    tot = collections.Counter()
    for _, r in rows:
        for k in FIELDS:
            tot[k] += r[k]
    text = README.read_text()
    if text.count(BEGIN) != 1 or text.count(END) != 1:
        sys.exit(f"README.md needs one `{BEGIN}` line and one `{END}` line")
    a, b = text.index(BEGIN) + len(BEGIN), text.index(END)
    README.write_text(text[:a] + "\n" + overview(rows, tot, today) + "\n" + text[b:])
    hist = history(today, rows)
    PAGE.write_text(page(rows, tot, today, hist))
    print(f"{'':18} {'functions in C':>23}   {'code bytes in C':>23}")
    print(f"{'':18} {'all':>10} {'to decompile':>12}   {'all':>10} {'to decompile':>12}")
    for (serial, name, _), r in rows + [(("all three", "", ""), tot)]:
        s = shares(r)
        print(f"{serial:18} {s[0]:9.2f}% {s[1]:11.2f}%   {s[2]:9.2f}% {s[3]:11.2f}%")
    print(f"wrote README.md (overview), {PAGE.relative_to(ROOT)}, {HISTORY.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
