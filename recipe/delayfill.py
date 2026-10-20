#!/usr/bin/env python3
"""ASPSX-style branch delay slot filling, as a post-maspsx pass.

Why this exists: `-fno-delayed-branch` gets the epilogue right for the PSY-Q
objects but leaves every OTHER delay slot empty, and the originals have them
filled. cc1 emits those objects in
`.set reorder` — the ASSEMBLER was expected to fill ordinary delay slots.
maspsx never moves an instruction, it only inserts nops, so ours stay empty.

The rule, which explains both observed cases: **move the instruction
immediately preceding the branch into its empty delay slot, when safe.**

    gpu_prim_setup_12   ... sw $a3 / jal / nop   -> ... jal / sw $a3   (moved)
    CdSync              ... sw $ra / jal / nop   -> unchanged (see below)

CdSync is the one that proves the safety rule matters: `jal` WRITES $ra, so
moving `sw $ra,N($sp)` into the delay slot would store the *return address*
instead of the caller's $ra. The original assembler refused it too — that is
exactly why CdSync's delay slot is a nop in the target.

maspsx lives in toolchain/, which is not in git, so this cannot be a patch
to maspsx — it is a pass over maspsx's output, opt-in per file via
`@delay-fill` in decomp/config/cc_flags.txt. Opt-in matters: files whose cc1
already filled its delay slots must not be touched, and every currently
matching function depends on that.

    python3 recipe/delayfill.py --selftest
"""
import re
import sys

BRANCH = {"j", "jal", "jr", "jalr", "b", "beq", "bne", "blez", "bgtz",
          "bltz", "bgez", "beqz", "bnez", "bal", "bltzal", "bgezal"}
WRITES_RA = {"jal", "jalr", "bal", "bltzal", "bgezal"}
# Moved only from this whitelist. Loads are deliberately excluded: maspsx has
# already inserted load-delay nops by this point, and relocating a load can
# invalidate that analysis.
MOVABLE = {"addiu", "addu", "subu", "and", "or", "xor", "nor", "andi", "ori",
           "xori", "sll", "srl", "sra", "sllv", "srlv", "srav", "lui",
           "slt", "sltu", "slti", "sltiu", "move", "li", "mfhi", "mflo",
           "sb", "sh", "sw", "negu", "not"}
NO_DEST = {"sb", "sh", "sw", "swl", "swr"}          # first operand is a source
LOADS = {"lb", "lbu", "lh", "lhu", "lw", "lwl", "lwr"}  # load_hazard_nop's A side
# `sw $4,SYM` / `sh $2,SYM+6` with no base register — a GAS macro, not a
# real instruction (group 2 = the whole address expression, group 3 = the
# bare symbol). The offset form is a member of a far global struct
# (`_spu_rev_attr.depth.right`): without it fill() moved the macro whole and
# GAS expanded it inside the slot (`j / lui $at / sh`: one row off, and the
# store never runs) — SLUS_006.28 SpuSetReverbModeParam, func_8009F618,
# 308/309 -> MATCH. Use _bare_far(), which refuses the offset form on a
# small-data symbol.
BARE_MEM = re.compile(
    r"^(\$\w+)\s*,\s*(([A-Za-z_]\w*)(?:\s*[+-]\s*(?:0x[0-9A-Fa-f]+|\d+))?)\s*$")
# load_hazard_nop's store side also takes `SYM+off` (an element of a small
# gp array: `sh $2,D_800B8EFC+2` — func_80074ADC, the post-`jal` reload).
BARE_MEM_OFF = re.compile(
    r"^(\$\w+)\s*,\s*([A-Za-z_]\w*)(?:\s*[+-]\s*(?:0x[0-9A-Fa-f]+|\d+))?\s*$")
# fill()'s and return_slot_fill's INDEXED far store: `sb $2,SYM($3)` /
# `sh $2,SYM+8($6)`
# (group 2 = the whole address expression, 3 = the bare symbol, 4 = the base).
INDEXED_MEM = re.compile(
    r"^(\$\w+)\s*,\s*(([A-Za-z_]\w*)(?:\s*[+-]\s*(?:0x[0-9A-Fa-f]+|\d+))?)"
    r"\s*\(\s*(\$\w+)\s*\)\s*$")
# fill(split_la=True)'s candidate: `la $r,SYM[+off]` (group 2 = the whole
# address expression, group 3 = the bare symbol).
LA_BARE = re.compile(
    r"^(\$\w+)\s*,\s*(([A-Za-z_]\w*)(?:\s*[+-]\s*(?:0x[0-9A-Fa-f]+|\d+))?)\s*$")
SMALL_DECL = re.compile(
    r"^\s*\.(?:extern|comm|lcomm)\s+([A-Za-z_]\w*)\s*,\s*(0x[0-9A-Fa-f]+|\d+)",
    re.M)

# maspsx emits unindented lines, numeric registers ($31 not $ra) and trailing
# `# DEBUG:` comments — all three broke the first version of this parser.
INSN = re.compile(r"^(\s*)([a-z][a-z0-9.]*)\b[ \t]*([^#]*?)\s*(?:#.*)?$")
REG = re.compile(r"\$(?:[a-z0-9]+)")
_NAMES = (["zero", "at", "v0", "v1"] + [f"a{i}" for i in range(4)]
          + [f"t{i}" for i in range(8)] + [f"s{i}" for i in range(8)]
          + ["t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"])
_NUM2NAME = {f"${i}": f"${n}" for i, n in enumerate(_NAMES)}


def _norm(reg):
    """`$31` and `$ra` are the same register; maspsx writes the number."""
    return _NUM2NAME.get(reg, reg)


def _regs(operands):
    return [_norm(r) for r in REG.findall(operands)]


def is_label(line):
    s = line.strip()
    return bool(s) and not s.startswith(("#", "/*")) and s.endswith(":")


def _parse(line):
    """(indent, mnemonic, operands) or None for labels/directives/comments."""
    s = line.strip()
    if not s or s.startswith(("#", "/*", ".")) or is_label(line):
        return None
    m = INSN.match(line)
    if not m:
        return None
    return m.group(1), m.group(2), m.group(3)


def _reads(mnem, ops):
    regs = _regs(ops)
    if not regs:
        return set()
    if mnem in NO_DEST or mnem in BRANCH:
        return set(regs)
    return set(regs[1:])


def _writes(mnem, ops):
    regs = _regs(ops)
    if mnem in WRITES_RA:
        return {"$ra"}
    if mnem in NO_DEST or mnem in BRANCH or not regs:
        return set()
    return {regs[0]}


def _small_syms(text):
    """Symbols this asm declares small (`.extern/.comm/.lcomm SYM,N`, N<=8):
    GNU as -G8 turns an `la` of one into a single `addiu $r,$gp`, so it
    has no halves to split (la_absolute is the lever for those)."""
    return {m.group(1) for m in SMALL_DECL.finditer(text)
            if int(m.group(2), 0) <= 8}


def _bare_far(mnem, operands, small):
    """The BARE_MEM match of a far-store macro `sX $r,SYM[+off]`, or None.
    `SYM+off` on a symbol declared small is an element of a small-data
    array: GNU as -G8 assembles it as ONE gp-relative instruction, so it
    has no `lui` half to leave behind and is moved whole like any other
    instruction. (The offset-free form on a small symbol never reaches
    this pass: maspsx has already written it `%gp_rel(SYM)($gp)`.)"""
    if mnem not in NO_DEST:
        return None
    m = BARE_MEM.match(operands)
    if m and m.group(2) != m.group(3) and m.group(3) in small:
        return None
    return m


