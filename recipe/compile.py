#!/usr/bin/env python3
"""Compile a segment C file to assembly for the matching build.

Two paths:
- Pure INCLUDE_ASM stub files are translated directly to .include
  directives (no compiler involved — fast and byte-exact).
- Files containing real C go through the era-correct pipeline:
      cpp | cc1 (GCC 2.8.1 PSX, -O2 -G8) | maspsx --expand-div
  which has been verified to reproduce the original codegen
  byte-for-byte (BUILD.md).
"""
import os
import re
import subprocess
import sys
import _paths

INC = re.compile(r'INCLUDE_(ASM|RODATA)\("([^"]+)",\s*([^)\s]+)\)\s*;')

CPP = [f"{_paths.TOOLCHAIN}/gcc-2.8.1-psx/cpp",
       "-I", str(_paths.DECOMP_INCLUDE),
       # The real Sony PSY-Q SDK (4.4 — the version whose CC1PSX is the
       # original of our compiler). psyq_types.h is a shim over it. The
       # headers are Sony's and are not in this repository: toolchain.sh
       # fetches them, or PSYQ_INCLUDE names your own copy (BUILD.md).
       "-I", _paths.PSYQ_INCLUDE,
       "-DINCLUDE_ASM_USE_MACRO_INC=1"]
CC1 = [f"{_paths.TOOLCHAIN}/gcc-2.8.1-psx/cc1", "-quiet", "-O2", "-G8"]
# The PSY-Q LIBRARY objects were built with an older compiler than the game
# code. GCC 2.8.1 turns a jump-to-return into a duplicated return; 2.7.2
# keeps ONE `jr $ra` and jumps to it, which is the shape every SDK object
# has. Opt-in per file via `@cc272` in cc_flags.txt — measured on
# _spu_FsetRXX (MATCH under 2.7.2, 14/17 under 2.8.1) and CdComstr/CdIntstr
# (11/13 vs 8/13). See docs/spec/code/subsystems/psyq/codegen.md, "Telling
# the compilers apart".
CC1_272 = [f"{_paths.TOOLCHAIN}/gcc-2.7.2-psx/cc1", "-quiet", "-O2", "-G8"]
# GCC 2.7.2 with GCC 2.8's 4-byte reload spill slots (one line in reload1.c
# alter_reg). Sony's SDK 4.5+
# library build (libmcrd LOW.OBJ, func_800A45B8) has 2.7.2's handling of a
# spilled register PARAMETER (a home in the callee's own frame) together
# with 2.8's mode-aligned slot size; stock 2.7.2 rounds every slot to 8 and
# 2.8.x homes the parameter in the caller's area.
# Only the `cc272_lib45` per-function recipe uses it; on code that spills
# nothing it is codegen-identical to CC1_272.
CC1_272_SLOT4 = [f"{_paths.TOOLCHAIN}/gcc-2.7.2-psx-slot4/cc1", "-quiet", "-O2",
                 "-G8"]
# The SDK 4.5+ library compiler itself, dated by measurement: GCC's
# development tree as of 1997-03-18, between the 2.7.2 and 2.8.0 releases
# (recipe/gcc-970318/build.sh has the four-snapshot
# bracket). It has what CC1_272_SLOT4 approximates — 4-byte reload slots
# with a spilled register parameter still homed in the callee's frame —
# and a 1996 loop.c no 2.7.2.x release carries. Only the `cc97_lib`
# per-function recipe uses it.
CC1_970318 = [f"{_paths.TOOLCHAIN}/gcc-970318-psx/cc1", "-quiet", "-O2", "-G8"]
#: Per-function flag markers that exist only in GCC 2.8 — never passed to a
#: 2.7.2 alt pass (see run_compiler's cc272* block).
CC1_272_ONLY_28 = frozenset(("-mno-split-addresses",))
CC_FLAGS_CFG = f"{_paths.DECOMP_CONFIG}/cc_flags.txt"
_cc_flags = None


def extra_cc1_flags(src):
    """Per-file cc1 flags from decomp/config/cc_flags.txt.

    Format: `<path-or-basename>  <flag> [flag...]`, `#` comments.

    Needed because the original build did NOT use one flag set for the whole
    image. Some translation units — nearly all of them PSY-Q library objects
    — were built with delayed-branch scheduling OFF, which puts the stack
    adjust BEFORE `jr $ra` and leaves the branch delay slot empty:
        lw $ra,N($sp) / addiu $sp,$sp,N / jr $ra / nop     (-fno-delayed-branch)
        lw $ra,N($sp) / nop / jr $ra / addiu $sp,$sp,N     (default)
    Verified byte-exact on CdSync.
    """
    global _cc_flags
    if _cc_flags is None:
        _cc_flags = []
        try:
            for line in open(CC_FLAGS_CFG):
                line = line.split("#")[0].strip()
                if not line:
                    continue
                pat, *flags = line.split()
                _cc_flags.append((pat, flags))
        except OSError:
            pass
    out = []
    for pat, flags in _cc_flags:
        if src == pat or os.path.basename(src) == pat:
            out += flags
    return [f for f in out if not f.startswith("@")]


def cc1_for(src):
    """The compiler for this file: 2.7.2 when cc_flags.txt marks it
    `@cc272`, else the default 2.8.1. See CC1_272."""
    extra_cc1_flags(src)                       # populate the cache
    for pat, flags in _cc_flags or []:
        if (src == pat or os.path.basename(src) == pat) and "@cc272" in flags:
            return CC1_272
    return CC1


def wants_delay_fill(src):
    """True when cc_flags.txt marks this file `@delay-fill`.

    OPT-IN, and it must stay that way: files whose cc1 already filled its
    delay slots must not be touched, and every currently matching function
    depends on maspsx not moving instructions. See recipe/delayfill.py.
    """
    extra_cc1_flags(src)                       # populate the cache
    for pat, flags in _cc_flags or []:
        if (src == pat or os.path.basename(src) == pat) and \
                "@delay-fill" in flags:
            return True
    return False
# --aspsx-version 2.60: relative to the default config this changes ONLY
# sltu_at (expand_li is already off): a negative sltu immediate stays one
# `sltiu`, as the game's assembler wrote it. The `li $at,-N; sltu` expansion
# occurs 0 times in code.bin, so no matching function can depend on it, and
# func_8001EA74's `sltiu ..., -1` needs it.
MASPSX = [sys.executable, f"{_paths.TOOLCHAIN}/maspsx/maspsx.py", "--expand-div",
          "--dont-force-G0", "--dont-expand-li", "--aspsx-version", "2.60"]


def _maspsx(cc1_out, src, label):
    """maspsx over cc1 output. Through the resident asmpost server when one
    is live (asmpost.request, mode=maspsx — strip_far stays in-process
    here), else the spawn. Identical by construction: the server runs
    maspsx.py's own main(). The server is station-tools' (to be released
    later): the spawn was ~60 of an attempt's 170 ms, paid per variant."""
    try:
        import asmpost
        out = asmpost.request(cc1_out, mode="maspsx")
        if out is not None:
            return out
    except Exception:                   # noqa: BLE001 — the helper must never break a build
        pass
    r = subprocess.run(MASPSX, input=cc1_out, capture_output=True)
    if r.returncode:
        sys.stderr.buffer.write(r.stderr)
        sys.exit(f"{src}: maspsx{label} failed")
    return r.stdout


def is_pure_stub(text):
    rest = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    rest = re.sub(r"//[^\n]*", "", rest)
    rest = re.sub(r"^\s*#.*$", "", rest, flags=re.M)
    rest = INC.sub("", rest)
    return not rest.strip()


def emit_stub_asm(text, dst):
    out = ['.include "macro.inc"', "", ".section .text",
           ".set noat", ".set noreorder", ""]
    for kind, folder, name in INC.findall(text):
        if kind == "RODATA":
            out.append(".section .rodata")
            out.append(f'.include "{folder}/{name}.s"')
            out.append(".section .text")
        else:
            out.append(f'.include "{folder}/{name}.s"')
    with open(dst, "w") as f:
        f.write("\n".join(out) + "\n")


