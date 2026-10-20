#!/usr/bin/env python3
"""Generate the parsers a GCC VCS snapshot does not carry.

    genparsers.py <snapshot gcc/ directory>

A release tarball ships c-parse.y/c-parse.c/c-parse.h and bi-parser.[ch]
pre-generated; a tree checked out of version control has only c-parse.in
and bi-parser.y. GCC's own Makefile would run bison at build time; build.sh
runs it here, on the host, so the Docker stage needs nothing beyond the
packages an old-gcc compiler build already installs.

Three adjustments make a 1997 grammar acceptable to a current bison (3.x).
None changes what the parser does:

  * four mid-rule actions in `structsp` assign an untyped `$$` that the
    rule's last action reads back as `$<ttype>N` — bison 1.x allowed the
    untyped store, bison 3 wants the type spelled at the store too;
  * the grammar's actions call the `YYLEX` macro of bison 1.x's skeleton;
  * bison 3 emits the token codes as an enum, and `RETURN` collides with
    rtl.def's RETURN in c-lex.c (which includes both). Outside the
    generated parser the header gives the `#define` form bison 1.x wrote.
"""
import os
import re
import subprocess
import sys


def bison(args, cwd):
    r = subprocess.run(["bison"] + args, cwd=cwd, capture_output=True,
                       text=True)
    if r.returncode:
        sys.stderr.write(r.stderr)
        sys.exit(f"bison {' '.join(args)} failed")


def replace_file(path, text):
    with open(path + ".new", "w") as f:
        f.write(text)
    os.replace(path + ".new", path)


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    d = sys.argv[1]
    with open(os.path.join(d, "c-parse.in")) as f:
        src = f.read()
    # Makefile.in's own recipe for c-parse.y: drop the Objective-C blocks,
    # keep the C ones.
    src = re.sub(r"^ifobjc\n.*?^end ifobjc\n", "", src, flags=re.S | re.M)
    src = re.sub(r"^(?:ifc|end ifc)\n", "", src, flags=re.M)
    src, n1 = re.subn(
        r"\{ \$\$ = (start_struct \((?:RECORD|UNION)_TYPE, \$2\);)",
        r"{ $<ttype>$ = \1", src)
    src, n2 = re.subn(r"\$\$ = (start_enum \((?:\$2|NULL_TREE)\); \})",
                      r"$<ttype>$ = \1", src)
    if (n1, n2) != (2, 2):
        sys.exit(f"c-parse.in: expected 2+2 untyped mid-rule stores in "
                 f"structsp, found {n1}+{n2} — not the grammar this was "
                 f"written for")
    if "%{\n" not in src:
        sys.exit("c-parse.in: no %{ prologue")
    src = src.replace("%{\n", "%{\n#define YYLEX yylex ()\n", 1)
    replace_file(os.path.join(d, "c-parse.y"), src)
    bison(["-d", "c-parse.y", "-o", "c-parse.c"], d)

    hp = os.path.join(d, "c-parse.h")
    with open(hp) as f:
        h = f.read()
    m = re.search(r"  enum yytokentype\n  \{\n(.*?)\n  \};\n"
                  r"  typedef enum yytokentype yytoken_kind_t;\n", h, re.S)
    if not m:
        sys.exit("c-parse.h: token enum not found — bison too old or too "
                 "new for this script (written against 3.8)")
    defs = [f"#define {name} {val}" for name, val in
            re.findall(r"^\s+([A-Za-z_]\w*) = (-?\d+)", m.group(1), re.M)
            if not name.startswith("YY")]
    replace_file(hp, h[:m.start()] + "#ifdef YYBISON\n" + m.group(0)
                 + "#else\n" + "\n".join(defs) + "\n#endif\n" + h[m.end():])

    bison(["-d", "bi-parser.y", "-o", "bi-parser.c"], d)
    print(f"genparsers: c-parse.[ych] ({len(defs)} tokens), bi-parser.[ch]")


if __name__ == "__main__":
    main()