def fill(text, skip_filled_slots=False, split_la=False, small=None):
    """-> (new_text, n_filled).

    skip_filled_slots=True is for input whose compiler ALREADY scheduled
    its own delay slots (the cc272_lib recipe: GCC 2.7.2 with delayed
    branches, then ASPSX's fill over what it left empty). maspsx output
    carries no `.set noreorder` brackets, so a filled slot looks like an
    ordinary instruction before the next branch: `jal f / addu $5,.. /
    j L / nop` would move the jal's slot into the j's — invalid code.
    With the flag, a candidate whose previous real instruction is a branch
    is left where it is. Off by default: every existing caller feeds
    -fno-delayed-branch output, where no slot is filled yet.

    split_la=True also takes an `la $r,SYM` macro as the candidate, the way
    a bare-memory store already is: its `lui $r,%hi(SYM)` half stays
    before the branch and `addiu $r,$r,%lo(SYM)` goes into the slot —
    the target's `lui $a0,%hi(fmt) / jal printf / addiu $a0,%lo(fmt)`.
    Off by default because no landed function needed it: in the target,
    the shape occurs ONLY in unconverted functions (43 of them, 2026-10-08:
    libcd's DiskError/CD_sync/CD_ready/CD_cw, libapi, libspu, ...), and no
    converted function's target has an unsplit `la` before an empty slot
    either, so turning it on cannot have been what any match relied on.
    A symbol declared small is refused (see _small_syms; `small` passes
    the set in when `text` is only part of the file)."""
    lines = text.split("\n")
    # GNU as's prev_prev_insn_unreordered rule (tc-mips.c append_insn): the
    # instruction right after a `.set noreorder` region is never swapped
    # into the next branch's slot. cc1 brackets every delay slot IT filled
    # that way, and maspsx drops the brackets, so the region is recovered
    # from the INPUT: a non-nop instruction directly after a branch is a
    # compiler-filled slot, and the single instruction after it is pinned.
    # Slots this pass fills are reorder-mode swaps and pin nothing, hence
    # the flags are computed once, up front, and kept in step with `lines`.
    pinned = [False] * len(lines)
    if skip_filled_slots:
        pp = pv = None                      # the two previous instructions
        for k, ln in enumerate(lines):
            q = _parse(ln)
            if not q:
                continue
            if (pp is not None and pv is not None and pp[1] in BRANCH
                    and pv[1] != "nop" and q[1] not in BRANCH):
                pinned[k] = True
            pp, pv = pv, q
    small_idx = _small_syms(text) if small is None else small
    if not split_la:
        small = set()
    elif small is None:
        small = _small_syms(text)
    n = 0
    i = -1
    while True:
        i += 1                    # a fill removes one line, so re-check len
        if i >= len(lines):
            break
        p = _parse(lines[i])
        if not p or p[1] not in BRANCH:
            continue
        # NEVER fill a function return's delay slot. In every 'E' object the
        # return slot is a nop with the stack adjust in front of it — that is
        # the whole reason these files are built -fno-delayed-branch. Filling
        # it moves `addiu $sp` back into the slot and reproduces the DEFAULT
        # epilogue, which is precisely what we are trying not to emit.
        # (Measured: without this, gpu_prim_setup_12 went 8/9 -> 7/9.)
        # cc1 spells the return `j $31`, maspsx keeps it; `jr $ra` also occurs.
        if p[1] in ("jr", "j") and _regs(p[2]) == ["$ra"]:
            continue
        # the delay slot is the next real instruction (maspsx interleaves
        # `#nop # DEBUG:` comment lines, which are not instructions)
        slot_i = next((k for k in range(i + 1, len(lines))
                       if _parse(lines[k])), None)
        if slot_i is None or _parse(lines[slot_i])[1] != "nop":
            continue
        # candidate: the previous real instruction, with no LABEL between it
        # and the branch — a label means other paths reach the branch without
        # executing it, so moving it would change those paths.
        cand_i = None
        for k in range(i - 1, -1, -1):
            if is_label(lines[k]):
                break
            if _parse(lines[k]):
                cand_i = k
                break
        if cand_i is None:
            continue
        cand = _parse(lines[cand_i])
        m_la = (LA_BARE.match(cand[2])
                if split_la and cand[1] == "la" else None)
        if m_la and m_la.group(3) in small:
            m_la = None
        if cand[1] not in MOVABLE and not m_la:
            continue
        # `sh $3,SYM+12($5)` is a GAS macro as well (lui $at / addu $at,$at,$5
        # / sh $3,%lo(SYM+12)($at)). GNU as's `.set reorder` swap takes only
        # the LAST instruction of the expansion: the two address halves stay
        # before the branch and the store itself goes into the slot. Moved
        # whole, GAS expands it inside the slot: `j / lui / addu / sh`, wrong
        # code and one row off (SLUS_006.28 GsSortClear, func_8008EF10:
        # 77/78 -> MATCH). A small-data symbol is refused (GAS -G8 expands
        # that one through $gp), and so is a base of $at.
        m_idx = INDEXED_MEM.match(cand[2]) if cand[1] in NO_DEST else None
        if m_idx and (m_idx.group(3) in small_idx
                      or _norm(m_idx.group(4)) == "$at"):
            continue
        if skip_filled_slots:
            prev_i = next((k for k in range(cand_i - 1, -1, -1)
                           if is_label(lines[k]) or _parse(lines[k])), None)
            if (prev_i is not None and not is_label(lines[prev_i])
                    and _parse(lines[prev_i])[1] in BRANCH):
                continue                 # the candidate IS a filled slot
        # a macro candidate (la / bare or indexed far store) is two or more
        # instructions: only its LAST one is swapped and the one before it
        # was assembled in reorder mode, so the pin does not apply to it
        if (pinned[cand_i] and not m_la and not m_idx
                and not _bare_far(cand[1], cand[2], small_idx)):
            continue
        br_reads, br_writes = _reads(p[1], p[2]), _writes(p[1], p[2])
        c_reads, c_writes = _reads(cand[1], cand[2]), _writes(cand[1], cand[2])
        # the branch must not depend on it, and it must not read anything the
        # branch clobbers ($ra for jal — the CdSync case)
        if (c_writes & br_reads) or (c_reads & br_writes):
            continue
        if m_la:
            # `la` is a macro too: the %hi half stays, the %lo half moves.
            reg, sym = m_la.group(1), m_la.group(2)
            ind = cand[0]
            lines[cand_i] = f"{ind}lui\t{reg},%hi({sym})"
            lines[slot_i] = f"{ind}addiu\t{reg},{reg},%lo({sym})"
            n += 1
            continue
        # `sw $5,SYM` is a GAS MACRO, not one instruction. Moving it WHOLE
        # makes GAS expand it inside the delay slot, so the `lui` half lands
        # in the slot and the store follows it — one row off the target,
        # which puts the `lui` BEFORE the branch. Expand it ourselves, the
        # same way return_slot_fill does. (Measured on SLUS_009.21
        # StSetRing/func_80124FE0: 10/12 -> MATCH.)
        if m_idx:
            reg, sym, base = m_idx.group(1), m_idx.group(2), m_idx.group(4)
            ind = cand[0]
            lines[slot_i] = f"{ind}{cand[1]}\t{reg},%lo({sym})($at)"
            lines[cand_i:cand_i + 1] = [f"{ind}lui\t$at,%hi({sym})",
                                        f"{ind}addu\t$at,$at,{base}"]
            pinned[cand_i:cand_i + 1] = [False, False]
            i += 1                       # the branch moved down one line
            n += 1
            continue
        m_bare = _bare_far(cand[1], cand[2], small_idx)
        if m_bare:
            reg, sym = m_bare.group(1), m_bare.group(2)
            ind = cand[0]
            lines[cand_i] = f"{ind}lui\t$at,%hi({sym})"
            lines[slot_i] = f"{ind}{cand[1]}\t{reg},%lo({sym})($at)"
            n += 1
            continue
        moved = lines.pop(cand_i)
        pinned.pop(cand_i)
        lines[slot_i - 1] = moved            # indices shifted down by the pop
        pinned[slot_i - 1] = False
        n += 1
    return "\n".join(lines), n


def reorder_fill(text, funcs):
    """The assembler's `.set reorder` slot fill over a DELAYED-BRANCH
    compile, in the named functions only. -> (new_text, n_filled).

    fill(skip_filled_slots=True, split_la=True) applied to each named
    function's .ent/.end span: cc1's reorg fills what it can, and the
    slots it left empty — a volatile store before a `j`, an `la` before a
    `jal` (reorg will not put a two-instruction macro in a slot) — get the
    preceding instruction, macro halves split, exactly as the cc272_lib
    recipe's pass does for 2.7.2 output. A compiler-independent post-pass,
    so it composes with any compiler marker. Measured on SLUS_006.28
    DiskError (libcd, 2026-10-08): its four `lui $a0 / jal / addiu $a0`
    printf calls and the `j / sb $0,0($v1)` tail of its DataReady case
    are the only rows the C does not decide."""
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
    small = _small_syms(text)          # declarations sit outside the spans
    total = 0
    for a, b in reversed(spans):
        body, k = fill("\n".join(lines[a:b + 1]), skip_filled_slots=True,
                       split_la=True, small=small)
        lines[a:b + 1] = body.split("\n")
        total += k
    return "\n".join(lines), total


SELFTEST = [
    # (name, asm, expected fills, expected delay-slot mnemonic)
    ("moves a safe store into a jal slot",
     "\tsw\t$a3,36($sp)\n\tjal\tfoo\n\tnop\n", 1, "sw"),
    ("refuses sw $ra before jal (jal clobbers $ra)",
     "\tsw\t$ra,16($sp)\n\tjal\tfoo\n\tnop\n", 0, "nop"),
    ("refuses when the branch reads the result",
     "\taddiu\t$v0,$v0,1\n\tbeqz\t$v0,.L1\n\tnop\n", 0, "nop"),
    ("moves an unrelated shift into a jal slot",
     "\tsra\t$a3,$a3,16\n\tjal\tfoo\n\tnop\n", 1, "sra"),
    ("refuses a load (maspsx already placed load-delay nops)",
     "\tlw\t$v0,0($a0)\n\tjal\tfoo\n\tnop\n", 0, "nop"),
    ("refuses across a label",
     "\taddiu\t$v0,$v0,1\n.L2:\n\tjal\tfoo\n\tnop\n", 0, "nop"),
    ("leaves an already-filled slot alone",
     "\taddiu\t$v0,$v0,1\n\tjal\tfoo\n\tmove\t$a0,$v1\n", 0, "move"),
    # the real maspsx output shape: unindented, numeric registers, and
    # `#nop # DEBUG:` comment lines between instructions. The first version
    # of the parser silently matched none of it.
    ("maspsx format: unindented, numeric regs",
     "sw\t$7,36($sp)\njal\tfoo\nnop  # DEBUG: branch/jump\n", 1, "sw"),
    ("maspsx format: refuses sw $31 before jal ($31 == $ra)",
     "sw\t$31,16($sp)\njal\tfoo\nnop  # DEBUG: branch/jump\n", 0, "nop"),
    ("skips maspsx #nop comment lines when finding the candidate",
     "sw\t$7,36($sp)\n#nop # DEBUG: x\njal\tfoo\nnop  # DEBUG: branch/jump\n",
     1, "sw"),
    # an INDEXED far store is a three-instruction macro: only the store
    # itself goes into the slot, the address halves stay before the branch.
    ("splits an indexed far store: the store alone goes into the slot",
     "sh\t$3,D_800E6F18+12($5)\nj\t$L3\nnop  # DEBUG: branch/jump\n", 1, "sh"),
    # the E epilogue must survive: filling the return slot rebuilds the
    # DEFAULT form and undoes -fno-delayed-branch entirely.
    ("never fills a return delay slot (jr $31)",
     "addu\t$sp,$sp,24\njr\t$31\nnop  # DEBUG: branch/jump\n", 0, "nop"),
    # cc1 actually spells the return `j $31`; checking only `jr` missed it
    # entirely and the epilogue got refilled.
    ("never fills a return delay slot (j $31, the real spelling)",
     "addu\t$sp,$sp,24\nj\t$31\nnop  # DEBUG: branch/jump\n", 0, "nop"),
    # a bare-memory store is a GAS macro: moving it whole puts its `lui` in
    # the slot instead of the store. It must be SPLIT, lui before the branch.
    ("splits a bare-memory store macro instead of moving it whole",
     "sw\t$5,D_801BE4C8\njal\tfoo\nnop  # DEBUG: branch/jump\n", 1, "sw"),
]