def load_far_syms(src=None):
    """Symbols in regular .data/.bss despite being <=8 bytes. Stripping
    their `.extern sym, size` directive makes GAS use the lui $at + %hi/%lo
    expansion instead of %gp_rel, matching the original.

    A line may be scoped to one translation unit:

        D_800B8E71                 # every TU (the default)
        D_800B8F38  @battle_b.c    # only when compiling battle_b.c

    Scoping exists because the battle segment is several TUs and ~15 globals
    are small (gp) in one and far (%hi/%lo) in another — a global far_syms
    entry cannot express that.  `src` is the file being compiled; when it is
    None only unscoped symbols are returned."""
    path = f"{_paths.DECOMP_CONFIG}/far_syms.txt"
    base = os.path.basename(src) if src else None
    syms = set()
    if os.path.exists(path):
        with open(path) as f:
            for line in f:
                line = line.split("#")[0].strip()
                if not line:
                    continue
                sym, _, scope = line.partition("@")
                sym, scope = sym.strip(), scope.strip()
                if not scope or scope == base:
                    syms.add(sym)
    return syms


GP_WINDOW = 0x7FFF
_gp_addr = None
_sym_addrs = None


def _gp():
    global _gp_addr
    if _gp_addr is None:
        try:
            m = re.search(r"_gp\s*=\s*(0x[0-9a-fA-F]+)",
                          open(str(_paths.PROGRAM_LD)).read())
            _gp_addr = int(m.group(1), 16) if m else 0
        except OSError:
            _gp_addr = 0
    return _gp_addr


def _sym_addr(name):
    """Address of a data/function symbol: from the splat naming convention
    (D_/func_<addr>) else config/symbol_addrs.txt. None when unknown."""
    m = re.match(r"^(?:D|func|FUN|jtbl)_(80[0-9A-Fa-f]{6})", name)
    if m:
        return int(m.group(1), 16)
    global _sym_addrs
    if _sym_addrs is None:
        _sym_addrs = {}
        try:
            for line in open(str(_paths.SYMBOL_ADDRS)):
                m2 = re.match(r"\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+)\s*;", line)
                if m2:
                    _sym_addrs.setdefault(m2.group(1), int(m2.group(2), 16))
        except OSError:
            pass
    return _sym_addrs.get(name)


def _out_of_gp_window(name):
    """An extern outside the $gp window CANNOT be small data — arithmetic,
    not judgement: %gp_rel is a
    signed 16-bit displacement, so abs(addr - _gp) > 0x7FFF link-fails
    with 'relocation truncated to fit: R_MIPS_GPREL16'. Stripping the
    `.extern sym, size` directive makes GAS use the %hi/%lo expansion
    instead — the only correct codegen for such a symbol. This was the
    single blocker seeding the two largest battle functions."""
    gp = _gp()
    if not gp:
        return False
    addr = _sym_addr(name)
    return addr is not None and abs(addr - gp) > GP_WINDOW


def strip_far_externs(asm, far_syms):
    out = []
    for line in asm.decode("utf-8", "replace").splitlines():
        m = re.match(r"\s*\.extern\s+([A-Za-z_]\w*)\s*,\s*(\d+)", line)
        if m and (m.group(1) in far_syms
                  or (int(m.group(2)) <= 8
                      and _out_of_gp_window(m.group(1)))):
            continue
        out.append(line)
    return ("\n".join(out) + "\n").encode()


MARKER = re.compile(r"/\*\s*far_syms:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")


def marker_far_syms(src):
    """`/* far_syms: SYM [SYM...] */` markers in the file being compiled —
    the file-local equivalent of a TU-scoped far_syms.txt entry. Lets a
    generated attempt carry its
    far-scalar evidence with it; when the chunk is pasted in-tree the
    marker rides along and keeps meaning the same thing."""
    try:
        text = open(src, errors="replace").read()
    except OSError:
        return set()
    syms = set()
    for m in MARKER.finditer(text):
        syms.update(m.group(1).split())
    return syms