#: fill(skip_filled_slots=True): (name, asm, expected fills, expected
#: mnemonic in the delay slot of the LAST branch)
FILLED_SLOT_SELFTEST = [
    ("skip_filled_slots: a jal's filled slot is not stolen by the next j",
     "jal\tfoo\naddu\t$5,$2,$5\nj\t$L2\nnop  # DEBUG: branch/jump\n",
     0, "nop"),
    ("skip_filled_slots: an ordinary candidate is still moved",
     "jal\tfoo\naddu\t$5,$2,$5\nmove\t$6,$7\naddiu\t$4,$4,1\nj\t$L2\n"
     "nop  # DEBUG: branch/jump\n", 1, "addiu"),
    # GNU as never swaps the ONE instruction after a `.set noreorder` region
    # (a slot cc1 filled). Target rows: libgpu _exeque `beqz $a0 /
    # addiu $v0,$v1,-8 / sw $zero,8($v0) / jalr $a0 / nop` (SLUS_006.28
    # func_8008DC4C); the vendored GNU as 2.42 does the same.
    ("skip_filled_slots: the instruction after a compiler-filled slot stays",
     "beq\t$4,$0,$L13\naddu\t$2,$3,-8\nsw\t$0,8($2)\njal\t$31,$4\n"
     "nop  # DEBUG: branch/jump\n", 0, "nop"),
    ("skip_filled_slots: a far-store macro after a filled slot is still split",
     "jal\tfoo\nmove\t$4,$0\nsw\t$2,D_800B60D4\nbeq\t$4,$3,$L4\n"
     "nop  # DEBUG: branch/jump\n", 1, "sw"),
    ("skip_filled_slots: a slot THIS pass filled pins nothing",
     "move\t$6,$7\njal\tfoo\nnop  # DEBUG: branch/jump\naddiu\t$4,$4,1\n"
     "j\t$L2\nnop  # DEBUG: branch/jump\n", 2, "addiu"),
]


#: maspsx's load-delay nop (`_handle_nop_before_next_instruction`): the
#: only nop this pass moves.
REUSE_NOP = re.compile(r"^\s*nop\s*# DEBUG: Reuse of ")


#: The closing comment of maspsx's `div` / `divu` / `rem` / `remu` macro
#: expansion (--expand-div, the only form this pipeline runs).
DIV_MACRO_END = re.compile(r"^\s*# EXPAND_DIVU? END\s*$")


def div_result_nop(text, funcs):
    """Drop maspsx's nop between a div MACRO and the first use of its
    result, in the named functions only. -> (new_text, n_dropped).

        mflo $2 / nop / sh $2,16($sp)       (maspsx: ASPSX)
        mflo $2 / sh $2,16($sp)             (wanted: GNU as)

    ASPSX treats the macro's closing `mflo` / `mfhi` like a load and puts a
    nop before a next instruction that reads the destination; maspsx
    reproduces that (`_handle_nop_before_next_instruction` after `# EXPAND_DIV
    END`), which is right for game code. The library's assembler is GNU as,
    whose div macro ends at the `mflo`. Measured on SLUS_006.28's image:
    of the div macros whose result the following
    instruction reads, game code has the nop 28 times of 28 and SDK library
    code 0 times of 10 (GsSetFlatLight x6, _SsStart, _SsSndTempo x2,
    func_8009E7C8).

    Only the `Reuse of` nop directly after the macro's END comment goes — a
    label maspsx put between the two is stepped over and kept. The plain
    `nop` pair maspsx emits when a mult/div FOLLOWS the mflo is the R3000's
    HI/LO hazard, which GNU as pads too: it is not a `Reuse of` line and is
    never touched, nor is any load's reuse nop.

    An ASSEMBLER fact, so it runs right after maspsx, before any slot pass.
    Per FUNCTION (compile.gnu_as_div_funcs: the library recipes and the
    gnu_as_loads marker), never a default: game code is assembled by ASPSX."""
    if not funcs:
        return text, 0
    out, n = [], 0
    cur, armed = None, False
    for line in text.split("\n"):
        s = line.strip()
        if s.startswith(".ent"):
            cur, armed = s.split()[-1], False
        elif s.startswith(".end"):
            cur, armed = None, False
        elif cur in funcs:
            if DIV_MACRO_END.match(line):
                armed = True
            elif armed and REUSE_NOP.match(line):
                n += 1
                armed = False
                continue
            elif armed and s and not is_label(line):
                armed = False
        out.append(line)
    return "\n".join(out), n


def _div_macro(dest="$2", mf="mflo", u=""):
    """maspsx's --expand-div output for `div{u} dest,$2,$4`, verbatim."""
    body = [f"# EXPAND_DIV{u} START", ".set\tnoat", f"div{u.lower()}\t$zero,$2,$4",
            "bnez\t$4,.L_NOT_DIV_BY_ZERO_9", "nop", "break\t0x7",
            ".L_NOT_DIV_BY_ZERO_9:"]
    if not u:
        body += ["addiu\t$at,$zero,-1", "bne\t$4,$at,.L_DIV_BY_POSITIVE_SIGN_9",
                 "lui\t$at,0x8000", "bne\t$2,$at,.L_DIV_BY_POSITIVE_SIGN_9",
                 "nop", "break\t0x6", ".L_DIV_BY_POSITIVE_SIGN_9:"]
    return "\n".join(body + [f"{mf}\t{dest}", ".set\tat",
                             f"# EXPAND_DIV{u} END"]) + "\n"


_DN_REUSE = "nop # DEBUG: Reuse of '$2'. 'sh\t$2,16($sp)' does not use $at\n"
_DN_USE = "sh\t$2,16($sp)\n"
DN_SELFTEST = [
    # (name, asm, funcs, expected drops, a line pair that must be adjacent
    #  afterwards or None)
    ("drops the reuse nop after a div macro (GsSetFlatLight's shape)",
     "\t.ent\tf\n" + _div_macro() + _DN_REUSE + _DN_USE + "\t.end\tf\n",
     {"f"}, 1, ("# EXPAND_DIV END", "sh\t$2,16($sp)")),
    ("divu / mfhi (remu) the same",
     "\t.ent\tf\n" + _div_macro(mf="mfhi", u="U") + _DN_REUSE + _DN_USE
     + "\t.end\tf\n", {"f"}, 1, ("# EXPAND_DIVU END", "sh\t$2,16($sp)")),
    ("a label between the macro and the nop is stepped over and kept",
     "\t.ent\tf\n" + _div_macro() + "$L7:\n" + _DN_REUSE + _DN_USE
     + "\t.end\tf\n", {"f"}, 1, ("$L7:", "sh\t$2,16($sp)")),
    ("an unnamed function keeps its nop (game code is ASPSX)",
     "\t.ent\tg\n" + _div_macro() + _DN_REUSE + _DN_USE + "\t.end\tg\n",
     {"f"}, 0, None),
    ("only the named one of two functions changes",
     "\t.ent\tg\n" + _div_macro() + _DN_REUSE + _DN_USE + "\t.end\tg\n"
     "\t.ent\tf\n" + _div_macro() + _DN_REUSE + _DN_USE + "\t.end\tf\n",
     {"f"}, 1, None),
    ("a LOAD's reuse nop is not a div macro's",
     "\t.ent\tf\nlw\t$2,D\nnop # DEBUG: Reuse of '$2'. x\n" + _DN_USE
     + "\t.end\tf\n", {"f"}, 0, None),
    ("the HI/LO hazard pair before a following mult stays (plain nops)",
     "\t.ent\tf\n" + _div_macro() + "nop\nnop\nmult\t$2,$5\n\t.end\tf\n",
     {"f"}, 0, None),
    ("an instruction after the macro disarms: a later load's nop stays",
     "\t.ent\tf\n" + _div_macro()
     + "#nop # DEBUG: 'ori\t$5,$5,1' does not load from $2\nori\t$5,$5,1\n"
     "lw\t$2,D\nnop # DEBUG: Reuse of '$2'. x\n" + _DN_USE + "\t.end\tf\n",
     {"f"}, 0, None),
    ("the macro's own delay-slot nops are never touched",
     "\t.ent\tf\n" + _div_macro() + _DN_REUSE + _DN_USE + "\t.end\tf\n",
     {"f"}, 1, ("bnez\t$4,.L_NOT_DIV_BY_ZERO_9", "nop")),
    ("no functions named -> identity",
     "\t.ent\tf\n" + _div_macro() + _DN_REUSE + _DN_USE + "\t.end\tf\n",
     set(), 0, None),
]


def dn_selftest():
    bad = 0
    for name, asm, funcs, want_n, pair in DN_SELFTEST:
        out, n = div_result_nop(asm, funcs)
        ok = n == want_n and (want_n or out == asm)
        # exactly the dropped line(s) are gone, nothing else moved
        ok = ok and len(out.split("\n")) == len(asm.split("\n")) - want_n
        if ok and pair:
            ls = out.split("\n")
            ok = any(ls[k] == pair[0] and ls[k + 1] == pair[1]
                     for k in range(len(ls) - 1))
        print(f"  [{'ok' if ok else 'FAIL'}] div_result_nop: {name}"
              + ("" if ok else f"  (dropped {n}, want {want_n})"))
        bad += not ok
    return bad


def nop_before_back_label(text, funcs):
    """Put maspsx's load-delay nop BEFORE the label that separates a load
    from its first use, for the named functions only. -> (new_text, n_moved).

        lw $3,SYM / L: / nop / lbu $2,0($3)     (maspsx)
        lw $3,SYM / nop / L: / lbu $2,0($3)     (wanted)

    maspsx always emits such a label BEFORE the reuse nop: ASPSX's order,
    right for game code (every path into the join executes the nop). The
    library's assembler is GNU as, whose append_insn emits the nops and
    then moves the pending insn_labels past them, so the label always
    lands AFTER the nop, whoever references it (probed on the vendored GNU
    as for a forward-referenced and a backward-only label alike). Measured
    on SLUS_006.28: StCdInterrupt (func_80093D28, libcd c_011.o) for a
    loop head only a later backward branch reaches, and CdSearchFile
    (func_80092508, libcd ISO9660.OBJ, 2026-10-08) for a FORWARD-referenced
    join (`beq $v0,$s5` over the path-copy loop lands past the nop:
    181/182 -> MATCH). The remaining target asm agrees: every such label in
    a psyq function has the nop first (4 forward, 2 backward), every one in
    game code has the label first (23). Until 2026-10-08 this pass skipped
    forward-referenced labels; no landed function had one.
    Per FUNCTION (the library recipes), never a default: game code is
    assembled by ASPSX."""
    lines = text.split("\n")
    n = 0
    cur, start = None, None
    i = 0
    while i < len(lines):
        s = lines[i].strip()
        if s.startswith(".ent"):
            cur, start = s.split()[-1], i
        elif s.startswith(".end"):
            cur = None
        elif (cur in funcs and is_label(lines[i]) and i + 1 < len(lines)
                and REUSE_NOP.match(lines[i + 1])):
            lines[i], lines[i + 1] = lines[i + 1], lines[i]
            n += 1
            i += 1
        i += 1
    return "\n".join(lines), n


NBL_LOOP = ("\t.ent\tf\nlw\t$3,D\n$L1:\nnop # DEBUG: Reuse of '$3'. x\n"
            "# $L1:  # DEBUG: skipped\nlbu\t$2,0($3)\nbne\t$2,$0,$L1\n"
            "nop  # DEBUG: branch/jump\n\t.end\tf\n")
NBL_SELFTEST = [
    # (name, asm, funcs, expected moves)
    ("moves the reuse nop above a backward-only loop label",
     NBL_LOOP, {"f"}, 1),
    ("moves it above a forward-referenced label too (GNU as moves labels)",
     "\t.ent\tf\nbeq\t$2,$0,$L1\nnop  # DEBUG: branch/jump\nlw\t$3,D\n"
     "$L1:\nnop # DEBUG: Reuse of '$3'. x\nlbu\t$2,0($3)\n\t.end\tf\n",
     {"f"}, 1),
    ("unnamed function untouched", NBL_LOOP, {"g"}, 0),
    ("a label prefix is not a reference ($L1 vs $L10)",
     "\t.ent\tf\nbeq\t$2,$0,$L10\nnop  # DEBUG: branch/jump\nlw\t$3,D\n"
     "$L1:\nnop # DEBUG: Reuse of '$3'. x\nlbu\t$2,0($3)\n"
     "bne\t$2,$0,$L1\nnop  # DEBUG: branch/jump\n\t.end\tf\n", {"f"}, 1),
]


def nbl_selftest():
    bad = 0
    for name, asm, funcs, want_n in NBL_SELFTEST:
        out, n = nop_before_back_label(asm, funcs)
        ok = n == want_n
        if ok and want_n:
            ls = [l.strip() for l in out.split("\n")]
            k = ls.index("$L1:")
            ok = ls[k - 1].startswith("nop # DEBUG: Reuse")
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (moved {n}, want {want_n})"))
        bad += not ok
    for name, asm, want_n, want_slot in FILLED_SLOT_SELFTEST:
        out, n = fill(asm, skip_filled_slots=True)
        rows = [r for r in (_parse(l) for l in out.split("\n")) if r]
        bi = max((k for k, r in enumerate(rows) if r[1] in BRANCH), default=None)
        got = rows[bi + 1][1] if bi is not None and bi + 1 < len(rows) else "?"
        ok = n == want_n and got == want_slot
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (filled {n}, want {want_n}; "
                               f"slot {got}, want {want_slot})"))
        bad += not ok
    return bad


ARG_REGS = {"$a0", "$a1", "$a2", "$a3"}


def arg_slot_fill(text, funcs):
    """Fill an EMPTY `jal` delay slot with the ARGUMENT SETUP, for the named
    functions only. -> (new_text, n_filled).

        addu $a0,$zero,$zero … sw $x,SYM / jal f / nop   (ours)
        … sw $x,SYM / jal f / addu $a0,$zero,$zero       (wanted)

    Why this is separate from fill(): fill() moves the instruction
    IMMEDIATELY before the branch, and here that is a store which the
    target does not put in the slot. Our cc1 hoists the constant argument
    setup to the top of the function; the original's emitted it adjacent to
    its `jal`, so the assembler's own "move the preceding instruction" rule
    produced the argument setup. We cannot reproduce that scheduling from C
    (measured on StClearRing: constant-through-a-variable and load-reorder
    both stay 23/24), so the placement is named per function instead.

    The rule is targeted rather than a "search backwards and take
    something": the candidate must WRITE AN ARGUMENT REGISTER, which is
    what makes it argument setup and not an arbitrary hoist. Moving it past
    the intervening instructions is checked for RAW/WAR/WAW, and the usual
    branch hazards still apply.
    """
    if not funcs:
        return text, 0
    lines = text.split("\n")
    cur, n, i = None, 0, -1
    while True:
        i += 1
        if i >= len(lines):
            break
        if is_label(lines[i]):
            lab = lines[i].strip()[:-1]
            if not lab.startswith((".", "$")):
                cur = lab
            continue
        if cur not in funcs:
            continue
        p = _parse(lines[i])
        if not p or p[1] != "jal":
            continue
        slot_i = next((k for k in range(i + 1, len(lines))
                       if _parse(lines[k])), None)
        if slot_i is None or _parse(lines[slot_i])[1] != "nop":
            continue
        br_reads, br_writes = _reads(p[1], p[2]), _writes(p[1], p[2])
        between = []
        cand_i = None
        for k in range(i - 1, -1, -1):
            if is_label(lines[k]):
                break
            c = _parse(lines[k])
            if not c:
                continue
            if c[1] in MOVABLE and (_writes(c[1], c[2]) & ARG_REGS):
                cw, cr = _writes(c[1], c[2]), _reads(c[1], c[2])
                if (cw & br_reads) or (cr & br_writes):
                    break
                bad = False
                for b in between:
                    bw, br_ = _writes(b[1], b[2]), _reads(b[1], b[2])
                    if (cw & br_) or (bw & cr) or (bw & cw):
                        bad = True
                        break
                if not bad:
                    cand_i = k
                break
            between.append(c)
        if cand_i is None:
            continue
        moved = lines.pop(cand_i)
        lines[slot_i - 1] = moved
        n += 1
    return "\n".join(lines), n


def load_slot_epilogue(text, funcs):
    """Move `addiu $sp` from the RETURN delay slot into the LOAD delay slot,
    for the named functions only. -> (new_text, n_swapped).

        lw $ra,N($sp) / nop          / jr $ra / addiu $sp,$sp,N   (ours)
        lw $ra,N($sp) / addiu $sp,.. / jr $ra / nop               (wanted)

    Both are correct and cost the same cycles: MIPS I has a load delay
    slot, so SOMETHING must sit between `lw $ra` and `jr $ra`, and the two
    forms differ only in which of the two slots takes the stack adjust and
    which takes the nop.

    WHY THIS IS OPT-IN PER FUNCTION, and must stay that way. The original
    compiler emitted BOTH forms for locally identical code. Measured on
    SLUS_009.21: `func_8012A4C0` ends `jal / addiu $a1 / lw $ra /
    addiu $sp / jr $ra / nop` and `func_8010E67C` ends `jal / addiu $a0 /
    lw $ra / nop / jr $ra / addiu $sp` — same structure, same 0x18 frame,
    opposite choice. Nothing local decides it, so a blanket pass would fix
    the ~62 functions in the first shape and break the 4 in the second.
    Naming the function and letting `make` judge is the only honest form
    this transform can take.

    Distinct from `fill()`, which REFUSES return slots outright (see its
    comment — filling one reproduces the default epilogue and cost
    gpu_prim_setup_12 a point). This does not FILL the return slot; it
    empties it, moving the adjust one instruction earlier.
    """
    if not funcs:
        return text, 0
    lines = text.split("\n")
    cur, n = None, 0
    for i, line in enumerate(lines):
        if is_label(line):
            lab = line.strip()[:-1]
            if not lab.startswith((".", "$")):  # $L2 is a LOCAL
                # label — treating it as a function boundary silently
                # no-op'd every per-function pass past the first branch
                cur = lab
            continue
        if cur not in funcs:
            continue
        p = _parse(line)
        # anchor on `lw $ra` whose next real instruction is a bare nop
        if not p or p[1] != "lw" or _regs(p[2])[:1] != ["$ra"]:
            continue
        idx = [k for k in range(i + 1, len(lines)) if _parse(lines[k])]
        if len(idx) < 3:
            continue
        a, b, c = idx[0], idx[1], idx[2]
        pa, pb, pc = _parse(lines[a]), _parse(lines[b]), _parse(lines[c])
        if pa[1] != "nop":
            continue                       # load slot already carries work
        if pb[1] not in ("jr", "j") or _regs(pb[2]) != ["$ra"]:
            continue
        # maspsx spells the stack adjust `addu $sp,$sp,24` (cc1's form,
        # which GAS accepts); the target .s disassembles it as `addiu`.
        # Accept both, or this matches nothing and fails silently.
        if pc[1] not in ("addiu", "addu") or _regs(pc[2])[:1] != ["$sp"]:
            continue
        # swap the two instructions, each keeping its own line's indent
        adjust = lines[c].strip()
        lines[a] = pa[0] + adjust
        lines[c] = pc[0] + "nop"
        n += 1
    return "\n".join(lines), n