EPI_MARKER = re.compile(
    r"/\*\s*epilogue_load_slot:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
RET_MARKER = re.compile(
    r"/\*\s*return_slot_fill:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
NODB_MARKER = re.compile(
    r"/\*\s*no_delayed_branch:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
ZR_MARKER = re.compile(
    r"/\*\s*zero_remat:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
CC272_MARKER = re.compile(
    r"/\*\s*cc272:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
CC272DB_MARKER = re.compile(
    r"/\*\s*cc272_db:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
CC272LIB_MARKER = re.compile(
    r"/\*\s*cc272_lib:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
CC272LIB45_MARKER = re.compile(
    r"/\*\s*cc272_lib45:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
CC97LIB_MARKER = re.compile(
    r"/\*\s*cc97_lib:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
GNUAS_MARKER = re.compile(
    r"/\*\s*gnu_as_loads:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
ARGSLOT_MARKER = re.compile(
    r"/\*\s*arg_slot_fill:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
LHN_MARKER = re.compile(
    r"/\*\s*load_hazard_nop:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
LAA_MARKER = re.compile(
    r"/\*\s*la_absolute:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
NSA_MARKER = re.compile(
    r"/\*\s*no_split_addresses:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
NSI_MARKER = re.compile(
    r"/\*\s*no_schedule_insns:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
RF_MARKER = re.compile(
    r"/\*\s*reorder_fill:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
LIBG0_MARKER = re.compile(
    r"/\*\s*lib_g0:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")
INLFN_MARKER = re.compile(
    r"/\*\s*inline_functions:\s*([A-Za-z_][\w\s]*?)\s*(?:\(|\*/)")


def zero_remat_funcs(src, scope_as=None):
    """Functions whose zero-holding register copies are rewritten to
    independent $zero materializations — see delayfill.zero_remat for
    the mechanism, its conservative proof rules, and the measurement
    that no source shape can produce the target bytes. Fifth member of
    the per-function family; same dual-source contract."""
    return _per_function_list(src, "zero_remat.txt", ZR_MARKER)


def reorder_fill_funcs(src, scope_as=None):
    """Functions whose DELAYED-BRANCH output gets the assembler's own fill
    over the slots reorg left empty, macro halves split (`lui $a0 / jal /
    addiu $a0`) — see delayfill.reorder_fill. Composes with any compiler
    marker. Eleventh member of the per-function family; same dual-source
    contract."""
    return _per_function_list(src, "reorder_fill.txt", RF_MARKER)


def arg_slot_fill_funcs(src, scope_as=None):
    """Functions whose empty `jal` delay slot takes the ARGUMENT SETUP —
    see delayfill.arg_slot_fill. Seventh member of the per-function family;
    same dual-source contract."""
    return _per_function_list(src, "arg_slot_fill.txt", ARGSLOT_MARKER)


def load_hazard_nop_funcs(src, scope_as=None):
    """Functions that get the ASPSX load-delay nop before a bare-symbol
    store reusing the loaded register — the extern-gp hazard maspsx
    cannot see. See delayfill.load_hazard_nop. Eighth member of the
    per-function family; same dual-source contract."""
    return _per_function_list(src, "load_hazard_nop.txt", LHN_MARKER)


def la_absolute_funcs(src, scope_as=None):
    """Functions whose `la $r,SYM` macros are expanded to lui/addiu
    before GNU as can turn a small-data one gp-relative — ASPSX never
    used $gp for `la` (maspsx's table: 2.81+ only). See
    delayfill.la_absolute. Ninth member of the per-function family;
    same dual-source contract."""
    return _per_function_list(src, "la_absolute.txt", LAA_MARKER)


def cc272_funcs(src, scope_as=None):
    """Functions compiled by GCC **2.7.2** instead of 2.8.1, individually,
    via a second cc1 run of the same TU and a block splice.

    The PSY-Q LIBRARY objects were built with the older compiler. The tell
    is the epilogue on a leaf: 2.8.1 turns a jump-to-return into a
    DUPLICATED `jr $ra` in every arm, 2.7.2 keeps ONE and jumps to it —
    which is what every SDK object has. No 2.8.1 flag reproduces it
    (measured: -O1, -fno-thread-jumps, -fno-cse-follow-jumps,
    -fno-expensive-optimizations, -fno-delayed-branch all still duplicate).

    Per FUNCTION, never per file: a split TU mixes library objects with
    already-converted game functions that need 2.8.1 — marking
    SLUS_009.21's main_an.c whole broke func_8012ABC0 and the image with
    it. Sixth member of the per-function family; same dual-source
    contract. See docs/spec/code/subsystems/psyq/codegen.md, "Telling
    the compilers apart"."""
    return _per_function_list(src, "cc272.txt", CC272_MARKER)


def cc272_db_funcs(src, scope_as=None):
    """cc272 WITH GCC's own delayed branches: the 2.7.2 alt pass drops the
    TU's `-fno-delayed-branch`. The psyq TUs' `-fno-delayed-branch
    @delay-fill` recipe was fitted under 2.8.1; over the 74 delay-fill-TU
    psyq seeds 2.7.2 with its own
    delay slots was the best recipe for 21 (3 MATCH) against 12 for plain
    cc272 — so a per-function choice, never a new default (the landed
    spu_cmd_01_sub_5368 matches under plain cc272). A function in both
    lists takes cc272_db."""
    return _per_function_list(src, "cc272_db.txt", CC272DB_MARKER)


def cc272_lib_funcs(src, scope_as=None):
    """The PSY-Q LIBRARY RECIPE, as one hypothesis: GCC 2.7.2 with its own
    delayed branches (cc272_db) AND `-fno-strength-reduce`, then ASPSX's
    delay-slot fill over the slots 2.7.2 left empty (delayfill.fill with
    skip_filled_slots — never stealing a slot reorg already filled) and
    ASPSX's load-delay nop BEFORE a loop-head label
    (delayfill.nop_before_back_label). Measured on SLUS_006.28
    StCdInterrupt (func_80093D28; libcd c_011.o, EXACT vs the 4.6 and 4.7
    objects): every part was needed — reorg's steal-from-target fills
    (`beqz ; li $v0,N` with the li repeated on the fall-through), macro
    halves in `j` slots (`lui $at` before, `sw %lo` in the slot) that only
    the assembler fill produces, an index loop the strength reducer would
    have turned into a pointer walk, and the loop-head nop. Opt-in per
    FUNCTION and never applied to the cc272 / cc272_db lists (a function
    here leaves both): the 32 functions already landed under those were
    re-proven byte-identical with this pass present. The measurements are
    in the commit that landed func_80093D28 with it (2026-10-08)."""
    return _per_function_list(src, "cc272_lib.txt", CC272LIB_MARKER)


def cc272_lib45_funcs(src, scope_as=None):
    """The SDK 4.5+ LIBRARY RECIPE: cc272_lib's flags and post-passes
    (own delayed branches, -fno-strength-reduce, the skip-filled-slots
    fill, the loop-head nop) on CC1_272_SLOT4 (2.7.2 + 2.8's 4-byte spill
    slots) with `-mhard-float`. The hard-float half matters to codegen
    with NO floating point in sight: soft-float marks the 32 FP registers
    fixed, which halves loop.c's invariant-hoisting threshold
    ((1 + n_non_fixed_regs) * (call ? 1 : 2)), and the 4.5 library's
    loops hoist exactly at the hard-float threshold. Measured on
    func_800A45B8 (libmcrd `_card_create2`, LOW.OBJ, first in SDK 4.5):
    the same C 346 -> 383 for the flag alone.
    Opt-in per FUNCTION; a function here leaves cc272 / cc272_db /
    cc272_lib. Refuses loudly when the slot4 cc1 is not vendored."""
    return _per_function_list(src, "cc272_lib45.txt", CC272LIB45_MARKER)


def cc97_lib_funcs(src, scope_as=None):
    """The SDK 4.5+ LIBRARY RECIPE ON ITS OWN COMPILER: CC1_970318 (the
    1997-03-18 GCC snapshot) with `-mhard-float -fno-exceptions`, its own
    delayed branches, then cc272_lib's two assembler passes (the
    skip-filled-slots fill, the loop-head nop). Unlike cc272_lib and
    cc272_lib45 the strength reducer stays ON: this compiler's loop.c
    leaves a lone `sym + (i << n)` address alone and reduces what the
    library's loops have reduced, where 2.7.2's does the opposite on both
    counts. `-fno-exceptions` because the tree of that month wrote an
    exception table for every C function with a call; `-mhard-float` for
    the reason cc272_lib45_funcs gives (239 vs 428 of 429 without it).
    Measured on func_800A45B8 (libmcrd `_card_create2`): plain index
    loops, byte-identical, where the best C under cc272_lib45 stopped at
    407/429 with hand-written pointer walks. Three of the four functions
    landed under cc272_lib45 also match under this recipe as written;
    func_8009C7D8 does not (324/347), so the older recipe stays.
    Opt-in per FUNCTION; a function here leaves the cc272* lists. Refuses
    loudly when the snapshot cc1 is not vendored."""
    return _per_function_list(src, "cc97_lib.txt", CC97LIB_MARKER)


def gnu_as_loads_funcs(src, scope_as=None):
    """Functions assembled by GNU as rather than ASPSX — see
    gnu_as_loads. Same dual-source contract as the rest of the family."""
    return _per_function_list(src, "gnu_as_loads.txt", GNUAS_MARKER)


def gnu_as_div_funcs(src, scope_as=None):
    """Functions whose `div` macro was expanded by GNU as, which puts NO nop
    between the macro's closing mflo/mfhi and a next instruction reading the
    result — see delayfill.div_result_nop (ASPSX, and so maspsx, does).

    Not a marker of its own: it is the functions the tree already says GNU
    as assembled — every function under a LIBRARY compiler recipe
    (cc272_lib / cc272_lib45 / cc97_lib: those recipes are the SDK library,
    whose assembler was GNU as) and every `gnu_as_loads` function (the
    per-function "this object's assembler is GNU as" marker under any other
    compiler). Game code is in neither set and keeps maspsx's nop. Measured
    on SLUS_006.28's image: 28 of 28 game sites have the nop, 0 of 10
    library sites do.

    GNU_AS_DIV_DISABLE=1 in the environment turns the pass off — the bisect
    switch (like ASMPOST_DISABLE): a doubt about this pass is settled by
    compiling the same file both ways."""
    if os.environ.get("GNU_AS_DIV_DISABLE"):
        return set()
    return (cc272_lib_funcs(src, scope_as) | cc272_lib45_funcs(src, scope_as)
            | cc97_lib_funcs(src, scope_as) | gnu_as_loads_funcs(src, scope_as))


_REG_NAMES =(["zero", "at", "v0", "v1"] + [f"a{i}" for i in range(4)]
              + [f"t{i}" for i in range(8)] + [f"s{i}" for i in range(8)]
              + ["t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"])
_REG_NUM = {**{f"${i}": i for i in range(32)},
            **{f"${n}": i for i, n in enumerate(_REG_NAMES)}, "$s8": 30}
#: The loads GNU as expands with the DESTINATION as the %hi temporary
#: (gas's ld macro: not lwl/lwr, which merge into the destination, and not
#: coprocessor loads). Stores always use $at, in both assemblers.
GNU_AS_DEST_TEMP_LOADS = frozenset(("lb", "lbu", "lh", "lhu", "lw"))
_EXPAND_AT = re.compile(
    r"(?P<i1>\s*)lui\s+\$at,\s*%hi\((?P<sym>[^)]+)\)\s*\n"
    r"(?P<i2>\s*)addu\s+\$at,\s*\$at,\s*(?P<base>\$\w+)\s*\n"
    r"(?P<i3>\s*)(?P<op>[a-z0-9]+)\s+(?P<dest>\$\w+),\s*%lo\((?P=sym)\)\(\$at\)")
#: maspsx's expansion of a NUMERIC offset outside 16 bits (`op $rd,K($rs)`:
#: a fixed hardware address indexed by a register). ASPSX adds the base
#: FIRST there (`addu $at,$rs,$at`), unlike its symbol form above; GNU as
#: writes `addu $t,$t,$rs` for both, with the same choice of temporary.
_EXPAND_AT_NUM = re.compile(
    r"(?P<i1>\s*)lui\s+\$at,\s*%hi\((?P<sym>-?(?:0[xX][0-9a-fA-F]+|\d+))\)\s*\n"
    r"(?P<i2>\s*)addu\s+\$at,\s*(?P<base>\$\w+),\s*\$at\s*\n"
    r"(?P<i3>\s*)(?P<op>[a-z0-9]+)\s+(?P<dest>\$\w+),\s*%lo\((?P=sym)\)\(\$at\)")


def gnu_as_loads(text, funcs):
    """Re-expand maspsx's indexed far loads the way GNU as does, in the
    named functions only. -> (new_text, n_rewritten).

    For `op $rd, SYM($rs)` maspsx always writes the ASPSX form

        lui $at,%hi(SYM) / addu $at,$at,$rs / op $rd,%lo(SYM)($at)

    while GNU as uses the destination as the temporary when it can:

        lui $rd,%hi(SYM) / addu $rd,$rd,$rs / op $rd,%lo(SYM)($rd)

    — for lb/lbu/lh/lhu/lw with $rd != $rs (and $rd not $at/$zero).
    It keeps $at when the destination IS the base, for lwl/lwr (they
    merge into the destination), for coprocessor loads and for every
    store. Verified on the vendored GNU as and on the target: the SDK 4.5+ library objects carry
    this form, 61 live functions use it, none uses the $at form for such
    a load, none mixes the two.

    A NUMERIC offset outside 16 bits (`op $rd, K($rs)`, cc1's one
    instruction for a fixed hardware address plus a register) is the
    same macro in GNU as, but maspsx writes ASPSX's own form of it, with
    the base first in the add:

        lui $at,%hi(K) / addu $at,$rs,$at / op $rd,%lo(K)($at)

    GNU as: the destination as the temporary under the rule above, else
    `lui $at,%hi(K) / addu $at,$at,$rs / op $rd,%lo(K)($at)` — so here
    every site is rewritten, the $at ones for the operand order alone.
    Probed on the vendored GNU as; the image has the shape at three
    sites, all in libcd's dma_execute (0x800946A8, 0x800946CC,
    0x80094710), all in GNU form, and ASPSX's numeric form nowhere, game
    code included.

    Applied right after maspsx, BEFORE any delay-slot pass, because it is
    an assembler expansion. A bare far load (`lw $rd,SYM`, no base) is
    already left for GNU as by maspsx and needs nothing. Per FUNCTION,
    opt-in: no matched function moves."""
    if not funcs:
        return text, 0
    lines = text.split("\n")
    spans, cur, start = [], None, 0
    for i, line in enumerate(lines):
        s = line.strip()
        if s.startswith(".ent"):
            name = s.split()[-1]
            cur, start = (name if name in funcs else None), i
        elif s.startswith(".end") and cur is not None:
            spans.append((start, i))
            cur = None
    n = 0

    def sub(m):
        nonlocal n
        op, dest, base = m.group("op"), m.group("dest"), m.group("base")
        dn, bn = _REG_NUM.get(dest), _REG_NUM.get(base)
        if (op not in GNU_AS_DEST_TEMP_LOADS or dn is None or bn is None
                or dn == bn or dn in (0, 1)):
            return m.group(0)
        n += 1
        sym = m.group("sym")
        return (f"{m.group('i1')}lui\t{dest},%hi({sym})\n"
                f"{m.group('i2')}addu\t{dest},{dest},{base}\n"
                f"{m.group('i3')}{op}\t{dest},%lo({sym})({dest})")

    def sub_num(m):
        # `op $rd,K($rs)`, K a number: the same choice of temporary; where
        # $at stays (destination is the base, lwl/lwr, a coprocessor load,
        # a store) GNU as still adds the base SECOND, so every site moves.
        nonlocal n
        op, dest, base = m.group("op"), m.group("dest"), m.group("base")
        dn, bn = _REG_NUM.get(dest), _REG_NUM.get(base)
        tmp = dest if (op in GNU_AS_DEST_TEMP_LOADS and dn is not None
                       and bn is not None and dn != bn
                       and dn not in (0, 1)) else "$at"
        n += 1
        k = m.group("sym")
        return (f"{m.group('i1')}lui\t{tmp},%hi({k})\n"
                f"{m.group('i2')}addu\t{tmp},{tmp},{base}\n"
                f"{m.group('i3')}{op}\t{dest},%lo({k})({tmp})")
    for a, b in reversed(spans):
        block = "\n".join(lines[a:b + 1])
        block = _EXPAND_AT_NUM.sub(sub_num, block)
        lines[a:b + 1] = _EXPAND_AT.sub(sub, block).split("\n")
    return "\n".join(lines), n


def _per_function_list(src, cfg_name, marker_re, config_dir=None):
    """The shared dual-source reader behind EVERY per-function list: a
    tracked `config/<cfg_name>` plus file-local `/* <name>: FUNC ... */`
    markers, so a generated attempt carries its own evidence and keeps
    meaning the same thing once pasted in-tree (the far_syms contract).
    All three lists (epilogue_load_slot / return_slot_fill /
    no_delayed_branch) route through here — there were three inline
    copies of this logic until 2026-08-08, and the config-file HALF had
    no live user at all (the first no_delayed_branch opt-in chose the
    in-source marker, rightly), so a break in it would have surfaced
    only on the first tracked entry, far from whatever broke it.
    `config_dir` exists for the selftest: the parse half must be
    testable without a program tree (a selftest that needs one is
    testing the tree, not the tool)."""
    out = set()
    try:
        for line in open(os.path.join(str(config_dir or _paths.DECOMP_CONFIG),
                                      cfg_name)):
            line = line.split("#")[0].strip()
            if line:
                out.update(line.split())
    except OSError:
        pass
    try:
        text = open(src, errors="replace").read()
    except OSError:
        return out
    for m in marker_re.finditer(text):
        out.update(m.group(1).split())
    return out


#: Every compile-directive comment an attempt can carry. These are LOAD-
#: BEARING: far_syms decides addressing shape, the other three decide
#: delay-slot codegen. Any tool that round-trips an attempt through a
#: comment-stripping parser (the vendored permuter's pycparser) MUST
#: re-attach them via carry_markers below — measured 2026-08-08: a
#: permuter output silently went $gp-relative and lost two instructions
#: to addressing because its far_syms marker died in the round trip.
DIRECTIVE_COMMENT = re.compile(
    r"/\*\s*(?:far_syms|epilogue_load_slot|return_slot_fill|arg_slot_fill|"
    r"no_delayed_branch|cc272_lib45|cc272_lib|cc272_db|cc272|cc97_lib|"
    r"load_hazard_nop|"
    r"zero_remat|no_split_addresses|no_schedule_insns|la_absolute|"
    r"gnu_as_loads|reorder_fill|lib_g0|inline_functions):.*?\*/", re.S)


def carry_markers(seed_text, text):
    """Prepend to `text` every compile-directive comment present in
    `seed_text` and missing from `text`. Returns `text` unchanged when
    nothing is missing — callers use identity to decide whether a
    .marked.c copy is needed."""
    missing = [m.group(0) for m in DIRECTIVE_COMMENT.finditer(seed_text)
               if m.group(0) not in text]
    if not missing:
        return text
    return "\n".join(missing) + "\n" + text


def no_delayed_branch_funcs(src, scope_as=None):
    """Functions compiled under `-fno-delayed-branch` INDIVIDUALLY, via a
    second cc1 run of the same TU and a block splice — see
    delayfill.splice for the mechanism and its refusal rules.

    Per FUNCTION because a per-file flag cannot express it: SLUS_009.21
    is a single TU, so cc_flags.txt is all-or-nothing there — the TU
    wall again, for compiler flags. func_80120380's target has empty
    conditional-branch delay slots (assembler-era scheduling) while the
    TU's 85 matched functions need the default. Fourth member of the
    per-function family; same dual-source contract as its siblings."""
    return _per_function_list(src, "no_delayed_branch.txt", NODB_MARKER)


def no_split_addresses_funcs(src, scope_as=None):
    """Functions compiled under `-mno-split-addresses` INDIVIDUALLY — the
    same alt-pass + block splice as no_delayed_branch (ALT_FLAG_PASSES).

    `-msplit-addresses` (cc1psx's default) lets the scheduler and CSE
    treat the `lui %hi` and the `%lo` use of a global's address as two
    instructions: the `lui` hoists out of loops, into delay slots, or
    into a callee-saved register. The original build's output shows
    functions where the pair stayed together — measured 2026-10-06 over
    the 345-function >=85% board: the flag alone took 10 near-misses to
    MATCH and improved 9 more while worsening 169, so it is a per-FUNCTION
    switch, never per file (ygofm-decomp's WORKFLOW.md reached the same
    verdict on its one-TU-per-function layout). Ninth member of the
    per-function family; same dual-source contract."""
    return _per_function_list(src, "no_split_addresses.txt", NSA_MARKER)


def no_schedule_insns_funcs(src, scope_as=None):
    """Functions compiled under `-fno-schedule-insns` (first scheduling
    pass off, second still on) INDIVIDUALLY, via ALT_FLAG_PASSES.
    Measured 2026-10-06 over the same board: 1 MATCH (func_800566AC),
    2 improved, 248 worse — real but rare, hence per function. Tenth
    member of the per-function family; same dual-source contract."""
    return _per_function_list(src, "no_schedule_insns.txt", NSI_MARKER)


def lib_g0_funcs(src, scope_as=None):
    """Functions compiled under `-G0` INDIVIDUALLY, via ALT_FLAG_PASSES:
    the SDK libraries were built with no small-data section, so their cc1
    saw every 4- and 8-byte static as a FAR symbol, and that decides code
    no declaration can stand in for. Where cc1 itself forces a scalar's
    address into a register (expand_increment, for a post-increment whose
    value is used: `buf[n++] = c`), cse's find_best_addr keeps the
    register only when the symbol is far (ADDRESS_COST 2 against the
    register's 1; a small-data symbol ties at 1 and the tie goes to the
    symbol). Target: `la r,n` / `lw x,0(r)` / ... / `sw x,0(r)` beside
    plain macro accesses of the same scalar; at -G8 the pair is two
    macros whatever far_syms says (that marker only stops the
    ASSEMBLER's gp addressing), and an unsized array instead puts every
    access through one shared register. Measured on libc2 `_putchar`
    (cd_func_2238, PUTCHAR.OBJ): 59/63 -> MATCH. The flag reaches
    the function's cc272* / cc97_lib pass like every row of the table (a
    later -G overrides the command's -G8). Per FUNCTION, never per file:
    landed library functions were matched at -G8 with their own
    declarations. Same dual-source contract as its siblings."""
    return _per_function_list(src, "lib_g0.txt", LIBG0_MARKER)


def inline_functions_funcs(src, scope_as=None):
    """Functions compiled under `-finline-functions` (all that -O3 adds
    to -O2 in these compilers) INDIVIDUALLY, via ALT_FLAG_PASSES: cc1 then
    expands in line the simple functions DEFINED ABOVE the marked one in
    its TU, with no `inline` keyword in the source, and still emits them
    as the global functions they are. libc2 PUTCHAR.OBJ was built that
    way: `putchar(c) { _putchar(c); _putchar_flash(); }` (cd_func_2370)
    carries both bodies in line, its only `jal`s being `_putchar`'s own
    recursion: 74/74 with the flag from the two calls. The keyword is no
    substitute here: GCC 2.8.1, the primary pass whose function order the
    splice keeps, defers EVERY inlinable function's output to the end of
    the file (toplev.c rest_of_compilation), which moves an `inline`
    global behind its caller; for the same reason the flag is never a
    TU-level one (measured: 257 words of psyq_cd_x.c move). Only the
    marked function's block is taken from the flagged run, so neither
    problem arises. Same dual-source contract as its siblings."""
    return _per_function_list(src, "inline_functions.txt", INLFN_MARKER)


INLINE_CONTEXT_MARKER = re.compile(
    r"/\*\s*inline_context:\s*([A-Za-z_][\w \t]*?)\s*\*/")


def inline_context_names(text):
    """Functions an attempt defines ONLY as inline context, in order.

    An `inline_functions` function is compiled with the TU's functions
    above it in sight, and cc1 expands them in line. A standalone attempt
    therefore has to carry those definitions (station-tools adds them; a
    hand attempt may write the marker itself), but they are not part of
    what is scored: `/* inline_context: g h */` names them, and the scorer
    drops their blocks from the assembly again (strip_asm_functions), so
    a call that was NOT expanded (a recursion, a function too large)
    resolves to the tree's address as it does in the linked image."""
    out = []
    for m in INLINE_CONTEXT_MARKER.finditer(text):
        for n in m.group(1).split():
            if n not in out:
                out.append(n)
    return out


def strip_asm_functions(asm, names):
    """Drop the named functions' blocks from compiler assembly text
    -> (text, [names dropped]). A block runs from the function's `.globl`
    line (when it stands within three lines above its `.ent`) to its
    `.end`; a name with no `.ent` / `.end` pair is left alone and not
    reported. References to a dropped function become undefined symbols,
    which is the point: the harness resolves them from the symbol table."""
    lines = asm.split("\n")
    dropped = []
    for name in names:
        ent = next((i for i, ln in enumerate(lines)
                    if ln.split() == [".ent", name]), None)
        if ent is None:
            continue
        end = next((i for i in range(ent, len(lines))
                    if lines[i].split() == [".end", name]), None)
        if end is None:
            continue
        start = ent
        for k in range(ent - 1, max(ent - 4, -1), -1):
            if lines[k].split() == [".globl", name]:
                start = k
                break
        del lines[start:end + 1]
        dropped.append(name)
    return "\n".join(lines), dropped


#: Per-function COMPILER-FLAG passes: (name, reader, cc1 flags). Each
#: named function is compiled by a second cc1 run of the SAME cpp output
#: under the default flags + these, and its block spliced in
#: (delayfill.splice). A function named in several lists gets ONE alt
#: run under the union of their flags. Adding a flag lever = one row
#: here + a marker regex + a reader + the DIRECTIVE_COMMENT vocabulary.
ALT_FLAG_PASSES = (
    ("no_delayed_branch", no_delayed_branch_funcs, ("-fno-delayed-branch",)),
    ("no_split_addresses", no_split_addresses_funcs, ("-mno-split-addresses",)),
    ("no_schedule_insns", no_schedule_insns_funcs, ("-fno-schedule-insns",)),
    ("lib_g0", lib_g0_funcs, ("-G0",)),
    ("inline_functions", inline_functions_funcs, ("-finline-functions",)),
)


def epilogue_load_slot_funcs(src, scope_as=None):
    """Functions whose epilogue puts `addiu $sp` in the LOAD delay slot and
    leaves the RETURN slot nop — see delayfill.load_slot_epilogue.

    Per FUNCTION, never per file, because the original compiler emitted
    both epilogue forms for locally identical code; nothing in the asm
    decides it, so the only honest mechanism is to name the function and
    let `make` judge the bytes.

    Two sources, exactly like far_syms: a tracked per-program list at
    `config/epilogue_load_slot.txt`, and a file-local
    `/* epilogue_load_slot: FUNC [FUNC...] */` marker so a generated
    attempt can carry its own evidence and keep meaning the same thing
    once pasted in-tree."""
    return _per_function_list(src, "epilogue_load_slot.txt", EPI_MARKER)


def return_slot_fill_funcs(src, scope_as=None):
    """Functions whose RETURN delay slot holds the preceding instruction —
    see delayfill.return_slot_fill.

    Per FUNCTION, exactly like epilogue_load_slot and for the same reason:
    `@delay-fill` refuses return slots outright (filling them blanket-style
    reproduces the default epilogue), so the only way to reach the shape
    where the original DID fill one is to name the function and let `make`
    judge the bytes.

    Two sources, same as far_syms and epilogue_load_slot: a tracked
    per-program list at `config/return_slot_fill.txt`, and a file-local
    `/* return_slot_fill: FUNC [FUNC...] */` marker so a generated attempt
    carries its own evidence into an in-tree paste."""
    return _per_function_list(src, "return_slot_fill.txt", RET_MARKER)


def run_compiler(src, dst, extra_cpp=(), scope_as=None):
    cpp = subprocess.run(CPP + list(extra_cpp) + [src], capture_output=True)
    if cpp.returncode:
        sys.stderr.buffer.write(cpp.stderr)
        sys.exit(f"{src}: cpp failed")
    cc1 = subprocess.run(cc1_for(scope_as or src)
                         + extra_cc1_flags(scope_as or src),
                         input=cpp.stdout, capture_output=True)
    sys.stderr.buffer.write(cc1.stderr)
    if cc1.returncode:
        sys.exit(f"{src}: cc1 failed")
    out = _maspsx(cc1.stdout, src, "")
    gnuas = gnu_as_loads_funcs(src, scope_as)
    if gnuas:
        # An ASSEMBLER expansion, so before every delay-slot pass; every
        # alt pass below gets the same rewrite at the same point.
        out = gnu_as_loads(out.decode("utf-8", "replace"), gnuas)[0].encode()
    gdiv = gnu_as_div_funcs(src, scope_as)
    if gdiv:
        # GNU as's div macro has no nop before a use of its result
        # (delayfill.div_result_nop). The same kind of fact as the loads
        # above — an assembler expansion — so it sits at the same point in
        # the primary output and in every alt pass below.
        import delayfill
        out = delayfill.div_result_nop(out.decode("utf-8", "replace"),
                                       gdiv)[0].encode()
    if wants_delay_fill(scope_as or src):
        # ASPSX filled ordinary branch delay slots; maspsx never moves an
        # instruction. Opt-in per file — see delayfill.py.
        import delayfill
        filled, _n = delayfill.fill(out.decode("utf-8", "replace"))
        out = filled.encode()
    # Per-function compiler flags (ALT_FLAG_PASSES): cc1 has no
    # per-function flag control, so each distinct flag set among the
    # marked functions gets a SECOND cc1 run of the same cpp output and
    # those functions' blocks are spliced in (delayfill.splice — label
    # renames + loud refusals). AFTER the file-wide fill() so it cannot
    # refill deliberately-empty slots; BEFORE the per-function slot
    # passes so those can still name a spliced function.
    want = {}
    for name, reader, flags in ALT_FLAG_PASSES:
        for f in reader(src, scope_as):
            want.setdefault(f, []).extend(x for x in flags
                                          if x not in want.get(f, []))
    if want:
        import delayfill
        groups = {}
        for f, flags in want.items():
            groups.setdefault(tuple(flags), set()).add(f)
        for flags, funcs in sorted(groups.items()):
            alt_cc1 = subprocess.run(
                cc1_for(scope_as or src)
                + extra_cc1_flags(scope_as or src) + list(flags),
                input=cpp.stdout, capture_output=True)
            if alt_cc1.returncode:
                sys.stderr.buffer.write(alt_cc1.stderr)
                sys.exit(f"{src}: cc1 ({' '.join(flags)} alt pass) failed")
            alt_mas_out = _maspsx(alt_cc1.stdout, src,
                                  f" (alt pass {' '.join(flags)})")
            alt_text = alt_mas_out.decode("utf-8", "replace")
            if gnuas:
                alt_text = gnu_as_loads(alt_text, gnuas & funcs)[0]
            if gdiv & funcs:
                alt_text = delayfill.div_result_nop(alt_text, gdiv & funcs)[0]
            if wants_delay_fill(scope_as or src) \
                    and "-fno-delayed-branch" not in flags:
                # A @delay-fill TU fills the PRIMARY output above; an alt
                # block that keeps delayed branches must be filled the
                # same way or the splice silently loses the TU's fill
                # (spu_cmd_01_sub_5488 under no_split_addresses, 2026-10-06).
                # -fno-delayed-branch blocks stay unfilled BY DESIGN.
                alt_text, _n = delayfill.fill(alt_text)
            text, spliced, refused = delayfill.splice(
                out.decode("utf-8", "replace"), alt_text, funcs)
            for f, why in sorted(refused.items()):
                print(f"WARN: {' '.join(flags)}: {f}: {why}", file=sys.stderr)
            out = text.encode()
    cc97_lib = cc97_lib_funcs(src, scope_as)
    cc272_lib45 = cc272_lib45_funcs(src, scope_as) - cc97_lib
    cc272_lib = cc272_lib_funcs(src, scope_as) - cc272_lib45 - cc97_lib
    cc272_db = (cc272_db_funcs(src, scope_as) - cc272_lib - cc272_lib45
                - cc97_lib)
    cc272 = (cc272_funcs(src, scope_as) - cc272_db - cc272_lib
             - cc272_lib45 - cc97_lib)
    for _all, _label in ((cc272, "cc272"), (cc272_db, "cc272_db"),
                         (cc272_lib, "cc272_lib"),
                         (cc272_lib45, "cc272_lib45"),
                         (cc97_lib, "cc97_lib")):
        if not _all:
            continue
        # Per-function compiler swap: same splice mechanism as
        # no_delayed_branch, but the alt pass uses GCC 2.7.2. See
        # cc272_funcs for why this cannot be per file.
        # cc272_db: GCC 2.7.2 schedules its own delay slots — the TU's
        # -fno-delayed-branch (a 2.8.1-era @delay-fill recipe) is dropped
        # cc272_lib: the whole library recipe (cc272_lib_funcs)
        # cc272_lib45: the SDK 4.5+ library recipe (cc272_lib45_funcs)
        # cc97_lib: the same library on its own compiler (cc97_lib_funcs)
        import delayfill
        _cc1 = CC1_272
        if _label == "cc272_lib45":
            _cc1 = CC1_272_SLOT4
            if not os.path.exists(_cc1[0]):
                sys.exit(f"{src}: cc272_lib45 needs {_cc1[0]}, which "
                         f"recipe/toolchain.sh does not build: no function "
                         f"in this repository used the recipe when it was "
                         f"written (named: {', '.join(sorted(_all))})")
        if _label == "cc97_lib":
            _cc1 = CC1_970318
            if not os.path.exists(_cc1[0]):
                sys.exit(f"{src}: cc97_lib needs {_cc1[0]}, which is not "
                         f"there — build it with recipe/toolchain.sh "
                         f"(named: {', '.join(sorted(_all))})")
        _fl = extra_cc1_flags(scope_as or src)
        if _label != "cc272":
            _fl = [x for x in _fl if x != "-fno-delayed-branch"]
        if _label in ("cc272_lib", "cc272_lib45"):
            _fl = _fl + ["-fno-strength-reduce"]
        if _label == "cc272_lib45":
            _fl = _fl + ["-mhard-float"]
        if _label == "cc97_lib":
            _fl = _fl + ["-mhard-float", "-fno-exceptions"]
        # A function's per-function FLAG markers (ALT_FLAG_PASSES — e.g.
        # no_schedule_insns) reach its 2.7.2 pass too: one run per
        # distinct flag set. Before 2026-10-08 this pass ran on _fl alone,
        # so the splice below silently dropped a marked flag for any
        # cc272* function. A function with no flag marker is in the ()
        # group, compiled by exactly the command it always was.
        # 2.8-only flags are dropped: 2.7.2 rejects `-mno-split-addresses`
        # (cc1 exit 33, "Invalid option") and never splits a symbol address
        # in the first place (`lw $3,g` stays a macro), so the marker is
        # already satisfied there — spu_cmd_01_sub_5368 (cc272 +
        # no_split_addresses, landed) keeps its exact command.
        _groups = {}
        for f in _all:
            _ex = tuple(x for x in want.get(f, ()) if x not in CC1_272_ONLY_28)
            _groups.setdefault(_ex, set()).add(f)
        for _extra, _funcs in sorted(_groups.items()):
            _tag = _label + (f" {' '.join(_extra)}" if _extra else "")
            alt2 = subprocess.run(_cc1 + _fl + list(_extra),
                                  input=cpp.stdout, capture_output=True)
            if alt2.returncode:
                sys.stderr.buffer.write(alt2.stderr)
                sys.exit(f"{src}: cc1 2.7.2 ({_tag} alt pass) failed")
            alt2_mas_out = _maspsx(alt2.stdout, src, f" ({_tag} alt pass)")
            alt2_text = alt2_mas_out.decode("utf-8", "replace")
            if gnuas:
                alt2_text = gnu_as_loads(alt2_text, gnuas & _funcs)[0]
            if gdiv & _funcs:
                # every function of a library label is in gdiv; under
                # cc272 / cc272_db only the gnu_as_loads ones are
                alt2_text = delayfill.div_result_nop(alt2_text,
                                                     gdiv & _funcs)[0]
            if _label in ("cc272_lib", "cc272_lib45", "cc97_lib"):
                alt2_text, _n = delayfill.fill(alt2_text,
                                               skip_filled_slots=True)
                alt2_text, _n = delayfill.nop_before_back_label(alt2_text,
                                                                _funcs)
            text, spliced, refused = delayfill.splice(
                out.decode("utf-8", "replace"), alt2_text, _funcs)
            for f, why in sorted(refused.items()):
                print(f"WARN: {_tag}: {f}: {why}", file=sys.stderr)
            out = text.encode()
    rf = reorder_fill_funcs(src, scope_as)
    if rf:
        # The assembler's fill over what reorg left empty, AFTER every
        # splice (the function's final compiler output) and BEFORE the
        # targeted slot passes; named functions only (delayfill.reorder_fill).
        import delayfill
        text, _n = delayfill.reorder_fill(out.decode("utf-8", "replace"), rf)
        out = text.encode()
    argslot = arg_slot_fill_funcs(src, scope_as)
    if argslot:
        import delayfill
        text, n = delayfill.arg_slot_fill(out.decode("utf-8", "replace"),
                                          argslot)
        out = text.encode()
    epi = epilogue_load_slot_funcs(src, scope_as)
    if epi:
        # Independent of @delay-fill: this EMPTIES a return slot rather
        # than filling one, and applies to named functions only.
        import delayfill
        swapped, _n = delayfill.load_slot_epilogue(
            out.decode("utf-8", "replace"), epi)
        out = swapped.encode()
    ret = return_slot_fill_funcs(src, scope_as)
    if ret:
        # The inverse of the block above: FILLS a return slot rather than
        # emptying one. Named functions only — @delay-fill cannot do this.
        import delayfill
        filled, _n = delayfill.return_slot_fill(
            out.decode("utf-8", "replace"), ret)
        out = filled.encode()
    lhn = load_hazard_nop_funcs(src, scope_as)
    if lhn:
        # ASPSX's load-delay nop before a bare-symbol store of the loaded
        # register — the extern-gp case maspsx cannot see. Named
        # functions only; see delayfill.load_hazard_nop.
        import delayfill
        nopped, _n = delayfill.load_hazard_nop(
            out.decode("utf-8", "replace"), lhn,
            far=load_far_syms(scope_as or src) | marker_far_syms(src))
        out = nopped.encode()
    laa = la_absolute_funcs(src, scope_as)
    if laa:
        # ASPSX's absolute `la` (lui/addiu on one register) for small-data
        # symbols GNU as -G8 would make gp-relative; named functions only
        # (delayfill.la_absolute).
        import delayfill
        expanded, _n = delayfill.la_absolute(
            out.decode("utf-8", "replace"), laa)
        out = expanded.encode()
    zr = zero_remat_funcs(src, scope_as)
    if zr:
        # Copies of known-zero registers -> independent $zero
        # materializations; named functions only (delayfill.zero_remat —
        # the class no C shape can express).
        import delayfill
        rz, _n = delayfill.zero_remat(out.decode("utf-8", "replace"), zr)
        out = rz.encode()
    with open(dst, "wb") as f:
        f.write(strip_far_externs(out,
                                  load_far_syms(scope_as or src)
                                  | marker_far_syms(src)))


def selftest():
    """Fixture test of the dual-source per-function readers and
    carry_markers. Exists because the config-file half of the
    no_delayed_branch reader shipped with NO live user (the first opt-in
    chose the in-source marker) — an untested primary interface breaks
    silently on its first real entry. Pure fixtures, no program tree."""
    import tempfile
    bad = 0

    def check(name, got, want):
        nonlocal bad
        ok = got == want
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (got {got!r}, want {want!r})"))
        bad += not ok

    with tempfile.TemporaryDirectory() as td:
        srcp = os.path.join(td, "attempt.c")
        with open(os.path.join(td, "no_delayed_branch.txt"), "w") as f:
            f.write("# header comment\nfunc_A func_B  # trailing\n\nfunc_C\n")
        with open(srcp, "w") as f:
            f.write("/* no_delayed_branch: func_D func_E */\n"
                    "void func_D(void) {}\n")
        check("config + marker union, comments stripped",
              _per_function_list(srcp, "no_delayed_branch.txt", NODB_MARKER,
                                 config_dir=td),
              {"func_A", "func_B", "func_C", "func_D", "func_E"})
        check("missing config file -> marker only",
              _per_function_list(srcp, "missing.txt", NODB_MARKER,
                                 config_dir=td),
              {"func_D", "func_E"})
        with open(srcp, "w") as f:
            f.write("void f(void) {}\n")
        check("no marker -> config only",
              _per_function_list(srcp, "no_delayed_branch.txt", NODB_MARKER,
                                 config_dir=td),
              {"func_A", "func_B", "func_C"})
        # the older two lists ride the SAME helper now — one probe each
        with open(os.path.join(td, "return_slot_fill.txt"), "w") as f:
            f.write("func_R\n")
        check("return_slot_fill config path",
              _per_function_list(srcp, "return_slot_fill.txt", RET_MARKER,
                                 config_dir=td), {"func_R"})
        with open(srcp, "w") as f:
            f.write("/* epilogue_load_slot: func_P */\nvoid f(void) {}\n")
        check("epilogue_load_slot marker path",
              _per_function_list(srcp, "absent.txt", EPI_MARKER,
                                 config_dir=td), {"func_P"})

        with open(srcp, "w") as f:
            f.write("/* no_split_addresses: func_S */\n"
                    "/* no_schedule_insns: func_T func_U */\nvoid f(void) {}\n")
        check("no_split_addresses marker path",
              no_split_addresses_funcs(srcp), {"func_S"})
        check("no_schedule_insns marker path",
              no_schedule_insns_funcs(srcp), {"func_T", "func_U"})
        check("every ALT_FLAG_PASSES marker is in the carry vocabulary",
              [n for n, _r, _f in ALT_FLAG_PASSES
               if not DIRECTIVE_COMMENT.search(f"/* {n}: f */")], [])
        with open(srcp, "w") as f:
            f.write("/* lib_g0: func_V */\n"
                    "/* inline_functions: func_W func_X */\nvoid f(void) {}\n")
        check("lib_g0 marker path", lib_g0_funcs(srcp), {"func_V"})
        check("inline_functions marker path",
              inline_functions_funcs(srcp), {"func_W", "func_X"})
        check("lib_g0 / inline_functions rows carry -G0 / -finline-functions",
              {n: fl for n, _r, fl in ALT_FLAG_PASSES
               if n in ("lib_g0", "inline_functions")},
              {"lib_g0": ("-G0",),
               "inline_functions": ("-finline-functions",)})
        check("neither flag is dropped from the cc272* / cc97_lib passes "
              "(CC1_272_ONLY_28)",
              [fl for fl in ("-G0", "-finline-functions")
               if fl in CC1_272_ONLY_28], [])
        check("a lib_g0 marker is not an inline_functions marker, nor the "
              "reverse",
              (inline_functions_funcs(srcp) & {"func_V"},
               lib_g0_funcs(srcp) & {"func_W", "func_X"}), (set(), set()))
        _ctx_asm = (".text\n.align\t2\n.globl\tg\n.text\n.ent\tg\ng:\n"
                    "jr\t$31\n.end\tg\n.align\t2\n.globl\tf\n.ent\tf\nf:\n"
                    "jal\tg\njr\t$31\n.end\tf\n")
        _ctx_out, _ctx_dropped = strip_asm_functions(_ctx_asm, ["g", "zz"])
        check("inline_context marker names, in order, once each",
              inline_context_names("/* inline_context: g h */\nint x;\n"
                                   "/* inline_context: g k */\n"),
              ["g", "h", "k"])
        check("an inline_functions marker is not an inline_context marker",
              inline_context_names("/* inline_functions: f */\n"), [])
        check("strip_asm_functions drops the named block with its .globl, "
              "keeps the caller and its call, ignores an absent name",
              (_ctx_dropped, "g:" in _ctx_out, ".globl\tg" in _ctx_out,
               "f:" in _ctx_out, "jal\tg" in _ctx_out,
               ".end\tf" in _ctx_out),
              (["g"], False, False, True, True, True))
        check("strip_asm_functions with nothing to drop returns the text",
              strip_asm_functions(_ctx_asm, []), (_ctx_asm, []))
        check("carry_markers keeps both new markers",
              carry_markers("/* lib_g0: f */\n/* inline_functions: g */\n"
                            "int x;\n", "int x;\n").startswith(
                  "/* lib_g0: f */\n/* inline_functions: g */"), True)

        with open(srcp, "w") as f:
            f.write("/* cc272_lib45: func_L */\n/* gnu_as_loads: func_G */\n"
                    "void f(void) {}\n")
        check("cc272_lib45 marker path", cc272_lib45_funcs(srcp), {"func_L"})
        check("gnu_as_loads marker path", gnu_as_loads_funcs(srcp),
              {"func_G"})
        check("a cc272_lib45 marker is NOT a cc272_lib / cc272 marker",
              (_per_function_list(srcp, "absent.txt", CC272LIB_MARKER,
                                  config_dir=td),
               _per_function_list(srcp, "absent.txt", CC272_MARKER,
                                  config_dir=td)), (set(), set()))
        check("new markers are in the carry vocabulary",
              [n for n in ("cc272_lib45", "gnu_as_loads")
               if not DIRECTIVE_COMMENT.search(f"/* {n}: f */")], [])

        with open(srcp, "w") as f:
            f.write("/* cc97_lib: func_M */\nvoid f(void) {}\n")
        check("cc97_lib marker path", cc97_lib_funcs(srcp), {"func_M"})
        check("a cc97_lib marker is NOT a cc272* marker",
              [m.pattern for m in (CC272_MARKER, CC272DB_MARKER,
                                   CC272LIB_MARKER, CC272LIB45_MARKER)
               if _per_function_list(srcp, "absent.txt", m,
                                     config_dir=td)], [])
        check("cc97_lib is in the carry vocabulary",
              bool(DIRECTIVE_COMMENT.search("/* cc97_lib: f */")), True)

        # GNU as's div macro (delayfill.div_result_nop): the library
        # recipes and gnu_as_loads name the functions — nothing else does
        _kill = os.environ.pop("GNU_AS_DIV_DISABLE", None)
        with open(srcp, "w") as f:
            f.write("/* cc272_lib: func_A */\n/* cc272_lib45: func_B */\n"
                    "/* cc97_lib: func_C */\n/* gnu_as_loads: func_D */\n"
                    "/* cc272: func_E */\n/* cc272_db: func_F */\n"
                    "/* return_slot_fill: func_G */\nvoid func_H(void) {}\n")
        check("gnu_as_div: the three library recipes + gnu_as_loads, "
              "not cc272 / cc272_db / a slot marker / an unmarked function",
              gnu_as_div_funcs(srcp),
              {"func_A", "func_B", "func_C", "func_D"})
        os.environ["GNU_AS_DIV_DISABLE"] = "1"
        check("gnu_as_div: GNU_AS_DIV_DISABLE=1 names nothing (the bisect "
              "switch)", gnu_as_div_funcs(srcp), set())
        del os.environ["GNU_AS_DIV_DISABLE"]
        with open(srcp, "w") as f:
            f.write("void func_H(void) {}\n")
        check("gnu_as_div: an unmarked file names nothing",
              gnu_as_div_funcs(srcp), set())
        if _kill is not None:
            os.environ["GNU_AS_DIV_DISABLE"] = _kill

    # LIVE vocabulary probe: div_result_nop keys on two strings the VENDORED
    # maspsx prints (`# EXPAND_DIV END`, `nop # DEBUG: Reuse of`). A maspsx
    # upgrade that renames either would turn the pass into a silent no-op,
    # so run the real thing on a direct use (f) and a use behind a label (g).
    import delayfill
    probe = ("\t.text\n\t.ent\tf\nf:\n\tdiv\t$2,$2,$4\n\tsh\t$2,16($sp)\n"
             "\tj\t$31\n\t.end\tf\n\t.ent\tg\ng:\n\tdivu\t$3,$3,$5\n$L5:\n"
             "\tsh\t$3,18($sp)\n\tj\t$31\n\t.end\tg\n")
    mas = subprocess.run(MASPSX, input=probe.encode(), capture_output=True)
    mtxt = mas.stdout.decode("utf-8", "replace")
    check("maspsx still prints the div macro's END comment and reuse nop "
          "(2 sites in the probe)",
          (mas.returncode,
           len(re.findall(r"^# EXPAND_DIVU? END\n(?:\$L5:\n)?nop # DEBUG: "
                          r"Reuse of ", mtxt, re.M))), (0, 2))
    both, nb = delayfill.div_result_nop(mtxt, {"f", "g"})
    one, n1 = delayfill.div_result_nop(mtxt, {"f"})
    check("div_result_nop on maspsx's own output: both sites dropped, the "
          "label kept; naming f alone leaves g's nop",
          (nb, n1,
           "# EXPAND_DIV END\nsh\t$2,16($sp)" in both,
           "# EXPAND_DIVU END\n$L5:\n# $L5:  # DEBUG: skipped\nsh\t$3,18($sp)"
           in both,
           "# EXPAND_DIVU END\n$L5:\nnop # DEBUG: Reuse of '$3'" in one),
          (2, 1, True, True, True))

    def _blk(op, dest, base):
        return ("# EXPAND_AT START\n.set\tnoat\nlui\t$at,%hi(S+4)\n"
                f"addu\t$at,$at,{base}\n{op}\t{dest},%lo(S+4)($at)\n"
                ".set\tat\n# EXPAND_AT END")
    fx = ("\t.ent\tg\n" + _blk("lw", "$2", "$4") + "\n\t.end\tg\n"
          "\t.ent\th\n" + _blk("lw", "$2", "$4") + "\n\t.end\th\n")
    got, n = gnu_as_loads(fx, {"g"})
    check("gnu_as_loads: lw $2,S($4) -> destination as the temporary",
          ("lui\t$2,%hi(S+4)\naddu\t$2,$2,$4\nlw\t$2,%lo(S+4)($2)" in got,
           n), (True, 1))
    check("gnu_as_loads: an unnamed function keeps $at",
          got.split("\t.ent\th\n")[1] == _blk("lw", "$2", "$4")
          + "\n\t.end\th\n", True)
    for why, op, dest, base in (
            ("destination is the base", "lw", "$4", "$4"),
            ("same register, two spellings", "lw", "$v0", "$2"),
            ("lwl merges into its destination", "lwl", "$2", "$4"),
            ("coprocessor load", "lwc2", "$2", "$4"),
            ("a store always uses $at", "sw", "$2", "$4")):
        t = "\t.ent\tg\n" + _blk(op, dest, base) + "\n\t.end\tg\n"
        check(f"gnu_as_loads keeps $at: {why}", gnu_as_loads(t, {"g"}),
              (t, 0))
    check("gnu_as_loads: lbu/lh/lhu/lb rewrite too",
          [gnu_as_loads("\t.ent\tg\n" + _blk(op, "$3", "$5")
                        + "\n\t.end\tg\n", {"g"})[1]
           for op in ("lb", "lbu", "lh", "lhu")], [1, 1, 1, 1])
    check("gnu_as_loads: no functions named -> identity",
          gnu_as_loads(fx, set()), (fx, 0))

    def _nblk(op, dest, base, order):
        return ("# EXPAND_AT START\n.set\tnoat\nlui\t$at,%hi(528486536)\n"
                f"addu\t$at,{order.format(base=base)}\n"
                f"{op}\t{dest},%lo(528486536)($at)\n.set\tat\n# EXPAND_AT END")
    aspsx, gnu = "{base},$at", "$at,{base}"
    check("gnu_as_loads: a NUMERIC offset, lw $2,K($5) -> destination as "
          "the temporary",
          gnu_as_loads("\t.ent\tg\n" + _nblk("lw", "$2", "$5", aspsx)
                       + "\n\t.end\tg\n", {"g"}),
          ("\t.ent\tg\n# EXPAND_AT START\n.set\tnoat\n"
           "lui\t$2,%hi(528486536)\naddu\t$2,$2,$5\n"
           "lw\t$2,%lo(528486536)($2)\n.set\tat\n# EXPAND_AT END\n"
           "\t.end\tg\n", 1))
    for why, op, dest in (("destination is the base", "lw", "$5"),
                          ("a store", "sw", "$2"),
                          ("lwl", "lwl", "$2")):
        check("gnu_as_loads: a NUMERIC offset keeps $at and adds the base "
              f"second: {why}",
              gnu_as_loads("\t.ent\tg\n" + _nblk(op, dest, "$5", aspsx)
                           + "\n\t.end\tg\n", {"g"}),
              ("\t.ent\tg\n" + _nblk(op, dest, "$5", gnu)
               + "\n\t.end\tg\n", 1))
    t = "\t.ent\th\n" + _nblk("lw", "$2", "$5", aspsx) + "\n\t.end\th\n"
    check("gnu_as_loads: a NUMERIC offset in an unnamed function stays "
          "ASPSX's", gnu_as_loads(t, {"g"}), (t, 0))

    seed = "/* far_syms: D_1 */\n/* no_delayed_branch: f */\nint x;\n"
    stripped = "int x;\n"
    carried = carry_markers(seed, stripped)
    check("carry_markers restores every missing directive",
          carried.startswith("/* far_syms: D_1 */\n"
                             "/* no_delayed_branch: f */"), True)
    check("carry_markers is identity when nothing is missing",
          carry_markers(seed, carried) is carried, True)
    print("compile selftest:", "all pass" if not bad else f"{bad} FAILED")
    return bad


def main():
    argv = sys.argv[1:]
    if "--selftest" in argv:
        sys.exit(1 if selftest() else 0)
    scope_as = None
    if "--scope-as" in argv:
        # compile a SCRATCH COPY under another file's identity: cc_flags
        # and TU-scoped far_syms resolve by path, so a rehearsal copy in
        # build/ would otherwise lose the real TU's -fno-delayed-branch
        # and @-scoped entries
        i = argv.index("--scope-as")
        scope_as = argv[i + 1]
        del argv[i:i + 2]
    src, dst = argv[0], argv[1]
    with open(src) as f:
        text = f.read()
    if is_pure_stub(text):
        emit_stub_asm(text, dst)
    else:
        run_compiler(src, dst, scope_as=scope_as)


if __name__ == "__main__":
    main()