def return_slot_fill(text, funcs):
    """Move the instruction before `jr $ra` INTO its return delay slot, for
    the named functions only. -> (new_text, n_filled).

        sw $a0,%lo(S)($at) / jr $ra / nop      (ours)
        jr $ra / sw $a0,%lo(S)($at)            (wanted)

    The third member of this family, and the only one that fills a RETURN
    slot:

      - `fill()` fills ordinary slots and REFUSES return slots outright,
        because doing so blanket-style moves `addiu $sp` back in and
        reproduces the default epilogue (measured: gpu_prim_setup_12
        8/9 -> 7/9).
      - `load_slot_epilogue()` EMPTIES a return slot, moving the adjust one
        instruction earlier.
      - this FILLS one, for functions that name themselves.

    Per FUNCTION for the same reason as its two siblings: nothing local
    decides which form the original compiler emitted, so a blanket pass
    would fix one shape and break the other. Naming the function and
    letting `make` judge the bytes is the only honest mechanism.

    Measured need (SLUS_009.21, 2026-08-08): six accessors of the shape
    `s32 f(s32 a) { s32 old = SYM; SYM = a; return old; }` compile to
    `sw / jr $ra / nop` while the target is `jr $ra / sw`. They sit at 4/5
    with the store as the only difference. `@delay-fill` cannot reach them
    (it refuses return slots) and enabling it file-wide on that program's
    single TU turns the build red.

    Safety is `fill()`'s, unchanged: no LABEL may sit between the candidate
    and the branch (another path would reach the branch without executing
    it), the candidate must be in MOVABLE, and it may neither write what
    the branch reads nor read what the branch writes -- so a candidate
    that assigns $ra is refused, since `jr $ra` would then jump to the new
    value.
    """
    if not funcs:
        return text, 0
    small = _small_syms(text)
    lines = text.split("\n")
    cur, n, i = None, 0, -1
    while True:
        i += 1
        if i >= len(lines):
            break
        if is_label(lines[i]):
            lab = lines[i].strip()[:-1]
            if not lab.startswith((".", "$")):  # $L2 is a LOCAL
                # label — treating it as a function boundary silently
                # no-op'd every per-function pass past the first branch
                cur = lab
            continue
        if cur not in funcs:
            continue
        p = _parse(lines[i])
        if not p or p[1] not in ("jr", "j") or _regs(p[2]) != ["$ra"]:
            continue
        slot_i = next((k for k in range(i + 1, len(lines)) if _parse(lines[k])),
                      None)
        if slot_i is None or _parse(lines[slot_i])[1] != "nop":
            continue
        cand_i = None
        for k in range(i - 1, -1, -1):
            if is_label(lines[k]):
                break
            if _parse(lines[k]):
                cand_i = k
                break
        if cand_i is None:
            continue
        cand = _parse(lines[cand_i])
        if cand[1] not in MOVABLE:
            continue
        br_reads, br_writes = _reads(p[1], p[2]), _writes(p[1], p[2])
        c_reads, c_writes = _reads(cand[1], cand[2]), _writes(cand[1], cand[2])
        if (c_writes & br_reads) or (c_reads & br_writes):
            continue
        # A store to a BARE SYMBOL (`sw $4,SYM`) is a macro: GAS expands it
        # to `lui $at,%hi(SYM)` + `sw $4,%lo(SYM)($at)`. Two instructions do
        # not fit one delay slot, so GAS hoists the whole pair and emits the
        # branch too early ("macro instruction expanded into multiple
        # instructions in a branch delay slot"). Expand it ourselves: the
        # `lui` stays where the candidate was, the `sw` goes in the slot —
        # which is exactly the shape the target has.
        m_bare = _bare_far(cand[1], cand[2], small)
        if m_bare:
            reg, sym = m_bare.group(1), m_bare.group(2)
            ind = cand[0]
            lines[cand_i] = f"{ind}lui\t$at,%hi({sym})"
            lines[slot_i] = f"{ind}{cand[1]}\t{reg},%lo({sym})($at)"
            n += 1
            continue
        # An INDEXED far store (`sb $2,SYM($3)`) is a THREE-instruction GAS
        # macro: `lui $at,%hi(SYM)` / `addu $at,$at,$3` / the store. GNU
        # as's `.set reorder` fill swaps only the last of the three into
        # the return slot, so the address pair stays before `jr`:
        #     lui $at,%hi(S) / addu $at,$at,$v1 / jr $ra / sb $v0,%lo(S)($at)
        # Moved whole (the line below), GAS expands it INSIDE the slot and
        # the function returns before its own store. A small (gp) symbol
        # and a base of $at are left to the generic path, unchanged.
        # (SLUS_006.28: func_8009B498, func_8009BDB8 and func_8008D888 are
        # the image's only three such returns, none converted before this.)
        m_idx = INDEXED_MEM.match(cand[2]) if cand[1] in NO_DEST else None
        if (m_idx and m_idx.group(3) not in small
                and _norm(m_idx.group(4)) != "$at"):
            reg, sym, base = m_idx.group(1), m_idx.group(2), m_idx.group(4)
            ind = cand[0]
            lines[slot_i] = f"{ind}{cand[1]}\t{reg},%lo({sym})($at)"
            lines.insert(slot_i + 1, f"{ind}.set\tat")
            lines[cand_i:cand_i + 1] = [f"{ind}.set\tnoat",
                                        f"{ind}lui\t$at,%hi({sym})",
                                        f"{ind}addu\t$at,$at,{base}"]
            n += 1
            i += 3                       # three lines went in ahead of `jr`
            continue
        moved = lines.pop(cand_i)
        lines[slot_i - 1] = moved
        n += 1
    return "\n".join(lines), n


def load_hazard_nop(text, funcs, far=frozenset()):
    """Insert the load-delay nop between a load and an IMMEDIATELY
    following bare-symbol store that reuses the loaded register, for the
    named functions only. -> (new_text, n_inserted).

        lw $2,SYM / sw $2,SYM2         (ours)
        lw $2,SYM / nop / sw $2,SYM2   (wanted)

    The gap this closes is maspsx's, measured on SLUS_009.21
    `func_8010E67C`: maspsx inserts the reuse
    nop when the next instruction visibly consumes the register with no
    `$at` expansion, or when it touches a gp symbol maspsx can SEE
    (file-local .comm/.sdata) — but a bare-symbol store to an EXTERN
    that only the linker resolves into the gp window falls through both
    tests, while the original ASPSX (which expands the macro itself and
    knows the final form) put the nop in. Eighth member of the
    per-function family, and per FUNCTION for the family's usual
    reason: whether the extern lands in the gp window is not decidable
    from the TU, so the function names itself and `make` judges the
    bytes.

    A LABEL between the two (the store is a join point — the cross-jumped
    tail of an if/else that stores the same global in both arms) puts the
    nop AFTER the label, where every path into the join executes it:

        lbu $2,15($3) / $L8: / sb $2,SYM          (ours)
        lbu $2,15($3) / $L8: / nop / sb $2,SYM    (wanted)

    That is ASPSX's placement (measured SLUS_006.28 func_8002D254 at
    0x8002D33C: the `beqz` into the join lands ON the nop) and maspsx's
    own for a gp symbol it can see (_handle_nop_before_next_instruction
    emits the label, then the nop). Only ONE label is crossed; anything
    else between them (a second label, a directive, a comment) refuses.
    An existing nop is never doubled.

    FAR stores never fire (`far` = the TU's far_syms set): ASPSX expands
    a far bare-symbol store to `lui $at,%hi(S)` + `sw $2,%lo(S)($at)`,
    and the lui already sits in the load delay — measured on SLUS_006.28
    func_80059498 (2026-10-07), whose target has the nop before its two
    gp-window stores (D_800B8C5C, D_800B8C4C) and NONE before the far
    D_800B8B3C store two instructions after `lhu $2,10($3)`.
    """
    if not funcs:
        return text, 0
    lines = text.split("\n")
    out, n = [], 0
    cur = None
    after_label = {}        # label line index -> nop to emit after it
    for i, line in enumerate(lines):
        out.append(line)
        if i in after_label:
            out.append(after_label.pop(i))
        s = line.strip()
        if s.startswith(".ent"):
            name = s.split()[-1]
            cur = name if name in funcs else None
            continue
        if s.startswith(".end"):
            cur = None
            continue
        if cur is None:
            continue
        p = _parse(line)
        if not p or p[1] not in LOADS:
            continue
        dest = _regs(p[2])[:1]
        if not dest:
            continue
        # the very next line decides — or, when it is ONE label, the line
        # after it (the nop then goes after the label); blank/comment/
        # directive break the measured shape, so only an instruction fires
        j = i + 1
        if j < len(lines) and is_label(lines[j]):
            j += 1
        nxt = _parse(lines[j]) if j < len(lines) else None
        if not nxt or nxt[1] == "nop" or nxt[1] not in NO_DEST:
            continue
        m_bare = BARE_MEM_OFF.match(nxt[2])
        if not m_bare or _norm(m_bare.group(1)) != _norm(dest[0]):
            continue
        if m_bare.group(2) in far:
            continue
        nop = (f"{p[0]}nop # DEBUG: load-hazard before bare-symbol "
               f"store (load_hazard_nop)")
        if j == i + 1:
            out.append(nop)
        else:
            after_label[j - 1] = nop
        n += 1
    return "\n".join(out), n


# `la $r,SYM` / `la $r,SYM+off` — the address-load macro (operand: one
# register, one symbol expression, no base register).
LA_OPS = re.compile(r"^(\$\w+)\s*,\s*([A-Za-z_.$][\w.$]*(?:[+-]\d+)?)\s*$")


def la_absolute(text, funcs):
    """Expand `la $r,SYM` into `lui $r,%hi(SYM)` + `addiu $r,$r,%lo(SYM)`
    in the named functions. -> (new_text, n_expanded).

    The original assembler never used $gp for `la`: maspsx's own ASPSX
    table lists "use $gp for la" for 2.81/2.86 only, and the project's
    maspsx runs with that off. But this pipeline assembles with GNU as
    `-G8` (maspsx `--dont-force-G0`), and as turns `la` of a SMALL-data
    symbol (a `.extern SYM,size<=8` cc1 emitted) into `addiu $r,$gp,
    %gp_rel(SYM)` — one instruction where ASPSX wrote two. cc1 emits
    `la` (unsplit) exactly for such symbols: mips_check_split refuses to
    split a SYMBOL_REF_FLAG (small) address. Scalar loads/stores of the
    same symbols must stay gp-relative, which only per-INSTRUCTION
    handling gives (dropping the .extern would make them absolute).

    Measured on SLUS_006.28 func_8001F508 (the pad/vibration driver,
    2026-10-08): its eight 2-byte motor tables (D_800B8698 .. D_800B86BC)
    are small arrays whose scalar elements are stored %gp_rel while
    every address-of / indexed access is `lui $r; addiu $r,$r` on ONE
    register — the assembler-macro shape, never cc1's split HIGH/LO_SUM
    (which puts the HIGH in a temp: `lui $2; addiu $17,$2`). The `_blk`
    alias pairs had been approximating this with a far twin symbol.

    Per FUNCTION (opt-in) like the rest of this family, so no matched
    function moves. An `la` in a branch delay slot is REFUSED (raises):
    cc1 sized it as one instruction there, and two cannot go in a slot.
    """
    if not funcs:
        return text, 0
    out, n = [], 0
    cur = None
    prev = None             # previous instruction's mnemonic (this function)
    for line in text.split("\n"):
        s = line.strip()
        if s.startswith(".ent"):
            name = s.split()[-1]
            cur = name if name in funcs else None
            prev = None
            out.append(line)
            continue
        if s.startswith(".end"):
            cur = None
            out.append(line)
            continue
        if cur is None:
            out.append(line)
            continue
        if is_label(line):
            prev = None
            out.append(line)
            continue
        p = _parse(line)
        if p and p[1] == "la":
            m = LA_OPS.match(p[2])
            if m:
                if prev in BRANCH:
                    raise ValueError(
                        f"la_absolute: `{s}` in {cur} sits in a branch delay "
                        f"slot — refusing to expand it into two instructions")
                reg, sym = m.group(1), m.group(2)
                out.append(f"{p[0]}lui\t{reg},%hi({sym}) # la_absolute")
                out.append(f"{p[0]}addiu\t{reg},{reg},%lo({sym}) # la_absolute")
                n += 1
                prev = "addiu"
                continue
        if p:
            prev = p[1]
        out.append(line)
    return "\n".join(out), n


def splice(primary, alt, funcs):
    """Replace named functions' `.ent f`…`.end f` blocks in PRIMARY with
    the same blocks from ALT — a second cc1 run of the SAME translation
    unit under different flags. -> (new_text, spliced_names, refused
    {name: reason}).

    This is what "per-function -fno-delayed-branch" means mechanically:
    GCC 2.8.1 has no per-function flag control and a single-TU program
    (SLUS_009.21's main) makes every cc_flags.txt entry all-or-nothing —
    the TU wall again, for compiler flags. The fourth member of the
    per-function family (fill / load_slot_epilogue / return_slot_fill),
    and like its siblings it exists because the original build mixed
    settings NOTHING LOCAL exposes: func_80120380's target has empty
    conditional-branch delay slots (bltz/nop, bnez/nop — assembler-era
    codegen) inside a TU whose 85 matched functions all need the default
    scheduling.

    Safety rules, each refusing loudly rather than degrading:

    - local code labels ($Lnn) DEFINED in the alt block are renamed with
      a per-function suffix — the two compiles number labels
      independently, and a colliding definition would either fail GAS or
      silently retarget a primary branch;
    - a block referencing a $LC constant (string literal, switch jump
      table) is REFUSED: the constant/table lives OUTSIDE the block with
      the OTHER compile's numbering, and a switch table would go on
      referencing the deleted primary labels. Splitting the TU is the
      honest fix there;
    - a function present in primary but missing from alt (or with no
      .end) is refused — a silent skip would score as "the flag did
      nothing".
    """
    spliced, refused = [], {}

    def block_of(text, f):
        lines = text.split("\n")
        s = next((i for i, ln in enumerate(lines)
                  if re.match(rf"\s*\.ent\s+{re.escape(f)}\s*$", ln)), None)
        if s is None:
            return None, None, lines
        e = next((i for i in range(s + 1, len(lines))
                  if re.match(rf"\s*\.end\s+{re.escape(f)}\s*$", lines[i])),
                 None)
        return s, e, lines

    out = primary
    for f in sorted(funcs):
        ps, pe, plines = block_of(out, f)
        if ps is None:
            continue                      # not defined in this TU — no-op,
                                          # same contract as the siblings
        a_s, a_e, alines = block_of(alt, f)
        if a_s is None or a_e is None or pe is None:
            refused[f] = "block not found (.ent/.end) in " + \
                ("alt" if a_s is None or a_e is None else "primary")
            continue
        blk = alines[a_s:a_e + 1]
        body = "\n".join(blk)
        if re.search(r"\$LC\d+", body):
            refused[f] = ("references a $LC constant (string/jump table) "
                          "— its label numbering belongs to the other "
                          "compile; split the TU instead")
            continue
        # rename in-block label definitions (and their in-block refs)
        defs = {m.group(1) for ln in blk
                for m in [re.match(r"(\$L\w+):", ln.strip())] if m}
        for lab in sorted(defs, key=len, reverse=True):
            body = re.sub(re.escape(lab) + r"\b", f"{lab}_ndb_{f}", body)
        out = "\n".join(plines[:ps] + body.split("\n") + plines[pe + 1:])
        spliced.append(f)
    return out, spliced, refused


def zero_remat(text, funcs):
    """Rewrite a copy FROM a register that provably holds zero into an
    independent materialization from $zero (`move $a3,$a2` -> `move
    $a3,$0`), for the named functions only. -> (new_text, n).

    Fifth member of the per-function family, and the first that exists
    because NO source shape can produce the target bytes: GCC 2.8.1
    CSEs a second constant-zero argument into a copy of the register
    already holding it, and the original compiler emitted an
    independent `addu $a3,$zero,$zero`. Measured on func_80121D30
    (27/28, the one mismatch exactly this): casts, argument widths,
    pointer-typed zeros and an unprototyped call ALL left the copy in
    place — the equivalence is in cse's table, below anything the
    source can express.

    Both forms write 0; the transform is byte-different and
    semantics-identical PROVIDED the source register holds zero, and
    that proof is discharged locally and conservatively:

    - a register enters the known-zero set only via `move $R,$0` /
      `addu $R,$0,$0` / `li $R,0` (numeric or named $zero — cc1 mixes
      them in ONE instruction: `addu $6,$0,$zero`);
    - ANY label clears the set (another path may join);
    - ANY branch/jump/call clears the set AFTER its delay slot — the
      slot executes before control transfers, and the real case
      (func_80121D30) has the copy exactly there: `jal $31,$2 /
      addu $7,$6,$zero`;
    - any other write to a register removes it from the set.

    The rewritten line uses cc1's own explicit spelling
    (`addu $R,$0,$zero`), never the `move` macro — GAS may assemble
    `move` as `or`, which is a DIFFERENT byte than the target's addu.
    """
    if not funcs:
        return text, 0
    lines = text.split("\n")
    cur, n = None, 0
    zero = set()
    pending_clear = False
    for i, line in enumerate(lines):
        if is_label(line):
            lab = line.strip()[:-1]
            if not lab.startswith((".", "$")):  # $L2 is a LOCAL
                # label — treating it as a function boundary silently
                # no-op'd every per-function pass past the first branch
                cur = lab
            zero.clear()
            pending_clear = False
            continue
        p = _parse(line)
        if not p:
            continue
        ind, mnem, ops = p
        if mnem in BRANCH:
            pending_clear = True   # clear AFTER the delay slot
            continue
        regs = _regs(ops)
        is_zero_src = (
            (mnem == "move" and len(regs) == 2 and regs[1] == "$zero")
            or (mnem == "addu" and len(regs) == 3
                and regs[1] == regs[2] == "$zero")
            or (mnem == "li" and len(regs) == 1
                and re.search(r",\s*(?:0|0x0)\s*$", ops)))
        if is_zero_src:
            zero.add(regs[0])
            if pending_clear:      # a materialization IN the slot does
                zero.clear()       # not survive the call's clobbers
                pending_clear = False
            continue
        is_copy = (mnem == "move" and len(regs) == 2) or \
                  (mnem == "addu" and len(regs) == 3 and "$zero" in regs[1:])
        if is_copy:
            src = regs[1] if mnem == "move" else \
                (regs[1] if regs[2] == "$zero" else regs[2])
            if src in zero and cur in funcs:
                # keep the ORIGINAL destination spelling (numeric $7 or
                # named); emit cc1's explicit addu form, never `move`
                lines[i] = (f"{ind}addu\t{ops.split(',')[0].strip()}"
                            f",$0,$zero")
                zero.add(regs[0])
                n += 1
                if pending_clear:
                    zero.clear()
                    pending_clear = False
                continue
        for w in _writes(mnem, ops):
            zero.discard(w)
        if pending_clear:
            zero.clear()
            pending_clear = False
    return "\n".join(lines), n


ZR_SELFTEST = [
    # (name, asm, funcs, expected rewrites, output substring expected)
    ("rewrites a copy of a known-zero reg via explicit addu",
     "f:\nmove\t$6,$0\nmove\t$7,$6\njal\tg\nnop\n", {"f"}, 1,
     "addu\t$7,$0,$zero"),
    ("cc1's mixed spelling addu $6,$0,$zero is a zero-def",
     "f:\naddu\t$6,$0,$zero\nmove\t$7,$6\n", {"f"}, 1,
     "addu\t$7,$0,$zero"),
    ("the DELAY SLOT still rewrites (the func_80121D30 shape)",
     "f:\naddu\t$6,$0,$zero\nlw\t$4,24($2)\njal\t$31,$2\naddu\t$7,$6,$zero\n",
     {"f"}, 1, "addu\t$7,$0,$zero"),
    ("unnamed function untouched (opt-in like all siblings)",
     "f:\nmove\t$6,$0\nmove\t$7,$6\n", {"g"}, 0, "$7,$6"),
    ("a label between def and copy refuses (join point)",
     "f:\nmove\t$6,$0\n$L1:\nmove\t$7,$6\n", {"f"}, 0, "$7,$6"),
    ("a copy AFTER the slot refuses (post-call clobber)",
     "f:\nmove\t$6,$0\njal\tg\nnop\nmove\t$7,$6\n", {"f"}, 0, "$7,$6"),
    ("redefinition removes the reg from the zero set",
     "f:\nmove\t$6,$0\naddiu\t$6,$6,4\nmove\t$7,$6\n", {"f"}, 0, "$7,$6"),
]


def zr_selftest():
    bad = 0
    for name, asm, funcs, want_n, want_sub in ZR_SELFTEST:
        out, n = zero_remat(asm, funcs)
        ok = n == want_n and want_sub in out
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (n={n} want {want_n}; out={out!r})"))
        bad += not ok
    return bad


SPLICE_SELFTEST_PRIMARY = """\
.ent\tf
f:
addiu\t$v0,$v0,1
bnez\t$v0,$L1
sw\t$v0,0($a0)
$L1:
jr\t$31
nop
.end\tf
.ent\tg
g:
jr\t$31
nop
.end\tg
"""
SPLICE_SELFTEST_ALT = """\
.ent\tf
f:
addiu\t$v0,$v0,1
bnez\t$v0,$L7
nop
sw\t$v0,0($a0)
$L7:
jr\t$31
nop
.end\tf
.ent\tg
g:
jr\t$31
nop
.end\tg
"""


def splice_selftest():
    bad = 0

    def check(name, ok, detail=""):
        nonlocal bad
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  ({detail})"))
        bad += not ok

    out, sp, ref = splice(SPLICE_SELFTEST_PRIMARY, SPLICE_SELFTEST_ALT, {"f"})
    check("splices the named function's alt block", sp == ["f"]
          and "bnez\t$v0,$L7_ndb_f" in out and not ref, f"{sp} {ref}")
    check("renames in-block label defs AND refs", "$L7_ndb_f:" in out
          and "$L7:" not in out)
    check("leaves the unnamed neighbor alone",
          out.count(".ent\tg") == 1 and "g:\njr" in out)
    out2, sp2, _ = splice(SPLICE_SELFTEST_PRIMARY, SPLICE_SELFTEST_ALT, {"h"})
    check("absent function is a no-op", out2 == SPLICE_SELFTEST_PRIMARY
          and sp2 == [])
    alt_lc = SPLICE_SELFTEST_ALT.replace("sw\t$v0,0($a0)",
                                         "lw\t$v0,%lo($LC0)($a0)")
    _, sp3, ref3 = splice(SPLICE_SELFTEST_PRIMARY, alt_lc, {"f"})
    check("refuses a $LC-referencing block loudly", sp3 == [] and "f" in ref3)
    alt_trunc = SPLICE_SELFTEST_ALT.replace(".end\tf\n", "")
    _, sp4, ref4 = splice(SPLICE_SELFTEST_PRIMARY, alt_trunc, {"f"})
    check("refuses when alt lacks the block/.end", sp4 == [] and "f" in ref4)
    return bad


RET_SELFTEST = [
    # (name, asm, funcs, expected fills, expected return-slot mnemonic)
    ("fills a named function's return slot with the preceding store",
     "f:\n\tsw\t$a0,4($at)\n\tjr\t$ra\n\tnop\n", {"f"}, 1, "sw"),
    ("leaves an UNnamed function alone (opt-in, like its two siblings)",
     "f:\n\tsw\t$a0,4($at)\n\tjr\t$ra\n\tnop\n", {"g"}, 0, "nop"),
    ("refuses a candidate that writes $ra (jr would jump to the new value)",
     "f:\n\tmove\t$ra,$v0\n\tjr\t$ra\n\tnop\n", {"f"}, 0, "nop"),
    ("refuses across a LABEL (another path reaches the branch)",
     "f:\n\tsw\t$a0,4($at)\n.L1:\n\tjr\t$ra\n\tnop\n", {"f"}, 0, "nop"),
    ("refuses a load candidate (maspsx already placed its load-delay nops)",
     "f:\n\tlw\t$v0,4($at)\n\tjr\t$ra\n\tnop\n", {"f"}, 0, "nop"),
    ("leaves an already-filled return slot alone",
     "f:\n\tsw\t$a0,4($at)\n\tjr\t$ra\n\tsw\t$a1,8($at)\n", {"f"}, 0, "sw"),
    # an indexed far store is three instructions: only the store itself
    # goes in the slot, `lui $at` / `addu $at,$at,base` stay before `jr`
    ("splits an INDEXED far store: the store alone goes in the slot",
     "f:\n\tsb\t$2,SYM($3)\n\tj\t$31\n\tnop\n", {"f"}, 1, "sb"),
    ("splits an indexed far store with an offset (SYM+8)",
     "f:\n\tsh\t$2,SYM+8($6)\n\tjr\t$ra\n\tnop\n", {"f"}, 1, "sh"),
]


LHN_SELFTEST = [
    # (name, asm, funcs, want_n)
    ("lhn: inserts between lw and bare-sym store of same reg",
     "\t.ent\tf\nlw\t$2,SYM_A\nsw\t$2,SYM_B\n\t.end\tf\n", {"f"}, 1),
    ("lhn: existing nop is not doubled",
     "\t.ent\tf\nlw\t$2,SYM_A\nnop\nsw\t$2,SYM_B\n\t.end\tf\n", {"f"}, 0),
    ("lhn: different data register does not fire",
     "\t.ent\tf\nlw\t$2,SYM_A\nsw\t$3,SYM_B\n\t.end\tf\n", {"f"}, 0),
    ("lhn: ONE label between load and store: nop goes after the label",
     "\t.ent\tf\nlw\t$2,SYM_A\n$L1:\nsw\t$2,SYM_B\n\t.end\tf\n", {"f"}, 1),
    ("lhn: two labels between load and store refuses",
     "\t.ent\tf\nlw\t$2,SYM_A\n$L1:\n$L2:\nsw\t$2,SYM_B\n\t.end\tf\n", {"f"}, 0),
    ("lhn: label then an existing nop is not doubled",
     "\t.ent\tf\nlw\t$2,SYM_A\n$L1:\nnop\nsw\t$2,SYM_B\n\t.end\tf\n", {"f"}, 0),
    ("lhn: offset store (already-resolved form) does not fire",
     "\t.ent\tf\nlw\t$2,SYM_A\nsw\t$2,4($3)\n\t.end\tf\n", {"f"}, 0),
    ("lhn: unnamed function untouched",
     "\t.ent\tg\nlw\t$2,SYM_A\nsw\t$2,SYM_B\n\t.end\tg\n", {"f"}, 0),
    ("lhn: numeric/name register alias still fires",
     "\t.ent\tf\nlw\t$v0,SYM_A\nsw\t$2,SYM_B\n\t.end\tf\n", {"f"}, 1),
    ("lhn: FAR store does not fire (its lui $at fills the delay)",
     "\t.ent\tf\nlhu\t$2,10($3)\nsh\t$2,SYM_FAR\n\t.end\tf\n", {"f"}, 0),
]


def lhn_selftest():
    bad = 0
    for name, asm, funcs, want_n in LHN_SELFTEST:
        out, n = load_hazard_nop(asm, funcs, far={"SYM_FAR"})
        ok = n == want_n
        if ok and want_n:
            rows = [r for r in (_parse(l) for l in out.split("\n")) if r]
            li = next((k for k, r in enumerate(rows) if r[1] in LOADS), None)
            ok = li is not None and rows[li + 1][1] == "nop"
            if ok and "$L1:" in asm:
                # the nop must sit AFTER the label (every path runs it)
                ls = [l.strip() for l in out.split("\n")]
                ok = ls[ls.index("$L1:") + 1].startswith("nop")
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (inserted {n}, want {want_n})"))
        bad += not ok
    return bad


LAA_SELFTEST = [
    # (name, asm, funcs, want_n, want_lines | None = must raise)
    ("laa: la of a symbol becomes lui/addiu on one register",
     "\t.ent\tf\nla\t$6,SYM_A\naddu\t$2,$2,$6\n\t.end\tf\n", {"f"}, 1,
     ["lui\t$6,%hi(SYM_A)", "addiu\t$6,$6,%lo(SYM_A)"]),
    ("laa: symbol+offset keeps its offset in both halves",
     "\t.ent\tf\nla\t$4,SYM_B+8\n\t.end\tf\n", {"f"}, 1,
     ["lui\t$4,%hi(SYM_B+8)", "addiu\t$4,$4,%lo(SYM_B+8)"]),
    ("laa: maspsx DEBUG comment on the line is tolerated",
     "\t.ent\tf\nla\t$3,SYM_A # x\n\t.end\tf\n", {"f"}, 1,
     ["lui\t$3,%hi(SYM_A)", "addiu\t$3,$3,%lo(SYM_A)"]),
    ("laa: unnamed function untouched",
     "\t.ent\tg\nla\t$6,SYM_A\n\t.end\tg\n", {"f"}, 0, ["la\t$6,SYM_A"]),
    ("laa: scalar loads/stores are not touched",
     "\t.ent\tf\nsb\t$0,SYM_A\nlbu\t$2,SYM_A+1\n\t.end\tf\n", {"f"}, 0,
     ["sb\t$0,SYM_A", "lbu\t$2,SYM_A+1"]),
    ("laa: after a label (not a delay slot) still expands",
     "\t.ent\tf\nbeq\t$2,$0,$L1\nnop\n$L1:\nla\t$5,SYM_A\n\t.end\tf\n", {"f"}, 1,
     ["lui\t$5,%hi(SYM_A)", "addiu\t$5,$5,%lo(SYM_A)"]),
    ("laa: la in a branch delay slot refuses",
     "\t.ent\tf\nbeq\t$2,$0,$L1\nla\t$5,SYM_A\n$L1:\n\t.end\tf\n", {"f"}, 0, None),
]


def laa_selftest():
    bad = 0
    for name, asm, funcs, want_n, want in LAA_SELFTEST:
        try:
            out, n = la_absolute(asm, funcs)
        except ValueError:
            ok = want is None
            print(f"  [{'ok' if ok else 'FAIL'}] {name}"
                  + ("" if ok else "  (raised)"))
            bad += not ok
            continue
        body = [l.split("#")[0].strip() for l in out.split("\n")]
        ok = want is not None and n == want_n and all(w in body for w in want)
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (expanded {n}, want {want_n})"))
        bad += not ok
    return bad


#: reorder_fill: (name, asm, funcs, expected fills, expected rows — the
#: function's instructions as "mnemonic operands", in order)
RF_SELFTEST = [
    ("splits an la into a jal slot: lui stays, addiu moves",
     ".ent\tf\nla\t$4,D_8001342C\njal\tg\nnop  # DEBUG: branch/jump\n.end\tf",
     {"f"}, 1, ["lui $4,%hi(D_8001342C)", "jal g",
                "addiu $4,$4,%lo(D_8001342C)"]),
    ("refuses an la the branch reads",
     ".ent\tf\nla\t$3,D_800E7108\nbeq\t$3,$0,L1\nnop  # DEBUG: branch/jump\n"
     ".end\tf", {"f"}, 0, ["la $3,D_800E7108", "beq $3,$0,L1", "nop"]),
    ("refuses an la of a symbol declared small",
     "\t.extern\tS,4\n.ent\tf\nla\t$4,S\njal\tg\nnop  # DEBUG: branch/jump\n"
     ".end\tf", {"f"}, 0, ["la $4,S", "jal g", "nop"]),
    ("moves a volatile store into a j slot (DiskError's DataReady tail)",
     ".ent\tf\nlw\t$3,D_800B6628\nli\t$2,4\nsb\t$0,0($3)\nj\tL9\n"
     "nop  # DEBUG: branch/jump\n.end\tf", {"f"}, 1,
     ["lw $3,D_800B6628", "li $2,4", "j L9", "sb $0,0($3)"]),
    ("never steals a slot reorg already filled",
     ".ent\tf\nbne\t$4,$6,L2\naddu\t$3,$3,1\nj\tL9\nnop  # DEBUG: branch/jump\n"
     ".end\tf", {"f"}, 0, ["bne $4,$6,L2", "addu $3,$3,1", "j L9", "nop"]),
    ("leaves functions it was not given alone",
     ".ent\tf\nla\t$4,D_8001342C\njal\tg\nnop  # DEBUG: branch/jump\n.end\tf",
     {"h"}, 0, ["la $4,D_8001342C", "jal g", "nop"]),
]


def rf_selftest():
    bad = 0
    for name, asm, funcs, want_n, want_rows in RF_SELFTEST:
        out, n = reorder_fill(asm, funcs)
        rows = [f"{r[1]} {r[2]}".strip()
                for r in (_parse(l) for l in out.split("\n")) if r]
        ok = n == want_n and rows == want_rows
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (filled {n}, want {want_n}; "
                               f"rows {rows}, want {want_rows})"))
        bad += not ok
    return bad


def bmo_selftest():
    """The offset form of the bare far store, checked on the EMITTED TEXT:
    the slot mnemonic is `sh` whether the macro was split or moved whole."""
    far = "lhu\t$2,10($18)\nsh\t$2,SYM+6\nj\t$L57\nnop  # DEBUG: branch/jump\n"
    cases = [
        ("fill splits `sh $2,SYM+6` before j: lui $at stays, %lo store in the slot",
         fill(far)[0],
         "lui\t$at,%hi(SYM+6)\nj\t$L57\nsh\t$2,%lo(SYM+6)($at)"),
        ("fill moves `sh $2,SYM+6` whole when SYM is small data (one gp insn)",
         fill("\t.extern\tSYM,8\n" + far)[0],
         "lhu\t$2,10($18)\nj\t$L57\nsh\t$2,SYM+6"),
        ("return_slot_fill splits `sh $2,SYM+6` before jr",
         return_slot_fill("f:\n\tsh\t$2,SYM+6\n\tjr\t$ra\n\tnop\n", {"f"})[0],
         "\tlui\t$at,%hi(SYM+6)\n\tjr\t$ra\n\tsh\t$2,%lo(SYM+6)($at)"),
        ("return_slot_fill moves it whole when SYM is small data",
         return_slot_fill("\t.extern\tSYM,8\nf:\n\tsh\t$2,SYM+6\n\tjr\t$ra\n"
                          "\tnop\n", {"f"})[0],
         "\tjr\t$ra\n\tsh\t$2,SYM+6"),
    ]
    bad = 0
    for name, out, want in cases:
        ok = want in out
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"\n      got: {out!r}"))
        bad += not ok
    return bad


def selftest():
    bad = (splice_selftest() + zr_selftest() + lhn_selftest() + nbl_selftest()
           + laa_selftest() + rf_selftest() + bmo_selftest() + dn_selftest())
    for name, asm, funcs, want_n, want_slot in RET_SELFTEST:
        out, n = return_slot_fill(asm, funcs)
        rows = [r for r in (_parse(l) for l in out.split("\n")) if r]
        bi = next((k for k, r in enumerate(rows) if r[1] in BRANCH), None)
        got = rows[bi + 1][1] if bi is not None and bi + 1 < len(rows) else "?"
        ok = n == want_n and got == want_slot
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (filled {n}, want {want_n}; "
                               f"slot {got}, want {want_slot})"))
        bad += not ok
    for name, asm, want_n, want_slot in SELFTEST:
        out, n = fill(asm)
        rows = [r for r in (_parse(l) for l in out.split("\n")) if r]
        bi = next((k for k, r in enumerate(rows) if r[1] in BRANCH), None)
        got_slot = rows[bi + 1][1] if bi is not None and bi + 1 < len(rows) else "?"
        ok = n == want_n and got_slot == want_slot
        print(f"  [{'ok' if ok else 'FAIL'}] {name}"
              + ("" if ok else f"  (filled {n}, want {want_n}; "
                               f"slot {got_slot}, want {want_slot})"))
        bad += not ok
    print("delayfill selftest:", "all pass" if not bad else f"{bad} FAILED")
    return bad


if __name__ == "__main__":
    if "--selftest" in sys.argv:
        sys.exit(1 if selftest() else 0)
    out, n = fill(sys.stdin.read())
    sys.stderr.write(f"delayfill: filled {n} delay slot(s)\n")
    sys.stdout.write(out)
